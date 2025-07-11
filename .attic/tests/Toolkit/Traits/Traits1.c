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
static char rcsid[] = "$XConsortium: Traits1.c /main/5 1995/07/13 19:39:47 drk $"
#endif
#endif

#include <testlib.h>
/*#include <X11/IntrinsicP.h>*/
#include <Xm/XmP.h>
#include <Xm/CareVisualT.h>
#include <Xm/DialogSavvyT.h>
#include <Xm/ActivatableT.h>
#include <Xm/SpecRenderT.h>
#include <Xm/TakesDefT.h>
#include <Xm/NavigatorT.h>
#include <Xm/ScrollFrameT.h>
#include <Xm/AccTextT.h>
#include <Xm/JoinSideT.h>
#include <Xm/MenuT.h>
#include <Xm/TransferT.h>

typedef struct
{
    char *name;
    Widget w;
    Widget (*createfunction) ();
} WidgetStruct;

static WidgetStruct managerwidgets[] =
{
    {"BulletinBoard", NULL, XmCreateBulletinBoard},
    {"SelectionBox", NULL, XmCreateSelectionBox},
    {"FileSelectionBox", NULL, XmCreateFileSelectionBox},
    {"Command", NULL, XmCreateCommand},
    {"MessageBox", NULL, XmCreateMessageBox},
    {"Form", NULL, XmCreateForm},
    {"RowColumn", NULL, XmCreateRowColumn},
    {"DrawingArea", NULL, XmCreateDrawingArea},
    {"PanedWindow", NULL, XmCreatePanedWindow},
    {"Frame", NULL, XmCreateFrame},
    {"Scale", NULL, XmCreateScale},
    {"MainWindow", NULL, XmCreateMainWindow},
    {"Notebook", NULL, XmCreateNotebook},
    {"SpinBox", NULL, XmCreateSpinBox},
    {NULL, NULL, NULL}
};

static WidgetStruct primitivewidgets[] =
{
    {"Label", NULL, XmCreateLabel},
    {"CascadeButton", NULL, XmCreateCascadeButton},
    {"DrawnButton", NULL, XmCreateDrawnButton},
    {"PushButton", NULL, XmCreatePushButton},
    {"ToggleButton", NULL, XmCreateToggleButton},
    {"ArrowButton", NULL, XmCreateArrowButton},
    {"List", NULL, XmCreateList},
    {"ScrollBar", NULL, XmCreateScrollBar},
    {"Separator", NULL, XmCreateSeparator},
    {"Text", NULL, XmCreateText},
    {"TextField", NULL, XmCreateTextField},
    {NULL, NULL, NULL}
};

static WidgetStruct activatablewidgets[] =
{
    {"DrawnButton", NULL, XmCreateDrawnButton},
    {"PushButton", NULL, XmCreatePushButton},
    {"ArrowButton", NULL, XmCreateArrowButton},
    {NULL, NULL, NULL}
};

static WidgetStruct activatorwidgets[] =
{
    {"SelectionBox", NULL, XmCreateSelectionBox},
    {"FileSelectionBox", NULL, XmCreateFileSelectionBox},
    {"Command", NULL, XmCreateCommand},
    {"MessageBox", NULL, XmCreateMessageBox},
    {"Notebook", NULL, XmCreateNotebook},
    {NULL, NULL, NULL}
};

static WidgetStruct rendertablewidgets[] =
{
    {"BulletinBoard", NULL, XmCreateBulletinBoard},
    {NULL, NULL, NULL}
};

static WidgetStruct takesdefaultwidgets [] = 
{
    {"PushButton", NULL, XmCreatePushButton},
    {NULL, NULL, NULL}
};

static WidgetStruct navwidgets[] = 
{
    {"ScrollBar", NULL, XmCreateScrollBar},
    {"SpinBox", NULL, XmCreateSpinBox},
    {NULL, NULL, NULL}
};

static WidgetStruct navigatorwidgets[] =
{
    {"ScrolledWindow", NULL, XmCreateScrolledWindow},
    {"List", NULL, XmCreateList},
    {"Text", NULL, XmCreateText},
    {"Notebook", NULL, XmCreateNotebook},
    {NULL, NULL, NULL}
};

static WidgetStruct scrollframewidgets[] =
{
    {"ScrolledWindow", NULL, XmCreateScrolledWindow},
    {"Notebook", NULL, XmCreateNotebook},
    {NULL, NULL, NULL}
};

static WidgetStruct scrollablewidgets[] =
{
    {"List", NULL, XmCreateList},
    {"Text", NULL, XmCreateText},
    {"ScrolledWindow", NULL, XmCreateScrolledWindow},
    {"Notebook", NULL, XmCreateNotebook},
    {NULL, NULL, NULL}
};

static WidgetStruct accesswidgets[] =
{
    {"Notebook", NULL, XmCreateNotebook},
    {"ComboBox", NULL, XmCreateComboBox},
    {"SelectionBox", NULL, XmCreateSelectionBox},
    {NULL, NULL, NULL}
};

static WidgetStruct accessablewidgets[] =
{
    {"Text", NULL, XmCreateText},
    {"TextField", NULL, XmCreateTextField},
    {"Label", NULL, XmCreateLabel},
    {NULL, NULL, NULL}
};

static WidgetStruct menusavvywidgets[] =
{
    {"Label", NULL, XmCreateLabel},
    {"CascadeButton", NULL, XmCreateCascadeButton},
    {"DrawnButton", NULL, XmCreateDrawnButton},
    {"PushButton", NULL, XmCreatePushButton},
    {"ToggleButton", NULL, XmCreateToggleButton},
    {"Separator", NULL, XmCreateSeparator},
    {NULL, NULL, NULL}
};

typedef struct
{
    char *name;
    Widget g;
    Widget (*createfunction) ();
} GadgetStruct;

static GadgetStruct gadgets[] = 
{
    {"LabelGadget", NULL, XmCreateLabelGadget},
    {"CascadeButtonGadget", NULL, XmCreateCascadeButtonGadget},
    {"PushButtonGadget", NULL, XmCreatePushButtonGadget},
    {"ToggleButtonGadget", NULL, XmCreateToggleButtonGadget},
    {"ArrowButtonGadget", NULL, XmCreateArrowButtonGadget},
    {"SeparatorGadget", NULL, XmCreateSeparatorGadget},
    {NULL, NULL, NULL}
};

static GadgetStruct activatablegadgets[] =
{
    {"DrawButtonGadget", NULL, XmCreatePushButtonGadget},
    {"PushButtonGadget", NULL, XmCreatePushButtonGadget},
    {"ArrowButtonGadget", NULL, XmCreateArrowButtonGadget},
    {NULL, NULL, NULL}
};

static GadgetStruct takesdefaultgadgets [] = 
{
    {"PushButtonGadget", NULL, XmCreatePushButtonGadget},
    {NULL, NULL, NULL}
};

static GadgetStruct menusavvygadgets[] = 
{
    {"LabelGadget", NULL, XmCreateLabelGadget},
    {"CascadeButtonGadget", NULL, XmCreateCascadeButtonGadget},
    {"PushButtonGadget", NULL, XmCreatePushButtonGadget},
    {"ToggleButtonGadget", NULL, XmCreateToggleButtonGadget},
    {"SeparatorGadget", NULL, XmCreateSeparatorGadget},
    {NULL, NULL, NULL}
};

void main(argc, argv)
unsigned int argc;
char *argv[];
{
  Widget	DialogShell1, BulletinBoard1, Notebook1;
  Widget	VendorShell1, RowColumn1;
  register	int i, j;

  /*
   * Initialize toolkit
   */

  CommonTestInit(argc,argv);


  XtRealizeWidget(Shell1);

  /*
   * XmQTcareParentVisual test.
   * Loop through each manager widget and with each manager widget,
   * loop through each primitive widget and then gadgets and test if 
   * the XmQTcareParentVisual trait exists
   */

  printf("Testing for XmQTcareParentVisual trait\n");
  for (i = 0; managerwidgets[i].name != NULL; i++)  {
      managerwidgets[i].w = 
	  managerwidgets[i].createfunction(Shell1,
					   managerwidgets[i].name,
					   NULL, 0 );

      for (j = 0; primitivewidgets[j].name != NULL; j++)  {
	  primitivewidgets[j].w = 
	      primitivewidgets[j].createfunction(managerwidgets[i].w, 
						 primitivewidgets[j].name, 
						 NULL, 0);
	  /* loop through primitive widgets */
	  if((XmCareVisualTrait) XmeTraitGet((XtPointer)
					     XtClass(primitivewidgets[j].w), 
					     XmQTcareParentVisual) != NULL)  {
	      printf("XmQTcareParentVisual trait found in %s, through %s\n",
		     managerwidgets[i].name, primitivewidgets[j].name);
	  }
	  else
	      printf("XmQTcareParentVisual trait NOT found in %s with %s\n", 
		     managerwidgets[i].name, primitivewidgets[j].name);
	  XtDestroyWidget(primitivewidgets[j].w);
      }
      XtDestroyWidget(managerwidgets[i].w);
      printf("\n");
  }
  printf("\n\n");

  CommonPause();

  /*
   * XmQTdialogShellSavvy trait test
   */
    
  printf("Testing for XmQTdialogShellSavvy trait\n");

  DialogShell1 = XmCreateDialogShell(Shell1, "DialogShell1", NULL, 0);

  BulletinBoard1 = XmCreateBulletinBoard(DialogShell1, "BulletinBoard1", NULL, 0);
	 
  if((XmDialogSavvyTrait) XmeTraitGet((XtPointer)XtClass(BulletinBoard1), 
					     XmQTdialogShellSavvy) != NULL)  {
      printf("XmQTdialogSavvy trait found in BulletinBoard through DialogShell\n");
  }
  else  {
      printf("XmQTdialogSavvy trait NOT found in BulletinBoard through DialogShell.\n");
  }
  printf("\n\n");
  XtDestroyWidget(DialogShell1);
  

  CommonPause();

  /*
   * XmQTactivatable trait test
   * Loop through all the dialog widgets using PushButton, DrawnButton
   * and ArrowButton widgets and gadgets to test for the existance of
   * the trait.
   */

  printf("Testing for XmQTactivatable trait\n");
  for (i = 0; activatorwidgets[i].name != NULL; i++)  {
      activatorwidgets[i].w = 
	  activatorwidgets[i].createfunction(Shell1,
					   activatorwidgets[i].name, 
					   NULL, 0 );
      for (j = 0; activatablewidgets[j].name != NULL; j++)  {
	  activatablewidgets[j].w = 
	      activatablewidgets[j].createfunction(activatorwidgets[i].w, 
						 activatablewidgets[j].name, 
						 NULL, 0);
	  /* loop through primitive widgets */
	  if((XmActivatableTrait) XmeTraitGet((XtPointer)
					     XtClass(activatablewidgets[j].w), 
					     XmQTactivatable) != NULL)  {
	      printf("XmQTactivatable trait found in %s, through %s\n",
		     activatablewidgets[j].name, activatorwidgets[i].name);
	  }
	  else  {
	      printf("XmQTactivatable trait NOT found in %s through %s\n", 
		     activatablewidgets[j].name, activatorwidgets[i].name);
	  }
	  XtDestroyWidget(activatablewidgets[j].w);
      }
      XtDestroyWidget(activatorwidgets[i].w);
      printf("\n");
  }

  printf("\n");
  printf (" Testing XmQTactivatable trait using gadgets\n");
  for (i = 0; activatorwidgets[i].name != NULL; i++)  {
      activatorwidgets[i].w = 
	  activatorwidgets[i].createfunction(Shell1,
					   activatorwidgets[i].name, 
					   NULL, 0 );
      /* loop through gadgets */
      for (j = 0; activatablegadgets[j].name != NULL; j++)  {

	  activatablegadgets[j].g = 
	      activatablegadgets[j].createfunction(activatorwidgets[i].w, 
					 activatablegadgets[j].name, 
					 NULL, 0);

	  if((XmActivatableTrait) XmeTraitGet((XtPointer)
					     XtClass(activatablegadgets[j].g), 
					     XmQTactivatable) != NULL)  {
	      printf("XmQTactivatable trait found in %s, through %s\n",
		     activatablegadgets[j].name, activatorwidgets[i].name);
	  }
	  else  {
	      printf("XmQTactivatable trait NOT found in %s through %s\n", 
		     activatablegadgets[j].name, activatorwidgets[i].name);
	  }
	  XtDestroyWidget(activatablegadgets[j].g);
      }
      XtDestroyWidget(activatorwidgets[i].w);
      printf("\n");
  }
  printf("\n\n");

  CommonPause();

  /*
   * XmQTspecifyRenderTable test.
   */

  printf("Testing for XmQTspecifyRenderTable trait\n");
  for (j = 0; rendertablewidgets[j].name != NULL; j++)  {
      rendertablewidgets[j].w = 
	  rendertablewidgets[j].createfunction(Shell1, 
						rendertablewidgets[j].name, 
						NULL, 0);

      if((XmRenderTable) XmeTraitGet((XtPointer)
				     XtClass(rendertablewidgets[j].w), 
				     XmQTspecifyRenderTable) != NULL)  {
	  printf("XmQTspecifyRenderTable trait found in %s, through Shell1\n",
		 rendertablewidgets[j].name);
      }
      else  {
	  printf("XmQTspecifyRenderTable trait NOT found in %s through Shell1\n", 
		 rendertablewidgets[j].name);
      }
      XtDestroyWidget(rendertablewidgets[j].w);
  }
  printf("\n\n");

  CommonPause();
  
  /*
   * XmQTtakesDefault test.
   */

  printf("Testing for XmQTtakesDefault trait\n");
  BulletinBoard1 = XmCreateBulletinBoard(Shell1,
					 "BulletinBoard1",
					 NULL, 0 );
  for (j = 0; takesdefaultwidgets[j].name != NULL; j++)  {
      takesdefaultwidgets[j].w = 
	  takesdefaultwidgets[j].createfunction(BulletinBoard1, 
						takesdefaultwidgets[j].name, 
						NULL, 0);
      if((XmTakesDefaultTrait) XmeTraitGet((XtPointer)
					   XtClass(takesdefaultwidgets[j].w), 
					   XmQTtakesDefault) != NULL)  {
	  printf("XmQTtakesDefault trait found in %s, through BulletinBoard\n",
		     takesdefaultwidgets[j].name);
      }
      else  {
	  printf("XmQTtakesDefault trait NOT found in %s through BulletinBoard\n", 
		 takesdefaultwidgets[j].name);
      }
      XtDestroyWidget(takesdefaultwidgets[j].w);
  }
  printf("\n");
  printf (" Testing XmQTtakesDefault trait using gadgets\n");
  /* loop through gadgets */
  for (j = 0; takesdefaultgadgets[j].name != NULL; j++)  {
      takesdefaultgadgets[j].g = 
	  activatablegadgets[j].createfunction(BulletinBoard1, 
					       takesdefaultgadgets[j].name, 
					       NULL, 0);
      if((XmActivatableTrait) XmeTraitGet((XtPointer)
					  XtClass(takesdefaultgadgets[j].g), 
					  XmQTactivatable) != NULL)  {
	  printf("XmQTtakesdefault trait found in %s, through BulletinBoard\n",
		 takesdefaultgadgets[j].name);
      }
      else  {
	  printf("XmQTtakesdefault trait NOT found in %s through BulletinBoard\n", 
		 takesdefaultgadgets[j].name);
      }
  }
  XtDestroyWidget(BulletinBoard1);
  printf("\n\n");

  CommonPause();

  /*
   * XmQTnavigator test.
   */

  for (i = 0; navigatorwidgets[i].name != NULL; i++)  {
      navigatorwidgets[i].w = 
	  navigatorwidgets[i].createfunction(Shell1,
					   navigatorwidgets[i].name, 
					   NULL, 0 );
      for (j = 0; navwidgets[j].name != NULL; j++)  {

	  navwidgets[j].w = 
	      navwidgets[j].createfunction(navigatorwidgets[i].w, 
					      navwidgets[j].name, 
					      NULL, 0);
	  if((XmNavigatorTrait) XmeTraitGet((XtPointer)
					     XtClass(navwidgets[j].w), 
					     XmQTnavigator) != NULL)  {
	      printf("XmQTnavigator trait found in %s, through %s\n",
		     navigatorwidgets[i].name, navwidgets[j].name);
	  }
	  else  {
	      printf("XmQTnavigator trait NOT found in %s through %s\n", 
		     navigatorwidgets[i].name, navwidgets[j].name);
	  }
	  XtDestroyWidget(navwidgets[j].w);
      }
      XtDestroyWidget(navigatorwidgets[i].w);
      printf("\n");
  }
  printf("\n\n");

  CommonPause();

  /*
   * XmQTscrollFrame test.
   */

  printf("Testing for XmQTscrollFrame trait\n");
  for (i = 0; scrollframewidgets[i].name != NULL; i++)  {
      scrollframewidgets[i].w = 
	  scrollframewidgets[i].createfunction(Shell1,
					       scrollframewidgets[i].name, 
					       NULL, 0 );

      if((XmScrollFrameTrait) XmeTraitGet((XtPointer)
					  XtClass(scrollframewidgets[i].w), 
					  XmQTscrollFrame) != NULL)  {
	  printf("XmQTscrollFrame trait found in %s, through Shell1\n",
		 scrollframewidgets[i].name);
      }
      else  {
	  printf("XmQTscrollFrame trait NOT found in %s through Shell1\n", 
		 scrollframewidgets[i].name);
      }

      XtDestroyWidget(scrollframewidgets[i].w);
      printf("\n");
  }
  printf("\n\n");

  CommonPause();

  /*
   * XmQTaccessTextual test.
   */

  printf("Testing for XmQTaccessTextual trait\n");
  for (i = 0; accesswidgets[i].name != NULL; i++)  {
      accesswidgets[i].w = 
	  accesswidgets[i].createfunction(Shell1,
					  accesswidgets[i].name, 
					  NULL, 0 );
      for (j = 0; accessablewidgets[j].name != NULL; j++)  {
	  accessablewidgets[j].w = 
	      accessablewidgets[j].createfunction(accesswidgets[i].w, 
						  accessablewidgets[j].name, 
						  NULL, 0);
	  if((XmAccessTextualTrait) XmeTraitGet((XtPointer)
					 XtClass(accessablewidgets[j].w), 
					 XmQTaccessTextual) != NULL)  {
	      printf("XmQTaccesTextual trait found in %s, through %s\n",
		     accessablewidgets[j].name, accesswidgets[i].name);
	  }
	  else  {
	      printf("XmQTaccessTextual trait NOT found in %s through %s\n", 
		     accessablewidgets[j].name, accesswidgets[i].name);
	  }
	  XtDestroyWidget(accessablewidgets[j].w);
      }
      XtDestroyWidget(accesswidgets[i].w);
      printf("\n");
  }

  printf("\n\n");

  CommonPause();

  /*
   * XmQTmenuSystem and XmQTmenuSavvy test
   */

  printf("Testing for XmQTmenuSystem and XmQTmenuSavvy traits \n");
  RowColumn1 =  XmCreateRowColumn(Shell1, "RowColumn1", NULL, 0 );

  if((XmMenuSystemTrait) XmeTraitGet((XtPointer)
				     XtClass(RowColumn1), 
				     XmQTmenuSystem) != NULL)  
      printf("XmQTmenuSystem trait found in RowColumn, through Shell1 \n");
  else 
      printf("XmQTmenuSystem trait NOT found in RowColumn, through Shell1\n\n"); 


  /* loop through widgets */
  for (j = 0; menusavvywidgets[j].name != NULL; j++)  {
      menusavvywidgets[j].w = 
	  menusavvywidgets[j].createfunction(RowColumn1,
					     menusavvywidgets[j].name, 
					     NULL, 0);
      if((XmMenuSavvyTrait) XmeTraitGet((XtPointer)
					 XtClass(menusavvywidgets[j].w), 
					 XmQTmenuSavvy) != NULL)  {
	  printf("XmQTmenuSavvy trait found in %s, through %s\n",
		 menusavvywidgets[j].name, "RowColumn");
      }
      else  {
	  printf("XmQTmenuSavvy trait NOT found in %s through %s\n", 
		 menusavvywidgets[j].name, "RowColumn");
      }
      XtDestroyWidget(menusavvywidgets[j].w);
      printf("\n");
  }

  /* loop through gadgets */
  for (j = 0; menusavvygadgets[j].name != NULL; j++)  {
      menusavvygadgets[j].g = 
	  menusavvygadgets[j].createfunction(RowColumn1,
					     menusavvygadgets[j].name, 
					     NULL, 0);
      if((XmMenuSavvyTrait) XmeTraitGet((XtPointer)
					 XtClass(menusavvygadgets[j].g), 
					 XmQTmenuSavvy) != NULL)  {
	  printf("XmQTmenuSavvy trait found in %s, through %s\n",
		 menusavvygadgets[j].name, "RowColumn");
      }
      else  {
	  printf("XmQTmenuSavvy trait NOT found in %s through %s\n", 
		 menusavvygadgets[j].name, "RowColumn");
      }
      XtDestroyWidget(menusavvygadgets[j].g);
      printf("\n");
  }

  XtDestroyWidget(RowColumn1);
  printf("\n\n");

  CommonPause();

}


