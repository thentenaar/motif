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
static char rcsid[] = "$XConsortium: StringComp.c /main/7 1995/07/13 19:53:41 drk $"
#endif
#endif

#include <stdio.h>

#include <testlib.h>

#include "String.h"
#include "StringComp.h"


main(argc, argv)
unsigned int	argc;
char		*argv[];
{

	int	i;
	char	*tmp;
	
    string_num = text_num = charset_num = test_num = 0;
    c_errors = bc_errors = 0;

    CommonTestInit(argc, argv);

    fprintf(stdout, "\nThe following is a list of all compound strings");
    fprintf(stdout, " created for this\ntest, and the method/creation");
    fprintf(stdout, " routines used to create the strings.\n\n");

/*
String  Creation Routine          Text                 Character Set
------  ----------------          -------------------  -------------
*/

    fprintf(stdout, "String  Creation Routine          Text                ");
    fprintf(stdout, " Character Set\n");
    fprintf(stdout, "------  ----------------          ------------------- ");
    fprintf(stdout, " -------------\n");
    fprintf(stdout, "   1    XmStringCreateLtoR        ");
    fprintf(stdout, "\"This is\\nString1.\"  XmSTRING_DEFAULT_CHARSET\n");
    fprintf(stdout, "   2    XmStringCopy(string1)\n");
    fprintf(stdout, "   3    XmStringCreate            \"String3 here\"    ");
    fprintf(stdout, "   \"ISOLatin1\"\n");
    fprintf(stdout, "   4    XmStringCreateLtoR        \"String3 here\"    ");
    fprintf(stdout, "   \"ISOLatin1\"\n");
    fprintf(stdout, "   5    XmStringCreateLtoR        \"String5\\n\"      ");
    fprintf(stdout, "    \"ISOLatin1\"\n");
    fprintf(stdout, "\n");
    fprintf(stdout, "   6    XmStringDirectionCreate");
    fprintf(stdout, "(XmSTRING_DIRECTION_L_TO_R)\n");
    fprintf(stdout, "\n");
    fprintf(stdout, "   7    XmStringCreate            \"String5\"         ");
    fprintf(stdout, "   \"ISOLatin1\"\n");
    fprintf(stdout, "\n");
    fprintf(stdout, "   8    XmStringConcat(string6, string7)\n");
    fprintf(stdout, "   9    XmStringSeparatorCreate()\n");
    fprintf(stdout, "  10    XmStringConcat(string8, string9)\n");
    fprintf(stdout, "\n");
    fprintf(stdout, "  11    XmStringCreateLtoR        \"Any old\\ntext.\" ");
    fprintf(stdout, "    \"ISOLatin1\"\n");
    fprintf(stdout, "  12    XmStringCreateLtoR        \"Any old\\ntext.\" ");
    fprintf(stdout, "    \"MyCharset\"\n");
    fprintf(stdout, "  13    XmStringCreateLtoR        \"Any old\\ntext.\" ");
    fprintf(stdout, "    XmSTRING_DEFAULT_CHARSET\n");
    fprintf(stdout, "  14    XmStringCreateLtoR        \"Any old\\ntext.\" ");
    fprintf(stdout, "    \"MyCharset\"\n");
    fprintf(stdout, "  15    XmStringCreateLtoR        \"This is the text\"");
    fprintf(stdout, "   XmSTRING_DEFAULT_CHARSET\n");
    fprintf(stdout, "  16    XmStringCreateLtoR        \"This is the text\"");
    fprintf(stdout, "   \"MyCharset\"\n");
    fprintf(stdout, "\n");
    fprintf(stdout, "  17    (string17 = NULL)\n");
#ifndef MOTIF1_1
    fprintf(stdout, 
			"  18    XmStringCreate            \"This is to test PIR3933\" ");
    fprintf(stdout, "    XmSTRING_DEFAULT_CHARSET\n");
    fprintf(stdout, 
			"  19    XmStringCreate            \"This is to test PIR3933\" ");
    fprintf(stdout, "    XmFONTLIST_DEFAULT_TAG\n");
    fprintf(stdout, "  20    XmStringCreateLtoR        \"This tests\\n\\n\\n PIR7403\"");
    fprintf(stdout, "    XmFONTLIST_DEFAULT_TAG\n");
    fprintf(stdout, "  21    XmCvtCTToXmString(XmCvtXmStringToCt(string20)\n");
    fprintf(stdout, "  22    XmStringCreateLtoR        \"CH\" ");
    fprintf(stdout, "                         XmFONTLIST_DEFAULT_TAG\n");

#endif

    fprintf(stdout, "\nEach row in the following table represents the");
    fprintf(stdout, " comparison of two of the\nabove strings using the");
    fprintf(stdout, " routines XmStringCompare and XmStringByteCompare.\n");
    fprintf(stdout, "For each comparison, the expected and actual results");
    fprintf(stdout, " are given.\n\n");
    
/*
                      XmStringCompare    XmStringByteCompare           Byte
      First   Second                                          Compare  Compare
Test  String  String  Expect   Actual    Expect    Actual     Error    Error
----  ------  ------  -------- --------  --------- ---------  -------  -------
*/

    fprintf(stdout, "                      XmStringCompare    ");
    fprintf(stdout, "XmStringByteCompare           Byte\n");
    fprintf(stdout, "      First   Second                     ");
    fprintf(stdout, "                     Compare  Compare\n");
    fprintf(stdout, "Test  String  String  Expect   Actual    ");
    fprintf(stdout, "Expect    Actual     Error    Error\n");
    fprintf(stdout, "----  ------  ------  -------- --------  ");
    fprintf(stdout, "--------- ---------  -------  -------\n");

/*
 * Test 1: Create a compound string and compare it to itself
 */

    if ((string[string_num] = XmStringCreateLtoR(text[text_num],
												 charset[charset_num])) 
												 == NULL) {
		sprintf(error_string, "Can't create string%d", string_num);
		error_proc(error_string);
    }
    
    do_comparisons(test_num, string_num, string_num);

    string_num++;
    test_num++;
    
/*
 * Test 2: Make a copy of a compound string and compare the two strings
 */

    if ((string[string_num] = XmStringCopy(string[string_num-1])) == NULL) {
		sprintf(error_string, "Can't copy string%d to string%d", string_num-1,
				string_num);
		error_proc(error_string);
    }
    
    do_comparisons(test_num, string_num-1, string_num);

    string_num++;
    text_num++;
    charset_num = charset_num+2;
    test_num++;

/*
 * Test 3: Create identical strings with XmStringCreate and 
 *         XmStringCreateLtoR and compare the two strings
 */

    if ((string[string_num] = XmStringCreate(text[text_num],
					    					 charset[charset_num])) 
											 == NULL) {
		sprintf(error_string, "Can't create string%d", string_num);
		error_proc(error_string);
    }

    string_num++;
    
    if ((string[string_num] = XmStringCreateLtoR(text[text_num],
												 charset[charset_num])) 
												 == NULL) {
		sprintf(error_string, "Can't create string%d", string_num);
		error_proc(error_string);
    }
    
    do_comparisons(test_num, string_num-1, string_num);

    string_num++;
    text_num++;
    test_num++;

/*
 * Test 4: Create two strings, one which has a separator and is created with
 *         XmStringCreateLtoR , and one which is a concatenation of a string
 *         which contains only the text portion of the first string, and a
 *         string which contains only a separator.  Compare the two strings.
 */

    if ((string[string_num] = XmStringCreateLtoR(text[text_num],
												 charset[charset_num])) 
												 == NULL) {
		sprintf(error_string, "Can't create string%d", string_num);
		error_proc(error_string);
    }

    string_num++;
    text_num++;
    
    if ((string[string_num] = 
		 XmStringDirectionCreate(XmSTRING_DIRECTION_L_TO_R)) == NULL) {
		sprintf(error_string, "Can't create string%d", string_num);
		error_proc(error_string);
    }

    string_num++;
    
    if ((string[string_num] = XmStringCreate(text[text_num],
					    					 charset[charset_num])) 
											 == NULL) {
		sprintf(error_string, "Can't create string%d", string_num);
		error_proc(error_string);
    }

    string_num++;
    
    if ((string[string_num] = XmStringConcat(string[string_num-2],
					    					 string[string_num-1])) 
											 == NULL) {
		sprintf(error_string, "Can't create string%d", string_num);
		error_proc(error_string);
    }
    
    string_num++;
    
    if ((string[string_num] = XmStringSeparatorCreate()) == NULL) {
		sprintf(error_string, "Can't create string%d", string_num);
		error_proc(error_string);
    }

    string_num++;

    if ((string[string_num] = XmStringConcat(string[string_num-2],
					    					 string[string_num-1])) 
											 == NULL) {
		sprintf(error_string, "Can't create string%d", string_num);
		error_proc(error_string);
    }
    
    do_comparisons(test_num, string_num-5, string_num);

    string_num++;
    text_num++;
    test_num++;

/*
 * Test 5: Create 2 strings, with XmStringCreateLtoR, with identical text
 *         components.  Give the two different character sets.
 */

    if ((string[string_num] = XmStringCreateLtoR(text[text_num],
												 charset[charset_num])) 
												 == NULL) {
		sprintf(error_string, "Can't create string%d", string_num);
		error_proc(error_string);
    }

    string_num++;
    charset_num++;

    if ((string[string_num] = XmStringCreateLtoR(text[text_num],
												 charset[charset_num])) 
												 == NULL) {
		sprintf(error_string, "Can't create string%d", string_num);
		error_proc(error_string);
    }

    do_comparisons(test_num, string_num-1, string_num);

    string_num++;
    test_num++;

/*
 * Test 6: Create 2 strings, with XmStringCreateLtoR, with identical text
 *         components.  Give one the charset XmSTRING_DEFAULT_CHARSET and
 *         the other another charset.  Compare the two strings.
 */

    if ((string[string_num] = XmStringCreateLtoR(text[text_num],
												 charset[0])) == NULL) {
		sprintf(error_string, "Can't create string%d", string_num);
		error_proc(error_string);
    }

    string_num++;

    if ((string[string_num] = XmStringCreateLtoR(text[text_num],
												 charset[charset_num])) 
												 == NULL) {
		sprintf(error_string, "Can't create string%d", string_num);
		error_proc(error_string);
    }

    do_comparisons(test_num, string_num-1, string_num);

    string_num++;
    text_num++;
    test_num++;

/*
 * Test 7: Create 2 strings, with XmStringCreate, with identical text
 *         components.  Give one the charset XmSTRING_DEFAULT_CHARSET
 *         and the other another charset.  Compare the two strings.
 */

    if ((string[string_num] = XmStringCreate(text[text_num],
					    					 charset[0])) == NULL) {
		sprintf(error_string, "Can't create string%d", string_num);
		error_proc(error_string);
    }

    string_num++;

    if ((string[string_num] = XmStringCreate(text[text_num],
					    					 charset[charset_num])) 
											 == NULL) {
		sprintf(error_string, "Can't create string%d", string_num);
		error_proc(error_string);
    }
    
    do_comparisons(test_num, string_num-1, string_num);

    string_num++;
    test_num++;

/*
 * Test 8: Compare a string to a NULL string
 */

    string[string_num] = NULL;
    
    do_comparisons(test_num, 0, string_num);

#ifndef MOTIF1_1

    string_num++;
    test_num++;

	text_num++;

/*
 * Test 9: Create 2 strings, with XmStringCreate, with identical text
 *         components and identical charset "MyCharset".
 *         Compare the two strings. This is to test PIR3933 against
 *		   "optimized" strings.
 */

    if ((string[string_num] = XmStringCreate(text[text_num],
					    					 XmFONTLIST_DEFAULT_TAG)) == NULL) {
		sprintf(error_string, "Can't create string%d", string_num);
		error_proc(error_string);
    }

    string_num++;

    if ((string[string_num] = XmStringCreate(text[text_num],
					    					 XmSTRING_DEFAULT_CHARSET)) 
											 == NULL) {
		sprintf(error_string, "Can't create string%d", string_num);
		error_proc(error_string);
    }
    
    do_comparisons(test_num, string_num-1, string_num);


	string_num++;
	test_num++;

	text_num++;

/*
 * Test 10: Create 1 string with separators with XmStringCreateLtoR.  
 *          "Copy" it by converting it to compound text and back.
 *          Compare the two strings. This is to test PIR7403 against
 *		   XmCvtXmStringToCT.
 */
	if ((string[string_num] = XmStringCreateLtoR(text[text_num],
						     XmFONTLIST_DEFAULT_TAG))
	    == NULL) {
	  sprintf(error_string, "Can't create string%d", string_num);
	  error_proc(error_string);
	}

	tmp = XmCvtXmStringToCT(string[string_num]);

	string_num++;

	string[string_num] = XmCvtCTToXmString(tmp);
	
	do_comparisons(test_num, string_num-1, string_num);

	string_num++;
	test_num++;

/*
 * Test 11: Compare a nonopt 2-line to an opt 2-char, both using first charset
 *	    This tests CR 8015.
 */
	
	string[string_num] = XmStringCreateLtoR("CH", charset[0]);

	do_comparisons(test_num, 0, string_num);
#endif

	for (i = 0; i <= string_num; i++)
		if (string[i] != NULL)
			XmStringFree(string[i]);


/*
 * Print an error summary
 */

    if (c_errors) {
		fprintf(stdout, "\nThe XmStringCompare portion of this test failed");
		fprintf(stdout, " with %d errors.\n", c_errors);
    }
    else
		fprintf(stdout, "\nThe XmStringCompare portion of this test passed.\n");
    
    if (bc_errors) {
		fprintf(stdout, "The XmStringByteCompare portion of this test failed");
		fprintf(stdout, " with %d errors.\n", bc_errors);
    }
    else {
		fprintf(stdout, "The XmStringByteCompare portion of this test");
		fprintf(stdout, " passed.\n");
    }

    errors = c_errors + bc_errors;

    summary_proc("StringComp", errors);

    CommonPause();
    XtAppMainLoop( app_context );

}


static void do_comparisons(test_num, string_num1, string_num2)
int	test_num,
	string_num1,
   	string_num2;
{

    int	error1, error2;
    int	result1, result2;

    error1 = error2 = 0;
    result1 = result2 = 0;
    
/*
 * Compare the two strings with XmStringCompare
 */
     
    result1 = XmStringCompare(string[string_num1], string[string_num2]);
    
    if (result1 != c_expect[test_num]) {
		c_errors++;
		error1++;
    }

/*
 * Compare the two strings with XmStringByteCompare
 */
     
    result2 = XmStringByteCompare(string[string_num1], string[string_num2]);

    if (result2 != bc_expect[test_num]) {
		bc_errors++;
		error2++;
    }

/*
 * Print the results of the comparisons
 */

    fprintf(stdout, "  %2d     %2d      %2d    ", test_num+1, string_num1+1,
	    	string_num2+1);

    fprintf(stdout, "%8s %8s  %9s %9s  ", result_string1[c_expect[test_num]],
	    	result_string1[result1], result_string2[bc_expect[test_num]],
	    	result_string2[result2]);
    	    
    if (error1)
		fprintf(stdout, "   *    ");
    else
		fprintf(stdout, "        ");
    
    if(error2)
		fprintf(stdout, "    *\n");
    else
		fprintf(stdout, "\n");

}
