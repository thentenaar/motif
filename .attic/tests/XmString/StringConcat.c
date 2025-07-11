/* $XConsortium: StringConcat.c /main/5 1995/07/15 21:12:34 drk $ */
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

#define NUM_COMPONENT_TYPES 13

static char *render_tag = "render";
static XmDirection direction = XmLEFT_TO_RIGHT;
static XmStringDirection string_direction = XmSTRING_DIRECTION_L_TO_R;

typedef struct _Components {
  XmStringComponentType	type;
  int                   length;
  XtPointer             value;
} Components;

Components components[NUM_COMPONENT_TYPES] = {
  { XmSTRING_COMPONENT_LAYOUT_PUSH, sizeof(XmDirection), (XtPointer)&direction },
  { XmSTRING_COMPONENT_RENDITION_BEGIN, 0, (XtPointer)0 },
  { XmSTRING_COMPONENT_TAG, 0, (XtPointer)0 },
  { XmSTRING_COMPONENT_LOCALE, 0, (XtPointer)0 },
  { XmSTRING_COMPONENT_TAB, 0, (XtPointer)NULL },
  { XmSTRING_COMPONENT_DIRECTION, sizeof(XmStringDirection), (XtPointer)&string_direction },
  { XmSTRING_COMPONENT_TEXT, 0, (XtPointer)0 },
  { XmSTRING_COMPONENT_LOCALE_TEXT, 0, (XtPointer)0 },
  { XmSTRING_COMPONENT_WIDECHAR_TEXT, 0, (XtPointer)0 },
  { XmSTRING_COMPONENT_RENDITION_END, 0, (XtPointer)0 },
  { XmSTRING_COMPONENT_LAYOUT_POP, 0, NULL },
  { XmSTRING_COMPONENT_SEPARATOR, 0, NULL },
  { XmSTRING_COMPONENT_END, 0, NULL }};

typedef struct _CompStrings {
  XmStringComponentType	type;
  String                string;
} CompStrings;

CompStrings comp_strings[NUM_COMPONENT_TYPES] = {
  { XmSTRING_COMPONENT_LAYOUT_PUSH, "XmSTRING_COMPONENT_LAYOUT_PUSH" },
  { XmSTRING_COMPONENT_RENDITION_BEGIN, "XmSTRING_COMPONENT_RENDITION_BEGIN" },
  { XmSTRING_COMPONENT_TAG, "XmSTRING_COMPONENT_TAG" },
  { XmSTRING_COMPONENT_LOCALE, "XmSTRING_COMPONENT_LOCALE" },
  { XmSTRING_COMPONENT_TAB, "XmSTRING_COMPONENT_TAB" },
  { XmSTRING_COMPONENT_DIRECTION, "XmSTRING_COMPONENT_DIRECTION" },
  { XmSTRING_COMPONENT_TEXT, "XmSTRING_COMPONENT_TEXT" },
  { XmSTRING_COMPONENT_LOCALE_TEXT, "XmSTRING_COMPONENT_LOCALE_TEXT" },
  { XmSTRING_COMPONENT_WIDECHAR_TEXT, "XmSTRING_COMPONENT_WIDECHAR_TEXT" },
  { XmSTRING_COMPONENT_RENDITION_END, "XmSTRING_COMPONENT_RENDITION_END" },
  { XmSTRING_COMPONENT_LAYOUT_POP, "XmSTRING_COMPONENT_LAYOUT_POP" },
  { XmSTRING_COMPONENT_SEPARATOR, "XmSTRING_COMPONENT_SEPARATOR" },
  { XmSTRING_COMPONENT_END, "XmSTRING_COMPONENT_END" }};

String get_component_string( XmStringComponentType type )
{
   	int 	i;
	String  string;
	

   for ( i = 0; i < NUM_COMPONENT_TYPES; i++ )
       if ( type == comp_strings[i].type )
       {
            string = comp_strings[i].string;
            break;
       }

   return( string );
}

void set_args( Components *comp, unsigned int *length, char **value )
{
           switch ( comp->type )
           {
               case XmSTRING_COMPONENT_TAG:
               case XmSTRING_COMPONENT_LOCALE_TEXT:
               case XmSTRING_COMPONENT_RENDITION_BEGIN:
               case XmSTRING_COMPONENT_RENDITION_END:
               case XmSTRING_COMPONENT_WIDECHAR_TEXT:
                 *length = strlen(render_tag);
                 *value = render_tag;
                 break;
               case XmSTRING_COMPONENT_LOCALE:
                 *length = strlen(_MOTIF_DEFAULT_LOCALE); 
                 *value = _MOTIF_DEFAULT_LOCALE;
                 break;
               default:
                 *length = comp->length;
                 *value = comp->value;
                 break;
            }

      return;
}

       
main ( unsigned int argc, char *argv[] )
{

     XmString 	comp1, comp2, string;
     int        i, j;
     unsigned int  length;
     char       *value;


     CommonTestInit( argc, argv );

   /*
    *  Call XmStringConcatAndFree() using every combination of
    *  XmStringComponentTypes.
    */
     for ( i = 0; i < NUM_COMPONENT_TYPES; i++ )
     {
        for ( j = 0; j < NUM_COMPONENT_TYPES; j++ ) 
        {
           set_args( &components[i], &length, &value );
           comp1 = XmStringComponentCreate( components[i].type,
                                            length, value );

           set_args( &components[j], &length, &value );
           comp2 = XmStringComponentCreate( components[j].type,
                                            length, value );
           
           printf("XmStringConcatAndFree(%s, \n",
                  get_component_string( components[i].type ));
           printf("                      %s):",
                  get_component_string( components[j].type ));
                 
           string = XmStringConcatAndFree( comp1, comp2 );

           if ( string )
              printf("         ** PASSED **\n");
           else
              printf("         ** FAILED **\n");

	   XmStringFree( string );
        }

        printf("\n");
     }

     CommonPause();
     CommonPause();

     XtAppMainLoop( app_context );
}
