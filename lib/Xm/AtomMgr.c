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
#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$TOG: AtomMgr.c /main/13 1997/09/08 14:06:18 cshi $"
#endif
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <Xm/AtomMgr.h>
#include <Xm/XmP.h>
#include <X11/Xresource.h>

/*****************************************************************************
 *
 *  XmInternAtom()
 *
 ****************************************************************************/

Atom
XmInternAtom(
        Display *display,
        String name,
        Boolean only_if_exists )
{
  /* While not yet obsolete, this routine is not in favor.  Use */
  /* XInternAtom directly. */
  return XInternAtom(display, name, only_if_exists);
}

/*****************************************************************************
 *
 *  XmGetAtomName()
 *
 ****************************************************************************/

String
XmGetAtomName(
        Display *display,
        Atom atom )
{
  /* While not yet obsolete, this routine is not in favor.  Use */
  /* XGetAtomName directly. */
  return XGetAtomName(display, atom);
}
