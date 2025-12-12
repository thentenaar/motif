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
#ifndef _XmDialogShellExtP_h
#define _XmDialogShellExtP_h

#include <Xm/VendorSEP.h>

#ifdef __cplusplus
extern "C" {
#endif


#ifndef XmIsDialogShellExt
#define XmIsDialogShellExt(w)	XtIsSubclass(w, xmDialogShellExtObjectClass)
#endif /* XmIsDialogShellExt */

externalref WidgetClass xmDialogShellExtObjectClass;

typedef struct _XmDialogShellExtClassRec	*XmDialogShellExtObjectClass ;
typedef struct _XmDialogShellExtRec		*XmDialogShellExtObject ;


typedef struct _XmDialogShellExtClassPart{
   XtPointer extension;   /* Pointer to extension record */
}XmDialogShellExtClassPart, *XmDialogShellExtClassPartPtr;

typedef struct _XmDialogShellExtClassRec{
    ObjectClassPart		object_class;
    XmExtClassPart		ext_class;
    XmDesktopClassPart		desktop_class;
    XmShellExtClassPart		shell_class;
    XmVendorShellExtClassPart 	vendor_class;
    XmDialogShellExtClassPart 	dialog_class;
}XmDialogShellExtClassRec;

typedef struct _XmDialogShellExtPart{
    int		      	empty;
} XmDialogShellExtPart;

externalref XmDialogShellExtClassRec 	xmDialogShellExtClassRec;

typedef struct _XmDialogShellExtRec{
    ObjectPart			object;
    XmExtPart			ext;
    XmDesktopPart		desktop;
    XmShellExtPart		shell;
    XmVendorShellExtPart 	vendor;
    XmDialogShellExtPart 	dialog;
}XmDialogShellExtRec;


/********    Private Function Declarations    ********/


/********    End Private Function Declarations    ********/


#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmDialogShellExtP_h */
/* DON'T ADD STUFF AFTER THIS #endif */
