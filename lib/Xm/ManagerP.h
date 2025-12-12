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
#ifndef _XmManagerP_h
#define _XmManagerP_h

#ifndef _XmNO_BC_INCL
#define _XmNO_BC_INCL
#endif

#include <Xm/XmP.h>

#ifdef __cplusplus
extern "C" {
#endif

/*  Access Macros  */

#define XmParentTopShadowGC(w) 		\
	(((XmManagerWidget) XtParent(w))->manager.top_shadow_GC)

#define XmParentBottomShadowGC(w)	\
	(((XmManagerWidget) XtParent(w))->manager.bottom_shadow_GC)

#define XmParentHighlightGC(w)		\
	(((XmManagerWidget) XtParent(w))->manager.highlight_GC)

#define XmParentBackgroundGC(w)		\
	(((XmManagerWidget) XtParent(w))->manager.background_GC)


#define MGR_KeyboardList(m)		\
			(((XmManagerRec *) (m))->manager.keyboard_list)
#define MGR_NumKeyboardEntries(m)	\
			(((XmManagerRec *) (m))->manager.num_keyboard_entries)
#define MGR_SizeKeyboardList(m)		\
			(((XmManagerRec *) (m))->manager.size_keyboard_list)
#define MGR_ShadowThickness(m)		\
			(((XmManagerRec *) (m))->manager.shadow_thickness)


#define XmInheritTraversalChildrenProc ((XmTraversalChildrenProc) _XtInherit)
#define XmInheritObjectAtPointProc     ((XmObjectAtPointProc) _XtInherit)

typedef Boolean (*XmTraversalChildrenProc)( Widget, Widget **, Cardinal *) ;
typedef Widget (*XmObjectAtPointProc)(Widget, Position, Position) ;


/*  Structure used for storing accelerator and mnemonic information.  */

typedef struct 
{
   unsigned int eventType;
   KeySym       keysym;
   KeyCode      key;
   unsigned int modifiers;
   Widget       component;
   Boolean      needGrab;
   Boolean      isMnemonic;
} XmKeyboardData;


/*  The class definition  */

typedef struct {
    XtPointer next_extension;
    XrmQuark record_type;
    long version;
    Cardinal record_size;
    XmTraversalChildrenProc traversal_children ;
    XmObjectAtPointProc     object_at_point ;
} XmManagerClassExtRec, *XmManagerClassExt ;

#define XmManagerClassExtVersion 1L

typedef struct _XmManagerClassPart
{
   String               translations;
   XmSyntheticResource * syn_resources;   
   int                  num_syn_resources;   
   XmSyntheticResource * syn_constraint_resources;   
   int                  num_syn_constraint_resources;   
   XmParentProcessProc  parent_process;
   XtPointer            extension;
} XmManagerClassPart;

typedef struct _XmManagerClassRec
{
    CoreClassPart       core_class;
    CompositeClassPart  composite_class;
    ConstraintClassPart constraint_class;
    XmManagerClassPart  manager_class;
} XmManagerClassRec;

externalref XmManagerClassRec xmManagerClassRec;


/*  The instance definition  */

typedef struct _XmManagerPart
{
   Pixel   foreground;

   Dimension   shadow_thickness;
   Pixel   top_shadow_color;
   Pixmap  top_shadow_pixmap;
   Pixel   bottom_shadow_color;
   Pixmap  bottom_shadow_pixmap;

   Pixel   highlight_color;
   Pixmap  highlight_pixmap;

   XtCallbackList help_callback;
   XtPointer      user_data;

   Boolean traversal_on;
   unsigned char unit_type;
   XmNavigationType navigation_type;
   
   Boolean event_handler_added;
   Widget  active_child;
   Widget  highlighted_widget;
   Widget  accelerator_widget;

   Boolean has_focus;

   XmStringDirection string_direction;

   XmKeyboardData * keyboard_list;
   short num_keyboard_entries;
   short size_keyboard_list;

   XmGadget selected_gadget;
   XmGadget eligible_for_multi_button_event;

   GC      background_GC;
   GC      highlight_GC;
   GC      top_shadow_GC;
   GC      bottom_shadow_GC;

   Widget  initial_focus;

#ifndef XM_PART_BC
   XtCallbackList   popup_handler_callback;
#endif

} XmManagerPart;

typedef struct _XmManagerRec
{
   CorePart       core;
   CompositePart  composite;
   ConstraintPart constraint;
   XmManagerPart  manager;
} XmManagerRec;


/*  The constraint definition  */

typedef struct _XmManagerConstraintPart
{
   int unused;
} XmManagerConstraintPart;

typedef struct _XmManagerConstraintRec
{
   XmManagerConstraintPart manager;
} XmManagerConstraintRec, * XmManagerConstraintPtr;


#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmManagerP_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
