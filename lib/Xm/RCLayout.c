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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#ifdef REV_INFO
#ifndef lint
static char *rcsid = "$XConsortium: RCLayout.c /main/6 1995/10/25 20:14:15 cde-sun $";
#endif
#endif

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <Xm/CascadeBGP.h>
#include <Xm/CascadeBP.h>
#include <Xm/GadgetP.h>
#include <Xm/LabelP.h>
#include <Xm/ManagerP.h>
#include <Xm/PrimitiveP.h>
#include <Xm/RowColumnP.h>
#include <Xm/TearOffBP.h>
#include <Xm/TearOffP.h>
#include <Xm/XmosP.h>		/* for bzero */
#include "LabelGI.h"
#include "GeoUtilsI.h"
#include "RCLayoutI.h"
#include "RowColumnI.h"
#include "XmI.h"

#define RESOURCE_MIN_WIDTH	16 /* 'cuz it's the size of a hot spot... */
#define RESOURCE_MIN_HEIGHT	16



static void CalcHelp(
                        XmRowColumnWidget m,
                        Dimension *m_width,
                        Dimension *m_height,
                        Dimension b,
                        Position max_x,
                        Position max_y,
                        Position *x,
                        Position *y,
                        Dimension w,
                        Dimension h) ;

static void GetMaxValues(
                        XmRowColumnWidget m,
                        Dimension *border,
                        Dimension *w,
                        Dimension *h,
                        int *items_per,
                        Dimension *baseline,
                        Dimension *shadow,
                        Dimension *highlight,
                        Dimension *margin_top,
                        Dimension *margin_height,
                        Dimension *text_height) ;

static void AdjustLast(
                        XmRowColumnWidget m,
                        int start_i,
                        Dimension w,
                        Dimension h) ;
static void SetAsking(
                        XmRowColumnWidget m,
                        Dimension *m_width,
                        Dimension *m_height,
                        Dimension b,
                        Position max_x,
                        Position max_y,
                        Position x,
                        Position y,
                        Dimension w,
                        Dimension h) ;
static void FindLargestOption(
                        XmRowColumnWidget submenu,
                        Dimension *c_width,
                        Dimension *c_height) ;
static void TopOrBottomAlignment(
                        XmRowColumnWidget m,
                        Dimension h,
                        Dimension shadow,
                        Dimension highlight,
                        Dimension baseline,
                        Dimension margin_top,
                        Dimension margin_height,
                        Dimension text_height,
                        Dimension *new_height,
                        int start_i,
                        int end_i) ;
static void BaselineAlignment(
                        XmRowColumnWidget m,
                        Dimension h,
                        Dimension shadow,
                        Dimension highlight,
                        Dimension baseline,
                        Dimension *new_height,
                        int start_i,
                        int end_i) ;
static void CenterAlignment(
                        XmRowColumnWidget m,
                        Dimension h,
                        int start_i,
                        int end_i) ;
static void ComputeTearOffHeight(
			XmRowColumnWidget m,
			Dimension *toc_b,
                        Dimension *b,
	                Dimension *toc_height,
                        int *start_i,
                        int *child_i,
			int r);
static void LayoutColumn(
                        XmRowColumnWidget m,
                        Dimension *m_width,
                        Dimension *m_height) ;
static void LayoutVerticalTight(
                        XmRowColumnWidget m,
                        Dimension *m_width,
                        Dimension *m_height) ;
static void LayoutHorizontaltight(
                        XmRowColumnWidget m,
                        Dimension *m_width,
                        Dimension *m_height) ;
static void LayoutNone(
                        XmRowColumnWidget m,
                        Dimension *m_width,
                        Dimension *m_height) ;
static void LayoutOptionAndSize(
                        register XmRowColumnWidget menu,
                        Dimension *width,
                        Dimension *height,
                        Widget instigator,
                        XtWidgetGeometry *request,
                        Boolean calcMenuDimension) ;
static void GetMenuKidMargins(
				 XmRowColumnWidget m,
				 Dimension *width,
				 Dimension *height,
				 Dimension *left,
				 Dimension *right,
				 Dimension *top,
				 Dimension *bottom );




/*************************************************************************
 *
 * This section is all the layout stuff, the whole thing has to operate
 * in two different modes, one: a read-only mode which
 * is nice for making decisions about the size of the row column vs. the size
 * of the children.  two: a change everything mode which implements the
 * change.
 *
 * further complicated by the xtoolkit restriction that a subwidget making
 * a geo request (referred to as the 'instigator') of the row column may not
 * have his resize proc called but all other widget children must.
 *
 * this is done by building a set of XtWidgetGeometry request blocks, one
 * for each child (widget and gadget), which holds the changes we would like
 * to make for this child.  If needed then another pass is made over the
 * requests to actually implement the changes.
 *************************************************************************/


/*
 * Decide where to put the help child.  He better be the last one
 * 'cuz we may trash the x, y's
 */
static void
CalcHelp(
        XmRowColumnWidget m,
        Dimension *m_width,     /* if 0 then caller's asking */
        Dimension *m_height,    /* if 0 then caller's asking */
        Dimension b,
        Position max_x,
        Position max_y,
        Position *x,
        Position *y,
        Dimension w,
        Dimension h )
{
   register Dimension subtrahend;

   if (IsVertical (m))             /* glue to bottom edge of ... */
   {
      if (Asking (*m_height))
      {
	 if (RC_NCol (m) == 1)       /* just use max_y */
	     *y = max_y;
	 else                /* go up from max_y */
	 {
	     subtrahend = RC_Spacing (m) + h + b;
	     *y = (max_y > (int)subtrahend) ? max_y - subtrahend : 0;
	 }
      }
      else
      {
	  subtrahend = MGR_ShadowThickness(m) + RC_MarginH (m) + h + b;
	  *y = (*m_height > (int)subtrahend) ? *m_height - subtrahend : 0;
      }
   }
   else                    /* glue to right edge of ... */
   {
      if (Asking (*m_width))
      {
	 if (RC_NCol (m) == 1)
	     *x = max_x;
	 else
	 {
	     subtrahend = RC_Spacing (m) + w + b;
	     *x = (max_x > (int)subtrahend) ? max_x - subtrahend : 0;
	 }
      }
      else
      {
	 subtrahend = MGR_ShadowThickness(m) + RC_MarginW (m) + w + b;
	 *x = (*m_width > (int)subtrahend) ? *m_width - subtrahend : 0;
      }
   }
}


/*
 * count the widest & tallest entry dimensions
 * and compute entries per row/column
 */
static void
GetMaxValues(
        XmRowColumnWidget m,
        Dimension *border,
        Dimension *w,
        Dimension *h,
        int *items_per,
        Dimension *baseline,
        Dimension *shadow,
        Dimension *highlight,
        Dimension *margin_top,
        Dimension *margin_height,
        Dimension *text_height )
{
    XtWidgetGeometry *b;
    Widget k ;
    int i, n ;

    *border = *w = *h = *baseline = *shadow = *highlight =
	*margin_top = *margin_height = *text_height = 0;

    /* skip the tearoff control */
    for (i = (RC_TearOffControl(m) &&
	      XtIsManaged(RC_TearOffControl(m)))? 1 : 0, n = 0;
	 RC_Boxes (m) [i].kid != NULL;
	 i++, n++) {
       b = &(RC_Boxes (m) [i].box);
       k = RC_Boxes (m) [i].kid ;

       ASSIGN_MAX(*w, BWidth (b));
       ASSIGN_MAX(*h, BHeight (b));

       if (XtIsWidget(k)) {
	   ASSIGN_MAX(*border, k->core.border_width);
       } else if (XmIsGadget(k)) {
	   ASSIGN_MAX(*border, ((XmGadget)k)->rectangle.border_width);
       }

       ASSIGN_MAX(*baseline, RC_Boxes (m) [i].baseline);

       if (XmIsGadget (k) || XmIsPrimitive (k) ) {
         XmBaselineMargins textMargins;
	 _XmRC_SetOrGetTextMargins(k, XmBASELINE_GET, &textMargins);
         ASSIGN_MAX(*shadow, textMargins.shadow);
         ASSIGN_MAX(*highlight, textMargins.shadow);
         ASSIGN_MAX(*margin_top, textMargins.margin_top);
         ASSIGN_MAX(*margin_height, textMargins.margin_height);
         ASSIGN_MAX(*text_height, textMargins.text_height);
       }
    }

    *items_per = n / RC_NCol (m);       /* calc column size */
    if ((n % RC_NCol (m)) != 0) (*items_per)++;  /* some left overs */
                                                 /* add another row/col */
}


/*
 * Make sure that entries in the right most column/row extend all the
 * way to the right/bottom edge of the row column widget.  This keeps
 * 'dead space' in the row column widget to a minimum.  For single
 * column widgets, the only column is the right most.
 *
 */
static void
AdjustLast(
        XmRowColumnWidget m,
        int start_i,
        Dimension w,
        Dimension h )
{
   XmRCKidGeometry kg = RC_Boxes (m);
   XtWidgetGeometry *b;
   register Dimension subtrahend;

   for ( ; kg [start_i].kid != NULL; start_i++)
   {
      b = &(kg[start_i].box);

      if (IsVertical (m))
      {
         subtrahend = MGR_ShadowThickness(m) + RC_MarginW (m) + BX (b)
	     + Double (BBorder (b));

	 /* if w (rowcol width) is greater than subtrahend (the smallest
	  * width of the child, we'll guarantee at least a width of 1.
	  */
	 if (w > subtrahend)
	     BWidth (b) = w-subtrahend;
      }
      else
      {
         subtrahend =  MGR_ShadowThickness(m) + RC_MarginH (m) + BY (b)
	     + Double (BBorder (b));

         /* When adjusting the last line, text and label widgets or gadgets, */
         /* use the extra height that is added differently. Text just adds  */
         /* it on whereas label tries to center it in the extra space.      */
         /* In order to make the baselines align again as a result of the   */
         /* above behavior,  Text's margin top has to be adjusted. */
	 if (h > subtrahend)
         {
             Dimension m_top;

	     /* Check for underflow */
	     /* The difference is what it grows in height */
	     m_top = ((h-subtrahend) > BHeight(b)) ?
		((h-subtrahend) - BHeight (b)) : 0 ;

	     BHeight (b) = h-subtrahend;

	     if (m_top && (XmIsText(kg [start_i].kid) ||
			   XmIsTextField(kg [start_i].kid) ||
			   XmIsCSText(kg [start_i].kid)))
             {
	       kg [start_i].margin_top += m_top/2; /* Since labels center it */
             }
         }
      }
   }
}


/*
 * decide exactly the dimensions of the row column widget we will return to
 * an asking caller based on the accumulated layout information.
 */
static void
SetAsking(
        XmRowColumnWidget m,
        Dimension *m_width,     /* if 0 then caller's asking */
        Dimension *m_height,    /* if 0 then caller's asking */
        Dimension b,
        Position max_x,
        Position max_y,
        Position x,
        Position y,
        Dimension w,
        Dimension h )
{
    long iheight;
    long iwidth;

    if (IsVertical (m))             /* tell caller what he wants */
    {
        if (Asking (*m_width))
            *m_width =   x + w + b      /* right edge of last child */
                   + MGR_ShadowThickness(m)
                   + RC_MarginW (m);    /* plus margin on right */

        if (Asking (*m_height))
        {
            ASSIGN_MAX (max_y, y);

            iheight = (long) max_y                /* last unused y */
                - (long) (RC_Spacing (m))         /* up by unused spacing */
                + (long) (MGR_ShadowThickness(m))
                + (long) (RC_MarginH (m)) ;       /* plus margin on bottom */

            if (iheight < 0)             /* this is a temporary fix */
                *m_height = 0;           /* to make sure we don't   */
            else                         /* compute a negative height */
                *m_height = (Dimension) iheight; /*in an unsigned short   */
        }
    }
    else
    {
        if (Asking (*m_width))
        {
            ASSIGN_MAX (max_x, x);

            iwidth = (long) max_x
                - (long) (RC_Spacing (m))
                + (long) (MGR_ShadowThickness(m))
                + (long) (RC_MarginW (m)) ;

            if (iwidth < 0)
                *m_width = 0;
            else
                *m_width = (Dimension) iwidth ;
        }

        if (Asking (*m_height))
            *m_height = y + h + b
                + MGR_ShadowThickness(m)
                + RC_MarginH (m);
    }
}





static void
FindLargestOption(
	XmRowColumnWidget submenu,
	Dimension *c_width,
	Dimension *c_height )
{
    int i;
    Widget *child ;

    if (!submenu) return ;

    ForManagedChildren (submenu, i, child) {
	/* Is this recursivity wanted ? */
	if (XmIsCascadeButton(*child))	{
	    FindLargestOption((XmRowColumnWidget)
			      CB_Submenu(*child),
			      c_width, c_height);
	}
	else if (XmIsCascadeButtonGadget(*child))	       {
	    FindLargestOption((XmRowColumnWidget)
			      CBG_Submenu(*child),
			      c_width, c_height);
	}
	else {
	    /* The entire size of the largest menu
	     * item is used instead of only its TextRect.  This may
	     * result in large expanses of label white space when items
	     * utilize left and right margins, shadow, or accelerator
	     * text - but the glyph will be visible when the submenu is
	     * posted!
	     */
	    if (XmIsMenuShell(XtParent(submenu))) {
		ASSIGN_MAX(*c_width, XtWidth(*child));
		ASSIGN_MAX(*c_height, XtHeight(*child));
	    }

	    /*
	     * must be a torn pane.  Don't rely on its dimensions
	     * since it may be stretched in the tear off so that
	     * the label string fits into the titlebar
	     */
	    else {
		XtWidgetGeometry preferred;

		XtQueryGeometry (*child, NULL, &preferred);
		ASSIGN_MAX(*c_width, preferred.width);
		ASSIGN_MAX(*c_height, preferred.height);

	    }
	}
    }
}



void
_XmRC_CheckAndSetOptionCascade(
      XmRowColumnWidget menu )
{
   Dimension width = 0;
   Dimension height = 0;
   int i;
   Widget cb;

   /*
    * if its is a pulldown menu, travel up the cascades to verify the
    * option menus cascade button is sized large enough.
    */
   if (IsPulldown(menu)) {
       for (i=0; i < menu->row_column.postFromCount; i++) {
	   _XmRC_CheckAndSetOptionCascade((XmRowColumnWidget)
				XtParent(menu->row_column.postFromList[i]));
       }
   }

   if (!IsOption(menu)  || RC_FromResize(menu)) return ;

   if ((cb = XmOptionButtonGadget( (Widget) menu)) != NULL) {
       if (RC_OptionSubMenu(menu)) {
	   FindLargestOption
	       ((XmRowColumnWidget)RC_OptionSubMenu(menu), &width, &height );

	   if (LayoutIsRtoLG(cb))
	     width += Double(G_HighlightThickness(cb)) +
	       G_ShadowThickness(cb) + LabG_MarginLeft(cb) +
		 Double(MGR_ShadowThickness(RC_OptionSubMenu(menu))) - 2;
	   else
	     width += Double(G_HighlightThickness(cb)) +
	       G_ShadowThickness(cb) + LabG_MarginRight(cb) +
		 Double(MGR_ShadowThickness(RC_OptionSubMenu(menu))) - 2;

	   height += Double(G_HighlightThickness(cb)) + LabG_MarginTop(cb)
	       + LabG_MarginBottom(cb);

	   /* change cb if needed */
	   if ((width != XtWidth(cb)) || (height != XtHeight(cb))) {

	       /* we have pixels, but the cascade unit type might not be
		  pixel, so save it and restore it after the setvalues */
	       unsigned char saved_unit_type =
		   ((XmGadget)cb)->gadget.unit_type ;

	       ((XmGadget)cb)->gadget.unit_type = XmPIXELS;
	       XtVaSetValues (cb, XmNwidth, width, XmNheight, height, NULL);
	       ((XmGadget)cb)->gadget.unit_type = saved_unit_type;
	   }
       }
   }
}


static void
TopOrBottomAlignment(
	XmRowColumnWidget m,
        Dimension h,
        Dimension shadow,
        Dimension highlight,
        Dimension baseline,	/* unused */
        Dimension margin_top,
        Dimension margin_height,
        Dimension text_height,
	Dimension *new_height,
        int start_i,
        int end_i)
{
  XmRCKidGeometry kg = RC_Boxes (m);

  while (start_i < end_i)
  {
    if (XmIsGadget(kg [start_i].kid) || XmIsPrimitive(kg [start_i].kid))
    {
      XmBaselineMargins textMargins;

      _XmRC_SetOrGetTextMargins(kg[start_i].kid, XmBASELINE_GET, &textMargins);
      kg[start_i].margin_top = textMargins.margin_top;
      kg[start_i].margin_bottom = textMargins.margin_bottom;

      if (textMargins.shadow < shadow)
      {
         kg[start_i].margin_top += shadow - textMargins.shadow;
         kg[start_i].box.height += shadow - textMargins.shadow;
      }
      if (textMargins.highlight < highlight)
      {
         kg[start_i].margin_top += highlight - textMargins.highlight;
         kg[start_i].box.height += highlight - textMargins.highlight;
      }
      if (textMargins.margin_top < margin_top)
      {
         kg[start_i].margin_top += margin_top - textMargins.margin_top;
         kg[start_i].box.height += margin_top - textMargins.margin_top;
      }
      if (textMargins.margin_height < margin_height)
      {
         kg[start_i].margin_top += margin_height - textMargins.margin_height;
         kg[start_i].box.height += margin_height - textMargins.margin_height;
      }
      if (AlignmentBottom (m))
       if (textMargins.text_height < text_height)
       {
         kg[start_i].margin_top += text_height - textMargins.text_height;
         kg[start_i].box.height += text_height - textMargins.text_height;
       }
      if (kg[start_i].box.height < h)
      {
	kg[start_i].margin_bottom += h - kg[start_i].box.height;
        kg[start_i].box.height = h;
      }
    }
    if (kg[start_i].box.height > h)
      if (kg[start_i].box.height > *new_height)
        *new_height = kg[start_i].box.height;
    start_i++;
  }
}

static void
BaselineAlignment(
	XmRowColumnWidget m,
        Dimension h,
        Dimension shadow,	/* unused */
        Dimension highlight,	/* unused */
        Dimension baseline,
	Dimension *new_height,
        int start_i,
        int end_i)
{
   XmRCKidGeometry kg = RC_Boxes (m);
   XmBaselineMargins textMargins;

   while (start_i < end_i)
   {
     if (XmIsPrimitive (kg [start_i].kid) || XmIsGadget (kg [start_i].kid))
     {
      unsigned char label_type;

      _XmRC_SetOrGetTextMargins(kg [start_i].kid, XmBASELINE_GET, &textMargins);
      kg[start_i].margin_top = textMargins.margin_top;
      kg[start_i].margin_bottom = textMargins.margin_bottom;
      XtVaGetValues(kg [start_i].kid, XmNlabelType, &label_type, NULL);

      if (label_type == XmSTRING)
      {
        if (kg[start_i].baseline < baseline)
        {
          kg[start_i].margin_top += baseline - kg[start_i].baseline;
          if (kg[start_i].box.height + (baseline - kg[start_i].baseline) > h)
          {
            if (kg[start_i].box.height + (baseline - kg[start_i].baseline) > *new_height)
	      *new_height = kg[start_i].box.height + (baseline - kg[start_i].baseline);
	    kg[start_i].box.height += baseline - kg[start_i].baseline;
          }
          else
          {
	    kg[start_i].margin_bottom += h  - (kg[start_i].box.height +
					       (baseline - kg[start_i].baseline));
	    kg[start_i].box.height = h;
          }
        }
        else
        {
	  kg[start_i].margin_bottom += h  - (kg[start_i].box.height +
					     (baseline - kg[start_i].baseline));
	  kg[start_i].box.height = h;
        }
      }
      else
       kg[start_i].box.height = h;
    }
    else
      kg[start_i].box.height = h;
    start_i++;
   }
}

static void
CenterAlignment(
        XmRowColumnWidget m,
        Dimension h,
        int start_i,
        int end_i)
{

  XmRCKidGeometry kg = RC_Boxes (m);

  while(start_i < end_i)
  {
    if (XmIsGadget (kg [start_i].kid) || XmIsPrimitive (kg [start_i].kid))
    {
      XmBaselineMargins textMargins;

      _XmRC_SetOrGetTextMargins(kg [start_i].kid, XmBASELINE_GET, &textMargins);
      kg[start_i].margin_top = textMargins.margin_top;
      kg[start_i].margin_bottom = textMargins.margin_bottom;
    }

    kg[start_i++].box.height = h;
  }
}


static void
ComputeTearOffHeight(
        XmRowColumnWidget m,
        Dimension *toc_b,
        Dimension *b,
	Dimension *toc_height,
        int *start_i,
        int *child_i,
        int r)

{
    XmRCKidGeometry kg = RC_Boxes (m);

    *toc_b = *b = Double (RC_EntryBorder (m));

    if (RC_TearOffControl(m) && XtIsManaged(RC_TearOffControl(m))) {
	XmTearOffButtonWidget tw = (XmTearOffButtonWidget)RC_TearOffControl(m);

	if (!RC_EntryBorder(m) && kg[0].kid && XtIsWidget(kg[0].kid))
	    *toc_b = Double(kg[0].kid->core.border_width);

	*toc_height = 0;

	/* Remember!  If toc exists, it has the  first kid geo */
	for (*start_i = 1;  kg[*start_i].kid != NULL; (*start_i)++)
	    ASSIGN_MAX(*toc_height, kg[*start_i].box.height);

	*toc_height = *toc_height >> r;    /* r is 1 or 2 depending on the
					    orientation. 1 makes the tear off
					    half the highest, 2 makes 1/4 */

	ASSIGN_MAX(*toc_height, 2 + *toc_b +  2*
	       ((XmPrimitiveWidget)kg[0].kid)->primitive.shadow_thickness);

	/* Sync up the kid geo */
	/* Fix CR# 4778 */
	if (tw -> label.recompute_size == True)
	    kg[0].box.height = *toc_height;
	else
	    kg[0].box.height = *toc_height = XtHeight(tw);
	kg[0].box.width = XtWidth(m);

	*start_i = *child_i = 1;
    }
    else
	*toc_height = *toc_b = *start_i = *child_i = 0;
}



/*
 * figure out where all the children of a column style widget go.  The
 * border widths are already set.
 *
 * In columnar mode, all heights and widths are identical.  They are the
 * size of the largest item.
 *
 * For vertical widgets the items are laid out in columns, going down the
 * first column and then down the second.  For horizonatal, think of the
 * columns as rows.
 *
 * By convention incoming row column size can be zero, indicating a request
 * for preferred size, this means lay it out and record the needed size.
 *
 * NOTE: the layout is predicated on the help child being the last one since
 * it messes up the x, y for a following child.
 */
static void
LayoutColumn(
        XmRowColumnWidget m,
        Dimension *m_width,     /* if 0 then caller's asking */
        Dimension *m_height )   /* if 0 then caller's asking */
{
    XmRCKidGeometry kg = RC_Boxes (m);
    XtWidgetGeometry *bx;
    Position x, y, max_x = 0, max_y = 0;
    int items_per_column,
        kid_i,
        child_i,                    /* which child we are doing */
        col_c   = 0,                /* items in col being done */
        start_i = 0;                /* index of first item in col */
    Dimension border, w, h, baseline, shadow, highlight,
              margin_top, margin_height, text_height;
    Dimension toc_height;
    Dimension new_height= 0;
    Dimension toc_b, b;


    ComputeTearOffHeight(m, &toc_b, &b, &toc_height, &start_i, &child_i, 1);

    /* loc of first item */
    x = MGR_ShadowThickness(m) + RC_MarginW (m);
    y = MGR_ShadowThickness(m) + RC_MarginH (m) + toc_height + toc_b;

    GetMaxValues (m, &border, &w, &h, &items_per_column, &baseline,
	      &shadow, &highlight, &margin_top, &margin_height, &text_height);

    if (!RC_EntryBorder(m) && kg[child_i].kid && XtIsWidget(kg[child_i].kid))
         b = Double(border);

    /* Loop through and find the new height, if any,  that the RowColumn */
    /* children need to grow to as a result of adjusting the baselines.  */
    /* The empty loop determine the number of kids                       */

    if (AlignmentBaselineTop(m) || AlignmentBaselineBottom(m))
    {
        kid_i = 0;
        while (kg [kid_i].kid != NULL)
	  kid_i++;
        BaselineAlignment(m, h, shadow, highlight, baseline,
			  &new_height, 0, kid_i);

    }
    else if (AlignmentTop(m) || AlignmentBottom(m))
    {
        kid_i = 0;
	while (kg [kid_i].kid != NULL)
	  kid_i++;
        TopOrBottomAlignment(m, h, shadow, highlight,
			     baseline, margin_top, margin_height,
			     text_height, &new_height, 0, kid_i);
    }
    else if (AlignmentCenter(m))
    {
        kid_i = 0;
	while (kg [kid_i].kid != NULL)
	  kid_i++;
        CenterAlignment(m, h, start_i, kid_i);
    }

    if (!new_height) new_height = h;

    for (; kg [child_i].kid != NULL; child_i++)
    {
        bx = &(kg[child_i].box);

        BWidth  (bx) = w;           /* all have same dimensions */

        if (AlignmentCenter(m))
         BHeight(bx) = h;

        if (++col_c > items_per_column)     /* start a new column */
        {
	   if (IsVertical (m))         /* calc loc of new column */
	   {
	      x += w + b + RC_Spacing (m);    /* to the right */

	      /*back at top of menu */
	      y = MGR_ShadowThickness(m) + RC_MarginH (m) + toc_height + toc_b;
	   }
	   else                /* calc loc of new row */
	   {
	      /* back to left edge */
	      x = MGR_ShadowThickness(m) + RC_MarginW (m);
	      /* down a row */
	      y += new_height + b + RC_Spacing (m);
	   }

	   col_c = 1;              /* already doing this one */
	   start_i = child_i;          /* record index */
        }

        if (IsHelp (m, ((Widget) kg[child_i].kid)))
            CalcHelp (m, m_width, m_height, b, max_x, max_y,
		       &x, &y, w, new_height);

        SetPosition (bx, x, y);         /* plunk him down */

        if (IsVertical (m))         /* get ready for next item */
            y += new_height + b + RC_Spacing (m);
        else
            x += w + b + RC_Spacing (m);

        ASSIGN_MAX (max_y, y);
	ASSIGN_MAX (max_x, x); /* record for use later */
     }

     if (new_height > h) {
        for(kid_i = 0; kid_i < child_i; kid_i++) {
          bx = &(kg[kid_i].box);
          if (BHeight(bx) != new_height) {
	    kg[kid_i].margin_bottom += new_height - kg[kid_i].box.height;
	    BHeight(bx) = new_height;
          }
        }
     }

    SetAsking (m, m_width, m_height, b, max_x, max_y, x, y, w, new_height);


/* Set toc width to the width of the pane */
/* declare a macro and use it in the next 3 routines */
#define SET_TEAR_OFF_BOX(toc_height) \
   if (toc_height)    {\
       kg[0].box.x = MGR_ShadowThickness(m) + RC_MarginW (m);\
       kg[0].box.y = MGR_ShadowThickness(m) + RC_MarginH (m);\
       kg[0].box.height = toc_height;\
       kg[0].box.width = *m_width - Double(MGR_ShadowThickness(m) + \
       RC_MarginW(m)) - toc_b; \
    }

    SET_TEAR_OFF_BOX(toc_height);

    if (RC_AdjLast(m))
        AdjustLast (m, start_i, *m_width, *m_height);
    if (LayoutIsRtoLM(m))
        for (child_i=0; kg [child_i].kid != NULL; child_i++)
        {
           bx = &(kg[child_i].box);
           /* Adjust x */
           BX (bx) = *m_width - BX (bx) - BWidth (bx) - b ;
        }
}


/*
 * do a vertical tight (non-column) layout.
 *
 * In a tight layout one dimension of the items is left alone and the other
 * is kept uniform.  In a vertical row column widgets, the widths of each child
 * are uniform for each column, the heights are never changed.  In a horiz
 * row column widget, the widths are never changed and the heights are kept
 * uniform for each row.
 *
 * It gets messy w.r.t. the help child because we don't know if there will
 * be room in the last column/row for it.  If there isn't room then a whole
 * new column/row has to be added.
 *
 * NOTE: the layout is predicated on the help child being the last one since
 * it messes up the x, y for a following child.
 */
static void
LayoutVerticalTight(
        XmRowColumnWidget m,
        Dimension *m_width,     /* if 0 then caller's asking */
        Dimension *m_height )   /* if 0 then caller's asking */
{
    XmRCKidGeometry kg = RC_Boxes (m);
    XtWidgetGeometry *bx;
    Position x, y, max_y = 0;
    Dimension h = 0;
    Dimension w = 0;                /* widest item width in col */
    int child_i, start_i;
    Dimension toc_height;
    Dimension toc_b, b;
    Dimension border = 0;

    ComputeTearOffHeight(m, &toc_b, &b, &toc_height, &start_i, &child_i, 1);

    /* first item location */
    x = MGR_ShadowThickness(m) + RC_MarginW (m);
    y = MGR_ShadowThickness(m) + RC_MarginH (m) + toc_height + toc_b;

    for (; kg [child_i].kid != NULL; child_i++)
    {
       bx = &(kg[child_i].box);
       if (!RC_EntryBorder(m) && kg[child_i].kid &&
	   XtIsWidget(kg[child_i].kid))
         b = Double(kg[child_i].kid->core.border_width);

       h = BHeight (bx) + b;           /* calc this item's height */

       if (((y + h) > *m_height) &&
	   ( ! Asking (*m_height)) &&
	   (child_i))
       {                   /* start new column */
	  while (start_i < child_i)
	      kg[start_i++].box.width = w;    /* set uniform width */

	  x += w + Double(border)
	      + MGR_ShadowThickness(m)
		  + RC_MarginW (m);       /* go right and */

	  y = MGR_ShadowThickness(m)
	      + RC_MarginH (m) + toc_height + toc_b;  /* back to top of menu */

	  w = BWidth (bx);            /* reset for new column */

          if (kg[child_i].kid && XtIsWidget(kg[child_i].kid))
            border = kg[child_i].kid->core.border_width;
          else
            border = ((XmGadget)kg[child_i].kid)->rectangle.border_width;
       }

       if (IsHelp (m, ((Widget) kg[child_i].kid)))
	   CalcHelp (m, m_width, m_height, b, 0, max_y, &x, &y, w, h);

       SetPosition (bx, x, y);

       ASSIGN_MAX(w, BWidth (bx));

       if (kg[child_i].kid && XtIsWidget(kg[child_i].kid))
       {
         if (border < kg[child_i].kid->core.border_width)
           border = kg[child_i].kid->core.border_width;
       }
       else
       {
         if (border < ((XmGadget)kg[child_i].kid)->rectangle.border_width)
           border = ((XmGadget)kg[child_i].kid)->rectangle.border_width;
       }

       y += h + RC_Spacing (m);        /* loc of next item */

       ASSIGN_MAX(max_y, y);       /* record for use later */
    }

    SetAsking (m, m_width, m_height, Double(border), 0, max_y, x, y, w, h);

    /* Set toc width to the width of the pane */
    SET_TEAR_OFF_BOX(toc_height);

    if (RC_AdjLast(m))
        AdjustLast (m, start_i, *m_width, *m_height);
    else
	while (start_i < child_i)
	    kg[start_i++].box.width = w;    /* set uniform width */
    if (LayoutIsRtoLM(m))
        for (child_i=0; kg [child_i].kid != NULL; child_i++)
        {
           bx = &(kg[child_i].box);
           /* Adjust x */
           BX (bx) = *m_width - BX (bx) - BWidth (bx) - b ;
        }
}


static void
LayoutHorizontaltight(
        XmRowColumnWidget m,
        Dimension *m_width,     /* if 0 then caller's asking */
        Dimension *m_height )   /* if 0 then caller's asking */
{
    XmRCKidGeometry kg = RC_Boxes (m);
    XtWidgetGeometry *bx;
    Position x, y, max_x = 0;
    Dimension w = 0;
    Dimension h = 0;                   /* tallest item height in row */
    Dimension new_height = 0;
    Dimension baseline = 0;
    Dimension shadow = 0;
    Dimension highlight = 0;
    Dimension margin_height = 0;
    Dimension margin_top = 0;
    Dimension text_height = 0;
    Dimension border = 0;
    int child_i, start_i;                /* index of first item in row */
    Dimension toc_height;
    Dimension toc_b, b;

    ComputeTearOffHeight(m, &toc_b, &b, &toc_height, &start_i, &child_i, 2);

    /* first item location */
    x = MGR_ShadowThickness(m) + RC_MarginW (m);
    y = MGR_ShadowThickness(m) + RC_MarginH (m) + toc_height + toc_b;

    for (; kg [child_i].kid != NULL; child_i++) {
	bx = &(kg[child_i].box);
	if (!RC_EntryBorder(m) && kg[child_i].kid &&
	    XtIsWidget(kg[child_i].kid))
	    b = Double(kg[child_i].kid->core.border_width);

	w = BWidth (bx) + b;            /* item's width */

	if (((x + w) > *m_width) &&
	    ( ! Asking (*m_width)) &&
	    (child_i)) {                   /* start a new row */

	    if (AlignmentBaselineTop(m) || AlignmentBaselineBottom(m))
		BaselineAlignment(m, h, shadow, highlight, baseline,
				  &new_height, start_i, child_i);
	    else if (AlignmentTop(m) || AlignmentBottom(m))
		TopOrBottomAlignment(m, h, shadow, highlight,
				     baseline, margin_top, margin_height,
				     text_height, &new_height,
				     start_i, child_i);
	    else if (AlignmentCenter(m))
		CenterAlignment(m, h, start_i, child_i);

	    if (new_height > h) {
		while (start_i < child_i) {
		    if (kg[start_i].box.height != new_height) {
		      kg[start_i].margin_bottom +=
			new_height - kg[start_i].box.height;
		      kg[start_i].box.height = new_height;
		    }
		    start_i++;
		}
		h = new_height;
	    }

	    start_i = child_i;

	    x = MGR_ShadowThickness(m)
		+ RC_MarginW (m);       /* left edge of menu */

	    y += h + Double(border) + MGR_ShadowThickness(m)
		+ RC_MarginH (m);       /* down to top of next row */

	    h = BHeight (bx);           /* reset for this row */
	    new_height = 0;
	    baseline = kg[child_i].baseline;
	    if (kg[child_i].kid && XtIsWidget (kg[child_i].kid))
		border = kg[child_i].kid->core.border_width;
	    else if (XmIsGadget (kg[child_i].kid))
		border = ((XmGadget)kg[child_i].kid)->rectangle.border_width;

	    if (XmIsGadget (kg[child_i].kid) ||
		XmIsPrimitive (kg[child_i].kid)) {
		XmBaselineMargins textMargins;

		_XmRC_SetOrGetTextMargins(kg[child_i].kid, XmBASELINE_GET,
					  &textMargins);
		shadow = textMargins.shadow;
		highlight = textMargins.highlight;
		margin_top = textMargins.margin_top;
		text_height = textMargins.text_height;
		margin_height = textMargins.margin_height;
	    }
	}

	if (IsHelp (m, ((Widget) kg[child_i].kid)))
	    CalcHelp (m, m_width, m_height, b, max_x, 0, &x, &y, w, h);

	SetPosition (bx, x, y);

	if (XmIsGadget (kg[child_i].kid) || XmIsPrimitive (kg[child_i].kid))
	  ASSIGN_MAX(baseline, kg[child_i].baseline);

	ASSIGN_MAX(h, BHeight (bx));

	if (kg[child_i].kid && XtIsWidget (kg[child_i].kid)) {
	    ASSIGN_MAX(border, kg[child_i].kid->core.border_width);
	} else if (XmIsGadget (kg[child_i].kid)) {
	    ASSIGN_MAX(border,
		      ((XmGadget)kg[child_i].kid)->rectangle.border_width);
	}

	if (XmIsGadget (kg[child_i].kid) || XmIsPrimitive (kg[child_i].kid))
	    {
		XmBaselineMargins textMargins;

		_XmRC_SetOrGetTextMargins(kg[child_i].kid, XmBASELINE_GET,
					  &textMargins);
		ASSIGN_MAX(shadow, textMargins.shadow);
		ASSIGN_MAX(highlight, textMargins.highlight);
		ASSIGN_MAX(margin_top, textMargins.margin_top);
		ASSIGN_MAX(text_height, textMargins.text_height);
		ASSIGN_MAX(margin_height, textMargins.margin_height);
	    }

	x += w + RC_Spacing (m);        /* loc of next item */

	ASSIGN_MAX (max_x, x);      /* record for use later */
    }

    /* Set toc width to the width of the pane */
    SET_TEAR_OFF_BOX(toc_height);


    if (AlignmentBaselineTop(m) || AlignmentBaselineBottom(m))
	BaselineAlignment(m, h, shadow, highlight, baseline,
			  &new_height, start_i, child_i);
    else if (AlignmentTop(m) || AlignmentBottom(m))
	TopOrBottomAlignment(m, h, shadow, highlight,
			     baseline, margin_top, margin_height,
			     text_height, &new_height, start_i, child_i);
    else if (AlignmentCenter(m))
	CenterAlignment(m, h, start_i, child_i);

    if (new_height > h){
	while (start_i < child_i){
		bx = &(kg[start_i].box);
		if (BHeight(bx) != new_height) {
		  kg[start_i].margin_bottom +=
		    new_height - kg [start_i].box.height;
		  BHeight(bx) = new_height;
		}
		start_i++;
	    }
    }

    if (new_height > h)
	SetAsking (m, m_width, m_height, Double(border),
		    max_x, 0, x, y, w, new_height);
    else
	SetAsking (m, m_width, m_height, Double(border),
		    max_x, 0, x, y, w, h);

    for (child_i = 0; kg[child_i].kid != NULL; child_i++)
    {
        bx = &(kg[child_i].box);
        ASSIGN_MIN(BWidth(bx), *m_width - Double(MGR_ShadowThickness(m))
            - Double(RC_MarginW(m)) - Double(BBorder(bx)));
        if (IsHelp(m, ((Widget) kg[child_i].kid)))
            ASSIGN_MAX(BX(&(kg[child_i].box)), MGR_ShadowThickness(m) + RC_MarginW(m));
    }
    if (RC_AdjLast(m))
	AdjustLast (m, start_i, *m_width, *m_height);
    else
	while (start_i < child_i) {
	    if (new_height > h) kg[start_i++].box.height = new_height;
	    else kg[start_i++].box.height = h;   /* set uniform height */
	}
    if (LayoutIsRtoLM(m))
        for (child_i=0; kg [child_i].kid != NULL; child_i++)
        {
           bx = &(kg[child_i].box);
           /* Adjust x */
           BX (bx) = *m_width - BX (bx) - BWidth (bx) - b ;
        }
}



/*
 * wrap a box around the entries, used with packing mode of none.
 *
 * we ignore negative positioning, ie. only worry about being wide enough
 * for the right edge of the rightmost entry (similarly for height)
 */
static void
LayoutNone(
        XmRowColumnWidget m,
        Dimension *m_width,
        Dimension *m_height )
{
    XtWidgetGeometry *b;
    XmRCKidGeometry kg = RC_Boxes (m);
    int i, dum;
    Dimension w, max_w = 0, max_h = 0;
    Dimension toc_height;
    Dimension toc_b, bw;
    short temp;

    ComputeTearOffHeight(m, &toc_b, &bw, &toc_height, &dum, &i, 2);

    for (; kg [i].kid != NULL; i++)
      {
        b = &(kg[i].box);
        if (!RC_EntryBorder(m) && kg[i].kid && XtIsWidget(kg[i].kid))
             bw = Double(kg[i].kid->core.border_width);

        if (Asking (*m_width))
        {
	    /* be careful about negative positions */
            w = BWidth (b) + bw;
            temp = ((short)w) + BX (b);
            if (temp <= 0)
                w = 1;
            else
                w = (Dimension) temp;

            ASSIGN_MAX (max_w, w);
        }

        if (Asking (*m_height))
        {
            /* be careful about negative positions */
            w = BHeight (b) + Double (bw);
            temp = ((short)w) + BY (b);
            if (temp <= 0)
                w = 1;
            else
                w = (Dimension) temp;

            ASSIGN_MAX (max_h, w);
        }
    }

    /* Set toc width to the width of the pane */
    SET_TEAR_OFF_BOX(toc_height);

    if (Asking (*m_width)) *m_width  = max_w;
    if (Asking (*m_height)) *m_height = max_h;
}


/*
 * Routine used to determine the size of the option menu or to layout
 * the option menu given the current size.  The boolean calcMenuDimension
 * indicates whether the dimensions of the menu should be recalculated.
 * This is true when called from _XmRCThinkAboutSize and false when called
 * from AdaptToSize.
 *
 * This combines the two routines from Motif1.1: think_about_option_size
 * and option_layout.  Also new for Motif 1.2, the instigator is considered.
 * If the instigator is the label or the cascabebuttongadget, then the
 * dimensions are honored if they are large enough.
 */
static void
LayoutOptionAndSize (
        register XmRowColumnWidget menu,
        Dimension *width,
        Dimension *height,
        Widget instigator,
        XtWidgetGeometry *request,
        Boolean calcMenuDimension )
{
   XtWidgetGeometry    *label_box = NULL, *button_box = NULL;
   Dimension c_width;
   Dimension c_height;
   register XmRowColumnWidget p = (XmRowColumnWidget) RC_OptionSubMenu(menu);
   XmCascadeButtonGadget cb =
      (XmCascadeButtonGadget)XmOptionButtonGadget( (Widget) menu);

   /*
    * if this is being destroyed, don't get new dimensions.  This routine
    * assumes that cb is valid.
    */

   if (menu->core.being_destroyed)
   {
       if (calcMenuDimension)
       {
           *width = XtWidth(menu);
           *height = XtHeight(menu);
       }
       return;
   }

   /* Find the interesting boxes */

   if (!XtIsManaged(XmOptionLabelGadget( (Widget) menu))) {
       button_box = &(RC_Boxes(menu)[0].box);
   } else {
       label_box = &(RC_Boxes(menu)[0].box);
       button_box = &(RC_Boxes(menu)[1].box);
   }


   if (p)
   {
      c_width = c_height = 0;

      FindLargestOption( p, &c_width, &c_height );

      if (LayoutIsRtoLG(cb))
	c_width += Double(G_HighlightThickness(cb)) + G_ShadowThickness(cb) +
	           LabG_MarginLeft(cb) + Double(MGR_ShadowThickness(p))  -
		   /* magic value */ 2;
      else
	c_width += Double(G_HighlightThickness(cb)) + G_ShadowThickness(cb) +
	           LabG_MarginRight(cb) + Double(MGR_ShadowThickness(p))  -
		   /* magic value */ 2;
      c_height += Double(G_HighlightThickness(cb)) + LabG_MarginTop(cb)
		 + LabG_MarginBottom(cb);

      /* allow settings in cbg to be honored if greater than best size */
      if (instigator == (Widget) cb)
      {
	  if ((request->request_mode & CWHeight) &&
	      (request->height > c_height))
	  {
	      c_height = request->height;
	  }
	  if ((request->request_mode & CWWidth) &&
	      (request->width > c_width))
	  {
	      c_width = request->width;
	  }
      }
      BWidth(button_box) = c_width;
      BHeight(button_box) = c_height;
  }
   else
   {
      /* Option menu draws a toggle indicator with a childless submenu */
      c_width = BWidth(button_box);
      c_height = BHeight(button_box);
   }

   /* treat separate the case where the label is unmanaged */
   if (!XtIsManaged(XmOptionLabelGadget( (Widget) menu))) {

       if (!calcMenuDimension &&  c_height > XtHeight(menu))
	   c_height = XtHeight(menu) - 2*RC_MarginH(menu);

       if (!calcMenuDimension && c_width > XtWidth (menu))
	   c_width = XtWidth(menu) - 2*RC_MarginW(menu);

       BWidth(button_box) = c_width;
       BHeight(button_box) = c_height;

       BX(button_box) = RC_MarginW(menu);
       BY(button_box) = RC_MarginH(menu);

       if (calcMenuDimension)
	   {
	       *width = c_width + 2*RC_MarginW(menu);
	       *height = c_height + 2*RC_MarginH(menu);
	   }
       return ;
   }

   if (IsHorizontal(menu))
   {
      /*
       * Set the height to the highest of the two but if calcMenuDimension
       * is false, limit it to the size of the option menu
       */

      if (BHeight(label_box) > c_height)
	  c_height = BHeight(label_box);

      if (!calcMenuDimension &&  c_height > XtHeight(menu))
	  c_height = XtHeight(menu) - 2*RC_MarginH(menu);

      BHeight(label_box) = c_height;
      BHeight(button_box) = c_height;

      /* The label box is placed at... */
      /* The button is placed just next to the label */
      /* Reverse if RtoL */

      if (LayoutIsRtoLM(menu)) {
	BX(button_box) = RC_MarginW(menu);
	BX(label_box) = BX(button_box) + BWidth(button_box) + RC_Spacing(menu);
      } else {
	BX(label_box) = RC_MarginW(menu);
	BX(button_box) = BX(label_box) + BWidth(label_box) + RC_Spacing(menu);
      }
      BY(label_box) = RC_MarginH(menu);
      BY(button_box) = RC_MarginH(menu);

      if (calcMenuDimension)
      {
	if (LayoutIsRtoLM(menu))
	  *width = BX(label_box) + BWidth(label_box) + RC_MarginW(menu);
	else
	  *width = BX(button_box) + c_width + RC_MarginW(menu);
	*height = c_height + 2*RC_MarginH(menu);
      }
   }
   else	/* is vertical menu */
   {
      /*
       * Set the height to the highest of the two but if calcMenuDimension
       * is false, limit it to the size of the option menu
       */
      if (BWidth(label_box) > c_width)
	  c_width = BWidth(label_box);

      if (!calcMenuDimension && c_width > XtWidth (menu))
	  c_width = XtWidth(menu) - 2*RC_MarginW(menu);

      BWidth(label_box) = c_width;
      BWidth(button_box) = c_width;

      /* The label box is placed at... */
      BX(label_box) = RC_MarginW(menu);
      BY(label_box) = RC_MarginH(menu);

      /* The button is placed just below the label */
      BX(button_box) = RC_MarginW(menu);
      BY(button_box) = BY(label_box) + BHeight(label_box) + RC_Spacing(menu);

      if (calcMenuDimension)
      {
	  *width = c_width + 2*RC_MarginW(menu);
	  *height = BY(button_box) + c_height + RC_MarginH(menu);
      }
   }
}


void
_XmRCThinkAboutSize(
        register XmRowColumnWidget m,
        Dimension *w,
        Dimension *h,
        Widget instigator,
        XtWidgetGeometry *request )
{
    if (!RC_ResizeWidth(m))  *w = XtWidth  (m);
    if (!RC_ResizeHeight(m)) *h = XtHeight (m);

    if (IsOption(m))
	LayoutOptionAndSize(m, w, h, instigator, request, TRUE);
    else if (PackNone (m))
        LayoutNone (m, w, h);
    else if (PackColumn (m))
	LayoutColumn (m, w, h);
    else {
	if (IsVertical (m))
	    LayoutVerticalTight (m, w, h);
	else
	    LayoutHorizontaltight (m, w, h);
    }

    if (!RC_ResizeHeight(m) && !RC_ResizeWidth(m))
        return;

    ASSIGN_MAX(*w, RESOURCE_MIN_WIDTH);
    ASSIGN_MAX(*h, RESOURCE_MIN_HEIGHT);
}



void
_XmRCPreferredSize(
        XmRowColumnWidget m,
        Dimension *w,
        Dimension *h )
{
   Widget *q;
   int i;
   Dimension * baselines;
   int line_count;
   Dimension y;

   if ((!IsOption(m)) && ((PackColumn(m) && (IsVertical(m) ||
					     IsHorizontal(m))) ||
			  (PackTight(m) && IsHorizontal(m))))
   {
       if ((PackColumn(m) && (IsVertical(m) || IsHorizontal(m))) ||
         (PackTight(m) && IsHorizontal(m)))
     {
      if (*h == 0)
      {
       ForManagedChildren(m, i, q) /* reset Top and Bottom Margins that were */
       {                           /* set for vertical Alignment to work     */
          if (XmIsGadget(*q) || XmIsPrimitive(*q))
          {
            XmBaselineMargins textMargins;

            textMargins.margin_top = SavedMarginTop(*q);
            textMargins.margin_bottom = SavedMarginBottom(*q);
            _XmRC_SetOrGetTextMargins(*q, XmBASELINE_SET, &textMargins);

          }
       }
      }
     }

   /*
    * get array built for both widgets and gadgets layout is based only on
    * this array, adjust width margins &  adjust height margins
    */
     RC_Boxes(m)=
       (XmRCKidGeometry)_XmRCGetKidGeo( (Widget) m, NULL, NULL,
				    RC_EntryBorder(m),
				    RC_EntryBorder (m),
				    (IsVertical (m) && RC_DoMarginAdjust (m)),
				    (IsHorizontal (m) &&
				     RC_DoMarginAdjust (m)),
				    RC_HelpPb (m),
				    RC_TearOffControl(m),
				    XmGET_PREFERRED_SIZE);
     for (i = 0; RC_Boxes(m) [i].kid != NULL; i++)
     {
       Widget rc_kid;
       XmBaselineMargins textMargins;
       XRectangle displayRect;
       unsigned char label_type = XmSTRING;
       rc_kid = RC_Boxes(m) [i].kid;

       if (XmIsGadget (rc_kid) || XmIsPrimitive (rc_kid))
       {
	 XtVaGetValues(rc_kid, XmNlabelType, &label_type, NULL);

	 if (label_type == XmSTRING)
	 {
	   if (XmIsLabel(rc_kid) || XmIsLabelGadget(rc_kid)) {
	     /* The baseline functions returns the baseline on the current size */
	     /* Since we need the preferred baseline, we need to calculate the y   */
	     /* coordinate on the preferred size and add this in to the baseline */
	     /* returned, after subtracting the y coordinate of the current widget */
	     _XmRC_SetOrGetTextMargins(rc_kid, XmBASELINE_GET, &textMargins);
	     y = (  textMargins.highlight + textMargins.shadow
		  + textMargins.margin_height + textMargins.margin_top
		  + ((  RC_Boxes(m) [i].box.height
		      - textMargins.margin_top
		      - textMargins.margin_bottom
		      - (2 * (  textMargins.margin_height
			      + textMargins.shadow
			      + textMargins.highlight))
		      - textMargins.text_height) / 2));
	     XmWidgetGetDisplayRect(rc_kid, &displayRect);
	   }
	   else {
	     displayRect.y = y = 0;
	   }

	    if (AlignmentBaselineTop(m) || AlignmentBaselineBottom(m)) {
	      if (XmWidgetGetBaselines(rc_kid, &baselines, &line_count)) {
		if (AlignmentBaselineTop(m))
		    RC_Boxes(m) [i].baseline = baselines[0] - displayRect.y + y;
		else if (AlignmentBaselineBottom(m))
		    RC_Boxes(m) [i].baseline = baselines[line_count - 1] -
		                               displayRect.y + y;
		XtFree((char *)baselines);
	      }
	      else
	      {
		RC_Boxes(m) [i].baseline = 0;
	      }
	    }
            RC_Boxes(m) [i].margin_top = 0;
            RC_Boxes(m) [i].margin_bottom = 0;
          }
          else
          {
            RC_Boxes(m) [i].baseline = 0;
            RC_Boxes(m) [i].margin_top = 0;
            RC_Boxes(m) [i].margin_bottom = 0;
          }
        }
      }
   }
   else
   {
     /*
      * get array built for both widgets and gadgets layout is based only on
      * this array, adjust width margins &  adjust height margins
      */
      RC_Boxes(m)=
       (XmRCKidGeometry)_XmRCGetKidGeo( (Widget) m, NULL, NULL,
				    RC_EntryBorder(m),
				    RC_EntryBorder (m),
				    (IsVertical (m) && RC_DoMarginAdjust (m)),
				    (IsHorizontal (m) &&
				     RC_DoMarginAdjust (m)),
				    RC_HelpPb (m),
				    RC_TearOffControl(m),
				    XmGET_PREFERRED_SIZE);
   }

   _XmRCThinkAboutSize (m, w, h, NULL, NULL);

   XtFree ((char *) RC_Boxes(m));
}

/*
 * Layout the row column widget to fit it's current size; ignore possible
 * non-fitting of the entries into a too small row column widget.
 *
 * Don't forget the instigator.
 */
void
_XmRCAdaptToSize(
        XmRowColumnWidget m,
        Widget instigator,
        XtWidgetGeometry *request )
{
   Dimension w = XtWidth (m);
   Dimension h = XtHeight (m);
   Dimension instigator_w = 0 ;
   Dimension instigator_h = 0;
   short i;
   Widget *q;

   if ((!IsOption(m)) && ((PackColumn(m) && (IsVertical(m) || IsHorizontal(m))) ||
       (PackTight(m) && IsHorizontal(m))))
   {
     ForManagedChildren(m, i, q)
     {
        if (XmIsGadget (*q) || XmIsPrimitive (*q))
        {
          XmBaselineMargins textMargins;

          textMargins.margin_top = SavedMarginTop(*q);
          textMargins.margin_bottom = SavedMarginBottom(*q);
          _XmRC_SetOrGetTextMargins(*q, XmBASELINE_SET, &textMargins);
        }
     }
   }
   /*
    * get array built for both widgets and gadgets,
    * layout is based only on this array,
    * adjust width margins and  adjust height margins
    */
   RC_Boxes(m) =
       (XmRCKidGeometry)_XmRCGetKidGeo( (Widget) m, instigator, request,
				       RC_EntryBorder(m),
				       RC_EntryBorder (m),
				       (IsVertical (m) &&
					RC_DoMarginAdjust (m)),
				       (IsHorizontal (m) &&
					RC_DoMarginAdjust (m)),
				       RC_HelpPb (m),
				       RC_TearOffControl(m),
				       XmGET_PREFERRED_SIZE);

   if ((!IsOption(m)) && ((PackColumn(m) && (IsVertical(m) ||
					     IsHorizontal(m))) ||
			  (PackTight(m) && IsHorizontal(m))))
   {
     for (i = 0; RC_Boxes(m) [i].kid != NULL; i++)
     {
       if (XmIsGadget(RC_Boxes(m) [i].kid) || XmIsPrimitive(RC_Boxes(m) [i].kid))
       {
	 unsigned char label_type = XmSTRING;
	 RectObj ro = (RectObj) RC_Boxes(m) [i].kid;

	 if (XtHeight (RC_Boxes(m) [i].kid) != RC_Boxes(m) [i].box.height)
         {
	   XmeConfigureObject( (Widget) ro, ro->rectangle.x, ro->rectangle.y,
			      ro->rectangle.width, RC_Boxes(m) [i].box.height,
			      ro->rectangle.border_width);
	 }

	 XtVaGetValues(RC_Boxes(m) [i].kid, XmNlabelType, &label_type, NULL);

	 if (label_type == XmSTRING &&
	     (AlignmentBaselineTop(m) || AlignmentBaselineBottom(m)))
	 {
	   Dimension *baselines;
	   int line_count;

	   XmWidgetGetBaselines(RC_Boxes(m) [i].kid, &baselines, &line_count);
	   if (AlignmentBaselineTop(m))
	     RC_Boxes(m) [i].baseline = baselines[0];
	   else if (AlignmentBaselineBottom(m))
	     RC_Boxes(m) [i].baseline = baselines[line_count - 1];
	   XtFree((char *)baselines);
	 }
	 else
	   RC_Boxes(m) [i].baseline = 0;

       }
     }
   }

   if (IsOption(m))
       LayoutOptionAndSize (m, &w, &h, instigator, request, FALSE);
   else if (PackColumn (m))
       LayoutColumn (m, &w, &h);
   else if (!PackNone (m)) {
       if (IsVertical (m))
	   LayoutVerticalTight (m, &w, &h);
       else
	   LayoutHorizontaltight (m, &w, &h);
   }

   if ((!IsOption(m)) && ((PackColumn(m) && (IsVertical(m) || IsHorizontal(m))) ||
       (PackTight(m) && IsHorizontal(m))))
   {
     for (i = 0; RC_Boxes(m) [i].kid != NULL; i++)
     {
       if (XmIsGadget(RC_Boxes(m) [i].kid) || XmIsPrimitive(RC_Boxes(m) [i].kid))
       {
         XmBaselineMargins textMargins;

         textMargins.margin_top = RC_Boxes(m) [i].margin_top;
         textMargins.margin_bottom = RC_Boxes(m) [i].margin_bottom;
         _XmRC_SetOrGetTextMargins(RC_Boxes(m) [i].kid, XmBASELINE_SET, &textMargins);

       }
     }
   }

   if (instigator)
     {
       /* CR 5419: Save the original instigator dimensions */
       instigator_w = XtWidth(instigator);
       instigator_h = XtHeight(instigator);
     }

   _XmRCSetKidGeo ((XmRCKidGeometry)RC_Boxes(m), instigator);

   /*
   ** Hack alert!
   ** This is special code to enforce that the CBG in an option menu is
   ** kept correctly-sized when the items in this pulldown, associated with
   ** that cascade, change. There is no protocol for communicating this
   ** information nor any mechanism for adapting one; so we make the size
   ** request ourselves, here. The point is not that we are setting to the
   ** correct size so much as that the cascade's size is being adjusted after
   ** the menu size is fixed. calc_CBG_dims is probably being called an extra
   ** time to figure out the real size. This is not a frequent case.
    *
    * Do not call this routine below if this call was initiated by a geometry
    * request from an option menu's label or cascade button.  In that case,
    * the geometry has already been taken care of and must not be meddled
    * with or it will reset some values incorrectly.
    */
   if (!IsOption(m) || !instigator ||
       !((instigator == RC_Boxes(m)[0].kid) ||
	 (instigator == RC_Boxes(m)[1].kid)))
   {
       _XmRC_CheckAndSetOptionCascade(m);
   }

/* The old geometry management took care of resizing the instigator if
   XtGeometryYes was returned even if core.width and core.height had
   not changed. However this is not the case with the new geometry
   management. Therefore if margins have changed but not the core
   width and height label's resize needs to be called to calculate
   the x & y coordinates for the label text, with the new margins. */

   if ((instigator) &&
       (instigator_w == XtWidth(instigator)) &&
       (instigator_h == XtHeight(instigator)) &&
       (XmIsLabel(instigator) || XmIsLabelGadget(instigator)))
   {
     WidgetClass wc = XtClass(instigator);
     XtWidgetProc resize;

     _XmProcessLock();
     resize = wc->core_class.resize;
     _XmProcessUnlock();
     (*resize) ((Widget) instigator);
   }
   /* Patch submitted by Kevin B. Hendrix of Java-Linux project. */
   if (RC_Boxes(m))
   {
     XtFree ( (char *) RC_Boxes(m));
     RC_Boxes(m) = NULL;
   }
}


void
_XmRC_SetOrGetTextMargins(
        Widget wid,
        unsigned char op,
        XmBaselineMargins *textMargins )
{
  WidgetClass wc = XtClass(wid);

  if (op == XmBASELINE_GET) {
    /* in case the class does not have this procedure */
    bzero((void *) textMargins, sizeof(XmBaselineMargins));
  }

  textMargins->get_or_set = op;

  if (XmIsGadget(wid))
    {
      XmGadgetClassExt     *wcePtr;

      wcePtr = _XmGetGadgetClassExtPtr(wc, NULLQUARK);
      if (*wcePtr && (*wcePtr)->version == XmGadgetClassExtVersion &&
	  (*wcePtr)->widget_margins)
	(*((*wcePtr)->widget_margins)) (wid, textMargins) ;
    }
  else if (XmIsPrimitive(wid))
    {
      XmPrimitiveClassExt  *wcePtr;

      wcePtr = _XmGetPrimitiveClassExtPtr(wc, NULLQUARK);
      if (*wcePtr && (*wcePtr)->widget_margins)
	(*((*wcePtr)->widget_margins)) (wid, textMargins) ;
  }
}


/****************************************************************
 * Assemble a kid box for each child widget and gadget, fill in data about
 *   each widget and optionally set up uniform border widths.
 * Returns a list of records, last one has a 'kid' field of NULL.  This memory
 *   for this list should eventually be freed with a call to XtFree().
 ****************/
XmRCKidGeometry
_XmRCGetKidGeo(
        Widget wid,                     /* Widget w/ children. */
        Widget instigator,              /* May point to a child who */
        XtWidgetGeometry *request,      /*   is asking to change. */
        int uniform_border,             /* T/F, enforce it. */
        Dimension border,               /* Value to use if enforcing.*/
        int uniform_width_margins,      /* unused, T/F, enforce it. */
        int uniform_height_margins,     /* unused, T/F, enforce it. */
        Widget help,                    /* May point to a help kid. */
	Widget toc,			/* May point to tear_off_control kid. */
        int geo_type )                  /* Actual or preferred. */
{
    CompositeWidget	c = (CompositeWidget) wid ;
    XmRCKidGeometry	geo ;
    Widget		kidWid ;
    int			i ;
    int			j = 0 ;
    Boolean		helpFound = FALSE ;
    Boolean		tocFound;

    tocFound = (toc && XtIsManaged(toc)) & 0x1;

    geo = (XmRCKidGeometry) XtMalloc((_XmGeoCount_kids(c) + 1 + tocFound) *
       sizeof (XmRCKidGeometryRec));

    i = 0;

    if (tocFound)
    {
       geo[j].kid = toc ;

       _XmGeoLoadValues( toc, geo_type, instigator, request, &(geo[j].box));

       geo[j].margin_top = 0;
       geo[j].margin_bottom = 0;
       geo[j].baseline = 0;


       if (uniform_border)     /* if asked override border */
       {
	  geo[j].box.border_width = border ;
       }
       j++ ;
    }

    /* load all managed kids */
    for( ; i < c->composite.num_children ; i++    )
    {
       kidWid = c->composite.children[i] ;
       if (XtIsManaged( kidWid))
       {
	  if(    kidWid == help    )
          {  /* Save to put help widget at the end of the widget list.*/
             helpFound = TRUE ;
          }
          else
	  {
	     geo[j].kid = kidWid ;

	     _XmGeoLoadValues( kidWid, geo_type, instigator, request,
							       &(geo[j].box)) ;
             geo[j].margin_top = 0;
             geo[j].margin_bottom = 0;
             geo[j].baseline = 0;

	     /* Fix for CR 5598 - If the child is a separator widget
		or gadget, set the width in the geo box to 0.  This
		will take the separator out of the width consideration. */
	     /* Fix for 8131: only does that to Separators when packing
               if not none: this is the only time when it matters, plus
               pack_none does not correct the setting to 0 and the
               rowcolumn dies in protocol error later when trying to
               configure the separator to its 0 box.width or height */

	     if ((XmIsSeparator(kidWid) || XmIsSeparatorGadget(kidWid)) &&
		 (RC_Packing (c) != XmPACK_NONE)) {
		 unsigned char orientation;
		 Arg args[1];

		 XtSetArg(args[0], XmNorientation, &orientation);
		 XtGetValues(kidWid, args, 1);

		 if (orientation == XmHORIZONTAL)
		     geo[j].box.width = 0;
		 else
		     geo[j].box.height = 0;
             }
	     /* End fix for CR 5598 and 8131 */


	     if (uniform_border)     /* if asked override border */
	     {
		geo[j].box.border_width = border ;
	     }
	     j++ ;
	  }
       }
    }

    if (helpFound)                 /* put help guy into list */
    {
        geo[j].kid = help ;

        _XmGeoLoadValues( help, geo_type, instigator, request, &(geo[j].box)) ;

        geo[j].margin_top = 0;
        geo[j].margin_bottom = 0;
        geo[j].baseline = 0;


        if (uniform_border)         /* if asked override border */
        {
	   geo[j].box.border_width = border ;
	}
        j++ ;
    }
    geo[j].kid = NULL ;                /* signal end of list */

    return( geo) ;
}


/****************************************************************
 * Take the kid geometry array and change each kid to match them.
 *   remember not to do the resize of the instigator.
 * The kid geometry "kg" is assumed to be fully specified.
 ****************/
void
_XmRCSetKidGeo(
        XmRCKidGeometry kg,
        Widget instigator )
{
    Widget          w ;
    XtWidgetGeometry * b ;
    int             i ;
/****************/

    for(i=0 ; kg[i].kid != NULL ; i++) {
        w = (Widget) kg[i].kid ;
        b = &(kg[i].box) ;

	if(    w != instigator    ) {
	    XmeConfigureObject(w, b->x, b->y, b->width, b->height,
			       b->border_width) ;
	} else {
	    XtX( w) = b->x ;
	    XtY( w) = b->y ;
	    XtWidth( w) = b->width ;
	    XtHeight( w) = b->height ;
	    XtBorderWidth( w) = b->border_width ;
	}
    }
    return ;
}



static void
GetMenuKidMargins(
        XmRowColumnWidget m,
        Dimension *width,
        Dimension *height,
        Dimension *left,
        Dimension *right,
        Dimension *top,
        Dimension *bottom )
{
   register int i;
   Widget *q;

   *width = *height = *left = *right = *top = *bottom = 0;

   ForManagedChildren(m, i, q) {
	if (XmIsLabelGadget(*q)) {
	    ASSIGN_MAX(*width, LabG_MarginWidth  (*q));
	    ASSIGN_MAX(*height, LabG_MarginHeight (*q));
	    ASSIGN_MAX(*left, LabG_MarginLeft   (*q));
	    ASSIGN_MAX(*right, LabG_MarginRight  (*q));
	} else if (XmIsLabel(*q)) {
	    ASSIGN_MAX(*width, Lab_MarginWidth  (*q));
	    ASSIGN_MAX(*height, Lab_MarginHeight (*q));
	    ASSIGN_MAX(*left, Lab_MarginLeft   (*q));
	    ASSIGN_MAX(*right, Lab_MarginRight  (*q));
	}
    }

    ForManagedChildren (m, i, q)
    {
      if (XmIsLabel(*q) || XmIsLabelGadget(*q))
      {
       if (SavedMarginTop(*q) > *top)
           *top = SavedMarginTop(*q);
       if (SavedMarginBottom(*q) > *bottom)
           *bottom = SavedMarginBottom(*q);
      }
    }
}


/*
 * Toggle buttons have this thingy hanging off the left of the
 * widget, before the text.  This dimension is known as the MarginLeft.
 * Pulldown's have hot spots in the MarginRight, accelerators go in the
 * marginRight also.
 *
 * For generality's sake we should insure that all
 * of the current label subclass widgets in the menu have the
 * margins set to the same value.
 */
void
_XmRCDoMarginAdjustment(
        XmRowColumnWidget m )
{
    register Widget *p;
    register int i;
    Dimension m_w, m_h, m_l, m_r, m_t, m_b;
    Dimension w, h;

    if ((!RC_DoMarginAdjust (m)) || (IsOption(m)))
    {
      ForManagedChildren (m, i, p)
      {
        if (XmIsGadget(*p) || XmIsPrimitive(*p))
        {

          XmBaselineMargins textMargins;

          _XmRC_SetOrGetTextMargins(*p, XmBASELINE_GET, &textMargins);
          SavedMarginTop(*p) = textMargins.margin_top;
          SavedMarginBottom(*p) = textMargins.margin_bottom;

        }
       }
       return;
    }
    /*
     * this should almost be part
     * of the layout process, except this requires a setvalue not a resize...
     */

    GetMenuKidMargins (m, &m_w, &m_h, &m_l, &m_r, &m_t, &m_b);

    ForManagedChildren (m, i, p)
    {
        if (XmIsLabelGadget(*p))
        {
            XmLabelGadget q;
	    XmLabelGCacheObjPart localCache;
            /*
             * If in a popup or pulldown pane,
             * don't do labels; i.e. only do buttons.
             */
            if (((*p)->core.widget_class == xmLabelGadgetClass) &&
                (IsPulldown(m) || IsPopup(m)))
                continue;

            w = XtWidth  (*p);
            h = XtHeight (*p);

            q = (XmLabelGadget) (*p);

            if (IsVertical (m))
            {
	       _XmQualifyLabelLocalCache(&localCache, q);

	       /* change horiz margins to  be uniform */
	       if (LabG_MarginLeft(q) != m_l)
	       {
		  w += m_l - LabG_MarginLeft(q);
		  _XmAssignLabG_MarginLeft_r((&localCache), m_l);
	       }

	       if (LabG_MarginRight(q) != m_r)
	       {
		  w += m_r - LabG_MarginRight(q);
		  _XmAssignLabG_MarginRight_r((&localCache), m_r);
	       }

	       if (LabG_MarginWidth(q) != m_w)
	       {
		  w += m_w - LabG_MarginWidth(q);
		  _XmAssignLabG_MarginWidth_r((&localCache), m_w);
	       }
       	       _XmReCacheLabG_r(&localCache, q);

	       if (q->rectangle.width != w)
	       {
		  XmeConfigureObject( (Widget) q, q->rectangle.x,
                                        q->rectangle.y, w, q->rectangle.height,
                                                    q->rectangle.border_width);
	       }
            }

            if (!IsVertical (m) || PackColumn(m))
            {
	       _XmQualifyLabelLocalCache(&localCache, q);

	       /* change vert margins */
	       if (LabG_MarginTop(q) != m_t)
	       {
		  h += m_t - LabG_MarginTop(q);
		  _XmAssignLabG_MarginTop_r((&localCache), m_t);
	       }

	       if (LabG_MarginBottom(q) != m_b)
	       {
		  h += m_b - LabG_MarginBottom(q);
		  _XmAssignLabG_MarginBottom_r((&localCache), m_b);
	       }

	       if (LabG_MarginHeight(q) != m_h)
	       {
		  h += m_h - LabG_MarginHeight(q);
		  _XmAssignLabG_MarginHeight_r((&localCache), m_h);
	       }

	       _XmReCacheLabG_r(&localCache, q);

	       if (q->rectangle.height != h)
	       {
		  XmeConfigureObject( (Widget) q, q->rectangle.x,
                                         q->rectangle.y, q->rectangle.width, h,
                                                    q->rectangle.border_width);
	       }
               SavedMarginTop(*p) = LabG_MarginTop (q);
               SavedMarginBottom(*p) = LabG_MarginBottom (q);
            }
	 }
        else if (XmIsLabel(*p))
        {
            XmLabelWidget lw;
            /*
             * If in a popup or pulldown pane,
             * don't do labels; i.e. only do buttons.
             */
            if (((*p)->core.widget_class == xmLabelWidgetClass) &&
                (IsPulldown(m) || IsPopup(m)))
                continue;

            w = XtWidth  (*p);
            h = XtHeight (*p);

            lw = (XmLabelWidget) (*p);

            if (IsVertical (m)) /* change horiz margins to */
            {                   /* be uniform */
               ChangeMargin (Lab_MarginLeft  (lw), m_l, w);
               ChangeMargin (Lab_MarginRight (lw), m_r, w);
               ChangeMargin (Lab_MarginWidth (lw), m_w, w);

               if (XtWidth (lw) != w)
               {
                    XmeConfigureObject( (Widget) lw, lw->core.x, lw->core.y,
                                    w, lw->core.height, lw->core.border_width);
               }
            }

            if (!IsVertical (m) || PackColumn(m))      /* change vert margins */
            {
                ChangeMargin (Lab_MarginTop (lw), m_t, h);
                ChangeMargin (Lab_MarginBottom (lw), m_b, h);
                ChangeMarginDouble (Lab_MarginHeight (lw), m_h, h);

                if (XtHeight (lw) != h)
                {
                    XmeConfigureObject( (Widget) lw, lw->core.x,lw->core.y,
                                     lw->core.width, h, lw->core.border_width);
                }
                SavedMarginTop(*p) = Lab_MarginTop (lw);
                SavedMarginBottom(*p) = Lab_MarginBottom (lw);
            }
        }
    }
}
