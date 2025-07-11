/* $TOG: Visual1.c /main/8 1999/07/02 11:21:39 vipin $ */
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
#include <Xm/XmP.h>
#include "collapsedState.bmp"
#include "expandedState.bmp"

#define MGR_WIDTH   600
#define MGR_HEIGHT  500
#define WIDGET_WIDTH  (MGR_WIDTH/4)
#define WIDGET_HEIGHT (MGR_HEIGHT/3)
#define POPUP_WIDTH 200
#define POPUP_HEIGHT 150
#define MARGINS     5
#define OFFSET      5
#define ORIGIN      0 + MARGINS
#define NUM_WIDGETS 15
#define NUM_COL     4
#define LIST_COUNT  6
#define NUM_PAGE    6
#define NUM_MAJOR_TAB 2
#define NUM_MINOR_TAB 3


Arg           args[25];
int           n, i, primitiveCount, managerCount,
              x_coord, y_coord, y1_coord, delta;
Widget        Manager, primitiveWidgets[NUM_WIDGETS],
              managerWidgets[NUM_WIDGETS], FileSelectB, ToggleB1, 
              ToggleB2, PullDown, pushb[LIST_COUNT], SpinBtn, TextF1,
              Page[NUM_PAGE], PageScroller, StatusArea, Notebook,
              OptionMenu,
              MajorTab[NUM_MAJOR_TAB], MinorTab[NUM_MINOR_TAB],
              PopupShell, Form1;
XmString      string, ListItems[LIST_COUNT];
XmStringTable ColumnHeadings;
Pixmap        CollapsedStatePixmap, ExpandedStatePixmap;
XmFontList    font_list;
Pixmap        pixmap1;
Pixel         foreground, background;

char *ColumnHeadingText[NUM_COL] = {
    "Icon",
    "Full Title",
    "Favorite Flavor",
    "Age"
};

char *Items[LIST_COUNT] = {
      "Grass", "Trees", "Leaves", "Dew", "Mountains", "Sunrise" };

char largeIcon[] = "../bitmaps/LargeIcon";
char smallIcon[] = "../bitmaps/SmallIcon";

void SetWidgetVisuals( Widget w, XtPointer client_data, XtPointer call_data )
{
   XmString string;
   int      j;

   /*
    *  Update the sensitive toggle button.
    *  Set XmNsensitive resource where appropriate.
    *  Call XtSetSensitive() with new value for composite widgets.
    */
     if (((XmToggleButtonCallbackStruct*)call_data)->set)
     {
        string = XmStringCreateLtoR("Insensitive Children",
                                    XmSTRING_DEFAULT_CHARSET );
        XtSetArg(args[0], XmNsensitive, False );
        for ( j = 0; j < primitiveCount; j++ )
          if ( primitiveWidgets[j] )
           XtSetValues( primitiveWidgets[j], args, 1 );
        for ( j = 0; j < managerCount; j++ )
          if ( managerWidgets[j] )
           XtSetSensitive( managerWidgets[j], False );
     }
     
     else
     {
        string = XmStringCreateLtoR("Sensitive Children",
                                    XmSTRING_DEFAULT_CHARSET );
        XtSetArg(args[0], XmNsensitive, True );
        for ( j = 0; j < primitiveCount; j++ )
          if ( primitiveWidgets[j] )
           XtSetValues( primitiveWidgets[j], args, 1 );
        for ( j = 0; j < managerCount; j++ )
          if ( managerWidgets[j] )
           XtSetSensitive( managerWidgets[j], True );
     }
    
     n = 0;
     XtSetArg(args[n], XmNlabelString, string); n++;
     XtSetValues(w, args, n);

     XmStringFree( string );
}


void SetParentVisuals( Widget w, XtPointer client_data, XtPointer call_data )
{
   XmString string;
   Widget   parent = (Widget)client_data;

   /*
    *  Update the sensitive toggle button.
    */
     if (((XmToggleButtonCallbackStruct*)call_data)->set)
     {
        string = XmStringCreateLtoR("Insensitive Parent",
                                    XmSTRING_DEFAULT_CHARSET );
        XtSetSensitive( parent, False );
     }
     else
     {
        string = XmStringCreateLtoR("Sensitive Parent",
                                    XmSTRING_DEFAULT_CHARSET );
        XtSetSensitive( parent, True );
     }

     n = 0;
     XtSetArg( args[n], XmNlabelString, string ); n++;
     XtSetValues( w, args, n );

}


void main( argc, argv )
int	argc;
char    **argv;
{
   char   name[10];

    CommonTestInit( argc, argv );

    /*
     *  Create Manager widget. 
     */
     n = 0;
     XtSetArg( args[n], XmNwidth, MGR_WIDTH ); n++;
     XtSetArg( args[n], XmNheight, MGR_HEIGHT ); n++;
     XtSetArg( args[n], XmNmarginWidth, MARGINS ); n++;
     XtSetArg( args[n], XmNmarginHeight, MARGINS ); n++;
     Manager = XmCreateBulletinBoard( Shell1, "Manager", args, n ); 
     XtManageChild( Manager );

     n = 0;
     XtSetArg (args[n], XmNforeground, &foreground);       n++;
     XtSetArg (args[n], XmNbackground, &background);       n++;
     XtGetValues (Manager, args, n);


     for ( i = 0; i < LIST_COUNT; i++ )
        ListItems[i] = XmStringCreateLtoR( Items[i], XmSTRING_DEFAULT_CHARSET );

    /*
     *  Create Manager Children
     */
    
     primitiveCount = 0;
     managerCount = 0;
     x_coord = y_coord = ORIGIN; 

    /*  Arrow Button */
     n = 0;
     XtSetArg( args[n], XmNx, x_coord ); n++;  
     XtSetArg( args[n], XmNy, y_coord ); n++;  
     XtSetArg( args[n], XmNwidth, WIDGET_WIDTH ); n++;
     XtSetArg( args[n], XmNheight, WIDGET_HEIGHT / 3 ); n++;
     primitiveWidgets[primitiveCount++] =
         XmCreateArrowButton( Manager, "ArrowBtn", args, n );

     y_coord += WIDGET_HEIGHT / 3;


     /*  Drawn Button */
     screen = XtScreen (Shell1);
     pixmap1 = XmGetPixmap (screen, "tie_fighter", foreground, background);

     n = 0;
     XtSetArg( args[n], XmNx, x_coord ); n++;  
     XtSetArg( args[n], XmNy, y_coord ); n++;  
     XtSetArg( args[n], XmNwidth, WIDGET_WIDTH ); n++;
     XtSetArg( args[n], XmNheight, WIDGET_HEIGHT / 3 ); n++;
     XtSetArg( args[n], XmNlabelType, XmPIXMAP ); n++;
     XtSetArg( args[n], XmNlabelPixmap, pixmap1 ); n++;
     XtSetArg( args[n], XmNrecomputeSize, FALSE ); n++;
     primitiveWidgets[primitiveCount++] =
         XmCreateDrawnButton( Manager, "DrawnBtn", args, n );

     y_coord += WIDGET_HEIGHT / 3;

    /* PushButton */
     string = XmStringCreateLtoR("Push Button", XmSTRING_DEFAULT_CHARSET );

     n = 0;
     XtSetArg( args[n], XmNx, x_coord ); n++;  
     XtSetArg( args[n], XmNy, y_coord ); n++;  
     XtSetArg( args[n], XmNwidth, WIDGET_WIDTH - (delta + MARGINS)); n++;
     XtSetArg( args[n], XmNheight, WIDGET_HEIGHT / 3 ); n++;
     XtSetArg( args[n], XmNlabelString, string ); n++;
     primitiveWidgets[primitiveCount++] =
         XmCreatePushButton(Manager, "PushButton", args, n);

     XmStringFree(string);

     x_coord += WIDGET_WIDTH;
     y_coord = ORIGIN;

    /* Text */
     n = 0;
     XtSetArg( args[n], XmNx, x_coord ); n++;  
     XtSetArg( args[n], XmNy, y_coord ); n++;  
     XtSetArg( args[n], XmNwidth, WIDGET_WIDTH ); n++;
     XtSetArg( args[n], XmNheight, WIDGET_HEIGHT / 3 ); n++;
     XtSetArg( args[n], XmNvalue, "An XmText widget." ); n++;
     XtSetArg( args[n], XmNeditable, False ); n++;
     primitiveWidgets[primitiveCount++] =
         XmCreateText( Manager, "Text", args, n );

     y_coord += WIDGET_HEIGHT / 3;

    /* TextField */
     n = 0;
     XtSetArg( args[n], XmNx, x_coord ); n++;  
     XtSetArg( args[n], XmNy, y_coord ); n++;  
     XtSetArg( args[n], XmNwidth, WIDGET_WIDTH ); n++;
     XtSetArg( args[n], XmNheight, WIDGET_HEIGHT / 3 ); n++;
     XtSetArg( args[n], XmNvalue, "An XmTextField widget." ); n++;
     XtSetArg( args[n], XmNeditable, False ); n++;
     primitiveWidgets[primitiveCount++] =
         XmCreateTextField( Manager, "TextField", args, n );

     x_coord += WIDGET_WIDTH;
     y_coord = ORIGIN;

    /* Scale */
     n = 0;
     XtSetArg( args[n], XmNx, x_coord ); n++;  
     XtSetArg( args[n], XmNy, y_coord ); n++;  
     XtSetArg( args[n], XmNwidth, WIDGET_WIDTH ); n++;
     XtSetArg( args[n], XmNheight, WIDGET_HEIGHT / 5 ); n++;
     XtSetArg( args[n], XmNmaximum, 100 ); n++;
     XtSetArg( args[n], XmNorientation, XmHORIZONTAL ); n++;
     primitiveWidgets[primitiveCount++] =
         XmCreateScale(Manager, "Scale", args, n);
     
     y_coord += WIDGET_HEIGHT / 3; 

    /* ScrollBar */
     n = 0;
     XtSetArg( args[n], XmNx, x_coord ); n++;  
     XtSetArg( args[n], XmNy, y_coord ); n++;  
     XtSetArg( args[n], XmNwidth, WIDGET_WIDTH ); n++;
     XtSetArg( args[n], XmNheight, WIDGET_HEIGHT / 5 ); n++;
     XtSetArg( args[n], XmNmaximum, 100 ); n++;
     XtSetArg( args[n], XmNorientation, XmHORIZONTAL ); n++;
     primitiveWidgets[primitiveCount++] =
         XmCreateScrollBar(Manager, "ScrollBar", args, n);

     /* ToggleButton */
     y_coord += WIDGET_HEIGHT / 3; 
     string = XmStringCreateLtoR("Toggle Away", XmSTRING_DEFAULT_CHARSET );

     n = 0;
     XtSetArg( args[n], XmNx, x_coord ); n++;  
     XtSetArg( args[n], XmNy, y_coord ); n++;  
     XtSetArg( args[n], XmNwidth, WIDGET_WIDTH); n++;
     XtSetArg( args[n], XmNheight, WIDGET_HEIGHT / 3 ); n++;
     XtSetArg( args[n], XmNlabelString, string ); n++;
     primitiveWidgets[primitiveCount++] =
         XmCreateToggleButton(Manager, "ToggleB", args, n);

     x_coord += WIDGET_WIDTH;
     y_coord = ORIGIN;

    /* Combo Box */
     font_list = XmeGetDefaultRenderTable(primitiveWidgets[primitiveCount - 1],
					  XmTEXT_FONTLIST );
     n = 0;
     XtSetArg( args[n], XmNx, x_coord ); n++;  
     XtSetArg( args[n], XmNy, y_coord ); n++;  
     XtSetArg( args[n], XmNwidth, WIDGET_WIDTH ); n++;
     XtSetArg( args[n], XmNheight, WIDGET_HEIGHT ); n++;
     XtSetArg( args[n], XmNcomboBoxType, XmCOMBO_BOX ); n++;
     XtSetArg( args[n], XmNfontList, font_list ); n++;
     XtSetArg( args[n], XmNmatchBehavior, XmNONE ); n++;
     XtSetArg( args[n], XmNstringDirection, XmSTRING_DIRECTION_L_TO_R ); n++;
     XtSetArg( args[n], XmNhighlightThickness, 10 ); n++;
     XtSetArg( args[n], XmNarrowSize, 15 ); n++;
     XtSetArg( args[n], XmNarrowSpacing, 10 ); n++;
     XtSetArg( args[n], XmNmarginWidth, 12 ); n++;
     XtSetArg( args[n], XmNmarginHeight, 12 ); n++;
     XtSetArg( args[n], XmNselectedPosition, 2 ); n++;
     XtSetArg( args[n], XmNitems, ListItems ); n++;
     XtSetArg( args[n], XmNitemCount, LIST_COUNT ); n++;
     XtSetArg( args[n], XmNvisibleItemCount, LIST_COUNT ); n++;
     XtSetArg( args[n], XmNcolumns, 10 ); n++;
     XtSetArg( args[n], XmNeditMode, XmSINGLE_LINE_EDIT ); n++;
/*     primitiveWidgets[primitiveCount++] =
         XmCreateComboBox( Manager, "ComboBox", args, n );*/
     managerWidgets[managerCount++] =
         XmCreateComboBox( Manager, "ComboBox", args, n );

     x_coord = ORIGIN;
     y_coord = ORIGIN + WIDGET_HEIGHT + OFFSET;

    /* File Selection Box */
     n = 0;
     XtSetArg( args[n], XmNx, x_coord ); n++;  
     XtSetArg( args[n], XmNy, y_coord ); n++;  
     XtSetArg( args[n], XmNwidth, WIDGET_WIDTH ); n++;
     XtSetArg( args[n], XmNheight, WIDGET_HEIGHT ); n++;
     XtSetArg( args[n], XmNfileListItems, ListItems ); n++;
     XtSetArg( args[n], XmNfileListItemCount, LIST_COUNT ); n++;
     XtSetArg( args[n], XmNlistVisibleItemCount, LIST_COUNT ); n++;
     XtSetArg( args[n], XmNresizePolicy, XmRESIZE_NONE ); n++;
     managerWidgets[managerCount++] =
         XmCreateFileSelectionBox(Manager, "FileSB", args, n);

     x_coord += WIDGET_WIDTH;
     y_coord += WIDGET_HEIGHT / 3;

    /* List */
     n = 0;
     XtSetArg( args[n], XmNx, x_coord ); n++;  
     XtSetArg( args[n], XmNy, y_coord ); n++;  
     XtSetArg( args[n], XmNwidth, WIDGET_WIDTH ); n++;
     XtSetArg( args[n], XmNheight, WIDGET_HEIGHT / 2 ); n++;
     XtSetArg( args[n], XmNitems, ListItems ); n++;
     XtSetArg( args[n], XmNitemCount, LIST_COUNT ); n++;
     XtSetArg( args[n], XmNvisibleItemCount, LIST_COUNT ); n++;
     primitiveWidgets[primitiveCount++] =
         XmCreateList(Manager, "List", args, n);

     y_coord += WIDGET_HEIGHT / 2 + OFFSET;

    /* Menu */
     PullDown = XmCreatePulldownMenu( Manager, "Pulldown", NULL, 0 ); 

     
/*
 *  The following causes a crash when creating second pushbutton.
 *  Go figure ?!
 */
/**
     for ( i = 0; i < LIST_COUNT; i++ )
     {
        sprintf( name, "PushB%d", i );
        XtSetArg( args[0], XmNlabelString, ListItems[i] );
        pushb[i] = XmCreatePushButton( PullDown, name, args, 1 );
     }
**/
     XtSetArg( args[0], XmNlabelString, ListItems[0] );
     pushb[0] = XmCreatePushButton( PullDown, "PushB2", args, 1 );
     XtSetArg( args[0], XmNlabelString, ListItems[1] );
     pushb[1] = XmCreatePushButton( PullDown, "PushB2", args, 1 );
     XtSetArg( args[0], XmNlabelString, ListItems[2] );
     pushb[2] = XmCreatePushButton( PullDown, "PushB3", args, 1 );
     XtSetArg( args[0], XmNlabelString, ListItems[3] );
     pushb[3] = XmCreatePushButton( PullDown, "PushB4", args, 1 );
     XtSetArg( args[0], XmNlabelString, ListItems[4] );
     pushb[4] = XmCreatePushButton( PullDown, "PushB5", args, 1 );
     XtSetArg( args[0], XmNlabelString, ListItems[5] );
     pushb[5] = XmCreatePushButton( PullDown, "PushB6", args, 1 );

     XtManageChildren( pushb, LIST_COUNT );
    
     string = XmStringCreateLtoR( "Menu", XmSTRING_DEFAULT_CHARSET );

     n = 0;
     XtSetArg( args[n], XmNx, x_coord ); n++;
     XtSetArg( args[n], XmNy, y_coord ); n++;
     XtSetArg( args[n], XmNwidth, WIDGET_WIDTH ); n++;
     XtSetArg( args[n], XmNheight, WIDGET_HEIGHT / 2 ); n++;
     XtSetArg(args[n], XmNsubMenuId, PullDown); n++;
     XtSetArg(args[n], XmNmenuHistory, pushb[0]); n++;
     XtSetArg(args[n], XmNlabelString, string ); n++;
     managerWidgets[managerCount++] =
         XmCreateOptionMenu(Manager, "Menu", args, n);

     XmStringFree( string );
     
     x_coord += WIDGET_WIDTH;
     y_coord = ORIGIN + WIDGET_HEIGHT + WIDGET_HEIGHT / 3; 

    /* Spin Button */
     n = 0;
     XtSetArg( args[n], XmNx, x_coord ); n++;
     XtSetArg( args[n], XmNy, y_coord ); n++;
     XtSetArg( args[n], XmNwidth, MGR_WIDTH / 3 - MARGINS ); n++;
     XtSetArg( args[n], XmNheight, WIDGET_HEIGHT / 4 ); n++;
     XtSetArg( args[n], XmNitems, ListItems); n++;
     SpinBtn = managerWidgets[managerCount++] =
         XmCreateSpinBox(Manager, "SpinBox", args, n);

     n=0;
     XtSetArg( args[n], XmNnumValues, LIST_COUNT); n++;
     XtSetArg( args[n], XmNvalues, ListItems); n++;
     TextF1 = XmCreateTextField (SpinBtn, "TextF1", args, n);
     XtManageChild (TextF1);


     y_coord += WIDGET_HEIGHT / 3;

    /* Container */
/**
     CollapsedStatePixmap = 
            XCreatePixmapFromBitmapData(display, rootWindow,
	       	  (char *)collapsedState_bits,
                  collapsedState_width, collapsedState_height,
	          CommonGetColor("black"),
		  CommonGetColor("white"),
		  XDefaultDepth(display, XDefaultScreen(display)));

     ExpandedStatePixmap = XCreatePixmapFromBitmapData(display, rootWindow,
		  (char *)expandedState_bits,
                  expandedState_width, expandedState_height,
        	  CommonGetColor("white"),
		  CommonGetColor("black"),
		  XDefaultDepth(display, XDefaultScreen(display)));

     ColumnHeadings = (XmStringTable) XtMalloc(NUM_COL * sizeof(XmString));

     for (i = 0; i < NUM_COL; i++)
       ColumnHeadings[i] = XmStringCreateLtoR(ColumnHeadingText[i],
                                              XmSTRING_DEFAULT_CHARSET);

     n = 0;
     XtSetArg( args[n], XmNx, x_coord ); n++;  
     XtSetArg( args[n], XmNy, y_coord ); n++;  
     XtSetArg( args[n], XmNwidth, WIDGET_WIDTH ); n++;
     XtSetArg( args[n], XmNheight, WIDGET_HEIGHT ); n++;
     XtSetArg( args[n], XmNlargeIcon, largeIcon ); n++;
     XtSetArg( args[n], XmNsmallIcon, smallIcon ); n++;
     XtSetArg( args[n], XmNhighlightColor, CommonGetColor("white")); n++;
     XtSetArg( args[n], XmNentryViewType, XmDETAIL ); n++;
     XtSetArg( args[n], XmNdetailCount, 4 ); n++;
     XtSetArg(args[n], XmNdetailColumnHeading, ColumnHeadings); n++;
     XtSetArg(args[n], XmNcollapsedStatePixmap, CollapsedStatePixmap); n++;
     XtSetArg(args[n], XmNexpandedStatePixmap, ExpandedStatePixmap); n++;
     primitiveWidgets[primitiveCount++] =
         XmCreateContainer(Manager, "Container", args, n);

        for (i = 0; i < NUM_COL; i++)
                XmStringFree(ColumnHeadings[i]);
        XtFree((XtPointer)ColumnHeadings);
*/
     

    /* Notebook */
     XtSetArg( args[n], XmNx, x_coord ); n++;  
     XtSetArg( args[n], XmNy, y_coord ); n++;  
     XtSetArg( args[n], XmNwidth, MGR_WIDTH / 3 - MARGINS ); n++;
     XtSetArg( args[n], XmNheight, WIDGET_HEIGHT ); n++;
     XtSetArg( args[n], XmNcurrentPageNumber, 1 ); n++;
     XtSetArg( args[n], XmNlastPageNumber, NUM_PAGE ); n++;
     XtSetArg( args[n], XmNorientation, XmHORIZONTAL ); n++;
     XtSetArg( args[n], XmNbackPageNumber, 12 ); n++;
     XtSetArg( args[n], XmNbackPageSize, 15 ); n++;
     XtSetArg( args[n], XmNbackPageForeground,
                        CommonGetColor("red")); n++;
     XtSetArg( args[n], XmNbackPageBackground, 
                        CommonGetColor("green")); n++;
     XtSetArg( args[n], XmNframeBackground,
                        CommonGetColor("red")); n++;
     XtSetArg( args[n], XmNbindingWidth, 5 ); n++;
     XtSetArg( args[n], XmNmajorTabSpacing, 7 ); n++;
     XtSetArg( args[n], XmNminorTabSpacing, 5 ); n++;
     Notebook = managerWidgets[managerCount++] =
         XmCreateNotebook(Manager, "Notebook", args, n);

        for (i=0; i < NUM_PAGE; i++)
          {
            sprintf (name, "Page%d", i+1);
            Page[i] = XmCreateForm (Notebook,name,NULL,0);
          }

        for (i=0; i< NUM_MAJOR_TAB; i++)
          {
            sprintf (name, "MajorTab%d", i+1);
            MajorTab[i] = XmCreatePushButton (Notebook,name,NULL,0);
          }


        for (i=0; i< NUM_MINOR_TAB; i++)
          {
            sprintf (name, "MinorTab%d", i+1);
            MinorTab[i] = XmCreatePushButton (Notebook,name,NULL,0);
          }

        PageScroller = XmCreateArrowButton (Notebook, "PageScroller", NULL, 0);

        StatusArea = XmCreateLabel (Notebook, "StatusArea", NULL, 0);
     


     XtManageChildren (Page,NUM_PAGE);
     XtManageChild (PageScroller);
     XtManageChild (StatusArea);
     XtManageChildren (MinorTab,NUM_MINOR_TAB);
     XtManageChildren (MajorTab, NUM_MAJOR_TAB);

    /*  Create toggle for the sensitive visuals. */
     string = XmStringCreateLtoR( "Sensitive Visuals", 
                                  XmSTRING_DEFAULT_CHARSET );
     n = 0;
     XtSetArg( args[n], XmNx, MGR_WIDTH - WIDGET_WIDTH ); n++;  
     XtSetArg( args[n], XmNy, MGR_HEIGHT - WIDGET_HEIGHT / 2 ); n++;  
     
    /* Container */
     CollapsedStatePixmap = 
            XCreatePixmapFromBitmapData(display, rootWindow,
	       	  (char *)collapsedState_bits,
                  collapsedState_width, collapsedState_height,
	          CommonGetColor("black"),
		  CommonGetColor("white"),
		  XDefaultDepth(display, XDefaultScreen(display)));

     ExpandedStatePixmap = XCreatePixmapFromBitmapData(display, rootWindow,
		  (char *)expandedState_bits,
                  expandedState_width, expandedState_height,
        	  CommonGetColor("white"),
		  CommonGetColor("black"),
		  XDefaultDepth(display, XDefaultScreen(display)));

     ColumnHeadings = (XmStringTable) XtMalloc(NUM_COL * sizeof(XmString));

     for (i = 0; i < NUM_COL; i++)
       ColumnHeadings[i] = XmStringCreateLtoR(ColumnHeadingText[i],
                                              XmSTRING_DEFAULT_CHARSET);

     n = 0;
     XtSetArg( args[n], XmNx, x_coord ); n++;  
     XtSetArg( args[n], XmNy, y_coord ); n++;  
     XtSetArg( args[n], XmNwidth, WIDGET_WIDTH ); n++;
     XtSetArg( args[n], XmNheight, WIDGET_HEIGHT ); n++;
     XtSetArg( args[n], XmNlargeIcon, largeIcon ); n++;
     XtSetArg( args[n], XmNsmallIcon, smallIcon ); n++;
     XtSetArg( args[n], XmNhighlightColor, CommonGetColor("white")); n++;
     XtSetArg( args[n], XmNentryViewType, XmDETAIL ); n++;
     XtSetArg( args[n], XmNdetailCount, 4 ); n++;
     XtSetArg(args[n], XmNdetailColumnHeading, ColumnHeadings); n++;
     XtSetArg(args[n], XmNcollapsedStatePixmap, CollapsedStatePixmap); n++;
     XtSetArg(args[n], XmNexpandedStatePixmap, ExpandedStatePixmap); n++;
/*
     primitiveWidgets[primitiveCount++] =
         XmCreateContainer(Manager, "Container", args, n);
*/

        for (i = 0; i < NUM_COL; i++)
                XmStringFree(ColumnHeadings[i]);
        XtFree((XtPointer)ColumnHeadings);
     
     XtManageChildren( primitiveWidgets, primitiveCount );
     XtManageChildren( managerWidgets, managerCount );

  /* 
   *  Create Popup Shell to contain toggle buttons for setting
   *  the sensitivity of the child and parent widgets.
   */
       n = 0;
       XtSetArg( args[n], XmNx, 200 ); n++;
       XtSetArg( args[n], XmNy, 550 ); n++;
       XtSetArg( args[n], XmNwidth, POPUP_WIDTH ); n++;
       XtSetArg( args[n], XmNheight, POPUP_HEIGHT ); n++;
       PopupShell = XtCreatePopupShell("Sensitivity Toggles",
                                       topLevelShellWidgetClass,
                                       Shell1, args, n );
       XtManageChild(PopupShell);

       n = 0;
       XtSetArg( args[n], XmNwidth, POPUP_WIDTH ); n++;
       XtSetArg( args[n], XmNheight, POPUP_HEIGHT ); n++;
       XtSetArg( args[n], XmNresizable, False ); n++;
       XtSetArg( args[n], XmNresizePolicy, XmRESIZE_NONE ); n++;
       Form1 = XmCreateBulletinBoard( PopupShell, "Form1", args, n );
       XtManageChild(Form1);

  /*  Create toggle for setting the sensitivity visuals of widgets. */
     
     string = XmStringCreateLtoR( "Sensitive Children", 
                                  XmSTRING_DEFAULT_CHARSET );
     n = 0;
     XtSetArg( args[n], XmNx, 0 ); n++;  
     XtSetArg( args[n], XmNy, 0 ); n++;  
     XtSetArg( args[n], XmNheight, POPUP_HEIGHT / 2 ); n++;
     XtSetArg( args[n], XmNwidth, POPUP_WIDTH - 2 * MARGINS ); n++;
     XtSetArg( args[n], XmNtopAttachment, XmATTACH_FORM ); n++;
     XtSetArg( args[n], XmNleftAttachment, XmATTACH_FORM ); n++;
     XtSetArg( args[n], XmNlabelString, string ); n++;
     ToggleB1 = XmCreateToggleButton(Form1, "VisualTB1", args, n );
     XtManageChild(ToggleB1);

     XmStringFree(string);

     XtAddCallback( ToggleB1, XmNvalueChangedCallback, 
                    SetWidgetVisuals, (XtPointer)0 );
                              
    /*  Create toggle for setting the sensitivity visuals on parent widget. */
     string = XmStringCreateLtoR( "Sensitive Parent", 
                                  XmSTRING_DEFAULT_CHARSET );
     n = 0;
     XtSetArg( args[n], XmNx, 0 ); n++;  
     XtSetArg( args[n], XmNy, POPUP_HEIGHT / 2 ); n++;  
     XtSetArg( args[n], XmNwidth, POPUP_WIDTH - 2 * MARGINS ); n++;
     XtSetArg( args[n], XmNheight, POPUP_HEIGHT / 2 ); n++;
     XtSetArg( args[n], XmNtopAttachment, XmATTACH_WIDGET ); n++;
     XtSetArg( args[n], XmNtopWidget, ToggleB1 ); n++;
     XtSetArg( args[n], XmNleftAttachment, XmATTACH_FORM ); n++;
     XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM ); n++;
     XtSetArg( args[n], XmNlabelString, string ); n++;
     ToggleB2 = XmCreateToggleButton(Form1, "VisualTB2", args, n );
     XtManageChild(ToggleB2);

     XmStringFree(string);

     XtAddCallback( ToggleB2, XmNvalueChangedCallback, 
                    SetParentVisuals, (XtPointer)Manager );

     XtPopup( PopupShell, XtGrabNone );
     XtRealizeWidget(Shell1);

     CommonPause();  
     CommonPause();  
     CommonPause();  
     CommonPause();  

     XtAppMainLoop( app_context ); 
}
        
     
      
      
