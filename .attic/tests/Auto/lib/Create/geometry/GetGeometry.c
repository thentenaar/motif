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
static char rcsid[] = "$XConsortium: GetGeometry.c /main/8 1995/07/14 10:58:40 drk $"
#endif
#endif
/***********************************************************************
  	@(#)GetGeometry.c	1.8.1.1	Date:1/22/91
  	Author: TAT
	History:
            05/21/90 SJS add to sccs
            06/22/90 SJS do not get geometry of widgets that are
                         being destroyed. (Send back 0 geo)
            07/20/90 PSN Fixed geometry calculation for XmRowColumn parenatage
	Calls:

	Summary:
            Returns the specified widget's position and size on the screen.    
           
            INPUTS:
                widget
        
            OUTPUTS:
                x,y           - position of widget relative to root window in
                                pixels
                width,height  - size of widget in pixels

            RETURNS:
                nothing
************************************************************************/


#include <AutoMessages.h>
#include "mvslib.h"

void mvsGetWidgetGeometry(widget,x,y,width,height,border_width)
Widget widget;
int *x, *y, *width, *height, *border_width;
{
    Window window;
    MvsWidgetInfoRecord *w_info;
    int window_x, window_y, tmp_x, tmp_y;
    Position g_x,g_y;
    Dimension g_h, g_w, g_bw;
    unsigned char g_unit_type;
    Arg args[8];
    int n=0;
    int being_destroyed;
    Window root_return,child_return;
    unsigned int tmp_d;

    if (widget == NULL) {
        AutoMessage(_AutoMessages[WARNMSG61]);
        return;
    }
    else
        window = XtWindowOfObject(widget);

    w_info = mvsWidgetToWidgetInfo(widget);
    if(w_info == NULL)
        being_destroyed = False; /* The widget is just being created and is
                                    not in our hierarchy yet */
    else
        being_destroyed = w_info->being_destroyed;
    
    if(!being_destroyed && XtIsRealized(widget)) {
        if (!XGetGeometry(mvsDisplay,window,&root_return,&tmp_x, &tmp_y,
                          (unsigned int *)width, (unsigned int *)height,
                          (unsigned int *)border_width,
                          (unsigned int *) &tmp_d)) {
            AutoError(_AutoMessages[WARNMSG62]);
            
        }

        window_x = 0;
        window_y = 0;

        if( !(XtIsWidget(widget)) ) {  /* Is this a Gadget? */
            XtSetArg(args[n], XmNheight, &g_h); n++;
            XtSetArg(args[n], XmNwidth, &g_w); n++;
            XtSetArg(args[n], XmNx, &g_x); n++;
            XtSetArg(args[n], XmNy, &g_y); n++;
            XtSetArg(args[n], XmNborderWidth, &g_bw); n++;
	    XtSetArg(args[n], XmNunitType, &g_unit_type); n++;
            XtGetValues(widget, args, n);

/* Automation Added - get proper value in pixels */

	    g_h = XmConvertUnits
			(widget, XmVERTICAL, g_unit_type, g_h, XmPIXELS);
	    g_y = XmConvertUnits
			(widget, XmVERTICAL, g_unit_type, g_y, XmPIXELS);
	    g_w = XmConvertUnits
			(widget, XmHORIZONTAL, g_unit_type, g_w, XmPIXELS);
	    g_x = XmConvertUnits
			(widget, XmHORIZONTAL, g_unit_type, g_x, XmPIXELS);
	    g_bw = XmConvertUnits
			(widget, XmHORIZONTAL, g_unit_type, g_bw, XmPIXELS);

/* End Automation Added */

            window_x += (int)g_x;
            window_y += (int)g_y;

	    if (!mvsIsParentRowColumn(w_info)) {
	    /* This is not added because XmRowColumn adds entryBorder to its
	     * children, which is part of widget's window! - weard! */
            	window_x += *border_width;
            	window_y += *border_width;
	    }

            *width = (int)g_w;
            *height = (int)g_h;
            *border_width = (int)g_bw;
        }

        if (!XTranslateCoordinates(mvsDisplay,window,
                                   DefaultRootWindow(mvsDisplay),
                                   window_x,window_y,x,y,&child_return)) {
            AutoMessage(_AutoMessages[WARNMSG63]);
            exit(-1);
        }
    }
    else {
        *x = *y = *width = *height = *border_width = 0;
    } /* End if(!being_destroyed) */

} /* End mvsGetWidgetGeometry() */
