/* $XConsortium: DrArrow.c /main/6 1995/10/25 19:59:56 cde-sun $ */
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
 *
 */
/*
 * HISTORY
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include "XmI.h"
#include <Xm/DrawP.h>

/****************************XmeDrawArrow**********************************/
void XmeDrawArrow(Display *display, Drawable d,
                  GC top_gc, GC bot_gc, GC cent_gc,
                  Position x, Position y,
                  Dimension width, Dimension height, Dimension shadow_thick,
                  unsigned char direction)
{
   /* cent_gc might be NULL, which means don't draw anything on the center,
      but if shadow_thick is 1, then center is not NULL, see in ArrowB */
   /* in the current implementation, on shadow_thick = 2, 1 or 0 supported */

   static unsigned int allocated = 0;
   static XRectangle * top  = NULL;
   static XRectangle * cent = NULL;
   static XRectangle * bot  = NULL;
   XRectangle * rect_tmp;
   int size, xOffset = 0, yOffset = 0, wwidth, start;
   register int temp, yy, i, h, w;
   short t = 0 , b = 0 , c = 0 ;
   XtAppContext app;

   if (!d) return ;

   app = XtDisplayToApplicationContext(display);
   _XmAppLock(app);

   /*  Get the size and the position and allocate the rectangle lists  */

   if (width > height) {
      size = height - 2;
      xOffset = (width - height) / 2;
   } else {
      size = width - 2 ;
      yOffset = (height - width) / 2;
   }
   if (size < 1) { _XmAppUnlock(app); return; }
   if (allocated < size) {
      _XmProcessLock();
      top  = (XRectangle *) XtRealloc ((char*)top,
                                       sizeof (XRectangle) * (size/2+6));
      cent = (XRectangle *) XtRealloc ((char*)cent,
                                       sizeof (XRectangle) * (size/2+6));
      bot  = (XRectangle *) XtRealloc ((char*)bot,
                                       sizeof (XRectangle) * (size/2+6));
      allocated = size;
      _XmProcessUnlock();
   }

#define SWAP(x,y) temp = x ; x = y; y = temp

   if (direction == XmARROW_RIGHT || direction == XmARROW_LEFT) {
      SWAP(xOffset,yOffset) ;
   }

   /*  Set up a loop to generate the segments.  */

   wwidth = size;
   yy = size - 1 + yOffset;
   start = 1 + xOffset;

   _XmProcessLock();
   while (wwidth > 0) {
       if (wwidth == 1) {
           top[t].x = start; top[t].y = yy + 1;
           top[t].width = 1; top[t].height = 1;
           t++;
       }
       else if (wwidth == 2) {
           if (size == 2 || (direction == XmARROW_UP ||
                             direction == XmARROW_LEFT)) {
               top[t].x = start; top[t].y = yy;
               top[t].width = 2; top[t].height = 1;
               t++;
               top[t].x = start; top[t].y = yy + 1;
               top[t].width = 1; top[t].height = 1;
               t++;
               bot[b].x = start + 1; bot[b].y = yy + 1;
               bot[b].width = 1; bot[b].height = 1;
               b++;
           }
       }
       else {
           if (start == 1 + xOffset)
               {
                   if (direction == XmARROW_UP || direction == XmARROW_LEFT) {
                       top[t].x = start; top[t].y = yy;
                       top[t].width = 2; top[t].height = 1;
                       t++;
                       top[t].x = start; top[t].y = yy + 1;
                       top[t].width = 1; top[t].height = 1;
                       t++;
                       bot[b].x = start + 1; bot[b].y = yy + 1;
                       bot[b].width = 1; bot[b].height = 1;
                       b++;
                       bot[b].x = start + 2; bot[b].y = yy;
                       bot[b].width = wwidth - 2; bot[b].height = 2;
                       b++;
                   }
                   else {
                       top[t].x = start; top[t].y = yy;
                       top[t].width = 2; top[t].height = 1;
                       t++;
                       bot[b].x = start; bot[b].y = yy + 1;
                       bot[b].width = 2; bot[b].height = 1;
                       b++;
                       bot[b].x = start + 2; bot[b].y = yy;
                       bot[b].width = wwidth - 2; bot[b].height = 2;
                       b++;
                   }
               }
           else {
               top[t].x = start; top[t].y = yy;
               top[t].width = 2; top[t].height = 2;
               t++;
               bot[b].x = start + wwidth - 2; bot[b].y = yy;
               bot[b].width = 2; bot[b].height = 2;
               if (wwidth == 3) {
                   bot[b].width = 1;
                   bot[b].x += 1;
               }
               b++;
               if (wwidth > 4) {
                   cent[c].x = start + 2; cent[c].y = yy;
                   cent[c].width = wwidth - 4; cent[c].height = 2;
                   c++;
               }
           }
       }
       start++;
       wwidth -= 2;
       yy -= 2;
   }

   if (direction == XmARROW_DOWN || direction == XmARROW_RIGHT) {
       rect_tmp = top; top = bot; bot = rect_tmp;
       SWAP(t, b);
   }


   /*  Transform the "up" pointing arrow to the correct direction  */

   switch (direction) {
   case XmARROW_LEFT:
       i = -1;
       do {
           i++;
           if (i < t) {
               SWAP(top[i].y, top[i].x);
               SWAP(top[i].width, top[i].height);
           }
           if (i < b) {
               SWAP(bot[i].y, bot[i].x);
               SWAP(bot[i].width, bot[i].height);
           }
           if (i < c) {
               SWAP(cent[i].y, cent[i].x);
               SWAP(cent[i].width, cent[i].height);
           }
       } while (i < t || i < b || i < c);
       break;

   case XmARROW_RIGHT:
       h = height - 2;
       w = width - 2;
       i = -1;
       do {
           i++;
           if (i < t) {
               SWAP(top[i].y, top[i].x);
               SWAP(top[i].width, top[i].height);
               top[i].x = w - top[i].x - top[i].width + 2;
               top[i].y = h - top[i].y - top[i].height + 2;
           }
           if (i < b) {
               SWAP(bot[i].y, bot[i].x);
               SWAP(bot[i].width, bot[i].height);
               bot[i].x = w - bot[i].x - bot[i].width + 2;
               bot[i].y = h - bot[i].y - bot[i].height + 2;
           }
           if (i < c) {
               SWAP(cent[i].y, cent[i].x);
               SWAP(cent[i].width, cent[i].height);
               cent[i].x = w - cent[i].x - cent[i].width + 2;
               cent[i].y = h - cent[i].y - cent[i].height + 2;
           }
       } while (i < t || i < b || i < c);
       break;

   case XmARROW_DOWN:
       w = width - 2;
       h = height - 2;
       i = -1;
       do {
           i++;
           if (i < t) {
               top[i].x = w - top[i].x - top[i].width + 2;
               top[i].y = h - top[i].y - top[i].height + 2;
           }
           if (i < b) {
               bot[i].x = w - bot[i].x - bot[i].width + 2;
               bot[i].y = h - bot[i].y - bot[i].height + 2;
           }
           if (i < c) {
               cent[i].x = w - cent[i].x - cent[i].width + 2;
               cent[i].y = h - cent[i].y - cent[i].height + 2;
           }
       } while (i < t || i < b || i < c);
       break;
   }

   if (x != 0 || y != 0) {
       for (i = 0; i < t; i++) {
           top[i].x += x;
           top[i].y += y;
       }
       for (i = 0; i < c; i++) {
           cent[i].x += x;
           cent[i].y += y;
       }
       for (i = 0; i < b; i++) {
           bot[i].x += x;
           bot[i].y += y;
       }
   }

   if (shadow_thick) {  /* 1 or 2 shadow thickness: always draw
			   2 thickness at that point, we'll correct it
			   later */
       XFillRectangles (display, d, top_gc, top, t);
       XFillRectangles (display, d, bot_gc, bot, b);
   } else {
       /* handle the case where arrow shadow_thickness = 0, which give
          a flat arrow: draw the shadow area with the center color */
       if (cent_gc) {
           XFillRectangles (display, d, cent_gc, top, t);
           XFillRectangles (display, d, cent_gc, bot, b);
       }
   }

   if (shadow_thick == 1) {
       /* we already drawn the shadow of 2, now let's draw a
	  bigger center area: ask for a smaller arrow with
	  flat look */
       XmeDrawArrow(display, d, top_gc, bot_gc, cent_gc,
		    x+1, y+1, width-2, height-2, 0, direction) ;
   } else
   if (cent_gc) XFillRectangles (display, d, cent_gc, cent, c);
   _XmProcessUnlock();
   _XmAppUnlock(app);
}

