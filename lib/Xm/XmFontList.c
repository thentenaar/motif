/* $XConsortium: XmFontList.c /main/16 1996/11/20 15:15:34 drk $ */
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef __cplusplus
extern "C" { /* some 'locale.h' do not have prototypes (sun) */
#endif
#include <X11/Xlocale.h>
#ifdef __cplusplus
} /* Close scope of 'extern "C"' declaration */
#endif /* __cplusplus */

#include <Xm/AtomMgr.h>
#include <Xm/XmosP.h>
#include "XmI.h"
#include "XmRenderTI.h"
#include "XmStringI.h"

/*
 * Data structure macros for fontlist access
 */

#define FLContextIndex(context)	(((XmFontListContextRec *)(context))->index)
#define FLContextTable(context)	(((XmFontListContextRec *)(context))->table)
#define FLContextError(context)	(((XmFontListContextRec *)(context))->error)
#define FLDisplay(fl)		((*fl)->display)

/********    Static Function Declarations    ********/


/********    End Static Function Declarations    ********/

/* NOTE - this function XmFontListEntryCreate is NOT MT-safe if the
   application is using threads with multiple application contexts

   to use Font lists in threaded code with multiple application contexts
   it is necessary to use the new API XmFontListEntryCreate_r function
   which passes in an additional widget parameter ... this widget
   MUST be for the display that font will be used in

    Font should not be shared between displays in an MT environment */

XmFontListEntry
XmFontListEntryCreate(
        char *tag,
        XmFontType type ,
        XtPointer font )
{
    char	*derived_tag;
    Cardinal	n;
    Arg		args[4];
    XmFontListEntry ret_val;

    _XmProcessLock();
    if ((font == NULL) || (tag == NULL) ||
        ((type != XmFONT_IS_FONTSET) && (type != XmFONT_IS_FONT))) {
	 _XmProcessUnlock();
         return (NULL);
    }

    if ((tag != XmFONTLIST_DEFAULT_TAG) &&
	(strcmp(tag, XmSTRING_DEFAULT_CHARSET) == 0))
      derived_tag = _XmStringGetCurrentCharset();
    else derived_tag = tag;

    n = 0;
    XtSetArg(args[n], XmNfontType, type); n++;
    XtSetArg(args[n], XmNloadModel, XmLOAD_IMMEDIATE); n++;
    XtSetArg(args[n], XmNfont, font); n++;

    ret_val =
      XmRenditionCreate(NULL,
		_XmStringCacheTag(derived_tag, XmSTRING_TAG_STRLEN),
			args, n);
    _XmProcessUnlock();
    return ret_val;
}

/*
 * MT safe version of XmFontList - requires widget parameter
 *
 *  A display variable is necessary to create a render table in an
 *  MT safe environment
 *
 *  the widget passed in DOES NOT need to be the widget which uses the
 *  font however it MUST be on the same display
 *
 *  Fonts can not be shared among displays in an MT environment
 */

XmFontListEntry
XmFontListEntryCreate_r(char *tag,
			XmFontType type,
			XtPointer font,
			Widget wid )
{
  char        *derived_tag;
  Cardinal    n;
  Arg         args[4];
  XmFontListEntry ret_val;

  _XmWidgetToAppContext(wid);
  _XmAppLock(app);
  if ((font == NULL) || (tag == NULL) ||
      ((type != XmFONT_IS_FONTSET) && (type != XmFONT_IS_FONT)))
    {
      _XmAppUnlock(app);
      return (NULL);
    }

  if ((tag != XmFONTLIST_DEFAULT_TAG) &&
      (strcmp(tag, XmSTRING_DEFAULT_CHARSET) == 0))
    derived_tag = _XmStringGetCurrentCharset();
  else
    derived_tag = tag;

  n = 0;
  XtSetArg(args[n], XmNfontType, type); n++;
  XtSetArg(args[n], XmNloadModel, XmLOAD_IMMEDIATE); n++;
  XtSetArg(args[n], XmNfont, font); n++;

  ret_val =
      XmRenditionCreate(wid,
			_XmStringCacheTag(derived_tag, XmSTRING_TAG_STRLEN),
			args, n);
  _XmAppUnlock(app);
  return ret_val;
}

void
XmFontListEntryFree(
        XmFontListEntry  *entry )
{
  if (entry != NULL) XmRenditionFree(*entry);
}

XtPointer
XmFontListEntryGetFont(
        XmFontListEntry entry ,
        XmFontType *typeReturn )
{
  XtPointer 	ret_val;
#if USE_XFT
  XtPointer     ret_val2;
#endif
  Arg		args[3];
  Cardinal	n;
  XtAppContext  app=NULL;

  if (entry == NULL)
      return (NULL);

#ifdef XTHREADS
  if ( _XmRendDisplay((XmRendition)entry) )
    app = XtDisplayToApplicationContext(_XmRendDisplay((XmRendition)entry));

  if (app)
    _XmAppLock(app);
  else
    _XmProcessLock();
#endif

  n = 0;
  XtSetArg(args[n], XmNfontType, typeReturn); n++;
  XtSetArg(args[n], XmNfont, &ret_val); n++;
#if USE_XFT
  XtSetArg(args[n], XmNxftFont, &ret_val2); n++;
#endif
  XmRenditionRetrieve(entry, args, n);

#if USE_XFT
  if (*typeReturn == XmFONT_IS_XFT)
      ret_val = ret_val2;
#endif

  if (*typeReturn == XmAS_IS)
      *typeReturn = XmFONT_IS_FONT;

  if (ret_val == (char *)XmAS_IS)
    {
#ifdef XTHREADS
      if (app)
	_XmAppUnlock(app);
      else
	_XmProcessUnlock();
#endif
      return(NULL);
    }
  else
    {
#ifdef XTHREADS
      if (app)
	_XmAppUnlock(app);
      else
	_XmProcessUnlock();
#endif
      return(ret_val);
    }
}

char *
XmFontListEntryGetTag(
        XmFontListEntry entry )
{
  Cardinal	n;
  Arg		args[1];
  char		*tag;
  char		*ret_val;
#ifdef XTHREADS
  XtAppContext  app=NULL;
#endif

  if (entry == NULL)
	return (NULL);

#ifdef XTHREADS
  if ( _XmRendDisplay((XmRendition)entry) )
    app = XtDisplayToApplicationContext(_XmRendDisplay((XmRendition)entry));

  if (app)
    _XmAppLock(app);
  else
    _XmProcessLock();
#endif

  n = 0;
  XtSetArg(args[n], XmNtag, &tag); n++;
  XmRenditionRetrieve(entry, args, n);

  ret_val = XtNewString(tag);

#ifdef XTHREADS
  if (app)
    _XmAppUnlock(app);
  else
    _XmProcessUnlock();
#endif

  return ret_val;
}

XmFontList
XmFontListAppendEntry(
        XmFontList old ,
        XmFontListEntry entry )
{
  XmRendition	rends[1];
  XmFontList ret_val;
#ifdef XTHREADS
  XtAppContext app=NULL;
#endif

  if (!entry) {
     return (old);
  }

#ifdef XTHREADS
  if (_XmRendDisplay((XmRendition)entry))
      app = XtDisplayToApplicationContext(_XmRendDisplay((XmRendition)entry));

  if (app)
    _XmAppLock(app);
  else
    _XmProcessLock();
#endif

  rends[0] = entry;
  ret_val = XmRenderTableAddRenditions(old, rends, 1, XmDUPLICATE);

#ifdef XTHREADS
  if (app)
    _XmAppUnlock(app);
  else
    _XmProcessUnlock();
#endif

  return ret_val;
}

XmFontListEntry
XmFontListNextEntry(
        XmFontContext context )
{
    XmRendition	entry;

    _XmProcessLock();
    if ((context == NULL) || (FLContextError(context))) {
	_XmProcessUnlock();
        return (NULL);
    }

    if (FLContextIndex(context) >=
	_XmRTCount(FLContextTable(context)))
      {
	FLContextError(context) = TRUE;
	_XmProcessUnlock();
	return (NULL);
      }

    entry =
      _XmRTRenditions(FLContextTable(context))[FLContextIndex(context)];
    FLContextIndex(context)++;
    _XmProcessUnlock();
    return((XmFontListEntry)entry);
}

XmFontList
XmFontListRemoveEntry(
        XmFontList old ,
        XmFontListEntry entry )
{
  Cardinal	n;
  Arg		args[3];
  XmStringTag	tags[1];
  XmFontType	type1;
  XtPointer	font1;
#ifdef XTHREADS
  XtAppContext  app=NULL;
#endif

  if ((old == NULL) || (entry == NULL))
      return (old);

#ifdef XTHREADS
  if ( _XmRendDisplay((XmRendition)entry) )
    app = XtDisplayToApplicationContext(_XmRendDisplay((XmRendition)entry));

  if (app)
    _XmAppLock(app);
  else
    _XmProcessLock();
#endif

  n = 0;
  XtSetArg(args[n], XmNtag, &tags[0]); n++;
  XtSetArg(args[n], XmNfontType, &type1); n++;
  XtSetArg(args[n], XmNfont, &font1); n++;
  XmRenditionRetrieve(entry, args, n);

  old = _XmRenderTableRemoveRenditions(old, tags, 1, TRUE, type1, font1);

#ifdef XTHREADS
  if (app)
    _XmAppUnlock(app);
  else
    _XmProcessUnlock();
#endif

  return(old);
}

XmFontListEntry
XmFontListEntryLoad(
        Display *display ,
        char *fontName ,
        XmFontType type ,
        char *tag )
{
  Cardinal	n;
  Arg		args[4];
  XmFontListEntry ret_val;
  _XmDisplayToAppContext(display);

  _XmAppLock(app);
  n = 0;
  XtSetArg(args[n], XmNfontName, fontName); n++;
  XtSetArg(args[n], XmNfontType, type); n++;
  XtSetArg(args[n], XmNloadModel, XmLOAD_IMMEDIATE); n++;

  ret_val = _XmRenditionCreate(display, NULL, XmS, XmCFontList,
		    _XmStringCacheTag(tag, XmSTRING_TAG_STRLEN),
			    args, n, NULL);
  _XmAppUnlock(app);
  return ret_val;
}

/* NOTE - this function XmFontListCreate is NOT MT-safe if the
   application is using threads with multiple application contexts

   to use Font lists in threaded code with multiple application contexts
   it is necessary to use the new API XmFontListCreate_r function
   which passes in an additional widget parameter ... this widget
   MUST be for the display that font will be used in

    Font should not be shared between displays in an MT environment */

XmFontList
XmFontListCreate(
        XFontStruct *font,
        XmStringCharSet charset )
{
  Cardinal		n;
  Arg			args[4];
  XmRendition		rends[1];
  XmStringCharSet	curcharset;
  XmRenderTable		ret_val;

  _XmProcessLock();
  if ((font == NULL) || (charset == NULL)) {
	_XmProcessUnlock();
	return (NULL);
  }

  if ((charset != XmFONTLIST_DEFAULT_TAG) &&
      (strcmp(charset, XmSTRING_DEFAULT_CHARSET) == 0))
    curcharset = _XmStringGetCurrentCharset();
  else curcharset = charset;

  n = 0;
  XtSetArg(args[n], XmNfontType, XmFONT_IS_FONT); n++;
  XtSetArg(args[n], XmNfont, (XtPointer)font); n++;
  XtSetArg(args[n], XmNloadModel, XmLOAD_IMMEDIATE); n++;

  rends[0] =
    XmRenditionCreate(NULL,
		      _XmStringCacheTag(curcharset, XmSTRING_TAG_STRLEN),
		      args, n);

  _XmProcessUnlock();
  ret_val = XmRenderTableAddRenditions(NULL, rends, 1, XmDUPLICATE);

  XmRenditionFree(rends[0]);

  return(ret_val);
}

/* MT safe version of XmFontListCreate - requires widget parameter

   A display variable is necessary to create a render table in an
   MT safe environment

   the widget passed in DOES NOT need to be the widget which uses the
   font however it MUST be on the same display

   Fonts can not be shared among displays in an MT environment
*/

XmFontList
XmFontListCreate_r(
        XFontStruct *font,
        XmStringCharSet charset,
        Widget wid )
{
  Cardinal              n;
  Arg                   args[4];
  XmRendition           rends[1];
  XmStringCharSet       curcharset;
  XmRenderTable         ret_val;


  _XmWidgetToAppContext(wid);
  _XmAppLock(app);
  if ((font == NULL) || (charset == NULL)) {
        _XmAppUnlock(app);
        return (NULL);
  }

  if ((charset != XmFONTLIST_DEFAULT_TAG) &&
      (strcmp(charset, XmSTRING_DEFAULT_CHARSET) == 0))
    curcharset = _XmStringGetCurrentCharset();
  else curcharset = charset;

  n = 0;
  XtSetArg(args[n], XmNfontType, XmFONT_IS_FONT); n++;
  XtSetArg(args[n], XmNfont, (XtPointer)font); n++;
  XtSetArg(args[n], XmNloadModel, XmLOAD_IMMEDIATE); n++;

  rends[0] =
    XmRenditionCreate(wid,
                      _XmStringCacheTag(curcharset, XmSTRING_TAG_STRLEN),
                      args, n);

  ret_val = XmRenderTableAddRenditions(NULL, rends, 1, XmDUPLICATE);

  XmRenditionFree(rends[0]);

  _XmAppUnlock(app);
  return(ret_val);
}

/* NOTE - this function XmStringCreateFontList is NOT MT-safe if the
   application is using threads with multiple application contexts

   to use Font lists in threaded code with multiple application contexts
   it is necessary to use the new API XmFontListCreate_r function
   which passes in an additional widget parameter ... this widget
   MUST be for the display that font will be used in

    Font should not be shared between displays in an MT environment */

XmFontList XmStringCreateFontList(XFontStruct *font, XmStringCharSet charset)
{ /* deprecated */
    return XmFontListAppendEntry(NULL, XmFontListEntryCreate(charset, XmFONT_IS_FONT, font));
}

/* MT safe version of XmStringCreateFontList - requires widget parameter

   A display variable is necessary to create a render table in an
   MT safe environment

   the widget passed in DOES NOT need to be the widget which uses the
   font however it MUST be on the same display

   Fonts can not be shared among displays in an MT environment
*/
XmFontList XmStringCreateFontList_r(XFontStruct *font, XmStringCharSet charset, Widget wid)
{ /* deprecated */
	(void)wid;
    return XmFontListAppendEntry(NULL, XmFontListEntryCreate(charset, XmFONT_IS_FONT, font));
}

/*
 * dump a font list
 */
void
XmFontListFree (
    XmFontList      fontlist)
{
  XmRenderTableFree(fontlist);
}

/*
 * extent a font list by one element, the old font list is gone
 */
XmFontList
XmFontListAdd(
        XmFontList old,
        XFontStruct *font,
        XmStringCharSet charset )
{
  XmStringCharSet	curcharset;
  Cardinal		n;
  Arg			args[4];
  XmRendition		rends[1];
  XmFontList		ret_val;
#ifdef XTHREADS
  XtAppContext		app=NULL;
#endif

  if (!old)
    return((XmFontList) NULL);
  if (!charset || !font)
    return ((XmFontList) old);

#ifdef XTHREADS
  if ( _XmRTDisplay((XmRenderTable)old) )
    app = XtDisplayToApplicationContext(_XmRTDisplay((XmRenderTable)old));

  if (app)
    _XmAppLock(app);
  else
    _XmProcessLock();
#endif

  if ((charset != XmFONTLIST_DEFAULT_TAG) &&
      (strcmp(charset, XmSTRING_DEFAULT_CHARSET) == 0))
    curcharset = _XmStringGetCurrentCharset();
  else
    curcharset = charset;

  n = 0;
  XtSetArg(args[n], XmNfontType, XmFONT_IS_FONT); n++;
  XtSetArg(args[n], XmNfont, (XtPointer)font); n++;
  XtSetArg(args[n], XmNloadModel, XmLOAD_IMMEDIATE); n++;

  rends[0] =
    XmRenditionCreate(NULL,
		      _XmStringCacheTag(curcharset, XmSTRING_TAG_STRLEN),
		      args, n);

  ret_val = XmRenderTableAddRenditions(old, rends, 1, XmDUPLICATE);

#ifdef XTHREADS
  if (app)
    _XmAppUnlock(app);
  else
    _XmProcessUnlock();
#endif

  return ret_val;
}

/*
 * replicate a font list
 */
XmFontList
XmFontListCopy(
        XmFontList fontlist )
{
  return(XmRenderTableCopy(fontlist, NULL, 0));
}

XFontStruct *
_XmGetFirstFont(
	XmFontListEntry  entry)
{
  XFontStruct	*font_struct;
  Cardinal	n;
  Arg		args[2];
  XmFontType	type;
  XtPointer	font;

  n = 0;
  XtSetArg(args[n], XmNfontType, &type); n++;
  XtSetArg(args[n], XmNfont, &font); n++;
  XmRenditionRetrieve(entry, args, n);

  if (font == (XtPointer)XmAS_IS)
    {
      font_struct = NULL;
    }
  else if (type == XmFONT_IS_FONTSET)
    {
      XFontStruct **font_struct_list;
      char **font_name_list;

      if (XFontsOfFontSet((XFontSet)font,
			  &font_struct_list, &font_name_list)
	  && font_struct_list[0]->fid != 0)
	font_struct = font_struct_list[0];
      else
	font_struct = NULL;
    }
  else
    font_struct = (XFontStruct *)font;

  return (font_struct);
}

/*
 * Find an entry in the fontlist which matches the current charset or
 * return the first font if none match.
 */
Boolean
XmeRenderTableGetDefaultFont(
        XmFontList fontlist,
        XFontStruct **font_struct )
{
  XmStringTag	       tag = XmFONTLIST_DEFAULT_TAG;
  short		       indx = -1;
  Boolean	       retval;
#ifdef XTHREADS
  XtAppContext	       app=NULL;

  if ( _XmRTDisplay((XmRenderTable)fontlist) )
    app = XtDisplayToApplicationContext(_XmRTDisplay((XmRenderTable)fontlist));

  if (app)
    _XmAppLock(app);
  else
    _XmProcessLock();
#endif

  retval = _XmFontListSearch (fontlist, tag, &indx, font_struct);

#ifdef XTHREADS
  if (app)
    _XmAppUnlock(app);
  else
    _XmProcessUnlock();
#endif

  return(retval);
}

/*
 * find an entry in the font list which matches, return index (or -1) and
 * font stuct ptr (or first in list).
 */
Boolean
_XmFontListSearch(
        XmFontList fontlist,
        XmStringCharSet charset,
        short *indx,
        XFontStruct **font_struct )
{
    XmFontListEntry    entry;
    Boolean            success;

    success = _XmRenderTableFindFallback(fontlist, charset,
					 FALSE, indx, &entry);

    /* For backward compatibility we must try to return something for */
    /* any non-null charset, not just XmFONTLIST_DEFAULT_TAG. */
    if (fontlist && charset && !success)
      success = _XmRenderTableFindFirstFont(fontlist, indx, &entry);

    if (success)
      *font_struct = _XmGetFirstFont(entry);
    else
      *font_struct = NULL;

    return success && (*font_struct != NULL);
}

/*
 * Fontlist access routines
 */
Boolean
XmFontListInitFontContext(
        XmFontContext *context,
        XmFontList fontlist )
{
  XmFontContext p;

  _XmProcessLock();
  if ((!fontlist) || (!context)) {
	_XmProcessUnlock();
	return(FALSE);
  }

  p = (XmFontContext) XtMalloc (sizeof (XmFontListContextRec));

  FLContextIndex(p) = 0;
  FLContextTable(p) = fontlist;
  FLContextError(p) = FALSE;
  *context = p;
  _XmProcessUnlock();
  return (TRUE);
}

Boolean
XmFontListGetNextFont(
        XmFontContext context,
        XmStringCharSet *charset,
        XFontStruct **font )
{
  XmRenderTable	table;
  XmRendition	rend;
  Cardinal	n;
  Arg		args[1];
  XmStringTag	tag;

  _XmProcessLock();
  if ((context == NULL) || FLContextError(context) ||
      (charset == NULL) || (font == NULL)) {
	_XmProcessUnlock();
	return (FALSE);
  }

  table = FLContextTable(context);

  if (FLContextIndex(context) >= _XmRTCount(table))
    {
      FLContextError(context) = TRUE;
      _XmProcessUnlock();
      return(FALSE);
    }

  rend = _XmRTRenditions(table)[FLContextIndex(context)];

  *font = _XmGetFirstFont(rend);

  _XmProcessUnlock();
  n = 0;
  XtSetArg(args[n], XmNtag, &tag); n++;
  XmRenditionRetrieve(rend, args, n);

  *charset = XtNewString(tag);

  FLContextIndex(context)++;
  return (TRUE);
}

void
XmFontListFreeFontContext(
        XmFontContext context )
{
  _XmProcessLock();
  if (context != NULL) XtFree((char *)context);
  _XmProcessUnlock();
}

#ifdef _XmDEBUG_XMSTRING

void
_Xm_dump_fontlist(
        XmFontList f )
{
    int i = 0;

    for ( ; FontListFont(f) != NULL; f++, i++)
    {
   	printf ("fontlist[%3d] of 0x%p\n", i, f);
	printf ("\ttype = %d\n", FontListType(f));
	printf ("\tfont = %p\n", FontListFont(f));
	printf ("\ttag = <%s>\n", FontListTag(f));
    }
}

void
_Xm_dump_fontlist_cache( void )
{
    FontlistEntry *cache;

    if (_fontlist_cache == NULL)
    {
        printf("fontlist cache is empty\n");
        return;
    }

    for (cache = _fontlist_cache; cache; cache = FontCacheNext(cache))
    {
        printf("cache pointer:   %p\n", cache);
        _Xm_dump_fontlist(FontCacheFontList(cache));
        printf("refcount:      %d\n", FontCacheRefCount(cache));
        printf("next:          %p\n\n", FontCacheNext(cache));
    }
}

#endif /* _XmDEBUG_XMSTRING */
