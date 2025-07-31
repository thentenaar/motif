/* $XConsortium: PixConv.c /main/12 1996/12/16 18:32:08 drk $ */
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <Xm/AccColorT.h>
#include <Xm/TraitP.h>
#include <Xm/XmP.h>
#include "XmI.h"
#include "ImageCachI.h"
#include "PixConvI.h"
#include "ScreenI.h"

/* Warning and Error messages */

#define DEPTH(widget)  \
    (XtIsWidget(widget))? \
       ((widget)->core.depth):((XtParent(widget))->core.depth)

/********    Static Function Declarations    ********/

static Boolean GetColorInfo (
			  Widget widget,
			  XmAccessColorData acc_color);
static Pixmap GetPixmap (
			 Widget widget,
			 unsigned char  conv_type,
			 String image_name,
			 Boolean  scaling);
static Boolean CvtStringToPixmap(
                        Display *dpy,
                        XrmValue *args,
                        Cardinal *numArgs,
                        XrmValue *fromVal,
                        XrmValue *toVal,
                        XtPointer *closure_ret) ;
/********    End Static Function Declarations    ********/


/*--------------------------------------------------------------*/
/*  Argument lists sent down to all pixmap converter functions  */

#define CONVERT_BITMAP  0
#define CONVERT_DYNAMIC 1
#define CONVERT_PIXMAP  2


static XtConvertArgRec bitmapArgs[] =
{
   { XtBaseOffset, (XtPointer) 0, sizeof (int) }, /* to get the widget */
   { XtAddress, (XtPointer)CONVERT_BITMAP, 0},
   { XtAddress, (XtPointer)True, 0}  /* scaling */
};

static XtConvertArgRec bitmapNoScalingArgs[] =
{
   { XtBaseOffset, (XtPointer) 0, sizeof (int) },
   { XtAddress, (XtPointer)CONVERT_BITMAP, 0},
   { XtAddress, (XtPointer)False, 0}
};

static XtConvertArgRec dynamicArgs[] =
{
   { XtBaseOffset, (XtPointer) 0, sizeof (int) },
   { XtAddress, (XtPointer)CONVERT_DYNAMIC, 0},
   { XtAddress, (XtPointer)True, 0}
};

static XtConvertArgRec dynamicNoScalingArgs[] =
{
   { XtBaseOffset, (XtPointer) 0, sizeof (int) },
   { XtAddress, (XtPointer)CONVERT_DYNAMIC, 0},
   { XtAddress, (XtPointer)False, 0}
};

static XtConvertArgRec pixmapArgs[] =
{
   { XtBaseOffset, (XtPointer) 0, sizeof (int) },
   { XtAddress, (XtPointer)CONVERT_PIXMAP, 0},
   { XtAddress, (XtPointer)False, 0}
};

/************************************************************************
 *
 *  _XmRegisterPixmapConverters
 *	Register the pixmap converters used in Motif 2.0
 *
 ************************************************************************/
void
_XmRegisterPixmapConverters( void )
{
    static Boolean inited = False;

    _XmProcessLock();
    if (inited == False) {
	inited = True;

	/* for icon masks - need scaling */
	XtSetTypeConverter (XmRString, XmRBitmap,
			    CvtStringToPixmap,
			    bitmapArgs, XtNumber(bitmapArgs),
			    (XtCacheNone | XtCacheRefCount), NULL);
	/* for insensitive stipple */
	XtSetTypeConverter (XmRString, XmRNoScalingBitmap,
			    CvtStringToPixmap,
			    bitmapNoScalingArgs,
			    XtNumber(bitmapNoScalingArgs),
			    (XtCacheNone | XtCacheRefCount), NULL);
	/* for most pixmap used as icon - need scaling */
	XtSetTypeConverter (XmRString, XmRDynamicPixmap,
			    CvtStringToPixmap,
			    dynamicArgs, XtNumber(dynamicArgs),
			    (XtCacheNone | XtCacheRefCount), NULL);
 	/* for pixmap used as tiling */
	XtSetTypeConverter (XmRString, XmRNoScalingDynamicPixmap,
			    CvtStringToPixmap,
			    dynamicNoScalingArgs,
			    XtNumber(dynamicNoScalingArgs),
			    (XtCacheNone | XtCacheRefCount), NULL);

 	/* for background and shell iconPixmap: no scaling by default */
	XtSetTypeConverter (XmRString, XmRPixmap,
			    CvtStringToPixmap,
			    pixmapArgs, XtNumber(pixmapArgs),
			    (XtCacheNone | XtCacheRefCount), NULL);


#ifndef _NO_PIXMAP_CONV_BC
/* Here we install the 1.2 pixmap converters by default,
   so that subwidgets can still use them.
   They use the pixmapArgs, meaning they create a matching
   depth pixmap, with background and foreground color, not
   highlight, top_shadow, etc.

   define _NO_PIXMAP_CONV_BC if you don't want them */

	XtSetTypeConverter (XmRString, XmRXmBackgroundPixmap,
			    CvtStringToPixmap,
			    pixmapArgs, XtNumber(pixmapArgs),
			    (XtCacheNone | XtCacheRefCount), NULL);

	XtSetTypeConverter (XmRString, XmRPrimForegroundPixmap,
			    CvtStringToPixmap,
			    pixmapArgs, XtNumber(pixmapArgs),
			    (XtCacheNone | XtCacheRefCount), NULL);

	XtSetTypeConverter (XmRString, XmRPrimHighlightPixmap,
			    CvtStringToPixmap,
			    pixmapArgs, XtNumber(pixmapArgs),
			    (XtCacheNone | XtCacheRefCount), NULL);

	XtSetTypeConverter (XmRString, XmRPrimTopShadowPixmap,
			    CvtStringToPixmap,
			    pixmapArgs, XtNumber(pixmapArgs),
			    (XtCacheNone | XtCacheRefCount), NULL);

	XtSetTypeConverter (XmRString, XmRPrimBottomShadowPixmap,
			    CvtStringToPixmap,
			    pixmapArgs, XtNumber(pixmapArgs),
			    (XtCacheNone | XtCacheRefCount), NULL);

	XtSetTypeConverter (XmRString, XmRManForegroundPixmap,
			    CvtStringToPixmap,
			    pixmapArgs, XtNumber(pixmapArgs),
			    (XtCacheNone | XtCacheRefCount), NULL);

	XtSetTypeConverter (XmRString, XmRManHighlightPixmap,
			    CvtStringToPixmap,
			    pixmapArgs, XtNumber(pixmapArgs),
			    (XtCacheNone | XtCacheRefCount), NULL);

	XtSetTypeConverter (XmRString, XmRManTopShadowPixmap,
			    CvtStringToPixmap,
			    pixmapArgs, XtNumber(pixmapArgs),
			    (XtCacheNone | XtCacheRefCount), NULL);

	XtSetTypeConverter (XmRString, XmRManBottomShadowPixmap,
			    CvtStringToPixmap,
			    pixmapArgs, XtNumber(pixmapArgs),
			    (XtCacheNone | XtCacheRefCount), NULL);

	XtSetTypeConverter (XmRString, XmRGadgetPixmap,
			    CvtStringToPixmap,
			    pixmapArgs, XtNumber(pixmapArgs),
			    (XtCacheNone | XtCacheRefCount), NULL);

	XtSetTypeConverter (XmRString, XmRAnimationPixmap,
			    CvtStringToPixmap,
			    pixmapArgs, XtNumber(pixmapArgs),
			    (XtCacheNone | XtCacheRefCount), NULL);

	XtSetTypeConverter (XmRString, XmRAnimationMask,
			    CvtStringToPixmap,
			    bitmapArgs, XtNumber(bitmapArgs),
			    (XtCacheNone | XtCacheRefCount), NULL);


#endif /* _NO_PIXMAP_CONV_BC */
    }

    _XmProcessUnlock();
}


/************************************************************************
 *
 *  CvtStringToPixmap
 *
 ************************************************************************/
static Boolean
CvtStringToPixmap(
        Display *dpy,
        XrmValue *args,
        Cardinal *numArgs,
        XrmValue *fromVal,
        XrmValue *toVal,
        XtPointer *closure_ret)	/* unused */
{
   Pixmap pixmap = XmUNSPECIFIED_PIXMAP;
   String image_name = (String) (fromVal->addr);
   Widget widget ;
   unsigned char conv_type ;
   Boolean scaling;

   /* only called locally, no need to check number of arguments,
      just be sure it's 3 */

   widget = *((Widget *) args[0].addr);

   if (XmeNamesAreEqual(image_name, "none")) {
       pixmap = None ;
       _XM_CONVERTER_DONE ( toVal, Pixmap, pixmap,
	     XmDestroyPixmap(XtScreen(widget), pixmap) ;)
   }

   if (XmeNamesAreEqual(image_name, XmSunspecified_pixmap)) {
       pixmap = XmUNSPECIFIED_PIXMAP ;
       _XM_CONVERTER_DONE ( toVal, Pixmap, pixmap,
	     XmDestroyPixmap(XtScreen(widget), pixmap) ;)
   }

   conv_type = (unsigned char) (int) (long) args[1].addr;
   scaling = (unsigned char) (int) (long) args[2].addr;

   pixmap = GetPixmap (widget, conv_type, image_name, scaling) ;

   if (pixmap == XmUNSPECIFIED_PIXMAP) {
       XtDisplayStringConversionWarning(dpy, image_name,
					XmRPixmap);
       return False;
   }


   _XM_CONVERTER_DONE ( toVal, Pixmap, pixmap,
		       XmDestroyPixmap(XtScreen(widget), pixmap) ;)
}


static Pixmap
GetPixmap (
	   Widget widget,
	   unsigned char converter_type,
	   String image_name,
	   Boolean scaling)
{
   int depth ;
   Screen * screen = XtScreen(widget);
   Pixmap pixmap ;
   XmAccessColorDataRec acc_color_rec;
   double scaling_ratio ;

   if (scaling) scaling_ratio = 0 ; else scaling_ratio = 1;

   if (converter_type == CONVERT_BITMAP) {
       pixmap = XmGetScaledPixmap (widget, image_name, 1, 0, 1,
				   scaling_ratio);
       /* pass a double: 0 means scaling using print shell resolution, etc
	  and 1 means no scaling explicitly */
       /* use the widget to find the print shell which gives
	  the pixmap resolution to be applied */
       return pixmap  ;
   }

   /* else it's the CONVERT_DYNAMIC or CONVERT_PIXMAP case */

   /* ask the class for color info */
   if (!GetColorInfo (widget, &acc_color_rec))
       /* If we cannot get the colors out of the widget,
	  we have to delay the conversion.
	  So we return a magic value for the pixmap,
	  so that the conversion be done later in Initialize.
	  This probably happens for gadget when the cache is not yet
	  created at the time the pixmap is converted, so the colors
	  cannot be accessed. */
       return  XmDELAYED_PIXMAP;

   depth = DEPTH(widget);

    /* here we want the function to return a bitmap in the xbm case
      or a pixmap (match depth actually) in the xpm case.
      since it is a breakage, I can either use a new private API,
      or a private convention, useful for others who know it :-)
      the convention is -depth (depth is an int). Positive depth

      will still fetch pixmap even for xbm. */
   depth = -depth ;


   /* if PIXMAP forced, force depth to be positive, meaning
      always a pixmap even if xbm specified */
   /* ...as long as the foreground color is not XmUNSPECIFIED_PIXEL.
      Otherwise, the pixmap gets created with a bogus foreground color.
      (Metro Link fix) */
   /* this is mainly for backgroundPixmap */
   /* if the resource name was available in the converter, I could
      give it to GetColorInfo and have the class decide.. Xt problem */

   if ((depth < 0) && (acc_color_rec.foreground != XmUNSPECIFIED_PIXEL) &&
       ((converter_type == CONVERT_PIXMAP) ||
	(_XmGetBitmapConversionModel(screen) == XmMATCH_DEPTH)))
       depth = - depth ;

   pixmap = _XmGetScaledPixmap (screen, widget, image_name,
				&acc_color_rec, depth, FALSE,
				scaling_ratio, 0, 0); /* pass scaling down to
				             ImageCache */

   return pixmap ;
}




static Boolean
GetColorInfo (
	       Widget widget,
	       XmAccessColorData acc_color)
{
    XmAccessColorsTrait access_colors_trait ;

    access_colors_trait = (XmAccessColorsTrait)
	XmeTraitGet((XtPointer)XtClass(widget), XmQTaccessColors) ;

    if (access_colors_trait) {
	acc_color->valueMask = AccessForeground | AccessBackgroundPixel |
	    AccessHighlightColor | AccessTopShadowColor |
		AccessBottomShadowColor | AccessSelectColor;
	access_colors_trait->getColors(widget, acc_color) ;

	/* some widget don't have select color */
	if (!(acc_color->valueMask & AccessSelectColor))
	    acc_color->select_color = XmUNSPECIFIED_PIXEL ;

	if (acc_color->valueMask == AccessColorInvalid)
	    return False ;
    }
    else /* not one of ours, set dumb default */ {
	if (XtIsWidget(widget))
	    acc_color->background = widget->core.background_pixel;
	else
	    acc_color->background = WhitePixelOfScreen(XtScreen(widget));
	acc_color->foreground = BlackPixelOfScreen(XtScreen(widget));
	acc_color->highlight_color = acc_color->top_shadow_color =
	    acc_color->bottom_shadow_color = acc_color->select_color
		= XmUNSPECIFIED_PIXEL ;
    }

    return True ;
}



/************************************************************************
 *
 *  Dynamic defaulting pixmap functions.
 *  Usable by most classes since they query the class back for pixel
 *  and depth information.
 *
 ************************************************************************/
void
_XmTopShadowPixmapDefault(
        Widget widget,
        int offset,		/* unused */
        XrmValue *value )
{
   static Pixmap pixmap;
   XmAccessColorDataRec acc_color_rec ;
   int depth ;

   pixmap = XmUNSPECIFIED_PIXMAP;

   value->addr = (char *) &pixmap;
   value->size = sizeof (Pixmap);

   /* no need to check for return value from GetColorInfo here
      since these resources are always converted for valid
      widget or gadget */
   (void) GetColorInfo (widget, &acc_color_rec) ;

   depth = DEPTH(widget);

   /* no scaling in this case: last arg 1 */
   if (depth == 1)
       pixmap = XmGetScaledPixmap (widget, XmS50_foreground,
				   1, 0, 1, 1);
   else
       if (acc_color_rec.top_shadow_color == acc_color_rec.background) {
	   /* forces a real pixmap here, otherwise the widget will
	      still use the top_shadow_color == background as a
	      stipple, so don't negative depth  */

	   pixmap = XmGetScaledPixmap (widget,
				       XmS50_foreground,
				       acc_color_rec.top_shadow_color,
				       acc_color_rec.foreground,
				       depth, 1);
       }
}

void
_XmHighlightPixmapDefault(
        Widget widget,
        int offset,		/* unused */
        XrmValue *value )
{
   static Pixmap pixmap;
   XmAccessColorDataRec acc_color_rec ;
   int depth ;

   pixmap = XmUNSPECIFIED_PIXMAP;

   value->addr = (char *) &pixmap;
   value->size = sizeof (Pixmap);

   /* no need to check for return value from GetColorInfo here
      since these resources are always converted for valid
      widget or gadget */
   (void) GetColorInfo (widget, &acc_color_rec) ;

   depth = DEPTH(widget);

   if (acc_color_rec.highlight_color == acc_color_rec.background) {
       /* forces a real pixmap here, otherwise the widget will still use
	  the highlight_color == background as a stipple */
       pixmap = XmGetScaledPixmap  (widget, XmS50_foreground,
				    acc_color_rec.highlight_color,
				    acc_color_rec.foreground,
				    depth, 1);
   }
}




/************************************************************************
 *
 *  _XmGetPixmapBasedGC
 *     Get the graphics context used for drawing with a pixmap.
 *
 ************************************************************************/
GC
_XmGetPixmapBasedGC(
        Widget w,
        Pixel foreground,
        Pixel background,
        Pixmap pixmap)
{
   XGCValues values;
   XtGCMask  valueMask;

   valueMask = GCForeground | GCBackground;
   values.foreground = foreground;
   values.background = background;

   if ((pixmap != None) && (pixmap != XmUNSPECIFIED_PIXMAP)) {
       int depth ;

       XmeGetPixmapData(XtScreen(w), pixmap,
			NULL,
			&depth,
			NULL, NULL, NULL, NULL, NULL, NULL);

       if (depth == 1) {
	   valueMask |= GCFillStyle | GCStipple ;
	   values.fill_style = FillOpaqueStippled;
	   values.stipple = pixmap;
	   /* topShadowPixmap has a foreground=background=1 */
	   if (foreground == background) values.foreground = background?0:1;
       } else {
	   valueMask |= GCFillStyle | GCTile ;
	   values.fill_style = FillTiled;
	   values.tile = pixmap;
       }

   }

   return (XtGetGC (w, valueMask, &values));
}



