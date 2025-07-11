/* $XConsortium: StringNext.c /main/6 1996/06/11 17:23:20 pascale $ */
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
 *  Type definitions
 */
typedef struct _Components {
	XmStringComponentType	type;
	unsigned int		length;
	XtPointer		value;
} Components;

typedef struct _Segments {
	char 			*text;
	char			*tag;
	XmTextType              type;
	XmStringTag             *rendition_tags;
	unsigned int		tag_count;
	XmStringDirection	direction;
	XmDirection		push_before;
	Boolean			pop_after;
	Boolean			separator;
	unsigned char		num_tabs; 	
} Segments;
	

/*
 *  Global variables.
 */
#define NUM_COMPONENTS 10

Components components[NUM_COMPONENTS] = {
    { XmSTRING_COMPONENT_RENDITION_BEGIN, 0, (XtPointer)"render_tag" },
    { XmSTRING_COMPONENT_DIRECTION, 0, (XtPointer)0 },
    { XmSTRING_COMPONENT_TEXT, 0, (XtPointer)"Howdy" },
    { XmSTRING_COMPONENT_SEPARATOR, 0, (XtPointer)0 },
    { XmSTRING_COMPONENT_TAB, 0, (XtPointer)0 },  
    { XmSTRING_COMPONENT_DIRECTION, 0, (XtPointer)0 },
    { XmSTRING_COMPONENT_TEXT, 0, (XtPointer)"Doody" },
    { XmSTRING_COMPONENT_SEPARATOR, 0, (XtPointer)0 },
    { XmSTRING_COMPONENT_RENDITION_END, 0, (XtPointer)"render_tag" },
    { XmSTRING_COMPONENT_END, 0, (XtPointer)0 }};

#define NUM_SEGMENTS 4

Segments segments[NUM_SEGMENTS] = {
    { "One", XmFONTLIST_DEFAULT_TAG, XmCHARSET_TEXT, (XmStringTag *)0,
      0, XmSTRING_DIRECTION_L_TO_R, XmLEFT_TO_RIGHT, False, True,
      2 },
    { "Two", XmFONTLIST_DEFAULT_TAG, XmCHARSET_TEXT, (XmStringTag *)0,
      0, XmSTRING_DIRECTION_R_TO_L, XmLEFT_TO_RIGHT, False, True,
      2 },
    { "Three", XmFONTLIST_DEFAULT_TAG, XmCHARSET_TEXT, (XmStringTag *)0,
      0, XmSTRING_DIRECTION_R_TO_L, (XmDirection)0, True, True,
      1 },
    { "Four", XmFONTLIST_DEFAULT_TAG, XmCHARSET_TEXT, (XmStringTag *)0,
      0, XmSTRING_DIRECTION_L_TO_R, XmRIGHT_TO_LEFT, False, False,
      0 }};

#define NUM_ORDERED_COMPONENTS 11
#define NUM_ORDERED_STRINGS 3

Components ordered_comp[NUM_ORDERED_STRINGS][NUM_ORDERED_COMPONENTS] = {
   {
    { XmSTRING_COMPONENT_LAYOUT_PUSH, 0, (XtPointer)"XmLEFT_TO_RIGHT" }, 
    { XmSTRING_COMPONENT_RENDITION_BEGIN, 0, (XtPointer)"render_tag1" }, 
    { XmSTRING_COMPONENT_RENDITION_BEGIN, 0, (XtPointer)"render_tag2" }, 
    { XmSTRING_COMPONENT_TAG, 0, (XtPointer)XmSTRING_ISO8859_1 }, 
    { XmSTRING_COMPONENT_TAB, 0, (XtPointer)NULL }, 
    { XmSTRING_COMPONENT_DIRECTION, 0, (XtPointer)"XmSTRING_DIRECTION_L_TO_R" },
    { XmSTRING_COMPONENT_TEXT, 0, (XtPointer)"Hello World" }, 
    { XmSTRING_COMPONENT_RENDITION_END, 0, (XtPointer)"render_tag2 " }, 
    { XmSTRING_COMPONENT_RENDITION_END, 0, (XtPointer)"render_tag1 " }, 
    { XmSTRING_COMPONENT_LAYOUT_POP, 0, (XtPointer)NULL }, 
    { XmSTRING_COMPONENT_SEPARATOR, 0, (XtPointer)NULL } 
   },
   {
    { XmSTRING_COMPONENT_LAYOUT_PUSH, 0, (XtPointer)"XmLEFT_TO_RIGHT" }, 
    { XmSTRING_COMPONENT_RENDITION_BEGIN, 0, (XtPointer)"render_tag2" }, 
    { XmSTRING_COMPONENT_RENDITION_BEGIN, 0, (XtPointer)"render_tag1" }, 
    { XmSTRING_COMPONENT_TAB, 0, (XtPointer)NULL }, 
    { XmSTRING_COMPONENT_DIRECTION, 0, (XtPointer)"XmSTRING_DIRECTION_R_TO_L" },
    { XmSTRING_COMPONENT_LOCALE_TEXT, 0, (XtPointer)"Hello World" }, 
    { XmSTRING_COMPONENT_DIRECTION, 0, (XtPointer)"XmSTRING_DIRECTION_L_TO_R" },
    { XmSTRING_COMPONENT_RENDITION_END, 0, (XtPointer)"render_tag2" },
    { XmSTRING_COMPONENT_LAYOUT_POP, 0, (XtPointer)NULL }, 
    { XmSTRING_COMPONENT_SEPARATOR, 0, (XtPointer)NULL }, 
    { XmSTRING_COMPONENT_RENDITION_END, 0, (XtPointer)"render_tag1" }
   },
   {
    { XmSTRING_COMPONENT_LAYOUT_PUSH, 0, (XtPointer)"XmRIGHT_TO_LEFT" }, 
    { XmSTRING_COMPONENT_RENDITION_BEGIN, 0, (XtPointer)"render_tag2" }, 
    { XmSTRING_COMPONENT_DIRECTION, 0, (XtPointer)"XmSTRING_DIRECTION_R_TO_L" },
    { XmSTRING_COMPONENT_WIDECHAR_TEXT, 0, "Hi " }, 
    { XmSTRING_COMPONENT_RENDITION_END, 0, (XtPointer)"render_tag2" }, 
    { XmSTRING_COMPONENT_RENDITION_BEGIN, 0, (XtPointer)"render_tag1" }, 
    { XmSTRING_COMPONENT_TAB, 0, (XtPointer)NULL }, 
    { XmSTRING_COMPONENT_DIRECTION, 0, (XtPointer)"XmSTRING_DIRECTION_L_TO_R" },
    { XmSTRING_COMPONENT_TEXT, 0, "World" }, 
    { XmSTRING_COMPONENT_RENDITION_END, 0, (XtPointer)"render_tag1" },
    { XmSTRING_COMPONENT_LAYOUT_POP, 0, (XtPointer)NULL }
   }};

#define NUM_CANONICAL_COMPONENTS 13

XmStringComponentType canonical_order[NUM_CANONICAL_COMPONENTS] = { 
  XmSTRING_COMPONENT_LAYOUT_PUSH,
  XmSTRING_COMPONENT_RENDITION_BEGIN,
  XmSTRING_COMPONENT_TAG,
  XmSTRING_COMPONENT_LOCALE,
  XmSTRING_COMPONENT_TAB,
  XmSTRING_COMPONENT_DIRECTION,
  XmSTRING_COMPONENT_TEXT,
  XmSTRING_COMPONENT_LOCALE_TEXT,
  XmSTRING_COMPONENT_WIDECHAR_TEXT,
  XmSTRING_COMPONENT_RENDITION_END,
  XmSTRING_COMPONENT_LAYOUT_POP,
  XmSTRING_COMPONENT_SEPARATOR,
  XmSTRING_COMPONENT_END };
  
#define DISPLAY_WIDTH 250
#define DISPLAY_HEIGHT 50
        
XmStringDirection     stringdir_ltor = XmSTRING_DIRECTION_L_TO_R;
XmStringDirection     stringdir_rtol = XmSTRING_DIRECTION_R_TO_L;
XmDirection           dir_ltor = XmLEFT_TO_RIGHT; 
XmDirection           dir_rtol = XmRIGHT_TO_LEFT; 
XmString              draw_string;
Widget                bboard, drawingArea;
GC                    gc;
XGCValues             gcv;
XFontStruct           *fontStruct;
XmRenderTable         renderTable;

void 
DrawString( Widget w, XtPointer client_data, XtPointer call_data )
{

    /*  Display XmString with tabs and renditions. */
    XmStringDrawImage( display, XtWindow( drawingArea ), renderTable, 
                       draw_string, gc, 0, 0, DISPLAY_WIDTH, 
                       XmALIGNMENT_BEGINNING,
                       XmSTRING_DIRECTION_L_TO_R, (XRectangle *)0 );
}

static char * 
CvtCompTypeToString( XmStringComponentType comp_type )
{
   char *comp_string;

      switch( comp_type )
      {
         case XmSTRING_COMPONENT_DIRECTION:
              comp_string = "XmSTRING_COMPONENT_DIRECTION";
              break; 
         case XmSTRING_COMPONENT_END:
              comp_string = "XmSTRING_COMPONENT_END";
              break; 
         case XmSTRING_COMPONENT_LAYOUT_POP:
              comp_string = "XmSTRING_COMPONENT_LAYOUT_POP";
              break; 
         case XmSTRING_COMPONENT_LAYOUT_PUSH:
              comp_string = "XmSTRING_COMPONENT_LAYOUT_PUSH";
              break; 
         case XmSTRING_COMPONENT_LOCALE:
              comp_string = "XmSTRING_COMPONENT_LOCALE";
              break; 
         case XmSTRING_COMPONENT_LOCALE_TEXT:
              comp_string = "XmSTRING_COMPONENT_LOCALE_TEXT";
              break; 
         case XmSTRING_COMPONENT_RENDITION_BEGIN:
              comp_string = "XmSTRING_COMPONENT_RENDITION_BEGIN";
              break; 
         case XmSTRING_COMPONENT_RENDITION_END:
              comp_string = "XmSTRING_COMPONENT_RENDITION_END";
              break; 
         case XmSTRING_COMPONENT_SEPARATOR:
              comp_string = "XmSTRING_COMPONENT_SEPARATOR";
              break; 
         case XmSTRING_COMPONENT_TAB:
              comp_string = "XmSTRING_COMPONENT_TAB";
              break; 
         case XmSTRING_COMPONENT_TAG:
              comp_string = "XmSTRING_COMPONENT_TAG";
              break; 
         case XmSTRING_COMPONENT_TEXT:
              comp_string = "XmSTRING_COMPONENT_TEXT";
              break; 
         case XmSTRING_COMPONENT_WIDECHAR_TEXT:
              comp_string = "XmSTRING_COMPONENT_WIDECHAR_TEXT";
              break; 
       }

     return( comp_string );
}

XmString
BuildOrderedString( Components *comp, int num_comp )
{
   int                   i, j;
   XmString              string, temp, finalString = NULL;
   Components            *current_comp;

     for ( j = 0, current_comp = comp; j < num_comp; j++, current_comp++ )
     {
         switch( current_comp->type ) 
         {
             case XmSTRING_COMPONENT_LAYOUT_PUSH:
               if ( strcmp( (char *)current_comp->value, 
                            "XmLEFT_TO_RIGHT" ) == 0 )
                  string = XmStringComponentCreate( current_comp->type,
                                           sizeof( XmDirection ),
                                           &dir_ltor );
               else if ( strcmp( (char *)current_comp->value, 
                            "XmRIGHT_TO_LEFT" ) == 0 )
                  string = XmStringComponentCreate( current_comp->type,
                                           sizeof( XmDirection ),
                                           &dir_rtol );
               break;

	     case XmSTRING_COMPONENT_DIRECTION:
               if ( strcmp( (char *)current_comp->value, 
                            "XmSTRING_DIRECTION_L_TO_R" ) == 0 )
                  string = XmStringComponentCreate( current_comp->type,
                                           sizeof( XmStringDirection ),
                                           &stringdir_ltor );
               else if ( strcmp( (char *)current_comp->value, 
                            "XmSTRING_DIRECTION_R_TO_L" ) == 0 )
                  string = XmStringComponentCreate( current_comp->type,
                                           sizeof( XmStringDirection ),
                                           &stringdir_rtol );
	       break;

	     case XmSTRING_COMPONENT_RENDITION_BEGIN: 
	     case XmSTRING_COMPONENT_RENDITION_END: 
	     case XmSTRING_COMPONENT_TAG: 
	     case XmSTRING_COMPONENT_LOCALE: 
	     case XmSTRING_COMPONENT_TEXT: 
	     case XmSTRING_COMPONENT_LOCALE_TEXT: 
	     case XmSTRING_COMPONENT_WIDECHAR_TEXT: 
                  string = XmStringComponentCreate( current_comp->type, 
                                           strlen( (char *)current_comp->value ), 
                                           current_comp->value ); 
               break;


	     case XmSTRING_COMPONENT_TAB:
	     case XmSTRING_COMPONENT_LAYOUT_POP:
             case XmSTRING_COMPONENT_SEPARATOR:
             case XmSTRING_COMPONENT_END:
                  string = XmStringComponentCreate( current_comp->type, 
                                           current_comp->length, 
                                           current_comp->value ); 
               break;
        }

        temp = finalString; 
        finalString = XmStringConcatAndFree( temp, string );

     }

  return( finalString );
}
   
Boolean
CheckCanonicalOrder( XmStringComponentType *compList )
{
        int                     i, j, previous_comp;
        int                     numPush, numTag, numLocale, numDir,
                                numText, numSeparator, numEnd, numSegments;
        Boolean                 status = True;
        Boolean                 new_segment;
        

        numPush = 0;
        numTag = 0;
        numLocale = 0; 
        numDir = 0;
        numText = 0;
        numSeparator = 0; 
        numEnd = 0;

        i = 0;
        previous_comp = 0;
        while ( compList[i] != NULL )
        {
           for ( j = 0; j < NUM_CANONICAL_COMPONENTS; j++ )
           { 
               if ( compList[i] == canonical_order[j] )
               {
                  switch( compList[i] )
                  { 
                    case XmSTRING_COMPONENT_LAYOUT_PUSH:
                    case XmSTRING_COMPONENT_TAG:
                    case XmSTRING_COMPONENT_LOCALE:
                    case XmSTRING_COMPONENT_DIRECTION:
                    case XmSTRING_COMPONENT_TEXT:
                    case XmSTRING_COMPONENT_LOCALE_TEXT:
                    case XmSTRING_COMPONENT_WIDECHAR_TEXT:
                    case XmSTRING_COMPONENT_LAYOUT_POP:
                    case XmSTRING_COMPONENT_SEPARATOR:
                    case XmSTRING_COMPONENT_END:
                       if ( j <= previous_comp )
                          status = False;
                      break;

                    case XmSTRING_COMPONENT_RENDITION_BEGIN:
                    case XmSTRING_COMPONENT_TAB:
                    case XmSTRING_COMPONENT_RENDITION_END:
                       if ( j < previous_comp )
                          status = False;
                      break;
                  }
               }
           }
           previous_comp = j;
           i++;
        }

    return( status );
}
 
int
GetStringSegmentsByComp( XmString string, int num_comps, 
                   XmStringComponentType ***compTable )
{
	XmStringContext         context;
	XmStringComponentType	compType;
        unsigned int            length;
        XtPointer               value = NULL;
        int                     i, j, k, numSegments, numComps, maxComps;
        int                     numPush, numTag, numLocale, numDir,
                                numText, numSeparator, numEnd;
	Boolean                 new_segment;
        XmString                newString, temp, finalString;

        numPush = 0;
        numTag = 0;
        numLocale = 0; 
        numDir = 0;
        numText = 0;
        numSeparator = 0; 
        numEnd = 0;
        numSegments = 0;
        maxComps = 0;
        numComps = 0;

       /*
        *  Determine how many segments are in the XmString.
        *  Determine the largest number of components in a given
        *  segment.
        */
	XmStringInitContext( &context, string );
	compType = XmStringGetNextTriple( context, &length, &value );  

        new_segment = False;
        for ( i = 0; i < num_comps; i++ ) 
        {
           switch( compType )
           {
             case XmSTRING_COMPONENT_LAYOUT_PUSH:
               numPush++;
               if ( numPush > 0 ) 
                  new_segment = True;
               break;

             case XmSTRING_COMPONENT_TAG:
               numTag++;
               if ( numTag > 1 ) 
                  new_segment = True;
               break;

             case XmSTRING_COMPONENT_LOCALE:
               numLocale++;
               if ( numLocale > 1 )
                  new_segment = True;
               break;

             case XmSTRING_COMPONENT_DIRECTION:
               numDir++;
               if ( numDir > 1 )
                  new_segment = True;
               break;

             case XmSTRING_COMPONENT_TEXT:
             case XmSTRING_COMPONENT_LOCALE_TEXT:
             case XmSTRING_COMPONENT_WIDECHAR_TEXT:
               numText++;
               if ( numText > 1 )
                  new_segment = True;
               break;

             case XmSTRING_COMPONENT_SEPARATOR:
               numSeparator++; 
               if ( numSeparator > 1 ) 
                  new_segment = True;
               break; 
             case XmSTRING_COMPONENT_END: 
               numEnd++; 
               if ( numEnd > 1 ) 
                  new_segment = True;
               break; 
           } 
      
         if ( value ) XtFree( value ); 
         value = NULL; 

         if ( new_segment )
         {
            numSegments++;
            if ( numComps > maxComps )
                 maxComps = numComps;
            numComps = 0;
         }
         new_segment = False;
         numComps++;
            
         compType = XmStringGetNextTriple( context, &length, &value );  
        } 
        if ( value ) XtFree( value );

        XmStringFreeContext( context );

        *compTable = (XmStringComponentType **)XtMalloc( numSegments *
                       sizeof( XmStringComponentType * ));
        for ( i = 0; i < numSegments; i++ )
            (*compTable)[i] = (XmStringComponentType *)XtMalloc( maxComps + 1 *
                       sizeof( XmStringComponentType ));

        /*
         *  Initialize the list of XmStringComponentTypes.
         */
        for ( i = 0; i < numSegments; i++ )
          for ( j = 0; j < maxComps + 1; j++ )
             (*compTable)[i][j] = NULL;

        numPush = 0;
        numTag = 0;
        numLocale = 0; 
        numDir = 0;
        numText = 0;
        numSeparator = 0; 
        numEnd = 0;

        finalString = NULL;

	XmStringInitContext( &context, string );
	compType = XmStringGetNextTriple( context, &length, &value );  

        j = -1;
        k = 0;
        new_segment = False;
        value = NULL;
        for ( i = 0; i < num_comps; i++ ) 
        {
           switch( compType )
           {
             case XmSTRING_COMPONENT_LAYOUT_PUSH:
               numPush++;
               if ( numPush > 0 )
                  new_segment = True;
               break;

             case XmSTRING_COMPONENT_TAG:
               numTag++;
               if ( numTag > 1 )
                  new_segment = True;
               break;

             case XmSTRING_COMPONENT_LOCALE:
               numLocale++;
               if ( numLocale > 1 )
                  new_segment = True;
               break;

             case XmSTRING_COMPONENT_DIRECTION:
               numDir++;
               if ( numDir > 1 )
                  new_segment = True;
               break;

             case XmSTRING_COMPONENT_TEXT:
             case XmSTRING_COMPONENT_LOCALE_TEXT:
             case XmSTRING_COMPONENT_WIDECHAR_TEXT:
               numText++;
               if ( numText > 1 )
                  new_segment = True;
               break;

             case XmSTRING_COMPONENT_SEPARATOR:
               numSeparator++;
               if ( numSeparator > 1 )
                  new_segment = True;
               break;

             case XmSTRING_COMPONENT_END:
               numEnd++;
               if ( numEnd > 1 )
                  new_segment = True;
               break;
           }

           if ( new_segment )
           {
             /*  Increment segment index. */
              j++; 

             /*
              * Terminate the list of XmStringComponentTypes from  
              * the previous segment. 
              */
              if ( k > 0 )
                 (*compTable)[j][k+1] = NULL;

              k = 0;
              (*compTable)[j][k] = compType;

              new_segment = False;
           }

           /*
            *  Add the latest component to the component list. 
            */
            else
              (*compTable)[j][k] = compType;

            if ( value ) XtFree( value );
            compType = XmStringGetNextTriple( context, &length, &value );  

          /*  Increment component index. */
           k++;
        }

        if ( value ) XtFree( value );
        XmStringFreeContext( context );

     return( numSegments );
}

/**
void
#ifdef _NO_PROTO
PrintComponentTypes( compList )
	XmStringComponentType	*compList;
#else 
PrintComponentTypes( XmStringComponentType *compList )
#endif
{
   int	                 i;

   i = 0;
   while ( compList[i] != NULL )
       printf("%s\n", CvtCompTypeToString( compList[i++] ));

   return;
}
**/


void
PrintComponentTypes( XmString string )
{
	XmStringContext	context;
	XmStringComponentType	compType;
        unsigned int            length;
        XtPointer               value = NULL;

	XmStringInitContext( &context, string );

        do
        {
	    compType = XmStringGetNextTriple( context, &length, &value );  
            printf("%s - ", CvtCompTypeToString( compType ));
            switch( compType )
            {
               case XmSTRING_COMPONENT_LAYOUT_PUSH:
                 if ( XmDirectionMatch(*(XmDirection *)value, XmLEFT_TO_RIGHT ))
                      printf("XmLEFT_TO_RIGHT");
                 else if ( XmDirectionMatch( *(XmDirection *)value, 
                           XmRIGHT_TO_LEFT ))
                      printf("XmRIGHT_TO_LEFT");
                 printf("\n");
                 break;

               case XmSTRING_COMPONENT_RENDITION_BEGIN:
               case XmSTRING_COMPONENT_RENDITION_END:
               case XmSTRING_COMPONENT_TAG:
               case XmSTRING_COMPONENT_LOCALE:
               case XmSTRING_COMPONENT_TEXT:
               case XmSTRING_COMPONENT_LOCALE_TEXT:
               case XmSTRING_COMPONENT_WIDECHAR_TEXT:
                 printf("%s", (char *)value );
                 printf("\n");
                 break;

               case XmSTRING_COMPONENT_DIRECTION:
                 if ( *(XmStringDirection *)value == XmSTRING_DIRECTION_L_TO_R )
                      printf("XmSTRING_DIRECTION_L_TO_R");
	         else if ( *(XmStringDirection *)value ==
                           XmSTRING_DIRECTION_R_TO_L )
                      printf("XmSTRING_DIRECTION_R_TO_L");
                 printf("\n");
                 break;

               case XmSTRING_COMPONENT_TAB:
               case XmSTRING_COMPONENT_LAYOUT_POP:
               case XmSTRING_COMPONENT_SEPARATOR:
               case XmSTRING_COMPONENT_END:
                 printf("\n");
                 break;
           }

           if ( value ) XtFree( (char *)value );
           value = NULL;
         }
         while ( compType != XmSTRING_COMPONENT_END );

         printf("\n\n");

	 XmStringFreeContext( context );

    return;
}
               
           

      
void
main( int argc, char **argv )
{
   int  		 i, j, k, n;
   Arg                   args[25];
   XmString              stringComp, stringSeg, temp, majorString;
   Components            *comp;
   Segments              *segment;
   XmStringContext	 context;
   XmStringComponentType type;
   XmTextType            text_type;
   unsigned int          length;
   XtPointer             value = (XtPointer)0;
   Boolean               passed, one_dir = False, separator, pop_after;
   char                  *text, *comp_string, buffer[20];
   XmStringTag	         tag, *rendition_tags;
   unsigned int          tag_count;
   XmStringDirection     direction;
   XmDirection           push_before;
   XmStringComponentType unknown_tag = 0;
   unsigned short        unknown_length = 0;
   unsigned char         *unknown_value = (unsigned char *)0, tabs;
   XmRendition           rendition;
   XmTab                 tab;
   XmTabList             tabList;
   Boolean		 status;
   int                   numSegments;
   XmStringComponentType **compTable;         

   CommonTestInit( argc, argv );

  /*
   *  Build an XmString from the components defined in Components structure.
   */
   for ( i = 0; i < NUM_COMPONENTS; i++ )
   {
        comp = &components[i]; 

       /*
        *  Compute the length and value of the components where necessary.
        */
        switch( comp->type )
        {
           case XmSTRING_COMPONENT_TEXT:
           case XmSTRING_COMPONENT_RENDITION_BEGIN:
           case XmSTRING_COMPONENT_RENDITION_END:
             comp->length = ( comp->value ) ? strlen( comp->value ) : 0;
              break;
           case XmSTRING_COMPONENT_DIRECTION:
             comp->length = sizeof( XmStringDirection );
             comp->value = ( one_dir ) ? (XtPointer)&stringdir_rtol : 
                           (XtPointer)&stringdir_ltor;
             one_dir = True;
              break;
        }

        if ( i == 0 )
           majorString =
    	   XmStringComponentCreate( comp->type, comp->length, comp->value );
        else
        {
           stringComp =
    	   XmStringComponentCreate( comp->type, comp->length, comp->value );
           temp = majorString;
           majorString = XmStringConcat( temp, stringComp ); 
           XmStringFree( temp );
           XmStringFree( stringComp );
        }
   }

  /*
   *  Retrieve each of the components in the XmString by
   *  calling XmStringPeekNextTriple() and XmStringGetNextTriple(). 
   */
   XmStringInitContext( &context, majorString ); 
   for ( i = 0; i < NUM_COMPONENTS; i++ )
   {
        comp = &components[i]; 
        comp_string = CvtCompTypeToString( comp->type );
        printf("Component #%d, Component Type = %s\n", i+1, comp_string );
	type = XmStringPeekNextTriple( context );	
        printf("XmStringPeekNextTriple() - "); 
        if ( type == comp->type )
           printf("PASSED.");
        else
           printf("FAILED.");
           printf("\n");

        passed = True;

	type = XmStringGetNextTriple( context, &length, &value );
        printf("XmStringGetNextTriple() - "); 
        if ( type == comp->type ) 
           if ( length == comp->length )
           {
              switch( type )
              {
                case XmSTRING_COMPONENT_TEXT:
                case XmSTRING_COMPONENT_RENDITION_BEGIN:
                case XmSTRING_COMPONENT_RENDITION_END:
                  if (( value &&
                       ( strcmp( (char *)value, (char *)comp->value ) != 0 )) ||
                       ( !value && comp->value )) 
                       passed = False;
                  break;
                case XmSTRING_COMPONENT_SEPARATOR:
                case XmSTRING_COMPONENT_TAB:
                case XmSTRING_COMPONENT_END:
                  if ( value != comp->value )
                     passed = False;
                  break;
                case XmSTRING_COMPONENT_DIRECTION:
                  if (( value && ( *((XmStringDirection *)value) != 
                        *((XmStringDirection *)comp->value ))) ||
                        ( !value && comp->value )) 
                     passed = False;
                  break;
               }
	       if ( value )
	       {
	          XtFree((char *)value);
	          value = (XtPointer)0;
               }
             }
             else
                passed = False;
           else
                passed = False;

           if ( passed )
              printf("PASSED.");
           else
              printf("FAILED.");

           printf("\n\n");
   }
    XmStringFreeContext( context ); 
    printf("\n\n\n");

   CommonPause();

  /*
   *  Reinitialize context -- and retrieve the contents of the string
   *  again, this time calling XmStringPeekNextComponent() and
   *  XmStringGetNextComponent().
   */
   XmStringInitContext( &context, majorString ); 
   for ( i = 0; i < NUM_COMPONENTS; i++ )
   {
        comp = &components[i]; 
        comp_string = CvtCompTypeToString( comp->type );
        printf("Component #%d, Component Type = %s\n", i+1, comp_string );

        passed = True;
        type = XmStringPeekNextComponent( context );
        printf("XmStringPeekNextComponent() - ");
        if ( type != comp->type )
           passed = False;

        if ( passed )
           printf("PASSED. \n");
        else
           printf("FAILED. \n");

        passed = True;

        type = XmStringGetNextComponent( context, &text, &tag, &direction,
                 &unknown_tag, &unknown_length, &unknown_value );
        printf("XmStringGetNextComponent() - ");
        if ( type == comp->type || type == XmSTRING_COMPONENT_UNKNOWN )
           switch( type )
           {
             case XmSTRING_COMPONENT_TEXT:
                if (( text &&
                    ( strcmp( text, (char *)comp->value ) != 0 )) ||
                    ( !text && comp->value ))
                    passed = False;
                if ( text ) XtFree( text );
                break;
             case XmSTRING_COMPONENT_DIRECTION:
               if ( direction != *((XmStringDirection *)comp->value )) 
                  passed = False;
               break;
             case XmSTRING_COMPONENT_SEPARATOR:
             case XmSTRING_COMPONENT_END:
                break;
             case XmSTRING_COMPONENT_UNKNOWN:
                switch( unknown_tag )
                { 
                  case XmSTRING_COMPONENT_RENDITION_BEGIN:
                  case XmSTRING_COMPONENT_RENDITION_END:
                    if ( unknown_value && 
                       ( strcmp( (char *)unknown_value, 
                       (char *)comp->value ) != 0 ) ||
                       ( !unknown_value && comp->value ))
                      passed = False;
                    if ( passed &&
                         unknown_length != comp->length )
                      passed = False;
                    break;                         
                  case XmSTRING_COMPONENT_TAB:
                    if ( (XtPointer)unknown_value != comp->value )
                      passed = False;
                    if ( passed &&
                         unknown_length != comp->length )
                      passed = False;
                    break;
                }
                break;
            }
           else
            passed = False;

           if ( passed )
              printf("PASSED.");
           else
              printf("FAILED.");
           printf("\n\n");

           unknown_tag = 0;
           unknown_length = 0;
           if ( unknown_value )
           {
              XtFree((char *)unknown_value);
              unknown_value = (unsigned char *)0;
           }
      }

    XmStringFreeContext( context ); 
    XmStringFree( majorString );

   CommonPause();

    printf("\n\n");


   /*
    *  Build XmString from segments defined in Segments structure. 
    */
    j = 0;
    for ( i = 0; i < NUM_SEGMENTS; i++ )
    {
       segment = &segments[i];
       if ( i == 0 )
         majorString = XmStringSegmentCreate( segment->text, segment->tag,
                     segment->direction, segment->separator );      
       else if ( i > 0 )
       {
         stringSeg = XmStringSegmentCreate( segment->text, segment->tag,
                     segment->direction, segment->separator );      
         temp = majorString;
         majorString = XmStringConcat( temp, stringSeg );
         XmStringFree( temp );
         XmStringFree( stringSeg );
       }
     }

    /*
     *  Retrieve segments from the XmString using XmStringGetNextSegment(). 
     */
    text = (char *)0;
    tag = (XmStringTag)0;
    XmStringInitContext( &context, majorString ); 
    for ( i = 0; i < NUM_SEGMENTS; i++ )
    {
       segment = &segments[i];
       printf("Segment #%d\n", i+1 );
       printf("XmStringGetNextSegment() - ");
       if ( XmStringGetNextSegment( context, &text, &tag, 
              &direction, &separator )) 
       {
         passed = True;
         if (( text && ( strcmp( text, segment->text )) != 0 ) ||
             ( !text && text != segment->text )) 
             passed = False;

         if ( passed && ( tag && ( strcmp( tag, segment->tag ) != 0 )) ||
             ( !tag && tag != segment->tag ))
             passed = False;

         if ( passed && direction != segment->direction )
             passed = False;

         if ( passed && separator != segment->separator )
             passed = False;
       }
       else
         passed = False;
    
       if ( text ) {
          XtFree((char *) text );
          text = (char *)0;
       }
       if ( tag ) {
          XtFree((char *) tag );
          tag = (XmStringTag)0;
       }
       if ( passed )
          printf("PASSED. \n");
       else
          printf("FAILED. \n");

     }
     printf("\n");
     XmStringFreeContext( context );
     XmStringFree( majorString );

    CommonPause();

    n = 0;
    XtSetArg( args[n], XmNwidth, DISPLAY_WIDTH ); n++;
    XtSetArg( args[n], XmNheight, DISPLAY_HEIGHT ); n++;
    XtSetArg( args[n], XmNmarginWidth, 0 ); n++;
    XtSetArg( args[n], XmNmarginHeight, 0 ); n++;
    bboard = XmCreateBulletinBoard( Shell1, "bulletinBoard", args, n );
    XtManageChild( bboard );

  /*  Create Drawing Area. */
    n = 0;
    XtSetArg( args[n], XmNx, 0 ); n++;
    XtSetArg( args[n], XmNy, 0 ); n++;
    XtSetArg( args[n], XmNwidth, DISPLAY_WIDTH ); n++;
    XtSetArg( args[n], XmNheight, DISPLAY_HEIGHT ); n++;
    XtSetArg( args[n], XmNbackground, CommonGetColor("light blue")); n++;
    drawingArea = XmCreateDrawingArea( bboard, "DrawArea", args, n );
    XtManageChild( drawingArea );

    XtAddCallback( drawingArea, XmNexposeCallback, DrawString, NULL );

    n = 0;
    XtSetArg(args[n], XmNforeground, &gcv.foreground); n++;
    XtSetArg(args[n], XmNbackground, &gcv.background); n++;
    XtGetValues(drawingArea, args, n);

    fontStruct = (XFontStruct *)XLoadQueryFont( display, "fixed" );

   /*
    *  Create tabList.
    */
    tab = XmTabCreate( 1.0, XmINCHES, XmABSOLUTE, XmALIGNMENT_BEGINNING,
                       (char *)0 );

    tabList = XmTabListInsertTabs( NULL, &tab, 1, 0 );

    XmTabFree( tab );

   /*
    *  Set up renderTable for displaying strings created using
    *  components in ordered_comp[] array.
    */
    n = 0;
    XtSetArg( args[n], XmNfont, (XtPointer)fontStruct ); n++;
    XtSetArg( args[n], XmNfontType, XmFONT_IS_FONT ); n++;
    XtSetArg( args[n], XmNtabList, tabList ); n++;
    XtSetArg( args[n], XmNrenditionForeground, CommonGetColor("black")); n++;
    rendition = XmRenditionCreate( drawingArea, "render_tag1", args, n );

    renderTable = XmRenderTableAddRenditions( NULL, &rendition, 1, XmMERGE_REPLACE );

    XmRenditionFree( rendition );

    n = 0;
    XtSetArg( args[n], XmNfontName, "9x15" ); n++;
    XtSetArg( args[n], XmNfontType, XmFONT_IS_FONT ); n++;
    XtSetArg( args[n], XmNrenditionForeground, CommonGetColor("red")); n++;
    rendition = XmRenditionCreate( drawingArea, "render_tag2", args, n );

    renderTable = XmRenderTableAddRenditions( renderTable, &rendition, 1, XmMERGE_REPLACE );
    
    XmRenditionFree( rendition );
    XmTabListFree( tabList );

    XtRealizeWidget( Shell1 );

    gcv.font = fontStruct->fid;
    gc = XCreateGC( display, XtWindow( drawingArea ),
                    GCForeground | GCBackground | GCFont, &gcv );


    for ( i = 0; i < NUM_ORDERED_STRINGS; i++ )
    {
      majorString =
        BuildOrderedString( &ordered_comp[i][0], NUM_ORDERED_COMPONENTS );
      draw_string = XmStringCopy( majorString );
      XClearArea(XtDisplay(drawingArea), XtWindow(drawingArea), 
                 0, 0, 0, 0, TRUE);
      XmStringFree( draw_string );
      PrintComponentTypes( majorString ); 
/**
      numSegments = GetStringSegmentsByComp( majorString, NUM_ORDERED_COMPONENTS,
                    &compTable ); 
      printf("XmString contains %d segments\n", numSegments );
      for ( j = 0; j < numSegments; j++ )
      {
         status = CheckCanonicalOrder( compTable[j] );
         printf("Checking canonical order of segment #%d:\n", j+1 );
         if ( status )
            printf("XmString components were returned in the CORRECT order.");
         else
            printf("XmString components were returned in the INCORRECT order.");
         printf("\n\n");
         PrintComponentTypes( compTable[j] );
         	printf("\n\n");
      }
**/
      CommonPause();
    }

    CommonPause();

    XtAppMainLoop( app_context );
}
