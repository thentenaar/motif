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
/******************************************************************************
 *
 *	File:	SSpinBP.h
 *	Date:	June 1, 1995
 *	Author:	Mitchell Greess
 *
 *	Contents:
 *		Private header file for the XmSimpleSpinBox widget.
 *		Implements the XmSimpleSpinBox.
 *
 ******************************************************************************/

#ifndef _XmSSpinBP_h
#define _XmSSpinBP_h

#include <Xm/SpinBP.h>
#include <Xm/SSpinB.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _XmSimpleSpinBoxConstraint
{
      int			unused;
} XmSimpleSpinBoxConstraintPart, *XmSimpleSpinBoxConstraint;

typedef struct _XmSimpleSpinBoxConstraintRec
{
    XmManagerConstraintPart	manager;
    XmSpinBoxConstraintPart	spinBox;
    XmSimpleSpinBoxConstraintPart simpleSpinBox;
} XmSimpleSpinBoxConstraintRec, *XmSimpleSpinBoxConstraintPtr;

/*  Simple Spin Box class structure  */
typedef struct _XmSimpleSpinBoxClassPart
{
    XtPointer			extension;	/* not used */
} XmSimpleSpinBoxClassPart;


/*  Full class record declaration for Simple Spin Box class  */
typedef struct _XmSimpleSpinBoxClassRec
{
    CoreClassPart		core_class;
    CompositeClassPart		composite_class;
    ConstraintClassPart		constraint_class;
    XmManagerClassPart		manager_class;
    XmSpinBoxClassPart		spinBox_class;
    XmSimpleSpinBoxClassPart	simpleSpinBox_class;
} XmSimpleSpinBoxClassRec;

externalref XmSimpleSpinBoxClassRec xmSimpleSpinBoxClassRec;

/*  The Simple Spin Box instance record  */
typedef struct _XmSimpleSpinBoxPart
{
    /*
     * (Public) resources
     */
    unsigned char	arrow_sensitivity;
    Boolean		wrap;

    /* Resources for autonumeric mode */
    short		decimal_points;
    int			increment_value;
    int			maximum_value;
    int			minimum_value;

    /* Resources for string values mode */
    int			num_values;	/* number of XmString in the array */
    int			position;	/* 1-based pos'n of current selection */
    unsigned char	position_type;	/* governs interpretation of .position:
					   XmPOSITION_{ARRAY,VALUE} */
    XmStringTable	values;		/* array of XmString */

    /* Resources for the text field child of the XmSimpleSpinBox */
    short		columns;	/* number of columns */
    Boolean		editable;	/* whether the text field is editable */
    unsigned char	sb_child_type;	/* XmSTRING or XmNUMERIC */
    Widget		text_field;

    /* (Private) state */
} XmSimpleSpinBoxPart;


/*  Full instance record declaration  */

typedef struct _XmSimpleSpinBoxRec
{
  CorePart			core;
  CompositePart			composite;
  ConstraintPart		constraint;
  XmManagerPart			manager;
  XmSpinBoxPart			spinBox;
  XmSimpleSpinBoxPart		simpleSpinBox;
} XmSimpleSpinBoxRec;

#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif  /* _SSpinBP_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */



