/* $XConsortium: sgi_visual.h /main/5 1995/07/15 21:01:43 drk $ */
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
/*
 * HISTORY
 */


#include <X11/Intrinsic.h>
/* #include <X11/Shell.h>
/* #include <X11/StringDefs.h>
/* #include <stdio.h>
/** **/

/* STATUS RETURNS */
#	define SG_VISUAL_SUCCESS	1
#	define SG_VISUAL_DEFAULT	(SG_VISUAL_SUCCESS + 1)

#	define SG_NO_VISUAL		(-1)
#	define SG_BAD_DISPLAY		(SG_NO_VISUAL-1)
#	define SG_NO_TYPE_AND_CLASS	(SG_BAD_DISPLAY-1)
#	define SG_NO_SUCH_VISUAL	(SG_NO_TYPE_AND_CLASS-1)


/* Visual types */
#	define SG_DEFAULT_PLANES	0	/* matches resource default */
#	define SG_UNDERLAY_PLANES	1
#	define SG_NORMAL_PLANES		2
#	define SG_OVERLAY_PLANES	3
#	define SG_POPUP_PLANES		4
#	define SG_MAX_TYPES		(SG_POPUP_PLANES + 1)


/* External declarations */

    extern int	    _SG_defaultDepthAndTypeResources
	( Display *display, int screen, int *requestedClass,
	  char *requestedType, int *requestedTypeV, int *requestedDepth,
	  Visual **requestedVisual, Colormap *requestedColormap,
	  Drawable *requestedDrawable);
    extern Colormap _SG_getDefaultColormap
	(Display *dpy, int scr, Visual *vsl);
    extern int      _SG_getDefaultDepth
	(Display *dpy, int scr, int *class, int type);
    extern XVisualInfo *_SG_getMatchingVisual
	(Display *dpy, int scr, VisualID vsl, int *class, int type, int depth);
    extern int      _SG_getMaxDepth
	(Display *dpy, int scr, int *class, int type);
    extern int      _SG_getNormalArgs
	(Display *dpy, int scr, ArgList args, int *n);
    extern int     _SG_getOverlayArgs
	(Display *dpy, int scr, ArgList args, int *n);
    extern int     _SG_getOverlay2Args
	(Display *dpy, int scr, ArgList args, int *n);
    extern int     _SG_getOverlay4Args
	(Display *dpy, int scr, ArgList args, int *n);
    extern int      _SG_getPopupArgs
	(Display *dpy, int scr, ArgList args, int *n);
    extern int      _SG_getUnderlayArgs
	(Display *dpy, int scr, ArgList args, int *n);
    extern int      _SG_getVisualArgs
	(Display *dpy, int scr, int dpth, int *class, int type, ArgList args, int *n);
