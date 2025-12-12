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
#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$XConsortium: file.c /main/7 1996/10/29 14:29:18 drk $"
#endif
#endif

/************************************************************
 *     file.c -- Code for dealing with files and filenames
 *
 *  Contains code to read, write, copy, move & remove files
 *
 ************************************************************/

#include <limits.h>
#include <stdio.h>

#include "basic.h"

/************************************************************
 * Remove File
 ************************************************************/


FileRemove(char *filnam)
{
    return -1;
}

/************************************************************
 * Save Text to File
 ************************************************************/

void
FileSaveText(FILE *fil, char *textchars, size_t numchars)
{
  size_t wrote;

    rewind(fil);
  wrote = fwrite(textchars, 1, numchars, fil);
    fflush(fil);
}

/************************************************************
 * Read Text from File
 ************************************************************/

char *
FileGetText(FILE *fil)
{
    char *textchars;
    int position = 0;
    int num;

    textchars = BasicMalloc( BUFSIZ );
    rewind( fil );
    while ( (num = read( fileno(fil),
                         textchars+position, BUFSIZ)) > 0 ) {
      position += num;
      textchars = BasicRealloc( textchars, position+BUFSIZ );
    }
    *( textchars+position ) = 0;
    return textchars;
}

/************************************************************
 * Return Trailing part of current filename
 ************************************************************/

char *
FileTrailingPart(char *filnam)
{
    char *trailnam;
    while (*filnam != '\0') filnam++;
    while (*filnam != '/') filnam--;
    filnam++;
    strdup( trailnam, filnam );
    return trailnam;
}

/************************************************************
 * Save txt in temporary file
 ************************************************************/

char *
FileSaveTemp(char *txt)
{
  FILE *temp;
  static char tmp_nam[L_tmpnam + 1];

  (void) tmpnam(tmp_nam);

  temp = fopen(tmp_nam, "w");
  fprintf(temp, "%s", txt);
  fclose(temp);
  return(tmp_nam);
}

