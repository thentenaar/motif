/* $XConsortium: ResEncod.c /main/8 1996/11/12 05:37:04 pascale $ */
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
#include <limits.h>
#include <ctype.h>

#include <Xm/XmosP.h>
#include "MessagesI.h"
#include "ResEncodI.h"
#include "XmI.h"
#include "XmosI.h"
#include "XmStringI.h"
#include "ResConverI.h"

#define MSG8    _XmMMsgResConvert_0007
#define MSG9    _XmMMsgResConvert_0008
#define MSG10   _XmMMsgResConvert_0009
#define MSG11   _XmMMsgResConvert_0010
#define MSG13   _XmMMsgResConvert_0012
#define MSG14   _XmMMsgResConvert_0013

typedef unsigned char Octet;
typedef Octet *OctetPtr;
typedef const Octet *const_OctetPtr;

typedef enum {
    ct_Dir_StackEmpty,
    ct_Dir_Undefined,
    ct_Dir_LeftToRight,
    ct_Dir_RightToLeft
} ct_Direction;

/*
** ct_Charset is used in the xmstring_to_text conversion to keep track
** of the prevous character set.  The order is not important.
*/
typedef enum {
    cs_none,
    cs_Hanzi,
    cs_JapaneseGCS,
    cs_Katakana,
    cs_KoreanGCS,
    cs_Latin1,
    cs_Latin2,
    cs_Latin3,
    cs_Latin4,
    cs_Latin5,
    cs_LatinArabic,
    cs_LatinCyrillic,
    cs_LatinGreek,
    cs_LatinHebrew,
    cs_NonStandard,
    cs_ir111
} ct_Charset;

/* Internal context block */
typedef struct _ct_context {
    OctetPtr	    octet;	 /* octet ptr into compound text stream */
    OctetPtr	    lastoctet;		/* ptr to last octet in stream */
    struct {				/* flags */
	unsigned    dircs	: 1;	/* direction control seq encountered */
	unsigned    gchar	: 1;	/*   graphic characters encountered */
	unsigned    ignext	: 1;	/*   ignore extensions */
	unsigned    gl		: 1;	/*   text is for gl */
	unsigned    text	: 1;	/*   current item is a text seq */
    } flags;
    ct_Direction    *dirstack;		/* direction stack pointer */
    unsigned int    dirsp;		/* current dir stack index */
    unsigned int    dirstacksize;	/* size of direction stack */
    OctetPtr        encoding;           /* ptr to current encoding sequence */
    unsigned int    encodinglen;        /* length of encoding sequence */
    OctetPtr	    item;		/* ptr to current item */
    unsigned int    itemlen;		/* length of current item */
    unsigned int    version;		/* version of compound text */
    const char*   gl_charset;		/* ptr to GL character set */
    unsigned char   gl_charset_size;	/* # of chars in GL charset */
    unsigned char   gl_octets_per_char;	/* # of octets per GL char */
    const char*   gr_charset;		/* ptr to GR character set */
    unsigned char   gr_charset_size;	/* # of chars in GR charset */
    unsigned char   gr_octets_per_char;	/* # of octets per GR char */
    XmString	    xmstring;		/* compound string to be returned */
    XmString	    xmsep;		/* compound string separator segment */
    XmString	    xmtab;		/* compound string tab segment */
} ct_context;

/*
 *    Segment Encoding Registry datatype and macros
 */

typedef struct _EncodingRegistry {
  char                        *fontlist_tag;
  char                        *ct_encoding;
  struct _EncodingRegistry    *next;
} SegmentEncoding;

#define EncodingRegistryTag(er)       ((SegmentEncoding *)(er))->fontlist_tag
#define EncodingRegistryEncoding(er)  ((SegmentEncoding *)(er))->ct_encoding
#define EncodingRegistryNext(er)      ((SegmentEncoding *)(er))->next

/*
** Define standard character set strings
*/

static const char CS_ISO8859_1[] = "ISO8859-1" ;
static const char CS_ISO8859_2[] = "ISO8859-2" ;
static const char CS_ISO8859_3[] = "ISO8859-3" ;
static const char CS_ISO8859_4[] = "ISO8859-4" ;
static const char CS_ISO8859_5[] = "ISO8859-5" ;
static const char CS_ISO8859_6[] = "ISO8859-6" ;
static const char CS_ISO8859_7[] = "ISO8859-7" ;
static const char CS_ISO8859_8[] = "ISO8859-8" ;
static const char CS_ISO8859_9[] = "ISO8859-9" ;
static const char CS_JISX0201[] = "JISX0201.1976-0" ;
static const char CS_GB2312_0[] = "GB2312.1980-0" ;
static const char CS_GB2312_1[] = "GB2312.1980-1" ;
static const char CS_JISX0208_0[] = "JISX0208.1983-0" ;
static const char CS_JISX0208_1[] = "JISX0208.1983-1" ;
static const char CS_KSC5601_0[] = "KSC5601.1987-0" ;
static const char CS_KSC5601_1[] = "KSC5601.1987-1" ;
static const char CS_UTF_8[] = "UTF-8" ;
static const char CS_ISO_IR_111[] = "ISO-IR-111" ;



/* Define handy macros (note: these constants are in OCTAL) */
#define EOS	00
#define STX	02
#define HT	011
#define NL	012
#define ESC	033
#define CSI	0233

static const Octet NEWLINESTRING[] = "\012";
#define NEWLINESTRING_LEN		sizeof(NEWLINESTRING)-1

static const Octet TABSTRING[] = "\011";
#define TABSTRING_LEN		sizeof(TABSTRING)-1

static const Octet CTEXT_L_TO_R[] = "\233\061\135";
#define CTEXT_L_TO_R_LEN		sizeof(CTEXT_L_TO_R)-1

static const Octet CTEXT_R_TO_L[] = "\233\062\135";
#define CTEXT_R_TO_L_LEN		sizeof(CTEXT_R_TO_L)-1

static const Octet CTEXT_SET_ISO8859_1[] = "\033\050\102\033\055\101";
#define CTEXT_SET_ISO8859_1_LEN		sizeof(CTEXT_SET_ISO8859_1)-1

static const Octet CTEXT_SET_ISO8859_2[] = "\033\050\102\033\055\102";
#define CTEXT_SET_ISO8859_2_LEN		sizeof(CTEXT_SET_ISO8859_2)-1

static const Octet CTEXT_SET_ISO8859_3[] = "\033\050\102\033\055\103";
#define CTEXT_SET_ISO8859_3_LEN		sizeof(CTEXT_SET_ISO8859_3)-1

static const Octet CTEXT_SET_ISO8859_4[] = "\033\050\102\033\055\104";
#define CTEXT_SET_ISO8859_4_LEN		sizeof(CTEXT_SET_ISO8859_4)-1

static const Octet CTEXT_SET_ISO8859_5[] = "\033\050\102\033\055\114";
#define CTEXT_SET_ISO8859_5_LEN		sizeof(CTEXT_SET_ISO8859_5)-1

static const Octet CTEXT_SET_ISO8859_6[] = "\033\050\102\033\055\107";
#define CTEXT_SET_ISO8859_6_LEN		sizeof(CTEXT_SET_ISO8859_6)-1

static const Octet CTEXT_SET_ISO8859_7[] = "\033\050\102\033\055\106";
#define CTEXT_SET_ISO8859_7_LEN		sizeof(CTEXT_SET_ISO8859_7)-1

static const Octet CTEXT_SET_ISO8859_8[] = "\033\050\102\033\055\110";
#define CTEXT_SET_ISO8859_8_LEN		sizeof(CTEXT_SET_ISO8859_8)-1

static const Octet CTEXT_SET_ISO8859_9[] = "\033\050\102\033\055\115";
#define CTEXT_SET_ISO8859_9_LEN		sizeof(CTEXT_SET_ISO8859_9)-1

static const Octet CTEXT_SET_JISX0201[] = "\033\050\112\033\051\111";
#define CTEXT_SET_JISX0201_LEN		sizeof(CTEXT_SET_JISX0201)-1

static const Octet CTEXT_SET_GB2312_0[] = "\033\044\050\101\033\044\051\101";
#define CTEXT_SET_GB2312_0_LEN		sizeof(CTEXT_SET_GB2312_0)-1

static const Octet CTEXT_SET_JISX0208_0[] = "\033\044\050\102\033\044\051\102";
#define CTEXT_SET_JISX0208_0_LEN	sizeof(CTEXT_SET_JISX0208_0)-1

static const Octet CTEXT_SET_KSC5601_0[] = "\033\044\050\103\033\044\051\103";
#define CTEXT_SET_KSC5601_0_LEN		sizeof(CTEXT_SET_KSC5601_0)-1

static const Octet CTEXT_SET_IR_111[] = "\033\050\102\033\055\100";
#define CTEXT_SET_IR_111_LEN		sizeof(CTEXT_SET_IR_111)-1

#if XM_UTF8
static const char UTF8_NEWLINESTRING[] = "\012";
#define UTF8_NEWLINESTRING_LEN		sizeof(UTF8_NEWLINESTRING)-1

static const char UTF8_TABSTRING[] = "\011";
#define UTF8_TABSTRING_LEN		sizeof(UTF8_TABSTRING)-1

static const char UTF8_L_TO_R[] = "\342\200\216";
#define UTF8_L_TO_R_LEN		sizeof(UTF8_L_TO_R)-1

static const char UTF8_R_TO_L[] = "\342\200\217";
#define UTF8_R_TO_L_LEN		sizeof(UTF8_R_TO_L)-1
#endif /* XM_UTF8 */

#define CTVERSION 1
#define _IsValidC0(ctx, c)	(((c) == HT) || ((c) == NL) || ((ctx)->version > CTVERSION))
#define _IsValidC1(ctx, c)	((ctx)->version > CTVERSION)

#define _IsValidESCFinal(c)	(((c) >= 0x30) && ((c) <= 0x7e))
#define _IsValidCSIFinal(c)	(((c) >= 0x40) && ((c) <= 0x7e))

#define _IsInC0Set(c)		((c) <= 0x1f)
#define _IsInC1Set(c)		(((c) >= 0x80) && ((c) <= 0x9f))
#define _IsInGLSet(c)		(((c) >= 0x20) && ((c) <= 0x7f))
#define _IsInGRSet(c)		((c) >= 0xa0)
#define _IsInColumn2(c)		(((c) >= 0x20) && ((c) <= 0x2f))
#define _IsInColumn3(c)		(((c) >= 0x30) && ((c) <= 0x3f))
#define _IsInColumn4(c)		(((c) >= 0x40) && ((c) <= 0x4f))
#define _IsInColumn5(c)		(((c) >= 0x50) && ((c) <= 0x5f))
#define _IsInColumn6(c)		(((c) >= 0x60) && ((c) <= 0x6f))
#define _IsInColumn7(c)		(((c) >= 0x70) && ((c) <= 0x7f))
#define _IsInColumn4or5(c)	(((c) >= 0x40) && ((c) <= 0x5f))


#define _SetGL(ctx, charset, size, octets)\
    (ctx)->flags.gl = True;\
    (ctx)->gl_charset = (charset);\
    (ctx)->gl_charset_size = (size);\
    (ctx)->gl_octets_per_char = (octets)

#define _SetGR(ctx, charset, size, octets)\
    (ctx)->flags.gl = False;\
    (ctx)->gr_charset = (charset);\
    (ctx)->gr_charset_size = (size);\
    (ctx)->gr_octets_per_char = (octets)

#define _PushDir(ctx, dir)\
    if ( (ctx)->dirsp == ((ctx)->dirstacksize - 1) ) {\
	(ctx)->dirstacksize += 8;\
	(ctx)->dirstack = \
	    (ct_Direction *)XtRealloc((char *)(ctx)->dirstack,\
				(ctx)->dirstacksize * sizeof(ct_Direction));\
    }\
    (ctx)->dirstack[++((ctx)->dirsp)] = dir;\
    (ctx)->flags.dircs = True

#define _PopDir(ctx)	((ctx)->dirsp)--

#define _CurDir(ctx)	(ctx)->dirstack[(ctx)->dirsp]

/* this should probably be the other way around, (XmFONTLIST_DEFAULT_TAG map to
   _MOTIF_DEFAULT_LOCALE) but this is the smallest code change, and the code
   will not work any differently */

/* Define the MIT registered charset */

static SegmentEncoding _mit_ISO_IR_1111_registry =
{ "ISO-IR-111", "ISO-IR-111", NULL};
static SegmentEncoding _mit_UTF_8_registry =
{ "UTF-8", "UTF-8", &_mit_ISO_IR_1111_registry};
static SegmentEncoding _mit_KSC5601_1987_1_registry =
{ "KSC5601.1987-1", "KSC5601.1987-1", &_mit_UTF_8_registry};
static SegmentEncoding _mit_KSC5601_1987_0_registry =
{ "KSC5601.1987-0", "KSC5601.1987-0", &_mit_KSC5601_1987_1_registry};
static SegmentEncoding _mit_JISX0208_1983_1_registry =
{ "JISX0208.1983-1", "JISX0208.1983-1", &_mit_KSC5601_1987_0_registry};
static SegmentEncoding _mit_JISX0208_1983_0_registry =
{ "JISX0208.1983-0", "JISX0208.1983-0", &_mit_JISX0208_1983_1_registry};
static SegmentEncoding _mit_GB2312_1980_1_registry =
{ "GB2312.1980-1", "GB2312.1980-1", &_mit_JISX0208_1983_0_registry};
static SegmentEncoding _mit_GB2312_1980_0_registry =
{ "GB2312.1980-0", "GB2312.1980-0", &_mit_GB2312_1980_1_registry};
static SegmentEncoding _mit_JISX0201_1976_0_registry =
{ "JISX0201.1976-0", "JISX0201.1976-0", &_mit_GB2312_1980_0_registry};
static SegmentEncoding _mit_ISO8859_9_registry =
{ "ISO8859-9", "ISO8859-9", &_mit_JISX0201_1976_0_registry};
static SegmentEncoding _mit_ISO8859_8_registry =
{ "ISO8859-8", "ISO8859-8", &_mit_ISO8859_9_registry};
static SegmentEncoding _mit_ISO8859_7_registry =
{ "ISO8859-7", "ISO8859-7", &_mit_ISO8859_8_registry};
static SegmentEncoding _mit_ISO8859_6_registry =
{ "ISO8859-6", "ISO8859-6", &_mit_ISO8859_7_registry};
static SegmentEncoding _mit_ISO8859_5_registry =
{ "ISO8859-5", "ISO8859-5", &_mit_ISO8859_6_registry};
static SegmentEncoding _mit_ISO8859_4_registry =
{ "ISO8859-4", "ISO8859-4", &_mit_ISO8859_5_registry};
static SegmentEncoding _mit_ISO8859_3_registry =
{ "ISO8859-3", "ISO8859-3", &_mit_ISO8859_4_registry};
static SegmentEncoding _mit_ISO8859_2_registry =
{ "ISO8859-2", "ISO8859-2", &_mit_ISO8859_3_registry};
static SegmentEncoding _mit_ISO8859_1_registry =
{ "ISO8859-1", "ISO8859-1", &_mit_ISO8859_2_registry};

static SegmentEncoding _loc_encoding_registry =
{ _MOTIF_DEFAULT_LOCALE, XmFONTLIST_DEFAULT_TAG, &_mit_ISO8859_1_registry};
static SegmentEncoding _encoding_registry =
{ XmFONTLIST_DEFAULT_TAG, XmFONTLIST_DEFAULT_TAG, &_loc_encoding_registry};
static SegmentEncoding *_encoding_registry_ptr = &_encoding_registry;


/********    Static Function Declarations    ********/

static SegmentEncoding * FindEncoding(
                        char *fontlist_tag) ;
static Boolean processCharsetAndText(XmStringTag tag,
				     OctetPtr	ctext,
				     Boolean	separator,
				     OctetPtr	*outc,
				     unsigned int	*outlen,
				     ct_Charset	*prev);
#if XM_UTF8
static Boolean processCharsetAndTextUtf8(XmStringTag tag,
				     OctetPtr	ctext,
				     Boolean	separator,
				     OctetPtr	*outc,
				     unsigned int	*outlen,
				     ct_Charset	*prev);
#endif

static Boolean processESCHack(
                        ct_context *ctx,
                        Octet final) ;
static Boolean processExtendedSegmentsHack(
                        ct_context *ctx,
                        Octet final) ;
static Boolean cvtTextToXmString(
                        XrmValue *from,
                        XrmValue *to) ;
static void outputXmString(
                        ct_context *ctx,
                        Boolean separator) ;
static XmString concatStringToXmString(
                        XmString compoundstring,
                        char *textstring,
			int textlen,
                        char *charset,
                        XmStringDirection direction,
                        Boolean separator) ;
static Boolean processESC(
                        ct_context *ctx,
                        Octet final) ;
static Boolean processCSI(
                        ct_context *ctx,
                        Octet final) ;
static Boolean processExtendedSegments(
                        ct_context *ctx,
                        Octet final) ;
static Boolean process94n(
                        ct_context *ctx,
                        Octet final) ;
static Boolean process94GL(
                        ct_context *ctx,
                        Octet final) ;
static Boolean process94GR(
                        ct_context *ctx,
                        Octet final) ;
static Boolean process96GR(
                        ct_context *ctx,
                        Octet final) ;
static Boolean cvtXmStringToText(
                        XrmValue *from,
                        XrmValue *to) ;
static OctetPtr ctextConcat(
                        OctetPtr str1,
                        unsigned int str1len,
                        const_OctetPtr str2,
                        unsigned int str2len) ;

#if XM_UTF8
static Boolean  cvtXmStringToUTF8String(
        XrmValue *from,
        XrmValue *to);
#endif

/********    End Static Function Declarations    ********/

/************************************************************************
 *
 *  FindEncoding
 *    Find the SegmentEncoding with fontlist_tag.  Return NULL if no
 *    such SegmentEncoding exists.  As a side effect, free any encodings
 *    encountered that have been unregistered.
 *
 ************************************************************************/
static SegmentEncoding *
FindEncoding(char *fontlist_tag)
{
  SegmentEncoding     *prevPtr, *encodingPtr = _encoding_registry_ptr;
  String              encoding = NULL;

  if (encodingPtr)
    {
      if (strcmp(fontlist_tag, EncodingRegistryTag(encodingPtr)) == 0)
      {
        encoding = EncodingRegistryEncoding(encodingPtr);

        /* Free unregistered encodings. */
        if (encoding == NULL)
          {
            _encoding_registry_ptr = EncodingRegistryNext(encodingPtr);
            XtFree( (char *) encodingPtr);
            encodingPtr = NULL;
          }

        return(encodingPtr);
      }
    }
  else return(encodingPtr);

  for (prevPtr = encodingPtr, encodingPtr = EncodingRegistryNext(encodingPtr);
       encodingPtr != NULL;
       prevPtr = encodingPtr, encodingPtr = EncodingRegistryNext(encodingPtr))
    {
      if (strcmp(fontlist_tag, EncodingRegistryTag(encodingPtr)) == 0)
      {
        encoding = EncodingRegistryEncoding(encodingPtr);

        /* Free unregistered encodings. */
        if (encoding == NULL)
          {
            EncodingRegistryNext(prevPtr) =
              EncodingRegistryNext(encodingPtr);
            XtFree( (char *) encodingPtr);
            encodingPtr = NULL;
          }

        return(encodingPtr);
      }
      /* Free unregistered encodings. */
      else if (EncodingRegistryEncoding(encodingPtr) == NULL)
      {
        EncodingRegistryNext(prevPtr) = EncodingRegistryNext(encodingPtr);
        XtFree( (char *) encodingPtr);
      }
    }

  return(NULL);
}


/************************************************************************
 *
 *  XmRegisterSegmentEncoding
 *    Register a compound text encoding format for a specified font list
 *    element tag.  Returns NULL for a new tag or a copy of the old encoding
 *    for an already registered tag.
 *
 ************************************************************************/
char *
XmRegisterSegmentEncoding(
     char     *fontlist_tag,
     char     *ct_encoding)
{
  SegmentEncoding     *encodingPtr = NULL;
  String              ret_val = NULL;

  _XmProcessLock();
  encodingPtr = FindEncoding(fontlist_tag);

  if (encodingPtr)
    {
      ret_val = XtNewString(EncodingRegistryEncoding(encodingPtr));
      EncodingRegistryEncoding(encodingPtr) =
      ct_encoding ? XtNewString(ct_encoding) : (String)NULL;
    }
  else if (ct_encoding != NULL)
    {
      encodingPtr =
      (SegmentEncoding *)XtMalloc((Cardinal)sizeof(SegmentEncoding));
      EncodingRegistryTag(encodingPtr) = XtNewString(fontlist_tag);
      EncodingRegistryEncoding(encodingPtr) = XtNewString(ct_encoding);

      EncodingRegistryNext(encodingPtr) = _encoding_registry_ptr;
      _encoding_registry_ptr = encodingPtr;
    }

  _XmProcessUnlock();
  return(ret_val);
}



/************************************************************************
 *
 * _XmGetEncodingRegistryTarget returns the current encoding registry
 * as a list of NULL separated items.  The length is returned through
 * the passed length pointer.
 *
 ************************************************************************/
XtPointer
_XmGetEncodingRegistryTarget(int *length)
{
  int i, count, total_size;
  SegmentEncoding *current;
  char *rval;

  total_size = 0;

  _XmProcessLock();
  current = _encoding_registry_ptr;
  while(current != NULL) {
    total_size += strlen(EncodingRegistryTag(current)) +
      strlen(EncodingRegistryEncoding(current)) + 2;
    current = EncodingRegistryNext(current);
  }

  *length = total_size;

  /* Create output buffer large enough for all the
     pairs of tags and encodings */
  rval = XtMalloc(sizeof(char) * total_size);

  i = 0;
  current = _encoding_registry_ptr;
  while(current != NULL) {
    count = strlen(EncodingRegistryTag(current));
    strcpy(&rval[i], EncodingRegistryTag(current));
    i += count;
    rval[i] = 0;
    i++;
    count = strlen(EncodingRegistryEncoding(current));
    strcpy(&rval[i], EncodingRegistryEncoding(current));
    i += count;
    rval[i] = 0;
    i++;
    current = EncodingRegistryNext(current);
  }

  _XmProcessUnlock();
  return((XtPointer) rval);
}

/************************************************************************
 *
 *  XmMapSegmentEncoding
 *    Returns the compound text encoding format associated with the
 *    specified font list element tag.  Returns NULL if not found.
 *
 ************************************************************************/
char *
XmMapSegmentEncoding(char        *fontlist_tag)
{
  SegmentEncoding     *encodingPtr = NULL;
  String              ret_val = NULL;

  _XmProcessLock();
  encodingPtr = FindEncoding(fontlist_tag);

  if (encodingPtr)
    ret_val = XtNewString(EncodingRegistryEncoding(encodingPtr));

  _XmProcessUnlock();
  return(ret_val);
}


/************************************************************************
 *
 *  XmCvtCTToXmString
 *	Convert a compound text string to a XmString.  This is the public
 *	version which takes only a compound text string as an argument.
 *	Note: processESC and processExtendedSegments have to be hacked
 *	for this to work.
 *
 ************************************************************************/
XmString
XmCvtCTToXmString(
        char *text )
{
    ct_context	    *ctx;		/* compound text context block */
    Boolean	    ok = True;
    Octet	    c;
    XmString	    xmsReturned;	/* returned Xm string */

    ctx = (ct_context *) XtMalloc(sizeof(ct_context));

/* initialize the context block */
    ctx->octet = (OctetPtr)text;
    ctx->flags.dircs = False;
    ctx->flags.gchar = False;
    ctx->flags.ignext = False;
    ctx->flags.gl = False;
    ctx->flags.text = False;
    ctx->dirstacksize = 8;
    ctx->dirstack = (ct_Direction *)
            XtMalloc(ctx->dirstacksize*sizeof(ct_Direction));
/*
 * Define XLIB_HANDLES_DIRECTION if vendor's X library knows how
 * to deal with direction control sequences in CT. Otherwise
 * no such ones will be output if there are no Rtol segments
 * in the XmString. Note that the MIT sample implementation
 * does not deal with direction control sequences...
 */
#ifdef XLIB_HANDLES_DIRECTION
    ctx->dirstack[0] = ct_Dir_StackEmpty;
    ctx->dirsp = 0;
#else
    ctx->dirstack[0] = ct_Dir_StackEmpty;
    ctx->dirstack[1] = ct_Dir_LeftToRight;
    ctx->dirsp = 1;
#endif
    ctx->encoding = NULL;
    ctx->encodinglen = 0;
    ctx->item = NULL;
    ctx->itemlen = 0;
    ctx->version = CTVERSION;
    ctx->gl_charset = CS_ISO8859_1;
    ctx->gl_charset_size = 94;
    ctx->gl_octets_per_char = 1;
    ctx->gr_charset = CS_ISO8859_1;
    ctx->gr_charset_size = 96;
    ctx->gr_octets_per_char = 1;
    ctx->xmstring = NULL;
    ctx->xmsep = NULL;
    ctx->xmtab = NULL;

/*
** check for version/ignore extensions sequence (must be first if present)
**  Format is:	ESC 02/03 V 03/00   ignoring extensions is OK
**		ESC 02/03 V 03/01   ignoring extensions is not OK
**  where V is in the range 02/00 thru 02/15 and represents versions 1 thru 16
*/
    if (    (ctx->octet[0] == ESC)
	&&  (ctx->octet[1] == 0x23)
	&&  (_IsInColumn2(ctx->octet[2])
	&&  ((ctx->octet[3] == 0x30) || ctx->octet[3] == 0x31))
       ) {
	ctx->version = ctx->octet[2] - 0x1f;	/* 0x20-0x2f => version 1-16 */
	if (ctx->octet[3] == 0x30)		/* 0x30 == can ignore extensions */
	    ctx->flags.ignext = True;
	ctx->octet += 4;			/* advance ptr to next seq */
    }


    while (ctx->octet[0] != 0) {
    switch (*ctx->octet) {			/* look at next octet in seq */
	case ESC:
	    /* %%% TEMP
	    ** if we have any text to output, do it
	    ** this section needs to be optimized so that it handles
	    ** paired character sets without outputting a new segment.
	    */
	    if (ctx->flags.text) {
		outputXmString(ctx, False);	/* with no separator */
	    }
	    ctx->flags.text = False;
	    ctx->item = ctx->octet;		/* remember start of this item */
	    ctx->itemlen = 0;

	    ctx->octet++; ctx->itemlen++;	/* advance ptr to next char */

	    /* scan for final char */
	    while (	(ctx->octet[0] != 0)
		     && (_IsInColumn2(*ctx->octet)) ) {
		ctx->octet++; ctx->itemlen++;	/* advance ptr to next char */
	    }

	    if (ctx->octet[0] == 0) {	/* if nothing after this, it's an error */
		ok = False;
		break;
	    }

	    c = *ctx->octet;			/* get next char in seq */
	    ctx->octet++; ctx->itemlen++;	/* advance ptr to next char */
	    if (_IsValidESCFinal(c)) {
		/* we have a valid ESC sequence - handle it */
		ok = processESCHack(ctx, c);
	    } else {
		ok = False;
	    }
	    if (ok) {
	      ctx->encoding = ctx->item;
	      ctx->encodinglen = ctx->itemlen;
	    }
	    break;

	case CSI:
	    /*
	    ** CSI format is:	CSI P I F   where
	    **	    03/00 <= P <= 03/15
	    **	    02/00 <= I <= 02/15
	    **	    04/00 <= F <= 07/14
	    */
	    /* %%% TEMP
	    ** if we have any text to output, do it
	    ** This may need optimization.
	    */
	    if (ctx->flags.text) {
		/* check whether we have a specific direction set */
                if (((ctx->octet[1] == 0x31) && (ctx->octet[2] == 0x5d))||
                    ((ctx->octet[1] == 0x32) && (ctx->octet[2] == 0x5d))||
                    (ctx->octet[1] == 0x5d))
                        outputXmString(ctx, False);    /* without a separator*/
                else
			outputXmString(ctx, True);	/* with a separator */
	    }
	    ctx->flags.text = False;
	    ctx->item = ctx->octet;		/* remember start of this item */
	    ctx->itemlen = 0;

	    ctx->octet++; ctx->itemlen++;	/* advance ptr to next char */

	    /* scan for final char */
	    while (	(ctx->octet[0] != 0)
		    &&	_IsInColumn3(*ctx->octet)  ) {
		ctx->octet++; ctx->itemlen++;	/* advance ptr to next char */
	    }
	    while (	(ctx->octet[0] != 0)
		    && _IsInColumn2(*ctx->octet)   ) {
		ctx->octet++; ctx->itemlen++;	/* advance ptr to next char */
	    }

	    /* if nothing after this, it's an error */
	    if (ctx->octet[0] == 0) {
		ok = False;
		break;
	    }

	    c = *ctx->octet;			/* get next char in seq */
	    ctx->octet++; ctx->itemlen++;	/* advance ptr to next char */
	    if (_IsValidCSIFinal(c)) {
		/* we have a valid CSI sequence - handle it */
		ok = processCSI(ctx, c);
	    } else {
		ok = False;
	    }
	    break;

	case NL:			    /* new line */
	    /* if we have any text to output, do it */
	    if (ctx->flags.text) {
	      outputXmString(ctx, True);	/* with a separator */
	      ctx->flags.text = False;
	    } else {
	      if (ctx->xmsep == NULL) {
		ctx->xmsep = XmStringSeparatorCreate();
	      }
	      ctx->xmstring = XmStringConcatAndFree(ctx->xmstring,
						    XmStringCopy(ctx->xmsep));
	    }
	    ctx->octet++;			/* advance ptr to next char */
	    break;

	case HT:
	    /* if we have any text to output, do it */
	    if (ctx->flags.text) {
	      outputXmString(ctx, False);
	      ctx->flags.text = False;
	    }
	    if (ctx->xmtab == NULL) {
	      ctx->xmtab = XmStringComponentCreate(XmSTRING_COMPONENT_TAB, 0,
						   NULL);
	    }
	    ctx->xmstring = XmStringConcatAndFree(ctx->xmstring,
						  XmStringCopy(ctx->xmtab));
	    ctx->octet++;			/* advance ptr to next char */
	    break;

	default:			    /* just 'normal' text */
	    ctx->item = ctx->octet;		/* remember start of this item */
	    ctx->itemlen = 0;
	    ctx->flags.text = True;
	    while (ctx->octet[0] != 0) {
		c = *ctx->octet;
		if ((c == ESC) || (c == CSI) || (c == NL) || (c == HT)) {
		    break;
		}
		if (	(_IsInC0Set(c) && (!_IsValidC0(ctx, c)))
		    ||	(_IsInC1Set(c) && (!_IsValidC1(ctx, c))) ) {
		    ok = False;
		    break;
		}
		ctx->flags.gchar = True;	/* We have a character! */

                /*
                 *  We should look at the actual character to
                 *  decide whether it's a gl or gr character.
                 *
                 *  We'll hit the problem if we get a CT that
                 *  isn't generated by Motif.
                 */
                if (isascii((unsigned char)c)) {
		    ctx->itemlen += ctx->gl_octets_per_char;
		    ctx->octet += ctx->gl_octets_per_char;
		} else {
		    ctx->octet += ctx->gr_octets_per_char;
		    ctx->itemlen += ctx->gr_octets_per_char;
		}
	    } /* end while */
	    break;
	} /* end switch */
    if (!ok) break;
    } /* end while */

/* if we have any text left to output, do it */
    if (ctx->flags.text) {
	outputXmString(ctx, False);		/* with no separator */
    }

    XtFree((char *) ctx->dirstack);
    if (ctx->xmsep != NULL) XmStringFree(ctx->xmsep);
    if (ctx->xmtab != NULL) XmStringFree(ctx->xmtab);
    xmsReturned = (XmString)ctx->xmstring;
    XtFree((char *) ctx);

    if (ok)
      return ( xmsReturned );
    else
	return ( (XmString)NULL );

}


/***********************************************************************
 *
 * Hacked procedures to work with XmCvtCTToXmString.
 *
 ***********************************************************************/

/* processESCHack - handle valid ESC sequences */
static Boolean
processESCHack(
        ct_context *ctx,
        Octet final )
{
    Boolean	    ok;

    switch (ctx->item[1]) {
    case 0x24:			/* 02/04 - invoke 94(n) charset into GL or GR */
	ok = process94n(ctx, final);
	break;
    case 0x25:			/* 02/05 - extended segments */
	/* if we have any text to output, do it */
	if (ctx->flags.text) {
	    outputXmString(ctx, False);	/* with no separator */
	    ctx->flags.text = False;
	}
	ok = processExtendedSegmentsHack(ctx, final);
	break;
    case 0x28:			/* 02/08 - invoke 94 charset into GL */
	ok = process94GL(ctx, final);
	break;
    case 0x29:			/* 02/09 - invoke 94 charset into GR */
	ok = process94GR(ctx, final);
	break;
    case 0x2d:			/* 02/13 - invoke 96 charset into GR */
	ok =  process96GR(ctx, final);
	break;
    default:
	ok = False;
	break;
    }
    return(ok);
}


static Boolean
processExtendedSegmentsHack(
        ct_context *ctx,
        Octet final )
{
    OctetPtr	    esptr;			/* ptr into ext seg */
    unsigned int    seglen;			/* length of ext seg */
    unsigned int    len;			/* length */
    String	    charset_copy;		/* ptr to NULL-terminated copy of ext seg charset */
    OctetPtr	    text_copy;			/* ptr to NULL-terminated copy of ext seg text */
    XmString	    tempxm1;
    Boolean	    ok = True;

    /* Extended segments
    **  01/11 02/05 02/15 03/00 M L	    variable # of octets/char
    **  01/11 02/05 02/15 03/01 M L	    1 octet/char
    **  01/11 02/05 02/15 03/02 M L	    2 octets/char
    **  01/11 02/05 02/15 03/03 M L	    3 octets/char
    **  01/11 02/05 02/15 03/04 M L	    4 octets/char
    */
    if (	(ctx->itemlen == 4)
	&&	(ctx->item[2] == 0x2f)
	&&	(_IsInColumn3(final))
       )
      {
	if (    (ctx->octet[0] < 0x80)
	    ||  (ctx->octet[1] < 0x80)
	   )
	  {
	    return(False);
	  }

	/*
	** The most significant bit of M and L are always set to 1
	** The number is computed as ((M-128)*128)+(L-128)
	*/
	seglen = *ctx->octet - 0x80;
	ctx->octet++; ctx->itemlen++;		/* advance pointer */
	seglen = (seglen << 7) + (*ctx->octet - 0x80);
	ctx->octet++; ctx->itemlen++;		/* advance pointer */

	/* Check for premature end. */
	for (esptr = ctx->octet; esptr < (ctx->octet + seglen); esptr++)
	  {
	    if (*esptr == 0)
	      {
		return(False);
	      }
	  }

        esptr = ctx->octet;			/* point to charset */
	ctx->itemlen += seglen;			/* advance pointer over segment */
	ctx->octet += seglen;

	switch (final) {
	case 0x30:				/* variable # of octets per char */
	case 0x31:				/* 1 octet per char */
	case 0x32:				/* 2 octets per char */
	    /* scan for STX separator between charset and text */
	    len = 0;
	    while (esptr[len] != STX)
		len++;
	    if (len > ctx->itemlen) {		/* if we ran off the end, error */
		ok = False;
		break;
	    }
	    charset_copy = XtMalloc(len + 1);
	    strncpy(charset_copy, (char *) esptr, len);
	    charset_copy[len] = EOS;
	    esptr += len + 1;			/* point to text part */
	    len = seglen - len - 1;		/* calc length of text part */

	    /* For two-octets charsets, make sure the text
	     * contains an integral number of characters. */
            if (final == 0x32 && len % 2) {
	      XtFree(charset_copy);
	      return (False);
            }

	    text_copy = (unsigned char *) XtMalloc(len + 1);
	    memcpy( text_copy, esptr, len);
	    text_copy[len] = EOS;
	    tempxm1 = XmStringConcatAndFree(
		         XmStringDirectionCreate((_CurDir(ctx) ==
						  ct_Dir_LeftToRight ?
						  XmSTRING_DIRECTION_L_TO_R :
						  (_CurDir(ctx) ==
						   ct_Dir_RightToLeft ?
						   XmSTRING_DIRECTION_R_TO_L :
						   XmSTRING_DIRECTION_UNSET))),
			 XmStringCreate((char *)text_copy, charset_copy));
	    ctx->xmstring = XmStringConcatAndFree(ctx->xmstring, tempxm1);
	    XtFree((char *) text_copy);
	    XtFree((char *) charset_copy);
	    ok = True;
	    break;

	case 0x33:				/* 3 octets per char */
	case 0x34:				/* 4 octets per char */
	    /* not supported */
	    ok = False;
	    break;

	default:
	    /* reserved for future use */
	    ok = False;
	    break;
	} /* end switch */
    } /* end if */

    return(ok);
}


/************************************************************************
 *
 *  XmCvtTextToXmString
 *	Convert a compound text string to a XmString.
 *
 ************************************************************************/
Boolean
XmCvtTextToXmString(
        Display *display,
        XrmValuePtr args,	/* unused */
        Cardinal *num_args,	/* unused */
        XrmValue *from_val,
        XrmValue *to_val,
        XtPointer *converter_data ) /* unused */
{
    Boolean		ok;

    if (from_val->addr == NULL)
	return( FALSE);

    ok = cvtTextToXmString(from_val, to_val);

    if (!ok)
    {
	to_val->addr = NULL;
	to_val->size = 0;
	XtAppWarningMsg(XtDisplayToApplicationContext(display),
			"conversionError","compoundText", "XtToolkitError",
			MSG13, (String *)NULL, (Cardinal *)NULL);
    }
    return(ok);
}


static Boolean
cvtTextToXmString(
        XrmValue *from,
        XrmValue *to )
{
    ct_context	    *ctx;		/* compound text context block */
    Boolean	    ok = True;
    Octet	    c;

    ctx = (ct_context *) XtMalloc(sizeof(ct_context));

/* initialize the context block */
    ctx->octet = (OctetPtr)from->addr;
    ctx->lastoctet = ctx->octet + from->size;
    ctx->flags.dircs = False;
    ctx->flags.gchar = False;
    ctx->flags.ignext = False;
    ctx->flags.gl = False;
    ctx->flags.text = False;
    ctx->dirstacksize = 8;
    ctx->dirstack = (ct_Direction *)
            XtMalloc(ctx->dirstacksize*sizeof(ct_Direction));
/*
 * Define XLIB_HANDLES_DIRECTION if vendor's X library knows how
 * to deal with direction control sequences in CT. Otherwise
 * no such ones will be output if there are no Rtol segments
 * in the XmString. Note that the MIT sample implementation
 * does not deal with direction control sequences...
 */
#ifdef XLIB_HANDLES_DIRECTION
    ctx->dirstack[0] = ct_Dir_StackEmpty;
    ctx->dirsp = 0;
#else
    ctx->dirstack[0] = ct_Dir_StackEmpty;
    ctx->dirstack[1] = ct_Dir_LeftToRight;
    ctx->dirsp = 1;
#endif
    ctx->encoding = NULL;
    ctx->encodinglen = 0;
    ctx->item = NULL;
    ctx->itemlen = 0;
    ctx->version = CTVERSION;
    ctx->gl_charset = CS_ISO8859_1;
    ctx->gl_charset_size = 94;
    ctx->gl_octets_per_char = 1;
    ctx->gr_charset = CS_ISO8859_1;
    ctx->gr_charset_size = 96;
    ctx->gr_octets_per_char = 1;
    ctx->xmstring = NULL;
    ctx->xmsep = NULL;
    ctx->xmtab = NULL;

/*
** check for version/ignore extensions sequence (must be first if present)
**  Format is:	ESC 02/03 V 03/00   ignoring extensions is OK
**		ESC 02/03 V 03/01   ignoring extensions is not OK
**  where V is in the range 02/00 thru 02/15 and represents versions 1 thru 16
*/
    if (    (from->size >= 4)
	&&  (ctx->octet[0] == ESC)
	&&  (ctx->octet[1] == 0x23)
	&&  (_IsInColumn2(ctx->octet[2])
	&&  ((ctx->octet[3] == 0x30) || ctx->octet[3] == 0x31))
       ) {
	ctx->version = ctx->octet[2] - 0x1f;	/* 0x20-0x2f => version 1-16 */
	if (ctx->octet[3] == 0x30)		/* 0x30 == can ignore extensions */
	    ctx->flags.ignext = True;
	ctx->octet += 4;			/* advance ptr to next seq */
    }


    while (ctx->octet < ctx->lastoctet) {
    switch (*ctx->octet) {			/* look at next octet in seq */
	case ESC:
	    /* %%% TEMP
	    ** if we have any text to output, do it
	    ** this section needs to be optimized so that it handles
	    ** paired character sets without outputting a new segment.
	    */
	    if (ctx->flags.text) {
		outputXmString(ctx, False);	/* with no separator */
	    }
	    ctx->flags.text = False;
	    ctx->item = ctx->octet;		/* remember start of this item */
	    ctx->itemlen = 0;

	    ctx->octet++; ctx->itemlen++;	/* advance ptr to next char */

	    /* scan for final char */
	    while (	(ctx->octet != ctx->lastoctet)
		     && (_IsInColumn2(*ctx->octet)) ) {
		ctx->octet++; ctx->itemlen++;	/* advance ptr to next char */
	    }

	    if (ctx->octet == ctx->lastoctet) {	/* if nothing after this, it's an error */
		ok = False;
		break;
	    }

	    c = *ctx->octet;			/* get next char in seq */
	    ctx->octet++; ctx->itemlen++;	/* advance ptr to next char */
	    if (_IsValidESCFinal(c)) {
		/* we have a valid ESC sequence - handle it */
		ok = processESC(ctx, c);
	    } else {
		ok = False;
	    }
	    if (ok) {
	      ctx->encoding = ctx->item;
	      ctx->encodinglen = ctx->itemlen;
	    }
	    break;

	case CSI:
	    /*
	    ** CSI format is:	CSI P I F   where
	    **	    03/00 <= P <= 03/15
	    **	    02/00 <= I <= 02/15
	    **	    04/00 <= F <= 07/14
	    */
	    /* %%% TEMP
	    ** if we have any text to output, do it
	    ** This may need optimization.
	    */
	    if (ctx->flags.text) {
		/* check whether we have a specific direction set */
                if (((ctx->octet[1] == 0x31) && (ctx->octet[2] == 0x5d))||
                    ((ctx->octet[1] == 0x32) && (ctx->octet[2] == 0x5d))||
                    (ctx->octet[1] == 0x5d))
                        outputXmString(ctx, False);    /* without a separator*/
                else
			outputXmString(ctx, True);	/* with a separator */
	    }
	    ctx->flags.text = False;
	    ctx->item = ctx->octet;		/* remember start of this item */
	    ctx->itemlen = 0;

	    ctx->octet++; ctx->itemlen++;	/* advance ptr to next char */

	    /* scan for final char */
	    while (	(ctx->octet != ctx->lastoctet)
		    &&	_IsInColumn3(*ctx->octet)  ) {
		ctx->octet++; ctx->itemlen++;	/* advance ptr to next char */
	    }
	    while (	(ctx->octet != ctx->lastoctet)
		    && _IsInColumn2(*ctx->octet)   ) {
		ctx->octet++; ctx->itemlen++;	/* advance ptr to next char */
	    }

	    /* if nothing after this, it's an error */
	    if (ctx->octet == ctx->lastoctet) {
		ok = False;
		break;
	    }

	    c = *ctx->octet;			/* get next char in seq */
	    ctx->octet++; ctx->itemlen++;	/* advance ptr to next char */
	    if (_IsValidCSIFinal(c)) {
		/* we have a valid CSI sequence - handle it */
		ok = processCSI(ctx, c);
	    } else {
		ok = False;
	    }
	    break;

	case NL:			    /* new line */
	    /* if we have any text to output, do it */
	    if (ctx->flags.text) {
	      outputXmString(ctx, True);	/* with a separator */
	      ctx->flags.text = False;
	    } else {
	      if (ctx->xmsep == NULL) {
		ctx->xmsep = XmStringSeparatorCreate();
	      }
	      ctx->xmstring = XmStringConcatAndFree(ctx->xmstring,
						    XmStringCopy(ctx->xmsep));
	    }
	    ctx->octet++;			/* advance ptr to next char */
	    break;

	case HT:
	    /* if we have any text to output, do it */
	    if (ctx->flags.text) {
	      outputXmString(ctx, False);
	      ctx->flags.text = False;
	    }
	    if (ctx->xmtab == NULL) {
	      ctx->xmtab = XmStringComponentCreate(XmSTRING_COMPONENT_TAB, 0,
						   NULL);
	    }
	    ctx->xmstring = XmStringConcatAndFree(ctx->xmstring,
						  XmStringCopy(ctx->xmtab));
	    ctx->octet++;			/* advance ptr to next char */
	    break;

	default:			    /* just 'normal' text */
	    ctx->item = ctx->octet;		/* remember start of this item */
	    ctx->itemlen = 0;
	    ctx->flags.text = True;
	    while (ctx->octet < ctx->lastoctet) {
		c = *ctx->octet;
		if ((c == ESC) || (c == CSI) || (c == NL) || (c == HT)) {
		    break;
		}
		if (	(_IsInC0Set(c) && (!_IsValidC0(ctx, c)))
		    ||	(_IsInC1Set(c) && (!_IsValidC1(ctx, c))) ) {
		    ok = False;
		    break;
		}
		ctx->flags.gchar = True;	/* We have a character! */

                /*
                 *  We should look at the actual character to
                 *  decide whether it's a gl or gr character.
                 *
                 *  We'll hit the problem if we get a CT that
                 *  isn't generated by Motif.
                 */
                if (isascii((unsigned char)c)) {
		    ctx->octet += ctx->gl_octets_per_char;
		    ctx->itemlen += ctx->gl_octets_per_char;
		} else {
		    ctx->octet += ctx->gr_octets_per_char;
		    ctx->itemlen += ctx->gr_octets_per_char;
		}
		if (ctx->octet > ctx->lastoctet) {
		    ok = False;
		    break;
		}
	    } /* end while */
	    break;
	} /* end switch */
    if (!ok) break;
    } /* end while */

/* if we have any text left to output, do it */
    if (ctx->flags.text) {
	outputXmString(ctx, False);		/* with no separator */
    }

    XtFree((char *) ctx->dirstack);
    if (ctx->xmstring != NULL) {
	to->addr = (char *) ctx->xmstring;
	to->size = sizeof(XmString);
    }
    if (ctx->xmsep != NULL) XmStringFree(ctx->xmsep);
    if (ctx->xmtab != NULL) XmStringFree(ctx->xmtab);
    XtFree((char *) ctx);

    return (ok);
}


static char **
cvtCTsegment(ct_context *ctx,
	     OctetPtr item,
	     unsigned int length)
{
  XTextProperty tmp_prop;
  OctetPtr octets;
  Boolean free_octets = False;
  int count;
  int ret_val;
  char **strings = NULL;

  if (ctx->encoding) {
    if (ctx->encoding + ctx->encodinglen != item) {
      octets =
	(OctetPtr)XtMalloc((ctx->encodinglen + length) * sizeof(Octet));
      memcpy((char *)octets, (char *)ctx->encoding, ctx->encodinglen);
      memcpy((char *)(octets + ctx->encodinglen), (char *)item, length);
      free_octets = True;
    } else {
      octets = ctx->encoding;
    }
  } else {
    octets = ctx->item;
  }

  tmp_prop.value = octets;
  tmp_prop.encoding = XInternAtom(_XmGetDefaultDisplay(),
				  XmSCOMPOUND_TEXT, False);
  tmp_prop.format = 8;
  tmp_prop.nitems = ctx->encodinglen + length;
  ret_val = XmbTextPropertyToTextList(_XmGetDefaultDisplay(),
				      &tmp_prop,
				      &strings,
				      &count);
  if (ret_val > 0) {
    XFreeStringList(strings);
    strings = NULL;
  }
  if (free_octets)
    XtFree((char *)octets);

  return strings;
}


/* outputXmString */
static void
outputXmString(
        ct_context *ctx,
        Boolean separator )
{
  char **strings = NULL;

  /*
   * CR # 8544: XmbTextListToTextProperty converts from locale encoding
   * to MIT registered encodings. We have to convert back - UNLESS the
   * CT was not created with XmbTextListToTextProperty. However, there
   * is no way to tell how we got the CT, but in most cases, the
   * tag will be FONTLIST_DEFAULT_TAG, and then we want to do this. So
   * we try this, and only use the encoding tags if we could not convert.
   */
  strings = cvtCTsegment(ctx, ctx->item, ctx->itemlen);
  if (strings) {
    ctx->xmstring = concatStringToXmString
    (ctx->xmstring,
     strings[0],
     strlen(strings[0]),
     XmFONTLIST_DEFAULT_TAG,
     (XmStringDirection) ((_CurDir(ctx) == ct_Dir_LeftToRight) ?
			  XmSTRING_DIRECTION_L_TO_R :
			  ((_CurDir(ctx) == ct_Dir_RightToLeft) ?
			   XmSTRING_DIRECTION_R_TO_L :
			   XmSTRING_DIRECTION_UNSET)),
     separator );

    XFreeStringList(strings);
    return;
  }

  /* If we couldn't convert to locale encoding... */
  /* This is not really right. We can probably never draw this string and
     get something that looks right out of this */

  /*
   *  If the GL charset is ISO8859-1, and the GR charset is any ISO8859
   *  charset, then they're a pair, so we can create a single segment using
   *  just the GR charset.
   *
   *  If GL and GR are multibyte charsets and they match (both GB2312 or both
   *  KSC5601) except for JISX0208, then we can create a single segment using
   *  just the GR charset. If GL and GR are multibyte charsets and they DON'T
   *  match, or if GL or GR is multibyte and the other is singlebyte, then
   *  there's no way to tell which characters belong to GL and which to GR,
   *  so treat it like a non-Latin1 in GL - 7 bit characters go to GL, 8 bit
   *  characters to to GR.  *** THIS APPEARS TO BE A HOLE IN THE COMPOUND
   *  TEXT SPEC ***.
   *
   *  Otherwise the charsets are not a pair and we will switch between GL
   *  and GR segments each time the high bit changes.
   */
  if (((ctx->gl_charset == CS_ISO8859_1)
       &&
       ((ctx->gr_charset == CS_ISO8859_1) ||
	(ctx->gr_charset == CS_ISO8859_2) ||
	(ctx->gr_charset == CS_ISO8859_3) ||
	(ctx->gr_charset == CS_ISO8859_4) ||
	(ctx->gr_charset == CS_ISO8859_5) ||
	(ctx->gr_charset == CS_ISO8859_6) ||
	(ctx->gr_charset == CS_ISO8859_7) ||
	(ctx->gr_charset == CS_ISO8859_8) ||
	(ctx->gr_charset == CS_ISO8859_9)))
      ||
      ((ctx->gl_charset == CS_GB2312_0) &&
       (ctx->gr_charset == CS_GB2312_1))
      ||
      ((ctx->gl_charset == CS_KSC5601_0) &&
       (ctx->gr_charset == CS_KSC5601_1)))
    {
      /* OK to do single segment output but always use GR charset */
      ctx->xmstring = concatStringToXmString
	(ctx->xmstring,
	 (char *) ctx->item,
	 ctx->itemlen,
	 (char *) ctx->gr_charset,
	 (XmStringDirection) ((_CurDir(ctx) == ct_Dir_LeftToRight) ?
			      XmSTRING_DIRECTION_L_TO_R :
			      ((_CurDir(ctx) == ct_Dir_RightToLeft) ?
			       XmSTRING_DIRECTION_R_TO_L :
			       XmSTRING_DIRECTION_UNSET)),
	 separator );

    }
  else
    {
      /* have to create a new segment everytime the highbit changes */
      unsigned int	j = 0;
      unsigned int	start = 0;
      Octet		c;
      Boolean		curseg_is_gl;

      curseg_is_gl = isascii((unsigned char)ctx->item[0]);

      while (j < ctx->itemlen)
	{
	  c = ctx->item[j];
	  if (isascii((unsigned char)c))
	    {
	      if (!curseg_is_gl)
		{
		  /* output gr string */
		  assert(j > start);
		  ctx->xmstring = concatStringToXmString
		    (ctx->xmstring,
		     (char *)ctx->item + start,
		     j - start,
		     (char *) ctx->gr_charset,
		     (XmStringDirection)
		     ((_CurDir(ctx) == ct_Dir_LeftToRight) ?
		      XmSTRING_DIRECTION_L_TO_R :
		      ((_CurDir(ctx) == ct_Dir_RightToLeft) ?
		       XmSTRING_DIRECTION_R_TO_L :
		       XmSTRING_DIRECTION_UNSET)),
		     False );
		  start = j;
		  curseg_is_gl = True;	/* start gl segment */
		};
	      j++;
	    }
	  else
	    {
	      if (curseg_is_gl)
		{
		  /* output gl string */
		  assert(j > start);
		  ctx->xmstring = concatStringToXmString
		    (ctx->xmstring,
		     (char *)ctx->item + start,
		     j - start,
		     (char *) ctx->gl_charset,
		     (XmStringDirection)
		     ((_CurDir(ctx) == ct_Dir_LeftToRight) ?
		      XmSTRING_DIRECTION_L_TO_R :
		      ((_CurDir(ctx) == ct_Dir_RightToLeft) ?
		       XmSTRING_DIRECTION_R_TO_L :
		       XmSTRING_DIRECTION_UNSET)),
		     False );
		  start = j;
		    curseg_is_gl = False;	/* start gr segment */
		};
	      j++;
	    }; /* end if */
	}; /* end while */

      /* output last segment */
      ctx->xmstring = concatStringToXmString
	(ctx->xmstring,
	(char *)ctx->item + start,
	 ctx->itemlen - start,
	 (char *) ((curseg_is_gl) ?
		   ctx->gl_charset :
		   ctx->gr_charset ),
	 (XmStringDirection) ((_CurDir(ctx) == ct_Dir_LeftToRight) ?
			      XmSTRING_DIRECTION_L_TO_R :
			      ((_CurDir(ctx) == ct_Dir_RightToLeft) ?
			       XmSTRING_DIRECTION_R_TO_L :
			       XmSTRING_DIRECTION_UNSET)),
	 False );


      if (separator)
	{
	  if (ctx->xmsep == NULL)
	    {
	      ctx->xmsep = XmStringSeparatorCreate();
	    };
	  ctx->xmstring = XmStringConcatAndFree(ctx->xmstring,
						XmStringCopy(ctx->xmsep));
	};
    }; /* end if paired */
}

static XmString
concatStringToXmString(
        XmString compoundstring,
        char *textstring,
	int textlen,
        char *charset,
        XmStringDirection direction,
        Boolean separator )
{
    XmString	tempxm1;

    (void)textlen;
    tempxm1 =
      XmStringConcatAndFree(XmStringDirectionCreate(direction),
			    XmStringCreate(textstring, charset));

    if (separator)
      tempxm1 = XmStringConcatAndFree(tempxm1,
				      XmStringSeparatorCreate());

    compoundstring = XmStringConcatAndFree(compoundstring, tempxm1);
    return (compoundstring);
}


/* processESC - handle valid ESC sequences */
static Boolean
processESC(
        ct_context *ctx,
        Octet final )
{
    Boolean	    ok;

    switch (ctx->item[1]) {
    case 0x24:			/* 02/04 - invoke 94(n) charset into GL or GR */
	ok = process94n(ctx, final);
	break;
    case 0x25:			/* 02/05 - extended segments */
	/* if we have any text to output, do it */
	if (ctx->flags.text) {
	    outputXmString(ctx, False);	/* with no separator */
	    ctx->flags.text = False;
	}
	ok = processExtendedSegments(ctx, final);
	break;
    case 0x28:			/* 02/08 - invoke 94 charset into GL */
	ok = process94GL(ctx, final);
	break;
    case 0x29:			/* 02/09 - invoke 94 charset into GR */
	ok = process94GR(ctx, final);
	break;
    case 0x2d:			/* 02/13 - invoke 96 charset into GR */
	ok =  process96GR(ctx, final);
	break;
    default:
	ok = False;
	break;
    }
    return(ok);
}


/*
**  processCSI - handle valid CSI sequences
**	CSI sequences
**	09/11 03/01 05/13   begin left-to-right text
**	09/11 03/02 05/13   begin right-to-left text
**	09/11 05/13	    end of string
**	09/11 P I F	    reserved for use in future extensions
*/
static Boolean
processCSI(
        ct_context *ctx,
        Octet final )
{
    Boolean	    ok = True;

    switch (final) {
    case 0x5d:				/* end of direction sequence */
	switch (ctx->item[1]) {
	case 0x31:			/* start left to right */
	    if (ctx->flags.gchar && ctx->dirsp == 0) {
		ok = False;
	    } else {
		_PushDir(ctx, ct_Dir_LeftToRight);
	    }
	    break;
	case 0x32:			/* start right to left */
	    if (ctx->flags.gchar && ctx->dirsp == 0) {
		ok = False;
	    } else {
		_PushDir(ctx, ct_Dir_RightToLeft);
	    }
	    break;
	case 0x5d:			/* Just CSI EOS - revert */
	    if (ctx->dirsp > 0) {
		_PopDir(ctx);

	    } else {
		ok = False;
	    }
	    break;
	default:			/* anything else is an error */
	    ok = False;
	}
	break;

    default:				/* reserved for future extensions */
	ok = False;
	break;
    }
    return(ok);
}



static Boolean
processExtendedSegments(
        ct_context *ctx,
        Octet final )
{
    OctetPtr	    esptr;			/* ptr into ext seg */
    unsigned int    seglen;			/* length of ext seg */
    unsigned int    len;			/* length */
    String	    charset_copy;		/* ptr to NULL-terminated copy of ext seg charset */
    OctetPtr	    text_copy;			/* ptr to NULL-terminated copy of ext seg text */
    XmString	    tempxm1;
    Boolean	    ok = True;

    /* Extended segments
    **  01/11 02/05 02/15 03/00 M L	    variable # of octets/char
    **  01/11 02/05 02/15 03/01 M L	    1 octet/char
    **  01/11 02/05 02/15 03/02 M L	    2 octets/char
    **  01/11 02/05 02/15 03/03 M L	    3 octets/char
    **  01/11 02/05 02/15 03/04 M L	    4 octets/char
    */
    if (	(ctx->itemlen == 4)
	&&	(ctx->item[2] == 0x2f)
	&&	(_IsInColumn3(final))
       ) {
	if (    ((ctx->lastoctet - ctx->octet) < 2)
	    ||  (ctx->octet[0] < 0x80)
	    ||  (ctx->octet[1] < 0x80)
	   ) {
	    return(False);
	}

	/*
	** The most significant bit of M and L are always set to 1
	** The number is computed as ((M-128)*128)+(L-128)
	*/
	seglen = *ctx->octet - 0x80;
	ctx->octet++; ctx->itemlen++;		/* advance pointer */
	seglen = (seglen << 7) + (*ctx->octet - 0x80);
	ctx->octet++; ctx->itemlen++;		/* advance pointer */
	if ((ctx->lastoctet - ctx->octet) < seglen) {
	    return(False);
	}
	esptr = ctx->octet;			/* point to charset */
	ctx->itemlen += seglen;			/* advance pointer over segment */
	ctx->octet += seglen;

	switch (final) {
	case 0x30:				/* variable # of octets per char */
	case 0x31:				/* 1 octet per char */
	case 0x32:				/* 2 octets per char */
	    /* scan for STX separator between charset and text */
	    len = 0;
	    while (esptr[len] != STX)
		len++;
	    if (len > ctx->itemlen) {		/* if we ran off the end, error */
		ok = False;
		break;
	    }
	    charset_copy = XtMalloc(len + 1);
	    strncpy(charset_copy, (char *) esptr, len);
	    charset_copy[len] = EOS;
	    esptr += len + 1;			/* point to text part */
	    len = seglen - len - 1;		/* calc length of text part */
	    text_copy = (unsigned char *) XtMalloc(len + 1);
	    memcpy( text_copy, esptr, len);
	    text_copy[len] = EOS;
	    tempxm1 = XmStringConcatAndFree(
		         XmStringDirectionCreate((_CurDir(ctx) ==
						  ct_Dir_LeftToRight ?
						  XmSTRING_DIRECTION_L_TO_R :
						  (_CurDir(ctx) ==
						   ct_Dir_RightToLeft ?
						   XmSTRING_DIRECTION_R_TO_L :
						   XmSTRING_DIRECTION_UNSET))),
			 XmStringCreate((char *)text_copy, charset_copy));
	    ctx->xmstring = XmStringConcatAndFree(ctx->xmstring, tempxm1);
	    XtFree((char *) text_copy);
	    XtFree(charset_copy);
	    ok = True;
	    break;

	case 0x33:				/* 3 octets per char */
	case 0x34:				/* 4 octets per char */
	    /* not supported */
	    ok = False;
	    break;

	default:
	    /* reserved for future use */
	    ok = False;
	    break;
	} /* end switch */
    } /* end if */

    return(ok);
}


static Boolean
process94n(
        ct_context *ctx,
        Octet final )
{
    if (ctx->itemlen > 3) {
	switch (ctx->item[2]) {
	case 0x28:				/* into GL */
	    switch (final) {
	    case 0x41:				/* 04/01 - China (PRC) Hanzi */
		_SetGL(ctx, CS_GB2312_0, 94, 2);
		break;
	    case 0x42:				/* 04/02 - Japanese GCS, level 2 */
		_SetGL(ctx, CS_JISX0208_0, 94, 2);
		break;
	    case 0x43:				/* 04/03 - Korean GCS */
		_SetGL(ctx, CS_KSC5601_0, 94, 2);
		break;
	    default:
		/* other character sets are not supported */
		return False;
	    } /* end switch (final) */
	    break;

	case 0x29:				/* into GR */
	    switch (final) {
	    case 0x41:				/* 04/01 - China (PRC) Hanzi */
		_SetGR(ctx, CS_GB2312_1, 94, 2);
		break;
	    case 0x42:				/* 04/02 - Japanese GCS, level 2 */
		_SetGR(ctx, CS_JISX0208_1, 94, 2);
		break;
	    case 0x43:				/* 04/03 - Korean GCS */
		_SetGR(ctx, CS_KSC5601_1, 94, 2);
		break;
	    default:
		/* other character sets are not supported */
		return False;
	    } /* end switch (final) */
	    break;

	default:
	    /* error */
	    return False;
	} /* end switch item[2] */
    }
    else {
	/* error */
	return False;
    } /* end if */
    return True;
}



static Boolean
process94GL(
        ct_context *ctx,
        Octet final )
{
    switch (final) {
    case 0x42:				/* 04/02 - Left half, ISO8859* (ASCII) */
	_SetGL(ctx, CS_ISO8859_1,  94, 1);
	break;
    case 0x4a:				/* 04/10 - Left half, Katakana */
	_SetGL(ctx, CS_JISX0201, 94, 1);
	break;
    default:
	return False;
    }

    return(True);
}


static Boolean
process94GR(
        ct_context *ctx,
        Octet final )
{
    switch (final) {
    case 0x49:				/* 04/09 - Right half, Katakana */
	_SetGR(ctx, CS_JISX0201, 94, 1);
	break;
    default:
	return False;
    }

    return(True);
}



static Boolean
process96GR(
        ct_context *ctx,
        Octet final )
{
    switch (final) {
    case 0x40:				/* 04/00 - Right half, IR-111 */
	_SetGR(ctx, CS_ISO_IR_111, 96, 1);
	break;
    case 0x41:				/* 04/01 - Right half, Latin 1 */
	_SetGR(ctx, CS_ISO8859_1, 96, 1);
	break;
    case 0x42:				/* 04/02 - Right half, Latin 2 */
	_SetGR(ctx, CS_ISO8859_2, 96, 1);
	break;
    case 0x43:				/* 04/03 - Right half, Latin 3 */
	_SetGR(ctx, CS_ISO8859_3, 96, 1);
	break;
    case 0x44:				/* 04/04 - Right half, Latin 4 */
	_SetGR(ctx, CS_ISO8859_4, 96, 1);
	break;
    case 0x46:				/* 04/06 - Right half, Latin/Greek */
	_SetGR(ctx, CS_ISO8859_7, 96, 1);
	break;
    case 0x47:				/* 04/07 - Right half, Latin/Arabic */
	_SetGR(ctx, CS_ISO8859_6, 96, 1);
	break;
    case 0x48:				/* 04/08 - Right half, Latin/Hebrew */
	_SetGR(ctx, CS_ISO8859_8, 96, 1);
	break;
    case 0x4c:				/* 04/12 - Right half, Latin/Cyrillic */
	_SetGR(ctx, CS_ISO8859_5, 96, 1);
	break;
    case 0x4d:				/* 04/13 - Right half, Latin 5 */
	_SetGR(ctx, CS_ISO8859_9, 96, 1);
	break;
    default:
	return False;
    }

    return(True);
}


/************************************************************************
 *
 *  XmCvtXmStringToCT
 *	Convert an XmString to a compound text string directly.
 *	This is the public version of the resource converter and only
 *	requires the XmString as an argument.
 *
 ************************************************************************/
char *
XmCvtXmStringToCT(
        XmString string )
{
  Boolean	ok;
  /* Dummy up some XrmValues to pass to cvtXmStringToText. */
  XrmValue	from_val;
  XrmValue	to_val;

  if (string == NULL)
    return ( (char *) NULL );

  from_val.addr = (char *) string;

  ok = cvtXmStringToText(&from_val, &to_val);

  if (!ok)
  {
    XtWarningMsg( "conversionError","compoundText", "XtToolkitError",
		 MSG8, NULL, NULL) ;
    return( (char *) NULL ) ;
    }
  return( (char *) to_val.addr) ;
  }

#if XM_UTF8
/************************************************************************
 *
 *  XmCvtXmStringToUTF8String
 *	Convert an XmString to a compound utf8 string directly.
 *	This is the public version of the resource converter and only
 *	requires the XmString as an argument.
 *
 ************************************************************************/
char *
XmCvtXmStringToUTF8String(
        XmString string )
{
  Boolean	ok;
  /* Dummy up some XrmValues to pass to cvtXmStringToText. */
  XrmValue	from_val;
  XrmValue	to_val;

  if (string == NULL)
    return ( (char *) NULL );

  from_val.addr = (char *) string;

  ok = cvtXmStringToUTF8String(&from_val, &to_val);

  if (!ok)
  {
    XtWarningMsg( "conversionError","compoundText", "XtToolkitError",
		 MSG8, NULL, NULL) ;
    return( (char *) NULL ) ;
    }
  return( (char *) to_val.addr) ;
  }
#endif

/***************************************************************************
 *									   *
 * _XmCvtXmStringToCT - public wrapper for the widgets to use.	  	   *
 *   This returns the length info as well - critical for the list widget   *
 * 									   *
 ***************************************************************************/
Boolean
_XmCvtXmStringToCT(
        XrmValue *from,
        XrmValue *to )
{
    return (cvtXmStringToText( from, to ));
}

#if XM_UTF8
/***************************************************************************
 *									   *
 * _XmCvtXmStringToUTF8String - public wrapper for the widgets to use.	   *
 *   This returns the length info as well - critical for the list widget   *
 * 									   *
 ***************************************************************************/
Boolean
_XmCvtXmStringToUTF8String(
        XrmValue *from,
        XrmValue *to )
{
    return (cvtXmStringToUTF8String( from, to ));
}
#endif

/************************************************************************
 *
 *  XmCvtXmStringToText
 *	Convert an XmString to an ASCII string.
 *
 ************************************************************************/
Boolean
XmCvtXmStringToText(
        Display *display,
        XrmValuePtr args,	/* unused */
        Cardinal *num_args,	/* unused */
        XrmValue *from_val,
        XrmValue *to_val,
        XtPointer *converter_data ) /* unused */
{
    Boolean		ok;

    if (from_val->addr == NULL)
	return( FALSE) ;

    ok = cvtXmStringToText(from_val, to_val);

    if (!ok)
    {
	XtAppWarningMsg(XtDisplayToApplicationContext(display),
			"conversionError","compoundText", "XtToolkitError",
			MSG14, (String *)NULL, (Cardinal *)NULL);
    }
    return(ok);
}

#if XM_UTF8
/************************************************************************
 *
 *  cvtXmStringToUTF8String
 *    Convert an XmString to a compound text string.  This is the
 *    underlying conversion routine for XmCvtXmStringToUTF8String,
 *    _XmCvtXmStringToUTF8String.
 *
 ************************************************************************/
static Boolean
cvtXmStringToUTF8String(
        XrmValue *from,
        XrmValue *to )
{
  Boolean		ok;
  OctetPtr		outc = NULL;
  unsigned int		outlen = 0;
  _XmStringContextRec	stack_context;
  XmStringTag	ct_encoding = NULL, cset_save = NULL;
  ct_Direction		prev_direction = ct_Dir_LeftToRight;
  ct_Charset		prev_charset = cs_Latin1;
  XmStringComponentType	comp;
  unsigned int		len;
  XtPointer		val = NULL;
  Octet			tmp_buf[256];
  OctetPtr		tmp;

  /* Initialize the return parameters. */
  to->addr = (XPointer) NULL;
  to->size = 0;

  if (!from->addr)
    return(False);

  memset(&stack_context, 0, sizeof stack_context);
  _XmStringContextReInit(&stack_context, (XmString)from->addr);

/* BEGIN OSF Fix CR 7403 */
  while ((comp = XmeStringGetComponent(&stack_context, True, False,
				       &len, &val)) !=
	 XmSTRING_COMPONENT_END)
    {
      switch (comp)
	{
	case XmSTRING_COMPONENT_LOCALE_TEXT:
	  cset_save = XmFONTLIST_DEFAULT_TAG;
	  /* Fall through */
	case XmSTRING_COMPONENT_TEXT:
	  if (cset_save != NULL) {
	    /* Check Registry */
	    if (ct_encoding)
	      XtFree((char *)ct_encoding);
	    ct_encoding = XmMapSegmentEncoding(cset_save);
	  }

	  /* We must duplicate val because the routines we want to */
	  /*	call don't take a length parameter. */
	  tmp = (OctetPtr) XmStackAlloc(len + 1, tmp_buf);
	  memcpy((char*)tmp, val, len);
	  tmp[len] = EOS;

	  if (ct_encoding != NULL) {
	    /* We have a mapping. */
	    ok = processCharsetAndTextUtf8(ct_encoding, tmp, FALSE,
				       &outc, &outlen, &prev_charset);
	  } else {
	    /* No mapping.  Vendor dependent. */
	    ok = processCharsetAndTextUtf8(cset_save, tmp, FALSE,
						    &outc, &outlen,
						    &prev_charset);
	  }

	  /* Free our local copy of val. */
	  XmStackFree((char*) tmp, tmp_buf);

	  if (!ok)
	    {
	      _XmStringContextFree(&stack_context);
	      return(False);
	    }
	  break;

	case XmSTRING_COMPONENT_TAG:
	  cset_save = (XmStringTag)val;
	  break;

	case XmSTRING_COMPONENT_DIRECTION:
	  /* Output the direction, if changed */
	  if (*(XmStringDirection *)val == XmSTRING_DIRECTION_L_TO_R) {
	    if (prev_direction != ct_Dir_LeftToRight) {
	      outc = ctextConcat(outc, outlen,
				 (unsigned char *) UTF8_L_TO_R,
				 (unsigned int)UTF8_L_TO_R_LEN);
	      outlen += UTF8_L_TO_R_LEN;
	      prev_direction = ct_Dir_LeftToRight;
	    }
	  } else {
	    if (prev_direction != ct_Dir_RightToLeft) {
	      outc = ctextConcat(outc, outlen,
				 (unsigned char *) UTF8_R_TO_L,
				 (unsigned int)UTF8_R_TO_L_LEN);
	      outlen += UTF8_R_TO_L_LEN;
	      prev_direction = ct_Dir_RightToLeft;
	    }
	  };
	  break;

	case XmSTRING_COMPONENT_SEPARATOR:
	  if (ct_encoding != NULL) {		  /* We have a mapping. */
	    ok = processCharsetAndTextUtf8(ct_encoding, NULL, TRUE,
				       &outc, &outlen, &prev_charset);
	  }
	  else
	    {
	      /* No mapping.  Vendor dependent. */
	      ok = processCharsetAndTextUtf8(cset_save, NULL, TRUE,
                                    &outc, &outlen, &prev_charset);
	    }

	  if (!ok)
	    {
	      _XmStringContextFree(&stack_context);
	      return(False);
	    }
	  break;

	case XmSTRING_COMPONENT_TAB:
	  outc = ctextConcat(outc, outlen,
			     (unsigned char *)UTF8_TABSTRING,
			     (unsigned int)UTF8_TABSTRING_LEN);
	  outlen++;
	  break;

	default:
	  /* Just ignore it. */
	  break;
	}
  } /* end while */

  if (ct_encoding)
    XtFree((char *)ct_encoding);

/* END OSF Fix CR 7403 */
  if (outc != NULL) {
    to->addr = (char *) outc;
    to->size = outlen;
  }

  _XmStringContextFree(&stack_context);

  return(True);
}
#endif

/************************************************************************
 *
 *  cvtXmStringToText
 *    Convert an XmString to a compound text string.  This is the
 *    underlying conversion routine for XmCvtXmStringToCT,
 *    _XmCvtXmStringToCT, and XmCvtXmStringToText.
 *
 ************************************************************************/
static Boolean
cvtXmStringToText(
        XrmValue *from,
        XrmValue *to )
{
  Boolean		ok;
  OctetPtr		outc = NULL;
  unsigned int		outlen = 0;
  _XmStringContextRec	stack_context;
  XmStringTag	ct_encoding = NULL, cset_save = NULL;
/*
 * Define XLIB_HANDLES_DIRECTION if vendor's X library knows how
 * to deal with direction control sequences in CT. Otherwise
 * no such beasts will be output if there are no Rtol segments
 * in the XmString. Note that the MIT sample implementation
 * does not deal with direction control sequences...
 */
#ifdef XLIB_HANDLES_DIRECTION
  ct_Direction		prev_direction = ct_Dir_Undefined;
#else
  ct_Direction		prev_direction = ct_Dir_LeftToRight;
#endif
  ct_Charset		prev_charset = cs_Latin1;
  XmStringComponentType	comp;
  unsigned int		len;
  XtPointer		val = NULL;
  Octet			tmp_buf[256];
  OctetPtr		tmp;

  /* Initialize the return parameters. */
  to->addr = (XPointer) NULL;
  to->size = 0;

  if (!from->addr)
    return(False);

  memset(&stack_context, 0, sizeof stack_context);
  _XmStringContextReInit(&stack_context, (XmString)from->addr);

/* BEGIN OSF Fix CR 7403 */
  while ((comp = XmeStringGetComponent(&stack_context, True, False,
				       &len, &val)) !=
	 XmSTRING_COMPONENT_END)
    {
      switch (comp)
	{
	case XmSTRING_COMPONENT_LOCALE_TEXT:
	  cset_save = XmFONTLIST_DEFAULT_TAG;
	  /* Fall through */
	case XmSTRING_COMPONENT_TEXT:
	  if (cset_save != NULL) {
	    /* Check Registry */
	    if (ct_encoding)
	      XtFree((char *)ct_encoding);
	    ct_encoding = XmMapSegmentEncoding(cset_save);
	  }

	  /* We must duplicate val because the routines we want to */
	  /*	call don't take a length parameter. */
	  tmp = (OctetPtr) XmStackAlloc(len + 1, tmp_buf);
	  memcpy((char*)tmp, val, len);
	  tmp[len] = EOS;

	  if (ct_encoding != NULL) {
	    /* We have a mapping. */
	    ok = processCharsetAndText(ct_encoding, tmp, FALSE,
				       &outc, &outlen, &prev_charset);
	  } else {
	    /* No mapping.  Vendor dependent. */
	    ok = _XmOSProcessUnmappedCharsetAndText(cset_save, tmp, FALSE,
						    &outc, &outlen,
						    &prev_charset);
	  }

	  /* Free our local copy of val. */
	  XmStackFree((char*) tmp, tmp_buf);

	  if (!ok)
	    {
	      _XmStringContextFree(&stack_context);
	      return(False);
	    }
	  break;

	case XmSTRING_COMPONENT_TAG:
	  cset_save = (XmStringTag)val;
	  break;

	case XmSTRING_COMPONENT_DIRECTION:
	  /* Output the direction, if changed */
	  if (*(XmStringDirection *)val == XmSTRING_DIRECTION_L_TO_R) {
	    if (prev_direction != ct_Dir_LeftToRight) {
	      outc = ctextConcat(outc, outlen,
				 (unsigned char *) CTEXT_L_TO_R,
				 (unsigned int)CTEXT_L_TO_R_LEN);
	      outlen += CTEXT_L_TO_R_LEN;
	      prev_direction = ct_Dir_LeftToRight;
	    }
	  } else {
	    if (prev_direction != ct_Dir_RightToLeft) {
	      outc = ctextConcat(outc, outlen,
				 (unsigned char *) CTEXT_R_TO_L,
				 (unsigned int)CTEXT_R_TO_L_LEN);
	      outlen += CTEXT_R_TO_L_LEN;
	      prev_direction = ct_Dir_RightToLeft;
	    }
	  };
	  break;

	case XmSTRING_COMPONENT_SEPARATOR:
	  if (ct_encoding != NULL) {		  /* We have a mapping. */
	    ok = processCharsetAndText(ct_encoding, NULL, TRUE,
				       &outc, &outlen, &prev_charset);
	  }
	  else
	    {
	      /* No mapping.  Vendor dependent. */
	      ok = _XmOSProcessUnmappedCharsetAndText(cset_save, NULL, TRUE,
						      &outc, &outlen, &prev_charset);
	    }

	  if (!ok)
	    {
	      _XmStringContextFree(&stack_context);
	      return(False);
	    }
	  break;

	case XmSTRING_COMPONENT_TAB:
	  outc = ctextConcat(outc, outlen,
			     (unsigned char *)TABSTRING,
			     (unsigned int)TABSTRING_LEN);
	  outlen++;
	  break;

	default:
	  /* Just ignore it. */
	  break;
	}
  } /* end while */

  if (ct_encoding)
    XtFree((char *)ct_encoding);

/* END OSF Fix CR 7403 */
  if (outc != NULL) {
    to->addr = (char *) outc;
    to->size = outlen;
  }

  _XmStringContextFree(&stack_context);

  return(True);
}

#if XM_UTF8
static Boolean
processCharsetAndTextUtf8(XmStringTag tag,
		      OctetPtr		ctext,
		      Boolean		separator,
		      OctetPtr		*outc,
		      unsigned int	*outlen,
		      ct_Charset	*prev)
{
  size_t ctlen = 0, len;

  if (strcmp(tag, XmFONTLIST_DEFAULT_TAG) == 0) {
      if (_XmStringIsCurrentCharset("UTF-8")) {
    	  if (ctext)
    		  ctlen = strlen((char *)ctext);

          if (ctlen > 0) {
              *outc = ctextConcat(*outc, *outlen, ctext, ctlen);
              *outlen += ctlen;
          };
      } else {
          XTextProperty	prop_rtn;
          int		ret_val;
          String	msg;

         /* Call XmbTextListToTextProperty */
          ret_val =
              XmbTextListToTextProperty(_XmGetDefaultDisplay(),
                      (char **)&ctext,
                      1, XUTF8StringStyle, &prop_rtn);

          if (ret_val) {
              switch (ret_val) {
                  case XNoMemory:
                      msg = MSG9;
                      break;
                  case XLocaleNotSupported:
                      msg = MSG10;
                      break;
                  default:
                      msg = MSG11;
                      break;
              }

              XtWarningMsg("conversionError", "textProperty",
                      "XtToolkitError", msg, NULL, 0);

              return(False);
          }

          ctlen = strlen((char *)prop_rtn.value);

          /* Now copy in the text */
          if (ctlen > 0) {
              *outc = ctextConcat(*outc, *outlen, prop_rtn.value, ctlen);
              *outlen += ctlen;
          };

          XFree(prop_rtn.value);
      }

      /* Finally, add the separator if any */
      if (separator) {
	*outc = ctextConcat(*outc, *outlen,
			    (unsigned char *)UTF8_NEWLINESTRING,
			    (unsigned int)UTF8_NEWLINESTRING_LEN);
	(*outlen)++;
      };

      *prev = cs_none;

      return(True);
  }

  if (ctext)
    ctlen = strlen((char *)ctext);

  /* Now copy in the text */
  if (ctlen > 0) {
    ctext = (OctetPtr)_Xmcsconv(tag, "UTF-8", (char *)ctext, ctlen, &ctlen);
    if (!ctext)
        return False;
    *outc = ctextConcat(*outc, *outlen, (const_OctetPtr)ctext, ctlen);
    *outlen += ctlen;
    XtFree((XtPointer)ctext);
  };

  /* Finally, add the separator if any */
  if (separator) {
    *outc = ctextConcat(*outc, *outlen,
			(unsigned char *)UTF8_NEWLINESTRING,
			(unsigned int)UTF8_NEWLINESTRING_LEN);
    (*outlen)++;
  }
  return(True);
}
#endif


static Boolean
processCharsetAndText(XmStringTag tag,
		      OctetPtr		ctext,
		      Boolean		separator,
		      OctetPtr		*outc,
		      unsigned int	*outlen,
		      ct_Charset	*prev)
{
  unsigned int		ctlen = 0, len;

  if (strcmp(tag, CS_UTF_8) == 0)
    tag = XmFONTLIST_DEFAULT_TAG;

  if (strcmp(tag, XmFONTLIST_DEFAULT_TAG) == 0)
    {
      XTextProperty	prop_rtn;
      int		ret_val;
      String		msg;

      /* Call XmbTextListToTextProperty */
      ret_val =
	XmbTextListToTextProperty(_XmGetDefaultDisplay(), (char **)&ctext,
				  1, XCompoundTextStyle, &prop_rtn);

      if (ret_val)
	{
	  switch (ret_val)
	    {
	    case XNoMemory:
	      msg = MSG9;
	      break;
	    case XLocaleNotSupported:
	      msg = MSG10;
	      break;
	    default:
	      msg = MSG11;
	      break;
	    }

	  XtWarningMsg("conversionError", "textProperty", "XtToolkitError",
		       msg, NULL, 0);

	  return(False);
	}

      /* Now copy in the text */
      if (prop_rtn.value) {
        ctlen = strlen((char *)prop_rtn.value);
	*outc = ctextConcat(*outc, *outlen, prop_rtn.value, ctlen);
	*outlen += ctlen;
      };

      XFree(prop_rtn.value);

      /* Finally, add the separator if any */
      if (separator) {
	*outc = ctextConcat(*outc, *outlen,
			    (unsigned char *)NEWLINESTRING,
			    (unsigned int)NEWLINESTRING_LEN);
	(*outlen)++;
      };

      *prev = cs_none;

      return(True);
    }

  if (ctext)
    ctlen = strlen((char *)ctext);

  /* Next output the charset */
  if (strcmp(tag, CS_ISO8859_1) == 0) {
    if (*prev != cs_Latin1) {
      *outc = ctextConcat(*outc, *outlen,
			  (unsigned char *)CTEXT_SET_ISO8859_1,
			  (unsigned int)CTEXT_SET_ISO8859_1_LEN);
      *outlen += CTEXT_SET_ISO8859_1_LEN;
      *prev = cs_Latin1;
    };
  }
  else if (strcmp(tag, CS_ISO8859_2) == 0) {
    if (*prev != cs_Latin2) {
      *outc = ctextConcat(*outc, *outlen,
			  (unsigned char *)CTEXT_SET_ISO8859_2,
			  (unsigned int)CTEXT_SET_ISO8859_2_LEN);
      *outlen += CTEXT_SET_ISO8859_2_LEN;
      *prev = cs_Latin2;
    };
  }
  else if (strcmp(tag, CS_ISO8859_3) == 0) {
    if (*prev != cs_Latin3) {
      *outc = ctextConcat(*outc, *outlen,
			  (unsigned char *)CTEXT_SET_ISO8859_3,
			  (unsigned int)CTEXT_SET_ISO8859_3_LEN);
      *outlen += CTEXT_SET_ISO8859_3_LEN;
      *prev = cs_Latin3;
    };
  }
  else if (strcmp(tag, CS_ISO8859_4) == 0) {
    if (*prev != cs_Latin4) {
      *outc = ctextConcat(*outc, *outlen,
			  (unsigned char *)CTEXT_SET_ISO8859_4,
			  (unsigned int)CTEXT_SET_ISO8859_4_LEN);
      *outlen += CTEXT_SET_ISO8859_4_LEN;
      *prev = cs_Latin4;
    };
  }
  else if (strcmp(tag, CS_ISO8859_5) == 0) {
    if (*prev != cs_LatinCyrillic) {
      *outc = ctextConcat(*outc, *outlen,
			  (unsigned char *)CTEXT_SET_ISO8859_5,
			  (unsigned int)CTEXT_SET_ISO8859_5_LEN);
      *outlen += CTEXT_SET_ISO8859_5_LEN;
      *prev = cs_LatinCyrillic;
    };
  }
  else if (strcmp(tag, CS_ISO8859_6) == 0) {
    if (*prev != cs_LatinArabic) {
      *outc = ctextConcat(*outc, *outlen,
			  (unsigned char *)CTEXT_SET_ISO8859_6,
			  (unsigned int)CTEXT_SET_ISO8859_6_LEN);
      *outlen += CTEXT_SET_ISO8859_6_LEN;
      *prev = cs_LatinArabic;
    };
  }
  else if (strcmp(tag, CS_ISO8859_7) == 0) {
    if (*prev != cs_LatinGreek) {
      *outc = ctextConcat(*outc, *outlen,
			  (unsigned char *)CTEXT_SET_ISO8859_7,
			  (unsigned int)CTEXT_SET_ISO8859_7_LEN);
      *outlen += CTEXT_SET_ISO8859_7_LEN;
      *prev = cs_LatinGreek;
    };
  }
  else if (strcmp(tag, CS_ISO8859_8) == 0) {
    if (*prev != cs_LatinHebrew) {
      *outc = ctextConcat(*outc, *outlen,
			  (unsigned char *)CTEXT_SET_ISO8859_8,
			  (unsigned int)CTEXT_SET_ISO8859_8_LEN);
      *outlen += CTEXT_SET_ISO8859_8_LEN;
      *prev = cs_LatinHebrew;
    };
  }
  else
    if (strcmp(tag, CS_ISO8859_9) == 0) {
      if (*prev != cs_Latin5) {
	*outc = ctextConcat(*outc, *outlen,
			    (unsigned char *)CTEXT_SET_ISO8859_9,
			    (unsigned int)CTEXT_SET_ISO8859_9_LEN);
	*outlen += CTEXT_SET_ISO8859_9_LEN;
	*prev = cs_Latin5;
      };
    }
    else if (strcmp(tag, CS_JISX0201) == 0) {
      if (*prev != cs_Katakana) {
	*outc = ctextConcat(*outc, *outlen,
			    (unsigned char *)CTEXT_SET_JISX0201,
			    (unsigned int)CTEXT_SET_JISX0201_LEN);
	*outlen += CTEXT_SET_JISX0201_LEN;
	*prev = cs_Katakana;
      };
    }
    else if ((strcmp(tag, CS_GB2312_0) == 0) ||
	     (strcmp(tag, CS_GB2312_1) == 0)) {
      if (*prev != cs_Hanzi) {
	*outc = ctextConcat(*outc, *outlen,
			    (unsigned char *)CTEXT_SET_GB2312_0,
			    (unsigned int)CTEXT_SET_GB2312_0_LEN);
	*outlen += CTEXT_SET_GB2312_0_LEN;
	*prev = cs_Hanzi;
      };
    }
    else if ((strcmp(tag, CS_JISX0208_0) == 0) ||
	     (strcmp(tag, CS_JISX0208_1) == 0)) {
      if (*prev != cs_JapaneseGCS) {
	*outc = ctextConcat(*outc, *outlen,
			    (unsigned char *)CTEXT_SET_JISX0208_0,
			    (unsigned int)CTEXT_SET_JISX0208_0_LEN);
	*outlen += CTEXT_SET_JISX0208_0_LEN;
	*prev = cs_JapaneseGCS;
      };
    }
    else if ((strcmp(tag, CS_KSC5601_0) == 0) ||
	     (strcmp(tag, CS_KSC5601_1) == 0)) {
      if (*prev != cs_KoreanGCS) {
	*outc = ctextConcat(*outc, *outlen,
			    (unsigned char *)CTEXT_SET_KSC5601_0,
			    (unsigned int)CTEXT_SET_KSC5601_0_LEN);
	*outlen += CTEXT_SET_KSC5601_0_LEN;
	*prev = cs_KoreanGCS;
      };
    }
    else if (strcmp(tag, CS_ISO_IR_111) == 0) {
      if (*prev != cs_ir111) {
	*outc = ctextConcat(*outc, *outlen,
			    (unsigned char *)CTEXT_SET_IR_111,
			    (unsigned int)CTEXT_SET_IR_111_LEN);
	*outlen += CTEXT_SET_IR_111_LEN;
	*prev = cs_ir111;
      };
    }
    else {
      /* Must be a non-standard character set! */
      OctetPtr        temp;

      len = strlen(tag);
      temp = (unsigned char *) XtMalloc(*outlen + 6 + len + 2);
      /* orig + header + tag + STX + EOS */
      memcpy( temp, *outc, *outlen);
      XtFree((char *) *outc);
      *outc = temp;
      temp = &((*outc)[*outlen]);
      /*
       ** Format is:
       **     01/11 02/05 02/15 03/nn M L tag 00/02 text
       */
      *temp++ = 0x1b;
      *temp++ = 0x25;
      *temp++ = 0x2f;
      /*
       ** HACK!  The next octet in the sequence is the # of octets/char.
       ** XmStrings don't have this information, so just set it to be
       ** variable # of octets/char, and hope the caller knows what to do.
       */
      *temp++ = 0x30;
      /* encode len in next 2 octets */
      *temp++ = 0x80 + (len+ctlen+1)/128;
      *temp++ = 0x80 + (len+ctlen+1)%128;
      strcpy((char *) temp, tag);
      temp += len;
      *temp++ = STX;
      *temp = EOS;		/* make sure there's a \0 on the end */
      *prev = cs_NonStandard;
      *outlen += 6 + len + 1;
    };

  /* Now copy in the text */
  if (ctlen > 0) {
    *outc = ctextConcat(*outc, *outlen, ctext, ctlen);
    *outlen += ctlen;
  };

  /* Finally, add the separator if any */
  if (separator) {
    *outc = ctextConcat(*outc, *outlen,
			(unsigned char *)NEWLINESTRING,
			(unsigned int)NEWLINESTRING_LEN);
    (*outlen)++;
  }
  return(True);
}


static OctetPtr
ctextConcat(
        OctetPtr str1,
        unsigned int str1len,
        const_OctetPtr str2,
        unsigned int str2len )
{
	str1 = (OctetPtr)XtRealloc((char *)str1, (str1len + str2len + 1));
	memcpy( &str1[str1len], str2, str2len);
	str1[str1len+str2len] = EOS;
	return(str1);
}

