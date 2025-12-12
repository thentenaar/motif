/* $TOG: dragsource.c /main/6 1997/03/05 11:46:31 dbl $ */
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
 *
 * The module that contains all drag source / initiator code
 *
 *    Created: Vania JOLOBOFF / May 1992
 *
 */

#include <Xm/XmAll.h>
#include "airport.h"
#include "dragsource.h"

/* ============================ prototypes  ======================= */

static void EnterTopLevel(Widget dc, Airport this,
			  XmTopLevelEnterCallbackStruct *call_data);
/* drag callback entering top level window */

static void LeaveTopLevel(Widget dc, Airport this,
			  XmTopLevelLeaveCallbackStruct *call_data);
/* drag callback leaving top level window */

static void DropSiteEnter(Widget dc, Airport this,
			  XmDropSiteEnterCallbackStruct *call_data);
/* drag callback entering drop site */

static void DropSiteLeave(Widget dc, Airport this,
			  XmDropSiteLeaveCallbackStruct *call_data);
/* drag callback leaving drop site */

static void Flying(Widget dc, Airport this,
		   XmDragMotionCallbackStruct *call_data);
/* drag motion callback on initiator */

static void OperationChanged(Widget dc, Flight flight,
			     XmOperationChangedCallbackStruct *call_data);
/* side effect change callback on initiator */

static void PlaneLanding(Widget dc, Flight flight,
			 XmDropStartCallbackStruct *call_data);
/* drop start callback on initiator side */

static void PlaneHasLanded(Widget dc, Flight flight,
			   XmDropFinishCallbackStruct *call_data);
/* drop end callback on initiator side */

static void FlightFinished(Widget dc, Flight flight,
				  XmDragDropFinishCallback *call_data);
/* drag drop finish callback on initiator. Remove data for that plane */

static Boolean SendPlaneID(Widget drag_context, Atom *selection, Atom *target, 
			 Atom *type,
			 XtPointer *value, unsigned long *length,
			 int *format); 

/* The Xt selection convert proc sending the data to the drop site */


/* ===============================================================
 *  Start dragging a plane icon.
 */

void TakeOff(Widget area, Airport this, XButtonEvent *evt, int spot)
{
   Arg args[20];
   int n;
   Flight flight;
   Widget dc;
   char msg[256];
   static Boolean init = True;

   if (init) {
      srand((unsigned int) XtWindow(area));
      init = False;
   }

   flight = (Flight)XtCalloc(sizeof(FlightRec), 1);
   flight->number = rand();
   flight->from = this;
   flight->gate = spot;
   sprintf(msg, "Flight %ld leaving from gate %d",
	   flight->number, flight->gate);
   Paging(this, msg);
   n = 0;
   XtSetArg(args[n], XmNclientData, flight); n++;
   XtSetArg(args[n], XmNconvertProc, SendPlaneID); n++;
   XtSetArg(args[n], XmNcursorBackground,
	    AirportResources.flight_background); n++;
   XtSetArg(args[n], XmNcursorForeground,
	    AirportResources.flight_foreground); n++;
   XtSetArg(args[n], XmNvalidCursorForeground,
	    AirportResources.valid_foreground); n++;
   XtSetArg(args[n], XmNinvalidCursorForeground,
	    AirportResources.invalid_foreground); n++;
   XtSetArg(args[n], XmNnoneCursorForeground,
	    AirportResources.none_foreground); n++ ;
   XtSetArg(args[n], XmNdragOperations, XmDROP_MOVE) ; n++;
   XtSetArg(args[n], XmNblendModel, XmBLEND_JUST_SOURCE) ; n++;
   XtSetArg(args[n], XmNsourceCursorIcon, this->dragIcon) ; n++;
   XtSetArg(args[n], XmNexportTargets, &FlightAtom); n++;
   XtSetArg(args[n], XmNnumExportTargets, 1); n++;
   dc = XmDragStart(area, (XEvent *)evt, args, n);
   XtAddCallback(dc, XmNdragMotionCallback,
		 (XtCallbackProc) Flying, (XtPointer) this);
   XtAddCallback(dc, XmNdropSiteEnterCallback,
		 (XtCallbackProc) DropSiteEnter, (XtPointer) this);
   XtAddCallback(dc, XmNdropSiteLeaveCallback,
		 (XtCallbackProc) DropSiteLeave, (XtPointer) this);
   XtAddCallback(dc, XmNtopLevelEnterCallback,
		 (XtCallbackProc) EnterTopLevel, (XtPointer) this);
   XtAddCallback(dc, XmNtopLevelLeaveCallback,
		 (XtCallbackProc) LeaveTopLevel, (XtPointer) this);
   XtAddCallback(dc, XmNdropStartCallback,
		 (XtCallbackProc) PlaneLanding, (XtPointer) flight);
   XtAddCallback(dc, XmNdropFinishCallback,
		 (XtCallbackProc) PlaneHasLanded, (XtPointer) flight);
   XtAddCallback(dc, XmNdragDropFinishCallback,
		 (XtCallbackProc) FlightFinished , (XtPointer) flight);
   XtAddCallback(dc, XmNoperationChangedCallback,
		 (XtCallbackProc) OperationChanged, (XtPointer) flight);

}

/* ===============================================================
 * Pointer enters a new top level window.
 * Check if this is the ocean or new land
 */

static void EnterTopLevel(Widget dc, Airport this,
			  XmTopLevelEnterCallbackStruct *call_data)
{

   if (call_data->window != this->ocean)
     XmToggleButtonSetState(this->from.over_land, True, True);
}

/* ===============================================================
 *  Pointer leaves current top level window.
 *  Assume we are back on the ocean. Next enter will reset the right value.
 */

static void LeaveTopLevel(Widget dc, Airport this,
			  XmTopLevelLeaveCallbackStruct *call_data)
{  
   XmToggleButtonSetState(this->from.over_ocean, True, True);
}

/* ===============================================================
 * Someone is entering over the airport.
 * Signal radar detection.
 */

static void DropSiteEnter(Widget dc, Airport this,
			  XmDropSiteEnterCallbackStruct *call_data)
{
   XmToggleButtonSetState(this->from.over_site, True, True);
}

/* ===============================================================
 * The thing that was over is leaving the airport.
 * Radar contact lost.
 */

static void DropSiteLeave(Widget dc, Airport this,
			  XmDropSiteLeaveCallbackStruct *call_data)
{  
   XmToggleButtonSetState(this->from.over_land, True, True);
}

/* ===============================================================
 * Motion callback on source side.
 * This is where we would could do fancy animation of the source icon.
 * Here we just beep.
 */

static void Flying(Widget dc,  Airport this,
		   XmDragMotionCallbackStruct *call_data)
{
   static Boolean first = True;

   if (first) 
     { 
	XKeyboardControl controlValues;
	unsigned long valueMask = KBBellPercent | KBBellPitch | KBBellDuration;

	controlValues.bell_percent  = AirportResources.bell_percent;
	controlValues.bell_pitch    = 440; 
	controlValues.bell_duration = AirportResources.bell_duration;
	XChangeKeyboardControl(this->display, valueMask, &controlValues);
	first = False; 
     }

   XBell(this->display, 100);

}

/* ===============================================================
 * The drop start callback on the source side.
 * Set state to plane landing
 */

static void PlaneLanding(Widget dc, Flight flight,
			 XmDropStartCallbackStruct *call_data)
{
   XmToggleButtonSetState(flight->from->from.landing, True, True);
}


/* ===============================================================
 * The operation changed callback. Planes only move, no copy
 */

static void OperationChanged(Widget dc, Flight flight,
			   XmOperationChangedCallbackStruct *call_data)
{

}

/* ===============================================================
 * The drop finish callback. The plane has landed somewhere
 *  Hold on, it's not finished yet
 */

static void PlaneHasLanded(Widget dc, Flight flight,
			   XmDropFinishCallbackStruct *call_data)
{
   Airport this = flight->from;
   char msg[256];

   XmToggleButtonSetState(this->from.landed, True, True);

   if (call_data->completionStatus == XmDROP_SUCCESS) {
      Spot spot = &(this->park.spots[flight->gate]);
      
      spot->empty = True;
      XClearArea(this->display, XtWindow(this->airport), 
		 spot->x, spot->y, plane_width, plane_height, False);
     sprintf(msg, "Passengers from flight %ld have landed safely",
	      flight->number);
   }
   else
     sprintf(msg, "Flight %ld has returned", flight->number);
   Paging(this, msg);
}

/* ===============================================================
 * the drag drop finish callback. Clean data structures.
 */

static void FlightFinished(Widget dc, Flight flight,
				  XmDragDropFinishCallback *call_data)
{
   Airport this = flight->from;

   XmToggleButtonSetState(this->from.departure, True, True);
   XtFree((char *) flight);
}
/* ===============================================================
 *  Xt convert selection proc. 
 *  Send flight ID on drop transfer request.
 *  Delete the source plane image on Delete. You know it's arrived there.
 */

static Boolean SendPlaneID(Widget drag_context, Atom *selection, Atom *target, 
			 Atom *type,
			 XtPointer *value, unsigned long *length,
			 int *format)
{
   Airport this;
   Flight flight;

   XtVaGetValues(drag_context, XmNclientData, &flight, NULL);
   this = flight->from;

   if (*target == FlightAtom) {
      long * data = (long *) XtMalloc(sizeof(long));

      *data = flight->number;
      *type = FlightAtom;
      *value = (XtPointer) data;
      *format = 32;
      *length = 1;
      return True;
   }
   else {
      Paging(flight->from, "Erroneous messsage on radio");
      return False;
   }
}
