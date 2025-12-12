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
#ifndef _XmPrintShellP_h
#define _XmPrintShellP_h

#include <Xm/XmP.h>
#include <X11/ShellP.h>
#include <Xm/Print.h>


#ifdef __cplusplus
extern "C" {
#endif

/* 
 * we make it a appShell subclass so it can have it's own instance
 * hierarchy
 */

typedef struct {
    XmSyntheticResource * syn_resources;   
    int                   num_syn_resources;   
    XtPointer             extension;
} XmPrintShellClassPart;

typedef struct _XmPrintShellClassRec{
    CoreClassPart      		core_class;
    CompositeClassPart 		composite_class;
    ShellClassPart  		shell_class;
    WMShellClassPart   		wm_shell_class;
    VendorShellClassPart 	vendor_shell_class;
    TopLevelShellClassPart 	top_level_shell_class;
    ApplicationShellClassPart 	application_shell_class;
    XmPrintShellClassPart	print_shell_class;
} XmPrintShellClassRec;


typedef struct {
    Boolean          xp_connected ;
    Boolean          last_page ;
    unsigned short   print_resolution ;
    Position         min_x, min_y, max_x, max_y ;
    unsigned short   default_pixmap_resolution ;
    XtCallbackList   start_job_callback;
    XtCallbackList   end_job_callback;
    XtCallbackList   page_setup_callback;
    XtCallbackList   pdm_notification_callback ;
} XmPrintShellPart, *XmPrintShellPartPtr;


typedef struct _XmPrintShellRec{
    CorePart 		core;
    CompositePart 	composite;
    ShellPart 		shell;
    WMShellPart		wm;
    VendorShellPart	vendor;
    TopLevelShellPart 	topLevel;
    ApplicationShellPart application;
    XmPrintShellPart	 print;
} XmPrintShellRec;

externalref XmPrintShellClassRec 	xmPrintShellClassRec;

#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmPrintShellP_h */
/* DON'T ADD STUFF AFTER THIS #endif */

