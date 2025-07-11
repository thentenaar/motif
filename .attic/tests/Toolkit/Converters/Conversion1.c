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
static char rcsid[] = "$XConsortium: Conversion1.c /main/8 1995/07/13 17:52:50 drk $"
#endif
#endif


/*
 *  Test the resource converters contained in ResConvert.c
 *
 *  As of 4/26/89 this does not test:
 *     _XmCvtStringToChar
 *     _XmCvtStringToXmString
 *     _XmCvtStringToXmFontList
 *     _XmCvtStringToXmStringTable
 */


#include <stdio.h>
#include <testlib.h>

extern void XmCvtStringToUnitType();

#define UNKNOWN	-99

/* 
 * Structure to define a resource and its values
 */

typedef struct
{
  char *type;
  char	*from_string;
  int	 correct_value;
} ConvertVal;

/*
 * Table of convert structures
 */

static ConvertVal convert_set[] =
{
  {XmRArrowDirection, "arrow_up"                , XmARROW_UP        },
  {XmRArrowDirection, "arrow_down"              , XmARROW_DOWN      },
  {XmRArrowDirection, "arrow_left"              , XmARROW_LEFT      },
  {XmRArrowDirection, "arrow_right"             , XmARROW_RIGHT     },
  {XmRArrowDirection, "foo bar"                 , UNKNOWN           },
  
  {XmRPacking        , "pack_none"               , XmPACK_NONE       },
  {XmRPacking        , "pack_tight"              , XmPACK_TIGHT      },
  {XmRPacking        , "pack_column"             , XmPACK_COLUMN     },
  {XmRPacking        , "foo bar"                 , UNKNOWN           },

  {XmROrientation    , "vertical"                , XmVERTICAL        },
  {XmROrientation    , "horizontal"              , XmHORIZONTAL      },
  {XmROrientation    , "foo bar"                 , UNKNOWN           },
  
  {XmRAlignment      , "alignment_center"        , XmALIGNMENT_CENTER},
  {XmRAlignment      , "alignment_beginning"     , XmALIGNMENT_BEGINNING},
  {XmRAlignment      , "alignment_end"           , XmALIGNMENT_END   },
  {XmRAlignment      , "foo bar"                 , UNKNOWN           },

  {XmRAttachment     , "attach_none"             , XmATTACH_NONE     },
  {XmRAttachment     , "attach_form"             , XmATTACH_FORM     },
  {XmRAttachment     , "attach_opposite_form"    , XmATTACH_OPPOSITE_FORM},
  {XmRAttachment     , "attach_widget"           , XmATTACH_WIDGET   },
  {XmRAttachment     , "attach_opposite_widget"  , XmATTACH_OPPOSITE_WIDGET},
  {XmRAttachment     , "attach_position"         , XmATTACH_POSITION },
  {XmRAttachment     , "attach_self"             , XmATTACH_SELF     },
  {XmRAttachment     , "foo bar"                 , UNKNOWN           },

  {XmRLabelType      , "string"                  , XmSTRING          },
  {XmRLabelType      , "pixmap"                  , XmPIXMAP          },
  {XmRLabelType      , "foo bar"                 , UNKNOWN           },

  {XmRIndicatorType  , "n_of_many"               , XmN_OF_MANY       },
  {XmRIndicatorType  , "one_of_many"             , XmONE_OF_MANY     },
  {XmRIndicatorType  , "foo bar"                 , UNKNOWN           },

  {XmREditMode       , "multi_line_edit"         , XmMULTI_LINE_EDIT },
  {XmREditMode       , "single_line_edit"        , XmSINGLE_LINE_EDIT},
  {XmREditMode       , "foo bar"                 , UNKNOWN           },

  {XmRScrollingPolicy, "automatic"               , XmAUTOMATIC       },
  {XmRScrollingPolicy, "application_defined"     , XmAPPLICATION_DEFINED},
  {XmRScrollingPolicy, "foo bar"                 , UNKNOWN           },

  {XmRVisualPolicy   , "variable"                , XmVARIABLE        },
  {XmRVisualPolicy   , "constant"                , XmCONSTANT        },
  {XmRVisualPolicy   , "foo bar"                 , UNKNOWN           },

  {XmRScrollBarDisplayPolicy, "as_needed"               , XmAS_NEEDED       },
  {XmRScrollBarDisplayPolicy, "static"                  , XmSTATIC          },
  {XmRScrollBarDisplayPolicy, "foo bar"                 , UNKNOWN           },

  {XmRScrollBarPlacement , "top_left"                , XmTOP_LEFT        },
  {XmRScrollBarPlacement , "top_right"               , XmTOP_RIGHT       },
  {XmRScrollBarPlacement , "bottom_left"             , XmBOTTOM_LEFT     },
  {XmRScrollBarPlacement , "bottom_right"            , XmBOTTOM_RIGHT    },
  {XmRScrollBarPlacement , "foo bar"                 , UNKNOWN           },

  {XmRSelectionPolicy, "single_select"           , XmSINGLE_SELECT   },
  {XmRSelectionPolicy, "multiple_select"         , XmMULTIPLE_SELECT },
  {XmRSelectionPolicy, "extended_select"         , XmEXTENDED_SELECT },
  {XmRSelectionPolicy, "browse_select"           , XmBROWSE_SELECT   },
  {XmRSelectionPolicy, "foo bar"                 , UNKNOWN           },

  {XmRListSizePolicy , "variable"                , XmVARIABLE        },
  {XmRListSizePolicy , "constant"                , XmCONSTANT        },
  {XmRListSizePolicy , "foo bar"                 , UNKNOWN           },

  {XmRUnitType       , "pixels"                  , XmPIXELS          },
  {XmRUnitType       , "100th_millimeters"       , Xm100TH_MILLIMETERS},
  {XmRUnitType       , "1000th_inches"           , Xm1000TH_INCHES   },
  {XmRUnitType       , "100th_points"            , Xm100TH_POINTS    },
  {XmRUnitType       , "100th_font_units"        , Xm100TH_FONT_UNITS},
  {XmRUnitType       , "foo bar"                 , UNKNOWN           },
  
  {XmRProcessingDirection   , "max_on_top"              , XmMAX_ON_TOP      },
  {XmRProcessingDirection   , "max_on_bottom"           , XmMAX_ON_BOTTOM   },
  {XmRProcessingDirection   , "max_on_left"             , XmMAX_ON_LEFT     },
  {XmRProcessingDirection   , "max_on_right"            , XmMAX_ON_RIGHT    },
  {XmRProcessingDirection   , "foo bar"                 , UNKNOWN           },
  
  {XmRStringDirection , "string_direction_l_to_r" , XmSTRING_DIRECTION_L_TO_R},
  {XmRStringDirection, "string_direction_r_to_l" , XmSTRING_DIRECTION_R_TO_L},
  {XmRStringDirection, "foo bar"                 , UNKNOWN           },

  {XmRSeparatorType  , "no_line"                 , XmNO_LINE         },
  {XmRSeparatorType  , "single_line"             , XmSINGLE_LINE     },
  {XmRSeparatorType  , "double_line"             , XmDOUBLE_LINE     },
  {XmRSeparatorType  , "single_dashed_line"      , XmSINGLE_DASHED_LINE},
  {XmRSeparatorType  , "double_dashed_line"      , XmDOUBLE_DASHED_LINE},
  {XmRSeparatorType  , "shadow_etched_out"       , XmSHADOW_ETCHED_OUT},
  {XmRSeparatorType  , "shadow_etched_in"        , XmSHADOW_ETCHED_IN},
  {XmRSeparatorType  , "foo bar"                 , UNKNOWN           },
  
  {XmRShadowType     , "shadow_etched_in"        , XmSHADOW_ETCHED_IN},
  {XmRShadowType     , "shadow_etched_out"       , XmSHADOW_ETCHED_OUT},
  {XmRShadowType     , "shadow_in"               , XmSHADOW_IN       },
  {XmRShadowType     , "shadow_out"              , XmSHADOW_OUT      },
  {XmRShadowType     , "foo bar"                 , UNKNOWN           },

  {XmRWhichButton    , "button1"                 , Button1           },
  {XmRWhichButton    , "button2"                 , Button2           },
  {XmRWhichButton    , "button3"                 , Button3           },
  {XmRWhichButton    , "button4"                 , Button4           },
  {XmRWhichButton    , "button5"                 , Button5           },
  {XmRWhichButton    , "foo bar"                 , UNKNOWN           },

  {XmRResizePolicy   , "resize_none"             , XmRESIZE_NONE     },
  {XmRResizePolicy   , "resize_grow"             , XmRESIZE_GROW     },
  {XmRResizePolicy   , "resize_any"              , XmRESIZE_ANY      },
  {XmRResizePolicy   , "foo bar"                 , UNKNOWN           },

  {XmRDialogStyle    , "dialog_work_area"        , XmDIALOG_WORK_AREA},
  {XmRDialogStyle    , "dialog_modeless"         , XmDIALOG_MODELESS },
  {XmRDialogStyle    , "dialog_application_modal", XmDIALOG_APPLICATION_MODAL},
  {XmRDialogStyle    , "dialog_system_modal"     , XmDIALOG_SYSTEM_MODAL},
  {XmRDialogStyle    , "foo bar"                 , UNKNOWN           },

  {XmRDialogType     , "dialog_error"            , XmDIALOG_ERROR    },
  {XmRDialogType     , "dialog_information"      , XmDIALOG_INFORMATION},
  {XmRDialogType     , "dialog_message"          , XmDIALOG_MESSAGE  },
  {XmRDialogType     , "dialog_question"         , XmDIALOG_QUESTION },
  {XmRDialogType     , "dialog_warning"          , XmDIALOG_WARNING  },
  {XmRDialogType     , "dialog_working"          , XmDIALOG_WORKING  },
  {XmRDialogType     , "foo bar"                 , UNKNOWN           },
#ifdef MOTIF1_1
  {XmRDialogType     , "dialog_prompt"           , XmDIALOG_PROMPT   },
  {XmRDialogType     , "dialog_selection"        , XmDIALOG_SELECTION},
  {XmRDialogType     , "dialog_command"          , XmDIALOG_COMMAND  },
  {XmRDialogType     , "dialog_file_selection"   , XmDIALOG_FILE_SELECTION},
  {XmRDialogType     , "dialog_work_area"        , XmDIALOG_WORK_AREA},
#else
  {XmRDialogType     , "dialog_template"         , XmDIALOG_TEMPLATE },

  {XmRSelectionType     , "dialog_prompt"           , XmDIALOG_PROMPT   },
  {XmRSelectionType     , "dialog_selection"        , XmDIALOG_SELECTION},
  {XmRSelectionType     , "dialog_command"          , XmDIALOG_COMMAND  },
  {XmRSelectionType     , "dialog_file_selection"   , XmDIALOG_FILE_SELECTION},
  {XmRSelectionType     , "dialog_work_area"        , XmDIALOG_WORK_AREA},
  {XmRSelectionType     , "foo bar"                 , UNKNOWN           },
#endif

  {XmRDefaultButtonType , "dialog_ok_button"        , XmDIALOG_OK_BUTTON},
  {XmRDefaultButtonType , "dialog_cancel_button"    , XmDIALOG_CANCEL_BUTTON},
  {XmRDefaultButtonType , "dialog_help_button"      , XmDIALOG_HELP_BUTTON},
  {XmRDefaultButtonType , "foo bar"                 , UNKNOWN           },

  {XmRRowColumnType     , "work_area"               , XmWORK_AREA       },
  {XmRRowColumnType     , "menu_bar"                , XmMENU_BAR        },
  {XmRRowColumnType     , "menu_popup"              , XmMENU_POPUP      },
  {XmRRowColumnType     , "menu_pulldown"           , XmMENU_PULLDOWN   },
  {XmRRowColumnType     , "menu_option"             , XmMENU_OPTION     },
  {XmRRowColumnType     , "foo bar"                 , UNKNOWN           },

  {NULL                 , NULL                      , UNKNOWN           },
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

char		*type;
XrmValuePtr	from;
XrmValuePtr	result;
int		correct_value;
#ifndef WORKAROUND
Boolean 	passed;
#endif
{
  char	name[25];

  sprintf(name,"XmR%s", type);
  fprintf(stdout,"%25s %24s", name, from->addr);

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
	fprintf(stdout,"       NULL   NULL Passed\n");
      else
	fprintf(stdout," %8d       NULL FAILED\n", correct_value);
	
      return;
    }

  /*
   * Print the results of the conversion and check to see what has to 
   * be done according to the size of the data being extracted from 
   * the result structure.
   */

  if (result->size == 1)
    {
      if (correct_value == UNKNOWN)
	{
	  fprintf(stdout," %8d        NULL FAILED\n", *(result->addr));
	}
      else
	{
	  fprintf(stdout," %8d %8d", *(result->addr), correct_value);
	  
	  if(*((char *) (result->addr)) == correct_value)
	    {
	      fprintf(stdout," Passed\n");
	    }	
	  else
	    {
	      fprintf(stdout," FAILED\n");
	    }
	}
    }
  else 
    if (result->size == 2)
      {
	if (correct_value == UNKNOWN)
	  {
	    fprintf(stdout," %8d        NULL FAILED\n",
		    *((short *) (result->addr)));
	  }
	else
	  {
	    fprintf(stdout," %8d %8d", *((short *) (result->addr)),
		  correct_value);
	    
	    if (*((short *) (result->addr)) == correct_value)
	      {
		fprintf(stdout," Passed\n");
	      }
	    else
	      {
		fprintf(stdout," FAILED\n");
	      }
	  }
      }
    else 
      {
	if (correct_value == UNKNOWN)
	{
	  fprintf(stdout," %8d        NULL FAILED\n",
		  *((int *) (result->addr)));
	}
	else
	  {
	    fprintf(stdout," %8d %8d", *((int *) (result->addr)),
		    correct_value);
	    
	    if (*((int *) (result->addr)) == correct_value)
	      {
		fprintf(stdout," Passed\n");
	      }
	    else
	      {
		fprintf(stdout," FAILED\n");
	      }
	  }
      }
}

void main(argc, argv)
unsigned int	argc;
char		*argv[];
{
  Boolean passed;
  Widget bulletin;
  XrmValue from;
  XrmValue to_return;
  register int	i;

  /*
   * Initialize toolkit
   */

  CommonTestInit(argc, argv);

  /*
   * Need to create a widget to get the converters registered
   */

  bulletin = XmCreateBulletinBoard(Shell1,	/* parent	*/
				   "bulletin",	/* name		*/
				   NULL, 0);	/* arguments	*/

  XtManageChild (bulletin);

  /*
   * Add the unit type converter
   */

  XtAppAddConverter(app_context,	  /* application context */
		    XmRString,		  /* source type	 */
		    XmRUnitType,	  /* destination type	 */
		    XmCvtStringToUnitType,/* converter		 */
		    NULL, 0);		  /* arguments		 */

  CommonPause();

  /*
   * Print header
   */

  fprintf(stdout,
	  "                                                   Expected");
  fprintf(stdout,	  "   Actual\n");
  fprintf(stdout,
	  "     Resource Type                 String             Value");
  fprintf(stdout,"    Value Result\n");
  fprintf(stdout,
	  "-------------------------     --------------------  --------");
  fprintf(stdout,"   ----- ------\n");

  /*
   * Loop through the converter set and process each conversion
   */

  for (i=0 ; convert_set[i].type!=NULL ; i++)
    {
      BuildFromVal(&from, convert_set[i].from_string);

      /* reset to_return; XtConvert&Store expects this */
      to_return.addr = NULL;
      to_return.size = 0;

#ifdef WORKAROUND

      XtConvert(bulletin, 		/* widget                */
		XmRString,		/* source type 		 */
		&from,			/* value to be converted */
		convert_set[i].type,    /* destination type      */
		&to_return);		/* converted value       */

      PrintToVal(convert_set[i].type, &from, &to_return, 
		 convert_set[i].correct_value);

#else  /* No workaround, shows Instrinsics bug in convert */

      passed = XtConvertAndStore(bulletin,/* widget	       	*/
		XmRString,	  	/* source type		*/
		&from,		  	/* value to be converted*/
		convert_set[i].type,	/* destination type	*/
		&to_return);	  	/* converted value	*/

      PrintToVal(convert_set[i].type, &from, &to_return, 
		 convert_set[i].correct_value, passed);
#endif

    }

  CommonPause();
}



