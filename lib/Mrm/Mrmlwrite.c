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
static char rcsid[] = "$XConsortium: Mrmlwrite.c /main/11 1996/11/13 14:02:48 drk $"
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
 *	This module contains all the routines which write a Literal
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
 *	UrmPutIndexedLiteral		Write indexed literal to IDB file
 *
 *	UrmPutRIDLiteral		Write RID literal to IDB file
 *
 */



/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	UrmPutIndexedLiteral puts a URMgLiteral resource record in the
 *	database. Its content is the literal. The resource type, access,
 *	and locking attributes are assumed to be already set.
 *
 *  FORMAL PARAMETERS:
 *
 *	file_id		file into which to write record
 *	index		case-sensitive index for the literal
 *	context_id	resource context containing literal
 *
 *  IMPLICIT INPUTS:
 *
 *  IMPLICIT OUTPUTS:
 *
 *  FUNCTION VALUE:
 *
 *	MrmSUCCESS	operation succeeded
 *	MrmBAD_CONTEXT	invalid resource context
 *	Others:		See UrmIdbPutIndexedResource
 *
 *  SIDE EFFECTS:
 *
 *--
 */

Cardinal 
UrmPutIndexedLiteral (IDBFile			file_id ,
		      String			index ,
		      URMResourceContextPtr	context_id )
{

  /*
   *  Local variables
   */
  Cardinal	result;	/* return status */

  /*
   * Validate record, then set resource group and enter in the
   * IDB file.
   */
  if ( ! UrmRCValid(context_id) )
    return Urm__UT_Error ("UrmPutIndexedLiteral", _MrmMMsg_0043,
			  file_id, context_id, MrmBAD_CONTEXT) ;

  UrmRCSetGroup (context_id, URMgLiteral) ;
  /*
   * The size, type, access, and lock fields should have already be
   * set.
   *
   *UrmRCSetSize (context_id, ) ;
   *UrmRCSetType (context_id, ) ;
   *UrmRCSetAccess (context_id, ) ;
   *UrmRCSetLock (context_id, ) ;
   */
  result = UrmIdbPutIndexedResource (file_id, index, context_id) ;
  return result ;

}



/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	UrmPutRIDLiteral puts a literal accessed by a resource id into the
 *	database. Its content is the literal. The resource type, access,
 *	and locking attributes are assumed to be already set.
 *
 *
 *  FORMAL PARAMETERS:
 *
 *	file_id		file into which to write record
 *	resource_id	resource id for the record
 *	context_id	resource context containing literal 
 *
 *  IMPLICIT INPUTS:
 *
 *  IMPLICIT OUTPUTS:
 *
 *  FUNCTION VALUE:
 *
 *	MrmSUCCESS	operation succeeded
 *	MrmBAD_CONTEXT	invalid resource context
 *	Others:		See UrmIdbPutRIDResource
 *
 *  SIDE EFFECTS:
 *
 *--
 */

Cardinal 
UrmPutRIDLiteral (IDBFile		file_id ,
		  MrmResource_id	resource_id ,
		  URMResourceContextPtr	context_id )
{

  /*
   *  Local variables
   */
  Cardinal		result ;	/* function results */


  /*
   * Validate record, then set resource group and enter in the
   * IDB file.
   */
  if ( ! UrmRCValid(context_id) )
    return Urm__UT_Error ("UrmPutRIDLiteral", _MrmMMsg_0043,
			  file_id, context_id, MrmBAD_CONTEXT) ;

  UrmRCSetGroup (context_id, URMgLiteral) ;
  UrmRCSetAccess (context_id, URMaPrivate) ;

  /*
   * The size, type, and lock fields are assumed to be set.
   *
   *UrmRCSetSize (context_id, ) ;
   *UrmRCSetType (context_id, ) ;
   *UrmRCSetLock (context_id, ) ;
   */

  result = UrmIdbPutRIDResource (file_id, resource_id, context_id) ;
  return result ;

}

