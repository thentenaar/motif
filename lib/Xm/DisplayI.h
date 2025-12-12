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
#ifndef _XmDisplayI_h
#define _XmDisplayI_h

#include <Xm/DisplayP.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _XmDisplayEventQueryStruct {
  XmDisplay			dd;
  XmDragContext			dc;
  XmTopLevelEnterCallbackStruct	*enterCB;
  XmDragMotionCallbackStruct	*motionCB;
  XmTopLevelLeaveCallbackStruct	*leaveCB;
  XmDropStartCallbackStruct	*dropStartCB;
  Boolean			hasEnter;
  Boolean			hasMotion;
  Boolean			hasLeave;
  Boolean			hasDropStart;
} XmDisplayEventQueryStruct;


/********    Private Function Declarations    ********/

extern XmDropSiteManagerObject _XmGetDropSiteManagerObject( 
                        XmDisplay xmDisplay) ;
extern unsigned char _XmGetDragProtocolStyle( 
                        Widget w) ;
extern unsigned char _XmGetDragTrackingMode( 
                        Widget w) ;
extern Widget _XmGetDragContextFromHandle( 
                        Widget w,
                        Atom iccHandle) ;
extern WidgetClass _XmGetXmDisplayClass( void ) ;
extern WidgetClass _XmSetXmDisplayClass( 
                        WidgetClass wc) ;

/********    End Private Function Declarations    ********/

#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmDisplayI_h */
/* DON'T ADD STUFF AFTER THIS #endif */

