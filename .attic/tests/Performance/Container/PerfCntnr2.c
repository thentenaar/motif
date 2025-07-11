/* $XConsortium: PerfCntnr2.c /main/6 1996/10/07 15:10:48 drk $ */
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
 * 
 */
/*
 * HISTORY
 */
#include <stdio.h>
#include <sys/time.h>

#include <testlib.h>
#include <math.h>

#define NUM_OBJ 500

void IncrBottomCB(Widget, XtPointer, XtPointer);
void DecrTopCB(Widget, XtPointer, XtPointer);
void IncrRightCB(Widget, XtPointer, XtPointer);
void DecrLeftCB(Widget, XtPointer, XtPointer);

static void     PrintLapseTime();

struct timeval	incr_to_bottom_time[2];
struct timeval	decr_to_top_time[2];
struct timeval	incr_to_right_time[2];
struct timeval	decr_to_left_time[2];

void Quit(Widget w, XtPointer client_data, XtPointer call_data)
{

	printf("Time elapsed to scroll to the bottom:\n");
	PrintLapseTime(&incr_to_bottom_time[0], &incr_to_bottom_time[1]);
	printf("Time elapsed to scroll to the top:\n");
	PrintLapseTime(&decr_to_top_time[0], &decr_to_top_time[1]);
	printf("Time elapsed to scroll to the right:\n");
	PrintLapseTime(&incr_to_right_time[0], &incr_to_right_time[1]);
	printf("Time elapsed to scroll to the left:\n");
	PrintLapseTime(&decr_to_left_time[0], &decr_to_left_time[1]);

	printf("Begin exiting ... please standby ... \n");
	exit(0);

}

void
main(int argc, char **argv)
{
    
    Widget		ScrolledW, Frame, Container, *IconGad;
    int			i,j;
    Cardinal		n, num_obj;
    Arg			args[MAX_ARGS];
    Cardinal            test_num;
    XmString            icon_name;
    char IconName[20] ;
	Widget	VertScrollB, HorizScrollB;
	int	VertMax, HorizMax, VertMin, HorizMin, VertIncr, HorizIncr;

    CommonTestInit(argc, argv);

    test_num = 0;
    num_obj = NUM_OBJ;
    
    if (UserData != NULL) {
	
	if (strcmp(UserData, "noscroll") == 0)
	    test_num = 1;
	else {
	    n = atoi(UserData);
	    if (n < num_obj)
	      num_obj = n;
	}

	free(UserData);
	
    }

    n = 0;
    XtSetArg(args[n], XmNheight, 360); n++;
    XtSetArg(args[n], XmNwidth, 320); n++;
    XtSetValues(Shell1, args, n);

    /* We want a scrolledwindow because the container window
       can potentially be huge. If we don't want a scrolled 
       window run it with -u noscroll */

    if (test_num != 1) {

       n = 0;
       XtSetArg(args[n], XmNscrollingPolicy, XmAUTOMATIC); n++;
       ScrolledW = XmCreateScrolledWindow(Shell1, "ScrolledW", args, n);
       XtManageChild(ScrolledW);
    
       /* but we don't want interaction with the scrolling header */
       n = 0;
       Frame= XmCreateFrame(ScrolledW, "Frame", args, n);
       XtManageChild(Frame);
    
       n = 0;
       Container= XmCreateContainer(Frame, "Container", args, n);
	   VertScrollB = XtNameToWidget(ScrolledW, "VertScrollBar");
	   HorizScrollB = XtNameToWidget(ScrolledW, "HorScrollBar");

	   XtAddCallback(VertScrollB, XmNincrementCallback, IncrBottomCB, (XtPointer)&VertMax);
	   XtAddCallback(VertScrollB, XmNdecrementCallback, DecrTopCB, (XtPointer)&VertMin);
	   XtAddCallback(HorizScrollB, XmNincrementCallback, IncrRightCB, (XtPointer)&HorizMax);
	   XtAddCallback(HorizScrollB, XmNdecrementCallback, DecrLeftCB, (XtPointer)&HorizMin);
    }
    else {
       n = 0;
       Container= XmCreateContainer(Shell1, "Container", args, n);
    }
      
    XtManageChild(Container);
	XtAddCallback(Container, XmNselectionCallback, Quit, NULL);
    
    IconGad = (Widget*) XtMalloc(num_obj*sizeof(Widget));

    /* this one creates a binary tree labelled 0, 00 01, etc */
    sprintf(IconName,"IconGad0");
    for (i = 0; i < num_obj; i++) {
	
	n = 0 ;
	j = (i+1)/2 - 1 ;
	if (i) {
	    XtSetArg(args[n], XmNentryParent, IconGad[j]); n++;
	    sprintf(IconName,"%s%d", XtName(IconGad[j]), i%2);
	} 
	
	icon_name = XmStringCreate(IconName, XmSTRING_DEFAULT_CHARSET); 
	XtSetArg(args[n], XmNlabelString, icon_name); n++;
	IconGad[i] = XmCreateIconGadget(Container, IconName, args, n);
	XmStringFree(icon_name);
    }

    XtManageChildren(IconGad, num_obj);

    XtRealizeWidget(Shell1);

	incr_to_bottom_time[0].tv_sec = 0;
	decr_to_top_time[0].tv_sec = 0;
	incr_to_bottom_time[0].tv_sec = 0;
	incr_to_bottom_time[0].tv_sec = 0;

    CommonPause();

    XtAppMainLoop(app_context);
    
}

void IncrBottomCB(Widget w, XtPointer client_data, XtPointer call_data)
{
	struct	timezone	tz;
	XmScrollBarCallbackStruct *cb = (XmScrollBarCallbackStruct *)call_data;

	/* printf("cb->value = %d\n", cb->value); */
	if (cb->value == 33875) {
		gettimeofday(&incr_to_bottom_time[1],&tz);
	} else {
		if (incr_to_bottom_time[0].tv_sec == 0)
			gettimeofday(&incr_to_bottom_time[0],&tz);
	}
}

void DecrTopCB(Widget w, XtPointer client_data, XtPointer call_data)
{
	struct  timezone	tz;
	XmScrollBarCallbackStruct *cb = (XmScrollBarCallbackStruct *)call_data;

	/* printf("cb->value = %d\n", cb->value); */
	if (cb->value == 0) {
		gettimeofday(&decr_to_top_time[1],&tz);
	} else {
		if (decr_to_top_time[0].tv_sec == 0)
			gettimeofday(&decr_to_top_time[0],&tz);
	}

}

void IncrRightCB(Widget w, XtPointer client_data, XtPointer call_data)
{

	struct	timezone	tz;
	XmScrollBarCallbackStruct *cb = (XmScrollBarCallbackStruct *)call_data;

	/* printf("cb->value = %d\n", cb->value); */
	if (cb->value == 753) {
		gettimeofday(&incr_to_right_time[1],&tz);
	} else {
		if (incr_to_right_time[0].tv_sec == 0)
			gettimeofday(&incr_to_right_time[0],&tz);
	}

}

void DecrLeftCB(Widget w, XtPointer client_data, XtPointer call_data)
{
	struct	timezone	tz;
	XmScrollBarCallbackStruct *cb = (XmScrollBarCallbackStruct *)call_data;

	/* printf("cb->value = %d\n", cb->value); */
	if (cb->value == 0) {
		gettimeofday(&decr_to_left_time[1],&tz);
	} else {
		if (decr_to_left_time[0].tv_sec == 0)
			gettimeofday(&decr_to_left_time[0],&tz);
	}

}

static void PrintLapseTime(first,second)
struct timeval *first, *second;
{
     struct timeval lapsed;

     if (first->tv_usec > second->tv_usec) {
        second->tv_usec += 1000000;
        second->tv_sec--;
     }

     lapsed.tv_usec = second->tv_usec - first->tv_usec;
     lapsed.tv_sec = second->tv_sec - first->tv_sec;

     printf("lapsed time = %ld.%06d sec\n\n",
            (long)lapsed.tv_sec, lapsed.tv_usec%1000000);
     fflush(stdout);

}
