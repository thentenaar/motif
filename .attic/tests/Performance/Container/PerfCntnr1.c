/* $XConsortium: PerfCntnr1.c /main/5 1995/07/15 21:01:00 drk $ */
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
#include <math.h>

#define NUM_OBJ 700

void
main(int argc, char **argv)
{
    
    Widget		ScrolledW, Frame, Container, *IconGad;
    int			i,j;
    Cardinal		n, N, num_obj;
    Arg			args[MAX_ARGS];
    Cardinal            test_num;
    XmString            icon_name;

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
    }
    else {
       n = 0;
       Container= XmCreateContainer(Shell1, "Container", args, n);
    }
      
    XtManageChild(Container);
    
    /* take the cube root of num_obj and compute the real num_obj */
    N = (Cardinal) cbrt ((double)num_obj) ;
    num_obj = N*N*N ; 
    IconGad = (Widget*) XtMalloc(num_obj*sizeof(Widget));


    for (i = 0; i < num_obj; i++) {
	char IconName[20] ;

	n = 0 ;
	if (i >= N && i < N*N) {
	    j = i/N - 1 ;
	    XtSetArg(args[n], XmNentryParent, IconGad[j]); n++;
	    sprintf(IconName,"%s%d", XtName(IconGad[j]),(i%N)+1);
	} else if (i > N) {
	    j = i/N ;
	    XtSetArg(args[n], XmNentryParent, IconGad[j]); n++;
	    sprintf(IconName,"%s%d", XtName(IconGad[j]),(i%N)+1);
	} else {
	    sprintf(IconName, "IconGad%d", i+1);
	}

	icon_name = XmStringCreate(IconName, XmSTRING_DEFAULT_CHARSET); 
	XtSetArg(args[n], XmNlabelString, icon_name); n++;
	IconGad[i] = XmCreateIconGadget(Container, IconName, args, n);
    }
    XtManageChildren(IconGad,num_obj);

    
    XtRealizeWidget(Shell1);
    
    CommonPause();

    XtAppMainLoop(app_context);
    
}


