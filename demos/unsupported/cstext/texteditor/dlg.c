/* 
 * Motif
 *
 * Copyright (c) 1987-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2.1 of the License, or (at your option)
 * any later version.
 *
 * These libraries and programs are distributed in the hope that
 * they will be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
*/ 
/* 
 * HISTORY
*/ 
#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$XConsortium: dlg.c /main/6 1995/07/14 09:43:54 drk $"
#endif
#endif

/************************************************************
 *     dlg.c -- toolkit-independent dialogue layer
 *
 *  The code in this file represents the toolkit-independent interface
 *  layer, and determines how the interface works.  This code
 *  attempts to be independent of any specific toolkit, as well as
 *  independent of the actual widgets used.  For example, by simply changing
 *  the Tk layer, it should be possible to obtain file names through
 *  a file selection box, a separate file manager, or through a
 *  command line interface.
 *
 *  When the user interacts with the application, the toolkit-specific
 *  layer Tk will generally make an upcall to a callback routine in this
 *  layer.  This code decides what to do, and then makes calls to the
 *  Tk layer to direct the interface (in abstract terms) to take appropriate
 *  actions.  This layer also calls the interface-independent application
 *  layer App to perform computations or store or retrieve data,
 *
 ************************************************************/

#include <Xmd/Print.h>

#include "basic.h"
#include "tk.h"
#include "app.h"
#include "file.h"

  /* Remembers file provided by file selection dialogue */

static char fileSelected[128];

  /* Keeps track of whether a file dialogue (if one is used)
     remains available after a file is selected */

static int keep_file_dialogue = false;

  /* Keeps track of whether the default operation performed on
     a selected file should revert to Open after a file is selected */

static int revert_to_open = true;

  /* When the user initiates some action, the user is often first
     asked whether the current file should be saved.  If the
     user answers Yes or No (instead of Cancel), the action is
     initiated (after saving the File in the case of Yes).
     Since this may happen in many different circumstances,
     the continuation pointer keeps track of the function to be
     called to initiate that action. */

static void (*continuation)();

/*===========================================================
                 Support Functions
============================================================*/

/************************************************************
 * Update Status
 ************************************************************/

      /* Tell the interface to display the current file name
         surrounded by asterisks if it is modified */

static void 
DlgUpdateStatus(char *nam, int mod)
{
    static char curnam[128] = "";
    char status[128];

    if ( nam != NULL )
        strcpy( curnam, nam );

    if ( mod ) {
        strcpy( status, "* " );
        if ( strlen( curnam ) > 0 ) {
            strcat( status, curnam );
            strcat( status, " *" );
         }
        TkUpdateStatus( status );
    }
    else
        TkUpdateStatus( curnam );
}

/************************************************************
 * Finalize file dialogue
 ************************************************************/

static void DlgFinalizeFileDialogue()
{
    if ( ! keep_file_dialogue )
        TkDoneAskingFile();
    else if ( revert_to_open )
        TkArrangeToOpen();
}

/************************************************************
 * Clear File
 ************************************************************/

static void DlgClear()
{
    TkTextClear();
    AppNewFile();
    DlgUpdateStatus( "", false );
}

/************************************************************
 * Open Selected File
 ************************************************************/

static void DlgOpen()
{
    char *txt;
    char *bufnam;

    if ( AppOpenReadFile( fileSelected ) ) {
        txt = AppReadFile();
        TkTextStore( txt );
	BasicFree( txt );
	bufnam = AppBufferName();
	DlgUpdateStatus( bufnam, false );
        BasicFree( bufnam );
        DlgFinalizeFileDialogue();
    }
    else
        TkWarn( warn_open );
}

/************************************************************
 * Save Current File
 ************************************************************/

static void DlgSave()
{
    char *txt;
    size_t numchars;
    
    if ( AppOpenSaveFile() ) {
      numchars = TkTextRetrieve(&txt, TkTextFormat());
	AppSaveFile(txt, numchars);
	BasicFree( txt );
	DlgUpdateStatus( NULL, false );
    }
    else
	TkWarnAndAskFileToSave( warn_save );
}

/************************************************************
 * Save Current File and Continue
 ************************************************************/

static void DlgSaveAndContinue()
{
    char *txt;
    size_t numchars;
    
    TkDoneAskingSave();
    if ( AppOpenSaveFile() ) {
      numchars = TkTextRetrieve(&txt, TkTextFormat());
	AppSaveFile(txt, numchars);
	BasicFree( txt );
	DlgUpdateStatus( NULL, false );
        (*continuation)();
    }
    else
        TkWarn( warn_save );
}

/************************************************************
 * Save As Selected File
 ************************************************************/

static void DlgSaveAs()
{
    char *txt;
    char *bufnam;
    size_t numchars;
    
      /* Note: DlgSaveAs, DlgCopyTo, and DlgMoveTo should be fixed to
         request confirmation if the destination files already exist */

    if ( AppOpenTransferFile( fileSelected ) ) {
        numchars = TkTextRetrieve(&txt, TkTextFormat());
        AppTransferFile(txt, numchars);
	BasicFree( txt );
        AppCompleteSaveAsFile();
	bufnam = AppBufferName();
	DlgUpdateStatus( bufnam, false );
        BasicFree( bufnam );
        DlgFinalizeFileDialogue();
    }
    else
        TkWarn( warn_write );
}

/************************************************************
 * Copy To Selected File
 ************************************************************/

void DlgCopyTo()
{
    char *txt;
    size_t numchars;
    
    if ( AppOpenTransferFile( fileSelected ) ) {
      numchars = TkTextRetrieve(&txt, TkTextFormat());
        AppTransferFile(txt, numchars);
	BasicFree( txt );
        DlgFinalizeFileDialogue();
        AppCompleteCopyFile();
    }
    else
        TkWarn( warn_write );
}

/************************************************************
 * Move To Selected File
 ************************************************************/

void DlgMoveTo()
{
    char *txt;
    char *bufnam;
    int result;
    size_t numchars;
    
    if ( AppOpenTransferFile( fileSelected ) ) {
      numchars = TkTextRetrieve(&txt, TkTextFormat());
        AppTransferFile(txt, numchars);
	BasicFree( txt );
        result = AppCompleteMoveFile();
	bufnam = AppBufferName();
	DlgUpdateStatus( bufnam, false );
        BasicFree( bufnam );
        DlgFinalizeFileDialogue();
        if (! result)
            TkWarn( warn_remove );
    }
    else
        TkWarn( warn_write );
}

/************************************************************
 * Remove Current File
 ************************************************************/

void DlgRemove()
{
    int result;
    result = AppRemoveFile();
    DlgClear();
    if ( ! result )
        TkWarn( warn_remove );
}

/*===========================================================
                   Callbacks
============================================================*/

/************************************************************
 * User specified if file selection dialogue should 
 * remain available
 ************************************************************/

void 
DlgKeepFileDialogueCB(int val)
{
    keep_file_dialogue = val;
}

/************************************************************
 * User specified if dialogue should revert to Open
 * after selecting a file
 ************************************************************/

void 
DlgRevertToOpenCB(int val)
{
    revert_to_open = val;
}

/************************************************************
 * User's first modification of current file since saving it
 ************************************************************/

void DlgNoteJustChangedCB()
{
    DlgUpdateStatus( NULL, true );
}

/************************************************************
 * User's first modification of current file since saving it 
 * or refusing to save it when asked
 ************************************************************/

void DlgNoteJustChangedSinceCB()
{
}

/************************************************************
 * User agreed to save current modified file when asked
 ************************************************************/

void DlgSaveYesCB()
{
    DlgSaveAndContinue();
}

/************************************************************
 * User declined to save current modified file when asked
 ************************************************************/

void DlgSaveNoCB()
{
    TkDoneAskingSave();
    TkTextActUnchangedSince();
    (*continuation)();
}

/************************************************************
 * User cancelled operation when asked if file should be saved
 ************************************************************/

void DlgSaveCancelCB()
{
    TkDoneAskingSave();
    DlgFinalizeFileDialogue();
}

/************************************************************
 * User cancelled operation when asked if file should be saved
 ************************************************************/

void 
DlgWarnCancelCB(enum warn_reasons reason)
{
    if ( reason != warn_remove )
        DlgFinalizeFileDialogue();
}

/************************************************************
 * User specified a file to open
 ************************************************************/

void 
DlgSelectOpenCB(char *filnam)
{
    strcpy( fileSelected, filnam );

      /* If current file has been changed, ask if it should
         be saved.  Set up continuation so that selected
         file is opened after uncancelled save dialogue */

    if ( TkTextChangedSince() ) {
        continuation = DlgOpen;
        TkAskSave();
	}
    else DlgOpen();
}

/************************************************************
 * User specified a file to save as
 ************************************************************/

void 
DlgSelectSaveCB(char *filnam)
{
    strcpy( fileSelected, filnam );
    DlgSaveAs();
}

/************************************************************
 * User specified a file to copy to
 ************************************************************/

void 
DlgSelectCopyCB(char *filnam)
{
    strcpy( fileSelected, filnam );
    DlgCopyTo();
}

/************************************************************
 * User selected a file to move to
 ************************************************************/

void 
DlgSelectMoveCB(char *filnam)
{
    strcpy( fileSelected, filnam );
    DlgMoveTo();
}

/************************************************************
 * User cancelled file selection
 ************************************************************/

void DlgSelectCancelCB( )
{
    TkDoneAskingFile();
}

/************************************************************
 * User wants to clear the workspace
 ************************************************************/

void DlgWantClearCB()
{
      /* If current file has been changed, ask if it should
         be saved.  Set up continuation so that workspace
         is cleared after uncancelled save dialogue */

    if ( TkTextChanged() ) {
        continuation = DlgClear;
        TkAskSave();
	}
    else DlgClear();
}

/************************************************************
 * User wants to print a file
 ************************************************************/


void
DlgPrintCB(Widget w, char *ignore, XmdPrintCallbackStruct *cb)
{
  char *tmp_nam;
  char *txt;
  
  TkTextRetrieve(&txt, TkTEXT_ONLY);
  tmp_nam = FileSaveTemp(txt);
  XmdPrintDocument(tmp_nam, cb);
  remove(tmp_nam);
}

/************************************************************
 * User wants to open a file
 ************************************************************/

void DlgWantOpenCB()
{
      /* If current file has been changed, ask if it should
         be saved.  Set up continuation so that user is
         asked for a file to open after uncancelled save dialogue */

    if ( TkTextChanged() ) {
        continuation = TkAskFileToOpen;
        TkAskSave();
	}
    else TkAskFileToOpen();
}

/************************************************************
 * User wants to save current file as modifed under a new name
 ************************************************************/

void DlgWantSaveAsCB()
{
    TkAskFileToSave();
}

/************************************************************
 * User wants to save current file as modified
 ************************************************************/

void DlgWantSaveCB()
{
    DlgSave();
}

/************************************************************
 * User wants to store modified version of file under a new
 * name, but to continue editing the original file
 ************************************************************/

void DlgWantCopyCB()
{
    TkAskFileToCopy();
}

/************************************************************
 * User wants to save current file as modified under a new name
 * and remove the original file
 ************************************************************/

void DlgWantMoveCB()
{
    TkAskFileToMove();
}

/************************************************************
 * User wants to remove the current file
 ************************************************************/

void DlgWantRemoveCB()
{
    TkQuestionRemove();
}

/************************************************************
 * User confirmed removal of the current file
 ************************************************************/

void DlgQuestionYesCB()
{
    DlgRemove();
}

/************************************************************
 * Used requested exit
 ************************************************************/

void DlgExitCB()
{

      /* If current file has been changed, ask if it should
         be saved.  Set up continuation so that application
         exits after uncancelled save dialogue */

    if ( TkTextChangedSince() ) {
        continuation = TkExit;
        TkAskSave();
	}
    else TkExit();
}

