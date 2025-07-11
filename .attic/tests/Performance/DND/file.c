/* $XConsortium: file.c /main/5 1995/07/15 21:01:24 drk $ */
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


/*
 * These routines load in files.  Because toolchest files are small,
 * we just allocate a buffer for the whole file and read the files in to that.
 * Pointers to strings in the file don't need to be malloced, just point
 * to the actual data in the buffer, and replace the ending white space with
 * a NULL.  Since we are only doing a few mallocs, it is cheap to free them
 * all when we read the file.
 * Note mallocs should be done using tcMalloc, so that it can be later
 * freed if needed with tcFreeAll.
 */

#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#include "toolchest.h"

#ifndef PATH_MAX
#define PATH_MAX 128
#endif

/* information about a file buffer
 * Normally, datap will point at data.  However, if we do an include,
 * we modify datap to point at where we left off in the data
 */
struct fileInfo
{
    char *datap;		/* a pointer to the read in file data */
    char *name;			/* the name of the file (used for includes) */
    int lineno;			/* the next line # to read */
    struct fileInfo *next;	/* next included file */
    char data[1];		/* actually longer, includes all the data */
};

/* create in initial file structure to simplify the algorithms */
static struct fileInfo dummyFile =
{
    dummyFile.data,
    "toolchest",
    0,
    NULL,
    ""
};
static char *curCharP=dummyFile.data;	/* pointer to next character to read */

static struct fileInfo *curFileP = &dummyFile;
static struct fileInfo *lastFileP = &dummyFile;

static Boolean hadNewLine=FALSE;     /* if we nulled out a new line, save it*/


static struct fileInfo *processDir(char *name);


/* read a file into a dataInfo buffer.  It is presumed that we have
 * already statted the file and know its length.
 */
static struct fileInfo *
loadFile(char *name, off_t length)
{
    int fd;
    struct fileInfo *info;
    int ret;

    if ((fd = open(name, O_RDONLY)) < 0)
    {
	perror(name);
	return (NULL);
    }
    else
    {
	/* allocate the fileInfo structure.  Note that although we do
	 * write in a trailing NULL, we don't need to account for it in
	 * the malloc, as sizeof(struct fileInfo) already includes a byte
	 * for data because of the declaration above
	 */
	info = (struct fileInfo *)tcMalloc(sizeof(struct fileInfo)+length);
	info->datap = info->data;
	info->next = NULL;
	/* read in the data */
	if ((ret = read (fd, info->data, length)) != length)
	{
	    if (ret < 0)
	    {
		perror (name);
		close(fd);
		return (NULL);
	    }
	    else
	    {
		fprintf (stderr, "%s: premature EOF\n", name);
		close(fd);
		return (NULL);
	    }
	}
	/* add a trailing NULL */
	info->data[length] = '\0';
	/* copy in the name */
	info->name = (char *)tcMalloc(strlen(name)+1);
	strcpy (info->name, name);
	info->lineno = 0;
    }
    close(fd);
    return(info);
}

/* check if a file with a given name is valid.  If it is, load it and
 * return the fileInfo structure.  If it is a directory, recursively
 * process the directory.  If it isn't found, return NULL.
 */
static struct fileInfo *
checkProcessFile(char *name)
{
    struct fileInfo *info;
    struct stat statbuf;

    if (stat (name, &statbuf) < 0)
	return (NULL);
    if ((statbuf.st_mode & S_IFMT) == S_IFDIR)
    {
	return (processDir(name));
    }
    return (loadFile(name, statbuf.st_size));
}


/* Process a file name.  Thie routine is responsible for trying the
 * various valid file names given the name "name".  If name is given,
 * it performs appropriate Language specific lookups.  It also handles
 * leading ~.  If "name" is NULL, it checks the system and user default
 * chest files.  processFile actually uses checkProcessFile to determine
 * if the file actually exists and is readable.  If the file is found,
 * it is read into a fileInfo structure which is returned.  If no file is
 * found, this routine returns NULL.
 *
 * If this is an include, then includeName is set to the name of the file
 * that included this file, and is used for handeling relative pathnames.
 */

static struct fileInfo *
processFile (char *name, char *includeName)
{

    char LANG[32];
    char fileName[PATH_MAX+1];

    struct fileInfo *result;

    /*
     * Get the LANG environment variable
     */
    if(getenv ("LANG") == NULL)
    {
        strcpy(LANG, " ");
    }
    else
    {
	strcpy(LANG, getenv ("LANG"));
    }
    

    /*
     * A file name was passed in.
     * Interpret "~/.." as relative to the user's home directory.
     * Use the LANG variable if set and .chestrc is in $HOME/$LANG/.chestrc  
     */

    if ((name != NULL) && (*name != '\0'))
    /* pointer to nonNULL string */
    {
        if ((name[0] == '~') && (name[1] == '/'))
	/* handle "~/..." */
	{
	    strcpy(fileName, getenv ("HOME"));
	    if (LANG[0] != '\0')
	    {
		strncat(fileName, &(LANG[0]), PATH_MAX-strlen(fileName));
		strncat(fileName, "/", PATH_MAX-strlen(fileName));
	    }
	    strncat(fileName, &(name[1]), PATH_MAX-strlen(fileName));
	    if ((result = checkProcessFile(fileName)) != NULL)
		return (result);
	    else
	    {
		/* 
		 * Try it without $LANG
		 */
		strcpy(fileName, getenv ("HOME"));
		strncat(fileName, &(name[1]), 
			PATH_MAX-strlen(fileName));
		if ((result = checkProcessFile(fileName)) != NULL)
		    return (result);
	    }
	}
	else
	/* relative to current directory or absolute */
	{
	    /* If includeName is set this is an include.  If the pathname
	     * is relative, check relative to the including directory
	     */
	    if (includeName && name[0] != '/')
	    {
		register char *slash;
		
		strcpy (fileName, includeName);
		slash = strrchr(fileName, '/');
		if (slash)
		{
		    strcpy (slash+1, name);
		    if ((result = checkProcessFile(fileName)) != NULL)
			return (result);
		}
	    }
	    if ((result = checkProcessFile(name)) != NULL)
		return (result);
	}

	/* Fail if the specified file could not be opened */
	return (NULL);
    }

    /*
     * The name resource didn't do it for us.
     * First try HOME_CHESTRC, then try SYS_CHESTRC .
     */

    strcpy(fileName, getenv ("HOME"));
    if (LANG[0] != '\0')
    {
	strncat(fileName, "/", PATH_MAX-strlen(fileName));
	strncat(fileName, &(LANG[0]), PATH_MAX-strlen(fileName));
    }
    strncat(fileName, HOME_CHESTRC, PATH_MAX - strlen(fileName));
    if ((result = checkProcessFile(fileName)) != NULL)
	return (result);
    else
    {
	/* 
	 * Just try $HOME/.chestrc
	 */
	strcpy(fileName, getenv ("HOME"));
	strncat(fileName, HOME_CHESTRC, PATH_MAX - strlen(fileName));
	if ((result = checkProcessFile(fileName)) != NULL)
	    return (result);
    }


    /*
     * Try /usr/lib/X11/$LANG/system.chestrc
     */
    strcpy(fileName, "/usr/lib/X11/");
    if (LANG[0] != '\0')
    {
	strncat(fileName, &(LANG[0]), PATH_MAX-strlen(fileName));
    }
    strncat(fileName, "/system.chestrc", PATH_MAX - strlen(fileName));
    if ((result = checkProcessFile(fileName)) != NULL)
	return (result);
    if ((result = checkProcessFile(SYS_CHESTRC)) != NULL)
	return (result);
    else
    {
	return (NULL);
    }
}

/* a function for sorting alphabetically */
static int *compar(char **name1, char **name2)
{
  int intptr;

  intptr = strcmp(*name1, *name2);
  return &intptr;
}

/*
 * process a directory.  Look for files ending in CHEST_SUFFIX (.chest)
 * and process each of them.  Return a linked list of the fileinfos.
 * If no files are found in the directory, return NULL.
 */
static struct fileInfo *
processDir(char *name)
{
    DIR *dirf;
    register struct dirent *dentp;
    int numchests = 0;
    char **chestnames = NULL;
    char filename[PATH_MAX+1];
    int dirnamelen;
    register int i;
    struct fileInfo *curFile;
    struct fileInfo *firstFile = NULL;
    struct fileInfo *lastFile=NULL;
    int (*compar)();

    if((dirf = opendir(name)) == NULL)
	return;
    while((dentp = readdir(dirf)) != NULL)
    {
	if (dentp->d_ino == 0)
	    continue;
	if (strncmp(dentp->d_name+strlen(dentp->d_name)-CHEST_SUFFIX_LENGTH,
		    CHEST_SUFFIX, CHEST_SUFFIX_LENGTH) == 0)
	{
	    if (++numchests == 1)
		chestnames = (char **)XtMalloc(sizeof(char *));
	    else
		chestnames = (char **)XtRealloc((char *) chestnames,
						sizeof(char *)*numchests);
	    chestnames[numchests-1] = (char *)XtMalloc(strlen(dentp->d_name)+1);
	    strcpy(chestnames[numchests-1], dentp->d_name);
	}
    }
    closedir(dirf);
    qsort (chestnames, numchests, sizeof (char *), compar);
    strcpy (filename, name);
    dirnamelen = strlen(name);
    filename[dirnamelen++] = '/';
    for (i=0; i<numchests; i++)
    {
	strcpy (filename+dirnamelen, chestnames[i]);
	XtFree (chestnames[i]);
	curFile =  checkProcessFile(filename);
	if (curFile)
	{
	    if (lastFile)
	    {
		lastFile->next = curFile;
		lastFile = curFile;
	    }
	    else
	    {
		firstFile = lastFile = curFile;
	    }
	    /* checkProcessFile may have recursively traversed directories
	     * so if it returned a list, traverse to the end
	     */
	    while (lastFile->next)
		lastFile = lastFile->next;
	}
    }
    if (numchests)
	free(chestnames);
    return (firstFile);
}


/*
 * Append a file with the given name (or NULL to specify the default files).
 */

void
AppendFile(char *name)
{
    struct fileInfo *info;

    info = processFile (name,NULL);
    if (info)
    {
	lastFileP->next = info;
	lastFileP = info;
	if (!curFileP)
	    curFileP = info;
	/* we may have been reeturned a list from a directory, so traverse it*/
	while (lastFileP->next)
	    lastFileP = lastFileP->next;
    }
    else
	fprintf (stderr, "Cannot open configuration File %s\n", name?name:"");
}

/* Include a file at the current point.  We need to insert the list at
 * curFileP, and make the last file point to the existing current file
 */
void
IncludeFile(char *name, Boolean warnIfMissing)
{
    struct fileInfo *info;
    struct fileInfo *last;

    info = processFile (name,curFileP->name);
    if (info)
    {
	/* find the last item in the list*/
	last = info;
	while (last->next)
	    last = last->next;
	/* set last to point to the currentFile */
	last->next = curFileP;
	/* save the current file offset */
	curFileP->datap = curCharP;
	curFileP->lineno = lineno-1; /* we will throw in a new line when we
				      * switch back, so subtract one */
	/* set the current file to point to the first included file, and
	 * set the data pointer
	 */
	curFileP = info;
	curCharP = curFileP->data;
	lineno = 0;
    }
    else if (warnIfMissing)
    {
	FileError ("Cannot open Include File %s\n", name);
    }
}

/* reset pointers to the next file.  Return TRUE if there was one */
static Boolean
NextFile()
{
    if (curFileP->next)
    {
	curFileP = curFileP->next;
	lineno = curFileP->lineno;
	curCharP = curFileP->datap;
	return (TRUE);
    }
    return (FALSE);
}

/* return the next character from the file */
char
NextChar()
{
    if (hadNewLine)
    {
	hadNewLine = FALSE;
	return ('\n');
    }
    if (*curCharP)
	return (*curCharP++);
    if (NextFile())
    {
	/* between files, throw in a new line to guarantee that we terminate
	 * any strings
	 */
	return ('\n');
    }
    return ('\0');
}

/* undo the previous nextChar (similar to ungetc) */
void
PrevChar()
{
    curCharP--;
}

/* return a pointer to the character just gotten */
char *
SaveChar()
{
    return (curCharP);
}

/* Null out the character just gotten , remembering if it was a newline*/
void
NullChar()
{
    if (*(curCharP-1) == '\n')
	hadNewLine = TRUE;
    *(curCharP-1) = '\0';
}

void
FileError (char *msg, void *arg1, void *arg2, void *arg3)
{
    fprintf (stderr, "line %d in %s: ", lineno, curFileP->name);
    fprintf (stderr, msg, arg1, arg2, arg3);
}
