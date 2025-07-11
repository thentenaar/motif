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
#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$TOG: DragOverS.c /main/18 1999/08/11 15:53:11 mgreess $"
#endif
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


/************************************************************************
 *
 *  This module dynamically blends and manages the dragover visual using
 *  the XmDragOverShell widget and the following API:
 *
 *	_XmDragOverHide()
 *	_XmDragOverShow()
 *	_XmDragOverMove()
 *	_XmDragOverChange()
 *	_XmDragOverFinish()
 *	_XmDragOverGetActiveCursor()
 *	_XmDragOverSetInitialPosition()
 *
 *  The XmDragOverShellPart structure has the following members:
 *
 *	Position hotX, hotY -- the coordinates of the dragover visual's
 *		hotspot.  These are settable resources and are maintained
 *		through the _XmDragOverMove() interface.
 *
 *	Position initialX, initialY -- the initial coordinates of the
 *		dragover visual's hotspot.  These are set in the
 *		XmDragOverShell's initialization and also through the
 *		_XmDragOverSetInitialPosition() interface.  These are
 *		used as the zap-back position in the zap-back dropFinish
 *		effect.
 *
 *	unsigned char mode -- one of {XmPIXMAP, XmCURSOR, XmWINDOW}.
 *		XmPIXMAP indicates the current drag protocol style is
 *		pre-register and indicates that either a pixmap or a
 *		cursor can be used for the dragover visual.  XmCURSOR
 *		indicates dynamic protocol style is in effect and that
 *		a cursor must be used for the dragover visual.  XmWINDOW
 *		is used during dropFinish processing so that the dragover
 *		visual can persist even without the server grabbed.
 *
 *	unsigned char activeMode -- one of {XmPIXMAP, XmCURSOR, XmWINDOW}.
 *		Determined by the value of mode and the capabilities of
 *		the hardware.  Indicates how the dragover visual is being
 *		rendered.  XmCURSOR indicates that the hardware cursor is
 *		being used.  XmPIXMAP indicates that a pixmap is being
 *		dragged.  XmWINDOW indicates that the XmDragOverShell's
 *		window is popped up and is being dragged.
 *
 *	Cursor activeCursor -- if activeMode == XmCURSOR, contains the
 *		cursor being used to render the dragover visual.
 *		Otherwise, contains a null cursor.
 *
 *	unsigned char cursorState -- set within _XmDragOverChange(),
 *		and one of {XmVALID_DROP_SITE, XmINVALID_DROP_SITE,
 *		XmNO_DROP_SITE}, indicates the status of the current
 *		dropsite.
 *
 *	Pixel cursorForeground, cursorBackground -- indicates the current
 *		foreground and background colors of the dragover visual.
 *		Can depend on the cursorState and the screen's default
 *		colormap.
 *
 *	XmDragIconObject stateIcon, opIcon -- indicates, respectively, the
 *		current state and operation icons being blended to the
 *		source icon to form the dragover visual.  If a state or
 *		operation icon is not being blended, the corresponding
 *		icon here is NULL.
 *
 *	XmDragOverBlendRec cursorBlend, rootBlend -- cursorBlend contains
 *		the blended icon data for XmCURSOR activeMode and rootBlend
 *		contains the blended icon data for XmPIXMAP and XmWINDOW
 *		activeMode.  The blended icon data consists of
 *
 *			XmDragIconObject sourceIcon --	the source icon.
 *			Position sourceX, sourceY -- the source location
 *				within the blended icon (dragover visual).
 *			XmDragIconObject mixedIcon -- the blended icon.
 *			GC gc -- the gc used to create the blended icon.
 *				the rootBlend gc is also used to render
 *				the blended icon to the display.
 *
 *	Boolean isVisible -- indicates whether the dragover visual is
 *		visible.  Used to avoid unnecessary computation.
 *
 *	XmBackingRec backing -- contains the backing store needed during
 *		pixmap dragging (XmPIXMAP activeMode) and the dropFinish
 *		effects.  Consists of an (x,y) position in root coordinates
 *		and a pixmap.
 *
 *	Pixmap tmpPix, tmpBit -- scratch pixmap and bitmap used during
 *		pixmap dragging (XmPIXMAP activeMode) to reduce screen
 *		flashing.
 *
 *	Cursor ncCursor -- the cursor id of the last noncached cursor
 *		used.  Needed so the cursor can be freed when it is no
 *		longer needed.
 *
 *
 *
 *  NOTE:  This module directly accesses three members of the
 *         XmDragContext structure:
 *
 *	dc->drag.origDragOver
 *	dc->drag.operation
 *	dc->drag.lastChangeTime
 *
 ***********************************************************************/

#include <stdio.h>
#include <X11/IntrinsicP.h>
#include <X11/Shell.h>
#include <X11/ShellP.h>
#include <X11/StringDefs.h>
#include <X11/Xatom.h>
#include <X11/Xos.h>
#include <X11/cursorfont.h>
#include <X11/extensions/shape.h>
#include <Xm/DragCP.h>
#include <Xm/VendorSEP.h>
#include <Xm/XmP.h>
#include <Xm/XmosP.h>
#include "DragCI.h"
#include "DragICCI.h"
#include "DragIconI.h"
#include "DragOverSI.h"
#include "MessagesI.h"
#include "RegionI.h"
#include "ScreenI.h"
#include "XmI.h"

#define MESSAGE1	_XmMMsgDragOverS_0000
#define MESSAGE2	_XmMMsgDragOverS_0001
#define MESSAGE3	_XmMMsgDragOverS_0002
#define MESSAGE4	_XmMMsgDragOverS_0003

#define PIXMAP_MAX_WIDTH	128
#define PIXMAP_MAX_HEIGHT	128

#define BackingPixmap(dos)	(dos->drag.backing.pixmap)
#define BackingX(dos)		(dos->drag.backing.x)
#define BackingY(dos)		(dos->drag.backing.y)

#define ZAP_TIME	50000L
#define MELT_TIME	50000L

/********    Static Function Declarations    ********/

static void DoZapEffect(
                        XtPointer clientData,
                        XtIntervalId *id) ;
static void DoMeltEffect(
                        XtPointer clientData,
                        XtIntervalId *id) ;
static void GetIconPosition(
                        XmDragOverShellWidget dos,
                        XmDragIconObject icon,
                        XmDragIconObject sourceIcon,
                        Position *iconX,
                        Position *iconY) ;
static void BlendIcon(  XmDragOverShellWidget dos,
                        XmDragIconObject icon,
                        XmDragIconObject mixedIcon,
#if NeedWidePrototypes
                        int iconX,
                        int iconY,
#else
                        Position iconX,
                        Position iconY,
#endif /* NeedWidePrototypes */
                        GC maskGC,
                        GC pixmapGC) ;
static void MixedIconSize(
                        XmDragOverShellWidget dos,
                        XmDragIconObject sourceIcon,
                        XmDragIconObject stateIcon,
                        XmDragIconObject opIcon,
                        Dimension *width,
                        Dimension *height) ;
static void DestroyMixedIcon(
                        XmDragOverShellWidget dos,
                        XmDragIconObject mixedIcon) ;
static void MixIcons(
                        XmDragOverShellWidget dos,
                        XmDragIconObject sourceIcon,
                        XmDragIconObject stateIcon,
                        XmDragIconObject opIcon,
                        XmDragOverBlendRec *blendPtr,
#if NeedWidePrototypes
                        int clip) ;
#else
                        Boolean clip) ;
#endif /* NeedWidePrototypes */
static Boolean FitsInCursor(
                        XmDragOverShellWidget dos,
                        XmDragIconObject sourceIcon,
                        XmDragIconObject stateIcon,
                        XmDragIconObject opIcon) ;
static Boolean GetDragIconColors(
                        XmDragOverShellWidget dos) ;
static Cursor GetDragIconCursor(
                        XmDragOverShellWidget dos,
                        XmDragIconObject sourceIcon,
                        XmDragIconObject stateIcon,
                        XmDragIconObject opIcon,
#if NeedWidePrototypes
			int clip,
                        int dirty) ;
#else
                        Boolean clip,
                        Boolean dirty) ;
#endif /* NeedWidePrototypes */
static void Initialize(
                        Widget req,
                        Widget new_w,
                        ArgList args,
                        Cardinal *numArgs) ;
static Boolean SetValues(
                        Widget current,
                        Widget req,
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args) ;
static void DrawIcon(
                        XmDragOverShellWidget dos,
                        XmDragIconObject icon,
                        Window window,
#if NeedWidePrototypes
                        int x,
                        int y) ;
#else
                        Position x,
                        Position y) ;
#endif /* NeedWidePrototypes */
static void Redisplay(
                        Widget wid,
                        XEvent *event,
                        Region region) ;
static void Destroy(
                        Widget w) ;
static void ChangeActiveMode(
                        XmDragOverShellWidget dos,
#if NeedWidePrototypes
                        unsigned int newActiveMode) ;
#else
                        unsigned char newActiveMode) ;
#endif /* NeedWidePrototypes */
static void ChangeDragWindow(
			XmDragOverShellWidget dos) ;
static void FindColormapShell(XmDragOverShellWidget dw) ;
static void InstallColormap(XmDragOverShellWidget dw) ;
static void UninstallColormap(XmDragOverShellWidget dw) ;
static void DragOverShellPunchHole(Widget w) ;
static void DragOverShellColormapWidget(Widget ds, Widget cw) ;
/********    End Static Function Declarations    ********/

#undef Offset
#define Offset(x) (XtOffsetOf(XmDragOverShellRec, x))

static XtResource resources[]=
{
    {
	XmNoverrideRedirect, XmCOverrideRedirect,
	XmRBoolean, sizeof(Boolean), Offset(shell.override_redirect),
	XtRImmediate, (XtPointer)True,
    },
    {
        XmNsaveUnder, XmCSaveUnder,
	XmRBoolean, sizeof(Boolean), Offset(shell.save_under),
	XtRImmediate, (XtPointer)True,
    },
    {
	XmNhotX, XmCHot, XmRPosition,
        sizeof(Position), Offset(drag.hotX),
        XmRImmediate, (XtPointer)0,
    },
    {
	XmNhotY, XmCHot, XmRPosition,
        sizeof(Position), Offset(drag.hotY),
        XmRImmediate, (XtPointer)0,
    },
    {
	XmNdragOverMode, XmCDragOverMode, XtRUnsignedChar,
        sizeof(unsigned char), Offset(drag.mode),
        XmRImmediate, (XtPointer)XmCURSOR,
    },
    {
	XmNdragOverActiveMode, XmCDragOverActiveMode, XtRUnsignedChar,
        sizeof(unsigned char), Offset(drag.activeMode),
        XmRImmediate, (XtPointer)XmCURSOR,
    },
    {
	XmNinstallColormap, XmCInstallColormap,	XmRBoolean,
	sizeof(Boolean), Offset(drag.installColormap),
	XmRImmediate, (XtPointer)FALSE,
    },

};

#undef Offset

/***************************************************************************
 *
 * DragOverShell class record
 *
 ***************************************************************************/

externaldef(xmdragovershellclassrec)
XmDragOverShellClassRec xmDragOverShellClassRec = {
    {					/* core class record */
	(WidgetClass) &vendorShellClassRec,	/* superclass */
	"XmDragOverShell",	 	/* class_name */
	sizeof(XmDragOverShellRec),	/* widget_size */
	(XtProc)NULL,			/* class_initialize proc */
	(XtWidgetClassProc)NULL,	/* class_part_initialize proc */
	FALSE, 				/* class_inited flag */
	Initialize,	 		/* instance initialize proc */
	(XtArgsProc)NULL, 		/* init_hook proc */
	XtInheritRealize,		/* realize widget proc */
	NULL,				/* action table for class */
	0,				/* num_actions */
	resources,			/* resource list of class */
	XtNumber(resources),		/* num_resources in list */
	NULLQUARK, 			/* xrm_class ? */
	FALSE, 				/* don't compress_motion */
        XtExposeCompressSeries |        /* compressed exposure */
	    XtExposeNoRegion,
	FALSE, 				/* do compress enter-leave */
	FALSE, 				/* do have visible_interest */
	Destroy,			/* destroy widget proc */
	XtInheritResize, 		/* resize widget proc */
	Redisplay,			/* expose proc */
	SetValues, 			/* set_values proc */
	(XtArgsFunc)NULL, 		/* set_values_hook proc */
	XtInheritSetValuesAlmost, 	/* set_values_almost proc */
	(XtArgsProc)NULL, 		/* get_values_hook */
	(XtAcceptFocusProc)NULL, 	/* accept_focus proc */
	XtVersion, 			/* current version */
	NULL, 				/* callback offset    */
	NULL,		 		/* default translation table */
	XtInheritQueryGeometry, 	/* query geometry widget proc */
	(XtStringProc)NULL, 		/* display accelerator    */
	NULL,				/* extension record      */
    },
     { 					/* composite class record */
	XtInheritGeometryManager,	/* geometry_manager */
	XtInheritChangeManaged,		/* change_managed		*/
	XtInheritInsertChild,		/* insert_child			*/
	XtInheritDeleteChild,		/* from the shell */
	NULL, 				/* extension record      */
    },
    { 					/* shell class record */
	NULL, 				/* extension record      */
    },
    { 					/* wm shell class record */
	NULL, 				/* extension record      */
    },
    { 					/* vendor shell class record */
	NULL, 				/* extension record      */
    },
    { 					/* dragOver shell class record */
	NULL,				/* extension record      */
    },
};
externaldef(xmDragOvershellwidgetclass) WidgetClass xmDragOverShellWidgetClass =
	(WidgetClass) (&xmDragOverShellClassRec);

typedef struct _MixedIconCache
{
	Cardinal		depth;
	Dimension		width;
	Dimension		height;
	Pixel                   cursorForeground;
	Pixel                   cursorBackground;
	Position		sourceX;
	Position		sourceY;
	Position		stateX;
	Position		stateY;
	Position		opX;
	Position		opY;
	Pixmap			sourcePixmap;
	Pixmap			statePixmap;
	Pixmap			opPixmap;
	Pixmap			sourceMask;
	Pixmap			stateMask;
	Pixmap			opMask;
	XmDragIconObject        mixedIcon;
	struct _MixedIconCache *next;
} MixedIconCache;

static MixedIconCache * mixed_cache = NULL;

static Boolean
CacheMixedIcon(
	XmDragOverShellWidget	dos,
	Cardinal		depth,
	Dimension		width,
	Dimension		height,
	XmDragIconObject	sourceIcon,
	XmDragIconObject	stateIcon,
	XmDragIconObject	opIcon,
	Position		sourceX,
	Position		sourceY,
	Position		stateX,
	Position		stateY,
	Position		opX,
	Position		opY,
	XmDragIconObject	mixedIcon)
{
    register MixedIconCache * cache_ptr;

    if (mixedIcon == NULL) return False;

    cache_ptr = XtNew (MixedIconCache);
    _XmProcessLock();
    cache_ptr->next = mixed_cache;
    mixed_cache = cache_ptr;
    _XmProcessUnlock();

    cache_ptr->depth = depth;
    cache_ptr->width = width;
    cache_ptr->height = height;
    cache_ptr->cursorForeground = dos->drag.cursorForeground;
    cache_ptr->cursorBackground = dos->drag.cursorBackground;
    cache_ptr->sourcePixmap = sourceIcon->drag.pixmap;
    cache_ptr->sourceMask = sourceIcon->drag.mask;
    cache_ptr->sourceX = sourceX;
    cache_ptr->sourceY = sourceY;

    if (stateIcon) {
    	cache_ptr->statePixmap = stateIcon->drag.pixmap;
    	cache_ptr->stateMask = stateIcon->drag.mask;
    	cache_ptr->stateX = stateX;
    	cache_ptr->stateY = stateY;
    } else {
    	cache_ptr->statePixmap = 0;
    }

    if (opIcon) {
       cache_ptr->opPixmap = opIcon->drag.pixmap;
       cache_ptr->opMask = opIcon->drag.mask;
       cache_ptr->opX = opX;
       cache_ptr->opY = opY;
    } else {
       cache_ptr->opPixmap = 0;
    }

    cache_ptr->mixedIcon = mixedIcon;

    return True;
}


static XmDragIconObject
GetMixedIcon(
	XmDragOverShellWidget	dos,
	Cardinal		depth,
	Dimension		width,
	Dimension		height,
	XmDragIconObject	sourceIcon,
	XmDragIconObject	stateIcon,
	XmDragIconObject	opIcon,
	Position		sourceX,
	Position		sourceY,
	Position		stateX,
	Position		stateY,
	Position		opX,
	Position		opY)
{
    register MixedIconCache * cache_ptr;

    for (cache_ptr = mixed_cache; cache_ptr; cache_ptr = cache_ptr->next)
    {
	if (cache_ptr->depth == depth &&
	    cache_ptr->width == width &&
	    cache_ptr->height == height &&
	    cache_ptr->cursorForeground == dos->drag.cursorForeground &&
	    cache_ptr->cursorBackground == dos->drag.cursorBackground &&
	    cache_ptr->sourcePixmap == sourceIcon->drag.pixmap &&
	    cache_ptr->sourceMask == sourceIcon->drag.mask &&
	    cache_ptr->sourceX == sourceX &&
	    cache_ptr->sourceY == sourceY &&
            ((cache_ptr->statePixmap == 0 && !stateIcon) ||
             (stateIcon && cache_ptr->statePixmap == stateIcon->drag.pixmap &&
			   cache_ptr->stateMask == stateIcon->drag.mask &&
	    		   cache_ptr->stateX == stateX &&
	    		   cache_ptr->stateY == stateY)) &&
            ((cache_ptr->opPixmap == 0 && !opIcon) ||
             (opIcon && cache_ptr->opPixmap == opIcon->drag.pixmap &&
			   cache_ptr->opMask == opIcon->drag.mask &&
	    		   cache_ptr->opX == opX &&
	    		   cache_ptr->opY == opY)))
	{
           return (cache_ptr->mixedIcon);
        }
     }
     return ((XmDragIconObject)NULL);
}


/************************************************************************
 *
 *  DoZapEffect()
 *
 ***********************************************************************/

/*ARGSUSED*/
static void
DoZapEffect(
    XtPointer		clientData,
    XtIntervalId	*id)	/* unused */
{
  XmDragOverShellWidget	dos = (XmDragOverShellWidget)clientData;
  Display		*display = XtDisplay((Widget) dos);
  XSegment	 	segments[4];
  int 			j;
  int 			i = 0;
  int 			rise, run;
  int 			centerX, centerY;
  GC			draw_gc = dos->drag.rootBlend.gc;
  XGCValues		v;
  unsigned long		vmask;
  Window		root = RootWindowOfScreen(XtScreen(dos));

  for (j = 0; j < 4; j++)
    {
      segments[j].x1 = dos->drag.initialX;
      segments[j].y1 = dos->drag.initialY;
    }
  segments[0].x2 = dos->core.x;
  segments[0].y2 = dos->core.y;
  segments[1].x2 = dos->core.x;
  segments[1].y2 = dos->core.y + dos->core.height;
  segments[2].x2 = dos->core.x + dos->core.width;
  segments[2].y2 = dos->core.y + dos->core.height;
  segments[3].x2 = dos->core.x + dos->core.width;
  segments[3].y2 = dos->core.y;

  centerX = dos->core.x + dos->core.width/2;
  centerY = dos->core.y + dos->core.height/2;
  rise = (dos->drag.initialY - centerY) / 5;
  run = (dos->drag.initialX - centerX) / 5;

  /*
   *  Draw the lines and add the timeout.
   */
  v.foreground = dos->drag.cursorForeground;
  v.function = GXxor;
  v.clip_mask = None;
  vmask = GCForeground|GCFunction|GCClipMask;
  XChangeGC (display, draw_gc, vmask, &v);
  XDrawSegments (display, root, draw_gc, segments, 4);
  XFlush(display);

  /*
   * Do an abbreviated zap effect if the rise and run are both small.
   */
  if (((rise <= 3) && (rise >= -3)) && ((run <= 3) && (run >= -3))) {
    i = 5;
  }

  for (; ; i++ )
    {
      /* wait */
      XmeMicroSleep (ZAP_TIME);

      /*
       *  Erase the previously drawn lines and restore the root.
       */

      XDrawSegments (display, root, draw_gc, segments, 4);

      if (dos->drag.activeMode != XmDRAG_WINDOW) {
	v.foreground = dos->drag.cursorForeground;
	v.function = GXcopy;
	vmask = GCForeground|GCFunction;
	XChangeGC (display, draw_gc, vmask, &v);
	XCopyArea (display, BackingPixmap(dos), root,
		   draw_gc,
		   0, 0, dos->core.width, dos->core.height,
		   segments[0].x2, segments[0].y2);
      }

      /* Here is where we always leave the loop */
      if (i == 5) break;

      /*
       *  Compute the new position.
       *  Save the root.
       *  Draw the pixmap (mixedIcon exists) and new lines.
       */

      segments[0].x2 += run;
      segments[0].y2 += rise;
      segments[1].x2 += run;
      segments[1].y2 += rise;
      segments[2].x2 += run;
      segments[2].y2 += rise;
      segments[3].x2 += run;
      segments[3].y2 += rise;

      if (dos->drag.activeMode == XmDRAG_WINDOW) {
	XtMoveWidget((Widget) dos, segments[0].x2, segments[0].y2);
      } else {
	XCopyArea (display, root, BackingPixmap(dos), draw_gc,
		   segments[0].x2, segments[0].y2,
		   dos->core.width, dos->core.height, 0, 0);
	DrawIcon (dos,
		  (dos->drag.rootBlend.mixedIcon ?
		   dos->drag.rootBlend.mixedIcon :
		   dos->drag.cursorBlend.mixedIcon),
		  root, segments[0].x2, segments[0].y2);
      }

      v.foreground = 1;
      v.function = GXxor;
      vmask = GCForeground|GCFunction;
      XChangeGC (display, draw_gc, vmask, &v);
      XDrawSegments (display, root, draw_gc, segments, 4);
      XFlush (display);
    }
  XFlush (display);
}

/************************************************************************
 *
 *  DoMeltEffect()
 *
 *   This function is responsible for restoring the display after a
 *   drop has occurred in a receptive area.  It uses a timeout to create
 *   a venetian blind effect as it removes the drag pixmap, and restores
 *   the original root window contents.
 ***********************************************************************/

/*ARGSUSED*/
static void
DoMeltEffect(
    XtPointer		clientData,
    XtIntervalId	*id)	/* unused */
{
  XmDragOverShellWidget	dos = (XmDragOverShellWidget)clientData;
  int 			iterations;
  int 			i = 0;
  int 			xClipOffset;
  int 			yClipOffset;
  XRectangle 		rects[4];

  if (dos->drag.activeMode == XmDRAG_WINDOW) {
    XRectangle 			rect;
    Dimension			width = XtWidth(dos);
    Dimension			height = XtHeight(dos);

    /*
     *  Determine how much to shrink on each pass.
     */
    if ((xClipOffset = (width / 16)) <= 0) {
	xClipOffset = 1;
    }
    if ((yClipOffset = (height / 16)) <= 0) {
	yClipOffset = 1;
    }

    iterations = MIN(width/(2*xClipOffset), height/(2*yClipOffset));
    /*
     *  Generate the clipping rectangles.
     *  We converge on the center of the cursor.
     */

    rect.x = 0;
    rect.y = 0;
    rect.width = width;
    rect.height = height;

    for (i = 0; i < iterations; i++)
    {
	XShapeCombineRectangles(XtDisplay(dos), XtWindow(dos),
				ShapeBounding, 0, 0,
				&rect, 1, ShapeSet, YXSorted);
	XFlush (XtDisplay((Widget) dos));

	rect.x += xClipOffset;
	rect.width -= 2*xClipOffset;
	rect.y += yClipOffset;
	rect.height -= 2*yClipOffset;

	/* wait */
	XmeMicroSleep (MELT_TIME);
    }
  } else {
    XmDragIconObject    	sourceIcon;
    XmDragOverBlend		blend;
    GC 				draw_gc = dos->drag.rootBlend.gc;

    /*
     *  Blend a new mixedIcon using only the source icon.
     *  Place the new mixedIcon to preserve the source icon location.
     *  The current mixedIcon data is valid.
     */
    if (dos->drag.rootBlend.sourceIcon) {
      sourceIcon = dos->drag.rootBlend.sourceIcon;
      blend = &dos->drag.rootBlend;
    } else {
      sourceIcon = dos->drag.cursorBlend.sourceIcon;
      blend = &dos->drag.cursorBlend;
    }

    /*
     *  Determine how much to shrink on each pass.
     */

    if ((xClipOffset = (sourceIcon->drag.width / 16)) <= 0) {
      xClipOffset = 1;
    }
    if ((yClipOffset = (sourceIcon->drag.height / 16)) <= 0) {
      yClipOffset = 1;
    }

    iterations = MIN(sourceIcon->drag.width/(2*xClipOffset),
		     sourceIcon->drag.height/(2*yClipOffset));
    /*
     *  Generate the clipping rectangles.
     *  We converge on the center of the cursor.
     */

    rects[0].x = dos->core.x;
    rects[0].y = dos->core.y;
    rects[0].width = dos->core.width;
    rects[0].height = blend->sourceY + yClipOffset;

    rects[1].x = rects[0].x + blend->sourceX + sourceIcon->drag.width -
      xClipOffset;
    rects[1].y = rects[0].y + yClipOffset + blend->sourceY;
    rects[1].width = dos->core.width - (rects[1].x - rects[0].x);
    rects[1].height = dos->core.height - ((yClipOffset * 2) + blend->sourceY);

    rects[2].x = rects[0].x;
    rects[2].y = rects[0].y + blend->sourceY + sourceIcon->drag.height -
      yClipOffset;
    rects[2].width = rects[0].width;
    rects[2].height = dos->core.height - (rects[2].y - rects[0].y);

    rects[3].x = rects[0].x;
    rects[3].y = rects[0].y + yClipOffset + blend->sourceY;
    rects[3].width = xClipOffset + blend->sourceX;
    rects[3].height = rects[1].height;

    for (i = 0; i < iterations; i++)
      {
	XSetClipRectangles (XtDisplay((Widget)dos),
			    draw_gc, 0, 0, rects, 4, Unsorted);
	XCopyArea (XtDisplay((Widget)dos),
		   BackingPixmap(dos),
		   RootWindowOfScreen(XtScreen(dos)),
		   draw_gc,
		   0, 0, dos->core.width, dos->core.height,
		   dos->core.x, dos->core.y);
	XFlush (XtDisplay((Widget)dos));

	rects[0].height += yClipOffset;
	rects[1].x -= xClipOffset;
	rects[1].width += xClipOffset;
	rects[2].y -= yClipOffset;
	rects[2].height += yClipOffset;
	rects[3].width += xClipOffset;

	/* wait */
	XmeMicroSleep (MELT_TIME);
      }

    XSetClipMask (XtDisplay((Widget)dos), draw_gc, None);
    XCopyArea (XtDisplay((Widget)dos),
	       BackingPixmap(dos),
	       RootWindowOfScreen(XtScreen(dos)),
	       draw_gc,
	       0, 0, dos->core.width, dos->core.height,
	       dos->core.x, dos->core.y);

    XFlush (XtDisplay((Widget)dos));
  }
}

/************************************************************************
 *
 *  GetIconPosition ()
 *
 *  Get the state or operation icon position, relative to the source.
 ***********************************************************************/

static void
GetIconPosition(
    XmDragOverShellWidget	dos,
    XmDragIconObject		icon,
    XmDragIconObject		sourceIcon,
    Position			*iconX,
    Position			*iconY)
{
    switch ((int) icon->drag.attachment) {

	    default:
		XmeWarning ((Widget) icon, MESSAGE2); /* cast ok here */
            case XmATTACH_NORTH_WEST:
                *iconX = icon->drag.offset_x;
                *iconY = icon->drag.offset_y;
	        break;

            case XmATTACH_NORTH:
                *iconX = ((Position) sourceIcon->drag.width/2)
			 + icon->drag.offset_x;
                *iconY = icon->drag.offset_y;
	        break;

            case XmATTACH_NORTH_EAST:
                *iconX = ((Position) sourceIcon->drag.width)
			 + icon->drag.offset_x;
                *iconY = icon->drag.offset_y;
	        break;

            case XmATTACH_EAST:
                *iconX = ((Position) sourceIcon->drag.width)
			 + icon->drag.offset_x;
                *iconY = ((Position) sourceIcon->drag.height/2)
			 + icon->drag.offset_y;
	        break;

            case XmATTACH_SOUTH_EAST:
                *iconX = ((Position) sourceIcon->drag.width)
			 + icon->drag.offset_x;
                *iconY = ((Position) sourceIcon->drag.height)
			 + icon->drag.offset_y;
	        break;

            case XmATTACH_SOUTH:
                *iconX = ((Position) sourceIcon->drag.width/2)
			 + icon->drag.offset_x;
                *iconY = ((Position) sourceIcon->drag.height)
			 + icon->drag.offset_y;
	        break;

            case XmATTACH_SOUTH_WEST:
                *iconX = icon->drag.offset_x;
                *iconY = ((Position) sourceIcon->drag.height)
			 + icon->drag.offset_y;
	        break;

            case XmATTACH_WEST:
                *iconX = icon->drag.offset_x;
                *iconY = ((Position) sourceIcon->drag.height/2)
			 + icon->drag.offset_y;
	        break;

            case XmATTACH_CENTER:
                *iconX = ((Position) sourceIcon->drag.width/2)
			 + icon->drag.offset_x;
                *iconY = ((Position) sourceIcon->drag.height/2)
			 + icon->drag.offset_y;
	        break;

            case XmATTACH_HOT:
	        {
		    XmDragContext	dc = (XmDragContext)XtParent (dos);
		    Window		root, child;
		    int			rootX, rootY, winX, winY;
		    unsigned int	modMask;
		    XmDragOverShellWidget ref;

		    /*
		     *  This code is only applicable for the stateIcon.
		     *  If the opIcon is XmATTACH_HOT, its hotspot should
		     *  be placed at the stateIcon's hotspot.
		     *
		     *  If this is the first time we are blending the
		     *  stateIcon, we will place its hotspot at the
		     *  same source icon position as the pointer's
		     *  position within the reference widget.
		     *
		     *  Otherwise, we want to keep the icon fixed relative
		     *  to the source.  To do this, we need the mixedIcon
		     *  data to be current.  This means the cursorCache
		     *  must not be used with stateIcon XmATTACH_HOT.
		     */

		    ref = (dc->drag.origDragOver != NULL) ?
		          dc->drag.origDragOver : dos;

		    if (ref->drag.rootBlend.mixedIcon) {
                        *iconX = ref->drag.rootBlend.mixedIcon->drag.hot_x
                                 - ref->drag.rootBlend.sourceX
				 - icon->drag.hot_x;
                        *iconY = ref->drag.rootBlend.mixedIcon->drag.hot_y
                                 - ref->drag.rootBlend.sourceY
				 - icon->drag.hot_y;
                    }
		    else if (ref->drag.cursorBlend.mixedIcon) {
                        *iconX = ref->drag.cursorBlend.mixedIcon->drag.hot_x
                                 - ref->drag.cursorBlend.sourceX
				 - icon->drag.hot_x;
                        *iconY = ref->drag.cursorBlend.mixedIcon->drag.hot_y
                                 - ref->drag.cursorBlend.sourceY
				 - icon->drag.hot_y;
                    }
                    else {
			Widget		sourceWidget;
			Dimension	borderW = 0;
			Dimension	highlightT = 0;
			Dimension	shadowT = 0;
			Cardinal	ac;
			Arg		al[3];

			/*
			 *  First time:  get position from pointer,
			 *  adjusting for sourceWidget's border, highlight,
			 *  and shadow.
			 */

			sourceWidget = dc->drag.sourceWidget;

			ac = 0;
			XtSetArg (al[ac], XmNborderWidth, &borderW); ac++;
			XtSetArg (al[ac], XmNhighlightThickness,
				  &highlightT); ac++;
			XtSetArg (al[ac], XmNshadowThickness,
				  &shadowT); ac++;
			XtGetValues (sourceWidget, al, ac);

		        XQueryPointer (XtDisplay (dos),
			               XtWindow (sourceWidget),
			               &root, &child, &rootX, &rootY,
			               &winX, &winY, &modMask);

                        *iconX = winX - icon->drag.hot_x -
                                 borderW - highlightT - shadowT;
                        *iconY = winY - icon->drag.hot_y -
				 borderW - highlightT - shadowT;
		    }
	        }
	        break;
    }
}

/************************************************************************
 *  BlendIcon ()
 *
 *  Blend the icon mask and pixmap into mixedIcon.
 ***********************************************************************/

static void
BlendIcon(
    XmDragOverShellWidget dos,
    XmDragIconObject	icon,
    XmDragIconObject	mixedIcon,
#if NeedWidePrototypes
    int			iconX,
    int			iconY,
#else
    Position		iconX,
    Position		iconY,
#endif /* NeedWidePrototypes */
    GC			maskGC,
    GC			pixmapGC)
{
  Display		*display = XtDisplay(dos);
  Position		sourceX = 0;
  Position		sourceY = 0;
  Position		destX = iconX;
  Position		destY = iconY;
  Dimension		destWidth = icon->drag.width;
  Dimension		destHeight = icon->drag.height;
  XGCValues		v;
  unsigned long		vmask;

  if (icon->drag.pixmap != XmUNSPECIFIED_PIXMAP) {

    /* Clip to the destination drawable. */
    if (destX < 0) {
      sourceX -= destX;	/* > 0 */
      destX = 0;
      if (destWidth <= (Dimension) sourceX) {
	return;
      }
      destWidth -= sourceX;
    }
    if (destX + destWidth > mixedIcon->drag.width) {
      if ((Dimension) destX >= mixedIcon->drag.width) {
	return;
      }
      destWidth = mixedIcon->drag.width - destX;
    }

    if (destY < 0) {
      sourceY -= destY;	/* > 0 */
      destY = 0;
      if (destHeight <= (Dimension) sourceY) {
	return;
      }
      destHeight -= sourceY;
    }
    if (destY + destHeight > mixedIcon->drag.height) {
      if ((Dimension) destY >= mixedIcon->drag.height) {
	return;
      }
      destHeight = mixedIcon->drag.height - destY;
    }

    v.clip_mask = None;
    vmask = GCClipMask;

    if (icon->drag.mask != XmUNSPECIFIED_PIXMAP) {
      /* Union the masks */
      v.function = GXor;
      vmask |= GCFunction;
      XChangeGC(display, maskGC, vmask, &v);
      XCopyArea(display, icon->drag.mask,
		mixedIcon->drag.mask, maskGC,
		sourceX, sourceY,
		mixedIcon->drag.width,
		mixedIcon->drag.height,
		destX, destY);
      v.clip_mask = icon->drag.mask;
      v.clip_x_origin = destX;
      v.clip_y_origin = destY;
      vmask = GCClipMask|GCClipXOrigin|GCClipYOrigin;
    } else {
      /* Create empty mask if we're masking and the blend icon
	 has no mask */
      if (mixedIcon->drag.mask != XmUNSPECIFIED_PIXMAP){
	v.function = GXset;
	vmask |= GCFunction;
	XChangeGC (display, maskGC, vmask, &v);
	XFillRectangle (display, mixedIcon->drag.mask, maskGC,
			destX, destY, destWidth, destHeight);
      }
    }

    /* Join the regions if they both have them,  otherwise,
       remove the region */
    if (icon->drag.region != NULL && mixedIcon->drag.region != NULL) {
      if (icon->drag.x_offset || icon->drag.y_offset)
	XOffsetRegion(icon->drag.region, -icon->drag.x_offset,
		      -icon->drag.y_offset);

      XOffsetRegion(icon->drag.region, destX, destY);
      icon->drag.x_offset = destX;
      icon->drag.y_offset = destY;

      XUnionRegion(mixedIcon->drag.region, icon->drag.region,
		   mixedIcon->drag.region);
    } else {
      if (mixedIcon->drag.region != NULL)
	XDestroyRegion(mixedIcon->drag.region);
      mixedIcon->drag.region = NULL;
    }

    /*
     *  Copy the pixmap.
     */
    if (mixedIcon->drag.depth > 1) {
      v.foreground = dos->drag.cursorForeground;
      v.background = dos->drag.cursorBackground;
    } else {
      /* Else B & W */
      v.foreground = 1;
      v.background = 0;
    }
    v.function = GXcopy;
    vmask |= GCFunction|GCForeground|GCBackground;
    XChangeGC (display, pixmapGC, vmask, &v);

    if (icon->drag.depth == 1) {
      XCopyPlane (display, icon->drag.pixmap,
		  mixedIcon->drag.pixmap, pixmapGC,
		  sourceX, sourceY,
		  icon->drag.width,
		  icon->drag.height,
		  destX, destY,
		  1L);
    } else if (icon->drag.depth == mixedIcon->drag.depth) {
      XCopyArea (display, icon->drag.pixmap,
		 mixedIcon->drag.pixmap, pixmapGC,
		 sourceX, sourceY,
		 icon->drag.width,
		 icon->drag.height,
		 destX, destY);
    } else {
      XmeWarning ((Widget) icon, MESSAGE1); /* cast ok here */
    }
  }
}

/************************************************************************
 *
 *  MixedIconSize ()
 *
 *  Determine the dimensions of the mixedIcon.
 ***********************************************************************/

static void
MixedIconSize(
    XmDragOverShellWidget	dos,
    XmDragIconObject		sourceIcon,
    XmDragIconObject		stateIcon,
    XmDragIconObject		opIcon,
    Dimension			*width,
    Dimension			*height)
{
    Position		sourceX = 0, sourceY = 0;
    Position		minX = 0, minY = 0;
    Position		stateX = 0, stateY = 0;
    Position		opX, opY;
    Position		maxX, maxY;

    if (stateIcon) {
	GetIconPosition (dos, stateIcon, sourceIcon, &stateX, &stateY);
        minX = MIN(stateX, minX);
        minY = MIN(stateY, minY);
    }

    if (opIcon) {
	if (opIcon->drag.attachment == XmATTACH_HOT && stateIcon) {
	    opX = stateX + stateIcon->drag.hot_x - opIcon->drag.hot_x;
	    opY = stateY + stateIcon->drag.hot_y - opIcon->drag.hot_y;
	}
	else {
	    GetIconPosition (dos, opIcon, sourceIcon, &opX, &opY);
	}
        minX = MIN(opX, minX);
        minY = MIN(opY, minY);
    }

    sourceX -= minX;	/* >= 0 */
    sourceY -= minY;	/* >= 0 */

    maxX = sourceX + sourceIcon->drag.width;
    maxY = sourceY + sourceIcon->drag.height;

    if (stateIcon) {
	stateX -= minX;
	stateY -= minY;
        maxX = MAX(stateX + ((Position) stateIcon->drag.width), maxX);
        maxY = MAX(stateY + ((Position) stateIcon->drag.height), maxY);
    }

    if (opIcon) {
	opX -= minX;
	opY -= minY;
        maxX = MAX(opX + ((Position) opIcon->drag.width), maxX);
        maxY = MAX(opY + ((Position) opIcon->drag.height), maxY);
    }

    *width = maxX;
    *height = maxY;
}

/************************************************************************
 *
 *  DestroyMixedIcon ()
 *
 *  The MixedIcon's pixmap and mask, if present, were scratch pixmaps,
 *  and not from the Xm pixmap cache.  Therefore, they need to be freed
 *  separately and reset to XmUNSPECIFIED_PIXMAP.
 ***********************************************************************/

static void
DestroyMixedIcon(
    XmDragOverShellWidget	dos,
    XmDragIconObject		mixedIcon)
{
    XmScreen		xmScreen = (XmScreen) XmGetXmScreen(XtScreen(dos));
    MixedIconCache 	*cache_ptr;
    MixedIconCache 	*prev_cache_ptr = NULL;

    if (mixedIcon->drag.pixmap != XmUNSPECIFIED_PIXMAP) {
        _XmFreeScratchPixmap (xmScreen, mixedIcon->drag.pixmap);
        mixedIcon->drag.pixmap = XmUNSPECIFIED_PIXMAP;
    }

    if (mixedIcon->drag.mask != XmUNSPECIFIED_PIXMAP) {
	_XmFreeScratchPixmap (xmScreen, mixedIcon->drag.mask);
        mixedIcon->drag.mask = XmUNSPECIFIED_PIXMAP;
    }

    _XmProcessLock();
    cache_ptr = mixed_cache;

    while(cache_ptr) {
      MixedIconCache *next_cache_ptr = cache_ptr -> next;

      if (cache_ptr->mixedIcon == mixedIcon) {
	if (cache_ptr == mixed_cache) {
	  prev_cache_ptr = mixed_cache = cache_ptr->next;
	} else {
	  prev_cache_ptr->next = cache_ptr->next;
	}
	XtFree((char *) cache_ptr);
      } else {
	prev_cache_ptr = cache_ptr;
      }

      cache_ptr = next_cache_ptr;
    }
    _XmProcessUnlock();

    XtDestroyWidget ((Widget) mixedIcon);
}

/************************************************************************
 *
 *  MixIcons ()
 *
 ***********************************************************************/

static void
MixIcons(
    XmDragOverShellWidget	dos,
    XmDragIconObject		sourceIcon,
    XmDragIconObject		stateIcon,
    XmDragIconObject		opIcon,
    XmDragOverBlendRec		*blendPtr,
#if NeedWidePrototypes
    int				clip)
#else
    Boolean			clip)
#endif /* NeedWidePrototypes */
{
    Display		*display = XtDisplay(dos);
    XmScreen		xmScreen = (XmScreen) XmGetXmScreen(XtScreen(dos));
    XmDragIconObject	mixedIcon = blendPtr->mixedIcon;
    XmDragOverBlendRec	*cursorBlend = &dos->drag.cursorBlend;
    Arg			al[8];
    Cardinal		ac;
    Pixmap		pixmap, mask;
    Position		sourceX = 0, sourceY = 0;
    Position		stateX = 0, stateY = 0;
    Position		opX = 0, opY = 0;
    Position		minX = 0, minY = 0;
    Position		maxX, maxY;
    Position		hotX, hotY;
    Dimension		width, height;
    Cardinal		depth;
    Boolean		need_mask = True;
    Boolean		do_cache = True;
    XGCValues		v;
    unsigned long	vmask;

    dos->drag.holePunched = False; /* Force update */

    /*
     *  Determine the dimensions of the blended icon and the positions
     *  of each component within it.
     */

    if (stateIcon) {
	GetIconPosition (dos, stateIcon, sourceIcon, &stateX, &stateY);
        minX = MIN(stateX, minX);
        minY = MIN(stateY, minY);
    }

    /*
     *  If the opIcon's attachment is XmATTACH_HOT, attach its hotspot
     *  to the stateIcon's hotspot -- the blended icon's hotspot will be
     *  there, and the blended icon will be positioned so that the blended
     *  hotspot is placed at the cursor position.
     */

    if (opIcon) {
	if (opIcon->drag.attachment == XmATTACH_HOT && stateIcon) {
	    opX = stateX + stateIcon->drag.hot_x - opIcon->drag.hot_x;
	    opY = stateY + stateIcon->drag.hot_y - opIcon->drag.hot_y;
	}
	else {
	    GetIconPosition (dos, opIcon, sourceIcon, &opX, &opY);
	}
        minX = MIN(opX, minX);
        minY = MIN(opY, minY);
    }

    sourceX -= minX;	/* >= 0 */
    sourceY -= minY;	/* >= 0 */

    maxX = sourceX + sourceIcon->drag.width;
    maxY = sourceY + sourceIcon->drag.height;

    if (stateIcon) {
	stateX -= minX;
	stateY -= minY;
        maxX = MAX(stateX + ((Position) stateIcon->drag.width), maxX);
        maxY = MAX(stateY + ((Position) stateIcon->drag.height), maxY);
        hotX = stateX + stateIcon->drag.hot_x;
	hotY = stateY + stateIcon->drag.hot_y;
    }
    else {
	hotX = sourceX + sourceIcon->drag.hot_x;
	hotY = sourceY + sourceIcon->drag.hot_y;
    }

    if (opIcon) {
	opX -= minX;
	opY -= minY;
        maxX = MAX(opX + ((Position) opIcon->drag.width), maxX);
        maxY = MAX(opY + ((Position) opIcon->drag.height), maxY);
    }

    width = maxX;
    height = maxY;

    depth = ((blendPtr == cursorBlend) ? 1 : dos->core.depth);

    /*
     *  If we are clipping the blended icon to fit within a cursor,
     *  we clip it around the hotspot.
     */

    if (clip) {
        Dimension	maxWidth, maxHeight;

	XmeQueryBestCursorSize((Widget) dos, &maxWidth, &maxHeight);

	/* Pick new bounds to get the maximum icon possible while
	   including the hotspot within the icon bounds */
	if (width > maxWidth) {
	  minX = MAX(0, hotX - ((Position) maxWidth) / 2);
	  minX = MIN(minX, width - ((Position) maxWidth));
	  hotX -= minX;
	  sourceX -= minX;
	  stateX -= minX;
	  opX -= minX;
	  width = maxWidth;
	}
	if (height > maxHeight) {
	  minY = MAX(0, hotY - ((Position) maxHeight) / 2);
	  minY = MIN(minY, height - ((Position) maxHeight));
	  hotY -= minY;
	  sourceY -= minY;
	  stateY -= minY;
	  opY -= minY;
	  height = maxHeight;
	}
    }

     mixedIcon = GetMixedIcon(dos, depth, width, height, sourceIcon, stateIcon,
			    opIcon, sourceX, sourceY, stateX, stateY, opX, opY);
    /*
     *  Create the blended XmDragIcon object or use the current one
     *  if it is the correct size and depth.
     */

    if (mixedIcon != NULL) {
       blendPtr->mixedIcon = mixedIcon;
       do_cache = False;
    }

    /*
     *  The blended icon needs a mask unless none of its component icons
     *  has a mask and its component icons completely cover it.
     */

    if ((sourceIcon->drag.mask == XmUNSPECIFIED_PIXMAP) &&
	(!stateIcon || stateIcon->drag.mask == XmUNSPECIFIED_PIXMAP) &&
	(!opIcon || opIcon->drag.mask == XmUNSPECIFIED_PIXMAP)) {

	XRectangle	rect;
	Region		source = XCreateRegion();
	Region		dest = XCreateRegion();
	Region		tmp;

	rect.x = (short) sourceX;
	rect.y = (short) sourceY;
	rect.width = (unsigned short) sourceIcon->drag.width;
	rect.height = (unsigned short) sourceIcon->drag.height;
	XUnionRectWithRegion (&rect, source, dest);

	if (stateIcon) {
	    tmp = source;
	    source = dest;
	    dest = tmp;

	    rect.x = (short) stateX;
	    rect.y = (short) stateY;
	    rect.width = (unsigned short) stateIcon->drag.width;
	    rect.height = (unsigned short) stateIcon->drag.height;
	    XUnionRectWithRegion (&rect, source, dest);
	}

	if (opIcon) {
	    tmp = source;
	    source = dest;
	    dest = tmp;

	    rect.x = (short) opX;
	    rect.y = (short) opY;
	    rect.width = (unsigned short) opIcon->drag.width;
	    rect.height = (unsigned short) opIcon->drag.height;
	    XUnionRectWithRegion (&rect, source, dest);
	}

	if (RectangleIn == XRectInRegion (dest, 0, 0, width, height)) {
	    need_mask = False;
	}

	XDestroyRegion (source);
	XDestroyRegion (dest);
    }

    if (mixedIcon == NULL) {
	pixmap = _XmAllocScratchPixmap (xmScreen, depth, width, height);

        mask = XmUNSPECIFIED_PIXMAP;

	ac = 0;
	XtSetArg(al[ac], XmNpixmap, pixmap); ac++;
	XtSetArg(al[ac], XmNmask, mask); ac++;
	XtSetArg(al[ac], XmNdepth, depth); ac++;
	XtSetArg(al[ac], XmNwidth, width); ac++;
	XtSetArg(al[ac], XmNheight, height); ac++;
	XtSetArg(al[ac], XmNhotX, hotX); ac++;
	XtSetArg(al[ac], XmNhotY, hotY); ac++;
	mixedIcon = blendPtr->mixedIcon = (XmDragIconObject)
	        XmCreateDragIcon ((Widget) xmScreen, "mixedIcon", al, ac);

	if (need_mask) {
	   mask = mixedIcon->drag.mask = _XmAllocScratchPixmap (xmScreen, 1,
							        width, height);
	}
    }
    else {
	pixmap = mixedIcon->drag.pixmap;
	mixedIcon->drag.hot_x = hotX;
	mixedIcon->drag.hot_y = hotY;
	if (need_mask && mixedIcon->drag.mask == XmUNSPECIFIED_PIXMAP) {
	    mixedIcon->drag.mask =
	        _XmAllocScratchPixmap (xmScreen, 1, width, height);
	}
	mask = mixedIcon->drag.mask;
    }

    if (sourceIcon->drag.region != NULL) {
       if (mixedIcon->drag.region != NULL)
          XDestroyRegion(mixedIcon->drag.region);

       mixedIcon->drag.region = XCreateRegion();
    }

    /*
     *  Get the cursorBlend GC if needed (the root GC already exists).
     *  Set the pixmap to its background color.
     *  Clear any mask.
     */

    if (blendPtr->gc == NULL) {
	v.background = 0;
	v.foreground = 1;
	v.function = GXset;
	v.graphics_exposures = False;
	v.subwindow_mode = IncludeInferiors;
	v.clip_mask = None;
        v.clip_x_origin = 0;	/* pedantic */
        v.clip_y_origin = 0;	/* pedantic */
	vmask = GCBackground|GCForeground|GCFunction|
	        GCClipXOrigin|GCClipYOrigin|GCClipMask|
		GCGraphicsExposures|GCSubwindowMode;
	blendPtr->gc = XtAllocateGC((Widget) dos, mixedIcon -> drag.depth,
				    vmask, &v, vmask, 0L);
    }
    else {
	v.clip_mask = None;
	v.function = GXset;
	vmask = GCClipMask|GCFunction;
        XChangeGC (display, blendPtr->gc, vmask, &v);
    }

    XFillRectangle (display, pixmap, blendPtr->gc,
		    0, 0, mixedIcon->drag.width, mixedIcon->drag.height);

    if (mask != XmUNSPECIFIED_PIXMAP) {
	if (cursorBlend->gc == NULL) {
	    v.background = 0;
	    v.foreground = 1;
	    v.function = GXclear;
	    v.graphics_exposures = False;
	    v.subwindow_mode = IncludeInferiors;
	    v.clip_mask = None;
            v.clip_x_origin = 0;	/* pedantic */
            v.clip_y_origin = 0;	/* pedantic */
	    vmask = GCBackground|GCForeground|GCFunction|
	            GCClipXOrigin|GCClipYOrigin|GCClipMask|
		    GCGraphicsExposures|GCSubwindowMode;
	    cursorBlend->gc = XtAllocateGC((Widget) dos, 1,
					   vmask, &v, vmask, 0L);
	}
	else {
	    v.function = GXclear;
	    vmask = GCFunction;
	    if (cursorBlend->gc != blendPtr->gc) {
	       v.clip_mask = None;
	       vmask |= GCClipMask;
            }
            XChangeGC (display, cursorBlend->gc, vmask, &v);
        }

	XFillRectangle (display, mixedIcon->drag.mask, cursorBlend->gc,
			0, 0, mixedIcon->drag.width, mixedIcon->drag.height);
    }

    /*
     *  Blend the icons into mixedIcon.
     */

    BlendIcon (dos, sourceIcon, mixedIcon,
	       sourceX, sourceY, cursorBlend->gc, blendPtr->gc);
    blendPtr->sourceX = sourceX;
    blendPtr->sourceY = sourceY;

    if (stateIcon) {
        BlendIcon (dos, stateIcon, mixedIcon,
		   stateX, stateY, cursorBlend->gc, blendPtr->gc);
    }

    if (opIcon) {
        BlendIcon (dos, opIcon, mixedIcon,
		   opX, opY, cursorBlend->gc, blendPtr->gc);
    }

    if (mixedIcon->drag.region != NULL) {
       XRectangle rect;

       if (mixedIcon->drag.restore_region != NULL)
          XDestroyRegion(mixedIcon->drag.restore_region);

       mixedIcon->drag.restore_region = XCreateRegion();

       rect.x = 0;
       rect.y = 0;
       rect.width = mixedIcon->drag.width;
       rect.height = mixedIcon->drag.height;

       XUnionRectWithRegion(&rect, mixedIcon->drag.restore_region,
                            mixedIcon->drag.restore_region);
       XSubtractRegion(mixedIcon->drag.restore_region, mixedIcon->drag.region,
                       mixedIcon->drag.restore_region);
    }

    if (do_cache) {
      CacheMixedIcon(dos, depth, width, height, sourceIcon, stateIcon, opIcon,
		     sourceX, sourceY, stateX, stateY, opX, opY, mixedIcon);
    } /* do_cache */
}

/************************************************************************
 *
 *  FitsInCursor ()
 *
 ***********************************************************************/

static Boolean
FitsInCursor(
    XmDragOverShellWidget	dos,
    XmDragIconObject		sourceIcon,
    XmDragIconObject		stateIcon,
    XmDragIconObject		opIcon)
{
    Dimension		maxWidth, maxHeight;
    Dimension		width, height;

    if (((sourceIcon->drag.depth != 1) ||
	 (sourceIcon->drag.pixmap == XmUNSPECIFIED_PIXMAP))) {
	return False;
    }

    MixedIconSize (dos, sourceIcon, stateIcon, opIcon, &width, &height);
    XmeQueryBestCursorSize((Widget)dos, &maxWidth, &maxHeight);

    if (width > maxWidth || height > maxHeight) {
	return False;
    }
    return True;
}

/************************************************************************
 *
 *  GetDragIconColors ()
 *
 *  Gets the cursor colors.  Creates or recolors the root's gc.
 ***********************************************************************/

static Boolean
GetDragIconColors(
    XmDragOverShellWidget	dos )
{
    XmDragContext	dc = (XmDragContext)XtParent(dos);
    Screen		*screen = XtScreen(dos);
    Display		*display = XtDisplay(dos);
    Boolean		doChange = False;
    Pixel		fg;
    Pixel		bg;
    XGCValues	        v;
    unsigned long	vmask;
    XColor 		colors[2];
    Colormap		colormap;

    colormap = dc->core.colormap;
    bg = dc->drag.cursorBackground;
    switch ((int) dos->drag.cursorState) {

	case XmVALID_DROP_SITE:
            fg = dc->drag.validCursorForeground;
	    break;

	case XmINVALID_DROP_SITE:
            fg = dc->drag.invalidCursorForeground;
	    break;

	default:
	    XmeWarning ((Widget) dos, MESSAGE3);
	case XmNO_DROP_SITE:
            fg = dc->drag.noneCursorForeground;
	    break;
    }

    /*
     *  Find the best RGB fit for fg and bg on the current screen.
     *  If XAllocColor() fails or if the fitted fg and bg are the same on the
     *  current screen, use black and white respectively.
     */

    colors[0].pixel = fg;
    colors[1].pixel = bg;
    XQueryColors(display, colormap, colors, 2);

    fg = BlackPixelOfScreen(screen);
    bg = WhitePixelOfScreen(screen);
    if (XAllocColor(display, DefaultColormapOfScreen(screen), &colors[0]) &&
        XAllocColor(display, DefaultColormapOfScreen(screen), &colors[1])) {
	fg = colors[0].pixel;
	bg = colors[1].pixel;

    	if (fg == bg) {
    	    fg = BlackPixelOfScreen(screen);
    	    bg = WhitePixelOfScreen(screen);
    	}
    }

    /*
     *  Create or recolor the root's gc.
     *  The cursorForeground and cursorBackground are first set
     *  when the root's gc is created.
     */

    if (dos->drag.rootBlend.gc == NULL) {
	doChange = True;
	v.background = dos->drag.cursorBackground = bg;
	v.foreground = dos->drag.cursorForeground = fg;
	v.graphics_exposures = False;
	v.subwindow_mode = IncludeInferiors;
	v.clip_mask = None;
        v.clip_x_origin = 0;	/* pedantic */
        v.clip_y_origin = 0;	/* pedantic */
	vmask = GCBackground|GCForeground|
	        GCClipXOrigin|GCClipYOrigin|GCClipMask|
	        GCGraphicsExposures|GCSubwindowMode;
	dos->drag.rootBlend.gc = XtAllocateGC ((Widget) dos,
					       DefaultDepthOfScreen(screen),
					       vmask, &v, vmask, 0L);
    }
    else if (dos->drag.cursorBackground != bg ||
	     dos->drag.cursorForeground != fg) {
	doChange = True;
	v.background = dos->drag.cursorBackground = bg;
	v.foreground = dos->drag.cursorForeground = fg;
	vmask = GCBackground|GCForeground;
	XChangeGC (display, dos->drag.rootBlend.gc, vmask, &v);
    }
    return (doChange);
}

/************************************************************************
 *
 *  GetDragIconCursor ()
 *
 *  Tries to create a pixmap cursor with correct colors.
 *  Creates and/or colors the root's gc.
 ***********************************************************************/

static Cursor
GetDragIconCursor(
    XmDragOverShellWidget	dos,
    XmDragIconObject		sourceIcon,
    XmDragIconObject		stateIcon,
    XmDragIconObject		opIcon,
#if NeedWidePrototypes
    int				clip,
    int				dirty)
#else
    Boolean			clip,
    Boolean			dirty)
#endif /* NeedWidePrototypes */
{
    Screen			*screen = XtScreen(dos);
    Display			*display = XtDisplay(dos);
    XmDragCursorCache		*cursorCachePtr= NULL;
    register XmDragCursorCache	cursorCache = NULL;
    XColor 			colors[2];
    Boolean			useCache = True;
    Cursor			cursor;
    XmDragIconObject		dirtysourceIcon = NULL;
    XmDragIconObject		dirtystateIcon = NULL;
    XmDragIconObject		dirtyopIcon = NULL;

    /*
     *  If the cursor doesn't fit and we cannot clip, return None.
     *  Don't look in the cursorCache -- the cursor would only be there
     *    if clipping were allowed.
     */

    if (!clip && !FitsInCursor (dos, sourceIcon, stateIcon, opIcon)) {
	return None;
    }

    /*
     *  Don't use the cursorCache with stateIcon attachment XmATTACH_HOT
     *  (opIcon attachment XmATTACH_HOT is OK).
     */

    colors[0].pixel = dos->drag.cursorForeground;
    colors[1].pixel = dos->drag.cursorBackground;
    XQueryColors(display, DefaultColormapOfScreen(screen), colors, 2);

    /*
     * Fix for CR 4817 - If one of the icons is dirty, check the cache to see
     *                   which cursors use the dirty icon.  When found, mark
     *                   the cache as dirty.
     */
    cursorCachePtr = _XmGetDragCursorCachePtr((XmScreen)XmGetXmScreen(screen));
    if (dirty)
      {
	cursorCache = *cursorCachePtr;
	if (sourceIcon->drag.isDirty)
	  dirtysourceIcon = sourceIcon;
	if (stateIcon && (stateIcon->drag.isDirty))
	  dirtystateIcon = stateIcon;
	if (opIcon && (opIcon->drag.isDirty))
	  dirtyopIcon = opIcon;

	while (cursorCache)
	  {
	    if ((dirtystateIcon && (cursorCache->stateIcon == dirtystateIcon)) ||
		(dirtysourceIcon && (cursorCache->sourceIcon == dirtysourceIcon)) ||
		(dirtyopIcon && (cursorCache->opIcon == dirtyopIcon)))
	      cursorCache->dirty = True;
	    cursorCache = cursorCache->next;
	  }
      }
    /*
     * End Fix for CR 4817
     */

    if (stateIcon && stateIcon->drag.attachment == XmATTACH_HOT) {
	useCache = False;
    }
    else {
	cursorCachePtr =
	    _XmGetDragCursorCachePtr((XmScreen)XmGetXmScreen(screen));

	cursorCache = *cursorCachePtr;
	while (cursorCache) {
            if ((cursorCache->stateIcon == stateIcon) &&
                (cursorCache->opIcon == opIcon) &&
	        (cursorCache->sourceIcon == sourceIcon)) {

		/*
		 *  Found a cursorCache match.
		 *  If any of the icons were dirty, replace this cursor.
		 *  Otherwise, use it.
		 */

		if (cursorCache -> dirty) {
		    break;
		}

	        /* recolor the cursor */
	        XRecolorCursor (display, cursorCache->cursor,
			        &colors[0], /* foreground_color */
			        &colors[1]  /* background_color */);
	        return cursorCache->cursor;
	    }
	    else {
	        cursorCache = cursorCache->next;
	    }
	}
    }

    /*
     *  We didn't find a valid match in the cursorCache.
     *  Blend the icons and create a pixmap cursor.
     */

    MixIcons (dos, sourceIcon, stateIcon, opIcon,
	      &dos->drag.cursorBlend, clip);

    cursor =
	XCreatePixmapCursor (display,
			     dos->drag.cursorBlend.mixedIcon->drag.pixmap,
	((dos->drag.cursorBlend.mixedIcon->drag.mask == XmUNSPECIFIED_PIXMAP) ?
		None : dos->drag.cursorBlend.mixedIcon->drag.mask),
			     &colors[0], /* foreground_color */
			     &colors[1], /* background_color */
			     dos->drag.cursorBlend.mixedIcon->drag.hot_x,
			     dos->drag.cursorBlend.mixedIcon->drag.hot_y);

    /*
     *  Cache the cursor if using the cursorCache.  If the cached cursor
     *    was dirty, replace it.  Otherwise, create a new cursorCache entry
     *    at the head of the cache.
     *  Otherwise, save it and free any previously saved cursor.
     */

    if (useCache) {
	if (cursorCache) {
	    XFreeCursor (display, cursorCache->cursor);
	}
	else {
	    cursorCache = XtNew(XmDragCursorRec);
	    cursorCache->sourceIcon = sourceIcon;
	    cursorCache->stateIcon = stateIcon;
	    cursorCache->opIcon = opIcon;
	    cursorCache->next = *cursorCachePtr;
	    *cursorCachePtr = cursorCache;
	}
        cursorCache->dirty = False;
	cursorCache->cursor = cursor;
    }
    else {
        if (dos->drag.ncCursor != None) {
	    XFreeCursor (display, dos->drag.ncCursor);
        }
        dos->drag.ncCursor = cursor;
    }

    return cursor;
}

/************************************************************************
 *
 *  Initialize ()
 *
 ***********************************************************************/

/*ARGSUSED*/
static void
Initialize(
    Widget	req,		/* unused */
    Widget	new_w,
    ArgList	args,		/* unused */
    Cardinal	*numArgs)	/* unused */
{
    XmDragOverShellWidget	dos = (XmDragOverShellWidget)new_w;

    /* Assure that *geometry will never affect this widget,  CR 5778 */
    dos->shell.geometry = NULL;

    dos->drag.opIcon = dos->drag.stateIcon = NULL;
    dos->drag.cursorBlend.gc =
	dos->drag.rootBlend.gc = NULL;
    dos->drag.cursorBlend.sourceIcon =
	dos->drag.cursorBlend.mixedIcon =
	    dos->drag.rootBlend.sourceIcon =
		dos->drag.rootBlend.mixedIcon = NULL;
    dos->drag.backing.pixmap = dos->drag.tmpPix =
	dos->drag.tmpBit = XmUNSPECIFIED_PIXMAP;

    dos->drag.initialX = dos->drag.hotX;
    dos->drag.initialY = dos->drag.hotY;
    dos->drag.ncCursor = None;
    dos->drag.isVisible = False;
    dos->drag.activeCursor = None;

    /*
     *  Width/height are valid only in XmPIXMAP and XmWINDOW active modes.
     */

    dos->core.width = 0;
    dos->core.height = 0;

    dos->drag.activeMode = XmCURSOR;

    /*
     * Get the DragOverShell out of the business of adding and removing
     * grabs for shell modality.
     */
    XtRemoveAllCallbacks( new_w, XmNpopupCallback) ;
    XtRemoveAllCallbacks( new_w, XmNpopdownCallback) ;

    /* Setup information for window dragging */
    dos->drag.holePunched = FALSE;
    dos->drag.colormapShell = (Widget) NULL;
    dos->drag.colormapWidget = (Widget) NULL;
    DragOverShellColormapWidget(new_w, XtParent(new_w));

    _XmDragOverChange (new_w, XmNO_DROP_SITE);
}

/************************************************************************
 *
 *  SetValues
 *
 ************************************************************************/

/*ARGSUSED*/
static Boolean
SetValues(
    Widget	current,
    Widget	req,		/* unused */
    Widget	new_w,
    ArgList	args,		/* unused */
    Cardinal	*num_args)	/* unused */
{
    XmDragOverShellWidget	dos = (XmDragOverShellWidget) new_w;
    XmDragOverShellWidget	oldDos = (XmDragOverShellWidget) current;
    XmDragContext		dc = (XmDragContext)XtParent(dos);

    /* If the hotspot or geometry changes we'll need to punch a
       new hole in the shaped window */
    if (oldDos->drag.hotX != dos->drag.hotX ||
	oldDos->drag.hotY != dos->drag.hotY ||
	oldDos->core.width != dos->core.width ||
	oldDos->core.height != dos->core.height)
      dos->drag.holePunched = FALSE;

    /*
     *  A mode change handles a change in hotspot automatically.
     *  If we are in XmPIXMAP mode and we haven't yet done a root blend,
     *  try XmCURSOR active mode.
     */

    if (oldDos->drag.mode != dos->drag.mode &&
	dc->drag.blendModel != XmBLEND_NONE) {
	if ((dos->drag.mode == XmPIXMAP ||
	     dos->drag.mode == XmDRAG_WINDOW) &&
	    (dos->drag.rootBlend.sourceIcon == NULL)) {
	    ChangeActiveMode(dos, XmCURSOR);
	}
	else {
	    ChangeActiveMode(dos, dos->drag.mode);
	}
    }
    else if ((dos->drag.hotX != oldDos->drag.hotX) ||
             (dos->drag.hotY != oldDos->drag.hotY)) {
	_XmDragOverMove (new_w, dos->drag.hotX, dos->drag.hotY);
    }
    return False;
}

/************************************************************************
 *
 *  DrawIcon ()
 *
 ************************************************************************/

static void
DrawIcon(
    XmDragOverShellWidget	dos,
    XmDragIconObject		icon,
    Window			window,
#if NeedWidePrototypes
    int				x,
    int				y)
#else
    Position			x,
    Position			y)
#endif /* NeedWidePrototypes */
{
    GC		draw_gc = dos->drag.rootBlend.gc;
    Boolean	clipped = False;
    XGCValues	        v;
    unsigned long	vmask;
    Display * 	display = XtDisplay((Widget)dos);

    v.function = GXcopy;
    vmask = GCFunction;

    if (icon->drag.region == NULL &&
        icon->drag.mask != XmUNSPECIFIED_PIXMAP) {
	v.clip_mask = icon->drag.mask;
	v.clip_x_origin = x;
	v.clip_y_origin = y;
	vmask |= GCClipMask|GCClipXOrigin|GCClipYOrigin;
	XChangeGC (display, draw_gc, vmask, &v);
	clipped = True;
    }
    else {
        if (icon->drag.region != NULL) {
	   XSetRegion(display, draw_gc, icon->drag.region);
	   v.clip_x_origin = x;
	   v.clip_y_origin = y;
	   vmask |= GCClipXOrigin|GCClipYOrigin;
	   XChangeGC (display, draw_gc, vmask, &v);
	   clipped = True;
        }
        else {
	   v.clip_mask = None;
	   vmask |= GCClipMask;
	   XChangeGC (display, draw_gc, vmask, &v);
        }
    }

    /*
     *  If the icon is from the cursorBlend, treat the icon as a bitmap
     *  and use XCopyPlane.  Otherwise, treat it as a pixmap and use
     *  XCopyArea.  The distinction is important -- the icon data are
     *  treated differently by XCopyPlane and XCopyArea.
     */

    if (icon == dos->drag.cursorBlend.mixedIcon) {
	XCopyPlane(display,
		   icon->drag.pixmap, window, draw_gc,
		   0, 0,
		   dos->core.width, dos->core.height,
		   x, y, 1L);
    }
    else if (icon->drag.depth == dos->core.depth) {
	XCopyArea(display,
		  icon->drag.pixmap, window, draw_gc,
		  0, 0,
		  dos->core.width, dos->core.height,
		  x, y);
    }
    else {
	XmeWarning ((Widget) icon, MESSAGE1); /* cast ok here */
    }

    if (clipped) {
        XSetClipMask (display, draw_gc, None);
    }
}

/************************************************************************
 *
 *  Redisplay ()
 *
 *  Called in XmWINDOW mode only.
 ***********************************************************************/

/*ARGSUSED*/
static void
Redisplay(
    Widget wid,
    XEvent *event,		/* unused */
    Region region)		/* unused */
{
    XmDragOverShellWidget	dos = (XmDragOverShellWidget)wid;

    DrawIcon (dos,
	      (dos->drag.rootBlend.mixedIcon ?
	       dos->drag.rootBlend.mixedIcon :
               dos->drag.cursorBlend.mixedIcon),
	      XtWindow(dos), 0, 0);
}

/************************************************************************
 *
 *  _XmDragOverHide ()
 *
 ***********************************************************************/

void
_XmDragOverHide(
    Widget	w,
#if NeedWidePrototypes
    int		clipOriginX,
    int		clipOriginY,
#else
    Position	clipOriginX,
    Position	clipOriginY,
#endif /* NeedWidePrototypes */
    XmRegion	clipRegion )
{
    XmDragOverShellWidget	dos = (XmDragOverShellWidget) w;
    XmDragContext	dc = (XmDragContext)XtParent(dos);
    Boolean		clipped = False;

    if (dos->drag.isVisible &&
	dc->drag.blendModel != XmBLEND_NONE &&
	dos->drag.activeMode != XmCURSOR) {

	if (dos->drag.activeMode == XmWINDOW ||
	    dos->drag.activeMode == XmDRAG_WINDOW) {
	    XtPopdown(w);
	    if (dos->drag.installColormap)
	      UninstallColormap(dos);
	}

	if (dos->drag.activeMode != XmWINDOW) {
	  if (clipRegion != None) {
	    clipped = True;
	    _XmRegionSetGCRegion (XtDisplay(w),
                                  dos->drag.rootBlend.gc,
				  clipOriginX, clipOriginY, clipRegion);
	  }
	  else {
	    XSetClipMask (XtDisplay(w),
                          dos->drag.rootBlend.gc, None);
	  }

	  if (BackingPixmap(dos) != XmUNSPECIFIED_PIXMAP) {
	    XCopyArea (XtDisplay(w),
	               BackingPixmap(dos),
	               RootWindowOfScreen(XtScreen(w)),
		       dos->drag.rootBlend.gc,
	               0, 0, dos->core.width, dos->core.height,
	               BackingX(dos), BackingY(dos));
	  }

	  if (clipped) {
            XSetClipMask (XtDisplay(w),
		          dos->drag.rootBlend.gc, None);
	  }
	}

	dos->drag.isVisible = False;
    }
}

/************************************************************************
 *
 *  _XmDragOverShow ()
 *
 ***********************************************************************/

void
_XmDragOverShow(
    Widget w,
#if NeedWidePrototypes
    int clipOriginX,
    int clipOriginY,
#else
    Position clipOriginX,
    Position clipOriginY,
#endif /* NeedWidePrototypes */
    XmRegion			clipRegion )
{
    XmDragOverShellWidget	dos = (XmDragOverShellWidget) w;
    Display		*display = XtDisplay(w);
    XmDragContext	dc = (XmDragContext)XtParent(dos);
    Boolean		clipped = False;

    if (!dos->drag.isVisible &&
	dc->drag.blendModel != XmBLEND_NONE &&
	dos->drag.activeMode != XmCURSOR) {

	if (dos->drag.activeMode != XmWINDOW && clipRegion != None) {
	    clipped = True;
	    _XmRegionSetGCRegion (display, dos->drag.rootBlend.gc,
	                          clipOriginX - BackingX(dos),
	                          clipOriginY - BackingY(dos),
				  clipRegion);
	}
	else {
	    XSetClipMask (display, dos->drag.rootBlend.gc, None);
	}

	if (dos->drag.activeMode == XmPIXMAP) {
	  XCopyArea (display, RootWindowOfScreen(XtScreen(w)),
		     BackingPixmap(dos),
		     dos->drag.rootBlend.gc,
		     BackingX(dos), BackingY(dos),
		     dos->core.width, dos->core.height,
		     0, 0);
	}

        if (clipped) {
            XSetClipMask (display, dos->drag.rootBlend.gc, None);
        }

	if (dos->drag.activeMode == XmPIXMAP) {
	  DrawIcon (dos,
		    (dos->drag.rootBlend.mixedIcon ?
		     dos->drag.rootBlend.mixedIcon :
		     dos->drag.cursorBlend.mixedIcon),
		    RootWindowOfScreen(XtScreen(w)),
		    dos->core.x, dos->core.y);
	} else {
	  XtPopup(w, XtGrabNone);
	  /*
	   * don't call thru class record since VendorS bug may be
	   * causing override
	   */
	  if (dos->drag.activeMode == XmDRAG_WINDOW) {
	    Arg args[1];
	    if (!dos->drag.holePunched)
	      DragOverShellPunchHole(w);
	    if (dos->drag.installColormap)
	      InstallColormap(dos);
	    XtSetArg(args[0], XmNbackgroundPixmap,
		     dos->drag.rootBlend.mixedIcon->drag.pixmap);
	    XtSetValues((Widget) dos, args, 1);
	  } else {
	    Redisplay(w, NULL, NULL);
	  }
	}
	dos->drag.isVisible = True;
    }
}

/************************************************************************
 *
 *  Destroy ()
 *
 *  Destroy method for the XmDragOverShellClass.
 *  Hide the XmDragOverShell before destroying it.
 ***********************************************************************/

static void
Destroy(
    Widget	w)
{
    XmDragOverShellWidget	dos = (XmDragOverShellWidget)w;
    Display			*display = XtDisplay((Widget)dos);
    XmScreen			xmScreen =
				    (XmScreen) XmGetXmScreen(XtScreen(dos));

    _XmDragOverHide (w, 0, 0, None);

    if (dos->drag.rootBlend.mixedIcon) {
	DestroyMixedIcon (dos, dos->drag.rootBlend.mixedIcon);
    }
    if (dos->drag.rootBlend.gc) {
        XtReleaseGC((Widget) dos, dos->drag.rootBlend.gc);
    }

    if (dos->drag.cursorBlend.mixedIcon) {
	DestroyMixedIcon (dos, dos->drag.cursorBlend.mixedIcon);
    }
    if (dos->drag.cursorBlend.gc) {
        XtReleaseGC ((Widget) dos, dos->drag.cursorBlend.gc);
    }
    if (BackingPixmap(dos) != XmUNSPECIFIED_PIXMAP) {
	_XmFreeScratchPixmap (xmScreen, BackingPixmap(dos));
    }
    if (dos->drag.tmpPix != XmUNSPECIFIED_PIXMAP) {
	_XmFreeScratchPixmap (xmScreen, dos->drag.tmpPix);
    }
    if (dos->drag.tmpBit != XmUNSPECIFIED_PIXMAP) {
	_XmFreeScratchPixmap (xmScreen, dos->drag.tmpBit);
    }

    if (dos->drag.ncCursor != None) {
	XFreeCursor (display, dos->drag.ncCursor);
    }
}

/************************************************************************
 *
 *  ChangeActiveMode ()
 *
 ***********************************************************************/

static void
ChangeActiveMode(
    XmDragOverShellWidget	dos,
#if NeedWidePrototypes
    unsigned int		newActiveMode)
#else
    unsigned char		newActiveMode)
#endif /* NeedWidePrototypes */
{
  Display		*display = XtDisplay((Widget)dos);
  XmDragContext	dc = (XmDragContext)XtParent(dos);
  GC			draw_gc = dos->drag.rootBlend.gc;

  /*
   *  Remove the effects of the current active mode.
   */

  if (dos->drag.activeMode == XmCURSOR) {
    if (newActiveMode != XmCURSOR) {
      dos->drag.activeCursor = XmeGetNullCursor((Widget)dos);
      XChangeActivePointerGrab (display,
				(unsigned int) _XmDRAG_EVENT_MASK(dc),
				dos->drag.activeCursor,
				dc->drag.lastChangeTime);
    }
  }
  else {
    /*
     *  BackingPixmap was created and core.width/height were set
     *  the first time we entered XmPIXMAP or XmWINDOW active mode.
     */

    if (dos->drag.activeMode == XmWINDOW ||
	dos->drag.activeMode == XmDRAG_WINDOW) {
      XtPopdown((Widget)dos);
    }
    XSetClipMask (display, draw_gc, None);
    if (BackingPixmap(dos) != XmUNSPECIFIED_PIXMAP) {
      XCopyArea (display, BackingPixmap(dos),
		 RootWindowOfScreen(XtScreen(dos)), draw_gc,
		 0, 0,
		 dos->core.width, dos->core.height,
		 BackingX(dos), BackingY(dos));
    }
  }
  dos->drag.isVisible = False;

  /*
   *  Add the effects of the new active mode.
   */
  if ((dos->drag.activeMode = newActiveMode) == XmCURSOR) {
    _XmDragOverChange ((Widget)dos, dos->drag.cursorState);
  } else {
    XmScreen		xmScreen = (XmScreen)
      XmGetXmScreen(XtScreen(dos));
    XmDragIconObject	sourceIcon;
    XmDragOverBlend		blend;

    /*
     *  (Re)generate a mixedIcon, in case we have a state
     *  change, or we have used the cursor cache up to this
     *  point.  Place the new mixedIcon so as to preserve the
     *  hotspot location.
     */

    if (dos->drag.rootBlend.sourceIcon) {
      sourceIcon = dos->drag.rootBlend.sourceIcon;
      blend = &dos->drag.rootBlend;
    } else {
      sourceIcon = dos->drag.cursorBlend.sourceIcon;
      blend = &dos->drag.cursorBlend;
    }
    MixIcons (dos, sourceIcon, dos->drag.stateIcon, dos->drag.opIcon,
	      blend, False);

    /*
     *  Compute the new location and handle an icon size change.
     */

    BackingX(dos) = dos->core.x =
      dos->drag.hotX - blend->mixedIcon->drag.hot_x;
    BackingY(dos) = dos->core.y =
      dos->drag.hotY - blend->mixedIcon->drag.hot_y;

    if (dos->core.width != blend->mixedIcon->drag.width ||
	dos->core.height != blend->mixedIcon->drag.height) {
      dos->core.width = blend->mixedIcon->drag.width;
      dos->core.height = blend->mixedIcon->drag.height;
      if (BackingPixmap(dos) != XmUNSPECIFIED_PIXMAP) {
	_XmFreeScratchPixmap (xmScreen, BackingPixmap(dos));
	BackingPixmap(dos) = XmUNSPECIFIED_PIXMAP;
      }
      if (dos->drag.tmpPix != XmUNSPECIFIED_PIXMAP) {
	_XmFreeScratchPixmap (xmScreen, dos->drag.tmpPix);
	dos->drag.tmpPix = XmUNSPECIFIED_PIXMAP;
      }
      if (dos->drag.tmpBit != XmUNSPECIFIED_PIXMAP) {
	_XmFreeScratchPixmap (xmScreen, dos->drag.tmpBit);
	dos->drag.tmpBit = XmUNSPECIFIED_PIXMAP;
      }
    }

    if (dos->drag.activeMode == XmDRAG_WINDOW &&
	BackingPixmap(dos) != XmUNSPECIFIED_PIXMAP) {
      _XmFreeScratchPixmap (xmScreen, BackingPixmap(dos));
      BackingPixmap(dos) = XmUNSPECIFIED_PIXMAP;
    }


    /*
     *  Save the obscured root in backing.
     */

    if (dos->drag.activeMode == XmPIXMAP) {
      if (BackingPixmap(dos) == XmUNSPECIFIED_PIXMAP) {
	BackingPixmap(dos) =
	  _XmAllocScratchPixmap (xmScreen, dos->core.depth,
				 dos->core.width, dos->core.height);
      }

      XSetClipMask (display, draw_gc, None);
      XCopyArea (display, RootWindowOfScreen(XtScreen(dos)),
		 BackingPixmap(dos), draw_gc,
		 BackingX(dos), BackingY(dos),
		 dos->core.width, dos->core.height, 0, 0);
      DrawIcon (dos, blend->mixedIcon,
		RootWindowOfScreen(XtScreen(dos)),
		dos->core.x, dos->core.y);
    } else if (dos->drag.activeMode == XmWINDOW) {
      XSetWindowAttributes  xswa;
      XtPopup((Widget)dos, XtGrabNone);
      xswa.cursor = XmeGetNullCursor((Widget)dos);
      xswa.do_not_propagate_mask = _XmDRAG_EVENT_MASK(dc);
      XChangeWindowAttributes (display, XtWindow(dos),
			       CWCursor|CWDontPropagate, &xswa);
      /*
       * don't call thru class record since VendorS bug may be
       * causing override
       */
      Redisplay((Widget)dos, NULL, NULL);
    } else { /* XmDRAG_WINDOW */
      /* Reposition before popping up */
      if (XtWindow(dos) != None)
	XMoveWindow(XtDisplay(dos), XtWindow(dos),
		    dos->core.x, dos->core.y);
      XtPopup((Widget)dos, XtGrabNone);
      if (dos->drag.activeMode == XmDRAG_WINDOW) {
	Arg args[1];
	if (!dos->drag.holePunched)
	  DragOverShellPunchHole((Widget) dos);
	if (dos->drag.installColormap)
	  InstallColormap(dos);
	XtSetArg(args[0], XmNbackgroundPixmap,
		 dos->drag.rootBlend.mixedIcon->drag.pixmap);
	XtSetValues((Widget) dos, args, 1);
      } else {
	Redisplay((Widget) dos, NULL, NULL);
      }
    }
    dos->drag.isVisible = True;
  }
}

/************************************************************************
 *
 *  ChangeDragWindow ()
 *
 ***********************************************************************/

static void
ChangeDragWindow(XmDragOverShellWidget	dos)
{
  Display		*display = XtDisplay((Widget)dos);
  Window		win = XtWindow((Widget)dos);
  GC			draw_gc = dos->drag.rootBlend.gc;
  XmScreen		xmScreen = (XmScreen) XmGetXmScreen(XtScreen(dos));
  XmDragIconObject	sourceIcon;
  XmDragOverBlend	blend;
  XmDragIconObject    mixedIcon;
  XmDragOverBlendRec	*cursorBlend = &dos->drag.cursorBlend;
  XGCValues           v;
  unsigned long       vmask;

    /*
     *  Blend a new mixedIcon using only the source icon.
     *  Place the new mixedIcon to preserve the source icon location.
     *  The current mixedIcon data is valid.
     */

  if (dos->drag.rootBlend.sourceIcon) {
    sourceIcon = dos->drag.rootBlend.sourceIcon;
    blend = &dos->drag.rootBlend;
  } else {
    sourceIcon = dos->drag.cursorBlend.sourceIcon;
    blend = &dos->drag.cursorBlend;
  }
  mixedIcon = blend->mixedIcon;

  XSetFunction (display, blend->gc, GXset);
  XFillRectangle (display, mixedIcon->drag.pixmap, blend->gc,
		  0, 0, mixedIcon->drag.width, mixedIcon->drag.height);

  if (mixedIcon->drag.mask != XmUNSPECIFIED_PIXMAP) {
    if (cursorBlend->gc == NULL) {
      v.background = 0;
      v.foreground = 1;
      v.function = GXclear;
      v.graphics_exposures = False;
      v.subwindow_mode = IncludeInferiors;
      v.clip_mask = None;
      v.clip_x_origin = 0;	/* pedantic */
      v.clip_y_origin = 0;	/* pedantic */
      vmask = GCBackground|GCForeground|GCFunction|
	GCClipXOrigin|GCClipYOrigin|GCClipMask|
	  GCGraphicsExposures|GCSubwindowMode;
      cursorBlend->gc = XtAllocateGC((Widget) dos, 1,
				     vmask, &v, vmask, 0L);
    } else {
      v.clip_mask = None;
      v.function = GXclear;
      vmask = GCClipMask|GCFunction;
      XChangeGC (display, cursorBlend->gc, vmask, &v);
    }
    XFillRectangle (display, mixedIcon->drag.mask, cursorBlend->gc,
		    0, 0, mixedIcon->drag.width, mixedIcon->drag.height);
  }

  BlendIcon (dos, sourceIcon, mixedIcon, blend->sourceX,
	     blend->sourceY, cursorBlend->gc, blend->gc);

  /*
   *  Remove the current drag window.
   */
  XUnmapWindow(display, win);

  /*
   *  Handle an icon size change.
   */

  if (dos->core.width != blend->mixedIcon->drag.width ||
      dos->core.height != blend->mixedIcon->drag.height) {
    if (BackingPixmap(dos) != XmUNSPECIFIED_PIXMAP) {
      _XmFreeScratchPixmap (xmScreen, BackingPixmap(dos));
      BackingPixmap(dos) = XmUNSPECIFIED_PIXMAP;
    }
    if (dos->drag.tmpPix != XmUNSPECIFIED_PIXMAP) {
      _XmFreeScratchPixmap (xmScreen, dos->drag.tmpPix);
      dos->drag.tmpPix = XmUNSPECIFIED_PIXMAP;
    }
    if (dos->drag.tmpBit != XmUNSPECIFIED_PIXMAP) {
      _XmFreeScratchPixmap (xmScreen, dos->drag.tmpBit);
      dos->drag.tmpBit = XmUNSPECIFIED_PIXMAP;
    }
  }

  /*
   *  Save the obscured root in backing.
   */

  if (BackingPixmap(dos) == XmUNSPECIFIED_PIXMAP) {
    BackingPixmap(dos) =
      _XmAllocScratchPixmap (xmScreen, dos->core.depth,
			     dos->core.width, dos->core.height);
  }
  BackingX(dos) = dos->core.x;
  BackingY(dos) = dos->core.y;

  XSetClipMask (display, draw_gc, None);
  XCopyArea (display, RootWindowOfScreen(XtScreen(dos)),
	     BackingPixmap(dos), draw_gc,
	     BackingX(dos), BackingY(dos),
	     dos->core.width, dos->core.height, 0, 0);

  /*
   *  Move, resize, and remap the drag window.
   *  This is an override_redirect window.
   */
  XMoveResizeWindow(display, win, dos->core.x, dos->core.y,
		    dos->core.width, dos->core.height);
  XMapWindow(display, win);

  /*
   * don't call thru class record since VendorS bug may be
   * causing override
   */
  Redisplay((Widget)dos, NULL, NULL);
}

/************************************************************************
 *
 *  _XmDragOverMove ()
 *
 *  This method is less efficient than the obvious method of copying the
 *  backing to the root, saving the new icon destination in the backing,
 *  and copying the icon to the root.  However, it results in a smoother
 *  appearance.
 ***********************************************************************/

void
_XmDragOverMove(
    Widget	w,
#if NeedWidePrototypes
    int		x,
    int		y)
#else
    Position	x,
    Position	y)
#endif /* NeedWidePrototypes */
{
    XmDragOverShellWidget	dos = (XmDragOverShellWidget) w;
    XmDragContext	dc = (XmDragContext)XtParent(dos);
    Display		*display = XtDisplay(w);
    XmScreen		xmScreen = (XmScreen) XmGetXmScreen(XtScreen(w));
    Window		root = RootWindowOfScreen(XtScreen(w));
    Pixmap		old_backing = BackingPixmap(dos);
    Pixmap		new_backing;
    GC			draw_gc = dos->drag.rootBlend.gc;
    XmDragIconObject	mixedIcon;
    XGCValues           v;
    unsigned long       vmask;

    dos->drag.hotX = x;
    dos->drag.hotY = y;

    if (!dos->drag.isVisible ||
	dc->drag.blendModel == XmBLEND_NONE ||
	dos->drag.activeMode == XmCURSOR) {
        return;
    }

    if (dos->drag.rootBlend.mixedIcon) {
        mixedIcon = dos->drag.rootBlend.mixedIcon;
    } else {	/* exists */
        mixedIcon = dos->drag.cursorBlend.mixedIcon;
    }

    dos -> core.x = x -= mixedIcon->drag.hot_x;
    dos -> core.y = y -= mixedIcon->drag.hot_y;

    if (dos->drag.activeMode == XmWINDOW ||
	dos->drag.activeMode == XmDRAG_WINDOW) {
      /* this is an override_redirect window */
      XMoveWindow(display, XtWindow (w), x, y);
      return;
    }

    /*
     *  From here on, the active mode is XmPIXMAP.
     */

    if (dos->drag.tmpPix == XmUNSPECIFIED_PIXMAP) {
        dos->drag.tmpPix =
	    _XmAllocScratchPixmap (xmScreen, dos->core.depth,
	                           dos->core.width, dos->core.height);
    }
    new_backing = dos->drag.tmpPix;

    /*
     *  Save the area where the new icon is to go.
     */

    v.clip_mask = None;
    v.function = GXcopy;
    vmask = GCClipMask|GCFunction;
    XChangeGC (display, draw_gc, vmask, &v);
    XCopyArea (display, root, new_backing, draw_gc,
	       x, y, dos->core.width, dos->core.height, 0, 0);

    if (x + ((Position) dos->core.width) > BackingX(dos) &&
	x < BackingX(dos) + ((Position) dos->core.width) &&
	y + ((Position) dos->core.height) > BackingY(dos) &&
	y < BackingY(dos) + ((Position) dos->core.height)) {

	XRectangle 	rect, rect1, rect2;
	XPoint 		pt;

	/*
	 *  Have overlap:
	 *
	 *  Calculate the intersection between the 2 areas and
	 *  copy the non-overlapping old area in the window.
	 *
         *  If the icon has a mask, create a mask through which we will
	 *  copy the old backing to the root.
	 *  Otherwise, use one or two rectangles.
	 */

        if (mixedIcon->drag.region == NULL &&
	    mixedIcon->drag.mask != XmUNSPECIFIED_PIXMAP) {

	    Pixmap	root_mask;
	    GC		mask_gc = dos->drag.cursorBlend.gc;

	    if (dos->drag.tmpBit == XmUNSPECIFIED_PIXMAP) {
		dos->drag.tmpBit =
		    _XmAllocScratchPixmap (xmScreen, 1, dos->core.width,
					   dos->core.height);
	    }
	    root_mask = dos->drag.tmpBit;

    	    v.clip_mask = None;
    	    v.function = GXset;
    	    vmask = GCClipMask|GCFunction;
    	    XChangeGC (display, mask_gc, vmask, &v);
	    XFillRectangle (display, root_mask, mask_gc,
		            0, 0, dos->core.width, dos->core.height);

	    XSetFunction (display, mask_gc, GXandInverted);
            XCopyArea (display, mixedIcon->drag.mask, root_mask, mask_gc,
	               0, 0, mixedIcon->drag.width, mixedIcon->drag.height,
	               x - BackingX(dos), y - BackingY(dos));

	    /*
	     *  Copy the icon into the new area and refresh the root.
	     */

	    DrawIcon (dos, mixedIcon, root, x, y);

    	    v.clip_mask = root_mask;
    	    v.clip_x_origin = BackingX(dos);
    	    v.clip_y_origin = BackingY(dos);
    	    vmask = GCClipMask|GCClipXOrigin|GCClipYOrigin;
    	    XChangeGC (display, draw_gc, vmask, &v);
            XCopyArea (display, old_backing, root, draw_gc,
	               0, 0, dos->core.width, dos->core.height,
	               BackingX(dos), BackingY(dos));
	    XSetClipMask (display, draw_gc, None);
	}
	else {

	    /*
	     *  Copy the icon into the new area.
	     */

	    DrawIcon (dos, mixedIcon, root, x, y);

	    /*
	     *  Use rectangles to refresh exposed root.
	     *  The first rectangle (horizontal movement).
	     */

	    if (x > BackingX(dos)) {
	        rect1.x = 0;
	        rect1.width = x - BackingX(dos);
	    }
	    else {
	        rect1.width = BackingX(dos) - x;
	        rect1.x = dos->core.width - rect1.width;
	    }

	    rect1.y = 0;
	    rect1.height = dos->core.height;
	    pt.x = BackingX(dos) + rect1.x;
	    pt.y = BackingY(dos);

	    if (rect1.width > 0) {
                XCopyArea (display, old_backing, root, draw_gc,
	                   rect1.x, rect1.y, rect1.width, rect1.height,
		           pt.x, pt.y);
	    }

	    /*
	     *  The second rectangle (vertical movement).
	     */

	    if (y > BackingY(dos)) {
	        rect2.y = 0;
	        rect2.height = y - BackingY(dos);
	    }
	    else {
	        rect2.height = BackingY(dos) - y;
	        rect2.y = dos->core.height - rect2.height;
	    }

	    rect2.x = 0;
	    rect2.width = dos->core.width;
	    pt.x = BackingX(dos);
	    pt.y = BackingY(dos) + rect2.y;

	    if (rect2.height > 0) {
                XCopyArea (display, old_backing, root, draw_gc,
	                   rect2.x, rect2.y, rect2.width, rect2.height,
		           pt.x, pt.y);
	    }
	}

	/*
	 *  Copy the overlapping area between old_backing and
	 *  new_backing into new_backing to be used for the
	 *  next cursor move.
	 */

	if (x > BackingX(dos)) {
	    rect.x = x - BackingX(dos);
	    pt.x = 0;
	    rect.width = dos->core.width - rect.x;
	}
	else {
	    rect.x = 0;
	    pt.x = BackingX(dos) - x;
	    rect.width = dos->core.width - pt.x;
	}

	if (y > BackingY(dos)) {
	    rect.y = y - BackingY(dos);
	    pt.y = 0;
	    rect.height = dos->core.height - rect.y;
	}
	else {
	    rect.y = 0;
	    pt.y = BackingY(dos) - y;
	    rect.height = dos->core.height - pt.y;
	}

	XCopyArea (display, old_backing, new_backing, draw_gc,
		   rect.x, rect.y, rect.width, rect.height, pt.x, pt.y);

        if (mixedIcon->drag.restore_region) {
            XSetRegion(display, draw_gc, mixedIcon->drag.restore_region);
            XSetClipOrigin(display, draw_gc, x, y);
            XCopyArea (display, new_backing, root,
                       draw_gc, 0, 0, dos->core.width,
                       dos->core.height, x, y);
            XSetClipMask(display, draw_gc, None);
        }
    }
    else {

	/*
	 *  No overlap:  refresh the root from old_backing.
	 *  new_backing is valid.
	 */

        XCopyArea (display, old_backing, root, draw_gc,
	           0, 0, dos->core.width, dos->core.height,
	           BackingX(dos), BackingY(dos));

	DrawIcon (dos, mixedIcon, root, x, y);
    }

    /*  Update the variables needed for the next loop  */

    BackingX(dos) = x;
    BackingY(dos) = y;
    BackingPixmap(dos) = new_backing;
    dos->drag.tmpPix = old_backing;
}

/************************************************************************
 *
 *  _XmDragOverChange ()
 *
 *  Make dragover changes to track changes in component icons or colors.
 ***********************************************************************/

void
_XmDragOverChange(
    Widget		w,
#if NeedWidePrototypes
    unsigned int	dropSiteStatus)
#else
    unsigned char	dropSiteStatus)
#endif /* NeedWidePrototypes */
{
    XmDragOverShellWidget	dos = (XmDragOverShellWidget) w;
    XmDragContext	dc = (XmDragContext)XtParent(dos);
    XmDragIconObject	sourceIcon = NULL;
    XmDragIconObject	opIcon = NULL;
    XmDragIconObject	stateIcon = NULL;
    Boolean		doChange, dirty = False;
    Boolean		usedSrcPixIcon = True;

    dos->drag.cursorState = dropSiteStatus;

    if (dos->drag.mode == XmWINDOW) {
      return;
    }

    if (dc->drag.blendModel == XmBLEND_NONE) {
	return;
    }

    /*
     *  Get the sourceIcon.
     *  If we are in XmPIXMAP mode use the XmNsourcePixmapIcon if:
     *    1. it exists,
     *    2. it has the same screen as dos, and
     *    3. it has depth 1 or the same depth as dos.
     *  Otherwise, use the XmNsourceCursorIcon if:
     *    1. it exists,
     *    2. it has the same screen as dos, and
     *    3. is a bitmap.
     *  Otherwise, use the XmNdefaultSourceCursorIcon.
     */

    if (dos->drag.mode == XmPIXMAP ||
	dos->drag.mode == XmDRAG_WINDOW ||
	dos->drag.mode == XmWINDOW) {
      sourceIcon = dc->drag.sourcePixmapIcon;
    }

    if (sourceIcon == NULL ||
        XtScreenOfObject(XtParent(sourceIcon)) != XtScreen(w) ||
	((sourceIcon->drag.depth != dos->core.depth) &&
	 (sourceIcon->drag.depth != 1))) {

	usedSrcPixIcon = False;
        sourceIcon = dc->drag.sourceCursorIcon;

	if (sourceIcon == NULL ||
            XtScreenOfObject(XtParent(sourceIcon)) != XtScreen(w) ||
	    sourceIcon->drag.depth != 1) {
	    sourceIcon = _XmScreenGetSourceIcon (w); /* nonNULL */
	}
    }

    /*
     *  Get the state and operation icons, according to the blending model.
     */

    switch ((int) dc->drag.blendModel) {

	default:
	    XmeWarning( (Widget) dc, MESSAGE4);
	case XmBLEND_ALL:
	    /*
	     *  Get the operation icon bitmap.
	     */

            opIcon = dc->drag.operationCursorIcon;

	    if (opIcon == NULL || opIcon->drag.depth != 1 ||
			XtScreenOfObject(XtParent(opIcon)) != XtScreen(w)) {
	    	opIcon = _XmScreenGetOperationIcon (w,
					            dc->drag.operation);
	        if (opIcon && opIcon->drag.depth != 1) {
	            opIcon = NULL;
	        }
	    }

	    /* fall through */

	case XmBLEND_STATE_SOURCE:
	    /*
	     *  Get the state icon bitmap.
	     */

            stateIcon = dc->drag.stateCursorIcon;

	    if (stateIcon == NULL || stateIcon->drag.depth != 1 ||
			XtScreenOfObject(XtParent(stateIcon)) != XtScreen(w)) {
	    	stateIcon = _XmScreenGetStateIcon (w,
						   dropSiteStatus);
	        if (stateIcon && stateIcon->drag.depth != 1) {
	            stateIcon = NULL;
	        }
	    }
	    break;

	case XmBLEND_JUST_SOURCE:
	    break;
    }

    /*
     *  Determine the cursor colors and create or recolor the root's gc.
     *  Record that a change is necessary if the cursor colors or any
     *  of the component icons have changed.
     */

    dirty = (_XmDragIconIsDirty (sourceIcon) ||
             (opIcon && _XmDragIconIsDirty (opIcon)) ||
	     (stateIcon && _XmDragIconIsDirty (stateIcon)));

    doChange = GetDragIconColors (dos) ||
	       dos->drag.opIcon != opIcon ||
               dos->drag.stateIcon != stateIcon ||
               dos->drag.rootBlend.sourceIcon != sourceIcon ||
	       dirty;

    /*
     *  If we are not using the XmNsourcePixmapIcon, then try to create
     *  a cursor from the specified icons.  If we are successful, we will
     *  use the cursor in both cursor and pixmap mode.
     *  Remember:  XmNsourcePixmapIcon is only used in XmPIXMAP mode.
     */

    dos->drag.opIcon = opIcon;
    dos->drag.stateIcon = stateIcon;
    dos->drag.cursorBlend.sourceIcon = sourceIcon;

    if (!usedSrcPixIcon &&
        (dos->drag.activeCursor =
	     GetDragIconCursor (dos, sourceIcon, stateIcon, opIcon,
				False /* no clip */, dirty))
	     != None) {
	/*
	 *  We have created a new cursor:  clean the icons.
	 */

	_XmDragIconClean (sourceIcon, stateIcon, opIcon);

	if (dos->drag.activeMode != XmCURSOR) {
	    _XmDragOverHide (w, 0, 0, None);
	    dos->drag.activeMode = XmCURSOR;
	}
	XChangeActivePointerGrab (XtDisplay(w),
				  (unsigned int) _XmDRAG_EVENT_MASK(dc),
				  dos->drag.activeCursor,
				  dc->drag.lastChangeTime);

	/*
	 *  We will use XmCURSOR active mode:  destroy any previously
	 *  used rootBlend icon.
	 */

	dos->drag.rootBlend.sourceIcon = NULL;
	if (dos->drag.rootBlend.mixedIcon) {
	    DestroyMixedIcon (dos, dos->drag.rootBlend.mixedIcon);
	    dos->drag.rootBlend.mixedIcon = NULL;
	}
	return;
    }

    /*
     *  Am using XmNsourcePixmapIcon or the cursor was too big.
     *  Save the sourceIcon for non-XmCURSOR active mode.
     */

    dos->drag.rootBlend.sourceIcon = sourceIcon;

    if (dos->drag.mode == XmCURSOR) {

    	/*
	 *  XmCURSOR mode:  the cursor was too large for the hardware; clip
	 *  it to the maximum hardware cursor size.
	 */

	dos->drag.activeCursor =
	    GetDragIconCursor (dos, sourceIcon, stateIcon, opIcon,
				True /* clip */, dirty);

	_XmDragIconClean (sourceIcon, stateIcon, opIcon);
	if (dos->drag.activeMode != XmCURSOR) {
	    _XmDragOverHide (w, 0, 0, None);
	    dos->drag.activeMode = XmCURSOR;
	}
	XChangeActivePointerGrab (XtDisplay(w),
				  (unsigned int) _XmDRAG_EVENT_MASK(dc),
				  dos->drag.activeCursor,
				  dc->drag.lastChangeTime);
    }

    /*
     *  Else unable to use XmCURSOR activeMode in XmPIXMAP mode.
     *  Change activeMode to XmPIXMAP or XmDRAG_WINDOW.
     */

    else if (dos->drag.mode == XmPIXMAP) {
      if (doChange || dos->drag.activeMode != XmPIXMAP) {
	_XmDragIconClean (sourceIcon, stateIcon, opIcon);
	ChangeActiveMode (dos, XmPIXMAP);
      }
    } else /* DRAG_WINDOW */
      if (doChange || dos->drag.activeMode != XmDRAG_WINDOW) {
      _XmDragIconClean (sourceIcon, stateIcon, opIcon);
      ChangeActiveMode (dos, XmDRAG_WINDOW);
    }
}

/************************************************************************
 *
 *  _XmDragOverFinish ()
 *
 ***********************************************************************/

void
_XmDragOverFinish(
    Widget		w,
#if NeedWidePrototypes
    unsigned int	completionStatus)
#else
    unsigned char	completionStatus)
#endif /* NeedWidePrototypes */
{
    XmDragOverShellWidget	dos = (XmDragOverShellWidget) w;
    XmDragContext	dc = (XmDragContext)XtParent(dos);
/*
    GC			draw_gc = dos->drag.rootBlend.gc;
*/

    if (dc->drag.blendModel != XmBLEND_NONE) {

/* If there could be some way to only do this code when we know that
 * there is animation being done under the drag over effects.  Maybe
 * add a XmDROP_ANIMATE completionStatus type?  XmDROP_ANIMATE would
 * be the same as XmDROP_SUCCESS, but it would also indicate that
 * animation is being done.  This code causes unecessary flashing
 * when animation is not being done.  It also fixes a bug the make
 * the melt effects look correct when the area under the icons has
 * changed.

	XFlush (XtDisplay((Widget)dos));
        XSetClipMask (XtDisplay((Widget)dos), draw_gc, None);
	XtPopdown(w);
        XCopyArea (XtDisplay((Widget)dos), RootWindowOfScreen(XtScreen(dos)),
	           BackingPixmap(dos), draw_gc,
	           BackingX(dos), BackingY(dos),
	           dos->core.width, dos->core.height, 0, 0);
    	XtPopup(w, XtGrabNone);
*/
	XGrabServer(XtDisplay(w));

	/*
	 *  Create and draw a source-only mixedIcon.
	 *  Do not recolor it, even though the state is no longer used.
	 *  Place the source-only mixedIcon so the source doesn't move.
	 *
	 *  The current active mode is XmWINDOW, so the blend data is valid.
	 *  However, the backing may not be, since the server was ungrabbed.
	 *  We keep the active mode as XmWINDOW and delay XtPopDown until
	 *  the finish effects are finished to force the server to generate
	 *  an expose event at the initial mixedIcon location.
	 */

	ChangeDragWindow (dos);

	if (completionStatus == XmDROP_FAILURE) {
	    /* generate zap back effects */
	    DoZapEffect((XtPointer)dos, (XtIntervalId *)NULL);
	}
	else {
	    /* generate melt effects */
	    DoMeltEffect((XtPointer)dos, (XtIntervalId *)NULL);
	}

	XtPopdown(w);
	dos->drag.isVisible = False;
	XUngrabServer(XtDisplay(w));
    }
}

/************************************************************************
 *
 *  _XmDragOverGetActiveCursor ()
 *
 ***********************************************************************/

Cursor
_XmDragOverGetActiveCursor(
    Widget	w)
{
    XmDragOverShellWidget	dos = (XmDragOverShellWidget) w;
    return dos->drag.activeCursor;
}

/************************************************************************
 *
 *  _XmDragOverSetInitialPosition ()
 *
 ***********************************************************************/

void
_XmDragOverSetInitialPosition(
    Widget	w,
#if NeedWidePrototypes
    int		initialX,
    int		initialY)
#else
    Position	initialX,
    Position	initialY)
#endif /* NeedWidePrototypes */
{
    XmDragOverShellWidget	dos = (XmDragOverShellWidget) w;
    dos->drag.initialX = initialX;
    dos->drag.initialY = initialY;
}

/* search up the tree to find the parent shell for whom colormapWidget
 * is a child widget. This is the shell on which the colormap is to be
 * installed.
 */
static void
FindColormapShell(XmDragOverShellWidget dw)
{
    Widget cw = dw->drag.colormapWidget;
    Arg args[1];

    while (cw && !XtIsShell(cw))
	cw = XtParent(cw);
    dw->drag.colormapShell = cw;

    /* find out if this shell is override redirect */
    XtSetArg(args[0], XmNoverrideRedirect, &dw->drag.colormapOverride);
    XtGetValues(cw, args, 1);
}

/* set the WmColormapWindows property on the parent shell to include dw's
 * colormap
 */
static void
InstallColormap(XmDragOverShellWidget dw)
{
    Status status;
    Window *windowsReturn;
    int countReturn;

    if (!dw->drag.colormapShell)
	FindColormapShell(dw);
    if (dw->drag.colormapShell)
    {
	/* check to see if there is a property */
	status = XGetWMColormapWindows(XtDisplay(dw),
				       XtWindow(dw->drag.colormapShell),
				       &windowsReturn, &countReturn);
	/* if no property, just create one */
	if (!status)
	{
	    Window windows[2];
	    windows[0] = XtWindow(dw);
	    windows[1] = XtWindow(dw->drag.colormapShell);
	    XSetWMColormapWindows(XtDisplay(dw),
				  XtWindow(dw->drag.colormapShell),
				  windows, 2);
	}
	/* there was a property, add myself to the beginning */
	else
	{
	    Window *windows = (Window *)XtMalloc((sizeof(Window))*(countReturn+1));
	    register int i;
	    windows[0] = XtWindow(dw);
	    for (i=0; i<countReturn; i++)
		windows[i+1] = windowsReturn[i];
	    XSetWMColormapWindows(XtDisplay(dw),
				  XtWindow(dw->drag.colormapShell),
				  windows, countReturn+1);
	    XtFree((char*)windows);
	    XtFree((char*)windowsReturn);
	}
	/* The window manager can't install colormaps for override redirect
	 * windows.  So we have to do it ourselves using XInstallColormap.
	 * This can confuse the window manager, so we save the current
	 * colormap and reinstall it when we're done.
	 *
	 * Note this code does not work with twm, who will kick us out
	 * upon seeing that we have installed a colormap.  But there
	 * is nothing we can do about that until the consortium tells
	 * us how to correctly install override redirect colormaps
	 */
	if (dw->drag.colormapOverride)
	{
	    dw->drag.savedColormaps = XListInstalledColormaps(
					  XtDisplay(dw), XtWindow(dw),
					  &dw->drag.numSavedColormaps);
	    XInstallColormap(XtDisplay(dw), dw->core.colormap);
	}
    }
}

/* set the WmColormapWindows property on the parent shell to exclude dw's
 * colormap
 */
static void
UninstallColormap(XmDragOverShellWidget dos)
{
    Status status;
    Window *windowsReturn;
    int countReturn;
    register int i;

    if (!dos->drag.colormapShell)
	FindColormapShell(dos);
    if (dos->drag.colormapShell)
    {
	/* check to see if there is a property */
	status = XGetWMColormapWindows(XtDisplay(dos),
				       XtWindow(dos->drag.colormapShell),
				       &windowsReturn, &countReturn);
	/* if no property, just return.  If there was a property, continue */
	if (status)
	{
	    /* search for a match */
	    for (i=0; i<countReturn; i++)
	    {
		if (windowsReturn[i] == XtWindow(dos))
		{
		    /* we found a match, now copu the rest down */
		    for (i++; i<countReturn; i++)
		    {
			windowsReturn[i-1] = windowsReturn[i];
		    }
		    XSetWMColormapWindows(XtDisplay(dos),
					  XtWindow(dos->drag.colormapShell),
					  windowsReturn, countReturn-1);
		    break;	/* from outer for */
		}
	    }
	    XtFree((char*)windowsReturn);
	}
	/* if this was an override window, we installed our own colormaps.
	 * put them back.  (See comments in installColormap for more details
	 */
	if (dos->drag.colormapOverride)
	{
	    for (i=0; i<dos->drag.numSavedColormaps; i++)
		XInstallColormap(XtDisplay(dos),
				 dos->drag.savedColormaps[i]);
	    XFree((char *)dos->drag.savedColormaps);
	}
    }
}

/* The following function relies on the shape extension. */

static void
DragOverShellPunchHole(Widget w)
{
    static XmConst XRectangle pixelPunch = { 0, 0, 1, 1 };
    XmDragOverShellWidget dos = (XmDragOverShellWidget)w;
    XmDragIconObject icon = (dos->drag.rootBlend.mixedIcon ?
			     dos->drag.rootBlend.mixedIcon :
			     dos->drag.cursorBlend.mixedIcon);

    /* The following code requires an XtWindow, so we force the widget
     * to be realized if it isn't already
     */
    XtRealizeWidget(w);

    /* clear effects of previous shaping */
    XShapeCombineMask (XtDisplay(dos), XtWindow(dos),
		       ShapeBounding,
		       0, 0, None, ShapeSet);

    /* shape the outside of the window */
    if (icon && icon -> drag.mask)
    {
        if (icon -> drag.mask)
            XShapeCombineMask (XtDisplay(dos), XtWindow(dos),
			 ShapeBounding, 0, 0,
			 icon -> drag.mask, ShapeSet);

        /* punch a hole in the window */
        XShapeCombineRectangles (XtDisplay(dos), XtWindow(dos),
			     ShapeBounding,
			     icon->drag.hot_x, icon->drag.hot_y,
			     (XRectangle*)&pixelPunch, 1,
			     ShapeSubtract, YXBanded);

        dos->drag.holePunched = TRUE;
    }
}

static void
DragOverShellColormapWidget(Widget ds, Widget cw)
{
    XmDragOverShellWidget dos = (XmDragOverShellWidget) ds;

    if (dos->drag.colormapWidget != cw)
    {
	dos->drag.colormapWidget = cw;
	dos->drag.colormapShell = NULL;
	FindColormapShell(dos);
    }
}
