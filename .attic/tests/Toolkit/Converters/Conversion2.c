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
static char rcsid[] = "$TOG: Conversion2.c /main/10 1999/12/03 18:40:25 jff $"
#endif

/*
*  Test the resource converters contained in Converters.c
 *
 *  As of 4/26/89 this does not test:
 *     _XmCvtStringToChar
 *     _XmCvtStringToXmString
 *     _XmCvtStringToXmFontList
 *     _XmCvtStringToXmStringTable
 */


#include <stdio.h>
#include <testlib.h>

typedef struct _ConverterRec *ConverterPtr;

typedef struct _ConverterRec
{
  ConverterPtr       next;
  XrmRepresentation  from, to;
  XtConverter        converter;
  XtConvertArgList   convert_args;
  Cardinal           num_args;
} ConverterRec;

extern void XmCvtStringToUnitType ();

#define UNKNOWN  -99
#define VARIABLE -999

/*
 * Structure to define a resource and its values
 */

typedef struct
{
  char	*type;
  char *from_string;
  int   correct_value;
} ConvertVal;

/*
 *  Table of convert structures
 */

static ConvertVal convert_set[] =
{
  {XmRBoolean            , "false"            , FALSE		    },
  {XmRBoolean            , "true"             , TRUE		    },
  {XmRBoolean            , "foo bar"          , UNKNOWN	    },

  {XmRBool               , "false"            , FALSE		    },
  {XmRBool               , "true"             , TRUE		    },
  {XmRBool               , "foo bar"          , UNKNOWN	    },

  {XmRCursor             , "X_cursor"         , VARIABLE	    },
  {XmRCursor             , "foo bar"          , UNKNOWN	    },

  {XmRFile               , "Conversion2.c"    , VARIABLE	    },
  {XmRFile               , "foo bar"          , UNKNOWN	    },

  {XmRInt                , "1234"             , 1234		    },
  {XmRInt                , "-1234"            , -1234		    },
  {XmRInt                , "foo bar"          , UNKNOWN	    },

  {XmRShort              , "1234"             , 1234		    },
  {XmRShort              , "-1234"            , -1234		    },
  {XmRShort              , "foo bar"          , UNKNOWN	    },

  {XmRUnsignedChar       , "127"              , 127		    },
  {XmRUnsignedChar       , "foo bar"          , UNKNOWN	    },

  {XmRPixel              , "foo bar"          , UNKNOWN	    },

  {XmRDeleteResponse     , "destroy"          , XmDESTROY	    },
  {XmRDeleteResponse     , "unmap"            , XmUNMAP	    },
  {XmRDeleteResponse     , "do_nothing"       , XmDO_NOTHING	    },
  {XmRDeleteResponse     , "foo bar"          , UNKNOWN	    },

  {XmRKeyboardFocusPolicy, "explicit"         , XmEXPLICIT	    },
  {XmRKeyboardFocusPolicy, "pointer"          , XmPOINTER	    },
  {XmRKeyboardFocusPolicy, "foo bar"          , UNKNOWN	    },

#ifdef MOTIF1_1
  {XmRShellUnitType      , "pixels"           , XmPIXELS           },
  {XmRShellUnitType      , "100th_millimeters", Xm100TH_MILLIMETERS},
  {XmRShellUnitType      , "1000th_inches"    , Xm1000TH_INCHES    },
  {XmRShellUnitType      , "100th_points"     , Xm100TH_POINTS	    },
  {XmRShellUnitType      , "100th_font_units" , Xm100TH_FONT_UNITS },
  {XmRShellUnitType      , "foo bar"          , UNKNOWN            },
#else
  {XmRUnitType      , "pixels"           , XmPIXELS           },
  {XmRUnitType      , "100th_millimeters", Xm100TH_MILLIMETERS},
  {XmRUnitType      , "1000th_inches"    , Xm1000TH_INCHES    },
  {XmRUnitType      , "100th_points"     , Xm100TH_POINTS	    },
  {XmRUnitType      , "100th_font_units" , Xm100TH_FONT_UNITS },
  {XmRShellUnitType      , "foo bar"          , UNKNOWN            },
#endif

  {XmRFontList, "8x13bold", VARIABLE}, /* returns some font id    */
  {XmRFontList, "foo bar", VARIABLE},  /* returns default font id */

  {NULL                  , NULL               , UNKNOWN            },
};


/*
 *  BuildFromVal - Build up the source of the conversion.
 */

BuildFromVal(XrmValuePtr from, char *string)
{
  from->size = strlen (string) + 1;
  from->addr = string;
}

/*
 *  PrintToVal - Print out the results of a conversion.
 */

PrintToVal(type, from, result, correct_value
#ifdef WORKAROUND
      )
#else
     , passed)
#endif
char *type;
XrmValuePtr from;
XrmValuePtr result;
int correct_value;
#ifndef WORKAROUND
Boolean passed;
#endif
{
  char	name[29];

  sprintf(name,"XmR%s", type);
  fprintf(stdout,"%22s   ", name);

#ifndef WORKAROUND
  if (!passed)
#else
  if (result->size == 0 && result->addr == NULL)
#endif
    {
      /*
       * A conversion did not occur.  If it happened because of bad
       * data, then the test passed.  Otherwise, the test failed.
       */

      if (correct_value == UNKNOWN)
	fprintf(stdout,"        Passed\n");
      else
	fprintf(stdout,"        FAILED\n");
      return;
    }  /* !passed */

  /*
   * Print the results of the conversion and check to see what has to 
   *  be done according to the size of the data being extracted from 
   *  the result structure.
   */

  if (result->size == 1)
    {
      if (correct_value == UNKNOWN)
	{
	  fprintf(stdout,"        FAILED\n");
	}
      else
	{
	  if (*((char *) (result->addr)) == correct_value)
	    {
	      fprintf(stdout,"        Passed\n");
	    }
	  else
	    {
	      fprintf(stdout,"        FAILED\n");
	    }
	}
    }
  else 
    if (result->size == 2)
      {
	if (correct_value == UNKNOWN)
	  {
	    fprintf(stdout, "        FAILED\n"); 
	  }
	else
	  {
	    if (*((short *) (result->addr)) == correct_value)
	      fprintf(stdout,"        Passed\n");
	    else
	      fprintf(stdout,"        FAILED\n");
	  }
      }
    else 
      {
	if (correct_value == VARIABLE)  
	  {
	    /*
	     * Returning some sort of font id, can't detect this, so
	     *  assume it passes.
	     */

	    fprintf(stdout,"        Passed\n");
	  }
	else 
	  {
	    if (correct_value == UNKNOWN)
	      {
		fprintf(stdout,"        FAILED\n");
	      }
	    else
	      {
		if (*((int *) (result->addr)) == correct_value)
		  {
		    fprintf(stdout,"        Passed\n");
		  }
		else
		  {
		    fprintf(stdout,"        FAILED\n");
		  }
	      }
	  }
      }
}


void main(argc, argv)
unsigned int	argc;
char *argv[];
{
  Boolean passed;
  Widget MessageDia;
  XrmValue from;
  XrmValue to_return;
  register int	i;
  XrmValue widgetname;

  static char MessageDialogName[] = "MessageDia";

  /*
   * Initialize toolkit
   */

  CommonTestInit(argc, argv);

  /*
   * Create a MessageDialog to recieve Vendor Shell resources to convert
   */

  MessageDia = XmCreateMessageDialog(Shell1, MessageDialogName, NULL, 0);

  CommonPause();

  /*
   * Print header
   */

  fprintf(stdout,"     Resource Type                Result   \n");
  fprintf(stdout,"    -----------------          -------------\n");

  /*
   *  Loop through the converter set and process each conversion
   */

  for (i=0 ; convert_set[i].type!=NULL ; i++)
    {
      BuildFromVal(&from, convert_set[i].from_string);

      /* reset to_return; XtConvert&Store expects this */
      to_return.addr = NULL;

#ifdef WORKAROUND

      XtConvert(MessageDia,		/* widget			*/
		XmRString,		/* source type			*/
		&from,			/* value to be converted	*/
		convert_set[i].type,	/* destination type		*/
		&to_return);		/* converted value		*/

      PrintToVal(convert_set[i].type, &from, &to_return, 
		 convert_set[i].correct_value);

#else   /* No workaround, shows Intrinsics bug in convert */

      passed = XtConvertAndStore(MessageDia,/* widget	       	*/
		XmRString,	  	/* source type		*/
		&from,		  	/* value to be converted*/
		convert_set[i].type,	/* destination type	*/
		&to_return);	  	/* converted value	*/

      PrintToVal(convert_set[i].type, &from, &to_return, 
		 convert_set[i].correct_value, passed);
#endif

    }

    /* special test: _XmCvtStringToWidget */



    widgetname.size = sizeof (MessageDialogName);
    widgetname.addr = MessageDialogName;

      XtConvert(MessageDia,		/* widget			*/
		XmRString,		/* source type			*/
		&widgetname,             /* value to be converted */
		XmRWidget,      	/* destination type		*/
		&to_return);		/* converted value		*/

#ifdef WORKAROUND

      PrintToVal("String", &widgetname, &to_return, 
		 MessageDia);
#else

      PrintToVal("String", &widgetname, &to_return, 
		 MessageDia, TRUE);
#endif
 
      fflush (stdout);

  CommonPause();
}
