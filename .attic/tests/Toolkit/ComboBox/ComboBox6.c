/* $XConsortium: ComboBox6.c /main/4 1996/05/08 23:33:34 drk $ */

#include <testlib.h>

#define NUM_STRINGS 3

static char *strings[NUM_STRINGS] = {
	"Apples", 
  	"Blueberries",
	"Cherries" };

XmString	list_items[NUM_STRINGS];

Widget		Form, ComboBox, Separator, TextField, TextFieldLabel, 
                UniqueTB, PositionScale, AddStringLabel, AddStringPB;

void
AddStringCB( Widget widget, XtPointer client_data, XtPointer call_data )
{
	int		n, position;
	Arg		args[10];
        char		*text_string;
        unsigned char	set;
	XmString	item;

        n = 0;
        XtSetArg( args[n], XmNvalue, &text_string ); n++;
        XtGetValues( TextField, args, n );

        n = 0;
        XtSetArg( args[n], XmNset, &set ); n++;
        XtGetValues( UniqueTB, args, n );

	n = 0;
	XtSetArg( args[n], XmNvalue, &position ); n++;
        XtGetValues( PositionScale, args, n );

	item = XmStringCreateLtoR( text_string, XmFONTLIST_DEFAULT_TAG );

	XmComboBoxAddItem( ComboBox, item, position, set ? True : False );

     return;
}

void
PrintNumItemsCB( Widget widget, XtPointer client_data, XtPointer call_data )
{
	int		n, itemCount;
	Arg		args[10];

	n = 0;
	XtSetArg( args[n], XmNitemCount, &itemCount ); n++;
	XtGetValues( ComboBox, args, n );

	printf("XmNitemCount = %d \n\n", itemCount );

	return;
}

void
ResetWidgets()
{

  int	n;
  Arg	args[10];

 /*
  *  Reset the three original XmStrings to the ComboBox.
  */
  n = 0;
  XtSetArg( args[n], XmNitems, list_items ); n++;
  XtSetArg( args[n], XmNitemCount, NUM_STRINGS ); n++;
  XtSetArg( args[n], XmNvisibleItemCount, NUM_STRINGS ); n++;
  XtSetValues( ComboBox, args, n );

  n = 0;
  XtSetArg( args[n], XmNvalue, (char *)0 ); n++;
  XtSetValues( TextField, args, n );

  n = 0;
  XtSetArg( args[n], XmNset, False ); n++;
  XtSetValues( UniqueTB, args, n );
  
  n = 0;
  XtSetArg( args[n], XmNvalue, 1 ); n++;
  XtSetValues( PositionScale, args, n );

  return;
}
  
	
void
main( int argc, char **argv )
{
int		i, n;
Arg		args[25];
XmString	label_string;
char		buffer[25];

int		instance;
unsigned char	combo_box_type;

  CommonTestInit (argc, argv);

  if (UserData == NULL)
  {
      printf ("Usage: ComboBox6 -u <comboBoxType>\n");
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

  n = 0;
  Form = XmCreateForm( Shell1, "Form", args, n ); 
  XtManageChild( Form );

  sprintf( buffer, "ComboBox%d", instance );

  for ( i = 0; i < NUM_STRINGS; i++ )
  {
      list_items[i] = XmStringCreateLtoR( strings[i],
                        XmFONTLIST_DEFAULT_TAG );
  }

  n = 0;
  XtSetArg( args[n], XmNtopAttachment, XmATTACH_FORM ); n++;
  XtSetArg( args[n], XmNleftAttachment, XmATTACH_FORM ); n++;
  XtSetArg( args[n], XmNrightAttachment, XmATTACH_FORM ); n++;
  XtSetArg( args[n], XmNcomboBoxType, combo_box_type ); n++;
  XtSetArg( args[n], XmNitems, list_items ); n++;
  XtSetArg( args[n], XmNitemCount, NUM_STRINGS ); n++;
  XtSetArg( args[n], XmNvisibleItemCount, NUM_STRINGS ); n++;
  ComboBox = XmCreateComboBox( Form, buffer, args, n ); 
  XtManageChild( ComboBox );


  n = 0;
  XtSetArg( args[n], XmNtopAttachment, XmATTACH_WIDGET ); n++;
  XtSetArg( args[n], XmNtopWidget, ComboBox ); n++;
  XtSetArg( args[n], XmNleftAttachment, XmATTACH_FORM ); n++;
  XtSetArg( args[n], XmNrightAttachment, XmATTACH_FORM ); n++;
  XtSetArg( args[n], XmNseparatorType, XmSHADOW_ETCHED_OUT ); n++;
  Separator = XmCreateSeparator( Form, "Separator", args, n );
  XtManageChild( Separator );

  label_string = XmStringCreateLtoR("Enter New String",
	 	   XmFONTLIST_DEFAULT_TAG );
 
  n = 0;
  XtSetArg( args[n], XmNtopAttachment, XmATTACH_WIDGET ); n++;
  XtSetArg( args[n], XmNtopWidget, Separator ); n++;
  XtSetArg( args[n], XmNleftAttachment, XmATTACH_FORM ); n++;
  XtSetArg( args[n], XmNlabelString, label_string ); n++;
  XtSetArg( args[n], XmNalignment, XmALIGNMENT_BEGINNING ); n++;
  TextFieldLabel = XmCreateLabel( Form, "TextFieldLabel", args, n );
  XtManageChild( TextFieldLabel );

  XmStringFree( label_string );
  
  n = 0;
  XtSetArg( args[n], XmNtopAttachment, XmATTACH_WIDGET ); n++;
  XtSetArg( args[n], XmNtopWidget, Separator ); n++;
  XtSetArg( args[n], XmNleftAttachment, XmATTACH_WIDGET ); n++;
  XtSetArg( args[n], XmNleftWidget, TextFieldLabel ); n++;
  XtSetArg( args[n], XmNrightAttachment, XmATTACH_FORM ); n++;
  XtSetArg( args[n], XmNeditable, True ); n++;
  TextField = XmCreateTextField( Form, "TextField", args, n );
  XtManageChild( TextField );

  label_string = XmStringCreateLtoR("Unique", XmFONTLIST_DEFAULT_TAG );

  n = 0;
  XtSetArg( args[n], XmNtopAttachment, XmATTACH_WIDGET ); n++;
  XtSetArg( args[n], XmNtopWidget, TextFieldLabel ); n++;
  XtSetArg( args[n], XmNleftAttachment, XmATTACH_FORM ); n++;
  XtSetArg( args[n], XmNlabelString, label_string ); n++;
  UniqueTB = XmCreateToggleButton( Form, "UniqueTB", args, n ); 
  XtManageChild( UniqueTB );

  XmStringFree( label_string );

  label_string = XmStringCreateLtoR("Position", XmFONTLIST_DEFAULT_TAG );

  n = 0;
  XtSetArg( args[n], XmNtopAttachment, XmATTACH_WIDGET ); n++;
  XtSetArg( args[n], XmNtopWidget, UniqueTB ); n++;
  XtSetArg( args[n], XmNleftAttachment, XmATTACH_FORM ); n++;
  XtSetArg( args[n], XmNtitleString, label_string ); n++;
  XtSetArg( args[n], XmNminimum, 1 ); n++;
  XtSetArg( args[n], XmNmaximum, 4 ); n++;
  XtSetArg( args[n], XmNorientation, XmHORIZONTAL ); n++;
  XtSetArg( args[n], XmNvalue, 1 ); n++;
  PositionScale = XmCreateScale( Form, "PositionScale", args, n );
  XtManageChild( PositionScale );

  XmStringFree( label_string );

  label_string = XmStringCreateLtoR("Add Item", XmFONTLIST_DEFAULT_TAG );

  n = 0;
  XtSetArg( args[n], XmNtopAttachment, XmATTACH_WIDGET ); n++;
  XtSetArg( args[n], XmNtopWidget, PositionScale ); n++;
  XtSetArg( args[n], XmNleftAttachment, XmATTACH_FORM ); n++;
  XtSetArg( args[n], XmNlabelString, label_string ); n++;
  AddStringPB = XmCreatePushButton( Form, "AddStringPB", args, n );
  XtManageChild( AddStringPB );

  XmStringFree( label_string );

  XtAddCallback( AddStringPB, XmNactivateCallback, AddStringCB, 
                 ( XtPointer )NULL ); 
  XtAddCallback( AddStringPB, XmNactivateCallback, PrintNumItemsCB, 
                 ( XtPointer )NULL ); 

  XtRealizeWidget( Shell1 );

  /* TP 1 */
  (void)printf( "Test Purpose 1\n" );
  CommonPause();

  XtRemoveCallback( AddStringPB, XmNactivateCallback, PrintNumItemsCB,
                 ( XtPointer )NULL ); 

  ResetWidgets();
 
  /* TP 2 */
  (void)printf( "Test Purpose 2\n" );
  CommonPause();

  ResetWidgets();
 
  /* TP 3 */
  (void)printf( "Test Purpose 3\n" );
  CommonPause();

  ResetWidgets();
 
  /* TP 4 */
  (void)printf( "Test Purpose 4\n" );
  CommonPause();

  /* TP 5 */
  (void)printf( "Test Purpose 5\n" );
  CommonPause();

  XtAppMainLoop( app_context );
}
