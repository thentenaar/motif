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
#ifndef _XmProtocolsP_h
#define _XmProtocolsP_h

#include <Xm/Protocols.h>
#include <Xm/ExtObjectP.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct _XmProtocolClassPart {
    XtPointer	extension;
} XmProtocolClassPart;

typedef struct _XmProtocolClassRec {
    ObjectClassPart	object_class;
    XmExtClassPart	ext_class;
    XmProtocolClassPart	protocol_class;
} XmProtocolClassRec, *XmProtocolObjectClass;

typedef struct _XmProtocolPart {
    XtCallbackRec	pre_hook, post_hook;
    XtCallbackList	callbacks;
    Atom		atom;
    Boolean		active;
} XmProtocolPart, *XmProtocolPartPtr;

typedef struct _XmProtocolRec {
    ObjectPart			object;
    XmExtPart			ext;
    XmProtocolPart		protocol;
} XmProtocolRec, *XmProtocol, **XmProtocolList;

#ifndef XmIsProtocol
#define XmIsProtocol(w)	XtIsSubclass(w, xmProtocolObjectClass)
#endif /* XmIsProtocol */

/* Class record constants */

externalref XmProtocolClassRec 	xmProtocolClassRec;
externalref WidgetClass xmProtocolObjectClass;

typedef struct _XmProtocolMgrRec{
    Atom		property;
    XmProtocolList 	protocols;
    Cardinal		num_protocols;
    Cardinal		max_protocols;
}XmProtocolMgrRec, *XmProtocolMgr, **XmProtocolMgrList;


typedef struct _XmAllProtocolsMgrRec{
  XmProtocolMgrList	protocol_mgrs;
  Cardinal		num_protocol_mgrs;
  Cardinal		max_protocol_mgrs;
  Widget		shell;
}XmAllProtocolsMgrRec, *XmAllProtocolsMgr;
    

#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmProtocolsP_h */
