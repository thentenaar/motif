/* $TOG: ColorObj.c /main/19 1999/05/07 11:36:18 samborn $ */
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

#include "XmI.h"
#include "ColorObjI.h"
#include <Xm/VendorSEP.h>
#include "CallbackI.h"
#include "ColorI.h"
#include "MessagesI.h"
#include <X11/Xlibint.h>

#if defined(__cplusplus) || defined(c_plusplus)
#define  OBJ_CLASS(w)   (((ApplicationShellWidget)(w))->application.c_class)
#else
#define  OBJ_CLASS(w)   (((ApplicationShellWidget)(w))->application.class)
#endif

#define WARNING1	_XmMMsgColObj_0001
#define WARNING2	_XmMMsgColObj_0002

static int trap_XCloseDisplay(Display *disp, XExtCodes *codes);

/** default should not be killed unless application is dying **/
externaldef (colorobj) XmColorObj _XmDefaultColorObj = NULL;
externaldef (colorobj) XContext _XmColorObjCache = 0;
externaldef (colorobj) Display  *_XmColorObjCacheDisplay = NULL;


/********    Static Function Declarations    ********/

static void Destroy(
                        Widget wid) ;
static void DisplayDestroy(
                        Widget wid,
			XtPointer clientData,
			XtPointer callData) ;
static void Initialize(
                        Widget rq,
                        Widget nw,
                        ArgList Args,
                        Cardinal *numArgs) ;
static void GetSelection(
                        Widget w,
                        XtPointer client_data,
                        Atom *selection,
                        Atom *type,
                        XtPointer val,
                        unsigned long *length,
                        int *format) ;
static void UpdatePixelSet(
                        XmPixelSet *toSet,
                        XmPixelSet *fromSet) ;
static void UpdateXrm(
                        Colors colors,
                        int screen,
                        XmColorObj tmpColorObj);

static void FetchPixelData(Widget w, char * value, int screen);

static Boolean ColorCachePropertyExists(
			Display *dpy,
			Window SelOwner,
			Widget w,
			int screen) ;

/********    End Static Function Declarations    ********/

#define UNSPECIFIED_USE_MULTI_COLOR_ICONS 2

static XtResource resources[] = {
    {
	XmNprimaryColorSetId,
	XmCPrimaryColorSetId,
	XmRInt,
	sizeof(int),
	XtOffset (XmColorObj, color_obj.primary),
	XmRImmediate,
	(XtPointer) 5,
    },
    {
	XmNsecondaryColorSetId,
	XmCSecondaryColorSetId,
	XmRInt,
	sizeof(int),
	XtOffset (XmColorObj, color_obj.secondary),
	XmRImmediate,
	(XtPointer) 6,
    },
    {
	XmNtextColorSetId,
	XmCTextColorSetId,
	XmRInt,
	sizeof(int),
	XtOffset (XmColorObj, color_obj.text),
	XmRImmediate,
	(XtPointer) 4,
    },
    {
	XmNuseTextColor,
	XmCUseTextColor,
	XmRBoolean,
	sizeof(Boolean),
	XtOffset (XmColorObj, color_obj.useText),
	XmRImmediate,
	(XtPointer) True,
    },
    {
	XmNuseTextColorForList,
	XmCUseTextColorForList,
	XmRBoolean,
	sizeof(Boolean),
	XtOffset (XmColorObj, color_obj.useTextForList),
	XmRImmediate,
	(XtPointer) True,
    },
    {
	XmNactiveColorSetId,
	XmCActiveColorSetId,
	XmRInt,
	sizeof(int),
	XtOffset (XmColorObj, color_obj.active),
	XmRImmediate,
	(XtPointer) 1,
    },
    {
	XmNinactiveColorSetId,
	XmCInactiveColorSetId,
	XmRInt,
	sizeof(int),
	XtOffset (XmColorObj, color_obj.inactive),
	XmRImmediate,
	(XtPointer) 2,
    },
    {
	XmNuseColorObj,
	XmCUseColorObj,
	XmRBoolean,
	sizeof(Boolean),
	XtOffset (XmColorObj, color_obj.useColorObj),
	XmRImmediate,
	(XtPointer) True,
    },
    {
	XmNuseMask,
	XmCUseMask,
	XmRBoolean,
	sizeof(Boolean),
	XtOffset (XmColorObj, color_obj.useMask),
	XmRImmediate,
	(XtPointer) True,
    },
    {
	XmNuseMultiColorIcons,
	XmCUseMultiColorIcons,
	XmRBoolean,
	sizeof(Boolean),
	XtOffset (XmColorObj, color_obj.useMultiColorIcons),
	XmRImmediate,
	(XtPointer) UNSPECIFIED_USE_MULTI_COLOR_ICONS,
    },
    {
	XmNuseIconFileCache,
	XmCUseIconFileCache,
	XmRBoolean,
	sizeof(Boolean),
	XtOffset (XmColorObj, color_obj.useIconFileCache),
	XmRImmediate,
	(XtPointer) True,
    },
};


externaldef(xmcolorobjclassrec)
XmColorObjClassRec xmColorObjClassRec =
{
    {
	/*
	 * make it a topLevelShell subclass in order to avoid
	 * baseClass recursion.  This is due to the posthook logic and
	 * the nested created of ColorObj inside of the first appShell
	 */
        (WidgetClass)&wmShellClassRec,    /* superclass       */
        "XmColorObj",                     /* class_name            */
        sizeof(XmColorObjRec),            /* widget_size           */
        NULL,                             /* class_initialize      */
        NULL,                             /* class_part_initialize */
        FALSE,                            /* class_inited          */
        Initialize,                       /* initialize            */
        NULL,                             /* initialize_hook       */
        XtInheritRealize,                 /* realize               */
        NULL,                             /* actions               */
        0,                                /* num_actions           */
        resources,                        /* resources             */
        XtNumber(resources),              /* num_resources         */
        NULLQUARK,                        /* xrm_class             */
        FALSE,                            /* compress_motion       */
        FALSE,                            /* compress_exposure     */
        FALSE,                            /* compress_enterleave   */
        FALSE,                            /* visible_interest      */
        Destroy,                          /* destroy               */
        NULL,                             /* resize                */
        NULL,                             /* expose                */
        NULL,                             /* set_values            */
        NULL,                             /* set_values_hook       */
        NULL,                             /* set_values_almost     */
        NULL,                             /* get_values_hook       */
        NULL,                             /* accept_focus          */
        XtVersion,                        /* version               */
        NULL,                             /* callback_offsets      */
        NULL,                             /* tm_table              */
        NULL,                             /* query_geometry        */
        NULL,                             /* display_accelerator   */
        NULL                              /* extension             */
    },
    { 					/* composite class record */
	NULL,		                /* geometry_manager 	*/
	NULL,	 			/* change_managed	*/
	XtInheritInsertChild,		/* insert_child		*/
	XtInheritDeleteChild, 		/* from the shell 	*/
	NULL, 				/* extension record     */
    },
    { 					/* shell class record 	*/
	NULL, 				/* extension record     */
    },
    { 					/* wm shell class record */
	NULL, 				/* extension record     */
    },
    {					/* colorObj class	*/
	NULL,				/* extension		*/
    },
};

externaldef(xmcolorobjclass) WidgetClass
    xmColorObjClass = (WidgetClass)&xmColorObjClassRec;



/**********************************************************************/
/** _XmColorObjCreate() - initialize_hook() from Display object...   **/
/**         Used to create a ColorObj.  Updated to support one per   **/
/**         display.  There will be a "default" display and ColorObj **/
/**         for the client, and each new Display Object will have    **/
/**         a new ColorObj associated with it.  This allows for      **/
/**         things like the dialog server (which hangs around and    **/
/**         runs clients as if they were seperate applications) to   **/
/**         utilize seperate resource databases for "pseudo-apps".   **/
/**                                                                  **/
/**********************************************************************/
void
_XmColorObjCreate(
        Widget w,
        ArgList al,
        Cardinal *acPtr )
{
    String	name, obj_class;

    /** don't create if in initialization of the color server **/
    /** that's for dtsession itself being a motif app */
    if (XtIsApplicationShell(w))
            if ( strcmp(OBJ_CLASS(w), XmSCOLOR_SRV_NAME) == 0 ) {
		return;
	    }

    /** this is really gross but it makes the resources work right **/
    XtGetApplicationNameAndClass(XtDisplay(w), &name, &obj_class);
    _XmProcessLock();
    xmColorObjClass->core_class.class_name = obj_class;
    _XmProcessUnlock();
    XtAppCreateShell(name, obj_class, xmColorObjClass, XtDisplay(w), NULL, 0);

    /** set up destroy callback on display object for this ColorObj **/
    XtAddCallback(w, XmNdestroyCallback, DisplayDestroy, NULL);
}


/**********************************************************************/
/** DisplayDestroy()                                                 **/
/**        Display object is being destroyed... destroy associated   **/
/**        colorObj if there is one.                                 **/
/**                                                                  **/
/**********************************************************************/
static void
DisplayDestroy( Widget wid, XtPointer clientData, XtPointer callData )
{
    XmColorObj tmpColorObj=NULL;
    XContext context;

    _XmProcessLock();
    context = _XmColorObjCache;
    _XmProcessUnlock();

    if (_XmColorObjCacheDisplay)
    if ( XFindContext(_XmColorObjCacheDisplay, (XID)XtDisplay(wid), context,
	 (XPointer *)&tmpColorObj) == 0)
    {
	if (tmpColorObj)
	{
 	    XtDestroyWidget((Widget)tmpColorObj);
	}
    }
}


/**********************************************************************/
/** Destroy()                                                        **/
/**        Free the data allocated for this ColorObj                 **/
/**                                                                  **/
/**********************************************************************/
static void
Destroy( Widget wid )
{
    XmColorObj tmpColorObj = (XmColorObj)wid;
    XContext context;

    _XmProcessLock();
    context = _XmColorObjCache;
    _XmProcessUnlock();

    if (tmpColorObj->color_obj.colors)
       XtFree ((char *) tmpColorObj->color_obj.colors);
    if (tmpColorObj->color_obj.atoms)
       XtFree ((char *) tmpColorObj->color_obj.atoms);
    if (tmpColorObj->color_obj.colorUse)
       XtFree ((char *) tmpColorObj->color_obj.colorUse);

    if (_XmColorObjCacheDisplay)
	XDeleteContext(_XmColorObjCacheDisplay,
	               (XID)tmpColorObj->color_obj.display,
	               context);

     _XmProcessLock();

    /* we're destory the "default" color obj, which shouldn't
       be use anyway, set it to null. A better solution would be
       to look for a new default, but XmeGetPixelData is obsolete
       API anyway */
    if (tmpColorObj == _XmDefaultColorObj) {
	_XmDefaultColorObj = NULL;

	_XmColorObjCacheDisplay = NULL;
    }

    _XmProcessUnlock();
}


/**********************************************************************/
/** Initialize()                                                     **/
/**                                                                  **/
/**********************************************************************/
static void
Initialize(
        Widget rq,
        Widget nw,
        ArgList Args,
        Cardinal *numArgs)
{
    XmColorObj new_obj = (XmColorObj) nw ;
    int      i, nscreens;
    Boolean  init_first_time = False;
    Atom     tmpAtom = 0;
    unsigned long savetimeout = (unsigned long)-1 ;
    /* window id of the selection owner */
    Window SelectionOwner ;
    int result, isNotNews ;
    XExtCodes *xExt;

    (void)rq;
    (void)Args;
    (void)numArgs;
    /* Ideally, we'd like check if we have a visual (like TrueColor)
       or colormap (non default) that would invalidate the use of
       shared pixels.. but since the color obj is a wmShell with no
       parent, global to a display, there's no way to retrieve the app
       shell or the widget that could be using wrongly these pixels.
       The app with a TrueColor visual of a custom colormap must set
       useColorObj False in its resource file, or its GUI will go
       technicolor (it can also be specific about the pixel it uses)
    */

    new_obj->color_obj.colorIsRunning = False;
    new_obj->color_obj.colors = NULL;
    new_obj->color_obj.atoms = NULL;
    new_obj->color_obj.colorUse = NULL;

    new_obj->color_obj.display = XtDisplay(new_obj);
    new_obj->color_obj.numScreens = nscreens =
      ScreenCount(new_obj->color_obj.display);

    /** initialize default colorObj and context if needed **/
    _XmProcessLock();
    if (!_XmColorObjCache) _XmColorObjCache = XUniqueContext();

    if (_XmColorObjCacheDisplay == NULL) {
        _XmColorObjCacheDisplay = new_obj->color_obj.display;
        xExt = XAddExtension(_XmColorObjCacheDisplay);
        XESetCloseDisplay(_XmColorObjCacheDisplay, xExt->extension, trap_XCloseDisplay);
    }

    if (_XmDefaultColorObj == NULL)
        _XmDefaultColorObj = new_obj;

    /** add new colorObj to the cache **/
    XSaveContext(_XmColorObjCacheDisplay, (XID)new_obj->color_obj.display,
		 _XmColorObjCache, (XPointer)new_obj);
    _XmProcessUnlock();

    /** if useColorObj = False, don't initialize or allocate color data **/
    if (new_obj->color_obj.useColorObj)
    {

        /** get screen info and allocate space for colors per screen **/
	new_obj->color_obj.colors =
	  (Colors *)XtCalloc(nscreens, sizeof(Colors));
	new_obj->color_obj.atoms = (Atom *)XtCalloc(nscreens, sizeof(Atom));
	new_obj->color_obj.colorUse = (int *)XtCalloc(nscreens, sizeof(int));

	if ( !new_obj->color_obj.colors || !new_obj->color_obj.atoms ||
	    !new_obj->color_obj.colorUse )
	{
	      XmeWarning(nw, WARNING1);
	      /* couldn't allocate memory */
	      new_obj->color_obj.colorIsRunning = False;
	      return;
	}

	/** set screen and color info for this application **/

	new_obj->color_obj.myScreen =
	  XScreenNumberOfScreen(XtScreen(new_obj));
	new_obj->color_obj.myColors =
	  new_obj->color_obj.colors[new_obj->color_obj.myScreen];


	/* check valid value, then -1 from colors, to index arrays */

	if (new_obj->color_obj.primary < 1 ||
	    new_obj->color_obj.primary > XmCO_NUM_COLORS)
	  new_obj->color_obj.primary = 1;
	if (new_obj->color_obj.secondary < 1 ||
	    new_obj->color_obj.secondary > XmCO_NUM_COLORS)
	  new_obj->color_obj.secondary = 1;
	if (new_obj->color_obj.active < 1 ||
	    new_obj->color_obj.active > XmCO_NUM_COLORS)
	  new_obj->color_obj.active = 1;
	if (new_obj->color_obj.inactive < 1 ||
	    new_obj->color_obj.inactive > XmCO_NUM_COLORS)
	  new_obj->color_obj.inactive = 1;
	if (new_obj->color_obj.text < 1 ||
	    new_obj->color_obj.text > XmCO_NUM_COLORS)
	  new_obj->color_obj.text = 1;

	new_obj->color_obj.primary   = new_obj->color_obj.primary   - 1;
	new_obj->color_obj.secondary = new_obj->color_obj.secondary - 1;
	new_obj->color_obj.active    = new_obj->color_obj.active    - 1;
	new_obj->color_obj.inactive  = new_obj->color_obj.inactive  - 1;
	new_obj->color_obj.text      = new_obj->color_obj.text  - 1;

	/* we're going to realize it */
	new_obj->core.mapped_when_managed = False;
	new_obj->core.width = 1;
	new_obj->core.height = 1;

	/*****
	 * Since dtsession's idea of the PIXEL_SET is pretty much constant
	 * it makes more sense to have a permanent property in the
	 * server that stored the information.  This means that the
	 * starting application can get the PIXEL_SET directly from
	 * the server without ever waking up dtsession.
	 *
	 * Obviously you have to consider the case where you aren't
	 * talking to a dtsession that knows to put the PIXEL_SET
	 * in the server, so everything falls back to the old way
	 * on failure. */

	/* a bug in this server makes us do that - pretty lame... */
	isNotNews = strcmp(ServerVendor(XtDisplay(nw)),
			   "X11/NeWS - Sun Microsystems Inc.");

	/* Intern all the atoms in a single round-trip. */
	if (nscreens > 0)
	  {
#define MAX_SCREENS		10
#define MAX_ATOM_LENGTH		24
	    char buf[MAX_SCREENS * MAX_ATOM_LENGTH];
	    char *name_buf[MAX_SCREENS];
	    char **names;

	    names = (char**) XmStackAlloc(nscreens * sizeof(char *), name_buf);
	    for (i = 0; i < nscreens; i++) {
	      names[i] = ((nscreens <= MAX_SCREENS) ?
			  &buf[i * MAX_ATOM_LENGTH] :
			  XtMalloc(MAX_ATOM_LENGTH));
	      sprintf(names[i], "%s%d", XmSCUSTOMIZE_DATA, i);
	    }

	    XInternAtoms(new_obj->color_obj.display, names,
			 nscreens, FALSE, new_obj->color_obj.atoms);

	    if (nscreens > MAX_SCREENS)
	      for (i = 0; i < nscreens; i++)
		XtFree(names[i]);
	    XmStackFree((char*) names, name_buf);
#undef MAX_SCREENS
#undef MAX_ATOM_LENGTH
	  }

	for (i = 0; i < nscreens; i++)
	{
	      /* if noone has created this one, no need to continue */
	      if (new_obj->color_obj.atoms[i] == None) break ;

	      /* Get XID of selection owner: dtsession most probably */
	      SelectionOwner = XGetSelectionOwner(XtDisplay(nw),
						  new_obj->color_obj.atoms[i]) ;

	      if (isNotNews || SelectionOwner) {

		  /* check if color cache properties have been
		     hung as property on selection owner window.
		     If so, get them.
		     */
		  result = ColorCachePropertyExists(XtDisplay(nw),
						    SelectionOwner,
						    nw, i);
		  if ( ! result ) {
		      /*
		       * we have to fall back to the selection way.
		       *
		       * We need the window created in the server
		       * so that dtsession can attach the pixel set
		       * property to it for us.
		       */

		      /* certain thing we have to do only once for all screens */
		      if (init_first_time == False) {
			  init_first_time = True;

			  if(!XtIsRealized((Widget) new_obj))
			      XtRealizeWidget((Widget) new_obj);

			  tmpAtom = XInternAtom(new_obj->color_obj.display,
						XmSPIXEL_SET, True);
			  /* if noone has created this one, no need to go on */
			  if (tmpAtom == None) break ;

			  /* Remember the original timeout */
			  savetimeout =
			      XtAppGetSelectionTimeout(
					    XtWidgetToApplicationContext(nw));

			  /** set the selection timeout to 900 seconds **/
			  XtAppSetSelectionTimeout(
					       XtWidgetToApplicationContext(nw),
					       (unsigned long)900000);
		      }

		      new_obj->color_obj.done = FALSE;

		      XtGetSelectionValue((Widget) new_obj,
					  new_obj->color_obj.atoms[i],
					  tmpAtom, GetSelection,
					  (XtPointer) 1, CurrentTime);

		      /* wait for the reply : GetSelection will be called
		         where color_obj.done is set */
		      while(new_obj->color_obj.done == FALSE)
			  XtAppProcessEvent
			      (XtWidgetToApplicationContext((Widget) new_obj),
			       XtIMAll);

		      if (!new_obj->color_obj.colorIsRunning)
			  break; /* don't bother with rest of screen */
		  }
	      }
	  }

	if (savetimeout != -1) /* Restore the timeout if we had changed it */
	    XtAppSetSelectionTimeout(XtWidgetToApplicationContext(nw),
				     savetimeout);
    }


    if (new_obj->color_obj.useMultiColorIcons ==
	UNSPECIFIED_USE_MULTI_COLOR_ICONS)  {
	  if (new_obj->color_obj.colorUse) {
	      if (new_obj->color_obj.colorUse[0] == XmCO_HIGH_COLOR
		  || new_obj->color_obj.colorUse[0] == XmCO_MEDIUM_COLOR)
	      {
		  new_obj->color_obj.useMultiColorIcons = True;
	      }
	      else
		new_obj->color_obj.useMultiColorIcons = False;
	  }
	  else /* no color server ??? */
	      new_obj->color_obj.useMultiColorIcons = False;
    }
}

static void
FetchPixelData(Widget w, char * value, int screen)
{
    int      i, count, colorUse;
    char     tmp[256];
    XmColorObj tmpColorObj = (XmColorObj)w;
    Colors   colors;

    /* read color use */
    count = 0;
    sscanf (&(value[count]), "%x_", &colorUse);
    sprintf(tmp, "%x_", colorUse);
    count += strlen(tmp);
    tmpColorObj->color_obj.colorUse[screen] = colorUse;

    for (i = 0; i < XmCO_NUM_COLORS; i++) {
	/* read data into PixelSet */
	sscanf (&(value[count]), "%lx_%lx_%lx_%lx_%lx_", &(colors[i].bg),
		&(colors[i].fg), &(colors[i].ts),
		&(colors[i].bs), &(colors[i].sc));
	sprintf(tmp,"%lx_%lx_%lx_%lx_%lx_", colors[i].bg, colors[i].fg,
		colors[i].ts, colors[i].bs, colors[i].sc);
	count += strlen(tmp);
    }
    UpdateXrm (colors, screen, tmpColorObj);
    tmpColorObj->color_obj.colorIsRunning = True;
    XFree (value);
}

static Boolean
ColorCachePropertyExists(
	Display *dpy,
	Window SelOwner,
	Widget w,
	int screen)
{
    Atom pixel_set_atom ;
    unsigned long bytesafter, length;
    char *value = NULL ;
    int format = 0 ;
    Atom target;
    int result = False ;

    if (!SelOwner)
	return False ;

    /* try to get the property if it exist only */
    if ((pixel_set_atom = XInternAtom(dpy, XmSPIXEL_SET_PROP, TRUE)) == None)
	return False ;

    /* get the content of the property */
    result = XGetWindowProperty(dpy, SelOwner, pixel_set_atom, 0L, 1000000,
		    False, (Atom)AnyPropertyType, &target, &format,
		    &length, &bytesafter, (unsigned char **) &value);

   if ((result != Success) || (format == 0) || (target == None))
	return False ;

    if (value != NULL)  {
        if (value[length - 1] != XmPIXEL_SET_PROP_VERSION)
	   return False ;
	value[length - 1] = 0 ; /* extract version info */

        FetchPixelData(w, value, screen);
    }

    return True ;
}

/**********************************************************************/
/** GetSelection()                                                   **/
/**        colorIsRunning = False on entry, gets set to True if      **/
/**        color info successfully read in.                          **/
/**                                                                  **/
/**********************************************************************/
static void
GetSelection(
        Widget w,
        XtPointer client_data,
        Atom *selection,
        Atom *type,
        XtPointer val,
        unsigned long *length,
        int *format)
{
    XmColorObj tmpColorObj = (XmColorObj)w;
    char * value = (char*) val ;
    int  i, screen;

    (void)client_data;
    (void)type;
    (void)length;
    (void)format;
    tmpColorObj->color_obj.done = TRUE;

    /** get screen number **/
    screen = -1;
    for (i = 0; i < tmpColorObj->color_obj.numScreens; i++)
    {
        if (*selection == tmpColorObj->color_obj.atoms[i])
        {
            screen = i;
            break;
        }
    }
    if (screen == -1)
    {
        XmeWarning( w, WARNING2);   /* bad screen number */
        return;
    }

    if (value != NULL)  {
	FetchPixelData(w, value, screen);
    }
}



/**********************************************************************/
/** UpdatePixelSet()                                                 **/
/**       just a convenience routine                                 **/
/**********************************************************************/
static void
UpdatePixelSet(
        XmPixelSet *toSet,
        XmPixelSet *fromSet )
{
    toSet->bg = fromSet->bg;
    toSet->fg = fromSet->fg;
    toSet->ts = fromSet->ts;
    toSet->bs = fromSet->bs;
    toSet->sc = fromSet->sc;
}

/**********************************************************************/
/** UpdateColorCache()                                               **/
/**         so that widget created in this process gets the
            correct pixels                                           **/
/**********************************************************************/
static void
UpdateColorCache (Screen * screen,
		  Colormap colormap,
		  XmPixelSet * pset)
{
    XmColorData     cacheRec;

    /** add this color to the Motif color cache **/
    cacheRec.screen = screen;
    cacheRec.color_map = colormap;
    cacheRec.background.pixel = pset->bg;
    cacheRec.foreground.pixel = pset->fg;
    cacheRec.top_shadow.pixel = pset->ts;
    cacheRec.bottom_shadow.pixel = pset->bs;
    cacheRec.select.pixel = pset->sc;
    cacheRec.allocated = XmBACKGROUND | XmFOREGROUND | XmTOP_SHADOW |
                         XmBOTTOM_SHADOW | XmSELECT;
    _XmAddToColorCache (&cacheRec);
}


/**********************************************************************/
/** UpdateXrm()                                                      **/
/**                                                                  **/
/**********************************************************************/
static void
UpdateXrm(
        Colors colors,
        int screen ,
	XmColorObj tmpColorObj)
{
    XrmDatabase     db;
    XrmValue        value;
    int             i;
    int             doList;

    /** update the internal color information **/
    for (i = 0; i < XmCO_NUM_COLORS; i++)
        UpdatePixelSet (&(tmpColorObj->color_obj.colors[screen][i]),&colors[i]);

    /** if this is not the application screen, do not update the database **/
    if (screen != tmpColorObj->color_obj.myScreen)  return;

    /** update the color cache in motif with primary and secondary **/
    UpdateColorCache (XtScreen(tmpColorObj),
		      DefaultColormapOfScreen(XtScreen(tmpColorObj)),
		      &(colors[tmpColorObj->color_obj.primary]));

    UpdateColorCache (XtScreen(tmpColorObj),
		      DefaultColormapOfScreen(XtScreen(tmpColorObj)),
		      &(colors[tmpColorObj->color_obj.secondary]));

    db = XtScreenDatabase(XtScreen(tmpColorObj));

    /** update the clients database with new colors **/

    value.size = sizeof(Pixel);

    /** update the highlight color information to use the active color - bg **/
    value.addr = (char*) &(colors[tmpColorObj->color_obj.active].bg);
    XrmPutResource (&db, "*highlightColor", "Pixel", &value);

    /** update the primary color set information **/

    i = tmpColorObj->color_obj.primary;
    value.addr = (char*) &(colors[i].bg);
    XrmPutResource (&db, "*background", "Pixel", &value);

/*
 * keep foreground for athena widgets and 2.0 compatability.
 * DONT write out the other dependent colors in order to allow motif
 * to generate them.
 */

    value.addr = (char*) &(colors[i].fg);
    XrmPutResource (&db, "*foreground", "Pixel", &value);


    if (XmCO_DitherTopShadow(tmpColorObj->color_obj.display, screen, &colors[i]))
        XrmPutStringResource (&db, "*topShadowPixmap", XmCO_DITHER);
    else
        XrmPutStringResource (&db, "*topShadowPixmap", XmCO_NO_DITHER);
    if (XmCO_DitherBottomShadow(tmpColorObj->color_obj.display, screen, &colors[i]))
        XrmPutStringResource (&db, "*bottomShadowPixmap", XmCO_DITHER);

    /** update the secondary color set information **/
    i = tmpColorObj->color_obj.secondary;
    value.addr = (char*) &(colors[i].bg);
    XrmPutResource (&db, "*XmDialogShell*background", "Pixel", &value);
    XrmPutResource (&db, "*XmMenuShell*background", "Pixel",&value);
    XrmPutResource (&db, "*XmCascadeButton*background", "Pixel", &value);
    XrmPutResource (&db, "*XmCascadeButtonGadget*background", "Pixel", &value);

/*
 * keep foreground for athena widgets and 2.0 compatability.
 * DONT write out the other dependent colors in order to allow motif
 * to generate them.
 */
    value.addr = (char*) &(colors[i].fg);
    XrmPutResource (&db, "*XmDialogShell*foreground", "Pixel", &value);
    XrmPutResource (&db, "*XmMenuShell*foreground", "Pixel",&value);
    XrmPutResource (&db, "*XmCascadeButton*foreground", "Pixel", &value);
    XrmPutResource (&db, "*XmCascadeButtonGadget*foreground", "Pixel", &value);


    if (XmCO_DitherTopShadow(tmpColorObj->color_obj.display, screen, &colors[i]))
    {
        XrmPutStringResource (&db, "*XmDialogShell*topShadowPixmap",
			      XmCO_DITHER);
        XrmPutStringResource (&db, "*XmMenuShell*topShadowPixmap",
			      XmCO_DITHER);
        XrmPutStringResource (&db, "*XmCascadeButton*topShadowPixmap",
                              XmCO_DITHER);
        XrmPutStringResource (&db,"*XmCascadeButtonGadget*topShadowPixmap",
                              XmCO_DITHER);
    }
    else if (XmCO_DitherTopShadow(tmpColorObj->color_obj.display, screen,
                             &(colors[tmpColorObj->color_obj.primary])))
    {
        XrmPutStringResource (&db, "*XmDialogShell*topShadowPixmap",
			      XmCO_NO_DITHER);
        XrmPutStringResource (&db, "*XmMenuShell*topShadowPixmap",
			      XmCO_NO_DITHER);
        XrmPutStringResource (&db, "*XmCascadeButton*topShadowPixmap",
                              XmCO_NO_DITHER);
        XrmPutStringResource (&db,"*XmCascadeButtonGadget*topShadowPixmap",
                              XmCO_NO_DITHER);
    }

    if (XmCO_DitherBottomShadow(tmpColorObj->color_obj.display, screen, &colors[i]))
    {
        XrmPutStringResource (&db, "*XmDialogShell*bottomShadowPixmap",
                              XmCO_DITHER);
        XrmPutStringResource (&db, "*XmMenuShell*bottomShadowPixmap",
                              XmCO_DITHER);
        XrmPutStringResource (&db, "*XmCascadeButton*bottomShadowPixmap",
                              XmCO_DITHER);
        XrmPutStringResource (&db, "*XmCascadeButtonGadget*bottomShadowPixmap",
                              XmCO_DITHER);
    }
    else if (XmCO_DitherBottomShadow(tmpColorObj->color_obj.display, screen,
                             &(colors[tmpColorObj->color_obj.primary])))
    {
        XrmPutStringResource (&db, "*XmDialogShell*bottomShadowPixmap",
                              XmCO_NO_DITHER);
        XrmPutStringResource (&db, "*XmMenuShell*bottomShadowPixmap",
                              XmCO_NO_DITHER);
        XrmPutStringResource (&db, "*XmCascadeButton*bottomShadowPixmap",
                              XmCO_NO_DITHER);
        XrmPutStringResource (&db, "*XmCascadeButtonGadget*bottomShadowPixmap",
                              XmCO_NO_DITHER);
    }


    /** set all of the text areas to the textColorSetId **/
    /** continue to write fg for backward compatibility **/

    if ( !tmpColorObj->color_obj.useText )
	return;

    doList = tmpColorObj->color_obj.useTextForList;

    /** add this color to the Motif color cache **/
    UpdateColorCache (XtScreen(tmpColorObj),
		      DefaultColormapOfScreen(XtScreen(tmpColorObj)),
		      &(colors[tmpColorObj->color_obj.text]));

    /** set for text color areas **/
    i = tmpColorObj->color_obj.text;
    value.addr = (char*) &(colors[i].bg);
    XrmPutResource (&db, "*XmText*background", "Pixel", &value);
    XrmPutResource (&db, "*XmTextField*background", "Pixel", &value);
    XrmPutResource (&db, "*DtTerm*background", "Pixel", &value);
    if (doList)
      XrmPutResource (&db, "*XmList*background", "Pixel", &value);

    value.addr = (char*) &(colors[i].fg);
    XrmPutResource (&db, "*XmText*foreground", "Pixel", &value);
    XrmPutResource (&db, "*XmTextField*foreground", "Pixel", &value);
    XrmPutResource (&db, "*DtTerm*foreground", "Pixel", &value);
    if (doList)
      XrmPutResource (&db, "*XmList*foreground", "Pixel", &value);

    /** set for all secondary color areas **/

    value.addr = (char*) &(colors[i].bg);  /* do background first */

    XrmPutResource (&db, "*XmDialogShell*XmText*background", "Pixel", &value);
    XrmPutResource (&db, "*XmDialogShell*XmTextField*background", "Pixel",
		    &value);
    XrmPutResource (&db, "*XmDialogShell*DtTerm*background", "Pixel", &value);
    XrmPutResource (&db, "*XmMenuShell*XmText*background", "Pixel", &value);
    XrmPutResource (&db, "*XmMenuShell*XmTextField*background", "Pixel",&value);
    XrmPutResource (&db, "*XmMenuShell*DtTerm*background", "Pixel", &value);
    XrmPutResource (&db, "*XmCascadeButton*XmText*background", "Pixel", &value);
    XrmPutResource (&db, "*XmCascadeButton*XmTextField*background", "Pixel",
		    &value);
    XrmPutResource (&db, "*XmCascadeButton*DtTerm*background", "Pixel", &value);
    XrmPutResource (&db, "*XmCascadeButtonGadget*XmText*background", "Pixel",
		    &value);
    XrmPutResource (&db, "*XmCascadeButtonGadget*XmTextField*background",
		    "Pixel", &value);
    XrmPutResource (&db, "*XmCascadeButtonGadget*DtTerm*background", "Pixel",
		    &value);
    if (doList) {
      XrmPutResource (&db, "*XmDialogShell*XmList*background", "Pixel", &value);
      XrmPutResource (&db, "*XmMenuShell*XmList*background", "Pixel", &value);
    }

    value.addr = (char*) &(colors[i].fg);  /* now do foreground */

    XrmPutResource (&db, "*XmDialogShell*XmText*foreground", "Pixel", &value);
    XrmPutResource (&db, "*XmDialogShell*XmTextField*foreground", "Pixel",
		    &value);
    XrmPutResource (&db, "*XmDialogShell*DtTerm*foreground", "Pixel", &value);
    XrmPutResource (&db, "*XmMenuShell*XmText*foreground", "Pixel", &value);
    XrmPutResource (&db, "*XmMenuShell*XmTextField*foreground", "Pixel",&value);
    XrmPutResource (&db, "*XmMenuShell*DtTerm*foreground", "Pixel", &value);
    XrmPutResource (&db, "*XmCascadeButton*XmText*foreground", "Pixel", &value);
    XrmPutResource (&db, "*XmCascadeButton*XmTextField*foreground", "Pixel",
		    &value);
    XrmPutResource (&db, "*XmCascadeButton*DtTerm*foreground", "Pixel", &value);
    XrmPutResource (&db, "*XmCascadeButtonGadget*XmText*foreground", "Pixel",
		    &value);
    XrmPutResource (&db, "*XmCascadeButtonGadget*XmTextField*foreground",
		    "Pixel", &value);
    XrmPutResource (&db, "*XmCascadeButtonGadget*DtTerm*foreground", "Pixel",
		    &value);
    if (doList) {
      XrmPutResource (&db, "*XmDialogShell*XmList*foreground", "Pixel", &value);
      XrmPutResource (&db, "*XmMenuShell*XmList*foreground", "Pixel", &value);
    }

    /** dither (or "undither") top shadow if needed **/

    if (XmCO_DitherTopShadow(tmpColorObj->color_obj.display, screen, &colors[i]))
    {
        XrmPutStringResource (&db, "*XmText*topShadowPixmap", XmCO_DITHER);
        XrmPutStringResource (&db, "*XmTextField*topShadowPixmap", XmCO_DITHER);
        XrmPutStringResource (&db, "*DtTerm*topShadowPixmap", XmCO_DITHER);
        if (doList)
	  XrmPutStringResource (&db, "*XmList*topShadowPixmap", XmCO_DITHER);

        XrmPutStringResource (&db, "*XmDialogShell*XmText*topShadowPixmap",
			      XmCO_DITHER);
        XrmPutStringResource (&db, "*XmDialogShell*XmTextField*topShadowPixmap",
			      XmCO_DITHER);
        XrmPutStringResource (&db, "*XmDialogShell*DtTerm*topShadowPixmap",
			      XmCO_DITHER);
        if (doList)
	  XrmPutStringResource (&db, "*XmDialogShell*XmList*topShadowPixmap",
				XmCO_DITHER);

        XrmPutStringResource (&db, "*XmMenuShell*XmText*topShadowPixmap",
			      XmCO_DITHER);
        XrmPutStringResource (&db, "*XmMenuShell*XmTextField*topShadowPixmap",
			      XmCO_DITHER);
        XrmPutStringResource (&db, "*XmMenuShell*DtTerm*topShadowPixmap",
			      XmCO_DITHER);
        if (doList)
	  XrmPutStringResource (&db, "*XmMenuShell*XmList*topShadowPixmap",
				XmCO_DITHER);

        XrmPutStringResource (&db, "*XmCascadeButton*XmText*topShadowPixmap",
			      XmCO_DITHER);
        XrmPutStringResource (&db,
			      "*XmCascadeButton*XmTextField*topShadowPixmap",
			      XmCO_DITHER);
        XrmPutStringResource (&db, "*XmCascadeButton*DtTerm*topShadowPixmap",
			      XmCO_DITHER);

        XrmPutStringResource (&db,
			      "*XmCascadeButtonGadget*XmText*topShadowPixmap",
			      XmCO_DITHER);
        XrmPutStringResource (&db,
			   "*XmCascadeButtonGadget*XmTextField*topShadowPixmap",
			      XmCO_DITHER);
        XrmPutStringResource (&db,
			      "*XmCascadeButtonGadget*DtTerm*topShadowPixmap",
		              XmCO_DITHER);
    }
    else  /** undo dithers set for primary and secondary if needed **/
    {
      if (XmCO_DitherTopShadow(tmpColorObj->color_obj.display, screen,
                             &(colors[tmpColorObj->color_obj.primary])))
      {
        XrmPutStringResource (&db, "*XmText*topShadowPixmap", XmCO_NO_DITHER);
        XrmPutStringResource (&db, "*XmTextField*topShadowPixmap",
			      XmCO_NO_DITHER);
        XrmPutStringResource (&db, "*DtTerm*topShadowPixmap", XmCO_NO_DITHER);
        if (doList)
	  XrmPutStringResource (&db, "*XmList*topShadowPixmap", XmCO_NO_DITHER);
      }
      if (XmCO_DitherTopShadow(tmpColorObj->color_obj.display, screen,
                             &(colors[tmpColorObj->color_obj.secondary])))
      {

        XrmPutStringResource (&db, "*XmDialogShell*XmText*topShadowPixmap",
			      XmCO_NO_DITHER);
        XrmPutStringResource (&db, "*XmDialogShell*XmTextField*topShadowPixmap",
			      XmCO_NO_DITHER);
        XrmPutStringResource (&db, "*XmDialogShell*DtTerm*topShadowPixmap",
			      XmCO_NO_DITHER);
        if (doList)
	  XrmPutStringResource (&db, "*XmDialogShell*XmList*topShadowPixmap",
				XmCO_NO_DITHER);

        XrmPutStringResource (&db, "*XmMenuShell*XmText*topShadowPixmap",
			      XmCO_NO_DITHER);
        XrmPutStringResource (&db, "*XmMenuShell*XmTextField*topShadowPixmap",
			      XmCO_NO_DITHER);
        XrmPutStringResource (&db, "*XmMenuShell*DtTerm*topShadowPixmap",
			      XmCO_NO_DITHER);
        if (doList)
	  XrmPutStringResource (&db, "*XmMenuShell*XmList*topShadowPixmap",
				XmCO_NO_DITHER);

        XrmPutStringResource (&db, "*XmCascadeButton*XmText*topShadowPixmap",
			      XmCO_NO_DITHER);
        XrmPutStringResource (&db,
			      "*XmCascadeButton*XmTextField*topShadowPixmap",
			      XmCO_NO_DITHER);
        XrmPutStringResource (&db, "*XmCascadeButton*DtTerm*topShadowPixmap",
			      XmCO_NO_DITHER);

        XrmPutStringResource (&db,
			      "*XmCascadeButtonGadget*XmText*topShadowPixmap",
			      XmCO_NO_DITHER);
        XrmPutStringResource (&db,
			   "*XmCascadeButtonGadget*XmTextField*topShadowPixmap",
			      XmCO_NO_DITHER);
        XrmPutStringResource (&db,
			      "*XmCascadeButtonGadget*DtTerm*topShadowPixmap",
		              XmCO_NO_DITHER);
      }
    }

    /** dither (or "undither") bottom shadow if needed **/

    if (XmCO_DitherBottomShadow(tmpColorObj->color_obj.display, screen, &colors[i]))
    {
        XrmPutStringResource (&db, "*XmText*bottomShadowPixmap", XmCO_DITHER);
        XrmPutStringResource (&db, "*XmTextField*bottomShadowPixmap", XmCO_DITHER);
        XrmPutStringResource (&db, "*DtTerm*bottomShadowPixmap", XmCO_DITHER);
        if (doList)
	  XrmPutStringResource (&db, "*XmList*bottomShadowPixmap", XmCO_DITHER);

        XrmPutStringResource (&db, "*XmDialogShell*XmText*bottomShadowPixmap",
			      XmCO_DITHER);
        XrmPutStringResource (&db,
			      "*XmDialogShell*XmTextField*bottomShadowPixmap",
			      XmCO_DITHER);
        XrmPutStringResource (&db, "*XmDialogShell*DtTerm*bottomShadowPixmap",
			      XmCO_DITHER);
        if (doList)
	  XrmPutStringResource (&db, "*XmDialogShell*XmList*bottomShadowPixmap",
			      XmCO_DITHER);

        XrmPutStringResource (&db, "*XmMenuShell*XmText*bottomShadowPixmap",
			      XmCO_DITHER);
        XrmPutStringResource (&db,"*XmMenuShell*XmTextField*bottomShadowPixmap",
			      XmCO_DITHER);
        XrmPutStringResource (&db, "*XmMenuShell*DtTerm*bottomShadowPixmap",
			      XmCO_DITHER);
        if (doList)
	  XrmPutStringResource (&db, "*XmMenuShell*XmList*bottomShadowPixmap",
			      XmCO_DITHER);

        XrmPutStringResource (&db, "*XmCascadeButton*XmText*bottomShadowPixmap",
			      XmCO_DITHER);
        XrmPutStringResource (&db,
			      "*XmCascadeButton*XmTextField*bottomShadowPixmap",
			      XmCO_DITHER);
        XrmPutStringResource (&db, "*XmCascadeButton*DtTerm*bottomShadowPixmap",
			      XmCO_DITHER);

        XrmPutStringResource (&db,
			     "*XmCascadeButtonGadget*XmText*bottomShadowPixmap",
			      XmCO_DITHER);
        XrmPutStringResource (&db,
			"*XmCascadeButtonGadget*XmTextField*bottomShadowPixmap",
			      XmCO_DITHER);
        XrmPutStringResource (&db,
			     "*XmCascadeButtonGadget*DtTerm*bottomShadowPixmap",
		              XmCO_DITHER);
    }
    else  /** undo dithers set for primary and secondary if needed **/
    {
      if (XmCO_DitherBottomShadow(tmpColorObj->color_obj.display, screen,
                             &(colors[tmpColorObj->color_obj.primary])))
      {
        XrmPutStringResource (&db, "*XmText*bottomShadowPixmap", XmCO_NO_DITHER);
        XrmPutStringResource (&db, "*XmTextField*bottomShadowPixmap",XmCO_NO_DITHER);
        XrmPutStringResource (&db, "*DtTerm*bottomShadowPixmap", XmCO_NO_DITHER);
        if (doList)
	  XrmPutStringResource (&db, "*XmList*bottomShadowPixmap", XmCO_NO_DITHER);
      }
      if (XmCO_DitherBottomShadow(tmpColorObj->color_obj.display, screen,
                             &(colors[tmpColorObj->color_obj.secondary])))
      {

        XrmPutStringResource (&db, "*XmDialogShell*XmText*bottomShadowPixmap",
			      XmCO_NO_DITHER);
        XrmPutStringResource (&db,
			      "*XmDialogShell*XmTextField*bottomShadowPixmap",
			      XmCO_NO_DITHER);
        XrmPutStringResource (&db, "*XmDialogShell*DtTerm*bottomShadowPixmap",
			      XmCO_NO_DITHER);
        if (doList)
	  XrmPutStringResource (&db, "*XmDialogShell*XmList*bottomShadowPixmap",
			      XmCO_NO_DITHER);

        XrmPutStringResource (&db, "*XmMenuShell*XmText*bottomShadowPixmap",
			      XmCO_NO_DITHER);
        XrmPutStringResource (&db,"*XmMenuShell*XmTextField*bottomShadowPixmap",
			      XmCO_NO_DITHER);
        XrmPutStringResource (&db, "*XmMenuShell*DtTerm*bottomShadowPixmap",
			      XmCO_NO_DITHER);
        if (doList)
	  XrmPutStringResource (&db, "*XmMenuShell*XmList*bottomShadowPixmap",
			      XmCO_NO_DITHER);

        XrmPutStringResource (&db, "*XmCascadeButton*XmText*bottomShadowPixmap",
			      XmCO_NO_DITHER);
        XrmPutStringResource (&db,
			      "*XmCascadeButton*XmTextField*bottomShadowPixmap",
			      XmCO_NO_DITHER);
        XrmPutStringResource (&db, "*XmCascadeButton*DtTerm*bottomShadowPixmap",
			      XmCO_NO_DITHER);

        XrmPutStringResource (&db,
			     "*XmCascadeButtonGadget*XmText*bottomShadowPixmap",
			      XmCO_NO_DITHER);
        XrmPutStringResource (&db,
			"*XmCascadeButtonGadget*XmTextField*bottomShadowPixmap",
			      XmCO_NO_DITHER);
        XrmPutStringResource (&db,
			     "*XmCascadeButtonGadget*DtTerm*bottomShadowPixmap",
		              XmCO_NO_DITHER);
      }
    }
}


/**********************************************************************/
/** XmeGetIconControlInfo                                            **/
/**              Get information needed for XmpGetIconFileName       **/
/**********************************************************************/
Boolean
XmeGetIconControlInfo(
        Screen  *screen,
	Boolean *useMaskRtn,
        Boolean *useMultiColorIconsRtn,
        Boolean *useIconFileCacheRtn)
{
    XmColorObj tmpColorObj = _XmDefaultColorObj;

    (void)screen;
    _XmProcessLock();
    /* return False if color srv is not running, or color obj not used */
    if (!tmpColorObj || !tmpColorObj->color_obj.colorIsRunning ||
	!tmpColorObj->color_obj.useColorObj)
    {
	*useMaskRtn = *useMultiColorIconsRtn = *useIconFileCacheRtn = True ;
	_XmProcessUnlock();
        return False;
    }
    *useMaskRtn = tmpColorObj->color_obj.useMask;
    *useMultiColorIconsRtn = tmpColorObj->color_obj.useMultiColorIcons;
    *useIconFileCacheRtn = tmpColorObj->color_obj.useIconFileCache;
    _XmProcessUnlock();
    return True;
}



/**********************************************************************/
/** XmeGetColorObjData()                                             **/
/**              pixelSet should be an array of num_pixelSet entries  **/
/**              Contrary to XmeGetPixelData, use the correct display **/
/**                                                                  **/
/**  for instance, to access the primary background, use
             primary_background = pixelSet[primary_id].bg ;          */
/**********************************************************************/
Boolean
XmeGetColorObjData(Screen * screen,
		   int *colorUse,
		   XmPixelSet *pixelSet,
		   unsigned short num_pixelSet,
		   short *active_id,
		   short *inactive_id,
		   short *primary_id,
		   short *secondary_id,
		   short *text_id)
{
    XmColorObj tmpColorObj ;
    int screen_num, k ;

    /* find the color obj for this screen's display */
    if ((!_XmColorObjCacheDisplay) || (XFindContext(_XmColorObjCacheDisplay,
	             (XID)XDisplayOfScreen(screen),
	             _XmColorObjCache,
	             (XPointer *)&tmpColorObj) != 0)) {
        /* no color obj for this display */
	return False ;
    }

    /* return False if color srv is not running, or color obj not used */
    _XmProcessLock();
    if (!tmpColorObj ||
	!tmpColorObj->color_obj.useColorObj ||
	!tmpColorObj->color_obj.colorIsRunning) {
      _XmProcessUnlock();
      return False;
    }

    screen_num = XScreenNumberOfScreen(screen) ;

    /* return False if screen invalid */
    if (screen_num >= tmpColorObj->color_obj.numScreens) {
	_XmProcessUnlock();
        return False;
    }

    if (colorUse) *colorUse = tmpColorObj->color_obj.colorUse[screen_num];

    for (k = 0; k < num_pixelSet; k++)
    {
        pixelSet[k].fg = tmpColorObj->color_obj.colors[screen_num][k].fg;
        pixelSet[k].bg = tmpColorObj->color_obj.colors[screen_num][k].bg;
        pixelSet[k].ts = tmpColorObj->color_obj.colors[screen_num][k].ts;
        pixelSet[k].bs = tmpColorObj->color_obj.colors[screen_num][k].bs;
        pixelSet[k].sc = tmpColorObj->color_obj.colors[screen_num][k].sc;
    }

    if (active_id) *active_id = (short)tmpColorObj->color_obj.active;
    if (inactive_id) *inactive_id = (short)tmpColorObj->color_obj.inactive;
    if (primary_id) *primary_id = (short)tmpColorObj->color_obj.primary;
    if (secondary_id) *secondary_id = (short)tmpColorObj->color_obj.secondary;
    if (text_id) *text_id = (short)tmpColorObj->color_obj.text;

    _XmProcessUnlock();
    return True;
}


/**********************************************************************/
/** XmeGetPixelData()                                                **/
/**              pixelSet should be an array of XmCO_NUM_COLORS      **/
/**              XmPixelSets, ie. XmPixelSet pixelSet[XmCO_NUM_COLORS]; **/
/**                                                                  **/
/**      Obsolete: doesn't always use the correct display in a
                   multi display app...  */
/**********************************************************************/
Boolean
XmeGetPixelData(
        int screen_number,
        int *colorUse,
        XmPixelSet *pixelSet,
        short *a,
        short *i,
        short *p,
        short *s )
{
    Display * display ;

    _XmProcessLock();
    if (_XmDefaultColorObj)
	display = XtDisplay(_XmDefaultColorObj) ;
    else
	return False ;
    _XmProcessUnlock();

    return XmeGetColorObjData(XScreenOfDisplay(display, screen_number),
			  colorUse,
			  pixelSet, XmCO_NUM_COLORS,
			  a, i, p, s, NULL);

}



static
Boolean NotBW(Screen* screen, Pixel pixel)
{
    return ((pixel != BlackPixel(DisplayOfScreen(screen),
				 XScreenNumberOfScreen(screen)))
	&&  (pixel != WhitePixel(DisplayOfScreen(screen),
				 XScreenNumberOfScreen(screen))));
}

static
Boolean DupPixel(Pixel pixel, XColor * colors, int up_to)
{
    int i ;
    for (i=0; i < up_to; i++)
	if (colors[i].pixel == pixel) return True;

    return False ;
}

/* std CDE desktop icon colors */
static String IconColorNames[] = {
    "black", "white", "red", "green", "blue", "yellow", "cyan", "magenta",
    "#dededededede", "#bdbdbdbdbdbd", "#adadadadadad", "#949494949494",
    "#737373737373", "#636363636363", "#424242424242", "#212121212121"
};

/**********************************************************************/
/** XmeGetDesktopColorCells                                          **/
/**              Get information needed for application using
                 private colormap       **/
/**********************************************************************/
Boolean
XmeGetDesktopColorCells (Screen * screen,
			 Colormap colormap, /* to update the color cache */
			 XColor * colors,  /* allocated by the caller */
			 int n_colors,     /* size available */
			 int * ncolors_returns) /* <= max_color */
{
    int colorUse ;
    XmPixelSet pixelSet[XmCO_NUM_COLORS] ;
    int i, k, num_icon_colors ;
    short primary, secondary, text, active, inactive;
    int reorder[XmCO_NUM_COLORS] ;

    if (!colors || !n_colors) return False ;

    /* get the pixel out of the color obj for this screen */
    if (!(XmeGetColorObjData(screen,
		       &colorUse,
		       pixelSet, XmCO_NUM_COLORS,
		       &active, &inactive, &primary, &secondary, &text)))
	return False ;

    /* now get the pixel in the XColor colors array, if any */
    if (colorUse == XmCO_BLACK_WHITE)
	return False ;


/* only report pixels if not black or white, which is what ts and bs
   are when usePixmap is On or fg is when dynamicForeground is off */
/* try also to be smart about not returning duplicate pixel id by
   comparing to what's already there each time, even if that's a little
   bit expensive */

#define OKPixel(pixel) (\
	 NotBW(screen, pixel) && !DupPixel(pixel, colors, i))
    i = 0 ;

    /* start with foreground are background for all palettes.
       These are the most important pixels by far. let's even
       reorder them as we go to get more important palette first:
       primary, secondary, text, active, inactive, frontpanel, wsbuttons */

    reorder[0] = primary ;
    reorder[1] = secondary;
    reorder[2] = text;
    reorder[3] = active;
    reorder[4] = inactive;
    reorder[5] = 7;
    reorder[6] = 2;
    reorder[7] = 6;

    for (k = 0; k < XmCO_NUM_COLORS; k++) {
	/* update the color cache for the pixels used in widgets */
	if (reorder[k] == primary ||
	    reorder[k] == secondary ||
	    reorder[k] == text) {
	    UpdateColorCache(screen, colormap, &(pixelSet[reorder[k]]));
	}
	if (OKPixel(pixelSet[reorder[k]].fg)) {
	    colors[i++].pixel = pixelSet[reorder[k]].fg ;
	    if (i == n_colors) break ;
	}
	if (OKPixel(pixelSet[reorder[k]].bg)) {
	    colors[i++].pixel = pixelSet[reorder[k]].bg ;
	    if (i == n_colors) break ;
	}
    }

    /* now the select color for primary and secondary: where
       select stuff can be */
    if (OKPixel(pixelSet[primary].sc)) {
	if (i < n_colors) colors[i++].pixel = pixelSet[primary].sc ;
    }
    if (OKPixel(pixelSet[secondary].sc)) {
	if (i < n_colors) colors[i++].pixel = pixelSet[secondary].sc ;
    }

    /* here we insert the icon pixels: just black and white for
       low color mode, or the 16 colors for high and medium */
    if (colorUse == XmCO_HIGH_COLOR || colorUse == XmCO_MEDIUM_COLOR)
	num_icon_colors = 16 ;
    else
	num_icon_colors = 2 ;

    for(k=0; k<num_icon_colors && i < n_colors; k++) {
	/* here we get the rgb value first
	   and then the pixel in the default colormap.
	   The query color below will get the rgb again for nothing,
	   no big deal */
	XParseColor(DisplayOfScreen(screen),
		    DefaultColormapOfScreen(screen),
		    IconColorNames[k],
		    &(colors[i]));
	XAllocColor(DisplayOfScreen(screen),
		    DefaultColormapOfScreen(screen),
		    &(colors[i++]));
    }

    /* now the shadows */
    for (k = 0; k < XmCO_NUM_COLORS && i < n_colors; k++) {
	if (OKPixel(pixelSet[reorder[k]].ts)) {
	    colors[i++].pixel = pixelSet[reorder[k]].ts ;
	}
    }
    for (k = 0; k < XmCO_NUM_COLORS && i < n_colors; k++) {
	if (OKPixel(pixelSet[reorder[k]].bs)) {
	    colors[i++].pixel = pixelSet[reorder[k]].bs ;
	}
    }
    /* finish with remaining select color
       DupPixel will take care of the primary/secondary already there */
    for (k = 0; k < XmCO_NUM_COLORS && i < n_colors; k++) {
	if (OKPixel(pixelSet[reorder[k]].sc)) {
	    colors[i++].pixel = pixelSet[reorder[k]].sc ;
	}
    }


    /* now get the rgb for the color obj pixels on the default
       colormap, i.e. the one the color server uses. note that
       for the icon colors, we already have them, but that will
       save us a request */
    XQueryColors (DisplayOfScreen(screen),
		  DefaultColormapOfScreen(screen),
		  colors, i);

    /* as a service to the app, add flags to all colors */
    for (k=0; k<i; k++) colors[k].flags = DoRed | DoGreen | DoBlue;

    if (ncolors_returns) *ncolors_returns = i ;
	return True;
}

static int trap_XCloseDisplay(Display *disp, XExtCodes *codes)
{
    if (disp == _XmColorObjCacheDisplay)
    	_XmColorObjCacheDisplay = NULL;
    return 0;
}

/**********************************************************************/
/** XmeUseColorObj()                                                 **/
/**           Return False if color is not working for some reason.  **/
/**                                                                  **/
/**           Could be due to useColorObj resource == False, or any  **/
/**           problem with the color server or color object.         **/
/**                                                                  **/
/**********************************************************************/
Boolean
XmeUseColorObj( void )
{
    XmColorObj tmpColorObj = _XmDefaultColorObj;

    _XmProcessLock();
    if (!tmpColorObj ||
      !tmpColorObj->color_obj.colorIsRunning ||
      !tmpColorObj->color_obj.useColorObj) {
      _XmProcessUnlock();
      return False;
    }
    else {
      _XmProcessUnlock();
      return True;
    }
}

