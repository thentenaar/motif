/* $XConsortium: wsmSend.h /main/5 1995/07/15 20:46:36 drk $ */
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

#ifndef wsmSend_h
#define wsmSend_h

#include "wsm.h"

extern void CreateStartStateRequest(
WSMGetStateReply *get_state,
WSMRequest *request
				    );

extern void CreateReStartStateRequest(
WSMGetStateReply *get_state,
WSMRequest *request
				    );



extern void CreateReStartStateRequest2(
WSMGetStateReply *get_state,
WSMRequest *request
				    );

extern void GetChangeSpaceRequest(
Space*, 
WSMRequest*
);


extern void GetRegisterOldWindowReply(
WorkWindow*, WSMReply*
);


extern void GetRegisterWindowReply(
Window, WSMReply*
);


extern void SaveState(
WSMGetStateReply*,
Boolean
);

extern void RemoveWindow(
Window 
);


extern void DealWithMapNotify(
Widget,
XMapEvent *ev
);
extern void DealWithUnmapNotify(
Widget,
XUnmapEvent *ev
);



extern void DealWithClientMessage(
Widget,
XClientMessageEvent *ev
);



extern void DealWithDestroyNotify(
Widget,
XDestroyWindowEvent *ev
);

extern void DealWithDestroyMwmNotify();

extern Boolean GetCopyWindowRequest(
Window,
Space *,
Space *,
WSMRequest *
);




extern void CopyWindowToAllSpaces(
WorkWindow*,
WorkWindow*,
XrmValue*,
XrmValue*,
Space *
);


extern void CopyWindowToSpace(
WorkWindow*,
WorkWindow*,
XrmValue*,
XrmValue*,
Space *
);



extern Boolean GetMoveWindowRequest(
Window,
Space *,
Space *,
WSMRequest *
);




extern Boolean GetDeleteWindowRequest(
Window,
Space *,
WSMRequest *
);



extern Boolean GetLinkWindowRequest(
Window,
Space *,
Space *,
WSMRequest *
);


extern Boolean GetManageAllWindowsRequest(
WSMRequest*
					  );

#endif
