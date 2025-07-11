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
#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$XConsortium: StringGLR.c /main/7 1995/07/13 19:55:57 drk $"
#endif
#endif

#include <stdio.h>

#include <testlib.h>

#include "String.h"

#define	NUM_TESTS 9

extern void	error_proc();

char		*get_text = (char *)0;


main(argc, argv)
unsigned int	argc;
char		*argv[];
{
    int         i;

    string_num = text_num = charset_num = test_num = 0;
    errors = 0;

    CommonTestInit(argc, argv);

    fprintf(stdout, "\nThe following is a list of all compound strings");
    fprintf(stdout, " created for this\ntest, and the method/creation");
    fprintf(stdout, " routines used to create the strings.\n\n");

/*
String  Creation Routine          Text                 Character Set
------  ----------------          -------------------  -------------
*/

    fprintf(stdout, "String  Creation Routine          Text                 ");
    fprintf(stdout, "Character Set\n");
    fprintf(stdout, "------  ----------------          -------------------  ");
    fprintf(stdout, "-------------\n");
    
    fprintf(stdout, "   1    XmStringCreateLtoR        \"String3 here\"     ");
#ifndef MOTIF1_1
    fprintf(stdout, "  XmFONTLIST_DEFAULT_TAG  \n");
#else
    fprintf(stdout, "  XmSTRING_DEFAULT_CHARSET\n");
#endif
    fprintf(stdout, "   2    XmStringCreateLtoR        \"String5\"          ");
    fprintf(stdout, "  XmFALLBACK_CHARSET\n");
    fprintf(stdout, "   3    XmStringCreateLtoR        \"This is the text\" ");
    fprintf(stdout, "  MyCharset\n");
    fprintf(stdout, "\n");
    fprintf(stdout, "   4    XmStringConcat(string2, string1)\n");
    fprintf(stdout, "   5    XmStringConcat(string3, string2)\n");
    fprintf(stdout, "   6    XmStringDirectionCreate");
    fprintf(stdout, "(XmSTRING_DIRECTION_L_TO_R)\n");
    fprintf(stdout, "   7    XmStringSeparatorCreate()\n");
    fprintf(stdout, "   8    (string8 = NULL)\n");

    fprintf(stdout, "\nEach row in the following table represents a call to");
    fprintf(stdout, " XmStringGetLtoR\nwith the given string and character ");
    fprintf(stdout, "set as arguments.  For each call,\nthe expected and");
    fprintf(stdout, " actual returns are given, as well as the test");
    fprintf(stdout, " segment\nreturned, where applicable.\n");
    
/*
                                       XmStringGetLtoR
                                       Expected Actual
Test String  Character Set             Return   Return  Text Return       Error
---- ------  ------------------------  -------- ------  ----------------  -----
*/

    fprintf(stdout, "                                       ");
    fprintf(stdout, "XmStringGetLtoR\n");
    fprintf(stdout, "                                       ");
    fprintf(stdout, "Expected Actual\n");
    fprintf(stdout, "Test String  Character Set             ");
    fprintf(stdout, "Return   Return  Text Return       Error\n");
    fprintf(stdout, "---- ------  ------------------------  ");
    fprintf(stdout, "-------- ------  ----------------  -----\n");
    
/*
 * Test 1: Create a compound string and get the text from it
 */

    text_num++;
    
    if ((string[string_num] = XmStringCreateLtoR(text[text_num],
												 charset[charset_num])) 
												 == NULL) {
		sprintf(error_string, "Can't create string%d", string_num);
		error_proc(error_string);
    }

    fprintf(stdout, " %2d     %1d    %24s  True     ", test_num+1,
	    	string_num+1, charsetname[charset_num]);

    if (!XmStringGetLtoR(string[string_num], charset[charset_num], &get_text)) {
		errors++;
		fprintf(stdout, "False                     *\n");
    }
    else {

		fprintf(stdout, "True    %16s  ", get_text);
		if(strcmp(get_text, text[text_num])) {
	    	errors++;
	    	fprintf(stdout, "  *\n");
		}
		else
	    	fprintf(stdout, "\n");
                if ( get_text )
                   XtFree( get_text );
                get_text = (char *)0;


    }

    string_num++;
    charset_num = charset_num + 2;
    test_num++;
    
/*
 * Test 2: Get the text from a string with the wrong character set
 */

    fprintf(stdout, " %2d     %1d    %24s  False    ", test_num+1,
	    	string_num, charsetname[charset_num]);

    if (XmStringGetLtoR(string[string_num-1], charset[charset_num], 
						&get_text)) {
		errors++;
		fprintf(stdout, "True    %16s    *\n", get_text);
                if ( get_text )
                   XtFree( get_text );
                get_text = (char *)0;
    }
    else
		fprintf(stdout, "False\n");

    text_num = text_num + 2;
    charset_num = charset_num + 2;
    test_num++;
    
/*
 * Test 3: Create a compound string with the fallback
 *         charset and get it with the default charset
 */

    if ((string[string_num] = XmStringCreateLtoR(text[text_num],
												 charset[charset_num])) 
												 == NULL) {
		sprintf(error_string, "Can't create string%d", string_num);
		error_proc(error_string);
    }
    
    fprintf(stdout, " %2d     %1d    %24s  True     ", test_num+1,
	    	string_num+1, charsetname[0]);

    if (!XmStringGetLtoR(string[string_num], charset[0], &get_text)) {
		errors++;
		fprintf(stdout, "False                     *\n");
    }
    else {

		fprintf(stdout, "True    %16s  ", get_text);
		if(strcmp(get_text, text[text_num])) {
	    	errors++;
	    	fprintf(stdout, "  *\n");
		}
		else
	    	fprintf(stdout, "\n");
                if ( get_text )
                   XtFree( get_text );
                get_text = (char *)0;

    }

    text_num = text_num + 2;
    charset_num = charset_num - 1;
    string_num++;
    test_num++;
    
/*
 * Test 4: Create a compound string with a non-default
 *         charset and get it with the same charset
 */

    if ((string[string_num] = XmStringCreateLtoR(text[text_num],
												 charset[charset_num])) 
												 == NULL) {
		sprintf(error_string, "Can't create string%d", string_num);
		error_proc(error_string);
    }
    
    fprintf(stdout, " %2d     %1d    %24s  True     ", test_num+1,
	    	string_num+1, charsetname[charset_num]);

    if (!XmStringGetLtoR(string[string_num], charset[charset_num],
						 &get_text)) {
		errors++;
		fprintf(stdout, "False                     *\n");
    }
    else {

		fprintf(stdout, "True    %16s  ", get_text);
		if(strcmp(get_text, text[text_num])) {
	    	errors++;
	    	fprintf(stdout, "  *\n");
		}
		else
	    	fprintf(stdout, "\n");
                if ( get_text )
                   XtFree( get_text );
                get_text = (char *)0;

    }

    charset_num++;
    string_num++;
    test_num++;
    
/*
 * Test 5: Create a compound string with 2 text components where the first text
 *         component has the fallback charset and the second text component
 *         has the default charset, and get a string with the default charset.
 */

    if ((string[string_num] = XmStringConcat(string[string_num-2],
					    					 string[string_num-3])) 
											 == NULL) {
		sprintf(error_string, "Can't create string%d", string_num);
		error_proc(error_string);
    }
    
    fprintf(stdout, " %2d     %1d    %24s  True     ", test_num+1,
	    	string_num+1, charsetname[0]);

    if (!XmStringGetLtoR(string[string_num], charset[0], &get_text)) {
		errors++;
		fprintf(stdout, "False                     *\n");
    }
    else {

		fprintf(stdout, "True    %16s  ", get_text);
		if(strcmp(get_text, text[text_num-2])) {
	    	errors++;
	    	fprintf(stdout, "  *\n");
		}
		else
	    	fprintf(stdout, "\n");
                if ( get_text )
                   XtFree( get_text );
                get_text = (char *)0;

    }

    test_num++;

/*
 * Test 6: Take a compound string with 2 text components where the first text
 *         component has the fallback charset and the second text component
 *         has the default charset, and get a string with the fallback charset.
 */

    fprintf(stdout, " %2d     %1d    %24s  True     ", test_num+1,
	    	string_num+1, charsetname[charset_num]);

    if (!XmStringGetLtoR(string[string_num], charset[charset_num], 
						 &get_text)) {
		errors++;
		fprintf(stdout, "False                     *\n");
    }
    else {

		fprintf(stdout, "True    %16s  ", get_text);
		if(strcmp(get_text, text[text_num-2])) {
	    	errors++;
	    	fprintf(stdout, "  *\n");
		}
		else
	    	fprintf(stdout, "\n");
                if ( get_text )
                   XtFree( get_text );
                get_text = (char *)0;

    }

    string_num++;
    charset_num = charset_num - 1;
    test_num++;

/*
 * Test 7: Create a compound string with 2 text components where the first text
 *         component has a non-default charset and the second text component
 *         has the fallback charset, and get a string with the default charset.
 */

    if ((string[string_num] = XmStringConcat(string[string_num-2],
					    					 string[string_num-3])) 
											 == NULL) {
		sprintf(error_string, "Can't create string%d", string_num);
		error_proc(error_string);
    }
    
    fprintf(stdout, " %2d     %1d    %24s  True     ", test_num+1,
	    	string_num+1, charsetname[0]);

    if (!XmStringGetLtoR(string[string_num], charset[0], &get_text)) {
		errors++;
		fprintf(stdout, "False                     *\n");
    }
    else {

		fprintf(stdout, "True    %16s  ", get_text);
		if(strcmp(get_text, text[text_num-2])) {
	    	errors++;
	    	fprintf(stdout, "  *\n");
		}
		else
	    	fprintf(stdout, "\n");
                if ( get_text )
                   XtFree( get_text );
                get_text = (char *)0;

    }

    test_num++;

/*
 * Test 8: Take a compound string with 2 text components where the first text
 *         component has a non-default charset and the second text component
 *         has the fallback charset, and get the first text component.
 */

    fprintf(stdout, " %2d     %1d    %24s  True     ", test_num+1,
	    	string_num+1, charsetname[charset_num]);

    if (!XmStringGetLtoR(string[string_num], charset[charset_num], 
		&get_text)) {
		errors++;
		fprintf(stdout, "False                     *\n");
    }
    else {

		fprintf(stdout, "True    %16s  ", get_text);
		if(strcmp(get_text, text[text_num])) {
	    	errors++;
	    	fprintf(stdout, "  *\n");
		}
		else
	    	fprintf(stdout, "\n");
                if ( get_text )
                   XtFree( get_text );
                get_text = (char *)0;

    }

    string_num++;
    test_num++;

/*
 * Test 9: Get the text component from a direction
 */

    if ((string[string_num] = 
		 XmStringDirectionCreate(XmSTRING_DIRECTION_L_TO_R)) == NULL) {
		sprintf(error_string, "Can't create string%d", string_num);
		error_proc(error_string);
    }

    fprintf(stdout, " %2d     %1d    %24s  False    ", test_num+1,
	    	string_num+1, charsetname[0]);

    if (XmStringGetLtoR(string[string_num], charset[0], &get_text)) {
		errors++;
		fprintf(stdout, "True    %16s    *\n", get_text);
                if ( get_text )
                   XtFree( get_text );
                get_text = (char *)0;
    }
    else
		fprintf(stdout, "False\n");

    string_num++;
    test_num++;
    
/*
 * Test 10: Get the text component from a separator
 */

    if ((string[string_num] = XmStringSeparatorCreate()) == NULL) {
		sprintf(error_string, "Can't create string%d", string_num);
		error_proc(error_string);
    }

    fprintf(stdout, " %2d     %1d    %24s  False    ", test_num+1,
	    	string_num+1, charsetname[0]);

    if (XmStringGetLtoR(string[string_num], charset[0], &get_text)) {
		errors++;
		fprintf(stdout, "True    %16s    *\n", get_text);
                if ( get_text )
                   XtFree( get_text );
                get_text = (char *)0;
    }
    else
		fprintf(stdout, "False\n");

    string_num++;
    test_num++;

/*
 * Test 11: Get the text component from a NULL string
 */

    fprintf(stdout, " %2d     %1d    %24s  False    ", test_num+1,
	    	string_num+1, charsetname[0]);

    if (XmStringGetLtoR(NULL, charset[0], &get_text)) {
		errors++;
		fprintf(stdout, "True    %16s    *\n", get_text);
                if ( get_text )
                   XtFree( get_text );
                get_text = (char *)0;
    }
    else
		fprintf(stdout, "False\n");

    test_num++;

/*
 * Test 12: Get the text component from a valid string, given a NULL charset
 */

    fprintf(stdout, " %2d     %1d    %24s  False    ", test_num+1,
	    	string_num+1, charsetname[1]);

    if (XmStringGetLtoR(string[0], charset[1], &get_text)) {
		errors++;
		fprintf(stdout, "True    %16s    *\n", get_text);
                if ( get_text )
                   XtFree( get_text );
                get_text = (char *)0;
    }
    else
		fprintf(stdout, "False\n");

    string_num++;

	summary_proc("StringGLR", errors);

    CommonPause();

    for ( i = 0; i < string_num; i++ )
        XmStringFree( string[i] );

    XtAppMainLoop( app_context );

}
