/* 
 * Motif
 *
 * Copyright (c) 1987-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2.1 of the License, or (at your option)
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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$XConsortium: Mrmwwrite.c /main/11 1996/11/13 14:08:21 drk $"
#endif
#endif

/*
 *++
 *  FACILITY:
 *
 *      UIL Resource Manager (URM):
 *
 *  ABSTRACT:
 *
 *	This module contains all the routines which write a widget
 *	from a resource context into an IDB file.
 *
 *--
 */


/*
 *
 *  INCLUDE FILES
 *
 */


#include <Mrm/MrmAppl.h>
#include <Mrm/Mrm.h>
#include "MrmMsgI.h"


/*
 *
 *  TABLE OF CONTENTS
 *
 *	UrmPutIndexedWidget		Write indexed widget to IDB file
 *
 *	UrmPutRIDWidget			Write RID widget to IDB file
 *
 */



/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	UrmPutIndexedWidget puts a URMgWidget resource record in the
 *	database. Its contents are the widget record. The resource type is
 *	the value of the widget record's type field. The access is the
 *	value of the record's access field; the locking attribute is the
 *	value of the record's lock field.
 *
 *  FORMAL PARAMETERS:
 *
 *	file_id		file into which to write record
 *	index		case-sensitive index for the widget
 *	context_id	resource context containing widget record
 *
 *  IMPLICIT INPUTS:
 *
 *  IMPLICIT OUTPUTS:
 *
 *  FUNCTION VALUE:
 *
 *	MrmSUCCESS	operation succeeded
 *	MrmBAD_CONTEXT	invalid resource context
 *	MrmBAD_WIDGET_REC	invalid widget record in context
 *	Others:		See UrmIdbPutIndexedResource
 *
 *  SIDE EFFECTS:
 *
 *--
 */

Cardinal 
UrmPutIndexedWidget (IDBFile			file_id ,
		     String			index ,
		     URMResourceContextPtr	context_id )
{
  Cardinal		result ;	/* function results */
  RGMWidgetRecordPtr	widgetrec ;	/* Widget record in context */


  /*
   * Validate record, then set resource group and type, and enter in
   * IDB file.
   */
  if ( ! UrmRCValid(context_id) )
    return Urm__UT_Error ("UrmPutIndexedWidget", _MrmMMsg_0043,
			  file_id, NULL, MrmBAD_CONTEXT) ;
  widgetrec = (RGMWidgetRecordPtr) UrmRCBuffer (context_id) ;
  if ( ! UrmWRValid(widgetrec) )
    return Urm__UT_Error ("UrmPutIndexedWidget", _MrmMMsg_0026,
			  file_id, context_id, MrmBAD_WIDGET_REC) ;

  UrmRCSetSize (context_id, widgetrec->size) ;
  UrmRCSetGroup (context_id, URMgWidget) ;
  UrmRCSetType (context_id, widgetrec->type) ;
  UrmRCSetAccess (context_id, widgetrec->access) ;
  UrmRCSetLock (context_id, widgetrec->lock) ;

  result = UrmIdbPutIndexedResource (file_id, index, context_id) ;
  return result ;

}



/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	UrmPutRIDWidget puts a widget accessed by a resource id into the
 *	given database file. The resource type is the value of the widget
 *	record's type field. As usual for resources accessed by resource id,
 *	the access is always private; this routine resets the record's access
 *	field if necessary to be consistent with private access. The value
 *	of the locking attribute is the value of the record's lock field.
 *
 *  FORMAL PARAMETERS:
 *
 *	file_id		file into which to write record
 *	resource_id	resource id for the record
 *	context_id	resource context containing widget record
 *
 *  IMPLICIT INPUTS:
 *
 *  IMPLICIT OUTPUTS:
 *
 *  FUNCTION VALUE:
 *
 *	MrmSUCCESS	operation succeeded
 *	MrmBAD_CONTEXT	invalid resource context
 *	MrmBAD_WIDGET_REC	invalid widget record in context
 *	Others:		See UrmIdbPutRIDResource
 *
 *  SIDE EFFECTS:
 *
 *--
 */

Cardinal 
UrmPutRIDWidget (IDBFile		file_id ,
		 MrmResource_id		resource_id ,
		 URMResourceContextPtr	context_id )
{
  Cardinal		result ;	/* function results */
  RGMWidgetRecordPtr	widgetrec ;	/* Widget record in context */


  /*
   * Validate record, then set resource group and type, and enter in
   * IDB file.
   */
  if ( ! UrmRCValid(context_id) )
    return Urm__UT_Error ("UrmPutRIDWidget", _MrmMMsg_0043,
			  file_id, NULL, MrmBAD_CONTEXT) ;
  widgetrec = (RGMWidgetRecordPtr) UrmRCBuffer (context_id) ;
  if ( ! UrmWRValid(widgetrec) )
    return Urm__UT_Error ("UrmPutRIDWidget", _MrmMMsg_0026,
			  file_id, context_id, MrmBAD_WIDGET_REC) ;

  UrmRCSetSize (context_id, widgetrec->size) ;
  UrmRCSetGroup (context_id, URMgWidget) ;
  UrmRCSetType (context_id, widgetrec->type) ;
  UrmRCSetAccess (context_id, URMaPrivate) ;
  UrmRCSetLock (context_id, widgetrec->lock) ;

  result = UrmIdbPutRIDResource (file_id, resource_id, context_id) ;
  return result ;

}

