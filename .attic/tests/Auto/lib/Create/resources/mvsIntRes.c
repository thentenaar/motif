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
static char rcsid[] = "$XConsortium: mvsIntRes.c /main/7 1995/07/14 11:21:40 drk $"
#endif
#endif
/***********************************************************************
  	@(#)mvsIntRes.c	1.13.1.1	Date:1/22/91
  	Author: PSN
	History:
	   05/16/90 PSN First draft.
	   05/29/90 SJS Someone added some cosmetic changes
	   07/13/90 SJS add XmString changes
	   09/26/90 SJS eliminate unused/unneeded functions
	Calls:

	Summary:
		This file contains function which copy internal resources.
	The method of copying is based on the type of each resource and
	a pointer/value is returned based on this type.
************************************************************************/

#include <AutoMessages.h>
#include "mvslib.h"

/************************************************************************
*   This function copies One resource. The resource is copied           *
*   based on its type and a pointer/value is returned.                  *
************************************************************************/

MvsArgVal mvsCopyIntRes (widget_class_info, src_resource, i) 
MvsWidgetClassInfo 	*widget_class_info;
MvsArgVal 		src_resource; 
int 			i;
{ 
    MvsArgVal 		dst_resource;
    MvsIntResInfoRecord *intres_info;
    char msg_string[125];

    intres_info = widget_class_info->intres_info;

    switch (mvsTypeInfo[intres_info[i].type_code].ref_type) {

            case RefXmStringTable: /* becomes clear */
	    case RefValue: 	 
            case RefGeneric:
            case RefBoolean: 
                dst_resource = src_resource; 
                break;

	    case RefXmString:
	    case RefString: 
            { 
                char *str = (char *)src_resource; 
                if (str != NULL) { 
                    dst_resource = (MvsArgVal)XtMalloc(strlen(str)+1);
                    strcpy( (char *)dst_resource,str); 
                } 
                else dst_resource = 0L; 
                break; 	 
            }

	    case RefCallbackList: 	 
            { 
                XtCallbackList cb_list = (XtCallbackList)src_resource; 
                int j=0; 
                if (cb_list != NULL) { 
                    while (cb_list->callback != NULL) { 
                        cb_list++; j++; 
                    } 
                    cb_list = (XtCallbackList)src_resource; 
                    dst_resource = (MvsArgVal)XtMalloc(sizeof(XtCallbackRec)*(j+1));
                    memcpy((char *)dst_resource,cb_list,sizeof(XtCallbackRec)*(j+1));
                }
                else dst_resource = 0L;
                break;
   	    } 

	    default: 
                dst_resource = src_resource; 
		sprintf (msg_string, _AutoMessages[WARNMSG67], mvsTypeInfo[intres_info[i].type_code].ref_type);
                AutoMessage(msg_string);
                          
                break;
    }

    return (dst_resource);
} /* End mvsCopyIntRes () */

 

/*************************************************************************
*  This function copies all resources. The entire list of resources is   *
* looped through and each resource is copied using mvsCopyResource().    *
*************************************************************************/

void mvsCopyIntResources(widget_class_info, dst_resource,src_resource)
MvsWidgetClassInfo 	*widget_class_info;
MvsArgVal 		dst_resource[], src_resource[]; 
{ 
    int 		i;

    for ( i=0 ; i<widget_class_info->num_intres ; i++ )
        dst_resource[i] = mvsCopyIntRes (widget_class_info, src_resource[i], i);

} /* End mvsCopyIntResources () */

 

/***********************************************************************
  	Author: PSN
	History:
	   05/16/90 PSN First draft.
	Calls:

	Summary:
		This function frees up all the internal resources associated 
	with the current widget state. Only the internal resources which are 
	pointers require any processing. The memory pointed to by the internal
	resources must be freed because it is dynamically allocated as each new
	widget state is entered.
************************************************************************/

void mvsFreeIntRes (widget_class_info, presource, i)
MvsWidgetClassInfo 	*widget_class_info;
MvsArgVal 		*presource;
int 			i;
{
    MvsIntResInfoRecord *intres_info;
    char msg_string[125];

    intres_info = widget_class_info->intres_info;

    switch (mvsTypeInfo[intres_info[i].type_code].ref_type) {

	case RefValue:
        case RefGeneric:
        case RefBoolean:
            /* Do nothing since it did not malloc a value for this */
	    if (presource)	*presource = 0L;
            break;

	case RefXmString:
	case RefString:
	case RefCallbackList:
	    if (presource && *presource) {
                XtFree( (char *)(*presource));
	        *presource = 0L;
	    }
            break;

	default:
            sprintf (msg_string, _AutoMessages[WARNMSG71], mvsTypeInfo[intres_info[i].type_code].ref_type);
            AutoMessage(msg_string);
                       
            break;
    }

} /* End mvsFreeIntRes () */
 

/***********************************************************************
  	Author: PSN
	History:
	   05/16/90 PSN First draft.
	Calls:

	Summary:
		This function frees up all the internal resources associated 
	with the current widget state. Only the internal resources which are 
	pointers require any processing. The memory pointed to by the internal
	resources must be freed because it is dynamically allocated as each new
	widget state is entered.
************************************************************************/

void mvsFreeIntResources (widget_class_info, resources)
MvsWidgetClassInfo 	*widget_class_info;
MvsArgVal 		resources[];
{
    int 		i;
    short 		num_intres;
    char                msg_string[125];
    
    num_intres = widget_class_info->num_intres;

    for (i=0; i<num_intres; i++)
        mvsFreeIntRes (widget_class_info, &(resources[i]), i);

} /* End mvsFreeIntResources () */
 

/***********************************************************************
  	Author: PSN
	History:
	   05/16/90 PSN First draft.
	Calls:

	Summary
		This function obtains the value of an internal resource 
	according to what MVS believes it currently to be. This may be 
	different than what is really true.
************************************************************************/

MvsArgVal mvsGetInternalResource (widget_info, resource_name)
MvsWidgetInfoRecord 	*widget_info;
char 			resource_name[];
{
    int 		i;
    short 		num_intres;
    MvsIntResInfoRecord *intres_info;

    char msg_string[125];

    num_intres = widget_info->widget_class_info->num_intres;
    intres_info = widget_info->widget_class_info->intres_info;

    for(i=0; i<num_intres && strcmp(resource_name,intres_info[i].name); i++);

    if ( i >= num_intres ) {
        sprintf (msg_string, _AutoMessages[WARNMSG72], resource_name);
        AutoMessage(msg_string);
        return(0);
    }

    return( widget_info->int_resources[i] );        

} /* End mvsGetInternalResource () */


/***********************************************************************
        Author: PSN
        History:
           07/20/90 PSN First draft.
        Calls:
 
        Summary:
           Get parent internal resource.
************************************************************************/
MvsArgVal mvsGetParentIntRes(widget_info, resource_name)
MvsWidgetInfoRecord *widget_info;
char resource_name[];
{
    MvsWidgetInfoRecord *pwidget_info = widget_info->parent;
    char msg_string[125];

    if (pwidget_info == NULL) {
        sprintf (msg_string, _AutoMessages[WARNMSG73], resource_name);
        AutoMessage(msg_string);
        return(0);
    }
    return( mvsGetInternalResource(pwidget_info, resource_name) );

} /* End mvsGetParentIntRes() */


/***********************************************************************
  	Author: PSN
	History:
	   05/16/90 PSN First draft.
	   09/20/90 SJS use NON_EXISTENT.
	Calls:

	Summary:
		This function returns the index number of an internal resource
	within the intres_info structure. A NON_EXISTENT(-1) is returned if the
	requested resource name was not located.
************************************************************************/

int mvsGetIntResNum (widget_class_info, resource_name)
MvsWidgetClassInfo 	*widget_class_info;
char 			resource_name[];
{
    int 		j;
    short 		num_intres;
    MvsIntResInfoRecord *intres_info;

    num_intres = widget_class_info->num_intres;
    intres_info = widget_class_info->intres_info;

    for (j=0 ; j<num_intres && strcmp(resource_name,intres_info[j].name) ; j++);

    return ( j < num_intres ? j : (NON_EXISTENT) );
        
} /* End mvsGetIntResNum () */
 

/***********************************************************************
  	Author: PSN
	History:
	   05/16/90 PSN First Draft.
	Calls:

	Summary:
		This function sets the local copy of the intres values.
	These are what the MVS thinks the widget intres values should be.
	For pointer intres, memory is allocated and structures copied.
************************************************************************/

void mvsSetLocalIntResValues(widget_info, args, nargs)
MvsWidgetInfoRecord 	*widget_info;
ArgList 		args;
Cardinal 		nargs;
{
    int 		i,j;
    short 		num_intres;
    MvsIntResInfoRecord *intres_info;
    MvsArgVal 		*int_resources;
    static char         routine_name[] = "mvsSetLocalIntResValues():";

    num_intres = widget_info->widget_class_info->num_intres;
    intres_info = widget_info->widget_class_info->intres_info;
    int_resources = widget_info->int_resources;

    for ( i=0 ; i<nargs ; i++ ) {
        for ( j=0 ; j<num_intres && 
		    strcmp ( args[i].name, intres_info[j].name ) ; j++ );
        if ( j >= num_intres )
            AutoMessage("%s Bad arg name '%s'\n", routine_name, args[i].name);
        else {
            switch ( mvsTypeInfo[intres_info[j].type_code].ref_type ) {
	        case RefBoolean:
	        case RefValue:
                case RefGeneric:
                    int_resources[j] = (MvsArgVal)args[i].value;
                    break;
   
	        case RefString:
                    if(int_resources[j]) {
                        XtFree((char *)(int_resources[j]));
			int_resources[j] = 0L;
		    }

                    if (args[i].value) {
                        int_resources[j] = (MvsArgVal)
                                              XtMalloc(
					       strlen((char *)args[i].value)+1);
                        strcpy((char *)int_resources[j],(char *)args[i].value);
                    }
                    else int_resources[j] = 0L;
                    break;

	        case RefCallbackList:
		{
                    XtCallbackList cb_list = (XtCallbackList)args[i].value;
                    int k=0;

                    if(int_resources[j]) {
                        XtFree((char *)(int_resources[j]));
			int_resources[j] = 0L;
		    }

                    if (cb_list != NULL) {
                        while (cb_list->callback != NULL) {
                            cb_list++;
                            k++;
                        }
                        cb_list = (XtCallbackList)args[i].value;

                        int_resources[j] = (MvsArgVal)
                                          XtMalloc(sizeof(XtCallbackRec)*(k+1));
                        memcpy((char *)int_resources[j],(char *)cb_list,
                               sizeof(XtCallbackRec)*(k+1));
                    }
                    else int_resources[j] = 0L;

                    /*** remove callback from list *****/

                    for (k=i; k<nargs-1; k++) {
                        args[k].name = args[k+1].name;
                        args[k].value = args[k+1].value;
                    }

                    if (i != nargs-1)
                        i--;

                    break;
		}

	        default:
		    
                    AutoMessage("%s Bad type code=%d\n", routine_name,
                              mvsTypeInfo[intres_info[j].type_code].ref_type);
                    break;
	    }
	}
    } /* End for() */

} /* End mvsSetLocalIntResValues() */


/***********************************************************************
  	Author: SJS
	History:
	   09/26/90 SJS create
	Calls:

	Summary:
		This function sets internal resources to their default
	values.
************************************************************************/

void mvsSetIntToDefaults(widget_info)
MvsWidgetInfoRecord 	*widget_info;
{
    int 		j;
    short 		num_intres;
    MvsIntResInfoRecord *intres_info;

    num_intres = widget_info->widget_class_info->num_intres;
    intres_info = widget_info->widget_class_info->intres_info;

    for (j=0; j<num_intres; j++) {
        if(intres_info[j].needs_converting) {
            AutoMessage(_AutoMessages[WARNMSG76]);
            widget_info->int_resources[j] = 0;
        }
        else {
            widget_info->int_resources[j] =
                         mvsCopyIntRes(widget_info->widget_class_info,
                                       intres_info[j].default_value, j);
        }
    }
        
} /* End mvsSetIntToDefaults() */
