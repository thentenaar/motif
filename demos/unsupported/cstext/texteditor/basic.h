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
 * HISTORY
*/ 
/*   $XConsortium: basic.h /main/5 1995/07/14 09:43:44 drk $ */

/************************************************************
 *     basic.h -- basic functions and macros
 ************************************************************/

#define false 0
#define true 1

#include <X11/Intrinsic.h>

#define BasicMalloc(siz) XtMalloc(siz)
#define BasicRealloc(buf,siz) XtRealloc(buf,siz)
#define BasicFree(ptr) XtFree(ptr)

#define strdup(strto,strfrom) ( (strto) = XtMalloc(strlen(strfrom)+1), strcpy(strto,strfrom) )
