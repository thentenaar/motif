/* $XConsortium: StringParse.c /main/6 1996/06/11 17:23:29 pascale $ */
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
#define NUM_MAPPINGS 2
#define DRAW_AREA_WIDTH 200
#define DRAW_AREA_HEIGHT 200
#define MARGIN 20
#define NUM_STRINGS 4

char *new_line_char = "/";
char *tab_char = ">";
char *text_end1 = NULL;

static char single_string[] =
"One./Two/>Tab";

char *ltor_char = ">";
char *rtol_char = "<";

static char *multi_strings[] =
{ ">Left To Right",
  "<tfeL oT thgiR",
  ">Left To Right",
  "<tfeL oT thgiR", };

static char *text_table[] =
{ "Orange",
  "Yellow",
  "Green",
  "Blue" };

/* Global variables used to draw the window. */
XmString	  output[10];
Cardinal	  num_output = 0;
GC                gc;
XmRenderTable     renderTable;

void 
expose (Widget widget,
	XtPointer closure,
	XtPointer call_data)
{
  Cardinal i;
  Dimension width, height;
  Position  x, y;

  x = y = MARGIN;
  for (i = 0; i < num_output; i++)
    {
      XmStringExtent(renderTable, output[i], &width, &height);
      XmStringDraw(XtDisplay(widget), XtWindow(widget), renderTable, 
		   output[i], gc, x, y, width, XmALIGNMENT_BEGINNING,
		   XmSTRING_DIRECTION_L_TO_R, NULL);
      y += height + MARGIN;
    }
}

XmIncludeStatus
parseProc( XtPointer *text_in_out, XtPointer text_end, XmTextType type, 
           XmStringTag tag, XmParseMapping entry, int pattern_length,
           XmString *str_include, XtPointer call_data )
{
   XmIncludeStatus  *include_status;
   XmString         text;
   Arg	            args[20];
   int	            n;
   char		    **string = (char **) text_in_out;

   text = XmStringGenerate(" String\n", XmFONTLIST_DEFAULT_TAG,
                           XmCHARSET_TEXT, (XmStringTag)0 );
   *str_include = text;

   n = 0;
   XtSetArg( args[n], XmNclientData, &include_status ); n++;
   XmParseMappingGetValues( entry, args, n );

   *string += pattern_length; 

   return( *include_status );
}

/*
 *  Main
 */
main( int argc, char **argv )
{
	Arg	          args[20];
	int	          i, n;
	XmParseMapping    parseTable[NUM_MAPPINGS];
	XmString	  temp, string;
	XmStringDirection direction, direction1, direction2;
	Widget            bboard, drawArea;
	Dimension	  width, height;
	Position	  x_coord, y_coord;
	Window		  window;
	XmFontType        type;
	XFontStruct       *fontStruct;
	XmFontListEntry   fontEntry1 = (XmFontListEntry)0;
    	XmFontList	  fontList1 = NULL;
        XGCValues         gcv;
	XmTab             tab;
	XmTabList	  tabList;
	XmRendition	  rendition;
	unsigned int	  position;
	XmIncludeStatus   status;
	char		  *unparse1, **unparse2, *text;
	XmStringTable     stringArray;
	XmString	  stringTable[NUM_STRINGS];
	Boolean           match;
        XmStringContext   context;
        XmStringComponentType comp_type, unknown_tag;
	XmStringCharSet   char_set = XmFONTLIST_DEFAULT_TAG;
  	unsigned short    unknown_length;
	unsigned char	  *unknown_value;


     CommonTestInit( argc, argv );

     n = 0;
     bboard = XmCreateBulletinBoard( Shell1, "bboard", args, n );
     XtManageChild( bboard );

     n = 0;
     XtSetArg( args[n], XmNx, 0 ); n++;
     XtSetArg( args[n], XmNy, 0 ); n++;
     XtSetArg( args[n], XmNwidth, DRAW_AREA_WIDTH ); n++;
     XtSetArg( args[n], XmNheight, DRAW_AREA_HEIGHT ); n++;
     drawArea = XmCreateDrawingArea( bboard, "DrawArea", args, n );
     XtAddCallback(drawArea, XmNexposeCallback, expose, NULL);
     XtManageChild( drawArea );

     XtRealizeWidget( Shell1 );


    /*  Display empty drawing area widget. */
     CommonPause();

     window = XtWindow( drawArea );

     fontEntry1 = XmFontListEntryLoad( display, "fixed", XmFONT_IS_FONT,
                               XmFONTLIST_DEFAULT_TAG );
     fontList1 = XmFontListAppendEntry( NULL, fontEntry1 );

  
     n = 0;
     XtSetArg(args[n], XmNforeground, &gcv.foreground); n++;
     XtSetArg(args[n], XmNbackground, &gcv.background); n++;
     XtGetValues(drawArea, args, n);

     fontStruct = (XFontStruct *)XmFontListEntryGetFont(fontEntry1, &type);
     gcv.font = fontStruct->fid;

    /*  Create Graphics Context. */
     gc = XCreateGC( display, window,
                     GCFont | GCForeground | GCBackground, &gcv );


    /*
     *  Create parse mapping to convert new_line_char to a separator component.
     */
     string = XmStringComponentCreate( XmSTRING_COMPONENT_SEPARATOR,
                0, (XtPointer)0 );

     i = 0;
     n = 0;
     XtSetArg( args[n], XmNincludeStatus, XmTERMINATE ); n++;
     XtSetArg( args[n], XmNpattern, (XtPointer)new_line_char ); n++;
     XtSetArg( args[n], XmNpatternType, XmCHARSET_TEXT ); n++;
     XtSetArg( args[n], XmNsubstitute, string  ); n++;
     parseTable[i++] = XmParseMappingCreate( args, n );
     XmStringFree( string );

    /*
     *  Create parse mapping to convert tab_char to a tab component.
     */
     string = XmStringComponentCreate( XmSTRING_COMPONENT_TAB,
                0, (XtPointer)0 );

     n = 0;
     XtSetArg( args[n], XmNincludeStatus, XmTERMINATE ); n++;
     XtSetArg( args[n], XmNpattern, (XtPointer)tab_char ); n++;
     XtSetArg( args[n], XmNpatternType, XmCHARSET_TEXT ); n++;
     XtSetArg( args[n], XmNsubstitute, string  ); n++;
     parseTable[i++] = XmParseMappingCreate( args, n );
     XmStringFree( string );

    /*
     *  Create rendition table which contains a tabList
     *  for displaying those parsed strings which contain
     *  a tab.
     */
     tab = XmTabCreate( 1.0, XmINCHES, XmABSOLUTE, XmALIGNMENT_BEGINNING,
                        (char *)0 );

     position = 0;
     tabList = XmTabListInsertTabs( (XmTabList)0, &tab, 1, 0 );
     
     XmTabFree( tab );

     n = 0;
     XtSetArg( args[n], XmNtabList, tabList ); n++;
     XtSetArg( args[n], XmNforeground, CommonGetColor("white")); n++;
     XtSetArg( args[n], XmNfontName, "fixed" ); n++;
     XtSetArg( args[n], XmNfontType, XmFONT_IS_FONT ); n++;
     XtSetArg( args[n], XmNloadModel, XmLOAD_IMMEDIATE ); n++;
     rendition = XmRenditionCreate( Shell1, "render_tag", args, n );

     XmTabListFree( tabList );
 
     renderTable= XmRenderTableAddRenditions( (XmRenderTable)0, &rendition,
                    1, XmMERGE_REPLACE );

     XmRenditionFree( rendition );


    /*  String is terminated after the first pattern is found. */ 
     string =
     XmStringParseText( (XtPointer)single_string, (XtPointer *)0, 
                        (XmStringTag)"render_tag", XmCHARSET_TEXT, parseTable,
                        NUM_MAPPINGS, (XtPointer)0 ); 

    /*  Display the string. */
     output[num_output++] = XmStringCopy(string);
     XmStringFree( string );

     XClearArea(XtDisplay(drawArea), XtWindow(drawArea), 0, 0, 0, 0, TRUE);
     CommonPause();

    /*
     *  Clear the drawing area.
     */
     while (num_output)
       XmStringFree(output[--num_output]);

    /*
     *  Reset includeStatus to be XmINSERT.
     */
     for ( i = 0; i < NUM_MAPPINGS; i++ )
     {
         n = 0;
         XtSetArg( args[n], XmNincludeStatus, XmINSERT ); n++;
         XmParseMappingSetValues( parseTable[i], args, n );
     }

    /*  Should parse string until delimitor is encountered. */
     text_end1 = &single_string[3];
     string =
     XmStringParseText( (XtPointer)single_string, (XtPointer *)&text_end1,
                        (XmStringTag)"render_tag", XmCHARSET_TEXT, parseTable,
                        NUM_MAPPINGS, (XtPointer)0 );

    /*  Display the string. */
     output[num_output++] = XmStringCopy(string);
     XmStringFree( string );

     XClearArea(XtDisplay(drawArea), XtWindow(drawArea), 0, 0, 0, 0, TRUE);
     CommonPause();

    /*
     *  Clear the drawing area.
     */
     while (num_output)
       XmStringFree(output[--num_output]);

    /*  Should parse string until the end. */
     string =
     XmStringParseText( (XtPointer)single_string, (XtPointer *)0,
                        (XmStringTag)"render_tag", XmCHARSET_TEXT, parseTable,
                        NUM_MAPPINGS, (XtPointer)0 );

    /*  Display the string. */
     output[num_output++] = XmStringCopy(string);

     XClearArea(XtDisplay(drawArea), XtWindow(drawArea), 0, 0, 0, 0, TRUE);
     CommonPause();

    /*
     *  Clear the drawing area.
     */
     while (num_output)
       XmStringFree(output[--num_output]);

    /*  Unparse the XmString to ensure the same character string is returned. */
     unparse1 = XmStringUnparse( string, (XmStringTag)"render_tag",
                                 XmCHARSET_TEXT,
                                 XmCHARSET_TEXT, parseTable, NUM_MAPPINGS,
                                 XmOUTPUT_ALL ); 
     XmStringFree( string );

     output[num_output++] = XmStringCreateLocalized("-- Original Text --");
     output[num_output++] = XmStringCreateLocalized(single_string);
     output[num_output++] = XmStringCreateLocalized("-- XmStringUnparse --");
     output[num_output++] = XmStringCreateLocalized(unparse1);

     printf("XmStringUnparse() ");
     if ( strcmp( unparse1, single_string ) == 0 )
          printf("succeeded."); 
     else
          printf("failed.");
     printf("\n\n");

     XtFree((char *)unparse1 );

     XClearArea(XtDisplay(drawArea), XtWindow(drawArea), 0, 0, 0, 0, TRUE);
     CommonPause();

    /*
     *  Clear the drawing area.
     */
     while (num_output)
       XmStringFree(output[--num_output]);

    /*
     *  Assign parse procedure to be called.
     *  Terminate parsing after first pattern is matched.
     */
     status = XmTERMINATE;

     n = 0;
     XtSetArg( args[n], XmNincludeStatus, XmINVOKE ); n++;
     XtSetArg( args[n], XmNinvokeParseProc, parseProc ); n++;
     XtSetArg( args[n], XmNclientData, &status ); n++;
     XmParseMappingSetValues( parseTable[0], args, n );

    /*  Should parse string until first pattern is matched. */
     string =
     XmStringParseText( (XtPointer)single_string, (XtPointer *)0,
                        (XmStringTag)"render_tag", XmCHARSET_TEXT, parseTable,
                        NUM_MAPPINGS, (XtPointer)0 );


    /*  Display the string. */
     output[num_output++] = XmStringCopy(string);
     XmStringFree( string );

     XClearArea(XtDisplay(drawArea), XtWindow(drawArea), 0, 0, 0, 0, TRUE);
     CommonPause();

    /*
     *  Clear the drawing area.
     */
     while (num_output)
       XmStringFree(output[--num_output]);


    /*  Reset client data to now be XmINSERT. */
     status = XmINSERT;
     n = 0;
     XtSetArg( args[n], XmNclientData, &status ); n++;
     XmParseMappingSetValues( parseTable[0], args, n );

    /*  Should parse string until the end. */
     string =
     XmStringParseText( (XtPointer)single_string, (XtPointer *)0,
                        (XmStringTag)"render_tag", XmCHARSET_TEXT, parseTable,
                        NUM_MAPPINGS, (XtPointer)0 );

    /*  Display the string. */
     output[num_output++] = XmStringCopy(string);

     XClearArea(XtDisplay(drawArea), XtWindow(drawArea), 0, 0, 0, 0, TRUE);
     CommonPause();

     for ( i = 0; i < NUM_MAPPINGS; i++ )
        XmParseMappingFree( parseTable[i] );

     while (num_output)
       XmStringFree(output[--num_output]);
     XmRenderTableFree( renderTable );

    /*
     *  Create new parseMappings for parsing the XmStringTable parsing
     *  functions.
     */
     direction1 = XmSTRING_DIRECTION_L_TO_R;

     string = XmStringComponentCreate( XmSTRING_COMPONENT_DIRECTION,
                sizeof(XmStringDirection), &direction1 );
 
     i = 0;
     n = 0;
     XtSetArg( args[n], XmNincludeStatus, XmINSERT ); n++;
     XtSetArg( args[n], XmNpattern, (XtPointer)ltor_char ); n++;
     XtSetArg( args[n], XmNpatternType, XmCHARSET_TEXT ); n++;
     XtSetArg( args[n], XmNsubstitute, string  ); n++;
     parseTable[i++] = XmParseMappingCreate( args, n );
     XmStringFree( string );

     direction2 = XmSTRING_DIRECTION_R_TO_L;

     string = XmStringComponentCreate( XmSTRING_COMPONENT_DIRECTION,
                sizeof(XmStringDirection), &direction2 );

     n = 0;
     XtSetArg( args[n], XmNincludeStatus, XmINSERT ); n++;
     XtSetArg( args[n], XmNpattern, (XtPointer)rtol_char ); n++;
     XtSetArg( args[n], XmNpatternType, XmCHARSET_TEXT ); n++;
     XtSetArg( args[n], XmNsubstitute, string  ); n++;
     parseTable[i++] = XmParseMappingCreate( args, n );
     XmStringFree( string );

     stringArray =
     XmStringTableParseStringArray( (XtPointer)multi_strings, NUM_STRINGS, 
                                    (XmStringTag)0, XmCHARSET_TEXT, parseTable,
                                     NUM_MAPPINGS, (XtPointer)0 ); 
             

    /*  Display each of the strings. */
     renderTable = fontList1;
     for ( i = 0; i < NUM_STRINGS; i++ )
       output[num_output++] = XmStringCopy(stringArray[i]);

    /*
     *  Unparse the XmStrings converting them back into an array
     *  of strings.
     */
     unparse2 = 
     (char **)XmStringTableUnparse( stringArray, NUM_STRINGS, (XmStringTag)0,
                           XmCHARSET_TEXT, XmCHARSET_TEXT, parseTable,
                           NUM_MAPPINGS, XmOUTPUT_ALL );

    /*
     *  Verify whether the unparsed character strings match
     *  the original strings.
     */ 
     match = True;
     for ( i = 0; i < NUM_STRINGS; i++ )
     {
         if ( strcmp( multi_strings[i], unparse2[i] ) != 0 )
            match = False;
     }
     printf("XmStringTableUnparse() ");
     if ( match )
        printf("succeeded");
     else
        printf("failed");
     printf("\n\n");

    /*
     *  Deallocate memory allocated by the XmStringTable parse/unparse
     *  functions.
     */
     for ( i = 0; i < NUM_STRINGS; i++ )
     {
         XmStringFree(stringArray[i]); 
         XtFree((char*)unparse2[i]);
     }
     XtFree((char *)stringArray );
     XtFree((char *)unparse2 );

     XClearArea(XtDisplay(drawArea), XtWindow(drawArea), 0, 0, 0, 0, TRUE);
     CommonPause();

    /*
     *  Clear the drawing area.
     */
     while (num_output)
       XmStringFree(output[--num_output]);

    /*
     *  Allocate an array of XmStrings using the text_table
     *  character strings for text components.
     */
     for ( i = 0; i < NUM_STRINGS; i++ )
         stringTable[i] = XmStringComponentCreate( XmSTRING_COMPONENT_TEXT,
              strlen( text_table[i] ), text_table[i] );

     /*
      *  Create a separator component to be the break component between
      *  each of the original XmStrings.
      */
      temp = XmStringComponentCreate( XmSTRING_COMPONENT_SEPARATOR,
              0, (XtPointer)0 );

     /*
      *  Convert the XmStringTable into one XmString inserting
      *  a separator component between each pair of text components.
      */
      string = XmStringTableToXmString( stringTable, NUM_STRINGS, temp );

     /*
      *  Traverse the newly created string to ensure that each text
      *  component and separator are included in the string.
      */
      match = True;
      XmStringInitContext( &context, string );
      for ( i = 0; i < NUM_STRINGS; i++ )
      {
          comp_type = 
          XmStringGetNextComponent( context, &text, &char_set,
            &direction, &unknown_tag, &unknown_length, &unknown_value ); 
 
         /*  
          *  Verify that the first component in each pair is a text
          *  component and that the text itself matches the original text.
          */ 
          if ( comp_type == XmSTRING_COMPONENT_TEXT )
	    {
	      if ( strcmp( text, text_table[i] ) != 0 )
		match = False;
	    }
          else
             match = False;

          if ( text )
	    {
	      XtFree( text );
	      text = NULL;
	    }

         /*
          *  Verify that the second component in each pair is
          *  a separator component.
          */
          if ( i < NUM_STRINGS - 1 )
          {
             comp_type = 
             XmStringGetNextComponent( context, &text, &char_set,
               &direction, &unknown_tag, &unknown_length, &unknown_value ); 

             if ( comp_type != XmSTRING_COMPONENT_SEPARATOR )
                match = False;
          }

          if ( text )
	    {
	      XtFree( text );
	      text = NULL;
	    }
      }

    /*
     *  Report on the status of XmStringTableToXmString().
     */
     printf("XmStringTableToXmString() ");
     if ( match )
        printf("succeeded.");
     else
        printf("failed.");
     printf("\n\n");

    /*
     *  Call XmStringToXmStringTable to convert the one XmString
     *  back into a table of XmStrings.
     */
     XmStringToXmStringTable( string, temp, &stringArray );
     XmStringFree( temp );

    /*
     *  Verify that the XmStrings returned in the XmStringTable
     *  match the original XmStrings.
     */
     match = True;
     for ( i = 0; i < NUM_STRINGS; i++ )
     {
       if ( match )
	 {
	   if ( ! XmStringCompare( stringTable[i], stringArray[i] ))
	     match = False;  
	 }
       else
           break;
     }

    /*
     *  Report on the status of XmStringToXmStringTable(). 
     */
     printf("XmStringToXmStringTable() ");
     if ( match )
        printf("succeeded.");
     else
        printf("failed.");

     printf("\n\n");

     for ( i = 0; i < NUM_STRINGS; i++ )
        XmStringFree( stringArray[i] );

     XmStringFreeContext( context );

     CommonPause();
     CommonPause();

     if ( fontEntry1 )
        XmFontListEntryFree( fontEntry1 );
     if ( fontList1 )
        XmFontListFree( fontList1 );
     XtAppMainLoop( app_context );
}


