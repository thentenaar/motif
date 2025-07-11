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
static char rcsid[] = "$XConsortium: PushBtn2.c /main/9 1995/07/13 17:30:29 drk $"
#endif
#endif

#include <signal.h>
#include <X11/Xlib.h>
#include <X11/StringDefs.h>

#include <testlib.h>

static int number = 1;
Widget bboard, pan, push[20];
Widget pushbutton, widget, rc, gadget, pushbutton1, pushbutton2;

void activate_msg(w, client_data, call_data)
Widget w;
XtPointer client_data;
XtPointer call_data; 
{
  int n;
  short show_as_default;
  Arg args[20];

  n=0;
  XtSetArg(args[n], XmNshowAsDefault, &show_as_default);  n++; 
  XtGetValues(w, args, n);

  n=0;
  if (show_as_default == 5) {
      XtSetArg(args[n], XmNshowAsDefault, 10);  n++; 
    }
  else
    {  XtSetArg(args[n], XmNshowAsDefault, 5);  n++;  }
  XtSetValues(w, args, n);
}

static void pushb_cb( w, client_data, call_data )
Widget w;
XtPointer client_data;
XtPointer call_data;
{
  if ((int)client_data == 1) 
    printf("Run push button hit\n" );
  else
    printf("Walk push button hit\n");
     
}

static void generic_callback(widget, tag, callback_data)
Widget widget; 
XtPointer tag;
XtPointer callback_data;
{
  int i;

  if ((int)tag == 0) {
    for (i = 1; i < number; i++) {
      XtDestroyWidget(push[i]);
    }
  number = 1;
  }
}

static void multiClickCB(w, client_data, call_data)
Widget w;
XtPointer client_data,call_data;
{
  printf ("Number of clicks is %d\n", 
	  ((XmPushButtonCallbackStruct*)call_data)->click_count);
  printf ("*************************\n");
}

void main (argc, argv)
unsigned int argc;
char **argv;
{
  Arg args[20], arg;
  XmString string, item_label;
  char s[10];
  int i,n;

  static XtCallbackRec activateCBs[] =
    {
      { activate_msg, NULL },
      { NULL, NULL },
    };

  CommonTestInit(argc, argv);


  /* Begin test for PIR 3469 */

  rc = XmCreateRowColumn(Shell1, "rc", NULL, 0);
  XtManageChild(rc);

  /* change values before the realize */
  XtSetArg(arg, XmNdefaultButtonShadowThickness, 2);
  widget = XmCreatePushButton(rc, "button_widget", &arg, 1);

  XtManageChild(widget);
  
  XtSetArg(arg, XmNdefaultButtonShadowThickness, 2);
  gadget = XmCreatePushButtonGadget(rc, "button_gadget", &arg, 1);
  XtManageChild(gadget);

  XtSetArg(arg, XmNshowAsDefault, True);
  XtSetValues(widget, &arg, 1);
  XtSetValues(gadget, &arg, 1);

  XtRealizeWidget(Shell1);

  CommonPause();
  XtDestroyWidget(rc);

  n=0;
  XtSetArg(args[n], XmNmarginWidth, 20);  n++; 
  XtSetArg(args[n], XmNmarginHeight, 20);  n++; 
  bboard = XmCreateFrame(Shell1, "bboard", args, n);

  n=0;
  XtSetArg(args[n], XmNwidth, 100);  n++; 
  XtSetArg(args[n], XmNheight, 100);  n++; 
  string = XmStringLtoRCreate("Push Here", XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNactivateCallback, activateCBs);  n++; 
  XtSetArg(args[n], XmNlabelType, XmSTRING);  n++; 
  XtSetArg(args[n], XmNlabelString, string);  n++; 
  XtSetArg(args[n], XmNmultiClick, XmMULTICLICK_KEEP); n++;

  pushbutton = XmCreatePushButton(bboard, "pushbutton", args, n); 

  XtAddCallback(pushbutton, XmNactivateCallback, multiClickCB, NULL);

  XmStringFree(string);

  XtManageChild (bboard);
  XtManageChild (pushbutton);

  CommonPause();  /* set multiclick */

  XtRemoveCallbacks (pushbutton, XmNactivateCallback, activateCBs);
  n = 0;
  XtSetArg(args[n], XmNmultiClick, XmMULTICLICK_DISCARD); n++;
  XtSetValues(pushbutton, args, n);

  CommonPause(); /* multiclick KEEP */

  n = 0;
  XtSetArg(args[n], XmNmultiClick, XmMULTICLICK_KEEP); n++;
  XtSetValues(pushbutton, args, n);
  CommonPause();

  XtDestroyWidget(bboard);

  /* Create container form and pushbutton */
  n = 0;
  bboard = XmCreateBulletinBoard(Shell1, "bboard", args, n );
  XtManageChild(bboard);
  item_label = XmStringCreateLtoR( "Run", XmSTRING_DEFAULT_CHARSET);
  n = 0;
  XtSetArg(args[n], XmNlabelString, item_label); n++;
  XtSetArg(args[n], XmNmnemonic, 'R'); n++;
  pushbutton1 = XmCreatePushButtonGadget(bboard, "pushbutton1", args, n);
  XtManageChild(pushbutton1);
  XtAddCallback(pushbutton1, XmNactivateCallback, pushb_cb, (XtPointer) 1);
  XmStringFree(item_label);

  item_label = XmStringCreateLtoR( "Walk", XmSTRING_DEFAULT_CHARSET);
  n = 0;
  XtSetArg(args[n], XmNlabelString, item_label); n++;
  XtSetArg(args[n], XmNmnemonic, 'l'); n++;
  XtSetArg(args[n], XmNy, 150); n++;
  pushbutton2 = XmCreatePushButtonGadget(bboard, "pushbutton2", args, n);
  XtManageChild(pushbutton2);
  XtAddCallback(pushbutton2, XmNactivateCallback, pushb_cb, (XtPointer) 2);


#ifndef MOTIF1_1

  CommonPause();

  /* Begin test for PIR 2404 */

  XtDestroyWidget(bboard);

  pan = XmCreatePanedWindow(Shell1, "pan", NULL, 0);
  XtManageChild(pan);

  number = 20;
  for (i= 0 ; i < number; i++) {
	sprintf(s,"push%d",i);
	push[i] = XmCreatePushButton (pan, s, NULL, 0) ;
	XtAddCallback (push[i], XmNactivateCallback, generic_callback, (XtPointer) i);
	XtManageChild (push[i]);
    }

  /* End test for PIR 2404 */
  
  CommonPause();

  XtDestroyWidget(pan);

#endif /* MOTIF1_1 */

  CommonPause(); /* exit */

  XtAppMainLoop(app_context);
}
