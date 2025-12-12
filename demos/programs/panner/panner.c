/* $TOG: panner.c /main/6 1997/03/31 13:38:32 dbl $ */
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
 *
 */
/*
 * HISTORY
 */
#include <stdio.h>
#include <stdlib.h>
#include <X11/Xmd.h>
#include <Xm/TransferP.h>
#include <Xm/AtomMgr.h>
#include <Xm/CascadeB.h>
#include <Xm/DrawingA.h>
#include <Xm/Frame.h>
#include <Xm/MainW.h>
#include <Xm/MessageB.h>
#include <Xm/Notebook.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/SelectioB.h>
#include <Xm/XmStrDefs.h>

#define INIT_SCREEN_WIDTH	50
#define MAX_DISPLAY_COUNT	10
#define LOCAL			0
#define PAN_FORMAT		8
#define PAN_SIZE		9
#define WM_SELECTION_FORMAT	"WM_S%1d"
#define COLOR_COUNT             20


String fallback[] = {
  "Panner.mappedWhenManaged:		FALSE",
  "Panner.width:			325",
  "Panner.height:			290",

  "Panner*frame.marginWidth:		7",
  "Panner*frame.marginHeight:		7",
  "Panner*notebook.firstPageNumber:	0",

  "Panner*tab.shadowThickness:		1",

  "Panner*canvas.background:		grey40",
  "Panner*canvas.foreground:		yellow",


  /* Menu entry definitions */
  "Panner*cascade1.labelString:		File",
  "Panner*cascade2.labelString:		Display",
  "Panner*cascade3.labelString:		Help",

  "Panner*b1.labelString:		Quit",
  "Panner*b2.labelString:		Update",
  "Panner*b3.labelString:		New Display...",

  "Panner*promptDlog*selectionLabelString: Display name:",

  "Panner*messageDlog*messageString:\
Panner Demo\\n-----------\\nGrab and move dashed box to pan display.\\n\
Use 'New Display...' command to add another display.\\n\
Select displays from tabs in the notebook.",

  "Panner*warningDlog*messageString:\
The panner window is not pinned!\\n\
Add the line:\\n\\n    \"Mwm*Panner*ClientPinned: True\"\\n\\n\
to your .Xdefaults file and restart Mwm.",

  NULL
  };

typedef struct _PannerInfoRec {
  Display      *display;
  Screen       *screen;
  Widget        shell;
  Widget        utmShell; 	/* drawing area used for UTM */
  Widget        canvas;
  int           thumbX, thumbY;
  unsigned int  thumbW, thumbH;
  int           lastEventX, lastEventY;
  Atom		WM;		/* need selections on the correct display */
  Atom		WM_PAN;
  Atom		WM_GOTO;
  Atom		WM_PAN_POS;
} PannerInfoRec;

typedef struct _PanPostion {
  long x;
  long y;
} PanPosition;


typedef enum { MENU_QUIT, MENU_UPDATE, MENU_NEW, MENU_HELP } MenuFunction;
typedef enum { UNKNOWN, VERIFYING, VERIFIED } PinState;

/*
 * globals.
 */
XtAppContext   app;
unsigned short DSP;		/* index of active display */
Widget         notebook;
GC             thumbGC, canvasGC;
XContext       context;
PannerInfoRec *pInfoList;
unsigned long  cells[COLOR_COUNT];
PinState       pinnedState = UNKNOWN;
int            origX, origY;

#ifdef X_ONLY
Boolean        LOCK = False;
#endif

static void      OpenNewDisplay(String, Widget, PannerInfoRec *);
static void      UpdatePannerCB (Widget, XtPointer, XtPointer);
static void      ChangePageCB (Widget, XtPointer, XtPointer);
static void      DoAddDisplayCB (Widget, XtPointer, XtPointer);
static void      DestinationCB (Widget, XtPointer, XtPointer);
static void      DoneMoveScreenCB (Widget, XtPointer, XtPointer);
static void HandlePropertyChange(Widget w, XtPointer clientData,
                                 XEvent *event, Boolean *cont);
static void      UpdatePannerView (PannerInfoRec *pInfoList, int remoteDsp);
static void      DrawWindows (PannerInfoRec *);
static void      DrawThumb (PannerInfoRec *);
static void      SetupColorsAndGCs();
static GC        GetXorGC (Widget);
static GC        GetCanvasGC (Widget);
static void      SetWindowColor (PannerInfoRec *, int);
static void      TranslateCoordinates (PannerInfoRec *,
				       int, int, unsigned int, unsigned int,
				       int*,int*,unsigned int*,unsigned int*);
static int       IgnoreError (Display *dsp, XErrorEvent *event);
static void      StartTracking (Widget, XtPointer, XEvent *, Boolean *);
static void      DoTracking (Widget, XtPointer, XEvent *, Boolean *);
static void      StopTracking (Widget, XtPointer, XEvent *, Boolean *);
static void      MoveScreen (PannerInfoRec *, int, int, Time);
static XtPointer PackCARD32 (XtPointer, CARD32);
static XtPointer PackCARD16 (XtPointer, CARD16);
static XtPointer PackCARD8 (XtPointer, CARD8);
static void      CreateMenuBar (Widget parent);
static void      MenuCB (Widget w, XtPointer clientData, XtPointer callData);
static void      DoUpdatePanner ();
static void      DoAddDisplay ();
static void      DoHelp ();
static void      DoQuit ();
static void      CheckPinnedState ();
static void      ShowPinStateWarning ();
static void      HandleInitialExpose (Widget, XtPointer, XEvent *, Boolean *);

/*----------------------------------------------------------------*
 |                             main                               |
 *----------------------------------------------------------------*/
int main (int argc, char *argv[])
{
  Widget mainWin, frame;
  XEvent event;

  pInfoList = (PannerInfoRec *) XtMalloc(sizeof(PannerInfoRec) *
					 MAX_DISPLAY_COUNT);
  for (DSP = 0;  DSP < MAX_DISPLAY_COUNT;  DSP++)
    pInfoList[DSP].display = NULL;
  DSP = LOCAL;

  XtSetLanguageProc(NULL, (XtLanguageProc) NULL, NULL);
  pInfoList[LOCAL].shell   = XtVaOpenApplication(&app, "Panner", NULL, 0,
					       &argc, argv,
                                               fallback,
                                               sessionShellWidgetClass, NULL);
  pInfoList[LOCAL].display = XtDisplay(pInfoList[LOCAL].shell);


  mainWin = XmCreateMainWindow(pInfoList[LOCAL].shell, "mainWin", NULL, 0);
  XtManageChild(mainWin);
  CreateMenuBar(mainWin);

  frame     = XtVaCreateManagedWidget("frame", xmFrameWidgetClass, mainWin,
				      NULL);
  notebook  = XtVaCreateManagedWidget("notebook", xmNotebookWidgetClass, frame,
				      NULL);

  XtRealizeWidget(pInfoList[LOCAL].shell);

  context  = XUniqueContext ();
  OpenNewDisplay(/*$DISPLAY*/NULL, notebook, pInfoList);
  SetupColorsAndGCs();
  XtAddCallback(notebook, XmNpageChangedCallback, ChangePageCB, pInfoList);
  XtAddEventHandler(notebook, ExposureMask, False, HandleInitialExpose, NULL);

  /* Watch whenever the window manager's panning position changes. */
  /* Mwm stores the position in properties on the root window.     */
  /* This is stored in the _MOTIF_WM_PAN_POSITION property.        */
  XtAddEventHandler(pInfoList[LOCAL].shell, PropertyNotify, False, HandlePropertyChange, NULL);
  XtMapWidget(pInfoList[LOCAL].shell);
  XtAppMainLoop(app);
  return 0;
}

/*----------------------------------------------------------------*
 |                        OpenNewDisplay                          |
 | - Get new display connection to named display.                 |
 | - If display name is not NULL, create shell on the display.    |
 | - Fill in correct record in infoList.                          |
 *----------------------------------------------------------------*/
static void
OpenNewDisplay(
     String         displayName,
     Widget         nb,
     PannerInfoRec *infoList)
{
  int             newDsp = 0;
  int             argc = 0;
  char          **argv = NULL;
  Dimension       canvasW, canvasH;
  char            selectionName[40];
  XmString        tabName;
  XtCallbackList  cbList;

  /*
   * If NULL, then the display's already been created.
   */
  if (displayName != NULL)
    {
      XtVaGetValues(nb, XmNlastPageNumber, &newDsp,
		    XmNpageChangedCallback, &cbList, NULL);
      newDsp++;


      if ((infoList[newDsp].display = XOpenDisplay(displayName)) == NULL)
	{
	  fprintf(stderr, "ERROR - Can't open display \"%s\".\n", displayName);
	  return;
	}

      XtDisplayInitialize(app, infoList[newDsp].display, "panner", "Panner",
			  NULL, 0, &argc, argv);

      /* create an unmapped shell on the remote display */
      infoList[newDsp].shell =
	XtVaAppCreateShell( "panner", "Panner", applicationShellWidgetClass,
			   infoList[newDsp].display,
			   XmNmappedWhenManaged, False, NULL);

      XtRealizeWidget(infoList[newDsp].shell);
      XtRegisterDrawable(infoList[newDsp].display,
                         DefaultRootWindow(infoList[newDsp].display),
                         infoList[newDsp].shell);
    }
  /*
   * For UTM to work, there must be a drawing area or UTM saavy
   * widget.
   *
   *  We must set-up a destination callback function that
   *  does the actual transfer of the parameter info to Mwm.
   */

  infoList[newDsp].utmShell
    = XtVaCreateManagedWidget("utmShell", xmDrawingAreaWidgetClass,
			      infoList[newDsp].shell,
			      XmNmappedWhenManaged, False,
			      NULL);

  XtAddCallback(infoList[newDsp].utmShell, XmNdestinationCallback,
		DestinationCB, &(infoList[newDsp]));

  /*
   * Initialize the correct record in the infoList.
   */
  infoList[newDsp].screen = XtScreen(infoList[newDsp].shell);

  /*
   * setup handler to watch when Mwm changes the root property.
   * first store some data on the root window.
   */
  XSaveContext(infoList[newDsp].display,
	       DefaultRootWindow(infoList[newDsp].display),
	       context,
	       (XPointer)(long)newDsp); /* store index into panner info. */

  /*
   * Add another page to the notebook.
   * First must set size correctly.
   */

  XtVaGetValues(infoList[LOCAL].shell,
		 XmNwidth, &canvasW, XmNheight, &canvasH, NULL);
  infoList[newDsp].canvas
    = XtVaCreateManagedWidget("canvas", xmDrawingAreaWidgetClass, nb,
			      XmNchildType, XmPAGE,
			      XmNpageNumber, newDsp,
			      XmNwidth, canvasW,
			      XmNheight, canvasH,
			      NULL);
  XtAddCallback(infoList[newDsp].canvas, XmNexposeCallback, UpdatePannerCB,
		infoList);

  if (displayName == NULL)
    tabName = XmStringCreate("LOCAL",XmFONTLIST_DEFAULT_TAG);
  else
    tabName = XmStringCreate(displayName,XmFONTLIST_DEFAULT_TAG);
  XtVaCreateManagedWidget("tab", xmPushButtonWidgetClass, nb,
				XmNlabelString, tabName,
				XmNchildType, XmMAJOR_TAB, NULL);
  XmStringFree(tabName);

  infoList[newDsp].thumbW = INIT_SCREEN_WIDTH;
  infoList[newDsp].thumbH = infoList[newDsp].thumbW *
                             HeightOfScreen(infoList[newDsp].screen) /
			     WidthOfScreen(infoList[newDsp].screen);
  XtVaGetValues(infoList[newDsp].canvas, XmNwidth,  &canvasW,
		                          XmNheight, &canvasH, NULL);
  infoList[newDsp].thumbX = (int)canvasW/2 - (int)infoList[newDsp].thumbW/2;
  infoList[newDsp].thumbY = (int)canvasH/2 - (int)infoList[newDsp].thumbH/2;

  /* Setup the atoms needed to communicate with Mwm. Check screen number! */
  sprintf(selectionName, WM_SELECTION_FORMAT,
	  XScreenNumberOfScreen(infoList[newDsp].screen));
  infoList[newDsp].WM         = XmInternAtom(infoList[newDsp].display,
					      selectionName, False);
  infoList[newDsp].WM_PAN     = XmInternAtom(infoList[newDsp].display,
					      "_MOTIF_WM_PAN", False);
  infoList[newDsp].WM_GOTO    = XmInternAtom(infoList[newDsp].display,
					      "_MOTIF_WM_GOTO", False);
  infoList[newDsp].WM_PAN_POS = XmInternAtom(infoList[newDsp].display,
					      "_MOTIF_WM_PAN_POSITION", False);

  XtAddEventHandler(infoList[newDsp].canvas, ButtonPressMask, False,
		    StartTracking, (XtPointer)&infoList[newDsp]);
}

/*========================== CALLBACKS ==========================*/

/*----------------------------------------------------------------*
 |                        UpdatePannerCB                          |
 *----------------------------------------------------------------*/
static void
UpdatePannerCB (
     Widget    w,
     XtPointer clientData,
     XtPointer callData)
{
  XmDrawingAreaCallbackStruct *cb = (XmDrawingAreaCallbackStruct *)callData;
  PannerInfoRec *infoList = (PannerInfoRec *)clientData;

  (void)w;
  if (cb->reason == XmCR_EXPOSE)
    {
      XExposeEvent *event = (XExposeEvent *)cb->event;

      /* Last expose event received - do the drawing. */
      if (event->count == 0)
	UpdatePannerView(infoList, DSP);
    }

  else
    /*
     * Update button pressed.
     */
    UpdatePannerView(infoList, DSP);
}

/*----------------------------------------------------------------*
 |                         ChangePageCB                           |
 | Invoked just prior to notebook page change. Any drawing here   |
 | would be lost.                                                 |
 *----------------------------------------------------------------*/
static void
ChangePageCB (
     Widget    w,
     XtPointer clientData,
     XtPointer callData)
{
  PannerInfoRec            *infoList = (PannerInfoRec *)clientData;
  XmNotebookCallbackStruct *nbData    = (XmNotebookCallbackStruct *)callData;
  int  pageNumber;

  (void)w;
  pageNumber = nbData->page_number;
  if ((pageNumber >= MAX_DISPLAY_COUNT) ||
      (infoList[pageNumber].display == NULL))
    {
      fprintf(stderr, "ERROR - bad display index. (%d).\n", pageNumber);
    }
  else
    {
      DSP = pageNumber;
    }
}

/*----------------------------------------------------------------*
 |                        DoAddDisplayCB                          |
 *----------------------------------------------------------------*/
static void
DoAddDisplayCB (
     Widget    w,
     XtPointer clientData,
     XtPointer callData)
{
  XmSelectionBoxCallbackStruct *cb = (XmSelectionBoxCallbackStruct *)callData;
  PannerInfoRec *infoList = (PannerInfoRec *)clientData;
  char          *dspName;           /* Free when done. */
  String         appName, appClass; /* Don't free - owned by Xt. */

  (void)w;
  XtGetApplicationNameAndClass(infoList[LOCAL].display, &appName, &appClass);
  dspName = XmStringUnparse(cb->value, NULL, XmCHARSET_TEXT, XmCHARSET_TEXT, NULL, 0, XmOUTPUT_ALL);
  OpenNewDisplay(dspName, notebook, infoList);
  if (dspName) XtFree(dspName);
}

/*----------------------------------------------------------------*
 |                         DestinationCB                          |
 | This function gets invoked by UTM when a sink has been estab-  |
 | lished and a request initiated against another selection.  The |
 | purpose here is to set-up the parameters and pass them to the  |
 | owner of the selection.  The parameter data has already been   |
 | allocated in the MoveScreen() function.                        |
 | clientData holds the pannerInfoRec corresponding to the right  |
 | display.                                                       |
 *----------------------------------------------------------------*/
static void
DestinationCB (
     Widget w,
     XtPointer clientData,
     XtPointer callData)
{
  XmDestinationCallbackStruct *dcs = (XmDestinationCallbackStruct *)callData;
  PannerInfoRec *pInfo = (PannerInfoRec *)clientData;

  /*
   * Pass the data to free in the clientData field.
   * location_data points to the param data. This was set in
   * MoveScreen().
   */

  /* FIRST - setup the parameters to pass. */
  (void)w;
  XmTransferSetParameters(dcs->transfer_id,
			  dcs->location_data,   /* pointer to param data. */
			  PAN_FORMAT,
			  PAN_SIZE,		/* should be calculated. */
			  dcs->selection);	/* type - don't care. */

  /* LAST - Make the transfer. */
  XmTransferValue(dcs->transfer_id,
		  pInfo->WM_PAN,	/* target for conversion. */
		  DoneMoveScreenCB,	/* CB proc invoked when done. */
		  dcs->location_data,	/* clientData - to be freed. */
		  dcs->time);
}

/*----------------------------------------------------------------*
 |                       DoneMoveScreenCB                         |
 *----------------------------------------------------------------*/
static void
DoneMoveScreenCB (
     Widget w,
     XtPointer clientData,
     XtPointer callData)
{
  /*
   * Conversion completed. Safe to free param data.
   */
  (void)w;
  (void)callData;
  XtFree((char *)clientData);
}

/*=========================== PAN-HANDLING ==========================*/
/*----------------------------------------------------------------*
 |                     HandlePropertyChange                       |
 | This routine checks the property changed and if its the right  |
 | property, grab the new panning position.                       |
 *----------------------------------------------------------------*/
static void HandlePropertyChange(Widget w, XtPointer clientData,
                                 XEvent *event, Boolean *cont)
{
  XPropertyEvent *propEvent = (XPropertyEvent *)event;
  int             iDsp;

  /* Get the correct info record stored with the context manager. */
  (void)w;
  (void)clientData;
  (void)cont;
  if (XFindContext(propEvent->display, propEvent->window, context, (XPointer *)&iDsp))
    return;

  /* check if this is the right one. Othersize, we'll update when another
   * display changes.
   */
  if (propEvent && propEvent->atom != pInfoList[iDsp].WM_PAN_POS)
    return;

  /* if the display doesn't match the current one, then punt. */
  if (iDsp == DSP)
    {
      if (pinnedState == VERIFIED)
	UpdatePannerView(pInfoList, iDsp);
      else
	{
	  Window        rWin, child;
	  int           x, y, newX, newY;
	  unsigned int  width, height, bWidth, depth;

	  /* Get position of the top-level shell */
	  XGetGeometry(pInfoList[LOCAL].display, XtWindow(pInfoList[LOCAL].shell),
		       &rWin, &x, &y, &width, &height, &bWidth, &depth);

	  XTranslateCoordinates(pInfoList[LOCAL].display,
				XtWindow(pInfoList[LOCAL].shell),
				rWin, x, y, &newX, &newY, &child);

	  if ((newX == origX) && (newY == origY))
	    pinnedState = VERIFIED;
	  else
	    ShowPinStateWarning();
	}
    }

#ifdef X_ONLY
  LOCK = False;
#endif
}

/*============================ DRAWING ===========================*/

/*----------------------------------------------------------------*
 |                         UpdatePannerView                       |
 *----------------------------------------------------------------*/
static void
UpdatePannerView (
     PannerInfoRec *infoList,
     int remoteDsp)
{
  XClearArea(infoList[LOCAL].display,
	     XtWindow(infoList[remoteDsp].canvas),
	     0, 0, 0, 0, False);

  DrawWindows(infoList);
  DrawThumb(&infoList[remoteDsp]);
}


/*----------------------------------------------------------------*
 |                          DrawWindows                           |
 *----------------------------------------------------------------*/
static void
DrawWindows (PannerInfoRec *infoList)
{
  Window        realRoot, root, parent, *child = NULL;
  int           i, x, y;
  unsigned int  childCount, width, height;
  int (*oldHandler)();


  realRoot = RootWindow(infoList[DSP].display,
			XScreenNumberOfScreen(infoList[DSP].screen));

  if (XQueryTree(infoList[DSP].display, realRoot,
		 &root, &parent, &child, &childCount))
    {
      /*
       * We need to install an error handler since the window-tree may
       * become invalid while where still processing the list.
       */
      oldHandler = XSetErrorHandler(IgnoreError);
      for (i=0; i<childCount; i++)
	{
	  XWindowAttributes attr;

	  XGetWindowAttributes(infoList[DSP].display, child[i], &attr);

	  if (attr.map_state == IsViewable)
	    {
	      TranslateCoordinates(&infoList[DSP],
				   attr.x, attr.y, attr.width, attr.height,
				   &x, &y, &width, &height);

	      SetWindowColor (&infoList[LOCAL], i);
	      XFillRectangle(infoList[LOCAL].display,
			     XtWindow(infoList[DSP].canvas),
			     canvasGC, x, y, width, height);
	    }
	}
      XSetErrorHandler(oldHandler);

      if (child != NULL)
	XFree((char*)child);
    }
}

/*----------------------------------------------------------------*
 |                          DrawThumb                             |
 *----------------------------------------------------------------*/
static void
DrawThumb (PannerInfoRec *pInfo)
{
  XDrawRectangle(XtDisplay(pInfo->canvas), XtWindow(pInfo->canvas), thumbGC,
		 pInfo->thumbX, pInfo->thumbY,
		 pInfo->thumbW, pInfo->thumbH);
}

/*----------------------------------------------------------------*
 |                       SetupColorsAndGCs                        |
 | Called once at the beginning to setup some drawing stuff.      |
 *----------------------------------------------------------------*/
static void SetupColorsAndGCs(void)
{
  int i;
  XColor color;
  Colormap cmap = DefaultColormapOfScreen(pInfoList[LOCAL].screen);

  /*
   * set-up the global GCs.
   */
  thumbGC  = GetXorGC(pInfoList[LOCAL].canvas);
  canvasGC = GetCanvasGC(pInfoList[LOCAL].canvas);

  /*
   * Allocate the global color cells for the drawing of each windows.
   * The more random the colors, the better.
   */
  if (XAllocColorCells(pInfoList[LOCAL].display,
		       cmap, False, NULL, 0, cells, COLOR_COUNT))
    for (i=0; i<COLOR_COUNT; i++)
      {
	color.red = rand() % 65535;
	color.blue = rand() % 65535;
	color.green = rand() % 65535;
	color.pixel = cells[i];
	XStoreColor(pInfoList[LOCAL].display, cmap, &color);
      }
}

/*----------------------------------------------------------------*
 |                           GetXorGC                             |
 *----------------------------------------------------------------*/
static GC GetXorGC(Widget w)
{
  GC        gc;
  XGCValues values;


  XtVaGetValues(w, XmNforeground, &values.foreground,
		XmNbackground, &values.background, NULL);
  values.foreground = values.foreground ^ values.background;
  values.function = GXxor;
  values.line_style = LineOnOffDash;

  gc = XtGetGC(w,
	       GCForeground | GCBackground | GCFunction | GCLineStyle,
	       &values);

  return gc;
}

/*----------------------------------------------------------------*
 |                         GetCanvasGC                            |
 *----------------------------------------------------------------*/
static GC GetCanvasGC(Widget w)
{
  GC        gc;
  XGCValues values;

  XtVaGetValues(w, XmNforeground, &values.foreground,
		XmNbackground, &values.background, NULL);
  values.foreground = values.background;
  values.function = GXcopy;

  gc = XtGetGC(w,
	       GCForeground | GCBackground | GCFunction,
	       &values);

  return gc;
}

/*----------------------------------------------------------------*
 |                       SetWindowColor                           |
 *----------------------------------------------------------------*/
static void
SetWindowColor (PannerInfoRec *pInfo, int i)
{
  XSetForeground(pInfo->display, canvasGC, cells[(i+1)%COLOR_COUNT]);
}


/*----------------------------------------------------------------*
 |                     TranslateCoordinates                       |
 *----------------------------------------------------------------*/
static void
TranslateCoordinates (
     PannerInfoRec *pInfo,
     int  x1, int  y1, unsigned int  width1, unsigned int  height1,
     int *x2, int *y2, unsigned int *width2, unsigned int *height2)
{
  int rootW, rootH;


  rootW = WidthOfScreen(pInfo->screen);
  rootH = HeightOfScreen(pInfo->screen);

  *x2      = x1      * (int)pInfo->thumbW / (int)rootW + pInfo->thumbX;
  *y2      = y1      * (int)pInfo->thumbH / (int)rootH + pInfo->thumbY;
  *width2  = width1  * pInfo->thumbW / rootW;
  *height2 = height1 * pInfo->thumbH / rootH;
}


/*----------------------------------------------------------------*
 |                          IgnoreError                           |
 *----------------------------------------------------------------*/
static int
IgnoreError (Display *dsp, XErrorEvent *event)
{
  /*
   * Do Nothing...
   * This is needed since we will may be updating the list of window
   * while one of them goes away.  Ie: the window list received from
   * XQueryTree may not be valid for the entire loop where we get each
   * window's geometry.
   */
  (void)dsp;
  (void)event;
  return 0;
}

/*======================= TRACKING HANDLERS ======================*/

/*----------------------------------------------------------------*
 |                         StartTracking                          |
 *----------------------------------------------------------------*/
static void
StartTracking (
     Widget     w,
     XtPointer  clientData,
     XEvent    *event,
     Boolean   *dispatch)
{
  PannerInfoRec      *pInfo       = (PannerInfoRec *)clientData;
  XPointerMovedEvent *motionEvent = (XPointerMovedEvent *)event;

  (void)dispatch;
  if ((pinnedState == VERIFIED) && (event->xbutton.button == Button1))
    {
      pInfo->lastEventX = event->xbutton.x;
      pInfo->lastEventY = event->xbutton.y;

      if ((event->xbutton.x < pInfo->thumbX) ||
	  (event->xbutton.y < pInfo->thumbY) ||
	  (event->xbutton.x > pInfo->thumbX + (int)pInfo->thumbW) ||
	  (event->xbutton.y > pInfo->thumbY + (int)pInfo->thumbH))
	{
	  /*
	   * if on the canvas, then center the thumb over the pointer.
	   */

	  MoveScreen(pInfo,
		     event->xbutton.x - (int)pInfo->thumbW/2,
		     event->xbutton.y - (int)pInfo->thumbH/2,
		     motionEvent->time);
	}

      XtAddEventHandler(w, ButtonReleaseMask, False, StopTracking, clientData);
      XtAddEventHandler(w, Button1MotionMask, False, DoTracking,   clientData);
    }
  else if (pinnedState != VERIFIED)
    CheckPinnedState ();
}

/*----------------------------------------------------------------*
 |                          DoTracking                            |
 *----------------------------------------------------------------*/
static void
DoTracking (
     Widget w,
     XtPointer clientData,
     XEvent *event,
     Boolean *dispatch)
{
  PannerInfoRec      *pInfo       = (PannerInfoRec *)clientData;
  XPointerMovedEvent *motionEvent = (XPointerMovedEvent *)event;

  (void)w;
  MoveScreen(pInfo,
	     pInfo->thumbX + event->xbutton.x - pInfo->lastEventX,
	     pInfo->thumbY + event->xbutton.y - pInfo->lastEventY,
	     motionEvent->time);

  pInfo->lastEventX = event->xbutton.x;
  pInfo->lastEventY = event->xbutton.y;
}

/*----------------------------------------------------------------*
 |                         StopTracking                           |
 *----------------------------------------------------------------*/
static void
StopTracking (
     Widget     w,
     XtPointer  clientData,
     XEvent    *event,
     Boolean   *dispatch)
{
  (void)dispatch;
  if (event->xbutton.button == Button1)
    {
      XtRemoveEventHandler(w, Button1MotionMask, False, DoTracking,
			   clientData);
      XtRemoveEventHandler(w, ButtonReleaseMask, False, StopTracking,
			   clientData);
    }
}

/*----------------------------------------------------------------*
 |                          MoveScreen                            |
 *----------------------------------------------------------------*/
static void
MoveScreen (
     PannerInfoRec *pInfo,
     int            newX,
     int            newY,
     Time           time)
{
  int           dx, dy, panDx, panDy, rootW, rootH;
  XtPointer     msg, fulldata;
  unsigned long size;

  DrawThumb(pInfo);
  dx = newX - pInfo->thumbX;
  dy = newY - pInfo->thumbY;

  pInfo->thumbX = newX;
  pInfo->thumbY = newY;
  DrawThumb(pInfo);

  /*
   * Send Pan message to mwm.
   */

  rootW = WidthOfScreen(pInfo->screen);
  rootH = HeightOfScreen(pInfo->screen);
  panDx = -(dx * rootW / (int)pInfo->thumbW);
  panDy = -(dy * rootH / (int)pInfo->thumbH);

  size  = sizeof(CARD32);  /* panDx */
  size += sizeof(CARD32);  /* panDy */
  size += sizeof(CARD8);   /* config */

  msg = fulldata = (XtPointer) XtMalloc(sizeof(CARD8) * size);
  msg = PackCARD32(msg, panDx);
  msg = PackCARD32(msg, panDy);
  (void)PackCARD8(msg, True);

#ifdef X_ONLY
  {
    Display       *display;
    Window         window;
    Atom           MY_PANNER_PROP;


    display = XtDisplay(notebook); /* notebook just happens to be a global. */
    window = XtWindow(notebook);
    MY_PANNER_PROP = XInternAtom(display, "MY_PANNER_PROP", False);


    /*
     * Note - to really make this work across multiple displays,
     * the window argument must reside on the same display as WM_Si.
     */

    /*
     * Use a lock to make sure the property was read by Mwm.
     * When the pan-property is updated, it's safe to make another
     * conversion.
     */
    if (!LOCK)
      {
	LOCK = True; /* Freed in HandlePropertyChange. */
	XChangeProperty(display, window, MY_PANNER_PROP, MY_PANNER_PROP,
			PAN_FORMAT, PropModeReplace,
			(unsigned char *)fulldata, size);

	XConvertSelection(pInfo->display, pInfo->WM, pInfo->WM_PAN,
			  MY_PANNER_PROP,
			  window, time);
      }
  }
#else
  if (! XmeNamedSink(pInfo->utmShell,	/* widget with destination callback */
		     pInfo->WM,	/* named selection - ie. Window Manager */
		     XmCOPY,		/* operation to perform on the data */
		     (XtPointer)fulldata, /* pointer to param data, */
		     time)		  /*  free n TransferDone proc. */
      )
    printf("Error - UTM Transfer failed.\n");
#endif /* X_ONLY */
}

/*----------------------------------------------------------------*
 |                      PACKING FUNCTIONS                         |
 *----------------------------------------------------------------*/

static XtPointer
PackCARD32 (XtPointer data, CARD32 val)
{
  CARD16 bottom = val & (0xFFFF);
  CARD16 top = val >> 16;

  data = PackCARD16(data, top);
  data = PackCARD16(data, bottom);
  return(data);
}


static XtPointer
PackCARD16 (XtPointer data, CARD16 val)
{
  CARD8 bottom = val & (0xFF);
  CARD8 top = val >> 8;

  data = PackCARD8(data, top);
  data = PackCARD8(data, bottom);
  return(data);
}


static XtPointer
PackCARD8 (XtPointer data, CARD8 val)
{
  CARD8 *ptr = (CARD8 *) data;

  *ptr = (CARD8) val;
  data = ((char*)data) + 1;
  return(data);
}

/*======================= USER INTERFACE ======================*/

/*----------------------------------------------------------------*
 |                         CreateMenuBar                          |
 *----------------------------------------------------------------*/
static void
CreateMenuBar (Widget parent)
{
  Cardinal n;
  Arg args[10];
  Widget menuBar;
  Widget cascade1, cascade2, cascade3;
  Widget menuPane1, menuPane2;
  Widget b1, b2, b3;

  menuBar   = XmCreateMenuBar(parent, "menuBar", NULL, 0);

  menuPane1 = XmCreatePulldownMenu(menuBar, "menuPane1", NULL, 0);
  menuPane2 = XmCreatePulldownMenu(menuBar, "menuPane2", NULL, 0);

  b1 = XtCreateManagedWidget("b1", xmPushButtonWidgetClass, menuPane1, NULL,0);
  XtAddCallback(b1, XmNactivateCallback, MenuCB, (XtPointer)MENU_QUIT);

  b2 = XtCreateManagedWidget("b2", xmPushButtonWidgetClass, menuPane2, NULL,0);
  XtAddCallback(b2, XmNactivateCallback, MenuCB, (XtPointer)MENU_UPDATE);

  b3 = XtCreateManagedWidget("b3", xmPushButtonWidgetClass, menuPane2, NULL,0);
  XtAddCallback(b3, XmNactivateCallback, MenuCB, (XtPointer)MENU_NEW);

  n = 0;
  XtSetArg(args[n], XmNsubMenuId, menuPane1);  n++;
  cascade1 = XmCreateCascadeButton(menuBar, "cascade1", args, n);
  XtManageChild(cascade1);

  n = 0;
  XtSetArg(args[n], XmNsubMenuId, menuPane2);  n++;
  cascade2 = XmCreateCascadeButton(menuBar, "cascade2", args, n);
  XtManageChild(cascade2);

  n = 0;
  cascade3 = XmCreateCascadeButton(menuBar, "cascade3", args, n);
  XtAddCallback(cascade3, XmNactivateCallback, MenuCB, (XtPointer)MENU_HELP);
  XtManageChild(cascade3);

  n = 0;
  XtSetArg(args[n], XmNmenuHelpWidget, cascade3);  n++;
  XtSetValues(menuBar, args, n);

  XtManageChild(menuBar);
}


/*----------------------------------------------------------------*
 |                             MenuCB                             |
 *----------------------------------------------------------------*/
static void
MenuCB (Widget w, XtPointer clientData, XtPointer callData)
{
  (void)w;
  (void)callData;
  switch ((long)clientData)
    {
    case MENU_UPDATE: DoUpdatePanner();  break;
    case MENU_NEW:    DoAddDisplay();    break;
    case MENU_QUIT:   DoQuit();          break;
    case MENU_HELP:   DoHelp();          break;
    }
}

/*----------------------------------------------------------------*
 |                         DoUpdatePanner                         |
 *----------------------------------------------------------------*/
static void DoUpdatePanner(void)
{
  XClearArea(pInfoList[LOCAL].display,
	     XtWindow(pInfoList[DSP].canvas),
	     0, 0, 0, 0, False);

  DrawWindows(pInfoList);
  DrawThumb(&pInfoList[DSP]);
}

/*----------------------------------------------------------------*
 |                          DoAddDisplay                          |
 *----------------------------------------------------------------*/
static void DoAddDisplay(void)
{
  static Widget dlog = NULL;
  Arg           args[3];
  int           n;

  if (dlog == NULL)
    {
      n = 0;
      XtSetArg(args[n], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL); n++;
      dlog = XmCreatePromptDialog(pInfoList[LOCAL].shell, "promptDlog",
				  args, n);

      XtAddCallback(dlog, XmNokCallback, DoAddDisplayCB, pInfoList);
      XtUnmanageChild(XtNameToWidget(dlog, "Help"));
    }
  XtManageChild(dlog);
}

/*----------------------------------------------------------------*
 |                              DoHelp                            |
 *----------------------------------------------------------------*/
static void DoHelp(void)
{
	static Widget dlog = NULL;

	if (!dlog) {
		dlog = XmCreateInformationDialog(pInfoList[LOCAL].shell, "messageDlog", NULL, 0);
		XtUnmanageChild(XtNameToWidget(dlog, "Help"));
		XtUnmanageChild(XtNameToWidget(dlog, "Cancel"));
	}

	XtManageChild(dlog);
}

/*----------------------------------------------------------------*
 |                              DoQuit                            |
 *----------------------------------------------------------------*/
static void DoQuit(void)
{
	XtAppSetExitFlag(app);
}

/*----------------------------------------------------------------*
 |                         GetTimeStamp                           |
 *----------------------------------------------------------------*/
Time GetTimestamp(Display *dsp)
{
  XEvent            event;
  XWindowAttributes attr;
  Atom   timeProp = XInternAtom(dsp, "_MOTIF_CURRENT_TIME", False);
  Window rwin     = DefaultRootWindow(dsp);

  XGetWindowAttributes(dsp, rwin, &attr);
  if (!(attr.your_event_mask & PropertyChangeMask))
    XSelectInput(dsp, rwin, attr.your_event_mask | PropertyChangeMask);

  XChangeProperty(dsp, rwin, timeProp, timeProp, 8, PropModeAppend, NULL, 0);
  XWindowEvent(dsp, rwin, PropertyChangeMask, &event);
  if (!(attr.your_event_mask & PropertyChangeMask))
    XSelectInput(dsp, rwin, attr.your_event_mask);

  return event.xproperty.time;
}

/*----------------------------------------------------------------*
 |                      CheckPinnedState                          |
 *----------------------------------------------------------------*/
static void CheckPinnedState(void)
{
  static int    panDx=0, panDy=-1;
  XtPointer     msg, fulldata;
  unsigned long size;
  Window        rWin, child;
  int           x, y;
  unsigned int  width, height, bWidth, depth;
  Time          time = GetTimestamp(pInfoList[LOCAL].display);

  if (pinnedState == VERIFIED)
    return;

  panDy = -panDy;

  pinnedState = VERIFYING;

  /* Get position of the top-level shell */
  XGetGeometry(pInfoList[LOCAL].display, XtWindow(pInfoList[LOCAL].shell),
	       &rWin, &x, &y, &width, &height, &bWidth, &depth);

  XTranslateCoordinates(pInfoList[LOCAL].display, XtWindow(pInfoList[LOCAL].shell),
			rWin, x, y, &origX, &origY, &child);

  /* panDx + panDy + config */
  size  = sizeof(CARD32) + sizeof(CARD32) + sizeof(CARD8);
  msg = fulldata = (XtPointer)XtMalloc(sizeof(CARD8) * size);

  msg = PackCARD32(msg, panDx);
  msg = PackCARD32(msg, panDy);
  (void)PackCARD8(msg, True);

  if (! XmeNamedSink(pInfoList[LOCAL].utmShell,
		     pInfoList[LOCAL].WM,
		     XmCOPY,
		     (XtPointer)fulldata,
		     time)
      )
    printf("Error - UTM Transfer failed.\n");
}

/*----------------------------------------------------------------*
 |                      ShowPinStateWarning                       |
 *----------------------------------------------------------------*/
static void ShowPinStateWarning(void)
{
	static Widget dlog = NULL;

	if (!dlog) {
		dlog = XmCreateWarningDialog(pInfoList[LOCAL].shell, "warningDlog", NULL, 0);
		XtUnmanageChild(XtNameToWidget(dlog, "Help"));
		XtUnmanageChild(XtNameToWidget(dlog, "Cancel"));
	}

	XtManageChild(dlog);
}

/*----------------------------------------------------------------*
 |                       HandleInitialExpose                      |
 *----------------------------------------------------------------*/
static void
HandleInitialExpose (
     Widget w,
     XtPointer clientData,
     XEvent *event,
     Boolean *cont)
{
  (void)clientData;
  (void)event;
  (void)cont;
  XtRemoveEventHandler(w, ExposureMask, False, HandleInitialExpose, NULL);
  CheckPinnedState();
}

