/* $XConsortium: dainput_dlg.c /main/5 1995/07/15 20:47:28 drk $ */
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
 * 
 */
/*
 * HISTORY
 */
#include <Xm/XmP.h> /* for XmeGetDefaultRenderTable */
#include <Xm/XmIm.h>
#include <stdlib.h>
#include "dainput.h"

typedef struct _DaTextDataRec {
  char        * text;		/* Contains the (multibyte) text */
  long          text_length;	/* Current number of bytes displayed */
  long          alloced_length;	/* Allocated nuber of bytes */
  XFontStruct * font;		/* Font for displaying text */
  XFontSet      font_set;	/* Font set for displaying text - takes
				   precedence over font */
  GC            gc;		/* GC for drawing text */
  Dimension     baseline;	/* Baseline for displayed text */
  Dimension     lineheight;	/* line height for font or font set */
  Dimension     cursor_pos;	/* X position for insertion cursor */
} DaTextDataRec, *DaTextData;

/****************************************************************
 * GetGC:
 *   Get a gc for drawing text and lines in the drawing area.
 ****************************************************************/
static void
GetGC(Widget widget,
      DaTextData text_data)
{
  Pixel foreground;
  XtGCMask value_mask = GCForeground|GCLineWidth;
  XGCValues values;

  XtVaGetValues(widget, 
		XmNforeground, &foreground, 
		NULL);
  values.foreground = foreground;
  values.line_width = 1;

  if (text_data->font) {
    values.font = text_data->font->fid;
    value_mask |= GCFont;
  }
    
  text_data->gc = XtGetGC(widget, value_mask, &values);
}


/****************************************************************
 * GetFont:
 *   Get a font for drawing text. Gets the default text render
 *   table, and extract the font or fontset from the rendition
 *   tagged XmFONTLIST_DEFAULT_TAG. Use the first font set or font
 *   found if there is no such rendition. Also fills in baseline
 *   and lineheight values.
 ****************************************************************/
static void 
GetFont(Widget widget,
	DaTextData text_data)
{
  XmRenderTable rt;
  XmFontContext context;
  XmFontListEntry next_entry;
  XmFontType type_return = XmFONT_IS_FONT;
  XtPointer tmp_font;
  Boolean have_font_struct = False;
  Boolean have_font_set = False;
  char* font_tag = NULL;
  
  rt = XmeGetDefaultRenderTable(widget, XmTEXT_FONTLIST);
  
  if (!XmFontListInitFontContext(&context, rt))
    return;
  
  do {
    next_entry = XmFontListNextEntry(context);
    if (next_entry) {
      tmp_font = XmFontListEntryGetFont(next_entry, &type_return);
      font_tag = XmFontListEntryGetTag(next_entry);
      if (strcmp(XmFONTLIST_DEFAULT_TAG, font_tag) == 0) {
	/* If there is a real font in this font entry, we're done */
	if (type_return == XmFONT_IS_FONTSET) {
	  text_data->font_set = (XFontSet)tmp_font;
	  text_data->font = NULL;
	  break;
	} else if (type_return == XmFONT_IS_FONT) {
	  text_data->font = (XFontStruct *)tmp_font;
	  text_data->font_set = NULL;
	  break;
	}
      } else {
	/* save the first one in case no font is found 
	 * in default entry. Font sets prevail over fonts */
	if (type_return == XmFONT_IS_FONTSET) {
	  if (!have_font_set) {
	    text_data->font_set = (XFontSet)tmp_font;
	    have_font_set = True;	  
	  }
	} else if (type_return == XmFONT_IS_FONT) {
	  if (!have_font_struct && !have_font_set) {
	    text_data->font = (XFontStruct*)tmp_font;
	    have_font_struct = True;
	  }
	}
      }
      if (font_tag) XtFree(font_tag);
    }
  } while (next_entry != NULL);
  
  XmFontListFreeFontContext(context);
  
  if (text_data->font_set) {
    XFontSetExtents *fs_extents;
    fs_extents = XExtentsOfFontSet(text_data->font_set);
    text_data->baseline = -fs_extents->max_logical_extent.y;
    text_data->lineheight = fs_extents->max_logical_extent.height;
  } else if (text_data->font) {
    text_data->baseline = text_data->font->max_bounds.ascent;
    text_data->lineheight = (text_data->font->max_bounds.ascent +
			     text_data->font->max_bounds.descent);
  }
}

/****************************************************************
 * DaExit:
 *   Exit application.
 ****************************************************************/
/*CCB*/
void
DaExit(Widget widget,
       XtPointer client_data,
       XtPointer calldata)
{
  exit(0);
}

/****************************************************************
 * DaOverView:
 *   Show help information.
 ****************************************************************/
/*CCB*/
void
DaOverView(Widget widget,
	   XtPointer client_data,
	   XtPointer calldata)
{
   /* Not yet implemented */
}

/****************************************************************
 * DaRedrawText:
 *   Draw a line one pixel below the text baseline.
 *   Draw the text.
 *   Draw a cursor marking where the next character typed will be 
 *   drawn.
 ****************************************************************/
/*CCB*/
void
DaRedrawText(Widget widget,
	     XtPointer client_data,
	     XtPointer calldata)
{
   DaTextData text_data = NULL;
   Dimension margin_width, margin_height, width;
   XPoint points[3];

   XtVaGetValues(widget, 
		 XmNuserData, &text_data, 
		 XmNmarginWidth, &margin_width,
		 XmNmarginHeight, &margin_height,
		 XmNwidth, &width,
		 NULL);

   if (text_data == NULL) {
     /* First time through - allocate the extra data we need */
     text_data = (DaTextData)XtCalloc(1, sizeof(DaTextDataRec));
     text_data->alloced_length = 20;
     text_data->text = XtMalloc(text_data->alloced_length);
     GetFont(widget, text_data);
     GetGC(widget, text_data);
     text_data->cursor_pos = margin_width;
     XtVaSetValues(widget, XmNuserData, text_data, NULL);
   }
   
   /* Draw baseline */
   XDrawLine(XtDisplay(widget), XtWindow(widget),
	     text_data->gc, 
	     margin_width, text_data->baseline + margin_height + 1, 
	     width - margin_width, text_data->baseline + margin_height + 1);
   
   /* If there is text, draw it */
   if (text_data->text_length > 0) {
     if (text_data->font_set) {
       XmbDrawString(XtDisplay(widget), XtWindow(widget), 
		     text_data->font_set, text_data->gc, 
		     margin_width, text_data->baseline + margin_height, 
		     text_data->text, text_data->text_length);
     } else if (text_data->font) {
       XDrawString(XtDisplay(widget), XtWindow(widget),
		   text_data->gc, 
		   margin_width, text_data->baseline + margin_height, 
		   text_data->text, text_data->text_length);
     }
   } 
   
   /* Draw the cursor */
   points[0].x = text_data->cursor_pos - 3;
   points[0].y = text_data->baseline + margin_height + 3;
   points[1].x = 3;
   points[1].y = -3;
   points[2].x = 4;
   points[2].y = 4;
   XDrawLines(XtDisplay(widget), XtWindow(widget),
	      text_data->gc, points, 3, CoordModePrevious);
   (points[0].y)--;
   XDrawLines(XtDisplay(widget), XtWindow(widget),
	      text_data->gc, points, 3, CoordModePrevious);
}


/****************************************************************
 * DaInputText:
 *   If we received a keypress event, check whether any character(s)
 *   was typed. If so, add it to the text, forward the cursor 
 *   position and redraw.
 ****************************************************************/
/*CCB*/
void
DaInputText(Widget widget,
	    XtPointer client_data,
	    XtPointer calldata)
{
  DaTextData text_data = NULL;
  XmAnyCallbackStruct *cbs = (XmAnyCallbackStruct *)calldata;
  XPoint spot;
  Dimension margin_width, margin_height;
  
  if (cbs->event && cbs->event->type == KeyPress) {
    char insert_string[100];
    int insert_length;
    Status status_return;
    
    XtVaGetValues(widget, 
		  XmNuserData, &text_data, 
		  XmNmarginWidth, &margin_width,
		  XmNmarginHeight, &margin_height,
		  NULL);
  
    /* Get the characters typed, if any */
    insert_length = XmImMbLookupString(widget, (XKeyEvent *) cbs->event, 
				       insert_string, 100,
				       NULL, &status_return);

    /* if we got any text, append it to the rest */
    if (insert_length > 0 &&
	(status_return == XLookupChars || status_return == XLookupBoth)) {
       if (text_data->text_length + insert_length > 
	   text_data->alloced_length) {
	 text_data->alloced_length += insert_length + 20;
	 text_data->text = XtRealloc(text_data->text, 
				     text_data->alloced_length);
       }
       strncpy(&(text_data->text[text_data->text_length]),
	       insert_string, insert_length);
       
       text_data->text_length += insert_length;

       /* compute new cursor position */
       if (text_data->text_length > 0) {
	 if (text_data->font_set) {
	   text_data->cursor_pos = 
	     margin_width + XmbTextEscapement(text_data->font_set, 
					      text_data->text, 
					      text_data->text_length);
	 } else if (text_data->font) {
	   text_data->cursor_pos = 
	     margin_width + XTextWidth(text_data->font, 
				       text_data->text, 
				       text_data->text_length);
	 }
       } 
       
       /* Tell input method about new cursor position */
       spot.x = text_data->cursor_pos;
       spot.y = text_data->baseline + margin_height;
       
       XmImVaSetValues(widget, 
		       XmNspotLocation, &spot,
		       NULL);
     } 
    /* redraw the text. This isn't very efficient, but it does the job... */
    XClearWindow(XtDisplay(widget), XtWindow(widget));
    DaRedrawText(widget, NULL, NULL);
  }
}

/****************************************************************
 * DaResizedText:
 *   Widget was resized. Redisplay the window.
 ****************************************************************/
/*CCB*/
void
DaResizedText(Widget widget,
	      XtPointer client_data,
	      XtPointer calldata)
{
  /* redraw the text. This isn't very efficient or smart, 
     but it does the job... */
  XClearWindow(XtDisplay(widget), XtWindow(widget));
  DaRedrawText(widget, NULL, NULL);   
}

/****************************************************************
 * DaFocusHandler:
 *   Keyboard focus movement event handler. Inform the input
 *   method when we lose or gain keyboard focus.
 ****************************************************************/
void 
DaFocusHandler(Widget widget, 
	       XtPointer client_data, 
	       XEvent *event, 
	       Boolean *cont)
{
  DaTextData text_data = NULL;
  XPoint spot;
  XmRenderTable rt;
  Dimension margin_height;

  if (!event)
    return;
  
  XtVaGetValues(widget, 
		XmNuserData, &text_data, 
		XmNmarginHeight, &margin_height,
		NULL);

  switch (event->type) 
    {
    case FocusIn:
    case EnterNotify:
      /* Tell input method about our cursor position, line spacing
       * and font.
       */
      if (text_data) {
	spot.x = text_data->cursor_pos;
	spot.y = text_data->baseline + margin_height;
      } else {
	spot.x = spot.y = 10;
      }
      rt = XmeGetDefaultRenderTable(widget, XmTEXT_FONTLIST);
	
      XmImVaSetFocusValues(widget, 
			   XmNspotLocation, &spot,
			   XmNrenderTable, rt, /* since DrawingArea does
						  not have a resource */
			   XmNlineSpace, (text_data ? 
					  text_data->lineheight :
					  10),
			   NULL);
      break;
    case FocusOut:
    case LeaveNotify:
      XmImUnsetFocus(widget);
      break;
    }
}

