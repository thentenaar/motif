/* $TOG: Color.c /main/13 1998/09/23 19:36:49 samborn $ */
/**
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <ctype.h>
#include <stdio.h>
#include <X11/xpm.h>
#include <X11/IntrinsicP.h>
#include <X11/ShellP.h>
#include <Xm/ManagerP.h>
#include <Xm/PrimitiveP.h>
#include <Xm/XmP.h>
#include <Xm/XmosP.h>		/* for bzero */
#include "ColorI.h"
#include "ImageCachI.h"
#include "MessagesI.h"
#include "ScreenI.h"
#include "XmI.h"

/* Warning and Error messages */

#define MESSAGE0	_XmMMsgVisual_0000
#define MESSAGE1	_XmMMsgVisual_0001
#define MESSAGE2	_XmMMsgVisual_0002


/********    Static Function Declarations    ********/

static void GetDefaultThresholdsForScreen(
                        Screen *screen) ;
static XmColorData * GetDefaultColors(
                        Screen *screen,
                        Colormap color_map) ;
static Pixel GetBlackPixel(
                        Screen *screen,
                        Colormap colormap,
                        XColor blackcolor) ;
static Pixel GetWhitePixel(
                        Screen *screen,
                        Colormap colormap,
                        XColor whitecolor) ;
static void SetMonochromeColors(
                        XmColorData *colors) ;
static int Brightness(
                        XColor *color) ;
static void CalculateColorsForLightBackground(
                        XColor *bg_color,
                        XColor *fg_color,
                        XColor *sel_color,
                        XColor *ts_color,
                        XColor *bs_color) ;
static void CalculateColorsForDarkBackground(
                        XColor *bg_color,
                        XColor *fg_color,
                        XColor *sel_color,
                        XColor *ts_color,
                        XColor *bs_color) ;
static void CalculateColorsForMediumBackground(
                        XColor *bg_color,
                        XColor *fg_color,
                        XColor *sel_color,
                        XColor *ts_color,
                        XColor *bs_color) ;
static void CalculateColorsRGB(
                        XColor *bg_color,
                        XColor *fg_color,
                        XColor *sel_color,
                        XColor *ts_color,
                        XColor *bs_color) ;
static Pixel AccessColorData(
                        XmColorData *cd,
                        unsigned char which) ;
static XmColorData * GetColors(
                        Screen *screen,
                        Colormap color_map,
                        Pixel background) ;
static unsigned int FromColorToBlackAndWhite(char *col) ;

/********    End Static Function Declarations    ********/



/*
 * GLOBAL VARIABLES
 *
 * These variables define the color cache.
 */
static int Set_Count=0, Set_Size=0;
static XmColorData *Color_Set=NULL;


/* Thresholds for brightness
   above LITE threshold, LITE color model is used
   below DARK threshold, DARK color model is be used
   use STD color model in between */

static int XmCOLOR_LITE_THRESHOLD;
static int XmCOLOR_DARK_THRESHOLD;
static int XmFOREGROUND_THRESHOLD;
static Boolean XmTHRESHOLDS_INITD = FALSE;


/* DEPRECATED : use XmScreen resource now */
static XmColorProc ColorRGBCalcProc = CalculateColorsRGB;


static void
GetDefaultThresholdsForScreen( Screen *screen )
{
  XmScreen xmScreen;
  int	   default_light_threshold_spec;
  int	   default_dark_threshold_spec;
  int	   default_foreground_threshold_spec;

  _XmProcessLock();
  XmTHRESHOLDS_INITD = True;
  _XmProcessUnlock();

  xmScreen = XmScreenOfScreen(screen);

  /* Get resources from the XmScreen */
  default_light_threshold_spec = xmScreen->screen.lightThreshold;
  default_dark_threshold_spec = xmScreen->screen.darkThreshold;
  default_foreground_threshold_spec = xmScreen->screen.foregroundThreshold;

  if ((default_light_threshold_spec <= 0) ||
      (default_light_threshold_spec > 100))
    default_light_threshold_spec = XmDEFAULT_LIGHT_THRESHOLD;

  if ((default_dark_threshold_spec <= 0) ||
      (default_dark_threshold_spec > 100))
    default_dark_threshold_spec = XmDEFAULT_DARK_THRESHOLD;

  if ((default_foreground_threshold_spec <= 0) ||
      (default_foreground_threshold_spec > 100))
    default_foreground_threshold_spec = XmDEFAULT_FOREGROUND_THRESHOLD;

  _XmProcessLock();
  XmCOLOR_LITE_THRESHOLD = default_light_threshold_spec * XmCOLOR_PERCENTILE;
  XmCOLOR_DARK_THRESHOLD = default_dark_threshold_spec * XmCOLOR_PERCENTILE;
  XmFOREGROUND_THRESHOLD = default_foreground_threshold_spec * XmCOLOR_PERCENTILE;
  _XmProcessUnlock();
}

static XColor *GetDefaultBackgroundColor(Screen *screen, Colormap color_map)
{
  XrmName names[2];
  XrmClass classes[2];
  XrmRepresentation rep;
  XrmValue db_value;
  static XColor color;

  names[0] = XrmPermStringToQuark(XmNbackground);
  names[1] = NULLQUARK;

  classes[0] = XrmPermStringToQuark(XmCBackground);
  classes[1] = NULLQUARK;

  if (XrmQGetResource(XtScreenDatabase(screen), names, classes,
		      &rep, &db_value))
     {
       if (rep == XrmPermStringToQuark(XmRString)) {
	 if (!XParseColor(DisplayOfScreen(screen), color_map, db_value.addr,
			  &color)) return NULL;
       }
       else if (rep == XrmPermStringToQuark(XmRPixel)) {
	 color.pixel = *(Pixel *) db_value.addr;

	 XQueryColor(DisplayOfScreen(screen), color_map, &color);
       }
     }
  else {
    if (!XParseColor(DisplayOfScreen(screen), color_map, XmDEFAULT_BACKGROUND,
		     &color)) return NULL;
  }

  return &color;
}

static XmColorData *
GetDefaultColors(
        Screen *screen,
        Colormap color_map )
{
	static XmColorData * default_set = NULL;
	static int default_set_count = 0;
	static int default_set_size = 0;
	register int i;
	XColor *color_def;
	static Pixel background;
        XrmValue fromVal;
        XrmValue toVal;
        XrmValue args[2];
        Cardinal num_args;
	String default_string = XtDefaultBackground;
	XmColorData *result;

	_XmProcessLock();

	/*  Look through  a set of screen / background pairs to see  */
	/*  if the default is already in the table.                  */

	for (i = 0; i < default_set_count; i++)
	{
	if ((default_set[i].screen == screen) &&
		(default_set[i].color_map == color_map))
	    {
	        result = default_set + i;
		_XmProcessUnlock();
		return result;
	    }
	}

	/*  See if more space is needed in the array  */

	if (default_set == NULL)
	{
		default_set_size = 10;
		default_set = (XmColorData *) XtRealloc((char *) default_set,
			(sizeof(XmColorData) * default_set_size));

	}
	else if (default_set_count == default_set_size)
	{
		default_set_size += 10;
		default_set = (XmColorData *) XtRealloc((char *) default_set,
			sizeof(XmColorData) * default_set_size);
	}

	/* Find the background based on the depth of the screen */
	if (DefaultDepthOfScreen(screen) == 1)
        {
	  /*
	   * Fix for 4603 - Convert the string XtDefaultBackground into a Pixel
	   *                value using the XToolkit converter.  This converter
	   *                will set this value to WhitePixelOfScreen if reverse
	   *                video is not on, and to BlackPixelOfScreen if reverse
	   *                video is on.
	   */
	  args[0].addr = (XPointer) &screen;
	  args[0].size = sizeof(Screen*);
	  args[1].addr = (XPointer) &color_map;
	  args[1].size = sizeof(Colormap);
	  num_args = 2;

	  fromVal.addr = default_string;
	  fromVal.size = strlen(default_string);

	  toVal.addr = (XPointer) &background;
	  toVal.size = sizeof(Pixel);

	  if(!XtCallConverter(DisplayOfScreen(screen),XtCvtStringToPixel,
			      args, num_args, &fromVal, &toVal, NULL))
	    background = WhitePixelOfScreen(screen);
        }

	else
	{
		/*  Parse out a color for the default background  */

		if ((color_def = GetDefaultBackgroundColor(screen,
							   color_map)) !=
		    NULL)
		{
		    XmAllocColorProc aproc =
			_XmGetColorAllocationProc(screen);
		    if (aproc == NULL)
			aproc = DEFAULT_ALLOCCOLOR_PROC;

			if ((*aproc)(DisplayOfScreen(screen), color_map,
				color_def))
			{
				background = color_def->pixel;
			}
			else
			{
				XtWarning(MESSAGE1);
				background = WhitePixelOfScreen(screen);
			}
		}
		else
		{
			XtWarning(MESSAGE2);
			background = WhitePixelOfScreen(screen);
		}
	}

	/*
	 * Get the color data generated and save it in the next open
	 * slot in the default set array.  default_set points to a subset
	 * of the data pointed to by color_set (defined in GetColors).
	 */

	default_set[default_set_count] =
		*GetColors(screen, color_map, background);
	default_set_count++;

	result = default_set + (default_set_count - 1);
	_XmProcessUnlock();
	return result;
}



Boolean
_XmSearchColorCache(
        unsigned int which,
        XmColorData *values,
        XmColorData **ret )
{
    register int i;

    /*
     * Look through  a set of screen, color_map, background triplets
     * to see if these colors have already been generated.
     */

    _XmProcessLock();
    for (i = 0; i < Set_Count; i++) {
	if ( (!(which & XmLOOK_AT_SCREEN) ||
	      ((Color_Set + i)->screen == values->screen))
	    &&
	    (!(which & XmLOOK_AT_CMAP) ||
	     ((Color_Set + i)->color_map == values->color_map))
	    &&
	    (!(which & XmLOOK_AT_BACKGROUND) ||
	     (((Color_Set + i)->allocated & XmBACKGROUND) &&
	      ((Color_Set + i)->background.pixel ==
	       values->background.pixel)))
	    &&
	    (!(which & XmLOOK_AT_FOREGROUND) ||
	     (((Color_Set + i)->allocated & XmFOREGROUND) &&
	      ((Color_Set + i)->foreground.pixel ==
	       values->foreground.pixel)))
	    &&
	    (!(which & XmLOOK_AT_TOP_SHADOW) ||
	     (((Color_Set + i)->allocated & XmTOP_SHADOW) &&
	      ((Color_Set + i)->top_shadow.pixel ==
	       values->top_shadow.pixel)))
	    &&
	    (!(which & XmLOOK_AT_BOTTOM_SHADOW) ||
	     (((Color_Set + i)->allocated & XmBOTTOM_SHADOW) &&
	      ((Color_Set+ i)->bottom_shadow.pixel ==
	       values->bottom_shadow.pixel)))
	    &&
	    (!(which & XmLOOK_AT_SELECT) ||
	     (((Color_Set + i)->allocated & XmSELECT) &&
	      ((Color_Set + i)->select.pixel ==
	       values->select.pixel))))
	    {
		*ret = (Color_Set + i);
		_XmProcessUnlock();
		return (TRUE);
	    }
    }

    *ret = NULL;
    _XmProcessUnlock();
    return (FALSE);
}

XmColorData *
_XmAddToColorCache(
        XmColorData *new_rec )
{
       XmColorData *result;

	/*  See if more space is needed */
	_XmProcessLock();
	if (Set_Count == Set_Size)
	{
		Set_Size += 10;
		Color_Set = (XmColorData *)XtRealloc((char *) Color_Set,
			sizeof(XmColorData) * Set_Size);
	}

	*(Color_Set + Set_Count) = *new_rec;
	Set_Count++;

	result = Color_Set + (Set_Count - 1);
	_XmProcessUnlock();

	return result;
}


static Pixel
GetBlackPixel(
	      Screen *screen,
	      Colormap colormap,
	      XColor blackcolor )
{
  Pixel p;
  XmAllocColorProc aproc = _XmGetColorAllocationProc(screen);

  if (aproc == NULL)
      aproc = DEFAULT_ALLOCCOLOR_PROC;

  blackcolor.red = 0;
  blackcolor.green = 0;
  blackcolor.blue = 0;

  if (colormap == DefaultColormapOfScreen(screen))
    p = blackcolor.pixel = BlackPixelOfScreen(screen);
  else if ((*aproc)(screen->display, colormap, &blackcolor))
    p = blackcolor.pixel;
  else
    p = blackcolor.pixel = BlackPixelOfScreen(screen); /* fallback pixel */

  return (p);
}


static Pixel
GetWhitePixel(
	      Screen *screen,
	      Colormap colormap,
	      XColor whitecolor )
{
  Pixel p;
  XmAllocColorProc aproc = _XmGetColorAllocationProc(screen);

  if (aproc == NULL)
      aproc = DEFAULT_ALLOCCOLOR_PROC;

  whitecolor.red = XmMAX_SHORT;
  whitecolor.green = XmMAX_SHORT;
  whitecolor.blue = XmMAX_SHORT;

  if (colormap == DefaultColormapOfScreen(screen))
    p = whitecolor.pixel = WhitePixelOfScreen(screen);
  else if ((*aproc)(screen->display, colormap, &whitecolor))
    p = whitecolor.pixel;
  else
    p = whitecolor.pixel = WhitePixelOfScreen(screen); /* fallback pixel */
  return (p);
}

static Pixel
AccessColorData(
        XmColorData *cd,
        unsigned char which )
{
    Pixel p;
    XmAllocColorProc aproc = _XmGetColorAllocationProc(cd->screen);

    if (aproc == NULL)
	aproc = DEFAULT_ALLOCCOLOR_PROC;

    switch(which) {
    case XmBACKGROUND:
	if (!(cd->allocated & which) &&
	    ((*aproc)(cd->screen->display,
		      cd->color_map, &(cd->background)) == 0)) {
	    if (Brightness(&(cd->background))
		< XmFOREGROUND_THRESHOLD )
		cd->background.pixel = GetBlackPixel(cd->screen,
						     cd->color_map,
						     cd->background);
	    else
		cd->background.pixel = GetWhitePixel(cd->screen,
						     cd->color_map,
						     cd->background);
	    XQueryColor(cd->screen->display, cd->color_map,
			&(cd->background));
	}
	p = cd->background.pixel;
	cd->allocated |= which;
	break;
    case XmFOREGROUND:
	if (!(cd->allocated & which) &&
	    ((*aproc)(cd->screen->display,
		      cd->color_map, &(cd->foreground)) == 0 ))
	    {
		if (Brightness(&(cd->background))
		    < XmFOREGROUND_THRESHOLD )
		    cd->foreground.pixel = GetWhitePixel(cd->screen,
							 cd->color_map,
							 cd->foreground);
		else
		    cd->foreground.pixel = GetBlackPixel(cd->screen,
							 cd->color_map,
							 cd->foreground);
		XQueryColor(cd->screen->display, cd->color_map,
			    &(cd->foreground));
	    }
	p =  cd->foreground.pixel;
	cd->allocated |= which;
	break;
    case XmTOP_SHADOW:
	if (!(cd->allocated & which) &&
	    ((*aproc)(cd->screen->display,
		      cd->color_map, &(cd->top_shadow)) == 0))
	    {
		if (Brightness(&(cd->background))
		    > XmCOLOR_LITE_THRESHOLD)
		    cd->top_shadow.pixel =
			GetBlackPixel(cd->screen, cd->color_map,
				      cd->top_shadow);
		else
		    cd->top_shadow.pixel =
			GetWhitePixel(cd->screen, cd->color_map,
				      cd->top_shadow);
		XQueryColor(cd->screen->display, cd->color_map,
			    &(cd->top_shadow));

	    }
	p = cd->top_shadow.pixel;
	cd->allocated |= which;
	break;
    case XmBOTTOM_SHADOW:
	if (!(cd->allocated & which) &&
	    ((*aproc)(cd->screen->display,
		      cd->color_map, &(cd->bottom_shadow)) == 0))
	    {
		if (Brightness(&(cd->background))
		    < XmCOLOR_DARK_THRESHOLD)
		    cd->bottom_shadow.pixel =
			GetWhitePixel(cd->screen, cd->color_map,
				      cd->bottom_shadow);
		else
		    cd->bottom_shadow.pixel =
			GetBlackPixel(cd->screen, cd->color_map,
				      cd->bottom_shadow);
		XQueryColor(cd->screen->display, cd->color_map,
			    &(cd->bottom_shadow));
	    }
	p = cd->bottom_shadow.pixel;
	cd->allocated |= which;
	break;
    case XmSELECT:
	if (!(cd->allocated & which) &&
	    ((*aproc)(cd->screen->display,
		      cd->color_map, &(cd->select)) == 0))
	    {
		if (Brightness(&(cd->background))
		    < XmFOREGROUND_THRESHOLD)
		    cd->select.pixel = GetWhitePixel(cd->screen,
						     cd->color_map,
						     cd->select);
		else
		    cd->select.pixel = GetBlackPixel(cd->screen,
						     cd->color_map,
						     cd->select);
		XQueryColor(cd->screen->display, cd->color_map,
			    &(cd->select));
	    }
	p = cd->select.pixel;
	cd->allocated |= which;
	break;
    default:
	XtWarning(MESSAGE0);
	p = GetBlackPixel(cd->screen, cd->color_map, cd->background);
	break;
    }

    return(p);
}

static void
SetMonochromeColors(
        XmColorData *colors )
{
	Screen *screen = colors->screen;
	Pixel background = colors->background.pixel;

	if (background == BlackPixelOfScreen(screen))
	{
		colors->foreground.pixel = WhitePixelOfScreen (screen);
		colors->foreground.red = colors->foreground.green =
			colors->foreground.blue = XmMAX_SHORT;

		colors->bottom_shadow.pixel = WhitePixelOfScreen(screen);
		colors->bottom_shadow.red = colors->bottom_shadow.green =
			colors->bottom_shadow.blue = XmMAX_SHORT;

		colors->select.pixel = WhitePixelOfScreen(screen);
		colors->select.red = colors->select.green =
			colors->select.blue = XmMAX_SHORT;

		colors->top_shadow.pixel = BlackPixelOfScreen(screen);
		colors->top_shadow.red = colors->top_shadow.green =
			colors->top_shadow.blue = 0;
	}
	else if (background == WhitePixelOfScreen(screen))
	{
		colors->foreground.pixel = BlackPixelOfScreen(screen);
		colors->foreground.red = colors->foreground.green =
			colors->foreground.blue = 0;

		colors->top_shadow.pixel = WhitePixelOfScreen(screen);
		colors->top_shadow.red = colors->top_shadow.green =
			colors->top_shadow.blue = XmMAX_SHORT;

		colors->bottom_shadow.pixel = BlackPixelOfScreen(screen);
		colors->bottom_shadow.red = colors->bottom_shadow.green =
			colors->bottom_shadow.blue = 0;

		colors->select.pixel = BlackPixelOfScreen(screen);
		colors->select.red = colors->select.green =
			colors->select.blue = 0;
	}

	colors->allocated |= (XmFOREGROUND | XmTOP_SHADOW
		| XmBOTTOM_SHADOW | XmSELECT);
}

static int
Brightness(
        XColor *color )
{
	int brightness;
	int intensity;
	int light;
	int luminosity, maxprimary, minprimary;
	int red = color->red;
	int green = color->green;
	int blue = color->blue;

	intensity = (red + green + blue) / 3;

	/*
	 * The casting nonsense below is to try to control the point at
	 * the truncation occurs.
	 */

	luminosity = (int) ((XmRED_LUMINOSITY * (float) red)
		+ (XmGREEN_LUMINOSITY * (float) green)
		+ (XmBLUE_LUMINOSITY * (float) blue));

	maxprimary = ( (red > green) ?
					( (red > blue) ? red : blue ) :
					( (green > blue) ? green : blue ) );

	minprimary = ( (red < green) ?
					( (red < blue) ? red : blue ) :
					( (green < blue) ? green : blue ) );

	light = (minprimary + maxprimary) / 2;

	brightness = ( (intensity * XmINTENSITY_FACTOR) +
				   (light * XmLIGHT_FACTOR) +
				   (luminosity * XmLUMINOSITY_FACTOR) ) / 100;
	return(brightness);
}

static void
CalculateColorsForLightBackground(
        XColor *bg_color,
        XColor *fg_color,
        XColor *sel_color,
        XColor *ts_color,
        XColor *bs_color )
{
	int brightness = Brightness(bg_color);
	int color_value;

	if (fg_color)
	{
/*
 * Fix for 4602 - Compare the brightness with the foreground threshold.
 *                If its larger, make the foreground color black.
 *                Otherwise, make it white.
 */
          if (brightness > XmFOREGROUND_THRESHOLD)
          {
                  fg_color->red = 0;
                  fg_color->green = 0;
                  fg_color->blue = 0;
          }
          else
          {
                  fg_color->red = XmMAX_SHORT;
                  fg_color->green = XmMAX_SHORT;
                  fg_color->blue = XmMAX_SHORT;
          }
/*
 * End Fix 4602
 */
	}

	if (sel_color)
	{
		color_value = bg_color->red;
		color_value -= (color_value * XmCOLOR_LITE_SEL_FACTOR) / 100;
		sel_color->red = color_value;

		color_value = bg_color->green;
		color_value -= (color_value * XmCOLOR_LITE_SEL_FACTOR) / 100;
		sel_color->green = color_value;

		color_value = bg_color->blue;
		color_value -= (color_value * XmCOLOR_LITE_SEL_FACTOR) / 100;
		sel_color->blue = color_value;
	}

	if (bs_color)
	{
		color_value = bg_color->red;
		color_value -= (color_value * XmCOLOR_LITE_BS_FACTOR) / 100;
		bs_color->red = color_value;

		color_value = bg_color->green;
		color_value -= (color_value * XmCOLOR_LITE_BS_FACTOR) / 100;
		bs_color->green = color_value;

		color_value = bg_color->blue;
		color_value -= (color_value * XmCOLOR_LITE_BS_FACTOR) / 100;
		bs_color->blue = color_value;
	}

	if (ts_color)
	{
		color_value = bg_color->red;
		color_value -= (color_value * XmCOLOR_LITE_TS_FACTOR) / 100;
		ts_color->red = color_value;

		color_value = bg_color->green;
		color_value -= (color_value * XmCOLOR_LITE_TS_FACTOR) / 100;
		ts_color->green = color_value;

		color_value = bg_color->blue;
		color_value -= (color_value * XmCOLOR_LITE_TS_FACTOR) / 100;
		ts_color->blue = color_value;
	}
}

static void
CalculateColorsForDarkBackground(
        XColor *bg_color,
        XColor *fg_color,
        XColor *sel_color,
        XColor *ts_color,
        XColor *bs_color )
{
	int brightness = Brightness(bg_color);
	int color_value;

	if (fg_color)
	{
/*
 * Fix for 4602 - Compare the brightness with the foreground threshold.
 *                If its larger, make the foreground color black.
 *                Otherwise, make it white.
 */
          if (brightness > XmFOREGROUND_THRESHOLD)
          {
                  fg_color->red = 0;
                  fg_color->green = 0;
                  fg_color->blue = 0;
          }
          else
          {
                  fg_color->red = XmMAX_SHORT;
                  fg_color->green = XmMAX_SHORT;
                  fg_color->blue = XmMAX_SHORT;
          }
/*
 * End Fix 4602
 */
	}

	if (sel_color)
	{
		color_value = bg_color->red;
		color_value += XmCOLOR_DARK_SEL_FACTOR *
			(XmMAX_SHORT - color_value) / 100;
		sel_color->red = color_value;

		color_value = bg_color->green;
		color_value += XmCOLOR_DARK_SEL_FACTOR *
			(XmMAX_SHORT - color_value) / 100;
		sel_color->green = color_value;

		color_value = bg_color->blue;
		color_value += XmCOLOR_DARK_SEL_FACTOR *
			(XmMAX_SHORT - color_value) / 100;
		sel_color->blue = color_value;
	}

	if (bs_color)
	{
		color_value = bg_color->red;
		color_value += XmCOLOR_DARK_BS_FACTOR *
			(XmMAX_SHORT - color_value) / 100;
		bs_color->red = color_value;

		color_value = bg_color->green;
		color_value += XmCOLOR_DARK_BS_FACTOR *
			(XmMAX_SHORT - color_value) / 100;
		bs_color->green = color_value;

		color_value = bg_color->blue;
		color_value += XmCOLOR_DARK_BS_FACTOR *
			(XmMAX_SHORT - color_value) / 100;
		bs_color->blue = color_value;
	}

	if (ts_color)
	{
		color_value = bg_color->red;
		color_value += XmCOLOR_DARK_TS_FACTOR *
			(XmMAX_SHORT - color_value) / 100;
		ts_color->red = color_value;

		color_value = bg_color->green;
		color_value += XmCOLOR_DARK_TS_FACTOR *
			(XmMAX_SHORT - color_value) / 100;
		ts_color->green = color_value;

		color_value = bg_color->blue;
		color_value += XmCOLOR_DARK_TS_FACTOR *
			(XmMAX_SHORT - color_value) / 100;
		ts_color->blue = color_value;
	}
}

static void
CalculateColorsForMediumBackground(
        XColor *bg_color,
        XColor *fg_color,
        XColor *sel_color,
        XColor *ts_color,
        XColor *bs_color )
{
	int brightness = Brightness(bg_color);
	int color_value, f;

	if (brightness > XmFOREGROUND_THRESHOLD)
	{
		fg_color->red = 0;
		fg_color->green = 0;
		fg_color->blue = 0;
	}
	else
	{
		fg_color->red = XmMAX_SHORT;
		fg_color->green = XmMAX_SHORT;
		fg_color->blue = XmMAX_SHORT;
	}

	if (sel_color)
	{
		f = XmCOLOR_LO_SEL_FACTOR + (brightness
			* ( XmCOLOR_HI_SEL_FACTOR - XmCOLOR_LO_SEL_FACTOR )
			/ XmMAX_SHORT );

		color_value = bg_color->red;
		color_value -= (color_value * f) / 100;
		sel_color->red = color_value;

		color_value = bg_color->green;
		color_value -= (color_value * f) / 100;
		sel_color->green = color_value;

		color_value = bg_color->blue;
		color_value -= (color_value * f) / 100;
		sel_color->blue = color_value;
	}

	if (bs_color)
	{
		f = XmCOLOR_LO_BS_FACTOR + (brightness
			* ( XmCOLOR_HI_BS_FACTOR - XmCOLOR_LO_BS_FACTOR )
			/ XmMAX_SHORT);

		color_value = bg_color->red;
		color_value -= (color_value * f) / 100;
		bs_color->red = color_value;

		color_value = bg_color->green;
		color_value -= (color_value * f) / 100;
		bs_color->green = color_value;

		color_value = bg_color->blue;
		color_value -= (color_value * f) / 100;
		bs_color->blue = color_value;
	}

	if (ts_color)
	{
		f = XmCOLOR_LO_TS_FACTOR + (brightness
			* ( XmCOLOR_HI_TS_FACTOR - XmCOLOR_LO_TS_FACTOR )
			/ XmMAX_SHORT);

		color_value = bg_color->red;
		color_value += f * ( XmMAX_SHORT - color_value ) / 100;
		ts_color->red = color_value;

		color_value = bg_color->green;
		color_value += f * ( XmMAX_SHORT - color_value ) / 100;
		ts_color->green = color_value;

		color_value = bg_color->blue;
		color_value += f * ( XmMAX_SHORT - color_value ) / 100;
		ts_color->blue = color_value;
	}
}

static void
CalculateColorsRGB(
        XColor *bg_color,
        XColor *fg_color,
        XColor *sel_color,
        XColor *ts_color,
        XColor *bs_color )
{
	int brightness = Brightness(bg_color);

	/* make sure DefaultThresholds are inited */
	if (!XmTHRESHOLDS_INITD)
	    	GetDefaultThresholdsForScreen(
			DefaultScreenOfDisplay(_XmGetDefaultDisplay()));

	if (brightness < XmCOLOR_DARK_THRESHOLD)
		CalculateColorsForDarkBackground(bg_color, fg_color,
			sel_color, ts_color, bs_color);
	else if (brightness > XmCOLOR_LITE_THRESHOLD)
		CalculateColorsForLightBackground(bg_color, fg_color,
			sel_color, ts_color, bs_color);
	else
		CalculateColorsForMediumBackground(bg_color, fg_color,
			sel_color, ts_color, bs_color);
}


/*********************************************************************
 *
 *  GetColors
 *
 *********************************************************************/
static XmColorData *
GetColors(
        Screen *screen,
        Colormap color_map,
        Pixel background )
{
	Display * display = DisplayOfScreen (screen);
	XmColorData *old_colors;
	XmColorData new_colors;


	new_colors.screen = screen;
	new_colors.color_map = color_map;
	new_colors.background.pixel = background;

	if (_XmSearchColorCache(
		(XmLOOK_AT_SCREEN | XmLOOK_AT_CMAP | XmLOOK_AT_BACKGROUND),
			&new_colors, &old_colors)) {
               /*
		* initialize the thresholds if the current color scheme
		* already matched what is in the cache and the thresholds
		* haven't already been initialized.
                */
                if (!XmTHRESHOLDS_INITD)
	            GetDefaultThresholdsForScreen(screen);
		return(old_colors);
        }

	XQueryColor (display, color_map, &(new_colors.background));
	new_colors.allocated = XmBACKGROUND;

	/*
	 * Just in case somebody looks at these before they're ready,
	 * initialize them to a value that is always valid (for most
	 * implementations of X).
	 */
	new_colors.foreground.pixel = 0;
	new_colors.top_shadow.pixel = 0;
	new_colors.top_shadow.pixel = 0;
	new_colors.select.pixel = 0;

	/*  Generate the foreground, top_shadow, and bottom_shadow based  */
	/*  on the background                                             */

	if (DefaultDepthOfScreen(screen) == 1)
		SetMonochromeColors(&new_colors);
	else
	  {
	      XmScreenColorProc screen_color_proc ;

	      GetDefaultThresholdsForScreen(screen);

	      /* look for the new per-Screen resource */
	      screen_color_proc = _XmGetColorCalculationProc(screen);

	      if (!screen_color_proc) {
		  /* no new color proc set, use the old one */
		  (*ColorRGBCalcProc)(&(new_colors.background),
				      &(new_colors.foreground),
				      &(new_colors.select),
				      &(new_colors.top_shadow),
				      &(new_colors.bottom_shadow));
	      } else {
		  /* call the application */
		  (*screen_color_proc)(screen,
				       &(new_colors.background),
				       &(new_colors.foreground),
				       &(new_colors.select),
				       &(new_colors.top_shadow),
				       &(new_colors.bottom_shadow));
	      }
	  }
	return (_XmAddToColorCache(&new_colors));
}



/*********************************************************************
         Global API
 ********************************************************************/


/* DEPRECATED in favor of the Screen resource XmNcolorcalculationProc
   that takes a Screen in argument (while colorProc doesn't) */
XmColorProc
XmSetColorCalculation(
        XmColorProc proc )
{
	XmColorProc a = ColorRGBCalcProc;

	_XmProcessLock();
	if (proc != NULL)
		ColorRGBCalcProc = proc;
	else
		ColorRGBCalcProc = CalculateColorsRGB;

	_XmProcessUnlock();
	return(a);
}

/* DEPRECATED */
XmColorProc
XmGetColorCalculation( void )
{
	return(ColorRGBCalcProc);
}




void
XmGetColors(
        Screen *screen,
        Colormap color_map,
        Pixel background,
        Pixel *foreground_ret,
        Pixel *top_shadow_ret,
        Pixel *bottom_shadow_ret,
        Pixel *select_ret )
{
	XmColorData *cd;

        _XmDisplayToAppContext(DisplayOfScreen(screen));
        _XmAppLock(app);
	_XmProcessLock();
	cd = GetColors(screen, color_map, background);

	if (foreground_ret)
		*foreground_ret = AccessColorData(cd, XmFOREGROUND);
	if (top_shadow_ret)
		*top_shadow_ret = AccessColorData(cd, XmTOP_SHADOW);
	if (bottom_shadow_ret)
		*bottom_shadow_ret = AccessColorData(cd, XmBOTTOM_SHADOW);
	if (select_ret)
		*select_ret = AccessColorData(cd, XmSELECT);
	_XmProcessUnlock();
	_XmAppUnlock(app);
}


/*********************************************************************
 *
 *  XmeGetDefaultPixel
 *	Given the widget and the requested type of default, generate the
 *	default and store it in the value structure to be returned.
 *
 *********************************************************************/
/*ARGSUSED*/
void
XmeGetDefaultPixel(
        Widget widget,
        int type,
        int offset,
        XrmValue *value )
{
    Screen *screen;
    Colormap color_map;
    static Pixel new_value;
    XmColorData *color_data;
    Pixel background = 0 ;
    Widget parent;

    _XmWidgetToAppContext(widget);

    _XmAppLock(app);
    value->size = sizeof(new_value);
    value->addr = (char *) &new_value;

    if (!XtIsWidget(widget))
	{
	parent = widget->core.parent;
	color_map = parent->core.colormap;

	/*
	 *  Skip this for the background case.  The background
	 * field hasn't been inited yet but for the background
	 * case it isn't needed.
	 */

	if (type != XmBACKGROUND)
	    {
	    if ((XmIsLabelGadget(widget)) ||
		(XmIsArrowButtonGadget(widget)) ||
		(XmIsSeparatorGadget(widget)))
		XtVaGetValues(widget, XmNbackground, &background, NULL);
	    else
		{
		/*
		  This line should not be executed but this case does
		  need to be handled.
		  */
		background = parent->core.background_pixel;
		}
	    }
	}
    else
	{
	color_map = widget->core.colormap;
	if(type != XmBACKGROUND)
	    background = widget->core.background_pixel;
	}


    screen = XtScreen(widget);

    if (type == XmBACKGROUND)
	{
	color_data = GetDefaultColors(screen, color_map);
	}
    else
	{
	color_data = GetColors(screen, color_map, background);
	}

    new_value = AccessColorData(color_data, type);
    _XmAppUnlock(app);
}

/************************************************************************
 *
 *  Dynamic defaulting color functions
 *
 ************************************************************************/

void
_XmForegroundColorDefault(
        Widget widget,
        int offset,
        XrmValue *value )
{
   XmeGetDefaultPixel (widget, XmFOREGROUND, offset, value);
}

void
_XmHighlightColorDefault(
        Widget widget,
        int offset,
        XrmValue *value )
{
   XmeGetDefaultPixel (widget, XmFOREGROUND, offset, value);
}

void
_XmBackgroundColorDefault(
        Widget widget,
        int offset,
        XrmValue *value )
{
   XmeGetDefaultPixel (widget, XmBACKGROUND, offset, value);
}

void
_XmTopShadowColorDefault(
        Widget widget,
        int offset,
        XrmValue *value )
{
   XmeGetDefaultPixel (widget, XmTOP_SHADOW, offset, value);
}

void
_XmBottomShadowColorDefault(
        Widget widget,
        int offset,
        XrmValue *value )
{
   XmeGetDefaultPixel (widget, XmBOTTOM_SHADOW, offset, value);
}

void
_XmSelectColorDefault(
        Widget widget,
        int offset,
        XrmValue *value )
{
   XmeGetDefaultPixel (widget, XmSELECT, offset, value);
}

static
unsigned int
FromColorToBlackAndWhite(char *col)
{
   unsigned long r, g, b, bw;
   char k[5];
   k[4] = '\0';

   memcpy(k, col, 4);
   r = strtoul(k, NULL, 16);
   memcpy(k, col + 4, 4);
   g = strtoul(k, NULL, 16);
   b = strtoul(col + 8, NULL, 16);
   bw = (0.3 * r + 0.59 * g + 0.11 * b);
   return (bw);
}

Pixmap
_XmConvertToBW(Widget w, Pixmap pm)
{
   XpmImage im;
   int i = 0;
   unsigned int bw = 0, bw2 = 0;
   char *col = NULL, *col2 = NULL;
   Pixmap bw_pixmap = XmUNSPECIFIED_PIXMAP;
   char *data_before = NULL, *data_after = NULL;

   if (pm == XmUNSPECIFIED_PIXMAP)
	   return bw_pixmap;

   XpmCreateBufferFromPixmap(XtDisplay(w), &data_before, pm, 0, NULL);
   XpmCreateXpmImageFromPixmap(XtDisplay(w), pm, 0, &im, NULL);
   if (im.ncolors > 0) {
	   if (im.ncolors <= 2) {
		   if (im.ncolors == 1) {
			   col = strdup(im.colorTable[0].c_color);
			   if (col[0] == '#') {
				   bw = (FromColorToBlackAndWhite(col + 1) * 0.65);
				   sprintf(im.colorTable[0].c_color, "#%04x%04x%04x", bw, bw, bw);
			   }
			   if (col)
				   free(col);
		   } else {
			   col = im.colorTable[0].c_color;
			   col2 = im.colorTable[1].c_color;
			   if ((col[0] == '#') && (col2[0] == '#')) {
				   bw = FromColorToBlackAndWhite(col + 1);
				   bw2 = FromColorToBlackAndWhite(col2 + 1);
				   if (bw >= bw2) {
					   bw2 = bw2 + ((bw - bw2) * 0.65);
					   sprintf(im.colorTable[1].c_color, "#%04x%04x%04x", bw2, bw2, bw2);
				   } else {
					   bw = bw + ((bw2 - bw) * 0.65);
					   sprintf (im.colorTable[0].c_color, "#%04x%04x%04x", bw, bw, bw);
				   }
			   }
		   }
	   } else {
		   char e[5];
		   for (i = 0; i < im.ncolors; i++) {
			   col = im.colorTable[i].c_color;
			   if (col[0] == '#') {
				   bw = FromColorToBlackAndWhite(col + 1);
				   /* could be
					sprintf(im.colorTable[i].c_color, "#%04x%04x%04x", bw, bw, bw);
					Four lower lines is sprintf optimized version */
				   sprintf(e, "%04x", bw);
				   memcpy(col + 1, e, 4);
				   memcpy(col + 5, e, 4);
				   memcpy(col + 9, e, 4);
			   }
		   }
	   }
   }
   XpmCreatePixmapFromXpmImage(XtDisplay(w), pm, &im, &bw_pixmap, 0, NULL);
   if (bw_pixmap)
	   XpmCreateBufferFromPixmap(XtDisplay(w), &data_after, bw_pixmap, 0, NULL);

   if (data_before && data_after && !strcmp(data_before, data_after))
	   bw_pixmap = 0;

   if (data_before)
	   XpmFree(data_before);
   if (data_after)
	   XpmFree(data_after);
   XpmFreeXpmImage(&im);

   return (bw_pixmap) ? bw_pixmap : pm;
}

