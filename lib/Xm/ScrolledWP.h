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
#ifndef _XmScrolledWindowP_h
#define _XmScrolledWindowP_h

#include <Xm/ManagerP.h>
#include <Xm/ScrolledW.h>

#include <Xm/ScrollBar.h>
#include <Xm/DrawingA.h>

#include <Xm/ScrollFrameT.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*XmGetAutoDragRectsProc)(Widget, XRectangle **, Cardinal *) ;

#define XmInheritGetAutoDragRectsProc ((XmGetAutoDragRectsProc) _XtInherit)

#define XmScrolledWindowClassExtVersion 1L

typedef struct _XmScrolledWindowClassExtRec{
    XtPointer           next_extension;
    XrmQuark            record_type;
    long                version;
    Cardinal            record_size;
    XmGetAutoDragRectsProc get_hor_rects;
    XmGetAutoDragRectsProc get_vert_rects;
} XmScrolledWindowClassExtRec, *XmScrolledWindowClassExt;

/* New fields for the ScrolledWindow widget class record */
typedef struct {
     XtPointer extension;   /* extension to new class methods */
} XmScrolledWindowClassPart;

/****************
 *
 * Class record declaration
 *
 ****************/
typedef struct _XmScrolledWindowClassRec {
    CoreClassPart	core_class;
    CompositeClassPart  composite_class;
    ConstraintClassPart constraint_class;
    XmManagerClassPart  manager_class;
    XmScrolledWindowClassPart	swindow_class;
} XmScrolledWindowClassRec;

externalref XmScrolledWindowClassRec xmScrolledWindowClassRec;

/****************
 *
 * Scrolled Window instance structure.
 *
 ****************/
typedef struct {

   int vmin;		  /*  slider minimum coordinate position     */
   int vmax;		  /*  slider maximum coordinate position     */
   int vOrigin;		  /*  slider edge location                  */
   int vExtent;		  /*  slider size                           */

   int hmin;		  /*  Same as above for horizontal bar.     */
   int hmax;
   int hOrigin;
   int hExtent;

   Position hsbX,hsbY;
   Dimension hsbWidth,hsbHeight;    /* Dimensions for the horiz bar */

   Position vsbX,vsbY;
   Dimension vsbWidth,vsbHeight;    /* Dimensions for the vertical bar */

   Position    GivenHeight, GivenWidth;
   /* these 2 fields are now obsolete, we re-use them for
      a different purpose, to track the initial position
      of the scrollbars */
#define sw_prev_x GivenWidth
#define sw_prev_y GivenHeight

   Dimension	AreaWidth,AreaHeight;
   Dimension	WidthPad,HeightPad;
   Position	XOffset, YOffset;

   Dimension	pad;

   Boolean	hasHSB;
   Boolean	hasVSB;
   Boolean	InInit;
   Boolean	FromResize;

   unsigned char	VisualPolicy;
   unsigned char	ScrollPolicy;
   unsigned char	ScrollBarPolicy;
   unsigned char	Placement;
   
   XmScrollBarWidget   	hScrollBar;
   XmScrollBarWidget   	vScrollBar;
   XmDrawingAreaWidget 	ClipWindow;
   Widget              	WorkWindow;
   
   XtCallbackList       traverseObscuredCallback;

   XtEnum       auto_drag_model;
   XtIntervalId auto_drag_timer; 

   Boolean scroll_frame_inited ;
   XmScrollFrameData scroll_frame_data ;

   XtPointer	auto_drag_rects;
   XtPointer	auto_drag_closure;
} XmScrolledWindowPart;


/************************************************************************
 *									*
 * Full instance record declaration					*
 *									*
 ************************************************************************/

typedef struct _XmScrolledWindowRec {
    CorePart	    core;
    CompositePart   composite;
    ConstraintPart constraint;
    XmManagerPart   manager;
    XmScrolledWindowPart   swindow;
} XmScrolledWindowRec;


/*  Frame constraint records  */

typedef struct _XmScrolledWindowConstraintPart
{
   unsigned char child_type;
   Position orig_x;
   Position orig_y;
} XmScrolledWindowConstraintPart, * XmScrolledWindowConstraint;

typedef struct _XmScrolledWindowConstraintRec
{
   XmManagerConstraintPart manager;
   XmScrolledWindowConstraintPart   swindow;
} XmScrolledWindowConstraintRec, * XmScrolledWindowConstraintPtr;


#define DEFAULT_HEIGHT	20
#define DEFAULT_WIDTH	20

#define RESOURCE_DEFAULT (-1)

#define GetSWConstraint(w) \
        (&((XmScrolledWindowConstraintPtr) (w)->core.constraints)->swindow)


#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmScrolledWindowP_h */
/* DON'T ADD STUFF AFTER THIS #endif */
