/* $XConsortium: dragsource.h /main/5 1995/07/15 20:44:38 drk $ */
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
 * Airport Drag Source / Initiator Header File
 * The dragsource module contains all functions that relate to the
 * drag operation.
 * 
 * The TakeOff functions starts the drag and creates the drag context.
 *
 * Most callbacks are one line source code to set the toggle button
 * corresponding to current state, in the radio box of outcoming flights.
 *
 * When the plane is flying over any drop site do audio feed back (beep)
 *
 * When the plane is flying over an airport, in addition change the
 * color of the plane icon (default is Pink)
 *
 * When the drop is finished
 *	if successful announce that it has landed
 *	if it failed announce the plane returned
 *
 *    Created: Vania JOLOBOFF / May 1992
 *
 */

/*
 *  Declare the function that starts dragging a plane icon.
 */

void TakeOff(Widget area, Airport this, XButtonEvent *evt, int spot);
