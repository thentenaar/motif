/* $XConsortium: Notebook.c /main/4 1995/07/15 20:59:22 drk $ */
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
/************************************************************************

    TEST: XmNotebook
    AUTHOR: M. S. Ramesh

************************************************************************/
#include "mvslib.h"
#include <Xm/Notebook.h>


/*************************************************************************/
/*            This file contains WIDGET SPECIFIC DEFINTIONS              */
/*************************************************************************/

/*************************************************************************/
/*      These are the resources associated with this Widget Class        */
/*************************************************************************/

static MvsResourceInfoRecord Wresource_info[] = {

/* The resource_info structure */
{ XmNaccelerators        , T_XtAccelerators     , False, (MvsArgVal)NULL           ,
  False,  NULL , 0x0|CREATE|SET|GET, False },
{ XmNancestorSensitive   , T_Boolean            , True , (MvsArgVal)MVSdynamic           ,
  True,  NULL , 0x0|GET,	False },
{ XmNbackground          , T_Pixel              , True , (MvsArgVal)MVSdynamic           ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNbackgroundPixmap    , T_Pixmap             , False, (MvsArgVal)XmUNSPECIFIED_PIXMAP ,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNbackPageBackground  , T_Pixel              , True , (MvsArgVal)MVSdynamic           ,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNbackPageForeground  , T_Pixel              , True , (MvsArgVal)MVSdynamic           ,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNbackPageNumber      , T_Cardinal           , False, (MvsArgVal)2                    ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNbackPagePlacement   , T_unsigned_char      , True,  (MvsArgVal)MVSdynamic           ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNbackPageSize        , T_Dimension          , False, (MvsArgVal)8                    ,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNbindingPixmap       , T_Pixmap             , False, (MvsArgVal)XmUNSPECIFIED_PIXMAP ,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNbindingType         , T_unsigned_char      , False, (MvsArgVal)XmSOLID              ,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNbindingWidth        , T_Dimension          , False, (MvsArgVal)25                   ,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNborderColor         , T_Pixel              , True , (MvsArgVal)MVSBlack             ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNborderPixmap        , T_Pixmap             , False, (MvsArgVal)XmUNSPECIFIED_PIXMAP ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNborderWidth         , T_Dimension          , False, (MvsArgVal)0                    ,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNbottomShadowColor   , T_Pixel              , True , (MvsArgVal)MVSdynamic           ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNbottomShadowPixmap  , T_Pixmap             , False, (MvsArgVal)XmUNSPECIFIED_PIXMAP ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNchildType           , T_unsigned_char      , True , (MvsArgVal)MVSdynamic           ,
  False, NULL , 0x0|CREATE|GET,	False },
{ XmNchildren            , T_Pointer            , False, (MvsArgVal)NULL                 ,
  False, NULL , 0x0|GET,	False },
{ XmNcolormap            , T_Colormap           , True , (MvsArgVal)MVSdynamic           ,
  False, NULL , 0x0|CREATE|GET,	False },
{ XmNcurrentPageNumber   , T_Integer            , True , (MvsArgVal)MVSdynamic           ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNdepth               , T_Integer            , True , (MvsArgVal)MVSdynamic           ,
  False, NULL , 0x0|CREATE|GET,	False },
{ XmNdestroyCallback     , T_XtCallbackList     , False, (MvsArgVal)NULL                 ,
  False, NULL , 0x0|CREATE,	False },
{ XmNfirstPageNumber     , T_Integer            , False, (MvsArgVal)1                    ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNforeground          , T_Pixel              , True , (MvsArgVal)MVSdynamic           ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNframeBackground     , T_Pixel              , True , (MvsArgVal)MVSdynamic           ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNframeShadowThickness, T_Dimension          , False, (MvsArgVal)0                    ,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNheight              , T_Dimension          , True , (MvsArgVal)MVSdynamic           ,
  True,  NULL , 0x0|CREATE|SET|GET,	True },
{ XmNhelpCallback        , T_XtCallbackList     , False, (MvsArgVal)NULL                 ,
  False, NULL , 0x0|CREATE,	False },
{ XmNhighlightColor      , T_Pixel              , True , (MvsArgVal)MVSdynamic           ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNhighlightPixmap     , T_Pixmap             , True , (MvsArgVal)MVSdynamic           ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNinitialFocus        , T_Pointer            , False, (MvsArgVal)NULL                 ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNinitialResourcesPersistent, T_Boolean	, False, (MvsArgVal)TRUE                 ,
  False, NULL , 0x0|CREATE,	False },
{ XmNinnerMarginHeight   , T_Dimension          , False, (MvsArgVal)0                    ,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNinnerMarginWidth    , T_Dimension          , False, (MvsArgVal)0                    ,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNinsertPosition      , T_XmFunction         , False, (MvsArgVal)NULL                 ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNlastPageNumber      , T_Integer            , True , (MvsArgVal)MVSdynamic           ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
/*{ XmNlayoutDirection   , T_XmDirection	, False, (MvsArgVal)XmLEFT_TO_RIGHT      ,
  True,  NULL , 0x0|CREATE|SET|GET,	False },*/
{ XmNmajorTabSpacing     , T_Integer		, False, (MvsArgVal)3                    ,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNminorTabSpacing     , T_Integer		, False, (MvsArgVal)3                    ,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNmappedWhenManaged   , T_Boolean            , False, (MvsArgVal)TRUE                 ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNmarginHeight        , T_Dimension          , False, (MvsArgVal)0                   ,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNmarginWidth         , T_Dimension          , False, (MvsArgVal)0                   ,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNnavigationType      , T_XmNavigationType   , False, (MvsArgVal)XmTAB_GROUP          ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNnumChildren         , T_Integer            , False, (MvsArgVal)0                    ,
  False, NULL , 0x0|GET,	False },
{ XmNorientation         , T_unsigned_char      , False, (MvsArgVal)XmHORIZONTAL         ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNpageChangedCallback , T_XtCallbackList     , False, (MvsArgVal)NULL                 ,
  False, NULL , 0x0|CREATE,	False },
{ XmNpageNumber          , T_Integer            , True , (MvsArgVal)MVSdynamic                    ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNpositionIndex       , T_Integer            , False, (MvsArgVal)0                    ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNresizable           , T_Boolean            , False, (MvsArgVal)TRUE                 ,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNscreen              , T_Pointer            , True , (MvsArgVal)MVSdynamic           ,
  False, NULL , 0x0|CREATE|GET,	False },
{ XmNsensitive           , T_Boolean            , False, (MvsArgVal)TRUE                 ,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNshadowThickness     , T_Dimension          , True , (MvsArgVal)MVSdynamic           ,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNstringDirection     , T_XmStringDirection  , True , (MvsArgVal)MVSdynamic           ,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNtopShadowColor      , T_Pixel              , True , (MvsArgVal)MVSdynamic           ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNtopShadowPixmap     , T_Pixmap             , True , (MvsArgVal)MVSdynamic           ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNtranslations        , T_XtTranslations     , True , (MvsArgVal)MVSdynamic           ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNtraversalOn         , T_Boolean            , False, (MvsArgVal)TRUE                 ,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNunitType            , T_unsigned_char      , True , (MvsArgVal)MVSdynamic           ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNuserData            , T_caddr_t            , False, (MvsArgVal)NULL                 ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNvalueChangedCallback, T_XtCallbackList     , False, (MvsArgVal)NULL                 ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNwidth               , T_Dimension          , True , (MvsArgVal)MVSdynamic           ,
  True,  NULL , 0x0|CREATE|SET|GET,	True },
{ XmNx                   , T_Position           , False, (MvsArgVal)0                    ,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNy                   , T_Position           , False, (MvsArgVal)0                    ,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNnotebookChildType   , T_unsigned_char	, True,  (MvsArgVal)MVSdynamic           ,
  False, NULL , 0x0|CREATE|GET,		False },

};


/*************************************************************************/
/*      This is the name of the class (for debugging purposes)           */
/*************************************************************************/

static char Widget_class_name[] =
			"XmNotebook";


/*************************************************************************/
/*    This function is used to create a widget of a particular Widget    */
/* Class. The class is apparent from the name of the convenience         */
/* used for widget creation.                                             */
/*************************************************************************/

static Widget CreateWidget(parent, name, args, nargs, xtclass)
Widget parent;
char *name;
Arg args[];
int nargs;
WidgetClass xtclass;
{
    Widget widget;
    char   *tmp_name;

    if (name == NULL)
       tmp_name = "XmNotebook";
    else
       tmp_name = name;

    widget = XmCreateNotebook (parent, tmp_name, args, nargs);
    return(widget);

} /* End CreateWidget() */


/*********************************************************************/
/*    This function sets up the widget info structure that contains  */
/* all information that applies to a particular widget class. This   */
/* information is of a static nature and is declared statically in   */
/* this module. However, the class info structure is globally visible*/
/*********************************************************************/


static MvsWidgetClassInfo mvXmNotebookWidgetClass = {

  /* Widget                (*proc_SetExpectedActions)(); */ NULL,
  /* void                  (*proc_CreateWidget)();       */ CreateWidget,
  /* void                  (*proc_TestBehavior)();       */ NULL,
  /* char                  class_name[];                 */ Widget_class_name,
  /* MvsResourceInfoRecord *resource_info;               */ Wresource_info,
  /* short                 num_resources;                */
                          sizeof(Wresource_info)/sizeof(MvsResourceInfoRecord),
  /* MvsIntResInfoRecord   *intres_info;  */                NULL,
  /* short                 num_intres;                   */ 
                           0,
  /* Arg                   common_args[20];              */
      { {"",0},{"",0},{"",0},{"",0},{"",0},{"",0},{"",0},{"",0},{"",0},
        {"",0},{"",0},{"",0},{"",0},{"",0},{"",0},{"",0},{"",0},{"",0},
        {"",0},{"",0} },
  /* short                 num_common_args;              */ 0,
  /* short    res_color_ref[10];     */
	{  2, 4, 5, -1, 12, 15, 24, 25, 29, 53 },
  /* short    res_pixmap_ref[10];     */
	{  3, -1, -1, 9, 13, 16, -1, -1, 30, 54 },
  /* short    num_res_colors;         */
	10,

};

MvsWidgetClassInfo *iXmNotebookWidgetClass = &mvXmNotebookWidgetClass;

