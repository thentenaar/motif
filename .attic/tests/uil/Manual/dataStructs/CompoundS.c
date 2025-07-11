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
static char rcsid[] = "$XConsortium: CompoundS.c /main/4 1995/07/13 20:33:27 drk $"
#endif
#endif

#include "CompoundS.h"
#include <Xm/Label.h>

void		dump_strings();
void		print_segment();

static Boolean	no_dump = True;

main(argc, argv)
     int argc;
     char *argv[];
{ 	
    int		i, j,
    		errors = 0,
    		total_errors = 0;

    char	mrm_err[25];
    char	msg[200];

    String	null_str = "";

    Arg		args[1];
    int		nargs;

/*
 * Initialize Mrm
 */
    MrmInitialize();

/*
 *  Initialize the toolkit.  This call returns the id of the "toplevel" widget.
 */

    toplevel = XtInitialize("CompoundS",	/* application name	   */
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
 * Print header
 */

    fprintf(stdout,"\n\nCompound String Test Results:\n\n");

/*
 * Print creation methods for X and UIL compound strings
 */

    fprintf(stdout,"\nThe following table lists the X-generated ");
    fprintf(stdout,"compound strings\nand their creation methods:\n\n");
    
    fprintf(stdout,"Number\tCreation Method\n");
    fprintf(stdout,"------\t---------------\n\n");

    for(i=0 ; i<=1 ; i++)
    {
	fprintf(stdout,"   %1d  \t%s\n", i+1, X_method[i]);
    }
    
    fprintf(stdout,"\n\nThe following table lists the UIL-generated ");
    fprintf(stdout,"compound strings\nand their creation methods:\n\n");
    
    fprintf(stdout,"Number\tCreation Method\n");
    fprintf(stdout,"------\t---------------\n\n");

    for(i=0 ; i<=2 ; i++)
    {
	fprintf(stdout,"   %1d  \t%s\n", i+1, UIL_method[i]);
    }
    
/*
 * Part 1
 */

/*
 * Create a compound string with XmStringCreate
 */

    X_compound_string1[0] = XmStringCreate(X_string, X_charset);

/*
 * Create a compound string with XmStringCreateLtoR
 */

    X_compound_string1[1] = XmStringCreateLtoR(X_string, X_charset);

/*
 * Fetch three compound strings, all of which should be the same, but
 * all of which were created in different ways
 */

    for(i=0 ; i<=2 ; i++)
    {
	if(MrmFetchLiteral(s_MrmHierarchy,		 /* hierarchy id    */
			   UIL_compound_string_names[i], /* UIL name	    */
			   dsp,				 /* display	    */
			   (XtPointer) &UIL_compound_string1[i],	 /* compound string */
			   &type) != MrmSUCCESS)	 /* type	    */
	{
	    sprintf(mrm_err, "Can't fetch UIL compound string %s",
		    UIL_compound_string_names[i]);
	    error_proc(mrm_err);
	}
    }

/*
 * Compare X-generated compound strings to X-generated compound strings,
 * UIL-generated compound strings to UIL-generated compound strings, and
 * UIL-generated compound strings to X-generated compound strings and
 * print the results
 */

    fprintf(stdout,"\n\n");
    fprintf(stdout,"Part 1: X-generated compound strings\n");
    fprintf(stdout,"------                vs\n");
    fprintf(stdout,"        UIL-generated compound strings\n\n");
    
    fprintf(stdout,"\n");
    fprintf(stdout,"         \t   X    \t   X\n");
    fprintf(stdout,"Identical\tCreation\tCreation\n");
    fprintf(stdout,"  Tables \t Method \t Method \tError\n");
    fprintf(stdout,"---------\t--------\t--------\t-----\n\n");

    if(XmStringCompare(X_compound_string1[0], X_compound_string1[1]))
    {
	fprintf(stdout,"   Yes   \t    1   \t    2\n");
    }
    else
    {
	errors++;
	
	fprintf(stdout,"   NO    \t    1   \t    2   \t  *\n");
    }	
		
    fprintf(stdout,"\n");
    fprintf(stdout,"         \t  UIL   \t  UIL\n");
    fprintf(stdout,"Identical\tCreation\tCreation\n");
    fprintf(stdout,"  Tables \t Method \t Method \tError\n");
    fprintf(stdout,"---------\t--------\t--------\t-----\n\n");

    for(i=0 ; i<=2 ; i++)
    {
	if(XmStringCompare(UIL_compound_string1[i],
			   UIL_compound_string1[(i+1)%3]))
	{
	    fprintf(stdout,"   Yes   \t    %1d   \t    %1d\n", i+1,
		    ((i+1)%3)+1);
	}
	else
	{
	    errors++;
	    
	    fprintf(stdout,"   NO   \t    %1d   \t    %1d   \t  *\n", i+1,
		    ((i+1)%3)+1);
	}
    }

    fprintf(stdout,"\n");
    fprintf(stdout,"         \t  UIL   \t   X\n");
    fprintf(stdout,"Identical\tCreation\tCreation\n");
    fprintf(stdout,"  Tables \t Method \t Method \tError\n");
    fprintf(stdout,"---------\t--------\t--------\t-----\n\n");

    for(i=0 ; i<=2 ; i++)
    {
	for(j=0 ; j<=1 ; j++)
	{
	    if(XmStringCompare(UIL_compound_string1[i],
			       X_compound_string1[j]))
	    {
		fprintf(stdout,"   Yes   \t    %1d   \t    %1d\n", i+1, j+1);
	    }
	    else
	    {
		errors++;
		
		fprintf(stdout,"   NO    \t    %1d   \t    %1d   \t  *\n", i+1,
			j+1);
	    }
	}
    }

    if(errors)
    {
	fprintf(stdout,"\nPart 1 failed with %d errors.\n", errors);

	if(!no_dump)
	{
	    fprintf(stdout, "\n");
	    fprintf(stdout, "--------- Error diagnostic report ---------\n\n");
	
	    fprintf(stdout, "The following table lists the individual");
	    fprintf(stdout, " components\nof the X-generated compound");
	    fprintf(stdout," strings:\n\n");

	    dump_strings(X_compound_string1, 2);

	    fprintf(stdout, "\n");
	    fprintf(stdout, "The following table lists the individual");
	    fprintf(stdout, " components\nof the UIL-generated compound");
	    fprintf(stdout," strings:\n\n");

	    dump_strings(UIL_compound_string1, 3);

	    fprintf(stdout, "\n");
	    fprintf(stdout, "-------------------------------------------\n\n");
	}

	total_errors += errors;
	errors = 0;
    }
    else
    {
	fprintf(stdout,"\nPart 1: test passed.\n");
    }

/*
 * Part 2
 */

/*
 * Create X widgets and fetch compound strings from them
 */

    for(i=0 ; i<=1 ; i++)
    {

	nargs = 0;
	XtSetArg(args[nargs], XmNlabelString, X_compound_string1[i]); nargs++;

	X_widgets[i] = XmCreateLabel(toplevel,
				    "X_widget",
				    args, nargs);

	nargs = 0;
	XtSetArg(args[nargs], XmNlabelString, &X_compound_string2[i]); nargs++;
	XtGetValues(X_widgets[i], args, nargs);
    }

/*
 * Fetch UIL widgets
 */

    for(i=0 ; i<=2 ; i++)
    {
	if(MrmFetchWidget(s_MrmHierarchy,		/* hierarchy id	     */
			   UIL_widget_names[i],		/* UIL name for font */
			   toplevel,			/* display	     */
			   &UIL_widgets[i],		/* font list	     */
			   &class) != MrmSUCCESS)	/* type		     */

	{
	    sprintf(mrm_err, "Can't fetch widget %s", UIL_widget_names[i]);
	    error_proc(mrm_err);
	}
	else
	{

/*
 * Get UIL compound strings from widgets
 */

	    nargs = 0;
	    XtSetArg(args[nargs], XmNlabelString, &UIL_compound_string2[i]);
	    nargs++;
	    XtGetValues(UIL_widgets[i], args, nargs);
	}
    }

/*
 * Compare X-generated compound strings to X-generated compound strings,
 * UIL-generated compound strings to UIL-generated compound strings, and
 * UIL-generated compound strings to X-generated compound strings and
 * print the results.  These compound strings are all arguments to widgets
 * and have been fetched from those widgets.
 */

    fprintf(stdout,"\n\n");
    fprintf(stdout,"Part 2: X-generated compound strings fetched from ");
    fprintf(stdout,"X-generated widgets\n");
    fprintf(stdout,"------                                vs\n");
    fprintf(stdout,"       UIL-generated compound strings fetched from ");
    fprintf(stdout,"UIL-generated widgets\n\n");
    
    fprintf(stdout,"\n");
    fprintf(stdout,"         \t   X    \t   X\n");
    fprintf(stdout,"Identical\tCreation\tCreation\n");
    fprintf(stdout,"  Tables \t Method \t Method \tError\n");
    fprintf(stdout,"---------\t--------\t--------\t-----\n\n");

    if(XmStringCompare(X_compound_string2[0], X_compound_string2[1]))
    {
	fprintf(stdout,"   Yes   \t    1   \t    2\n");
    }
    else
    {
	errors++;
    
	fprintf(stdout,"   NO    \t    1   \t    2   \t  *\n");
    }	
		
    fprintf(stdout,"\n");
    fprintf(stdout,"         \t  UIL   \t  UIL\n");
    fprintf(stdout,"Identical\tCreation\tCreation\n");
    fprintf(stdout,"  Tables \t Method \t Method \tError\n");
    fprintf(stdout,"---------\t--------\t--------\t-----\n\n");

    for(i=0 ; i<=2 ; i++)
    {
	if(XmStringCompare(UIL_compound_string2[i],
			   UIL_compound_string2[(i+1)%3]))
	{
	    fprintf(stdout,"   Yes   \t    %1d   \t    %1d\n", i+1,
		    ((i+1)%3)+1);
	}
	else
	{
	    errors++;
	    
	    fprintf(stdout,"   NO    \t    %1d   \t    %1d   \t  *\n", i+1,
		    ((i+1)%3)+1);
	}
    }

    fprintf(stdout,"\n");
    fprintf(stdout,"         \t  UIL   \t   X\n");
    fprintf(stdout,"Identical\tCreation\tCreation\n");
    fprintf(stdout,"  Tables \t Method \t Method \tError\n");
    fprintf(stdout,"---------\t--------\t--------\t-----\n\n");

    for(i=0 ; i<=2 ; i++)
    {
	for(j=0 ; j<=1 ; j++)
	{
	    if(XmStringCompare(UIL_compound_string2[i],
			       X_compound_string2[j]))
	    {
		fprintf(stdout,"   Yes   \t    %1d   \t    %1d\n", i+1, j+1);
	    }
	    else
	    {
		errors++;
		
		fprintf(stdout,"   NO    \t    %1d   \t    %1d   \t  *\n", i+1,
			j+1);
	    }
	}
    }

    if(errors)
    {
	fprintf(stdout,"\nPart 2 failed with %d errors.\n", errors);

	if(!no_dump)
	{
	    fprintf(stdout, "\n");
	    fprintf(stdout, "--------- Error diagnostic report ---------\n\n");
	
	    fprintf(stdout, "The following table lists the individual");
	    fprintf(stdout, " components\nof the X-generated compound");
	    fprintf(stdout," strings:\n\n");

	    dump_strings(X_compound_string2, 2);

	    fprintf(stdout, "\n");
	    fprintf(stdout, "The following table lists the individual");
	    fprintf(stdout, " components\nof the UIL-generated compound");
	    fprintf(stdout," strings:\n\n");

	    dump_strings(UIL_compound_string2, 3);

	    fprintf(stdout, "\n");
	    fprintf(stdout, "-------------------------------------------\n\n");
	}

	total_errors += errors;
    }
    else
    {
	fprintf(stdout,"\nPart 2: test passed.\n");
    }

/*
 * Part 3
 */
/*
 * Create the XStrings with XmStringSegmentCreate with the direction set
 * appropriately. The strings as we have set them up have already been 
 * parsed so they may look differently than the uil representation but
 * the XmStrings themselves should be the same.
 */
    for(i=0 ; i<=8 ; i++)
	{
        X_compound_string3[i] = XmStringSegmentCreate(X_direction_strings[i],
						      X_direction_charset[i],
						      X_rtol_direction[i],
						      False);
	}

/*
 * Fetch eight compound strings.
 */

    for(i=0 ; i<=8 ; i++)
    {
	if(MrmFetchLiteral(s_MrmHierarchy,		 /* hierarchy id    */
			   UIL_direction_names[i],       /* UIL name	    */
			   dsp,				 /* display	    */
			   (XtPointer) &UIL_compound_string3[i],	 /* compound string */
			   &type) != MrmSUCCESS)	 /* type	    */
	{
	    sprintf(mrm_err, "Can't fetch UIL compound string %s",
		    UIL_compound_string_names[i]);
	    error_proc(mrm_err);
	}
    }

/*
 * UIL-generated compound strings to UIL-generated compound strings and
 * print the results
 */

    fprintf(stdout,"\n\n");
    fprintf(stdout,"Part 3: X-generated compound strings\n");
    fprintf(stdout,"------                vs\n");
    fprintf(stdout,"        UIL-generated compound strings\n\n");
    
    fprintf(stdout,"\n");
    fprintf(stdout,"         \t  UIL\n");
    fprintf(stdout,"Identical\tCS String\n");
    fprintf(stdout,"  Tables \t  Name  \tError\n");
    fprintf(stdout,"---------\t--------\t-----\n\n");

    for(i=0 ; i<=8 ; i++)
    {
    if(XmStringCompare(UIL_compound_string3[i],
		       X_compound_string3[i]))
	{
	fprintf(stdout,"   Yes   \t%s\t\n", UIL_direction_names[i]);
	}
    else
	{
	errors++;
	fprintf(stdout,"   NO    \t%s\t  *\n", UIL_direction_names[i]);
	}
    }

    if(errors)
    {
	fprintf(stdout,"\nPart 3 failed with %d errors.\n", errors);

	if(!no_dump)
	{
	    fprintf(stdout, "\n");
	    fprintf(stdout, "--------- Error diagnostic report ---------\n\n");
	
	    fprintf(stdout, "The following table lists the individual");
	    fprintf(stdout, " components\nof the X-generated compound");
	    fprintf(stdout," strings:\n\n");

	    dump_strings(X_compound_string3, 9);

	    fprintf(stdout, "\n");
	    fprintf(stdout, "The following table lists the individual");
	    fprintf(stdout, " components\nof the UIL-generated compound");
	    fprintf(stdout," strings:\n\n");

	    dump_strings(UIL_compound_string3, 9);

	    fprintf(stdout, "\n");
	    fprintf(stdout, "-------------------------------------------\n\n");
	}

	total_errors += errors;
	errors = 0;
    }
    else
    {
	fprintf(stdout,"\nPart 3: test passed.\n");
    }

/*
 * Print error totals
 */

    if(total_errors)
    {
	fprintf(stdout,"\nCompound String: test failed with %d errors.\n\n",
		total_errors);
    }
    else
    {
	fprintf(stdout,"\nCompound String: all tests passed\n\n");
    }
    
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


void dump_strings(string, n)
     XmString	string[];
     int	n;

/*
 * This routine is provided for debugging and validation convenience.  It
 * does a complete dump of each string, inviting comparison of the segments
 * and components, in order to identify the reason an XmStringCompare
 * failed. This routine no-ops if the global no_dump is TRUE.
 */

{
    Boolean		segment,
    			separator;
    XmStringDirection	direction;
    XmStringCharSet	*charset;
    XmStringContext	context;
    char		*text;
    int			segment_number = 0,
    			i, j,
    			next;

    fprintf(stdout,"String   Segment\n");
    fprintf(stdout,"Number   Number    Text     Charset     Direction");
    fprintf(stdout,"                    Separator\n");
    fprintf(stdout,"------   -------   ----     -------     ---------");
    fprintf(stdout,"                    ---------\n");

    for(i=0 ; i<n ; i++)
    {
	fprintf(stdout ,"   %1d  ", i+1);

	if(!XmStringInitContext(&context, string[i]))
	{
	    fprintf(stdout, "   ill-formed compound string %d\n", i);
	}
	else
	{
	    segment = XmStringGetNextSegment(context, &text, charset, 
					     &direction, &separator);

	    segment_number = 0;

	    while(segment)
	    {
		if(segment_number)
		{
		    fprintf(stdout, "      ");
		}

		fprintf(stdout, "      %1d   ", segment_number+1);

		fprintf(stdout, "   %s", text);

		for(j=0 ; j<(9-strlen(text)) ; j++)
		{
		    fprintf(stdout, " ");
		}

		fprintf(stdout, "%s", charset);

		for(j=0 ; j<(12-strlen((char *)charset)) ; j++)
		{
		    fprintf(stdout, " ");
		}

		switch(direction)
		{
		  case XmSTRING_DIRECTION_L_TO_R:
		    fprintf(stdout, "XmSTRING_DIRECTION_L_TO_R ");
		    break;
		  case XmSTRING_DIRECTION_R_TO_L:
		    fprintf(stdout, "XmSTRING_DIRECTION_R_TO_L ");
		    break;
		  case XmSTRING_DIRECTION_DEFAULT:
		    fprintf(stdout, "XmSTRING_DIRECTION_DEFAULT");
		    break;
		  default:
		    fprintf(stdout, "Undefined string direction");
		    break;
		}

		if(separator)
		{
		    fprintf(stdout, "     True\n");
		}
		else
		{
		    fprintf(stdout, "     False\n");
		}

		segment_number++;

		segment = XmStringGetNextSegment(context, &text, charset,
						 &direction, &separator);
	    }
	}
    }

    fprintf(stdout,"\n");
}


void print_segment(segment, segment_number, text, charset, direction,
		   separator)
     Boolean		segment[2];
     int		segment_number;
     char		*text[2];
     XmStringCharSet	charset[2];
     XmStringDirection	direction[2];
     Boolean		separator[2];

/*
 * This routine dumps a segment to stdout
 */

{
    int	i, j;
    
    fprintf(stdout,"                              Segment %d\n",
	    segment_number);

    fprintf(stdout ,"text     :");

    for(i=0 ; i<=1 ; i++)
    {
	if(segment[i])
	{
	    fprintf(stdout, "     %s", text[i]);

	    for(j=0 ; j<(26-strlen(text[i])) ; j++)
	    {
		fprintf(stdout, " ");
	    }
	}
	else
	{
	    fprintf(stdout, "                               ");
	}
    }
    
    fprintf(stdout,"\ncharset  :");

    for(i=0 ; i<=1 ; i++)
    {
	if(segment[i])
	{
	    fprintf(stdout, "     %s", charset[i]);

	    for(j=0 ; j<(26-strlen(charset[i])) ; j++)
	    {
		fprintf(stdout, " ");
	    }
	}
	else
	{
	    fprintf(stdout, "                               ");
	}
    }
    
    fprintf(stdout,"\ndirection:");

    for(i=0 ; i<=1 ; i++)
    {
	if(segment[i])
	{
	    switch(direction[i])
	    {
	      case XmSTRING_DIRECTION_L_TO_R:
		fprintf(stdout, "     XmSTRING_DIRECTION_L_TO_R ");
		break;
	      case XmSTRING_DIRECTION_R_TO_L:
		fprintf(stdout, "     XmSTRING_DIRECTION_R_TO_L ");
		break;
	      case XmSTRING_DIRECTION_DEFAULT:
		fprintf(stdout, "     XmSTRING_DIRECTION_DEFAULT");
		break;
	      default:
		fprintf(stdout, "     Undefined string direction");
		break;
	    }
	}
	else
	{
	    fprintf(stdout, "                               ");
	}
    }    

    fprintf(stdout, "\nseparator:");

    for(i=0 ; i<=1 ; i++)
    {
	if(segment[i])
	{
	    if(separator[i])
	    {
		fprintf(stdout, "     True                      ");
	    }
	    else
	    {
		fprintf(stdout, "     False                     ");
	    }
	}
	else
	{
	    fprintf(stdout, "                               ");
	}
    }
}
    
