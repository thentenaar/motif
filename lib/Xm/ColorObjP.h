/* $XConsortium: ColorObjP.h /main/10 1996/12/16 18:30:49 drk $ */
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
 * 
 */
/*
 * HISTORY
 */
#ifndef _XmColorObjP_h
#define _XmColorObjP_h

#include <Xm/VendorSP.h>

#ifdef __cplusplus
extern "C" {
#endif

/** misc structures, defines, and functions for using ColorObj **/

#define XmCO_MAX_NUM_COLORS	 8
#define XmCO_NUM_COLORS		 XmCO_MAX_NUM_COLORS
#define XmPIXEL_SET_PROP_VERSION '1'

/* Constants for color usage */
enum { XmCO_BLACK_WHITE, XmCO_LOW_COLOR, XmCO_MEDIUM_COLOR, XmCO_HIGH_COLOR };

typedef struct {
    Pixel fg;
    Pixel bg;
    Pixel ts;
    Pixel bs;
    Pixel sc;
} XmPixelSet;

typedef XmPixelSet Colors[XmCO_NUM_COLORS];

typedef struct _XmColorObjPart {
    XtArgsProc          RowColInitHook;
    XmPixelSet       	*myColors;     /* colors for my (application) screen */
    int             	myScreen;
    Display             *display;     /* display connection for "pseudo-app" */
    Colors         	*colors;      /* colors per screen for workspace mgr */
    int             	numScreens;   /*               for workspace manager */
    Atom           	*atoms;       /* to identify colorsrv screen numbers */
    Boolean         	colorIsRunning;   /* used for any color problem      */
    Boolean         	done;
    int            	*colorUse;
    int             	primary;
    int             	secondary;
    int             	text;          /* color set id for text widgets */
    int             	active;
    int             	inactive;
    Boolean         	useColorObj;  /* read only resource variable */
    Boolean             useText;        /* use text color set id for text? */
    Boolean             useTextForList; /* use text color set id for lists? */
    
    Boolean		useMask;
    Boolean		useMultiColorIcons;
    Boolean		useIconFileCache;

} XmColorObjPart;


typedef struct _XmColorObjRec {
    CorePart 		core;
    CompositePart 	composite;
    ShellPart 		shell;
    WMShellPart		wm;
    XmColorObjPart	color_obj;
} XmColorObjRec;

typedef struct _XmColorObjClassPart {
    XtPointer        extension;
} XmColorObjClassPart;

/* 
 * we make it a appShell subclass so it can have it's own instance
 * hierarchy
 */
typedef struct _XmColorObjClassRec{
    CoreClassPart      		core_class;
    CompositeClassPart 		composite_class;
    ShellClassPart  		shell_class;
    WMShellClassPart   		wm_shell_class;
    XmColorObjClassPart		color_obj_class;
} XmColorObjClassRec;


externalref XmColorObjClassRec xmColorObjClassRec;


#ifndef XmIsColorObj
#define XmIsColorObj(w) (XtIsSubclass(w, xmColorObjClass))
#endif /* XmIsXmDisplay */

externalref WidgetClass  xmColorObjClass;
typedef struct _XmColorObjClassRec *XmColorObjClass;
typedef struct _XmColorObjRec      *XmColorObj;


#define  XmCO_DitherTopShadow(display, screen, pixelSet) \
                        ((pixelSet)->bs == BlackPixel((display), (screen)))

#define  XmCO_DitherBottomShadow(display, screen, pixelSet) \
                        ((pixelSet)->ts == WhitePixel((display), (screen)))

#define  XmCO_DITHER     XmS50_foreground
#define  XmCO_NO_DITHER  XmSunspecified_pixmap


/********    Private Function Declarations    ********/

extern Boolean XmeGetIconControlInfo( 
                        Screen *screen,
                        Boolean *useMaskRtn,
                        Boolean *useMultiColorIconsRtn,
                        Boolean *useIconFileCacheRtn) ;

extern Boolean XmeUseColorObj( void ) ;


extern Boolean XmeGetColorObjData(
                   Screen * screen,
                   int *colorUse,
		   XmPixelSet *pixelSet,
		   unsigned short num_pixelSet,
		   short *active_id,
		   short *inactive_id,
		   short *primary_id,
		   short *secondary_id,
		   short *text_id) ;

extern Boolean XmeGetDesktopColorCells (
                         Screen * screen, 
			 Colormap colormap, 
			 XColor * colors,  
			 int n_colors,     
			 int * ncolors_returns) ;

extern Boolean XmeGetPixelData (
			int screen_number,
			int *colorUse,
			XmPixelSet *pixelSet,
			short *a,
			short *i,
			short *p,
			short *s) ;

/********    End Private Function Declarations    ********/


#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmColorObjP_h */

