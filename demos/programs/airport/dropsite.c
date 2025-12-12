/* $XConsortium: dropsite.c /main/5 1995/07/15 20:44:42 drk $ */
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
 * The module that contains all drop site / receiver code
 */

#include "airport.h"
#include "dropsite.h"

/* ============================ prototypes  ======================= */

static void TrafficSignal(Widget dropSite, XtPointer client_data,
			  XmDragProcCallbackStruct *call_data);
/* drag proc callback on airport. Something is flying over */

static void SomethingLanding(Widget dc, XtPointer client_data,
			     XmDropProcCallbackStruct *call_data);
/* drop proc callback on airport. Something is landing */

static void TaxiIn(Widget transfer,  Airport this,
		   Atom * selection, Atom * type, XtPointer value,
		   unsigned long *length, int *format);
/* Xt Selection Callback. Transfer flight number from source */

/* ===============================================================
 *   Expose callback. Redraw the airport
 */
void MakeAirportDropSite(Widget area, Airport this)
{
   Arg args[10];
   int n = 0;
   
   XtSetArg(args[n], XmNimportTargets, &FlightAtom); n++;
   XtSetArg(args[n], XmNnumImportTargets, 1); n++;
   XtSetArg(args[n], XmNdragProc, TrafficSignal); n++;
   XtSetArg(args[n], XmNdropProc, SomethingLanding); n++;
   XtVaSetValues(area, XmNuserData, this, NULL);
   XmDropSiteRegister(area, args, n);
}

/* ===============================================================
 * Drag proc callback of the drop site.
 * The airport control is detecting/loosing radar echo on enter/leave
 * On motion events do animation of the tracks (flashing)
 */

static void TrafficSignal(Widget dropSite, XtPointer client_data,
			  XmDragProcCallbackStruct *call_data)
{
   Airport this;

   XtVaGetValues(dropSite, XmNuserData, &this, NULL);
   switch(call_data->reason) { 
      
   case XmCR_DROP_SITE_MOTION_MESSAGE:
      DrawAirport(this, XtWindow(dropSite),
		  AirportResources.drag_under_background,
		  AirportResources.drag_under_foreground);
      break;
      
   case XmCR_DROP_SITE_ENTER_MESSAGE:
      XmToggleButtonSetState(this->to.enter, True, True);
      break;

   case XmCR_DROP_SITE_LEAVE_MESSAGE: 
      XmToggleButtonSetState(this->to.leave, True, True);
      DrawAirport(this, XtWindow(dropSite),
		  AirportResources.track_background,
		  AirportResources.track_foreground);
      break;

   }
}

/* ===============================================================
 * Drop proc callback of the drop site. 
 * The airport knows that something is landing. What is it ?
 * If it is a plane then check there is a gate available.
 * 	If no gate available, reject the drop
 * If it is not a plane landing, reject the drop too.
 */

static void SomethingLanding(Widget dropSite, XtPointer client_data,
			     XmDropProcCallbackStruct *call_data)
{
#define NUMREC 2
   Arg args[16];
   int n;
   XmDropTransferEntryRec target_data[NUMREC];
   Airport this;
   unsigned char status = XmTRANSFER_FAILURE;
   int num_targets = 0;

   XtVaGetValues(dropSite, XmNuserData, &this, NULL);
   XmToggleButtonSetState(this->to.landing, True, True);
   n = 0;
   if (call_data->dropSiteStatus == XmVALID_DROP_SITE)
     {
	int gate;
	      
	for (gate = 0; gate < this->park.spot_count; gate ++) {
	   if (this->park.spots[gate].empty == True)
	     break;
	}
	if (gate == this->park.spot_count)
	  Paging(this, "Airport saturated, no landing");
	else {
	   status = XmTRANSFER_SUCCESS;
	   call_data->operation = XmDROP_MOVE;
	   num_targets = 1;
	   this->to.gate = gate;
	   target_data[0].client_data = (XtPointer) this;
	   target_data[0].target = FlightAtom;
	   XtSetArg(args[n], XmNtransferProc, TaxiIn);  n++;
	   XtSetArg(args[n], XmNdropTransfers, target_data);  n++;
	}
     }
   else {
      Paging(this, "Non Identified Flying Object is landing");
   }
   XtSetArg(args[n], XmNnumDropTransfers, num_targets);  n++;
   XtSetArg(args[n], XmNtransferStatus, status);  n++;
   XmDropTransferStart(call_data->dragContext, args, n);
#undef NUMREC
}

/* ===============================================================
 * The selection callback proc after drop transfer
 * Accept plane at free gate and signal arrival on paging.
 * If some error occured during the transfer, do plane crash
 */

static void TaxiIn(Widget transfer, Airport this,
		   Atom * selection, Atom * type, XtPointer value,
		   unsigned long *length, int *format)
{
   if ((*format == 32) && (*length == 1) && (*type == FlightAtom)) {
      /* 
       * received a 32 bit value that is the flight number
       */
      long flight_number;
      char msg[256];

      flight_number =  *((long *)value); 
      this->park.spots[this->to.gate].empty = False;
      sprintf(msg, "Flight %ld arrived at gate %d",
	   flight_number, this->to.gate);
      Paging(this, msg);
      DrawAirport(this, XtWindow(this->airport),
		  AirportResources.track_foreground,
		  AirportResources.track_background);
      XmToggleButtonSetState(this->to.leave, True, True);
      XtFree(value);
  }
   else {
     Paging(this, "Too bad: plane has crashed in landing");
     XtVaSetValues(transfer,
		   XmNtransferStatus, XmTRANSFER_FAILURE,
		   XmNnumDropTransfers, 0,
		   NULL);
     return;
  }
}
