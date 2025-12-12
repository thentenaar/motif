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
#ifndef _XmArrowGadgetP_h
#define _XmArrowGadgetP_h

#include <Xm/ArrowBG.h>
#include <Xm/GadgetP.h>

#ifdef __cplusplus
extern "C" {
#endif

/*  Arrow class structure  */

typedef struct _XmArrowButtonGadgetClassPart
{
   XtPointer extension;
} XmArrowButtonGadgetClassPart;


/*  Full class record declaration for Arrow class  */

typedef struct _XmArrowButtonGadgetClassRec
{
  RectObjClassPart             rect_class;
  XmGadgetClassPart            gadget_class;
  XmArrowButtonGadgetClassPart arrow_button_class;
} XmArrowButtonGadgetClassRec;

externalref XmArrowButtonGadgetClassRec xmArrowButtonGadgetClassRec;

/*  The Arrow instance record  */

typedef struct _XmArrowButtonGadgetPart
{
  XtCallbackList activate_callback;
  XtCallbackList arm_callback;
  XtCallbackList disarm_callback;
  unsigned char  direction;	/* The direction the arrow is pointing. */

  Boolean	 selected;

  short		 top_count;
  short		 cent_count;
  short		 bot_count;
  XRectangle	*top;
  XRectangle	*cent;
  XRectangle	*bot;

  Position	 old_x;
  Position	 old_y;

  GC		 arrow_GC;
  XtIntervalId	 timer;	
  unsigned char	 multiClick;	/* KEEP/DISCARD resource */
  int		 click_count;
  GC		 insensitive_GC;

   
  GC		 background_GC;
  GC		 top_shadow_GC;
  GC		 bottom_shadow_GC;
  GC		 highlight_GC;
   
  Pixel		 foreground;
  Pixel		 background;
   
  Pixel		 top_shadow_color;
  Pixmap	 top_shadow_pixmap;
   
  Pixel		 bottom_shadow_color;
  Pixmap	 bottom_shadow_pixmap;
   
  Pixel		 highlight_color;
  Pixmap	 highlight_pixmap;

  Boolean	 fill_bg_box;
  Dimension detail_shadow_thickness ;
} XmArrowButtonGadgetPart;


/*  Full instance record declaration  */

typedef struct _XmArrowButtonGadgetRec
{
   ObjectPart              object;
   RectObjPart             rectangle;
   XmGadgetPart            gadget;
   XmArrowButtonGadgetPart arrowbutton;
} XmArrowButtonGadgetRec;



#define ArrowBG_BackgroundGC(w)		(((XmArrowButtonGadget)(w)) -> \
                                           arrowbutton. background_GC)
#define ArrowBG_TopShadowGC(w)		(((XmArrowButtonGadget)(w)) -> \
                                           arrowbutton. top_shadow_GC)
#define ArrowBG_BottomShadowGC(w)	(((XmArrowButtonGadget)(w)) -> \
                                           arrowbutton. bottom_shadow_GC)
#define ArrowBG_HighlightGC(w)		(((XmArrowButtonGadget)(w)) -> \
                                           arrowbutton. highlight_GC)
#define ArrowBG_Foreground(w)		(((XmArrowButtonGadget)(w)) -> \
                                           arrowbutton. foreground)
#define ArrowBG_Background(w)		(((XmArrowButtonGadget)(w)) -> \
                                           arrowbutton. background)
#define ArrowBG_TopShadowColor(w)	(((XmArrowButtonGadget)(w)) -> \
                                           arrowbutton. top_shadow_color)
#define ArrowBG_TopShadowPixmap(w)	(((XmArrowButtonGadget)(w)) -> \
                                           arrowbutton. top_shadow_pixmap)
#define ArrowBG_BottomShadowColor(w)	(((XmArrowButtonGadget)(w)) -> \
                                           arrowbutton. bottom_shadow_color)
#define ArrowBG_BottomShadowPixmap(w)	(((XmArrowButtonGadget)(w)) -> \
                                           arrowbutton. bottom_shadow_pixmap)
#define ArrowBG_HighlightColor(w)	(((XmArrowButtonGadget)(w)) -> \
                                           arrowbutton. highlight_color)
#define ArrowBG_HighlightPixmap(w)	(((XmArrowButtonGadget)(w)) -> \
                                           arrowbutton. highlight_pixmap)

/********    Private Function Declarations    ********/


/********    End Private Function Declarations    ********/


#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmArrowGadgetP_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
