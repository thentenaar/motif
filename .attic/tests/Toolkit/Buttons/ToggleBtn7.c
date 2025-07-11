/* $XConsortium: ToggleBtn7.c /main/3 1996/07/08 15:38:28 jakku $ */
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

#define MGR_WIDTH 200
#define MGR_HEIGHT 300

#define NUM_INDICATOR_ON_VALUES 7
#define NUM_INDICATOR_TYPE_VALUES 4

#define NUM_TOGGLE_BUTTONS 3


static char *indicator_on_strings[NUM_INDICATOR_ON_VALUES] = {
	"XmINDICATOR_NONE",
	"XmINDICATOR_FILL",
        "XmINDICATOR_BOX",
	"XmINDICATOR_CHECK",
	"XmINDICATOR_CHECK_BOX",
	"XmINDICATOR_CROSS", 
	"XmINDICATOR_CROSS_BOX" 
     };

static char *indicator_type_strings[NUM_INDICATOR_TYPE_VALUES] = {
        "XmN_OF_MANY",
	"XmONE_OF_MANY",
	"XmONE_OF_MANY_ROUND",
	"XmONE_OF_MANY_DIAMOND"
     };

static unsigned int indicator_on[NUM_INDICATOR_ON_VALUES] = {
	XmINDICATOR_NONE,
	XmINDICATOR_FILL,
        XmINDICATOR_BOX,
	XmINDICATOR_CHECK,
	XmINDICATOR_CHECK_BOX,
	XmINDICATOR_CROSS, 
	XmINDICATOR_CROSS_BOX }; 
	
static unsigned int indicator_type[NUM_INDICATOR_TYPE_VALUES] = {
        XmN_OF_MANY,
	XmONE_OF_MANY,
	XmONE_OF_MANY_ROUND,
	XmONE_OF_MANY_DIAMOND };
 
Widget ToggleButton[NUM_TOGGLE_BUTTONS];
Widget ToggleButtonGadget[NUM_TOGGLE_BUTTONS];
Widget IndicatorOnOptions, IndicatorTypeOptions;
Widget IndicatorOnPB[NUM_INDICATOR_ON_VALUES];
Widget IndicatorTypePB[NUM_INDICATOR_TYPE_VALUES];
int n, i;
Arg args[25];

void PrintIndicatorValues( Widget widget )
{
unsigned char  	indicatorOn;
unsigned char	indicatorType;

     n = 0;
     XtSetArg( args[n], XmNindicatorOn, &indicatorOn ); n++ ;
     XtSetArg( args[n], XmNindicatorType, &indicatorType ); n++ ;
     XtGetValues( widget, args, n );

     if ( XtIsSubclass( widget, xmToggleButtonWidgetClass )) {
        printf("ToggleButton Resource Values: \n");
        printf("============================: \n\n");
		}
     else {
	if ( XtIsSubclass( widget, xmToggleButtonGadgetClass ))
	 {
           printf("ToggleButtonGadget Resource Values: \n");
           printf("==================================  \n\n");
		}
	}
     printf("XmNindicatorOn = ");
     switch( indicatorOn )
     {
	case XmINDICATOR_NONE:
	     printf("XmINDICATOR_NONE \n");
             break;
	case XmINDICATOR_FILL:
	     printf("XmINDICATOR_FILL \n");
             break;
        case XmINDICATOR_BOX:
	     printf("XmINDICATOR_BOX \n");
             break;
	case XmINDICATOR_CHECK:
	     printf("XmINDICATOR_CHECK \n");
             break;
	case XmINDICATOR_CHECK_BOX:
	     printf("XmINDICATOR_CHECK_BOX \n");
             break;
	case XmINDICATOR_CROSS:
	     printf("XmINDICATOR_CROSS \n");
             break;
	case XmINDICATOR_CROSS_BOX:
	     printf("XmINDICATOR_CROSS_BOX \n");
             break;
        default:
             printf("*** an invalid value *** \n");
     }

     printf("XmNindicatorType = "); 
     switch( indicatorType )
     {
        case XmN_OF_MANY:
	     printf("XmN_OF_MANY \n");
             break;
	case XmONE_OF_MANY:
	     printf("XmONE_OF_MANY \n");
             break;
	case XmONE_OF_MANY_ROUND:
	     printf("XmONE_OF_MANY_ROUND \n");
             break;
	case XmONE_OF_MANY_DIAMOND:
	     printf("XmONE_OF_MANY_DIAMOND \n");
             break;
        default:
             printf("*** an invalid value *** \n");
     }

     return;
}

void IndicatorOnCB( Widget widget, XtPointer client_data, XtPointer call_data )
{
	unsigned int indicator_on = *((unsigned int *)client_data );

	n = 0;
	XtSetArg( args[n], XmNindicatorOn, indicator_on ); n++;
	for ( i = 0; i < NUM_TOGGLE_BUTTONS; i++ )
        { 
		XtSetValues( ToggleButton[i], args, n );
		XtSetValues( ToggleButtonGadget[i], args, n );
	}
}

void IndicatorTypeCB( Widget widget, XtPointer client_data, XtPointer call_data )
{
	unsigned int indicator_type = *((unsigned int *)client_data );

	n = 0;
	XtSetArg( args[n], XmNindicatorType, indicator_type ); n++;
	for ( i = 0; i < NUM_TOGGLE_BUTTONS; i++ )
        { 
		XtSetValues( ToggleButton[i], args, n );
		XtSetValues( ToggleButtonGadget[i], args, n );
	}
}

void CreateControlPanel()
{
Widget 		Form, PopupShell, RowColumn, Label1, Label2;
Widget		Pulldown1, Pulldown2;
int		n;
Arg		args[25];
XmString  	string;
char		buffer[25];

        n = 0;
        XtSetArg( args[n], XmNx, 0 ); n++;
        XtSetArg( args[n], XmNy, 500 ); n++;
        XtSetArg( args[n], XmNwidth,  300 ); n++;
        XtSetArg( args[n], XmNheight,  300 ); n++;
        PopupShell = XtCreatePopupShell("Resource Control Panel",
                                         topLevelShellWidgetClass,
                                         Shell1, args, n );

	n = 0;
	Form = XmCreateForm( PopupShell, "Form", args, n );
	XtManageChild( Form );

        n = 0;
        XtSetArg( args[n], XmNtopAttachment, XmATTACH_FORM ); n++;
        XtSetArg( args[n], XmNleftAttachment, XmATTACH_FORM ); n++;
        XtSetArg(args[n], XmNnumColumns, 4); n++;
        XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
        XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
        RowColumn = XmCreateRowColumn( Form, "RowColumn", args, n );
        XtManageChild( RowColumn ); 

       /*
        *  Create label and option menu for XmNindicatorOn
        *  resource settings.
        */
	string = XmStringCreateLtoR("XmNindicatorOn:", 
	         XmFONTLIST_DEFAULT_TAG );

	n = 0;
        XtSetArg( args[n], XmNlabelString, string ); n++;
	Label1 = XmCreateLabel( RowColumn, "Label1", args, n );
	XtManageChild( Label1 );
	XmStringFree( string );

	n = 0;
	Pulldown1 = XmCreatePulldownMenu(RowColumn, "Pulldown1", 
                   args, n );

	for ( i = 0; i < NUM_INDICATOR_ON_VALUES; i++ )
	{
	    string = XmStringCreateLtoR( indicator_on_strings[i],
		     XmFONTLIST_DEFAULT_TAG );
            n = 0;
	    sprintf( buffer, "IndicatorOnPB%d", i ); 
	    XtSetArg( args[n], XmNlabelString, string ); n++;
            IndicatorOnPB[i] = 
               XmCreatePushButton( Pulldown1, buffer, args, n ); 
            XmStringFree( string );	
	    XtAddCallback( IndicatorOnPB[i], XmNactivateCallback,
	                   IndicatorOnCB, (XtPointer)&indicator_on[i] ); 
        }
	XtManageChildren( IndicatorOnPB, NUM_INDICATOR_ON_VALUES );

	n = 0;
        XtSetArg( args[n], XmNmenuHistory,
                  IndicatorOnPB[0] ); n++;
        XtSetArg( args[n], XmNsubMenuId, Pulldown1 ); n++;
        IndicatorOnOptions =
        XmCreateOptionMenu( RowColumn, "IndicatorOnOptions", args, n );
        XtManageChild( IndicatorOnOptions );

       /*
        *  Create label and option menu for XmNindicatorType
        *  resource settings.
        */
	string = XmStringCreateLtoR("XmNindicatorType:", 
	         XmFONTLIST_DEFAULT_TAG );

	n = 0;
        XtSetArg( args[n], XmNlabelString, string ); n++;
	Label2 = XmCreateLabel( RowColumn, "Label2", args, n );
	XtManageChild( Label2 );
	XmStringFree( string );

	n = 0;
	Pulldown2 = XmCreatePulldownMenu(RowColumn, "Pulldown2", 
                   args, n );

	for ( i = 0; i < NUM_INDICATOR_TYPE_VALUES; i++ )
	{
	    string = XmStringCreateLtoR( indicator_type_strings[i],
		     XmFONTLIST_DEFAULT_TAG );
            n = 0;
	    sprintf( buffer, "IndicatorTypePB%d", i ); 
	    XtSetArg( args[n], XmNlabelString, string ); n++;
            IndicatorTypePB[i] = 
               XmCreatePushButton( Pulldown2, buffer, args, n ); 
            XmStringFree( string );	
	    XtAddCallback( IndicatorTypePB[i], XmNactivateCallback,
	                   IndicatorTypeCB, (XtPointer)&indicator_type[i] ); 
        }
	XtManageChildren( IndicatorTypePB, NUM_INDICATOR_TYPE_VALUES );

	n = 0;
        XtSetArg( args[n], XmNmenuHistory,
                  IndicatorTypePB[0] ); n++;
        XtSetArg( args[n], XmNsubMenuId, Pulldown2 ); n++;
        IndicatorTypeOptions =
        XmCreateOptionMenu( RowColumn, "IndicatorTypeOptions", args, n );
        XtManageChild( IndicatorTypeOptions ); 


	XtPopup( PopupShell, XtGrabNone );
}

/*
 * Main
 */
void main ( int argc, char **argv )
{

/*int	 	n;
Arg    	 	args[25];*/
char		buffer[25];
Widget		Manager1, RadioBox;

     CommonTestInit( argc, argv );

     n = 0;
     XtSetArg( args[n], XmNwidth, MGR_WIDTH ); n++;
     XtSetArg( args[n], XmNheight, MGR_HEIGHT ); n++;
     Manager1 = XmCreateForm( Shell1, "Manager1", args, n );
     XtManageChild( Manager1 );

    /*
     *  Create XmToggleButton widgets, with default resource values,
     *  which are not children of an XmRadioBox widget.
     */
     i = 0;

     for ( i = 0; i < NUM_TOGGLE_BUTTONS; i++ )
     {
        sprintf( buffer, "ToggleButton%d", i );

     	n = 0;
       if ( i == 0 )
	{
     		XtSetArg( args[n], XmNtopAttachment, XmATTACH_FORM ); n++;
	}
	else
	{
     		XtSetArg( args[n], XmNtopAttachment, XmATTACH_WIDGET ); n++;
     		XtSetArg( args[n], XmNtopWidget, ToggleButton[i-1] ); n++;
        }
     	XtSetArg( args[n], XmNleftAttachment, XmATTACH_FORM ); n++;
        ToggleButton[i] = 
           XmCreateToggleButton( Manager1, buffer, args, n );
        XtManageChild( ToggleButton[i] );
     }

    /*
     *  Create XmToggleButtonGadgets, with default resource values,
     *  that are not children of an XmRadioBox widget.
     */
     for ( i = 0; i < NUM_TOGGLE_BUTTONS; i++ )
     {
        sprintf( buffer, "ToggleButtonGadget%d", i );

      	n = 0;

	if ( i == 0 )
	{
     		XtSetArg( args[n], XmNtopAttachment, XmATTACH_FORM ); n++;
	}
	else
	{
     		XtSetArg( args[n], XmNtopAttachment, XmATTACH_WIDGET ); n++;
     		XtSetArg( args[n], XmNtopWidget, ToggleButtonGadget[i-1] );
                n++;
	}
     	XtSetArg( args[n], XmNleftAttachment, XmATTACH_WIDGET ); n++;
     	XtSetArg( args[n], XmNleftWidget, ToggleButton[i] ); n++;
     	ToggleButtonGadget[i] = 
          XmCreateToggleButtonGadget( Manager1, buffer, args, n );
     	XtManageChild( ToggleButtonGadget[i] );
      }

    /*
     *  Create control panel which enables testers to set
     *  ToggleButton and ToggleButtonGadget resource values.
     */
     CreateControlPanel();

     XtRealizeWidget( Shell1 );


     CommonPause(); 

    /*
     *  Print the default values of the XmNindicatorOn and 
     *  XmNindicatorType resource values for the ToggleButton
     *  and ToggleButtonGadget.
     */
     PrintIndicatorValues( ToggleButton[0] );
     PrintIndicatorValues( ToggleButtonGadget[0] );

     CommonPause(); 

    /*
     *  Destroy the ToggleButton and ToggleButtonGadget widgets, so that
     *  they can be re-created as children of the RadioBox.
     *  The reason these widgets are destroyed rather than just unmanaged,
     *  is that when tests are run automated, the automated instances of
     *  the widgets are managed at creation time and stay managed.
     */
     for ( i = 0; i < NUM_TOGGLE_BUTTONS; i++ )
     {
     	XtDestroyWidget( ToggleButton[i] );
     	XtDestroyWidget( ToggleButtonGadget[i] );
     }

    /*
     *  Reset the option menu settings in the Resource Control
     *  Panel.
     */
     n = 0;
     XtSetArg( args[n], XmNmenuHistory, IndicatorOnPB[0] ); n++;
     XtSetValues( IndicatorOnOptions, args, n );

     n = 0;
     XtSetArg( args[n], XmNmenuHistory, IndicatorTypePB[0] ); n++;
     XtSetValues( IndicatorTypeOptions, args, n );

    /*
     *  Recreate the widgets as children of a RadioBox.
     */
     XtSetArg( args[n], XmNleftAttachment, XmATTACH_FORM ); n++;
     XtSetArg( args[n], XmNtopAttachment, XmATTACH_FORM ); n++;
     XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM ); n++;
     XtSetArg( args[n], XmNorientation, XmHORIZONTAL ); n++;
     XtSetArg( args[n], XmNnumColumns, 2 ); n++;
     RadioBox = XmCreateRadioBox( Manager1, "RadioBox", args, n ); 

    /*
     *  Create XmToggleButtons and XmToggleButtonGadgets, 
     *  with default resource values, and a children of
     *  a XmRadioBox widget.
     */
     for ( i = 0; i < NUM_TOGGLE_BUTTONS; i++ ) 
     {
	sprintf( buffer, "ToggleButton%d", i );

        n = 0;
     	ToggleButton[i] =
           XmCreateToggleButton( RadioBox, buffer, args, n );
        XtManageChild( ToggleButton[i] );

	sprintf( buffer, "ToggleButtonGadget%d", i );

        n = 0;
     	ToggleButtonGadget[i] =
           XmCreateToggleButtonGadget( RadioBox, buffer, args, n );
        XtManageChild( ToggleButtonGadget[i] );
     }

     PrintIndicatorValues( ToggleButton[0] );
     PrintIndicatorValues( ToggleButtonGadget[0] );

     CommonPause(); 
     CommonPause(); 

     XtAppMainLoop( app_context );
}
