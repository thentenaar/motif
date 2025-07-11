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
#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$XConsortium: Font.c /main/9 1995/07/14 10:56:39 drk $"
#endif
#endif
#include "testlib.h"

/*************************************************************************
*									 *
*	Name:		CommonGetFontList()				 *
*									 *
*	Purpose:	This function will given a name of a desired     *
*			return a appropriate Font List. If the font      *
*			cannot be found, the default font will be	 *
*			used. default_font is either read from the       *
*			command line or from testlib.h (DEFAULT_FONT)	 *
*									 *
*	Return:		Appropriate font list corresponding to string    *
*			input.						 *
*									 *
*************************************************************************/

XmFontList CommonGetFontList(char *fontstr)

{

#ifdef MOTIF1_1
  XFontStruct      *newfont;
#else
  static XmFontListEntry  fixed_font = NULL;
  XmFontListEntry  fontlistEntry;
#endif
  XmFontList       fontlist;


#ifdef MOTIF1_1

    if (!(newfont = XLoadQueryFont (display, fontstr)))
      newfont = XLoadQueryFont (display, default_font);
    fontlist = XmFontListCreate (newfont, XmSTRING_DEFAULT_CHARSET);

#else

  if (!fixed_font) {
    fixed_font = XmFontListEntryLoad(display, "fixed", XmFONT_IS_FONT,
				     XmFONTLIST_DEFAULT_TAG);
  }

  if ((fontlistEntry = XmFontListEntryLoad(display, fontstr, XmFONT_IS_FONT,
					   XmFONTLIST_DEFAULT_TAG)) == NULL)
    {
      fontlist = XmFontListAppendEntry(NULL, fixed_font );
    }
  else
    {
      fontlist = XmFontListAppendEntry(NULL, fontlistEntry);
      XmFontListEntryFree(&fontlistEntry);
    }

#endif

  return(fontlist);

}

/*************************************************************************
*									 *
*	Name:		CommonGetFontListithCharset()			 *
*									 *
*	Purpose:	This function will be given the name of a 	 *
*			desired font and the desired charset name	 *
*			and will return the appropriate Font List. If 	 *
*			the font cannot be found, the default font will  *
*			be used. default_font is either read from the 	 *
*			command line or from testlib.h (DEFAULT_FONT)	 *
*									 *
*	Return:		Appropriate font list                            *
*									 *
*************************************************************************/

XmFontList CommonGetFontListWithCharset(char *fontstr, char *charset)

{

#ifdef MOTIF1_1
    XFontStruct   *newfont;
#else
    static XmFontListEntry fixed_font = NULL;
    XmFontListEntry fontlistEntry;
#endif
    XmFontList    fontlist;

#ifdef MOTIF1_1

    if (!(newfont = XLoadQueryFont (display, fontstr)))
    	newfont = XLoadQueryFont (display, default_font);
    fontlist = XmFontListCreate (newfont, charset);

#else

    if (!fixed_font) {
        fixed_font = XmFontListEntryLoad(display, "fixed", XmFONT_IS_FONT,
                                         charset);
    }

    if ((fontlistEntry = XmFontListEntryLoad(display, fontstr, XmFONT_IS_FONT,
                                        charset)) == NULL)
      {
	fontlist = XmFontListAppendEntry(NULL, fixed_font);
      }
    else
      {
	fontlist = XmFontListAppendEntry(NULL, fontlistEntry);
	XmFontListEntryFree(&fontlistEntry);
      }

#endif

	return(fontlist);

}
