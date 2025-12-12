/* $TOG: wsm.h /main/6 1997/05/02 10:08:43 dbl $ */
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
#ifndef wsm_h
#define wsm_h

#include "Wsm/wsm_proto.h"

/*  Internal Data Strucures */

#define MAX_LENGTH 256

extern Widget utmShell;

typedef struct _Space {
  XrmQuark nameq;
  char name[MAX_LENGTH];
  Pixmap pixmap_label;
  char pixmap_name[MAX_LENGTH];
  char background[MAX_LENGTH];
  unsigned long pixel;
  Pixmap pixmap;
  struct _WorkWindowList *w_list;
  struct _Space *next;
} Space;

typedef struct _SpaceList {
 Space *space;
 struct _SpaceList *next;
} SpaceList;

typedef struct _WorkWindow {
  Window window;
  Boolean all_workspaces;
  Boolean linked;
  Boolean mapped;
  XrmQuarkList specifier_qlist;
  char *name;
  XrmQuarkList attrib_qlist;
  WSMAttribute *attrib_list;
  int num_attrib_list;
  Space *last_space;
  WSMWinData *win_data;
  Boolean used;
  struct _SpaceList *s_list;
  struct _WorkWindow *next;
  struct _WorkWindow *prev;
} WorkWindow;

typedef struct _WorkWindowList {
  WorkWindow *work_win;
  WorkWindow *sib_win;
  struct _WorkWindowList *next;
} WorkWindowList;

#ifdef DATA
typedef struct _DataList {
   WSMWinData **data;
   struct _DataList *next;
} DataList;
#endif


typedef struct _IndexStruct {
int hide;
int iconic;
} IndexStruct;

typedef struct _AtomStruct {
Atom save_property;
Atom wm_role_property;
Atom wm_client_leader;
Atom protocols_property;
Atom sm_client_id_property;
Atom own_selection;
} AtomStruct;

/* funcs */


extern void SendConnect(
Widget
			);


extern void SendLeaveRoom(
Space*
			  );




extern void SendSaveWsm(
char *
			  );

extern void RequestReceived(
Widget,
XtPointer,
WSMRequest*,
WSMReply*
				 );


extern void ChangeRoomCB(
Widget,
XtPointer,
XtPointer
			 );

extern void PrintRoomsCB(
Widget,
XtPointer,
XtPointer
		    );

extern void SendExtensionCB(
Widget,
XtPointer,
XtPointer
		    );

extern void SendConfigCB(
Widget,
XtPointer,
XtPointer
		    );

extern void SendStartGetStateCB(
Widget,
XtPointer,
XtPointer
		    );

extern void SendConnectCB(
Widget,
XtPointer,
XtPointer
       	    );



extern void ReceiveLeaveRoomRCB(
Widget,
XtPointer,
WSMReply*,
WSMErrorCode
				);




extern void ReceiveSaveWsmRCB(
Widget,
XtPointer,
WSMReply*,
WSMErrorCode
				);

extern void ReceiveEnterRoomRCB(
Widget,
XtPointer,
WSMReply*,
WSMErrorCode
				);

extern void ReceiveConnectRCB(
Widget,
XtPointer,
WSMReply*,
WSMErrorCode
				);

extern void ReceiveExtensionsRCB(
Widget,
XtPointer,
WSMReply*,
WSMErrorCode
				);


extern void ReceiveConfigRCB(
Widget,
XtPointer,
WSMReply*,
WSMErrorCode
				);

extern void ReceiveSetStateRCB(
Widget,
XtPointer,
WSMReply*,
WSMErrorCode
				);

extern void SendStartSetStateRCB(
Widget,
XtPointer,
WSMReply*,
WSMErrorCode
				);

extern void CopyRCB(
Widget,
XtPointer,
WSMReply*,
WSMErrorCode
				);



extern void CopyWindow(
WorkWindow*,
Space*,
Space*
				);

extern void LinkRCB(
Widget,
XtPointer,
WSMReply*,
WSMErrorCode
				);




extern void LinkWindow(
WorkWindow*,
Space*,
Space*
				);


extern void DeleteWindow(
WorkWindow*,
Space*
				);

extern void MoveRCB(
Widget,
XtPointer,
WSMReply*,
WSMErrorCode
				);


extern void MoveWindow(
WorkWindow*,
Space*,
Space*
				);



extern void BackgroundRCB(
Widget,
XtPointer,
WSMReply*,
WSMErrorCode
				);
extern void SetSpaceBackground(
Space*
			  );



extern void QuitCB(
Widget,
XtPointer,
XtPointer
			 );

extern void
ManageAllWindowsAndExit();

extern void ExitRCB(
Widget,
XtPointer,
WSMReply*,
WSMErrorCode
				);


extern void CleanUpAndExit();


#endif
