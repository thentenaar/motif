/* $XConsortium: TxtPropCv.h /main/5 1995/07/15 20:56:52 drk $ */
/*
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
 *
 */
/*
 * HISTORY
 */

#ifndef _XmTxtPropCvP_h
#define _XmTxtPropCvP_h

#include <X11/Intrinsic.h>

#ifndef _Xm_h
typedef union __XmStringRec	*XmString;
typedef XmString *XmStringTable;
#endif

/* We are making an attempt (perhaps unnecessaryily) to keep our style
   constants the same as the equivalent Xlib style constants. The first
   Motif specific style constant starts at 32 so that the consortium can
   add constants to their list without overlapping with ours. */
typedef enum {
	XmSTYLE_STRING            = XStringStyle,
	XmSTYLE_COMPOUND_TEXT     = XCompoundTextStyle,
	XmSTYLE_TEXT              = XTextStyle,
	XmSTYLE_STANDARD_ICC_TEXT = XStdICCTextStyle,
	XmSTYLE_LOCALE            = 32,
	XmSTYLE_COMPOUND_STRING
} XmICCEncodingStyle;

#ifdef __cplusplus
extern "C" {
#endif

/********    Public Function Declarations    ********/

extern int XmCvtXmStringTableToTextProperty(Display *display,
				 XmStringTable string_table,
				 int count,
				 XmICCEncodingStyle style,
				 XTextProperty *text_prop_return);

extern int XmCvtTextPropertyToXmStringTable(Display *display,
				 XTextProperty *text_prop,
				 XmStringTable *string_table_return,
				 int *count_return);

/**
 * Convert an XmString to XTextProperty
 *
 * Converts the string specified by \a s into the given \a prop. The
 * encoding field of \a prop should be set to the desired property
 * encoding, which should be one of the following atoms:
 * COMPOUND_TEXT, STRING, UTF8_STRING, or _MOTIF_COMPOUND_STRING.
 *
 * The caller must free the value member of \a prop when done
 * with it.
 *
 * This function returns Success on success, and on failure returns
 * XNoMemory, XConverterNotFound, or XLocaleNotSupported as reported
 * by Xutf8TextListToTextProperty.
 */
extern int XmCvtXmStringToTextProperty(Display *d, const XmString s, XTextProperty *prop);

/**
 * Convert a XmTextProperty to XmString
 *
 * Returns the XmString representation of the given \a prop, or NULL
 * if \a prop could not be converted to XmString.
 *
 * Remember to free the resulting XmString with XmStringFree() when
 * done with it.
 */
extern XmString XmCvtTextPropertyToXmString(Display *d, XTextProperty *prop);

/********    End Public Function Declarations    ********/

#ifdef __cplusplus
}
#endif
#endif  /* _XmTxtPropCvP_h */

