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
static char rcsid[] = "$XConsortium: multC.c /main/4 1995/07/13 20:30:51 drk $"
#endif
#endif

#include <multC.h>

main(argc, argv)
     int argc;
     char *argv[];
{ 	
    int	i,
    	errors = 0;

/*
 * Initialize Mrm
 */
    MrmInitialize();

/*
 *  Initialize the toolkit.  This call returns the id of the "toplevel" widget.
 */

    toplevel = XtInitialize("multC",		/* application name	   */
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
 * Register the names of the callback routines with Mrm
 */
    
    if(MrmRegisterNames(reglist,	/* list of names	*/
			reglist_num)	/* number of names	*/
       != MrmSUCCESS)
    {
	error_proc("Can't register names\n");
    }
    
/*
 * Get display, colormap
 */

    dsp = XtDisplay(toplevel);

    cmap = DefaultColormap(dsp, DefaultScreen(dsp));

/*
 * Part 1
 */

/*
 * Set up header for test results
 */

    fprintf(stdout,"Part 1: Test multiple callback procedures\n------\n\n");
    fprintf(stdout,"                                                        ");
    fprintf(stdout,"            Callback\n");
    fprintf(stdout,"                                                        ");
    fprintf(stdout,"              Order\n");
    fprintf(stdout,"Test                                                    ");
    fprintf(stdout,"    Result  Preserved\n");
    fprintf(stdout,"----                                                    ");
    fprintf(stdout,"    ------  ---------\n");

    for(i=0 ; i<NUM_TESTS_PART1 ; i++)
    {

/*
 * Fetch a widget with one or more callback
 * procedures associated with one reason
 */

	if(MrmFetchWidget(s_MrmHierarchy,	
			  part1_widget_names[i],
			  toplevel,
			  &part1_widget[i],
			  &class) != MrmSUCCESS)
	{
	    sprintf(mrm_err, "Can't fetch widget %s", part1_widget_names[i]);
	    error_proc(mrm_err);
	}
	else
	{

/*
 * Call the callback list associated with the XmNactivateCallback reason
 */

	    callbacks_called = 0;
	    order_preserved = 0;
	    
	    XtCallCallbacks(part1_widget[i], XmNactivateCallback, NULL);

	    part1_callbacks_called[i] = callbacks_called;
	    part1_order_actual[i] = order_preserved;

/*	    
 * Compare callbacks called to callbacks expected and print results
 */

	    fprintf(stdout, "%s", part1_test_names[i]);

	    if(part1_callbacks_called[i] == part1_callbacks_expected[i])
	    {
		fprintf(stdout, "  Passed");
	    }
	    else
	    {
		fprintf(stdout, "  FAILED");
	    }

	    if(part1_order_actual[i] == part1_order_expected[i])
	    {
		fprintf(stdout, "     Yes\n");
	    }
	    else
	    {
		fprintf(stdout, "     NO\n");
	    }
	}
    }

    fprintf(stdout,"%s\n", result_buffer);

/*
 * Part 2
 */

/*
 * Set up header for test results
 */

    fprintf(stdout,"\n\nPart 2: Test callbacks lists\n------\n\n");
    fprintf(stdout,"Test                             ");
    fprintf(stdout,"  Result\n");
    fprintf(stdout,"----                             ");
    fprintf(stdout,"  ------\n");

    for(i=0 ; i<NUM_TESTS_PART2 ; i++)
    {

/*
 * Fetch a widget with one callback procedure
 * associated with each of several reasons
 */

	if(MrmFetchWidget(s_MrmHierarchy,	
			  part2_widget_names[i],
			  toplevel,
			  &part2_widget[i],
			  &class) != MrmSUCCESS)
	{
	    sprintf(mrm_err, "Can't fetch widget %s", part2_widget_names[i]);
	    error_proc(mrm_err);
	}
	else
	{

/*
 * Call the callback lists associated with the
 * XmNactivateCallback, XmNarmCallback, and XmNdisarmCallback reasons
 */

	    callbacks_called = 0;
	    
	    XtCallCallbacks(part2_widget[i], XmNactivateCallback, NULL);
	    XtCallCallbacks(part2_widget[i], XmNarmCallback, NULL);
	    XtCallCallbacks(part2_widget[i], XmNdisarmCallback, NULL);

	    part2_callbacks_called[i] = callbacks_called;

/*	    
 * Compare callbacks called to callbacks expected and print results
 */

	    fprintf(stdout, "%s", part2_test_names[i]);

	    if(part2_callbacks_called[i] == part2_callbacks_expected[i])
	    {
		fprintf(stdout, "  Passed\n");
	    }
	    else
	    {
		fprintf(stdout, "  FAILED\n");
	    }
	}
    }

/*
 * Part 3
 */

/*
 * Set up header for test results
 */

    fprintf(stdout,"\n\nPart 3: Test multiply defined reasons\n------\n\n");
    fprintf(stdout,"Test                                                    ");
    fprintf(stdout,"         Result\n");
    fprintf(stdout,"----                                                    ");
    fprintf(stdout,"         ------\n");

    for(i=0 ; i<NUM_TESTS_PART3 ; i++)
    {

/*
 * Fetch a widget with one callback procedure
 * associated with each of several reasons
 */

	if(MrmFetchWidget(s_MrmHierarchy,	
			  part3_widget_names[i],
			  toplevel,
			  &part3_widget[i],
			  &class) != MrmSUCCESS)
	{
	    sprintf(mrm_err, "Can't fetch widget %s", part3_widget_names[i]);
	    error_proc(mrm_err);
	}
	else
	{

/*
 * Call the callback lists associated with the
 * XmNactivateCallback, XmNarmCallback, and XmNdisarmCallback reasons
 */

	    callbacks_called = 0;
	    
	    XtCallCallbacks(part3_widget[i], XmNactivateCallback, NULL);
	    XtCallCallbacks(part3_widget[i], XmNarmCallback, NULL);
	    XtCallCallbacks(part3_widget[i], XmNdisarmCallback, NULL);

	    part3_callbacks_called[i] = callbacks_called;

/*	    
 * Compare callbacks called to callbacks expected and print results
 */

	    fprintf(stdout, "%s", part3_test_names[i]);

	    if(part3_callbacks_called[i] == part3_callbacks_expected[i])
	    {
		fprintf(stdout, "  Passed\n");
	    }
	    else
	    {
		fprintf(stdout, "  FAILED\n");
	    }
	}
    }

/*
 * Part 4
 */

/*
 * Set up header for test results
 */

    fprintf(stdout,"\n\nPart 4: Test repeated callback assignments\n------\n\n");
    fprintf(stdout,"Test                                                    ");
    fprintf(stdout,"         Result\n");
    fprintf(stdout,"----                                                    ");
    fprintf(stdout,"         ------\n");

    for(i=0 ; i<NUM_TESTS_PART4 ; i++)
    {

/*
 * Fetch a widget with one callback procedure
 * associated with each of several reasons
 */

	if(MrmFetchWidget(s_MrmHierarchy,	
			  part4_widget_names[i],
			  toplevel,
			  &part4_widget[i],
			  &class) != MrmSUCCESS)
	{
	    sprintf(mrm_err, "Can't fetch widget %s", part4_widget_names[i]);
	    error_proc(mrm_err);
	}
	else
	{

/*
 * Call the callback lists associated with the
 * XmNactivateCallback, XmNarmCallback, and XmNdisarmCallback reasons
 */

	    callbacks_called = 0;
	    
	    XtCallCallbacks(part4_widget[i], XmNactivateCallback, NULL);
	    XtCallCallbacks(part4_widget[i], XmNarmCallback, NULL);
	    XtCallCallbacks(part4_widget[i], XmNdisarmCallback, NULL);

	    part4_callbacks_called[i] = callbacks_called;

/*	    
 * Compare callbacks called to callbacks expected and print results
 */

	    fprintf(stdout, "%s", part4_test_names[i]);

	    if(part4_callbacks_called[i] == part4_callbacks_expected[i])
	    {
		fprintf(stdout, "  Passed\n");
	    }
	    else
	    {
		fprintf(stdout, "  FAILED\n");
	    }
	}
    }

    fprintf(stdout,"%s\n", result_buffer);

#ifdef notdef
    fprintf(stdout,"\n\nPart 1\n------\n\n");
	
    for(i=0 ; i<NUM_TESTS_PART1 ; i++)
    {
	fprintf(stdout,"callbacks called = %8d\tcallbacks expected = %8d\n",
		part1_callbacks_called[i], part1_callbacks_expected[i]);

	fprintf(stdout,"    actual order = %8d\t    expected order = %8d\n",
		part1_order_actual[i], part1_order_expected[i]);
    }
    
    fprintf(stdout,"\n\n\nPart 2\n------\n\n");
	
    for(i=0 ; i<NUM_TESTS_PART2 ; i++)
    {
	fprintf(stdout,"callbacks called = %8d\tcallbacks expected = %8d\n",
		part2_callbacks_called[i], part2_callbacks_expected[i]);
    }

    fprintf(stdout,"\n\n\nPart 3\n------\n\n");
	
    for(i=0 ; i<NUM_TESTS_PART2 ; i++)
    {
	fprintf(stdout,"callbacks called = %8d\tcallbacks expected = %8d\n",
		part3_callbacks_called[i], part3_callbacks_expected[i]);
    }

    fprintf(stdout,"\n\n\nPart 4\n------\n\n");
	
    for(i=0 ; i<NUM_TESTS_PART4 ; i++)
    {
	fprintf(stdout,"callbacks called = %8d\tcallbacks expected = %8d\n",
		part4_callbacks_called[i], part4_callbacks_expected[i]);
    }
#endif
/*
 * Close Mrm Hierarchy
 */

    MrmCloseHierarchy(s_MrmHierarchy);

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
 * Callback routines
 */

void callback1_1(w, tag, reason)
     Widget		w;
     caddr_t		tag;
     unsigned long	*reason;
{
    if(callbacks_called == 0)
    {
	order_preserved++;
    }
    
    callbacks_called |= CALLBACK1_1;
}

void callback2_1(w, tag, reason)
     Widget		w;
     caddr_t		tag;
     unsigned long	*reason;
{
    if(callbacks_called == 0)
    {
	order_preserved++;
    }
    
    callbacks_called |= CALLBACK2_1;
}

void callback2_2(w, tag, reason)
     Widget		w;
     caddr_t		tag;
     unsigned long	*reason;
{
    if(callbacks_called == CALLBACK2_1)
    {
	order_preserved++;
    }
    
    callbacks_called |= CALLBACK2_2;
}

void callback2_3(w, tag, reason)
     Widget		w;
     caddr_t		tag;
     unsigned long	*reason;
{
    if(callbacks_called == (CALLBACK2_1 | CALLBACK2_2))
    {
	order_preserved++;
    }
    
    callbacks_called |= CALLBACK2_3;
}

void callback3_1(w, tag, reason)
     Widget		w;
     caddr_t		tag;
     unsigned long	*reason;
{
    if(callbacks_called == 0)
    {
	order_preserved++;
    }
    
    callbacks_called |= CALLBACK3_1;
}

void callback3_2(w, tag, reason)
     Widget		w;
     caddr_t		tag;
     unsigned long	*reason;
{
    if(callbacks_called == CALLBACK3_1)
    {
	order_preserved++;
    }
    
    callbacks_called |= CALLBACK3_2;
}

void callback4_1(w, tag, reason)
     Widget		w;
     caddr_t		tag;
     unsigned long	*reason;
{
    if(callbacks_called == (CALLBACK3_1 | CALLBACK3_2))
    {
	order_preserved++;
    }
    
    callbacks_called |= CALLBACK4_1;
}

void callback4_2(w, tag, reason)
     Widget		w;
     caddr_t		tag;
     unsigned long	*reason;
{
    if(callbacks_called == (CALLBACK3_1 | CALLBACK3_2 | CALLBACK4_1))
    {
	order_preserved++;
    }
    
    callbacks_called |= CALLBACK4_2;
}

void callback5_1(w, tag, reason)
     Widget		w;
     caddr_t		tag;
     unsigned long	*reason;
{
    callbacks_called |= CALLBACK5_1;
}

void callback5_2(w, tag, reason)
     Widget		w;
     caddr_t		tag;
     unsigned long	*reason;
{
    callbacks_called |= CALLBACK5_2;
}

void callback5_3(w, tag, reason)
     Widget		w;
     caddr_t		tag;
     unsigned long	*reason;
{
    callbacks_called |= CALLBACK5_3;
}

void callback6_1(w, tag, reason)
     Widget		w;
     caddr_t		tag;
     unsigned long	*reason;
{
    callbacks_called |= CALLBACK6_1;
}

void callback6_2(w, tag, reason)
     Widget		w;
     caddr_t		tag;
     unsigned long	*reason;
{
    callbacks_called |= CALLBACK6_2;
}

void callback7_1(w, tag, reason)
     Widget		w;
     caddr_t		tag;
     unsigned long	*reason;
{
    callbacks_called |= CALLBACK7_1;
}

void callback8_1(w, tag, reason)
     Widget		w;
     caddr_t		tag;
     unsigned long	*reason;
{
    callbacks_called |= CALLBACK8_1;
}

void callback8_2(w, tag, reason)
     Widget		w;
     caddr_t		tag;
     unsigned long	*reason;
{
    callbacks_called |= CALLBACK8_2;
}

void callback8_3(w, tag, reason)
     Widget		w;
     caddr_t		tag;
     unsigned long	*reason;
{
    callbacks_called |= CALLBACK8_3;
}

void callback8_4(w, tag, reason)
     Widget		w;
     caddr_t		tag;
     unsigned long	*reason;
{
    callbacks_called |= CALLBACK8_4;
}

void callback8_5(w, tag, reason)
     Widget		w;
     caddr_t		tag;
     unsigned long	*reason;
{
    callbacks_called |= CALLBACK8_5;
}

void callback9_1(w, tag, reason)
     Widget		w;
     caddr_t		tag;
     unsigned long	*reason;
{
    callbacks_called |= CALLBACK9_1;
}

void callback9_2(w, tag, reason)
     Widget		w;
     caddr_t		tag;
     unsigned long	*reason;
{
    callbacks_called |= CALLBACK9_2;
}

void callback9_3(w, tag, reason)
     Widget		w;
     caddr_t		tag;
     unsigned long	*reason;
{
    callbacks_called |= CALLBACK9_3;
}

void callback9_4(w, tag, reason)
     Widget		w;
     caddr_t		tag;
     unsigned long	*reason;
{
    callbacks_called |= CALLBACK9_4;
}

void callback10_1(w, tag, reason)
     Widget		w;
     caddr_t		tag;
     unsigned long	*reason;
{
    callbacks_called |= CALLBACK10_1;
}

void callback10_2(w, tag, reason)
     Widget		w;
     caddr_t		tag;
     unsigned long	*reason;
{
    callbacks_called |= CALLBACK10_2;
}

void callback10_3(w, tag, reason)
     Widget		w;
     caddr_t		tag;
     unsigned long	*reason;
{
    callbacks_called |= CALLBACK10_3;
}

void callback11_1(w, tag, reason)
     Widget		w;
     caddr_t		tag;
     unsigned long	*reason;
{
    if(callbacks_called & CALLBACK11_1_1)
    {
	callbacks_called |= CALLBACK11_1_2;
    }
    else
    {
	callbacks_called |= CALLBACK11_1_1;
    }
}

void callback11_2(w, tag, reason)
     Widget		w;
     caddr_t		tag;
     unsigned long	*reason;
{
    callbacks_called |= CALLBACK11_2;
}

void callback12_1(w, tag, reason)
     Widget		w;
     caddr_t		tag;
     unsigned long	*reason;
{
    if(callbacks_called & CALLBACK12_1_1)
    {
	callbacks_called |= CALLBACK12_1_2;
    }
    else
    {
	callbacks_called |= CALLBACK12_1_1;
    }
}

void callback12_2(w, tag, reason)
     Widget		w;
     caddr_t		tag;
     unsigned long	*reason;
{
    callbacks_called |= CALLBACK12_2;
}
