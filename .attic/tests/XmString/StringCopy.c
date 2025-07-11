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
static char rcsid[] = "$XConsortium: StringCopy.c /main/7 1995/07/13 19:54:14 drk $"
#endif
#endif

#include <stdio.h>

#include <testlib.h>

#include "String.h"


#define	NUM_TESTS 3

extern void	error_proc();

char	*description_string[NUM_TESTS] =
{
    "Create a compound string, make a copy if it, and\n\
     compare the two strings with XmStringByteCompare",
    "Create a compound string, make a copy if it, and\n\
     compare the two strings with XmStringCompare    ",
    "Copy a NULL string to a non-NULL string         "
};

main(argc, argv)
unsigned int	argc;
char		*argv[];
{

    string_num = text_num = charset_num = test_num = 0;
    errors = 0;

    CommonTestInit(argc, argv);

/*
                                                       Copy     Compare
Test Description                                       Result   Result    Error
---- -----------                                       -------  --------- -----
*/

    fprintf(stdout, "Test Description                                       ");
    fprintf(stdout, "Result   Result    Error\n");
    fprintf(stdout, "---- -----------                                       ");
    fprintf(stdout, "-------  --------- -----\n");

/*
 * Test 1: Create a compound string, make a copy of it, and
 *         compare the two strings with XmStringByteCompare
 */

    if ((string[string_num] = XmStringCreateLtoR(text[text_num],
												 charset[charset_num])) 
												 == NULL) {
		sprintf(error_string, "Can't create string%d", string_num);
		error_proc(error_string);
    }

    string_num++;
    
    if ((string[string_num] = XmStringCopy(string[string_num-1])) == NULL) {
		sprintf(error_string, "Can't copy string%d to string%d", string_num-1,
				string_num);
		error_proc(error_string);
    }

    fprintf(stdout, "  %d  %s  success  ", test_num+1, 
			description_string[test_num]);
	
    if (!XmStringByteCompare(string[string_num - 1], string[string_num])) {
		errors++;
		fprintf(stdout, "different    *\n");
    }
    else
		fprintf(stdout, "identical\n");

    string_num++;
    text_num++;
    charset_num = charset_num+2;
    test_num++;
    
/*
 * Test 2: Create a compound string, make a copy of it, and
 *         compare the two strings with XmStringCompare
 */

    if ((string[string_num] = XmStringCreate(text[text_num],
					    					 charset[charset_num])) 
											 == NULL) {
		sprintf(error_string, "Can't create string%d", string_num);
		error_proc(error_string);
    }

    string_num++;
    
    if ((string[string_num] = XmStringCopy(string[string_num-1])) == NULL) {
		sprintf(error_string, "Can't copy string%d to string%d", string_num-1,
				string_num);
		error_proc(error_string);
    }
	
    fprintf(stdout, "\n  %d  %s  success  ", test_num+1,
	    	description_string[test_num]);
	
    if (!XmStringCompare(string[string_num - 1], string[string_num])) {
		errors++;
		fprintf(stdout, "mismatch     *\n");
    }
    else
		fprintf(stdout, "match\n");

    test_num++;
    
/*
 * Test 3: Copy a NULL string to a valid string
 */

    fprintf(stdout, "\n  %d  %s  ", test_num+1,
	    	description_string[test_num]);
	
    if ((string[string_num] = XmStringCopy(NULL)) != NULL) {
		errors++;
		fprintf(stdout, "nonNULL      *\n");
    }
    else
		fprintf(stdout, "NULL\n");
	
    summary_proc("StringCopy", errors);

    CommonPause();
    XtAppMainLoop( app_context );

}
