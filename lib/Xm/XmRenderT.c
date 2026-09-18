/**
 * Motif
 *
 * Copyright (c) 2026 Tim Hentenaar
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

#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$TOG: XmRenderT.c /main/14 1998/10/26 20:14:42 samborn $"
#endif
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include <X11/IntrinsicP.h>
#include <X11/ShellP.h>
#include <X11/Xresource.h>
#include <X11/Xft/Xft.h>
#include <Xm/Display.h>		/* For XmGetXmDisplay */
#include <Xm/DisplayP.h>	/* For direct access to callback fields */
#include <Xm/XmRenderT.h>
#include "MessagesI.h"
#include "XmI.h"
#include "HashI.h"
#include "SharedPtrI.h"
#include "XmRenderTI.h"
#include "XmStringI.h"
#include "XmTabListI.h"

/* Warning Messages */
#define NO_NULL_TAG_MSG         _XmMMsgXmRenderT_0000
#define NULL_DISPLAY_MSG        _XmMMsgXmRenderT_0001
#define INVALID_TYPE_MSG        _XmMMsgXmRenderT_0002
#define LOAD_FAILED             _XmMMsgXmRenderT_0003
#define NULL_LOAD_IMMEDIATE_MSG _XmMMsgXmRenderT_0005
#define INVALID_RENDITION       _XmMMsgXmRenderT_0006
#if 0
#define NULL_FONT_TYPE_MSG    		_XmMMsgXmRenderT_0004 /* TODO: Remove */
#endif

/**
 * TODO: Refactor the code using this later
 */
#define GetPtr(r) (*(char **)(r))

/********    Static Function Declarations    ********/

static void CopyFromArg(XtArgVal src,
			char *dst,
			unsigned int size);
static Cardinal GetNamesAndClasses(Widget w,
				   XrmNameList names,
				   XrmClassList classes);
static XrmResourceList CompileResourceTable(XtResourceList resources,
					    Cardinal num_resources);
static Boolean GetResources(XmRendition rend,
			    Display *dsp,
			    Widget wid,
			    String resname,
			    String resclass,
			    XmStringTag tag,
			    ArgList arglist,
			    Cardinal argcount);
static void merge_renditions(XmRendition to, XmRendition from);
static void set_props_from_pattern(XmRendition rend, const FcPattern *p);
static XftColor GetCachedXftColor(Display *display, Pixel color);

/********    End Static Function Declarations    ********/

/* Resource List. */
static XtResource _XmRenditionResources[] = {
  {
    XmNloadModel, XmCLoadModel, XmRLoadModel,
    sizeof(unsigned char), XtOffsetOf(_XmRenditionRec, loadModel),
    XmRImmediate, (XtPointer)XmUNSPECIFIED_LOAD_MODEL
  },
  {
    XmNfontType, XmCFontType, XmRFontType,
    sizeof(XmFontType), XtOffsetOf(_XmRenditionRec, fontType),
    XmRImmediate, (XtPointer)XmAS_IS
  },
  {
    XmNtag, XmCTag, XmRString,
    sizeof(XmStringTag), XtOffsetOf(_XmRenditionRec, tag),
    XmRImmediate, XmS
  },
  {
    XmNfont, XmCFont, XmRFontStruct,
    sizeof(XtPointer), XtOffsetOf(_XmRenditionRec, font),
    XmRImmediate, NULL
  },
  {
    XmNxftFont, XmCXftFont, XmRXftFont,
    sizeof(XtPointer), XtOffsetOf(_XmRenditionRec, xftFont),
    XmRImmediate, NULL
  },
  { /* This will use XmNfontName for backward compatbility */
    XmNfontName, XmCFontName, XmRString,
    sizeof(String), XtOffsetOf(_XmRenditionRec, pattern),
    XmRImmediate, NULL
  },
  {
    XmNfontFoundry, XmCFontFoundry, XmRString,
    sizeof(String), XtOffsetOf(_XmRenditionRec, fontFoundry),
    XmRImmediate, NULL
  },
  {
    XmNfontFamily, XmCFontFamily, XmRString,
    sizeof(String), XtOffsetOf(_XmRenditionRec, fontFamily),
    XmRImmediate, NULL
  },
  {
    XmNfontStyle, XmCFontStyle, XmRString,
    sizeof(String), XtOffsetOf(_XmRenditionRec, fontStyle),
    XmRImmediate, NULL
  },
  {
    XmNfontSize, XmCFontSize, XmRInt,
    sizeof(int), XtOffsetOf(_XmRenditionRec, fontSize),
    XmRImmediate, NULL
  },
  {
    XmNfontPixelSize, XmCFontPixelSize, XmRInt,
    sizeof(int), XtOffsetOf(_XmRenditionRec, pixelSize),
    XmRImmediate, NULL
  },
  {
    XmNfontSlant, XmCFontSlant, XmRFontSlant,
    sizeof(unsigned char), XtOffsetOf(_XmRenditionRec, fontSlant),
    XmRImmediate, (XtPointer)XmAS_IS
  },
  {
    XmNfontWeight, XmCFontWeight, XmRFontWeight,
    sizeof(unsigned char), XtOffsetOf(_XmRenditionRec, fontWeight),
    XmRImmediate, (XtPointer)XmAS_IS
  },
  {
    XmNwidth, XmCWidth, XmRInt,
    sizeof(int), XtOffsetOf(_XmRenditionRec, width),
    XmRImmediate, NULL
  },
  {
    XmNinkWidth, XmCInkWidth, XmRInt,
    sizeof(int), XtOffsetOf(_XmRenditionRec, ink_width),
    XmRImmediate, NULL
  },
  {
    XmNascent, XmCAscent, XmRInt,
    sizeof(int), XtOffsetOf(_XmRenditionRec, ascent),
    XmRImmediate, NULL
  },
  {
    XmNdescent, XmCDescent, XmRInt,
    sizeof(int), XtOffsetOf(_XmRenditionRec, descent),
    XmRImmediate, NULL
  },
  {
    XmNtabList, XmCTabList, XmRTabList,
    sizeof(XmTabList), XtOffsetOf(_XmRenditionRec, tabs),
    XmRImmediate, NULL
  },
  {
    XmNrenditionBackground, XmCRenditionBackground, XmRRenditionPixel,
    sizeof(Pixel), XtOffsetOf(_XmRenditionRec, style.bg.pixel),
    XmRImmediate, (XtPointer)XmUNSPECIFIED_PIXEL
  },
  {
    XmNrenditionForeground, XmCRenditionForeground, XmRRenditionPixel,
    sizeof(Pixel), XtOffsetOf(_XmRenditionRec, style.fg.pixel),
    XmRImmediate, (XtPointer)XmUNSPECIFIED_PIXEL
  },
  {
    XmNunderlineType, XmCUnderlineType, XmRLineType,
    sizeof(unsigned char), XtOffsetOf(_XmRenditionRec, style.underline),
    XmRImmediate, (XtPointer)XmAS_IS
  },
  {
    XmNstrikethruType, XmCStrikethruType, XmRLineType,
    sizeof(unsigned char), XtOffsetOf(_XmRenditionRec, style.strikethru),
    XmRImmediate, (XtPointer)XmAS_IS
  },
  {
    XmNforegroundState, XmCGroundState, XmRGroundState,
    sizeof(unsigned char), XtOffsetOf(_XmRenditionRec, style.fg_state),
    XmRImmediate, (XtPointer)XmAS_IS
  },
  {
    XmNbackgroundState, XmCGroundState, XmRGroundState,
    sizeof(unsigned char), XtOffsetOf(_XmRenditionRec, style.bg_state),
    XmRImmediate, (XtPointer)XmAS_IS
  },
};

static const Cardinal _XmNumRenditionResources = XtNumber(_XmRenditionResources);

/* Searches up widget hierarchy, quarkifying ancestor names and */
/* classes. */
static Cardinal
GetNamesAndClasses(Widget w, XrmNameList names, XrmClassList classes)
{
  Cardinal length, j;
  XrmQuark t;
  WidgetClass wc;

  /* Return null-terminated quark arrays, with length the number of
     quarks (not including NULL) */

  for (length = 0; w != NULL; w = (Widget)w->core.parent)
    {
      names[length] = w->core.xrm_name;
      wc = XtClass(w);
      /* KLUDGE KLUDGE KLUDGE KLUDGE */
      if (w->core.parent == NULL && XtIsApplicationShell(w)) {
	classes[length] =
	  ((ApplicationShellWidget) w)->application.xrm_class;
      } else classes[length] = wc->core_class.xrm_class;
      length++;
    }
  /* They're in backwards order, flop them around */
  for (j = 0; j < length/2; j++)
    {
      t = names[j];
      names[j] = names[length-j-1];
      names[length-j-1] = t;
      t = classes[j];
      classes[j] = classes[length-j-1];
      classes[length-j-1] = t;
    }
  names[length] = NULLQUARK;
  classes[length] = NULLQUARK;
  return length;
}						  /* GetNamesAndClasses */

/* Converts resource list to quarkified list. */
static XrmResourceList
CompileResourceTable(XtResourceList resources,
		     Cardinal num_resources)
{
  Cardinal		count;
  XrmResourceList	table, tPtr;
  XtResourceList	rPtr;

  tPtr = table = (XrmResourceList)XtMalloc(num_resources * sizeof(XrmResource));
  rPtr = resources;

  for (count = 0; count < num_resources; count++, tPtr++, rPtr++)
    {
      tPtr->xrm_name 		= XrmPermStringToQuark(rPtr->resource_name);
      tPtr->xrm_class 		= XrmPermStringToQuark(rPtr->resource_class);
      tPtr->xrm_type 		= XrmPermStringToQuark(rPtr->resource_type);
      tPtr->xrm_size		= rPtr->resource_size;
      tPtr->xrm_offset		= rPtr->resource_offset;
      tPtr->xrm_default_type 	= XrmPermStringToQuark(rPtr->default_type);
      tPtr->xrm_default_addr	= rPtr->default_addr;
    }
  return(table);
}

/* Does resource database lookup for arglist, filling in defaults from */
/* resource list as necessary. */
static Boolean
GetResources(XmRendition rend,
	     Display *dsp,
	     Widget wid,
	     String resname,
	     String resclass,
	     XmStringTag tag,
	     ArgList arglist,
	     Cardinal argcount)
{
  XrmName		names[100];
  XrmClass		classes[100];
  Cardinal		length = 0;
  static XrmQuarkList	quarks = NULL;
  static Cardinal	num_quarks = 0;
  static Boolean	*found = NULL;
  int			i, j;
  static XrmResourceList	table = NULL;
  static XrmQuark	QString, Qfont, QxftFont;
  Arg			*arg;
  XrmName		argName;
  XrmResource		*res;
  XrmDatabase		db = NULL;
  XrmHashTable   	stackSearchList[100];
  XrmHashTable    	*searchList = stackSearchList;
  unsigned int    	searchListSize = 100;
  Boolean		got_one = False;
  XrmValue		value;
  XrmQuark		rawType;
  XrmValue		convValue;
  Boolean		have_value, copied;
  XtAppContext		app=NULL;

  if (wid)
	app = XtWidgetToApplicationContext(wid);
  else if (dsp)
	app = XtDisplayToApplicationContext(dsp);
  if (app)
      _XmAppLock(app);
  else _XmProcessLock();

  /* Initialize quark cache */
  if (quarks == NULL)
    {
      quarks = (XrmQuark *)XtMalloc(_XmNumRenditionResources *
				    sizeof(XrmQuark));
      num_quarks = _XmNumRenditionResources;
    }

  /* Initialize found */
  if (found == NULL)
    found = (Boolean *)XtMalloc(_XmNumRenditionResources * sizeof(Boolean));
  memset(found, 0, _XmNumRenditionResources * sizeof(Boolean));

  /* Compile names and classes. */
  if (wid != NULL)
    length = GetNamesAndClasses(wid, names, classes);

  names[length] = XrmStringToQuark(resname);
  classes[length] = XrmStringToQuark(resclass);
  length++;

  if (tag != NULL)
    {
      names[length] = XrmStringToQuark(tag);
      classes[length] = XrmPermStringToQuark(XmCRendition);
      length++;
    }

  names[length] = NULLQUARK;
  classes[length] = NULLQUARK;

  /* Cache arglist */
  if (num_quarks < argcount)
    {
      quarks = (XrmQuark *)XtRealloc((char *)quarks,
				     argcount * sizeof(XrmQuark));
      num_quarks = argcount;
    }
  for (i = 0; i < argcount; i++)
    quarks[i] = XrmStringToQuark(arglist[i].name);

  /* Compile resource description into XrmResourceList if not already done. */
  if (table == NULL)
    {
      table = CompileResourceTable(_XmRenditionResources,
				   _XmNumRenditionResources);
      QString = XrmPermStringToQuark(XtCString);
      Qfont = XrmPermStringToQuark(XmNfont);
      QxftFont = XrmPermStringToQuark(XmNxftFont);
    }

  /* Set resources from arglist. */
  for (arg = arglist, i = 0; i < argcount; arg++, i++)
    {
      argName = quarks[i];

      for (j = 0, res = table; j < _XmNumRenditionResources; j++, res++)
	{
	  if (res->xrm_name == argName)
	    {
	      CopyFromArg((arg->value),
			  ((char *)GetPtr(rend) + res->xrm_offset),
			  res->xrm_size);
	      found[j] = TRUE;
	      break;
	    }
	}
    }

  /* DB query */
  /* Get database */
  if ((wid != NULL) || (dsp != NULL))
    {
      if (wid != NULL)
	db = XtScreenDatabase(XtScreenOfObject(wid));
      else db = XtScreenDatabase(DefaultScreenOfDisplay(dsp));

      /* Get searchlist */
      while (!XrmQGetSearchList(db, names, classes,
				searchList, searchListSize))
	{
	  if (searchList == stackSearchList)
	    searchList = NULL;
	  searchList = (XrmHashTable *)XtRealloc((char*)searchList,
						 sizeof(XrmHashTable) *
						 (searchListSize *= 2));
	}
    }

  /* Loop over table */
  for (j = 0, res = table; j < _XmNumRenditionResources; j++, res++)
    {
      if (!found[j])
	{
	  copied = False;
	  have_value = False;

	  if ((db != NULL) &&
	      (XrmQGetSearchResource(searchList, res->xrm_name,
				     res->xrm_class, &rawType, &value)))
	    {
	      /* convert if necessary */
	      if (rawType != res->xrm_type)
		{
		  if (wid && res->xrm_name != Qfont && res->xrm_name != QxftFont)
		    {
		      convValue.size = res->xrm_size;
		      convValue.addr = (char *)GetPtr(rend) + res->xrm_offset;
		      copied = have_value = XtConvertAndStore(
		      	wid, XrmQuarkToString(rawType), &value,
		      	XrmQuarkToString(res->xrm_type), &convValue
		      );
		    }
		  else have_value = False;
		}
	      else have_value = True;
	    }

	  if (!got_one && have_value) got_one = True;

	  /* Set defaults */
	  if (!have_value)
	    {
	      CopyFromArg((XtArgVal)(res->xrm_default_addr),
			  ((char *)GetPtr(rend) + res->xrm_offset),
			  res->xrm_size);
	      copied = True;
	    }

	  /* Copy if needed */
	  if (!copied)
	    {
	      if (res->xrm_type == QString)
		*((String *)((char *)GetPtr(rend) + res->xrm_offset)) =
		  value.addr;
	      else if (value.addr != NULL)
		memcpy(((char *)GetPtr(rend) + res->xrm_offset),
		       value.addr, res->xrm_size);
	      else
		memset(GetPtr(rend) + res->xrm_offset, 0, res->xrm_size);
	    }

	}
    }
  if (searchList != stackSearchList) XtFree((char *)searchList);

  if (app)
      _XmAppUnlock(app);
  else _XmProcessUnlock();

  return got_one;
}

static void
CopyFromArg(XtArgVal src, char *dst, unsigned int size)
{
  if (size > sizeof(XtArgVal))
    memcpy((char *)dst, (char *)src, (size_t)size);
  else {
    union {
      long	longval;
      int	intval;
      short	shortval;
      char	charval;
      char*	charptr;
      XtPointer	ptr;
    } u;
    char *p = (char*)&u;
    if      (size == sizeof(long))	    u.longval = (long)src;
    else if (size == sizeof(int))	    u.intval = (int) src;
    else if (size == sizeof(short))	    u.shortval = (short)src;
    else if (size == sizeof(char))	    u.charval = (char)src;
    else if (size == sizeof(XtPointer))	    u.ptr = (XtPointer)src;
    else if (size == sizeof(char*))	    u.charptr = (char*)src;
    else				    p = (char*)&src;

    memcpy((char *)dst, p, (size_t)size);
  }
} /* CopyFromArg */

/* Wrapper for calling XtWarning functions. */
static void RenditionWarning(char *tag, char *type, char *message, Display *d)
{
	const char *params[1];
	Cardinal num_params = 1;

	/**
	 * the MotifWarningHandler installed in VendorS.c knows about
	 * this convention
	 */
	params[0] = XME_WARNING;
	XtAppWarningMsg(
		XtDisplayToApplicationContext(d ? d : _XmGetDefaultDisplay()),
		tag, type, "XmRendition", message, (String *)params, &num_params
	);
}

/**
 * Create and initialize a XmRenditionStyle
 */
XmRenditionStyle XmRenditionStyleCreate(void)
{
	XmRenditionStyle style = (XmRenditionStyle)XtCalloc(1, sizeof *style);

	style->underline  = XmAS_IS;
	style->strikethru = XmAS_IS;
	style->fg_state   = XmAS_IS;
	style->bg_state   = XmAS_IS;
	style->fg.pixel   = XmUNSPECIFIED_PIXEL;
	style->bg.pixel   = XmUNSPECIFIED_PIXEL;
	style->fg.alpha   = 0xffff;
	style->bg.alpha   = 0xffff;
	return style;
}

/**
 * Duplicate a XmRenditionStyle
 */
XmRenditionStyle XmRenditionStyleDup(const XmRenditionStyle style)
{
	XmRenditionStyle s;

	if (!style)
		return XmRenditionStyleCreate();

	s = (XmRenditionStyle)XtCalloc(1, sizeof *style);
	memcpy(s, style, sizeof *s);
	return s;
}

/**
 * Cascade style properties from b to a.
 *
 * Returns True if modifications were made to a, False otherwise
 */
Boolean XmRenditionStyleMerge(XmRenditionStyle a, const XmRenditionStyle b)
{
	Boolean mod = False;

	if (!a || !b)
		return True;

	if (a->underline == XmAS_IS) {
		a->underline = b->underline;
		mod = True;
	}

	if (a->strikethru == XmAS_IS) {
		a->strikethru = b->strikethru;
		mod = True;
	}

	if (a->fg_state != XmFORCE_COLOR && a->fg.pixel == XmUNSPECIFIED_PIXEL) {
		memcpy(&a->fg, &b->fg, sizeof a->fg);
		a->fg_state = b->fg_state;
		mod = True;
	}

	if (a->bg_state != XmFORCE_COLOR && a->bg.pixel == XmUNSPECIFIED_PIXEL) {
		memcpy(&a->bg, &b->bg, sizeof a->bg);
		a->bg_state = b->bg_state;
		mod = True;
	}

	if (!a->fg.alpha) a->fg.alpha = 0xffff;
	if (!a->bg.alpha) a->bg.alpha = 0xffff;
	return mod;
}

/**
 * Free a XmRenditionStyle
 */
void XmRenditionStyleFree(XmRenditionStyle style)
{
	XtFree((XtPointer)style);
}

/**
 * If we somehow end up with an empty RenderTable, try to load
 * the default hardcoded renditions, and add the first one
 * in preference order (Xft, FontSet, Font) to the table and
 * return True if successful. If rend_out is set, it will receive
 * the created rendition, or NULL on failure.
 */
static Boolean rendertable_default_font(XmRenderTable rt, XmRendition *rend_out)
{
	Arg args[4];
	XmRendition rend = NULL;
	struct __XmRenderTableRec *t;
	const struct __XmRenditionRec *r;
	XmScreen s;

	if (rend_out) *rend_out = NULL;
	if (!(t = XmSharedPtrGet(rt)))
		return False;

	/* Try the default Xft font */
	s = XmScreenOfScreen(DefaultScreenOfDisplay(t->display));
	XtSetArg(args[0], XmNloadModel, XmLOAD_IMMEDIATE);
	XtSetArg(args[1], XmNfontName, XmDEFAULT_XFTFONT);
	XtSetArg(args[2], XmNfontType, XmFONT_IS_XFT);
	XtSetArg(args[3], XmNfontPixelSize, 10 + (int)(3 * (XmScreenDpi(s) / 96.)));
	rend = _XmRenditionCreate(t->display, NULL, XmS, XmCRenderTable,
	                          XmFONTLIST_DEFAULT_TAG, args, 4, NULL);
	r = XmSharedPtrGet(rend);
	if (r && !r->xftFont) {
		XmRenditionFree(rend);
		rend = NULL;
	}

	/* See if we have the default fontset */
	if (!rend) {
		XtSetArg(args[1], XmNfontName, XmDEFAULT_FONTSET);
		XtSetArg(args[2], XmNfontType, XmFONT_IS_FONTSET);
		rend = _XmRenditionCreate(t->display, NULL, XmS, XmCRenderTable,
		                          XmFONTLIST_DEFAULT_TAG, args, 3, NULL);
		r = XmSharedPtrGet(rend);
		if (r && r->font) {
			XmRenditionFree(rend);
			rend = NULL;
		}
	}

	/* Fall back to the default core font */
	if (!rend) {
		XtSetArg(args[1], XmNfontName, XmDEFAULT_FONT);
		XtSetArg(args[2], XmNfontType, XmFONT_IS_FONT);
		rend = _XmRenditionCreate(t->display, NULL, XmS, XmCRenderTable,
		                          XmFONTLIST_DEFAULT_TAG, args, 3, NULL);
		r = XmSharedPtrGet(rend);
		if (r && !r->font) {
			XmRenditionFree(rend);
			rend = NULL;
		}
	}

	/* Add it to the render table */
	if (rend) {
		t->renditions = (XmRendition *)XtRealloc(
			(XtPointer)t->renditions,
			(t->count + 1) * sizeof *t->renditions
		);
		t->renditions[t->count++] = rend;

		if (r && !_XmGetHashEntry(t->ht, r->tag))
			_XmAddHashEntry(t->ht, r->tag, rend);
	}

	if (rend_out) *rend_out = XmSharedPtrCopy(rend, False);
	return !!rend;
}

/**
 * Free a rendition table struct
 */
static void rendertable_free(void *ptr)
{
	Cardinal i;
	struct __XmRenderTableRec *t = (struct __XmRenderTableRec *)ptr;

	for (i = 0; i < t->count; i++)
		XmSharedPtrFree(t->renditions[i]);
	_XmFreeHashTable(t->ht);
	XtFree((XtPointer)t->renditions);
	XtFree((XtPointer)t);
	return;
}

/**
 * Duplicate a rendition table struct
 */
static void *rendertable_dup(void *ptr)
{
	Cardinal i;
	const struct __XmRenderTableRec *t = (const struct __XmRenderTableRec *)ptr;
	struct __XmRenderTableRec *new;

	new = (struct __XmRenderTableRec *)XtCalloc(1, sizeof *new);
	new->ht      = _XmAllocHashTable(16, XmHashCompareString, XmHashString);
	new->display = t->display;
	if (!t->count)
		return new;

	new->count      = t->count;
	new->renditions = (XmRendition *)XtCalloc(t->count, sizeof *t->renditions);
	for (i = 0; i < t->count; i++) {
		new->renditions[i] = XmSharedPtrCopy(t->renditions[i], True);
		if (!_XmGetHashEntry(new->ht, (*t->renditions[i])->tag))
			_XmAddHashEntry(new->ht, (*t->renditions[i])->tag, new->renditions[i]);
	}

	return new;
}

/**
 * Allocate a new RenderTable
 */
XmRenderTable XmRenderTableCreate(Widget parent)
{
	return (XmRenderTable)_XmCreateRenderTable(parent, NULL, NULL, 0);
}

/* Mrm create function for rendertables. */
Widget _XmCreateRenderTable(Widget parent, String name, ArgList args, Cardinal count)
{
	struct __XmRenderTableRec *table;
	const struct __XmRenderTableRec *p;

	(void)name;
	(void)args;
	(void)count;

	table     = (struct __XmRenderTableRec *)XtCalloc(1, sizeof *table);
	table->ht = _XmAllocHashTable(16, XmHashCompareString, XmHashString);
	table->display = parent ? XtDisplay(parent) : _XmGetDefaultDisplay();
	return (Widget)XmSharedPtrCreate(table, rendertable_free, rendertable_dup);
}

/**
 * Extern function to pick out display from rendertable.
 * Used by Mrm.
 */
Display * _XmRenderTableDisplay(XmRenderTable table)
{
	const struct __XmRenderTableRec *rt;

	rt = XmSharedPtrGet(table);
	return rt ? rt->display : NULL;
}

/**
 * Duplicates renditions matching tags to a new table.
 */
XmRenderTable XmRenderTableCopy(XmRenderTable table, XmStringTag *tags,
                                int tag_count)
{
	Cardinal i, count = 0;
	XmRenderTable new;
	XmRendition *matches;
	struct __XmRenderTableRec *new_rt;
	const struct __XmRenderTableRec *rt;
	XtAppContext app;

	if (!(rt = XmSharedPtrGet(table)))
		return NULL;

	app = _XmLock(rt->display);
	if (!tags || !tag_count) {
		_XmUnlock(app);
		return XmSharedPtrCopy(table, True);
	}

	if (!(new = XmRenderTableCreate(NULL))) {
		_XmUnlock(app);
		return NULL;
	}

	new_rt = XmSharedPtrGet(new);
	new_rt->display = rt->display;
	new_rt->renditions = (XmRendition *)XtCalloc(tag_count, sizeof *new_rt->renditions);
	if ((matches = XmRenderTableGetRenditions(table, tags, tag_count))) {
		for (i = 0; i < (Cardinal)tag_count; i++) {
			if (matches[i]) {
				new_rt->renditions[count++] = XmSharedPtrCopy(matches[i], True);
				if (!_XmGetHashEntry(new_rt->ht, (*new_rt->renditions[count - 1])->tag)) {
					_XmAddHashEntry(new_rt->ht,
					                (*new_rt->renditions[count - 1])->tag,
					                new_rt->renditions[count - 1]);
				}
				XmRenditionFree(matches[i]);
			}
		}
	}
	XtFree((XtPointer)matches);

	new_rt->renditions = (XmRendition *)XtRealloc(
		(XtPointer)new_rt->renditions,
		count * sizeof *new_rt->renditions
	);

	if (!(new_rt->count = count))
		new_rt->renditions = NULL;
	_XmUnlock(app);
	return new;
}

/**
 * Get list of tags of all renditions in table.
 */
int XmRenderTableGetTags(XmRenderTable table, XmStringTag **tag_list)
{
	Cardinal i;
	int count = 0;
	const struct __XmRenderTableRec *rt;
	const struct __XmRenditionRec *r;
	XtAppContext app;

	if (!(rt = XmSharedPtrGet(table)) || !tag_list || !rt->count) {
		if (tag_list) *tag_list = NULL;
		return 0;
	}

	app = _XmLock(rt->display);
	*tag_list = (XmStringTag *)XtCalloc(rt->count, sizeof **tag_list);
	for (i = 0; i < rt->count; i++) {
		if (!(r = XmSharedPtrGet(rt->renditions[i])))
			continue;
		*tag_list[count++] = XtNewString(r->tag);
	}

	*tag_list = (XmStringTag *)XtRealloc((XtPointer)*tag_list, count * sizeof **tag_list);
	if (!count) *tag_list = NULL;
	_XmUnlock(app);
	return count;
}

/**
 * Get renditions matching particular tags from a render table
 */
XmRendition *XmRenderTableGetRenditions(XmRenderTable table,
                                        XmStringTag *tags, Cardinal tag_count)
{
	XmRendition rend, *rends;
	Cardinal i, count;
	const struct __XmRenderTableRec *t;
	XtAppContext app;

	if (!(t = XmSharedPtrGet(table)) || !tags || !tag_count)
		return NULL;

	app   = _XmLock(t->display);
	rends = (XmRendition *)XtCalloc(tag_count, sizeof rend);

	for (i = 0; i < tag_count; i++) {
		if (tags[i] && (rend = _XmGetHashEntry(t->ht, tags[i])))
			rends[i] = XmSharedPtrCopy(rend, False);
	}

	_XmUnlock(app);
	return rends;
}

/**
 * Get the rendition in the table matching the given tag.
 */
XmRendition XmRenderTableGetRendition(XmRenderTable table, XmStringTag tag)
{
	XmRendition r = NULL;
	const struct __XmRenderTableRec *t;
	XtAppContext app;

	if (!(t = XmSharedPtrGet(table)) || !tag)
		return NULL;

	app = _XmLock(t->display);
	r   = _XmGetHashEntry(t->ht, tag);
	r   = XmSharedPtrCopy(r, False);
	_XmUnlock(app);
	return r;
}

/***
 * Add a series of entries to a rendertable, handling tag conflicts
 * according to \a merge_mode.
 *
 * Like most rendertable ops, this assumes tags are unique in the
 * renditions array / table. Only the first entry with a particular tag
 * is considered when matching by tag.
 *
 * Renditions with an empty tag (XmS) will be copied thru.
 */
XmRenderTable XmRenderTableAddRenditions(XmRenderTable oldtable,  XmRendition *renditions,
                                         Cardinal rendition_count, XmMergeMode merge_mode)
{

	Cardinal i, count = 0;
	struct __XmRenderTableRec *t, *new;
	XmRendition r, m;
	XmRenderTable newtable;
	XmHashTable ht;
	XtAppContext app;

	if (!renditions || !rendition_count)
		return oldtable;

	if (!(t = XmSharedPtrGet(oldtable)))
		oldtable = NULL;
	app = _XmLock(t ? t->display : _XmGetDefaultDisplay());

	/**
	 * If we don't have an old table, create a new one containing
	 * the given renditions.
	 */
	if (!oldtable) {
		oldtable = XmRenderTableCreate(NULL);
		t = XmSharedPtrGet(oldtable);
		t->count      = rendition_count;
		t->renditions = (XmRendition *)XtMalloc(rendition_count * sizeof *t->renditions);

		for (i = 0; i < rendition_count; i++) {
			t->renditions[i] = XmSharedPtrCopy(renditions[i], False);
			if (!_XmGetHashEntry(t->ht, (*renditions[i])->tag))
				_XmAddHashEntry(t->ht, (*renditions[i])->tag, t->renditions[i]);
		}

		_XmUnlock(app);
		return oldtable;
	}

	/**
	 * Otherwise, merge the two rendition lists into a newly-allocated
	 * table.
	 */
	newtable = XmRenderTableCreate(NULL);
	new      = XmSharedPtrGet(newtable);
	ht       = _XmAllocHashTable(0, XmHashCompareString, XmHashString);
	new->display = t->display;

	/**
	 * Only the first instance of a particular tag is considered
	 * when matching, in keeping with existing RenderTable customs.
	 *
	 * This hash table speeds up searching the rendition list by tag.
	 */
	for (i = 0; i < rendition_count; i++) {
		if (*(*renditions[i])->tag && !_XmGetHashEntry(ht, (*renditions[i])->tag))
			_XmAddHashEntry(ht, (*renditions[i])->tag, renditions[i]);
	}

	new->renditions = (XmRendition *)XtMalloc(
		(t->count + rendition_count) * sizeof *new->renditions
	);

	for (i = 0; i < t->count; i++) {
		/* Copy renditions thru that don't match the rendition list */
		if (!(r = _XmGetHashEntry(ht, (*t->renditions[i])->tag))) {
			new->renditions[count++] = t->renditions[i];
			if (!_XmGetHashEntry(new->ht, (*t->renditions[i])->tag))
				_XmAddHashEntry(new->ht, (*t->renditions[i])->tag, t->renditions[i]);
			t->renditions[i] = NULL;
			continue;
		}

		/**
		 * We have a hit, merge the two renditions
		 */
		switch (merge_mode) {
		case XmMERGE_OLD: /* Dup the old, update it with the new */
			m = XmSharedPtrCopy(t->renditions[i], True);
			merge_renditions(m, r);
			new->renditions[count++] = m;
			if (!_XmGetHashEntry(new->ht, (*r)->tag))
				_XmAddHashEntry(new->ht, (*r)->tag, new->renditions[count - 1]);
			_XmRemoveHashEntry(ht, (*r)->tag);
			break;
		case XmMERGE_NEW: /* Dup the new, update it with the old */
			r = XmSharedPtrCopy(r, True);
			merge_renditions(r, t->renditions[i]);
			new->renditions[count++] = r;
			if (!_XmGetHashEntry(new->ht, (*r)->tag))
				_XmAddHashEntry(new->ht, (*r)->tag, new->renditions[count - 1]);
			_XmRemoveHashEntry(ht, (*r)->tag);
			break;
		case XmMERGE_REPLACE: /* Take the supplied rendition */
			new->renditions[count++] = XmSharedPtrCopy(r, False);
			if (!_XmGetHashEntry(new->ht, (*r)->tag))
				_XmAddHashEntry(new->ht, (*r)->tag, new->renditions[count - 1]);
			_XmRemoveHashEntry(ht, (*r)->tag);
			break;
		case XmDUPLICATE: /* Copy both */
			new->renditions[count++] = XmSharedPtrCopy(t->renditions[i], False);
			if (!_XmGetHashEntry(new->ht, (*t->renditions[i])->tag))
				_XmAddHashEntry(new->ht, (*t->renditions[i])->tag, new->renditions[count - 1]);
			break;
		case XmSKIP: /* Copy neither */
			_XmRemoveHashEntry(ht, (*r)->tag);
			break;
		}
	}

	/**
	 * Copy unhandled entries from the provided rendition array.
	 */
	for (i = 0; i < rendition_count; i++) {
		if (*(*renditions[i])->tag && !_XmGetHashEntry(ht, (*renditions[i])->tag))
			continue;
		new->renditions[count++] = XmSharedPtrCopy(renditions[i], False);
		if (!_XmGetHashEntry(new->ht, (*renditions[i])->tag))
			_XmAddHashEntry(new->ht, (*renditions[i])->tag, new->renditions[count - 1]);
	}

	new->renditions = (XmRendition *)XtRealloc(
		(XtPointer)new->renditions,
		count * sizeof *new->renditions
	);

	if (!(new->count = count))
		new->renditions = NULL;

	XmRenderTableFree(oldtable);
	_XmFreeHashTable(ht);
	_XmUnlock(app);
	return newtable;
}

/**
 * Remove all renditions from oldtable matching the given tags.
 *
 * If all renditions would be removed, free oldtable and return NULL.
 * Otherwise, free any matching renditions along with oldtable, and
 * return a newly-allocated rendertable containing the renditions
 * that didn't match the given tags.
 */
XmRenderTable XmRenderTableRemoveRenditions(XmRenderTable oldtable,
                                            XmStringTag *tags, int tag_count)
{
	Boolean match;
	Cardinal i, count = 0;
	int j;
	XmRenderTable ret;
	const struct __XmRenditionRec *r;
	struct __XmRenderTableRec *rt, *new;
	XtAppContext app;

	if (!(rt = XmSharedPtrGet(oldtable)) || !tags || !tag_count)
		return oldtable;

	app = _XmLock(rt->display);
	ret = XmRenderTableCreate(NULL);
	new = XmSharedPtrGet(ret);
	new->display    = rt->display;
	new->renditions = (XmRendition *)XtMalloc(rt->count * sizeof *new->renditions);

	/**
	 * Filter-out renditions matching the given tags
	 * by stealing renditions that don't match.
	 */
	for (i = 0; i < rt->count; i++) {
		match = False;
		r = XmSharedPtrGet(rt->renditions[i]);

		for (j = 0; j < tag_count; j++) {
			if (tags[j] && (tags[j] == r->tag || !strcmp(tags[j], r->tag))) {
				match = True;
				break;
			}
		}

		if (!match) {
			new->renditions[count++] = rt->renditions[i];
			_XmRemoveHashEntry(rt->ht, (*rt->renditions[i])->tag);
			if (!_XmGetHashEntry(new->ht, (*rt->renditions[i])->tag)) {
				_XmAddHashEntry(new->ht,
				               (*rt->renditions[i])->tag,
				               rt->renditions[i]);
			}
			rt->renditions[i] = NULL;
		}
	}

	if ((new->count = count)) {
		new->renditions = (XmRendition *)XtRealloc(
			(XtPointer)new->renditions,
			count * sizeof *new->renditions
		);
	} else {
		XmRenderTableFree(ret);
		ret = NULL;
	}

	XmRenderTableFree(oldtable);
	_XmUnlock(app);
	return ret;
}

/**
 * Finds the first font in the rendertable in the following order:
 * 1. Xft
 * 2. XFontSet
 * 3. XFontStruct
 *
 * This function is used by XmFontList.
 */
Boolean _XmRenderTableFindFirstFont(XmRenderTable rt, XmRendition *rend_out)
{
	Cardinal i, f_idx = UINT_MAX, fs_idx = UINT_MAX;
	struct __XmRenderTableRec *t;
	const struct __XmRenditionRec *r;
	XmScreen s;
	Cardinal xft_idx = UINT_MAX;

	if (!rend_out || !(t = XmSharedPtrGet(rt)))
		return False;

	*rend_out = NULL;
	for (i = t->count - 1; i < UINT_MAX; i--) {
		r = XmSharedPtrGet(t->renditions[i]);
		if (r->loadModel == XmLOAD_DEFERRED)
			XmRenditionLoad(t->renditions[i], True);

		if (r->font) {
			if (r->fontType == XmFONT_IS_FONT)         f_idx  = i;
			else if (r->fontType == XmFONT_IS_FONTSET) fs_idx = i;
		} else if (r->xftFont && r->fontType == XmFONT_IS_XFT) xft_idx = i;
	}

	if (xft_idx < UINT_MAX) {
		if (rend_out) *rend_out = XmSharedPtrCopy(t->renditions[xft_idx], False);
		return True;
	} else if (fs_idx < UINT_MAX) {
		if (rend_out) *rend_out = XmSharedPtrCopy(t->renditions[fs_idx], False);
		return True;
	} else if (f_idx < UINT_MAX) {
		if (rend_out) *rend_out = XmSharedPtrCopy(t->renditions[f_idx], False);
		return True;
	} else if (rendertable_default_font(rt, rend_out))
		return True;

	return False;
}

/**
 * Call the "no rendition" callback.
 *
 * This should happen when we don't find a rendition in a rendertable
 * matching a particular tag, and only gets called during the process
 * of rendering a XmString segment.
 *
 * Returns True if we added a rendition to \a rt for the given tag,
 * False otherwise.
 */
static Boolean no_rendition(XmRenderTable rt, XmStringTag tag)
{
	Boolean ret = False;
	XmDisplay d;
	XmRendition r;
	XmDisplayCallbackStruct cb;
	struct __XmRenderTableRec *t;
	const struct __XmRenderTableRec *t2;

	if (!(t = XmSharedPtrGet(rt)) || !(d = (XmDisplay)XmGetXmDisplay(t->display)))
		return ret;

	if (!d->display.noRenditionCallback)
		return ret;

	/* NB: We copy the rendertable so the callback can't rug-pull it */
	memset(&cb, 0, sizeof cb);
	cb.reason       = XmCR_NO_RENDITION;
	cb.render_table = XmSharedPtrCopy(rt, False);
	cb.tag          = tag;

	/**
	 * The callback will be expected to free the render table passed
	 * via the callback struct, and present us with a new render table
	 * containing a rendition for the given tag.
	 *
	 * See: XmDisplay(3)
	 */
	XtCallCallbackList((Widget)d, d->display.noRenditionCallback, &cb);

	/**
	 * Best thing we can do here is to add the rendition supplied
	 * in the table to the original table. The previous implementation
	 * allowed the application to effectively nuke the rendertable by
	 * changing the table's handle to point to the new one. This is
	 * much safer.
	 */
	if (cb.render_table != rt && (t2 = XmSharedPtrGet(cb.render_table))) {
		if ((r = _XmGetHashEntry(t2->ht, tag))) {
			t->renditions = (XmRendition *)XtRealloc(
				(XtPointer)t->renditions,
				(t->count + 1) * sizeof *t->renditions
			);

			t->renditions[t->count++] = XmSharedPtrCopy(r, False);
			if (!_XmGetHashEntry(t->ht, tag))
				_XmAddHashEntry(t->ht, tag, t->renditions[t->count - 1]);
			ret = True;
		}

		XmRenderTableFree(cb.render_table);
	}

	return ret;
}

/**
 * Set the rendition (if we finally have a font) and cascade style props.
 *
 * Returns True if the rendition has a font and the style has been fully
 * populated
 */
static Boolean cascade(XmRendition *rend, XmRendition tmp, XmRenditionStyle style)
{
	Boolean style_mod = False;
	struct __XmRenditionRec *r;

	if (!(r = XmSharedPtrGet(tmp)))
		return False;

	if (!*rend && r->loadModel == XmLOAD_DEFERRED && !r->font && !r->xftFont)
		XmRenditionLoad(tmp, True);

	if (!*rend && (r->font || r->xftFont))
		*rend = tmp;

	style_mod = XmRenditionStyleMerge(style, &r->style);
	return *rend && !style_mod;
}

/**
 * Resolve the rendition and style properties for the given set of tags,
 * and fallback tag.
 *
 * If \a style is specified, the merged styles are placed therein.
 */
XmRendition XmRenderTableResolve(XmRenderTable rt, XmStringTag *tags,
                                 Cardinal count, XmStringTag fallback,
                                 XmRenditionStyle style)
{
	Cardinal i;
	Boolean complete = False;
	XmRendition rend = NULL, tmp;
	struct __XmRenditionRec *r;

	if (!rt)
		return NULL;

	/**
	 * First, search by tag (in reverse order -- in keeping with the
	 * previous implementation) taking the first font, and cascading
	 * the style values.
	 *
	 * If we don't find a tag, and the NO_RENDITION callback yields
	 * a new rendition, try again.
	 */
	if (tags && count) {
		for (i = count - 1; i < UINT_MAX; i--) {
			if ((tmp = XmRenderTableGetRendition(rt, tags[i]))) {
				if (cascade(&rend, tmp, style))
					break;
			} else if (no_rendition(rt, tags[i])) ++i;
		}
	}

	/* If we don't have a font, keep trying to find one */
	if ((r = XmSharedPtrGet(rend)) && !r->font && !r->xftFont) {
		XmRenditionFree(rend);
		rend = NULL;
	}

	/* Next, try the fallback tag */
	if (!rend && !(rend = XmRenderTableGetRendition(rt, fallback))) {
		/* Try the current charset */
		fallback = XmStringGetCharset();
		rend     = XmRenderTableGetRendition(rt, fallback);
		XtFree(fallback);
	}

	if ((r = XmSharedPtrGet(rend)) && r->loadModel == XmLOAD_DEFERRED)
		XmRenditionLoad(rend, True);

	if (r && !r->font && !r->xftFont) {
		XmRenditionFree(rend);
		rend = NULL;
	}

	/* ... and failing that, the default tag (Motif 1.x) */
	if (!rend)
		rend = XmRenderTableGetRendition(rt, XmFONTLIST_DEFAULT_TAG);

	if ((r = XmSharedPtrGet(rend)) && r->loadModel == XmLOAD_DEFERRED)
		XmRenditionLoad(rend, True);

	if (r && !r->font && !r->xftFont) {
		XmRenditionFree(rend);
		rend = NULL;
	}

	/* Motif 2.x: Used for multibyte strings */
	if (!rend)
		rend = XmRenderTableGetRendition(rt, _MOTIF_DEFAULT_LOCALE);

	if ((r = XmSharedPtrGet(rend)) && r->loadModel == XmLOAD_DEFERRED)
		XmRenditionLoad(rend, True);

	if (r && !r->font && !r->xftFont) {
		XmRenditionFree(rend);
		rend = NULL;
	}

	/* Finally... */
    if (!rend)
    	_XmRenderTableFindFirstFont(rt, &rend);

out:
	if (rend && (r = XmSharedPtrGet(rend))) {
		XmRenditionStyleMerge(style, &r->style);
		XmRenditionLoad(rend, False);
	}

	return rend;
}

/**
 * Get the width, height, ascent, and descent of the font corresponding
 * to XmFONTLIST_DEFAULT_TAG, or the default if such could not be found.
 *
 * "width" is the maximum character layout width supplied by the rendition.
 */
void XmRenderTableGetDefaultExtents(XmRenderTable rt, int *width, int *ink_width,
                                    int *height, int *ascent, int *descent)
{
	int n = 0, a = 0, d = 0;
	Arg arg[4];
	XmRendition rend;
	const struct __XmRenderTableRec *t;
	XtAppContext app;

	if (ink_width) *ink_width = 0;
	if (width)     *width     = 0;

	if (!(t = XmSharedPtrGet(rt)))
		goto out;
	app = _XmLock(t->display);

	/* Get default rendition */
	if (!(rend = XmRenderTableResolve(rt, NULL, 0, XmFONTLIST_DEFAULT_TAG, NULL))) {
		if (!rendertable_default_font(rt, &rend))
			goto unlock;
	}

	if (width)     { XtSetArg(arg[n], XmNwidth,    width);     n++; }
	if (ink_width) { XtSetArg(arg[n], XmNinkWidth, ink_width); n++; }
	XtSetArg(arg[n], XmNascent,  &a); n++;
	XtSetArg(arg[n], XmNdescent, &d); n++;
	XmRenditionLoad(rend, False);
	XmRenditionGetValues(rend, arg, n);
	XmRenditionFree(rend);

unlock:
	_XmUnlock(app);

out:
	if (height)  *height  = a + d;
	if (ascent)  *ascent  = a;
	if (descent) *descent = d;
}

/**
 * Free a XmRenderTable
 */
void XmRenderTableFree(XmRenderTable table)
{
	XmSharedPtrFree(table);
}

/**
 * Copy a color from one rendition to another, reallocating from the
 * colormap if needed
 */
static void dup_colors(struct __XmRenditionRec *to, const struct __XmRenditionRec *from)
{
	Colormap from_map, to_map;
	XColor c;

	from_map = DefaultColormap(from->display, DefaultScreen(from->display));
	to_map   = DefaultColormap(to->display, DefaultScreen(to->display));
	to->free_bg = False;
	to->free_fg = False;

	if (from->style.fg.pixel != XmUNSPECIFIED_PIXEL &&
	    to->style.fg.pixel == XmUNSPECIFIED_PIXEL) {
		if (from->free_fg) {
			c.pixel = from->style.fg.pixel;
			XQueryColor(from->display, from_map, &c);
			if (XAllocColor(to->display, to_map, &c)) {
				to->style.fg.pixel = c.pixel;
				to->free_fg = True;
			}
		} else to->style.fg.pixel = from->style.fg.pixel;
	}

	if (from->style.bg.pixel != XmUNSPECIFIED_PIXEL &&
	    to->style.bg.pixel == XmUNSPECIFIED_PIXEL) {
		if (from->free_bg) {
			c.pixel = from->style.bg.pixel;
			XQueryColor(from->display, from_map, &c);
			if (XAllocColor(to->display, to_map, &c)) {
				to->style.bg.pixel = c.pixel;
				to->free_bg = True;
			}
		} else to->style.bg.pixel = from->style.bg.pixel;
	}
}

/**
 * Merge two renditions, replacing any default values in \a to with
 * values from \a from.
 */
static void merge_renditions(XmRendition to, XmRendition from)
{
	struct __XmRenditionRec *rt, *rf;

	rt = XmSharedPtrGet(to);
	rf = XmSharedPtrGet(from);
	if (!rt || !rf)
		return;

	if (rt->loadModel == XmAS_IS)
		rt->loadModel = rf->loadModel;

	if (!rt->tag)
		rt->tag = _XmStringCacheTag(rf->tag, XmSTRING_TAG_STRLEN);

	if (rt->fontType == XmAS_IS)
		rt->fontType = rf->fontType;

	if (!rt->font && rf->font) {
		XtFree(rt->pattern);
		rt->pattern = NULL;
		if (rf->pattern)
			rt->pattern = XtNewString(rf->pattern);
		XmRenditionLoad(to, False);
	}

	if (!rt->tabs && rf->tabs)
		rt->tabs = XmTabListCopy(rf->tabs, 0, 0);

	if (rt->style.underline == XmAS_IS)
		rt->style.underline = rf->style.underline;

	if (rt->style.strikethru == XmAS_IS)
		rt->style.strikethru = rf->style.strikethru;

	if (rt->style.bg_state == XmAS_IS)
		rt->style.bg_state = rf->style.bg_state;

	if (rt->style.fg_state == XmAS_IS)
		rt->style.fg_state = rf->style.fg_state;

	if (rt->style.bg.pixel == XmUNSPECIFIED_PIXEL ||
	    rt->style.fg.pixel == XmUNSPECIFIED_PIXEL)
		dup_colors(rt, rf);

	/* These should only be updated if the Xft font changes */
	if (rt->fontType == XmFONT_IS_XFT && !rt->xftFont && rf->xftFont) {
		XtFree(rt->pattern);
		rt->pattern     = NULL;
		rt->xftFont     = XftFontCopy(rf->display, rf->xftFont);
		rt->fontFoundry = rf->fontFoundry;
		rt->fontFamily  = rf->fontFamily;
		rt->fontStyle   = rf->fontStyle;
		rt->pixelSize   = rf->pixelSize;
		rt->width       = rf->width;
		rt->ink_width   = rf->ink_width;
		rt->ascent      = rf->ascent;
		rt->descent     = rf->descent;
		rt->display     = rf->display;

		if (rf->pattern)
			rt->pattern = XtNewString(rf->pattern);
	} else if (!rt->xftFont && !rf->xftFont) {
		if (!rt->pattern && rf->pattern)
			rt->pattern = XtNewString(rf->pattern);
	}
}

/**
 * Free a rendition struct
 */
static void rendition_free(void *ptr)
{
	Display *d;
	Colormap cmap;
	struct __XmRenditionRec *r = ptr;

	if (!r) return;
	d = r->display ? r->display : _XmGetDefaultDisplay();
	XtFree(r->pattern);
	XmTabListFree(r->tabs);

	if (r->fontType != XmFONT_IS_XFT || !r->xftFont) {
		/* These are copied from args or the X font info */
		XtFree(r->fontFamily);
		XtFree(r->fontStyle);
		XtFree(r->fontFoundry);
	} else {
		/* These belong to the XftFont's pattern */
		r->fontFamily  = NULL;
		r->fontStyle   = NULL;
		r->fontFoundry = NULL;
	}

	/* Release font if we own it */
	if (r->loadModel != XmUNSPECIFIED_LOAD_MODEL) {
		switch (r->fontType) {
		case XmFONT_IS_FONT:
			if (r->font) XFreeFont(d, r->font);
			r->font = NULL;
			break;
		case XmFONT_IS_FONTSET:
			if (r->font) XFreeFontSet(d, (XFontSet)r->font);
			r->font = NULL;
			break;
		case XmFONT_IS_XFT:
			if (r->xftFont) XftFontClose(d, r->xftFont);
			r->xftFont = NULL;
			break;
		}
	}

	/* Free colors if we allocated them */
	cmap = DefaultColormap(d, DefaultScreen(d));
	if (r->free_fg) XFreeColors(d, cmap, &r->style.fg.pixel, 1, 0);
	if (r->free_bg) XFreeColors(d, cmap, &r->style.bg.pixel, 1, 0);
	r->free_fg = False;
	r->free_bg = False;
	XtFree((XtPointer)r);
}

/**
 * Duplicate a rendition struct
 */
static void *rendition_dup(void *ptr)
{
	Display *d;
	XmSharedPtr p;
	int mcnt;
	char **mcset = NULL, *def_str;
	struct __XmRenditionRec *new;
	const struct __XmRenditionRec *r = ptr;

	d = r->display;
	if (!d) d = _XmGetDefaultDisplay();

	new = (struct __XmRenditionRec *)XtCalloc(1, sizeof *new);
	new->loadModel         = r->loadModel;
	new->fontType          = r->fontType;
	new->tag               = r->tag;
	new->display           = d;
	new->tabs              = XmTabListCopy(r->tabs, 0, 0);
	new->fontSize          = r->fontSize;
	new->fontSlant         = r->fontSlant;
	new->fontSpacing       = r->fontSpacing;
	new->fontWeight        = r->fontWeight;
	new->pixelSize         = r->pixelSize;
	new->style.underline   = r->style.underline;
	new->style.strikethru  = r->style.strikethru;
	new->style.fg_state    = r->style.fg_state;
	new->style.bg_state    = r->style.bg_state;
	new->style.fg.pixel    = XmUNSPECIFIED_PIXEL;
	new->style.bg.pixel    = XmUNSPECIFIED_PIXEL;
	dup_colors(new, r);

	if (r->pattern)
		new->pattern = XtNewString(r->pattern);

	if (r->fontType == XmFONT_IS_XFT) {
		new->fontFoundry = r->fontFoundry;
		new->fontFamily  = r->fontFamily;
		new->fontStyle   = r->fontStyle;
		if (r->loadModel == XmUNSPECIFIED_LOAD_MODEL)
			new->xftFont = r->xftFont;
		else if (r->xftFont) new->xftFont = XftFontCopy(d, r->xftFont);
	} else {
		new->fontFoundry = XtNewString(r->fontFoundry);
		new->fontFamily  = XtNewString(r->fontFamily);
		new->fontStyle   = XtNewString(r->fontStyle);

		if (r->loadModel == XmUNSPECIFIED_LOAD_MODEL)
			new->font = r->font;
		else if (r->font && r->pattern) {
			switch (r->fontType) {
			case XmFONT_IS_FONT:
				new->font = XLoadQueryFont(d, r->pattern);
				break;
			case XmFONT_IS_FONTSET:
				new->font = (XtPointer)XCreateFontSet(d, r->pattern, &mcset, &mcnt, &def_str);
				if (mcset) XFreeStringList(mcset);
				break;
			default:
				new->font = NULL;
			}
		}
	}

	return new;
}

/**
 * Create a new rendition
 */
XmRendition XmRenditionCreate(Widget widget, XmStringTag tag,
                              ArgList args, Cardinal count)
{
	return _XmRenditionCreate(NULL, widget, XmS, XmCRenderTable, tag,
	                          args, count, NULL);
}

/* Internal function.  Called from XmRenditionCreate, resource */
/* converter, and Mrm create function. */
XmRendition _XmRenditionCreate(Display *display, Widget widget, String resname,
                               String resclass, XmStringTag tag, ArgList args,
                               Cardinal count, Boolean *in_db)
{
	XmRendition rend;
	struct __XmRenditionRec *r;
	Boolean result;

	if (!display)
		display = widget ? XtDisplayOfObject(widget) : _XmGetDefaultDisplay();

	/* Allocate rendition. */
	r    = (struct __XmRenditionRec *)XtCalloc(1, sizeof *r);
	rend = (XmRendition)XmSharedPtrCreate(r, rendition_free, rendition_dup);
	r->display = display;

	/* Ensure we're fully opaque */
	r->style.bg.alpha = 0xffff;
	r->style.fg.alpha = 0xffff;
	r->free_bg        = False;
	r->free_fg        = False;

	/* X resource DB query */
	result = GetResources(rend, display, widget, resname, resclass, tag,
	                      args, count);
	if (in_db) *in_db = result;

	if (!tag && !result) {
		XmRenditionFree(rend);
		return NULL;
	} else if (!tag)
		tag = XmFONTLIST_DEFAULT_TAG;

	/* Just in case we get junk from Xrm */
	if (r->loadModel != XmUNSPECIFIED_LOAD_MODEL) {
		r->font    = NULL;
		r->xftFont = NULL;
	}

	r->tag = _XmStringCacheTag(tag, XmSTRING_TAG_STRLEN);
	if (r->tabs)      r->tabs      = XmTabListCopy(r->tabs, 0, 0);
	if (r->pattern)   r->pattern   = XtNewString(r->pattern);
	if (r->fontStyle) r->fontStyle = XtNewString(r->fontStyle);
	if (r->loadModel != XmLOAD_DEFERRED && r->loadModel != XmLOAD_LAZY) {
		if (!XmRenditionLoad(rend, True) && result)
			RenditionWarning(r->tag, "LOAD_FAILED", LOAD_FAILED, r->display);
	}
	return rend;
}

/* Mrm create function for renditions. */
Widget _XmCreateRendition(Widget parent, String name, ArgList args, Cardinal count)
{
	XmRenderTable rt = (XmRenderTable)parent;
	struct __XmRenderTableRec *table;
	const struct __XmRenditionRec *r;
	XmRendition rend;

	if (!(table = XmSharedPtrGet(rt)))
		return NULL;

	rend = _XmRenditionCreate(table->display, NULL, XmS, XmCRenderTable,
	                          name, args, count, NULL);
	r    = XmSharedPtrGet(rend);

	/* Ignore repeats */
	if (_XmGetHashEntry(table->ht, r->tag)) {
		XmRenditionFree(rend);
		return NULL;
	}

	table->renditions = (XmRendition *)XtRealloc(
		(XtPointer)table->renditions,
		++table->count * sizeof rend
	);

	table->renditions[table->count - 1] = rend;
	if (!_XmGetHashEntry(table->ht, r->tag))
		_XmAddHashEntry(table->ht, r->tag, rend);
	return XmSharedPtrCopy(rend, False);
}

/**
 * Determine if a rendition has a glyph for a particular codepoint
 */
Boolean XmRenditionHasCodepoint(const XmRendition rend, XmCodepoint cp)
{
	struct __XmRenditionRec *r;

	if (!(r = XmSharedPtrGet(rend)))
		return False;

	if (r->fontType != XmFONT_IS_XFT || !r->xftFont)
		return True;

	return !!FcCharSetHasChar(r->xftFont->charset, (FcChar32)cp);
}

/**
 * Set XmRendition properties from a FontConfig pattern
 */
static void set_props_from_pattern(XmRendition rend, const FcPattern *p)
{
	int i;
	double d;
	FcChar8 *s;
	struct __XmRenditionRec *r;

	if (!p || !(r = XmSharedPtrGet(rend)))
		return;

	r->fontSize    = 0;
	r->pixelSize   = 0;
	r->fontSlant   = XmAS_IS;
	r->fontWeight  = XmAS_IS;
	r->fontSpacing = XmAS_IS;
	r->fontFoundry = NULL;
	r->fontFamily  = NULL;
	r->fontStyle   = NULL;

	if (FcPatternGetString(p, FC_FOUNDRY, 0, &s) == FcResultMatch)
		r->fontFoundry = (String)s;
	if (FcPatternGetString(p, FC_FAMILY, 0, &s) == FcResultMatch)
		r->fontFamily = (String)s;
	if (FcPatternGetString(p, FC_STYLE, 0, &s) == FcResultMatch)
		r->fontStyle = (String)s;
	if (FcPatternGetDouble(p, FC_SIZE, 0, &d) == FcResultMatch)
		r->fontSize = (int)d;
	if (FcPatternGetDouble(p, FC_PIXEL_SIZE, 0, &d) == FcResultMatch)
		r->pixelSize = (int)d;
	if (FcPatternGetInteger(p, FC_SLANT, 0, &i) == FcResultMatch)
		r->fontSlant = i;
	if (FcPatternGetInteger(p, FC_WEIGHT, 0, &i) == FcResultMatch)
		r->fontWeight = i;
	if (FcPatternGetInteger(p, FC_SPACING, 0, &i) == FcResultMatch)
		r->fontSpacing = i;
}

/**
 * Ignore X errors, in case of BadAtom
 */
static int ignore_x_errors(Display *disp, XErrorEvent *event)
{
	(void)disp;
	(void)event;
    return 0;
}

/**
 * Set XmRendition properties from XFontProps.
 *
 * Normalize integer props to standard values used by fontconfig for
 * parity with Xft.
 *
 * NB: These values, point size in particular, might not be accurate.
 */
static void set_props_from_fontstruct(XmRendition rend, const XFontStruct *fs)
{
	int i;
	double dpi;
	String slant, weight;
	Atom FOUNDRY, WEIGHT_NAME, SLANT;
	XErrorHandler olderr;
	struct __XmRenditionRec *r;

	if (!fs || !(r = XmSharedPtrGet(rend)))
		return;

	FOUNDRY     = XInternAtom(r->display, "FOUNDRY", False);
	WEIGHT_NAME = XInternAtom(r->display, "WEIGHT_NAME", False);
	SLANT       = XInternAtom(r->display, "SLANT", False);

	/* In case of BadAtom... */
	olderr = XSetErrorHandler(ignore_x_errors);

	for (i = 0; i < fs->n_properties; i++) {
		if (fs->properties[i].name == FOUNDRY) {
			XtFree(r->fontFoundry);
			r->fontFoundry = XGetAtomName(r->display, fs->properties[i].card32);
			continue;
		}

		if (fs->properties[i].name == XA_FAMILY_NAME) {
			XtFree(r->fontFamily);
			r->fontFamily = XGetAtomName(r->display, fs->properties[i].card32);
			continue;
		}

		if (fs->properties[i].name == XA_POINT_SIZE) {
			r->fontSize = fs->properties[i].card32 / 10;
			continue;
		}

		if (fs->properties[i].name == SLANT) {
			slant = XGetAtomName(r->display, fs->properties[i].card32);
			switch (*slant) {
			case 'r': case 'R': r->fontSlant = 0;   break; /* Roman   */
			case 'i': case 'I': r->fontSlant = 100; break; /* Italic  */
			case 'o': case 'O': r->fontSlant = 110; break; /* Oblique */
			default:            r->fontSlant = 0;
			}

			XFree(slant);
			continue;
		}

		if (fs->properties[i].name == WEIGHT_NAME) {
			weight = XGetAtomName(r->display, fs->properties[i].card32);
			switch (*weight) {
			case 'm': case 'M': r->fontWeight = 100; break; /* Medium   */
			case 'd': case 'D': r->fontWeight = 180; break; /* Demibold */
			case 'b': case 'B': r->fontWeight = 200; break; /* Bold     */
			default:            r->fontWeight = 80;         /* Regular  */
			}

			XFree(weight);
			continue;
		}
	}

	dpi = DpiOfXmScreen(XmScreenOfScreen(DefaultScreenOfDisplay(r->display)));
	r->pixelSize = (int)(r->fontSize * (dpi / 72.));
	XSetErrorHandler(olderr);
}

static void set_props_from_font(XmRendition rend)
{
	XFontStruct **f_list;
	XFontSetExtents *e;
	char **names = NULL;
	struct __XmRenditionRec *r;

	if (!(r = XmSharedPtrGet(rend)))
		return;

	if (r->fontType == XmFONT_IS_FONT && r->font) {
		set_props_from_fontstruct(rend, r->font);
		r->width     = ((XFontStruct *)r->font)->max_bounds.rbearing -
		               ((XFontStruct *)r->font)->max_bounds.lbearing;
		r->ink_width = ((XFontStruct *)r->font)->max_bounds.width;
		r->ascent    = ((XFontStruct *)r->font)->ascent;
		r->descent   = ((XFontStruct *)r->font)->descent;
	}

	if (r->fontType == XmFONT_IS_FONTSET && r->font) {
		if (!XFontsOfFontSet((XFontSet)r->font, &f_list, &names))
			return;

		set_props_from_fontstruct(rend, *f_list);
		e = XExtentsOfFontSet((XFontSet)r->font);
		r->width     = e->max_logical_extent.width;
		r->ink_width = e->max_ink_extent.width;
		r->ascent    = -e->max_logical_extent.y;
		r->descent   = e->max_logical_extent.height + e->max_logical_extent.y;
	}

	if (r->fontType == XmFONT_IS_XFT && r->xftFont) {
		set_props_from_pattern(rend, r->xftFont->pattern);
		r->width     = r->xftFont->max_advance_width;
		r->ink_width = r->width;
		r->ascent    = r->xftFont->ascent;
		r->descent   = r->xftFont->descent;
	}
}

/**
 * Load an X font, trying to load it as a FontSet first, falling back
 * to XFontStruct if it fails.
 */
static Boolean load_xfont(XmRendition rend)
{
	int mcnt;
	char **mcset = NULL, *def_str;
	struct __XmRenditionRec *r;

	if (!(r = XmSharedPtrGet(rend)) || !r->pattern)
		return False;

	if (r->fontType == XmFONT_IS_FONTSET &&
	    (r->font = XCreateFontSet(r->display, r->pattern, &mcset, &mcnt, &def_str)))
		if (mcset) XFreeStringList(mcset);

	if (r->fontType == XmFONT_IS_FONT || !r->font) {
		if ((r->font = XLoadQueryFont(r->display, r->pattern)))
			r->fontType = XmFONT_IS_FONT;
	}

	if (r->font) set_props_from_font(rend);
	return !!r->font;
}

static Boolean load_xft(XmRendition rend)
{
	FcPattern *p, *p2;
	FcResult res;
	struct __XmRenditionRec *r;

	if (!(r = XmSharedPtrGet(rend)) || !r->pattern)
		return False;

	/* r->pattern is either a pattern, or family name */
	if (strchr(r->pattern, '-') || strchr(r->pattern, '='))
		p = FcNameParse((FcChar8 *)r->pattern);
	else {
		p = FcPatternCreate();
		FcPatternAddString(p, FC_FAMILY, (FcChar8 *)r->pattern);
	}

	if (r->fontFoundry) FcPatternAddString(p,  FC_FOUNDRY,    (FcChar8 *)r->fontFoundry);
	if (r->fontStyle)   FcPatternAddString(p,  FC_STYLE,      (FcChar8 *)r->fontStyle);
	if (r->fontSize)    FcPatternAddDouble(p,  FC_SIZE,       (double)r->fontSize);
	if (r->pixelSize)   FcPatternAddDouble(p,  FC_PIXEL_SIZE, (double)r->pixelSize);
	if (r->fontSlant   != XmAS_IS) FcPatternAddInteger(p, FC_SLANT,   r->fontSlant);
	if (r->fontWeight  != XmAS_IS) FcPatternAddInteger(p, FC_WEIGHT,  r->fontWeight);
	if (r->fontSpacing != XmAS_IS) FcPatternAddInteger(p, FC_SPACING, r->fontSpacing);
	FcPatternAddDouble(p, FC_DPI, DpiOfXmScreen(XmScreenOfScreen(DefaultScreenOfDisplay(r->display))));

	p2 = XftFontMatch(r->display, 0, p, &res);
	if (!(r->xftFont = XftFontOpenPattern(r->display, p2)))
		FcPatternDestroy(p2);
	else {
		XtFree(r->fontStyle);
		r->fontStyle = NULL;
	}
	FcPatternDestroy(p);

	set_props_from_font(rend);
	return !!r->xftFont;
}

/**
 * Load the font specified by the rendition's font type and pattern.
 *
 * If do_callback is True, the "NO_FONT" callback will be called
 * in the event that the font fails to load.
 *
 * Returns True if the font was successfully loaded, False otherwise.
 */
Boolean XmRenditionLoad(XmRendition rend, Boolean do_callback)
{
	Boolean loaded = False;
	XmDisplay d;
	XmDisplayCallbackStruct cb;
	struct __XmRenditionRec *r;

	if (!(r = XmSharedPtrGet(rend)))
		return False;

	/* User-supplied fonts are presumed loaded */
	if (r->loadModel == XmUNSPECIFIED_LOAD_MODEL)
		return True;

	if (!r->display) {
		RenditionWarning(r->tag, "NULL_DISPLAY", NULL_DISPLAY_MSG, NULL);
		return False;
	}

	if ((!r->pattern || !*r->pattern)) {
		if (r->loadModel == XmLOAD_IMMEDIATE) {
			RenditionWarning(r->tag, "NULL_LOAD_IMMEDIATE",
			                 NULL_LOAD_IMMEDIATE_MSG, r->display);
		}

		return False;
	}

	switch (r->fontType) {
	case XmFONT_IS_FONT:
	case XmFONT_IS_FONTSET:
		if (r->font) {
			set_props_from_font(rend);
			return True;
		}

		loaded = load_xfont(rend);
		break;
	case XmFONT_IS_XFT:
		if (r->xftFont) {
			set_props_from_font(rend);
			return True;
		}

		loaded = load_xft(rend);
		break;
	default:
		RenditionWarning(r->tag, "INVALID_TYPE", INVALID_TYPE_MSG, r->display);
	}

	/* Call the NO_FONT callback to supply a font if requested */
	if (!loaded && do_callback) {
		d = (XmDisplay)XmGetXmDisplay(r->display);
		if (d && d->display.noFontCallback) {
			memset(&cb, 0, sizeof cb);
			cb.reason    = XmCR_NO_FONT;
			cb.rendition = rend;
			cb.font_name = r->pattern;
			XtCallCallbackList((Widget)d, d->display.noFontCallback, &cb);
			loaded = r->font || r->xftFont;
		}
	}

	return loaded;
}

/**
 * Unload a font, resetting the font properties
 */
void XmRenditionUnload(XmRendition rend)
{
	struct __XmRenditionRec *r;

	if (!(r = XmSharedPtrGet(rend)) || r->loadModel == XmUNSPECIFIED_LOAD_MODEL)
		return;

	if (r->fontType == XmFONT_IS_XFT) {
		if (!r->xftFont)
			goto out;

		XftFontClose(r->display, r->xftFont);
		r->xftFont = NULL;
		goto out;
	}

	if (!r->font)
		goto out;

	if (r->fontType == XmFONT_IS_FONTSET)
		XFreeFontSet(r->display, (XFontSet)r->font);
	else XFreeFont(r->display, r->font);
	r->font = NULL;
	XtFree(r->fontFoundry);
	XtFree(r->fontFamily);

out:
	r->fontFoundry = NULL;
	r->fontFamily  = NULL;
	r->fontStyle   = NULL;
	r->fontSize    = 0;
	r->pixelSize   = 0;
	r->fontSlant   = XmAS_IS;
	r->fontWeight  = XmAS_IS;
	r->fontSpacing = XmAS_IS;
}

/**
 * Determine if a rendition has already been loaded
 */
Boolean XmRenditionIsLoaded(XmRendition rendition)
{
	struct __XmRenditionRec *r;

	return (r = XmSharedPtrGet(rendition)) && (r->font || r->xftFont);
}

/**
 * Find a fallback rendition for a given codepoint
 *
 * \param tbl  Render table to search
 * \param orig Rendition in which we encountered the missing codepoint
 * \param cp   The missing codepoint
 * \return A new XmRendition or NULL if a suitable rendition could
 *         not be found, or invalid parameters were given.
 *
 * This only applies for Xft fonts.
 *
 * Search the render table for a stylistically-compatible font that
 * contains the given codepoint. If one cannot be found, progressively
 * decompose the original font's pattern and attempt to match a font.
 * If we manage to match something, add it to the RenderTable, otherwise
 * return NULL.
 */
XmRendition XmRenditionFallbackForCodepoint(XmRenderTable tbl,
                                            const XmRendition orig,
                                            XmCodepoint cp)
{
	Cardinal i;
	Display *d;
	struct __XmRenderTableRec *rt;
	struct __XmRenditionRec *r;
	const struct __XmRenditionRec *orig_r;
	XmRendition rend = NULL;
	XtAppContext app;
	FcPattern *p = NULL, *p2 = NULL;
	FcCharSet *cs;
	FcObjectSet *os;
	FcResult res;
	FcValue val;
	static FcCharSet *rej = NULL;
	char buf[16];

	if (!(rt = XmSharedPtrGet(tbl)))
		return NULL;

	/* This only applies to Xft fonts */
	orig_r = XmSharedPtrGet(orig);
	if (!rt->display || !orig_r || orig_r->fontType != XmFONT_IS_XFT)
		return NULL;

	d   = rt->display;
	app = _XmLock(d);
	if (!rej) rej = FcCharSetCreate();

	/* Make sure it's not a reject */
	if (FcCharSetHasChar(rej, (FcChar32)cp) || !(cs = FcCharSetCreate()))
		goto unlock;
	FcCharSetAddChar(cs, (FcChar32)cp);

	/* Probe the render table for a compatible font */
	os = FcObjectSetCreate();
	FcObjectSetAdd(os, FC_FOUNDRY);
	FcObjectSetAdd(os, FC_FAMILY);
	FcObjectSetAdd(os, FC_STYLE);
	FcObjectSetAdd(os, FC_WEIGHT);
	FcObjectSetAdd(os, FC_SLANT);
	FcObjectSetAdd(os, FC_SIZE);
	FcObjectSetAdd(os, FC_PIXEL_SIZE);

	for (i = 0; i < rt->count; i++) {
		rend = rt->renditions[i];
		r    = XmSharedPtrGet(rend);
		if (rend == orig || r == orig_r || r->fontType != XmFONT_IS_XFT || !r->xftFont)
			continue;

		if (!FcCharSetHasChar(r->xftFont->charset, (FcChar32)cp))
			continue;

		/* We have the codepoint, see if we have a style fit */
		if (!orig_r->xftFont) {
			FcCharSetDestroy(cs);
			FcObjectSetDestroy(os);
			goto unlock;
		}

		p  = orig_r->xftFont->pattern;
		p2 = r->xftFont->pattern;
		if (FcPatternEqualSubset(p, p2, os)) {
			FcCharSetDestroy(cs);
			FcObjectSetDestroy(os);
			goto unlock;
		}
	}

	/* Try the same font pattern with the codepoint */
	p2 = FcPatternDuplicate(orig_r->xftFont->pattern);
	FcObjectSetAdd(os, FC_FOUNDRY);
	FcObjectSetAdd(os, FC_FAMILY);
	p = FcPatternFilter(p2, os);
	FcPatternDestroy(p2);
	FcObjectSetDestroy(os);

	FcPatternAddCharSet(p, FC_CHARSET, cs);
	p2 = XftFontMatch(d, 0, p, &res);
	if (res == FcResultMatch) goto found;
	FcPatternDestroy(p2);

	/* Try any foundry */
	if (FcPatternGet(p, FC_FOUNDRY, 0, &val) == FcResultMatch) {
		FcPatternDel(p, FC_FOUNDRY);
		p2 = XftFontMatch(d, 0, p, &res);
		if (res == FcResultMatch) goto found;
		FcPatternDestroy(p2);
	}

	/* Try any family */
	if (FcPatternGet(p, FC_FAMILY, 0, &val) == FcResultMatch) {
		FcPatternDel(p, FC_FAMILY);
		p2 = XftFontMatch(d, 0, p, &res);
		if (res == FcResultMatch) goto found;
		FcPatternDestroy(p2);
	}

	/* Try any style */
	if (FcPatternGet(p, FC_STYLE, 0, &val) == FcResultMatch) {
		FcPatternDel(p, FC_STYLE);
		p2 = XftFontMatch(d, 0, p, &res);
		if (res == FcResultMatch) goto found;
		FcPatternDestroy(p2);
	}

	/**
	 * We couldn't match at all.
	 * Let's not try again.
	 */
	FcCharSetAddChar(rej, (FcChar32)cp);
	rend = NULL;

done:
	FcPatternDestroy(p);
	FcCharSetDestroy(cs);

unlock:
	_XmUnlock(app);
	return XmSharedPtrCopy(rend, False);

found:
	rend = XmRenditionCreate(NULL, XmS, NULL, 0);
	r    = XmSharedPtrGet(rend);
	if (!(r->xftFont = XftFontOpenPattern(d, p2))) {
		FcPatternDestroy(p2);
		XmRenditionFree(rend);
		rend = NULL;
		goto done;
	}

	r->fontType = XmFONT_IS_XFT;
	r->display  = orig_r->display;
	set_props_from_pattern(rend, r->xftFont->pattern);

	/**
	 * Fontconfig thinks this should match, but in reality, it doesn't.
	 * This could be a bad font, fontconfig misconfiguration, etc.
	 * We need to bail here to avoid getting stuck in a loop between
	 * thinking we have a suitable font, and probing for a fallback.
	 */
	if (!FcCharSetHasChar(r->xftFont->charset, (FcChar32)cp)) {
		XmRenditionFree(rend);
		rend = NULL;
		goto done;
	}

	/* Append it to the render table */
	rt->renditions = (XmRendition *)XtRealloc(
		(XtPointer)rt->renditions,
		(rt->count + 1) * sizeof *rt->renditions
	);

	rt->renditions[rt->count++] = rend;
	goto done;
}

/* Get resource values from rendition. */
void XmRenditionGetValues(XmRendition rendition, ArgList args, Cardinal count)
{
	Cardinal i, j;
	Display *d;
	void *p;
	int mcnt;
	char **mcset = NULL, *def_str;
	XmLoadModel lm;
	struct __XmRenditionRec *orig;
	XtAppContext app;

	if (!rendition || !args || !count)
		return;

	/* Ensure our rendition is still valid */
	d = _XmGetDefaultDisplay();
	if (!(orig = XmSharedPtrGet(rendition))) {
		RenditionWarning(NULL, "INVALID_RENDITION", INVALID_RENDITION, d);
		return;
	}

	if (orig->display) d = orig->display;
	app = _XmLock(d);

	/* Ensure we have a display */
	if (!orig->display)
		orig->display = d;

	for (i = 0; i < count; i++) {
		if (!args[i].value)
			continue;

		for (j = 0; j < _XmNumRenditionResources; j++) {
			if (args[i].name != _XmRenditionResources[j].resource_name &&
			    strcmp(_XmRenditionResources[j].resource_name, args[i].name))
				continue;

			/* Ensure the font is loaded if requested (and we own it) */
			lm = orig->loadModel;
			if (lm != XmUNSPECIFIED_LOAD_MODEL && lm != XmLOAD_LAZY) {
				orig->loadModel = XmLOAD_IMMEDIATE;
				if (!orig->font && _XmRenditionResources[j].resource_type == XmRFontStruct)
					XmRenditionLoad(rendition, True);

				if (!orig->xftFont && _XmRenditionResources[j].resource_type == XmRXftFont)
					XmRenditionLoad(rendition, True);
				orig->loadModel = lm;
			}

			assert(_XmRenditionResources[j].resource_offset +
			       _XmRenditionResources[j].resource_size <= sizeof *orig);

			p = (char *)orig + _XmRenditionResources[j].resource_offset;
			if (_XmRenditionResources[j].resource_type == XtRString) {
				*(String *)args[i].value = XtNewString(*(String *)p);
				break;
			}

			if (_XmRenditionResources[j].resource_type == XmRTabList) {
				*(XmTabList *)args[i].value = XmTabListCopy(*(XmTabList *)p, 0, 0);
				break;
			}

			if (orig->font && _XmRenditionResources[j].resource_type == XmRFontStruct) {
				if (orig->fontType != XmFONT_IS_FONT && orig->fontType != XmFONT_IS_FONTSET) {
					*(XtPointer *)args[i].value = NULL;
					break;

				}
			}

			if (orig->xftFont && _XmRenditionResources[j].resource_type == XmRXftFont) {
				if (orig->fontType != XmFONT_IS_XFT) {
					*(XtPointer *)args[i].value = NULL;
					break;
				}
			}

			memcpy((void *)args[i].value, p,
			       _XmRenditionResources[j].resource_size);
			break;
		}
	}

	_XmUnlock(app);
}

/* Set resources in rendition. */
void XmRenditionSetValues(XmRendition rendition, ArgList args, Cardinal count)
{
	Cardinal i, j;
	Display *d;
	Colormap cmap;
	void *p;
	Boolean has_font = False, user_font = False;
	struct __XmRenditionRec new, *orig;
	XtAppContext app;

	if (!rendition || !args || !count)
		return;

	/* Ensure our rendition is still valid */
	d = _XmGetDefaultDisplay();
	if (!(orig = XmSharedPtrGet(rendition))) {
		RenditionWarning(NULL, "INVALID_RENDITION", INVALID_RENDITION, d);
		return;
	}

	if (orig->display) d = orig->display;
	app = _XmLock(d);

	/* Ensure we have a display */
	if (!orig->display)
		orig->display = d;

	/* Process args on a scratch instance */
	memcpy(&new, orig, sizeof new);
	for (i = 0; i < count; i++) {
		for (j = 0; j < _XmNumRenditionResources; j++) {
			if (args[i].name != _XmRenditionResources[j].resource_name &&
			    strcmp(_XmRenditionResources[j].resource_name, args[i].name))
				continue;

			p = (char *)&new + _XmRenditionResources[j].resource_offset;
			assert(_XmRenditionResources[j].resource_offset +
			       _XmRenditionResources[j].resource_size <= sizeof new);

			if (_XmRenditionResources[j].resource_type == XtRString) {
				*(String *)p = XtNewString((String)args[i].value);
				break;
			}

			if (_XmRenditionResources[j].resource_type == XmRTabList) {
				*(XmTabList *)p = XmTabListCopy((XmTabList)args[i].value, 0, 0);
				break;
			}

			memcpy(p, &args[i].value, _XmRenditionResources[j].resource_size);
			break;
		}
	}

	/* Now, check for changes between orig and new */
	if (orig->tabs != new.tabs) {
		XmTabListFree(orig->tabs);
		orig->tabs = new.tabs;
	}

	/**
	 * Check for a caller-supplied font.
	 *
	 * We don't take ownership of caller-supplied fonts, and presume
	 * validity if the supplied font is non-NULL.
	 */
	user_font  = new.font && orig->font != new.font;
	user_font |= new.xftFont && orig->xftFont != new.xftFont;

	/**
	 * (Re-)load the font if related properties change.
	 */
	if (orig->loadModel != new.loadModel || user_font ||
	    (orig->pattern && new.pattern && strcmp(orig->pattern, new.pattern)) ||
	    (!orig->pattern && new.pattern)  ||
	    orig->fontType != new.fontType) {
		has_font = !!orig->font || !!orig->xftFont;

		if (has_font && orig->loadModel != XmUNSPECIFIED_LOAD_MODEL)
			XmRenditionUnload(rendition);

		if ((orig->pattern && new.pattern && strcmp(orig->pattern, new.pattern)) ||
		    (!orig->pattern && new.pattern)) {
			XtFree(orig->pattern);
			orig->pattern = NULL;

			if (new.pattern) {
				orig->pattern = new.pattern;
				new.pattern = NULL;
			}
		}

		orig->loadModel = new.loadModel;
		orig->fontType  = new.fontType;
		if (user_font) {
			orig->loadModel = XmUNSPECIFIED_LOAD_MODEL;
			if (new.fontType == XmFONT_IS_FONT || new.fontType == XmFONT_IS_FONTSET) {
				if ((orig->font = new.font))
					set_props_from_font(rendition);
			} else if ((orig->xftFont = new.xftFont))
				set_props_from_font(rendition);
		} else if (orig->pattern && orig->loadModel != XmLOAD_LAZY)
			XmRenditionLoad(rendition, True);
	}

	orig->style.underline  = new.style.underline;
	orig->style.strikethru = new.style.strikethru;
	orig->style.bg_state   = new.style.bg_state;
	orig->style.fg_state   = new.style.fg_state;

	/* Free colors we allocated them and they get reset */
	cmap = DefaultColormap(d, DefaultScreen(d));
	if (orig->style.fg.pixel != new.style.fg.pixel) {
		if (orig->free_fg)
			XFreeColors(d, cmap, &orig->style.fg.pixel, 1, 0);
		orig->free_fg = False;
		orig->style.fg.pixel = new.style.fg.pixel;
	}

	if (orig->style.bg.pixel != new.style.bg.pixel) {
		if (orig->free_bg)
			XFreeColors(d, cmap, &orig->style.bg.pixel, 1, 0);
		orig->free_bg = False;
		orig->style.bg.pixel = new.style.bg.pixel;
	}

	/* Free copied strings on new */
	XtFree(new.fontFoundry);
	XtFree(new.fontFamily);
	XtFree(new.fontStyle);
	XtFree(new.pattern);
	_XmUnlock(app);
}

void XmRenditionFree(XmRendition rendition)
{
	Display *d;
	struct __XmRenditionRec *orig;
	XtAppContext app;

	if (!rendition)
		return;

	d = _XmGetDefaultDisplay();
	if (!(orig = XmSharedPtrGet(rendition)))
		return;
	if (orig->display) d = orig->display;

	app = _XmLock(d);
	XmSharedPtrFree(rendition);
	_XmUnlock(app);
}

/**
 * Free an array of renditions
 */
void XmFreeRenditionArray(XmRendition *rends, Cardinal count)
{
	Cardinal i;

	for (i = 0; i < count; i++)
		XmRenditionFree(rends[i]);
	XtFree((XtPointer)rends);
}

/**
 * Serialize the render table in TLV (8:8:N) format with the following
 * types. Any field that would exceed 8 bits for length will be discarded.
 * RT_RENDERTABLE will utilize a 32-bit length, while RT_RENDITION will
 * utilize a 16-bit length.
 *
 * The structure is: [ RT_RENDITION, ... ]
 *                   RT_RENDITION   -> [ ... RT_TAG ...  RT_TABEND ]
 */
enum rt_serialized_type {
	RT_RENDITION = 1,
	RT_TAB,           /* end of tab list item marker */
	RT_TAG,           /* string */
	RT_FONTLOADMODEL, /* 8-bit unsigned */
	RT_FONTTYPE,      /* 8-bit unsigned */
	RT_FONTPATTERN,   /* string */
	RT_FONTSTYLE,     /* string */
	RT_FONTSLANT,     /* 8-bit unsigned */
	RT_FONTWEIGHT,    /* 8-bit unsigned */
	RT_BGCOLOR,       /* a, r, g, b */
	RT_FGCOLOR,
	RT_UNDERLINE,     /* 8-bit unsigned */
	RT_STRIKETHRU,    /* 8-bit unsigned */
	RT_TABVALUE,      /* stringified float */
	RT_TABUNITS,      /* 8-bit unsigned */
	RT_TABALIGNMENT,  /* 8-bit unsigned */
	RT_TABOFFSETMODEL /* 8-bit unsigned (1 if XmABSOLUTE, 0 if XmRELATIVE) */
};

static unsigned int rt_add_string(unsigned char **buf, unsigned int buflen,
                                  enum rt_serialized_type tag, char *s)
{
	size_t len = 0;
	unsigned char *b = *buf;

	if (s && (len = strlen(s)) > UCHAR_MAX)
		len = UCHAR_MAX - 1;

	if (!s || !len)
		return buflen;

	b = (unsigned char *)XtRealloc((XtPointer)b, buflen + len + 3);
	b[buflen]     = tag;
	b[buflen + 1] = (len + 1) & 0xff;
	memcpy(b + buflen + 2, s, len + 1);
	*buf = b;
	return buflen + len + 3;
}

static unsigned int rt_add_byte(unsigned char **buf, unsigned int buflen,
                                enum rt_serialized_type tag, unsigned char c)
{
	unsigned char *b = *buf;

	if (c == XmAS_IS)
		return buflen;

	b = (unsigned char *)XtRealloc((XtPointer)b, buflen + 3);
	b[buflen]     = tag;
	b[buflen + 1] = 1;
	b[buflen + 2] = c;
	*buf = b;
	return buflen + 3;
}

static unsigned int rt_add_pixel(unsigned char **buf, unsigned int buflen,
                                enum rt_serialized_type tag, Display *d,
                                Pixel p, unsigned short a)
{
	XColor c;
	Colormap cmap;
	unsigned char *b = *buf;

	if (p == XmUNSPECIFIED_PIXEL)
		return buflen;

	c.pixel = p;
	cmap = DefaultColormap(d, DefaultScreen(d));
	XQueryColor(d, cmap, &c);
	b = (unsigned char *)XtRealloc((XtPointer)b, buflen + 10);
	b[buflen]     = tag;
	b[buflen + 1] = 8;
	b[buflen + 2] = (a >> 8) & 0xff;
	b[buflen + 3] = a & 0xff;
	b[buflen + 4] = (c.red >> 8) & 0xff;
	b[buflen + 5] = c.red & 0xff;
	b[buflen + 6] = (c.green >> 8) & 0xff;
	b[buflen + 7] = c.green & 0xff;
	b[buflen + 8] = (c.blue >> 8) & 0xff;
	b[buflen + 9] = c.blue & 0xff;

	*buf = b;
	return buflen + 10;
}

static unsigned int rt_add_tab(unsigned char **buf, unsigned int buflen, XmTab tab)
{
	char *f;
	unsigned int flen, x = buflen;
	unsigned char *b = *buf;

	flen = snprintf(NULL, 0, "%f", tab->value);
	f    = XtMalloc(flen + 1);
	snprintf(f, flen + 1, "%f", tab->value);

	buflen = rt_add_string(&b, buflen, RT_TABVALUE, f);
	buflen = rt_add_byte(&b, buflen, RT_TABUNITS,     tab->units);
	buflen = rt_add_byte(&b, buflen, RT_TABALIGNMENT, tab->alignment);
	buflen = rt_add_byte(&b, buflen, RT_TABOFFSETMODEL, tab->offsetModel == XmABSOLUTE);
	XtFree(f);

	/* Mark the end of the tab item */
	b = (unsigned char *)XtRealloc((XtPointer)b, buflen + 2);
	b[buflen] = RT_TAB;
	b[buflen + 1] = '\0';
	buflen += 2;
	*buf = b;
	return buflen;
}

/**
 * Pack the given rendertable for transport as _MOTIF_RENDER_TABLE
 */
unsigned int XmRenderTableCvtToProp(Widget widget, XmRenderTable table,
                                    char **prop_return)
{
	Cardinal i, j;
	size_t len;
	unsigned char *buf = NULL;
	unsigned int ret = 0, iret, x;
	XtAppContext app;
	_XmTab tab;
	_XmTabList tlist;
	const struct __XmRenditionRec *r;
	const struct __XmRenderTableRec *rt;

	if (!(rt = XmSharedPtrGet(table)) || !rt->count) {
		if (prop_return) *prop_return = NULL;
		return 0;
	}

	/* Serialize our renditions */
	app = _XmLockWidget(widget);
	for (i = 0; i < rt->count; i++) {
		if (!(r = XmSharedPtrGet(rt->renditions[i])))
			continue;

		buf = (unsigned char *)XtRealloc((XtPointer)buf, ret + 3);
		buf[ret] = RT_RENDITION;
		ret += 3;
		iret = ret;

		ret = rt_add_string(&buf, ret, RT_TAG,           r->tag);
		ret = rt_add_byte(&buf,   ret, RT_FONTTYPE,      r->fontType);
		ret = rt_add_byte(&buf,   ret, RT_FONTLOADMODEL, r->loadModel);
		ret = rt_add_string(&buf, ret, RT_FONTPATTERN,   r->pattern);
		ret = rt_add_string(&buf, ret, RT_FONTSTYLE,     r->fontStyle);
		ret = rt_add_byte(&buf,   ret, RT_FONTSLANT,     r->fontSlant);
		ret = rt_add_byte(&buf,   ret, RT_FONTWEIGHT,    r->fontWeight);
		ret = rt_add_byte(&buf,   ret, RT_UNDERLINE,     r->style.underline);
		ret = rt_add_byte(&buf,   ret, RT_STRIKETHRU,    r->style.strikethru);
		ret = rt_add_pixel(&buf,  ret, RT_FGCOLOR, r->display,
		                   r->style.fg.pixel, r->style.fg.alpha);
		ret = rt_add_pixel(&buf,  ret, RT_BGCOLOR, r->display,
		                   r->style.bg.pixel, r->style.bg.alpha);

		if ((tlist = (_XmTabList)r->tabs)) {
			tab = tlist->start;
			for (j = 0; tab && j < tlist->count; j++) {
				ret = rt_add_tab(&buf, ret, tab);
				tab = tab->next;
			}
		}

		x = ret - iret;
		buf[iret - 1] = (x >> 8) & 0xff;
		buf[iret - 2] = x & 0xff;
	}

	/* Tack on a terminator to ensure the last rendition is added */
	buf = (unsigned char *)XtRealloc((XtPointer)buf, ret + 3);
	buf[ret]     = RT_RENDITION;
	buf[ret + 1] = 0;
	buf[ret + 2] = 0;
	ret += 3;

	/* Finalize the length */
	x = ret - 3;
	buf[1] = (x >> 8) & 0xff;
	buf[2] = x & 0xff;

	if (prop_return) *prop_return = (char *)buf;
	else XtFree((XtPointer)buf);
	_XmUnlock(app);
	return ret;
}

static Boolean next_tag(unsigned char **buf, unsigned int *len,
                        enum rt_serialized_type *tag, unsigned char **value,
                        unsigned int *clen)
{
	unsigned char *b = *buf;
	unsigned int l;

	if (!buf || !*buf || !len || !*len || !value || !clen)
		return False;

	switch ((*tag = *b)) {
	case RT_RENDITION:
		if (*len < 3)
			goto bad_length;

		l      = (b[1] << 16) | b[2];
		*value = *buf = b + 3;
		*clen  = (unsigned int)l;

		if (*len < l + 3)
			goto bad_length;
		*len -= 3;
		return True;
	case RT_TAB:
		if (*len < 2)
			goto bad_length;
		*value = b + 2;
		*clen  = 0;
		*len  -= 2;
		break;
	default:
		if (*len < 3)
			goto bad_length;
		*clen  = b[1];
		*value = b + 2;
		l      = 2 + b[1];

		if (*len < l)
			goto bad_length;
		*len -= l;
		break;
	}

	*buf = *value + *clen;
	return True;

bad_length:
	*value = NULL;
	*clen  = 0;
	return False;
}

/**
 * Unpack a serialized rendertable, stopping immediately if we notice
 * invalid input.
 */
XmRenderTable XmRenderTableCvtFromProp(Widget w, char *prop, unsigned int len)
{
	Arg arg[11];
	Cardinal n = 0;
	Display *d;
	Colormap cmap;
	XColor c;
	Boolean free_fg = False, free_bg = False;
	XtAppContext app;
	XmRenderTable rt = NULL;
	XmRendition rend = NULL;
	XmStringTag tag  = NULL;
	XmTab tab        = NULL;
	XmTabList tabs   = NULL;
	XmOffsetModel t_model = XmRELATIVE;
	float t_value = .0f;
	unsigned char t_units = XmINCHES, t_align = XmALIGNMENT_BEGINNING;
	unsigned int clen;
	unsigned short fg_alpha = 0xffff, bg_alpha = 0xffff;
	unsigned char *buf = (unsigned char *)prop, *value;
	enum rt_serialized_type type;
	struct __XmRenditionRec *r;

	if (!buf || *buf != RT_RENDITION || !len)
		return NULL;

	if (!w) d = _XmGetDefaultDisplay();
	else d = XtDisplay(w);
	app = _XmLockWidget(w);

	cmap = DefaultColormap(d, DefaultScreen(d));
	rt   = XmRenderTableCreate(w);
	if (!XmSharedPtrGet(rt))
		goto done;

	while (len && n < XtNumber(arg) && next_tag(&buf, &len, &type, &value, &clen)) {
		if (type != RT_TAG && type != RT_RENDITION && type != RT_TAB && (!value || !clen))
			break;

		switch (type) {
		case RT_RENDITION:
			if (n) {
				if (tabs) {
					XtSetArg(arg[n], XmNtabList, tabs);
					n++;
				}

				/* Add the current rendition and accumulate params */
				rend = XmRenditionCreate(w, tag, arg, n);
				if (!(r = XmSharedPtrGet(rend)))
					goto done;

				r->style.fg.alpha = fg_alpha;
				r->style.bg.alpha = bg_alpha;
				r->free_fg        = free_fg;
				r->free_bg        = free_bg;
				rt = XmRenderTableAddRenditions(rt, &rend, 1, XmMERGE_REPLACE);
				XmRenditionFree(rend);
				rend = NULL;
			}

			/* Reset our state */
			XmTabListFree(tabs);
			n        = 0;
			tag      = NULL;
			tabs     = NULL;
			free_fg  = False;
			free_bg  = False;
			fg_alpha = 0xffff;
			bg_alpha = 0xffff;
			t_value  = 0.f;
			t_units  = XmINCHES;
			t_model  = XmRELATIVE;
			t_align  = XmALIGNMENT_BEGINNING;
			break;
		case RT_TAB:
			tab  = XmTabCreate(t_value, t_units, t_model, t_align, NULL);
			tabs = XmTabListInsertTabs(tabs, &tab, 1, INT_MAX);
			break;
		case RT_TAG:
			tag = (XmStringTag)value;
			value[clen - 1] = '\0';
			break;
		case RT_FONTPATTERN:
			value[clen - 1] = '\0';
			XtSetArg(arg[n], XmNfontName, value); n++;
			break;
		case RT_FONTSTYLE:
			value[clen - 1] = '\0';
			XtSetArg(arg[n], XmNfontStyle, value); n++;
			break;
		case RT_FONTTYPE:
			XtSetArg(arg[n], XmNfontType, *value); n++;
			break;
		case RT_FONTLOADMODEL:
			XtSetArg(arg[n], XmNloadModel, *value); n++;
			break;
		case RT_FONTSLANT:
			XtSetArg(arg[n], XmNfontSlant, *value); n++;
			break;
		case RT_FONTWEIGHT:
			XtSetArg(arg[n], XmNfontWeight, *value); n++;
			break;
		case RT_UNDERLINE:
			XtSetArg(arg[n], XmNunderlineType, *value); n++;
			break;
		case RT_STRIKETHRU:
			XtSetArg(arg[n], XmNstrikethruType, *value); n++;
			break;
		case RT_FGCOLOR:
		case RT_BGCOLOR:
			if (clen < 8)
				goto done;

			if (type == RT_FGCOLOR)
				fg_alpha = (value[0] << 8) | value[1];
			else bg_alpha = (value[0] << 8) | value[1];
			c.red   = (value[2] << 8) | value[3];
			c.green = (value[4] << 8) | value[5];
			c.blue  = (value[6] << 8) | value[7];

			if (XAllocColor(d, cmap, &c)) {
				if (type == RT_FGCOLOR) {
					free_fg = True;
					XtSetArg(arg[n], XmNrenditionForeground, c.pixel);
					n++;
				}

				if (type == RT_BGCOLOR) {
					free_bg = True;
					XtSetArg(arg[n], XmNrenditionBackground, c.pixel);
					n++;
				}
			}
			break;
		case RT_TABVALUE:
			value[clen - 1] = '\0';
			if (sscanf((char *)value, "%f", &t_value) != 1)
				t_value = .0f;
			break;
		case RT_TABUNITS:
			t_units = value[0];
			break;
		case RT_TABALIGNMENT:
			t_align = value[0];
			break;
		case RT_TABOFFSETMODEL:
			t_model = value[0] ? XmABSOLUTE : XmRELATIVE;
			break;
		default:
			goto done;
		}
	}

done:
	XmRenditionFree(rend);
	XmTabListFree(tabs);
	_XmUnlock(app);
	return rt;
}

static struct _XmXftDrawCacheStruct {
	Display	*display;
	Window	window;
	XftDraw	*draw;
} *_XmXftDrawCache = NULL;
static int _XmXftDrawCacheSize = 0;

static XErrorHandler           oldErrorHandler;
static int xft_error;

static int
_XmXftErrorHandler(
        Display *display,
        XErrorEvent *error )
{
   (void) fprintf(stderr,
   "Ignoring Xlib error: error code %d request code %d\n",
   error->error_code,
   error->request_code) ;
   xft_error = BadWindow;

    /* No exit! - but keep lint happy */

    return 0 ;
}

XftDraw *
_XmXftDrawCreate(Display *display, Window window)
{
	XftDraw			*draw;
	XWindowAttributes	wa;
	int			i;
	Status status;

	for (i=0; i<_XmXftDrawCacheSize; i++) {
		if (_XmXftDrawCache[i].display == display &&
		    _XmXftDrawCache[i].window == window) {
			return _XmXftDrawCache[i].draw;
		}
	}

	if (!(draw = XftDrawCreate(display, window,
	    DefaultVisual(display, DefaultScreen(display)),
	    DefaultColormap(display, DefaultScreen(display)))))
            	draw = XftDrawCreateBitmap(display, window);
	/* Store it in the cache. Look for an empty slot first */
	for (i=0; i<_XmXftDrawCacheSize; i++)
		if (_XmXftDrawCache[i].display == NULL) {
			_XmXftDrawCache[i].display = display;
			_XmXftDrawCache[i].draw = draw;
			_XmXftDrawCache[i].window = window;
			return draw;
		}
	i = _XmXftDrawCacheSize;	/* Next free index */
	_XmXftDrawCacheSize = _XmXftDrawCacheSize * 2 + 8;
	_XmXftDrawCache = (struct _XmXftDrawCacheStruct *)
		XtRealloc((char *)_XmXftDrawCache,
		sizeof(struct _XmXftDrawCacheStruct) * _XmXftDrawCacheSize);
	memset(_XmXftDrawCache + i, 0, (_XmXftDrawCacheSize - i) * sizeof(*_XmXftDrawCache));

	_XmXftDrawCache[i].display = display;
	_XmXftDrawCache[i].draw = draw;
	_XmXftDrawCache[i].window = window;

	return draw;
}

void
_XmXftDrawDestroy(Display *display, Window window, XftDraw *draw)
{
    int i;

    for (i=0; i<_XmXftDrawCacheSize; i++)
	if (_XmXftDrawCache[i].display == display &&
	    _XmXftDrawCache[i].window == window) {
	        _XmXftDrawCache[i].display = NULL;
	        _XmXftDrawCache[i].draw = NULL;
	        _XmXftDrawCache[i].window = None;
	        XftDrawDestroy(draw);
	        return;
        }
    XmeWarning(NULL, "_XmXftDrawDestroy() this should not happen\n");
}

void
_XmXftDrawString2(Display *display, Window window, GC gc, XftFont *font, int bpc,
                Position x, Position y,
                char *s, int len)
{
    XftDraw	*draw = _XmXftDrawCreate(display, window);
    XGCValues gc_val;
    XftColor xftcol;

    XGetGCValues(display, gc, GCForeground, &gc_val);
    xftcol = GetCachedXftColor(display, gc_val.foreground);

    switch (bpc)
    {
	case -1:
		XftDrawString8(draw, &xftcol, font,
			x, y, (XftChar8 *)s, len);
		break;
	case 1:
		XftDrawStringUtf8(draw, &xftcol, font,
			x, y, (XftChar8 *)s, len);
		break;
	case 2:
		XftDrawString16(draw, &xftcol, font,
			x, y, (XftChar16 *)s, len);
		break;
	case 4:
		XftDrawString32(draw, &xftcol, font,
			x, y, (XftChar32 *)s, len);
		break;
	default:
		XmeWarning(NULL, "_XmXftDrawString(unsupported bpc)\n");
    }
}

void _XmXftDrawString(Display *display, Window window, XmRendition rend,
                      XmRenditionStyle style, int bpc, Position x,
                      Position y, char *s, int len, Boolean image)
{
    XGCValues gc_val;
    XGlyphInfo ext;
    XftDraw	*draw;
    XftColor fg_color, bg_color;
    struct __XmRenditionRec *r;

	if (!(r = XmSharedPtrGet(rend)))
		return;

	/* Fill in the queried RGB values for our colors */
	draw = _XmXftDrawCreate(display, window);
	if (style->bg.pixel == XmUNSPECIFIED_PIXEL) {
		XGetGCValues(display, style->gc, GCBackground, &gc_val);
		bg_color = GetCachedXftColor(display, gc_val.background);
	} else bg_color = GetCachedXftColor(display, style->bg.pixel);

	if (style->fg.pixel == XmUNSPECIFIED_PIXEL) {
		XGetGCValues(display, style->gc, GCForeground, &gc_val);
		fg_color = GetCachedXftColor(display, gc_val.foreground);
	} else fg_color = GetCachedXftColor(display, style->fg.pixel);

    if (image)
    {
	ext.xOff = 0;

	switch (bpc)
	{
	    case -1:
	        XftTextExtents8(display, r->xftFont, (FcChar8 *)s, len, &ext);
		break;
	    case 1:
	        XftTextExtentsUtf8(display, r->xftFont, (FcChar8*)s, len, &ext);
		break;
	    case 2:
	        XftTextExtents16(display, r->xftFont, (FcChar16*)s, len, &ext);
		break;
	    case 4:
	        XftTextExtents32(display, r->xftFont, (FcChar32*)s, len, &ext);
		break;
	}

        XftDrawRect(draw, &bg_color, x, y - r->xftFont->ascent,
	            ext.xOff,
		    r->xftFont->ascent +
		    r->xftFont->descent);
    }

    switch (bpc)
    {
	case -1:
		XftDrawString8(draw, &fg_color, r->xftFont, x, y, (XftChar8 *)s, len);
		break;
	case 1:
		XftDrawStringUtf8(draw, &fg_color, r->xftFont, x, y, (XftChar8 *)s, len);
		break;
	case 2:
		XftDrawString16(draw, &fg_color, r->xftFont, x, y, (XftChar16 *)s, len);
		break;
	case 4:
		XftDrawString32(draw, &fg_color, r->xftFont, x, y, (XftChar32 *)s, len);
		break;
	default:
		XmeWarning(NULL, "_XmXftDrawString(unsupported bpc)\n");
    }
}

void
_XmXftSetClipRectangles(Display *display, Window window, Position x, Position y, XRectangle *rects, int n)
{
	XftDraw	*d = _XmXftDrawCreate(display, window);

	XftDrawSetClipRectangles(d, x, y, rects, n);
}

static XftColor
GetCachedXftColor(Display *display, Pixel color)
{
  static XftColor *color_cache = NULL;
  static int colors_count = 0;

  XftColor xftcol = {0, {0,0,0,0xFFFF}};
  XColor xcol;
  Boolean color_exist = FALSE;
  int i;

  if (color_cache != NULL)
  {
    for (i = 0; i < colors_count; ++i)
    {
      if (color_cache[i].pixel == color)
      {
        xftcol = color_cache[i];
        color_exist = TRUE;
        break;
      }
    }
  }

  if (!color_exist)
  {
    xcol.pixel = color;
    XQueryColor(display, DefaultColormap(display,
      DefaultScreen(display)), &xcol);
    xftcol.pixel = color;
    xftcol.color.red = xcol.red;
    xftcol.color.blue = xcol.blue;
    xftcol.color.green = xcol.green;
    xftcol.color.alpha = 0xFFFF;

    color_cache = (XftColor *) XtRealloc((char *) color_cache,
      (Cardinal) (sizeof(XftColor) * (colors_count + 1)));
    if (color_cache != NULL)
      color_cache[colors_count++] = xftcol;
  }

  return xftcol;
}

XftColor
_XmXftGetXftColor(Display *display, Pixel color)
{
    return GetCachedXftColor(display, color);
}

void _XmXftFontAverageWidth(Widget w, XtPointer f, int *width)
{
	XftFont *fp = (XftFont *)f;
	static char	*s = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	int l = 62; /* strlen(s) */
	XGlyphInfo	ext;

	XftTextExtents8(XtDisplay(w), fp, (unsigned char *)s, l, &ext);
    if (width)
    	*width = ext.width / l;
}

