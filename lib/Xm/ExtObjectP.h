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
#ifndef _XmExtObjectP_h
#define _XmExtObjectP_h

#include <Xm/XmP.h>

#ifdef __cplusplus
extern "C" {
#endif

enum {
  XmCACHE_EXTENSION = 1,		XmDESKTOP_EXTENSION,
  XmSHELL_EXTENSION,			XmPROTOCOL_EXTENSION,
  XmDEFAULT_EXTENSION
};

#ifndef XmIsExtObject
#define XmIsExtObject(w)	XtIsSubclass(w, xmExtObjectClass)
#endif /* XmIsExtObject */

#define XmLOGICAL_PARENT_RESOURCE	(0x80 << sizeof(Cardinal))

/* Class record constants */

typedef struct _XmExtRec *XmExtObject;
typedef struct _XmExtClassRec *XmExtObjectClass;

externalref WidgetClass xmExtObjectClass;

/* Class Extension definitions */

typedef struct _XmExtClassPart {
  XmSyntheticResource *syn_resources;   
  int		       num_syn_resources;   
  XtPointer	       extension;
} XmExtClassPart, *XmExtClassPartPtr;

typedef struct _XmExtClassRec {
  ObjectClassPart object_class;
  XmExtClassPart  ext_class;
} XmExtClassRec;

typedef struct {
  Widget	logicalParent;
  unsigned char	extensionType;
} XmExtPart, *XmExtPartPtr;

externalref XmExtClassRec xmExtClassRec;

typedef struct _XmExtRec {
  ObjectPart object;
  XmExtPart  ext;
} XmExtRec;


#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif  /* _XmExtObjectP_h */
