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
#ifndef _XmMessage_h
#define _XmMessage_h

#include <Xm/Xm.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Class record constants */

externalref WidgetClass xmMessageBoxWidgetClass;

typedef struct _XmMessageBoxClassRec * XmMessageBoxWidgetClass;
typedef struct _XmMessageBoxRec      * XmMessageBoxWidget;

/* fast XtIsSubclass define */
#ifndef XmIsMessageBox
#define XmIsMessageBox(w) XtIsSubclass (w, xmMessageBoxWidgetClass)
#endif


/********    Public Function Declarations    ********/

extern Widget XmCreateMessageBox(
                        Widget parent,
                        char *name,
                        ArgList al,
                        Cardinal ac) ;
extern Widget XmCreateMessageDialog(
                        Widget parent,
                        char *name,
                        ArgList al,
                        Cardinal ac) ;
extern Widget XmCreateErrorDialog(
                        Widget parent,
                        char *name,
                        ArgList al,
                        Cardinal ac) ;
extern Widget XmCreateInformationDialog(
                        Widget parent,
                        char *name,
                        ArgList al,
                        Cardinal ac) ;
extern Widget XmCreateQuestionDialog(
                        Widget parent,
                        char *name,
                        ArgList al,
                        Cardinal ac) ;
extern Widget XmCreateWarningDialog(
                        Widget parent,
                        char *name,
                        ArgList al,
                        Cardinal ac) ;
extern Widget XmCreateWorkingDialog(
                        Widget parent,
                        char *name,
                        ArgList al,
                        Cardinal ac) ;
extern Widget XmCreateTemplateDialog(
                        Widget parent,
                        char *name,
                        ArgList al,
                        Cardinal ac) ;

XM_ALTERNATIVE(Use XtNameToWidget instead)
extern Widget XmMessageBoxGetChild(
                        Widget widget,
                        unsigned char child);

/*
 * Variable argument list functions
 */

extern Widget XmVaCreateMessageBox(
                        Widget parent,
                        char *name,
                        ...);
extern Widget XmVaCreateManagedMessageBox(
                        Widget parent,
                        char *name,
                        ...);

/********    End Public Function Declarations    ********/


#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmMessage_h */
