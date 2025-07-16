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
#ifndef _XmTextStrSoI_h
#define _XmTextStrSoI_h

#include <Xm/TextStrSoP.h>

#ifdef __cplusplus
extern "C" {
#endif

/********    Private Function Declarations    ********/

extern char  * _XmStringSourceGetString(XmTextWidget tw,
				        XmTextPosition from,
				        XmTextPosition to,
                                        Boolean want_wchar);
extern Boolean _XmTextFindStringBackwards(Widget w,
					  XmTextPosition start,
					  char *search_string,
					  XmTextPosition *position);
extern Boolean _XmTextFindStringForwards(Widget w,
					 XmTextPosition start,
					 char *search_string,
					 XmTextPosition *position);
extern void    _XmStringSourceSetGappedBuffer(XmSourceData data,
					      XmTextPosition position);
extern Boolean _XmTextModifyVerify(XmTextWidget initiator,
				   XEvent *event,
				   XmTextPosition *start,
				   XmTextPosition *end,
				   XmTextPosition *cursorPos,
				   XmTextBlock block,
				   XmTextBlock newblock,
				   Boolean *freeBlock);
extern XmTextSource _XmStringSourceCreate(char *value,
                                          Boolean is_wchar);
extern void    _XmStringSourceDestroy(XmTextSource source);
extern char  * _XmStringSourceGetValue(XmTextSource source,
                                       Boolean want_wchar);
extern void    _XmStringSourceSetValue(XmTextWidget widget,
				       char *value);
extern Boolean _XmStringSourceHasSelection(XmTextSource source);
extern Boolean _XmStringSourceGetEditable(XmTextSource source);
extern void    _XmStringSourceSetEditable(XmTextSource source,
                                         Boolean editable);
extern int     _XmStringSourceGetMaxLength(XmTextSource source);
extern void    _XmStringSourceSetMaxLength(XmTextSource source,
					   int max);
extern int _XmTextBytesToCharacters(char *characters,
				    char *bytes,
				    int num_chars,
				    Boolean add_null_terminator,
				    int max_char_size);
extern int _XmTextCharactersToBytes(char *bytes,
				    char *characters,
				    int num_chars,
				    int max_char_size);
extern void    _XmTextValueChanged(XmTextWidget initiator,
				   XEvent *event);
extern Boolean *_XmStringSourceGetPending(XmTextWidget widget);
extern void    _XmStringSourceSetPending(XmTextWidget widget,
					 Boolean *pending);

/********    End Private Function Declarations    ********/


#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /*  _XmTextStrSoI_h */
