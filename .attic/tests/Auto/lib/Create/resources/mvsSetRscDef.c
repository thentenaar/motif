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
static char rcsid[] = "$XConsortium: mvsSetRscDef.c /main/7 1995/07/14 11:22:51 drk $"
#endif
#endif
/***********************************************************************
  	@(#)mvsSetRscDef.c	1.14.1.1	Date:1/22/91
  	Author: TAT
	History:
	   03/30/90 SJS Add to sccs tree
	   05/29/90 SJS make changes for composites
	   06/27/90 SJS add call for dynamic default descriptions
           06/28/90 SJS change if to a switch
           07/11/90 SJS correctly handle dynamic XmStrings
           07/29/90 PSN use mvsCopyResource()
           09/23/90 SJS elim. MVSWhite

	Calls:

	Summary:
		This function sets the local copy of resource values to
	the default settings. This is really equivalent of saying that
	the widget is in the "start" state. A point of note, is that
	mvsSetLocalStartConstraints is called after everything is set.
	This is to take into account the constraints imposed by the
	parent widget. For example, an application shell forces its
	children to be the size of the shell.
************************************************************************/
#include <AutoMessages.h>
#include "mvslib.h"

void mvsSetResourcesToDefaults(widget_info,args,nargs) 
MvsWidgetInfoRecord *widget_info;
Arg args[];
Cardinal nargs;
{ 
    int i,j; 
    Arg loc_args[1];
    Pixel pixel;
    short num_resources;
    MvsResourceInfoRecord *resource_info;
    Widget widget;
    MvsArgVal *resources;
    MvsArgVal (*dflt_func)();
    static char routine_name[] = "mvsSetResourcesToDefaults():";
    char msg_string[100];

    num_resources = widget_info->widget_class_info->num_resources;
    resource_info = widget_info->widget_class_info->resource_info;
    resources = widget_info->mvs_resources;
    widget = widget_info->widget;

    if (mvsDisplay == (Display *)NULL) {
        sprintf (msg_string, _AutoMessages[WARNMSG79], routine_name);
        AutoMessage(msg_string);
    }
    for (i=0; i<num_resources; i++) {

        /* Only set those resources whose values don't appear in args list */

        for (j=0; j<nargs && strcmp(args[j].name,resource_info[i].name); j++);

        if (j == nargs) { /* nope, not there (not in args list) */
            if(resource_info[i].needs_converting) {

                switch((int) resource_info[i].default_value) {

                case MVSdynamic:
                    XtSetArg(loc_args[0],resource_info[i].name,&resources[i]);
                    XtGetValues(widget,loc_args,1);
                    resources[i] = mvsConvertToType(resources[i],
                                                    resource_info[i].type_code);
                    if (resource_info[i].type_code == T_Pixel) {
			if (resources[i] < 0)
			    resources[i] = 0;
                        if (!mvsAllocColor(resources[i])) {
			    sprintf (msg_string, _AutoMessages[WARNMSG26], 
				     routine_name);
                            AutoMessage(msg_string); }
                    }
                    else
                        if (resource_info[i].type_code == T_XmString) {
                            if((XmString)resources[i] != (XmString)NULL) {
                                XmString save_str = (XmString)resources[i];
                                resources[i] = (MvsArgVal)
                                      mvsExtractString((XmString)resources[i],
                                                       resource_info[i].name);
                                XmStringFree(save_str);
                            }
                        } /* End if(type_code == T_XmString) */
                    break;

                case MVSBlack:
                    if (resource_info[i].type_code == T_Pixel) {
                        if (!mvsAllocNamedColor("Black",&pixel)) {
			   sprintf (msg_string, _AutoMessages[WARNMSG27],
				    routine_name);
                            AutoMessage(msg_string);
                            pixel = XBlackPixel(mvsDisplay,xisScreen);
                        }
                        if (!mvsAllocColor(pixel)) {
			    sprintf (msg_string, _AutoMessages[WARNMSG28],
				     routine_name);
			    AutoMessage(msg_string);}

                            
                        resources[i] = (MvsArgVal) pixel;
                        if(pixel != XBlackPixel(mvsDisplay,xisScreen)) 
                            AutoMessage (_AutoMessages[WARNMSG29]);
                    }
                    else
                        AutoMessage(_AutoMessages[WARNMSG30]);

                    break;

                case MVSXtCopyFromParent:
                    XtSetArg(loc_args[0],resource_info[i].name,&resources[i]);
                    XtGetValues(XtParent(widget),loc_args,1); 	
                    if (resource_info[i].type_code == T_Pixel) {
                        if (!mvsAllocColor(resources[i])) {
			  sprintf (msg_string, _AutoMessages[WARNMSG31],
			           routine_name);
			  AutoMessage(msg_string);}

                    }
                    break;

                case MVSXtCopyScreen:
                    resources[i]=(MvsArgVal)XScreenOfDisplay(mvsDisplay,
                                                             xisScreen);
                    break;

                default:
                    dflt_func=(MvsArgVal (*)())(resource_info[i].default_value);
                    if(dflt_func)
                        resources[i] = (*dflt_func)(widget_info);
                    else {
		       sprintf (msg_string, _AutoMessages[WARNMSG80], 
				routine_name);
                       AutoMessage(msg_string); }
                } /* End switch() */
            }
            else
                resources[i] = mvsCopyResource(widget_info->widget_class_info,
					resource_info[i].default_value, i);

        } /* End if (j == nargs) */
    }  /* End for(i<num_resources) */

    mvsSetLocalStartConstraints(widget_info,resources,args,nargs);

} /* End mvsSetResourcesToDefaults() */
  
