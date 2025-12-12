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
#ifndef _XmDialogShell_h
#define _XmDialogShell_h

#include <Xm/Xm.h>

#ifdef __cplusplus
extern "C" {
#endif


#ifndef XmIsDialogShell
#define XmIsDialogShell(w)	XtIsSubclass(w, xmDialogShellWidgetClass)
#endif /* XmIsDialogShell */

externalref WidgetClass xmDialogShellWidgetClass;

typedef struct _XmDialogShellClassRec       * XmDialogShellWidgetClass;
typedef struct _XmDialogShellRec            * XmDialogShellWidget;


/********    Public Function Declarations    ********/

extern Widget XmCreateDialogShell( 
                        Widget p,
                        char *name,
                        ArgList al,
                        Cardinal ac) ;

/********    End Public Function Declarations    ********/


#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmDialogShell_h */
/* DON'T ADD STUFF AFTER THIS #endif */
