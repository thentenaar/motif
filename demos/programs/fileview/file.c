/*
 * Motif
 *
 * Copyright (c) 1987-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
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
#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$XConsortium: file.c /main/6 1995/07/14 10:02:30 drk $"
#endif
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "file.h"

/* =====================================================================
 * Open File
 */

FILE * OpenFile(char *path)
{
   return  fopen(path, "r");
}

/* =====================================================================
 * Close File
 */

void CloseFile(FILE * file)
{
   fclose (file);
}

/* =====================================================================
 * Read File in buffer
 */

char * ReadFile(FILE * file, int *filesize)
{
   char * buffer;

   fseek(file, 0L, SEEK_END);
   * filesize = ftell(file);
/*   fgetpos(file, (fpos_t *) filesize);*/
   rewind(file);
   buffer = (char *) XtMalloc(*filesize+1);
   if (fread(buffer, 1, *filesize, file) == *filesize ) {
      buffer[*filesize] = '\0';
      return buffer;
   }
   XtFree(buffer);
   return NULL;
}
