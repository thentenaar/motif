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
static char rcsid[] = "$XConsortium: RowCol3.c /main/10 1995/07/13 18:55:07 drk $"
#endif
#endif

#include <testlib.h>

#define NUM_LEGAL_CHILDREN 17
#define NUM_LEGAL_PARENTS 5
#define MAX_CHILDREN 25

#if XmVERSION  < 2
#define XmSET 1 
#endif

Widget RC, parent, child[MAX_CHILDREN];
Widget Popup;
WidgetClass parent_class;
int parent_index;
Arg args[15];
int n;
int num_children = 0;
char *text_value = NULL;

typedef struct {
  char *string;
  WidgetClass class;
} StringClassPair;

static StringClassPair parent_table[] = {
  { "xmBulletinBoardWidgetClass", (WidgetClass) NULL },
  { "xmDrawingAreaWidgetClass", (WidgetClass) NULL },
  { "xmFrameWidgetClass", (WidgetClass) NULL  },
  { "xmMainWindowWidgetClass", (WidgetClass) NULL },
  { "xmFormWidgetClass", (WidgetClass) NULL }
};

static StringClassPair child_table[] = {
  { "xmScaleWidgetClass",  (WidgetClass) NULL },
  { "xmScrollBarWidgetClass",  (WidgetClass) NULL },
  { "xmCommandWidgetClass",  (WidgetClass) NULL },
  { "xmArrowButtonWidgetClass",  (WidgetClass) NULL },
  { "xmListWidgetClass",  (WidgetClass) NULL },
  { "xmSeparatorWidgetClass",  (WidgetClass) NULL },
  { "xmTextWidgetClass",  (WidgetClass) NULL },
  { "xmTextFieldWidgetClass",  (WidgetClass) NULL },
  { "xmLabelWidgetClass",  (WidgetClass) NULL },
  { "xmDrawnButtonWidgetClass",  (WidgetClass) NULL },
  { "xmPushButtonWidgetClass",  (WidgetClass) NULL },
  { "xmToggleButtonWidgetClass",  (WidgetClass) NULL },
  { "xmArrowButtonGadgetClass",  (WidgetClass) NULL },
  { "xmSeparatorGadgetClass",  (WidgetClass) NULL },
  { "xmLabelGadgetClass",  (WidgetClass) NULL },
  { "xmPushButtonGadgetClass",  (WidgetClass) NULL },
  { "xmToggleButtonGadgetClass", (WidgetClass) NULL }
};

WidgetClass ParentClass(char *parent_class_name);
void CreateControlPanel(void);
void InitializeClassLookupTables(void);
static void CreateParent(WidgetClass parent_class);
static void CreateChild(char *name);

/*  CALLBACKS TO UPDATE THE CONTROL PANEL */

static void
updateNumColumns(Widget w, XtPointer closure, 
		 XtPointer data)
{
  XmScaleCallbackStruct *call_data=(XmScaleCallbackStruct *)data;
  XtSetArg(args[0], XmNnumColumns, call_data->value);
  XtSetValues(RC, args, 1);
  return;
}

static void
updateSpacing(Widget w, XtPointer closure, 
		XtPointer data)
{
  XmScaleCallbackStruct *call_data=(XmScaleCallbackStruct *)data;
  XtSetArg(args[0], XmNspacing, call_data->value);
  XtSetValues(RC, args, 1);
  return;
}

static void
updateWidth(Widget w, XtPointer closure, 
		XtPointer data)
{
  XmScaleCallbackStruct *call_data=(XmScaleCallbackStruct *)data;
  XtSetArg(args[0], XmNmarginWidth, call_data->value);
  XtSetValues(RC, args, 1);
  return;
}

static void
updateHeight(Widget w, XtPointer closure,
		XtPointer data)
{
  XmScaleCallbackStruct *call_data=(XmScaleCallbackStruct *)data;
  XtSetArg(args[0], XmNmarginHeight, call_data->value);
  XtSetValues(RC, args, 1);
  return;
}

static void
updateEntryBorder(Widget w, XtPointer closure,
		XtPointer data)
{
  XmScaleCallbackStruct *call_data=(XmScaleCallbackStruct *)data;
  XtSetArg(args[0], XmNentryBorder, call_data->value);
  XtSetValues(RC, args, 1);
  return;
}

static void
updateAdjustLast(Widget w, XtPointer adjustLast, 
		XtPointer data)
{
  XmToggleButtonCallbackStruct *call_data=(XmToggleButtonCallbackStruct *)data;
  XtSetArg(args[0], XmNadjustLast, call_data->set);
  XtSetValues(RC, args, 1);
  return;
}

static void 
updateAdjustMargin(Widget w, XtPointer adjustMargin, 
		XtPointer data)
{
  XmToggleButtonCallbackStruct *call_data=(XmToggleButtonCallbackStruct *)data;
  XtSetArg(args[0], XmNadjustMargin, call_data->set);
  XtSetValues(RC, args, 1);
  return;
}

static void 
updateIsAligned(Widget w, XtPointer isAligned, 
		XtPointer data)
{
  XmToggleButtonCallbackStruct *call_data=(XmToggleButtonCallbackStruct *)data;
  XtSetArg(args[0], XmNisAligned, call_data->set);
  XtSetValues(RC, args, 1);
  return;
}

static void 
updateResizeHeight(Widget w, XtPointer resizeHeight,
		XtPointer data)
{
  XmToggleButtonCallbackStruct *call_data=(XmToggleButtonCallbackStruct *)data;
  XtSetArg(args[0], XmNresizeHeight, call_data->set);
  XtSetValues(RC, args, 1);
  return;
}

static void 
updateResizeWidth(Widget w, XtPointer resizeWidth, 
		XtPointer data)
{
  XmToggleButtonCallbackStruct *call_data=(XmToggleButtonCallbackStruct *)data;
  XtSetArg(args[0], XmNresizeWidth, call_data->set);
  XtSetValues(RC, args, 1);
  return;
}

static void 
updateOrientation(Widget w, XtPointer orientation,
		  XtPointer data)
{
  XmToggleButtonCallbackStruct *call_data=(XmToggleButtonCallbackStruct *)data;
	if ( call_data->set == XmSET ) {
  XtSetArg(args[0], XmNorientation, orientation);
  XtSetValues(RC, args, 1);
	}
  return;
}

static void 
updateAlignment(Widget w, XtPointer alignment,
		XtPointer data)
{
  XmToggleButtonCallbackStruct *call_data=(XmToggleButtonCallbackStruct *)data;
  XtSetArg(args[0], XmNentryAlignment, alignment);
  XtSetValues(RC, args, 1);
  return;
}

static void 
updateChildren(Widget w, XtPointer closure,
		XtPointer data)
{
  XmScaleCallbackStruct *call_data=(XmScaleCallbackStruct *)data;
  register int i, temp;
  temp = num_children;

  if (call_data->value < num_children)
    {
      /*  Destroy extra children */

      for (i = temp - 1; i >= call_data->value; i--)
	{
	  XtDestroyWidget(child[i]);
	  num_children--;
	}
    }	
  else
    if (call_data->value > num_children)
      {	
	/* Add more children */
	
	for (i = num_children; i < call_data->value; i++)
	  CreateChild(text_value);
      }
  return;
}

static void
print_width_height(Widget w, XtPointer closure, XtPointer call_data)
{
  Dimension my_width, my_height;

  XtSetArg(args[0], XmNwidth, &my_width);
  XtSetArg(args[1], XmNheight, &my_height);
  XtGetValues(w, args, 2);
  printf("My width = %d, my height = %d\n", my_width, my_height);
}

static void 
updatePacking(Widget w, XtPointer packtype,
		XtPointer data)
{
  XmToggleButtonCallbackStruct *call_data=(XmToggleButtonCallbackStruct *)data;
  register int n;

  if (call_data->set)
    {
      n = 0;
      XtSetArg(args[n], XmNpacking, packtype); n++;
      XtSetValues(RC, args, n);
    }
}

static void
updateRCwidth(Widget w, XtPointer closure,
	      XtPointer call_data)
{
  int rc_width;
  char *value = NULL;
  value = XmTextFieldGetString(w);
  if (value != NULL)
    {
      rc_width = atoi(value);
      XtSetArg(args[0], XmNwidth, rc_width); 
      XtSetValues(RC, args, 1);
    }
}

static void
updateRCheight(Widget w, XtPointer closure,
	       XtPointer call_data)
{
  int rc_height;
  char *value = NULL;
  value = XmTextFieldGetString(w);
  if (value != NULL)
    {
      rc_height = atoi(value);
      XtSetArg(args[0], XmNheight, rc_height); 
      XtSetValues(RC, args, 1);
    }
}

static void
updateButtonLabels(Widget w, XtPointer closure,
		   XtPointer call_data)
{
  XmString tcs;
  register int i;

  text_value = XmTextFieldGetString(w);
  tcs = XmStringCreateLtoR(text_value, XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[0], XmNlabelString, tcs);

  for (i = 0; i < num_children; i++)
      XtSetValues(child[i], args, 1);

  XmStringFree(tcs);
  return;
}

static void
updateShadow( Widget w, XtPointer closure, XtPointer data)
{ 
  XmScaleCallbackStruct *call_data=(XmScaleCallbackStruct *)data;
  int i;
  XtSetArg(args[0], XmNshadowThickness, call_data->value);

  for (i = 0; i < num_children; i++)
      XtSetValues(child[i], args, 1);
  return;
}

static void
resetControlPanel(Widget w, XtPointer parent_widget, XtPointer call_data)
{
  /* Destroy the topmost widgets */

  XtDestroyWidget(RC);
  XtDestroyWidget(Popup);

  /* Reset global variable num_children */
  
  num_children = 0;
  text_value = NULL;

  /* recreate the rowColumn */

  n = 0;
  XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
  XtSetArg(args[n], XmNnumColumns, 1); n++;
  RC = XmCreateRowColumn(parent, "outer", (ArgList) args, n);

  if (parent_class == xmMainWindowWidgetClass)
    {
      n = 0;
      XtSetArg(args[n], XmNworkWindow, RC); n++;
      XtSetValues(parent, args, n);
    }

  CreateChild(NULL);

  XtManageChild(RC);

  /* Recreate the control panel */

  CreateControlPanel();
}

/*****  MAIN PROGRAM  ***********/

int main(argc, argv)
unsigned int argc;
char **argv;
{

  /*  initialize toolkit  */
  CommonTestInit(argc, argv);

  n = 0;
  XtSetArg(args[n], XmNallowShellResize, True); n++;
  XtSetArg(args[n], XmNuseAsyncGeometry, True); n++;
  XtSetValues(Shell1, args, n);

  if (UserData != NULL)
    {
      InitializeClassLookupTables();
      parent_class = ParentClass(UserData);
      if (parent_class != NULL)
	CreateParent(parent_class);
    }
  else
    {
      printf("Usage: %s -u xm<Parent>WidgetClass\n", argv[0]);
      fflush(stdout);
      exit(1);
    }

  n = 0;
  XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
  XtSetArg(args[n], XmNnumColumns, 1); n++;
  RC = XmCreateRowColumn(parent, "outer", (ArgList) args, n);

  if (parent_class == xmMainWindowWidgetClass)
    {
      n = 0;
      XtSetArg(args[n], XmNworkWindow, RC); n++;
      XtSetValues(parent, args, n);
    }

  CreateChild(NULL);

  XtManageChild(RC);
  XtManageChild(parent);

  CreateControlPanel();

  XtRealizeWidget(Shell1);

  CommonPause();
  CommonPause();                               /* look around */
  CommonPause();
  CommonPause();
  CommonPause();
  CommonPause();
  CommonPause();
  CommonPause();
  CommonPause();
  XtAppMainLoop(app_context);
}

static void CreateParent(WidgetClass p_class)
{
  int n;
  switch (parent_index) {
  case 0:
    parent = XmCreateBulletinBoard(Shell1, "parent", NULL, 0);
    break;
  case 1:
    parent = XmCreateDrawingArea(Shell1, "parent", NULL, 0);
    break;
  case 2:
    parent = XmCreateFrame(Shell1, "parent", NULL, 0);
    break;
  case 3:
    parent = XmCreateMainWindow(Shell1, "parent", NULL, 0);
    break;
  case 4:
    parent = XmCreateForm(Shell1, "parent", NULL, 0);
    break;
  }
  return;
}

static void CreateChild(char *name)
{
  register int n;
  static char strbuf[50];
  
  n = 0;
  XtSetArg(args[n], XmNhighlightOnEnter, True); n++;
  XtSetArg(args[n], XmNhighlightThickness, 2); n++;
  if (name == NULL)
    {
      sprintf(strbuf, "%d", num_children + 1);
      child[num_children] = XmCreatePushButton(RC, strbuf, args, n); 
    }
  else
    child[num_children] = XmCreatePushButton(RC, name, args, n); 
  XtManageChild(child[num_children]);
  XtAddCallback(child[num_children], XmNactivateCallback, 
		print_width_height, NULL);
  num_children++;
  return;
}

WidgetClass
ParentClass(char *parent_class_name)
{
  int i;

  for (i = 0; i <= NUM_LEGAL_PARENTS - 1; i++) 
    if (strcmp(parent_class_name, parent_table[i].string) == 0)
      {
	parent_index = i;
	return parent_table[i].class;
      }

  return ((WidgetClass) NULL);

}

void
InitializeClassLookupTables()
{
  int n;
  n = 0;
  child_table[n].class = xmScaleWidgetClass; n++;
  child_table[n].class = xmScrollBarWidgetClass; n++;
  child_table[n].class = xmCommandWidgetClass; n++;
  child_table[n].class = xmArrowButtonWidgetClass; n++;
  child_table[n].class = xmListWidgetClass; n++;
  child_table[n].class = xmSeparatorWidgetClass; n++;
  child_table[n].class = xmTextWidgetClass; n++;
  child_table[n].class = xmTextFieldWidgetClass; n++;
  child_table[n].class = xmLabelWidgetClass; n++;
  child_table[n].class = xmDrawnButtonWidgetClass; n++;
  child_table[n].class = xmPushButtonWidgetClass; n++;
  child_table[n].class = xmToggleButtonWidgetClass; n++;
  child_table[n].class = xmArrowButtonGadgetClass; n++;
  child_table[n].class = xmSeparatorGadgetClass; n++;
  child_table[n].class = xmLabelGadgetClass; n++;
  child_table[n].class = xmPushButtonGadgetClass; n++;
  child_table[n].class = xmToggleButtonGadgetClass; n++;

  n = 0;
  parent_table[n].class = xmBulletinBoardWidgetClass; n++;
  parent_table[n].class = xmDrawingAreaWidgetClass; n++;
  parent_table[n].class = xmFrameWidgetClass; n++;
  parent_table[n].class = xmMainWindowWidgetClass; n++;
  parent_table[n].class = xmFormWidgetClass; n++;
}

void
CreateControlPanel()
{
  Widget parenttype;
  Widget controlpanel, textf1, labeltext1, labelscale1, scale1;
  Widget labelnumcolumns, numcolscale, mainParent, labelpacking;
  Widget entryborder, marginheight, marginwidth, spacing, spacingscale;
  Widget actionsBoard, reportPanel, formPanel, kidPanel, rbox1, rbox2;
  Widget horizontal, vertical, rbox3, align_begin,align_center, align_end;
  Widget pack_tight, pack_none, pack_col, button1, button2, clear1, clear2;
  Widget labelAlign, entborscale, widthscale, heightscale, labelOrient;
  Widget separator, adjustlast, adjustmargin, isaligned, resizeheight;
  Widget resetPanel, resizewidth, rcwidth, rcheight, widthtext,heighttext;
  Widget shadow, shadowscale;
  
  XmString tcs, tcs1, tcs2;

  register int i, n;
  char buf[32];

  n=0;
  XtSetArg (args[n], XmNy, 200); n++;
  XtSetArg(args[n], XtNallowShellResize, True); n++;
  Popup = XtCreatePopupShell ("Popup", topLevelShellWidgetClass, Shell1, args,
                              n);


  n = 0;
  XtSetArg (args[n], XmNallowResize, True); n++;
  controlpanel = XmCreateBulletinBoard(Popup, "Control Panel", args, n);

  n = 0;
  mainParent = XmCreatePanedWindow(controlpanel, "mainParent", args, n);
 
  n = 0;
  actionsBoard = XmCreateBulletinBoard(mainParent, "actions", args, n);

  n = 0;
  tcs = XmStringCreateLtoR("Parent Widget Class:  ", XmSTRING_DEFAULT_CHARSET);
  tcs1 = XmStringCreateLtoR(parent_table[parent_index].string,
			    XmSTRING_DEFAULT_CHARSET);
  tcs2 = XmStringConcat(tcs, tcs1);
  XtSetArg(args[n], XmNlabelString, tcs2); n++;
  XtSetArg(args[n], XmNx, 10); n++;
  XtSetArg(args[n], XmNy, 10); n++;
  parenttype = XmCreateLabel(actionsBoard, "label2", args, n);
  XtManageChild(parenttype);
  XmStringFree(tcs);
  XmStringFree(tcs1);
  XmStringFree(tcs2);


  n = 0;
  tcs = XmStringCreateLtoR("Button Label String: ", XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  XtSetArg(args[n], XmNx, 10); n++;
  XtSetArg(args[n], XmNy, 48); n++;
  labeltext1 = XmCreateLabel(actionsBoard, "label", args, n);
  XtManageChild(labeltext1);
  XmStringFree(tcs);

  n = 0;
  XtSetArg(args[n], XmNcolumns, 15); n++;
  XtSetArg(args[n], XmNy, 40); n++;
  XtSetArg(args[n], XmNx, 150); n++;
  textf1 = XmCreateTextField(actionsBoard, "textf1", args, n);
  XtManageChild(textf1);
  XtAddCallback(textf1, XmNactivateCallback, updateButtonLabels, NULL);
  
  n = 0;
  tcs = XmStringCreateLtoR("Number of Children: ", XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  XtSetArg(args[n], XmNx, 10); n++;
  XtSetArg(args[n], XmNy, 90); n++;
  labelscale1 = XmCreateLabel(actionsBoard, "label2", args, n);
  XtManageChild(labelscale1);
  XmStringFree(tcs);

  n = 0;
  XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
  XtSetArg(args[n], XmNmaximum, MAX_CHILDREN); n++;
  XtSetArg(args[n], XmNminimum, 1); n++;
  XtSetArg(args[n], XmNx, 150); n++;
  XtSetArg(args[n], XmNy, 75); n++;
  XtSetArg(args[n], XmNshowValue, True); n++;
  scale1 = XmCreateScale(actionsBoard, "scale1", args, n);
  XtManageChild(scale1);
  XtAddCallback(scale1, XmNvalueChangedCallback, updateChildren, NULL);

  n = 0;
  tcs = XmStringCreateLtoR("XmNnumColumns: ", XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  XtSetArg(args[n], XmNx, 10); n++;
  XtSetArg(args[n], XmNy, 130); n++;
  labelnumcolumns = XmCreateLabel(actionsBoard, "label3", args, n);
  XtManageChild(labelnumcolumns);
  XmStringFree(tcs);

  n = 0;
  XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
  XtSetArg(args[n], XmNmaximum, 15); n++;
  XtSetArg(args[n], XmNminimum, 1); n++;
  XtSetArg(args[n], XmNx, 150); n++;
  XtSetArg(args[n], XmNy, 115); n++;
  XtSetArg(args[n], XmNshowValue, True); n++;
  numcolscale = XmCreateScale(actionsBoard, "numcolscale", args, n);
  XtManageChild(numcolscale);
  XtAddCallback(numcolscale, XmNvalueChangedCallback, updateNumColumns, NULL);

  n = 0;
  tcs = XmStringCreateLtoR("XmNentryBorder: ", XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  XtSetArg(args[n], XmNx, 10); n++;
  XtSetArg(args[n], XmNy, 170); n++;
  entryborder = XmCreateLabel(actionsBoard, "entryBorder", args, n);
  XtManageChild(entryborder);
  XmStringFree(tcs);

  n = 0;
  XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
  XtSetArg(args[n], XmNmaximum, 15); n++;
  XtSetArg(args[n], XmNminimum, 0); n++;
  XtSetArg(args[n], XmNx, 150); n++;
  XtSetArg(args[n], XmNy, 155); n++;
  XtSetArg(args[n], XmNshowValue, True); n++;
  entborscale = XmCreateScale(actionsBoard, "entborscale", args, n);
  XtManageChild(entborscale);
  XtAddCallback(entborscale, XmNvalueChangedCallback, updateEntryBorder, NULL);

  n = 0;
  tcs = XmStringCreateLtoR("XmNspacing: ", XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  XtSetArg(args[n], XmNx, 10); n++;
  XtSetArg(args[n], XmNy, 210); n++;
  spacing = XmCreateLabel(actionsBoard, "spacing", args, n);
  XtManageChild(spacing);
  XmStringFree(tcs);

  n = 0;
  XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
  XtSetArg(args[n], XmNmaximum, 15); n++;
  XtSetArg(args[n], XmNminimum, 0); n++;
  XtSetArg(args[n], XmNx, 150); n++;
  XtSetArg(args[n], XmNy, 195); n++;
  XtSetArg(args[n], XmNshowValue, True); n++;
  XtSetArg(args[n], XmNvalue, 3); n++;
  spacingscale = XmCreateScale(actionsBoard, "spacingscale", args, n);
  XtManageChild(spacingscale);
  XtAddCallback(spacingscale, XmNvalueChangedCallback, updateSpacing, NULL);

  n = 0;
  tcs = XmStringCreateLtoR("XmNmarginHeight: ", XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  XtSetArg(args[n], XmNx, 10); n++;
  XtSetArg(args[n], XmNy, 250); n++;
  marginheight = XmCreateLabel(actionsBoard, "marginheight", args, n);
  XtManageChild(marginheight);
  XmStringFree(tcs);

  n = 0;
  XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
  XtSetArg(args[n], XmNmaximum, 20); n++;
  XtSetArg(args[n], XmNminimum, 0); n++;
  XtSetArg(args[n], XmNx, 150); n++;
  XtSetArg(args[n], XmNy, 235); n++;
  XtSetArg(args[n], XmNshowValue, True); n++;
  XtSetArg(args[n], XmNvalue, 3); n++;
  heightscale = XmCreateScale(actionsBoard, "heightscale", args, n);
  XtManageChild(heightscale);
  XtAddCallback(heightscale, XmNvalueChangedCallback, updateHeight, NULL);

  n = 0;
  tcs = XmStringCreateLtoR("XmNmarginWidth: ", XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  XtSetArg(args[n], XmNx, 10); n++;
  XtSetArg(args[n], XmNy, 290); n++;
  marginwidth = XmCreateLabel(actionsBoard, "marginwidth", args, n);
  XtManageChild(marginwidth);
  XmStringFree(tcs);

  n = 0;
  XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
  XtSetArg(args[n], XmNmaximum, 20); n++;
  XtSetArg(args[n], XmNminimum, 0); n++;
  XtSetArg(args[n], XmNx, 150); n++;
  XtSetArg(args[n], XmNy, 275); n++;
  XtSetArg(args[n], XmNshowValue, True); n++;
  XtSetArg(args[n], XmNvalue, 3); n++;
  widthscale = XmCreateScale(actionsBoard, "widthscale", args, n);
  XtManageChild(widthscale);
  XtAddCallback(widthscale, XmNvalueChangedCallback, updateWidth, NULL);

  n = 0;
  tcs = XmStringCreateLtoR("XmNshadowThickness: ", XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  XtSetArg(args[n], XmNx, 10); n++;
  XtSetArg(args[n], XmNy, 330); n++;
  shadow = XmCreateLabel(actionsBoard, "shadowthickness", args, n);
  XtManageChild(shadow);
  XmStringFree(tcs);

  n = 0;
  XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
  XtSetArg(args[n], XmNmaximum, 20); n++;
  XtSetArg(args[n], XmNminimum, 0); n++;
  XtSetArg(args[n], XmNx, 150); n++;
  XtSetArg(args[n], XmNy, 315); n++;
  XtSetArg(args[n], XmNshowValue, True); n++;
  XtSetArg(args[n], XmNvalue, 2); n++;
  shadowscale = XmCreateScale(actionsBoard, "shadowscale", args, n);
  XtManageChild(shadowscale);
  XtAddCallback(shadowscale, XmNvalueChangedCallback, updateShadow, NULL);

  n = 0;
  tcs = XmStringCreateLtoR("XmNpacking: ", XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  XtSetArg(args[n], XmNx, 10); n++;
  XtSetArg(args[n], XmNy, 370); n++;
  labelpacking = XmCreateLabel(actionsBoard, "labelpacking", args, n);
  XtManageChild(labelpacking);
  XmStringFree(tcs);

  n = 0;
  XtSetArg(args[n], XmNisHomogeneous, True); n++;
  XtSetArg(args[n], XmNradioAlwaysOne, True); n++;
  XtSetArg(args[n], XmNborderWidth, 1); n++;
  XtSetArg(args[n], XmNx, 15); n++;
  XtSetArg(args[n], XmNy, 390); n++;
  rbox1 = XmCreateRadioBox (actionsBoard, "rbox1", args, n);

  n = 0;
  tcs = XmStringCreateLtoR("XmPACK_TIGHT", XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  pack_tight = XmCreateToggleButtonGadget(rbox1, "pack_tight", args, n);
  XtManageChild(pack_tight);
  XmStringFree(tcs);
  XtAddCallback(pack_tight, XmNvalueChangedCallback, updatePacking, 
		(XtPointer) XmPACK_TIGHT);

  n = 0;
  tcs = XmStringCreateLtoR("XmPACK_COLUMN", XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  XtSetArg(args[n], XmNset, True); n++;
  pack_col = XmCreateToggleButtonGadget(rbox1, "pack_col", args, n);
  XtManageChild(pack_col);
  XmStringFree(tcs);
  XtAddCallback(pack_col, XmNvalueChangedCallback, updatePacking, 
		(XtPointer) XmPACK_COLUMN);

  n = 0;
  tcs = XmStringCreateLtoR("XmPACK_NONE", XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  pack_none = XmCreateToggleButtonGadget(rbox1, "pack_none", args, n);
  XtManageChild(pack_none);
  XmStringFree(tcs);
  XtAddCallback(pack_none, XmNvalueChangedCallback, updatePacking, 
		(XtPointer) XmPACK_NONE);
    

  n = 0;
  tcs = XmStringCreateLtoR("XmNorientation: ", XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  XtSetArg(args[n], XmNx, 152); n++;
  XtSetArg(args[n], XmNy, 370); n++;
  labelOrient = XmCreateLabel(actionsBoard, "labelOrient", args, n);
  XtManageChild(labelOrient);
  XmStringFree(tcs);

  n = 0;
  XtSetArg(args[n], XmNisHomogeneous, True); n++;
  XtSetArg(args[n], XmNradioAlwaysOne, True); n++;
  XtSetArg(args[n], XmNborderWidth, 1); n++;
  XtSetArg(args[n], XmNx, 155); n++;
  XtSetArg(args[n], XmNy, 390); n++;
  rbox2 = XmCreateRadioBox (actionsBoard, "rbox2", args, n);

  n = 0;
  tcs = XmStringCreateLtoR("XmHORIZONTAL", XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  horizontal = XmCreateToggleButtonGadget(rbox2, "horizontal", args, n);
  XtManageChild(horizontal);
  XmStringFree(tcs);
  XtAddCallback(horizontal, XmNvalueChangedCallback, updateOrientation, 
		(XtPointer) XmHORIZONTAL);
    
  n = 0;
  tcs = XmStringCreateLtoR("XmVERTICAL", XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  XtSetArg(args[n], XmNset, True); n++;
  vertical = XmCreateToggleButtonGadget(rbox2, "vertical", args, n);
  XtManageChild(vertical);
  XmStringFree(tcs);
  XtAddCallback(vertical, XmNvalueChangedCallback, updateOrientation, 
		(XtPointer) XmVERTICAL);
  XtManageChild(rbox1);
  XtManageChild(rbox2);

  n = 0;
  tcs = XmStringCreateLtoR("XmNadjustLast", XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  XtSetArg(args[n], XmNx, 315); n++;
  XtSetArg(args[n], XmNy, 40); n++;
  XtSetArg(args[n], XmNset, True); n++;
  adjustlast = XmCreateToggleButton(actionsBoard, "adjustlast", args, n);
  XtManageChild(adjustlast);
  XmStringFree(tcs);
  XtAddCallback(adjustlast, XmNvalueChangedCallback, updateAdjustLast, NULL);
  
  n = 0;
  tcs = XmStringCreateLtoR("XmNadjustMargin", XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  XtSetArg(args[n], XmNx, 315); n++;
  XtSetArg(args[n], XmNy, 70); n++;
  XtSetArg(args[n], XmNset, True); n++;
  adjustmargin = XmCreateToggleButton(actionsBoard, "adjustmargin", args, n);
  XtManageChild(adjustmargin);
  XmStringFree(tcs);
  XtAddCallback(adjustmargin, XmNvalueChangedCallback, updateAdjustMargin, 
		NULL);

  n = 0;
  tcs = XmStringCreateLtoR("XmNisAligned", XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  XtSetArg(args[n], XmNx, 315); n++;
  XtSetArg(args[n], XmNy, 100); n++;
  XtSetArg(args[n], XmNset, True); n++;
  isaligned = XmCreateToggleButton(actionsBoard, "isAligned", args, n);
  XtManageChild(isaligned);
  XmStringFree(tcs);
  XtAddCallback(isaligned, XmNvalueChangedCallback, updateIsAligned, NULL);

  n = 0;
  tcs = XmStringCreateLtoR("XmNresizeHeight", XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  XtSetArg(args[n], XmNx, 315); n++;
  XtSetArg(args[n], XmNy, 130); n++;
  XtSetArg(args[n], XmNset, True); n++;
  resizeheight = XmCreateToggleButton(actionsBoard, "resizeHeight", args, n);
  XtManageChild(resizeheight);
  XmStringFree(tcs);
  XtAddCallback(resizeheight, XmNvalueChangedCallback, updateResizeHeight, 
		NULL);

  n = 0;
  tcs = XmStringCreateLtoR("XmNresizeWidth", XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  XtSetArg(args[n], XmNx, 315); n++;
  XtSetArg(args[n], XmNy, 160); n++;
  XtSetArg(args[n], XmNset, True); n++;
  resizewidth = XmCreateToggleButton(actionsBoard, "resizewidth", args, n);
  XtManageChild(resizewidth);
  XmStringFree(tcs);
  XtAddCallback(resizewidth, XmNvalueChangedCallback, updateResizeWidth, NULL);

  n = 0;
  XtSetArg(args[n], XmNx, 315); n++;
  XtSetArg(args[n], XmNy, 205); n++;
  XtSetArg(args[n], XmNmarginWidth, 5); n++;
  XtSetArg(args[n], XmNmarginHeight, 5); n++;
  resetPanel = XmCreatePushButton(actionsBoard, "Reset_Panel", args, n);
  XtManageChild(resetPanel);
  XtAddCallback(resetPanel, XmNactivateCallback, resetControlPanel, 
		(XtPointer) controlpanel);

  n = 0;
  tcs = XmStringCreateLtoR("XmNwidth: ", XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  XtSetArg(args[n], XmNx, 315); n++;
  XtSetArg(args[n], XmNy, 255); n++; 
  rcwidth = XmCreateLabel(actionsBoard, "rcwidth", args, n);
  XtManageChild(rcwidth);
  XmStringFree(tcs);

  n = 0;
  XtSetArg(args[n], XmNcolumns, 15); n++;
  XtSetArg(args[n], XmNx, 390); n++;
  XtSetArg(args[n], XmNy, 250); n++;
  widthtext = XmCreateTextField(actionsBoard, "widthtext", args, n);
  XtManageChild(widthtext);
  XtAddCallback(widthtext, XmNactivateCallback, updateRCwidth, NULL);

  n = 0;
  tcs = XmStringCreateLtoR("XmNheight: ", XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  XtSetArg(args[n], XmNx, 315); n++;
  XtSetArg(args[n], XmNy, 290); n++;
  rcheight = XmCreateLabel(actionsBoard, "rcheight", args, n);
  XtManageChild(rcheight);
  XmStringFree(tcs);

  n = 0;
  XtSetArg(args[n], XmNcolumns, 15); n++;
  XtSetArg(args[n], XmNx, 390); n++;
  XtSetArg(args[n], XmNy, 285); n++;
  heighttext = XmCreateTextField(actionsBoard, "heighttext", args, n);
  XtManageChild(heighttext);
  XtAddCallback(heighttext, XmNactivateCallback, updateRCheight, NULL);

  n = 0;
  tcs = XmStringCreateLtoR("XmNentryAlignment: ", XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  XtSetArg(args[n], XmNx, 315); n++;
  XtSetArg(args[n], XmNy, 370); n++;
  labelAlign = XmCreateLabel(actionsBoard, "labelAlign", args, n);
  XtManageChild(labelAlign);
  XmStringFree(tcs);

  n = 0;
  XtSetArg(args[n], XmNisHomogeneous, True); n++;
  XtSetArg(args[n], XmNradioAlwaysOne, True); n++;
  XtSetArg(args[n], XmNborderWidth, 1); n++;
  XtSetArg(args[n], XmNx, 320); n++;
  XtSetArg(args[n], XmNy, 390); n++;
  rbox3 = XmCreateRadioBox (actionsBoard, "rbox3", args, n);
  XtManageChild(rbox3);

  n = 0;
  tcs = XmStringCreateLtoR("XmALIGNMENT_BEGINNING", XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  XtSetArg(args[n], XmNset, True); n++;
  align_begin = XmCreateToggleButtonGadget(rbox3, "align_begin", args, n);
  XtManageChild(align_begin);
  XmStringFree(tcs);
  XtAddCallback(align_begin, XmNvalueChangedCallback, updateAlignment, 
		(XtPointer) XmALIGNMENT_BEGINNING);

  n = 0;
  tcs = XmStringCreateLtoR("XmALIGNMENT_CENTER", XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  align_center = XmCreateToggleButtonGadget(rbox3, "align_center", args, n);
  XtManageChild(align_center);
  XmStringFree(tcs);
  XtAddCallback(align_center, XmNvalueChangedCallback, updateAlignment, 
		(XtPointer) XmALIGNMENT_CENTER);

  n = 0;
  tcs = XmStringCreateLtoR("XmALIGNMENT_END", XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  align_end = XmCreateToggleButtonGadget(rbox3, "align_end", args, n);
  XtManageChild(align_end);
  XmStringFree(tcs);
  XtAddCallback(align_end, XmNvalueChangedCallback, updateAlignment, 
		(XtPointer) XmALIGNMENT_END);

  XtManageChild(actionsBoard);
  XtManageChild(mainParent);
  XtManageChild(controlpanel);

  XtPopup (Popup,XtGrabNone);


}
