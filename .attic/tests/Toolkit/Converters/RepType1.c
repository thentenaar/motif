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
static char rcsid[] = "$TOG: RepType1.c /main/8 1999/02/12 17:15:05 jff $"
#endif
#endif


/*
 *  This test tests the Representation Type Management facility in terms
 *  of its static registering capabilities.
 *
 *  Select approximately 25+ resources from various widgets
 *  (at least one in each, several in complicated widgets like
 *  RowColumn) (which fits the criteria of being a string-to-
 *  enumerated value) and attempt to convert them.  The idea is to 
 *  verify that the developers have put in the necessary hooks for 
 *  each of the widgets to allow the conversions.
 */


#include <testlib.h>

#ifndef MOTIF1_1

#include <Xm/RepType.h>

typedef struct
{
  char *rep_type;
  unsigned char num_of_tests;
} RepTypeVal;

static RepTypeVal reptypedata[] =
{ 
  {XmRAlignment, 3},
  {XmRArrowDirection, 4},
  {XmRAttachment, 7},
  {XmRAudibleWarning, 2},
  {XmRChildHorizontalAlignment, 3},
  {XmRChildPlacement, 3},
  {XmRChildType, 3},
  {XmRChildVerticalAlignment, 7},
  {XmRCommandWindowLocation, 2},
  {XmRDefaultButtonType, 4},
  {XmRDialogStyle, 6},
  {XmRDialogType, 7},
  {XmREditMode, 2},
  {XmRFileTypeMask, 3},
  {XmRIndicatorType, 4},
  {XmRLabelType, 2},
  {XmRListSizePolicy, 3},
  {XmRMultiClick, 2},
  {XmRNavigationType, 4},
  {XmROrientation, 2},
  {XmRPacking, 3},
  {XmRProcessingDirection, 4},
  {XmRResizePolicy, 3},
  {XmRRowColumnType, 5},
  {XmRScrollBarDisplayPolicy, 2},
  {XmRScrollBarPlacement, 4},
  {XmRScrollingPolicy, 2},
  {XmRSelectionPolicy, 4},
  {XmRSelectionType, 5},
  {XmRSeparatorType, 9},
  {XmRShadowType, 4},
  {XmRStringDirection, 2},
  {XmRTearOffModel, 2},
  {XmRUnpostBehavior, 2},
  {XmRVerticalAlignment, 5},
  {XmRVisualPolicy, 2},
  {0,0}
};

typedef struct
{
  char *from_string;
  unsigned char correct_value;
} ConvertVal;

static ConvertVal convert_set[] =
{
  /* XmRAlignment */
  { "alignment_beginning", XmALIGNMENT_BEGINNING},
  { "alignment_center", XmALIGNMENT_CENTER},
  { "alignment_end", XmALIGNMENT_END},

  /* XmRArrowDirection */
  { "arrow_up", XmARROW_UP},
  { "arrow_down", XmARROW_DOWN},
  { "arrow_left", XmARROW_LEFT},
  { "arrow_right", XmARROW_RIGHT},

  /* XmRAttachment */
  { "attach_none", XmATTACH_NONE},
  { "attach_form", XmATTACH_FORM},
  { "attach_opposite_form", XmATTACH_OPPOSITE_FORM},
  { "attach_widget", XmATTACH_WIDGET},
  { "attach_opposite_widget", XmATTACH_OPPOSITE_WIDGET},
  { "attach_position", XmATTACH_POSITION},
  { "attach_self", XmATTACH_SELF},

  /* XmRAudibleWarning */
  { "none", XmNONE},
  { "bell", XmBELL},

  /* XmRChildHorizontalAlignment */
  { "alignment_beginning", XmALIGNMENT_BEGINNING},
  { "alignment_center", XmALIGNMENT_CENTER},
  { "alignment_end", XmALIGNMENT_END},

  /* XmRChildPlacement */
  { "place_top", XmPLACE_TOP},
  { "place_above_selection", XmPLACE_ABOVE_SELECTION},
  { "place_below_selection", XmPLACE_BELOW_SELECTION},

  /* XmRChildType */
  { "frame_generic_child", XmFRAME_GENERIC_CHILD},
  { "frame_workarea_child", XmFRAME_WORKAREA_CHILD},
  { "frame_title_child", XmFRAME_TITLE_CHILD},

  /* XmRChildVerticalAlignment */
  { "alignment_baseline_top", XmALIGNMENT_BASELINE_TOP},
  { "alignment_center", XmALIGNMENT_CENTER},
  { "alignment_baseline_bottom", XmALIGNMENT_BASELINE_BOTTOM},
  { "alignment_widget_top", XmALIGNMENT_WIDGET_TOP},
  { "alignment_widget_bottom", XmALIGNMENT_WIDGET_BOTTOM},
  { "alignment_child_top", XmALIGNMENT_CHILD_TOP},
  { "alignment_child_bottom", XmALIGNMENT_CHILD_BOTTOM},
  
  /* XmRCommandWindowLocation */
  { "command_above_workspace", XmCOMMAND_ABOVE_WORKSPACE},
  { "command_below_workspace", XmCOMMAND_BELOW_WORKSPACE},
  
  /* XmRDefaultButtonType */
  { "dialog_none", XmDIALOG_NONE},
  { "dialog_cancel_button", XmDIALOG_CANCEL_BUTTON},
  { "dialog_ok_button", XmDIALOG_OK_BUTTON},
  { "dialog_help_button", XmDIALOG_HELP_BUTTON},

  /* XmRDialogStyle */
  { "dialog_modeless", XmDIALOG_MODELESS},
  { "dialog_work_area", XmDIALOG_WORK_AREA},
  { "dialog_primary_application_modal", XmDIALOG_PRIMARY_APPLICATION_MODAL},
  { "dialog_application_modal", XmDIALOG_APPLICATION_MODAL},
  { "dialog_full_application_modal", XmDIALOG_FULL_APPLICATION_MODAL},
  { "dialog_system_modal", XmDIALOG_SYSTEM_MODAL},
  
  /* XmRDialogType */
  { "dialog_template", XmDIALOG_TEMPLATE},
  { "dialog_error", XmDIALOG_ERROR},
  { "dialog_information", XmDIALOG_INFORMATION},
  { "dialog_message", XmDIALOG_MESSAGE},
  { "dialog_question", XmDIALOG_QUESTION},
  { "dialog_warning", XmDIALOG_WARNING},
  { "dialog_working", XmDIALOG_WORKING},
  
  /* XmREditMode */
  { "multi_line_edit", XmMULTI_LINE_EDIT},
  { "single_line_edit", XmSINGLE_LINE_EDIT},

  /* XmRFileTypeMask */
  { "file_directory", XmFILE_DIRECTORY},
  { "file_regular", XmFILE_REGULAR},
  { "file_any_type", XmFILE_ANY_TYPE},

  /* XmRIndicatorType */
  { "n_of_many", XmN_OF_MANY},
  { "one_of_many", XmONE_OF_MANY},
  { "one_of_many_round", XmONE_OF_MANY_ROUND},
  { "one_of_many_diamond", XmONE_OF_MANY_DIAMOND},

  /* XmRLabelType */
  { "pixmap", XmPIXMAP},
  { "string", XmSTRING},

  /* XmRListSizePolicy */
  { "variable", XmVARIABLE},
  { "constant", XmCONSTANT},
  { "resize_if_possible", XmRESIZE_IF_POSSIBLE},

  /* XmRMultiClick */
  { "multiclick_discard", XmMULTICLICK_DISCARD},
  { "multiclick_keep", XmMULTICLICK_KEEP},

  /* XmRNavigationType */
  { "none", XmNONE},
  { "tab_group", XmTAB_GROUP},
  { "sticky_tab_group", XmSTICKY_TAB_GROUP},
  { "exclusive_tab_group", XmEXCLUSIVE_TAB_GROUP},

  /* XmROrientation */
  { "vertical", XmVERTICAL},
  { "horizontal", XmHORIZONTAL},

  /* XmRPacking */
  { "pack_tight", XmPACK_TIGHT},
  { "pack_column", XmPACK_COLUMN},
  { "pack_none", XmPACK_NONE},

  /* XmRProcessingDirection */
  { "max_on_top", XmMAX_ON_TOP},
  { "max_on_bottom", XmMAX_ON_BOTTOM},
  { "max_on_left", XmMAX_ON_LEFT},
  { "max_on_right", XmMAX_ON_RIGHT},

  /* XmRResizePolicy */
  { "resize_none", XmRESIZE_NONE},
  { "resize_grow", XmRESIZE_GROW},
  { "resize_any", XmRESIZE_ANY},

  /* XmRRowColumnType */
  { "work_area", XmWORK_AREA},
  { "menu_bar", XmMENU_BAR},
  { "menu_pulldown", XmMENU_PULLDOWN},
  { "menu_popup", XmMENU_POPUP},
  { "menu_option", XmMENU_OPTION},
  
  /* XmRScrollBarDisplayPolicy */
  { "static", XmSTATIC},
  { "as_needed", XmAS_NEEDED},

  /* XmRScrollBarPlacement */
  { "bottom_right", XmBOTTOM_RIGHT},
  { "top_right", XmTOP_RIGHT},
  { "bottom_left", XmBOTTOM_LEFT},
  { "top_left", XmTOP_LEFT},

  /* XmRScrollingPolicy */
  { "automatic", XmAUTOMATIC},
  { "application_defined", XmAPPLICATION_DEFINED},

  /* XmRSelectionPolicy */
  { "single_select", XmSINGLE_SELECT},
  { "multiple_select", XmMULTIPLE_SELECT},
  { "extended_select", XmEXTENDED_SELECT},
  { "browse_select", XmBROWSE_SELECT},

  /* XmRSelectionType */
  { "dialog_work_area", XmDIALOG_WORK_AREA},
  { "dialog_prompt", XmDIALOG_PROMPT},
  { "dialog_selection", XmDIALOG_SELECTION},
  { "dialog_command", XmDIALOG_COMMAND},
  { "dialog_file_selection", XmDIALOG_FILE_SELECTION},

  /* XmRSeparatorType */
  { "no_line", XmNO_LINE},
  { "single_line", XmSINGLE_LINE},
  { "double_line", XmDOUBLE_LINE},
  { "single_dashed_line", XmSINGLE_DASHED_LINE},
  { "double_dashed_line", XmDOUBLE_DASHED_LINE},
  { "shadow_etched_in", XmSHADOW_ETCHED_IN},
  { "shadow_etched_out", XmSHADOW_ETCHED_OUT},
  { "shadow_etched_in_dash", XmSHADOW_ETCHED_IN_DASH},
  { "shadow_etched_out_dash", XmSHADOW_ETCHED_OUT_DASH},

  /* XmRShadowType */
  { "shadow_etched_in", XmSHADOW_ETCHED_IN},
  { "shadow_etched_out", XmSHADOW_ETCHED_OUT},
  { "shadow_in", XmSHADOW_IN},
  { "shadow_out", XmSHADOW_OUT},

  /* XmRStringDirection */
  { "string_direction_l_to_r", XmSTRING_DIRECTION_L_TO_R},
  { "string_direction_r_to_l", XmSTRING_DIRECTION_R_TO_L},

  /* XmRTearOffModel */
  { "tear_off_enabled", XmTEAR_OFF_ENABLED},
  { "tear_off_disabled", XmTEAR_OFF_DISABLED},
  
  /* XmRUnpostBehavior */
  { "unpost", XmUNPOST},
  { "unpost_and_replay", XmUNPOST_AND_REPLAY},

  /* XmRVerticalAlignment */
  { "alignment_baseline_top", XmALIGNMENT_BASELINE_TOP},
  { "alignment_center", XmALIGNMENT_CENTER},
  { "alignment_baseline_bottom", XmALIGNMENT_BASELINE_BOTTOM},
  { "alignment_contents_top", XmALIGNMENT_CONTENTS_TOP},
  { "alignment_contents_bottom", XmALIGNMENT_CONTENTS_BOTTOM},

  /* XmRVisualPolicy */
  { "variable", XmVARIABLE},
  { "constant", XmCONSTANT},

  /*  End of test */
  { NULL, 0}
};

#endif

void main(argc, argv)
unsigned int argc;
char *argv[];
{
  Widget bulletin;
  XrmValue from;
  XrmValue to_return;
  register int i, value_counter,k;
  unsigned short rep_id, error_counter;

#ifndef MOTIF1_1
  XmRepTypeEntry rep_type;
#endif

  /*
   * Initialize toolkit
   */

  CommonTestInit(argc,argv);

  CommonPause();

#ifndef MOTIF1_1
  /*
   * Need to create a widget to get the converters registered
   */

  bulletin = XmCreateBulletinBoard(Shell1,	/* parent	*/
				   "bulletin",	/* name		*/
				   NULL, 0);	/* arguments	*/
  XtManageChild (bulletin);

  XtRealizeWidget(Shell1);

  /*
   * First test to see if it can detect an invalid rep type
   *  It should return XmREP_TYPE_INVALID
   */

  printf("Testing detection of invalid representation type : ");
  rep_id = XmRepTypeGetId("bogus");

  if (rep_id != XmREP_TYPE_INVALID)
    printf("   FAILED\n");
  else
    printf("   PASSED\n");

  /*
   * Query each legal static representation type
   */

  value_counter = 0;
  for (i=0; reptypedata[i].rep_type != NULL; i++)
    {
      error_counter = 0;

      printf("Representation Type = %s,", reptypedata[i].rep_type);

      rep_id = XmRepTypeGetId(reptypedata[i].rep_type);
      if (rep_id == XmREP_TYPE_INVALID)
	{
	  printf("\n   FAILED:  Detected invalid rep type\n");
	  value_counter += reptypedata[i].num_of_tests;
	  continue;
	}

      rep_type = XmRepTypeGetRecord(rep_id);

      /*
       * Diagnose the answers
       */
      
      if (reptypedata[i].num_of_tests != rep_type->num_values)
	{
	  printf("\n   FAILED: Discrepancy in returned number of values\n");
	  printf("\n   expected %d received %d\n",
		reptypedata[i].num_of_tests,rep_type->num_values);
	  value_counter += reptypedata[i].num_of_tests;
	  continue;
	}

      /* Look at each to see if they match */

      for (k=0; k < rep_type->num_values; k++)
	{
	  if (convert_set[value_counter].correct_value !=
	      rep_type->values[k])
	    {
	      printf("\n   Error: Discrepancy in value returned \n");
	      error_counter++;
	    }

	  if (strcmp(rep_type->value_names[k], 
		     convert_set[value_counter].from_string) != 0)
	    {
	      printf("\n   Error: Discrepancy in name returned \n");
	      error_counter++;
	    }

	  value_counter++;
	}

      if (error_counter > 0)
	printf("\n   Test FAILED\n");
      else
	printf("   Test PASSED\n");

      /* Free the structure since we're done with it */
      XtFree((char *)rep_type);
    }
  CommonPause();

#endif
}
