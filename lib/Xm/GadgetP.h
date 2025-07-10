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
#ifndef _XmGadgetP_h
#define _XmGadgetP_h

#ifndef _XmNO_BC_INCL
#define _XmNO_BC_INCL
#endif

#include <Xm/XmP.h>

#ifdef __cplusplus
extern "C" {
#endif


/*  Masks to define input the gadget is interested in  */

#define XmNO_EVENT              0x000
#define XmENTER_EVENT           0x001
#define XmLEAVE_EVENT           0x002
#define XmFOCUS_IN_EVENT        0x004
#define XmFOCUS_OUT_EVENT       0x008
#define XmMOTION_EVENT          0x010
#define XmARM_EVENT             0x020
#define XmACTIVATE_EVENT        0x040
#define XmHELP_EVENT            0x080
#define XmKEY_EVENT             0x100
#define XmMULTI_ARM_EVENT       0x200
#define XmMULTI_ACTIVATE_EVENT  0x400
#define XmBDRAG_EVENT		0x800
#define XmALL_EVENT             0xFFF


/* Gadget access Macros */
#define G_ShadowThickness(g) \
	(((XmGadget)(g))->gadget.shadow_thickness)
#define G_HighlightThickness(g) \
	(((XmGadget)(g))->gadget.highlight_thickness)

#define GCEPTR(wc)  ((XmGadgetClassExt *)(&(((XmGadgetClass)(wc))->gadget_class.extension)))
#define _XmGetGadgetClassExtPtr(wc, owner) \
  ((*GCEPTR(wc) && (((*GCEPTR(wc))->record_type) == owner))\
   ? GCEPTR(wc) \
   :((XmGadgetClassExt *) _XmGetClassExtensionPtr(((XmGenericClassExt *)GCEPTR(wc)), owner)))

#define XmGadgetClassExtVersion 2L


/* Gadget cache header for each gadget's Cache Part */

typedef struct _XmGadgetCache
{
   struct _XmGadgetCache	*next;
   struct _XmGadgetCache	*prev;
   int				ref_count;
} XmGadgetCache, *XmGadgetCachePtr;


/* A cache entry for each class which desires gadget caching */

typedef struct _XmCacheClassPart
{
   XmGadgetCache	cache_head;
   XmCacheCopyProc	cache_copy;
   XmGadgetCacheProc	cache_delete;
   XmCacheCompareProc   cache_compare;
} XmCacheClassPart, *XmCacheClassPartPtr;

/* A struct for properly aligning the data part of the cache entry. */

typedef struct _XmGadgetCacheRef
{
   XmGadgetCache	cache;
   XtArgVal		data;
} XmGadgetCacheRef, *XmGadgetCacheRefPtr;

/*  Gadget class structure  */

typedef struct _XmGadgetClassExtRec{
    XtPointer           next_extension;
    XrmQuark            record_type;
    long                version;
    Cardinal            record_size;
    XmWidgetBaselineProc widget_baseline;
    XmWidgetDisplayRectProc  widget_display_rect;
    XmWidgetMarginsProc widget_margins;
}XmGadgetClassExtRec, *XmGadgetClassExt;

typedef struct _XmGadgetClassPart
{
   XtWidgetProc         border_highlight;
   XtWidgetProc         border_unhighlight;
   XtActionProc         arm_and_activate;
   XmWidgetDispatchProc input_dispatch;
   XmVisualChangeProc   visual_change;		/* unused in Motif 2.0 */
   XmSyntheticResource * syn_resources;
   int                  num_syn_resources;
   XmCacheClassPartPtr	cache_part;
   XtPointer		extension;
} XmGadgetClassPart;


/*  Full class record declaration for Gadget class  */

typedef struct _XmGadgetClassRec
{
   RectObjClassPart  rect_class;
   XmGadgetClassPart gadget_class;
} XmGadgetClassRec;

externalref XmGadgetClassRec xmGadgetClassRec;


/*  The Gadget instance record  */

typedef struct _XmGadgetPart
{
   Dimension shadow_thickness;
   Dimension highlight_thickness;

   XtCallbackList help_callback;
   XtPointer      user_data;

   Boolean traversal_on;
   Boolean highlight_on_enter;
   Boolean have_traversal;

   unsigned char unit_type;
   XmNavigationType navigation_type;

   Boolean highlight_drawn;
   Boolean highlighted;
   Boolean visible;

   Mask event_mask;
   XmDirection layout_direction;
#if XM_22_COMPAT
   XmString tool_tip_string;
#endif
} XmGadgetPart;

/*  Full instance record declaration  */

typedef struct _XmGadgetRec
{
   ObjectPart   object;
   RectObjPart  rectangle;
   XmGadgetPart gadget;
} XmGadgetRec;

#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmGadgetP_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
