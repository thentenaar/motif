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
#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$XConsortium: AutoError.c /main/10 1995/07/14 11:27:33 drk $"
#endif
#endif
#include "mvslib.h"
#include <X11/Intrinsic.h>
#include "Automation.h"
#include <stdio.h>

void
AutoError(
        char *error_string )
{

/*
	print error messages to stdout and stderr. stdout to get
	message into the .prt file
*/

    fprintf (stdout,"     %s\n",error_string);
    fprintf(stderr, "AUTOERROR** : Exiting Test ...\n");
    exit(1);

}


void
AutoMessage(
        char *message_string )
{

  fprintf (stdout,"     %s\n",message_string);

}


/* this is here temporarily, until all files calling AutoWarning are changed*/

void
AutoWarning(
        char *message_string )
{

  fprintf (stdout,"     %s\n",message_string);

}


void
AutoTraceMsg(
        char *trace_string )
{

    fprintf(stdout, "AutoTrace:	%s\n", trace_string);

}
