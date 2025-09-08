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
static char rcsid[] = "$XConsortium: client_win.c /main/5 1995/07/14 09:47:48 drk $"
#endif
#endif

#include <X11/Xlib.h>
#include <X11/Xatom.h>

/* Find a window with WM_STATE, else return win itself, as per ICCCM */
static Atom WM_STATE = None;

static Window TryChildren(Display *dpy, Window win, unsigned int **retdata)
{
	Window root, parent;
	Window *children;
	unsigned int nchildren;
	unsigned int i;
	Atom type = None;
	int format;
	unsigned long nitems, after;
	unsigned int *data;
	Window inf = 0;

	if (!XQueryTree(dpy, win, &root, &parent, &children, &nchildren))
		return 0;
	for (i = 0; !inf && (i < nchildren); i++) {
		XGetWindowProperty(dpy, children[i], WM_STATE, 0, 2, False,
			AnyPropertyType, &type, &format, &nitems,
			&after, (unsigned char **)&data);
		if (type) {
			inf = children[i];
			*retdata = data;
		}
	}
	for (i = 0; !inf && (i < nchildren); i++)
		inf = TryChildren(dpy, children[i], retdata);
	if (children) XFree((char *)children);
	return inf;
}

Window IsClientWindow(Display *dpy, Window win, unsigned int **retdata)
{
	Atom type = None;
	int format;
	unsigned long nitems, after;
	unsigned int	*data;
	Window inf;

	*retdata = NULL;
	if (WM_STATE == None)
		WM_STATE = XInternAtom(dpy, "WM_STATE", True);
	if (!WM_STATE) return win;

	XGetWindowProperty(dpy, win, WM_STATE, 0, 2, False, AnyPropertyType,
		       &type, &format, &nitems, &after, (unsigned char **)&data);
	if (type) {
		*retdata = data;
		return win;
	}
	inf = TryChildren(dpy, win, retdata);
	return inf ? inf : win;
}

