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
/*   $XConsortium: dlg.h /main/6 1995/07/14 09:44:04 drk $ */

/************************************************************
 *     dlg.h -- toolkit-independent dialogue layer
 ************************************************************/

#ifndef _dlg_h
#define _dlg_h

#include <Xmd/Print.h>

#include "tkdef.h"


extern void DlgKeepFileDialogueCB(int val);
extern void DlgRevertToOpenCB(int val);
extern void DlgNoteJustChangedCB();
extern void DlgNoteJustChangedSinceCB();

extern void DlgSelectOpenCB(char *filnam);
extern void DlgSelectSaveCB(char *filnam);
extern void DlgSelectCopyCB(char *filnam);
extern void DlgSelectMoveCB(char *filnam);

extern void DlgSelectCancelCB();

extern void DlgSaveYesCB();
extern void DlgSaveNoCB();
extern void DlgSaveCancelCB();
extern void DlgWarnCancelCB(enum warn_reasons reason);
extern void DlgQuestionYesCB();

extern void DlgPrintCB(Widget w, char *ignore, XmdPrintCallbackStruct *cb);

extern void DlgWantClearCB();
extern void DlgWantOpenCB();
extern void DlgWantSaveAsCB();
extern void DlgWantSaveCB();
extern void DlgWantCopyCB();
extern void DlgWantMoveCB();
extern void DlgWantRemoveCB();

extern void DlgExitCB();


#endif /* _dlg_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
