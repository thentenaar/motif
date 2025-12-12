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

/*
**++
**  FACILITY:
**
**      User Interface Language Compiler (UIL)
**
**  ABSTRACT:
**
**      This include file defines the interface to the operating system
**	io facilities.  
**
**--
**/

#ifndef UilIODef_h
#define UilIODef_h


/*
**  Define a UIL File Control Block or FCB.
*/


#include <stdio.h>
#ifndef NULL
#define NULL 0L
#endif

typedef struct  
{
    FILE	*az_file_ptr;
    char	*c_buffer;
    boolean	v_position_before_get;
    z_key	last_key;
    char	expanded_name[ 256 ];
} uil_fcb_type;

#endif /* UilIODef_h */
/* DON'T ADD STUFF AFTER THIS #endif */
