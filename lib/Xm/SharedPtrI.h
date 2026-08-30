/**
 * Motif
 *
 * Copyright (c) 2026 Tim Hentenaar
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
#ifndef XM_SHAREDPTRI_H
#define XM_SHAREDPTRI_H

#include <X11/Intrinsic.h>

typedef struct _XmSharedPtr *XmSharedPtr;
typedef struct _XmWeakPtr   *XmWeakPtr;

/**
 * Construct a shared pointer for the concrete pointer \a ptr
 *
 * The destroy callback will be called when the ref count drops to zero.
 * The dup callback is used to create a deep copy of the underlying object.
 */
XmSharedPtr XmSharedPtrCreate(void *ptr, void (*destroy)(void *), void *(*dup)(void *));

/**
 * "Copy" (or duplicate) a shared pointer
 */
void *XmSharedPtrCopy(void *ptr, Boolean duplicate);

/**
 * Retrieve the underlying pointer value
 */
void *XmSharedPtrGet(void *ptr);

/**
 * Release a shared pointer
 */
void XmSharedPtrFree(void *ptr);

/**
 * Allocate a weak pointer. This is a handle to the shared pointer, not
 * the underlying object.
 */
XmWeakPtr XmWeakPtrCreate(void *ptr);

/**
 * True if the object behind this weak pointer yet lives
 */
Boolean XmWeakPtrLives(XmWeakPtr ptr);

/**
 * Strenthen our pointer reference so the underlying object doesn't get
 * rug-pulled while we use it.
 *
 * This returns NULL if we couldn't strengthen our pointer, or if the
 * shared pointer's refcount would overflow.
 */
void *XmWeakPtrLock(XmWeakPtr ptr);

/**
 * Release a weak pointer
 */
void XmWeakPtrFree(XmWeakPtr ptr);

#endif /* XM_SHAREDPTRI_H */
