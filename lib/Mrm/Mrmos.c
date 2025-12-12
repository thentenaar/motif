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
 *
 */
/*
 * HISTORY
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$XConsortium: Mrmos.c /main/7 1996/11/13 14:03:33 drk $"
#endif
#endif

#include <X11/Intrinsic.h>

#ifdef __cplusplus
extern "C" { /* some 'locale.h' do not have prototypes (sun) */
#endif
#include <X11/Xlocale.h>
#ifdef __cplusplus
} /* Close scope of 'extern "C"' declaration */
#endif /* __cplusplus */

#include "MrmosI.h"

/************************************************************
 *
 * The following routines are used for portable uid files to convert
 * between the host's native format for floats and IEEE format floats.
 * Type SINGLE_FLOAT in a uid file should be stored as a 32 bit IEEE float
 * Type FLOAT in a uid file should be stored as a 64 bit IEEE double
 * The routines need support for the VAX architecture, maybe others.
 *
 ************************************************************/

/*ARGSUSED*/
void _MrmOSHostFloatToIEEE(float *val) /* unused */
{
  /*EMPTY*/
}

/*ARGSUSED*/
void _MrmOSHostDoubleToIEEE(double *val) /* unused */
{
  /*EMPTY*/
}

/*ARGSUSED*/
void _MrmOSIEEEFloatToHost(float *val) /* unused */
{
  /*EMPTY*/
}


/*ARGSUSED*/
void _MrmOSIEEEDoubleToHost(double *val)	/* unused */
{
  /*EMPTY*/
}

/************************************************************************
 *
 *    _MrmOSSetLocale   wrapper so vendor can disable call to set
 *                    if locale is superset of "C".
 *
 ************************************************************************/

String
_MrmOSSetLocale(String locale)
{
  return setlocale(LC_ALL, locale);
}
