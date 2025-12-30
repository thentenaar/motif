/**
 * Motif
 *
 * Copyright (c) 2025 Tim Hentenaar
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

#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$TOG: ImageCache.c /main/44 1998/10/06 17:26:25 samborn $"
#endif
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "XmI.h"
#include "XmosI.h"              /* for mask name API and stuff */
#include "BitmapsI.h"		/* for built-in images */
#include "ColorI.h"		/* for _XmSearchColorCache() */
#include "HashI.h"		/* for hash table routines. */
#include "ImageCachI.h"		/* for DIRECT_PIXMAP_CACHED */
#include "SvgI.h"

#include <X11/xpm.h>
#include <X11/Xresource.h>
#include <X11/Xlibint.h>

#include <Xm/AccColorT.h>       /* for new _XmGetColoredPixmap API */
#include <Xm/ColorObjP.h>       /* for Xme Color Obj access API */
#include <Xm/IconFile.h>        /* XmGetIconFileName */

#if XM_WITH_JPEG
#include "JpegI.h"
#endif
#if XM_WITH_PNG
#include "PngI.h"
#endif

/* additional value for GetImage return, FALSE, TRUE and */
#define NOT_CACHED 2

/* index and max size of the override_colorsarray */
#define NUM_SYMBOLIC_COLORS       7
#define SYMB_BACKGROUND           0
#define SYMB_FOREGROUND           1
#define TRANSPARENT_COLOR         2
#define SYMB_TOP_SHADOW_COLOR     3
#define SYMB_BOTTOM_SHADOW_COLOR  4
#define SYMB_SELECT_COLOR         5
#define SYMB_HIGHLIGHT_COLOR      6

/*  Image set handling defines, structure, and global statics  */

typedef struct _ImageData
{
   int		 hot_x, hot_y;
   XImage	 *image;
   char		 *image_name;
   unsigned char *builtin_data;
} ImageData;

static XmHashTable image_set = NULL;


/*  Pixmap caching structure and global statics  */

typedef struct _PixmapData
{
   Screen * screen;
   char   * image_name;
   XmAccessColorData acc_color ;
   Pixmap   pixmap;
   int	    depth;
   Dimension width ;
   Dimension height ;
   int      reference_count;
   unsigned short print_resolution ;
   double   scaling_ratio ;
   Pixel  * pixels;
   int      npixels;
} PixmapData;

static XmHashTable pixmap_set = NULL;
static XmHashTable pixmap_data_set = NULL;


/*  Color caching structure and global statics  */

typedef struct _CachedColorStruct {
    Display	*display;
    Colormap 	 colormap;
    XrmQuark 	 colorname;
    unsigned short red, green, blue;
    Pixel    	 pixel;
    unsigned int num_cached;	/* reference count */
} CachedColor;

typedef struct _CachedColorListStruct {
    int          numEntries;
    int          maxEntries;
    CachedColor  *cache;
} CachedColorList;

static CachedColorList colorCacheList;


/*  GC caching structure and global statics  */

typedef struct _GCData
{
    GC		gc;
    Screen     *screen;
    int		depth;
    int		image_depth;
    Pixel	foreground;
    Pixel	background;
} GCData;

static XmHashTable gc_set = NULL;

/********    Static Function Declarations    ********/

static Boolean ComparePixmaps (XmHashKey key_1, XmHashKey key_2);
static Boolean ComparePixmapDatas (XmHashKey key_1, XmHashKey key_2);
static Boolean CompareStrings (XmHashKey key_1, XmHashKey key_2);
static Boolean CompareGCDatas (XmHashKey key_1, XmHashKey key_2);
static XmHashValue HashPixmap (XmHashKey key);
static XmHashValue HashPixmapData (XmHashKey key);
static XmHashValue HashString (XmHashKey key);
static XmHashValue HashGCData (XmHashKey key);
static void InitializeImageSet( void ) ;
static void InitializePixmapSets( void ) ;
static Boolean UninstallImageMapProc (XmHashKey key, XtPointer value,
			       XtPointer image);
static Boolean SymbolicColorUsed(String color_name,
				 XpmColor * xpm_colors,
				 unsigned int ncolors);
static void CompleteUnspecColors(
			       Screen *screen,
			       XpmColorSymbol *override_colors);
static int GetOverrideColors(
			    Screen *screen,
			    XmAccessColorData acc_color,
			    XpmColorSymbol *override_colors);
static XtEnum GetXpmImage(Screen *screen, char *image_name, char *file_name,
                          XmAccessColorData acc_color, XImage **image,
                          unsigned short *pixmap_resolution,
                          Pixel **pixels, int *npixels);
static XtEnum GetImage(Screen *screen, char *image_name,
		       XmAccessColorData acc_color, XImage **image,
		       unsigned short * pixmap_resolution,
		       Pixel **pixels,	/* allocated pixels */
		       int *npixels);
static GC GetGCForPutImage(Screen *screen,
			   XImage * image,
			   Pixmap pixmap,
			   int depth,
			   Pixel foreground,
			   Pixel background);

static int GetCacheColor(Display *display, Colormap colormap,
			 char *colorname, XColor *color, void *data);
static int FreeCacheColors(Display *display, Colormap colormap,
			   Pixel *pixels, int npixels, void *data);

/********    End Static Function Declarations    ********/




/*** IMAGE CACHE PART FIRST ***/




/* Compare two strings. */
static Boolean
CompareStrings (XmHashKey key_1,
		XmHashKey key_2)
{
  char *data_1 = (char *) key_1;
  char *data_2 = (char *) key_2;

  return ((data_1 == data_2) || (strcmp(data_1, data_2) == 0));
}


/* Hash a string. */
static XmHashValue
HashString (XmHashKey key)
{
  char *data = (char *) key;
  unsigned int len = strlen(data);

  return (((len << 8) | data[0]) << 8) | data[len];
}

/************************************************************************
 *
 *  InitializeImageSet
 *	Initialize the image set.
 *
 ************************************************************************/
static void
InitializeImageSet( void )
{
  int i;

  /* Allocate the hash table. */
  assert (image_set == NULL);

  _XmProcessLock();
  image_set =
    _XmAllocHashTable (MAX_BUILTIN_IMAGES + 100, CompareStrings, HashString);

  /* Load the built-in image data.
     Builtins have a non NULL builtin data.
     Their image field is setdynamically at GetImage time */
  for (i = 0; i < MAX_BUILTIN_IMAGES ; i++)
    {
      ImageData *entry = XtNew(ImageData);

      entry->hot_x = 0;
      entry->hot_y = 0;
      entry->image = NULL;
      entry->image_name = (char*) bitmap_name_set[i];
      entry->builtin_data = (unsigned char *) bitmaps[i];

      _XmAddHashEntry(image_set, entry->image_name, entry);
    }
  _XmProcessUnlock();
}

/************************************************************************
 *
 *  _XmInstallImage
 *
 *		Allow a hot_spot to be specified
 *
 ************************************************************************/
Boolean
_XmInstallImage(
        XImage *image,
        char *image_name,
	int hot_x,
	int hot_y)
{
   ImageData *entry;


   /*  Error checking  */

   if (image == NULL || image_name == NULL) return (False);


   /*  Check for the initial allocation of the image set array  */

   if (image_set == NULL) InitializeImageSet();


   /*  Verify that the image_name is not already in the image set.  */
   _XmProcessLock();

   if (_XmGetHashEntry(image_set, image_name) != NULL) {
     _XmProcessUnlock();
     return (False);
   }


   /*  Initialize the image element for the new image and return True.  */

   entry = XtNew(ImageData);
   entry->hot_x = hot_x;
   entry->hot_y = hot_y;
   entry->image = image;
   entry->image_name = XtNewString(image_name);
   entry->builtin_data = NULL;

   _XmAddHashEntry(image_set, entry->image_name, entry);

   _XmProcessUnlock();
   return (True);
}

/************************************************************************
 *
 *  XmInstallImage
 *	Add the provided image for the image set and return an
 *	id to be used for further referencing.
 *
 ************************************************************************/
Boolean
XmInstallImage(
        XImage *image,
        char *image_name )
{
    Boolean	ret_val;

    _XmProcessLock();
    ret_val = _XmInstallImage(image, image_name, 0, 0);
    _XmProcessUnlock();

    return ret_val;
}
/* A hash table map procedure to uninstall an image. */

static Boolean
UninstallImageMapProc (XmHashKey key, /* unused */
		XtPointer value,
		XtPointer image)
{
  ImageData *entry = (ImageData *) value;

  if (entry->image == (XImage*) image)
    {
      /* Can't free built-in data. */
      if (entry->builtin_data == NULL)
	{
	  _XmProcessLock();
	  _XmRemoveHashEntry (image_set, entry->image_name);
	  _XmProcessUnlock();
	  XtFree(entry->image_name);
	  /* uninstall do not free the XImage itself */
	  XtFree((char*) entry);
	}

      return True;
    }

  return False;
}

/************************************************************************
 *
 *  XmUninstallImage
 *	Remove an image from the image set.
 *	Return a boolean (True) if the uninstall succeeded.  Return
 *	a boolean (False) if an error condition occurs.
 *
 ************************************************************************/
Boolean
XmUninstallImage(
        XImage *image )
{
   Cardinal old_count;
   Boolean	ret_val;

   /*  Check for invalid conditions  */

   if ((image == NULL) || (image_set == NULL)) return (False);

   _XmProcessLock();
   /*  Since we can't index based on the image, search the hash table */
   /*  until the desired entry is found and then remove it.	      */

   old_count = _XmHashTableCount(image_set);

   _XmMapHashTable(image_set, UninstallImageMapProc, image);
   ret_val = (old_count > _XmHashTableCount(image_set));
   _XmProcessUnlock();

   return ret_val;
}



/************************************************************************
 *
 * SymbolicColorUsed
 *  Used to determine if a given color string is defined as a symbolic
 *   color in an Xpm image. This is used for caching policy where we
 *   don't want to cache based on irrelevant colors.
 *  Note that we don't look if the color is actually used because it
 *   is too expensive.
 *
 ************************************************************************/
static Boolean
SymbolicColorUsed(String color_name,
		  XpmColor * xpm_colors,
		  unsigned int ncolors)
{
    Cardinal i;

    /* first look if the color is present as a symbolic in the
       colorTable */
    for (i = 0; i < ncolors; i++, xpm_colors++) {
	if (xpm_colors->symbolic &&
	    !strcmp(xpm_colors->symbolic, color_name))
	    break ;
    }

    if (i == ncolors) return False ;

    return False ;
}


/************************************************************************
 *
 *  CompleteUnspecColors
 *     Complete the unspecified_pixel values of an override_colors array
 *      based on the values coming from the ColorObj or the color cache.
 *     Does not handle highlight.
 *     The logic here comes directly from CDE.
 *
 *  The above description does not fit with the actual logic of CDE where
 *  not only the unspecified colors are set here but the whole set of
 *  symbols! See defect CDExc17529.
 *
 ************************************************************************/
static void
CompleteUnspecColors(
		Screen *screen,
		XpmColorSymbol *override_colors)
{
    XmPixelSet 	pixelSets[XmCO_NUM_COLORS];
    int 	colorUse ;
    Boolean	bgFound = False, fgFound = False, result = False;
    Pixel	*pixelPtr;
    Cardinal i, j ;
    XmAccessColorDataRec loc_acc_color;


    /* Ask the color obj */
    if (!(result = XmeGetColorObjData(screen,
				      &colorUse, pixelSets, XmCO_NUM_COLORS,
				      NULL, NULL, NULL, NULL, NULL)))
      i = XmCO_NUM_COLORS;
    else
      i = 0;

    /* Look for a pixels set containing the given fg and bg,
     * no matter whether they are actually the fg and bg of the set...
     */
    for ( ; i < XmCO_NUM_COLORS; i++) {
	bgFound = False;
	fgFound = False;
	pixelPtr = (Pixel *)&(pixelSets[i].fg);
	for (j = 0; j < 5; j++, pixelPtr++) {
	    if (*pixelPtr == override_colors[SYMB_BACKGROUND].pixel)
	      bgFound = True;
	    else if (*pixelPtr == override_colors[SYMB_FOREGROUND].pixel)
	      fgFound = True;
	}
	if (bgFound && fgFound)
	  break;
    }

    if (i == XmCO_NUM_COLORS) {
	/*
	 * We didn't find the bg/fg tuple in any of the Dt colorsets
	 * or if the color server is not running, we will now try
	 * the color cache.
	 */

      XmColorData *old_colors;
      XmColorData new_colors;

      loc_acc_color.background = override_colors[SYMB_BACKGROUND].pixel ;

      new_colors.screen = screen;
      new_colors.color_map = DefaultColormapOfScreen(screen);
      new_colors.background.pixel = loc_acc_color.background ;

      /* Use motif color set only if already allocated.
       */
      if (!result && _XmSearchColorCache(
		 (XmLOOK_AT_SCREEN | XmLOOK_AT_CMAP | XmLOOK_AT_BACKGROUND),
					 &new_colors, &old_colors)) {
	  XmGetColors(screen,
		      DefaultColormapOfScreen(screen),
		      loc_acc_color.background,
		      &loc_acc_color.foreground,
		      &loc_acc_color.top_shadow_color,
		      &loc_acc_color.bottom_shadow_color,
		      &loc_acc_color.select_color);
      } else {
	  /* Cannot generate missing colors */
	  return ;
       }
    }
    else {
	loc_acc_color.background = pixelSets[i].bg;
	loc_acc_color.foreground = pixelSets[i].fg;
	loc_acc_color.top_shadow_color = pixelSets[i].ts;
	loc_acc_color.bottom_shadow_color = pixelSets[i].bs;
	loc_acc_color.select_color = pixelSets[i].sc;
    }

    /* Now process the setting for unspecified pixel values.
       If we're here, all the colors but highlight are available */

    override_colors[SYMB_BACKGROUND].pixel = loc_acc_color.background ;

    override_colors[SYMB_FOREGROUND].pixel = loc_acc_color.foreground ;

    override_colors[SYMB_TOP_SHADOW_COLOR].pixel =
	loc_acc_color.top_shadow_color ;

    override_colors[SYMB_BOTTOM_SHADOW_COLOR].pixel =
	loc_acc_color.bottom_shadow_color ;

    override_colors[SYMB_SELECT_COLOR].pixel = loc_acc_color.select_color ;
}


/************************************************************************
 *
 *  GetOverrideColors
 *   Given a set of colors in acc_color with values specified or not
 *   this function fills up an XpmColorSymbol override_colors array to
 *   be used by the Xpm reader.
 *   It then calls the ColorObject aware filling proc to find values
 *   for the still unspecified ones.
 *   It returns the number of entries to consider in override_colors,
 *   which can be 2 (background and foreground are always valid),
 *   NUM_SYMBOLIC_COLORS - 1 (if everything but the highlight could
 *   be generated), or NUM_SYMBOLIC_COLOR.
 *
 ************************************************************************/

static int
GetOverrideColors(
		Screen *screen,
		XmAccessColorData acc_color,
		XpmColorSymbol *override_colors)
{
    Cardinal i, n ;

    /* init the value fields */
    for (i=0; i<NUM_SYMBOLIC_COLORS; i++) override_colors[i].value = NULL ;

    /* proceed each color in turn: acc_color might point to
       XmUNSPECIFIED_PIXEL, but background/foreground should be valid */

    override_colors[SYMB_BACKGROUND].name = XmNbackground ;
    override_colors[SYMB_BACKGROUND].pixel = acc_color->background ;

    override_colors[SYMB_FOREGROUND].name = XmNforeground ;
    override_colors[SYMB_FOREGROUND].pixel = acc_color->foreground ;

    /* Set "none" color to background in case the mask is not used. */
    override_colors[TRANSPARENT_COLOR].name = NULL ;
    override_colors[TRANSPARENT_COLOR].value = "None" ;
    override_colors[TRANSPARENT_COLOR].pixel = acc_color->background ;

    override_colors[SYMB_TOP_SHADOW_COLOR].name = XmNtopShadowColor ;
    override_colors[SYMB_TOP_SHADOW_COLOR].pixel =
	acc_color->top_shadow_color ;

    override_colors[SYMB_BOTTOM_SHADOW_COLOR].name = XmNbottomShadowColor ;
    override_colors[SYMB_BOTTOM_SHADOW_COLOR].pixel =
	acc_color->bottom_shadow_color ;

    override_colors[SYMB_SELECT_COLOR].name = XmNselectColor ;
    override_colors[SYMB_SELECT_COLOR].pixel = acc_color->select_color ;
    /* need to hack around selectColor which might be still
       unspecified while highlight is not (and that woudl break our
       ordering rule - see below). Use top_shadow color so that select
       follows its set/unset semantics regarding overriding */
    if (override_colors[SYMB_SELECT_COLOR].pixel == XmUNSPECIFIED_PIXEL)
	override_colors[SYMB_SELECT_COLOR].pixel =
	    override_colors[SYMB_TOP_SHADOW_COLOR].pixel ;

    override_colors[SYMB_HIGHLIGHT_COLOR].name = XmNhighlightColor ;
    override_colors[SYMB_HIGHLIGHT_COLOR].pixel = acc_color->highlight_color ;



    /* now call ColorObject aware routine to give it a chance to
       fill the remaining unspecified pixel field in override_colors */

    CompleteUnspecColors (screen, override_colors) ;

    /* Just count the number of valid pixel.
       This makes a strong assumption on the ordering: if we find
       5 valid pixels, the 5 first one will be used.
       This is ok since _XmGetColoredPixmap is only used internally
       in such a way that this happens to be true: background/foreground
       are always here, and highlight color is the only one
       that might be missing after CompleteUnspecColors call.
       The day XmGetColoredPixmap is promoted public, this simple
       logic will have to be reviewed */

    n = 0 ;
    for (i=0; i<NUM_SYMBOLIC_COLORS; i++)
	if (override_colors[i].pixel != XmUNSPECIFIED_PIXEL) n++;

    return n ;
}

static XtEnum
GetXpmImage(
        Screen *screen,
        char *image_name, /* original image file name */
        char *file_name,
        XmAccessColorData acc_color,
        XImage **image,
        unsigned short *pixmap_resolution,
        Pixel **pixels,
        int *npixels)
{
    XpmAttributes attrib;
    int xpmStatus;
    Boolean useIconFileCache;
    Boolean useMask;
    Boolean useColor;
    XpmColorSymbol override_colors[NUM_SYMBOLIC_COLORS]; /* max */
    int num_override_colors;
    XImage * mask_image = NULL ;
    int hot_x = 0 , hot_y = 0 ;
    unsigned int w, h;
    unsigned char *data;
    Display *display = DisplayOfScreen(screen);

    /* init so that we can call safely XpmFreeAttributes. */
    attrib.valuemask = 0;

    /* Init the Xpm attributes to be passed to the reader */
    attrib.closeness = 40000;
    attrib.bitmap_format = XYBitmap;
    attrib.alloc_color = GetCacheColor;
    attrib.free_colors = FreeCacheColors;
    attrib.valuemask = XpmCloseness | XpmBitmapFormat | XpmReturnColorTable
	| XpmAllocColor | XpmFreeColors | XpmReturnAllocPixels;

    /* if any symbolic color are defined, used them */
    if (acc_color &&
	(num_override_colors = GetOverrideColors(screen,
						 acc_color,
						 override_colors)) != 0) {
	attrib.colorsymbols = override_colors;
	attrib.numsymbols = num_override_colors;
	attrib.valuemask |= XpmColorSymbols;
    }

    /* ask the color object for information about the
       use of mask or color */
    (void)XmeGetIconControlInfo(screen,
				&useMask,
				&useColor,
				&useIconFileCache); /* unused */

    if (!useColor) {
	attrib.depth = 1;
	attrib.valuemask |= XpmDepth;
    }

    *image = NULL ;

    xpmStatus = XpmReadFileToImage(display,
			      file_name,
			      image,
			      &mask_image,
			      &attrib);
    if (xpmStatus < 0)
	*image = NULL ;
    else {
	/* store allocated colors */
	if (pixels) *pixels = attrib.alloc_pixels;
	if (npixels) *npixels = attrib.nalloc_pixels;
	/* and make sure they won't be free'd */
	attrib.alloc_pixels = NULL;
	attrib.nalloc_pixels = 0;
    }

    if (!*image) {
		if (XReadBitmapFileData(file_name, &w, &h, &data, &hot_x, &hot_y) == BitmapSuccess)
			_XmCreateImage(*image, display, (char *)data, w, h, LSBFirst);
    }

    /* get the image "design" resolution */
    /* in the future: look in the file */
    if (pixmap_resolution) *pixmap_resolution = 0 ;


    if (*image) {

	/* if the XPM file contained a embedded mask,
	   install it using our mask name scheme if the color object
	   tell us to do so */
	if (mask_image && useMask) {
	    char mask_name[255] ;

	    _XmOSGenerateMaskName(image_name, mask_name, sizeof mask_name);
	    /* if an image already exist under that
	       name, nothing will be done */
	    _XmInstallImage (mask_image, mask_name, hot_x, hot_y);
	}

	/* now we have to adjust the passed acc_color */
	if (acc_color) {
	    if ((*image)->depth == 1) {
		/* we've loaded an xbm file, just forget about
		   the 'other' symbolic colors, for they
		   are not going to be used during pixmap
		   generation for depth 1 => pixmap putimage */
		acc_color->top_shadow_color = XmUNSPECIFIED_PIXEL;
		acc_color->bottom_shadow_color= XmUNSPECIFIED_PIXEL;
		acc_color->select_color = XmUNSPECIFIED_PIXEL;
		acc_color->highlight_color = XmUNSPECIFIED_PIXEL;
		if (acc_color->foreground == XmUNSPECIFIED_PIXEL
		    && acc_color->background == XmUNSPECIFIED_PIXEL) {
		    acc_color->foreground = 1;
		    acc_color->background = 0;
		}
	    } else {

		/* we've loaded a xpm, check which symbolics
		   colors were actually used during the read,
		   we don't want to remember the unused one
		   for the pixmap caching */

		if (!SymbolicColorUsed(XmNbackground,
				       attrib.colorTable,
				       attrib.ncolors))
		    acc_color->background = XmUNSPECIFIED_PIXEL;

		if (!SymbolicColorUsed(XmNforeground,
				       attrib.colorTable,
				       attrib.ncolors))
		    acc_color->foreground = XmUNSPECIFIED_PIXEL;

		if (!SymbolicColorUsed(XmNtopShadowColor,
				       attrib.colorTable,
				       attrib.ncolors))
		    acc_color->top_shadow_color = XmUNSPECIFIED_PIXEL;

		if (!SymbolicColorUsed(XmNbottomShadowColor,
				       attrib.colorTable,
				       attrib.ncolors))
		    acc_color->bottom_shadow_color = XmUNSPECIFIED_PIXEL;

		if (!SymbolicColorUsed(XmNselectColor,
				       attrib.colorTable,
				       attrib.ncolors))
		    acc_color->select_color = XmUNSPECIFIED_PIXEL;

		if (!SymbolicColorUsed(XmNhighlightColor,
				       attrib.colorTable,
				       attrib.ncolors))
		    acc_color->highlight_color = XmUNSPECIFIED_PIXEL;

	    }
	}


	/* install the XImage with that name.
	   We do not cache the non-depth 1 ones
	   for we don't want to keep them in the image
	   cache, since they need color lookup,
	   which is done one level up in the pixmap cache */
	if (((*image)->depth == 1)
            && acc_color
	    && (acc_color->foreground == 1)
	    && (acc_color->background == 0)
	    ) {
	    _XmInstallImage (*image, image_name, hot_x, hot_y);
	    return TRUE ;
	} else {
	    if (xpmStatus >= 0)
		XpmFreeAttributes(&attrib);
	    return NOT_CACHED ; /* mean the image can be destroyed
				   after it is used */
	}
    }

    if (xpmStatus >= 0)
	XpmFreeAttributes(&attrib);

    return FALSE;
}

/**
 * Load an image from a file
 */
static XtEnum LoadImage(Screen *screen, char *image_name,
                        XmAccessColorData acc_color,
                        XImage **image,
                        unsigned short *pixmap_res,
                        Pixel **pixels,
                        int *npixels)
{
	FILE *fp;
	char *fname;
	XColor bg;
	XtEnum ret = False;
	unsigned char header[4];

	fname = XmGetIconFileName(screen, NULL, image_name,
	                          NULL, XmUNSPECIFIED_ICON_SIZE);
	if (!fname)
		return False;

	if (!(fp = fopen(fname, "rb"))) {
		XtFree(fname);
		return False;
	}

	if (!fread(header, sizeof header, 1, fp)) {
		fclose(fp);
		XtFree(fname);
		return False;
	}

	/* Grab the background color from the colormap */
	bg.pixel = 0;
	if (acc_color)
		bg.pixel = acc_color->background;
	XQueryColor(screen->display, screen->cmap, &bg);
	rewind(fp);

	switch (header[0]) {
	case '<':  /* SVG */
		if (!_XmSvgGetImage(fp, image))
			return True;
		break;
	case 0xff: /* 0xff 0xd8 - JPEG/Exif SOI */
#if XM_WITH_JPEG
		if (header[1] != 0xd8)
			break;

		ret = !_XmJpegGetImage(fp, image) ? NOT_CACHED : False;
#endif
		break;
	case 0x89: /* 0x89 'P' 'N' 'G' - PNG */
#if XM_WITH_PNG
		if (memcmp((char *)header + 1, "PNG", 3))
			break;

		ret = !_XmPngGetImage(fp, &bg, image) ? NOT_CACHED : False;
#endif
		break;
	default: /* Try XPM */
		ret = GetXpmImage(screen, image_name, fname, acc_color,
		                  image, pixmap_res, pixels, npixels);
	}

	fclose(fp);
	XtFree(fname);
	return ret;
}

/************************************************************************
 *
 *  GetImage
 *	Main routine of the image cache part.
 *
 ************************************************************************/
static XtEnum
GetImage(
	 Screen *screen,
	 char *image_name,
	 XmAccessColorData acc_color,
	 XImage **image,
	 unsigned short * pixmap_resolution,
	 Pixel **pixels,	/* allocated pixels */
	 int *npixels)
{
    static XImage  * built_in_image = NULL;
    Display *display = DisplayOfScreen(screen);
    ImageData *entry;

    /* init for when we go thru the image cache first */
    if (pixmap_resolution) *pixmap_resolution = 0 ;

    if (pixels) *pixels = NULL;
    if (npixels) *npixels = 0;

    /***  Check for the initial allocation of the image set array  */

    if (image_set == NULL) InitializeImageSet();

    if (!image_name) return FALSE ;

    /*** look in the XImage cache first */
    _XmProcessLock();
    entry = (ImageData*) _XmGetHashEntry(image_set, image_name);
    _XmProcessUnlock();


    if (entry) {

	/*  If the image is a builtin image then get it.  */

	if (entry->builtin_data) {
	    /* there is one builtin XImage shared by all builtins,
	       only the data part change every time a query is made */
	    _XmProcessLock();
	    if (!built_in_image) {
		/* update that with new R6 init image stuff when out */
		_XmCreateImage(built_in_image, display, NULL,
			       16, 16, MSBFirst);
	    }

	    built_in_image->data = (char *) entry->builtin_data;
	    _XmProcessUnlock();
	    *image = built_in_image;

	} else {

	    /* other entry found are just fine, set the image and return  */

	    *image = entry->image ;
	}

	return TRUE;
    }

    /*** if no entry, try to read a new file and cache it
         only if it is a bitmap */
    if (!acc_color) return FALSE;
    return LoadImage(screen, image_name, acc_color, image,
                     pixmap_resolution, pixels, npixels);
}

/**
 * This function is unused, and can serve to fetch a cache entry only.
 *
 * To load an image, _XmGetScaledPixmap() should be used instead.
 *
 ** Keep this one in here, this is the only entry point to
 ** the Image cache. It can be used to duplicate them, etc
 */
Boolean
_XmGetImage(
	Screen *screen,
	char *image_name,
	XImage **image )
{
    return GetImage(screen, image_name, NULL, image, NULL, NULL, NULL) ;
}

/************************************************************************
*
*  _XmInImageCache
*       Used by IconFile.c
*
************************************************************************/

Boolean
_XmInImageCache(
	String image_name)
{
    XtPointer	ret_val;

    if (!image_set) return False ;
    _XmProcessLock();
    ret_val = _XmGetHashEntry(image_set, image_name);
    _XmProcessUnlock();

    return (ret_val != NULL) ;
}


/*** PIXMAP CACHE NOW ***/

/** static dependencies with Image part:
  * GetImage(), ImageData&image_set, HashString
  **/
/** static dependencies with Color part:
  * FreeCacheColors()
  **/




static Boolean
ComparePixmaps (XmHashKey key_1,
		XmHashKey key_2)
{
  PixmapData *data_1 = (PixmapData *) key_1;
  PixmapData *data_2 = (PixmapData *) key_2;

    /* Check for a matching pixmap using screen and pixmap */

    return
	(data_1->screen == data_2->screen &&
	 data_1->pixmap == data_2->pixmap) ;
}

static Boolean
ComparePixmapDatas (XmHashKey key_1,
		    XmHashKey key_2)
{
  PixmapData *data_1 = (PixmapData *) key_1;
  PixmapData *data_2 = (PixmapData *) key_2;

    /* Check for a matching pixmap using depth, screen, name and
       colors info + print_resolution/scaling_ratio */
    /* if a negative depth is given, we must look for a matching
       pixmap of -depth or 1 */

    return
	((data_2->pixmap == XmUNSPECIFIED_PIXMAP ||
	  data_1->pixmap == data_2->pixmap) &&
	 data_1->image_name != NULL &&
	 data_2->image_name != NULL &&
	 strcmp (data_1->image_name, data_2->image_name) == 0  &&

	 (data_1->screen == data_2->screen) &&

	 ( /* ratio == 0 means print resolution is used */
	  (data_1->scaling_ratio == data_2->scaling_ratio &&
	   data_1->scaling_ratio) ||
	  (data_1->print_resolution == data_2->print_resolution &&
	   !data_1->scaling_ratio)) &&

	 ((data_1->depth == data_2->depth)  ||
	  (data_2->depth < 0 && data_1->depth == -data_2->depth) ||
	  (data_2->depth < 0 && data_1->depth == 1)) &&

	 ((data_1->acc_color->foreground == data_2->acc_color->foreground) ||
	  (data_2->depth < 0 && data_1->acc_color->foreground == 1) ||
	  (data_1->acc_color->foreground == XmUNSPECIFIED_PIXEL) ||
	  (data_2->acc_color->foreground == XmUNSPECIFIED_PIXEL))          &&
	 ((data_1->acc_color->background == data_2->acc_color->background) ||
	  (data_2->depth < 0 && data_1->acc_color->background == 0) ||
	  (data_1->acc_color->background == XmUNSPECIFIED_PIXEL)||
	  (data_2->acc_color->background == XmUNSPECIFIED_PIXEL))          &&

	 ((data_1->acc_color->top_shadow_color ==
	   data_2->acc_color->top_shadow_color) ||
	  (data_1->acc_color->top_shadow_color == XmUNSPECIFIED_PIXEL)||
	  (data_2->acc_color->top_shadow_color == XmUNSPECIFIED_PIXEL))     &&
	 ((data_1->acc_color->bottom_shadow_color
	   == data_2->acc_color->bottom_shadow_color)||
	  (data_1->acc_color->bottom_shadow_color == XmUNSPECIFIED_PIXEL)||
	  (data_2->acc_color->bottom_shadow_color == XmUNSPECIFIED_PIXEL))  &&
	 ((data_1->acc_color->select_color ==
	   data_2->acc_color->select_color) ||
	  (data_1->acc_color->select_color == XmUNSPECIFIED_PIXEL)||
	  (data_2->acc_color->select_color == XmUNSPECIFIED_PIXEL))       &&
	 ((data_1->acc_color->highlight_color ==
	   data_2->acc_color->highlight_color) ||
	  (data_1->acc_color->highlight_color == XmUNSPECIFIED_PIXEL) ||
	  (data_2->acc_color->highlight_color == XmUNSPECIFIED_PIXEL))) ;
}

/* Hash a Pixmap entry. */
static XmHashValue
HashPixmap (XmHashKey key)
{
  PixmapData *data = (PixmapData *) key;

  return ((long)data->screen + data->pixmap);
}

/* Hash a PixmapData entry. Reuse the string hash function */
static XmHashValue
HashPixmapData (XmHashKey key)
{
  PixmapData *data = (PixmapData *) key;

  return ((long)data->screen + HashString ((XmHashKey)data->image_name));
}

/************************************************************************
 *
 *  InitializePixmapSets
 *	Initialize the pixmap sets.
 *
 ************************************************************************/
static void
InitializePixmapSets( void )
{
    /* Allocate the pixmap hash tables.
     One for the regular XmGetPixmap lookup, one for the
     XmeGetPixmapData lookup */
    assert (pixmap_data_set == NULL && pixmap_set == NULL);

    _XmProcessLock();
    pixmap_data_set = _XmAllocHashTable (100, ComparePixmapDatas,
					 HashPixmapData);

    pixmap_set = _XmAllocHashTable (100, ComparePixmaps, HashPixmap);
    _XmProcessUnlock();

}

/************************************************************************
 *
 *  _XmCachePixmap
 *	Install a pixmap into the pixmap cache.  This is used to add
 *	cached pixmaps which have no XImage associated with them.
 *      This one you can pass depth and size info, if you don't,
 *      the X server will be queried: very expensive.
 *
 ************************************************************************/
Boolean
_XmCachePixmap(
        Pixmap pixmap,
        Screen *screen,
        char *image_name,
        Pixel foreground,
        Pixel background,
        int depth,
        Dimension width,
        Dimension height)
{
   PixmapData * pix_entry;

   Window root;
   int x,y;
   unsigned int loc_width, loc_height, border_width, loc_depth;

   /*  Error checking  */
   if (image_name == NULL) return (False);

   if (!pixmap_data_set) InitializePixmapSets() ;

   /* if no information was given, get it from the server */
   if (!(width && height && depth))
       XGetGeometry(DisplayOfScreen(screen), pixmap, &root, &x, &y,
		    &loc_width, &loc_height, &border_width, &loc_depth);
   else {
	   loc_width = width ;
	   loc_height = height ;
	   loc_depth = depth ;
       }

   /*  Allocate the cache structure and put it into the list  */

   pix_entry = XtNew (PixmapData);

   pix_entry->screen = screen;
   pix_entry->acc_color = XtNew(XmAccessColorDataRec);
   pix_entry->acc_color->foreground = foreground;
   pix_entry->acc_color->background = background;
   pix_entry->acc_color->top_shadow_color = XmUNSPECIFIED_PIXEL;
   pix_entry->acc_color->bottom_shadow_color = XmUNSPECIFIED_PIXEL;
   pix_entry->acc_color->select_color = XmUNSPECIFIED_PIXEL;
   pix_entry->acc_color->highlight_color = XmUNSPECIFIED_PIXEL;
   pix_entry->depth = loc_depth;
   pix_entry->width = loc_width;
   pix_entry->height = loc_height;
   pix_entry->image_name = XtNewString(image_name);
   pix_entry->pixmap = pixmap;
   pix_entry->reference_count = 1;
   pix_entry->print_resolution = 100 ;
   pix_entry->scaling_ratio = 1 ;
   pix_entry->pixels = NULL;
   pix_entry->npixels = 0;

   _XmProcessLock();
   _XmAddHashEntry(pixmap_set, (XmHashKey)pix_entry, (XtPointer)pix_entry);

   /* Only add pixmaps not issued from XmeGetPixmapData to the "real" table */
   if (0 != strcmp(image_name, DIRECT_PIXMAP_CACHED))
       _XmAddHashEntry(pixmap_data_set, (XmHashKey)pix_entry,
		       (XtPointer)pix_entry);
   _XmProcessUnlock();

   return (True);
}



/********************************************************************/

Boolean
_XmGetPixmapData(
		   Screen *screen,
		   Pixmap pixmap,
		   char **image_name,
		   int *depth,
		   Pixel *foreground,
		   Pixel *background,
		   int *hot_x,
		   int *hot_y,
		   unsigned int *width,
		   unsigned int *height)
/* no pixmap_resolution returned... */
{
    PixmapData pix_data, *pix_entry;
    ImageData * entry;

    if (!pixmap_data_set) InitializePixmapSets() ;

    /*  checks for a matching screen and pixmap.            */

    pix_data.screen = screen ;
    pix_data.pixmap = pixmap ;

    _XmProcessLock();
    if ((pix_entry = (PixmapData*)
	 _XmGetHashEntry(pixmap_set, (XmHashKey)&pix_data)) != NULL) {

	*foreground = pix_entry->acc_color->foreground;
	*background = pix_entry->acc_color->background;
	*depth = pix_entry->depth;
	*image_name = pix_entry->image_name;
	*width = pix_entry->width;
	*height = pix_entry->height;
	/* try to get the hot spot data from the image cache */
	if (image_set)  {
	    entry = (ImageData *) _XmGetHashEntry(image_set, *image_name);
	    if (entry) {
		*hot_x = entry->hot_x;
		*hot_y = entry->hot_y;
	    }
	}
	_XmProcessUnlock();
	return True;
    }
    _XmProcessUnlock();
    return False ;
}



/*******************************************************************/
/* the real one, used by PixConv and locally too */
Pixmap
_XmGetScaledPixmap(
    Screen *screen,
    Widget widget,
    char *image_name,
    XmAccessColorData acc_color,
    int depth,
    Boolean only_if_exists,
    double scaling_ratio,
    int desired_w,
    int desired_h)
{
    Display * display = DisplayOfScreen(screen);
    XImage * image;
    Pixmap   pixmap;
    GC gc ;
    XtEnum ret ;
    PixmapData pix_data, *pix_entry ;
    unsigned short pixmap_resolution ;
    Pixel *pixels;		/* allocated colors */
    int npixels;
    int old_image_format;
    char *old_image_data = NULL;

    /*  Error checking  */
    if (image_name == NULL) return (XmUNSPECIFIED_PIXMAP);

    if (!pixmap_data_set) InitializePixmapSets() ;

    /* if screen not provided, widget has to be! */
    if (!screen) screen = XtScreen(widget);

    /* since more than one printers can attach to one screen,
       we need to use the print shell in the caching */
    /* since resolution can be dynamic for a printer, we need
       to use the resolution too, when scaling is 0 */
    pix_data.screen = screen ;
    pix_data.image_name = image_name ;
    pix_data.depth = depth ;
    pix_data.acc_color = acc_color ;
    pix_data.print_resolution = 100 ; /* default */
    pix_data.scaling_ratio = scaling_ratio ;
    pix_data.pixmap = XmUNSPECIFIED_PIXMAP;

    if (desired_h) pix_data.height = desired_h;
    if (desired_w) pix_data.width  = desired_w;

    /* if scaling_ratio a real number, like 1 or 1.2
       print_resolution still 100 and will not be used */

    /* look if we have a match and return it */
    _XmProcessLock();
    if ((pix_entry = (PixmapData*)
	 _XmGetHashEntry(pixmap_data_set, (XmHashKey)&pix_data)) != NULL) {
	if ((!desired_w || (desired_w && pix_entry->width  == desired_w)) &&
	    (!desired_h || (desired_h && pix_entry->height == desired_h))) {
	    pix_entry->reference_count++;
	    _XmProcessUnlock();
		return pix_entry->pixmap ;
	}
    }
    _XmProcessUnlock();

    /* The desired pixmap is not already cached.  Quit now or create it. */
    if (only_if_exists)
	return (XmUNSPECIFIED_PIXMAP);

    /* no pixmap found, look for an XImage.
       generate a new one if needed.
       GetImage can return a bitmap or a pixmap XImage */
    /* GetImage modify the acc_color struct to update the
       colors that are not used during reading:
         - for xbm, everything but foreground and background.
	 - for xpm, on a case by case basis, depending
	 if the corresponding symbolic color was used. */
    if (!(ret = GetImage(screen, image_name, acc_color, &image,
			 &pixmap_resolution, &pixels, &npixels)))
	return (XmUNSPECIFIED_PIXMAP);

    /* now find how much we need to scale this baby */

    /* compute the real ratio to be used in PutImage */
    if (!pix_data.scaling_ratio) {
	/* if scaling ratio is real, like 1.2, the pixmap resolution is
	   not used, neither is the print_resolution */

	if (!pixmap_resolution) {
	    /* GetImage returns the resolution for which the image
	       was designed. if none was provided in the file, it's 0,
	       in which case, the print shell default resolution
	       for pixmap is used */

		pixmap_resolution = 100 ;
	}

	pix_data.scaling_ratio = (double)pix_data.print_resolution /
	    (double)pixmap_resolution ;
    }


    /* now we treat the -depth case: if a negative depth was given
       then xbm file should map into bitmap and xpm into pixmap */

    if (depth < 0) {
	if (image->depth == 1)
	  depth = 1 ;
	else
	  depth = -depth ;
    }

    /* force the foreground/background if a Bitmap
       is returned. These values are going to go in the cache
       too, so we will have to remember this forcing for the
       case -depth: you ask for -depth and some
       foreground/background, that should match a depth 1
       with 1/0. Look at ComparePixmapDatas above for details */

    if (depth == 1) {
	acc_color->foreground = 1;
	acc_color->background = 0;
    }

    /* XPutImage will only deepen bitmaps -- fake it with pixmaps. */
    old_image_format = image->format;
    if (image->depth == 1) {
      switch(image->format)
	{
	case XYBitmap:
	  /* XPutImage will do the right thing. */
	  break;

	case XYPixmap:
	  /* We're going to muck with the shared image... */
	  if (ret != NOT_CACHED)
	    _XmProcessLock();

	  /* Assume black == fg in the image. */
	  if ((BlackPixelOfScreen(screen) == 0) ||
	      (WhitePixelOfScreen(screen) == 1))
	    {
	      /* Flip the bits so fg == 1 in the image. */
	      int nbytes = image->height * image->bytes_per_line;
	      int byte;

	      /* Image data may be constant, so we must copy it. */
	      old_image_data = image->data;
	      image->data = XtMalloc(nbytes);

	      for (byte = 0; byte < nbytes; byte++)
		image->data[byte] = ~old_image_data[byte];
	    }

	  if (ret != NOT_CACHED)
	    _XmProcessUnlock();

	  /* In this depth-1 case the image formats are equivalent. */
	  image->format = XYBitmap;
	  break;

	case ZPixmap:
	  /* Assume the application really wants BadMatch if (depth != 1). */
	  break;

	default:
	  assert(FALSE);
	}
    }


    /*
     * Create a pixmap to hold the image, allocate a new pixmap
     * cache entry, put the cache entry in the tables.
     */

    pix_entry = XtNew (PixmapData);

    pix_entry->screen = screen;
    pix_entry->acc_color = XtNew(XmAccessColorDataRec);
    memcpy((void*)pix_entry->acc_color, (void*)acc_color,
	   sizeof(XmAccessColorDataRec));
    pix_entry->depth = depth;
    pix_entry->image_name = XtNewString(image_name);
    pix_entry->print_resolution = pix_data.print_resolution ;
    pix_entry->pixels = pixels;
    pix_entry->npixels = npixels;

    /* use scaling_ratio for caching: as 0, in which case the
       print resolution will be used, or 1, no scaling, or a real ratio.
       That's because when we enter this function, we don't have
       pixmap_resolution to cache against, and also because
       print_resolution might change for the same print shell */
    pix_entry->scaling_ratio = scaling_ratio ;

    /* Scale according to the desired w/h or use the scaling ratio */
    if (desired_w || desired_h) {
        if (!desired_w) { /* Scale according to height */
            pix_entry->width  = image->width * ((double)desired_h / image->height);
            pix_entry->height = desired_h;
        } else if (!desired_h) { /* Scale according to width */
            pix_entry->width  = desired_w;
            pix_entry->height = image->height * ((double)desired_w / image->width);
        } else { /* Fixed size */
            pix_entry->width  = desired_w;
            pix_entry->height = desired_h;
        }
    } else {
        pix_entry->width  = image->width  * pix_data.scaling_ratio ;
        pix_entry->height = image->height * pix_data.scaling_ratio ;
    }

    pixmap = XCreatePixmap (display, RootWindowOfScreen(screen),
			    pix_entry->width,
			    pix_entry->height,
			    depth);

    pix_entry->pixmap = pixmap;
    pix_entry->reference_count = 1;

    /* put the new entry in both tables: the table used for
       this routine and the table used in XmeGetPixmapData */
    _XmProcessLock();
    _XmAddHashEntry(pixmap_set, (XmHashKey)pix_entry, (XtPointer)pix_entry);
    _XmAddHashEntry(pixmap_data_set, (XmHashKey)pix_entry,
		    (XtPointer)pix_entry);
    _XmProcessUnlock();

    /*  Set up a gc for the image to pixmap copy, store the image  */
    /*  into the pixmap and return the pixmap.                     */

    gc = GetGCForPutImage(screen,
			  image, pixmap, depth,
			  acc_color->foreground,
			  acc_color->background);

    /* transfer and scale the image */
    _XmPutScaledImage (screen, display, pixmap, depth, gc, image,
		       0, 0, image->width, image->height, 0, 0,
		       pix_entry->width, pix_entry->height);

    /* Destroy non-cached XImage now that we've cached the pixmap. */
    if (ret == NOT_CACHED)
      {
	XDestroyImage(image) ;
	if (old_image_data)
	  XtFree(image->data);
      }
    else if (image->format != old_image_format)
      {
	/* Undo the XYPixmap to XYBitmap conversion done earlier. */
	image->format = old_image_format;
	assert(old_image_format == XYPixmap);

	if (old_image_data)
	  {
	    XtFree(image->data);
	    image->data = old_image_data;
	  }

	/* We're done mangling the image. */
	_XmProcessUnlock();
      }

    return (pixmap);
}


/*******************************************************************
 *
 * _XmGetColoredPixmap
 *   a improved version of XmGetPixmapByDepth that lets the caller
 *   specify more color information usable during the loading
 *   of the pixmap file to override symbolic colors.
 *
 *   It also supports the negative depth convention where if a -depth
 *   is given, xbm file give Bitmap and xpm file give Pixmap, while
 *   with positive depth, all files give Pixmap result.
 *
 *   Now a wrapper, but still used by Text directly.
 *
 *******************************************************************/
Pixmap
_XmGetColoredPixmap(Screen *screen,
		    char *image_name,
		    XmAccessColorData acc_color,
		    int depth,
                   Boolean only_if_exists)
{
    return _XmGetScaledPixmap (screen, NULL,
			       image_name, acc_color, depth,
			       only_if_exists, 1, 0, 0); /* no scaling */
}

/**
 * Get a pixmap, scaled to the given width and height. If either
 * width or height is zero, scale according to the ratio between
 * the given dimension and that of the image.
 */
Pixmap XmGetSizedPixmap(Widget widget, char *image_name, Pixel foreground,
                        Pixel background, int depth, int width, int height)
{
	Pixmap ret;
	XmAccessColorDataRec acc_color_rec;
	XtAppContext app = XtWidgetToApplicationContext(widget);

	_XmAppLock(app);
	_XmProcessLock();
	acc_color_rec.foreground          = foreground;
	acc_color_rec.background          = background;
	acc_color_rec.top_shadow_color    = XmUNSPECIFIED_PIXEL;
	acc_color_rec.bottom_shadow_color = XmUNSPECIFIED_PIXEL;
	acc_color_rec.select_color        = XmUNSPECIFIED_PIXEL;
	acc_color_rec.highlight_color     = XmUNSPECIFIED_PIXEL;
	ret = _XmGetScaledPixmap(XtScreen(widget), widget, image_name,
	                         &acc_color_rec, depth, False, 1,
	                         width, height);
	_XmProcessUnlock();
	_XmAppUnlock(app);
	return ret;
}

Pixmap
XmGetScaledPixmap(
    Widget widget,
    char *image_name,
    Pixel foreground,
    Pixel background,
    int depth,
    double scaling_ratio)
{
    XmAccessColorDataRec acc_color_rec;
    Pixmap ret_val;
    XtAppContext app = XtWidgetToApplicationContext(widget);

    _XmAppLock(app);
    _XmProcessLock();
    acc_color_rec.foreground = foreground ;
    acc_color_rec.background = background ;
    acc_color_rec.top_shadow_color = XmUNSPECIFIED_PIXEL ;
    acc_color_rec.bottom_shadow_color = XmUNSPECIFIED_PIXEL ;
    acc_color_rec.select_color = XmUNSPECIFIED_PIXEL ;
    acc_color_rec.highlight_color = XmUNSPECIFIED_PIXEL ;
    ret_val = _XmGetScaledPixmap(XtScreen(widget), widget, image_name,
				 &acc_color_rec, depth, False,
				 scaling_ratio, 0, 0);
    _XmProcessUnlock();
    _XmAppUnlock(app);

    return  ret_val;

}


/*******************************************************************
 *
 * XmGetPixmapByDepth.
 *  create a pixmap from the image_name.  foreground and background
 *  must be valid values. For depth 1 they should be 1 and 0
 *  respectively.
 *
 *******************************************************************/
Pixmap
XmGetPixmapByDepth(
    Screen *screen,
    char *image_name,
    Pixel foreground,
    Pixel background,
    int depth)
{
    XmAccessColorDataRec acc_color_rec;
    Pixmap ret_val;
    XtAppContext app = XtDisplayToApplicationContext(
				DisplayOfScreen(screen));

    _XmAppLock(app);
    _XmProcessLock();
    acc_color_rec.foreground = foreground ;
    acc_color_rec.background = background ;
    acc_color_rec.top_shadow_color = XmUNSPECIFIED_PIXEL ;
    acc_color_rec.bottom_shadow_color = XmUNSPECIFIED_PIXEL ;
    acc_color_rec.select_color = XmUNSPECIFIED_PIXEL ;
    acc_color_rec.highlight_color = XmUNSPECIFIED_PIXEL ;
    ret_val = _XmGetScaledPixmap(screen, NULL, image_name,
				 &acc_color_rec, depth, False,
				 1, 0, 0);
    _XmProcessUnlock();
    _XmAppUnlock(app);

    return  ret_val;
}


/************************************************************************
 *
 *  This one is deprecated, but keep it in here, it was public.
 *
 ************************************************************************/

Pixmap
XmGetPixmap(
        Screen *screen,
        char *image_name,
        Pixel foreground,
        Pixel background )
{
    Pixmap	ret_val;

    XtAppContext app = XtDisplayToApplicationContext(
				DisplayOfScreen(screen));
    _XmAppLock(app);
    ret_val = XmGetPixmapByDepth(screen, image_name, foreground, background,
				 DefaultDepthOfScreen(screen));
    _XmAppUnlock(app);

    return (ret_val);
}



/************************************************************************
*
*  XmeGetMask
*
************************************************************************/
Pixmap
XmeGetMask(
        Screen *screen,
        char *image_name)
{
    char 		mask_name[256];
    Pixmap		ret_val;
    XtAppContext app = XtDisplayToApplicationContext(
				DisplayOfScreen(screen));

    _XmAppLock(app);

    _XmProcessLock();
    _XmOSGenerateMaskName(image_name, mask_name, sizeof mask_name);
    _XmProcessUnlock();

    ret_val = XmGetPixmapByDepth(screen, mask_name, 1, 0, 1);

    _XmAppUnlock(app);
    return ret_val;
}


/************************************************************************
 *
 *  XmDestroyPixmap
 *	Use the pixmap table (pixmap/screen) to find the pixmap,
 *      then remove it from both pixmap and pixmap_data tables.
 *
 ************************************************************************/
Boolean
XmDestroyPixmap(
        Screen *screen,
        Pixmap pixmap )
{
   PixmapData pix_data, *pix_entry ;
   XtAppContext app;

   /*  Check for invalid conditions  */

   if (screen == NULL || pixmap == 0 || pixmap_set == NULL)
     return (False);

   app = XtDisplayToApplicationContext(
				DisplayOfScreen(screen));

   _XmAppLock(app);
   _XmProcessLock();

   pix_data.screen = screen ;
   pix_data.pixmap = pixmap ;

   if ((pix_entry = (PixmapData*)
	_XmGetHashEntry(pixmap_set, (XmHashKey)&pix_data)) != NULL) {
      pix_entry->reference_count--;
      if (pix_entry->reference_count == 0) {
	  _XmRemoveHashEntry (pixmap_data_set, pix_entry);
	  _XmRemoveHashEntry (pixmap_set, pix_entry);
   	  if (0 != strcmp(pix_entry->image_name, DIRECT_PIXMAP_CACHED))
	    XFreePixmap(DisplayOfScreen(pix_entry->screen), pix_entry->pixmap);
	  XtFree(pix_entry->image_name);
	  XtFree((char*)pix_entry->acc_color);
	  if (pix_entry->pixels) {
	      FreeCacheColors(DisplayOfScreen(pix_entry->screen),
			      DefaultColormapOfScreen(pix_entry->screen),
			      pix_entry->pixels, pix_entry->npixels, NULL);
	      XpmFree(pix_entry->pixels);
	  }
	  XtFree((char*)pix_entry);
      }

      _XmProcessUnlock();
      _XmAppUnlock(app);
      return True;
  }

  _XmProcessUnlock();
  _XmAppUnlock(app);
  return False;
}


/* Compare two gc entry. Only care about the colors for depth 1 */
static Boolean
CompareGCDatas (XmHashKey key_1,
		XmHashKey key_2)
{
  GCData *data_1 = (GCData *) key_1;
  GCData *data_2 = (GCData *) key_2;

  return ((data_1->screen == data_2->screen) &&
	  (data_1->depth == data_2->depth) &&
	  ((data_1->image_depth != 1) ||
	   ((data_1->image_depth == 1) &&
	   (data_1->foreground == data_2->foreground) &&
	   (data_1->background == data_2->background)))) ;
}

/* Hash a GC entry. Only care about the colors for depth 1 */
static XmHashValue
HashGCData (XmHashKey key)
{
  GCData *data = (GCData *) key;
  XmHashValue hv = 0 ;

  if (data->image_depth == 1)
    hv = data->foreground + data->background ;

  return hv + (long)data->screen + data->depth + data->image_depth;
}

/***
 * GetGCForPutImage
 * Maintain a cache of GC to use for transfering the XImage to
 * a Pixmap depending on the screen, depth, colors.
 ***/
static GC
GetGCForPutImage(Screen *screen,
		 XImage * image,
		 Pixmap pixmap,
		 int depth,
		 Pixel foreground,
		 Pixel background)
{
    XGCValues gcValues;
    GCData gc_data, *gc_entry ;

    /* There is a problem here: we cannot simply use the xmScreen object
     * to store the GCs, for the pixmap conversion can be called
     * at a time the xmScreen is not yet available: a conversion
     * for the first VendorShell on this screen, or a conversion for
     * the xmScreen or xmDisplay.
     * We can either arrange for delaying the conversion for those cases,
     * or not use the xmScreen here. Since we need to cache per depth
     * in addition to per screen, and since the Xt routine cannot be used
     * since there is no widget available, let's forget about using the
     * xmScreen object, do our own specialized caching.
     */

    /* There shouldn't be a lot of different GCs, one for
     * depth 1 to depth 1, foreground 1, back 0, one for
     * depth N to N, fore/back don't matter and some for depth 1 to
     * depth N, where colors matter, for backgroundPixmap resources
     * and regular use
     * of XmGetPixmapByDepth (with no -depth) issued from XBM file.
     */

    _XmProcessLock();
    if (gc_set == NULL)
	gc_set = _XmAllocHashTable (20, CompareGCDatas, HashGCData);
    _XmProcessUnlock();

    /* set up an entry for search. We only care about the colors
       for image_depth 1 to pixmap depth (N or 1), since otherwise, N to N,
       the transfer is a plain copy */
    gc_data.screen = screen ;
    gc_data.depth = depth ;
    gc_data.image_depth = image->depth ;
    gc_data.foreground = foreground ;
    gc_data.background = background ;

    /* look if we have a match and return it */
    _XmProcessLock();
    if ((gc_entry = (GCData*)
	 _XmGetHashEntry(gc_set, (XmHashKey)&gc_data)) != NULL) {
	_XmProcessUnlock();
	return gc_entry->gc ;
    }
    _XmProcessUnlock();

    /* create a new GC, cache it and return it */
    gc_entry = XtNew(GCData);
    gc_entry->screen = screen ;
    gc_entry->depth = depth ;
    gc_entry->image_depth = image->depth ;
    gc_entry->foreground = foreground ;
    gc_entry->background = background ;
    _XmProcessLock();
    _XmAddHashEntry(gc_set, (XmHashKey)gc_entry, (XtPointer)gc_entry);
    _XmProcessUnlock();

    gcValues.foreground = foreground;
    gcValues.background = background;
    gc_entry->gc = XCreateGC (DisplayOfScreen(screen), pixmap,
			     GCForeground | GCBackground, &gcValues);

    return gc_entry->gc ;
}

/**
 * When a Screen is closed we need to clean up the respective cache
 */
static Boolean CleanGCMapProc(XmHashKey key, XtPointer value, XtPointer data)
{
	GCData *entry = (GCData *) value;

	(void)key;
	if (entry->screen != (Screen *)data)
		return False;

	_XmProcessLock();
	_XmRemoveHashEntry (gc_set, entry);
	_XmProcessUnlock();
	XFreeGC(DisplayOfScreen(entry->screen), entry->gc);
	XtFree((char *)entry);
	return False;
}

static Boolean CleanPixmapMapProc(XmHashKey key, XtPointer value, XtPointer data)
{
	PixmapData *entry = (PixmapData *)value;

	(void)key;
	if (entry->screen == (Screen *)data)
		XmDestroyPixmap(entry->screen, entry->pixmap);

	return False;
}

/**
 * This is called from Screen.destroy with a NULL shell, to remove the GCs
 * and Pixmap used for a given screen.
 */
void _XmCleanPixmapCache(Screen *screen)
{
	_XmMapHashTable(gc_set, CleanGCMapProc, screen);
	_XmMapHashTable(pixmap_set, CleanPixmapMapProc, screen);
}

/************************************************************************
*******************************************************************------

  Design notes:

  There is really two different caches here, an XImage cache and
  a Pixmap cache (+ a GC cache).

  The XImage cache is name based (hash key is image_name). No colors are
  supported, and therefore, it is only useful with depth 1 XImage.

  Program can directly install non-depth 1 XImage in this cache using
  the public API (XmInstallImage), but that's not advised, for the
  non-depth 1 XImage resulting from XPM file have color variations
  for the same image_name. If you ask for image_name Foo, foreground
  red and background blue, and a Foo has been installed in the image
  cache using different foreground & background, you don't want
  to get back this Foo, but a new instance of Foo, one that uses
  the red and blue colors.

  Also, the only access to the XImage reader (GetImage) is via the pixmap
  API, so if a XPM with no colors sensitivity (no pertinent Motif
  symbolic colors) is read once, it will be found by the
  pixmap cache first, no need to cache it in the image cache.

  The Pixmap cache is name/depth/colors or pixmap based, depending
  which API is looking up.
  The old XmGetPixmapByDepth() is equivalent to asking for
  valid foreground/background and unspecified others, which
  means these additional colors (shadows, select) will not be
  used during Xpm reading for overriding symbolic colors.

  Motifnext adds printing support and clean cache support.

***/

/**
 * Simple alpha interpolation
 */
#define BLEND(bg, c, a) (int)(((((bg) * (1.0 - (a))) + ((c) * (a)))) * 255.0)

/**
 * Count trailing zeroes
 */
static unsigned int ctz(unsigned long n) {
#if defined(__has_builtin) && __has_builtin(__builtin_ctzl)
	return __builtin_ctzl(n);
#else
	static const unsigned int mod37[] = {
		-1, 0, 1, 26, 2, 23, 27, 0, 3, 16, 24, 30, 28, 11, 0, 13, 4,
		7, 17, 0, 25, 22, 31, 15, 29, 10, 12, 6, 0, 21, 14, 9, 5,
		20, 8, 19, 18
	};

    return mod37[(-n & n) % 37];
#endif
}

/**
 * Count the number of bits set in a long
 */
static unsigned int ones(unsigned long n) {
#if defined(__has_builtin) && __has_builtin(__builtin_popcountl)
	return __builtin_popcountl(n);
#elif sizeof(unsigned long) == 8
	n = n - ((n >> 1) & 0x5555555555555555);
    n = (n & 0x3333333333333333) + ((n >> 2) & 0x3333333333333333);
    n = (n + (n >> 4)) & 0x0F0F0F0F0F0F0F0F;
    return (n * 0x0101010101010101) >> 56;
#else
	n = n - ((n >> 1) & 0x55555555);
	n = (n & 0x33333333) + ((n >> 2) & 0x33333333);
	n = (n + (n >> 4)) & 0x0F0F0F0F;
	return (n * 0x01010101) >> 24;
#endif
}

/**
 * Simple scaling / rendering to a Drawable (most likely a Pixmap.)
 */
static void render_image(Screen *screen, Display *display, Drawable d,
                         int depth, GC gc, XImage *src, int sx, int sy,
                         int sw, int sh, int dx, int dy, int dw, int dh)
{
	char *data;
	Visual *vis;
	XImage *dest_image;
	Pixel pixel = 0;
	XColor bg, xc;
	XGCValues gcv;
	int psz, x, y, ox, oy, r, g, b, a;

	psz = depth > 16 ? 32 : depth < 16 ? 8 : 16;
	if (!(data = Xmalloc(dw * dh * (psz >> 3)))) {
		XmeWarning(NULL, "render_image: Out of memory");
		return;
	}

	vis = DefaultVisualOfScreen(screen);
	gcv.foreground = ULONG_MAX;
	gcv.background = ULONG_MAX;
	XGetGCValues(display, gc, GCBackground | GCForeground, &gcv);
	if (gcv.background == ULONG_MAX || gcv.foreground == ULONG_MAX) {
		gcv.background = XBlackPixelOfScreen(screen);
		gcv.foreground = XWhitePixelOfScreen(screen);
		XChangeGC(display, gc, GCBackground | GCForeground, &gcv);
	}

	bg.pixel = gcv.background;
	XQueryColor(display, screen->cmap, &bg);
	dest_image = XCreateImage(display, vis, depth, ZPixmap, 0, data,
	                          dw, dh, psz, 0);

	for (y = 0; y < dh; y++) {
		for (x = 0; x < dw; x++) {
			r = g = b = 0;
			ox = sx + (int)(x / (double)dw * sw);
			oy = sy + (int)(y / (double)dh * sh);

			/* Anything out of bounds is background */
			if (ox < sx || oy < sy || ox > sx + sw || oy > sy + sh) {
				XPutPixel(dest_image, dx + x, dy + y, gcv.background);
				continue;
			}

			/* Extract the color components */
			switch (src->depth) {
			case 1: /* Good ol' XBM */
				pixel = ((src->data[(oy * src->bytes_per_line) + (ox >> 3)] >> (ox & 7)) & 1)
				        ? gcv.foreground : gcv.background;
				break;
			case 8:
				xc.pixel = XGetPixel(src, ox, oy);
				XQueryColor(display, screen->cmap, &xc);
				r = xc.red   / 257;
				g = xc.green / 257;
				b = xc.blue  / 257;
				break;
			default:
				pixel = XGetPixel(src, ox, oy);
				r = (pixel & src->red_mask)   >> ctz(src->red_mask);
				g = (pixel & src->green_mask) >> ctz(src->green_mask);
				b = (pixel & src->blue_mask)  >> ctz(src->blue_mask);

				if (src->depth == 32) {
					if (!(a = (pixel >> 24) & 0xff)) {
						pixel = gcv.background;
						break;
					} else if (a != 0xff) {
						r = BLEND(bg.red   / 65535.0, r / 255.0, a / 255.0);
						g = BLEND(bg.green / 65535.0, g / 255.0, a / 255.0);
						b = BLEND(bg.blue  / 65535.0, b / 255.0, a / 255.0);
					}
				}

				if (r == 255 && g == 255 && b == 255)
					pixel = XWhitePixelOfScreen(screen);
				else if (!r && !g && !b)
					pixel = XBlackPixelOfScreen(screen);
				else {
					if (src->depth > 16 && depth == 16 && ones(vis->red_mask) == 5) { /* 5:5:5 */
						r >>= 3;
						g >>= 3;
						b >>= 3;
					}

					/* Assume TrueColor or DirectColor */
					pixel = (((r << ctz(vis->red_mask))   & vis->red_mask)   |
					         ((g << ctz(vis->green_mask)) & vis->green_mask) |
					         ((b << ctz(vis->blue_mask))  & vis->blue_mask));
				}
			}

			if (src->depth == 1 || vis->class == TrueColor || vis->class == DirectColor) {
				XPutPixel(dest_image, dx + x, dy + y, pixel);
				continue;
			}

			if (depth == 1 || vis->map_entries <= 2) {
				XPutPixel(dest_image, dx + x, dy + y,
				          (r | g | b) ? gcv.foreground : gcv.background);
				continue;
			}

			/* Colormap time */
			xc.pixel = 0;
			xc.red   = r * 257;
			xc.green = g * 257;
			xc.blue  = b * 257;
			pixel    = gcv.foreground;

			/* Slowly interrogate the colormap for Pixel values */
			if (XAllocColor(display, screen->cmap, &xc))
				pixel = xc.pixel;
			XPutPixel(dest_image, dx + x, dy + y, pixel);
		}
	}

	XPutImage(display, d, gc, dest_image, 0, 0, dx, dy, dw, dh);
	XDestroyImage(dest_image);
}

void _XmPutScaledImage(Screen *screen, Display *display, Drawable d,
                       int depth, GC gc, XImage *src, int sx, int sy,
                       int sw, int sh, int dx, int dy, int dw, int dh)
{
	int free_src = 0;
	Visual *vis = DefaultVisualOfScreen(screen);

	/* svg: Rasterize to the given size */
	if (XImageIsSVG(src)) {
		++free_src;
		src = src->f.sub_image(src, sx, sy, dw, dh);
		sx  = 0;
		sy  = 0;
		sw  = dw;
		sh  = dh;
	}

	/* Same depth, size, and format */
	if (src->depth == depth && dw == sw && dh == sh &&
	    src->red_mask   == vis->red_mask   &&
	    src->green_mask == vis->green_mask &&
	    src->blue_mask  == vis->blue_mask) {
		XPutImage(display, d, gc, src, sx, sy, dx, dy, dw, dh);
		if (free_src) XDestroyImage(src);
		return;
	}

	render_image(screen, display, d, depth, gc, src, sx, sy, sw, sh,
	             dx, dy, dw, dh);
	if (free_src) XDestroyImage(src);
}

/*** COLOR CACHE NOW ***/

static Boolean
GetCacheColorByName( Display *display, Colormap colormap,
		     char *colorname, XColor *xcolor)
{
    static Boolean firstTime = True;
    XrmQuark colorname_q;
    int i;
    CachedColor *color;

    if (!colorname)
	return False;

    if ( firstTime )
    {
       colorCacheList.numEntries = colorCacheList.maxEntries = 0;
       colorCacheList.cache = NULL;
       firstTime = False;
       return False;
    }

    colorname_q = XrmStringToQuark(colorname);

    for (i = 0, color = colorCacheList.cache; i < colorCacheList.numEntries;
	 i++, color++)
    {
	if (color->colorname == colorname_q &&
            color->colormap == colormap &&
            color->display == display)
        {
	   xcolor->pixel = color->pixel;
	   color->num_cached++;
           return True;
        }
    }
    return False;
}

static Boolean
GetCacheColorByRGB( Display *display, Colormap colormap, XColor *xcolor)
{
    int i;
    CachedColor *color;

    for (i = 0, color = colorCacheList.cache; i < colorCacheList.numEntries;
	 i++, color++)
    {
	if (color->colormap == colormap && color->display == display &&
	    color->red == xcolor->red &&
	    color->green == xcolor->green &&
	    color->blue == xcolor->blue)
        {
	   xcolor->pixel = color->pixel;
	   color->num_cached++;
           return True;
        }
    }
    return False;
}

static Boolean
FreeCacheColor( Display *display, Colormap colormap, Pixel pixel)
{
    int i;
    CachedColor *color;

    for (i = 0, color = colorCacheList.cache; i < colorCacheList.numEntries;
	 i++, color++)
    {
	if (color->colormap == colormap && color->display == display &&
	    color->pixel == pixel)
        {
	    color->num_cached--;
	    if (color->num_cached == 0) {
		int j;
		for (j = i + 1; j < colorCacheList.numEntries; i++, j++)
		    colorCacheList.cache[i] = colorCacheList.cache[j];
		colorCacheList.numEntries--;
		XFreeColors(display, colormap, &pixel, 1, 0);
		return True;
	    }
	}
    }
    return False;
}

static void
CacheColorPixel( Display *display, Colormap colormap,
                 char *colorname, XColor *xcolor)
{
    int numEntries = colorCacheList.numEntries;

    if (numEntries == colorCacheList.maxEntries)
    {
        colorCacheList.maxEntries += 25;
	colorCacheList.cache = (CachedColor *)
				  XtRealloc((char *)colorCacheList.cache,
					    colorCacheList.maxEntries *
				            sizeof(CachedColor));
    }

    colorCacheList.cache[numEntries].display = display;
    colorCacheList.cache[numEntries].colormap = colormap;
    colorCacheList.cache[numEntries].colorname =
	colorname ? XrmStringToQuark(colorname) : NULLQUARK;

    colorCacheList.cache[numEntries].red = xcolor->red;
    colorCacheList.cache[numEntries].green = xcolor->green;
    colorCacheList.cache[numEntries].blue = xcolor->blue;
    colorCacheList.cache[numEntries].pixel = xcolor->pixel;
    colorCacheList.cache[numEntries].num_cached = 1;

    colorCacheList.numEntries++;
}


static int
GetCacheColor(Display *display, Colormap colormap,
	      char *colorname, XColor *xcolor, void *closure)
{
    int status;
    if (colorname) {
	/* look for the colorname in the cache */
	if (!GetCacheColorByName(display, colormap, colorname, xcolor)) {
	    /* when not found parse and alloc color */
	    if (!XParseColor(display, colormap, colorname, xcolor))
		return -1;
	    if (!GetCacheColorByRGB(display, colormap, xcolor)) {
		status = XAllocColor(display, colormap, xcolor);
		if (status)	/* if allocation succeeded, store color */
		    CacheColorPixel(display, colormap, colorname, xcolor);
	    } else
		status = 1;
	} else
	    status = 1;
    } else {
	if (!GetCacheColorByRGB(display, colormap, xcolor)) {
	    status = XAllocColor(display, colormap, xcolor);
	    if (status)		/* if allocation succeeded, store color */
		CacheColorPixel(display, colormap, colorname, xcolor);
	} else
	    status = 1;
    }

    return status != 0 ? 1 : 0;
}

static int
FreeCacheColors(Display *display, Colormap colormap,
		Pixel *pixels, int n, void *closure)
{
    int i, status;
    for (i = 0; i < n; i++, pixels++)
	status = FreeCacheColor(display, colormap, *pixels);
    return 0;
}

