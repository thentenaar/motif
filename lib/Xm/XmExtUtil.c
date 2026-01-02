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
 *
 */

/************************************************************
*	INCLUDE FILES
*************************************************************/
#include <stdarg.h>
#include <X11/IntrinsicP.h>

#include "XmI.h"
#include <Xm/XmP.h>
#include <Xm/PrimitiveP.h>
#include <Xm/GadgetP.h>
#include <Xm/ExtP.h>

/************************************************************
*	TYPEDEFS AND DEFINES
*************************************************************/

typedef struct _PixmapCache {
    Screen *screen;
    Pixmap pixmap;
    Pixel foreground, background;
    unsigned int depth;
    int ref_count;
    struct _PixmapCache *next;
} CacheEntry;

/************************************************************
*	MACROS
*************************************************************/

/************************************************************
*	GLOBAL DECLARATIONS
*************************************************************/



String xm_std_filter[] = { XmNx, XmNy, XmNwidth, XmNheight,
			   XmNdestroyCallback, XmNsensitive, XmNuserData,
			   XmNnavigationType, NULL };

String xm_std_constraint_filter[] = { XmNx, XmNy, XmNwidth, XmNheight,
			   XmNdestroyCallback, XmNsensitive, XmNuserData,
			   XmNnavigationType, XmNbottomAttachment,
			   XmNbottomOffset, XmNbottomPosition, XmNbottomWidget,
			   XmNbottomAttachment, XmNtopAttachment,
			   XmNleftOffset, XmNleftPosition, XmNleftWidget,
			   XmNtopOffset, XmNtopPosition, XmNtopWidget,
			   XmNrightOffset, XmNrightPosition, XmNrightWidget,
			   XmNrightAttachment, XmNleftAttachment,
			   XmNallowResize, XmNpaneMinimum, XmNshowSash,
			   XmNpaneMaximum, XmNpreferredPaneSize,
			   XmNresizeToPreferred, XmNskipAdjust,
			   XmNdFieldPrefHeight, XmNdFieldMaxHeight,
			   XmNdFieldPrefWidth, XmNdFieldMaxWidth,
                           XmNdFieldMinHeight, XmNdFieldMinWidth,
			   /* CR03683 */ XmNpixmapWidth, XmNpixmapHeight,
			   NULL };

/************************************************************
*	EXTERNAL DECLARATIONS
*************************************************************/

/************************************************************
*	STATIC DECLARATIONS
*************************************************************/

static CacheEntry *pixmapCache = NULL;

/************************************************************
*	GLOBAL CODE
*************************************************************/

/*	Function Name: _XmRequestNewSize
 *	Description:   Asks our parent for a new size.
 *	Arguments:     w - the data request tree widget.
 *                     query_only - only ask what would happen, don't
 *                                  change anything.
 *                     width, height - size to request.
 *                     r_width, r_height - allowed size.
 *	Returns:       none.
 */

XtGeometryResult
_XmRequestNewSize(Widget w, Boolean query_only,
		  Dimension width, Dimension height,
		  Dimension * r_width, Dimension * r_height)
{
    XtGeometryResult ret_val;
    XtWidgetGeometry request, result;

    request.width = width;
    request.height = height;
    request.request_mode = CWWidth | CWHeight;

    if (query_only)
	request.request_mode |= XtCWQueryOnly;

    ret_val = XtMakeGeometryRequest(w, &request, &result);

    if (ret_val == XtGeometryAlmost) {
	if (!query_only)
	    ret_val = XtMakeGeometryRequest(w, &result, NULL);

	*r_width = result.width;
	*r_height = result.height;
    }
    else if (ret_val == XtGeometryYes) {
	*r_width = request.width;
	*r_height = request.height;
    }
    else {
	*r_width = w->core.width;
	*r_height = w->core.height;
    }

    return(ret_val);
}

/*	Function Name: _XmHWQuery
 *	Description:   Handles much of the generic height and width query
 *                     geometry processing.
 *	Arguments:     w - the widget to process.
 *                     intended, preferred - The values from the real query
 *                                           procedure.  It is assumed
 *                                           that preferred is already
 *                                           filled with the correct desired
 *                                           size.
 *	Returns:       an XtGeometryResult.
 */

XtGeometryResult
_XmHWQuery(Widget w, XtWidgetGeometry * intended, XtWidgetGeometry * preferred)
{
    if (intended == NULL) {
	if ((preferred->width == w->core.width) &&
 	    (preferred->height == w->core.height))
	{
	    return(XtGeometryNo);
	}
    }
    else {
	if ((intended->request_mode & CWWidth) &&
	    (intended->request_mode & CWHeight))
	{
	    if ((intended->width == preferred->width) &&
		(intended->height == preferred->height))
	    {
		return(XtGeometryYes);
	    }
	    else {
		return(XtGeometryNo);
	    }
	}
    }

    preferred->request_mode = CWWidth | CWHeight;
    return(XtGeometryAlmost);
}

/*	Function Name: _XmGadgetWarning
 *	Description: Checks to see if this is a gadget.  If it is then
 *                   print out a warning, and return True.
 *	Arguments: w - the widget to check.
 *	Returns: True if this object is a gadget.
 */

Boolean
_XmGadgetWarning(Widget w)
{
    if (!XtIsRectObj(w) || XtIsWidget(w))
	return(False);

    XmeWarning(XtParent(w), XmNnoGadgetSupportMsg);

    return(True);
}

/*	Function Name: _XmGetFocus
 *	Description:   Gets the XmFocus.
 *	Arguments:     w - the icon button widget.
 *                     event - the event that caused this action.
 *                     params, num_params - action routine parameters.
 *	Returns:       none.
 */

void
_XmGetFocus(Widget w, XEvent * event, String * params, Cardinal * num_params)
{
#ifdef VMS
    if (XtIsRealized(w))
#endif
	(void) XmProcessTraversal(w, XmTRAVERSE_CURRENT);
}

/*	Function Name: _XmFilterArgs
 *	Description: Filters certain resources out of an argument list.
 *	Arguments: args, num_args - the input argument list.
 *                 filter - the list of resources (by name) to remove.
 * RETURNED        filtered_args, num_filtered_args - new filtered list.
 *	Returns: none.
 *
 * NOTE The caller of this function is responsible for freeing "filtered_args"
 *      with XtFree() when it is no longer in use.
 */

void
_XmFilterArgs(ArgList args, Cardinal num_args, String *filter,
	      ArgList *filtered_args, Cardinal *num_filtered_args)
{
    ArgList fargs = (ArgList) XtMalloc(sizeof(Arg) * num_args);
    int i;
    String *ptr;

    *filtered_args = fargs;
    *num_filtered_args = 0;
    for (i = 0; i < num_args; i++) {
	Boolean match = False;
	for (ptr = filter; *ptr != NULL; ptr++) {
	    if (streq(*ptr, args[i].name)) {
		match = True;
		break;
	    }
	}
	if (!match) {
	    *fargs++ = args[i];
	    (*num_filtered_args)++;
	}
    }
}

/*	Function Name: _XmSetValuesOnChildren
 *	Description: Calls setvalues on all children of this widget,
 *                   and then recurses.
 *	Arguments: w - the widget to set.
 *                 args, num_args - arguments to set.
 *	Returns: none.
 */

void
_XmSetValuesOnChildren(Widget w, ArgList args, Cardinal num_args)
{
    Widget *childP;

    if (!XtIsSubclass(w, compositeWidgetClass))
	return;

    ForAllChildren((CompositeWidget) w, childP) {
	XtSetValues(*childP, args, num_args);
	_XmSetValuesOnChildren(*childP, args, num_args);
    }
}

/*      Function Name: _XmUtilIsSubclassByNameQ
 *      Description:   Determines whether this is a subclass of the named
 *                     class.
 *      Arguments:     w - the widget to check.
 *                     nameq - a quarkified name for the class to check.
 *      Returns:       True if this is a subclass.
 */

Boolean
_XmUtilIsSubclassByNameQ(Widget w, XrmQuark nameq)
{
    WidgetClass class;

    Boolean returnValue = False;

    _XmProcessLock();
    for (class = XtClass(w) ;
         class != NULL ;
         class = class->core_class.superclass)
    {
        if (nameq == XrmStringToQuark(class->core_class.class_name))
        {
            returnValue = True;
            break;
        }
    }
    _XmProcessUnlock();

    return(returnValue);
}

/*	Function Name: _XmGetMBStringFromXmString
 *	Description: Given an Xm String, returns an MB string
 *	Arguments: xmstr - an Xm String.
 *	Returns: A multi byte string.
 */

String
_XmGetMBStringFromXmString(XmString xmstr)
{
    String 			text;
    XmStringContext 		context;   /* context for conversion	*/
    char			*newText;  /* new text string        	*/
    XmStringTag		charset;   /* dummy			*/
    XmStringDirection   	direction; /* dummy			*/
    XmStringComponentType	u_tag;	   /* is newline		*/
    int				length;    /* length of string		*/
    unsigned int		u_length;  /* bogus length		*/
    unsigned char		*u_value;  /* bogus value		*/
    XmStringComponentType 	type;	   /* type			*/
    Boolean			done, separator; /* done with it	*/

    if ( !XmStringInitContext(&context, xmstr) )
    {
      XmStringFree(xmstr);
      return(NULL);
    }

    /*
     * First path to get length.
     */
    length = 0;
    if (XmStringPeekNextTriple(context) == XmSTRING_COMPONENT_UNKNOWN) {
      XmStringFree(xmstr);
      XmStringFreeContext(context);
      return(NULL);
    }

    done = False;
    while( !done )
    {
	type = XmStringGetNextTriple(context, &u_length, (XtPointer *)&newText);

        switch( type )
	{
    	case XmSTRING_COMPONENT_TEXT:
	case XmSTRING_COMPONENT_LOCALE_TEXT:
	    length += strlen( newText );
	    break;
 	case XmSTRING_COMPONENT_SEPARATOR:
	    length += 1;
	    break;
	case XmSTRING_COMPONENT_USER_BEGIN:
	case XmSTRING_COMPONENT_USER_END:
	case XmSTRING_COMPONENT_TAG:
	case XmSTRING_COMPONENT_DIRECTION:
	    break;
	case XmSTRING_COMPONENT_END:
	default:
	    done = True;
	}

	XtFree((XtPointer)newText);
   }

    if (!length && (type = XmStringGetNextTriple(context, &u_length, (XtPointer *)&newText))) {
	    text = XtMalloc(u_length + 2);
	    text[0] = '\0';
	    strncat(text, newText, u_length);

	    if (type == XmSTRING_COMPONENT_SEPARATOR) {
		    text[u_length] = '\n';
		    text[u_length + 1] = '\0';
	    }

	    XtFree(newText);
	    XmStringFreeContext(context);
	    return text;
    }

    /*
     * Failed to obtain any compound string, return with NULL pointer.
     */
    if(length == 0) return(NULL);

    XmStringFreeContext( context );
    text = XtMalloc( length + 1 );
    text[0] = '\0';

    /*
     * Fill in the string.
     */
    XmStringInitContext(&context, xmstr);

    done = False;
    while( !done )
    {
	type = XmStringGetNextTriple(context, &u_length, (XtPointer *)&newText);
        switch( type )
	{
    	case XmSTRING_COMPONENT_TEXT:
	case XmSTRING_COMPONENT_LOCALE_TEXT:
	    strcat(text, newText);
	    break;
 	case XmSTRING_COMPONENT_SEPARATOR:
	    strcat(text, "\n");
	    break;
	case XmSTRING_COMPONENT_USER_BEGIN:
	case XmSTRING_COMPONENT_USER_END:
	case XmSTRING_COMPONENT_TAG:
	case XmSTRING_COMPONENT_DIRECTION:
	    break;
	case XmSTRING_COMPONENT_END:
	default:
	    done = True;
	}

	XtFree((XtPointer)newText);
   }

    XmStringFreeContext(context);
    return(text);
}

/*	Function Name: _XiWoveWidget
 *	Description: Wrapper for XtMoveWidget to deal with Motif1.2
 *                   drag N drop.
 *	Arguments: w - The widget to change
 *                 x, y - The new location for this widget.
 *	Returns: none.
 */

void
_XmMoveWidget(Widget w, Position x, Position y)
{
    XmDropSiteStartUpdate(w);
    XtMoveWidget(w, x, y);
    XmDropSiteEndUpdate(w);
}

/*	Function Name: _XmResizeWidget
 *	Description: Wrapper for XtResizeWidget to deal with Motif1.2
 *                   drag N drop.
 *	Arguments: w - The widget to change
 *                 width, height, bw - The new size for the widget.
 *	Returns: none.
 */

void
_XmResizeWidget(Widget w, Dimension width, Dimension height, Dimension bw)
{
    XmDropSiteStartUpdate(w);
    XtResizeWidget(w, width, height, bw);
    XmDropSiteEndUpdate(w);
}

/*	Function Name: _XmConfigureWidget
 *	Description: Wrapper for XtConfigureWidget to deal with Motif1.2
 *                   drag N drop.
 *	Arguments: w - The widget to change
 *                 x, y - The new location for this widget.
 *                 width, height, bw - The new size for the widget.
 *	Returns: none.
 */

void
_XmConfigureWidget(Widget w, Position x, Position y,
		   Dimension width, Dimension height, Dimension bw)
{
    /* 0x0 will sometimes result in a BadValue Error for X_ConfigureWindow */
    if (height < 1) height = 1;
    if (width < 1) width = 1;

    XmDropSiteStartUpdate(w);
    XtConfigureWidget(w, x, y, width, height, bw);
    XmDropSiteEndUpdate(w);

}

/************************************************************
 *
 *  This code is taken from the MIT X Consortium's Xmu
 *  Utility Library.
 *
 ************************************************************/

#define  XK_LATIN1
#include <X11/keysymdef.h>

/*
 * ISO Latin-1 case conversion routine
 */

/*
 * Function:
 *	XmCompareISOLatin1(first, second)
 * Description:
 *	Compares two ISO Latin 1 strings to determine if they are equivalent.
 *	Case is not considered for the comparison.
 * Input:
 *	first  : char * - the first ISO Latin 1 String
 *	second : char * - the second ISO Latin 1 String
 * Output:
 *	int - (-1) -> first < second
 *            ( 0) -> first == second
 *            ( 1) -> first > second
 */
int XmCompareISOLatin1(char *first, char *second)
{
    unsigned char *ap, *bp;

    for (ap = (unsigned char *) first, bp = (unsigned char *) second;
         *ap && *bp; ap++, bp++) {
        unsigned char a, b;

        if ((a = *ap) != (b = *bp)) {
            /* try lowercasing and try again */

            if ((a >= XK_A) && (a <= XK_Z))
		a += (XK_a - XK_A);
            else if ((a >= XK_Agrave) && (a <= XK_Odiaeresis))
              a += (XK_agrave - XK_Agrave);
            else if ((a >= XK_Ooblique) && (a <= XK_Thorn))
              a += (XK_oslash - XK_Ooblique);

            if ((b >= XK_A) && (b <= XK_Z))
              b += (XK_a - XK_A);
            else if ((b >= XK_Agrave) && (b <= XK_Odiaeresis))
              b += (XK_agrave - XK_Agrave);
            else if ((b >= XK_Ooblique) && (b <= XK_Thorn))
              b += (XK_oslash - XK_Ooblique);

            if (a != b) break;
        }
    }
    return (((int) *bp) - ((int) *ap));
}

void XmCopyISOLatin1Lowered(char *dst, char *src)
{
    unsigned char *dest, *source;

    for (dest = (unsigned char *)dst, source = (unsigned char *)src;
	 *source;
	 source++, dest++)
    {
	if ((*source >= XK_A) && (*source <= XK_Z))
	    *dest = *source + (XK_a - XK_A);
	else if ((*source >= XK_Agrave) && (*source <= XK_Odiaeresis))
	    *dest = *source + (XK_agrave - XK_Agrave);
	else if ((*source >= XK_Ooblique) && (*source <= XK_Thorn))
	    *dest = *source + (XK_oslash - XK_Ooblique);
	else
	    *dest = *source;
    }
    *dest = '\0';
}

/*
 *	Creates a stippled pixmap of specified depth
 *	caches these so that multiple requests share the pixmap
 */

#define pixmap_width 2
#define pixmap_height 2

Pixmap
XiCreateStippledPixmap(Screen *screen,
		       Pixel fore, Pixel back, unsigned int depth)
{
    Display *display = DisplayOfScreen(screen);
    CacheEntry *cachePtr;
    Pixmap stippled_pixmap;
    static unsigned char pixmap_bits[] = {
	0x02, 0x01,
    };

    /* see if we already have a pixmap suitable for this screen */
    for (cachePtr = pixmapCache; cachePtr; cachePtr = cachePtr->next) {
	if (cachePtr->screen == screen && cachePtr->foreground == fore &&
	    cachePtr->background == back && cachePtr->depth == depth)
	    return( cachePtr->ref_count++, cachePtr->pixmap );
    }

    stippled_pixmap = XCreatePixmapFromBitmapData (display,
			RootWindowOfScreen(screen), (char *)pixmap_bits,
			pixmap_width, pixmap_height, fore, back, depth);

    /* and insert it at the head of the cache */
    cachePtr = XtNew(CacheEntry);
    cachePtr->screen = screen;
    cachePtr->foreground = fore;
    cachePtr->background = back;
    cachePtr->depth = depth;
    cachePtr->pixmap = stippled_pixmap;
    cachePtr->ref_count = 1;

    _XmProcessLock();
    cachePtr->next = pixmapCache;
    pixmapCache = cachePtr;
    _XmProcessUnlock();
    return( stippled_pixmap );
}

void
XiReleaseStippledPixmap(Screen *screen, Pixmap pixmap)
{
    Display *display = DisplayOfScreen(screen);
    CacheEntry *cachePtr, **prevP;

    _XmProcessLock();
    for (prevP = &pixmapCache, cachePtr = pixmapCache; cachePtr;)
    {
        if (cachePtr->screen == screen && cachePtr->pixmap == pixmap)
        {
	        if (--cachePtr->ref_count == 0)
            {
		        XFreePixmap( display, pixmap );
		        *prevP = cachePtr->next;
		        XtFree( (char*)cachePtr );
		        break;
	        }
	    }
	    prevP = &cachePtr->next;
	    cachePtr = *prevP;
    }
    _XmProcessUnlock();
}

/*
 * Function:
 *	XmCompareXtWidgetGeometryToWidget(geom, widget)
 * Description:
 *	This function compares an XtWidgetGeometry structure to the
 *	actual values contained in a widget.
 * Input:
 *	geom   : XtWidgetGeometry* - the geometry to test against the widget
 *	widget : Widget            - the widget to use in the comparison
 * Output:
 *	Boolean - True if the geometry fits the specified widget, else False
 */
Boolean
XmCompareXtWidgetGeometryToWidget(XtWidgetGeometry *geom, Widget widget)
{
    Boolean returnValue = True;

    _XmProcessLock();
    if( (geom->request_mode & CWX && geom->x != widget->core.x) ||
        (geom->request_mode & CWY && geom->y != widget->core.y) ||
        (geom->request_mode & CWWidth && geom->width != XtHeight(widget)) ||
        (geom->request_mode & CWHeight && geom->height != XtHeight(widget)) ||
        (geom->request_mode & CWBorderWidth && geom->border_width != widget->core.border_width) )
    {
        returnValue = False;
    }
    _XmProcessUnlock();
    return( returnValue );
}

/*
 * Function:
 *	XmCompareXtWidgetGeometry(geom1, geom2)
 * Description:
 *	Compares to XtWidgetGeometry structures to check for equality.
 * Input:
 *	geom1 : XtWidgetGeometry* - geometry to compare
 *	geom2 : XtWidgetGeometry* - geometry to compare
 * Output:
 *	Boolean - True if both the request_mode and values match, else False
 */
Boolean
XmCompareXtWidgetGeometry(XtWidgetGeometry *geom1, XtWidgetGeometry *geom2)
{
    if( geom1->request_mode != geom2->request_mode )
        return( False );

    if( (geom1->request_mode & CWX              && (geom1->x != geom2->x)) ||
        (geom1->request_mode & CWY              && (geom1->y != geom2->y)) ||
        (geom1->request_mode & CWWidth          && (geom1->width != geom2->width)) ||
        (geom1->request_mode & CWHeight         && (geom1->height != geom2->height)) ||
        (geom1->request_mode & CWBorderWidth    && (geom1->border_width != geom2->border_width)) ||
        (geom1->request_mode & CWSibling        && (geom1->sibling != geom2->sibling)) ||
        (geom1->request_mode & CWStackMode      && (geom1->stack_mode != geom2->stack_mode)) )
        return( False );

    return( True );
}

/************************************************************
 *
 *  Static functions
 *
 ************************************************************/

#include <Xm/XmP.h>

/*************************************************************************/
/* The following code is snipped directly from OSF Motif 1.2.4           */
/* DEC did not compile this correctly (ALIGN_SUBCLASS_PARTS was not def- */
/* ined) on all versions of Motif for Digital Unix.  We include it here  */
/* to get correct behavior.                                              */
/*************************************************************************/
/* and now there's a fix added; refer to OSF contact number 24617 for
** details
*/
/* And rewritten to support manual alignment.  HP/UX aligns doubles to
** 8 bytes, but everthing else to 4.  There is NO WAY to discover at
** runtime whether or not a structure contains doubles.  It cannot be
** done, period; thus, the OSF mechanism won't work for PA-RISC
** processors.  We get around that by specifying an extra flag (see
** XiResolvePartOffsets64) saying "align my subpart (only) to 8
** bytes".  This is needed in EPak only for the XiPanner widget.
** Note, this mechanism does NOT work for subclasses of 8-byte-aligned
** subparts.  We'd need to provide a "copy my parents offset array"
** flag for that.  Since EPak doesn't need it, we don't.  Beware...
*/

#define ALIGN_SUBCLASS_PARTS

#ifdef ALIGN_SUBCLASS_PARTS
#undef _ALIGN
#define _ALIGN(size) (((size) + (sizeof(double)-1)) & ~(sizeof(double)-1))
#else
#define _ALIGN(size) (size)
#endif

/*
 *  FIX for 5178: remove dependency on Xt private data
 */
static Boolean
IsSubclassOf(
	WidgetClass wc,
	WidgetClass sc)
{
	WidgetClass p = wc;

    _XmProcessLock();
    while (p && sc && p != sc)
    	p = p->core_class.superclass;
    _XmProcessUnlock();

	return p == sc;
}

#define XtIsConstraintClass(wc) IsSubclassOf(wc, constraintWidgetClass)
/*
 *  end FIX for 5178.
 */

void
_XiResolveAllPartOffsets(
        WidgetClass w_class,
        XmOffsetPtr *offset,
        XmOffsetPtr *constraint_offset,
	Boolean     align64)
{
    WidgetClass c, super = w_class->core_class.superclass;
    ConstraintWidgetClass cc = NULL, scc = NULL;
    int i, classcount = 0;
    XmPartResource *pr;

    Boolean do_align = False;

    _XmProcessLock();
    if (sizeof(int) != sizeof(void*))
	do_align = True;

    /*
     *  Set up constraint class pointers
     */
    if (XtIsConstraintClass(super))
    {
	cc = (ConstraintWidgetClass)w_class;
	scc = (ConstraintWidgetClass)super;
    }

    /*
     *  Update the part size value (initially, it is the size of this part)
     */

    if (do_align)
    {
	w_class->core_class.widget_size =
	    w_class->core_class.widget_size
	    + _ALIGN(super->core_class.widget_size);
	w_class->core_class.widget_size =
	    _ALIGN(w_class->core_class.widget_size);
    }
    else
    {
	w_class->core_class.widget_size =
	    w_class->core_class.widget_size + super->core_class.widget_size;
    }

    /*
     * Another nasty hack.  Just plain old DON'T allow the size to be
     * a multiple of anything smaller than 4.  This causes bus errors on
     * most platforms when the constraint record is appended (by Xt)
     * to the widget struct.  Just round up.
     * The do_align flag above does the same thing, but only on 8 byte
     * boundaries on 64 bit systems.  All of this code desperately
     * needs to be re-written.
     */
    if(w_class->core_class.widget_size & 3) {
	int size = w_class->core_class.widget_size;
	w_class->core_class.widget_size = 4 * ((size / 4) + 1);
    }

    if (cc && scc)
    {
	if (do_align)
	{
	    cc->constraint_class.constraint_size =
		cc->constraint_class.constraint_size
		+ _ALIGN(scc->constraint_class.constraint_size);
	    cc->constraint_class.constraint_size =
		_ALIGN(cc->constraint_class.constraint_size);
	}
	else
	    cc->constraint_class.constraint_size =
		cc->constraint_class.constraint_size +
		scc->constraint_class.constraint_size;
    }

    /*
     *  Count the number of superclasses and allocate the offset record(s)
     */
    for (c = w_class; c != NULL; c = c->core_class.superclass) classcount++;

    *offset = (XmOffsetPtr) XtMalloc(classcount * sizeof(XmOffset));
    if (cc)
	*constraint_offset = (XmOffsetPtr) XtMalloc(classcount
						    * sizeof(XmOffset));
    else
	if(constraint_offset != NULL) *constraint_offset = NULL;

    /*
     *  Fill in the offset table(s) with the offset of all parts
     */
    for (i = classcount-1, c = super; i > 0; c = c->core_class.superclass, i--)
    {
	/*
	 * The do_align flag is true iff _all_ subparts must be 8-byte
	 * aligned (e.g. on an Alpha).
	 *
	 * align64 is true only if the _current_ widget part must be
	 * 8-byte aligned (e.g. subparts containing doubles on HP/UX)
	 */
	if (do_align ||
	    (c == super && align64))
	    (*offset)[i] = (long)_ALIGN((c->core_class.widget_size));
	else
	    (*offset)[i] = (long)(c->core_class.widget_size);
    }

    (*offset)[0] = 0;

    if (constraint_offset != NULL && *constraint_offset != NULL) {
	for (i = classcount-1, scc = (ConstraintWidgetClass) super; i > 0;
	     scc = (ConstraintWidgetClass)(scc->core_class.superclass), i--)
	    if (XtIsConstraintClass((WidgetClass)scc))
	    {
		if (do_align)
		    (*constraint_offset)[i] =
			(long)_ALIGN((scc->constraint_class.constraint_size));
		else
		    (*constraint_offset)[i] =
			(long)(scc->constraint_class.constraint_size);
	    }
	    else
		(*constraint_offset)[i] = 0;

	(*constraint_offset)[0] = 0;
    }

    /*
     *  Update the resource list(s) offsets in place
     */
    for (i = 0; i < w_class->core_class.num_resources; i++)
    {
	pr = (XmPartResource *) &w_class->core_class.resources[i];

	/* The next line updates this in place--be careful */

	w_class->core_class.resources[i].resource_offset =
	    XmGetPartOffset(pr, offset);
    }

    if (cc)
	for (i = 0; i < cc->constraint_class.num_resources; i++)
	{
	    pr = (XmPartResource *) &cc->constraint_class.resources[i];

	    /* The next line updates this in place--be careful */

	    cc->constraint_class.resources[i].resource_offset =
		XmGetPartOffset(pr, constraint_offset);
	}
    _XmProcessUnlock();
}

void
XiResolveAllPartOffsets(
        WidgetClass w_class,
        XmOffsetPtr *offset,
        XmOffsetPtr *constraint_offset )
{
    _XiResolveAllPartOffsets( w_class, offset, constraint_offset, False );
}

void
XmResolveAllPartOffsets64(
        WidgetClass w_class,
        XmOffsetPtr *offset,
        XmOffsetPtr *constraint_offset )
{
#ifdef XM_ALIGN_64
    _XiResolveAllPartOffsets( w_class, offset, constraint_offset, True );
#else
    _XiResolveAllPartOffsets( w_class, offset, constraint_offset, False );
#endif
}


/* -------------------------------------------------------------------------- */




/************************************************************************
 *
 *  The border highlighting and unhighlighting routines.
 *
 *  These routines were originally in Primitive.c but not used anywhere.
 *
 ************************************************************************/

void
_XmExtHighlightBorder(Widget w )
{
XtWidgetProc border_highlight;
    if(XmIsPrimitive(w))
    {
        _XmProcessLock();
        border_highlight = xmPrimitiveClassRec.primitive_class.border_highlight;
        _XmProcessUnlock();

        (*border_highlight) (w);
    }
    else
    {
	    if(XmIsGadget(w))
        {
            _XmProcessLock();
            border_highlight = xmGadgetClassRec.gadget_class.border_highlight;
            _XmProcessUnlock();

            (*border_highlight) (w);
	    }
    }
    return ;
}


void
_XmExtUnhighlightBorder(Widget w)
{
XtWidgetProc border_unhighlight;

    if( XmIsPrimitive(w))
    {
        _XmProcessLock();
        border_unhighlight = xmPrimitiveClassRec.primitive_class.border_unhighlight;
        _XmProcessUnlock();
        (*border_unhighlight)(w) ;
    }
    else
    {
        if(XmIsGadget(w))
        {
            _XmProcessLock();
            border_unhighlight = xmGadgetClassRec.gadget_class.border_unhighlight;
            _XmProcessUnlock();
            (*border_unhighlight)(w);
        }
    }
    return ;
}

