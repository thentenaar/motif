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
static char rcsid[] = "$XConsortium: mvsSetValues.c /main/7 1995/07/14 11:23:35 drk $"
#endif
#endif
/***********************************************************************
	Calls:

	Summary:
		This function sets the resource values requested and
	then calls mvsCompareResources() to verify that the requested
	changes were made.
************************************************************************/

#include "mvslib.h"

extern void NormalizePixmap();

void mvsSetValues(widget_info,args,nargs)
MvsWidgetInfoRecord *widget_info;
ArgList args;
Cardinal nargs;
{
    int i,j, n;
    XtCallbackList cb_list;
    int t, x, y;
    Arg new_args[100];
    Cardinal new_nargs = 0;
    short num_resources;
    MvsResourceInfoRecord *resource_info;
    Widget widget;
    MvsArgVal *mvs_resources;
    Arg color_args[10];
    Pixel foreground, background;
    Window root;
    int pix_x, pix_y;
    unsigned int pix_width, pix_height, pix_border, depth;
    XImage *pix_image = NULL;
    Pixel pixel1, pixel2;
    GC gc;

    num_resources = widget_info->widget_class_info->num_resources;
    resource_info = widget_info->widget_class_info->resource_info;
    widget = widget_info->widget;
    mvs_resources = widget_info->mvs_resources;

    for(i=0; i<nargs; i++) {
        for (j=0;
             (j<num_resources)&&strcmp(args[i].name,resource_info[j].name);
             j++);

        if (j<num_resources) {
            t = resource_info[j].type_code;
            switch (mvsTypeInfo[t].ref_type) {

            case RefCallbackList:
		new_args[new_nargs].name = args[i].name;
		new_args[new_nargs].value = args[i].value;
		new_nargs++;
                break;
	    case RefValue:
                if (t != T_Pixel) {
			new_args[new_nargs].name = args[i].name;
			new_args[new_nargs].value = args[i].value;
			new_nargs++;
		}
                break;
	    default:
		new_args[new_nargs].name = args[i].name;
		new_args[new_nargs].value = args[i].value;
		new_nargs++;
                break;
	    }
        }
	else {
	    /* Don't change colors on Gadgets that are not ready in
	       automation for these when recording or comparing */
	    if ((mvsGetVisualMode() != VISUAL_MODE_DONT_CHECK) &&
		(XmIsGadget(widget) && 
		(mvsGetClassCode(widget) != mvsXmIconGadgetClass) &&
		(strcmp(args[i].name, "background") == 0 ||
		 strcmp(args[i].name, "foreground") == 0 ||
		 strcmp(args[i].name, "bottomShadowColor") == 0 ||
		 strcmp(args[i].name, "highlightColor") == 0 ||
		 strcmp(args[i].name, "topShadowColor") == 0 ||
		 strcmp(args[i].name, "borderColor") == 0))) {
		;
	    }
	    else {
		new_args[new_nargs].name = args[i].name;
		new_args[new_nargs].value = args[i].value;
		new_nargs++;
	    }
	}
    } /* for() */

    /* Normalize pixmap when in record or compare mode */

    if (mvsGetVisualMode() != VISUAL_MODE_DONT_CHECK) {
	for (i = 0; i < new_nargs; i++) {
	    if ((strcmp(new_args[i].name, "labelPixmap") == 0 		  ||
		 strcmp(new_args[i].name, "labelInsensitivePixmap") == 0  ||
		 strcmp(new_args[i].name, "symbolPixmap") == 0		  ||
		 strcmp(new_args[i].name, "selectPixmap") == 0		  ||
		 strcmp(new_args[i].name, "selectInsensitivePixmap") == 0 ||
		 strcmp(new_args[i].name, "armPixmap") == 0 		  ||
		 strcmp(new_args[i].name, "backgroundPixmap") == 0	  ||
		 strcmp(new_args[i].name, "highlightPixmap") == 0         ||
		 strcmp(new_args[i].name, "borderPixmap") == 0		  ||
		 strcmp(new_args[i].name, "topShadowPixmap") == 0	  ||
		 strcmp(new_args[i].name, "bottomShadowPixmap") == 0	  ||
		 strcmp(new_args[i].name, "cascadePixmap") == 0		  ||
		 strcmp(new_args[i].name, "largeIconPixmap") == 0	  ||
		 strcmp(new_args[i].name, "largeIconMask") == 0	          ||
		 strcmp(new_args[i].name, "smallIconPixmap") == 0         ||
		 strcmp(new_args[i].name, "smallIconMask") == 0	          ||
		 strcmp(new_args[i].name, "bindingPixmap") == 0	          ||
		 strcmp(new_args[i].name, "collapsedStatePixmap") == 0	  ||
		 strcmp(new_args[i].name, "expandedStatePixmap") == 0)	  &&
		(new_args[i].value != (XtArgVal) XmUNSPECIFIED_PIXMAP) &&
		new_args[i].value != (XtArgVal) None) {

		NormalizePixmap(widget_info, new_args[i].value);
	 
	    }
	}
    }

    if (mvsGetVisualMode() == VISUAL_MODE_GENERATE_PIXMAPS)
      XtSetValues(widget, new_args, new_nargs);
/*    else
      XtSetValues(widget, args, nargs);
*/
/* Don't change colors on gadgets until automation is really updated to 2.0
   and supports colors on gadgets */
    else
	if (XmIsGadget(widget) &&  
	    (mvsGetClassCode(widget) != mvsXmIconGadgetClass))
	    XtSetValues(widget, new_args, new_nargs);
        else
	    XtSetValues(widget, args, nargs);


    mvsSetLocalValues(widget_info, args, &nargs);

} /* End mvsSetValuesNoCheck() */
