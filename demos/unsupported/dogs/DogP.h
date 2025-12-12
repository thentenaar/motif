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
/*   $XConsortium: DogP.h /main/5 1995/07/14 10:06:06 drk $ */

/*****************************************************************************
*
*  DogP.H - widget private header file
*  
******************************************************************************/

#ifndef _DogP_h
#define _DogP_h

#include <Xm/PrimitiveP.h>
#include "Dog.h"

void _DogDrawPixmap();
void _DogPosition();

#define DogIndex (XmPrimitiveIndex + 1)

typedef struct _DogClassPart {
    XtPointer reserved;
} DogClassPart;

typedef struct _DogClassRec {
    CoreClassPart core_class;
    XmPrimitiveClassPart primitive_class;
    DogClassPart dog_class;
} DogClassRec;

extern DogClassRec dogClassRec;

typedef struct _DogPart {
    int wag_time;
    int bark_time;
    XtCallbackList bark_callback;

    Boolean wagging;
    Boolean barking;
    GC draw_GC;
    Pixmap up_pixmap;
    Pixmap down_pixmap;
    Pixmap bark_pixmap;
    Position pixmap_x;
    Position pixmap_y;
    Position draw_x;
    Position draw_y;
    Dimension draw_width;
    Dimension draw_height;
    int curr_px;
    Pixmap curr_pixmap;
    Dimension curr_width;
    Dimension curr_height;
    XtIntervalId wagId;
    XtIntervalId barkId;
} DogPart;

typedef struct _DogRec {
    CorePart core;
    XmPrimitivePart primitive;
    DogPart dog;
} DogRec;

#define UpPx 0
#define DownPx 1
#define BarkPx 2

#endif /* _DogP_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
