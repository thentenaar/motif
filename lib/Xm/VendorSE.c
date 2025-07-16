/*
 * Motif
 *
 * Copyright (c) 1987-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$TOG: VendorSE.c /main/21 1997/10/13 14:58:18 cshi $"
#endif
#endif

/* Make sure all wm properties can make it out of the resource manager */

#include <stdlib.h>
#include <string.h>
#include <Xm/Xm.h>		/* To make cpp on Sun happy. CR 5943 */
#include <X11/IntrinsicP.h>
#include <X11/ShellP.h>
#include <Xm/AtomMgr.h>
#include <Xm/BaseClassP.h>
#include <Xm/LayoutT.h>
#include <Xm/RepType.h>
#include <Xm/TraitP.h>
#include <Xm/VendorSEP.h>
#include "BaseClassI.h"
#include "CallbackI.h"
#include "MessagesI.h"
#include "ResIndI.h"
#include "SyntheticI.h"
#include "TraversalI.h"
#include "VendorSEI.h"
#include "XmI.h"

#define NOTVENDORSHELL	_XmMMsgProtocols_0000

#define DONT_CARE	-1L
#define BIGSIZE		((Dimension)32767)


/********    Static Function Declarations    ********/

static Boolean CvtStringToHorizontalInt(
                        Display *dpy,
                        XrmValue *args,
                        Cardinal *num_args,
                        XrmValue *from_val,
                        XrmValue *toVal,
                        XtPointer *data) ;
static Boolean CvtStringToVerticalInt(
                        Display *dpy,
                        XrmValue *args,
                        Cardinal *num_args,
                        XrmValue *from_val,
                        XrmValue *toVal,
                        XtPointer *data) ;
static void ClassInitialize( void ) ;
static void ClassPartInitialize(
                        WidgetClass w) ;
static void DeleteWindowHandler(
                        Widget wid,
                        XtPointer closure,
                        XtPointer call_data) ;
static void OffsetHandler(
                        Widget shell,
                        XtPointer clientData,
                        XtPointer cd) ;
static void InitializePrehook(
                        Widget req,
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args) ;
static void Destroy(
                        Widget wid) ;
static void GetMWMFunctionsFromProperty(
                        Widget wid,
                        int resource_offset,
                        XtArgVal *value) ;
static void CheckSetRenderTables(Widget wid,
				int offset,
				XrmValue *value);

/********    End Static Function Declarations    ********/


static XtConvertArgRec selfConvertArgs[] = {
    { XtBaseOffset, (XtPointer) 0, sizeof(int) }
};



/***************************************************************************
 *
 * Vendor shell class record
 *
 ***************************************************************************/

#define Offset(field) XtOffsetOf( struct _XmVendorShellExtRec, vendor.field)

static XtResource extResources[] =
{
    {
	XmNextensionType,
	XmCExtensionType, XmRExtensionType, sizeof (unsigned char),
	XtOffsetOf( struct _XmExtRec, ext.extensionType),
	XmRImmediate, (XtPointer)XmSHELL_EXTENSION,
    },
    {
	XmNdefaultFontList,
	XmCDefaultFontList, XmRFontList, sizeof (XmFontList),
	Offset (default_font_list),
	XmRImmediate, (XtPointer) NULL,
    },
    {
        "pri.vate",
        "Pri.vate", XmRUnsignedChar, sizeof (unsigned char),
        Offset (mapStyle),
	XmRImmediate, 0
    },
    {
        XmNbuttonFontList,
        XmCButtonFontList, XmRButtonFontList, sizeof (XmFontList),
        Offset (button_font_list),
	XmRCallProc, (XtPointer)CheckSetRenderTables,
    },
    {
        XmNlabelFontList,
        XmCLabelFontList, XmRLabelFontList, sizeof (XmFontList),
        Offset (label_font_list),
	XmRCallProc, (XtPointer)CheckSetRenderTables,
    },
    {
        XmNtextFontList,
        XmCTextFontList, XmRTextFontList, sizeof (XmFontList),
        Offset (text_font_list),
	XmRCallProc, (XtPointer)CheckSetRenderTables,
    },
    {
        XmNbuttonRenderTable,
        XmCButtonRenderTable, XmRButtonRenderTable, sizeof (XmRenderTable),
        Offset (button_font_list),
	XmRCallProc, (XtPointer)CheckSetRenderTables,
    },
    {
        XmNlabelRenderTable,
        XmCLabelRenderTable, XmRLabelRenderTable, sizeof (XmRenderTable),
        Offset (label_font_list),
	XmRCallProc, (XtPointer)CheckSetRenderTables,
    },
    {
        XmNtextRenderTable,
        XmCTextRenderTable, XmRTextRenderTable, sizeof (XmRenderTable),
        Offset (text_font_list),
	XmRCallProc, (XtPointer)CheckSetRenderTables,
    },
    {
	XmNaudibleWarning, XmCAudibleWarning, XmRAudibleWarning,
	sizeof (Boolean), Offset (audible_warning),
	XmRImmediate, (XtPointer) XmBELL,
    },
    {
	XmNshellUnitType, XmCShellUnitType, XmRUnitType,
	sizeof (unsigned char), Offset (unit_type),
	XmRImmediate, (XtPointer) XmPIXELS,
    },
    {
	XmNdeleteResponse, XmCDeleteResponse,
	XmRDeleteResponse, sizeof(unsigned char),
	Offset(delete_response),
	XmRImmediate, (XtPointer) XmDESTROY,
    },
    {
	XmNinputPolicy, XmCInputPolicy,
	XmRInputPolicy, sizeof(XmInputPolicy),
	Offset(input_policy),
	XmRImmediate, (XtPointer) XmPER_SHELL,
    },
    {
	XmNkeyboardFocusPolicy, XmCKeyboardFocusPolicy, XmRKeyboardFocusPolicy,
	sizeof(unsigned char),
	Offset(focus_policy),
	XmRImmediate, (XtPointer)XmEXPLICIT,
    },
    {
	XmNmwmDecorations, XmCMwmDecorations, XmRInt,
	sizeof(int), Offset(mwm_hints.decorations),
	XmRImmediate, (XtPointer) DONT_CARE,
    },
    {
	XmNmwmFunctions, XmCMwmFunctions, XmRInt,
	sizeof(int), Offset(mwm_hints.functions),
	XmRImmediate, (XtPointer) DONT_CARE,
    },
    {
	XmNmwmInputMode, XmCMwmInputMode, XmRInt,
	sizeof(int), Offset(mwm_hints.input_mode),
	XmRImmediate, (XtPointer) DONT_CARE,
    },
    {
	XmNmwmMenu, XmCMwmMenu, XmRString,
	sizeof(String), Offset(mwm_menu),
	XmRImmediate, (XtPointer) NULL,
    },
    {
	XmNfocusMovedCallback, XmCCallback, XmRCallback,
	sizeof(XtCallbackList), Offset(focus_moved_callback),
	XmRImmediate, NULL,
    },
    {
	XmNrealizeCallback, XmCCallback, XmRCallback,
	sizeof(XtCallbackList), Offset(realize_callback),
	XmRImmediate, NULL,
    },
    {
	XmNinputMethod, XmCInputMethod, XmRString,
	sizeof(String), Offset(input_method_string),
	XmRImmediate, NULL,
    },
    {
	XmNpreeditType, XmCPreeditType, XmRString,
	sizeof(String), Offset(preedit_type_string),
	XmRImmediate, "OverTheSpot,OffTheSpot,Root,OnTheSpot",
    },
    {
      XmNlightThreshold, XmCLightThreshold, XmRInt,
      sizeof(unsigned int), Offset(light_threshold),
      XmRImmediate, NULL,
    },
    {
      XmNdarkThreshold, XmCDarkThreshold, XmRInt,
      sizeof(unsigned int), Offset(dark_threshold),
      XmRImmediate, NULL,
    },
    {
      XmNforegroundThreshold, XmCForegroundThreshold, XmRInt,
      sizeof(unsigned int), Offset(foreground_threshold),
      XmRImmediate, NULL,
    },
    {
      XmNlayoutDirection, XmCLayoutDirection, XmRDirection,
      sizeof(XmDirection), Offset(layout_direction),
      XmRImmediate, (XtPointer) XmLEFT_TO_RIGHT,
    },
    /* add a synonym to ShellUnitType */
    {
	XmNunitType, XmCUnitType, XmRUnitType,
	sizeof (unsigned char), Offset (unit_type),
	XmRImmediate, (XtPointer) XmPIXELS,
    },
    {
        XmNverifyPreedit, XmCVerifyPreedit, XmRBoolean,
        sizeof (Boolean), Offset (verify_preedit),
        XmRImmediate, (XtPointer) False,
    },
};
#undef Offset

/*  Definition for resources that need special processing in get values  */

#define ParentOffset(x) 	\
	(XtOffsetOf(VendorShellRec, x) | XmLOGICAL_PARENT_RESOURCE)
#define ExtOffset(x)		XtOffsetOf(XmVendorShellExtRec, x)

static XmSyntheticResource synResources[] =
{
    {
	XmNx, sizeof (Position),
	ParentOffset (core.x),
	XmeFromHorizontalPixels,
	XmeToHorizontalPixels,
    },
    {
	XmNy, sizeof (Position),
	ParentOffset (core.y),
	XmeFromVerticalPixels,
	XmeToVerticalPixels,
    },
    {
	XmNwidth, sizeof (Dimension),
	ParentOffset (core.width),
	XmeFromHorizontalPixels,
	XmeToHorizontalPixels,
    },
    {
	XmNheight, sizeof (Dimension),
	ParentOffset (core.height),
	XmeFromVerticalPixels,
	XmeToVerticalPixels,
    },
    {
	XmNborderWidth, sizeof (Dimension),
	ParentOffset (core.border_width),
	XmeFromHorizontalPixels,
	XmeToHorizontalPixels,
    },

/* size_hints minus things stored in core */

    {
	XmNminWidth, sizeof(int),
	ParentOffset(wm.size_hints.min_width),
	XmeFromHorizontalPixels,
	XmeToHorizontalPixels,
    },
    {
	XmNminHeight, sizeof(int),
	ParentOffset(wm.size_hints.min_height),
	XmeFromVerticalPixels,
	XmeToVerticalPixels,
    },
    {
	XmNmaxWidth, sizeof(int),
	ParentOffset(wm.size_hints.max_width),
	XmeFromHorizontalPixels,
	XmeToHorizontalPixels,
    },
    {
	XmNmaxHeight,sizeof(int),
	ParentOffset(wm.size_hints.max_height),
	XmeFromVerticalPixels,
	XmeToVerticalPixels,
    },

/* wm_hints */

    {
	XmNiconX, sizeof(int),
	ParentOffset(wm.wm_hints.icon_x),
	XmeFromHorizontalPixels,
	XmeToHorizontalPixels,
    },
    {
	XmNiconY, sizeof(int),
	ParentOffset(wm.wm_hints.icon_y),
	XmeFromVerticalPixels,
	XmeToVerticalPixels,
    },
    {
	XmNmwmFunctions, sizeof(int),
	ExtOffset(vendor.mwm_hints.functions),
	GetMWMFunctionsFromProperty,
	(XmImportProc)NULL,
    },
};

#undef ParentOffset
#undef ExtOffset

static XmBaseClassExtRec myBaseClassExtRec = {
    NULL,                               /* Next extension         */
    NULLQUARK,                          /* record type XmQmotif   */
    XmBaseClassExtVersion,              /* version                */
    sizeof(XmBaseClassExtRec),          /* size                   */
    InitializePrehook,		        /* initialize prehook     */
    XmInheritSetValuesPrehook,	        /* set_values prehook     */
    (XtInitProc)NULL,		        /* initialize posthook    */
    (XtSetValuesFunc)NULL,	        /* set_values posthook    */
    NULL,				/* secondary class        */
    (XtInitProc)NULL,		        /* creation proc          */
    (XmGetSecResDataFunc)NULL,          /* getSecRes data         */
    {0},                                /* fast subclass          */
    XmInheritGetValuesPrehook,	        /* get_values prehook     */
    (XtArgsProc)NULL,		        /* get_values posthook    */
    XmInheritClassPartInitPrehook,	/* class_part_prehook     */
    XmInheritClassPartInitPosthook,     /* class_part_posthook    */
    NULL,	 			/* compiled_ext_resources */
    NULL,	 			/* ext_resources       	  */
    0,					/* resource_count     	  */
    TRUE,				/* use_sub_resources	  */
};

externaldef(xmvendorshellextclassrec)
XmVendorShellExtClassRec xmVendorShellExtClassRec = {
    {
	(WidgetClass) &xmShellExtClassRec,/* superclass		*/
	"VendorShell",			/* class_name 		*/
	sizeof(XmVendorShellExtRec), 	/* size 		*/
	ClassInitialize, 		/* Class Initializer 	*/
	ClassPartInitialize,	        /* class_part_init 	*/
	FALSE, 				/* Class init'ed ? 	*/
	(XtInitProc)NULL,		/* initialize         	*/
	(XtArgsProc)NULL, 		/* initialize_notify    */
	NULL,	 			/* realize            	*/
	NULL,		 		/* actions            	*/
	0,				/* num_actions        	*/
	extResources, 			/* resources          	*/
	XtNumber(extResources),		/* resource_count     	*/
	NULLQUARK, 			/* xrm_class          	*/
	FALSE, 				/* compress_motion    	*/
	FALSE, 				/* compress_exposure  	*/
	FALSE, 				/* compress_enterleave	*/
	FALSE, 				/* visible_interest   	*/
	Destroy,			/* destroy            	*/
	NULL,	 			/* resize             	*/
	NULL, 				/* expose             	*/
	(XtSetValuesFunc)NULL,		/* set_values         	*/
	(XtArgsFunc)NULL, 		/* set_values_hook      */
	NULL,	 			/* set_values_almost    */
	(XtArgsProc)NULL,		/* get_values_hook      */
	NULL,				/* accept_focus       	*/
	XtVersion, 			/* intrinsics version 	*/
	NULL, 				/* callback offsets   	*/
	NULL,				/* tm_table           	*/
	NULL, 				/* query_geometry       */
	NULL,				/* display_accelerator  */
	(XtPointer)&myBaseClassExtRec,	/* extension            */
    },
    {					/* ext			*/
	synResources,			/* synthetic resources	*/
	XtNumber(synResources),		/* num syn resources	*/
	NULL,				/* extension		*/
    },
    {					/* desktop		*/
	NULL,				/* child_class		*/
	XtInheritInsertChild,		/* insert_child		*/
	XtInheritDeleteChild,		/* delete_child		*/
	NULL,				/* extension		*/
    },
    {					/* shell ext		*/
	XmInheritEventHandler,		/* structureNotify	*/
	NULL,				/* extension		*/
    },
    {					/* vendor ext		*/
	DeleteWindowHandler,            /* delete window handler*/
	OffsetHandler,	                /* offset handler	*/
	NULL,				/* extension		*/
    },
};

externaldef(xmVendorShellExtobjectclass) WidgetClass
       xmVendorShellExtObjectClass = (WidgetClass) (&xmVendorShellExtClassRec);

static Boolean
CvtStringToHorizontalInt(
        Display *display,
        XrmValue *args,
        Cardinal *num_args,	/* unused */
        XrmValue *from,
        XrmValue *to,
        XtPointer *converter_data)
{
    Widget widget = *(Widget*) args[0].addr ;
    Screen * screen = XtScreen(widget) ;
    unsigned char defaultFromType = _XmGetUnitType(widget) ;
    int tmpPix;
    Boolean parseError;

    tmpPix = (int)
      _XmConvertStringToUnits (screen, from->addr, (int) defaultFromType,
			       XmHORIZONTAL, XmPIXELS, (XtEnum*) &parseError);
    if (parseError)
        {
        XtDisplayStringConversionWarning(display, (char *)from->addr,
					 XmRHorizontalDimension);
        return False;
        }
    else
        _XM_CONVERTER_DONE( to, int, tmpPix, ; )
}

static Boolean
CvtStringToVerticalInt(
        Display *display,
        XrmValue *args,
        Cardinal *num_args,	/* unused */
        XrmValue *from,
        XrmValue *to,
        XtPointer *converter_data)
{
    Widget widget = *(Widget*) args[0].addr ;
    Screen * screen = XtScreen(widget) ;
    unsigned char defaultFromType = _XmGetUnitType(widget) ;
    int tmpPix;
    Boolean parseError;

    tmpPix = (int)
	_XmConvertStringToUnits(screen, from->addr, (int) defaultFromType,
				XmVERTICAL, XmPIXELS, (XtEnum*) &parseError);
    if (parseError)
	{
            XtDisplayStringConversionWarning(display, (char *)from->addr,
					     XmRVerticalPosition);
            return False;
	}
    else
	_XM_CONVERTER_DONE( to, int, tmpPix, ; )

}



/************************************************************************
 *
 *  ClassInitialize
 *    Set up the converters for VendorShell int pixels
 *
 ************************************************************************/
static void
ClassInitialize( void )
{
    XtSetTypeConverter(XmRString,
		       XmRHorizontalInt,
		       CvtStringToHorizontalInt,
		       selfConvertArgs,
		       XtNumber(selfConvertArgs),
		       XtCacheNone, (XtDestructor)NULL);
    XtSetTypeConverter(XmRString,
		       XmRVerticalInt,
		       CvtStringToVerticalInt,
		       selfConvertArgs,
		       XtNumber(selfConvertArgs),
		       XtCacheNone, (XtDestructor)NULL);

    myBaseClassExtRec.record_type = XmQmotif;
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
        WidgetClass w )
{
    XmVendorShellExtObjectClass wc = (XmVendorShellExtObjectClass) w;
    XmVendorShellExtObjectClass sc =
      (XmVendorShellExtObjectClass) wc->object_class.superclass;

    if (wc == (XmVendorShellExtObjectClass)xmVendorShellExtObjectClass)
      return;

    if (wc->vendor_class.delete_window_handler == XmInheritProtocolHandler)
      wc->vendor_class.delete_window_handler =
	sc->vendor_class.delete_window_handler;

    if (wc->vendor_class.offset_handler == XmInheritProtocolHandler)
      wc->vendor_class.offset_handler =
	sc->vendor_class.offset_handler;
}



/************************************************************************
 *  DeleteWindowHandler
 *
 ************************************************************************/
static void
DeleteWindowHandler(
        Widget wid,
        XtPointer closure,
        XtPointer call_data )	/* unused */
{
        VendorShellWidget w = (VendorShellWidget) wid ;
    XmVendorShellExtObject ve = (XmVendorShellExtObject) closure;

    switch(ve->vendor.delete_response)
      {
	case XmUNMAP:
	  if (w->shell.popped_up)
	    XtPopdown((Widget) w);
	  else
	    XtUnmapWidget((Widget) w);
	  break;

	case XmDESTROY:
	  if (XtIsApplicationShell((Widget) w))
	    {
		XtDestroyApplicationContext
		  (XtWidgetToApplicationContext((Widget) w));
		exit(0);
	    }
	  else
	    XtDestroyWidget((Widget) w);
	  break;

	case XmDO_NOTHING:
	default:
	  break;
      }
}


/************************************************************************
 *
 *     OffsetHandler
 *
 ************************************************************************/
static void
OffsetHandler(
        Widget shell,		/* unused */
        XtPointer clientData,
        XtPointer cd )
{
        XmAnyCallbackStruct *callData = (XmAnyCallbackStruct *) cd ;
    XClientMessageEvent		*offsetEvent;
    XmVendorShellExtObject	ve = (XmVendorShellExtObject)clientData;

    offsetEvent = (XClientMessageEvent *) callData->event;

    ve->vendor.lastOffsetSerial = offsetEvent->serial;
    ve->vendor.xOffset = (Position) offsetEvent->data.l[1];
    ve->vendor.yOffset = (Position) offsetEvent->data.l[2];
}


/************************************************************************
 *
 *     InitializePrehook
 *
 ************************************************************************/
static void
InitializePrehook(
        Widget req,
        Widget new_w,
        ArgList args,
        Cardinal *num_args )
{
    XmExtObjectClass		ec = (XmExtObjectClass) XtClass(new_w);
    XmBaseClassExt		*wcePtr;
    XmExtObject			ne = (XmExtObject) new_w;
    Widget			parent = ne->ext.logicalParent;
    XmExtObjectClass		pec = (XmExtObjectClass) XtClass(parent);
    XmBaseClassExt		*pcePtr;
    XmWidgetExtData		extData;

    wcePtr = _XmGetBaseClassExtPtr(ec, XmQmotif);
    pcePtr = _XmGetBaseClassExtPtr(pec, XmQmotif);

    if ((*wcePtr)->use_sub_resources)
      {
          _XmProcessLock();
	  /*
	   * get a uncompiled resource list to use with
	   * XtGetSubresources. We can't do this in
	   * ClassPartInitPosthook because Xt doesn't set class_inited at
	   * the right place and thereby mishandles the
	   * XtGetResourceList call
	   */
	  if ((*wcePtr)->ext_resources == NULL)
	    {
		ec->object_class.resources =
		  (*wcePtr)->compiled_ext_resources;
		ec->object_class.num_resources =
		  (*wcePtr)->num_ext_resources;

		XtGetResourceList((WidgetClass) ec,
				  &((*wcePtr)->ext_resources),
				  &((*wcePtr)->num_ext_resources));

	    }
	  if ((*pcePtr)->ext_resources == NULL)
	    {
		XtGetResourceList((WidgetClass) pec,
				  &((*pcePtr)->ext_resources),
				  &((*pcePtr)->num_ext_resources));
	    }
	  XtGetSubresources(parent,
			    (XtPointer)new_w,
			    NULL, NULL,
			    (*wcePtr)->ext_resources,
			    (*wcePtr)->num_ext_resources,
			    args, *num_args);

	  extData = (XmWidgetExtData) XtCalloc(1, sizeof(XmWidgetExtDataRec));
	  _XmPushWidgetExtData(parent, extData, ne->ext.extensionType);

	  extData->widget = new_w;
	  extData->reqWidget = (Widget)
	    XtMalloc(XtClass(new_w)->core_class.widget_size);
	  memcpy( extData->reqWidget, req,
		XtClass(new_w)->core_class.widget_size);

	  /*  Convert the fields from unit values to pixel values  */

	  XtGetSubresources(parent,
			    (XtPointer)parent,
			    NULL, NULL,
			    (*pcePtr)->ext_resources,
			    (*pcePtr)->num_ext_resources,
			    args, *num_args);

	  _XmExtImportArgs(new_w, args, num_args);
	  _XmProcessUnlock();
      }
}


/************************************************************************
 *
 *  Destroy
 *
 *    This needs to be in the ext object because the extension gets
 *    blown away before the primary does since it's a child. Otherwise
 *    we'd have it in the primary.
 *
 ************************************************************************/
static void
Destroy(
        Widget wid )
{
    XmVendorShellExtObject ve = (XmVendorShellExtObject) wid ;
    if (ve->vendor.mwm_menu)
      XtFree(ve->vendor.mwm_menu);
    if (ve->vendor.input_method_string)
      XtFree(ve->vendor.input_method_string);
    if (ve->vendor.preedit_type_string)
      XtFree(ve->vendor.preedit_type_string);
    if (ve->vendor.label)
	XtDestroyWidget(ve->vendor.label);
    if (ve->vendor.slider)
	XtDestroyWidget(ve->vendor.slider);
    if (ve->vendor.timer)
	XtRemoveTimeOut(ve->vendor.timer);
    if (ve->vendor.duration_timer)
	XtRemoveTimeOut(ve->vendor.duration_timer);
    _XmDestroyFocusData(ve->vendor.focus_data);
}

/*
 * XmRCallProc routine for checking VendorShell render table resources
 * before setting them to NULL if no value is specified
 * for both XmN<foo>renderTable and XmN<foo>fontList.
 * If the appropriate bit in "mapStyle" has been set, then the
 * function has been called twice on same widget and resource offset, thus
 * resource needs to be set NULL, otherwise leave it alone.
 */
static void
CheckSetRenderTables(Widget wid,
		     int offset,
		     XrmValue *value )
{
  XmVendorShellExtObject ve;
  XmWidgetExtData extData;

#define SET_BFL(state) (state |= 0x01)
#define IS_SET_BFL(state) (state & 0x01)
#define SET_LFL(state) (state |= 0x02)
#define IS_SET_LFL(state) (state & 0x02)
#define SET_TFL(state) (state |= 0x04)
#define IS_SET_TFL(state) (state & 0x04)

  extData = _XmGetWidgetExtData(wid, XmSHELL_EXTENSION);

if(extData == NULL)
{
#ifdef DEBUG
        XmeWarning(NULL, "_XmGetWidgetExtData() returned NULL pointer.");
#endif
        return;
}

  ve = (XmVendorShellExtObject)(extData->widget);

  if (((char *)ve + offset) == (char *) &(ve->vendor.button_font_list)) {
	if (IS_SET_BFL(ve->vendor.mapStyle))
		value->addr = NULL;
	else {
		SET_BFL(ve->vendor.mapStyle);
		value->addr = ((char *)ve + offset);
	}
  }
  else if (((char *)ve + offset) == (char *) &(ve->vendor.label_font_list)) {
	if (IS_SET_LFL(ve->vendor.mapStyle))
		value->addr = NULL;
	else {
		SET_LFL(ve->vendor.mapStyle);
		value->addr = ((char *)ve + offset);
	}
  }
  else if (((char *)ve + offset) == (char *) &(ve->vendor.text_font_list)) {
	if (IS_SET_TFL(ve->vendor.mapStyle))
		value->addr = NULL;
	else {
		SET_TFL(ve->vendor.mapStyle);
		value->addr = ((char *)ve + offset);
	}
  }
}

/************************************************************************
 *
 *  _XmGetAudibleWarning
 *       This function is called by a widget to get the audibleWarning
 *   value. This is done by checking to see if any of the widgets,
 *   in the widget's parent hierarchy is a subclass of VendorShell widget
 *   class, and if it is, returning the  VendorShell resource value.
 *   If no VendorShell is found, returns XmBELL, since it is the default
 *   value for this resource.
 *************************************************************************/
unsigned char
_XmGetAudibleWarning(Widget w)
{
  XmWidgetExtData extData ;
  XmVendorShellExtObject vendorExt;

  while (w) {
    if (XmIsVendorShell (w))
      {
	extData = _XmGetWidgetExtData(w, XmSHELL_EXTENSION);

        if(extData == NULL)
          {
#ifdef DEBUG
            XmeWarning(NULL, "_XmGetWidgetExtData() returned NULL pointer.");
#endif
            return XmBELL;
        }


	vendorExt = (XmVendorShellExtObject) extData->widget;
	return vendorExt->vendor.audible_warning;
      }
    else
      w = XtParent(w);
  }
  return (XmBELL);
}

/****************************************************************/
static void
GetMWMFunctionsFromProperty(
        Widget wid,
        int resource_offset,	/* unused */
        XtArgVal *value )
{
  Atom actual_type;
  int actual_format;
  unsigned long num_items, bytes_after;
  PropMwmHints *prop = NULL ;
  XmVendorShellExtObject ve = (XmVendorShellExtObject) wid ;
  Widget shell = ve->ext.logicalParent ;
  Atom mwm_hints_atom ;

  if(    !XtIsRealized( shell)    )
    {
      *value = (XtArgVal) ve->vendor.mwm_hints.functions ;
      return ;
    }
  mwm_hints_atom = XInternAtom( XtDisplay( shell), _XA_MWM_HINTS, FALSE);
  XGetWindowProperty( XtDisplay( shell), XtWindow( shell), mwm_hints_atom, 0,
		     (long) PROP_MWM_HINTS_ELEMENTS, FALSE, mwm_hints_atom,
		     &actual_type, &actual_format, &num_items, &bytes_after,
		     (unsigned char **) &prop);
  if(    (actual_type != mwm_hints_atom)
     ||  (actual_format != 32)
     ||  (num_items < PROP_MWM_HINTS_ELEMENTS)
     ||  (prop == NULL)    )
    {
      if(    prop != NULL    )
	{
	  XFree( (char *)prop) ;
	}
      *value = (XtArgVal) ve->vendor.mwm_hints.functions ;
      return ;
    }
  *value = (XtArgVal) prop->functions ;
  XFree( (char *) prop) ;
}


/******* XmeFunctions ********/

void
XmeAddFocusChangeCallback(Widget w,
			  XtCallbackProc proc,
			  XtPointer data)
{
  XmWidgetExtData extData ;
  XmVendorShellExtObject vendorExt;
  _XmWidgetToAppContext(w);

  _XmAppLock(app);

  if (XmIsVendorShell(w) == False) {
    XmeWarning(w, NOTVENDORSHELL);
    _XmAppUnlock(app);
    return;
  }

  extData = _XmGetWidgetExtData(w, XmSHELL_EXTENSION);

  if(extData == NULL)
  {
#ifdef DEBUG
    XmeWarning(NULL, "_XmGetWidgetExtData() returned NULL pointer.");
#endif
    _XmAppUnlock(app);
    return;
  }

  vendorExt = (XmVendorShellExtObject) extData->widget;

  _XmAddCallback((InternalCallbackList *)
		 &(vendorExt->vendor.focus_moved_callback), proc, data);
  _XmAppUnlock(app);
}

void
XmeRemoveFocusChangeCallback(Widget w,
			     XtCallbackProc proc,
			     XtPointer data)
{
  XmWidgetExtData extData ;
  XmVendorShellExtObject vendorExt;
  _XmWidgetToAppContext(w);

  _XmAppLock(app);

  if (XmIsVendorShell(w) == False) {
    XmeWarning(w, NOTVENDORSHELL);
    _XmAppUnlock(app);
    return;
  }

  extData = _XmGetWidgetExtData(w, XmSHELL_EXTENSION);

  if(extData == NULL)
  {
#ifdef DEBUG
    XmeWarning(NULL, "_XmGetWidgetExtData() returned NULL pointer.");
#endif
    _XmAppUnlock(app);
    return;
  }

  vendorExt = (XmVendorShellExtObject) extData->widget;

  _XmRemoveCallback((InternalCallbackList *)
		    &(vendorExt->vendor.focus_moved_callback), proc, data);
  _XmAppUnlock(app);
}
