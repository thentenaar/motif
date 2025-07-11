/* 
 * Motif
 *
 * Copyright (c) 1987-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
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
static char rcsid[] = "$XConsortium: Font1.c /main/6 1995/07/13 19:04:59 drk $"
#endif
#endif
/*
 *  Create labels, buttons, and text widgets with and without fonts specified 
 *  to test the correct search path for default fonts.  This test is not valid
 *  for Motif releases prior to 1.2.
 *  
 *  For BulletinBoard hierarchies:
 *	If the XmNxxxFontList resources are set on the BulletinBoard,
 *         the xxx children inherit that font
 *      If the XmNxxxFontList resources are NOT set, XmDEFAULT_FONT (fixed)
 *         is used.
 * 
 *  For MenuShell and VendorShell hierarchies:
 *	If the XmNxxxFontList resources are set on the Shell,
 *         the xxx children inherit that font
 *      If the XmNxxxFontList resources are NOT set, 
 *          if XmNdefaultFontList is set, use that
 *          else use XmDEFAULT_FONT
 */

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <testlib.h>

int WhichButton = 1;

void PostIt();
void PrintName();

void main(argc, argv)
unsigned int argc;
char **argv;
{ 	
  Widget separator, menushell, bb, bboard, bboard2, rowcolumn1, rowcolumn2;
  Widget rowcolumn3, label[15], fontlabel[5], bulletinboard, rowcol1;
  Widget headerlabel, DialogShell1, DialogShell2, DialogShell3;
  Widget vendorshell, toggle1, pushbutton1, label1, textfield1, drawnbutton1;
  Widget toggle2, pushbutton2, label2, textfield2, drawnbutton2;
  Widget popup[5], popupmenu, pushbuttonnew, poplabel;
  Arg args[10];
  int j = 0, n;
  XmString tcs;
  XmFontList  font0, font1, font2, font3, font4;
  XmStringCharSet  cs = (char *) XmSTRING_DEFAULT_CHARSET;

  /*  initialize toolkit  */
  CommonTestInit(argc, argv);

  /*  Allow the shell to resize */

  n = 0;
  XtSetArg(args[n], XmNallowShellResize, True); n++;
  XtSetValues(Shell1, args, n);


  /*  Load some fonts */

  font0 = CommonGetFontList("fixed");
  font1 = CommonGetFontList("6x10");
  font2 = CommonGetFontList("8x13bold");
  font3 = CommonGetFontList("10x20");
  font4 = CommonGetFontList("8x13");

  /*
   * Display the fonts, for comparison's sake.
   */

  n = 0;
  bb = XmCreateBulletinBoard(Shell1, "popup", args, n);

  n = 0;
  tcs = XmStringLtoRCreate("VARIOUS FONTS USED IN THIS TEST", 
			   XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNx, 10); n++;
  XtSetArg(args[n], XmNy, 10); n++;
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  XtSetArg(args[n], XmNfontList, font3); n++;
  headerlabel = XmCreateLabel(bb, "headerlabel", args, n);
  XtManageChild(headerlabel);
  XmStringFree(tcs);

  n = 0;
  tcs = XmStringLtoRCreate("fixed", XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNx, 30); n++;
  XtSetArg(args[n], XmNy, 60); n++;
  XtSetArg(args[n], XmNfontList, font0);      n++;
  XtSetArg(args[n], XmNlabelString, tcs);	n++;
  fontlabel[0] = XmCreateLabel (bb, "fontlabel0", args, n);
  XtManageChild(fontlabel[0]);
  XmStringFree(tcs);

  n = 0;
  tcs = XmStringLtoRCreate("6x10", XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNx, 30); n++;
  XtSetArg(args[n], XmNy, 110); n++;
  XtSetArg(args[n], XmNfontList, font1);      n++;
  XtSetArg(args[n], XmNlabelString, tcs);	n++;
  fontlabel[1] = XmCreateLabel (bb, "fontlabel1", args, n);
  XtManageChild(fontlabel[1]);
  XmStringFree(tcs);

  n = 0;
  tcs = XmStringLtoRCreate("8x13bold", XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNx, 30); n++;
  XtSetArg(args[n], XmNy, 160); n++;
  XtSetArg(args[n], XmNfontList, font2);      n++;
  XtSetArg(args[n], XmNlabelString, tcs);	n++;
  fontlabel[2] = XmCreateLabel(bb, "fontlabel2", args, n);
  XtManageChild(fontlabel[2]);
  XmStringFree(tcs);

  n = 0;
  tcs = XmStringLtoRCreate("10x20", XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNx, 30); n++;
  XtSetArg(args[n], XmNy, 210); n++;
  XtSetArg(args[n], XmNfontList, font3);      n++;
  XtSetArg(args[n], XmNlabelString, tcs);	n++;
  fontlabel[3] = XmCreateLabel(bb, "fontlabel3", args, n);
  XtManageChild(fontlabel[3]);
  XmStringFree(tcs);

  n = 0;
  tcs = XmStringLtoRCreate("8x13", XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNx, 30); n++;
  XtSetArg(args[n], XmNy, 260); n++;
  XtSetArg(args[n], XmNfontList, font4);      n++;
  XtSetArg(args[n], XmNlabelString, tcs);	n++;
  fontlabel[4] = XmCreateLabel(bb, "fontlabel4", args, n);
  XtManageChild(fontlabel[4]);
  XmStringFree(tcs);

  /* Manage the bulletin Board */

  XtManageChild(bb);
  XtRealizeWidget(Shell1);

  CommonPause();

  /* Create a Popup shell with a bulletin board as a child */

  n = 0; 
  XtSetArg(args[n], XmNallowShellResize, True); n++;
  DialogShell1 = XtCreatePopupShell("DialogShell1", topLevelShellWidgetClass,
				    Shell1, args, n);
  n = 0;
  XtSetArg(args[n], XmNbuttonFontList, font1); n++;
  XtSetArg(args[n], XmNtextFontList, font4); n++;
  bulletinboard = XmCreateBulletinBoard(DialogShell1, "bulletinboard", 
					args, n);

  /*  Create Label, PushButton,ToggleButton and Text Children */

  n = 0;
  tcs = XmStringLtoRCreate("Toggle: Should be 6x10", XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNy, 10); n++;
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  toggle1 = XmCreateToggleButton(bulletinboard, "toggle1", args, n);
  XtManageChild(toggle1);
  XmStringFree(tcs);

  n = 0;
  tcs = XmStringLtoRCreate("Drawn: Should be 6x10", XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNy, 60); n++;
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  XtSetArg(args[n], XmNpushButtonEnabled, True); n++;
  drawnbutton1 = XmCreateDrawnButton(bulletinboard, "draw1", args, n);
  XtManageChild(drawnbutton1);
  XmStringFree(tcs);

  n = 0;
  tcs = XmStringLtoRCreate("Push: Should be 6x10", XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNy, 110); n++;
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  pushbutton1 = XmCreatePushButton(bulletinboard, "push1", args, n);
  XtManageChild(pushbutton1);
  XmStringFree(tcs);

  n = 0;
  tcs = XmStringLtoRCreate("Label: Should be fixed", 
			   XmSTRING_DEFAULT_CHARSET); 
  XtSetArg(args[n], XmNy, 160); n++;
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  label1 = XmCreateLabel(bulletinboard, "label1", args, n);
  XtManageChild(label1);
  XmStringFree(tcs);

  n = 0;
  XtSetArg(args[n], XmNvalue, "Text: Should be 8x13"); n++;
  XtSetArg(args[n], XmNy, 210); n++;
  textfield1 = XmCreateTextField(bulletinboard, "textfield1", args, n);
  XtManageChild(textfield1);
  XtManageChild(bulletinboard);
  XtManageChild(DialogShell1);

  CommonPause();
  XtDestroyWidget(DialogShell1);

  /* Create a Popup shell with a bulletin board as a child */

  n = 0; 
  XtSetArg(args[n], XmNallowShellResize, True); n++;
  DialogShell1 = XtCreatePopupShell("DialogShell1", topLevelShellWidgetClass,
				    Shell1, args, n);
  n = 0;
  XtSetArg(args[n], XmNbuttonFontList, font1); n++;
  XtSetArg(args[n], XmNlabelFontList, font2); n++;
  XtSetArg(args[n], XmNtextFontList, font3); n++;
  bulletinboard = XmCreateBulletinBoard(DialogShell1, "bulletinboard", 
					args, n);

  /*  Create Label, PushButton,ToggleButton and Text Children */

  n = 0;
  tcs = XmStringLtoRCreate("Toggle: Should be 6x10", XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNy, 10); n++;
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  toggle1 = XmCreateToggleButton(bulletinboard, "toggle1", args, n);
  XtManageChild(toggle1);
  XmStringFree(tcs);

  n = 0;
  tcs = XmStringLtoRCreate("Drawn: Should be 6x10", XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNy, 60); n++;
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  XtSetArg(args[n], XmNpushButtonEnabled, True); n++;
  drawnbutton1 = XmCreateDrawnButton(bulletinboard, "draw1", args, n);
  XtManageChild(drawnbutton1);
  XmStringFree(tcs);

  n = 0;
  tcs = XmStringLtoRCreate("Push: Should be 6x10", XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNy, 110); n++;
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  pushbutton1 = XmCreatePushButton(bulletinboard, "push1", args, n);
  XtManageChild(pushbutton1);
  XmStringFree(tcs);

  n = 0;
  tcs = XmStringLtoRCreate("Label: Should be 8x13bold", 
			   XmSTRING_DEFAULT_CHARSET); 
  XtSetArg(args[n], XmNy, 160); n++;
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  label1 = XmCreateLabel(bulletinboard, "label1", args, n);
  XtManageChild(label1);
  XmStringFree(tcs);

  n = 0;
  XtSetArg(args[n], XmNvalue, "Text: Should be 10x20"); n++;
  XtSetArg(args[n], XmNcolumns, strlen("Text: Should be 10x20")); n++;
  XtSetArg(args[n], XmNy, 210); n++;
  textfield1 = XmCreateTextField(bulletinboard, "textfield1", args, n);
  XtManageChild(textfield1);
  XtManageChild(bulletinboard);
  XtManageChild(DialogShell1);

  CommonPause();

  /* 
   * Change the value at setvalues time and then add a kid
   */

  n = 0;
  XtSetArg(args[n], XmNbuttonFontList, font2); n++;
  XtSetValues(bulletinboard, args, n);

  n = 0;
  tcs = XmStringLtoRCreate("Push: Should be 8x13bold",
			   XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNy, 260); n++;
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  pushbuttonnew = XmCreatePushButton(bulletinboard, "pushnew", args, n);
  XtManageChild(pushbuttonnew);
  XmStringFree(tcs);

  CommonPause();

  XtDestroyWidget(bulletinboard);

  /*
   * Create a Popup shell with a menu shell as a child
   */

  n = 0; 
  XtSetArg(args[n], XmNbuttonFontList, font3); n++;
  XtSetArg(args[n], XmNlabelFontList, font2); n++;
  XtSetArg(args[n], XmNorientation, XmVERTICAL); n++;
  popupmenu = XmCreatePopupMenu(DialogShell1, "popupmenu", args, n);

  XtAddEventHandler(DialogShell1, ButtonPressMask, False, PostIt, popupmenu);
  
  tcs = XmStringLtoRCreate("Press MB3 for Popup", XmSTRING_DEFAULT_CHARSET);
  n = 0;
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  poplabel = XmCreateLabel(DialogShell1, "poplabel", args, n);
  XtManageChild(poplabel);
  XmStringFree(tcs);

  /*
   * XmLabelGadget: popup[0]
   */

  popup[0] = XmCreateLabelGadget(popupmenu, "LabelGadget 8X13bold", NULL, 0);

  /*
   * XmSeparatorGadget: popup[1]
   */

  popup[1] = XmCreateSeparatorGadget(popupmenu, "popup1", NULL, 0);

  /*
   * Others: popup[234]
   */

  popup[2] = XmCreatePushButtonGadget(popupmenu, "PushBGadget 10x20", NULL, 0);
  XtAddCallback(popup[2], XmNactivateCallback, PrintName, NULL);

  n = 0;
  popup[3] = XmCreateToggleButton(popupmenu, "ToggleB 10x20", args, n);

  popup[4] = XmCreateToggleButtonGadget(popupmenu, "ToggleBGadget 10x20", 
					NULL, 0);
  XtManageChildren(&(popup[0]), 5);

  
  CommonPause();

  XtDestroyWidget(popupmenu);

  n = 0;
  XtSetArg(args[n], XmNallowShellResize, True); n++;
  XtSetArg(args[n], XmNbuttonFontList, font2); n++;
  XtSetArg(args[n], XmNlabelFontList, font4); n++;  
  XtSetArg(args[n], XmNwidth, 100); n++;
  XtSetArg(args[n], XmNheight, 100); n++;
  menushell = XmCreateMenuShell(DialogShell1, "menushell", args, n);

  n = 0;
  XtSetArg(args[n], XmNorientation, XmVERTICAL); n++;
  XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
  rowcol1 = XmCreateRowColumn(menushell, "rowcol1", args, n);

  /*  Create Label, PushButton,and ToggleButton Children */

  n = 0;
  tcs = XmStringLtoRCreate("Toggle 8x13Bold", XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNy, 10); n++;
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  toggle2 = XmCreateToggleButton(rowcol1, "toggle2", args, n);
  XtManageChild(toggle2);
  XmStringFree(tcs);

  n = 0;
  tcs = XmStringLtoRCreate("Drawn 8x13Bold", XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNy, 60); n++;
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  XtSetArg(args[n], XmNpushButtonEnabled, True); n++;
  drawnbutton2 = XmCreateDrawnButton(rowcol1, "draw2", args, n);
  XtManageChild(drawnbutton2);
  XmStringFree(tcs);

  n = 0;
  tcs = XmStringLtoRCreate("PushButton 8x13Bold", XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNy, 110); n++;
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  pushbutton2 = XmCreatePushButton(rowcol1, "push2", args, n);
  XtManageChild(pushbutton2);
  XmStringFree(tcs);

  n = 0;
  tcs = XmStringLtoRCreate("Label 8x13", XmSTRING_DEFAULT_CHARSET); 
  XtSetArg(args[n], XmNy, 160); n++;
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  label2 = XmCreateLabel(rowcol1, "label2", args, n);
  XtManageChild(label2);
  XmStringFree(tcs);

  XtManageChild(rowcol1);
  XtManageChild(menushell);

  CommonPause();

  XtDestroyWidget(menushell);
  XtDestroyWidget(DialogShell1);

  /* Create a Popup shell with a bulletin board as a child */
  /* Test XmNdefaultFontList for backwards compatibility */

  n = 0; 
  XtSetArg(args[n], XmNdefaultFontList, font2); n++;
  XtSetArg(args[n], XmNallowShellResize, True); n++;
  DialogShell1 = XtCreatePopupShell("DialogShell1", topLevelShellWidgetClass,
				    Shell1, args, n);
  n = 0;
  XtSetArg(args[n], XmNbuttonFontList, font1); n++;
  XtSetArg(args[n], XmNtextFontList, font3); n++;
  bulletinboard = XmCreateBulletinBoard(DialogShell1, "bulletinboard", 
					args, n);

  /*  Create Label, PushButton,ToggleButton and Text Children */

  n = 0;
  tcs = XmStringLtoRCreate("Toggle: Should be 6x10", XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNy, 10); n++;
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  toggle1 = XmCreateToggleButton(bulletinboard, "toggle1", args, n);
  XtManageChild(toggle1);
  XmStringFree(tcs);

  n = 0;
  tcs = XmStringLtoRCreate("Drawn: Should be 6x10", XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNy, 60); n++;
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  XtSetArg(args[n], XmNpushButtonEnabled, True); n++;
  drawnbutton1 = XmCreateDrawnButton(bulletinboard, "draw1", args, n);
  XtManageChild(drawnbutton1);
  XmStringFree(tcs);

  n = 0;
  tcs = XmStringLtoRCreate("Push: Should be 6x10", XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNy, 110); n++;
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  pushbutton1 = XmCreatePushButton(bulletinboard, "push1", args, n);
  XtManageChild(pushbutton1);
  XmStringFree(tcs);

  n = 0;
  tcs = XmStringLtoRCreate("Label: Should be 8x13bold", 
			   XmSTRING_DEFAULT_CHARSET); 
  XtSetArg(args[n], XmNy, 160); n++;
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  label1 = XmCreateLabel(bulletinboard, "label1", args, n);
  XtManageChild(label1);
  XmStringFree(tcs);

  n = 0;
  XtSetArg(args[n], XmNvalue, "Text: Should be 10x20"); n++;
  XtSetArg(args[n], XmNcolumns, strlen("Text: Should be 10x20")); n++;
  XtSetArg(args[n], XmNy, 210); n++;
  textfield1 = XmCreateTextField(bulletinboard, "textfield1", args, n);
  XtManageChild(textfield1);
  XtManageChild(bulletinboard);
  XtManageChild(DialogShell1);

  CommonPause();

  CommonPause(); /* Exit */

  XtAppMainLoop(app_context);
}


void PrintName(Widget w)
{
  fprintf(stdout, "Widget %s was activated\n",
	  XrmQuarkToString(w->core.xrm_name));
}


void PostIt(Widget w, Widget lpopup, XEvent *event, Boolean *c)
{

  XmMenuPosition(lpopup, (XButtonPressedEvent *) event);
  XtManageChild(lpopup);

}
