/* $XConsortium: ComboBox8.c /main/4 1996/05/08 23:33:59 drk $ */
#include <testlib.h>

/* Changing this value can destroy the test */
#define NUM_STRINGS 4

static char *strings[NUM_STRINGS] = {
	"Mother",
	"Father",
	"Sister",
	"Brother" };

XmString	list_items[NUM_STRINGS];

Widget		Form, ComboBox;

  int
main( int argc, char **argv )
{
int		i, n;
Arg		args[25];
char		buffer[25];
XmString	string;

int		instance;
unsigned char	combo_box_type;

  CommonTestInit (argc, argv);

  (void)printf( "Begin Test\n" );

  if (UserData == NULL)
  {
      printf ("Usage: ComboBox8 -u <comboBoxType>\n");
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
    { list_items[i]=XmStringCreateLtoR(strings[i],XmFONTLIST_DEFAULT_TAG); }

  n = 0;
  XtSetArg( args[n], XmNtopAttachment, XmATTACH_FORM ); n++;
  XtSetArg( args[n], XmNleftAttachment, XmATTACH_FORM ); n++;
  XtSetArg( args[n], XmNrightAttachment, XmATTACH_FORM ); n++;
  XtSetArg( args[n], XmNcomboBoxType, combo_box_type ); n++;
  XtSetArg( args[n], XmNitems, list_items ); n++;
  XtSetArg( args[n], XmNitemCount, NUM_STRINGS ); n++;
  XtSetArg( args[n], XmNvisibleItemCount, NUM_STRINGS-1 ); n++;
  ComboBox = XmCreateComboBox( Form, buffer, args, n ); 
  XtManageChild( ComboBox );

  XtRealizeWidget( Shell1 );


  /* TP 1 */
  (void)printf( "Test Purpose 1\n" );
  XmComboBoxSelectItem( ComboBox, list_items[0] );	/* The first item */
  CommonPause();
 
  /* TP 2 */
  (void)printf( "Test Purpose 2\n" );
  XmComboBoxSelectItem( ComboBox, list_items[3] );	/* The last item */
  CommonPause();
 
  /* TP 3 */
  (void)printf( "Test Purpose 3\n" );
  /* The user scrolls down to display the final item */
  CommonPause();
 
  /* TP 4 */
  (void)printf( "Test Purpose 4\n" );
  (void)printf( "Deliberate Error: XmComboBoxSelectItem called with an "
	"item not present in the ComboBox.\n" );
  string = XmStringCreate( "Non-matching", XmFONTLIST_DEFAULT_TAG );
  XmComboBoxSelectItem( ComboBox, string );
  XmStringFree( string );
  CommonPause();

  /* TP 5 */
  (void)printf( "Test Purpose 5\n" );
  XmComboBoxSetItem( ComboBox, list_items[1] );		/* The 2nd item */
  CommonPause();

  /* TP 6 */
  (void)printf( "Test Purpose 6\n" );
  XmComboBoxSetItem( ComboBox, list_items[0] );		/* The 1st item */
  CommonPause();

  /* TP 7 */
  (void)printf( "Test Purpose 7\n" );
  XmComboBoxSetItem( ComboBox, list_items[3] );		/* The last item */
  CommonPause();

  XtAppMainLoop( app_context );

  for ( i = 0; i < NUM_STRINGS; i++ )
    { XmStringFree(list_items[i]); }

  XtDestroyWidget( ComboBox );
  XtDestroyWidget( Form );

  (void)printf( "End Test\n" );

  return 0;
}
