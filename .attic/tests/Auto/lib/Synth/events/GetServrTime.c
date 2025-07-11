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
static char rcsid[] = "$XConsortium: GetServrTime.c /main/7 1995/07/14 11:30:11 drk $"
#endif
#endif
/***********************************************************************
  	@(#)GetServrTime.c	1.5.1.1	Date:1/22/91
  	Author: TAT
	History:
            05/29/90 SJS add to sccs
            12/18/90 PSN Remove use of xisSyntheticTime in case of server3
			 bug. This gets rid of Text bugs.
	Calls:

	Summary:
            Gets a good approximation to the current server time in
        milliseconds by reading the last known server time. If this is
        invalid (zero'ed) then go and get current server time using a
        PropertyNotify event.

        INPUTS
            inter_button_time - Time to be sure that passes (func blocks) since 
                                xisLastButtonPressTime prior to returning server
                                time. If 0 then don't delay.
 
        OUTPUTS
            none
 
        RETURNS
            nothing
************************************************************************/

#include "xislib.h"

Time xisGetServerTime(inter_button_time)
Time inter_button_time;
{
    static int first_time = TRUE;
    static Atom *list_prop = (Atom *)NULL;
    static int num_prop = 0;
    static Atom my_property[2];
    static Atom property_type;
    static unsigned char property_value = 0;
    Time old_xisLastEventTime = 0;
    Time new_time;

    if (first_time) {

        if (xisSyncWindow == 0)
            xisProcessEvents(NULL,0);

        my_property[0] = XInternAtom(xisDisplay,"my property",False);
        my_property[1] = XInternAtom(xisDisplay,"your property",False);
        property_type  = XInternAtom(xisDisplay,"INTEGER",True);

	XChangeProperty(xisDisplay, xisSyncWindow, my_property[0], 
			property_type, 8, PropModeReplace, &property_value, 1);
	XChangeProperty(xisDisplay, xisSyncWindow, my_property[1], 
			property_type, 8, PropModeReplace, &property_value, 1);
	list_prop = XListProperties(xisDisplay, xisSyncWindow, &num_prop);

        first_time = FALSE;
    }

    if (!xisUseSyntheticTime) {
        while ( (xisLastEventTime == 0) || 
              (xisLastEventTime < (inter_button_time+xisLastButtonPressTime))) {

            XRotateWindowProperties(xisDisplay, xisSyncWindow, 
		 		    list_prop, num_prop, 1);

            old_xisLastEventTime = xisLastEventTime; 
            xisProcessEvents(NULL,0);

	    /* The following becomes true because of server 3 problem. */
            if (xisLastEventTime == old_xisLastEventTime) {
		xisLastEventTime += inter_button_time + (unsigned)10;
                break;
            }
        }
    } /* End if() */

    /* This statement should never be reached */
    if (xisUseSyntheticTime) {
        new_time = xisLastButtonPressTime + inter_button_time + 10;

        if (xisSyntheticTime > new_time)
            xisSyntheticTime += 10;
        else
            xisSyntheticTime = new_time;
            
        return(xisSyntheticTime);
    }
    else {
        return(xisLastEventTime);
    }

} /* End xisGetServerTime() */
