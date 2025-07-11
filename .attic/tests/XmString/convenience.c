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
static char rcsid[] = "$XConsortium: convenience.c /main/7 1995/07/13 19:57:17 drk $"
#endif
#endif

#include <stdio.h>

/*
 * Error handling
 */

void error_proc(caller, error_string)
char	*caller,
 		*error_string;
{

    fprintf(stderr, "%s\n", error_string);
    fprintf(stderr, "\nTest \"%s\" failed to run to completion.\n", caller);
    fflush(stderr);
    exit(1);

}


/*
 * Summary handling
 */

void summary_proc(caller, errors)
char	*caller;
int		errors;
{

    if(errors)
		fprintf(stdout, "\nTest \"%s\" failed with %d error(s).\n", caller,
				errors);
    else
		fprintf(stdout, "\nTest \"%s\" passed.\n", caller, errors);

    fflush(stdout);

}
