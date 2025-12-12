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
#ifndef _XmPrimitiveP_h
#define _XmPrimitiveP_h

#ifndef _XmNO_BC_INCL
#define _XmNO_BC_INCL
#endif

#include <Xm/XmP.h>

#ifdef __cplusplus
extern "C" {
#endif


/* Access Macros */

#define Prim_ShadowThickness(w) (((XmPrimitiveWidget)(w))->primitive.shadow_thickness)
#define Prim_HaveTraversal(w) (((XmPrimitiveWidget)(w))->primitive.have_traversal)

#define PCEPTR(wc)  ((XmPrimitiveClassExt *)(&(((XmPrimitiveWidgetClass)(wc))->primitive_class.extension)))
#define _XmGetPrimitiveClassExtPtr(wc, owner) \
  ((*PCEPTR(wc) && (((*PCEPTR(wc))->record_type) == owner))\
   ? PCEPTR(wc) \
   :((XmPrimitiveClassExt *) _XmGetClassExtensionPtr(((XmGenericClassExt *)PCEPTR(wc)), owner)))


#define XmPrimitiveClassExtVersion 1L


typedef struct _XmPrimitiveClassExtRec{
    XtPointer           next_extension;
    XrmQuark            record_type;
    long                version;
    Cardinal            record_size;
    XmWidgetBaselineProc widget_baseline;
    XmWidgetDisplayRectProc  widget_display_rect;
    XmWidgetMarginsProc widget_margins;
} XmPrimitiveClassExtRec, *XmPrimitiveClassExt;

typedef struct _XmPrimitiveClassPart
{
   XtWidgetProc         border_highlight;
   XtWidgetProc         border_unhighlight;
   String               translations;
   XtActionProc         arm_and_activate;
   XmSyntheticResource * syn_resources;
   int                  num_syn_resources;
   XtPointer            extension;
} XmPrimitiveClassPart;

typedef struct _XmPrimitiveClassRec
{
    CoreClassPart        core_class;
    XmPrimitiveClassPart primitive_class;
} XmPrimitiveClassRec;

externalref XmPrimitiveClassRec xmPrimitiveClassRec;


/*  The Primitive instance record  */

typedef struct _XmPrimitivePart
{
   Pixel   foreground;

   Dimension   shadow_thickness;
   Pixel   top_shadow_color;
   Pixmap  top_shadow_pixmap;
   Pixel   bottom_shadow_color;
   Pixmap  bottom_shadow_pixmap;

   Dimension   highlight_thickness;
   Pixel   highlight_color;
   Pixmap  highlight_pixmap;

   XtCallbackList help_callback;
   XtPointer      user_data;

   Boolean traversal_on;
   Boolean highlight_on_enter;
   Boolean have_traversal;

   unsigned char unit_type;
   XmNavigationType navigation_type;

   Boolean highlight_drawn;
   Boolean highlighted;

   GC      highlight_GC;
   GC      top_shadow_GC;
   GC      bottom_shadow_GC;

   /* New fields in Motif 2.0 */

#ifndef XM_PART_BC

   XtCallbackList   convert_callback;       /* Selection convert callback */
   XtCallbackList   popup_handler_callback;

   XmDirection layout_direction;
#endif
} XmPrimitivePart;

#ifdef XM_PART_BC
extern XmDirection XmPrimLayoutDir ;
#define XmPrim_layout_direction(w) (XmPrimLayoutDir)
#else
#define XmPrim_layout_direction(w) ((w)->primitive.layout_direction)
#endif


typedef struct _XmPrimitiveRec
{
   CorePart        core;
   XmPrimitivePart primitive;
} XmPrimitiveRec;


#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmPrimitiveP_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
