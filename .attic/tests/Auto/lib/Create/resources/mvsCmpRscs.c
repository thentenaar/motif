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
static char rcsid[] = "$XConsortium: mvsCmpRscs.c /main/7 1995/07/14 11:19:20 drk $"
#endif
#endif
/***********************************************************************
  	@(#)mvsCmpRscs.c	1.20.1.1	Date:1/22/91
  	Author: TAT
	History:
	   03/30/90 SJS Add to the sccs tree
	   04/13/90 SJS change msg_error to msg_failure for future
	                addition of Assertions.
	   05/29/90 SJS changes for composites
	   06/11/90 SJS add compare for XmStrings
	   06/26/90 SJS can not check any callbacks at all (damn it)
	   07/11/90 SJS XmStrings are internally justy strings, compare them
                        like Strings
	   07/29/90 PSN initialize args, parameter changes
	   09/04/90 SJS check for gettability
	   11/12/90 PSN don't print addresses (RefGeneric) on failed 
			comparisons & also number failures.
	Calls:

	Summary:
		This function compares a set of resource values with the
	values contained in the current widget. Comparisons are done
	according to the type of the resource. (For example, in comparing
	strings, the contents of the string pointers are compared, NOT the
	pointers themselves.)

************************************************************************/

#include <AutoMessages.h>
#include "mvslib.h"

void mvsCompareResources(widget_info)
MvsWidgetInfoRecord *widget_info;
{
    MvsArgVal resources2[MAX_RESOURCES];
    MvsArgVal *resources;
    Widget widget;
    MvsResourceInfoRecord *resource_info;
    short num_resources;
    int i;
    char msg_string[100];

    resources = widget_info->mvs_resources;
    num_resources = widget_info->widget_class_info->num_resources;
    resource_info = widget_info->widget_class_info->resource_info;

    for (i=0; i<MAX_RESOURCES; i++) resources2[i] = NULL;

    mvsLoadResources(widget_info,resources2);

    for (i=0; i<num_resources; i++) {
        if(!(resource_info[i].access & GET)) {
            continue;
        }
        switch (mvsTypeInfo[resource_info[i].type_code].ref_type) {
            case RefXmStringTable: /* Do nothing for now */
                break;

	    case RefBoolean:
                if ( ((resources[i] == 0)&&(resources2[i] != 0)) ||
                     ((resources[i] != 0)&&(resources2[i] == 0)) ) {
                    msg_failure("1000.%d: Resource 'XmN%s' mismatch:\n\t\texpected %d, received %d\n",
                       i, resource_info[i].name,resources[i], resources2[i]);
                    resources[i] = resources2[i];
                }
                break;

	    case RefValue:
                if (resources[i] != resources2[i]) {
                    msg_failure("1000.%d: Resource 'XmN%s' mismatch:\n\t\texpected %d, received %d\n",
                       i, resource_info[i].name,resources[i], resources2[i]);
                    resources[i] = resources2[i];
                }
                break;

            case RefGeneric:
                if (resources[i] != resources2[i]) {
                    msg_failure("1000.%d: Resource 'XmN%s' mismatch.\n", i,
				resource_info[i].name); /* Don't print addresses */
                    resources[i] = resources2[i];
                }
		break;

            case RefXmString: /* XmStrings loaded & stored internally as
                                 "normal" strings */
	    case RefString:
                if (resources[i] == NULL)
                    if (resources2[i] != NULL) {
                        if (strcmp(resources2[i],"") != 0) {
                            msg_failure("1000.%d: Resource 'XmN%s' mismatch:\n\t\texpected NULL, received '%s'\n",
                                        i, resource_info[i].name,resources2[i]);
                            resources[i] =
                                 mvsCopyResource(widget_info->widget_class_info,
                                                 resources2[i],i);
                        }
		    }
                    else
                        /* do nothing since both are NULL */;
                else 
                    if (resources2[i] == NULL) {
                        if (strcmp(resources[i],"") != 0) {
                            msg_failure("1000.%d: Resource 'XmN%s' mismatch:\n\t\texpected '%s', received NULL\n",
                                        i, resource_info[i].name,resources[i]);
                            resources[i] =
                                 mvsCopyResource(widget_info->widget_class_info,
                                                 resources2[i],i);
                        }
                    }
                    else { /* both are non-null */
                        if (strcmp(resources[i],resources2[i]) != 0) {
                            msg_failure("1000.%d: Resource 'XmN%s' mismatch:\n\t\texpected '%s' received '%s'\n",
                                        i, resource_info[i].name,
                                        resources[i],resources2[i]);
                            resources[i] =
                                 mvsCopyResource(widget_info->widget_class_info,
                                                 resources2[i],i);
                        }
                    }
                break; 

            case RefCallbackList:
                widget = widget_info->widget;
                if ((resources[i] == NULL) ||
                    (((XtCallbackRec *)resources[i])->callback == NULL))
            /** cannot do this check since Xt & Xm put funcs on cb list --    **/
            /**                                                               **/
            /**     if(XtHasCallbacks(widget,resource_info[i].name) !=        **/
            /**                                         XtCallbackHasNone)    **/
            /**        msg_failure("Resource 'XmN%s' mismatch: expected empty **/
            /**               callback list, received non-empty status\n",    **/
            /**               resource_info[i].name);                         **/
            /**     else                                                      **/
                        /* do nothing since both are NULL */;
                else { 
                    if(XtHasCallbacks(widget,resource_info[i].name) !=
                                                            XtCallbackHasSome)
                        msg_failure("1000.%d: Resource 'XmN%s' mismatch:\n\t\texpected non-empy callback list, received empty list\n",
                            i, resource_info[i].name);
                    else { /* both are non-null, test check sum if available */
                        if (mvsCallbackEnum >= 0) {
                            mvsCallbackSum = 0;
            /** cannot do this check since Xt & Xm put funcs on cb list -- **/
            /**                                                            **/
            /**           XtCallCallbacks(widget,resource_info[i].name,0); **/
            /**if (mvsCallbackSum != mvsCallbacksCheckSum[mvsCallbackEnum])**/
            /**    msg_failure("Resource 'XmN%s' mismatch: enumerated      **/
            /**                callback list #%d differs\n",               **/
            /**                resource_info[i].name,mvsCallbackEnum);     **/
                        }
		    }
		}
                break;

	    default:
		sprintf (msg_string, _AutoMessages[WARNMSG64]), 
		         mvsTypeInfo[resource_info[i].type_code].ref_type);
                AutoMessage(msg_string);
                          
	 }
    } /* End for(i<num_resources) */
    mvsFreeResources(widget_info->widget_class_info, resources2);    

} /* End mvsCompareResources() */
