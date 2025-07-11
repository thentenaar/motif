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
static char rcsid[] = "$XConsortium: SimpleM1.c /main/6 1995/07/13 18:39:06 drk $"
#endif
#endif


#include <testlib.h>

int activation_count;

char  *name[] = {  "button 1",     "button 2",     "button 3"};
char  *cname[] = { "cascade 1",    "cascade 2",    "cascade 3"};
char  *pname[] = { "popup push",   "popup toggle", "popup radio",
                   "popup cascade","popup sep",    "popup 2sep",
                   "popup title"};

XmButtonType button_type[] = {XmPUSHBUTTON,XmTOGGLEBUTTON,XmRADIOBUTTON,
				XmCASCADEBUTTON,XmSEPARATOR,
				XmDOUBLE_SEPARATOR,XmTITLE};

Widget simple_pulldown = NULL,
  second_pulldown = NULL, alternate_simple_pulldown = NULL;

/*
 * Toggle the option menu's associated pulldown between 
 * two different pulldown menus 
 */

void change_option_menu(Widget widget, XtPointer option_menu,
			XtPointer call_data)
{
 Arg args[1];
 XtSetArg(args[0],XmNsubMenuId,alternate_simple_pulldown);
 XtSetValues((Widget) option_menu,args,1);

 /* 
  * Desensitize the push button
  */

 XtSetArg(args[0], XmNsensitive, False);
 XtSetValues(widget, args, 1);
}

/* print activation information to stdout */

void simple_activation(Widget widget, XtPointer client_data,
		       XtPointer call_data)
{
 printf("Simple activate called from widget %ld; client data %ld.\n",
	(long int) widget, (long int) client_data);
}

void pop_handler(Widget widget, XtPointer client_data, XEvent *the_event,
		 Boolean *continue_to_dispatch)
{
  printf("Event handler called with ");
  printf("button = %d\n", the_event->xbutton.button);

  if (the_event->xbutton.button == 3)
    {	
      XmMenuPosition(client_data, (XButtonPressedEvent *)the_event);
      XtManageChild((Widget) client_data);
    }
}

/* popup the popup menu passed as client data */

void pop(Widget widget, XtPointer client_data, XtPointer call_data)
{
  printf("Callback Proc called\n");
  XmMenuPosition((Widget) client_data, 
	   (XButtonPressedEvent *) ((XmAnyCallbackStruct *)call_data)->event);
  XtManageChild((Widget) client_data);
}

/* add a cascade button gadget to the menubar passed as client_data */

void add_cascade_gadget(Widget widget, XtPointer client_data, 
			XtPointer call_data)
{
  Arg args[2];
  int n;
  XtSetArg(args[0], XmNsubMenuId, second_pulldown);
  XtManageChild(XmCreateCascadeButtonGadget((Widget) client_data,
					    "new_cascade",
					    args,1));
  n=0;
  XtSetArg(args[n], XmNpostFromButton, 3); n++;
  XtSetArg(args[n], XmNbuttonCount, 3); n++;
  second_pulldown = XmCreateSimplePulldownMenu((Widget) client_data, 
					       "second", args, n);
}

void main(int argc, char **argv)
{
  Widget mainwin,  bboard,  menubar, pb,  om,  popup;
  XmString xmsarray[3];
  XmString pull_xmsarray[3];
  XmString alternate_xmsarray[3];
  XmString radio_xmsarray[3];
  XmString work_xmsarray[3];
  XmString popup_xmsarray[7];
  XmString acceltextarray[3];
  String accelarray[3];
  Arg args[10];
  int n, x;

  CommonTestInit(argc,  argv);

  mainwin = XmCreateMainWindow(Shell1, "mainwin", NULL, 0);
  XtManageChild(mainwin);

  for(x = 0; x < 7; x++) {
    popup_xmsarray[x] = XmStringCreate(pname[x], "");
  }

  for(x = 0; x < 3; x++) {
    char temp[20];
    sprintf(temp, "cascade %d", x);
    xmsarray[x] = XmStringCreate(temp, "");
    sprintf(temp, "pulldown %d", x);
    pull_xmsarray[x] = XmStringCreate(temp, "");
    sprintf(temp, "alternate %d", x);
    alternate_xmsarray[x] = XmStringCreate(temp, "");
    sprintf(temp, "radio %d", x);
    radio_xmsarray[x] = XmStringCreate(temp, "");
    sprintf(temp, "work %d", x);
    work_xmsarray[x] = XmStringCreate(temp, "");
    sprintf(temp, "Ctrl<Key>%d", x);
    accelarray[x] = XtCalloc(strlen(temp)+1, sizeof(char));
    strcpy(accelarray[x], temp);
    sprintf(temp, "Ctrl+%d", x);
    acceltextarray[x] = XmStringCreate(temp, "");
  }

  n=0;
  XtSetArg(args[n], XmNbuttons, xmsarray); n++;
  XtSetArg(args[n], XmNbuttonCount, 3); n++;

  /*add this when certain of mnemonic behavior */
  /*XtSetArg(args[n], XmNbuttonMnemonics, mnemarray); n++;*/

  menubar = XmCreateSimpleMenuBar(mainwin, "menubar", args, n);
  XtManageChild(menubar);

  bboard = XmCreateBulletinBoard(mainwin, "bboard", NULL, 0);
  XtManageChild(bboard);

  n=0;
  XtSetArg(args[n], XmNbuttons, pull_xmsarray); n++;
  XtSetArg(args[n], XmNbuttonCount, 3); n++;
  XtSetArg(args[n], XmNpostFromButton, 1); n++;
  XtSetArg(args[n], XmNbuttonAccelerators, accelarray); n++;
  XtSetArg(args[n], XmNbuttonAcceleratorText, acceltextarray); n++;
  XtSetArg(args[n], XmNsimpleCallback, simple_activation); n++;
  simple_pulldown = XmCreateSimplePulldownMenu(menubar, "simple", args, n);

  n=0;
  XtSetArg(args[n], XmNbuttons, alternate_xmsarray); n++;
  XtSetArg(args[n], XmNbuttonCount, 3); n++;
  XtSetArg(args[n], XmNsimpleCallback, simple_activation); n++;
  alternate_simple_pulldown = 
    XmCreateSimplePulldownMenu(bboard, "alternate_simple", args, n);

  n=0;
  XtSetArg(args[n], XmNbuttons, xmsarray); n++;
  XtSetArg(args[n], XmNbuttonCount, 3); n++;
  XtSetArg(args[n], XmNsimpleCallback, simple_activation); n++;
  XtSetArg(args[n], XmNoptionLabel, XmStringCreate("options", "")); n++;
  XtManageChild(om=XmCreateSimpleOptionMenu(bboard, "option", args, n));

  n=0;
  XtSetArg(args[n], XmNy, 60); n++;
  XtManageChild(pb=XmCreatePushButton(bboard, "Pop Simple Popup Menu 1", 
				      args, n));

  n=0;
  XtSetArg(args[n], XmNbuttons, popup_xmsarray); n++;
  XtSetArg(args[n], XmNbuttonCount, 7); n++;
  XtSetArg(args[n], XmNbuttonType, button_type); n++;
  XtSetArg(args[n], XmNsimpleCallback, simple_activation); n++;
  popup=XmCreateSimplePopupMenu(bboard, "popup", args, n);

  XtAddEventHandler(bboard, ButtonPressMask, FALSE, pop_handler, popup);
  XtAddCallback(pb, XmNactivateCallback, pop, popup);

  n=0;
  XtSetArg(args[n], XmNx, 290); n++;
  XtSetArg(args[n], XmNy, 60); n++;
  XtManageChild(pb=XmCreatePushButton(bboard, "Change Option Menu", args, n));
  XtAddCallback(pb, XmNactivateCallback, change_option_menu, om);

  n=0;
  XtSetArg(args[n], XmNx, 420); n++;
  XtSetArg(args[n], XmNy, 60); n++;
  XtManageChild(pb=XmCreatePushButton(bboard, 
				      "Add CascGadget to Menubar", args, n));
  XtAddCallback(pb, XmNactivateCallback, add_cascade_gadget, menubar);

  n=0;
  XtSetArg(args[n], XmNy, 90); n++;
  XtSetArg(args[n], XmNbuttons, radio_xmsarray); n++;
  XtSetArg(args[n], XmNbuttonCount, 3); n++;
  XtSetArg(args[n], XmNbuttonSet, 1); n++;
  XtSetArg(args[n], XmNsimpleCallback, simple_activation); n++;
  XtManageChild(XmCreateSimpleRadioBox(bboard, "popup", args, n));

  n=0;
  XtSetArg(args[n], XmNy, 90); n++;
  XtSetArg(args[n], XmNx, 150); n++;
  XtSetArg(args[n], XmNbuttons, work_xmsarray); n++;
  XtSetArg(args[n], XmNbuttonCount, 3); n++;
  XtSetArg(args[n], XmNsimpleCallback, simple_activation); n++;
  XtManageChild(XmCreateSimpleCheckBox(bboard, "popup", args, n));

  for(x = 0; x < 7; x++) {
    XmStringFree(popup_xmsarray[x]);
  }

  for(x = 0; x < 3; x++) {
    XmStringFree(xmsarray[x]);
    XmStringFree(pull_xmsarray[x]);
    XmStringFree(alternate_xmsarray[x]);
    XmStringFree(radio_xmsarray[x]);
    XmStringFree(work_xmsarray[x]);
    XtFree(accelarray[x]);
    XmStringFree(acceltextarray[x]);
  }

  XtRealizeWidget(Shell1);

  CommonPause();
  CommonPause();
  CommonPause();

  CommonPause();
  CommonPause();
  CommonPause();

  CommonPause();
  CommonPause();
  CommonPause();

  XtAppMainLoop(app_context);
}











