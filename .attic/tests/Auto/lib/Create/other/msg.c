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
static char rcsid[] = "$XConsortium: msg.c /main/7 1995/07/14 11:18:29 drk $"
#endif
#endif
/***********************************************************************
  	@(#)msg.c	1.16.1.1	Date:1/22/91
  	Author: PJL
	History:
           03/30/90 SJS Adding to sccs tree.
           04/13/90 SJS add msg_failure
           07/11/90 SJS add __STDC__ test for varargs
           07/20/90 SJS change format somewhat
	Calls:

	Summary:
		This file contains functions that are used to print
	out messages. The base function is called process_msg() and can
	only be called by the other functions in this file.
************************************************************************/

#include <stdio.h>

#ifdef __STDC__
#include <stdarg.h>
#else
#include <varargs.h>
#endif

#include <string.h>
#include <ctype.h>
#include "mvslib.h"

#define MAX_ARGS 30

/* This function writes out messages. It can NOT be called directly
   by any function outside of this module (static declaration).
   One of the other message routines must be used in order to control
   the level of messaging. The routines that call this function must
   determine if the user has requested the level of detail needed to
   print the message.
*/

static void process_msg( parse, ap )
char parse[];
va_list ap;
{
    char *args [MAX_ARGS], *allargs [MAX_ARGS];
    int  *intargs [MAX_ARGS];
    long *longargs [MAX_ARGS];
    char *chargs [MAX_ARGS];
    int argno = 0, allargno = 0, i;
    int intargno = 0;
    int longargno = 0;
    int chargno = 0;
    int type_found;

    for (i=0; i<MAX_ARGS; i++) {
        allargs [i] = (char *) XtMalloc(1 * MAX_STRING_LENGTH);
	*allargs [i] = '\0';
    }

    for (i=0; i<strlen (parse); i++) {
        if (parse [i] == '%') {
            type_found = FALSE;
            while(!type_found && (i<strlen (parse))) {
                switch (parse [++i]) {
                case 'd':
#if !defined(__STDC__)
		      if (ap != NULL) 
#endif
		      {
	                 intargs[intargno] = (int *)va_arg(ap, int *);
		         sprintf(allargs [allargno ++], 
				"%d", intargs[intargno++]);
		         parse [i] = 's';
		      }
                      type_found = TRUE;
		      break;

	        case 's':
#if !defined(__STDC__)
		      if (ap != NULL) 
#endif
		      {
	       	          args[argno] = (char *)va_arg(ap, char *); 
		          if (args [argno] != (char *) NULL)
                             strcpy (allargs [allargno], args [argno]);
		          allargno ++; argno++;
		      }
                      type_found = TRUE;
		      break;

                case 'l':
#if !defined(__STDC__)
		      if (ap != NULL) 
#endif
		      {
	                 longargs[longargno] = (long *)va_arg(ap, long *);
		         sprintf (allargs [allargno ++], "%l", 
			          longargs [longargno ++]);
		         parse [i] = 's';
		      }
                      type_found = TRUE;
		      break;

                case 'c':
#if !defined(__STDC__)
		      if (ap != NULL) 
#endif
		      {
	                 *chargs[chargno] = (char)va_arg(ap, char);
		         sprintf (allargs [allargno ++], "%c", 
			          chargs [chargno ++]);
		      }
                      type_found = TRUE;
		      break;

                default: /* trust them to be using a modifier */
		      break;
                } /* End switch() */
            } /* End while() */
        }
    } /* End for(i<strlen) */
    va_end(ap);

    /* This printf should be done a better way */
    printf (parse, allargs [0], allargs [1], allargs [2], 
	    allargs [3], allargs [4], allargs [5], allargs [6],
	    allargs [7], allargs [8], allargs [9]);
    fflush (stdout);

    for (i=0; i<MAX_ARGS; i++)
        XtFree( (char *) allargs[i]);

} /* End process_msg() */


/* This function is used to print out messages that have been classified
   as being "Failures." Messages are only printed if the level of
   detail has been requested. "Failure" implies that some part of the
   implementation did not act the way the AES has specified. Thus, it is
   also necessary to give the Assertion that has been broken.
*/

#ifdef __STDC__
void msg_failure ( char *parse2, ... )
#else
void msg_failure ( va_alist )
 va_dcl
#endif
{
    va_list ap;
    char parse[MAX_STRING_LENGTH];

#ifndef __STDC__
    char *parse2;
#endif

    if(mvsDetailLevel & FAILURE) { 
        strcpy(parse, "    FAILURE **: ");

#ifdef __STDC__
        va_start (ap, parse2);
#else
        va_start (ap);
        parse2 = va_arg (ap, char *);
#endif
        strcat(parse,parse2);
        mvsFailAssert();
        process_msg(parse, ap);
    }
    return;
} /* End msg_failure() */


/* This function is used to print out messages that have been classified
   as being "Errors." Messages are only printed if the level of
   detail has been requested.
*/

#ifdef __STDC__
void msg_error ( char *parse2, ... )
#else
void msg_error ( va_alist )
 va_dcl
#endif
{
    va_list ap;
    char parse[MAX_STRING_LENGTH];

#ifndef __STDC__
    char *parse2;
#endif
    *parse = '\0';

    if(mvsDetailLevel & ERROR) { 
        strcpy(parse, "    ERROR **: ");

#ifdef __STDC__
        va_start (ap, parse2);
#else
        va_start (ap);
        parse2 = va_arg (ap, char *);
#endif
        strcat(parse,parse2);
        process_msg(parse, ap);
    }
    return;
} /* End msg_error() */


/* This function is used to print out messages that have been classified
   as being "Informational." Messages are only printed if the level of
   detail has been requested.
*/

#ifdef __STDC__
void msg_info ( char *parse2, ... )
#else
void msg_info ( va_alist )
 va_dcl
#endif
{
    va_list ap;
    char parse[MAX_STRING_LENGTH];

#ifndef __STDC__
    char *parse2;
#endif

    if(mvsDetailLevel & INFO) { 
        strcpy(parse, "    INFO: ");

#ifdef __STDC__
        va_start (ap, parse2);
#else
        va_start (ap);
        parse2 = va_arg (ap, char *);
#endif
        strcat(parse,parse2);
        process_msg(parse, ap);
    }
    return;

} /* End msg_info() */


/* This function is used to print out messages that have been classified
   as being "Warnings." Messages are only printed if the level of
   detail has been requested.
*/

#ifdef __STDC__
void msg_warning ( char *parse2, ... )
#else
void msg_warning ( va_alist )
 va_dcl
#endif
{
    va_list ap;
    char parse[MAX_STRING_LENGTH];

#ifndef __STDC__
    char *parse2;
#endif

    if(mvsDetailLevel & WARNING) {
        strcpy(parse, "    WARNING **: ");

#ifdef __STDC__
        va_start (ap, parse2);
#else
        va_start (ap);
        parse2 = va_arg (ap, char *);
#endif
        strcat(parse,parse2);
        process_msg(parse, ap);
    }
    return;

} /* End msg_warning() */


/* This function is used to print out messages that have been classified
   as being "Trace." Messages are only printed if the level of
   detail has been requested.
*/

#ifdef __STDC__
void msg_trace ( char *parse2, ...)
#else
void msg_trace ( va_alist )
 va_dcl
#endif
{
    va_list ap;
    char parse[MAX_STRING_LENGTH];

#ifndef __STDC__
    char *parse2;
#endif

    if(mvsDetailLevel & TRACE) {
        strcpy(parse, "        TRACE: ");

#ifdef __STDC__
        va_start (ap, parse2);
#else
        va_start (ap);
        parse2 = va_arg (ap, char *);
#endif
        strcat(parse,parse2);
        process_msg(parse, ap);
    }
    return;

} /* End msg_trace() */


/* This function is used to print out any messages that are passed to it.
*/

#ifdef __STDC__
void msg ( char *parse2, ... )
#else
void msg ( va_alist )
   va_dcl
#endif
{
    va_list ap;
    char parse[MAX_STRING_LENGTH];

#ifdef __STDC__
    va_start (ap, parse2);
#else
    char *parse2;

    va_start (ap);
    parse2 = va_arg (ap, char *);
#endif
    strcpy(parse,parse2);
    process_msg(parse, ap);
    return;
} /* End msg() */
