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
/*   $TOG: CallUil.h /main/5 1999/11/23 16:51:12 jff $ */
#include <stdio.h>
#include <signal.h>
#include <Mrm/MrmAppl.h>    
#include <Xm/Xm.h>
#include <uil/UilDef.h>

#define MAX_FILES       10
#define MAX_INFO        32000
#define MAX_CALLBACK    32
#define MAX_LEN         255

/* routines called from Uil file via callbacks */

static void exit_test();
static void Change_Font();
static void Change_Position();
static void Place_Hunt();
static void Hunt_String();
static void Place_FsBox();
static void Get_File();
static void Continue();
void dummy_handler();

/* internally called routines */

static void FlushEvents();
static void Pause();
static void Get_Text();
static char * cs_to_rs();
static void Create_Uid_File();

/* Callback routines for Callable UIL */

Uil_status_type 	MessageCB();
Uil_status_type		StatusCB();

char value_string[MAX_INFO];

char mess_string[MAX_INFO] = "This Test will test the Callable UIL compiler.\nIt will compile three UIL files:\n\n   1) The first will have an error (missing ; on end module)\n   2) The second will have a warning (fontList not supported)\n \
  3) The third will compile correctly\n\nThe following options will be set when calling the Callable Uil compiler:\n\n   1) Listing files will be created\n   2) UID file will be created\n   3) error and warning messages will be reported\n   4) machine code will be produced in listing file\n   5) status_update_delay is set at 300\n   6) Three include directories will be searched\n\nWhen xlist.uil file has sucessfully compiled, It will be used\nin a list utility created in UIL\n\nContinue for UIL compilation error test\n";

static MrmHierarchy	s_MrmHierarchy;
static char		*vec[MAX_FILES];   	
static char		rootname_err[MAX_LEN] = "xlistError";
static char		rootname_warn[MAX_LEN] = "xlistWarn";
static char		rootname_corr[MAX_LEN] = "xlist";
static char 		uid_file[MAX_LEN];

Widget toplevel = NULL;                    
Widget  widgetmain = NULL;
int cur_pos = 0;
Boolean wait;
