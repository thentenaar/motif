/* $XConsortium: CxxManager.h /main/4 1995/07/15 20:58:55 drk $ */
/* This may look like C code, but it is really -*- C++ -*- */
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
 * This is the header file for the C++ version of Manager widget.
 * It contains #ifdef __cplusplus clause to make it possible
 * to include it from a C source file.
 */

#ifndef _CxxManager_h
#define _CxxManager_h

#ifdef __cplusplus

#include <Xm/Xm.h>
#include <X11/IntrinsicP.h>
#include <Xm/ManagerP.h>


/**********************************
 * Widget class record definition *
 **********************************/

class XmCxxManagerClass {

    friend class XmCxxManager;

  public:
   
    XmCxxManagerClass(char*		name,
		      WidgetClass	parent,
		      Cardinal		widgetSize,
		      XtProc		class_init,
		      XtActionList	actions,
		      Cardinal		num_actions,
		      XtResourceList	resources,
		      Cardinal		num_resources,
		      XtInitProc	cxx_cast);

    /*
     * static class-methods
     * (must be public : passed as parameter to class_rec static constructor)
     */
    static void			ClassInitialize();

  protected:

    CoreClassPart		core_class;
    CompositeClassPart		composite_class;
    ConstraintClassPart		constraint_class;
    XmManagerClassPart		manager_class;

    XtPointer			cxx_manager_extension;

    // convenience function;
    void			SetBaseClassExtensionQuark();

    /*
     * virtual chained class-methods
     */
    virtual void		class_part_initialize();

    /*
     * virtual self contained class-methods
     */
    virtual void		class_part_initialize_prehook();
    virtual void		class_part_initialize_posthook();
    virtual Cardinal		sec_res_data(XmSecondaryResourceData **r_d);

  private:

    /*
     * static version of class-methods (calling virtual version)
     */
    static void			ClassPartInitialize(WidgetClass wc);
    static void			ClassPartInitializePrehook(WidgetClass wc);
    static void			ClassPartInitializePosthook(WidgetClass wc);
    static Cardinal		GetSecResData(WidgetClass wc,
					      XmSecondaryResourceData **r_d);
};


/***************************
 * Widget class definition *
 ***************************/

class XmCxxManager {

    friend class XmCxxManagerClass;

  public:

    CorePart		core;
    CompositePart	composite;
    ConstraintPart	constraint;
    XmManagerPart	manager;

  protected:

    /*
     * Constructor & destructor, operators new and delete.
     * These members are protected.
     * There were 2 options to create C++ and Xt objects :
     *	 a) new calls XtCreateWidget
     *      (the use of XtCreateWidget is forbiden for the application)
     * or
     *   b) XtCreateWidget calls indirectly new
     *      (the use of new is forbiden for the app)
     *
     * The first solution implies C++ programmation style but precludes the
     * creation of C++ widgets from C.
     * The second solution implies Xt programmation style. It allows a
     * C function to create C++ widgets.
     *
     * A widget created with `new' or created on the stack wouldn't be
     * properly initialized by Xt.
     * The application HAS to use XtDestroyWidget() and XtCreateWidget()
     * or any equivalent function e.g. XmCreateMyWidgetSubclass().
     */
    virtual ~XmCxxManager();
    XmCxxManager();

  public: // sgi/C++ does not want 'new' to be protected.
    void* operator new(size_t size, void* w = NULL);
  protected:
    void  operator delete(void*);

    /*
     * virtual chained class-methods
     */
    // core class part;
    virtual void		initialize(Widget req_w, ArgList, Cardinal*);
    virtual void		initialize_hook(ArgList, Cardinal*);
    virtual Boolean		set_values(Widget old_w, Widget req_w,
					   ArgList, Cardinal*);
    virtual Boolean		set_values_hook(ArgList, Cardinal*);
    virtual void		get_values_hook(ArgList, Cardinal*);

    // constraint class part;
    virtual void		constraint_initialize(Widget req_w,
						      Widget new_w,
						      ArgList, Cardinal*);
    virtual void		constraint_destroy(Widget w);
    virtual Boolean		constraint_set_values(Widget old_w,
						      Widget req_w,
						      Widget new_w,
						      ArgList, Cardinal*);
    /* constraint.extension->constraint_get_values_hook ???? */

    /*
     * virtual self contained class-methods
     */
    // core class part
    virtual void		realize(XtValueMask* mask,
					XSetWindowAttributes* win_att);
    virtual void		resize();
    virtual void		expose(XEvent* event, Region region);
    virtual Boolean		accept_focus(Time* t);
    virtual void		set_values_almost(Widget old_w,
						  XtWidgetGeometry* request,
						  XtWidgetGeometry* reply);
    virtual XtGeometryResult	query_geometry(XtWidgetGeometry* intended,
					       XtWidgetGeometry* reply);

    // base class extension part;
    virtual Boolean		set_values_prehook(Widget old_w, Widget req_w,
						   ArgList, Cardinal*);
    virtual void		initialize_posthook(Widget req_w,
						    ArgList, Cardinal*);
    virtual Boolean		set_values_posthook(Widget old_w,Widget req_w,
						    ArgList, Cardinal*);
    virtual void		secondary_object_create(Widget req_w,
							ArgList, Cardinal*);
    virtual void		get_values_prehook(ArgList, Cardinal*);
    virtual void		get_values_posthook(ArgList, Cardinal*);
    virtual XmNavigability	widget_navigable();
    virtual void		focus_change(XmFocusChange);
    virtual void		display_accelerator(String);

    // composite class part;
    virtual XtGeometryResult	geometry_manager(Widget child,
						 XtWidgetGeometry* intended,
						 XtWidgetGeometry* reply);
    virtual void		change_managed();
    virtual void		insert_child(Widget child);
    virtual void		delete_child(Widget child);

    // manager class part;
    virtual Boolean parent_process(XmParentProcessData ppdata);

    // manager class extension part;
    virtual Boolean traversal_children(Widget** children,
				       Cardinal* num_children);


  public:

    /*
     * static class-methods
     * (must be public : passed as parameter to class_rec static constructor)
     */
    static void			_MakeCxxWidget(Widget req, Widget new_w,
					       ArgList, Cardinal*);

  private:

    /*
     * static version of class-methods (calling virtual version)
     */
    // core class part;
    static void			Initialize(Widget req_w, Widget new_w,
					   ArgList, Cardinal*);
    static void 		InitializeHook(Widget w, ArgList, Cardinal*);
    static Boolean	 	SetValues(Widget old_w,
					  Widget req_w,
					  Widget new_w,
					  ArgList, Cardinal*);
    static Boolean 		SetValuesHook(Widget, ArgList, Cardinal*);
    static void 		GetValuesHook(Widget, ArgList, Cardinal*);
    
    static void			Realize(Widget, XtValueMask*,
					XSetWindowAttributes*);
    static void			Destroy(Widget);
    static void			Resize(Widget);
    static void			Redisplay(Widget, XEvent*, Region);
    static void			SetValuesAlmost(Widget, Widget,
						XtWidgetGeometry*,
						XtWidgetGeometry*);
    static Boolean		AcceptFocus(Widget, Time*);
    static void			DisplayAccelerator(Widget, String);
    static XtGeometryResult	QueryGeometry(Widget,
					      XtWidgetGeometry*,
					      XtWidgetGeometry*);

    // base class extension part;
    static Boolean		SetValuesPrehook(Widget old_w,
						 Widget req_w,
						 Widget new_w,
						 ArgList, Cardinal*);
    static void			InitializePosthook(Widget req_w,
						   Widget new_w,
						   ArgList, Cardinal*);
    static Boolean		SetValuesPosthook(Widget old_w,
						  Widget req_w,
						  Widget new_w,
						  ArgList, Cardinal*);
    static void			SecondaryObjectCreate(Widget req_w,
						      Widget new_w,
						      ArgList, Cardinal*);
    static void			GetValuesPrehook(Widget, ArgList, Cardinal*);
    static void			GetValuesPosthook(Widget, ArgList, Cardinal*);
    static XmNavigability	WidgetNavigable(Widget);
    static void			FocusChange(Widget, XmFocusChange);

    // composite class part;
    static XtGeometryResult	GeometryManager(Widget child,
						XtWidgetGeometry* request,
						XtWidgetGeometry* reply);
    static void			ChangeManaged(Widget w);
    static void			InsertChild(Widget child);
    static void			DeleteChild(Widget child);
    
    // constraint class part;
    static void			ConstraintInitialize(Widget req_w,
						     Widget new_w,
						     ArgList, Cardinal*);
    static void			ConstraintDestroy(Widget w);
    static Boolean		ConstraintSetValues(Widget old_w,
						    Widget req_w,
						    Widget new_w,
						    ArgList, Cardinal*);

    // constraint class extension part;
    /* static void ConstraintGetValuesHook(Widget w, ArgList, Cardinal*); */

    // manager class part;
    static Boolean		ParentProcess(Widget, XmParentProcessData);

    // manager class extension part;
    static Boolean		TraversalChildren(Widget, Widget**, Cardinal*);
};


/*
 * versions of g++ < 2.3.1 use a syntax with {} for placement.
 */
#ifndef IDLE_NEW
#  ifdef OLD_STYLE_NEW
#    define IDLE_NEW(ptr) new {ptr}
#  else
#    define IDLE_NEW(ptr) new(ptr)
#  endif /* ! OLD_STYLE_NEW */
#endif /* ! IDLE_NEW */

typedef class XmCxxManagerClass	*XmCxxManagerWidgetClass;
typedef class XmCxxManager	*XmCxxManagerWidget;

externalref XmCxxManagerClass	xmCxxManagerClassRec;

#endif /* not __cplusplus */

/* This symbol may be used from C code */
externalref WidgetClass		xmCxxManagerWidgetClass;

#ifndef XmIsCxxManager
#define XmIsCxxManager(w)	XtIsSubclass(w, xmCxxManagerWidgetClass)
#endif

#endif /* _CxxManager_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
