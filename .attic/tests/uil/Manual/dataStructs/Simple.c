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
static char rcsid[] = "$XConsortium: Simple.c /main/4 1995/07/13 20:34:27 drk $"
#endif
#endif

#include <stdio.h>

#include <Mrm/MrmAppl.h>

/*
 * Name of uid file
 */

String		filename_vec[] =
{
    "dataS.uid"
};

MrmCount	filename_num =
(sizeof(filename_vec) / sizeof(filename_vec[0]));

/*
 * Convenience routine
 */

void		error_proc();

/*
 * Local declarations
 */

MrmHierarchy	s_MrmHierarchy;
MrmType		*class;
MrmCode		type;

Widget		toplevel;

Display		*dsp;
Colormap	cmap;

XColor		rgb_db_def,
		colorcell_def;

int		*UIL_boolean_literal,
		X_boolean_literal;

Pixel		UIL_color_literal,
		X_color_literal;

double		*UIL_float_literal,
		X_float_literal;

float		*UIL_single_float_literal,
                X_single_float_literal;

int		*UIL_integer_literal,
		X_integer_literal;

String		UIL_string_literal,
		X_string_literal;

String		UIL_keysym_literal;

Boolean		booleans[2] =
{
    True,
    False,
};

String		colors[5] =
{
    "black",
    "white",
    "red",
    "green",
    "blue",
};

XColor		RGB_colors[2] = 
{
  { 0, 0, 0, 0 },
  { 0, 65025, 12996, 7396 }
};

double		floats[4] =
{
    3.14159,
    6000,
    -4579,
    0,
};

float		sfloats[4] = 
{
  3.14159,
  6000,
  -4579,
  0,
};

int		integers[3] =
{
    0,
    -444,
    53,
};

String		strings[2] =
{
    "",
    "teSt String1",
};

char		keysyms[3] = 
{
  'Q',
  's',
  '3',
};

String		boolean_names[2] =
{
    "boolean1",
    "boolean2",
};

String		RGB_color_names[2] =
{
  "zilch",
  "coral",
};

String		float_names[4] =
{
    "float1",
    "float2",
    "float3",
    "float4",
};

String		sfloat_names[4] = 
{
  "sfloat1",
  "sfloat2",
  "sfloat3",
  "sfloat4",
};

String		string_names[2] =
{
    "string1",
    "string2",
};

String		keysym_names[3] = 
{
  "bigQ",
  "littleS",
  "three",
};

String		integer_names[3] =
{
    "integer1",
    "integer2",
    "integer3",
};

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

    toplevel = XtInitialize("Simple",		/* application name	   */
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
 * Get display, colormap
 */

    dsp = XtDisplay(toplevel);

    cmap = DefaultColormap(dsp, DefaultScreen(dsp));

/*
 * Print header for test results
 */

    fprintf(stdout,"\n\nSimple Test Results:\n\n");
    fprintf(stdout,"         Type\t  UIL Value  \tXt/Xlib Value\tError\n");
    fprintf(stdout,"         ----\t-------------\t-------------\t-----\n");

/*
 * Fetch UIL Boolean literals and test against equivalent Xt value
 */
    for(i=0 ; i<=1 ; i++)
    {
	if(MrmFetchLiteral(s_MrmHierarchy,
			   boolean_names[i],
			   dsp,
			   (XtPointer) &UIL_boolean_literal,
			   &type) != MrmSUCCESS)
	{
	    error_proc("Can't fetch boolean literal");
	}

	fprintf(stdout,"%13s\t%13d\t%13d", "Boolean", *UIL_boolean_literal,
		booleans[i]);

	if(*UIL_boolean_literal != booleans[i])
	{
	    errors++;
	    fprintf(stdout,"\t  *\n");
	}
	else
	{
	    fprintf(stdout,"\n");
	}
    }
    
/*
 * Fetch UIL Color literals and test against
 * equivalent Xlib colors (from rgb database)
 */

    fprintf(stdout, "\n");
    
    for(i=0 ; i<=4 ; i++)
    {
	XAllocNamedColor(dsp,		 /* display			 */
			 cmap,		 /* colormap			 */
			 colors[i],	 /* color name			 */
			 &colorcell_def, /* RGB & pixel values from cmap */
			 &rgb_db_def);   /* exact RGB values from db     */

	X_color_literal = colorcell_def.pixel;

	if(MrmFetchColorLiteral(s_MrmHierarchy,
				colors[i],
				dsp,
				cmap,
				&UIL_color_literal) != MrmSUCCESS)
	{
	    error_proc("Can't fetch color literal");
	}

	fprintf(stdout,"%13s\t%13d\t%13d", "Color", UIL_color_literal,
		X_color_literal);

	if(UIL_color_literal != X_color_literal)
	{
	    errors++;

	    fprintf(stdout,"\t  *\n");
	}
	else
	{
	    fprintf(stdout,"\n");
	}
    }

/*
 * Fetch UIL RGB Color literals and test against
 * equivalent Xlib colors generated with XAllocColor 
 */

    fprintf(stdout, "\n");
    
    for(i=0 ; i<=1 ; i++)
    {
      if (XAllocColor(dsp,		 /* display			 */
		      cmap,		 /* colormap			 */
		      &(RGB_colors[i]))	 /* RGB values			 */
	  != TRUE)
	{
	  error_proc("Can't allocate color");
	}
      
	X_color_literal = RGB_colors[i].pixel;

	if(MrmFetchColorLiteral(s_MrmHierarchy,
				RGB_color_names[i],
				dsp,
				cmap,
				&UIL_color_literal) != MrmSUCCESS)
	{
	    error_proc("Can't fetch color literal");
	}

	fprintf(stdout,"%13s\t%13d\t%13d", "RGB Color", UIL_color_literal,
		X_color_literal);

	if(UIL_color_literal != X_color_literal)
	{
	    errors++;

	    fprintf(stdout,"\t  *\n");
	}
	else
	{
	    fprintf(stdout,"\n");
	}
    }

/*
 * Fetch UIL Float literals and test against equivalent Xt value
 */

    fprintf(stdout,"\n");

    for(i=0 ; i<=3 ; i++)
    {
	if(MrmFetchLiteral(s_MrmHierarchy,
			   float_names[i],
			   dsp,
			   (XtPointer) &UIL_float_literal,
			   &type) != MrmSUCCESS)
	{
	    error_proc("Can't fetch float literal");
	}

	fprintf(stdout,"%13s\t%+8e\t%+8e", "Float", *UIL_float_literal,
		floats[i]);

	if(*UIL_float_literal != floats[i])
	{
	    errors++;

	    fprintf(stdout,"\t  *\n");
	}
	else
	{
	    fprintf(stdout,"\n");
	}
    }

/*
 * Fetch UIL Single_Float literals and test against equivalent Xt value
 */

    fprintf(stdout,"\n");

    for(i=0 ; i<=3 ; i++)
    {
	if(MrmFetchLiteral(s_MrmHierarchy,
			   sfloat_names[i],
			   dsp,
			   (XtPointer) &UIL_single_float_literal,
			   &type) != MrmSUCCESS)
	{
	    error_proc("Can't fetch single_float literal");
	}

	fprintf(stdout,"%13s\t%+8e\t%+8e", "Single_Float", 
		*UIL_single_float_literal, sfloats[i]);

	if(*UIL_single_float_literal != sfloats[i])
	{
	    errors++;

	    fprintf(stdout,"\t  *\n");
	}
	else
	{
	    fprintf(stdout,"\n");
	}
    }
    
/*
 * Fetch UIL Integer literals and test against equivalent Xt value
 */

    fprintf(stdout,"\n");

    for(i=0 ; i<=2 ; i++)
    {
	if(MrmFetchLiteral(s_MrmHierarchy,
			   integer_names[i],
			   dsp,
			   (XtPointer) &UIL_integer_literal,
			   &type) != MrmSUCCESS)
	{
	    error_proc("Can't fetch integer literal");
	}

	fprintf(stdout,"%13s\t%13d\t%13d", "Integer", *UIL_integer_literal,
		integers[i]);

	if(*UIL_integer_literal != integers[i])
	{
	    errors++;

	    fprintf(stdout,"\t  *\n");
	}
	else
	{
	    fprintf(stdout,"\n");
	}
    }
    
/*
 * Fetch UIL String literals and test against equivalent Xt value
 */

    fprintf(stdout,"\n");

    for(i=0 ; i<=1 ; i++)
    {
	if(MrmFetchLiteral(s_MrmHierarchy,
			   string_names[i],
			   dsp,
			   (XtPointer) &UIL_string_literal,
			   &type) != MrmSUCCESS)
	{
	    error_proc("Can't fetch string literal");
	}

	fprintf(stdout,"%13s\t%13s\t%13s", "String", UIL_string_literal,
		strings[i]);

	if(strcmp(UIL_string_literal, strings[i]) != 0)
	{
	    errors++;

	    fprintf(stdout,"\t  *\n");
	}
	else
	{
	    fprintf(stdout,"\n");
	}
    }

/*
 * Fetch UIL Character literals and test against equivalent Xt value
 */

    fprintf(stdout,"\n");

    for(i=0 ; i<=2 ; i++)
      {
	if (MrmFetchLiteral(s_MrmHierarchy,
			    keysym_names[i],
			    dsp,
			    (XtPointer) &UIL_keysym_literal,
			    &type) != MrmSUCCESS)
	  {	
	    error_proc("Can't fetch string literal");
	  }

	fprintf(stdout,"%13s\t%13c\t%13c", "Keysym", UIL_keysym_literal,
		keysyms[i]);

	if (UIL_keysym_literal != (String) keysyms[i])
	  {
	    errors++;

	    fprintf(stdout,"\t  *\n");
	  }
	else
	  {
	    fprintf(stdout,"\n");
	  }
      }
    
/*
 * Print error results
 */

    if(errors)
    {
	fprintf(stdout, "\nSimple: test failed with %d errors.\n\n", errors);
    }
    else
    {
	fprintf(stdout, "\nSimple: all tests passed.\n\n");
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
