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
static char rcsid[] = "$XConsortium: StringEmpty.c /main/7 1995/07/13 19:55:40 drk $"
#endif
#endif

#include <stdio.h>

#include <testlib.h>

#include "String.h"

#define	NUM_TESTS 4

extern void	error_proc();

char	*description_string[NUM_TESTS] =
{
    "Test a string with a nonempty string component",
    "Test a string consisting of just a direction  ",
    "Test a string consisting of just a separator  ",
    "Test a NULL string                            "
};


main(argc, argv)
unsigned int	argc;
char		*argv[];
{

    string_num = text_num = charset_num = 0;
    errors = 0;

    CommonTestInit(argc, argv);

/*
                                                      XmStringEmpty
Test  Description                                     Expect   Actual    Error
----  -----------                                     -------- --------  -----
  1   Test a string with a nonempty string component  nonempty
  2   Test a string consisting of just a direction    empty
  3   Test a string consisting of just a separator    empty
  4   Test a NULL string                              empty
*/

    fprintf(stdout, "                                                      ");
    fprintf(stdout, "XmStringEmpty\n");
    fprintf(stdout, "Test  Description                                     ");
    fprintf(stdout, "Expect   Actual    Error\n");
    fprintf(stdout, "----  -----------                                     ");
    fprintf(stdout, "-------- --------  -----\n");

/*
 * Test 1: Test a compound string with a nonempty text component
 *         
 */

    if ((string[string_num] = XmStringCreateLtoR(text[text_num],
												 charset[charset_num])) 
												 == NULL) {
		sprintf(error_string, "Can't create string%d", string_num);
		error_proc(error_string);
    }

    fprintf(stdout, "  %d   %s  nonempty ", test_num+1,
	    	description_string[test_num]);
    
    if (XmStringEmpty(string[string_num])) {
		errors++;
		fprintf(stdout, "empty       *\n");
    }
    else
		fprintf(stdout, "nonempty\n");

    string_num++;
    test_num++;
    
/*
 * Test 2: Test a string consisting of just a direction
 */

    if ((string[string_num] = 
		 XmStringDirectionCreate(XmSTRING_DIRECTION_L_TO_R)) == NULL) {
		sprintf(error_string, "Can't create string%d", string_num);
		error_proc(error_string);
    }

    fprintf(stdout, "  %d   %s  empty    ", test_num+1,
	    	description_string[test_num]);
    
    if (!XmStringEmpty(string[string_num])) {
		errors++;
		fprintf(stdout, "nonempty    *\n");
    }
    else
		fprintf(stdout, "empty\n");

    string_num++;
    test_num++;
    
/*
 * Test 3: Test a string consisting of just a separator
 */

    if ((string[string_num] = XmStringSeparatorCreate()) == NULL) {
		sprintf(error_string, "Can't create string%d", string_num);
		error_proc(error_string);
    }

    fprintf(stdout, "  %d   %s  empty    ", test_num+1,
	    	description_string[test_num]);
    
    if (!XmStringEmpty(string[string_num])) {
		errors++;
		fprintf(stdout, "nonempty    *\n");
    }
    else
		fprintf(stdout, "empty\n");

    string_num++;
    test_num++;

/*
 * Test 4: Test a NULL string
 */

    fprintf(stdout, "  %d   %s  empty    ", test_num+1,
	    	description_string[test_num]);
    
    if (!XmStringEmpty(NULL)) {
		errors++;
		fprintf(stdout, "nonempty    *\n");
    }
    else
		fprintf(stdout, "empty\n");

    summary_proc("StringEmpty", errors);

    CommonPause();
    XtAppMainLoop( app_context );

}
