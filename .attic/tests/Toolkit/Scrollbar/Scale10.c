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
static char rcsid[] = "$XConsortium: Scale10.c /main/8 1995/07/13 19:11:04 drk $"
#endif
#endif

/* 
	The problems: (Pirs 865, 902)

	* setting scaleHeight of horizontal widget shrinks
	  width down to 100; 

	* setting scaleWidth of vertical widget shrinks
	  height down to 100; 
*/


/* Scale set value test */

#include <testlib.h>

Arg args[MAX_ARGS];
int	n;

static void set_scale_height (Widget w, XtPointer client_data, XtPointer
	call_data) 
{
   n = 0;
   XtSetArg(args[n], XmNscaleHeight, 50);  n++;
   XtSetValues((Widget)client_data, args, n);
}


static void set_scale_width(Widget w, XtPointer client_data, XtPointer
	call_data) 
{
   n = 0;
   XtSetArg(args[n], XmNscaleWidth, 50);  n++;
   XtSetValues((Widget)client_data, args, n);
}

static void valueChangedProc(Widget w, XtPointer client_data, XtPointer
	call_data) 
{
	XmScaleCallbackStruct *eventStruct;
	static int oldValue = 0;
	int newValue;

	eventStruct = (XmScaleCallbackStruct *) call_data;
	newValue = eventStruct->value;
	printf ("Old value was %d, new value is %d\n", oldValue, newValue);
	oldValue = newValue;
	fflush (stdout);
}


void main (argc, argv)
int argc;
char **argv;
{
   Widget bb, scale1, scale2;
   Widget pb1, pb2, pb3, pb4;
   Display *display;
   XmString	tcs;


   CommonTestInit(argc, argv);
    

   n=0;
   XtSetArg(args[n], XmNwidth, 455);  n++;
   XtSetArg(args[n], XmNheight, 490);  n++;
   bb = XmCreateBulletinBoard(Shell1, "bb", args, n);
   XtManageChild(bb);

   n = 0;
   XtSetArg(args[n], XmNwidth, 375);  n++;
   XtSetArg(args[n], XmNheight, 75);  n++;
   XtSetArg(args[n], XmNorientation, XmHORIZONTAL);  n++;
   XtSetArg(args[n], XmNshowValue, True);  n++;
   XtSetArg(args[n], XmNvalue, 44);  n++;
   XtSetArg(args[n], XmNscaleWidth, 375);  n++;
   XtSetArg(args[n], XmNscaleHeight, 30);  n++;
   scale1 = XmCreateScale(bb, "scale1", args, n);
   XtManageChild(scale1);

   XtAddCallback(scale1, XmNvalueChangedCallback, 
		 (XtCallbackProc) valueChangedProc, NULL);

   n = 0;
   XtSetArg(args[n], XmNy, 100); n++;
   XtSetArg(args[n], XmNwidth, 75);  n++;;
   XtSetArg(args[n], XmNheight, 375);  n++;;
   XtSetArg(args[n], XmNorientation, XmVERTICAL);  n++;;
   XtSetArg(args[n], XmNshowValue, True); n++;
   XtSetArg(args[n], XmNvalue, 66); n++;
   XtSetArg(args[n], XmNscaleWidth, 30);  n++;;
   XtSetArg(args[n], XmNscaleHeight, 375);  n++;;
   scale2 = XmCreateScale(bb, "scale2", args, n);
   XtManageChild(scale2);

   tcs = XmStringCreateLtoR("Horiz scaleHeight to 50",
	XmSTRING_DEFAULT_CHARSET);
   n = 0;
   XtSetArg(args[n], XmNx, 100);  n++;;
   XtSetArg(args[n], XmNy, 100);  n++;;
   XtSetArg(args[n], XmNlabelString, tcs);  n++;
   pb1 = XmCreatePushButton(bb, "pb1", args, n);
   XtManageChild(pb1);
   XtAddCallback(pb1, XmNactivateCallback,
	set_scale_height, (XtPointer)scale1);
   XmStringFree(tcs);

   tcs = XmStringCreateLtoR("Vert scaleHeight to 50",
	XmSTRING_DEFAULT_CHARSET);
   n = 0;
   XtSetArg(args[n], XmNx, 100);  n++;;
   XtSetArg(args[n], XmNy, 130);  n++;;
   XtSetArg(args[n], XmNlabelString, tcs);  n++;
   pb2 = XmCreatePushButton(bb, "pb2", args, n);
   XtManageChild(pb2);
   XtAddCallback(pb2, XmNactivateCallback,
	set_scale_height, (XtPointer)scale2);
   XmStringFree(tcs);

   tcs = XmStringCreateLtoR("Horiz scaleWidth to 50",
	XmSTRING_DEFAULT_CHARSET);
   n = 0;
   XtSetArg(args[n], XmNy, 100);  n++;;
   XtSetArg(args[n], XmNx, 300);  n++;;
   XtSetArg(args[n], XmNlabelString, tcs);  n++;
   pb3 = XmCreatePushButton(bb, "pb3", args, n);
   XtManageChild(pb3);
   XtAddCallback(pb3, XmNactivateCallback,
	set_scale_width, (XtPointer)scale1);
   XmStringFree(tcs);

   tcs = XmStringCreateLtoR("Vert scaleWidth to 50",
	XmSTRING_DEFAULT_CHARSET);
   n=0;
   XtSetArg(args[n], XmNy, 130);  n++;;
   XtSetArg(args[n], XmNx, 300);  n++;;
   XtSetArg(args[n], XmNlabelString, tcs);  n++;
   pb4 = XmCreatePushButton(bb, "pb4", args, n);
   XtManageChild(pb4);
   XtAddCallback(pb4, XmNactivateCallback,
	set_scale_width, (XtPointer)scale2);
   XmStringFree(tcs);

   XtRealizeWidget(Shell1);

   CommonPause();

   XtAppMainLoop(app_context);
}
