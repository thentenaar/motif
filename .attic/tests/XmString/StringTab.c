/* $XConsortium: StringTab.c /main/7 1996/08/15 14:17:58 jakku $ */
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

/*
 *   StringTab.c - 
 *     Tests the tab components within XmString.
 */
#include <testlib.h>

/*
 *  String initializations.
 */
char fontname[] = "fixed";
static char *rendstrings[] = {
  "Headings", "Entries" };

static char string1[] = 
"Fat and Skinny had a race";

static char *unit_types[] = {
 "PIXELS", "100TH_MILLIMETERS", "1000TH_INCHES", 
 "100TH_POINTS", "100TH_FONT_UNITS", "INCHES",
 "CENTIMETERS", "MILLIMETERS", "POINTS", 
 "FONT_UNITS" }; 

static char *offset_types[] = {
 "ABSOLUTE", "RELATIVE" };

static char *tab_options[] = {
 "Default Tabs", "Proposed Tabs" };

static char *headings[] = {
 "BREADS", "SALADS", "SOUPS" };

static char *breads[] = {
 "Rye", "Wheat", "Pumpernickel", " ", " " };

static char *salads[] = {
 "Tossed", "Veggie", "Greek", "Chef", " " };

static char *soups[] = {
 "Turkey Rice", "Minestrone", "Pea", "Chicken", "Tomato" };



/*
 *  Defines.
 */
#define NUM_ARGS   50
#define FORM_WIDTH 500
#define FORM_HEIGHT 350
#define OFFSET     5 
#define MARGINS    10
#define DRAW_AREA_WIDTH FORM_WIDTH
#define DRAW_AREA_HEIGHT FORM_HEIGHT / 2
#define NUM_UNITS  ( sizeof( unit_types ) / sizeof( unit_types[0] )) 
#define NUM_OFFSETS ( sizeof( offset_types ) / sizeof( offset_types[0] )) 
#define NUM_TAB_OPTIONS ( sizeof( tab_options )/ sizeof( tab_options[0] ))
#define NUM_COLUMNS ( sizeof( headings ) / sizeof( headings[0] ))   
#define NUM_ROWS    ( sizeof( breads ) / sizeof( breads[0] ) + 1 ) 
#define MAX_TABS   5
#define POPUP_WIDTH 500
#define POPUP_HEIGHT 125

/*
 *  Local structures.
 */
typedef struct _TabInfo
{
  float          value;
  unsigned char	 units;
  XmOffsetModel  offset_model;
  unsigned char  alignment;
  char           *decimal;
  XmTab          tab;
  XmString       tab_comp;
  XmTextPosition insert_pos;
} TabInfo;

/* 
 *  Global variables.
 */
Arg       args[NUM_ARGS];
int       n, i;
Widget    Form1, Text1, Scale1, UnitLabel, UnitMenu, OffsetLabel, OffsetMenu,
          UnitPullDown, UnitPB[NUM_UNITS],  OffsetPullDown, 
          OffsetPB[NUM_OFFSETS], DrawArea1, Label1,
          CreateTabPB, ResetTabsPB, DrawStringPB, VertSep1, VertSep2, HorizSep1,
          PopupShell, Form2, PositionLabel, ValueLabel, UnitLabel, OffsetLabel,
          PositionList, ValueList, UnitList, OffsetList,
          TabOptionsPullDown, TabOptionsMenu, 
          TabOptionsPB[NUM_TAB_OPTIONS];
XmString  Label, TextComp[MAX_TABS + 1], RendComp[2], TabbedString,
          UntabbedString, TempString,
          ListPosition[MAX_TABS], ListValue[MAX_TABS], ListUnit[MAX_TABS],
          ListOffset[MAX_TABS], stringTable[NUM_ROWS];
TabInfo   TabData[MAX_TABS];
XmTab     Tabs[MAX_TABS], TabPtrs[MAX_TABS], NewTab;
XmTabList TabList = NULL;
XmRendition rend;
XmRenderTable RenderTable = NULL;
Boolean   TabSet = False;
Display   *display;
Window    window; 
GC        gc;
XGCValues      gcv;
XFontStruct    *fontStruct;

/*
 *  Error Handler
 */
static int ProtoError(dpy, ev)
Display         *dpy;
XErrorEvent     *ev;
{

        fprintf(stderr, "Proto Error\n");
        exit(0);
}

unsigned char GetTabUnits( Widget w )
{
  unsigned char units = XmPIXELS;

     if ( w == UnitPB[0] )
          units = XmPIXELS;
     else if ( w == UnitPB[1] )
          units = Xm100TH_MILLIMETERS;
     else if ( w == UnitPB[2] )
          units = Xm1000TH_INCHES;
     else if ( w == UnitPB[3] )
          units = Xm100TH_POINTS;
     else if ( w == UnitPB[4] )
          units = Xm100TH_FONT_UNITS;
     else if ( w == UnitPB[5] )
          units = XmINCHES;
     else if ( w == UnitPB[6] )
          units = XmCENTIMETERS;
     else if ( w == UnitPB[7] )
          units = XmMILLIMETERS;
     else if ( w == UnitPB[8] )
          units = XmPOINTS;
     else if ( w == UnitPB[9] )
          units = XmFONT_UNITS;

  return( units );
}
           
XmOffsetModel GetTabOffsets( Widget w )
{
   XmOffsetModel  offset_model = XmABSOLUTE; 

   if ( w == OffsetPB[0] )
        offset_model = XmABSOLUTE;
   else if ( w == OffsetPB[1] )
        offset_model = XmRELATIVE;
  
  return( offset_model );
}

/*
 *  Callbacks 
 */
void CreateTab( Widget w, XtPointer client_data, XtPointer call_data )
{
  int      count, value;
  Widget   widgetID;
  TabInfo  *tab_info;
  char     *text, buffer[20];

  /*  Get the current tab count. */
  count = ( TabList ) ? XmTabListTabCount( TabList ) : 0;
  if ( count >= MAX_TABS )
       printf("\nMaximum number of tabs already created.\n");

  else /* ( count < MAX_TABS ) */
  {
     tab_info = &TabData[count];

    /*  Get tab value. */
     XmScaleGetValue( Scale1, &value );
     tab_info->value = value;

    /*  Update TabValue list. */
     sprintf(buffer, "%.1f", tab_info->value);
     ListValue[count] = XmStringCreateLtoR(buffer, XmFONTLIST_DEFAULT_TAG ); 

     n = 0;
     XtSetArg( args[n], XmNitemCount, count + 1 ); n++;
     XtSetArg( args[n], XmNitems, ListValue ); n++;
     XtSetArg( args[n], XmNvisibleItemCount, MAX_TABS ); n++;
     XtSetValues( ValueList, args, n );

    /*  Get tab units. */
     n = 0;
     XtSetArg( args[n], XmNmenuHistory, &widgetID ); n++;
     XtGetValues( UnitMenu, args, n );

     tab_info->units = GetTabUnits( widgetID );

    /*  Update TabUnit List. */
     ListUnit[count] = XmStringCreateLtoR(unit_types[tab_info->units],
                          XmFONTLIST_DEFAULT_TAG ); 

     n = 0;
     XtSetArg( args[n], XmNitemCount, count + 1 ); n++;
     XtSetArg( args[n], XmNitems, ListUnit ); n++;
     XtSetArg( args[n], XmNvisibleItemCount, MAX_TABS ); n++;
     XtSetValues( UnitList, args, n );

    /*  Get tab offset model. */
     n = 0;
     XtSetArg( args[n], XmNmenuHistory, &widgetID ); n++;
     XtGetValues( OffsetMenu, args, n );

     tab_info->offset_model = GetTabOffsets( widgetID );

    /*  If count is zero, offset_model must be absolute. */
     if (( count == 0 ) && tab_info->offset_model != XmABSOLUTE )
     {
        printf("For initial tab, Tab Offset must be Absolute. \n"); 
        tab_info->offset_model = XmABSOLUTE;
     } 

    /*  Update TabOffset List. */
     switch( tab_info->offset_model )
     {
        case XmABSOLUTE:
            i = 0;
            break;

        case XmRELATIVE:
            i = 1;
            break;
     }
 
     ListOffset[count] = 
         XmStringCreateLtoR(offset_types[i],
                            XmFONTLIST_DEFAULT_TAG ); 

     n = 0;
     XtSetArg( args[n], XmNitemCount, count + 1 ); n++;
     XtSetArg( args[n], XmNitems, ListOffset ); n++;
     XtSetArg( args[n], XmNvisibleItemCount, MAX_TABS ); n++;
     XtSetValues( OffsetList, args, n );

    /*  Create tab and insert at end of tab list. */
     tab_info->tab = XmTabCreate( tab_info->value, tab_info->units, 
                             tab_info->offset_model, 
                             XmALIGNMENT_BEGINNING, NULL );

     TabList = XmTabListInsertTabs( TabList, &tab_info->tab, 1, count );

    /*  Create tab component. */
     tab_info->tab_comp = XmStringComponentCreate( XmSTRING_COMPONENT_TAB,
                                                   0, NULL ); 

    /*  Get current insertion position from Text widget. */
     tab_info->insert_pos = XmTextGetInsertionPosition( Text1 );

     sprintf(buffer, "%d", tab_info->insert_pos);
     ListPosition[count] = XmStringCreateLtoR( buffer,
                             XmFONTLIST_DEFAULT_TAG );

     n = 0;
     XtSetArg( args[n], XmNitemCount, count + 1 ); n++;
     XtSetArg( args[n], XmNitems, ListPosition ); n++;
     XtSetArg( args[n], XmNvisibleItemCount, MAX_TABS ); n++;
     XtSetValues( PositionList, args, n );
   }
}

void PopupTabList( Widget w, XtPointer client_data, XtPointer call_data )
{

    if ( TabSet == False )
    {
       n = 0;
       XtSetArg( args[n], XmNx, 250 ); n++;
       XtSetArg( args[n], XmNy, 400 ); n++;
       XtSetArg( args[n], XmNwidth, POPUP_WIDTH ); n++;
       XtSetArg( args[n], XmNheight, POPUP_HEIGHT ); n++;
       PopupShell = XtCreatePopupShell("Current Tab Values", 
                                       topLevelShellWidgetClass,
                                       Shell1, args, n ); 
       XtManageChild(PopupShell);

       n = 0;
       XtSetArg( args[n], XmNwidth, POPUP_WIDTH ); n++;
       XtSetArg( args[n], XmNheight, POPUP_HEIGHT ); n++;
       Form2 = XmCreateForm( PopupShell, "Form2", args, n ); 
       XtManageChild(Form2);

       Label = XmStringCreateLtoR("Tab Positions", XmFONTLIST_DEFAULT_TAG );

       n = 0;
       XtSetArg( args[n], XmNtopAttachment, XmATTACH_FORM ); n++;
       XtSetArg( args[n], XmNleftAttachment, XmATTACH_FORM ); n++;
       XtSetArg( args[n], XmNwidth, POPUP_WIDTH / 4 ); n++;
       XtSetArg( args[n], XmNlabelString, Label ); n++;
       PositionLabel = XmCreateLabel( Form2, "PositionLabel", args, n );
       XtManageChild(PositionLabel);

       XmStringFree(Label);

       Label = XmStringCreateLtoR("Tab Values", XmFONTLIST_DEFAULT_TAG );

       n = 0;
       XtSetArg( args[n], XmNtopAttachment, XmATTACH_FORM ); n++;
       XtSetArg( args[n], XmNleftAttachment, XmATTACH_WIDGET ); n++;
       XtSetArg( args[n], XmNleftWidget, PositionLabel ); n++;
       XtSetArg( args[n], XmNwidth, POPUP_WIDTH / 4 ); n++;
       XtSetArg( args[n], XmNlabelString, Label ); n++;
       ValueLabel = XmCreateLabel( Form2, "ValueLabel", args, n );
       XtManageChild(ValueLabel);

       XmStringFree(Label);

       Label = XmStringCreateLtoR("Tab Units", XmFONTLIST_DEFAULT_TAG );

       n = 0;
       XtSetArg( args[n], XmNtopAttachment, XmATTACH_FORM ); n++;
       XtSetArg( args[n], XmNleftAttachment, XmATTACH_WIDGET ); n++;
       XtSetArg( args[n], XmNleftWidget, ValueLabel ); n++;
       XtSetArg( args[n], XmNwidth, POPUP_WIDTH / 4 ); n++;
       XtSetArg( args[n], XmNlabelString, Label ); n++;
       UnitLabel = XmCreateLabel( Form2, "UnitLabel", args, n );
       XtManageChild(UnitLabel);

       XmStringFree(Label);

       Label = XmStringCreateLtoR("Tab Offsets", XmFONTLIST_DEFAULT_TAG );

       n = 0;
       XtSetArg( args[n], XmNtopAttachment, XmATTACH_FORM ); n++;
       XtSetArg( args[n], XmNleftAttachment, XmATTACH_WIDGET ); n++;
       XtSetArg( args[n], XmNleftWidget, UnitLabel ); n++;
       XtSetArg( args[n], XmNwidth, POPUP_WIDTH / 4 ); n++;

       XtSetArg( args[n], XmNlabelString, Label ); n++;
       OffsetLabel = XmCreateLabel( Form2, "OffsetLabel", args, n );
       XtManageChild(OffsetLabel);

       XmStringFree(Label);

       n = 0;
       XtSetArg( args[n], XmNtopAttachment, XmATTACH_WIDGET ); n++;
       XtSetArg( args[n], XmNtopWidget, PositionLabel ); n++;
       XtSetArg( args[n], XmNleftAttachment, XmATTACH_FORM ); n++;
       XtSetArg( args[n], XmNlistSizePolicy, XmCONSTANT ); n++;
       XtSetArg( args[n], XmNwidth, POPUP_WIDTH / 4 ); n++;
       XtSetArg( args[n], XmNvisibleItemCount, MAX_TABS ); n++;
       PositionList = XmCreateList( Form2, "PositionList", args, n );
       XtManageChild(PositionList);

       n = 0;
       XtSetArg( args[n], XmNtopAttachment, XmATTACH_WIDGET ); n++;
       XtSetArg( args[n], XmNtopWidget, ValueLabel ); n++;
       XtSetArg( args[n], XmNleftAttachment, XmATTACH_WIDGET ); n++;
       XtSetArg( args[n], XmNleftWidget, PositionList ); n++;
       XtSetArg( args[n], XmNwidth, POPUP_WIDTH / 4 ); n++;
       XtSetArg( args[n], XmNlistSizePolicy, XmCONSTANT ); n++;
       XtSetArg( args[n], XmNvisibleItemCount, MAX_TABS ); n++;
       ValueList = XmCreateList( Form2, "ValueList", args, n );
       XtManageChild(ValueList);
       
       n = 0;
       XtSetArg( args[n], XmNtopAttachment, XmATTACH_WIDGET ); n++;
       XtSetArg( args[n], XmNtopWidget, UnitLabel ); n++;
       XtSetArg( args[n], XmNleftAttachment, XmATTACH_WIDGET ); n++;
       XtSetArg( args[n], XmNleftWidget, ValueList ); n++;
       XtSetArg( args[n], XmNwidth, POPUP_WIDTH / 4 ); n++;
       XtSetArg( args[n], XmNlistSizePolicy, XmCONSTANT ); n++;
       XtSetArg( args[n], XmNvisibleItemCount, MAX_TABS ); n++;
       UnitList = XmCreateList( Form2, "UnitList", args, n );
       XtManageChild(UnitList);
       
       n = 0;
       XtSetArg( args[n], XmNtopAttachment, XmATTACH_WIDGET ); n++;
       XtSetArg( args[n], XmNtopWidget, OffsetLabel ); n++;
       XtSetArg( args[n], XmNleftAttachment, XmATTACH_WIDGET ); n++;
       XtSetArg( args[n], XmNleftWidget, UnitList ); n++;
       XtSetArg( args[n], XmNwidth, POPUP_WIDTH / 4 ); n++;
       XtSetArg( args[n], XmNlistSizePolicy, XmCONSTANT ); n++;
       XtSetArg( args[n], XmNvisibleItemCount, MAX_TABS ); n++;
       OffsetList = XmCreateList( Form2, "OffsetList", args, n );
       XtManageChild(OffsetList);
       
       XtPopup( PopupShell, XtGrabNone ); 

       TabSet = True;
    }
}
    
void ResetTabs( Widget w, XtPointer client_data, XtPointer call_data )
{
   int            j;
   unsigned int   count, position[MAX_TABS];

    /*  Get the current tab count. */
     count = ( TabList ) ? XmTabListTabCount( TabList ) : 0;

     if ( count == 0 )
        return;

     for ( j = 0; j < count; j++ )
     {     
       position[j] = j;
       XmTabFree( TabData[j].tab ); 
       XmStringFree( TabData[j].tab_comp );
     }

     TabList = XmTabListRemoveTabs( TabList, position, count );

     rend = XmRenderTableGetRendition( RenderTable, "Tabs" );

     if ( rend )
     {
       n = 0;
       XtSetArg(args[n], XmNtabList, TabList); n++;
       XmRenditionUpdate( rend, args, n );

       RenderTable = XmRenderTableAddRenditions( RenderTable, &rend, 1,
                       XmMERGE_REPLACE );

       XmRenditionFree( rend );
     }

    /*  Free the current List items. */
     for ( j = 0; j < count; j++ )
     {
        if ( ListPosition[j] ) XmStringFree( ListPosition[j] ); 
        if ( ListValue[j] ) XmStringFree( ListValue[j] ); 
        if ( ListUnit[j] ) XmStringFree( ListUnit[j] ); 
        if ( ListOffset[j] ) XmStringFree( ListOffset[j] ); 
     }

    /*  Reset the Lists containing the current tab information. */
     n = 0;
     XtSetArg( args[n], XmNitemCount, 0 ); n++;
     XtSetArg( args[n], XmNitems, (XmStringTable)0 ); n++;
     XtSetValues( PositionList, args, n );

     XtSetValues( ValueList, args, n );
     XtSetValues( UnitList, args, n );
     XtSetValues( OffsetList, args, n );

     return;
}

void DrawString( Widget w, XtPointer client_data, XtPointer call_data )
{
   Widget         drawArea = (Widget)client_data;
   int            j, k, n, count, new_count;
   TabInfo        temp_tab_info;
   char           *string, *sub_string;
   int            string_length, sub_string_length, num_chars, NumTextComp;
   Dimension      width, height;
   XmTextPosition tab_pos;
   unsigned int   position[MAX_TABS]; 
   unsigned char  units, alignment;
   XmOffsetModel  offset;
   char           *decimal;
   float          value;

    /*  Free the old tabbed string. */
     if ( TabbedString )
     {
        XmStringFree( TabbedString );
        TabbedString = (XmString)0;
     }

    /*  Get the current tab count. */
     count = ( TabList ) ? XmTabListTabCount( TabList ) : 0;

    /*
     *  Clear the drawing area prior to displaying the next string.
     */
     n = 0;
     XtSetArg( args[n], XmNwidth, &width ); n++;
     XtSetArg( args[n], XmNheight, &height ); n++;
     XtGetValues( drawArea, args, n );

     XFillRectangle( display, window, gc, 0, 0, width, height );

    /*
     *  Create render table used for XmString display attributes.
     */
     n = 0;
     XtSetArg(args[n], XmNfontName, fontname);      n++;
     XtSetArg(args[n], XmNfontType, XmFONT_IS_FONT);       n++;
     XtSetArg(args[n], XmNloadModel, XmLOAD_IMMEDIATE);    n++;
     XtSetArg(args[n], XmNrenditionForeground, CommonGetColor("blue"));    n++;
     rend = XmRenditionCreate(drawArea, "Tabs", args, n);

     RenderTable = XmRenderTableAddRenditions( RenderTable, &rend, 
                                               1, XmMERGE_REPLACE ); 
     XmRenditionFree( rend );

    /*  If no tabs are set, display the Untabbed String. */
     if ( count <= 0 )
     {
       XmStringExtent( RenderTable, UntabbedString, &width, &height );
       XmStringDraw( display, window, RenderTable, UntabbedString,
                     gc, MARGINS, MARGINS, width, XmALIGNMENT_BEGINNING, 
                     XmSTRING_DIRECTION_L_TO_R, (XRectangle *)0 );
     }
     else
     {
        /* 
         *  If the first tab is not at position zero, then there is
         *  one more text component than there are tab components
         *  since a text component is created for the text from
         *  position zero to the first tab.
         */ 
         NumTextComp = ( TabData[0].insert_pos > 0 ) ? count + 1 : count; 

    /*  Shuffle TabData structures into insert position order. */
       for ( j = 0; j < count - 1; j++ )
       {
         for ( k = j + 1; k < count; k++ ) 
         {
            if ( TabData[j].insert_pos > TabData[k].insert_pos )
            {
               memcpy( &temp_tab_info, &TabData[j], sizeof(TabData[j]));
               memcpy( &TabData[j], &TabData[k], sizeof(TabData[j]));
               memcpy( &TabData[k], &temp_tab_info, sizeof(TabData[k]));
            }
         }
        }

    /*  Update the TabList with the new insert position order. */
     for ( j = 0; j < count; j++ )
     {
       position[j] = j;
       memcpy( &Tabs[j], &(TabData[j].tab), sizeof( Tabs[j] ));
     }

     TabList = XmTabListReplacePositions( TabList, position, Tabs, count ); 

    /* 
     *  Verify that
     *    1. The number of tabs remained the same.
     *    2. Tabs in new TabList are ordered the same as tabs
     *       in TabData array.
     */
     new_count = ( TabList ) ? XmTabListTabCount( TabList ) : 0;
     if ( new_count != count )
        printf("\nTabList count has changed after replacing all tabs.\n");

     for ( j = 0; j < count; j++ )
     {
        NewTab = XmTabListGetTab( TabList, j );
        value =
          XmTabGetValues( NewTab, &units, &offset, &alignment, &decimal );
        if ( (int)value != (int)TabData[j].value )
             printf("Value different.\n");
        if ( units != TabData[j].units )
             printf("Units different\n");
        if ( offset != TabData[j].offset_model )
             printf("Offset different\n");
        XmTabFree( NewTab );
     }

    /*  Create Rendition Components. */
     RendComp[0] = XmStringComponentCreate( XmSTRING_COMPONENT_RENDITION_BEGIN,
                     strlen("Tabs"), "Tabs" );
                      
     RendComp[1] = XmStringComponentCreate( XmSTRING_COMPONENT_RENDITION_END,
                     strlen("Tabs"), "Tabs" );
     

    /*
     *  Create Text Components corresponding to the text before
     *  and after tab breaks. 
     */
     string = XmTextGetString( Text1 );
     string_length = ( string ) ? strlen( string ) : 0; 
     for ( j = 0; j < count; j++ )
     {
        tab_pos = TabData[j].insert_pos;
        num_chars = ( j < count - 1 ) ?
                     TabData[j+1].insert_pos - tab_pos :
                     string_length - tab_pos;
        sub_string = (char *)XtMalloc( ( num_chars + 1 ) * sizeof( char ));
        XmTextGetSubstring( Text1, tab_pos, num_chars,
                            num_chars + 1, sub_string ); 

        sub_string_length = ( sub_string ) ? strlen( sub_string ) : 0;
        TextComp[j] = XmStringComponentCreate( XmSTRING_COMPONENT_TEXT,
                        sub_string_length, (XtPointer)sub_string );

       /*
        *  If there are to be more text substrings than tabs,
        *  create a text component for the substring from
        *  position zero to the first tab.
        */
        if (( j == ( count - 1 )) && ( NumTextComp > count ))
        {
          num_chars = TabData[0].insert_pos - 0; 
          XtFree( sub_string );
          sub_string = (char *)XtMalloc( ( num_chars + 1 ) * sizeof( char ));
          XmTextGetSubstring( Text1, 0, num_chars,
                            num_chars + 1, sub_string ); 
          sub_string_length = ( sub_string ) ? strlen( sub_string ) : 0;
          TextComp[count] = XmStringComponentCreate( XmSTRING_COMPONENT_TEXT,
                              sub_string_length, (XtPointer)sub_string );
        }

          XtFree( sub_string );
      }
      XtFree( string );

     /* Concat Rendition, Text and Tab components into one XmString. */
      for ( j = 0; j < count; j++ )
      {
         if ( j == 0 )
         {
           if ( NumTextComp > count )
           {
            TabbedString = XmStringConcat( RendComp[j], TextComp[count] );
            TempString = TabbedString;
            TabbedString = XmStringConcat( TempString, TabData[j].tab_comp );
            XmStringFree(TempString);
            TempString = TabbedString;
            TabbedString = XmStringConcat( TempString, TextComp[j] );
            XmStringFree(TempString);

           }
           else
           {
            TabbedString = XmStringConcat( RendComp[j], TabData[j].tab_comp );
            TempString = TabbedString;
            TabbedString = XmStringConcat( TempString, TextComp[j] );
            XmStringFree(TempString);
           }
         }
         else
         {
            TempString = TabbedString;
            TabbedString = XmStringConcat( TempString, TabData[j].tab_comp ); 
            XmStringFree(TempString);
            TempString = TabbedString;
            TabbedString = XmStringConcat( TempString, TextComp[j] ); 
            XmStringFree(TempString);
         }

         if ( j == count - 1 )
         {
            TempString = TabbedString;
            TabbedString = XmStringConcat( TempString, RendComp[1] ); 
            XmStringFree(TempString);
         }
      }

      for ( j = 0; j <= count; j++ ) {
	if (TextComp[j]) XmStringFree(TextComp[j]);
	TextComp[j] = NULL;
      }
      XmStringFree(RendComp[0]);
      XmStringFree(RendComp[1]);
     /*
      *  Update rendition with the current tablist.
      */
      rend = XmRenderTableGetRendition(RenderTable, "Tabs");
      if ( rend )
      {
        n = 0;
        XtSetArg(args[n], XmNtabList, TabList);            n++;
        XmRenditionUpdate( rend, args, n );

        RenderTable = XmRenderTableAddRenditions( RenderTable, &rend, 1,
                              XmMERGE_REPLACE );

        XmRenditionFree( rend );
      }
      
      XmStringExtent( RenderTable, TabbedString, &width, &height );
      XmStringDraw( display, window, RenderTable, TabbedString,
                    gc, MARGINS, MARGINS, width, XmALIGNMENT_BEGINNING, 
                    XmSTRING_DIRECTION_L_TO_R, (XRectangle *)0 );
     }
}

XmTabList CreateDefaultTabList()
{
   XmTab          tab;
   XmTabList      tabList = (XmTabList)0;
   int            i;
   float          tab_value;
   XmOffsetModel  offset;

     for ( i = 0; i < NUM_ROWS * 2; i++ )
     {
       offset = ( i % 2 ) ? XmRELATIVE : XmABSOLUTE;
       tab_value = 0.5;

       /*  Create tab and insert at end of tab list. */
       tab = XmTabCreate( tab_value, XmINCHES, offset,
                          XmALIGNMENT_BEGINNING, NULL );

       tabList = XmTabListInsertTabs( tabList, &tab, 1, i );
       XmTabFree( tab );
     }

   return( tabList );
}


void
DrawStringTable( Widget w, XtPointer client_data, XtPointer call_data )
{
  Widget                drawArea = (Widget)client_data;
  Widget	        widgetID;
  int			padValue;
  XmOffsetModel         offset;
  unsigned char         units;
  XmRendition           rendition[2];
  XmTabList             tabList;
  int 			i;
  Dimension             width, height;
  Position              y_coord;

    /*  Get tab options selected. */
     n = 0;
     XtSetArg( args[n], XmNmenuHistory, &widgetID ); n++;
     XtGetValues( TabOptionsMenu, args, n );

    /*  Create renditions. */
     n = 0;
     XtSetArg(args[n], XmNfontName, fontname);      n++;
     XtSetArg(args[n], XmNfontType, XmFONT_IS_FONT);       n++;
     XtSetArg(args[n], XmNloadModel, XmLOAD_IMMEDIATE);    n++;
     XtSetArg(args[n], XmNrenditionForeground, CommonGetColor("red"));    n++;
     rendition[0] = XmRenditionCreate(drawArea, rendstrings[0], args, n);


     n = 0;
     XtSetArg(args[n], XmNfontName, fontname);      n++;
     XtSetArg(args[n], XmNfontType, XmFONT_IS_FONT);       n++;
     XtSetArg(args[n], XmNloadModel, XmLOAD_IMMEDIATE);    n++;
     XtSetArg(args[n], XmNrenditionForeground, CommonGetColor("blue"));    n++;
     rendition[1] = XmRenditionCreate(drawArea, rendstrings[1], args, n);

     RenderTable = XmRenderTableAddRenditions( RenderTable, rendition, 2,
                     XmMERGE_REPLACE );

    /*  Default tab list. */
     if ( widgetID == TabOptionsPB[0] )
        tabList = CreateDefaultTabList();

    /*  Propose tab list. */
     else if ( widgetID == TabOptionsPB[1] )
     {
        XmScaleGetValue( Scale1, &padValue );

        n = 0;
        XtSetArg( args[n], XmNmenuHistory, &widgetID ); n++;
        XtGetValues( OffsetMenu, args, n );
 
        offset = GetTabOffsets( widgetID );
        
        n = 0;
        XtSetArg( args[n], XmNmenuHistory, &widgetID ); n++;
        XtGetValues( UnitMenu, args, n );
 
        units = GetTabUnits( widgetID );

       /*
        *  Create a Label widget which will supply display,
        *  renderTable and unit information to the XmStringTableProposeTablist
        *  function.
        *  Note:  Need to unmanage Label widget when test is run automated. 
        */
        n = 0;
        XtSetArg( args[n], XmNunitType, units ); n++;
        XtSetArg( args[n], XmNrenderTable, RenderTable ); n++;
        Label1 = XmCreateLabel( drawArea, "", args, n );
        if ( XtIsManaged( Label1 ))
             XtUnmanageChild( Label1 );

        tabList = XmStringTableProposeTablist( stringTable, NUM_ROWS,
                    Label1, (float)padValue, offset );                      

        XtDestroyWidget( Label1 );
      }

     /*
      *  Update the render table with the desired tablist.
      */
      n = 0;
      XtSetArg( args[0], XmNtabList, tabList ); n++; 
      XmRenditionUpdate( rendition[0], args, n );
      XmRenditionUpdate( rendition[1], args, n );

      RenderTable = XmRenderTableAddRenditions( RenderTable, rendition, 2,
                      XmMERGE_REPLACE );

      XmRenditionFree( rendition[0] );
      XmRenditionFree( rendition[1] );

      /*
       *  Reset the size of the DrawingArea widget.
       *  This usually needs to be done when test is run automated
       *  since drawing area shrinks to the height of the Label
       *  widget which was temporarily created.
       */
       n = 0;
       XtSetArg( args[n], XmNwidth, DRAW_AREA_WIDTH ); n++;
       XtSetArg( args[n], XmNheight, DRAW_AREA_HEIGHT ); n++;
       XtSetValues( drawArea, args, n );

      /*
       *  Clear the drawing area prior to displaying the next string.
       */
       XFillRectangle( display, window, gc, 0, 0, 
                       DRAW_AREA_WIDTH, DRAW_AREA_HEIGHT );

      /*
       *  Display the tabbed strings.
       */
      y_coord = MARGINS; 
      for ( i = 0; i < NUM_ROWS; i++ )
      {
        XmStringExtent( RenderTable, stringTable[i], &width, &height ); 
        XmStringDraw( display, window,
                RenderTable, stringTable[i], gc, 
                MARGINS, y_coord, width, XmALIGNMENT_BEGINNING, 
                XmSTRING_DIRECTION_L_TO_R, (XRectangle *)0 );
        y_coord += height;

      }

      XmTabListFree( tabList );
}
  

void
CreateStringTable()
{
	int		i, j;
	XmString	text, separator, tab, temp1, temp2, temp3;
	XmString	rend_begin, rend_end;

        tab = temp1 = temp2 = temp3 = (XmString)0;

  /*  Create an XmString with the column headings. */
    for ( i = 0; i < NUM_COLUMNS; i++ )
    {
        text = XmStringComponentCreate( XmSTRING_COMPONENT_TEXT,
                 strlen(headings[i]), headings[i] );
           temp1 = XmStringConcat( temp2, text );
           if ( temp2 ) XmStringFree( temp2 );
           XmStringFree( text );

        if ( i < NUM_COLUMNS - 1 )
        {
           tab = XmStringComponentCreate( XmSTRING_COMPONENT_TAB, (int)0,
                                          (XtPointer)0 );
        
           temp2 = temp1;
           temp1 = XmStringConcat( temp2, tab );
           XmStringFree( temp2 );
           XmStringFree( tab );
        }
        temp2 = temp1;
        tab = (XmString)0;
     }

	separator = XmStringSeparatorCreate();
        temp1 = temp2;
        temp3 = XmStringConcat( temp1, separator );
        XmStringFree( temp1 );
        XmStringFree( separator );

        j = 0;
        stringTable[j++] = XmStringPutRendition( temp3, rendstrings[0] );
        XmStringFree( temp3 );

	temp2 = (XmString)0;

     /*  Create XmStrings for each row of text below the headings. */
        for ( i = 0; i < NUM_ROWS - 1; i++, j++ )
        {
           text = XmStringComponentCreate( XmSTRING_COMPONENT_TEXT,
                    strlen(breads[i]), breads[i]);
           tab = XmStringComponentCreate( XmSTRING_COMPONENT_TAB, (int)0,
                                          (XtPointer)0 );
           if ( i == 0 )
           {
              rend_begin = XmStringComponentCreate(
                             XmSTRING_COMPONENT_RENDITION_BEGIN,
                             strlen( rendstrings[1]), rendstrings[1] );
              temp1 = XmStringConcat( rend_begin, text );
              temp2 = temp1;
              temp1 = XmStringConcat( temp2, tab );
	      XmStringFree( rend_begin );
	      XmStringFree( text );
	      XmStringFree( tab );
	      XmStringFree( temp2 );
           }
           else
           {
             temp1 = XmStringConcat( text, tab ); 
             XmStringFree( text );
             XmStringFree( tab );
           }

           text = XmStringComponentCreate( XmSTRING_COMPONENT_TEXT,
                    strlen(salads[i]), salads[i] );
           temp2 = temp1;
           temp1 = XmStringConcat( temp2, text );
           XmStringFree( temp2 );
           XmStringFree( text );

           tab = XmStringComponentCreate( XmSTRING_COMPONENT_TAB, (int)0,
                                          (XtPointer)0 );
           temp2 = temp1;
           temp1 = XmStringConcat( temp2, tab );
           XmStringFree( temp2 );
           XmStringFree( tab );

           text = XmStringComponentCreate( XmSTRING_COMPONENT_TEXT,
                    strlen( soups[i]), soups[i] );
           temp2 = temp1;
           temp1 = XmStringConcat( temp2, text );
           XmStringFree( temp2 );
           XmStringFree( text );

           if ( j < NUM_COLUMNS - 1 )
           {
              tab = XmStringComponentCreate( XmSTRING_COMPONENT_TAB, (int)0,
                                          (XtPointer)0 );
              temp2 = temp1;
              temp1 = XmStringConcat( temp2, tab );
              XmStringFree( temp2 );
              XmStringFree( tab );
           }

           if ( i == NUM_ROWS - 1 )
           {
              rend_end = XmStringComponentCreate(
                             XmSTRING_COMPONENT_RENDITION_END,
                             strlen( rendstrings[1]), rendstrings[1] );
              temp2 = temp1;
              separator = XmStringSeparatorCreate();
              temp1 = XmStringConcat( temp2, separator );
              XmStringFree( temp2 );
              XmStringFree( separator );
              stringTable[j] = XmStringConcat( temp1, rend_end );
           }
           else
           {
              separator = XmStringSeparatorCreate();
              stringTable[j] = XmStringConcat( temp1, separator );
              XmStringFree( temp1 );
              XmStringFree( separator );
           }
        }
}

void main( int argc, char **argv )
{
   char    name[20];
   Dimension width, height;

    TabbedString = (XmString)0;
    UntabbedString = XmStringGenerate( string1, XmSTRING_DEFAULT_CHARSET,
                       XmCHARSET_TEXT, (XmStringTag)"Tabs" );
    PopupShell = (Widget)0;

    XSetErrorHandler(ProtoError);

    CommonTestInit( argc, argv );


   /*  Create Manager Form widget for main panel. */
    n = 0;
    XtSetArg( args[n], XmNwidth, FORM_WIDTH ); n++;
    XtSetArg( args[n], XmNheight, FORM_HEIGHT ); n++;
    XtSetArg( args[n], XmNresizable, True ); n++;
    Form1 = XmCreateForm( Shell1, "Form1", args, n );
    XtManageChild( Form1 );

    n = 0;
    XtSetArg( args[n], XmNtopAttachment, XmATTACH_FORM ); n++;
    XtSetArg( args[n], XmNtopOffset, OFFSET ); n++;
    XtSetArg( args[n], XmNleftAttachment, XmATTACH_FORM ); n++;
    XtSetArg( args[n], XmNleftOffset, OFFSET ); n++;
    XtSetArg( args[n], XmNwidth, FORM_WIDTH / 3 ); n++;
    XtSetArg( args[n], XmNbackground, CommonGetColor("white")); n++;
    XtSetArg( args[n], XmNforeground, CommonGetColor("blue")); n++;
    XtSetArg( args[n], XmNrows, 8 ); n++;
    XtSetArg( args[n], XmNeditable, False ); n++;
    XtSetArg( args[n], XmNeditMode, XmMULTI_LINE_EDIT ); n++;
    XtSetArg( args[n], XmNwordWrap, False ); n++;
    XtSetArg( args[n], XmNvalue, string1 ); n++;
    Text1 = XmCreateText( Form1, "Text1", args, n );
    XtManageChild( Text1 );

    Label = XmStringCreateLtoR("Tab Value", XmFONTLIST_DEFAULT_TAG );

    n = 0;
    XtSetArg( args[n], XmNtopAttachment, XmATTACH_FORM ); n++;
    XtSetArg( args[n], XmNtopOffset, OFFSET ); n++;
    XtSetArg( args[n], XmNleftAttachment, XmATTACH_WIDGET ); n++;
    XtSetArg( args[n], XmNleftWidget, Text1 ); n++;
    XtSetArg( args[n], XmNleftOffset, OFFSET ); n++;
    XtSetArg( args[n], XmNorientation, XmHORIZONTAL ); n++;
    XtSetArg( args[n], XmNminimum, 0 ); n++;
    XtSetArg( args[n], XmNmaximum, 10 ); n++;
    XtSetArg( args[n], XmNshowValue, True ); n++;
    XtSetArg( args[n], XmNtitleString, Label ); n++;
    Scale1 = XmCreateScale( Form1, "Scale1", args, n );
    XtManageChild( Scale1 );

    XmStringFree( Label );

    UnitPullDown = XmCreatePulldownMenu( Form1, "UnitPullDown", NULL, 0 );

    for ( i = 0; i < NUM_UNITS; i++ )
    {
      Label = XmStringCreateLtoR( unit_types[i], XmFONTLIST_DEFAULT_TAG );
      sprintf( name, "UnitPushB%d", i );
      n = 0;
      XtSetArg( args[n], XmNlabelString, Label ); n++; 
      UnitPB[i] = XmCreatePushButton( UnitPullDown, name, args, n );
      XmStringFree(Label); 
    }

    XtManageChildren( UnitPB, NUM_UNITS );

    Label = XmStringCreateLtoR("Tab Units", XmFONTLIST_DEFAULT_TAG );

    n = 0;
    XtSetArg( args[n], XmNtopAttachment, XmATTACH_WIDGET ); n++;
    XtSetArg( args[n], XmNtopWidget, Scale1 ); n++;
    XtSetArg( args[n], XmNleftAttachment, XmATTACH_WIDGET ); n++;
    XtSetArg( args[n], XmNleftWidget, Text1 ); n++;
    XtSetArg( args[n], XmNleftOffset, OFFSET ); n++;
    XtSetArg( args[n], XmNsubMenuId, UnitPullDown); n++;
    XtSetArg( args[n], XmNmenuHistory, UnitPB[0]); n++;
    XtSetArg( args[n], XmNlabelString, Label ); n++;
    UnitMenu = XmCreateOptionMenu(Form1, "UnitMenu", args, n);
    XtManageChild( UnitMenu );
         
    XmStringFree( Label );

    OffsetPullDown = XmCreatePulldownMenu( Form1, "OffsetPullDown", NULL, 0 );

    for ( i = 0; i < NUM_OFFSETS; i++ )
    {
      Label = XmStringCreateLtoR( offset_types[i], XmFONTLIST_DEFAULT_TAG );
      sprintf( name, "OffsetPushB%d", i );
      n = 0;
      XtSetArg( args[n], XmNlabelString, Label ); n++; 
      OffsetPB[i] = XmCreatePushButton( OffsetPullDown, name, args, n );
      XmStringFree(Label); 
    }

    XtManageChildren( OffsetPB, NUM_OFFSETS );

    Label = XmStringCreateLtoR("Tab Offset", XmFONTLIST_DEFAULT_TAG );

    n = 0;
    XtSetArg( args[n], XmNtopAttachment, XmATTACH_WIDGET ); n++;
    XtSetArg( args[n], XmNtopWidget, UnitMenu ); n++;
    XtSetArg( args[n], XmNleftAttachment, XmATTACH_WIDGET ); n++;
    XtSetArg( args[n], XmNleftWidget, Text1 ); n++;
    XtSetArg( args[n], XmNleftOffset, OFFSET ); n++;
    XtSetArg( args[n], XmNsubMenuId, OffsetPullDown); n++;
    XtSetArg( args[n], XmNmenuHistory, OffsetPB[0]); n++;
    XtSetArg( args[n], XmNlabelString, Label ); n++;
    OffsetMenu = XmCreateOptionMenu(Form1, "OffsetMenu", args, n);
    XtManageChild( OffsetMenu );
         
    XmStringFree( Label );

    n = 0;
    XtSetArg( args[n], XmNtopAttachment, XmATTACH_WIDGET ); n++;
    XtSetArg( args[n], XmNtopWidget, OffsetMenu ); n++;
    XtSetArg( args[n], XmNleftAttachment, XmATTACH_FORM ); n++;
    XtSetArg( args[n], XmNleftOffset, OFFSET ); n++;
    XtSetArg( args[n], XmNrightAttachment, XmATTACH_FORM ); n++;
    XtSetArg( args[n], XmNrightOffset, OFFSET ); n++;
    XtSetArg( args[n], XmNorientation, XmHORIZONTAL ); n++;
    XtSetArg( args[n], XmNseparatorType, XmSHADOW_ETCHED_OUT ); n++;
    HorizSep1 = XmCreateSeparator( Form1, "HorizSep1", args, n );
    XtManageChild( HorizSep1 );

    n = 0;
    XtSetArg( args[n], XmNtopAttachment, XmATTACH_FORM ); n++;
    XtSetArg( args[n], XmNtopOffset, OFFSET ); n++;
    XtSetArg( args[n], XmNleftAttachment, XmATTACH_WIDGET ); n++;
    XtSetArg( args[n], XmNleftWidget, UnitMenu ); n++;
    XtSetArg( args[n], XmNleftOffset, OFFSET ); n++;
    XtSetArg( args[n], XmNbottomAttachment, XmATTACH_WIDGET ); n++;
    XtSetArg( args[n], XmNbottomWidget, HorizSep1 ); n++;
    XtSetArg( args[n], XmNbottomOffset, OFFSET ); n++;
    XtSetArg( args[n], XmNorientation, XmVERTICAL ); n++;
    XtSetArg( args[n], XmNseparatorType, XmSHADOW_ETCHED_OUT ); n++;
    VertSep1 = XmCreateSeparator( Form1, "VertSep1", args, n );
    XtManageChild( VertSep1 );

    Label = XmStringCreateLtoR("Set Tab", XmFONTLIST_DEFAULT_TAG );

   /*
    *  For some wierd reason, just attaching to form is not working. 
    *  Need to specify y-dimension.
    */
    n = 0;
    XtSetArg( args[n], XmNy, 2 * OFFSET ); n++;
    XtSetArg( args[n], XmNtopAttachment, XmATTACH_FORM ); n++;
    XtSetArg( args[n], XmNtopOffset, 2 * OFFSET ); n++;
    XtSetArg( args[n], XmNleftAttachment, XmATTACH_WIDGET ); n++;
    XtSetArg( args[n], XmNleftWidget, VertSep1 ); n++;
    XtSetArg( args[n], XmNleftOffset, OFFSET ); n++;
    XtSetArg( args[0], XmNlabelString, Label ); n++;
    CreateTabPB = XmCreatePushButton( Form1, "CreateTabPB", args, n );
    XtManageChild( CreateTabPB );

    XtAddCallback( CreateTabPB, XmNactivateCallback, PopupTabList, NULL );
    XtAddCallback( CreateTabPB, XmNactivateCallback, CreateTab, NULL );
 
    XmStringFree(Label);

    n = 0;
    XtSetArg( args[n], XmNheight, &height ); n++;
    XtGetValues( CreateTabPB, args, n );

   /*  Create button for Resetting Tabs. */ 

    Label = XmStringCreateLtoR("Reset Tabs", XmFONTLIST_DEFAULT_TAG );

    n = 0;
    XtSetArg( args[n], XmNy, height + 2 * OFFSET ); n++;
    XtSetArg( args[n], XmNtopAttachment, XmATTACH_WIDGET ); n++;
    XtSetArg( args[n], XmNtopWidget, CreateTabPB ); n++;
    XtSetArg( args[n], XmNtopOffset, OFFSET ); n++;
    XtSetArg( args[n], XmNleftAttachment, XmATTACH_WIDGET ); n++;
    XtSetArg( args[n], XmNleftWidget, VertSep1 ); n++;
    XtSetArg( args[n], XmNleftOffset, OFFSET ); n++;
    XtSetArg( args[0], XmNlabelString, Label ); n++;
    ResetTabsPB = XmCreatePushButton( Form1, "ResetTabsPB", args, n );
    XtManageChild( ResetTabsPB );

    XtAddCallback( ResetTabsPB, XmNactivateCallback, ResetTabs, NULL );

    XmStringFree(Label);

    Label = XmStringCreateLtoR("Draw String", XmFONTLIST_DEFAULT_TAG );

    n = 0;
    XtSetArg( args[n], XmNheight, &height ); n++;
    XtGetValues( ResetTabsPB, args, n );

   /*
    *  For some wierd reason, just attaching to top widget is not working. 
    *  Need to specify y-dimension.
    */
    n = 0;
    XtSetArg( args[n], XmNy, height + 2 * OFFSET ); n++;
    XtSetArg( args[n], XmNtopAttachment, XmATTACH_WIDGET ); n++;
    XtSetArg( args[n], XmNtopWidget, ResetTabsPB ); n++;
    XtSetArg( args[n], XmNtopOffset, OFFSET ); n++;
    XtSetArg( args[n], XmNleftAttachment, XmATTACH_WIDGET ); n++;
    XtSetArg( args[n], XmNleftWidget, VertSep1 ); n++;
    XtSetArg( args[n], XmNleftOffset, OFFSET ); n++;
    XtSetArg( args[0], XmNlabelString, Label ); n++;
    DrawStringPB = XmCreatePushButton( Form1, "DrawStringPB", args, n );
    XtManageChild( DrawStringPB );


    XmStringFree(Label);

   /*  Create drawing area widget for displaying tabbed string. */
    n = 0;
    XtSetArg( args[n], XmNtopAttachment, XmATTACH_WIDGET ); n++;
    XtSetArg( args[n], XmNtopWidget, HorizSep1 ); n++;
    XtSetArg( args[n], XmNtopOffset, OFFSET ); n++;
    XtSetArg( args[n], XmNleftAttachment, XmATTACH_FORM ); n++;
    XtSetArg( args[n], XmNleftOffset, OFFSET ); n++;
    XtSetArg( args[n], XmNrightAttachment, XmATTACH_FORM ); n++;
    XtSetArg( args[n], XmNrightOffset, OFFSET ); n++;
    XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM ); n++;
    XtSetArg( args[n], XmNbottomOffset, OFFSET ); n++;
    XtSetArg( args[n], XmNwidth, DRAW_AREA_WIDTH ); n++;
    XtSetArg( args[n], XmNheight, DRAW_AREA_HEIGHT ); n++;
    XtSetArg( args[n], XmNbackground, CommonGetColor("white")); n++;
    XtSetArg( args[n], XmNforeground, CommonGetColor("white")); n++;
    DrawArea1 = XmCreateDrawingArea( Form1, "DrawArea1", args, n ); 

    XtManageChild( DrawArea1 );

    XtAddCallback( DrawStringPB, XmNactivateCallback, DrawString, DrawArea1 );

    XtRealizeWidget(Shell1);

    display = XtDisplay(DrawArea1);
    window = XtWindow(DrawArea1);

    n = 0;
    XtSetArg(args[n], XmNforeground, &gcv.foreground); n++;
    XtSetArg(args[n], XmNbackground, &gcv.background); n++;
    XtSetArg(args[n], XmNwidth, &width); n++;
    XtSetArg(args[n], XmNheight, &height); n++;
    XtGetValues(DrawArea1, args, n);

    fontStruct = (XFontStruct *)XLoadQueryFont( display, fontname ); 

    gcv.font = fontStruct->fid;

    gc = XCreateGC( display, window, GCForeground | GCBackground | GCFont,
                     &gcv ); 

   /*  Test setting tabs in the given string. */
    CommonPause();
   
   /*
    *  Deallocate unnecessary structures from the first portion
    *  of the test.
    */
    XtUnmanageChild( Text1 );
    XtDestroyWidget( Text1 );

    if ( PopupShell )
    {
       XtPopdown( PopupShell );
       XtDestroyWidget( PopupShell );
    }
    XmRenderTableFree( RenderTable );
    RenderTable = (XmRenderTable)0;

   /*
    *  Reconfigure Control Panel to test XmStringProposeTablist.
    */
    XtUnmanageChild( Scale1 );
    Label = XmStringCreateLtoR("Pad Value", XmFONTLIST_DEFAULT_TAG );

    n = 0;
    XtSetArg( args[n], XmNleftAttachment, XmATTACH_FORM ); n++;
    XtSetArg( args[n], XmNleftOffset, OFFSET ); n++;
    XtSetArg( args[n], XmNminimumValue, 1 ); n++;
    XtSetArg( args[n], XmNmaximumValue, 20 ); n++;
    XtSetArg( args[n], XmNtitleString, Label ); n++;
    XtSetValues( Scale1, args, n );

    XmStringFree( Label );
    XtManageChild( Scale1 );

    XtUnmanageChild( UnitMenu );
    Label = XmStringCreateLtoR("Unit Model", XmFONTLIST_DEFAULT_TAG );

    n = 0;
    XtSetArg( args[n], XmNleftAttachment, XmATTACH_FORM ); n++;
    XtSetArg( args[n], XmNleftOffset, OFFSET ); n++;
    XtSetArg( args[n], XmNlabelString, Label ); n++;
    XtSetValues( UnitMenu, args, n );

    XmStringFree( Label );
    XtManageChild( UnitMenu );

    XtUnmanageChild( OffsetMenu );
    Label = XmStringCreateLtoR("Offset Model", XmFONTLIST_DEFAULT_TAG );

    n = 0;
    XtSetArg( args[n], XmNleftAttachment, XmATTACH_FORM ); n++;
    XtSetArg( args[n], XmNleftOffset, OFFSET ); n++;
    XtSetArg( args[n], XmNlabelString, Label ); n++;
    XtSetValues( OffsetMenu, args, n );

    XmStringFree( Label );
    XtManageChild( OffsetMenu );

    XtUnmanageChild( CreateTabPB );
    XtDestroyWidget( CreateTabPB );

    XtUnmanageChild( ResetTabsPB );
    XtDestroyWidget( ResetTabsPB );
    
    TabOptionsPullDown = 
       XmCreatePulldownMenu( Form1, "UnitPullDown", NULL, 0 );

    for ( i = 0; i < NUM_TAB_OPTIONS; i++ )
    {
      Label = XmStringCreateLtoR( tab_options[i], 
                XmFONTLIST_DEFAULT_TAG );
      sprintf( name, "TabOptionsPushB%d", i );
      n = 0;
      XtSetArg( args[n], XmNlabelString, Label ); n++; 
      TabOptionsPB[i] = 
         XmCreatePushButton( TabOptionsPullDown, name, args, n );
      XmStringFree(Label); 
    }

    XtManageChildren( TabOptionsPB, NUM_TAB_OPTIONS );

    Label = XmStringCreateLtoR("Tab Options", XmFONTLIST_DEFAULT_TAG );

    n = 0;
    XtSetArg( args[n], XmNtopAttachment, XmATTACH_FORM ); n++;
    XtSetArg( args[n], XmNleftAttachment, XmATTACH_WIDGET ); n++;
    XtSetArg( args[n], XmNleftWidget, VertSep1 ); n++;
    XtSetArg( args[n], XmNleftOffset, OFFSET ); n++;
    XtSetArg( args[n], XmNsubMenuId, TabOptionsPullDown); n++;
    XtSetArg( args[n], XmNmenuHistory, TabOptionsPB[0]); n++;
    XtSetArg( args[n], XmNlabelString, Label ); n++;
    TabOptionsMenu = 
       XmCreateOptionMenu(Form1, "TabOptionsMenu", args, n);
    XtManageChild( TabOptionsMenu );
         
    XmStringFree( Label );

    Label = XmStringCreateLtoR("Draw Strings", 
              XmFONTLIST_DEFAULT_TAG );

    n = 0;
    XtSetArg( args[n], XmNtopAttachment, XmATTACH_WIDGET ); n++;
    XtSetArg( args[n], XmNtopWidget, TabOptionsMenu ); n++;
    XtSetArg( args[n], XmNtopOffset, OFFSET ); n++;
    XtSetArg( args[n], XmNlabelString, Label ); n++;
    XtSetValues( DrawStringPB, args, n );

    XmStringFree( Label );

    XtRemoveCallback( DrawStringPB, XmNactivateCallback, DrawString, 
                      DrawArea1 ); 
    XtAddCallback( DrawStringPB, XmNactivateCallback, DrawStringTable, 
                   DrawArea1 ); 

    CreateStringTable();

   /*  Clear the drawing area for the next panel of tests. */
    n = 0;
    XtSetArg( args[n], XmNwidth, &width ); n++;
    XtSetArg( args[n], XmNheight, &height ); n++;
    XtGetValues( DrawArea1, args, n );

   /*
    *  Clear the drawing area prior to displaying the next 
    *  instruction panel.
    */
    XFillRectangle( display, window, gc, 0, 0, width, height );

    CommonPause();
    CommonPause();
    CommonPause();

    XtAppMainLoop( app_context );
}
