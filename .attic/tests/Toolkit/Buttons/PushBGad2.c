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
static char rcsid[] = "$XConsortium: PushBGad2.c /main/8 1995/07/13 17:29:25 drk $"
#endif
#endif


#include <X11/Xlib.h>
#include <X11/StringDefs.h>
#include <testlib.h>

void activate_msg(w, client_data, call_data)
Widget w;
XtPointer client_data;
XtPointer call_data; 
{
  int n;
  short show_as_default;
  Arg args[20];

  n=0;
  XtSetArg(args[n], XmNshowAsDefault, &show_as_default); n++; 
  XtGetValues(w, args, n);

  n=0;
  if (show_as_default == 5)
    {
      XtSetArg(args[n], XmNshowAsDefault,10); n++; 
    }
  else
    {
      XtSetArg(args[n], XmNshowAsDefault, 5); n++; 
    }
  XtSetValues(w, args, n);
}

void main (argc, argv)
unsigned int argc;
char **argv;

{
  Widget bboard;
  Widget pushbutton;
  Arg args[20];
  XmString string;
  int n;

  /* callback lists */

  static XtCallbackRec activateCBs[] =
    {
      { activate_msg, NULL },
      { NULL, NULL },
    };

  CommonTestInit(argc, argv);
    
  n=0;
  XtSetArg(args[n], XmNwidth, 200); n++;
  XtSetArg(args[n], XmNheight,200); n++;
  bboard = XmCreateFrame(Shell1,"bboard", args, n);

  n=0;
  XtSetArg(args[n], XmNwidth,100); n++; 
  XtSetArg(args[n], XmNheight,100); n++; 
  string = XmStringLtoRCreate("Push Here", XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNactivateCallback, activateCBs); n++; 
  XtSetArg(args[n], XmNlabelType, XmSTRING); n++; 
  XtSetArg(args[n], XmNlabelString, string); n++; 
  pushbutton = XmCreatePushButtonGadget(bboard,"pushbutton" ,args,n); 

  XtManageChild (bboard);
  XtManageChild (pushbutton);

  XtRealizeWidget (Shell1);
   
  CommonPause(); /* now change background color of manager */

  n = 0;
  XtSetArg(args[n], XmNbackground, CommonGetColor("yellow")); n++;
  XtSetValues(bboard, args, n);

  CommonPause();

  XtAppMainLoop(app_context);
  XmStringFree(string);
  XtDestroyWidget(bboard);
  XtDestroyWidget(pushbutton);
}
