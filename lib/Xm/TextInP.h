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
#ifndef _XmTextInP_h
#define _XmTextInP_h

#include <Xm/Text.h>
#include <Xm/TextStrSoP.h>

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************
 *
 * Definitions for modules implementing text input modules.
 *
 ****************************************************************/

typedef struct {
    int x;
    int y;
} SelectionHint;

typedef struct _InputDataRec {
    XmTextWidget widget;		/* Back-pointer to widget record. */
    XmTextScanType *sarray;	/* Description of what to cycle through on */
				/* selections. */
    int sarraycount;		/* Number of elements in above. */
    int new_sel_length;		/* New selection length for selection moves. */
    int threshold;		/* number of pixels crossed -> drag */
    SelectionHint selectionHint; /* saved coords of button down */
    SelectionHint Sel2Hint;	/* saved the coords of button down */
    XtIntervalId select_id;
    XmTextScanType stype;	/* Current selection type. */
    XmTextScanDirection extendDir;
    XmTextScanDirection Sel2ExtendDir;
    XmTextPosition origLeft, origRight;
    XmTextPosition Sel2OrigLeft, Sel2OrigRight;
    XmTextPosition stuffpos;
    XmTextPosition sel2Left, sel2Right; /* secondary selection */
    XmTextPosition anchor;	/* anchor point of the primary selection */
    Position select_pos_x;	/* x position for timer-based scrolling */
    Position select_pos_y;	/* y position for timer-based scrolling */
    Boolean pendingdelete;	/* TRUE if we're implementing pending delete */
    Boolean syncing;		/* If TRUE, then we've multiple keystrokes */
    Boolean extending;      /* true if we are extending */
    Boolean Sel2Extending;	/* true if we are extending */
    Boolean hasSel2;   		/* has secondary selection */
    Boolean has_destination;  	/* has destination selection */
    Boolean selectionMove;	/* delete selection after stuff */
    Boolean cancel;		/* indicates that cancel was pressed */
    Boolean overstrike;     	/* overstrike */
    Boolean sel_start;		/* indicates that a btn2 was pressed */
    Time dest_time;		/* time of destination selection ownership */
    Time sec_time;		/* time of secondary selection ownership */
    Time lasttime;		/* Time of last event. */
    Boolean selectionLink;	/* This is a link vs. a copy operation */
    XtIntervalId drag_id;       /* timer to start btn1 drag */
    _XmTextActionRec *transfer_action;  /* to keep track of delayed action */
} InputDataRec, *InputData;


/* 
 * Create a new instance of an input object.  By the time this is called,
 * the widget context has been saved.
 */

typedef void (*InputCreateProc)(
			Widget,
			ArgList,
			Cardinal) ;

/*
 * Get values out of the input object.
 */
typedef void (*InputGetValuesProc)(
			Widget,
			ArgList,
			Cardinal) ;

/*
 * Set values in the input object.
 */

typedef void (*InputSetValuesProc)(
			Widget,
			Widget,
			Widget,
			ArgList,
			Cardinal *) ;

/*
 * Inform input of invalidated positions.
 */
typedef void (*InputInvalidateProc)(
			XmTextWidget,
			XmTextPosition,
			XmTextPosition,
			long) ;

/*
 * Get secondary resources.
 */
typedef void (*InputGetSecResProc)(
			XmSecondaryResourceData *) ;


typedef struct _InputRec {
    struct _InputDataRec *data;	/* Input-specific data; opaque type. */
    InputInvalidateProc Invalidate;
    InputGetValuesProc  GetValues;
    InputSetValuesProc	SetValues;
    XtWidgetProc	destroy;
    InputGetSecResProc  GetSecResData;
} InputRec;


externalref XtPointer _XmdefaultTextActionsTable;
externalref Cardinal  _XmdefaultTextActionsTableSize;


#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmTextInP_h */
/*DON'T ADD ANYTHING AFTER THIS #endif */
