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
/*********************************************************************
 *
 * XmPanedWindowWidget Private Data
 *
 *********************************************************************/

#ifndef _XmPanedWP_h
#define _XmPanedWP_h

#include <Xm/PanedW.h>
#include <Xm/ManagerP.h>

#ifdef __cplusplus
extern "C" {
#endif

/* New fields for the PanedWindow widget class record */

typedef struct _XmPanedWindowClassPart
{
    XtPointer extension;
} XmPanedWindowClassPart;


/* Full Class record declaration */

typedef struct _XmPanedWindowClassRec {
    CoreClassPart       core_class;
    CompositeClassPart  composite_class;
    ConstraintClassPart constraint_class;
    XmManagerClassPart  manager_class;
    XmPanedWindowClassPart     paned_window_class;
} XmPanedWindowClassRec;

externalref XmPanedWindowClassRec xmPanedWindowClassRec;


/* PanedWindow constraint record */

typedef struct _XmPanedWindowConstraintPart {
    int		position;	/* position location in PanedWindow */
    int		dheight;	/* Desired size */
    Position	dy;		/* Desired Location */
    Position	olddy;		/* The last value of dy. */
    Dimension	min;		/* Minimum height */
    Dimension	max;		/* Maximum height */
    Boolean     isPane;         /* true if constraint of pane, false if
				   constraint of sash */
    Boolean	allow_resize;	/* TRUE iff child resize requests are ok */
    Boolean	skip_adjust;	/* TRUE iff child's height should not be */
				/* changed without explicit user action. */
    Widget	sash;		/* The sash for this child */
    Widget	separator;	/* The separator for this child */
    short       position_index; /* new 1.2 positionIndex resource */
} XmPanedWindowConstraintPart;

typedef struct _XmPanedWindowConstraintRec 
{
    XmManagerConstraintPart manager;
    XmPanedWindowConstraintPart  panedw;
} XmPanedWindowConstraintRec, * XmPanedWindowConstraintPtr;


/* New Fields for the PanedWindow widget record */

typedef struct {
    /* resources */
    Boolean     refiguremode;        /* Whether to refigure changes right now */
    Boolean	separator_on;	     /* make separator visible */

    Dimension  	margin_width;	     /* space between right and left edges of
					PanedWindow window and it's children */
    Dimension  	margin_height;	     /* space between top and bottom edges of
					PanedWindow window and it's children */
    Dimension   spacing;             /* whitespace between panes
				        around window, else leave none */
    /* sash modifying resources */
    Dimension	sash_width;	       /* Modify sash width */
    Dimension	sash_height;	       /* Modify sash height */
    Dimension   sash_shadow_thickness; /* Modify sash shadow_thickness */

    Position    sash_indent;           /* Location of sashs (offset
                                          from right margin)	*/
    /* private */
    int         starty;               /* mouse origin when adjusting */

    short	increment_count;      /* Sash increment count */
    short       pane_count;           /* number of managed panes */
    short       num_slots;	      /* number of avail. slots for children*/
    short       num_managed_children; /* holds number of managed children */

    Boolean     recursively_called;    /* For change_managed AND creation of
					* private sash and separator
					* children
					*/
    Boolean     resize_at_realize;    /* For realize if GeometryNo condition */

    XmPanedWindowConstraintPtr top_pane;    /* pane closest to 0 index */
    XmPanedWindowConstraintPtr bottom_pane; /* pane farthest away from 0 index*/

    GC          flipgc;               /* GC to use when animating borders */
    WidgetList  managed_children;     /* keep track of managed children */

    unsigned char orientation ;     /* horizontal or vertical panedw */
    
    XtIntervalId timer ;            /* handle key event on sash */
} XmPanedWindowPart;

/**************************************************************************
 *
 * Full instance record declaration
 *
 **************************************************************************/

typedef struct _XmPanedWindowRec {
    CorePart       core;
    CompositePart  composite;
    ConstraintPart constraint;
    XmManagerPart  manager;
    XmPanedWindowPart   paned_window;
} XmPanedWindowRec;


/********    Private Function Declarations    ********/


/********    End Private Function Declarations    ********/


#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmPanedWP_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
