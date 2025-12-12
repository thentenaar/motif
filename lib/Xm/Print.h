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

#ifndef _XmPrintShell_h
#define _XmPrintShell_h

#include <Xm/Xm.h>
#include <X11/extensions/Print.h>
  
#ifdef __cplusplus
extern "C" {
#endif

/* Class record constants */

externalref WidgetClass xmPrintShellWidgetClass;

typedef struct _XmPrintShellClassRec * XmPrintShellWidgetClass;
typedef struct _XmPrintShellRec      * XmPrintShellWidget;


#ifndef XmIsPrintShell
#define XmIsPrintShell(w)  (XtIsSubclass (w, xmPrintShellWidgetClass))
#endif

/********    Public Function Declarations    ********/

extern Widget XmPrintSetup(
             Widget           video_widget,
             Screen           *print_screen,
             String            print_shell_name,
             ArgList           args,
             Cardinal          num_args);

extern void XmRedisplayWidget(Widget widget) ;

extern XtEnum XmPrintToFile(Display *dpy, 
			    char *file_name,
			    XPFinishProc finish_proc, 
			    XPointer client_data) ;

extern XtEnum XmPrintPopupPDM(Widget print_shell,
			      Widget transient_for);

/********    End Public Function Declarations    ********/


#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmPrintShell_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
