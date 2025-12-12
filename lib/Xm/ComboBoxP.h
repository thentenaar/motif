/* $XConsortium: ComboBoxP.h /main/8 1995/09/19 23:00:21 cde-sun $ */
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
/*
 * HISTORY
 */
/*	ComboBoxP.h  */
#ifndef _XmComboBoxP_H
#define _XmComboBoxP_H

#include <Xm/ManagerP.h>
#include <Xm/ComboBox.h> 


#ifdef __cplusplus
extern "C" {
#endif

/* New fields for the ComboBox widget class record. */

typedef struct _XmComboBoxClassPart {
  XtPointer extension;		/* Pointer to extension record. */
} XmComboBoxClassPart;


/* Full class record declaration. */	
typedef struct _XmComboBoxClassRec {
  CoreClassPart		core_class;
  CompositeClassPart	composite_class;
  ConstraintClassPart	constraint_class;
  XmManagerClassPart	manager_class;
  XmComboBoxClassPart	combo_box_class;
} XmComboBoxClassRec;

externalref XmComboBoxClassRec xmComboBoxClassRec;

/*
 * New fields for the ComboBox widget record.	
 */

typedef struct _XmComboBoxPart {
  /* Resources */
  unsigned char 	type;
  unsigned char		match_behavior;
  Dimension 		highlight_thickness;
  Dimension 		arrow_size;
  Dimension 		arrow_spacing;
  Dimension 		margin_width;
  Dimension 		margin_height;
  XtCallbackList	selection_callback;
  XmString		selected_item; /* synthetic, not updated */
  int			selected_position;
  XmFontList		render_table;

  /* Internal data */
  Widget		list_shell; 
  Widget		list;		/* Now accessible as a resource */
  Widget		scrolled_w; 
  Widget		vsb;
  Widget		hsb;
  int 			ideal_ebheight;
  int			ideal_ebwidth;
  GC 			arrow_GC;
  XRectangle 		hit_rect;
  Dimension 		arrow_shadow_width;
  Boolean 		arrow_pressed;
  Boolean		highlighted;
  Boolean		scrolling;
  XtEnum		shell_state;
  /* NOTE that text_changed is also used for MT_safe resolution of
   * the XmNRenderTable, XmNFontList resource settings 
   */
  Boolean		text_changed;

  /* New resources/data for CDE compatibility. */
  Widget		edit_box;
  XmStringTable		items;
  int			item_count;
  int			visible_item_count;
  short			columns;
  XtEnum		position_mode;

} XmComboBoxPart;


/* Full instance record declaration. */	

typedef struct _XmComboBoxRec {
  CorePart		core;
  CompositePart		composite;
  ConstraintPart	constraint;
  XmManagerPart		manager;
  XmComboBoxPart 	combo_box;
} XmComboBoxRec;


/********    Private Function Declarations    ********/

/********    End Private Function Declarations    ********/


/* Access macros */
#define CB_ArrowPressed(w)   (((XmComboBoxWidget)(w))->combo_box.arrow_pressed)
#define CB_ArrowSize(w)	     (((XmComboBoxWidget)(w))->combo_box.arrow_size)
#define CB_ArrowSpacing(w)   (((XmComboBoxWidget)(w))->combo_box.arrow_spacing)
#define CB_EditBox(w) 	     (((XmComboBoxWidget)(w))->combo_box.edit_box)
#define CB_HighlightThickness(w)	\
	(((XmComboBoxWidget)(w))->combo_box.highlight_thickness)
#define CB_Highlighted(w)    (((XmComboBoxWidget)(w))->combo_box.highlighted)
#define CB_HitRect(w)	     (((XmComboBoxWidget)(w))->combo_box.hit_rect)
#define CB_List(w) 	     (((XmComboBoxWidget)(w))->combo_box.list)
#define CB_ListShell(w)      (((XmComboBoxWidget)(w))->combo_box.list_shell)
#define CB_MarginHeight(w)   (((XmComboBoxWidget)(w))->combo_box.margin_height)
#define CB_MarginWidth(w)    (((XmComboBoxWidget)(w))->combo_box.margin_width)
#define CB_MatchBehavior(w)		\
	(((XmComboBoxWidget)(w))->combo_box.match_behavior)
#define CB_PositionMode(w)   (((XmComboBoxWidget)(w))->combo_box.position_mode)
#define CB_RenderTable(w)    (((XmComboBoxWidget)(w))->combo_box.render_table)
#define CB_ScrolledW(w)      (((XmComboBoxWidget)(w))->combo_box.scrolled_w)
#define CB_SelectionCB(w)		\
	(((XmComboBoxWidget)(w))->combo_box.selection_callback)
#define CB_ShellState(w)     (((XmComboBoxWidget)(w))->combo_box.shell_state)
#define CB_TextChanged(w)    (((XmComboBoxWidget)(w))->combo_box.text_changed)
#define CB_Type(w) 	     (((XmComboBoxWidget)(w))->combo_box.type)


#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif


#endif /* _XmComboBoxP_H */
/* DON'T ADD ANYTHING AFTER THIS #endif */
