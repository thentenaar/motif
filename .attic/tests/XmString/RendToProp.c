/* $XConsortium: RendToProp.c /main/6 1996/06/11 17:23:07 pascale $ */
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
#include <math.h>

/*
 * Defines
 */
#define NUM_RENDITIONS 5

/*
 * Global variables.
 */
int            n;
Arg            args[25];

typedef struct _RenditionResources {
  Pixel          rendition_background;
  XtPointer      font;
  String         font_name;
  XmFontType     font_type;
  Pixel          rendition_foreground;
  unsigned char  load_model;
  unsigned char  strikethru_type;
  XmTabList      tabList;
  XmStringTag    tag;
  unsigned char  underline_type;
} RenditionResources;

typedef struct _TabValues {
  float          value;
  unsigned char  units;
  XmOffsetModel  offset;
  unsigned char  alignment;
  char           *decimal;
} TabValues;

XmRenderTable
CreateRenderTable()
{
        XmRendition    renditions[NUM_RENDITIONS];
        XmRenderTable  renderTable = (XmRenderTable)0;
	char           tag[10];
	XmTab          tab1, tab2;
	XmTabList      tabList = NULL;
        int            i;

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

   return( renderTable );
}


Boolean
CompareRenderTables( XmRenderTable renderTable1, XmRenderTable renderTable2 ) 
{
     XmRendition         *renditions1, *renditions2;
     XmStringTag         *tags1, *tags2;
     Cardinal            tag_count1, tag_count2, tab_count1, tab_count2;
     Boolean             status = True;
     int                 i, j;
     RenditionResources  rendRes1, rendRes2;
     TabValues           tabVal1, tabVal2;
     XmTab               tab1, tab2;

    /*
     *  Get renditions for the original renderTable.
     */
     tag_count1 = XmRenderTableGetTags( renderTable1, &tags1 ); 
     renditions1 =
     XmRenderTableGetRenditions( renderTable1, tags1, tag_count1 ); 

    /*
     *  Get renditions for the converted renderTable.
     */
     tag_count2 = XmRenderTableGetTags( renderTable2, &tags2 ); 
     renditions2 =
     XmRenderTableGetRenditions( renderTable2, tags2, tag_count2 ); 

     if ( tag_count1 != tag_count2 )
        status = False;

     if ( status )
        for ( i = 0; i < tag_count1; i++ )
        {
           n = 0;
           XtSetArg( args[n], XmNrenditionBackground,
                     &rendRes1.rendition_background ); n++;
           XtSetArg( args[n], XmNfont, &rendRes1.font ); n++;
           XtSetArg( args[n], XmNfontName, &rendRes1.font_name ); n++;
           XtSetArg( args[n], XmNfontType, &rendRes1.font_type ); n++;
           XtSetArg( args[n], XmNrenditionForeground, 
                     &rendRes1.rendition_foreground ); n++;
           XtSetArg( args[n], XmNloadModel, &rendRes1.load_model ); n++;
           XtSetArg( args[n], XmNstrikethruType, 
                     &rendRes1.strikethru_type ); n++;
           XtSetArg( args[n], XmNtabList, &rendRes1.tabList ); n++;
           XtSetArg( args[n], XmNtag, &rendRes1.tag ); n++;
           XtSetArg( args[n], XmNunderlineType, 
                     &rendRes1.underline_type ); n++;
           XmRenditionRetrieve( renditions1[i], args, n );
           
           n = 0;
           XtSetArg( args[n], XmNrenditionBackground,
                     &rendRes2.rendition_background ); n++;
           XtSetArg( args[n], XmNfont, &rendRes2.font ); n++;
           XtSetArg( args[n], XmNfontName, &rendRes2.font_name ); n++;
           XtSetArg( args[n], XmNfontType, &rendRes2.font_type ); n++;
           XtSetArg( args[n], XmNrenditionForeground, 
                     &rendRes2.rendition_foreground ); n++;
           XtSetArg( args[n], XmNloadModel, &rendRes2.load_model ); n++;
           XtSetArg( args[n], XmNstrikethruType, 
                     &rendRes2.strikethru_type ); n++;
           XtSetArg( args[n], XmNtabList, &rendRes2.tabList ); n++;
           XtSetArg( args[n], XmNtag, &rendRes2.tag ); n++;
           XtSetArg( args[n], XmNunderlineType, 
                     &rendRes2.underline_type ); n++;
           XmRenditionRetrieve( renditions2[i], args, n );

	  /* Compare tags. */
	  if ((rendRes1.tag != rendRes2.tag) &&
	      (strcmp(rendRes1.tag, rendRes2.tag) != 0))
	    break;
	   
          /*  Compare values of XmNrenditionBackground resource. */
           if ( rendRes1.rendition_background != 
                rendRes2.rendition_background )
              break;

          /*  Compare values of XmNfont resource. */
           if (( rendRes1.font != rendRes2.font ) &&
               ( ((XFontStruct *)rendRes1.font)->fid !=
                   ((XFontStruct *)rendRes2.font)->fid ))
                 break;
           
          /*  Compare values of XmNfontName resource. */
           if (( rendRes1.font_name != rendRes2.font_name ) &&
               ( strcmp( rendRes1.font_name, rendRes2.font_name ) != 0 ))
                 break;

          /*  Compare values of XmNfontType resource. */
           if ( rendRes1.font_type != rendRes2.font_type )
              break;

          /*  Compare values of XmNrenditionForeground resource. */
           if ( rendRes1.rendition_foreground != 
                rendRes2.rendition_foreground )
              break;

          /*  Compare values of XmNloadModel resource. */
           if ( rendRes1.load_model != rendRes2.load_model )
              break;

          /*  Compare values of XmNstrikethruType resource. */
           if ( rendRes1.strikethru_type != 
                rendRes2.strikethru_type )
              break;

          /*  Compare values of XmNtabList resource. */
           if ( rendRes1.tabList != rendRes2.tabList )
           {
	     if ((rendRes1.tabList == (XtPointer)XmAS_IS) ||
		 (rendRes2.tabList == (XtPointer)XmAS_IS))
	       break;
		 
              tab_count1 = XmTabListTabCount( rendRes1.tabList ); 
              tab_count2 = XmTabListTabCount( rendRes2.tabList ); 
              if ( tab_count1 != tab_count2 )
                 break;

              for ( j = 0; j < tab_count1; j++ )
              {
                 tab1 = XmTabListGetTab( rendRes1.tabList, j );
                 tab2 = XmTabListGetTab( rendRes2.tabList, j );

                 tabVal1.value =
                 XmTabGetValues( tab1, &tabVal1.units, &tabVal1.offset,
                         &tabVal1.alignment, &tabVal1.decimal ); 
/*                 sprintf( tabVal_int1, "%f", tabVal1.value ); */

                 tabVal2.value =
                 XmTabGetValues( tab2, &tabVal2.units, &tabVal2.offset,
                         &tabVal2.alignment, &tabVal2.decimal ); 
/*                 sprintf( tabVal_int2, "%f", tabVal2.value ); */

                 if ( tabVal1.value != tabVal2.value )
                    break;
                 if ( tabVal1.units != tabVal2.units )
                    break;
                 if ( tabVal1.offset != tabVal2.offset )
                    break;
                 if ( tabVal1.alignment != tabVal2.alignment )
                    break;
                 if (( tabVal1.decimal != tabVal2.decimal ) &&
		     ( strcmp( tabVal1.decimal, tabVal2.decimal ) != 0 ))
                       break;
              }
              if ( j != tab_count1 )
                 break;
           }
	   /* else both XmAS_IS */
          
          /*  Compare values of XmNunderlineType resource. */
           if ( rendRes1.underline_type != 
                rendRes2.underline_type )
              break;
         }

         if ( status && ( i != tag_count1 ))
            status = False;

      return( status );
}



void
main( int argc, char **argv )
{
        XmRenderTable  renderTable1, renderTable2;
        int            length;
        char           *prop_return;
        Boolean        status;

 	CommonTestInit( argc, argv );

        renderTable1 = renderTable2 = NULL;

        renderTable1 = CreateRenderTable();

        length =
        XmRenderTableCvtToProp( Shell1, renderTable1, &prop_return );

        printf("XmRenderTableCvtToProp() returns\n");
        printf("String representation= %s\n", prop_return );

        renderTable2 = 
        XmRenderTableCvtFromProp( Shell1, prop_return, length );

       /*
        *  Compare contents of original renderTable with the
        *  contents of the returned renderTable.
        */
        status = CompareRenderTables( renderTable1, renderTable2 );

        printf("XmRenderTableCvtFromProp: ");
        if ( status )
           printf("PASSED");
        else 
           printf("FAILED");
        printf("\n\n");

        CommonPause();
        CommonPause();
        XtAppMainLoop( app_context );
}
