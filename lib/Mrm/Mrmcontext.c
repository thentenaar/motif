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
static char rcsid[] = "$XConsortium: Mrmcontext.c /main/12 1996/11/13 14:00:18 drk $"
#endif
#endif


/*
 *++
 *  FACILITY:
 *
 *      UIL Resource Manager (URM):
 *
 *	URM context routines
 *
 *  ABSTRACT:
 *
 *	These routines create, modify, and delete a URM context structure.
 *	Note that accessors for a URM context are macros.
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
 *	UrmGetResourceContext		- Allocate a new resource context
 *
 *	UrmResizeResourceContext	- Resize a resource context
 *
 *	UrmFreeResourceContext	- Free a resource context
 *
 */



/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	UrmGetResourceContext allocates a new resource context, then
 *	allocates a memory buffer of the requested size and associates it
 *	with the context.
 *
 *  FORMAL PARAMETERS:
 *
 *	alloc_func		Function to use in allocating memory for this
 *				context. A null pointer means use default
 *				(XtMalloc).
 *	free_func		Function to use in freeing memory for this
 * 				context. A null pointer means use default
 *				(XtFree).
 *	size			Size of memory buffer to allocate.
 *	context_id_return	To return new context.
 *
 *  IMPLICIT INPUTS:
 *
 *  IMPLICIT OUTPUTS:
 *
 *  FUNCTION VALUE:
 *
 *	MrmSUCCESS	operation succeeded
 *	MrmFAILURE	allocation failure
 *
 *  SIDE EFFECTS:
 *
 *--
 */

Cardinal UrmGetResourceContext(char *(*alloc_func)(Cardinal),
                               void (*free_func)(void *),
                               MrmSize size,
                               URMResourceContextPtr *context_id_return)
{
  /*
   * Set function defaults if NULL
   */
  if (!alloc_func) alloc_func = XtMalloc;
  if (!free_func)  free_func  = (void (*)(void *))XtFree;

  /*
   * Allocate the context buffer and memory buffer, and set the
   * context up.
   */
  *context_id_return =
    (URMResourceContextPtr) (*alloc_func) (sizeof(URMResourceContext)) ;
  if ( *context_id_return == NULL )
    return Urm__UT_Error ("UrmGetResourceContext", _MrmMMsg_0027,
			  NULL, *context_id_return, MrmFAILURE) ;

  (*context_id_return)->validation = URMResourceContextValid ;
  (*context_id_return)->data_buffer = NULL ;

  if ( size > 0 )
    {
      (*context_id_return)->data_buffer = (char *) (*alloc_func) (size) ;
      if ( (*context_id_return)->data_buffer == NULL )
        {
	  (*free_func) (*context_id_return) ;
	  return Urm__UT_Error ("UrmGetResourceContext", _MrmMMsg_0001,
				NULL, *context_id_return, MrmFAILURE) ;
        }
    }

  (*context_id_return)->buffer_size = size ;
  (*context_id_return)->resource_size = 0 ;
  (*context_id_return)->group = URMgNul ;
  (*context_id_return)->type = URMtNul ;
  (*context_id_return)->access = 0 ;
  (*context_id_return)->lock = 0 ;
  (*context_id_return)->alloc_func = alloc_func ;
  (*context_id_return)->free_func = free_func ;

  /*
   * Context successfully created
   */
  return MrmSUCCESS ;

}



/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	UrmResizeResourceContext reallocates the memory buffer in a
 *	resource context to increase its size. The contents of the current
 *	memory buffer are copied into the new memory buffer. If the size
 *	parameter is smaller than the current memory buffer size, the request
 *	is ignored. If the size parameter is greater than MrmMaxResourceSize, a
 *	MrmTooMany error is returned
 *
 *  FORMAL PARAMETERS:
 *
 *	context_id	Resource context to receive resized memory buffer
 *	size		New size for memory buffer in resource context
 *
 *  IMPLICIT INPUTS:
 *
 *  IMPLICIT OUTPUTS:
 *
 *  FUNCTION VALUE:
 *
 *	MrmSUCCESS	operation succeeded
 *	MrmBAD_CONTEXT	Illegal resource context
 *	MrmFAILURE	memory allocation failure
 *	MrmTOO_MANY	size was larger than MrmMaxResourceSize
 *
 *  SIDE EFFECTS:
 *
 *--
 */

Cardinal
UrmResizeResourceContext (URMResourceContextPtr	context_id,
			  int			size)

{

  /*
   *  Local variables
   */
  char			*newbuf ;	/* new buffer */


  if ( ! UrmRCValid(context_id) )
    return Urm__UT_Error ("UrmResizeResourceContext", _MrmMMsg_0028,
			  NULL, context_id, MrmBAD_CONTEXT) ;

  if ( size > MrmMaxResourceSize)
    return Urm__UT_Error ("MrmResizeResourceContext", _MrmMMsg_0029,
			  NULL, context_id, MrmTOO_MANY) ;

  /*
   * CR 8391: buffer will eventually be passed to
   * Urm__CW_EvaluateValOrOffset which is expecting a long.  Make sure to
   * allocate at least a long so that we don't get array bounds read
   * violations.  Ideally Urm__CW_EvaluateValOrOffset and the code that
   * calls it should be rewritten, but we don't have time for that now.
   */
  if (size < sizeof(long)) size = sizeof(long);

  /*
   * Resize unless buffer is bigger than requested size.
   */
  if ( context_id->buffer_size > size ) return MrmSUCCESS ;

  /*
   * Allocate the new buffer, copy the old buffer contents, and
   * update the context.
   */
  if ( context_id->alloc_func == XtMalloc )
    {
      context_id->data_buffer = XtRealloc (context_id->data_buffer, size) ;
      context_id->buffer_size = size ;
    }
  else
    {
      newbuf = (char *) (*(context_id->alloc_func)) (size) ;
      if ( newbuf == NULL )
        return Urm__UT_Error ("UrmResizeResourceContext", _MrmMMsg_0001,
			      NULL, context_id, MrmFAILURE) ;
      if ( context_id->data_buffer != NULL )
        {
	  UrmBCopy (context_id->data_buffer, newbuf, context_id->buffer_size) ;
	  (*(context_id->free_func)) (context_id->data_buffer) ;
        }
      context_id->data_buffer = newbuf ;
      context_id->buffer_size = size ;
    }

  /*
   * Resize succeeded
   */
  return MrmSUCCESS ;

}



/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	UrmFreeResourceContext frees the memory buffer and the
 *	resource context.
 *
 *  FORMAL PARAMETERS:
 *
 *	context_id	Resource context to be freed
 *
 *  IMPLICIT INPUTS:
 *
 *  IMPLICIT OUTPUTS:
 *
 *  FUNCTION VALUE:
 *
 *	MrmSUCCESS	operation succeeded
 *	MrmBAD_CONTEXT	Illegal resource context
 *
 *  SIDE EFFECTS:
 *
 *--
 */

Cardinal
UrmFreeResourceContext (URMResourceContextPtr	context_id)
{

  if ( ! UrmRCValid(context_id) )
    return Urm__UT_Error ("UrmFreeResourceContext", _MrmMMsg_0028,
			  NULL, context_id, MrmBAD_CONTEXT) ;

  context_id->validation = 0 ;
  if ( context_id->data_buffer != NULL )
    (*(context_id->free_func)) (context_id->data_buffer) ;
  (*(context_id->free_func)) (context_id) ;
  return MrmSUCCESS ;

}
