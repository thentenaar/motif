/* $XConsortium: IconGadget.c /main/4 1995/07/15 20:59:16 drk $ */
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
/*******************************************************************

    TEST: XmIconGadget
    AUTHOR: M. S. Ramesh
*************************************************************************/

#include "mvslib.h"


/*************************************************************************/
/*            This file contains WIDGET SPECIFIC DEFINTIONS              */
/*************************************************************************/

/*************************************************************************/
/*      These are the resources associated with this Widget Class        */
/*************************************************************************/

static MvsResourceInfoRecord Wresource_info[] = {

/* The resource_info structure */
{ XmNancestorSensitive   , T_Boolean            , True , (MvsArgVal)MVSdynamic           ,
  True,  NULL , 0x0|GET,	False },
{ XmNbackground      	 , T_Pixel              , True , (MvsArgVal)MVSdynamic           ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNbackgroundPixmap   , T_Pixmap              , False, (MvsArgVal)XmUNSPECIFIED_PIXMAP           ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNborderWidth         , T_Dimension          , False, (MvsArgVal)1                    ,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNbottomShadowColor   , T_Pixel              , True , (MvsArgVal)MVSdynamic           ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNbottomShadowPixmap  , T_Pixmap             , False, (MvsArgVal)XmUNSPECIFIED_PIXMAP		 ,
  False, NULL , 0x0|CREATE|SET|GET,     False },
{ XmNdetail		 , T_XmStringTable	, False, (MvsArgVal)NULL                 ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNdetailCount	 , T_Cardinal		, False, (MvsArgVal)0                 ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNforeground      	 , T_Pixel              , True , (MvsArgVal)MVSdynamic		 ,
  False, NULL , 0x0|CREATE|SET|GET,     False },
{ XmNheight              , T_Dimension          , True , (MvsArgVal)MVSdynamic           ,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNhighlightColor      , T_Pixel              , True , (MvsArgVal)MVSdynamic           ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNhighlightOnEnter    , T_Boolean            , False, (MvsArgVal)FALSE                ,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNhighlightPixmap     , T_Pixmap             , True , (MvsArgVal)MVSdynamic		 ,
  False, NULL , 0x0|CREATE|SET|GET,     False },
{ XmNhighlightThickness  , T_Dimension          , False, (MvsArgVal)2                    ,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNlabelString         , T_XmString           , False, (MvsArgVal)NULL           ,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNlargeIcon         , T_String               , True , (MvsArgVal)MVSdynamic           ,
  True,  NULL , 0x0|CREATE,	False },
{ XmNlargeIconMask     , T_Pixmap               , False, (MvsArgVal)XmUNSPECIFIED_PIXMAP           ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNlargeIconPixmap   , T_Pixmap               , False, (MvsArgVal)XmUNSPECIFIED_PIXMAP           ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNnavigationType      , T_XmNavigationType   , False, (MvsArgVal)XmNONE               ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNrenderTable         , T_XmRenderTable      , False, (MvsArgVal)NULL           ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNsensitive           , T_Boolean            , False, (MvsArgVal)TRUE                 ,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNshadowThickness     , T_Dimension          , False, (MvsArgVal)2                    ,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNsmallIcon         , T_String               , True , (MvsArgVal)MVSdynamic           ,
  True,  NULL , 0x0|CREATE,	False },
{ XmNsmallIconMask     , T_Pixmap               , False, (MvsArgVal)XmUNSPECIFIED_PIXMAP           ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNsmallIconPixmap   , T_Pixmap               , False, (MvsArgVal)XmUNSPECIFIED_PIXMAP           ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNtopShadowColor      , T_Pixel              , True , (MvsArgVal)MVSdynamic		 ,
  False, NULL , 0x0|CREATE|SET|GET,     False },
{ XmNtopShadowPixmap     , T_Pixmap             , True , (MvsArgVal)MVSdynamic		 ,
  False, NULL , 0x0|CREATE|SET|GET,     False },
{ XmNtraversalOn         , T_Boolean            , False, (MvsArgVal)TRUE                 ,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNunitType            , T_unsigned_char      , True , (MvsArgVal)MVSdynamic           ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNuserData            , T_caddr_t            , False, (MvsArgVal)NULL                 ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNviewType            , T_unsigned_char	, False, (MvsArgVal)XmLARGE_ICON           ,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNvisualEmphasis      , T_unsigned_char	, False, (MvsArgVal)XmNOT_SELECTED       ,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNwidth               , T_Dimension          , True , (MvsArgVal)MVSdynamic           ,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNx                   , T_Position           , False, (MvsArgVal)0                    ,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNy                   , T_Position           , False, (MvsArgVal)0                    ,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNdestroyCallback     , T_XtCallbackList     , False, (MvsArgVal)NULL                 ,
  False, NULL , 0x0|CREATE,		False },
{ XmNhelpCallback        , T_XtCallbackList     , False, (MvsArgVal)NULL                 ,
  False, NULL , 0x0|CREATE,		False },

};

static char Widget_class_name[] = "XmIconGadget";


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
       tmp_name = "XmIconGadget";
    else
       tmp_name = name;

    widget = XmCreateIconGadget (parent, tmp_name, args, nargs);
    return(widget);

} /* End CreateWidget() */


/*********************************************************************/
/*    This function sets up the widget info structure that contains  */
/* all information that applies to a particular widget class. This   */
/* information is of a static nature and is declared statically in   */
/* this module. However, the class info structure is globally visible*/
/*********************************************************************/


static MvsWidgetClassInfo mvXmIconGadgetClass = {

  /* Widget                (*proc_SetExpectedActions)(); */ NULL,
  /* void                  (*proc_CreateWidget)();       */ CreateWidget,
  /* void                  (*proc_TestBehavior)();       */ NULL,
                                                            Widget_class_name,
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
        {  1, 4, 8, 10, -1, -1, -1, -1, 25, 0 },
  /* short    res_pixmap_ref[10];     */
        {  2, 5, -1, 12, 16, 17, 23, 24, 26, 0 },
  /* short    num_res_colors;         */
	9,

};

MvsWidgetClassInfo *iXmIconGadgetClass = &mvXmIconGadgetClass;


