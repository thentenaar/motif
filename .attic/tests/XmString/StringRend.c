/* $XConsortium: StringRend.c /main/6 1996/06/11 17:23:37 pascale $ */
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

/*
 *  Defines
 */
#define NUM_RENDITIONS 5 
#define DISPLAY_WIDTH  300
#define DISPLAY_HEIGHT 400
#define DISPLAY_MARGIN 10

#define RENDER_PANEL_WIDTH 150
#define RENDER_PANEL_HEIGHT 200

#define RESOURCE_PANEL_WIDTH 350
#define RESOURCE_PANEL_HEIGHT 400

#define MERGE  0
#define MODIFY 1

typedef struct _RendResources {
  char          fontName[100];
  XmFontType	fontType;
  unsigned char	loadModel, underlineType, strikethruType;
  XtPointer	font;
  XmTabList	tabList;
  Pixel		background, foreground;
} RendResources;

/*
 *  Text for XmStrings.
 */
static char *text[] = {
  "Begin - Rendition 1",
  "Begin - Rendition 2", 
  "Begin - Rendition 3",
  "Begin - Rendition 4 - End", 
  "Begin - Rendition 5 - End",
  "End - Rendition 2", 
  "Rendition 3 - End",
  "End - Rendition 1" };

/*
 *  Resource names used in Rendition Resource Panel.
 */
static char *resources[] = {
  "Tag", "FontName", "FontType",
  "LoadModel", "Font", "TabList",
  "Background", "Foreground", 
  "UnderlineType", "StrikethruType" }; 

static char *font_types[] = {
  "AS_IS", "FONT_IS_FONT", "FONT_IS_FONTSET" };

static char *load_models[] = {
  "AS_IS", "LOAD_IMMEDIATE", "LOAD_DEFERRED" };

static char *line_types[] = {
  "AS_IS", "NO_LINE", "SINGLE_LINE",
  "DOUBLE_LINE", "SINGLE_DASHED_LINE", "DOUBLE_DASHED_LINE" };

#define NUM_STRINGS      ( sizeof(text) / sizeof( text[0] )) 
#define NUM_RESOURCES    ( sizeof( resources ) / sizeof( resources[0] ))
#define NUM_FONT_TYPES   ( sizeof( font_types ) / sizeof( font_types[0] )) 
#define NUM_LOAD_MODELS  ( sizeof( load_models ) / sizeof( load_models[0] )) 
#define NUM_LINE_TYPES   ( sizeof( line_types ) / sizeof( line_types[0] )) 


/*
 *  Global variables.
 */
XmRenderTable renderTable, origRenderTable;
RendResources   rendResources;
Widget drawingArea, RenderTagList; 
Widget MergePB, RemovePB, ModifyPB; 
Widget MergePopup, ModifyPopup;
Widget ApplyPB, CancelPB, QuitPB;
XmStringTag   *tag_list;
Widget MergeForm, ModifyForm, MergeOptions;
Widget ReplacePB, MergeOldPB, MergeNewPB, SkipPB;
Widget ResourceLabels[NUM_RESOURCES], ResourceTextF[NUM_RESOURCES];
Widget RenderTagTF, FontNameTF, FontTF, TabListTF, ForegroundTF,
       BackgroundTF;
Widget FontTypePullDown, FontTypePB[NUM_FONT_TYPES], FontTypeOptions,
       LoadModelPullDown, LoadModelPB[NUM_LOAD_MODELS], LoadModelOptions,
       UnderlinePullDown, UnderlinePB[NUM_LINE_TYPES], UnderlineOptions,
       StrikethruPullDown, StrikethruPB[NUM_LINE_TYPES], StrikethruOptions;
XmString finalString;
Window          window;
GC              gc;
XGCValues       gcv;
XFontStruct     *fontStruct;

XmStringTable listItems;

static unsigned char CvtStringToLineType();

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

static char *
CvtLineTypeToString( unsigned char line_type )
{
   char string[35];

     if ( line_type == XmNO_LINE )
	sprintf( string, "%s", "NO_LINE" );
     else if ( line_type == XmSINGLE_LINE )
	sprintf( string, "%s", "SINGLE_LINE" );
     else if ( line_type == XmDOUBLE_LINE )
	sprintf( string, "%s", "DOUBLE_LINE" );
     else if ( line_type == XmSINGLE_DASHED_LINE )
	sprintf( string, "%s", "SINGLE_DASHED_LINE" );
     else if ( line_type == XmDOUBLE_DASHED_LINE )
	sprintf( string, "%s", "DOUBLE_DASHED_LINE" );
     else if ( line_type == XmAS_IS )
	sprintf( string, "%s", "AS_IS" );

   return( string );
}

static unsigned char
CvtStringToLineType( char *string )
{
   unsigned char line_type;

     if ( strcmp( string, "NO_LINE" ) == 0 )
          line_type = XmNO_LINE;
     else if ( strcmp( string, "SINGLE_LINE" ) == 0 )
          line_type = XmSINGLE_LINE;
     else if ( strcmp( string, "DOUBLE_LINE" ) == 0 )
          line_type = XmDOUBLE_LINE;
     else if ( strcmp( string, "SINGLE_DASHED_LINE" ) == 0 )
          line_type = XmSINGLE_DASHED_LINE;
     else if ( strcmp( string, "DOUBLE_DASHED_LINE" ) == 0 )
          line_type = XmDOUBLE_DASHED_LINE;
     else if ( strcmp( string, "AS_IS" ) == 0 )
          line_type = XmAS_IS;

   return( line_type );
}

static void
CreateRenditions( XmRendition *renditions )  
{
   int           n, i;
   Arg           args[25];
   char          tag[10];
   XmTab         tab1, tab2;
   XmTabList     tabList = (XmTabList)0;

   /*
    *  Create renditions which will be applied to a group of XmStrings.
    */
    i = 0;
    n = 0;
    XtSetArg( args[n], XmNrenditionBackground, CommonGetColor("black")); n++;
    XtSetArg( args[n], XmNrenditionForeground, CommonGetColor("white")); n++;
    XtSetArg( args[n], XmNfontName, "8x13" ); n++;
    XtSetArg( args[n], XmNfontType, XmFONT_IS_FONT ); n++;
    XtSetArg( args[n], XmNloadModel, XmLOAD_IMMEDIATE ); n++;
    sprintf( tag, "render%d", i+1 ); 
    renditions[i++] = XmRenditionCreate( Shell1, (XmStringTag)tag, args, n ); 

    n = 0;
    XtSetArg( args[n], XmNrenditionBackground, CommonGetColor("blue")); n++;
    XtSetArg( args[n], XmNunderlineType, XmSINGLE_LINE ); n++;
    sprintf( tag, "render%d", i+1 ); 
    renditions[i++] = XmRenditionCreate( Shell1, (XmStringTag)tag, args, n ); 

    tab1 = XmTabCreate( 1.0, XmCENTIMETERS, XmABSOLUTE, XmALIGNMENT_BEGINNING,
                        (char *)0 );
    tabList = XmTabListInsertTabs( (XmTabList)0, &tab1, 1, 0 );
    XmTabFree( tab1 );

    n = 0;
    XtSetArg( args[n], XmNtabList, tabList); n++;
    XtSetArg( args[n], XmNunderlineType, XmNO_LINE ); n++;
    XtSetArg( args[n], XmNfontName, "8x13bold" ); n++;
    XtSetArg( args[n], XmNfontType, XmFONT_IS_FONT ); n++;
    XtSetArg( args[n], XmNloadModel, XmLOAD_IMMEDIATE ); n++;
    sprintf( tag, "render%d", i+1 ); 
    renditions[i++] = XmRenditionCreate( Shell1, (XmStringTag)tag, args, n ); 

    tab2 = XmTabCreate( 2.0, XmCENTIMETERS, XmABSOLUTE, XmALIGNMENT_BEGINNING,
                        (char *)0 );

    tabList = XmTabListInsertTabs( tabList, &tab2, 1, 0 );
    XmTabFree( tab2 );

    n = 0;
    XtSetArg( args[n], XmNrenditionForeground, CommonGetColor("yellow")); n++;
    XtSetArg( args[n], XmNtabList, tabList); n++;
    XtSetArg( args[n], XmNstrikethruType, XmSINGLE_DASHED_LINE ); n++;
    XtSetArg( args[n], XmNfontName, "9x15" ); n++;
    XtSetArg( args[n], XmNfontType, XmFONT_IS_FONT ); n++;
    XtSetArg( args[n], XmNloadModel, XmLOAD_IMMEDIATE ); n++;
    sprintf( tag, "render%d", i+1 );
    renditions[i++] = XmRenditionCreate( Shell1, (XmStringTag)tag, args, n );

    n = 0;
    XtSetArg( args[n], XmNfontName, XmAS_IS ); n++;
    XtSetArg( args[n], XmNfontType, XmAS_IS ); n++;
    XtSetArg( args[n], XmNloadModel, XmAS_IS ); n++;
    XtSetArg( args[n], XmNtabList, XmAS_IS); n++;
    XtSetArg( args[n], XmNrenditionForeground, XmUNSPECIFIED_PIXEL ); n++;
    XtSetArg( args[n], XmNrenditionBackground, XmUNSPECIFIED_PIXEL ); n++;
    XtSetArg( args[n], XmNunderlineType, XmAS_IS ); n++;
    XtSetArg( args[n], XmNstrikethruType, XmAS_IS ); n++;
    sprintf( tag, "render%d", i+1 );
    renditions[i++] = XmRenditionCreate( Shell1, (XmStringTag)tag, args, n );

   /*
    *  Create rendition table. 
    */
    renderTable =
      XmRenderTableAddRenditions( (XmRenderTable)0, renditions, NUM_RENDITIONS,
                                   XmMERGE_REPLACE );
    XmTabListFree( tabList );

    for ( i = 0; i < NUM_RENDITIONS; i++ )
        XmRenditionFree( renditions[i] );

   /*
    *  Keep a copy of the original render table so that rendition resource
    *  values can be restored..
    */
    origRenderTable = XmRenderTableCopy( renderTable, tag_list, 
                                         NUM_RENDITIONS );
  return;
}


static XmString
CreateStrings( XmStringTag *tag_list )
{
   int           n, i, j;
   Arg           args[25];
   XmString      textComponents[NUM_STRINGS];
   XmString      rendComponent;
   XmString      tabComponent;
   XmString      temp, temp2, separator, string;
   XmStringTable stringTable;
   char          *tab = "\t";
   char          *newline = "\n";
   char          *buffer;
   int           length;

  /*
   *  Create the XmString text components for each of the remaining
   *  XmStrings to be displayed. 
   */
   for ( j = 0; j < NUM_STRINGS; j++ )
       textComponents[j] = XmStringComponentCreate( XmSTRING_COMPONENT_TEXT, 
                             strlen( text[j]), text[j] );

/*  The first rendition begins with the first string. */
   i = 0;
   rendComponent = 
       XmStringComponentCreate( XmSTRING_COMPONENT_RENDITION_BEGIN,
                                strlen(tag_list[i]), tag_list[i] ); 
   string = XmStringConcat( rendComponent, textComponents[i] );
   XmStringFree( rendComponent );

   temp = string;
   separator = XmStringComponentCreate( XmSTRING_COMPONENT_SEPARATOR,
                 (int)0, 0 );
   string = XmStringConcat( temp, separator );
   XmStringFree( temp );
   XmStringFree( separator );

/*  The second rendition begins with the second string. */
   i++;
   rendComponent = 
       XmStringComponentCreate( XmSTRING_COMPONENT_RENDITION_BEGIN,
                                strlen(tag_list[i]), tag_list[i] ); 
    
   temp = string;
   string = XmStringConcat( temp, rendComponent );
   XmStringFree( temp );
   XmStringFree( rendComponent );

   temp = string;
   string = XmStringConcat( temp, textComponents[i] );
   XmStringFree( temp );

   temp = string;
   separator = XmStringSeparatorCreate();
   string = XmStringConcat( temp, separator );
   XmStringFree( temp );
   XmStringFree( separator );
   

/*  The third rendition begins with the third string which includes a tab. */
   i++;
   rendComponent = 
       XmStringComponentCreate( XmSTRING_COMPONENT_RENDITION_BEGIN,
                                strlen(tag_list[i]), tag_list[i] ); 
   tabComponent = 
       XmStringComponentCreate( XmSTRING_COMPONENT_TAB,
                                0, (XmStringTag)0 ); 

   temp = string;
   string = XmStringConcat( temp, rendComponent );
   XmStringFree( temp );
   XmStringFree( rendComponent );

   temp = string;
   string = XmStringConcat( temp, tabComponent );
   XmStringFree( temp );
   XmStringFree( tabComponent );

   temp = string;
   string = XmStringConcat( temp, textComponents[i] );
   XmStringFree( temp );

   temp = string;
   separator = XmStringSeparatorCreate();
   string = XmStringConcat( temp, separator );
   XmStringFree( temp );
   XmStringFree( separator );

/*
 *  The fourth rendition begins and ends with the fourth string 
 *  which includes a tab.
 */
   i++;
   length = strlen( tab ) + strlen( text[i] ) + 1;
   buffer = (char *)XtMalloc( sizeof(char) * ( length + 1 ));

   buffer = strcpy( buffer, tab );
   buffer = strcat( buffer, text[i] );

   temp = string;
   string = 
     XmStringGenerate( buffer, XmSTRING_DEFAULT_CHARSET, XmCHARSET_TEXT,
                       tag_list[i] );  
   XtFree( buffer );

   temp2 = XmStringConcat( temp, string );
   XmStringFree( temp );
   XmStringFree( string );

   string = temp2;

/*  Begin and end the fifth rendition with the fifth string. */
   i++;

   length = strlen( tab ) + strlen( text[i] ) + 2 * strlen( newline ) + 1;
   buffer = (char *)XtMalloc( sizeof(char) * ( length + 1 ));

   buffer = strcpy( buffer, newline );
   buffer = strcat( buffer, tab );
   buffer = strcat( buffer, text[i] );
   buffer = strcat( buffer, newline );

   temp = string;
   string = XmStringGenerate( buffer, XmSTRING_DEFAULT_CHARSET, XmCHARSET_TEXT,
                      tag_list[i] );
   XtFree( buffer );

   temp2 = XmStringConcat( temp, string );
   XmStringFree( temp );
   XmStringFree( string );

   string = temp2;

/*  End the second rendition at the beginning of the sixth string. */
   i++;
   rendComponent =
       XmStringComponentCreate( XmSTRING_COMPONENT_RENDITION_END,
                                strlen(tag_list[1]), tag_list[1] );
   tabComponent = 
       XmStringComponentCreate( XmSTRING_COMPONENT_TAB,
                                0, (XmStringTag)0 ); 
   temp = string;
   string = XmStringConcat( temp, rendComponent );
   XmStringFree( temp );
   XmStringFree( rendComponent );

   temp = string;
   string = XmStringConcat( temp, tabComponent );
   XmStringFree( temp );
   XmStringFree( tabComponent );

   temp = string;
   string = XmStringConcat( temp, textComponents[i] );
   XmStringFree( temp );

   temp = string;
   separator = XmStringSeparatorCreate();
   string = XmStringConcat( temp, separator );
   XmStringFree( temp );
   XmStringFree( separator );

/*  End the third rendition at the end of the seventh string. */
   i++;
   rendComponent =
       XmStringComponentCreate( XmSTRING_COMPONENT_RENDITION_END,
                                strlen(tag_list[2]), tag_list[2] );
   tabComponent = 
       XmStringComponentCreate( XmSTRING_COMPONENT_TAB,
                                0, (XmStringTag)0 ); 
   temp = string;
   string = XmStringConcat( temp, tabComponent );
   XmStringFree( temp );
   XmStringFree( tabComponent );

   temp = string;
   string = XmStringConcat( temp, textComponents[i] );
   XmStringFree( temp );

   temp = string;
   string = XmStringConcat( temp, rendComponent );
   XmStringFree( temp );
   XmStringFree( rendComponent );

   temp = string;
   separator = XmStringSeparatorCreate();
   string = XmStringConcat( temp, separator );
   XmStringFree( temp );
   XmStringFree( separator );


/*  End the first rendition at the beginning of the eighth string. */
   i++;
   rendComponent =
       XmStringComponentCreate( XmSTRING_COMPONENT_RENDITION_END,
                                strlen(tag_list[0]), tag_list[0] );
   temp = string;
   string = XmStringConcat( temp, rendComponent );
   XmStringFree( temp );
   XmStringFree( rendComponent );

   temp = string;
   string = XmStringConcat( temp, textComponents[i] );
   XmStringFree( temp );

   temp = string;
   separator = XmStringSeparatorCreate();
   string = XmStringConcat( temp, separator );
   XmStringFree( temp );
   XmStringFree( separator );

   for ( j = 0; j < NUM_STRINGS; j++ )
       XmStringFree( textComponents[j] );

   return( string );

}


static void
DisplayStrings()
{ 
   Position        x_coord, y_coord;
   Dimension       width, height;
   int             n, i;
   XRectangle      clip;
   XmFontType      type;
   Arg             args[25];

   x_coord = y_coord = DISPLAY_MARGIN;
   width = DISPLAY_WIDTH - 2 * DISPLAY_MARGIN;
   height = DISPLAY_HEIGHT - 2 * DISPLAY_MARGIN;

  /*
   *  Clear the drawing area.
   */
   XFillRectangle( display, window, gc, 0, 0, width, height );

   clip.x = x_coord;
   clip.y = y_coord;
   clip.width = width;
   clip.height = height;

    /*  Display XmString with tabs and renditions. */
    XmStringExtent( renderTable, finalString, &width, &height );
    XmStringDrawImage( display, window, renderTable, finalString, gc, 
                       x_coord, y_coord, width, XmALIGNMENT_BEGINNING,
                       XmSTRING_DIRECTION_L_TO_R, (XRectangle *)0 ); 
}

static void
SetMergeMode( Widget widget, XtPointer client_data, XtPointer call_data )
{
	Arg	args[5];
	int    	   n;
	Widget  pb = (Widget)client_data;

      n = 0;
      XtSetArg( args[n], XmNmenuHistory, pb ); n++;
      XtSetValues( MergeOptions, args, n );   
}

static void 
GetRendResources( XmStringTag render_tag )
{
	Arg		args[25];
	int		n;
	XmRendition	rendition;
        String          string;
        XmTabList       tabList;

    /*
     *  Get the rendition resource values. 
     */
     rendition = XmRenderTableGetRendition( renderTable, render_tag );

     n = 0;
     XtSetArg( args[n], XmNfontName, &string ); n++;
     XtSetArg( args[n], XmNfontType, &rendResources.fontType ); n++;
     XtSetArg( args[n], XmNloadModel, &rendResources.loadModel ); n++;
     XtSetArg( args[n], XmNfont, &rendResources.font ); n++;
     XtSetArg( args[n], XmNtabList, &tabList ); n++;
     XtSetArg( args[n], XmNrenditionBackground, &rendResources.background ); n++;
     XtSetArg( args[n], XmNrenditionForeground, &rendResources.foreground ); n++;
     XtSetArg( args[n], XmNunderlineType, &rendResources.underlineType ); n++;
     XtSetArg( args[n], XmNstrikethruType, 
                              &rendResources.strikethruType ); n++;
     XmRenditionRetrieve( rendition, args, n ); 

     if ( (int)string == XmAS_IS )
        sprintf( rendResources.fontName, "%s", "XmAS_IS" );
     else
        sprintf( rendResources.fontName, "%s", string );

     if ((int)tabList == XmAS_IS)
        rendResources.tabList = NULL;
     else
        rendResources.tabList = XmTabListCopy( tabList, 0, 0 );

     XmRenditionFree( rendition );

   return;
}

    
static void 
SetRendResources( XmStringTag render_tag )
{
	Arg		args[25];
	int		n;
	XmRendition	rendition;

    /*
     *  Set the latest resource values to the given rendition. 
     */
     rendition = XmRenderTableGetRendition( renderTable, render_tag );

     n = 0;
     if ( strcmp( rendResources.fontName, "XmAS_IS" ) == 0 )
     {
       XtSetArg( args[n], XmNfontName, XmAS_IS ); n++;
     } 
     else
     {
       XtSetArg( args[n], XmNfontName, rendResources.fontName ); n++;
     }
     XtSetArg( args[n], XmNfontType, rendResources.fontType ); n++;
     XtSetArg( args[n], XmNloadModel, &rendResources.loadModel ); n++;
    /*
     *  Don't reset Font or TabList - leave as is. 
     XtSetArg( args[n], XmNfont, &rendResources.font ); n++;
     XtSetArg( args[n], XmNtabList, &rendResources.tabList ); n++;
     */ 
     XtSetArg( args[n], XmNrenditionBackground, &rendResources.background ); n++;
     XtSetArg( args[n], XmNrenditionForeground, &rendResources.foreground ); n++;
     XtSetArg( args[n], XmNunderlineType, &rendResources.underlineType ); n++;
     XtSetArg( args[n], XmNstrikethruType, &rendResources.strikethruType ); n++;
     XmRenditionUpdate( rendition, args, n ); 
     XmRenditionFree( rendition );

     if ( rendResources.tabList )
        XmTabListFree( rendResources.tabList );

   return;
}

static void
DisplayRendResources( char *render_tag )
{
	Arg	args[25];
	int	n, i;
        char    buffer[40], *char_ptr;

    /*
     *  Assign the rendition resource values to their corresponding
     *  text field widget for display.
     */ 
     i = 0;
     
    /*  XmNtag */
     n = 0;
     XtSetArg( args[n], XmNvalue, render_tag ); n++;
     XtSetValues( RenderTagTF, args, n );

    /*  XmNfontName */
     n = 0;
     char_ptr = (char *)rendResources.fontName;

     XtSetArg( args[n], XmNvalue, char_ptr ); n++;
     XtSetValues( FontNameTF, args, n );


    /*  XmNfontType */
     n = 0;
     if ( rendResources.fontType == XmAS_IS )
     {
        XtSetArg( args[n], XmNmenuHistory, FontTypePB[0] ); n++;
     }
     else if ( rendResources.fontType == XmFONT_IS_FONT )
     {
        XtSetArg( args[n], XmNmenuHistory, FontTypePB[1] ); n++;
     }
     else if ( rendResources.fontType == XmFONT_IS_FONTSET )
     {
        XtSetArg( args[n], XmNmenuHistory, FontTypePB[2] ); n++;
     }
     XtSetValues( FontTypeOptions, args, n );

    /*  XmNloadModel */
     n = 0;
     if ( rendResources.loadModel == XmAS_IS )
     {
        XtSetArg( args[n], XmNmenuHistory, LoadModelPB[0] ); n++;
     }
     else if ( rendResources.loadModel == XmLOAD_IMMEDIATE )
     {
        XtSetArg( args[n], XmNmenuHistory, LoadModelPB[1] ); n++;
     }
     else if ( rendResources.loadModel == XmLOAD_DEFERRED )
     {
        XtSetArg( args[n], XmNmenuHistory, LoadModelPB[2] ); n++;
     }
     XtSetValues( LoadModelOptions, args, n );

    /*  XmNfont */
     if ( (int)rendResources.font == XmAS_IS )
        char_ptr = "XmAS_IS";
     else
        char_ptr = "Non-Null";

     n = 0;
     XtSetArg( args[n], XmNvalue, char_ptr ); n++;
     XtSetValues( FontTF, args, n );

    /*  XmNtabList */
     n = 0;
     if ( !rendResources.tabList )
        char_ptr = "XmAS_IS";
     else
        char_ptr = "Non-Null";

     n = 0;
     XtSetArg( args[n], XmNvalue, char_ptr ); n++;
     XtSetValues( TabListTF, args, n );

    /*  XmNrenditionBackground */
     n = 0;
     if ( rendResources.background != XmUNSPECIFIED_PIXEL )
        sprintf( buffer, "%d", rendResources.background );
     else
        sprintf( buffer, "%s", "UNSPECIFIED_PIXEL" );
     XtSetArg( args[n], XmNvalue, buffer ); n++;
     XtSetValues( BackgroundTF, args, n );

    /*  XmNrenditionForeground */
     n = 0;
     if ( rendResources.foreground != XmUNSPECIFIED_PIXEL )
        sprintf( buffer, "%d", rendResources.foreground );
     else
        sprintf( buffer, "%s", "UNSPECIFIED_PIXEL" );
     XtSetArg( args[n], XmNvalue, buffer ); n++;
     XtSetValues( ForegroundTF, args, n );


    /*  XmNunderlineType */
     n = 0;
     if ( rendResources.underlineType == XmAS_IS )
     {
         XtSetArg( args[n], XmNmenuHistory, UnderlinePB[0] ); n++;
     }
     if ( rendResources.underlineType == XmNO_LINE )
     {
         XtSetArg( args[n], XmNmenuHistory, UnderlinePB[1] ); n++;
     }
     if ( rendResources.underlineType == XmSINGLE_LINE )
     {
         XtSetArg( args[n], XmNmenuHistory, UnderlinePB[2] ); n++;
     }
     if ( rendResources.underlineType == XmDOUBLE_LINE )
     {
         XtSetArg( args[n], XmNmenuHistory, UnderlinePB[3] ); n++;
     }
     if ( rendResources.underlineType == XmSINGLE_DASHED_LINE )
     {
         XtSetArg( args[n], XmNmenuHistory, UnderlinePB[4] ); n++;
     }
     if ( rendResources.underlineType == XmDOUBLE_DASHED_LINE )
     {
         XtSetArg( args[n], XmNmenuHistory, UnderlinePB[5] ); n++;
     }
     XtSetValues( UnderlineOptions, args, n );

    /*  XmNstrikethruType */
     n = 0;
     if ( rendResources.strikethruType == XmAS_IS )
     {
         XtSetArg( args[n], XmNmenuHistory, StrikethruPB[0] ); n++;
     }
     if ( rendResources.strikethruType == XmNO_LINE )
     {
         XtSetArg( args[n], XmNmenuHistory, StrikethruPB[1] ); n++;
     }
     if ( rendResources.strikethruType == XmSINGLE_LINE )
     {
         XtSetArg( args[n], XmNmenuHistory, StrikethruPB[2] ); n++;
     }
     if ( rendResources.strikethruType == XmDOUBLE_LINE )
     {
         XtSetArg( args[n], XmNmenuHistory, StrikethruPB[3] ); n++;
     }
     if ( rendResources.strikethruType == XmSINGLE_DASHED_LINE )
     {
         XtSetArg( args[n], XmNmenuHistory, StrikethruPB[4] ); n++;
     }
     if ( rendResources.strikethruType == XmDOUBLE_DASHED_LINE )
     {
         XtSetArg( args[n], XmNmenuHistory, StrikethruPB[5] ); n++;
     }
     XtSetValues( StrikethruOptions, args, n );
}

static void 
MergeRendResources( XmStringTag render_tag,
		  XmMergeMode merge_mode )
{
	Arg		args[25];
	int		n;
	XmRendition	rendition;
        XmStringTag     *tags;
        int             num_tags;

    /*
     *  Set the rendition resource values. 
     */
     n = 0;
     if ( strcmp( rendResources.fontName, "XmAS_IS" ) == 0 )
     {
       XtSetArg( args[n], XmNfontName, XmAS_IS ); n++;
     } 
     else
     {
       XtSetArg( args[n], XmNfontName, rendResources.fontName ); n++;
     }
     XtSetArg( args[n], XmNfontType, rendResources.fontType ); n++;
     XtSetArg( args[n], XmNloadModel, rendResources.loadModel ); n++;
     if ( (int)rendResources.font == XmAS_IS )
     {
       XtSetArg( args[n], XmNfont, XmAS_IS ); n++;
     }
     else
     {
       XtSetArg( args[n], XmNfont, rendResources.font ); n++;
     }
     if ( (int)rendResources.tabList == XmAS_IS )
     {
       XtSetArg( args[n], XmNtabList, XmAS_IS ); n++;
     }
     else
     {
       XtSetArg( args[n], XmNtabList, rendResources.tabList ); n++;
     }
     XtSetArg( args[n], XmNrenditionBackground, rendResources.background ); n++;
     XtSetArg( args[n], XmNrenditionForeground, rendResources.foreground ); n++;
     XtSetArg( args[n], XmNunderlineType, rendResources.underlineType ); n++;
     XtSetArg( args[n], XmNstrikethruType, rendResources.strikethruType ); n++;
     rendition = XmRenditionCreate( Shell1, render_tag, args, n );

     renderTable =
     XmRenderTableAddRenditions( renderTable, &rendition, 1, merge_mode );
     XmRenditionFree( rendition );

   return;
}

static void
ModifyRendResources( XmStringTag render_tag )
{
     Arg		args[25];
     int		n;
     XmRendition	rendition;

     rendition = XmRenderTableGetRendition( renderTable, render_tag );

    /*
     *  Set the rendition resource values. 
     */
     n = 0;
     if ( strcmp( rendResources.fontName, "XmAS_IS" ) == 0 )
     {
       XtSetArg( args[n], XmNfontName, XmAS_IS ); n++;
     } 
     else
     {
       XtSetArg( args[n], XmNfontName, rendResources.fontName ); n++;
     }
     XtSetArg( args[n], XmNfontType, rendResources.fontType ); n++;
     XtSetArg( args[n], XmNloadModel, rendResources.loadModel ); n++;
     if ( (int)rendResources.font == XmAS_IS )
     {
       XtSetArg( args[n], XmNfont, XmAS_IS ); n++;
     }
     else
     {
       XtSetArg( args[n], XmNfont, rendResources.font ); n++;
     }
     if ( (int)rendResources.tabList == XmAS_IS )
     {
       XtSetArg( args[n], XmNtabList, XmAS_IS ); n++;
     }
     else
     {
       XtSetArg( args[n], XmNtabList, rendResources.tabList ); n++;
     }
     XtSetArg( args[n], XmNrenditionBackground, rendResources.background ); n++;
     XtSetArg( args[n], XmNrenditionForeground, rendResources.foreground ); n++;
     XtSetArg( args[n], XmNunderlineType, rendResources.underlineType ); n++;
     XtSetArg( args[n], XmNstrikethruType, rendResources.strikethruType ); n++;
     XmRenditionUpdate( rendition, args, n );

     renderTable =
     XmRenderTableAddRenditions( renderTable, &rendition, 1, XmMERGE_REPLACE ); 
     XmRenditionFree( rendition );
  
   return;
}

static Widget
CreateMergeOptions( Widget parent, Dimension width, Dimension height )
{
        Arg		args[20];
	int		n;
	XmString	string;
	Widget		MergeLabel, MergeOptions, MergePulldown;

     n = 0;
     MergePulldown = XmCreatePulldownMenu( parent, "MergePulldown", 
                                           args, n );
    /*
     *  Create pushbuttons for each merge option.
     */
     string = XmStringCreateLtoR("Replace", XmFONTLIST_DEFAULT_TAG );

     n = 0;
     XtSetArg( args[n], XmNlabelString, string ); n++;
     XtSetArg( args[n], XmNforeground, CommonGetColor("white")); n++;
     ReplacePB = XmCreatePushButton( MergePulldown, "ReplacePB", args, n );
     XtManageChild( ReplacePB );
     XmStringFree(string);

     XtAddCallback( ReplacePB, XmNactivateCallback, SetMergeMode, 
                    (XtPointer)ReplacePB );

     string = XmStringCreateLtoR("Merge Old", XmFONTLIST_DEFAULT_TAG );

     n = 0;
     XtSetArg( args[n], XmNlabelString, string ); n++;
     XtSetArg( args[n], XmNforeground, CommonGetColor("white")); n++;
     MergeOldPB = XmCreatePushButton( MergePulldown, "MergeOldPB", args, n );
     XtManageChild( MergeOldPB );
     XmStringFree(string);

     XtAddCallback( MergeOldPB, XmNactivateCallback, SetMergeMode, 
                    (XtPointer)MergeOldPB );

     string = XmStringCreateLtoR("Merge New", XmFONTLIST_DEFAULT_TAG );

     n = 0;
     XtSetArg( args[n], XmNlabelString, string ); n++;
     XtSetArg( args[n], XmNforeground, CommonGetColor("white")); n++;
     MergeNewPB = XmCreatePushButton( MergePulldown, "MergeNewPB", args, n );
     XtManageChild( MergeNewPB );
     XmStringFree(string);

     XtAddCallback( MergeNewPB, XmNactivateCallback, SetMergeMode, 
                    (XtPointer)MergeNewPB );

     string = XmStringCreateLtoR("Skip", XmFONTLIST_DEFAULT_TAG );

     n = 0;
     XtSetArg( args[n], XmNlabelString, string ); n++;
     XtSetArg( args[n], XmNforeground, CommonGetColor("white")); n++;
     SkipPB = XmCreatePushButton( MergePulldown, "SkipPB", args, n );
     XtManageChild( SkipPB );
     XmStringFree(string);

     XtAddCallback( SkipPB, XmNactivateCallback, SetMergeMode, 
                    (XtPointer)SkipPB );

    /*
     *  Create option menu.
     */ 
     string = XmStringCreateLtoR("Merge Mode:", XmFONTLIST_DEFAULT_TAG );
     n = 0;
     XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
     XtSetArg( args[n], XmNlabelString, string ); n++;
     MergeLabel = XmCreateLabel( parent, "MergeLabel", args, n );
     XtManageChild( MergeLabel );
     XmStringFree(string);

     n = 0;
     XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
     XtSetArg(args[n], XmNleftWidget, MergeLabel); n++;
     XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
     XtSetArg(args[n], XmNsubMenuId,   MergePulldown);   n++;
     XtSetArg(args[n], XmNmenuHistory, ReplacePB);    n++;
     XtSetArg(args[n], XmNwidth, width );    n++;
     XtSetArg(args[n], XmNheight, height );    n++;
     XtSetArg(args[n], XmNforeground, CommonGetColor("white"));    n++;
     MergeOptions = XmCreateOptionMenu(parent, "MergeOptions", args, n);

     return( MergeOptions );
}

static void
GetDisplayResources( XmStringTag render_tag )
{
    Arg            args[20];
    int            n, i;
    char           *value;
    Widget         widgetID;
    XmTabList      tabList;
    XmRendition    rendition;

   /*
    *  Translate the values displayed in the resource panel
    *  to the correct rendition resource value.
    */


   /*  XmNfontName */
    n = 0;
    XtSetArg( args[n], XmNvalue, &value ); n++; 
    XtGetValues( FontNameTF, args, n ); 

    if ( ! ( strcmp( value, "XmAS_IS" ) == 0 ) ||
         ! ( strcmp( value, "" ) == 0 ))
       sprintf( rendResources.fontName, "%s", value );

   /*  XmNfontType */
    n = 0;
    XtSetArg( args[n], XmNmenuHistory, &widgetID ); n++; 
    XtGetValues( FontTypeOptions, args, n ); 
   
    if ( widgetID == FontTypePB[0] )
       rendResources.fontType = XmAS_IS;
    else if ( widgetID == FontTypePB[1] )
       rendResources.fontType = XmFONT_IS_FONT;
    else if ( widgetID == FontTypePB[2] )
       rendResources.fontType = XmFONT_IS_FONTSET;

   /*  XmNloadModel */
    n = 0;
    XtSetArg( args[n], XmNmenuHistory, &widgetID ); n++; 
    XtGetValues( LoadModelOptions, args, n ); 

    if ( widgetID == LoadModelPB[0] )
       rendResources.loadModel = XmAS_IS;
    else if ( widgetID == LoadModelPB[1] )
       rendResources.loadModel = XmLOAD_IMMEDIATE;
    else if ( widgetID == LoadModelPB[2] )
       rendResources.loadModel = XmLOAD_DEFERRED;
 
   /*  XmNfont */
    n = 0;
    XtSetArg( args[n], XmNvalue, &value ); n++; 
    XtGetValues( FontTF, args, n ); 

    if ( ! strcmp( value, "Non-Null" ) == 0 )
         rendResources.font = NULL;

   /*  XmNtabList 
    *
    *  For now, tabList resource is displayed as either Null or Non-Null.
    *  The actual value of tabList is left untouched in rendResources
    *  structure.
    */
    n = 0;
    XtSetArg( args[n], XmNvalue, &value ); n++; 
    XtGetValues( TabListTF, args, n ); 

                            
   /*  XmNrenditionBackground */
    n = 0;
    XtSetArg( args[n], XmNvalue, &value ); n++; 
    XtGetValues( BackgroundTF, args, n ); 

    if ( strcmp( value, "UNSPECIFIED_PIXEL") == 0 )
       rendResources.background = XmUNSPECIFIED_PIXEL;
    else
       rendResources.background = (Pixel)atoi( value );

   /*  XmNrenditionForeground */
    n = 0;
    XtSetArg( args[n], XmNvalue, &value ); n++; 
    XtGetValues( ForegroundTF, args, n ); 

    if ( strcmp( value, "UNSPECIFIED_PIXEL") == 0 )
       rendResources.foreground = XmUNSPECIFIED_PIXEL;
    else
       rendResources.foreground = (Pixel)atoi( value );

   /*  XmNunderlineType */
    n = 0;
    XtSetArg( args[n], XmNmenuHistory, &widgetID ); n++;
    XtGetValues( UnderlineOptions, args, n ); 

    if ( widgetID == UnderlinePB[0] )
       rendResources.underlineType = XmAS_IS; 
    else if ( widgetID == UnderlinePB[1] )
       rendResources.underlineType = XmNO_LINE; 
    else if ( widgetID == UnderlinePB[2] )
       rendResources.underlineType = XmSINGLE_LINE; 
    else if ( widgetID == UnderlinePB[3] )
       rendResources.underlineType = XmDOUBLE_LINE; 
    else if ( widgetID == UnderlinePB[4] )
       rendResources.underlineType = XmSINGLE_DASHED_LINE; 
    else if ( widgetID == UnderlinePB[5] )
       rendResources.underlineType = XmDOUBLE_DASHED_LINE; 

   /*  XmNstrikethruType */
    n = 0;
    XtSetArg( args[n], XmNmenuHistory, &widgetID ); n++;
    XtGetValues( StrikethruOptions, args, n );

    if ( widgetID == StrikethruPB[0] )
       rendResources.strikethruType = XmAS_IS;
    else if ( widgetID == StrikethruPB[1] )
       rendResources.strikethruType = XmNO_LINE;
    else if ( widgetID == StrikethruPB[2] )
       rendResources.strikethruType = XmSINGLE_LINE;
    else if ( widgetID == StrikethruPB[3] )
       rendResources.strikethruType = XmDOUBLE_LINE;
    else if ( widgetID == StrikethruPB[4] )
       rendResources.strikethruType = XmSINGLE_DASHED_LINE;
    else if ( widgetID == StrikethruPB[5] )
       rendResources.strikethruType = XmDOUBLE_DASHED_LINE;
}

static void
MergeApply( Widget widget, XtPointer client_data, XtPointer call_data )
{
    XmStringTag    render_tag = (XmStringTag)client_data;
    int            n;
    Arg            args[5];
    XmMergeMode    merge_mode;
    Widget	   mergePB;

   /*
    *  Get the current rendition resource values from the merge
    *  panel. 
    */
    GetDisplayResources( render_tag );

   /*
    *  Get the current merge mode from the merge
    *  option menu and merge the new rendition
    *  values with the current render table. 
    */
    n = 0;
    XtSetArg( args[n], XmNmenuHistory, &mergePB ); n++;
    XtGetValues( MergeOptions, args, n );
    if ( mergePB == ReplacePB )
         merge_mode = XmMERGE_REPLACE;
    else if ( mergePB == MergeOldPB ) 
         merge_mode = XmMERGE_OLD;
    else if ( mergePB == MergeNewPB ) 
         merge_mode = XmMERGE_NEW;
    else if ( mergePB == SkipPB ) 
         merge_mode = XmSKIP;

    MergeRendResources( render_tag, merge_mode );

  /*  Redisplay XmString. */
   DisplayStrings();

   return;
}
static void
ModifyApply( Widget widget, XtPointer client_data, XtPointer call_data )
{
    XmStringTag    render_tag = (XmStringTag)client_data;

   /*
    *  Get the current rendition resource values from the modify
    *  panel.
    */
    GetDisplayResources( render_tag );

   /*
    *  Update the rendition with the new rendition values.
    */
    ModifyRendResources( render_tag );   

  /*  Redisplay XmString. */
   DisplayStrings();
}

static void
MergeCancel( Widget widget, XtPointer client_data, XtPointer call_data )
{
	char            *render_tag = (XmStringTag)client_data; 
        int             n;
        Arg             args[5];

     /*
      *  Reset MergeOptions back to default value.
      */
      n = 0;
      XtSetArg( args[n], XmNmenuHistory, ReplacePB ); n++;
      XtSetValues( MergeOptions, args, n );   

     /*
      *  Redisplay the current rendition resource values.
      */
      GetRendResources( render_tag );
      DisplayRendResources( render_tag );
}

static void
ModifyCancel( Widget widget, XtPointer client_data, XtPointer call_data )
{
        char            *render_tag = (XmStringTag)client_data;

     /*
      *  Redisplay the current rendition resource values.
      */
      GetRendResources( render_tag );
      DisplayRendResources( render_tag );
}



static void
MergeQuit( Widget widget, XtPointer client_data, XtPointer call_data )
{

    /*  
     *  Upon quitting from a Merge Panel, reset the other pushbuttons
     *  from the resource panel back to being sensitive.
     */
     XtSetSensitive( ModifyPB, True );
     XtSetSensitive( RemovePB, True );
     XtSetSensitive( RenderTagList, True );

/* Free tab list	*/
     if ( rendResources.tabList )
        XmTabListFree( rendResources.tabList );

     XtPopdown( MergePopup );
     XtDestroyWidget( MergePopup );
}

static void
ModifyQuit( Widget widget, XtPointer client_data, XtPointer call_data )
{
    /*
     *  Upon quitting from a Modify Panel, reset the other pushbuttons
     *  from the resource panel back to being sensitive.
     */
     XtSetSensitive( MergePB, True );
     XtSetSensitive( RemovePB, True );
     XtSetSensitive( RenderTagList, True );

     XtPopdown( ModifyPopup );
     XtDestroyWidget( ModifyPopup );
}


static Widget
CreateResourcePanel( XmStringTag render_tag, Widget parent, Position x_coord, 
                     Position y_coord, Widget top_attach ) 
{
	Arg     args[20];
        int     i, n;
	Widget	ResourcePanel;
	XmString string;
	char     buffer[40];


     n = 0;
     if ( top_attach )
     {
        XtSetArg( args[n], XmNtopAttachment, XmATTACH_WIDGET ); n++;
        XtSetArg( args[n], XmNtopWidget, top_attach ); n++;
     }
     else
     {
        XtSetArg( args[n], XmNtopAttachment, XmATTACH_FORM ); n++;
     }
     XtSetArg( args[n], XmNleftAttachment, XmATTACH_FORM ); n++;
     XtSetArg( args[n], XmNrightAttachment, XmATTACH_FORM ); n++;
     XtSetArg( args[n], XmNnumColumns, 2 ); n++;
     XtSetArg( args[n], XmNpacking, XmPACK_COLUMN ); n++;
     XtSetArg( args[n], XmNorientation, XmVERTICAL ); n++;
     XtSetArg( args[n], XmNspacing, 5 ); n++;
     XtSetArg( args[n], XmNmarginWidth, 5 ); n++;
     XtSetArg( args[n], XmNentryAlignment, XmALIGNMENT_BEGINNING ); n++;
     ResourcePanel = XmCreateRowColumn( parent, "ResourcePanel", args, n );

     for ( i = 0; i < NUM_RESOURCES; i++ )
     {
	string = XmStringCreateLtoR( resources[i], XmFONTLIST_DEFAULT_TAG );
        n = 0;
        XtSetArg( args[n], XmNlabelString, string ); n++;
        sprintf( buffer, "Label%d", i );
        ResourceLabels[i] = 
          XmCreateLabel( ResourcePanel, buffer, args, n );
        XtManageChild(ResourceLabels[i]);
        XmStringFree(string);
     }

       /*
        *  The renderTag resource is not settable.
        */
        n = 0;
        XtSetArg( args[n], XmNeditable, False ); n++;
        XtSetArg( args[n], XmNforeground, CommonGetColor("white")); n++;
        RenderTagTF = XmCreateTextField( ResourcePanel, "RenderTagTF",
				args, n ); 
        XtManageChild(RenderTagTF);

       /*
        *  XmNfontName
        */
        n = 0;
        XtSetArg( args[n], XmNforeground, CommonGetColor("white")); n++;
        FontNameTF = XmCreateTextField( ResourcePanel, "FontNameTF",
				args, n ); 
        XtManageChild(FontNameTF);
        
       /*
        *  XmNfontType
        */
        n = 0;
        FontTypePullDown = XmCreatePulldownMenu( ResourcePanel,
                             "FontTypePullDown", args, n );

        for ( i = 0; i < NUM_FONT_TYPES; i++ )
        {
           string = XmStringCreateLtoR( font_types[i],
                      XmFONTLIST_DEFAULT_TAG );
           sprintf( buffer, "FontTypePB%d", i );
           n = 0;
           XtSetArg( args[n], XmNlabelString, string ); n++;
           XtSetArg( args[n], XmNforeground, CommonGetColor( "white" )); n++;
           FontTypePB[i] = XmCreatePushButton( FontTypePullDown, buffer,
                             args, n );
           XmStringFree( string );
        }
        XtManageChildren( FontTypePB, NUM_FONT_TYPES );

        n = 0;
        XtSetArg(args[n], XmNsubMenuId,   FontTypePullDown);   n++;
        XtSetArg(args[n], XmNmenuHistory, FontTypePB[0]);    n++;
        XtSetArg(args[n], XmNforeground, CommonGetColor("white"));    n++;
        FontTypeOptions = XmCreateOptionMenu( ResourcePanel, "FontTypeOptions",
                            args, n );
        XtManageChild( FontTypeOptions );

        n = 0;
        LoadModelPullDown = XmCreatePulldownMenu( ResourcePanel,
                             "LoadModelPullDown", args, n );

        for ( i = 0; i < NUM_LOAD_MODELS; i++ )
        {
           string = XmStringCreateLtoR( load_models[i],
                      XmFONTLIST_DEFAULT_TAG );
           sprintf( buffer, "LoadModelPB%d", i );
           n = 0;
           XtSetArg( args[n], XmNforeground, CommonGetColor( "white" )); n++;
           XtSetArg( args[n], XmNlabelString, string ); n++;
           LoadModelPB[i] = XmCreatePushButton( LoadModelPullDown, buffer,
                             args, n );
           XmStringFree( string );
        }
        XtManageChildren( LoadModelPB, NUM_LOAD_MODELS );

        n = 0;
        XtSetArg(args[n], XmNsubMenuId,   LoadModelPullDown);   n++;
        XtSetArg(args[n], XmNmenuHistory, LoadModelPB[0]);    n++;
        XtSetArg(args[n], XmNforeground, CommonGetColor("white"));    n++;
        LoadModelOptions = XmCreateOptionMenu( ResourcePanel, 
                             "LoadModelOptions", args, n );
        XtManageChild( LoadModelOptions );

       /*
        *  XmNfont
        */
        n = 0;
        XtSetArg( args[n], XmNforeground, CommonGetColor("white")); n++;
        FontTF = XmCreateTextField( ResourcePanel, "FontTF",
				args, n ); 
        XtManageChild( FontTF );
        
       /*
        *  XmNtabList
        */
        n = 0;
        XtSetArg( args[n], XmNeditable, False); n++;
        XtSetArg( args[n], XmNforeground, CommonGetColor("white")); n++;
        TabListTF = XmCreateTextField( ResourcePanel, "TabListTF",
				args, n ); 
        XtManageChild( TabListTF );
                                           
       /*
        *  XmNrenditionBackground
        */
        n = 0;
        XtSetArg( args[n], XmNforeground, CommonGetColor("white")); n++;
        BackgroundTF = XmCreateTextField( ResourcePanel, "BackgroundTF",
				args, n ); 
        XtManageChild( BackgroundTF );

       /*
        *  XmNrenditionForeground
        */
        n = 0;
        XtSetArg( args[n], XmNforeground, CommonGetColor("white")); n++;
        ForegroundTF = XmCreateTextField( ResourcePanel, "ForegroundTF",
				args, n ); 
        XtManageChild( ForegroundTF );

        n = 0;
        UnderlinePullDown = XmCreatePulldownMenu( ResourcePanel,
                             "UnderlinePullDown", args, n );

        for ( i = 0; i < NUM_LINE_TYPES; i++ )
        {
           string = XmStringCreateLtoR( line_types[i],
                      XmFONTLIST_DEFAULT_TAG );
           sprintf( buffer, "UnderlinePB%d", i );
           n = 0;
           XtSetArg( args[n], XmNforeground, CommonGetColor( "white" )); n++;
           XtSetArg( args[n], XmNlabelString, string ); n++;
           UnderlinePB[i] = XmCreatePushButton( UnderlinePullDown, buffer,
                             args, n );
           XmStringFree( string );
        }
        XtManageChildren( UnderlinePB, NUM_LINE_TYPES );

        n = 0;
        XtSetArg(args[n], XmNsubMenuId,   UnderlinePullDown);   n++;
        XtSetArg(args[n], XmNmenuHistory, UnderlinePB[0]);    n++;
        XtSetArg(args[n], XmNforeground, CommonGetColor("white"));    n++;
        UnderlineOptions = XmCreateOptionMenu( ResourcePanel,
                             "UnderlineOptions", args, n );
        XtManageChild( UnderlineOptions );

        n = 0;
        StrikethruPullDown = XmCreatePulldownMenu( ResourcePanel,
                             "StrikethruPullDown", args, n );

        for ( i = 0; i < NUM_LINE_TYPES; i++ )
        {
           string = XmStringCreateLtoR( line_types[i],
                      XmFONTLIST_DEFAULT_TAG );
           sprintf( buffer, "StrikethruPB%d", i );
           n = 0;
           XtSetArg( args[n], XmNforeground, CommonGetColor( "white" )); n++;
           XtSetArg( args[n], XmNlabelString, string ); n++;
           StrikethruPB[i] = XmCreatePushButton( StrikethruPullDown, buffer,
                             args, n );
           XmStringFree( string );
        }
        XtManageChildren( StrikethruPB, NUM_LINE_TYPES );

        n = 0;
        XtSetArg(args[n], XmNsubMenuId,   StrikethruPullDown);   n++;
        XtSetArg(args[n], XmNmenuHistory, StrikethruPB[0]);    n++;
        XtSetArg(args[n], XmNforeground, CommonGetColor("white"));    n++;
        StrikethruOptions = XmCreateOptionMenu( ResourcePanel,
                             "StrikethruOptions", args, n );
        XtManageChild( StrikethruOptions );

    /*
     *  Get the current rendition resource values.
     */
     GetRendResources( render_tag );

    /* 
     *  Display resources values in rendition resource panel.
     */
     DisplayRendResources( render_tag );

   return( ResourcePanel );
}

static void
MergePanel( Widget widget, XtPointer client_data, XtPointer call_data )
{
   Arg      args[25];
   Boolean  selection;
   int      n, *position_list, position_count;
   XmStringTag renderTag;
   Dimension button_height = 50;
   Dimension button_width = RESOURCE_PANEL_WIDTH / 3;
   Position x_coord, y_coord;
   XmString string, *selectedItems;
   Widget   ResourcePanel, ApplyPB, CancelPB, QuitPB;

    /*
     *  Determine which rendition tag was selected from the List
     *  in the Render Tag Panel.
     */  
     n = 0;
     XtSetArg( args[n], XmNselectedItems, &selectedItems ); n++;
     XtGetValues( RenderTagList, args, n );

     for ( n = 0; n < NUM_RENDITIONS; n++ )
         if ( XmStringCompare( listItems[n], selectedItems[0] ))
            break;

     renderTag = tag_list[n];   

    /*  
     *  Once Merge pushbutton is selected from the Render Tag Panel,
     *  set the other pushbuttons to be insensitive as well as the
     *  RenderTagList.
     */
     XtSetSensitive( ModifyPB, False );
     XtSetSensitive( RemovePB, False );
     XtSetSensitive( RenderTagList, False );

    /*
     *  Create panel for users to merge new resource values to
     *  an existing rendition.
     */
     n = 0;
     XtSetArg( args[n], XmNx, 0 ); n++; 
     XtSetArg( args[n], XmNy, DISPLAY_HEIGHT ); n++; 
     XtSetArg( args[n], XmNwidth, RESOURCE_PANEL_WIDTH ); n++; 
     XtSetArg( args[n], XmNheight, RESOURCE_PANEL_HEIGHT + 2 * button_height );
     n++;
     MergePopup = XtCreatePopupShell("Merge Panel",
                                      topLevelShellWidgetClass,
                                      Shell1, args, n );
     XtManageChild( MergePopup );

     n = 0;
     MergeForm = XmCreateForm( MergePopup, "MergeForm", args, n ); 
     XtManageChild( MergeForm );

    /*
     *  Create option menu containing the merge options.
     */ 
     MergeOptions = CreateMergeOptions( MergeForm,
                       RESOURCE_PANEL_WIDTH, button_height );
     XtManageChild(MergeOptions);

     y_coord += button_height + 1;
     x_coord = 0;

    /*
     *  Create editable panel of rendition resources.
     */
     ResourcePanel = CreateResourcePanel( renderTag, MergeForm, x_coord,
                                          y_coord, MergeOptions );
     XtManageChild( ResourcePanel );


    /* 
     *  Create pushbuttons to be displayed below the resource values. 
     */
     string = XmStringCreateLtoR("Apply", XmFONTLIST_DEFAULT_TAG );

     n = 0;
     XtSetArg( args[n], XmNtopAttachment, XmATTACH_WIDGET ); n++;
     XtSetArg( args[n], XmNtopWidget, ResourcePanel ); n++;
     XtSetArg( args[n], XmNleftAttachment, XmATTACH_FORM ); n++;
     XtSetArg( args[n], XmNwidth, button_width ); n++;
     XtSetArg( args[n], XmNheight, button_height ); n++;
     XtSetArg( args[n], XmNlabelString, string ); n++;
     ApplyPB = XmCreatePushButton( MergeForm, "ApplyPB", args, n ); 
     XtManageChild( ApplyPB );
     XmStringFree( string );

     XtAddCallback( ApplyPB, XmNactivateCallback, MergeApply, 
                    (XtPointer)renderTag );

     x_coord += button_width;

     string = XmStringCreateLtoR("Cancel", XmFONTLIST_DEFAULT_TAG );

     n = 0;
     XtSetArg( args[n], XmNtopAttachment, XmATTACH_WIDGET ); n++;
     XtSetArg( args[n], XmNtopWidget, ResourcePanel ); n++;
     XtSetArg( args[n], XmNleftAttachment, XmATTACH_WIDGET ); n++;
     XtSetArg( args[n], XmNleftWidget, ApplyPB ); n++;
     XtSetArg( args[n], XmNwidth, button_width ); n++;
     XtSetArg( args[n], XmNheight, button_height ); n++;
     XtSetArg( args[n], XmNlabelString, string ); n++;
     CancelPB = XmCreatePushButton( MergeForm, "CancelPB", args, n );
     XtManageChild( CancelPB );
     XmStringFree( string );

     XtAddCallback( CancelPB, XmNactivateCallback, MergeCancel, 
                    (XtPointer)renderTag );
                    

     x_coord += button_width;

     string = XmStringCreateLtoR("Quit", XmFONTLIST_DEFAULT_TAG );

     n = 0;
     XtSetArg( args[n], XmNtopAttachment, XmATTACH_WIDGET ); n++;
     XtSetArg( args[n], XmNtopWidget, ResourcePanel ); n++;
     XtSetArg( args[n], XmNleftAttachment, XmATTACH_WIDGET ); n++;
     XtSetArg( args[n], XmNleftWidget, CancelPB ); n++;
     XtSetArg( args[n], XmNwidth, button_width ); n++;
     XtSetArg( args[n], XmNheight, button_height ); n++;
     XtSetArg( args[n], XmNlabelString, string ); n++;
     QuitPB = XmCreatePushButton( MergeForm, "QuitPB", args, n );
     XtManageChild( QuitPB );
     XmStringFree( string );

     XtAddCallback( QuitPB, XmNactivateCallback, MergeQuit, 
                    (XtPointer)renderTag );
                    

     XtPopup( MergePopup, XtGrabNone );
}
  
static void
ModifyPanel( Widget widget, XtPointer client_data, XtPointer call_data )
{
   Arg      args[25];
   Boolean  selection;
   XmStringTag renderTag;
   int      n, *position_list, position_count;
   Dimension button_height = 50;
   Dimension button_width = RESOURCE_PANEL_WIDTH / 3;
   Position x_coord, y_coord;
   XmString string, *selectedItems;
   Widget   ResourcePanel, ApplyPB, CancelPB, QuitPB;

    /*
     *  Determine which rendition tag was selected from the List
     *  in the Render Tag Panel.
     */
     n = 0;
     XtSetArg( args[n], XmNselectedItems, &selectedItems ); n++;
     XtGetValues( RenderTagList, args, n );

     for ( n = 0; n < NUM_RENDITIONS; n++ )
         if ( XmStringCompare( listItems[n], selectedItems[0] ))
            break;

     renderTag = tag_list[n];

    /*
     *  Once Modify pushbutton is selected from the Render Tag Panel,
     *  set the other pushbuttons to be insensitive as well as the
     *  RenderTagList.
     */
     XtSetSensitive( MergePB, False );
     XtSetSensitive( RemovePB, False );
     XtSetSensitive( RenderTagList, False );

    /*
     *  Create panel for users to modify resource values in
     *  an existing rendition.
     */
     n = 0;
     XtSetArg( args[n], XmNx, 0 ); n++; 
     XtSetArg( args[n], XmNy, DISPLAY_HEIGHT ); n++; 
     XtSetArg( args[n], XmNwidth, RESOURCE_PANEL_WIDTH ); n++;
     XtSetArg( args[n], XmNheight, RESOURCE_PANEL_HEIGHT + button_height );
     n++;
     ModifyPopup = XtCreatePopupShell("Modify Panel",
                                      topLevelShellWidgetClass,
                                      Shell1, args, n );
     XtManageChild( ModifyPopup );

     n = 0;
     ModifyForm = XmCreateForm( ModifyPopup, "ModifyForm", args, n );
     XtManageChild( ModifyForm );

     x_coord = y_coord = 0;

    /*
     *  Create editable panel of rendition resources.
     */
     ResourcePanel = CreateResourcePanel( renderTag, ModifyForm, x_coord,
                                          y_coord, (Widget)0 );
     XtManageChild( ResourcePanel );

    /*
     *  Create pushbuttons to be displayed below the resource values.
     */
     string = XmStringCreateLtoR("Apply", XmFONTLIST_DEFAULT_TAG );

     n = 0;
     XtSetArg( args[n], XmNtopAttachment, XmATTACH_WIDGET ); n++;
     XtSetArg( args[n], XmNtopWidget, ResourcePanel ); n++;
     XtSetArg( args[n], XmNleftAttachment, XmATTACH_FORM ); n++;
     XtSetArg( args[n], XmNwidth, button_width ); n++;
     XtSetArg( args[n], XmNheight, button_height ); n++;
     XtSetArg( args[n], XmNlabelString, string ); n++;
     ApplyPB = XmCreatePushButton( ModifyForm, "ApplyPB", args, n );
     XtManageChild( ApplyPB );
     XmStringFree( string );

     XtAddCallback( ApplyPB, XmNactivateCallback, ModifyApply,
                    (XtPointer)renderTag );

     x_coord += button_width;

     string = XmStringCreateLtoR("Cancel", XmFONTLIST_DEFAULT_TAG );

     n = 0;
     XtSetArg( args[n], XmNtopAttachment, XmATTACH_WIDGET ); n++;
     XtSetArg( args[n], XmNtopWidget, ResourcePanel ); n++;
     XtSetArg( args[n], XmNleftAttachment, XmATTACH_WIDGET ); n++;
     XtSetArg( args[n], XmNleftWidget, ApplyPB ); n++;
     XtSetArg( args[n], XmNwidth, button_width ); n++;
     XtSetArg( args[n], XmNheight, button_height ); n++;
     XtSetArg( args[n], XmNlabelString, string ); n++;
     CancelPB = XmCreatePushButton( ModifyForm, "CancelPB", args, n );
     XtManageChild( CancelPB );
     XmStringFree( string );

     XtAddCallback( CancelPB, XmNactivateCallback, ModifyCancel,
                    (XtPointer)renderTag );

     x_coord += button_width;

     string = XmStringCreateLtoR("Quit", XmFONTLIST_DEFAULT_TAG );

     n = 0;
     XtSetArg( args[n], XmNtopAttachment, XmATTACH_WIDGET ); n++;
     XtSetArg( args[n], XmNtopWidget, ResourcePanel ); n++;
     XtSetArg( args[n], XmNleftAttachment, XmATTACH_WIDGET ); n++;
     XtSetArg( args[n], XmNleftWidget, CancelPB ); n++;
     XtSetArg( args[n], XmNwidth, button_width ); n++;
     XtSetArg( args[n], XmNheight, button_height ); n++;
     XtSetArg( args[n], XmNlabelString, string ); n++;
     QuitPB = XmCreatePushButton( ModifyForm, "QuitPB", args, n );
     XtManageChild( QuitPB );
     XmStringFree( string );

     XtAddCallback( QuitPB, XmNactivateCallback, ModifyQuit, 
                    (XtPointer)renderTag );

     XtPopup( ModifyPopup, XtGrabNone );
}

static void
Remove( Widget widget, XtPointer client_data, XtPointer call_data )
{
   Boolean     selection;
   XmStringTag renderTag;
   int         n;
   Arg         args[10];
   XmString    *selectedItems;

    /*
     *  Determine which rendition tag was selected from the List
     *  in the Render Tag Panel.
     */
     n = 0;
     XtSetArg( args[n], XmNselectedItems, &selectedItems ); n++;
     XtGetValues( RenderTagList, args, n );

     for ( n = 0; n < NUM_RENDITIONS; n++ )
         if ( XmStringCompare( listItems[n], selectedItems[0] ))
            break;

     renderTag = tag_list[n];

    /*
     *  Remove the selected rendition from the render table.
     */
     renderTable =
     XmRenderTableRemoveRenditions( renderTable, &renderTag, 1 );

    /*
     *  Remove the selected item from the render tag list.
     */
     XmListDeleteItem( RenderTagList, selectedItems[0] ); 

    /* Redisplay XmString. */
     DisplayStrings();
   
   return;
}

static void
DisplayRenderTagPanel()
{
    Arg         args[20];
    int         n, i;
    Widget      RenderTagPopup, Form;
    Dimension   list_height, button_height, button_width;
    Position    x_coord, y_coord;
    int         num_tags;
    XmStringTag *tag_list;
    XmString    string;

       n = 0;
       XtSetArg( args[n], XmNx, DISPLAY_WIDTH + 15 ); n++;
       XtSetArg( args[n], XmNy, 0 ); n++;
       XtSetArg( args[n], XmNwidth, RENDER_PANEL_WIDTH ); n++;
       XtSetArg( args[n], XmNheight, RENDER_PANEL_HEIGHT ); n++;
       RenderTagPopup = XtCreatePopupShell("Renditions",
                                       topLevelShellWidgetClass,
                                       Shell1, args, n );
       XtManageChild(RenderTagPopup);

       n = 0;
       XtSetArg( args[n], XmNwidth, RENDER_PANEL_WIDTH ); n++;
       XtSetArg( args[n], XmNheight, RENDER_PANEL_HEIGHT ); n++;
       Form = XmCreateForm( RenderTagPopup, "Form", args, n );
       XtManageChild(Form);

      /*
       *  The form will contain a list and three
       *  pushbuttons. Calculate the dimensions alloted to
       *  each of these widgets.
       */
       button_height = RENDER_PANEL_HEIGHT / 4;
       button_width = RENDER_PANEL_WIDTH / 3; 
       list_height = RENDER_PANEL_HEIGHT - button_height;

       x_coord = y_coord = DISPLAY_MARGIN;

      /*  
       *  Create List widget which displays the render tags
       *  of the current renditions in the render table.
       */
       n = 0;
       XtSetArg( args[n], XmNx, x_coord ); n++; 
       XtSetArg( args[n], XmNy, y_coord ); n++; 
       XtSetArg( args[n], XmNwidth, RENDER_PANEL_WIDTH ); n++; 
       XtSetArg( args[n], XmNheight, RENDER_PANEL_HEIGHT ); n++; 
       XtSetArg( args[n], XmNtopAttachment, XmATTACH_FORM ); n++; 
       XtSetArg( args[n], XmNleftAttachment, XmATTACH_FORM ); n++; 
       XtSetArg( args[n], XmNrightAttachment, XmATTACH_FORM ); n++; 
       XtSetArg( args[n], XmNselectionPolicy, XmSINGLE_SELECT ); n++; 
       XtSetArg( args[n], XmNvisibleItemCount, 5 ); n++; 
       RenderTagList = XmCreateList( Form, "RendList", args, n );
       XtManageChild(RenderTagList);

      /*
       *  Update rendition list with the current render tags
       *  in the render table.
       */
       num_tags = XmRenderTableGetTags( renderTable, &tag_list );
       listItems = (XmStringTable)XtMalloc( num_tags * sizeof( XmString ));
       for ( i = 0; i < num_tags; i++ )
         listItems[i] = XmStringCreateLtoR( tag_list[i],
                          XmFONTLIST_DEFAULT_TAG ); 
       n = 0;
       XtSetArg( args[n], XmNitemCount, num_tags ); n++;
       XtSetArg( args[n], XmNitems, listItems ); n++;
       XtSetValues( RenderTagList, args, n );
       
       string = XmStringCreateLtoR("Merge", XmFONTLIST_DEFAULT_TAG );

       y_coord = list_height + 1;

       n = 0;
       XtSetArg( args[n], XmNx, x_coord ); n++; 
       XtSetArg( args[n], XmNy, list_height + 1 ); n++; 
       XtSetArg( args[n], XmNwidth, button_width ); n++; 
       XtSetArg( args[n], XmNheight, button_height ); n++; 
       XtSetArg( args[n], XmNleftAttachment, XmATTACH_FORM ); n++; 
       XtSetArg( args[n], XmNtopAttachment, XmATTACH_WIDGET ); n++; 
       XtSetArg( args[n], XmNtopWidget, RenderTagList ); n++; 
       XtSetArg( args[n], XmNlabelString, string ); n++; 
       MergePB = XmCreatePushButton( Form, "MergePB", args, n );
       XtManageChild(MergePB);

       XmStringFree( string );

       XtAddCallback( MergePB, XmNactivateCallback, MergePanel, (XtPointer)0 );

       x_coord += button_width;

       string = XmStringCreateLtoR("Modify", XmFONTLIST_DEFAULT_TAG );

       n = 0;
       XtSetArg( args[n], XmNx, x_coord ); n++;
       XtSetArg( args[n], XmNy, list_height + 1 ); n++;
       XtSetArg( args[n], XmNwidth, button_width ); n++;
       XtSetArg( args[n], XmNheight, button_height ); n++;
       XtSetArg( args[n], XmNleftAttachment, XmATTACH_WIDGET ); n++; 
       XtSetArg( args[n], XmNleftWidget, MergePB ); n++; 
       XtSetArg( args[n], XmNtopAttachment, XmATTACH_WIDGET ); n++; 
       XtSetArg( args[n], XmNtopWidget, RenderTagList ); n++; 
       XtSetArg( args[n], XmNlabelString, string ); n++;
       ModifyPB = XmCreatePushButton( Form, "ModifyPB", args, n );
       XtManageChild(ModifyPB);

       XmStringFree( string );

       XtAddCallback( ModifyPB, XmNactivateCallback, ModifyPanel, 
                      (XtPointer)0 ); 

       x_coord += button_width;

       string = XmStringCreateLtoR("Remove", XmFONTLIST_DEFAULT_TAG );

       n = 0;
       XtSetArg( args[n], XmNx, x_coord ); n++;
       XtSetArg( args[n], XmNy, list_height + 1 ); n++;
       XtSetArg( args[n], XmNwidth, button_width ); n++;
       XtSetArg( args[n], XmNheight, button_height ); n++;
       XtSetArg( args[n], XmNleftAttachment, XmATTACH_WIDGET ); n++; 
       XtSetArg( args[n], XmNleftWidget, ModifyPB ); n++; 
       XtSetArg( args[n], XmNtopAttachment, XmATTACH_WIDGET ); n++; 
       XtSetArg( args[n], XmNtopWidget, RenderTagList ); n++; 
       XtSetArg( args[n], XmNlabelString, string ); n++;
       RemovePB = XmCreatePushButton( Form, "RemovePB", args, n );
       XtManageChild(RemovePB);

       XtAddCallback( RemovePB, XmNactivateCallback, Remove, (XtPointer)0 ); 

       XmStringFree( string );

       XtPopup( RenderTagPopup, XtGrabNone ); 
}



/*
 *  Main
 */
void main( int argc, char **argv )
{
   Arg           args[25];
   XmRendition   renditions[NUM_RENDITIONS];
   int           num_tags, n;
   Widget        bulletinBoard;
   int           i;


   CommonTestInit( argc, argv );

   n = 0;
   XtSetArg( args[n], XmNtitle, "Display Strings" ); n++;
   XtSetValues( Shell1, args, n );
   XtRealizeWidget(Shell1);

   n = 0;
   XtSetArg( args[n], XmNwidth, DISPLAY_WIDTH ); n++; 
   XtSetArg( args[n], XmNheight, DISPLAY_HEIGHT ); n++; 
   bulletinBoard = 
     XmCreateBulletinBoard( Shell1, "bulletinBoard", args, n );
   XtManageChild( bulletinBoard );


   CreateRenditions( renditions ); 

   num_tags = XmRenderTableGetTags( renderTable, &tag_list );

   finalString = CreateStrings( tag_list ); 


  /*  Create Drawing Area. */
   n = 0;
   XtSetArg( args[n], XmNx, 0 ); n++; 
   XtSetArg( args[n], XmNy, 0 ); n++; 
   XtSetArg( args[n], XmNwidth, DISPLAY_WIDTH ); n++; 
   XtSetArg( args[n], XmNheight, DISPLAY_HEIGHT ); n++; 
   XtSetArg( args[n], XmNforeground, CommonGetColor("red")); n++; 
   XtSetArg( args[n], XmNbackground, CommonGetColor("red")); n++; 
   drawingArea = XmCreateDrawingArea( bulletinBoard, "DrawArea", args, n );
   XtManageChild( drawingArea );
 
  /*
   *  Draw blank drawing area prior to displaying strings.
   *  Need to have an expose event in order for strings to
   *  be displayed.
   */
   CommonPause();

   n = 0;
   XtSetArg(args[n], XmNforeground, &gcv.foreground); n++;
   XtSetArg(args[n], XmNbackground, &gcv.background); n++;
   XtGetValues(drawingArea, args, n);

  /*  Create Graphics Context. */
   window = XtWindow( drawingArea );
   fontStruct = (XFontStruct *)XLoadQueryFont( display, "fixed" );
   gcv.font = fontStruct->fid;
   gc = XCreateGC( display, window,
                   GCForeground | GCBackground | GCFont | GCLineStyle, &gcv );

  /*  Display XmString. */
   DisplayStrings();

  /*  List display attributes of each of the XmStrings. */
   CommonPause();
   CommonPause();
   CommonPause();
   CommonPause();

   DisplayRenderTagPanel();

   CommonPause();
   CommonPause();
   CommonPause();
   CommonPause();
   CommonPause();
   CommonPause();

   CommonPause();
   XtAppMainLoop( app_context );

   XtFree( (char *)tag_list );
   XmStringFree( finalString );
}
