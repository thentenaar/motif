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

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <limits.h>
#include <X11/Intrinsic.h>

#include "SharedPtrI.h"

/**
 * Prefer standard atomics, and employ a thread fence even though we won't
 * presently be in danger of having multiple threads.
 */
#if !defined(__STDC_NO_ATOMICS__) && defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
#include <stdatomic.h>
#define INIT(x) atomic_init(&x, 1)
#define LOAD(x) atomic_load_explicit(&x, memory_order_relaxed)
#define INC(x)  atomic_fetch_add_explicit(&x, 1, memory_order_acquire)
#define DEC(x)  atomic_fetch_sub_explicit(&x, 1, memory_order_release)
#define CAS(x, y, z) atomic_compare_exchange_weak_explicit(&x, y, z,\
                     memory_order_acquire, memory_order_relaxed)
#define FENCE() atomic_thread_fence(memory_order_acquire)
#elif (defined(__clang__) && __clang_major__ >= 3) || \
      (defined(__GNUC__) && ((__GNUC__ << 8) + __GNUC_MINOR__) >= 0x407)
/* Try GCC/clang builtins */
#define INIT(x) __atomic_store_n(&x, 1, __ATOMIC_RELAXED)
#define LOAD(x) __atomic_load_n(&x, __ATOMIC_RELAXED)
#define INC(x)  __atomic_fetch_add(&x, 1, __ATOMIC_ACQUIRE)
#define DEC(x)  __atomic_fetch_sub(&x, 1, __ATOMIC_RELEASE)
#define CAS(x, y, z) __atomic_compare_exchange_n(&x, y, z, 1, \
                     __ATOMIC_ACQUIRE, __ATOMIC_RELAXED)
#define FENCE() __atomic_thread_fence(__ATOMIC_ACQUIRE)
#define _Atomic
#else /* No atomics :( */
#define _Atomic
#define INIT(x) do { (x) = 1; } while (0);
#define LOAD(x) (x)
#define INC(x)  ((x)++)
#define DEC(x)  ((x)--)
#define CAS(x, y, z) (((x) = (z)) == (z))
#define FENCE() ;
#endif

/**
 * Shared pointer with potentially-atomic refcounting. This can also be
 * casted to a pointer-to-type for the underlying object.
 */
struct _XmSharedPtr {
	void *ptr;
	_Atomic unsigned long ref;     /**< Ref count on ptr         */
	_Atomic unsigned long weakref; /**< Ref count on this struct */
	void (*free)(void *);          /**< Free proc for ptr        */
	void *(*dup)(void *);          /**< Duplicate proc for ptr   */
};

/**
 * Keep shared/weak pointers separate and encourage access through
 * the functions given here vs casting.
 */
struct _XmWeakPtr {
	XmSharedPtr strong;
};

/**
 * Release a weak reference
 *
 * Last man out turns out the lights and closes the door.
 */
static void release_weak(XmSharedPtr ptr)
{
	if (DEC(ptr->weakref) > 1)
		return;

	FENCE();
	XtFree((XtPointer)ptr);
}

/**
 * Construct a shared pointer for the concrete pointer \a ptr
 *
 * The destroy callback will be called when the ref count drops to zero.
 * The dup callback is used to create a deep copy of the underlying object.
 */
XmSharedPtr XmSharedPtrCreate(void *ptr, void (*destroy)(void *), void *(*dup)(void *))
{
	XmSharedPtr p;

	if (!ptr)
		return NULL;

	p = (XmSharedPtr)(void *)XtCalloc(1, sizeof *p);
	p->ptr  = ptr;
	p->free = destroy;
	p->dup  = dup;
	INIT(p->ref);
	INIT(p->weakref);
	return p;
}

/**
 * "Copy" (or duplicate) a shared pointer
 */
void *XmSharedPtrCopy(void *ptr, Boolean duplicate)
{
	XmSharedPtr p = (XmSharedPtr)ptr;

	/* He's dead, Jim... */
	if (!ptr || !LOAD(p->ref))
		return NULL;

	if (duplicate)
		return XmSharedPtrCreate(p->dup(p->ptr), p->free, p->dup);

	/* Return NULL on refcount wraparound */
	if (INC(p->ref) == ULONG_MAX) {
		FENCE();
		DEC(p->ref);
		return NULL;
	}

	return ptr;
}

/**
 * Retrieve the underlying pointer value
 */
void *XmSharedPtrGet(void *ptr)
{
	XmSharedPtr p = (XmSharedPtr)ptr;
	return (ptr && LOAD(p->ref)) ? p->ptr : NULL;
}

/**
 * Release a shared pointer
 */
void XmSharedPtrFree(void *ptr)
{
	XmSharedPtr p = (XmSharedPtr)ptr;

	if (!ptr || DEC(p->ref) > 1)
		return;

	FENCE();
	if (p->free) p->free(p->ptr);
	p->ptr = NULL;

	/* Free the control data */
	release_weak(p);
}

/**
 * Allocate a weak pointer. This is a handle to the shared pointer, not
 * the underlying object.
 */
XmWeakPtr XmWeakPtrCreate(void *ptr)
{
	XmWeakPtr w;
	XmSharedPtr p = (XmSharedPtr)ptr;

	if (!ptr || !LOAD(p->ref))
		return NULL;

	/* Refuse to create more weak pointers on refcount wraparound */
	if (INC(p->weakref) == ULONG_MAX) {
		FENCE();
		DEC(p->weakref);
		return NULL;
	}

	w = (XmWeakPtr)(void *)XtMalloc(sizeof *w);
	w->strong = ptr;
	return w;
}

/**
 * True if the object behind this weak pointer yet lives
 */
Boolean XmWeakPtrLives(XmWeakPtr ptr)
{
	return ptr && ptr->strong && LOAD(ptr->strong->ref);
}

/**
 * Strenthen our pointer reference so the underlying object doesn't get
 * rug-pulled while we use it.
 *
 * This returns NULL if we couldn't strengthen our pointer, or if the
 * shared pointer's refcount would overflow.
 */
void *XmWeakPtrLock(XmWeakPtr ptr)
{
	unsigned long n;

	if (!ptr || !XmWeakPtrLives(ptr))
		return NULL;

	/* We need CAS here since we don't hold a strong ref */
	n = LOAD(ptr->strong->ref);
	do {
		if (!n || n == ULONG_MAX)
			return NULL;

		if (CAS(ptr->strong->ref, &n, n + 1))
			break;
	} while (True);

	return ptr->strong;
}

/**
 * Release a weak pointer
 */
void XmWeakPtrFree(XmWeakPtr ptr)
{
	if (ptr && ptr->strong) {
		release_weak(ptr->strong);
		ptr->strong = NULL;
	}

	XtFree((XtPointer)ptr);
}

