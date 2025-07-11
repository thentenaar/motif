/* $XConsortium: TestPerf.c /main/6 1996/10/07 15:13:04 drk $ */
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

/*  Xt headers  */

#include <X11/Intrinsic.h>
#include <X11/Shell.h>

/*  Xm headers  */

#include <Xm/Xm.h>

#define STARTUP_TIME_MESSAGE "_STARTUP_TIME_MESSAGE"

Boolean startup_time = False;	   /* Measure startup time ?? */
Boolean startup_exit = False;	   /* Measure startup time and exit ?? */

void CommonGetTime();
void CommonPrintTime();

void    CommonCheckEvent(XEvent *event, Widget shell);

struct timeval value_time[10];
static int     num_value_time = 0;
static Atom    time_atom = (Atom) 0;

/*************************************************************************
*									 *
*   This module contains routines for testing Startup Time performance.  *
*                                                                        *
*       CommonCheckEvent  : is for checking event. The first time the    *
*                           application receives an Expose event, this   *
*                           will send a Client message, and measure the  *
*                           time.                                        *
*                           When receiving this Client message back      *
*                           the current time is get again, and           *
*                           information on time cumulation is printed.   *
*       CommonGetTime     : is getting the current time, and store it in *
*                           the "value_time" array.                      *
*       CommonPrintTime   : is printing the time difference between each *
*                           time value of the array.                     *
*                                                                        *
*   An application willing to test its startup time, should do the       *
*   following:                                                           *
*                                                                        *
*     - Call CommonGetTime at the beginning                              *
*                                                                        *
*     - Modify its main loop to call CommonCheckEvent, passing the event *
*       and the shell as parameter.                                      *
*                                                                        *
*   A boolean startup_time can be used to conditionalize these calls.    *
*   A boolean startup_exit can be used to exit after printing the        *
*   startup time.                                                        *
*                                                                        * 
*************************************************************************/


void CommonCheckEvent(XEvent *event, Widget shell)
{
    
    if (event->type == Expose) {
	XClientMessageEvent xmessage;
	static Boolean first = True;

	if (!first)
	    return;

	first = False;

	time_atom = XInternAtom(XtDisplay(shell), STARTUP_TIME_MESSAGE, False);

	CommonGetTime();

	xmessage.type = ClientMessage;
	xmessage.serial = 0;
	xmessage.send_event = TRUE;
	xmessage.display = XtDisplay(shell);
	xmessage.window = XtWindow(shell);
	xmessage.message_type = time_atom;
	xmessage.format = 8;
	xmessage.data.b[0] = 'z';

	XSendEvent(XtDisplay(shell), XtWindow(shell),  
		   FALSE, NoEventMask, (XEvent *) &xmessage);    
    }
    else
    
	if (time_atom != (Atom) 0) {
	    XClientMessageEvent *xmessage;

	    xmessage = (XClientMessageEvent *) event;

	    if ((xmessage->type == ClientMessage) && 
		(xmessage->message_type == time_atom)) {
                CommonGetTime();
                CommonPrintTime();
		if (startup_exit)
		    exit(0);
	    }
	}
}

void CommonGetTime()
{
    struct timezone zone_time;

    gettimeofday(&value_time[num_value_time], &zone_time);
    num_value_time++;
}

void CommonPrintTime()
{
     struct timeval first, previous, current, lapsed, cumul;
     int i;
     
     first = value_time[0];

     current = value_time[num_value_time-1];

     if (first.tv_usec > current.tv_usec) {
	 current.tv_usec += 1000000;
	 current.tv_sec--;
     }

     cumul.tv_usec = current.tv_usec - first.tv_usec;
     cumul.tv_sec = current.tv_sec - first.tv_sec;

     printf("\nStartup time : %ld.%06d sec\n",
	    (long)cumul.tv_sec, cumul.tv_usec%1000000);
     fflush(stdout);

/*     for (i = 1; i < num_value_time; i++) {

	 previous = value_time[i-1];
	 current = value_time[i];

	 if (previous.tv_usec > current.tv_usec) {
	     current.tv_usec += 1000000;
	     current.tv_sec--;
	 }

	 lapsed.tv_usec = current.tv_usec - previous.tv_usec;
	 lapsed.tv_sec = current.tv_sec - previous.tv_sec;

	 if (first.tv_usec > current.tv_usec) {
	     current.tv_usec += 1000000;
	     current.tv_sec--;
	 }

	 cumul.tv_usec = current.tv_usec - first.tv_usec;
	 cumul.tv_sec = current.tv_sec - first.tv_sec;

	 printf("\nEllapsed time = %d.%06d sec\t Cumul = %d.%06d sec\n",
                lapsed.tv_sec, lapsed.tv_usec%1000000,
		cumul.tv_sec, cumul.tv_usec%1000000);
     }
*/
}

