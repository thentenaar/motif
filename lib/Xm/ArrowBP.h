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
#ifndef _XmArrowButtonP_h
#define _XmArrowButtonP_h

#include <Xm/ArrowB.h>
#include <Xm/PrimitiveP.h>

#ifdef __cplusplus
extern "C" {
#endif

/*  Arrow class structure  */

typedef struct _XmArrowButtonClassPart
{
  XtPointer extension;
} XmArrowButtonClassPart;


/*  Full class record declaration for Arrow class  */

typedef struct _XmArrowButtonClassRec
{
  CoreClassPart	  	 core_class;
  XmPrimitiveClassPart	 primitive_class;
  XmArrowButtonClassPart arrowbutton_class;
} XmArrowButtonClassRec;

externalref XmArrowButtonClassRec xmArrowButtonClassRec;


/*  The ArrowButton instance record  */

typedef struct _XmArrowButtonPart
{
  XtCallbackList activate_callback;
  XtCallbackList arm_callback;
  XtCallbackList disarm_callback;
  unsigned char  direction;	/* The direction the arrow is pointing. */

  Boolean 	 selected;
  short          top_count;
  short          cent_count;
  short          bot_count;
  XRectangle    *top;
  XRectangle    *cent;
  XRectangle    *bot;

  GC		 arrow_GC;
  XtIntervalId   timer;	
  unsigned char  multiClick;	/* KEEP/DISCARD resource */
  int            click_count;
  Time		 armTimeStamp;
  GC		 insensitive_GC;
  Dimension detail_shadow_thickness ;
} XmArrowButtonPart;


/*  Full instance record declaration  */

typedef struct _XmArrowButtonRec
{
  CorePart	   	core;
  XmPrimitivePart	primitive;
  XmArrowButtonPart	arrowbutton;
} XmArrowButtonRec;


/********    Private Function Declarations    ********/


/********    End Private Function Declarations    ********/


#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmArrowButtonP_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
