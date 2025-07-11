/* $XConsortium: ComboBox5.c /main/4 1996/05/08 23:33:22 drk $ */
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

#include <testlib.h>

#define PROBE()	do { \
		    (void)printf( "%s{%d}\n", __FILE__, __LINE__ ); } \
		while (0)


#define NUM_COMBO_BOX_TYPES 3

#define NUM_STRINGS 5
static char *list_strings[NUM_STRINGS] =
	{ "0123456789012345678901234567890123456789",
          "ABCDEFGHIABCDEFGHIABCDEFGHIABCDEFGHIABCDEFGHI",
          "abcdefghiabcdefghiabcdefghiabcdefghiabcdefghi",
          "shortstring",
          "SayHello" };

Widget Manager;
Widget ComboBox;                /* This is the ComboBox under test. */
Widget TextField;

  /*
   *  Get the default value of the XmNcolumns resource from the TextField.
   */
  void
PrintNumColumns( void )
{
  Arg   args[10];
  int   n;
  short num_columns;

  n = 0;
  XtSetArg( args[n], XmNcolumns, &num_columns ); n++;
  XtGetValues( TextField, args, n );
  (void)printf("XmNcolumns = %d\n", (int)num_columns );
  return;
}

  int
main (int argc, char **argv)
{
  Arg 		args[10];
  int 		n,i, instance;
  XmString      list_item[NUM_STRINGS];
  unsigned char combo_box_type;
  char		buffer[25];

  CommonTestInit (argc, argv);

  (void)printf( "Begin Test\n" );

  if (UserData == NULL)
  {
    (void)printf ("Usage: ComboBox5 -u <comboBoxType>\n");
    exit(0);
  }

 /*
  *  Determine which type of ComboBox is being created.
  */
  if ( strcmp( UserData, "COMBO_BOX" ) == 0 ) 
  {
    instance = 1;
    combo_box_type = XmCOMBO_BOX;
  }
  else if ( strcmp( UserData, "DROP_DOWN_COMBO_BOX" ) == 0 ) 
  {
    instance = 2;
    combo_box_type = XmDROP_DOWN_COMBO_BOX;
  }
  else if ( strcmp( UserData, "DROP_DOWN_LIST" ) == 0 ) 
  {
    instance = 3;
    combo_box_type = XmDROP_DOWN_LIST;
  }
  else {
    (void)printf( "Incorrect ComboBox Type %s\n", UserData );
    exit(0);
  }


  /* Create the parent. */
  n=0;
  Manager = XmCreateForm( Shell1, "Shell1", args, n );
  XtManageChild (Manager);

  /* Create a list item for the ComboBox List child */
  for ( n=0; n<NUM_STRINGS; n++ )
    list_item[n] = XmStringCreateLtoR( list_strings[n], XmFONTLIST_DEFAULT_TAG);

  /*
   *  Create a ComboBox of the requested comboBoxType.
   */
  (void)sprintf( buffer, "ComboBox%d", instance );
  (void)printf( "Creating %s of type %s\n", buffer, UserData );

  n = 0;
  XtSetArg( args[n], XmNtopAttachment, XmATTACH_FORM ); n++;
  XtSetArg( args[n], XmNleftAttachment, XmATTACH_FORM ); n++;
  XtSetArg( args[n], XmNcomboBoxType, combo_box_type ); n++;
  XtSetArg( args[n], XmNitems, &list_item ); n++;
  XtSetArg( args[n], XmNitemCount, 3 ); n++;
  XtSetArg( args[n], XmNvisibleItemCount, (NUM_STRINGS-1) ); n++;
  ComboBox = XmCreateComboBox( Manager, buffer, args, n );
  XtManageChild( ComboBox );

  for ( n=0; n<NUM_STRINGS; n++ )
      XmStringFree( list_item[n] );

  XtRealizeWidget( Shell1 );

  /*
   *  Retrieve the XmTextField child widget from the ComboBox
   */
  TextField = XtNameToWidget( ComboBox, "*Text" );

#if defined(DEFECT)
  /* The following code should be equivalent to the XtNameToWidget code,
   * but it causes a NULL widget pointer to be returned.
   */
  n = 0;
  XtSetArg( args[n], XmNtextField, &TextField ); n++;
  XtGetValues( ComboBox, args, n );
#endif /* defined(DEFECT) */


  (void)printf( "CommonPause 1\n" );
  PrintNumColumns();
  CommonPause();

  (void)printf( "CommonPause 2\n" );
  n = 0;
  XtSetArg( args[n], XmNcolumns, 10 ); n++;
  XtSetValues( TextField, args, n );
  PrintNumColumns();
  CommonPause();

  (void)printf( "CommonPause 3\n" );
  n = 0;
  XtSetArg( args[n], XmNcolumns, 30 ); n++;
  XtSetValues( TextField, args, n );
  PrintNumColumns();
  CommonPause();

  XtAppMainLoop( app_context );

  XtDestroyWidget( ComboBox );
  XtDestroyWidget( Manager );

  (void)printf( "End Test\n" );

  return 0;
} 
