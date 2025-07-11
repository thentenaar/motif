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
static char rcsid[] = "$XConsortium: RepType2.c /main/9 1995/07/13 17:54:07 drk $"
#endif
#endif


/*
 *  This test tests the Representation Type Management facility in terms
 *  of its dynamic registering capabilities.
 *
 *  Several representation type will be added to the manager to manage. 
 *  The functionality of the XmRepTypeRegister, XmRepTypeAddReverse,
 *  XmRepTypeValidValue, XmRepTypeGetNameList, XmRepTypeGetId,
 *  XmRepTypeGetRecord, and XmRepTypeGetRegistered routines will be checked.
 *
 *  Alternating queries of static and dynamic rep types will occur at 
 *  different points in the test to make sure that dynamically adding a
 *  rep type doesn;t mess up the static part, etc.
 */


#include <testlib.h>

#ifndef MOTIF1_1
#include <Xm/RepType.h>

static void CheckStaticTypes(void);
static void GetRegisteredAndDiagnose(void);
static void AddRepType(String rep_type,
		       char *names[],
		       unsigned char num_values,
		       unsigned char *values);


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
  {XmRCommandWindowLocation, 2},
  {XmREditMode, 2},
  {XmRFileTypeMask, 3},
  {XmRLabelType, 2},
  {XmRListSizePolicy, 3},
  {XmRRowColumnType, 5},
  {XmRScrollBarDisplayPolicy, 2},
  {XmRScrollBarPlacement, 4},
  {XmRScrollingPolicy, 2},
  {XmRSelectionPolicy, 4},
  {XmRSeparatorType, 9},
  {0,0}
};

typedef struct
{
  char *from_string;
  unsigned char correct_value;
} ConvertVal;

Widget bulletin;

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

  /* XmRCommandWindowLocation */
  { "command_above_workspace", XmCOMMAND_ABOVE_WORKSPACE},
  { "command_below_workspace", XmCOMMAND_BELOW_WORKSPACE},
  
  /* XmREditMode */
  { "multi_line_edit", XmMULTI_LINE_EDIT},
  { "single_line_edit", XmSINGLE_LINE_EDIT},

  /* XmRFileTypeMask */
  { "file_directory", XmFILE_DIRECTORY},
  { "file_regular", XmFILE_REGULAR},
  { "file_any_type", XmFILE_ANY_TYPE},

  /* XmRLabelType */
  { "pixmap", XmPIXMAP},
  { "string", XmSTRING},

  /* XmRListSizePolicy */
  { "variable", XmVARIABLE},
  { "constant", XmCONSTANT},
  { "resize_if_possible", XmRESIZE_IF_POSSIBLE},

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

  { NULL, 0}
};

#endif /* MOTIF1_1 */

void main(argc, argv)
unsigned int argc;
char *argv[];
{
  unsigned short temp_rep_id;

#ifndef MOTIF1_1

  XmRepTypeEntry temp_rep_type, rep_type1, rep_type2;
  String XmRNewRepType1 = "XmRNewRepType1";
  String XmRNewRepType2 = "XmRNewRepType2";
  String XmRNewRepType3 = "XmRNewRepType3";
  String XmRNewRepType4 = "XmRNewRepType4";

  static String RepType1Names[] = {
    "rep_type1_fe", "rep_type1_fi", "rep_type1_fa", "rep_type1_fo"};
 
  static String RepType2Names[] = 
    { "rep_type2_fe", "rep_type2_fi", "rep_type2_fa", "rep_type2_fo",
      "rep_type2_fu"
      };
  static String RepType3Names[] = 
    { "rep_type3_two", "rep_type3_four", "rep_type3_six", "rep_type3_eight" };

  static unsigned char RepType3Values[] =
    { '\002', '\004', '\006', '\010' };

  static String RepType4Names[] =
    { "rep_type4_69", "rep_type4_42", "rep_type4_105", "rep_type4_0", 
	"rep_type4_99" };

  static unsigned char RepType4Values[] =
    { '\105', '\052', '\151', '\000', '\143' };

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

  AddRepType(XmRNewRepType1, RepType1Names, 4, NULL);
 
  /*
   * Query some legal static representation types to see if they're
   *  still OK.
   */

  CheckStaticTypes();
  
  /*
   *  Now add another representation type.
   */

  AddRepType(XmRNewRepType2, RepType2Names, 5, NULL);

  /*
   *  Query the first rep type added at this point to make sure it's ok
   */

  temp_rep_id = XmRepTypeGetId(XmRNewRepType1);
  
  if (temp_rep_id == XmREP_TYPE_INVALID)
    {
      printf("Error getting ID of RepType1\n");
    }
  else
    {
      temp_rep_type = XmRepTypeGetRecord(temp_rep_id);
      /* verify record */
 
      XtFree((char *)temp_rep_type);
   }

  /* 
   *  Query the static types *just* to make sure
   */

  CheckStaticTypes();

  /* 
   * Do the same thing over again for a pair of mapped types.
   */

  AddRepType(XmRNewRepType3, RepType3Names, 4, RepType3Values);
 
  /*
   * Query some legal static representation types to see if they're
   *  still OK.
   */

  CheckStaticTypes();
  
  /*
   *  Now add another representation type.
   */

  AddRepType(XmRNewRepType4, RepType4Names, 5, RepType4Values);

  /*
   *  Query the first rep type added at this point to make sure it's ok
   */

  temp_rep_id = XmRepTypeGetId(XmRNewRepType3);
  
  if (temp_rep_id == XmREP_TYPE_INVALID)
    {
      printf("Error getting ID of RepType3\n");
    }
  else
    {
      temp_rep_type = XmRepTypeGetRecord(temp_rep_id);
      /* verify record */
 
      XtFree((char *)temp_rep_type);
   }

  /* 
   *  Query the static types *just* to make sure
   */

  CheckStaticTypes();

  /*
   *  Do a full-blown XmRepTypeGetRegistered to make sure we're all 
   *    there!
   */

  GetRegisteredAndDiagnose();

  CommonPause();

#else

  printf("This test is a 1.2 test only\n");
  CommonPause();

#endif
}

#ifndef MOTIF1_1

static void AddRepType(String rep_type, char *names[], 
		       unsigned char num_values, unsigned char *values)
{

  unsigned short temp_rep_id, new_rep_id;
  XmRepTypeEntry temp_rep_type_record;
  XrmValue from, to_return;
  unsigned char value;
  char **name_list;
  int i;

  /* 
   * Add the representation type
   */

  temp_rep_id = XmRepTypeRegister(rep_type, names, values, num_values); 

  /*
   *  Install reverse converter.
   */

  XmRepTypeAddReverse(temp_rep_id);

  /*
   * Do a forwards conversion
   */

  printf("Conversion from type XmRString to %s...", rep_type);

  from.size = strlen(names[0]) + 1;
  from.addr = names[0];

  XtConvert(bulletin,	/* widget */
	    XmRString,  /* source type */
	    &from,	/* value to be converted */
	    rep_type,   /* destination type */
	    &to_return);/* converted value */
 
  /* Convert the first one. */
  if (values)
    value = values[0];
  else
    value = 0;
      
  if (to_return.addr == NULL) 
    {
      printf("FAILED.\n");
    }
  else
    {
      if ((*((unsigned char *) to_return.addr) != value ) || 
	  (to_return.size != 1))
	printf(" FAILED.\n");
      else
	printf(" PASSED.\n");
    }

  /*
   * Do a backwards conversion
   */

  printf("Conversion from type %s to XmRString ...", rep_type);
 
  /* Look for the first one */
  from.size = 1;
  from.addr = (char *) &value;

  XtConvert(bulletin,	/* widget */
	    rep_type,   /* source type */
	    &from,	/* value to be converted */
	    XmRString,  /* destination type */
	    &to_return);/* converted value */

  if (strcmp(*((char **)to_return.addr), names[0]) != 0)
    printf(" FAILED.\n");
  else
    printf(" PASSED.\n");

  /*
   * Check for valid values
   */

  printf("Verify valid value... ");

  /* Verify the last one. */
  if (values)
    value = values[num_values - 1];
  else
    value = num_values - 1;

  if (XmRepTypeValidValue(temp_rep_id, value, NULL) != True)
    printf("  FAILED\n");
  else
    printf("  PASSED\n");

  /*
   * Get the RepId for this one.  Is the same as the original?
   */

  new_rep_id = XmRepTypeGetId(rep_type);
  if (new_rep_id != temp_rep_id)
    printf("FAILED: Problem with rep id returned\n");

  /*
   * Get the namelist for this new rep type.  ok?
   */

  name_list = XmRepTypeGetNameList(temp_rep_id, True);

  /*
   *  Verify validity of name list
   */

  i = 0;
  while (name_list[i] != NULL)
    {
      printf("Found name %s\n", name_list[i]);
      i++;
    }

  /*  
   *  Free the name list 
   */

  XtFree((char *)name_list);

  /*
   * Get the record for this new rep type.  ok?
   */

  temp_rep_type_record = XmRepTypeGetRecord(temp_rep_id);
  
  /*
   *  Diagnose it
   */

  printf("\n\n");
  printf("RepType Name = %s\n", temp_rep_type_record->rep_type_name);
  printf("Allowable values and value names: \n");
  for (i = 0; i < temp_rep_type_record->num_values; i++)
    {
      printf("%d", temp_rep_type_record->values[i]);
      printf(" %s\n", temp_rep_type_record->value_names[i]);
    }

  if (temp_rep_type_record->reverse_installed)
    printf("Reverse converter installed\n");
  else
    printf("Reverse converter NOT installed\n");

  XtFree((char *)temp_rep_type_record);

}

static void CheckStaticTypes(void)
{
  register int i=0, value_counter=0, k=0;
  unsigned short temp_rep_id, error_counter;
  XmRepTypeEntry temp_rep_type;
 
  for (i=0; reptypedata[i].rep_type != NULL; i++)
    {
      error_counter = 0;
      printf("Representation Type = %s,", reptypedata[i].rep_type);

      temp_rep_id = XmRepTypeGetId(reptypedata[i].rep_type);
      if (temp_rep_id == XmREP_TYPE_INVALID)
	{
	  printf("\n   FAILED: Detected invalid rep type\n");
	  value_counter += reptypedata[i].num_of_tests;
	  continue;
	}

      temp_rep_type = XmRepTypeGetRecord(temp_rep_id);

      /*
       * Diagnose the answers
       */
      
      if (reptypedata[i].num_of_tests != temp_rep_type->num_values)
	{
	  printf("\n   FAILED: Discrepancy in returned number of values\n");
	  value_counter += reptypedata[i].num_of_tests;
	  continue;
	}

      /* Look at each to see if they match */

      for (k=0; k < temp_rep_type->num_values; k++)
	{
	  if (convert_set[value_counter].correct_value !=
	      temp_rep_type->values[k])
	    {
	      printf("\n   Error: Discrepancy in value returned \n");
	      error_counter++;
	    }

	  if (strcmp(temp_rep_type->value_names[k], 
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
      XtFree((char *)temp_rep_type);

    } /* End for each reptypedata */
}

static void GetRegisteredAndDiagnose(void)
{
  int j;
  XmRepTypeList rep_list, save_list;
  XmRepTypeEntry *temp;

  rep_list = XmRepTypeGetRegistered();

  save_list = rep_list;

  /* Diagnose rep_list */
  
  printf("\n\n");

  while (rep_list != NULL && rep_list->rep_type_name != NULL)
    {
      printf("RepType Name = %s\n", rep_list->rep_type_name);
      printf("Allowable values and value names: \n");
      for (j = 0; j < rep_list->num_values; j++)
	{
	  printf("%d", rep_list->values[j]);
	  printf(" %s\n", rep_list->value_names[j]);
	}

      if (rep_list->reverse_installed)
	printf("Reverse converter installed\n");
      else
	printf("Reverse converter NOT installed\n");
      printf("\n\n");
      rep_list++;
    }

  XtFree((char *)save_list);

  return;
}

#endif /*  MOTIF1_1 */









