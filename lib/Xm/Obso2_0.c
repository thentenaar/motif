/* $TOG: Obso2_0.c /main/10 1998/08/18 18:59:44 samborn $ */
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

#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>

#include <X11/Intrinsic.h>
#include <Xm/BaseClassP.h>
#include <Xm/ColorP.h>
#include <Xm/DesktopP.h>
#include <Xm/DisplayP.h>
#include <Xm/DrawingAP.h>
#include <Xm/FileSBP.h>
#include <Xm/GadgetP.h>
#include <Xm/List.h>
#include <Xm/ManagerP.h>
#include <Xm/MenuShellP.h>
#include <Xm/MenuStateP.h>
#include <Xm/PrimitiveP.h>
#include <Xm/PushBGP.h>
#include <Xm/PushBP.h>
#include <Xm/RowColumnP.h>
#include <Xm/ScaleP.h>
#include <Xm/ScreenP.h>
#include <Xm/ScrolledWP.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>
#include <Xm/TextInP.h>
#include <Xm/TextP.h>
#include <Xm/TransltnsP.h>
#include <Xm/VendorSEP.h>
#include <Xm/XmP.h>
#include <Xm/XmosP.h>
#include "BaseClassI.h"
#include "BulletinBI.h"
#include "ColorI.h"
#include "ExtObjectI.h"
#include "ImageCachI.h"
#include "MapEventsI.h"
#include "MenuStateI.h"
#include "MessagesI.h"
#include "PixConvI.h"
#include "ReadImageI.h"
#include "ResConverI.h"
#include "TextOutI.h"
#include "TravActI.h"
#include "TraversalI.h"
#include "XmI.h"

/********    Static Function Declarations    ********/

/********    End Static Function Declarations    ********/

/* Exported variables that are now static. */

XmPrimitiveClassExtRec _XmLabelPrimClassExtRec = {
  NULL, NULLQUARK, 1L, sizeof(XmPrimitiveClassExtRec), NULL, NULL, NULL
};

XmPrimitiveClassExtRec _XmTextPrimClassExtRec = {
  NULL, NULLQUARK, 1L, sizeof(XmPrimitiveClassExtRec),
  _XmTextGetBaselines, _XmTextGetDisplayRect, _XmTextMarginsProc
};

XmPrimitiveClassExtRec _XmTextFPrimClassExtRec = {
  NULL, NULLQUARK, 1L, sizeof(XmPrimitiveClassExtRec), NULL, NULL, NULL
};


XmGadgetClassExtRec _XmGadClassExtRec = {
  NULL, NULLQUARK, 1L, sizeof(XmGadgetClassExtRec), NULL, NULL
};

XmGadgetClassExtRec _XmLabelGadClassExtRec = {
  NULL, NULLQUARK, 1L, sizeof(XmGadgetClassExtRec), NULL, NULL
};

XmGadgetClassExtRec _XmPushBGadClassExtRec = {
  NULL, NULLQUARK, 1L, sizeof(XmGadgetClassExtRec),
  XmInheritBaselineProc, XmInheritDisplayRectProc
};

XmGadgetClassExtRec _XmToggleBGadClassExtRec = {
  NULL, NULLQUARK, 1L, sizeof(XmGadgetClassExtRec),
  XmInheritBaselineProc, XmInheritDisplayRectProc
};

/* XmWorld is defunct. */

typedef struct _XmWorldClassPart {
  XtPointer		extension;
} XmWorldClassPart;

typedef struct _XmWorldClassRec {
  ObjectClassPart	object_class;
  XmExtClassPart	ext_class;
  XmDesktopClassPart 	desktop_class;
  XmWorldClassPart	world_class;
} XmWorldClassRec;

typedef struct {
  int			foo;
} XmWorldPart;

typedef struct _XmWorldRec {
  ObjectPart		object;
  XmExtPart		ext;
  XmDesktopPart		desktop;
  XmWorldPart		world;
} XmWorldRec;

externaldef(worldobjectclass) XmWorldClassRec xmWorldClassRec =
{
  {
    (WidgetClass) &xmDesktopClassRec, /* superclass		*/
    "World",			/* class_name 		*/
    sizeof(XmWorldRec),		/* size 		*/
    NULL,			/* Class Initializer 	*/
    NULL,			/* class_part_init 	*/
    FALSE,			/* Class init'ed ? 	*/
    NULL,			/* initialize         	*/
    NULL,			/* initialize_notify    */
    NULL,			/* realize            	*/
    NULL,			/* actions            	*/
    0,				/* num_actions        	*/
    NULL,			/* resources          	*/
    0,				/* resource_count     	*/
    NULLQUARK, 			/* xrm_class          	*/
    FALSE,			/* compress_motion    	*/
    FALSE,			/* compress_exposure  	*/
    FALSE,			/* compress_enterleave	*/
    FALSE,			/* visible_interest   	*/
    NULL,			/* destroy            	*/
    NULL,			/* resize             	*/
    NULL,			/* expose             	*/
    NULL,			/* set_values         	*/
    NULL,			/* set_values_hook      */
    NULL,			/* set_values_almost    */
    NULL,			/* get_values_hook      */
    NULL,			/* accept_focus       	*/
    XtVersion, 			/* intrinsics version 	*/
    NULL,			/* callback offsets   	*/
    NULL,			/* tm_table           	*/
    NULL,			/* query_geometry       */
    NULL,			/* world_accelerator    */
    NULL,			/* extension            */
  },
  {				/* ext			*/
    NULL,			/* synthetic resources	*/
    0,				/* num syn resources	*/
    NULL,			/* extension		*/
  },
  {				/* desktop		*/
    (WidgetClass) &xmDisplayClassRec, /* child_class		*/
    XtInheritInsertChild,	/* insert_child		*/
    XtInheritDeleteChild,	/* delete_child		*/
    NULL,			/* extension		*/
  }
};

externaldef(worldobjectclass) WidgetClass
     xmWorldClass = (WidgetClass) &xmWorldClassRec;

/************************************************************************
 *                                                                      *
 * _XmGetRealXlations - was global in ScrolledW.c, is now static.       *
 *                                                                      *
 ************************************************************************/

char *
_XmGetRealXlations(
        Display *dpy,
        _XmBuildVirtualKeyStruct *keys,
        int num_keys )
{
  char *result, buf[1000];
  char *tmp = buf;
  char *keystring;
  register int i;
  int num_vkeys;
  XmKeyBinding vkeys;
  KeySym    keysym;
  Modifiers mods;

  *tmp = '\0';
  for (i = 0; i < num_keys; i++)
    {
      keysym = XStringToKeysym(keys[i].key);
      if (keysym == NoSymbol)
	break;

      /* A virtual keysym may map to multiple real keysyms. */
      num_vkeys = XmeVirtualToActualKeysyms(dpy, keysym, &vkeys);
      while (--num_vkeys >= 0)
	{
	  keystring = XKeysymToString(vkeys[num_vkeys].keysym);
	  if (!keystring)
	    break;
	  mods = vkeys[num_vkeys].modifiers | keys[i].mod;

	  if (mods & ControlMask)
	    strcat(tmp, "Ctrl ");

	  if (mods & ShiftMask)
	    strcat(tmp, "Shift ");

	  if (mods & Mod1Mask)
	    strcat(tmp, "Mod1 ");  /* "Alt" may not be right on some systems */

	  strcat(tmp,"<Key>");
	  strcat(tmp, keystring);
	  strcat(tmp,": ");
	  strcat(tmp,keys[i].action);

	  tmp += strlen(tmp);
	  assert((tmp - buf) < 1000);
	}

      XtFree((char*) vkeys);
    }

  if (buf[0] != '\0')
    result = XtNewString(buf);
  else
    result = NULL;
  return(result);

}


/************************************************************************
 *
 *  _XmSetEtchedSlider
 *	Set the scrollbar variable which causes the slider pixmap
 *	to be etched. Was in ScrollBar.c
 ************************************************************************/
void
_XmSetEtchedSlider(
        XmScrollBarWidget sbw )
{
   XtVaSetValues((Widget) sbw, XmNsliderVisual, XmETCHED_LINE, NULL);
}




/**********************************************************************
 *
 * _XmSortResourceList: superceded by XmReorderResourceList
 *
 **********************************************************************/
void
_XmSortResourceList(
        XrmResource *list[],
        Cardinal len )
{
	static Boolean first_time = TRUE;
	static XrmQuark unitQ;
	int n,i;
	XrmResource *p = NULL;

	if (first_time)
	{
		unitQ = XrmPermStringToQuark(XmNunitType);
		first_time = FALSE;
	}

	for (n=0; n < len; n++)
		if (list[n]->xrm_name == unitQ)
		{
			p = list[n];
			break;
		}

	if (n == len)
		return; /* No unit type resource found in this list. */
	else
	{
		for (i=n; i > 0; i--)
			list[i] = list[i-1];

		list[0] = p;
	}
}


/************************************************************************
 ************************************************************************
  Stuff that was in Visual.c, now obsolete by the XtReOrderResourceList
  API.
 ************************************************************************/



/************************************************************************
 *
 *  _XmGetBGPixmapName
 *	Return the background pixmap name set by the string to background
 *	resource converter.  This is used by primitive and manager.
 *
 ************************************************************************/
char *
_XmGetBGPixmapName( void )
{
   return NULL;
}



/************************************************************************
 *
 *  _XmClearBGPixmapName
 *	Clear the background pixmap name set by the string to background
 *	resource converter.  This is used by primitive and manager.
 *
 ************************************************************************/
void
_XmClearBGPixmapName( void )
{
  /*EMPTY*/
}


/**---------------------------------------------------------*/
/**---------------------------------------------------------*/
/* This code was in Visual.c, global while only localy used */

#define MESSAGE0	_XmMsgVisual_0000
#define MESSAGE1	_XmMsgVisual_0001
#define MESSAGE2	_XmMsgVisual_0002


extern Pixel
_XmBlackPixel(
	      Screen *screen,
	      Colormap colormap,
	      XColor blackcolor )
{
  Pixel p;

  blackcolor.red = 0;
  blackcolor.green = 0;
  blackcolor.blue = 0;

  if (colormap == DefaultColormapOfScreen(screen))
    p = blackcolor.pixel = BlackPixelOfScreen(screen);
  else if (XAllocColor(screen->display, colormap, &blackcolor))
    p = blackcolor.pixel;
  else
    p = blackcolor.pixel = BlackPixelOfScreen(screen); /* fallback pixel */

  return (p);
}


extern Pixel
_XmWhitePixel(
	      Screen *screen,
	      Colormap colormap,
	      XColor whitecolor )
{
  Pixel p;

  whitecolor.red = XmMAX_SHORT;
  whitecolor.green = XmMAX_SHORT;
  whitecolor.blue = XmMAX_SHORT;

  if (colormap == DefaultColormapOfScreen(screen))
    p = whitecolor.pixel = WhitePixelOfScreen(screen);
  else if (XAllocColor(screen->display, colormap, &whitecolor))
    p = whitecolor.pixel;
  else
    p = whitecolor.pixel = WhitePixelOfScreen(screen); /* fallback pixel */
  return (p);
}


String
_XmGetDefaultBackgroundColorSpec( Screen *screen )
{
  XrmName names[2];
  XrmClass classes[2];
  XrmRepresentation rep;
  XrmValue db_value;
  String default_background_color_spec = NULL ;

  names[0] = XrmPermStringToQuark(XmNbackground);
  names[1] = NULLQUARK;

  classes[0] = XrmPermStringToQuark(XmCBackground);
  classes[1] = NULLQUARK;

  if (XrmQGetResource(XtScreenDatabase(screen), names, classes,
		      &rep, &db_value))
     {
	if (rep == XrmPermStringToQuark(XmRString))
	    default_background_color_spec = db_value.addr;
     }
  else default_background_color_spec = XmDEFAULT_BACKGROUND;

  return(default_background_color_spec);
}


/*ARGSUSED*/
void
_XmSetDefaultBackgroundColorSpec(
	Screen *screen,	/* unused */
        String new_color_spec )
{
    static Boolean app_defined = FALSE;
    String default_background_color_spec = NULL ;

    if (app_defined)
	{
	    XtFree(default_background_color_spec);
	}

    default_background_color_spec = (String)
	XtMalloc(strlen(new_color_spec) + 1);
    /* this needs to be set per screen */
    strcpy(default_background_color_spec, new_color_spec);

    app_defined = TRUE;
}

/*
 * GLOBAL VARIABLES
 *
 * These variables define the color cache.
 */


/* Thresholds for brightness
   above LITE threshold, LITE color model is used
   below DARK threshold, DARK color model is be used
   use STD color model in between */

static int XmCOLOR_LITE_THRESHOLD;
static int XmCOLOR_DARK_THRESHOLD;
static int XmFOREGROUND_THRESHOLD;
static Boolean XmTHRESHOLDS_INITD = FALSE;



void
_XmGetDefaultThresholdsForScreen( Screen *screen )
{
  XrmName names[2];
  XrmClass classes[2];
  XrmRepresentation rep;
  XrmValue db_value, to_value;
  int int_value;
  int default_light_threshold_spec;
  int default_dark_threshold_spec;
  int default_foreground_threshold_spec;
  WidgetRec widget;

  XmTHRESHOLDS_INITD = True;

 /*
  * We need a widget to pass into the XtConvertAndStore() function
  * to convert the string to an int.  Since a widget can't be
  * passed into this procedure because the public interfaces
  * that call this routine don't have a widget, we need this hack
  * to create a dummy widget.
  */
  bzero((void*) &widget, sizeof(widget) );
  widget.core.self = &widget;
  widget.core.widget_class = coreWidgetClass;
  widget.core.screen = screen;
  XtInitializeWidgetClass(coreWidgetClass);


  names[0] = XrmPermStringToQuark(XmNlightThreshold);
  names[1] = NULLQUARK;

  classes[0] = XrmPermStringToQuark(XmCLightThreshold);
  classes[1] = NULLQUARK;

  if (XrmQGetResource(XtScreenDatabase(screen), names, classes,
		      &rep, &db_value))
    {
     /* convert the string to an int value */
      to_value.size = sizeof(int);
      to_value.addr = (XPointer) &int_value;
      if (XtConvertAndStore(&widget, XmRString, &db_value, XmRInt, &to_value))
      {
	default_light_threshold_spec = int_value;
	if ( (default_light_threshold_spec < 0) ||
	     (default_light_threshold_spec > 100) )
	  default_light_threshold_spec = XmDEFAULT_LIGHT_THRESHOLD;
      }
      else default_light_threshold_spec = XmDEFAULT_LIGHT_THRESHOLD;
    }
  else default_light_threshold_spec = XmDEFAULT_LIGHT_THRESHOLD;

  names[0] = XrmPermStringToQuark(XmNdarkThreshold);
  names[1] = NULLQUARK;

  classes[0] = XrmPermStringToQuark(XmCDarkThreshold);
  classes[1] = NULLQUARK;

  if (XrmQGetResource(XtScreenDatabase(screen), names, classes,
		      &rep, &db_value))
    {
     /* convert the string to an int value */
      to_value.size = sizeof(int);
      to_value.addr = (XPointer) &int_value;
      if (XtConvertAndStore(&widget, XmRString, &db_value, XmRInt, &to_value))
      {
        XtConvertAndStore(&widget, XmRString, &db_value, XmRInt, &to_value);
	default_dark_threshold_spec = int_value;
	if ( (default_dark_threshold_spec < 0) ||
	     (default_dark_threshold_spec > 100) )
	  default_dark_threshold_spec = XmDEFAULT_DARK_THRESHOLD;
       }
       else default_dark_threshold_spec = XmDEFAULT_DARK_THRESHOLD;
    }
  else default_dark_threshold_spec = XmDEFAULT_DARK_THRESHOLD;

  names[0] = XrmPermStringToQuark(XmNforegroundThreshold);
  names[1] = NULLQUARK;

  classes[0] = XrmPermStringToQuark(XmCForegroundThreshold);
  classes[1] = NULLQUARK;

  if (XrmQGetResource(XtScreenDatabase(screen), names, classes,
		      &rep, &db_value))
    {
     /* convert the string to an int value */
      to_value.size = sizeof(int);
      to_value.addr = (XPointer) &int_value;
      if (XtConvertAndStore(&widget, XmRString, &db_value, XmRInt, &to_value))
      {
	default_foreground_threshold_spec = int_value;
	if ( (default_foreground_threshold_spec < 0) ||
	     (default_foreground_threshold_spec > 100) )
	  default_foreground_threshold_spec = XmDEFAULT_FOREGROUND_THRESHOLD;
      }
      else default_foreground_threshold_spec = XmDEFAULT_FOREGROUND_THRESHOLD;
    }
  else default_foreground_threshold_spec = XmDEFAULT_FOREGROUND_THRESHOLD;

  XmCOLOR_LITE_THRESHOLD = default_light_threshold_spec * XmCOLOR_PERCENTILE;
  XmCOLOR_DARK_THRESHOLD = default_dark_threshold_spec * XmCOLOR_PERCENTILE;
  XmFOREGROUND_THRESHOLD = default_foreground_threshold_spec * XmCOLOR_PERCENTILE;
}




/********--------------------------------------
  IconPixmap VendorShell stuff previously in VendorSE.c.
  Obsolete because XmRBitmap is now used to convert
 ********-------------------------------------*/


/************************************************************************
 *
 *  _XmGetIconPixmapName
 *      Return the icon pixmap name set by the string to icon resource
 *      converter.  This is used by the vendor shell.
 *
 ************************************************************************/
char*
_XmGetIconPixmapName( void )
{
   return NULL;
}

/************************************************************************
 *
 *  _XmClearIconPixmapName
 *      Clear the icon pixmap name set by the string to icon resource
 *      converter.  This is used by the vendor shell.
 *
 ************************************************************************/
void
_XmClearIconPixmapName( void )
{
  /*EMPTY*/
}


/************************************************************************
 *									*
 * _XmInitializeScrollBars - initialize the scrollbars for auto mode.	*
 *			WAS in ScrolledW.c      			*
 ************************************************************************/
#define ScrollBarVisible( wid)      (wid && XtIsManaged( wid))
void
_XmInitializeScrollBars(
        Widget w )
{
    XmScrolledWindowWidget	sw = (XmScrolledWindowWidget) w;
    int i, inc;
    Dimension bw;
    Arg vSBArgs[6];
    Arg hSBArgs[6];

    if (sw->swindow.VisualPolicy == XmVARIABLE)
	return;

    bw = 0;
    if (sw->swindow.WorkWindow)
        bw = sw->swindow.WorkWindow->core.border_width;

    sw->swindow.vmin = 0;
    sw->swindow.vOrigin = 0;
    sw->swindow.hmin = 0;
    sw->swindow.hOrigin = 0;
    if (ScrollBarVisible(sw->swindow.WorkWindow))
    {
        sw->swindow.vOrigin = abs(sw->swindow.WorkWindow->core.y);
	sw->swindow.vmax = sw->swindow.WorkWindow->core.height + (2 * bw);
	if (sw->swindow.vmax <1) sw->swindow.vmax = 1;
	sw->swindow.vExtent = sw->swindow.AreaHeight;
        if (sw->swindow.vOrigin < sw->swindow.vmin)
            sw->swindow.vOrigin = sw->swindow.vmin;

	if ((sw->swindow.vExtent + sw->swindow.vOrigin) > sw->swindow.vmax)
	    sw->swindow.vExtent = sw->swindow.vmax - sw->swindow.vOrigin;
	if (sw->swindow.vExtent < 0)
        {
	    sw->swindow.vExtent = sw->swindow.vmax;
            sw->swindow.vOrigin = sw->swindow.vmin;
        }

	sw->swindow.hmax = sw->swindow.WorkWindow->core.width + (2 * bw);
	if (sw->swindow.hmax <1) sw->swindow.hmax = 1;
        sw->swindow.hOrigin = abs(sw->swindow.WorkWindow->core.x);
	sw->swindow.hExtent = sw->swindow.AreaWidth;
        if (sw->swindow.hOrigin < sw->swindow.hmin)
            sw->swindow.hOrigin = sw->swindow.hmin;

	if ((sw->swindow.hExtent + sw->swindow.hOrigin) > sw->swindow.hmax)
	    sw->swindow.hExtent = sw->swindow.hmax - sw->swindow.hOrigin;
	if (sw->swindow.hExtent < 0)
        {
	    sw->swindow.hExtent = sw->swindow.hmax;
            sw->swindow.hOrigin = sw->swindow.hmin;
        }

    }
    else
    {
	sw->swindow.vExtent = (sw->swindow.ClipWindow->core.height > 0) ?
			       sw->swindow.ClipWindow->core.height : 1;
	sw->swindow.hExtent = (sw->swindow.ClipWindow->core.width > 0) ?
			       sw->swindow.ClipWindow->core.width : 1;
	sw->swindow.vmax = sw->swindow.vExtent;
	sw->swindow.hmax = sw->swindow.hExtent;
    }
    if(sw->swindow.vScrollBar)
    {
	i = 0;
        if (sw->swindow.WorkWindow)
        {
            if ((inc = ((sw->swindow.WorkWindow->core.height) / 10)) < 1)
                inc = 1;
            XtSetArg (vSBArgs[i], XmNincrement, (XtArgVal) inc); i++;
        }
	if ((inc = (sw->swindow.AreaHeight - (sw->swindow.AreaHeight / 10))) < 1)
	    inc = sw->swindow.AreaHeight;
        XtSetArg (vSBArgs[i], XmNpageIncrement, (XtArgVal) inc); i++;
	XtSetArg (vSBArgs[i], XmNminimum, (XtArgVal) (sw->swindow.vmin)); i++;
	XtSetArg (vSBArgs[i], XmNmaximum, (XtArgVal) (sw->swindow.vmax)); i++;
	XtSetArg (vSBArgs[i], XmNvalue, (XtArgVal) sw->swindow.vOrigin); i++;
	XtSetArg (vSBArgs[i], XmNsliderSize, (XtArgVal) (sw->swindow.vExtent)); i++;
	assert(i <= XtNumber(vSBArgs));
	XtSetValues((Widget) sw->swindow.vScrollBar,vSBArgs,i);
    }
    if(sw->swindow.hScrollBar)
    {
	i = 0;
        if (sw->swindow.WorkWindow)
        {
            if ((inc = ((sw->swindow.WorkWindow->core.width) / 10)) < 1)
                inc = 1;
            XtSetArg (hSBArgs[i], XmNincrement, (XtArgVal) inc); i++;
        }
	if ((inc = (sw->swindow.AreaWidth - (sw->swindow.AreaWidth / 10))) < 1)
	    inc = sw->swindow.AreaWidth;

        XtSetArg (hSBArgs[i], XmNpageIncrement, (XtArgVal) inc); i++;
	XtSetArg (hSBArgs[i], XmNminimum, (XtArgVal) (sw->swindow.hmin)); i++;
	XtSetArg (hSBArgs[i], XmNmaximum, (XtArgVal) (sw->swindow.hmax)); i++;
	XtSetArg (hSBArgs[i], XmNvalue, (XtArgVal) sw->swindow.hOrigin); i++;
	XtSetArg (hSBArgs[i], XmNsliderSize, (XtArgVal) (sw->swindow.hExtent)); i++;
	assert(i <= XtNumber(hSBArgs));
	XtSetValues((Widget) sw->swindow.hScrollBar,hSBArgs,i);
    }

}

void
InitializeScrollBars(
        Widget w )
{
  _XmInitializeScrollBars( w) ;
}


void
_XmClearBCompatibility(
        Widget pb )
{

	((XmPushButtonWidget) pb)->pushbutton.compatible =  False;
}


void
_XmClearBGCompatibility(
        Widget pbg )
{
	 PBG_Compatible (pbg) = False;
}




/****************************************************************/
void
_XmBulletinBoardSetDefaultShadow(
        Widget button )
{
            Arg             argv[2] ;
            Cardinal        argc ;
            Dimension       dbShadowTh = 0;
            Dimension       shadowTh ;
/****************/

    if(    XmIsPushButtonGadget( button)    )
    {   _XmClearBGCompatibility( button) ;
        }
    else
    {   if(    XmIsPushButton( button)    )
        {   _XmClearBCompatibility( button) ;
            }
        }
    argc = 0 ;
    XtSetArg( argv[argc], XmNshadowThickness, &shadowTh) ; ++argc ;
    XtSetArg( argv[argc], XmNdefaultButtonShadowThickness,
                                                        &dbShadowTh) ; ++argc ;
    XtGetValues( button, argv, argc) ;

    if(    !dbShadowTh    )
    {   if(    shadowTh > 1    )
        {   dbShadowTh = shadowTh >> 1 ;
            }
        else
        {   dbShadowTh = shadowTh ;
            }
        argc = 0 ;
        XtSetArg( argv[argc], XmNdefaultButtonShadowThickness,
                                                         dbShadowTh) ; ++argc ;
        XtSetValues( button, argv, argc) ;
        }
    return ;
    }


static Widget
GetBBWithDB(
 	Widget wid)
 {
   Widget focus ;

   if(    (_XmGetFocusPolicy( wid) == XmEXPLICIT)
      &&  (    (focus = XmGetFocusWidget( wid))
 	  ||  (focus = _XmGetFirstFocus( wid)))    )
     {
       while(    focus
 	    &&  !XtIsShell( focus)    )
 	{
 	  if(    XmIsBulletinBoard( focus)
 	     &&  BB_DefaultButton( focus)    )
 	    {
 	      return focus ;
 	    }
 	  focus = XtParent( focus) ;
 	}

     }
   return NULL ;
 }


void
_XmBBUpdateDynDefaultButton(
 	Widget bb)
 {
   Widget bbwdb = GetBBWithDB( bb) ;

   if(    bbwdb == NULL    )
     {
       if(    ((XmBulletinBoardWidget) bb)
 	                           ->bulletin_board.dynamic_default_button    )
 	{
 	  _XmBulletinBoardSetDynDefaultButton( bb, NULL) ;
 	}
     }
   else
     {
       if(    bbwdb == bb    )
 	{
 	  _XmBulletinBoardSetDynDefaultButton( bb, BB_DefaultButton( bb)) ;
 	}
     }
 }

/********************************************************************/

Boolean
_XmDifferentBackground(
        Widget w,
        Widget parent )
{
    if (XmIsPrimitive (w) && XmIsManager (parent)) {
	if (w->core.background_pixel != parent->core.background_pixel ||
	    w->core.background_pixmap != parent->core.background_pixmap)
	    return (True);
    }

    return (False);
}


/*******************************************************************
  More Visual.c stuff not used */



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


/* This one still used by mwm: fix mwm */

/*********************************************************************
 *
 *  _XmGetColors
 *
 *********************************************************************/
XmColorData *
_XmGetColors(
        Screen *screen,
        Colormap color_map,
        Pixel background )
{
	Display * display = DisplayOfScreen (screen);
	XmColorData *old_colors;
	XmColorData new_colors;
	XmColorProc ColorRGBCalcProc ;


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
	            _XmGetDefaultThresholdsForScreen(screen);
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
	    _XmGetDefaultThresholdsForScreen(screen);
	    ColorRGBCalcProc = XmGetColorCalculation() ;
	    (*ColorRGBCalcProc)(&(new_colors.background),
				&(new_colors.foreground), &(new_colors.select),
				&(new_colors.top_shadow), &(new_colors.bottom_shadow));
	  }
	return (_XmAddToColorCache(&new_colors));
}



XmColorData *
_XmGetDefaultColors(
        Screen *screen,
        Colormap color_map )
{
	static XmColorData ** default_set = NULL;
	static int default_set_count = 0;
	static int default_set_size = 0;
	register int i;
	XColor color_def;
	static Pixel background;
        XrmValue fromVal;
        XrmValue toVal;
        XrmValue args[2];
        Cardinal num_args;
	String default_string = XtDefaultBackground;

	/*  Look through  a set of screen / background pairs to see  */
	/*  if the default is already in the table.                  */

	for (i = 0; i < default_set_count; i++)
	{
	if ((default_set[i]->screen == screen) &&
		(default_set[i]->color_map == color_map))
		return (default_set[i]);
	}

	/*  See if more space is needed in the array  */

	if (default_set == NULL)
	{
		default_set_size = 10;
		default_set = (XmColorData **) XtRealloc((char *) default_set,
			(sizeof(XmColorData *) * default_set_size));

	}
	else if (default_set_count == default_set_size)
	{
		default_set_size += 10;
		default_set = (XmColorData **) XtRealloc((char *) default_set,
			sizeof(XmColorData *) * default_set_size);
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

		if (XParseColor(DisplayOfScreen(screen), color_map,
			_XmGetDefaultBackgroundColorSpec(screen), &color_def))
		{
			if (XAllocColor(DisplayOfScreen(screen), color_map,
				&color_def))
			{
				background = color_def.pixel;
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
	 * of the data pointed to by color_set (defined in _XmGetColors).
	 */

	default_set[default_set_count] =
		_XmGetColors(screen, color_map, background);
	default_set_count++;

	return (default_set[default_set_count - 1]);
}



static int
_XmBrightness(
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

/* This one still used by mwm: fix mwm */
Pixel
_XmAccessColorData(
        XmColorData *cd,
        unsigned char which )
{
    Pixel p;

    switch(which) {
    case XmBACKGROUND:
	if (!(cd->allocated & which) &&
	    (XAllocColor(cd->screen->display,
			 cd->color_map, &(cd->background)) == 0)) {
	    if (_XmBrightness(&(cd->background))
		< XmFOREGROUND_THRESHOLD )
		cd->background.pixel = _XmBlackPixel(cd->screen,
						     cd->color_map,
						     cd->background);
	    else
		cd->background.pixel = _XmWhitePixel(cd->screen,
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
	    (XAllocColor(cd->screen->display,
			 cd->color_map, &(cd->foreground)) == 0 ))
	    {
		if (_XmBrightness(&(cd->background))
		    < XmFOREGROUND_THRESHOLD )
		    cd->foreground.pixel = _XmWhitePixel(cd->screen,
							 cd->color_map,
							 cd->foreground);
		else
		    cd->foreground.pixel = _XmBlackPixel(cd->screen,
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
	    (XAllocColor(cd->screen->display,
			 cd->color_map, &(cd->top_shadow)) == 0))
	    {
		if (_XmBrightness(&(cd->background))
		    > XmCOLOR_LITE_THRESHOLD)
		    cd->top_shadow.pixel =
			_XmBlackPixel(cd->screen, cd->color_map,
				      cd->top_shadow);
		else
		    cd->top_shadow.pixel =
			_XmWhitePixel(cd->screen, cd->color_map,
				      cd->top_shadow);
		XQueryColor(cd->screen->display, cd->color_map,
			    &(cd->top_shadow));

	    }
	p = cd->top_shadow.pixel;
	cd->allocated |= which;
	break;
    case XmBOTTOM_SHADOW:
	if (!(cd->allocated & which) &&
	    (XAllocColor(cd->screen->display,
			 cd->color_map, &(cd->bottom_shadow)) == 0))
	    {
		if (_XmBrightness(&(cd->background))
		    < XmCOLOR_DARK_THRESHOLD)
		    cd->bottom_shadow.pixel =
			_XmWhitePixel(cd->screen, cd->color_map,
				      cd->bottom_shadow);
		else
		    cd->bottom_shadow.pixel =
			_XmBlackPixel(cd->screen, cd->color_map,
				      cd->bottom_shadow);
		XQueryColor(cd->screen->display, cd->color_map,
			    &(cd->bottom_shadow));
	    }
	p = cd->bottom_shadow.pixel;
	cd->allocated |= which;
	break;
    case XmSELECT:
	if (!(cd->allocated & which) &&
	    (XAllocColor(cd->screen->display,
			 cd->color_map, &(cd->select)) == 0))
	    {
		if (_XmBrightness(&(cd->background))
		    < XmFOREGROUND_THRESHOLD)
		    cd->select.pixel = _XmWhitePixel(cd->screen,
						     cd->color_map,
						     cd->select);
		else
		    cd->select.pixel = _XmBlackPixel(cd->screen,
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
	p = _XmBlackPixel(cd->screen, cd->color_map, cd->background);
	break;
    }

    return(p);
}


/* OBSOLETE: Replaced by _XmMapKeyEvents. */
Boolean
_XmMapKeyEvent(
        register String str,
        int *eventType,
        unsigned int *keysym,
        unsigned int *modifiers )
{
  int		count;
  int          *all_types;
  KeySym       *all_keys;
  unsigned int *all_mods;

  /* Initialize the return parameters. */
  *eventType = 0;
  *keysym = 0;
  *modifiers = 0;

  /* Convert the string to a list of keysyms. */
  count = _XmMapKeyEvents(str, &all_types, &all_keys, &all_mods);

  /* The old implementation ignored trailing garbage. */
  if (count > 0)
    {
      *eventType = *all_types;
      *keysym = *all_keys;
      *modifiers = *all_mods;
    }

  XtFree((char*) all_types);
  XtFree((char*) all_keys);
  XtFree((char*) all_mods);

  return (count > 0);
}


void XmRegisterConverters() { _XmRegisterConverters() ; }


/************************************************************************
 *
 *  _XmGetImageAndHotSpotFromFile
 *	Given a filename, extract and create an image from the file data.
 *
 ************************************************************************/
XImage *
_XmGetImageAndHotSpotFromFile(
        char *filename,
	int *hot_x,
	int *hot_y)
{
    return _XmReadImageAndHotSpotFromFile(_XmGetDefaultDisplay(),
					filename, hot_x, hot_y) ;

}


/************************************************************************
 *
 *  _XmGetImageFromFile
 *	Given a filename, extract and create an image from the file data.
 *
 ************************************************************************/

XImage *
_XmGetImageFromFile(
        char *filename )
{
   int hot_x, hot_y;
   return _XmReadImageAndHotSpotFromFile(_XmGetDefaultDisplay(),
					filename, &hot_x, &hot_y) ;
}

Boolean
_XmFocusIsInShell(
        Widget wid)
{
  /* CR 7568: Actually return the value. */
  return XmeFocusIsInShell(wid);
}


void
_XmSleep(
        unsigned int secs)
{
  sleep( secs) ;
}

int
_XmOSPutenv(
    char *string)
{
#if HAVE_PUTENV
  return putenv(string);
#else
  char *value;

  if ((value = strchr(string, '=')) != NULL)
    {
      char *name  = XtNewString(string);
      int result;

      name[value-string] = '\0';

      result = setenv(name, value+1, 1);
      XtFree(name);
      return result;
    }
  else
    return -1;
#endif /* HAVE_PUTENV */
}

/* after rework of inheritance of class extension method */
externaldef(xmprimbaseclassextrec) XmBaseClassExtRec
     _XmPrimbaseClassExtRec = { 0 };
externaldef(xmprimclassextrec) XmPrimitiveClassExtRec
     _XmPrimClassExtRec = { 0 };
externaldef(xmdrawnbprimclassextrec) XmPrimitiveClassExtRec
     _XmDrawnBPrimClassExtRec = { 0 };
externaldef(xmtogglebprimclassextrec) XmPrimitiveClassExtRec
     _XmToggleBPrimClassExtRec = { 0 };
externaldef(xmpushbprimclassextrec) XmPrimitiveClassExtRec
     _XmPushBPrimClassExtRec = { 0 };
externaldef(xmcascadebprimclassextrec) XmPrimitiveClassExtRec
     _XmCascadeBPrimClassExtRec = { 0 };
externaldef(xmtearoffbprimclassextrec) XmPrimitiveClassExtRec
     _XmTearOffBPrimClassExtRec = { 0 };




/* From old Visual.c */

void
_XmPrimitiveTopShadowPixmapDefault(
        Widget widget,
        int offset,
        XrmValue *value )
{
    _XmTopShadowPixmapDefault(widget, offset, value );
}

void
_XmManagerTopShadowPixmapDefault(
        Widget widget,
        int offset,
        XrmValue *value )
{
    _XmTopShadowPixmapDefault(widget, offset, value );
}

void
_XmPrimitiveHighlightPixmapDefault(
        Widget widget,
        int offset,
        XrmValue *value )
{
    _XmHighlightPixmapDefault(widget, offset, value );
}

void
_XmManagerHighlightPixmapDefault(
        Widget widget,
        int offset,
        XrmValue *value )
{
    _XmHighlightPixmapDefault(widget, offset, value );
}



/************************************************************************
 *
 *  _XmFilterResources
 *
 ************************************************************************/
Cardinal
_XmFilterResources(
        XtResource *resources,
        Cardinal numResources,
        WidgetClass filterClass,
        XtResource **filteredResourcesRtn )
{
    XtResource		*filteredResources;
    Cardinal		copyIndexes[256];
    Cardinal		filterOffset;
    Cardinal		i, j;

    filterOffset = filterClass->core_class.widget_size;

    for (i = 0, j = 0; i < numResources; i++)
      {
	  if (resources[i].resource_offset >= filterOffset)
	    {
		copyIndexes[j++] = i;
	    }
      }

    filteredResources = (XtResource *) XtMalloc(j * sizeof(XtResource));

    for (i = 0; i < j; i++)
      {
	  filteredResources[i] = resources[copyIndexes[i]];
      }
    *filteredResourcesRtn = filteredResources;
    return j;
}



/************************************************************************
 *
 *  _XmRootGeometryManager
 *
 ************************************************************************/
/*ARGSUSED*/
XtGeometryResult
_XmRootGeometryManager(
	 Widget w,
	 XtWidgetGeometry *request,
	 XtWidgetGeometry *reply )
{
    XmWidgetExtData	extData = _XmGetWidgetExtData(w, XmSHELL_EXTENSION);
    XmShellExtObject	se = (XmShellExtObject)extData->widget;
    XtGeometryHandler	wmGeoHandler;
    ShellWidgetClass	swc = (ShellWidgetClass)wmShellWidgetClass;
    ShellClassExtensionRec **scExtPtr;
    XtGeometryResult	returnVal = XtGeometryNo;
    WMShellWidget	wmShell = (WMShellWidget)w;

    if(extData == NULL)
    {
#ifdef DEBUG
        XmeWarning(NULL, "_XmGetWidgetExtData() returned NULL pointer.");
#endif
        return XtGeometryNo;
    }

    if (se)
      {
	   se->shell.lastConfigureRequest = NextRequest(XtDisplay(w));
      }
#ifdef DEBUG
    else
      XtError("no extension object");
#endif /* DEBUG */

    scExtPtr = (ShellClassExtensionRec **)
      _XmGetClassExtensionPtr( (XmGenericClassExt *) &(swc->shell_class.extension),
			       NULLQUARK);
    if (request->request_mode & XtCWQueryOnly)
      {
	   if (!(wmShell->shell.allow_shell_resize) &&
	       (request->request_mode &
		(CWWidth | CWHeight | CWBorderWidth)))
	     return XtGeometryNo;
	   /*
	    * we should switch on useAsyncGeometry but we won't |||
	    */
	   else
	     return XtGeometryYes;
      }

    if (se->shell.useAsyncGeometry)
      {
	   /* make wait_for_wm = FALSE to force desired behaviour */
	   wmShell->wm.wait_for_wm = FALSE;
	   /* FIX for 1684: remove the timeout = 0 line, not
	      needed and introduced a bug if not saved/restore together
	      with useAsync change - wait_for_wm will be later reset
	      by Shell in Xt */
      }
    if ((wmGeoHandler = (*scExtPtr)->root_geometry_manager) != NULL)
      {
	   returnVal =  (*wmGeoHandler)(w, request, reply);
	   if (se->shell.useAsyncGeometry) {
	       /* X configure was sent to the server, while this is happening,
		  let's everybody think it's a success (which is true
		  most of the time): set the shell size to what it wants
		  to be and return Yes */
	       if (request->request_mode & CWWidth)
		   w->core.width = request->width;
	       if (request->request_mode & CWHeight)
		   w->core.height = request->height ;
	       if (request->request_mode & CWBorderWidth)
		   w->core.border_width = request->border_width ;
	       if (request->request_mode & CWX)
		   w->core.x = request->x;
	       if (request->request_mode & CWY)
		   w->core.y = request->y;

	       returnVal = XtGeometryYes;
	   }
      }
    return returnVal;
}


/*ARGSUSED*/
void
_XmVendorExtRealize(
	 Widget w,
	 XtPointer closure,	/* unused */
	 XtPointer call_data )	/* unused */
{
    XtWarning("Don't use _XmVendorExtRealize! It does nothing.");
}



/**********************
  Stuff from Desktop.c
**********************/

static XContext	actualClassContext = 0;


/*ARGSUSED*/
static void
DisplayDestroyCallback
	( Widget w,
        XtPointer client_data,
        XtPointer call_data )	/* unused */
{
    XDeleteContext(XtDisplay(w), (XID) client_data, actualClassContext);
}


/************************************************************************
 *
 *  _XmGetActualClass
 *
 ************************************************************************/
/* ARGSUSED */
WidgetClass
_XmGetActualClass(
	Display     *display,
        WidgetClass w_class )
{
	  WidgetClass		actualClass;

	  if (!actualClassContext)
	    actualClassContext = XUniqueContext();

	  /*
	   * see if a non-default class has been specified for the
	   * class
	   */
	  if (XFindContext(display,
			   (Window) w_class,
			   actualClassContext,
			   (char **) &actualClass))
	    {
		return w_class;
	    }
	  else
	    return actualClass;
}

/************************************************************************
 *
 *  _XmSetActualClass
 *
 ************************************************************************/
/* ARGSUSED */
void
_XmSetActualClass(
	Display     *display,
        WidgetClass w_class,
        WidgetClass actualClass )
{
    XmDisplay   dd = (XmDisplay) XmGetXmDisplay(display);
    WidgetClass previous;
    WidgetClass oldActualClass;

    if (!actualClassContext)
      actualClassContext = XUniqueContext();

    /*
     * see if a non-default class has been specified for the
     * class
     */
    previous = _XmGetActualClass(display, w_class);
    XtRemoveCallback((Widget)dd, XtNdestroyCallback,
			DisplayDestroyCallback, (XtPointer) previous);


    /*
     * Save class data.
     * Delete old context if one exists.
     */
    if (XFindContext (display, (Window) w_class, actualClassContext,
			(char **)&oldActualClass)) {
	XSaveContext(display, (Window) w_class, actualClassContext,
			(char *) actualClass);
    }
    else if (oldActualClass != actualClass) {
	XDeleteContext (display, (Window) w_class, actualClassContext);
	XSaveContext(display, (Window) w_class, actualClassContext,
			(char *) actualClass);

    }

    XtAddCallback((Widget)dd, XtNdestroyCallback,
			DisplayDestroyCallback, (XtPointer) w_class);
}



/************************************************************************
 *
 *  _XmGetWorldObject
 *
 ************************************************************************/
/* ARGSUSED */
XmDesktopObject
_XmGetWorldObject(
        Widget shell,
        ArgList args,
        Cardinal *num_args )
{
    XmDesktopObject	worldObject;
    static XContext	worldObjectContext = 0;
    XmWidgetExtData     ext;
    Display		*display;

    /*
    ** Note: in an ideal World we would be sure to delete this context when
    ** the display is closed, so that we don't get bad data if a second
    ** display with the same id is opened.
    */
    if (!worldObjectContext)
      worldObjectContext = XUniqueContext();

    display = XtDisplayOfObject(shell);

    if (XFindContext(display,
		     (Window) NULL,
		     worldObjectContext,
		     (char **) &worldObject))
      {
	  WidgetClass		worldClass;
	  Widget		appShell = shell;

	  worldClass = _XmGetActualClass(display, xmDesktopClass);

	  while (XtParent(appShell))
	    appShell = XtParent(appShell);

	  worldObject = (XmDesktopObject)
	    XtCreateWidget("world",
			   worldClass,
			   appShell,
			   args,
			   num_args ? *num_args: 0);

          ext = _XmGetWidgetExtData(worldObject->ext.logicalParent,
                                    worldObject->ext.extensionType);

          if(ext != NULL)
          {
            _XmExtObjFree((XtPointer) ext->reqWidget);
            ext->reqWidget = NULL;
          }
#ifdef DEBUG
          else
          {
            XmeWarning(NULL, "_XmGetWidgetExtData() returned NULL pointer.");
          }
#endif

	  XSaveContext(display,
		       (Window) NULL,
		       worldObjectContext,
		       (char *) worldObject);
      }
    return
      worldObject;
}

/************************************************************************
 *
 *  _XmVirtKeysHandler
 *
 ************************************************************************/

/*ARGSUSED*/
void
_XmVirtKeysHandler(Widget    widget,
		   XtPointer client_data,
		   XEvent   *event,
		   Boolean  *dontSwallow )
{
  if (widget->core.being_destroyed)
    {
      *dontSwallow = False;
      return;
    }
}

/****************************************************************
 *
 *  TextOut.c functions
 *
 ****************************************************************/

void _XmTextDrawDestination(XmTextWidget tw)
{
}

void _XmTextClearDestination(XmTextWidget tw, Boolean ignore_sens)
{
}

void _XmTextDestinationVisible(Widget w, Boolean turn_on)
{
}

/****************************************************************
 *
 *  TextStrSo.c functions
 *
 ****************************************************************/

Boolean
_XmStringSourceFindString(Widget w,
			  XmTextPosition start,
			  char* string,
			  XmTextPosition *position)
{
  return(XmTextFindString(w, start, string, XmTEXT_FORWARD, position));
}

/****************************************************************
 *
 *  TextIn.c functions
 *
 ****************************************************************/

XmTextPosition
_XmTextGetAnchor(XmTextWidget tw)
{
  InputData data = tw->text.input->data;

  return(data->anchor);
}

/****************************************************************
 *
 *  Traversal.c functions
 *
 ****************************************************************/

/*ARGSUSED*/
Boolean
_XmGrabTheFocus(
        Widget w,
        XEvent *event )		/* unused */
{
  /* Function used by TextEdit widgets to grab the focus.
   */
  return _XmMgrTraversal( w, XmTRAVERSE_CURRENT) ;
}


void
_XmProcessTraversal(
        Widget w,
        XmTraversalDirection dir,
        Boolean check )		/* unused */
{
  _XmMgrTraversal( w, dir ) ;
}


Widget
_XmFindNextTabGroup(
        Widget wid )
{
  return _XmNavigate( wid, XmTRAVERSE_NEXT_TAB_GROUP) ;
}


Widget
_XmFindPrevTabGroup(
        Widget wid )
{
  return _XmNavigate( wid, XmTRAVERSE_PREV_TAB_GROUP) ;
}


Boolean
_XmCreateVisibilityRect(Widget w,
			XRectangle *rectPtr)
{
  return _XmComputeVisibilityRect(w, rectPtr, FALSE, TRUE);
}

/****************************************************************
 *
 *  MenuShell.c functions
 *
 ****************************************************************/
void
_XmSetLastManagedMenuTime (
	Widget wid,
	Time newTime )
{
   _XmGetMenuState((Widget)wid)->MS_LastManagedMenuTime = newTime;
}


/****************************************************************
 *
 *  XmString.c functions
 *
 ****************************************************************/

/*
 * count the number of lines in an _XmString.
 */
int
_XmStringLineCount(
        _XmString string )
{
  return(XmStringLineCount(string));
}

/* Used to create internal XmString, now just copies. */
_XmString
_XmStringCreate(
        XmString cs )
{
  return((_XmString)XmStringCopy(cs));
}

/* Used to create external XmString from internal, now just copies. */
/*ARGSUSED*/
XmString
_XmStringCreateExternal(
        XmRenderTable rendertable, /* unused */
        _XmString cs )
{
  return(XmStringCopy((XmString)cs));
}

Boolean
_XmStringEmpty(
        _XmString string )
{
  return(XmStringEmpty(string));
}

void
_XmStringFree(
        _XmString string )
{
  XmStringFree(string);
}

/*
 * find total height of XmString
 */
Dimension
_XmStringHeight(
        XmRenderTable rendertable,
        _XmString string )
{
  if (!string || !rendertable) return (0);

  return(XmStringHeight(rendertable, string));
}

/*
 * find the rectangle which will enclose the text
 */
void
_XmStringExtent(
        XmRenderTable rendertable,
        _XmString string,
        Dimension *width,
        Dimension *height )
{
  if (!string || !rendertable)
    {
      *width = 0;
      *height = 0;
      return;
    }

  XmStringExtent(rendertable, string, width, height);
}


/*
 * find width of widest line in XmString
 */
Dimension
_XmStringWidth(
        XmRenderTable rendertable,
        _XmString string )
{
  if (!string || !rendertable) return (0);

  return(XmStringWidth(rendertable, string));
}

void
_XmStringDraw(
        Display *d,
        Window w,
        XmRenderTable rendertable,
        _XmString string,
        GC gc,
        Position x,
        Position y,
        Dimension width,
        unsigned char align,
        unsigned char lay_dir,
        XRectangle *clip )
{
    XmStringDraw(d, w, rendertable, string, gc, x, y, width,
		 align, lay_dir, clip);
}

void
_XmStringDrawImage(
        Display *d,
        Window w,
        XmRenderTable rendertable,
        _XmString string,
        GC gc,
        Position x,
        Position y,
        Dimension width,
        unsigned char align,
        unsigned char lay_dir,
        XRectangle *clip )
{
    XmStringDrawImage(d, w, rendertable, string, gc, x, y, width,
		      align, lay_dir, clip);
}

void
_XmStringDrawUnderline(
        Display *d,
        Window w,
        XmRenderTable f,
        _XmString s,
        GC gc,
        Position x,
        Position y,
        Dimension width,
        unsigned char align,
        unsigned char lay_dir,
        XRectangle *clip,
        _XmString u )
{
    XmStringDrawUnderline(d, w, f, s, gc, x, y, width,
			  align, lay_dir, clip, u);
}

void
_XmStringDrawMnemonic(
        Display *d,
        Window w,
        XmRenderTable rendertable,
        _XmString string,
        GC gc,
        Position x,
        Position y,
        Dimension width,
        unsigned char align,
        unsigned char lay_dir,
        XRectangle *clip,
        String mnemonic,
        XmStringTag tag )
{
    XmString underline;

    underline = XmStringCreate(mnemonic, tag);

    XmStringDrawUnderline(d, w, rendertable, string, gc, x, y, width,
			  align, lay_dir, clip, underline);
    XmStringFree(underline);
}

/*
 * internal structure access routines
 */
Boolean
_XmStringInitContext(
        _XmStringContext *context,
        _XmString string )
{
  return(XmStringInitContext(context, string));
}

void
_XmStringFreeContext(
        _XmStringContext context )
{
  XmStringFreeContext(context);
}

_XmString
_XmStringCopy( _XmString string )
{
  if (string == (_XmString) NULL) return ((_XmString) NULL);
  else return((_XmString)XmStringCopy(string));
}

/*
 * check these two internals
 */
Boolean
_XmStringByteCompare(
        _XmString a,
        _XmString b )
{
  if (!a && !b) return(TRUE);
  if (!a || !b) return(FALSE);

  return(XmStringByteCompare(a, b));
}

Dimension
_XmStringBaseline(
        XmRenderTable rendertable,
        _XmString string )
{
  if (!string || !rendertable) return (0);

  return(XmStringBaseline(rendertable, string));
}

Boolean
_XmStringHasSubstring(
        _XmString string,
        _XmString substring )
{
  if ((string == NULL) || (substring == NULL) || (XmStringEmpty(substring)))
    return (FALSE);

  return(XmStringHasSubstring(string, substring));
}




/* Replaced by XmTextGetInsertionPosition */

XmTextPosition
XmTextGetCursorPosition(Widget widget)
{
  if (XmIsTextField(widget))
    return(XmTextFieldGetCursorPosition(widget));
  else {
    return(XmTextGetInsertionPosition(widget));
  }
}


/* Replaced by XmTextSetInsertionPosition */

void
XmTextSetCursorPosition(Widget widget,
			XmTextPosition position)
{
  if (XmIsTextField(widget))
    XmTextFieldSetCursorPosition(widget, position);
  else {
    XmTextSetInsertionPosition(widget, position);
  }
}

/* Replaced by XmTextFieldSetInsertionPosition */

void
XmTextFieldSetCursorPosition(Widget w,
			     XmTextPosition position)
{
  XmTextFieldSetInsertionPosition(w, position);
}

/* Replaced by XmTextFieldGetInsertionPosition */

XmTextPosition
XmTextFieldGetCursorPosition(Widget w)
{
  return XmTextFieldGetInsertionPosition(w);
}


/****************************************************************
 *
 *  TravAct.c functions
 *
 ****************************************************************/


/*
 * Get the state of the 'ResettingFocus' flag, based upon the
 * display to which the widget is tied.
 */
Boolean
_XmGetFocusResetFlag(
        Widget w )
{
   return( (Boolean) _XmGetFocusFlag(w, XmFOCUS_RESET) );
}


/*
 * Set the state of the 'ResettingFocus' flag.
 */
void
_XmSetFocusResetFlag(
        Widget w,
        Boolean value )
{
   _XmSetFocusFlag(w, XmFOCUS_RESET, value);
}

/********************************************************************/

void
_XmStringUpdate(XmFontList fontlist, /* unused */
		_XmString string ) /* unused */
{
}

/* From BaseClass.c */

void _XmFreeWidgetExtData(Widget widget)		/* unused */
{
}


void _XmBaseClassPartInitialize(WidgetClass wc)	/* unused */
{
}

Boolean _XmIsSlowSubclass(WidgetClass wc, unsigned int bit)
{
  XmBaseClassExt *wcePtr = _XmGetBaseClassExtPtr(wc, XmQmotif);

  if (!wcePtr || !(*wcePtr))
    return False;

  if ((_XmGetFlagsBit((*wcePtr)->flags, bit)) != 0)
    return True;
  else
    return False;
}


Boolean
_XmIsStandardMotifWidgetClass(
        WidgetClass wc)
{
  /* This routine depends on ALL standard Motif classes use fast subclassing. */
  XmBaseClassExt * fastPtr;
  XmBaseClassExt * superFastPtr;
  WidgetClass super_wc = wc->core_class.superclass;

  if ((fastPtr = _XmGetBaseClassExtPtr( wc, XmQmotif)) && *fastPtr)
    {
      if (!(superFastPtr = _XmGetBaseClassExtPtr( super_wc, XmQmotif)))
        {
          /* This catches all Motif classes which are direct subclasses
           * of an Xt Intrinsics widget class.
           */
          return TRUE;
        }
      if (*superFastPtr)
        {
          unsigned char * flags = (*fastPtr)->flags;
          unsigned char * superFlags = (*superFastPtr)->flags;
          unsigned numBytes = (XmLAST_FAST_SUBCLASS_BIT >> 3) + 1;
          unsigned Index;

          Index = numBytes;
          while (Index--)
            {
              if (flags[Index] != superFlags[Index])
                {
                  /* Since all Motif classes use fast subclassing, the fast
                   * subclassing bits of any standard Motif class will be
                   * different than those of its superclass (the superclass
                   * will have one less bit set).  Any non-standard Motif
                   * subclass will have the same fast subclass bits as its
                   * superclass.
                   */
                  return TRUE;
                }
            }
        }
    }
  return FALSE;
}

/** Obsolete from ImageCache.c */

Pixmap
_XmGetPixmap(
    Screen *screen,
    char *image_name,
    int depth,
    Pixel foreground,
    Pixel background)
{
    return(XmGetPixmapByDepth(screen, image_name,
			      foreground, background, depth));
}

Boolean
_XmInstallPixmap(
        Pixmap pixmap,
        Screen *screen,
        char *image_name,
        Pixel foreground,
        Pixel background )
{
  return _XmCachePixmap(pixmap, screen, image_name,
			foreground, background, 0, 0, 0);
}

