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
#ifndef _XmProtocols_h
#define _XmProtocols_h

#include <Xm/Xm.h>

#ifdef __cplusplus
extern "C" {
#endif

/* should be in XmP.h */

#ifndef XmCR_WM_PROTOCOLS
#define XmCR_WM_PROTOCOLS 6666
#endif /* XmCR_WM_PROTOCOLS */

/* define the XM_PROTOCOLS atom for use in  routines */
#ifdef XA_WM_PROTOCOLS
#define XM_WM_PROTOCOL_ATOM(shell) XA_WM_PROTOCOLS
#else
#define XM_WM_PROTOCOL_ATOM(shell) \
    XInternAtom(XtDisplay(shell),"WM_PROTOCOLS",FALSE)
#endif /* XA_WM_PROTOCOLS */


#define XmAddWMProtocols(shell, protocols, num_protocols) \
      XmAddProtocols(shell, XM_WM_PROTOCOL_ATOM(shell), \
			 protocols, num_protocols)

#define XmRemoveWMProtocols(shell, protocols, num_protocols) \
      XmRemoveProtocols(shell, XM_WM_PROTOCOL_ATOM(shell), \
			protocols, num_protocols)

#define XmAddWMProtocolCallback(shell, protocol, callback, closure) \
      XmAddProtocolCallback(shell, XM_WM_PROTOCOL_ATOM(shell), \
			    protocol, callback, closure)

#define XmRemoveWMProtocolCallback(shell, protocol, callback, closure) \
  XmRemoveProtocolCallback(shell, XM_WM_PROTOCOL_ATOM(shell), \
			    protocol, callback, closure)

#define XmActivateWMProtocol(shell, protocol) \
      XmActivateProtocol(shell, XM_WM_PROTOCOL_ATOM(shell), protocol)

#define XmDeactivateWMProtocol(shell, protocol) \
      XmDeactivateProtocol(shell, XM_WM_PROTOCOL_ATOM(shell), protocol)

#define XmSetWMProtocolHooks(shell, protocol, pre_h, pre_c, post_h, post_c) \
      XmSetProtocolHooks(shell, XM_WM_PROTOCOL_ATOM(shell), \
			 protocol, pre_h, pre_c, post_h, post_c)


/********    Public Function Declarations    ********/

extern void XmAddProtocols(
                        Widget shell,
                        Atom property,
                        Atom *protocols,
                        Cardinal num_protocols) ;
extern void XmRemoveProtocols(
                        Widget shell,
                        Atom property,
                        Atom *protocols,
                        Cardinal num_protocols) ;
extern void XmAddProtocolCallback(
                        Widget shell,
                        Atom property,
                        Atom proto_atom,
                        XtCallbackProc callback,
                        XtPointer closure) ;
extern void XmRemoveProtocolCallback(
                        Widget shell,
                        Atom property,
                        Atom proto_atom,
                        XtCallbackProc callback,
                        XtPointer closure) ;
extern void XmActivateProtocol(
                        Widget shell,
                        Atom property,
                        Atom proto_atom) ;
extern void XmDeactivateProtocol(
                        Widget shell,
                        Atom property,
                        Atom proto_atom) ;
extern void XmSetProtocolHooks(
                        Widget shell,
                        Atom property,
                        Atom proto_atom,
                        XtCallbackProc pre_hook,
                        XtPointer pre_closure,
                        XtCallbackProc post_hook,
                        XtPointer post_closure) ;

/********    End Public Function Declarations    ********/


#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmProtocols_h */
