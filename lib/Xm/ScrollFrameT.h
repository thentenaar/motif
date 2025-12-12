/* $TOG: ScrollFrameT.h /main/6 1997/07/25 16:49:23 samborn $ */
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
/*
 * HISTORY
 */
#ifndef _XmScrollFrameT_H
#define _XmScrollFrameT_H

#include <Xm/Xm.h>
#include <Xm/NavigatorT.h>

#ifdef __cplusplus
extern "C" {
#endif

externalref XrmQuark XmQTscrollFrame;

/* Trait structures and typedefs, place typedefs first */

typedef void (*XmScrollFrameInitProc)(Widget sf, 
				      XtCallbackProc moveCB,
				      Widget scrollable);
typedef Boolean  (*XmScrollFrameGetInfoProc)(Widget sf,
					     Cardinal * dimension,
					     Widget ** nav_list,
					     Cardinal * num_nav_list);
typedef void (*XmScrollFrameAddNavigatorProc)(Widget sf, 
					      Widget nav,
					      Mask dimMask);
typedef void (*XmScrollFrameRemoveNavigatorProc)(Widget sf, 
						 Widget nav);
typedef void (*XmScrollFrameUpdateOrigGeomProc)(Widget sf, 
						Widget child,
						XtWidgetGeometry *geom);


/* Version 1: added updateOrigGeom */

typedef struct _XmScrollFrameTraitRec {
  int				    version;		/* 1 */
  XmScrollFrameInitProc		    init;       
  XmScrollFrameGetInfoProc	    getInfo; 
  XmScrollFrameAddNavigatorProc     addNavigator;
  XmScrollFrameRemoveNavigatorProc  removeNavigator;
  XmScrollFrameUpdateOrigGeomProc   updateOrigGeom;
} XmScrollFrameTraitRec, *XmScrollFrameTrait;


/* This one gets allocated per instance by the scrollFrame
   class. It is just a convenient structure reusable by other scrollFrame
   and it needs not to be part of the public trait API */

typedef struct _XmScrollFrameDataRec {
   XtCallbackProc move_cb ;
   Widget         scrollable ;
   Widget *       nav_list;
   Cardinal       num_nav_list ;
   Cardinal       num_nav_slots;
} XmScrollFrameDataRec, *XmScrollFrameData;


#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmScrollFrameT_H */
