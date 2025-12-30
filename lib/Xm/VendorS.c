/**
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$TOG: VendorS.c /main/21 1999/08/09 10:49:41 mgreess $"
#endif
#endif

/* Make sure all wm properties can make it out of the resource manager */

#include <stdio.h>
#include <Xm/AccColorT.h>
#include <Xm/DisplayP.h>
#include <Xm/LayoutT.h>
#include <Xm/MenuShell.h>
#include <Xm/ProtocolsP.h>
#include <Xm/ScreenP.h>
#include <Xm/SpecRenderT.h>
#include <Xm/TraitP.h>
#include <Xm/UnitTypeT.h>
#include <Xm/VendorSEP.h>
#include <Xm/VendorSP.h>
#include <Xm/XmosP.h>		/* for bzero */
#include <Xm/ToolTipCT.h>
#if defined(__APPLE__)
#include <Xm/GrabShell.h>
#include <Xm/DragOverS.h>
#endif
#include "BaseClassI.h"
#include "CallbackI.h"
#include "ExtObjectI.h"
#include "MenuShellI.h"
#include "MessagesI.h"
#include "PixConvI.h"
#include "ProtocolsI.h"
#include "ResConverI.h"
#include "SyntheticI.h"
#include "TraitI.h"
#include "TravActI.h"
#include "TraversalI.h"
#include "XmImI.h"
#include "VendorSI.h"
#include "XmI.h"

#include <X11/Xmu/Editres.h>

#if XM_MSGCAT
#if !defined(NL_CAT_LOCALE)
#define NL_CAT_LOCALE	0
#endif
#endif


/* #define DEBUG_GRABS */

#define MSG1	_XmMMsgVendor_0000
#define MSG2	_XmMMsgVendor_0001
#define MSG3	_XmMMsgVendor_0002
#define MSG4	_XmMMsgVendor_0003

#define DONT_CARE -1

typedef struct {
    XmVendorShellExtObject ve ;
    Widget shell ;
    } XmDestroyGrabRec, *XmDestroyGrabList ;


/********    Static Function Declarations    ********/

static XtPointer BaseProc(
                        Widget widget,
                        XtPointer client_data) ;
static Cardinal GetSecResData(
                        WidgetClass w_class,
                        XmSecondaryResourceData **secResDataRtn) ;
static void ClassInitialize( void ) ;
static void ClassPartInitialize(
                        WidgetClass wc) ;
static void SetMwmStuff(
                        XmVendorShellExtObject ove,
                        XmVendorShellExtObject nve) ;
static void AddGrab(
                        XmVendorShellExtObject ve,
                        Widget shell,
                        Boolean exclusive,
                        Boolean springLoaded,
                        XmVendorShellExtObject origKid) ;
static void RemoveGrab(
                        XmVendorShellExtObject ve,
                        Boolean being_destroyed,
                        Widget shell) ;
static void RemoveGrabCallback(
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data) ;
static void AddToGrabList(
                        Widget parent,
                        Widget excludedKid,
                        Widget origKid) ;
static void AddCousinsToGrabList(
                        Widget parent,
                        Widget excludedKid,
                        Widget origKid) ;
static Boolean IsPopupShell(
                        Widget shell) ;
static void PopupCallback(
                        Widget shellParent,
                        XtPointer closure,
                        XtPointer callData) ;
static void PopdownCallback(
                        Widget shellParent,
                        XtPointer closure,
                        XtPointer callData) ;
static Widget GetNextShell(
                        Widget vw) ;
static XmDesktopObject GetShellDesktopParent(
                        VendorShellWidget vw,
                        ArgList args,
                        Cardinal *num_args) ;
static void SecondaryObjectCreate(
                        Widget req,
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args) ;
static void InitializePrehook(
                        Widget req,
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args) ;
static void VendorExtInitialize(
                        Widget req,
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args) ;
static void MotifWarningHandler (
                        String name,
                        String type,
                        String s_class,
                        String message,
                        String * params,
                        Cardinal* num_params);
static void Initialize(
                        Widget req,
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args) ;
static void InitializePosthook(
                        Widget req,
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args) ;
static Boolean SetValuesPrehook(
                        Widget old,
                        Widget ref,
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args) ;
static Boolean VendorExtSetValues(
                        Widget old,
                        Widget ref,
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args) ;
static Boolean SetValues(
                        Widget current,
                        Widget req,
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args) ;
static Boolean SetValuesPosthook(
                        Widget current,
                        Widget req,
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args) ;
static void GetValuesPrehook(
                        Widget w,
                        ArgList args,
                        Cardinal *num_args) ;
static void GetValuesHook(
                        Widget w,
                        ArgList args,
                        Cardinal *num_args) ;
static void GetValuesPosthook(
                        Widget w,
                        ArgList args,
                        Cardinal *num_args) ;
static void PendingTransientDestroyed(
                        Widget vw,
                        XtPointer cl_data,
                        XtPointer ca_data) ;
static void SetTransientFor(
                        Widget w,
                        XtPointer closure,
                        XtPointer call_data) ;
static void Resize(
                        Widget w) ;
static void ChangeManaged(
                        Widget wid) ;
static void UpdateCoreGeometry(
                        VendorShellWidget vw,
                        XmVendorShellExtObject vendorExt) ;
static void Realize(
                        Widget wid,
                        XtValueMask *vmask,
                        XSetWindowAttributes *attr) ;
static XtGeometryResult GeometryManager(
                        Widget wid,
                        XtWidgetGeometry *request,
                        XtWidgetGeometry *reply) ;
static XtGeometryResult RootGeometryManager(
                        Widget wid,
                        XtWidgetGeometry *request,
                        XtWidgetGeometry *reply) ;
static void SetMwmHints(
                        XmVendorShellExtObject ve) ;
static void SetMwmMenu(
                        XmVendorShellExtObject ve) ;
static void VendorExtRealize(
                        Widget w,
                        XtPointer closure,
                        XtPointer call_data) ;

static void AddDLEntry(
                        XmVendorShellExtObject ve,
                        Widget shell) ;
static void RemoveDLEntry(
                        unsigned pos) ;
static void Destroy(
                        Widget wid) ;
static void Redisplay(
                        Widget wid,
                        XEvent *event,
                        Region region) ;
static XmFontList GetTable(Widget wid,
			   XtEnum type);
static XmDirection GetDirection(Widget);
static void GetColors(Widget widget,
		      XmAccessColorData color_data);
static unsigned char GetUnitType(Widget);

/********    End Static Function Declarations    ********/

#ifdef DEBUG_GRABS


static void PrintModal(
                        XmModalData modal) ;
static void PrintXmGrabs(
                        Widget wid) ;


#endif /* DEBUG_GRABS */

char _XmVersionString[] = XmVERSION_STRING ;

static XmDestroyGrabList destroy_list ;
static unsigned short destroy_list_size ;
static unsigned short destroy_list_cnt ;

static Display * _XmDisplayHandle = NULL ;
static XtErrorMsgHandler previousWarningHandler = NULL;


#if defined(__APPLE__)
/* Hack necessary to handle Apple two-level namespaces */
extern WidgetClass vendorShellWidgetClass; /* from Xt/Vendor.c */
extern VendorShellClassRec xmVendorShellClassRec;
#define vendorShellClassRec xmVendorShellClassRec

__attribute__((constructor))
static void __VendorShellHack(void)
{
    vendorShellWidgetClass = (WidgetClass)(&xmVendorShellClassRec);
    transientShellWidgetClass->core_class.superclass = vendorShellWidgetClass;
    topLevelShellWidgetClass->core_class.superclass = vendorShellWidgetClass;
    xmGrabShellWidgetClass->core_class.superclass = vendorShellWidgetClass;
    xmDragOverShellWidgetClass->core_class.superclass = vendorShellWidgetClass;
}
#endif


/***************************************************************************
 *
 * Vendor shell class record
 *
 ***************************************************************************/

#define Offset(field) (XtOffsetOf(WMShellRec, field))

static int default_unspecified_shell_int = XtUnspecifiedShellInt;
/*
 * Warning, casting XtUnspecifiedShellInt (which is -1) to an (XtPointer)
 * can result is loss of bits on some machines (i.e. crays)
 */

static XtResource resources[] =
{
    {
	XmNx, XmCPosition, XmRHorizontalPosition, sizeof(Position),
	XtOffsetOf(WidgetRec, core.x), XmRImmediate, (XtPointer) 0,
    },

    {
	XmNy, XmCPosition, XmRVerticalPosition, sizeof(Position),
	XtOffsetOf(WidgetRec, core.y), XmRImmediate, (XtPointer) 0,
    },

    {
	XmNwidth, XmCDimension, XmRHorizontalDimension, sizeof(Dimension),
	XtOffsetOf(WidgetRec, core.width), XmRImmediate, (XtPointer) 0,
    },

    {
	XmNheight, XmCDimension, XmRVerticalDimension, sizeof(Dimension),
	XtOffsetOf(WidgetRec, core.height), XmRImmediate, (XtPointer) 0,
    },

    {
	XmNborderWidth, XmCBorderWidth, XmRHorizontalDimension,
	sizeof(Dimension),
	XtOffsetOf(WidgetRec, core.border_width), XmRImmediate, (XtPointer) 0,
    },

    /* size_hints minus things stored in core */
    { XmNbaseWidth, XmCBaseWidth, XmRHorizontalInt, sizeof(int),
	Offset(wm.base_width),
	XmRHorizontalInt, (XtPointer) &default_unspecified_shell_int},
    { XmNbaseHeight, XmCBaseHeight, XmRVerticalInt, sizeof(int),
	Offset(wm.base_height),
	XmRVerticalInt, (XtPointer) &default_unspecified_shell_int},
    { XmNminWidth, XmCMinWidth, XmRHorizontalInt, sizeof(int),
	Offset(wm.size_hints.min_width),
	XmRHorizontalInt, (XtPointer) &default_unspecified_shell_int},
    { XmNminHeight, XmCMinHeight, XmRVerticalInt, sizeof(int),
	Offset(wm.size_hints.min_height),
	XmRVerticalInt, (XtPointer) &default_unspecified_shell_int},
    { XmNmaxWidth, XmCMaxWidth, XmRHorizontalInt, sizeof(int),
	Offset(wm.size_hints.max_width),
	XmRHorizontalInt, (XtPointer) &default_unspecified_shell_int},
    { XmNmaxHeight, XmCMaxHeight, XmRVerticalInt, sizeof(int),
	Offset(wm.size_hints.max_height),
	XmRVerticalInt, (XtPointer) &default_unspecified_shell_int},
    { XmNwidthInc, XmCWidthInc, XmRHorizontalInt, sizeof(int),
	Offset(wm.size_hints.width_inc),
	XmRHorizontalInt, (XtPointer) &default_unspecified_shell_int},
    { XmNheightInc, XmCHeightInc, XmRVerticalInt, sizeof(int),
	Offset(wm.size_hints.height_inc),
	XmRVerticalInt, (XtPointer) &default_unspecified_shell_int},
    { XmNminAspectX, XmCMinAspectX, XmRHorizontalInt, sizeof(int),
	Offset(wm.size_hints.min_aspect.x),
	XmRHorizontalInt, (XtPointer) &default_unspecified_shell_int},
    { XmNminAspectY, XmCMinAspectY, XmRVerticalInt, sizeof(int),
	Offset(wm.size_hints.min_aspect.y),
	XmRVerticalInt, (XtPointer) &default_unspecified_shell_int},
    { XmNmaxAspectX, XmCMaxAspectX, XmRHorizontalInt, sizeof(int),
	Offset(wm.size_hints.max_aspect.x),
	XmRHorizontalInt, (XtPointer) &default_unspecified_shell_int},
    { XmNmaxAspectY, XmCMaxAspectY, XmRVerticalInt, sizeof(int),
	Offset(wm.size_hints.max_aspect.y),
	XmRVerticalInt, (XtPointer) &default_unspecified_shell_int},
    /* wm_hints */
    { XmNiconPixmap, XmCIconPixmap, XmRDynamicPixmap, sizeof(Pixmap),
	Offset(wm.wm_hints.icon_pixmap), XmRPixmap, NULL},
    { XmNiconX, XmCIconX, XmRHorizontalInt, sizeof(int),
	Offset(wm.wm_hints.icon_x),
	XmRHorizontalInt, (XtPointer) &default_unspecified_shell_int},
    { XmNiconY, XmCIconY, XmRVerticalInt, sizeof(int),
	Offset(wm.wm_hints.icon_y),
	XmRVerticalInt, (XtPointer) &default_unspecified_shell_int},

    { /* override dec default */
	XmNinput, XmCInput, XmRBool,
	sizeof(Bool), Offset(wm.wm_hints.input),
	XmRImmediate, (XtPointer) TRUE,
    },
    { /* override incorrect default in Shell.c */
	XmNwindowGroup, XmCWindowGroup, XmRWindow, sizeof(Window),
	Offset(wm.wm_hints.window_group),
	XmRImmediate, (XtPointer)XtUnspecifiedWindowGroup,
    },
   {  /* default visual dynamically, see _XmDefaultVisualResources */
       XtNvisual, XtCVisual,
       XtRVisual, sizeof(Visual*),
       XtOffsetOf(ShellRec, shell.visual),
       XtRImmediate, (XtPointer)INVALID_VISUAL
   },
};
#undef Offset

static XtResource subresources[] =
{
    {
        XmNtoolTipPostDelay, XmCToolTipPostDelay, XmRInt,
        sizeof(unsigned int), XtOffsetOf(XmToolTipConfigTraitRec, post_delay),
        XmRImmediate, (XtPointer) 5000,
    },
    {
        XmNtoolTipPostDuration, XmCToolTipPostDuration, XmRInt,
        sizeof(unsigned int), XtOffsetOf(XmToolTipConfigTraitRec, post_duration),
        XmRImmediate, (XtPointer) 5000,
    },
    {
        XmNtoolTipEnable, XmCToolTipEnable, XmRBoolean,
        sizeof(Boolean), XtOffsetOf(XmToolTipConfigTraitRec, enable),
        XmRImmediate, (XtPointer) False,
    },
};

static CompositeClassExtensionRec compositeClassExtRec = {
    NULL,
    NULLQUARK,
    XtCompositeExtensionVersion,
    sizeof(CompositeClassExtensionRec),
    TRUE,
};

static ShellClassExtensionRec shellClassExtRec = {
    NULL,
    NULLQUARK,
    XtShellExtensionVersion,
    sizeof(ShellClassExtensionRec),
    RootGeometryManager,
};

static XmBaseClassExtRec baseClassExtRec = {
    NULL,
    NULLQUARK,
    XmBaseClassExtVersion,
    sizeof(XmBaseClassExtRec),
    InitializePrehook,				/* init prehook		*/
    SetValuesPrehook,				/* setval prehook	*/
    InitializePosthook,				/* init posthook	*/
    SetValuesPosthook,				/* setval posthook	*/
    (WidgetClass)&xmVendorShellExtClassRec,	/* secondary class	*/
    SecondaryObjectCreate,	        /* secondary create	*/
    GetSecResData,		                /* getSecRes data       */
    {0},					/* class flags		*/
    GetValuesPrehook,				/* get_values_prehook	*/
    GetValuesPosthook,				/* get_values_posthook  */
};

externaldef(vendorshellclassrec)
VendorShellClassRec vendorShellClassRec = {
    {
	(WidgetClass) &wmShellClassRec, /* superclass 		*/
	"VendorShell", 			/* class_name 		*/
	sizeof(VendorShellRec), 	/* size 		*/
	ClassInitialize, 		/* Class Initializer 	*/
	ClassPartInitialize, 		/* class_part_init 	*/
	FALSE, 				/* Class init'ed ? 	*/
	Initialize, 			/* initialize         	*/
	NULL, 				/* initialize_hook	*/
	Realize, 			/* realize            	*/
	NULL,	 			/* actions            	*/
	0,				/* num_actions        	*/
	resources, 			/* resources          	*/
	XtNumber(resources), 		/* resource_count     	*/
	NULLQUARK, 			/* xrm_class          	*/
	FALSE, 				/* compress_motion    	*/
	XtExposeCompressSeries, 	/* compress_exposure  	*/
	FALSE, 				/* compress_enterleave	*/
	FALSE, 				/* visible_interest   	*/
	Destroy,			/* destroy            	*/
	Resize,		 		/* resize             	*/
	NULL,				/* expose             	*/
	SetValues,	 		/* set_values         	*/
	NULL, 				/* set_values_hook      */
	XtInheritSetValuesAlmost, 	/* set_values_almost    */
	GetValuesHook,			/* get_values_hook      */
	NULL, 				/* accept_focus       	*/
	XtVersion, 			/* intrinsics version 	*/
	NULL, 				/* callback offsets   	*/
	NULL,				/* tm_table           	*/
	NULL, 				/* query_geometry       */
	NULL, 				/* display_accelerator  */
	(XtPointer)&baseClassExtRec,	/* extension		*/
    },
    { 					/* composite_class	*/
	GeometryManager,		/* geometry_manager 	*/
	ChangeManaged,		 	/* change_managed 	*/
	XtInheritInsertChild, 		/* insert_child 	*/
	XtInheritDeleteChild, 		/* delete_child 	*/
	(XtPointer)&compositeClassExtRec,/* extension            */
    },
    {                            	/* shell class		*/
	(XtPointer)&shellClassExtRec,	/* extension 		*/
    },
    {                            	/* wmshell class	*/
	NULL, 				/* extension            */
    },
    {   				/* vendorshell class	*/
	NULL, 				/* extension            */
    }
};

externaldef(vendorshellwidgetclass) WidgetClass
  vendorShellWidgetClass = (WidgetClass) (&vendorShellClassRec);




/* Trait record for VendorS specify render table */

static XmConst XmSpecRenderTraitRec vsSRT = {
  0,		/* version */
  GetTable,
};

/* Trait record for VendorS specify layout direction  */

static XmConst XmSpecifyLayoutDirectionTraitRec vsLDT = {
  0,			/* version */
  GetDirection
};


/* Access Colors Trait record for vendor shell */

static XmConst XmAccessColorsTraitRec vsACT = {
  0,			/* version */
  GetColors
};

/* Unit Type Trait record for VendorShell */

static XmConst XmSpecUnitTypeTraitRec vsUTT = {
  0,			/* version */
  GetUnitType
};


/************************************************************************
 *
 *  BaseProc
 *    This function can be used to check if the widget has actually
 *    been initialize (vs in the process of creation, like a conversion)
 *
 ************************************************************************/
/*ARGSUSED*/
static XtPointer
BaseProc(
        Widget widget,
        XtPointer client_data )	/* unused */
{
   XmWidgetExtData	extData;
   Widget		secObj = NULL;
   _XmWidgetToAppContext(widget);

   _XmAppLock(app);


    if ((extData = _XmGetWidgetExtData(widget, XmSHELL_EXTENSION)))
    {
	secObj = extData->widget;
    }

    _XmAppUnlock(app);
    return secObj;
}


/************************************************************************
 *
 *  GetSecResData
 *
 ************************************************************************/
static Cardinal
GetSecResData(
        WidgetClass w_class,
        XmSecondaryResourceData **secResDataRtn )
{
    XmBaseClassExt	*bcePtr;
    int arrayCount = 0;
    String  resource_class, resource_name;
    XtPointer  client_data;

    _XmProcessLock();
    if ((bcePtr = _XmGetBaseClassExtPtr(w_class, XmQmotif)) &&
	(bcePtr) && (*bcePtr) &&
	((*bcePtr)->secondaryObjectClass))
    {
      client_data = NULL;
      resource_class = NULL;
      resource_name = NULL;
      arrayCount =
        _XmSecondaryResourceData ( *bcePtr, secResDataRtn, client_data,
                                   resource_name, resource_class,
			           BaseProc) ;

    }
    _XmProcessUnlock();
    return arrayCount;
}

/************************************************************************
 *
 *  ClassInitialize
 *    Initialize the vendorShell class structure.  This is called only
 *    the first time a vendorShell widget is created.  It registers the
 *    resource type converters unique to this class.
 *
 ************************************************************************/
static void
ClassInitialize( void )
{
  Cardinal                    wc_num_res, sc_num_res;
  XtResource                  *merged_list;
  int                         i, j;
  XtResourceList              uncompiled;
  Cardinal                    num;

/**************************************************************************
   ShellExt's and VendorShellExt's resource lists are being merged into one
   and assigned to xmVendorShellExtObjectClassRec. This is for performance
   reasons, since, instead of two calls to XtGetSubResources() XtGetSubvaluse()
   and XtSetSubvalues() for both the superclass and the widget class, now
   we have just one call with a merged resource list.
   NOTE: At this point the resource lists for ShellExt and VendorShellExt do
         have unique entries, but if there are resources in the superclass
         that are being overwritten by the subclass then the merged_lists
         need to be created differently.
****************************************************************************/

  _XmProcessLock();
  wc_num_res = xmVendorShellExtClassRec.object_class.num_resources;

  sc_num_res = xmShellExtClassRec.object_class.num_resources;

  merged_list = (XtResource *)XtMalloc((sizeof(XtResource) * (wc_num_res +
                                                                 sc_num_res)));

  _XmTransformSubResources(xmShellExtClassRec.object_class.resources,
                           sc_num_res, &uncompiled, &num);

  for (i = 0; i < num; i++)
  {

  merged_list[i] = uncompiled[i];

  }
  _XmProcessUnlock();

  _XmProcessLock();
  for (i = 0, j = num; i < wc_num_res; i++, j++)
  {
   merged_list[j] =
        xmVendorShellExtClassRec.object_class.resources[i];
  }
  _XmProcessUnlock();

  _XmProcessLock();
  xmVendorShellExtClassRec.object_class.resources = merged_list;
  xmVendorShellExtClassRec.object_class.num_resources = wc_num_res + sc_num_res ;
  _XmProcessUnlock();

  _XmRegisterConverters();
  _XmRegisterPixmapConverters();

  _XmInitializeExtensions();
  _XmInitializeTraits();

{
   XtProc class_initialize;
   _XmProcessLock();
   class_initialize = xmVendorShellExtObjectClass->core_class.class_initialize;
   _XmProcessUnlock();

   (*class_initialize) ();
}
   baseClassExtRec.record_type = XmQmotif;

   _XmBuildExtResources((WidgetClass) baseClassExtRec.secondaryObjectClass);

    _XmProcessLock();

   if (((XmShellExtObjectClass)baseClassExtRec.secondaryObjectClass)->desktop_class.insert_child ==
          XtInheritInsertChild)
       ((XmShellExtObjectClass)baseClassExtRec.secondaryObjectClass)->desktop_class.insert_child =
        ((XmShellExtObjectClass) xmDesktopClass)->desktop_class.insert_child;

   if (((XmShellExtObjectClass)baseClassExtRec.secondaryObjectClass)->desktop_class.delete_child ==
          XtInheritDeleteChild)
       ((XmShellExtObjectClass)baseClassExtRec.secondaryObjectClass)->desktop_class.delete_child =
        ((XmShellExtObjectClass) xmDesktopClass)->desktop_class.delete_child;

   if (((XmShellExtObjectClass)baseClassExtRec.secondaryObjectClass)->shell_class.structureNotifyHandler ==
          XmInheritEventHandler)
       ((XmShellExtObjectClass)baseClassExtRec.secondaryObjectClass)->shell_class.structureNotifyHandler =
        ((XmShellExtObjectClass) xmShellExtObjectClass)->shell_class.structureNotifyHandler;

   if (((XmVendorShellExtObjectClass)baseClassExtRec.secondaryObjectClass)->vendor_class.offset_handler ==
          XmInheritProtocolHandler)
       ((XmVendorShellExtObjectClass)baseClassExtRec.secondaryObjectClass)->vendor_class.offset_handler =
        ((XmVendorShellExtObjectClass) xmVendorShellExtObjectClass)->vendor_class.offset_handler;

    _XmProcessUnlock();


   XtFree((char *)uncompiled);

#if XM_MSGCAT
    Xm_catd = catopen("Xm", NL_CAT_LOCALE);
#endif
}


/************************************************************************
 *
 *  ClassPartInitialize
 *    Set up the inheritance mechanism for the routines exported by
 *    vendorShells class part.
 *
 ************************************************************************/
static void
ClassPartInitialize(
        WidgetClass wc )
{
    CompositeWidgetClass	compWc = (CompositeWidgetClass)wc;

    CompositeClassExtensionRec 	**compExtPtr;
    XmBaseClassExt		*wcePtr, *scePtr;
    XmVendorShellWidgetClass    vc = (XmVendorShellWidgetClass) wc;

    CompositeWidgetClass	superWc;
    _XmProcessLock();
    superWc = (CompositeWidgetClass)wc->core_class.superclass;
    _XmProcessUnlock();

    wcePtr = _XmGetBaseClassExtPtr(wc, XmQmotif);

    _XmProcessLock();
    scePtr = _XmGetBaseClassExtPtr(wc->core_class.superclass, XmQmotif);

    if ( (vc != (XmVendorShellWidgetClass)vendorShellWidgetClass) &&
	scePtr && *scePtr &&
	(*wcePtr)->secondaryObjectClass != NULL &&
	((*scePtr)->secondaryObjectClass != (*wcePtr)->secondaryObjectClass)){
	XmVendorShellExtObjectClass  wceClass, sceClass;

	wceClass = (XmVendorShellExtObjectClass)(*wcePtr)->
	    secondaryObjectClass;
	sceClass = (XmVendorShellExtObjectClass)(*scePtr)->
	    secondaryObjectClass;
	_XmBuildExtResources((WidgetClass) (*wcePtr)->secondaryObjectClass);

	if (wceClass->desktop_class.insert_child == XtInheritInsertChild)
	    wceClass->desktop_class.insert_child =
		sceClass->desktop_class.insert_child;

	if (wceClass->desktop_class.delete_child == XtInheritDeleteChild)
	    wceClass->desktop_class.delete_child =
		sceClass->desktop_class.delete_child;

	if (wceClass->shell_class.structureNotifyHandler ==
	    XmInheritEventHandler)
	    wceClass->shell_class.structureNotifyHandler =
		sceClass->shell_class.structureNotifyHandler;

	if (wceClass->vendor_class.offset_handler == XmInheritProtocolHandler)
	    wceClass->vendor_class.offset_handler =
		sceClass->vendor_class.offset_handler;
      }

    compExtPtr = (CompositeClassExtensionRec **)
	&(compWc->composite_class.extension);

    _XmProcessUnlock();

    compExtPtr = (CompositeClassExtensionRec **)
	_XmGetClassExtensionPtr( (XmGenericClassExt *) compExtPtr, NULLQUARK);

    if (*compExtPtr == NULL) {
	CompositeClassExtensionRec 	**superExtPtr;

    _XmProcessLock();
	superExtPtr = (CompositeClassExtensionRec **) &(superWc->composite_class.extension);
    _XmProcessUnlock();

	superExtPtr = (CompositeClassExtensionRec **)
                  _XmGetClassExtensionPtr(
                      (XmGenericClassExt *) superExtPtr,
                      NULLQUARK);

	*compExtPtr = XtNew(CompositeClassExtensionRec);
	memcpy((char*)*compExtPtr, (char*)*superExtPtr,
	       sizeof(CompositeClassExtensionRec));
    }

    /* Do this here because of bug in Xt */
    _XmProcessLock();
    wc->core_class.expose = Redisplay;
    _XmProcessUnlock();

   /* Install the render table trait for all subclasses as well. */
    XmeTraitSet((XtPointer)wc, XmQTspecifyRenderTable, (XtPointer)&vsSRT);

   /* Install the direction trait for all subclasses as well. */
    XmeTraitSet((XtPointer)wc, XmQTspecifyLayoutDirection, (XtPointer)&vsLDT);

   /* Install the accessColors trait for all subclasses as well. */
    XmeTraitSet((XtPointer)wc, XmQTaccessColors, (XtPointer)&vsACT);

   /* Install the unit type trait for all subclasses as well. */
    XmeTraitSet((XtPointer)wc, XmQTspecifyUnitType, (XtPointer)&vsUTT);

}


/************************************************************************
 *
 *  SetMwmStuff
 *     ov will be null when called from Initialize
 *
 ************************************************************************/
static void
SetMwmStuff(
        XmVendorShellExtObject ove,
        XmVendorShellExtObject nve )
{
    Boolean		changed = FALSE;
    Widget		extParent = nve->ext.logicalParent;

    if (!ove || (ove->vendor.mwm_menu != nve->vendor.mwm_menu))
      {
	  /* make mwm_menu local */
	  if (ove && ove->vendor.mwm_menu)
	    XtFree(ove->vendor.mwm_menu);
	  if (nve->vendor.mwm_menu)
	    nve->vendor.mwm_menu = XtNewString(nve->vendor.mwm_menu);
	  if (XtIsRealized(extParent))
	    SetMwmMenu(nve);
      }

    if (!ove || (ove->vendor.mwm_hints.functions != nve->vendor.mwm_hints.functions))
      {
	  if (nve->vendor.mwm_hints.functions == DONT_CARE)
	    nve->vendor.mwm_hints.flags &= ~MWM_HINTS_FUNCTIONS;
	  else
	    nve->vendor.mwm_hints.flags |= MWM_HINTS_FUNCTIONS;
	  changed |= TRUE;
      }

    if (!ove || (ove->vendor.mwm_hints.decorations != nve->vendor.mwm_hints.decorations))
      {
	  if (nve->vendor.mwm_hints.decorations == DONT_CARE)
	    nve->vendor.mwm_hints.flags &= ~MWM_HINTS_DECORATIONS;
	  else
	    nve->vendor.mwm_hints.flags |= MWM_HINTS_DECORATIONS;
	  changed |= TRUE;
      }

    if (!ove || (ove->vendor.mwm_hints.input_mode != nve->vendor.mwm_hints.input_mode))
      {
	  if (nve->vendor.mwm_hints.input_mode == DONT_CARE)
	    nve->vendor.mwm_hints.flags &= ~MWM_HINTS_INPUT_MODE;
	  else
	    nve->vendor.mwm_hints.flags |= MWM_HINTS_INPUT_MODE;
	  changed |= TRUE;
      }

    if (changed && XtIsRealized(extParent))
      SetMwmHints(nve);
}

/* The AddGrab and RemoveGrab routines manage a virtual Xt modal
 * cascade that allows us to remove entries in the list without
 * flushing out the grabs of all following entries.
 */
void
_XmAddGrab(
        Widget wid,
        Boolean exclusive,
        Boolean spring_loaded)
{
  AddGrab( NULL, wid, exclusive, spring_loaded, NULL) ;
}

void
_XmRemoveGrab(
        Widget wid)
{
  RemoveGrab( NULL, FALSE, wid) ;
}

static void
AddGrab(
        XmVendorShellExtObject ve,
        Widget shell,
        Boolean exclusive,
        Boolean springLoaded,
        XmVendorShellExtObject origKid )
{
    Cardinal		     	position;
    XmModalData			modals;
    XmDisplay			xmDisplay;

    if(    shell == NULL    )
      {
        shell = ve->ext.logicalParent ;
      }
    xmDisplay = (XmDisplay) XmGetXmDisplay(XtDisplay(shell));

    modals = xmDisplay->display.modals;

    position = xmDisplay->display.numModals;

    if (xmDisplay->display.numModals == xmDisplay->display.maxModals) {
	/* Allocate more space */
	xmDisplay->display.maxModals +=  (xmDisplay->display.maxModals / 2) + 2;
	xmDisplay->display.modals = modals = (XmModalData)
	  XtRealloc((char *) modals, (unsigned)
		    ((xmDisplay->display.maxModals) * sizeof(XmModalDataRec)));
    }
    modals[position].wid = shell;
    modals[position].ve = ve;
    modals[position].grabber = origKid;
    modals[position].exclusive = exclusive;
    modals[position].springLoaded = springLoaded;
    xmDisplay->display.numModals++;
    XtAddGrab((Widget) shell,
	      exclusive,
	      springLoaded);
#ifdef DEBUG_GRABS
    printf( "AddGrab: XtAddGrab( %s, excl: %d, spring: %d); grabber: %s\n",
                  shell->core.name, exclusive, springLoaded,
                    origKid ? origKid->ext.logicalParent->core.name : "NULL") ;
#endif

    /* If the shell gets destroyed, we don't have to worry about removing
     * the Xt grab, but we do have to remove the ve from the list of modals.
     * Should the client_data be ve or origKid?
     */
    XtAddCallback((Widget) shell, XmNdestroyCallback, RemoveGrabCallback,(XtPointer)ve);
}

/*
 * we add a new argument here, being_destroyed.  If true, it means that
 * we are being called from a callback triggered by the destruction of a
 * shell.  If it is true, we should remove shells from the list of modals,
 * but we should not call XtRemoveGrab on them, because the intrinsics
 * already handle that.
 */
static void
RemoveGrab(
        XmVendorShellExtObject ve,
	Boolean being_destroyed,
	Widget shell)
{
    XmDisplay			xmDisplay;
    Cardinal		     	incr, i, numRemoves, numModals;
    XmModalData			modals;

    if(    !being_destroyed    )
      {
        /* The "shell" argument of this routine is required when the
         * "being_destroyed" argument is TRUE, since the vendor
         * extension record may be already de-allocated before the
         * RemoveGrabCallback is called, and thus cannot be de-referenced
         * to get the shell logical parent.  We will assume that the
         * shell argument is ignored by callers when the "being_destroyed"
         * argument is FALSE.
         */
        if(    shell == NULL    )
          {
            shell = ve->ext.logicalParent ;
          }
        XtRemoveCallback( shell, XmNdestroyCallback, RemoveGrabCallback,
                                                              (XtPointer) ve) ;
      }
#ifdef DEBUG_GRABS
    printf( "\n**** Entering RemoveGrab on %s (0x%x) ****\n",
	    shell->core.name, ve) ;
    PrintXmGrabs( (Widget) shell) ;
#endif

    xmDisplay = (XmDisplay) XmGetXmDisplay(XtDisplay(shell));
    modals = xmDisplay->display.modals;
    numModals = xmDisplay->display.numModals;

    for (i = 0, numRemoves = 0;
	 i < numModals;
	 i++)
      {
	  if(    (modals[i].wid == shell)  &&  (modals[i].ve == ve)    )
	    numRemoves++;
      }
#ifdef DEBUG_GRABS
    printf( "RemoveGrab: numRemoves: %d\n", numRemoves) ;
#endif
    if (numRemoves == 0) return;

    if (!being_destroyed)
       for (i = 0; i < numRemoves; i++)
         {
	   XtRemoveGrab((Widget) shell);
#ifdef DEBUG_GRABS
           printf( "RemoveGrab: XtRemoveGrab( %s)\n",
                                                  ((Widget)shell)->core.name) ;
#endif
         }

    /* Add back all the grabs that were flushed by the removes */
    /*
     ** What this piece of code is trying to do is to iterate over the list,
     ** and for each item to be removed, skip ahead and find a good one to
     ** replace it with. "incr" being set below indicates that a replacement
     ** in fact was found.
     ** However, the code is not obviously replacing items that are to be
     ** discarded; rather, it's simply letting them fall off the end via
     ** the magic of the combination of i, incr, numRemoves, and numModals
     ** incr should probably be absolute w.r.t. the front of the list and
     ** should not double as a marker.
     ** It's easy to know where the first removed item was, and
     ** to add the grabs back in to later widgets.
     ** As is, it's not obvious when modals[i] is to be overwritten.
     */

    for (i = 0, incr = 0;
	 (i + numRemoves) < numModals;
	 i++)
      {
	  /* We remove both the shell that's being pulled off the
	   * cascade and any raw mode shells that we've added. These
	   * should only be the app shells ?!
	   */
	  for (/*EMPTY*/;
	       ((i + incr) < numModals);
	       incr++)
	    {
	        Widget incrWid = modals[i+incr].wid;
		/* if it's not the shell, or it is the shell but some
		   other entry for another grab */
		if ( (incrWid != shell) || (modals[i+incr].ve != ve) )
		  {
		      if  ((ve != NULL) && (modals[i+incr].grabber == ve))
                        {
                          /* Avoid re-adding to the grab list widgets
                           * that were non-exclusives associated with
                           * the shell grab being removed.
                           */
			  numRemoves++;
                        }
		      else
			break;
		  }
	    }
	  if (incr && ((i+incr) < numModals))
	    {
              modals[i] = modals[i+incr];
	      if (!(modals[i].wid->core.being_destroyed))
	      {
		  XtAddGrab( modals[i].wid, modals[i].exclusive,
                                                      modals[i].springLoaded) ;
#ifdef DEBUG_GRABS
		  printf( "RemoveGrab: XtAddGrab( %s, excl: %d, spring: %d)\n",
			  modals[i].wid->core.name,
			  modals[i].exclusive, modals[i].springLoaded) ;
#endif
	      }
	    }
      }
    xmDisplay->display.numModals -= numRemoves ;

#ifdef DEBUG_GRABS
    printf( "\n**** Leaving RemoveGrab on %s (0x%x) ****\n",
	    shell->core.name, ve) ;
    PrintXmGrabs( (Widget) shell) ;
#endif
}

/* ARGSUSED */
static void
RemoveGrabCallback(
	Widget w,
	XtPointer client_data,
	XtPointer call_data )
{
  if(    XmIsVendorShell( w)    )
    {
      /* For VendorShells, defer the removal of the grab until after
       *   all destroy callbacks are completed, since the order of
       *   these callbacks can result in a situation where an
       *   Xt-invoked grab removal can strip "new" grabs which are
       *   installed in the RemoveGrab routine.
       */
      AddDLEntry( (XmVendorShellExtObject) client_data, w) ;
    }
  else
    {
      /* Nested calls to _XmAddGrab of non-VendorShell widgets
       *   are problematic, since the use of the Destroy method
       *   to ensure that the XtRemoveGrabs are completed before
       *   the Xm RemoveGrabs, cannot be employed for these widgets.
       */
      RemoveGrab( (XmVendorShellExtObject) client_data, TRUE, w) ;
    }
}

static void
AddToGrabList(
        Widget  parent,
        Widget  excludedKid,
        Widget  origKid )
{
    Widget		*currKid;
    Widget		*children;
    Cardinal		numChildren;
    Cardinal		i;

    if (!parent)
      return;
    else if (XmIsScreen(parent)) {
	XmScreen	xmScreen = (XmScreen)parent;
	children = xmScreen->desktop.children;
	numChildren = xmScreen->desktop.num_children;
    }
    else if (XmIsDisplay(parent)) {
	XmDisplay	xmDisplay = (XmDisplay)parent;
	children = (Widget *)xmDisplay->composite.children;
	numChildren = xmDisplay->composite.num_children;
    }
    else {
	XmDesktopObject	deskObj = (XmDesktopObject)parent;
	children = (Widget *)deskObj->desktop.children;
	numChildren = deskObj->desktop.num_children;
    }
    for (i = 0, currKid = children; i < numChildren; currKid++, i++) {
	  if (*currKid != excludedKid) {
	      if (!XmIsDisplay(parent)) {
		  ShellWidget shell;
		  shell = (ShellWidget)
		    ((XmDesktopObject)(*currKid))->ext.logicalParent;

		  if(    shell->shell.popped_up
                     || (    XtIsRealized((Widget)shell)
                          && !IsPopupShell( (Widget) shell))    )
		    AddGrab((XmVendorShellExtObject)*currKid, NULL,
			   (Boolean)False, (Boolean)False,
			   (XmVendorShellExtObject)origKid);
	      }
	      else if (!XmIsScreen(*currKid))
		  /*
		   * Don't traverse down non-Screen children of
		   * XmDisplay widgets.
		   */
		  continue;

	      AddToGrabList(*currKid, NULL, origKid);
	  }
      }
}

static void
AddCousinsToGrabList(
        Widget parent,
        Widget excludedKid,
        Widget origKid )
{

    Widget	grandParent;

    if (!parent)
      return;
    else if (XmIsScreen(parent)) {
	XmScreen	xmScreen = (XmScreen)parent;
	grandParent = XtParent(xmScreen);
    }
    else if (XmIsDisplay(parent)) {
	grandParent = NULL;
    }
    else {
	XmDesktopObject	deskObj = (XmDesktopObject)parent;
	grandParent = deskObj->desktop.parent;
    }
    AddToGrabList(parent, excludedKid, origKid);
    AddCousinsToGrabList(grandParent, parent, origKid);
}

static Boolean
IsPopupShell(
        Widget shell)
{
  Widget parent = XtParent( shell) ;

  if(    parent != NULL    )
    {
      Widget *ps_list = parent->core.popup_list ;
      unsigned n_psl = parent->core.num_popups ;

      while(    n_psl--    )
        {
          if(    ps_list[n_psl] == shell    )
            {
              return TRUE ;
            }
        }
    }
  return FALSE ;
}

/************************************************************************
 *
 *     PopupCallback
 *
 ************************************************************************/
/*ARGSUSED*/
static void
PopupCallback(
        Widget shellParent,
        XtPointer closure,
        XtPointer callData )	/* unused */
{
    XmVendorShellExtObject	ve = (XmVendorShellExtObject)closure;
    XtGrabKind			grab_kind = XtGrabNone;
    Boolean			grabCousins = False;
    XmScreen			xmScreen = XmScreenOfObject(shellParent);

    ve->vendor.xAtMap = shellParent->core.x;
    ve->vendor.yAtMap = shellParent->core.y;

    /*
     * work around broken Xt spec ordering for realize and popup callback
     */
    if (!XtIsRealized(shellParent))
      XtRealizeWidget(shellParent);

    /*
     * get the request num + 1 Since it's a map raised. This will
     * only work when the hierarchy is already realized, i.e. after
     * first time
     */

    ve->vendor.lastMapRequest = NextRequest(XtDisplay(shellParent)) + 1;


    switch (ve->vendor.mwm_hints.input_mode)
      {
	case DONT_CARE:
	case MWM_INPUT_MODELESS:
          grab_kind = XtGrabNonexclusive;
          break ;
	case MWM_INPUT_PRIMARY_APPLICATION_MODAL:
	  /*
	   * if we're not running mwm then this becomes full app modal
	   */
	  if (xmScreen->screen.mwmPresent)
	    grabCousins = True;
	  grab_kind = XtGrabExclusive;
	  break;
	case MWM_INPUT_SYSTEM_MODAL:
	case MWM_INPUT_FULL_APPLICATION_MODAL:
	  grab_kind = XtGrabExclusive;
	  break;
	default:
	  break;
      }

    if (grab_kind == XtGrabExclusive)
    {
	Cardinal	numModals;
        XmModalData	modals;
        XmDisplay	xmDisplay;
	int		i;

        xmDisplay = (XmDisplay) XmGetXmDisplay(XtDisplay(shellParent));
        modals = xmDisplay->display.modals;
	numModals = xmDisplay->display.numModals;
        for (i=0; i<numModals; i++)
      	{
	   if (xmMenuShellWidgetClass == XtClass((Widget) modals[i].wid))
	     (*(((XmMenuShellClassRec *)xmMenuShellWidgetClass)->
		 menu_shell_class.popdownOne))(modals[i].wid, NULL, NULL, NULL);
	   else if (xmDragContextClass == XtClass((Widget) modals[i].wid))
	     XmDragCancel((Widget) modals[i].wid);
      	}
    }

    if (grab_kind != XtGrabNone)
      AddGrab(ve, NULL,
	      (grab_kind == XtGrabExclusive),
	      False,
	      ve);

    ve->vendor.grab_kind = grab_kind;

    if (grabCousins)
      AddCousinsToGrabList((Widget)ve->desktop.parent,
			   (Widget)ve,
			   (Widget)ve);
#ifdef DEBUG_GRABS
    printf( "\n**** After popup: ****\n") ;
    PrintXmGrabs( shellParent) ;
#endif
}

/************************************************************************
 *
 *     PopdownCallback
 *
 ************************************************************************/
/*ARGSUSED*/
static void
PopdownCallback(
        Widget shellParent,	/* unused */
        XtPointer closure,
        XtPointer callData )	/* unused */
{
    XmVendorShellExtObject	ve = (XmVendorShellExtObject)closure;

    if (ve->vendor.grab_kind != XtGrabNone)
      RemoveGrab(ve, False, NULL);
#ifdef DEBUG_GRABS
    printf( "\n**** After popdown: ****\n" ) ;
    PrintXmGrabs( shellParent) ;
#endif
}

static Widget
GetNextShell(
        Widget vw )
{
    Widget 	parent;

    parent = XtParent(vw);
    while (parent && !XmIsVendorShell(parent))
      parent = XtParent(parent);

    return parent;
}

/*ARGSUSED*/
static XmDesktopObject
GetShellDesktopParent(
        VendorShellWidget vw,
        ArgList args,		/* unused */
        Cardinal *num_args )	/* unused */
{
	Widget			transientParent = NULL;
	XmDesktopObject		desktopParent = NULL;


	if (vw->wm.transient)
	{
		if (XtIsSubclass((Widget) vw, transientShellWidgetClass))
		{
			TransientShellWidget tw = (TransientShellWidget)vw;

			if (!(transientParent = tw->transient.transient_for))
			{
				tw->transient.transient_for =
					transientParent = GetNextShell( (Widget) vw);
			}
		}
		else
		{
			transientParent = GetNextShell((Widget) vw);
		}
	}

	if (transientParent)
	{
		XmWidgetExtData	extData;

		if (XmIsVendorShell(transientParent))
		{
			extData = _XmGetWidgetExtData(transientParent, XmSHELL_EXTENSION);
            if(extData == NULL)
            {
#ifdef DEBUG
                XmeWarning(NULL, "_XmGetWidgetExtData() returned NULL pointer.");
#endif
                return desktopParent;
            }
            else
            {
    			desktopParent = (XmDesktopObject)extData->widget;
            }
		}
	}
	else if (!XmIsDisplay((Widget)vw))
	{
		desktopParent = (XmDesktopObject)XmScreenOfObject(vw);
	}
	return desktopParent;
}

/************************************************************************
 *
 *     DisplayClosedCallback
 *
 ************************************************************************/
/*ARGSUSED*/
static void
DisplayClosedCallback(
        Widget shellParent,	/* unused */
        XtPointer closure,
        XtPointer callData )	/* unused */
{
  _XmProcessLock();
  _XmDisplayHandle = NULL;
  _XmProcessUnlock();
}


/************************************************************************
 *
 *  SecondaryObjectCreate
 *
 ************************************************************************/
/* ARGSUSED */
static void
SecondaryObjectCreate(
        Widget req,
        Widget new_w,
        ArgList args,
        Cardinal *num_args )
{
    XmBaseClassExt		*pePtr;
    WidgetClass			vec;
    XmDesktopObject		desktopParent;
    Cardinal                    size;
    XtPointer                   newSec, reqSec;
    XmWidgetExtData             extData;

    _XmProcessLock();
    if (!_XmDisplayHandle)
      {
	XmDisplay xmDisplay;

	if ((xmDisplay = (XmDisplay)XmGetXmDisplay(XtDisplay(new_w))) != NULL)
	  XtAddCallback((Widget)xmDisplay, XmNdestroyCallback,
			DisplayClosedCallback, NULL);
      }
    _XmDisplayHandle = XtDisplay(new_w);
    _XmProcessUnlock();

    desktopParent = GetShellDesktopParent( (VendorShellWidget) new_w,
                                                    args, num_args) ;
    if (desktopParent) {

	/*
	 * if the secondary object is using sub_resources then
	 * create it as a child of the shell. Otherwise try to
	 * create it as a sibling in order to fake out resource path
	 */
	_XmProcessLock();
	pePtr = _XmGetBaseClassExtPtr(XtClass(new_w), XmQmotif);
	vec = (*pePtr)->secondaryObjectClass;

    _XmProcessLock();
	size = vec->core_class.widget_size;
    _XmProcessUnlock();

	newSec = XtMalloc(size);
	reqSec = _XmExtObjAlloc(size);
	_XmProcessUnlock();

	extData = (XmWidgetExtData) XtCalloc(1, sizeof(XmWidgetExtDataRec));
	extData->widget = (Widget)newSec;
	extData->reqWidget = (Widget)reqSec;

	((XmVendorShellExtObject)newSec)->ext.extensionType =
	    XmSHELL_EXTENSION;
	((XmVendorShellExtObject)newSec)->ext.logicalParent = new_w;
	((XmVendorShellExtObject)newSec)->desktop.parent =
	    (Widget) desktopParent;

    _XmProcessLock();
	((XmVendorShellExtObject)newSec)->object.widget_class = vec;
    _XmProcessUnlock();

	((XmVendorShellExtObject)newSec)->object.parent = new_w;

	_XmPushWidgetExtData(new_w, extData,
                         ((XmVendorShellExtObject)newSec)->ext.extensionType);

	/*
	 * fetch the resources in superclass to subclass order
	 */
	_XmProcessLock();
	XtGetSubresources(new_w,
			  newSec,
			  NULL, NULL,
			  vec->core_class.resources,
			  vec->core_class.num_resources,
			  args, *num_args );
    _XmProcessUnlock();

	memcpy(reqSec, newSec, size);

	_XmExtImportArgs((Widget)newSec, args, num_args);

	{
	   XtInitProc initialize;
	   _XmProcessLock();
	   initialize = xmDesktopClass->core_class.initialize;
	   _XmProcessUnlock();
	   (*initialize)((Widget)reqSec, (Widget)newSec, args, num_args);
	}
    }
}

/************************************************************************
 *
 *  InitializePrehook
 *
 ************************************************************************/
/* ARGSUSED */
static void
InitializePrehook(
        Widget req,
        Widget new_w,
        ArgList args,
        Cardinal *num_args )
{
    XmBaseClassExt		*cePtr;
    XtInitProc         		secondaryCreate;
    XmToolTipConfigTrait        ttp;           /* ToolTip pointer */
    XmToolTipConfigTraitRec     base;

    ttp = (XmToolTipConfigTrait) XmeTraitGet (new_w, XmQTtoolTipConfig);

    if (ttp == NULL)
    {
        ttp = (XmToolTipConfigTrait) XtMalloc (sizeof (XmToolTipConfigTraitRec));

        ttp->post_delay = 5000;
        ttp->post_duration = 5000;
        ttp->enable = False;
        ttp->timer = 0;
        ttp->duration_timer = 0;
        ttp->leave_time = 0;
        ttp->slider = ttp->label = NULL;

        XtGetSubresources(new_w, &base, NULL, NULL,
 		subresources, XtNumber(subresources),
 		args, *num_args);

        ttp->post_delay = base.post_delay;
        ttp->post_duration = base.post_duration;
        ttp->enable = base.enable;

        XmeTraitSet ((XtPointer) new_w, XmQTtoolTipConfig, (XtPointer) ttp);
    }

    cePtr = _XmGetBaseClassExtPtr(XtClass(new_w), XmQmotif);

    if ((secondaryCreate = (*cePtr)->secondaryObjectCreate) != NULL)
	(*secondaryCreate)(req, new_w, args, num_args);
}


/************************************************************************
 *
 *     VendorExtInitialize
 *
 ************************************************************************/
/*ARGSUSED*/
static void
VendorExtInitialize(
        Widget req,
        Widget new_w,
        ArgList args,		/* unused */
        Cardinal *num_args )	/* unused */
{
    enum { XmA_MOTIF_WM_OFFSET, XmA_MOTIF_WM_MESSAGES,
	   XmAWM_DELETE_WINDOW, NUM_ATOMS };
    static char *atom_names[] = { _XA_MOTIF_WM_OFFSET, _XA_MOTIF_WM_MESSAGES,
	 XmIWM_DELETE_WINDOW };

    XmFontList                  defaultFont;
    XmVendorShellExtObject	ve;
    XmVendorShellExtObject	req_ve;
    XmVendorShellExtObjectClass	vec = (XmVendorShellExtObjectClass) XtClass(new_w);
    Atom			atoms[XtNumber(atom_names)];
    XtCallbackProc		delete_window_handler;
    Widget			extParent;
    XmShellExtObjectClass	sec = (XmShellExtObjectClass) XtClass(new_w);
    XtEventHandler		handler;

    ve  = (XmVendorShellExtObject) new_w;
    req_ve  = (XmVendorShellExtObject) req;

    ve->shell.lastConfigureRequest = 0;

    extParent = ve->ext.logicalParent;

    /* add the handler for tracking whether the hierarchy has focus */

    XtInsertEventHandler(extParent,
			 (EventMask)FocusChangeMask | EnterWindowMask | LeaveWindowMask,
			 FALSE,
			 _XmTrackShellFocus,
			 (XtPointer)new_w,
			 XtListHead);

    _XmProcessLock();
    handler = sec->shell_class.structureNotifyHandler;
    _XmProcessUnlock();

    if (handler)
      {
	  XtInsertEventHandler(extParent,
			       (EventMask) StructureNotifyMask,
			       TRUE,
			       handler,
			       (XtPointer)new_w,
			       XtListHead);
      }
#ifdef DEBUG
    else
      XtError("No structure notify handler for shell");
#endif /* DEBUG */

    ve->vendor.lastOffsetSerial =
      ve->vendor.lastMapRequest = 0;

    ve->vendor.xAtMap =
      ve->vendor.yAtMap =
	ve->vendor.xOffset =
	  ve->vendor.yOffset = 0;

    _XmAddCallback((InternalCallbackList *) &(ve->vendor.realize_callback),
		   VendorExtRealize, NULL);

    ve->vendor.externalReposition = False;
    extParent = ve->ext.logicalParent;

    ve->vendor.focus_data = (XmFocusData) _XmCreateFocusData();

    switch (ve->vendor.delete_response){
      case XmUNMAP:
      case XmDESTROY:
      case XmDO_NOTHING:
	break;
      default:
	XmeWarning(new_w, MSG1);
	ve->vendor.delete_response = XmDESTROY;
    }

    XtAddCallback(extParent, XmNpopupCallback, PopupCallback,(XtPointer)new_w);
    XtAddCallback(extParent, XmNpopdownCallback, PopdownCallback,(XtPointer)new_w);

    assert(XtNumber(atom_names) == NUM_ATOMS);
    XInternAtoms(XtDisplay(extParent), atom_names,
		 XtNumber(atom_names), FALSE, atoms);

    XmAddWMProtocols(extParent, &atoms[XmA_MOTIF_WM_MESSAGES], 1);

    XmAddProtocols(extParent,
		   atoms[XmA_MOTIF_WM_MESSAGES],
		   &atoms[XmA_MOTIF_WM_OFFSET], 1);

    _XmProcessLock();
    XmAddProtocolCallback( extParent,
			  atoms[XmA_MOTIF_WM_MESSAGES],
			  atoms[XmA_MOTIF_WM_OFFSET],
			  vec->vendor_class.offset_handler,
			  (XtPointer) ve);
    _XmProcessUnlock();
    /*
     * add deleteWindow stuff
     */
    XmAddWMProtocols(extParent, &atoms[XmAWM_DELETE_WINDOW], 1);

    /* add a post hook for delete response */

    _XmProcessLock();
    delete_window_handler = vec->vendor_class.delete_window_handler;
    _XmProcessUnlock();

    XmSetWMProtocolHooks( extParent,
			 atoms[XmAWM_DELETE_WINDOW], NULL, NULL,
			 delete_window_handler, (XtPointer) ve);


    /* initialize the old_managed field for focus change tracking */

    ve->vendor.old_managed = NULL;

    /* initialize the mwm_hints flags */
    ve->vendor.mwm_hints.flags = 0;

    SetMwmStuff( NULL, (XmVendorShellExtObject) new_w);

    if ((ve->vendor.focus_policy != XmEXPLICIT) &&
	(ve->vendor.focus_policy != XmPOINTER))
      {
	  ve->vendor.focus_policy = XmEXPLICIT;
      }

    /* initialize input manager resources */

    ve->vendor.input_method_string =
			XtNewString(req_ve->vendor.input_method_string);
    ve->vendor.preedit_type_string =
			XtNewString(req_ve->vendor.preedit_type_string);
    defaultFont =  ve->vendor.button_font_list;
    if ( !defaultFont )
      {
	defaultFont =  ve->vendor.default_font_list; /* backward compatibility */
	if ( !defaultFont )
	  defaultFont = XmeGetDefaultRenderTable( (Widget) extParent, XmBUTTON_FONTLIST);
      }
     ve->vendor.button_font_list = XmFontListCopy (defaultFont);

     defaultFont =  ve->vendor.label_font_list;
     if ( !defaultFont )
       {
	 defaultFont =  ve->vendor.default_font_list; /* backward compatibility */
	 if ( !defaultFont )
	   defaultFont = XmeGetDefaultRenderTable( (Widget) extParent, XmLABEL_FONTLIST);
     }
     ve->vendor.label_font_list = XmFontListCopy (defaultFont);

     defaultFont =  ve->vendor.text_font_list;
     if ( !defaultFont )
       {
	 defaultFont =  ve->vendor.default_font_list; /* backward compatibility */
	 if ( !defaultFont )
	     defaultFont = XmeGetDefaultRenderTable( (Widget) extParent, XmTEXT_FONTLIST);
     }
    ve->vendor.text_font_list = XmFontListCopy (defaultFont);
    ve->vendor.im_height = 0;
    ve->vendor.im_vs_height_set = False;
    ve->vendor.im_info = NULL;
    ve->vendor.label = NULL;
    ve->vendor.slider = NULL;
    ve->vendor.timer = (XtIntervalId)0;
    ve->vendor.duration_timer = (XtIntervalId)0;
    ve->vendor.leave_time = (Time)0;

}

/************************************************************************
 *  MotifWarningHandler
 *    Build up a warning message and print it
 *    Code which used to be directly in _XmWarning.
 *
 ************************************************************************/
static void
MotifWarningHandler (String name,
		     String type,
		     String s_class,
		     String message,
		     String * params,
		     Cardinal* num_params)
{
   char buf[1024], buf2[1024], header[200], *bp, *newline_pos;
   int pos;

   if (!(params && num_params && (*num_params > 0) &&
	 (params[*num_params-1] == XME_WARNING)) &&
       previousWarningHandler) {

     /* We assume it is not coming from our XmeWarning function */
     /* call the previous Warning handler */
     (*previousWarningHandler) (name, type, s_class, message,
				params, num_params);
     return;
   }

   XtGetErrorDatabaseText(name, type, s_class, message, buf2, 1024);
   XtGetErrorDatabaseText("motif", "header", "Motif",
			  _XmMMsgMotif_0000, header, 200);

   sprintf(buf, header, name, s_class);
   if (num_params && *num_params > 1) {
     int i = *num_params-1;
     char *par[10];
     if (i > 10) i = 10;
     memcpy((char*)par, (char*)params, i * sizeof(String));
     bzero((char *)&par[i], (10-i) * sizeof(String));
     (void) sprintf(&buf[strlen(buf)], buf2, par[0], par[1], par[2], par[3],
		    par[4], par[5], par[6], par[7], par[8], par[9]);
   } else
     strcat(buf, buf2);

   pos = 0;
   bp = buf;
   do {
     newline_pos = strchr (bp, '\n');
     if (newline_pos == NULL) {
       strcpy (&buf2[pos], bp);
       pos += strlen (bp);
     } else {
       strncpy (&buf2[pos], bp, (int) (newline_pos - bp + 1));
       pos +=  (int) (newline_pos - bp + 1);
       bp  +=  (int) (newline_pos - bp + 1);
       strcpy (&buf2[pos], "    ");
       pos += 4;
     }
   } while (newline_pos != NULL);

   buf2[pos] = '\n'; buf2[++pos] = '\0';

   XtWarning(buf2);
}



/************************************************************************
 *
 *  Initialize
 *
 ************************************************************************/
/* ARGSUSED */
static void
Initialize(
	 Widget req,
	 Widget new_w,
	 ArgList args,
	 Cardinal *num_args )
{
    VendorShellWidget		vw = (VendorShellWidget)new_w;
    XmWidgetExtData		extData;

    if ((extData = _XmGetWidgetExtData((Widget)vw, XmSHELL_EXTENSION)) != NULL)
      {
	   VendorExtInitialize(extData->reqWidget,
			       extData->widget,
			       args,
			       num_args);
      }

    /* get reasonable defaults for visual, depth and colormap */
    _XmDefaultVisualResources(new_w);

    /* Keep count of the number of VendorShells. When it reaches zero
     * we will destroy the XmDisplay object in Destroy()
     */
    if (!XmIsDisplay(new_w)) {
	 XmDisplay xmDisplay = (XmDisplay) XmGetXmDisplay (XtDisplay(new_w));

	 xmDisplay->display.shellCount += 1;
	 XtAddEventHandler(new_w, (EventMask)0, True, (XtEventHandler)_XEditResCheckMessages, NULL);
    }

    /* install the Motif warning handler that works with XmeWarning */
    _XmProcessLock();
    if (!previousWarningHandler)
      previousWarningHandler =
	XtAppSetWarningMsgHandler(XtWidgetToApplicationContext(new_w),
				  MotifWarningHandler);
    _XmProcessUnlock();

}

/************************************************************************
 *
 *  InitializePosthook
 *
 ************************************************************************/
/* ARGSUSED */
static void
InitializePosthook(
	 Widget req,
	 Widget new_w,
	 ArgList args,
	 Cardinal *num_args )
{
    XmWidgetExtData	ext;

    if ((ext = _XmGetWidgetExtData(new_w, XmSHELL_EXTENSION)) != NULL) {
	 _XmProcessLock();
	 _XmExtObjFree((XtPointer) ext->reqWidget);
	 ext->reqWidget = NULL;
	 _XmProcessUnlock();
	 /* extData gets freed at destroy */
    }
}



/************************************************************************
 *
 *  SetValuesPrehook
 *
 ************************************************************************/
/* ARGSUSED */
static Boolean
SetValuesPrehook(
	 Widget old,
	 Widget ref,
	 Widget new_w,
	 ArgList args,
	 Cardinal *num_args )
{
    XmWidgetExtData	oldExtData, newExtData;
    XmBaseClassExt      *cePtr;
    WidgetClass         ec;
    Cardinal            extSize;

    cePtr = _XmGetBaseClassExtPtr(XtClass(new_w), XmQmotif);
    ec = (*cePtr)->secondaryObjectClass;

    _XmProcessLock();
    extSize = ec->core_class.widget_size;
    _XmProcessUnlock();

    oldExtData = _XmGetWidgetExtData(new_w, XmSHELL_EXTENSION);
    newExtData = (XmWidgetExtData) XtCalloc(1, sizeof(XmWidgetExtDataRec));

    if (oldExtData && newExtData) {
	 _XmPushWidgetExtData(new_w, newExtData, XmSHELL_EXTENSION);

    newExtData->widget = oldExtData->widget;

    _XmProcessLock();
    newExtData->oldWidget = (Widget) _XmExtObjAlloc(extSize);
    memcpy((char *)newExtData->oldWidget, (char *)oldExtData->widget, extSize);
    _XmProcessUnlock();

    _XmProcessLock();
    XtSetSubvalues(oldExtData->widget,
		    ec->core_class.resources,
		    ec->core_class.num_resources,
		    args, *num_args);
    _XmProcessUnlock();

    _XmProcessLock();
    newExtData->reqWidget = (Widget) _XmExtObjAlloc(extSize);
    memcpy((char *)newExtData->reqWidget, (char *)oldExtData->widget, extSize);
    _XmProcessUnlock();

    /*  Convert the necessary fields from unit values to pixel values  */
     _XmProcessLock();
     oldExtData->widget->core.widget_class = ec;
     _XmProcessUnlock();
    _XmExtImportArgs(oldExtData->widget, args, num_args);

    }
    return FALSE;
}

/************************************************************************
 *
 *  SetValues
 *
 ************************************************************************/
/* ARGSUSED */
static Boolean
VendorExtSetValues(
	 Widget old,
	 Widget ref,
	 Widget new_w,
	 ArgList args,
	 Cardinal *num_args )
{
  XmVendorShellExtPartPtr ove, nve;
  XmVendorShellExtObject  ov = (XmVendorShellExtObject) old;
  XmVendorShellExtObject  nv = (XmVendorShellExtObject) new_w;
  XmFontList		  defaultFont;

  ove = (XmVendorShellExtPartPtr) &(ov->vendor);
  nve = (XmVendorShellExtPartPtr) &(nv->vendor);

  switch (nve->delete_response)
    {
    case XmUNMAP:
    case XmDESTROY:
    case XmDO_NOTHING:
      break;
    default:
      XmeWarning(new_w, MSG1);
      nve->delete_response = XmDESTROY;
    }

  if ((nve->focus_policy != XmEXPLICIT) &&
      (nve->focus_policy != XmPOINTER))
    {
      nve->focus_policy = ove->focus_policy;
    }

  if (nve->focus_policy != ove->focus_policy)
    {
      _XmFocusModelChanged( nv->ext.logicalParent, NULL,
			   (XtPointer)(unsigned long)nve->focus_policy );
    }

  SetMwmStuff(ov, nv);

  if (nve->input_method_string != ove->input_method_string)
    {
      XtFree(ove->input_method_string);
      nve->input_method_string = XtNewString(nve->input_method_string);
    }

  if (nve->preedit_type_string != ove->preedit_type_string)
    {
      XtFree(ove->preedit_type_string);
      nve->preedit_type_string = XtNewString(nve->preedit_type_string);
    }

  if (nve->button_font_list != ove->button_font_list)
    {
      XmFontListFree(ove->button_font_list);
      defaultFont = nve->button_font_list;
      if (!defaultFont)
	{
	  defaultFont = nve->default_font_list;
	  if (!defaultFont )
	    defaultFont = XmeGetDefaultRenderTable( (Widget) new_w,
						XmBUTTON_FONTLIST);
	}
      nve->button_font_list = XmFontListCopy (defaultFont);
    }

  if (nve->label_font_list != ove->label_font_list)
    {
      XmFontListFree(ove->label_font_list);
      defaultFont = nve->label_font_list;
      if (!defaultFont)
	{
	  defaultFont = nve->default_font_list;
	  if (!defaultFont )
	    defaultFont = XmeGetDefaultRenderTable( (Widget) new_w,
						XmLABEL_FONTLIST);
	}
      nve->label_font_list = XmFontListCopy (defaultFont);
    }

  if (nve->text_font_list != ove->text_font_list)
    {
      XmFontListFree(ove->text_font_list);
      defaultFont = nve->text_font_list;
      if (!defaultFont)
	{
	  defaultFont = nve->default_font_list;
	  if (!defaultFont )
	    defaultFont = XmeGetDefaultRenderTable( (Widget) new_w,
						XmTEXT_FONTLIST);
	}
      nve->text_font_list = XmFontListCopy (defaultFont);
    }

  if (nve->input_policy != ove->input_policy)
    {
      switch (nve->input_policy)
	{
	case XmPER_SHELL:
	case XmPER_WIDGET:
	  break;
	default:
	  XmeWarning(new_w, MSG2);
	  nve->input_policy = ove->input_policy;
	}
    }

    if (nve->layout_direction != ove->layout_direction)
      {
	XmeWarning(new_w, MSG3);
	nve->layout_direction = ove->layout_direction;
      }

  return FALSE;
}

/************************************************************************
 *
 *  SetValues
 *
 ************************************************************************/
static Boolean
SetValues(
	 Widget current,
	 Widget req,
	 Widget new_w,
	 ArgList args,
	 Cardinal *num_args )
{

    VendorShellWidget		vw = (VendorShellWidget)new_w;
    XmWidgetExtData		extData;
    XmVendorShellExtObject	vse;
    int i;
    XmToolTipConfigTrait ttp;

    ttp = (XmToolTipConfigTrait) XmeTraitGet (new_w, XmQTtoolTipConfig);
    if (ttp != NULL)
    {
        _XmProcessLock ();
        for (i = 0; i < *num_args; i++)
        {
            if (strcmp (args[i].name, XmNtoolTipPostDelay) == 0)
            {
                ttp->post_delay = args[i].value;
            }
            else if (strcmp (args[i].name, XmNtoolTipPostDuration) == 0)
            {
                ttp->post_duration = args[i].value;
            }
            else if (strcmp (args[i].name, XmNtoolTipEnable) == 0)
            {
                ttp->enable = (Boolean) args[i].value;
            }
        }
        _XmProcessUnlock ();
    }

    if ((extData = _XmGetWidgetExtData( (Widget) vw, XmSHELL_EXTENSION)) &&
	 (vse = (XmVendorShellExtObject) extData->widget))
      {
	   VendorExtSetValues(extData->oldWidget,
			      extData->reqWidget,
			      extData->widget,
			      args,
			      num_args);

	     vse = (XmVendorShellExtObject) extData->widget;
	     if (req->core.height != current->core.height)
		 vse->vendor.im_vs_height_set = True;

      }

	 return(FALSE);
}


/************************************************************************
 *
 *  SetValuesPosthook
 *
 ************************************************************************/
/*ARGSUSED*/
static Boolean
SetValuesPosthook(
	 Widget current,	/* unused */
	 Widget req,		/* unused */
	 Widget new_w,
	 ArgList args,		/* unused */
	 Cardinal *num_args )	/* unused */
{
    XmWidgetExtData	ext;

    _XmPopWidgetExtData(new_w, &ext, XmSHELL_EXTENSION);

    if (ext) {
	 _XmProcessLock();
	 _XmExtObjFree( (XtPointer) ext->reqWidget);
	 _XmExtObjFree( (XtPointer) ext->oldWidget);
	 _XmProcessUnlock();
	 XtFree((char *) ext);
    }
	 return(FALSE);
}


/************************************************************************
 *
 *  GetValuesPrehook
 *
 ************************************************************************/
/*ARGSUSED*/
static void
GetValuesPrehook(
	 Widget w,
	 ArgList args,		/* unused */
	 Cardinal *num_args )	/* unused */
{
    XmWidgetExtData	oldExtData, newExtData;

    if ((oldExtData = _XmGetWidgetExtData(w, XmSHELL_EXTENSION)) != NULL) {
	 newExtData = (XmWidgetExtData) XtCalloc(1, sizeof(XmWidgetExtDataRec));
	 newExtData->widget = oldExtData->widget;
	 _XmPushWidgetExtData(w, newExtData, XmSHELL_EXTENSION);
    }
}

static void
GetValuesHook(
	 Widget w,
	 ArgList args,
	 Cardinal *num_args )
{
    XmWidgetExtData	ext;
    XmBaseClassExt      *cePtr;
    WidgetClass         ec;
    int i;
    int *ip;
    Boolean *bp;
    XmToolTipConfigTrait ttp;

    ttp = (XmToolTipConfigTrait) XmeTraitGet (w, XmQTtoolTipConfig);

    if (ttp != NULL)
    {
        _XmProcessLock ();
        for (i = 0; i < *num_args; i++)
        {
            if (strcmp (args[i].name, XmNtoolTipPostDelay) == 0)
            {
                ip = (int *) args[i].value;
                *ip = ttp->post_delay;
            }
            else if (strcmp (args[i].name, XmNtoolTipPostDuration) == 0)
            {
                ip = (int *) args[i].value;
                *ip = ttp->post_duration;
            }
            else if (strcmp (args[i].name, XmNtoolTipEnable) == 0)
            {
                bp = (Boolean *) args[i].value;
                *bp = ttp->enable;
            }
        }
        _XmProcessUnlock ();
    }

    cePtr = _XmGetBaseClassExtPtr(XtClass(w), XmQmotif);
    ec = (*cePtr)->secondaryObjectClass;

    if ((ext = _XmGetWidgetExtData(w, XmSHELL_EXTENSION)) != NULL)
    {
      _XmProcessLock();
      XtGetSubvalues(ext->widget,
		      ec->core_class.resources,
		      ec->core_class.num_resources,
		      args, *num_args);
      _XmProcessUnlock();
      _XmExtGetValuesHook( ext->widget, args, num_args );
    }

}

/*ARGSUSED*/
static void
GetValuesPosthook(
	 Widget w,
	 ArgList args,		/* unused */
	 Cardinal *num_args )	/* unused */
{
    XmWidgetExtData	ext = NULL;

    _XmPopWidgetExtData(w, &ext, XmSHELL_EXTENSION);
    if (ext)
      XtFree((char *) ext);
}

/*
 * This handles the case where the secondary shells is waiting for the
 * primary to get mapped and is destroyed in the interim.
 */
/*ARGSUSED*/
static void
PendingTransientDestroyed(
	 Widget vw,
	 XtPointer cl_data,
	 XtPointer ca_data )	/* unused */
{
	 XmExtObject ancestorExtObj = (XmExtObject) cl_data ;
	 Widget ancestor = ancestorExtObj->ext.logicalParent ;

    if(    !ancestor->core.being_destroyed    )
    {
/*
	 XtRemoveCallback( (Widget) ancestorExtObj, XmNrealizeCallback,
					      SetTransientFor, (XtPointer) vw) ;
*/
        _XmRemoveCallback((InternalCallbackList *) &(((XmVendorShellExtObject)ancestorExtObj)->vendor.realize_callback),
			  SetTransientFor, (XtPointer) vw) ;
	 }
    return ;
    }

/*
 * Handle having the application shell realized after the secondary shells
 */
/*ARGSUSED*/
static void
SetTransientFor(
	 Widget w,
	 XtPointer closure,
	 XtPointer call_data)	/* unused */
{
	 VendorShellWidget vw = (VendorShellWidget) closure ;
	 Widget ancestor = ((XmExtObject) w)->ext.logicalParent ;
	 Arg args[2] ;
	 Cardinal i = 0 ;

    if(    !XtIsRealized( ancestor)    )
    {
	 XtRealizeWidget( ancestor) ;
	 }
    XtSetArg( args[i], XtNwindowGroup, XtWindow( ancestor)) ; i++ ;

    if(    XtIsTransientShell( (Widget) vw)    )
    {
	 /* because Shell.c is broken force the code */
	 ((TransientShellWidget) vw)->transient.transient_for = NULL ;

	 XtSetArg( args[i], XtNtransientFor, ancestor) ; i++ ;
	 }
    XtSetValues( (Widget) vw, args, i) ;
/*
    XtRemoveCallback( w, XmNrealizeCallback, SetTransientFor, (XtPointer) vw) ;
*/

    _XmRemoveCallback((InternalCallbackList *) &(((XmVendorShellExtObject)w)->vendor.realize_callback),
			   SetTransientFor, (XtPointer) vw) ;

    XtRemoveCallback( (Widget) vw, XmNdestroyCallback,
				     PendingTransientDestroyed, (XtPointer) w) ;
    return ;
    }


/************************************************************************
 *
 *  Resize
 *
 ************************************************************************/
static void
Resize(
	 Widget w )
{
    ShellWidget sw = (ShellWidget)w;
    Widget childwid;
    int i;
    int y;
    XmVendorShellExtObject vendorExt;
    XmWidgetExtData	extData;

    extData = _XmGetWidgetExtData((Widget)sw, XmSHELL_EXTENSION);
if(extData == NULL)
{
#ifdef DEBUG
        XmeWarning(NULL, "_XmGetWidgetExtData() returned NULL pointer.");
#endif
}
    vendorExt = (XmVendorShellExtObject) extData->widget;

    _XmImResize((Widget)sw);
    y = sw->core.height - vendorExt->vendor.im_height;
    for(i = 0; i < sw->composite.num_children; i++) {
	 if(XtIsManaged(sw->composite.children[i])) {
	     childwid = sw->composite.children[i];
	     XmeConfigureObject(childwid,
				childwid->core.x, childwid->core.y,
				sw->core.width, y,
				childwid->core.border_width);
	 }
    }
}

/************************************************************************
 *
 *  ChangeManaged
 *
 ************************************************************************/
static void
ChangeManaged(
	 Widget wid )
{
    VendorShellWidget vw = (VendorShellWidget) wid ;
    WMShellWidgetClass	super = (WMShellWidgetClass)wmShellWidgetClass;
    Widget		firstManaged = NULL;
    Cardinal		i;
    XmVendorShellExtObject vendorExt;
    XmWidgetExtData	extData;
    XtWidgetProc 	change_managed;

    extData = _XmGetWidgetExtData((Widget)vw, XmSHELL_EXTENSION);

if(extData == NULL)
{
#ifdef DEBUG
        XmeWarning(NULL, "_XmGetWidgetExtData() returned NULL pointer.");
#endif
}

    vendorExt = (XmVendorShellExtObject) extData->widget;

    for (i= 0; i < vw->composite.num_children; i++)
      if (XtIsManaged(vw->composite.children[i]))
	 firstManaged = vw->composite.children[i];

    /* Danger! Danger! Ugly Code Warning! Since the shell's
     * change Managed routine in the intrinsics will always
     * configure the child to be the size of the parent, if
     * there is any im_height we must subtract it here and
     * add it back in after this call.
     */

    vw->core.height -= vendorExt->vendor.im_height;
    _XmProcessLock();
    change_managed = super->composite_class.change_managed;
    _XmProcessUnlock();
    (*change_managed) ((Widget) vw);
    vw->core.height += vendorExt->vendor.im_height;

    /*
     * make sure that there is a reasonable initial focus path. This
     * is especially important for making sure the event handler is
     * there.
     */
    XtSetKeyboardFocus((Widget)vw, (Widget)firstManaged);

    XmeNavigChangeManaged((Widget)vw);
}

static void
UpdateCoreGeometry(
	 VendorShellWidget vw,
	 XmVendorShellExtObject vendorExt )
{
	 /* ||| check if geometry was user specified and convert if it was */
	 if (vw->shell.geometry && vendorExt)
	 {
		 if (vendorExt->vendor.unit_type != XmPIXELS)
		 {
			 if (vw->wm.size_hints.flags & USPosition)
			 {
				 vw->core.x = (Position) XmCvtToHorizontalPixels(
					 vw->core.screen, (int) vw->core.x,
					 vendorExt->vendor.unit_type);
				 vw->core.y = (Position) XmCvtToVerticalPixels(
					 vw->core.screen, (int) vw->core.y,
					 vendorExt->vendor.unit_type);
			 }
			 if (vw->wm.size_hints.flags & USSize)
			 {
				 vw->core.width = (Dimension) XmCvtToHorizontalPixels(
					 vw->core.screen, (int) vw->core.width,
					 vendorExt->vendor.unit_type);
				 vw->core.height = (Dimension) XmCvtToVerticalPixels(
					 vw->core.screen, (int) vw->core.height,
					 vendorExt->vendor.unit_type);
			 }
		 }
	 }
}

/************************************************************************
 *
 *  Realize
 *
 ************************************************************************/
static void
Realize(
	 Widget wid,
	 XtValueMask *vmask,
	 XSetWindowAttributes *attr )
{
    VendorShellWidget vw = (VendorShellWidget) wid ;
    WMShellWidgetClass	super = (WMShellWidgetClass)wmShellWidgetClass;
    XmVendorShellExtObject vendorExt;
    XmWidgetExtData	extData;

    if ((extData = _XmGetWidgetExtData((Widget)vw, XmSHELL_EXTENSION)) != NULL)
      {
	 vendorExt = (XmVendorShellExtObject) extData->widget;
	 _XmImChangeManaged((Widget)vw);
      }
    else
      vendorExt = NULL;

    UpdateCoreGeometry(vw, vendorExt);

    /*
     * Set nearest shell as transientFor so Mwm will be able to build tree.
     */
    if (vendorExt && XmIsShellExt(vendorExt->desktop.parent))
      {
	   Widget ancestor = ((XmExtObject)(vendorExt->desktop.parent))
							   ->ext.logicalParent ;
	   /* try to have WMShell do the work */
	   if (XtIsRealized(ancestor))
	     vw->wm.wm_hints.window_group = XtWindow(ancestor);
	   else
	     {
	       XmWidgetExtData ancestorExtData = _XmGetWidgetExtData( ancestor,
							    XmSHELL_EXTENSION) ;
	       if(    ancestorExtData && ancestorExtData->widget    )
		 {
		   _XmAddCallback((InternalCallbackList *) &(((XmVendorShellExtObject)ancestorExtData->widget)->vendor.realize_callback),
		    SetTransientFor,
		      (XtPointer) vw);

		   XtAddCallback( (Widget) vw, XmNdestroyCallback,
					PendingTransientDestroyed,
					  (XtPointer) ancestorExtData->widget) ;
		 }
	     }
      }

    /*	Make sure height and width are not zero, without warning, since
	 we change the internal behavior of BB and DA and we don't want
	 to change the external one.
    */
   if (!XtWidth(wid)) XtWidth(wid) = 1 ;
   if (!XtHeight(wid)) XtHeight(wid) = 1 ;

    /* Make my superclass do all the dirty work */
    {
	XtRealizeProc realize;
	_XmProcessLock();
	realize = super->core_class.realize;
	_XmProcessUnlock();
    	(*realize)((Widget) vw, vmask, attr);
    }
    if (vendorExt)
      _XmImRealize((Widget)vw);
}

/************************************************************************
 *
 *  GeometryManager
 *
 ************************************************************************/
/*ARGSUSED*/
static XtGeometryResult
GeometryManager(
	 Widget wid,
	 XtWidgetGeometry *request,
	 XtWidgetGeometry *reply ) /* unused */
{
    ShellWidget 	shell = (ShellWidget)(wid->core.parent);
    XtWidgetGeometry 	my_request;
    XmVendorShellExtObject ve;
    XmWidgetExtData   extData;
    XtGeometryResult res ;

    if (!(extData = _XmGetWidgetExtData((Widget)shell,
					 XmSHELL_EXTENSION)))
      return XtGeometryNo;

    ve = (XmVendorShellExtObject) extData->widget;

    if(!(shell->shell.allow_shell_resize) && XtIsRealized(wid) &&
	(request->request_mode & (CWWidth | CWHeight | CWBorderWidth)))
      return(XtGeometryNo);

    my_request.request_mode = 0;
    /* %%% worry about XtCWQueryOnly */
    if (request->request_mode & XtCWQueryOnly)
      my_request.request_mode |= XtCWQueryOnly;

    if (request->request_mode & CWWidth) {
	 my_request.width = request->width;
	 my_request.request_mode |= CWWidth;
    }
    if (request->request_mode & CWHeight) {
	 my_request.height = request->height + ve->vendor.im_height;
	 my_request.request_mode |= CWHeight;
    }
    if (request->request_mode & CWBorderWidth) {
	 my_request.border_width = request->border_width;
	 my_request.request_mode |= CWBorderWidth;
    }

  if (request->request_mode & CWX) {
    my_request.x = request->x;
    my_request.request_mode |= CWX;
  }
  if (request->request_mode & CWY) {
    my_request.y = request->y;
    my_request.request_mode |= CWY;
  }

    res = XtMakeGeometryRequest((Widget)shell, &my_request, NULL) ;

    if (res == XtGeometryYes)
      {
	   _XmImResize((Widget)shell);
	   if (!(request->request_mode & XtCWQueryOnly))
	     {
		 if (request->request_mode & CWWidth)
			 wid->core.width = shell->core.width;
		 if (request->request_mode & CWHeight)
			 wid->core.height = shell->core.height -
						 ve->vendor.im_height;
		   if (request->request_mode & CWX)
			       wid->core.x = 0;
		   if (request->request_mode & CWY)
			       wid->core.y = 0;
	     }
	   return XtGeometryYes;
      }
    else
      return XtGeometryNo;
}


/************************************************************************
 *
 *  RootGeometryManager
 *
 ************************************************************************/
/*ARGSUSED*/
static XtGeometryResult
RootGeometryManager(
	 Widget w,
	 XtWidgetGeometry *request,
	 XtWidgetGeometry *reply )
{
    XtGeometryHandler	wmGeoHandler;
    ShellWidgetClass	swc = (ShellWidgetClass)wmShellWidgetClass;
    ShellClassExtensionRec **scExtPtr;
    XtGeometryResult	returnVal = XtGeometryNo;
    WMShellWidget	wmShell = (WMShellWidget)w;
    XmShellExtObject	se;

    XmWidgetExtData	extData = _XmGetWidgetExtData(w, XmSHELL_EXTENSION);

    if(extData == NULL)
    {
#ifdef DEBUG
        XmeWarning(NULL, "_XmGetWidgetExtData() returned NULL pointer.");
#endif
        return XtGeometryNo;
    }

    se = (XmShellExtObject)extData->widget;


    if (se)
      {
	   se->shell.lastConfigureRequest = NextRequest(XtDisplay(w));
      }
#ifdef DEBUG
    else
      XtError("no extension object");
#endif /* DEBUG */

    _XmProcessLock();
    scExtPtr = (ShellClassExtensionRec **)
                    _XmGetClassExtensionPtr(
                        (XmGenericClassExt *) &(swc->shell_class.extension),
                        NULLQUARK);
    _XmProcessUnlock();

    if (request->request_mode & XtCWQueryOnly)
      {
	   if (!(wmShell->shell.allow_shell_resize) &&
	       (request->request_mode &
		(CWWidth | CWHeight | CWBorderWidth)))
	     return XtGeometryNo;
	   /*
	    * we should switch on useAsyncGeometry but we won't |||
	    */
	   else
	     return XtGeometryYes;
      }

    if (se->shell.useAsyncGeometry)
      {
	   /* make wait_for_wm = FALSE to force desired behaviour */
	   wmShell->wm.wait_for_wm = FALSE;
	   /* FIX for 1684: remove the timeout = 0 line, not
	      needed and introduced a bug if not saved/restore together
	      with useAsync change - wait_for_wm will be later reset
	      by Shell in Xt */
      }
    _XmProcessLock();
    wmGeoHandler = (*scExtPtr)->root_geometry_manager;
    _XmProcessUnlock();
    if (wmGeoHandler != NULL)
      {
	   returnVal =  (*wmGeoHandler)(w, request, reply);
	   if (se->shell.useAsyncGeometry) {
	       /* X configure was sent to the server, while this is happening,
		  let's everybody think it's a success (which is true
		  most of the time): set the shell size to what it wants
		  to be and return Yes */
	       if (request->request_mode & CWWidth)
		   w->core.width = request->width;
	       if (request->request_mode & CWHeight)
		   w->core.height = request->height ;
	       if (request->request_mode & CWBorderWidth)
		   w->core.border_width = request->border_width ;
	       if (request->request_mode & CWX)
		   w->core.x = request->x;
	       if (request->request_mode & CWY)
		   w->core.y = request->y;

	       returnVal = XtGeometryYes;
	   }
      }
    return returnVal;
}

/************************************************************************
 *
 *  SetMwmHints
 *
 ************************************************************************/
static void
SetMwmHints(
	 XmVendorShellExtObject ve )
{
    PropMwmHints	prop;
    Atom		mwm_hints_atom;
    Widget		shell = ve->ext.logicalParent;

    mwm_hints_atom = XInternAtom(XtDisplay(shell),
				   _XA_MWM_HINTS,
				   FALSE);

#define SET(field) prop.field = ve->vendor.mwm_hints.field
    SET(flags);
    SET(functions);
    SET(decorations);
    prop.inputMode = ve->vendor.mwm_hints.input_mode;
    SET(status);
#undef SET

    XChangeProperty (XtDisplay(shell),
		      XtWindow(shell),
		      mwm_hints_atom,mwm_hints_atom,
		      32, PropModeReplace,
		      (unsigned char *) &prop, PROP_MWM_HINTS_ELEMENTS);
}




/************************************************************************
 *
 *  SetMwmMenu
 *
 ************************************************************************/
static void
SetMwmMenu(
	 XmVendorShellExtObject ve )
{
    Widget	  shell = ve->ext.logicalParent;
    Atom	  mwm_menu_atom;
    XTextProperty text_prop;
    int		  status;


    mwm_menu_atom = XInternAtom(XtDisplay(shell),
				_XA_MWM_MENU,
				FALSE);

    text_prop.value = NULL;
    status = XmbTextListToTextProperty(XtDisplay(shell),
				       &ve->vendor.mwm_menu, 1,
				       XStdICCTextStyle,
				       &text_prop);

    if (status == Success || status > 0)
      {
	XSetTextProperty(XtDisplay(shell), XtWindow(shell),
			 &text_prop, mwm_menu_atom);

	if (text_prop.value != NULL) XFree((char*)text_prop.value);
      }
}


/*ARGSUSED*/
static void
VendorExtRealize(
	 Widget w,
	 XtPointer closure,	/* unused */
	 XtPointer call_data )	/* unused */
{
    XmVendorShellExtObject	ve = (XmVendorShellExtObject)w;
    VendorShellWidget		vw;

    vw = (VendorShellWidget)ve->ext.logicalParent;
    if (ve->vendor.mwm_hints.flags)
      SetMwmHints(ve);

    if (ve->vendor.mwm_menu)
      SetMwmMenu(ve);

    _XmInstallProtocols(ve->ext.logicalParent);

    if(    !IsPopupShell( (Widget) vw)    )
      {
	 /* Non-popup shells are allowed input as soon as
	  * they are realized, since they don't use XtPopup.
	  */
	 AddGrab(ve, NULL, FALSE, FALSE, ve);
      }
}

static void
AddDLEntry(
	 XmVendorShellExtObject ve,
	 Widget shell)
{
  unsigned short i = 0 ;

  _XmProcessLock();
  while(    i < destroy_list_cnt    )
    {
      if(    destroy_list[i].shell == shell    )
	 {
	   /* Already on list; once is enough.
	   */
	   _XmProcessUnlock();
	   return ;
	 }
      ++i ;
    }
  if(    destroy_list_cnt == destroy_list_size    )
    {
      destroy_list_size += 2 ;
      destroy_list = (XmDestroyGrabList) XtRealloc( (char *) destroy_list,
				destroy_list_size * sizeof( XmDestroyGrabRec)) ;
    }
  destroy_list[i].shell = shell ;
  destroy_list[i].ve = ve ;
  ++destroy_list_cnt ;
  _XmProcessUnlock();
}

static void
RemoveDLEntry(
	 unsigned pos)
{
  _XmProcessLock();
  while(    ++pos < destroy_list_cnt    )
    {
      destroy_list[pos-1].shell = destroy_list[pos].shell ;
      destroy_list[pos-1].ve = destroy_list[pos].ve ;
    }
  --destroy_list_cnt ;
  _XmProcessUnlock();
}

static void
Destroy(
	 Widget wid)
{
  XmWidgetExtData ext;
  XmVendorShellExtObject ve;
  unsigned short n = 0 ;

  _XmProcessLock();
  while(    n < destroy_list_cnt    )
  {
    if(    destroy_list[n].shell == wid    )
      {
	 RemoveGrab( destroy_list[n].ve, TRUE, destroy_list[n].shell) ;
	 RemoveDLEntry( n) ;
	 break ;
      }
    ++n ;
  }
  _XmProcessUnlock();

    _XmPopWidgetExtData(wid, &ext, XmSHELL_EXTENSION);
    if (ext != NULL) {
	 if ((ve = (XmVendorShellExtObject) ext->widget) != NULL) {
	     if (ve->vendor.mwm_menu)
		 XtFree(ve->vendor.mwm_menu);
	     if (ve->vendor.input_method_string)
		 XtFree(ve->vendor.input_method_string);
	     if (ve->vendor.preedit_type_string)
		 XtFree(ve->vendor.preedit_type_string);
	     if (ve->vendor.button_font_list)
		 XmFontListFree(ve->vendor.button_font_list);
	     if (ve->vendor.label_font_list)
		 XmFontListFree(ve->vendor.label_font_list);
	     if (ve->vendor.text_font_list)
		 XmFontListFree(ve->vendor.text_font_list);
	     if (ve->vendor.im_info)
	         _XmImFreeShellData(wid, &ve->vendor.im_info);

	     _XmDestroyFocusData(ve->vendor.focus_data);

	     _XmRemoveAllCallbacks((InternalCallbackList *)
				 &(ve->vendor.realize_callback));
	     _XmRemoveAllCallbacks((InternalCallbackList *)
				 &(ve->vendor.focus_moved_callback));

         {
         XtWidgetProc destroy;
         _XmProcessLock();
         destroy = xmDesktopClass->core_class.destroy;
         _XmProcessUnlock();
	     (*destroy) ((Widget) ve);
         }
	     XtFree((char *) ve);
	 }
	 XtFree((char *) ext);
    }

    {
        XmToolTipConfigTrait        ttp;           /* ToolTip pointer */

        if ((ttp = XmeTraitGet((XtPointer) wid, XmQTtoolTipConfig)) != NULL)
        {
            XmeTraitSet((XtPointer) wid, XmQTtoolTipConfig, (XtPointer) NULL);
            XtFree((char *)ttp);
        }
    }

    /*
     * If all VendorShells have been destroyed, destroy the XmDisplay object
     * in order to reset any per-display and per-screen data. This is
     * necessary, since the application may be about to call XtCloseDisplay.
     */
    if (!XmIsDisplay (wid)) {
	 XmDisplay xmDisplay = (XmDisplay) XmGetXmDisplay (XtDisplay(wid));

	 xmDisplay->display.shellCount -= 1;
	 if (xmDisplay->display.shellCount == 0) {
	     XmImCloseXIM (wid);
	     XtDestroyWidget ((Widget)xmDisplay);
	 }
    }
}

/*ARGSUSED*/
static void
Redisplay(
	 Widget wid,
	 XEvent *event,		/* unused */
	 Region region )	/* unused */
{
    _XmImRedisplay(wid);
}


/****************************************************************
 *
 * Trait method for specify render table
 *
 **************************************************************/
static XmFontList
GetTable(
	  Widget wid,
	  XtEnum type)
{
    XmWidgetExtData   extData;
    XmVendorShellExtObject ve;

    if ((extData = _XmGetWidgetExtData(wid, XmSHELL_EXTENSION)) &&
	(ve = (XmVendorShellExtObject) extData->widget))
	{
	    switch(type) {
	    case XmLABEL_RENDER_TABLE : return ve->vendor.label_font_list ;
	    case XmBUTTON_RENDER_TABLE : return ve->vendor.button_font_list ;
	    case XmTEXT_RENDER_TABLE : return ve->vendor.text_font_list ;
	    }
	}

    return NULL ;
}

/****************************************************************
 *
 * Trait method for specify layout direction
 *
 **************************************************************/
static XmDirection
GetDirection(Widget w)
{
  XmWidgetExtData extData;
  XmVendorShellExtObject vendorExt;

  extData = _XmGetWidgetExtData(w, XmSHELL_EXTENSION);

  if(extData == NULL)
  {
#ifdef DEBUG
    XmeWarning(NULL, "_XmGetWidgetExtData() returned NULL pointer.");
#endif
    return XmRIGHT_TO_LEFT;
  }

  vendorExt = (XmVendorShellExtObject) extData->widget;
  return vendorExt->vendor.layout_direction;
}

static unsigned char
GetUnitType(Widget w)
{
    XmWidgetExtData extData;
    XmVendorShellExtObject vendorExt;

    if ((extData = _XmGetWidgetExtData(w, XmSHELL_EXTENSION)) &&
	(vendorExt = (XmVendorShellExtObject) extData->widget))
	return vendorExt->vendor.unit_type ;
    else
	/* cannot fetch unit type yet, the VendorShell is
	   being created, so force pixel... */
	return XmPIXELS ;
}


static void
GetColors(Widget w,
	  XmAccessColorData color_data)
{
    /* cannot use XmGetColors to generate the other for
       it ends up in a loop since XmGetColors creates a XmScreen,
       which needs an XmDisplay, which is a VendorS. */

   color_data->valueMask = AccessForeground | AccessBackgroundPixel |
       AccessHighlightColor | AccessTopShadowColor |
	   AccessBottomShadowColor ;
   color_data->background = w->core.background_pixel;
   color_data->foreground = BlackPixelOfScreen(XtScreen(w)) ;
   color_data->highlight_color =
       color_data->top_shadow_color =
	   color_data->bottom_shadow_color = XmUNSPECIFIED_PIXEL ;
}



/************************************************************************
 *
 *  _XmGetDefaultDisplay
 *
 ************************************************************************/
Display *
_XmGetDefaultDisplay(void)
{
    Display *theDisplay = NULL;

    _XmProcessLock();
    if (_XmDisplayHandle)
	theDisplay =  _XmDisplayHandle ;
    else {
      XtWarning(MSG4);
    }
    _XmProcessUnlock();

    return theDisplay;
}

/************************************************************************
 *
 *  _XmDefaultVisualResources
 *
 ************************************************************************/
void
_XmDefaultVisualResources(
        Widget widget )
{
   /* Default the Shell visual resources (visual, depth and colormap)
      from the nearest shell. See CR 5459 */

    ShellWidget next_shell, this_shell = (ShellWidget)widget;

    /* go find the next shell ancestor, which might be widget itself
       in the case of a root shell. Looking for the next widget with
       a "visual" resource is too expensive, remember this is going to be done
       99.999999% of the time */
    next_shell = (ShellWidget) this_shell;
    if (XtParent((Widget)next_shell))
	do {
	    next_shell = (ShellWidget) XtParent((Widget)next_shell);
	} while (!XtIsShell((Widget)next_shell));

    /* now default the 3 visual resources */
    if (this_shell->shell.visual == INVALID_VISUAL) {
	/* next_shell might be self, still invalid, but in this case,
	 we know it is a root shell: take regular Xt default */

	if (this_shell == next_shell) {
	    this_shell->shell.visual = CopyFromParent ;
	    this_shell->core.depth = DefaultDepthOfScreen(XtScreen(widget));
	    this_shell->core.colormap = DefaultColormapOfScreen(XtScreen(widget));
	} else {
	    this_shell->shell.visual = next_shell->shell.visual;
	    this_shell->core.depth = next_shell->core.depth;
	    this_shell->core.colormap = next_shell->core.colormap;
	}
    }

}


#ifdef DEBUG_GRABS

static void
PrintModal(
        XmModalData modal)
{
  printf( "log_p: %s, grabber: %s, excl: %d, spring: %d, ve: 0x%x\n",
	  modal->wid->core.name,
	  (modal->grabber
	     ? modal->grabber->ext.logicalParent->core.name : "NULL"),
	  modal->exclusive,
          modal->springLoaded,
	  modal->ve) ;
}

static void
PrintXmGrabs(
        Widget wid)
{
  XmDisplay disp = (XmDisplay) XmGetXmDisplay( XtDisplay( wid));
  XmModalData modals = disp->display.modals ;
  Cardinal nmodals = disp->display.numModals ;
  Cardinal cnt = 0 ;

  while(    cnt < nmodals    )
    {
      PrintModal( &(modals[cnt])) ;
      ++cnt ;
    }
}
#endif /* DEBUG_GRABS */

