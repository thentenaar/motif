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

#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$TOG: XmStringFunc.c /main/9 1999/10/14 11:20:50 mgreess $"
#endif
#endif

#include <Xm/XmosP.h>

#include "XmI.h"
#include "XmStringI.h"
#include "XmRenderTI.h"
#include "XmTabListI.h"

/********    Static Function Declarations    ********/

static void new_line(_XmString string) ;

/********    End Static Function Declarations    ********/


XmStringComponentType
XmStringPeekNextTriple(XmStringContext context)
{
  unsigned int len;
  XtPointer    val;

  return XmeStringGetComponent((_XmStringContext) context, False, False, &len, &val);
}

Boolean
XmStringHasSubstring(
        XmString string,
        XmString substring )
{
  _XmStringContextRec stack_context;
  char               *text;
  char               *subtext;
  short               char_count;
  short               subchar_count;
  Boolean             found;
  int                 i, j, max;
  _XmStringEntry      line, *entry, seg;
  XmStringComponentType type;
  unsigned int        len;
  XtPointer	      val;

  _XmProcessLock();
  if ((string == NULL) || (substring == NULL) || (XmStringEmpty(substring))) {
    _XmProcessUnlock();
    return (FALSE);
  }

  /*
   * The substring must be a one line/one segment string.
   */

  if (_XmStrEntryCountGet(substring) != 1) {
    _XmProcessUnlock();
    return (FALSE);
  }

  if (((entry = _XmStrEntryGet(substring)) != NULL) &&
      _XmEntrySegmentCountGet(entry[0]) > 1) {
    _XmProcessUnlock();
    return (FALSE);
  }

  /*
   * Get the text out of the substring.
   */

  if (_XmStrOptimized(substring))
    {
      subchar_count = (short)_XmStrByteCount(substring);
      subtext = (char *)_XmStrText(substring);
    }
  else if (_XmStrMultiple(substring))
    {
      line = entry[0];

      if (_XmEntryMultiple(line))
	{
	  seg = (_XmStringEntry)_XmEntrySegmentGet(line)[0];

	  subchar_count = (short)_XmEntryByteCountGet(seg);
	  subtext = (char*) _XmEntryTextGet(seg);
	}
      else
	{
	  subchar_count = (short)_XmEntryByteCountGet(line);
	  subtext = (char*) _XmEntryTextGet(line);
	}
    }
  else { 		  /* Oops, some weird string! */
    _XmProcessUnlock();
    return (FALSE);
  }


  if ((subchar_count == 0) || (subtext == NULL)) {
    _XmProcessUnlock();
    return (FALSE);
  }

  /** Find a text component that matches. **/
  if (string) {
    _XmStringContextReInit(&stack_context, string);
    while ((type = XmeStringGetComponent(&stack_context, TRUE, FALSE,
					 &len, &val)) !=
	   XmSTRING_COMPONENT_END)
      {
	switch(type)
	  {
	  case XmSTRING_COMPONENT_TEXT:
	  case XmSTRING_COMPONENT_LOCALE_TEXT:
	  case XmSTRING_COMPONENT_WIDECHAR_TEXT:

	    char_count = len;
	    text = (char *)val;

	    if (char_count >= subchar_count) {
	      max = char_count - subchar_count;
	      for (i = 0; i <= max; i++) {
		found = TRUE;

		for (j = 0; j < subchar_count; j++) {
		  if (text[i+j] != subtext[j])  {
		    found = FALSE;
		    break;
		  }
		}
		if (found) {
		  _XmStringContextFree(&stack_context);
    		  _XmProcessUnlock();
		  return(TRUE);
		}
	      }
	    }
	    break;
	  default:
	    break;
	  }
      }
    _XmStringContextFree(&stack_context);
  }
  _XmProcessUnlock();
  return (FALSE);
}

XmStringTable
XmStringTableParseStringArray(XtPointer   *strings,
			      Cardinal     count,
			      XmStringTag  tag,
			      XmTextType   type,
			      XmParseTable parse,
			      Cardinal     parse_count,
			      XtPointer    call_data)
{
  int	i;
  XmStringTable strs;

  _XmProcessLock();
  if ((strings == NULL) || (count == 0)) {
    _XmProcessUnlock();
    return(NULL);
  }

  strs = (XmStringTable)XtMalloc(count * sizeof(XmString));

  for (i = 0; i < count; i++)
    {
      strs[i] = XmStringParseText(strings[i], NULL, tag, type,
				  parse, parse_count, call_data);
    }

  _XmProcessUnlock();
  return(strs);
}

XtPointer *
XmStringTableUnparse(XmStringTable     table,
		     Cardinal	       count,
		     XmStringTag       tag,
		     XmTextType        tag_type,
		     XmTextType        output_type,
		     XmParseTable      parse,
		     Cardinal          parse_count,
		     XmParseModel      parse_model)
{
  XtPointer	*strs;
  int		i;

  _XmProcessLock();
  if ((table == NULL) || (count == 0)) {
    _XmProcessUnlock();
    return(NULL);
  }

  strs = (XtPointer *)XtMalloc(count * sizeof(XtPointer));

  for (i = 0; i < count; i++)
    strs[i] = XmStringUnparse(table[i], tag, tag_type, output_type,
			      parse, parse_count, parse_model);
  _XmProcessUnlock();
  return(strs);
}

XmString
XmStringTableToXmString(XmStringTable  table,
			Cardinal       count,
			XmString       break_comp)
{
  /* Note: this is a very expensive way to do this.  Fix for Beta */
  int		i;
  XmString	str = NULL, tmp1, tmp2;

  _XmProcessLock();
  tmp1 = NULL;

  for (i = 0; i < count; i++)
    {
      tmp2 = XmStringConcatAndFree(tmp1, XmStringCopy(table[i]));
      str = XmStringConcatAndFree(tmp2, XmStringCopy(break_comp));

      tmp1 = str;
    }

  _XmProcessUnlock();
  return(str);
}

XmString
XmStringPutRendition(XmString string,
		     XmStringTag rendition)
{
  /* Quick and dirty.  Fix for beta! */
  XmString	str, tmp1, tmp2;

  tmp1 = XmStringComponentCreate(XmSTRING_COMPONENT_RENDITION_BEGIN,
				 strlen(rendition), (XtPointer)rendition);
  tmp2 = XmStringConcatAndFree(tmp1, XmStringCopy(string));

  tmp1 = XmStringComponentCreate(XmSTRING_COMPONENT_RENDITION_END,
				 strlen(rendition), (XtPointer)rendition);
  str = XmStringConcatAndFree(tmp2, tmp1);

  return(str);
}

void
XmParseMappingGetValues(XmParseMapping mapping,
			ArgList        arg_list,
			Cardinal       arg_count)
{
  Cardinal i;
  String arg_name;

  _XmProcessLock();
  /* Do a little error checking. */
  if (mapping == NULL) {
    _XmProcessUnlock();
    return;
  }

  /* Modify the specified values. */
  for (i = 0; i < arg_count; i++)
    {
      arg_name = arg_list[i].name;

      if ((arg_name == XmNpattern) ||
	  (strcmp(arg_name, XmNpattern) == 0))
	*((XtPointer*)arg_list[i].value) = mapping->pattern;
      else if ((arg_name == XmNpatternType) ||
	       (strcmp(arg_name, XmNpatternType) == 0))
	*((XmTextType*)arg_list[i].value) = mapping->pattern_type;
      else if ((arg_name == XmNsubstitute) ||
	       (strcmp(arg_name, XmNsubstitute) == 0))
	*((XmString*)arg_list[i].value) = XmStringCopy(mapping->substitute);
      else if ((arg_name == XmNinvokeParseProc) ||
	       (strcmp(arg_name, XmNinvokeParseProc) == 0))
	*((XmParseProc*)arg_list[i].value) = mapping->parse_proc;
      else if ((arg_name == XmNclientData) ||
	       (strcmp(arg_name, XmNclientData) == 0))
	*((XtPointer*)arg_list[i].value) = mapping->client_data;
      else if ((arg_name == XmNincludeStatus) ||
	       (strcmp(arg_name, XmNincludeStatus) == 0))
	*((XmIncludeStatus*)arg_list[i].value) = mapping->include_status;
    }
  _XmProcessUnlock();
}

void
XmParseMappingFree(XmParseMapping mapping)
{
  _XmProcessLock();
  if (mapping != NULL)
    {
      /* Free copied data. */
      XmStringFree(mapping->substitute);

      /* Free the record. */
      XtFree((char*) mapping);
    }
  _XmProcessUnlock();
}

void
XmParseTableFree(XmParseTable parse_table,
		 Cardinal     parse_count)
{
  /* Free each entry in the table. */
  Cardinal i;

  _XmProcessLock();
  for (i = 0; i < parse_count; i++)
    XmParseMappingFree(parse_table[i]);

  /* Free the table itself. */
  XtFree((char*) parse_table);
  _XmProcessUnlock();
}

/*
 * XmeGetNextCharacter: An XmParseProc to consume the triggering
 *	character and insert the following character.
 */
XmIncludeStatus
XmeGetNextCharacter(XtPointer     *in_out,
		    XtPointer      text_end,
		    XmTextType     type,
		    XmStringTag    tag,
		    XmParseMapping entry, /* unused */
		    int		   pattern_length,
		    XmString      *str_include,
		    XtPointer      call_data) /* unused */
{
  char* ptr = (char*) *in_out;
  int len = 0;
  XmStringComponentType comp_type;
  assert(in_out != NULL);

  _XmProcessLock();
  /* Initialize the out parameters */
  *str_include = NULL;

  /* Consume the triggering characters. */
  ptr += pattern_length;

  /* Select the component type. */
  switch (type)
    {
    case XmCHARSET_TEXT:
      if ((tag != NULL) && (strcmp(XmFONTLIST_DEFAULT_TAG, tag) == 0))
	comp_type = XmSTRING_COMPONENT_LOCALE_TEXT;
      else
	comp_type = XmSTRING_COMPONENT_TEXT;
      if ((text_end == NULL) || (ptr < (char*) text_end))
	len = mblen(ptr, MB_CUR_MAX);
      break;

    case XmMULTIBYTE_TEXT:
      /* In Motif 2.0 dynamic switching of locales isn't supported. */
      comp_type = XmSTRING_COMPONENT_LOCALE_TEXT;
      if ((text_end == NULL) || (ptr < (char*) text_end))
	len = mblen(ptr, MB_CUR_MAX);
      break;

    case XmWIDECHAR_TEXT:
      comp_type = XmSTRING_COMPONENT_WIDECHAR_TEXT;
      if ((text_end == NULL) || (ptr < (char*) text_end))
	len = sizeof(wchar_t);
      break;

    default:
      comp_type = XmSTRING_COMPONENT_UNKNOWN;
      break;
    }

  /* Quit if mblen() failed or if type was unrecognized. */
  if ((len <= 0) || (comp_type == XmSTRING_COMPONENT_UNKNOWN))
    {
      *in_out = (XtPointer) ptr;
      _XmProcessUnlock();
      return XmINSERT;
    }

  /* Create a component containing the next character. */
  *str_include = XmStringComponentCreate(comp_type, len, ptr);
  ptr += len;
  *in_out = (XtPointer) ptr;

  _XmProcessUnlock();
  return XmINSERT;
}

static void
new_line(
        _XmString string )
{
    int lc = _XmStrEntryCount(string);
    _XmStringEntry line;

    _XmStrImplicitLine(string) = TRUE;

    _XmStrEntry(string) = (_XmStringEntry *)
      XtRealloc((char *) _XmStrEntry(string),
		sizeof(_XmStringEntry) * (lc + 1));

    _XmEntryCreate(line, XmSTRING_ENTRY_ARRAY);
    _XmStrEntry(string)[lc] = line;

    _XmEntrySegmentCount(line) = 0;
    _XmEntrySegment(line) = NULL;

    _XmStrEntryCount(string)++;
}

static XmString
MakeStrFromSeg(XmStringContext start)
{
  _XmStringEntry	*line;
  _XmStringEntry	*segs, seg;
  _XmString		str;

  if (_XmStrContOpt(start)) {
    _XmStrContError(start) = TRUE;
    return(XmStringCopy(_XmStrContString(start)));
  } else {
    /* get segment */
    line = _XmStrEntry(_XmStrContString(start));

    /* Create XmString structure */
    _XmStrCreate(str, XmSTRING_MULTIPLE_ENTRY, 0);

    if (_XmEntryMultiple(line[_XmStrContCurrLine(start)])) {
      segs = (_XmStringEntry*)_XmEntrySegment(line[_XmStrContCurrLine(start)]);

      new_line(str);

      if (_XmStrContCurrSeg(start) < _XmEntrySegmentCount(line)) {
	seg = segs[_XmStrContCurrSeg(start)];

	_XmStringSegmentNew(str, 0, seg, True);

	_XmStrContCurrSeg(start)++;

	_XmStrContDir(start)     = _XmEntryDirectionGet(seg);
	_XmStrContTag(start)     = _XmEntryTag(seg);
	_XmStrContTagType(start) = (XmTextType) _XmEntryTextTypeGet(seg);
      } else {
	new_line(str);
	_XmStrContCurrSeg(start) = 0;
	_XmStrContCurrLine(start)++;
      }
    } else {
      seg = line[_XmStrContCurrLine(start)];
      _XmStringSegmentNew(str, 0, seg, True);

      _XmStrContCurrSeg(start) = 0;
      _XmStrContCurrLine(start)++;

      _XmStrContDir(start)     = _XmEntryDirectionGet(seg);
      _XmStrContTag(start)     = _XmEntryTag(seg);
      _XmStrContTagType(start) = (XmTextType) _XmEntryTextTypeGet(seg);
    }
    _XmStrContState(start)   = PUSH_STATE;
  }
  return(str);
}

static Boolean
LastSeg(XmStringContext start)
{
  _XmStringEntry	*line;

  if (_XmStrContOpt(start))
    {
      return(TRUE);
    } else {
      line = _XmStrEntry(_XmStrContString(start));

      if (_XmEntryMultiple(line[_XmStrContCurrLine(start)]))
	return(_XmStrContCurrSeg(start) == _XmEntrySegmentCount(line));
      else return(TRUE);
    }
}

static Boolean
ContextsMatch(XmStringContext a,
	      XmStringContext b)
{
  if ((_XmStrContCurrLine(a) == _XmStrContCurrLine(b)) &&
      (_XmStrContCurrSeg(a) == _XmStrContCurrSeg(b)) &&
      (_XmStrContState(a) == _XmStrContState(b)))

    if (((_XmStrContState(a) == BEGIN_REND_STATE) ||
	(_XmStrContState(a) == END_REND_STATE)))

      if (_XmStrContRendIndex(a) == _XmStrContRendIndex(b))
	return(TRUE);
      else return(FALSE);

    else return(TRUE);

  else return(FALSE);
}

static XmString
MakeStr(XmStringContext start,
	XmStringContext end)
{
  /* This is quick and dirty, need to be smarter about it before Beta. */
  XmStringComponentType	type;
  unsigned int		len;
  XtPointer		val;
  XmString		str;


  /* Next component over start until at segment break */
  str = NULL;

  while (_XmStrContState(start) != PUSH_STATE)
    {
      type = XmeStringGetComponent(start, TRUE, FALSE, &len, &val);

      if (ContextsMatch(start, end)) return(str);

      str = XmStringConcatAndFree(str,
				  XmStringComponentCreate(type, len, val));
    }

  /* Next segment over start incrementing until one segment before context */
  while ((_XmStrContCurrLine(start) < (_XmStrContCurrLine(end) - 1)) ||
	 ((_XmStrContCurrLine(start) == _XmStrContCurrLine(end)) &&
	  (_XmStrContCurrSeg(start) < _XmStrContCurrSeg(end))) ||
	 !LastSeg(start))
    {
      str = XmStringConcatAndFree(str, MakeStrFromSeg(start));
    }

  /* Next component over start until it matches context */
  type = XmeStringGetComponent(start, TRUE, FALSE, &len, &val);
  while (!ContextsMatch(start, end))
    {
      str = XmStringConcatAndFree(str, XmStringComponentCreate(type, len, val));
      type = XmeStringGetComponent(start, TRUE, FALSE, &len, &val);
  }

  return(str);
}

Cardinal
XmStringToXmStringTable(XmString string,
			XmString break_component,
			XmStringTable *table)
{
  /* Note: this is a very expensive way to do this.  Fix for Beta */
  _XmStringContextRec	stack_context, stack_start;
  XmStringComponentType	type, b_type;
  unsigned int		len, b_len;
  XtPointer		val, b_val;
  int			i, count;

  _XmProcessLock();
  /* Get triple for first component of break_component */
  if (break_component)
    {
      _XmStringContextReInit(&stack_context, break_component);
      b_type = XmeStringGetComponent(&stack_context, TRUE, FALSE,
				     &b_len, &b_val);
      _XmStringContextFree(&stack_context);
    }
  else
    /* Nothing to match against.  Return complete string. */
    {
      if (table != NULL)
	{
	  *table = (XmStringTable)XtMalloc(sizeof(XmString));
	  *table[0] = XmStringCopy(string);
	}
      _XmProcessUnlock();
      return(1);
    }

  /* Get context */
  if (!string)
    {
      if (table != NULL) *table = NULL;
      _XmProcessUnlock();
      return(0);
    }
  _XmStringContextReInit(&stack_context, string);

  /* Count number of entries for table */
  count = 0;
  while ((type = XmeStringGetComponent(&stack_context, TRUE, FALSE,
				       &len, &val)) !=
	 XmSTRING_COMPONENT_END)
    {
      if ((type == b_type) && (len == b_len) &&
	  (memcmp(val, b_val, len) == 0))
	count++;
    }

  /* Allocate table and insert new strings */
  if (table != NULL)
    {
      *table = (XmStringTable)XtMalloc(count * sizeof(XmString));

      _XmStringContextReInit(&stack_context, string);
      _XmStringContextReInit(&stack_start, string);

      i = 0;

      while ((type = XmeStringGetComponent(&stack_context, TRUE, FALSE,
					   &len, &val)) !=
	     XmSTRING_COMPONENT_END)
	{
	  if ((type == b_type) && (len == b_len) &&
	      (memcmp(val, b_val, len) == 0))
	    {
	      /* make XmString from start to end */
	      (*table)[i] = MakeStr(&stack_start, &stack_context);
	      i++;
	    }
	}

      _XmStringContextFree(&stack_start);
    }
  _XmStringContextFree(&stack_context);

  _XmProcessUnlock();
  return(count);
}



XmTabList
XmStringTableProposeTablist(XmStringTable strings,
			    Cardinal num_strings,
			    Widget widget,
			    float pad_value,
			    XmOffsetModel offset_model)
{
  int			i, j;
  _XmStringContextRec	stack_ctx;
  XmTabList		tl;
  XmTab			tab, prev, start;
  float			width, val;
  unsigned char		units;
  Arg			args[1];
  int			n;
  _XmRenditionRec	scratch;
  XmRendition		rend;
  _XmRendition		tmp;
  XmRenderTable		rt;
  NextTabResult		ret_val;

  _XmProcessLock();
  if ((strings == NULL) || (num_strings == 0)) {
    _XmProcessUnlock();
    return ((XmTabList)NULL);
  }

  bzero((char*) &scratch, sizeof(_XmRenditionRec));
  tmp = &scratch;
  rend = &tmp;

  _XmRendDisplay(rend) = XtDisplayOfObject(widget);

  n = 0;
  XtSetArg(args[n], XmNrenderTable, &rt); n++;
  XtGetValues(widget, args, n);

  /* Work around weird bug with XtGetValues. */
  n = 0;
  XtSetArg(args[n], XmNunitType, &units); n++;
  XtGetValues(widget, args, n);

  if (rt == NULL) rt = XmeGetDefaultRenderTable(widget, XmTEXT_FONTLIST);

  tab = XmTabCreate(0.0, units, offset_model, XmALIGNMENT_BEGINNING, ".");

  tl = XmTabListInsertTabs(NULL, &tab, 1, 0);

  XmTabFree(tab);

  for (i = 0; i < num_strings; i++)
    {
      if (!strings[i])
	{
	  /* Clean up */
	  XmTabListFree(tl);
	  _XmProcessUnlock();
	  return((XmTabList)NULL);
	}
      _XmStringContextReInit(&stack_ctx, strings[i]);

      tab = _XmTabLStart(tl);
      val = 0.0;

      /* Scan str for tabs, update tl if necessary. */
      j = 0;

      while ((ret_val = _XmStringGetNextTabWidth(&stack_ctx, widget, units,
						 rt, &width, &rend)) !=
	     XmTAB_EOS)
	{
	  if (ret_val == XmTAB_NEWLINE)
	    {
	      tab = _XmTabLStart(tl);
	      j = 0;
	      continue;
	    }

	  val = width + pad_value;

	  if (j >= _XmTabLCount(tl))
	    /* Need to add a tab */
	    {
	      tab = XmTabCreate(0.0, units, offset_model,
				XmALIGNMENT_BEGINNING, ".");
	      start = _XmTabLStart(tl);
	      prev = _XmTabPrev(start);

	      _XmTabNext(prev) = tab;
	      _XmTabPrev(tab) = prev;
	      _XmTabNext(tab) = start;
	      _XmTabPrev(start) = tab;
	      _XmTabLCount(tl)++;
	    }
	  else if (j > 0)
	    {
	      tab = _XmTabNext(tab);
	    }

	  if (val > _XmTabValue(tab)) XmTabSetValue(tab, val);
	  else val = _XmTabValue(tab);
	  j++;
	}

      _XmStringContextFree(&stack_ctx);
    }

  if (offset_model == XmABSOLUTE)
    {
      start = _XmTabLStart(tl);
      val = _XmTabValue(start);

      for (tab = _XmTabNext(start); tab != start; tab = _XmTabNext(tab))
	{
	  val += _XmTabValue(tab);
	  XmTabSetValue(tab, val);
	}
    }

  _XmProcessUnlock();
  return(tl);
}

/*
 * Helper function for XmTabList.c
 * This routine performs successive reads on an XmStringContext
 * and returns the width (in units of XmNunitType of widget) of
 * the text segments between the previous and next tab or end of line.
 * It uses the XmNrenderTable from widget to calculate the width.  It
 * returns XmTAB_EOS if the end of the string has been reached, XmTAB_NEWLINE
 * if the end of line is reached and XmTAB_NEXT if a tab is encountered.
 */
NextTabResult
_XmStringGetNextTabWidth(XmStringContext ctx,
			 Widget widget,
			 unsigned char units,
			 XmRenderTable rt,
			 float *width,
			 XmRendition *rend)
{
  float 	divisor;
  int		toType;			  /*  passed to XmConvertUnits */
  Dimension	w_sum, w_cur;

  if (_XmStrContError(ctx))
    {
      *width = 0.0;
      return(XmTAB_EOS);
    }

  w_sum = 0;
  *width = 0.0;

  /* Big units need to be converted to small ones. */
  toType = _XmConvertFactor(units, &divisor);

  /* Calculate the width to the next tab. */
  if (_XmStrContOpt(ctx))
    {
      _XmStrContError(ctx) = True;
      return(XmTAB_EOS);
    }
  else
    {
      _XmString 		str = _XmStrContString(ctx);
      _XmStringEntry 		line;
      int			line_count;
      _XmStringEntry 		seg;
      int			seg_count;
      _XmStringArraySegRec	array_seg;

      line_count = _XmStrLineCountGet(str);

      /* Keep checking lines and segments until we run out or hit a tab. */
      if (_XmStrContCurrLine(ctx) < line_count)
	{
	  if (_XmStrImplicitLine(str))
	    {
	      line = _XmStrEntry(str)[_XmStrContCurrLine(ctx)];
	    }
	  else
	    {
	      _XmEntryType(&array_seg) = XmSTRING_ENTRY_ARRAY;
	      _XmEntrySegmentCount(&array_seg) = _XmStrEntryCount(str);
	      _XmEntrySegment(&array_seg) = (_XmStringNREntry *)_XmStrEntry(str);
	      line = (_XmStringEntry)&array_seg;
	    }

	  if (_XmEntryMultiple(line))
	    seg_count = _XmEntrySegmentCount(line);
	  else
	    seg_count = 1;

	  if (seg_count == 0) {
	    /* Empty line. */
	    _XmStrContCurrLine(ctx)++;
	    *width = 0.0;
	    return(XmTAB_NEWLINE);
	  }

	  while (_XmStrContCurrSeg(ctx) < seg_count)
	    {
	      if (_XmEntryMultiple(line))
		seg = (_XmStringEntry)_XmEntrySegment(line)[_XmStrContCurrSeg(ctx)];
	      else
		seg = line;

	      w_cur = 0;

	      if (_XmStrContTabCount(ctx) < _XmEntryTabsGet(seg)) {
		_XmStrContTabCount(ctx)++;
		*width = (XmConvertUnits(widget, XmHORIZONTAL,
					 XmPIXELS, w_sum, toType) / divisor);
		return(XmTAB_NEXT);
	      }

	      (void)_XmStringSegmentExtents(seg, rt, rend, NULL,
					    &w_cur, NULL, NULL, NULL);
	      w_sum += w_cur;

	      _XmStrContCurrSeg(ctx)++;
	      _XmStrContTabCount(ctx) = 0;
	    }

	  _XmStrContCurrLine(ctx)++;
	  _XmStrContCurrSeg(ctx) = 0;
	  _XmStrContTabCount(ctx) = 0;

	  return(XmTAB_NEWLINE);
	}

      _XmStrContError(ctx) = True;
      return(XmTAB_EOS);
    }
}

