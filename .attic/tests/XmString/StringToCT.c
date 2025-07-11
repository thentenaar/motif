/* $TOG: StringToCT.c /main/6 1997/09/15 14:20:29 cshi $ */
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
#include <stdio.h>
#include <testlib.h>

#define STRING_LENGTH 256

static char *string1 = "This is a test.";
static char *string2 = "This is only a test.";


typedef struct  _Component {
  XmStringComponentType     type;
  char                      charset[STRING_LENGTH];
  char                      text[STRING_LENGTH];
  XmStringDirection         direction;
} Component;

static Component  *components;
static int        num_components = 0;

static char *
CvtCompTypeToString( XmStringComponentType comp_tag )
{
     char *comp_string;

      switch( comp_tag )
      {
         case XmSTRING_COMPONENT_TEXT:
              comp_string = "XmSTRING_COMPONENT_TEXT";
              break;
         case XmSTRING_COMPONENT_LOCALE_TEXT:
              comp_string = "XmSTRING_COMPONENT_LOCALE_TEXT";
              break;
         case XmSTRING_COMPONENT_CHARSET:
              comp_string = "XmSTRING_COMPONENT_CHARSET";
              break;
         case XmSTRING_COMPONENT_DIRECTION:
              comp_string = "XmSTRING_COMPONENT_DIRECTION";
              break;
         case XmSTRING_COMPONENT_SEPARATOR:
              comp_string = "XmSTRING_COMPONENT_SEPARATOR";
              break;
      }

     return( comp_string );
}

static char *
CvtDirectionToString( XmStringDirection direction )
{
     char *dir_string;

     switch( direction )
     {
        case XmSTRING_DIRECTION_L_TO_R:
             dir_string = "XmSTRING_DIRECTION_L_TO_R";
             break;

        case XmSTRING_DIRECTION_R_TO_L:
             dir_string = "XmSTRING_DIRECTION_R_TO_L";
             break;
     }

   return( dir_string );
}

static char *
CvtSeparatorToString( Boolean separator )
{
    return(( ( separator ) ? "True" : "False" ));
}
 
static Boolean
PrintXmStringComponents( string, compare )
	XmString	string;
        Boolean		compare;
{
    XmStringComponentType  comp_type, unknown_tag;
    XmStringContext        context = NULL;
    char                   *text = NULL;
    XmStringCharSet        charset = NULL;
    XmStringDirection      direction;
    unsigned short         unknown_length;
    unsigned char          *unknown_value;
    Boolean                passed = True;
    int                    i;

    if ( XmStringInitContext(&context, string ))
    {
       for ( i = 0; i < num_components; i++ )
       {
           comp_type = XmStringGetNextComponent( context, &text, &charset,
                         &direction, &unknown_tag, &unknown_length,
                         &unknown_value );
           switch( comp_type )
           {
 	      case XmSTRING_COMPONENT_END:
 		   i = num_components;	/* stop loop */
 		   break;
              case XmSTRING_COMPONENT_CHARSET:
                   if ( !compare )
                   {
                      components[i].type = XmSTRING_COMPONENT_CHARSET;
                      sprintf( components[i].charset, "%s", charset );
                   }
                   else if ((components[i].type != XmSTRING_COMPONENT_CHARSET) ||
                            ( strcmp( components[i].charset, charset )))
                      passed = False;
                   printf("%s: %s\n", CvtCompTypeToString(comp_type),
                                      charset );
                   XtFree( charset );
                   charset = NULL;
                break;
              case XmSTRING_COMPONENT_LOCALE_TEXT:
                   if ( !compare )
                   {
                       components[i].type = XmSTRING_COMPONENT_LOCALE_TEXT;
                       sprintf( components[i].text, "%s", text );
                   }
                   else if ((components[i].type != XmSTRING_COMPONENT_LOCALE_TEXT ) ||
                            ( strcmp( components[i].text, text )))
                      passed = False;
                   printf("%s: %s\n", CvtCompTypeToString(comp_type),
                                       text );
                   XtFree( text );
                   text = NULL;
                break;
              case XmSTRING_COMPONENT_TEXT:
                   if ( !compare )
                   {
                      components[i].type = XmSTRING_COMPONENT_TEXT;
                      sprintf( components[i].text, "%s", text );
                   }
                   else if ((components[i].type != XmSTRING_COMPONENT_TEXT ) ||
                            ( strcmp( components[i].text, text )))
                      passed = False;
                   printf("%s: %s\n", CvtCompTypeToString(comp_type),
                                      text );
                   XtFree( text );
                   text = NULL;
                break;

              case XmSTRING_COMPONENT_DIRECTION:
                   if ( !compare )
                   {
                      components[i].type = XmSTRING_COMPONENT_DIRECTION;
                      components[i].direction = direction;
                   }
                   else if (( components[i].type != 
                              XmSTRING_COMPONENT_DIRECTION ) ||
                            ( components[i].direction != direction ))
                      passed = False;
                   printf("%s: %s\n", CvtCompTypeToString(comp_type),
                                      CvtDirectionToString( direction ));
                break;

	      case XmSTRING_COMPONENT_SEPARATOR:
                   if ( !compare )
                      components[i].type = XmSTRING_COMPONENT_SEPARATOR;  
                   else if (components[i].type != XmSTRING_COMPONENT_SEPARATOR )
                      passed = False;
                   printf("%s: True\n", 
                          CvtCompTypeToString(comp_type)); 
                break;

	      case XmSTRING_COMPONENT_UNKNOWN:
                 switch( unknown_tag )
                 {
                   case XmSTRING_COMPONENT_CHARSET:
                        if ( !compare )
                        {
                           components[i].type = XmSTRING_COMPONENT_CHARSET;
                           sprintf( components[i].charset, "%s", 
                                    (char *)unknown_value );
                        }
                        else if ((components[i].type != 
                                  XmSTRING_COMPONENT_CHARSET ) ||
                            ( strcmp( components[i].charset, 
                                  (char *)unknown_value )))
                           passed = False;
                        printf("%s: %s\n", CvtCompTypeToString(unknown_tag),
                                           (char *)unknown_value );
                        XtFree( (char *)unknown_value );
                        unknown_value = NULL;
                     break;
                   case XmSTRING_COMPONENT_TEXT:
                        if ( !compare )
                        {
                           components[i].type = XmSTRING_COMPONENT_TEXT;
                           sprintf( components[i].text, "%s", 
                                    (char*)unknown_value );
                        }
                        else if (( components[i].type != 
                                   XmSTRING_COMPONENT_TEXT ) ||
                                 ( strcmp( components[i].text, 
                                   (char *)unknown_value )))
                           passed = False;
                        printf("%s: %s\n", CvtCompTypeToString(unknown_tag),
                                           (char *)unknown_value );
                        XtFree((char *)unknown_value );
                        unknown_value = NULL;
                     break;

                   case XmSTRING_COMPONENT_DIRECTION:
                        if ( !compare )
                        {
                           components[i].type = XmSTRING_COMPONENT_DIRECTION;
                           components[i].direction = 
                                      *((XmStringDirection *)unknown_value );
                        }
                        else if (( components[i].type != 
                                  XmSTRING_COMPONENT_DIRECTION ) ||
                                ( components[i].direction != 
                                  *((XmStringDirection *)unknown_value )))
                           passed = False;
                        printf("%s: %s\n", 
                          CvtCompTypeToString(unknown_tag), 
                          CvtDirectionToString( 
                          *((XmStringDirection *)unknown_value ))); 
                        XtFree((char *)unknown_value );
                        unknown_value = NULL;
                     break;

	           case XmSTRING_COMPONENT_SEPARATOR:
                        if ( !compare )
                           components[i].type = XmSTRING_COMPONENT_SEPARATOR;  
                        else if ( components[i].type != 
                                  XmSTRING_COMPONENT_SEPARATOR )
                           passed = False;
                        printf("%s: True", 
                          CvtCompTypeToString(unknown_tag)); 
                     break;
                   default:
                        components[i].type = XmSTRING_COMPONENT_UNKNOWN;
                        printf("Unknown component encountered.\n"); 
                  }
                break;
             }
          }
    }

    return( passed );
}

void
main(argc, argv)
unsigned int    argc;
char            *argv[];
{
    XmString    	   s1, s2, dir1, dir2, sep, temp, finalString;
    int                    i;
    char                   *text = NULL;
    Boolean                compare, passed;

    CommonTestInit( argc, argv );
    XtRealizeWidget( Shell1 );

   /*
    *  Create text components of XmStrings.
    */
    s1 = XmStringCreate( string1, XmFONTLIST_DEFAULT_TAG );
    s2 = XmStringCreate( string2 , XmFONTLIST_DEFAULT_TAG);

   /*
    *  Create direction components of XmStrings.
    */
    dir1 = XmStringDirectionCreate( XmSTRING_DIRECTION_R_TO_L );
    dir2 = XmStringDirectionCreate( XmSTRING_DIRECTION_L_TO_R );

   /*
    *  Create separator component of XmStrings.
    */
    sep = XmStringSeparatorCreate();

   /*
    *  Concatenate components into a final XmString.
    */
    finalString = XmStringConcat( dir1, s1 );
    XmStringFree( dir1 );
    XmStringFree( s1 );
    num_components+=3;

    temp = finalString;
    finalString = XmStringConcat( temp, sep );
    XmStringFree( temp );
    num_components++;

    temp = finalString;
    finalString = XmStringConcat( temp, sep );
    XmStringFree( temp );
    num_components++;

    temp = finalString;
    finalString = XmStringConcat( temp, sep );
    XmStringFree( temp );
    num_components++;

    temp = finalString;
    finalString = XmStringConcat( temp, dir2 );
    XmStringFree( temp );
    num_components++;

    temp = finalString;
    finalString = XmStringConcat( temp, s2 );
    XmStringFree( temp );
    num_components+=2;

   /*
    *  Create array to store component types.
    */
    components = (Component *)XtMalloc( num_components * sizeof( Component ));

    printf("\nComponents in original XmString:\n");
    printf("----------------------------------\n");

    compare = False;
    passed = PrintXmStringComponents( finalString, compare );

    CommonPause();

    printf("\nCompound Text returned from XmCvtXmStringToCT(): \n");
    printf("-------------------------------------------------- \n");

    text = XmCvtXmStringToCT( finalString );
    printf("%s\n", text );

    CommonPause();

    printf("\nComponents in XmString returned from XmCvtCTToXmString():\n");
    printf("------------------------------------------------------------\n");

    XmStringFree( finalString );
    finalString = XmCvtCTToXmString( text ); 
    compare = True;
    passed = PrintXmStringComponents( finalString, compare );


    printf("\nXmCvtCTToXmString: ");
    if ( passed )
       printf("PASSED\n");
    else
       printf("FAILED\n");

    CommonPause();
    CommonPause();
    XtAppMainLoop( app_context );
}

