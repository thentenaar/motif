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
#ifndef _XmMainWindowP_h
#define _XmMainWindowP_h

#include <Xm/MainW.h>
#include <Xm/ScrolledWP.h>
#include <Xm/SeparatoG.h>

#ifdef __cplusplus
extern "C" {
#endif


#define DEFAULT_HEIGHT 20
#define DEFAULT_WIDTH 20
  
/* Constraint part record for MainWindow widget */
typedef struct _XmMainWindowConstraintPart
{
   char unused;
} XmMainWindowConstraintPart, * XmMainWindowConstraint;


/* New fields for the MainWindow widget class record */
typedef struct {
   XtPointer extension;   /* Pointer to extension record */
} XmMainWindowClassPart;

/****************
 *
 * Class record declaration
 *
 ****************/
typedef struct _XmMainWindowClassRec {
    CoreClassPart	core_class;
    CompositeClassPart  composite_class;
    ConstraintClassPart constraint_class;
    XmManagerClassPart  manager_class;
    XmScrolledWindowClassPart	swindow_class;
    XmMainWindowClassPart	mwindow_class;
} XmMainWindowClassRec;

externalref XmMainWindowClassRec xmMainWindowClassRec;

/****************
 *
 * Main Window instance structure.
 *
 ****************/
typedef struct {


   Dimension	AreaWidth,AreaHeight;
   Dimension	margin_width,margin_height;
   Widget       CommandWindow;
   Widget       MenuBar;
   Widget       Message;
   unsigned char CommandLoc;
   XmSeparatorGadget       Sep1,Sep2,Sep3;
   Boolean	ManagingSep;
   Boolean	ShowSep;
   
} XmMainWindowPart;


/************************************************************************
 *									*
 * Full instance record declaration					*
 *									*
 ************************************************************************/

typedef struct _XmMainWindowRec {
    CorePart	    core;
    CompositePart   composite;
    ConstraintPart constraint;
    XmManagerPart   manager;
    XmScrolledWindowPart   swindow;
    XmMainWindowPart   mwindow;
} XmMainWindowRec;


/********    Private Function Declarations    ********/


/********    End Private Function Declarations    ********/


#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmMainWindowP_h */
/* DON'T ADD STUFF AFTER THIS #endif */
