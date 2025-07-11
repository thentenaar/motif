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
static char rcsid[] = "$XConsortium: Menu2.c /main/10 1995/07/13 18:43:48 drk $"
#endif
#endif

#include <signal.h>

#include <X11/Xlib.h>
#include <X11/cursorfont.h>
#include <X11/StringDefs.h>

#include <Xm/XmP.h>
#include <Xm/RowColumnP.h>
#include <testlib.h>
#include "armPixft.h"
#include "disarmPixft.h"
#include "arrowPixft.h"

/*************Some Globals***********************/

Widget mainw, rc0, rc1, rc2, rc3;
Widget special2, special3, frame1, frame2, frame3;
Widget button1, button2, button3, button4;
Widget submenu1, submenu2, submenu3, submenu4;
Widget option_menu1, option_menu2, option_menu3, option_menu4, rcparent;
Widget popup, help;

Boolean CallbacksEnabled = False;
int WhichButton = 2;
int WAWhichButton = 1;
Pixmap arm = None;
Pixmap disarm = None;
Pixmap arrowPix = None;
unsigned long fg, bg;

void CreatePixmaps();
extern CreateOption();
static XtTranslations translation1;

void _XmRegisterHelpEvent(String event)
{
  static String action1 = ": Help()";
  String bufr;

  if ((event == NULL) || (event[0] == '\0'))
    {
      printf ("_XmRegisterHelpEvent(): NULL event string specified\n");
      return;
    }

  bufr = XtMalloc(strlen(event) + strlen(action1) + 1);
  strcpy(bufr, event);
  strcat(bufr, action1);
  translation1 = XtParseTranslationTable(bufr);
  XtFree(bufr);
}

void _XmBindHelpAction (Widget widget, void (*func)(), XtPointer client_data, 
			Boolean recursive)
{
   int i;
   if (!widget || (!XtIsWidget(widget) && !XmIsGadget(widget)))
      return;

   if (XmIsGadget(widget))
   {
      XtAddCallback(widget, XmNhelpCallback, func, client_data);
      return;
   }

   if (!XtIsShell(widget))
   {
      XtOverrideTranslations(widget, translation1);
      XtAddCallback(widget, XmNhelpCallback, func, client_data);
   }

   if (recursive)
   {
      /* Process normal descendants */
      if (XtIsComposite(widget))
      {
         XmManagerWidget mw = (XmManagerWidget)widget;

         for (i = 0; i < mw->composite.num_children; i++)
         {
            _XmBindHelpAction(mw->composite.children[i], func, client_data, 
                              recursive);
         }
      }

      /* Handle popup descendants */
      for (i = 0; i < widget->core.num_popups; i++)
      {
         _XmBindHelpAction(widget->core.popup_list[i], func, client_data, 
                           recursive);
      }
   }
}

void PrintName(Widget w,
XtPointer d1,
XtPointer d2)
{
  printf ("Widget %s was activated\n", XrmQuarkToString(w->core.xrm_name));
}

/* ARGSUSED */

void PrintName_2(Widget rc, XtPointer client_data, 
		 XtPointer data)
{
   XmRowColumnCallbackStruct *rc_data = (XmRowColumnCallbackStruct *)data ;
  printf ("Widget %s was activated\n", 
	  XrmQuarkToString(rc_data->widget->core.xrm_name));
}

static XtCallbackRec PrintName_CB[] = {
   {PrintName_2, NULL},
   {NULL, NULL}
};

/* ARGSUSED */

void PrintNameAndChangeCursor(Widget rc, XtPointer client_data, 
			      XtPointer data)
{
XmRowColumnCallbackStruct *rc_data = (XmRowColumnCallbackStruct *)data ;
  if (rc_data->data == NULL)
    {
      printf ("Widget %s was activated\n", 
	      XrmQuarkToString(rc_data->widget->core.xrm_name));
    }
  else
    { Cursor cursor = (Cursor)rc_data->data;
      XmSetMenuCursor(XtDisplay(rc), cursor);
      if (XmGetMenuCursor(XtDisplay(rc)) != cursor)
	printf("Error: problem occurred while changing menu cursor\n");
    }
}

static XtCallbackRec PrintNameAndChangeCursor_CB[] = {
   {PrintNameAndChangeCursor, NULL},
   {NULL, NULL}
};


Help(Widget w)
{
  printf("Help requested for widget %s\n", 
	  XrmQuarkToString(w->core.xrm_name));
}

/* ARGSUSED */

PostIt(Widget w, Widget popup, XEvent *event)
{
  Arg args[5];
  Boolean popupEnabled;

  if (popup == NULL)
    return;

  if ((event->type == ButtonPress) && (event->xbutton.button != WhichButton))
    return;

  XtSetArg(args[0], XmNpopupEnabled, &popupEnabled);
  XtGetValues(popup, args, 1);
  if (popupEnabled)
    {
      XmMenuPosition(popup, (XButtonPressedEvent *) event);
      XtManageChild(popup);
    }
}

AddAllTabGroups()
{
   /* Add in correct order */
   XmAddTabGroup(rc1);
   XmAddTabGroup(option_menu1);
   XmAddTabGroup(option_menu2);
   XmAddTabGroup(option_menu3);
   XmAddTabGroup(option_menu4);
   XmAddTabGroup(rcparent);
}

/* ARGSUSED */

void TogglePopupAccelerator(Widget w)
{
  Arg args[2];
  static Boolean set = True;
  if (set == False)
    {
      XtSetArg(args[0], XmNmenuAccelerator, "<Key>F4");
      set = True;
    }
  else
    {
      XtSetArg(args[0], XmNmenuAccelerator, "<Key>F2");
      set = False;
    }
  XtSetValues(popup, args, 1);
}

void ChangeMenuHistory(Widget w,
XtPointer d1,
XtPointer d2)
{
  Arg args[1];
  int n = 0;

  XtSetArg(args[n], XmNmenuHistory, special2); n++;
  XtSetValues(option_menu1, args, n);
  XtSetSensitive(w, False);
}

/* ARGSUSED */

void ChangeMnemonic(Widget w,
XtPointer d1,
XtPointer d2)
{
  KeySym mnemonic;
  Arg args[1];
  int n = 0;

  XtSetArg(args[n], XmNmnemonic, &mnemonic); n++;
  XtGetValues(special3, args, n);

  n = 0;
  if (mnemonic == (KeySym) '\0')
    {
      XtSetArg(args[n], XmNmnemonic, '3'); n++;
    }	
  else
    {
      XtSetArg(args[n], XmNmnemonic, '\0'); n++;
    }
  XtSetValues(special3, args, n);
}

/* ARGSUSED */

void MakeOptionMenuInsensitive(Widget w,
XtPointer d1,
XtPointer d2)
{
  Boolean sensitive;
  Arg args[1];
  int n = 0;

  XtSetArg(args[n], XmNsensitive, &sensitive); n++;
  XtGetValues(option_menu1, args, n);

  if (sensitive)
    {
      XtSetSensitive(option_menu1, False);
    }
  else
    {
      XtSetSensitive(option_menu1, True);
    }
}

/* ARGSUSED */

void ChangeOptionMenuLabel(Widget w,
XtPointer d1,
XtPointer d2)
{
  XmString string;
  Arg args[1];
  int n;

  string = XmStringLtoRCreate("NewMenuLabel ", XmSTRING_DEFAULT_CHARSET);

  n = 0;
  XtSetArg(args[n], XmNlabelString, string); n++;
  XtSetValues(XmOptionLabelGadget(option_menu4), args, n);

  XmStringFree(string);
  XtSetSensitive(w, False);
}

/*************End of CALLBACK PROCEDURES****************/


int main (argc, argv)
unsigned int argc;
char **argv;
{
  Widget mb,tb, pb, menu1, casc1, tb1, pb1, menu2, casc2, tb2, pb2;
  Widget child[20], child2[20];
  Arg args[15];
  int n;
  XmString string;

  CommonTestInit(argc, argv);
  
  /****************************************************************/
  /*  Create the of the top-level frame and row-column manager    */
  /****************************************************************/

  mainw = XmCreateMainWindow(Shell1, "mainw", (ArgList) args, 0);
  XtManageChild(mainw);

  /*
   * Create label pixmaps
   */

  CreatePixmaps();

  /****************************************************************/
  /*  Create the children of the top-level form       manager     */
  /****************************************************************/

  rc0 = XmCreateRowColumn(mainw, "rc0", (ArgList) args, 0);
  XtManageChild(rc0);

  XtSetArg(args[0], XmNworkWindow, rc0);
  XtSetValues(mainw, args, 1);

  /******************************************************/
  /*  Create the children of the row column manager     */
  /******************************************************/

  XtSetArg(args[0], XmNshadowThickness, 2);
  frame1 = (Widget)XmCreateFrame(rc0, "frame1", args, 1);
  XtManageChild(frame1);

  XtSetArg(args[0], XmNpacking, XmPACK_COLUMN);
  XtSetArg(args[1], XmNnumColumns, 4);
  XtSetArg(args[2], XmNadjustLast, False);
  rc1 = XmCreateRowColumn(frame1, "rc1", args, 3);
  XtManageChild(rc1);

  XtSetArg(args[0], XmNshadowThickness, 2);
  frame2 = (Widget)XmCreateFrame(rc0, "frame2", args, 1);
  XtManageChild(frame2);

  XtSetArg(args[0], XmNpacking, XmPACK_COLUMN);
  XtSetArg(args[1], XmNnumColumns, 2);
  rc2 = XmCreateRowColumn(frame2, "rc2", (ArgList) args, 2);
  XtManageChild(rc2);

  XtSetArg(args[0], XmNshadowThickness, 2);
  frame3 = (Widget)XmCreateFrame(rc0, "frame3", args, 1);
  XtManageChild(frame3);

  XtSetArg(args[0], XmNpacking, XmPACK_COLUMN);
  XtSetArg(args[1], XmNnumColumns, 2);
  rc3 = XmCreateRowColumn(frame3, "rc3", (ArgList) args, 2);
  XtManageChild(rc3);

  /****************************************************************/
  /*  Create the widget children of the first row-column manager  */
  /****************************************************************/

  n = 0;
  XtSetArg(args[n], XmNmarginLeft, 11); n++;
  XtSetArg(args[n], XmNmarginRight, 11); n++;

   button1 = XmCreatePushButtonGadget(rc1, "button1", args, n);
   XtAddCallback(button1, XmNactivateCallback, ChangeMenuHistory, NULL);
   XtManageChild(button1);

   button2 = XmCreatePushButtonGadget(rc1, "button2", args, n);
   XtAddCallback(button2, XmNactivateCallback, ChangeMnemonic, NULL);
   XtManageChild(button2);

   button3 = XmCreatePushButtonGadget(rc1, "button3", args, n);
   XtAddCallback(button3, XmNactivateCallback, MakeOptionMenuInsensitive,
		 NULL);
   XtManageChild(button3);

   button4 = XmCreatePushButtonGadget(rc1, "button4", args, n);
   XtAddCallback(button4, XmNactivateCallback, ChangeOptionMenuLabel, NULL);
   XtManageChild(button4);

   /****************************************************************/
   /*  Create the widget children of the 3rd   row-column manager  */
   /****************************************************************/

   CreateOption();

   /****************************************************************/
   /*  Create the widget children of the 4rd   row-column manager  */
   /****************************************************************/

   XtSetArg(args[0], XmNentryCallback, (XtArgVal) PrintName_CB);
   submenu3 = (Widget)XmCreatePulldownMenu(rc3, "submenu3", args, 1);

   XtSetArg(args[0], XmNlabelType, (XtArgVal) XmPIXMAP);
   XtSetArg(args[1], XmNlabelPixmap, (XtArgVal) arm);
   child[0] = XmCreatePushButtonGadget(submenu3, "option1u",args, 2);

   XtSetArg(args[0], XmNlabelType, (XtArgVal) XmPIXMAP);
   XtSetArg(args[1], XmNlabelPixmap, (XtArgVal) disarm);
   child[1] = XmCreatePushButtonGadget(submenu3, "option7u",args, 2);

   XtManageChildren(child, 2);

   string = XmStringCreateLtoR("OptionMenu3: ", XmSTRING_DEFAULT_CHARSET);
   n = 0; 
   XtSetArg(args[n], XmNsubMenuId, submenu3); n++;
   XtSetArg(args[n], XmNmenuHistory, child[1]); n++;
   XtSetArg(args[n], XmNlabelString, string); n++;
   option_menu3 = XmCreateOptionMenu(rc3, "option_menu3", args, n);
   XtManageChild(option_menu3);
   XmStringFree(string);

   XtSetArg(args[0], XmNentryCallback, (XtArgVal) PrintName_CB);
   submenu4 = (Widget)XmCreatePulldownMenu(rc3, "submenu4", args, 1);

   XtSetArg(args[0], XmNlabelType, (XtArgVal) XmPIXMAP);
   XtSetArg(args[1], XmNlabelPixmap, (XtArgVal) arm);
   child[0] = XmCreatePushButtonGadget(submenu4, "option1u2",args, 2);

   XtSetArg(args[0], XmNlabelType, (XtArgVal) XmPIXMAP);
   XtSetArg(args[1], XmNlabelPixmap, (XtArgVal) disarm);
   child[1] = XmCreatePushButtonGadget(submenu4, "option7u2",args, 2);

   XtManageChildren(child, 2);

   string = XmStringCreateLtoR("OptionMenu4: ", XmSTRING_DEFAULT_CHARSET);

   n = 0; 
   XtSetArg(args[n], XmNsubMenuId, submenu4); n++;
   XtSetArg(args[n], XmNmenuHistory, child[0]); n++;
   XtSetArg(args[n], XmNlabelString, string); n++;
   option_menu4 = XmCreateOptionMenu(rc3, "option_menu4", args, n);
   XtManageChild(option_menu4);
   XmStringFree(string);

  /*   
   *  Begin Test PIR 2480
   */

  rcparent = XmCreateRowColumn(rc0, "rc", NULL, 0);
  XtManageChild(rcparent);
  
  n=0;
  XtSetArg(args[n], XmNborderWidth, 0); n++;
  tb = XmCreateToggleButton(rcparent, "ToggleButton1", args, n);
  XtManageChild(tb);

  pb = XmCreatePushButton(rcparent, "PushButton1", args, n);
  XtManageChild(pb);

  mb = XmCreateMenuBar(rcparent, "mb", NULL, 0);
  XtManageChild(mb);

  menu1 = XmCreatePulldownMenu(mb, "menu1", NULL, 0);

  n = 0;
  XtSetArg(args[n], XmNsubMenuId, menu1); n++;
  casc1 = XmCreateCascadeButton(mb, "Buttons", args, n);
  XtManageChild(casc1);

  n=0;
  XtSetArg(args[n], XmNborderWidth, 0); n++;
  tb1 = XmCreateToggleButton(menu1, "ToggleButton2", args, n);
  XtManageChild(tb1);
  pb1 = XmCreatePushButton(menu1, "PushButton2", args, n);
  XtManageChild(pb1);
  tb1 = XmCreateToggleButton(menu1, "ToggleButton3", args, n);
  XtManageChild(tb1);
  pb1 = XmCreatePushButton(menu1, "PushButton3", args, n);
  XtManageChild(pb1);

  menu2 = XmCreatePulldownMenu(mb, "menu2", NULL, 0);
  n = 0;
  XtSetArg(args[n], XmNsubMenuId, menu2); n++;
  casc2 = XmCreateCascadeButton(mb, "Gadgets", args, n);
  XtManageChild(casc2);

  n=0;
  XtSetArg(args[n], XmNborderWidth, 0); n++;
  pb2 = XmCreatePushButtonGadget(menu2, "PushButtonGadget1", args, n);
  XtManageChild(pb2);
  tb2 = XmCreateToggleButtonGadget(menu2, "ToggleGadget1", args, n);
  XtManageChild(tb2);
  pb2 = XmCreatePushButtonGadget(menu2, "PushButtonGadget2", args, n);
  XtManageChild(pb2);
  tb2 = XmCreateToggleButtonGadget(menu2, "ToggleGadget2", args, n);
  XtManageChild(tb2);

  /****************************************************************/
  /*  Realize the widget hierarchy, and enter mainloop processing */
  /****************************************************************/

  AddAllTabGroups();
   
  XtRealizeWidget (Shell1);

/*    _XmRegisterHelpEvent("<Key>F8"); */
/*    _XmBindHelpAction(Shell1, Help, NULL, True); */

  CommonPause();
  CommonPause();
  CommonPause();

  XtDestroyWidget(mainw);
  CommonPause();
  XtAppMainLoop(app_context);
}

void CreatePixmaps()
{
    unsigned int depth;
    Arg	args[2];
    int	n;
    Drawable drawable;
    
    n = 0;
    XtSetArg(args[n], XmNforeground, &fg); n++;
    XtSetArg(args[n], XmNbackground, &bg); n++;
    XtGetValues(mainw, args, n);

    depth = DefaultDepthOfScreen(mainw->core.screen);
    drawable = RootWindowOfScreen(XtScreen(Shell1));
       
    arm = XCreatePixmapFromBitmapData(display,
				      drawable,
				      armPix_bits,
				      armPix_width,
				      armPix_height,
				      fg, bg,
				      depth);

    disarm = XCreatePixmapFromBitmapData(display, 
					 drawable,
					 disarmPix_bits,
					 disarmPix_width,
					 disarmPix_height,
					 fg, bg, 
					 depth);

    arrowPix = XCreatePixmapFromBitmapData(display, 
					   drawable,
					   arrowPix_bits,
					   arrowPix_width,
					   arrowPix_height,
					   fg, bg,
					   depth);
}

CreateOption()
{
   Widget child[20];
   Arg args[10];
   int n;
   XmString string;

   submenu1 = (Widget)XmCreatePulldownMenu(rc2, "submenu1", args, 0);

   child[0] = XmCreatePushButtonGadget(submenu1, "option1a", args, 0);

   child[1] =
   special2 = XmCreatePushButtonGadget(submenu1, "option2a", args, 0);
   child[2] = XmCreatePushButtonGadget(submenu1, "option3a", args, 0);
   child[3] = XmCreatePushButtonGadget(submenu1, "option4a", args, 0);
   child[4] = XmCreatePushButtonGadget(submenu1, "option5a", args, 0);
   child[5] = XmCreatePushButtonGadget(submenu1, "option6a", args, 0);
   child[6] = XmCreatePushButtonGadget(submenu1, "option7a", args, 0);

   XtManageChildren(child, 7);

   string = XmStringCreateLtoR("OptionMenu1: ", XmSTRING_DEFAULT_CHARSET);

   n = 0; 
   XtSetArg(args[n], XmNsubMenuId, submenu1); n++;
   XtSetArg(args[n], XmNmenuHistory, child[4]); n++;
   XtSetArg(args[n], XmNlabelString, string); n++;
   option_menu1 = XmCreateOptionMenu(rc2, "option_menu1", args, n);
   XtManageChild(option_menu1);
   XmStringFree(string);

   submenu2 = (Widget)XmCreatePulldownMenu(rc2, "submenu2", args, 0);

   child[0] = XmCreatePushButtonGadget(submenu2, "option1b", args, 0);
   XtAddCallback(child[0], XmNactivateCallback, PrintName, NULL);

   child[1] = XmCreatePushButtonGadget(submenu2, "option2b", args, 0);
   XtAddCallback(child[1], XmNactivateCallback, PrintName, NULL);

   n = 0;
   XtSetArg(args[n], XmNmnemonic, '3'); n++;
   child[2] = 
   special3 = XmCreatePushButtonGadget(submenu2, "option3b", args, n);
   XtAddCallback(child[2], XmNactivateCallback, PrintName, NULL);

   child[3] = XmCreatePushButtonGadget(submenu2, "option4b", args, 0);
   XtAddCallback(child[3], XmNactivateCallback, PrintName, NULL);

   child[4] = XmCreatePushButtonGadget(submenu2, "option5b", args, 0);
   XtAddCallback(child[4], XmNactivateCallback, PrintName, NULL);

   child[5] = XmCreatePushButtonGadget(submenu2, "option6b", args, 0);
   XtAddCallback(child[5], XmNactivateCallback, PrintName, NULL);

   child[6] = XmCreatePushButtonGadget(submenu2, "option7b", args, 0);
   XtAddCallback(child[6], XmNactivateCallback, PrintName, NULL);

   XtManageChildren(child, 7);

   string = XmStringCreateLtoR("OptionMenu2: ", XmSTRING_DEFAULT_CHARSET);

   n = 0; 
   XtSetArg(args[n], XmNsubMenuId, submenu2); n++;
   XtSetArg(args[n], XmNmenuHistory, child[3]); n++;
   XtSetArg(args[n], XmNlabelString, string); n++;
   option_menu2 = XmCreateOptionMenu(rc2, "option_menu2", args, n);
   XtManageChild(option_menu2);
   XmStringFree(string);
}

