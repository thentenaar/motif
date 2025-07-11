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
static char rcsid[] = "$XConsortium: FileSB.c /main/8 1995/07/14 11:03:19 drk $"
#endif
#endif
/*      @(#)instance	1.2     Date:02/12/91                              */
/*************************************************************************
                                                           
    TEST: XmFileSelectionBox
    AUTHOR: Automatic generation.
    DATE: Wed Oct  9 16:40:15 EDT 1991

       HISTORY:
         12/11/90 PSN Put it under source control. This is version at the
                      time of release.
**************************************************************************/
#include "mvslib.h"
#include <Xm/FileSB.h>


/*************************************************************************/
/*            This file contains WIDGET SPECIFIC DEFINTIONS              */
/*************************************************************************/

/*************************************************************************/
/*      These are the resources associated with this Widget Class        */
/*************************************************************************/

static MvsResourceInfoRecord Wresource_info[] = {

/* The resource_info structure */
{ XmNallowOverlap        , T_Boolean            , False, (MvsArgVal)TRUE                 ,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNallowResize         , T_Boolean            , False, (MvsArgVal)FALSE                ,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNancestorSensitive   , T_Boolean            , True , (MvsArgVal)MVSdynamic           ,
  True,  NULL , 0x0|GET,	False },
{ XmNapplyLabelString    , T_XmString           , False, (MvsArgVal)"Filter"	,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNautoUnmanage        , T_Boolean            , False, (MvsArgVal)FALSE                ,
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
{ XmNcancelLabelString   , T_XmString           , False, (MvsArgVal)"Cancel"	,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNcolormap            , T_Colormap           , True , (MvsArgVal)MVSdynamic           ,
  False, NULL , 0x0|CREATE|GET,	False },
{ XmNdefaultButton       , T_Widget_ref         , True , (MvsArgVal)MVSdynamic           ,
  False, NULL , 0x0|SET|GET,	True },
{ XmNdefaultPosition     , T_Boolean            , False, (MvsArgVal)TRUE                 ,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNdepth               , T_Integer            , True , (MvsArgVal)MVSdynamic           ,
  False, NULL , 0x0|CREATE|GET,	False },
{ XmNdialogStyle         , T_unsigned_char      , False, (MvsArgVal)XmDIALOG_WORK_AREA   ,
  False, NULL                 , 0x0|GET,	False },
{ XmNdialogTitle         , T_XmString           , False, (MvsArgVal)NULL	,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNdialogType          , T_unsigned_char      , True , (MvsArgVal)MVSdynamic           ,
  True,  NULL                 , 0x0|GET,	False },
{ XmNdirListItems        , T_XmStringTable      , True , (MvsArgVal)MVSdynamic           ,
  True,  NULL , 0x0|CREATE|SET|GET,	True },
{ XmNdirListLabelString  , T_XmString           , False, (MvsArgVal)"Directories"	,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNdirMask             , T_XmString_temp      , True , (MvsArgVal)MVSdynamic,
  True,  NULL , 0x0|CREATE|SET|GET,	True },
{ XmNdirSearchProc       , T_XtProc             , True , (MvsArgVal)MVSdynamic           ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNdirSpec             , T_XmString_temp      , True , (MvsArgVal)MVSdynamic,
  True,  NULL , 0x0|CREATE|SET|GET,	True },
{ XmNdirectory           , T_XmString_temp      , True , (MvsArgVal)MVSdynamic,
  True,  NULL , 0x0|CREATE|SET|GET,	True },
{ XmNdirectoryValid      , T_Boolean            , True , (MvsArgVal)MVSdynamic           ,
  False, NULL , 0x0|SET|GET,	False },
{ XmNfileListItemCount   , T_Integer            , True , (MvsArgVal)MVSdynamic           ,
  True,  NULL , 0x0|CREATE|SET|GET,	True },
{ XmNfileListItems       , T_XmStringTable      , True , (MvsArgVal)MVSdynamic           ,
  True,  NULL , 0x0|CREATE|SET|GET,	True },
{ XmNfileListLabelString , T_XmString           , False, (MvsArgVal)"Files"	,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNfileSearchProc      , T_XtProc             , True , (MvsArgVal)MVSdynamic           ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNfileTypeMask        , T_unsigned_char      , False, (MvsArgVal)XmFILE_REGULAR       ,
  False, NULL , 0x0|CREATE|SET|GET,	True },
{ XmNfilterLabelString   , T_XmString           , False, (MvsArgVal)"Filter"	,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNforeground          , T_Pixel              , True , (MvsArgVal)MVSdynamic           ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNheight              , T_Dimension          , True , (MvsArgVal)MVSdynamic           ,
  True,  NULL , 0x0|CREATE|SET|GET,	True },
{ XmNhelpLabelString     , T_XmString           , False, (MvsArgVal)"Help"	,
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
{ XmNlistItemCount       , T_Integer            , True , (MvsArgVal)MVSdynamic           ,
  True,  NULL , 0x0|CREATE|SET|GET,	True },
{ XmNlistItems           , T_XmStringTable      , False, (MvsArgVal)NULL                 ,
  True,  NULL , 0x0|CREATE|SET|GET,	True },
{ XmNlistLabelString     , T_XmString           , False, (MvsArgVal)"Files"	,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNlistUpdated         , T_Boolean            , True , (MvsArgVal)MVSdynamic           ,
  False, NULL , 0x0|SET|GET,	False },
{ XmNlistVisibleItemCount, T_Integer            , True , (MvsArgVal)MVSdynamic           ,
  True,  NULL, 0x0|CREATE|SET|GET,	False },
{ XmNmappedWhenManaged   , T_Boolean            , False, (MvsArgVal)TRUE                 ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNmarginHeight        , T_Dimension          , False, (MvsArgVal)10                   ,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNmarginWidth         , T_Dimension          , False, (MvsArgVal)10                   ,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNminimizeButtons     , T_Boolean            , False, (MvsArgVal)FALSE                ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNmustMatch           , T_Boolean            , False, (MvsArgVal)FALSE                ,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNnavigationType      , T_XmNavigationType   , False, (MvsArgVal)XmTAB_GROUP          ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNnoMatchString       , T_XmString           , False, (MvsArgVal)" [    ] "	,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNnoResize            , T_Boolean            , False, (MvsArgVal)FALSE                ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNokLabelString       , T_XmString           , False, (MvsArgVal)"OK"	,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNpaneMaximum         , T_Integer            , False, (MvsArgVal)1000                 ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNpaneMinimum         , T_Integer            , False, (MvsArgVal)1                    ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNpattern             , T_XmString_temp      , True , (MvsArgVal)MVSdynamic,
  False, NULL , 0x0|CREATE|SET|GET,	True },
{ XmNqualifySearchDataProc, T_XtProc             , True , (MvsArgVal)MVSdynamic           ,
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
{ XmNselectionLabelString, T_XmString           , False, (MvsArgVal)"Selection"	,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNsensitive           , T_Boolean            , False, (MvsArgVal)TRUE                 ,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNshadowThickness     , T_Dimension          , True , (MvsArgVal)MVSdynamic           ,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNshadowType          , T_unsigned_char      , False, (MvsArgVal)XmSHADOW_OUT         ,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNskipAdjust          , T_Boolean            , False, (MvsArgVal)FALSE                ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNstringDirection     , T_XmStringDirection  , True , (MvsArgVal)MVSdynamic           ,
  True,  NULL , 0x0|CREATE|GET,	False },
{ XmNtextAccelerators    , T_XtAccelerators     , True , (MvsArgVal)MVSdynamic           ,
  False, NULL , 0x0|CREATE,	False },
{ XmNtextColumns         , T_short              , True , (MvsArgVal)MVSdynamic           ,
  True,  NULL , 0x0|CREATE|SET|GET,	True },
{ XmNtextFontList        , T_XmFontList         , True , (MvsArgVal)MVSdynamic           ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNtextString          , T_XmString_temp      , True , (MvsArgVal)MVSdynamic,
  True,  NULL , 0x0|SET|GET,	False },
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
{ XmNapplyCallback       , T_XtCallbackList     , False, (MvsArgVal)NULL                 ,
  False, NULL , 0x0|CREATE,	False },
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
{ XmNnoMatchCallback     , T_XtCallbackList     , False, (MvsArgVal)NULL                 ,
  False, NULL , 0x0|CREATE,	False },
{ XmNokCallback          , T_XtCallbackList     , False, (MvsArgVal)NULL                 ,
  False, NULL , 0x0|CREATE,	False },
{ XmNunmapCallback       , T_XtCallbackList     , False, (MvsArgVal)NULL                 ,
  False, NULL , 0x0|CREATE,	False },
{ XmNpathMode       	 , T_XtEnum     	, False, (MvsArgVal)XmPATH_MODE_FULL                 ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNfileFilterStyle	 , T_XtEnum     	, False, (MvsArgVal)XmFILTER_NONE                 ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNdirTextLabelString	 , T_XmString     	, False, (MvsArgVal)NULL                 ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNenableButtonTab     , T_Boolean            , False, (MvsArgVal)FALSE		 ,
  False, NULL       , 0x0|CREATE|SET|GET,       False },

};


/*************************************************************************/
/*      This is the name of the class (for debugging purposes)           */
/*************************************************************************/

static char Widget_class_name[] =
			"XmFileSelectionBox";


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
       tmp_name = "XmFileSelectionBox";
    else
       tmp_name = name;

    widget = XmCreateFileSelectionBox (parent, tmp_name, args, nargs);
    return(widget);

} /* End CreateWidget() */


/*********************************************************************/
/*    This function sets up the widget info structure that contains  */
/* all information that applies to a particular widget class. This   */
/* information is of a static nature and is declared statically in   */
/* this module. However, the class info structure is globally visible*/
/*********************************************************************/


static MvsWidgetClassInfo mvXmFileSelectionBoxWidgetClass = {

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
	{  5, 7, 13, 39, 42, 90, 0, 0, 0, 0 },
  /* short    res_pixmap_ref[10];     */
	{  6, 8, 14, -1, 43, 91, 0, 0, 0, 0 },
  /* short    num_res_colors;         */
	6,

};

MvsWidgetClassInfo *iXmFileSelectionBoxWidgetClass = &mvXmFileSelectionBoxWidgetClass;

