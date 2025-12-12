/* $XConsortium: wsm_create.h /main/5 1995/07/15 20:46:50 drk $ */
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
#ifndef wsmCreate_h
#define wsmCreate_h


#include "wsm_ui.h"

extern Widget CreateNameWorkspace(
Widget,
WSM_UI*
				);



extern Widget CreateBackgroundWorkspace(
Widget,
WSM_UI*
				);




extern Widget CreateDeleteWorkspace(
Widget,
WSM_UI*
				);


extern Widget CreateWorkspace(
Widget,
WSM_UI*
				);


extern Widget CreateWorkspace(
Widget,
WSM_UI*
				);


extern Widget CreateConfigureWorkspace(
Widget,
WSM_UI*
				);


extern Widget CreateWorkspacePanelBX(
Widget,
WSM_UI*,
Boolean
				);



extern Widget CreateSaveAs(
Widget,
WSM_UI*
				);



extern Widget CreateOccupyWorkspace(
Widget,
WSM_UI*
				);


#endif
