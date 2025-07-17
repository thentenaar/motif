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
#ifndef _XmCommand_h
#define _XmCommand_h

#include <Xm/Xm.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Class record constants */

externalref WidgetClass xmCommandWidgetClass;

typedef struct _XmCommandClassRec * XmCommandWidgetClass;
typedef struct _XmCommandRec      * XmCommandWidget;

#ifndef XmIsCommand
#define XmIsCommand(w)  (XtIsSubclass (w, xmCommandWidgetClass))
#endif

/********    Public Function Declarations    ********/

extern Widget XmCreateCommand(
                        Widget parent,
                        String name,
                        ArgList al,
                        Cardinal ac) ;
extern Widget XmVaCreateCommand(
                        Widget parent,
                        char *name,
                        ...);
extern Widget XmVaCreateManagedCommand(
                        Widget parent,
                        char *name,
                        ...);
extern void XmCommandSetValue(
                        Widget widget,
                        XmString value) ;
extern void XmCommandAppendValue(
                        Widget widget,
                        XmString value) ;
extern void XmCommandError(
                        Widget widget,
                        XmString error) ;
extern Widget XmCreateCommandDialog(
                        Widget ds_p,
                        String name,
                        ArgList fsb_args,
                        Cardinal fsb_n) ;

XM_ALTERNATIVE(Use XtNameToWidget instead)
extern Widget XmCommandGetChild(
                        Widget widget,
                        unsigned char child);

/********    End Public Function Declarations    ********/


#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmCommand_h */
