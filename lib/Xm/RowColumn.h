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
#ifndef _XmRowColumn_h
#define _XmRowColumn_h

#include <Xm/Xm.h>

#ifdef __cplusplus
extern "C" {
#endif

externalref WidgetClass xmRowColumnWidgetClass;

typedef struct _XmRowColumnClassRec * XmRowColumnWidgetClass;
typedef struct _XmRowColumnRec      * XmRowColumnWidget;

#ifndef XmIsRowColumn
#define XmIsRowColumn(w) XtIsSubclass((w),xmRowColumnWidgetClass)
#endif


/********    Public Function Declarations    ********/

extern void XmMenuPosition( 
                        Widget p,
                        XButtonPressedEvent *event) ;
extern Widget XmCreateRowColumn( 
                        Widget p,
                        char *name,
                        ArgList al,
                        Cardinal ac) ;
extern Widget XmCreateWorkArea( 
                        Widget p,
                        char *name,
                        ArgList al,
                        Cardinal ac) ;
extern Widget XmCreateRadioBox( 
                        Widget p,
                        char *name,
                        ArgList al,
                        Cardinal ac) ;
extern Widget XmCreateOptionMenu( 
                        Widget p,
                        char *name,
                        ArgList al,
                        Cardinal ac) ;
extern Widget XmOptionLabelGadget( 
                        Widget m) ;
extern Widget XmOptionButtonGadget( 
                        Widget m) ;
extern Widget XmCreateMenuBar( 
                        Widget p,
                        char *name,
                        ArgList al,
                        Cardinal ac) ;
extern Widget XmCreatePopupMenu( 
                        Widget p,
                        char *name,
                        ArgList al,
                        Cardinal ac) ;
extern Widget XmCreatePulldownMenu( 
                        Widget p,
                        char *name,
                        ArgList al,
                        Cardinal ac) ;
extern Widget XmGetPostedFromWidget( 
                        Widget menu) ;
extern Widget XmGetTearOffControl(
			Widget menu) ;

extern void XmAddToPostFromList(
			Widget m,
			Widget widget );
extern void XmRemoveFromPostFromList(
			Widget m,
			Widget widget );
/*
 * Variable argument list functions
 */

extern Widget XmVaCreateRowColumn(
                        Widget parent,
                        char *name,
                        ...);
extern Widget XmVaCreateManagedRowColumn(
                        Widget parent,
                        char *name,
                        ...);

/********    End Public Function Declarations    ********/


#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmRowColumn_h  */
/* DON'T ADD STUFF AFTER THIS #endif */
