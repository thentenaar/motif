/* $XConsortium: GrabShellP.h /main/5 1995/07/15 20:51:26 drk $ */
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
#ifndef _XmGrabShellP_h
#define _XmGrabShellP_h

#include <Xm/GrabShell.h>
#include <Xm/XmP.h>
#include <X11/ShellP.h>

#ifdef __cplusplus
extern "C" {
#endif

/* The GrabShell instance record */

typedef struct 
{
  Cursor	cursor;
  Dimension	shadow_thickness;
  Pixel		top_shadow_color;
  Pixmap  	top_shadow_pixmap;
  Pixel   	bottom_shadow_color;
  Pixmap  	bottom_shadow_pixmap;
  GC      	top_shadow_GC;
  GC      	bottom_shadow_GC;
  Boolean	owner_events;
  int		grab_style;
  /* Internal fields */
  Time		post_time;
  Time		unpost_time;
  Boolean	mapped;
  Window	old_focus;
  int		old_revert_to;
} XmGrabShellPart;


/* Full instance record declaration */

typedef  struct _XmGrabShellRec 
{
  CorePart		core;
  CompositePart		composite;
  ShellPart		shell;
  WMShellPart		wm_shell;
  VendorShellPart	vendor_shell;
  XmGrabShellPart	grab_shell;
} XmGrabShellRec;

typedef  struct _XmGrabShellWidgetRec /* OBSOLETE (for compatibility only).*/
{
  CorePart		core;
  CompositePart		composite;
  ShellPart		shell;
  WMShellPart		wm_shell;
  VendorShellPart	vendor_shell;
  XmGrabShellPart	grab_shell;
} XmGrabShellWidgetRec;



/* GrabShell class structure */

typedef struct 
{
  XtPointer		extension;	 /* Pointer to extension record */
} XmGrabShellClassPart;


/* Full class record declaration */

typedef struct _XmGrabShellClassRec 
{
  CoreClassPart	    	core_class;
  CompositeClassPart	composite_class;
  ShellClassPart	shell_class;
  WMShellClassPart	wm_shell_class;
  VendorShellClassPart	vendor_shell_class;
  XmGrabShellClassPart  grab_shell_class;
} XmGrabShellClassRec;


externalref XmGrabShellClassRec  xmGrabShellClassRec;

/********    Private Function Declarations    ********/

/********    End Private Function Declarations    ********/


#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmGrabShellP_h */
/* DON'T ADD STUFF AFTER THIS #endif */
