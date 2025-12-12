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
static char rcsid[] = "$XConsortium: tk.c /main/6 1995/07/14 09:45:11 drk $"
#endif
#endif
/************************************************************
 *     tk.c -- toolkit-specific dialogue layer
 *
 *  The code in this file specifically interacts with the
 *  OSF/Motif toolkit.  
 *
 *  The first half of the file provides utility routines and a
 *  toolkit-independent (but application-specific) interface that can
 *  be used by the toolkit-independent dialogue layer of the application
 *
 *  The second half of the file defines all the callback routines.
 *  Many of these simply update state internal to this layer;
 *  others perform additional actions.  Where those actions
 *  unambiguously correspond to toolkit-specific action, routines
 *  defined in the first half of the file are called.
 *  Otherwise, an upcall is made to the toolkit-independent dialogue
 *  layer to decide what to do.
 *
 ************************************************************/

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>

#include <Xm/Xm.h>
#include <Xm/XmP.h>				  /* For XmeRenderTableGetDefaultFont */
#include <Xm/Text.h>
#include <Xm/ToggleB.h>
#include <Xm/RowColumn.h>
#include <Xm/PanedW.h>
#include <Xm/MessageB.h>
#include <Xm/FileSB.h>
#include <Xm/CSText.h>

#include <Mrm/MrmPublic.h>

#include <Xmd/Help.h>

#include "basic.h"
#include "tkdef.h"
#include "dlg.h"

#define BOLD_FONT_NAME "*helvetica-bold-r-*-12-*"
#define EMPHASIS_FONT_NAME "*helvetica-medium-o-*-12-*"
#define HEADING_FONT_NAME "*helvetica-bold-r-*-14-*"
#define TITLE_FONT_NAME "*helvetica-bold-o-*-14-*"

#define MAX_ARGS 10
#define MAX_ATTR 20
#define MAX_CHAR 20

extern Widget DemoTextPaneCreate();

/* imported strings */

char *appString;
XmString openFileString, openString, saveFileString, saveString,
         copyFileString, copyString, moveFileString, moveString,
         warnOpenString, warnWriteString, warnRemoveString;

/* globals for callbacks */

Display *dpy;
Widget toplevel, panedWindow, textStore, textFirst;
Widget fileDialog, saveDialog, warnDialog, questionDialog;
Widget helpDialog, printDialog;

   /* The File Selection Box contains a radio box which allows the user
        to select whether the file chosen is to be opened, or to be
        the destination of a save, move, or copy operation.
      openToggle, saveToggle, copyToggle, moveToggle are the toggle
        buttons; curToggle indicates which is currently selected.
      The currently selected toggle is also reflected in the title
        of the FSB, and in the label of its "ok" button. */

Widget openToggle, saveToggle, copyToggle, moveToggle;
Widget curToggle = NULL;

/* The File Selection Box contains an option menu which controls the
   format for reading and writing files. */
Widget formatMenu;

   /* statics for callbacks */

static XmString xms;
static Arg args[MAX_ARGS];
static int n;

static Cardinal tk_font_count = 0;
static char *tk_fonts[MAX_ATTR];
static Cardinal tk_color_count = 0;
static char *tk_colors[MAX_ATTR];
static Cardinal tk_thru_count = 2;
static char *tk_thru[] = {"Strikethru0", "Strikethru1"};
static Cardinal tk_under_count = 2;
static char *tk_under[] = {"Underline0", "Underline1"};

   /* keep track of why a warning was raised */

static enum warn_reasons warn_reason;

   /* "textchanged" and "textchangedsince" indicate whether the text
        has been modified.
      "textchanged" is reset whenever the text is read in or written out.
      "textchangedsince" can also be explicitly reset, typically because
        a decision has been made not to save the modified text. */

static int textchanged = 0;
static int textchangedsince = 0;

   /* These four variables keep track of aspects of the various text widgets.
        textCurPrimary -- which one has the primary selection (if any)
        textCurFocus -- which currently has the focus (if any)
        textLastFocus -- which last had the focus (if any)
        textLastDestination -- which one the user selected or edited
      When an action is chosen from a menu which could potentially
        apply to any of the widgets, these determine which widget
        to use.
      textCurPrimary is used when Cut or Copy is chosen from the Edit
        Menu; the selection is cut or copied from the text widget which
        has the primary selection.
      The other variable are used when Paste is chosen from the Edit Menu,
        or when operations on panes are performed (e.g. Remove Pane).
        See TkPaneTarget for the logic as mandated by the Motif Style Guide. */

static Widget textCurFocus = NULL;
static Widget textLastFocus = NULL;
static Widget textLastDestination = NULL;
static Widget textCurPrimary = NULL;

  /* Remembers if an explicit or implicit focus policy is in use */

static unsigned char policy;

static void TkSetSelectedText();
static XmString TkSetFont();
static XmString TkSetColor();
static XmString TkGetComp();
static XmString TkSetTextAttribute();
static Pixel TkGetColor();
static XmString TkSetStrikethru();
static XmString TkSetUnderline();


/*===========================================================
                 Support Functions
============================================================*/

/************************************************************
 * Beep
 ************************************************************/

void
TkBeep()
{
    XBell( dpy, 0 );
}

/************************************************************
 * Exit
 ************************************************************/

void
TkExit()
{
    exit(0);
}

/*===========================================================
                 Window Title
============================================================*/

/************************************************************
 * Display the application name plus "str" as the window title.
 ************************************************************/

  /* Note: this code should be fixed to be locale-independent */

void
TkUpdateStatus(char *str)
{
    char title[128];
    
    strcpy( title, appString );
    if ( strlen(str) > 0 ) {
        strcat( title, ": " );
        strcat( title, str );
    }

    n = 0;
    XtSetArg( args[n], XmNtitle, title ); n++;
    XtSetValues( toplevel, args, n );
}

/*===========================================================
                 Text Functions
============================================================*/

/************************************************************
 * Clear XmText's XmNvalue
 ************************************************************/

void TkTextClear()
{
  static XmString empty = NULL;
  
  if (empty == NULL)
    empty = XmStringComponentCreate(XmSTRING_COMPONENT_END, 0, NULL);
  
     n = 0;
     XtSetArg( args[n], XmNcstextValue, empty); n++;
     XtSetValues( textStore, args, n );
     textchanged = 0;
     textchangedsince = 0;
}

static XmIncludeStatus 
parse_proc(XtPointer      *in_out,
	   XtPointer       text_end,
	   XmTextType      type,
	   XmStringTag     tag,
	   XmParseMapping  entry,
	   int		   pattern_length,
	   XmString       *str_include,
	   XtPointer       call_data)
{
  char *rendition_name;
  Boolean start = True;
  char *ptr;
  int len;

  /* The remaining characters are either rendition or /rendition to
     indicate the start or end of a rendition */

  ptr = (char*) *in_out;
  ptr++;

  if (*ptr == '/') {
    start = False;
    ptr++;
  }

  /* Get rendition name.  The name ends at the > delimiter */
  rendition_name = ptr;
  ptr = strchr(ptr, '>');
  if (ptr == NULL) {
    str_include = NULL;
    return XmINSERT;
  }

  /* Null terminate tag */
  *ptr = 0;
  ptr++;
  rendition_name = XtNewString(rendition_name);
  len = strlen(rendition_name);

  *in_out = (XtPointer) ptr;
  if (start)
    *str_include =
      XmStringComponentCreate(XmSTRING_COMPONENT_RENDITION_BEGIN, len, 
			      rendition_name);
  else
    *str_include =
      XmStringComponentCreate(XmSTRING_COMPONENT_RENDITION_END, len, 
			      rendition_name);
  XtFree(rendition_name);
  return XmINSERT;
}

static XmIncludeStatus 
esc_parse_proc(XtPointer      *in_out,
	       XtPointer       text_end,
	       XmTextType      type,
	       XmStringTag     tag,
	       XmParseMapping  entry,
	       int	       pattern_length,
	       XmString       *str_include,
	       XtPointer       call_data)
{
  char *ptr;
  char temp[2];

  ptr = (char*) *in_out;
  ptr++;

  /* Move to the next character and create an XmString for it */
  temp[0] = *ptr++;
  temp[1] = 0;
  *str_include = XmStringCreateLocalized(temp);

  *in_out = (XtPointer) ptr;

  return XmINSERT;
}

/************************************************************
 * Get the parse table for the Help markup language.
 ************************************************************/
static int
TkHelpMarkupTable(XmParseTable *table_return)
{
  static XmParseTable table = NULL;
  int table_size = 4;
  
  if (table == NULL)
    {
      Arg args[10];
      Cardinal nargs;
      XmString tmp;
      int index = 0;

      table = (XmParseTable) XtCalloc(table_size, sizeof(XmParseMapping));

      tmp = XmStringComponentCreate(XmSTRING_COMPONENT_TAB, 0, NULL);
      nargs = 0;
      XtSetArg(args[nargs], XmNincludeStatus, XmINSERT), nargs++;
      XtSetArg(args[nargs], XmNsubstitute, tmp), 	 nargs++;
      XtSetArg(args[nargs], XmNpattern, "\t"), 		 nargs++;
      table[index++] = XmParseMappingCreate(args, nargs);
      XmStringFree(tmp);

      tmp = XmStringSeparatorCreate();
      nargs = 0;
      XtSetArg(args[nargs], XmNincludeStatus, XmINSERT), nargs++;
      XtSetArg(args[nargs], XmNsubstitute, tmp),	 nargs++;
      XtSetArg(args[nargs], XmNpattern, "\n"),		 nargs++;
      table[index++] = XmParseMappingCreate(args, nargs);

      nargs = 0;
      XtSetArg(args[nargs], XmNincludeStatus, XmINVOKE), nargs++;
      XtSetArg(args[nargs], XmNpattern, "\\"),		 nargs++;
      XtSetArg(args[nargs], XmNinvokeParseProc, esc_parse_proc), nargs++;
      table[index++] = XmParseMappingCreate(args, nargs);

      nargs = 0;
      XtSetArg(args[nargs], XmNpattern, "<"), 	 	     nargs++;
      XtSetArg(args[nargs], XmNincludeStatus, XmINVOKE),     nargs++;
      XtSetArg(args[nargs], XmNinvokeParseProc, parse_proc), nargs++;
      table[index++] = XmParseMappingCreate(args, nargs);
      XmStringFree(tmp);
    }
  
  *table_return = table;
  return(table_size);
}

/************************************************************
 * Store "txt" as XmCSText's XmNcstextValue
 ************************************************************/

void
TkTextStore(char *txt)
{
  XmString 	val;
  Widget 	formatButton;
  Cardinal 	format;
  int table_size;
  XmParseTable table;
  
  XtVaGetValues(formatMenu, XmNmenuHistory, &formatButton, NULL);
  XtVaGetValues(formatButton, XmNuserData, &format, NULL);
  
  switch (format)
    {
    case TkTEXT_ONLY:
      val = XmStringGenerate(txt, NULL, XmCHARSET_TEXT, NULL);
      break;
      
    case TkBYTE_STREAM:
      val = XmCvtByteStreamToXmString((unsigned char *)txt);
      break;
      
    case TkHELP_MARKUP:
      table_size = TkHelpMarkupTable(&table);
      val = XmStringParseText((XtPointer)txt, NULL, NULL, XmCHARSET_TEXT,
			      table, table_size, NULL);
      break;
			      
    default:
      val = NULL;
      break;
    }

  XtVaSetValues(textStore, XmNcstextValue, val, NULL);
  textchanged = 0;
  textchangedsince = 0;
}

/************************************************************
 * Get the preferred format for reading and writing
 ************************************************************/

int
TkTextFormat()
{
  Widget 	formatButton;
  Cardinal 	format;

  XtVaGetValues(formatMenu, XmNmenuHistory, &formatButton, NULL);
  XtVaGetValues(formatButton, XmNuserData, &format, NULL);
  return(format);
}

/************************************************************
 * Get XmCSText's XmNcstextValue and convert as necessary
 ************************************************************/

size_t
TkTextRetrieve(char **txt, int format)
{
  XmString val;
  int table_size = 2;
  static XmParseTable table = NULL;
  size_t numchars;
  
  if (table == NULL)
    {
      Arg args[10];
      Cardinal nargs;
      XmString tmp;
      int index = 0;

      table = (XmParseTable)XtCalloc(table_size, sizeof(XmParseMapping));
	 
      /* Parse tab characters. */
      tmp = XmStringComponentCreate(XmSTRING_COMPONENT_TAB, 0, NULL);
      nargs = 0;
      XtSetArg(args[nargs], XmNincludeStatus, XmINSERT), nargs++;
      XtSetArg(args[nargs], XmNsubstitute, tmp), 	 nargs++;
      XtSetArg(args[nargs], XmNpattern, "\t"), 		 nargs++;
      table[index] = XmParseMappingCreate(args, nargs);
      index++;
      XmStringFree(tmp);

      /* Parse newline characters. */
      tmp = XmStringSeparatorCreate();
      nargs = 0;
      XtSetArg(args[nargs], XmNincludeStatus, XmINSERT), nargs++;
      XtSetArg(args[nargs], XmNsubstitute, tmp),	 nargs++;
      XtSetArg(args[nargs], XmNpattern, "\n"),		 nargs++;
      table[index] = XmParseMappingCreate(args, nargs);
      index++;
    }
     
  XtVaGetValues(textStore, XmNcstextValue, &val, NULL);

  switch (format)
    {
    case TkTEXT_ONLY:
      *txt = XmStringUnparse(val, NULL, XmCHARSET_TEXT, XmCHARSET_TEXT, 
			    table, table_size, XmOUTPUT_ALL);
      numchars = strlen(*txt);
      break;
      
    case TkBYTE_STREAM:
      numchars = (size_t)XmCvtXmStringToByteStream(val, (unsigned char **)txt);
      break;
      
    default:
      txt = NULL;
      numchars = 0;
      break;
    }
     
  textchanged = 0;
  textchangedsince = 0;
  return numchars;
}

/************************************************************
 * Has Text Changed? 
 ************************************************************/

Boolean TkTextChanged()
{
    return ( textchanged > 0 );
}

/************************************************************
 * Set "Since" point for text change
 ************************************************************/

void TkTextActUnchangedSince()
{
    textchangedsince = 0;
}

/************************************************************
 * Has Text Changed Since?
 ************************************************************/

Boolean TkTextChangedSince()
{
    return ( textchangedsince > 0 );
}

/*===========================================================
                     Panes
============================================================*/

#define MAX_PANES 20

static Widget panes[MAX_PANES];
static int nextpane = 1;

/************************************************************
 * Initialize Paned Window support
 ************************************************************/

static void TkPaneInit()
{
    panes[0] = textFirst;

    n = 0;
    XtSetArg( args[n], XmNkeyboardFocusPolicy, &policy ); n++;
    XtGetValues( toplevel, args, n );
}

/************************************************************
 * Get Pane Target
 ************************************************************/

   /* Determines which pane to use when an action is chosen from a menu
        which could potentially apply to any of them, as mandated by the
        Motif Style Guide.
      When an explicit focus policy is being used, the applicable pane
        is the one which has, or last had focus.
      When an implicit focus policy is being used, and the pointer is
        in one of the panes (which means that the menu action is invoked by
        typing an accelerator), the applicable pane is the one under the
        pointer.
      When an implicit focus policy is being used, but the pointer not
        in one of the panes, the applicable pane is the one which the user
        last selected or edited. */

static Widget TkPaneTarget()
{
    Widget w;

    if (nextpane == 1)  return panes[0];

    if ( policy == XmEXPLICIT )
        w = textLastFocus;
    else if ( textCurFocus != NULL ) /* when an accelerator is used */
        w = textCurFocus;
    else
        w = textLastDestination;

    return w;
}

/************************************************************
 * Return index of the pane which holds Text Widget "w"
 ************************************************************/

static int
TkPaneForText(Widget w)
{
    int findpane = 0;

    while ( panes[findpane] != w )
        findpane++;

    return findpane;
}

/************************************************************
 * Delete Text Pane
 ************************************************************/

static void
TkPaneDelete(Widget w)
{
    XtDestroyWidget( w );
    if ( w == textCurFocus ) textCurFocus = NULL;
    if ( w == textLastFocus ) textLastFocus = NULL;
    if ( w == textCurPrimary ) textCurPrimary = NULL;
    if ( w == textLastDestination ) textLastDestination = NULL;
}

/************************************************************
 * Append a new Text Pane
 ************************************************************/

static void TkPaneAppend()
{
    short ht;

    if ( nextpane == MAX_PANES ) {
        TkBeep();
        return;
    }    

      /* Set the new pane's height so it will be 90%
         of the average pane height */

    n = 0;
    XtSetArg( args[n], XmNheight, &ht ); n++;
    XtGetValues( panedWindow, args, n );

    ht = ( 9 * ht ) / ( 10 * ( nextpane + 1 ) );

    panes[nextpane] = DemoTextPaneCreate( ht );
    nextpane++;
}

/************************************************************
 * Remove the target Text Pane
 ************************************************************/

static void TkPaneRemove()
{
    Widget w;
    int findpane;

    if ( nextpane == 1 ) {
        TkBeep();
        return;
    }    

    nextpane--;
    w = TkPaneTarget();
    if ( w == NULL )
        findpane = nextpane;
    else
        findpane = TkPaneForText( w );

    TkPaneDelete( panes[findpane] );

    while ( findpane < nextpane ) {
        panes[findpane] = panes[findpane+1];
        findpane++;
    }
}

/************************************************************
 * Remove all panes but the target Text Pane
 ************************************************************/

static void TkPaneOne()
{
    Widget w;
    int findpane;

    if ( nextpane == 1 ) {
        TkBeep();
        return;
    }    

    w = TkPaneTarget();
    if ( w == NULL )
        findpane = 0;
    else
        findpane = TkPaneForText( w );

    if ( findpane > 0 ) {
        w = panes[0];
        panes[0] = panes[findpane];
        panes[findpane] = w;
    }

    while ( nextpane > 1 ) {
        nextpane--;
        TkPaneDelete( panes[nextpane] );
    }
}

/*===========================================================
                 FSB Functions
============================================================*/

/************************************************************
 * Arrange FSB to Open
 ************************************************************/

void TkArrangeToOpen()
{
    if ( curToggle == openToggle ) return;

    n = 0;
    XtSetArg( args[n], XmNdialogTitle, openFileString ); n++;
    XtSetValues( fileDialog, args, n );

    n = 0;
    XtSetArg( args[n], XmNokLabelString, openString ); n++;
    XtSetValues( fileDialog, args, n );

    curToggle = openToggle;
    XmToggleButtonSetState( curToggle, 1, true );
}

/************************************************************
 * Arrange FSB to Save
 ************************************************************/

static void TkArrangeToSave()
{
    if ( curToggle == saveToggle ) return;

    n = 0;
    XtSetArg( args[n], XmNdialogTitle, saveFileString ); n++;
    XtSetValues( fileDialog, args, n );

    n = 0;
    XtSetArg( args[n], XmNokLabelString, saveString ); n++;
    XtSetValues( fileDialog, args, n );

    curToggle = saveToggle;
    XmToggleButtonSetState( curToggle, 1, true );
}

/************************************************************
 * Arrange FSB to Copy
 ************************************************************/

static void TkArrangeToCopy()
{
    if ( curToggle == copyToggle ) return;

    n = 0;
    XtSetArg( args[n], XmNdialogTitle, copyFileString ); n++;
    XtSetValues( fileDialog, args, n );

    n = 0;
    XtSetArg( args[n], XmNokLabelString, copyString ); n++;
    XtSetValues( fileDialog, args, n );

    curToggle = copyToggle;
    XmToggleButtonSetState( curToggle, 1, true );
}

/************************************************************
 * Arrange FSB to Move
 ************************************************************/

static void TkArrangeToMove()
{
    if ( curToggle == moveToggle ) return;

    n = 0;
    XtSetArg( args[n], XmNdialogTitle, moveFileString ); n++;
    XtSetValues( fileDialog, args, n );

    n = 0;
    XtSetArg( args[n], XmNokLabelString, moveString ); n++;
    XtSetValues( fileDialog, args, n );

    curToggle = moveToggle;
    XmToggleButtonSetState( curToggle, 1, true );
}

/************************************************************
 * Post FSB for Opening
 ************************************************************/

void TkAskFileToOpen()
{
    TkArrangeToOpen();
    XtManageChild( fileDialog );
}

/************************************************************
 * Post FSB for Saving
 ************************************************************/

void TkAskFileToSave()
{
    TkArrangeToSave();
    XtManageChild( fileDialog );
}

/************************************************************
 * Post FSB for Copying
 ************************************************************/

void TkAskFileToCopy()
{
    TkArrangeToCopy();
    XtManageChild( fileDialog );
}

/************************************************************
 * Post FSB for Moving
 ************************************************************/

void TkAskFileToMove()
{
    TkArrangeToMove();
    XtManageChild( fileDialog );
}

/************************************************************
 * Unpost file selection box
 ************************************************************/

void TkDoneAskingFile()
{
    XtUnmanageChild( fileDialog );
}

/************************************************************
 * Get Filename selected in FSB
 ************************************************************/

static char *TkGetFilename()
{
    char *str;

    n = 0;
    XtSetArg( args[n], XmNtextString, &xms ); n++;
    XtGetValues( fileDialog, args, n );
    XmStringGetLtoR( xms, XmSTRING_DEFAULT_CHARSET, &str );
    return str;
}

/*===========================================================
                 Warn & Question Dialog
============================================================*/

/************************************************************
 * Warn couldn't open
 ************************************************************/

void
TkWarn(enum warn_reasons reason)
{
    TkBeep();
    warn_reason = reason;
    n = 0;
    switch ( reason )
    {
    case warn_open:
        XtSetArg( args[n], XmNmessageString, warnOpenString ); n++;
	break;
    case warn_write:
        XtSetArg( args[n], XmNmessageString, warnWriteString ); n++;
	break;
    case warn_save:
        XtSetArg( args[n], XmNmessageString, warnWriteString ); n++;
	break;
    case warn_remove:
        XtSetArg( args[n], XmNmessageString, warnRemoveString ); n++;
	break;
    }
    XtSetValues( warnDialog, args, n );
    XtManageChild( warnDialog );
}

/************************************************************
 * Post FSB for Saving, and warn
 ************************************************************/

void
TkWarnAndAskFileToSave(enum warn_reasons reason)
{
    TkAskFileToSave();
    TkWarn( reason );
}

/************************************************************
 * Question about Removing
 ************************************************************/

void TkQuestionRemove()
{
    XtManageChild( questionDialog );
}

/*===========================================================
                 Save Dialog
============================================================*/

/************************************************************
 * Post Save Dialog
 ************************************************************/

void TkAskSave()
{
    XtManageChild( saveDialog );
}

/************************************************************
 * Unpost Save Dialog
 ************************************************************/

void TkDoneAskingSave()
{
    XtUnmanageChild( saveDialog );
}

/************************************************************
 * Set specified attribute of selected text
 ************************************************************/

static void
TkSetSelectedText(Widget w, enum attributes attr, XtPointer val)
{
  XmString sel, text;
  XmTextPosition left, right, pos;
  
  /* Get selection. */
  sel = XmCSTextGetSelection(w);
  
  /* Set attribute. */
  switch (attr)
    {
    case TkFONT:
      text = TkSetFont(sel, (char *)val);
      break;
    case TkCOLOR:
      text = TkSetColor(sel, (char *)val);
      break;
    case TkTHRU:
      text = TkSetStrikethru(sel, *(Cardinal *)val);
      break;
    case TkUNDER:
      text = TkSetUnderline(sel, *(Cardinal *)val);
      break;
    }
  
  /* Replace text or insert begin/end. */
  if (sel == NULL)
    {
      pos = XmCSTextGetCursorPosition(w);
      XmCSTextInsert(w, pos, text);
    }
  else 
    {
      XmCSTextGetSelectionPosition(w, &left, &right);
      XmCSTextReplace(w, left, right, text);
      XmCSTextSetSelection(w, left, right, XtLastTimestampProcessed(dpy));
    }
    
  XmStringFree(text);
}
  
static XmString
TkSetFont(XmString text, char *font)
{
  XmString begin = NULL, end = NULL;
  
  /* Get begin and end components. */
  if (font != NULL)
    {
      begin = TkGetComp(TkBEGIN, TkFONT, font);
      end = TkGetComp(TkEND, TkFONT, font);
    }
  
  return(TkSetTextAttribute(text, begin, end, tk_fonts, tk_font_count));
}

static XmString
TkSetColor(XmString text, char *color)
{
  XmString begin = NULL, end = NULL;
  
  /* Get begin and end components. */
  if (color != NULL)
    {
      begin = TkGetComp(TkBEGIN, TkCOLOR, color);
      end = TkGetComp(TkEND, TkCOLOR, color);
    }
  
  return(TkSetTextAttribute(text, begin, end, tk_colors, tk_color_count));
}

static XmString
TkSetStrikethru(XmString text, Cardinal line)
{
  XmString begin = NULL, end = NULL;
  char buf[MAX_CHAR];
  
  /* Get begin and end components. */
  sprintf(buf, "Strikethru%d", line);
  
  begin = TkGetComp(TkBEGIN, TkTHRU, buf);
  end = TkGetComp(TkEND, TkTHRU, buf);
  
  return(TkSetTextAttribute(text, begin, end, tk_thru, tk_thru_count));
}

static XmString
TkSetUnderline(XmString text, Cardinal line)
{
  XmString begin = NULL, end = NULL;
  char buf[MAX_CHAR];
  
  /* Get begin and end components. */
  sprintf(buf, "Underline%d", line);
  
  begin = TkGetComp(TkBEGIN, TkUNDER, buf);
  end = TkGetComp(TkEND, TkUNDER, buf);
  
  return(TkSetTextAttribute(text, begin, end, tk_under, tk_under_count));
}

static XmString
TkGetComp(Cardinal comp, enum attributes attr, char *val)
{
  static XmString 	color_comps[MAX_ATTR][2];
  static XmString 	font_comps[MAX_ATTR][2];
  static XmString 	thru_comps[MAX_ATTR][2];
  static XmString 	under_comps[MAX_ATTR][2];
  static Boolean	col_inited, font_inited, thru_inited, under_inited;
  Cardinal 		limit;
  char 			**index;
  XmString 		(*comps)[2];
  XmStringComponentType type;
  int i;
  
  switch (attr)
    {
    case TkCOLOR:
      if (!col_inited)
	{
	  for (i = 0; i < MAX_ATTR; i++)
	    color_comps[i][0] = color_comps[i][1] = NULL;
	  col_inited = TRUE;
	}
      
      limit = tk_color_count;
      index = tk_colors;
      comps = color_comps;
      break;

    case TkFONT:
      if (!font_inited)
	{
	  for (i = 0; i < MAX_ATTR; i++)
	    font_comps[i][0] = font_comps[i][1] = NULL;
	  font_inited = TRUE;
	}
      
      limit = tk_font_count;
      index = tk_fonts;
      comps = font_comps;
      break;

    case TkTHRU:
      if (!thru_inited)
	{
	  for (i = 0; i < MAX_ATTR; i++)
	    thru_comps[i][0] = thru_comps[i][1] = NULL;
	  thru_inited = TRUE;
	}
      
      limit = tk_thru_count;
      index = tk_thru;
      comps = thru_comps;
      break;

    case TkUNDER:
      if (!under_inited)
	{
	  for (i = 0; i < MAX_ATTR; i++)
	    under_comps[i][0] = under_comps[i][1] = NULL;
	  under_inited = TRUE;
	}
      
      limit = tk_under_count;
      index = tk_under;
      comps = under_comps;
      break;
    }
  
  for (i = 0; i < limit; i++)
    {
      if (strcmp(val, index[i]) == 0)
	{
	  if (comps[i][comp] == NULL)
	    {
	      switch(comp)
		{
		case TkBEGIN:
		  type = XmSTRING_COMPONENT_RENDITION_BEGIN;
		  break;
		case TkEND:
		  type = XmSTRING_COMPONENT_RENDITION_END;
		  break;
		default:
		  break;
		}
	      comps[i][comp] = XmStringComponentCreate(type, strlen(val), val);
	    }
	  
	  return(XmStringCopy(comps[i][comp]));
	}
    }
  return(NULL);
}

static XmString 
TkSetTextAttribute(XmString text, XmString begin, XmString end,
		   char **tags, Cardinal tag_count)
{
  XmStringContext 	ctx;
  XmString		comp;
  XmString		result;
  unsigned int		len;
  XtPointer		val;
  XmStringComponentType	type, peek;
  Boolean		end_done = FALSE, init_push = FALSE;
  int			i;
  Boolean		skip;
  
  /* No selection, just insert begin/end. */
  if (text == NULL) return(XmStringConcatAndFree(begin, end));
      
  /* Search after first layout push. */
  if (!XmStringInitContext(&ctx, text))
    {
      /* error */
    }

  type = XmStringGetNextTriple(ctx, &len, &val);
  
  result = begin;

  /* Remove conflicting begin/ends */
  do {
    switch (type)
      {
      case XmSTRING_COMPONENT_LAYOUT_PUSH:
      case XmSTRING_COMPONENT_LAYOUT_POP:
	/* strip push & pop - widget cannot deal
	   with BIDI yet */
	continue;
	break;
       
      case XmSTRING_COMPONENT_RENDITION_BEGIN:
      case XmSTRING_COMPONENT_RENDITION_END:
	/* Check for match and skip if found */
	skip = FALSE;
	for (i = 0; i < tag_count; i++)
	  if (strcmp((XmStringTag)val, tags[i]) == 0)
	    {
	      skip = TRUE;
	      break;
	    }
	if (skip) continue;
	break;

      default:	
	break;
      }
    /* Create component and concat. */
    comp = XmStringComponentCreate(type, len, val);
    result = XmStringConcatAndFree(result, comp);

  } while ((type = XmStringGetNextTriple(ctx, &len, &val)) !=
	   XmSTRING_COMPONENT_END);
    
  /* Insert end component if necessary. */
  if (!end_done) result = XmStringConcatAndFree(result, end);

  XmStringFreeContext(ctx);
  
  return(result);
}

XmRenderTable
TkUpdateRenditions(Widget w, XmRenderTable rt)
{
  int 		i, j, n;
  XmRendition 	rend[20];
  Arg		args[MAX_ARGS];
  Pixel		fg;
  XFontStruct	*font;
  Cardinal	line;
  XmTab tabs[5];
  XmTabList tablist;
  
  rt = XmRenderTableCopy(rt, NULL, 0);
  
  /* Add renditions for all the Help Markup renditions. */
  i = 0;
    n = 0;
    XtSetArg(args[n], XmNfontName, BOLD_FONT_NAME); n++;
    XtSetArg(args[n], XmNfontType, XmFONT_IS_FONT); n++;
    rend[i] = XmRenditionCreate(w, "bold", args, n); i++;
    n = 0;
    XtSetArg(args[n], XmNfontName, HEADING_FONT_NAME); n++;
    XtSetArg(args[n], XmNfontType, XmFONT_IS_FONT); n++;
    rend[i] = XmRenditionCreate(w, "heading", args, n); i++;
    n = 0;
    XtSetArg(args[n], XmNfontName, EMPHASIS_FONT_NAME); n++;
    XtSetArg(args[n], XmNfontType, XmFONT_IS_FONT); n++;
    rend[i] = XmRenditionCreate(w, "emphasis", args, n); i++;
    n = 0;
    XtSetArg(args[n], XmNfontName, TITLE_FONT_NAME); n++;
    XtSetArg(args[n], XmNfontType, XmFONT_IS_FONT); n++;
    rend[i] = XmRenditionCreate(w, "title", args, n); i++; 
    n = 0;
    XtSetArg(args[n], XmNfontName, BOLD_FONT_NAME); n++;
    XtSetArg(args[n], XmNfontType, XmFONT_IS_FONT); n++;
    rend[i] = XmRenditionCreate(w, "subtitle", args, n); i++; 
  n = 0;
  tabs[n] = XmTabCreate(2.0, XmINCHES, XmABSOLUTE,
			XmALIGNMENT_BEGINNING, NULL); n++;
  tabs[n] = XmTabCreate(4.0, XmINCHES, XmABSOLUTE,
			XmALIGNMENT_BEGINNING, NULL); n++;
  tablist = XmTabListInsertTabs(NULL, tabs, n, 0);
  for(i = 0; i < n; i++) XmTabFree(tabs[i]);

  n = 0;
  XtSetArg(args[n], XmNtabList, tablist); n++;
  rend[i] = XmRenditionCreate(w, "table", args, n); i++;
  n = 0;
  XtSetArg(args[n], XmNfontName, "*courier-medium-r-*-12-*"); n++;
  XtSetArg(args[n], XmNfontType, XmFONT_IS_FONT); n++;
  rend[i] = XmRenditionCreate(w, "program", args, n); i++; 
  n = 0;
  XtSetArg(args[n], XmNunderlineType, XmSINGLE_LINE); n++;
  rend[i] = XmRenditionCreate(w, "underline", args, n); i++; 
  
  rt = XmRenderTableAddRenditions(rt, rend, i, XmSKIP);

  for (i = 0; i < tk_color_count; i++)
    {
      n = 0;
      XtSetArg(args[n], XmNrenditionForeground,
	       TkGetColor(w, tk_colors[i])); n++;
      
      rend[0] = XmRenditionCreate(w, tk_colors[i], args, n);
      rt = XmRenderTableAddRenditions(rt, rend, 1, XmSKIP);
      XmRenditionFree(rend[0]);
    }
  
  /* Default Color */
  tk_colors[i] = "DefaultColor";
  tk_color_count++;
  
  XtVaGetValues(w, XmNforeground, &fg, NULL);
  n = 0;
  XtSetArg(args[n], XmNrenditionForeground, fg); n++;
      
  rend[0] = XmRenditionCreate(w, tk_colors[i], args, n);
  rt = XmRenderTableAddRenditions(rt, rend, 1, XmSKIP);
  XmRenditionFree(rend[0]);

  for (i = 0; i < tk_font_count; i++)
    {
      n = 0;
      XtSetArg(args[n], XmNfontName, tk_fonts[i]); n++;
      XtSetArg(args[n], XmNfontType, XmFONT_IS_FONT); n++;
      XtSetArg(args[n], XmNloadModel, XmLOAD_DEFERRED); n++; 
      
      rend[0] = XmRenditionCreate(w, tk_fonts[i], args, n);
      rt = XmRenderTableAddRenditions(rt, rend, 1, XmSKIP);
      XmRenditionFree(rend[0]);
    }
  
  /* Default font. */
  tk_fonts[i] = "DefaultFont";
  tk_font_count++;
  
  XmeRenderTableGetDefaultFont(rt, &font);
  
  n = 0;
  XtSetArg(args[n], XmNfont, font); n++;
  XtSetArg(args[n], XmNfontType, XmFONT_IS_FONT); n++;
      
  rend[0] = XmRenditionCreate(w, tk_fonts[i], args, n);
  rt = XmRenderTableAddRenditions(rt, rend, 1, XmSKIP);
  XmRenditionFree(rend[0]);

  for (i = 0; i < tk_thru_count; i++)
    {
      sscanf(tk_thru[i], "Strikethru%d", &line);
      
      n = 0;
      XtSetArg(args[n], XmNstrikethruType, line); n++;
      
      rend[0] = XmRenditionCreate(w, tk_thru[i], args, n);
      rt = XmRenderTableAddRenditions(rt, rend, 1, XmSKIP);
      XmRenditionFree(rend[0]);
    }
  
  for (i = 0; i < tk_under_count; i++)
    {
      sscanf(tk_under[i], "Underline%d", &line);
      n = 0;
      XtSetArg(args[n], XmNunderlineType, line); n++;
      
      rend[0] = XmRenditionCreate(w, tk_under[i], args, n);
      rt = XmRenderTableAddRenditions(rt, rend, 1, XmSKIP);
      XmRenditionFree(rend[0]);
    }

  return(rt);
}

static Pixel
TkGetColor(Widget w, char *colstr)
{
  XrmValue from, to;

  from.size = strlen(colstr) + 1;
  if (from.size < sizeof(String)) from.size = sizeof(String);
  from.addr = colstr;
  to.addr = NULL;
  XtConvert(w, XmRString, &from, XmRPixel, &to);

  if (to.addr != NULL) return((Pixel)*((Pixel *)to.addr));
  else return ((Pixel)XmUNSPECIFIED_PIXEL);
}

/*===========================================================
               Initialization & Callbacks
============================================================*/

void TkInit()
{
    TkPaneInit();   /* Text Pane support initialization */
    DlgWantClearCB(); /* Arrange for a clear text area */
}

/************************************************************
 * User toggled among Open/Save/Copy/Move
 ************************************************************/

static void
ToggleOpCB(Widget w, XtPointer cd, XtPointer wd)
{
    w = ((XmRowColumnCallbackStruct *)wd)->widget;
    wd = (XtPointer)( ((XmRowColumnCallbackStruct *)wd)->callbackstruct );
    if ( ((XmToggleButtonCallbackStruct *)wd)->set )
        if      ( w == openToggle ) TkArrangeToOpen();
        else if ( w == saveToggle ) TkArrangeToSave();
        else if ( w == copyToggle ) TkArrangeToCopy();
        else if ( w == moveToggle ) TkArrangeToMove();
}

/************************************************************
 * User toggled whether or not FSB should stay mapped
 ************************************************************/

static void
ToggleKeepFileDialogueCB(Widget w, XtPointer cd, XtPointer wd)
{
    unsigned char val;

    n = 0;
    XtSetArg( args[n], XmNset, &val ); n++;
    XtGetValues( w, args, n );
    DlgKeepFileDialogueCB( val );
}

/************************************************************
 * User toggled whether operation should revert to Open after selecting a file
 ************************************************************/

static void
ToggleRevertToOpenCB(Widget w, XtPointer cd, XtPointer wd)
{
    unsigned char val;

    n = 0;
    XtSetArg( args[n], XmNset, &val ); n++;
    XtGetValues( w, args, n );
    DlgRevertToOpenCB( val );
}

/************************************************************
 * A text pane gained focus
 ************************************************************/

static void
TextGainFocusCB(Widget w, XtPointer cd, XtPointer wd)
{
    textCurFocus = w;
    textLastFocus = w;
}

/************************************************************
 * A text pane lost focus
 ************************************************************/

static void
TextLoseFocusCB(Widget w, XtPointer cd, XtPointer wd)
{
    textCurFocus = NULL;
    if ( w == XmGetDestination( dpy ) )
        textLastDestination = w;
}

/************************************************************
 * A text pane became owner of the primary selection
 ************************************************************/

static void
TextGainPrimaryCB(Widget w, XtPointer cd, XtPointer wd)
{
    textCurPrimary = w;
}

/************************************************************
 * A text pane lost ownership of the primary selection
 ************************************************************/

static void
TextLosePrimaryCB(Widget w, XtPointer cd, XtPointer wd)
{
    textCurPrimary = NULL;
}

/************************************************************
 * The text was modified
 ************************************************************/

static void
TextChangedCB(Widget w, XtPointer cd, XtPointer wd)
{
  static Boolean init = TRUE;
  
  /* Skip the initial callback when the text is created. */
  if (init)
    {
      init = FALSE;
      return;
    }
  
    if ( textchanged == 0 ) DlgNoteJustChangedCB();
    textchanged++;
    if ( textchangedsince == 0 ) DlgNoteJustChangedSinceCB();
    textchangedsince++;
}

/************************************************************
 * The user pressed the FSB's Ok button
 ************************************************************/

static void
OkFileCB(Widget w, XtPointer cd, XtPointer wd)
{
    char *filnam;

    filnam = TkGetFilename();

    if      ( curToggle == openToggle ) DlgSelectOpenCB( filnam );
    else if ( curToggle == saveToggle ) DlgSelectSaveCB( filnam );
    else if ( curToggle == copyToggle ) DlgSelectCopyCB( filnam );
    else if ( curToggle == moveToggle ) DlgSelectMoveCB( filnam );

    XtFree( filnam );
}

/************************************************************
 * The user pressed the FSB's Cancel button
 ************************************************************/

static void
CancelFileCB(Widget w, XtPointer cd, XtPointer wd)
{
    DlgSelectCancelCB();
}

/************************************************************
 * The user pressed Yes when asked about saving the File
 ************************************************************/

static void
SaveYesCB(Widget w, XtPointer cd, XtPointer wd)
{
    DlgSaveYesCB();
}

/************************************************************
 * The user pressed No when asked about saving the File
 ************************************************************/

static void
SaveNoCB(Widget w, XtPointer cd, XtPointer wd)
{
    DlgSaveNoCB();
}

/************************************************************
 * The user pressed Cancel when asked about saving the File
 ************************************************************/

static void
SaveCancelCB(Widget w, XtPointer cd, XtPointer wd)
{
    DlgSaveCancelCB();
}

/************************************************************
 * The user pressed Cancel when warned
 ************************************************************/

static void
WarnCancelCB(Widget w, XtPointer cd, XtPointer wd)
{
    DlgWarnCancelCB( warn_reason );
}

/************************************************************
 * The user pressed Yes when questioned about remving the file
 ************************************************************/

static void
QuestionYesCB(Widget w, XtPointer cd, XtPointer wd)
{
    DlgQuestionYesCB();
}

/************************************************************
 * The user pressed New in the File Menu
 ************************************************************/

static void
NewCB(Widget w, XtPointer cd, XtPointer wd)
{
    DlgWantClearCB();
}

/************************************************************
 * The user pressed Print in the File Menu
 ************************************************************/

static void
PrintCB(Widget w, XtPointer cd, XtPointer wd)
{
    XtManageChild(printDialog);
}

/************************************************************
 * The user pressed Open in the File Menu
 ************************************************************/

static void
OpenCB(Widget w, XtPointer cd, XtPointer wd)
{
    DlgWantOpenCB();
}

/************************************************************
 * The user pressed Save in the File Menu
 ************************************************************/

static void
SaveCB(Widget w, XtPointer cd, XtPointer wd)
{
    DlgWantSaveCB();
}

/************************************************************
 * The user pressed Save As in the File Menu
 ************************************************************/

static void
SaveAsCB(Widget w, XtPointer cd, XtPointer wd)
{
    DlgWantSaveAsCB();
}

/************************************************************
 * The user pressed Copy in the File Menu
 ************************************************************/

static void
CopyToCB(Widget w, XtPointer cd, XtPointer wd)
{
    DlgWantCopyCB();
}

/************************************************************
 * The user pressed Move in the File Menu
 ************************************************************/

static void
MoveToCB(Widget w, XtPointer cd, XtPointer wd)
{
    DlgWantMoveCB();
}

/************************************************************
 * The user pressed Remove in the File Menu
 ************************************************************/

static void
RemoveCB(Widget w, XtPointer cd, XtPointer wd)
{
    DlgWantRemoveCB();
}

/************************************************************
 * The user pressed Exit in the File Menu
 ************************************************************/

static void
ExitCB(Widget w, XtPointer cd, XtPointer wd)
{
    DlgExitCB();
}

/************************************************************
 * The user pressed Cut in the Edit Menu
 ************************************************************/

static void
CutCB(Widget w, XtPointer cd, XtPointer wd)
{
    if ( textCurPrimary != NULL )
        XmCSTextCut( textCurPrimary, XtLastTimestampProcessed( dpy ) );
    else
        TkBeep();
}

/************************************************************
 * The user pressed Copy in the Edit Menu
 ************************************************************/

static void
CopyCB(Widget w, XtPointer cd, XtPointer wd)
{
    if ( textCurPrimary != NULL )
        XmCSTextCopy( textCurPrimary, XtLastTimestampProcessed( dpy ) );
    else
        TkBeep();
}

/************************************************************
 * The user pressed Paste in the Edit Menu
 ************************************************************/

static void
PasteCB(Widget w, XtPointer cd, XtPointer wd)
{
    w = TkPaneTarget();
    if ( w != NULL )
        XmCSTextPaste( w );
    else
        TkBeep();
}

/************************************************************
 * The user pressed Clear in the Edit Menu
 ************************************************************/

static void
ClearCB(Widget w, XtPointer cd, XtPointer wd)
{
    if ( textCurPrimary != NULL )
        XtCallActionProc( textCurPrimary, "clear-selection",
            ((XmAnyCallbackStruct *)(wd))->event, NULL, 0 );
    else
        TkBeep();
}

/************************************************************
 * The user pressed Delete in the Edit Menu
 ************************************************************/

static void
DeleteCB(Widget w, XtPointer cd, XtPointer wd)
{
    if ( textCurPrimary != NULL )
        XmCSTextRemove( textCurPrimary );
    else
        TkBeep();
}

/************************************************************
 * The user pressed Split in the View Menu
 ************************************************************/

static void
SplitCB(Widget w, XtPointer cd, XtPointer wd)
{
      /* It would be nice if the target pane could be split, and the
         new pane inserted underneath the target pane, but this is
         not easy to do in Motif 1.1 */

    TkPaneAppend();
}

/************************************************************
 * The user pressed Remove Pane in the View Menu
 ************************************************************/

static void
RemovePaneCB(Widget w, XtPointer cd, XtPointer wd)
{
    TkPaneRemove();
}

/************************************************************
 * The user pressed One Pane in the View Menu
 ************************************************************/

static void
OnePaneCB(Widget w, XtPointer cd, XtPointer wd)
{
    TkPaneOne();
}

/************************************************************
 * The user pressed the default button in the Fonts Menu
 ************************************************************/

static void
DefaultFontCB(Widget w, XtPointer cd, XtPointer wd)
{
    if ( textCurPrimary != NULL )
      TkSetSelectedText(textCurPrimary, TkFONT, "DefaultFont");
    else
      TkBeep();
}

/************************************************************
 * The user pressed a font button in the Fonts Menu
 ************************************************************/

static void
SetFontCB(Widget w, XtPointer cd, XtPointer wd)
{
    if ( textCurPrimary != NULL )
      TkSetSelectedText(textCurPrimary, TkFONT, cd);
    else
      TkBeep();
}

/************************************************************
 * Creation callback for a font button in the fonts Menu
 ************************************************************/

static void
AddFontCB(Widget w, XtPointer cd, XtPointer wd)
{
/*  tk_fonts = (char **)XtRealloc((char *)tk_fonts, sizeof(char *) * (tk_font_count + 1));
  */tk_fonts[tk_font_count] = (char *)cd;
  tk_font_count++;
}

/************************************************************
 * The user pressed the default button in the Colors Menu
 ************************************************************/

static void
DefaultColorCB(Widget w, XtPointer cd, XtPointer wd)
{
    if ( textCurPrimary != NULL )
      TkSetSelectedText(textCurPrimary, TkCOLOR, "DefaultColor");
    else
      TkBeep();
}

/************************************************************
 * The user pressed a color button in the Colors Menu
 ************************************************************/

static void
SetColorCB(Widget w, XtPointer cd, XtPointer wd)
{
    if ( textCurPrimary != NULL )
      TkSetSelectedText(textCurPrimary, TkCOLOR, cd);
    else
      TkBeep();
}

/************************************************************
 * Creation callback for a color button in the Colors Menu
 ************************************************************/

static void
AddColorCB(Widget w, XtPointer cd, XtPointer wd)
{
 /* tk_colors = (char **)XtRealloc((char *)tk_colors, sizeof(char *) * (tk_color_count + 1));
*/  tk_colors[tk_color_count] = (char *)cd;
  tk_color_count++;
}

/************************************************************
 * The user pressed the Remove button in the Lines Menu
 ************************************************************/

static void
RemoveLinesCB(Widget w, XtPointer cd, XtPointer wd)
{
  Cardinal val;
  
  if ( textCurPrimary != NULL )
    {
      val = XmNO_LINE;
      
      TkSetSelectedText(textCurPrimary, TkTHRU, &val);
      TkSetSelectedText(textCurPrimary, TkUNDER, &val);
    }
    else TkBeep();
}

/************************************************************
 * The user pressed the Strikethru button in the Lines Menu
 ************************************************************/

static void
StrikethruCB(Widget w, XtPointer cd, XtPointer wd)
{
    if ( textCurPrimary != NULL )
      TkSetSelectedText(textCurPrimary, TkTHRU, cd);
    else
      TkBeep();
}

/************************************************************
 * The user pressed the Underline button in the Lines Menu
 ************************************************************/

static void
UnderlineCB(Widget w, XtPointer cd, XtPointer wd)
{
    if ( textCurPrimary != NULL )
      TkSetSelectedText(textCurPrimary, TkUNDER, cd);
    else
      TkBeep();
}

/************************************************************
 * The user pressed the Overview button in the Help Menu
 ************************************************************/

static void
OverviewCB(Widget w, XtPointer cd, XtPointer wd)
{
  XtVaSetValues(helpDialog, XmdNhelpFile, "texteditor.help", NULL);
  XtManageChild(helpDialog);
}

/************************************************************
 * The user pressed some button not yet implemented.
 ************************************************************/
static void
NoopCB(Widget w, XtPointer cd, XtPointer wd)
{
  TkBeep();
}

  /* To associate UIL callback names with actual callback routines */

static MRMRegisterArg argNames[] = {
  { "ToggleOpCB", (XtPointer)ToggleOpCB },
  { "ToggleKeepFileDialogueCB", (XtPointer)ToggleKeepFileDialogueCB },
  { "ToggleRevertToOpenCB", (XtPointer)ToggleRevertToOpenCB },
  { "TextChangedCB", (XtPointer)TextChangedCB },
  { "TextGainFocusCB", (XtPointer)TextGainFocusCB },
  { "TextLoseFocusCB", (XtPointer)TextLoseFocusCB },
  { "TextGainPrimaryCB", (XtPointer)TextGainPrimaryCB },
  { "TextLosePrimaryCB", (XtPointer)TextLosePrimaryCB },
  { "NewCB", (XtPointer)NewCB },
  { "PrintCB", (XtPointer)PrintCB },
  { "PrintSetupCB", (XtPointer)NoopCB },
  { "CloseCB", (XtPointer)NoopCB },
  { "UndoCB", (XtPointer)NoopCB },
  { "RepeatRedoCB", (XtPointer)NoopCB },
  { "DragMoveCB", (XtPointer)NoopCB },
  { "SelectAllCB", (XtPointer)NoopCB },
  { "DeselectAllCB", (XtPointer)NoopCB },
  { "ChangeViewCB", (XtPointer)NoopCB },
  { "NewWindowCB", (XtPointer)NoopCB },
  { "OpenCB", (XtPointer)OpenCB },
  { "SaveCB", (XtPointer)SaveCB },
  { "SaveAsCB", (XtPointer)SaveAsCB },
  { "CopyToCB", (XtPointer)CopyToCB },
  { "MoveToCB", (XtPointer)MoveToCB },
  { "RemoveCB", (XtPointer)RemoveCB },
  { "ExitCB", (XtPointer)ExitCB },
  { "CutCB", (XtPointer)CutCB },
  { "CopyCB", (XtPointer)CopyCB },
  { "PasteCB", (XtPointer)PasteCB },
  { "ClearCB", (XtPointer)ClearCB },
  { "DeleteCB", (XtPointer)DeleteCB },
  { "SplitCB", (XtPointer)SplitCB },
  { "RemovePaneCB", (XtPointer)RemovePaneCB },
  { "OverviewCB", (XtPointer)OverviewCB },
  { "TocCB", (XtPointer)NoopCB },
  { "TasksCB", (XtPointer)NoopCB },
  { "ReferenceCB", (XtPointer)NoopCB },
  { "OnePaneCB", (XtPointer)OnePaneCB },
  { "OnItemCB", (XtPointer)NoopCB },
  { "UsingHelpCB", (XtPointer)NoopCB },
  { "AboutCB", (XtPointer)NoopCB },
  { "OkFileCB", (XtPointer)OkFileCB },
  { "CancelFileCB", (XtPointer)CancelFileCB },
  { "SaveYesCB", (XtPointer)SaveYesCB },
  { "SaveNoCB", (XtPointer)SaveNoCB },
  { "SaveCancelCB", (XtPointer)SaveCancelCB },
  { "WarnCancelCB", (XtPointer)WarnCancelCB },
  { "QuestionYesCB", (XtPointer)QuestionYesCB },
  { "DefaultFontCB", (XtPointer)DefaultFontCB },
  { "SetFontCB", (XtPointer)SetFontCB },
  { "AddFontCB", (XtPointer)AddFontCB },
  { "DefaultColorCB", (XtPointer)DefaultColorCB },
  { "SetColorCB", (XtPointer)SetColorCB }, 
  { "AddColorCB", (XtPointer)AddColorCB },
  { "textOnlyFormat", (XtPointer)TkTEXT_ONLY },
  { "byteStreamFormat", (XtPointer)TkBYTE_STREAM },
  { "RemoveLinesCB", (XtPointer)RemoveLinesCB },
  { "StrikethruCB", (XtPointer)StrikethruCB },
  { "UnderlineCB", (XtPointer)UnderlineCB }
  
};

void TkRegisterCallbacks()
{
  MrmRegisterNames(argNames, XtNumber(argNames));
}  
