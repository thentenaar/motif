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
#ifndef _XmTextStrSoP_h
#define _XmTextStrSoP_h

#include <Xm/XmP.h>
#include <Xm/Text.h>

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************
 *
 * Definitions for use by sources and source users.
 *
 ****************************************************************/

typedef struct _XmSourceDataRec {
  XmTextSource source;	/* Backpointer to source record. */
  XmTextWidget *widgets;	/* Array of widgets displaying this source. */
  XmTextPosition left, right; /* Left and right extents of selection. */
  char * ptr;			/* Actual string data. */
  char * value;		/* Value of the string data. */
  char * gap_start;		/* Gapped buffer start pointer */
  char * gap_end;		/* Gapped buffer end pointer */
  char * PSWC_NWLN;           /* Holder for char*, short*, int* rep of NWLN */
  int length;			/* Number of chars of data. */
  int maxlength;		/* Space allocated. */
  int old_length;		/* Space allocated for value pointer. */
  int numwidgets;		/* Number of entries in above. */
  int maxallowed;		/* The user is not allowed to grow source */
  /* to a size greater than this. */
  Time prim_time;             /* time of primary selection */
  Boolean hasselection;	/* Whether we own the selection. */
  Boolean editable;		/* Whether we allow any edits. */
  Boolean take_selection;	/* Whether we should take the selection. */
} XmSourceDataRec, *XmSourceData;

typedef void (*AddWidgetProc)(XmTextSource,
			      XmTextWidget);

typedef int (*CountLinesProc)(XmTextSource,
			      XmTextPosition,
			      unsigned long);

typedef void (*RemoveWidgetProc)(XmTextSource,
				 XmTextWidget);

typedef XmTextPosition (*ReadProc)(XmTextSource,
				   XmTextPosition,	/* starting position */
				   XmTextPosition,	/* The last position
							   we're interested in.
							   Don't return info
							   about any later
							   positions. */
				   XmTextBlock);	/* RETURN: text read */

typedef XmTextStatus (*ReplaceProc)(XmTextWidget,
				    XEvent *,
				    XmTextPosition *,
				    XmTextPosition *,
				    XmTextBlock,
                                    Boolean);

typedef XmTextPosition (*ScanProc)(XmTextSource,
				   XmTextPosition,
				   XmTextScanType,
				   XmTextScanDirection,	/*XmsdLeft/XmsdRight*/
				   int,
    		                   Boolean);

typedef Boolean (*GetSelectionProc)(XmTextSource,
                                    XmTextPosition *,
                                    XmTextPosition *);

typedef void (*SetSelectionProc)(XmTextSource,
				 XmTextPosition,
				 XmTextPosition,
				 Time);


typedef struct _XmTextSourceRec {
  struct _XmSourceDataRec *data;   /* Source-defined data (opaque type). */
  AddWidgetProc	AddWidget;
  CountLinesProc	CountLines;
  RemoveWidgetProc	RemoveWidget;
  ReadProc		ReadSource;
  ReplaceProc		Replace;
  ScanProc		Scan;
  GetSelectionProc	GetSelection;
  SetSelectionProc	SetSelection;
} XmTextSourceRec;


#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /*  _XmTextStrSoP_h */
