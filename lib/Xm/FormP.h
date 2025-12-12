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
#ifndef _XmFormP_h
#define _XmFormP_h


#include <Xm/Form.h>
#include <Xm/BulletinBP.h>

#ifdef __cplusplus
extern "C" {
#endif

/* define index constants which are used to access attribute array of Form 
 * so that subclass of Form can make use of them.
 */

#define _XmFORM_LEFT    0
#define _XmFORM_RIGHT   1
#define _XmFORM_TOP     2
#define _XmFORM_BOTTOM  3


typedef struct _XmFormAttachmentRec 
{
   unsigned char type;
   Widget w;
   int percent;
   int offset;
   int value;
   int tempValue;
} XmFormAttachmentRec, * XmFormAttachment;


#ifdef att
#undef att
#endif

typedef struct _XmFormConstraintPart
{
   XmFormAttachmentRec att[4];
   Widget next_sibling;
   Boolean sorted;
   Boolean resizable;
   Dimension preferred_width, preferred_height;
} XmFormConstraintPart, * XmFormConstraint;

typedef struct _XmFormConstraintRec
{
   XmManagerConstraintPart manager;
   XmFormConstraintPart    form;
} XmFormConstraintRec, * XmFormConstraintPtr;


/*  Form class structure  */

typedef struct _XmFormClassPart
{
   XtPointer extension;   /* Pointer to extension record */
} XmFormClassPart;


/*  Full class record declaration for form class  */

typedef struct _XmFormClassRec
{
   CoreClassPart       core_class;
   CompositeClassPart  composite_class;
   ConstraintClassPart constraint_class;
   XmManagerClassPart  manager_class;
   XmBulletinBoardClassPart  bulletin_board_class;
   XmFormClassPart     form_class;
} XmFormClassRec;

externalref XmFormClassRec xmFormClassRec;


/*  The Form instance record  */

typedef struct _XmFormPart
{
   Dimension horizontal_spacing;
   Dimension vertical_spacing;
   int fraction_base;
   Boolean rubber_positioning;
   Widget first_child;
   Boolean initial_width, initial_height;
   Boolean processing_constraints;
} XmFormPart;


/*  Full instance record declaration  */

typedef struct _XmFormRec
{
   CorePart	  core;
   CompositePart  composite;
   ConstraintPart constraint;
   XmManagerPart  manager;
   XmBulletinBoardPart  bulletin_board;
   XmFormPart     form;
} XmFormRec;

/********    Private Function Declarations    ********/


/********    End Private Function Declarations    ********/

#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmFormP_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
