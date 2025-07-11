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
static char rcsid[] = "$XConsortium: ForRef.c /main/4 1995/07/13 20:37:15 drk $"
#endif
#endif
  
#include <stdio.h>

#include <testlib.h> 
#include <Mrm/MrmAppl.h>
  
  /*
   * Name of uid file
   */
  
String		filename_vec[] =
{
  "ForRef.uid"
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
MrmCode		type;

Widget		toplevel;

Display		*dsp;
Colormap	cmap;

XColor		rgb_db_def,
  colorcell_def;

Boolean		*UIL_boolean_literal,
  X_boolean_literal;

Pixel		UIL_color_literal,
  X_color_literal;

double		*UIL_float_literal,
  X_float_literal;

int		*UIL_integer_literal,
  X_integer_literal;

String		UIL_string_literal,
  X_string_literal;

int	       	*temp_table_ptr,
  *UIL_integer_table,
  X_integer_table[3];

String		*UIL_asciz_table,
  X_asciz_table[3];

String		*UIL_translation_table,
  X_translation_table[2];

int	integer[4] = 
{
  10,
  -2,
  99,
  -12
};

double		cfloat[2] =
{
  2.14159,
  1
};

String		string[7] =
{
  "This is \0                        ",
  "a concatenated string",
  "string2",
  "string3",
  "string4",
  "<Btn1Down>: set()",
  "<Btn1Up>: notify() unset()"
};

int		integer_exp[4];
String		string_exp[1];
double		float_exp[2];
double		temp[5];

String		integer_exp_names[4] = 
{
  "integer_exp0",
  "integer_exp1",
  "integer_exp2",
  "integer_exp3"
};

String		float_exp_names[2] = 
{
  "float_exp0",
  "float_exp1"
};

String		string_exp_names[1] =
{
  "string_exp1"
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
   *  Initialize the toolkit.  This call returns the id of the "Shell1"
   *  widget.  The applications "main" widget must be the only child
   *  of this widget.
   */

  CommonTestUilInit(argc, argv);

  
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
  
  dsp = XtDisplay(Shell1);
  
  cmap = DefaultColormap(dsp, DefaultScreen(dsp));
  
  /*
   * Print header for test results
   */
  
  fprintf(stdout,"\n\nForward Reference Test Results:\n\n");
  fprintf(stdout,"    UIL Value  \t\tC Value\t\tError\n");
  fprintf(stdout,"    -----------\t\t-------\t\t-----\n");
  
  
  /*
   * Fetch UIL integer expression values and test against equivalent C value
   */
  integer_exp [1] = integer [2] + integer [0];
  integer_exp [0] = integer_exp [1] + integer [3];
  integer_exp [2] = integer_exp [1] + integer [3];
  integer_exp [3] = integer [0] + integer_exp [0];
  
  for(i=0 ; i<=3 ; i++)
    {
      if(MrmFetchLiteral(s_MrmHierarchy,
			 integer_exp_names[i],
			 dsp,
			 (XtPointer *)&UIL_integer_literal,
			 &type) != MrmSUCCESS)
	{
	  error_proc("Can't fetch expression value");
	}
      
      fprintf(stdout,"\t%d\t\t%d", *UIL_integer_literal,
	      integer_exp[i]);
      
      if(*UIL_integer_literal != integer_exp[i])
	{
	  errors++;
	  fprintf(stdout,"\t\t  *\n");
	}
      else
	{
	  fprintf(stdout,"\n");
	}
    }
  
  
  /*
   * Fetch UIL float expression values and test against equivalent C value
   */
  temp [4] = 3.2;
  temp [3] = temp [4] - integer [1] * cfloat [1];
  temp [1] = temp [3];
  temp [2] = temp [1];
  temp [0] = temp[2];
  float_exp [1] = temp [0];
  float_exp [0] = cfloat [0] + cfloat [1];
  
  for(i=0 ; i<=1 ; i++)
    {
      if(MrmFetchLiteral(s_MrmHierarchy,
			 float_exp_names[i],
			 dsp,
			 (XtPointer *)&UIL_float_literal,
			 &type) != MrmSUCCESS)
	{
	  error_proc("Can't fetch expression value");
	}
      
      fprintf(stdout,"\t%f\t%f", *UIL_float_literal,
	      float_exp[i]);
      
      if(*UIL_float_literal != float_exp[i])
	{
	  errors++;
	  fprintf(stdout,"\t\t  *\n");
	}
      else
	{
	  fprintf(stdout,"\n");
	}
    }
  
  
  /*
   * Fetch UIL string expression values and test against equivalent C value
   */
  string_exp [0] = strcat (string [0], string [1]);
  
  for(i=0 ; i<=0 ; i++)
    {
      if(MrmFetchLiteral(s_MrmHierarchy,
			 string_exp_names[i],
			 dsp,
			 (XtPointer *)&UIL_string_literal,
			 &type) != MrmSUCCESS)
	{
	  error_proc("Can't fetch expression value");
	}
      
      fprintf(stdout,"\t%s\t%s", UIL_string_literal,
	      string_exp[i]);
      
      if (strcmp(UIL_string_literal, string_exp[i]) != 0)
	{
	  errors++;
	  fprintf(stdout,"\t\t  *\n");
	}
      else
	{
	  fprintf(stdout,"\n");
	}
    }
  
  
  /*
   * Fetch UIL integer table values and test against equivalent C value
   */
  X_integer_table [0] = integer [0];
  X_integer_table [1] = integer [2];
  X_integer_table [2] = integer [2] + integer [3];
  
  if (MrmFetchLiteral(s_MrmHierarchy,
		      "integer_table0",
		      dsp,
		      (XtPointer *)&temp_table_ptr,
		      &type) != MrmSUCCESS)
    error_proc("Can't fetch expression value");
  UIL_integer_table = (int *)temp_table_ptr;
  
  for(i=0 ; i<=2 ; i++)
    {
      fprintf(stdout,"\t%d\t\t%d", UIL_integer_table [i],
	      X_integer_table [i]);
      
      if (UIL_integer_table [i] != X_integer_table [i])
	{
	  errors++;
	  fprintf(stdout,"\t\t  *\n");
	}
      else
	{
	  fprintf(stdout,"\n");
	}
    }
  
  
  /*
   * Fetch UIL asciz string table values and test against equivalent C value
   */
  X_asciz_table [0] = (char *)(string [2]);
  X_asciz_table [1] = (char *)(string [3]);
  X_asciz_table [2] = (char *)(string [4]);
  
  if (MrmFetchLiteral(s_MrmHierarchy,
		      "asciz_table0",
		      dsp,
		      (XtPointer *)&temp_table_ptr,
		      &type) != MrmSUCCESS)
    error_proc("Can't fetch expression value");
  UIL_asciz_table = (String *)temp_table_ptr;
  
  for(i=0 ; i<=2 ; i++)
    {
      fprintf(stdout,"\t%s\t\t%s", UIL_asciz_table [i],
	      X_asciz_table [i]);
      
      if (strcmp(UIL_asciz_table [i], X_asciz_table [i]) != 0)
	{
	  errors++;
	  fprintf(stdout,"\t\t  *\n");
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
      fprintf(stdout, "\nForward refs: test failed with %d errors.\n\n", errors);
    }
  else
    {
      fprintf(stdout, "\nForward refs: all tests passed.\n\n");
    }
  /*
   * Close Mrm Hierarchy
   */
  
  MrmCloseHierarchy(s_MrmHierarchy);

  CommonPause();

  CommonPause();   /* Exit */
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
