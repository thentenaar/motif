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
static char *rcsid = "$XConsortium: interface.c /main/7 1996/06/11 17:27:01 pascale $";
#endif
#endif

#include <stdlib.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#ifdef USE_CMS
#include <X11/Xcms.h>
#endif /* USE_CMS */
#include <Xm/Xm.h>
#include <Xm/Protocols.h>
#include <Xmd/Menus.h>
#include <Xmd/Help.h>

#include <stdarg.h>

#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include "transfer.h"


#include <Xm/CascadeB.h>
#include <Xm/CascadeBG.h>
#include <Xm/DrawingA.h>
#include <Xm/Form.h>
#include <Xm/Frame.h>
#include <Xm/LabelG.h>
#include <Xm/List.h>
#include <Xm/MainW.h>
#include <Xm/MessageB.h>
#include <Xm/MwmUtil.h>
#include <Xm/PushBG.h>
#include <Xm/RowColumn.h>
#include <Xm/ScrolledW.h>
#include <Xm/SeparatoG.h>
#include <Xm/CSText.h>
#include <Xm/ToggleB.h>
#include <Xm/ToggleBG.h>

/* These translations make togglebutton ignore the user,  they're
   used to prevent the user from setting or clearing the ownership
   display */
#define TB_NO_USER_INPUT \
"<EnterWindow>:		NOP()\n\
<LeaveWindow>:		NOP()\n\
c<Btn1Down>:		NOP()\n\
~c<Btn1Down>:		NOP()\n\
~c<Btn1Up>:		NOP()"

Widget CreateHelpDialog();
void manageCB();

/* Local variables used when selecting screen & display */
static Display * sDisplay = NULL;
static Screen  * sScreen  = NULL;


Widget mainwindow, menubar, form, optionmenu,
       selectionmenu, primary, secondary, clipboard, named,
       state, op, omd, onamed, frame,
       target, frame1, workarea, field2, l2, text2,
       format_om, formatmenu, format_s, format_l,
       format_t, format_cs, format_id, format_ih, field1,
       l1, text1, buttons, typeOM, typepulldown, b_atom,
       b_drawable, b_window, b_colormap, b_integer, b_pixel,
       b_compound_text, b_string, b_span, b_locale_text,
       b__motif_compound_string, unknown_button, list,
       b_locale, framelabel, defer, text_display;

/****************************************************************
 * get_constraint_widget:
 ****************************************************************/
static Widget get_constraint_widget(Widget child, Widget parent)
{
  Widget w;
  
  w = child;
  while (XtParent(w) != parent)
    w = XtParent(w);
  return (w);
}



/****************************************************************
 *
 *  Main C code for presentation component
 *
 ****************************************************************/

/**************************************************************
 * create_mainWindow_widget:
 **************************************************************/
Widget create_mainWindow_widget(char    * name,
				Widget    parent)
{
  Arg args[27];
  Arg pargs[27];
  Widget tmpw;
  Widget helpDialog, helpMenu, help, help_overview, pulldown;
  Widget frame1;
  XmString xms;
  XmString xms1;
  XtPointer closure;
  int inc;
  int n;
  int pn;
#ifdef USE_CMS
  XcmsColor target_color, exact_color;
#else
  XColor target_color, exact_color;
#endif /* USE_CMS */
  Pixel target_color_pixel;
  XtTranslations toggle_button_trans = 
    XtParseTranslationTable(TB_NO_USER_INPUT);
  XmRendition rend[1];
  XmRenderTable listRT;
  XmTabList listTabs = NULL;
  XmTab tabs[1];
  WidgetList menu;
  Cardinal count;

  /* RenderTable */
  tabs[0] = XmTabCreate(3.0, XmINCHES, XmABSOLUTE, 
			XmALIGNMENT_BEGINNING, NULL);
  listTabs = XmTabListInsertTabs(NULL, tabs, 1, 0);
  XmTabFree(tabs[0]);

  n = 0;
  XtSetArg(args[n], XmNfontName, "*helvetica-bold-r-*-12-*"); n++;
  XtSetArg(args[n], XmNfontType, XmFONT_IS_FONT); n++;
  XtSetArg(args[n], XmNtabList, listTabs); n++;
  rend[0] = XmRenditionCreate(parent, XmFONTLIST_DEFAULT_TAG, args, n);
  listRT = XmRenderTableAddRenditions(NULL, rend, 1, XmMERGE_REPLACE);;
  XmTabListFree(listTabs);
  XmRenditionFree(rend[0]);

  /* Get red color,  or white if monochrome or failure */
  if (DefaultDepthOfScreen(XtScreen(parent)) > 1 &&
      /* XcmsLookupColor(XtDisplay(parent),
	 DefaultColormapOfScreen(XtScreen(parent)),
	 "CIEuvY:0.0/0.0/1.0", 
	 &exact_color, &target_color,
	 XcmsUndefinedFormat) == XcmsSuccess &&
	 XcmsAllocColor(XtDisplay(parent), 
	 DefaultColormapOfScreen(XtScreen(parent)),
	 &target_color,
	 XcmsUndefinedFormat) == XcmsSuccess) */
      
      XLookupColor(XtDisplay(parent),
		   DefaultColormapOfScreen(XtScreen(parent)),
		   "red", &exact_color, &target_color) != 0 &&
      XAllocColor(XtDisplay(parent), 
		  DefaultColormapOfScreen(XtScreen(parent)),
		  &target_color) != 0)
    target_color_pixel = target_color.pixel;
  else
    target_color_pixel = WhitePixelOfScreen(XtScreen(parent));
  
  mainwindow = XmCreateMainWindow(parent, name, NULL, 0);
  
  menubar = XmCreateMenuBar(mainwindow, "menuBar", NULL, 0);
  
  pulldown = XmdCreateMenu(SELECTED_MENU, menubar, &menu, &count);
  XtVaSetValues(pulldown, XmNtearOffModel, XmTEAR_OFF_ENABLED,
		NULL, NULL);
  XtUnmanageChild(menu[SELECTED_OPEN]);
  XtManageChild(menu[SELECTED_PROPERTIES]);
  XtManageChild(menu[SELECTED_NEW]);
  XtUnmanageChild(menu[SELECTED_SEP1]);
  XtManageChild(menu[SELECTED_DELETE]);
  XtAddCallback(menu[SELECTED_DELETE], XmNactivateCallback, 
		(XtCallbackProc) RemoveValue, NULL);
  XtAddCallback(menu[SELECTED_NEW], XmNactivateCallback, 
		(XtCallbackProc) manageCB, (XtPointer) editBox);
  XtAddCallback(menu[SELECTED_PROPERTIES], XmNactivateCallback, 
		(XtCallbackProc) manageCB, (XtPointer) editBox);
  XtAddCallback(menu[SELECTED_NEW], XmNactivateCallback, 
		(XtCallbackProc) EditItem, (XtPointer) 0);
  XtAddCallback(menu[SELECTED_PROPERTIES], XmNactivateCallback, 
		(XtCallbackProc) EditItem, (XtPointer) 1);

  XtAddCallback(menu[SELECTED_EXIT], XmNactivateCallback, 
		(XtCallbackProc) quitCB, NULL);

  pulldown = XmdCreateMenu(EDIT_MENU, menubar, &menu, &count);
  XtVaSetValues(pulldown, XmNtearOffModel, XmTEAR_OFF_ENABLED,
		NULL, NULL);
  XtAddCallback(menu[EDIT_CUT],	XmNactivateCallback, 
		(XtCallbackProc) cut, NULL);
  XtAddCallback(menu[EDIT_COPY], XmNactivateCallback,
		(XtCallbackProc) copy, NULL);
  XtAddCallback(menu[EDIT_PASTE], XmNactivateCallback, 
		(XtCallbackProc) paste, NULL);
  xms = XmStringCreateLocalized("Paste and Delete");
  XtVaSetValues(menu[EDIT_PASTE_SPECIAL],
		XmNlabelString, xms, NULL, NULL);
  XmStringFree(xms);
  XtAddCallback(menu[EDIT_PASTE_SPECIAL], XmNactivateCallback,
		(XtCallbackProc) pasteMove, NULL);
  XtManageChild(menu[EDIT_PASTE_SPECIAL]);
  XtUnmanageChild(menu[EDIT_DELETE]);
  
  /*********** Create the help menu and help text *********/
  helpMenu = XmCreatePulldownMenu(menubar, "helpMenu", NULL, 0);

  n = 0;
  XtSetArg(args[n], XmNsubMenuId, helpMenu); n++;
  XtSetArg(args[n], XmNmnemonic, 'H'); n++;
  help = XmCreateCascadeButtonGadget(menubar, "Help", args, n);
  XtManageChild(help);

  n = 0;
  XtSetArg(args[n], XmNmenuHelpWidget, help); n++;
  XtSetValues(menubar, args, n);

  helpDialog = CreateHelpDialog(mainwindow, "helpDialog");

  n = 0;
  XtSetArg(args[n], XmNmnemonic, 'O'); n++;
  help_overview = XmCreatePushButtonGadget(helpMenu, "Overview", args, n);
  XtManageChild(help_overview);
  XtAddCallback(help_overview, XmNactivateCallback,
		(XtCallbackProc) manageCB, (XtPointer) helpDialog);

  /***************** form : XmForm *****************/
  n = 0;
  XtSetArg(args[n], XmNrubberPositioning, False); n++;
  XtSetArg(args[n], XmNresizePolicy, XmRESIZE_ANY); n++;
  form = XmCreateForm(mainwindow, "form", args, n);

  /***************** SelectionMenu : XmPulldownMenu *****************/
  selectionmenu = XmCreatePulldownMenu(form, "SelectionMenu", NULL, 0);

  /***************** Primary : XmPushButtonGadget *****************/
  n = 0;
  XtSetArg(args[n], XmNmnemonic, 'P'); n++;
  primary = XmCreatePushButtonGadget(selectionmenu, "Primary", args, n);

  /***************** Secondary : XmPushButtonGadget *****************/
  n = 0;
  XtSetArg(args[n], XmNmnemonic, 'S'); n++;
  secondary = XmCreatePushButtonGadget(selectionmenu, "Secondary", args, n);

  /***************** Clipboard : XmPushButtonGadget *****************/
  n = 0;
  XtSetArg(args[n], XmNmnemonic, 'C'); n++;
  clipboard = XmCreatePushButtonGadget(selectionmenu, "Clipboard", args, n);

  /***************** Named : XmPushButtonGadget *****************/
  n = 0;
  XtSetArg(args[n], XmNmnemonic, 'A'); n++;
  named = XmCreatePushButtonGadget(selectionmenu, "NamedAtom", args, n);

  /***************** optionMenu : XmOptionMenu *****************/
  n = 0;
  xms = XmStringCreateLocalized("Selection");
  XtSetArg(args[n], XmNlabelString, xms); n++;
  XtSetArg(args[n], XmNmnemonic, 'S'); n++;
  XtSetArg(args[n], XmNresizable, True); n++;
  XtSetArg(args[n], XmNsubMenuId, selectionmenu); n++;
  optionmenu = XmCreateOptionMenu(form, "optionMenu", args, n);
  if (xms) XmStringFree(xms);

  /***************** state : XmRowColumn *****************/
  n = 0;
  XtSetArg(args[n], XmNorientation, XmVERTICAL); n++;
  state = XmCreateRowColumn(form, "state", args, n);

  /***************** showPrimary : XmToggleButtonGadget *****************/
  n = 0;
  xms = XmStringCreateLocalized("Owns Primary");
  XtSetArg(args[n], XmNlabelString, xms); n++;
  XtSetArg(args[n], XmNindicatorType, XmN_OF_MANY); n++;
  XtSetArg(args[n], XmNindicatorOn, XmINDICATOR_CHECK); n++;
  XtSetArg(args[n], XmNindicatorSize, 15); n++;
  XtSetArg(args[n], XmNtraversalOn, False); n++;
  op = XmCreateToggleButton(state, "showPrimary", args, n);
  XtOverrideTranslations(op, toggle_button_trans);

  if (xms) XmStringFree(xms);

  /***************** showMotifDest : XmToggleButtonGadget *****************/
  n = 0;
  xms = XmStringCreateLocalized("Owns Destination");
  XtSetArg(args[n], XmNlabelString, xms); n++;
  XtSetArg(args[n], XmNindicatorType, XmN_OF_MANY); n++;
  XtSetArg(args[n], XmNindicatorOn, XmINDICATOR_CHECK); n++;
  XtSetArg(args[n], XmNindicatorSize, 15); n++;
  XtSetArg(args[n], XmNtraversalOn, False); n++;
  omd = XmCreateToggleButton(state, "showMotifDest", args, n);
  XtOverrideTranslations(omd, toggle_button_trans);

  if (xms) XmStringFree(xms);

  /***************** showNamed : XmToggleButtonGadget *****************/
  n = 0;
  xms = XmStringCreateLocalized("Owns Named");
  XtSetArg(args[n], XmNlabelString, xms); n++;
  XtSetArg(args[n], XmNindicatorType, XmN_OF_MANY); n++;
  XtSetArg(args[n], XmNindicatorOn, XmINDICATOR_CHECK); n++;
  XtSetArg(args[n], XmNindicatorSize, 15); n++;
  XtSetArg(args[n], XmNtraversalOn, False); n++;
  onamed = XmCreateToggleButton(state, "showNamed", args, n);
  XtOverrideTranslations(onamed, toggle_button_trans);

  if (xms) XmStringFree(xms);

  /***************** frame : XmFrame *****************/
  n = 0;
  XtSetArg(args[n], XmNx, 57); n++;
  XtSetArg(args[n], XmNy, 63); n++;
  XtSetArg(args[n], XmNresizable, True); n++;
  frame = XmCreateFrame(form, "frame", args, n);

  /***************** drawingArea : XmDrawingArea *****************/
  n = 0;
  XtSetArg(args[n], XmNx, 145); n++;
  XtSetArg(args[n], XmNy, 68); n++;
  XtSetArg(args[n], XmNwidth, 70); n++;
  XtSetArg(args[n], XmNheight, 70); n++;
  XtSetArg(args[n], XtNbackground, target_color_pixel); n++;
  target = XmCreateDrawingArea(frame, "drawingArea", args, n);

  /***************** frame1 : XmFrame *****************/
  n = 0;
  XtSetArg(args[n], XmNresizable, True); n++;
  frame1 = XmCreateFrame(form, "frame1", args, n);

  /***************** display : XmScrolledCSText ******************/
  n = 0;
  XtSetArg(args[n], XmNresizable, True); n++;
  XtSetArg(args[n], XmNeditMode, XmMULTI_LINE_EDIT); n++;
  XtSetArg(args[n], XmNeditable, False); n++;
  XtSetArg(args[n], XmNrows, 5); n++;
  text_display = XmCreateScrolledCSText(frame1, "display", args, n);

  /***************** targetList : XmScrolledList *****************/
  n = 0;
  XtSetArg(args[n], XmNx, 52); n++;
  XtSetArg(args[n], XmNy, 226); n++;
  XtSetArg(args[n], XmNwidth, 250); n++;
  XtSetArg(args[n], XmNheight, 240); n++;
  XtSetArg(args[n], XmNselectionPolicy, XmBROWSE_SELECT); n++;
  XtSetArg(args[n], XmNfontList, listRT); n++;
  list = XmCreateScrolledList(form, "targetList", args, n);
  XtManageChild(menubar);

  /***************** optionMenu : XmOptionMenu *****************/
  n = 0;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNleftOffset, 10); n++;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNtopOffset, 10); n++;
  XtSetValues(optionmenu, args, n);


  XtAddCallback(primary, XmNactivateCallback, 
		(XtCallbackProc) setMode, (XtPointer) Mprimary);
  XtManageChild(primary);

  XtAddCallback(secondary,
		XmNactivateCallback, 
		(XtCallbackProc) setMode, (XtPointer) Msecondary);
  XtManageChild(secondary);

  XtAddCallback(clipboard,
		XmNactivateCallback, 
		(XtCallbackProc) setMode, (XtPointer) Mclipboard);
  XtManageChild(clipboard);

  XtAddCallback(named,
		XmNactivateCallback,
		(XtCallbackProc) setMode, (XtPointer) Mnamed);
  XtManageChild(named);
  XtManageChild(optionmenu);

  /***************** state : XmRowColumn *****************/
  n = 0;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
  XtSetArg(args[n], XmNleftWidget, frame); n++;
  XtSetValues(state, args, n);

  XtManageChild(op);
  XtManageChild(omd);
  XtManageChild(onamed);
  XtManageChild(state);

  /***************** frame : XmFrame *****************/
  n = 0;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNtopOffset, 10); n++;
  XtSetArg(args[n], XmNleftOffset, 10); n++;
  XtSetArg(args[n], XmNleftWidget, optionmenu); n++;
  XtSetValues(frame, args, n);
  XtManageChild(target);
  XtManageChild(frame);

  /***************** frame1 : XmFrame *****************/
  n = 0;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNleftOffset, 10); n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
  XtSetArg(args[n], XmNtopWidget, frame); n++;
  XtSetArg(args[n], XmNtopOffset, 6); n++;
  XtSetArg(args[n], XmNrightOffset, 10); n++;
  XtSetValues(frame1, args, n);

  /***************** targetList : XmScrolledList *****************/
  pn = 0;

  XtAddCallback(list, XmNbrowseSelectionCallback,
		(XtCallbackProc) ListSelectItem, NULL);
  XtAddCallback(list, XmNdefaultActionCallback,
		(XtCallbackProc) manageCB, (XtPointer) editBox);
  XtAddCallback(list, XmNdefaultActionCallback,
		(XtCallbackProc) EditItem, (XtPointer) 1);

  XtSetArg(pargs[pn], XmNleftAttachment, XmATTACH_OPPOSITE_WIDGET); pn++;
  XtSetArg(pargs[pn], XmNrightAttachment, XmATTACH_OPPOSITE_WIDGET); pn++;
  XtSetArg(pargs[pn], XmNtopAttachment, XmATTACH_WIDGET); pn++;
  XtSetArg(pargs[pn], XmNtopWidget, frame1); pn++;
  XtSetArg(pargs[pn], XmNtopOffset, 5); pn++;
  XtSetArg(pargs[pn], XmNleftWidget, frame1); pn++;
  XtSetArg(pargs[pn], XmNleftOffset, 0); pn++;
  XtSetArg(pargs[pn], XmNrightWidget, frame1); pn++;
  XtSetArg(pargs[pn], XmNrightOffset, 0); pn++;
  XtSetArg(pargs[pn], XmNbottomAttachment, XmATTACH_FORM); pn++;
  XtSetArg(pargs[pn], XmNbottomOffset, 5); pn++;
  tmpw = get_constraint_widget(list, form);
  if (tmpw)
    XtSetValues(tmpw, pargs, pn);

  XtManageChild(text_display);
  XtManageChild(frame1);
  XtManageChild(list);
  XtManageChild(form);
  XtManageChild(mainwindow);

  return mainwindow;
}

/**************************************************************
 * create_editbox_widget:
 **************************************************************/
Widget create_editbox_widget(char    * name,
				Widget    parent)
{
  Arg args[27];
  Arg pargs[27];
  Widget dialog, tmpw;
  Widget button;
  Cardinal n, pn;
  XmString xms;
  XmString ls1, ls2, ls3;

  n = 0;
  ls1 = XmStringCreateLocalized("OK");
  ls2 = XmStringCreateLocalized("Cancel");
  ls3 = XmStringCreateLocalized("Help");
  XtSetArg(args[n], XmNokLabelString, ls1); n++;
  XtSetArg(args[n], XmNcancelLabelString, ls2); n++;
  XtSetArg(args[n], XmNhelpLabelString, ls3); n++;
  XtSetArg(args[n], XmNautoUnmanage, False); n++;
  dialog = XmCreateTemplateDialog(parent, name, args, n);
  XmStringFree(ls1);
  XmStringFree(ls2);
  XmStringFree(ls3);
  XtAddCallback(dialog, XmNokCallback, (XtCallbackProc) unmanageCB, dialog);
  XtAddCallback(dialog, XmNcancelCallback,
		(XtCallbackProc) unmanageCB, dialog);
  XtAddCallback(dialog, XmNhelpCallback, (XtCallbackProc) unmanageCB, dialog);

  XtAddCallback(dialog, XmNokCallback, (XtCallbackProc) SetValue, dialog);

  n = 0;
  button = XmCreatePushButtonGadget(dialog, "Apply", args, n);
  XtManageChild(button);
  XtAddCallback(button, XmNactivateCallback, 
		(XtCallbackProc) SetValue, dialog);

  /***************** frame1 : XmFrame *****************/
  n = 0;
  XtSetArg(args[n], XmNresizable, True); n++;
  frame1 = XmCreateFrame(dialog, "frame1", args, n);

  /***************** workArea : XmWorkArea *****************/
  n = 0;
  XtSetArg(args[n], XmNx, 0); n++;
  XtSetArg(args[n], XmNy, 0); n++;
  XtSetArg(args[n], XmNcolumns, 3); n++;
  workarea = XmCreateWorkArea(frame1, "workArea", args, n);

  /***************** field2 : XmForm *****************/
  n = 0;
  XtSetArg(args[n], XmNrubberPositioning, True); n++;
  XtSetArg(args[n], XmNresizePolicy, XmRESIZE_ANY); n++;
  field2 = XmCreateForm(workarea, "field2", args, n);

  /***************** l2 : XmLabelGadget *****************/
  n = 0;
  XtSetArg(args[n], XmNx, 31); n++;
  XtSetArg(args[n], XmNy, 28); n++;
  XtSetArg(args[n], XmNwidth, 72); n++;
  XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
  XtSetArg(args[n], XmNresizable, False); n++;
  l2 = XmCreateLabelGadget(field2, "Target", args, n);

  /***************** tf2 : XmCSText *****************/
  n = 0;
  XtSetArg(args[n], XmNresizable, True); n++;
  text2 = XmCreateCSText(field2, "tf2", args, n);

  /***************** FormatMenu : XmPulldownMenu *****************/
  formatmenu = XmCreatePulldownMenu(workarea, "FormatMenu", NULL, 0);

  /***************** C_String : XmPushButtonGadget *****************/
  n = 0;
  format_s = XmCreatePushButtonGadget(formatmenu, "C_String", args, n);

  /***************** LocaleText : XmPushButtonGadget *****************/
  n = 0;
  format_l = XmCreatePushButtonGadget(formatmenu, "LocaleText", args, n);

  /***************** CompoundText : XmPushButtonGadget *****************/
  n = 0;
  format_t = XmCreatePushButtonGadget(formatmenu, "CompoundText", args, n);

  /***************** CompoundString : XmPushButtonGadget *****************/
  n = 0;
  format_cs = XmCreatePushButtonGadget(formatmenu, "CompoundString", args, n);

  /***************** DecInt : XmPushButtonGadget *****************/
  n = 0;
  xms = XmStringCreateLocalized("Integer (Decimal)");
  XtSetArg(args[n], XmNlabelString, xms); n++;
  format_id = XmCreatePushButtonGadget(formatmenu, "DecInt", args, n);
  if (xms) XmStringFree(xms);

  /***************** HexInt : XmPushButtonGadget *****************/
  n = 0;
  xms = XmStringCreateLocalized("Integer (Hex)");
  XtSetArg(args[n], XmNlabelString, xms); n++;
  format_ih = XmCreatePushButtonGadget(formatmenu, "HexInt", args, n);
  if (xms) XmStringFree(xms);

  /***************** FormatOM : XmOptionMenu *****************/
  n = 0;
  xms = XmStringCreateLocalized("Input Format\n\
");
  XtSetArg(args[n], XmNlabelString, xms); n++;
  XtSetArg(args[n], XmNmnemonic, 'I'); n++;
  XtSetArg(args[n], XmNmarginWidth, 10); n++;
  XtSetArg(args[n], XmNsubMenuId, formatmenu); n++;
  format_om = XmCreateOptionMenu(workarea, "FormatOM", args, n);
  if (xms) XmStringFree(xms);

  /***************** field1 : XmForm *****************/
  n = 0;
  XtSetArg(args[n], XmNrubberPositioning, True); n++;
  XtSetArg(args[n], XmNresizePolicy, XmRESIZE_ANY); n++;
  field1 = XmCreateForm(workarea, "field1", args, n);

  /***************** l1 : XmLabelGadget *****************/
  n = 0;
  XtSetArg(args[n], XmNx, 31); n++;
  XtSetArg(args[n], XmNy, 28); n++;
  XtSetArg(args[n], XmNwidth, 72); n++;
  xms = XmStringCreateLocalized("TargetValue");
  XtSetArg(args[n], XmNlabelString, xms); n++;
  XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
  XtSetArg(args[n], XmNresizable, False); n++;
  l1 = XmCreateLabelGadget(field1, "l1", args, n);
  if (xms) XmStringFree(xms);

  /***************** tf1 : XmScrolledCSText *****************/
  n = 0;
  XtSetArg(args[n], XmNrows, 3); n++;
  XtSetArg(args[n], XmNeditMode, XmMULTI_LINE_EDIT); n++;
  text1 = XmCreateScrolledCSText(field1, "tf1", args, n);

  /***************** typePulldown : XmPulldownMenu *****************/
  typepulldown = XmCreatePulldownMenu(workarea, "typePulldown", NULL, 0);

  /***************** b_ATOM : XmPushButtonGadget *****************/
  n = 0;
  xms = XmStringCreateLocalized("ATOM");
  XtSetArg(args[n], XmNlabelString, xms); n++;
  b_atom = XmCreatePushButtonGadget(typepulldown, "b_ATOM", args, n);
  if (xms) XmStringFree(xms);

  /***************** b_DRAWABLE : XmPushButtonGadget *****************/
  n = 0;
  xms = XmStringCreateLocalized("DRAWABLE");
  XtSetArg(args[n], XmNlabelString, xms); n++;
  b_drawable = XmCreatePushButtonGadget(typepulldown, "b_DRAWABLE", args, n);
  if (xms) XmStringFree(xms);

  /***************** b_WINDOW : XmPushButtonGadget *****************/
  n = 0;
  xms = XmStringCreateLocalized("WINDOW");
  XtSetArg(args[n], XmNlabelString, xms); n++;
  b_window = XmCreatePushButtonGadget(typepulldown, "b_WINDOW", args, n);
  if (xms) XmStringFree(xms);

  /***************** b_COLORMAP : XmPushButtonGadget *****************/
  n = 0;
  xms = XmStringCreateLocalized("COLORMAP");
  XtSetArg(args[n], XmNlabelString, xms); n++;
  b_colormap = XmCreatePushButtonGadget(typepulldown, "b_COLORMAP", args, n);
  if (xms) XmStringFree(xms);

  /***************** b_INTEGER : XmPushButtonGadget *****************/
  n = 0;
  xms = XmStringCreateLocalized("INTEGER");
  XtSetArg(args[n], XmNlabelString, xms); n++;
  b_integer = XmCreatePushButtonGadget(typepulldown, "b_INTEGER", args, n);
  if (xms) XmStringFree(xms);

  /***************** b_PIXEL : XmPushButtonGadget *****************/
  n = 0;
  xms = XmStringCreateLocalized("PIXEL");
  XtSetArg(args[n], XmNlabelString, xms); n++;
  b_pixel = XmCreatePushButtonGadget(typepulldown, "b_PIXEL", args, n);
  if (xms) XmStringFree(xms);

  /***************** b_COMPOUND_TEXT : XmPushButtonGadget *****************/
  n = 0;
  xms = XmStringCreateLocalized("COMPOUND_TEXT");
  XtSetArg(args[n], XmNlabelString, xms); n++;
  b_compound_text = 
    XmCreatePushButtonGadget(typepulldown, "b_COMPOUND_TEXT", args, n);
  if (xms) XmStringFree(xms);

  /***************** b_STRING : XmPushButtonGadget *****************/
  n = 0;
  xms = XmStringCreateLocalized("Locale encoding");
  XtSetArg(args[n], XmNlabelString, xms); n++;
  b_locale = XmCreatePushButtonGadget(typepulldown, "b_LOCALE", args, n);
  if (xms) XmStringFree(xms);

  /***************** b_STRING : XmPushButtonGadget *****************/
  n = 0;
  xms = XmStringCreateLocalized("STRING");
  XtSetArg(args[n], XmNlabelString, xms); n++;
  b_string = XmCreatePushButtonGadget(typepulldown, "b_STRING", args, n);
  if (xms) XmStringFree(xms);

  /***************** b_SPAN : XmPushButtonGadget *****************/
  n = 0;
  xms = XmStringCreateLocalized("SPAN");
  XtSetArg(args[n], XmNlabelString, xms); n++;
  b_span = XmCreatePushButtonGadget(typepulldown, "b_SPAN", args, n);
  if (xms) XmStringFree(xms);

  /***************** b_LOCALE_TEXT : XmPushButtonGadget *****************/
  n = 0;
  xms = XmStringCreateLocalized("LOCALE_TEXT");
  XtSetArg(args[n], XmNlabelString, xms); n++;
  b_locale_text = 
    XmCreatePushButtonGadget(typepulldown, "b_LOCALE_TEXT", args, n);
  if (xms) XmStringFree(xms);

  /***************** b__MOTIF_COMPOUND_STRING : XmPushButtonGadget *****************/
  n = 0;
  xms = XmStringCreateLocalized("MOTIF COMPOUND STRING");
  XtSetArg(args[n], XmNlabelString, xms); n++;
  b__motif_compound_string = 
    XmCreatePushButtonGadget(typepulldown, 
			     "b__MOTIF_COMPOUND_STRING", args, n);
  if (xms) XmStringFree(xms);

  /***************** unknown_button : XmPushButtonGadget *****************/
  n = 0;
  xms = XmStringCreateLocalized("unknown");
  XtSetArg(args[n], XmNlabelString, xms); n++;
  unknown_button = XmCreatePushButtonGadget(typepulldown, "b_unknown", 
					    args, n);
  if (xms) XmStringFree(xms);

  /***************** typeMenu : XmOptionMenu *****************/
  n = 0;
  xms = XmStringCreateLocalized("Type");
  XtSetArg(args[n], XmNlabelString, xms); n++;
  XtSetArg(args[n], XmNmnemonic, 'T'); n++;
  XtSetArg(args[n], XmNsubMenuId, typepulldown); n++;
  typeOM = XmCreateOptionMenu(workarea, "typeMenu", args, n);

  if (xms) XmStringFree(xms);

  /***************** buttons : XmWorkArea *****************/
  n = 0;
  XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
  buttons = XmCreateWorkArea(workarea, "buttons", args, n);

  /***************** DeferButton : XmToggleButtonGadget *****************/
  n = 0;
  XtSetArg(args[n], XmNmarginWidth, 20); n++;
  XtSetArg(args[n], XmNindicatorSize, 15); n++;
  XtSetArg(args[n], XmNindicatorOn, XmINDICATOR_CROSS_BOX); n++;
  xms = XmStringCreateLocalized("Defer Target");
  XtSetArg(args[n], XmNlabelString, xms); n++;
  defer = XmCreateToggleButtonGadget(buttons, "Defer", args, n);
  XtManageChild(defer);
  XtAddCallback(defer, XmNvalueChangedCallback, 
		(XtCallbackProc) deferCB, NULL);
  if (xms) XmStringFree(xms);

  /***************** frame1 : XmFrame *****************/
  n = 0;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNleftOffset, 10); n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
  XtSetArg(args[n], XmNtopWidget, frame); n++;
  XtSetArg(args[n], XmNtopOffset, 6); n++;
  XtSetArg(args[n], XmNrightOffset, 10); n++;
  XtSetValues(frame1, args, n);


  /***************** l2 : XmLabelGadget *****************/
  n = 0;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNleftOffset, 10); n++;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNtopOffset, 10); n++;
  XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNbottomOffset, 10); n++;
  XtSetValues(l2, args, n);

  XtManageChild(l2);

  /***************** tf2 : XmCSText *****************/
  n = 0;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNbottomAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNrightOffset, 5); n++;
  XtSetArg(args[n], XmNleftWidget, l2); n++;
  XtSetArg(args[n], XmNleftOffset, 5); n++;
  XtSetArg(args[n], XmNtopPosition, 5); n++;
  XtSetArg(args[n], XmNbottomPosition, 95); n++;
  XtSetValues(text2, args, n);

  XtManageChild(text2);
  XtManageChild(field2);

  XtAddCallback(format_s, XmNactivateCallback, 
		(XtCallbackProc) SetFormat, (XtPointer) 'S');
  XtManageChild(format_s);

  XtAddCallback(format_l, XmNactivateCallback, 
		(XtCallbackProc) SetFormat, (XtPointer) 'L');
  XtManageChild(format_l);

  XtAddCallback(format_t, XmNactivateCallback,
		(XtCallbackProc) SetFormat, (XtPointer) 'T');
  XtManageChild(format_t);

  XtAddCallback(format_cs, XmNactivateCallback, 
		(XtCallbackProc) SetFormat, (XtPointer) 'C');
  XtManageChild(format_cs);

  XtAddCallback(format_id, XmNactivateCallback, 
		(XtCallbackProc) SetFormat, (XtPointer) 'D');
  XtManageChild(format_id);

  XtAddCallback(format_ih, XmNactivateCallback, 
		(XtCallbackProc) SetFormat, (XtPointer) 'H');
  XtManageChild(format_ih);
  XtManageChild(format_om);

  /***************** l1 : XmLabelGadget *****************/
  n = 0;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNleftOffset, 10); n++;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNtopOffset, 10); n++;
  XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNbottomOffset, 10); n++;
  XtSetValues(l1, args, n);

  XtManageChild(l1);

  /***************** tf1 : XmScrolledCSText *****************/
  pn = 0;
  XtSetArg(pargs[pn], XmNleftAttachment, XmATTACH_WIDGET); pn++;
  XtSetArg(pargs[pn], XmNrightAttachment, XmATTACH_FORM); pn++;
  XtSetArg(pargs[pn], XmNtopAttachment, XmATTACH_FORM); pn++;
  XtSetArg(pargs[pn], XmNbottomAttachment, XmATTACH_FORM); pn++;
  XtSetArg(pargs[pn], XmNrightOffset, 5); pn++;
  XtSetArg(pargs[pn], XmNleftWidget, l1); pn++;
  XtSetArg(pargs[pn], XmNleftOffset, 5); pn++;
  XtSetArg(pargs[pn], XmNtopOffset, 5); pn++;
  XtSetArg(pargs[pn], XmNbottomOffset, 5); pn++;
  tmpw = get_constraint_widget(text1, field1);
  if (tmpw)
    XtSetValues(tmpw, pargs, pn);

  XtManageChild(text1);
  XtManageChild(field1);

  XtAddCallback(b_atom,	XmNactivateCallback, 
		(XtCallbackProc) SetType, (XtPointer) "ATOM");
  XtManageChild(b_atom);

  XtAddCallback(b_drawable, XmNactivateCallback, 
		(XtCallbackProc) SetType, (XtPointer) "DRAWABLE");
  XtManageChild(b_drawable);

  XtAddCallback(b_window, XmNactivateCallback, 
		(XtCallbackProc) SetType, (XtPointer) "WINDOW");
  XtManageChild(b_window);

  XtAddCallback(b_colormap, XmNactivateCallback, 
		(XtCallbackProc) SetType, (XtPointer) "COLORMAP");
  XtManageChild(b_colormap);

  XtAddCallback(b_integer, XmNactivateCallback, 
		(XtCallbackProc) SetType, (XtPointer) "INTEGER");
  XtManageChild(b_integer);

  XtAddCallback(b_pixel, XmNactivateCallback, 
		(XtCallbackProc) SetType, (XtPointer) "PIXEL");
  XtManageChild(b_pixel);

  XtAddCallback(b_compound_text, XmNactivateCallback, 
		(XtCallbackProc) SetType, (XtPointer) "COMPOUND_TEXT");
  XtManageChild(b_compound_text);

  XtAddCallback(b_string, XmNactivateCallback, 
		(XtCallbackProc) SetType, (XtPointer) "STRING");
  XtManageChild(b_string);

  XtAddCallback(b_locale, XmNactivateCallback, 
		(XtCallbackProc) SetType, (XtPointer) "");
  XtManageChild(b_locale);

  XtAddCallback(b_span,	XmNactivateCallback, 
		(XtCallbackProc) SetType, (XtPointer) "SPAN");
  XtManageChild(b_span);

  XtAddCallback(b__motif_compound_string, XmNactivateCallback, 
		(XtCallbackProc) SetType,
		(XtPointer) "_MOTIF_COMPOUND_STRING");
  XtManageChild(b__motif_compound_string);

  XtAddCallback(unknown_button,	XmNactivateCallback, 
		(XtCallbackProc) SetType, (XtPointer) "NONE");

  XtManageChild(unknown_button);
  XtManageChild(typeOM);

  XtManageChild(buttons);
  XtManageChild(workarea);
  XtManageChild(frame1);

  return(dialog);
}

Widget CreateHelpDialog(parent, name)
     Widget parent;
     char* name;
{
  Widget help;
  Arg args[10];
  int n;
  XmRendition rend[10];
  XmRenderTable rt;
  int i;
  XmTab tabs[5];
  XmTabList tablist;

  n = 0;
  XtSetArg(args[n], XmdNhelpFile, "transfer"); n++;
  help = XmdCreateHelpDialog(parent, "HelpDialog", args, n);
  /* Get the rendertable and add some new renditions */
  XtVaGetValues(help, XmNrenderTable, &rt, NULL, NULL);
  /* Make the tabs for the tables */
  n = 0;
  tabs[n] = XmTabCreate(1.5, XmINCHES, XmABSOLUTE,
			XmALIGNMENT_BEGINNING, NULL); n++;
  tabs[n] = XmTabCreate(3.0, XmINCHES, XmABSOLUTE,
			XmALIGNMENT_BEGINNING, NULL); n++;
  tabs[n] = XmTabCreate(4.5, XmINCHES, XmABSOLUTE,
			XmALIGNMENT_BEGINNING, NULL); n++;
  tabs[n] = XmTabCreate(6.0, XmINCHES, XmABSOLUTE,
			XmALIGNMENT_BEGINNING, NULL); n++;
  tablist = XmTabListInsertTabs(NULL, tabs, n, 0);
  for(i = 0; i < n; i++) XmTabFree(tabs[i]);

  /* Add extra renditions needed for the help file */
  n = 0; i = 0;
  XtSetArg(args[n], XmNtabList, tablist); n++;
  rend[i] = XmRenditionCreate(help, "table", args, n); i++;
  n = 0;
  XtSetArg(args[n], XmNfontName, "*courier-medium-r-*-12-*"); n++;
  XtSetArg(args[n], XmNfontType, XmFONT_IS_FONT); n++;
  rend[i] = XmRenditionCreate(help, "program", args, n); i++; 
  n = 0;
  XtSetArg(args[n], XmNunderlineType, XmSINGLE_LINE); n++;
  rend[i] = XmRenditionCreate(help, "underline", args, n); i++; 
  /* Make a copy so that setvalues will work correctly */
  rt = XmRenderTableCopy(rt, NULL, 0);
  rt = XmRenderTableAddRenditions(rt, rend, i, XmMERGE_NEW);
  for(n = 0; n < i; n++) XmRenditionFree(rend[n]);

  XtVaSetValues(help, XmNrenderTable, rt, NULL, NULL);
  XmRenderTableFree(rt);

  return(help);
}

void manageCB( widget, w_to_manage, callback_data)
     Widget widget;
     Widget w_to_manage;
     XtPointer callback_data;
{
  if (w_to_manage != (Widget) NULL)
    XtManageChild(w_to_manage);
}

void unmanageCB( widget, w_to_unmanage, callback_data)
     Widget widget;
     Widget w_to_unmanage;
     XtPointer callback_data;
{
  if (w_to_unmanage != (Widget) NULL)
    XtUnmanageChild(w_to_unmanage);
}

