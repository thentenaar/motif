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
**      This include file defines the set of definitions used by the public
**	access routines Uil and UilDumpSymbolTable.
**
**--
**/

#ifndef UilDef_h
#define UilDef_h

/*
**
**  INCLUDE FILES
**
**/

/*
** Includes needed by other include files.
*/
#include <X11/Intrinsic.h>

/*
**
** Common includes needed by public functions.
**
*/
#include <uil/Uil.h>
#include <uil/UilDBDef.h> 	/* This has to be loaded first. */
#include <uil/UilSymGl.h>
#include <uil/UilSymDef.h>

/*
** Function declarations not defined elsewhere
*/
#define _ARGUMENTS(arglist) arglist

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/* uilmain.c */
extern Uil_status_type Uil _ARGUMENTS((Uil_command_type
*comand_desc,Uil_compile_desc_type *compile_desc,Uil_continue_type
(*message_cb)(), char *message_data, Uil_continue_type (*status_cb)(),
char *status_data));

/* uilsymstor.c */
extern void UilDumpSymbolTable  _ARGUMENTS(( sym_entry_type *node_entry ));

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif /* UilDef_h */
/* DON'T ADD STUFF AFTER THIS #endif */
