/* $XConsortium: wsm_cb.h /main/5 1995/07/15 20:46:46 drk $ */
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
#ifndef wsmCB_h
#define wsmCB_h

#include "wsm_ui.h"

extern void InitializeInterface(
				WSM_UI*
				);




extern void HideWsm();

extern void ShowWsm();

extern Widget CreateWorkspacePanel(
				   Widget,
				   WSM_UI*,
				   Boolean
				   );




extern void CreateNewSpaceButton(
				 int,
				 char*,
				 WSM_UI*
				 );



extern void SendLeaveRoomCB(
			    Widget,
			    XtPointer,
			    XtPointer
			    );





extern void NewActivateCB(
Widget,
XtPointer,
XtPointer
);


extern void HideActivateCB(
Widget,
XtPointer,
XtPointer
);


extern void SaveActivateCB(
Widget,
XtPointer,
XtPointer
);


extern void ExitCB(
Widget,
XtPointer,
XtPointer
);

extern void CreateFromOptionButton(
int,
char*
);



extern void CreateToOptionButton(
int,
char*
);

extern void UpdateList(
Widget,
Space*
);

extern void UpdateBothList(
Space*
);

extern void CreateConfigureCB(
Widget,
XtPointer,
XtPointer
);

extern void MoveCB(
Widget,
XtPointer,
XtPointer
);


extern void DeleteCB(
Widget,
XtPointer,
XtPointer
);


extern void ToWorkspaceCB(
Widget,
XtPointer,
XtPointer
);

extern void FromWorkspaceCB(
Widget,
XtPointer,
XtPointer
);

extern void Copy(
WSM_UI*
);


extern void Link(
WSM_UI*
);

extern void OccupyCB(
Widget,
XtPointer,
XtPointer
);


extern void SelectFromListCB(
Widget,
XtPointer,
XtPointer
);



extern void MultSelectFromListCB(
Widget,
XtPointer,
XtPointer
);




extern void ClientModeCB(
Widget,
XtPointer,
XtPointer
);


extern void WindowModeCB(
Widget,
XtPointer,
XtPointer
);



extern void SelectToListCB(
Widget,
XtPointer,
XtPointer
);

extern void DismissConfigureCB(
Widget,
XtPointer,
XtPointer
);


extern void CreateNameCB(
Widget,
XtPointer,
XtPointer
);


extern void UpdateSpaceList(
Widget
);


extern void SelectNameSpaceCB(
Widget,
XtPointer,
XtPointer
);


extern void ChangeSpaceName(
WSM_UI*,
Space*,
int
);


extern void NameActivateCB(
Widget,
XtPointer,
XtPointer
);



extern void PixmapActivateCB(
Widget,
XtPointer,
XtPointer
);


extern void NameOkActivateCB(
Widget,
XtPointer,
XtPointer
);


extern void DismissNameCB(
Widget,
XtPointer,
XtPointer
);




extern void CreateBackgroundCB(
Widget,
XtPointer,
XtPointer
);


extern void SelectBackgroundSpaceCB(
Widget,
XtPointer,
XtPointer
);


extern void BackgroundActivateCB(
Widget,
XtPointer,
XtPointer
);


extern void DismissBackgroundCB(
Widget,
XtPointer,
XtPointer
);


extern void CreateDeleteCB(
Widget,
XtPointer,
XtPointer
);


extern void DismissDeleteCB(
Widget,
XtPointer,
XtPointer
);


extern void DeleteActivateCB(
Widget,
XtPointer,
XtPointer
);



extern void SelectDeleteCB(
Widget,
XtPointer,
XtPointer
);


extern void SaveAsCB(
Widget,
XtPointer,
XtPointer
);


extern void SaveAsOkCB(
Widget,
XtPointer,
XtPointer
);


extern void DismissSaveAsCB(
Widget,
XtPointer,
XtPointer
);





extern void CreateOccupy(
WorkWindow*
);


extern void DismissOccupyCB(
Widget,
XtPointer,
XtPointer
);


extern void OccupyActivateCB(
Widget,
XtPointer,
XtPointer
);



extern void CopyModeCB(
Widget,
XtPointer,
XtPointer
);


extern void LinkModeCB(
Widget,
XtPointer,
XtPointer
);

extern void
UpdateButtons(WorkWindow *w_window);

#endif
