/* $XConsortium: command_ui.h /main/5 1995/07/15 20:46:19 drk $ */
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
#ifndef wsmCmd_h
#define wsmCmd_h


#include "wsm.h"

typedef struct _CommandSpace {
Space *space;
CARD32 copy_command_id;
CARD32 link_command_id;
CARD32 switch_command_id;
struct _CommandSpace *next;
} CommandSpace;

extern void SetInitialCommands(
void
);

extern void ShowWsmCommand(
void
);

extern void AddSpaceCommand(
Space *s
	);


extern void AddWindow(
Window
	);


extern void InvokeCommand(
Widget,
Atom,
MessageData,
unsigned long,
int
);

extern void RemoveSpaceCommand(
int i
	);



extern void ChangeSpaceCommandName(
Space*
	);


extern void InternStuff(
Display *dsp
	);

extern void DisableDeleteCommand(
Window
				 );



extern void EnableDeleteCommand(
Window
				 );

extern void RemoveAllCommandsAndExit();

#endif
