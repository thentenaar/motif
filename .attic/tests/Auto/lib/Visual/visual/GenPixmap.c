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
static char rcsid[] = "$XConsortium: GenPixmap.c /main/8 1995/07/14 11:49:52 drk $"
#endif
#endif
/***********************************************************************
  	@(#)GenPixmap.c	1.6.1.1	Date:1/22/91
  	Author: TAT
	History:
            05/24/90 SJS add to sccs
	Calls:

	Summary:
            Generates one known good pixmap.

        INPUTS:
            widget - the widget whose image needs generating.
        
        OUTPUTS:
            none

        RETURNS:
            nothing

        FILE FORMAT:

            4 byte file start code
            4 byte beginning of rec id table
            ----- Begin Image Records ----
            4 byte image record start code
            4 byte checksum
            2 byte width
            2 byte height
            n byte compressed image (n=width*height)
        
            4 byte image record start code
            4 byte checksum
            2 byte width
            2 byte height
            n byte compressed image (n=width*height)
               :
               :
            ----- Begin Rec Id Table ----
            2 byte num record ids

            2 byte rec id record start code
            2 byte rec id length = n
            4 byte image file position
            n byte rec id

            2 byte rec id record start code
            2 byte rec id length = n
            4 byte image file position
            n byte rec id

               :
               :
************************************************************************/

#include <AutoMessages.h>
#include "vislib.h"
#include "mvslib.h"

extern XtAppContext app_context;


static void CheckNormalizedColors(MvsWidgetInfoRecord *widget_info);

void mvsGeneratePixmap(widget_info)
MvsWidgetInfoRecord *widget_info;
{
    int i,j,x,y;
    Widget widget;
    XImage *image;
    Window tmp_window;
    static int first_time = 1;
    unsigned int checksum;
    unsigned char *bp;  /* byte pointer */
    unsigned int width,height;
    Window root,child;
    unsigned int depth;
    unsigned int border_width;

    widget = widget_info->widget;

    if (first_time) {
        first_time = 0;
        mvsNumRecIds = 0;
    }

/*
    See if we already have this visual state 

    Don't need to calculate hash - always compare - Automation
    ---------------------------------------------
*/

    i = mvsNumRecIds;

    /* no, don't have this visual state's pixmap */

        if (mvsNumRecIds >= MAX_RECORD_IDS)
            AutoMessage(_AutoMessages[VISMSG11]);
        else {
            mvsNumRecIds++;

            /* no we don't have this visual state... go get it */

            tmp_window = XtWindowOfObject(widget);


	    /* Be sure all colors are normalized */

	    CheckNormalizedColors(widget_info);


            /* Get the image (assumed with normalized colors) */

            XGetGeometry(visDisplay, tmp_window, &root,&x,&y,&width,&height,
                         &border_width,&depth);

            width += 2*border_width;
            height += 2*border_width;


            XTranslateCoordinates(visDisplay, tmp_window,root,
				  -border_width,
				  -border_width,
                                  &x,&y,&child);

            image = XGetImage( visDisplay,XDefaultRootWindow(visDisplay),
                               x,y,width,height,AllPlanes, ZPixmap   );

            if (image == 0) {
                AutoError(_AutoMessages[VISMSG12]);

            }

            /* copy to image buffer and calculate check_sum */

            checksum = 0;
            bp = &mvsImageBuffer[0];
            for (y=0; y<height; y++) {
                for (x=0; x<width; x++) {
                    *bp = XGetPixel(image,x,y);

/* Automation - dont look at color. Would require a lookup of widget ID
   and then conversion to correct pixel */

                    checksum += (x+1)*(y+1)*(*bp);
                    bp++;
                }
            }

            mvsVisualChecksum[i] = checksum;

	    if (image != NULL) 
            	XDestroyImage(image);

            /* check to see if we have record with identical checksum */

/* 
   Automation - Even if visual image is the same still store. Up to writer
   to resolve extra storages.  
*/

	    j = i;

            if (j < i) {
                VIS_trace("#### This image has same checksum as that for recid #%d to make recid #%d\n",j,i);
            }
            else { /* no, couldn't find pixmap; generate one */
                static image_count = 0;
                mvsFilePos[i] = ftell(mvsImageFd);
                fwrite_long(mvsImageFd,mvsImageRecordStartCode);
                fwrite_long(mvsImageFd,checksum);
                fwrite_short(mvsImageFd,width);
                fwrite_short(mvsImageFd,height);
                mvsCompressWrite(mvsImageFd,mvsImageBuffer,width*height);
                VIS_trace("#### This is unique image #%d in unique recid #%d\n",
                          image_count++,i);
            }
        }
/* 
 	Automation Fix

    }

*/
} /* End mvsGeneratePixmap() */

static void CheckNormalizedColors(MvsWidgetInfoRecord *widget_info)
{
    MvsWidgetInfoRecord *winfo;
    MvsWidgetClassInfo *widget_class_info;
    MvsResourceInfoRecord *resource_info;
    int num_res_colors;
    short *res_color_ref;
    int i;
    Widget widget;
    short done, need_to_wait;
    Arg color_args[10];
    Cardinal num_color_args;
    MvsWidgetInfoRecord *widget_info_stack[100];
    short stack_top = 0;

    /* Go down the hierarchy of widgets, and check that each current value 
       of a Color resource is normalized */

    winfo = widget_info;
    need_to_wait = 0;

    done = 0;
    while (!done) {

	/* Get the Color resource names associated with this widget class */

	widget_class_info = winfo->widget_class_info;
	resource_info = widget_class_info->resource_info;
	num_res_colors = widget_class_info->num_res_colors;
	res_color_ref = widget_class_info->res_color_ref;
	widget = winfo->widget;
	num_color_args = 0;

	for (i=0; i<num_res_colors; i++) {
	  if (res_color_ref[i] >= 0) {
	      if ( (Pixel) mvsGetResourceValue(widget, 
				    resource_info[res_color_ref[i]].name, 
				    T_Pixel) 
                   != winfo->unique_color_ref[i]) {
		  
		  XtSetArg(color_args[num_color_args],
			   resource_info[res_color_ref[i]].name,
			   winfo->unique_color_ref[i]);
	          num_color_args++;
               }	  
	  }
	}
	
	if (num_color_args != (Cardinal) 0) {
	    XtSetValues(widget, color_args, num_color_args);
	    need_to_wait = 1;
	}
	        
        if (winfo->first_child != NULL) {
            widget_info_stack[stack_top++] = winfo;
            winfo = (MvsWidgetInfoRecord *)winfo->first_child;
        }
        else 
            if (winfo->next_sibling != NULL)
                winfo = (MvsWidgetInfoRecord *)winfo->next_sibling;
            else {
                winfo = NULL;
                while (stack_top > 0 && winfo == NULL)
                    winfo = (MvsWidgetInfoRecord *)
                             widget_info_stack[--stack_top]->next_sibling;

                if (stack_top == 0)
                    done = 1;
            } 
    } /* End while(!done) */    

    /* Be sure the SetValues is going to happen */

    if (need_to_wait) {
	XEvent event;

	XSync(mvsDisplay, False);
	while (XtAppPending(app_context)) {
	    XtAppNextEvent(app_context, &event);
	    XtDispatchEvent(&event);
	}	
    }
    
} /* End CheckNormalizedColors() */
