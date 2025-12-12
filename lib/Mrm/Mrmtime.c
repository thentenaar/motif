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
static char rcsid[] = "$XConsortium: Mrmtime.c /main/19 1996/11/21 20:03:40 drk $"
#endif
#endif


/*
 *++
 *  FACILITY:
 *
 *      UIL Resource Manager (URM):
 *
 *  ABSTRACT:
 *
 *	System-dependent routines dealing with time and dates.
 *
 *--
 */


/*
 *
 *  INCLUDE FILES
 *
 */

#define X_INCLUDE_TIME_H
#define XOS_USE_XT_LOCKING

#if HAVE_X11_XOS_R_H
#include <X11/Xos_r.h> /* Must precede Mrm/MrmAppl.h and Mrm/Mrm.h to avoid
			  possible redefinitions of MIN() and MAX(). */
#else
 #include <Xm/Xmos_r.h>
#endif

#include <Mrm/MrmAppl.h>
#include <Mrm/Mrm.h>


/*
 *
 *  TABLE OF CONTENTS
 *
 *	Urm__UT_Time		- Get current time as a string
 *
 */


/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	This routine writes the current date/time string into a buffer
 *
 *  FORMAL PARAMETERS:
 *
 *	time_stg	location into which to copy time string. The
 *			length of this buffer should be at least
 *			URMhsDate1
 *
 *  IMPLICIT INPUTS:
 *
 *  IMPLICIT OUTPUTS:
 *
 *  FUNCTION VALUE:
 *
 *  SIDE EFFECTS:
 *
 *--
 */

void
Urm__UT_Time (char		*time_stg)
{
#if defined(__STDC__)
  time_t	timeval;
#else
  long		timeval;
#endif /* __STDC__ */

  _Xctimeparams ctime_buf;
  char *result;

  time (&timeval);
  if ((result = _XCtime(&timeval, ctime_buf)) != NULL)
    strcpy(time_stg, result);
  else
    *time_stg = 0;
}

