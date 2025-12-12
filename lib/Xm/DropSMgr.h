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
*/ 
#ifndef _XmDropSMgr_h
#define _XmDropSMgr_h

#include <Xm/Xm.h>
#include <Xm/DragC.h>

#ifdef __cplusplus
extern "C" {
#endif


#define XmCR_DROP_SITE_LEAVE_MESSAGE  1
#define XmCR_DROP_SITE_ENTER_MESSAGE  2
#define XmCR_DROP_SITE_MOTION_MESSAGE 3
#define XmCR_DROP_MESSAGE             4

#define XmNO_DROP_SITE 		1
#define XmINVALID_DROP_SITE	2
#define XmVALID_DROP_SITE	3

/* begin fix for CR 5754 */
/* documented values are XmDROP_SITE_VALID and XmDROP_SITE_INVALID.
   However, we can't just throw out the incorrect Xm[IN]VALID_DROP_SITE
   now since people have probably started using them. Instead, we just
   define the correct values using the incorrect ones.
*/

#define XmDROP_SITE_INVALID XmINVALID_DROP_SITE
#define XmDROP_SITE_VALID XmVALID_DROP_SITE

/* end fix for CR 5754 */

enum { XmDRAG_UNDER_NONE, XmDRAG_UNDER_PIXMAP,
    XmDRAG_UNDER_SHADOW_IN, XmDRAG_UNDER_SHADOW_OUT,
    XmDRAG_UNDER_HIGHLIGHT };

enum { XmDROP_SITE_SIMPLE, XmDROP_SITE_COMPOSITE,
    XmDROP_SITE_SIMPLE_CLIP_ONLY = 128,
    XmDROP_SITE_COMPOSITE_CLIP_ONLY };

enum { XmABOVE, XmBELOW };

enum { XmDROP_SITE_ACTIVE, XmDROP_SITE_INACTIVE, XmDROP_SITE_IGNORE };

typedef struct _XmDragProcCallbackStruct {
    int				reason;
    XEvent *		event;
    Time			timeStamp;
	Widget			dragContext;
    Position		x, y;
    unsigned char	dropSiteStatus;
    unsigned char	operation;
    unsigned char	operations;
	Boolean			animate;
} XmDragProcCallbackStruct, * XmDragProcCallback;

typedef struct _XmDropProcCallbackStruct {
    int				reason;
    XEvent *		event;
    Time			timeStamp;
	Widget			dragContext;
    Position		x, y;
    unsigned char	dropSiteStatus;
    unsigned char	operation;
    unsigned char	operations;
	unsigned char	dropAction;
} XmDropProcCallbackStruct, * XmDropProcCallback;


typedef struct _XmDropSiteVisualsRec {
	Pixel	background;
	Pixel	foreground;
	Pixel	topShadowColor;
	Pixmap	topShadowPixmap;
	Pixel	bottomShadowColor;
	Pixmap	bottomShadowPixmap;
	Dimension	shadowThickness;
	Pixel	highlightColor;
	Pixmap	highlightPixmap;
	Dimension	highlightThickness;
	Dimension	borderWidth;
} XmDropSiteVisualsRec, * XmDropSiteVisuals;


/* DropSite Widget */

externalref WidgetClass xmDropSiteManagerObjectClass;

typedef struct _XmDropSiteManagerClassRec *XmDropSiteManagerObjectClass;
typedef struct _XmDropSiteManagerRec *XmDropSiteManagerObject;

#ifndef XmIsDropSiteManager
#define XmIsDropSiteManager(w)  XtIsSubclass((w), xmDropSiteManagerObjectClass)
#endif /* XmIsDropSite */

/********    Public Function Declarations    ********/

extern void XmDropSiteRegister( 
                        Widget widget,
                        ArgList args,
                        Cardinal argCount) ;
extern void XmDropSiteUnregister( 
                        Widget widget) ;
extern Boolean XmDropSiteRegistered(
                          Widget widget) ;
extern void XmDropSiteStartUpdate( 
                        Widget refWidget) ;
extern void XmDropSiteUpdate( 
                        Widget enclosingWidget,
                        ArgList args,
                        Cardinal argCount) ;
extern void XmDropSiteEndUpdate( 
                        Widget refWidget) ;
extern void XmDropSiteRetrieve( 
                        Widget enclosingWidget,
                        ArgList args,
                        Cardinal argCount) ;
extern int XmDropSiteQueryStackingOrder( 
                        Widget widget,
                        Widget *parent_rtn,
                        Widget **children_rtn,
                        Cardinal *num_children_rtn) ;
extern void XmDropSiteConfigureStackingOrder( 
                        Widget widget,
                        Widget sibling,
                        Cardinal stack_mode) ;
extern XmDropSiteVisuals XmDropSiteGetActiveVisuals( 
                        Widget widget) ;

/********    End Public Function Declarations    ********/

#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmDropSMgr_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
