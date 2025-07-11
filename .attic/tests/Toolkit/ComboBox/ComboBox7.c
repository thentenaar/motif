/* $XConsortium: ComboBox7.c /main/3 1996/05/08 23:33:47 drk $ */
#include <testlib.h>

#define PROBE()	do { \
		    (void)printf( "%s{%d}\n", __FILE__, __LINE__ ); } \
		while (0)

/* Changing this value can destroy the test */
#define NUM_STRINGS 3

static char *strings[NUM_STRINGS] = {
	"Apples", 
  	"Blueberries",
	"Cherries" };

XmString	list_items[NUM_STRINGS];

Widget		Form, ComboBox, Separator, PositionScale, DelStringPB;

void
DelStringCB( Widget widget, XtPointer client_data, XtPointer call_data )
{
    int		position;
    Arg		args;

    XtSetArg( args, XmNvalue, &position );
    XtGetValues( PositionScale, &args, 1 );


    XmComboBoxDeletePos( ComboBox, position );
    return;
}

void
PrintNumItems( void )
{
  int itemCount;
  Arg arg;

  XtSetArg( arg, XmNitemCount, &itemCount );
  XtGetValues( ComboBox, &arg, 1 );

  (void)printf( "XmNitemCount = %d\n\n", itemCount );

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
  
  /* Reset the Position scale to one */
  n = 0;
  XtSetArg( args[n], XmNvalue, 1 ); n++;
  XtSetValues( PositionScale, args, n );

  return;
}
  
	
  int
main( int argc, char **argv )
{
int		i, n;
Arg		args[25];
XmString	string;
char		buffer[25];

int		instance;
unsigned char	combo_box_type;

  CommonTestInit (argc, argv);

  (void)printf( "Begin test\n" );

  if (UserData == NULL)
  {
      (void)printf ("Usage: ComboBox7 -u <comboBoxType>\n");
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

  (void)sprintf( buffer, "ComboBox%d", instance );

  for ( i = 0; i < NUM_STRINGS; i++ )
    { list_items[i]=XmStringCreateLtoR(strings[i],XmFONTLIST_DEFAULT_TAG); }

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

  /* Don't free the strings: ResetWidgets uses them. */
  
  /* Make the Separator */
  n = 0;
  XtSetArg( args[n], XmNtopAttachment, XmATTACH_WIDGET ); n++;
  XtSetArg( args[n], XmNtopWidget, ComboBox ); n++;
  XtSetArg( args[n], XmNleftAttachment, XmATTACH_FORM ); n++;
  XtSetArg( args[n], XmNrightAttachment, XmATTACH_FORM ); n++;
  XtSetArg( args[n], XmNseparatorType, XmSHADOW_ETCHED_OUT ); n++;
  Separator = XmCreateSeparator( Form, "Separator", args, n );
  XtManageChild( Separator );

  /* Make Position Slider */

  n = 0;
  string = XmStringCreateLtoR("Position", XmFONTLIST_DEFAULT_TAG );
  XtSetArg( args[n], XmNtopAttachment, XmATTACH_WIDGET ); n++;
  XtSetArg( args[n], XmNtopWidget, Separator ); n++;
  XtSetArg( args[n], XmNleftAttachment, XmATTACH_FORM ); n++;
  XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM ); n++;
  XtSetArg( args[n], XmNtitleString, string ); n++;
  XtSetArg( args[n], XmNminimum, 1 ); n++;
  XtSetArg( args[n], XmNmaximum, 4 ); n++;
  XtSetArg( args[n], XmNorientation, XmHORIZONTAL ); n++;
  XtSetArg( args[n], XmNvalue, 1 ); n++;
  PositionScale = XmCreateScale( Form, "PositionScale", args, n );
  XtManageChild( PositionScale );
  XmStringFree( string );

  /* Create Delete Button */
  n = 0;
  string = XmStringCreateLtoR("Delete Item", XmFONTLIST_DEFAULT_TAG );
  XtSetArg( args[n], XmNtopAttachment, XmATTACH_WIDGET ); n++;
  XtSetArg( args[n], XmNtopWidget, Separator ); n++;
  XtSetArg( args[n], XmNleftAttachment, XmATTACH_WIDGET ); n++;
  XtSetArg( args[n], XmNleftWidget, PositionScale ); n++;
  XtSetArg( args[n], XmNlabelString, string ); n++;
  DelStringPB = XmCreatePushButton( Form, "DelStringPB", args, n );
  XtManageChild( DelStringPB );
  XmStringFree( string );
  XtAddCallback( DelStringPB, XmNactivateCallback, DelStringCB, 
                 ( XtPointer )NULL ); 

  XtRealizeWidget( Shell1 );


  /* TP 1 */
  (void)printf( "Test Purpose 1\n" );
  CommonPause();
  PrintNumItems();
  ResetWidgets();
 
  /* TP 2 */
  (void)printf( "Test Purpose 2\n" );
  XtSetArg( args[0], XmNvisibleItemCount, NUM_STRINGS-1 );
  XtSetValues( ComboBox, args, 1 );
  CommonPause();
  /* Don't reset the widgets: the next test uses it as is */
 
  /* TP 3 */
  (void)printf( "Test Purpose 3\n" );
  CommonPause();
  /* Don't reset the widgets: the next test uses it as is */
 
  /* TP 4 */
  (void)printf( "Test Purpose 4\n" );
  /* Delete the last item from the ComboBox */
  XmComboBoxDeletePos( ComboBox, NUM_STRINGS );
  CommonPause();
  ResetWidgets();

  /* TP 5 */
  (void)printf( "Test Purpose 5\n" );
  XtSetArg( args[0], XmNvisibleItemCount, NUM_STRINGS-1 );
  XmComboBoxDeletePos( ComboBox, 2 );	/* Should be second of three */
  XtSetValues( ComboBox, args, 1 );
  CommonPause();
  ResetWidgets();	/* No need, but leaves things clean for future */

  XtAppMainLoop( app_context );

  for ( i = 0; i < NUM_STRINGS; i++ )
    { XmStringFree(list_items[i]); }

  XtDestroyWidget( DelStringPB );
  XtDestroyWidget( PositionScale );
  XtDestroyWidget( Separator );
  XtDestroyWidget( ComboBox );
  XtDestroyWidget( Form );

  (void)printf( "End test\n" );
  return 0;
}
