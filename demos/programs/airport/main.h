/* $TOG: main.h /main/5 1997/03/31 13:46:32 dbl $ */
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
 *    Airport Main Header File
 *
 *
 * The airport demo is a totally useless program to illustrate
 * Drag and Drop in Motif 1.2.
 * It consists of an airport containing 
 *	- plane icons that can be dragged
 *	- tracks where planes can be dropped (land)
 *	- a radio box that shows current state of outcoming flights
 *	- a raido box that shows current state of incoming flights
 *	- a message area that lists successive interesting messages.
 *	
 * The demo is more interesting when two instances of this program
 * are run simultaneously.
 *
 * Each time a plane is dragged, a flight number is randomly created
 * and the plane is announced to leave in the message area.
 *
 * Each time a plane is dropped onto an airport, if a gate is available,
 * the plane is taxi-ed to this gate. The data transferred between the 
 * source airport and the destination airport is the flight number.
 * The arriving flight is announced in the message area.
 * If the destination airport is saturated (no gate available) the drop
 * is rejected.
 * If the data transfer fails because the network is down or the source
 * client died, or whatever reason, the plane is announced to have crashed.
 *
 *
 *    Created: Vania JOLOBOFF / May 1992
 * ======================================================================
 * 
 */

#define DECLAREGLOBAL

#include "airport.h"
#include "dragsource.h"
#include "dropsite.h"

#include <Xm/XmAll.h>

/* ======================================================================
 *  Constants
 */

#define MIN_TRACK_SIZE	16 

/*
 * The flying plane icon
 */

#define flying_icon_width 30
#define flying_icon_height 31
static unsigned char flying_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00,
   0x00, 0x70, 0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 0x00, 0xe0, 0x00, 0x00,
   0x00, 0xe0, 0x00, 0x00, 0x00, 0xe0, 0x01, 0x00, 0x00, 0xe0, 0x01, 0x00,
   0x00, 0xc0, 0x03, 0x00, 0x00, 0xc0, 0x03, 0x00, 0x00, 0xc0, 0x07, 0x00,
   0x00, 0xc0, 0x0f, 0x00, 0xf8, 0xff, 0xff, 0x0f, 0xfc, 0xff, 0xff, 0x1f,
   0xfe, 0xff, 0xff, 0x1f, 0xfc, 0xff, 0xff, 0x1f, 0xf8, 0xff, 0xff, 0x0f,
   0x00, 0xc0, 0x0f, 0x00, 0x00, 0xc0, 0x07, 0x00, 0x00, 0xc0, 0x03, 0x00,
   0x00, 0xc0, 0x03, 0x00, 0x00, 0xe0, 0x01, 0x00, 0x00, 0xe0, 0x01, 0x00,
   0x00, 0xe0, 0x00, 0x00, 0x00, 0xe0, 0x00, 0x00, 0x00, 0x70, 0x00, 0x00,
   0x00, 0x70, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00};

#define flying_mask_width 30
#define flying_mask_height 31
static unsigned char flying_mask_bits[] = {
   0x00, 0x38, 0x00, 0x00, 0x00, 0x7c, 0x00, 0x00, 0x00, 0x7c, 0x00, 0x00,
   0x00, 0xf8, 0x00, 0x00, 0x00, 0xf8, 0x00, 0x00, 0x00, 0xf0, 0x01, 0x00,
   0x00, 0xf0, 0x01, 0x00, 0x00, 0xf0, 0x03, 0x00, 0x00, 0xf0, 0x03, 0x00,
   0x00, 0xe0, 0x07, 0x00, 0x00, 0xe0, 0x07, 0x00, 0x00, 0xe0, 0x0f, 0x00,
   0xf8, 0xff, 0xff, 0x0f, 0xfc, 0xff, 0xff, 0x1f, 0xfe, 0xff, 0xff, 0x3f,
   0xff, 0xff, 0xff, 0x3f, 0xfe, 0xff, 0xff, 0x3f, 0xfc, 0xff, 0xff, 0x1f,
   0xf8, 0xff, 0xff, 0x0f, 0x00, 0xe0, 0x0f, 0x00, 0x00, 0xe0, 0x07, 0x00,
   0x00, 0xe0, 0x07, 0x00, 0x00, 0xf0, 0x03, 0x00, 0x00, 0xf0, 0x03, 0x00,
   0x00, 0xf0, 0x01, 0x00, 0x00, 0xf0, 0x01, 0x00, 0x00, 0xf8, 0x00, 0x00,
   0x00, 0xf8, 0x00, 0x00, 0x00, 0x7c, 0x00, 0x00, 0x00, 0x7c, 0x00, 0x00,
   0x00, 0x38, 0x00, 0x00};

/*
 * The parked plane icon
 */

static unsigned char plane_bits[] = {
   0x07, 0x00, 0x00, 0x00, 0x1f, 0x00, 0x00, 0x00, 0x3f, 0x00, 0x00, 0x00,
   0x7e, 0x00, 0x00, 0x00, 0xfe, 0x00, 0x00, 0x00, 0xfc, 0xff, 0xff, 0x01,
   0xf8, 0xff, 0xff, 0x01, 0xf0, 0xff, 0xff, 0x01, 0xe0, 0xff, 0xff, 0x00,
   0xe0, 0xff, 0x7f, 0x00, 0xe0, 0xff, 0x0f, 0x00, 0xe0, 0xff, 0x00, 0x00,
   0xe0, 0xff, 0x00, 0x00, 0xe0, 0xff, 0x01, 0x00, 0xe0, 0xff, 0x03, 0x00,
   0xe0, 0xff, 0x07, 0x00, 0xe0, 0xe7, 0x0f, 0x00, 0xe0, 0xc7, 0x1f, 0x00,
   0xe0, 0x87, 0x3f, 0x00, 0xe0, 0x07, 0x7f, 0x00, 0xe0, 0x03, 0xfe, 0x00,
   0xe0, 0x03, 0xfc, 0x01, 0xe0, 0x03, 0xf8, 0x01, 0xe0, 0x01, 0xf0, 0x01,
   0xe0, 0x00, 0xe0, 0x01};

static XtResource AirportResourcesSpecs[] =
{
   { "tooSmall", XtCString, XtRString, sizeof(String),
       XtOffsetOf( struct _AirportResources, too_small), XtRImmediate, 
       (XtPointer) "Window too small, enlarge please" },
   { 
      "trackBackground", XmCBackground, XmRPixel, sizeof (Pixel),
      XtOffsetOf( struct _AirportResources, track_background),
      XtRString, (XtPointer)"XtDefaultBackground"
     },
   {
      "spotBackground", XmCBackground, XmRPixel, sizeof (Pixel),
      XtOffsetOf( struct _AirportResources, spot_background),
      XtRString, (XtPointer)"XtDefaultBackground"
     },
   {	
      "flyingBackground", XmCBackground, XmRPixel, sizeof (Pixel),
      XtOffsetOf( struct _AirportResources, flight_background),
      XtRString, (XtPointer)"XtDefaultBackground"
  },
   {
      "dragUnderBackground", XmCBackground, XmRPixel, sizeof(Pixel),
      XtOffsetOf( struct _AirportResources, drag_under_background),
      XtRString, (XtPointer)"XtDefaultForeground"
    },
   {
      "trackForeground", XmCForeground, XmRPixel, sizeof(Pixel),
      XtOffsetOf( struct _AirportResources, track_foreground),
      XtRString,(XtPointer)"XtDefaultForeground"
    },
   {
      "dragUnderForeground", XmCForeground, XmRPixel, sizeof(Pixel),
      XtOffsetOf( struct _AirportResources, drag_under_foreground),
      XtRString,(XtPointer)"XtDefaultForeground"
    },
   {
      "spotForeground", XmCForeground, XmRPixel, sizeof (Pixel),
      XtOffsetOf( struct _AirportResources, spot_foreground),
      XtRString,(XtPointer)"XtDefaultForeground"
    },
   {
      "flyingForeground", XmCForeground, XmRPixel, sizeof (Pixel),
      XtOffsetOf( struct _AirportResources, flight_foreground),
      XtRString,(XtPointer)"XtDefaultForeground"
    },
   {
      "invalidForeground", XmCForeground, XmRPixel, sizeof (Pixel),
      XtOffsetOf( struct _AirportResources, invalid_foreground),
      XtRString,(XtPointer)"XtDefaultForeground"
    },
   {
      "validForeground", XmCHighlightColor, XmRPixel, sizeof (Pixel),
      XtOffsetOf( struct _AirportResources, valid_foreground),
      XtRString,(XtPointer)"XtDefaultForeground"
    },
   {
      "noneForeground", XmCHighlightColor, XmRPixel, sizeof (Pixel),
      XtOffsetOf( struct _AirportResources, none_foreground),
      XtRString,(XtPointer)"XtDefaultForeground"
    },
   {
     "trackBorderWidth", XmCBorderWidth, XmRDimension, sizeof(Dimension),
     XtOffsetOf( struct _AirportResources, track_border_width),
     XmRImmediate, (XtPointer) 5
    },
   {
      "trackMargin", XmCDimension, XmRDimension, sizeof(Dimension),
      XtOffsetOf( struct _AirportResources, track_margin),
      XmRImmediate, (XtPointer)  16
   },
   {
      "bellDuration", XmCMaxValue, XmRInt, sizeof(int),
      XtOffsetOf( struct _AirportResources, bell_duration),
      XmRImmediate, (XtPointer)  100
   },
   {
      "bellPercent", XmCMaxValue, XmRInt, sizeof(int),
      XtOffsetOf( struct _AirportResources, bell_percent),
      XmRImmediate, (XtPointer) 100
   }
};
