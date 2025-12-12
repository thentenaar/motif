/* $XConsortium: CxxString.h /main/4 1995/07/15 20:42:28 drk $ */
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
 * CxxString.h - Class definitions for CxxString
 */

#ifndef _ExmCxxString_h
#define _ExmCxxString_h

#ifdef __cplusplus

#include "./CxxSimple.h"
#include <Xm/AccTextT.h> /* header file for AccessTextual trait */


/**********************************
 * Widget class record definition *
 **********************************/

class ExmCxxStringClass : public ExmCxxSimpleClass {
  friend class ExmCxxString;
public:
  ExmCxxStringClass( char*		name,
		    WidgetClass	        parent,
		    Cardinal		widgetSize,
		    XtProc		class_init,
		    XtActionList	actions,
		    Cardinal		num_actions,
		    XtResourceList	resources,
		    Cardinal		num_resources,
		    XtInitProc		cxx_cast);
    
 // public static methods
  static  void      ClassInitialize();

protected:
  virtual void      class_part_initialize(); 
  unsigned char     default_render_table_type;
};

/* Define a new enumerated type. */
typedef enum { ExmCENTER_STRING, ExmSTART_STRING_LEFT_SIDE,
                     ExmSTART_STRING_RIGHT_SIDE} ExmHowAligned;


/* Define string equivalents of new resource names. */
#define ExmNcompoundString "compoundString"
#define ExmCCompoundString "CompoundString"


typedef struct _exmCxxStringPart
{
 /* Provide space for the five resources of ExmString. */
	XmString		compound_string;
	unsigned char		alignment;
        XmRenderTable           render_table;
        Boolean                 recompute_size;

        ExmHowAligned           text_starts_here; 
} ExmCxxStringPart;


class ExmCxxString : public ExmCxxSimple {
  friend class ExmCxxStringClass;
  friend class ExmCxxStringActions;
public:
  ExmCxxStringPart string;

  ~ExmCxxString();

  // core class part
  virtual void	    initialize(Widget req_w, ArgList, Cardinal*);
  virtual Boolean   set_values(Widget old_w, Widget req_w,
                               ArgList, Cardinal*);
  virtual void resize();
  virtual XtGeometryResult query_geometry(XtWidgetGeometry*, XtWidgetGeometry*);

  // override certain methods of primitive class extension record.
  virtual Boolean             widget_baseline(Dimension** dim, int* nb);
  virtual Boolean             widget_display_rect(XRectangle*);
    
  // override certain methods of simple class 
  virtual void draw_visual(void);
  virtual void create_gc(void);
  virtual void calc_visual_size(void);
  
  // provide methods for string class 
  virtual void AlignmentDirection(void);

  static void DefaultFont (Widget, int, XrmValue*);
  static void GetValuesCompoundString(Widget, int, XtArgVal *);

  static XtPointer StringGetValue(Widget, int);
  static void StringSetValue(Widget, XtPointer, int);
  static int StringPreferredFormat(Widget);

 public:
  // static class-methods: passed as parameter to class_rec static constructor.
  static void _MakeCxxWidget(Widget, Widget new_w, ArgList, Cardinal*);
};

typedef class ExmCxxStringClass     *ExmCxxStringWidgetClass;
typedef class ExmCxxString          *ExmCxxStringWidget;

externalref   ExmCxxStringClass      exmCxxStringClassRec;
#endif /* __cplusplus */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
extern Widget ExmCreateCxxString(Widget, char*, ArgList, Cardinal);
#ifdef __cplusplus
}
#endif /* __cplusplus */

externalref WidgetClass exmCxxStringWidgetClass;

#endif /* _ExmCxxString_h */
