/* $XConsortium: wsmStruct.h /main/5 1995/07/15 20:46:41 drk $ */
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

#ifndef wsmStruct_h
#define wsmStruct_h

#include "wsm.h"

extern Space* CreateSpace(
XrmQuark,
char*
);



extern Boolean SetSpacePixel(
Space *,
char*
);


extern Boolean SpacePixelSet(
Space *
);



extern Boolean SetSpacePixmap(
Space *,
char*
);




extern Boolean SpacePixmapSet(
Space *
);



extern Boolean SetSpaceLabelPixmap(
Space *,
char*
);




extern Boolean SpaceLabelPixmapSet(
Space *
);


extern Boolean CreateInternalStructure(
WorkWindow*,
XrmQuark*
);


extern WorkWindow* CreateWorkWindow(
Window
);


extern WorkWindow* ReCreateWorkWindow(
Window
);



extern void SetWorkWindowValues(
WorkWindow*,
Boolean,
Boolean
);

extern Space* GetSpace(
XrmQuark
		       );



extern Space* GetSpaceFromName(
char*
		       );




extern int GetSpaceID(
Space*
		       );



extern int GetNumberSpaces();


extern WorkWindow* GetWorkWindowID(
Space*,
int
);


extern Space* GetSpaceFromID(
int
);


extern WorkWindow* GetWorkWindow(
Window
				 );


extern WorkWindow* GetWorkWindowClient(
Window,
WorkWindow***,
int*
				 );




extern Boolean GetWorkWindowClientIDs(
int,
Space*,
int**,
int*
				 );




extern WorkWindow* GetIconWorkWindow(
Window
				 );




extern void AddSpaceToWindow(
Space*,
WorkWindow*
				 );



extern void AddWindowToSpace(
Space*,
WorkWindow*
				 );


extern void RemoveWorkWindowFromSpace(
Space*,
WorkWindow*
				 );



extern void RemoveSpaceFromWindow(
Space*,
WorkWindow*
				 );



extern void RemoveWorkWindow(
WorkWindow*,
Boolean
				 );



extern void RemoveSpace(
Space*
				 );



extern Boolean IsWorkWindowInSpace(
WorkWindow*,
Space*
				 );



extern Boolean IsSpaceInWorkWindow(
Space*,
WorkWindow*
				 );


extern void InitializeWsm();

extern void CompleteRestartWSM();
extern void RestartWSM();
extern void FinishRestartWSM();

extern Window GetWSMWindow(
WorkWindow*
			   );

extern void MapWorkWindow(
WorkWindow*
			  );

extern void UnmapWorkWindow(
WorkWindow*
			    );




extern void UnmapCurrentSpace(
			    );

#endif
