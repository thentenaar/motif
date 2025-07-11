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
static char rcsid[] = "$XConsortium: ExtraArgs.c /main/7 1995/07/14 11:58:56 drk $"
#endif
#endif
#include <testlib.h>

Arg 		CommonExtraArgs[MAX_ARGS];	/* Array of Extra Arguments */
Cardinal 	CommonExtraN = 0;		/* Number of Extra Arguments */

void
CommonExtraResources(Arg args[], Cardinal n)
{
    int i;


    for (i = 0 ; i < n ; i++)
	CommonExtraArgs[i] = args[i];

    CommonExtraN = n;

}
