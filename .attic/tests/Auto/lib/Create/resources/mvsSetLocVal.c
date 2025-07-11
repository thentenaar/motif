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
static char rcsid[] = "$XConsortium: mvsSetLocVal.c /main/8 1995/07/14 11:22:30 drk $"
#endif
#endif
/***********************************************************************
  	@(#)mvsSetLocVal.c	1.10.1.1	Date:1/22/91
  	Author: TAT
	History:
	   03/30/90 SJS Add to sccs tree
	   05/29/90 SJS add chanegs for composites
	   07/11/90 SJS handle XmStrings properly
	   08/01/90 SJS correct free/extract logic of XmString case
	Calls:

	Summary:
		This function sets the local copy of the resource values.
	These are what the MVS thinks the widget resource values should be.
	For pointer resources, memory is allocated and structures copied.
************************************************************************/
#include "mvslib.h"
#include <AutoMessages.h>

void mvsSetLocalValues(widget_info, args, nargs)
MvsWidgetInfoRecord *widget_info;
ArgList args;
Cardinal *nargs;
{
    int i,j,k;
    short num_resources;
    MvsResourceInfoRecord *resource_info;
    MvsArgVal *mvs_resources;
    char msg_string[125];

    num_resources = widget_info->widget_class_info->num_resources;
    resource_info = widget_info->widget_class_info->resource_info;
    mvs_resources = widget_info->mvs_resources;

    for (i=0; i<*nargs; i++) {
        for (j=0;
             (j<num_resources)&&strcmp(args[i].name,resource_info[j].name);j++);
        if (j >= num_resources)
		;
        else {
            switch (mvsTypeInfo[resource_info[j].type_code].ref_type) {
                case RefXmStringTable: /* For now until decided how to handle */
	        case RefBoolean:
	        case RefValue:
                case RefGeneric:
                    if (((resource_info[j].type_code == T_Pixel ||
                        resource_info[j].type_code == T_Pixmap  )) &&
			(mvsGetVisualMode() == VISUAL_MODE_GENERATE_PIXMAPS)) {
                     /*       if (resource_info[j].type_code == T_Pixel)
                                mvsFreeColor(args[i].value); */
                            for (k=i; k<(*nargs-1); k++) {
                                args[k].name = args[k+1].name;
                                args[k].value = args[k+1].value;
                            }
                            (*nargs)--;
                    }
		    else
			mvs_resources[j] = (MvsArgVal)args[i].value;
                    break;
   
                case RefXmString:
                    if((char *)mvs_resources[j] != (char *)NULL) {
                        XtFree( (char *)mvs_resources[j]);
                    }
                    mvs_resources[j] = (MvsArgVal)
                                      mvsExtractString((XmString)args[i].value,
                                                       args[i].name);
                    break;

	        case RefString:
                    if(mvs_resources[j]) {
                        XtFree((char *)(mvs_resources[j]));
			mvs_resources[j] = 0L;
		    }

                    if (args[i].value) {
                        mvs_resources[j] = (MvsArgVal)XtMalloc(
					       strlen((char *)args[i].value)+1);
                        strcpy((char *)mvs_resources[j],(char *)args[i].value);
                    }
                    else mvs_resources[j] = 0L;
                    break;

	        case RefCallbackList:
		{
                    XtCallbackList cb_list = (XtCallbackList)args[i].value;
                    int k=0;

                    if(mvs_resources[j]) {
                        XtFree((char *)(mvs_resources[j]));
			mvs_resources[j] = 0L;
		    }

                    if (cb_list != NULL) {
                        while (cb_list->callback != NULL) {
                            cb_list++;
                            k++;
                        }
                        cb_list = (XtCallbackList)args[i].value;

                        mvs_resources[j] = (MvsArgVal)XtMalloc(sizeof(XtCallbackRec)*(k+1));
                        memcpy((char *)mvs_resources[j],(char *)cb_list,
                               sizeof(XtCallbackRec)*(k+1));
                    }
                    else mvs_resources[j] = 0L;

                    /*** remove callback from list *****/

                    for (k=i; k<*nargs-1; k++) {
                        args[k].name = args[k+1].name;
                        args[k].value = args[k+1].value;
                    }

                    break;
		}

	        default:
		   sprintf(msg_string, _AutoMessages[WARNMSG119], 
			   mvsTypeInfo[resource_info[j].type_code].ref_type);
                   AutoMessage(msg_string);
                             
                   break;
	    }
	}
    }
    mvsSetLocalConstraints(widget_info,mvs_resources,args, *nargs);

} /* End mvsSetLocalValues() */
