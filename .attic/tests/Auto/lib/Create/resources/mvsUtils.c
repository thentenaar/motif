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
static char rcsid[] = "$XConsortium: mvsUtils.c /main/7 1995/07/14 11:23:52 drk $"
#endif
#endif

/* @(#)mvsUtils.c	1.19.1.1 ver 1/22/91 rel 1 */

#include <AutoMessages.h>
#include "mvslib.h"

/***********************************************************************
  	Author: PSN
	History:
	   05/21/90 PSN First draft.
	Calls:

	Summary:
		This file contains utility functions that used in MDL 
	description to manipulate local copy of the resources. The method 
	of copying is based on the type of each resource.
************************************************************************/


/***********************************************************************
  	Author: PSN
	History:
	   05/21/90 PSN First draft.
	Calls:

	Summary
		This function assigs the value of the source resource to 
	the destination resource. 
************************************************************************/

#define mvsRESOURCE	1
#define mvsINTRES	2

int mvsAssignRes (widget_info, dest_res, src_res)
MvsWidgetInfoRecord 	*widget_info;
char 			src_res[], dest_res[];
{
    int 		src_id, dest_id, src_typ, dest_typ, s_typ;
    MvsArgVal 		*src_val, *dest_val;
    MvsWidgetClassInfo*	widget_class_info = widget_info->widget_class_info;
    static char         routine_name[] = "mvsAssignRes():";
    char                msg_string[100];

    if((src_id=mvsGetResourceNum(widget_class_info,src_res)) != NON_EXISTENT) {
	src_val = &(widget_info->mvs_resources[src_id]);
	src_typ = widget_class_info->resource_info[src_id].type_code;
	s_typ = mvsRESOURCE;
    }
    else
        if((src_id=mvsGetIntResNum(widget_class_info,
                                   src_res)) != NON_EXISTENT) {
            src_val = &(widget_info->int_resources[src_id]);
	    src_typ = widget_class_info->intres_info[src_id].type_code;
	    s_typ = mvsINTRES;
        }
        else {
	    sprintf (msg_string, _AutoMessages[WARNMSG81], routine_name, 
		     src_res);
	    AutoMessage(msg_string);
	    return(-1);
        }

    if((dest_id=mvsGetResourceNum(widget_class_info,dest_res))!= NON_EXISTENT) {
	dest_val = &(widget_info->mvs_resources[dest_id]);
	dest_typ = widget_class_info->resource_info[dest_id].type_code;
	mvsFreeResource (widget_class_info, dest_val, dest_id);
    }
    else
        if((dest_id=mvsGetIntResNum(widget_class_info,
                                    dest_res) ) != NON_EXISTENT ) {
	    dest_val = &(widget_info->int_resources[dest_id]);
	    dest_typ = widget_class_info->intres_info[dest_id].type_code;
	    mvsFreeIntRes (widget_class_info, dest_val, dest_id);
        }
        else {
	    sprintf (msg_string, _AutoMessages[WARNMSG82], routine_name, dest_res);
	    AutoMessage(msg_string);
	    return(-1);
        }

    if ( src_typ != dest_typ ) {
        sprintf (msg_string,_AutoMessages[WARNMSG83], routine_name,src_res,dest_res);
	AutoMessage(msg_string);
	return(-1);
    }
    else {
	*dest_val = ( s_typ == mvsRESOURCE ? 
		      mvsCopyResource (widget_class_info, *src_val, src_id) :
		      mvsCopyIntRes (widget_class_info, *src_val, src_id) );
        return(0);
    }
} /* End mvsAssignRes() */


/***********************************************************************
  	Author: PSN
	History:
	   05/21/90 PSN First draft.
	   05/30/90 PSN Fix function call to mvsSetLocal..
	   07/03/90 PSN Add function mvsAssignParentVal()
	   08/30/90 PSN Add function mvsAssignParentRes()
	   09/14/90 SJS val is already an MvsArgVal, so no need to Convert
	Calls:

	Summary
		This function assigns the constant value to the destination 
	resource. 
************************************************************************/

int mvsAssignVal(widget_info, dest_res, val)
MvsWidgetInfoRecord 	*widget_info;
char 			dest_res[];
MvsArgVal 		val;
{
    int 		dest_id, dest_typ, d_typ;
    MvsArgVal 		*dest_val;
    Arg			args[1];
    Cardinal		num=0;
    MvsWidgetClassInfo	*widget_class_info = widget_info->widget_class_info;
    static char         routine_name[] = "mvsAssignVal():";
    char msg_string[100];

    if((dest_id=mvsGetResourceNum(widget_class_info,dest_res))!= NON_EXISTENT) {
	dest_val = &(widget_info->mvs_resources[dest_id]);
	dest_typ = widget_class_info->resource_info[dest_id].type_code;
	d_typ = mvsRESOURCE;
	mvsFreeResource (widget_class_info, dest_val, dest_id);
    }
    else
        if((dest_id=mvsGetIntResNum(widget_class_info,
                                    dest_res)) != NON_EXISTENT ) {
	    dest_val = &(widget_info->int_resources[dest_id]);
	    dest_typ = widget_class_info->intres_info[dest_id].type_code;
	    d_typ = mvsINTRES;
	    mvsFreeIntRes (widget_class_info, dest_val, dest_id);
        }
        else {
	    sprintf (msg_string,_AutoMessages[WARNMSG84],routine_name,dest_res);
	    AutoMessage(msg_string);
                      
	    return(-1);
        }

    XtSetArg (args[0], dest_res, val ); num++;
    if ( d_typ == mvsRESOURCE ) {
	mvsSetLocalValues (widget_info, args, &num);
    }
    else {
	mvsSetLocalIntResValues (widget_info, args, num);
    }
    return(0);

} /* End mvsAssignVal() */


/***********************************************************************
  	Author: PSN
	History:
	   08/30/90 PSN Add function mvsAssignParentRes()
	Calls:

	Summary
            Assign value to a parent's resource
***********************************************************************/

int mvsAssignParentVal(widget_info, dest_res, val)
MvsWidgetInfoRecord 	*widget_info;
char 			dest_res[];
MvsArgVal 		val;
{
  char msg_string[100];
    if (widget_info->parent == NULL) {
        sprintf (msg_string,_AutoMessages[WARNMSG32], widget_info);
	AutoMessage(msg_string);
	return(0);
    }

    return(mvsAssignVal(widget_info->parent, dest_res, val));

} /* End mvsAssignParentVal() */


/***********************************************************************
  	Author: PSN
	History:
	   08/30/90 PSN Add function mvsAssignParentRes()
	Calls:

	Summary
            Assign resource to a parent's resource 
***********************************************************************/

int mvsAssignParentRes(widget_info, dest_res, src_res)
MvsWidgetInfoRecord 	*widget_info;
char 			dest_res[], src_res[];
{
  char msg_string[100];

    if (widget_info->parent == NULL) {
        sprintf (msg_string,_AutoMessages[WARNMSG33], widget_info);
	AutoMessage(msg_string);
	return(0);
    }

    return(mvsAssignRes(widget_info->parent, dest_res, src_res));

} /* End mvsAssignParentRes() */


/***********************************************************************
  	Author: PSN
	History:
	   05/21/90 PSN First draft.
	Calls:

	Summary
		This function is the core routine to add or subtract source
	resource value to/from the destination resource.
************************************************************************/
#define	mvsADD		1
#define mvsSUBTRACT	2

int mvsIncrOrDecrRes (widget_info, dest_res, src_res, opr)
MvsWidgetInfoRecord 	*widget_info;
char 			src_res[], dest_res[];
int			opr;
{
    int 		src_id, dest_id, src_typ, dest_typ;
    MvsArgVal 		*src_val, *dest_val;
    MvsWidgetClassInfo	*widget_class_info = widget_info->widget_class_info;
    static char         routine_name[] = "mvsIncrOrDecrRes():";
    char msg_string[100];

    if((src_id=mvsGetResourceNum(widget_class_info,src_res)) != NON_EXISTENT ) {
	src_val = &(widget_info->mvs_resources[src_id]);
	src_typ = widget_class_info->resource_info[src_id].type_code;
    }
    else
        if((src_id = mvsGetIntResNum (widget_class_info,
                                      src_res) ) != NON_EXISTENT) {
	src_val = &(widget_info->int_resources[src_id]);
	src_typ = widget_class_info->intres_info[src_id].type_code;
    }
    else {
        sprintf (msg_string,_AutoMessages[WARNMSG81],routine_name,src_res);
	AutoMessage(msg_string);
	return(-1);
    }

    if((dest_id=mvsGetResourceNum(widget_class_info,
                                  dest_res)) != NON_EXISTENT) {
	dest_val = &(widget_info->mvs_resources[dest_id]);
	dest_typ = widget_class_info->resource_info[dest_id].type_code;
    }
    else
        if((dest_id = mvsGetIntResNum (widget_class_info,
                                       dest_res)) != NON_EXISTENT ) {
	    dest_val = &(widget_info->int_resources[dest_id]);
	    dest_typ = widget_class_info->intres_info[dest_id].type_code;
        }
        else {
	    sprintf(msg_string, _AutoMessages[WARNMSG82],routine_name,dest_res);
	    AutoMessage(msg_string);
	    return(-1);
        }

    if ( src_typ != dest_typ ) {
        sprintf(msg_string, _AutoMessages[WARNMSG83],routine_name,src_res,dest_res);
	AutoMessage(msg_string); }
    else {
	if ( mvsTypeInfo[src_typ].ref_type == RefValue )
	    switch (opr) {
		case mvsADD:
			*dest_val += (int) *src_val;
			break;
		case mvsSUBTRACT:
			*dest_val -= (int) *src_val;
			break;
		default:
			sprintf (msg_string, _AutoMessages[WARNMSG85], routine_name);
			AutoMessage(msg_string);
			return (-1);
            }
	return (0);
    }
} /* End mvsIncrOrDecrRes () */


/***********************************************************************
  	Author: PSN
	History:
	   06/05/90 PSN First draft.
	Calls:

	Summary
		This function is the core routine to add or subtract source
	value to/from the destination resource.
************************************************************************/

int mvsIncrOrDecrVal (widget_info, dest_res, src_val, opr)
MvsWidgetInfoRecord 	*widget_info;
char 			dest_res[];
MvsArgVal		src_val;
int			opr;
{
    int 		dest_id, dest_typ;
    MvsArgVal 		*dest_val;
    MvsWidgetClassInfo	*widget_class_info = widget_info->widget_class_info;
    static char         routine_name[] = "mvsIncrOrDecrVal";
    char                msg_string[125];

    if((dest_id=mvsGetResourceNum(widget_class_info,dest_res))!= NON_EXISTENT) {
	dest_val = &(widget_info->mvs_resources[dest_id]);
	dest_typ = widget_class_info->resource_info[dest_id].type_code;
    }
    else
        if((dest_id=mvsGetIntResNum (widget_class_info,
                                     dest_res) ) != NON_EXISTENT ) {
	    dest_val = &(widget_info->int_resources[dest_id]);
	    dest_typ = widget_class_info->intres_info[dest_id].type_code;
        }
        else {
	    sprintf (msg_string, _AutoMessages[WARNMSG82], routine_name, dest_res);
	    AutoMessage(msg_string);
	    return(-1);
        }

    if ( mvsTypeInfo[dest_typ].ref_type == RefValue )
	switch (opr) {
	    case mvsADD:
		*dest_val += (int) src_val;
		break;
	    case mvsSUBTRACT:
		*dest_val -= (int) src_val;
		break;
	    default:
		sprintf (msg_string, _AutoMessages[WARNMSG85], routine_name);
		AutoMessage(msg_string);
		return (-1);
	}
    return (0);

} /* End mvsIncrOrDecrVal () */


/***********************************************************************
  	Author: PSN
	History:
	   05/21/90 PSN First draft.
	Calls:

	Summary
		This function adds the value of the source resource to 
	the destination resource. 
************************************************************************/

void mvsIncrRes (widget_info, dest_res, src_res)
MvsWidgetInfoRecord 	*widget_info;
char 			src_res[], dest_res[];
{
    mvsIncrOrDecrRes (widget_info, dest_res, src_res, mvsADD);
}


/***********************************************************************
  	Author: PSN
	History:
	   05/21/90 PSN First draft.
	Calls:

	Summary
		This function subtracts the value of the source resource from 
	the destination resource. 
************************************************************************/

void mvsDecrRes (widget_info, dest_res, src_res)
MvsWidgetInfoRecord 	*widget_info;
char 			src_res[], dest_res[];
{
    mvsIncrOrDecrRes (widget_info, dest_res, src_res, mvsSUBTRACT);
}


/***********************************************************************
  	Author: PSN
	History:
	   06/05/90 PSN First draft.
	Calls:

	Summary
		This function adds the source value to the destination 
	resource. 
************************************************************************/

void mvsIncrVal (widget_info, dest_res, src_val)
MvsWidgetInfoRecord 	*widget_info;
char 			dest_res[];
MvsArgVal 		src_val;
{
    mvsIncrOrDecrVal (widget_info, dest_res, src_val, mvsADD);
}


/***********************************************************************
  	Author: PSN
	History:
	   06/05/90 PSN First draft.
	Calls:

	Summary
		This function subtracts the source value from the destination 
	resource. 
************************************************************************/

void mvsDecrVal (widget_info, dest_res, src_val)
MvsWidgetInfoRecord 	*widget_info;
char 			dest_res[];
MvsArgVal		src_val;
{
    mvsIncrOrDecrVal (widget_info, dest_res, src_val, mvsSUBTRACT);
}


/***********************************************************************
  	Author: PSN
	History:
	   07/03/90 PSN First draft.
	Calls:

	Summary
		This function copies the source resource to the the destination 
	resource in the widget referred to by widget_res in current widget. 
************************************************************************/

int mvsAssignResInWidget (widget_info, widget_res, dest_res, src_res)
MvsWidgetInfoRecord 	*widget_info;
char 			widget_res[], src_res[], dest_res[];
{
    Widget		widget;
    char                msg_string[100];

    widget = (Widget)mvsGetResourceValue(widget_info->widget, widget_res, 
                                         T_Widget_ref);

    if (widget == NULL) {
        sprintf(msg_string,_AutoMessages[WARNMSG34], widget_res);
	AutoMessage(msg_string);
	return(-1);
    }

    return( mvsAssignRes( mvsWidgetToWidgetInfo(widget), dest_res, src_res ) );

} /* End mvsAssignResInWidget() */


/***********************************************************************
  	Author: PSN
	History:
	   08/13/90 PSN First draft.
	Calls:

	Summary
		This function copies the source value to the the destination 
	resource in the widget referred to by widget_res in current widget. 
************************************************************************/

int mvsAssignValInWidget (widget_info, widget_res, dest_res, val)
MvsWidgetInfoRecord 	*widget_info;
char 			widget_res[], dest_res[];
MvsArgVal 		val;
{
    Widget		widget;
    char                msg_string[100];

    widget = (Widget)mvsGetResourceValue(widget_info->widget, widget_res,
                                                                T_Widget_ref);

    if (widget == NULL) {
        sprintf(msg_string, _AutoMessages[WARNMSG35], widget_res);
	AutoMessage(msg_string);
        return(-1);
    }

    return(mvsAssignVal(mvsWidgetToWidgetInfo(widget), dest_res, val));

} /* End mvsAssignValInWidget() */


/***********************************************************************
  	Author: PSN
	History:
	   11/28/90 PSN First draft.
	Calls:

	Summary
		This function copies the source value to the the destination 
	resource in the widget referred to by widget_res in the parent of the
	current widget. 
************************************************************************/

int mvsAssignValInParentWidget (widget_info, widget_res, dest_res, val)
MvsWidgetInfoRecord 	*widget_info;
char 			widget_res[], dest_res[];
MvsArgVal 		val;
{
    Widget		widget;
    int         	idx1, idx2;
    char                msg_string[100];

    widget_info = widget_info->parent;

    while (1==1) {

        /* Find widget's widget_info throught its reference in resource */
        if ((idx1 = mvsGetResourceNum(widget_info->widget_class_info,
                                                widget_res)) < 0 &&
            (idx2 = mvsGetIntResNum(widget_info->widget_class_info,
                                              widget_res)) < 0 )
           break;

        if (!(idx1 >= 0 &&
             (widget = (Widget)mvsGetLocalResource(widget_info, widget_res))
                        != NULL) &&
            !(idx2 >= 0 &&
             (widget = (Widget)mvsGetInternalResource(widget_info,
                                                      widget_res)) != NULL))
           break;

	return(mvsAssignVal(mvsWidgetToWidgetInfo(widget), dest_res, val));
    }

    sprintf(msg_string, _AutoMessages[WARNMSG36]);
    AutoMessage(msg_string);
    return(-1);

} /* End mvsAssignValInParentWidget() */


/***********************************************************************
  	Author: PSN
	History:
	   11/28/90 PSN First draft.
	Calls:

	Summary
		This function copies the source value to the the destination 
	resource in the widget referred to by widget_res in the widget referred
	to by the resource widget_ref in the current widget. 
************************************************************************/

int mvsAssignValInWidRefWidget (widget_info, widget_ref, widget_res, dest_res, val)
MvsWidgetInfoRecord 	*widget_info;
char 			widget_ref[], widget_res[], dest_res[];
MvsArgVal 		val;
{
    Widget		widget;
    int         	idx1, idx2;
    char                msg_string[100];

    while (1==1) {

        /* Find widget_ref widget_info through its reference in resource */
        if ((idx1 = mvsGetResourceNum(widget_info->widget_class_info,
                                                widget_ref)) < 0 &&
            (idx2 = mvsGetIntResNum(widget_info->widget_class_info,
                                                widget_ref)) < 0)
           break;

        if (!(idx1 >= 0 &&
             (widget = (Widget)mvsGetLocalResource(widget_info, widget_ref))
                        != NULL) &&
            !(idx2 >= 0 &&
             (widget = (Widget)mvsGetInternalResource(widget_info,
                                                      widget_ref)) != NULL))
           break;

        if ((widget_info = mvsWidgetToWidgetInfo(widget)) == NULL)
           break;

        /* Find widget's widget_info throught its reference in resource */
        if ((idx1 = mvsGetResourceNum(widget_info->widget_class_info,
                                                widget_res)) < 0 &&
            (idx2 = mvsGetIntResNum(widget_info->widget_class_info,
                                              widget_res)) < 0 )
           break;

        if (!(idx1 >= 0 &&
             (widget = (Widget)mvsGetLocalResource(widget_info, widget_res))
                        != NULL) &&
            !(idx2 >= 0 &&
             (widget = (Widget)mvsGetInternalResource(widget_info,
                                                      widget_res)) != NULL))
           break;

	if (widget == NULL) 
	   break;

	return(mvsAssignVal(mvsWidgetToWidgetInfo(widget), dest_res, val));
    }

    sprintf(msg_string, _AutoMessages[WARNMSG37]);
    AutoMessage(msg_string);
    return(-1);

} /* End mvsAssignValInWidRefWidget() */
