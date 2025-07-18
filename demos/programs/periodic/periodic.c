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
#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$XConsortium: periodic.c /main/8 1996/04/22 23:28:50 pascale $"
#endif
#endif

/******************************************************************************
 * periodic.c
 *
 * Copy and rename the file periodic.ad to Periodic in your home directory
 * or app-defaults directory, or merge it with your .Xdefaults file.
 *
 * It provides useful default values for Periodic fonts and colors
 *
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <Xm/Xm.h>
#include <Xm/ComboBox.h>
#include <Xm/RowColumn.h>
#include <Xm/Scale.h>
#include <Xm/ScrolledW.h>
#include <Xm/ToggleB.h>
#include <Mrm/MrmPublic.h>

typedef struct _DrawData {
	GC gc;
	Position drawX;
	Position drawY;
	Dimension drawWidth;
	Dimension drawHeight;
} DrawData;


static GC GetGC(
	Widget w );
static void ConfigureDrawData(
	Widget w,
	DrawData *data );
static void DrawButton(
	Widget w );
static void DrawArea(
	Widget w );
static void PopupHandler(
        Widget w,
        Widget pw,
        XEvent *event,
        Boolean *ctd );

static void ManageCb(
        Widget w,
        XtPointer cd,
        XmContainerSelectCallbackStruct *cb );
static void UnmanageCb(
        Widget w,
        String id,
        XtPointer cb );
static void InitPopupCb(
        Widget w,
        String id,
        XtPointer cb );
static void PopdownCb(
        Widget w,
        XtPointer cd,
        XtPointer cb );
static void DaExposeCb(
        Widget w,
        XtPointer cd,
        XtPointer cb );
static void DaResizeCb(
        Widget w,
        XtPointer cd,
        XtPointer cb );
static void DbExposeCb(
        Widget w,
        XtPointer cd,
        XtPointer cb );
static void DbResizeCb(
        Widget w,
        XtPointer cd,
        XtPointer cb );
static void ScaleCb(
        Widget w,
        XtPointer cd,
        XtPointer cb );
static void SetScaleCb(
        Widget w,
        int *value,
        XmToggleButtonCallbackStruct *cb );
static void ViewCb(
        Widget w,
        XtPointer cd,
        XmToggleButtonCallbackStruct *cb );
static void LayoutCb(
        Widget w,
        XtPointer cd,
        XmToggleButtonCallbackStruct *cb );
static void ToggleLightsCb(
        Widget w,
        XtPointer cd,
        XmToggleButtonCallbackStruct *cb );
static void ShowCb(
        Widget w,
        String id,
        XtPointer cb );
static void ExitCb(
        Widget w,
        XtPointer cd,
        XtPointer cb );
static void ScrollVisibleCb(
        Widget w,
        XtPointer cd,
        XmTraverseObscuredCallbackStruct *cb );
static void ToggleControlCb(
	Widget w,
        XtPointer client_data,
	XmComboBoxCallbackStruct *cb );
static void ToggleValueChangedCb(
	Widget w,
	XtPointer client_data,
        XmToggleButtonCallbackStruct *cb );

static MrmHierarchy mrmId;
static char *mrmFile[]={"periodic.uid"};
static MrmCode mrmClass;
static MRMRegisterArg mrmNames[] = {
        {"InitPopupCb", (XtPointer)InitPopupCb },
        {"PopdownCb", (XtPointer)PopdownCb },
        {"UnmanageCb", (XtPointer)UnmanageCb },
        {"ManageCb", (XtPointer)ManageCb },
        {"DaExposeCb", (XtPointer)DaExposeCb },
        {"DaResizeCb", (XtPointer)DaResizeCb },
        {"DbExposeCb", (XtPointer)DbExposeCb },
        {"DbResizeCb", (XtPointer)DbResizeCb },
        {"ScaleCb", (XtPointer)ScaleCb },
        {"SetScaleCb", (XtPointer)SetScaleCb },
        {"ViewCb", (XtPointer)ViewCb },
        {"LayoutCb", (XtPointer)LayoutCb },
        {"ToggleLightsCb", (XtPointer)ToggleLightsCb },
        {"ShowCb", (XtPointer)ShowCb },
        {"ExitCb", (XtPointer)ExitCb },
	{"ScrollVisibleCb", (XtPointer)ScrollVisibleCb },
        {"ToggleValueChangedCb", (XtPointer)ToggleValueChangedCb },
        {"ToggleControlCb", (XtPointer)ToggleControlCb }
};

static String fallbackResources[] = {
"*XmText.columns:                         10",
"*XmTextField.columns:                    10",
"*XmComboBox*columns:                      8",
"*scaleFrame*XmScale.width:               50",
"*scrollFrame*XmScrollBar.width:          50",
"*toggleButtonControls*Text.marginHeight:  1",
"*toggleButtonControls.spacing:            0",
"?.toolTipEnable: True",
"?.toolTipPostDelay: 2000",
"?.toolTipPostDuration: 5000",
"?.TipShell.TipLabel.background: yellow",
"*pushButton.toolTipString: A Tool Tip for all children of XmGadget and XmPrimitive",

"*fontList:                      *-*-*-medium-r-*-*-*-100-*-*-*-*-*-*",
"*HeaderDA*fontList:             *-*-*-bold-r-*-*-*-100-*-*-*-*-*-*",
"*titleLabel.fontList:           *-*-*-bold-r-*-*-*-180-*-*-*-*-*-*",
"*subtitleLabel.fontList:        *-*-*-bold-r-*-*-*-140-*-*-*-*-*-*",
"*labelLabel.fontList:           *-*-*-bold-r-*-*-*-180-*-*-*-*-*-*",
"*menuBar*fontList:              *-*-*-medium-r-*-*-*-140-*-*-*-*-*-*",
"*popupMenu*fontList:            *-*-*-medium-r-*-*-*-140-*-*-*-*-*-*",
"*XmMessageBox*fontList:         *-*-*-medium-r-*-*-*-140-*-*-*-*-*-*",
"*fileDialog*fontList:           *-*-*-medium-r-*-*-*-140-*-*-*-*-*-*",
"*selectDialog*fontList:         *-*-*-medium-r-*-*-*-140-*-*-*-*-*-*",
"*promptDialog*fontList:         *-*-*-medium-r-*-*-*-140-*-*-*-*-*-*",
"*toggleButtonExample*fontList:  *-*-*-medium-r-*-*-*-140-*-*-*-*-*-*",
"*toggleButtonControls*fontList: *-*-*-medium-r-*-*-*-100-*-*-*-*-*-*",
"*toggleButtonPage*fontList:     *-*-*-medium-r-*-*-*-100-*-*-*-*-*-*",
NULL
};

#define APP_NAME	"periodic"
#define APP_CLASS	"XmdPeriodic"

static XtAppContext  appContext;
static Widget shell;
static unsigned setting_toggle = 0;

int
main(int argc, char *argv[])
{
    Widget appMain;

    XtSetLanguageProc(NULL, NULL, NULL);
    MrmInitialize();
    shell = XtVaOpenApplication( &appContext,
                                 APP_CLASS,
                                 NULL,
                                 0,
                                 &argc,
                                 argv,
                                 fallbackResources,
                                 sessionShellWidgetClass,
                                 NULL);

    if (MrmOpenHierarchy (1, mrmFile, NULL, &mrmId) != MrmSUCCESS) exit(0);
    MrmRegisterNames(mrmNames, XtNumber(mrmNames));
    MrmFetchWidget(mrmId, "appMain", shell, &appMain, &mrmClass);
    XtManageChild(appMain);
    XtRealizeWidget(shell);
    XtAppMainLoop(appContext);
    return 0;    /* make compiler happy */
}

static void
ExitCb(
    Widget w,
    XtPointer cd,
    XtPointer cb )
{
    exit(0);
}

/*****************************************************************
 *
 * Display selected Dialog widget
 *
 *****************************************************************/

static void
ManageCb(
    Widget w,
    XtPointer cd,
    XmContainerSelectCallbackStruct *cb )
{
  static Widget managedDialog = NULL;

  int i;

  for (i = 0; i < cb->selected_item_count; i++)
    {
      Widget dialog = NULL;
      char *name = XtName(cb->selected_items[i]);

      if ((managedDialog != NULL) &&
	  XtIsManaged(managedDialog))
	{
	  XtUnmanageChild(managedDialog);
	  managedDialog = NULL;
	}

      if ((strlen(name) > 4) &&
	  strcmp(name + strlen(name) - 4, "Pick") == 0)
	{
	  char buf[64];

	  buf[0] = '*';
	  strcpy(buf + 1, name);
	  strcpy(buf + strlen(buf) - 4, "Dialog");
	  dialog = XtNameToWidget(shell, buf);

	  if (dialog)
	    {
	      managedDialog = dialog;
	      XtManageChild(dialog);
	    }
	}
    }
}

static void
UnmanageCb(
    Widget w,
    String id,
    XtPointer cb )
{
    XtUnmanageChild (XtNameToWidget (shell, id));
}

static void
ShowCb(
    Widget w,
    String id,
    XtPointer cb )
{
    static Widget tb = NULL;
    static Widget sc = NULL;
    int value;

    if (tb == NULL) tb = XtNameToWidget (shell, "*toggleButton");
    if (sc == NULL) sc = XtNameToWidget (shell, "*valueScale");

    XmScaleGetValue (sc, &value);
    if (XmToggleButtonGetState(tb) == True && value == 1020)
	XtManageChild (XtNameToWidget (shell, id));
}


/*****************************************************************
 *
 * Provide RadioBox behavior inside a PulldownMenu
 *
 *****************************************************************/

static void
ViewCb(
    Widget w,
    XtPointer cd,
    XmToggleButtonCallbackStruct *cb )
{
    static Widget viewToggle = NULL;

    if (cb->set) {
	if (viewToggle) XmToggleButtonSetState (viewToggle, False, False);
	viewToggle = w;
    }
    else {
	if (w == viewToggle) XmToggleButtonSetState (w, True, False);
    }
}

static void
LayoutCb(
    Widget w,
    XtPointer cd,
    XmToggleButtonCallbackStruct *cb )
{
    static Widget layoutToggle = NULL;

    if (cb->set) {
	if (layoutToggle) XmToggleButtonSetState (layoutToggle, False, False);
	layoutToggle = w;
    }
    else {
	if (w == layoutToggle) XmToggleButtonSetState (w, True, False);
    }
}


/*****************************************************************
 *
 * PopupMenu support
 *
 *****************************************************************/

static Time popupLastEventTime = 0;

static void
InitPopupCb(
    Widget w,
    String id,
    XtPointer cb )
{
    Widget popupWindow = XtNameToWidget (shell, id);

    XtAddEventHandler (popupWindow, ButtonPressMask, False,
			(XtEventHandler)PopupHandler, (XtPointer)w);
}

static void
PopupHandler (
    Widget w,
    Widget pw,
    XEvent *event,
    Boolean *ctd )
{
    if (((XButtonEvent *)event)->button != Button3) return;
    if (((XButtonEvent *)event)->time <= popupLastEventTime) return;

    XmMenuPosition((Widget) pw, (XButtonEvent *)event);
    XtManageChild ((Widget) pw);
}

/* By default, cancelling a popup menu with Button 3 will cause the
 * popup to be reposted at the location of the cancelling click.
 *
 * To switch off this behavior, remember when the menu was popped down.
 * In PopupHandler, don't repost the menu if the posting click just
 * cancelled a popup menu.
 */
static void
PopdownCb(
    Widget w,
    XtPointer cd,
    XtPointer cb )
{
    popupLastEventTime = XtLastTimestampProcessed (XtDisplayOfObject(w));
}


/*****************************************************************
 *
 * Draw utilities
 *
 *****************************************************************/

static DrawData *drawData = NULL;
static DrawData *buttonData = NULL;

static GC
GetGC(
    Widget w )
{
    Arg args[2];
    XGCValues gcv;
    Pixel fg;
    Pixel bg;
    GC gc;

    XtSetArg (args[0], XmNforeground, &fg);
    XtSetArg (args[1], XmNbackground, &bg);
    XtGetValues (w, args, 2);
    gcv.foreground = fg;
    gcv.background = bg;
    gcv.line_width = 1;
    gc = XtGetGC (w, GCForeground | GCBackground | GCLineWidth, &gcv);

    return (gc);
}

static void
ConfigureDrawData(
    Widget w,
    DrawData *data )
{
    Arg args[6];
    Dimension width, height, st, ht, mw, mh;
    Dimension totalMarginWidth;
    Dimension totalMarginHeight;

    width = height = st = ht = mw = mh = 0;
    XtSetArg (args[0], XmNwidth, &width);
    XtSetArg (args[1], XmNheight, &height);
    XtSetArg (args[2], XmNshadowThickness, &st);
    XtSetArg (args[3], XmNhighlightThickness, &ht);
    XtSetArg (args[4], XmNmarginWidth, &mw);
    XtSetArg (args[5], XmNmarginHeight, &mh);
    XtGetValues (w, args, 6);

    totalMarginWidth = st + ht + mw;
    totalMarginHeight = st + ht + mh;

    if (2 * totalMarginWidth < width && 2 * totalMarginHeight < height) {
	data->drawX = totalMarginWidth;
	data->drawY = totalMarginHeight;
	data->drawWidth = width - 2 * totalMarginWidth;
	data->drawHeight = height - 2 * totalMarginHeight;
    }
    else {
	data->drawWidth = 0;
	data->drawHeight = 0;
    }
}

/*****************************************************************
 *
 * DrawingArea display code
 *
 *****************************************************************/

static void
DaResizeCb(
    Widget w,
    XtPointer cd,
    XtPointer cb )
{
    if (drawData == NULL) return;

    ConfigureDrawData (w, drawData);
    XClearWindow (XtDisplayOfObject(w), XtWindowOfObject(w));
    DrawArea (w);
}

static void
DaExposeCb(
    Widget w,
    XtPointer cd,
    XtPointer cb )
{
    if (drawData == NULL) {
	drawData = (DrawData *)XtMalloc (sizeof(DrawData));
	drawData->gc = GetGC (w);
	ConfigureDrawData (w, drawData);
    }
    DrawArea(w);
}

#define NPOINTS 40

static void
DrawArea(
    Widget w )
{
    int i, x, y, m;
    XPoint p[NPOINTS];

    if (drawData->drawWidth == 0) return;

    XClearArea (XtDisplayOfObject(w), XtWindowOfObject(w),
		drawData->drawX, drawData->drawY,
		drawData->drawWidth, drawData->drawHeight,
		False);
    XDrawRectangle (XtDisplayOfObject(w), XtWindowOfObject(w), drawData->gc,
		drawData->drawX, drawData->drawY,
		drawData->drawWidth, drawData->drawHeight);
    XDrawLine (XtDisplayOfObject(w), XtWindowOfObject(w), drawData->gc,
		drawData->drawX, drawData->drawY + drawData->drawHeight/2,
		drawData->drawX + drawData->drawWidth,
		drawData->drawY + drawData->drawHeight/2);

    m = 20 * drawData->drawHeight / 100;
    p[0].x = drawData->drawX;
    p[0].y = drawData->drawY + drawData->drawHeight/2;
    for (i = 1; i < NPOINTS-1; i++) {
	p[i].x = drawData->drawX + (i * drawData->drawWidth)/NPOINTS;
	p[i].y = drawData->drawY + m/2 + (rand() % (drawData->drawHeight - m));
    }
    p[NPOINTS-1].x = drawData->drawX + drawData->drawWidth;
    p[NPOINTS-1].y = drawData->drawY + drawData->drawHeight/2;

    XDrawLines (XtDisplayOfObject(w), XtWindowOfObject(w), drawData->gc,
		p, NPOINTS, CoordModeOrigin);
}

static void
ScaleCb(
    Widget w,
    XtPointer cd,
    XtPointer cb )
{
    static Widget da = NULL;

    if (drawData == NULL) return;

    if (da == NULL) da = XtNameToWidget (shell, "*drawArea");

    DrawArea (da);
}

static void
SetScaleCb(
    Widget w,
    int *value,
    XmToggleButtonCallbackStruct *cb )
{
    static Widget da = NULL;
    static Widget sc = NULL;

    if (drawData == NULL) return;

    /* CR 9647: ignore calls that unset the toggle. */
    if (! cb->set) return;

    if (da == NULL) da = XtNameToWidget (shell, "*drawArea");
    if (sc == NULL) sc = XtNameToWidget (shell, "*valueScale");

    XmScaleSetValue (sc, *value);

    DrawArea (da);
}

/*****************************************************************
 *
 * DrawnButton display code
 *
 *****************************************************************/

static Boolean lightsOn = False;

static void
DbResizeCb(
    Widget w,
    XtPointer cd,
    XtPointer cb )
{
    if (buttonData == NULL) return;

    ConfigureDrawData (w, buttonData);
    XClearArea (XtDisplayOfObject(w), XtWindowOfObject(w),
		buttonData->drawX, buttonData->drawY,
		buttonData->drawWidth, buttonData->drawHeight,
		False);
    DrawButton (w);
}

static void
DbExposeCb(
    Widget w,
    XtPointer cd,
    XtPointer cb )
{
    if (buttonData == NULL) {
	buttonData = (DrawData *)XtMalloc (sizeof(DrawData));
	buttonData->gc = GetGC (w);
	ConfigureDrawData (w, buttonData);
    }
    DrawButton(w);
}

#define NARCS 6

static void
DrawButton(
    Widget w )
{
    int i, x, y, incX, incY;
    XArc a[NARCS];

    if (buttonData->drawWidth == 0 || !lightsOn) return;

    a[0].x = buttonData->drawX + (buttonData->drawWidth - 1)/2;
    a[0].y = buttonData->drawY + (buttonData->drawHeight - 1)/2;
    a[0].width = 1;
    a[0].height = 1;
    a[0].angle1 = 0;
    a[0].angle2 = 360*64;
    incX = (buttonData->drawWidth - 1)/(2 * NARCS);
    incY = (buttonData->drawHeight - 1)/(2 * NARCS);

    for (i = 1; i < NARCS; i++) {
	a[i].x = a[i-1].x - incX;
	a[i].y = a[i-1].y - incY;
	a[i].width = a[i-1].width + 2 * incX;
	a[i].height = a[i-1].height + 2 * incY;
#ifndef BROKEN_SERVER_ARCS
	a[i].angle1 = 0;
	a[i].angle2 = 360 * 64;
#else
	XDrawRectangle (XtDisplayOfObject(w), XtWindowOfObject(w), buttonData->gc,
			a[i].x, a[i].y, a[i].width, a[i].height);
#endif
    }

#ifndef BROKEN_SERVER_ARCS
    XDrawArcs (XtDisplayOfObject(w), XtWindowOfObject(w), buttonData->gc, a, NARCS);
#endif
}

static void
ToggleLightsCb(
    Widget w,
    XtPointer cd,
    XmToggleButtonCallbackStruct *cb )
{
    static Widget db = NULL;

    if (buttonData == NULL) return;

    if (db == NULL) db = XtNameToWidget (shell, "*drawnButton");

    lightsOn = cb->set;

    if (lightsOn)
	DrawButton (db);
    else
	XClearArea (XtDisplayOfObject(db), XtWindowOfObject(db),
		buttonData->drawX, buttonData->drawY,
		buttonData->drawWidth, buttonData->drawHeight,
		False);
}


/*****************************************************************
 *
 * TraverseObscured callback
 *
 *****************************************************************/

static void
ScrollVisibleCb(
    Widget w,
    XtPointer cd,
    XmTraverseObscuredCallbackStruct *cb )
{
  Widget target = cb->traversal_destination;
  Widget parent = XtParent(target);

  XmScrollVisible(w, (XmIsComboBox(parent) ? parent : target), 0, 0);
}


/*****************************************************************
 *
 * Toggle value changed callback
 *
 *****************************************************************/

static void
ToggleValueChangedCb(Widget w,
		     XtPointer client_data,
		     XmToggleButtonCallbackStruct *cb)
{
  if (! setting_toggle++)
    {
      Widget toggle_set = XtNameToWidget(XtParent(w), "*toggleSetControl");
      XtVaSetValues(toggle_set, XmNselectedPosition, cb->set + 1, NULL);
    }
  --setting_toggle;
}


/*****************************************************************
 *
 * Toggle controls callback
 *
 *****************************************************************/

/* Resource lists for ToggleControlCb(). */
static unsigned char toggle_ind_on[] = {
  XmINDICATOR_NONE, XmINDICATOR_FILL, XmINDICATOR_BOX,
  XmINDICATOR_CHECK, XmINDICATOR_CHECK_BOX,
  XmINDICATOR_CROSS, XmINDICATOR_CROSS_BOX
};
static unsigned char toggle_ind_type[] = {
  XmN_OF_MANY, XmONE_OF_MANY, XmONE_OF_MANY_ROUND, XmONE_OF_MANY_DIAMOND
};
static Dimension toggle_ind_size[] = {
  10, 15, 20, 25, 30
};
static unsigned char toggle_toggle_mode[] = {
  XmTOGGLE_BOOLEAN, XmTOGGLE_INDETERMINATE
};
static unsigned char toggle_set[] = {
  XmUNSET, XmSET, XmINDETERMINATE
};
static Pixel toggle_select_color[] = {
  XmDEFAULT_SELECT_COLOR, XmREVERSED_GROUND_COLORS, XmHIGHLIGHT_COLOR
};
static Pixel toggle_unselect_color[] = {
  XmUNSPECIFIED_PIXEL		/* initialized dynamically */
};
static Boolean toggle_boolean[] = {
  True, False
};

#define RESET_VALUE(w, list, value)				\
  {								\
    int pos;							\
    for (pos = 0; pos < XtNumber(list); pos++)			\
      if (list[pos] == value)					\
	{							\
	  XtVaSetValues(w, XmNselectedPosition, pos + 1, NULL);	\
	  break;						\
	}							\
  }

static Boolean
get_color(Widget widget,
	  XmString name,
	  char *res_type,
	  Pixel *value)
{
  Boolean result = False;
  char *text;

  if ((text = XmStringUnparse(name, NULL, XmCHARSET_TEXT, XmCHARSET_TEXT, NULL, 0, XmOUTPUT_ALL))) {
      XrmValue from, to;

      from.size = sizeof(char*);
      from.addr = (XPointer)text;
      to.size = sizeof(Pixel);
      to.addr = (XPointer)value;

      result = XtConvertAndStore(widget, XmRString, &from, res_type, &to);
      XtFree(text);
    }

  return result;
}

static void
ToggleControlCb(Widget w,
		XtPointer client_data,
		XmComboBoxCallbackStruct *cb)
{
  static Widget tw = NULL;
  static Widget ind_type_wid, tog_mode_wid, set_wid;

  int item = cb->item_position - 1;
  int control = *(int *)client_data;
  unsigned char uchar_value;

  if (tw == NULL)
    {
      tw = XtNameToWidget(shell, "*toggleButtonExample");
      ind_type_wid = XtNameToWidget(XtParent(tw), "*toggleIndTypeControl");
      tog_mode_wid = XtNameToWidget(XtParent(tw), "*toggleTogModeControl");
      set_wid      = XtNameToWidget(XtParent(tw), "*toggleSetControl");

      XtVaGetValues(tw, XmNbackground, &toggle_unselect_color[0], NULL);
    }

  if (! setting_toggle++)
    {
      switch (control)
	{
	case 1:
	  XtVaSetValues(tw, XmNindicatorOn, toggle_ind_on[item], NULL);
	  break;
	case 2:
	  XtVaSetValues(tw, XmNindicatorType, toggle_ind_type[item], NULL);
	  XtVaGetValues(tw, XmNindicatorType, &uchar_value, NULL);
	  if (uchar_value != toggle_ind_type[item])
	    RESET_VALUE(ind_type_wid, toggle_ind_type, uchar_value);
	  if (uchar_value != XmN_OF_MANY)
	    {
	      unsigned char toggle_mode, set;
	      XtVaGetValues(tw, XmNtoggleMode, &toggle_mode,
			    XmNset, &set, NULL);
	      RESET_VALUE(tog_mode_wid, toggle_toggle_mode, toggle_mode);
	      RESET_VALUE(set_wid, toggle_set, set);
	    }
	  break;
	case 3:
	  XtVaSetValues(tw, XmNindicatorSize, toggle_ind_size[item], NULL);
	  break;
	case 4:
	  XtVaSetValues(tw, XmNtoggleMode, toggle_toggle_mode[item], NULL);
	  XtVaGetValues(tw, XmNtoggleMode, &uchar_value, NULL);
	  if (uchar_value != toggle_toggle_mode[item])
	    RESET_VALUE(tog_mode_wid, toggle_toggle_mode, uchar_value);
	  if (uchar_value != XmTOGGLE_INDETERMINATE)
	    {
	      XtVaGetValues(tw, XmNset, &uchar_value, NULL);
	      RESET_VALUE(set_wid, toggle_set, uchar_value);
	    }
	  break;
	case 5:
	  XtVaSetValues(tw, XmNset, toggle_set[item], NULL);
	  XtVaGetValues(tw, XmNset, &uchar_value, NULL);
	  if (uchar_value != toggle_set[item])
	    RESET_VALUE(set_wid, toggle_set, uchar_value);
	  break;
	case 6:
	  if (item >= 0)
	    XtVaSetValues(tw, XmNselectColor, toggle_select_color[item], NULL);
	  else
	    {
	      Pixel color;
	      if (get_color(tw, cb->item_or_text, XmRSelectColor, &color))
		XtVaSetValues(tw, XmNselectColor, color, NULL);
	      else
		XBell(XtDisplayOfObject(tw), 0);
	    }
	  break;
	case 7:
	  XtVaSetValues(tw, XmNfillOnSelect, toggle_boolean[item], NULL);
	  break;
	case 8:
	  XtVaSetValues(tw, XmNvisibleWhenOff, toggle_boolean[item], NULL);
	  break;
	case 9:
	  if (item >= 0)
	    XtVaSetValues(tw, XmNunselectColor, toggle_unselect_color[item],
			  NULL);
	  else
	    {
	      Pixel color;
	      if (get_color(tw, cb->item_or_text, XmRPixel, &color))
		XtVaSetValues(tw, XmNunselectColor, color, NULL);
	      else
		XBell(XtDisplayOfObject(tw), 0);
	    }
	  break;
	}
    }
  --setting_toggle;
}
