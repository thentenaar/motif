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
static char rcsid[] = "$XConsortium: mvsLoadRscs.c /main/7 1995/07/14 11:21:50 drk $"
#endif
#endif
/***********************************************************************
  	@(#)mvsLoadRscs.c	1.11.1.1	Date:1/22/91
  	Author: TAT
	History:
	   03/30/90 SJS Add to sccs tree
	   05/29/90 SJS add changes for composites
	   07/11/90 SJS make XmStrings be stored internally as regular strings
	   07/12/90 PSN add singular load resource function all other functions
  		  	here are rearranged to use same function common*
	Calls:

	Summary:
		This function loads the real values of the widget
	resources. It also copies the information pointed to by those
	resources that are pointers.
************************************************************************/
#include <AutoMessages.h>
#include "mvslib.h"

void commonLoadResources(widget_info, resources, num_resources, resource_info) 
MvsWidgetInfoRecord  *widget_info; 
MvsArgVal resources[];
int num_resources;
MvsResourceInfoRecord *resource_info;
{ 
    Arg args[MAX_RESOURCES];
    Widget widget;
    int i,j;
    char msg_string[125];

    widget = widget_info->widget;

    if (XtIsManaged(widget))  {

    	for (i=0, j=0; i<num_resources; i++) {
		if(resource_info[i].access & GET) {
       	    		XtSetArg(args[j],resource_info[i].name,&resources[i]);
	   		j++;
		}
    	}

    	XtGetValues(widget,args,j);

    	for (i=0; i<num_resources; i++) 
        	resources[i] =
		      mvsConvertToType(resources[i],resource_info[i].type_code);

    	for (i=0; i<num_resources; i++) {

        	switch (mvsTypeInfo[resource_info[i].type_code].ref_type) {

            	case RefXmStringTable: /* Here until handling of compound string
                                      tables is determined */
	    	case RefValue: 	 
            	case RefGeneric:
            	case RefBoolean: 
	    	case RefString: 
               		 /* Do nothing since it already copied the value */ 
                break;

	    	case RefXmString:
                	if ((XmString)resources[i] != (XmString)NULL) {
                    		XmString save_str = (XmString)resources[i];

                    		resources[i] = (MvsArgVal)
                              	        mvsExtractString((XmString)resources[i],
                                                         resource_info[i].name);
				XmStringFree(save_str);
                	}
                	break;

	    	case RefCallbackList:
                	if (resources[i]) 	/* MEMLEAK fix */
				XtFree( (char *)(resources[i]));
                	resources[i] = 0; /* can't get this resource this way */
                	break;

	    	default: 
		        sprintf (msg_string, _AutoMessages[WARNMSG117], 
			    mvsTypeInfo[resource_info[i].type_code].ref_type); 
                	AutoMessage(msg_string);
                             
                	break; 	
        	} 
    	}  /* End for(i<num_resources) */
    }

} /* End commonLoadResources() */


/***********************************************************************
  	Author: PSN
	History:
	   07/12/90 PSN add singular load resource function all other
                        functions here are rearranged to use same
                        function common
	Calls:

	Summary:
 	    Make mvsLoadResources() do the right thing
***********************************************************************/

void mvsLoadResources(widget_info, resources) 
MvsWidgetInfoRecord  *widget_info; 
MvsArgVal resources[];
{ 
    int num_resources;
    MvsResourceInfoRecord *resource_info;

    num_resources = (int)widget_info->widget_class_info->num_resources;
    resource_info = widget_info->widget_class_info->resource_info;

    commonLoadResources(widget_info, resources, num_resources, resource_info);

} /* End mvsLoadResources() */


/***********************************************************************
  	Author: PSN
	History:
	   07/12/90 PSN add singular load resource function all other
                        functions here are rearranged to use same
                        function common
	Summary:
	    Singular mvsLoadResource() loads only one resource specified
        by res_name. Also, returns that value to the caller
***********************************************************************/

MvsArgVal mvsLoadResource(widget_info, res_name)
MvsWidgetInfoRecord  	*widget_info; 
char			res_name[];
{ 
    int 	res_index;
    MvsArgVal	resource;
    MvsResourceInfoRecord *resource_info;
    char msg_string[125];

    res_index = mvsGetResourceNum(widget_info->widget_class_info, res_name);
    if(res_index == -1) {
        sprintf (msg_string, _AutoMessages[WARNMSG117], res_name);
        AutoMessage(msg_string);
	return (MvsArgVal)(-1);	/* Don't know what to return */
    }

    resource_info = &(widget_info->widget_class_info->resource_info[res_index]);

    commonLoadResources(widget_info, &resource, (int)1, resource_info);

    return (resource); 	/* WARNING: since commonLoadResources() does */
			/* not return error code on failure this will */
			/* return unspecified value!? */ 

} /* End mvsLoadResource() */
