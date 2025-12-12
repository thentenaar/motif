/* $TOG: main.c /main/6 1997/03/31 13:47:48 dbl $ */
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

/*
 * The Airport Drag And Drop Demo
 */

#include "main.h"

/* ============================ local constants ======================= */

#define MAX_ARGS 8

/* ============================ prototypes  ======================= */

static Widget MenuBarEntry(Widget menubar, String entry, String labels[],
		XtCallbackProc procs[], XtPointer private[], int count);

static void HelpCallback(Widget button, Airport this, XtPointer cbs);

static void ExitCallback(Widget	button, Airport this, XtPointer cbs);

static void ResizeAirport(Widget area, Airport this, XtPointer cbs);
/* resize callback of airport */

static void ShowAirport(Widget area, Airport this,  XtPointer cbs);
/* expose callback of airport */

static void AirportInput(Widget area, Airport this, XtPointer cbs);
/* Callback when pointer or keyboard input in airport */

static void ConfigureAirport(Widget area, Airport this);
/* configure geometry */

static void Warning(Airport this, XmString s1, XmString s2);
/* issue a warning to user */


/* ==============================================================
 *    Main loop
 */

int
main(int argc, char *argv[])
{
#define MAIN_CHILDREN		3
#define FORM_CHILDREN		3
#define PLANE_CHILDREN		6
#define TRAFFIC_CHILDREN	3
#define SetMenuEntry(k,l,p,d) labels[k]=l;procs[k]=p;private[k]=d;

   Pixmap flying;
   Pixmap parked;
   Pixmap mask;
   Widget theWidgetRoot;
   Widget main_tab[MAIN_CHILDREN];
   Widget from_tab[PLANE_CHILDREN];
   Widget to_tab[TRAFFIC_CHILDREN];
   Widget form_tab[FORM_CHILDREN];
   Widget framed[2];
   Widget entry;
   static char myClass[] = "XmdAirport";
   Arg args[10];
   int n ;
   String labels[4];
   XtCallbackProc procs[4];
   XtPointer private[4];
   XGCValues gcv;
   Airport this;


/*
 * Initialize
 */
   this = (Airport) XtCalloc(sizeof(AirportRec), 1);

   XtSetLanguageProc(NULL, (XtLanguageProc) NULL, NULL);

   theWidgetRoot = XtVaOpenApplication( &this->context,
                                        myClass,
				        NULL,
                                        0,
                                        &argc,
                                        argv,
                                        NULL,
                                        sessionShellWidgetClass,
				        XmNallowShellResize, True,
                                        NULL);

   this->display = XtDisplay(theWidgetRoot);
   FlightAtom = XmInternAtom(this->display, "FLIGHT", False);
   DeleteAtom = XmInternAtom(this->display, "DELETE", False);

   XtGetApplicationResources(theWidgetRoot, &AirportResources,
			     AirportResourcesSpecs,
			     XtNumber(AirportResourcesSpecs),
			     (ArgList) NULL, 0);

/*
 * Create the Main Window, the menubar and the pulldown menus
 */
   n = 0;
   this->main = XmCreateMainWindow(theWidgetRoot, "main", args, n);

   n = 0;
   main_tab[0] = XmCreateMenuBar(this->main, "menubar", args, n);
   n = 0;
   SetMenuEntry(n, "Exit",
		(XtCallbackProc) ExitCallback, (XtPointer) this ); n++;
   entry = MenuBarEntry(main_tab[0], "File", labels, procs, private, n);
   n = 0;
   SetMenuEntry(0, "Tutorial",
		(XtCallbackProc) HelpCallback, (XtPointer) this); n++;
   entry = MenuBarEntry(main_tab[0], "Help", labels, procs, private, n);
   n = 0;
   XtSetArg(args[n], XmNmenuHelpWidget, entry); n++;
   XtSetValues(main_tab[0], args, n);

/*
 * Create the Airport widget structure.
 * Two framed radio boxes show state.
 * A drawing area in the middle show track and planes
 */

   n = 0;
   this->screen_width = WidthOfScreen(XtScreenOfObject(this->main));
   this->screen_height = HeightOfScreen(XtScreenOfObject(this->main));
   XtSetArg(args[n], XmNwidth, this->screen_width*3/7); n++;
   XtSetArg(args[n], XmNresizePolicy, XmRESIZE_NONE); n++;
   main_tab[1] = XmCreateForm(this->main, "airport", args, n);

   n = 0;
   XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
   XtSetArg(args[n], XmNbottomAttachment,XmATTACH_FORM); n++;
   XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM) ; n++;
   form_tab[0] = XmCreateFrame(main_tab[1], "flight", args, n);

   n = 0;
   XtSetArg(args[n], XmNchildType, XmFRAME_TITLE_CHILD); n++;
   framed[0] = XmCreateLabel(form_tab[0], "title", args, n);

   n = 0;
   XtSetArg(args[n], XmNchildType, XmFRAME_WORKAREA_CHILD); n++;
   XtSetArg(args[n], XmNresizePolicy, XmRESIZE_GROW); n++;
   XtSetArg(args[n], XmNradioBehavior, True); n++;
   framed[1] = XmCreateRadioBox(form_tab[0], "panel", args, n);

   n = 0;
   XtSetArg(args[n], XmNset, True); n++;
   XtSetArg(args[n], XmNvisibleWhenOff, False); n++;
   this->from.departure =
     from_tab[0] = XmCreateToggleButton(framed[1],
					       "departure", args, n);
   n = 0;
   XtSetArg(args[n], XmNvisibleWhenOff, False); n++;
   this->from.over_ocean =
     from_tab[1] = XmCreateToggleButton(framed[1],
					       "over_ocean", args, n);
   n = 0;
   XtSetArg(args[n], XmNvisibleWhenOff, False); n++;
   this->from.over_land =
     from_tab[2] = XmCreateToggleButton(framed[1],
					       "over_land", args, n);
   n = 0;
   XtSetArg(args[n], XmNvisibleWhenOff, False); n++;
   this->from.over_site =
     from_tab[3] = XmCreateToggleButton(framed[1],
					       "radar", args, n);
   n = 0;
   XtSetArg(args[n], XmNvisibleWhenOff, False); n++;
   this->from.landing =
     from_tab[4] = XmCreateToggleButton(framed[1], "landing", args, n);

   n = 0;
   XtSetArg(args[n], XmNvisibleWhenOff, False); n++;
   this->from.landed =
     from_tab[5] = XmCreateToggleButton(framed[1], "landed", args, n);
   XtManageChildren(from_tab, PLANE_CHILDREN);
   XtManageChildren(framed, 2);

/*
 * Create the Airport Traffic control state
 */

   n = 0;
   XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
   XtSetArg(args[n], XmNbottomAttachment,XmATTACH_FORM); n++;
   XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM) ; n++;
   form_tab[1] = XmCreateFrame(main_tab[1], "traffic", args, n);

   n = 0;
   XtSetArg(args[n], XmNchildType, XmFRAME_TITLE_CHILD); n++;
   framed[0] = XmCreateLabel(form_tab[1], "title", args, n);

   n = 0;
   XtSetArg(args[n], XmNchildType, XmFRAME_WORKAREA_CHILD); n++;
   XtSetArg(args[n], XmNradioBehavior, True); n++;
   framed[1] = XmCreateRadioBox(form_tab[1], "traffic_box", args, n);

   n = 0;
   XtSetArg(args[n], XmNset, True); n++;
   XtSetArg(args[n], XmNvisibleWhenOff, False); n++;
   this->to.leave =
     to_tab[1] = XmCreateToggleButton(framed[1],
				      "no_traffic", args, n);
   n = 0;
   XtSetArg(args[n], XmNvisibleWhenOff, False); n++;
   this->to.enter =
     to_tab[0] = XmCreateToggleButton(framed[1],
				      "radar_echo", args, n);
   n = 0;
   XtSetArg(args[n], XmNvisibleWhenOff, False); n++;
   this->to.landing =
     to_tab[2] = XmCreateToggleButton(framed[1],
				      "landing", args, n);

   XtManageChildren(to_tab, TRAFFIC_CHILDREN);
   XtManageChildren(framed, 2);

/*
 * Create the Airport picture
 */
   n = 0;
   XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
   XtSetArg(args[n], XmNbottomAttachment,XmATTACH_FORM); n++;
   XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET) ; n++;
   XtSetArg(args[n], XmNleftWidget, form_tab[0]) ; n++;
   XtSetArg(args[n], XmNrightAttachment, XmATTACH_WIDGET) ; n++;
   XtSetArg(args[n], XmNrightWidget, form_tab[1]) ; n++;
   this->airport =
     form_tab[2] = XmCreateDrawingArea(main_tab[1], "da", args, n);
   XtAddCallback(this->airport, XmNexposeCallback,
		 (XtCallbackProc) ShowAirport, (XtPointer) this);
   XtAddCallback(this->airport, XmNresizeCallback,
		 (XtCallbackProc) ResizeAirport, (XtPointer) this);
   XtAddCallback(this->airport, XmNinputCallback,
		 (XtCallbackProc) AirportInput, (XtPointer) this);
/*
 * Create the Airport Message area
 */

   n = 0;
   main_tab[2] = XmCreateFrame(this->main, "message", args, n);

   n = 0;
   XtSetArg(args[n], XmNchildType, XmFRAME_TITLE_CHILD); n++;
   framed[0] = XmCreateLabel(main_tab[2], "title", args, n);

   n = 0;
   XtSetArg(args[n], XmNchildType, XmFRAME_WORKAREA_CHILD); n++;
   XtSetArg(args[n], XmNvisibleItemCount, 4); n++;
   this->msg_area = XmCreateScrolledList(main_tab[2], "msg_list", args, n);
   XtManageChild(this->msg_area);
   framed[1] = XtParent(this->msg_area);
   XtManageChildren(framed, 2);

   n = 0;
   XtSetArg(args[n], XmNmenuBar, main_tab[0]); n++;
   XtSetArg(args[n], XmNworkWindow, main_tab[1]); n++;
   XtSetArg(args[n], XmNmessageWindow, main_tab[2]); n++;
   XtSetValues(this->main, args, n);
   XtManageChildren(form_tab, FORM_CHILDREN);
   XtManageChildren(main_tab, MAIN_CHILDREN);
   XtManageChild(this->main);

   XtRealizeWidget(theWidgetRoot);

/*
 * Create the drag icon
 */

   parked = XCreateBitmapFromData(this->display,
			   XtWindow(this->main), (char*)plane_bits,
			   plane_width, plane_height);

   flying = XCreateBitmapFromData(this->display,
				  XtWindow(this->main), (char*)flying_bits,
				  flying_icon_width, flying_icon_height);

   mask = XCreateBitmapFromData(this->display,
				XtWindow(this->main), (char*)flying_mask_bits,
				flying_mask_width, flying_mask_height);
   n = 0;
   XtSetArg(args[n], XmNpixmap, flying); n++;
   XtSetArg(args[n], XmNdepth, 1); n++;
   XtSetArg(args[n], XmNmask, mask); n++;
   XtSetArg(args[n], XmNwidth, flying_icon_width); n++;
   XtSetArg(args[n], XmNheight, flying_icon_height) ; n++;
   this->dragIcon = XmCreateDragIcon(form_tab[2], "drag", args, n);
   if (this->dragIcon  == NULL) {
      printf("cannot create drag icon\n");
      exit(0);
   }
  /* Create GC for drawing planes */
   gcv.function = GXcopy;
   gcv.fill_style = FillOpaqueStippled;
   gcv.stipple = parked;
   gcv.foreground = AirportResources.spot_foreground;
   gcv.background = AirportResources.spot_background;
   this->park.gc = XCreateGC(this->display, XtWindow(this->main),
			     GCFunction|GCStipple|GCFillStyle
			     |GCForeground|GCBackground,
			     &gcv);

   gcv.line_style = LineDoubleDash;
   gcv.line_width = AirportResources.track_border_width;
   gcv.fill_style = FillSolid;
   gcv.foreground = AirportResources.track_foreground;
   gcv.background = AirportResources.track_background;
   this->track.gc = XCreateGC(this->display, XtWindow(this->main),
			      GCFunction|GCLineWidth|GCLineStyle
			      |GCFillStyle|GCForeground|GCBackground,
			      &gcv);
   XtAppMainLoop(this->context);

   return 0;    /* make compiler happy */
}

/* ===============================================================
 *   Create a cascade and a pulldown entry into a menubar,
 * 	from a list of push button specs.
 *   If the label is NULL, create a separator instead of a push button.
 */

static Widget MenuBarEntry(Widget menubar, String entry, String labels[],
			   XtCallbackProc procs[], XtPointer private[],
			   int count)
{
   Widget menu;
   Widget child;
   Arg args[8];
   int n;
   int i;
   char name[128];

   n = 0;
   menu = XmCreatePulldownMenu(menubar,
			       strcat(strcpy(name, entry), "Menu"),
			       args, n);
   for( i = 0; i < count; i++) {
      n = 0;
      if (labels[i] == NULL) {
	 child = XmCreateSeparator(menu, "sep", args, n);
      }
      else {
	 child = XmCreatePushButton(menu, labels[i], args, n);
	 if (procs[i] != NULL)
	   XtAddCallback(child, XmNactivateCallback, procs[i], private[i]);
      }
      XtManageChild(child);
   }
   n = 0;
   XtSetArg(args[n], XmNlabelString, XmStringCreateLocalized(entry)); n++;
   XtSetArg(args[n], XmNsubMenuId, menu); n++;
   child = XmCreateCascadeButton(menubar, entry, args, n);
   XtManageChild(child);
   return child;
}


/* ===============================================================
 *   The Exit Callback.  Exit properly.
 */

static void ExitCallback(Widget	button, Airport this, XtPointer cbs)
{
   XtCloseDisplay(this->display);
   exit(0);
}

/* ======================================================================
 *  Help Callback
 */

static void HelpCallback(Widget button, Airport this, XtPointer cbs)
{
  static char helpmsg[] = "Read the README file, please...";

  if ( this->help_dialog == NULL ) {
     Arg args[8];
     int n = 0;

     XtSetArg(args[n], XmNmessageString,
	      XmStringCreateLocalized(helpmsg)); n++;
     this->help_dialog = XmCreateInformationDialog(button,
					    "helpdialog", args, n);
  }
  XtManageChild(this->help_dialog);
}

/* ===============================================================
 * Create Airport tracks. Open airport to traffic.
 *
 */

static void ConfigureAirport(Widget area, Airport this)
{
   Arg args[10];
   int i, n;
   Window root, parent, *wlist;
   unsigned int count;
   Dimension width, height;
   int min_width, min_height;

/*
 * Try to make the screen background window the ocean.
 * Be aware of running without a window manager,
 * or with a reparenting window manager
 * or a window manager like HP VUEWM who have a background window
 */

   this->ocean = (Window) NULL;
   if (XQueryTree(this->display, XtWindow(XtParent(this->main)),
		  &root, &parent, &wlist, &count)  != 0) {
      Window current;

      do {
	 XWindowAttributes wa;

	 current = this->ocean = parent;
	 if (XQueryTree(this->display, current, &root, &parent,
			&wlist, &count) == 0)
	   break;

	 if (current == root)
	   {
	      /* beware of HP-VUE */

	      XGetWindowAttributes(this->display, wlist[0], &wa);
	      this->ocean = (wa.width >= this->screen_width
			     && wa.height >= this->screen_height) ?
			       wlist[0] : root;
	   }
	 else {
	    XGetWindowAttributes(this->display, current, &wa);
	    if (wa.width >= this->screen_width
		&& wa.height >= this->screen_height)
	      {
		 this->ocean = current;
		 break;
	      }
	 }
      } while (root != current);
   }

   n = 0;
   XtSetArg(args[n], XmNwidth, &width); n++;
   XtSetArg(args[n], XmNheight, &height); n++;
   XtGetValues(area, args, n);

   if (this->state == open)  { /* destroy current data */
      XtFree((char *)this->park.spots);
      XClearWindow(this->display, XtWindow(area) );
      this->park.spot_count = 0;
   }
   min_height = AirportResources.track_margin * 4 + plane_height*2
     		+ AirportResources.track_border_width*2 + MIN_TRACK_SIZE;
   min_width = AirportResources.track_margin*2 + plane_width ;
   if ( min_height > (int) height || min_width > (int) width)
     {
	Warning(this,
		XmStringCreateLocalized(AirportResources.too_small),
		NULL);
	this->state = closed;
     }
   else
     {
	Position x, y;

	this->track.bbox.x = (short) AirportResources.track_margin;
	this->track.bbox.y = (short) AirportResources.track_margin*2
	  + plane_height;
	this->track.bbox.width = width - AirportResources.track_margin*2;
	this->track.bbox.height = height - 2*this->track.bbox.y;
	this->park.plane_count = (int) this->track.bbox.width / plane_width;
	this->park.spot_count = 2 * this->park.plane_count;
	this->park.spots = (Spot) XtCalloc(sizeof(SpotRec),
					   this->park.spot_count);
	x = AirportResources.track_margin;
	y = AirportResources.track_margin;
	for (i = 0; i < this->park.plane_count ; i++) {
	   this->park.spots[i].empty = False;
	   this->park.spots[i].x = x ;
	   this->park.spots[i].y = y ;
	   x += plane_width;

	}
	x = AirportResources.track_margin;
	y += this->track.bbox.y + this->track.bbox.height
	  	+ AirportResources.track_margin;
	for (i = this->park.plane_count; i < this->park.spot_count ; i++) {
	   this->park.spots[i].empty = True;
	   this->park.spots[i].x = x ;
	   this->park.spots[i].y = y ;
	   x += plane_width;
	}
      this->park.state = normal;
   }
}

/* ===============================================================
 *   On Resize, clear the data structure and reset airport
 *   So the first time it is initialized.
 */

static void ResizeAirport(Widget area, Airport this, XtPointer cbs)
{

   if (! XtIsRealized(area) || this->state == closed)
     return ;
   ConfigureAirport(area, this);

}

/* ===============================================================
 *   Expose callback. Redraw the airport
 */
static void ShowAirport(Widget area, Airport this, XtPointer cbs)
{

   if (this->state == closed) {
      ConfigureAirport(area, this);
      MakeAirportDropSite(area, this);
      this->state = open;
   }
   DrawAirport(this, XtWindow(area), AirportResources.track_background,
	       AirportResources.track_foreground);
}

/* ===============================================================
 *  Input callback. On button 2 drag plane.
 */

static void AirportInput(Widget area, Airport this, XtPointer cbs)
{
   int i;
   XButtonEvent *evt;
   XmDrawingAreaCallbackStruct *call_data = (XmDrawingAreaCallbackStruct *)cbs;

   if ((call_data->event->type != ButtonPress)
       && (call_data->event->type != ButtonRelease))
     return;

   evt = (XButtonEvent *) call_data->event;
   switch(evt->type) {

   case ButtonPress:
      switch (evt->button) {

      case Button2: /* start drag ?? */

	 for (i = 0; i < this->park.spot_count ; i++) {
	    if (evt->x >= this->park.spots[i].x
		&& evt->y >= this->park.spots[i].y
		&& evt->x <= this->park.spots[i].x + plane_width
		&& evt->y <= this->park.spots[i].y + plane_height ) {
	       /* hit detection of the starting gate */
	       if (this->park.spots[i].empty == False)
		 TakeOff(area, this, evt, i);
	       break;
	    }
	 }
	 break;

      default:
	 return;
      }
      break;

   default:
      return;
   }
}

/* ===============================================================
 *
 */
void DrawAirport(Airport this, Window wd, Pixel bg, Pixel fg)
{
   int i;
   Position y;

   XSetForeground(this->display, this->track.gc, fg);
   XSetBackground(this->display, this->track.gc, bg);
   XFillRectangle(this->display, wd, this->track.gc,
		  this->track.bbox.x, this->track.bbox.y,
		  this->track.bbox.width, this->track.bbox.height);
   XDrawLine(this->display, wd, this->track.gc,
	     this->track.bbox.x,
	     this->track.bbox.y + AirportResources.track_border_width,
	     this->track.bbox.x + this->track.bbox.width,
	     this->track.bbox.y + AirportResources.track_border_width);
   y = this->track.bbox.y + this->track.bbox.height
	     - AirportResources.track_border_width;
   XDrawLine(this->display, wd, this->track.gc,
	     this->track.bbox.x, y,
	     this->track.bbox.x + this->track.bbox.width, y);

   for (i = 0; i < this->park.spot_count ; i++) {
      if (this->park.spots[i].empty == False) {
	 XSetTSOrigin(this->display, this->park.gc,
		      this->park.spots[i].x, this->park.spots[i].y);
	 XFillRectangle(this->display, wd, this->park.gc,
			this->park.spots[i].x, this->park.spots[i].y,
			plane_width, plane_height);
      }
   }
}

/* ===============================================================
 *   Display a message in the message area
 */
void Paging(Airport this, String msg)
{
   XmString item;

   item = XmStringCreateLocalized(msg);
   XmListDeselectAllItems(this->msg_area);
   XmListAddItem(this->msg_area, item, 0);
   XmListSetBottomItem(this->msg_area, item);
}

/* =====================================================================
 * Show application modal warning dialogue box with specified messages
 */

static void Warning(Airport this, XmString s1, XmString s2)
{
   XmString msg;

   if (s1 == NULL)
     msg = s2;
   else if (s2 == NULL)
     msg = s1;
   else
     msg = XmStringConcat(s1, s2);

   if (this->warning_dialog == NULL) {
      Arg args[8];
      int n = 0;

      XtSetArg(args[n], XmNdialogStyle,
	       XmDIALOG_FULL_APPLICATION_MODAL);
      n++;
      this->warning_dialog = XmCreateWarningDialog(this->main,
						"warning", args, n);
   }
   XtVaSetValues(this->warning_dialog, XmNmessageString, msg, NULL);
   XtManageChild(this->warning_dialog);
}
