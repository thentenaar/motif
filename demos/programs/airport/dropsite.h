/* $XConsortium: dropsite.h /main/5 1995/07/15 20:44:46 drk $ */
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
 * Airport Drop Site / Receiver Header File
 * The drop site module contains all functions that relate to the
 * drop site operations.
 * 
 * The MakeAirportDropSite functions registers the airport as a drop site.
 *
 * The drag callback sets the toggle button
 * corresponding to current state, in the radio box of incoming flights.
 *
 * When something is flying over the airport, change the color of the track
 *
 * When the drop is finished
 *	if successful announce that it has landed
 *	if it failed announce the plane returned
 *
 *    Created: Vania JOLOBOFF / May 1992
 *
 */

#include "airport.h"

/*
 *  Declare the function that will start dragging a plane icon.
 */

void MakeAirportDropSite(Widget area, Airport this);
