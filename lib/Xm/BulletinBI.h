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
/* $XConsortium: BulletinBI.h /main/7 1996/06/14 23:09:13 pascale $ */
#ifndef _XmBulletinBI_h
#define _XmBulletinBI_h

#include <Xm/BulletinBP.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    XmOkStringLoc,
    XmCancelStringLoc,
    XmSelectionStringLoc,
    XmApplyStringLoc, 
    XmHelpStringLoc,
    XmFilterStringLoc,
    XmDirListStringLoc,
    XmItemsStringLoc,
    XmDirTextStringLoc,
    XmPromptStringLoc
} XmLabelStringLoc;

/********    Private Function Declarations    ********/

extern Widget _XmBB_CreateButtonG( 
                        Widget bb,
                        XmString l_string,
			char *name,
                        XmLabelStringLoc l_loc) ;
extern Widget _XmBB_CreateLabelG( 
                        Widget bb,
                        XmString l_string,
                        char *name,
                        XmLabelStringLoc l_loc) ;
extern void _XmBulletinBoardSizeUpdate( 
                        Widget wid) ;
extern void _XmBulletinBoardFocusMoved( 
                        Widget wid,
                        XtPointer client_data,
                        XtPointer data) ;
extern void _XmBulletinBoardReturn( 
                        Widget wid,
                        XEvent *event,
                        String *params,
                        Cardinal *numParams) ;
extern void _XmBulletinBoardCancel( 
                        Widget wid,
                        XEvent *event,
                        String *params,
                        Cardinal *numParams) ;
extern void _XmBulletinBoardMap( 
                         Widget wid,
                         XEvent *event,
                         String *params,
                         Cardinal *numParams) ;
extern void _XmBulletinBoardSetDynDefaultButton( 
                        Widget wid,
                        Widget newDefaultButton) ;

/********    End Private Function Declarations    ********/

#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmBulletinBI_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
