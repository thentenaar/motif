/* $XConsortium: CxxManager.cxx /main/4 1995/07/17 11:45:46 drk $ */
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

#include <XmCxx/CxxManager.h>


/*************
 * Resources * -- NONE
 *************/

/***********************
 * Action Declarations * -- NONE
 ***********************/

/****************
 * Translations * -- NONE
 ****************/

/*************************************
 * Declare internal widget functions * -- NONE
 *************************************/

/************************************
 * Class declaration for CxxManager * 
 ************************************/

externaldef (xmcxxmanagerclassrec)
XmCxxManagerClass xmCxxManagerClassRec("XmCxxManager",
				       (WidgetClass) &xmManagerClassRec,
				       sizeof(XmCxxManager),
				       XmCxxManagerClass::ClassInitialize,
				       NULL		/* action */,
				       0		/* num_action */,
				       NULL		/* resources */,
				       0		/* num_resources */,
				       XmCxxManager::_MakeCxxWidget);


externaldef(xmcxxmanagerwidgetclass)
WidgetClass xmCxxManagerWidgetClass = (WidgetClass) &xmCxxManagerClassRec;


/*
 * 'Class record' constructor
 * Some entries *MUST* be overriden by subclass constructors.
 *	They either cannot be inherited (ex: class_name, superclass...)
 *	or are inherited through and Intrinsics mechanism (ex: resources,
 *	constraints). Most of these entries are the constructor parameters so
 *	that subclasses do not manipulate the structure fields.
 *   -  _MakeCxxWidget must be redefined by each class and must call 'new'
 *	for its widget instance.
 *   -  synthetic resources (unless NULL) may not be inherited
 *	(resource chaining is automatically done by Motif)
 *
 *   -  TO BE CONTINUED...
 *
 * Some entries *MAY NOT* be overriden by subclass constructors :
 *   -	all fields containing containing a pointer to a function
 *	(i.e. Xt or Xm class method) must not be modified by subclasses.
 *	The sublasses should redifined the corresponding virtual method
 *	instead. Exception : the ClassInitialize method must be explicitely
 *	given as a parameter to the constructor.
 *   -  core_class.destroy (subclass destructors should be used instead)
 *   -  basically all function slots corresponding to a virtual function
 *	(subclasses should implement their own virtual function instead)
 *	
 *   -  TO BE CONTINUED...
 *
 * Some entries may be overriden by subclass constructors :
 */
XmCxxManagerClass::XmCxxManagerClass(char*		name,
				     WidgetClass	parentClass,
				     Cardinal		widgetSize,
				     XtProc		class_init,
				     XtActionList	actions,
				     Cardinal		num_actions,
				     XtResourceList	resources,
				     Cardinal		num_resources,
				     XtInitProc		cxx_cast) {
    
    XmBaseClassExt core_ext = 
	(XmBaseClassExt) XtMalloc(sizeof(XmBaseClassExtRec));
    
    /*
     * Passed parameter initialization
     */
    core_class.class_name		= name;
    core_class.superclass		= parentClass;
    core_class.widget_size		= widgetSize;
    core_class.class_initialize		= class_init;
    core_class.actions			= actions;
    core_class.num_actions		= num_actions;
    core_class.resources		= resources;
    core_class.num_resources		= num_resources;
    core_ext->initializePrehook		= cxx_cast;


    /*
     * Core part data
     */
    core_class.class_inited		= FALSE;
    core_class.xrm_class		= NULLQUARK;
    core_class.compress_motion		= FALSE;
    core_class.compress_exposure	= XtExposeNoCompress;
    core_class.compress_enterleave	= FALSE;
    core_class.visible_interest		= FALSE;
    core_class.version			= XtVersion;
    core_class.callback_private		= NULL;
    core_class.tm_table			= XtInheritTranslations;
    core_class.extension		= core_ext;
	
    /*
     * Core part class-methods
     * Subclasses : DO NOT OVERRIDE THESE ENTRIES !!
     */
    // Chained methods must be NULL for all classes but CxxManagerClass;
    if (this == &xmCxxManagerClassRec) {	
	core_class.class_part_initialize=
	  		XmCxxManagerClass::ClassPartInitialize;
	core_class.initialize		= XmCxxManager::Initialize;
	core_class.initialize_hook	= XmCxxManager::InitializeHook;
	core_class.set_values		= XmCxxManager::SetValues;
	core_class.set_values_hook	= XmCxxManager::SetValuesHook;
	core_class.get_values_hook	= XmCxxManager::GetValuesHook;
	core_class.destroy		= XmCxxManager::Destroy;
    }
    else {
	core_class.class_part_initialize= (XtWidgetClassProc)NULL;
	core_class.initialize		= (XtInitProc)NULL;
	core_class.initialize_hook	= (XtArgsProc)NULL;
	core_class.set_values		= (XtSetValuesFunc)NULL;
	core_class.set_values_hook	= (XtArgsFunc)NULL;
	core_class.get_values_hook	= (XtArgsProc)NULL;
	core_class.destroy		= (XtWidgetProc)NULL;
    }
    // self contained methods must be set to these values for all classes;
    core_class.realize			= XmCxxManager::Realize;
    core_class.resize			= XmCxxManager::Resize;
    core_class.expose			= XmCxxManager::Redisplay;
    core_class.set_values_almost	= XmCxxManager::SetValuesAlmost;
    core_class.accept_focus		= XmCxxManager::AcceptFocus;
    core_class.query_geometry		= XmCxxManager::QueryGeometry;
    core_class.display_accelerator	= XmCxxManager::DisplayAccelerator;

    
    /*
     * Base class extension data
     */
    core_ext->next_extension		= NULL;
    core_ext->record_type		= NULLQUARK;
    core_ext->version			= XmBaseClassExtVersion;
    core_ext->record_size		= sizeof(XmBaseClassExtRec);
    core_ext->secondaryObjectClass	= XmInheritClass;
    // Initialize char flags[32] to 0;
    int nb = sizeof(core_ext->flags) / sizeof(char);
    for (int i = 0; i < nb; core_ext->flags[i++] = 0);

    core_ext->ext_resources		= NULL;
    core_ext->compiled_ext_resources	= NULL;
    core_ext->num_ext_resources		= 0;
    core_ext->use_sub_resources		= FALSE;
    core_ext->wrapperData		= NULL;

    /*
     * Base class extension class-methods
     * Subclasses : DO NOT OVERRIDE THESE ENTRIES !
     */
    // Self contained methods;
    core_ext->setValuesPrehook		= XmCxxManager::SetValuesPrehook;
    core_ext->initializePosthook	= XmCxxManager::InitializePosthook;
    core_ext->setValuesPosthook		= XmCxxManager::SetValuesPosthook;
    core_ext->secondaryObjectCreate	= XmCxxManager::SecondaryObjectCreate;
    core_ext->getSecResData		= XmCxxManagerClass::GetSecResData;
    core_ext->getValuesPrehook		= XmCxxManager::GetValuesPrehook;
    core_ext->getValuesPosthook		= XmCxxManager::GetValuesPosthook;
    core_ext->classPartInitPrehook	=
			XmCxxManagerClass::ClassPartInitializePrehook;
    core_ext->classPartInitPosthook	=
			XmCxxManagerClass::ClassPartInitializePosthook;
    core_ext->widgetNavigable		= XmCxxManager::WidgetNavigable;
    core_ext->focusChange		= XmCxxManager::FocusChange;


    /*
     * Composite part data
     */
    composite_class.extension		= NULL;

    /*
     * Composite part class methods
     * Subclasses : DO NOT OVERRIDE THESE ENTRIES !
     */
    // Self contained methods;
    composite_class.geometry_manager	= XmCxxManager::GeometryManager;
    composite_class.change_managed	= XmCxxManager::ChangeManaged;
    composite_class.insert_child	= XmCxxManager::InsertChild;
    composite_class.delete_child	= XmCxxManager::DeleteChild;

    
    /*
     * Constraint part data :
     * .resources array and .num_resources should only be set for the class
     * implementing them and should not be set in subclasses (Xt job).
     * On the other hand, .constraint_size MUST be set for subclasses, even
     * when they do not define new constraints.
     */
    constraint_class.resources		= NULL;
    constraint_class.num_resources	= 0;
    constraint_class.constraint_size	= 0;
    constraint_class.extension		= NULL;

    /*
     * Constraint part class-methods
     * Subclasses : DO NOT OVERRIDE THESE ENTRIES !
     */
    // Chained methods must be set to null for all classes but CxxManager;
    if (this == &xmCxxManagerClassRec) {	
	constraint_class.initialize = XmCxxManager::ConstraintInitialize;
	constraint_class.destroy    = XmCxxManager::ConstraintDestroy;
	constraint_class.set_values = XmCxxManager::ConstraintSetValues;
    }
    else {
	constraint_class.initialize	= (XtInitProc)NULL;
	constraint_class.destroy	= (XtWidgetProc)NULL;
	constraint_class.set_values	= (XtSetValuesFunc)NULL;
    }

    /*
     * Manager part data
     */
    XmManagerClassExt mgr_ext =
	(XmManagerClassExt) XtMalloc(sizeof(XmManagerClassExtRec));

    manager_class.translations			= NULL;
    manager_class.syn_resources			= NULL;
    manager_class.num_syn_resources		= 0;
    manager_class.syn_constraint_resources	= NULL;
    manager_class.num_syn_constraint_resources	= 0;
    manager_class.extension			= mgr_ext;
    
    /*
     * Manager part class-methods
     * Subclasses : DO NOT OVERRIDE THESE ENTRIES !
     */
    manager_class.parent_process	= XmCxxManager::ParentProcess;


    /*
     * Manager extension part data;
     */
    mgr_ext->next_extension	= NULL;
    mgr_ext->record_type	= NULLQUARK;
    mgr_ext->version		= XmManagerClassExtVersion,
    mgr_ext->record_size	= sizeof(XmManagerClassExtRec);

    /*
     * Manager extension part class-methods
     * Subclasses : DO NOT OVERRIDE THESE ENTRIES;
     */
    mgr_ext->traversal_children	= XmCxxManager::TraversalChildren;

    /*
     * CxxManager part
     */
    cxx_manager_extension	= NULL;
}



/****************************
 * constructor & destructor *
 ****************************/

XmCxxManager::XmCxxManager() {}
XmCxxManager::~XmCxxManager() {}

/*
 * Each widget is allocated by Xt but we still have to call 'new' to initialize
 * the virtual function table.
 * This is why we override 'new' so it does not perform any allocation.
 * For some reason, we cannot just redefine 'new' with an extra parameter
 * without redefining the default version of 'new', so let us do both at
 * the same time.
 */
void* XmCxxManager::operator new(size_t, void* w /* = NULL */) {
    return w;
}

/*
 * Each widget is freed by Xt but we still have to call 'delete' for the
 * destructors to be called.
 * This is why we override 'delete' so it does not free any memory.
 */
void XmCxxManager::operator delete(void*) {
}

    
/*************************
 * convenience functions *
 *************************/


void XmCxxManagerClass::SetBaseClassExtensionQuark() {
    /*
     * Set base_class extension record type. 
     * This cannot be done staticaly (quarks are not initialized then).
     * Make sure to get the right extension record...
     */
    XmBaseClassExt ext = (XmBaseClassExt) core_class.extension;
    while (ext) {
	if (ext->record_type == NULLQUARK &&
	    ext->record_size == sizeof(XmBaseClassExtRec) &&
	    ext->setValuesPrehook == XmCxxManager::SetValuesPrehook) {
	    ext->record_type = XmQmotif;
	    return;
	}
	ext = (XmBaseClassExt) ext->next_extension;
    }
}



			/****************************
			 *                          *
			 * Intrinsics class methods *
			 *                          *
			 ****************************/



/*********************************************************************
 *
 * ClassInitialize
 *
 ********************************************************************/         
void XmCxxManagerClass::ClassInitialize() {
    /* initialize base_class extension quark */
    xmCxxManagerClassRec.SetBaseClassExtensionQuark();
}



/************************************************************************
 *
 *  _MakeCxxWidget
 *	Call `new' to make each widget a C++ object
 *
 ************************************************************************/
void XmCxxManager::_MakeCxxWidget(Widget /*req_w*/, Widget new_w,
				  ArgList, Cardinal*) {
    (void) IDLE_NEW(new_w) XmCxxManager;
}


void XmCxxManager::Destroy(Widget w) {
    // Upward chained;
    // call overloaded operator delete : no free, just call all destructors;
    delete (XmCxxManagerWidget) w; 
}


/*
 *   The following functions go by pair :
 *         one static member, capitalized
 *         one virtual member, in lower case
 *   e.g. SetValues and set_values
 *
 *   Each static member do nothing else than calling the corresponding
 *   virtual member.
 *   The static members are only defined for CxxManager and are stored
 *   in each class record, or for some of them (chained methods) only in
 *   the CxxManagerClass instance.
 *   
 *   Instead of overriding the function slot in the class record,
 *   new subclasses should (must) implement their version of the
 *   corresponding virtual function.
 *   For the methods usually chained by the intrinsics, it is the
 *   widget programmer's responsability to call its superclass virtual method
 *   in its own version of the method.
 */



/************************************************************************
 *
 *  class_part_initialize : virtual
 *     Downward chained Xt method :
 *     each class re-implementing class_part_initialize() should start by 
 *     calling <superclass>::class_part_initialize()
 *
 ************************************************************************/
void XmCxxManagerClass::class_part_initialize() {
}

void XmCxxManagerClass::ClassPartInitialize(WidgetClass wc) {
    // just call the widgetclass virtual function;
    ((XmCxxManagerWidgetClass) wc)->class_part_initialize();
}



/************************************************************************
 *
 *  initialize : virtual
 *     Downward chained Xt method :
 *     each class re-implementing initialize() should start by
 *     calling <superclass>::initialize()
 *
 ************************************************************************/
void XmCxxManager::initialize(Widget, ArgList, Cardinal*) {
}

void XmCxxManager::Initialize(Widget req_w, Widget new_w,
			      ArgList args, Cardinal* nargs) {
    // just call the widget virtual function;
    ((XmCxxManagerWidget) new_w)->initialize(req_w, args, nargs);
}



/************************************************************************
 *
 *  initialize_hook : virtual
 *     Downward chained Xt method :
 *     each class re-implementing initialize_hook() should start by 
 *     calling <superclass>::initialize_hook()
 *
 ************************************************************************/
void XmCxxManager::initialize_hook(ArgList, Cardinal*) {
}

void XmCxxManager::InitializeHook(Widget w, ArgList args, Cardinal* nargs) {
    // just call the widget virtual function;
    ((XmCxxManagerWidget) w)->initialize_hook(args, nargs);
}



/************************************************************************
 *
 *  set_values : virtual
 *     Downward chained Xt method :
 *     each class re-implementing set_values() should start by
 *     calling <superclass>::set_values()
 *
 ************************************************************************/
Boolean XmCxxManager::set_values(Widget, Widget, ArgList, Cardinal*) {
    return FALSE;
}

Boolean XmCxxManager::SetValues(Widget old_w, Widget req_w, Widget new_w,
				ArgList args, Cardinal* nargs) {
    // just call the widget virtual function;
    return ((XmCxxManagerWidget) new_w)->set_values(old_w, req_w, args, nargs);
}



/************************************************************************
 *
 *  set_values_hook : virtual
 *     Downward chained Xt method :
 *     each class re-implementing set_values_hook() should start by
 *     calling <superclass>::set_values_hook()
 *
 ************************************************************************/
Boolean XmCxxManager::set_values_hook(ArgList, Cardinal*) {
    return FALSE;
}

Boolean XmCxxManager::SetValuesHook(Widget w, ArgList args, Cardinal* nargs) {
    // just call the widget virtual function;
    return ((XmCxxManagerWidget) w)->set_values_hook(args, nargs);
}



/************************************************************************
 *
 *  get_values_hook : virtual
 *     Downward chained Xt method :
 *     each class re-implementing get_values_hook() should start by
 *     calling <superclass>::get_values_hook()
 *
 ************************************************************************/
void XmCxxManager::get_values_hook(ArgList, Cardinal*) {
}

void XmCxxManager::GetValuesHook(Widget w, ArgList args, Cardinal* nargs) {
    // just call the widget virtual function;
    ((XmCxxManagerWidget) w)->get_values_hook(args, nargs);
}



/************************************************************************
 *
 *  constraint_initialize : virtual
 *     Downward chained Xt method :
 *     each class re-implementing constraint_initialize() should start by
 *     calling <superclass>::constraint_initialize()
 *
 ************************************************************************/
void XmCxxManager::constraint_initialize(Widget, Widget, ArgList, Cardinal*) {
}

void XmCxxManager::ConstraintInitialize(Widget req_w, Widget new_w,
					ArgList args, Cardinal* nargs) {
    // just call the widget virtual function;
    ((XmCxxManagerWidget) XtParent(new_w))->
	constraint_initialize(req_w, new_w, args, nargs);
}



/************************************************************************
 *
 *  constraint_destroy : virtual
 *     Downward chained Xt method :
 *     each class re-implementing constraint_destroy() should start by
 *     calling <superclass>::constraint_destroy()
 *
 ************************************************************************/
void XmCxxManager::constraint_destroy(Widget) {
}

void XmCxxManager::ConstraintDestroy(Widget w) {
    // just call the widget virtual function;
    ((XmCxxManagerWidget) XtParent(w))->constraint_destroy(w);
}



/************************************************************************
 *
 *  constraint_set_values : virtual
 *     Downward chained Xt method :
 *     each class re-implementing constraint_set_values() should start by
 *     calling <superclass>::constraint_set_values()
 *
 ************************************************************************/
Boolean XmCxxManager::constraint_set_values(Widget, Widget, Widget,
					    ArgList, Cardinal*) {
    return FALSE;
}

Boolean XmCxxManager::ConstraintSetValues(Widget old_w, Widget req_w,
					  Widget new_w,
					  ArgList args, Cardinal* nargs) {
    // just call the widget virtual function;
    return ((XmCxxManagerWidget) XtParent(new_w))->
	constraint_set_values(old_w, req_w, new_w, args, nargs);
}




/************************************************************************
 *
 *  realize : virtual
 *     self contained Xt method
 *
 ************************************************************************/
void XmCxxManager::realize(XtValueMask* mask, XSetWindowAttributes* win_att) {
    XtRealizeProc proc = xmManagerWidgetClass->core_class.realize;
    // if proc == NULL, error : Realize method cannot be NULL;
    proc((Widget) this, mask, win_att);
}

void XmCxxManager::Realize(Widget w, XtValueMask* mask,
			   XSetWindowAttributes* win_att) {
    ((XmCxxManagerWidget) w)->realize(mask, win_att);
}



/************************************************************************
 *
 *  resize : virtual
 *     self contained Xt method
 *
 ************************************************************************/
void XmCxxManager::resize() {
    XtWidgetProc proc = xmManagerWidgetClass->core_class.resize;
    if (proc)
	proc((Widget) this);
}

void XmCxxManager::Resize(Widget w) {
    ((XmCxxManagerWidget) w)->resize();
}



/************************************************************************
 *
 *  expose - redisplay : virtual
 *     self contained Xt method
 *
 ************************************************************************/
void XmCxxManager::expose(XEvent* event, Region region) {
    XtExposeProc proc = xmManagerWidgetClass->core_class.expose;
    if (proc)
	proc((Widget) this, event, region);
}

void XmCxxManager::Redisplay(Widget w, XEvent *event, Region region) {
    ((XmCxxManagerWidget) w)->expose(event, region);
}



/************************************************************************
 *
 *  accept_focus : virtual
 *     self contained Xt method
 *
 ************************************************************************/
Boolean XmCxxManager::accept_focus(Time* t) {
    XtAcceptFocusProc func = xmManagerWidgetClass->core_class.accept_focus;
    return (func ? func((Widget) this, t) : FALSE);
}

Boolean XmCxxManager::AcceptFocus(Widget w, Time* t) {
    return ((XmCxxManagerWidget) w)->accept_focus(t);
}
    


/************************************************************************
 *
 *  set_values_almost : virtual
 *     self contained Xt method
 *
 ************************************************************************/
void XmCxxManager::set_values_almost(Widget old_w,
				     XtWidgetGeometry* request,
				     XtWidgetGeometry* reply) {
    XtAlmostProc proc = xmManagerWidgetClass->core_class.set_values_almost;
    // if proc == NULL, error : SetValuesAlmost method cannot be NULL;
    proc(old_w, (Widget) this, request, reply);
}

void XmCxxManager::SetValuesAlmost(Widget old_w, Widget new_w,
				   XtWidgetGeometry* request,
				   XtWidgetGeometry* reply) {
    ((XmCxxManagerWidget) new_w)->set_values_almost(old_w, request, reply);
}



/************************************************************************
 *
 *  query_geometry : virtual
 *     self contained Xt method
 *
 ************************************************************************/
XtGeometryResult XmCxxManager::query_geometry(XtWidgetGeometry *intended,
					      XtWidgetGeometry *reply) {
    XtGeometryHandler func = xmManagerWidgetClass->core_class.query_geometry;
    /*
     * return (func ? func((Widget) this, intended, reply) : XtGeometryYes);
     * does not compile on sgi/C++.
     */
    if (func)
	return func((Widget) this, intended, reply);
    return XtGeometryYes;
}

XtGeometryResult XmCxxManager::QueryGeometry(Widget w,
					     XtWidgetGeometry *intended,
					     XtWidgetGeometry *reply) {
    return ((XmCxxManagerWidget) w)->query_geometry(intended, reply);
}



/************************************************************************
 *
 *  display_accelerator : virtual
 *     self contained Xt method
 *
 ************************************************************************/
void XmCxxManager::display_accelerator(String str) {
    XtStringProc proc = xmManagerWidgetClass->core_class.display_accelerator;
    if (proc)
	proc((Widget) this, str);
}

void XmCxxManager::DisplayAccelerator(Widget w, String str) {
    ((XmCxxManagerWidget) w)->display_accelerator(str);
}



/************************************************************************
 *
 *  class_part_initialize_prehook : virtual
 *     self contained method
 *
 ************************************************************************/
void XmCxxManagerClass::class_part_initialize_prehook() {
}

void XmCxxManagerClass::ClassPartInitializePrehook(WidgetClass wc) {
    ((XmCxxManagerClass*) wc)->class_part_initialize_prehook();
}



/************************************************************************
 *
 *  class_part_initialize_posthook : virtual
 *     self contained method
 *
 ************************************************************************/
void XmCxxManagerClass::class_part_initialize_posthook() {
}

void XmCxxManagerClass::ClassPartInitializePosthook(WidgetClass wc) {
    ((XmCxxManagerClass*) wc)->class_part_initialize_posthook();
}



/************************************************************************
 *
 *  sec_res_data : virtual
 *     self contained method
 *
 ************************************************************************/
Cardinal XmCxxManagerClass::sec_res_data(XmSecondaryResourceData **r_d) {
    XmGetSecResDataFunc func =
	((XmBaseClassExt) xmManagerWidgetClass->core_class.extension)->
	    getSecResData;
    return (func ? func((WidgetClass) this, r_d) : 0);
}

Cardinal XmCxxManagerClass::GetSecResData(WidgetClass wc,
					  XmSecondaryResourceData **r_d) {
    return ((XmCxxManagerClass*) wc)->sec_res_data(r_d);
}



/************************************************************************
 *
 *  set_values_prehook : virtual
 *     self contained method
 *
 ************************************************************************/
Boolean XmCxxManager::set_values_prehook(Widget old_w, Widget req_w,
					 ArgList args, Cardinal* nargs){
    XtSetValuesFunc func =
	((XmBaseClassExt) xmManagerWidgetClass->core_class.extension)->
	    setValuesPrehook;
    return (func ? func(old_w, req_w, (Widget) this, args, nargs) : FALSE);
}

Boolean XmCxxManager::SetValuesPrehook(Widget old_w, Widget req_w,
				       Widget new_w,
				       ArgList args, Cardinal* nargs) {
    return ((XmCxxManagerWidget) new_w)->
	set_values_prehook(old_w, req_w, args, nargs);
}



/************************************************************************
 *
 *  initialize_posthook : virtual
 *     self contained method
 *
 ************************************************************************/
void XmCxxManager::initialize_posthook(Widget req_w,
				       ArgList args, Cardinal* nargs) {
    XtInitProc proc =
	((XmBaseClassExt) xmManagerWidgetClass->core_class.extension)->
	    initializePosthook;
    if (proc)
	proc(req_w, (Widget) this, args, nargs);
}

void XmCxxManager::InitializePosthook(Widget req_w, Widget new_w,
				      ArgList args, Cardinal* nargs) {
    ((XmCxxManagerWidget)new_w)->initialize_posthook(req_w, args, nargs);
}



/************************************************************************
 *
 *  set_values_posthook : virtual
 *     self contained method
 *
 ************************************************************************/
Boolean XmCxxManager::set_values_posthook(Widget old_w, Widget req_w,
					  ArgList args, Cardinal* nargs) {
    XtSetValuesFunc func =
	((XmBaseClassExt) xmManagerWidgetClass->core_class.extension)->
	    setValuesPosthook;
    return (func ? func(old_w, req_w, (Widget) this, args, nargs) : FALSE);
}

Boolean XmCxxManager::SetValuesPosthook(Widget old_w, Widget req_w,
					Widget new_w,
					ArgList args, Cardinal* nargs) {
    return ((XmCxxManagerWidget) new_w)->
	set_values_posthook(old_w, req_w, args, nargs);
}



/************************************************************************
 *
 *  secondary_object_create : virtual
 *     self contained method
 *
 ************************************************************************/
void XmCxxManager::secondary_object_create(Widget req_w,
					   ArgList args, Cardinal* nargs) {
    XtInitProc proc =
	((XmBaseClassExt) xmManagerWidgetClass->core_class.extension)->
	    secondaryObjectCreate;
    if (proc)
	proc(req_w, (Widget) this, args, nargs);
}

void XmCxxManager::SecondaryObjectCreate(Widget req_w, Widget new_w,
					 ArgList args, Cardinal* nargs) {
    ((XmCxxManagerWidget) new_w)->
	secondary_object_create(req_w, args, nargs);
}



/************************************************************************
 *
 *  get_values_prehook : virtual
 *     self contained method
 *
 ************************************************************************/
void XmCxxManager::get_values_prehook(ArgList args, Cardinal* nargs) {
    XtArgsProc proc =
	((XmBaseClassExt) xmManagerWidgetClass->core_class.extension)->
	    getValuesPrehook;
    if (proc)
	proc((Widget) this, args, nargs);
}

void XmCxxManager::GetValuesPrehook(Widget w, ArgList args, Cardinal* nargs) {
    ((XmCxxManagerWidget) w)->get_values_prehook(args, nargs);
}



/************************************************************************
 *
 *  get_values_posthook : virtual
 *     self contained method
 *
 ************************************************************************/
void XmCxxManager::get_values_posthook(ArgList args, Cardinal* nargs) {
    XtArgsProc proc =
	((XmBaseClassExt) xmManagerWidgetClass->core_class.extension)->
	    getValuesPosthook;
    if (proc)
	proc((Widget) this, args, nargs);
}

void XmCxxManager::GetValuesPosthook(Widget w, ArgList args, Cardinal* nargs) {
    ((XmCxxManagerWidget) w)->get_values_posthook(args, nargs);
}



/************************************************************************
 *
 *  widget_navigable : virtual
 *     self contained method
 *
 ************************************************************************/
XmNavigability XmCxxManager::widget_navigable() {
    XmWidgetNavigableProc func =
	((XmBaseClassExt) xmManagerWidgetClass->core_class.extension)->
	    widgetNavigable;
    /*
     * return (func ? func((Widget) this) : XmNOT_NAVIGABLE);
     * does not compile on sgi/C++.
     */
    if (func)
	return func((Widget) this);
    return XmNOT_NAVIGABLE;
}

XmNavigability XmCxxManager::WidgetNavigable(Widget w) {
    return ((XmCxxManagerWidget) w)->widget_navigable();
}



/************************************************************************
 *
 *  focus_change : virtual
 *     self contained method
 *
 ************************************************************************/
void XmCxxManager::focus_change(XmFocusChange change) {
    XmFocusChangeProc proc =
	((XmBaseClassExt) xmManagerWidgetClass->core_class.extension)->
	    focusChange;
    if (proc)
	proc((Widget) this, change);
}

void XmCxxManager::FocusChange(Widget w, XmFocusChange change) {
    ((XmCxxManagerWidget) w)->focus_change(change);
}



/************************************************************************
 *
 *  geometry_manager : virtual
 *     self contained Xt method
 *
 ************************************************************************/
XtGeometryResult XmCxxManager::geometry_manager(Widget child,
						XtWidgetGeometry* intended,
						XtWidgetGeometry* reply) {
    // should do nothing, just like XmManager;
    XtGeometryHandler func = ((XmManagerWidgetClass) xmManagerWidgetClass)->
	composite_class.geometry_manager;
    // if func == NULL, error : GeometryManager method cannot be NULL;
    return func(child, intended, reply);
}

XtGeometryResult XmCxxManager::GeometryManager(Widget child,
					       XtWidgetGeometry* request,
					       XtWidgetGeometry* reply) {
    return ((XmCxxManagerWidget) child->core.parent)->
	geometry_manager(child, request, reply);
}



/************************************************************************
 *
 *  change_managed : virtual
 *     self contained Xt method
 *
 ************************************************************************/
void XmCxxManager::change_managed() {
    XtWidgetProc proc = ((XmManagerWidgetClass) xmManagerWidgetClass)->
	composite_class.change_managed;
    // if proc == NULL, error : ChangeManaged method cannot be NULL;
    proc((Widget) this);
}

void XmCxxManager::ChangeManaged(Widget w) {
    ((XmCxxManagerWidget) w)->change_managed();
}



/************************************************************************
 *
 *  insert_child : virtual
 *     self contained Xt method
 *
 ************************************************************************/
void XmCxxManager::insert_child(Widget child) {
    XtWidgetProc proc = ((XmManagerWidgetClass) xmManagerWidgetClass)->
	composite_class.insert_child;
    // if proc == NULL, error : InsertChild method cannot be NULL;
    proc(child);
}

void XmCxxManager::InsertChild(Widget child) {
    ((XmCxxManagerWidget) child->core.parent)->insert_child(child);
}



/************************************************************************
 *
 *  delete_child : virtual
 *     self contained Xt method
 *
 ************************************************************************/
void XmCxxManager::delete_child(Widget child) {
    XtWidgetProc proc = ((XmManagerWidgetClass) xmManagerWidgetClass)->
	composite_class.delete_child;
    // if proc == NULL, error : DeleteChild method cannot be NULL;
    proc(child);
}

void XmCxxManager::DeleteChild(Widget child) {
    ((XmCxxManagerWidget) child->core.parent)->delete_child(child);
}



/************************************************************************
 *
 *  parent_process : virtual
 *     self contained method
 *
 ************************************************************************/
Boolean XmCxxManager::parent_process(XmParentProcessData ppdata) {
    XmParentProcessProc func = ((XmManagerWidgetClass) xmManagerWidgetClass)->
	manager_class.parent_process;
    return (func ? func((Widget) this, ppdata) : FALSE);
}

Boolean XmCxxManager::ParentProcess(Widget w, XmParentProcessData ppdata) {
    return ((XmCxxManagerWidget) w)->parent_process(ppdata);
}



/************************************************************************
 *
 *  traversal_children : virtual
 *     self contained method
 *
 ************************************************************************/
Boolean XmCxxManager::traversal_children(Widget** children,
					 Cardinal* num_children) {
    XmTraversalChildrenProc func =
	((XmManagerClassExt)(((XmManagerWidgetClass) xmManagerWidgetClass)->
			     manager_class.extension))->traversal_children;
    return (func ? func((Widget) this, children, num_children) : FALSE);
}

Boolean XmCxxManager::TraversalChildren(Widget w, Widget** children,
					Cardinal* num_children) {
    return ((XmCxxManagerWidget) w)->
	traversal_children(children, num_children);
}

