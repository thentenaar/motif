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
static char rcsid[] = "$TOG: SharedTearM1.c /main/7 1999/05/19 11:21:49 jff $"
#endif
#endif
#include <testlib.h>

#include <malloc.h>

#include <X11/IntrinsicP.h>
#include <X11/CoreP.h>
#include <X11/CompositeP.h>

#include <Xm/Xm.h>
#include <Xm/CascadeB.h>
#include <Xm/CascadeBG.h>
#include <Xm/Label.h>
#include <Xm/LabelG.h>
#include <Xm/MainW.h>
#include <Xm/MenuShell.h>
#include <Xm/MenuShellP.h>
#include <Xm/PushB.h>
#include <Xm/PushBG.h>
#include <Xm/RowColumn.h>
#include <Xm/RowColumnP.h>
#include <Xm/Separator.h>
#include <Xm/TearOffP.h>
#include <Xm/ToggleB.h>
#include <Xm/ToggleBG.h>

#include <X11/Xutil.h>

/* for _XmAllowAcceleratedInsensitiveUnmanatedMenuItems() */
#include <Xm/RowColumnI.h>

#define NPOP 16
Widget popup[NPOP];
Widget mw;
Widget mb;
Widget mb_cb1, mb_cb2, mb_cb3;
Widget pd1, pd2, pd3, pd4, pd5;
Widget pd1_lbl, pd1_sep, pd1_pb1, pd1_pb2, pd1_tb1, pd1_tb2, pd1_cb1, pd1_cb2;
Widget pd2_pb1, pd2_pb2;
Widget pd3_tb1, pd3_tb2;
Widget pd4_pb1, pd4_pb2;
Widget pd5_tb1, pd5_tb2;
Widget wa;
Widget wa_om;
Widget action_wa;
Widget action_reset, action_test;
Widget wa_pb1, wa_pb2, wa_pb3, wa_pb4, wa_pb5, wa_pb6, wa_pb7,
       wa_pb8, wa_pb9, wa_pb10, wa_pb11, wa_pb12, wa_pb13, wa_pb14,
       wa_pb15, wa_pb16, wa_pb17, wa_pb18;
Widget pop = NULL;
Widget pop_lbl, pop_pb1, pop_pb2, pop_cb1, pop_cb2;

typedef struct _ButtonDataRec {
	Widget context;
	Widget button;
	Widget menu_item;
	Boolean sensitive;
} ButtonDataRec, *ButtonData;

ButtonDataRec * ButtonDataList = NULL;
short nButtonData;

typedef struct _ToggleStateRec {
	Widget context;
	Widget menu_item;
	Boolean state;
} ToggleStateRec, *ToggleState;

ToggleStateRec * ToggleStateList = NULL;
short nToggleState;

#define TEST_MAP_CB 0
#define TEST_CASCADE_CB 1

static int num_popups = 3;
static int handle_warnings = FALSE;
static int ignore_warnings = FALSE;
static int test_type = TEST_MAP_CB;
static int quiet = FALSE;

#define TEST_MAP_CB 0
#define TEST_CASCADE_CB 1
#define LAST_TEST_TYPE 1

/* 
 * Some Utility routines
 */

/* number of toplevels */
#define NFLAG "-nt"
/* use our my warning handler */
#define WFLAG "-mw"
/* ignore (supress) warnings */
#define IFLAG "-ig"
/* test type -tt<#> to specify cascading or rowcolumn-map callback, default 0 */
#define TFLAG "-tt"
/* Don't print info */
#define QFLAG "-qt"

void
get_options(unsigned int argc, char **argv)
{
   int i;

   for(i=0; i<argc; i++)
   {
	if ( argv[i] != NULL ) { /* Some arguments can be replaced with NULLs */
				/* in CommonTestInit		*/		
      if(!strncmp(NFLAG,argv[i], strlen(NFLAG)))
      {
        num_popups = atoi(&(argv[i][strlen(NFLAG)])) - 1;
	if (num_popups > NPOP)
	   num_popups = NPOP;
      } else
      if(!strncmp(TFLAG,argv[i], strlen(TFLAG)))
      {
        test_type = atoi(&(argv[i][strlen(TFLAG)]));
        if ((test_type < 0) || (test_type > LAST_TEST_TYPE))
           test_type = 0;
      } else
      if(!strncmp(WFLAG,argv[i], strlen(WFLAG)))
      {
        handle_warnings = TRUE;
      } else
      if(!strncmp(IFLAG,argv[i], strlen(IFLAG)))
      {
        ignore_warnings = TRUE;
        handle_warnings = TRUE;
      } else
      if(!strncmp(QFLAG,argv[i], strlen(QFLAG)))
      {
        quiet = TRUE;
      }
	}
   }
}

void 
MyWarningHandler(char *message)
{
   if (!ignore_warnings && !quiet)
      printf("MyWarningHandler: %s\n", message);
}

char *ButtonLabel(Widget button)
{
    static char ret_str[128];
    unsigned char label_type;
    XmString label_xmstr;
    XmStringContext strContext;
    char *text;
    XmStringCharSet c;
    XmStringDirection d;
    Boolean s;
    Arg args[2];

    XtSetArg(args[0], XmNlabelType, &label_type);
    XtSetArg(args[1], XmNlabelString, &label_xmstr);
    XtGetValues(button, args, 2);

    ret_str[0] = '\0';
    if (label_type == XmSTRING)
    {
       XmStringInitContext(&strContext, label_xmstr);
       while (XmStringGetNextSegment(strContext, &text, &c, &d, &s))
          strcat(ret_str, text);
    }

    return(ret_str);
}

/*
 * This is by no means an example that should be used to track *real*
 * application menu_item sensitivity.  For this test, it's a simplisitic
 * approach that's quick and dirty.
 */
void
update_submenu_state(Widget context, XmRowColumnWidget rc)
{
   Arg args[15];
   int n;
   unsigned char rc_type;
   Widget menu_item;
   int i, j;

   if (XmIsRowColumn(rc))
   {
      n = 0;
      XtSetArg(args[n], XmNrowColumnType, &rc_type); n++;
      XtGetValues((Widget)rc, args, n);

      if (context &&
	  (rc_type == XmMENU_PULLDOWN) || (rc_type == XmMENU_POPUP))
      {
	 for(i=0; i<rc->composite.num_children; i++)
	 {
	    menu_item = rc->composite.children[i];

	    for(j=0; j<nButtonData; j++)
	    {
	       if ((context == ButtonDataList[j].context) &&
		   (menu_item == ButtonDataList[j].menu_item))
		  XtSetSensitive(menu_item, ButtonDataList[j].sensitive);
	    }

            if(XmIsToggleButton(menu_item) || XmIsToggleButtonGadget(menu_item))
	    {
	       for(j=0; j<nToggleState; j++)
	       {
		  if ((context == ToggleStateList[j].context) &&
		      (menu_item == ToggleStateList[j].menu_item))
		     XmToggleButtonSetState(menu_item, 
			ToggleStateList[j].state, False);
	       }
	    }
	 }
      }
   }
}

Widget
FindTopMostShell(Widget w)
{
   while(w && !XtIsShell(w))
      w = XtParent(w);
   
   return(w);
}

#ifndef TEST_TORN_ACCEL_CONTEXT_SHARED
/* Although we're only testing sensitivity, managed applies as well! */
Boolean
SensitiveManagedVerified(Widget context, Widget menu_item)
{
   int i;

   if (!XtIsManaged(menu_item))
      return(False);

   for(i=0; i<nButtonData; i++)
   {
      if ((context == ButtonDataList[i].context) &&
	  (menu_item == ButtonDataList[i].menu_item))
	 return (ButtonDataList[i].sensitive);
   }
}
#endif

/* 
 * Popup event handlers
 */

/*
 * Context state is NOT updated in PostIt because this routine is not called
 * for the Popup menu accelerator (F4).  So context is dealt with in
 * MenuShell's XmNpopupCallback.
 *
 * If no menu accelerator is defined for Popup, then it's ok to use this
 * routine to set context (test_type == TEST_CASCADE_CB).  In our case we 
 * would use the topmost shell of 'w' as our context.  It's the same as the 
 * postedFromWidget.
 *
 * FYI: In pre Motif1.2, you cannot call XmGetPostedFromWidget until after
 * the call to XmMenuPosition().
 */
void
PostIt(Widget w, Widget popup, XEvent *event, Boolean *continue_to_dispatch)
{
   Arg args[4];
   Boolean popupEnabled;
   Widget toplevel;

   if (popup == NULL)
      return;

   XtSetArg(args[0], XmNpopupEnabled, &popupEnabled);
   XtGetValues(popup, args, 1);

   if (popupEnabled)
   {
      XmMenuPosition(popup, (XButtonPressedEvent*) event);
      XtManageChild(popup);
   }

   *continue_to_dispatch = !XtIsManaged(popup);
}

void
PrintInfo(char *wid_name, char *btn_name, char *cb, char *cb_value, 
	  char *postedFrom, char *toplevel)
{
   static int cnt = 0;
   char s1max[32], s2max[32], s3max[32], s4max[32];

   if (quiet) return;

   if (!(cnt++ % 20))
   {
      printf("\n%-8s %-16s %-16s %4s %-12s %s\n", 
         "Name", "Button Label", "Callback Reason", "CBvl", "PstFrmWdgt",
	 "Toplevel Shell");
      printf("%-8s %-16s %-16s %4s %-12s %s\n", 
         "----", "------------", "---------------", "----", "---------",
	 "--------------");
   }

   strncpy(s1max, wid_name, 8); s1max[8] = '\0';
   strncpy(s2max, btn_name, 16); s2max[16] = '\0';
   strncpy(s3max, cb, 16); s3max[16] = '\0';
   strncpy(s4max, toplevel, 18); s4max[18] = '\0';
   printf("%-8s %-16s %-18s (%2s) %-10s %s\n",
      s1max, s2max, s3max, cb_value, postedFrom, s4max);
}

/*
 * Callbacks
 */

void 
ResetMenuCB(Widget w, caddr_t clientData, caddr_t callData)
{
   int i;
   Widget toplevel;
   Widget menu_item;

   /* ResetMenuCB() was called from a button in a workarea - NOT a menu.
    * so toplevel is simply the topmost shell in the parent heirarchy.
    */
   toplevel = FindTopMostShell(w);

   if (!quiet)
      printf ("Resetting sensitivity and toggle state for menus in toplevel: %s\n",
	 XrmQuarkToString(toplevel->core.xrm_name));

   for(i=0; i<nButtonData; i++)
   {
      if (toplevel == ButtonDataList[i].context)
      {
	 menu_item = ButtonDataList[i].menu_item;
	 ButtonDataList[i].sensitive = TRUE;

	 /* If the parent pane is torn, only change the sensitivity
	  * immeditately if the toplevels match.  Otherwise, the next 
	  * post/map will update sensitivity appropriately.
	  */
	 if (!XmIsMenuShell(XtParent(XtParent(menu_item))))
	 {
	    Widget tearoff_toplevel =
	       FindTopMostShell(XmGetPostedFromWidget(XtParent(menu_item)));

	    if (tearoff_toplevel == ButtonDataList[i].context)
	    {
	       XtSetSensitive(menu_item, ButtonDataList[i].sensitive);
	       if (XmIsToggleButton(menu_item) || 
		  XmIsToggleButtonGadget(menu_item))
	       {
		  XmToggleButtonSetState(menu_item, False, False);
	       }
	    }
	 }
      }
   }
   for(i=0; i<nToggleState; i++)
      ToggleStateList[i].state = False;
}

void 
TestAccCB (Widget w, caddr_t clientData, caddr_t callData)
{
   Arg args[8];
   int n;

   if (!quiet)
      printf("Changing accelerator of pd1_pb1 to Ctrl-Z\n");
   XtSetArg(args[0], XmNaccelerator, "Ctrl<Key>Z");
   XtSetValues(pd1_pb1, args, 1);
}

void 
TestOpaqueCB(Widget w, caddr_t clientData, caddr_t callData)
{
   Arg args[8];
   int n;
   Widget xmscreen;
   Boolean opaque;

   xmscreen = (Widget) XmGetXmScreen(screen);
   n = 0;
   XtSetArg(args[n], XmNmoveOpaque, &opaque); n++;
   XtGetValues(xmscreen, args, n);
   opaque = !opaque;

   if (!quiet)
      if (opaque)
         printf("Setting MoveOpaque to True\n");
      else
         printf("Setting MoveOpaque to False\n");

   n = 0;
   XtSetArg(args[n], XmNmoveOpaque, opaque); n++;
   XtSetValues(xmscreen, args, n);
}

void 
CascadingCB (Widget w, caddr_t clientData, caddr_t callData)
{
   Arg args[15];
   int n;
   char *s;
   Widget postedFromWidget, toplevel;
   Widget subMenuId;

   s = ButtonLabel(w);
   postedFromWidget = XmGetPostedFromWidget(XtParent(w));
   toplevel = FindTopMostShell(postedFromWidget);

   PrintInfo(
      XrmQuarkToString(w->core.xrm_name), 
      s,
      "CASCADING",     
      "",
      XrmQuarkToString(postedFromWidget->core.xrm_name),
      XrmQuarkToString(toplevel->core.xrm_name));

   if (test_type == TEST_CASCADE_CB)
   {
      XtSetArg(args[0], XmNsubMenuId, &subMenuId);
      XtGetValues(w, args, 1);

      update_submenu_state(toplevel, (XmRowColumnWidget)subMenuId);
   }
}

void 
ArmCB(Widget w, caddr_t clientData, caddr_t callData)
{
   char *s;
   Widget postedFromWidget, toplevel;
   char cr_value[32];
   Boolean is_sensitive_managed;

   s = ButtonLabel(w);
   postedFromWidget = XmGetPostedFromWidget(XtParent(w));
   toplevel = FindTopMostShell(postedFromWidget);

#ifndef TEST_TORN_ACCEL_CONTEXT_SHARED
   if (!(is_sensitive_managed = SensitiveManagedVerified(toplevel, w)))
      strcpy(cr_value, "ignore arm");
   else
#endif
   strcpy(cr_value, "ARM");

   PrintInfo(
      XrmQuarkToString(w->core.xrm_name), 
      s,
      cr_value,
      "",
      XrmQuarkToString(postedFromWidget->core.xrm_name),
      XrmQuarkToString(toplevel->core.xrm_name));
}

void 
DisarmCB (Widget w, caddr_t clientData, caddr_t callData)
{
   char *s;
   Widget postedFromWidget, toplevel;
   char cr_value[32];
   Boolean is_sensitive_managed;

   s = ButtonLabel(w);
   postedFromWidget = XmGetPostedFromWidget(XtParent(w));
   toplevel = FindTopMostShell(postedFromWidget);

#ifndef TEST_TORN_ACCEL_CONTEXT_SHARED
   if (!(is_sensitive_managed = SensitiveManagedVerified(toplevel, w)))
      strcpy(cr_value, "ignore disarm");
   else
#endif
   strcpy(cr_value, "DISARM");

   PrintInfo(
      XrmQuarkToString(w->core.xrm_name), 
      s,
      cr_value,
      "",
      XrmQuarkToString(postedFromWidget->core.xrm_name),
      XrmQuarkToString(toplevel->core.xrm_name));
}

void 
ActivateCB (Widget w, caddr_t clientData, XmPushButtonCallbackStruct pb_cbs)
{
   char *s;
   Widget postedFromWidget, toplevel;
   char cr_value[32];
   Boolean is_sensitive_managed;

   s = ButtonLabel(w);
   postedFromWidget = XmGetPostedFromWidget(XtParent(w));
   toplevel = FindTopMostShell(postedFromWidget);

#ifndef TEST_TORN_ACCEL_CONTEXT_SHARED
   if (!(is_sensitive_managed = SensitiveManagedVerified(toplevel, w)))
      strcpy(cr_value, "ignore activate");
   else
#endif
   strcpy(cr_value, "ACTIVATE");

   PrintInfo(
      XrmQuarkToString(w->core.xrm_name), 
      s,
      cr_value,
      "",
      XrmQuarkToString(postedFromWidget->core.xrm_name),
      XrmQuarkToString(toplevel->core.xrm_name));
}

void 
ValueChangedCB (Widget w, caddr_t clientData, 
		XmToggleButtonCallbackStruct *tb_cbs)
{
   char *s;
   Widget toplevel, postedFromWidget;
   int j;
   char svalue[8];
   char cr_value[32];
   Boolean is_sensitive_managed;

   s = ButtonLabel(w);
   postedFromWidget = XmGetPostedFromWidget(XtParent(w));
   toplevel = FindTopMostShell(postedFromWidget);

#ifndef TEST_TORN_ACCEL_CONTEXT_SHARED
   if (!(is_sensitive_managed = SensitiveManagedVerified(toplevel, w)))
      strcpy(cr_value, "ignore value_changed");
   else
#endif
   strcpy(cr_value, "VALUE_CHANGED");

   sprintf(svalue, "%d", tb_cbs->set);
   PrintInfo(
      XrmQuarkToString(w->core.xrm_name), 
      s,
      cr_value,
      svalue,
      XrmQuarkToString(postedFromWidget->core.xrm_name),
      XrmQuarkToString(toplevel->core.xrm_name));

   if (is_sensitive_managed)
   {
      for(j=0; j<nToggleState; j++)
      {
	 if ((toplevel == ToggleStateList[j].context) &&
	     (w == ToggleStateList[j].menu_item))
	 {
	    /* since the toggle's state is not always set to the correct
	     * context before an accelerator is pressed, we cannot depend
	     * on the Motif state and must use our application's own 
	     * tracking.  Note this occurs when no menu is posted which
	     * would give our map/tear off activate callbacks a chance to
	     * properly set toggle state.
	     */
	    if ((tb_cbs->event->type == KeyPress) ||
		 (tb_cbs->event->type == KeyRelease))
	    {
	       ToggleStateList[j].state = !ToggleStateList[j].state;
	       if (!quiet)
		  printf("ToggleB REAL internal tracked value: %d\n", 
		     ToggleStateList[j].state);
	    }
	    else
	       ToggleStateList[j].state = XmToggleButtonGetState(w); 
	    break;
	 }
      }
   }
}

void 
PopupCB (Widget w, caddr_t clientData, caddr_t callData)
{
   Arg args[15];
   int n;
   unsigned char rc_type;
   XmMenuShellWidget ms = (XmMenuShellWidget)w;
   XmRowColumnWidget rc;
   Widget toplevel, postedFromWidget;

   if (ms && ms->composite.num_children)
      rc = (XmRowColumnWidget) ms->composite.children[0];
   else
      return;

   postedFromWidget = XmGetPostedFromWidget((Widget)rc);
   toplevel = FindTopMostShell(postedFromWidget);

   PrintInfo(
      XrmQuarkToString(w->core.xrm_name), 
      "",
      "POPUP",     
      "",
      XrmQuarkToString(postedFromWidget->core.xrm_name),
      XrmQuarkToString(toplevel->core.xrm_name));

   if (test_type == TEST_CASCADE_CB)
   {
      n = 0;
      XtSetArg(args[n], XmNrowColumnType, &rc_type); n++;
      XtGetValues((Widget)rc, args, n);

      if (rc_type == XmMENU_POPUP)
         update_submenu_state(toplevel, rc);
   }
}

void 
MapCB(Widget w, caddr_t clientData, caddr_t callData)
{
   XmRowColumnWidget rc = (XmRowColumnWidget)w;
   Widget toplevel, postedFromWidget;

   postedFromWidget = XmGetPostedFromWidget((Widget)rc);
   toplevel = FindTopMostShell(postedFromWidget);

   PrintInfo(
      XrmQuarkToString(w->core.xrm_name), 
      "",
      "MAP",     
      "",
      XrmQuarkToString(postedFromWidget->core.xrm_name),
      XrmQuarkToString(toplevel->core.xrm_name));

   if (test_type == TEST_MAP_CB)
      update_submenu_state(toplevel, rc);
}

void 
UnmapCB(Widget w, caddr_t clientData, caddr_t callData)
{
   Widget toplevel, postedFromWidget;

   postedFromWidget = XmGetPostedFromWidget(w);
   toplevel = FindTopMostShell(postedFromWidget);

   PrintInfo(
      XrmQuarkToString(w->core.xrm_name), 
      "",
      "UNMAP",     
      "",
      XrmQuarkToString(postedFromWidget->core.xrm_name),
      XrmQuarkToString(toplevel->core.xrm_name));
}

void 
TearOffActivateCB (Widget w,caddr_t clientData, 
		   XmRowColumnCallbackStruct *callData)
{
   Widget toplevel, postedFromWidget;
   char svalue[8];

   postedFromWidget = XmGetPostedFromWidget(w);
   toplevel = FindTopMostShell(postedFromWidget);

   sprintf(svalue, "%d", (unsigned short)callData->data);
   PrintInfo(
      XrmQuarkToString(w->core.xrm_name), 
      "",
      "TEAR_OFF_ACTIVATE",     
      svalue,
      XrmQuarkToString(postedFromWidget->core.xrm_name),
      XrmQuarkToString(toplevel->core.xrm_name));
  
   if (test_type == TEST_CASCADE_CB)
      update_submenu_state(toplevel, (XmRowColumnWidget)w);
}

void 
TearOffDeactivateCB (Widget w, caddr_t clientData,
		     XmRowColumnCallbackStruct *callData)
{
   Widget toplevel, postedFromWidget;
   char svalue[8];

   postedFromWidget = XmGetPostedFromWidget(w);
   toplevel = FindTopMostShell(postedFromWidget);

   sprintf(svalue, "%d", (unsigned short)callData->data);
   PrintInfo(
      XrmQuarkToString(w->core.xrm_name), 
      "",
      "TEAR_OFF_DEACTIVATE",     
      svalue,
      XrmQuarkToString(postedFromWidget->core.xrm_name),
      XrmQuarkToString(toplevel->core.xrm_name));
}

void 
ChangeSensitivity (Widget w, caddr_t clientData, caddr_t callData)
{
   Widget toplevel, menu_item;
   int i;
   char *s;
   char svalue[8];

   s = ButtonLabel(w);

   /* ChangeSensitivity() was called from a button in a workarea - NOT a menu.
    * so toplevel is simply the topmost shell in the parent heirarchy.
    */
   toplevel = FindTopMostShell(w);

   for(i=0; i<nButtonData; i++)
   {
      if ( (toplevel == ButtonDataList[i].context) &&
	   (w == ButtonDataList[i].button) )
      {
	 menu_item = ButtonDataList[i].menu_item;
	 ButtonDataList[i].sensitive = !ButtonDataList[i].sensitive;
	 
         sprintf(svalue, "%d", ButtonDataList[i].sensitive);
	 PrintInfo(
	    XrmQuarkToString(w->core.xrm_name), 
	    s,
	    "Change Sensitivity",     
	    svalue,
	    "",
	    XrmQuarkToString(toplevel->core.xrm_name));

	 /* If the parent pane is torn, only change the sensitivity
	  * immeditately if the toplevels match.  Otherwise, the next 
	  * post/map will update sensitivity appropriately.
	  */
	 if (!XmIsMenuShell(XtParent(XtParent(menu_item))))
	 {
	    Widget tearoff_toplevel =
	       FindTopMostShell(XmGetPostedFromWidget(XtParent(menu_item)));

	    if (tearoff_toplevel == ButtonDataList[i].context)
	       XtSetSensitive(menu_item, ButtonDataList[i].sensitive);
	 }
	 break;
      }
   }
}

/*
 * BEGIN the BEGIN
 */

/*
 * This is by no means an example that should be used to track *real*
 * application menu_item sensitivity.  For this test, it's a simplisitic
 * approach that's quick and dirty.
 */
void
RegisterButtonData(Widget wid, Widget data)
{
   /* inefficient, but quick and dirty */
   if (!ButtonDataList)
   {
      ButtonDataList = 
	 (ButtonDataRec *)XtMalloc((Cardinal)sizeof(ButtonDataRec));
      nButtonData = 0;
   } 
   else
      {
         ButtonDataList = (ButtonDataRec *)XtRealloc((char *)ButtonDataList,
	    (Cardinal)((nButtonData + 1) * sizeof(ButtonDataRec)));
      }
   
   ButtonDataList[nButtonData].context = FindTopMostShell(wid);
   ButtonDataList[nButtonData].button = wid;
   ButtonDataList[nButtonData].menu_item = data;
   ButtonDataList[nButtonData].sensitive = XtIsSensitive(data);

   nButtonData++;
}

void
RegisterToggleState(Widget wid, Widget tb)
{
   if (!ToggleStateList)
   {
      ToggleStateList = 
	 (ToggleStateRec *)XtMalloc((Cardinal)sizeof(ToggleStateRec));
      nToggleState = 0;
   } 
   else
      {
         ToggleStateList = (ToggleStateRec *)XtRealloc((char *)ToggleStateList,
	    (Cardinal)((nToggleState + 1) * sizeof(ToggleStateRec)));
      }

   ToggleStateList[nToggleState].context = FindTopMostShell(wid);
   ToggleStateList[nToggleState].menu_item = tb;
   ToggleStateList[nToggleState].state = XmToggleButtonGetState(tb);

   nToggleState++;
}

/*
 * Meat and Potatoes
 */
Widget
CreateSelectionButton(Widget parent, char * name, caddr_t data)
{
    Arg args[15];
    int n;
    Widget wid;

    n = 0;
    wid = XmCreatePushButton(parent, name, args, n);
    XtManageChild(wid);
    XtAddCallback(wid, XmNactivateCallback, 
		  (XtCallbackProc) ChangeSensitivity, (XtPointer)data);

    RegisterButtonData(wid, (Widget)data);

    if(XmIsToggleButton((Widget)data) || XmIsToggleButtonGadget((Widget)data))
    {
       RegisterToggleState(wid, (Widget)data);
    }

    return(wid);
}

void 
AddMenuCallbacks(Widget w)
{
   XtAddCallback(w, XmNmapCallback, (XtCallbackProc) MapCB, (XtPointer)NULL);
   XtAddCallback(w, XmNunmapCallback, (XtCallbackProc) UnmapCB, 
		 (XtPointer)NULL);
   XtAddCallback(w, XmNtearOffMenuActivateCallback, 
		 (XtCallbackProc) TearOffActivateCB, (XtPointer) NULL);
   XtAddCallback(w, XmNtearOffMenuDeactivateCallback, 
		 (XtCallbackProc) TearOffDeactivateCB, 
      (XtPointer) NULL);
}

void 
AddMenuPushButtonCallbacks(Widget w)
{
   XtAddCallback(w, XmNarmCallback, (XtCallbackProc) ArmCB, (XtPointer) NULL);
   XtAddCallback(w, XmNactivateCallback, (XtCallbackProc) ActivateCB, 
		 (XtPointer) NULL);
   XtAddCallback(w, XmNdisarmCallback, (XtCallbackProc) DisarmCB, 
		 (XtPointer) NULL);
}

void 
AddMenuToggleButtonCallbacks(Widget w)
{
   XtAddCallback(w, XmNarmCallback, (XtCallbackProc) ArmCB, (XtPointer) NULL);
   XtAddCallback(w, XmNvalueChangedCallback, (XtCallbackProc) ValueChangedCB, 
		 (XtPointer) NULL);
   XtAddCallback(w, XmNdisarmCallback, (XtCallbackProc) DisarmCB, 
		 (XtPointer) NULL);
}

void 
CreatePopup(Widget attach_widget)
{
   Arg args[15];
   int n;
   Widget child[16];
   int nchild;

   if (pop)
   {
      XmAddToPostFromList(pop, attach_widget);
      XtAddEventHandler(attach_widget, ButtonPressMask, False, 
			(XtEventHandler)PostIt, (XtPointer) pop);
      return;
   }

   /* The create time parent MUST be the attach_widget.  This provides
    * the popup with the attach_widget to insert into its internal
    * postFromList.
    */
   n = 0;
   pop = XmCreatePopupMenu(attach_widget, "pop", args, n);
   AddMenuCallbacks(pop);
   XtAddCallback(XtParent(pop), XmNpopupCallback, (XtCallbackProc) PopupCB, 
		 (XtPointer) NULL);

   XtAddEventHandler(attach_widget, ButtonPressMask, False, 
		     (XtEventHandler)PostIt, (XtPointer) pop);

   nchild = 0;

   n = 0;
   child[nchild++] =
   pop_lbl = XmCreateLabel(pop, "pop_lbl", args, n);

   n = 0;
   child[nchild++] =
   XmCreateSeparator(pop, "pop_sep", args, n);

   n = 0;
   child[nchild++] =
   pop_pb1 = XmCreatePushButton(pop, "pop_pb1", args, n);
   AddMenuPushButtonCallbacks(pop_pb1);

   n = 0;
   child[nchild++] =
   pop_pb2 = XmCreatePushButtonGadget(pop, "pop_pb2", args, n);
   AddMenuPushButtonCallbacks(pop_pb2);

   n = 0;
   XtSetArg(args[n], XmNsubMenuId, pd1); n++;
   child[nchild++] =
   pop_cb1 = XmCreateCascadeButton(pop, "pop_cb1", args, n);
   XtAddCallback(pop_cb1, XmNcascadingCallback, (XtCallbackProc) CascadingCB, 
		 (XtPointer) NULL);

   n = 0;
   XtSetArg(args[n], XmNsubMenuId, pd4); n++;
   child[nchild++] =
   pop_cb2 = XmCreateCascadeButtonGadget(pop, "pop_cb2", args, n);
   XtAddCallback(pop_cb2, XmNcascadingCallback,(XtCallbackProc) CascadingCB, 
		 (XtPointer) NULL);

   XtManageChildren(child, nchild);

   /* Share the popup with the other main window */
}

void
SetupMainWindow(Widget toplevel)
{
   Arg args[15];
   int n;
   Widget child[16];
   int nchild;
   static Boolean one_time_only = TRUE;

   n = 0;
   mw = XmCreateMainWindow(toplevel, "mw", (ArgList) args, n);
   XtManageChild(mw);

   CreatePopup(mw);

   /**
    ** Create menubar and it's immediate cascade buttons 
    **/
   n = 0;
   mb = XmCreateMenuBar(mw, "mb", args, n);
   XtManageChild(mb);

   nchild = 0;

   n = 0;
   child[nchild++] = 
   mb_cb1 = XmCreateCascadeButton(mb, "mb_cb1", args, n);
   XtAddCallback(mb_cb1, XmNcascadingCallback, (XtCallbackProc) CascadingCB, 
		 (XtPointer) NULL);

   n = 0;
   child[nchild++] =
   mb_cb2 = XmCreateCascadeButtonGadget(mb, "mb_cb2", args, n);
   XtAddCallback(mb_cb2, XmNcascadingCallback, (XtCallbackProc) CascadingCB, 
		 (XtPointer) NULL);

   n = 0;
   child[nchild++] =
   mb_cb3 = XmCreateCascadeButtonGadget(mb, "mb_cb3", args, n);
   XtAddCallback(mb_cb3, XmNcascadingCallback, (XtCallbackProc) CascadingCB, 
		 (XtPointer) NULL);

   XtManageChildren(child, nchild);
   
   /**
    ** Now set up the toplevel menu to cascading pulldown connections
    ** In the technical sense, these are shared because they have more
    ** than one attach-widget (cascade button).
    **/
  
   n = 0;
   XtSetArg(args[n], XmNsubMenuId, pd1); n++;
   XtSetValues(mb_cb1, args, n);

   n = 0;
   XtSetArg(args[n], XmNsubMenuId, pd4); n++;
   XtSetValues(mb_cb2, args, n);

   n = 0;
   XtSetArg(args[n], XmNsubMenuId, pd5); n++;
   XtSetValues(mb_cb3, args, n);

   /**
    ** Create Workarea for selection pushbuttons
    **/
   n = 0;
   wa = XmCreateRowColumn(mw, "wa", args, n);
   XtManageChild(wa);

   n = 0;
   XtSetArg(args[n], XmNsubMenuId, pd4); n++;
   wa_om = XmCreateOptionMenu(wa, "wa_om", args, n);
   XtManageChild(wa_om);

   child[0] = XmCreateSeparator(wa, "separator", args, 0);
   child[1] = XmCreateSeparator(wa, "separator", args, 0);
   XtManageChildren(child, 2);

   /***/

   n = 0;
   XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
   action_wa = XmCreateRowColumn(wa, "action_wa", args, n);
   XtManageChild(action_wa);

   n = 0;
   action_reset = XmCreatePushButton(action_wa, "action_reset", args, n);
   XtManageChild(action_reset);
   XtAddCallback(action_reset, XmNactivateCallback, 
		 (XtCallbackProc)ResetMenuCB, (XtPointer)NULL);

   if (one_time_only)
   {
      n = 0;
      action_test = XmCreatePushButton(action_wa, "action_test_acc", args, n);
      XtManageChild(action_test);
      XtAddCallback(action_test, XmNactivateCallback, 
		    (XtCallbackProc)TestAccCB, (XtPointer) NULL);

      n = 0;
      action_test = 
	 XmCreatePushButton(action_wa, "action_test_opaque", args, n);
      XtManageChild(action_test);
      XtAddCallback(action_test, XmNactivateCallback, 
		    (XtCallbackProc) TestOpaqueCB, (XtPointer) NULL);

      one_time_only = FALSE;
   }

   /***/

   child[0] = XmCreateSeparator(wa, "separator", args, 0);
   XtManageChild(child[0]);

   wa_pb1 = CreateSelectionButton(wa, "wa_pb1", (caddr_t) pd1_pb1);
   wa_pb2 = CreateSelectionButton(wa, "wa_pb2", (caddr_t) pd1_pb2);
   wa_pb3 = CreateSelectionButton(wa, "wa_pb3", (caddr_t) pd1_tb1);
   wa_pb4 = CreateSelectionButton(wa, "wa_pb4", (caddr_t) pd1_tb2);
   wa_pb5 = CreateSelectionButton(wa, "wa_pb5", (caddr_t) pd1_cb1);
   wa_pb6 = CreateSelectionButton(wa, "wa_pb6", (caddr_t) pd1_cb2);

   child[0] = XmCreateSeparator(wa, "separator", args, 0);
   XtManageChild(child[0]);

   wa_pb7 = CreateSelectionButton(wa, "wa_pb7", (caddr_t) pd2_pb1);
   wa_pb8 = CreateSelectionButton(wa, "wa_pb8", (caddr_t) pd2_pb2);

   child[0] = XmCreateSeparator(wa, "separator", args, 0);
   XtManageChild(child[0]);

   wa_pb9 = CreateSelectionButton(wa, "wa_pb9", (caddr_t) pd3_tb1);
   wa_pb10 = CreateSelectionButton(wa, "wa_pb10", (caddr_t) pd3_tb2);

   child[0] = XmCreateSeparator(wa, "separator", args, 0);
   XtManageChild(child[0]);

   wa_pb11 = CreateSelectionButton(wa, "wa_pb11", (caddr_t) pd4_pb1);
   wa_pb12 = CreateSelectionButton(wa, "wa_pb12", (caddr_t) pd4_pb2);

   child[0] = XmCreateSeparator(wa, "separator", args, 0);
   XtManageChild(child[0]);

   wa_pb17 = CreateSelectionButton(wa, "wa_pb17", (caddr_t) pd5_tb1);
   wa_pb18 = CreateSelectionButton(wa, "wa_pb18", (caddr_t) pd5_tb2);

   child[0] = XmCreateSeparator(wa, "separator", args, 0);
   XtManageChild(child[0]);

   wa_pb13 = CreateSelectionButton(wa, "wa_pb13", (caddr_t) pop_pb1);
   wa_pb14 = CreateSelectionButton(wa, "wa_pb14", (caddr_t) pop_pb2);
   wa_pb15 = CreateSelectionButton(wa, "wa_pb15", (caddr_t) pop_cb1);
   wa_pb16 = CreateSelectionButton(wa, "wa_pb16", (caddr_t) pop_cb2);

   /**
    ** Assign the main window areas
    **/

   XmMainWindowSetAreas(mw, mb, NULL, NULL, NULL, wa);
}

void
CreateCascadingMenuHierarchy(Widget toplevel)

{
   Arg args[15];
   int n;
   Widget child[16];
   int nchild;

   /**
    ** Create a pulldown hierarchy for the menubars
    ** Note that this will only work for >= Motif1.2 because we are using
    ** a mixed shared menushell model.  I.e. pd1 is not MenuShell shared
    ** and pd2, pd3, and pd4 are sharing.  I think a bug prevents this
    ** model from working in 1.1.
    **/

   /** pulldown 1 **/
   /* use toplevel as the parent because mb is not yet created */
   n = 0;
   pd1 = XmCreatePulldownMenu(toplevel, "pd1", args, n);
   AddMenuCallbacks(pd1);
   child[0] = XmGetTearOffControl(pd1);
   if (!quiet)
   {
      if (child[0])
      {
	 printf("pd1's tear off control 0x%p (%s)\n", child[0], 
	    child[0]->core.name);

	 if(XtParent(child[0]) == pd1)
	    printf("Tear off control's parent verified\n");
	 else
	    printf("ERROR: Tear off control's parent is incorrect\n");
      }
      else
	 printf("ERROR: Tear off control is NULL\n");
   }

   nchild = 0;

   n = 0;
   child[nchild++] = 
   pd1_lbl = XmCreateLabel(pd1, "pd1_lbl", args, n);

   n = 0;
   child[nchild++] = 
   pd1_sep = XmCreateSeparator(pd1, "pd1_sep", args, n);

   n = 0;
   child[nchild++] = 
   pd1_pb1 = XmCreatePushButton(pd1, "pd1_pb1", args, n);
   AddMenuPushButtonCallbacks(pd1_pb1);

   n = 0;
   child[nchild++] = 
   pd1_pb2 = XmCreatePushButtonGadget(pd1, "pd1_pb2", args, n);
   AddMenuPushButtonCallbacks(pd1_pb2);

   n = 0;
   child[nchild++] = 
   pd1_tb1 = XmCreateToggleButton(pd1, "pd1_tb1", args, n);
   AddMenuToggleButtonCallbacks(pd1_tb1);

   n = 0;
   child[nchild++] = 
   pd1_tb2 = XmCreateToggleButtonGadget(pd1, "pd1_tb2", args, n);
   AddMenuToggleButtonCallbacks(pd1_tb2);

   n = 0;
   child[nchild++] = 
   pd1_cb1 = XmCreateCascadeButton(pd1, "pd1_cb1", args, n);
   XtAddCallback(pd1_cb1, XmNcascadingCallback, (XtCallbackProc) CascadingCB, 
		 (XtPointer) NULL);

   n = 0;
   child[nchild++] = 
   pd1_cb2 = XmCreateCascadeButtonGadget(pd1, "pd1_cb2", args, n);
   XtAddCallback(pd1_cb2, XmNcascadingCallback, (XtCallbackProc) CascadingCB, 
		 (XtPointer) NULL);

   XtManageChildren(child, nchild);

   /** pulldown 2 **/
   n = 0;
   pd2 = XmCreatePulldownMenu(pd1, "pd2", args, n);
   AddMenuCallbacks(pd2);

   nchild = 0;

   n = 0;
   child[nchild++] = 
   pd2_pb1 = XmCreatePushButton(pd2, "pd2_pb1", args, n);
   AddMenuPushButtonCallbacks(pd2_pb1);

   n = 0;
   child[nchild++] = 
   pd2_pb2 = XmCreatePushButtonGadget(pd2, "pd2_pb2", args, n);
   AddMenuPushButtonCallbacks(pd2_pb2);

   XtManageChildren(child, nchild);

   /** pulldown 3 **/
   n = 0;
   pd3 = XmCreatePulldownMenu(pd1, "pd3", args, n);
   AddMenuCallbacks(pd3);

   nchild = 0;

   n = 0;
   child[nchild++] = 
   pd3_tb1 = XmCreateToggleButton(pd3, "pd3_tb1", args, n);
   AddMenuToggleButtonCallbacks(pd3_tb1);

   n = 0;
   child[nchild++] = 
   pd3_tb2 = XmCreateToggleButtonGadget(pd3, "pd3_tb2", args, n);
   AddMenuToggleButtonCallbacks(pd3_tb2);

   XtManageChildren(child, nchild);

   /** pulldown 4 **/
   /* Use pd1 as the parent widget so that menushells are shared - Note
    * that typically this would be mb (except that mb is not yet created).
    */
   n = 0;
   pd4 = XmCreatePulldownMenu(pd1, "pd4", args, n);
   AddMenuCallbacks(pd4);

   nchild = 0;

   n = 0;
   child[nchild++] =
   pd4_pb1 = XmCreatePushButton(pd4, "pd4_pb1", args, n);
   AddMenuPushButtonCallbacks(pd4_pb1);

   n = 0;
   child[nchild++] =
   pd4_pb2 = XmCreatePushButton(pd4, "pd4_pb2", args, n);
   AddMenuPushButtonCallbacks(pd4_pb2);

   XtManageChildren(child, nchild);

   /** pulldown 5 **/
   n = 0;
   pd5 = XmCreatePulldownMenu(pd1, "pd5", args, n);
   AddMenuCallbacks(pd5);

   nchild = 0;

   n = 0;
   child[nchild++] =
   pd5_tb1 = XmCreatePushButton(pd5, "pd5_tb1", args, n);
   AddMenuPushButtonCallbacks(pd5_tb1);

   n = 0;
   child[nchild++] =
   pd5_tb2 = XmCreatePushButton(pd5, "pd5_tb2", args, n);
   AddMenuPushButtonCallbacks(pd5_tb2);

   XtManageChildren(child, nchild);

   /**
    ** Now set up the cascading pulldown to cascading pulldown connections
    ** In the technical sense, these are not shared because they only have
    ** one attach-widget (cascade button).  However, in reality, they are
    ** and still require state tracking.
    **/

   n = 0;
   XtSetArg(args[n], XmNsubMenuId, pd2); n++;
   XtSetValues(pd1_cb1, args, n);

   n = 0;
   XtSetArg(args[n], XmNsubMenuId, pd3); n++;
   XtSetValues(pd1_cb2, args, n);

#ifndef TEST_TORN_ACCEL_CONTEXT_SHARED
   _XmAllowAcceleratedInsensitiveUnmanagedMenuItems(toplevel, TRUE);
#endif
}

void 
main (argc, argv)
unsigned int argc;
char **argv;
{
   Arg args[15];
   int n;
   int i;
   char name[32];
   XrmDatabase save_db, new_db;

   XmRepTypeInstallTearOffModelConverter();

   CommonTestInit(argc, argv);

   XrmInitialize();
   save_db = XtDatabase(display);
   sprintf(name, "%s.db", argv[0]);
   new_db = XrmGetFileDatabase(name);
   XrmCombineDatabase(new_db, &save_db, TRUE);

   get_options(argc, argv);

   if (!quiet)
      printf("Test type: %d\n", test_type);

   if (handle_warnings)
      XtSetWarningHandler((XtErrorHandler)MyWarningHandler);

   CreateCascadingMenuHierarchy(Shell1);
   SetupMainWindow(Shell1);
   XtRealizeWidget(Shell1);

   n = 0;
   for(i=0; i<num_popups; i++)
   {
      if (i)
      {
	 n = 0;
	 XtSetArg(args[n], XmNiconic, True); n++;
      }
      sprintf(name, "Dup%d", i+1);
      popup[i] = XtCreatePopupShell (name, topLevelShellWidgetClass,
				   Shell1, args, n);
      SetupMainWindow(popup[i]);
      XtPopup(popup[i], XtGrabNone);
   }

   for(i=0; i < 40; i++)
   {
      if (!quiet)
	 printf("CommonPause #%d\n", i);
      CommonPause();
   }

   XtAppMainLoop(app_context);
}
