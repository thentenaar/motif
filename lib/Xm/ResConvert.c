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
#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$TOG: ResConvert.c /main/29 1999/05/18 19:19:39 mgreess $"
#endif
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#define X_INCLUDE_STRING_H
#define XOS_USE_XT_LOCKING
#if HAVE_X11_XOS_R_H
#include <X11/Xos_r.h>
#else
#include <Xm/Xmos_r.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <Xm/SpecRenderT.h>
#include <Xm/TraitP.h>
#include <Xm/XmosP.h>
#include "MessagesI.h"
#include "RepTypeI.h"
#include "ResConverI.h"
#include "ResIndI.h"
#include "XmI.h"
#include "XmRenderTI.h"


#define MSG2	_XmMMsgResConvert_0001
#define MSG3    _XmMMsgResConvert_0002
#define MSG4    _XmMMsgResConvert_0003
#define MSG6    _XmMMsgResConvert_0005
#define MSG7    _XmMMsgResConvert_0006
#define MSG12   _XmMMsgResConvert_0011



/********    Static Function Declarations    ********/

static Boolean StringToEntity(
                        Display *disp,
                        XrmValue *args,
                        Cardinal *n_args,
                        XrmValue *from,
                        XrmValue *to,
                        XtPointer *converter_data) ;
static Boolean CvtStringToWidget(
                        Display *disp,
                        XrmValue *args,
                        Cardinal *n_args,
                        XrmValue *from,
                        XrmValue *to,
                        XtPointer *converter_data) ;
static Boolean CvtStringToWindow(
                        Display *disp,
                        XrmValue *args,
                        Cardinal *n_args,
                        XrmValue *from,
                        XrmValue *to,
                        XtPointer *converter_data) ;
static Boolean CvtStringToChar(
                        Display *disp,
                        XrmValue *args,
                        Cardinal *n_args,
                        XrmValue *from,
                        XrmValue *to,
                        XtPointer *converter_data) ;
static Boolean CvtStringToKeySym(
                        Display *display,
                        XrmValue *args,
                        Cardinal *num_args,
                        XrmValue *from,
                        XrmValue *to,
                        XtPointer *converter_data) ;
static void CvtStringToXmStringDestroy(
                        XtAppContext app,
                        XrmValue *to,
                        XtPointer converter_data,
                        XrmValue *args,
                        Cardinal *num_args) ;
static Boolean CvtStringToXmString(
                        Display *display,
                        XrmValue *args,
                        Cardinal *num_args,
                        XrmValue *from,
                        XrmValue *to,
                        XtPointer *converter_data) ;
static void CvtStringToXmFontListDestroy(
                        XtAppContext app,
                        XrmValue *to,
                        XtPointer converter_data,
                        XrmValue *args,
                        Cardinal *num_args) ;
static Boolean CvtStringToXmFontList(
                        Display *display,
                        XrmValue *args,
                        Cardinal *num_args,
                        XrmValue *from,
                        XrmValue *to,
                        XtPointer *converter_data) ;
static Boolean CvtStringToButtonFontList(
                        Display *display,
                        XrmValue *args,
                        Cardinal *num_args,
                        XrmValue *from,
                        XrmValue *to,
                        XtPointer *converter_data) ;
static Boolean CvtStringToLabelFontList(
                        Display *display,
                        XrmValue *args,
                        Cardinal *num_args,
                        XrmValue *from,
                        XrmValue *to,
                        XtPointer *converter_data) ;
static Boolean CvtStringToTextFontList(
                        Display *display,
                        XrmValue *args,
                        Cardinal *num_args,
                        XrmValue *from,
                        XrmValue *to,
                        XtPointer *converter_data) ;
static Boolean GetNextFontListEntry(
                        char **s,
                        char **fontNameRes,
                        char **fontTagRes,
                        XmFontType *fontTypeRes,
                        char *delim) ;
static Boolean GetFontName(
                        char **s,
                        char **name,
                        char *delim) ;
static Boolean GetFontTag(
                        char **s,
                        char **tag,
                        char *delim) ;
static Boolean GetNextXmString(
                        char **s,
                        char **cs) ;
static Boolean CvtStringToXmStringTable(
                        Display *dpy,
                        XrmValuePtr args,
                        Cardinal *num_args,
                        XrmValue *from_val,
                        XrmValue *to_val,
                        XtPointer *data) ;
static void XmStringCvtDestroy(
                        XtAppContext app,
                        XrmValue *to,
                        XtPointer data,
                        XrmValue *args,
                        Cardinal *num_args) ;
static Boolean CvtStringToStringTable(
                        Display *dpy,
                        XrmValuePtr args,
                        Cardinal *num_args,
                        XrmValue *from_val,
                        XrmValue *to_val,
                        XtPointer *data) ;
static void StringCvtDestroy(
                        XtAppContext app,
                        XrmValue *to,
                        XtPointer data,
                        XrmValue *args,
                        Cardinal *num_args) ;
static Boolean CvtStringToCardinalList(
                        Display *dpy,
                        XrmValuePtr args,
                        Cardinal *num_args,
                        XrmValue *from_val,
                        XrmValue *to_val,
                        XtPointer *data) ;
static void CardinalListCvtDestroy(
                        XtAppContext app,
                        XrmValue *to,
                        XtPointer data,
                        XrmValue *args,
                        Cardinal *num_args) ;
static Boolean CvtStringToHorizontalPosition(
                        Display *display,
                        XrmValue *args,
                        Cardinal *num_args,
                        XrmValue *from,
                        XrmValue *to,
                        XtPointer *converter_data) ;
static Boolean CvtStringToHorizontalDimension(
                        Display *display,
                        XrmValue *args,
                        Cardinal *num_args,
                        XrmValue *from,
                        XrmValue *to,
                        XtPointer *converter_data) ;
static Boolean CvtStringToVerticalPosition(
                        Display *display,
                        XrmValue *args,
                        Cardinal *num_args,
                        XrmValue *from,
                        XrmValue *to,
                        XtPointer *converter_data) ;
static Boolean CvtStringToVerticalDimension(
                        Display *display,
                        XrmValue *args,
                        Cardinal *num_args,
                        XrmValue *from,
                        XrmValue *to,
                        XtPointer *converter_data) ;
static void ConvertStringToButtonTypeDestroy(
                        XtAppContext app,
                        XrmValue *to,
                        XtPointer converter_data,
                        XrmValue *args,
                        Cardinal *num_args) ;
static Boolean ConvertStringToButtonType(
                        Display *display,
                        XrmValue *args,
                        Cardinal *num_args,
                        XrmValue *from,
                        XrmValue *to,
                        XtPointer *converter_data) ;
static void CvtStringToKeySymTableDestroy(
                        XtAppContext app,
                        XrmValue *to,
                        XtPointer converter_data,
                        XrmValue *args,
                        Cardinal *num_args) ;
static Boolean CvtStringToKeySymTable(
                        Display *display,
                        XrmValue *args,
                        Cardinal *num_args,
                        XrmValue *from,
                        XrmValue *to,
                        XtPointer *converter_data) ;
static void CvtStringToCharSetTableDestroy(
                        XtAppContext app,
                        XrmValue *to,
                        XtPointer converter_data,
                        XrmValue *args,
                        Cardinal *num_args) ;
static Boolean CvtStringToCharSetTable(
                        Display *display,
                        XrmValue *args,
                        Cardinal *num_args,
                        XrmValue *from,
                        XrmValue *to,
                        XtPointer *converter_data) ;
static Boolean CvtStringToBooleanDimension(
                        Display *display,
                        XrmValue *args,
                        Cardinal *num_args,
                        XrmValue *from,
                        XrmValue *to,
                        XtPointer *converter_data) ;
static Boolean CvtStringToAtomList(
                        Display *dpy,
                        XrmValue *args,
                        Cardinal *num_args,
                        XrmValue *from,
                        XrmValue *to,
                        XtPointer *converter_data) ;
static void SimpleDestructor(
                        XtAppContext app,
                        XrmValue *to,
                        XtPointer data,
                        XrmValue *args,
                        Cardinal *num_args) ;
static Boolean OneOf(
                        char c,
                        char *set) ;
static char * GetNextToken(
                        char *src,
                        char *delim,
			char **context) ;
static Boolean CvtStringToCardinal(
                        Display *display,
                        XrmValue *args,
                        Cardinal *num_args,
                        XrmValue *from,
                        XrmValue *to,
                        XtPointer *converter_data) ;
static Boolean CvtStringToTextPosition(
                        Display *display,
                        XrmValue *args,
                        Cardinal *num_args,
                        XrmValue *from,
                        XrmValue *to,
                        XtPointer *converter_data) ;
static Boolean CvtStringToTopItemPosition(
                        Display *display,
                        XrmValue *args,
                        Cardinal *num_args,
                        XrmValue *from,
                        XrmValue *to,
                        XtPointer *converter_data) ;
static Boolean isInteger(
                        String string,
                        int *value) ;
static Boolean CvtStringToRenditionPixel(Display *disp,
					    XrmValuePtr args,
					    Cardinal *num_args,
					    XrmValue *from_val,
					    XrmValue *to_val,
					    XtPointer *converter_data);
static Boolean CvtPixelToRenditionPixel(Display *disp,
					    XrmValuePtr args,
					    Cardinal *num_args,
					    XrmValue *from_val,
					    XrmValue *to_val,
					    XtPointer *converter_data);
static Boolean CvtStringToSelectColor(Display *disp,
					 XrmValuePtr args,
					 Cardinal *num_args,
					 XrmValue *from_val,
					 XrmValue *to_val,
					 XtPointer *converter_data);
static void CvtStringToXmTabListDestroy(XtAppContext app,
					   XrmValue *to,
					   XtPointer converter_data,
					   XrmValue *args,
					   Cardinal *num_args);
static Boolean GetNextTab(char **s,
			  float *value,
			  char *unitType,
			  XmOffsetModel *offsetModel);
static Boolean CvtStringToXmTabList(Display *dpy,
				       XrmValue *args,
				       Cardinal *num_args,
				       XrmValue *from,
				       XrmValue *to,
				       XtPointer *converter_data);
static void CvtStringToXmRenderTableDestroy(XtAppContext app,
					       XrmValue *to,
					       XtPointer converter_data,
					       XrmValue *args,
					       Cardinal *num_args);
static Boolean CvtStringToRenderTable(Display *dpy,
					 XrmValue *args,
					 Cardinal *num_args,
					 XrmValue *from,
					 XrmValue *to,
					 XtPointer *converter_data);
static Boolean CvtStringToButtonRenderTable(Display *dpy,
					       XrmValue *args,
					       Cardinal *num_args,
					       XrmValue *from,
					       XrmValue *to,
					       XtPointer *converter_data);
static Boolean CvtStringToLabelRenderTable(Display *dpy,
					      XrmValue *args,
					      Cardinal *num_args,
					      XrmValue *from,
					      XrmValue *to,
					      XtPointer *converter_data);
static Boolean CvtStringToTextRenderTable(Display *dpy,
					     XrmValue *args,
					     Cardinal *num_args,
					     XrmValue *from,
					     XrmValue *to,
					     XtPointer *converter_data);

static void _XmGetDisplayArg(Widget widget,
					Cardinal *size,
					XrmValue *value);


/********    End Static Function Declarations    ********/


static XtConvertArgRec selfConvertArgs[] = {
    { XtBaseOffset, (XtPointer) 0, sizeof(int) }
};

static XtConvertArgRec  displayConvertArg[] = {
    {XtProcedureArg, (XtPointer)_XmGetDisplayArg, 0},
};

/* Motif widget set version number.  Accessable by applications via Xm.h. */

externaldef(xmuseversion) int xmUseVersion = XmVersion;


/************************************************************************
 *
 *  _XmGetDisplayArg
 *
 * Function used to allow Fonts to be per display
 *
 ************************************************************************/
static void _XmGetDisplayArg(Widget widget, Cardinal *size, XrmValue *value)
{
    if (widget == NULL)
            XtErrorMsg("missingWidget", "_XmGetDisplayArg", "XtToolkitError",
                 "_XmGetDisplayArg called without a widget to reference",
                 (String*)NULL, (Cardinal*)NULL );
        /* can't return any useful Display and caller will de-ref NULL,
           so aborting is the only useful option */

    value->size = sizeof(Display*);
    value->addr = (XPointer)&DisplayOfScreen(XtScreenOfObject(widget));
}


/************************************************************************
 *
 *  _XmRegisterConverters
 *	Register all of the Xm resource type converters.  Retain a
 *	flag indicating whether the converters have already been
 *	registered.
 *
 ************************************************************************/
void
_XmRegisterConverters( void )
{
    static Boolean registered = False ;

    _XmProcessLock();
    if(    !registered    )
    {
        _XmRepTypeInstallConverters() ;

        XtSetTypeConverter( XmRString, XmRWidget, CvtStringToWidget,
                            selfConvertArgs, XtNumber(selfConvertArgs),
                            XtCacheNone, (XtDestructor) NULL) ;
        XtSetTypeConverter( XmRString, XmRWindow, CvtStringToWindow,
                            selfConvertArgs, XtNumber(selfConvertArgs),
                            XtCacheNone, (XtDestructor) NULL) ;
        XtSetTypeConverter( XmRString, XmRChar, CvtStringToChar, NULL, 0,
			   XtCacheNone, NULL) ;
        XtSetTypeConverter( XmRString, XmRFontList, CvtStringToXmFontList,
                            displayConvertArg,  XtNumber(displayConvertArg),
                            XtCacheByDisplay, CvtStringToXmFontListDestroy);
        XtSetTypeConverter( XmRString, XmRXmString, CvtStringToXmString,
			    NULL, 0, (XtCacheNone | XtCacheRefCount),
			    CvtStringToXmStringDestroy ) ;
        XtSetTypeConverter( XmRString, XmRKeySym, CvtStringToKeySym,
			   NULL, 0, XtCacheNone, NULL) ;

        XtSetTypeConverter( XmRString, XmRHorizontalPosition,
                           CvtStringToHorizontalPosition, selfConvertArgs,
                             XtNumber( selfConvertArgs), XtCacheNone, NULL) ;
        XtSetTypeConverter( XmRString, XmRHorizontalDimension,
                          CvtStringToHorizontalDimension, selfConvertArgs,
                             XtNumber( selfConvertArgs), XtCacheNone, NULL) ;
        XtSetTypeConverter( XmRString, XmRVerticalPosition,
                             CvtStringToVerticalPosition, selfConvertArgs,
                             XtNumber( selfConvertArgs), XtCacheNone, NULL) ;
        XtSetTypeConverter( XmRString, XmRVerticalDimension,
                            CvtStringToVerticalDimension, selfConvertArgs,
                             XtNumber( selfConvertArgs), XtCacheNone, NULL) ;
        XtSetTypeConverter( XmRString, XmRBooleanDimension,
                             CvtStringToBooleanDimension, selfConvertArgs,
                             XtNumber( selfConvertArgs), XtCacheNone, NULL) ;

        XtSetTypeConverter( XmRCompoundText, XmRXmString, XmCvtTextToXmString,
			   NULL, 0, XtCacheNone, NULL) ;
        XtSetTypeConverter( XmRXmString, XmRCompoundText, XmCvtXmStringToText,
                           NULL, 0, XtCacheNone, NULL) ;

        XtSetTypeConverter( XmRString, XmRCharSetTable,
			   CvtStringToCharSetTable, NULL, 0, XtCacheNone,
			   CvtStringToCharSetTableDestroy) ;
        XtSetTypeConverter( XmRString, XmRKeySymTable,
			   CvtStringToKeySymTable, NULL, 0, XtCacheNone,
			   CvtStringToKeySymTableDestroy) ;
        XtSetTypeConverter( XmRString, XmRButtonType,
			   ConvertStringToButtonType, NULL, 0, XtCacheNone,
			   ConvertStringToButtonTypeDestroy) ;
        XtSetTypeConverter( XmRString, XmRXmStringTable,
			   CvtStringToXmStringTable, NULL, 0,
			   (XtCacheNone | XtCacheRefCount),
			   XmStringCvtDestroy) ;
        XtSetTypeConverter (XmRString, XmRStringTable,
			    CvtStringToStringTable, NULL, 0,
			    (XtCacheNone | XtCacheRefCount),
			    StringCvtDestroy) ;
	XtSetTypeConverter( XmRString, XmRCardinalList,
                        CvtStringToCardinalList, NULL, 0,
                        XtCacheNone, CardinalListCvtDestroy) ;
        XtSetTypeConverter( XmRString, XmRAtomList,
                    CvtStringToAtomList, NULL, 0,
                      (XtCacheNone | XtCacheRefCount), SimpleDestructor) ;
        XtSetTypeConverter( XmRString, XmRCardinal,
                            CvtStringToCardinal, NULL, 0,
			    XtCacheNone, NULL) ;
        XtSetTypeConverter( XmRString, XmRTextPosition,
                            CvtStringToTextPosition, NULL, 0,
                            XtCacheNone, NULL) ;
        XtSetTypeConverter( XmRString, XmRTopItemPosition,
                             CvtStringToTopItemPosition, NULL, 0,
                             XtCacheNone, NULL) ;
	XtSetTypeConverter(XmRString, XmRRenditionPixel,
			   CvtStringToRenditionPixel,
			   (XmConst XtConvertArgList)colorConvertArgs, 2,
			   XtCacheByDisplay, NULL);

	/* also set a converter from Pixel to RenditionPixel so that
	   the ColorObject setting as Pixel directly in the resource
	   Database be taken for rendition background and foreground */
	XtSetTypeConverter(XmRPixel, XmRRenditionPixel,
			   CvtPixelToRenditionPixel, NULL, 0,
			   XtCacheByDisplay, NULL);

	XtSetTypeConverter(XmRString, XmRSelectColor,
			   CvtStringToSelectColor,
			   (XmConst XtConvertArgList)colorConvertArgs, 2,
			   XtCacheByDisplay, NULL);

	XtSetTypeConverter(XmRString, XmRTabList,
			   CvtStringToXmTabList, NULL, 0,
			   (XtCacheAll | XtCacheRefCount),
			   CvtStringToXmTabListDestroy);
        XtSetTypeConverter(XmRString, XmRRenderTable,
			   CvtStringToRenderTable,
			   selfConvertArgs, XtNumber(selfConvertArgs),
			   (XtCacheNone | XtCacheRefCount),
			   CvtStringToXmRenderTableDestroy);
        XtSetTypeConverter(XmRString, XmRButtonRenderTable,
			   CvtStringToButtonRenderTable,
			   selfConvertArgs, XtNumber(selfConvertArgs),
			   (XtCacheNone | XtCacheRefCount),
			   CvtStringToXmRenderTableDestroy);
        XtSetTypeConverter(XmRString, XmRLabelRenderTable,
			   CvtStringToLabelRenderTable,
			   selfConvertArgs, XtNumber(selfConvertArgs),
			   (XtCacheNone | XtCacheRefCount),
			   CvtStringToXmRenderTableDestroy);
        XtSetTypeConverter(XmRString, XmRTextRenderTable,
			   CvtStringToTextRenderTable,
			   selfConvertArgs, XtNumber(selfConvertArgs),
			   (XtCacheNone | XtCacheRefCount),
			   CvtStringToXmRenderTableDestroy);

        XtSetTypeConverter(XmRString, XmRButtonFontList,
			   CvtStringToButtonFontList,
			   selfConvertArgs, XtNumber(selfConvertArgs),
			   (XtCacheNone | XtCacheRefCount),
			   CvtStringToXmFontListDestroy);
        XtSetTypeConverter(XmRString, XmRLabelFontList,
			   CvtStringToLabelFontList,
			   selfConvertArgs, XtNumber(selfConvertArgs),
			   (XtCacheNone | XtCacheRefCount),
			   CvtStringToXmFontListDestroy);
        XtSetTypeConverter(XmRString, XmRTextFontList,
			   CvtStringToTextFontList,
			   selfConvertArgs, XtNumber(selfConvertArgs),
			   (XtCacheNone | XtCacheRefCount),
			   CvtStringToXmFontListDestroy);

        registered = True;
        }
    _XmProcessUnlock();
    return ;
}



/************************************************************************
 *
 *  XmeNamesAreEqual
 *	Compare two strings and return true if equal.
 *	The comparison is on lower cased strings.  It is the callers
 *	responsibility to ensure that test_str is already lower cased.
 *
 ************************************************************************/
Boolean
XmeNamesAreEqual(
        register char *in_str,
        register char *test_str )
{
        register char i ;

    if(    ((in_str[0] == 'X') || (in_str[0] == 'x'))
        && ((in_str[1] == 'M') || (in_str[1] == 'm'))    )
    {
        in_str +=2;
        }
    do
    {
 /*
  * Fix for 5330 - For OS compatibility with old operating systems, always
  *                check a character with isupper before using tolower on it.
  */
        if (isupper((unsigned char)*in_str))
            i = (char) tolower((unsigned char) *in_str) ;
        else
            i = *in_str;
        in_str++;

        if(    i != *test_str++    )
        {
            return( False) ;
            }
    }while(    i    ) ;

    return( True) ;
    }



/************************************************************************
 *
 *  StringToEntity
 *    Allow widget or window to be specified by name
 *
 ************************************************************************/
static Boolean
StringToEntity(
        Display *disp,
        XrmValue *args,
        Cardinal *n_args,
        XrmValue *from,
        XrmValue *to,
        XtPointer *converter_data) /* unused */
{
    Widget child;
    Widget widget = *(Widget*) args[0].addr ;
    static Widget  itsChild;
    Boolean        success;

    if (*n_args != 1)
      XtAppWarningMsg (
            XtDisplayToApplicationContext(disp),
            "wrongParameters", "cvtStringToWidget", "XtToolkitError",
            MSG12, (String*)NULL, (Cardinal*)NULL );

    /* handle the XmSELF case */
    if (XmeNamesAreEqual ((String) from->addr, "self"))
	child  = widget ;
    else {
	child  = XtNameToWidget(XtParent(widget), (String)from->addr);
    }

    success   = !( child == NULL );

    if ( success )
    {
        if (to->addr == NULL) {
           itsChild = child;
           to->addr = (XPointer) &itsChild;
	}

        else if (to->size < sizeof(Widget))
            success  = FALSE;

        else
            *(Widget*) to->addr = child;

        to->size = sizeof(Widget);
    }
    else
        XtDisplayStringConversionWarning(disp, from->addr, "Widget");

    return ( success );
}

/************************************************************************
 *
 *  CvtStringToWidget
 *    Allow widget to be specified by name
 *
 ************************************************************************/
static Boolean
CvtStringToWidget(
        Display *disp,
        XrmValue *args,
        Cardinal *n_args,
        XrmValue *from,
        XrmValue *to,
        XtPointer *converter_data)
{
    return (StringToEntity( disp, args, n_args, from, to, converter_data ) );
}

/************************************************************************
 *
 *  CvtStringToWindow
 *    Allow widget(Window) to be specified by name
 *
 ************************************************************************/
static Boolean
CvtStringToWindow(
        Display *disp,
        XrmValue *args,
        Cardinal *n_args,
        XrmValue *from,
        XrmValue *to,
        XtPointer *converter_data)
{
    return (StringToEntity( disp, args, n_args, from, to, converter_data ) );
}



/************************************************************************
 *
 *  CvtStringToChar
 *	Convert string to a single character (a mnemonic)
 *
 ************************************************************************/
static Boolean
CvtStringToChar(
        Display *disp,		/* unused */
        XrmValue *args,		/* unused */
        Cardinal *n_args,	/* unused */
        XrmValue *from,
        XrmValue *to,
        XtPointer *converter_data) /* unused */
{
   unsigned char in_char = *((unsigned char *) (from->addr)) ;

   _XM_CONVERTER_DONE( to, unsigned char, in_char, ; )
   }


/************************************************************************
 *
 *   CvtStringToKeySym
 *	Convert a string to a KeySym
 *
 ************************************************************************/
static Boolean
CvtStringToKeySym(
        Display *display,
        XrmValue *args,		/* unused */
        Cardinal *num_args,	/* unused */
        XrmValue *from,
        XrmValue *to,
        XtPointer *converter_data) /* unused */
{
    KeySym tmpKS = XStringToKeysym( (char *) (from->addr)) ;

    if(    tmpKS != NoSymbol    )
    {
        _XM_CONVERTER_DONE( to, KeySym, tmpKS, ; )
        }
    XtDisplayStringConversionWarning(display, (char *) from->addr, XmRKeySym) ;

    return( FALSE) ;
    }

static void
CvtStringToXmStringDestroy(
        XtAppContext app,	/* unused */
        XrmValue *to,
        XtPointer converter_data, /* unused */
        XrmValue *args,		/* unused */
        Cardinal *num_args)	/* unused */
{
    XmStringFree( *((XmString *) to->addr)) ;
    return ;
    }

/************************************************************************
 *
 *  CvtStringToXmString
 *	Convert an ASCII string to a XmString.
 *
 ************************************************************************/
static Boolean
CvtStringToXmString(
        Display *display,
        XrmValue *args,		/* unused */
        Cardinal *num_args,	/* unused */
        XrmValue *from,
        XrmValue *to,
        XtPointer *converter_data) /* unused */
{
        XmString tmpStr ;

    if(    from->addr    )
    {
        tmpStr = XmStringGenerate((char *)from->addr,
				  XmFONTLIST_DEFAULT_TAG,
				  XmCHARSET_TEXT, NULL);
        if(    tmpStr    )
        {
            _XM_CONVERTER_DONE( to, XmString, tmpStr, XmStringFree( tmpStr) ; )
            }
        }
    XtDisplayStringConversionWarning(display, ((char *) from->addr),
				     XmRXmString) ;

    return( FALSE) ;
    }

static void
CvtStringToXmFontListDestroy(
        XtAppContext app,	/* unused */
        XrmValue *to,
        XtPointer converter_data, /* unused */
        XrmValue *args,		/* unused */
        Cardinal *num_args)	/* unused */
{
    XmFontListFree( *((XmFontList *) to->addr)) ;

    return ;
    }

/************************************************************************
 *
 *  CvtStringToXmFontList
 *	Convert a string to a fontlist.  This is in the form :
 *
 *  <XmFontList>	::=	<fontlistentry> { ',' <fontlistentry> }
 *
 *  <fontlistentry>	::=	<fontset> | <font>
 *
 *  <fontset>		::=	<fontname> { ';' <fontname> } ':' [ <tag> ]
 *
 *  <font>		::=	<fontname> [ '=' <tag> ]
 *
 *  <fontname>		::=	<XLFD String>
 *
 *  <tag>		::=	<characters from ISO646IRV except newline>
 *
 *
 *  Additional syntax is allowed for compatibility with Xm1.1:
 *
 *  1. The fontlistentries may be separated by whitespace, rather than ','.
 *  2. Empty fontlistentries are ignored.
 *
 ************************************************************************/

static Boolean
CvtStringToXmFontList(
        Display *dpy,
        XrmValue *args,		/* unused */
        Cardinal *num_args,	/* unused */
        XrmValue *from,
        XrmValue *to,
        XtPointer *converter_data) /* unused */
{
  Boolean got_it = FALSE;
  char *s;
  char *newString;
  char *sPtr;
  char *fontName;
  char *fontTag;
  XmFontType fontType;
  char delim;
  XmFontListEntry fontListEntry;
  XmFontList      fontList = NULL;

  if (from->addr)
    {
      /* Copy the input string. */
      s = (char *) from->addr;
      sPtr = newString = XtNewString(s);

      /* Get the first fontlist entries. */
      if (!GetNextFontListEntry(&sPtr, &fontName, &fontTag,
				&fontType, &delim))
	{
	  XtFree(newString);
	  s = (char *) XmDEFAULT_FONT;
	  sPtr = newString = XtNewString(s);


	  if (!GetNextFontListEntry(&sPtr, &fontName, &fontTag,
				    &fontType, &delim))
	    {
	      XtFree(newString);
	      XmeWarning(NULL, MSG2);
	      exit(1);
	    }
	}

      /* Parse additional font list entries. */
      do {
	if (*fontName)
	  {
	    fontListEntry = XmFontListEntryLoad(dpy, fontName,
						fontType, fontTag);
	    if (fontListEntry != NULL)
	      {
		got_it = TRUE;
		fontList = XmFontListAppendEntry(fontList, fontListEntry);
		XmFontListEntryFree(&fontListEntry);
	      }
	    else
	      XtDisplayStringConversionWarning(dpy, fontName, XmRFontList);
	  }
      } while ((delim == ',') && *++sPtr &&
	       GetNextFontListEntry(&sPtr, &fontName, &fontTag,
				    &fontType, &delim));
      XtFree(newString);
    }

  if (got_it)
    {
      _XM_CONVERTER_DONE(to, XmFontList, fontList, XmFontListFree(fontList);)
    }

  XtDisplayStringConversionWarning(dpy, (char *) from->addr, XmRFontList);
  return FALSE;
}


static Boolean
CvtStringToButtonFontList(Display *dpy,
				XrmValue *args,
				Cardinal *num_args, /* unused */
				XrmValue *from,
				XrmValue *to,
				XtPointer *converter_data) /* unused */
{

  return(CvtStringToXmFontList(dpy, args, num_args, from, to,
			       converter_data));
}

static Boolean
CvtStringToLabelFontList(Display *dpy,
			       XrmValue *args,
			       Cardinal *num_args, /* unused */
			       XrmValue *from,
			       XrmValue *to,
			       XtPointer *converter_data) /* unused */
{

  return(CvtStringToXmFontList(dpy, args, num_args, from, to,
			       converter_data));
}

static Boolean
CvtStringToTextFontList(Display *dpy,
			       XrmValue *args,
			       Cardinal *num_args, /* unused */
			       XrmValue *from,
			       XrmValue *to,
			       XtPointer *converter_data) /* unused */
{
  return(CvtStringToXmFontList(dpy, args, num_args, from, to,
			       converter_data));
}

/************************************************************************
 *
 *  GetNextFontListEntry
 *
 ************************************************************************/
static Boolean
GetNextFontListEntry (
    char **s ,
    char **fontNameRes ,
    char **fontTagRes ,
    XmFontType *fontTypeRes ,
    char *delim )
{
    char *fontName;
    char *fontTag = XmFONTLIST_DEFAULT_TAG;
    char *fontPtr;
    String params[2];
    Cardinal num_params;

    *fontTypeRes = XmFONT_IS_FONT;

    /*
     * Parse the fontname or baselist.
     */

    if (!GetFontName(s, &fontName, delim))
    {
	return (FALSE);
    }

    while (*delim == ';')
    {
        *fontTypeRes = XmFONT_IS_FONTSET;

        **s = ',';
        (*s)++;

        if (!GetFontName(s, &fontPtr, delim))
        {
	    return (FALSE);
        }
    }

    /*
     * Parse the fontsettag or fonttag.
     */

    if (*delim == ':')
    {
        *fontTypeRes = XmFONT_IS_FONTSET;

	(*s)++;
        if (!GetFontTag(s, &fontTag, delim))
        {
	    fontTag = XmFONTLIST_DEFAULT_TAG;
        }
    }
    else
    {
	if (*fontTypeRes == XmFONT_IS_FONTSET)
	{
	    /* CR4721 */
            params[0] = fontName;
	    num_params = 1;
	    XtWarningMsg("conversionWarning", "string", "XtToolkitError",
			 MSG3, params, &num_params);

	    return (FALSE);
	}

        if (*delim == '=')
        {
	    (*s)++;
            if (!GetFontTag(s, &fontTag, delim))
            {
	        return (FALSE);
            }
        }
	else if ((*delim == ',') || *delim == '\0')
	{
	    fontTag = XmFONTLIST_DEFAULT_TAG;
	}
	else
        {
	    /* CR4721 */
	    params[0] = fontTag;
	    num_params = 1;
	    XtWarningMsg("conversionWarning", "string", "XtToolkitError",
			 MSG4, params, &num_params);

	    return (FALSE);
        }
    }
    *fontNameRes = fontName;
    *fontTagRes = fontTag;
    return (TRUE);
}

/************************************************************************
 *
 *  GetFontName
 *
 *
 *  May return null string as fontname (Xm1.1 compatibility).
 ************************************************************************/
static Boolean
GetFontName (
    char **s,
    char **name,
    char *delim )
{
    String params[2];
    Cardinal num_params;

    /*
     * Skip any leading whitespace.
     */

    while (**s != '\0' && isspace((unsigned char)**s))
    {
	(*s)++;
    }
    if (**s == '\0')
    {
        return (FALSE);
    }

    /*
     * Have nonspace.  Find the end of the name.
     */

    *name = *s;
    if (**s == '"')
    {
        (*name)++;
        (*s)++;
        while (**s != '\0' && (**s != '"'))
	{
            (*s)++;
	}
        if (**s == '\0')
        {
	  /* CR4721 */
	  params[0] = --(*name);
	  num_params = 1;
	  XtWarningMsg("conversionWarning", "string", "XtToolkitError",
		       MSG6, params, &num_params);
	  return (FALSE);
        }
        **s = '\0';
        (*s)++;
	*delim = **s;
    }
    else
    {
        while ((**s != '\0') &&
	       (**s != ',') && (**s != ':') && (**s != ';') && (**s != '='))
	{
	      (*s)++;
	}
	*delim = **s;
        **s = '\0';
    }

    return (TRUE);
}

/************************************************************************
 *
 *  GetFontTag
 *
 ************************************************************************/
static Boolean
GetFontTag (
    char **s,
    char **tag,
    char *delim )
{
    String params[2];
    Cardinal num_params;
    Boolean needs_tag = (*delim == '=');

    /*
     * Skip any leading whitespace.
     */

    while (**s != '\0' && isspace((unsigned char)**s))
    {
	(*s)++;
    }
    if (**s == '\0')
    {
        return (FALSE);
    }

    /*
     * Have nonspace.  Find the end of the tag.
     */

    *tag = *s;
    if (**s == '"')
    {
        (*tag)++;
        (*s)++;
        while (**s != '\0' && (**s != '"'))
	{
            (*s)++;
	}
        if (**s == '\0')
        {
	  /* CR4721 */
	  params[0] = --(*tag);
	  num_params = 1;
	  XtWarningMsg("conversionWarning", "string", "XtToolkitError",
		       MSG6, params, &num_params);
	  return (FALSE);
        }
        **s = '\0';
        (*s)++;
	*delim = **s;
    }
    else
    {
        while (!isspace((unsigned char)**s) && (**s != ',') && (**s != '\0'))
	{
	    (*s)++;
	}
	/* Xm1.1 compatibility */
	*delim = isspace ((unsigned char)**s) ? ',' : **s;
        **s = '\0';
    }

    /* Null tags are not accepted. */

    if (*s == *tag)
    {
        if (needs_tag) {
	  /* CR4721 */
	  params[0] = XmRFontList;
	  num_params = 1;
	  XtWarningMsg("conversionWarning", "string", "XtToolkitError",
		       MSG7, params, &num_params);
	}
        return (FALSE);
    }

    return (TRUE);
}

/************************************************************************
 *									*
 * GetNextXmString - return a pointer to a null-terminated string.	*
 *                   The pointer is passed in cs. Up to the caller to   *
 *    		     free that puppy. Returns FALSE if end of string.	*
 *									*
 ************************************************************************/
static Boolean
GetNextXmString(
        char **s,
        char **cs )
{
   char *tmp;
   int csize;

   if (**s == '\0')
      return(FALSE);


   /*  Skip any leading whitespace.  */

   while(isspace((unsigned char)**s) && **s != '\0') (*s)++;

   if (**s == '\0')
      return(FALSE);


   /* Found something. Allocate some space (ugh!) and start copying  */
   /* the next string                                                */

   *cs = XtMalloc(strlen(*s) + 1);
   tmp = *cs;

   while((**s) != '\0')
   {
      if ((**s) == '\\' && *((*s)+1) == ',')	/* Quoted comma */
      {
         (*s)+=2;
         *tmp = ',';
         tmp++;
      }
      else
      {
         if((**s) == ',')			/* End of a string */
         {
            *tmp = '\0';
            (*s)++;
            return(TRUE);
         }
         else
         {
	    if (MB_CUR_MAX > 1) {
	      if ((csize = mblen(*s, MB_CUR_MAX)) < 0)
	        break;
	      strncpy(tmp, *s, csize);
	      tmp += csize;
	      (*s) += csize;
	    } else {
	      *tmp = **s;
	      tmp++;
	      (*s)++;
	    }
         }
       }
    }

    *tmp = '\0';
    return(TRUE);
}

/************************************************************************
 *
 * CvtStringToXmStringTable
 *
 * Convert a string table to an array of XmStrings.This is in the form :
 *
 *       		String [, String2]*
 *
 * The comma delimeter can be  quoted by a \
 *
 ************************************************************************/
static Boolean
CvtStringToXmStringTable(
        Display *dpy,
        XrmValuePtr args,
        Cardinal *num_args,
        XrmValue *from_val,
        XrmValue *to_val,
        XtPointer *data )
{
  char  *s, *cs;
  XmString *table;
  static  XmString *tblptr;
  int	  str_no, table_size;

  if (from_val->addr == NULL)
    return FALSE;

  s = (char *) from_val->addr;
  table_size = 100;
  table = (XmString *) XtMalloc(sizeof(XmString) * table_size);
  for (str_no = 0; GetNextXmString(&s, &cs); str_no++)
    {
      if (str_no >= table_size)
	{
	  table_size *= 2;
	  table = (XmString *)XtRealloc((char *)table,
					sizeof(XmString) * table_size);
	}
      table[str_no] = XmStringGenerate(cs, XmFONTLIST_DEFAULT_TAG,
				       XmCHARSET_TEXT, NULL);
      XtFree(cs);
    }

  /* NULL terminate the array... */
  table_size = str_no + 1;
  table = (XmString *)XtRealloc((char *) table, sizeof(XmString) * table_size);
  table[str_no] = (XmString) NULL;

  if (to_val->addr != NULL)
    {
      if (to_val->size < sizeof(XtPointer))
	{
	  to_val->size = sizeof(XtPointer);
	  return False;
	}
      *(XmString **)(to_val->addr) = table;
    }
  else
    {
      tblptr = table;
      to_val->addr = (XPointer)&tblptr;
    }
  to_val->size = sizeof(XtPointer);
  return TRUE;
}

/****************
 *
 * XmStringCvtDestroy - free up the space allocated by the converter
 *
 ****************/
static void
XmStringCvtDestroy(
        XtAppContext app,	/* unused */
        XrmValue *to,
        XtPointer data,		/* unused */
        XrmValue *args,		/* unused */
        Cardinal *num_args )	/* unused */
{
   int i;
   XmString *table = *(XmString **)(to->addr);
   for (i = 0; table[i] != NULL; i++)
       XmStringFree(table[i]);
   XtFree((char*)table);
}

static Boolean
CvtStringToStringTable(
        Display *dpy,		/* unused */
        XrmValuePtr args,	/* unused */
        Cardinal *num_args,	/* unused */
        XrmValue *from_val,
        XrmValue *to_val,
        XtPointer *data)	/* unused */
{
    register char *p ;
            char *top ;
            String *table ;
    static String *tblptr ;
            int size = 50 ;
            int i, len ;
            int csize;

    if(    (p = from_val->addr) == NULL    )
    {   return( False) ;
        }
    table = (String *) XtMalloc( sizeof( String) * size) ;

    for(    i = 0 ; *p ; i++    )
    {
        while(    isspace((unsigned char) *p) && *p != '\0'    )
        {   p++ ;
            }
        if(    *p == '\0'    )
        {
            if(    i == size    )
            {
                size++ ;
                table = (String *)XtRealloc( (char *) table,
                                                      sizeof( String) * size) ;
                }
            table[i] = XtMalloc( sizeof( char)) ;
            *(table[i]) = '\0' ;

            break ;
            }
        for(    top = p ; *p != ',' && *p != '\0' ; p+=csize    )
        {
            if(    *p == '\\' && *(p + 1) == ','    )
            {   p++ ;
                }
	    if((csize = mblen(p, MB_CUR_MAX)) < 0)
 	      break;
            }
        if(    i == size    )
        {
            size *= 2 ;
            table = (String *)XtRealloc( (char *) table,
                                                      sizeof( String) * size) ;
            }
        len = p - top ;
        table[i] = XtMalloc( len + 1) ;
        strncpy( table[i], top, len) ;
	(table[i])[len] = '\0' ;
        if (*p != '\0') p++ ;
        }
    table = (String *)XtRealloc( (char *) table, sizeof( String) * (i + 1)) ;
    table[i] = NULL ;

    if(    to_val->addr != NULL    )
    {
        if(    to_val->size < sizeof( XPointer)    )
        {
            to_val->size = sizeof( XPointer) ;
            return( False) ;
            }
        *(String **)(to_val->addr) = table ;
        }
    else
    {   tblptr = table ;
        to_val->addr = (XPointer)&tblptr ;
        }
    to_val->size = sizeof( XPointer) ;
    return( True) ;
    }

static void
StringCvtDestroy(
        XtAppContext app,	/* unused */
        XrmValue *to,
        XtPointer data,		/* unused */
        XrmValue *args,		/* unused */
        Cardinal *num_args)	/* unused */
{
            int i ;
            String *table = * (String **) (to->addr) ;

    for(    i = 0 ; table[i] != NULL ; i++    )
    {   XtFree( (char *) table[i]) ;
        }
    XtFree( (char *) table) ;

    return ;
    }

static Boolean
CvtStringToCardinalList(
    Display *dpy,		/* unused */
    XrmValuePtr args,	/* unused */
    Cardinal *num_args,	/* unused */
    XrmValue *from_val,
    XrmValue *to_val,
    XtPointer *data)	/* unused */
{
    register char *	p;
    Cardinal *		crd_array;
    int             	crd_array_size = 50;
    int			crd_array_count = 0;
    int			new_element;

    if ((p = from_val->addr) == NULL)
        return(False);

    crd_array = (Cardinal *)XtCalloc
			(crd_array_size,sizeof(Cardinal));
    while (*p != '\0')
	{
	while ((isspace(*p) || ispunct(*p)) && *p != '\0') /* Skip blanks */
	    p++;
	if (*p == '\0')				/* end-of data */
	    break;
	if (isdigit(*p))
	    {
	    new_element = atoi(p);		/* Grab number */
	    while (isdigit(*p))		/* advance pointer past number */
		p++;
	    if (crd_array_size == crd_array_count)
		{
		crd_array_size *= 2;		/* Double array size */
		crd_array = (Cardinal *)XtRealloc((char *)crd_array,
				sizeof(Cardinal) * crd_array_size);
		}
	    crd_array[crd_array_count] = new_element;
	    crd_array_count++;
	    }
	else
	    p++;
	}

        _XM_CONVERTER_DONE(to_val,Cardinal *,crd_array,;)
}

static void
CardinalListCvtDestroy(
        XtAppContext app,	/* unused */
        XrmValue *to,
        XtPointer data,		/* unused */
        XrmValue *args,		/* unused */
        Cardinal *num_args)	/* unused */
{
        XtFree((XPointer)to->addr);
}

static Boolean
CvtStringToHorizontalPosition(
        Display *display,
        XrmValue *args,
        Cardinal *num_args,	/* unused */
        XrmValue *from,
        XrmValue *to,
        XtPointer *converter_data) /* unused */
{
    Widget widget = *(Widget*) args[0].addr ;
    Screen * screen = XtScreen(widget) ;
    unsigned char defaultFromType = _XmGetUnitType(widget) ;
    Position tmpPix;
    Boolean parseError;

    tmpPix = (Position)
	_XmConvertStringToUnits (screen, from->addr, (int) defaultFromType,
			       XmHORIZONTAL, XmPIXELS, (XtEnum*) &parseError);
    if (parseError) {
        XtDisplayStringConversionWarning(display, (char *)from->addr,
					 XmRHorizontalPosition);
        return False;
    }
    else
        _XM_CONVERTER_DONE( to, Position, tmpPix, ; )
}

static Boolean
CvtStringToHorizontalDimension(
        Display *display,
        XrmValue *args,
        Cardinal *num_args,	/* unused */
        XrmValue *from,
        XrmValue *to,
        XtPointer *converter_data) /* unused */
{
    Widget widget = *(Widget*) args[0].addr ;
    Screen * screen = XtScreen(widget) ;
    unsigned char defaultFromType = _XmGetUnitType(widget) ;
    Dimension tmpPix;
    Boolean parseError;

    tmpPix = (Dimension)
      _XmConvertStringToUnits (screen, from->addr, (int) defaultFromType,
			       XmHORIZONTAL, XmPIXELS, (XtEnum*) &parseError);
    if (parseError)
        {
        XtDisplayStringConversionWarning(display, (char *)from->addr,
					 XmRHorizontalDimension);
        return False;
        }
    else
        _XM_CONVERTER_DONE( to, Dimension, tmpPix, ; )
    }

static Boolean
CvtStringToVerticalPosition(
        Display *display,
        XrmValue *args,
        Cardinal *num_args,	/* unused */
        XrmValue *from,
        XrmValue *to,
        XtPointer *converter_data) /* unused */
{
    Widget widget = *(Widget*) args[0].addr ;
    Screen * screen = XtScreen(widget) ;
    unsigned char defaultFromType = _XmGetUnitType(widget) ;
    Position tmpPix;
    Boolean parseError;

    tmpPix = (Position)
	_XmConvertStringToUnits(screen, from->addr, (int) defaultFromType,
				XmVERTICAL, XmPIXELS, (XtEnum*) &parseError);
    if (parseError)
	{
            XtDisplayStringConversionWarning(display, (char *)from->addr,
					     XmRVerticalPosition);
            return False;
        }
        else
            _XM_CONVERTER_DONE( to, Position, tmpPix, ; )
}

static Boolean
CvtStringToVerticalDimension(
        Display *display,
        XrmValue *args,
        Cardinal *num_args,	/* unused */
        XrmValue *from,
        XrmValue *to,
        XtPointer *converter_data) /* unused */
{
        Widget widget = *(Widget*) args[0].addr ;
        Screen * screen = XtScreen(widget) ;
        unsigned char defaultFromType = _XmGetUnitType(widget) ;
	Dimension tmpPix;
        Boolean parseError;

        tmpPix = (Dimension)
	  _XmConvertStringToUnits(screen, from->addr, (int) defaultFromType,
				  XmVERTICAL, XmPIXELS, (XtEnum*) &parseError);
        if (parseError)
            {
            XtDisplayStringConversionWarning(display, (char *)from->addr,
					     XmRVerticalDimension);
            return False;
            }
        else
            _XM_CONVERTER_DONE( to, Dimension, tmpPix, ; )
    }


/************************************************************************
 *
 *  XmeGetDefaultRenderTable
 *       This function is called by a widget to initialize it's rendertable
 *   resource with a default, when it is NULL. This is done by checking to
 *   see if any of the widgets, in the widget's parent hierarchy has the
 *   specifyRenderTable trait.
 *
 *************************************************************************/
typedef struct _system_font_list
{
    Display	*display;
    XmFontList	 fontlist;
} SystemFontList;

static XmFontList DefaultSystemFontList(Display *display, XmFontList fontlist)
{
    static SystemFontList	*sFontLists = NULL;
    static int			 nsFontLists = 0;
    static int			 maxnsFontLists = 0;

    if (fontlist)
    {
	if (nsFontLists >= maxnsFontLists)
	{
	    Cardinal nbytes;

	    maxnsFontLists += 8;
	    nbytes = (Cardinal) sizeof(SystemFontList)*maxnsFontLists;
	    if (NULL == sFontLists)
	    {
	        sFontLists = (SystemFontList*) XtMalloc(nbytes);
		memset((void*) sFontLists, 0, nbytes);
	    }
	    else
	    {
	        sFontLists =
		  (SystemFontList*) XtRealloc((char*) sFontLists, nbytes);
		memset((void*) &sFontLists[nsFontLists], 0, nbytes);
	    }
	    sFontLists[nsFontLists].display = display;
	    sFontLists[nsFontLists].fontlist = fontlist;
	    nsFontLists++;
	}
    }
    else
    {
	int i;
	if (NULL == sFontLists) return NULL;

	for (i=0; i<nsFontLists; i++)
	{
	    if (sFontLists[i].display == display)
	      return sFontLists[i].fontlist;
	}
    }
    return NULL;
}

XmFontList
XmeGetDefaultRenderTable(
        Widget w,
        unsigned char fontListType )
{
    XmFontList fontlist = NULL;
    static XmFontList sFontList = NULL;
    Widget origw = w;
    XmFontListEntry fontListEntry;
    char *s;
    char *newString;
    char *sPtr;
    char *fontName;
    char *fontTag;
    XmFontType fontType;
    char delim;
    XmSpecRenderTrait trait ;
    _XmWidgetToAppContext(w);

    if (fontListType) {
        _XmAppLock(app);
	/* look for the first ancestor with the trait */
	while ((w = XtParent(w)) != NULL) {
	    if ((trait = (XmSpecRenderTrait)
		 XmeTraitGet((XtPointer) XtClass(w),
			     XmQTspecifyRenderTable)) != NULL) {
		fontlist = trait->getRenderTable(w, fontListType) ;
		break ;
	    }
	}
        _XmAppUnlock(app);
    }

    if (fontlist) {
        return (fontlist);
    }
#if 0
    else if (sFontList) {
	printf("Reusing sFontList\n");
	return(sFontList);
    }
#endif

    _XmProcessLock();
    fontlist = DefaultSystemFontList(XtDisplay(origw), (XmFontList) NULL);
    if (!fontlist) {
	/* Begin fixing OSF 4735 */
	s = (char *) XmDEFAULT_FONT;
	sPtr = newString = XtNewString (s);

	if (!GetNextFontListEntry (&sPtr, &fontName, &fontTag,
				   &fontType, &delim)) {
	    _XmProcessUnlock();
	    XtFree (newString);
	    XmeWarning(NULL, MSG2);
	    exit( 1) ;
	}

	do {
	    if (*fontName) {
		fontListEntry = XmFontListEntryLoad (XtDisplay(origw),
						     fontName,
						     fontType, fontTag);
		if (fontListEntry != NULL) {
		    fontlist = XmFontListAppendEntry (fontlist, fontListEntry);
		    XmFontListEntryFree (&fontListEntry);
		}
		else
		    XtDisplayStringConversionWarning(XtDisplay(origw),
						     fontName, XmRFontList);
	    }
	}
	while ((delim == ',') && *++sPtr && !fontlist &&
	       GetNextFontListEntry (&sPtr, &fontName, &fontTag,
				     &fontType, &delim));
	XtFree (newString);
	DefaultSystemFontList(XtDisplay(origw), fontlist);
	/* End fixing OSF 4735 */
    }
    _XmProcessUnlock();
    return (fontlist);
}

static void
ConvertStringToButtonTypeDestroy(
        XtAppContext app,	/* unused */
        XrmValue *to,
        XtPointer converter_data, /* unused */
        XrmValue *args,		/* unused */
        Cardinal *num_args)	/* unused */
{
    XtFree( *((char **) to->addr)) ;

    return ;
    }

static Boolean
ConvertStringToButtonType(
        Display *display,
        XrmValue *args,		/* unused */
        Cardinal *num_args,	/* unused */
        XrmValue *from,
        XrmValue *to,
        XtPointer *converter_data) /* unused */
{
    String in_str = (String) from->addr ;
    unsigned int in_str_size = 0 ;
    XmButtonTypeTable buttonTable ;
    int i, comma_count ;
    String work_str, btype_str ;
    _Xstrtokparams strtok_buf;

    comma_count = 0 ;
    while(    in_str[in_str_size]    )
    {   if(    in_str[in_str_size++] == ','    )
        {   ++comma_count ;
            }
        }
    ++in_str_size ;

    buttonTable = (XmButtonTypeTable) XtMalloc(
                                   sizeof( XmButtonType) * (comma_count + 2)) ;
    buttonTable[comma_count+1] = (XmButtonType)0;
    work_str = (String) XtMalloc( in_str_size) ;
    strcpy( work_str, in_str) ;

    for(    i = 0, btype_str = _XStrtok(work_str, ",", strtok_buf) ;
            btype_str ;
            btype_str = _XStrtok(NULL, ",", strtok_buf), ++i)
    {
        while (*btype_str && isspace((unsigned char)*btype_str)) btype_str++;
        if (*btype_str == '\0')
            break;
        if (XmeNamesAreEqual(btype_str, "pushbutton"))
            buttonTable[i] = XmPUSHBUTTON;
        else if (XmeNamesAreEqual(btype_str, "togglebutton"))
            buttonTable[i] = XmTOGGLEBUTTON;
        else if (XmeNamesAreEqual(btype_str, "cascadebutton"))
            buttonTable[i] = XmCASCADEBUTTON;
        else if (XmeNamesAreEqual(btype_str, "separator"))
            buttonTable[i] = XmSEPARATOR;
        else if (XmeNamesAreEqual(btype_str, "double_separator"))
            buttonTable[i] = XmDOUBLE_SEPARATOR;
        else if (XmeNamesAreEqual(btype_str, "title"))
            buttonTable[i] = XmTITLE;
        else
        {
            XtDisplayStringConversionWarning(display, (char *) btype_str,
					     XmRButtonType) ;
            XtFree( (char *) buttonTable) ;
            XtFree( (char *) work_str) ;

            return( FALSE) ;
            }
        }
    XtFree( work_str) ;

    _XM_CONVERTER_DONE( to, XmButtonTypeTable, buttonTable, XtFree( (char *) buttonTable) ; )
    }

static void
CvtStringToKeySymTableDestroy(
        XtAppContext app,	/* unused */
        XrmValue *to,
        XtPointer converter_data, /* unused */
        XrmValue *args,		/* unused */
        Cardinal *num_args)	/* unused */
{
    XtFree( *((char **) to->addr)) ;

    return ;
    }

static Boolean
CvtStringToKeySymTable(
        Display *display,
        XrmValue *args,		/* unused */
        Cardinal *num_args,	/* unused */
        XrmValue *from,
        XrmValue *to,
        XtPointer *converter_data) /* unused */
{
  String in_str = (String) from->addr;
  unsigned int in_str_size = 0;
  XmKeySymTable keySymTable;
  int i, comma_count;
  String work_str, ks_str;
  KeySym ks;
  _Xstrtokparams strtok_buf;

  comma_count = 0;
  while (in_str[in_str_size])
    {
      if (in_str[in_str_size++] == ',')
	++comma_count;
    }
  ++in_str_size;

  keySymTable = (XmKeySymTable) XtMalloc(sizeof(KeySym) * (comma_count + 2));
  keySymTable[comma_count + 1] = (KeySym)NULL;
  work_str = XtNewString(in_str);

  for (ks_str = _XStrtok(work_str, ",", strtok_buf), i = 0;
       ks_str;
       ks_str = _XStrtok(NULL, ",", strtok_buf), i++)
    {
      if (!*ks_str)
	keySymTable[i] = NoSymbol;
      else
	{
	  if ((ks = XStringToKeysym(ks_str)) == NoSymbol)
	    {
	      XtDisplayStringConversionWarning(display, ks_str, XmRKeySym);
	      XtFree((char *) work_str);
	      XtFree((char *) keySymTable);

	      return FALSE;
	    }
	  keySymTable[i] = ks;
	}
    }
  XtFree((char *) work_str);

  _XM_CONVERTER_DONE(to, XmKeySymTable, keySymTable,
		     XtFree((char*)keySymTable);)
}

static void
CvtStringToCharSetTableDestroy(
        XtAppContext app,	/* unused */
        XrmValue *to,
        XtPointer converter_data, /* unused */
        XrmValue *args,		/* unused */
        Cardinal *num_args)	/* unused */
{
    XtFree( *((char **) to->addr)) ;

    return ;
    }

static Boolean
CvtStringToCharSetTable(
        Display *display,	/* unused */
        XrmValue *args,		/* unused */
        Cardinal *num_args,	/* unused */
        XrmValue *from,
        XrmValue *to,
        XtPointer *converter_data) /* unused */
{
  String in_str = (String) from->addr;
  XmStringCharSetTable charsetTable;
  unsigned int numCharsets = 0;
  unsigned int strDataSize = 0;
  char * dataPtr;
  int i;
  String work_str, cs_str;
  _Xstrtokparams strtok_buf;

  work_str = XtNewString(in_str);

  for (cs_str = _XStrtok(work_str, ",", strtok_buf);
       cs_str;
       cs_str = _XStrtok(NULL, ",", strtok_buf))
    {
      if (*cs_str)
	strDataSize += strlen(cs_str) + 1;
      ++numCharsets;
    }

  charsetTable = (XmStringCharSetTable)
    XtMalloc(strDataSize + sizeof(XmStringCharSet) * (numCharsets+1));
  charsetTable[numCharsets] = (XmStringCharSet)NULL;
  dataPtr = (char *) &charsetTable[numCharsets+1];
  strcpy(work_str, in_str);

  for (i = 0, cs_str = _XStrtok(work_str, ",", strtok_buf);
       cs_str;
       cs_str = _XStrtok(NULL, ",", strtok_buf), ++i)
    {
      if (*cs_str)
	{
	  charsetTable[i] = dataPtr;
	  strcpy(dataPtr, cs_str);
	  dataPtr += strlen(cs_str) + 1;
	}
      else
	{
	  charsetTable[i] = NULL;
	}
    }
  XtFree((char *) work_str);

  _XM_CONVERTER_DONE(to, XmStringCharSetTable, charsetTable,
		     XtFree((char *) charsetTable);)
}

/************************************************************************
 *
 *  CvtStringToBooleanDimension
 *
 ************************************************************************/
static Boolean
CvtStringToBooleanDimension(
        Display *display,
        XrmValue *args,
        Cardinal *num_args,	/* unused */
        XrmValue *from,
        XrmValue *to,
        XtPointer *converter_data) /* unused */
{
        char *in_str = (char *) from->addr ;
        Dimension outVal ;
        int intermediate;

    if (isInteger(from->addr, &intermediate))
    {
        /* Is numeric argument, so convert to horizontal dimension.  This is
        *   to preserve 1.0 compatibility (the resource actually behaves like
        *   a boolean in version 1.1).
        */
        Widget widget = *(Widget*) args[0].addr ;
        Screen * screen = XtScreen(widget) ;
        unsigned char unitType = _XmGetUnitType(widget) ;

        if(    intermediate < 0    )
        {   XtDisplayStringConversionWarning(display, (char *)from->addr,
					     XmRBooleanDimension) ;
            return( FALSE) ;
            }
        outVal = (Dimension) _XmConvertUnits( screen, XmHORIZONTAL,
                                      (int) unitType, intermediate, XmPIXELS) ;
        }
    else
    {   /* Presume Boolean (version 1.1).
        */
        if(    XmeNamesAreEqual( in_str, XtEtrue)    )
        {   outVal = (Dimension) 1 ;
            }
        else
        {   if(    XmeNamesAreEqual( in_str, XtEfalse)    )
            {   outVal = (Dimension) 0 ;
                }
            else
            {   XtDisplayStringConversionWarning(display, in_str,
						 XmRBooleanDimension) ;
                return( FALSE) ;
                }
            }
        }
    _XM_CONVERTER_DONE( to, Dimension, outVal, ; )
    }



/************************************************************************
 *
 *  XmCvtStringToAtomList
 *	Convert a string to an array of atoms.  Atoms within the string
 *  are delimited by commas.  If the comma is preceded by a backslash,
 *  it is considered to be part of the atom.
 *
 ************************************************************************/
static Boolean
CvtStringToAtomList(
	Display *dpy,
	XrmValue *args,		/* unused */
	Cardinal *num_args,	/* unused */
	XrmValue *from,
	XrmValue *to,
	XtPointer *converter_data ) /* unused */
{
  char *atom_name;
  char *stack_names[128];
  char **name_list = stack_names;
  int max_atoms = XtNumber(stack_names);
  int atom_count;
  Atom *atom_list;
  char *context_string;

  if (from->addr == NULL)
    return(False);

  atom_count = 0;
  for (atom_name = GetNextToken((char*) from->addr, ",", &context_string);
       atom_name != NULL;
       atom_name = GetNextToken(NULL, ",", &context_string))
    {
      if (atom_count == max_atoms)
	{
	  max_atoms *= 2;

	  if (name_list == stack_names)
	    {
	      char **new_names = (char **) XtMalloc(sizeof(char *) * max_atoms);
	      memcpy((char *)new_names, (char *)name_list,
		     (sizeof(char *) * atom_count));
	      name_list = new_names;
	    }
	  else
	    {
	      name_list = (char **) XtRealloc((char *)name_list,
					      sizeof(char *) * max_atoms);
	    }
	}

      name_list[atom_count++] = atom_name;
    }

  atom_list = (Atom *) XtMalloc(sizeof(Atom) * atom_count);
  XInternAtoms(dpy, name_list, atom_count, False, atom_list);

  while(--atom_count >= 0)
    XtFree(name_list[atom_count]);

  {
    static Atom *buf;

    if(to->addr)
      {
	if(to->size < sizeof(Atom *))
	  {
	    XtFree((char *) atom_list);
	    to->size = sizeof(Atom *);
	    return(False);
	  }
	else
	  *((Atom **) (to->addr)) = atom_list;
      }
    else
      {
	buf = atom_list;
	to->addr = (XPointer) &buf;
      }

    to->size = sizeof(Atom *);
    return(True);
  }
}

static void
SimpleDestructor(
        XtAppContext app,	/* unused */
        XrmValue *to,
        XtPointer data,		/* unused */
        XrmValue *args,		/* unused */
        Cardinal *num_args )	/* unused */
{
   char *simple_struct = *(char **)(to->addr);

   XtFree(simple_struct);
}

/*
 *
 * GetNextToken
 *
 * This should really be in some sort of utility library.
 * This function is supposed to behave a bit like strtok in that it
 * saves a context which is used if src is NULL.  We'd like to use
 * strok, but strok can't handle backslashes.
 *
 * A token is the contiguous substring of src which begins with either
 * a backslashed space character or a non-space character and
 * terminates with occurance of a non-backslashed delimiter character
 * or the character before the last non-backshashed space character.
 *
 * Caller is responsible to free the returned string.
 *
 * Example A:
 *    The delimiter string is ","   The src is
 *           "   \ token  token\ , next token"
 *    The token is
 *           " token token "
 *
 * Example B:
 *
 *    The delimiter string is
 *        ".:"
 *    The src is
 *        "   \: the \t token \. \    : next token  "
 *    The token returned is
 *        ": the \t token .  "
 *
 */

static Boolean
OneOf(
        char c,
	char *set )
{
	char *p;

	for (p = set; *p != 0; p++)
		if (*p == c)
			return(True);

	return(False);
}

static char *
GetNextToken(
	char *src,
	char *delim,
	char **context)
{
	Boolean terminated = False;
	char *s, *e, *p;
	char *next_context;
	char *buf = NULL;
	int len;

	if (src != NULL)
		*context = src;

	if (*context == NULL)
		return(NULL);

	s = *context;

	/* find the end of the token */
	for (e = s; (!terminated) && (*s != '\0'); e = s++)
	{
		if ((*s == '\\') && (*(s+1) != '\0'))
			s++;
		else if (OneOf(*s, delim))
			terminated = True;
	}

	/* assert (OneOf(*e,delim) || (*e == '\0')) */
	if (terminated)
	{
		next_context = (e + 1);
		e--;
	}
	else
		next_context = NULL;

	/* Strip out non-backslashed leading and trailing whitespace */
	s = *context;
	while ((s != e) && isspace((unsigned char)*s))
		s++;
	while ((e != s) && isspace((unsigned char)*e) && ((*e-1) != '\\'))
		e--;

	if (e == s)
	{
		/*
		 * Only white-space between the delimiters,
		 * if we're at the end of the string anyway, indicate
		 * that we're done, otherwise return an empty string.
		 */
		if (terminated)
		{
			buf = (char *) XtMalloc(1);
			*buf = '\0';
			return(buf);
		}
		else
			return(NULL);
	}

	/*
	 * Copy into buffer.  Swallow any backslashes which precede
	 * delimiter characters or spaces.  It would be great if we had
	 * time to implement full C style backslash processing...
	 */
	len = (e - s) + 1;

	p = buf = XtMalloc(len + 1);
	while (s != e)
	{
		if ((*s == '\\') &&
		    (OneOf(*(s+1), delim) || isspace((unsigned char)*(s+1))))
			s++;

		*(p++) = *(s++);
	}
	*(p++) = *(s++);
	*p = '\0';

	*context = next_context;

	return(buf);
}

static Boolean
CvtStringToCardinal(
        Display *display,
        XrmValue *args,		/* unused */
        Cardinal *num_args,	/* unused */
        XrmValue *from,
        XrmValue *to,
        XtPointer *converter_data) /* unused */
{
    Cardinal value;
    int intermediate;
    if (!isInteger(from->addr,&intermediate) || intermediate < 0)
	{
	XtDisplayStringConversionWarning(display, (char *)from->addr,
					 XmRCardinal);
	return False;
	}

    value = (Cardinal) intermediate;
    _XM_CONVERTER_DONE( to, Cardinal, value, ; )
}


static Boolean
CvtStringToTextPosition(
        Display *display,
        XrmValue *args,		/* unused */
        Cardinal *num_args,	/* unused */
        XrmValue *from,
        XrmValue *to,
        XtPointer *converter_data) /* unused */
{
    XmTextPosition value;
    int intermediate;
    if (!isInteger(from->addr,&intermediate) || intermediate < 0)
        {
        XtDisplayStringConversionWarning(display, (char *)from->addr,
					 XmRTextPosition);
        return False;
        }

    value = (XmTextPosition) intermediate;
    _XM_CONVERTER_DONE( to, XmTextPosition, value, ; )
}


static Boolean
CvtStringToTopItemPosition(
        Display *display,
        XrmValue *args,		/* unused */
        Cardinal *num_args,	/* unused */
        XrmValue *from,
        XrmValue *to,
        XtPointer *converter_data) /* unused */
{
    int value;
    int intermediate;
    if (!isInteger(from->addr,&intermediate) || intermediate < 0)
        {
        XtDisplayStringConversionWarning(display, (char *)from->addr,
					 XmRTopItemPosition);
        return False;
        }

    value = intermediate - 1;
    _XM_CONVERTER_DONE( to, int, value, ; )
}


static Boolean
isInteger(
    String string,
    int *value)		/* RETURN */
{
    Boolean foundDigit = False;
    Boolean isNegative = False;
    Boolean isPositive = False;
    int val = 0;
    char ch;
    /* skip leading whitespace */
    while ((ch = *string) == ' ' || ch == '\t') string++;
    while ((ch = *string++) != '\0') {
	if (ch >= '0' && ch <= '9') {
	    val *= 10;
	    val += ch - '0';
	    foundDigit = True;
	    continue;
	}
	if (ch == ' ' || ch == '\t') {
	    if (!foundDigit) return False;
	    /* make sure only trailing whitespace */
	    while ((ch = *string++) != '\0') {
		if (ch != ' ' && ch != '\t')
		    return False;
	    }
	    break;
	}
	if (ch == '-' && !foundDigit && !isNegative && !isPositive) {
	    isNegative = True;
	    continue;
	}
	if (ch == '+' && !foundDigit && !isNegative && !isPositive) {
	    isPositive = True;
	    continue;
	}
	return False;
    }
    if (ch == '\0') {
	if (isNegative)
	    *value = -val;
	else
	    *value = val;
	return True;
    }
    return False;
}


/************************************************************************
 *
 *  CvtStringToRenditionPixel
 *	Convert a string to Pixel, checking for the special value
 *	"unspecified_pixel" which returns XmUNSPECIFIED_PIXEL
 *
 ************************************************************************/
static Boolean
CvtStringToRenditionPixel(Display *disp,
			     XrmValuePtr args,
			     Cardinal *num_args,
			     XrmValue *from_val,
			     XrmValue *to_val,
			     XtPointer *converter_data )
{
  String 	str = (String)from_val->addr;
  Boolean	result = False;

  if (XmeNamesAreEqual(str, "unspecified_pixel"))
    {
      _XM_CONVERTER_DONE(to_val, Pixel, XmUNSPECIFIED_PIXEL, ;)
      }

  result = XtCallConverter(disp, XtCvtStringToPixel, args, *num_args,
			   from_val, to_val, NULL);

  if (result == False)
    {
      *converter_data = False;
      return False;
    }
  else
    {
      *converter_data = (char *)True;
      return True;
    }
}

/************************************************************************
 *
 *  CvtPixelToRenditionPixel
 *	Convert a Pixel to a RenditionPixel, which really means
 *      doing nothing.
 *
 ************************************************************************/

static Boolean
CvtPixelToRenditionPixel(Display *disp,
			     XrmValuePtr args, /* unused */
			     Cardinal *num_args,
			     XrmValue *from_val,
			     XrmValue *to_val,
			     XtPointer *converter_data ) /* unused */
{
    if (*num_args != 0)
	XtAppWarningMsg(XtDisplayToApplicationContext(disp),
		"wrongParameters","CvtPixelToRenditionPixel", "ToolkitError",
		"Pixel to RenditionPixel conversion needs no extra arguments",
			(String *) NULL, (Cardinal *)NULL);
    _XM_CONVERTER_DONE(to_val, Pixel, *(int*)from_val->addr, ;)
}

/************************************************************************
 *
 *  CvtStringToSelectColor
 *	Convert a string to Pixel, checking for the special values
 *	"default_select_color" which returns the XmDEFAULT_SELECT_COLOR,
 *	"highlight_color" which returns the XmHIGHLIGHT_COLOR.
 *	"reversed_ground_colors" which returns XmREVERSED_GROUND_COLORS.
 *
 ************************************************************************/
static Boolean
CvtStringToSelectColor(Display *disp,
			  XrmValuePtr args,
			  Cardinal *num_args,
			  XrmValue *from_val,
			  XrmValue *to_val,
			  XtPointer *converter_data )
{
  String 	str = (String)from_val->addr;
  Boolean	result = False;

  /* in those 3 cases, just return the constant and the widget
     will have to do teh onversion itself. It's ok, since the widget
     has to handle the constant case anyway for direct use by
     a program at creation and setvalues time */
  if (XmeNamesAreEqual(str, "default_select_color")) {
      _XM_CONVERTER_DONE(to_val, Pixel, XmDEFAULT_SELECT_COLOR, ;)
      }
  else if (XmeNamesAreEqual(str, "reversed_ground_colors")) {
      _XM_CONVERTER_DONE(to_val, Pixel, XmREVERSED_GROUND_COLORS, ;)
      }
  else if (XmeNamesAreEqual(str, "highlight_color")) {
      _XM_CONVERTER_DONE(to_val, Pixel, XmHIGHLIGHT_COLOR, ;)
      }

  /* else call the Xt converter, passing it the colorConvertArg */
  result = XtCallConverter(disp, XtCvtStringToPixel, args, *num_args,
			   from_val, to_val, NULL);

  if (result == False)
    {
      *converter_data = False;
      return False;
    }
  else
    {
      *converter_data = (char *)True;
      return True;
    }
}


/************************************************************************
 *
 *  GetNextTab
 *
 ************************************************************************/
static Boolean
GetNextTab(char **s,
	   float *value,
	   char *unitType,
	   XmOffsetModel *offsetModel)
{
  int	ret_val;
  char 	sign[3];
  char	*tmp;

  bzero(sign, sizeof(sign));
  unitType[0] = '\0';

  if (sscanf(*s, " %2[+]", sign) == 1)
    ret_val = sscanf(*s, " %2[+] %f %12[^ \t\r\n\v\f,] ",
		     sign, value, unitType);
  else ret_val = sscanf(*s, " %f %12[^ \t\r\n\v\f,] ",
			value, unitType);

  if (ret_val == EOF) return(FALSE);

  if (sign[1] != '\0')
    {
      /* Error message */
      return(FALSE);
    }

  switch (sign[0])
    {
    case '\0':
      *offsetModel = XmABSOLUTE;
      break;

    case '+':
      *offsetModel = XmRELATIVE;
      break;
    }

  tmp = strpbrk(*s, ",");

  if (tmp == NULL) *s += strlen(*s);
  else *s = (tmp + 1);

  return(TRUE);
}


static void
CvtStringToXmTabListDestroy(XtAppContext app, /* unused */
			       XrmValue *to,
			       XtPointer converter_data, /* unused */
			       XrmValue *args, /* unused */
			       Cardinal *num_args) /* unused */
{
  XmTabListFree(*((XmTabList *)to->addr));

  return;
}

/************************************************************************
 *
 *  CvtStringToXmTabList
 *	Convert a string to a tab list.  This is in the form :
 *
 *  <XmTabList>	::=	<tab> { ',' <tab> }*
 *
 *  <tab>	::=	<float> <units>
 *
 *  <float>	::=	{ <sign> } { {DIGIT}*.}DIGIT+
 *
 *  <sign>	::=	+ | -
 *
 ************************************************************************/

static Boolean
CvtStringToXmTabList(Display *dpy,
			XrmValue *args,	/* unused */
			Cardinal *num_args, /* unused */
			XrmValue *from,
			XrmValue *to,
			XtPointer *converter_data) /* unused */
{
  Boolean 	got_one = FALSE;
  char 		*s;
  float 	value;
  char 		unitType[12]; /* longest unit name is "millimeters"  */
  XmOffsetModel	offsetModel = XmABSOLUTE;
  int		units;
  XmParseResult	result;
  XmTab		tab;
  XmTabList	tl = NULL;

  if (from->addr)
    {
      s = (char *)from->addr;

      /* Parse the tabs */
      while (GetNextTab(&s, &value, unitType, &offsetModel))
	{
	  got_one = TRUE;

	  result = XmeParseUnits(unitType, &units);

	  if (result == XmPARSE_ERROR)
	    {
	      got_one = FALSE;
	      break;
	    }
	  else if (result == XmPARSE_NO_UNITS)
	    {
	      units = XmPIXELS;
	    }
	  tab = XmTabCreate(value, (unsigned char)units, offsetModel,
			    XmALIGNMENT_BEGINNING, XmS);

	  tl = XmTabListInsertTabs(tl, &tab, 1, -1);

	  XmTabFree(tab);
	}
    }

  if (got_one)
    _XM_CONVERTER_DONE(to, XmTabList, tl, XmTabListFree(tl);)

  XtDisplayStringConversionWarning(dpy, (char *)from->addr, XmRTabList);
  return(FALSE);
}

static Boolean
cvtStringToXmRenderTable(Display *dpy,
			 Widget widget,
			 String resname,
			 String resclass,
			 XrmValue *from,
			 XrmValue *to)
{
  char 		*s;
  XmRendition	rend[1];
  XmRenderTable	rt;
  char		*tag;
  Boolean	has_default = FALSE, in_db = FALSE;
  _Xstrtokparams strtok_buf;

  if (from->addr)
    {
      s = XtNewString((char *)from->addr);
      rt = NULL;
      has_default = FALSE;

      /* Try for default rendition */
      rend[0] = _XmRenditionCreate(NULL, widget, resname, resclass,
				   NULL, NULL, 0, NULL);

      if (rend[0] != NULL)
	{
	  rt = XmRenderTableAddRenditions(NULL, rend, 1, XmMERGE_REPLACE);
	  has_default = TRUE;
	}

      /* Try to get first tag. */
      if ((tag = _XStrtok(s, " \t\r\n\v\f,", strtok_buf)) != NULL)
	{
	  XmRenditionFree(rend[0]);
	  rend[0] = _XmRenditionCreate(NULL, widget, resname, resclass,
				       tag, NULL, 0, &in_db);

	  if (!has_default && !in_db)
	    {
	      /* Call the fontlist converter */
	      XmRenditionFree(rend[0]);
	      XtFree(s);
	      return CvtStringToXmFontList(dpy, NULL, 0, from, to, NULL);
	    }

	  rt = XmRenderTableAddRenditions(rt, rend, 1, XmMERGE_REPLACE);
	}
      else if (rend[0] == NULL)
	{
	  /* warning */
	  XtFree(s);
	  return FALSE;
	}
      else
	{
	  /* only a default rendition */
	  XtFree(s);
	  XmRenditionFree(rend[0]);
	  _XM_CONVERTER_DONE(to, XmRenderTable, rt, XmRenderTableFree(rt);)
	}

      while ((tag = _XStrtok(NULL, " \t\r\n\v\f,", strtok_buf)) != NULL)
	{
	  XmRenditionFree(rend[0]);

	  rend[0] = _XmRenditionCreate(NULL, widget, resname, resclass,
				       tag, NULL, 0, NULL);

	  rt = XmRenderTableAddRenditions(rt, rend, 1, XmMERGE_REPLACE);
	}

      XtFree(s);
      XmRenditionFree(rend[0]);
      _XM_CONVERTER_DONE(to, XmRenderTable, rt, XmRenderTableFree(rt);)
    }

  return FALSE;
}

static Boolean
CvtStringToRenderTable(Display *dpy,
			  XrmValue *args,
			  Cardinal *num_args, /* unused */
			  XrmValue *from,
			  XrmValue *to,
			  XtPointer *converter_data) /* unused */
{
  Widget wid;

  wid = *(Widget *)args[0].addr;

  return(cvtStringToXmRenderTable(dpy, wid,
				  XmNrenderTable, XmCRenderTable,
				  from, to));
}

static Boolean
CvtStringToButtonRenderTable(Display *dpy,
				XrmValue *args,
				Cardinal *num_args, /* unused */
				XrmValue *from,
				XrmValue *to,
				XtPointer *converter_data) /* unused */
{
  Widget wid;

  wid = *(Widget *)args[0].addr;

  return(cvtStringToXmRenderTable(dpy, wid,
				  XmNbuttonRenderTable, XmCButtonRenderTable,
				  from, to));
}

static Boolean
CvtStringToLabelRenderTable(Display *dpy,
			       XrmValue *args,
			       Cardinal *num_args, /* unused */
			       XrmValue *from,
			       XrmValue *to,
			       XtPointer *converter_data) /* unused */
{
  Widget wid;

  wid = *(Widget *)args[0].addr;

  return(cvtStringToXmRenderTable(dpy, wid,
				  XmNlabelRenderTable, XmCLabelRenderTable,
				  from, to));
}

static Boolean
CvtStringToTextRenderTable(Display *dpy,
			      XrmValue *args,
			      Cardinal *num_args, /* unused */
			      XrmValue *from,
			      XrmValue *to,
			      XtPointer *converter_data) /* unused */
{
  Widget wid;

  wid = *(Widget *)args[0].addr;

  return(cvtStringToXmRenderTable(dpy, wid,
				  XmNtextRenderTable, XmCTextRenderTable,
				  from, to));
}

static void
CvtStringToXmRenderTableDestroy(XtAppContext app, /* unused */
				 XrmValue *to,
				 XtPointer converter_data, /* unused */
				 XrmValue *args, /* unused */
				 Cardinal *num_args) /* unused */
{
  XmRenderTableFree(*((XmRenderTable *)to->addr));
}
