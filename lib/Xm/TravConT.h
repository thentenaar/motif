/* $XConsortium: TravConT.h /main/5 1995/07/15 20:56:44 drk $ */
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
#ifndef _XmTravConT_h
#define _XmTravConT_h

#include <Xm/Xm.h>

#ifdef __cplusplus
extern "C" {
#endif

externalref XrmQuark XmQTtraversalControl;


typedef Widget (*XmTraversalRedirectionProc)(Widget               old_focus,
					     Widget               new_focus,
					     unsigned int         focus_policy,
					     XmTraversalDirection direction,
					     unsigned int         pass);


/* Version 0: initial release. */

typedef struct _XmTraversalControlTraitRec {
  int				version;	/* 0 */
  XmTraversalRedirectionProc	redirect;
} XmTraversalControlTraitRec, *XmTraversalControlTrait;


#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* XmTravConT_h */
