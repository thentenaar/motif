/* $XConsortium: VenShellM1.c /main/5 1995/07/15 21:06:46 drk $ */
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

#include <testlib.h>


Widget  toplevelShell;
Widget  PushButton1 = NULL;
Widget  BBoard1 = NULL;
char *bool_str[2];
char *result_str[2];

static void
Junk(Widget w, XtPointer client_data, XEvent *event, 
	 Boolean *continue_to_dispatch)
{

    printf("Substructure Notify Event Handler (Junk) called.\n");

}


static void
activatePB(Widget w, XtPointer client_data, XtPointer call_data)
{

    Cardinal	n;
    Arg 		args[MAX_ARGS];
	static int count = 0;

    printf("activatePB called.\n");

	if (count == 0) {

		n = 0;
    	XtSetArg(args[n], XmNwidth, 200); n++;
    	XtSetArg(args[n], XmNheight, 200); n++;
    	XtSetValues(PushButton1, args, n);
		count++;

	}

}


static void
ChangeNoResize(Widget w, XtPointer client_data,
                       XtPointer call_data)
{
    Cardinal n;
    Arg      args[MAX_ARGS];

    n = 0;
    XtSetArg(args[n], XmNnoResize, True); n++;
    XtSetValues(BBoard1, args, n);

    XtUnmapWidget(toplevelShell);
    XtMapWidget(toplevelShell);
}   



/* for CR 1684 */
static void 
GetAndPrint(Boolean async_in, Boolean wfw_in, int wTo_in)
{
  Boolean async_out;
  Boolean wfw_out;
  int wTo_out;
  int inval, outval, result;
  Arg args[MAX_ARGS];
  int n;

  n = 0;
  XtSetArg(args[n], XmNuseAsyncGeometry, &async_out); n++;
  XtSetArg(args[n], XmNwaitForWm, &wfw_out); n++;
  XtSetArg(args[n], XmNwmTimeout, &wTo_out); n++;
  XtGetValues(Shell1, args, n);

  inval = async_in ? 1 : 0;
  outval = async_out ? 1 : 0;
  result = (async_in == async_out) ? 1 : 0;
  printf("UseAsyncGeo should be %s, is %s: The test %s\n",
          bool_str[inval], bool_str[outval], result_str[result]);

  inval = wfw_in ? 1 : 0;
  outval = wfw_out ? 1 : 0;
  result = (wfw_in == wfw_out) ? 1 : 0;
  printf("WaitForWm should be %s, is %s: The test %s\n",
          bool_str[inval], bool_str[outval], result_str[result]);

  result = (wTo_in == wTo_out) ? 1 : 0;
  printf("WmTimeout should be %d, is %d: The test %s\n",
          wTo_in, wTo_out, result_str[result]);
}


void
main(unsigned int argc, char **argv)
{
    Cardinal n;
    Arg      args[MAX_ARGS];
    Boolean async_in;  
    Boolean wfw_in;    
    int     wTo_in;
  

    CommonTestInit(argc, argv);
      
    /*
     * test case for PIR 3058 
     * Create a Shell with TWO children - only one is managed.  The
     * SubstructureNotify eventhandler on the shell confuses it when
     * the UNmanaged child is getting resized.  See report for details.
     */
    XtAddEventHandler(Shell1, SubstructureNotifyMask, True, Junk, NULL);

    BBoard1 = XmCreateBulletinBoard(Shell1, "BBoard1", NULL, 0);

    PushButton1 = XmCreatePushButton(Shell1, "PushButton1", NULL, 0);
    XtManageChild(PushButton1);
    
    XtAddCallback(PushButton1, XmNactivateCallback, activatePB, NULL);
    /* end of test case for PIR 3058 */

    XtRealizeWidget(Shell1);


    CommonPause();

    /*
     * This case shows that it works as expected when there isn't
     * an event handler registered for SubstructureNotify (PIR 3058)
     */
    XtRemoveEventHandler(Shell1, SubstructureNotifyMask, True, Junk, NULL);
    XtDestroyWidget(PushButton1);
    XtDestroyWidget(BBoard1);

    BBoard1 = XmCreateBulletinBoard(Shell1, "BBoard1", NULL, 0);

    PushButton1 = XmCreatePushButton(Shell1, "PushButton1", NULL, 0);
    XtManageChild(PushButton1);
    
    XtAddCallback(PushButton1, XmNactivateCallback, activatePB, NULL);

    CommonPause();

/*
 * Text for CR 2940 - Check for changes in BulletinBoard noResize resource.
 */
    XtDestroyWidget(PushButton1);
    XtDestroyWidget(BBoard1);

    n = 0;
    toplevelShell = XtAppCreateShell("topLevelShell", "TopLevelShell",
                                     topLevelShellWidgetClass, display,
                                     args, n);

    BBoard1 = XmCreateBulletinBoard(toplevelShell, "BBoard1", NULL, 0);
    PushButton1 = XmCreatePushButton(BBoard1, "PushButton1", NULL, 0);
    XtManageChild(PushButton1);
    XtAddCallback(PushButton1, XmNactivateCallback, ChangeNoResize, NULL);

    XtManageChild(BBoard1);
    XtRealizeWidget(toplevelShell);

    CommonPause();

    XtDestroyWidget (toplevelShell);
    /* make output on term window look nicer */
    printf ("\n");
    printf ("\n");
    printf ("\n");

    CommonPause();

   /* begin test for CR 1684 */

    bool_str[0] = "False";
    bool_str[1] = "True";
    result_str[0] = "Failed";
    result_str[1] = "Passed";


    /* Test 1 - Initialization */
  async_in = True;
  wfw_in = False;
  wTo_in = 0;
  
  n=0;
  XtSetArg(args[n], XmNuseAsyncGeometry, async_in); n++;
  XtSetValues (Shell1, args, n);
  
  n=0;
  BBoard1 = XmCreateBulletinBoard(Shell1, "BBoard1", args, n);

  n=0;
  PushButton1 = XmCreatePushButton(BBoard1, "PushButton1", args, n);

  printf("Test1: Initialize UseAsyncGeo to True\n");
  GetAndPrint(async_in, wfw_in, wTo_in);

/* Test 2 - Using SetValues, return UseAsyncGeo to False */
  async_in = False;
  wfw_in = True;
  wTo_in = 5000;
  n = 0;
  XtSetArg(args[n], XmNuseAsyncGeometry, async_in); n++;
  XtSetValues(Shell1, args, n);

  printf("\nTest 2: Change UseAsyncGeo to False using SetValues\n");
  GetAndPrint(async_in, wfw_in, wTo_in);

/* Test 3 - Using SetValues, return UseAsyncGeo to True */
  async_in = True;
  wfw_in = False;
  wTo_in = 0;
  n = 0;
  XtSetArg(args[n], XmNuseAsyncGeometry, async_in); n++;
  XtSetValues(Shell1, args, n);
 
  printf("\nTest 3: Change UseAsyncGeo to True using SetValues\n");
  GetAndPrint(async_in, wfw_in, wTo_in);

/* Test 4 - Using SetValues, change UseAsyncGeo to True and WaitForWm
            to False and WmTimeout to 1000 */
  async_in = False;
  wfw_in = False;
  wTo_in = 1000;
  n = 0;
  XtSetArg(args[n], XmNuseAsyncGeometry, False); n++;
  XtSetArg(args[n], XmNwaitForWm, False); n++;
  XtSetArg(args[n], XmNwmTimeout, 1000); n++;
  XtSetValues(Shell1, args, n);

  printf("\nTest 4: Change UseAsyncGeo to False, WaitForWm to False and WmTimeout to 1000 using SetValues\n");
  GetAndPrint(async_in, wfw_in, wTo_in);

   CommonPause();

/* Test 5 - Using SetValues, change UseAsyncGeo to True, WaitForWm to Ture,
            and WmTimeout to 2000. */
  async_in = True;
  wfw_in = False;
  wTo_in = 0;
  n = 0;
  XtSetArg(args[n], XmNuseAsyncGeometry, async_in); n++;
  XtSetArg(args[n], XmNwaitForWm, True); n++;
  XtSetArg(args[n], XmNwmTimeout, 2000); n++;
  XtSetValues(Shell1, args, n);

  printf("\nTest 5: Change UseAsyncGeo to True, WaitForWm to True, and WmTimeout to 2000 using SetValues\n");
  GetAndPrint(async_in, wfw_in, wTo_in);

/* Test 6 - Using SetValues, change UseAsyncGeo to False to check the
            results of Test 5 */
  async_in = False;
  wfw_in = True;
  wTo_in = 2000;
  n = 0;
  XtSetArg(args[n], XmNuseAsyncGeometry, async_in); n++;
  XtSetValues(Shell1, args, n);

  printf("\nTest 6: Change UseAsyncGeo to False to verify values loaded in Test 5 using SetValues\n");
  GetAndPrint(async_in, wfw_in, wTo_in);

/* Test 7 - Using SetValues, change UseAsyncGeo to True. Then change WaitForWm 
            to False and WmTimeout to 1000. */
  async_in = True;
  wfw_in = False;
  wTo_in = 0;
  n = 0;
  XtSetArg(args[n], XmNuseAsyncGeometry, async_in); n++;
  XtSetValues(Shell1, args, n);

  n = 0;
  XtSetArg(args[n], XmNwaitForWm, False); n++;
  XtSetArg(args[n], XmNwmTimeout, 1000); n++;
  XtSetValues(Shell1, args, n);

  printf("\nTest 7: Change UseAsyncGeo to True.  Then change WaitForWm to False, and WmTimeout to 1000 afterwards\n");
  GetAndPrint(async_in, wfw_in, wTo_in);

/* Test 8 - Using SetValues, change UseAsyncGeo to False to check the 
            previously loaded data */
  async_in = False;
  wfw_in = False;
  wTo_in = 1000;
  n = 0;
  XtSetArg(args[n], XmNuseAsyncGeometry, async_in); n++;
  XtSetValues(Shell1, args, n);

  printf("\nTest 8: Change UseAsyncGeo to False to check the previously loaded data\n");
  GetAndPrint(async_in, wfw_in, wTo_in);

/* Test 9 - Using SetValues, change WaitForWm to True, and WmTimeout to 2000. */
  async_in = False;
  wfw_in = True;
  wTo_in = 2000;
  n = 0;
  XtSetArg(args[n], XmNwaitForWm, True); n++;
  XtSetArg(args[n], XmNwmTimeout, 2000); n++;
  XtSetValues(Shell1, args, n);

  printf("\nTest 9: Change WaitForWm to True and WmTimeout to 2000 while UseAsyncGeo is False using SetValues\n");
  GetAndPrint(async_in, wfw_in, wTo_in);

  /* End test for CR 1684 */

  CommonPause();
  XtAppMainLoop(app_context);

}
