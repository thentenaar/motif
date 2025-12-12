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
static char rcsid[] = "$XConsortium: DNDDemo.c /main/8 1996/10/10 16:32:41 drk $"
#endif
#endif
/*
 *    file: DNDDemo.c
 *
 *	A demo program showing the basic Drag And Drop operations.
 */

#include "DNDDemo.h"


/*
 * The folowing character arrays are for use with the drop help
 * dialogs.  For internationalization, message catalogs should
 * replace these static declarations.
 */
char HELP_MSG1[] =
"This drop action will change the color\n\
of the rectangle that the paint palette\n\
icon is dropped on. To accept this drop\n\
press the o.k. button, otherwise press\n\
cancel";

char HELP_MSG2[] =
"This drop action will move the rectangle\n\
to the new position.  To accept this drop\n\
press the o.k. button, otherwise press\n\
cancel";

char HELP_MSG3[] =
"This drop action will copy the rectangle\n\
to the new position.  To accept this drop\n\
press the o.k. button, otherwise press\n\
cancel";

char HELP_MSG4[] =
"This drop action can either copy or\n\
move the rectangle to the new position.\n\
Select the operation that you desire.\n\
In the future, use Ctrl with Btn2 to\n\
perform copy operations.  The default\n\
operation is move.  To accept this drop\n\
press the o.k. button, otherwise press\n\
cancel";

char HELP_MSG5[] =
"This drop action is at an Invalid drop\n\
position.  Please cancel this drop \n\
by pressing the cancel button.";


/* Globals variables */
Widget          topLevel;
Widget          drawingArea;
Widget          helpDialog = NULL;
Widget          helpLabel, helpMenu;
Widget          myDC;
XtAppContext    appContext;
XmDestinationCallbackStruct *savedCB;

/* This function creates the Drag Icon. */
static Widget
GetDragIcon(Widget w, Pixmap icon, Pixmap iconMask, Dimension width,
Dimension height, Pixel background, Pixel foreground)
{

    Widget  dragIcon;
    Arg     args[10];
    int     n = 0;

    XtSetArg(args[n], XmNhotX, ICON_X_HOT); n++;
    XtSetArg(args[n], XmNhotY, ICON_Y_HOT); n++;
    XtSetArg(args[n], XmNwidth, width); n++;
    XtSetArg(args[n], XmNheight, height); n++;
    XtSetArg(args[n], XmNmaxWidth, appInfo->maxCursorWidth); n++;
    XtSetArg(args[n], XmNmaxHeight, appInfo->maxCursorHeight); n++;
    XtSetArg(args[n], XmNbackground, background); n++;
    XtSetArg(args[n], XmNforeground, foreground); n++;
    XtSetArg(args[n], XmNpixmap, icon); n++;
    if (iconMask != XmUNSPECIFIED_PIXMAP &&
	iconMask != None) {
      XtSetArg(args[n], XmNmask, iconMask); n++;
    }
    dragIcon = XmCreateDragIcon(w, "dragIcon", args, n);

    return(dragIcon);

}


/* This function creates the bitmaps for the icon and the mask
 * and then calls GetDragIcon() to  create the drag icon.
 */
static Widget
GetDragIconFromBits(Widget w, char *bits, char *mask, Dimension width,
                    Dimension height, Pixel background, Pixel foreground)
{

    Pixmap     icon, iconMask;
    Display    *display = XtDisplay(w);

    icon = XCreateBitmapFromData(display, DefaultRootWindow(display), bits,
                                 width, height);

    iconMask = XCreateBitmapFromData(display, DefaultRootWindow(display),
                                     mask, width, height);

    return(GetDragIcon(w, icon, iconMask, width, height,
                       background, foreground));

}


/* This function creates the rectangle bitmaps for the icon and
 * the mask based on the maximum server allowable cursor size
 * and then calls GetDragIcon() to create the drag icon.
 */
static Widget
GetDragIconFromRect(Widget w, RectPtr rect, Pixel background)
{

    Pixmap      icon;
    Pixel       foreground = RectGetColor(rect);
    Dimension   width, height;

    /* Create a depth 1 pixmap (bitmap) for use with the drag icon */
    icon = GetBitmapFromRect(w, rect, background, foreground,
                                         &width, &height);

    /* use bitmap for both the bitmap and mask */
    return(GetDragIcon(w, icon, None, width, height,
                       background, foreground));

}


/* This is a selection conversion function that is used in
 * converting drag/drop export background color targets.
 * The return types follow ICCC statndards.
 */
void
ColorConvert(Widget widget, XtPointer ignore, XmConvertCallbackStruct *cs)
{

  Display     *display = XtDisplay(widget);
  Atom        BACKGROUND = XInternAtom(display, "BACKGROUND", False);
  Atom        PIXEL = XInternAtom(display, "PIXEL", False);
  Atom        TARGETS = XInternAtom(display, "TARGETS", False);
  Atom	ME_TARGETS =
    XInternAtom(display, XmS_MOTIF_EXPORT_TARGETS, False);

  if (cs -> target == BACKGROUND) {
    /* Get widget's background */
    Pixel *background;

    background = (Pixel *) XtMalloc(sizeof(Pixel));
    XtVaGetValues(widget, XmNbackground, background, NULL, NULL);

      /* value, type, length, and format must be set */
      cs -> value = (XtPointer) background;
      cs -> type = PIXEL;
      cs -> length = sizeof(Pixel);
      cs -> format = 32;

    } else if (cs -> target == TARGETS || cs -> target == ME_TARGETS) {
      /* This target is required by ICCCM */
      Atom *targs;
      int target_count = 0;

      if (cs -> target == ME_TARGETS)
	targs = (Atom *)XtMalloc((unsigned) (1 * sizeof(Atom)));
      else
	targs = XmeStandardTargets(widget, 1, &target_count);

      targs[target_count++] = BACKGROUND;

      cs -> value = (XtPointer) targs;
      cs -> length = target_count;
      cs -> format = 32;
      cs -> type = XA_ATOM;
      cs -> status = XmCONVERT_DONE;
    } else
      cs -> status = XmCONVERT_REFUSE;
}


/* This callback procedure resets the drag icon cursor to show
 * when the drag is in a valid region .  It cause the
 * state icon to become visible when a drop is at a valid
 * position for drag over effects.
 */
static void
DragMotionCallback(Widget w, XtPointer client, XtPointer call)
{

    XmDragMotionCallback    cb = (XmDragMotionCallback) call;
    Arg                     args[2];
    Widget                  stateIcon, invalidIcon;

    if (cb->dropSiteStatus == XmVALID_DROP_SITE) {

        stateIcon = (Widget) client;

        XtSetArg(args[0], XmNblendModel, XmBLEND_STATE_SOURCE);
        XtSetArg(args[1], XmNstateCursorIcon, stateIcon);
        XtSetValues(w, args, 2);

    }
    else if (cb->dropSiteStatus == XmINVALID_DROP_SITE) {

        XtSetArg(args[0], XmNdefaultInvalidCursorIcon, &invalidIcon);
        XtGetValues(XmGetXmScreen(XtScreen(w)), args, 1);
        XtSetArg(args[0], XmNblendModel, XmBLEND_STATE_SOURCE);
        XtSetArg(args[1], XmNstateCursorIcon, invalidIcon);
        XtSetValues(w, args, 2);

    }
    else {
        XtSetArg(args[0], XmNblendModel, XmBLEND_JUST_SOURCE);
        XtSetValues(w, args, 1);
    }

}


/* This callback procedure resets the drag icon cursor to show
 * when the drag is in a valid region .  It cause the
 * state icon to become visible when a drop is at a valid
 * position for drag over effects.
 */
static void
DropSiteLeaveCallback(Widget w, XtPointer client, XtPointer call)
{

    Arg    args[1];

    XtSetArg(args[0], XmNblendModel, XmBLEND_JUST_SOURCE);
    XtSetValues(w, args, 1);

}


/* This callback procedure removes the icons when the drop is complete */
static void
ColorDragDropFinishCB(Widget w, XtPointer client, XtPointer call)
{

    Widget  sourceIcon;
    Widget  stateIcon = (Widget) client;
    Arg     args[1];

    XtSetArg(args[0], XmNsourceCursorIcon, &sourceIcon);
    XtGetValues(w, args, 1);

    XtDestroyWidget(sourceIcon);
    XtDestroyWidget(stateIcon);

}


/* This action procedure sets up the drag data and begins the drag operation */
void
ColorRect(Widget w, XEvent *event, String *params, Cardinal *num_params)
{

    static XtCallbackRec dragDropFinishCB[] = {
        {ColorDragDropFinishCB, NULL},
        {NULL, NULL}
    };

    static XtCallbackRec dragMotionCB[] = {
        {DragMotionCallback, NULL},
        {NULL, NULL}
    };

    static XtCallbackRec dropSiteLeaveCB[] = {
        {DropSiteLeaveCallback, NULL},
        {NULL, NULL}
    };

    Atom        targets[1];
    Widget      sourceIcon, stateIcon;
    Pixel       background, foreground;
    char        *source_bits, *source_mask;
    char        *state_bits, *state_mask;
    Dimension   width, height;
    Arg         args[16];
    int         n = 0;

    n = 0;
    XtSetArg(args[n], XmNbackground, &background); n++;
    XtSetArg(args[n], XmNforeground, &foreground); n++;
    XtGetValues(w, args, n);

    /* If the server will handle a large icon, create one */
    if (appInfo->maxCursorWidth >= ICON_WIDTH &&
        appInfo->maxCursorHeight >= ICON_HEIGHT) {

        source_bits = (char *)SOURCE_ICON_BITS;
        source_mask = (char *)SOURCE_ICON_MASK;
        state_bits = (char *)STATE_ICON_BITS;
        state_mask = (char *)STATE_ICON_MASK;
        width = ICON_WIDTH;
        height = ICON_HEIGHT;

    }
    else {

        /* If the server will handle a small icon, create one */
        source_bits = (char *)SMALL_SOURCE_ICON_BITS;
        source_mask = (char *)SMALL_SOURCE_ICON_MASK;
        state_bits = (char *)SMALL_STATE_ICON_BITS;
        state_mask = (char *)SMALL_STATE_ICON_MASK;
        width = SMALL_ICON_WIDTH;
        height = SMALL_ICON_HEIGHT;

    }

    /* Create the drag cursor icons */
    sourceIcon = GetDragIconFromBits(w, source_bits, source_mask, width,
                                     height, background, foreground);

    stateIcon = GetDragIconFromBits(w, state_bits, state_mask, width,
                                    height, background, foreground);

    /* Setup the arglist for the drag context that is created at drag start */
    n = 0;
    /* initially only show the source icon */
    XtSetArg(args[n], XmNblendModel, XmBLEND_JUST_SOURCE); n++;

    /* set cursor colors for the drag states */
    XtSetArg(args[n], XmNvalidCursorForeground,
             GetColor(VALID_CURSOR_FG_COLOR)); n++;
    XtSetArg(args[n], XmNinvalidCursorForeground,
             GetColor(INVALID_CURSOR_FG_COLOR)); n++;
    XtSetArg(args[n], XmNnoneCursorForeground,
             GetColor(NONE_CURSOR_FG_COLOR)); n++;

    /* set args for the drag cursor icons */
    XtSetArg(args[n], XmNcursorBackground, background); n++;
    XtSetArg(args[n], XmNcursorForeground, foreground); n++;
    XtSetArg(args[n], XmNsourceCursorIcon, sourceIcon); n++;
    XtSetArg(args[n], XmNstateCursorIcon, stateIcon); n++;

    /* identify the necessary callbacks */
    dragDropFinishCB[0].closure = (XtPointer) stateIcon;
    XtSetArg(args[n], XmNdragDropFinishCallback, dragDropFinishCB); n++;
    dragMotionCB[0].closure = (XtPointer) stateIcon;
    XtSetArg(args[n], XmNdragMotionCallback, dragMotionCB); n++;
    XtSetArg(args[n], XmNdropSiteLeaveCallback, dragMotionCB); n++;

    /* set the drag operations that are supported */
    XtSetArg(args[n], XmNdragOperations, XmDROP_COPY); n++;

    /* start the drag.  This creates a drag context. */
    myDC = XmeDragSource(w, NULL, event, args, n);

}


/*
 * This is a selection conversion function that is used in
 * converting requests for rectangle targets.  The return types follow
 * ICCCM standards.
 */
void
RectConvert(Widget widget, XtPointer ignore, XmConvertCallbackStruct *cs)
{

    Display         *display = XtDisplay(widget);
    Atom            MY_RECT = XmInternAtom(display, "_MY_RECTANGLE", False);
    Atom            RECT_INFO = XmInternAtom(display, "RECT_INFO", False);
    Atom            DELETE = XmInternAtom(display, "DELETE", False);
    Atom            TARGETS = XmInternAtom(display, "TARGETS", False);
    Atom	    ME_TARGETS =
      XmInternAtom(display, XmS_MOTIF_EXPORT_TARGETS, False);
    Atom            *targs;
    int             target_count;
    Arg             args[1];
    RectPtr         rect, oldRect;

    if (cs -> target == MY_RECT) {

        /* Create a new rectangle using information from the old retangle */
        oldRect = (RectPtr) cs -> location_data;

        /* We create create a new rectangle and wait for a delete target on the
	 * old rectangle instead of just moving the old rectangle because
	 * the rectangle movement might be an interclient move.
	 */
        rect = RectCreate(oldRect->x, oldRect->y, oldRect->width,
            oldRect->height, oldRect->color, oldRect->pixmap);
        /* value, type, length, and format must be assigned values */
        cs -> value = (XtPointer) rect;
        cs -> type = RECT_INFO;
        cs -> length = sizeof(*rect);
        cs -> format = 8;
	cs -> status = XmCONVERT_DONE;
    }
    else if (cs -> target == DELETE) {

        /* delete the old rectangle */
        RectHide(XtDisplay(widget), XtWindow(widget), cs -> location_data);
        RectFree(cs -> location_data);

        /*
         * DELETE target return parameters MUST be assigned as follows to
         * ICCCM compliant.
         */
        cs -> value = NULL;
        cs -> type = XmInternAtom(XtDisplay(widget), "NULL", False);
        cs -> length = 0;
        cs -> format = 8;
	cs -> status = XmCONVERT_DONE;
    }
    else if (cs -> target == TARGETS || cs -> target == ME_TARGETS) {
        /* This target is required by ICCCM */
        target_count = 0;

	if (cs -> target == ME_TARGETS)
	  targs = (Atom *)XtMalloc((unsigned) (1 * sizeof(Atom)));
	else
	  targs = XmeStandardTargets(widget, 1, &target_count);

        targs[target_count++] = MY_RECT;

        cs -> value = (XtPointer) targs;
	cs -> length = target_count;
	cs -> format = 32;
	cs -> type = XA_ATOM;
	cs -> status = XmCONVERT_DONE;
    } else
      cs -> status = XmCONVERT_REFUSE;
}


/* This callback procedure removes the old cursor icon */
static void
RectDragDropFinishCB(Widget w, XtPointer client, XtPointer call)
{

    Widget          sourceCursorIcon;
    Arg             args[1];

    XtSetArg(args[0], XmNsourceCursorIcon, &sourceCursorIcon);
    XtGetValues(w, args, 1);
    XtDestroyWidget(sourceCursorIcon);

    /* Free temp data */
    XtFree(client);
}


/* This callback procedure redraws the rectangles once the drop is completed */
static void
RectDropFinishCB(Widget w, XtPointer client, XtPointer call)
{

    Widget redraw = (Widget) client;

    appInfo->clearRect = NULL;
    appInfo->doMove = True;
    RedrawRectangles(redraw);

}


/* This callback procedure handle the drawing of the target
 * rectangle depending of the dropSiteStatus for drag over
 * effects.
 */
static void
RectDragMotionCB(Widget w, XtPointer client, XtPointer call)
{

    XmDragMotionCallback    cb = (XmDragMotionCallback) call;
    DragConvertPtr          conv = (DragConvertPtr) client;
    Display                 *display;
    Window                  window;
    RectPtr                 rect;

    if (cb->dropSiteStatus == XmVALID_DROP_SITE) {

        /* re-stipple the rectangle when the poitner is inside the drop site */
        if ( appInfo->clearRect == NULL && appInfo->doMove) {

            display = XtDisplay(conv->widget);
            window = XtWindow(conv->widget);
            rect = conv->rect;

            RectHide(display, window, rect);
            RectDrawStippled(display, window, rect);

        }

    }
    else {

        /* re-fill the rectangle when the poitner is outside the drop site */
        if (appInfo->clearRect != NULL && appInfo->doMove) {
            appInfo->clearRect = NULL;
            RedrawRectangles(conv->widget);
        }

    }

}


/* This callback procedure handle the drawing of the target
 * rectangle When the operation changes.
 */
static void
RectOperationChangedCB(Widget w, XtPointer client, XtPointer call)
{

    XmDragMotionCallback    cb = (XmDragMotionCallback) call;
    DragConvertPtr          conv = (DragConvertPtr) client;
    Display                 *display;
    Window                  window;
    RectPtr                 rect;

    /* re-stipple the rectangle when the poitner is inside the drop site */
    if ( appInfo->clearRect == NULL && appInfo->doMove) {

        display = XtDisplay(conv->widget);
        window = XtWindow(conv->widget);
        rect = conv->rect;

        RectHide(display, window, rect);
        RectDrawStippled(display, window, rect);

    }

    /* re-fill the rectangle when the operation changes to copy */
    if (appInfo->clearRect != NULL && !appInfo->doMove) {
        appInfo->clearRect = NULL;
        RedrawRectangles(conv->widget);
    }

}


/* This action procedure sets up the drag data and begins the drag operation */
static void
     StartMove(Widget w, XEvent *event, String *params, Cardinal *num_params)
{

    RectPtr     rect;
    Position    x = event->xbutton.x;
    Position    y = event->xbutton.y;
    static XtCallbackRec dragDropFinishCB[] = {
        {RectDragDropFinishCB, NULL},
        {NULL, NULL}
    };

    static XtCallbackRec dropFinishCB[] = {
        {RectDropFinishCB, NULL},
        {NULL, NULL}
    };

    static XtCallbackRec dragMotionCB[] = {
        {RectDragMotionCB, NULL},
        {NULL, NULL}
    };

    static XtCallbackRec operationChangedCB[] = {
        {RectOperationChangedCB, NULL},
        {NULL, NULL}
    };

    Atom            targets[1];
    Display         *display = XtDisplay(w);
    Widget          sourceCursorIcon;
    DragConvertPtr  conv;
    Pixel           background, foreground;
    Arg             args[16];
    int             n = 0;

    /* find a rectangle at the given x,y position */
    rect = RectFind(x, y);

    /* start move only if it begins on a rectangle */
    if (rect) {

        XtSetArg(args[0], XmNbackground, &background);
        XtGetValues(w, args, 1);

        foreground = RectGetColor(rect);
        sourceCursorIcon = GetDragIconFromRect(w, rect, background);

        /* On a move operation, draw the current
	 * rectangle as a stippled outline.
	 */
        if (!(event->xbutton.state & ControlMask)) {
            RectHide(display, XtWindow(w), rect);
            RectDrawStippled(display, XtWindow(w), rect);
        }
        else
            appInfo->doMove = False;

        /* Setup arglist for the drag context that is created at drag start */
        n = 0;
        /* initially only show the source icon */
        XtSetArg(args[n], XmNblendModel, XmBLEND_JUST_SOURCE); n++;

        /* set args for the drag cursor icons */
        XtSetArg(args[n], XmNcursorBackground, background); n++;
        XtSetArg(args[n], XmNcursorForeground, foreground); n++;
        XtSetArg(args[n], XmNsourceCursorIcon, sourceCursorIcon); n++;

	/*
	 * Set up information to pass to the convert
	 * function and callback procs.
	 */
	conv = (DragConvertPtr) XtMalloc(sizeof(DragConvertRec));
	conv->widget = w;
	conv->rect = rect;

        /* identify the necessary callbacks and the client data to be passed */
        dragDropFinishCB[0].closure = (XtPointer) conv;
        XtSetArg(args[n], XmNdragDropFinishCallback, dragDropFinishCB); n++;
        dropFinishCB[0].closure = (XtPointer) w;
        XtSetArg(args[n], XmNdropFinishCallback, dropFinishCB); n++;
        dragMotionCB[0].closure = (XtPointer) conv;
        XtSetArg(args[n], XmNdragMotionCallback, dragMotionCB); n++;
        operationChangedCB[0].closure = (XtPointer) conv;
        XtSetArg(args[n], XmNoperationChangedCallback, operationChangedCB); n++;

        /* set the drag operations that are supported */
        XtSetArg(args[n], XmNdragOperations, XmDROP_COPY | XmDROP_MOVE); n++;

        /* start the drag. This creates a drag context. */
        myDC = XmeDragSource(w, (XtPointer) rect, event, args, n);
    }

}


/* This procedure searches through the export targets and
 * returns flags to indicate which targets were found
 */
static void
CheckTargets(Widget w, Display *display, Boolean *rectFound,
	     Boolean *bgFound, Boolean *pixFound)
{

    Atom        MY_RECT = XmInternAtom(display, "_MY_RECTANGLE", False);
    Atom        BACKGROUND = XmInternAtom(display, "BACKGROUND", False);
    Atom        PIXMAP = XmInternAtom(display, "PIXMAP", False);
    Atom        *exportTargets;
    Cardinal    numExportTargets;
    Arg         args[2];
    int         n;

    /* Get list of transfer targets */
    n = 0;
    XtSetArg(args[0], XmNexportTargets, &exportTargets);
    XtSetArg(args[1], XmNnumExportTargets, &numExportTargets);
    XtGetValues(w, args, 2);

    /* initialize targets found flags */
    *rectFound = *bgFound = *pixFound = False;

    /* search through the export targets */
    for (n = 0; n < numExportTargets; n++) {

        if (exportTargets[n] == MY_RECT)
            *rectFound = True;
        else if (exportTargets[n] == BACKGROUND)
            *bgFound = True;
        else if (exportTargets[n] == PIXMAP)
            *pixFound = True;

    }

}


/* This procedure handles drop site messages and performs the
 * appropriate drag under effects.
 */
static void
DragProcCallback(Widget w, XtPointer client, XtPointer call)
{

    XmDragProcCallbackStruct    *cb = (XmDragProcCallbackStruct *) call;
    Display                     *display = XtDisplay(w);
    Boolean                     rectFound, bgFound, pixFound;
    static unsigned char        initial_operations;
    static unsigned char        initial_operation;
    RectPtr                     rect;

    CheckTargets(cb->dragContext, display, &rectFound, &bgFound, &pixFound);

    switch(cb->reason) {

        case XmCR_DROP_SITE_ENTER_MESSAGE:

            /* save the value of the operations and operation fields */
            initial_operations = cb->operations;
            initial_operation = cb->operation;

            rect = RectFind(cb->x, cb->y);

            /* Remove any operations for the operations field which do not
             * apply to the simulated drop site.
             */
            if (rect) {

                if (bgFound || pixFound) {
                    cb->operations = XmDROP_COPY;
                    RectHighlight(w, rect);
                }
                else if (rectFound) {
                    cb->operations = cb->operations &
                                     (XmDROP_COPY | XmDROP_MOVE);
                    RectUnhighlight(w);
                }

            }
            else {
                cb->operations = initial_operations &
                                 (XmDROP_COPY | XmDROP_MOVE);
                RectUnhighlight(w);
            }

            /* Set operation to the valid operation preferred by the simulated
             * drop site or to XmDROP_NOOP if the operations list does not
             * contain the preferred operation.
             */
            if (rect) {

                if (bgFound || pixFound) {

                    if (cb->operations & XmDROP_COPY)
                        cb->operation = XmDROP_COPY;
                    else
                        cb->operation = XmDROP_NOOP;

                }
                else if (rectFound) {

                    if (cb->operations & XmDROP_MOVE)
                        cb->operation = XmDROP_MOVE;
                    else if (cb->operations & XmDROP_COPY)
                        cb->operation = XmDROP_COPY;
                    else
                        cb->operation = XmDROP_NOOP;

                }

            }
            else {

                if (rectFound) {

                    if (cb->operations & XmDROP_MOVE)
                        cb->operation = XmDROP_MOVE;
                    else if (cb->operations & XmDROP_COPY)
                        cb->operation = XmDROP_COPY;
                    else
                        cb->operation = XmDROP_NOOP;

                }
                else
                    cb->operation = initial_operation;

            }

            /*
             * Set dropSiteStatus to XmDROP_SITE_INVALID if the operation
             * field is XmDROP_NOOP, or if there are no common targets
             * between the source and the nested drop site.  Otherwise, set
             * dropSiteStatus to XmDROP_SITE_VALID.
             */
            if (cb->operation == XmDROP_NOOP ||
                (rect && (!rectFound && !bgFound && !pixFound)) ||
                (!rect && !rectFound))
                cb->dropSiteStatus = XmINVALID_DROP_SITE;
            else
                cb->dropSiteStatus = XmVALID_DROP_SITE;

            /*
             * Display appropriate drag under visuals.  Only highlight
             * the rectangle if we are changing rectangle attributes.
             */
            if ((rect && bgFound) ||
                (pixFound && cb->dropSiteStatus == XmVALID_DROP_SITE))
                RectHighlight(w, rect);
            break;

        case XmCR_DROP_SITE_LEAVE_MESSAGE:

            /* Only unhighlight the rectangle if previously highlighted */
            if (appInfo->highlightRect != NULL)
                RectUnhighlight(w);
            break;

        case XmCR_DROP_SITE_MOTION_MESSAGE:

            rect = RectFind(cb->x, cb->y);

            /*
             * Remove any operations for the operations field which do not
             * apply to the simulated drop site.
             */
            if (rect) {

                if (bgFound || pixFound) {
                    cb->operations = XmDROP_COPY;
                    RectHighlight(w, rect);
                }
                else if (rectFound) {
                    cb->operations = cb->operations &
                                     (XmDROP_COPY | XmDROP_MOVE);
                    RectUnhighlight(w);
                }

            }
            else {
                cb->operations = initial_operations &
                                 (XmDROP_COPY | XmDROP_MOVE);
                RectUnhighlight(w);
            }

            /*
             * Set operation to the valid operation preferred by the simulated
             * drop site or to XmDROP_NOOP if the operations list does not
             * contain the preferred operation.
             */
            if (rect) {

                if (bgFound || pixFound) {

                    if (cb->operations & XmDROP_COPY)
                        cb->operation = XmDROP_COPY;
                    else
                        cb->operation = XmDROP_NOOP;

                }
                else if (rectFound) {

                    if (cb->operations & XmDROP_MOVE)
                        cb->operation = XmDROP_MOVE;
                    else if (cb->operations & XmDROP_COPY)
                        cb->operation = XmDROP_COPY;
                    else
                        cb->operation = XmDROP_NOOP;

                }
            }
            else {

                if (rectFound) {
                    if (cb->operations & XmDROP_MOVE)
                        cb->operation = XmDROP_MOVE;
                    else if (cb->operations & XmDROP_COPY)
                        cb->operation = XmDROP_COPY;
                    else
                        cb->operation = XmDROP_NOOP;

                }
                else
                    cb->operation = initial_operation;

            }

            /*
             * Set dropSiteStatus to XmDROP_SITE_INVALID if the operation
             * field is XmDROP_NOOP, or if there are no common targets
             * between the source and the nested drop site.  Otherwise,
             * set dropSiteStatus to XmDROP_SITE_VALID.
             */
            if (cb->operation == XmDROP_NOOP ||
                (rect && (!rectFound && !bgFound && !pixFound)) ||
                (!rect && !rectFound))
                cb->dropSiteStatus = XmINVALID_DROP_SITE;
            else
                cb->dropSiteStatus = XmVALID_DROP_SITE;

            /*
             * Display appropriate drag under visuals.  Only highlight
             * the rectangle if we are changing rectangle attributes.
             */
            if ((rect && bgFound) ||
                (pixFound && cb->dropSiteStatus == XmVALID_DROP_SITE))
                RectHighlight(w, rect);
            break;

        case XmCR_OPERATION_CHANGED:

            if (rectFound) {

                if (cb->operation == XmDROP_MOVE)
                    appInfo->doMove = True;
                else
                    appInfo->doMove = False;

            }
            break;

        default:

            /* other messages we consider invalid */
            cb->dropSiteStatus = XmINVALID_DROP_SITE;
            break;

    }

    /* allow animation to be performed */
    cb->animate = True;

}


/* This procedure handles the data that is being transfer */
static void
TransferProcCallback(Widget wid,
		     XtPointer closure,
		     XtPointer call_data)
{
    XmSelectionCallbackStruct *cs = (XmSelectionCallbackStruct *) call_data;
    DropTransfer    transferRec = (DropTransfer) closure;
    Display         *display = XtDisplay(wid);
    Atom            RECT_INFO = XmInternAtom(display, "RECT_INFO", False);
    Atom            PIXEL = XmInternAtom(display, "PIXEL", False);
    Atom            NULL_ATOM = XmInternAtom(display, "NULL", False);
    Arg             args[10];
    RectPtr         rect;
    int             n;

    /*
     * The delete target returns a NULL_ATOM type and value equal to NULL
     * so it isn't a failure.  Otherwise, check for NULL value or targets
     * that wee don't support and set transfer failure.
     */
    if (cs -> type != NULL_ATOM &&
	(!cs -> value
	 || (cs -> type != RECT_INFO &&
	     cs -> type != PIXEL &&
	     cs -> type != XA_DRAWABLE))) {
      /* Free the value if there is one, or we would have a memory leak */
      XtFree(cs -> value);
      return;
    }

    /* Handle pixel type (i.e. change in background) */
    if (cs -> type == PIXEL) {
      rect = RectFind(transferRec->x, transferRec->y);
      RectSetColor(rect, display, XtWindow(wid), *((Pixel*)cs -> value));
    }
    /* Handle drawable type (i.e. change in pixmap) */
    else if (cs -> type == XA_DRAWABLE) {
      rect = RectFind(transferRec->x, transferRec->y);
      RectSetPixmap(rect, display, XtWindow(wid), *((Pixmap *)cs -> value));
    }
    /* Handle rect_info type (i.e. new rectangle) */
    else if (cs -> type == RECT_INFO) {
      rect = (RectPtr) cs -> value;
      RectRegister(rect, transferRec->x, transferRec->y);
      RedrawRectangles(wid);
      cs -> value = NULL; /* No need to free, it is being stored in RecTable */
    }

    /* Free the value if there is one, or we would have a memory leak */
    if (cs -> value) XtFree(cs -> value);
}


/* This procedure frees the data used the data transfer proc that
 * was passed from the drop procedure.
 */
static void
DropDestroyCB(Widget w, XtPointer clientData, XtPointer callData)
{
    XtFree((char *)clientData);
}


/* This procedure initiates the drop transfer. */
static void
HandleDrop(Widget w, XtPointer call, XtPointer call_data)
{
  XmDestinationCallbackStruct *cs = (XmDestinationCallbackStruct *) call;
  XmDropProcCallbackStruct *ds;
  static XtCallbackRec dropDestroyCB[] = {
    {DropDestroyCB, NULL},
    {NULL, NULL}
  };
  Display                     *display = XtDisplay(w);
  Arg                         args[10];
  DropTransfer                transferRec;
  int                         n;
  Boolean                     rectFound, bgFound, pixFound;
  RectPtr                     rect;
  Boolean		      transferValid = False;

  /* For drop help we save the struct in a global */
  if (call == NULL) cs = savedCB;

  ds = (XmDropProcCallbackStruct *) cs -> destination_data;

  /* Cancel the drop on invalid drop operations */
  if (!(ds->operations & XmDROP_MOVE || ds->operations & XmDROP_COPY)) {
    XmTransferDone(cs -> transfer_id, XmTRANSFER_DONE_FAIL);
  } else {
    /* Find out which nested dropsite contains the pointer */
    rect = RectFind(ds->x, ds->y);

    CheckTargets(ds->dragContext, display, &rectFound, &bgFound, &pixFound);

    /* rect !NULL indicates we are within a nested dropsite */
    if (rect) {

      /* MY_RECT is a possible target, we'll support it first */
      if (rectFound)
	transferValid = True;
      else if (bgFound || pixFound) {
	/* support only copy with the BACKGROUND and PIXMAP targets */
	if (ds->operation != XmDROP_COPY)
	  ds->operation = XmDROP_COPY;
	transferValid = True;
      }
    } else {
      if (rectFound)
	transferValid = True;
    }

    if (transferValid) {
      /* intialize data to send to drop transfer callback */
      transferRec = (DropTransfer) XtMalloc(sizeof(DropTransferRec));
      transferRec->widget = w;
      transferRec->x = ds->x;
      transferRec->y = ds->y;

      XmTransferValue(cs -> transfer_id,
		      XmInternAtom(display, "_MY_RECTANGLE", False),
		      TransferProcCallback,
		      (XtPointer) transferRec, CurrentTime);
      XmTransferValue(cs -> transfer_id,
		      XmInternAtom(display, "BACKGROUND", False),
		      TransferProcCallback,
		      (XtPointer) transferRec, CurrentTime);
      XmTransferValue(cs -> transfer_id,
		      XmInternAtom(display, "PIXMAP", False),
		      TransferProcCallback,
		      (XtPointer) transferRec, CurrentTime);
      /* Set up move targets */
      if (ds->operation == XmDROP_MOVE) {
	XmTransferValue(cs -> transfer_id,
			XmInternAtom(display, "DELETE", False),
			TransferProcCallback,
			(XtPointer) transferRec, CurrentTime);
      }
    } else {
      XmTransferDone(cs -> transfer_id, XmTRANSFER_DONE_FAIL);
    }
  }
}

/* This procedure is used with the drop help dialog to cancel the drop */
static void
CancelDrop(Widget w, XtPointer call, XtPointer ignore)
{
  XmDestinationCallbackStruct *cs;

  /* For drop help we save the struct in a global */
  cs = savedCB;

  XmTransferDone(cs -> transfer_id, XmTRANSFER_DONE_FAIL);
}


void
ChangeOperation(Widget widget,
		caddr_t client_data,
		XmAnyCallbackStruct *call_data)
{

    if (client_data == 0)
        appInfo->operation = XmDROP_MOVE;
    else
        appInfo->operation = XmDROP_COPY;

}

/* This procedure manages the help dialog and determines which
 * message is displayed in the dialog depending on the position
 * and the type of drop.
 */
static void
HandleHelp(Widget w, XtPointer call, XtPointer ignore)
{
    XmDestinationCallbackStruct *cs = (XmDestinationCallbackStruct *) call;
    XmDropProcCallbackStruct *ds =
    (XmDropProcCallbackStruct *) cs -> destination_data;
  Boolean                         rectFound, bgFound, pixFound;
  XmString                        helpStr = NULL;
  RectPtr                         rect;
  Arg                             args[5];
  XmString                        tempStr, buttonArray[2];
  int                             n = 0;

  savedCB = cs;

  /* if we haven't created a help dialog, create one now */
  if (helpDialog == NULL) {
    XtSetArg(args[n], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL); n++;
    XtSetArg(args[n], XmNtitle, "Drop Help"); n++;
    helpDialog = XmCreateMessageDialog(topLevel, "Help", args, n);

    n = 0;
    buttonArray[0] = XmStringCreateLocalized("Move");
    buttonArray[1] = XmStringCreateLocalized("Copy");
    XtSetArg(args[n], XmNbuttons, buttonArray); n++;
    XtSetArg(args[n], XmNbuttonCount, 2); n++;
    XtSetArg(args[n], XmNbuttonSet, 0); n++;
    XtSetArg(args[n], XmNsimpleCallback, ChangeOperation); n++;
    tempStr = XmStringCreateLocalized("Operations:");
    XtSetArg(args[n], XmNoptionLabel, tempStr); n++;
    helpMenu = XmCreateSimpleOptionMenu(helpDialog, "helpMenu", args, n);
    XmStringFree(tempStr);
    XmStringFree(buttonArray[0]);
    XmStringFree(buttonArray[1]);

    XtAddCallback(helpDialog, XmNokCallback, HandleDrop, NULL);
    XtAddCallback(helpDialog, XmNcancelCallback, CancelDrop, NULL);
    XtUnmanageChild(XtNameToWidget(helpDialog, "Help"));
    XtRealizeWidget(helpDialog);
  }

  /* find the valid targets */
  CheckTargets(ds->dragContext, XtDisplay(w), &rectFound,
	       &bgFound, &pixFound);

  /* determine the appropriate help message */
  if (rectFound) {
    if (ds->operations == (XmDROP_MOVE | XmDROP_COPY)) {
      XtManageChild(helpMenu);
      helpStr = XmStringLtoRCreate(HELP_MSG4, XmFONTLIST_DEFAULT_TAG);
      XtManageChild(XtNameToWidget(helpDialog, "OK"));
    } else if (ds->operation == XmDROP_MOVE) {
      XtUnmanageChild(helpMenu);
      helpStr = XmStringLtoRCreate(HELP_MSG2, XmFONTLIST_DEFAULT_TAG);
      XtManageChild(XtNameToWidget(helpDialog, "OK"));
    } else if (ds->operation == XmDROP_COPY) {
      XtUnmanageChild(helpMenu);
      helpStr = XmStringLtoRCreate(HELP_MSG3, XmFONTLIST_DEFAULT_TAG);
      XtManageChild(XtNameToWidget(helpDialog, "OK"));
    }
  } else if (bgFound || (pixFound && ds->operation == XmDROP_COPY)) {
    XtUnmanageChild(helpMenu);
    rect = RectFind(ds->x, ds->y);
    if (rect) {
      helpStr = XmStringLtoRCreate(HELP_MSG1, XmFONTLIST_DEFAULT_TAG);
      XtManageChild(XtNameToWidget(helpDialog, "OK"));
    } else {
      helpStr = XmStringLtoRCreate(HELP_MSG5, XmFONTLIST_DEFAULT_TAG);
      XtUnmanageChild(XtNameToWidget(helpDialog, "OK"));
    }
  } else {
    XtUnmanageChild(helpMenu);
    helpStr = XmStringLtoRCreate(HELP_MSG5, XmFONTLIST_DEFAULT_TAG);
    XtUnmanageChild(XtNameToWidget(helpDialog, "OK"));
  }

  /* set the help message into the dialog */
  XtSetArg(args[0], XmNmessageString, helpStr);
  XtSetValues(helpDialog, args, 1);

  /* Free the XmString */
  XmStringFree(helpStr);

  /* map the help dialog */
  XtManageChild(helpDialog);
}


/* The procedure either begins the drop of initiates the help dialog
 * depending on the dropAction.
 */
void
handleDestination(Widget w, XtPointer client, XtPointer call)
{
  XmDestinationCallbackStruct *cs = (XmDestinationCallbackStruct *) call;

  if (appInfo->highlightRect != NULL)
    RectUnhighlight(w);

  if (cs->operation != XmOTHER)
    HandleDrop(w, call, NULL);
  else
    HandleHelp(w, call, NULL);
}


/* This procedure registers the drop targets and the drop site */
static void
RegisterDropSite(Widget w)
{

    Display *display = XtDisplay(w);
    Atom    targets[3];
    Arg     args[5];
    int     n = 0;

    /* Only accept moves or copies */
    XtSetArg(args[n], XmNdragOperations, XmDROP_COPY | XmDROP_MOVE); n++;

    /* set all possible targets for any of the nested drop sites */
    targets[0] = XmInternAtom(display, "_MY_RECTANGLE", False);
    targets[1] = XmInternAtom(display, "BACKGROUND", False);
    targets[2] = XmInternAtom(display, "PIXMAP", False);
    XtSetArg(args[n], XmNimportTargets, targets); n++;
    XtSetArg(args[n], XmNnumImportTargets, 3); n++;

    /* register a dragProc - necessary for simulating nested drop sites */
    XtSetArg(args[n], XmNdragProc, DragProcCallback); n++;
    XmeDropSink(w, args, n);

}


static void
SetInvalidIcon(Widget w)
{

    Widget      invalidIcon;
    char        *invalid_bits;
    Dimension   width, height;
    Arg         args[1];

    if (appInfo->maxCursorWidth >= ICON_WIDTH &&
        appInfo->maxCursorHeight >= ICON_HEIGHT) {
        invalid_bits = (char *)INVALID_ICON_BITS;
        width = ICON_WIDTH;
        height = ICON_HEIGHT;
    } else {
        /* If the server will handle a small icon, create one */
        invalid_bits = (char *)SMALL_INVALID_ICON_BITS;
        width = SMALL_ICON_WIDTH;
        height = SMALL_ICON_HEIGHT;
    }

    invalidIcon = GetDragIconFromBits(w, invalid_bits, invalid_bits, width,
                                      height, GetColor(DRAW_AREA_FG_COLOR),
                                      GetColor(DRAW_AREA_BG_COLOR));

    XtSetArg(args[0], XmNdefaultInvalidCursorIcon, invalidIcon);
    XtSetValues(XmGetXmScreen(XtScreen(w)), args, 1);

}


/* This procedure initalizes the toolkit and other application information */
static void
InitializeApplication(int *argc, String *argv)
{

    static XtActionsRec new_actions[] = {
        {"StartRect", StartRect},
        {"ExtendRect", ExtendRect},
        {"EndRect", EndRect},
        {"StartMove", StartMove},
        {"ColorRect", ColorRect},
    };
    Arg         args[5];
    Cardinal    n = 0;

    /* Ininialize struct that hold global information */
    InitializeAppInfo();

    /* Initialize Toolkit and create shell */
    XtSetArg(args[n], XmNwidth, 295); n++;
    XtSetArg(args[n], XmNheight, 270); n++;
    topLevel = XtAppInitialize(&appContext, "DNDDemo", NULL, 0,
                               argc, argv, NULL, args, n);

    /* Set drag protocol styles */
    n = 0;
    XtSetArg(args[n], XmNdragInitiatorProtocolStyle,
             XmDRAG_PREFER_RECEIVER); n++;
    XtSetArg(args[n], XmNdragReceiverProtocolStyle, XmDRAG_DYNAMIC); n++;
    XtSetValues(XmGetXmDisplay(XtDisplay(topLevel)), args, n);

    /* Initialize tables for holding rectangle information */
    InitializeRectDpyTable();

    /* Add new actions for use with translation tables */
    XtAppAddActions(appContext, new_actions, 5);

    /* Get the display server's best cursor size */
    XQueryBestCursor(XtDisplay(topLevel),
                     RootWindowOfScreen(XtScreen(topLevel)), 64, 64,
                     &appInfo->maxCursorWidth, &appInfo->maxCursorHeight);

}

/* This the program start procedure */
int
main (int argc, String *argv)
{

    /* Initialize toolkit and application global values */
    InitializeApplication(&argc, argv);

    /* Create main window, drawing area, and color labels */
    CreateLayout();

    /* Register the drawing area as a drop site */
    RegisterDropSite(drawingArea);

    SetInvalidIcon(drawingArea);

    /* Realize and map widget hiearchy */
    XtRealizeWidget(topLevel);

    /* Create GC for drawing rectangles */
    CreateRectGC();

    /* Begin event loop processing */
    XtAppMainLoop(appContext);

    /* Make compiler happy */
    return 0;
}
