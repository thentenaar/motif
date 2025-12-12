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
/* $XConsortium: TextI.h /main/6 1996/05/29 13:45:16 pascale $ */
#ifndef _XmTextI_h
#define _XmTextI_h

#include <Xm/TextP.h>

#ifdef __cplusplus
extern "C" {
#endif

/********    Private Function Declarations    ********/

extern XmTextPosition _XmTextFindScroll(XmTextWidget widget,
					XmTextPosition start,
					int delta);
extern int _XmTextGetTotalLines(Widget widget);
extern XmTextLineTable _XmTextGetLineTable(Widget widget,
					   int *total_lines);
extern void _XmTextRealignLineTable(XmTextWidget widget,
				    XmTextLineTable *temp_table,
				    int *temp_table_size,
				    register unsigned int cur_index,
				    register XmTextPosition cur_start,
				    register XmTextPosition cur_end);
extern unsigned int _XmTextGetTableIndex(XmTextWidget widget,
					 XmTextPosition pos);
extern void _XmTextUpdateLineTable(Widget widget,
				   XmTextPosition start,
				   XmTextPosition end,
				   XmTextBlock block,
				   Boolean update
				   );
extern void _XmTextMarkRedraw(XmTextWidget widget,
			      XmTextPosition left,
			      XmTextPosition right);
extern LineNum _XmTextNumLines(XmTextWidget widget);
extern void _XmTextLineInfo(XmTextWidget widget,
			    LineNum line,
			    XmTextPosition *startpos,
			    LineTableExtra *extra);
extern LineNum _XmTextPosToLine(XmTextWidget widget,
				XmTextPosition position);
extern void _XmTextInvalidate(XmTextWidget widget,
			      XmTextPosition position,
			      XmTextPosition topos,
			      long delta);
extern void _XmTextSetTopCharacter(Widget widget,
				   XmTextPosition top_character);
extern int _XmTextCountCharacters(char *str,
				  int num_count_bytes);
extern void _XmTextSetCursorPosition(Widget widget,
				     XmTextPosition position);
extern void _XmTextDisableRedisplay(XmTextWidget widget,
                                    Boolean losesbackingstore);
extern void _XmTextEnableRedisplay(XmTextWidget widget);

extern void _XmTextSetHighlight(Widget, XmTextPosition,
                                XmTextPosition, XmHighlightMode);
extern void _XmTextShowPosition(Widget, XmTextPosition);
extern void _XmTextSetEditable(Widget widget,
                               Boolean editable);
extern void _XmTextResetIC(Widget widget);
extern Boolean _XmTextNeedsPendingDeleteDis(XmTextWidget tw,
                                            XmTextPosition *left,
                                            XmTextPosition *right,
                                            int check_add_mode);
extern void _XmTextReplace(Widget widget,
                           XmTextPosition frompos,
                           XmTextPosition topos,
	                   char *value,
                           Boolean is_wchar);
extern void _XmTextValidate(XmTextPosition *start,
		            XmTextPosition *end,
		            int maxsize);

extern XmTextPosition _XmTextSetPreeditPosition(Widget w,
                                                XmTextPosition cursor_position);
/********    End Private Function Declarations    ********/


#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmTextI_h */
