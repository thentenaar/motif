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
static char rcsid[] = "$XConsortium: FontListC.c /main/7 1995/07/13 19:52:48 drk $"
#endif
#endif

#include <stdio.h>
#include <testlib.h>

#include "FontList.h"

#define	NUM_TESTS 5

extern void	error_proc();

String		expected_result[NUM_TESTS] =
{
    "FAILURE",
    "SUCCESS",
    "FAILURE",
    "SUCCESS",
    "SUCCESS"
};

main(argc, argv)
     unsigned int	argc;
     char		*argv[];
{
    
    fontlist_num = font_num = charset_num = 0;
    errors = 0;
    
    CommonTestInit(argc, argv);

/*
 * Load valid fonts into font structures
 */

    for(font_num=1 ; font_num<NUM_FONTS ; font_num++) {

		if((font[font_num] = XLoadQueryFont(display, fontname[font_num]))
	   		== NULL) {
	    	sprintf(error_string, "Can't load font \"%s\"",
		    		fontname[font_num]);
	    	error_proc("FontListC", error_string);
		}

    }

    font_num = 0;

    fprintf(stdout, "                                          Expected  ");
    fprintf(stdout, " Actual\n");
    fprintf(stdout, "Test  Font      Character Set              Result   ");
    fprintf(stdout, " Result  Error\n");
    fprintf(stdout, "----  --------  ------------------------  --------  ");
    fprintf(stdout, "-------  -----\n");
    fflush(stdout);
   
/*
                                          Expected   Actual
Test  Font      Character Set              Result    Result  Error
----  --------  ------------------------  --------  -------  -----
*/

/*************************************************************
 * Test 1: Create a font list with an invalid font structure *
 *************************************************************/

    fprintf(stdout, "  %1d   %8s  %24s   %7s", fontlist_num+1,
	    	fontname[font_num], charsetname[charset_num],
	    	expected_result[fontlist_num]);
    fflush(stdout);

/*
 * Create a font list
 */

    if ((fontlist[fontlist_num++] = XmFontListCreate(font[font_num++],
						    						 charset[charset_num]))
       												 == NULL)
		fprintf(stdout, "  FAILURE\n");
    else {
		errors++;
		fprintf(stdout, "  SUCCESS    *\n");
    }
    
/**********************************************************
 * Test 2: Create a font list with a valid font structure *
 **********************************************************/

    fprintf(stdout, "  %1d   %8s  %24s   %7s", fontlist_num+1,
	    	fontname[font_num], charsetname[charset_num],
	    	expected_result[fontlist_num]);
    fflush(stdout);

/*
 * Create a font list
 */

    if ((fontlist[fontlist_num++] = XmFontListCreate(font[font_num],
						    						 charset[charset_num++]))
       												 == NULL) {
		errors++;
		fprintf(stdout, "  FAILURE    *\n");
    }
    else
		fprintf(stdout, "  SUCCESS\n");
    
/************************************************************
 * Test 3: Create a font list with an invalid character set *
 ************************************************************/

    fprintf(stdout, "  %1d   %8s  %24s   %7s", fontlist_num+1,
	    	fontname[font_num], charsetname[charset_num],
	    	expected_result[fontlist_num]);
    fflush(stdout);

/*
 * Create a font list
 */

    if ((fontlist[fontlist_num++] = XmFontListCreate(font[font_num++],
						    						 charset[charset_num++]))
       												 == NULL)
		fprintf(stdout, "  FAILURE\n");
    else {
		errors++;
		fprintf(stdout, "  SUCCESS    *\n");
    }

/*********************************************************
 * Test 4: Create a font list with a valid character set *
 *********************************************************/

    fprintf(stdout, "  %1d   %8s  %24s   %7s", fontlist_num+1,
	    	fontname[font_num], charsetname[charset_num],
	    	expected_result[fontlist_num]);
    fflush(stdout);

/*
 * Create a font list
 */

    if ((fontlist[fontlist_num++] = XmFontListCreate(font[font_num++],
						    						 charset[charset_num++]))
       												 == NULL) {
		errors++;
		fprintf(stdout, "  FAILURE    *\n");
    }
    else
		fprintf(stdout, "  SUCCESS\n");

/***********************************************
 * Test 5: Create a font list with a character *
 *         set defined in an app-defaults file *
 ***********************************************/

    fprintf(stdout, "  %1d   %8s  %24s   %7s", fontlist_num+1,
	    	fontname[font_num], charsetname[charset_num],
	    	expected_result[fontlist_num]);
    fflush(stdout);

/*
 * Create a font list
 */

    if ((fontlist[fontlist_num++] = XmFontListCreate(font[font_num],
						    						 charset[charset_num]))
       												 == NULL) {
		errors++;
		fprintf(stdout, "  FAILURE    *\n");
    }
    else
		fprintf(stdout, "  SUCCESS\n");

    summary_proc("FontListC", errors);

    CommonPause();
    XtAppMainLoop( app_context );

}
