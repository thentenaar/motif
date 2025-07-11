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
static char rcsid[] = "$XConsortium: MenuBar1.c /main/9 1995/07/13 18:46:39 drk $"
#endif
#endif

#include <X11/Xlib.h>
#include <X11/StringDefs.h>
#include <X11/IntrinsicP.h>
#include <Xm/XmP.h>
#include <testlib.h>

#define	NEW_MENU_ACCELERATOR "<Key>F9"

Widget newcb = NULL, newcbg = NULL, newms = NULL, newrc = NULL,
  newrc_label = NULL, separator = NULL, menubar = NULL, new_menu = NULL,
  beagle = NULL, corgi = NULL, collie = NULL, more = NULL, 
  unspec_pixmap = NULL, more2 = NULL, more3 = NULL, move_submenu = NULL;

Widget menu1 = NULL, menu2 = NULL, menu3 = NULL, menu4 = NULL, 
  submenu1 = NULL, submenu2 = NULL, submenu3 = NULL, submenu4 = NULL,
  submenu5 = NULL, submenu6 = NULL,
  invalid_submenu = NULL, mine = NULL, yours = NULL, ours = NULL, 
  toggle_RC = NULL, toggle_MS = NULL, toggle_CB = NULL, toggle_CBG = NULL,
  toggle_ML;

Boolean	ownms, longlabel=FALSE;   

XmStringCharSet	CharSet = (XmStringCharSet) XmSTRING_DEFAULT_CHARSET;

void PrintName(Widget w,
XtPointer d1,
XtPointer d2)
{
  fprintf(stdout, "Widget %s was activated\n",
	  XrmQuarkToString(w->core.xrm_name));
}

void AddSubmenu(Widget cb, XtPointer submenu, XtPointer call_data)
{
  Arg args[2];
  int n;

  n = 0;
  XtSetArg(args[n], XmNsubMenuId, submenu); n++;
  XtSetValues(cb, args, n);
  XtRemoveCallback(cb, XmNcascadingCallback, AddSubmenu, submenu);
}

void ShadowThickness(Widget w,
XtPointer d1,
XtPointer d2)
{
  short	shadow;
  Arg  args[10];
  int n;

  n = 0;
  XtSetArg(args[n], XmNshadowThickness, &shadow); n++;
  XtGetValues(w, args, n);

  if (shadow > 10)
    {
      shadow = 2;
    }
  else
    {
      shadow = 12;
    }
    
  n = 0;
  XtSetArg(args[n], XmNshadowThickness, shadow); n++;
  XtSetValues(w, args, n);
}

void ToggleCb(Widget w,
XtPointer d1,
XtPointer d2)
{
  if (newcb)
    {
      XtDestroyWidget(newcb);
      newcb = NULL;
    }
  else
    {
      if (newrc)
	{
	  newcb = XmCreateCascadeButton(newrc, "New CascadeButton", NULL, 0);
	  XtManageChild(newcb);
	}
    }	
}

void ToggleCbg(Widget w,
XtPointer d1,
XtPointer d2)
{
  if (newcbg)
    {
      XtDestroyWidget(newcbg);
      newcbg = NULL;
    }
  else
    {
      if (newrc)
	{
	  newcbg = XmCreateCascadeButtonGadget(newrc,
					       "New CascadeButtonGadget",
					       NULL, 0);
	  XtAddCallback(newcbg, XmNactivateCallback, ShadowThickness, NULL);
	  XtManageChild(newcbg);
	}
    }	
}

void ToggleRc(Widget w,
XtPointer d1,
XtPointer d2)
{
  XmString string;
  Arg args[10];
  int n;
   
  if (newrc)
    {
      XtDestroyWidget(newrc);
      newrc = NULL;
      newrc_label = NULL;
      newcb = NULL;
      newcbg = NULL;
    }
  else
    {
      if (newms)
	{
	  n = 0;
	  XtSetArg(args[n], XmNrowColumnType, XmMENU_PULLDOWN); n++;
	  newrc = XmCreateRowColumn(newms, "newrc", args, n);
	}
      else
	{
	  newrc = XmCreatePulldownMenu(menubar, "newrc", NULL, 0);
	  newms = XtParent(newrc);
	  ownms = FALSE;
	}

      string = XmStringLtoRCreate("New Menu", CharSet);

      n = 0;
      XtSetArg(args[n], XmNlabelString, string); n++;
      newrc_label = XmCreateLabel(newrc, "new menu", args, n);
      XtManageChild(newrc_label);
      XmStringFree (string);

      separator = XmCreateSeparatorGadget(newrc, "separator", NULL, 0);
      XtManageChild(separator);

      n = 0;
      XtSetArg(args[n], XmNsubMenuId, newrc); n++;
      XtSetValues(new_menu, args, n);
    }
}


void ToggleMs(Widget w,
XtPointer d1,
XtPointer d2)
{
  Arg args[10];
  int n;
   
  if (newms)
    {
      if (ownms)
	{
	  XtDestroyWidget(newms);
	  newms = NULL;
	  newrc = NULL;
	  newcb = NULL;
	  newcbg = NULL;
	}
      else
	{
	  fprintf(stdout, "Can't destroy private ms\n");
	}
    }
  else
    {
      if (menubar)
	{
	  n = 0;
	  XtSetArg(args[n], XmNwidth, 5); n++;
	  XtSetArg(args[n], XmNheight, 5); n++;
	  newms = XmCreateMenuShell(menubar, "newms", args, n);
	  ownms = TRUE;
	}
    }
}

void InvalidSubmenu(Widget w,
XtPointer d1,
XtPointer d2)
{
  Arg args[1];
  int	n;

  if (newcbg)
    {
      n = 0;
      XtSetArg(args[n], XmNsubMenuId, newcbg); n++;
      XtSetValues(newcbg, args, n);
    }
  else
    {
      fprintf(stdout, "CascadeButtonGadget not yet created \n");
    }
}

void MoveCascade(Widget w, XtPointer newsubmenu, XtPointer d2)
{
  XmRowColumnWidget submenu;
  Arg args[1];
  int n;
   
  n = 0;
  XtSetArg(args[n], XmNsubMenuId, &submenu); n++;
  XtGetValues(more2, args, n);

  n = 0;
  XtSetArg(args[n], XmNsubMenuId, newsubmenu); n++;
  if (submenu == NULL)
    {
      XtSetValues(more2, args, n);
      n = 0;
      XtSetArg(args[n], XmNsubMenuId, NULL); n++;
      XtSetValues(more3, args, n);
    }
  else
    {
      XtSetValues(more3, args, n);
      n = 0;
      XtSetArg(args[n], XmNsubMenuId, NULL); n++;
      XtSetValues(more2, args, n);
    }
}

void UnspecifiedPixmap(Widget w,
XtPointer d1,
XtPointer d2)
{
  Arg args[1];
  int n = 0;

  XtSetArg(args[n], XmNcascadePixmap, XmUNSPECIFIED_PIXMAP); n++;
  XtSetValues(more, args, n);
}

void ToggleML(Widget w,
XtPointer d1,
XtPointer d2)
{
  Arg args[10];
  int n;
  XmString tcs,tcs2;
   
  if (longlabel) {
    tcs = XmStringLtoRCreate("Menu3", CharSet);
    tcs2 = XmStringLtoRCreate("Menu4", CharSet);
    longlabel = FALSE;
  }
  else {
    tcs = XmStringLtoRCreate("LongMenuLabel3", CharSet);
    tcs2 = XmStringLtoRCreate("LongMenuLabel4", CharSet);
    longlabel = TRUE;
  }

  n = 0;
  XtSetArg(args[n], XmNlabelString, tcs);		n++;
  XtSetValues(menu3, args, n);

  n = 0;
  XtSetArg(args[n], XmNlabelString, tcs2);		n++;
  XtSetValues(menu4, args, n);

  XmStringFree(tcs);
  XmStringFree(tcs2);
}

int main(int argc, char *argv[])
{
  Arg args[20];
  XmString tcs, tcs1, tcs2;
  char *accelerator_string;
  int n;

  CommonTestInit(argc, argv);
    
  /*
   * XmMenuBar: menubar
   */

  accelerator_string = XtMalloc(sizeof(NEW_MENU_ACCELERATOR) + 1);
  strcpy(accelerator_string, NEW_MENU_ACCELERATOR);
   
  n = 0;
  XtSetArg(args[n], XmNmenuAccelerator, accelerator_string);	n++;
  XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
  menubar = XmCreateMenuBar(Shell1, "menubar", args, n);
  XtManageChild(menubar);

  /* Test code for Pir2692 */
  XtFree(accelerator_string);

  n = 0;
  XtSetArg(args[n], XmNmenuAccelerator, &accelerator_string);	n++;
  XtGetValues(menubar, args, n);

  printf("XmNmenuAccelerator for MenuBar is %s\n", accelerator_string);
  fflush(stdout);
  /* End Test code for Pir2692 */

  /*
   * XmPulldownMenu: submenu1
   */
   
  n = 0; 
  submenu1 = XmCreatePulldownMenu(menubar, "submenu1", args, n);

  /*
   * XmPushButton: toggle_MS
   */

  n = 0; 
  tcs = XmStringLtoRCreate("Toggle MenuShell", CharSet);
  tcs1 = XmStringCreate("Ctrl-M", XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  XtSetArg(args[n], XmNaccelerator, "Ctrl<Key>M"); n++;
  XtSetArg(args[n], XmNacceleratorText, tcs1); n++;
  toggle_MS = XmCreatePushButton(submenu1, "toggle_MS", args, n);
  XtAddCallback(toggle_MS, XmNactivateCallback,  ToggleMs, NULL);
  XtManageChild(toggle_MS);
  XmStringFree(tcs1);
  XmStringFree(tcs);
   
  /*
   * XmPushButton: toggle_RC
   */

  n = 0; 
  tcs = XmStringLtoRCreate("Toggle RowColumn", CharSet);
  tcs1 = XmStringCreate("Ctrl-R", XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  XtSetArg(args[n], XmNaccelerator, "Ctrl<Key>R"); n++;
  XtSetArg(args[n], XmNacceleratorText, tcs1); n++;
  toggle_RC = XmCreatePushButton(submenu1, "toggle_RC", args, n);
  XtAddCallback(toggle_RC, XmNactivateCallback,  ToggleRc, NULL);
  XtManageChild(toggle_RC);
  XmStringFree(tcs);
  XmStringFree(tcs1);

  /*
   * XmPushButton: toggle_CB
   */

  n = 0; 
  tcs = XmStringLtoRCreate("Toggle CascadeButton", CharSet);
  tcs1 = XmStringCreate("Ctrl-C", XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  XtSetArg(args[n], XmNaccelerator, "Ctrl<Key>C"); n++;
  XtSetArg(args[n], XmNacceleratorText, tcs1); n++;
  toggle_CB = XmCreatePushButton(submenu1, "toggle_CB", args, n);
  XtAddCallback(toggle_CB, XmNactivateCallback,  ToggleCb, NULL);
  XtManageChild(toggle_CB);
  XmStringFree(tcs);
  XmStringFree(tcs1);
   
  /*
   * XmPushButton: toggle_CBG
   */

  n = 0; 
  tcs = XmStringLtoRCreate("Toggle CascadeButtonGadget", CharSet);
  tcs1 = XmStringCreate("Ctrl-G", XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  XtSetArg(args[n], XmNaccelerator, "Ctrl<Key>G"); n++;
  XtSetArg(args[n], XmNacceleratorText, tcs1); n++;
  toggle_CBG = XmCreatePushButton(submenu1, "toggle_CBG", args, n);
  XtAddCallback(toggle_CBG, XmNactivateCallback,  ToggleCbg, NULL);
  XtManageChild(toggle_CBG);
  XmStringFree(tcs);
  XmStringFree(tcs1);

  /*
   * XmPulldownMenu: submenu2
   */
   
  n = 0; 
  submenu2 = XmCreatePulldownMenu(menubar, "submenu2", args, n);

  /*
   * XmPulldownMenu: submenu4
   */

  n = 0; 
  submenu4 = XmCreatePulldownMenu(submenu2, "submenu4", args, n);

  /*
   * XmCascadeButton: more2
   */

  n = 0; 
  tcs = XmStringLtoRCreate("More2", CharSet);
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  more2 = XmCreateCascadeButton(submenu2, "more2", args, n);
  XtAddCallback(more2, XmNcascadingCallback,  AddSubmenu, (XtPointer) submenu4);
  XtManageChild(more2);
  XmStringFree(tcs);

  /*
   * XmCascadeButton: more3
   */

  n = 0; 
  tcs = XmStringLtoRCreate("More3", CharSet);
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  more3 = XmCreateCascadeButton(submenu2, "more3", args, n);
  XtManageChild(more3);
  XmStringFree(tcs);

  /*
   * XmPushButtonGadget: move_submenu
   */

  n = 0; 
  tcs = XmStringLtoRCreate("Move Submenu", CharSet);
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  move_submenu = XmCreatePushButtonGadget(submenu2, "move_submenu", args, n);
  XtAddCallback(move_submenu, XmNactivateCallback, MoveCascade, submenu4);
  XtManageChild(move_submenu);
  XmStringFree(tcs);

  /*
   * XmPushButtonGadget: beagle
   */

  n = 0; 
  tcs = XmStringLtoRCreate("Beagle", CharSet);
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  beagle = XmCreatePushButtonGadget(submenu4, "beagle", args, n);
  XtAddCallback(beagle, XmNactivateCallback,  PrintName, NULL);
  XtManageChild(beagle);
  XmStringFree(tcs);

  /*
   * XmPushButtonGadget: corgi
   */

  n = 0; 
  tcs = XmStringLtoRCreate("Corgi", CharSet);
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  corgi = XmCreatePushButtonGadget(submenu4, "corgi", args, n);
  XtAddCallback(corgi, XmNactivateCallback,  PrintName, NULL);
  XtManageChild(corgi);
  XmStringFree(tcs);

  /*
   * XmPushButtonGadget: collie
   */

  n = 0; 
  tcs = XmStringLtoRCreate("Collie", CharSet);
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  collie = XmCreatePushButtonGadget(submenu4, "collie", args, n);
  XtAddCallback(collie, XmNactivateCallback,  PrintName, NULL);
  XtManageChild(collie);
  XmStringFree(tcs);

  /*
   * XmPulldownMenu: submenu3
   */
   
  n = 0; 
  submenu3 = XmCreatePulldownMenu(menubar, "submenu3", args, n);

  /*
   * XmPushButton: invalid_submenu
   */

  n = 0; 
  tcs = XmStringLtoRCreate("Invalid Submenu", CharSet);
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  invalid_submenu = XmCreatePushButton(submenu3, "invalid_submenu", args, n);
  XtAddCallback(invalid_submenu, XmNactivateCallback, InvalidSubmenu, NULL);
  XtManageChild(invalid_submenu);
  XmStringFree(tcs);

  /*
   * XmPushButton: unspec_pixmap
   */

  n = 0; 
  tcs = XmStringLtoRCreate("Unspecified Pixmap", CharSet);
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  unspec_pixmap = XmCreatePushButton(submenu3, "unspec_pixmap", args, n);
  XtAddCallback(unspec_pixmap, XmNactivateCallback, UnspecifiedPixmap, NULL);
  XtManageChild(unspec_pixmap);
  XmStringFree(tcs);

  /*
   * XmPulldownMenu: submenu5
   */
   
  n = 0; 
  submenu5 = XmCreatePulldownMenu(submenu3, "submenu5", args, n);

  /*
   * XmPushButtonGadget: mine
   */

  n = 0; 
  tcs = XmStringLtoRCreate("Mine", CharSet);
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  mine = XmCreatePushButtonGadget(submenu5, "mine", args, n);
  XtManageChild(mine);
  XmStringFree(tcs);

  /*
   * XmPushButtonGadget: yours
   */

  n = 0; 
  tcs = XmStringLtoRCreate("Yours", CharSet);
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  yours = XmCreatePushButtonGadget(submenu5, "yours", args, n);
  XtManageChild(yours);
  XmStringFree(tcs);

  /*
   * XmPushButtonGadget: ours
   */

  n = 0; 
  tcs =  XmStringLtoRCreate("Ours", CharSet);
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  ours = XmCreatePushButtonGadget(submenu5, "ours", args, n);
  XtManageChild(ours);
  XmStringFree(tcs);

  /*
   * XmCascadeButton: more
   */

  n = 0; 
  tcs = XmStringLtoRCreate("More", CharSet);
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  XtSetArg(args[n], XmNsubMenuId, submenu5); n++;
  more = XmCreateCascadeButton(submenu3, "more", args, n);
  XtManageChild(more);
  XmStringFree(tcs);

  /*
   * XmPulldownMenu: submenu6  (4th menu in menu bar)
   */
   
  n = 0; 
  submenu6 = XmCreatePulldownMenu(menubar, "submenu6", args, n);

  /*
   * XmPushButton: toggle_ML  (toggle Menu Labels)
   */

  n = 0; 
  tcs = XmStringLtoRCreate("Toggle Menu Labels", CharSet);
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  toggle_ML = XmCreatePushButton(submenu6, "toggle_ML", args, n);
  XtAddCallback(toggle_ML, XmNactivateCallback, ToggleML, NULL);
  XtManageChild(toggle_ML);
  XmStringFree(tcs);

  /*
   * XmCascadeButton: menu1
   */

  n = 0; 
  tcs = XmStringLtoRCreate("Menu1", CharSet);
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  XtSetArg(args[n], XmNsubMenuId, submenu1); n++;
  menu1 = XmCreateCascadeButton(menubar, "menu1", args, n);
  XtManageChild(menu1);
  XmStringFree(tcs);

  /*
   * XmCascadeButton: menu2
   */

  n = 0; 
  tcs =  XmStringLtoRCreate("Menu2", CharSet);
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  XtSetArg(args[n], XmNsubMenuId, submenu2); n++;
  menu2 = XmCreateCascadeButton(menubar, "menu2", args, n);
  XtManageChild(menu2);
  XmStringFree(tcs);

  /*
   *  XmCascadeButton: menu3
   */

  n = 0; 
  tcs = XmStringLtoRCreate("Menu3", CharSet);
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  XtSetArg(args[n], XmNsubMenuId, submenu3); n++;
  menu3 = XmCreateCascadeButton(menubar, "menu3", args, n);
  XtManageChild(menu3);
  XmStringFree(tcs);

  /*
   *  XmCascadeButton: menu4
   */

  n = 0; 
  tcs = XmStringLtoRCreate("Menu4", CharSet);
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  XtSetArg(args[n], XmNsubMenuId, submenu6); n++;
  menu4 = XmCreateCascadeButton(menubar, "menu4", args, n);
  XtManageChild(menu4);
  XmStringFree(tcs);

  /*
   * XmCascadeButton: new_menu
   */

  /* put in to test PIR785 underlining in multisegment 
     compound strings  */

  tcs = XmStringLtoRCreate("New", CharSet);
  tcs1 = XmStringLtoRCreate("Menu", CharSet);
  tcs2 = XmStringConcat(tcs, tcs1);
   
  n = 0; 
  XtSetArg(args[n], XmNlabelString, tcs2);    n++;
  XtSetArg(args[n], XmNmnemonic, 'e');        n++;
  new_menu = XmCreateCascadeButton(menubar, "new_menu", args, n);
  XtManageChild(new_menu);

  XmStringFree(tcs);
  XmStringFree(tcs1);
  XmStringFree(tcs2);

  XtRealizeWidget(Shell1);

  CommonPause();
  CommonPause();
  CommonPause();
  CommonPause();
  CommonPause();
  CommonPause();  /* 2098 test */

  /* 
   * Begin Test code for PIR 2814.  Vertical menu bars should have
   *  pulldown menus which cascade to the side.
   */

  n = 0;
  XtSetArg(args[n], XmNorientation, XmVERTICAL); n++;
  XtSetValues(menubar, args, n);
  
  CommonPause();
    
  XtDestroyWidget(menubar);
  CommonPause();
  XtAppMainLoop(app_context);
}










