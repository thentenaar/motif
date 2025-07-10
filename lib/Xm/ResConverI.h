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
 *
 */
/*
 * HISTORY
 */
/* $XConsortium: ResConverI.h /main/5 1995/07/13 17:48:35 drk $ */
#ifndef _XmResConvertI_h
#define _XmResConvertI_h

#include <Xm/XmP.h>

#ifdef __cplusplus
extern "C" {
#endif

/********    Private Function Declarations for ResConvert.c    ********/

extern void _XmRegisterConverters( void ) ;
extern char * _XmConvertCSToString(
                        XmString cs) ;
extern Boolean _XmCvtXmStringToCT(
                        XrmValue *from,
                        XrmValue *to) ;
#if XM_UTF8
extern Boolean _XmCvtXmStringToUTF8String(
                        XrmValue *from,
                        XrmValue *to) ;
#endif
/********    End Private Function Declarations    ********/

#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmResConvertI_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
