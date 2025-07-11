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
static char rcsid[] = "$XConsortium: TopLS.c /main/8 1995/07/14 11:14:25 drk $"
#endif
#endif
/*      @(#)instance	1.2     Date:02/12/91                              */
/************************************************************************

    TEST: XmTopLevelShell
    AUTHOR: Automatic generation.
    DATE: Wed Oct  9 16:44:40 EDT 1991

       HISTORY:
         12/11/90 PSN Put it under source control. This is version at the
                      time of release.
**************************************************************************/
#include "mvslib.h"
#include <Xm/TopLS.h>


/*************************************************************************/
/*            This file contains WIDGET SPECIFIC DEFINTIONS              */
/*************************************************************************/

/*************************************************************************/
/*      These are the resources associated with this Widget Class        */
/*************************************************************************/

static MvsResourceInfoRecord Wresource_info[] = {

/* The resource_info structure */
{ XmNaccelerators        , T_XtAccelerators     , True , (MvsArgVal)MVSdynamic           ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNallowResize         , T_Boolean            , False, (MvsArgVal)FALSE                ,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNallowShellResize    , T_Boolean            , False, (MvsArgVal)FALSE                ,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNancestorSensitive   , T_Boolean            , True , (MvsArgVal)MVSdynamic           ,
  True,  NULL , 0x0|GET,	False },
{ XmNbackground          , T_Pixel              , True , (MvsArgVal)MVSdynamic           ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNbackgroundPixmap    , T_Pixmap             , False, (MvsArgVal)XmUNSPECIFIED_PIXMAP ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNbaseHeight          , T_Integer            , True , (MvsArgVal)MVSdynamic           ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNbaseWidth           , T_Integer            , True , (MvsArgVal)MVSdynamic           ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNborderColor         , T_Pixel              , True , (MvsArgVal)MVSBlack             ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNborderPixmap        , T_Pixmap             , False, (MvsArgVal)XmUNSPECIFIED_PIXMAP ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNborderWidth         , T_Dimension          , False, (MvsArgVal)1                    ,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNbottomAttachment    , T_unsigned_char      , False, (MvsArgVal)XmATTACH_NONE        ,
  True,  NULL , 0x0|CREATE|SET,	False },
{ XmNbottomOffset        , T_Integer            , False, (MvsArgVal)0                    ,
  True,  NULL , 0x0|CREATE|SET,	False },
{ XmNbottomPosition      , T_Integer            , False, (MvsArgVal)0                    ,
  True,  NULL , 0x0|CREATE|SET,	False },
{ XmNbottomWidget        , T_Widget_ref         , False, (MvsArgVal)NULL                 ,
  False, NULL , 0x0|CREATE|SET,	False },
{ XmNcolormap            , T_Colormap           , True , (MvsArgVal)MVSdynamic           ,
  False, NULL , 0x0|CREATE|GET,	False },
{ XmNdefaultFontList     , T_XmFontList         , True , (MvsArgVal)MVSdynamic           ,
  True,  NULL , 0x0|CREATE,	False },
{ XmNdeleteResponse      , T_unsigned_char      , True , (MvsArgVal)MVSdynamic           ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNdepth               , T_Integer            , True , (MvsArgVal)MVSdynamic           ,
  False, NULL , 0x0|CREATE|GET,	False },
{ XmNgeometry            , T_String             , False, (MvsArgVal)NULL	,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNheight              , T_Dimension          , True , (MvsArgVal)MVSdynamic           ,
  True,  NULL , 0x0|CREATE|SET|GET,	True },
{ XmNheightInc           , T_Integer            , True , (MvsArgVal)MVSdynamic           ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNiconMask            , T_Pixmap             , False, (MvsArgVal)NULL                 ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNiconName            , T_String             , False, (MvsArgVal)NULL	,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNiconPixmap          , T_Pixmap             , False, (MvsArgVal)NULL                 ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNiconWindow          , T_Widget_ref         , False, (MvsArgVal)NULL                 ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNiconX               , T_Integer            , True , (MvsArgVal)MVSdynamic           ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNiconY               , T_Integer            , True , (MvsArgVal)MVSdynamic           ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNiconic              , T_Boolean            , True , (MvsArgVal)MVSdynamic           ,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNinitialResourcesPersistent, T_Boolean            , False, (MvsArgVal)TRUE                 ,
  False, NULL , 0x0|CREATE,	False },
{ XmNinitialState        , T_Integer            , True , (MvsArgVal)MVSdynamic           ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNinput               , T_Boolean            , False, (MvsArgVal)FALSE                ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNinsertPosition      , T_XmFunction         , False, (MvsArgVal)NULL                 ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNkeyboardFocusPolicy , T_unsigned_char      , True , (MvsArgVal)MVSdynamic           ,
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
{ XmNmaxAspectX          , T_Integer            , True , (MvsArgVal)MVSdynamic           ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNmaxAspectY          , T_Integer            , True , (MvsArgVal)MVSdynamic           ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNmaxHeight           , T_Integer            , True , (MvsArgVal)MVSdynamic           ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNmaxWidth            , T_Integer            , True , (MvsArgVal)MVSdynamic           ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNminAspectX          , T_Integer            , True , (MvsArgVal)MVSdynamic           ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNminAspectY          , T_Integer            , True , (MvsArgVal)MVSdynamic           ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNminHeight           , T_Integer            , True , (MvsArgVal)MVSdynamic           ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNminWidth            , T_Integer            , True , (MvsArgVal)MVSdynamic           ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNmwmDecorations      , T_Integer            , True , (MvsArgVal)MVSdynamic           ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNmwmFunctions        , T_Integer            , True , (MvsArgVal)MVSdynamic           ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNmwmInputMode        , T_Integer            , True , (MvsArgVal)MVSdynamic           ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNmwmMenu             , T_String             , False, (MvsArgVal)NULL	,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNoverrideRedirect    , T_Boolean            , True , (MvsArgVal)MVSdynamic           ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNpaneMaximum         , T_Integer            , False, (MvsArgVal)1000                 ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNpaneMinimum         , T_Integer            , False, (MvsArgVal)1                    ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNpopdownCallback     , T_XtCallbackList     , False, (MvsArgVal)NULL                 ,
  False, NULL , 0x0|CREATE,	False },
{ XmNpopupCallback       , T_XtCallbackList     , False, (MvsArgVal)NULL                 ,
  False, NULL , 0x0|CREATE,	False },
{ XmNresizable           , T_Boolean            , False, (MvsArgVal)TRUE                 ,
  True,  NULL , 0x0|CREATE|SET,	False },
{ XmNrightAttachment     , T_unsigned_char      , False, (MvsArgVal)XmATTACH_NONE        ,
  True,  NULL , 0x0|CREATE|SET,	False },
{ XmNrightOffset         , T_Integer            , False, (MvsArgVal)0                    ,
  True,  NULL , 0x0|CREATE|SET,	False },
{ XmNrightPosition       , T_Integer            , False, (MvsArgVal)0                    ,
  True,  NULL , 0x0|CREATE|SET,	False },
{ XmNrightWidget         , T_Widget_ref         , False, (MvsArgVal)NULL                 ,
  False, NULL , 0x0|CREATE|SET,	False },
{ XmNsaveUnder           , T_Boolean            , False, (MvsArgVal)FALSE                ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNscreen              , T_Pointer            , True , (MvsArgVal)MVSdynamic           ,
  False, NULL , 0x0|CREATE|GET,	False },
{ XmNsensitive           , T_Boolean            , False, (MvsArgVal)TRUE                 ,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNshellUnitType       , T_unsigned_char      , True , (MvsArgVal)MVSdynamic           ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNskipAdjust          , T_Boolean            , False, (MvsArgVal)FALSE                ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNtitle               , T_String             , True , (MvsArgVal)MVSdynamic,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNtopAttachment       , T_unsigned_char      , False, (MvsArgVal)XmATTACH_NONE        ,
  True,  NULL , 0x0|CREATE|SET,	False },
{ XmNtopOffset           , T_Integer            , False, (MvsArgVal)0                    ,
  True,  NULL , 0x0|CREATE|SET,	False },
{ XmNtopPosition         , T_Integer            , False, (MvsArgVal)0                    ,
  True,  NULL , 0x0|CREATE|SET,	False },
{ XmNtopWidget           , T_Widget_ref         , False, (MvsArgVal)NULL                 ,
  False, NULL , 0x0|CREATE|SET,	False },
{ XmNtransient           , T_Boolean            , False, (MvsArgVal)FALSE                ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNtranslations        , T_XtTranslations     , True , (MvsArgVal)MVSdynamic           ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNwaitForWm           , T_Boolean            , False, (MvsArgVal)TRUE                 ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNwidth               , T_Dimension          , True , (MvsArgVal)MVSdynamic           ,
  True,  NULL , 0x0|CREATE|SET|GET,	True },
{ XmNwidthInc            , T_Integer            , True , (MvsArgVal)MVSdynamic           ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNwinGravity          , T_Integer            , True , (MvsArgVal)MVSdynamic           ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNwindowGroup         , T_Widget_ref         , True , (MvsArgVal)MVSdynamic           ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNwmTimeout           , T_Integer            , True , (MvsArgVal)MVSdynamic           ,
  False, NULL , 0x0|CREATE|SET|GET,	False },
{ XmNx                   , T_Position           , False, (MvsArgVal)0                    ,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNy                   , T_Position           , False, (MvsArgVal)0                    ,
  True,  NULL , 0x0|CREATE|SET|GET,	False },
{ XmNdestroyCallback     , T_XtCallbackList     , False, (MvsArgVal)NULL                 ,
  False, NULL , 0x0|CREATE,	False },

};


/*************************************************************************/
/*      This is the name of the class (for debugging purposes)           */
/*************************************************************************/

static char Widget_class_name[] =
			"XmTopLevelShell";


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
       tmp_name = "XmTopLevelShell";
    else
       tmp_name = name;

    widget = XmCreateTopLevelShell (parent, tmp_name, args, nargs, xtclass);

    return(widget);

} /* End CreateWidget() */


/*********************************************************************/
/*    This function sets up the widget info structure that contains  */
/* all information that applies to a particular widget class. This   */
/* information is of a static nature and is declared statically in   */
/* this module. However, the class info structure is globally visible*/
/*********************************************************************/


static MvsWidgetClassInfo mvXmTopLevelShellWidgetClass = {

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
	{  4, 8, 0, 0, 0, 0, 0, 0, 0, 0 },
  /* short    res_pixmap_ref[10];     */
	{  5, 9, 0, 0, 0, 0, 0, 0, 0, 0 },
  /* short    num_res_colors;         */
	2,

};

MvsWidgetClassInfo *iXmTopLevelShellWidgetClass = &mvXmTopLevelShellWidgetClass;

