/* $XConsortium: SpinBox5.c /main/6 1996/04/30 13:57:26 schuldy $ */
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
 * 
 */
/*
 * HISTORY
 */
#include <testlib.h>

/*
 *  Defines
 */
#define MARGIN 10
#define SPIN_BOX_WIDTH 150
#define SPIN_BOX_HEIGHT 70
#define CONTROL_PANEL_WIDTH 950
#define CONTROL_PANEL_HEIGHT 600
#define NUM_ARROW_LAYOUT_BTNS 3 
#define NUM_ARROW_SIZE_BTNS 3 
#define NUM_DEF_ARROW_SENS_BTNS 4 
#define NUM_ARROW_SENS_BTNS 5 
#define NUM_CHILD_TYPE_BTNS 2

/*
 *  Structure definitions
 */
typedef struct _ArrowSensCD { 
	Widget		textfield;
	unsigned int	sensitivity;
} ArrowSensCD;

typedef struct _ChildTypeCD {
	Widget		textfield;
	unsigned int	child_type;
} ChildTypeCD;


/* 
 *  Global variables.
 */
Arg     	args[MAX_ARGS];
int     	n;  
Widget          SpinBox1, SpinBox2, StringTextF, NumericTextF,
                StringPositionTF, NumericPositionTF;
ArrowSensCD  	string_arrow_sens_cd[NUM_ARROW_SENS_BTNS],
                numeric_arrow_sens_cd[NUM_ARROW_SENS_BTNS];
ChildTypeCD     string_child_type_cd[NUM_CHILD_TYPE_BTNS],
        	numeric_child_type_cd[NUM_CHILD_TYPE_BTNS];

static char *arrow_layout_strings[NUM_ARROW_LAYOUT_BTNS] = {
       "Arrows Split",
       "Arrows Beginning",
       "Arrows End" };

static unsigned int arrow_layout[NUM_ARROW_LAYOUT_BTNS] =
	{ XmARROWS_SPLIT, XmARROWS_FLAT_BEGINNING, XmARROWS_FLAT_END };

static char *arrow_size_strings[NUM_ARROW_SIZE_BTNS] = {
       "16", "24", "32" };

static char *def_arrow_sens_strings[NUM_DEF_ARROW_SENS_BTNS] = {
       "Arrows Sensitive",
       "Arrows Decrement Sensitive",  
       "Arrows Increment Sensitive",  
       "Arrows Insensitive" };

static unsigned int def_arrow_sens[NUM_DEF_ARROW_SENS_BTNS] =
                    { XmARROWS_SENSITIVE, XmARROWS_DECREMENT_SENSITIVE,
                      XmARROWS_INCREMENT_SENSITIVE, XmARROWS_INSENSITIVE };

static char *arrow_sens_strings[NUM_ARROW_SENS_BTNS] = {
       "Arrows Sensitive",
       "Arrows Decrement Sensitive",  
       "Arrows Increment Sensitive",  
       "Arrows Insensitive",
       "Arrows Default Sensitive" };

static unsigned int arrow_sens[NUM_ARROW_SENS_BTNS] =
                    { XmARROWS_SENSITIVE, XmARROWS_DECREMENT_SENSITIVE,
                      XmARROWS_INCREMENT_SENSITIVE, XmARROWS_INSENSITIVE,
                      XmARROWS_DEFAULT_SENSITIVITY };

static char *child_type_strings[NUM_CHILD_TYPE_BTNS] = {
       "String",
       "Numeric" };

static unsigned int child_types[NUM_CHILD_TYPE_BTNS] =
                    { XmSTRING, XmNUMERIC };

void
ArrowLayoutCB( Widget widget, XtPointer client_data, XtPointer call_data )
{
    unsigned int layout = *((unsigned int *)client_data);

    n = 0;
    XtSetArg( args[n], XmNarrowLayout, layout ); n++; 
    XtSetValues( SpinBox1, args, n );
    XtSetValues( SpinBox2, args, n );
}

void
ArrowSizeCB( Widget widget, XtPointer client_data, XtPointer call_data )
{
    char *arrow_size = (char *)client_data;
    int  size;

    size = atoi( arrow_size );

    n = 0;
    XtSetArg( args[n], XmNarrowSize, size ); n++;
    XtSetValues( SpinBox1, args, n );
    XtSetValues( SpinBox2, args, n );
}
    
void
DefArrowSensitiveCB( Widget widget, XtPointer client_data, XtPointer call_data )
{
     unsigned int sensitivity = *((unsigned int *)client_data);

     n = 0;
     XtSetArg( args[n], XmNdefaultArrowSensitivity, sensitivity ); n++; 
     XtSetValues( SpinBox1, args, n );
     XtSetValues( SpinBox2, args, n );
}

void
InitialDelayCB( Widget widget, XtPointer client_data, XtPointer call_data )
{
    char *value;
    int  delay;

    n = 0;
    XtSetArg( args[n], XmNvalue, &value ); n++;
    XtGetValues( widget, args, n );

    delay = atoi( value );

    n = 0;
    XtSetArg( args[n], XmNinitialDelay, delay ); n++;
    XtSetValues( SpinBox1, args, n );
    XtSetValues( SpinBox2, args, n );
}

void
RepeatDelayCB( Widget widget, XtPointer client_data, XtPointer call_data )
{
    char *value;
    int  delay;

    n = 0;
    XtSetArg( args[n], XmNvalue, &value ); n++;
    XtGetValues( widget, args, n );

    delay = atoi( value );

    n = 0;
    XtSetArg( args[n], XmNrepeatDelay, delay ); n++;
    XtSetValues( SpinBox1, args, n );
    XtSetValues( SpinBox2, args, n );
}


void
ArrowSensCB( Widget widget, XtPointer client_data, XtPointer call_data )
{
    ArrowSensCD  *arrow_sens_cd;
    arrow_sens_cd = (ArrowSensCD *)client_data;

    n = 0;
    XtSetArg( args[n], XmNarrowSensitivity, arrow_sens_cd->sensitivity ); n++; 
    XtSetValues( arrow_sens_cd->textfield, args, n );
}

void
DecimalPtsCB( Widget widget, XtPointer client_data, XtPointer call_data )
{
    Widget  TextF = (Widget)client_data;
    char *value;
    int  decimals;

    n = 0;
    XtSetArg( args[n], XmNvalue, &value ); n++;
    XtGetValues( widget, args, n );

    decimals = atoi( value );

    n = 0;
    XtSetArg( args[n], XmNdecimalPoints, decimals ); n++;
    XtSetValues( TextF, args, n );
}

void
IncrValueCB( Widget widget, XtPointer client_data, XtPointer call_data )
{
    Widget  TextF = (Widget)client_data;
    char *value;
    int  incr;

    n = 0;
    XtSetArg( args[n], XmNvalue, &value ); n++;
    XtGetValues( widget, args, n );

    incr = atoi( value );

    n = 0;
    XtSetArg( args[n], XmNincrementValue, incr ); n++;
    XtSetValues( TextF, args, n );
}

void
MaxValueCB( Widget widget, XtPointer client_data, XtPointer call_data )
{
    Widget  TextF = (Widget)client_data;
    char *value;
    int  max;

    n = 0;
    XtSetArg( args[n], XmNvalue, &value ); n++;
    XtGetValues( widget, args, n );

    max = atoi( value );

    n = 0;
    XtSetArg( args[n], XmNmaximumValue, max ); n++;
    XtSetValues( TextF, args, n );
}


void
MinValueCB( Widget widget, XtPointer client_data, XtPointer call_data )
{
    Widget  TextF = (Widget)client_data;
    char *value;
    int  min;

    n = 0;
    XtSetArg( args[n], XmNvalue, &value ); n++;
    XtGetValues( widget, args, n );

    min = atoi( value );

    n = 0;
    XtSetArg( args[n], XmNminimumValue, min ); n++;
    XtSetValues( TextF, args, n );
}

void
NumValuesCB( Widget widget, XtPointer client_data, XtPointer call_data )
{}

void
UpdatePositionCB( Widget widget, XtPointer client_data, XtPointer call_data )
{
   Widget  TextF = (Widget)client_data;
   Widget  SpinBChild;
   int     position;
   char    buffer[100];

    SpinBChild = ( widget == SpinBox1 ) ? StringTextF : NumericTextF;


    n = 0;
    XtSetArg( args[n], XmNposition, &position ); n++;
    XtGetValues( SpinBChild, args, n );

    sprintf( buffer, "%d", position );

    n = 0;
    XtSetArg( args[n], XmNvalue, buffer ); n++;
    XtSetValues( TextF, args, n );
}




void
PositionCB( Widget widget, XtPointer client_data, XtPointer call_data )
{
    Widget  TextF = (Widget)client_data;
    char *value;
    int  position;

    n = 0;
    XtSetArg( args[n], XmNvalue, &value ); n++;
    XtGetValues( widget, args, n );

    position = atoi( value );

    n = 0;
    XtSetArg( args[n], XmNposition, position ); n++;
    XtSetValues( TextF, args, n );
}

void
ChildTypeCB( Widget widget, XtPointer client_data, XtPointer call_data )
{
    ChildTypeCD  *child_type_cd = (ChildTypeCD *)client_data; 

    n = 0;
    XtSetArg( args[n], XmNspinBoxChildType, child_type_cd->child_type ); n++;
    if ( child_type_cd->child_type == XmNUMERIC ) {
	XtSetArg( args[n], XmNpositionType, XmPOSITION_VALUE ); n++;
	XtSetArg( args[n], XmNposition, 0 ); n++;
    }
    XtSetValues( child_type_cd->textfield, args, n );
}

Widget
CreateSpinBoxResourceControls( Widget parent, Widget top_widget, 
                               Widget left_widget, Widget right_widget )
                               
                
{
	XmString 	string;
	Widget  	SBResLabel, RowColumn;
	Widget  	ArrowLayoutLabel, ArrowLayoutPulldown, 
                        ArrowLayoutOptions, 
                	ArrowLayoutPB[NUM_ARROW_LAYOUT_BTNS]; 
	Widget  	ArrowSizeLabel, ArrowSizePulldown, ArrowSizeOptions, 
                	ArrowSizePB[NUM_ARROW_SIZE_BTNS]; 
	Widget  	DefArrowSensitiveLabel, DefArrowSensitivePulldown, 
                	DefArrowSensitiveOptions, 
                	DefArrowSensitivePB[NUM_DEF_ARROW_SENS_BTNS];
        Widget  	InitialDelayLabel, InitialDelayTF;
        Widget  	RepeatDelayLabel, RepeatDelayTF;
	int		i;
	char		buffer[30];

        n = 0;
        XtSetArg( args[n], XmNtopAttachment, XmATTACH_WIDGET ); n++;
        XtSetArg( args[n], XmNtopWidget, top_widget ); n++;
        XtSetArg( args[n], XmNleftAttachment, XmATTACH_WIDGET ); n++;
        XtSetArg( args[n], XmNleftWidget, left_widget ); n++;
        XtSetArg( args[n], XmNrightAttachment, XmATTACH_WIDGET ); n++;
        XtSetArg( args[n], XmNrightWidget, right_widget ); n++;
   	XtSetArg(args[n], XmNnumColumns, 5); n++;
   	XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
   	XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
        RowColumn = XmCreateRowColumn( parent, "RowColumn", args, n );
        XtManageChild( RowColumn );

      /*
       *  Create label and option menu for XmNarrowLayout resource.
       */
        string = XmStringCreateLtoR("Arrow Layout",
                   XmFONTLIST_DEFAULT_TAG );
        n = 0;
        XtSetArg( args[n], XmNlabelString, string ); n++;
        ArrowLayoutLabel = XmCreateLabel( RowColumn, "ArrowLayoutLabel",
                             args, n );
        XtManageChild( ArrowLayoutLabel );
        XmStringFree( string );

        n = 0;
        ArrowLayoutPulldown = XmCreatePulldownMenu(RowColumn,
                                                   "ArrowLayoutPulldown",
                                                   args, n);

        for ( i = 0; i < NUM_ARROW_LAYOUT_BTNS; i++ )
        {
           string = XmStringCreateLtoR( arrow_layout_strings[i],
                      XmFONTLIST_DEFAULT_TAG );
           sprintf( buffer, "ArrowLayoutPB%d", i );
           n = 0;
           XtSetArg( args[n], XmNlabelString, string ); n++;
           ArrowLayoutPB[i] = 
           XmCreatePushButton( ArrowLayoutPulldown, buffer, args, n );
           XmStringFree( string );

           XtAddCallback( ArrowLayoutPB[i], XmNactivateCallback,
                          ArrowLayoutCB, (XtPointer)&arrow_layout[i] ); 
        }
        XtManageChildren( ArrowLayoutPB, NUM_ARROW_LAYOUT_BTNS );


        n = 0;
        XtSetArg( args[n], XmNmenuHistory, 
                  ArrowLayoutPB[NUM_ARROW_LAYOUT_BTNS - 1] ); n++;
        XtSetArg( args[n], XmNsubMenuId, ArrowLayoutPulldown ); n++;
        ArrowLayoutOptions =
        XmCreateOptionMenu( RowColumn, "ArrowLayoutOptions", args, n );
        XtManageChild( ArrowLayoutOptions );
        
      /*
       *  Create label and option menu for XmNarrowSize resource.
       */
        string = XmStringCreateLtoR("Arrow Size",
                   XmFONTLIST_DEFAULT_TAG );
        n = 0;
        XtSetArg( args[n], XmNlabelString, string ); n++;
        ArrowSizeLabel = XmCreateLabel( RowColumn, "ArrowSizeLabel",
                             args, n );
        XtManageChild( ArrowSizeLabel );
        XmStringFree( string );

        n = 0;
        ArrowSizePulldown = XmCreatePulldownMenu(RowColumn,
                                                 "ArrowSizePulldown",
                                                 args, n);

        for ( i = 0; i < NUM_ARROW_SIZE_BTNS; i++ )
        {
           string = XmStringCreateLtoR( arrow_size_strings[i],
                      XmFONTLIST_DEFAULT_TAG );
           sprintf( buffer, "ArrowSizePB%d", i );
           n = 0;
           XtSetArg( args[n], XmNlabelString, string ); n++;
           ArrowSizePB[i] =
           XmCreatePushButton( ArrowSizePulldown, buffer, args, n );
           XmStringFree( string );

           XtAddCallback( ArrowSizePB[i], XmNactivateCallback,
                          ArrowSizeCB, (XtPointer)arrow_size_strings[i] );
        }
        XtManageChildren( ArrowSizePB, NUM_ARROW_SIZE_BTNS );

        n = 0;
        XtSetArg( args[n], XmNmenuHistory, ArrowSizePB[0] ); n++;
        XtSetArg( args[n], XmNsubMenuId, ArrowSizePulldown ); n++;
        ArrowSizeOptions =
        XmCreateOptionMenu( RowColumn, "ArrowSizeOptions", args, n );
        XtManageChild( ArrowSizeOptions );

      /*
       *  Create label and option menu for XmNdefaultArrowSensitivity resource.
       */
        string = XmStringCreateLtoR("Default Arrow Sensitivity",
                   XmFONTLIST_DEFAULT_TAG );
                   
        n = 0;
        XtSetArg( args[n], XmNlabelString, string ); n++;
        DefArrowSensitiveLabel = 
        XmCreateLabel( RowColumn, "DefArrowSensitiveLabel", args, n );
        XtManageChild( DefArrowSensitiveLabel );
        XmStringFree( string );

        n = 0;
        DefArrowSensitivePulldown = 
        XmCreatePulldownMenu(RowColumn, "DefArrowSensitivePulldown",
                             args, n);

        for ( i = 0; i < NUM_DEF_ARROW_SENS_BTNS; i++ )
        {
           string = XmStringCreateLtoR( def_arrow_sens_strings[i],
                      XmFONTLIST_DEFAULT_TAG );
           sprintf( buffer, "DefArrowSensitivePB%d", i );
           n = 0;
           XtSetArg( args[n], XmNlabelString, string ); n++;
           DefArrowSensitivePB[i] =
           XmCreatePushButton( DefArrowSensitivePulldown, buffer, args, n );
           XmStringFree( string );

           XtAddCallback( DefArrowSensitivePB[i], XmNactivateCallback,
                          DefArrowSensitiveCB, (XtPointer)&def_arrow_sens[i] );
        }
        XtManageChildren( DefArrowSensitivePB, NUM_DEF_ARROW_SENS_BTNS );

        n = 0;
        XtSetArg( args[n], XmNmenuHistory, DefArrowSensitivePB[0] ); n++;
        XtSetArg( args[n], XmNsubMenuId, DefArrowSensitivePulldown ); n++;
        DefArrowSensitiveOptions =
        XmCreateOptionMenu( RowColumn, "DefArrowSensitiveOptions", args, n );
        XtManageChild( DefArrowSensitiveOptions );

      /*
       *  Create label and text field for XmNinitialDelay resource.
       */
        string = XmStringCreateLtoR("Initial Delay",
                   XmFONTLIST_DEFAULT_TAG );
        n = 0;
        XtSetArg( args[n], XmNlabelString, string ); n++;
        InitialDelayLabel =
        XmCreateLabel( RowColumn, "InitialDelayLabel", args, n );
        XtManageChild( InitialDelayLabel );
        XmStringFree( string );

        n = 0;
        XtSetArg( args[n], XmNvalue, "250" ); n++;
        InitialDelayTF = 
        XmCreateTextField( RowColumn, "InitialDelayTF", args, n );  
        XtManageChild( InitialDelayTF );

        XtAddCallback( InitialDelayTF, XmNactivateCallback,
                       InitialDelayCB, (XtPointer)0 );

      /*
       *  Create label and text field for XmNinitialDelay resource.
       */
        string = XmStringCreateLtoR("Repeat Delay",
                   XmFONTLIST_DEFAULT_TAG );
        n = 0;
        XtSetArg( args[n], XmNlabelString, string ); n++;
        RepeatDelayLabel =
        XmCreateLabel( RowColumn, "RepeatDelayLabel", args, n );
        XtManageChild( RepeatDelayLabel );
        XmStringFree( string );

        n = 0;
        XtSetArg( args[n], XmNvalue, "200" ); n++;
        RepeatDelayTF =
        XmCreateTextField( RowColumn, "RepeatDelayTF", args, n );
        XtManageChild( RepeatDelayTF );

        XtAddCallback( RepeatDelayTF, XmNactivateCallback,
                       RepeatDelayCB, (XtPointer)0 );
     
     return( RowColumn );
}

Widget
CreateConstraintResourceControls( Widget parent, Widget left_widget,
                             Widget bottom_widget,
 			     unsigned int child_type )
{
	XmString	string;
	Widget		RowColumn;
	Widget          ArrowSensLabel, ArrowSensPulldown, ArrowSensOptions,
	                ArrowSensPB[NUM_ARROW_SENS_BTNS];
	Widget          DecimalPtsLabel, DecimalPtsTF;
	Widget          IncrValueLabel, IncrValueTF;
	Widget          MaxValueLabel, MaxValueTF;
	Widget          MinValueLabel, MinValueTF;
	Widget          NumValuesLabel, NumValuesTF;
	Widget          PositionLabel;
	Widget          ChildTypeLabel, ChildTypePulldown, ChildTypeOptions,
                        ChildTypePB[NUM_CHILD_TYPE_BTNS];
	Widget          ValuesLabel, ValuesList;
	char		buffer[30], buffer2[30];
	Widget          client_widget;
	int		i, j;	
        XmStringTable   stringTable;

        client_widget = ( child_type == XmSTRING ) ? StringTextF :
                        NumericTextF;

        n = 0;
        XtSetArg( args[n], XmNleftAttachment, XmATTACH_WIDGET ); n++;
        XtSetArg( args[n], XmNleftWidget, left_widget ); n++;
        XtSetArg( args[n], XmNbottomAttachment, XmATTACH_WIDGET ); n++;
        XtSetArg( args[n], XmNbottomWidget, bottom_widget ); n++;
        XtSetArg(args[n], XmNnumColumns, 9); n++;
        XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
        XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
        RowColumn = XmCreateRowColumn( parent, "RowColumn", args, n );
	XtManageChild( RowColumn );


      /*
       *  Create label and option menu for XmNarrowSensitivity resource.
       */
        string = XmStringCreateLtoR("Arrow Sensitivity",
                   XmFONTLIST_DEFAULT_TAG );
        n = 0;
        XtSetArg( args[n], XmNlabelString, string ); n++;
        ArrowSensLabel = XmCreateLabel( RowColumn, "ArrowSensLabel",
                             args, n );
        XtManageChild( ArrowSensLabel );
        XmStringFree( string );

        n = 0;
        ArrowSensPulldown = XmCreatePulldownMenu(RowColumn,
                                                 "ArrowSensPulldown",
                                                 args, n);

        for ( i = 0; i < NUM_ARROW_SENS_BTNS; i++ )
        {
           string = XmStringCreateLtoR( arrow_sens_strings[i],
                      XmFONTLIST_DEFAULT_TAG );
           if ( child_type == XmSTRING )
                sprintf( buffer, "StringArrowSensPB%d", i );
           else if ( child_type == XmNUMERIC )
                sprintf( buffer, "NumericArrowSensPB%d", i );

           n = 0;
           XtSetArg( args[n], XmNlabelString, string ); n++;
           ArrowSensPB[i] = 
           XmCreatePushButton( ArrowSensPulldown, buffer, args, n );
           XmStringFree( string );

           if ( child_type == XmSTRING )
           {
              string_arrow_sens_cd[i].textfield = client_widget; 
              string_arrow_sens_cd[i].sensitivity = arrow_sens[i];
              XtAddCallback( ArrowSensPB[i], XmNactivateCallback,
                             ArrowSensCB, &string_arrow_sens_cd[i]); 
           }
           else if ( child_type = XmNUMERIC )
           {
              numeric_arrow_sens_cd[i].textfield = client_widget; 
              numeric_arrow_sens_cd[i].sensitivity = arrow_sens[i];
              XtAddCallback( ArrowSensPB[i], XmNactivateCallback,
                             ArrowSensCB, &numeric_arrow_sens_cd[i]); 
           }
        }
        XtManageChildren( ArrowSensPB, NUM_ARROW_SENS_BTNS );

        if ( child_type == XmSTRING )
             sprintf( buffer, "StringArrowSensOptions", i );
        else if ( child_type == XmNUMERIC )
             sprintf( buffer, "NumericArrowSensOptions", i );

        n = 0;
        XtSetArg( args[n], XmNmenuHistory, 
                           ArrowSensPB[NUM_ARROW_SENS_BTNS - 1] ); n++;
        XtSetArg( args[n], XmNsubMenuId, ArrowSensPulldown ); n++;
        ArrowSensOptions =
        XmCreateOptionMenu( RowColumn, buffer, args, n );
        XtManageChild( ArrowSensOptions );

        string = XmStringCreateLtoR("Decimal Points",
                   XmFONTLIST_DEFAULT_TAG );
        n = 0;
        XtSetArg( args[n], XmNlabelString, string ); n++;
        DecimalPtsLabel =
        XmCreateLabel( RowColumn, "DecimalPtsLabel", args, n );
        XtManageChild( DecimalPtsLabel );
        XmStringFree( string );

        n = 0;
        XtSetArg( args[n], XmNvalue, "0" ); n++;
        if ( child_type == XmSTRING )
           sprintf( buffer, "%s", "StringDecimalPtsTF" );
        else if ( child_type == XmNUMERIC )
           sprintf( buffer, "%s", "NumericDecimalPtsTF" );
        DecimalPtsTF =
        XmCreateTextField( RowColumn, buffer, args, n );
        XtManageChild( DecimalPtsTF );

        XtAddCallback( DecimalPtsTF, XmNactivateCallback,
                       DecimalPtsCB, (XtPointer)client_widget );

        string = XmStringCreateLtoR("Increment Value",
                   XmFONTLIST_DEFAULT_TAG );
        n = 0;
        XtSetArg( args[n], XmNlabelString, string ); n++;
        IncrValueLabel =
        XmCreateLabel( RowColumn, "IncrValueLabel", args, n );
        XtManageChild( IncrValueLabel );
        XmStringFree( string );

        n = 0;
        XtSetArg( args[n], XmNvalue, "1" ); n++;
        if ( child_type == XmSTRING )
           sprintf( buffer, "%s", "StringIncrValueTF" );
        else if ( child_type == XmNUMERIC )
           sprintf( buffer, "%s", "NumericIncrValueTF" );
        IncrValueTF =
        XmCreateTextField( RowColumn, buffer, args, n );
        XtManageChild( IncrValueTF );

        XtAddCallback( IncrValueTF, XmNactivateCallback,
                       IncrValueCB, (XtPointer)client_widget );

        string = XmStringCreateLtoR("Maximum Value",
                   XmFONTLIST_DEFAULT_TAG );
        n = 0;
        XtSetArg( args[n], XmNlabelString, string ); n++;
        MaxValueLabel =
        XmCreateLabel( RowColumn, "MaxValueLabel", args, n );
        XtManageChild( MaxValueLabel );
        XmStringFree( string );

        n = 0;
        XtSetArg( args[n], XmNvalue, "10" ); n++;
        if ( child_type == XmSTRING )
           sprintf( buffer, "%s", "StringMaxValueTF" );
        else if ( child_type == XmNUMERIC )
           sprintf( buffer, "%s", "NumericMaxValueTF" );
        MaxValueTF =
        XmCreateTextField( RowColumn, buffer, args, n );
        XtManageChild( MaxValueTF );

        XtAddCallback( MaxValueTF, XmNactivateCallback,
                       MaxValueCB, (XtPointer)client_widget );

        string = XmStringCreateLtoR("Minimum Value",
                   XmFONTLIST_DEFAULT_TAG );
        n = 0;
        XtSetArg( args[n], XmNlabelString, string ); n++;
        MinValueLabel =
        XmCreateLabel( RowColumn, "MinValueLabel", args, n );
        XtManageChild( MinValueLabel );
        XmStringFree( string );

        n = 0;
        XtSetArg( args[n], XmNvalue, "0" ); n++;
        if ( child_type == XmSTRING )
           sprintf( buffer, "%s", "StringMinValueTF" );
        else if ( child_type == XmNUMERIC )
           sprintf( buffer, "%s", "NumericMinValueTF" );
        MinValueTF =
        XmCreateTextField( RowColumn, buffer, args, n );
        XtManageChild( MinValueTF );

        XtAddCallback( MinValueTF, XmNactivateCallback,
                       MinValueCB, (XtPointer)client_widget );

        string = XmStringCreateLtoR("Num Values",
                   XmFONTLIST_DEFAULT_TAG );
        n = 0;
        XtSetArg( args[n], XmNlabelString, string ); n++;
        NumValuesLabel =
        XmCreateLabel( RowColumn, "NumValuesLabel", args, n );
        XtManageChild( NumValuesLabel );
        XmStringFree( string );

        client_widget = ( child_type == XmSTRING ) ? StringTextF :
                        NumericTextF;

      /*
       *  Get current value of XmNnumValues resource.
       */
        n = 0;
        XtSetArg( args[n], XmNnumValues, &j ); n++;
        XtGetValues( client_widget, args, n );

        sprintf( buffer, "%d", j );

        n = 0;
        XtSetArg( args[n], XmNvalue, buffer ); n++;
        if ( child_type == XmSTRING )
           sprintf( buffer2, "%s", "StringNumValuesTF" );
        else if ( child_type == XmNUMERIC )
           sprintf( buffer2, "%s", "NumericNumValuesTF" );
        NumValuesTF =
        XmCreateTextField( RowColumn, buffer2, args, n );
        XtManageChild( NumValuesTF );

        XtAddCallback( NumValuesTF, XmNactivateCallback,
                       NumValuesCB, (XtPointer)client_widget );

        string = XmStringCreateLtoR("Position",
                   XmFONTLIST_DEFAULT_TAG );
        n = 0;
        XtSetArg( args[n], XmNlabelString, string ); n++;
        PositionLabel =
        XmCreateLabel( RowColumn, "PositionLabel", args, n );
        XtManageChild( PositionLabel );
        XmStringFree( string );

        n = 0;
        XtSetArg( args[n], XmNvalue, "0" ); n++;
        if ( child_type == XmSTRING )
        {
           sprintf( buffer, "%s", "StringPositionTF" );
           StringPositionTF =
           XmCreateTextField( RowColumn, buffer, args, n );
           XtManageChild( StringPositionTF );
           XtAddCallback( StringPositionTF, XmNactivateCallback,
                          PositionCB, (XtPointer)client_widget );
           XtAddCallback( SpinBox1, XmNvalueChangedCallback,
                          UpdatePositionCB, (XtPointer)StringPositionTF );
        }
        else if ( child_type == XmNUMERIC )
        {
           sprintf( buffer, "%s", "NumericPositionTF" );
           NumericPositionTF =
           XmCreateTextField( RowColumn, buffer, args, n );
           XtManageChild( NumericPositionTF );
           XtAddCallback( NumericPositionTF, XmNactivateCallback,
                          PositionCB, (XtPointer)client_widget );
           XtAddCallback( SpinBox2, XmNvalueChangedCallback,
                          UpdatePositionCB, (XtPointer)NumericPositionTF );
        }

        string = XmStringCreateLtoR("SpinBox Child Type",
                   XmFONTLIST_DEFAULT_TAG );
        n = 0;
        XtSetArg( args[n], XmNlabelString, string ); n++;
        ChildTypeLabel =
        XmCreateLabel( RowColumn, "ChildTypeLabel", args, n );
        XtManageChild( ChildTypeLabel );
        XmStringFree( string );

        n = 0;
        ChildTypePulldown = XmCreatePulldownMenu(RowColumn,
                                                 "ChildTypePulldown",
                                                 args, n);

        for ( i = 0; i < NUM_CHILD_TYPE_BTNS; i++ )
        {
           string = XmStringCreateLtoR( child_type_strings[i],
                      XmFONTLIST_DEFAULT_TAG );
           if ( child_type == XmSTRING )
              sprintf( buffer, "StringChildTypePB%d", i );
           else if ( child_type == XmNUMERIC )
              sprintf( buffer, "NumericChildTypePB%d", i );
           n = 0;
           XtSetArg( args[n], XmNlabelString, string ); n++;
           ChildTypePB[i] =
           XmCreatePushButton( ChildTypePulldown, buffer, args, n );
           XmStringFree( string );

           if ( child_type == XmSTRING )
           {
              string_child_type_cd[i].textfield = client_widget;
              string_child_type_cd[i].child_type = child_types[i];
              XtAddCallback( ChildTypePB[i], XmNactivateCallback,
                             ChildTypeCB, &string_child_type_cd[i] );
           }
           else if ( child_type == XmNUMERIC )
           {
              numeric_child_type_cd[i].textfield = client_widget;
              numeric_child_type_cd[i].child_type = child_types[i];
              XtAddCallback( ChildTypePB[i], XmNactivateCallback,
                             ChildTypeCB, &numeric_child_type_cd[i] );
           }
        }
        XtManageChildren( ChildTypePB, NUM_CHILD_TYPE_BTNS );

        n = 0;
        if ( child_type == XmSTRING )
        {
           XtSetArg( args[n], XmNmenuHistory, ChildTypePB[0] ); n++;
        }
        else if ( child_type == XmNUMERIC )
        {
           XtSetArg( args[n], XmNmenuHistory, ChildTypePB[1] ); n++;
        }
        XtSetArg( args[n], XmNsubMenuId, ChildTypePulldown ); n++;
        ChildTypeOptions =
        XmCreateOptionMenu( RowColumn, "ChildTypeOptions", args, n );
        XtManageChild( ChildTypeOptions );

        string = XmStringCreateLtoR( "Values",
                      XmFONTLIST_DEFAULT_TAG );

        n = 0;
        XtSetArg( args[n], XmNlabelString, string ); n++;
        ValuesLabel =
        XmCreateLabel( RowColumn, "ValuesLabel", args, n );
        XtManageChild( ValuesLabel );
        XmStringFree( string );

       /*
        *  Get current values for XmNvalues resource.
        */
        n = 0;
        XtSetArg( args[n], XmNnumValues, &j ); n++;
        XtSetArg( args[n], XmNvalues, &stringTable ); n++;
        XtGetValues( client_widget, args, n );

       /*
        *  Assign current string values to List for display.
        */
        if ( child_type == XmSTRING )
           sprintf( buffer, "%s", "StringValuesList" );
        else if ( child_type == XmNUMERIC )
           sprintf( buffer, "%s", "NumericValuesList" );
        n = 0;
        XtSetArg( args[n], XmNitemCount, j ); n++;
        XtSetArg( args[n], XmNitems, stringTable ); n++;
        XtSetArg( args[n], XmNvisibleItemCount, 1 ); n++;
        ValuesList = XmCreateScrolledList( RowColumn, buffer,
                     args, n );
        XtManageChild( ValuesList );

     return( RowColumn );
}

void
CreateControlPanel()
{
	Widget	PopupShell, Form1, SBResLabel, SBStringResLabel,
                SBNumericResLabel;
	Widget  Manager1, Manager2, Manager3;
	Widget  ArrowSensLabel, ArrowSensPulldown, ArrowSensOptions,
	        ArrowSensPB[NUM_ARROW_SENS_BTNS];
	Widget  DecimalPtsLabel, DecimalPtsTF;
	Widget  IncrValueLabel, IncrValueTF;
	Widget  MaxValueLabel, MaxValueTF;
	Widget  MinValueLabel, MinValueTF;
	Widget  NumValuesLabel, NumValuesTF;
	Widget  PositionLabel, PositionTF;
	Widget  ChildTypeLabel, ChildTypePulldown, ChildTypeOptions,
                ChildTypePB[NUM_CHILD_TYPE_BTNS];
	Widget  ValuesLabel, ValuesComboBox;
	XmString	string;
	int	i;

        n = 0;
        XtSetArg( args[n], XmNx, 0 ); n++;
        XtSetArg( args[n], XmNy, 500 ); n++;
        XtSetArg( args[n], XmNwidth,  CONTROL_PANEL_WIDTH ); n++;
        XtSetArg( args[n], XmNheight,  CONTROL_PANEL_HEIGHT ); n++;
        PopupShell = XtCreatePopupShell("Resource Control Panel",
                                         topLevelShellWidgetClass,
                                         Shell1, args, n );
        XtManageChild( PopupShell );

        n = 0;
	Form1 = XmCreateForm( PopupShell, "Form1", args, n );
        XtManageChild( Form1 );



        string = XmStringCreateLtoR("SpinBox Resources",
				XmFONTLIST_DEFAULT_TAG );

        n = 0;
        XtSetArg( args[n], XmNtopAttachment, XmATTACH_FORM ); n++;
        XtSetArg( args[n], XmNleftAttachment, XmATTACH_FORM ); n++;
        XtSetArg( args[n], XmNlabelString, string ); n++;
        XtSetArg( args[n], XmNbackground, CommonGetColor("white")); n++;
        XtSetArg( args[n], XmNforeground, CommonGetColor("blue")); n++;
        SBResLabel = XmCreateLabel( Form1, "SBResLabel", args, n );
        XtManageChild( SBResLabel );

        XmStringFree( string );

        Manager1 = 
        CreateSpinBoxResourceControls( Form1, SBResLabel, Form1, Form1 );

      /*
       *  Create SpinBox Constraint Resource Panel for an XmSTRING child
       *  type.
       */
        Manager2 = CreateConstraintResourceControls( Form1, Form1, Form1, 
                          XmSTRING );

        string = XmStringCreateLtoR("SpinBox Constraint Resources - XmSTRING",
                     XmFONTLIST_DEFAULT_TAG );
        n = 0; 
        XtSetArg( args[n], XmNtopAttachment, XmATTACH_WIDGET ); n++;
        XtSetArg( args[n], XmNtopWidget, Manager1 ); n++;
        XtSetArg( args[n], XmNleftAttachment, XmATTACH_FORM ); n++;
        XtSetArg( args[n], XmNlabelString, string ); n++;
        XtSetArg( args[n], XmNbackground, CommonGetColor("white")); n++;
        XtSetArg( args[n], XmNforeground, CommonGetColor("blue")); n++;
        SBStringResLabel = XmCreateLabel( Form1, "SBStringResLabel",
                                          args, n );
        XtManageChild( SBStringResLabel );

        XmStringFree( string );

      /*
       *  Create SpinBox Constraint Resource Panel for an XmNUMERIC child
       *  type.
       */
        Manager3 = CreateConstraintResourceControls( Form1, Manager2,
                          Form1, XmNUMERIC );

        string = XmStringCreateLtoR("SpinBox Constraint Resources - XmNUMERIC",
                                XmFONTLIST_DEFAULT_TAG );

        n = 0;
        XtSetArg( args[n], XmNtopAttachment, XmATTACH_WIDGET ); n++;
        XtSetArg( args[n], XmNtopWidget, Manager1 ); n++;
        XtSetArg( args[n], XmNx, CONTROL_PANEL_WIDTH / 2 - MARGIN ); n++;
        XtSetArg( args[n], XmNlabelString, string ); n++;
        XtSetArg( args[n], XmNbackground, CommonGetColor("white")); n++;
        XtSetArg( args[n], XmNforeground, CommonGetColor("blue")); n++;
        SBNumericResLabel = XmCreateLabel( Form1, "SBStringResLabel",
                                          args, n );
        XtManageChild( SBNumericResLabel );

        XmStringFree( string );

   XtPopup( PopupShell, XtGrabNone );

}

  void
MotifConvergeTP3()
{
  n = 0;
  XtSetArg( args[n], XmNminimumValue, 5 ); n++;
  XtSetArg( args[n], XmNmaximumValue, 25 ); n++;
  XtSetArg( args[n], XmNincrementValue, 5 ); n++;
  XtSetValues( SpinBox2, args, n );

  n = 0;
  XtSetArg( args[n], XmNvalue, 2 ); n++;
  XtSetValues( NumericTextF, args, n );

  CommonPause();
  return;
}

  void
MotifConvergeTP4()
{
  n = 0;
  XtSetArg( args[n], XmNvalue, 30 ); n++;
  XtSetValues( NumericTextF, args, n );

  CommonPause();
  return;
}

  void
MotifConvergeTP5()
{
  n = 0;
  XtSetArg( args[n], XmNvalue, 11 ); n++;
  XtSetValues( NumericTextF, args, n );

  CommonPause();
  return;
}

  void
MotifConvergeTP6()
{
  n = 0;
  XtSetArg( args[n], XmNvalue, 20 ); n++;
  XtSetValues( NumericTextF, args, n );

  CommonPause();
  return;
}

  void
MotifConvergeTP7()
{
  n = 0;
  XtSetArg( args[n], XmNvalue, .10 ); n++;
  XtSetValues( NumericTextF, args, n );

  CommonPause();
  return;
}

void
main(int argc, char **argv)
{
	Widget	  MainForm, ControlPanel, Separator1, Separator2,
                  Label1, Label2;
	XmString  string;

	CommonTestInit( argc, argv );

        n = 0;
        XtSetArg( args[n], XmNx, 0 ); n++;
        XtSetArg( args[n], XmNy, 0 ); n++;
        XtSetValues( Shell1, args, n );

        XtRealizeWidget( Shell1 );

        n = 0;
        MainForm = XmCreateForm( Shell1, "MainForm", args, n );
        XtManageChild( MainForm );

        string = XmStringCreateLtoR("XmSTRING:", XmFONTLIST_DEFAULT_TAG );
        n = 0;
        XtSetArg( args[n], XmNtopAttachment, XmATTACH_FORM ); n++;
        XtSetArg( args[n], XmNleftAttachment, XmATTACH_FORM ); n++;
        XtSetArg( args[n], XmNwidth, SPIN_BOX_WIDTH ); n++;
        XtSetArg( args[n], XmNlabelString, string ); n++;
        XtSetArg( args[n], XmNalignment, XmALIGNMENT_CENTER ); n++;
        Label1 = XmCreateLabel( MainForm, "Label1", args, n );
        XtManageChild( Label1 );

        XmStringFree( string );
        n = 0;
        XtSetArg( args[n], XmNtopAttachment, XmATTACH_WIDGET ); n++;
        XtSetArg( args[n], XmNtopWidget, Label1 ); n++;
        XtSetArg( args[n], XmNleftAttachment, XmATTACH_FORM ); n++;
        XtSetArg( args[n], XmNwidth, SPIN_BOX_WIDTH ); n++;
        XtSetArg( args[n], XmNheight, SPIN_BOX_HEIGHT ); n++;
	XtSetArg( args[n], XmNarrowLayout, XmARROWS_FLAT_END ); n++;
	XtSetArg( args[n], XmNarrowOrientation, XmARROWS_HORIZONTAL ); n++;
        SpinBox1 = XmCreateSpinBox( MainForm, "SpinBox1", args, n );
        XtManageChild( SpinBox1 );

        n = 0;
        StringTextF = XmCreateTextField( SpinBox1, "StringTextF", args, n );
        XtManageChild( StringTextF );



        string = XmStringCreateLtoR("XmNUMERIC:", XmFONTLIST_DEFAULT_TAG );
        n = 0;
        XtSetArg( args[n], XmNtopAttachment, XmATTACH_FORM ); n++;
        XtSetArg( args[n], XmNleftAttachment, XmATTACH_WIDGET ); n++;
        XtSetArg( args[n], XmNleftWidget, Label1 ); n++;
        XtSetArg( args[n], XmNwidth, SPIN_BOX_WIDTH ); n++;
        XtSetArg( args[n], XmNlabelString, string ); n++;
        XtSetArg( args[n], XmNalignment, XmALIGNMENT_CENTER ); n++;
        Label2 = XmCreateLabel( MainForm, "Label2", args, n );
        XtManageChild( Label2 );
        XmStringFree( string );

        n = 0;
        XtSetArg( args[n], XmNtopAttachment, XmATTACH_WIDGET ); n++;
        XtSetArg( args[n], XmNtopWidget, Label2 ); n++;
        XtSetArg( args[n], XmNleftAttachment, XmATTACH_WIDGET ); n++;
        XtSetArg( args[n], XmNleftWidget, SpinBox1 ); n++;
        XtSetArg( args[n], XmNwidth, SPIN_BOX_WIDTH ); n++;
        XtSetArg( args[n], XmNheight, SPIN_BOX_HEIGHT ); n++;
	XtSetArg( args[n], XmNarrowLayout, XmARROWS_FLAT_END ); n++;
	XtSetArg( args[n], XmNarrowOrientation, XmARROWS_HORIZONTAL ); n++;
        SpinBox2 = XmCreateSpinBox( MainForm, "SpinBox2", args, n );
        XtManageChild( SpinBox2 );

        n = 0;
        NumericTextF = XmCreateTextField( SpinBox2, "NumericTextF", args, n );
        XtManageChild( NumericTextF );

	CreateControlPanel();

        CommonPause();
        CommonPause();
        CommonPause();
        CommonPause();
        CommonPause();
        CommonPause();
        CommonPause();
        CommonPause();
        CommonPause();
        CommonPause();
        CommonPause();
        CommonPause();
        CommonPause();
        CommonPause();
        CommonPause();
        CommonPause();

	MotifConvergeTP3();
	MotifConvergeTP4();
	MotifConvergeTP5();
	MotifConvergeTP6();
	MotifConvergeTP7();

        XtAppMainLoop( app_context );
}
