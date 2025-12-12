/* $TOG: TxtPropCv.c /main/15 1997/06/18 17:46:05 samborn $ */
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <Xm/XmosP.h>
#include "XmI.h"
#include "XmStringI.h"

static Atom
GetLocaleEncodingAtom(Display *dpy)
{
  int ret_status = 0;
  XTextProperty tmp_prop;
  char * tmp_string = "ABC";  /* these are characters in XPCS, so... safe */
  Atom encoding;

  tmp_prop.value = NULL; /* just in case X doesn't do it */
  ret_status = XmbTextListToTextProperty(dpy, &tmp_string, 1,
					 (XICCEncodingStyle)XTextStyle,
					 &tmp_prop);
  if (ret_status == Success)
    encoding = tmp_prop.encoding;
  else
    encoding = None;        /* XmbTextList... should always be able
			   * to convert XPCS characters; but in
			   * case its broken, this prevents a core
			   * dump.
			   */
  if (tmp_prop.value != NULL) XFree((char *)tmp_prop.value);
  return(encoding);
}

/************************************************************************
 *
 *  TextPropertyToSingleTextItem
 *
 ************************************************************************/

static int
TextPropertyToSingleTextItem(Display *display, XTextProperty *text_prop,
			     char **text_item)
{
    int result, count;
    char **textlist;

    result = XmbTextPropertyToTextList(display, text_prop, &textlist,
				       &count);
    if (result != Success) return(result);
    if (count == 1)
    {
	/* Otherwise just return the first element of the
	   text list */
	*text_item = XtNewString(textlist[0]);
	XFreeStringList(textlist);
    }
    else if (count > 1)
    {
	/* If we got back more than one string, then
	   let's concatenate them all together. */
	int i, length = 0;
	char *newstring;

	/* First figure out how big the final string will be */
	for (i = 0; i < count; ++i)
	  length += strlen(textlist[i]);
	/* Allocate a buffer and jam all the strings into it */
	newstring = (char *)XtMalloc(sizeof(char)*(length+1));
	newstring[0] = '\0';
	for (i = 0; i < count; ++i)
	  strcat(newstring, textlist[i]);
	*text_item = newstring;
	XFreeStringList(textlist);
    }
    return(Success);
}

/************************************************************************
 *
 *  GetTextSegment
 *
 *  This function gets the next full segment from the given xmstring using
 *  the given xmcontext and tries to extract either locale text or
 *  string text (i.e. ISO8859-1 text) based on the texttype argument.
 *
 *  This function can return one of three return values:
 *
 *    _VALID_SEGMENT: we found a segment that we could decompose. The
 *		appropriate text has been stored in the given buffer
 *
 *    _INVALID_SEGMENT: we could not decompose the segment. The given
 *		buffer does not contain valid data
 *
 *    _NO_MORE_SEGMENTS: there are no more segments available in the
 *		xmstring using the xmcontext.
 *
 ************************************************************************/

/* Segment type return value */
enum { _VALID_SEGMENT, _INVALID_SEGMENT, _NO_MORE_SEGMENTS };

/* Valid text types */
enum { _LOCALE_TEXT, _STRING_TEXT };

static unsigned char
GetTextSegment(Display *display, /* unused */
	       XmStringContext xmcontext,
	       XmString xmstring, /* unused */
	       char **buffer,
	       unsigned char texttype)
{
    XtPointer text;
    XmStringTag tag, *rendition_tags;
    XmTextType type;
    XmStringDirection direction;
    XmDirection push_before;
    Boolean separator, pop_after;
    unsigned int tag_count;
    unsigned char return_status, tabs;
    short char_count;
    char *encoding;
    int i;

    /* Initialize the locale_buffer just in case we need to return
       Failure at any point along the way. */
    *buffer = NULL;
    return_status = _VALID_SEGMENT;

    /* Decompose the xmstring and handle each segment separately. */
    if (_XmStringGetSegment(xmcontext, TRUE, FALSE, &text, &tag, &type,
			    &rendition_tags, &tag_count, &direction,
			    &separator, &tabs, &char_count,
			    &push_before, &pop_after))
    {
	switch (type)
	{
	  case XmMULTIBYTE_TEXT:
	  case XmWIDECHAR_TEXT:
	    if (texttype == _LOCALE_TEXT)
	    {
	      /* The text should be already be valid locale text */
	      char *tmp = XtMalloc(char_count + sizeof(wchar_t));
	      memcpy(tmp, text, char_count);
	      bzero(tmp + char_count, sizeof(wchar_t));

	      *buffer = tmp;
	    }
	    else if (texttype == _STRING_TEXT)
	    {
		*buffer = NULL;
		return(_INVALID_SEGMENT);
	    }
	    break;
	  case XmCHARSET_TEXT:
	    /* "tag" is the charset when type is CHARSET_TEXT */
	    encoding = XmMapSegmentEncoding(tag);
	    if (encoding != NULL)
	    {
		if (texttype == _LOCALE_TEXT &&
		    (strcmp(encoding, _MOTIF_DEFAULT_LOCALE) == 0 ||
		     strcmp(encoding, XmFONTLIST_DEFAULT_TAG) == 0))
		  /* @@ Is ISO8859-1 valid here? */
		{
		  /* Given the above charset encodings, the text should
		     already be valid locale text. */
		  char *tmp = XtMalloc(char_count + sizeof(wchar_t));
		  memcpy(tmp, text, char_count);
		  bzero(tmp + char_count, sizeof(wchar_t));

		  *buffer = tmp;
		}
		else if (texttype == _STRING_TEXT &&
			 (strcmp(encoding, "ISO8859-1") == 0))
		{
		  /* The text is valid STRING text */
		  char *tmp = XtMalloc(char_count + sizeof(wchar_t));
		  memcpy(tmp, text, char_count);
		  bzero(tmp + char_count, sizeof(wchar_t));

		  *buffer = tmp;
		}
		else return(_INVALID_SEGMENT);
	    }
	    else return(_INVALID_SEGMENT); /* the encoding was unregistered */
	    break;
	case XmNO_TEXT:
	    break;
	}
	/* Before returning Success, check to see whether we need to
	   prepend any tabs or append any newlines. */
	if ((return_status == _VALID_SEGMENT) &&
	    (separator == True || tabs != 0))
	{
	    int newlength;
	    char *newstring;

	    newlength =
	      strlen(*buffer) + (separator ? 1 : 0) + tabs;
	    newstring = (char *)XtMalloc(sizeof(char) * (newlength + 1));
	    for (i = 0; i < tabs; i++) newstring[i] = '\t';
	    strcpy(&newstring[i], *buffer);
	    strcat(newstring, "\n");
	    XtFree(*buffer);
	    *buffer = newstring;
	}
	return(return_status);
    }
    else
    {
	/* Return NULL text but a value of Success to designate that
	   there are no more segments to process. */
	*buffer = NULL;
	return(_NO_MORE_SEGMENTS);
    }
}

/************************************************************************
 *
 *  GetUseableText
 *
 *  Given an XmString, decompose it into text specified by the texttype
 *  argument. Valid text types are _LOCALE_TEXT for text encoded in the
 *  current locale and _STRING_TEXT for text encoded in ISO8859-1 with
 *  newlines and tabs.
 *
 *  If the "strict" argument is False, then any compound strings which
 *  have a segment that can not be decomposed as specified will be converted
 *  to compound text and then converted to the appropriate text by using
 *  a series of XmbTextListToTextProperty and XmbTextPropertyToTextList
 *  calls to acchive the conversion.
 *
 *  If the "strict" argument is True, then all compound strings must be
 *  fully convertible to the specified text type. If one is not in any
 *  way, then XLocaleNotSupported is returned.
 *
 ************************************************************************/

static int
GetUseableText(Display *display, XmString xmstring, char **buffer,
	       Boolean strict, unsigned char texttype)
{
    _XmStringContextRec stack_context;
    XTextProperty text_prop_return;
    char *text = NULL, *final_string = NULL, *text_item=NULL, *compound_text;
    unsigned char return_status;
    int result, size_so_far = 1; /* initialized for the ending NULL */
    XICCEncodingStyle encoding_style;

    /* Initialize the buffer in case we have to abort and return
       failure. */
    *buffer = NULL;
    switch(texttype)
    {
      case _LOCALE_TEXT:
	encoding_style = XTextStyle;
	break;
      case _STRING_TEXT:
	encoding_style = XStringStyle;
	break;
      default:
	return(XLocaleNotSupported);
    }

    /* Decompose the xmstring and handle each segment separately. */
    _XmStringContextReInit(&stack_context, xmstring);

    /* Get text for each segment of the compound string. Concatenate
       all the text from the segments together into one string that will
       be returned in the buffer. */
    while((return_status =
	   GetTextSegment(display, &stack_context, xmstring,
			  &text, texttype)) == _VALID_SEGMENT)
    {
	size_so_far += strlen(text);
	final_string = (char *)XtRealloc(final_string, size_so_far);
	final_string[0] = '\0';
	strcat(final_string, text);
	XtFree(text);
	text = NULL;
    }

    /* If we encountered an invalid segment, then we will throw up our
       hands and try all over again by converting the compound string to
       compound text and then converting the compound text to a text
       property and from a text property back to a text list. This should
       result in text but with possible loss of information. */
    if (return_status == _INVALID_SEGMENT)
    {
	int  txt_len;
	char *txt_value;

	/* First, free what we've malloc'ed already */
	if (final_string) XtFree(final_string);

	/* If we are in strict mode, then return XLocaleNotSupported to
	   let the caller know that we could not decompose one of the
	   XmStrings in the context of the current locale */
	if (strict)
	  {
	    _XmStringContextFree(&stack_context);
	    return(XLocaleNotSupported);
	  }

	/* Now convert the compound string to compound text ... */
	if ((compound_text = XmCvtXmStringToCT(xmstring))
	    == (char *)NULL)
	  {
	    _XmStringContextFree(&stack_context);
	    return (XLocaleNotSupported);
	  }
	/* then to a text property in TextStyle encoding ... */
	txt_len = strlen(compound_text) + 1;
	txt_value = XtMalloc((txt_len + 1) * sizeof(char));
	strcpy(txt_value, compound_text);
	text_prop_return.value = (unsigned char *) txt_value;
	text_prop_return.value[txt_len] = '\0';
	text_prop_return.nitems = txt_len;
	text_prop_return.encoding =
		XInternAtom(display, XmSCOMPOUND_TEXT, False);
	text_prop_return.format = 8;
	XtFree(compound_text);
	/* then to a text list, which should result in text. */
	result = TextPropertyToSingleTextItem(display, &text_prop_return,
					      &text_item);
	if (text_prop_return.value != NULL)
	    XtFree((char *)text_prop_return.value);

	if (result != Success)
	  {
	    _XmStringContextFree(&stack_context);
	    return(result);
	  }
	final_string = text_item;
    }

    /* Otherwise, we should have valid text */
    *buffer = final_string;
    _XmStringContextFree(&stack_context);
    return(Success);
}

/************************************************************************
 *
 *  XmCvtXmStringTableToTextProperty
 *
 ************************************************************************/
int
XmCvtXmStringTableToTextProperty(Display *display,
				 XmStringTable string_table,
				 int count,
				 XmICCEncodingStyle style,
				 XTextProperty *text_prop_return)
{
  char **compound_text, **useable_text, *xm_compound_text, *ptr;
  unsigned char *ubufptr, *bufptr,  texttype =_LOCALE_TEXT , *final_string;
  int i, result, total_size;
  Boolean strict = True;
  Atom encoding = 0;
  _XmDisplayToAppContext(display);

  _XmAppLock(app);
  switch (style)
    {
    case XmSTYLE_COMPOUND_TEXT:
      compound_text = (char **)XtMalloc(sizeof(char *) * count);
      /* Convert each XmString to Compound Text */
      for (i = 0, total_size = 0; i < count; ++i) {
	compound_text[i] = XmCvtXmStringToCT(string_table[i]);
	total_size += (compound_text[i] ? strlen(compound_text[i]) : 0) +1;
      }
      /* Generate the resulting XTextProperty value as a set of
	 null-separated compound text elements. A final terminating
	 null is stored at the end of the value field of text_prop_return
	 but is not included in the nitems member. */
      ptr = xm_compound_text =
	  (char *) XtMalloc(sizeof(char) * (total_size + 1));
      for (i = 0; i < count; i++)
      {
	if (compound_text[i])
	{
	  strcpy(ptr, compound_text[i]);
	  XtFree(compound_text[i]);
	}
	else
	{
	  *ptr = '\0';
	}
	ptr += strlen(ptr) + 1;
      }
      *ptr = '\0';
      XtFree((char *) compound_text);
      text_prop_return->value = (unsigned char *) xm_compound_text;
      text_prop_return->encoding =
	  XInternAtom(display, XmSCOMPOUND_TEXT, False);
      text_prop_return->format = 8;
      text_prop_return->nitems = total_size;
      _XmAppUnlock(app);
      return(Success);

    case XmSTYLE_COMPOUND_STRING:
      /* First calculate how much space the compound strings will occupy
	 when they are all converted to ASN1 strings. */
      for (i = 0, total_size = 0; i < count; ++i)
	total_size += XmCvtXmStringToByteStream(string_table[i],NULL);
      /* Allocate that amount of space and convert the compound strings
	 to ASN1 strings, putting them directly into the buffer. */
      text_prop_return->value = ubufptr =
	(unsigned char *) XtMalloc(sizeof(unsigned char) * total_size);
      for (i = 0; i < count; ++i)
	{
	  int size;
	  size = XmCvtXmStringToByteStream(string_table[i],&bufptr);
	  memcpy(ubufptr, bufptr, size);
	  XtFree((char *) bufptr);
	  ubufptr += size;
	}
      *(++ubufptr) = '\0';
      text_prop_return->nitems = total_size;
      text_prop_return->format = 8;
      text_prop_return->encoding =
	XInternAtom(display, XmS_MOTIF_COMPOUND_STRING, False);
      _XmAppUnlock(app);
      return(Success);

    case XmSTYLE_LOCALE:
    case XmSTYLE_TEXT:
    case XmSTYLE_STRING:
    case XmSTYLE_STANDARD_ICC_TEXT:
      /* We do mostly the same thing for these four styles. Do another
	 switch to set up some parameters which will change the behavior
	 slightly depending on which style is being requested. */
      switch (style) {
      case XmSTYLE_LOCALE:
	  strict = False;
	  texttype = _LOCALE_TEXT;
	  encoding = GetLocaleEncodingAtom(display);
	  break;
      case XmSTYLE_TEXT:
	  strict = True;
	  texttype = _LOCALE_TEXT;
	  encoding = GetLocaleEncodingAtom(display);
	  break;
      case XmSTYLE_STRING:
	  strict = False;
	  texttype = _STRING_TEXT;
	  encoding = XA_STRING;
	  break;
      case XmSTYLE_STANDARD_ICC_TEXT:
	  strict = True;
	  texttype = _STRING_TEXT;
	  encoding = XA_STRING;
	  break;
      case XmSTYLE_COMPOUND_TEXT:
      case XmSTYLE_COMPOUND_STRING:
	  break;
	}
      /* Get useable text for each compound string */
      useable_text = (char **)XtMalloc(sizeof(char *) * count);
      for (i = 0; i < count; ++i)
	{
	  result = GetUseableText(display, string_table[i],
				  &useable_text[i], strict, texttype);
	  if (result != Success)
	    {
	      /* Free up what we have so far ... */
	      --i; /* skip the one that failed */
	      while (i >= 0) XtFree(useable_text[i--]);
	      if (strict)
		{
		  /* If we got back XLocaleNotSupported, then pretend the
		     caller asked for COMPOUND_TEXT style. */
		  if (result == XLocaleNotSupported)
		    {
		      /* Try to do a straight COMPOUND_TEXT conversion
			 and return whatever we got back. */
		      _XmAppUnlock(app);
		      return (XmCvtXmStringTableToTextProperty
			      (display, string_table, count,
			       XmSTYLE_COMPOUND_TEXT, text_prop_return));
		    }
		  else
		    {
		      /* Otherwise, we got back an error we didn't know how
			 to handle. Just return it. */
		      _XmAppUnlock(app);
		      return(result);
		    }
		}
	      else
		{
		  /* If we are not being strict, then an error was really
		     an error that we could not handle. */
		  _XmAppUnlock(app);
		  return(result);
		}
	    }
	}
      /* Now take the useable_text array and convert it to one
	 long string with null separated elements. */
      for (i = 0, total_size = 0; i < count; ++i)
	total_size += strlen(useable_text[i]) + 1;
      final_string =
	(unsigned char *)XtMalloc(sizeof(char) * (total_size + 1));
      final_string[0] = '\0';
      bufptr = final_string;
      for (i = 0; i < count; ++i)
	{
	  strcpy((char *)bufptr, useable_text[i]);
	  bufptr += strlen(useable_text[i]) + 1;
	}
      *bufptr = '\0';

      /* Fill in the text property with the data */
      text_prop_return->encoding = encoding;
      text_prop_return->value = final_string;
      text_prop_return->nitems = total_size;
      text_prop_return->format = 8;

      /* Clean up and leave town */
      for (i = 0; i < count; ++i) XtFree(useable_text[i]);
      XtFree((char *) useable_text);
      _XmAppUnlock(app);
      return(Success);

    default:
      _XmAppUnlock(app);
      return(XLocaleNotSupported);
    }
}

/************************************************************************
 *
 *  XmCvtTextPropertyToXmStringTable
 *
 ************************************************************************/
int
XmCvtTextPropertyToXmStringTable(Display *display,
				 XTextProperty *text_prop,
				 XmStringTable *string_table_return,
				 int *count_return)
{
    enum { XmACOMPOUND_TEXT, XmA_MOTIF_COMPOUND_STRING,
#if XM_UTF8
        XmAUTF8_STRING,
#endif
        NUM_ATOMS };
    static char* atom_names[] = { XmSCOMPOUND_TEXT, XmS_MOTIF_COMPOUND_STRING,
#if XM_UTF8
        XmSUTF8_STRING
#endif
	};

    char **text_list;
    int i, elements = 0;
    XmStringTable string_table;
    XmStringTag tag;
    XmTextType type;
    Atom LOCALE_ATOM = GetLocaleEncodingAtom(display);
    Atom atoms[XtNumber(atom_names)];
    _XmDisplayToAppContext(display);

    assert(XtNumber(atom_names) == NUM_ATOMS);
    XInternAtoms(display, atom_names, XtNumber(atom_names), False, atoms);

    _XmAppLock(app);
    if (text_prop->encoding == atoms[XmACOMPOUND_TEXT])
    {
	char *ptr;

	/* First found how many XmString we need to allocate. */
	for (*count_return = 1, i = 0; i < text_prop->nitems; i++)
	{
	    if (text_prop->value[i] == '\0')
		(*count_return)++;
	}
	string_table =
	    (XmStringTable)XtMalloc(sizeof(XmString) * (*count_return));
	/* Now convert each compound text to an XmString. */
	for (i = 0, ptr = (char *)text_prop->value;
	     i < *count_return;
	     i++, ptr += strlen(ptr) + 1)
	{
	    XmString tempstr;
	    tempstr = XmCvtCTToXmString(ptr);
	    string_table[i] = tempstr;
	}
	*string_table_return = string_table;

	_XmAppUnlock(app);
	return(Success);
    }
    else if (text_prop->encoding == atoms[XmA_MOTIF_COMPOUND_STRING])
    {
	unsigned char *asn1_head;

	/* First calculate how many elements there are */
	asn1_head = text_prop->value;
	for (elements = 0; *asn1_head != '\0'; ++elements)
	    asn1_head += XmStringByteStreamLength(asn1_head);

	/* Now allocate a string table to put them in */
	string_table = (XmStringTable)XtMalloc(sizeof(XmString) * elements);

	/* Run through again, converting the strings. */
	asn1_head = text_prop->value;
	for (elements = 0; *asn1_head != '\0'; ++elements)
	{
	    string_table[elements] = XmCvtByteStreamToXmString(asn1_head);
	    /* If the string is NULL, then we don't know what to do */
	    if (string_table[elements] == (XmString) NULL)
	    {
		while (elements > 0) XtFree((char *)string_table[--elements]);
		XtFree((char *)string_table);
		_XmAppUnlock(app);
		return(XConverterNotFound);
	    }
	    /* Find the next asn1 string header */
	    asn1_head += XmStringByteStreamLength(asn1_head);
	}
	*string_table_return = string_table;
	*count_return = elements;
	_XmAppUnlock(app);
	return(Success);
    }
    else if (text_prop->encoding == LOCALE_ATOM)
    {
	tag = _MOTIF_DEFAULT_LOCALE;
	type = XmMULTIBYTE_TEXT;
    }
    else if (text_prop->encoding == XA_STRING)
    {
	tag = "ISO8859-1";
	type = XmCHARSET_TEXT;
    }
#if XM_UTF8
    else if (text_prop->encoding == atoms[XmAUTF8_STRING])
    {
	tag = "UTF-8";
	type = XmCHARSET_TEXT;
    }
#endif
    else {
	_XmAppUnlock(app);
        return(XLocaleNotSupported);
    }


    /* We fell through the else-if's so pull apart the data in the text
       property and set up a return string table. */

    /* First count up how many string elements there are in the value. */
    for (i = 0, elements = 1; i < text_prop->nitems - 1; ++i)
    {
	/* The text prop value will have two NULL's at the end,
	   one for the end of the last string and one to terminate
	   the entire value. The terminating NULL will be excluded
	   by looping until i == nitems since the terminating NULL
	   is not included in the nitems calculation. */
	if (text_prop->value[i] == '\0')
	  ++elements;
    }
    /* Create an appropriately sized array of xmstrings */
    string_table =
      (XmStringTable)XtMalloc(sizeof(XmString) * (elements));

    /* Create XmStrings from each string in the value field */
    string_table[0] = XmStringGenerate((XtPointer)text_prop->value,
				       tag, type, NULL);
    for (i = 0, elements = 1; i < text_prop->nitems - 1; ++i)
    {
	if (text_prop->value[i] == '\0')
	  string_table[elements++] =
	    XmStringGenerate((XtPointer) &(text_prop->value[i + 1]),
			     tag, type, NULL);
    }
    *string_table_return = string_table;
    *count_return = elements;
    _XmAppUnlock(app);
    return(Success);
}

