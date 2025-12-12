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
/*   $XConsortium: file.h /main/6 1995/07/14 09:44:23 drk $ */

/************************************************************
 *     file.h -- dealing with files and filenames
 ************************************************************/

#ifndef _file_h
#define _file_h

#include <stdio.h>


extern FileRemove(char *filnam);
extern void FileSaveText(FILE *fil, char *textchars, size_t numchars);
extern char *FileGetText(FILE *fil);
extern char *FileTrailingPart(char *filnam);
extern char *FileSaveTemp(char *txt);


#endif /* _file_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
