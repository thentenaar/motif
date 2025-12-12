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
#ifndef _XmCommandP_h
#define _XmCommandP_h

#include <Xm/SelectioBP.h>
#include <Xm/Command.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Constraint part record for Command widget */

typedef struct _XmCommandConstraintPart
{
  char unused;
} XmCommandConstraintPart, * XmCommandConstraint;

/*  New fields for the Command widget class record  */

typedef struct
{
  XtPointer           extension;      /* Pointer to extension record */
} XmCommandClassPart;


/* Full class record declaration */

typedef struct _XmCommandClassRec
{
  CoreClassPart            core_class;
  CompositeClassPart       composite_class;
  ConstraintClassPart      constraint_class;
  XmManagerClassPart       manager_class;
  XmBulletinBoardClassPart bulletin_board_class;
  XmSelectionBoxClassPart  selection_box_class;
  XmCommandClassPart       command_class;
} XmCommandClassRec;

externalref XmCommandClassRec xmCommandClassRec;

/* New fields for the Command widget record */

typedef struct
{
  XtCallbackList   callback;
  XtCallbackList   value_changed_callback;
  int              history_max_items;
  Boolean          error;        /* error has been made visible in list */
} XmCommandPart;


/****************************************************************
 *
 * Full instance record declaration
 *
 ****************************************************************/

typedef struct _XmCommandRec
{
    CorePart	        core;
    CompositePart       composite;
    ConstraintPart      constraint;
    XmManagerPart       manager;
    XmBulletinBoardPart bulletin_board;
    XmSelectionBoxPart  selection_box;
    XmCommandPart       command;
} XmCommandRec;


#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmCommandP_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
