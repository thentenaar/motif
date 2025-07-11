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
static char rcsid[] = "$XConsortium: FetchL.c /main/9 1995/07/14 10:47:50 drk $"
#endif
#endif

#include <MrmTest.h>

#include <values.h>

/*
 * Name of uid file
 */

String		filename_vec[] =
{
    "FetchL.uid"
};

MrmCount	filename_num =
(sizeof(filename_vec) / sizeof(filename_vec[0]));

/*
 * Convenience routine
 */

void		error_proc();
extern void	return_summary();

/*
 * Data and declarations for summary
 */

#define NUM_TESTS		9
#define TEST_DESCRIP_LENGTH	33

struct TestInfo FetchLiteralInfo[NUM_TESTS] = 
{
    {"Fetch literal from NULL hierarchy", MrmBAD_HIERARCHY,	0},
    {"Fetch non-exported literal       ", MrmNOT_FOUND,		0},
    {"Fetch nonexistent literal        ", MrmNOT_FOUND,		0},
    {"Fetch icon literal               ", MrmWRONG_TYPE,	0},
    {"Fetch color literal              ", MrmWRONG_TYPE,	0},
    {"Fetch valid boolean literal      ", MrmSUCCESS,		0},
    {"Fetch valid float literal        ", MrmSUCCESS,		0},
    {"Fetch valid integer literal      ", MrmSUCCESS,		0},
    {"Fetch valid string literal       ", MrmSUCCESS,		0}
};

#define PASSED		1
#define FAILED		0

String	result_choice[2] =
{
    "FAILED", "Passed"
};

/*
 * Possible Mrm errors
 */

struct ReturnInfo retval[63] = 
{
    {"MrmFAILURE       ", 0},
    {"MrmSUCCESS       ", 1},
    {"MrmNOT_FOUND     ", 2},
    {"MrmCREATE_NEW    ", 3},
    {"MrmEXISTS        ", 4},
    {"MrmINDEX_RETRY   ", 5},
    {"MrmNUL_GROUP     ", 6},
    {"MrmINDEX_GT      ", 7},
    {"MrmNUL_TYPE      ", 8},
    {"MrmINDEX_LT      ", 9},
    {"MrmWRONG_GROUP   ", 10},
    {"                 ", 11},
    {"MrmWRONG_TYPE    ", 12},
    {"                 ", 13},
    {"MrmOUT_OF_RANGE  ", 14},
    {"                 ", 15},
    {"MrmBAD_RECORD    ", 16},
    {"                 ", 17},
    {"MrmNULL_DATA     ", 18},
    {"                 ", 19},
    {"MrmBAD_DATA_INDEX", 20},
    {"                 ", 21},
    {"MrmBAD_ORDER     ", 22},
    {"                 ", 23},
    {"MrmBAD_CONTEXT   ", 24},
    {"                 ", 25},
    {"MrmNOT_VALID     ", 26},
    {"                 ", 27},
    {"MrmBAD_B_TREE    ", 28},
    {"                 ", 29},
    {"MrmBAD_WIDGET    ", 30},
    {"                 ", 31},
    {"MrmBAD_CLASS     ", 32},
    {"                 ", 33},
    {"MrmNO_CLASS_NAME ", 34},
    {"                 ", 35},
    {"MrmTOO_MANY      ", 36},
    {"                 ", 37},
    {"MrmBAD_IF_MODULE ", 38},
    {"                 ", 39},
    {"MrmNULL_DESC     ", 40},
    {"                 ", 41},
    {"MrmOUT_OF_BOUNDS ", 42},
    {"                 ", 43},
    {"MrmBAD_COMPRESS  ", 44},
    {"                 ", 45},
    {"MrmBAD_ARG_TYPE  ", 46},
    {"                 ", 47},
    {"MrmNOT_IMP       ", 48},
    {"                 ", 49},
    {"MrmNULL_INDEX    ", 50},
    {"                 ", 51},
    {"MrmBAD_KEY_TYPE  ", 52},
    {"                 ", 53},
    {"MrmBAD_CALLBACK  ", 54},
    {"                 ", 55},
    {"MrmNULL_ROUTINE  ", 56},
    {"                 ", 57},
    {"MrmVEC_TOO_BIG   ", 58},
    {"                 ", 59},
    {"MrmBAD_HIERARCHY ", 60},
    {"                 ", 61},
    {"MrmBAD_CLASS_CODE", 62},
};

static String	result;
static String	result_this_test;

/*
 * Local declarations
 */

MrmHierarchy	s_MrmHierarchy;
MrmType		class;
MrmCode		type;

Widget		toplevel = NULL;

Display		*dsp;

int		bad_literal;
Pixmap		icon_literal;
Pixel		color_literal;

int		*boolean_literal,
		boolean_expect = TRUE;

double		*float_literal,
		float_expect = 3.14159;

int		*integer_literal,
		integer_expect = 42;

String		string_literal,
		string_expect = "stRi ng8";

main(argc, argv)
     int argc;
     char *argv[];
{ 	

/*
 * Initialize Mrm
 */
    MrmInitialize();

/*
 *  Initialize the toolkit.  This call returns the id of the "toplevel" widget.
 */

    toplevel = XtInitialize("FetchLiteral",	/* application name	   */
			    "genericclass",	/* application class	   */
			    NULL, 0,		/* options		   */
			    &argc, argv);	/* command line parameters */

/*
 *  Open the Mrm hierarchy
 */

    if(MrmOpenHierarchy(filename_num,		/* number of files	*/
			filename_vec,		/* files		*/
			NULL,			/* os_ext_list (null)	*/
			&s_MrmHierarchy)	/* ptr to returned id	*/
       != MrmSUCCESS) 
    {
	error_proc("Can't open hierarchy\n");
    }

/*
 * Get display
 */

	dsp = XtDisplay(toplevel);

/*
 * Try to fetch literal from nonexistent hierarchy
 */

    FetchLiteralInfo[0].actual_return = MrmFetchLiteral(NULL,
							"integer_literal",
							dsp,
							(XtPointer)
							&bad_literal,
							&type);
    
/*
 * Try to fetch nonexported literal
 */

    FetchLiteralInfo[1].actual_return = MrmFetchLiteral(s_MrmHierarchy,
							"nonexported_literal",
							dsp,
							(XtPointer)
							&bad_literal,
							&type);

/*
 * Try to fetch nonexistent literal
 */

    FetchLiteralInfo[2].actual_return = MrmFetchLiteral(s_MrmHierarchy,
							"nonexistent_literal",
							dsp,
							(XtPointer)
							&bad_literal,
							&type);

/*
 * Try to fetch icon literal
 */

    FetchLiteralInfo[3].actual_return = MrmFetchLiteral(s_MrmHierarchy,
							"blank_icon",
							dsp,
							(XtPointer)
							&icon_literal,
							&type);

/*
 * Try to fetch color literal
  */

    FetchLiteralInfo[4].actual_return = MrmFetchLiteral(s_MrmHierarchy,
							"red",
							dsp,
							(XtPointer)
							&color_literal,
							&type);

/*
 * Try to fetch boolean literal
 */

    FetchLiteralInfo[5].actual_return = MrmFetchLiteral(s_MrmHierarchy,
							"boolean_literal",
							dsp,
							(XtPointer)
							&boolean_literal,
							&type);

/*
 * Try to fetch integer literal
 */

    FetchLiteralInfo[7].actual_return = MrmFetchLiteral(s_MrmHierarchy,
							"integer_literal",
							dsp,
							(XtPointer)
							&integer_literal,
							&type);

/*
 * Try to fetch string literal
 */

    FetchLiteralInfo[8].actual_return = MrmFetchLiteral(s_MrmHierarchy,
							"string_literal",
							dsp,
							(XtPointer)
							&string_literal,
							&type);

/*
 * Try to fetch float literal
 */

    FetchLiteralInfo[6].actual_return = MrmFetchLiteral(s_MrmHierarchy,
							"float_literal",
							dsp,
							(XtPointer)
							&float_literal,
							&type);

/*
 * Print results of valid fetches
 */

    fprintf(stdout,"\n\nTest completed.\n\nValid MrmFetchLiteral Calls:\n\n");
    fprintf(stdout,"                           \tExpected\tFetched\n");
    fprintf(stdout,"Test                       \tValue   \tValue   \n");
    fprintf(stdout,"----                       \t--------\t--------\n");

    fprintf(stdout,"Fetch valid boolean literal\t%8d\t%8d\n",
	    boolean_expect, *boolean_literal);

    fprintf(stdout,"Fetch valid float literal  \t%8f\t%8f\n",
	    float_expect, *float_literal);

    fprintf(stdout,"Fetch valid integer literal\t%8d\t%8d\n",
	    integer_expect, *integer_literal);

    fprintf(stdout,"Fetch valid string literal \t%8s\t%8s\n",
	    string_expect, string_literal);

/*
 * Close Mrm Hierarchy
 */

    MrmCloseHierarchy(s_MrmHierarchy);

    return_summary(NUM_TESTS, TEST_DESCRIP_LENGTH, FetchLiteralInfo);
}

/*
 * Error handling
 */

void error_proc(error_string)
     String	error_string;
{
    fprintf(stdout,"Mrm error: %s\n", error_string);
    fflush(stdout);
}

/*
 * Print a summary of the error return values
 */

void return_summary(num_tests, description_length, test_info)
     int		num_tests,
     			description_length;
     struct TestInfo	*test_info;
{
    String	header1 = "\nError Return Results:\n\n",
    		header2 = "Expected           Actual           \n",
    		header3 = "Return             Return             Result\n",
    		header4 = "--------           ------             ------\n",
    		header5 = "Test",
    		header6 = "----";
    

    int		i;

    String	padding,
    		ptr;
    
    padding = calloc(description_length - 2, 1);

/*
 * Make padding a blank string
 */

    ptr = padding;
    for(i=0 ; i<description_length-2 ; i++)
    {
	*ptr++ = ' ';
    }
    *ptr = '\0';

/*
 * Print header lines
 */

    fprintf(stdout,"%s", header1);
    fprintf(stdout,"    %s%s", padding, header2);
    fprintf(stdout,"%s%s%s", header5, padding, header3);
    fprintf(stdout,"%s%s%s", header6, padding, header4);
    
/*
 * Print description of, and results from, each individual test
 */

    for(i=0 ; i<num_tests ; i++)
    {
	if((*test_info).actual_return == (*test_info).expected_return)
	{
	    result = result_choice[PASSED];
	}
	else
	{
	    result = result_choice[FAILED];
	}

	fprintf(stdout,"%s  %s  %s  %s\n",
		(*test_info).description,
		retval[(*test_info).expected_return].name,
		retval[(*test_info).actual_return].name,
		result);

	*test_info++;
    }
}
