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
#ifndef _XmDialogShellP_h
#define _XmDialogShellP_h

#include <Xm/XmP.h>
#include <Xm/DialogS.h>
#include <X11/ShellP.h>

#ifdef __cplusplus
extern "C" {
#endif

/* The DialogShell instance record */

typedef struct 
{
    /* internal fields */
    XtGrabKind 		grab_kind;
    Position		old_x, old_y;
} XmDialogShellPart;


/* Full instance record declaration */

typedef  struct _XmDialogShellRec 
{	
    CorePart		    core;
    CompositePart	    composite;
    ShellPart		    shell;
    WMShellPart		    wm;
    VendorShellPart	    vendor;
    TransientShellPart	    transient;
    XmDialogShellPart	    dialog;
} XmDialogShellRec;

typedef  struct _XmDialogShellWidgetRec /* OBSOLETE (for compatibility only).*/
{	
    CorePart		    core;
    CompositePart	    composite;
    ShellPart		    shell;
    WMShellPart		    wm;
    VendorShellPart	    vendor;
    TransientShellPart	    transient;
    XmDialogShellPart	    dialog;
} XmDialogShellWidgetRec;



/* DialogShell class structure */

typedef struct 
{
    XtPointer			extension;	 /* Pointer to extension record */
} XmDialogShellClassPart;


/* Full class record declaration */

typedef struct _XmDialogShellClassRec 
{
    CoreClassPart 		core_class;
    CompositeClassPart 		composite_class;
    ShellClassPart 		shell_class;
    WMShellClassPart	        wm_shell_class;
    VendorShellClassPart 	vendor_shell_class;
    TransientShellClassPart  	transient_shell_class;
    XmDialogShellClassPart 	dialog_shell_part;
} XmDialogShellClassRec;


externalref XmDialogShellClassRec  xmDialogShellClassRec;


/********    Private Function Declarations    ********/


/********    End Private Function Declarations    ********/


#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmDialogShellP_h */
/* DON'T ADD STUFF AFTER THIS #endif */
