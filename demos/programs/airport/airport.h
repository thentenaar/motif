/* $XConsortium: airport.h /main/5 1995/07/15 20:44:29 drk $ */
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
 *    Airport Data Structure
 *
 *
 *    Created: Vania JOLOBOFF / May 1992
 *
 *
 * ======================================================================
 * 
 */

/*
 * ===================== include ========================================
 */

#if (! defined airport_h)
#define airport_h 

#include <stdlib.h>
#include <stdio.h>
#include <Xm/XmAll.h>

/* ======================================================================
 *  General Types
 */

typedef struct _Source {
   Widget departure;
   Widget over_ocean;
   Widget over_land;
   Widget over_site;
   Widget landing;
   Widget landed;
} SourceRec, *Source;

typedef struct _Destination {
   Widget enter;
   Widget leave;
   Widget landing;
   Widget parked;
   int gate;
} DestinationRec, *Destination;

typedef struct _Spot {
   Boolean empty;
   Position x,y;
} SpotRec, *Spot;

typedef enum {empty, normal, full} ParkState ;
 
typedef struct _Park {
   ParkState state;
   GC gc;
   int spot_count;
   int plane_count;
   Spot spots; /* variable length array of spots */
} ParkRec, *Park;

typedef struct _Track {
   int borderwidth;
   XRectangle bbox;
   GC gc;
} TrackRec, *Track;

typedef enum {closed, open} AirportState ;

/*
 * The airport data structure consists of
 * 	- the widget needed in various callbacks 
 *	- the drag source data
 *	- the drop site data
 *	- the details of the track graphics 
 */

typedef struct _Airport {
   XtAppContext context; 
   Display * display;
   Window ocean;
   int screen_width;
   int screen_height;
   AirportState state;
   Widget main; /* the main window */
   Widget airport; /* the airport drawing area */
   Widget help_dialog;
   Widget error_dialog;
   Widget warning_dialog;
   Widget dragIcon;
   Widget msg_area;
   SourceRec from; /* the drag data */
   DestinationRec to; /* drop site data */
   ParkRec park; /* info on available gates at the airport */
   TrackRec track; /* info on track graphics */
} AirportRec, *Airport;

/*
 * A flight has a flight number. It starts from some airport (from)
 * When landing, it will be taxi-ed to some gate.
 */

typedef struct _Flight {
   long number;
   int gate;     /* the gate it's going to be taxi-ed */
   Airport from; /* the airport it took off */
} FlightRec, *Flight;

/* ======================================================================
 * Global constants
 */

#define plane_width 25
#define plane_height 25

/* ======================================================================
 * Global variables 
 */

#ifdef DECLAREGLOBAL
#define global
#else
#define global extern
#endif

global Atom FlightAtom;
global Atom DeleteAtom;

/* 
 * Application Resources
 */

global struct _AirportResources {
   String too_small;
   Pixel spot_background;
   Pixel spot_foreground;
   Pixel flight_background;
   Pixel flight_foreground;
   Pixel invalid_foreground;
   Pixel valid_foreground;
   Pixel none_foreground;
   Pixel track_background;
   Pixel track_foreground;
   Pixel drag_under_background;
   Pixel drag_under_foreground;
   Dimension track_border_width;
   Dimension track_margin;
   int bell_duration;
   int bell_percent;
} AirportResources;

global void DrawAirport(Airport this, Window wd, Pixel bg, Pixel fg);
/* draw the tracks and parked planes */

global void Paging(Airport this, String msg);
/* display a message in message area */


#endif /* airport_h */
