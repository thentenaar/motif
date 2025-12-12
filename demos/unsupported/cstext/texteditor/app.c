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
static char rcsid[] = "$XConsortium: app.c /main/6 1995/07/14 09:43:21 drk $"
#endif
#endif

/************************************************************
 *     app.c -- toolkit-independent code
 *
 *  Contains code to read, write, copy, move & remove files
 *
 ************************************************************/

#include <limits.h>
#include <stdio.h>

#include "basic.h"
#include "file.h"

  /* curfile is the file currently open
     newfile is a newly named file */

static FILE *curfile;
static FILE *newfile;

static char curname[256];
static char newname[256];

  /* open_to_read and open_to_write indicate whether
       curfile is open for reading or writing
     open_to_transfer indicates whether newfile
       is open for writing */

static int open_to_read = false;
static int open_to_write = false;
static int open_to_transfer = false;

/*===========================================================
      Public transfer operations, guarded by state flags
============================================================*/

/************************************************************
 * Read File
 ************************************************************/

char *AppReadFile()
{
    if (! open_to_read) return NULL;
    return FileGetText( curfile );
}

/************************************************************
 * Save File
 ************************************************************/

void
  AppSaveFile(char *textchars, size_t numchars)
{
    if (! open_to_write ) return;
    FileSaveText( curfile, textchars, numchars );
}

/************************************************************
 * Transfer File
 ************************************************************/

void
AppTransferFile(char *textchars, size_t numchars)
{
    if (! open_to_transfer ) return;
    FileSaveText( newfile, textchars, numchars );
}

/************************************************************
 * New file
 ************************************************************/

void AppNewFile()
{
    if ( open_to_read || open_to_write )
    {
        fclose( curfile );
	open_to_read = false;
	open_to_write = false;
    };
}

/************************************************************
 * Remove File
 ************************************************************/

AppRemoveFile()
{
    if ( ! (open_to_read || open_to_write) ) return false;
    AppNewFile();
    return ( FileRemove(curname) == 0 );
}

/************************************************************
 * Get Buffer Name of Current File
 ************************************************************/

char *AppBufferName()
{
    if ( open_to_read || open_to_write )
        return FileTrailingPart( curname );
    else
        return NULL;
}

/*===========================================================
           Initiate and Complete various operations
============================================================*/

/************************************************************
 * Open file for reading
 ************************************************************/


AppOpenReadFile(char *filnam)
{
    if ( (newfile = fopen(filnam,"r")) != NULL )
    {
        AppNewFile();
        strcpy( curname, filnam );
        curfile = newfile;
        open_to_read = true;
        return ( true );
    }
    else
        return ( false );
}

/************************************************************
 * Reopen current file for saving
 ************************************************************/

AppOpenSaveFile()
{
    if ( open_to_write ) return true;
    if (! open_to_read ) return false;
    if ((open_to_write = ((newfile = fopen(curname,"w+")) != NULL)))
    {
        fclose( curfile );
	curfile = newfile;
        return( true );
    }
    else
        return ( false );
}

/************************************************************
 * Open new file for transfer
 ************************************************************/


AppOpenTransferFile(char *filnam)
{
    strcpy( newname, filnam );
    return ( open_to_transfer =
             ( (newfile = fopen(newname,"w+")) != NULL ) );
}

/************************************************************
 * Complete Save As operation
 ************************************************************/

void AppCompleteSaveAsFile()
{
    AppNewFile();
    open_to_transfer = false;
    open_to_write = true;
    curfile = newfile;
    strcpy( curname, newname );
}

/************************************************************
 * Complete Move operation
 ************************************************************/

AppCompleteMoveFile()
{
    int retval;
    if ( ! open_to_transfer ) return false;
    retval = AppRemoveFile();
    open_to_transfer = false;
    open_to_write = true;
    curfile = newfile;
    strcpy( curname, newname );
    return retval;
}

/************************************************************
 * Complete Copy operation
 ************************************************************/

void AppCompleteCopyFile()
{
    open_to_transfer = false;
    fclose( newfile );
}
