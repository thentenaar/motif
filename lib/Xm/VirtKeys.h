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
#ifndef _XmVirtKeys_h
#define _XmVirtKeys_h

#include <Xm/Xm.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _OSF_Keysyms
#define _OSF_Keysyms

#define osfXK_Activate		0x1004FF44
#define osfXK_AddMode		0x1004FF31
#define osfXK_BackSpace		0x1004FF08
#define osfXK_BackTab		0x1004FF07
#define osfXK_BeginData		0x1004FF5A
#define osfXK_BeginLine		0x1004FF58
#define osfXK_Cancel		0x1004FF69
#define osfXK_Clear		0x1004FF0B
#define osfXK_Copy		0x1004FF02
#define osfXK_Cut		0x1004FF03
#define osfXK_Delete		0x1004FFFF
#define osfXK_DeselectAll	0x1004FF72
#define osfXK_Down		0x1004FF54
#define osfXK_EndData		0x1004FF59
#define osfXK_EndLine		0x1004FF57
#define osfXK_Escape		0x1004FF1B
#define osfXK_Extend		0x1004FF74
#define osfXK_Help		0x1004FF6A
#define osfXK_Insert		0x1004FF63
#define osfXK_Left		0x1004FF51
#define osfXK_LeftLine		0x1004FFF8
#define osfXK_Menu		0x1004FF67
#define osfXK_MenuBar		0x1004FF45
#define osfXK_Next		0x1004FF56
#define osfXK_NextField		0x1004FF5E
#define osfXK_NextMenu		0x1004FF5C
#define osfXK_NextMinor		0x1004FFF5
#define osfXK_PageDown		0x1004FF42
#define osfXK_PageLeft		0x1004FF40
#define osfXK_PageRight		0x1004FF43
#define osfXK_PageUp		0x1004FF41
#define osfXK_Paste		0x1004FF04
#define osfXK_PrevField		0x1004FF5D
#define osfXK_PrevMenu		0x1004FF5B
#define osfXK_PrimaryPaste	0x1004FF32
#define osfXK_Prior		0x1004FF55
#define osfXK_PriorMinor	0x1004FFF6
#define osfXK_QuickPaste	0x1004FF33
#define osfXK_Reselect		0x1004FF73
#define osfXK_Restore		0x1004FF78
#define osfXK_Right		0x1004FF53
#define osfXK_RightLine		0x1004FFF7
#define osfXK_Select		0x1004FF60
#define osfXK_SelectAll		0x1004FF71
#define osfXK_SwitchDirection	0x1004FF7E
#define osfXK_Undo		0x1004FF65
#define osfXK_Up		0x1004FF52

#endif  /* OSF_Keysyms */


/********    Public Function Declarations    ********/

extern void XmTranslateKey(
                        Display *dpy,
                        KeyCode keycode,
                        Modifiers modifiers,
                        Modifiers *modifiers_return,
                        KeySym *keysym_return) ;

/********    End Public Function Declarations    ********/


#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmVirtKeys_h */
