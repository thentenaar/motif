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
static char rcsid[] = "$TOG: XmString.c /main/34 1998/04/16 14:35:32 mgreess $"
#endif
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <limits.h>		/* for MB_LEN_MAX */
#include <ctype.h>

#ifdef __cplusplus
extern "C" { /* some 'locale.h' do not have prototypes (sun) */
#endif
#include <X11/Xlocale.h>
#ifdef __cplusplus
} /* Close scope of 'extern "C"' declaration */
#endif /* __cplusplus */

#include <Xm/Display.h>		/* for XmGetXmDisplay */
#include <Xm/DisplayP.h>	/* for noFontCallback list */
#include <Xm/XmosP.h>
#include "MessagesI.h"
#include "ResIndI.h"
#include "XmI.h"
#include "XmosI.h"
#include "XmRenderTI.h"
#include "XmStringI.h"
#include "XmTabListI.h"

/* Warning Messages */
#define NO_FONT_MSG	_XmMMsgXmString_0000

/* These are the os-specific environment variables checked for a language
** specification.
*/
#define env_variable "LANG"

struct __Xmlocale {
    char   *tag;
    int    taglen;
    Boolean inited;
};

/* enums for which_seg for calculating widths */
enum { XmSTRING_FIRST_SEG, XmSTRING_MIDDLE_SEG, XmSTRING_LAST_SEG,
	 XmSTRING_SINGLE_SEG
	 };

/* Enums for marking internal_flags field in XmParseMappings. */
enum {
  XmSTRING_UNPARSE_UNKNOWN,	/* Parse mapping hasn't been examined. */
  XmSTRING_UNPARSE_PLAUSIBLE,	/* Parse mapping might unparse. */
  XmSTRING_UNPARSE_IMPLAUSIBLE	/* Parse mapping will never unparse. */
  };

/* Values for drawing underlining renditions. */
#define SINGLE_OFFSET 1
#define DOUBLE_OFFSET 2

#define Half(x)		(x >> 1)

/* _XmStringEntry macros only acessible inside XmString code */

#define _XmEntryRendCountedBegins(entry, count)				   \
	(_XmEntryOptimized(entry) ? 					   \
	 (((count) && _XmEntryRendIndex(entry) != REND_INDEX_UNSET) ? 	   \
	   &(_tag_cache[_XmEntryRendIndex(entry)]) : NULL) :	  	   \
	 _XmUnoptSegRendBegins(entry))

#define _XmEntryRendBegins(entry) 					   \
	(_XmEntryOptimized(entry) ? 					   \
	 ((_XmEntryRendBeginCountGet(entry) && 				   \
	   _XmEntryRendIndex(entry) != REND_INDEX_UNSET) ? 		   \
	   &(_tag_cache[_XmEntryRendIndex(entry)]) : NULL) :	  	   \
	 _XmUnoptSegRendBegins(entry))

#define _XmEntryRendCountedEnds(entry, count)				   \
        (_XmEntryOptimized(entry) ?					   \
	 (((count) && _XmEntryRendIndex(entry) != REND_INDEX_UNSET) ? 	   \
	  &(_tag_cache[_XmEntryRendIndex(entry)]) : NULL) :		   \
	 _XmUnoptSegRendEnds(entry))

#define _XmEntryRendEnds(entry) 					   \
        (_XmEntryOptimized(entry) ?					   \
	 ((_XmEntryRendEndCountGet(entry) && 				   \
	   _XmEntryRendIndex(entry) != REND_INDEX_UNSET) ? 		   \
	  &(_tag_cache[_XmEntryRendIndex(entry)]) : NULL) :		   \
	 _XmUnoptSegRendEnds(entry))


/*
 * this set constructs ASN.1 XmString format object.  The TLV version
 */

/*
 	The ASN.1 version of XmString is:

	COMPOUND_STRING			4 or 6 bytes (see description below)

	    component tag		1 byte
	    length			1 or 3 bytes
	    value			n bytes

	    component tag		1 byte
	    length			1 or 3 bytes
	    value			n bytes

	eg. very simple...
*/


/*
 * ASN.1 header for compound string - 3 byte header, followed by length
 * which is three bytes maximum, but almost always 1 byte.
 *
 * The first byte defines the ASN.1 space:  (0xdf)
 *              1 1      0      1 1 1 1 1
 *              class    form   ID code
 *
 *    class is private, form is primitive (not constructed from other
 *    forms), and the ID code value means the actual ID code value is
 *    extended into one or more octets.
 *

 * The second and third bytes define the actual ID code value.  The
 * value used for 1.2 is the inverse of the original XUI value.
 *     second byte:  (0x80)
 *               1       0000000
 *              MSB      high seven bits of ID code
 *
 *     third byte:   (0x06)
 *               0       0000110
 *              LSB      low seven bits of ID code
 *

 * The length field of the ASN.1 conformant compound string header
 * is dynamically constructed.  There are two possible forms depending
 * upon the length of the string.  Note that this length excludes the
 * header bytes.
 *
 *    Short Form: range 0 .. 127
 *    one byte
 *                  0         nnnnnnn
 *                 short       7 bit length
 *
 *    Long Form: range 128 .. 2**16-1
 *    three bytes
 *    first:        1         nnnnnnn
 *                 long       number of bytes to follow
 *
 *    second:
 *                  nnnnnnnn
 *                  MSB of length
 *
 *    third:
 *                  nnnnnnnn
 *                  LSB of length
 *

 * This process for constructing the length field will also be
 * used to construct the length field within individual tag-length-value
 * triplets.
 */

#define ASNHEADERLEN     3
static XmConst unsigned char ASNHeader[ASNHEADERLEN] = { 0xdf, 0x80, 0x06 };


#define MAXSHORTVALUE   127             /* maximum len to be used for short
                                           length form */
#define CSLONGLEN       3
#define CSSHORTLEN      1
#define CSLONGLEN1      0x82
#define CSLONGBIT	0x80

#define ASNTAG		1
/* Num bytes for tag & length = ASNTAG + [CSSHORTLEN | CSLONGLEN] */

#define HEADER 3	/* num bytes for tag & length */

/*
 * calculates the number of bytes in the header of an external compound
 * string, given the total length of the components.
 */

#define _calc_header_size(len) \
    ((((unsigned short)(len)) > MAXSHORTVALUE) ? (ASNHEADERLEN + CSLONGLEN) : \
     (ASNHEADERLEN + CSSHORTLEN))

#define _asn1_size(len) \
    ((((unsigned short)(len)) > MAXSHORTVALUE) ? (ASNTAG + CSLONGLEN) : \
     (ASNTAG + CSSHORTLEN))

#define _is_asn1_long(p) \
  ((*((unsigned char *)(p) + ASNTAG)) & ((unsigned char)CSLONGBIT))

/********    Static Function Declarations    ********/

static Boolean _is_short_length(
                        unsigned char *p) ;
static void _write_long_length(
                        unsigned char *p,
                        unsigned short length) ;
static unsigned char * _write_header(
                        unsigned char *p,
                        unsigned short length) ;
static unsigned char * _read_header(
                        unsigned char *p) ;
static unsigned short _read_header_length(
                        unsigned char *p) ;
static unsigned short _read_length(
                        unsigned char *p) ;
static unsigned short _read_string_length(
                        unsigned char *p) ;
static unsigned char * _write_component(
                        unsigned char *p,
                        unsigned char tag,
                        unsigned short length,
                        unsigned char *value,
                        Boolean move_by_length) ;
static unsigned char * _read_component(
                        unsigned char *p,
                        unsigned char *tag,
                        unsigned short *length,
                        unsigned char *value) ;
static Boolean RenditionsCompatible(_XmStringEntry seg1,
				    _XmStringEntry seg2);
static void MergeEnds(_XmStringEntry a,
		      _XmStringEntry b);
static void MergeBegins(_XmStringEntry a,
			_XmStringEntry b);
static Boolean _is_asn1(unsigned char *string) ;
static XmString Clone(XmString string, int lines);
static void OptLineMetrics(XmRenderTable rendertable,
			   _XmString line,
			   XmRendition *rend_io,
			   XmRendition base_rend,
			   Dimension *width,
			   Dimension *height,
			   Dimension *ascender,
			   Dimension *descender );
static Dimension OptLineAscender(
                        XmRenderTable f,
                        _XmStringOpt opt) ;
static void LineMetrics(_XmStringEntry line,
			XmRenderTable r,
			XmRendition *rend_io,
			XmRendition base,
			XmDirection prim_dir,
			Dimension *width,
			Dimension *height,
			Dimension *ascender,
			Dimension *descender);
static void SubStringPosition(
                        Boolean one_byte,
			XmRenderTable rt,
                        XmRendition entry,
                        _XmStringEntry seg,
                        _XmStringEntry under_seg,
                        Position x,
                        Dimension *under_begin,
                        Dimension *under_end) ;
static void recursive_layout(_XmString string,
			     int *line_index,
			     int *seg_index,
                             XmDirection direction,
                             XmDirection p_direction,
			     int depth);
static void last_direction(_XmStringEntry line,
			   int *index,
			   XmDirection *direction);
static void DrawLine(Display *d,
		     Window w,
		     Screen **pscreen,
		     int x,
		     int y,
		     _XmStringEntry line,
		     XmRendition *scr_rend,
		     XmRendition base,
		     XmRenderTable rendertable,
		     XmDirection prim_dir,
		     Boolean image,
		     _XmString *underline,
		     Dimension descender,
                     Boolean opt,
		     Dimension opt_width,
		     Dimension opt_height
		     );
static void _calc_align_and_clip(
                        Display *d,
			Window w,
                        GC gc,
                        Position *x,
                        Position y,
                        Dimension width,
                        int line_width,
                        int line_height,
                        unsigned char lay_dir,
                        XRectangle *clip,
                        unsigned char align,
                        int descender,
                        int *restore,
			XmFontType font_type) ;
static void _draw(
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
                        Boolean image,
                        _XmString underline) ;
static void _render(Display *d,
		    Drawable w,
		    XmRenderTable rendertable,
		    XmRendition rend,
		    _XmString string,
		    Position x,
		    Position y,
		    Dimension width,
		    unsigned char align,
		    unsigned char lay_dir,
		    Boolean image,
		    _XmString underline,
		    XRectangle *clip);

static _XmString _XmStringOptCreate(
                        unsigned char *c,
                        unsigned char *end,
                        unsigned short textlen,
                        Boolean havetag,
                        unsigned int tag_index) ;
static void finish_segment(_XmString str,
			   _XmStringUnoptSeg seg,
			   int *lc,
			   int *sc,
			   Boolean *unopt,
			   XmStringDirection dir);
static _XmString _XmStringNonOptCreate(
                        unsigned char *c,
                        unsigned char *end,
                        Boolean havetag) ;
static Boolean SpecifiedSegmentExtents(_XmStringEntry entry,
				       XmRenderTable rendertable,
				       XmRendition *rend_in_out,
				       XmRendition base,
				       int which_seg,
				       Dimension *width,
				       Dimension *height,
				       Dimension *ascent,
				       Dimension *descent);

static void ComputeMetrics(XmRendition rend,
			   XtPointer text,
			   unsigned int byte_count,
			   XmTextType type,
			   int which_seg,
			   Dimension *width,
			   Dimension *height,
			   Dimension *ascent,
			   Dimension *descent,
                           Boolean utf8);
static Dimension ComputeWidth(unsigned char which,
			      XCharStruct char_ret);
static void _parse_locale(char *str, int *indx, int *len);
static Boolean match_pattern(XtPointer      text,
			     XmStringTag    tag,
			     XmTextType     type,
			     XmParseMapping pattern,
			     int            char_len,
			     Boolean        dir_change);
static void parse_unmatched(XmString  *result,
			    char     **ptr,
			    XmTextType text_type,
			    int        length);
static Boolean parse_pattern(XmString      *result,
			     char         **ptr,
			     XtPointer      text_end,
			     XmStringTag    tag,
			     XmTextType     type,
			     XmParseMapping pat,
			     int            length,
			     XtPointer      call_data,
			     Boolean       *terminate);
static void check_unparse_models(XmStringContext   context,
				 XmStringTag       tag,
				 XmTextType        tag_type,
				 XmParseModel      parse_model,
				 Boolean	  *prev_text_match,
				 Boolean	  *next_text_match,
				 Boolean	  *non_text_match);
static void unparse_text(char                **result,
			 int                  *length,
			 XmTextType            output_type,
			 XmStringComponentType c_type,
			 unsigned int          c_length,
			 XtPointer             c_value);
static Boolean unparse_is_plausible(XmParseMapping pattern);
static void unparse_components(char          **result,
			       int            *length,
			       XmTextType      output_type,
			       XmStringContext context,
			       XmParseTable    parse_table,
			       Cardinal        parse_count);

static void begin_context_rends(_XmStringContext context,
				Boolean	         update_context,
				XmStringTag	*renditions,
				int		 count);
static void end_context_rends(_XmStringContext context,
			      Boolean          update_context,
			      XmStringTag     *rendition,
			      int	       count);
static XFontStruct * GetFont(XmRenderTable rt,
			     _XmStringEntry entry);
static _XmStringCache CacheGet(_XmStringEntry entry,
			       int type,
			       int create,
			       XtPointer match_value);
static _XmStringEntry EntryCvtToOpt(_XmStringEntry entry);
static _XmStringEntry EntryCvtToUnopt(_XmStringEntry entry);

static XmString StringTabCreate(void);
static XmString StringEmptyCreate(void);

static int _get_generate_parse_table (XmParseTable *gen_table);

/********    End Static Function Declarations    ********/


static struct __Xmlocale locale;
static char **_tag_cache;
static int    _cache_count = 0;

/*
 * Determines whether this string has a short or long length field
 */
static Boolean
_is_short_length(
        unsigned char *p )
{

  unsigned char *uchar_p = (unsigned char *) p;

  uchar_p += ASNHEADERLEN;

  if (*uchar_p & (char)CSLONGBIT)
    return (FALSE);
  else return (TRUE);
}

/*
 * Routine that writes a long length field
 */
static void
_write_long_length(
        unsigned char *p,
        unsigned short length )
{

  unsigned char   * uchar_p = (unsigned char *) p;

  /*
   * flag the long version
   */
  *uchar_p = CSLONGLEN1;
  uchar_p++;

  /*
   * need to pull off the high 8 bits
   */

  *uchar_p = (unsigned char) (length >> 8);
  uchar_p++;
  *uchar_p = (unsigned char) (length & 0xff);

}


/*
 * Private routines for manipulating the ASN.1 header of external
 * compound strings.
 */
static unsigned char *
_write_header(
        unsigned char *p,
        unsigned short length )
{

  unsigned char * uchar_p = p;
  int     headlen;

  /* write the header in. */

  headlen = ASNHEADERLEN;
  memcpy( uchar_p, ASNHeader, ASNHEADERLEN);
  uchar_p += ASNHEADERLEN;

  /* short or long length value? */

  if (length > MAXSHORTVALUE)
    {
      _write_long_length(uchar_p, length);
      headlen += CSLONGLEN;
    }
  else {
    /* Short version */
    *uchar_p = (unsigned char) length;
    headlen += CSSHORTLEN;
  }
  return (p + headlen);
}

/*
 * extracts the ASN.1 header from the external compound string.
 */
static unsigned char *
_read_header(
        unsigned char *p )
{
  /*
   * Read past the ASN.1 header; get the first length byte and see if this
   * is a one or three byte length.
   */

  if (_is_short_length(p))
    return (p + ASNHEADERLEN + CSSHORTLEN);
  else
    return (p + ASNHEADERLEN + CSLONGLEN);
}

/*
 * reads the length the ASN.1 header of an external
 * compound string.
 */
static unsigned short
_read_header_length(
        unsigned char *p )
{
  /*
   * Get the first length byte and see if this
   * is a one or three byte length.
   */

  if (_is_short_length(p))
    return (ASNHEADERLEN + CSSHORTLEN);
  else
    return (ASNHEADERLEN + CSLONGLEN);

}

/*
 * calculates the length of the external compound string, excluding the
 * ASN.1 header.
 */
static unsigned short
_read_string_length(
        unsigned char *p )
{

  unsigned char * uchar_p = (unsigned char *) p;
  unsigned short totallen = 0;

  /*
   * Read past the ASN.1 header; get the first length byte and see if this
   * is a one or three byte length.
   */

  uchar_p += ASNHEADERLEN;

  if (_is_short_length(p))
    {
      totallen += (unsigned short) *uchar_p;
    }
  else {
    unsigned short i;

    uchar_p++;
    i = ((unsigned short) *uchar_p) << 8;
    uchar_p++;
    i |= ((unsigned short) *uchar_p); /* Mask on the low byte */
    totallen += i;
  }
  return (totallen);
}

/*
 * calculates length of component marked by a tag-length-value triple.
 */
static unsigned short
_read_asn1_length(
        unsigned char *p )
{
  unsigned char * uchar_p = (unsigned char *) p;
  unsigned short totallen = 0;

  /*
   * Read past the tag; get the first length byte and see if this
   * is a one or three byte length.
   */

  uchar_p += ASNTAG;

  if (_is_asn1_long(p))
    {
      unsigned short i;

      uchar_p++;
      i = ((unsigned short) *uchar_p) << 8;
      uchar_p++;
      i |= ((unsigned short) *uchar_p); /* Mask on the low byte */
      totallen += i;
    }
  else
    {
      totallen += (unsigned short) *uchar_p;
    }
  return (totallen);
}

/*
 * determines length of ASN.1 length field of component of external
 * compound string.
 */
static unsigned short
_read_length(
        unsigned char *p )
{
  /*
   * Read past the tag field; get the first length byte and see if this
   * is a one or three byte length.
   */
  if (_is_asn1_long(p))
    return ((unsigned short)(ASNTAG + CSLONGLEN));
  else
    return ((unsigned short)(ASNTAG + CSSHORTLEN));
}

/*
 * Private routines for reading/writing the individual compound string
 * components
 */
static unsigned char *
_write_component(
        unsigned char *p,
        unsigned char tag,
        unsigned short length,
        unsigned char *value,
        Boolean move_by_length )
{
  unsigned char * uchar_p = p;

  *uchar_p = tag;		/* stuff tag */
  uchar_p += ASNTAG;

  /* short or long length value? */
  if (length > MAXSHORTVALUE)
    {
      _write_long_length(uchar_p, length);
      uchar_p += CSLONGLEN;
    }
  else {
    /* Short version */
    *uchar_p = (unsigned char) length;
    uchar_p += CSSHORTLEN;
  }

  if (value != (unsigned char *) NULL)
    memcpy((char *)uchar_p, (char *)value, (size_t)length);

  if (move_by_length)
    return (uchar_p + length);
  else
    return (uchar_p);
}

static unsigned char *
_read_component(
        unsigned char *p,
        unsigned char *tag,
        unsigned short *length,
        unsigned char *value)
{
  unsigned char * uchar_p = (unsigned char *) p;

  *tag = *uchar_p;		/* read tag */

  *length = _read_asn1_length(p);

  uchar_p += _read_length(p);	/* move to value */
  if (value != NULL) memcpy(value, uchar_p, *length);

  return (uchar_p + *length);

}

/* Create a new XmString */
XmString
XmStringCreate(
        char *text,
        XmStringTag tag )
{
  XmString ret_val;

  _XmProcessLock();
  ret_val = _XmStringNCreate (text, tag, -1);
  _XmProcessUnlock();
  return ret_val;
}

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
      curtag = _XmStringGetCurrentCharset();
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
      _XmEntryTextSet((_XmStringEntry)&seg, text);
      _XmUnoptSegByteCount(&seg) = t_length;

      _XmStringSegmentNew(str, 0, (_XmStringEntry)&seg, True);
      return(str);
    }
}

/*
 * Convenience routine creating localized XmString from NULL terminated string.
 */
XmString
XmStringCreateLocalized(
        String text )
{
  return (XmStringGenerate(text, NULL, XmCHARSET_TEXT, NULL));
}

/* Create an optimized _XmString with only direction set. */
XmString
XmStringDirectionCreate(
        XmStringDirection direction )
{
  /* Maintain a static cache of the common results. */
  static XmConst XmStringDirection dir_index[] =
    {
      XmSTRING_DIRECTION_L_TO_R, XmSTRING_DIRECTION_R_TO_L,
      XmSTRING_DIRECTION_UNSET, XmSTRING_DIRECTION_DEFAULT
    };
  static _XmString cache_str[] = { NULL, NULL, NULL, NULL };

  int index;
  _XmString opt_str = NULL;

  _XmProcessLock();
  /* Find the static cache index and string for this direction. */
  assert(XtNumber(dir_index) == XtNumber(cache_str));
  for (index = 0; index < XtNumber(dir_index); index++)
    if (dir_index[index] == direction)
      {
	opt_str = cache_str[index];
	break;
      }

  /* Create the return string if necessary and this is a known direction. */
  if (!opt_str && (index < XtNumber(dir_index)))
    {
      _XmStrCreate(opt_str, XmSTRING_OPTIMIZED, 0);
      _XmStrDirection(opt_str) = direction;

      cache_str[index] = opt_str;
    }

  /* Try to copy a cached string by incrementing its reference count. */
  if ((index < XtNumber(dir_index)) &&
      (_XmStrRefCountInc(opt_str) == 0))
    {
      _XmStrRefCountDec(opt_str);	/* Undo previous increment. */
      XmStringFree(opt_str);		/* Release our cached copy. */
      cache_str[index] = NULL;
      opt_str = XmStringDirectionCreate(direction);
      _XmProcessUnlock();
      return (XmString) opt_str;
    }

  _XmProcessUnlock();
  return (XmString) opt_str;
}

/* Create an empty non-optimized _XmString containing a single newline */
XmString
XmStringSeparatorCreate( void )
{
  static _XmString str = NULL;
  XmString ret_val;

  _XmProcessLock();
  if (!str)
    {
      int i;

      _XmStrCreate(str, XmSTRING_MULTIPLE_ENTRY, 0);

      _XmStrImplicitLine(str) = True;
      _XmStrEntry(str) = (_XmStringEntry *)XtMalloc(2*sizeof(_XmStringEntry));
      _XmStrEntryCount(str) = 2;
      for (i = 0; i < 2; i++) {
	_XmEntryCreate(_XmStrEntry(str)[i], XmSTRING_ENTRY_OPTIMIZED);
      }
    }

  /* If the reference count gets too big free the old string. */
  if (_XmStrRefCountInc(str) == 0)
    {
      _XmStrRefCountDec(str);	/* Undo previous increment. */
      XmStringFree(str);	/* Release our cached copy. */
      str = NULL;
      ret_val = XmStringSeparatorCreate();
      _XmProcessUnlock();
      return ret_val;
    }

  ret_val = Clone(str, _XmStrEntryCountGet(str)); /* ??? */
  _XmProcessUnlock();
  return (XmString)ret_val;
}

/* Create an empty optimized _XmString containing a single tab. */
static XmString
StringTabCreate( void )
{
  static _XmString opt_str = NULL;

  if (!opt_str)
    {
      _XmStrCreate(opt_str, XmSTRING_OPTIMIZED, 0);
      _XmStrTabs(opt_str) = 1;
    }

  /* If the reference count gets too big free the old string. */
  if (_XmStrRefCountInc(opt_str) == 0)
    {
      _XmStrRefCountDec(opt_str); /* Undo previous increment. */
      XmStringFree(opt_str);     /* Release our cached copy. */
      opt_str = NULL;
      return StringTabCreate();
    }

  return (XmString)opt_str;
}

/* Create an empty optimized _XmString. */
static XmString
StringEmptyCreate( void )
{
  static _XmString opt_str = NULL;

  if (!opt_str)
    {
      _XmStrCreate(opt_str, XmSTRING_OPTIMIZED, 0);
    }

  /* If the reference count gets too big free the old string. */
  if (_XmStrRefCountInc(opt_str) == 0)
    {
      _XmStrRefCountDec(opt_str); /* Undo previous increment. */
      XmStringFree(opt_str);     /* Release our cached copy. */
      opt_str = NULL;
      return StringEmptyCreate();
    }

  return (XmString)opt_str;
}

/*
 * this set provides access to the internal components of XmStrings
 */

/*
 * set up the read-out context
 */
Boolean
XmStringInitContext(
        XmStringContext *context,
        XmString string )
{
  XmStringContext  	ct;

  _XmProcessLock();
  /* Initialize the out parameters. */
  if (context)	*context = NULL;

  /* make sure there is something in the string.  we are
     going to assume a good string in the get next routine
     */
  if (!(string && context)) {
	_XmProcessUnlock();
	return (FALSE);
  }

  ct = (XmStringContext) XtMalloc(sizeof(_XmStringContextRec));
  _XmStringContextReInit(ct, string);

  *context = ct;
  _XmProcessUnlock();
  return (TRUE);
}

/*
 * free the read-out context
 */
void
XmStringFreeContext(
        _XmStringContext context )
{
  _XmProcessLock();
  if (context)
    {
      _XmStringContextFree(context);
      XtFree((char *) context);
    }
  _XmProcessUnlock();
}

/*
 * this set is the TCS font list handling stuff
 */
XmStringTag
_XmStringIndexGetTag(int index)
{
  XmStringTag ret_val;
  _XmProcessLock();
  if (index > _cache_count) {
    _XmProcessUnlock();
    return NULL;
  }
  ret_val = _tag_cache[index];
  _XmProcessUnlock();
  return ret_val;
}

int
_XmStringIndexCacheTag(XmStringTag tag,
		       int length )
{
  char *a;
  register int i;

  /* Initialize cache with XmFONTLIST_DEFAULT_TAG, _MOTIF_DEFAULT_LOCALE, and
     locale.tag if necessary, to keep indices low. */

  _XmProcessLock();
  if (_cache_count == 0)
    {
      _tag_cache = (char **)XtMalloc(sizeof(char **) * 3);

      _tag_cache[_cache_count] = XmFONTLIST_DEFAULT_TAG;
      _cache_count++;
      _tag_cache[_cache_count] = _MOTIF_DEFAULT_LOCALE;
      _cache_count++;
      _tag_cache[_cache_count] = _XmStringGetCurrentCharset();
      _cache_count++;
    }

  /* Look for an existing cache entry. */
  for (i = 0; i < _cache_count; i++)
    {
      if (((tag == _tag_cache[i]) ||
	   ((length != XmSTRING_TAG_STRLEN) &&
	    (strncmp(tag, _tag_cache[i], length) == 0)) ||
	   ((length == XmSTRING_TAG_STRLEN) &&
	    (strcmp(tag, _tag_cache[i]) == 0))) &&
	  ((length == XmSTRING_TAG_STRLEN) || (_tag_cache[i][length] == '\0')))
	{
	  _XmProcessUnlock();
	  return( i) ;
	}
    }

  /* Add this entry to the cache. */
  if (length == XmSTRING_TAG_STRLEN) length = strlen(tag);

  _tag_cache = (char **) XtRealloc ((char *) _tag_cache,
				    sizeof (char **) * (_cache_count + 1));

  a = XtMalloc (length + 1);
  memcpy(a, tag, length);
  a[length] = '\0';

  _tag_cache[_cache_count] = a;
  _cache_count++;

  _XmProcessUnlock();
  return(i) ;
}

XmStringTag
_XmStringCacheTag(XmStringTag tag,
		  int length )
{
  int tag_index ;
  XmStringTag ret_val;

  _XmProcessLock();
  if (tag == NULL) {
      _XmProcessUnlock();
      return NULL;
  }
  tag_index = _XmStringIndexCacheTag( tag, length) ;
  ret_val = _tag_cache[tag_index] ;
  _XmProcessUnlock();
  return ret_val;
}

static Boolean
RenditionsCompatible(_XmStringEntry seg1,
		     _XmStringEntry seg2)
{
  XmStringTag	*begin1, *begin2, *end1, *end2;
  short		bcnt1, bcnt2, ecnt1, ecnt2;

  _XmProcessLock();
  bcnt1 = _XmEntryRendBeginCountGet(seg1);
  bcnt2 = _XmEntryRendBeginCountGet(seg2);
  ecnt1 = _XmEntryRendEndCountGet(seg1);
  ecnt2 = _XmEntryRendEndCountGet(seg2);
  begin1 = _XmEntryRendCountedBegins(seg1, bcnt1);
  begin2 = _XmEntryRendCountedBegins(seg2, bcnt2);
  end1 = _XmEntryRendCountedEnds(seg1, ecnt1);
  end2 = _XmEntryRendCountedEnds(seg2, ecnt2);
  _XmProcessUnlock();

  /* if seg1 is optimized, we are very limited in what renditions will be
     compatible, since there is only one rend_index. */
  if (_XmEntryOptimized(seg1) &&
      (((ecnt1 != 0) && (bcnt2 != 0)) ||
       ((bcnt1 > 0) && (bcnt2 > 0)) ||
       ((ecnt1 > 0) && (ecnt2 > 0)) ||
       ((bcnt1 > 0) && (ecnt2 > 0) && (begin1[0] != end2[0]))))
    return (FALSE);

  if ((_XmEntryByteCountGet(seg1) == 0) && (ecnt1 == 0))
    return(TRUE);

  if ((_XmEntryByteCountGet(seg2) == 0) && (bcnt2 == 0))
    return(TRUE);

  if ((ecnt1 == 0) && (bcnt2 == 0)) return(TRUE);

  return(FALSE);
}

static void
MergeEnds(_XmStringEntry a,
	  _XmStringEntry b)
{
  int		i;
  short		a_count, b_count;

  a_count = _XmEntryRendEndCountGet(a);
  b_count = _XmEntryRendEndCountGet(b);

  if ((a_count == 0) && (b_count == 0)) return;

  if (_XmEntryOptimized(a)) {
    if (a_count == 0 && b_count == 1)
      {
	_XmEntryRendIndex(a) = _XmEntryOptimized(b) ? _XmEntryRendIndex(b) :
	  _XmStringIndexCacheTag(_XmEntryRendEndGet(b, 0),
				 XmSTRING_TAG_STRLEN);
      	_XmEntryRendEndCountSet(a, 1);
      }
    return;
  }

  _XmUnoptSegRendEnds(a) =
    (XmStringTag *)XtRealloc((char *)_XmUnoptSegRendEnds(a),
			     (sizeof(XmStringTag) *
			      (a_count + b_count)));

  for (i = 0; i < b_count; i++)
      _XmUnoptSegRendEnds(a)[(a_count + i)] = _XmEntryRendEndGet(b, i);

  _XmEntryRendEndCountSet(a, (a_count + b_count));
}

static void
MergeBegins(_XmStringEntry a,
	    _XmStringEntry b)
{
  XmStringTag 	*b_begin;
  short		a_b_cnt;
  short		b_b_cnt;
  int		i;

  a_b_cnt = _XmEntryRendBeginCountGet(a);
  b_b_cnt = _XmEntryRendBeginCountGet(b);
  b_begin = _XmEntryRendCountedBegins(b, b_b_cnt);

  if ((a_b_cnt == 0) && (b_b_cnt == 0)) return;

  if (_XmEntryOptimized(a)) {
    if (a_b_cnt == 0 && b_b_cnt == 1)
      {
	_XmEntryRendIndex(a) = _XmEntryOptimized(b) ? _XmEntryRendIndex(b) :
	  _XmStringIndexCacheTag(b_begin[0], XmSTRING_TAG_STRLEN);
      	_XmEntryRendBeginCountSet(a, 1);
      }
    return;
  }

  _XmUnoptSegRendBegins(a) =
    (XmStringTag *)XtRealloc((char *)_XmUnoptSegRendBegins(a),
			     sizeof(XmStringTag) * (a_b_cnt + b_b_cnt));

  for (i = 0; i < b_b_cnt; i++)
      _XmUnoptSegRendBegins(a)[(a_b_cnt + i)] = _XmEntryRendBeginGet(b, i);

  _XmEntryRendBeginCountSet(a, (a_b_cnt + b_b_cnt));
}

/*
 * general external TCS utilties
 */

static Boolean
IsUnopt(_XmString str, int lines)
{
  _XmStringEntry *line;

  if (lines > 0)
    {
      if (!(line = _XmStrEntryGet(str)) || !*line)
          return False;

      if ((_XmEntrySegmentCountGet(*line) > 0) &&
	  (_XmEntryType(_XmEntrySegmentGet(*line)[0]) !=
	   XmSTRING_ENTRY_OPTIMIZED))
	return True;
    }
  return False;
}

static _XmStringEntry
Unoptimize(_XmStringEntry entry, int do_free)
{
  _XmStringEntry new_entry = NULL;
  _XmStringNREntry tmp_seg;
  int j;

  if (entry == NULL)
    return NULL;

  if (_XmEntryType(entry) == XmSTRING_ENTRY_OPTIMIZED) {
    new_entry = EntryCvtToUnopt(entry);
    if (do_free)
      _XmStringEntryFree(entry);
  } else if (_XmEntryMultiple(entry)) {
    if (do_free) {
      for (j = 0; j < _XmEntrySegmentCount(entry); j++) {
	tmp_seg = _XmEntrySegment(entry)[j];
	if (_XmEntryType(tmp_seg) == XmSTRING_ENTRY_OPTIMIZED) {
	  _XmEntrySegment(entry)[j] =
	    (_XmStringNREntry)EntryCvtToUnopt((_XmStringEntry)tmp_seg);
	  _XmStringEntryFree((_XmStringEntry)tmp_seg);
	}
      }
      new_entry = entry;
    } else {
      _XmEntryCreate(new_entry, XmSTRING_ENTRY_ARRAY);
      _XmEntrySegmentCount(new_entry) = _XmEntrySegmentCount(entry);
      _XmEntrySoftNewlineSet(new_entry, _XmEntrySoftNewlineGet(entry));
      _XmEntrySegment(new_entry) =
	(_XmStringNREntry *)XtMalloc(_XmEntrySegmentCount(entry) *
				     sizeof(_XmStringNREntry));
      for (j = 0; j < _XmEntrySegmentCount(entry); j++) {
	tmp_seg = _XmEntrySegment(entry)[j];
	if (_XmEntryType(tmp_seg) == XmSTRING_ENTRY_OPTIMIZED)
	  _XmEntrySegment(new_entry)[j] =
	    (_XmStringNREntry)EntryCvtToUnopt((_XmStringEntry)tmp_seg);
	else
	  _XmEntrySegment(new_entry)[j] =
	    (_XmStringNREntry)_XmStringEntryCopy((_XmStringEntry)tmp_seg);
      }
    }
  } else {
    if (do_free)
      new_entry = entry;
    else
      new_entry = _XmStringEntryCopy(entry);
  }
  return new_entry;
}


XmString
XmStringConcat(XmString a,
	       XmString b )
{
  return XmStringConcatAndFree (XmStringCopy(a), XmStringCopy(b));
}

XmString
XmStringConcatAndFree(XmString a,
		      XmString b)
{
  _XmString 		opt_str;
  unsigned int 		a_len, b_len, a_lc, b_lc, a_sc, b_sc, lc;
  unsigned int 		a_index, b_index, a_rend_index, b_rend_index;
  unsigned int 		a_tabs, b_tabs;
  XmTextType            a_type, b_type;
  XmString 		a_str, b_str;
  _XmStringMultiRec     b_tmp;
  _XmStringEntry 	a_line, b_line, *tmp_line=NULL, *segs=NULL;
  _XmStringNREntry 	a_last, b_seg = NULL, tmp_seg;
  String		a_tag;
  String		b_tag;
  int 			i, j;
  int			merged = 0;
  XmStringDirection	last = XmSTRING_DIRECTION_UNSET;
  Boolean		modify_a, modify_b, free_b;
  Boolean               a_needs_unopt=False, b_needs_unopt=False;
  _XmStringArraySegRec	array_seg;

  _XmProcessLock();
  if (a == NULL)
  {
    _XmProcessUnlock();
    return b;
  }
  if (b == NULL)
  {
    _XmProcessUnlock();
    return a;
  }

  if ((_XmStrOptimized(a) && _XmStrOptimized(b)))
    {
      /* Both optimized */
      a_len = _XmStrByteCount(a);
      b_len = _XmStrByteCount(b);

      a_index = _XmStrTagIndex(a);
      b_index = _XmStrTagIndex(b);

      a_rend_index = _XmStrRendIndex(a);
      b_rend_index = _XmStrRendIndex(b);

      a_type = (XmTextType) _XmStrTextType(a);
      b_type = (XmTextType) _XmStrTextType(b);

      a_tabs = _XmStrTabs(a);
      b_tabs = _XmStrTabs(b);

      if (((a_index == b_index) ||
	   (a_index == TAG_INDEX_UNSET) ||
	   (b_index == TAG_INDEX_UNSET)) &&
	  RenditionsCompatible((_XmStringEntry)a,
			       (_XmStringEntry)b) &&
	  ((_XmStrDirection(a) == _XmStrDirection(b)) ||
	   (_XmStrDirection(b) == XmSTRING_DIRECTION_UNSET) ||
	   ((_XmStrDirection(a) == XmSTRING_DIRECTION_UNSET) &&
	    (a_len == 0))) &&
	  (a_type == b_type || a_type == XmNO_TEXT || b_type == XmNO_TEXT) &&
	  ((a_len + b_len) < (1 << BYTE_COUNT_BITS)) &&
	  ((_XmStrText(a) && b_tabs==0) || (a_tabs+b_tabs <= 3)))
	{
	  /* Compatible strings.  Make an optimized string. */
	  if ((b_len == 0) && (_XmStrRefCountGet(a) == 1))
	    opt_str = (_XmString) a;
	  else if ((a_len == 0) && (_XmStrRefCountGet(b) == 1))
	    opt_str = (_XmString) b;
	  else
	    _XmStrCreate(opt_str, XmSTRING_OPTIMIZED, a_len + b_len);

	  _XmStrByteCount((_XmString)opt_str) = a_len + b_len;
	  _XmStrTextType((_XmString)opt_str) =
	    (a_type == XmNO_TEXT) ? b_type : a_type;
	  _XmStrTagIndex((_XmString)opt_str) =
	    (a_index == TAG_INDEX_UNSET) ? b_index : a_index;
	  _XmStrRendIndex((_XmString)opt_str) =
	    (a_rend_index == REND_INDEX_UNSET) ? b_rend_index : a_rend_index;

	  /* Push begin and end state. */
	  _XmStrRendBegin((_XmString)opt_str) =
	    (_XmStrRendBegin(a) ? _XmStrRendBegin(a) : _XmStrRendBegin(b));
	  _XmStrRendEnd((_XmString)opt_str) =
	    (_XmStrRendEnd(b) ? _XmStrRendEnd(b) : _XmStrRendEnd(a));

	  _XmStrDirection((_XmString)opt_str) =
	    (_XmStrDirection(a) == XmSTRING_DIRECTION_UNSET) ?
	      _XmStrDirection(b) : _XmStrDirection(a);
	  _XmStrTabs((_XmString)opt_str) = a_tabs+b_tabs;

	  if (a_len && (opt_str != a))
	    memcpy(_XmStrText((_XmString)opt_str), _XmStrText(a), a_len);
	  if (b_len && (opt_str != b))
	    memcpy((_XmStrText((_XmString)opt_str) + a_len),
		   _XmStrText(b), b_len);

	  if (opt_str != a)
	    XmStringFree(a);
	  if (opt_str != b)
	    XmStringFree(b);
	  _XmProcessUnlock();
	  return (XmString)opt_str;
	}
    }

  /* Concatenate non-optimized versions */
  a_lc = _XmStrEntryCountGet(a);
  b_lc = _XmStrEntryCountGet(b);

  if (_XmStrAddNewline(a)) {
    if (_XmStrAddNewline(b) && b_lc > 0)
      lc = a_lc + b_lc - 1;
    else
      lc = a_lc;
  } else {
    if (_XmStrAddNewline(b))
      lc = b_lc ? b_lc : 1;
    else
      lc = a_lc + b_lc;
  }

  modify_a = !_XmStrOptimized(a) && (_XmStrRefCountGet(a) == 1);
  if (modify_a)
    {
      a_str = a;
      if (a_lc > 1 && !_XmStrAddNewline(a) && _XmStrAddNewline(b)) {
	segs = _XmStrEntry(a_str);
	_XmStrEntry(a_str) = NULL;
      }
      _XmStrEntry(a_str) = (_XmStringEntry *)
	XtRealloc((char*) _XmStrEntry(a_str),
		  sizeof(_XmStringEntry) * lc);
      for (i = (segs ? 0 : a_lc); i < lc; i++)
	_XmStrEntry(a_str)[i] = NULL;
    }
  else if (_XmStrOptimized(a))
    {
      a_str = _XmStringOptToNonOpt((_XmStringOpt)a);
      if (a_lc > 1 && !_XmStrAddNewline(a) && _XmStrAddNewline(b)) {
	segs = _XmStrEntry(a_str);
	_XmStrEntry(a_str) = NULL;
      }
      _XmStrEntry(a_str) = (_XmStringEntry *)
	XtRealloc((char*) _XmStrEntry(a_str),
		  sizeof(_XmStringEntry) * lc);
      for (i = (segs ? 0 : a_lc); i < lc; i++)
	_XmStrEntry(a_str)[i] = NULL;
    }
  else {
    if (a_lc > 1 && !_XmStrAddNewline(a) && _XmStrAddNewline(b)) {
      segs = _XmStrEntry(a);
      _XmStrEntry(a) = NULL;
      _XmStrEntryCount(a) = 0;
    }
    a_str = Clone(a, lc);
    if (a_lc > 1 && !_XmStrAddNewline(a) && _XmStrAddNewline(b)) {
      _XmStrEntry(a) = segs;
      _XmStrEntryCount(a) = a_lc;
      if (segs) {
	segs = (_XmStringEntry *)XtMalloc(a_lc * sizeof(_XmStringEntry));
	for (i = 0; i < a_lc; i++)
	  segs[i] = _XmStringEntryCopy(_XmStrEntry(a)[i]);
      }
    }
  }

  if (segs) {
    /* need to move a:s segments down one level */
    _XmStringEntry line;

    _XmEntryCreate(line, XmSTRING_ENTRY_ARRAY);
    _XmEntrySegmentCount(line) = a_lc;
    _XmEntrySegment(line) = (_XmStringNREntry *)segs;
    _XmStrEntry(a_str)[0] = line;
    _XmStrImplicitLine(a_str) = True;
    a_lc = 1;
  }

  modify_b = !_XmStrOptimized(b) && (_XmStrRefCountGet(b) == 1);
  free_b = True;
  if (modify_b)
    b_str = b;
  else if (_XmStrOptimized(b)) {
#ifndef _XmDEBUG_XMSTRING_MEM
    /* This won't work in that case -
       _XmStrMalloc adds bytes at the beginning */
    if (_XmStrRefCountGet(b) == 1) {
      /*
       * An optimized XmString looks very much like an
       * optimized segment. In this case we can use the
       * optimized XmString as the segment with a little
       * of poking around in the last byte of the header.
       * This saves us from allocating a new string, which
       * will get free:d anyway.
       */
      b_str = (XmString)&b_tmp;
      _XmStrInit(b_str, XmSTRING_MULTIPLE_ENTRY);
      _XmStrEntryCount(b_str) = 1;
      _XmStrRefCountSet(b, 0);
      _XmEntryTabsSet(b, _XmStrTabs(b));
      _XmEntryImm(b) = 1;
      if (_XmStrText(b) != (char *)_XmEntryTextGet((_XmStringEntry)b)) {
	/* If the XtPointer in the union in the
	 * optimized segment leads to padding in the structure
	 * between the header and the text data
	 * (it will on some 64-bit architectures) we have
	 * to move the text data, since the optimized
	 * string does not have this padding.
	 */
	unsigned int size = sizeof(_XmStringOptSegRec);
	if (_XmStrByteCount(b) > sizeof(XtPointer))
	  size += _XmStrByteCount(b) - sizeof(XtPointer);
	b = (XmString)XtRealloc((char *)b, size);
	memmove(_XmEntryTextGet((_XmStringEntry)b),
		_XmStrText(b),
		_XmStrByteCount(b));
      }
      _XmStrEntry(b_str) = (_XmStringEntry *)&b;
      free_b = False;
    } else {
#endif
      b_str = _XmStringOptToNonOpt((_XmStringOpt)b);
      XmStringFree(b);
#ifndef _XmDEBUG_XMSTRING_MEM
    }
#endif
    modify_b = TRUE;
  } else
    b_str = b;

  assert((a != b) || (!modify_a && !modify_b));

  /* convert a to unopt segs if necessary */
  a_needs_unopt = IsUnopt(a_str, a_lc);

  if (!a_needs_unopt) {
    b_needs_unopt = IsUnopt(b_str, _XmStrEntryCount(b_str));

    if (b_needs_unopt)
      for (i = 0; i < a_lc; i++)
	_XmStrEntry(a_str)[i] = Unoptimize(_XmStrEntry(a_str)[i], True);
  }

  _XmStrEntryCount(a_str) = lc;

  _XmStrImplicitLine(a_str) =
    _XmStrImplicitLine(a_str) || _XmStrImplicitLine(b_str);

  /* Add first line of b_str to last line of a_str */
  a_line = _XmStrEntry(a_str)[a_lc - 1];
  if (_XmStrImplicitLine(b_str))
    {
      b_line = _XmStrEntry(b_str)[0];
    }
  else
    {
      _XmEntryType(&array_seg) = XmSTRING_ENTRY_ARRAY;
      _XmEntrySegmentCount(&array_seg) = _XmStrEntryCount(b_str);
      _XmEntrySegment(&array_seg) = (_XmStringNREntry *)_XmStrEntry(b_str);
      b_line = (_XmStringEntry)&array_seg;
      b_lc = 1;
    }

  a_sc = _XmEntrySegmentCountGet(a_line);
  b_sc = _XmEntrySegmentCountGet(b_line);

  if ((a_sc != 0) && (b_sc != 0))
    {
      /* Need to combine last segment of a with first of b if compatible. */
      a_last = _XmEntrySegmentGet(a_line)[a_sc - 1];
      b_seg = _XmEntrySegmentGet(b_line)[0];
      a_len = _XmEntryByteCountGet((_XmStringEntry)a_last);
      b_len = _XmEntryByteCountGet((_XmStringEntry)b_seg);

      /* Remember last direction set. */
      last = _XmEntryDirectionGet((_XmStringEntry)a_last);

      a_tag = _XmEntryTag((_XmStringEntry)a_last);
      b_tag = _XmEntryTag((_XmStringEntry)b_seg);

      a_type = (XmTextType) _XmEntryTextTypeGet((_XmStringEntry)a_last);
      b_type = (XmTextType) _XmEntryTextTypeGet((_XmStringEntry)b_seg);

      a_tabs = _XmEntryTabsGet((_XmStringEntry)a_last);
      b_tabs = _XmEntryTabsGet((_XmStringEntry)b_seg);

      merged = 0;

      if (((a_tag == b_tag) ||
	   (a_tag == NULL) || (b_tag == NULL)) &&
	  RenditionsCompatible((_XmStringEntry)a_last,
			       (_XmStringEntry)b_seg) &&
	  (a_type == b_type || a_type == XmNO_TEXT || b_type == XmNO_TEXT) &&
	  ((last == _XmEntryDirectionGet((_XmStringEntry)b_seg)) ||
	   (_XmEntryDirectionGet((_XmStringEntry)b_seg) ==
	    XmSTRING_DIRECTION_UNSET) ||
	   ((last == XmSTRING_DIRECTION_UNSET) &&
	    (a_len == 0))) &&
	  !_XmEntryPopGet((_XmStringEntry)a_last) &&
	  !_XmEntryPushGet((_XmStringEntry)b_seg) &&
	  (((a_len != 0) && b_tabs==0) ||
	   ((a_len == 0) && a_tabs+b_tabs <= 7)))
	{
	  if (b_len) {
	    if ((_XmEntryType(a_last) == XmSTRING_ENTRY_OPTIMIZED) &&
		_XmEntryImm(a_last)) {
	      unsigned int size = sizeof(_XmStringOptSegRec);
	      if (a_len + b_len > sizeof(XtPointer))
		size += a_len + b_len - sizeof(XtPointer);
	      if (a_line == (_XmStringEntry)a_last) {
		a_last = (_XmStringNREntry)XtRealloc((char *)a_last, size);
		_XmStrEntry(a_str)[a_lc - 1] = a_line = (_XmStringEntry)a_last;
	      } else
		_XmEntrySegmentGet(a_line)[a_sc-1] = a_last =
		  (_XmStringNREntry)XtRealloc((char *)a_last, size);
	    } else {
	      _XmEntryTextSet((_XmStringEntry)a_last,
			      XtRealloc((char *)
				       _XmEntryTextGet((_XmStringEntry)a_last),
				       a_len + b_len));
	    }
	  }
	  if (_XmEntryOptimized(a_last))
	    _XmEntryTagIndex(a_last) =
	      ((a_tag == NULL) ?
	       (b_tag == NULL ?
		TAG_INDEX_UNSET :
		_XmStringIndexCacheTag(b_tag, XmSTRING_TAG_STRLEN)) :
	       _XmEntryTagIndex(a_last));
	  else
	    _XmUnoptSegTag(a_last) = (a_tag == NULL) ? b_tag : a_tag;

	  /* Fixup rendition begins and ends */
	  if (a_len == 0)
	    MergeBegins((_XmStringEntry)a_last, (_XmStringEntry)b_seg);

	  if (_XmEntryRendEnds((_XmStringEntry)a_last) == NULL) {
	    if (_XmEntryOptimized(a_last) &&
		_XmEntryRendEndCountGet((_XmStringEntry)b_seg) > 0) {
	      b_tag = _XmEntryRendEndGet((_XmStringEntry)b_seg, 0);
	      _XmEntryRendIndex(a_last) =
		_XmStringIndexCacheTag(b_tag, XmSTRING_TAG_STRLEN);
	    }
	    if (!_XmEntryOptimized(a_last) &&
		(_XmEntryRendEndCountGet((_XmStringEntry)b_seg) != 0))
            {
	      if (_XmEntryOptimized(b_seg))
              {
		_XmUnoptSegRendEnds(a_last) =
		  (XmStringTag *)XtMalloc(sizeof(XmStringTag));
		_XmUnoptSegRendEnds(a_last)[0] =
		  _XmEntryRendEndGet((_XmStringEntry)b_seg, 0);
	      } else if (modify_b)
              {
		_XmUnoptSegRendEnds(a_last) = _XmUnoptSegRendEnds(b_seg);
	      } else
              {
		int end_count = _XmEntryRendEndCountGet((_XmStringEntry)b_seg);
		int k;
		_XmUnoptSegRendEnds(a_last) =
		  (XmStringTag *)XtMalloc(end_count * sizeof(XmStringTag));
		for (k = 0; k < end_count; k++)
		  _XmUnoptSegRendEnds(a_last)[k] =
		    _XmUnoptSegRendEnds(b_seg)[k];
	      }
          }
	    _XmEntryRendEndCountSet
	      (a_last, _XmEntryRendEndCountGet((_XmStringEntry)b_seg));
	  } else
	    MergeEnds((_XmStringEntry)a_last, (_XmStringEntry)b_seg);

	  _XmEntryTextTypeSet(a_last, (a_type == XmNO_TEXT) ? b_type : a_type);
	  memcpy(((char *)_XmEntryTextGet((_XmStringEntry)a_last)) + a_len,
		 _XmEntryTextGet((_XmStringEntry)b_seg), b_len);

	  _XmEntryByteCountSet(a_last, a_len + b_len);

	  _XmEntryTabsSet(a_last, a_tabs+b_tabs);
	  if (last == XmSTRING_DIRECTION_UNSET)
	    _XmEntryDirectionSet((_XmStringEntry)a_last,
				 _XmEntryDirectionGet((_XmStringEntry)b_seg));
	  _XmEntryPopSet(a_last, _XmEntryPopGet((_XmStringEntry)b_seg));

	  if (modify_b) {
	    /* Free leftover bits of b_seg */
	    if (_XmEntryUnoptimized(b_seg)) {
	      if (_XmEntryOptimized(a_last) ||
		  (_XmUnoptSegRendBegins(a_last) !=
		   _XmUnoptSegRendBegins(b_seg)))
		XtFree((char *)_XmUnoptSegRendBegins(b_seg));

	      if (_XmEntryOptimized(a_last) ||
		  (_XmUnoptSegRendEnds(a_last) != _XmUnoptSegRendEnds(b_seg)))
		XtFree((char *)_XmUnoptSegRendEnds(b_seg));
	    }
	    if ( ! ((_XmEntryType(b_seg) == XmSTRING_ENTRY_OPTIMIZED) &&
		    _XmEntryImm(b_seg)))
	      XtFree((char*)_XmEntryTextGet((_XmStringEntry)b_seg));
	    XtFree((char *)b_seg);
	  }

	  merged = 1;
	}
    }
  else /* Need to figure out last direction set. */
    {
      for (i = a_lc; i > 0; i--) {
	tmp_line = _XmStrEntryGet(a_str);
	if (!tmp_line) break;
	for (j = _XmEntrySegmentCountGet(tmp_line[i - 1]); j > 0; j--) {
	  tmp_seg = _XmEntrySegmentGet(tmp_line[i - 1])[j - 1];
	  if (_XmEntryDirectionGet((_XmStringEntry)tmp_seg) !=
	      XmSTRING_DIRECTION_UNSET) {
	    last = _XmEntryDirectionGet((_XmStringEntry)tmp_seg);
	    break;
	  }
	}
	if (last != XmSTRING_DIRECTION_UNSET) break;
      }
    }

  if (merged && !_XmStrImplicitLine(a_str))
    _XmStrEntryCount(a_str)--;

  if (b_sc - merged > 0 && _XmStrImplicitLine(a_str)) {
    Boolean free_b_line = (modify_b && _XmEntryMultiple(b_line) &&
			   ((_XmStringEntry)b_seg != b_line));

    if (_XmEntryMultiple(a_line)) {
      _XmEntrySegment(a_line) =
	(_XmStringNREntry *)XtRealloc((char *)_XmEntrySegment(a_line),
				      sizeof(_XmStringNREntry) *
				      (a_sc + b_sc - merged));
      _XmEntrySegmentCount(a_line) = a_sc + b_sc - merged;
    } else {
      _XmEntryCreate(a_line, XmSTRING_ENTRY_ARRAY);
      _XmEntrySegmentCount(a_line) = a_sc + b_sc - merged;
      _XmEntrySegment(a_line) =
	(_XmStringNREntry *)XtMalloc(sizeof(_XmStringNREntry) *
				     (a_sc + b_sc - merged));
      _XmEntrySegment(a_line)[0] =
	(_XmStringNREntry)_XmStrEntry(a_str)[a_lc - 1];
      _XmStrEntry(a_str)[a_lc - 1] = a_line;
      _XmStrImplicitLine(a_str) = True;
    }

    for (i = 0; i < (b_sc - merged); i++)
      {
	b_seg = _XmEntrySegmentGet(b_line)[i + merged];
	if (a_needs_unopt && !b_needs_unopt)
	  b_seg = (_XmStringNREntry)Unoptimize((_XmStringEntry)b_seg,modify_b);
	else if (!modify_b)
	  b_seg = (_XmStringNREntry)_XmStringEntryCopy((_XmStringEntry)b_seg);

	if (_XmEntryDirectionGet((_XmStringEntry)b_seg) ==
	    XmSTRING_DIRECTION_UNSET)
	  _XmEntryDirectionSet((_XmStringEntry)b_seg, last);
	else last = _XmEntryDirectionGet((_XmStringEntry)b_seg);
	_XmEntrySegment(a_line)[i + a_sc] = b_seg;
      }

    if (free_b_line) {
      if (_XmEntrySegment(b_line) != (_XmStringNREntry *)&b &&
	  _XmEntrySegment(b_line) != (_XmStringNREntry *)_XmStrEntry(b_str))
	XtFree((char *)_XmEntrySegment(b_line));
      if (b_line != (_XmStringEntry)&array_seg) XtFree((char *)b_line);
    }
  } else if (b_sc - merged > 0 && !_XmStrImplicitLine(a_str)) {
    for (i = 0; i < (b_sc - merged); i++)
      {
	b_line = _XmStrEntry(b_str)[i + merged];
	if (a_needs_unopt && !b_needs_unopt)
	  _XmStrEntry(a_str)[a_lc] = Unoptimize(b_line, modify_b);
	else if (modify_b)
	  _XmStrEntry(a_str)[a_lc] = b_line;
	else
	  _XmStrEntry(a_str)[a_lc] = _XmStringEntryCopy(b_line);
	a_lc++;
      }
  }

  /* Add rest of b's lines to a */
  for (i = 0; i < (b_lc - 1); i++)
    {
      b_line = _XmStrEntry(b_str)[i + 1];
      if (a_needs_unopt && !b_needs_unopt)
	b_line = Unoptimize(b_line, modify_b);
      else if (!modify_b)
	b_line = _XmStringEntryCopy(b_line);

      b_sc = _XmEntrySegmentCountGet(b_line);

      for (j = 0; j < b_sc; j++)
	{
	  b_seg = _XmEntrySegmentGet(b_line)[j];

	  if (_XmEntryDirectionGet((_XmStringEntry)b_seg) ==
	      XmSTRING_DIRECTION_UNSET)
	    _XmEntryDirectionSet((_XmStringEntry)b_seg, last);
	  else break;
	}

      _XmStrEntry(a_str)[i + a_lc] = b_line;
    }

  if (modify_b && free_b) {
    /* Free leftover bits of b_str. */
    XtFree((char *)_XmStrEntry(b_str));
    _XmStrFree ((char *)b_str);
  }

  /* Set layout cache dirty */
  if (a_str && _XmStrEntryCount(a_str) > 0 ) {
    tmp_line = _XmStrEntryGet(a_str);
    if (tmp_line && *tmp_line && _XmEntrySegmentCountGet(*tmp_line) > 0) {
      _XmStringCache cache;

      tmp_seg = _XmEntrySegmentGet(*tmp_line)[0];
      for (cache = _XmStringCacheGet(_XmEntryCacheGet((_XmStringEntry)tmp_seg),
				     _XmSCANNING_CACHE);
	   cache;
	   cache = _XmStringCacheGet(cache->next, _XmSCANNING_CACHE))
	_XmCacheDirty(cache) = True;
    }
  }

  if (!modify_a)
    XmStringFree(a);
  if (!modify_b)
    XmStringFree(b);

  _XmProcessUnlock();
  return (XmString)a_str;
}

/************************************************************************
 *									*
 * XmStringCompare - compare two strings.  				*
 *									*
 * Returns TRUE if the strings are equal, FALSE o.w.			*
 *									*
 ************************************************************************/
Boolean
XmStringCompare(
        XmString a,
        XmString b )
{
  _XmProcessLock();
  if ((a == NULL) && (b == NULL)) {
	_XmProcessUnlock();
	return TRUE;
  }
  if ((a == NULL) || (b == NULL)) {
	_XmProcessUnlock();
	return FALSE;
  }

  if (_XmStrOptimized(a) && _XmStrOptimized(b)) {
    if (!((_XmStrTagGet(a) == _XmStrTagGet(b)) ||
	  (_XmStrTagGet(a) == NULL) ||
	  (_XmStrTagGet(b) == NULL) ||
	  ((strcmp(_XmStringIndexGetTag(_XmStrTagIndex(a)), XmFONTLIST_DEFAULT_TAG) == 0) &&
	   _XmStringIsCurrentCharset(_XmStrTagGet(b))) ||
	  ((strcmp(_XmStringIndexGetTag(_XmStrTagIndex(b)), XmFONTLIST_DEFAULT_TAG) == 0) &&
	   _XmStringIsCurrentCharset(_XmStrTagGet(a))))) {
      _XmProcessUnlock();
      return (FALSE);
    }
    if (_XmStrByteCount(a) != _XmStrByteCount(b)) {
      _XmProcessUnlock();
      return (FALSE);
    }
    if ((_XmStrDirection(a) != _XmStrDirection(b)) &&
	(((_XmStrDirection(a) == XmSTRING_DIRECTION_UNSET) &&
	  (_XmStrDirection(b) != XmSTRING_DIRECTION_L_TO_R)) ||
	 ((_XmStrDirection(b) == XmSTRING_DIRECTION_UNSET) &&
	  (_XmStrDirection(a) != XmSTRING_DIRECTION_L_TO_R)))) {
      _XmProcessUnlock();
      return (FALSE);
    }
    if (strncmp(_XmStrText(a), _XmStrText(b), _XmStrByteCount(a)) != 0) {
      _XmProcessUnlock();
      return (FALSE);
    }
  } else {
    int i, j;
    _XmStringEntry *entry_a;
    _XmStringEntry *entry_b;
    XmString a_unopt = NULL;
    XmString b_unopt = NULL;

    if (_XmStrEntryCountGet(a) != _XmStrEntryCountGet(b)) {
      _XmProcessUnlock();
      return (FALSE);
    }

    if (_XmStrOptimized(a)) {
      a_unopt = _XmStringOptToNonOpt((_XmStringOpt)a);
      entry_a = _XmStrEntry(a_unopt);
    } else {
      entry_a = _XmStrEntry(a);
    }

    if (_XmStrOptimized(b)) {
      b_unopt = _XmStringOptToNonOpt((_XmStringOpt)b);
      entry_b = _XmStrEntry(b_unopt);
    } else {
      entry_b = _XmStrEntry(b);
    }

    for (i = 0; i < _XmStrEntryCountGet(a); i++) {
      if (_XmEntryMultiple(entry_a[i]) && _XmEntryMultiple(entry_b[i])) {
	if (_XmEntrySegmentCount(entry_a[i]) !=
	    _XmEntrySegmentCount(entry_b[i])) {
	  if (a_unopt) XmStringFree(a_unopt);
	  if (b_unopt) XmStringFree(b_unopt);
	  _XmProcessUnlock();
	  return (FALSE);
	}

	for (j=0; j<_XmEntrySegmentCount(entry_a[i]); j++) {
	  _XmStringNREntry a_seg = _XmEntrySegment(entry_a[i])[j];
	  _XmStringNREntry b_seg = _XmEntrySegment(entry_b[i])[j];
	  unsigned int len;
	  XmStringTag a_tag = _XmEntryTag((_XmStringEntry)a_seg);
	  XmStringTag b_tag = _XmEntryTag((_XmStringEntry)b_seg);

	  if (!((a_tag == b_tag) ||
		(a_tag == NULL) ||
		(b_tag == NULL) ||
		((strcmp(a_tag, XmFONTLIST_DEFAULT_TAG) == 0) &&
		 _XmStringIsCurrentCharset(b_tag)) ||
		((strcmp(b_tag, XmFONTLIST_DEFAULT_TAG) == 0) &&
		 _XmStringIsCurrentCharset(a_tag)))) {
	        if (a_unopt) XmStringFree(a_unopt);
	        if (b_unopt) XmStringFree(b_unopt);
	        _XmProcessUnlock();
		return (FALSE);
	  }

	  len = _XmEntryByteCountGet((_XmStringEntry)a_seg);
	  if (len != _XmEntryByteCountGet((_XmStringEntry)b_seg)) {
	    if (a_unopt) XmStringFree(a_unopt);
	    if (b_unopt) XmStringFree(b_unopt);
	    _XmProcessUnlock();
	    return (FALSE);
	  }

	  {
	    unsigned int a_dir = _XmEntryDirectionGet((_XmStringEntry)a_seg);
	    unsigned int b_dir = _XmEntryDirectionGet((_XmStringEntry)b_seg);
	    if ((a_dir != b_dir) &&
		(((a_dir == XmSTRING_DIRECTION_UNSET) &&
		  (b_dir != XmSTRING_DIRECTION_L_TO_R)) ||
		 ((b_dir == XmSTRING_DIRECTION_UNSET) &&
		  (a_dir != XmSTRING_DIRECTION_L_TO_R)))) {
	      if (a_unopt) XmStringFree(a_unopt);
	      if (b_unopt) XmStringFree(b_unopt);
	      _XmProcessUnlock();
	      return (FALSE);
	    }
	  }

	  if (strncmp ((char*)_XmEntryTextGet((_XmStringEntry)a_seg),
		       (char*)_XmEntryTextGet((_XmStringEntry)b_seg),
		       len) != 0) {
	    if (a_unopt) XmStringFree(a_unopt);
	    if (b_unopt) XmStringFree(b_unopt);
	    _XmProcessUnlock();
	    return (FALSE);
	  }
	}
      } else if (!_XmEntryMultiple(entry_a[i]) &&
		 !_XmEntryMultiple(entry_b[i])) {
	unsigned int len;

	if (!((_XmEntryTag(entry_a[i]) == _XmEntryTag(entry_b[i])) ||
	      (_XmEntryTag(entry_a[i]) == NULL) ||
	      (_XmEntryTag(entry_b[i]) == NULL) ||
	      ((strcmp(_XmEntryTag(entry_a[i]),
		       XmFONTLIST_DEFAULT_TAG) == 0) &&
	       _XmStringIsCurrentCharset(_XmEntryTag(entry_b[i]))) ||
	      ((strcmp(_XmEntryTag(entry_b[i]),
		       XmFONTLIST_DEFAULT_TAG) == 0) &&
	       _XmStringIsCurrentCharset(_XmEntryTag(entry_a[i])))))
	{
	  if (a_unopt) XmStringFree(a_unopt);
	  if (b_unopt) XmStringFree(b_unopt);
	  _XmProcessUnlock();
	  return (FALSE);
	}

	len = _XmEntryByteCountGet(entry_a[i]);
	if (len != _XmEntryByteCountGet(entry_b[i])) {
	  if (a_unopt) XmStringFree(a_unopt);
	  if (b_unopt) XmStringFree(b_unopt);
	  _XmProcessUnlock();
	  return (FALSE);
	}

	if ((_XmEntryDirectionGet(entry_a[i])  !=
	     _XmEntryDirectionGet(entry_b[i]))         &&
	    (((_XmEntryDirectionGet(entry_a[i]) ==
	       XmSTRING_DIRECTION_UNSET) &&
	      (_XmEntryDirectionGet(entry_b[i]) !=
	       XmSTRING_DIRECTION_L_TO_R))           ||
	     ((_XmEntryDirectionGet(entry_b[i]) ==
	       XmSTRING_DIRECTION_UNSET) &&
	      (_XmEntryDirectionGet(entry_a[i]) !=
	       XmSTRING_DIRECTION_L_TO_R)))) {
	  if (a_unopt) XmStringFree(a_unopt);
	  if (b_unopt) XmStringFree(b_unopt);
	  _XmProcessUnlock();
	  return (FALSE);
	}

	if (strncmp ((char*) _XmEntryTextGet(entry_a[i]),
		     (char*) _XmEntryTextGet(entry_b[i]),
		     len) != 0) {
	  if (a_unopt) XmStringFree(a_unopt);
	  if (b_unopt) XmStringFree(b_unopt);
	  _XmProcessUnlock();
	  return (FALSE);
	}
      } else {
        if (a_unopt) XmStringFree(a_unopt);
	if (b_unopt) XmStringFree(b_unopt);
	_XmProcessUnlock();
	return (FALSE);
      }
    }
    if (a_unopt) XmStringFree(a_unopt);
    if (b_unopt) XmStringFree(b_unopt);
  }
  _XmProcessUnlock();
  return (TRUE);
}

int
XmStringLength(
        XmString string )
{
  unsigned int	len;

  if (!string) return (0);
  if (!XmeStringIsValid(string)) return (0);

  len = XmCvtXmStringToByteStream(string, NULL);

  return((int)len);
}

/************************************************************************
 *                                                                      *
 * XmeStringIsXmString - returns TRUE if the parameter is an XmString.   *
 *                                                                      *
 ************************************************************************/
Boolean
XmeStringIsValid(
        XmString string )
{
  if (string == NULL) return(FALSE);
  return(TRUE);
}

/*
 * determines from ASN.1 header whether this is an ASN.1 conformant
 * external compound string.  Returns T or F.
 */
static Boolean
_is_asn1( unsigned char *string )
{
  unsigned char *uchar_p = string;

  /*  Compare the ASN.1 header. */
  return (strncmp ((char *)uchar_p, (char *)ASNHeader, ASNHEADERLEN) == 0);
}

/*
 * optimized internal TCS structure handling routines
 */
/*
 * find the ascender for the given optimized line
 */
static Dimension
OptLineAscender(
        XmRenderTable f,
        _XmStringOpt opt )
{
  Dimension width, height, ascent, descent;

  OptLineMetrics(f, (_XmString)opt, NULL, NULL,
		 &width, &height, &ascent, &descent);

  return(ascent);
}

int
_XmConvertFactor(unsigned char units,
	      float *factor)
{
  switch (units)
    {
    case XmINCHES:
      *factor = 1000.0;
      return(Xm1000TH_INCHES);
    case XmCENTIMETERS:
      *factor = 1000.0;
      return(Xm100TH_MILLIMETERS);
    case XmMILLIMETERS:
      *factor = 100.0;
      return(Xm100TH_MILLIMETERS);
    case XmPOINTS:
      *factor = 100.0;
      return(Xm100TH_POINTS);
    case XmFONT_UNITS:
      *factor = 100.0;
      return(Xm100TH_FONT_UNITS);
    default:
      *factor = 1.0;
      return(units);
    }
}

static int
TabVal(Display *d,
       Screen **pscreen,
       Window w,
       XmTab tab)
{
  int	fromType;
  int	intValue;
  float	multiplier, convertValue;

  fromType = _XmConvertFactor(_XmTabUnits(tab), &multiplier);

  convertValue = multiplier * _XmTabValue(tab);

  /* error */
  if (((convertValue < 0.0) ? -convertValue : convertValue) > (float)INT_MAX)
    return(0);

  convertValue += (convertValue > 0.0) ? 0.5 : -0.5;
  intValue = convertValue;

  /*
   * The pscreen storage should be pushed higher; we may still make
   * several round trips to the server to draw a single string???
   */
  /* All we really want is the screen, but we may only have a drawable. */
  assert(w || *pscreen);
  if (*pscreen == NULL)
    {
      Widget widget = XtWindowToWidget(d, w);

      /* If this drawable is really a widget Xt will have cached it. */
      if (widget)
	*pscreen = XtScreenOfObject(widget);
      else
	{
	  /* Give up and ask the server. */
	  XWindowAttributes attr;
	  XGetWindowAttributes(d, w, &attr);
	  *pscreen = attr.screen;
	}
    }

  return _XmConvertUnits(*pscreen, XmHORIZONTAL, fromType, intValue, XmPIXELS);
}


/*
 * Find width, height, ascent and descent for the given optimized line.
 */
static void
OptLineMetrics(XmRenderTable 	r,
	       _XmString 	opt,
	       XmRendition *rend_io,
	       XmRendition base_rend,
	       Dimension *width,
	       Dimension *height,
	       Dimension *ascent,
	       Dimension *descent)
{
  short	                rend_index;
  XmRendition		rend = NULL;
  XmStringTag		tags[1];
  Display              *d;
  Screen	       *screen;
  int			prev_val, val, i, ref_cnt, rt_ref_cnt;
  XmTabList		tl = NULL;
  XmTab			tab;
  unsigned short	tab_cnt;
  Dimension             tab_w = 0;
  _XmRendition		rend_int;

  /* compute rendition */
  /* Find font as per I 198. */
  /* 1. Find font from rendition tags. */
  /* 2. Find font from locale/charset tag. */
  if (base_rend == NULL)
    {
      if (_XmStrRendBegin(opt))
	rend = _XmRenderTableFindRendition(r, _XmStrRendTagGet(opt),
					   TRUE, FALSE, TRUE, &rend_index);

      if ((rend == NULL) ||
          (_XmRendFont(rend) == NULL && _XmRendXftFont(rend) == NULL))
	rend = _XmRenderTableFindRendition(r, _XmStrTagGet(opt),
					   TRUE, FALSE, TRUE, &rend_index);
    }
  else
    {
      if (_XmStrRendBegin(opt))
	{
	  tags[0] = _XmStrRendTagGet(opt);

	  rend = _XmRenditionMerge(_XmRendDisplay(base_rend), rend_io,
				   base_rend, r, _XmStrTagGet(opt), tags, 1,
				   FALSE);
	}
      else
	{
	  rend = _XmRenditionMerge(_XmRendDisplay(base_rend), rend_io,
				   base_rend, r, _XmStrTagGet(opt), NULL, 0,
				   FALSE);
	}
    }

  /* 3. Default rendition. */
  if ((rend == NULL) ||
      (_XmRendFont(rend) == NULL && _XmRendXftFont(rend) == NULL))
    {
      tags[0] = ((_XmStrTextType(opt) == XmCHARSET_TEXT) ?
		 XmFONTLIST_DEFAULT_TAG :
		 _MOTIF_DEFAULT_LOCALE);

      rend = _XmRenderTableFindRendition(r, tags[0],
					 TRUE, FALSE, FALSE, NULL);
      if ((rend != NULL) &&
          (_XmRendFont(rend) == NULL) && (_XmRendXftFont(rend) == NULL) &&
	  (((base_rend != NULL) && (_XmRendDisplay(base_rend) != NULL)) ||
	   (_XmRendDisplay(rend) != NULL)))
	/* Call noFontCallback. */
	{
	  XmDisplay			dsp;
	  XmDisplayCallbackStruct	cb;

	  rt_ref_cnt = _XmRTRefcount(r);
	  rend = _XmRTRenditions(r)[0];
	  rend_int = *rend;
	  ref_cnt = _XmRendRefcount(rend);

	  if ((base_rend != NULL) && (_XmRendDisplay(base_rend) != NULL))
	    dsp = (XmDisplay)XmGetXmDisplay(_XmRendDisplay(base_rend));
	  else dsp = (XmDisplay)XmGetXmDisplay(_XmRendDisplay(rend));

	  cb.reason = XmCR_NO_FONT;
	  cb.event = NULL;
	  cb.rendition = rend;
	  cb.font_name = XmS;

	  XtCallCallbackList((Widget)dsp, dsp->display.noFontCallback, &cb);

	  if (rend_int != *rend)		  /* Changed in callback. */
	    {
	      /* Need to split ref counts. */
	      _XmRendRefcount(&rend_int) = ref_cnt - rt_ref_cnt;
	      _XmRendRefcount(rend) = rt_ref_cnt;
	    }

	  if (_XmRendFont(rend) == NULL && _XmRendXftFont(rend) == NULL)
	    rend = NULL;
	}

      /* 4a. Take the first one */
      if ((rend == NULL) &&
	  ((_XmStrTextType(opt) == XmCHARSET_TEXT) ||
	    ((_XmStrTextType(opt) == XmMULTIBYTE_TEXT) &&
	     (_XmStrTagGet(opt) == XmFONTLIST_DEFAULT_TAG))) &&
	  (r != NULL) && (_XmRTCount(r) > 0))
	_XmRenderTableFindFirstFont(r, &rend_index, &rend);

      if ((rend != NULL) &&
          (_XmRendFont(rend) == NULL && _XmRendXftFont(rend) == NULL) &&
	  (((base_rend != NULL) && (_XmRendDisplay(base_rend) != NULL)) ||
	   (_XmRendDisplay(rend) != NULL)))
	/* Call noFontCallback. */
	{
	  XmDisplay			dsp;
	  XmDisplayCallbackStruct	cb;

	  rt_ref_cnt = _XmRTRefcount(r);
	  rend = _XmRTRenditions(r)[0];
	  rend_int = *rend;
	  ref_cnt = _XmRendRefcount(rend);

	  if ((base_rend != NULL) && (_XmRendDisplay(base_rend) != NULL))
	    dsp = (XmDisplay)XmGetXmDisplay(_XmRendDisplay(base_rend));
	  else dsp = (XmDisplay)XmGetXmDisplay(_XmRendDisplay(rend));

	  cb.reason = XmCR_NO_FONT;
	  cb.event = NULL;
	  cb.rendition = rend;
	  cb.font_name = XmS;

	  XtCallCallbackList((Widget)dsp, dsp->display.noFontCallback, &cb);

	  if (rend_int != *rend)		  /* Changed in callback. */
	    {
	      /* Need to split ref counts. */
	      _XmRendRefcount(&rend_int) = ref_cnt - rt_ref_cnt;
	      _XmRendRefcount(rend) = rt_ref_cnt;
	    }

	  if (_XmRendFont(rend) == NULL && _XmRendXftFont(rend) == NULL)
	    rend = NULL;
	}

      /* 4b/5a. Emit warning and don't render. */
      if ((rend == NULL) ||
          (_XmRendFont(rend) == NULL && _XmRendXftFont(rend) == NULL))
	{
	  /* Don't emit warning if no tags, e.g. just a dir component. */
	  if (_XmStrRendBegin(opt) ||
	      (_XmStrTagGet(opt) != NULL))
	    XmeWarning(NULL, NO_FONT_MSG);
	  if ((base_rend != NULL) && (rend_io == NULL))
	    XmRenditionFree(rend);
	  rend = NULL;
	  return;
	}
      else if (rend_io != NULL)
	{
	  _XmRendFont(*rend_io) = _XmRendFont(rend);
#if USE_XFT
	  _XmRendXftFont(*rend_io) = _XmRendXftFont(rend);
#endif
	  _XmRendFontName(*rend_io) = _XmRendFontName(rend);
	  _XmRendFontType(*rend_io) = _XmRendFontType(rend);
	}
    }

  /* Use the raster extent for a single line. */
  if (rend != NULL)
    ComputeMetrics(rend,
		   (XtPointer)_XmStrText(opt),
		   _XmStrByteCount(opt), (XmTextType) _XmStrTextType(opt),
		   XmSTRING_SINGLE_SEG, width, height, ascent, descent,
#if XM_UTF8
                   (_XmStrTextType(opt) == XmCHARSET_TEXT ||
                    _XmStrTextType(opt) == XmMULTIBYTE_TEXT) &&
                   ((_XmStrTagGet(opt) == XmFONTLIST_DEFAULT_TAG
                     && _XmStringIsCurrentCharset("UTF-8"))
                    || (_XmStrTagGet(opt)
		     && strcmp(_XmStringIndexGetTag(_XmStrTagIndex(opt)), "UTF-8") == 0))
#else
                   False
#endif
		    );

  if (rend != NULL) tl = _XmRendTabs(rend);
  d = (_XmRTDisplay(r) == NULL) ? _XmGetDefaultDisplay() : _XmRTDisplay(r);
  screen = XtScreenOfObject(XmGetXmDisplay(d));

  tab = ((tl == NULL) || ((long)tl == XmAS_IS)) ? NULL : _XmTabLStart(tl);

  prev_val = 0;
  tab_cnt = 0;

  /* If this string is tabbed, set width accordingly. */
  if ((tab != NULL) &&
      (_XmStrTabs(opt) != 0) &&
      (tab_cnt < _XmTabLCount(tl)))
    {
      for (i = 0;
	   (i < _XmStrTabs(opt)) && (tab_cnt < _XmTabLCount(tl));
	   tab = _XmTabNext(tab), tab_cnt++, i++)
	{
	  val = TabVal(d, &screen, None, tab);
	  if (_XmTabModel(tab) == XmABSOLUTE)
	    {
	      tab_w = val;
	      prev_val = val;
	    }
	  else				  /* XmRELATIVE */
	    {
	      tab_w = prev_val + val;
	      prev_val += val;
	    }
	}
    }

  (*width) += tab_w;
  if ((base_rend != NULL) && (rend_io == NULL)) XmRenditionFree(rend);
}

/*
 * internal TCS structure handling routines
 */


/*
 * find biggest ascender and descender and width and height in this line
 */
static void
LineMetrics(_XmStringEntry line,
	    XmRenderTable r,
	    XmRendition *rend_io,
	    XmRendition base,
	    XmDirection prim_dir,
	    Dimension *width,
	    Dimension *height,
	    Dimension *ascender,
	    Dimension *descender)
{
  int 			i, seg_index = 0;
  Dimension w, tab_w = 0, h, max_h = 0, asc, max_asc = 0, dsc, max_dsc = 0;
  Display		*d;
  Screen		*screen;
  int			prev_val, val;
  XmTabList		tl = NULL;
  XmTab			tab;
  unsigned short	tab_cnt;
  _XmStringNREntry	seg, peek_seg;
  XmDirection 		lay_dir = 0;
  Boolean		set_direction = FALSE;

  d = _XmRendDisplay(*rend_io);
  screen = XtScreenOfObject(XmGetXmDisplay(d));

  seg = _XmEntrySegmentGet(line)[seg_index];

  if (_XmEntryType(seg) != XmSTRING_ENTRY_OPTIMIZED)
    {
      lay_dir = _XmEntryLayoutGet(seg, prim_dir);

      if (XmDirectionMatch(lay_dir, XmLEFT_TO_RIGHT))
	{
	  while (_XmEntryLeftGet(seg, prim_dir) != NULL)
	    seg = (_XmStringNREntry)_XmEntryLeftGet(seg, prim_dir);
	  peek_seg = (_XmStringNREntry)_XmEntryRightGet(seg, prim_dir);
	}
      else
	{
	  while (_XmEntryRightGet(seg, prim_dir) != NULL)
	    seg = (_XmStringNREntry)_XmEntryRightGet(seg, prim_dir);
	  peek_seg = (_XmStringNREntry)_XmEntryLeftGet(seg, prim_dir);
	}
    }
  else
    {
      peek_seg = ((seg_index + 1) < _XmEntrySegmentCountGet(line) ?
		  _XmEntrySegmentGet(line)[seg_index + 1] :
		  NULL);
    }

  if (_XmEntryDirectionGet((_XmStringEntry)seg) == XmSTRING_DIRECTION_UNSET)
    {
      _XmEntryDirectionSet((_XmStringEntry)seg,
			   XmDirectionToStringDirection(prim_dir));
      set_direction = True;
    }

  if (peek_seg != NULL)
    (void)SpecifiedSegmentExtents((_XmStringEntry)seg, r, rend_io, base,
				  XmSTRING_FIRST_SEG, &w, &h, &asc, &dsc);
  else (void)SpecifiedSegmentExtents((_XmStringEntry)seg, r, rend_io, base,
				     XmSTRING_SINGLE_SEG, &w, &h, &asc, &dsc);

  if (*rend_io != NULL) tl = _XmRendTabs(*rend_io);

  tab = ((tl == NULL) || ((long)tl == XmAS_IS)) ? NULL : _XmTabLStart(tl);

  prev_val = 0;
  tab_cnt = 0;

  while (seg != NULL)
    {
      /* If this segment is tabbed, set width accordingly. */
	if ((tab != NULL) &&
	    (_XmEntryTabsGet((_XmStringEntry)seg) != 0) &&
	    (tab_cnt < _XmTabLCount(tl)))
	  {
	    for (i = 0;
		 (i < _XmEntryTabsGet((_XmStringEntry)seg)) &&
		 (tab_cnt < _XmTabLCount(tl));
		 i++, tab = _XmTabNext(tab), tab_cnt++)
	      {
		val = TabVal(d, &screen, None, tab);
		if (_XmTabModel(tab) == XmABSOLUTE)
		  tab_w = MAX(tab_w, val);
		else				  /* XmRELATIVE */
		  tab_w = MAX(tab_w, prev_val + val);

		prev_val = tab_w;
	      }
	  }

	tab_w += w;
	if (h > max_h) max_h = h;
	if (asc > max_asc) max_asc = asc;
	if (dsc >max_dsc) max_dsc = dsc;

	if (set_direction)
	  {
	    _XmEntryDirectionSet((_XmStringEntry)seg,
				 XmSTRING_DIRECTION_UNSET);
	    set_direction = False;
	  }

	if (_XmEntryType(seg) != XmSTRING_ENTRY_OPTIMIZED)
	  {
	    if (XmDirectionMatch(lay_dir, XmLEFT_TO_RIGHT))
	      {
		seg = (_XmStringNREntry)_XmEntryRightGet(seg, prim_dir);
		peek_seg = (_XmStringNREntry)_XmEntryRightGet(seg, prim_dir);
	      }
	    else
	      {
		seg = (_XmStringNREntry)_XmEntryLeftGet(seg, prim_dir);
		peek_seg = (_XmStringNREntry)_XmEntryLeftGet(seg, prim_dir);
	      }
	  }
	else {
	  seg_index++;
	  seg = (seg_index < _XmEntrySegmentCountGet(line) ?
		 _XmEntrySegmentGet(line)[seg_index] :
		 NULL);
	  peek_seg = ((seg_index + 1) < _XmEntrySegmentCountGet(line) ?
		      _XmEntrySegmentGet(line)[seg_index + 1] :
		      NULL);
	}
	if (seg != NULL)
	  {
	    if (_XmEntryDirectionGet((_XmStringEntry)seg) ==
		XmSTRING_DIRECTION_UNSET)
	      {
		_XmEntryDirectionSet((_XmStringEntry)seg,
				     XmDirectionToStringDirection(prim_dir));
		set_direction = True;
	      }
	    if (peek_seg != NULL)
	      (void)SpecifiedSegmentExtents((_XmStringEntry)seg, r, rend_io, base,
					    XmSTRING_MIDDLE_SEG,
					    &w, &h, &asc, &dsc);
	    else (void)SpecifiedSegmentExtents((_XmStringEntry)seg, r, rend_io, base,
					       XmSTRING_LAST_SEG,
					       &w, &h, &asc, &dsc);
	  }
      }

  *width = tab_w;
  if (max_h > 0) *height = max_h;
  if (max_asc > 0) *ascender = max_asc;
  if (max_dsc > 0) *descender = max_dsc;
}

static XFontStruct *
GetFont(XmRenderTable rt,
	_XmStringEntry entry)
{
  XmRendition rend = _XmEntryRenditionGet(entry, rt);
  short	 indx = -1;
  Cardinal	n;
  Arg		args[2];
  XmFontType	type;
  XtPointer	font;

  if (rend == NULL)
    (void)_XmRenderTableFindFallback(rt, _XmEntryTag(entry), TRUE, &indx, &rend);

  if (rend != NULL) {
    n = 0;
    XtSetArg(args[n], XmNfontType, &type); n++;
    XtSetArg(args[n], XmNfont, &font); n++;
    XmRenditionRetrieve(rend, args, n);

    if (type == XmFONT_IS_FONT)
      return (XFontStruct *)font;
    else
      return (XFontStruct *)NULL;
  }
  return (XFontStruct *)NULL;
}


unsigned char
_XmStringCharacterCount(XtPointer text,
			XmTextType text_type,
			int byte_count,
			XFontStruct *font)
{
  if (text == NULL)
    return 0;
  if (byte_count == 0)
    byte_count = strlen((char *)text);

  switch (text_type)
    {
    case XmCHARSET_TEXT:
      {
	if (font && two_byte_font(font))
	  return (byte_count/2);
	else
	  return byte_count;
      }
    case XmMULTIBYTE_TEXT:
      {
	char *s = (char *) text;
	if (MB_CUR_MAX == 1)
	  return byte_count;
	else {
	  int cnt = 0;
	  int len;
	  while (byte_count > 0 && (len = mblen(s, MB_CUR_MAX)) > 0) {
	    cnt++;
	    s += len;
	    byte_count -= len;
	  }
	  return cnt;
	}
      }
    case XmWIDECHAR_TEXT:
      {
	wchar_t *wcs = (wchar_t *)text;
	int cnt = 0;
	while (byte_count > 0 && wcs[cnt]) {
	  cnt++;
	  byte_count -= sizeof(wchar_t);
	}
	return cnt;
      }
    default:
      return byte_count;
    }
}

unsigned char
_XmEntryCharCountGet(_XmStringEntry entry,
		     XmRenderTable rt)
{
  unsigned int len;

  if (_XmEntryOptimized(entry)) {
    if ((len = _XmEntryByteCountGet(entry)) == 0) {
      return 0;
    } else {
      return _XmStringCharacterCount((char *)_XmEntryTextGet(entry),
				     (XmTextType) _XmEntryTextTypeGet(entry),
				     len,
				     GetFont(rt, entry));
    }
  }
  if (_XmEntryUnoptimized(entry))  {
    if (((_XmStringUnoptSeg)entry)->char_count == 0 &&
	(len = _XmEntryByteCountGet(entry)) != 0) {
      ((_XmStringUnoptSeg)entry)->char_count =
	_XmStringCharacterCount((char *)_XmEntryTextGet(entry),
				(XmTextType) _XmEntryTextTypeGet(entry),
				len,
				GetFont(rt, entry));
    }
    return ((_XmStringUnoptSeg)entry)->char_count;
  }
  return(0);
}


_XmStringCache
_XmStringCacheGet(_XmStringCache caches,
		  int type)
{
  _XmStringCache cache = caches;

  while (cache && cache->cache_type != type)
    cache = cache->next;

  return cache;
}


void
_XmStringCacheFree(_XmStringCache caches)
{
  _XmStringCache prev = NULL, current = caches;

  while (current) {
    prev = current;
    current = current->next;
    if (prev)
      {
	if (prev->cache_type == _XmRENDERING_CACHE &&
	    ((_XmStringRenderingCache)prev)->rendition != NULL)
	  XmRenditionFree(((_XmStringRenderingCache)prev)->rendition);

	XtFree((char *)prev);
      }
  }
}


static _XmStringCache
CacheGet(_XmStringEntry entry,
         int type,
         int create,
         XtPointer match_value)
{
  _XmStringCache cache;

  if (!entry || !_XmEntryUnoptimized(entry))
    return NULL;

  switch (type)
    {
    case _XmSCANNING_CACHE:
      {
        XmDirection  d;

	d = (XmDirection)(long)match_value;

        if (d) {
          cache = _XmEntryCacheGet(entry);
          while (cache &&
                 !(cache->cache_type == type &&
                   (XmDirectionMatch(((_XmStringScanningCache)cache)->prim_dir,
                                     d))))
            cache = cache->next;
          if (!cache && create) {
            cache = (_XmStringCache)XtCalloc(1, sizeof(_XmStringScanningRec));
            cache->cache_type = type;
            cache->dirty = True;
            cache->next = _XmEntryCacheGet(entry);
            _XmEntryCacheSet(entry, cache);
            ((_XmStringScanningCache)cache)->prim_dir = d;
          }
        } else
          cache = NULL;
      }
      break;
    case _XmRENDERING_CACHE:
      {
        XmRenderTable rt;

	rt = (XmRenderTable)match_value;

        if (rt) {
          cache = _XmEntryCacheGet(entry);
          while (cache &&
                 !(cache->cache_type == type &&
                   ((_XmStringRenderingCache)cache)->rt == rt))
            cache = cache->next;
          if (!cache && create) {
            cache = (_XmStringCache)XtCalloc(1, sizeof(_XmStringRenderingRec));
            cache->cache_type = type;
            cache->dirty = True;
            cache->next = _XmEntryCacheGet(entry);
            _XmEntryCacheSet(entry, cache);
            ((_XmStringRenderingCache)cache)->rt = rt;
          }
        } else
          cache = NULL;
      }
      break;
    default:
      cache = NULL;
      break;
    }
  return cache;
}


XtPointer
_XmScanningCacheGet(_XmStringNREntry entry,
		    XmDirection d,
		    int field)
{
  _XmStringScanningCache cache;

  cache = (_XmStringScanningCache)CacheGet((_XmStringEntry)entry,
					   _XmSCANNING_CACHE, False,
					   (XtPointer)(long)d);
  if (!cache)
    {
      if (entry && _XmEntryUnoptimized(entry) && (field == _XmCACHE_DIRTY))
	return (XtPointer)True;
      else
	return NULL;
    }

  switch (field)
    {
    case _XmCACHE_DIRTY:
      return (XtPointer)(long)cache->header.dirty;
    case _XmCACHE_SCAN_LEFT:
      return (XtPointer)cache->left;
    case _XmCACHE_SCAN_RIGHT:
      return (XtPointer)cache->right;
    case _XmCACHE_SCAN_LAYOUT:
      return (XtPointer)(long)cache->layout_direction;
    case _XmCACHE_SCAN_DEPTH:
      return (XtPointer)(long)cache->depth;
    default:
      return NULL;
    }
}


void
_XmScanningCacheSet(_XmStringNREntry entry,
		    XmDirection d,
		    int field,
		    XtPointer value)
{
  _XmStringScanningCache cache;

  cache = (_XmStringScanningCache)CacheGet((_XmStringEntry)entry,
					   _XmSCANNING_CACHE, True,
					   (XtPointer)(long)d);
  if (!cache)
    return;

  switch (field)
    {
    case _XmCACHE_DIRTY:
      cache->header.dirty = (Boolean)(long)value;
      break;
    case _XmCACHE_SCAN_LEFT:
      cache->left = (_XmStringEntry)value;
      break;
    case _XmCACHE_SCAN_RIGHT:
      cache->right = (_XmStringEntry)value;
      break;
    case _XmCACHE_SCAN_LAYOUT:
      cache->layout_direction = (XmDirection)(long)value;
      break;
    case _XmCACHE_SCAN_DEPTH:
      cache->depth = (unsigned short)(long)value;
      break;
    }
}


XtPointer
_XmRenderCacheGet(_XmStringEntry entry,
		  XmRenderTable rt,
		  int field)
{
  _XmStringRenderingCache cache;

  cache = (_XmStringRenderingCache)CacheGet(entry, _XmRENDERING_CACHE, False,
					    (XtPointer)rt);
  if (!cache)
    {
      if (entry && _XmEntryUnoptimized(entry) && (field == _XmCACHE_DIRTY))
	return (XtPointer)True;
      else
	return NULL;
    }

  switch (field)
    {
    case _XmCACHE_DIRTY:
      return (XtPointer)(long)cache->header.dirty;
    case _XmCACHE_RENDER_X:
      return (XtPointer)(long)cache->x;
    case _XmCACHE_RENDER_Y:
      return (XtPointer)(long)cache->y;
    case _XmCACHE_RENDER_WIDTH:
      return (XtPointer)(long)cache->width;
    case _XmCACHE_RENDER_HEIGHT:
      return (XtPointer)(long)cache->height;
    case _XmCACHE_RENDER_BASELINE:
      return (XtPointer)(long)cache->baseline;
    case _XmCACHE_RENDER_ASCENT:
      return (XtPointer)(long)cache->ascent;
    case _XmCACHE_RENDER_DESCENT:
      return (XtPointer)(long)cache->descent;
    case _XmCACHE_RENDER_RENDITION:
      return (XtPointer)cache->rendition;
    case _XmCACHE_RENDER_PREV_TABS:
      return (XtPointer)(long)cache->prev_tabs;
    default:
      return NULL;
    }
}


void
_XmRenderCacheSet(_XmStringEntry entry,
		  XmRenderTable rt,
		  int field,
		  XtPointer value)
{
  _XmStringRenderingCache cache;

  cache = (_XmStringRenderingCache)CacheGet(entry, _XmRENDERING_CACHE, True,
					    (XtPointer)rt);
  if (!cache)
    return;

  switch (field)
    {
    case _XmCACHE_DIRTY:
      cache->header.dirty = (Boolean)(long)value;
      break;
    case _XmCACHE_RENDER_X:
      cache->x = (int)(long)value;
      break;
    case _XmCACHE_RENDER_Y:
      cache->y = (int)(long)value;
      break;
    case _XmCACHE_RENDER_WIDTH:
      cache->width = (int)(long)value;
      break;
    case _XmCACHE_RENDER_HEIGHT:
      cache->height = (int)(long)value;
      break;
    case _XmCACHE_RENDER_BASELINE:
      cache->baseline = (int)(long)value;
      break;
    case _XmCACHE_RENDER_ASCENT:
      cache->ascent = (int)(long)value;
      break;
    case _XmCACHE_RENDER_DESCENT:
      cache->descent = (int)(long)value;
      break;
    case _XmCACHE_RENDER_RENDITION:
      if (cache->rendition != NULL) XmRenditionFree(cache->rendition);
      cache->rendition = (XmRendition)value;
      break;
    case _XmCACHE_RENDER_PREV_TABS:
      cache->prev_tabs = (char)(long)value;
      break;
    }
}


/*
 * find width of widest line in XmString
 */
Dimension
XmStringWidth(
        XmRenderTable rendertable,
        XmString string )
{
  Dimension width, height;
  XmStringExtent(rendertable, string, &width, &height);
  return(width);
}

/*
 * find total height of an XmString
 */
Dimension
XmStringHeight(
        XmRenderTable rendertable,
        XmString string )
{
  Dimension width, height;
  XmStringExtent(rendertable, string, &width, &height);
  return(height);
}

/*
 * find the rectangle which will enclose the text
 */
void
XmStringExtent(
        XmRenderTable rendertable,
        XmString string,
        Dimension *width,
        Dimension *height )
{
  Dimension cur_width = 0, max_width = 0;
  Dimension cur_height = 0, line_height = 0;
  Dimension asc, dsc;
  int j;
  Display *d;
  XtAppContext app = NULL;

  *width = 0, *height = 0;

  if ((rendertable == NULL) || (string == NULL)) return;

#ifdef XTHREADS
  if (_XmRTDisplay(rendertable))
    app = XtDisplayToApplicationContext(_XmRTDisplay(rendertable));
  if (app)
  {
      _XmAppLock(app);
  }
  else
  {
      _XmProcessLock();
  }
#endif
  if (_XmStrOptimized(string))
    OptLineMetrics(rendertable, string, NULL, NULL, width, height, NULL, NULL);
  else
    {
      _XmRenditionRec	scratch;
      _XmRendition	tmp;
      XmRendition	rend;
      _XmStringArraySegRec array_seg;

      bzero((char*) &scratch, sizeof(_XmRenditionRec));
      tmp = &scratch;
      rend = &tmp;

      /* Initialize for tabs. */
      d = (_XmRTDisplay(rendertable) == NULL) ?
	_XmGetDefaultDisplay()
	  : _XmRTDisplay(rendertable);

      _XmRendDisplay(rend) = d;

      _XmStringLayout(string, XmLEFT_TO_RIGHT);

      for (j = 0; j < _XmStrLineCountGet(string); j++)
	{
	  _XmStringEntry line;

	  if (_XmStrImplicitLine(string))
	    line = _XmStrEntry(string)[j];
	  else {
	    _XmEntryType(&array_seg) = XmSTRING_ENTRY_ARRAY;
	    _XmEntrySegmentCount(&array_seg) = _XmStrEntryCount(string);
	    _XmEntrySegment(&array_seg) =
	      (_XmStringNREntry *)_XmStrEntry(string);
	    line = (_XmStringEntry)&array_seg;
	  }

	  LineMetrics(line, rendertable, &rend, NULL, XmLEFT_TO_RIGHT,
		      &cur_width, &cur_height, &asc, &dsc);

	  /* Returned height for empty lines is zero, so go
	     with previous in that case. */
	  if (cur_height != 0) line_height = cur_height;

	  *height += line_height;

	  if (cur_width > max_width) max_width = cur_width;
	}
      *width = max_width;
      if (_XmRendTags(rend) != NULL)
	XtFree((char *)_XmRendTags(rend));
    }
#ifdef XTHREADS
  if (app)
  {
     _XmAppUnlock(app);
  }
  else
  {
     _XmProcessUnlock();
  }
#endif
}

Boolean
XmStringEmpty(
        XmString string )
{
  int i, j;

  _XmProcessLock();
  if (!string) {
    _XmProcessUnlock();
    return (TRUE);
  }

  if (_XmStrOptimized(string)) {
    if (_XmStrByteCount(string) > 0) {
      _XmProcessUnlock();
      return FALSE;
    }
  } else {
    _XmStringEntry  *entry = _XmStrEntry(string);

    for (i = 0; i < _XmStrEntryCount(string); i++) {
      if (_XmEntryMultiple(entry[i])) {
	int segcount = _XmEntrySegmentCount(entry[i]);
	for (j = 0; j < segcount; j++) {
	  _XmStringNREntry seg = _XmEntrySegment(entry[i])[j];
	  if (_XmEntryByteCountGet((_XmStringEntry)seg) > 0) {
	    _XmProcessUnlock();
	    return (FALSE);
	  }
	}
      } else {
	if (_XmEntryByteCountGet(entry[i]) > 0) {
	  _XmProcessUnlock();
	  return (FALSE);
	}
      }
    }
  }
  _XmProcessUnlock();
  return (TRUE);
}

Boolean
XmStringIsVoid(XmString string)
{
  XmStringComponentType	type;
  _XmStringContextRec	stack_context;
  unsigned int	len;
  XtPointer	val;

  _XmProcessLock();
  if (!string) {
     _XmProcessUnlock();
     return (TRUE);
  }

  _XmStringContextReInit(&stack_context, string);

  while ((type = XmeStringGetComponent(&stack_context, TRUE, FALSE,
				       &len, &val)) !=
	 XmSTRING_COMPONENT_END)
    {
      switch(type)
	{
	case XmSTRING_COMPONENT_TAB:
	case XmSTRING_COMPONENT_TEXT:
	case XmSTRING_COMPONENT_LOCALE_TEXT:
	case XmSTRING_COMPONENT_WIDECHAR_TEXT:
	case XmSTRING_COMPONENT_SEPARATOR:
	  _XmStringContextFree(&stack_context);
	  _XmProcessUnlock();
	  return(FALSE);

	default:
	  break;
	}
    }

  _XmStringContextFree(&stack_context);
  _XmProcessUnlock();
  return(TRUE);
}

/****************************************************************
 * EntryCvtToOpt:
 *
 * Converts a single segment to a single Opt segment.
 * Returns NULL if no conversion could be done.
 *
 ****************************************************************/
static _XmStringEntry
EntryCvtToOpt(_XmStringEntry entry)
{
  char *text;
  _XmStringEntry new_entry;

  if (!entry)
    return NULL;

  /* Array segment */
  if (_XmEntryMultiple(entry))
    return NULL;

  /* Unoptimized, convert if possible */
  if (_XmEntryUnoptimized(entry)) {
    if (_XmEntryPushGet(entry) ||
	_XmEntryPopGet(entry) ||
	_XmUnoptSegRendBeginCount(entry) > 1 ||
	_XmUnoptSegRendEndCount(entry) > 1 ||
	_XmEntryTabsGet(entry) > 7 ||
	_XmEntryByteCountGet(entry) > 255 ||
	(_XmUnoptSegRendBeginCount(entry) &&
	 _XmStringIndexCacheTag(_XmEntryRendBeginGet(entry, 0),
				XmSTRING_TAG_STRLEN) >= REND_INDEX_MAX) ||
	(_XmUnoptSegRendEndCount(entry) &&
	 _XmStringIndexCacheTag(_XmEntryRendEndGet(entry, 0),
				XmSTRING_TAG_STRLEN) >= REND_INDEX_MAX) ||
	(_XmUnoptSegRendBeginCount(entry) && _XmUnoptSegRendEndCount(entry) &&
	 _XmEntryRendEndGet(entry, 0) != _XmEntryRendBeginGet(entry, 0)) ||
	(_XmUnoptSegTag(entry) &&
	 _XmStringIndexCacheTag(_XmUnoptSegTag(entry), XmSTRING_TAG_STRLEN) >=
	 TAG_INDEX_MAX))
      return NULL;

    new_entry = (_XmStringEntry)XtCalloc(1, sizeof(_XmStringOptSegRec));
    _XmEntryType(new_entry) = XmSTRING_ENTRY_OPTIMIZED;
    _XmEntryImm(new_entry) = 0;
    _XmEntryTextTypeSet(new_entry, (XmTextType) _XmEntryTextTypeGet(entry));
    _XmEntryTagIndex(new_entry) =
      (_XmUnoptSegTag(entry) ?
       _XmStringIndexCacheTag(_XmUnoptSegTag(entry), XmSTRING_TAG_STRLEN)
       : TAG_INDEX_UNSET);
    _XmEntryByteCountSet(new_entry, _XmUnoptSegByteCount(entry));
    _XmEntryRendIndex(new_entry) =
      (_XmUnoptSegRendBeginCount(entry) ?
       _XmStringIndexCacheTag(_XmEntryRendBeginGet(entry, 0),
			      XmSTRING_TAG_STRLEN)
       : (_XmUnoptSegRendEndCount(entry) ?
	  _XmStringIndexCacheTag(_XmEntryRendEndGet(entry, 0),
				 XmSTRING_TAG_STRLEN)
	  : REND_INDEX_UNSET));
    _XmEntryRendBeginCountSet(new_entry, _XmUnoptSegRendBeginCount(entry));
    _XmEntryRendEndCountSet(new_entry, _XmUnoptSegRendEndCount(entry));
    _XmEntryTabsSet(new_entry, _XmEntryTabsGet(entry));
    _XmEntryDirectionSet(new_entry, _XmEntryDirectionGet(entry));
    _XmEntryFlippedSet(new_entry, _XmEntryFlippedGet(entry));
    _XmEntryPermSet(new_entry, _XmEntryPermGet(entry));
    _XmEntrySoftNewlineSet(new_entry, _XmEntrySoftNewlineGet(entry));
    if (_XmEntryPermGet(entry))
      _XmEntryTextSet(new_entry, _XmEntryTextGet(entry));
    else {
      unsigned int len = _XmEntryByteCountGet(entry);
      text = (char *)XtMalloc(len);
      memcpy(text, _XmEntryTextGet(entry), len);
      _XmEntryTextSet(new_entry, text);
    }
    return new_entry;
  }
  /* If we were already opt., return a copy */
  return _XmStringEntryCopy(entry);
}



/****************************************************************
 * EntryCvtToUnOpt:
 *
 * Converts a single segment to a single UnOpt segment.
 * Returns NULL if no conversion could be done (only for Array segs)
 *
 ****************************************************************/
static _XmStringEntry
EntryCvtToUnopt(_XmStringEntry entry)
{
  char *text;
  _XmStringEntry new_entry;
  unsigned int len;

  if (!entry)
    return NULL;

  /* Array segment */
  if (_XmEntryMultiple(entry))
    return NULL;

  /* Unoptimized, return copy */
  if (_XmEntryUnoptimized(entry))
    return _XmStringEntryCopy(entry);

  /* Optimized: convert */
  len = _XmEntryByteCountGet(entry);
  new_entry = (_XmStringEntry)XtCalloc(1, sizeof(_XmStringUnoptSegRec));
  _XmEntryType(new_entry) = XmSTRING_ENTRY_UNOPTIMIZED;
  _XmEntryTextTypeSet(new_entry, (XmTextType) _XmEntryTextTypeGet(entry));
  _XmUnoptSegTag(new_entry) = _XmEntryTag(entry);
  _XmUnoptSegByteCount(new_entry) = len;
  _XmUnoptSegRendBeginCount(new_entry) = _XmEntryRendBeginCountGet(entry);
  _XmUnoptSegRendEndCount(new_entry) = _XmEntryRendEndCountGet(entry);
  if (_XmEntryRendBeginCountGet(entry)) {
    _XmUnoptSegRendBegins(new_entry) =
      (XmStringTag *)XtMalloc(sizeof(XmStringTag));
    _XmUnoptSegRendBegins(new_entry)[0] = _XmEntryRendBeginGet(entry, 0);
  }
  if (_XmEntryRendEndCountGet(entry)) {
    _XmUnoptSegRendEnds(new_entry) =
      (XmStringTag *)XtMalloc(sizeof(XmStringTag));
    _XmUnoptSegRendEnds(new_entry)[0] = _XmEntryRendEndGet(entry, 0);
  }
  _XmEntryTabsSet(new_entry, _XmEntryTabsGet(entry));
  _XmEntryDirectionSet(new_entry, _XmEntryDirectionGet(entry));
  _XmEntryFlippedSet(new_entry, _XmEntryFlippedGet(entry));
  _XmEntryPermSet(new_entry, _XmEntryPermGet(entry));
  _XmEntrySoftNewlineSet(new_entry, _XmEntrySoftNewlineGet(entry));
  _XmEntryPushSet(new_entry, _XmEntryPushGet(entry));
  _XmEntryPopSet(new_entry, _XmEntryPopGet(entry));
  if (_XmEntryPermGet(entry))
    _XmEntryTextSet(new_entry, _XmEntryTextGet(entry));
  else if (len>0) {
    text = (char *)XtMalloc(len);
    memcpy(text, _XmEntryTextGet(entry), len);
    _XmEntryTextSet(new_entry, text);
  }
  else
    _XmEntryTextSet(new_entry, NULL);
  return new_entry;
}



/* Convert an optimized _XmString to an equivalent non-optimized _XmString */
_XmString
_XmStringOptToNonOpt(_XmStringOpt string)
{
  _XmString str;
  _XmStringUnoptSegRec seg;

  _XmStrCreate(str, XmSTRING_MULTIPLE_ENTRY, 0);
  _XmEntryInit((_XmStringEntry)&seg, XmSTRING_ENTRY_OPTIMIZED);

  _XmEntryTagIndex(&seg) = _XmStrTagIndex((_XmString)string);
  _XmEntryRendIndex(&seg) = _XmStrRendIndex((_XmString)string);
  _XmEntryRendBeginCountSet(&seg, _XmStrRendBegin((_XmString)string));
  _XmEntryRendEndCountSet(&seg, _XmStrRendEnd((_XmString)string));
  _XmEntryTextTypeSet(&seg, (XmTextType) _XmStrTextType((_XmString)string));
  _XmEntryByteCountSet(&seg, _XmStrByteCount((_XmString)string));
  _XmEntryDirectionSet((_XmStringEntry)&seg,
		       _XmStrDirection((_XmString)string));
  _XmEntryTabsSet(&seg, _XmStrTabs((_XmString)string));
  _XmEntryFlippedSet(&seg, _XmStrFlipped((_XmString)string));
  _XmEntryTextSet((_XmStringEntry)&seg, _XmStrText((_XmString)string));

  _XmStringSegmentNew(str, 0, (_XmStringEntry)&seg, True);
   return str;
}

/*
 * figure out if there is sub string match, and if so the begining
 * and end of the match section in pixels.  Don't touch anything if
 * there is no match
 */
static void
SubStringPosition(
        Boolean one_byte,
	XmRenderTable rt,
	XmRendition entry,
        _XmStringEntry seg,
        _XmStringEntry under_seg,
        Position x,
        Dimension *under_begin,
        Dimension *under_end )
{
  char *a = (char*) _XmEntryTextGet(seg);
  char *b = (char*) _XmEntryTextGet(under_seg);
  char *seg_tag = _XmEntryTag(seg);
  int i, j, k, begin, max, width;
  unsigned int seg_len, under_seg_len;
  Boolean fail;

  /* Metro Link fix: _XmEntryTag(seg) can be NULL, but the original Motif
   * code never checked for that.  We check, and if it is NULL, we treat
   * it as if it was set to XmFONTLIST_DEFAULT_TAG. */

  if (seg_tag == NULL)
    seg_tag = XmFONTLIST_DEFAULT_TAG;

  if (!((seg_tag == _XmEntryTag(under_seg)) ||
	((strcmp(seg_tag, XmFONTLIST_DEFAULT_TAG) == 0) &&
	 _XmStringIsCurrentCharset(_XmEntryTag(under_seg))) ||
	((strcmp(_XmEntryTag(under_seg), XmFONTLIST_DEFAULT_TAG) == 0) &&
	 _XmStringIsCurrentCharset(seg_tag))))
    return;

  seg_len = _XmEntryByteCountGet(seg);
  under_seg_len = _XmEntryByteCountGet(under_seg);
  if (seg_len < under_seg_len)
    return;

  max = (seg_len - under_seg_len);

  if (_XmRendFontType(entry) == XmFONT_IS_FONT
      || _XmRendFontType(entry) == XmFONT_IS_XFT) {
    XFontStruct *font_struct = (XFontStruct *)_XmRendFont(entry);

    if (one_byte) {
      for (i = 0; i <= max; i++) {
	fail = FALSE;
	begin = i;

	for (j = 0; j < under_seg_len; j++) {
	  if (a[i+j] != b[j]) {
	    fail = TRUE;
	    break;
	  }
	}
	if ( ! fail) {    /* found it */
	  if (begin == 0)
	    *under_begin = x;
	  else
	    if (_XmRendFontType(entry) == XmFONT_IS_FONT)
	      *under_begin = x + abs(XTextWidth (font_struct, a, begin));
#if USE_XFT
	    else {
	      XGlyphInfo ext;
	      XftTextExtentsUtf8(_XmRendDisplay(entry), _XmRendXftFont(entry),
	                      (FcChar8*)a, begin, &ext);
	      *under_begin = x + ext.xOff;
	    }
#endif

	  width = _XmEntryWidthGet((_XmStringEntry)under_seg, rt);

	  if (width == 0) {
	    if (_XmRendFontType(entry) == XmFONT_IS_FONT)
	      width = abs(XTextWidth(font_struct, b, under_seg_len));
#if USE_XFT
	    else {
	      XGlyphInfo ext;
	      XftTextExtentsUtf8(_XmRendDisplay(entry), _XmRendXftFont(entry),
	                      (FcChar8*)b, under_seg_len, &ext);
	      width = ext.xOff;
	    }
#endif
	    _XmEntryWidthSet((_XmStringEntry)under_seg, rt, width);
	  }

	  *under_end = *under_begin + width;

	  return;
	}
      }
    } else {
      /*
       * If either string isn't even byte length, it can't be
       * two bytes/char.
       */

      if (((seg_len % 2) != 0) || ((under_seg_len % 2) != 0))
	return;

      /*
       * search for the substring
       */

      for (i = 0; i <= max; i+=2) {
	fail = FALSE;
	begin = i;

	for (j = 0; j < under_seg_len; j+=2) {
	  if ((a[i+j] != b[j]) || (a[i+j+1] != b[j+1])) {
	    fail = TRUE;
	    break;
	  }
	}
	if ( ! fail) {    /* found it */
	  if (begin == 0)
	    *under_begin = x;
	  else
	    if (_XmRendFontType(entry) == XmFONT_IS_FONT)
	      *under_begin =
	        x + abs(XTextWidth16 (font_struct, (XChar2b *) a, begin/2));
#if USE_XFT
	    else {
	      XGlyphInfo ext;
	      XftTextExtents16(_XmRendDisplay(entry), _XmRendXftFont(entry),
	                      (FcChar16*)a, begin, &ext);
	      *under_begin = x + ext.xOff;
	    }
#endif

	  width = _XmEntryWidthGet((_XmStringEntry)under_seg, rt);

	  if (width == 0) {
	    if (_XmRendFontType(entry) == XmFONT_IS_FONT)
	      width = abs(XTextWidth16(font_struct, (XChar2b *) b,
				       under_seg_len/2));
#if USE_XFT
	    else {
	      XGlyphInfo ext;
	      XftTextExtents16(_XmRendDisplay(entry), _XmRendXftFont(entry),
	                      (FcChar16*)b, under_seg_len, &ext);
	      width = ext.xOff;
	    }
#endif
	    _XmEntryWidthSet((_XmStringEntry)under_seg, rt, width);
	  }

	  *under_end = *under_begin + width;
	  return;
	}
      }
    }
  } else {
    XFontSet font_set = (XFontSet)_XmRendFont(entry);
    XmTextType type = (XmTextType) _XmEntryTextTypeGet(under_seg);
    int len_a, len_a1, len_b;

    for (i = 0; i <= max; i += len_a) {
      fail = FALSE;
      begin = i;

      if (type == XmWIDECHAR_TEXT) {
	len_a = sizeof(wchar_t);

	for (j = 0; j < under_seg_len; j += sizeof(wchar_t))
	  if (((wchar_t *)a)[(i+j)/len_a] != ((wchar_t *)b)[j/len_a]) {
	    fail = TRUE;
	    break;
	  }
      } else {
	len_a = mblen(&a[i], MB_CUR_MAX);
	if (len_a < 1) return;
	len_a1 = len_a;

	for (j = 0; j < under_seg_len; j += len_b) {
	  len_b = mblen(&b[j], MB_CUR_MAX);
	  if (len_b < 1) return;

	  if (len_b == len_a1) {
	    for (k = 0; k < len_b; k++) {
	      if (a[i+j+k] != b[j+k]) {
		fail = TRUE;
		break;
	      }
	    }
	    if (fail == TRUE) break;
	  } else {
	    fail = TRUE;
	    break;
	  }
	}
      }

      if (!fail) {          /* found it */
#if XM_UTF8
        Boolean utf8 = ((_XmEntryTextTypeGet(seg) == XmCHARSET_TEXT) &&
              (((_XmEntryTag((_XmStringEntry)seg) ==
                 XmFONTLIST_DEFAULT_TAG) &&
                _XmStringIsCurrentCharset("UTF-8")) ||
               (strcmp(seg_tag, "UTF-8") == 0)));
#else
        Boolean utf8 = False;
#endif
	if (begin == 0)
	  *under_begin = x;
	else if (type == XmWIDECHAR_TEXT) {
	  *under_begin =
	    x + abs(XwcTextEscapement(font_set, (wchar_t *)a,
				      begin/sizeof(wchar_t)));
        } else {
#if XM_UTF8
            if (utf8)
                *under_begin =
                    x + abs(Xutf8TextEscapement(font_set, a, begin));
            else
#endif
                *under_begin =
                    x + abs(XmbTextEscapement(font_set, a, begin));
        }

	width = _XmEntryWidthGet((_XmStringEntry)under_seg, rt);

	if (width == 0) {
#if XM_UTF8
	  width = (type == XmWIDECHAR_TEXT)
	    ? abs(XwcTextEscapement(font_set, (wchar_t *)b,
				  under_seg_len / sizeof(wchar_t)))
	    : (utf8
                    ? abs(Xutf8TextEscapement(font_set, b, under_seg_len))
                    : abs(XmbTextEscapement(font_set, b, under_seg_len)));
#else
	  width = (type == XmWIDECHAR_TEXT)
	    ? abs(XwcTextEscapement(font_set, (wchar_t *)b,
				  under_seg_len / sizeof(wchar_t)))
	    : abs(XmbTextEscapement(font_set, b, under_seg_len));
#endif
	  _XmEntryWidthSet((_XmStringEntry)under_seg, rt, width);
	}

	*under_end = *under_begin + width;

	return;
      }
    }
  }
}

extern void
_XmStringDrawLining(Display *d,
		    Drawable w,
		    Position x,
		    Position y,
		    Dimension width,
		    Dimension height,
		    Dimension descender,
		    XmRendition rend,
		    Pixel select_color,	/* unused */
		    XmHighlightMode mode,
		    Boolean colors_set)
{
  GC			gc;
  XGCValues 		xgcv;
  Pixel			fg, bg, old_fg, old_bg;
  unsigned char		under, thru;
  XGCValues 		current_gcv;
  int			style, old_style=LineSolid, cur_style=LineSolid;

  old_fg = old_bg = XmUNSPECIFIED_PIXEL;

  _XmRendDisplay(rend) = d;

  gc = _XmRendGC(rend);

  fg = _XmRendFG(rend);
  bg = _XmRendBG(rend);

  under = _XmRendUnderlineType(rend);
  thru = _XmRendStrikethruType(rend);

  if (!colors_set)
    {
      if (fg != XmUNSPECIFIED_PIXEL)
	{
	  XGetGCValues(d, gc, GCForeground, &current_gcv);
	  if (current_gcv.foreground != fg)
	    {
	      old_fg = current_gcv.foreground;
	      xgcv.foreground = fg;
	      XChangeGC(d, gc, GCForeground, &xgcv);
	    }
	}

      if (bg != XmUNSPECIFIED_PIXEL)
	{
	  XGetGCValues(d, gc, GCBackground, &current_gcv);
	  if (current_gcv.background != bg)
	    {
	      old_bg = current_gcv.background;
	      xgcv.background = bg;
	      XChangeGC(d, gc, GCBackground, &xgcv);
	    }
	}
    }

  if (mode == XmHIGHLIGHT_SECONDARY_SELECTED)
    {
      /* Draw lines */
      XGetGCValues(d, gc, GCLineStyle, &current_gcv);
      old_style = current_gcv.line_style;

      style = LineSolid;

      if (old_style != style)
	{
	  cur_style = xgcv.line_style = style;
	  XChangeGC(d, gc, GCLineStyle, &xgcv);
	}

      XDrawLine (d, w, gc,
		 x, y + SINGLE_OFFSET,
		 x + width - 1, y + SINGLE_OFFSET);
    }
  else
    {
      XGetGCValues(d, gc, GCLineStyle, &current_gcv);
      cur_style = old_style = current_gcv.line_style;

      if ((under != XmAS_IS) && (under != XmNO_LINE))
	{
	  if ((under == XmSINGLE_DASHED_LINE) ||
	      (under == XmDOUBLE_DASHED_LINE))
	    style = LineDoubleDash;
	  else style = LineSolid;

	  if (cur_style != style)
	    {
	      cur_style = xgcv.line_style = style;
	      XChangeGC(d, gc, GCLineStyle, &xgcv);
	    }

	  if ((under == XmSINGLE_LINE) ||
	      (under == XmSINGLE_DASHED_LINE))
	    {
	      XDrawLine(d, w, gc,
			x, y + SINGLE_OFFSET,
			(x + width - 1), y + SINGLE_OFFSET);
	    }
	  else if ((under == XmDOUBLE_LINE) ||
		   (under == XmDOUBLE_DASHED_LINE))
	    {
	      XSegment	segs[2];

	      segs[0].x1 = segs[1].x1 = x;
	      segs[0].x2 = segs[1].x2 = x + width - 1;
	      segs[0].y1 = segs[0].y2 = y;
	      segs[1].y1 = segs[1].y2 = y + DOUBLE_OFFSET;

	      XDrawSegments(d, w, gc, segs, 2);
	    }
	}

      if ((thru != XmAS_IS) && (thru != XmNO_LINE))
	{
	  if ((thru == XmSINGLE_DASHED_LINE) ||
	      (thru == XmDOUBLE_DASHED_LINE))
	    style = LineDoubleDash;
	  else style = LineSolid;

	  if (cur_style != style)
	    {
	      cur_style = xgcv.line_style = style;
	      XChangeGC(d, gc, GCLineStyle, &xgcv);
	    }

	  if ((thru == XmSINGLE_LINE) ||
	      (thru == XmSINGLE_DASHED_LINE))
	    {
	      XDrawLine(d, w, gc,
			x, (y + descender - height/2 - 1),
			(x + width - 1), (y + descender - height/2 - 1));
	    }
	  else if ((thru == XmDOUBLE_LINE) ||
		   (thru == XmDOUBLE_DASHED_LINE))
	    {
	      XSegment	segs[2];

	      segs[0].x1 = segs[1].x1 = x;
	      segs[0].x2 = segs[1].x2 = x + width - 1;
	      segs[0].y1 = segs[0].y2 = (y + descender - height/2) - 2;
	      segs[1].y1 = segs[1].y2 = (y + descender - height/2) + 1;

	      XDrawSegments(d, w, gc, segs, 2);
	    }
	}
    }

  if ((cur_style != old_style) &&
      ((old_style == LineSolid) || (old_style == LineOnOffDash) ||
       (old_style == LineDoubleDash)))
    {
      xgcv.line_style = old_style;
      XChangeGC(d, gc, GCLineStyle, &xgcv);
    }

  if (!colors_set)
    {
      if (old_fg != XmUNSPECIFIED_PIXEL)
	{
	  xgcv.foreground = old_fg;
	  XChangeGC(d, gc, GCForeground, &xgcv);
	}

      if (old_bg != XmUNSPECIFIED_PIXEL)
	{
	  xgcv.background = old_bg;
	  XChangeGC(d, gc, GCBackground, &xgcv);
	}
    }
}

extern void
_XmStringDrawSegment(Display *d,
		     Drawable w,
		     Position x,
		     Position y,
		     Dimension width,
		     Dimension height,
		     _XmStringNREntry seg,
		     XmRendition rend,
		     XmRenderTable rendertable,
		     Boolean image,
		     XmString *underline,
		     Dimension descender
		     )
{
  Boolean 		text16 = False, multibyte, widechar, utf8;
  Font    		oldfont = (Font) 0;
  GC			gc;
  XGCValues 		xgcv;
  char 			*draw_text;       /* text to be drawn -
					     flipped in RtoL mode */
  char  		flip_char[100];	  /* but simple */
  char 			*flip_char_extra = NULL;
  Pixel			fg, bg, old_fg, old_bg;
  XGCValues 		current_gcv;
  Dimension		under_begin, under_end;
  unsigned int		seg_len;
  int			font_type;
  int			text_type;

  old_fg = old_bg = XmUNSPECIFIED_PIXEL;

  _XmRendDisplay(rend) = d;
  font_type = _XmRendFontType(rend);
  text_type = _XmEntryTextTypeGet((_XmStringEntry)seg);

  seg_len = _XmEntryByteCountGet((_XmStringEntry)seg);
  if (seg_len  > 0)
    {
      multibyte = (((text_type ==  XmMULTIBYTE_TEXT) ||
		    (text_type == XmCHARSET_TEXT)) &&
		   (font_type == XmFONT_IS_FONTSET));

      widechar = ((text_type == XmWIDECHAR_TEXT) &&
		  (font_type == XmFONT_IS_FONTSET));

#if XM_UTF8
      utf8 = ((text_type == XmMULTIBYTE_TEXT || text_type == XmCHARSET_TEXT) &&
		   (font_type == XmFONT_IS_FONTSET ||
		    font_type == XmFONT_IS_XFT ||
		    (font_type == XmFONT_IS_FONT
		     && _XmIsISO10646(d, _XmRendFont(rend)))) &&
              (((_XmEntryTag((_XmStringEntry)seg) == XmFONTLIST_DEFAULT_TAG &&
                (_XmStringIsCurrentCharset("UTF-8"))) ||
               ((_XmEntryTagIndex(seg) != TAG_INDEX_UNSET
	       && strcmp(_XmEntryTag((_XmStringEntry)seg), "UTF-8") == 0)))));
#else
      utf8 = False;
#endif
      gc = _XmRendGC(rend);

      fg = _XmRendFG(rend);
      bg = _XmRendBG(rend);


      if (fg != XmUNSPECIFIED_PIXEL)
	{
	  XGetGCValues(d, gc, GCForeground, &current_gcv);
	  if (current_gcv.foreground != fg)
	    {
	      old_fg = current_gcv.foreground;
	      xgcv.foreground = fg;
	      XChangeGC(d, gc, GCForeground, &xgcv);
	    }
	}

      if (bg != XmUNSPECIFIED_PIXEL)
	{
	  XGetGCValues(d, gc, GCBackground, &current_gcv);
	  if (current_gcv.background != bg)
	    {
	      old_bg = current_gcv.background;
	      xgcv.background = bg;
	      XChangeGC(d, gc, GCBackground, &xgcv);
	    }
	}

      if (!multibyte && !widechar && _XmRendFontType(rend) != XmFONT_IS_XFT)
	{
	  XFontStruct *f = (XFontStruct *)_XmRendFont(rend);

	  /* If we don't have a font, don't render. */
	  if (f == NULL)
	      return;

	  text16 = two_byte_font(f);

	  XGetGCValues(d, gc, GCFont, &current_gcv) ;

	  xgcv.font = f->fid;			  /* get segment font */

	  if (current_gcv.font != xgcv.font)	  /* not right one */
	    {					  /* change it */
	      oldfont = current_gcv.font;
	      XChangeGC(d, gc, GCFont, &xgcv);
	    }
	}

      if (_XmEntryDirectionGet((_XmStringEntry)seg) ==
	  XmSTRING_DIRECTION_R_TO_L)
	{
	  /* Flip the bytes. */
	  char *p = flip_char, *q;
	  char *ltor_text;
	  int i, j;
	  if (seg_len > 100)
	    p = flip_char_extra = (char *) ALLOCATE_LOCAL(seg_len);

	  draw_text = p;
	  ltor_text = (char *)_XmEntryTextGet((_XmStringEntry)seg);

	  if (multibyte)	/* Have to flip a mb character at time. */
	    {
	      int   len;

	      q = ltor_text;
	      p += seg_len;
	      for (i = 0; i < seg_len; i += len)
		{
		  len = mblen(q, MB_CUR_MAX);
		  if (len < 1) /* Something went wrong, just return for now. */
		    return;

		  p -= len;
		  for (j = 0; j < len; j++)
		    {
		      p[j] = q[j];
		    }
		  q += len;
		}
	    }
	  else if (!text16)
	    {
	      q = (ltor_text + seg_len - 1);
	      for (i = 0; i < seg_len; i++)
		*p++ = *q--;
	    }
	  else
	    /* Have to flip two at a time, maintaining their order. */
	    {
	      char tmp;

	      q = (ltor_text + seg_len - 1);
	      for (i = 0; i < Half(seg_len); i++)
		{
		  tmp = *q--;
		  *p++ = *q--;
		  *p++ = tmp;
		}
	    }
	} else /* LtoR */ {
	  draw_text = (char *)_XmEntryTextGet((_XmStringEntry)seg);
	}

      if (*underline != (_XmString)NULL)
	{
	  under_begin = under_end = 0;
	  if (_XmStrOptimized(*underline))
	    {
	      /*
	       * This is an optimized string; coerce underline to segment
	       * and call the sub-string search routine.
	       */
	      Boolean			imm;
	      _XmStringOptSegRec	under_seg;

	      if (_XmStrText(*underline) !=
		  (char *)_XmEntryTextGet((_XmStringEntry)*underline))
		/* If XtPointer in union in optimized segment leads to
		 * padding in struct between header and text data
		 * (on some 64-bit architectures) we have to move
		 * text data, since optimized string does not have padding.
		 */
		{
		  bzero((char*)&under_seg, sizeof(_XmStringOptSegRec));
		  _XmEntryType(&under_seg) = XmSTRING_ENTRY_OPTIMIZED;
		  _XmEntryTagIndex(&under_seg) = _XmStrTagIndex(*underline);
		  _XmEntryByteCountSet(&under_seg, _XmStrByteCount(*underline));
		  _XmEntryTextTypeSet(&under_seg,
				      (XmTextType)_XmStrTextType(*underline));
		  _XmEntryTextSet((_XmStringEntry)&under_seg,
				  (char *)_XmStrText(*underline));

		  SubStringPosition((!text16), rendertable, rend,
				    (_XmStringEntry)seg,
				    (_XmStringEntry)&under_seg, x,
				    &under_begin, &under_end);
		}
	      else
		{
		  imm = _XmEntryImm((_XmStringEntry)*underline);
		  _XmEntryImm((_XmStringEntry)*underline) = TRUE;
		  SubStringPosition((!text16), rendertable, rend,
				    (_XmStringEntry)seg,
				    (_XmStringEntry)*underline, x,
				    &under_begin, &under_end);
		  _XmEntryImm((_XmStringEntry)*underline) = imm;
		}
	    }
	  else {
	    _XmStringEntry line;
	    line = _XmStrEntry(*underline)[0];

	    if ((_XmStrEntryCount(*underline) > 0) &&
		(_XmEntrySegmentCountGet(line) > 0))
	      {
		_XmStringNREntry under_seg;

		under_seg = (_XmStringNREntry)_XmEntrySegmentGet(line)[0];

		SubStringPosition((!text16), rendertable, rend,
				  (_XmStringEntry)seg,
				  (_XmStringEntry)under_seg, x,
				  &under_begin, &under_end);
	      }
	  }
	}

#if USE_XFT
      if (_XmRendFontType(rend) == XmFONT_IS_XFT)
        {
	    _XmXftDrawString(d, w, rend, 1, x, y, draw_text, seg_len, image);
        }
      else /* TODO: fix indentation */
#endif
        {
      if (image)
	{
	  if (text16)
	    if (utf8)
	    {
		size_t  ucs_str_len;
		XChar2b *ucs_str;

		/* TODO: it is very unoptimized convert the same sting
		 * twice - for getting extents and drawing */
		ucs_str = _XmUtf8ToUcs2(draw_text, seg_len, &ucs_str_len);
		XDrawImageString16(d, w, gc, x, y, ucs_str, ucs_str_len);
		XFree(ucs_str);
	    } else
	        XDrawImageString16(d, w, gc, x, y, (XChar2b*)draw_text,
			       Half(seg_len));
#if XM_UTF8
          else if (utf8)
            Xutf8DrawImageString(d, w, (XFontSet)_XmRendFont(rend), gc, x, y,
                                 draw_text, seg_len);
#endif
	  else if (multibyte)
	    XmbDrawImageString (d, w, (XFontSet)_XmRendFont(rend), gc, x, y,
				draw_text, seg_len);
	  else if (widechar)
	    XwcDrawImageString (d, w, (XFontSet)_XmRendFont(rend),
				gc, x, y, (wchar_t *) draw_text,
				(seg_len / sizeof(wchar_t)));
	  else
	    XDrawImageString (d, w, gc, x, y, draw_text, seg_len);
	}
      else
	{
	  if (text16)
	  {
	    if (utf8)
	    {
		size_t  ucs_str_len;
		XChar2b *ucs_str;

		/* TODO: it is very unoptimized convert the same sting
		 * twice - for getting extents and drawing */
		ucs_str = _XmUtf8ToUcs2(draw_text, seg_len, &ucs_str_len);
		XDrawString16(d, w, gc, x, y, ucs_str, ucs_str_len);
		XFree(ucs_str);
	    } else
		    XDrawString16 (d, w, gc, x, y, (XChar2b *)draw_text,
				    Half(seg_len));
	  }
#if XM_UTF8
          else if (utf8)
	    Xutf8DrawString(d, w, (XFontSet)_XmRendFont(rend), gc, x, y,
                            draw_text, seg_len);
#endif
	  else if (multibyte)
	    XmbDrawString (d, w, (XFontSet)_XmRendFont(rend), gc, x, y,
			   draw_text, seg_len);
	  else if (widechar)
	    XwcDrawString (d, w, (XFontSet)_XmRendFont(rend),
			   gc, x, y, (wchar_t *) draw_text,
			   (seg_len / sizeof(wchar_t)));
	  else
	    XDrawString(d, w, gc, x, y, draw_text, seg_len);
	}
	}

      /* Draw lines */
      if ((*underline != NULL) && (under_begin != under_end))
	{
	  *underline = (_XmString) NULL;	  /* only once */

	  XDrawLine (d, w, gc,
		     under_begin, (y + descender),
		     under_end, (y + descender));
	}

      _XmStringDrawLining(d, w, x, y, width, height, descender,
			  rend, XmUNSPECIFIED_PIXEL, XmHIGHLIGHT_NORMAL, TRUE);

      if (((Font)0 != oldfont) &&		  /* if font was changed */
	  ((Font)~0 != oldfont))		  /* put it back */
	{
	  xgcv.font = oldfont;
	  XChangeGC (d, gc, GCFont, &xgcv);
	}

      if (old_fg != XmUNSPECIFIED_PIXEL)
	{
	  xgcv.foreground = old_fg;
	  XChangeGC(d, gc, GCForeground, &xgcv);
	}

      if (old_bg != XmUNSPECIFIED_PIXEL)
	{
	  xgcv.background = old_bg;
	  XChangeGC(d, gc, GCBackground, &xgcv);
	}

      if (flip_char_extra != NULL)
	{
	  DEALLOCATE_LOCAL(flip_char_extra);
	}
    }
}

/****************************************************************
 * recursive_layout:
 *    This (partly) recursive function sets up the left/right
 *    pointers for segments to ensure that segments will be
 *    laid out in the correct order
 ****************************************************************/
static void
recursive_layout(_XmString string,
		 int *line_index,
		 int *seg_index,
		 XmDirection direction,
		 XmDirection p_direction,
		 int depth)
{
  _XmStringEntry        line;
  _XmStringNREntry 	seg, seg2;
  _XmStringNREntry 	last;
  XmDirection           pop_dir = 0;
  int                   pop_index = -1;
  int                   nseg, nline;
  int                   push_line;

  if (*line_index >= (nline = _XmStrLineCountGet(string)))
    return;

  if (_XmStrImplicitLine(string)) {
    line = _XmStrEntry(string)[*line_index];
    nseg = _XmEntrySegmentCountGet(line);
  } else
    nseg = _XmStrEntryCount(string);

  if (*seg_index >= nseg) {
    (*line_index)++;
    (*seg_index) = 0;
    if (*line_index >= nline)
      return;
  }

  if (*seg_index > 0)
    if (_XmStrImplicitLine(string))
      last = _XmEntrySegmentGet(line)[*seg_index-1];
    else
      last = (_XmStringNREntry)_XmStrEntry(string)[*seg_index-1];
  else
    last = NULL;

  while (*line_index < nline) {
    if (_XmStrImplicitLine(string)) {
      line = _XmStrEntry(string)[*line_index];
      nseg = _XmEntrySegmentCountGet(line);
    } else
      nseg = _XmStrEntryCount(string);

    while (*seg_index < nseg) {

      if (_XmStrImplicitLine(string))
	seg = _XmEntrySegmentGet(line)[*seg_index];
      else
	seg = (_XmStringNREntry)_XmStrEntry(string)[*seg_index];

      if (_XmEntryPushGet((_XmStringEntry)seg) &&
	  !_XmEntryPopGet((_XmStringEntry)seg)) {
	push_line = *line_index;
	(*seg_index)++;
	_XmEntryLayoutSet(seg, p_direction,
			  (long)_XmEntryPushGet((_XmStringEntry)seg));
	_XmEntryLayoutDepthSet(seg, p_direction, (long)++depth);
	recursive_layout(string, line_index, seg_index,
			 _XmEntryPushGet((_XmStringEntry)seg),
			 p_direction, depth);

	if (XmDirectionMatch(_XmEntryPushGet((_XmStringEntry)seg),direction)) {
	  /* False push - treat as normal case */
	  if (XmDirectionMatch(_XmEntryPushGet((_XmStringEntry)seg),
			       XmLEFT_TO_RIGHT)) {
	    if (last) {
	      _XmEntryRightSet(last, p_direction, seg);
	      _XmEntryLeftSet(seg, p_direction, last);
	      _XmEntryDirtySet(last, _XmSCANNING_CACHE, p_direction, False);
	    }
	  } else
	    if (last) {
	      _XmEntryLeftSet(last, p_direction, seg);
	      _XmEntryRightSet(seg, p_direction, last);
	      _XmEntryDirtySet(last, _XmSCANNING_CACHE, p_direction, False);
	    }
	  if (_XmStrImplicitLine(string)) {
	    if (*line_index < nline) {
	      line = _XmStrEntry(string)[*line_index];
	      nseg = _XmEntrySegmentCountGet(line);
	      last = _XmEntrySegmentGet(line)[*seg_index];
	    } else {
	      line = NULL;
	      nseg = 0;
	      last = NULL;
	    }
	  } else {
	    nseg = _XmStrEntryCount(string);
	    last = (_XmStringNREntry)_XmStrEntry(string)[*seg_index];
	  }
	} else if (*line_index == push_line) {
	  /* connect segment before push with pop segment
	   */
	  if (_XmStrImplicitLine(string))
	    seg2 = _XmEntrySegmentGet(line)[*seg_index];
	  else
	    seg2 = (_XmStringNREntry)_XmStrEntry(string)[*seg_index];
	  if (XmDirectionMatch(_XmEntryPushGet((_XmStringEntry)seg),
			       XmLEFT_TO_RIGHT)) {
	    if (last) {
	      _XmEntryLeftSet(last, p_direction, seg2);
	      _XmEntryRightSet(seg2, p_direction, last);
	      _XmEntryDirtySet(last, _XmSCANNING_CACHE, p_direction, False);
	      _XmEntryDirtySet(seg2, _XmSCANNING_CACHE, p_direction, False);
	    }
	  } else {
	    if (last){
	      _XmEntryRightSet(last, p_direction, seg2);
	      _XmEntryLeftSet(seg2, p_direction, last);
	      _XmEntryDirtySet(last, _XmSCANNING_CACHE, p_direction, False);
	      _XmEntryDirtySet(seg2, _XmSCANNING_CACHE, p_direction, False);
	    }
	  }
	  last = seg;
	} else {
	  /* pop is on a different line
	   */
	  if (last && nseg > 0) {
	    /* connect last segment on line with the one before push
	     */
	    _XmStringNREntry conn_seg;
	    if (_XmStrImplicitLine(string))
	      conn_seg = _XmEntrySegmentGet(line)[nseg-1];
	    else
	      conn_seg = (_XmStringNREntry)_XmStrEntry(string)[nseg-1];
	    if (XmDirectionMatch(_XmEntryPushGet((_XmStringEntry)seg),
				 XmLEFT_TO_RIGHT)) {
	      while (_XmEntryRightGet(conn_seg,  p_direction))
		conn_seg =
		  (_XmStringNREntry)_XmEntryRightGet(conn_seg, p_direction);
	      _XmEntryLeftSet(last, p_direction, conn_seg);
	      _XmEntryRightSet(conn_seg, p_direction, last);
	      _XmEntryDirtySet(last, _XmSCANNING_CACHE, p_direction, False);
	      _XmEntryDirtySet(conn_seg, _XmSCANNING_CACHE,
			       p_direction, False);
	    } else {
	      while (_XmEntryLeftGet(conn_seg, p_direction))
		conn_seg =
		  (_XmStringNREntry)_XmEntryLeftGet(conn_seg, p_direction);
	      _XmEntryRightSet(last, p_direction, conn_seg);
	      _XmEntryLeftSet(conn_seg, p_direction, last);
	      _XmEntryDirtySet(last, _XmSCANNING_CACHE, p_direction, False);
	      _XmEntryDirtySet(conn_seg, _XmSCANNING_CACHE,
			       p_direction, False);
	    }
	    last = NULL;
	  }
	  if (_XmStrImplicitLine(string)) {
	    if (*line_index < nline) {
	      line = _XmStrEntry(string)[*line_index];
	      nseg = _XmEntrySegmentCountGet(line);
	    } else {
	      line = NULL;
	      nseg = 0;
	    }
	  } else if (*line_index > 0)
	    {
	      line = NULL;
	      nseg = 0;
	    }
	  else
	    nseg = _XmStrEntryCount(string);
	  /* save these things till we return from this level
	   */
	  pop_index = *seg_index;
	  pop_dir = _XmEntryPushGet((_XmStringEntry)seg);
	}
	(*seg_index)++;
      } else if (!_XmEntryPushGet((_XmStringEntry)seg) &&
		 _XmEntryPopGet((_XmStringEntry)seg)) {
	/* attach this segment to the previous one */
	if (last) {
	  if (XmDirectionMatch(direction, XmLEFT_TO_RIGHT)) {
	    _XmEntryLeftSet(seg, p_direction, last);
	    _XmEntryRightSet(last, p_direction, seg);
	  } else {
	    _XmEntryRightSet(seg, p_direction, last);
	    _XmEntryLeftSet(last, p_direction, seg);
	  }
	  _XmEntryDirtySet(last, _XmSCANNING_CACHE, p_direction, False);
	}
	_XmEntryLayoutSet(seg, p_direction, (long)direction);
	_XmEntryLayoutDepthSet(seg, p_direction, (long)depth);
	/* If we had a pop with its matching push on another line,
	   now we know what will be adjacent to it in the layout
	   If we popped from left-to-right, we will attach the leftmost
	   segment in the previous level with the rightmost segment in
	   the current level. Vice versa for right-to-left.
	   */
	if (pop_index >= 0 && pop_index != *seg_index) {
	  if (_XmStrImplicitLine(string))
	    last = _XmEntrySegmentGet(line)[pop_index];
	  else
	    last = (_XmStringNREntry)_XmStrEntry(string)[pop_index];
	  if (XmDirectionMatch(pop_dir, XmLEFT_TO_RIGHT)) {
	    while (_XmEntryLeftGet(last, p_direction))
	      last = (_XmStringNREntry)_XmEntryLeftGet(last, p_direction);
	    while (_XmEntryRightGet(seg, p_direction))
	      seg = (_XmStringNREntry)_XmEntryRightGet(seg, p_direction);
	    _XmEntryRightSet(seg, p_direction, last);
	    _XmEntryLeftSet(last, p_direction, seg);
	    _XmEntryDirtySet(last, _XmSCANNING_CACHE, p_direction, False);
	    _XmEntryDirtySet(seg, _XmSCANNING_CACHE, p_direction, False);
	  } else {
	    while (_XmEntryRightGet(last, p_direction))
	      last = (_XmStringNREntry)_XmEntryRightGet(last, p_direction);
	    while (_XmEntryLeftGet(seg, p_direction))
	      seg = (_XmStringNREntry)_XmEntryLeftGet(seg, p_direction);
	    _XmEntryLeftSet(seg, p_direction, last);
	    _XmEntryRightSet(last, p_direction, seg);
	    _XmEntryDirtySet(last, _XmSCANNING_CACHE, p_direction, False);
	    _XmEntryDirtySet(seg, _XmSCANNING_CACHE, p_direction, False);
	  }
	}
	return;
      } else {
	/* Default: No push/pop or push, pop in same segment */
	/* set up connection */
	if (last) {
	  if (XmDirectionMatch(direction, XmLEFT_TO_RIGHT)) {
	    _XmEntryLeftSet(seg, p_direction, last);
	    _XmEntryRightSet(last, p_direction, seg);
	  } else {
	    _XmEntryRightSet(seg, p_direction, last);
	    _XmEntryLeftSet(last, p_direction, seg);
	  }
	  _XmEntryDirtySet(last, _XmSCANNING_CACHE, p_direction, False);
	}
	last = seg;
	_XmEntryLayoutSet(seg, p_direction, (long)direction);
	_XmEntryLayoutDepthSet(seg, p_direction, (long)depth);
	(*seg_index)++;
      }
    }
    /* If we had a pop with its matching push on another line,
       now we know what will be adjacent to it in the layout,
       namely the last segment on this line.
       */
    if (pop_index >= 0 && last) {
      if (pop_index != *seg_index-1)  {
	if (_XmStrImplicitLine(string))
	  seg2 = _XmEntrySegmentGet(line)[pop_index];
	else
	  seg2 = (_XmStringNREntry)_XmStrEntry(string)[pop_index];
	if (XmDirectionMatch(pop_dir, XmLEFT_TO_RIGHT)) {
	  _XmEntryRightSet(seg2, p_direction, last);
	  _XmEntryLeftSet(last, p_direction, seg2);
	} else {
	  _XmEntryLeftSet(seg2, p_direction, last);
	  _XmEntryRightSet(last, p_direction, seg2);
	}
	_XmEntryDirtySet(last, _XmSCANNING_CACHE, p_direction, False);
	_XmEntryDirtySet(seg2, _XmSCANNING_CACHE, p_direction, False);
      }
      pop_index = -1;
    }
    (*line_index)++;
    (*seg_index) = 0;
    last = NULL;
  }
}


void
_XmStringLayout(_XmString string,
                XmDirection direction)
{
  int seg_index=0, line_index=0;
  _XmStringEntry        *line;
  _XmStringNREntry 	seg;
  Boolean needs_recompute = False;

  if (!_XmStrMultiple(string))
    return;

  if (_XmStrEntryCount(string) && (line = _XmStrEntryGet(string)) && *line) {
    if (_XmEntrySegmentCountGet(*line)) {
      seg = _XmEntrySegmentGet(*line)[0];
      needs_recompute = _XmEntryDirtyGet(seg, _XmSCANNING_CACHE, direction);
    }
  }

  if (!needs_recompute)
    return;

  while (line_index < _XmStrEntryCount(string) && (line = _XmStrEntryGet(string))) {
    while (seg_index < _XmEntrySegmentCountGet(line[line_index])) {
      seg = _XmEntrySegmentGet(line[line_index])[seg_index];
      if (_XmEntrySegmentCountGet(line[line_index]) > 1) {
	_XmEntryDirtySet(seg, _XmSCANNING_CACHE, direction, True);
	_XmEntryLeftSet(seg, direction, NULL);
	_XmEntryRightSet(seg, direction, NULL);
      }
      seg_index++;
    }
    seg_index = 0;
    line_index++;
  }

  line_index = seg_index = 0;
  recursive_layout(string, &line_index, &seg_index,
		   direction, direction, 0);

  /* if there are pops w/o matching pushes, ignore them */
  line = _XmStrEntryGet(string);
  while (line_index < _XmStrLineCountGet(string) &&
	 seg_index < _XmEntrySegmentCountGet(line[line_index]))
    {
      seg = _XmEntrySegmentGet(line[line_index])[seg_index];
      _XmEntryPopSet(seg, False);
      recursive_layout(string, &line_index, &seg_index,
		       direction, direction, 0);
      _XmEntryPopSet(seg, True);
    }
}

/****************************************************************
 *  What's the layout direction at the end of the line?
 ****************************************************************/
static void
last_direction(_XmStringEntry line,
	       int *index,
	       XmDirection *direction)
{
  _XmStringNREntry      seg;
  XmDirection         sub_dir = *direction;

  while (*index < _XmEntrySegmentCountGet(line))
    {
      seg = _XmEntrySegmentGet(line)[*index];
      if (_XmEntryPushGet((_XmStringEntry)seg) &&
	  !_XmEntryPopGet((_XmStringEntry)seg)) {
        sub_dir = _XmEntryPushGet((_XmStringEntry)seg);
        (*index)++;
        last_direction(line, index, &sub_dir);
        if (*index < _XmEntrySegmentCountGet(line))
          sub_dir = *direction;
        (*index)++;
      } else if (!_XmEntryPushGet((_XmStringEntry)seg) &&
		 _XmEntryPopGet((_XmStringEntry)seg))
         return;
      else
        (*index)++;
    }
  *direction = sub_dir;
}

/*
 * Draw a single internal TCS line
 */
static void
DrawLine(
	 Display *d,
	 Window w,
	 Screen **pscreen,
	 int x,
	 int y,
	 _XmStringEntry line,
	 XmRendition *scr_rend,
	 XmRendition base,
	 XmRenderTable rendertable,
	 XmDirection prim_dir,
	 Boolean image,
	 _XmString *underline,
	 Dimension descender,
	 Boolean opt,
	 Dimension opt_width,
	 Dimension opt_height
	 )
{
  int				i, prev_val, val, offset;
  XmTabList			tl = NULL;
  XmTab				tab;
  unsigned short		tab_cnt;

  /* Absolute tabs use this as left margin */
  offset = x;

  if (opt)
    {
      /*
       * This is optimized; build an optimized segment and call the drawing
       * routine.
       */
      _XmStringUnoptSegRec	segm;
      _XmString 		optline = (_XmString)line;

      _XmEntryInit((_XmStringEntry)&segm, XmSTRING_ENTRY_OPTIMIZED);

      _XmEntryTagIndex(&segm) = _XmStrTagIndex(optline);

      _XmEntryByteCountSet(&segm, _XmStrByteCount(optline));
      _XmEntryTextTypeSet(&segm,
			  (XmTextType) _XmStrTextType(optline));
      _XmEntryTextSet((_XmStringEntry)&segm, _XmStrText(optline));
       if (_XmStrDirection(optline) != XmSTRING_DIRECTION_UNSET)
	 _XmEntryDirectionSet((_XmStringEntry)&segm, _XmStrDirection(optline));
       else
	 _XmEntryDirectionSet((_XmStringEntry)&segm,
			      XmDirectionToStringDirection(prim_dir));

      if (_XmStrRendBegin(optline))
	  _XmEntryRendIndex(&segm) = _XmStrRendIndex(optline);

      if (*scr_rend != NULL) tl = _XmRendTabs(*scr_rend);

      tab = (tl == NULL) ? NULL : _XmTabLStart(tl);

      prev_val = x;
      tab_cnt = 0;

      if ((tab != NULL) &&
	  (_XmEntryTabsGet((_XmStringEntry)&segm) != 0) &&
	  (tab_cnt < _XmTabLCount(tl)) &&
	  _XmEntryDirectionGet((_XmStringEntry)&segm) !=
	  XmSTRING_DIRECTION_R_TO_L)
	{
	  for (i = 0;
	       (i < _XmEntryTabsGet((_XmStringEntry)&segm)) &&
	       (tab_cnt < _XmTabLCount(tl));
	       i++, tab = _XmTabNext(tab), tab_cnt++)
	    {
	      val = TabVal(d, pscreen, w, tab);
	      if (_XmTabModel(tab) == XmABSOLUTE)
		{
		  x = val + offset;
		  prev_val = x;
		}
	      else				  /* XmRELATIVE */
		{
		  x = prev_val + val;
		  prev_val += val;
		}
	    }
	}

      _XmStringDrawSegment(d, w, x, y, opt_width, opt_height,
			   (_XmStringNREntry)&segm, *scr_rend, rendertable,
			   image, underline, descender);
    }
  else {
    _XmStringNREntry 	seg;
    int			seg_index = 0;
    Boolean		ok;
    Dimension		width, height;
    Boolean             set_direction = False;
    XmDirection         lay_dir = prim_dir; /* layout direction of this line */

    seg = _XmEntrySegmentGet(line)[seg_index];

    if (_XmEntryType(seg) != XmSTRING_ENTRY_OPTIMIZED) {
      lay_dir = _XmEntryLayoutGet(seg, prim_dir);
    }

    if (XmDirectionMatch(lay_dir, XmLEFT_TO_RIGHT)) {

      if (_XmEntryType(seg) != XmSTRING_ENTRY_OPTIMIZED) {
	while (_XmEntryLeftGet(seg, prim_dir) != NULL)
	  seg = (_XmStringNREntry)_XmEntryLeftGet(seg, prim_dir);
      }

      if (_XmEntryDirectionGet((_XmStringEntry)seg) ==
	  XmSTRING_DIRECTION_UNSET) {
	_XmEntryDirectionSet((_XmStringEntry)seg,
			     XmDirectionToStringDirection(prim_dir));
	set_direction = True;
      }

      ok = SpecifiedSegmentExtents((_XmStringEntry)seg, rendertable, scr_rend,
				   base, XmSTRING_MIDDLE_SEG,
				   &width, &height, NULL, NULL);

      if (*scr_rend != NULL) tl = _XmRendTabs(*scr_rend);

      tab = (tl == NULL) ? NULL : _XmTabLStart(tl);

      prev_val = x;
      tab_cnt = 0;

      while (seg != NULL)
	{
	  /* If this segment is tabbed, set x accordingly. */
	  if ((tab != NULL) &&
	      (_XmEntryTabsGet((_XmStringEntry)seg) != 0) &&
	      (tab_cnt < _XmTabLCount(tl)))
	    {
	      int start_x = x;

	      for (i = 0;
		   (i < _XmEntryTabsGet((_XmStringEntry)seg)) &&
		   (tab_cnt < _XmTabLCount(tl));
		   i++, tab = _XmTabNext(tab), tab_cnt++)
		{
		  val = TabVal(d, pscreen, w, tab);

		  if (_XmTabModel(tab) == XmABSOLUTE)
		    x = MAX(x, (val + offset));
		  else				  /* XmRELATIVE */
		    x = MAX(x, prev_val + val);

		  prev_val = x;
		}

	      _XmStringDrawLining(d, w, start_x, y,
				  (x - start_x), height, descender,
				  *scr_rend, XmUNSPECIFIED_PIXEL,
				  XmHIGHLIGHT_NORMAL, FALSE);
	    }

	  if (ok)
	    _XmStringDrawSegment(d, w, x, y, width, height, seg, *scr_rend,
				 rendertable, image, underline, descender);

	  x += width;

	  if (set_direction) {
	    _XmEntryDirectionSet((_XmStringEntry)seg,
				 XmSTRING_DIRECTION_UNSET);
	    set_direction = False;
	  }

	  if (_XmEntryType(seg) != XmSTRING_ENTRY_OPTIMIZED) {
	    seg = (_XmStringNREntry)_XmEntryRightGet(seg, prim_dir);
	  } else {
	    seg_index++;
	    seg = (seg_index < _XmEntrySegmentCountGet(line) ?
		   _XmEntrySegmentGet(line)[seg_index] :
		   NULL);
	  }
	  if (seg != NULL) {
	    if (_XmEntryDirectionGet((_XmStringEntry)seg) ==
		XmSTRING_DIRECTION_UNSET) {
	      _XmEntryDirectionSet((_XmStringEntry)seg,
				   XmDirectionToStringDirection(prim_dir));
	      set_direction = True;
	    }
	    ok = SpecifiedSegmentExtents((_XmStringEntry)seg, rendertable,
					 scr_rend,
					 base, XmSTRING_MIDDLE_SEG,
					 &width, &height, NULL, NULL);
	  }
	}
    } else {
      if (_XmEntryType(seg) != XmSTRING_ENTRY_OPTIMIZED) {
	while (_XmEntryRightGet(seg, prim_dir) != NULL)
	  seg = (_XmStringNREntry)_XmEntryRightGet(seg, prim_dir);
      }

      if (_XmEntryDirectionGet((_XmStringEntry)seg) ==
	  XmSTRING_DIRECTION_UNSET) {
	_XmEntryDirectionSet((_XmStringEntry)seg,
			     XmDirectionToStringDirection(prim_dir));
	set_direction = True;
      }

      ok = SpecifiedSegmentExtents((_XmStringEntry)seg, rendertable, scr_rend,
				   base, XmSTRING_MIDDLE_SEG,
				   &width, &height, NULL, NULL);

      if (*scr_rend != NULL) tl = _XmRendTabs(*scr_rend);

      tab = (tl == NULL) ? NULL : _XmTabLStart(tl);

      x += opt_width;
      offset = prev_val = x;
      tab_cnt = 0;

      while (seg != NULL)
	{
	  /* If this segment is tabbed, set x accordingly. */
	  if ((tab != NULL) &&
	      (_XmEntryTabsGet((_XmStringEntry)seg) != 0) &&
	      (tab_cnt < _XmTabLCount(tl)))
	    {
	      int start_x = x;

	      for (i = 0;
		   (i < _XmEntryTabsGet((_XmStringEntry)seg)) &&
		   (tab_cnt < _XmTabLCount(tl));
		   i++, tab = _XmTabNext(tab), tab_cnt++)
		{
		  val = TabVal(d, pscreen, w, tab);

		  if (_XmTabModel(tab) == XmABSOLUTE)
		    x = MIN(x, offset - val);
		  else				  /* XmRELATIVE */
		    x = MIN(x, prev_val - val);

		  prev_val = x;
		}

	      _XmStringDrawLining(d, w, x, y,
				  (start_x - x), height, descender,
				  *scr_rend, XmUNSPECIFIED_PIXEL,
				  XmHIGHLIGHT_NORMAL, FALSE);
	    }

	  x -= width;

	  if (ok)
	    _XmStringDrawSegment(d, w, x, y, width, height, seg, *scr_rend,
				 rendertable, image, underline, descender);

	  if (set_direction) {
	    _XmEntryDirectionSet((_XmStringEntry)seg,
				 XmSTRING_DIRECTION_UNSET);
	    set_direction = False;
	  }

	  if (_XmEntryType(seg) != XmSTRING_ENTRY_OPTIMIZED) {
	    seg = (_XmStringNREntry)_XmEntryLeftGet(seg, prim_dir);
	  } else {
	    seg_index++;
	    seg = (seg_index < _XmEntrySegmentCountGet(line) ?
		   _XmEntrySegmentGet(line)[seg_index] :
		   NULL);
	  }
	  if (seg != NULL) {
	    if (_XmEntryDirectionGet((_XmStringEntry)seg) ==
		XmSTRING_DIRECTION_UNSET) {
	      _XmEntryDirectionSet((_XmStringEntry)seg,
				   XmDirectionToStringDirection(prim_dir));
	      set_direction = True;
	    }
	    ok = SpecifiedSegmentExtents((_XmStringEntry)seg, rendertable,
					 scr_rend,
					 base, XmSTRING_MIDDLE_SEG,
					 &width, &height, NULL, NULL);
	  }
	}
    }
  }
}

/*
 * calculate the alignment, position and clipping for the string
 */
static void
_calc_align_and_clip(
        Display *d,
	Window w,
        GC gc,
        Position *x,
        Position y,
        Dimension width,
        int line_width,
        int line_height,
        unsigned char lay_dir,
        XRectangle *clip,
        unsigned char align,
        int descender,
        int *restore,
	XmFontType font_type)
{

    Boolean l_to_r = XmDirectionMatch(lay_dir, XmSTRING_DIRECTION_L_TO_R);


    switch (align)
    {
    	case XmALIGNMENT_BEGINNING:
	    if ( ! l_to_r) *x += width - line_width;
	    break;

    	case XmALIGNMENT_CENTER:
	    *x += Half (width) - Half (line_width);
	    break;

    	case XmALIGNMENT_END :
	    if (l_to_r)
	    	*x += width - line_width;
	    break;
   }

    if ((clip != NULL) && ( ! *restore))

        if (((*x) <= clip->x + clip->width) &&
            (clip->x <= (*x) + line_width) &&
            (y - line_height + descender <= clip->y + clip->height) &&
            (clip->y <= y + descender))
	{
	    *restore = TRUE;
#if USE_XFT
            if (font_type == XmFONT_IS_XFT)
	      _XmXftSetClipRectangles(d, w, 0, 0, clip, 1);
#endif
            XSetClipRectangles (d, gc, 0, 0, clip, 1, YXBanded);
	}
}

/*
 * draw a complete internal format TCS
 */
static void
_draw(
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
        Boolean image,
        _XmString underline )
{
  static XmRendition	rend = NULL;

  if (!string) return;

  _XmProcessLock();
  if (rend == NULL) rend = XmRenditionCreate(NULL, XmS, NULL, 0);

  _XmRendDisplay(rend) = d;
  _XmRendGC(rend) = gc;
  _XmRendTags(rend) = NULL;
  _XmRendTagCount(rend) = 0;

  _render(d, w, rendertable, rend, string, x, y, width,
	  align, lay_dir, image, underline, clip);
  _XmProcessUnlock();
}

/*
 * render a complete internal format TCS
 */
static void
_render(Display *d,
        Drawable w,
        XmRenderTable rendertable,
	XmRendition rend,
        _XmString string,
        Position x,
        Position y,
        Dimension width,
        unsigned char align,
        unsigned char lay_dir,
        Boolean image,
	_XmString underline,
        XRectangle *clip)
{
  Position base_x = x, draw_x;
  Dimension line_width, line_height, ascender = 0, descender = 0;
  _XmStringEntry line;
  int i;
  int restore_clip = FALSE;
  _XmRenditionRec	scratch1, scratch2;
  _XmRendition		tmp1, tmp2;
  XmRendition		rend1, rend2;
  GC			gc;
  Screen		*screen = NULL;

  if (!string) return;

  tmp1 = &scratch1;
  bzero((char *)tmp1, sizeof(_XmRenditionRec));
  rend1 = &tmp1;
  tmp2 = &scratch2;
  bzero((char *)tmp2, sizeof(_XmRenditionRec));
  rend2 = &tmp2;

  _XmRendDisplay(rend1) = _XmRendDisplay(rend2) = d;
  gc = _XmRendGC(rend1) = _XmRendGC(rend2) = _XmRendGC(rend);
  _XmRendTags(rend1) = _XmRendTags(rend2) = NULL;
  _XmRendTagCount(rend1) = _XmRendTagCount(rend2) = 0;

  if (lay_dir <= 1) /* got passed XmStringDirection value */
    lay_dir = XmStringDirectionToDirection(lay_dir);

  if (_XmStrOptimized(string))
    {
      OptLineMetrics(rendertable, string, &rend2, rend,
		     &line_width, &line_height, &ascender, &descender);
	y += ascender;

        if (line_width != 0)
          {
            draw_x = base_x ; /* most left position */
            _calc_align_and_clip( d, w, gc, &draw_x, y, width, line_width,
                                line_height, lay_dir, clip, align, descender,
                                &restore_clip, _XmRendFontType(rend2));

            DrawLine(d, w, &screen, draw_x, y, (_XmStringEntry)string,
		     &rend2, rend, rendertable, lay_dir, image,
		     &underline, descender, TRUE, line_width, line_height);
          }
        y += descender ;      /* go to bottom of this line */
      }
  else {
    XmDirection 		direction = lay_dir;
    int 			val;
    _XmStringArraySegRec	array_seg;

    _XmStringLayout(string, lay_dir);

    for (i = 0; i < _XmStrLineCountGet(string); i++)
      {
	if (_XmStrImplicitLine(string))
	  {
	    line = _XmStrEntry(string)[i];
	  }
	else
	  {
	    _XmEntryType(&array_seg) = XmSTRING_ENTRY_ARRAY;
	    _XmEntrySegmentCount(&array_seg) = _XmStrEntryCount(string);
	    _XmEntrySegment(&array_seg) = (_XmStringNREntry *)_XmStrEntry(string);
	    line = (_XmStringEntry)&array_seg;
	  }

	/* width, height, ascent, descent of this line */
	LineMetrics(line, rendertable, &rend1, rend, lay_dir,
		    &line_width, &line_height, &ascender, &descender);

	y += ascender;

	if (line_width != 0)
	  {
	    draw_x = base_x;			  /* most left position */

	    _calc_align_and_clip(d, w, gc, &draw_x, y, width, line_width,
	            line_height, direction, clip, align, descender,
				 &restore_clip, _XmRendFontType(rend1));

	    DrawLine(d, w, &screen, draw_x, y, line, &rend1, rend,
		     rendertable, lay_dir, image, &underline,
		     descender, FALSE, line_width, line_height);
	    val = 0;
	    last_direction((_XmStringEntry)line, &val, &direction);
	    if (val < _XmEntrySegmentCountGet(line))
	      /* found an 'unmatched' pop */
	      direction = lay_dir;
	  }

	y += descender;			  /* go to bottom of this line */
      }
  }
  if (restore_clip) {
#if USE_XFT
	  if (_XmRendFontType((_XmStrOptimized(string)) ? rend2 : rend1) == XmFONT_IS_XFT) {
		  XftDraw *draw = _XmXftDrawCreate(d, w);
		  XftDrawSetClip(draw, NULL);
	  } else
#endif
		  XSetClipMask (d, gc, None);
  }

  if (_XmRendTags(rend1) != NULL) XtFree((char *)_XmRendTags(rend1));
  if (_XmRendTags(rend2) != NULL) XtFree((char *)_XmRendTags(rend2));
}

void
_XmStringRender(Display *d,
		Drawable w,
		XmRenderTable rendertable,
		XmRendition rend,
		_XmString string,
		Position x,
		Position y,
		Dimension width,
		unsigned char align,
		unsigned char lay_dir
		)
{
  _render(d, w, rendertable, rend, string, x, y, width,
	  align, lay_dir, FALSE, NULL, NULL);
}

/*
 * add a new segment to a particular line in an XmString
 */
void
_XmStringSegmentNew(
        _XmString string,
        int line_index,
        _XmStringEntry value,
	int copy)
{
    _XmStringEntry line;
    _XmStringEntry seg;
    int sc;
    int lc = _XmStrEntryCount(string);

    if (lc == 0 || lc-1 < line_index) {
      _XmStrEntry(string) = (_XmStringEntry *)
	XtRealloc((char *) _XmStrEntry(string),
		  sizeof(_XmStringEntry) * (lc + 1));
      _XmStrEntryCount(string)++;
      if (line_index > lc) line_index = lc;
      if (copy)
	line = _XmStringEntryCopy(value);
      else {
	line = value;
      }
      _XmStrEntry(string)[line_index] = line;
    } else {
      line  = _XmStrEntry(string)[line_index];
      if (!_XmEntryMultiple(line)) {
	/* have to create an array entry */
	sc = 1;
	seg = line;
	_XmEntryCreate(line, XmSTRING_ENTRY_ARRAY);
	_XmEntrySegmentCount(line) = sc;
	_XmEntrySoftNewlineSet(line, _XmEntrySoftNewlineGet(seg));
	_XmEntrySegment(line) = (_XmStringNREntry *)
	  XtMalloc(sizeof(_XmStringEntry) * 2);
	_XmEntrySegment(line)[0] = (_XmStringNREntry)seg;
	_XmStrEntry(string)[line_index] = line;
	_XmStrImplicitLine(string) = True;
      } else {
	sc  = _XmEntrySegmentCount(line);
	_XmEntrySegment(line) = (_XmStringNREntry *)
	  XtRealloc((char *) _XmEntrySegment(line),
		    sizeof(_XmStringEntry) * (sc+1));
      }
      seg = (copy ? _XmStringEntryCopy(value) : value);
      _XmEntrySegment(line)[sc] = (_XmStringNREntry)seg;
      _XmEntrySegmentCount(line)++;
    }
}

static _XmString
_XmStringOptCreate(
        unsigned char *c,
        unsigned char *end,
        unsigned short textlen,
        Boolean havetag,
        unsigned int tag_index )
{
  _XmString      string;
  char          *tag = NULL;
  unsigned short length;

  _XmStrCreate(string, XmSTRING_OPTIMIZED, textlen);
  if (havetag)
    {
      _XmStrTagIndex((_XmString)string) = tag_index ;
    }
  else
    {
      tag = XmFONTLIST_DEFAULT_TAG;
      _XmStrTagIndex((_XmString)string) =
	_XmStringIndexCacheTag((char *) tag, XmSTRING_TAG_STRLEN);
    }

  while (c < end)
    {
      length = _read_asn1_length (c);

      switch (*c)
	{
	case XmSTRING_COMPONENT_RENDITION_BEGIN:
	  _XmStrRendIndex(string) =
	    _XmStringIndexCacheTag((char *)(c + _asn1_size(length)),
				   (int)length);
	  _XmStrRendBegin(string) = TRUE;
	  break;

        case XmSTRING_COMPONENT_LOCALE:
	  _XmStrTextType((_XmString)string) = XmMULTIBYTE_TEXT;
	  break;

        case XmSTRING_COMPONENT_TAG:
	  _XmStrTextType((_XmString)string) = XmCHARSET_TEXT;
	  break;

	case XmSTRING_COMPONENT_TAB:
	  _XmStrTabs(string)++;
	  break;

	case XmSTRING_COMPONENT_DIRECTION:		/* record dir */
	  _XmStrDirection((_XmString) string) =
	    ((XmStringDirection)*(c + _asn1_size(length)));
	  break;

	case XmSTRING_COMPONENT_TEXT:
	  _XmStrTextType((_XmString)string) = XmCHARSET_TEXT;
	  memcpy(_XmStrText((_XmString)string), (c + _asn1_size(length)),
		 textlen);
	  break;

	case XmSTRING_COMPONENT_LOCALE_TEXT:
	  _XmStrTextType((_XmString)string) = XmMULTIBYTE_TEXT;
	  memcpy(_XmStrText((_XmString)string), (c + _asn1_size(length)),
		 textlen);
	  break;

	case XmSTRING_COMPONENT_RENDITION_END:
	  _XmStrRendIndex(string) =
	    _XmStringIndexCacheTag((char *)(c + _asn1_size(length)),
				   (int)length);
	  _XmStrRendEnd(string) = TRUE;
	  break;

	case XmSTRING_COMPONENT_SEPARATOR:		/* start new line */
	  _XmStrFree ((char *) string);
	  return (NULL);
	  /* break; */

	default:
	  break;
	}

      c += length + _asn1_size(length);
    }

  return((_XmString) string);
}

static void
finish_segment(_XmString str,
	       _XmStringUnoptSeg seg,
	       int *lc,
	       int *sc,
	       Boolean *unopt,
	       XmStringDirection dir)
{
  _XmStringEntry opt_seg;

  _XmEntryDirectionSet((_XmStringEntry)seg, dir);

  if (!*unopt &&
      (opt_seg = EntryCvtToOpt((_XmStringEntry)seg)))
    _XmStringSegmentNew(str, _XmStrImplicitLine(str) ? *lc : *sc,
		opt_seg, False);
  else
    _XmStringSegmentNew(str, _XmStrImplicitLine(str) ? *lc : *sc,
		(_XmStringEntry)seg, True);
  (*sc)++;
  *unopt = False;
  _XmEntryInit((_XmStringEntry)seg, XmSTRING_ENTRY_UNOPTIMIZED);
}


static _XmString
_XmStringNonOptCreate(
        unsigned char *c,
        unsigned char *end,
        Boolean havetag )
{
  int lc, sc;
  _XmStringUnoptSegRec seg;
  unsigned short length;
  _XmString string ;
  char *tag = NULL;
  Boolean needs_unopt = False;
  Boolean txt_seen = False;
  Boolean push_seen = False;
  Boolean pop_seen = False;
  Boolean need_finish = False;
  int rend_cnt;
  int tab_cnt;
  XmTextType prev_type = XmCHARSET_TEXT;
  XmStringDirection dir = XmSTRING_DIRECTION_UNSET;

  _XmStrCreate(string, XmSTRING_MULTIPLE_ENTRY, 0);
  _XmEntryInit((_XmStringEntry)&seg, XmSTRING_ENTRY_UNOPTIMIZED);

  if (!havetag)
    {
      tag = XmFONTLIST_DEFAULT_TAG;
      _XmUnoptSegTag(&seg) =
	_XmStringCacheTag((char *) (tag), XmSTRING_TAG_STRLEN);
    }
  _XmEntryDirectionSet((_XmStringEntry)&seg, XmSTRING_DIRECTION_L_TO_R);

  lc = sc = 0;

  while (c < end)
    {
      length = _read_asn1_length (c);
      need_finish = True;

      switch (*c)
	{
	case XmSTRING_COMPONENT_LAYOUT_PUSH:		/* record dir */
	  if ((txt_seen) || (push_seen))
	    {
	      finish_segment(string, &seg, &lc, &sc, &needs_unopt, dir);
	      push_seen = txt_seen = pop_seen = False;
	    }

	  needs_unopt = True;
	  push_seen = True;
	  _XmEntryPushSet(&seg, (XmDirection)*(c + _asn1_size(length)));
	  break;

	case XmSTRING_COMPONENT_RENDITION_BEGIN:
	  if (txt_seen)
	    {
	      finish_segment(string, &seg, &lc, &sc, &needs_unopt, dir);
	      push_seen = txt_seen = pop_seen = False;
	    }

	  rend_cnt = ++(_XmUnoptSegRendBeginCount(&seg));

	  if (rend_cnt > 1) needs_unopt = True;

	  _XmUnoptSegRendBegins(&seg) = (XmStringTag *)
	    XtRealloc((char *)_XmUnoptSegRendBegins(&seg), rend_cnt);

	  _XmUnoptSegRendBegins(&seg)[rend_cnt - 1] =
	    _XmStringCacheTag((char *)(c + _asn1_size(length)),
			      (int)length);
	  break;

        case XmSTRING_COMPONENT_LOCALE:
	  if (txt_seen)
	    {
	      finish_segment(string, &seg, &lc, &sc, &needs_unopt, dir);
	      push_seen = txt_seen = pop_seen = False;
	    }

	  _XmEntryTextTypeSet(&seg, XmMULTIBYTE_TEXT);
	  prev_type = XmMULTIBYTE_TEXT;
	  _XmUnoptSegTag(&seg) =
	    _XmStringCacheTag((char *)(c+_asn1_size(length)), (int)length);
	  break;

        case XmSTRING_COMPONENT_TAG:
	  if (txt_seen)
	    {
	      finish_segment(string, &seg, &lc, &sc, &needs_unopt, dir);
	      push_seen = txt_seen = pop_seen = False;
	    }

	  _XmEntryTextTypeSet(&seg, XmCHARSET_TEXT);
	  prev_type = XmCHARSET_TEXT;
	  _XmUnoptSegTag(&seg) =
	    _XmStringCacheTag((char *)(c+_asn1_size(length)), (int)length);
	  break;

	case XmSTRING_COMPONENT_TAB:
	  if (txt_seen)
	    {
	      finish_segment(string, &seg, &lc, &sc, &needs_unopt, dir);
	      push_seen = txt_seen = pop_seen = False;
	    }

	  tab_cnt = _XmEntryTabsGet((_XmStringEntry)&seg);
	  if (++tab_cnt > 7) needs_unopt = True;
	  _XmEntryTabsSet(&seg, tab_cnt);
	  break;

	case XmSTRING_COMPONENT_DIRECTION:		/* record dir */
	  if (txt_seen)
	    {
	      finish_segment(string, &seg, &lc, &sc, &needs_unopt, dir);
	      push_seen = txt_seen = pop_seen = False;
	    }

	  dir = (XmStringDirection)*(c + _asn1_size(length));
	  break;

	case XmSTRING_COMPONENT_WIDECHAR_TEXT:
	  if (txt_seen)
	    {
	      finish_segment(string, &seg, &lc, &sc, &needs_unopt, dir);
	      push_seen = txt_seen = pop_seen = False;
	    }

	  _XmEntryTextTypeSet(&seg, XmWIDECHAR_TEXT);
	  prev_type = XmWIDECHAR_TEXT;
	  /* Fall through */
	case XmSTRING_COMPONENT_LOCALE_TEXT:
	  if (txt_seen)
	    {
	      finish_segment(string, &seg, &lc, &sc, &needs_unopt, dir);
	      push_seen = txt_seen = pop_seen = False;
	    }

	  /* from above */
	  if (_XmEntryTextTypeGet((_XmStringEntry)&seg) != XmWIDECHAR_TEXT)
	    {
	      _XmEntryTextTypeSet(&seg, XmMULTIBYTE_TEXT);
	      prev_type = XmMULTIBYTE_TEXT;
	    }
	  _XmUnoptSegTag(&seg) =
	    _XmStringCacheTag((char *) XmFONTLIST_DEFAULT_TAG,
			      XmSTRING_TAG_STRLEN);
	  /* Fall through to regular text. */
	case XmSTRING_COMPONENT_TEXT:
	  if (txt_seen)
	    {
	      push_seen = txt_seen = pop_seen = False;
	      finish_segment(string, &seg, &lc, &sc, &needs_unopt, dir);
	    }

	  if (_XmEntryTextTypeGet((_XmStringEntry)&seg) == XmNO_TEXT)
	    _XmEntryTextTypeSet(&seg, prev_type);

	  _XmEntryTextSet((_XmStringEntry)&seg, (c + _asn1_size(length)));
	  _XmUnoptSegByteCount(&seg) = length;

	  txt_seen = True;
	  break;

	case XmSTRING_COMPONENT_RENDITION_END:
	  txt_seen = True;

	  rend_cnt = ++(_XmUnoptSegRendEndCount(&seg));

	  if (rend_cnt > 1) needs_unopt = True;

	  _XmUnoptSegRendEnds(&seg) = (XmStringTag *)
	    XtRealloc((char *)_XmUnoptSegRendEnds(&seg), rend_cnt);

	  _XmUnoptSegRendEnds(&seg)[rend_cnt - 1] =
	    _XmStringCacheTag((char *)(c + _asn1_size(length)),
			      (int)length);
	  break;

	case XmSTRING_COMPONENT_LAYOUT_POP:
	  if (pop_seen)
	    {
	      finish_segment(string, &seg, &lc, &sc, &needs_unopt, dir);
	      push_seen = txt_seen = pop_seen = False;
	    }

	  needs_unopt = True;
	  txt_seen = True;
	  pop_seen = True;
	  _XmEntryPopSet(&seg, TRUE);
	  break;

	case XmSTRING_COMPONENT_SEPARATOR:              /* start new line */
	  finish_segment(string, &seg, &lc, &sc, &needs_unopt, dir);
	  need_finish = push_seen = txt_seen = pop_seen = False;

	  if (!_XmStrImplicitLine(string) && _XmStrEntryCount(string) > 1) {
	    /* need to move segments down one level */
	    _XmStringEntry line;
	    line = (_XmStringEntry)XtMalloc(sizeof(_XmStringArraySegRec));
	    _XmEntryType(line) = XmSTRING_ENTRY_ARRAY;
	    _XmEntrySoftNewlineSet(line, False);
	    _XmEntrySegmentCount(line) = _XmStrEntryCount(string);
	    _XmEntrySegment(line) = (_XmStringNREntry *)_XmStrEntry(string);
	    _XmStrEntry(string) =
	      (_XmStringEntry *)XtMalloc(sizeof(_XmStringEntry));
	    _XmStrEntry(string)[0] = line;
	    _XmStrEntryCount(string) = 1;
	  }
	  _XmStrImplicitLine(string) = True;
	  lc++;
	  break;

	default:
	  break;
	}

      c += length + _asn1_size(length);
    }

  if (need_finish) finish_segment(string, &seg, &lc, &sc, &needs_unopt, dir);

  return(string);
}

/*
 * Converts from ASN.1 formatted byte stream to XmString.
 */
XmString
XmCvtByteStreamToXmString(unsigned char *property)
{
  unsigned char       *c;
  unsigned char       *c_opt;
  unsigned char       *end;
  unsigned short      length;
  unsigned short      txtlength;
  XmString            string ;
  Boolean     	      continue_flag;
  Boolean     	      optimized;
  Boolean     	      havetag;
  Boolean	      begin_seen, end_seen;
  Boolean	      txt_seen;
  unsigned int 	      tag_index = TAG_INDEX_MAX;
  unsigned int 	      begin_index = REND_INDEX_MAX;
  unsigned int 	      end_index = REND_INDEX_MAX;
  unsigned char	      tab_cnt;

  _XmProcessLock();
  if (!property) {
      _XmProcessUnlock();
      return((XmString) NULL);
  }
  /* If property isn't an asn.1 conformant string, return NULL. */
  if (!_is_asn1(property)) {
      _XmProcessUnlock();
      return ((XmString) NULL);
  }

  c  = (unsigned char *) _read_header((unsigned char *) property);
  end = c + _read_string_length ((unsigned char *) property);
  if (c >= end) {
      _XmProcessUnlock();
      return ((_XmString) NULL);
  }

  /*
   * In order to build an optimized string, we have to see if this one
   * qualifies.  Do some preprocessing to see.
   * We also need to know if this CS contains a character set component,
   * so look for that too.
   */

  c_opt = c;
  continue_flag = TRUE;
  optimized = TRUE;
  txtlength = 0;		/* For strings with no text component. */
  havetag = FALSE;
  end_seen = begin_seen = FALSE;
  txt_seen = FALSE;
  tab_cnt = 0;

  while (continue_flag)
    {
      length = _read_asn1_length (c_opt);

      switch (*c_opt)
        {
	/* All non-optimized */
	case XmSTRING_COMPONENT_LAYOUT_PUSH:
	case XmSTRING_COMPONENT_LAYOUT_POP:
        case XmSTRING_COMPONENT_SEPARATOR: /* start new line */
          optimized = FALSE;
          break;

	case XmSTRING_COMPONENT_RENDITION_BEGIN:
	  if (begin_seen || txt_seen)
	    {
	      optimized = FALSE;
	      break;
	    }
	  else
	    {
	      begin_seen = TRUE;
	      begin_index =
		_XmStringIndexCacheTag((char *)(c_opt + _asn1_size(length)),
				       (int)length);
	      if (begin_index >= REND_INDEX_MAX) optimized = FALSE ;
	    }
	  break;

        case XmSTRING_COMPONENT_LOCALE:
        case XmSTRING_COMPONENT_TAG:
          tag_index = _XmStringIndexCacheTag
            ((char *) (c_opt + _asn1_size(length)), (int) length);

          if (txt_seen ||
	      (tag_index >= TAG_INDEX_MAX))
	    optimized = FALSE ;

          havetag = TRUE;
          break;

	case XmSTRING_COMPONENT_TAB:
	  if (++tab_cnt > 3) optimized = FALSE;
	  break;

	case XmSTRING_COMPONENT_DIRECTION:
	  if (txt_seen) optimized = FALSE;
	  break;

	case XmSTRING_COMPONENT_WIDECHAR_TEXT:
	  optimized = FALSE;
	  txtlength = length;
	  break;

        case XmSTRING_COMPONENT_LOCALE_TEXT:
	  /* Check the tag. */
          tag_index = _XmStringIndexCacheTag((char *)XmFONTLIST_DEFAULT_TAG,
					     XmSTRING_TAG_STRLEN);
          havetag = TRUE;

	  if ((txt_seen) ||
	      (tag_index >= TAG_INDEX_MAX))
	    {
	      optimized = FALSE;
	      break;
	    }

          /* Else fall through to text case. */
        case XmSTRING_COMPONENT_TEXT:
          if (txt_seen ||
	      (((c_opt + length + _asn1_size(length)) < end) ||
	       (length >= (1 << BYTE_COUNT_BITS))))
	    optimized = FALSE;

	  txtlength = length;
	  txt_seen = TRUE;

          break;

	case XmSTRING_COMPONENT_RENDITION_END:
	  if (end_seen)
	    {
	      optimized = FALSE;
	      break;
	    }
	  else
	    {
	      end_seen = TRUE;
	      txt_seen = TRUE;
	      end_index =
		_XmStringIndexCacheTag((char *)(c_opt + _asn1_size(length)),
				       (int)length);
	      if ((end_index >= REND_INDEX_MAX) ||
		  (end_index != begin_index))
		optimized = FALSE;
	    }
	  break;

        default:
          break;
        }

      c_opt += length + _asn1_size(length);
      if ((c_opt >= end) || (!optimized))
	continue_flag = FALSE;
    }

  if (optimized) string = (_XmString)
    _XmStringOptCreate(c, end, txtlength, havetag, tag_index);
  else string = _XmStringNonOptCreate(c, end, havetag);

  _XmProcessUnlock();
  return (string);
}

_XmStringEntry
_XmStringEntryCopy(_XmStringEntry entry)
{
  int i;
  int size;
  XtPointer text;
  _XmStringEntry new_entry = NULL;
  unsigned int entry_len;

  if (!entry)
    return NULL;

  entry_len = _XmEntryByteCountGet(entry);

  switch (_XmEntryType(entry)) {
  case XmSTRING_ENTRY_OPTIMIZED:
    if (_XmEntryImm(entry)) {
      if (entry_len > sizeof(XtPointer))
	size = sizeof(_XmStringOptSegRec) + entry_len - sizeof(XtPointer);
      else
	size = sizeof(_XmStringOptSegRec);
      new_entry = (_XmStringEntry)XtMalloc(size);
      memcpy((char *)new_entry, (char *)entry, size);
    } else {
      new_entry = (_XmStringEntry)XtMalloc(sizeof(_XmStringOptSegRec));
      memcpy((char *)new_entry, (char *)entry, sizeof(_XmStringOptSegRec));
      if (_XmEntryPermGet(entry)) {
	_XmEntryTextSet(new_entry, _XmEntryTextGet(entry));
      } else if (entry_len > 0) {
	text = (XtPointer)XtMalloc(entry_len);
	memcpy((char *)text, (char *)_XmEntryTextGet(entry), entry_len);
	_XmEntryTextSet(new_entry, text);
      } else
	_XmEntryTextSet(new_entry, NULL);
    }
    break;
  case XmSTRING_ENTRY_ARRAY:
    {
      _XmStringNREntry *arr;
      new_entry = (_XmStringEntry)XtMalloc(sizeof(_XmStringArraySegRec));
      memcpy((char *)new_entry, (char *)entry, sizeof(_XmStringArraySegRec));
      if (_XmEntrySegmentCount(entry) > 0) {
	arr = (_XmStringNREntry *)XtMalloc(_XmEntrySegmentCount(entry) *
					   sizeof(_XmStringNREntry));
	for (i = 0; i < _XmEntrySegmentCount(entry); i++)
	  arr[i] = (_XmStringNREntry)
	    _XmStringEntryCopy((_XmStringEntry)_XmEntrySegment(entry)[i]);
	_XmEntrySegment(new_entry) = arr;
      } else
	_XmEntrySegment(new_entry) = NULL;
    }
    break;
  case XmSTRING_ENTRY_UNOPTIMIZED:
    {
      new_entry = (_XmStringEntry)XtMalloc(sizeof(_XmStringUnoptSegRec));
      memcpy((char *)new_entry, (char *)entry,
	     sizeof(_XmStringUnoptSegRec));
      if (_XmEntryPermGet(entry)) {
	_XmEntryTextSet(new_entry, _XmEntryTextGet(entry));
      } else if (entry_len > 0) {
	text = (XtPointer)XtMalloc(entry_len);
	memcpy((char *)text,
	       (char *)_XmEntryTextGet((_XmStringEntry)entry),
	       entry_len);
	_XmEntryTextSet(new_entry, text);
      } else
	_XmEntryTextSet(new_entry, NULL);
      if (_XmUnoptSegRendBegins(entry)) {
	_XmUnoptSegRendBegins(new_entry) =
	  (XmStringTag *)XtMalloc(_XmUnoptSegRendBeginCount(entry) *
				  sizeof(XmStringTag));
	for (i = 0; i < _XmUnoptSegRendBeginCount(entry); i++)
	  _XmUnoptSegRendBegins(new_entry)[i] =
	    _XmUnoptSegRendBegins(entry)[i];
      } else
	_XmUnoptSegRendBegins(new_entry) = NULL;
      if (_XmUnoptSegRendEnds(entry)) {
	_XmUnoptSegRendEnds(new_entry) =
	  (XmStringTag *)XtMalloc(_XmUnoptSegRendEndCount(entry) *
				  sizeof(XmStringTag));
	for (i = 0; i < _XmUnoptSegRendEndCount(entry); i++)
	  _XmUnoptSegRendEnds(new_entry)[i] = _XmUnoptSegRendEnds(entry)[i];
      } else
	_XmUnoptSegRendEnds(new_entry) = NULL;
      _XmEntryCacheSet(new_entry, NULL);
    }
    break;
  }
  return(new_entry);
}

/** Begin macros converted to functions. **/

XmStringTag
_XmEntryTag(_XmStringEntry entry)
{
#if 1
  XmStringTag rettag;

  if (_XmEntryOptimized(entry))
  {
    if (_XmEntryTagIndex(entry) != TAG_INDEX_UNSET)
      rettag = _XmStringIndexGetTag(_XmEntryTagIndex(entry));
    else
      rettag = NULL;
  }
  else
    rettag = _XmUnoptSegTag(entry);

  return rettag;
#else
  return (_XmEntryOptimized(entry) ?
	  (_XmEntryTagIndex(entry) != TAG_INDEX_UNSET ?
	   _XmStringIndexGetTag(_XmEntryTagIndex(entry)) : NULL) :
	  _XmUnoptSegTag(entry));
#endif
}

void
_XmEntryTagSet(_XmStringEntry entry, XmStringTag tag)
{
  if (_XmEntryOptimized(entry))
    {
      if (tag == NULL)
	_XmEntryTagIndex(entry) = TAG_INDEX_UNSET;
      else
	_XmEntryTagIndex(entry) =
	  _XmStringIndexCacheTag(tag, XmSTRING_TAG_STRLEN);
    }
  else
    {
      _XmUnoptSegTag(entry) = tag;
    }
}


XmDirection
_XmEntryPushGet(_XmStringEntry entry)
{
  return (_XmEntryUnoptimized(entry) ?
	  ((_XmStringEntry)(entry))->unopt_single.push_before :
	  False);
}

unsigned int
_XmEntryByteCountGet(_XmStringEntry entry)
{
  switch (_XmEntryType(entry))
    {
    case XmSTRING_ENTRY_OPTIMIZED:
      return ((_XmStringEntry)(entry))->single.byte_count;
    case XmSTRING_ENTRY_ARRAY:
      return 0;
    case XmSTRING_ENTRY_UNOPTIMIZED:
      return _XmUnoptSegByteCount(entry);

    default:
      assert(FALSE);
      return 0;
    }
}

void
_XmEntryTextSet(_XmStringEntry entry,
		XtPointer val)
{
  (_XmEntryOptimized(entry) ?
   (!_XmEntryImm(entry) ?
    (((_XmStringOptSeg)(entry))->data.text = val) :
    /* This below is potentially dangerous. This function needs a length
       parameter, or requires that the byte count is set in the segment.
       However, to my knowledge, nobody needs this now. But it needs to
       be looked at.. */
    strcpy((char *)((_XmStringOptSeg)(entry))->data.chars, (char *)val)) :
   (((_XmStringUnoptSeg)(entry))->data.text = val));
}

XtPointer
_XmEntryTextGet(_XmStringEntry entry)
{
  return(_XmEntryOptimized(entry) ?
 	 (!_XmEntryImm(entry) ?
 	  ((_XmStringOptSeg)(entry))->data.text :
 	  (XtPointer)((_XmStringOptSeg)(entry))->data.chars) :
	 (((_XmStringUnoptSeg)(entry))->data.text));
}

unsigned int
_XmEntryDirectionGet(_XmStringEntry entry)
{
  return(_XmEntryOptimized(entry) ?
	 ((_XmStringEntry)(entry))->single.str_dir :
	 ((_XmStringEntry)(entry))->unopt_single.str_dir);
}

void
_XmEntryDirectionSet(_XmStringEntry entry,
		     XmDirection val)
{
  (_XmEntryOptimized(entry) ?
   (((_XmStringEntry)entry)->single.str_dir = val) :
   (((_XmStringEntry)entry)->unopt_single.str_dir = val));
}

unsigned int
_XmEntryTextTypeGet(_XmStringEntry entry)
{
  return(_XmEntryOptimized(entry) ?
	 ((_XmStringEntry)(entry))->single.text_type :
	 ((_XmStringEntry)(entry))->unopt_single.text_type);
}

_XmStringCache
_XmEntryCacheGet(_XmStringEntry entry)
{
  return (_XmEntryUnoptimized(entry) ?
	  ((_XmStringUnoptSeg)(entry))->cache :
	  NULL);
}

unsigned char
_XmEntryRendEndCountGet(_XmStringEntry entry)
{
  return(_XmEntryOptimized(entry) ?
	 ((_XmStringEntry)(entry))->single.rend_end :
	 _XmUnoptSegRendEndCount(entry));
}

unsigned char
_XmEntryRendBeginCountGet(_XmStringEntry entry)
{
  return(_XmEntryOptimized(entry) ?
	 ((_XmStringEntry)(entry))->single.rend_begin :
	 _XmUnoptSegRendBeginCount(entry));
}

Boolean
_XmEntryPopGet(_XmStringEntry entry)
{
  return (_XmEntryUnoptimized(entry) ?
	  ((_XmStringEntry)(entry))->unopt_single.pop_after :
	  False);
}

XmStringTag
_XmEntryRendEndGet(_XmStringEntry entry,
		   int n)
{
  return((_XmEntryRendEndCountGet(entry) > n) ?
	 (_XmEntryOptimized(entry) ?
	  (_XmEntryRendIndex(entry) != REND_INDEX_UNSET ?
	   _XmStringIndexGetTag(_XmEntryRendIndex(entry)) : NULL) :
	  (((_XmStringUnoptSeg)(entry))->rend_end_tags)[n]) :
	 NULL);
}

XmStringTag
_XmEntryRendBeginGet(_XmStringEntry entry,
		     int n)
{
  return((_XmEntryRendBeginCountGet(entry) > n) ?
	 (_XmEntryOptimized(entry) ?
	  (_XmEntryRendIndex(entry) != REND_INDEX_UNSET ?
	   _XmStringIndexGetTag(_XmEntryRendIndex(entry)) : NULL) :
	  (((_XmStringUnoptSeg)(entry))->rend_begin_tags)[n]) :
	 NULL);
}

void
_XmEntryRendEndSet(_XmStringEntry entry,
		   XmStringTag tag,
		   int n)
{
  int i;

  if (_XmEntryOptimized(entry))
    {
      if (tag == NULL) {
 	if (_XmEntryRendBeginCountGet(entry) == 0)
 	  _XmEntryRendIndex(entry) = REND_INDEX_UNSET;
      } else {
 	_XmEntryRendIndex(entry) = _XmStringIndexCacheTag(tag,
							  XmSTRING_TAG_STRLEN);
      }
      _XmEntryRendEndCountSet(entry, ((tag == NULL) ? 0 : 1));
    }
  else
    {
      if (tag == NULL)
  	{
 	  if (_XmEntryRendEndCountGet(entry) > n) {
	    for (i = n; i < _XmEntryRendEndCountGet(entry) - 1; i++)
 	      _XmUnoptSegRendEnds(entry)[i] = _XmUnoptSegRendEnds(entry)[i+1];

 	    _XmUnoptSegRendEndCount(entry)--;
 	    _XmUnoptSegRendEnds(entry)[_XmEntryRendEndCountGet(entry)] = NULL;
 	    if (_XmEntryRendEndCountGet(entry) == 0) {
 	      XtFree((char *)_XmUnoptSegRendEnds(entry));
 	      _XmUnoptSegRendEnds(entry) = NULL;
 	    }
 	  }
  	}
      else
  	{
 	  if (n >= _XmUnoptSegRendEndCount(entry)) {
 	    n = _XmUnoptSegRendEndCount(entry);
 	    _XmUnoptSegRendEndCount(entry)++;
 	    _XmUnoptSegRendEnds(entry) =
 	      (XmStringTag *)XtRealloc((char *)_XmUnoptSegRendEnds(entry),
 				       _XmUnoptSegRendEndCount(entry) *
 				       sizeof(XmStringTag));
 	  }
  	  _XmUnoptSegRendEnds(entry)[n] = tag;
  	}
    }
}


void
_XmEntryRendBeginSet(_XmStringEntry entry,
 		     XmStringTag tag,
 		     int n)
{
  int i;

  if (_XmEntryOptimized(entry))
    {
      if (tag == NULL) {
 	if (_XmEntryRendEndCountGet(entry) == 0)
 	  _XmEntryRendIndex(entry) = REND_INDEX_UNSET;
      } else {
 	_XmEntryRendIndex(entry) = _XmStringIndexCacheTag(tag,
 							  XmSTRING_TAG_STRLEN);
      }
      _XmEntryRendBeginCountSet(entry, ((tag == NULL) ? 0 : 1));
    }
  else
    {
      if (tag == NULL)
  	{
 	  if (_XmEntryRendBeginCountGet(entry) > n) {
	    for (i = n; i < _XmEntryRendBeginCountGet(entry) - 1; i++)
 	      _XmUnoptSegRendBegins(entry)[i] =
		_XmUnoptSegRendBegins(entry)[i+1];

 	    _XmUnoptSegRendBeginCount(entry)--;
 	    _XmUnoptSegRendBegins(entry)[_XmEntryRendBeginCountGet(entry)] =
 	      NULL;
 	    if (_XmEntryRendBeginCountGet(entry) == 0) {
 	      XtFree((char *)_XmUnoptSegRendBegins(entry));
 	      _XmUnoptSegRendBegins(entry) = NULL;
 	    }
 	  }
  	}
      else
  	{
 	  if (n >= _XmUnoptSegRendBeginCount(entry)) {
 	    n = _XmUnoptSegRendBeginCount(entry);
 	    _XmUnoptSegRendBeginCount(entry)++;
 	    _XmUnoptSegRendBegins(entry) =
 	      (XmStringTag *)XtRealloc((char *)_XmUnoptSegRendBegins(entry),
 				       _XmUnoptSegRendBeginCount(entry) *
 				       sizeof(XmStringTag));
 	  }
 	  _XmUnoptSegRendBegins(entry)[n] = tag;
  	}
    }
}

unsigned char
_XmEntryTabsGet(_XmStringEntry entry)
{
  return(_XmEntryOptimized(entry) ?
	 ((_XmStringEntry)(entry))->single.tabs_before :
	 ((_XmStringEntry)(entry))->unopt_single.tabs_before);
}

/** End macros converted to functions. **/

void
_XmStringEntryFree(_XmStringEntry entry)
{
  int i;

  if (!entry)
    return;

  switch (_XmEntryType(entry)) {
  case XmSTRING_ENTRY_OPTIMIZED:
    if (!_XmEntryImm(entry) && !_XmEntryPermGet(entry) && _XmEntryTextGet(entry))
      XtFree((char *)_XmEntryTextGet(entry));
    XtFree((char *)entry);
    break;
  case XmSTRING_ENTRY_ARRAY:
    for (i = 0; i < _XmEntrySegmentCount(entry); i++)
      _XmStringEntryFree((_XmStringEntry)_XmEntrySegment(entry)[i]);
    if (_XmEntrySegment(entry)) XtFree((char *)_XmEntrySegment(entry));
    XtFree((char *)entry);
    break;
  case XmSTRING_ENTRY_UNOPTIMIZED:
    _XmStringCacheFree(_XmEntryCacheGet(entry));
    if (_XmUnoptSegRendBegins(entry))
      XtFree((char *)_XmUnoptSegRendBegins(entry));
    if (_XmUnoptSegRendEnds(entry)) XtFree((char *)_XmUnoptSegRendEnds(entry));
    if (_XmEntryTextGet(entry) && !_XmEntryPermGet(entry))
      XtFree((char *)_XmEntryTextGet(entry));
    XtFree((char *)entry);
    break;
  }
}


/*
 * free the XmString internal data structure
 */
void
XmStringFree(
        XmString string )
{
  int i;
  int lcount;

  _XmProcessLock();
  if (!string) {
     _XmProcessUnlock();
     return;
  }

  /* Decrement refcount.  If not zero, just return. */
  if (_XmStrRefCountDec(string) != 0) {
     _XmProcessUnlock();
     return;
  }

  if (!_XmStrOptimized(string))
    {
      lcount = _XmStrEntryCount(string);
      for (i = 0; i < lcount; i++)
	{
	  _XmStringEntryFree(_XmStrEntry(string)[i]);
	}
      XtFree((char *)_XmStrEntry(string));
    }
  _XmStrFree ((char *) string);
  _XmProcessUnlock();
}

static void
ComputeMetrics(XmRendition rend,
	       XtPointer text,
	       unsigned int byte_count,
	       XmTextType type,
	       int which_seg,
	       Dimension *width,
	       Dimension *height,
	       Dimension *ascent,
	       Dimension *descent,
               Boolean utf8)
{
  Dimension	wid, hi;
  int		dir, asc, desc;

  wid = 0;
  hi = 0;
  asc = 0;
  desc = 0;

  switch (_XmRendFontType(rend)) {
    case XmFONT_IS_FONT:
    {
      XFontStruct *font_struct = (XFontStruct *)_XmRendFont(rend);
      XCharStruct	char_ret;

      if (two_byte_font(font_struct))
	{
	  if (byte_count >= 2 || utf8)
	    {
	      if (utf8)
		{
		  /* TODO: it is very unoptimized convert the same sting
		   * twice - for getting extents and drawing */
		  size_t  str_len = 0;
		  XChar2b *str = _XmUtf8ToUcs2(text, byte_count, &str_len);

		  XTextExtents16(font_struct, str, str_len,
			     &dir, &asc, &desc, &char_ret);
		  XFree(str);
		} else
	      XTextExtents16(font_struct,
			     (XChar2b *)text, Half(byte_count),
			     &dir, &asc, &desc, &char_ret);

	      wid = ComputeWidth(which_seg, char_ret);

	      /* pir 2967 */
	      if (wid == 0)
		wid = Half(byte_count) * (font_struct->max_bounds.width);
	      hi = asc + desc;
	    }
	}
      else
	{
	  if (byte_count >= 1)
	    {
	      XTextExtents(font_struct, (char *)text, byte_count,
			   &dir, &asc, &desc, &char_ret);

	      wid = ComputeWidth(which_seg, char_ret);

	      /* pir 2967 */
	      if (wid == 0)
		wid = byte_count * (font_struct->max_bounds.width);
	      hi = asc + desc;
	    }
	}
    }
    break;
    case XmFONT_IS_FONTSET:
    {
      if (byte_count >= 1)
	{
	  XFontSet font_set = (XFontSet)_XmRendFont(rend);
	  XRectangle ink, logical;

	  if (type == XmWIDECHAR_TEXT)
	    XwcTextExtents(font_set, (wchar_t *)text,
			   byte_count/sizeof(wchar_t),
			   &ink, &logical);
	  else {
#if XM_UTF8
              if (utf8)
                  Xutf8TextExtents(font_set, (char *)text, byte_count,
                          &ink, &logical);
              else
#endif
                  XmbTextExtents(font_set, (char *)text, byte_count,
                          &ink, &logical);
          }

	  if (logical.height == 0)
	    {
	      XFontSetExtents *extents = XExtentsOfFontSet(font_set);
	      logical.height = extents->max_logical_extent.height;
	    }
	  wid = logical.width;
	  hi = logical.height;
	  asc = -(logical.y);
	  desc = logical.height + logical.y;
	}
    }
    break;
#if USE_XFT
    case XmFONT_IS_XFT:
	asc = _XmRendXftFont(rend)->ascent;
	desc = _XmRendXftFont(rend)->descent;
	/* FIXME
	 * Following Keith Packard comments it should be
	 *  hi = _XmRendXftFont(rend)->height;
	 * but is looking ascent + descent better. Is it a bug?
	 */
	hi = asc+desc;
    {
	XGlyphInfo	info;
	XftTextExtentsUtf8(_XmRendDisplay(rend),
		_XmRendXftFont(rend),
		text, byte_count,
		&info);
	wid = info.xOff;
    }
#endif
  }

  /* Adjust for underlining. Add one pixel for line and one pixel at bottom so
   * that line doesn't bleed into background with select color of
   * XmREVERSED_GROUND_COLORS.
   */
  switch (_XmRendUnderlineType(rend))
    {
    case XmSINGLE_LINE:
    case XmSINGLE_DASHED_LINE:
      if (desc < (SINGLE_OFFSET + 2))
	{
	  hi += (SINGLE_OFFSET + 2) - desc;
	  desc = SINGLE_OFFSET + 2;
	}
      break;
    case XmDOUBLE_LINE:
    case XmDOUBLE_DASHED_LINE:
      if (desc < (DOUBLE_OFFSET + 2))
	{
	  hi += (DOUBLE_OFFSET + 2) - desc;
	  desc = DOUBLE_OFFSET + 2;
	}
      break;
    default:
      break;
    }

  if (width != NULL) *width = wid;
  if (height != NULL) *height = hi;
  if (ascent != NULL) *ascent = asc;
  if (descent != NULL) *descent = desc;
}

static Dimension
ComputeWidth(unsigned char which,
	     XCharStruct char_ret)
{
  Dimension wid = 0;
  int bearing;

  /* Width of first segment is -leftbearing + width. */
  /* Width of last segment is max of width and rightbearing. */
  /* Width of single segment is max of width and rightbearing - leftbearing. */
  /* Width of all other segments is width. */
  switch (which)
    {
    case XmSTRING_FIRST_SEG:
      if (char_ret.lbearing < 0)
	wid = -(char_ret.lbearing);
      /* Fall through */
    case XmSTRING_MIDDLE_SEG:
      wid += char_ret.width;
      break;
    case XmSTRING_LAST_SEG:
      wid = ((char_ret.width > char_ret.rbearing) ?
	     char_ret.width : char_ret.rbearing);
      break;
    case XmSTRING_SINGLE_SEG:
      bearing = char_ret.rbearing - char_ret.lbearing;
      wid = (char_ret.width > bearing) ? char_ret.width : bearing;
      break;
    }
  return(wid);
}

/****************************************************************
 *
 ****************************************************************/
Boolean
_XmStringSegmentExtents(_XmStringEntry entry,
			XmRenderTable    rendertable,
			XmRendition    * rend_in_out,
			XmRendition	 base,
			Dimension      * width,
			Dimension      * height,
			Dimension      * ascent,
			Dimension      * descent)
{
  return(SpecifiedSegmentExtents(entry, rendertable, rend_in_out, base,
				 XmSTRING_MIDDLE_SEG,
				 width, height, ascent, descent));
}

static Boolean
SpecifiedSegmentExtents(_XmStringEntry entry,
			XmRenderTable    rendertable,
			XmRendition    * rend_in_out,
			XmRendition	 base,
			int		 which_seg,
			Dimension      * width,
			Dimension      * height,
			Dimension      * ascent,
			Dimension      * descent)
{
  unsigned short	count;
  XmStringTag		*tags = NULL;
  unsigned int		tag_count =0;
  Display		*d;
  XmStringTag		def_tag;
  XmStringTag		entry_tag;
  XmRendition		rend, cached_rend = NULL, def_rend;
  int			i, j, depth, hits, ref_cnt, rt_ref_cnt;
  Dimension		h, w, asc, dsc;
  Boolean		can_do = TRUE;
  _XmRendition		rend_int;
  _XmStringRenderingCache render_cache;

  /* Fetching the cache once and accessing the fields directly saves
   * substantial time searching the cache
   */
  render_cache = (_XmStringRenderingCache)CacheGet(entry, _XmRENDERING_CACHE, False,
						   (XtPointer)rendertable);

  if ((render_cache != NULL) && !render_cache->header.dirty)
    {
      if (width != NULL)
	*width = (Dimension)render_cache->width;
      if (height != NULL)
	*height = (Dimension)render_cache->height;
      if (ascent != NULL)
	*ascent = (Dimension)render_cache->ascent;
      if (descent != NULL)
	*descent = (Dimension)render_cache->descent;
      if (rend_in_out != NULL)
	*rend_in_out = render_cache->rendition;
      return True;
    } else if (rend_in_out == NULL)
      {
	if (render_cache != NULL)
	  cached_rend = render_cache->rendition;

	if (cached_rend == NULL) return(FALSE);
	else rend_in_out = &cached_rend;
      }

  entry_tag = _XmEntryTag(entry);

  if (cached_rend == NULL)			  /* Update *rend_in_out. */
    {
      /* Add rendition begins */
      d = _XmRendDisplay(*rend_in_out);

      /* Prepare tags. */
      count = _XmEntryRendBeginCountGet(entry);
      tags = _XmRendTags(*rend_in_out);
      tag_count = _XmRendTagCount(*rend_in_out);

      /* Update tag stack. */
      if (count > 0)
	{
	  tags = (XmStringTag *)XtRealloc((char *)tags,
					  (sizeof(XmStringTag) *
					   (tag_count + count)));
	  for (i = 0; i < count; i++)
	    tags[tag_count + i] = _XmEntryRendBeginGet(entry, i);

	  tag_count += count;
	}

      /* compute rendition */
      /* Find font as per I 198. */
      /* 1. Find font from rendition tags. */
      /* 2. Find font from locale/charset tag. */
      if ((_XmRendTag(*rend_in_out) != entry_tag) ||
	  (count != 0) || _XmRendHadEnds(*rend_in_out))
	{
	  *rend_in_out = _XmRenditionMerge(d, rend_in_out, base, rendertable,
					   entry_tag, tags, tag_count,
					   (render_cache != NULL));
	  _XmRendTag(*rend_in_out) = entry_tag;
	}

      /* 3. Default rendition. */
      if (_XmRendFont(*rend_in_out) == NULL &&
          _XmRendXftFont(*rend_in_out) == NULL)
	{
	  def_tag = ((_XmEntryTextTypeGet(entry) == XmCHARSET_TEXT) ?
		     XmFONTLIST_DEFAULT_TAG :
		     _MOTIF_DEFAULT_LOCALE);

	  rend = _XmRenditionMerge(d, rend_in_out, base, rendertable,
				   def_tag, NULL, 0, (render_cache != NULL));

	  if ((rend != NULL) &&
	      (_XmRendFont(rend) == NULL) && (_XmRendXftFont(rend) == NULL) &&
	      ((def_rend = _XmRenderTableFindRendition(rendertable, def_tag,
						       TRUE, FALSE, FALSE, NULL))
	       != NULL))
	    /* Call noFontCallback. */
	    {
	      XmDisplay			dsp;
	      XmDisplayCallbackStruct	cb;

	      rt_ref_cnt = _XmRTRefcount(rendertable);
	      def_rend = _XmRTRenditions(rendertable)[0];
	      rend_int = *def_rend;
	      ref_cnt = _XmRendRefcount(def_rend);

	      dsp = (XmDisplay)XmGetXmDisplay(d);
	      cb.reason = XmCR_NO_FONT;
	      cb.event = NULL;
	      cb.rendition = def_rend;
	      cb.font_name = XmS;

	      XtCallCallbackList((Widget)dsp, dsp->display.noFontCallback, &cb);

	      if (rend_int != *def_rend)	  /* Changed in callback. */
		{
		  /* Need to split ref counts. */
		  _XmRendRefcount(&rend_int) = ref_cnt - rt_ref_cnt;
		  _XmRendRefcount(def_rend) = rt_ref_cnt;
		}

	      if (_XmRendFont(def_rend) != NULL)
		{
		  _XmRendFontType(rend) = _XmRendFontType(def_rend);
		  _XmRendFont(rend) = _XmRendFont(def_rend);
		}
#if USE_XFT
              else if (_XmRendXftFont(def_rend) != NULL)
	        {
		  _XmRendFontType(rend) = _XmRendFontType(def_rend);
		  _XmRendXftFont(rend) = _XmRendXftFont(def_rend);
		}
#endif
	      else rend = NULL;
	    }

	  /* 4a. Take the first one */
	  if ((rend == NULL) &&
	      ((_XmEntryTextTypeGet(entry) == XmCHARSET_TEXT) ||
	       ((_XmEntryTextTypeGet(entry) == XmMULTIBYTE_TEXT) &&
		(entry_tag == XmFONTLIST_DEFAULT_TAG))) &&
	      (rendertable != NULL) && (_XmRTCount(rendertable) > 0))
	    rend = _XmRenditionMerge(d, rend_in_out, base, rendertable,
				     NULL, NULL, 0, (render_cache != NULL));

	  if ((rend != NULL) &&
	      (_XmRendFont(rend) == NULL) && (_XmRendXftFont(rend) == NULL))
	    /* Call noFontCallback. */
	    {
	      XmDisplay			dsp;
	      XmDisplayCallbackStruct	cb;

	      rt_ref_cnt = _XmRTRefcount(rendertable);
	      def_rend = _XmRTRenditions(rendertable)[0];
	      rend_int = *def_rend;
	      ref_cnt = _XmRendRefcount(def_rend);

	      dsp = (XmDisplay)XmGetXmDisplay(d);
	      cb.reason = XmCR_NO_FONT;
	      cb.event = NULL;
	      cb.rendition = def_rend;
	      cb.font_name = XmS;

	      XtCallCallbackList((Widget)dsp, dsp->display.noFontCallback, &cb);

	      if (rend_int != *def_rend)	  /* Changed in callback. */
		{
		  /* Need to split ref counts. */
		  _XmRendRefcount(&rend_int) = ref_cnt - rt_ref_cnt;
		  _XmRendRefcount(def_rend) = rt_ref_cnt;
		}

	      if (_XmRendFont(def_rend) != NULL)
		{
		  _XmRendFontType(rend) = _XmRendFontType(def_rend);
		  _XmRendFont(rend) = _XmRendFont(def_rend);
		}
#if USE_XFT
              else if (_XmRendXftFont(def_rend) != NULL)
	        {
		  _XmRendFontType(rend) = _XmRendFontType(def_rend);
		  _XmRendXftFont(rend) = _XmRendXftFont(def_rend);
		}
#endif
	      else rend = NULL;
	    }

	  /* 4b/5a. Emit warning and don't render. */
	  if ((rend == NULL) ||
	      (_XmRendFont(rend) == NULL && _XmRendXftFont(rend) == NULL))
	    {
	      /* No warning if no tags, e.g. just dir component. */
	      if ((tag_count > 0) || (entry_tag != NULL))
		XmeWarning(NULL, NO_FONT_MSG);
	      if (width != NULL)
		{
		  *width = 0;
		  if (render_cache)
		    render_cache->width = 0;
		}
	      if (height != NULL)
		{
		  *height = 0;
		  if (render_cache)
		    render_cache->height = 0;
		}
	      if (ascent != NULL)
		{
		  *ascent = 0;
		  if (render_cache)
		    render_cache->ascent = 0;
		}
	      if (descent != NULL)
		{
		  *descent = 0;
		  if (render_cache)
		    render_cache->descent = 0;
		}
	      can_do = FALSE;
	    }
	}
    }

  if (can_do)
    {
      /* compute width & height */
      ComputeMetrics(*rend_in_out, _XmEntryTextGet(entry),
		     _XmEntryByteCountGet(entry),
		     (XmTextType) _XmEntryTextTypeGet(entry),
		     which_seg, &w, &h, &asc, &dsc,
#if XM_UTF8
                   _XmEntryType(entry) == XmCHARSET_TEXT &&
                   (_XmEntryTag(entry) == XmFONTLIST_DEFAULT_TAG
                    && (_XmStringIsCurrentCharset("UTF-8")
                    || (_XmEntryTagIndex(entry) !=  TAG_INDEX_UNSET
		    && strcmp(_XmEntryTag(entry), "UTF-8") == 0)))
#else
                   False
#endif
		    );

      /* If cache exists, set it. */
      if (render_cache != NULL)
	{
	  if (width != NULL) render_cache->width = w;
	  if (height != NULL) render_cache->height = h;
	  if (ascent != NULL) render_cache->ascent = asc;
	  if (descent != NULL) render_cache->descent = dsc;

	  render_cache->rendition = *rend_in_out;
	  render_cache->header.dirty = False;
	}

      if (width != NULL) *width = w;
      if (height != NULL) *height = h;
      if (ascent != NULL) *ascent = asc;
      if (descent != NULL) *descent = dsc;
    }

  if (cached_rend == NULL)			  /* Update *rend_in_out */
    {
      /* Remove rendition ends. */
      count = _XmEntryRendEndCountGet(entry);

      if (count > 0)
	{
	  depth = tag_count;
	  hits = 0;

	  for (i = 0; i < count; i++)
	    for (j = (tag_count - 1); j >= 0; j--)
	      if (_XmEntryRendEndGet(entry, i) == (tags)[j])
		{
		  tags[j] = NULL;
		  depth = j;
		  hits++;
		  break;
		}

	  j = depth;
	  for (i = (depth + 1); i < tag_count; i++)
	    if (tags[i] != NULL)
	      {
		tags[j] = tags[i];
		j++;
	      }

	  if(tags != NULL && tag_count - hits)
	    tags = (XmStringTag *)XtRealloc((char *)tags,
					  (sizeof(XmStringTag) *
					   (tag_count - hits)));
	  tag_count -= hits;
	  _XmRendHadEnds(*rend_in_out) = TRUE;
	}
      else
	{
	  _XmRendHadEnds(*rend_in_out) = FALSE;
	}

      _XmRendTagCount(*rend_in_out) = tag_count;
      _XmRendTags(*rend_in_out) = tags;
    }

  return(can_do);
}

static void _parse_locale(char *str, int *indx, int *len)
{
    char     *temp;
    int      start;
    int      end;

    /*
     *  Set the return variables to zero.  If we find what we're looking
     *  for, we reset them.
     */

    *indx = 0;
    *len = 0;
    if (!str) return;

    /*
     *  The format of the locale string is:
     *          language[_territory[.codeset]]
     */

    temp = str;
    end = 0;
    while ((temp[end] != '.') && (temp[end] != 0))
      end++;

    if (temp[end] == '.')
    {
        start = end + 1;
        *indx = start;
	end = start;
        while (temp[end] != 0)
	  end++;
        *len = end - start;
    }
}

 /* This function returns current default charset being used.  This is */
 /* determined from the value of the $LANG environment variable or */
 /* XmFALLBACK_CHARSET.  */
char *_XmStringGetCurrentCharset(void)
{
	char *ptr, *str, *ret_val;
	int indx, len;

	_XmProcessLock();
	if (locale.inited)
		goto out;

	locale.tag    = NULL;
	locale.taglen = 0;
	str           = getenv(env_variable);

	_parse_locale(str, &indx, &len);
	if (len > 0) {
		ptr = str + indx;
		if (!strcmp(ptr, "UTF8")) {
			ptr = "UTF-8";
			len = 5;
		}
	} else {
		ptr = XmFALLBACK_CHARSET;
		len = strlen(XmFALLBACK_CHARSET);
	}

	locale.tag = XtMalloc(len + 1);
	strncpy(locale.tag, ptr, len);
	locale.tag[len] = '\0';
	locale.taglen = len;

	/* Register XmSTRING_DEFAULT_CHARSET for compound text conversion. */
	XmRegisterSegmentEncoding(XmSTRING_DEFAULT_CHARSET, XmFONTLIST_DEFAULT_TAG);
	locale.inited = TRUE;

out:
	ret_val = locale.tag;
	_XmProcessUnlock();
	return ret_val;
}

 /* This function compares a given charset to the current default charset
    being used.  It return TRUE if they match, FALSE otherwise.
 */
Boolean
_XmStringIsCurrentCharset( XmStringCharSet c )
{
  return (strcmp(c, _XmStringGetCurrentCharset()) == 0);
}

/*
 * copy a refcounted string
 */
XmString
XmStringCopy(
        XmString string )
{
  _XmProcessLock();
  if (string == NULL) {
      _XmProcessUnlock();
      return((XmString)NULL);
  }

  /* If the refcount wraps around, have to make clone,
     otherwise just return. */
  if (_XmStrRefCountInc(string) != 0) {
    _XmProcessUnlock();
    return(string);
  }
  else
    {
      XmString ret_val;

      _XmStrRefCountDec(string);
      ret_val = Clone(string, _XmStrEntryCountGet(string));
      _XmProcessUnlock();
      return ret_val;
    }
}

/*
 * duplicate structure of an internal string
 */
static XmString
Clone(XmString string,
      int lines)
{
  XmString new_string;

  if (_XmStrOptimized(string))
    {
      _XmStringOpt n_o_string =
	(_XmStringOpt) _XmStrMalloc(sizeof(_XmStringOptRec) +
				    _XmStrByteCount(string) -
				    TEXT_BYTES_IN_STRUCT);

      memcpy(n_o_string, string,
	     sizeof(_XmStringOptRec) +
	     _XmStrByteCount(string) -
	     TEXT_BYTES_IN_STRUCT) ;
      new_string = (XmString)n_o_string;
    }
  else
    {
      int i;
      _XmString n_string;

      _XmStrCreate(n_string, XmSTRING_MULTIPLE_ENTRY, 0);

      _XmStrImplicitLine(n_string) = _XmStrImplicitLine(string);

      _XmStrEntryCount(n_string) = _XmStrEntryCount(string);
      _XmStrEntry(n_string) = (_XmStringEntry *)
	XtMalloc(sizeof(_XmStringEntry) * lines);

      for (i = 0; i < _XmStrEntryCount(string); i++)
	_XmStrEntry(n_string)[i] = _XmStringEntryCopy(_XmStrEntry(string)[i]);
      for (i = _XmStrEntryCount(string); i < lines; i++)
	_XmStrEntry(n_string)[i] = NULL;

      new_string = (XmString)n_string;
    }

  _XmStrRefCountSet(new_string, 1);

  return(new_string);
}

/*
 * Given a string in ASN.1 format, return the size of the
 * string, including the header.
 */
unsigned int
XmStringByteStreamLength(unsigned char *string)
{
    unsigned int len;

    _XmProcessLock();
    len = _read_string_length( string );
    len += _calc_header_size(len);
    _XmProcessUnlock();
    return (len);
}

/*
 * build the ASN.1 format given an XmString.
 * This makes a pass to figure out how big it
 * needs to be and builds the ASN.1 string in-place.
 * If prop_return is NULL, just computes size.
 */
unsigned int
XmCvtXmStringToByteStream(XmString string,
		  unsigned char **prop_return)
{
  /* Using XmeStringGetComponent makes this almost trivial. */
  _XmStringContextRec	stack_context;
  unsigned int		length;
  XtPointer		value;
  unsigned int		len;
  unsigned int		str_len;
  unsigned char		*ext;
  XmStringComponentType	tag;

  _XmProcessLock();
  if (!string)
    {
      if (prop_return != NULL) *prop_return = NULL;
      _XmProcessUnlock();
      return(0);
    }

  _XmStringContextReInit(&stack_context, string);

  /* Compute size */
  len = 0;
  while (XmeStringGetComponent(&stack_context, TRUE, FALSE, &length, &value) !=
	 XmSTRING_COMPONENT_END)
    len += _asn1_size(length) + length;

  str_len = len;
  len += _calc_header_size(len);

  _XmStringContextFree(&stack_context);

  /* We're just computing size. */
  if (prop_return == NULL) {
	 _XmProcessUnlock();
	 return(len);
  }

  /* Allocate. */
  ext = (unsigned char *)XtMalloc(len);
  *prop_return = ext;

  /* Write components. */
  ext = _write_header(ext, str_len);

  _XmStringContextReInit(&stack_context, string);

  while ((tag = XmeStringGetComponent(&stack_context, TRUE, FALSE,
				      &length, &value)) !=
	 XmSTRING_COMPONENT_END)
    ext = _write_component(ext, tag, length, (unsigned char *)value, TRUE);

  _XmStringContextFree(&stack_context);

  _XmProcessUnlock();
  return(len);
}

Dimension XmStringBaseline(XmRenderTable rendertable, XmString string)
{
  Dimension 		width, height, asc = 0, desc;
  _XmRenditionRec	scratch;
  _XmRendition		tmp;
  XmRendition		rend;
  _XmStringEntry 	line;
  _XmStringArraySegRec 	array_seg;
  Display		*d;
  XtAppContext		app = NULL;

  if (!rendertable || !string)
      return 0;

#ifdef XTHREADS
  if (_XmRTDisplay(rendertable))
    app = XtDisplayToApplicationContext(_XmRTDisplay(rendertable));
  if (app)
  {
	_XmAppLock(app);
  }
  else
  {
	_XmProcessLock();
  }
#endif

  memset(&scratch, 0, sizeof scratch);
  tmp = &scratch;
  rend = &tmp;

  /* Initialize for tabs. */
  d = !_XmRTDisplay(rendertable) ? _XmGetDefaultDisplay() : _XmRTDisplay(rendertable);

  _XmRendDisplay(rend) = d;
  _XmStringLayout(string, XmLEFT_TO_RIGHT);

  if (!_XmStrOptimized(string))
    {
      if (_XmStrImplicitLine(string))
	line = _XmStrEntry(string)[0];
      else {
	_XmEntryType(&array_seg) = XmSTRING_ENTRY_ARRAY;
	_XmEntrySegmentCount(&array_seg) = _XmStrEntryCount(string);
	_XmEntrySegment(&array_seg) =
	  (_XmStringNREntry *)_XmStrEntry(string);
	line = (_XmStringEntry)&array_seg;
      }

      LineMetrics(line, rendertable, &rend, NULL, XmLEFT_TO_RIGHT,
		  &width, &height, &asc, &desc);

      if (app)
  	{
         _XmAppUnlock(app);
	}
      else
	{
	 _XmProcessUnlock();
	}
    }
  else
    {
      if (app)
	{
         _XmAppUnlock(app);
	}
      else
	{
	 _XmProcessUnlock();
	}
      asc = OptLineAscender(rendertable, (_XmStringOpt)string);
    }

    return asc < 2048 ? asc : 0;
}

void
_XmStringGetBaselines(XmRenderTable rendertable,
		      _XmString string,
		      Dimension **baselines,
		      Cardinal *line_count)
{
  /* Initialize the return values. */
  *baselines = NULL;
  *line_count = 0;

  if (rendertable && string)
    *line_count = XmStringLineCount(string);

  if (*line_count == 1)
    {
      *baselines = (Dimension*) XtMalloc(*line_count * sizeof(Dimension));
      (*baselines)[0] = XmStringBaseline(rendertable, string);
    }
  else if (*line_count > 1)
    {
      Cardinal line_num;
      Dimension offset;
      Dimension prev_height;

      Dimension		   width, height, asc, desc;
      _XmRenditionRec	   scratch;
      _XmRendition	   tmp = &scratch;
      XmRendition	   rend = &tmp;
      _XmStringArraySegRec array_seg;

      *baselines = (Dimension*) XtMalloc(*line_count * sizeof(Dimension));

      /* Initialize the scratch rendition for tabs. */
      bzero((char*) &scratch, sizeof(_XmRenditionRec));
      _XmRendDisplay(rend) =
	((_XmRTDisplay(rendertable) == NULL) ?
	 _XmGetDefaultDisplay() : _XmRTDisplay(rendertable));

      _XmStringLayout(string, XmLEFT_TO_RIGHT);

      offset = prev_height = 0;
      for (line_num = 0; line_num < *line_count; line_num++)
	{
	  _XmStringEntry line;

	  if (_XmStrImplicitLine(string))
	    line = _XmStrEntry(string)[line_num];
	  else {
	    _XmEntryType(&array_seg) = XmSTRING_ENTRY_ARRAY;
	    _XmEntrySegmentCount(&array_seg) = _XmStrEntryCount(string);
	    _XmEntrySegment(&array_seg) =
	      (_XmStringNREntry *)_XmStrEntry(string);
	    line = (_XmStringEntry)&array_seg;
	  }

	  LineMetrics(line, rendertable, &rend, NULL, XmLEFT_TO_RIGHT,
		      &width, &height, &asc, &desc);

	  /* Treat empty lines as the same height as the previous line. */
	  if (height)
	    prev_height = height;

	  (*baselines)[line_num] = offset + asc;
	  offset += prev_height;
	}
    }
}

/*
 * count the number of lines in an XmString.
 */
int
XmStringLineCount(
        XmString string )
{
  int ret_val;

  _XmProcessLock();
  if (!string) {
	_XmProcessUnlock();
	return(0);
  }

  if (_XmStrOptimized(string)) {
    _XmProcessUnlock();
    return( 1) ;
  }

  ret_val = (int) _XmStrLineCountGet(string) ;
  _XmProcessUnlock();
  return ret_val;
}

/*
 * drawing routine for external TCS
 */
void
XmStringDraw(
        Display *d,
        Window w,
        XmRenderTable rendertable,
        XmString string,
        GC gc,
        Position x,
        Position y,
        Dimension width,
        unsigned char align,
        unsigned char lay_dir,
        XRectangle *clip )
{
  _XmDisplayToAppContext(d);
  _XmAppLock(app);

  if (string)
    _draw (d, w, rendertable, (_XmString)string, gc, x, y, width,
	   align, lay_dir, clip, FALSE, NULL);

  _XmAppUnlock(app);
}

void
XmStringDrawImage(
        Display *d,
        Window w,
        XmRenderTable rendertable,
        XmString string,
        GC gc,
        Position x,
        Position y,
        Dimension width,
        unsigned char align,
        unsigned char lay_dir,
        XRectangle *clip )
{
  _XmDisplayToAppContext(d);
  _XmAppLock(app);

  if (string)
    _draw (d, w, rendertable, (_XmString)string, gc, x, y, width,
	   align, lay_dir, clip, TRUE, NULL);

  _XmAppUnlock(app);
}

void
XmStringDrawUnderline(
        Display *d,
        Window w,
        XmRenderTable fntlst,
        XmString str,
        GC gc,
        Position x,
        Position y,
        Dimension width,
        unsigned char align,
        unsigned char lay_dir,
        XRectangle *clip,
        XmString under )
{
  _XmDisplayToAppContext(d);
  _XmAppLock(app);
  if (str)
    _draw (d, w, fntlst, (_XmString)str, gc, x, y, width,
	   align, lay_dir, clip, FALSE, (_XmString)under);

  _XmAppUnlock(app);
}


#ifdef _XmDEBUG_XMSTRING

void
_Xm_dump_stream(
        unsigned char *cs )
{
    unsigned char *c;
    unsigned char *end;
    int k;

    if (_is_asn1(cs))
    {
	printf ("Compound string\n");
	printf ("overall length = %d\n", _read_string_length(cs));
	c = _read_header(cs);
    }
    else {
        printf ("Not a compound string\n");
        return;
    }

    c = (unsigned char *) cs;
    end = c + _read_string_length (c) + _read_header_length(c);

    while (c < end)
    {
	unsigned short length = _read_asn1_length (c);

	switch (*c)
	{
	    case XmSTRING_COMPONENT_TAG:
	    case XmSTRING_COMPONENT_LOCALE:
                if (*c ==  XmSTRING_COMPONENT_LOCALE)
		  printf ("\tLocale name component\n");
		else
		  printf ("\tCharacter set component\n");
		printf ("\tlength = %d\n", length);
	  	printf ("\tvalue  = <");
		for (k=0; k<length; k++)
		  printf ("%c", *(c + _asn1_size(length) + k));
		printf (">\n");
		c += length + _asn1_size(length);
		break;

	    case XmSTRING_COMPONENT_TEXT:
            case XmSTRING_COMPONENT_LOCALE_TEXT:
                if (*c ==  XmSTRING_COMPONENT_TEXT)
                  printf ("\tText component\n");
                else printf ("\tLocalized text component\n");

		printf ("\tlength = %d\n", length);
	  	printf ("\tvalue  = <");
		for (k=0; k<length; k++)
		  printf ("%c", *(c + _asn1_size(length) + k));
		printf (">\n");
		c += length + _asn1_size(length);
		break;

            case XmSTRING_COMPONENT_WIDECHAR_TEXT:
                printf ("\tWide char text component\n");
		printf ("\tlength = %d\n", length);
	  	printf ("\tvalue  = <");
		for (k=0; k<length; k++)
		  printf ("%c", *(c + _asn1_size(length) + k));
		printf (">\n");
		c += length + _asn1_size(length);
		break;

	    case XmSTRING_COMPONENT_DIRECTION:		/* record dir */
		printf ("\tDirection component\n");
		printf ("\tlength = %d\n", length);
	  	printf ("\tvalue  = %d\n", *(c + _asn1_size(length)));
		c += length + _asn1_size(length);
		break;

	    case XmSTRING_COMPONENT_SEPARATOR:		/* start new line */
		printf ("\tSeparator component\n");
		printf ("\tlength = %d\n", length);
		c += length + _asn1_size(length);
		break;

	    default:
		printf ("\tUnknown component\n");
		printf ("\tlength = %d\n", length);
	  	printf ("\tvalue  = <");
		for (k=0; k<length; k++)
			printf ("%3d ", (int) *(c + _asn1_size(length) + k));
		printf ("\n");
		c += length + _asn1_size(length);
		break;
	}

	printf ("\n");

    }
}

static char*
type_image(XmTextType type)
{
  switch (type)
    {
    case XmCHARSET_TEXT:
      return "XmCHARSET_TEXT";
    case XmMULTIBYTE_TEXT:
      return "XmMULTIBYTE_TEXT";
    case XmWIDECHAR_TEXT:
      return "XmWIDECHAR_TEXT";
    case XmNO_TEXT:
      return "XmNO_TEXT";
    default:
      return "unknown";
    }
}

static char*
entry_type_image(int type)
{
  switch (type)
    {
    case XmSTRING_ENTRY_OPTIMIZED:
      return "XmSTRING_ENTRY_OPTIMIZED";
    case XmSTRING_ENTRY_ARRAY:
      return "XmSTRING_ENTRY_ARRAY";
    case XmSTRING_ENTRY_UNOPTIMIZED:
      return "XmSTRING_ENTRY_UNOPTIMIZED";
    default:
      return "unknown";
    }
}

void
_Xm_dump_internal(
        _XmString string )
{
  int i, j, k;

  if (_XmStrOptimized(string))
    {
      printf ("string with 1 line\n") ;
      printf ("\tOptimized string - single segment\n");
      printf ("\t    tag type      = %4d\n", _XmStrTextType(string));
      printf ("\t    direction     = %4d\n", _XmStrDirection(string));
      printf ("\t    tag index     = %4d\n", _XmStrTagIndex(string));
      printf ("\t    rend_begin    = %4d\n", _XmStrRendBegin(string));
      printf ("\t    rend_end      = %4d\n", _XmStrRendEnd(string));
      printf ("\t    rend_index    = %4d <%s>\n", _XmStrRendIndex(string),
	      (_XmStrRendTagGet(string) ?
	       _XmStrRendTagGet(string) : "(unset)"));
      printf ("\t    tab_before    = %4d\n", _XmStrTabs(string));
      printf ("\t    refcount      = %4d\n", _XmStrRefCountGet(string));
      printf ("\t    char count    = %4d\n", _XmStrByteCount(string));
      printf ("\t    text          = <");
      for (k=0; k<_XmStrByteCount(string); k++)
	printf ("%c", _XmStrText(string)[k]);
      printf (">\n");
    }

  else {
    _XmStringEntry 		line;
    int			line_count;
    _XmStringEntry 		seg;
    int			seg_count;
    _XmStringArraySegRec	array_seg;

    line_count = _XmStrLineCountGet(string);

    printf ("string with %d lines\n", line_count);
    for (i = 0; i < line_count; i++)
      {
	if (_XmStrImplicitLine(string))
	  {
	    line = _XmStrEntry(string)[i];
	  }
	else
	  {
	    _XmEntryType(&array_seg) = XmSTRING_ENTRY_ARRAY;
	    _XmEntrySegmentCount(&array_seg) = _XmStrEntryCount(string);
	    _XmEntrySegment(&array_seg) = (_XmStringNREntry *)_XmStrEntry(string);
	    line = (_XmStringEntry)&array_seg;
	  }

	if (_XmEntryMultiple(line))
	  seg_count = _XmEntrySegmentCount(line);
	else
	  seg_count = 1;

	printf ("\tline [%d] has %d segments\n", i, seg_count);

	for (j = 0; j < seg_count; j++)
	  {
	    if (_XmEntryMultiple(line))
	      seg = (_XmStringEntry)_XmEntrySegment(line)[j];
	    else seg = line;

	    printf ("\t    segment [%d]\n", j);
	    if (seg == NULL) {
              printf ("\t\tNULL?\n");
              continue;
	    }

	    printf ("\t\ttype            = %d (%s)\n",
		    _XmEntryType(seg),
		    entry_type_image(_XmEntryType(seg)));
	    printf ("\t\tpush before     = %d \n", _XmEntryPushGet(seg));
	    printf ("\t\trend_begin_tags = (%d)\n",
		    _XmEntryRendBeginCountGet(seg));
	    for (k=0; k<_XmEntryRendBeginCountGet(seg); k++)
	      printf ("\t\t  %4d <%s>\n",
		      _XmStringIndexCacheTag(_XmEntryRendBeginGet(seg,k),
					     XmSTRING_TAG_STRLEN),
		      _XmEntryRendBeginGet(seg,k));
	    printf ("\t\ttag             = <%s>\n", _XmEntryTag(seg));
	    printf ("\t\ttabs            = %d\n", _XmEntryTabsGet(seg));
	    printf ("\t\tdirection       = %d\n", _XmEntryDirectionGet(seg));
	    printf ("\t\ttext type       = %d (%s)\n",
		    _XmEntryTextTypeGet(seg),
		    type_image(_XmEntryTextTypeGet(seg)));
	    printf ("\t\ttext            = <");
	    for (k=0; k<_XmEntryByteCountGet(seg); k++)
	      printf ("%c", ((char *)_XmEntryTextGet(seg))[k]);
	    printf (">\n");
	    printf ("\t\tbyte count      = %d\n", _XmEntryByteCountGet(seg));
	    printf ("\t\trend_end_tags   = (%d)\n",
		    _XmEntryRendEndCountGet(seg));
	    for (k=0; k<_XmEntryRendEndCountGet(seg); k++)
	      printf ("\t\t  %4d <%s>\n", _XmStringIndexCacheTag(_XmEntryRendEndGet(seg,k), XmSTRING_TAG_STRLEN), _XmEntryRendEndGet(seg,k));
	    printf ("\t\tpop after       = %d \n", _XmEntryPopGet(seg));
	  }
      }
  }
  printf("\n\n");
}

#endif /* _XmDEBUG_XMSTRING */

/****************************************************************
 * _XmStringGetTextConcat:
 * Note: at some point this could be reimplemented as two pass
 * process to eliminate calls to XtRealloc.
 ****************************************************************/
char *
_XmStringGetTextConcat(
        XmString string)
{
  _XmStringContextRec stack_context;
  XmStringComponentType type ;
  unsigned int len ;
  XtPointer val ;
  size_t OldLen ;
  size_t OutLen = 0 ;
  char * OutStr = NULL ;

  if (string) {
    _XmStringContextReInit(&stack_context, string);

    while((type = XmeStringGetComponent(&stack_context, TRUE, FALSE,
					&len, &val)) !=
	  XmSTRING_COMPONENT_END)
      {
	switch( type)
	  {
	  case XmSTRING_COMPONENT_TEXT:
	  case XmSTRING_COMPONENT_LOCALE_TEXT:
	  case XmSTRING_COMPONENT_WIDECHAR_TEXT:
	    OldLen = OutLen;
	    OutLen += len;
	    OutStr = XtRealloc( OutStr, OutLen + 1) ;
	    memcpy( &OutStr[OldLen], (char *)val, len) ;
	    OutStr[OutLen] = '\0';
	    break ;
	  default:
	    break ;
	  }
      }

    _XmStringContextFree(&stack_context);
  }
  return( OutStr) ;
}

/****************************************************************
 * Allocates a copy of the text and character set of the specified XmString
 *   if the XmString is composed of a single segment.
 * Returns TRUE if str is a single segment, FALSE otherwise.
 * If TRUE, pTextOut is valid; if FALSE, pTextOut and pTagOut are NULL.
****************/
Boolean
_XmStringSingleSegment(
        XmString str,
        char **pTextOut,
        XmStringTag *pTagOut )
{
  _XmStringContextRec stack_context ;
  Boolean           retVal;
  unsigned int	    len;
  XtPointer	    val;
  XmStringComponentType type;

  /* Initialize the return parameters. */
  retVal = FALSE;
  *pTextOut = NULL;
  *pTagOut = NULL;

  if (str)
    {
      _XmStringContextReInit(&stack_context, str);

      /** Get the first tag and text. **/
      /* Peak ahead and only copy tag or text. */
      while ((type = XmeStringGetComponent(&stack_context, FALSE, FALSE,
					   &len, &val)) !=
	     XmSTRING_COMPONENT_END)
	{
	  switch (type)
	    {
	    case XmSTRING_COMPONENT_TAG:
	    case XmSTRING_COMPONENT_LOCALE:
	      XmeStringGetComponent(&stack_context, TRUE, TRUE, &len, &val);
	      XtFree((char *)*pTagOut);
	      *pTagOut = (XmStringTag)val;
	      break;

	    case XmSTRING_COMPONENT_TEXT:
	    case XmSTRING_COMPONENT_LOCALE_TEXT:
	    case XmSTRING_COMPONENT_WIDECHAR_TEXT:
	      XmeStringGetComponent(&stack_context, TRUE, TRUE, &len, &val);

	      retVal = TRUE;
	      *pTextOut = (char *)val;

	      if (type == XmSTRING_COMPONENT_LOCALE_TEXT)
		{
		  XtFree((char *)*pTagOut);
		  *pTagOut = (XmStringTag)XtNewString(XmFONTLIST_DEFAULT_TAG);
		}

	      /* Make sure there are no more segments. */
	      while ((type = XmeStringGetComponent(&stack_context, TRUE, FALSE,
						   &len, &val)) !=
		     XmSTRING_COMPONENT_END)
		switch (type)
		  {
		  /* These are all okay */
		  case XmSTRING_COMPONENT_RENDITION_END:
		  case XmSTRING_COMPONENT_LAYOUT_POP:
		  case XmSTRING_COMPONENT_SEPARATOR:
		    break;

		  /* Anything else is a second segment. */
		  default:
		    retVal = FALSE;
		    continue;
		  }
	      continue;

	    /* Advance the context. */
	    default:
	      XmeStringGetComponent(&stack_context, TRUE, FALSE, &len, &val);
	      break;
	    }
	}

      _XmStringContextFree(&stack_context);
    }

  if (!retVal)
    {
      XtFree(*pTextOut);
      XtFree((char *)*pTagOut);
      *pTextOut = NULL;
      *pTagOut = NULL;
    }

  return retVal;
}

/****************************************************************************
 ***									  ***
 ***  This next function SUPERCEDES UpdateWMShellTitle() in BulletinB.c!  ***
 ***  REMOVE other copy and reuse for 1.2.1!				  ***
 ***									  ***
 ****************************************************************************/

#define STRING_CHARSET          "ISO8859-1"

void
XmeSetWMShellTitle(
	XmString xmstr,
	Widget shell)
{
  char *          text = (char*) NULL;
  XmStringTag	  tag = (XmStringTag) NULL;
  Arg             al[10] ;
  Cardinal        ac ;
  XrmValue        from ;
  Atom            encoding = None;
  XrmValue        to ;
  _XmWidgetToAppContext(shell);

  _XmAppLock(app);
  /* Set WMShell title (if present). */
  if(    XtIsWMShell( shell)    )
    {
      /* Shell is a Window Manager Shell, so set WMShell title
       *   from XmNdialogTitle.
       */
      text = NULL ;
      ac = 0 ;
      if (_XmStringSingleSegment(xmstr, &text, &tag))
	{
	  if ((tag != NULL) && (strcmp(STRING_CHARSET, tag) == 0))
	    {
	      /* dialog_title is a single segment of charset STRING_CHARSET,
	       *   so use atom of "STRING".  Otherwise, convert to compound
	       *   text and use atom of "COMPOUND_TEXT".
	       */
	      XtFree( (char *) tag) ;
	      encoding = XA_STRING;
	    }
	  else if ((tag != NULL) &&
		   (strcmp(XmFONTLIST_DEFAULT_TAG, tag) == 0))
	    {
	      /* dialog_title locale encoded so use constant of None */
	      XtFree((char *)tag);
	      encoding = None;
	    }
	  else
	    {
	      /* Don't need this, since dialog_title will be converted from
	       *   original XmString to compound text.
	       */
	      if (tag != NULL)
	        XtFree( (char *) tag) ;
	      XtFree( (char *) text) ;
	      text = NULL ;
	    }
	}
      if (!text)
	{
	  from.addr = (char *) xmstr;
	  if(    XmCvtXmStringToText( XtDisplay( shell), NULL, NULL,
				     &from, &to, NULL)    )
	    {
	      text = to.addr ;
	      encoding = XInternAtom(XtDisplay(shell), XmSCOMPOUND_TEXT, FALSE);
	    }
	}
      if(    text    )
        {
	  XtSetArg( al[ac], XtNtitle, text) ;  ++ac ;
	  XtSetArg( al[ac], XtNtitleEncoding, encoding) ; ++ac ;
	  XtSetArg( al[ac], XtNiconName, text) ;  ++ac ;
	  XtSetArg( al[ac], XtNiconNameEncoding, encoding) ; ++ac ;
	  XtSetValues( shell, al, ac) ;
	  XtFree( (char *) text) ;
	}
    }
    _XmAppUnlock(app);
}

/*
 * XmeGetDirection: An XmParseProc to insert a direction component.
 *	Does not consume the triggering character.
 */
XmIncludeStatus
XmeGetDirection(XtPointer     *in_out,
		XtPointer      text_end, /* unused */
		XmTextType     type,
		XmStringTag    tag,
		XmParseMapping entry, /* unused */
		int	       pattern_length, /* unused */
		XmString      *str_include,
		XtPointer      call_data) /* unused */
{
  XmCharDirectionProc char_proc = _XmOSGetCharDirection;
  XmStringDirection dir;
  (void) XmOSGetMethod(NULL, XmMCharDirection, (XtPointer *)&char_proc, NULL);

  /* Create a component for the new direction. */
  dir = XmDirectionToStringDirection((*char_proc)(*in_out, type, tag));
  *str_include = XmStringComponentCreate(XmSTRING_COMPONENT_DIRECTION,
					 sizeof (dir),
					 (XtPointer) &dir);

  /* Don't consume the triggering character. */
  return XmINSERT;
}

/*
 * match_pattern: A helper for XmStringParseText.  Determine whether
 *	the text matches a XmParseMapping pattern.
 */
static Boolean
match_pattern(XtPointer      text,
	      XmStringTag    tag, /* unused */
	      XmTextType     type,
	      XmParseMapping pattern,
	      int	     char_len,
	      Boolean	     dir_change)
{
  if (pattern == NULL)
    {
      return False;
    }
  else if (pattern->pattern == XmDIRECTION_CHANGE)
    {
      return dir_change;
    }
  else if ((pattern->pattern_type == XmWIDECHAR_TEXT) &&
	   (type == XmWIDECHAR_TEXT))
    {
      /* Compare wchar_t text to wchar_t pattern. */
      return (*((wchar_t*) text) == *((wchar_t*) pattern->pattern));
    }
  else if (type == XmWIDECHAR_TEXT)
    {
      /* Compare wchar_t text to a mbs pattern. */
      char mb_text[MB_LEN_MAX];
      wctomb(mb_text, (wchar_t) '\0');
      wctomb(mb_text, *((wchar_t*)text));
      return !strncmp(mb_text, (char*) pattern->pattern, char_len);
    }
  else if (pattern->pattern_type == XmWIDECHAR_TEXT)
    {
      /* Compare mbs text to wchar_t pattern. */
      char mb_pattern[MB_LEN_MAX];
      wctomb(mb_pattern, (wchar_t) '\0');
      wctomb(mb_pattern, *((wchar_t*)pattern->pattern));
      return !strncmp((char*) text, mb_pattern, char_len);
    }
  else if (strlen((char*) pattern->pattern) == char_len)
    {
      /* The normal case: mbs text and pattern. */
      return !strncmp((char*) text, (char*) pattern->pattern, char_len);
    }

  return False;
}

/*
 * parse_unmatched: A Helper routine for XmStringParseText.  Produce
 *	a component for characters that weren't matched by any pattern.
 */
static void
parse_unmatched(XmString  *result,
		char     **ptr,
		XmTextType text_type,
		int        length)
{
  /* Insert length bytes from ptr into result, and update ptr. */
  XmString tmp_1, tmp_2;
  XmStringComponentType ctype;

  /* Do nothing if there are no unmatched bytes. */
  if (length <= 0)
    return;

  /* Choose a component type. */
  switch (text_type)
    {
    case XmCHARSET_TEXT:
      ctype = XmSTRING_COMPONENT_TEXT;
      break;
    case XmMULTIBYTE_TEXT:
      ctype = XmSTRING_COMPONENT_LOCALE_TEXT;
      break;
    case XmWIDECHAR_TEXT:
      ctype = XmSTRING_COMPONENT_WIDECHAR_TEXT;
      break;

    default:
      return;
    }

  /* Can't concat without copying both strings? */
  tmp_1 = *result;
  tmp_2 = XmStringComponentCreate(ctype, length, (XtPointer) *ptr);
  if (tmp_2 == NULL)
    return;

  *result = XmStringConcatAndFree(tmp_1, tmp_2);
  *ptr += length;
}

/*
 * parse_pattern: A helper routine for XmStringParseText.  Process a
 *	pattern that has matched.
 */
static Boolean
parse_pattern(XmString      *result,
	      char         **ptr,
	      XtPointer      text_end,
	      XmStringTag    tag,
	      XmTextType     type,
	      XmParseMapping pat,
	      int            length,
	      XtPointer      call_data,
	      Boolean	    *terminate)
{
  /* Process a matched pattern.  Return True if ptr is updated. */
  char* orig_ptr = *ptr;
  XmIncludeStatus action = pat->include_status;
  XmString insertion = NULL;

  /* Compute the action and insertion. */
  if (action == XmINVOKE)
    {
      /* Resolve parse procs. */
      if (pat->parse_proc)
	action = (pat->parse_proc) ((XtPointer*) ptr, text_end, type,
				    tag, pat, length, &insertion, call_data);

      /* Recursive parse procs are not supported. */
      if (action == XmINVOKE)
	{
	  *ptr = orig_ptr;
	  XmStringFree (insertion);
	  return False;
	}
    }
  else
    {
      /* Non-parse_procs always advance the pointer and terminate matching. */
      *ptr += length;
      insertion = XmStringCopy(pat->substitute);
    }

  /* Insert the substitution. */
  switch (action)
    {
    case XmTERMINATE:
      *terminate = True;
      /* Fall through. */
    case XmINSERT:
      if (insertion != NULL)
	*result = XmStringConcatAndFree(*result, insertion);
      break;

    default:
      /* Ignore substitution string. */
      XmStringFree(insertion);
      break;
    }

  /* Advancing the pointer prevents multiple matches. */
  return (*ptr != orig_ptr);
}

XmString
XmStringParseText(XtPointer    text,
		  XtPointer   *text_end,
		  XmStringTag  tag,
		  XmTextType   type,
		  XmParseTable parse_table,
		  Cardinal     parse_count,
		  XtPointer    call_data)
{
  /* This routine needs to be reentrant so application supplied */
  /* XmParseProcs can make recursive calls. */
  static XmParseMapping default_dir_pattern = NULL;

  char*			 ptr = (char*) text;
  char*			 prev_ptr = ptr;
  XtPointer		 end_ptr = (text_end ? *text_end : NULL);
  XmString		 result;
  Boolean		 has_dir_pattern;
  Boolean		 wide_char = False;
  Boolean		 advanced;
  Boolean		 halt;
  unsigned int		 index;
  char*			 dir_ptr;
  XmStringComponentType  tag_type;
  XmInitialDirectionProc init_char_proc = _XmOSGetInitialCharsDirection;

  _XmProcessLock();

  /* Check some error conditions. */
  if (parse_count && !parse_table)
  {
    _XmProcessUnlock();
    return NULL;
  }
  if (!text)
  {
    _XmProcessUnlock();
    return NULL;
  }

  /* Validate the tag and set the tag_type. */
  switch (type)
    {
    case XmCHARSET_TEXT:
      if (tag == NULL)
	tag = XmFONTLIST_DEFAULT_TAG;
      tag_type = XmSTRING_COMPONENT_TAG;
      break;

    case XmWIDECHAR_TEXT:
      wide_char = True;
      /* Fall through */

    case XmMULTIBYTE_TEXT:
      /* Non-NULL values (except _MOTIF_DEFAULT_LOCALE)
         are not accepted in Motif 2.0. */
      if ((tag != NULL) && (strcmp(tag, _MOTIF_DEFAULT_LOCALE) != 0))
      {
	_XmProcessUnlock();
	return NULL;
      }

      if (tag == NULL)
	tag = _MOTIF_DEFAULT_LOCALE;
      tag_type = XmSTRING_COMPONENT_LOCALE;
      break;

    default:
      /* Error: bad text type. */
      _XmProcessUnlock();
      return NULL;
    }

  /* Create an empty segment with the right tag. */
  result = XmStringComponentCreate(tag_type, strlen(tag), (XtPointer) tag);

  /* Did the user provide an XmDIRECTION_CHANGE pattern? */
  has_dir_pattern = False;
  for (index = 0; (index < parse_count) && !has_dir_pattern; index++)
    has_dir_pattern = (parse_table[index]->pattern == XmDIRECTION_CHANGE);
  if (!has_dir_pattern && !default_dir_pattern)
    {
      /* Create a default direction pattern. */
      Arg args[10];
      Cardinal nargs = 0;

      XtSetArg(args[nargs], XmNincludeStatus, XmINVOKE),          nargs++;
      XtSetArg(args[nargs], XmNinvokeParseProc, XmeGetDirection), nargs++;
      XtSetArg(args[nargs], XmNpattern, XmDIRECTION_CHANGE),	  nargs++;
      assert(nargs < XtNumber(args));
      default_dir_pattern = XmParseMappingCreate(args, nargs);
    }

  /* Process characters until text has been consumed. */
  dir_ptr = NULL;
  (void) mblen((char*) NULL, MB_CUR_MAX);
  (void) XmOSGetMethod(NULL, XmMInitialCharsDirection,
		       (XtPointer *)&init_char_proc, NULL);
  halt = (end_ptr && (ptr >= (char*) end_ptr));
  while (!halt && (wide_char ? *((wchar_t*) ptr) : *ptr))
    {
      int len = (wide_char ? sizeof(wchar_t) : mblen(ptr, MB_CUR_MAX));
      advanced = False;

      /* If we have an invalid character, treat it as a single byte. */
      if (len < 0)
	len = 1;

      /* Reset dir_ptr if the input text has changed directions. */
      if (ptr > dir_ptr)
	{
	  XmDirection xm_dir;
	  if ((*init_char_proc)((XtPointer) ptr, type, tag, &index, &xm_dir) ==
	      Success)
	    dir_ptr = ptr + index;
	}

      /* Match against an implicit XmDIRECTION_CHANGE pattern. */
      if (!has_dir_pattern && (ptr == dir_ptr))
	{
	  parse_unmatched(&result, &prev_ptr, type, ptr - prev_ptr);
	  advanced =
	    parse_pattern(&result, &ptr, end_ptr, tag, type,
			  default_dir_pattern, len, call_data, &halt);
	}

      /* Try to match this character against the patterns. */
      for (index = 0; !advanced && !halt && (index < parse_count); index++)
	{
	  XmParseMapping pat = parse_table[index];
	  if (match_pattern(ptr, tag, type, pat, len, (ptr == dir_ptr)))
	    {
	      parse_unmatched(&result, &prev_ptr, type, ptr - prev_ptr);
	      advanced = parse_pattern(&result, &ptr, end_ptr, tag,
				       type, pat, len, call_data, &halt);

	      /* Insert the charset component after pattern insertion */
	      result = XmStringConcatAndFree(result, XmStringComponentCreate(tag_type, strlen(tag), (XtPointer) tag));
	    }
	}

      /* Match an implicit "self-insert" pattern if all else fails. */
      if (!advanced)
	{
	  /* Buffer unmatched characters into one long insertion. */
	  ptr += len;
	}
      else
	{
	  /* Discard this character and reset unmatched pointer. */
	  prev_ptr = ptr;
	}

      /* Stop processing at the end of the text. */
      halt |= (end_ptr && (ptr >= (char*) end_ptr));
    }

  /* Output and trailing unmatched characters. */
  parse_unmatched(&result, &prev_ptr, type, ptr - prev_ptr);

  /* Return the true end of parsing if possible. */
  if (text_end)
    *text_end = (XtPointer) ptr;

  _XmProcessUnlock();
  return result;
}

/*
 * check_unparse_models: A helper for XmStringUnparse.  Invoked
 *	after a text component is processed, this routine determines
 *	whether future non-text and text components will be unparsed.
 */
static void
check_unparse_models(XmStringContext context,
		     XmStringTag     tag,
		     XmTextType	     tag_type,
		     XmParseModel    parse_model,
		     Boolean	    *prev_text_match,
		     Boolean	    *next_text_match,
		     Boolean	    *non_text_match)
{
  /* Scan ahead to see whether the next text segment will match. */
  {
    /* Peek ahead in the iterator for a real text segment. */
    Boolean		  done = False;
    _XmStringContextRec   n_context;
    XtPointer             n_value;
    unsigned int          n_length;
    XmStringComponentType n_ctype;

    /* Compute text_match for the next text segment. */
    *prev_text_match = *next_text_match;
    _XmStringContextCopy(&n_context, context);
    while (!done)
      {
	n_ctype = XmeStringGetComponent(&n_context, True, False, &n_length, &n_value);
	switch (n_ctype)
	  {
	  case XmSTRING_COMPONENT_TEXT:
	  case XmSTRING_COMPONENT_LOCALE_TEXT:
	  case XmSTRING_COMPONENT_WIDECHAR_TEXT:
	    if (!tag)
	      *next_text_match = True;
	    else if ((tag_type == _XmStrContTagType(&n_context)) &&
		     (!_XmStrContTag(&n_context) ||
		      (tag == _XmStrContTag(&n_context)) ||
		      (strcmp(tag, _XmStrContTag(&n_context)) == 0)))
	      *next_text_match = True;
	    else
	      *next_text_match = False;
	    done = True;
	    break;

	  case XmSTRING_COMPONENT_END:
	    *next_text_match = False;
	    done = True;
	    break;
	  }
      }

    _XmStringContextFree(&n_context);
  }

  /* Compute parse_match for components up to the next text segment. */
  switch (parse_model)
    {
    case XmOUTPUT_ALL:
      *non_text_match = True;
      break;

    case XmOUTPUT_BETWEEN:
      *non_text_match = *prev_text_match && *next_text_match;
      break;

    case XmOUTPUT_BEGINNING:
      *non_text_match = *next_text_match;
      break;

    case XmOUTPUT_END:
      *non_text_match = *prev_text_match;
      break;

    case XmOUTPUT_BOTH:
      *non_text_match = *prev_text_match || *next_text_match;
      break;

    default:
      /* This is an error. */
      *non_text_match = False;
      break;
    }
}

/*
 * unparse_text: A helper for XmStringUnparse.  Output a matched text
 *	component.
 */
static void
unparse_text(char                **result,
	     int                  *length,
	     XmTextType            output_type,
	     XmStringComponentType c_type,
	     unsigned int	   c_length,
	     XtPointer	           c_value)
{
  /* If we have an invalid character, treat it as a single byte. */
  if ((int)c_length < 0)
    c_length = 1;

  /* Convert c_value to the appropriate type and insert it. */
  if ((c_type == XmSTRING_COMPONENT_WIDECHAR_TEXT) ==
      (output_type == XmWIDECHAR_TEXT))
    {
      /* No conversion is necessary. */
      *result = XtRealloc(*result, *length + c_length);
      memcpy(*result + *length, c_value, c_length);
      *length += c_length;
    }
  else if (output_type != XmWIDECHAR_TEXT)
    {
      /* Convert c_value to a multibyte string. */
      int len;
      int max_bytes = c_length * MB_CUR_MAX / sizeof(wchar_t);
      wchar_t *null_text = (wchar_t*) XtMalloc(c_length + sizeof(wchar_t));
      memcpy(null_text, c_value, c_length);
      null_text[c_length / sizeof(wchar_t)] = (wchar_t) '\0';

      *result = XtRealloc(*result, *length + max_bytes);
      len = wcstombs(*result + *length, null_text, max_bytes);
      if (len > 0)
	*length += len;

      XtFree((char*) null_text);
    }
  else
    {
      /* Convert c_value to a widechar string. */
      int len;
      char *null_text = XtMalloc(c_length + 1);
      memcpy(null_text, c_value, c_length);
      null_text[c_length] = '\0';

      *result = XtRealloc(*result, *length + c_length * sizeof(wchar_t));
      len = mbstowcs((wchar_t*) (*result + *length), null_text, c_length);
      if (len > 0)
	*length += len * sizeof(wchar_t);

      XtFree(null_text);
    }
}

/*
 * unparse_is_plausible: A helper routine for unparse_components.
 *	Decided whether a pattern is even eligible for unparsing.
 */
static Boolean
unparse_is_plausible(XmParseMapping pattern)
{
  /* Look for a cached result from previous computations. */
  switch (pattern->internal_flags)
    {
    case XmSTRING_UNPARSE_UNKNOWN:
      break;

    case XmSTRING_UNPARSE_PLAUSIBLE:
      return True;

    case XmSTRING_UNPARSE_IMPLAUSIBLE:
      return False;
    }

  /* Test the pattern to see if it might ever be unparsed. */
  if (/* Filter patterns based on the include status. */
      (pattern->include_status == XmINVOKE) ||

      /* Filter patterns based on the substitution. */
      (!pattern->substitute) ||

      /* Filter patterns based on the pattern. */
      (pattern->pattern == XmDIRECTION_CHANGE))
    {
      pattern->internal_flags = XmSTRING_UNPARSE_IMPLAUSIBLE;
      return False;
    }
  else
    {
      /* Give up and compare the segments component by component. */
      pattern->internal_flags = XmSTRING_UNPARSE_PLAUSIBLE;
      return True;
    }
}

/*
 * unparse_components: A helper for XmStringUnparse.  Compare
 *	components against the parse table.
 */
static void
unparse_components(char          **result,
		   int            *length,
		   XmTextType      output_type,
		   XmStringContext context,
		   XmParseTable    parse_table,
		   Cardinal        parse_count)
{
  Boolean match = False;
  XmParseMapping pat;
  int n_pat;
  int n_comp;

  /* Compare each pattern component. */
  for (n_pat = 0; !match && (n_pat < parse_count); n_pat++)
    {
      pat = parse_table[n_pat];
      if (unparse_is_plausible(pat))
	{
	  _XmStringContextRec   m_context, p_context;
	  XmStringComponentType m_ctype, p_ctype;
	  XtPointer             m_value, p_value;
	  unsigned int          m_length, p_length;

	  /* Setup master and pattern context iterators. */
	  _XmStringContextCopy(&m_context, context);
	  _XmStringContextReInit(&p_context, pat->substitute);

	  /* Iterate over each of the strings. */
	  match = True;
	  for (n_comp = 0; match; n_comp++)
	    {
	      /* Get the next component from each source. */
	      m_ctype = XmeStringGetComponent(&m_context, True, False,
				      &m_length, &m_value);
	      p_ctype = XmeStringGetComponent(&p_context, True, False,
				      &p_length, &p_value);

	      /* It's a match! */
	      if (p_ctype == XmSTRING_COMPONENT_END)
		break;

	      /* Comparison of text components always fails. */
	      if ((p_ctype == XmSTRING_COMPONENT_TEXT) ||
		  (p_ctype == XmSTRING_COMPONENT_LOCALE_TEXT) ||
		  (p_ctype == XmSTRING_COMPONENT_WIDECHAR_TEXT))
		{
		  pat->internal_flags = XmSTRING_UNPARSE_IMPLAUSIBLE;
		  match = False;
		}

	      /* Bit-compare components. */
	      else if ((m_ctype != p_ctype) ||
		       (m_length != p_length) ||
		       ((m_value != p_value) &&
			memcmp(m_value, p_value, m_length)))
		match = False;
	    }

	  /* Should we undo this substitution? */
	  if (match)
	    {
	      /* Output the original pattern. */
	      if (pat->pattern_type == XmWIDECHAR_TEXT)
		unparse_text(result, length, output_type,
			     XmSTRING_COMPONENT_WIDECHAR_TEXT,
			     sizeof(wchar_t), pat->pattern);
	      else
		unparse_text(result, length, output_type,
			     XmSTRING_COMPONENT_TEXT,
			     mblen((char*) pat->pattern, MB_CUR_MAX),
			     pat->pattern);

	      /* Skip all but the last matched component. */
	      while (--n_comp > 0)
		{
		  m_ctype = XmeStringGetComponent(context, True, False,
					  &m_length, &m_value);
		  assert(m_ctype != XmSTRING_COMPONENT_END);
		}
	    }

	  /* Cleanup. */
	  _XmStringContextFree(&m_context);
	  _XmStringContextFree(&p_context);
	}
    }
}

XtPointer
XmStringUnparse(XmString          string,
		XmStringTag       tag,
		XmTextType        tag_type,
		XmTextType        output_type,
		XmParseTable      parse_table,
		Cardinal          parse_count,
		XmParseModel      parse_model)
{
  char               *result = NULL;
  int		      length = 0;
  _XmStringContextRec stack_context;
  Boolean             prev_text_match;
  Boolean             next_text_match;
  Boolean	      non_text_match;
  Boolean	      done;

  XmStringComponentType c_type;
  unsigned int	        c_length;
  XtPointer	        c_value;

  _XmProcessLock();
  /* Convert special tags to real values. */
  if ((tag_type == XmCHARSET_TEXT) && tag &&
      ((tag == XmSTRING_DEFAULT_CHARSET) ||
       (strcmp(tag, XmSTRING_DEFAULT_CHARSET) == 0)))
    tag = _XmStringGetCurrentCharset();

  /* Process the components of string individually. */
  prev_text_match = next_text_match = non_text_match = False;
  done = (string == NULL);
  if (!done)
   {
     _XmStringContextReInit(&stack_context, string);
     check_unparse_models(&stack_context, tag, tag_type, parse_model,
			  &prev_text_match, &next_text_match, &non_text_match);
   }
  while (!done)
    {
      /* Peek at the next component. */
      c_type = XmeStringGetComponent(&stack_context, False, False,
				     &c_length, &c_value);
      switch (c_type)
	{
	case XmSTRING_COMPONENT_TEXT:
	case XmSTRING_COMPONENT_LOCALE_TEXT:
	case XmSTRING_COMPONENT_WIDECHAR_TEXT:
	  /* Text component matches are computed in advance. */
	  if (next_text_match)
	    unparse_text(&result, &length, output_type,
			 c_type, c_length, c_value);

	  /* Advance to the next component. */
	  (void) XmeStringGetComponent(&stack_context, True, False,
				       &c_length, &c_value);

	  /* Update the text match values. */
	  check_unparse_models(&stack_context, tag, tag_type, parse_model,
			       &prev_text_match, &next_text_match,
			       &non_text_match);
	  break;

	case XmSTRING_COMPONENT_END:
	  done = True;
	  /* We're done after processing this component. */
	default:
	  /* Non-text components are under the control of parse_model. */
	  if (non_text_match)
	    unparse_components(&result, &length, output_type, &stack_context,
			       parse_table, parse_count);

	  /* Advance to the next component. */
	  if (!done)
	    (void) XmeStringGetComponent(&stack_context, True, False,
					 &c_length, &c_value);

	  break;
	}
    }

  /* Clean up. */
  if (string != NULL)
   _XmStringContextFree(&stack_context);

  /* Null terminate the result. */
  switch(output_type)
    {
    case XmWIDECHAR_TEXT:
      {
	wchar_t zero = 0;
	unparse_text(&result, &length, output_type,
		     XmSTRING_COMPONENT_WIDECHAR_TEXT,
		     sizeof(wchar_t), (XtPointer) &zero);
      }
      break;

    case XmCHARSET_TEXT:
    case XmMULTIBYTE_TEXT:
    case XmNO_TEXT:
      unparse_text(&result, &length, output_type,
		   XmSTRING_COMPONENT_TEXT, 1, (XtPointer) "");
      break;
    }

  _XmProcessUnlock();
  return (XtPointer) result;
}

XmString
XmStringComponentCreate(XmStringComponentType c_type,
			unsigned int          length,
			XtPointer             value)
{
  _XmString           str;
  _XmStringUnoptSegRec seg;
  _XmStringEntry      opt_seg;
  _XmStringOptRec     opt;
  int                 tag_index = TAG_INDEX_UNSET;
  Boolean             optimized = False;
  XmStringTag         rend_tags[1];

  _XmProcessLock();
  /* We can't do anything if a needed value is missing. */
  if ((length > 0) && (value == NULL)) {
    _XmProcessUnlock();
    return NULL;
  }

  /* Initialize the proto-segments */
  _XmEntryInit((_XmStringEntry)&seg, XmSTRING_ENTRY_UNOPTIMIZED);
  _XmStrInit((_XmString)&opt, XmSTRING_OPTIMIZED);

  /* Modify a proto-segment appropriately or return a special value. */
  switch (c_type)
    {
    case XmSTRING_COMPONENT_TAG:
      if (!value || (length != strlen((char*) value))) {
        _XmProcessUnlock();
	return NULL;
      }
      if ((value == XmSTRING_DEFAULT_CHARSET) ||
	  (strcmp((char*) value, XmSTRING_DEFAULT_CHARSET) == 0)) {
	value = _XmStringGetCurrentCharset();
	length = strlen((char*) value);
      }

      tag_index = _XmStringIndexCacheTag((char*) value, length);
      optimized = (tag_index < TAG_INDEX_MAX);
      if (optimized) {
	_XmStrTextType((_XmString)&opt) = XmCHARSET_TEXT;
	_XmStrTagIndex((_XmString)&opt) = tag_index;
      } else {
	_XmEntryTextTypeSet(&seg, XmCHARSET_TEXT);
	_XmUnoptSegTag(&seg) = _XmStringCacheTag((char*) value, length);
      }
      break;

    case XmSTRING_COMPONENT_TEXT:
      optimized = (length < (1 << BYTE_COUNT_BITS));
      if (optimized) {
	_XmStrTextType((_XmString)&opt)  = XmCHARSET_TEXT;
	_XmStrByteCount((_XmString)&opt) = length;
      } else {
	_XmEntryTextTypeSet(&seg, XmCHARSET_TEXT);
	if (value != NULL) {
	  _XmEntryTextSet((_XmStringEntry)&seg, value);
	  _XmEntryByteCountSet(&seg, length);
	}
      }
      break;

    case XmSTRING_COMPONENT_DIRECTION:
      if (length != sizeof(XmStringDirection)) {
	_XmProcessUnlock();
	return NULL;
      }
      _XmProcessUnlock();
      return XmStringDirectionCreate(*((XmStringDirection*) value));

    case XmSTRING_COMPONENT_SEPARATOR:
      if (value != NULL) {
	_XmProcessUnlock();
	return NULL;
      }
      _XmProcessUnlock();
      return XmStringSeparatorCreate();

    case XmSTRING_COMPONENT_LOCALE_TEXT:
      tag_index = _XmStringIndexCacheTag(
				(char*) XmFONTLIST_DEFAULT_TAG,
				XmSTRING_TAG_STRLEN);

      if (length < (1 << BYTE_COUNT_BITS))
	optimized = (tag_index < TAG_INDEX_MAX);

      if (optimized) {
	_XmStrTextType((_XmString)&opt)  = XmMULTIBYTE_TEXT;
	_XmStrTagIndex((_XmString)&opt)  = tag_index;
	_XmStrByteCount((_XmString)&opt) = length;
      } else {
	_XmEntryTextTypeSet(&seg, XmMULTIBYTE_TEXT);
	_XmUnoptSegTag(&seg) = _tag_cache[tag_index];
	if (value != NULL) {
	  _XmEntryTextSet((_XmStringEntry)&seg, value);
	  _XmEntryByteCountSet(&seg, length);
	}
      }
      break;

    case XmSTRING_COMPONENT_LOCALE:
      if (!value || (length != strlen((char*) value))) {
	_XmProcessUnlock();
	return NULL;
      }
      if (strcmp((char*) value, _MOTIF_DEFAULT_LOCALE) != 0) {
	_XmProcessUnlock();
	return NULL;
      }

      tag_index = _XmStringIndexCacheTag((char*) value, length);
      optimized = (tag_index < TAG_INDEX_MAX);
      if (optimized) {
	_XmStrTextType((_XmString)&opt) = XmMULTIBYTE_TEXT;
	_XmStrTagIndex((_XmString)&opt) = tag_index;
      } else {
	_XmEntryTextTypeSet(&seg, XmMULTIBYTE_TEXT);
	_XmUnoptSegTag(&seg)  = _XmStringCacheTag((char*) value, length);
      }
      break;

    case XmSTRING_COMPONENT_WIDECHAR_TEXT:
      optimized = (length < (1 << BYTE_COUNT_BITS));
      if (optimized) {
	_XmStrTextType((_XmString)&opt)  = XmWIDECHAR_TEXT;
	_XmStrTagIndex((_XmString)&opt)  = tag_index;
	_XmStrByteCount((_XmString)&opt) = length;
      } else {
	_XmEntryTextTypeSet(&seg, XmWIDECHAR_TEXT);
	if (value != NULL) {
	  _XmEntryTextSet((_XmStringEntry)&seg, value);
	  _XmEntryByteCountSet(&seg, length);
	}
      }
      break;

    case XmSTRING_COMPONENT_LAYOUT_POP:
      if (value != NULL) {
	_XmProcessUnlock();
	return NULL;
      }

      /* There is no optimized representation for layout components? */
      optimized = False;
      _XmEntryPopSet(&seg, TRUE);
      break;

    case XmSTRING_COMPONENT_LAYOUT_PUSH:
      if (length != sizeof(XmDirection)) {
	_XmProcessUnlock();
	return NULL;
      }

      /* There is no optimized representation for layout components? */
      optimized = False;
      _XmEntryPushSet(&seg, *((XmDirection *)value));
      break;

    case XmSTRING_COMPONENT_RENDITION_BEGIN:
      if (!value || (length != strlen((char*)value))) {
	_XmProcessUnlock();
	return NULL;
      }

      tag_index = _XmStringIndexCacheTag((char *)value, length);
      optimized = (tag_index < REND_INDEX_MAX);
      if (optimized)
	{
	  _XmStrRendIndex((_XmString)&opt) = tag_index;
	  _XmStrRendBegin((_XmString)&opt) = TRUE;
	}
      else
	{
	  _XmUnoptSegRendBegins(&seg) = rend_tags;
	  rend_tags[0] = _XmStringCacheTag((char *)value, length);
	  _XmUnoptSegRendBeginCount(&seg) = 1;
	}
      break;

    case XmSTRING_COMPONENT_RENDITION_END:
      if (!value || (length != strlen((char*)value))) {
	_XmProcessUnlock();
	return NULL;
      }

      tag_index = _XmStringIndexCacheTag((char *)value, length);
      optimized = (tag_index < REND_INDEX_MAX);
      if (optimized)
	{
	  _XmStrRendIndex((_XmString)&opt) = tag_index;
	  _XmStrRendEnd((_XmString)&opt) = TRUE;
	}
      else
	{
	  _XmUnoptSegRendEnds(&seg) = rend_tags;
	  rend_tags[0] = _XmStringCacheTag((char *)value, length);
	  _XmUnoptSegRendEndCount(&seg) = 1;
	}
      break;

    case XmSTRING_COMPONENT_TAB:
      {
         XmString ret_val;

         if (value != NULL) {
	    _XmProcessUnlock();
	    return NULL;
         }
         ret_val = StringTabCreate();
         _XmProcessUnlock();
         return ret_val;
       }

    case XmSTRING_COMPONENT_END:
      {
         XmString ret_val;

         if (value != NULL) {
	    _XmProcessUnlock();
	    return NULL;
         }
	 ret_val = StringEmptyCreate();
	 _XmProcessUnlock();
	 return ret_val;
      }

    case XmSTRING_COMPONENT_UNKNOWN:
    default:
      _XmProcessUnlock();
      return NULL;
    }

  /* Convert one of the proto-segments into a real _XmString. */
  if (optimized)
    {
      /* Convert opt into an optimized XmString. */
      str = (_XmString) _XmStrMalloc(sizeof(_XmStringOptRec) +
				     (_XmStrByteCount((_XmString)&opt) ?
				      (_XmStrByteCount((_XmString)&opt) -
				       TEXT_BYTES_IN_STRUCT) :
				      0));

      memcpy(str, &opt, sizeof(_XmStringOptRec) - TEXT_BYTES_IN_STRUCT);
      memcpy(_XmStrText(str), value, _XmStrByteCount((_XmString)&opt));

      _XmStrRefCountSet(str, 1);
    }
  else
    {
      /* Convert seg into a non-optimized XmString. */
      _XmStrCreate(str, XmSTRING_MULTIPLE_ENTRY, 0);

      if ((opt_seg = EntryCvtToOpt((_XmStringEntry)&seg)) != NULL)
	_XmStringSegmentNew(str, 0, opt_seg, False);
      else
	_XmStringSegmentNew(str, 0, (_XmStringEntry)&seg, True);
    }

  _XmProcessUnlock();
  return (XmString) str;
}

XmStringComponentType
XmStringGetNextTriple(XmStringContext context,
		      unsigned int   *length,
		      XtPointer      *value)
{
  return XmeStringGetComponent((_XmStringContext) context, True, True, length, value);
}

/*
 * XmeStringGetComponent: A generalized implementation of XmStringGetNextTriple.
 */
XmStringComponentType
XmeStringGetComponent(_XmStringContext context,
		      Boolean	       update_context,
		      Boolean	       copy_data,
		      unsigned int    *length,
		      XtPointer       *value)
{
  short			   tmp_index;
  Boolean		   optimized;
  char			   state;
  Boolean		   last_seg, last_line;
  Boolean                  pop_dir;
  XmDirection              push_dir;
  XmStringDirection	   dir;
  XmStringTag		   tag = NULL;
  int			   char_count = 0;
  XmTextType		   text_type = 0;
  char 		          *seg_text = NULL;
  XmStringTag	          *begin_rends = NULL;
  short			   begin_count = 0;
  XmStringTag	          *end_rends = NULL;
  short			   end_count = 0;
  unsigned char		   tabs;
  _XmString		   opt = NULL;
  _XmStringEntry 	   seg = NULL;
  _XmStringArraySegRec	   array_seg;
  Boolean		   skip;

  _XmProcessLock();
  /* Initialize the out parameters. */
  if (length)	*length = 0;
  if (value)	*value  = NULL;

  /* No NULL pointers allowed. */
  if (! (length  && value)) {
    _XmProcessUnlock();
    return XmSTRING_COMPONENT_END;
  }

  /* We may be done already. */
  if (_XmStrContError(context)) {
    _XmProcessUnlock();
    return XmSTRING_COMPONENT_END;
  }

  /* Gather the current segment information. */
  state = _XmStrContState(context);
  optimized = _XmStrContOpt(context);
  if (optimized) {
    XmStringTag *rend_tag = NULL;

    opt = (_XmString) _XmStrContString(context);

    last_seg    = True;
    last_line   = True;
    /* Only lookup pop_dir when we need it. */
    /* Only lookup push_dir when we need it. */
    /* Only lookup dir when we need it. */
    /* Only lookup tag when we need it. */
    char_count  = _XmStrByteCount(opt);
    text_type   = (XmTextType) _XmStrTextType(opt);
    seg_text    = _XmStrText(opt);
    begin_count = _XmStrRendBegin(opt);
    end_count   = _XmStrRendEnd(opt);
    if (begin_count || end_count) {
      assert(_XmStrRendIndex(opt) != REND_INDEX_UNSET);
      rend_tag = _tag_cache + _XmStrRendIndex(opt);
    }
    begin_rends = (begin_count ? rend_tag : NULL);
    end_rends   = (end_count ? rend_tag : NULL);
    /* Only lookup tabs when we need it. */
  } else {
    _XmString str = _XmStrContString(context);
    _XmStringEntry line;

    /* If we've run off the end we're done. */
    if (_XmStrContCurrLine(context) >= _XmStrLineCountGet(str)) {
      if (update_context)
	_XmStrContError(context) = TRUE;
      _XmProcessUnlock();
      return XmSTRING_COMPONENT_END;
    }

    if (_XmStrImplicitLine(str))
      {
	line = _XmStrEntry(str)[_XmStrContCurrLine(context)];
      }
    else
      {
	_XmEntryType(&array_seg) = XmSTRING_ENTRY_ARRAY;
	_XmEntrySegmentCount(&array_seg) = _XmStrEntryCount(str);
	_XmEntrySegment(&array_seg) = (_XmStringNREntry *)_XmStrEntry(str);
	line = (_XmStringEntry)&array_seg;
      }

    last_line = (_XmStrContCurrLine(context) + 1 >= _XmStrLineCountGet(str));
    if (_XmEntryMultiple(line))
      last_seg = (_XmStrContCurrSeg(context) + 1 >=_XmEntrySegmentCount(line));
    else
      last_seg = True;

    if (_XmEntryMultiple(line) && _XmEntrySegmentCount(line) == 0) {
      /* Empty lines are separators. */
      state = SEP_STATE;

      /* The normal state variables shouldn't be referenced. */
    } else {
      /* Extract data from a real segment. */
      if (_XmEntryMultiple(line))
	seg =
	  (_XmStringEntry)_XmEntrySegment(line)[_XmStrContCurrSeg(context)];
      else
	seg = line;

      /* Only lookup pop_dir when we need it. */
      /* Only lookup push_dir when we need it. */
      /* Only lookup dir when we need it. */
      /* Only lookup tag when we need it. */
      char_count  = _XmEntryByteCountGet(seg);
      text_type   = (XmTextType) _XmEntryTextTypeGet(seg);
      seg_text    = (char*) _XmEntryTextGet(seg);
      begin_count = _XmEntryRendBeginCountGet(seg);
      begin_rends = _XmEntryRendCountedBegins(seg, begin_count);
      end_count   = _XmEntryRendEndCountGet(seg);
      end_rends   = _XmEntryRendCountedEnds(seg, end_count);
      /* Only lookup tabs when we need it. */
    }
  }

  /* Return the next non-default component. */
  switch (state)
    {
    case PUSH_STATE:
      push_dir = (optimized ? 0 : _XmEntryPushGet(seg));
      if (push_dir != 0)
	{
	  if (copy_data)
	    {
	      XmDirection* tmp = XtNew(XmDirection);
	      *tmp = push_dir;
	      *value = (XtPointer) tmp;
	      *length = sizeof(XmDirection);
	    }
	  else
	    {
	      _XmStrContTmpDir(context) = push_dir;
	      *value = (XtPointer) &_XmStrContTmpDir(context);
	      *length = sizeof(XmDirection);
	    }
	  if (update_context)
	    {
	      _XmStrContState(context) = BEGIN_REND_STATE;
	      _XmStrContRendIndex(context) = 0;
	    }
          _XmProcessUnlock();
	  return XmSTRING_COMPONENT_LAYOUT_PUSH;
	}
      /* Fall through if no push components exist. */

    case BEGIN_REND_STATE:
      tmp_index = ((_XmStrContState(context) == BEGIN_REND_STATE) ?
		   _XmStrContRendIndex(context) : 0);
      if (tmp_index < begin_count)
	{
	  /* Process another rendition start. */
	  if (copy_data)
	    *value = (XtPointer) XtNewString(begin_rends[tmp_index]);
	  else
	    *value = (XtPointer) begin_rends[tmp_index];
	  *length = strlen((char*) *value);

	  if (update_context)
	    {
	      /* Add this rendition to the list of active renditions. */
	      begin_context_rends(context, update_context,
				  begin_rends + tmp_index, 1);

	      _XmStrContState(context) = BEGIN_REND_STATE;
	      _XmStrContRendIndex(context) = tmp_index + 1;
	    }

          _XmProcessUnlock();
	  return XmSTRING_COMPONENT_RENDITION_BEGIN;
	}
      /* Fall through if there are no more rendition starts. */

    case TAG_STATE:
      /* Don't output implicit leading charset component. */
      tag = (optimized ? _XmStrTagGet(opt) : _XmEntryTag(seg));
      if ((tag == XmSTRING_DEFAULT_CHARSET) ||
	  (tag && !strcmp((char*) tag, XmSTRING_DEFAULT_CHARSET)))
	tag = _XmStringGetCurrentCharset();

      if ((text_type != XmNO_TEXT &&
	   text_type != _XmStrContTagType(context)) ||
	  (tag && (tag != _XmStrContTag(context)) &&
	   (!_XmStrContTag(context) || strcmp(tag, _XmStrContTag(context)))))
	{
	  skip = (tag == NULL);

	  assert(tag != XmSTRING_DEFAULT_CHARSET);

	  /* If we have MB text with FONTLIST_DEFAULT_TAG, we really have
	   * oldstyle locale_text so don't output tag component, but set
	   * context if necessary for GetNextSegment. */
	  if ((tag == XmFONTLIST_DEFAULT_TAG) &&
	      (text_type == XmMULTIBYTE_TEXT))
	    {
	      skip = TRUE;
	      _XmStrContTag(context) = tag;
	    }

	  if (!skip)
	    {
	      /* Tag is changing. */
	      if (copy_data)
		*value = (XtPointer) XtNewString(tag);
	      else
		*value = (XtPointer) tag;
	      *length = strlen(tag);

	      if (update_context)
		{
		  _XmStrContTag(context)       = tag;
		  _XmStrContTagType(context)   = text_type;
		  _XmStrContState(context)     = TAB_STATE;
		  _XmStrContTabCount(context)  = 0;
		}

	      _XmProcessUnlock();
	      return ((text_type == XmCHARSET_TEXT) ?
		      XmSTRING_COMPONENT_TAG : XmSTRING_COMPONENT_LOCALE);
	    }
	}
      /* Fall through if no tag set. */

    case TAB_STATE:
      tmp_index = ((_XmStrContState(context) == TAB_STATE) ?
		   _XmStrContTabCount(context) : 0);
      tabs = (optimized ? _XmStrTabs(opt) : _XmEntryTabsGet(seg));
      if (tmp_index < tabs)
	{
	  /* A Tab precedes this segment. */
	  if (update_context)
	    {
	      _XmStrContState(context) = TAB_STATE;
	      _XmStrContTabCount(context) = tmp_index + 1;
	    }

	  _XmProcessUnlock();
	  return XmSTRING_COMPONENT_TAB;
	}
      /* Fall through if there are no tabs. */

    case DIR_STATE:
      dir = (optimized ? _XmStrDirection(opt) : _XmEntryDirectionGet(seg));
      if (dir != _XmStrContDir(context))
	{
	  skip = FALSE;

	  /* Try to resolve unset directions. */
	  if (dir == XmSTRING_DIRECTION_UNSET)
	    {
	      if ((char_count > 0) ||
		  (_XmStrContDir(context) == XmSTRING_DIRECTION_UNSET))
		{
		  XmCharDirectionProc char_proc = _XmOSGetCharDirection;
		  (void)XmOSGetMethod(NULL, XmMCharDirection,
				      (XtPointer *)&char_proc, NULL);

		  if (state > TAG_STATE)
		    tag = (optimized ? _XmStrTagGet(opt) : _XmEntryTag(seg));

		  dir = XmDirectionToStringDirection
		    ((*char_proc)(seg_text, text_type, tag));
		}
	      else skip = TRUE;
	    }

	  if (!skip)
	    {
	      /* Direction is changing. */
	      if (copy_data)
		{
		  XmStringDirection* tmp = XtNew(XmStringDirection);
		  *tmp = dir;
		  *value = (XtPointer) tmp;
		  *length = sizeof(XmStringDirection);
		}
	      else
		{
		  _XmStrContTmpStrDir(context) = dir;
		  *value = (XtPointer) &_XmStrContTmpStrDir(context);
		  *length = sizeof(XmStringDirection);
		}
	      if (update_context)
		{
		  _XmStrContDir(context) = dir;
		  _XmStrContState(context) = TEXT_STATE;
		}
	      _XmProcessUnlock();
	      return XmSTRING_COMPONENT_DIRECTION;
	    }
	}
      /* Fall through if no direction set. */

    case TEXT_STATE:
      switch (text_type)
	{
	case XmCHARSET_TEXT:
	case XmMULTIBYTE_TEXT:
	case XmWIDECHAR_TEXT:
	  if (copy_data)
	    {
	      char* tmp = XtMalloc(char_count + sizeof(wchar_t));
	      memcpy(tmp, seg_text, char_count);
	      bzero(tmp + char_count, sizeof(wchar_t));
	      *value = (XtPointer) tmp;
	    }
	  else
	    {
	      if (seg_text != NULL) *value = seg_text;
	      else *value = XmS;
	    }
	  *length = char_count;

	  if (update_context)
	    {
	      _XmStrContState(context) = END_REND_STATE;
	      _XmStrContRendIndex(context) = 0;
	    }

	  switch (text_type)
	    {
	    case XmCHARSET_TEXT:
	      _XmProcessUnlock();
	      return XmSTRING_COMPONENT_TEXT;
	    case XmMULTIBYTE_TEXT:
	      _XmProcessUnlock();
	      return XmSTRING_COMPONENT_LOCALE_TEXT;
	    case XmWIDECHAR_TEXT:
	      _XmProcessUnlock();
	      return XmSTRING_COMPONENT_WIDECHAR_TEXT;
	    case XmNO_TEXT:
	      assert(FALSE);
	    }

	case XmNO_TEXT:
	  break;

	default:
	  /* Something is wrong! */
	  assert(False);
	  if (update_context)
	    _XmStrContError(context) = True;
	  _XmProcessUnlock();
	  return XmSTRING_COMPONENT_END;
	}
      /* Fall through if there is no text. */

    case END_REND_STATE:
      tmp_index = ((_XmStrContState(context) == END_REND_STATE) ?
		   _XmStrContRendIndex(context) : 0);
      if (tmp_index < end_count)
	{
	  /* Process another rendition end. */
	  if (copy_data)
	    *value = (XtPointer) XtNewString(end_rends[tmp_index]);
	  else
	    *value = (XtPointer) end_rends[tmp_index];
	  *length = strlen((char*) *value);

	  if (update_context)
	    {
	      /* Remove this rendition from the list of active renditions. */
	      end_context_rends(context, update_context,
				end_rends + tmp_index, 1);

	      _XmStrContState(context) = END_REND_STATE;
	      _XmStrContRendIndex(context) = tmp_index + 1;
	    }

	  _XmProcessUnlock();
	  return XmSTRING_COMPONENT_RENDITION_END;
	}
      /* Fall through if there are no more rendition ends. */

    case POP_STATE:
      pop_dir = (optimized ? 0 : _XmEntryPopGet(seg));
      if (pop_dir)
	{
	  /* A pop layout direction follows this segment. */
	  if (update_context)
	    _XmStrContState(context) = SEP_STATE;

	  _XmProcessUnlock();
	  return XmSTRING_COMPONENT_LAYOUT_POP;
	}
      /* Fall through if there is no pop layout direction. */

    case SEP_STATE:
      /* This is the last possible component for a segment. */
      if (last_seg && last_line)
	{
	  /* Separators only appear between lines. */
	  if (update_context)
	    _XmStrContError(context) = True;

	  _XmProcessUnlock();
	  return XmSTRING_COMPONENT_END;
	}
      else if (last_seg && _XmStrImplicitLine(_XmStrContString(context)))
	{
	  /* Advance to the next line. */
	  if (update_context)
	    {
	      _XmStrContState(context) = PUSH_STATE;
	      _XmStrContCurrSeg(context) = 0;
	      _XmStrContCurrLine(context)++;
	    }

	  _XmProcessUnlock();
	  return XmSTRING_COMPONENT_SEPARATOR;
	}
      else
	{
	  /* Try the next segment of this line recursively. */
	  XmStringComponentType answer;
	  char saved_state = _XmStrContState(context);
	  unsigned short saved_seg = _XmStrContCurrSeg(context);

	  _XmStrContState(context) = PUSH_STATE;
	  _XmStrContCurrSeg(context)++;

	  answer = XmeStringGetComponent(context, update_context, copy_data,
				 length, value);

	  if (!update_context)
	    {
	      _XmStrContState(context) = saved_state;
	      _XmStrContCurrSeg(context) = saved_seg;
	    }
	  _XmProcessUnlock();
	  return answer;
	}
      /*NOTREACHED*/
      assert(False);

    default:
      /* An unknown _XmStrContState? */
      assert(False);
      if (update_context)
	_XmStrContError(context) = True;
      _XmProcessUnlock();
      return XmSTRING_COMPONENT_END;
    }
}

/*
 * _XmStringContextReInit: Initialize an allocated _XmStringContext.
 */
void
_XmStringContextReInit(_XmStringContext context,
		       _XmString	string)
{
  assert(context != NULL);
  bzero((char*) context, sizeof(_XmStringContextRec));

  _XmStrContString(context) = string;
  _XmStrContOpt(context)    = _XmStrOptimized(string);
  _XmStrContDir(context)    = XmSTRING_DIRECTION_UNSET;
}

/*
 * _XmStringContextCopy: Copy allocated _XmStringContexts.  The active
 *	rendition list is always copied because expanding it to
 *	contain new entries via XtRealloc may free the old pointer.
 *	Use _XmStringContextFree() to deallocate storage.
 */
void
_XmStringContextCopy(_XmStringContext target,
		     _XmStringContext source)
{
  int size;
  assert(source && target && (source != target));

  /* Copy the normal fields. */
  memcpy(target, source, sizeof(_XmStringContextRec));

  /* Copy the active renditions list so we can modify it. */
  if (_XmStrContRendCount(target) > 0) {
     size = sizeof(XmStringTag) * _XmStrContRendCount(target);
     _XmStrContRendTags(target) = (XmStringTag*) XtMalloc(size);
     memcpy(_XmStrContRendTags(target), _XmStrContRendTags(source), size);
  }
}

/*
 * _XmStringContextFree: Deallocate an _XmStringContext's internal storage.
 */
void
_XmStringContextFree(_XmStringContext context)
{
  assert(context);

  /* Free the active rendition list. */
  if (_XmStrContRendTags(context)) {
     XtFree((char*) _XmStrContRendTags(context));
  }
  _XmStrContRendTags(context) = NULL;
}

/*
 * begin_context_rends: Update an _XmStringContext to reflect some
 *	newly active renditions.
 */
static void
begin_context_rends(_XmStringContext context,
		    Boolean	     update_context,
		    XmStringTag     *rends,
		    int		     count)
{
  /* Append these renditions the context's list of active renditions. */
  _XmStrContRendTags(context) = (XmStringTag*)
    XtRealloc((char*) _XmStrContRendTags(context),
	      sizeof(XmStringTag) * (_XmStrContRendCount(context) + count));
  memcpy(_XmStrContRendTags(context) + _XmStrContRendCount(context),
	 rends,
	 sizeof(XmStringTag) * count);

  /* Update the total number only if we're advancing the context. */
  if (update_context)
    _XmStrContRendCount(context) += count;
}

/*
 * end_context_rends: Remove some renditions from an _XmStringContext's
 *	list of active renditions.
 */
static void
end_context_rends(_XmStringContext context,
		  Boolean	   update_context,
		  XmStringTag     *rends,
		  int		   count)
{
  int n_rend, n_tag;
  int i;

  /* Check some simple error conditions. */
  if (!update_context || (count <= 0))
    return;

  /* Remove the last matching instance of each rendition. */
  for (n_rend = 0; n_rend < count; n_rend++)
    {
      /* Renditions are cached, so we can compare pointers. */
      n_tag = _XmStrContRendCount(context);
      while (--n_tag >= 0)
	if (_XmStrContRendTags(context)[n_tag] == rends[n_rend])
	  {
	    /* Delete rendition n_tag from the context's list. */
	    for (i = n_tag; i < (_XmStrContRendCount(context) - 1); i++)
	      _XmStrContRendTags(context)[i] =
		_XmStrContRendTags(context)[i + 1];

	    _XmStrContRendCount(context)--;
	  }
    }
}

XmString
XmStringGenerate(XtPointer   text,
		 XmStringTag tag,
		 XmTextType  type,
		 XmStringTag rendition)
{
  XmString result;
  int table_size;
  XmParseTable gen_table;
  int i;

  _XmProcessLock();
  /*
  ** Get the parse table shared by generate and ungenerate.
  */
  table_size = _get_generate_parse_table (&gen_table);

  /* Parse the text into an XmString. */
  result = XmStringParseText
    (text, NULL, tag, type, gen_table, table_size, NULL);

  /* If no rendition was supplied return the parsetext result. */
  if (rendition == NULL) {
    _XmProcessUnlock();
    return result;
  }

  /* Try to wrap this rendition around an optimized result. */
  if (_XmStrOptimized(result) && (_XmStrRendIndex(result) == REND_INDEX_UNSET))
    {
      unsigned int rend_index;
      assert (!_XmStrRendBegin(result) && !_XmStrRendEnd(result));

      rend_index = _XmStringIndexCacheTag((char *)rendition,
					  XmSTRING_TAG_STRLEN);
      if (rend_index < REND_INDEX_MAX)
	{
	  _XmStrRendIndex(result) = rend_index;
	  _XmStrRendBegin(result) = _XmStrRendEnd(result) = True;
    	  _XmProcessUnlock();
	  return result;
	}
    }

  /* Try to wrap this rendition around an unoptimized result. */
  if (!_XmStrOptimized(result))
    {
      /* We only know how to do this if there is at least one segment. */
      XmStringTag cached_rend =
	_XmStringCacheTag(rendition, XmSTRING_TAG_STRLEN);
      int n_line;
      _XmStringEntry line;
      _XmStringEntry seg;

      /* Locate the first segment. */
      for (n_line = 0; n_line < _XmStrEntryCount(result); n_line++)
	{
	  line = _XmStrEntry(result)[n_line];
	  if (_XmEntrySegmentCountGet(line) > 0)
	    {
	      /* Prepend rendition_begin to the first segment. */
	      if (_XmStrImplicitLine(result))
		seg = (_XmStringEntry)_XmEntrySegmentGet(line)[0];
	      else seg = line;

	      if (_XmEntryOptimized(seg) &&
		  _XmEntryRendIndex(seg) == REND_INDEX_UNSET) {
		unsigned int rend_index;
		assert (!_XmEntryRendBeginCountGet(seg) &&
			!_XmEntryRendEndCountGet(seg));

		rend_index = _XmStringIndexCacheTag((char *)rendition,
						    XmSTRING_TAG_STRLEN);
		if (rend_index < REND_INDEX_MAX) {
		  _XmEntryRendIndex(seg) = rend_index;
		  _XmEntryRendBeginCountSet(seg, 1);
		}
	      } else {
		if (_XmEntryOptimized(seg)) {
		  _XmStringEntry new_seg = EntryCvtToUnopt(seg);
		  _XmStringEntryFree(seg);
		  seg = new_seg;
		  if (_XmEntryMultiple(line))
		    _XmEntrySegment(line)[0] = (_XmStringNREntry)seg;
		  else
		    _XmStrEntry(result)[n_line] = seg;
		}
		_XmUnoptSegRendBegins(seg) = (XmStringTag*)
		  XtRealloc((char*) _XmUnoptSegRendBegins(seg),
			    (_XmUnoptSegRendBeginCount(seg) + 1)*
			    sizeof(XmStringTag));
		/* Put rendition first in begins. */
		for (i = 0; i < _XmUnoptSegRendBeginCount(seg); i++)
		  _XmUnoptSegRendBegins(seg)[i + 1] =
		    _XmUnoptSegRendBegins(seg)[i];
		_XmUnoptSegRendBegins(seg)[0] = cached_rend;
		_XmUnoptSegRendBeginCount(seg)++;
	      }
	      break;
	    }
	}
      /* Locate the last segment. */
      n_line = _XmStrEntryCount(result);
      while (--n_line >= 0)
	{
	  line = _XmStrEntry(result)[n_line];
	  if (_XmEntrySegmentCountGet(line) > 0)
	    {
	      /* Append rendition_end to the last segment. */
	      if (_XmStrImplicitLine(result))
		seg = (_XmStringEntry)
		  _XmEntrySegmentGet(line)[_XmEntrySegmentCountGet(line)-1];
	      else seg = line;

	      if (_XmEntryOptimized(seg))
		{
		  unsigned int rend_index;
		  rend_index = _XmStringIndexCacheTag((char *)rendition,
						      XmSTRING_TAG_STRLEN);

		  assert ((_XmEntryRendBeginCountGet(seg) <= 1) &&
			  (_XmEntryRendEndCountGet(seg) == 0));




		if (((_XmEntryRendIndex(seg) == REND_INDEX_UNSET) ||
		     (_XmEntryRendIndex(seg) == rend_index)) &&
		    (rend_index < REND_INDEX_MAX)) {
		  _XmEntryRendIndex(seg) = rend_index;
		  _XmEntryRendEndCountSet(seg, 1);
    	  	  _XmProcessUnlock();
		  return result;
		} else {
		  break;
		}
	      } else {
		if (_XmEntryOptimized(seg)) {
		  _XmStringEntry new_seg = EntryCvtToUnopt(seg);
		  if (_XmEntryMultiple(line))
		    _XmEntrySegment(line)[0] = (_XmStringNREntry)new_seg;
		  else
		    _XmStrEntry(result)[n_line] = new_seg;
		  _XmStringEntryFree(seg);
		  seg = new_seg;
		}
		_XmUnoptSegRendEnds(seg) = (XmStringTag*)
		  XtRealloc((char*) _XmUnoptSegRendEnds(seg),
			    (_XmUnoptSegRendEndCount(seg) + 1) *
			    sizeof(XmStringTag));
		_XmUnoptSegRendEnds(seg)[_XmUnoptSegRendEndCount(seg)] =
		  cached_rend;
		_XmUnoptSegRendEndCount(seg)++;
	  	_XmProcessUnlock();
		return result;
	      }
	    }
	}
    }

  /* As a last resort merge the rendition components normally. */
  {
    XmString tmp_1, tmp_2;

    /* Prepend the rendition begin. */
    tmp_1 = XmStringComponentCreate(XmSTRING_COMPONENT_RENDITION_BEGIN,
				    strlen(rendition), rendition);
    tmp_2 = result;
    result = XmStringConcatAndFree(tmp_1, tmp_2);

    /* Append the rendition end. */
    tmp_1 = result;
    tmp_2 = XmStringComponentCreate(XmSTRING_COMPONENT_RENDITION_END,
				    strlen(rendition), rendition);
    result = XmStringConcatAndFree(tmp_1, tmp_2);
  }
  _XmProcessUnlock();
  return result;
}

XtPointer
_XmStringUngenerate(XmString    string,
		    XmStringTag tag,
		    XmTextType  tag_type,
		    XmTextType  output_type)
{
  XtPointer result;
  int table_size;
  XmParseTable gen_table;

  /*
  ** Get the parse table shared by generate and ungenerate.
  */
  table_size = _get_generate_parse_table (&gen_table);

  /* Unparse the XmString into text. */
  result = XmStringUnparse
    (string, tag, tag_type, output_type, gen_table, table_size, XmOUTPUT_ALL);

  /*
  ** It might be useful to figure out rendition here to return the reverse of
  ** what came in for XmStringGenerate.  I'm not real sure about how to do that
  ** and it isn't necessary for the immediate needs of CSText, so...
  ** RJS
  */
  return result;

}

XmParseMapping
XmParseMappingCreate(ArgList  arg_list,
		     Cardinal arg_count)
{
  /* Allocate and initialize the return value. */
  XmParseMapping result = XtNew(_XmParseMappingRec);
  bzero((char*)result, sizeof(_XmParseMappingRec));

  /* Default values are established by bzero().
   *
   * result->pattern        = XmDIRECTION_CHANGE = NULL;
   * result->pattern_type   = XmCHARSET_TEXT;
   * result->substitute     = NULL;
   * result->parse_proc     = NULL;
   * result->client_data    = NULL;
   * result->include_status = XmINSERT;
   * result->internal_flags = XmSTRING_UNPARSE_UNKNOWN;
   */

  /* Insert specified values. */
  XmParseMappingSetValues(result, arg_list, arg_count);

  return result;
}

void
XmParseMappingSetValues(XmParseMapping mapping,
			ArgList        arg_list,
			Cardinal       arg_count)
{
  register Cardinal i;
  register String arg_name;
  Cardinal unknown = 0;

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
	mapping->pattern = (XtPointer) arg_list[i].value;
      else if ((arg_name == XmNpatternType) ||
	       (strcmp(arg_name, XmNpatternType) == 0))
	mapping->pattern_type = (XmTextType) arg_list[i].value;
      else if ((arg_name == XmNsubstitute) ||
	       (strcmp(arg_name, XmNsubstitute) == 0))
	mapping->substitute = XmStringCopy((XmString) arg_list[i].value);
      else if ((arg_name == XmNinvokeParseProc) ||
	       (strcmp(arg_name, XmNinvokeParseProc) == 0))
	mapping->parse_proc = (XmParseProc) arg_list[i].value;
      else if ((arg_name == XmNclientData) ||
	       (strcmp(arg_name, XmNclientData) == 0))
	mapping->client_data = (XtPointer) arg_list[i].value;
      else if ((arg_name == XmNincludeStatus) ||
	       (strcmp(arg_name, XmNincludeStatus) == 0))
	mapping->include_status = (XmIncludeStatus) arg_list[i].value;
      else
	unknown++;
    }

  /* If there were any known values reset internal_flags. */
  if (unknown < arg_count)
    mapping->internal_flags = XmSTRING_UNPARSE_UNKNOWN;
  _XmProcessUnlock();
}

static int
_get_generate_parse_table (XmParseTable *gen_table)
/*
**
** Utility function to build and supply the parse table shared by
** XmStringGenerate and _XmStringUngenerate.  All of the information about
** the size and real storage of the table is maintained here.
**
*/
{
  int table_size = 2;
  Arg args[10];
  Cardinal nargs;
  XmString tmp;
  int index = 0;
  static XmParseTable table = NULL;


  _XmProcessLock();
  /* Allocate a parse table only if necessary. */
  if (table)
    {
      *gen_table = table;
      _XmProcessUnlock();
      return table_size;
    }
  else
    {
      table = (XmParseTable) XtCalloc (table_size, sizeof(XmParseMapping));
      *gen_table = table;
    }
  _XmProcessUnlock();

  /* Parse tab characters. */
  tmp = XmStringComponentCreate(XmSTRING_COMPONENT_TAB, 0, NULL);
  nargs = 0;
  XtSetArg(args[nargs], XmNincludeStatus, XmINSERT), nargs++;
  XtSetArg(args[nargs], XmNsubstitute, tmp), 	 nargs++;
  XtSetArg(args[nargs], XmNpattern, "\t"), 		 nargs++;
  assert(nargs < XtNumber(args));
  _XmProcessLock();
  table[index++] = XmParseMappingCreate(args, nargs);
  _XmProcessUnlock();
  XmStringFree(tmp);

  /* Parse newline characters. */
  tmp = XmStringSeparatorCreate();
  nargs = 0;
  XtSetArg(args[nargs], XmNincludeStatus, XmINSERT), nargs++;
  XtSetArg(args[nargs], XmNsubstitute, tmp),	 nargs++;
  XtSetArg(args[nargs], XmNpattern, "\n"),		 nargs++;
  assert(nargs < XtNumber(args));
  _XmProcessLock();
  table[index++] = XmParseMappingCreate(args, nargs);
  _XmProcessUnlock();

  assert(index == table_size);

  return (table_size);
}

/* Destructively truncates str to be n bytes or less, insuring that it
   remains a legal ASN.1 encoding. */
unsigned char *
_XmStringTruncateASN1(unsigned char *str, int n)
{
  unsigned char    *a = str;
  unsigned short   used , delta, d1;
  unsigned char    *new_c, *a_end;
  unsigned char    *ap;
  unsigned char	   d2;
  short    	   head_size;
  int	     	   len, length, header;

  if (a == NULL) return((unsigned char *)NULL);
  if (n < ASNHEADERLEN + CSSHORTLEN) return((unsigned char *)NULL);

  head_size = used = _read_header_length(a);
  len = _read_string_length(a);

  ap = _read_header(a);

  a_end = ((unsigned char *) a) + len + head_size;

  length = _read_asn1_length(ap);
  header = _asn1_size(length);

  /* Read the components adding up their lengths. */
  while (((length + header) < (n - used)) && (ap < a_end))
    {
      new_c = _read_component(ap, &d2, &d1, NULL);

      delta = length + header;
      used += delta;
      ap = new_c;
      length = _read_asn1_length(ap);
      header = _asn1_size(length);
    }

  if ((head_size == (ASNHEADERLEN + CSLONGLEN)) &&
      ((used - head_size) <= MAXSHORTVALUE))
    {
      /* Have to reallocate string. */
      unsigned char	*tmp;
      short		diff = (CSLONGLEN - CSSHORTLEN);

      used -= diff;

      tmp = (unsigned char *)XtMalloc(used * sizeof(unsigned char));
      memcpy(tmp, (str + diff), used);
      XtFree((char *)str);
      str = tmp;
    }
  else
    {
      str = (unsigned char *)XtRealloc((char *)str, used);
    }

  _write_header(str, used);

  return (str);
}

