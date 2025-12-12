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
#ifndef _XmAtomMgr_h
#define _XmAtomMgr_h

#include <Xm/Xm.h>
#include <X11/Xresource.h>

#ifdef __cplusplus
extern "C" {
#endif

/* X11r5' XInternAtom equivalent */
extern Atom XmInternAtom(
        		Display *display,
        		String name,
        		Boolean only_if_exists );

/* X11r5's XGetAtomName equivalent */
extern String XmGetAtomName( Display *display, Atom atom);

#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

/* This macro name is confusing, and of unknown benefit.
 * #define XmNameToAtom(display, atom) \
 *      XmGetAtomName(display, atom)
 */

#endif /* _XmAtomMgr_h */
