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
static char rcsid[] = "$XConsortium: summary.c /main/9 1995/07/14 10:54:37 drk $"
#endif
#endif

#include <MrmTest.h>

/*
 * Local declarations
 */
#define PASSED		1
#define FAILED		0

String	result_choice[2] =
{
    "FAILED", "Passed"
};

/*
 * Possible Mrm errors
 */

struct ReturnInfo retstr[63] = 
{
    {"MrmFAILURE        ", 0},
    {"MrmSUCCESS        ", 1},
    {"MrmNOT_FOUND      ", 2},
    {"MrmCREATE_NEW     ", 3},
    {"MrmEXISTS         ", 4},
    {"MrmINDEX_RETRY    ", 5},
    {"MrmNUL_GROUP      ", 6},
    {"MrmINDEX_GT       ", 7},
    {"MrmNUL_TYPE       ", 8},
    {"MrmINDEX_LT       ", 9},
    {"MrmWRONG_GROUP    ", 10},
    {"MrmPARTIAL_SUCCESS", 11},
    {"MrmWRONG_TYPE     ", 12},
    {"                  ", 13},
    {"MrmOUT_OF_RANGE   ", 14},
    {"                  ", 15},
    {"MrmBAD_RECORD     ", 16},
    {"                  ", 17},
    {"MrmNULL_DATA      ", 18},
    {"                  ", 19},
    {"MrmBAD_DATA_INDEX ", 20},
    {"                  ", 21},
    {"MrmBAD_ORDER      ", 22},
    {"                  ", 23},
    {"MrmBAD_CONTEXT    ", 24},
    {"                  ", 25},
    {"MrmNOT_VALID      ", 26},
    {"                  ", 27},
    {"MrmBAD_BTREE_     ", 28},
    {"                  ", 29},
    {"MrmBAD_WIDGET_REC ", 30},
    {"                  ", 31},
    {"MrmBAD_CLASS_TYPE ", 32},
    {"                  ", 33},
    {"MrmNO_CLASS_NAME  ", 34},
    {"                  ", 35},
    {"MrmTOO_MANY       ", 36},
    {"                  ", 37},
    {"MrmBAD_IF_MODULE  ", 38},
    {"                  ", 39},
    {"MrmNULL_DESC      ", 40},
    {"                  ", 41},
    {"MrmOUT_OF_BOUNDS  ", 42},
    {"                  ", 43},
    {"MrmBAD_COMPRESS   ", 44},
    {"                  ", 45},
    {"MrmBAD_ARG_TYPE   ", 46},
    {"                  ", 47},
    {"MrmNOT_IMP        ", 48},
    {"                  ", 49},
    {"MrmNULL_INDEX     ", 50},
    {"                  ", 51},
    {"MrmBAD_KEY_TYPE   ", 52},
    {"                  ", 53},
    {"MrmBAD_CALLBACK   ", 54},
    {"                  ", 55},
    {"MrmNULL_ROUTINE   ", 56},
    {"                  ", 57},
    {"MrmVEC_TOO_BIG    ", 58},
    {"                  ", 59},
    {"MrmBAD_HIERARCHY  ", 60},
    {"                  ", 61},
    {"MrmBAD_CLASS_CODE ", 62},
};

/*
 * summary():
 * summary() will return a String formed by formatting the test results
 * together with some other verbage to make a nice pretty results display.
 *
 * The String that summary() returns to the calling routine is allocated 
 * within this routine.  It is the calling routine's responsibility to
 * ***FREE THE STRING THAT IS RETURNED***
 * after it is used in the calling routine.
 */
String
summary(num_tests, description_length, test_info)
	int		num_tests,
			description_length;
	struct TestInfo	*test_info;
{
/*
 * All of the headers except header_top and trailer will be used as
 * strings to satisfy the %s's in testline (below) so that the format
 * of the headers will always match the format of the description lines.
 */
    String	testline			= "%s    %s  %s  %s\n",
			header_top			= "Test completed.\n\nResults:\n\n",
			header_test			= "Test",
			header_exp			= "Expected Result   ",
			header_act			= "Actual Result     ",
				/* exp and act sizes match the name field of retstr[] above */
    		header_rslt			= "Result",
			header_test_dash	= "----",
			header_exp_act_dash	= "------------------",
    		header_rslt_dash	= "------\n",
    		trailer				= "\n\n",
				/* no specific size for padding, just a lot of spaces */
			padding				= "                                                                                                                          ";
	String	retval,
			result_this_test,
			temp_padded_str;

    int		i,
			padding_len,	/* amount of padding to get "Test" out to 
								description_length size */
			testline_len;	/* length of a full test description line */

/*
 * Allocate space for string we're creating
 */
	testline_len = strlen(testline) +
					description_length +
					strlen(header_exp) +
					strlen(header_act) +
					strlen(header_rslt);
	/* 
	 * Yes, I know I haven't accounted for the extra space occupied by
	 * the %s's and the \ in the \n in testline, but that's okay.  So,
	 * we allocate a few extra bytes - it will allow for things like
	 * NULLs, etc.
	 */
    result_this_test = XtCalloc(testline_len, 1);

	padding_len = description_length - strlen(header_test);
    retval = XtCalloc(((num_tests + 2) * testline_len) +
							strlen(header_top) +
							strlen(trailer),
							1);
	/*
	 * num_tests + 2 because there are two header lines on top of the
	 * description lines
	 */

	temp_padded_str = XtCalloc(description_length + 5, 1);
	/* add 5 more bytes just for good luck */

/*
 * Build and add header lines, use testline as the format to insure uniformity
 */
	strcpy(retval, header_top);
	/* have to pad the test header out to the size of description_length */
	strcpy(temp_padded_str, header_test);
	strncat(temp_padded_str, padding, padding_len);
	sprintf(result_this_test, testline,
			temp_padded_str,
			header_exp,
			header_act,
			header_rslt);
	strcat(retval, result_this_test);

	/* have to pad the test_dash header out to the size of description_length */
	strcpy(temp_padded_str, header_test_dash);
	strncat(temp_padded_str, padding, padding_len);
	sprintf(result_this_test, testline,
			temp_padded_str,
			header_exp_act_dash,
			header_exp_act_dash,
			header_rslt_dash);
	strcat(retval, result_this_test);

/*
 * Add description of, and results from, each individual test
 */
    for(i=0 ; i<num_tests ; i++)
    {
		sprintf(result_this_test, testline,
			(*test_info).description,
			retstr[(*test_info).expected_return].name,
			retstr[(*test_info).actual_return].name,
			((*test_info).actual_return == (*test_info).expected_return) ?
			result_choice[PASSED] : result_choice[FAILED]);
	
		strcat(retval, result_this_test);
		*test_info++;
    }

/*
 * Add closing message
 */
    strcat(retval, trailer);
    
    return(retval);
}


