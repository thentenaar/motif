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
#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$XConsortium: WarningD.c /main/8 1995/07/14 11:14:38 drk $"
#endif
#endif
/*      @(#)instance	1.2     Date:02/12/91                              */
/**********************************************************************

    TEST: XmWarningDialog
    AUTHOR: Automatic generation.
    DATE: Wed Oct  9 16:44:47 EDT 1991

       HISTORY:
         12/11/90 PSN Put it under source control. This is version at the
                      time of release.
**************************************************************************/
#include "mvslib.h"
#include <Xm/MessageB.h>


/*************************************************************************/
/*            This file contains WIDGET SPECIFIC DEFINTIONS              */
/*************************************************************************/

/*************************************************************************/
/*      These are the resources associated with this Widget Class        */
/*************************************************************************/

static MvsResourceInfoRecord Wresource_info[] = {

/* The resource_info structure */
{ XmNallowResize         , T_Boolean            , False, (MvsArgVal)FALSE                ,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNancestorSensitive   , T_Boolean            , True , (MvsArgVal)MVSdynamic           ,
  True,  NULL , 0x0|GET,	False },
{ XmNautoUnmanage        , T_Boolean            , False, (MvsArgVal)TRUE                 ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNbackground          , T_Pixel              , True , (MvsArgVal)MVSdynamic           ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNbackgroundPixmap    , T_Pixmap             , False, (MvsArgVal)XmUNSPECIFIED_PIXMAP ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNborderColor         , T_Pixel              , True , (MvsArgVal)MVSBlack             ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNborderPixmap        , T_Pixmap             , False, (MvsArgVal)XmUNSPECIFIED_PIXMAP ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNborderWidth         , T_Dimension          , False, (MvsArgVal)0                    ,
  True,  NULL , 0x0|GET,	False },
{ XmNbottomAttachment    , T_unsigned_char      , False, (MvsArgVal)XmATTACH_NONE        ,
  True,  NULL , 0x0|CREATE|SET,	False },
{ XmNbottomOffset        , T_Integer            , False, (MvsArgVal)0                    ,
  True,  NULL , 0x0|CREATE|SET,	False },
{ XmNbottomPosition      , T_Integer            , False, (MvsArgVal)0                    ,
  True,  NULL , 0x0|CREATE|SET,	False },
{ XmNbottomShadowColor   , T_Pixel              , True , (MvsArgVal)MVSdynamic           ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNbottomShadowPixmap  , T_Pixmap             , False, (MvsArgVal)XmUNSPECIFIED_PIXMAP ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNbottomWidget        , T_Widget_ref         , False, (MvsArgVal)NULL                 ,
  False, NULL , 0x0|CREATE|SET,	False },
{ XmNbuttonFontList      , T_XmFontList         , True , (MvsArgVal)MVSdynamic           ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNcancelButton        , T_Widget_ref         , True , (MvsArgVal)MVSdynamic           ,
  False, NULL , 0x0|SET|GET,	False },
{ XmNcancelLabelString   , T_XmString           , True,  (MvsArgVal)MVSdynamic	,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNcolormap            , T_Colormap           , True , (MvsArgVal)MVSdynamic           ,
  False, NULL , 0x0|CREATE|GET,	False },
{ XmNdefaultButton       , T_Widget_ref         , True , (MvsArgVal)MVSdynamic           ,
  False, NULL , 0x0|SET|GET,	True },
{ XmNdefaultButtonType   , T_unsigned_char      , False, (MvsArgVal)XmDIALOG_OK_BUTTON   ,
  False, NULL , 0x0|CREATE|SET|GET,	True },
{ XmNdefaultPosition     , T_Boolean            , False, (MvsArgVal)TRUE                 ,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNdepth               , T_Integer            , True , (MvsArgVal)MVSdynamic           ,
  False, NULL , 0x0|CREATE|GET,	False },
{ XmNdialogStyle         , T_unsigned_char      , True , (MvsArgVal)MVSdynamic           ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNdialogTitle         , T_XmString           , False, (MvsArgVal)NULL	,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNdialogType          , T_unsigned_char      , False, (MvsArgVal)XmDIALOG_WARNING     ,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNforeground          , T_Pixel              , True , (MvsArgVal)MVSdynamic           ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNheight              , T_Dimension          , True , (MvsArgVal)MVSdynamic           ,
  True,  NULL , 0x0|CREATE|SET|GET,	True },
{ XmNhelpLabelString     , T_XmString           , True,  (MvsArgVal)MVSdynamic	,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNhighlightColor      , T_Pixel              , True , (MvsArgVal)MVSdynamic           ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNhighlightPixmap     , T_Pixmap             , True , (MvsArgVal)MVSdynamic           ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNinitialResourcesPersistent, T_Boolean            , False, (MvsArgVal)TRUE                 ,
  False, NULL , 0x0|CREATE,	False },
{ XmNinsertPosition      , T_XmFunction         , False, (MvsArgVal)NULL                 ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNlabelFontList       , T_XmFontList         , True , (MvsArgVal)MVSdynamic           ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNleftAttachment      , T_unsigned_char      , False, (MvsArgVal)XmATTACH_NONE        ,
  True,  NULL , 0x0|CREATE|SET,	False },
{ XmNleftOffset          , T_Integer            , False, (MvsArgVal)0                    ,
  True,  NULL , 0x0|CREATE|SET,	False },
{ XmNleftPosition        , T_Integer            , False, (MvsArgVal)0                    ,
  True,  NULL , 0x0|CREATE|SET,	False },
{ XmNleftWidget          , T_Widget_ref         , False, (MvsArgVal)NULL                 ,
  False, NULL , 0x0|CREATE|SET,	False },
{ XmNmappedWhenManaged   , T_Boolean            , False, (MvsArgVal)TRUE                 ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNmarginHeight        , T_Dimension          , False, (MvsArgVal)10                   ,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNmarginWidth         , T_Dimension          , False, (MvsArgVal)10                   ,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNmessageAlignment    , T_unsigned_char      , False, (MvsArgVal)XmALIGNMENT_BEGINNING,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNmessageString       , T_XmString           , False, (MvsArgVal)NULL	,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNminimizeButtons     , T_Boolean            , False, (MvsArgVal)FALSE                ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNnavigationType      , T_XmNavigationType   , False, (MvsArgVal)XmTAB_GROUP          ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNnoResize            , T_Boolean            , False, (MvsArgVal)FALSE                ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNokLabelString       , T_XmString           , True,  (MvsArgVal)MVSdynamic	,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNpaneMaximum         , T_Integer            , False, (MvsArgVal)1000                 ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNpaneMinimum         , T_Integer            , False, (MvsArgVal)1                    ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNresizable           , T_Boolean            , False, (MvsArgVal)TRUE                 ,
  True,  NULL , 0x0|CREATE|SET,	False },
{ XmNresizePolicy        , T_unsigned_char      , False, (MvsArgVal)XmRESIZE_ANY         ,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNrightAttachment     , T_unsigned_char      , False, (MvsArgVal)XmATTACH_NONE        ,
  True,  NULL , 0x0|CREATE|SET,	False },
{ XmNrightOffset         , T_Integer            , False, (MvsArgVal)0                    ,
  True,  NULL , 0x0|CREATE|SET,	False },
{ XmNrightPosition       , T_Integer            , False, (MvsArgVal)0                    ,
  True,  NULL , 0x0|CREATE|SET,	False },
{ XmNrightWidget         , T_Widget_ref         , False, (MvsArgVal)NULL                 ,
  False, NULL , 0x0|CREATE|SET,	False },
{ XmNscreen              , T_Pointer            , True , (MvsArgVal)MVSdynamic           ,
  False, NULL , 0x0|CREATE|GET,	False },
{ XmNsensitive           , T_Boolean            , False, (MvsArgVal)TRUE                 ,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNshadowThickness     , T_Dimension          , True , (MvsArgVal)MVSdynamic           ,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNshadowType          , T_unsigned_char      , False, (MvsArgVal)XmSHADOW_OUT         ,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNskipAdjust          , T_Boolean            , False, (MvsArgVal)FALSE                ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNstringDirection     , T_XmStringDirection  , True , (MvsArgVal)MVSdynamic           ,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNsymbolPixmap        , T_Pixmap             , True , (MvsArgVal)MVSdynamic           ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNtextFontList        , T_XmFontList         , True , (MvsArgVal)MVSdynamic           ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNtextTranslations    , T_XtTranslations     , False, (MvsArgVal)NULL                 ,
  False, NULL , 0x0|CREATE,	False },
{ XmNtopAttachment       , T_unsigned_char      , False, (MvsArgVal)XmATTACH_NONE        ,
  True,  NULL , 0x0|CREATE|SET,	False },
{ XmNtopOffset           , T_Integer            , False, (MvsArgVal)0                    ,
  True,  NULL , 0x0|CREATE|SET,	False },
{ XmNtopPosition         , T_Integer            , False, (MvsArgVal)0                    ,
  True,  NULL , 0x0|CREATE|SET,	False },
{ XmNtopShadowColor      , T_Pixel              , True , (MvsArgVal)MVSdynamic           ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNtopShadowPixmap     , T_Pixmap             , True , (MvsArgVal)MVSdynamic           ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNtopWidget           , T_Widget_ref         , False, (MvsArgVal)NULL                 ,
  False, NULL , 0x0|CREATE|SET,	False },
{ XmNtranslations        , T_XtTranslations     , True , (MvsArgVal)MVSdynamic           ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNtraversalOn         , T_Boolean            , False, (MvsArgVal)TRUE                 ,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNunitType            , T_unsigned_char      , True , (MvsArgVal)MVSdynamic           ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNuserData            , T_caddr_t            , False, (MvsArgVal)NULL                 ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNwidth               , T_Dimension          , True , (MvsArgVal)MVSdynamic           ,
  True,  NULL , 0x0|CREATE|SET|GET,	True },
{ XmNx                   , T_Position           , False, (MvsArgVal)0                    ,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNy                   , T_Position           , False, (MvsArgVal)0                    ,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNcancelCallback      , T_XtCallbackList     , False, (MvsArgVal)NULL                 ,
  False, NULL , 0x0|CREATE,	False },
{ XmNdestroyCallback     , T_XtCallbackList     , False, (MvsArgVal)NULL                 ,
  False, NULL , 0x0|CREATE,	False },
{ XmNfocusCallback       , T_XtCallbackList     , False, (MvsArgVal)NULL                 ,
  False, NULL , 0x0|CREATE,	False },
{ XmNhelpCallback        , T_XtCallbackList     , False, (MvsArgVal)NULL                 ,
  False, NULL , 0x0|CREATE,	False },
{ XmNmapCallback         , T_XtCallbackList     , False, (MvsArgVal)NULL                 ,
  False, NULL , 0x0|CREATE,	False },
{ XmNokCallback          , T_XtCallbackList     , False, (MvsArgVal)NULL                 ,
  False, NULL , 0x0|CREATE,	False },
{ XmNunmapCallback       , T_XtCallbackList     , False, (MvsArgVal)NULL                 ,
  False, NULL , 0x0|CREATE,	False },

};


/*************************************************************************/
/*      This is the name of the class (for debugging purposes)           */
/*************************************************************************/

static char Widget_class_name[] =
			"XmWarningDialog";


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
       tmp_name = "XmWarningDialog";
    else
       tmp_name = name;

    widget = XmCreateWarningDialog (parent, tmp_name, args, nargs);
    return(widget);

} /* End CreateWidget() */


/*********************************************************************/
/*    This function sets up the widget info structure that contains  */
/* all information that applies to a particular widget class. This   */
/* information is of a static nature and is declared statically in   */
/* this module. However, the class info structure is globally visible*/
/*********************************************************************/


static MvsWidgetClassInfo mvXmWarningDialogWidgetClass = {

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
	{  3, 5, 11, 25, 28, 66, -1, 0, 0, 0 },
  /* short    res_pixmap_ref[10];     */
	{  4, 6, 12, -1, 29, 67, 60, 0, 0, 0 },
  /* short    num_res_colors;         */
	7,

};

MvsWidgetClassInfo *iXmWarningDialogWidgetClass = &mvXmWarningDialogWidgetClass;

