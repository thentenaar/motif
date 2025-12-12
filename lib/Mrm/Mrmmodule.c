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
static char rcsid[] = "$XConsortium: Mrmmodule.c /main/12 1996/11/13 14:03:12 drk $"
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
 *	These routines get and put data entries from a record into a buffer.
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
 *	UrmIFMInitModule		- Init module in a context
 *
 *	UrmIFMSetTopmost		- Set a topmost widget in module
 *
 *	UrmIFMPutModule			- Write module to IDB file
 *
 *	UrmIFMHGetModule		- Get module from hierarchy
 *
 *	UrmIFMGetModule			- Get module from IDB file
 *
 *
 */


/*
 *
 *  MACROS
 *
 */

/*
 * Macro to validate a resource context and bind interface module pointer
 *
 * Formal parameters:
 *	ctx_ptr		variable bound to context pointer
 *	routine		quoted string giving routine name
 *	recptr		variable to be bound to module pointer
 *
 * WARNING: Don't use expressions where variables are expected!
 *
 */
#define	_validate_context(ctx_ptr,routine,recptr)		\
 {								\
  if ( ! UrmRCValid(ctx_ptr) )					\
    return Urm__UT_Error (routine, _MrmMMsg_0043,		\
			  NULL, ctx_ptr, MrmBAD_CONTEXT) ;	\
  recptr = (RGMModuleDescPtr) UrmRCBuffer (ctx_ptr) ;		\
 }


/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	UrmIFMInitModule initializes a context for creating a new
 *	interface module. It stores the access and lock attributes, and
 *	sizes and initializes the topmost widget descriptors for the number
 *	of widgets specified.
 *
 *	This routine sets all the context parameters, including group,
 *	type, and size.
 *
 *  FORMAL PARAMETERS:
 *
 *	context_id	URM resource context in which to build the
 *			interface module
 *	num_widget	number of topmost widgets to allocate in module
 *	access		The module's access attribute, URMaPublic or
 *			URMaPrivate
 *	lock		The module's locking attribute, TRUE to lock the
 *			module
 *
 *  IMPLICIT INPUTS:
 *
 *  IMPLICIT OUTPUTS:
 *
 *  FUNCTION VALUE:
 *
 *	MrmSUCCESS	operation succeeded
 *	MrmBAD_CONTEXT	invalid resource context
 *	MrmTOO_MANY	number of indexes exceeds internal limit
 *	MrmFAILURE	unexplained failure
 *
 *  SIDE EFFECTS:
 *
 *--
 */

Cardinal 
UrmIFMInitModule (URMResourceContextPtr		context_id,
		  MrmCount			num_widget,
		  MrmCode			access,
		  MrmCode			lock)
{
  /*
   *  Local variables
   */
  Cardinal		result ;	/* Function results */
  RGMModuleDescPtr	ifmodptr ;	/* IF module in context */
  MrmCount		descsiz ;	/* # bytes for descriptor */
  int			ndx ;		/* loop index */


  /*
   * validate context and bind pointer. Size check.
   */
  _validate_context (context_id, "UrmIFMInitModule", ifmodptr) ;
  if ( num_widget >= RGMListSizeMax )
    return Urm__UT_Error ("UrmIFMInitModule", _MrmMMsg_0044,
			  NULL, context_id, MrmTOO_MANY) ;

  /*
   * compute descriptor size and guarantee space. Null all the widgets.
   */
  descsiz = sizeof(RGMModuleDesc) + (num_widget-1)*sizeof(RGMTopmostDesc) ;
  descsiz = _FULLWORD (descsiz) ;

  result = UrmResizeResourceContext (context_id, descsiz) ;
  if ( result != MrmSUCCESS ) return result ;

  ifmodptr = (RGMModuleDescPtr) UrmRCBuffer(context_id) ;
  ifmodptr->validation = URMInterfaceModuleValid ;
  ifmodptr->count = num_widget ;
  ifmodptr->annex1 = 0 ;
  ifmodptr->annex2 = 0 ;

  for ( ndx=0 ; ndx<num_widget ; ndx++ )
    strcpy (ifmodptr->topmost[ndx].index, "") ;

  /*
   * Set context parameters
   */
  UrmRCSetSize (context_id, descsiz) ;
  UrmRCSetGroup (context_id, URMgResourceSet) ;
  UrmRCSetType (context_id, URMrsInterfaceModule) ;
  UrmRCSetAccess (context_id, access) ;
  UrmRCSetLock (context_id, lock) ;

  /*
   * Successfully created
   */
  return MrmSUCCESS ;

}



/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	This routine sets the topmost widget index for one of the topmost
 *	widgets specified by an interface module.
 *
 *  FORMAL PARAMETERS:
 *
 *	context_id	resource context holding a valid interface module
 *	topmost_ndx	0-based index of the widget in the module
 *	index		the widget's index (name)
 *
 *  IMPLICIT INPUTS:
 *
 *  IMPLICIT OUTPUTS:
 *
 *  FUNCTION VALUE:
 *
 *	MrmSUCCESS	operation succeeded
 *	MrmBAD_IF_MODULE	invalid interface module
 *	MrmOUT_OF_BOUNDS	topmost_ndx out of bounds
 *
 *  SIDE EFFECTS:
 *
 *--
 */

Cardinal 
UrmIFMSetTopmost (URMResourceContextPtr		context_id ,
		  Cardinal			topmost_ndx ,
		  String			index )

{

  /*
   *  Local variables
   */
  RGMModuleDescPtr	ifmodptr ;	/* IF module in context */


  /*
   * Validate context and interface module. Make sure the index is in bounds
   */
  _validate_context (context_id, "UrmIFMSetTopmost", ifmodptr) ;
  if ( ifmodptr->validation != URMInterfaceModuleValid )
    return Urm__UT_Error ("UrmIFMSetTopmost", _MrmMMsg_0025,
			  NULL, context_id, MrmBAD_IF_MODULE) ;

  if ( topmost_ndx >= ifmodptr->count )
    return Urm__UT_Error ("UrmIFMSetTopmost", _MrmMMsg_0045,
			  NULL, context_id, MrmOUT_OF_BOUNDS) ;

  /*
   * Copy in index, and return
   */
  strncat (ifmodptr->topmost[topmost_ndx].index, index, URMMaxIndexLen) ;

  return MrmSUCCESS ;

}


/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	UrmIFMPutModule creates a resource set entry of type interface
 *	module in the given IDB file. The access and locking attriutes
 *	are taken from the context.
 *
 *  FORMAL PARAMETERS:
 *
 *	file_id		IDB file into which to write the interface module
 *	index		index (name) under which to enter the module
 *	context_id	URM resource context containing the module
 *			representation. This must be a valid interface
 *			module.
 *
 *  IMPLICIT INPUTS:
 *
 *  IMPLICIT OUTPUTS:
 *
 *	MrmSUCCESS	Operation succeeded
 *	MrmBAD_CONTEXT	invalid resource context
 *	MrmBAD_IF_MODULE	invalid interface module in context
 *	Others		see UrmIdbPutIndexedResource
 *
 *  FUNCTION VALUE:
 *
 *  SIDE EFFECTS:
 *
 *--
 */

Cardinal 
UrmIFMPutModule (IDBFile		file_id ,
		 String			index ,
		 URMResourceContextPtr	context_id )
{

  /*
   *  Local variables
   */
  Cardinal		result ;	/* Function results */
  RGMModuleDescPtr	ifmodptr ;	/* IF module in context */


  /*
   * Validate context and interface module.
   */
  _validate_context (context_id, "UrmIFMPutModule", ifmodptr) ;
  if ( ifmodptr->validation != URMInterfaceModuleValid )
    return Urm__UT_Error ("UrmIFMPutModule", _MrmMMsg_0025,
			  NULL, context_id, MrmBAD_IF_MODULE) ;

  /*
   * Make sure the context is properly set, then put the module in the file
   */
  UrmRCSetGroup (context_id, URMgResourceSet) ;
  UrmRCSetType (context_id, URMrsInterfaceModule) ;

  result = UrmIdbPutIndexedResource (file_id, index, context_id) ;
  return result ;

}



/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	UrmIFMHGetModule searches the database hierarchy for the module
 *	given its index. It returns the module descriptor in the resource
 *	context.
 *
 *  FORMAL PARAMETERS:
 *
 *	hierarchy_id	id of an open URM database hierarchy
 *	index		index (name) of the desired interface module
 *	context_id	resource context into which to read the module
 *	file_id_return	to return IDB file in which the module is found
 *
 *  IMPLICIT INPUTS:
 *
 *  IMPLICIT OUTPUTS:
 *
 *  FUNCTION VALUE:
 *
 *	MrmSUCCESS	module found and loaded in context
 *	MrmNOT_FOUND	module not found in hierarchy
 *	MrmBAD_HIERARCHY	hierarchy not valid
 *	Others		See UrmGetModule
 *
 *  SIDE EFFECTS:
 *
 *--
 */

Cardinal 
UrmIFMHGetModule (MrmHierarchy		hierarchy_id ,
		  String		index ,
		  URMResourceContextPtr	context_id ,
		  IDBFile		*file_id_return )
{

  /*
   *  Local variables
   */
  Cardinal		result ;	/* function results */

  /*
   * Get the module
   */
  result = UrmHGetIndexedResource
    (hierarchy_id, index, URMgResourceSet, URMrsInterfaceModule,
     context_id, file_id_return) ;

  return result ;

}



/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	UrmIFMGetModule retrieves an interface module from an IDB file, and
 *	reads it into the given resource context.
 *
 *  FORMAL PARAMETERS:
 *
 *	file_id		open IDB file from which to retrieve module
 *	index		index (name) of the desired interface module
 *	context_id	resource context into which to read the module
 *
 *  IMPLICIT INPUTS:
 *
 *  IMPLICIT OUTPUTS:
 *
 *  FUNCTION VALUE:
 *
 *	MrmSUCCESS	operation succeeded
 *	DMBadContext	invalid resource context
 *	MrmBAD_IF_MODULE	invalid interface module found in file
 *	Others		see UrmIdbGetIndexedResource
 *
 *  SIDE EFFECTS:
 *
 *--
 */

Cardinal 
UrmIFMGetModule (IDBFile		file_id ,
		 String			index ,
		 URMResourceContextPtr	context_id )
{

  /*
   *  Local variables
   */
  Cardinal		result ;	/* function results */
  RGMModuleDescPtr	ifmodptr ;	/* IF module in context */


  /*
   * Validate context, then attempt the read
   */
  if ( ! UrmRCValid(context_id) )
    return Urm__UT_Error ("UrmIFMGetModule", _MrmMMsg_0043,
			  file_id, context_id, MrmBAD_CONTEXT) ;

  result = UrmIdbGetIndexedResource (file_id, index, URMgResourceSet,
				     URMrsInterfaceModule, context_id) ;
  if ( result != MrmSUCCESS ) return result ;

  /*
   * validate the interface module
   */
  ifmodptr = (RGMModuleDescPtr) UrmRCBuffer(context_id) ;
  if ( ifmodptr->validation != URMInterfaceModuleValid )
    return Urm__UT_Error ("UrmIFMPutModule", _MrmMMsg_0025,
			  NULL, context_id, MrmBAD_IF_MODULE) ;

  /*
   * Successfully retrieved
   */
  return MrmSUCCESS ;

}

