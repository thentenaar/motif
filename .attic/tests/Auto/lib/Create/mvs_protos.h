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
/*   $XConsortium: mvs_protos.h /main/7 1995/07/14 11:26:13 drk $ */
/***********************************************************************
  	@(#)mvs_protos.h	1.31.1.1	Date:1/22/91
  	Author: SJS
	History:
            08/08/90 SJS Create
	Calls:

	Summary:
            This file contains mvslib prototype declarations
************************************************************************/
/**************************************************************************
*                     FUNCTION PROTOTYPE DECLARATIONS                     *
*                                                                         *
*   This file should be included in mvslib.h because it uses type def'n   *
**************************************************************************/

/**************************************************************************
*                             mvslib/behavior                             *
**************************************************************************/

extern void mvsButtonBehaviorSet PARAMS((MvsWidgetClassInfo *widget_class_info,
                                       MvsWidgetInfoRecord *parent_info,
                                       int focus_mode, Arg args[],
				       int n, int i));

extern void mvsBoxBehaviorSet PARAMS((MvsWidgetClassInfo *widget_class_info,
                                       MvsWidgetInfoRecord *parent_info,
                                       int focus_mode, Arg args[],
				       int n, int i));

extern void mvsInitBehavior PARAMS((MvsWidgetClassInfo *widget_class_info,
                                    int *total_cases,
                                    MvsBehavior_Table behavior_array[],
                                    int indexes[], int numBehaviors));

extern void mvsListBehaviorSet PARAMS((MvsWidgetClassInfo *widget_class_info,
                                       MvsWidgetInfoRecord *parent_info,
                                       int focus_mode, Arg args[],
				       int n, int i));

extern void mvsManagerBehaviorSet PARAMS((MvsWidgetClassInfo *widget_class_info,
                                       MvsWidgetInfoRecord *parent_info,
                                       int focus_mode, Arg args[],
				       int n, int i));

extern void mvsPanedBehaviorSet PARAMS((MvsWidgetClassInfo *widget_class_info,
                                       MvsWidgetInfoRecord *parent_info,
                                       int focus_mode, Arg args[],
				       int n, int i));

extern void mvsTempBehaviorSet PARAMS((MvsWidgetClassInfo *widget_class_info,
                                       MvsWidgetInfoRecord *parent_info,
                                       int focus_mode, Arg args[],
				       int n, int i));

extern int CascadeBehavior PARAMS((Widget curr_widget, int rc_type,
				       Widget submenu,
                                       int level, int mode, int focus_mode, char *perm));

extern int GetChildrenList PARAMS((MvsWidgetInfoRecord *widget_info,
				   MvsWidgetInfoRecord ***c_list,
				   char		       ***c_name));

extern int GetTravChildList PARAMS((MvsWidgetInfoRecord *widget_info,
				   MvsWidgetInfoRecord ***c_list,
				   char		       ***c_name));

extern void mvsMenuBarBehaviorSet PARAMS((MvsWidgetClassInfo *widget_class_info,
                                       MvsWidgetInfoRecord *parent_info,
                                       int focus_mode, Arg args[],
				       int n, int i));

extern void mvsRadioBoxBehaviorSet PARAMS((
				       MvsWidgetClassInfo *widget_class_info,
                                       MvsWidgetInfoRecord *parent_info,
                                       int focus_mode, Arg args[],
				       int n, int i));

extern void mvsPopupMenuBehaviorSet PARAMS((
				       MvsWidgetClassInfo *widget_class_info,
                                       MvsWidgetInfoRecord *parent_info,
                                       int focus_mode, Arg args[],
				       int n, int i));

extern void mvsOptionMenuBehaviorSet PARAMS((
				       MvsWidgetClassInfo *widget_class_info,
                                       MvsWidgetInfoRecord *parent_info,
                                       int focus_mode, Arg args[],
				       int n, int i));



/**************************************************************************
*                                mvslib/enums                             *
**************************************************************************/

extern MvsArgVal EnumBoolean PARAMS((int n));
extern MvsArgVal Enumcaddr_t PARAMS((int n));
extern MvsArgVal EnumXtCallbackList PARAMS((int n));
extern MvsArgVal Enumchar PARAMS((int n));
extern MvsArgVal EnumDimension PARAMS((int n));
extern MvsArgVal EnumInteger PARAMS((int n));
extern MvsArgVal EnumPosition PARAMS((int n));
extern MvsArgVal Enumshort PARAMS((int n));
extern MvsArgVal EnumString PARAMS((int n));
extern MvsArgVal EnumXtTranslations PARAMS((int n));
extern MvsArgVal Enumunsigned_char PARAMS((int n));
extern MvsArgVal EnumRadioBChild PARAMS((int n));
extern MvsArgVal EnumOptionMChild PARAMS((int n));
extern MvsArgVal EnumPopupMChild PARAMS((int n));
extern MvsArgVal EnumMenuBChild PARAMS((int n));


/**************************************************************************
*                                mvslib/other                             *
**************************************************************************/

extern void mvsAssertion PARAMS((char *assertion));
extern void mvsFailAssert PARAMS((void));
extern void mvsClearAsserts PARAMS((void));
extern int mvsGetClassCode PARAMS((Widget widget));
extern char *mvsGetClassName PARAMS((Widget widget));
extern char *mvsGetReasonName PARAMS((int reason));
extern char *mvsGetObjectRegionName PARAMS((int object_code));
extern MvsWidgetInfoRecord *mvsWindowToWidgetInfo PARAMS((Window window));
extern MvsWidgetInfoRecord *mvsWidgetToWidgetInfo PARAMS((Widget widget));
extern Widget mvsWindowToWidget PARAMS((Window window));
extern void mvsCleanup PARAMS((void));
extern MvsWidgetInfoRecord *mvsInitRootWidgetInfo PARAMS((void));
extern void mvsSetFocus PARAMS((Widget widget, int object_type, int instance,
                               int focus_mode));
extern char *mvsExtractString PARAMS((XmString in_string, char *resource_name));
extern void mvsProcessArgs PARAMS((int *argc, char **argv));

/* extern void msg ();  variable argument list */


/**************************************************************************
*                              mvslib/geometry                            *
**************************************************************************/

extern void mvsGenerateObjects PARAMS((XisObjectRecord *root_obj));
extern void mvsGetWidgetGeometry PARAMS((Widget widget, int *x, int *y,
                                         int *width, int *height,
                                         int *border_width));

extern XisObjectRecord *mvsGetSubObjects PARAMS((XisObjectRecord *parent_obj, 
                                                Widget widget,
                                                int object_type, int instance,
						int PreDefComposite));


/**************************************************************************
*                             mvslib/resources                            *
**************************************************************************/

extern void mvsCompareResources PARAMS((MvsWidgetInfoRecord *widget_info));

extern MvsArgVal mvsConvertToType PARAMS((MvsArgVal value, int type));

extern void mvsCopyReplace1Arg PARAMS((MvsWidgetClassInfo *widget_class_info,
				Arg orig_args[],int *orig_argcnt, 
                                char new_arg_name[], MvsArgVal new_arg_value)); 

extern void mvsCopyReplaceArgs PARAMS((MvsWidgetClassInfo *widget_class_info,
				Arg orig_args[], Cardinal *orig_argcnt, 
                                Arg new_args[], Cardinal new_arg_cnt)); 

extern MvsArgVal mvsCopyResource PARAMS((MvsWidgetClassInfo *widget_class_info,
                                         MvsArgVal src_resource, int i));

extern void mvsCopyResources PARAMS((MvsWidgetClassInfo *widget_class_info,
                                     MvsArgVal dst_resources[], 
                                     MvsArgVal src_resources[]));

extern int mvsFindCorrespondingResource PARAMS((MvsWidgetClassInfo *wc_info,
                                                int i, char remove_str[],
                                                char add_str[]));

extern void mvsFreeResources PARAMS((MvsWidgetClassInfo *widget_class_info,
                                     MvsArgVal resources[]));

extern void mvsFreeResource PARAMS((MvsWidgetClassInfo *widget_class_info,
                                     MvsArgVal *presource, int i));

extern MvsArgVal mvsGetLocalResource PARAMS((MvsWidgetInfoRecord *widget_info,
                                             char resource_name[]));

extern MvsArgVal mvsGetParentResource PARAMS((MvsWidgetInfoRecord *widget_info,
                                             char resource_name[]));

extern MvsArgVal mvsGetParentIntRes PARAMS((MvsWidgetInfoRecord *widget_info,
                                             char resource_name[]));

extern int mvsGetResourceNum PARAMS((MvsWidgetClassInfo *widget_class_info,
                                     char resource_name[]));

extern int mvsGetResNumInArgs PARAMS((Arg args[], int nargs,
                                     char resource_name[]));

extern void commonLoadResources PARAMS((MvsWidgetInfoRecord *widget_info,
                                     MvsArgVal resources[], 
				     int num_resources, 
				     MvsResourceInfoRecord *resource_info));

extern void mvsLoadResources PARAMS((MvsWidgetInfoRecord *widget_info,
                                     MvsArgVal resources[]));

extern MvsArgVal mvsLoadResource PARAMS((MvsWidgetInfoRecord *widget_info,
                                     char res_name[]));

extern char *mvsResourceValueToString PARAMS((
                                     MvsWidgetClassInfo *widget_class_info,
                                     int i, MvsArgVal value));

extern void mvsSetLocalConstraints PARAMS((MvsWidgetInfoRecord *widget_info, 
                                           MvsArgVal resources[],
                                           ArgList args, Cardinal nargs));

extern void mvsSetLocalStartConstraints PARAMS((
                                           MvsWidgetInfoRecord  *widget_info,
                                           MvsArgVal resources[],
                                           ArgList args, Cardinal nargs));

extern void mvsSetLocalValues PARAMS((MvsWidgetInfoRecord *widget_info,
                                      ArgList args, Cardinal *nargs));

extern void mvsSetResourcesToDefaults PARAMS((MvsWidgetInfoRecord *widget_info,
                                              ArgList args,
                                              Cardinal nargs));

extern void mvsSetIntToDefaults PARAMS((MvsWidgetInfoRecord *widget_info));

extern void mvsSetValues PARAMS((MvsWidgetInfoRecord *widget_info,
                                 ArgList args, Cardinal nargs));

extern void mvsSetValuesNoCheck PARAMS((MvsWidgetInfoRecord *widget_info,
                                        ArgList args, Cardinal nargs));

extern MvsArgVal mvsGetResourceValue PARAMS(( Widget widget, 
                                              char *resource_name,
                                              int type ));

extern MvsArgVal mvsCopyIntRes PARAMS(( MvsWidgetClassInfo *widget_class_info,
					MvsArgVal src_resource, int i ));

extern void mvsCopyIntResources PARAMS(( MvsWidgetClassInfo *widget_class_info,
					MvsArgVal dst_resource[],
					MvsArgVal src_resource[] ));

extern void mvsFreeIntRes PARAMS(( MvsWidgetClassInfo *widget_class_info,
					MvsArgVal *presource, int i ));

extern void mvsFreeIntResources PARAMS(( MvsWidgetClassInfo *widget_class_info,
					MvsArgVal resources[] ));

extern MvsArgVal mvsGetInternalResource PARAMS(( MvsWidgetInfoRecord *widget_info,
					char resource_name[] ));

extern int mvsGetIntResNum PARAMS(( MvsWidgetClassInfo *widget_class_info,
					char resource_name[] ));

extern void mvsSetLocalIntResValues PARAMS(( MvsWidgetInfoRecord *widget_info,
					ArgList args, Cardinal nargs ));

extern int mvsAssignRes PARAMS(( MvsWidgetInfoRecord     *widget_info,
					char dest_res[], char src_res[] ));

extern int mvsAssignVal PARAMS(( MvsWidgetInfoRecord *widget_info, 
					char dest_res[], MvsArgVal val ));

extern int mvsAssignParentVal PARAMS(( MvsWidgetInfoRecord *widget_info, 
					char dest_res[], MvsArgVal val ));

extern int mvsAssignResInWidget PARAMS(( MvsWidgetInfoRecord *widget_info, 
					char widget_res[],
					char dest_res[], char src_res[] ));

extern int mvsAssignValInWidget PARAMS(( MvsWidgetInfoRecord *widget_info, 
					char widget_res[],
					char dest_res[], MvsArgVal val ));

extern int mvsAssignValInParentWidget PARAMS(( MvsWidgetInfoRecord *widget_info,
					char widget_res[],
					char dest_res[], MvsArgVal val ));

extern int mvsAssignValInWidRefWidget PARAMS(( MvsWidgetInfoRecord *widget_info,
					char widget_ref[], char widget_res[],
					char dest_res[], MvsArgVal val ));

extern int mvsIncrOrDecrRes PARAMS(( MvsWidgetInfoRecord *widget_info,
					char dest_res[], char src_res[],
					int opr ));

extern int mvsIncrOrDecrVal PARAMS(( MvsWidgetInfoRecord *widget_info,
					char dest_res[], MvsArgVal src_val,
					int opr ));

extern void mvsIncrRes PARAMS(( MvsWidgetInfoRecord *widget_info, 
					char dest_res[], char src_res[] ));

extern void mvsDecrRes PARAMS(( MvsWidgetInfoRecord *widget_info,
					char dest_res[], char src_res[] ));

extern void mvsTestSetResources PARAMS((MvsWidgetClassInfo *widget_class_info,
                                        MvsWidgetInfoRecord *parent_info));

extern void mvsTestCreateResources PARAMS((
                                        MvsWidgetClassInfo *widget_class_info,
                                        MvsWidgetInfoRecord *parent_info));

extern void mvsSetUpGlobalArgs PARAMS((MvsWidgetClassInfo *widget_class_info));


/**************************************************************************
*                               mvslib/widgets                            *
**************************************************************************/

extern MvsWidgetInfoRecord *mvsAllocWidgetInfo PARAMS((
                                        MvsWidgetClassInfo *widget_class_info,
                                        MvsWidgetInfoRecord *parent_info,
                                        Widget widget                   ));

extern void mvsFreeWidgetInfo PARAMS(( MvsWidgetInfoRecord *widget_info ));
extern void mvsFreeArgs PARAMS(( MvsWidgetClassInfo *widget_class_info,
                                 Arg *args, Cardinal nargs ));

extern MvsWidgetInfoRecord *mvsCreateManageWidget PARAMS((
                                        MvsWidgetClassInfo *widget_class_info,
                                        MvsWidgetInfoRecord *parent_info,
					char *name,
                                        int use_common, Arg args[], 
                                        Cardinal nargs, int manage,
  				        WidgetClass xtclass));

extern MvsWidgetInfoRecord *mvsCreateWidget PARAMS((
                                        MvsWidgetClassInfo *widget_class_info,
                                        MvsWidgetInfoRecord *parent_info,
					char *name,
                                        int use_common, Arg args[], 
                                        Cardinal nargs,
 					WidgetClass xtclass));

extern MvsWidgetInfoRecord *mvsCreateNotManageWidget PARAMS((
                                        MvsWidgetClassInfo *widget_class_info,
                                        MvsWidgetInfoRecord *parent_info,
					char *name,
                                        int use_common, Arg args[], 
                                        Cardinal nargs,
					WidgetClass xtclass));

extern MvsWidgetInfoRecord *mvsSetupNewWidgetInfo PARAMS((
                                     MvsWidgetInfoRecord *parent_info,
                                     Widget widget,
                                     MvsWidgetClassInfo *widget_class_info ));

extern void mvsCreateChildren PARAMS(( MvsWidgetInfoRecord *parent_info ));

extern void mvsDestroyWidget PARAMS((MvsWidgetInfoRecord *widget_info,
                                     int check_callbacks));

extern void SetupMotifClass PARAMS((void));

extern ClassRef *ConvertClass PARAMS((ClassRefEle   ref_val, int ref_typ));


/**************************************************************************
*                              mvslib/callbacks                           *
**************************************************************************/

extern int mvsAchievedCallbackSync PARAMS((void));

extern int mvsAllocCallback PARAMS(( MvsWidgetInfoRecord *widget_info,
                                     int resource_num                 ));

extern void mvsCallbackFunc PARAMS(( Widget w, caddr_t cd1, caddr_t cd2 ));

extern void mvsCheckCallbacksAndVisual PARAMS(( 
                                     MvsWidgetInfoRecord *widget_info ));

extern void mvsCheckCallbacks PARAMS(( void ));

extern void mvsClearCallbacks PARAMS(( void ));

extern void mvsCreateCallbacks PARAMS(( MvsWidgetInfoRecord *widget_info )); 

extern void mvsDeleteCallback PARAMS(( int cb_num, int resource_num ));
extern void mvsDeleteCallbacks PARAMS(( MvsWidgetInfoRecord *widget_info ));

extern void mvsExpect1MoreCallback PARAMS(( MvsWidgetInfoRecord *widget_info,
                                            char *resource_name,
					    int reason ));

extern void mvsExpect1MoreParentCallback PARAMS((
                                            MvsWidgetInfoRecord *widget_info,
                                            char *resource_name,
					    int reason ));

extern void mvsExpect1MoreWidgetCallback PARAMS((
                                            MvsWidgetInfoRecord *widget_info,
                                            char *widget_ref,
                                            char *resource_name,
					    int reason ));

extern void mvsExpect1MoreRefParRefWidgetCallback PARAMS((
                                            MvsWidgetInfoRecord *widget_info,
                                            char *parent_ref,
                                            char *widget_ref,
                                            char *resource_name,
					    int reason ));

extern void mvsExpect1MoreParentRefWidgetCallback PARAMS((
                                            MvsWidgetInfoRecord *widget_info,
                                            char *widget_ref,
                                            char *resource_name,
					    int reason ));

extern void mvsExpectSomeCallback PARAMS(( MvsWidgetInfoRecord *widget_info,
                                            char *resource_name,
					    int reason ));

extern void mvsExpectMultiClickCallback PARAMS((
                                       MvsWidgetInfoRecord *widget_info,
                                       char *resource_name,
				       int reason ));

extern void mvsFreeCallback PARAMS(( int cb_num, int resource_num,
                                     int widget_exists ));

extern void mvsFreeCallbacks PARAMS(( MvsWidgetInfoRecord *widget_info,
                                      int widget_exists ));

extern void mvsInitCallbacks PARAMS(( void ));

extern void mvsUseCallbackInfo PARAMS(( char *routine_name ));


/**************************************************************************
*                              mvslib/specific                            *
**************************************************************************/

extern int mvsIsParentRowColumn PARAMS((MvsWidgetInfoRecord *widget_info));
extern int mvsIsLessThanMin PARAMS((MvsWidgetInfoRecord *widget_info));
extern int mvsIsMoreThanMax PARAMS((MvsWidgetInfoRecord *widget_info));
extern Widget XmCreateApplicationShell PARAMS((Widget parent, char *name,
                                              Arg args[], int nargs,
					      WidgetClass xtclass));
extern Widget XmCreatePopupShell PARAMS((Widget parent, char *name,
                                              Arg args[], int nargs,
					      WidgetClass xtclass));
extern int mvsIsParentDialogShell PARAMS((MvsWidgetInfoRecord *widget_info));
extern MvsWidgetInfoRecord **CreateRCchildren PARAMS((
					MvsWidgetInfoRecord *Parent_info,
				        int     num_children,
					int	level, int flag));
extern MvsWidgetInfoRecord *CreateMenuWidget PARAMS((
					MvsWidgetInfoRecord *Parent_info,
				        int     rc_type,
					int	level,
					int	child_created,
                                        int flag ));
extern void Post1 PARAMS(( Widget w, Widget popup, XEvent *event));
extern void Post2 PARAMS(( Widget w, Widget popup, XEvent *event));
extern ClassRef *ChooseClass PARAMS((int rc_type, int num_children, int level,
				int child_created, Arg cargs[], int *pcn, 
				int flag));

extern int mvsIsSubMenuPosted PARAMS(( MvsWidgetInfoRecord *widget_info));
extern void mvsIncrParentCurrPtr PARAMS(( MvsWidgetInfoRecord *widget_info));
extern void mvsDecrParentCurrPtr PARAMS(( MvsWidgetInfoRecord *widget_info));
extern void AdjustCurrPtr PARAMS(( MvsWidgetInfoRecord *widget_info,
				   int	val));
extern void mvsInitMenuPost PARAMS(( MvsWidgetInfoRecord *widget_info));
extern void mvsInitMenuPostWidget PARAMS(( MvsWidgetInfoRecord *widget_info,
					char	*widget_ref));
extern void mvsSetLevel PARAMS(( MvsWidgetInfoRecord *widget_info));
extern void SetCurrInfo PARAMS(( MvsWidgetInfoRecord *widget_info));
extern void mvsResetLevel PARAMS(( MvsWidgetInfoRecord *widget_info));
extern int mvsIsReducingLevel PARAMS(( MvsWidgetInfoRecord *widget_info));
extern int mvsIsIncreasingLevel PARAMS(( MvsWidgetInfoRecord *widget_info));
extern int mvsIsSameLevel PARAMS(( MvsWidgetInfoRecord *widget_info));
extern int mvsIsUnposted PARAMS(( MvsWidgetInfoRecord *widget_info));
extern void MovePointerTo PARAMS(( Widget widget, int object_code, int instance,
				   int direction));
extern int mvsIsWidgetViewable PARAMS(( MvsWidgetInfoRecord *widget_info));
extern int mvsIsMovingToSubmenu PARAMS(( MvsWidgetInfoRecord *widget_info));

extern void mvsUpdateToggle PARAMS(( MvsWidgetInfoRecord *widget_info));
extern int mvsIsSameToggle PARAMS(( MvsWidgetInfoRecord *widget_info));
extern MvsWidgetInfoRecord *mvsCreateHierarchy PARAMS((
					MvsWidgetClassInfo *widget_class_info,
				        MvsWidgetInfoRecord *parent_info,
					Arg args[], Cardinal nargs));

/* Add other prototypes under the appropriate heading ABOVE THIS LINE */
/* Add other prototypes under the appropriate heading ABOVE THIS LINE */
