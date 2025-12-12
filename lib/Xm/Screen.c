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
#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$TOG: Screen.c /main/16 1997/06/18 17:41:50 samborn $"
#endif
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include <stdio.h>
#include <X11/Xatom.h>
#include <Xm/Xm.h>		/* To make cpp on Sun happy. CR 5943 */
#include <Xm/AtomMgr.h>
#include <Xm/DisplayP.h>
#include "DragIconI.h"
#include "HashI.h"
#include "ImageCachI.h"
#include "MessagesI.h"
#include "RepTypeI.h"
#include "ScreenI.h"
#include "PixConvI.h"

#define DEFAULT_QUAD_WIDTH 10

#define RESOURCE_DEFAULT  (-1)

#define MESSAGE1	_XmMMsgScreen_0000
#define MESSAGE2        _XmMMsgScreen_0001


/********    Static Function Declarations    ********/

static void ClassPartInitialize(
                        WidgetClass wc) ;
static void ClassInitialize( void ) ;
static void GetUnitFromFont(
                        Display *display,
                        XFontStruct *fst,
                        int *ph_unit,
                        int *pv_unit) ;
static void Initialize(
                        Widget requested_widget,
                        Widget new_widget,
                        ArgList args,
                        Cardinal *num_args) ;
static Boolean SetValues(
                        Widget current,
                        Widget requested,
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args) ;
static void Destroy(
                        Widget widget) ;
static void InsertChild(
                        Widget wid) ;
static void DeleteChild(
                        Widget wid) ;
static Boolean MatchPixmap(XmHashKey, XmHashKey) ;
static XmHashValue HashPixmap(XmHashKey) ;
static Boolean FreePixmap(XmHashKey k, XtPointer p, XtPointer client_data) ;

/********    End Static Function Declarations    ********/

#define Offset(x) (XtOffsetOf(XmScreenRec, x))

static XtResource resources[] = {
    {
	XmNdarkThreshold, XmCDarkThreshold, XmRInt,
	sizeof(int), Offset(screen.darkThreshold),
	XmRImmediate, (XtPointer)NULL,
    },
    {
	XmNlightThreshold, XmCLightThreshold, XmRInt,
	sizeof(int), Offset(screen.lightThreshold),
	XmRImmediate, (XtPointer)NULL,
    },
    {
	XmNforegroundThreshold, XmCForegroundThreshold, XmRInt,
	sizeof(int), Offset(screen.foregroundThreshold),
	XmRImmediate, (XtPointer)NULL,
    },
    {
	XmNdefaultNoneCursorIcon, XmCDefaultNoneCursorIcon, XmRWidget,
	sizeof(Widget), Offset(screen.defaultNoneCursorIcon),
	XmRImmediate, (XtPointer)NULL,
    },
    {
	XmNdefaultValidCursorIcon, XmCDefaultValidCursorIcon,
	XmRWidget, sizeof(Widget), Offset(screen.defaultValidCursorIcon),
	XmRImmediate, (XtPointer)NULL,
    },
    {
	XmNdefaultInvalidCursorIcon, XmCDefaultInvalidCursorIcon, XmRWidget,
	sizeof(Widget), Offset(screen.defaultInvalidCursorIcon),
	XmRImmediate, (XtPointer)NULL,
    },
    {
	XmNdefaultMoveCursorIcon, XmCDefaultMoveCursorIcon, XmRWidget,
	sizeof(Widget), Offset(screen.defaultMoveCursorIcon),
	XmRImmediate, (XtPointer)NULL,
    },
    {
	XmNdefaultLinkCursorIcon, XmCDefaultLinkCursorIcon,
	XmRWidget, sizeof(Widget), Offset(screen.defaultLinkCursorIcon),
	XmRImmediate, (XtPointer)NULL,
    },
    {
	XmNdefaultCopyCursorIcon, XmCDefaultCopyCursorIcon, XmRWidget,
	sizeof(Widget), Offset(screen.defaultCopyCursorIcon),
	XmRImmediate, (XtPointer)NULL,
    },
    {
	XmNdefaultSourceCursorIcon, XmCDefaultSourceCursorIcon, XmRWidget,
	sizeof(Widget), Offset(screen.defaultSourceCursorIcon),
	XmRImmediate, (XtPointer)NULL,
    },
    {
	XmNmenuCursor, XmCCursor, XmRCursor,
	sizeof(Cursor), Offset(screen.menuCursor),
	XmRString, "arrow",
    },
    {
	XmNunpostBehavior, XmCUnpostBehavior, XmRUnpostBehavior,
	sizeof(unsigned char), Offset(screen.unpostBehavior),
	XmRImmediate, (XtPointer)XmUNPOST_AND_REPLAY,
    },
    {
	XmNfont, XmCFont, XmRFontStruct,
	sizeof(XFontStruct *), Offset(screen.font_struct),
	XmRString, XmDEFAULT_FONT,
    },
    {
	XmNhorizontalFontUnit, XmCHorizontalFontUnit, XmRInt,
	sizeof(int), Offset(screen.h_unit),
	XmRImmediate, (XtPointer) RESOURCE_DEFAULT,
    },
    {
	XmNverticalFontUnit, XmCVerticalFontUnit, XmRInt,
	sizeof(int), Offset(screen.v_unit),
	XmRImmediate, (XtPointer) RESOURCE_DEFAULT,
    },
    {
        XmNmoveOpaque, XmCMoveOpaque, XmRBoolean,
        sizeof(unsigned char), Offset(screen.moveOpaque),
        XmRImmediate, (XtPointer) False,
    },
    {
        XmNcolorCalculationProc, XmCColorCalculationProc, XmRProc,
        sizeof(XtProc), Offset(screen.color_calc_proc),
        XmRImmediate, (XtPointer) NULL,
    },
    {
        XmNcolorAllocationProc, XmCColorAllocationProc, XmRProc,
        sizeof(XtProc), Offset(screen.color_alloc_proc),
        XmRImmediate, (XtPointer) NULL,
    },
    {
        XmNbitmapConversionModel, XmCBitmapConversionModel,
        XmRBitmapConversionModel,
        sizeof(XtEnum), Offset(screen.bitmap_conversion_model),
        XmRImmediate, (XtPointer) XmMATCH_DEPTH,
    },
    {
        XmNuserData, XmCUserData, XmRPointer,
        sizeof(XtPointer), Offset(screen.user_data),
        XmRPointer, (XtPointer) NULL
    },
    {
	XmNinsensitiveStippleBitmap, XmCInsensitiveStippleBitmap,
	XmRNoScalingBitmap,
	sizeof(Pixmap), Offset(screen.insensitive_stipple_bitmap),
	XmRString, XmS50_foreground
    },
#ifdef DEFAULT_GLYPH_PIXMAP
    {
        XmNdefaultGlyphPixmap, XmCDefaultGlyphPixmap,
	/* if you move this one as a real pixmap,
	   change the code in the buttons as well,
	   move the copyplane into a copyarea */
	XmRBitmap,
	sizeof (Pixmap), Offset(screen.default_glyph_pixmap),
	XmRImmediate, (XtPointer) XmUNSPECIFIED_PIXMAP
     },
#endif

};

#undef Offset


/***************************************************************************
 *
 * Class Record
 *
 ***************************************************************************/

/***************************************************************************
 *
 * Class Record
 *
 ***************************************************************************/

static XmBaseClassExtRec baseClassExtRec = {
    NULL,
    NULLQUARK,
    XmBaseClassExtVersion,
    sizeof(XmBaseClassExtRec),
    NULL,				/* InitializePrehook	*/
    NULL,				/* SetValuesPrehook	*/
    NULL,				/* InitializePosthook	*/
    NULL,				/* SetValuesPosthook	*/
    NULL,				/* secondaryObjectClass	*/
    NULL,				/* secondaryCreate	*/
    NULL,          		        /* getSecRes data	*/
    { 0 },     			        /* fastSubclass flags	*/
    NULL,				/* getValuesPrehook	*/
    NULL,				/* getValuesPosthook	*/
    NULL,               /* classPartInitPrehook */
    NULL,               /* classPartInitPosthook*/
    NULL,               /* ext_resources        */
    NULL,               /* compiled_ext_resources*/
    0,                  /* num_ext_resources    */
    FALSE,              /* use_sub_resources    */
    NULL,               /* widgetNavigable      */
    NULL                /* focusChange          */
};


externaldef(xmscreenclassrec)
XmScreenClassRec xmScreenClassRec = {
    {
	(WidgetClass) &coreClassRec, /* superclass		*/
	"XmScreen",			/* class_name 		*/
	sizeof(XmScreenRec), 		/* size 		*/
	ClassInitialize,		/* Class Initializer 	*/
	ClassPartInitialize,		/* class_part_init 	*/
	FALSE, 				/* Class init'ed ? 	*/
	Initialize,		        /* initialize         	*/
	NULL, 				/* initialize_notify    */
	NULL,	 			/* realize            	*/
	NULL,	 			/* actions            	*/
	0,				/* num_actions        	*/
	resources,			/* resources          	*/
	XtNumber(resources),		/* resource_count     	*/
	NULLQUARK, 			/* xrm_class          	*/
	FALSE, 				/* compress_motion    	*/
	FALSE, 				/* compress_exposure  	*/
	FALSE, 				/* compress_enterleave	*/
	FALSE, 				/* visible_interest   	*/
	Destroy,			/* destroy            	*/
	NULL,		 		/* resize             	*/
	NULL, 				/* expose             	*/
	SetValues, 		        /* set_values         	*/
	NULL, 				/* set_values_hook      */
	NULL,			 	/* set_values_almost    */
	NULL,				/* get_values_hook      */
	NULL, 				/* accept_focus       	*/
	XtVersion, 			/* intrinsics version 	*/
	NULL, 				/* callback offsets   	*/
	NULL,				/* tm_table           	*/
	NULL, 				/* query_geometry       */
	NULL, 				/* screen_accelerator  */
	(XtPointer)&baseClassExtRec,	/* extension            */
    },
    {					/* desktop		*/
	NULL,				/* child_class		*/
	InsertChild,		        /* insert_child		*/
	DeleteChild,		        /* delete_child		*/
	NULL,				/* extension		*/
    },
    {
	NULL,
    },
};

externaldef(xmscreenclass) WidgetClass
      xmScreenClass = (WidgetClass) (&xmScreenClassRec);



externaldef(xmscreenquark) XrmQuark	_XmInvalidCursorIconQuark = NULLQUARK;
externaldef(xmscreenquark) XrmQuark	_XmValidCursorIconQuark = NULLQUARK;
externaldef(xmscreenquark) XrmQuark	_XmNoneCursorIconQuark = NULLQUARK;
externaldef(xmscreenquark) XrmQuark	_XmDefaultDragIconQuark = NULLQUARK;
externaldef(xmscreenquark) XrmQuark	_XmMoveCursorIconQuark = NULLQUARK;
externaldef(xmscreenquark) XrmQuark	_XmCopyCursorIconQuark = NULLQUARK;
externaldef(xmscreenquark) XrmQuark	_XmLinkCursorIconQuark = NULLQUARK;

static void
ClassInitialize( void )
{
  baseClassExtRec.record_type = XmQmotif;

  _XmInvalidCursorIconQuark = XrmPermStringToQuark("defaultInvalidCursorIcon");
  _XmValidCursorIconQuark = XrmPermStringToQuark("defaultValidCursorIcon");
  _XmNoneCursorIconQuark = XrmPermStringToQuark("defaultNoneCursorIcon");
  _XmDefaultDragIconQuark = XrmPermStringToQuark("defaultSourceCursorIcon");
  _XmMoveCursorIconQuark = XrmPermStringToQuark("defaultMoveCursorIcon");
  _XmCopyCursorIconQuark = XrmPermStringToQuark("defaultCopyCursorIcon");
  _XmLinkCursorIconQuark = XrmPermStringToQuark("defaultLinkCursorIcon");
}


static void
ClassPartInitialize(
	WidgetClass wc )
{
    _XmFastSubclassInit(wc, XmSCREEN_BIT);

}

/************************************************************************
 *
 *  GetUnitFromFont
 *
 ************************************************************************/
static void
GetUnitFromFont(
	Display * display,
	XFontStruct * fst,
	int * ph_unit,
	int * pv_unit)
{
    enum { XmAAVERAGE_WIDTH, XmAPIXEL_SIZE, XmARESOLUTION_Y, NUM_ATOMS };
    static char *atom_names[] = {
      XmIAVERAGE_WIDTH, XmIPIXEL_SIZE, XmIRESOLUTION_Y };
    Atom atoms[XtNumber(atom_names)];
    unsigned long pixel_s, avg_w, point_s, resolution_y;
    unsigned long font_unit_return;

    if (fst) {
      assert(XtNumber(atom_names) == NUM_ATOMS);
      XInternAtoms(display, atom_names, XtNumber(atom_names), TRUE, atoms);

              /* Horizontal units */
      if (ph_unit) {
	  if (atoms[XmAAVERAGE_WIDTH] &&
	      XGetFontProperty(fst, atoms[XmAAVERAGE_WIDTH], &avg_w)) {
	      *ph_unit = (int) ((float) (avg_w / 10) + 0.5) ;
	  } else if (XGetFontProperty (fst, XA_QUAD_WIDTH, &font_unit_return)){
	      *ph_unit = font_unit_return ;
	  } else {
	      *ph_unit = (int) ((fst->min_bounds.width +
				 fst-> max_bounds.width) / 2.3) + 0.5;
	  }
      }

              /* Vertical units */
      if (pv_unit) {
	  if (XGetFontProperty(fst, atoms[XmAPIXEL_SIZE], &pixel_s)) {
	      *pv_unit = (int) (((float) pixel_s) / 1.8) + 0.5;
	  } else if ((XGetFontProperty(fst, XA_POINT_SIZE, &point_s)) &&
		     (XGetFontProperty(fst, atoms[XmARESOLUTION_Y],
				       &resolution_y))) {
	      float ps, ry, tmp;

	      ps = point_s;
	      ry = resolution_y;

	      tmp = (ps * ry) / 1400.0;

	      *pv_unit = (int) (tmp + 0.5) ;
	  } else {
	      *pv_unit = (int) ((float) (fst->max_bounds.ascent +
					 fst->max_bounds.descent) / 2.2) + 0.5;
	  }
      }

    } else {  /* no X fontstruct */
      if (ph_unit) *ph_unit = DEFAULT_QUAD_WIDTH ;
      if (pv_unit) *pv_unit = DEFAULT_QUAD_WIDTH ;
    }

}


/************************************************************************
 *
 *  ScreenInitialize
 *
 ************************************************************************/
static void
Initialize(
        Widget requested_widget,
        Widget new_widget,
        ArgList args,
        Cardinal *num_args )
{
    XmScreen     xmScreen = (XmScreen)new_widget;
    Display      *display = XtDisplay(new_widget);

    xmScreen->screen.screenInfo = NULL;

    XQueryBestCursor(display,
		     RootWindowOfScreen(XtScreen(xmScreen)),
		     1024, 1024,
		     &xmScreen->screen.maxCursorWidth,
		     &xmScreen->screen.maxCursorHeight);

    xmScreen->screen.nullCursor = None;
    xmScreen->screen.cursorCache = NULL;
    xmScreen->screen.scratchPixmaps = (XtPointer)
      _XmAllocHashTable(20, MatchPixmap, HashPixmap);
    xmScreen->screen.inUsePixmaps = (XtPointer)
      _XmAllocHashTable(20, NULL, NULL);
    xmScreen->screen.xmStateCursorIcon = NULL;
    xmScreen->screen.xmMoveCursorIcon = NULL;
    xmScreen->screen.xmCopyCursorIcon = NULL;
    xmScreen->screen.xmLinkCursorIcon = NULL;
    xmScreen->screen.xmSourceCursorIcon = NULL;
    xmScreen->screen.mwmPresent = XmIsMotifWMRunning( new_widget) ;
    xmScreen->screen.numReparented = 0;
    xmScreen->desktop.num_children = 0;
    xmScreen->desktop.children = NULL;
    xmScreen->desktop.num_slots = 0;

    if(!XmRepTypeValidValue(XmRID_UNPOST_BEHAVIOR,
                          xmScreen->screen.unpostBehavior,
                          new_widget)) {
      xmScreen->screen.unpostBehavior = XmUNPOST_AND_REPLAY;
    }

    /* font unit stuff, priority to actual unit values, if they haven't
     been set yet, compute from the font, otherwise, stay unchanged */

    if (xmScreen->screen.h_unit == RESOURCE_DEFAULT)
	GetUnitFromFont(display, xmScreen->screen.font_struct,
			&xmScreen->screen.h_unit, NULL);

    if (xmScreen->screen.v_unit == RESOURCE_DEFAULT)
	GetUnitFromFont(display, xmScreen->screen.font_struct,
			NULL, &xmScreen->screen.v_unit);

    xmScreen->screen.screenInfo = (XtPointer) XtNew(XmScreenInfo);

    ((XmScreenInfo *)(xmScreen->screen.screenInfo))->menu_state =
		(XtPointer)NULL;
    ((XmScreenInfo *)(xmScreen->screen.screenInfo))->destroyCallbackAdded =
		False;


#ifdef DEFAULT_GLYPH_PIXMAP
    if (xmScreen->screen.default_glyph_pixmap != XmUNSPECIFIED_PIXMAP) {
	(void) XmeGetPixmapData(XtScreen(new_widget),
			 xmScreen->screen.default_glyph_pixmap,
			 NULL, NULL, NULL, NULL, NULL, NULL,
			 &(xmScreen->screen.default_glyph_pixmap_width),
			 &(xmScreen->screen.default_glyph_pixmap_height));
    }
#endif

}

/************************************************************************
 *
 *  SetValues
 *
 ************************************************************************/
static Boolean
SetValues(
        Widget current,
        Widget requested,
        Widget new_w,
        ArgList args,
        Cardinal *num_args )
{
    XmScreen		newScr = (XmScreen)new_w;
    XmScreen		oldScr = (XmScreen)current;
    Display * display = XtDisplay( new_w);

    if(!XmRepTypeValidValue(XmRID_UNPOST_BEHAVIOR,
                          newScr->screen.unpostBehavior, new_w)) {
      newScr->screen.unpostBehavior = XmUNPOST_AND_REPLAY;
    }

    /*
     *  If we are setting a default cursor icon, verify that
     *  it has the same screen as the new XmScreen.
     */

    /* defaultNoneCursorIcon */

    if (newScr->screen.defaultNoneCursorIcon !=
	    oldScr->screen.defaultNoneCursorIcon &&
	newScr->screen.defaultNoneCursorIcon != NULL &&
	XtScreenOfObject (XtParent (newScr->screen.defaultNoneCursorIcon)) !=
	     XtScreen (newScr)) {

	XmeWarning( (Widget) new_w, MESSAGE1);
	newScr->screen.defaultNoneCursorIcon =
	    oldScr->screen.defaultNoneCursorIcon;
    }

    /* defaultValidCursorIcon */

    if (newScr->screen.defaultValidCursorIcon !=
	    oldScr->screen.defaultValidCursorIcon &&
	newScr->screen.defaultValidCursorIcon != NULL &&
	XtScreenOfObject (XtParent (newScr->screen.defaultValidCursorIcon)) !=
	     XtScreen (newScr)) {

	XmeWarning( (Widget) new_w, MESSAGE1);
	newScr->screen.defaultValidCursorIcon =
	    oldScr->screen.defaultValidCursorIcon;
    }

    /* defaultInvalidCursorIcon */

    if (newScr->screen.defaultInvalidCursorIcon !=
	    oldScr->screen.defaultInvalidCursorIcon &&
	newScr->screen.defaultInvalidCursorIcon != NULL &&
	XtScreenOfObject (XtParent (newScr->screen.defaultInvalidCursorIcon)) !=
	     XtScreen (newScr)) {

	XmeWarning( (Widget) new_w, MESSAGE1);
	newScr->screen.defaultInvalidCursorIcon =
	    oldScr->screen.defaultInvalidCursorIcon;
    }

    /* defaultMoveCursorIcon */

    if (newScr->screen.defaultMoveCursorIcon !=
	    oldScr->screen.defaultMoveCursorIcon &&
	newScr->screen.defaultMoveCursorIcon != NULL &&
	XtScreenOfObject (XtParent (newScr->screen.defaultMoveCursorIcon)) !=
	     XtScreen (newScr)) {

	XmeWarning( (Widget) new_w, MESSAGE1);
	newScr->screen.defaultMoveCursorIcon =
	    oldScr->screen.defaultMoveCursorIcon;
    }

    /* defaultCopyCursorIcon */

    if (newScr->screen.defaultCopyCursorIcon !=
	    oldScr->screen.defaultCopyCursorIcon &&
	newScr->screen.defaultCopyCursorIcon != NULL &&
	XtScreenOfObject (XtParent (newScr->screen.defaultCopyCursorIcon)) !=
	     XtScreen (newScr)) {

	XmeWarning( (Widget) new_w, MESSAGE1);
	newScr->screen.defaultCopyCursorIcon =
	    oldScr->screen.defaultCopyCursorIcon;
    }

    /* defaultLinkCursorIcon */

    if (newScr->screen.defaultLinkCursorIcon !=
	    oldScr->screen.defaultLinkCursorIcon &&
	newScr->screen.defaultLinkCursorIcon != NULL &&
	XtScreenOfObject (XtParent (newScr->screen.defaultLinkCursorIcon)) !=
	     XtScreen (newScr)) {

	XmeWarning( (Widget) new_w, MESSAGE1);
	newScr->screen.defaultLinkCursorIcon =
	    oldScr->screen.defaultLinkCursorIcon;
    }

    /* defaultSourceCursorIcon */

    if (newScr->screen.defaultSourceCursorIcon !=
	    oldScr->screen.defaultSourceCursorIcon &&
	newScr->screen.defaultSourceCursorIcon != NULL &&
	XtScreenOfObject (XtParent (newScr->screen.defaultSourceCursorIcon)) !=
	     XtScreen (newScr)) {

	XmeWarning( (Widget) new_w, MESSAGE1);
	newScr->screen.defaultSourceCursorIcon =
	    oldScr->screen.defaultSourceCursorIcon;
    }

    /* font unit stuff, priority to actual unit values, if the
       font has changed but not the unit values, report the change,
       otherwise, use the unit value - i.e do nothing */

    if (newScr->screen.font_struct->fid !=
	oldScr->screen.font_struct->fid) {

	if (newScr->screen.h_unit == oldScr->screen.h_unit) {
	    GetUnitFromFont(display, newScr->screen.font_struct,
			    &newScr->screen.h_unit, NULL);
	}

	if (newScr->screen.v_unit == oldScr->screen.v_unit) {
	    GetUnitFromFont(display, newScr->screen.font_struct,
			    NULL, &newScr->screen.v_unit);
	}
    }

    return FALSE ;
}


/************************************************************************
 *
 *  Destroy
 *
 ************************************************************************/
static void
Destroy(
        Widget widget )
{
    XmScreen		xmScreen = (XmScreen)widget;
    XmDragCursorCache 	prevCache, cache;
    XmHashTable		tab;

    /* destroy any default icons created by Xm */

    if (xmScreen->screen.xmStateCursorIcon != NULL) {
	_XmDestroyDefaultDragIcon (xmScreen->screen.xmStateCursorIcon);
    }
    if (xmScreen->screen.xmMoveCursorIcon != NULL) {
	_XmDestroyDefaultDragIcon (xmScreen->screen.xmMoveCursorIcon);
    }
    if (xmScreen->screen.xmCopyCursorIcon != NULL) {
	_XmDestroyDefaultDragIcon (xmScreen->screen.xmCopyCursorIcon);
    }
    if (xmScreen->screen.xmLinkCursorIcon != NULL) {
	_XmDestroyDefaultDragIcon (xmScreen->screen.xmLinkCursorIcon);
    }
    if (xmScreen->screen.xmSourceCursorIcon != NULL) {
	_XmDestroyDefaultDragIcon (xmScreen->screen.xmSourceCursorIcon);
    }

    XtFree((char *) xmScreen->desktop.children);

    /* free the cursorCache and the pixmapCache */
    cache = xmScreen->screen.cursorCache;
    while(cache) {
	prevCache = cache;
	if (cache->cursor)
	  XFreeCursor(XtDisplay(xmScreen), cache->cursor);
	cache = cache->next;
	XtFree((char *)prevCache);
    }

    _XmProcessLock();
    tab = (XmHashTable) xmScreen->screen.scratchPixmaps;
    _XmMapHashTable(tab, FreePixmap, xmScreen);
    _XmFreeHashTable(tab);
    _XmFreeHashTable((XmHashTable) xmScreen->screen.inUsePixmaps);
    _XmProcessUnlock();

    XtFree((char *) xmScreen->screen.screenInfo);

    /* need to remove pixmap and GC related to this screen */
    _XmCleanPixmapCache (XtScreen(widget), NULL);
}

static void
InsertChild(
        Widget wid )
{
    XmDesktopObject w = (XmDesktopObject) wid ;
    register Cardinal	     	position;
    register Cardinal        	i;
    register XmScreen 	cw;
    register WidgetList      	children;

    cw = (XmScreen) w->desktop.parent;
    children = cw->desktop.children;

    position = cw->desktop.num_children;

    if (cw->desktop.num_children == cw->desktop.num_slots) {
	/* Allocate more space */
	cw->desktop.num_slots +=  (cw->desktop.num_slots / 2) + 2;
	cw->desktop.children = children =
	  (WidgetList) XtRealloc((char *) children,
		(unsigned) (cw->desktop.num_slots) * sizeof(Widget));
    }
    /* Ripple children up one space from "position" */
    for (i = cw->desktop.num_children; i > position; i--) {
	children[i] = children[i-1];
    }
    children[position] = (Widget)w;
    cw->desktop.num_children++;
}

static void
DeleteChild(
        Widget wid )
{
    XmDesktopObject w = (XmDesktopObject) wid ;
    register Cardinal	     	position;
    register Cardinal	     	i;
    register XmScreen 	cw;

    cw = (XmScreen) w->desktop.parent;

    for (position = 0; position < cw->desktop.num_children; position++) {
        if (cw->desktop.children[position] == (Widget)w) {
	    break;
	}
    }
    if (position == cw->desktop.num_children) return;

    /* Ripple children down one space from "position" */
    cw->desktop.num_children--;
    for (i = position; i < cw->desktop.num_children; i++) {
        cw->desktop.children[i] = cw->desktop.children[i+1];
    }
}

/************************************************************************
 *
 *  _XmScreenRemoveFromCursorCache
 *
 *  Mark any cursor cache reference to the specified icon as deallocated.
 ************************************************************************/

void
_XmScreenRemoveFromCursorCache(
    XmDragIconObject	icon )
{
    XmScreen	       xmScreen = (XmScreen) XmGetXmScreen(XtScreen(icon));
    XmDragCursorCache  ptr = xmScreen->screen.cursorCache;
    XmDragCursorCache  previous = xmScreen->screen.cursorCache;
    XmDragCursorCache  next;

    while (ptr) {
	next = ptr->next;

	if ((ptr->sourceIcon == icon) ||
	    (ptr->stateIcon == icon) ||
	    (ptr->opIcon == icon))
	  {
	    if (ptr->cursor)
	      XFreeCursor (XtDisplay(icon), ptr->cursor);

	    if ( xmScreen->screen.cursorCache == ptr )
		xmScreen->screen.cursorCache = ptr->next;
	    else
		previous->next = ptr->next;

	    XtFree ((char*)ptr);
	  }
	else
	    previous = ptr;

	ptr = next;
    }
}

static Boolean
FreePixmap(XmHashKey k, XtPointer p, XtPointer client_data)
{
  XmScreen	xmScreen = (XmScreen) client_data;

  XtFree((char*) k);
  XFreePixmap(XtDisplay(xmScreen), (Pixmap) p);

  return False;
}



/************************************************************************
 *
 *  _XmScreenGetOperationIcon ()
 *
 *  Returns one of the three XmScreen operation cursor types. These aren't
 *  created ahead of time in order to let the client specify its own.
 *  If they haven't by now (a drag is in process) then we create our
 *  own. The name of the OperatonIcon can cause the built-in cursor data
 *  to get loaded in (if not specified in the resource file).
 ************************************************************************/

XmDragIconObject
_XmScreenGetOperationIcon(
        Widget w,
        unsigned char operation )
{
    XmScreen		xmScreen = (XmScreen) XmGetXmScreen(XtScreenOfObject(w));
    XrmQuark		nameQuark = NULLQUARK;
    XmDragIconObject	*ptr = NULL;
    XmDragIconObject	*pt2 = NULL;

    switch ((int) operation) {
	case XmDROP_MOVE:
	    ptr = &xmScreen->screen.defaultMoveCursorIcon;
	    pt2 = &xmScreen->screen.xmMoveCursorIcon;
	    nameQuark = _XmMoveCursorIconQuark;
	    break;

	case XmDROP_COPY:
	    ptr = &xmScreen->screen.defaultCopyCursorIcon;
	    pt2 = &xmScreen->screen.xmCopyCursorIcon;
	    nameQuark = _XmCopyCursorIconQuark;
	    break;

	case XmDROP_LINK:
	    ptr = &xmScreen->screen.defaultLinkCursorIcon;
	    pt2 = &xmScreen->screen.xmLinkCursorIcon;
	    nameQuark = _XmLinkCursorIconQuark;
	    break;

	default:
	    return (NULL);
    }
    if (*ptr == NULL) {
	if (*pt2 == NULL) {
	    *pt2 = (XmDragIconObject)
	        XmCreateDragIcon ((Widget) xmScreen,
			          XrmQuarkToString(nameQuark), NULL, 0);
	}
	*ptr = *pt2;
    }
    return *ptr;
}

/************************************************************************
 *
 *  _XmScreenGetStateIcon ()
 *
 *  Returns one of the three XmScreen state cursor types. These aren't
 *  created ahead of time in order to let the client specify its own.
 *  If they haven't by now (a drag is in process) then we create our
 *  own. The name of the StateIcon can cause the built-in cursor data
 *  to get loaded in (if not specified in the resource file).
 *  The default state cursors are the same XmDragIcon object.
 ************************************************************************/

XmDragIconObject
_XmScreenGetStateIcon(
        Widget w,
        unsigned char state )
{
    XmScreen		xmScreen = (XmScreen) XmGetXmScreen(XtScreenOfObject(w));
    XrmQuark		nameQuark = NULLQUARK;
    XmDragIconObject	icon = NULL;

    switch(state) {
	default:
	case XmNO_DROP_SITE:
	    icon = xmScreen->screen.defaultNoneCursorIcon;
	    nameQuark = _XmNoneCursorIconQuark;
	    break;

	case XmVALID_DROP_SITE:
	    icon = xmScreen->screen.defaultValidCursorIcon;
	    nameQuark = _XmValidCursorIconQuark;
	    break;

	case XmINVALID_DROP_SITE:
	    icon = xmScreen->screen.defaultInvalidCursorIcon;
	    nameQuark = _XmInvalidCursorIconQuark;
	    break;
    }
    if (icon == NULL) {

	/*
	 *  We need to create the default state icons.
	 *  Set all uncreated state icons to the same XmDragIcon object.
	 */

	if (xmScreen->screen.xmStateCursorIcon == NULL) {
	    xmScreen->screen.xmStateCursorIcon = (XmDragIconObject)
	        XmCreateDragIcon ((Widget) xmScreen,
			          XrmQuarkToString(nameQuark),
				  NULL, 0);
	}
	icon = xmScreen->screen.xmStateCursorIcon;

	if (xmScreen->screen.defaultNoneCursorIcon == NULL) {
	    xmScreen->screen.defaultNoneCursorIcon = icon;
	}
	if (xmScreen->screen.defaultValidCursorIcon == NULL) {
	    xmScreen->screen.defaultValidCursorIcon = icon;
	}
	if (xmScreen->screen.defaultInvalidCursorIcon == NULL) {
	    xmScreen->screen.defaultInvalidCursorIcon = icon;
	}
    }
    return (icon);
}

/************************************************************************
 *
 *  _XmScreenGetSourceIcon ()
 *
 *  Returns the XmScreen source cursor types.  This isn't created ahead of
 *  time in order to let the client specify its own.  If it hasn't by now
 *  (a drag is in process) then we create our own.
 ************************************************************************/

XmDragIconObject
_XmScreenGetSourceIcon(
        Widget w )
{
    XmScreen	xmScreen = (XmScreen) XmGetXmScreen(XtScreenOfObject(w));

    if (xmScreen->screen.defaultSourceCursorIcon == NULL) {

	if (xmScreen->screen.xmSourceCursorIcon == NULL) {
	    xmScreen->screen.xmSourceCursorIcon = (XmDragIconObject)
	        XmCreateDragIcon ((Widget) xmScreen,
			          XrmQuarkToString(_XmDefaultDragIconQuark),
			          NULL, 0);
	}
	xmScreen->screen.defaultSourceCursorIcon =
	    xmScreen->screen.xmSourceCursorIcon;
    }
    return xmScreen->screen.defaultSourceCursorIcon;
}

/************************************************************************
 *
 *  _XmAllocScratchPixmap
 *
 ************************************************************************/

static Boolean
MatchPixmap(XmHashKey k1, XmHashKey k2)
{
  XmScratchPixmapKey key1 = (XmScratchPixmapKey) k1;
  XmScratchPixmapKey key2 = (XmScratchPixmapKey) k2;

  return(key1 -> height == key2 -> height &&
	 key1 -> width == key2 -> width &&
	 key1 -> depth == key2 -> depth);
}

static XmHashValue
HashPixmap(XmHashKey k)
{
  XmScratchPixmapKey key = (XmScratchPixmapKey) k;

  return(key -> height + key -> width + key -> depth);
}

Pixmap
_XmAllocScratchPixmap(
        XmScreen xmScreen,
        Cardinal depth,
        Dimension width,
        Dimension height )
{
    XmHashTable		scratchTable =
      (XmHashTable) xmScreen->screen.scratchPixmaps;
    XmHashTable		inUseTable =
      (XmHashTable) xmScreen->screen.inUsePixmaps;
    Pixmap		scratchPixmap = None;
    XmScratchPixmapKeyRec	key;
    XmScratchPixmapKey	returned_key;

    key.height = height;
    key.width = width;
    key.depth = depth;

    _XmProcessLock();
    scratchPixmap = (Pixmap) _XmGetHashEntry(scratchTable, &key);

    if (scratchPixmap != None) {
      /* remove from free table */
      returned_key = (XmScratchPixmapKey)
	_XmRemoveHashEntry(scratchTable, &key);
    } else {
      returned_key = XtNew(XmScratchPixmapKeyRec);
      returned_key->width = width;
      returned_key->height = height;
      returned_key->depth = depth;
      scratchPixmap = XCreatePixmap (XtDisplay(xmScreen),
				     RootWindowOfScreen(XtScreen(xmScreen)),
				     width, height,
				     depth);
    }

    /* Place key in inUse table */
    _XmAddHashEntry(inUseTable, (XmHashKey) scratchPixmap, returned_key);

    _XmProcessUnlock();
    return(scratchPixmap);
}

/************************************************************************
 *
 *  _XmFreeScratchPixmap
 *
 ************************************************************************/

void
_XmFreeScratchPixmap(
        XmScreen xmScreen,
        Pixmap pixmap )
{
    XmHashTable scratchTable = (XmHashTable) xmScreen->screen.scratchPixmaps;
    XmHashTable inUseTable   = (XmHashTable) xmScreen->screen.inUsePixmaps;
    XmScratchPixmapKey returned_key;

    _XmProcessLock();
    returned_key = (XmScratchPixmapKey)
      _XmGetHashEntry(inUseTable, (XmHashKey) pixmap);

    if (returned_key == NULL) {
      /* Bad,  this pixmap wasn't in the cache,  return */
      _XmProcessUnlock();
      return;
    }

    _XmRemoveHashEntry(inUseTable, (XmHashKey) pixmap);

    _XmAddHashEntry(scratchTable, returned_key, (XtPointer) pixmap);
    _XmProcessUnlock();
}


/************************************************************************
 *
 *  _XmGetDragCursorCachePtr ()
 *
 ************************************************************************/

XmDragCursorCache *
_XmGetDragCursorCachePtr(
        XmScreen xmScreen )
{
    return &xmScreen->screen.cursorCache;
}

/************************************************************************
 *
 *  XmeQueryBestCursorSize()
 *
 ************************************************************************/

void
XmeQueryBestCursorSize(
        Widget w,
        Dimension *width,
        Dimension *height )
{
    XmScreen	xmScreen;
    _XmWidgetToAppContext(w);

    _XmAppLock(app);
    xmScreen = (XmScreen) XmGetXmScreen(XtScreenOfObject(w));
    *width = (Dimension)xmScreen->screen.maxCursorWidth;
    *height = (Dimension)xmScreen->screen.maxCursorHeight;
    _XmAppUnlock(app);
    return;
}

static XmConst char nullBits[] =
{
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
};

/************************************************************************
 *
 *  XmeGetNullCursor ()
 *
 ************************************************************************/

Cursor
XmeGetNullCursor(
        Widget w )
{
    XmScreen		xmScreen;
    Pixmap		pixmap;
    Cursor		cursor;
    XColor		foreground;
    XColor		background;
    _XmWidgetToAppContext(w);

    _XmAppLock(app);
    xmScreen = (XmScreen) XmGetXmScreen(XtScreenOfObject(w));
    if (xmScreen->screen.nullCursor == None) {
	foreground.pixel =
	  background.pixel = 0;
	pixmap =
	  XCreatePixmapFromBitmapData(XtDisplayOfObject(w),
				      RootWindowOfScreen(XtScreenOfObject(w)),
				      (char*)nullBits,
				      4, 4,
				      0, 0,
				      1);
	cursor =
	  XCreatePixmapCursor(XtDisplayOfObject(w),
			      pixmap,
			      pixmap,
			      &foreground, &background,
			      0, 0);
	XFreePixmap(XtDisplayOfObject(w), pixmap);
	xmScreen->screen.nullCursor = cursor;
    }
    cursor = xmScreen->screen.nullCursor;
    _XmAppUnlock(app);
    return cursor;
}

/*
 * The following set of functions support the menu cursor functionality.
 * They have moved from MenuUtil to here.
 */


/* Obsolete global per-display menu cursor manipulation functions */
/* Programs have to use XtSet/GetValues on the XmScreen object instead */
void
XmSetMenuCursor(
        Display *display,
        Cursor cursorId )
{
    XmScreen          xmScreen;
    Screen            *scr;
    int i ;
    _XmDisplayToAppContext(display);

    /* This function has no screen parameter, so we have to set the
       menucursor for _all_ the xmscreen available on this display. why?
       because when RowColumn will be getting a menucursor for a particular
       screen, it will have to get what the application has set using
       this function, not the default for that particular screen (which is
       what will happen if we were only setting the default display here) */

    _XmAppLock(app);
    for (i=0, scr = ScreenOfDisplay(display, i); i < ScreenCount(display);
       i++, scr = ScreenOfDisplay(display, i)) {

      xmScreen = (XmScreen) XmGetXmScreen(scr);
      xmScreen->screen.menuCursor = cursorId ;
    }
    _XmAppUnlock(app);
}


Cursor
XmGetMenuCursor(
        Display *display )
{
   XmScreen           xmScreen;
   Cursor	      cursor;
   _XmDisplayToAppContext(display);

   /* get the default screen menuCursor since there is no
      other information available to this function */
   _XmAppLock(app);
   xmScreen = (XmScreen) XmGetXmScreen(DefaultScreenOfDisplay(display));
   cursor = xmScreen->screen.menuCursor;
   _XmAppUnlock(app);
   return cursor;
}

/* a convenience for RowColumn */
Cursor
_XmGetMenuCursorByScreen(
        Screen * screen  )
{
   XmScreen           xmScreen;

   xmScreen = (XmScreen) XmGetXmScreen(screen);
   return(xmScreen->screen.menuCursor);
}

Boolean
_XmGetMoveOpaqueByScreen(
        Screen * screen  )
{
   XmScreen           xmScreen;

   xmScreen = (XmScreen) XmGetXmScreen(screen);
   return(xmScreen->screen.moveOpaque);
}

/* a convenience for RowColumn */
unsigned char
_XmGetUnpostBehavior(
        Widget wid )
{
   XmScreen	xmScreen = (XmScreen) XmGetXmScreen(XtScreenOfObject(wid));

   return(xmScreen->screen.unpostBehavior);
}


/**********************************************************************
 **********************************************************************

      Font unit handling functions

 **********************************************************************
 **********************************************************************/

/**********************************************************************
 *
 *  XmSetFontUnits
 *    Set the font_unit value for all screens.  These values can
 *    then be used later to process the font unit conversions.
 *
 **********************************************************************/
static void _XmSetFontUnits(Display *display, int h_value, int v_value)
{
    XmScreen          xmScreen;
    Screen            *scr;
    int i ;
    _XmDisplayToAppContext(display);

    /* This function has no screen parameter, so we have to set the
       fontunit for _all_ the xmscreen available on this display. why?
       because when someone will be getting fontunits for a particular
       screen, it will have to get what the application has set using
       this function, not the default for that particular screen (which is
       what will happen if we were only setting the default display here) */

    _XmAppLock(app);
    for (i=0, scr = ScreenOfDisplay(display, i); i < ScreenCount(display);
       i++, scr = ScreenOfDisplay(display, i)) {

      xmScreen = (XmScreen) XmGetXmScreen(scr);
      xmScreen->screen.h_unit =  h_value ;
      xmScreen->screen.v_unit =  v_value ;
    }
    _XmAppUnlock(app);
}

/* DEPRECATED */
void XmSetFontUnits(Display *display, int h_value, int v_value)
{
	_XmSetFontUnits(display, h_value, v_value);
}

/* DEPRECATED */
void
XmSetFontUnit(
        Display *display,
        int value )
{
    _XmSetFontUnits(display, value, value);
}

/**********************************************************************
 *
 *  _XmGetFontUnit
 *
 **********************************************************************/
int
_XmGetFontUnit(
        Screen *screen,
        int dimension )
{
    XmScreen          xmScreen;

    xmScreen = (XmScreen) XmGetXmScreen(screen);
    if (dimension == XmHORIZONTAL)
      return(xmScreen->screen.h_unit);
    else
      return(xmScreen->screen.v_unit);
}


/**********************************************************************
 *
 *  _XmGetColorCalculationProc
 *     Here there is no point of supporting the old function as
 *     place holder for the new color proc since the signature are
 *     different. The old color proc will still be managed by Visual.c
 *     in its own way.
 **********************************************************************/
XmScreenColorProc
_XmGetColorCalculationProc(
        Screen *screen)
{
    XmScreen          xmScreen;

    xmScreen = (XmScreen) XmGetXmScreen(screen);
    return(xmScreen->screen.color_calc_proc);
}

/**********************************************************************
 *
 *  _XmGetColorAllocationProc
 *
 **********************************************************************/
XmAllocColorProc
_XmGetColorAllocationProc(
        Screen *screen)
{
    XmScreen          xmScreen;

    xmScreen = (XmScreen) XmGetXmScreen(screen);
    return(xmScreen->screen.color_alloc_proc);
}

/**********************************************************************
 *
 *  _XmGetBitmapConversionModel
 *
 **********************************************************************/

XtEnum
_XmGetBitmapConversionModel(
        Screen *screen)
{
    XmScreen          xmScreen;

    xmScreen = (XmScreen) XmGetXmScreen(screen);
    return(xmScreen->screen.bitmap_conversion_model);
}




/************************************************************************
 *
 * _XmGetInsensitiveStippleBitmap
 *
 * Returns the insensitive_stipple_bitmap field of the XmScreenPart
 ************************************************************************/

Pixmap
_XmGetInsensitiveStippleBitmap (Widget w)
{
    XmScreen    xmScreen = (XmScreen) XmGetXmScreen(XtScreen(w));

    return(xmScreen->screen.insensitive_stipple_bitmap);
}


#ifdef DEFAULT_GLYPH_PIXMAP
/**********************************************************************
 *
 *  _XmGetDefaultGlyphPixmap
 *
 **********************************************************************/
Pixmap
_XmGetDefaultGlyphPixmap(
        Screen *screen,
        unsigned int * width,
        unsigned int  *height)
{
    XmScreen          xmScreen;

    xmScreen = (XmScreen) XmGetXmScreen(screen);

    if (width) *width = xmScreen->screen.default_glyph_pixmap_width ;
    if (height) *height = xmScreen->screen.default_glyph_pixmap_height ;

    return(xmScreen->screen.default_glyph_pixmap);
}
#endif



/*********************************************************************
 *
 *  XmGetXmScreen
 *
 *********************************************************************/

Widget
XmGetXmScreen(
        Screen *screen )
{
    XmDisplay	xmDisplay;
    WidgetList	children;
    Widget 	widget;
    int	num_children;
    Arg args[5];
    int i;
    Screen *scr;
    char name[25];
    _XmDisplayToAppContext(DisplayOfScreen(screen));

    _XmAppLock(app);
    if ((xmDisplay = (XmDisplay)
	 XmGetXmDisplay(DisplayOfScreen(screen))) == NULL)
	{
		XmeWarning(NULL, MESSAGE2);
		_XmAppUnlock(app);
		return(NULL);
	}

	children = xmDisplay->composite.children;
	num_children = xmDisplay->composite.num_children;

	for (i=0; i < num_children; i++)
	{
		Widget child = children[i];
		if ((XmIsScreen(child)) &&
			(screen == XtScreen(child))) {
			_XmAppUnlock(app);
			return(child);
		}
	}

	/* Not found; have to do an implied creation */
	for (scr = ScreenOfDisplay(XtDisplay(xmDisplay), i);
		i < ScreenCount(XtDisplay(xmDisplay));
		i++, scr = ScreenOfDisplay(XtDisplay(xmDisplay), i))
	{
		if (scr == screen)
			break;
	}

	sprintf(name, "screen%d", i);

	i = 0;
	XtSetArg(args[i], XmNscreen, screen); i++;
	widget = XtCreateWidget(name, xmScreenClass, (Widget)xmDisplay,
		args, i);
	_XmAppUnlock(app);
	return widget;
}
