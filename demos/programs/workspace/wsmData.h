/* $XConsortium: wsmData.h /main/5 1995/07/15 20:46:28 drk $ */
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

#ifndef wsmData_h
#define wsmData_h

/* i use the following code to switch between XrmValues and WSMWinData structures */


extern void UpdateXrm();

extern Boolean CreateWSMWinData(
XrmValue*,
Boolean,
WorkWindow*,
WSMWinData**,
int*
);




extern Boolean CreateStartWSMWinData(
XrmValue*,
Boolean,
WSMWinData*,
int,
WorkWindow*,
WSMWinData**,
int*
);



extern void	CreateStartHideWSMWinData(
Boolean,
WSMWinData *,
int,
WorkWindow*,
WSMWinData**,
int*
);



extern void	CreateHideWSMWinData(
Boolean,
WorkWindow*,
WSMWinData**,
int*
);


extern void	CreateUnhideWSMWinData(
XrmValue *,
WorkWindow*,
WSMWinData**,
int*
);

extern
Boolean
CreateValues(
WorkWindow *,
WSMWinData *data_list,
int num_data_list,
XrmValue **value_list_return
);



extern
void
FreeValues(
int num_values,
XrmValue *value_list
);

extern
void UpdateWinData(
WSMWinData*,
int,
WSMWinData*,
int
			);


#endif
