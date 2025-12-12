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
/*   $XConsortium: tkdef.h /main/5 1995/07/14 09:45:45 drk $ */


/************************************************************
 *     tkdef.h -- typedefs and defines for toolkit-specific dialogue layer
 ************************************************************/

#ifndef _tkdef_h
#define _tkdef_h

enum warn_reasons { warn_open, warn_write, warn_save, warn_remove };

enum attributes { TkFONT, TkCOLOR, TkTHRU, TkUNDER }; 

#define TkBEGIN 0
#define TkEND 1

#define TkTEXT_ONLY 1
#define TkBYTE_STREAM 2
#define TkHELP_MARKUP 3

#endif /* _tkdef_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
