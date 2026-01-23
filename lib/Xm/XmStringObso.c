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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$XConsortium: XmStringObso.c /main/6 1995/09/19 23:13:52 cde-sun $"
#endif
#endif

#include <Xm/XmosP.h>
#include "XmStringI.h"
#include "XmI.h"
#include "XmRenderTI.h"

/* Since this stuff is obsolete, don't worry about deprecation */
#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#elif defined(__clang__)
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif

extern Boolean _XmStringGetNextSegment(
                        _XmStringContext context,
                        XmStringCharSet *charset,
                        XmStringDirection *direction,
                        char **text,
                        short *char_count,
                        Boolean *separator) ;
extern void _XmStringDrawMnemonic(
                        Display *d,
                        Window w,
                        XmFontList fontlist,
                        _XmString string,
                        GC gc,
                        Position x,
                        Position y,
                        Dimension width,
                        unsigned char align,
                        unsigned char lay_dir,
                        XRectangle *clip,
                        String mnemonic,
                        XmStringCharSet charset) ;


/* Create a new XmString */
XmString
_XmStringNCreate(char *text,
		XmStringTag tag,
		int len)
{
  XmString		str;
  char     		*curtag = NULL;
  int      		t_length;
  unsigned int 		tag_index = 0;
  _XmString		opt_str;
  XmTextType            type = XmCHARSET_TEXT;

  if (!text) return ((XmString) NULL);
  if (!tag) return ((XmString) NULL);

  t_length = ((len >= 0) ? len : strlen (text));

  if ((tag == XmFONTLIST_DEFAULT_TAG) ||
      (strcmp(tag, XmFONTLIST_DEFAULT_TAG) == 0))
    {
      curtag = tag;
      type = XmMULTIBYTE_TEXT;
    }
  else {
    if ((strcmp(tag, XmSTRING_DEFAULT_CHARSET) == 0))
      curtag = XmStringGetCharset();
    else curtag = tag;
  }

  tag_index = _XmStringIndexCacheTag(curtag, XmSTRING_TAG_STRLEN);

  if ((tag_index < TAG_INDEX_UNSET) &&
      (t_length < (1 << BYTE_COUNT_BITS)))
    /* Create optimized string. */
    {
      _XmStrCreate(opt_str, XmSTRING_OPTIMIZED, t_length);
      _XmStrTagIndex(opt_str) = tag_index;
      _XmStrTextType(opt_str) = type;
      memcpy(_XmStrText(opt_str), text, t_length);

      return(opt_str);
    }
  else /* Non-optimized */
    {
      _XmStringUnoptSegRec 	seg;
      _XmStrCreate(str, XmSTRING_MULTIPLE_ENTRY, 0);

      _XmEntryInit((_XmStringEntry)&seg, XmSTRING_ENTRY_UNOPTIMIZED);

      _XmUnoptSegTag(&seg) = _XmStringCacheTag(curtag, XmSTRING_TAG_STRLEN);

      _XmEntryTextTypeSet(&seg, type);
      _XmUnoptSegByteCount(&seg) = t_length;
      _XmEntryTextSet((_XmStringEntry)&seg, text);

      _XmStringSegmentNew(str, 0, (_XmStringEntry)&seg, True);
      return(str);
    }
}

/*
 * as close as we can come to Latin1Create without knowing the charset of
 * Latin1.  This imposes the semantic of \n meaning separator.
 */
XmString
XmStringLtoRCreate(
        char *text,
        XmStringTag tag )
{
  char *start, *end;
  Boolean done;
  XmString string;

  _XmProcessLock();
  if (!text) {
	_XmProcessUnlock();
	return (NULL);
  }

  start = text;
  done = FALSE;

  /* Set the direction once only at the beginning. */
  string = XmStringDirectionCreate(XmSTRING_DIRECTION_L_TO_R);

  while ( ! done)		/* loop thu local copy */
    {				/* looking for \n */
      end = start;

      while ((*end != '\0') && (*end != '\n'))  end++;

      if (*end == '\0')
	done = TRUE;		/* we are at the end */

      /* Don't convert empty string unless it's an initial newline. */
      /* Done so StringHeight has clue to size of empty lines. */
      if ((start != end) || (start == text))
        string = XmStringConcatAndFree (string,
					_XmStringNCreate(start, tag,
							 end - start));

      /* Make a separator if this isn't the last segment. */
      if (!done) {
        string = XmStringConcatAndFree(string, XmStringSeparatorCreate());
	start = ++end;		/* start at next char */
      }
    }

  _XmProcessUnlock();
  return (string);
}

XmString
XmStringCreateLtoR(
        char *text,
        XmStringTag tag )
{
  return (XmStringLtoRCreate (text, tag));
}

/*
 * build an external TCS 'segment', just a high level create
 */
XmString
XmStringSegmentCreate(
        char *text,
        XmStringTag tag,
        XmStringDirection direction,
        Boolean separator )
{
  XmString result;

  result = XmStringConcatAndFree (XmStringDirectionCreate (direction),
				  XmStringCreate (text, tag));

  if (separator)
    result = XmStringConcatAndFree (result, XmStringSeparatorCreate ());

  return result;
}

/*
 * Convenience routine to create an XmString from a NULL terminated string.
 */
XmString
XmStringCreateSimple(
        char *text )
{
  return (XmStringCreate(text, XmSTRING_DEFAULT_CHARSET));
}
/*
 * concat two external strings.  Only concat a component at a time
 * so that we always wind up with a meaningful string
 *
 * NB: This is used in Netscape 4, with has a very interesting bug
 * report in cmd/xfe/src/BookmarkBase.cpp that should no longer be
 * relevant. Seems it was a result of XmStringNCopy's behavior,
 * as it was previously used to limit the size of the combined
 * stream (when serialized.)
 */
XmString XmStringNConcat(XmString first, XmString second, int n)
{
	(void)n;
	return XmStringConcat(first, second);
}

/*
 * Copy a compound string, such that the equivalent ASN.1 form
 * has <= n bytes.  Only copy a component at a time
 * so that we always wind up with a meaningful string
 *
 * XXX: I can't find any use of this function anywhere...
 */
XmString XmStringNCopy(XmString str, int n)
{
	(void)n;
	return XmStringCopy(str);
}

/* Compare ASN.1 form of strings. */
Boolean
XmStringByteCompare(
        XmString a1,
        XmString b1 )
{
    unsigned char  *a;
    unsigned char  *b;
    unsigned short a_length, b_length;
    Boolean	   ret_val;

    _XmProcessLock();
    if ((a1 == NULL) && (b1 == NULL)) {
	_XmProcessUnlock();
	return (TRUE);
    }
    if ((a1 == NULL) || (b1 == NULL)) {
	_XmProcessUnlock();
	return (FALSE);
    }

    a_length = (unsigned int)XmStringSerialize(a1, &a);
    b_length = (unsigned int)XmStringSerialize(b1, &b);

    if ((a_length != b_length) || (memcmp(a, b, a_length) != 0))
      ret_val = FALSE;
    else ret_val = TRUE;

    XtFree((char *)a);
    XtFree((char *)b);

    _XmProcessUnlock();
    return(ret_val);
}

XmStringComponentType
XmStringGetNextComponent(
        XmStringContext context,
        char **text,
        XmStringTag *str_tag,
        XmStringDirection *direction,
        XmStringComponentType *unknown_tag,
        unsigned short *unknown_length,
        unsigned char **unknown_value )
{
  XmStringComponentType type;
  unsigned int  	len;
  XtPointer		val;

  _XmProcessLock();
  type = XmeStringGetComponent((_XmStringContext) context, True, True, &len, &val);

  /* Case on return type */
  /* Set appropriate return value and return. */
  switch (type)
    {
    case XmSTRING_COMPONENT_DIRECTION:
      *direction = *(XmStringDirection *)val;
      XtFree((char *)val);
      break;
    case XmSTRING_COMPONENT_TAG:
      *str_tag = (XmStringTag)val;
      break;
    case XmSTRING_COMPONENT_TEXT:
    case XmSTRING_COMPONENT_LOCALE_TEXT:
      *text = (char *)val;
      break;
    case XmSTRING_COMPONENT_SEPARATOR:
    case XmSTRING_COMPONENT_END:
      break;
    default:
      *unknown_tag = type;
      *unknown_length = len;
      *unknown_value = (unsigned char *)val;
      type = XmSTRING_COMPONENT_UNKNOWN;
    }
  _XmProcessUnlock();
  return(type);
}

XmStringComponentType
XmStringPeekNextComponent(XmStringContext context)
{
  unsigned int len;
  XtPointer    val;

  return XmeStringGetComponent((_XmStringContext) context, False, False, &len, &val);
}

/*
 * fetch the first text 'segment' of the external TCS that matches the given
 * char set.
 */
Boolean
XmStringGetLtoR(
  XmString string,
  XmStringTag tag,
  char **text )
{
  XmStringContext context;
  char * t;
  XmStringTag c, curtag = NULL;
  XmStringDirection d;
  Boolean s, is_local = FALSE, done = FALSE, is_default = FALSE;

  _XmProcessLock();
  if (!string) {
	_XmProcessUnlock();
	return(FALSE);
  }
  if (!tag) {
	_XmProcessUnlock();
	return (FALSE);
  }

  if ((tag == XmFONTLIST_DEFAULT_TAG) ||
      (strcmp(tag, XmFONTLIST_DEFAULT_TAG) == 0))
    is_local = TRUE;

  *text = NULL;				  /* pre-condition result */

  if (!is_local)
    {
      if ((strcmp(tag, XmSTRING_DEFAULT_CHARSET) == 0))
	{
	  curtag = XmStringGetCharset();
	  is_default = TRUE;
	}
      else curtag = tag;
    }

  XmStringInitContext (&context, string);

  while ( ! done)
    {
      if (XmStringGetNextSegment (context, &t, &c, &d, &s))
	{
	  if (c && ((d == XmSTRING_DIRECTION_L_TO_R) ||
		    (d == XmSTRING_DIRECTION_UNSET)) &&
	      (((is_local || is_default) &&
		((c == XmFONTLIST_DEFAULT_TAG) ||
		 (strcmp(c, XmFONTLIST_DEFAULT_TAG) == 0) ||
		 (strcmp(c, XmStringGetCharset()) == 0))) ||
	       (curtag && (strcmp (c, curtag) == 0))))
	    {
	      *text = t;		  /* OK, pass text to caller */
	      done = TRUE;
	    }
	  else
	    XtFree (t);			  /* not this text */

	  if (c)
	    XtFree (c);			  /* always dump charset */
	}
      else
	done = TRUE;
    }

  XmStringFreeContext (context);
  _XmProcessUnlock();
  return (*text != NULL);
}

/*
 * _XmStringGetSegment: A generalized version of XmStringGetNextSegment.
 *	Returns char_count, and allows explicit control over peeking
 *	and copying of data.  Interleaving of calls to _XmStringGetSegment()
 *	and XmeStringGetComponent() is supported.
 */
Boolean
_XmStringGetSegment(_XmStringContext   context,
		    Boolean	       update_context,
		    Boolean	       copy_data,
		    XtPointer         *text,
		    XmStringTag       *tag,
		    XmTextType        *type,
		    XmStringTag      **rendition_tags,
		    unsigned int      *tag_count,
		    XmStringDirection *direction,
		    Boolean           *separator,
		    unsigned char     *tabs,
		    short             *char_count,
		    XmDirection       *push_before,
		    Boolean	      *pop_after)
{
  XmStringTag*   	perm_rends = NULL;
  _XmStringContextRec	local_context_data;
  _XmStringContext	local_context = context;
  Boolean		result = FALSE;
  Boolean 		done;
  Boolean		new_renditions;
  XmStringComponentType ctype;
  unsigned int 		len;
  XtPointer 		val;

  /* Initialize the out parameters */
  if (text)           *text           = NULL;
  if (tag)            *tag            = NULL;
  if (type)           *type           = XmCHARSET_TEXT;
  if (rendition_tags) *rendition_tags = NULL;
  if (tag_count)      *tag_count      = 0;
  if (direction)      *direction      = _XmStrContDir(context);	/* for BC */
  if (separator)      *separator      = False;
  if (tabs)           *tabs           = 0;
  if (char_count)     *char_count     = 0;
  if (push_before)    *push_before    = 0;
  if (pop_after)      *pop_after      = False;

  /* No NULL pointers allowed. */
  if (! (context && text && tag && type && rendition_tags && tag_count
	 && direction && separator && tabs && char_count && push_before
	 && pop_after))
    return False;

  if (_XmStrContError(context))
    return False;

  /* Setup a writable context. */
  if (!update_context)
    {
      local_context = &local_context_data;
      _XmStringContextCopy(local_context, context);
    }

  /* N.B.: This code relies on the order of components from XmeStringGetComponent()! */
  done = new_renditions = FALSE;
  while (!done)
    {
      /* Peek at components before consuming them. */
      ctype = XmeStringGetComponent(local_context, FALSE, FALSE, &len, &val);
      switch (ctype)
	{
	case XmSTRING_COMPONENT_LAYOUT_PUSH:
	  if (*tabs || *text)
	    done = TRUE;
	  else
	    *push_before = *((XmDirection *) val);
	  break;

	case XmSTRING_COMPONENT_RENDITION_BEGIN:
	  if (*text)
	    done = TRUE;
	  else if (*tabs)
	    new_renditions = TRUE;
	  break;

	case XmSTRING_COMPONENT_TAG:
	case XmSTRING_COMPONENT_LOCALE:
	  if (*text)
	    done = TRUE;
	  else
	    *tag = (XmStringTag) val;
	  break;

	case XmSTRING_COMPONENT_TAB:
	  if (*text)
	    done = TRUE;
	  else
	    {
	      /* Save the renditions now. */
	      if ((*tag_count == 0) && _XmStrContRendCount(local_context))
		{
		  *tag_count = _XmStrContRendCount(local_context);
		  if (copy_data)
		    {
		      int tmp;
		      *rendition_tags = (XmStringTag *)
			XtMalloc(sizeof(XmStringTag) * *tag_count);
		      for (tmp = 0; tmp < *tag_count; tmp++)
			(*rendition_tags)[tmp] =
			  XtNewString(_XmStrContRendTags(local_context)[tmp]);
		    }
		  else
		    {
		      perm_rends = (XmStringTag *)
			XtMalloc(sizeof(XmStringTag) * *tag_count);
		      memcpy((char*) perm_rends,
			     _XmStrContRendTags(local_context),
			     sizeof(XmStringTag) * *tag_count);
		      *rendition_tags = perm_rends;
		    }
		}

	      /* Return at the end of this line. */
	      (*tabs)++;
	      result = TRUE;
	    }
	  break;

	case XmSTRING_COMPONENT_DIRECTION:
	  if (*text)
	    done = TRUE;
	  else
	    *direction = *((XmStringDirection *) val);
	  break;

	case XmSTRING_COMPONENT_TEXT:
	case XmSTRING_COMPONENT_LOCALE_TEXT:
	case XmSTRING_COMPONENT_WIDECHAR_TEXT:
	  if (*text)
	    done = TRUE;
	  else if (*tabs && new_renditions)
	    {
	      /* Tabs had a different set of renditions than the text, */
	      /* so we can't return both tabs and text at once. */
	      done = TRUE;
	    }
	  else
	    {
	      *char_count = len;
	      *text = val;

	      if (ctype == XmSTRING_COMPONENT_TEXT)
		*type = XmCHARSET_TEXT;
	      else if (ctype == XmSTRING_COMPONENT_LOCALE_TEXT)
		*type = XmMULTIBYTE_TEXT;
	      else if (ctype == XmSTRING_COMPONENT_WIDECHAR_TEXT)
		*type = XmWIDECHAR_TEXT;
	      else
		{ assert(FALSE); }

	      /* Force a tag for backward compatibility with Motif 1.2 */
	      if (! *tag)
		*tag = _XmStrContTag(local_context);

	      result = TRUE;

	      /* Save the renditions now. */
	      if ((*tag_count == 0) && _XmStrContRendCount(local_context))
		{
		  *tag_count = _XmStrContRendCount(local_context);
		  if (copy_data)
		    {
		      int tmp;
		      *rendition_tags = (XmStringTag*)
			XtMalloc(sizeof(XmStringTag) * *tag_count);
		      for (tmp = 0; tmp < *tag_count; tmp++)
			(*rendition_tags)[tmp] =
			  XtNewString(_XmStrContRendTags(local_context)[tmp]);
		    }
		  else
		    {
		      perm_rends = (XmStringTag *)
			XtMalloc(sizeof(XmStringTag) * *tag_count);
		      memcpy((char*) perm_rends,
			     _XmStrContRendTags(local_context),
			     sizeof(XmStringTag) * *tag_count);
		      *rendition_tags = perm_rends;
		    }
		}
	    }
	  break;

	case XmSTRING_COMPONENT_RENDITION_END:
	  break;

	case XmSTRING_COMPONENT_LAYOUT_POP:
	  if (*tabs || *text)
	    {
	      /* We're almost done, so record this pop. */
	      *pop_after = TRUE;
	    }
	  else
	    {
	      /* We're ignoring this pop, so discard any recorded push. */
	      *push_before = 0;
	    }
	  break;

	case XmSTRING_COMPONENT_SEPARATOR:
	  if (*tabs || *text)
	    {
	      *separator = TRUE;
	      done = TRUE;
	    }
	  break;

	case XmSTRING_COMPONENT_END:
	default:
	  done = TRUE;
	  break;
	}

      /* Consume the component if we aren't done. */
      if (!done)
	(void) XmeStringGetComponent(local_context, TRUE, FALSE, &len, &val);
    }

  if (copy_data && result)
    {
      /* Copy the tag. */
      if (*tag)
	*tag = XtNewString(*tag);

      /* Copy the text. */
      if (*text)
	{
	  char *tmp = XtMalloc(*char_count + sizeof(wchar_t));
	  memcpy(tmp, *text, *char_count);
	  bzero(tmp + *char_count, sizeof(wchar_t));

	  *text = (XtPointer) tmp;
	}
    }

  /* Free the local context. */
  if (local_context == &local_context_data)
    _XmStringContextFree(local_context);

  return result;
}

Boolean
_XmStringGetNextSegment(
        _XmStringContext context,
        XmStringTag *tag,
        XmStringDirection *direction,
        char **text,
        short *char_count,
        Boolean *separator )
{
  Boolean       result;
  XmTextType    type;
  XmStringTag * rendition_tags;
  unsigned int  tag_count;
  unsigned char tabs;
  XmDirection   push_before;
  Boolean	pop_after;

  /* Get all the fields and discard the ones we don't want. */
  result = _XmStringGetSegment(context, True, True, (XtPointer*) text, tag,
		       &type, &rendition_tags, &tag_count, direction,
		       separator, &tabs, char_count, &push_before, &pop_after);
  if (result) {
    if (rendition_tags)
      {
	while (tag_count-- > 0)
	  XtFree((char*) rendition_tags[tag_count]);
	XtFree((char*) rendition_tags);
      }

    if (type == XmWIDECHAR_TEXT && *text) {
      /* must convert (this should be done in segment's locale instead) */
      int len;
      wchar_t *wtext = (wchar_t *)(*text);

      /* should be enough */
      len = ((*char_count)*MB_CUR_MAX)/sizeof(wchar_t);

      *text = (char *)XtMalloc(len+1);
      *char_count = wcstombs(*text, wtext, len);
      if ((*char_count) == (size_t)-1) {
	result = False;
	XtFree(*text); *text = NULL;
	XtFree((char *)*tag); *tag = NULL;
      } else
	(*text)[*char_count] = '\0';
      XtFree((char *)wtext);
    }
  }

  return result;
}

/*
 * fetch the next 'segment' of the external TCS
 */
Boolean
XmStringGetNextSegment(XmStringContext context,
		       char **text,
		       XmStringTag *tag,
		       XmStringDirection *direction,
		       Boolean *separator )
{
  short char_count;
  Boolean ret_val;

  _XmProcessLock();
  ret_val = _XmStringGetNextSegment((_XmStringContext)context,
		 tag, direction, text, &char_count, separator);
  _XmProcessUnlock();
  return ret_val;
}

int XmStringLength(XmString string)
{
	return (int)XmStringSerialize(string, NULL);
}

/* Made public */
char *_XmStringGetCurrentCharset(void)
{
	return XmStringGetCharset();
}

/* Renamed */
Boolean XmeStringIsValid(const XmString string)
{
	return XmStringIsValid(string);
}

unsigned int XmCvtXmStringToByteStream(const XmString string, unsigned char **ret)
{
	return (unsigned int)XmStringSerialize(string, ret);
}

XmString XmCvtByteStreamToXmString(const unsigned char *stream)
{
	return XmStringUnserialize(stream);
}

extern unsigned int XmStringByteStreamLength(const unsigned char *stream)
{
	return (unsigned int)XmStringSerializedLength(stream);
}

