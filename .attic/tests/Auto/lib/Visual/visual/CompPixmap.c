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
static char rcsid[] = "$XConsortium: CompPixmap.c /main/10 1996/10/07 14:57:02 drk $"
#endif
#endif
/***********************************************************************
  	@(#)CompPixmap.c	1.8.1.1	Date:1/22/91
  	Author: TAT
	History:
            05/24/90 SJS add to sccs
	Calls:

	Summary:
            Compares the pixmap and dumps all errors to stdout.

        INPUTS:
            widget - the widget whose image needs comparing

        OUTPUTS:
            none

        RETURNS:
            nothing
************************************************************************/

#include <AutoMessages.h>
#include "vislib.h"
#include "xislib.h"
#include "mvslib.h"

static  void DumpWindow();

extern  Boolean AutoFullOutput;
extern  Boolean AutoTrace;
extern  Boolean AutoWindowDumps;

extern	void    AutoError();
extern  void 	AutoSystem();
extern  void	AutoTraceMsg();

extern  void    mvsGetWidgetColors();

void mvsComparePixmapBatch(widget_info)
MvsWidgetInfoRecord  *widget_info;
{
    int x,y,begin_x, begin_y, n;
    Arg args[1];
    XImage *image = NULL;			
    Pixmap bpack_pix = 0L;
    Widget widget;
    /* The length of the widget_name used to be 15 but in some
       tests I ran, the widget name had an entire directory path
       prepended to it and was much more than 15. 256 seems like
       a safe number. -- rgcote 7/29/93 */
    char widget_name[256];
    char *name = NULL;
    int widget_class_code;
    unsigned int checksum,checksum2;
    unsigned char *bp;  /* byte pointer */
    unsigned short border_width, width,height;
    short curpixel;
    Pixel matchpixel;
    short found_error = False;
    short no_widget_info = False;
    MvsWidgetClassInfo *wcinfo2;
    MvsWidgetInfoRecord *winfo, *winfo2;
    MvsArgVal *mvs_resources2;
    short color_ref;
    short done;
    MvsWidgetInfoRecord *winfo_stack[100];
    XisObjectRecord *tmp_object;
    MvsWidgetInfoRecord *tmp_info = NULL;
    MvsWidgetInfoRecord *gad_info = NULL;
    Widget tmp_widget = NULL;
    short stack_top = 0;
    int temp_x, temp_y, temp_width, temp_height, temp_bw, revert;
    static unsigned int newhashid = 0;
    Window focus_window;
    Boolean possible_gad_color = False;
    Boolean first_fail_time = True;
    struct _MvsUniqueColorAllocInfoRecord2 
			*widget_unique_color_info;
    struct _MvsUniqueColorAllocInfoRecord2 
			*gad_unique_color_info;
    char msg_string[100];
    Pixmap back_pix=0L;
    Boolean foreign_obj = False;

    xisProcessObjects();
    xisUpdateObjectAttributes();

    XGetInputFocus(mvsDisplay, &focus_window, &revert);

    /* Do depth first traversal to register any newly seen pixmaps */
    /* and to load latest x,y,width,height information             */
    /* Be sure also the widget has the latest color values         */

    widget = widget_info->widget;
    strcpy(widget_name, XtName(widget));

    winfo = widget_info;
 
    done = 0;
    while (!done) {

        temp_x = winfo->x;
        temp_y = winfo->y;
        temp_height = winfo->height;
        temp_width = winfo->width;
        temp_bw = winfo->border_width;
        mvsGetWidgetGeometry(winfo->widget, &temp_x,&temp_y,&temp_width,
                             &temp_height, &temp_bw   );
        winfo->x = temp_x;
        winfo->y = temp_y;
        winfo->height = temp_height;
        winfo->width = temp_width;
        winfo->border_width = temp_bw;

        mvsGetWidgetColors(winfo);

        if (winfo->first_child != NULL) {
            winfo_stack[stack_top++] = winfo;
            winfo = (MvsWidgetInfoRecord *)winfo->first_child;
        }
        else 
            if (winfo->next_sibling != NULL)
                winfo = (MvsWidgetInfoRecord *)winfo->next_sibling;
            else {
                winfo = NULL;
                while (stack_top > 0 && winfo == NULL)
                    winfo = (MvsWidgetInfoRecord *)
                                  winfo_stack[--stack_top]->next_sibling;

                if (stack_top == 0)
                    done = 1;
            } 
    } /* End while(!done) */

    if (newhashid == mvsNumRecIds || 
    			fseek(mvsImageFd,mvsFilePos[newhashid++],0) != 0)
        AutoMessage(_AutoMessages[WARNMSG11]);

    else {
        VIS_trace("#### Found image in recid #%d\n",newhashid);

        if (fread_long(mvsImageFd) != mvsImageRecordStartCode) {
	    sprintf (msg_string, _AutoMessages[WARNMSG12],"");
	    AutoMessage (msg_string);
            AutoMessage(_AutoMessages[WARNMSG13]);
        }
        else {
            checksum = fread_long(mvsImageFd);

            width = fread_short(mvsImageFd);
            height = fread_short(mvsImageFd);

            if ((width != widget_info->width + 2*widget_info->border_width)||
                (height != widget_info->height + 2*widget_info->border_width)) {
	           sprintf (msg_string, _AutoMessages[VISMSG6], 
			    width,height,
			    widget_info->width + 2*widget_info->border_width,
			    widget_info->height + 2*widget_info->border_width,
			    widget_name);
		   AutoMessage (msg_string);
                    
            }
            else {
                mvsDecompressRead(mvsImageFd,mvsImageBuffer,width*height);

                /* check checksum */

                checksum2 = 0;
                bp = &mvsImageBuffer[0];
                for (y=0; y<height; y++) {
                    for (x=0; x<width; x++) {
                        checksum2 += (x+1)*(y+1)*(*bp);
                        bp++;
                    }
                }

                if (checksum != checksum2) {
		    sprintf (msg_string, _AutoMessages[WARNMSG121], 
			     widget_name);
                    AutoMessage (msg_string);
		    AutoMessage (_AutoMessages[WARNMSG123]);
		    

                }
                else {

/*
   		    get border width and subtract from the the current
		    x and y. In GenPixmap() this done through Translate
    		    Coordinates. A better fix maybe to put this in 
		    mvsGetSubObjects() so that x and y returned include
		    the border_width. 
*/

		    border_width = widget_info->border_width;
		  
                    x = widget_info->x - border_width;
                    y = widget_info->y - border_width;

                    image = XGetImage(visDisplay,DefaultRootWindow(visDisplay),
                                      x,y,width,height,AllPlanes, ZPixmap);

		    begin_x = x;
		    begin_y = y;

                    if (image == 0) {
		        sprintf (msg_string, _AutoMessages[VISMSG7], 
				 widget_name);
                        AutoError (msg_string);
				 

                    }

                    /* Compare images pixel by pixel */

                    bp = (&mvsImageBuffer[0]);

                    for (y=0; y<height; y++) {
                        for (x=0; x<width; x++) {

                            /* Get normalized (canonical) color */

			    foreign_obj = False;

		    	    tmp_object = xisFindObjectAtLocation(begin_x + x,
								 begin_y + y);

			    if (tmp_object == NULL) {
				if (AutoFullOutput) {
				   foreign_obj = True;
				   sprintf (msg_string, _AutoMessages[VISMSG8],
					     widget_name, x,y);
                                   AutoMessage(msg_string);
                                    	    
				   break;
				}
				else {
				   
				    sprintf(msg_string, 
					   _AutoMessages[VISMSG34],
					  widget_name); 
			    	   AutoMessage(msg_string);
				   break;
				}
			    }

			    if (foreign_obj)
				break;

		    	    tmp_widget = tmp_object->id.widget;

			    if (tmp_widget != NULL) 
			       widget_class_code  = mvsGetClassCode(tmp_widget);
			    else {
			       /* If the widget is NULL, we may be looking at
				  a pixel on the root window. If so, then
				  we really don't care whether the pixel matches
				  so dump the pixel as PASS and just continue
				  the loop. */
			       if (tmp_object->id.window ==
				   DefaultRootWindow(visDisplay))
			       {
				   bp++;
				   continue;
			       }
			       else
			       {
				   /* Otherwise we found a real error. Log an
				      error message and quit the loop */
				   sprintf(msg_string, _AutoMessages[VISMSG46],
					   x, y);
				   AutoMessage(msg_string);
				   break;
			       }
			    }

			    if (XmIsGadget(tmp_widget) && 
				(widget_class_code 
					== mvsXmPushButtonGadgetClass ||
				 widget_class_code
					== mvsXmToggleButtonGadgetClass)) {
				gad_info = mvsWidgetToWidgetInfo(tmp_widget);
				gad_unique_color_info =
					gad_info->widgetUniqueColorInfo;
				possible_gad_color = True;
			    }

			    /* If the object is a gadget then find parent
			       of the gadget and use it */
				
			    while (XmIsGadget(tmp_widget) &&
				   widget_class_code != mvsXmIconGadgetClass)
				tmp_widget = XtParent(tmp_widget);

		    	    tmp_info = mvsWidgetToWidgetInfo(tmp_widget);

                            curpixel = *bp;

			    /* check if alloced too many colors per widget */
                            if (curpixel >= MAX_UNIQUE_COLORS) {
			        sprintf (msg_string, _AutoMessages[VISMSG23]);
				found_error = True;
			    }

			    if (tmp_info == NULL)
			    {
				if (no_widget_info == False)
				{
				    /* We only want to log this message once
				       into the output file. Doing it on every
				       pixel of the offending widget will degrade
				       performance too much. */
				    no_widget_info = True;
				    sprintf(msg_string, _AutoMessages[VISMSG47], 
					    XtName(tmp_widget));
				    AutoMessage(msg_string);
				    found_error = True; /* because we couldn't find
							   enough info to fix the
							   pixel */
				}
				/* We can't normalize matchpixel, but at least
				   give it a value. */
				matchpixel = curpixel;
			    }
			    else
			    {
			      if (curpixel < MAX_UNIQUE_COLORS) {
				widget_unique_color_info = 
				  tmp_info->widgetUniqueColorInfo;
				
				/* Convert to expected color "match_pixel" */
				winfo2 = 
				  widget_unique_color_info[curpixel].widget_info;

				if (winfo2 == NULL)
				{
				    found_error = True; /* because we couldn't find
							   enough info to fix the
							   pixel */
				    /* We can't normalize matchpixel, but at least
				       give it a value. */
				    matchpixel = curpixel;
				}
				else {
				    wcinfo2 = winfo2->widget_class_info;
				    color_ref =
				      widget_unique_color_info[curpixel].color_ref;
				    mvs_resources2 = winfo2->mvs_resources;
				    matchpixel = (Pixel)
				      mvs_resources2[wcinfo2->res_color_ref[color_ref]];

				}
			      } 
			    }

	       /* If you dont receive a match it may be that the widget
		  does not have the highlight from the parent. If this
		  is the case then use the background color of the parent 
		  to satisfy the highlight color */


                if (XGetPixel(image,x,y) != matchpixel && 
		    (curpixel < MAX_UNIQUE_COLORS) &&
		    focus_window != XtWindowOfObject(tmp_widget)) {
			tmp_widget = XtParent(tmp_widget);
			tmp_info = mvsWidgetToWidgetInfo(tmp_widget);
			if (tmp_info != NULL) {
                            widget_unique_color_info =
                                tmp_info->widgetUniqueColorInfo;
			    color_ref = 
				widget_unique_color_info[curpixel].color_ref;
                            winfo2 = 
				widget_unique_color_info[curpixel].widget_info;
			}
			else
				found_error = True; /* because we couldn't find
						       enough info to fix the
						       pixel */

			/* 
			   If the current pixel cannot be found on the parent
			   then we know that there is a failure within the
			   widget itself. 
			*/

			if (tmp_info != NULL && winfo2 != NULL) {
                       	    	wcinfo2 = winfo2->widget_class_info;
                        	mvs_resources2 = winfo2->mvs_resources;
				name = wcinfo2->resource_info[wcinfo2->res_color_ref[color_ref]].name;

				n = 0;
				XtSetArg(args[n], XmNbackgroundPixmap,
					 &back_pix); n++;
				XtGetValues(tmp_widget, args, n);

			/* Only get matchpixel if the pixel you are looking at
			   is the background color of the parent to see if it
                           matches the highlightColor of the "Picture" widget.
			   No way to find highlightColor of the widget */

				if (strcmp("background", name) == 0)
			  		matchpixel = (Pixel)
					  mvs_resources2[wcinfo2->res_color_ref[color_ref]];
				else {
				   if (back_pix != 0L) {
				       if (strcmp("foreground", name) == 0 ||
					   strcmp("bottomShadowColor", name) == 0 ||
					   strcmp("topShadowColor", name) == 0)
					       matchpixel = (Pixel)
						   mvs_resources2[wcinfo2->res_color_ref[color_ref]];
				   }
				}
			}
		}

                if (XGetPixel(image,x,y) != matchpixel && 
		    (curpixel < MAX_UNIQUE_COLORS) &&
		    possible_gad_color) {
			color_ref = 
				gad_unique_color_info[curpixel].color_ref;
                        winfo2 = 
				gad_unique_color_info[curpixel].widget_info;

			/* 
			   If the current pixel cannot be found on the parent
			   then we know that there is a failure within the
			   widget itself. 
			*/

			if (winfo2 == NULL)
				found_error = True; /* because we couldn't find
						       enough info to fix the
						       pixel */
			else {
			    int k, num_colors;
			    Pixel image_pixel;

			    wcinfo2 = winfo2->widget_class_info;
			    mvs_resources2 = winfo2->mvs_resources;
			    num_colors = wcinfo2->num_res_colors;
		       
			    image_pixel = XGetPixel(image, x, y);

			    for (k = 0; k < num_colors; k++) {
			     if (wcinfo2->res_color_ref[k] >= 0) {

			       name = wcinfo2->resource_info[wcinfo2->res_color_ref[k]].name;

			/* Only get matchpixel if the pixel you are looking at
			   is the armcolor (in case of PushButtonGadget)
			   and selectColor (in case of ToggleButtonGadget) */

			       if ((strcmp("selectColor", name) == 0 ||
				    strcmp("unselectColor", name) == 0) &&
				   widget_class_code == 
				        mvsXmToggleButtonGadgetClass)
				    if (image_pixel == (Pixel)
				                       mvs_resources2[wcinfo2->res_color_ref[k]])
					matchpixel = (Pixel)
					    mvs_resources2[wcinfo2->res_color_ref[k]];
				if (strcmp("armColor", name ) == 0 &&
				    widget_class_code ==
				    mvsXmPushButtonGadgetClass)
				    matchpixel = (Pixel)
					mvs_resources2[wcinfo2->res_color_ref[k]];
			     }
			    }
			}
		}

                            /* Compare expected "match_pixel" with 
			       actual pixel */

                            if (XGetPixel(image,x,y) != matchpixel) {
                                found_error = True; /* because of pixel
						       mismatch */
				if (AutoFullOutput) {
					if (first_fail_time && AutoWindowDumps){
						DumpWindow(widget);
						first_fail_time = False;
					}
					sprintf (msg_string, 
						 _AutoMessages[VISMSG10],
						 widget_name, x,y,matchpixel,
						 XGetPixel(image,x,y)); 

                                	AutoMessage(msg_string);

				}
                            }
                            bp++;
                        }
                    }
                }
            }
            if (found_error) {
		sprintf(msg_string, 
			_AutoMessages[VISMSG35],
			widget_name); 
                AutoMessage(msg_string);
	    }
            if (image != NULL)
                XDestroyImage(image);
        }
    }

} /* End mvsComparePixmapBatch() */


static void DumpWindow(widget)
Widget widget;

{
    char trace_msg[100];
    char file_string[25];
    char window_string[25];
    char *send_string[6];
    static int num_failure = 0;

    sprintf(file_string, "%s_fail%d", mvsTestName, num_failure++);
    sprintf(window_string, "%ld", XtWindow(widget));

    send_string[0] = "xwd";
    send_string[1] = "-id";
    send_string[2] = window_string;
    send_string[3] = "-out";
    send_string[4] = file_string;
    send_string[5] = (char *) 0;

    if (AutoTrace) {
	sprintf(trace_msg, "DumpWindow: xwd -id %s -out %s for widget %s",
		  window_string, file_string, XtName(widget));
	AutoTraceMsg(trace_msg);
    }
	
    AutoSystem("xwd", send_string);

}

void mvsGetWidgetColors(widget_info)
MvsWidgetInfoRecord  *widget_info;
{

    int i;
    Arg color_args[10];
    Cardinal num_args;
    MvsResourceInfoRecord *resource_info;
    int num_res_colors;
    short *res_color_ref;

    num_args = 0;
    resource_info = widget_info->widget_class_info->resource_info;
    num_res_colors = widget_info->widget_class_info->num_res_colors;
    res_color_ref = widget_info->widget_class_info->res_color_ref;

    for (i = 0; i < num_res_colors; i++) {
	if (res_color_ref[i] >= 0) {
	    XtSetArg(color_args[num_args], 
		     resource_info[res_color_ref[i]].name,
		     &widget_info->mvs_resources[res_color_ref[i]]);
	    num_args++;
	}
    }

    XtGetValues(widget_info->widget, color_args, num_args);
}
