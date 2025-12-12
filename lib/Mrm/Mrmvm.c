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
static char rcsid[] = "$XConsortium: Mrmvm.c /main/10 1996/11/13 14:05:05 drk $"
#endif
#endif

/*
 *++
 *  FACILITY:
 *
 *      UIL Resource Manager (URM)
 *
 *  ABSTRACT:
 *
 *      This module handles memory management for URM.
 *
 *  AUTHORS:
 *
 *  MODIFICATION HISTORY:
 *--
 */


/*
 *
 *  INCLUDE FILES
 *
 */

#include <Mrm/MrmAppl.h>
#include <Mrm/Mrm.h>

/*
 *
 *  TABLE OF CONTENTS
 *
 *	Urm__UT_AllocString	- allocates and copies a nul-terminated string
 *
 */



/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	This utility allocates memory for a string from the miscellaneous
 *	zone and copies the string into it.
 *
 *  FORMAL PARAMETERS:
 *
 *	stg		null-terminated string to copy
 *
 *  IMPLICIT INPUTS:
 *
 *  IMPLICIT OUTPUTS:
 *
 *  FUNCTION VALUE:
 *
 *	pointer to the new string; NULL if an allocation error
 *
 *  SIDE EFFECTS:
 *
 *--
 */

String 
Urm__UT_AllocString (String		stg)
{
  /*
   *  Local variables
   */
  String			new_stg ;	/* the string copy */


  if ( stg == NULL ) return NULL ;
  new_stg = (String) XtMalloc (strlen(stg)+1) ;
  if ( new_stg != NULL )
    strcpy (new_stg, stg) ;
  return new_stg ;

}



