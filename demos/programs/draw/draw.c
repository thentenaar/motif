/* $XConsortium: draw.c /main/5 1996/06/13 10:32:01 drk $ */
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
 * 
 */
/*
 * HISTORY
 */
/*
**  This demo shows an AUTOMATIC scrolled window and some graphics
**  done in a DrawingArea.
**
**  It lets you draw polylines and buttons, resizes the picture,
**  and eventually save the data in an X resource format.
**
*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <Xm/XmAll.h>
#include <Xmd/Help.h>   
#include <X11/IntrinsicP.h>

/*-------------------------------------------------------------
**	Graphic data structure and API
*/

#define APP_CLASS "XmdDraw"

/* very simple data struct, static size, etc...
*/
#define MAX_POINT 100
typedef struct { 
    XPoint points[MAX_POINT] ; 
    Cardinal num_points ;
#define POLYLINE 1
    unsigned char type ;
} GraphicUnit ;

#define MAX_GRAPH 100
typedef struct { 
     GraphicUnit graphics[MAX_GRAPH] ;
     Cardinal num_graphics ;

     XPoint anchor_point ;
     XPoint drag_point ;
     GC drag_gc ;
     Boolean in_drag ;
     Dimension old_width, old_height ;

     Widget work_area ;
     Widget textf ;
} Graphic ;

static char drawTranslations[] = "#replace\n\
~s ~m ~a <Key>Return:DrawingAreaInput() ManagerParentActivate()\n\
<Key>Return:DrawingAreaInput() ManagerGadgetSelect()\n\
<Key>osfActivate:DrawingAreaInput() ManagerParentActivate()\n\
<Key>osfCancel:DrawingAreaInput() ManagerParentCancel()\n\
<Key>osfHelp:DrawingAreaInput() ManagerGadgetHelp()\n\
<Key>space:DrawingAreaInput() ManagerGadgetSelect()\n\
<Key>osfSelect:DrawingAreaInput() ManagerGadgetSelect()\n\
<KeyDown>:DrawingAreaInput() ManagerGadgetKeyInput()\n\
<KeyUp>:DrawingAreaInput()\n\
<BtnMotion>:DrawingAreaInput() ManagerGadgetButtonMotion()\n\
<Motion>:DrawingAreaInput() ManagerGadgetButtonMotion()\n\
<Btn1Down>:DrawingAreaInput() ManagerGadgetArm()\n\
<Btn1Up>:DrawingAreaInput() ManagerGadgetActivate()\n\
<Btn2Down>:DrawingAreaInput() ManagerGadgetDrag()\n\
<BtnDown>:DrawingAreaInput()\n\
<BtnUp>:DrawingAreaInput()";

/*-------------------------------------------------------------
**	 Application data stuff: this serve as the file saving format
*/


typedef struct {
  char * lines;
  char * towns;
} ApplicationData, * ApplicationDataPtr ;

#define XtNlines "lines"
#define XtCLines "Lines"
#define XtNtowns "towns"
#define XtCTowns "Towns"

/* if this stuff was as easily initializable, I would put it local to main */
static XtResource app_resources[] = {
  { XtNlines, XtCLines, XmRString, sizeof(String),
    XtOffset(ApplicationDataPtr, lines), XmRString, "" },
  { XtNtowns, XtCTowns, XmRString, sizeof(String),
    XtOffset(ApplicationDataPtr, towns), XmRString, "" },
};

/* if this stuff was as easily initializable, I would put it local to main */
/* better to have that in automatic mode */
static String fallback_resources[] = {
    "*help_manager.helpFile: draw",
    "*work_area.width:1000",
    "*work_area.height:1000",
    "*XmScale.minimum: 100",
    "*XmScale.maximum: 16000",
    ".geometry:600x500",
    NULL
    } ;

/*-------------------------------------------------------------
**	forwarded functions
*/
static void CreateApplication (Widget parent, Graphic *graph);

/*      Xt callbacks 
*/
static void SaveCB (Widget w, XtPointer client_data, XtPointer call_data);
static void OK_WriteCB (Widget w, XtPointer client_data, XtPointer call_data);
static void QuitCB (Widget w, XtPointer client_data, XtPointer call_data);
static void HelpCB (Widget w, XtPointer client_data, XtPointer call_data);
static void DrawCB (Widget w, XtPointer client_data, XtPointer call_data);
static void ValueCB (Widget w, XtPointer client_data, XtPointer call_data);
static void TravCB (Widget w, XtPointer client_data, XtPointer call_data);
static void PushCB (Widget w, XtPointer client_data, XtPointer call_data);

static void InitDraw (Graphic *graph, ApplicationData *app_data);
static void ReDraw (Graphic *graph, XEvent *event);
static void ReSize (Graphic *graph, Dimension widget, Dimension height);
static void StartUnit (Graphic *graph, Position x, Position y);
static void DragUnit (Graphic *graph, Position x, Position y);
static Boolean NearPoint (XPoint point, Position x, Position y);
static void EndUnit (Graphic *graph, Position x, Position y);
static void DeleteUnit (Graphic *graph, Position x, Position y);


/*-------------------------------------------------------------
**	    Main body
*/
int
main(int argc, char	*argv[])
{
    XtAppContext app_context;
    Widget      toplevel;
    ApplicationData app_data;
    Graphic legraph;

    toplevel = XtAppInitialize(&app_context, APP_CLASS, NULL, 0,
			       &argc, argv, fallback_resources, NULL, 0);

    XtGetApplicationResources(toplevel, (XtPointer)&app_data,
			      app_resources, XtNumber(app_resources),
			      NULL, 0);

    CreateApplication (toplevel, &legraph);

    InitDraw(&legraph, &app_data);
    
    XtRealizeWidget(toplevel);
    XtAppMainLoop(app_context);

    /*NOTREACHED*/
    exit(0);
}


/*-------------------------------------------------------------
**	Create a Main Window with a menubar, a command panel containing
**      2 scales and a textfied, and a workarea.
**      Also put in the graphic structure the workarea info and the
**      textfield ids.
*/
static void 
CreateApplication (Widget parent, Graphic *	graph)
{
    Widget main_window, menu_bar, menu_pane, cascade, button, comw, scale;
    Arg args[5];
    int	n;
    XtTranslations xlations;

    /*	Create automatic MainWindow.
     */
    n = 0;
    XtSetArg (args[n], XmNscrollingPolicy, XmAUTOMATIC);  n++;
    main_window = XmCreateMainWindow (parent, "main_window", args, n);

    XtAddCallback (main_window, XmNtraverseObscuredCallback, TravCB, 
		   (XtPointer)graph);

    XtManageChild (main_window);


    /*	Create MenuBar in MainWindow.
     */
    n = 0;
    menu_bar = XmCreateMenuBar (main_window, "menu_bar", args, n); 
    XtManageChild (menu_bar);


    /*	Create "File" PulldownMenu with Save and Quit buttons
     */
    n = 0;
    menu_pane = XmCreatePulldownMenu (menu_bar, "menu_pane", args, n);

    n = 0;
    button = XmCreatePushButton (menu_pane, "Save...", args, n);
    XtManageChild (button);
    /* pass the graphic id to the save function */
    XtAddCallback (button, XmNactivateCallback, SaveCB, (XtPointer)graph);
    n = 0;
    button = XmCreatePushButton (menu_pane, "Quit", args, n);
    XtManageChild (button);
    XtAddCallback (button, XmNactivateCallback, QuitCB, NULL);

    n = 0;
    XtSetArg (args[n], XmNsubMenuId, menu_pane);  n++;
    cascade = XmCreateCascadeButton (menu_bar, "File", args, n);
    XtManageChild (cascade);


    /*	Create "Help" PulldownMenu with Help button.
     */
    n = 0;
    menu_pane = XmCreatePulldownMenu (menu_bar, "menu_pane", args, n);

    n = 0;
    button = XmCreatePushButton (menu_pane, "Overview", args, n);
    XtManageChild (button);
    XtAddCallback (button, XmNactivateCallback, HelpCB, NULL);

    n = 0;
    XtSetArg (args[n], XmNsubMenuId, menu_pane);  n++;
    cascade = XmCreateCascadeButton (menu_bar, "Help", args, n);
    XtManageChild (cascade);

    n = 0;
    XtSetArg (args[n], XmNmenuHelpWidget, cascade);  n++;
    XtSetValues (menu_bar, args, n);


    /*	Create work_area in MainWindow 
     */
    n = 0;
    XtSetArg (args[n], XmNresizePolicy, XmRESIZE_NONE); n++ ;
    XtSetArg (args[n], XmNmarginWidth, 0); n++ ;
    XtSetArg (args[n], XmNmarginHeight, 0); n++ ;
    /* hardcode this one, since its is required for the motion handling */
    xlations = XtParseTranslationTable(drawTranslations);
    XtSetArg (args[n], XmNtranslations, xlations); n++;
    graph->work_area = XmCreateDrawingArea(main_window, "work_area", args, n);
    XtAddCallback (graph->work_area, XmNexposeCallback, DrawCB, 
		   (XtPointer)graph);
    XtAddCallback (graph->work_area, XmNresizeCallback, DrawCB, 
		   (XtPointer)graph);
    XtAddCallback (graph->work_area, XmNinputCallback, DrawCB, 
		   (XtPointer)graph);
    XtManageChild (graph->work_area);


    /*	Create a commandWindow in MainWindow with text and scales 
     */
    n = 0;
    comw = XmCreateRowColumn(main_window, "comw", args, n);
    XtManageChild (comw);
    n = 0;
    XtSetArg (args[n], XmNcommandWindow, comw);  n++;
    XtSetValues (main_window, args, n);

    /* find the initial size of the work_area and report to the scales */
    n = 0;
    XtSetArg (args[n], XmNwidth, &graph->old_width);  n++;
    XtSetArg (args[n], XmNheight, &graph->old_height);  n++;
    XtGetValues (graph->work_area, args, n);
	
    n = 0;
    XtSetArg (args[n], XmNorientation, XmHORIZONTAL);  n++;
    XtSetArg (args[n], XmNshowValue, True);  n++;
    XtSetArg (args[n], XmNvalue, graph->old_width);  n++;
    scale = XmCreateScale(comw, "scale_w", args, n); /* scale_w is the name */
    XtAddCallback (scale, XmNvalueChangedCallback, ValueCB, 
		   (XtPointer)graph->work_area);
    XtManageChild (scale);

    n = 0;
    XtSetArg (args[n], XmNorientation, XmHORIZONTAL);  n++;
    XtSetArg (args[n], XmNshowValue, True);  n++;
    XtSetArg (args[n], XmNvalue, graph->old_height);  n++;
    scale = XmCreateScale(comw, "scale_h", args, n);
    XtAddCallback (scale, XmNvalueChangedCallback, ValueCB, 
		   (XtPointer)graph->work_area);
    XtManageChild (scale);

    n = 0;
    graph->textf = XmCreateTextField(comw, "textf", args, n);
    XtManageChild (graph->textf);
    

    /*	Set MainWindow areas 
     */
    XmMainWindowSetAreas (main_window, menu_bar, comw, NULL, NULL,
			  graph->work_area);

}


/*-------------------------------------------------------------
**	PushCB		- callback for push button inside the workarea
*/
static void 
PushCB (Widget		w,		/*  widget id		*/
	XtPointer	client_data,	/*  data from application   */
	XtPointer	call_data)	/*  data from widget class  */
{
    XmPushButtonCallbackStruct * pbcs =
	(XmPushButtonCallbackStruct *) call_data ;

    if ((pbcs->event->type == ButtonRelease) &&
	(pbcs->event->xbutton.state & ShiftMask))
	XtDestroyWidget(w);
/*    else 
	printf("Activate %s\n",XtName(w));*/
}

/*-------------------------------------------------------------
**	TravCB		- callback for traverseObscure
*/
static void 
TravCB (Widget		w,		/*  widget id		*/
	XtPointer	client_data,	/*  data from application   */
	XtPointer	call_data)	/*  data from widget class  */
{
    XmTraverseObscuredCallbackStruct * tocs =
	(XmTraverseObscuredCallbackStruct *) call_data ;
    Graphic * graph = (Graphic *) client_data ;

    if (tocs->traversal_destination != graph->work_area)
	XmScrollVisible(w, tocs->traversal_destination, 20, 20) ; 
}

/*-------------------------------------------------------------
**	ValueCB		- callback for scales
*/
static void 
ValueCB (Widget		w,		/*  widget id		*/
	 XtPointer	client_data,	/*  data from application   */
	 XtPointer	call_data)	/*  data from widget class  */
{
    
    Arg args[5];	
    int	n ;		   
    int value ;
    Widget workarea = (Widget) client_data ;

    /* get the value outof the Scale */
    n = 0;
    XtSetArg (args[n], XmNvalue, &value);  n++;
    XtGetValues (w, args, n);

    n = 0;
    if (strcmp(XtName(w), "scale_w") == 0 ) { /* width scale */
	XtSetArg (args[n], XmNwidth, value);  n++;
    } else {
	XtSetArg (args[n], XmNheight, value);  n++;
    }
    XtSetValues (workarea, args, n);
}



/*-------------------------------------------------------------
**	SaveCB			- callback for Save button
*/
static void 
SaveCB (Widget		w,		/*  widget id		*/
	XtPointer	client_data,	/*  data from application   */
	XtPointer	call_data)	/*  data from widget class  */
{
	static Widget fsb_box = NULL ;

	if (!fsb_box) {
	    fsb_box = XmCreateFileSelectionDialog (w, "Save graphics", 
						   NULL, 0);
	    /* just propagate the graphic information */
	    XtAddCallback (fsb_box, XmNokCallback, OK_WriteCB, client_data);
	}    


	XtManageChild (fsb_box);
}


/*-------------------------------------------------------------
**	QuitCB			- callback for quit button
*/
static void 
QuitCB (Widget		w,		/*  widget id		*/
	XtPointer	client_data,	/*  data from applicaiton   */
	XtPointer	call_data)	/*  data from widget class  */
{
    exit (0);
}


/*-------------------------------------------------------------
**	HelpCB			- callback for help button
*/
static void 
HelpCB (Widget		w,		/*  widget id		*/
	XtPointer	client_data,	/*  data from application   */
	XtPointer	call_data)	/*  data from widget class  */
{
    static Widget help_widget = NULL ;

    if (!help_widget)
	help_widget = XmdCreateHelpDialog(w, "help_manager", NULL, 0);

    XtManageChild(help_widget);   
}

/*-------------------------------------------------------------
**	OK_WriteCB	- callback for saving the graphics in a file
*/
static void 
OK_WriteCB (Widget	w,		/*  widget id		*/
	    XtPointer	client_data,	/*  data from application   */
	    XtPointer	call_data)	/*  data from widget class  */
{
    Widget text ;
    char * filename ;
    FILE  *out_file ;
    Cardinal i,j ;
    Arg args[5];	
    int	n ;		   
    Widget * children ;
    Cardinal num_children ;
    Graphic * graph = (Graphic *) client_data ;
    Position x,y ;

    text = XmFileSelectionBoxGetChild(w, XmDIALOG_TEXT);
    filename = XmTextGetString(text);

    if ((out_file=fopen(filename, "w")) == NULL) {
	printf("Can't open file: %s ", filename);
	return ;
    }

    fprintf(out_file, "draw.lines: ");
    for (i=0; i < graph->num_graphics; i++) {
	for (j=0; j < graph->graphics[i].num_points; j++) {
	    fprintf(out_file, "%d_%d", graph->graphics[i].points[j].x,
		                       graph->graphics[i].points[j].y) ;
	    if (j == graph->graphics[i].num_points - 1) {
		if (i != graph->num_graphics - 1) fprintf(out_file, ". ") ;
	    } else fprintf(out_file, ", ") ;
	    
	}
    }

    fprintf(out_file, "\n");
    fprintf(out_file, "draw.towns: ");
    
    n = 0;
    XtSetArg (args[n], XmNnumChildren, &num_children);  n++;
    XtSetArg (args[n], XmNchildren, &children);  n++;
    XtGetValues (graph->work_area, args, n);

    for (i=0; i < num_children; i++) {
	fprintf(out_file, "%s", XtName(children[i]));
	if (i != num_children - 1) fprintf(out_file, ", ") ;
    }

    fprintf(out_file, "\n");

    for (i=0; i < num_children; i++) {
	n = 0;
	XtSetArg (args[n], XmNx, &x);  n++;
	XtSetArg (args[n], XmNy, &y);  n++;
	XtGetValues (children[i], args, n);
	fprintf(out_file, "draw*%s.x: %d\n",XtName(children[i]),x);
	fprintf(out_file, "draw*%s.y: %d\n",XtName(children[i]),y);
    }

    fclose(out_file);

}


 
 

/*-------------------------------------------------------------
**	DrawCB			- callback for drawing area
*/
static void 
DrawCB (Widget		w,		/*  widget id		*/
	XtPointer	client_data,	/*  data from application   */
	XtPointer	call_data)	/*  data from widget class  */
{
    
    XmDrawingAreaCallbackStruct * dacs =
	(XmDrawingAreaCallbackStruct *) call_data ;
    Arg args[5];	
    int	n ;	
    Dimension width, height ;
    String name ;
    Widget newpush ;
    XSetWindowAttributes xswa;
    Graphic * graph = (Graphic *) client_data ;

    static Boolean first_time = True ;

    switch (dacs->reason) {
    case XmCR_EXPOSE: 
	if (first_time) {
	    /* Change once the bit gravity of the Drawing Area; default
	       is north west and we want forget, so that resize 
	       always generates exposure events */
	    first_time = False ;
	    xswa.bit_gravity = ForgetGravity ;
	    XChangeWindowAttributes(XtDisplay(w), XtWindow(w),
				    CWBitGravity, &xswa);
	}
	ReDraw(graph, dacs->event) ;

	break ;
    case XmCR_RESIZE: 
	n = 0;
	XtSetArg (args[n], XmNwidth, &width);  n++;
	XtSetArg (args[n], XmNheight, &height);  n++;
	XtGetValues (w, args, n);
	ReSize(graph, width, height) ;

	break ;
    case XmCR_INPUT:
	if (dacs->event->type == ButtonPress) {
	    name = XmTextFieldGetString(graph->textf) ; /* textfield */
	    if (strcmp ("", name) != 0) {
		n = 0;
		XtSetArg (args[n], XmNx, dacs->event->xbutton.x);  n++;
		XtSetArg (args[n], XmNy, dacs->event->xbutton.y);  n++;
		newpush = XmCreatePushButton(w, name, args, n);
		XtAddCallback (newpush, XmNactivateCallback, PushCB, NULL);
		XtManageChild (newpush);
	    } else 
	    if ((dacs->event->xbutton.state & ShiftMask) && 
		(!graph->in_drag)) {
		DeleteUnit (graph, dacs->event->xbutton.x,
			    dacs->event->xbutton.y);
	    } else {
		if (!graph->in_drag) {
		    StartUnit(graph, dacs->event->xbutton.x,
			      dacs->event->xbutton.y);
		} else {
		    EndUnit(graph, dacs->event->xbutton.x,
			    dacs->event->xbutton.y);
		}
	    }
	    XtFree(name);
    	} else  /* need to get motion events here: app_default should
		   modified DrawingArea translation with both Motion
		   and BtnMotion addition */
	if (dacs->event->type == MotionNotify) {
	    /* this one just exits if in_drag is False */
	    DragUnit(graph, dacs->event->xbutton.x,
		     dacs->event->xbutton.y);
	}
	break ;
    }
}


/*************************** GRAPHICS **********************************/

static void 
InitDraw(Graphic * 	   graph,
	 ApplicationData * app_data)
{
    XGCValues val ;
    Arg args[5];	
    int	n ;		   
    Cardinal i ;
    Dimension width, height ;
    String pstr, wstr ;
    int x, y ;
    Widget newpush ;

    /* create the gc used for the rudder banding effect */
    n = 0;
    XtSetArg (args[n], XmNforeground, &val.foreground);  n++;
    XtSetArg (args[n], XmNbackground, &val.background);  n++;
    XtGetValues (graph->work_area, args, n);
    
    val.foreground = val.foreground ^ val.background ;
    val.function = GXxor ;
    graph->drag_gc = XtGetGC(graph->work_area, 
			     GCForeground | GCBackground | GCFunction, &val);

    /* initialize the graphic stuff */
    graph->in_drag = False ;
    
    graph->num_graphics = 0 ;
    for (i=0; i < MAX_GRAPH; i++) {
	graph->graphics[i].num_points = 0 ;
    }

    /* polylines syntax: 
         draw.lines: 10_10, 20_30, 28_139. 11_112, 145_60. 211_112, 45_60
       would be nice to use nested strtok but the sucker is not reentrant...
    */
    wstr = XtNewString(app_data->lines)  ;
    for(pstr = (char *) strtok(wstr, ".,") ; pstr ;	
	pstr = (char *) strtok( NULL, ".,")) {
	while (*pstr && isspace(*pstr)) pstr++;
	if (*pstr == '\0') break;
	
	sscanf(pstr, "%d_%d", &x, &y);
	graph->graphics[graph->num_graphics].points
	    [graph->graphics[graph->num_graphics].num_points].x = x ;
	graph->graphics[graph->num_graphics].points
	    [graph->graphics[graph->num_graphics].num_points].y = y ;
	graph->graphics[graph->num_graphics].num_points ++ ;
	graph->graphics[graph->num_graphics].type = POLYLINE ;

	/* look in the original to see if it is a new unit */
	if (app_data->lines[pstr - wstr + strlen(pstr)] == '.')
	    graph->num_graphics ++ ;
    }
    XtFree(wstr);

    if (strlen(app_data->lines)) graph->num_graphics ++ ;
   
    /* Towns syntax:
         draw.towns: Boston, Woburn, SanJose
         draw*Boston.x: 30
         draw*Boston.y: 30
         draw*Woburn.x: 130
         draw*Woburn.y: 30
         draw*SanJose.x: 30
         draw*SanJose.y: 130
    */
    wstr = XtNewString(app_data->towns)  ;
    for(pstr = (char *) strtok(wstr, ".,") ; pstr ;
	pstr = (char *) strtok( NULL, ".,")) {
        while (*pstr && isspace(*pstr)) pstr++;
        if (*pstr == '\0') break;
	newpush = XmCreatePushButton(graph->work_area, pstr, NULL, 0);
	XtAddCallback (newpush, XmNactivateCallback, PushCB, NULL);
	XtManageChild (newpush);
    }
    XtFree(wstr);
}

static void 
ReDraw(Graphic * graph,
       XEvent * event)
{
    Cardinal i ;
    Widget w = graph->work_area ;

    /* the expose event region could be used for better performance */
    for (i=0; i < graph->num_graphics; i++) {
	if (graph->graphics[i].type == POLYLINE)
	    XDrawLines(XtDisplay(w), XtWindow(w), 
		       XDefaultGC(XtDisplay(w),XDefaultScreen(XtDisplay(w))),
		       graph->graphics[i].points, 
		       graph->graphics[i].num_points, 
		       CoordModeOrigin);
    }
}

static void 
ReSize(Graphic * graph,
       Dimension width,
       Dimension height)
{
    /* Note: very bad design: resize should not modify the data since
       you lose everything when you resize tiny tiny tiny, but hey, what
       do you expect for a demo ? This way at least I can use the
       resource interface for creating the buttons in place (see InitDraw) */

    Widget w = graph->work_area ;
    Cardinal i,j ;
    Arg args[5];	
    int	n ;		   
    Widget * children ;
    Cardinal num_children ;
    Position x,y ;
    float xratio = (float) width / graph->old_width, 
          yratio = (float) height / graph->old_height ;

    /* reposition and resize the graphic units */
    for (i=0; i < graph->num_graphics; i++) {
	for (j=0; j < graph->graphics[i].num_points; j++) {
	    graph->graphics[i].points[j].x *= xratio ;
	    graph->graphics[i].points[j].y *= yratio ;
	}
    }

    /* reposition the pushbutton children */
    /* I can use XtMoveWidget here since it's like being part of the
       widget resize class method... */
    n = 0;
    XtSetArg (args[n], XmNnumChildren, &num_children);  n++;
    XtSetArg (args[n], XmNchildren, &children);  n++;
    XtGetValues (w, args, n);
    for (i=0; i < num_children; i++) {
	n = 0;
	XtSetArg (args[n], XmNx, &x);  n++;
	XtSetArg (args[n], XmNy, &y);  n++;
	XtGetValues (children[i], args, n);
	XtMoveWidget(children[i], 
		     (Position) (x * xratio),
		     (Position) (y * yratio));
    }
    
    graph->old_width = width ;
    graph->old_height = height ;
}


static void 
StartUnit(Graphic * graph,
	  Position x,
	  Position y)
{
    Widget w = graph->work_area ;

    graph->drag_point.x = graph->anchor_point.x = x ;
    graph->drag_point.y = graph->anchor_point.y = y ;
    graph->in_drag = True ;
    XDrawLine(XtDisplay(w), XtWindow(w), 
	      graph->drag_gc,
	      graph->anchor_point.x, graph->anchor_point.y,
	      graph->drag_point.x, graph->drag_point.y);
}

static void 
DragUnit(Graphic * graph,
	 Position x, 
	 Position y)
{
    Widget w = graph->work_area ;

    if (!graph->in_drag) return ;

    XDrawLine(XtDisplay(w), XtWindow(w), 
	      graph->drag_gc,
	      graph->anchor_point.x, graph->anchor_point.y,
	      graph->drag_point.x, graph->drag_point.y);

    graph->drag_point.x = x ;
    graph->drag_point.y = y ;

    XDrawLine(XtDisplay(w), XtWindow(w), 
	      graph->drag_gc,
	      graph->anchor_point.x, graph->anchor_point.y,
	      graph->drag_point.x, graph->drag_point.y);
}


static Boolean 
NearPoint (XPoint point,
	   Position x, 
	   Position y)
{
#define ERROR 5
    if ((point.x > x - ERROR) &&
	(point.x < x + ERROR) &&
	(point.y > y - ERROR) &&
	(point.y < y + ERROR)) return True ;
    else return False ;
}


static void 
EndUnit(Graphic * graph,
	Position x, 
	Position y)
{
    Widget w = graph->work_area ;
    Cardinal num_points ;

    /* no matter what happens, we need to remove the current rubber band */
    XDrawLine(XtDisplay(w), XtWindow(w), 
	      graph->drag_gc,
	      graph->anchor_point.x, graph->anchor_point.y,
	      graph->drag_point.x, graph->drag_point.y);

    /* if the given point if the same as the anchor, we're done with
       this polyline, exit drag mode and be ready for the next 
       graphic unit, i.e increment num_graphics */

    if (NearPoint(graph->anchor_point, x, y)) {
	graph->in_drag = False ;
	/* now see if a new unit needs to be created */
	if (graph->graphics[graph->num_graphics].num_points) {
	    graph->graphics[graph->num_graphics].type = POLYLINE ;
	    if (graph->num_graphics < MAX_GRAPH) graph->num_graphics ++ ;
	    else printf("The graphic buffer is full, overwrite the last...\n");
	}
    } else {

	/* draw the real line and store it in the structure */
	XDrawLine(XtDisplay(w), XtWindow(w), 
		  XDefaultGC(XtDisplay(w),XDefaultScreen(XtDisplay(w))),
		  graph->anchor_point.x, graph->anchor_point.y,
		  x, y);

	/* first point in a unit is actually special */
	num_points = graph->graphics[graph->num_graphics].num_points ;
	if (num_points == 0) {
	    graph->graphics[graph->num_graphics].points[num_points].x = 
		graph->anchor_point.x ;
	    graph->graphics[graph->num_graphics].points[num_points].y = 
		graph->anchor_point.y ;
	    graph->graphics[graph->num_graphics].num_points ++ ;
	    num_points ++ ;
	}
	graph->graphics[graph->num_graphics].points[num_points].x = x ;
	graph->graphics[graph->num_graphics].points[num_points].y = y ;
	if (graph->graphics[graph->num_graphics].num_points < MAX_POINT) 
	    graph->graphics[graph->num_graphics].num_points ++ ;
	else printf("The unit buffer is full, overwrite the last...\n");

	/* now start the new unit */
	graph->drag_point.x = graph->anchor_point.x = x ;
	graph->drag_point.y = graph->anchor_point.y = y ;
	XDrawLine(XtDisplay(w), XtWindow(w), 
		  graph->drag_gc,
		  graph->anchor_point.x, graph->anchor_point.y,
		  graph->drag_point.x, graph->drag_point.y);
    }
}

static void 
DeleteUnit(Graphic * graph,
	   Position x, 
	   Position y)
{
    Widget w = graph->work_area ;
    Cardinal i,j ;
    int a = -1 ;

    /* try to find a unit under this point */
    for (i=0; (i < graph->num_graphics) && (a == -1); i++) {
	for (j=0; j < graph->graphics[i].num_points; j++) {
	    if (NearPoint(graph->graphics[i].points[j], x, y)) {
		a = i ;
		break ;
	    }
	}
    }

    if (a != -1) { 
	/* found a unit under the current point, delete and redisplay */
	for (i = a ; i < graph->num_graphics ; i++) {
	    graph->graphics[i] = graph->graphics[i+1] ;
	}
	graph->num_graphics -- ;

	XClearArea(XtDisplay(w), XtWindow(w), 
		   0, 0, graph->old_width, graph->old_height, True);
    }
}

