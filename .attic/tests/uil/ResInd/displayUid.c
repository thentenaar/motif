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
static char rcsid[] = "$XConsortium: displayUid.c /main/4 1995/07/13 20:38:13 drk $"
#endif
#endif
  /*
   *
   *This a "generic" routine that will display widgets that are
   * created in uil files. This routine assumes that a single
   * Hierarchy will be opened.
   * 
   * Syntax - displayUid root_filename or
   * displayUid (and will ask you for the UID root filename)
   * 
   * The uid file will be opened and the widget realized.
   */
  
  
#include <stdio.h>
  
#define MAX_FILES	10
#define MAX_CALLBACK	32
#define MAX_LEN		255

/*
 *  Set the number of CommonPauses for each uil test
 */

#define ENUMVAL2_CMN_PAUSES  1
#define ENUMVAL3_CMN_PAUSES  1
#define FOR_REF_CMN_PAUSES   1 
#define FOR_REF4_CMN_PAUSES  1
            
#include <testlib.h>
#include <Mrm/MrmAppl.h>    
#include "reasons.h"
  
  /* Callback routines */
  
static void exit_test();
static void Report_Callback();
static void Report_Callback_widget();
static void popup_popup_menu();
static void Change_Parent_Bkgd();
static void ProcessCommandArgs();

static MrmHierarchy	s_MrmHierarchy;
static char		*vec[MAX_FILES];   	
static MrmCode		class;
static char             uidname[MAX_LEN];
static int              num_common_pauses;


static MrmRegisterArg reglist[] = {
  {"exit_test", (caddr_t)exit_test},
  {"Report_Callback", (caddr_t)Report_Callback},
  {"Report_Callback_widget", (caddr_t)Report_Callback_widget},
  {"Change_Parent_Bkgd", (caddr_t)Change_Parent_Bkgd}
};

static int reglist_num = (sizeof (reglist) / sizeof (reglist[0]));
     
static XtActionsRec actions[] = {
  {"popup_popup_menu", (XtActionProc) popup_popup_menu}
};
     
static int num_actions = sizeof(actions) / sizeof(XtActionsRec);
     
     
Widget  TopManager = NULL;
     
/*
 *  Main program
 */
int main(argc, argv)
     int argc;
     char **argv;
     
{
  Arg arglist[1];
  int i;
  
  
  /*
   *  Initialize the Mrm
   */
  
  MrmInitialize ();
  
  /*
   *  Initialize the toolkit.  This call returns the id of the "Shell1"
   *  widget.  The applications "main" widget must be the only child
   *  of this widget.
   */
  
  CommonTestUilInit(argc, argv);
  

  /*
   *  Process command args - put uid filename into uidname and
   *  determine which uil test is being run.
   */

  ProcessCommandArgs();
  
  
  XtAppAddActions(app_context, actions, num_actions);
  
  /*
   *  Define the Mrm hierarchy (only 1 file)
   */
  
  if (MrmOpenHierarchy (1,			    /* number of files	    */
			vec, 			    /* files     	    */
			NULL,			    /* os_ext_list (null)   */
			&s_MrmHierarchy)	    /* ptr to returned id   */
      != MrmSUCCESS) 
    {
      printf ("Can't open hierarchy\n");
      exit(1);
    }
  
  /*
   *		Register all Mrm functions
   */	
  
  
  if (MrmRegisterNames (reglist, reglist_num)
      != MrmSUCCESS)
    {
      printf("Can't register names\n");
      exit(1);
    }
  
  
  
  /*
   *  Call Mrm to fetch the main widget
   */
  
  if (MrmFetchWidget (s_MrmHierarchy,
		      "TopManager",
		      Shell1,
		      &TopManager,
		      &class)
      != MrmSUCCESS)
    {
      printf("Can't fetch interface\n");
      exit(1);
    }
  
  /*
   *  Make the Shell1 widget "manage" the main window (or whatever the
   *  the uil defines as the topmost widget).  This will
   *  cause it to be "realized" when the Shell1 widget is "realized"
   */
  
  XtManageChild(TopManager);
  
  /*
   *  Realize the Shell1 widget.  This will cause the entire "managed"
   *  widget hierarchy to be displayed
   */
  
  XtRealizeWidget(Shell1);
  
  /*
   *  Loop through the correct number CommonPauses for the
   *  particular uil file currently being run.
   */


  for (i = 0; i < num_common_pauses; i++)
    {
      CommonPause();
    }


  CommonPause();   /* Exit */


  /*
   *  Loop and process events
   */

  XtAppMainLoop(app_context);
}

/****************************************************************************/
/*	Procedure exit_test()  - called when user had examined test and     */
/*	is ready to exit test. Called via a callback defined in UIL. 	    */
/****************************************************************************/

static void exit_test( widget, tag, callback_data )
     Widget  widget;
     char    *tag;
     XmAnyCallbackStruct *callback_data;
{
  printf("Test finished -- Exiting....\n");
  exit(0);
}

/***************************************************************************/
/* 	Procedure Report_Callback() -					   */
/*	   when called will print to stdout the name of the callback 	   */
/*	   which is contained in tag. It also prints out the reason	   */
/*	   for the callback. This info is found in the CallbackStruct	   */
/*	   and in the header file reasons.h which contains all possible    */
/*	   Reasons in the array REASONS.				   */
/***************************************************************************/

static void Report_Callback_widget( widget, apply_widget, callback_data )
     Widget  widget;
     Widget  apply_widget;
     XmAnyCallbackStruct *callback_data;
{
  char *reason;
  int reason_num;
  int n;
  Arg args[1];
  
  reason_num = callback_data->reason;
  reason = REASONS[reason_num];
  
  printf("\n");
  printf("/*********************************************************/\n");
  printf("---- %s callback made ----\n\n", "Activate");
  printf("---- Reason was %s ----\n", reason);
  printf("---- Background color of %s ----\n", XtName(apply_widget));
  printf("---- should change to yellow ----\n");
  printf("/*********************************************************/\n");
  printf("\n");
  
  n = 0;
  XtSetArg(args[n], XmNbackground, CommonGetColor("yellow")); n++;
  XtSetValues(apply_widget, args, n);
  
}

/***************************************************************************/
/* 	Procedure Report_Callback() -					   */
/*	   when called will print to stdout the name of the callback 	   */
/*	   which is contained in tag. It also prints out the reason	   */
/*	   for the callback. This info is found in the CallbackStruct	   */
/*	   and in the header file reasons.h which contains all possible    */
/*	   Reasons in the array REASONS.				   */
/***************************************************************************/

static void Report_Callback( widget, tag, callback_data )
     Widget  widget;
     char    *tag;
     XmAnyCallbackStruct *callback_data;
{
  char *reason;
  int reason_num;
  
  reason_num = callback_data->reason;
  reason = REASONS[reason_num];
  
  printf("\n");
  printf("/*********************************************************/\n");
  printf("---- %s callback made ----\n\n", tag);
  printf("---- Reason was %s ----\n", reason);
  printf("/*********************************************************/\n");
  printf("\n");
}

/****************************************************************************/
/*	Procedure fetch() - will extract from a heirarchy the widget ID of  */
/*	the specified widget name.				    	    */
/****************************************************************************/

void fetch(widget_index_string,widgetptr)
     char *widget_index_string;
     Widget *widgetptr;
{
  MrmType class;
  *widgetptr = NULL;
  if(MrmFetchWidget(s_MrmHierarchy,
		    widget_index_string,
		    Shell1,
		    widgetptr,
		    &class)!=MrmSUCCESS)
    printf("fetch failed\n");
}

/***************************************************************************/
/*	Procedure popup_popup_menu() - will popup a popupMenu associated   */
/*	with the widget name Popup_Menu					   */
/***************************************************************************/

static void popup_popup_menu(widget, event, params, num_params)
     Widget                      widget;
     XButtonPressedEvent *       event;
     char **                     params;
     int                         num_params;
{
  static Widget fetched;
  
  fetch("Popup_Menu",&fetched);
  XmMenuPosition(fetched, event);  
  XtManageChild(fetched);
}

static void Change_Parent_Bkgd(widget, tag, callback_data)
     Widget widget;
     char * tag;
     XmAnyCallbackStruct *callback_data;
     
{
  int n;
  Arg args[1];
  static int flag = 0;
  
  if (flag == 0) {
    n = 0;
    XtSetArg (args[n], XmNbackground, CommonGetColor("white")); n++;
    XtSetValues (XtParent(widget), args, n);
    flag = 1;
  } 
  else {
    n = 0;
    XtSetArg (args[n], XmNbackground, CommonGetColor("blue")); n++;
    XtSetValues (XtParent(widget), args, n);
    flag = 0;
  }
  
}


static void ProcessCommandArgs()

{
  char    *user_data;

  user_data = UserData;

  /*
   * Check for uid filename, add uid extension and put into uidname
   */

  if (UserData == NULL)
    {
      printf("Usage: displayUid <uid_filename>\n");
      exit (0);
    }
  else
    {
      strcpy(uidname, UserData);
      strcat(uidname, ".uid");
    }
  printf("/*********************************************************/\n");
  printf("---- The uid file to be opened is %s ---- \n", uidname);
  printf("/*********************************************************/\n\n");
  vec[0] = uidname;

  /*
   *  Set int value to use the correct number of CommonPauses
   *  NOTE: this section will vary from directory to directory
   */
 if (strcmp(UserData, "ForRef") == 0)
    {
      num_common_pauses = FOR_REF_CMN_PAUSES;
    }
  else if (strcmp(UserData, "ForRef4") == 0)
    {
      num_common_pauses = FOR_REF4_CMN_PAUSES;
    }

  else if (strcmp(UserData, "EnumVal2") == 0)
    {
      num_common_pauses = ENUMVAL2_CMN_PAUSES;
    }
  else if (strcmp(UserData, "EnumVal3") == 0)
    {
      num_common_pauses = ENUMVAL3_CMN_PAUSES;
    }
  else
    {
      printf("ERROR: incorrect UserData\n");
      exit (0);
    }
}



