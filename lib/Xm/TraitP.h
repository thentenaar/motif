/* $XConsortium: TraitP.h /main/5 1995/07/15 20:56:18 drk $ */
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
/*
 * HISTORY
 */

#ifndef _XmTraitP_H
#define _XmTraitP_H 1

#include <Xm/XmP.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Macros */

#define XmeTraitRemove(w, t) XmeTraitSet((XtPointer) w, t, NULL)


/********    Private Function Declarations    ********/

/*
 * XmeTraitGet(object, trait) returns a pointer to the trait_record
 * from looking up the trait on this object.  If the trait
 * is not found then NULL is returned.  This can therefore be used
 * in the following cliche'
 *
 * if (trait_rec = XmeTraitGet(XtClass(w), XmQTactivate)) {
 *   trait_rec -> activate();
 *   trait_rec -> disarm();
 * }
 */

extern XtPointer XmeTraitGet(XtPointer, XrmQuark);

/* 
 * Boolean XmeTraitSet(object, traitname, traitrecord)
 *
 * Installs the trait on the object.  Boolean will indicate
 * success of the installation.  
 * 
 * Install will use the direct pointer to traitrecord given.  The
 * implementation is therefore not allowed to use automatic
 * storage for traitrecord,  but can use malloc or static initialization
 *
 */

extern Boolean XmeTraitSet(XtPointer, XrmQuark, XtPointer);

/********    End Private Function Declarations    ********/

#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmTraitP_H */

