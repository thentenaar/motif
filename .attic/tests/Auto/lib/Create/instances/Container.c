/* $XConsortium: Container.c /main/4 1995/07/15 20:59:10 drk $ */
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

    TEST: XmContainer
    AUTHOR: M. S. Ramesh

************************************************************************/
#include "mvslib.h"
#include <Xm/Container.h>


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
{ XmNautomaticSelection  , T_unsigned_char      , False, (MvsArgVal)XmAUTO_SELECT        ,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNbackground          , T_Pixel              , True , (MvsArgVal)MVSdynamic           ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNbackgroundPixmap    , T_Pixmap             , False, (MvsArgVal)XmUNSPECIFIED_PIXMAP ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
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
{ XmNchildren            , T_Pointer            , False, (MvsArgVal)NULL                 ,
  False, NULL , 0x0|GET,	False },
{ XmNcollapsedStatePixmap, T_Pixmap             , True,  (MvsArgVal)MVSdynamic           ,
  True, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNcolormap            , T_Colormap           , True , (MvsArgVal)MVSdynamic           ,
  False, NULL , 0x0|CREATE|GET,	False },
{ XmNconvertCallback     , T_XtCallbackList     , False, (MvsArgVal)NULL           ,
  False, NULL , 0x0|CREATE,	False },
{ XmNdefaultActionCallback,T_XtCallbackList     , False, (MvsArgVal)NULL                 ,
  False, NULL , 0x0|CREATE,	False },
{ XmNdepth               , T_Integer            , True , (MvsArgVal)MVSdynamic           ,
  False, NULL , 0x0|CREATE|GET,	False },
{ XmNdestinationCallback , T_XtCallbackList     , False, (MvsArgVal)NULL           ,
  False, NULL , 0x0|CREATE,	False },
{ XmNdestroyCallback     , T_XtCallbackList     , False, (MvsArgVal)NULL                 ,
  False, NULL , 0x0|CREATE,	False },
{ XmNdetailColumnHeading , T_XmStringTable      , False, (MvsArgVal)NULL                 ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNdetailOrderCount    , T_Cardinal           , False, (MvsArgVal)0                    ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNdetailTabList       , T_XmTabList          , False, (MvsArgVal)NULL                 ,
  False, NULL , 0x0|CREATE|SET|GET,	False }, 
{ XmNentryParent         , T_Widget_ref		, False, (MvsArgVal)NULL                 ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNentryViewType       , T_unsigned_char      , False, (MvsArgVal)XmANY_ICON         ,
  True , NULL , 0x0|CREATE|SET|GET,	False },
{ XmNexpandedStatePixmap , T_Pixmap             , True,  (MvsArgVal)MVSdynamic           ,
  True, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNforeground          , T_Pixel              , True , (MvsArgVal)MVSdynamic           ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
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
{ XmNinitialResourcesPersistent, T_Boolean      , False, (MvsArgVal)TRUE                 ,
  False, NULL , 0x0|CREATE,	False },
{ XmNinsertPosition      , T_XmFunction         , False, (MvsArgVal)NULL                 ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNlargeCellHeight     , T_Dimension          , False, (MvsArgVal)0           ,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNlargeCellWidth      , T_Dimension          , False, (MvsArgVal)0           ,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNlayoutType          , T_unsigned_char      , False, (MvsArgVal)XmSPATIAL            ,
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
{ XmNoutlineIndentation  , T_Dimension          , False, (MvsArgVal)100                   ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNoutlineLineStyle    , T_unsigned_char      , False, (MvsArgVal)XmSINGLE             ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNprimaryOwnership    , T_unsigned_char      , False, (MvsArgVal)XmOWN_POSSIBLE_MULTIPLE,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNpositionIndex       , T_Integer            , True,  (MvsArgVal)MVSdynamic                    ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNrenderTable         , T_XmRenderTable      , False, (MvsArgVal)NULL           ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNscreen              , T_Pointer            , True , (MvsArgVal)MVSdynamic           ,
  False, NULL , 0x0|CREATE|GET,	False },
{ XmNselectedObjects     , T_WidgetList		, False, (MvsArgVal)NULL                 ,
  False, NULL , 0x0|SET|GET,	False },
{ XmNselectedObjectCount , T_Integer       	, False, (MvsArgVal)0                    ,
  False, NULL , 0x0|SET|GET,	False },
{ XmNselectionCallback   , T_XtCallbackList     , False, (MvsArgVal)NULL                 ,
  False, NULL , 0x0|CREATE,	False },
{ XmNselectionPolicy     , T_unsigned_char      , False, (MvsArgVal)XmEXTENDED_SELECT    ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNselectionTechnique  , T_unsigned_char      , False, (MvsArgVal)XmTOUCH_OVER         ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNsensitive           , T_Boolean            , False, (MvsArgVal)TRUE                 ,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNshadowThickness     , T_Dimension          , False ,(MvsArgVal)0           ,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNsmallCellHeight     , T_Dimension          , False, (MvsArgVal)0           ,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNsmallCellWidth      , T_Dimension          , False, (MvsArgVal)0           ,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNspatialStyle        , T_unsigned_char      , False, (MvsArgVal)XmGRID                ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
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
{ XmNoutlineChangedCallback, T_XtCallbackList   , False, (MvsArgVal)NULL                 ,
  False, NULL , 0x0|CREATE,		False },
{ XmNwidth               , T_Dimension          , True , (MvsArgVal)MVSdynamic           ,
  True,  NULL , 0x0|CREATE|SET|GET,	True },
{ XmNx                   , T_Position           , False, (MvsArgVal)0                    ,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNy                   , T_Position           , False, (MvsArgVal)0                    ,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNdetailColumnHeadingCount , T_Cardinal      , False, (MvsArgVal)0                    ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNdetailOrder	, T_Cardinal		, False, (MvsArgVal)NULL                 ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNoutlineButtonPolicy, T_unsigned_char	, False, (MvsArgVal)XmOUTLINE_BUTTON_PRESENT ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNoutlineColumnWidth	, T_Dimension		, False, (MvsArgVal)0                    ,
  True,  NULL , 0x0|CREATE|SET|GET,	True },
{ XmNselectColor	, T_Pixel		, False, (MvsArgVal)XmREVERSED_GROUND_COLORS           ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNspatialIncludeModel, T_unsigned_char	, False, (MvsArgVal)XmAPPEND             ,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNspatialResizeModel	, T_unsigned_char	, False, (MvsArgVal)XmGROW_MINOR             ,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNspatialSnapModel	, T_unsigned_char	, False, (MvsArgVal)XmNONE             ,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNoutlineState	, T_unsigned_char	, False, (MvsArgVal)XmCOLLAPSED       ,
  True,  NULL , 0x0|CREATE|SET|GET,	False },

};


/*************************************************************************/
/*      This is the name of the class (for debugging purposes)           */
/*************************************************************************/

static char Widget_class_name[] =
			"XmContainer";


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
       tmp_name = "XmContainer";
    else
       tmp_name = name;

    widget = XmCreateContainer (parent, tmp_name, args, nargs);
    return(widget);

} /* End CreateWidget() */


/*********************************************************************/
/*    This function sets up the widget info structure that contains  */
/* all information that applies to a particular widget class. This   */
/* information is of a static nature and is declared statically in   */
/* this module. However, the class info structure is globally visible*/
/*********************************************************************/


static MvsWidgetClassInfo mvXmContainerWidgetClass = {

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
	{  3, 5, 8, -1, -1, 24, 27, 57, 71, 0 },
  /* short    res_pixmap_ref[10];     */
	{  4, 6, 9, 11, 23, -1, 28, 58, -1, 0 },
  /* short    num_res_colors;         */
	9,

};

MvsWidgetClassInfo *iXmContainerWidgetClass = &mvXmContainerWidgetClass;

