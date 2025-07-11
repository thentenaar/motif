/* $XConsortium: List13.c /main/6 1996/06/11 17:25:56 pascale $ */
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
#include <Xm/Display.h>

#define NUM_ITEMS 3
#define MGR_WIDTH 400
#define MGR_HEIGHT 150
#define LIST_WIDTH 3 * MGR_WIDTH / 4 
#define BUTTON_WIDTH MGR_WIDTH / 4

static char *items[] = {
  "Seeing Red", 
  "Green with Envy",
  "Sad and Blue" }; 

static char *tags[] = {
  "first", "second", "third" };

Arg	  args[25];
int       i, n;
Widget    DisplayObj, Manager, List1, ToggleB1, ToggleB2;
XmRendition  render[NUM_ITEMS];
XmString  RendComp0, RendComp1, RendComp2, TextComp, TempString1, 
          TempString2, ListItems[NUM_ITEMS];

/*
 *  Callbacks
 */
void LoadFont( Widget w, XtPointer client_data, XtPointer call_data )
{
    XmDisplayCallbackStruct *cb = (XmDisplayCallbackStruct *)call_data;

    if ( cb->reason == XmCR_NO_FONT )
    {
       printf("XmNnoFontCallback called. \n");

      /*  Assign a font to the rendition. */ 
       n = 0;
       XtSetArg( args[n], XmNfontName, "10x20" ); n++;
       XtSetArg( args[n], XmNfontType, XmFONT_IS_FONT ); n++;
       XmRenditionUpdate( cb->rendition, args, n ); 

    }
}

void LoadRendition( Widget w, XtPointer client_data, XtPointer call_data )
{
    XmDisplayCallbackStruct *cb = (XmDisplayCallbackStruct *)call_data;
    XmRendition             rendition;

    if ( cb->reason == XmCR_NO_RENDITION )
    {
       printf("XmNnoRenditionCallback called. \n");

      /*  Create new rendition with specified tag name. */
       if ( cb->tag )
       {
          n = 0;
          XtSetArg( args[n], XmNrenditionBackground, CommonGetColor("gray")); n++;
          rendition = XmRenditionCreate( List1, cb->tag, args, n );

          cb->render_table = XmRenderTableAddRenditions( cb->render_table,
                          &rendition, 1, XmMERGE_OLD );
	  XmRenditionFree( rendition );
       }
     }
}

void Underline( Widget w, XtPointer client_data, XtPointer call_data )
{
  unsigned char  underline;
  XmRenderTable  RenderTable;
  XmStringTag    *tag_list;
  int            tags;
  XmRendition    rendition;

  /*  Retrieve render table. */
  n = 0;
  XtSetArg( args[n], XmNrenderTable, &RenderTable ); n++;
  XtGetValues( List1, args, n );

  RenderTable = XmRenderTableCopy(RenderTable, NULL, 0); 

    tags = XmRenderTableGetTags( RenderTable, &tag_list );

   /*  Update Renditions with new values for XmNunderlineType resource. */
    for ( i = 0; i < tags; i++ )
    {
       rendition = XmRenderTableGetRendition( RenderTable, tag_list[i] );  

       n = 0;
       XtSetArg( args[n], XmNunderlineType, &underline ); n++;
       XmRenditionRetrieve( rendition, args, n );

       if (( underline == XmAS_IS ) || ( underline == XmNO_LINE ))
            underline = XmSINGLE_LINE;

       else if ( underline == XmSINGLE_LINE )
             underline = XmNO_LINE;

       n = 0; 
       XtSetArg( args[n], XmNunderlineType, underline ); n++;
       XmRenditionUpdate( rendition, args, n );

       RenderTable = XmRenderTableAddRenditions( RenderTable, &rendition,
                       1, XmMERGE_REPLACE );
    }


   /*  Update RenderTable for List. */
    n = 0;
    XtSetArg( args[n], XmNrenderTable, RenderTable ); n++;
    XtSetValues( List1, args, n );

    XmRenderTableFree(RenderTable);
}
          
void StrikeThru( Widget w, XtPointer client_data, XtPointer call_data )
{
   unsigned char  strikethru;
   XmRenderTable  RenderTable = NULL;
   XmStringTag    *tag_list;
   int            tags;
   XmRendition    rendition;

  /*  Retrieve render table. */
  n = 0;
  XtSetArg( args[n], XmNrenderTable, &RenderTable ); n++;
  XtGetValues( List1, args, n );

  RenderTable = XmRenderTableCopy(RenderTable, NULL, 0); 

  tags = XmRenderTableGetTags( RenderTable, &tag_list );

    for ( i = 0; i < tags; i++ )
    {
       rendition = XmRenderTableGetRendition( RenderTable, tag_list[i] );  

       n = 0;
       XtSetArg( args[n], XmNstrikethruType, &strikethru ); n++;
       XmRenditionRetrieve( rendition, args, n );

       if (( strikethru == XmAS_IS ) || ( strikethru == XmNO_LINE ))
            strikethru = XmSINGLE_DASHED_LINE;

       else if ( strikethru == XmSINGLE_DASHED_LINE )
             strikethru = XmNO_LINE;

       n = 0;
       XtSetArg( args[n], XmNstrikethruType, strikethru ); n++;
       XmRenditionUpdate( rendition, args, n );

       RenderTable = XmRenderTableAddRenditions( RenderTable, &rendition,
                       1, XmMERGE_REPLACE );
    }

   /*  Update RenderTable for List. */
    n = 0;
    XtSetArg( args[n], XmNrenderTable, RenderTable ); n++;
    XtSetValues( List1, args, n );

    XmRenderTableFree(RenderTable);
}

void main( argc, argv )
int	argc;
char	**argv;
{
   XmRenderTable  RenderTable = NULL;
   XmRendition    Renditions[NUM_ITEMS + 1];

	CommonTestInit( argc, argv );

        DisplayObj = XmGetXmDisplay( display );

        XtAddCallback( DisplayObj, XmNnoFontCallback, LoadFont, NULL );
        XtAddCallback( DisplayObj, XmNnoRenditionCallback, 
                                      LoadRendition, NULL );

        n = 0;
        XtSetArg( args[n], XmNwidth, MGR_WIDTH ); n++;
        XtSetArg( args[n], XmNheight, MGR_HEIGHT ); n++;
        Manager = XmCreateForm( Shell1, "Manager", args, n );
        XtManageChild(Manager);

        n = 0;
        XtSetArg( args[n], XmNtopAttachment, XmATTACH_FORM ); n++;
        XtSetArg( args[n], XmNleftAttachment, XmATTACH_FORM ); n++;
        XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM ); n++;
        XtSetArg( args[n], XmNwidth, LIST_WIDTH ); n++;
        XtSetArg( args[n], XmNlistSizePolicy, XmCONSTANT ); n++;
        XtSetArg( args[n], XmNvisibleItemCount, NUM_ITEMS ); n++;
        List1 = XmCreateList( Manager, "List1", args, n );
        XtManageChild(List1);

        n = 0;
        XtSetArg( args[n], XmNrenditionForeground, 
                  CommonGetColor("Red")); n++;
        Renditions[0] = XmRenditionCreate( List1, 
                                           (XmStringTag)tags[0],
                                           args, n );

        n = 0;
        XtSetArg( args[n], XmNrenditionForeground, 
                  CommonGetColor("Green")); n++;
        Renditions[1] = XmRenditionCreate( List1, 
                                           (XmStringTag)tags[1],
                                           args, n );
 
        n = 0;
        XtSetArg( args[n], XmNrenditionForeground, 
                           CommonGetColor("Blue")); n++;
        Renditions[2] = XmRenditionCreate( List1, 
                                           (XmStringTag)tags[2],
                                           args, n );

        RenderTable = XmRenderTableAddRenditions( RenderTable, 
                        Renditions, NUM_ITEMS, XmMERGE_REPLACE );

	for ( i = 0; i < NUM_ITEMS; i++ )
            XmRenditionFree( Renditions[i] );
  
        n = 0;
        XtSetArg( args[n], XmNrenderTable, RenderTable ); n++;
        XtSetValues( List1, args, n );

	XmRenderTableFree( RenderTable );

        /* 
         *  Use a rendition component in XmString which does
         *  not have a matching render_tag in the render table.
         *  This should result in the XmNnoRenditionCallback 
         *  getting called.
         */
         RendComp0 = XmStringComponentCreate(
                       XmSTRING_COMPONENT_RENDITION_BEGIN,
                       strlen( "Background" ), "Background" );
 

	/*  Create strings for List widget using the renditions. */
        for ( i = 0; i < NUM_ITEMS; i++ )
        {
            RendComp1 = XmStringComponentCreate( 
                          XmSTRING_COMPONENT_RENDITION_BEGIN,
                          strlen( tags[i] ), tags[i] );
            RendComp2 = XmStringComponentCreate( 
                          XmSTRING_COMPONENT_RENDITION_END,
                          strlen( tags[i] ), tags[i] );
            TextComp = XmStringComponentCreate( XmSTRING_COMPONENT_TEXT,
                          strlen( items[i] ), items[i] );
            if ( i == 0 )
            {
                 TempString2 = XmStringConcat( RendComp0, RendComp1 );
                 TempString1 = XmStringConcat( TempString2, TextComp );
                 XmStringFree( TempString2 );
            }
            else
                 TempString1 = XmStringConcat( RendComp1, TextComp );

            ListItems[i] = XmStringConcat( TempString1, RendComp2 );

            XmStringFree( RendComp1 );
            XmStringFree( RendComp2 );
            XmStringFree( TextComp );
            XmStringFree( TempString1 );
         }

        n = 0;
        XtSetArg( args[n], XmNitemCount, NUM_ITEMS ); n++;
        XtSetArg( args[n], XmNitems, ListItems ); n++;
        XtSetValues( List1, args, n );
        XtManageChild(List1);


       /*
        *  Create pushbutton for changing status of underline resource
        *  in renditions.
        */
        TempString1 = XmStringCreateLtoR("Underline",
                       XmFONTLIST_DEFAULT_TAG );

        n = 0;
        XtSetArg( args[n], XmNtopAttachment, XmATTACH_FORM ); n++;
        XtSetArg( args[n], XmNleftAttachment, XmATTACH_WIDGET ); n++;
        XtSetArg( args[n], XmNleftWidget, List1 ); n++;
        XtSetArg( args[n], XmNrightAttachment, XmATTACH_FORM ); n++;
        XtSetArg( args[n], XmNlabelString, TempString1 ); n++;
        ToggleB1 = XmCreateToggleButton( Manager, "ToggleB1", args, n );
        XtManageChild(ToggleB1);

        XtAddCallback( ToggleB1, XmNvalueChangedCallback, Underline, NULL ); 
        XmStringFree(TempString1);

       /*
        *  Create pushbutton for changing status of strikethru resource
        *  in renditions.
        */
        TempString1 = XmStringCreateLtoR("StrikeThru",
                       XmFONTLIST_DEFAULT_TAG );

        n = 0;
        XtSetArg( args[n], XmNtopAttachment, XmATTACH_WIDGET ); n++;
        XtSetArg( args[n], XmNtopWidget, ToggleB1 ); n++;
        XtSetArg( args[n], XmNleftAttachment, XmATTACH_WIDGET ); n++;
        XtSetArg( args[n], XmNleftWidget, List1 ); n++;
        XtSetArg( args[n], XmNrightAttachment, XmATTACH_FORM ); n++;
        XtSetArg( args[n], XmNlabelString, TempString1 ); n++;
        ToggleB2 = XmCreateToggleButton( Manager, "ToggleB2", args, n );
        XtManageChild(ToggleB2);

        XtAddCallback( ToggleB2, XmNvalueChangedCallback, StrikeThru, NULL ); 
        XmStringFree(TempString1);

        XtRealizeWidget(Shell1);

        CommonPause();
        CommonPause();

        XtAppMainLoop(app_context);
}
