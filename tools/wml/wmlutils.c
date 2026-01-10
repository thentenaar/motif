/**
 * Motif
 *
 * Copyright (c) 2025 Tim Hentenaar
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
#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$XConsortium: wmlutils.c /main/8 1995/08/29 11:11:24 drk $"
#endif
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

/*
 * This file contains utilities used by WML.
 */

#include "wml.h"

/*
 * Utility to allocate dynamic space for a string, and return the
 * dynamic copy. Produces a NULL on null input.
 */
char *wmlAllocateString(const char *s)
{
	char *d = NULL;

	if (s && (d = malloc(strlen(s) + 1)))
		strcpy(d, s);
	return d;
}

/**
 * Utility to convert a string to upper case. The conversion happens in
 * place, destroying the original string.
 */
void wmlUpperCaseString(char *s)
{
	int i;

	for (i = 0; s && s[i]; i++)
		s[i] = _upper(s[i]);
}

/*
 * Routines for accessing and manipulating dynamic handle lists.
 */

/**
 * Initialize a dynamic handle list. Allocate a vector of the given
 * size, and set the count and number used (0).
 *
 *	listptr		the list to be inited
 *	size		# entries in handle vector
 *	is_ordered	TRUE if list is to be ordered
 *	dup         TRUE if list can contain duplicates
 *
 * Returns non-zero on success, 0 on failure.
 */
int wmlInitHList(DynamicHandleListDefPtr listptr, int size, int is_ordered, int dup)
{
	listptr->dup     = !!dup;
	listptr->cnt     = 0;
	listptr->max     = size;
	listptr->ordered = !!is_ordered;
	listptr->hvec    = malloc(size * sizeof(ObjectHandleDef));
	return !!listptr->hvec;
}

/**
 * Routine to resize a dynamic handle list. Increases the size if required,
 * but does nothing if the list is already big enough.
 *
 *	listptr		the dynamic list
 *	new_size	new list size
 */
void wmlResizeHList(DynamicHandleListDefPtr listptr, int new_size)
{
	ObjectHandleDefPtr new_vec;	/* reallocated vector */

	if (!listptr || listptr->max >= new_size) return;
	if ((new_vec = realloc(listptr->hvec, new_size * sizeof(ObjectHandleDef)))) {
		listptr->max  = new_size;
		listptr->hvec = new_vec;
	}
}

/**
 * Routine to clear a dynamic handle list. It leaves the handle vector intact,
 * but frees all the allocated names. The count is reset to 0.
 * but does nothing if the list is already big enough.
 *
 *	listptr		the dynamic list
 */
void wmlClearHList(DynamicHandleListDefPtr listptr)
{
	int i;

	for (i=0; i < listptr->cnt; i++)
		free(listptr->hvec[i].objname);

	memset(listptr->hvec, 0, listptr->cnt * sizeof *listptr->hvec);
	listptr->cnt = 0;
}

/**
 * Function to find a name in a dynamic list. This will function on both
 * ordered and unordered lists.
 *
 *	listptr		the dynamic list
 *	name		the name to look up in the list
 *
 * returns:
 *	>= 0		name found, index in list
 *	< 0		name not found
 */
int wmlFindInHList(DynamicHandleListDefPtr listptr, const char *name)
{
	int i, lo, hi, mid;

	if (!listptr || !listptr->cnt)
		return -1;

	/* Binary search if ordered, brute force otherwise */
	if (!listptr->ordered) {
		for (i = 0; i < listptr->cnt; i++) {
			if (!strcmp(name, listptr->hvec[i].objname))
				return i;
		}

		return -1;
	}

	for (lo=0, hi=listptr->cnt - 1; hi >= lo; ) {
		mid = (unsigned int)(lo + hi) >> 1;
		if (!(i = strcmp(name, listptr->hvec[mid].objname)))
			return mid;
		if (i < 0) hi = mid - 1;
		if (i > 0) lo = mid + 1;
	}

	return -1;
}

/**
 * Routine to insert an entry into a list. The insertion is ordered or
 * unordered depending on the way the list is marked. Unordered lists
 * insert at the end. This routine assumes no duplicates will be entered
 * in the list if listptr->dup is zero.
 *
 *	listptr		the list
 *	name		the name under which to insert
 *	obj		the object to insert
 *
 *	Returns 1 on success, 0 on failure.
 */
int wmlInsertInHList(DynamicHandleListDefPtr listptr, const char *name, ObjectPtr obj)
{
	int i = 0, hi, lo, mid = 0;

	/* Guarantee enough capacity in the list */
	wmlResizeHList(listptr, (hi = (listptr ? listptr->cnt + 1 : 1)));
	if (!listptr || listptr->max < hi) {
		printf("\nwmlInsertInHList: failed to resize list for '%s'\n", name);
		return 0;
	}

	/* Binary search and insert if ordered, append otherwise */
	if (!listptr->ordered || !listptr->cnt) {
		listptr->hvec[listptr->cnt].objname  = wmlAllocateString(name);
		listptr->hvec[listptr->cnt++].objptr = obj;
		return 1;
	}

	for (lo=0, hi=listptr->cnt - 1; hi >= lo;) {
		mid = (unsigned int)(lo + hi) >> 1;
		if (!(i = strcmp(name, listptr->hvec[mid].objname))) {
			if (!listptr->dup) {
				printf("\nwmlInsertInHList: duplicate name '%s' found\n", name);
				return 0;
			}

			break;
		}

		if (i < 0) hi = mid - 1;
		if (i > 0) lo = mid + 1;
	}

    /**
     * The new entry will go either at mid or after mid. Move down
     * the vector appropriately.
     */
    mid += i >= 0;
	memmove(listptr->hvec + mid + 1, listptr->hvec + mid,
	        (listptr->cnt - mid) * sizeof(ObjectHandleDef));
	listptr->hvec[mid].objname = wmlAllocateString(name);
	listptr->hvec[mid].objptr  = obj;
	listptr->cnt++;
	return 1;
}

/**
 * Routine to insert an entry into a token list. The insertion is ordered.
 * This routine allows duplicates
 *
 *	listptr		the list
 *	name		the name under which to insert
 *	obj		the object to insert
 *
 *	Returns 1 on success, 0 on failure.
 */
int wmlInsertInKeyList(DynamicHandleListDefPtr listptr, const char *name, ObjectPtr obj)
{
	if (!listptr) {
		printf("\nwmlInsertInKeyList: NULL listptr given for '%s'\n", name);
		return 0;
	}

	if (!listptr->dup) {
		printf("\nWARNING: wmlInsertInKeyList: '%s' doesn't allow duplicates... enabling duplicates\n", name);
		listptr->dup |= 1;
	}

	return wmlInsertInHList(listptr, name, obj);
}

/**
 * Indicate if a resource is in a resource reference list by returning its
 * reference pointer.
 */
WmlClassResDefPtr wmlResolveResIsMember(WmlResourceDefPtr resobj, WmlClassResDefPtr resref)
{
	while (resref) {
		if (resref->act_resource == resobj)
			return resref;
		resref = resref->next;
	}

	return NULL;
}

/**
 * Indicate if a child is in a child reference list by returning its
 * reference pointer.
 */
WmlClassChildDefPtr wmlResolveChildIsMember(WmlChildDefPtr childobj, WmlClassChildDefPtr childref)
{
	while (childref) {
		if (childref->act_child == childobj)
			return childref;
		childref = childref->next;
	}

	return NULL;
}

