/* $XConsortium: CxxSimple.h /main/4 1995/07/15 20:42:24 drk $ */
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

/*
 * CxxSimple.h - Class definitions for CxxSimple
 */

/* Ensure that the file be included only once. */
#ifndef _ExmCxxSimple_h
#define _ExmCxxSimple_h

/* Allow for compilation by a C++ compiler. */
#ifdef __cplusplus

// Include appropriate header files. 
#include <XmCxx/CxxPrimiti.h>
#include <Xm/ContItemT.h>   	// header file for XmQTcontainerItem trait
#include <Xm/ContainerT.h>  	// header file for XmQTcontainer trait
#include <Xm/CareVisualT.h> 	// header file for XmQTcareParentVisual trait

// Define the widget initialization class. 
class ExmCxxSimpleClass : public XmCxxPrimitiveClass {
  friend class ExmCxxSimple;
public:
 // define class constructor 
  ExmCxxSimpleClass( char*		name,
		    WidgetClass	        parent,
		    Cardinal		widgetSize,
		    XtProc		class_init,
		    XtActionList	actions,
		    Cardinal		num_actions,
		    XtResourceList	resources,
		    Cardinal		num_resources,
		    XtInitProc		cxx_cast);

  // virtual chained class methods
  virtual void		class_part_initialize();
    
  // define static class methods
  static void		ClassInitialize();
protected:
  XtPointer 		cxx_simple_extension;
};


// Define the widget instance part. 
typedef struct _exmCxxSimplePart
{
  	// Provide space for the three resources of ExmSimple.
	unsigned char		simple_shape;
	Dimension		margin_height;
        Dimension		margin_width;

 	// Provide space for the other protected fields of ExmSimple.
	// normal_gc holds a Graphics Context for rendering the vis
	// when the value of XmNsensitive is True.
	GC			normal_gc;
 	// insensitive_gc holds a Graphics Context for rendering the vis
	// when the value of XmNsensitive is False.
	GC			insensitive_gc;
	// pref_width holds an integral value representing the widget's 
        // current preferred width.
	Dimension		pref_width;
	// pref_height holds an integral value representing the widget's 
        // current preferred width.
	Dimension		pref_height;
	// need_to_compute_width is a flag.  If its value is True, 
        // then the widget needs to renegotiate its width.
	Boolean			need_to_compute_width;
	// need_to_compute_height is a flag.  If its value is True, 
        // then the widget needs to renegotiate its height.
	Boolean			need_to_compute_height;
	// visual is an XRectangle value representing the bounding box of the
        // widget's visual.
	XRectangle		visual;
	// need_to_reconfigure is a flag.  If its value is True, 
        // then the widget needs to call the reconfigure method.
	Boolean			need_to_reconfigure;
	// saved_foreground is used for the management of the selected state
	Pixel			saved_foreground;
} ExmCxxSimplePart;

// Define the widget class. 
class ExmCxxSimple : public XmCxxPrimitive {
  friend class ExmCxxSimpleClass;
  friend class ExmCxxSimpleActions;
public:
  ExmCxxSimplePart simple;

  ~ExmCxxSimple();

 // core class methods provided by this widget.
  virtual void	    initialize(Widget req_w, ArgList, Cardinal*);
  virtual Boolean   set_values(Widget old_w, Widget req_w,
                               ArgList, Cardinal*);
  virtual void	    realize(XtValueMask* mask,
			    XSetWindowAttributes* win_att);
  virtual void 	    resize();
  virtual void      expose(XEvent*, Region);
  virtual XtGeometryResult query_geometry(XtWidgetGeometry*,
					  XtWidgetGeometry*);

  // primitive class methods provided by this widget.
  // none 

  // primitive class extension methods provided by this widget.
  virtual Boolean widget_display_rect(XRectangle*);
    
  // simple class methods
  virtual void draw_visual(void);
  virtual void draw_shadow(void);
  virtual void create_gc(void);
  virtual void destroy_gc(void);
  virtual GC   select_gc(void);
  virtual void calc_visual_size(void);
  virtual void calc_widget_size(void);
  virtual void reconfigure(WidgetClass, Widget);

  // define trait static class methods
  static void 		ContItemSetValues(Widget w, 
					  XmContainerItemData contItemData);
  static void		ContItemGetValues(Widget w, 
					  XmContainerItemData contItemData);
  static Boolean	HandleRedraw (Widget kid, 
				      Widget cur_parent,
				      Widget new_parent,
				      Mask visual_flag);
  static void		SetSelectedVisual (Widget wid) ;
  
  // static class-methods: passed as parameter to class_rec static constructor.
  static void  _MakeCxxWidget(Widget, Widget new_w, ArgList, Cardinal*);
};

// Define the WidgetClass and the Widget.
typedef class ExmCxxSimpleClass     *ExmCxxSimpleWidgetClass;
typedef class ExmCxxSimple          *ExmCxxSimpleWidget;

externalref   ExmCxxSimpleClass      exmCxxSimpleClassRec;

// Define an IsSubclass macro. 
#ifndef XmIsCxxSimple
#define XmIsCxxSimple(w)          XtIsSubclass(w, exmCxxSimpleWidgetClass)
#endif  /* IsSubclass macro. */

#endif /* __cplusplus */

/* Define symbols that may be accessed from a C application. */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
extern Widget ExmCreateCxxSimple(Widget, char*, ArgList, Cardinal);
#ifdef __cplusplus
}
#endif /* __cplusplus */

externalref WidgetClass exmCxxSimpleWidgetClass;

// Define string equivalents of new resource names.
#define ExmNsimpleShape "simpleShape"
#define ExmCSimpleShape "SimpleShape"
#define ExmRSimpleShape "SimpleShape"

// Specify new enumerated constants for resources. 
enum { ExmSHAPE_OVAL=0, ExmSHAPE_RECTANGLE=1 };

#endif /* _ExmCxxSimple_h */
