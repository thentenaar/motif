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
/*   $XConsortium: app.h /main/6 1995/07/14 09:43:32 drk $ */

/************************************************************
 *     app.h -- toolkit-independent code
 ************************************************************/

#ifndef _app_h
#define _app_h


extern char *AppBufferName();

extern char *AppReadFile( );
extern void AppSaveFile(char *textchars, size_t numchars);
extern void AppTransferFile(char *textchars, size_t numchars);

extern void AppNewFile( );
extern AppRemoveFile();

extern AppOpenReadFile(char *filnam);
extern AppOpenSaveFile( );
extern AppOpenTransferFile(char *filnam);

extern void AppCompleteSaveAsFile( );
extern void AppCompleteCopyFile( );
extern AppCompleteMoveFile();


#endif /* _app_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
