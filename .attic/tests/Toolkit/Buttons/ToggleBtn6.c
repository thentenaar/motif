/* $TOG: ToggleBtn6.c /main/6 1999/03/01 18:18:03 jff $ */
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

#include "gnu.bmp"
#include "smiley.bmp"

/*
 *  Defines
 */
#define NUM_INDICATORS   6
#define NUM_TOGGLES      NUM_INDICATORS + 2 
#define NUM_PUSHBUTTONS  2
#define NUM_STATES       3
#define WIDGET_WIDTH     150 
#define TOGGLE_WIDTH     WIDGET_WIDTH 
#define TOGGLE_HEIGHT    50
#define PUSHB_WIDTH      WIDGET_WIDTH
#define PUSHB_HEIGHT     25
#define MGR_WIDTH        175 
#define MGR_HEIGHT       650

#define BM1_WIDTH  gnu_width
#define BM1_HEIGHT gnu_height
#define BM1_BITS   gnu_bits

#define BM2_WIDTH  smiley_width
#define BM2_HEIGHT smiley_height
#define BM2_BITS   smiley_bits

/*
 *  Declare strings.
 */
static char *toggle_labels[] = {
 "Indicator None",
 "Indicator Fill",
 "Indicator Check",
 "Indicator CheckBox",
 "Indicator Cross",
 "Indicator CrossBox",
 "Pixmap",
 "One of Many" };

static char *pushb_labels[] = {
 "Select All",
 "Deselect All" };

static char *states[] = {
 "Unset",
 "Set",
 "Indeterminate" };

/*
 *  Global variables.
 */
int      i, n, x_coord, y_coord, cols, rows;
char     name[25];
Arg      args[25];
Widget   Manager1, Indicators[NUM_TOGGLES], PushB[NUM_PUSHBUTTONS], ToggleB1,
         ToggleB2, Separator1, Label1, TextField1,
         PullDown, OptionMenu, OptionPB[NUM_STATES];
XmString Label;
Pixmap   pixmap1, pixmap2;
Pixel    SelectColor, UnselectColor;
int      CurrentState = XmUNSET;

/*
 *  Callbacks
 */
void SelectAll( Widget w, XtPointer client_data, XtPointer call_data )
{
     
     for ( i = 0; i < NUM_INDICATORS; i++ )
     {
        n = 0;
        XtSetArg( args[n], XmNset, XmSET ); n++;
        XtSetValues( Indicators[i], args, n );
     }
}

void DeselectAll( Widget w, XtPointer client_data, XtPointer call_data )
{
     for ( i = 0; i < NUM_INDICATORS; i++ )
     {
        n = 0;
        XtSetArg( args[n], XmNset, XmUNSET ); n++;
        XtSetValues( Indicators[i], args, n );
     }
}

void FillOnSelect( Widget w, XtPointer client_data, XtPointer call_data )
{
   Boolean  fill = (((XmToggleButtonCallbackStruct*)call_data)->set) ?
                   True : False;

     for ( i = 0; i < NUM_INDICATORS; i++ )
     {
        n = 0;
        XtSetArg( args[n], XmNfillOnSelect, fill ); n++;
        XtSetValues( Indicators[i], args, n );
     }
}

void ToggleMode( Widget w, XtPointer client_data, XtPointer call_data )
{
   Boolean  set;
   unsigned char  mode;
   XmString       Label;

     set = (((XmToggleButtonCallbackStruct*)call_data)->set) ?
              True : False;
     if ( set == True )
     {
        mode = XmTOGGLE_INDETERMINATE;
        Label = XmStringCreateLtoR("Toggle Indeterminate",
                        XmFONTLIST_DEFAULT_TAG );
     }
     else
     {
        mode = XmTOGGLE_BOOLEAN;
        Label = XmStringCreateLtoR("Toggle Boolean",
                        XmFONTLIST_DEFAULT_TAG );
     }
                         

   /*  Change the toggle mode for all appropriate toggle buttons. */
     for ( i = 0; i < NUM_TOGGLES; i++ )
     {
        n = 0;
        XtSetArg( args[n], XmNtoggleMode, mode ); n++;
        XtSetValues( Indicators[i], args, n );
     }

     n = 0;
     XtSetArg( args[n], XmNlabelString, Label ); n++;
     XtSetValues( w, args, n );
}

void SetNextState( Widget w, XtPointer client_data, XtPointer call_data )
{
   char *text;

      switch( CurrentState )
      {
        case XmUNSET:
             CurrentState = XmSET;
             text = states[1];
             break;

        case XmSET:
             CurrentState = XmINDETERMINATE;
             text = states[2];
             break;

        case XmINDETERMINATE:
             CurrentState = XmUNSET;
             text = states[0];
             break;
      }

      for ( i = 0; i < NUM_TOGGLES; i++ )
      {
        n = 0;
        XtSetArg( args[n], XmNset, CurrentState ); n++;
        XtSetValues( Indicators[i], args, n );
      }

        n = 0;
        XtSetArg( args[n], XmNvalue, text ); n++;
        XtSetValues( TextField1, args, n );
}

void SetState( Widget w, XtPointer client_data, XtPointer call_data )
{
   unsigned char state, value;
   Boolean       notify;

  /*  Get the state specified by the option menu. */
   if ( w == OptionPB[0] )
           state = XmUNSET;

   else if ( w == OptionPB[1] )
           state = XmSET;

   else if ( w == OptionPB[2] )
           state = XmINDETERMINATE;

  /* Determine whether valueChangedCallback should be called. */
   n = 0;
   XtSetArg( args[n], XmNset, &value ); n++;
   XtGetValues( ToggleB2, args, n ); 

   notify = ( value == XmSET ) ? True : False;

   for ( i = 0; i < NUM_TOGGLES; i++ )
   {
      XmToggleButtonSetValue( Indicators[i], state, notify ); 
   }
}

void ValueChanged( Widget w, XtPointer client_data, XtPointer call_data )
{
   char  *name;

      name = (char *)client_data;   
      printf("XmNvalueChangedCallback called for %s\n", name);
}
           
/*
 * Main
 */
void main ( int argc, char **argv )
{
     CommonTestInit( argc, argv );
 
     n = 0;
     XtSetArg( args[n], XmNwidth, MGR_WIDTH ); n++;
     XtSetArg( args[n], XmNheight, MGR_HEIGHT ); n++;
     Manager1 = XmCreateBulletinBoard( Shell1, "Manager1", args, n ); 
     XtManageChild( Manager1 );

    /*  Set select and unselect colors. */
     SelectColor = CommonGetColor("red");
     UnselectColor = CommonGetColor("gray");

     x_coord = y_coord = 0;

     for ( i = 0; i < NUM_INDICATORS; i++ )
     {
       sprintf( name, "Indicator%d", i );
       Label = XmStringCreateLtoR(toggle_labels[i], XmFONTLIST_DEFAULT_TAG );

       n = 0;
       XtSetArg( args[n], XmNx, x_coord ); n++;
       XtSetArg( args[n], XmNy, y_coord ); n++;
       XtSetArg( args[n], XmNwidth, TOGGLE_WIDTH ); n++;
       XtSetArg( args[n], XmNheight, TOGGLE_HEIGHT ); n++;
       XtSetArg( args[n], XmNlabelString, Label ); n++;
       XtSetArg( args[n], XmNselectColor, SelectColor); n++;
       XtSetArg( args[n], XmNunselectColor, UnselectColor); n++;
       XtSetArg( args[n], XmNfillOnSelect, False); n++;
       Indicators[i] = XmCreateToggleButton( Manager1, name, args, n ); 
       XtManageChild(Indicators[i]);
       XmStringFree(Label);

       XtAddCallback( Indicators[i], XmNvalueChangedCallback, 
                      ValueChanged, toggle_labels[i] );

       y_coord += TOGGLE_HEIGHT;
     }

     /*  Set indicatorOn type for each toggle button. */
       i = 0;
       XtSetArg( args[0], XmNindicatorOn, XmINDICATOR_NONE );
       XtSetValues( Indicators[i++], args, 1); 

       XtSetArg( args[0], XmNindicatorOn, XmINDICATOR_FILL );
       XtSetValues( Indicators[i++], args, 1); 
      
       XtSetArg( args[0], XmNindicatorOn, XmINDICATOR_CHECK );
       XtSetValues( Indicators[i++], args, 1); 
      
       XtSetArg( args[0], XmNindicatorOn, XmINDICATOR_CHECK_BOX );
       XtSetValues( Indicators[i++], args, 1); 
      
       XtSetArg( args[0], XmNindicatorOn, XmINDICATOR_CROSS );
       XtSetValues( Indicators[i++], args, 1); 
      
       XtSetArg( args[0], XmNindicatorOn, XmINDICATOR_CROSS_BOX );
       XtSetValues( Indicators[i++], args, 1); 
      
     pixmap1 = XCreatePixmapFromBitmapData(display, DefaultRootWindow(display),
                                 BM1_BITS, BM1_WIDTH, BM1_HEIGHT,
                                 WhitePixel(display, DefaultScreen(display)),
                                 BlackPixel(display, DefaultScreen(display)),
                                 DefaultDepth(display,DefaultScreen(display)));

     pixmap2 = XCreatePixmapFromBitmapData(display, DefaultRootWindow(display),
                                 BM2_BITS, BM2_WIDTH, BM2_HEIGHT,
                                 WhitePixel(display, DefaultScreen(display)),
                                 BlackPixel(display, DefaultScreen(display)),
                                 DefaultDepth(display,DefaultScreen(display)));

     sprintf( name, "Indicator%d", NUM_INDICATORS );

     n = 0;
     XtSetArg( args[n], XmNx, x_coord ); n++;
     XtSetArg( args[n], XmNy, y_coord ); n++;
     XtSetArg( args[n], XmNwidth, WIDGET_WIDTH ); n++;
     XtSetArg( args[n], XmNlabelType, XmPIXMAP ); n++;
     XtSetArg( args[n], XmNlabelPixmap, pixmap1 ); n++;
     XtSetArg( args[n], XmNindeterminatePixmap, pixmap2 ); n++;
     XtSetArg( args[n], XmNindeterminatePixmap, pixmap2 ); n++;
     XtSetArg( args[n], XmNselectColor, SelectColor ); n++;
     XtSetArg( args[n], XmNunselectColor, UnselectColor ); n++;
     Indicators[NUM_INDICATORS] = 
         XmCreateToggleButton( Manager1, name, args, n );
     XtAddCallback( Indicators[NUM_INDICATORS], XmNvalueChangedCallback, 
                    ValueChanged, toggle_labels[NUM_INDICATORS] );
 
     y_coord += ( 1.5 * TOGGLE_HEIGHT );

     Label = XmStringCreateLtoR("One Of Many", XmFONTLIST_DEFAULT_TAG );

     sprintf( name, "Indicator%d", NUM_INDICATORS + 1 );

     n = 0;
     XtSetArg( args[n], XmNx, x_coord ); n++;
     XtSetArg( args[n], XmNy, y_coord ); n++;
     XtSetArg( args[n], XmNwidth, WIDGET_WIDTH ); n++;
     XtSetArg( args[n], XmNlabelString, Label ); n++;
     XtSetArg( args[n], XmNindicatorType, XmONE_OF_MANY ); n++;
     XtSetArg( args[n], XmNselectColor, SelectColor ); n++;
     XtSetArg( args[n], XmNunselectColor, UnselectColor ); n++;
     Indicators[NUM_INDICATORS + 1] = 
         XmCreateToggleButton( Manager1, name, args, n );
     XtAddCallback( Indicators[NUM_INDICATORS + 1], XmNvalueChangedCallback, 
                    ValueChanged, toggle_labels[NUM_INDICATORS + 1] );

     XmStringFree(Label);

     y_coord += TOGGLE_HEIGHT;

     n = 0;
     XtSetArg( args[n], XmNx, x_coord ); n++;
     XtSetArg( args[n], XmNy, y_coord ); n++;
     XtSetArg( args[n], XmNwidth, WIDGET_WIDTH ); n++;
     XtSetArg( args[n], XmNorientation, XmHORIZONTAL ); n++;
     XtSetArg( args[n], XmNseparatorType, XmSHADOW_ETCHED_OUT ); n++;
     Separator1 = XmCreateSeparator( Manager1, "Separator1", args, n );
     XtManageChild(Separator1);

     y_coord += PUSHB_HEIGHT;
     
     for ( i = 0; i < NUM_PUSHBUTTONS; i++ )
     {
        sprintf( name, "PushB%d", i );
        Label = XmStringCreateLtoR(pushb_labels[i], XmFONTLIST_DEFAULT_TAG );

        n = 0;
        XtSetArg( args[n], XmNx, x_coord ); n++;
        XtSetArg( args[n], XmNy, y_coord ); n++;
        XtSetArg( args[n], XmNwidth, PUSHB_WIDTH ); n++;
        XtSetArg( args[n], XmNheight, PUSHB_HEIGHT ); n++;
        XtSetArg( args[n], XmNlabelString, Label ); n++;
        PushB[i] = XmCreatePushButton( Manager1, name, args, n ); 
        XtManageChild(PushB[i]);

        XmStringFree(Label);

        y_coord += PUSHB_HEIGHT;
     }

     y_coord += TOGGLE_HEIGHT;

     XtAddCallback( PushB[0], XmNactivateCallback, SelectAll, NULL );
     XtAddCallback( PushB[1], XmNactivateCallback, DeselectAll, NULL );

     Label = XmStringCreateLtoR("Fill On Select", XmFONTLIST_DEFAULT_TAG );

     n = 0;
     XtSetArg( args[n], XmNx, x_coord ); n++;
     XtSetArg( args[n], XmNy, y_coord ); n++;
     XtSetArg( args[n], XmNwidth, TOGGLE_WIDTH ); n++;
     XtSetArg( args[n], XmNheight, TOGGLE_HEIGHT ); n++;
     XtSetArg( args[n], XmNlabelString, Label ); n++;
     ToggleB1 = XmCreateToggleButton( Manager1, "ToggleB1", args, n );
     XtManageChild( ToggleB1 ); 

     XtAddCallback( ToggleB1, XmNvalueChangedCallback, FillOnSelect, NULL );
     XmStringFree(Label);
     
     XtRealizeWidget(Shell1);       

     CommonPause();
     CommonPause();

     XtManageChild( Indicators[NUM_INDICATORS] );
     XtManageChild( Indicators[NUM_INDICATORS + 1] );

    /*  Change the first push button for setting the next toggle state. */
     XtRemoveCallback( PushB[0], XmNactivateCallback, SelectAll, NULL ); 
     
     Label = XmStringCreateLtoR("Set Next State", XmFONTLIST_DEFAULT_TAG );

     n = 0;
     XtSetArg( args[n], XmNlabelString, Label ); n++;
     XtSetValues( PushB[0], args, n ); 
     XtAddCallback( PushB[0], XmNactivateCallback, SetNextState, TextField1 ); 

     XmStringFree(Label);

    /*  Unmange PushB[1] */
     XtUnmanageChild( PushB[1] );

     y_coord -= TOGGLE_HEIGHT;
     
     Label = XmStringCreateLtoR("State:", XmFONTLIST_DEFAULT_TAG );

     n = 0;
     XtSetArg( args[n], XmNx, x_coord ); n++;
     XtSetArg( args[n], XmNy, y_coord ); n++;
     XtSetArg( args[n], XmNwidth, WIDGET_WIDTH / 3 ); n++;
     XtSetArg( args[n], XmNheight, TOGGLE_HEIGHT ); n++;
     XtSetArg( args[n], XmNlabelString, Label ); n++;
     Label1 = XmCreateLabel( Manager1, "Label1", args, n );
     XtManageChild(Label1);

     XmStringFree(Label);

     x_coord += ( WIDGET_WIDTH / 2 );
     XtSetArg( args[n], XmNx, x_coord ); n++;
     XtSetArg( args[n], XmNy, y_coord ); n++;
     XtSetArg( args[n], XmNwidth, WIDGET_WIDTH * 2/3 ); n++;
     XtSetArg( args[n], XmNheight, TOGGLE_HEIGHT ); n++;
     XtSetArg( args[n], XmNvalue, "Unset" ); n++;
     TextField1 = XmCreateTextField( Manager1, "TextField1", args, n );
     XtManageChild(TextField1);

     x_coord = 0;

    /*  Change ToggleB1 for setting the toggle mode. */
     XtRemoveCallback( ToggleB1, XmNvalueChangedCallback, FillOnSelect, NULL ); 

     Label = XmStringCreateLtoR("Toggle Boolean", XmFONTLIST_DEFAULT_TAG );
     n = 0;
     XtSetArg( args[n], XmNlabelString, Label ); n++;
     XtSetArg( args[n], XmNset, XmUNSET ); n++;
     XtSetValues( ToggleB1, args, n ); 

     XtAddCallback( ToggleB1, XmNvalueChangedCallback, ToggleMode, NULL ); 
     XmStringFree(Label);

     CommonPause();
     CommonPause();

     y_coord -= ( 2 * PUSHB_HEIGHT );

     XtUnmanageChild( PushB[0] ); 
     XtUnmanageChild( Label1 ); 
     XtUnmanageChild( TextField1 ); 
      
     n = 0;
     XtSetArg( args[n], XmNx, x_coord ); n++;
     XtSetArg( args[n], XmNy, y_coord ); n++;
     XtSetArg( args[n], XmNwidth, WIDGET_WIDTH ); n++;
     XtSetArg( args[n], XmNheight, PUSHB_HEIGHT ); n++;
     PullDown = XmCreatePulldownMenu( Manager1, "PullDown", args, n );

     for ( i = 0; i < NUM_STATES; i++ )
     {
       sprintf( name, "OptionPB%d", i );

       Label = XmStringCreateLtoR( states[i], XmFONTLIST_DEFAULT_TAG );

       n = 0;
       XtSetArg( args[n], XmNlabelString, Label ); n++;
       OptionPB[i] = XmCreatePushButton( PullDown, name, args, n );  

       XtAddCallback( OptionPB[i], XmNactivateCallback, SetState, NULL ); 
       XmStringFree( Label );
     }

     XtManageChildren( OptionPB, NUM_STATES );
     
     Label = XmStringCreateLtoR("State", XmFONTLIST_DEFAULT_TAG );

     n = 0;
     XtSetArg( args[n], XmNx, x_coord ); n++;
     XtSetArg( args[n], XmNy, y_coord ); n++;
     XtSetArg( args[n], XmNwidth, WIDGET_WIDTH ); n++;
     XtSetArg( args[n], XmNheight, PUSHB_HEIGHT ); n++;
     XtSetArg( args[n], XmNsubMenuId, PullDown); n++;
     XtSetArg( args[n], XmNmenuHistory, OptionPB[0]); n++;
     XtSetArg( args[n], XmNlabelString, Label ); n++;
     OptionMenu = XmCreateOptionMenu( Manager1, "OptionMenu", args, n );
     XtManageChild(OptionMenu);

     XmStringFree(Label);

     y_coord += ( 2 * PUSHB_HEIGHT );

     Label = XmStringCreateLtoR("valueChangedCallback", 
               XmFONTLIST_DEFAULT_TAG );

     n = 0;
     XtSetArg( args[n], XmNx, x_coord ); n++;
     XtSetArg( args[n], XmNy, y_coord ); n++;
     XtSetArg( args[n], XmNwidth, WIDGET_WIDTH ); n++;
     XtSetArg( args[n], XmNheight, TOGGLE_HEIGHT ); n++;
     XtSetArg( args[n], XmNlabelString, Label ); n++;
     ToggleB2 = XmCreateToggleButton( Manager1, "ToggleB2", args, n );
     XtManageChild( ToggleB2 );


     XmStringFree(Label);

     CommonPause();
     CommonPause();
     CommonPause();

     CommonPause();

     XtAppMainLoop( app_context );
 
}
      
