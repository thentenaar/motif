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
static char rcsid[] = "$XConsortium: CmpStrPix.c /main/6 1996/10/07 14:58:09 drk $"
#endif
#endif
/***********************************************************************
  	Author: REC
	History:
	    Created 04/15/93 by REC
	Calls:

	Summary:
            Compares the current pixmap to one generated prior in the
	    current compare run.  Dumps all errors to stdout.

        INPUTS:
            widget - the widget whose image needs comparing
	    identifier - tag associated with the image

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

void mvsCompareStoredPixmapBatch(widget_info, identifier)
MvsWidgetInfoRecord *widget_info;
char *identifier;
{
    int x,y,begin_x, begin_y, n, i;
    Arg args[1];
    XImage *image = NULL;			
    Pixmap bpack_pix = 0L;
    Widget widget;
    char widget_name[15];
    char *name = NULL;
    int widget_class_code;
    unsigned char *bp;  /* byte pointer */
    unsigned short border_width, width,height;
    short curpixel;
    Pixel matchpixel;
    short found_error = False;
    MvsWidgetClassInfo *wcinfo2;
    MvsWidgetInfoRecord *winfo, *winfo2;
    MvsArgVal *mvs_resources2;
    short color_ref;
    short done, found;
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
    struct mvsTempImageRec *image_rec;
    char msg_string[100];
    Pixmap back_pix=0L;
    Boolean forgein_obj = False;
    unsigned char *tmpbuf;

    /* NOT COMPLETELY IMPLIMENTED, RETURN AND DO NOTHING */
    /* if is added to suppress compiler warnings	*/
    if ( !forgein_obj ) return;

    tmpbuf = (unsigned char *)XtMalloc(MAX_IMAGE_DIMENSION*MAX_IMAGE_DIMENSION);

    xisProcessObjects();
    xisUpdateObjectAttributes();

    XGetInputFocus(mvsDisplay, &focus_window, &revert);

    /* Do depth first traversal to register any newly seen pixmaps */
    /* and to load latest x,y,width,height information             */

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


    /* search for the identifier in the temp image names buffer */
    found = 0;
    image_rec = mvsTempImageRecHead;
    while ((image_rec != NULL) && !found)  {
	if (strcmp(identifier, image_rec->mvsTempImageName) == 0)  {
	    found = 1;
	}	
	else {
	    image_rec = image_rec->next;
	}
    }

    if(!found)  {
/* CHANGE TO AUTOMESSAGES WARNING */
	printf ("ERROR IMAGE NOT FOUND");
	exit(1);
    }

    width = image_rec->mvsTempImageWidth;
    height = image_rec->mvsTempImageHeight;

    if ((width != widget_info->width + 2*widget_info->border_width)||
	(height != widget_info->height + 2*widget_info->border_width)) {
	
	sprintf (msg_string, _AutoMessages[VISMSG6], width,height,
		 widget_info->width + 2*widget_info->border_width,
		 widget_info->height + 2*widget_info->border_width,
		 widget_name);
	AutoMessage(msg_string);
    }


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

    strcpy ((char *)tmpbuf, (char *)image_rec->mvsTempImageBuffer);

    bp = &tmpbuf[0];
    
    for (y=0; y<height; y++) {
	for (x=0; x<width; x++) {
	    
	    /* Get normalized (canonical) color */
	    
	    forgein_obj = False;
	    
	    tmp_object = xisFindObjectAtLocation(begin_x + x,
						 begin_y + y);
	    
	    if (tmp_object == NULL) {
		if (AutoFullOutput) {
		    forgein_obj = True;
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
	    
	    if (forgein_obj)
		break;
	    
	    tmp_widget = tmp_object->id.widget;
	    
	    if (tmp_widget != NULL) 
		widget_class_code  = mvsGetClassCode(tmp_widget);
	    else {
		found_error = True;
		break;
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
	    
	    while (XmIsGadget(tmp_widget))
		tmp_widget = XtParent(tmp_widget);
	    
	    tmp_info = mvsWidgetToWidgetInfo(tmp_widget);
	    
	    widget_unique_color_info = 
		tmp_info->widgetUniqueColorInfo;
	    
	    curpixel = *bp;
	    if (curpixel >= MAX_COLORS) {
		sprintf (msg_string, _AutoMessages[VISMSG9],
			 curpixel,x,y);
		AutoError(msg_string);
		
		
	    }
	    
	    /* Convert to expected color "match_pixel" */
	    
	    winfo2 = 
		widget_unique_color_info[curpixel].widget_info;
	    
	    if (winfo2 == NULL)  {  
		found_error = True;
printf("found_error winfo2 == NULL (1)\n");
	    }
	    else {
		wcinfo2 = winfo2->widget_class_info;
		color_ref =
		    widget_unique_color_info[curpixel].color_ref;
		mvs_resources2 = winfo2->mvs_resources;
		matchpixel = (Pixel)
		    mvs_resources2[wcinfo2->res_color_ref[color_ref]];
	    }
	    
	    /* If you dont receive a match it may be that the widget
	       does not have the highlight from the parent. If this
	       is the case then use the background color of the parent 
	       to satisfy the highlight color */
	    
	    
	    if (XGetPixel(image,x,y) != matchpixel && 
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
		else  {
		    found_error = True;
		}
		
		if (winfo2 == NULL)  {
		    found_error = True;
printf("found_error, winfo2 (2) == NULL\n");
		}
		
		/* 
		  If the current pixel cannot be found on the parent
		  then we know that there is a failure within the
		  widget itself. 
		  */
		
		if (! found_error) {
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
			    if (strcmp("foreground", name) == 0)
				matchpixel = (Pixel)
				    mvs_resources2[wcinfo2->res_color_ref[color_ref]];
			}
		    }
		}
	    }
	    
	    if (XGetPixel(image,x,y) != matchpixel && 
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
		
		if (winfo2 == NULL)  {
		    found_error = True;
printf("found_error winfo2 == NULL (3)\n");
		}
		else {
		    wcinfo2 = winfo2->widget_class_info;
		    mvs_resources2 = winfo2->mvs_resources;
		    name = wcinfo2->resource_info[wcinfo2->res_color_ref[color_ref]].name;
		    
		    /* Only get matchpixel if the pixel you are looking at
		       is the armcolor (in case of PushButtonGadget)
		       and selectColor (in case of ToggleButtonGadget) */
		    
		    if (strcmp("selectColor", name) == 0 &&
			widget_class_code == 
			mvsXmToggleButtonGadgetClass)
			matchpixel = (Pixel)
			    mvs_resources2[wcinfo2->res_color_ref[color_ref]];
		    if (strcmp("armColor", name ) == 0 &&
			widget_class_code ==
			mvsXmPushButtonGadgetClass)
			matchpixel = (Pixel)
			    mvs_resources2[wcinfo2->res_color_ref[color_ref]];
		}
	    }
	    
	    /* Compare expected "match_pixel" with 
	       actual pixel */
	    
	    if (XGetPixel(image,x,y) != matchpixel) {
		found_error = True;
printf("XGetPixel does not match matchpixel\n");
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
		else {
		    found_error = True;
		}
	    }
	    bp++;
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

    XtFree((char *)tmpbuf);

    return ;

} /* End CompareStoredPixmapBatch() */


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

    return ;
}
