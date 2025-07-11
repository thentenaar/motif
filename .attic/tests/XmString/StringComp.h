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
/*   $XConsortium: StringComp.h /main/7 1995/07/13 19:53:54 drk $ */

#define	NUM_TESTS	11

#define mismatch	0
#define match		1

#define different	0
#define identical	1

extern void	error_proc();

static void	do_comparisons();

int	c_expect[NUM_TESTS] =
{
    match,
    match,
    match,
    match,
    mismatch,
    mismatch,
    mismatch,
    mismatch,
    match,
    match,
    mismatch
};

int	bc_expect[NUM_TESTS] =
{
    identical,
    identical,
    different,
    identical,
    different,
    different,
    different,
    different,
    different,
    identical,
    different
};

char	*result_string1[2] =
{
    "mismatch",
    "match   "
};

char	*result_string2[2] =
{
    "different",
    "identical"
};

int	c_errors,
	bc_errors;
