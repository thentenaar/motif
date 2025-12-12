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
 * 
 */

#ifndef _XmIconBoxP_h
#define _XmIconBoxP_h

#if defined(VMS) || defined(__VMS)
#include <X11/apienvset.h>
#endif

/************************************************************
*	INCLUDE FILES
*************************************************************/
#include <Xm/ManagerP.h>
#include <Xm/IconBox.h>

#ifdef __cplusplus
extern "C" {
#endif

/************************************************************
*	TYPEDEFS AND DEFINES
*************************************************************/


/*
 * Hack to get around naming conventions.  The XmConstraintPartOffset macro
 * requires our contraint part structure to have this name
 */
#define XmIconBoxConstraintPart IconInfo

/*
 * Access macros for widget instance fields
 */
#define XmIconBox_min_v_cells(w)	(((XmIconBoxWidget)(w))->box.min_v_cells)
#define XmIconBox_min_h_cells(w)	(((XmIconBoxWidget)(w))->box.min_h_cells)
#define XmIconBox_v_margin(w)	(((XmIconBoxWidget)(w))->box.v_margin)
#define XmIconBox_h_margin(w)	(((XmIconBoxWidget)(w))->box.h_margin)
#define XmIconBox_min_cell_width(w)  (((XmIconBoxWidget)(w))->box.min_cell_width)
#define XmIconBox_min_cell_height(w) (((XmIconBoxWidget)(w))->box.min_cell_height)
#define XmIconBox_cell_width(w)	(((XmIconBoxWidget)(w))->box.cell_width)
#define XmIconBox_cell_height(w)	(((XmIconBoxWidget)(w))->box.cell_height)

#define XmIconBoxC_cell_x(w)      (((XmIconBoxConstraintsRec*)((w)->core.constraints))->icon.cell_x)
#define XmIconBoxC_cell_y(w)      (((XmIconBoxConstraintsRec*)((w)->core.constraints))->icon.cell_y)
#define XmIconBoxC_pref_width(w)  (((XmIconBoxConstraintsRec*)((w)->core.constraints))->icon.pref_width)
#define XmIconBoxC_pref_height(w) (((XmIconBoxConstraintsRec*)((w)->core.constraints))->icon.pref_height)

/************************************************************
*	MACROS
*************************************************************/

/************************************************************
*	GLOBAL DECLARATIONS
*************************************************************/

typedef struct {
    XtPointer extension;	/* Just in case we need it later. */
} XmIconBoxClassPart;

typedef struct _XmIconBoxClassRec {
    CoreClassPart		core_class;
    CompositeClassPart		composite_class;
    ConstraintClassPart		constraint_class;
    XmManagerClassPart		manager_class;
    XmIconBoxClassPart	        box_class;
} XmIconBoxClassRec;

typedef struct {
    /* resources */

    Dimension min_v_cells;	/* Default number of cells in the vert dir. */
    Dimension min_h_cells;	/* Default number of cells in the horiz dir. */
    Dimension v_margin;		/* Amount of space to leave between cells */
    Dimension h_margin;		/* and window edges. */
    Dimension min_cell_width;	/* Minimum width of the cells. */
    Dimension min_cell_height;	/* Minimum height of the cells. */

    /* private state */

    Dimension cell_width;	/* Width and height of all cells. */
    Dimension cell_height;

} XmIconBoxPart;


typedef struct _XmIconBoxRec {
    CorePart		core;
    CompositePart	composite;
    ConstraintPart	constraint;
    XmManagerPart	manager;
    XmIconBoxPart	box;
} XmIconBoxRec;

typedef struct _IconInfo {

    /*
     * Resources.
     */

    short cell_x;		/* X location of this icon in cell space. */
    short cell_y;		/* Y location of this icon in cell space. */

    /*
     * Private state.
     */

    Dimension pref_width, pref_height; /* The preferred size of this widget. */
} IconInfo;

typedef struct _XmIconBoxConstraintsRec {
    XmManagerConstraintPart	manager;
    IconInfo			icon;
} XmIconBoxConstraintsRec, *XmIconBoxConstraints;

/************************************************************
*	EXTERNAL DECLARATIONS
*************************************************************/

extern XmIconBoxClassRec xmIconBoxClassRec;

/************************************************************
*	STATIC DECLARATIONS
*************************************************************/

#ifdef __cplusplus
}	/* Closes scope of 'extern "C"' declaration */
#endif

#if defined(VMS) || defined(__VMS)
#include <X11/apienvrst.h>
#endif

#endif /* _XmIconBoxP_h */
