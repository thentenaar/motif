/* $XConsortium: Notebook6.h /main/5 1995/07/15 21:07:53 drk $ */

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


/* This is the header file for the test PNotebook1.c */



/* Mechanism used to get the creation function associated with a widget. */

typedef struct { 
	         char *name;
		 Widget (*CreateFunction) (Widget, char *, ArgList, Cardinal);
	       } WidgetItem;





WidgetItem WidgetStruct[] =  {
                   "XmArrowButton", XmCreateArrowButton,   
                   "XmArrowButtonGadget", XmCreateArrowButtonGadget, 
		   "XmBulletinBoard", XmCreateBulletinBoard,
		   "XmDrawnButton", XmCreateDrawnButton,   
		   "XmDrawingArea", XmCreateDrawingArea,
                   "XmFileSelectionBox",XmCreateFileSelectionBox,
                   "XmForm", XmCreateForm,
		   "XmFrame", XmCreateFrame,
		   "XmLabel", XmCreateLabel,  
		   "XmLabelGadget", XmCreateLabelGadget,   
                   "XmList", XmCreateList  ,
		   "XmMainWindow", XmCreateMainWindow,
                   "XmMessageBox", XmCreateMessageBox,   
		   "XmPanedWindow",XmCreatePanedWindow,
                   "XmPushButton",   XmCreatePushButton,
		   "XmPushButtonGadget", XmCreatePushButtonGadget,  
		   "XmRowColumn", XmCreateRowColumn,
		   "XmScale", XmCreateScale,
                   "XmScrollBar", XmCreateScrollBar,  
		   "XmScrolledText", XmCreateScrolledText,
                   "XmSelectionBox", XmCreateSelectionBox,  
		   "XmSeparator", XmCreateSeparator,                     
                   "XmSeparatorGadget",XmCreateSeparatorGadget, 
		   "XmText", XmCreateText,
                   "XmTextField", XmCreateTextField,   
		   "XmToggleButton", XmCreateToggleButton,   
                   "XmToggleButtonGadget", XmCreateToggleButtonGadget,  
		   "XmScrolledWindow", XmCreateScrolledWindow
		   };

