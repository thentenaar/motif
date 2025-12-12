/* $XConsortium: CxxPrimiti.cxx /main/4 1995/07/17 11:45:53 drk $ */
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

#include <XmCxx/CxxPrimiti.h>


/***************
 * Resources   * -- NONE;
 ***************/

/***********************
 * Action Declarations * -- NONE;
 ***********************/

/****************
 * Translations * -- NONE;
 ****************/

/*************************************
 * Declare internal widget functions * -- NONE;
 *************************************/

/******************************************
 * Class declaration for CxxPrimitive * 
 ******************************************/

externaldef (xmcxxprimitiveclassrec)
XmCxxPrimitiveClass xmCxxPrimitiveClassRec("XmCxxPrimitive",
					   (WidgetClass) &xmPrimitiveClassRec,
					   sizeof(XmCxxPrimitive),
					  XmCxxPrimitiveClass::ClassInitialize,
					   NULL	/* action */,
					   0	/* num_action */,
					   NULL	/* resources */,
					   0	/* num_resources */,
					   XmCxxPrimitive::_MakeCxxWidget);


externaldef(xmcxxprimitivewidgetclass)
WidgetClass xmCxxPrimitiveWidgetClass = (WidgetClass) &xmCxxPrimitiveClassRec;


/*
 * 'Class record' constructor
 * Some entries *MUST* be overriden by subclass constructors
 *   -  _MakeCxxWidget must be redefined by each class and must call 'new'
 *	for its widget instance.
 *   -  synthetic resources (unless NULL) may not be inherited
 *	(resource chaining is automatically done by Motif)
 *
 *   -  TO BE CONTINUED...
 *
 * Some entries *MAY NOT* be overriden by subclass constructors :
 *   -  core_class.destroy (subclass destructors should be used instead)
 *   -  basically all function slots corresponding to a virtual function
 *	(subclasses should implement their own virtual function instead)
 *	
 *   -  TO BE CONTINUED...
 *
 * Some entries may be overriden by subclass constructors :
 */
XmCxxPrimitiveClass::XmCxxPrimitiveClass(char*		name,
					 WidgetClass	parentClass,
					 Cardinal	widgetSize,
					 XtProc		class_init,
					 XtActionList	actions,
					 Cardinal	num_actions,
					 XtResourceList	resources,
					 Cardinal	num_resources,
					 XtInitProc	cxx_cast) {

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
    core_class.compress_motion		= TRUE;
    core_class.compress_exposure	= XtExposeCompressMaximal;
    core_class.compress_enterleave	= TRUE;
    core_class.visible_interest		= FALSE;
    core_class.version			= XtVersion;
    core_class.callback_private		= NULL;
    core_class.tm_table			= XtInheritTranslations;
    core_class.extension		= core_ext;
	

    /*
     * Core part class-methods
     * Subclasses : DO NOT OVERRIDE THESE ENTRIES !!
     */
    // Chained methods must be NULL for all classes but CxxPrimitiveClass;
    if (this == &xmCxxPrimitiveClassRec) {
        core_class.class_part_initialize= 
	    XmCxxPrimitiveClass::ClassPartInitialize;
	core_class.initialize		= XmCxxPrimitive::Initialize;
	core_class.initialize_hook	= XmCxxPrimitive::InitializeHook;
	core_class.set_values		= XmCxxPrimitive::SetValues;
	core_class.set_values_hook	= XmCxxPrimitive::SetValuesHook;
	core_class.get_values_hook	= XmCxxPrimitive::GetValuesHook;
	core_class.destroy		= XmCxxPrimitive::Destroy;
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

    // Self contained methods must be set to these values for all classes;
    core_class.realize			= XmCxxPrimitive::Realize;
    core_class.resize			= XmCxxPrimitive::Resize;
    core_class.expose			= XmCxxPrimitive::Redisplay;
    core_class.set_values_almost	= XmCxxPrimitive::SetValuesAlmost;
    core_class.accept_focus		= XmCxxPrimitive::AcceptFocus;
    core_class.query_geometry		= XmCxxPrimitive::QueryGeometry;
    core_class.display_accelerator	= XmCxxPrimitive::DisplayAccelerator;
    
    
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
    // self contained methods;
    core_ext->setValuesPrehook		= XmCxxPrimitive::SetValuesPrehook;
    core_ext->initializePosthook	= XmCxxPrimitive::InitializePosthook;
    core_ext->setValuesPosthook		= XmCxxPrimitive::SetValuesPosthook;
    core_ext->secondaryObjectCreate	=XmCxxPrimitive::SecondaryObjectCreate;
    			// or XmInheritSecObjectCreate ?;
    core_ext->getSecResData		= XmCxxPrimitiveClass::GetSecResData;
    core_ext->getValuesPrehook		= XmCxxPrimitive::GetValuesPrehook;
    core_ext->getValuesPosthook		= XmCxxPrimitive::GetValuesPosthook;
    core_ext->classPartInitPrehook	=
			XmCxxPrimitiveClass::ClassPartInitializePrehook;
    core_ext->classPartInitPosthook	=
			XmCxxPrimitiveClass::ClassPartInitializePosthook;
    core_ext->widgetNavigable		= XmCxxPrimitive::WidgetNavigable;
    core_ext->focusChange		= XmCxxPrimitive::FocusChange;


    /*
     * Primitive part data;
     */
    XmPrimitiveClassExt prim_ext	= (XmPrimitiveClassExt)
	XtMalloc(sizeof(XmPrimitiveClassExtRec));
    
    primitive_class.translations	= NULL;
    primitive_class.syn_resources	= NULL;
    primitive_class.num_syn_resources	= 0;
    primitive_class.extension		= prim_ext;

    /*
     * Primitive part class-methods
     * Subclasses : DO NOT OVERRIDE THESE ENTRIES !
     */
    primitive_class.border_highlight	= XmCxxPrimitive::BorderHighlight;
    primitive_class.border_unhighlight	= XmCxxPrimitive::BorderUnhighlight;
    primitive_class.arm_and_activate	= XmCxxPrimitive::ArmAndActivate;

    /*
     * Primitive extension part data;
     */
    prim_ext->next_extension		= NULL;
    prim_ext->record_type		= NULLQUARK;
    prim_ext->version			= XmPrimitiveClassExtVersion,
    prim_ext->record_size		= sizeof(XmPrimitiveClassExtRec);

    /*
     * Primitive extension part class-methods
     * subclasses : DO NOT OVERRIDE THESE ENTRIES;
     */
    prim_ext->widget_baseline		= XmCxxPrimitive::WidgetBaseline;
    prim_ext->widget_display_rect	=
      			XmCxxPrimitive::WidgetDisplayRect;
    prim_ext->widget_margins		= XmCxxPrimitive::WidgetMargins;

    /*
     * CxxPrimitive part
     */
    cxx_primitive_extension		= NULL;
}


/******************************
 * constructors & destructors *
 ******************************/

XmCxxPrimitive::XmCxxPrimitive() {}
XmCxxPrimitive::~XmCxxPrimitive() {}


/*
 * Each widget is allocated by Xt but we still have to call 'new' to initialize
 * the virtual function table.
 * This is why we override 'new' so it does not perform any allocation.
 * For some reason, we cannot just redefine 'new' with an extra parameter
 * without redefining the default version of 'new', so let us do both at
 * the same time.
 */
void* XmCxxPrimitive::operator new(size_t, void* w /* =NULL */) {
    return w;
}

/*
 * Each widget is freed by Xt but we still have to call 'delete' for the
 * destructors to be called.
 * This is why we override 'delete' so it does not free any memory.
 */
void XmCxxPrimitive::operator delete(void*) {}


void XmCxxPrimitiveClass::SetBaseClassExtensionQuark() {
    /*
     * Set base_class extension record type. 
     * This cannot be done staticaly (quarks are not initialized then).
     * Make sure to get the right extension record...
     */
    XmBaseClassExt ext = (XmBaseClassExt) core_class.extension;
    while (ext) {
	if (ext->record_type == NULLQUARK &&
	    ext->record_size == sizeof(XmBaseClassExtRec) &&
	    ext->setValuesPrehook == XmCxxPrimitive::SetValuesPrehook) {
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
void XmCxxPrimitiveClass::ClassInitialize() {
    /* set up base_class extension quark */
    xmCxxPrimitiveClassRec.SetBaseClassExtensionQuark();
}


/************************************************************************
 *
 *  _MakeCxxWidget
 *	Call `new' to make each widget a C++ object
 *
 ************************************************************************/
void XmCxxPrimitive::_MakeCxxWidget(Widget /*req_w*/, Widget new_w,
				    ArgList, Cardinal*) {
    (void) IDLE_NEW(new_w) XmCxxPrimitive;
}


void XmCxxPrimitive::Destroy(Widget w) {
    // Upward chained;
    // call overloaded operator delete : no free, just call all destructors;
    delete (XmCxxPrimitiveWidget) w; 
}


/*
 *   The following functions go by pair :
 *         one static member, capitalized
 *         one virtual member, in lower case
 *   e.g. SetValues and set_values
 *
 *   Each static member do nothing else than calling the corresponding
 *   virtual member.
 *   The static members are only defined for CxxPrimitive and are stored
 *   in each class record, or for some of them (chained methods) only in
 *   the CxxPrimitiveClass instance.
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
void XmCxxPrimitiveClass::class_part_initialize() {
}

void XmCxxPrimitiveClass::ClassPartInitialize(WidgetClass wc) {
    // just call the widgetclass virtual function;
    ((XmCxxPrimitiveWidgetClass) wc)->class_part_initialize();
}


/************************************************************************
 *
 *  initialize : virtual
 *     Downward chained Xt method :
 *     each class re-implementing initialize() should start by
 *     calling <superclass>::initialize()
 *
 ************************************************************************/
void XmCxxPrimitive::initialize(Widget, ArgList, Cardinal*) {
}

void XmCxxPrimitive::Initialize(Widget req_w, Widget new_w,
				ArgList args, Cardinal* nargs) {
    // just call the widget virtual function;
    ((XmCxxPrimitiveWidget) new_w)->initialize(req_w, args, nargs);
}


/************************************************************************
 *
 *  initialize_hook : virtual
 *     Downward chained Xt method :
 *     each class re-implementing initialize_hook() should start by 
 *     calling <superclass>::initialize_hook()
 *
 ************************************************************************/
void XmCxxPrimitive::initialize_hook(ArgList, Cardinal*) {
}

void XmCxxPrimitive::InitializeHook(Widget w, ArgList args, Cardinal* nargs) {
    // just call the widget virtual function;
    ((XmCxxPrimitiveWidget) w)->initialize_hook(args, nargs);
}


/************************************************************************
 *
 *  set_values : virtual
 *     Downward chained Xt method :
 *     each class re-implementing set_values() should start by
 *     calling <superclass>::set_values()
 *
 ************************************************************************/
Boolean XmCxxPrimitive::set_values(Widget, Widget, ArgList, Cardinal*) {
    return FALSE;
}

Boolean XmCxxPrimitive::SetValues(Widget old_w, Widget req_w, Widget new_w,
				  ArgList args, Cardinal* nargs) {
    // just call the widget virtual function;
    return ((XmCxxPrimitiveWidget) new_w)->set_values(old_w,req_w, args,nargs);
}


/************************************************************************
 *
 *  set_values_hook : virtual
 *     Downward chained Xt method :
 *     each class re-implementing set_values_hook() should start by
 *     calling <superclass>::set_values_hook()
 *
 ************************************************************************/
Boolean XmCxxPrimitive::set_values_hook(ArgList, Cardinal*) {
    return FALSE;
}

Boolean XmCxxPrimitive::SetValuesHook(Widget w, ArgList args,Cardinal* nargs) {
    // just call the widget virtual function;
    return ((XmCxxPrimitiveWidget) w)->set_values_hook(args, nargs);
}


/************************************************************************
 *
 *  get_values_hook : virtual
 *     Downward chained Xt method :
 *     each class re-implementing get_values_hook() should start by
 *     calling <superclass>::get_values_hook()
 *
 ************************************************************************/
void XmCxxPrimitive::get_values_hook(ArgList, Cardinal*) {
}

void XmCxxPrimitive::GetValuesHook(Widget w, ArgList args, Cardinal* nargs) {
    // just call the widget virtual function;
    ((XmCxxPrimitiveWidget) w)->get_values_hook(args, nargs);
}


/************************************************************************
 *
 *  realize : virtual
 *     self contained Xt method
 *
 ************************************************************************/
void XmCxxPrimitive::realize(XtValueMask* mask,XSetWindowAttributes* win_att) {
    XtRealizeProc proc = xmPrimitiveClassRec.core_class.realize;
    // if proc == NULL, error : Realize method cannot be NULL;
    proc((Widget) this, mask, win_att);
}

void XmCxxPrimitive::Realize(Widget w, XtValueMask* mask,
			     XSetWindowAttributes* win_att) {
    ((XmCxxPrimitiveWidget) w)->realize(mask, win_att);
}


/************************************************************************
 *
 *  resize : virtual
 *     self contained Xt method
 *
 ************************************************************************/
void XmCxxPrimitive::resize() {
    XtWidgetProc proc = xmPrimitiveClassRec.core_class.resize;
    if (proc)
	proc((Widget) this);
}

void XmCxxPrimitive::Resize(Widget w) {
    ((XmCxxPrimitiveWidget) w)->resize();
}


/************************************************************************
 *
 *  expose - redisplay : virtual
 *     self contained Xt method
 *
 ************************************************************************/
void XmCxxPrimitive::expose(XEvent* event, Region region) {
    XtExposeProc proc = xmPrimitiveClassRec.core_class.expose;
    if (proc)
	proc((Widget) this, event, region);
}

void XmCxxPrimitive::Redisplay(Widget w, XEvent *event, Region region) {
    ((XmCxxPrimitiveWidget) w)->expose(event, region);
}


/************************************************************************
 *
 *  accept_focus : virtual
 *     self contained Xt method
 *
 ************************************************************************/
Boolean XmCxxPrimitive::accept_focus(Time* t) {
    XtAcceptFocusProc func = xmPrimitiveClassRec.core_class.accept_focus;
    return (func ? func((Widget) this, t) : FALSE);
}

Boolean XmCxxPrimitive::AcceptFocus(Widget w, Time* t) {
    return ((XmCxxPrimitiveWidget) w)->accept_focus(t);
}
    

/************************************************************************
 *
 *  set_values_almost : virtual
 *     self contained Xt method
 *
 ************************************************************************/
void XmCxxPrimitive::set_values_almost(Widget old_w,
				       XtWidgetGeometry* request,
				       XtWidgetGeometry* reply) {
    XtAlmostProc proc = xmPrimitiveClassRec.core_class.set_values_almost;
    // if proc == NULL, error : SetValuesAlmost method cannot be NULL;
    proc(old_w, (Widget) this, request, reply);
}

void XmCxxPrimitive::SetValuesAlmost(Widget old_w, Widget new_w,
				     XtWidgetGeometry* request,
				     XtWidgetGeometry* reply) {
    ((XmCxxPrimitiveWidget) new_w)->set_values_almost(old_w, request,reply);
}


/************************************************************************
 *
 *  query_geometry : virtual
 *     self contained Xt method
 *
 ************************************************************************/
XtGeometryResult XmCxxPrimitive::query_geometry(XtWidgetGeometry *intended,
						XtWidgetGeometry *reply) {
    XtGeometryHandler func = xmPrimitiveClassRec.core_class.query_geometry;
/*
 * return (func ? func((Widget) this, intended, reply) : XtGeometryYes);
 * does not compile on sgi/C++.
 */
    if (func)
	return func((Widget) this, intended, reply);
    return XtGeometryYes;
}

XtGeometryResult XmCxxPrimitive::QueryGeometry(Widget w,
					       XtWidgetGeometry *intended,
					       XtWidgetGeometry *reply) {
    return ((XmCxxPrimitiveWidget) w)->query_geometry(intended, reply);
}


/************************************************************************
 *
 *  display_accelerator : virtual
 *     self contained Xt method
 *
 ************************************************************************/
void XmCxxPrimitive::display_accelerator(String str) {
    XtStringProc proc = xmPrimitiveClassRec.core_class.display_accelerator;
    if (proc)
	proc((Widget) this, str);
}

void XmCxxPrimitive::DisplayAccelerator(Widget w, String str) {
    ((XmCxxPrimitiveWidget) w)->display_accelerator(str);
}


/************************************************************************
 *
 *  class_part_initialize_prehook : virtual
 *     self contained method
 *
 ************************************************************************/
void XmCxxPrimitiveClass::class_part_initialize_prehook() {
}

void XmCxxPrimitiveClass::ClassPartInitializePrehook(WidgetClass wc) {
    ((XmCxxPrimitiveClass*) wc)->class_part_initialize_prehook();
}


/************************************************************************
 *
 *  class_part_initialize_posthook : virtual
 *     self contained method
 *
 ************************************************************************/
void XmCxxPrimitiveClass::class_part_initialize_posthook() {
}

void XmCxxPrimitiveClass::ClassPartInitializePosthook(WidgetClass wc) {
    ((XmCxxPrimitiveClass*) wc)->class_part_initialize_posthook();
}


/************************************************************************
 *
 *  sec_res_data : virtual
 *     self contained method
 *
 ************************************************************************/
Cardinal XmCxxPrimitiveClass::sec_res_data(XmSecondaryResourceData **r_d) {
    XmGetSecResDataFunc func =
	((XmBaseClassExt) xmPrimitiveClassRec.core_class.extension)->
	    getSecResData;
    return (func ? func((WidgetClass) this, r_d) : 0);
}

Cardinal XmCxxPrimitiveClass::GetSecResData(WidgetClass wc,
					    XmSecondaryResourceData **r_d) {
    return ((XmCxxPrimitiveClass*) wc)->sec_res_data(r_d);
}


/************************************************************************
 *
 *  set_values_prehook : virtual
 *     self contained method
 *
 ************************************************************************/
Boolean XmCxxPrimitive::set_values_prehook(Widget old_w, Widget req_w,
					   ArgList args, Cardinal* nargs) {
    XtSetValuesFunc func =
	((XmBaseClassExt) xmPrimitiveClassRec.core_class.extension)->
	    setValuesPrehook;
    return (func ? func(old_w, req_w, (Widget) this, args, nargs) : FALSE);
}

Boolean XmCxxPrimitive::SetValuesPrehook(Widget old_w, Widget req_w,
					 Widget new_w,
					 ArgList args, Cardinal* nargs) {
    return ((XmCxxPrimitiveWidget) new_w)->
	set_values_prehook(old_w, req_w, args, nargs);
}


/************************************************************************
 *
 *  initialize_posthook : virtual
 *     self contained method
 *
 ************************************************************************/
void XmCxxPrimitive::initialize_posthook(Widget req_w,
					 ArgList args, Cardinal* nargs) {
    XtInitProc proc =
	((XmBaseClassExt) xmPrimitiveClassRec.core_class.extension)->
	    initializePosthook;
    if (proc)
	proc(req_w, (Widget) this, args, nargs);
}

void XmCxxPrimitive::InitializePosthook(Widget req_w, Widget new_w,
					ArgList args, Cardinal* nargs) {
    ((XmCxxPrimitiveWidget) new_w)->initialize_posthook(req_w, args, nargs);
}


/************************************************************************
 *
 *  set_values_posthook : virtual
 *     self contained method
 *
 ************************************************************************/
Boolean XmCxxPrimitive::set_values_posthook(Widget old_w, Widget req_w,
					    ArgList args,Cardinal* nargs) {
    XtSetValuesFunc func =
	((XmBaseClassExt) xmPrimitiveClassRec.core_class.extension)->
	    setValuesPosthook;
    return (func ? func(old_w, req_w, (Widget) this, args, nargs) : FALSE);
}

Boolean XmCxxPrimitive::SetValuesPosthook(Widget old_w, Widget req_w,
					  Widget new_w,
					  ArgList args, Cardinal* nargs) {
    return ((XmCxxPrimitiveWidget) new_w)->
	set_values_posthook(old_w, req_w, args, nargs);
}


/************************************************************************
 *
 *  secondary_object_create : virtual
 *     self contained method
 *
 ************************************************************************/
void XmCxxPrimitive::secondary_object_create(Widget req_w,
					     ArgList args,Cardinal* nargs) {
    XtInitProc proc =
	((XmBaseClassExt) xmPrimitiveClassRec.core_class.extension)->
	    secondaryObjectCreate;
    if (proc)
	proc(req_w, (Widget) this, args, nargs);
}

void XmCxxPrimitive::SecondaryObjectCreate(Widget req_w, Widget new_w,
					   ArgList args, Cardinal* nargs) {
    ((XmCxxPrimitiveWidget) new_w)->secondary_object_create(req_w, args,nargs);
}


/************************************************************************
 *
 *  get_values_prehook : virtual
 *     self contained method
 *
 ************************************************************************/
void XmCxxPrimitive::get_values_prehook(ArgList args, Cardinal* nargs) {
    XtArgsProc proc =
	((XmBaseClassExt) xmPrimitiveClassRec.core_class.extension)->
	    getValuesPrehook;
    if (proc)
	proc((Widget) this, args, nargs);
}

void XmCxxPrimitive::GetValuesPrehook(Widget w,
				      ArgList args, Cardinal* nargs) {
    ((XmCxxPrimitiveWidget) w)->get_values_prehook(args, nargs);
}


/************************************************************************
 *
 *  get_values_posthook : virtual
 *     self contained method
 *
 ************************************************************************/
void XmCxxPrimitive::get_values_posthook(ArgList args, Cardinal* nargs) {
    XtArgsProc proc =
	((XmBaseClassExt) xmPrimitiveClassRec.core_class.extension)->
	    getValuesPosthook;
    if (proc)
	proc((Widget) this, args, nargs);
}

void XmCxxPrimitive::GetValuesPosthook(Widget w, ArgList args,Cardinal* nargs){
    ((XmCxxPrimitiveWidget) w)->get_values_posthook(args, nargs);
}


/************************************************************************
 *
 *  widget_navigable : virtual
 *     self contained method
 *
 ************************************************************************/
XmNavigability XmCxxPrimitive::widget_navigable() {
    XmWidgetNavigableProc func =
	((XmBaseClassExt) xmPrimitiveClassRec.core_class.extension)->
	    widgetNavigable;
    /*
     * return (func ? func((Widget) this) : XmNOT_NAVIGABLE);
     * does not compile on sgi/C++.
     */
    if (func)
	return func((Widget) this);
    return XmNOT_NAVIGABLE;
}

XmNavigability XmCxxPrimitive::WidgetNavigable(Widget w) {
    return ((XmCxxPrimitiveWidget) w)->widget_navigable();
}


/************************************************************************
 *
 *  focus_change : virtual
 *     self contained method
 *
 ************************************************************************/
void XmCxxPrimitive::focus_change(XmFocusChange change) {
    XmFocusChangeProc proc =
	((XmBaseClassExt) xmPrimitiveClassRec.core_class.extension)->
	    focusChange;
    if (proc)
	proc((Widget) this, change);
}

void XmCxxPrimitive::FocusChange(Widget w, XmFocusChange change) {
    ((XmCxxPrimitiveWidget) w)->focus_change(change);
}


/************************************************************************
 *
 *  border_highlight : virtual
 *     self contained method
 *
 ************************************************************************/
void XmCxxPrimitive::border_highlight() {
    XtWidgetProc proc = xmPrimitiveClassRec.primitive_class.border_highlight;
    if (proc)
	proc((Widget) this);
}

void XmCxxPrimitive::BorderHighlight(Widget w) {
    ((XmCxxPrimitiveWidget) w)->border_highlight();
}


/************************************************************************
 *
 *  border_unhighlight : virtual
 *     self contained method
 *
 ************************************************************************/
void XmCxxPrimitive::border_unhighlight() {
    XtWidgetProc proc = xmPrimitiveClassRec.primitive_class.border_highlight;
    if (proc)
	proc((Widget) this);
}

void XmCxxPrimitive::BorderUnhighlight(Widget w) {
    ((XmCxxPrimitiveWidget) w)->border_unhighlight();
}


/************************************************************************
 *
 *  arm_and_activate : virtual
 *     self contained method
 *
 ************************************************************************/
void XmCxxPrimitive::arm_and_activate(XEvent* event,
				      String* params, Cardinal* nparams) {
    XtActionProc proc = xmPrimitiveClassRec.primitive_class.arm_and_activate;
    if (proc)
	proc((Widget) this, event, params, nparams);
}

void XmCxxPrimitive::ArmAndActivate(Widget w, XEvent* event,
				    String* params, Cardinal* nparams) {
    ((XmCxxPrimitiveWidget) w)->arm_and_activate(event, params, nparams);
}


/************************************************************************
 *
 *  widget_baseline : virtual
 *     self contained method
 *
 ************************************************************************/
Boolean XmCxxPrimitive::widget_baseline(Dimension** dim, int* nb) {
    XmWidgetBaselineProc func =
	((XmPrimitiveClassExt)
	 xmPrimitiveClassRec.primitive_class.extension)->widget_baseline;
    return (func ? func((Widget) this, dim, nb) : FALSE);
}

Boolean XmCxxPrimitive::WidgetBaseline(Widget w, Dimension** dim, int* nb){
    return ((XmCxxPrimitiveWidget) w)->widget_baseline(dim, nb);
}


/************************************************************************
 *
 *  widget_display_rect : virtual
 *     self contained method
 *
 ************************************************************************/
Boolean XmCxxPrimitive::widget_display_rect(XRectangle* rect) {
    XmWidgetDisplayRectProc func =
	((XmPrimitiveClassExt)
	 xmPrimitiveClassRec.primitive_class.extension)->widget_display_rect;
    return (func ? func((Widget) this, rect) : FALSE);
}

Boolean XmCxxPrimitive::WidgetDisplayRect(Widget w, XRectangle* rect) {
    return ((XmCxxPrimitiveWidget) w)->widget_display_rect(rect);
}


/************************************************************************
 *
 *  widget_margins : virtual
 *     self contained method
 *
 ************************************************************************/
void XmCxxPrimitive::widget_margins(XmBaselineMargins* margins) {
    XmWidgetMarginsProc proc =
	((XmPrimitiveClassExt)
	 xmPrimitiveClassRec.primitive_class.extension)->widget_margins;
    if (proc)
	proc((Widget) this, margins);
}

void XmCxxPrimitive::WidgetMargins(Widget w, XmBaselineMargins* margins) {
    ((XmCxxPrimitiveWidget) w)->widget_margins(margins);
}

