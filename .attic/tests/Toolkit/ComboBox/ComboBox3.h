/* $XConsortium: ComboBox3.h /main/4 1995/07/15 21:03:08 drk $ */
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

/* This file contains declarations for ComboBox3.c. */


/* enumeration of values for XmNmatchBehavior */

enum          {
                 NONE = 0,
		 QUICK_NAVIGATE
	      };



/* enumeration of values for XmNstringDirection */
enum          {
                 STRING_DIRECTION_L_TO_R,
		 STRING_DIRECTION_R_TO_L
	      };		   


/* enumeration of values for XmNtype */
enum          {
                 COMBO_BOX,
		 DROP_DOWN_COMBO_BOX,
		 DROP_DOWN_LIST
	      };		   

/* enumberation of offsets for array of "apply" PushButtons. */


enum          {
                 SetHighlightThickness = 0,
		 SetArrowSize,
		 SetArrowSpacing,
		 SetMarginWidth,
		 SetMarginHeight,
		 SetMatchBehavior,
		 SetSelectedPosition
		 };



enum          {
                 GetHighlightThickness = 0,
		 GetArrowSize,
		 GetArrowSpacing,
		 GetMarginWidth,
		 GetMarginHeight,
		 GetMatchBehavior,
		 GetSelectedPosition
		 };


enum          {
                 HighlightThickness = 0,
		 ArrowSize,
		 ArrowSpacing,
		 MarginWidth,
		 MarginHeight,
		 SelectedPosition
		 };


enum          {
                 Width = 0,
		 Height,
		 X,
		 Y,
		 BorderWidth
		 };


/* enumeration of client_data parameters for setting and getting values */


enum             {
                   SET,
		   GET
		 };















