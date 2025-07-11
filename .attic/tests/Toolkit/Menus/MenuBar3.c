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
static char rcsid[] = "$XConsortium: MenuBar3.c /main/7 1995/07/13 18:47:32 drk $"
#endif
#endif


#include <testlib.h>

static void domw ();			/* create a main window */
static void doexit ();			/* exit callback routine */
static void dosomething ();		/* random callback */
static void donothing ();		/* random callback */
static void docookies ();		/* random callback */
static void dohelp ();		        /* random callback */

static Widget mw, mb, pd1, cb1, pb11, pd2, cb2, pb21, pb22, pb23,pb24;

int main (argc, argv)
unsigned int argc;
char **argv;
{

  static Arg args[50];
  static int n;	
  
  CommonTestInit(argc, argv);

  /*
   * creates a mainwindow with a menubar which contains 2 pulldowns
   */

  n = 0;
  XtSetArg(args[n], XmNx, 50); 				n++;
  XtSetArg(args[n], XmNy, 50); 				n++;
  XtSetArg(args[n], XmNborderWidth, 5); 		n++;
  mw = XmCreateMainWindow(Shell1, "mw", args, n);

  n = 0;
  XtSetArg(args[n], XmNx, 50); 				n++;
  XtSetArg(args[n], XmNy, 50); 				n++;
  XtSetValues (Shell1, args, n);

  n = 0;
  mb = XmCreateMenuBar(mw, "mb", args, 0);

  n = 0;
  XtSetArg(args[n], XmNmenuBar, mb);
  XtSetValues(mw, args, n);

  /*
   * pulldown number 1
   */

  n = 0;
  pd1 = XmCreatePulldownMenu(mb, "pd1", args, n);

  n = 0;
  XtSetArg(args[n], XmNsubMenuId, pd1); 			n++;
  XtSetArg(args[n], XmNlabelString, 
	   XmStringCreateSimple("Commands")); 			n++;
  XtSetArg(args[n], XmNmnemonic, 'C'); 				n++;
  cb1 = XmCreateCascadeButton(mb, "cb1", args, n);
  XtManageChild (cb1);

  n = 0;
  XtSetArg(args[n], XmNlabelString, 
	   XmStringCreateSimple("Exit")); 			n++;
  XtSetArg(args[n], XmNmnemonic, 'E'); 				n++;
  XtSetArg(args[n], XmNborderWidth, 0); 			n++;
  XtSetArg(args[n], XmNaccelerator, "Ctrl<Key>e"); 		n++;
  XtSetArg(args[n], XmNacceleratorText, 
	   XmStringCreateSimple("Ctrl+E")); 			n++;
  pb11 = XmCreatePushButton(pd1, "pb11", args, n);
  XtManageChild (pb11);

  XtAddCallback(pb11, XmNactivateCallback, doexit, 0);

  /*
   * pulldown number 2
   */

  n = 0;
  pd2 = XmCreatePulldownMenu(mb, "pd2", args, n);

  n = 0;
  XtSetArg(args[n], XmNsubMenuId, pd2); 			n++;
  XtSetArg(args[n], XmNlabelString, 
	   XmStringCreateSimple("Stuff")); 			n++;
  XtSetArg(args[n], XmNmnemonic, 'S'); 				n++;
  cb2 = XmCreateCascadeButton(mb, "cb2", args, n);
  XtManageChild (cb2);

  n = 0;
  XtSetArg(args[n], XmNlabelString, 
	   XmStringCreateSimple("Nothing")); 	  		n++;
  XtSetArg(args[n], XmNmnemonic, 'N'); 				n++;
  XtSetArg(args[n], XmNborderWidth, 0);				n++;
  XtSetArg(args[n], XmNaccelerator, "Ctrl Shift<Key>n"); 	n++;
  XtSetArg(args[n], XmNacceleratorText, 
	   XmStringCreateSimple("Ctrl+Shift+N")); 		n++;
  pb21 = XmCreatePushButton(pd2, "pb21", args, n);
  XtManageChild(pb21);

  XtAddCallback(pb21, XmNactivateCallback, donothing, (XtPointer) 1);

  n = 0;
  XtSetArg(args[n], XmNlabelString, 
	   XmStringCreateSimple("Something")); 			n++;
  XtSetArg(args[n], XmNmnemonic, 'S'); 				n++;
  XtSetArg(args[n], XmNborderWidth, 0);				n++;
  XtSetArg(args[n], XmNaccelerator, "Shift Ctrl<Key>s"); 	n++;
  XtSetArg(args[n], XmNacceleratorText, 
	   XmStringCreateSimple("Shift+Ctrl+S")); 		n++;
  pb22 = XmCreatePushButton(pd2, "pb22", args, n);
  XtManageChild(pb22);

  XtAddCallback(pb22, XmNactivateCallback, dosomething, (XtPointer) 2);



  n = 0;
  XtSetArg(args[n], XmNlabelString, 
	   XmStringCreateSimple("Cookies")); 			n++;
  XtSetArg(args[n], XmNmnemonic, 'C'); 				n++;
  XtSetArg(args[n], XmNborderWidth, 0); 			n++;
  XtSetArg(args[n], XmNaccelerator, "Ctrl<Key>c"); 		n++;
  XtSetArg(args[n], XmNacceleratorText, 
	   XmStringCreateSimple("Ctrl+C"));			n++;
  pb23 = XmCreatePushButton(pd2, "pb23", args, n);
  XtManageChild(pb23);

  XtAddCallback(pb23, XmNactivateCallback, docookies, (XtPointer) 3);

 /* test case for CR 5201 */

  n = 0;
  XtSetArg(args[n], XmNlabelString, 
	   XmStringCreateSimple("Help")); 			n++;
  XtSetArg(args[n], XmNmnemonic, 'C'); 				n++;
  XtSetArg(args[n], XmNborderWidth, 0); 			n++;
  XtSetArg(args[n], XmNaccelerator, "Ctrl<Key>osfHelp"); 		n++;
  XtSetArg(args[n], XmNacceleratorText, 
	   XmStringCreateSimple("Ctrl+osfHelp"));			n++;
  pb24 = XmCreatePushButton(pd2, "pb24", args, n);
  XtManageChild(pb24);

  XtAddCallback(pb24, XmNactivateCallback, dohelp, (XtPointer) 4);

/* end test for CR 5201 */


  XtManageChild(mb);			/* menu bar */
  XtManageChild(mw);	  	  	/* main window */

  XtRealizeWidget(Shell1);

  CommonPause();
  CommonPause();
  CommonPause();
  CommonPause();  /* Exit */
  XtAppMainLoop(app_context);
}

static void doexit(Widget widget, caddr_t tag, 
		    XmAnyCallbackStruct *callback_data)
{
    
  printf ("Exiting...\n");
  exit (1);

}

static void donothing(Widget widget, XtPointer tag, 
		    XmAnyCallbackStruct *callback_data)
{
  printf ("Doin Nothing\n");
  return;
}

static void dosomething(Widget widget, XtPointer tag,
			 XmAnyCallbackStruct *callback_data)
{
  printf ("Doin Something\n");
  return;
}

static void docookies(Widget widget, XtPointer tag, 
		       XmAnyCallbackStruct *callback_data)
{
  printf ("I want COOKIES!!!!!!!!!!\n");
  return;
}

static void dohelp(Widget widget, XtPointer tag, 
		       XmAnyCallbackStruct *callback_data)
{
  printf ("Help?!? HA!!!!!!!!!!\n");
  return;
}
