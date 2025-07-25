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
static char *rcsidRowColumnIH = "$XConsortium: RowColumnI.h /main/6 1996/08/15 17:26:22 pascale $";
#endif
#endif

#ifndef _XmRowColumnI_h
#define _XmRowColumnI_h

#include <Xm/RowColumnP.h>

#ifdef __cplusplus
extern "C" {
#endif

#define UNDEFINED_TYPE -1
#define POST_TIME_OUT	3 /* sec */ * 1000

#define Double(x)       ((x) << 1)
#define Half(x)         ((x) >> 1)

#define IsSensitive(r)      XtIsSensitive(r)
#define IsManaged(w)        XtIsManaged(w)
#define IsNull(p)       ((p) == NULL)

#define PackTight(m)        (RC_Packing (m) == XmPACK_TIGHT)
#define PackColumn(m)       (RC_Packing (m) == XmPACK_COLUMN)
#define PackNone(m)         (RC_Packing (m) == XmPACK_NONE)

#define Asking(i)       ((i) == 0)
#define IsVertical(m)   \
       (((XmRowColumnWidget) (m))->row_column.orientation == XmVERTICAL)
#define IsHorizontal(m) \
       (((XmRowColumnWidget) (m))->row_column.orientation == XmHORIZONTAL)
#define IsAligned(m)    \
       (((XmRowColumnWidget) (m))->row_column.do_alignment)

#define IsPopup(m)     \
    (((XmRowColumnWidget) (m))->row_column.type == XmMENU_POPUP)
#define IsPulldown(m)  \
    (((XmRowColumnWidget) (m))->row_column.type == XmMENU_PULLDOWN)
#define IsOption(m)    \
    (((XmRowColumnWidget) (m))->row_column.type == XmMENU_OPTION)
#define IsBar(m)       \
    (((XmRowColumnWidget) (m))->row_column.type == XmMENU_BAR)
#define IsWorkArea(m)  \
    (((XmRowColumnWidget) (m))->row_column.type == XmWORK_AREA)
#define IsRadio(m)     \
    ((((XmRowColumnWidget) (m))->row_column.type == XmWORK_AREA) && \
         ((((XmRowColumnWidget) (m))->row_column.radio)))
#define IsHelp(m,w)     ((w) == RC_HelpPb (m))

#define WasManaged(w)  \
    (((XmRowColumnConstraintRec *) ((w)->core.constraints))-> \
     row_column.was_managed)

#define SavedMarginTop(w)  \
    (((XmRowColumnConstraintRec *) ((w)->core.constraints))-> \
     row_column.margin_top)

#define SavedMarginBottom(w)  \
    (((XmRowColumnConstraintRec *) ((w)->core.constraints))-> \
     row_column.margin_bottom)

#define SavedBaseline(w)  \
    (((XmRowColumnConstraintRec *) ((w)->core.constraints))-> \
     row_column.baseline)

#define BX(b)           ((b)->x)
#define BY(b)           ((b)->y)
#define BWidth(b)       ((b)->width)
#define BHeight(b)      ((b)->height)
#define BBorder(b)      ((b)->border_width)

#define SetPosition(b,x,y)  { BX (b) = x;  BY (b) = y; }

#define ChangeMargin(margin,new_w,sum)  {  \
    if ((margin) != new_w)        \
    {               \
        sum += new_w - (margin);  \
        (margin) = new_w;     \
    }\
}

#define ChangeMarginDouble(margin,new_w,sum) {   \
    if ((margin) != new_w)        \
    {               \
        sum += 2* (new_w - (margin));  \
        (margin) = new_w;     \
    }\
}

#define ForAllChildren(m, i, q)     \
    for (i = 0, q = m->composite.children; \
     i < m->composite.num_children;     \
     i++, q++)

#define ForManagedChildren(m, i, q)  \
    for (i = 0, q = m->composite.children; \
     i < m->composite.num_children;     \
     i++, q++)          \
                    \
    if (XtIsManaged(*q))

#define AlignmentBaselineTop(m) \
(((XmRowColumnWidget) (m))->row_column.entry_vertical_alignment == XmALIGNMENT_BASELINE_TOP)
#define AlignmentBaselineBottom(m) \
(((XmRowColumnWidget) (m))->row_column.entry_vertical_alignment == XmALIGNMENT_BASELINE_BOTTOM)
#define AlignmentCenter(m) \
(((XmRowColumnWidget) (m))->row_column.entry_vertical_alignment == XmALIGNMENT_CENTER)
#define AlignmentTop(m) \
(((XmRowColumnWidget) (m))->row_column.entry_vertical_alignment == XmALIGNMENT_CONTENTS_TOP)
#define AlignmentBottom(m) \
(((XmRowColumnWidget) (m))->row_column.entry_vertical_alignment == XmALIGNMENT_CONTENTS_BOTTOM)

/* Warning Messages */
#define BadWidthSVMsg			_XmMMsgRowColumn_0000
#define BadHeightSVMsg			_XmMMsgRowColumn_0001
#define BadPopupHelpMsg 		_XmMMsgRowColumn_0002
#define BadPulldownHelpMsg		_XmMMsgRowColumn_0003
#define BadOptionHelpMsg		_XmMMsgRowColumn_0004
#define BadWorkAreaHelpMsg		_XmMMsgRowColumn_0005
#define BadTypeParentMsg		_XmMMsgRowColumn_0007
#define BadTypeSVMsg			_XmMMsgRowColumn_0008
#define BadMenuBarHomogenousSVMsg	_XmMMsgRowColumn_0015
#define BadMenuBarEntryClassSVMsg	_XmMMsgRowColumn_0016
#define BadPulldownWhichButtonSVMsg	_XmMMsgRowColumn_0017
#define BadPulldownMenuPostSVMsg	_XmMMsgRowColumn_0018
#define BadMenuPostMsg			_XmMMsgRowColumn_0019
#define BadShadowThicknessSVMsg		_XmMMsgRowColumn_0020
#define WrongMenuChildMsg		_XmMMsgRowColumn_0022
#define WrongChildMsg			_XmMMsgRowColumn_0023
#define BadOptionIsHomogeneousSVMsg	_XmMMsgRowColumn_0025
#define TearOffSharedMenupaneMsg	_XmMMsgRowColumn_0026
#define BadMnemonicCharMsg		_XmMMsgRowColumn_0027

#define RCIndex(w)    (((XmRowColumnConstraintRec *)(w)->core.constraints)\
                       ->row_column.position_index)


/********    Private Function Declarations    ********/

extern void _XmRC_KeyboardInputHandler(
				       Widget reportingWidget,
				       XtPointer data,
				       XEvent *event,
				       Boolean *cont );
extern void _XmAllowAcceleratedInsensitiveUnmanagedMenuItems(
							     Widget wid,
							     Boolean allowed);
extern void _XmPostPopupMenu(
                        Widget wid,
                        XEvent *event) ;
extern void _XmCallRowColumnMapCallback(
                        Widget wid,
                        XEvent *event) ;
extern void _XmCallRowColumnUnmapCallback(
                        Widget wid,
                        XEvent *event) ;
extern void _XmRC_RemoveFromPostFromListOnDestroyCB (
 			Widget w,
 			caddr_t clientData,
 			caddr_t callData) ;

extern void _XmRC_CheckAndSetOptionCascade(XmRowColumnWidget menu) ;

/********    End Private Function Declarations    ********/

#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif  /* _XmRowColumnI_h */
/* DON'T ADD STUFF AFTER THIS #endif */
