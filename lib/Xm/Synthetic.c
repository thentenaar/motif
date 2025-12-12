/* $TOG: Synthetic.c /main/8 1999/04/13 13:21:09 mgreess $ */
/*
 * Motif
 *
 * Copyright (c) 1987-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2.1 of the License, or (at your option)
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include "XmI.h"
#include <Xm/ManagerP.h>
#include <Xm/PrimitiveP.h>
#include <Xm/GadgetP.h>
#include <Xm/VendorSEP.h>
#if XM_PRINTING
#include <Xm/PrintSP.h>
#endif
#include "SyntheticI.h"

/**
 * For conversion between a String and a XrmQuark
 */
union string_quark {
  String str;
  XrmQuark quark;
};

/*******    Static Function Declarations    ********/

static void GetValuesHook(Widget w,
			  XtPointer base,
			  Widget alt_w,
			  XtPointer alt_base,
			  Cardinal alt_mask,
			  XmSyntheticResource *resources,
			  int num_resources,
			  ArgList args,
			  Cardinal num_args) ;
static void ConstraintGetValuesHook(Widget w,
				    Widget alt_w,
				    Cardinal alt_mask,
				    ArgList args,
				    Cardinal *num_args) ;
static void ImportArgs(Widget w,
		       XtPointer base,
		       Widget alt_w,
		       XtPointer alt_base,
		       Cardinal alt_mask,
		       XmSyntheticResource *resources,
		       int num_resources,
		       ArgList args,
		       Cardinal num_args) ;
static void ImportConstraintArgs(Widget w,
				 Widget alt_w,
				 Cardinal alt_mask,
				 ArgList args,
				 Cardinal *num_args) ;


/********    End Static Function Declarations    ********/

/**********************************************************************
 *
 *  _XmBuildResources
 *	Build up a new synthetic resource list based on a combination
 *	the the widget's class and super class resource list.
 *
 **********************************************************************/

void
_XmBuildResources(XmSyntheticResource **wc_resources_ptr,
		  int *wc_num_resources_ptr,
		  XmSyntheticResource *sc_resources,
		  int sc_num_resources)
{
  XmSyntheticResource *wc_resources;
  int                  wc_num_resources;
  XmSyntheticResource *new_resources;
  int                  new_num_resources;
  int i, j;
  Boolean override;


  wc_resources = (XmSyntheticResource *) *wc_resources_ptr;
  wc_num_resources = (int) *wc_num_resources_ptr;


  /*  If there are no new resources, just use the super class data  */

  if (wc_num_resources == 0)
    {
      *wc_resources_ptr = sc_resources;
      *wc_num_resources_ptr = sc_num_resources;
      return;
    }


  /*
   * Allocate a new resource list to contain the combined set of
   * resources from the class and super class.  This allocation
   * may create to much space if there are overrides in the new
   * resource list.  Copy sc's resources into the space.
   */

  new_resources = (XmSyntheticResource *)
    XtMalloc(sizeof (XmSyntheticResource) *
	     (wc_num_resources + sc_num_resources));
  if (sc_num_resources)
    memcpy ((char *) new_resources, (char *) sc_resources,
	    sc_num_resources * sizeof (XmSyntheticResource));


  /*
   * Loop through the wc resources and copy
   * them into the new resources
   */

  new_num_resources = sc_num_resources;

  for (i = 0; i < wc_num_resources; i++)
    {

      /*  First check to see if this is an override  */

      override = False;
      for (j = 0; j < sc_num_resources; j++)
	{
	  /* ???
	   * We do name overrides while the intrinsics do offset overrides.
	   * Be sure to mask off alt_mask if we change this.
	   */
	  if (new_resources[j].resource_name == wc_resources[i].resource_name)
	    {
	      override = True;
	      new_resources[j].export_proc = wc_resources[i].export_proc;
	      new_resources[j].import_proc = wc_resources[i].import_proc;
	      break;
	    }
	}


      /*  If it wasn't an override stuff it into the list  */

      if (override == False)
	new_resources[new_num_resources++] = wc_resources[i];
    }


  /*  Replace the resource list and count will the new one.  */

  *wc_resources_ptr = new_resources;
  *wc_num_resources_ptr = new_num_resources;
}

/**********************************************************************
 *
 *  InitializeSyntheticResources
 *	Initialize a synthetic resource list.  This is called before
 *	Primitive, Manager and Gadgets build resources to convert the
 *	resource names to quarks in the class' synthetic processing
 *	lists.
 *
 **********************************************************************/

void
_XmInitializeSyntheticResources(XmSyntheticResource *resources,
				int num_resources )
{
  register int i;
  union string_quark q;

  for (i = 0, q.str = NULL; i < num_resources; i++, q.str = NULL) {
    q.quark = XrmPermStringToQuark(resources[i].resource_name);
    resources[i].resource_name = q.str;
  }
}

/**********************************************************************
 *
 *  GetValuesHook
 *	This procedure is used as the synthetic hook in Primitive,
 *	Manager, and Gadget.  It uses the synthetic resource list
 *	attached to the class, comparing it to the input resource list,
 *	and for each match, calling the function to process the get
 *	value data.
 *
 **********************************************************************/

static void
GetValuesHook(Widget w,
	      XtPointer base,
	      Widget alt_w,
	      XtPointer alt_base,
	      Cardinal alt_mask,
	      XmSyntheticResource *resources,
	      int num_resources,
	      ArgList args,
	      Cardinal num_args)
{
  int i, j;
  XrmQuark quark;
  XtArgVal value;
  XtArgVal orig_value;
  Cardinal value_size;
  XtPointer value_ptr;
  Widget value_widget;
  Cardinal value_offset;
  union string_quark q;

  /*  Loop through each argument, quarkifing the name.  Then loop  */
  /*  through each synthetic resource to see if there is a match.  */

  for (i = 0; i < num_args; i++)
    {
      quark = XrmStringToQuark (args[i].name);

      for (j = 0; j < num_resources; j++)
	{
	  q.str = resources[j].resource_name;
	  if (resources[j].export_proc && q.quark == quark)
	    {
	      value_size = resources[j].resource_size;

	      /* CR 5385: Use alt_w and alt_base if alt_mask is set in the */
	      /*	offset.  This lets extension sythetic resources */
	      /*	point to the "real" widget class resources. */
	      value_offset = resources[j].resource_offset;
	      if (value_offset & alt_mask)
		{
		  value_widget = alt_w;
		  value_offset &= ~alt_mask;
		  value_ptr = (XtPointer) ((char *) alt_base + value_offset);
		}
	      else
		{
		  value_widget = w;
		  value_ptr = (XtPointer) ((char *) base + value_offset);
		}

	      if (value_size == sizeof(long))
		value = (XtArgVal)(*(long *)value_ptr);
	      else if (value_size == sizeof(int))
		value = (XtArgVal)(*(int *)value_ptr);
	      else if (value_size == sizeof(short))
		value = (XtArgVal)(*(short *)value_ptr);
	      else if (value_size == sizeof(char))
		value = (XtArgVal)(*(char *)value_ptr);
	      else
		value = *(XtArgVal*)value_ptr;

	      orig_value = value;

	      (*(resources[j].export_proc))
		(value_widget, value_offset, &value);

#if defined(GETVALUES_BUG)
	      /* if the value that was in the widget prior to calling
		 the get hook is the same as the one put by Xt in the
		 arg list, we are in the XtSetArg(args[i], XtNfoo, NULL)
		 case, where args[i].value is supposed to receive the
		 result of the conversion. */
	      if (orig_value == args[i].value)
		{
		  args[i].value = value;
		}
	      else
#endif
		{
		  value_ptr = (XtPointer) args[i].value;

		  if (value_size == sizeof(long))
		    *(long *) value_ptr = (long) value;
		  else if (value_size == sizeof(int))
		    *(int *) value_ptr = (int) value;
		  else if (value_size == sizeof(short))
		    *(short *) value_ptr = (short) value;
		  else if (value_size == sizeof(char))
		    *(char *) value_ptr = (char) value;
		  else
		    *(XtArgVal*) value_ptr = value;
		}

	      break;
	    }
	}
    }
}

/**********************************************************************
 *
 *  ConstraintGetValuesHook
 *	When a widget is a child of a constraint manager, this function
 *	processes the synthetic arg list with for any constraint based
 *	resource processing that needs to be done.
 *
 **********************************************************************/

static void
ConstraintGetValuesHook(Widget w,
			Widget alt_w,
			Cardinal alt_mask,
			ArgList args,
			Cardinal *num_args )
{
  XmManagerWidgetClass parent_wc =
    (XmManagerWidgetClass) w->core.parent->core.widget_class;

  if (XmIsManager(w->core.parent) &&
      parent_wc->manager_class.num_syn_constraint_resources)
    GetValuesHook(w, w->core.constraints,
		  alt_w, alt_w->core.constraints, alt_mask,
		  parent_wc->manager_class.syn_constraint_resources,
		  parent_wc->manager_class.num_syn_constraint_resources,
		  args, *num_args);
}

/**********************************************************************
 *
 *  _XmPrimitiveGetValuesHook
 *	Process the synthetic resources that need to be synthesized
 *
 **********************************************************************/

void
_XmPrimitiveGetValuesHook(
        Widget w,
        ArgList args,
        Cardinal *num_args )
{
  XmPrimitiveWidgetClass wc;

  _XmProcessLock();
  wc = (XmPrimitiveWidgetClass) w->core.widget_class;
  if (wc->primitive_class.num_syn_resources != 0)
    GetValuesHook (w, (XtPointer)w, w, (XtPointer)w, None,
		   wc->primitive_class.syn_resources,
		   wc->primitive_class.num_syn_resources,
		   args, *num_args);

  if (w->core.constraints != NULL)
    ConstraintGetValuesHook (w, w, None, args, num_args);
  _XmProcessUnlock();
}

/**********************************************************************
 *
 *  _XmGadgetGetValuesHook
 *	Process the synthetic resources that need to be synthesized
 *
 **********************************************************************/

void
_XmGadgetGetValuesHook(Widget w,
		       ArgList args,
		       Cardinal *num_args )
{
  XmGadgetClass wc;

  _XmProcessLock();
  wc = (XmGadgetClass) w->core.widget_class;
  if (wc->gadget_class.num_syn_resources != 0)
    GetValuesHook (w, (XtPointer) w, w, (XtPointer) w, None,
		   wc->gadget_class.syn_resources,
		   wc->gadget_class.num_syn_resources,
		   args, *num_args);

  if (w->core.constraints != NULL)
    ConstraintGetValuesHook (w, w, None, args, num_args);
  _XmProcessUnlock();
}

/**********************************************************************
 *
 *  _XmManagerGetValuesHook
 *	Process the synthetic resources that need to be synthesized
 *
 **********************************************************************/

void
_XmManagerGetValuesHook(Widget w,
			ArgList args,
			Cardinal *num_args)
{
  XmManagerWidgetClass wc;

  _XmProcessLock();
  wc = (XmManagerWidgetClass) w->core.widget_class;
  if (wc->manager_class.num_syn_resources != 0)
    GetValuesHook (w, (XtPointer) w, w, (XtPointer) w, None,
		   wc->manager_class.syn_resources,
		   wc->manager_class.num_syn_resources,
		   args, *num_args);

  if (w->core.constraints != NULL)
    ConstraintGetValuesHook (w, w, None, args, num_args);
  _XmProcessUnlock();
}


#if XM_PRINTING

/**********************************************************************
 *
 *  _XmPrintShellGetValuesHook
 *	Process the synthetic resources that need to be synthesized
 *
 **********************************************************************/

void
_XmPrintShellGetValuesHook(Widget w,
			ArgList args,
			Cardinal *num_args)
{
  XmPrintShellWidgetClass wc;

  _XmProcessLock();
  wc = (XmPrintShellWidgetClass) w->core.widget_class;
  if (wc->print_shell_class.num_syn_resources != 0)
    GetValuesHook (w, (XtPointer) w, w, (XtPointer) w, None,
		   wc->print_shell_class.syn_resources,
		   wc->print_shell_class.num_syn_resources,
		   args, *num_args);

  _XmProcessUnlock();
}
#endif /* XM_PRINTING */

/**********************************************************************
 *
 *  _XmExtGetValuesHook
 *	Process the synthetic resources that need to be synthesized
 *
 **********************************************************************/

void
_XmExtGetValuesHook(Widget w,
		    ArgList args,
		    Cardinal *num_args )
{
  XmExtObjectClass wc;
  Widget parent;

  _XmProcessLock();
  wc = (XmExtObjectClass)XtClass(w);
  parent = ((XmExtObject)w)->ext.logicalParent;
  if (wc->ext_class.num_syn_resources != 0)
    GetValuesHook(w, (XtPointer) w,
		  parent, (XtPointer) parent, XmLOGICAL_PARENT_RESOURCE,
		  wc->ext_class.syn_resources,
		  wc->ext_class.num_syn_resources,
		  args, *num_args);
  _XmProcessUnlock();
}

/**********************************************************************
 *
 * ImportArgs
 * Convert the value in the arg list from the application type to the
 * appropriate internal representation by calling the import_proc
 * specified for the given resource.
 *
 **********************************************************************/

static void
ImportArgs(Widget w,
	   XtPointer base,
	   Widget alt_w,
	   XtPointer alt_base,
	   Cardinal alt_mask,
	   XmSyntheticResource *resources,
	   int num_resources,
	   ArgList args,
	   Cardinal num_args )
{
  int i, j;
  XrmQuark quark;
  XtArgVal value;
  Cardinal value_size;
  XtPointer value_ptr;
  Cardinal value_offset;
  Widget value_widget;
  XtPointer value_base;
  XmImportOperator op;
  union string_quark q;

  /*  Loop through each argument, quarkifing the name.  Then loop  */
  /*  through each synthetic resource to see if there is a match.  */

  for (i = 0; i < num_args; i++)
    {
      quark = XrmStringToQuark (args[i].name);

      for (j = 0; j < num_resources; j++)
	{
	  q.str = resources[j].resource_name;
	  if (resources[j].import_proc && q.quark == quark)
	    {
	      value = args[i].value;

	      /* CR 5385: Let VendorSE mix real and extension resources. */
	      value_offset = resources[j].resource_offset;
	      if (value_offset & alt_mask)
		{
		  value_offset &= ~alt_mask;
		  value_widget = alt_w;
		  value_base = alt_base;
		}
	      else
		{
		  value_base = base;
		  value_widget = w;
		}

	      op = (*(resources[j].import_proc))
		(value_widget, value_offset, &value);
	      if ((op == XmSYNTHETIC_LOAD) && (value_base != NULL))
		{
		  /* Load the converted value into the structure */
		  value_size = resources[j].resource_size;
		  value_ptr = (XtPointer) ((char *) value_base + value_offset);

		  if (value_size == sizeof(long))
		    *(long *) value_ptr = (long) value;
		  else if (value_size == sizeof(int))
		    *(int *) value_ptr = (int) value;
		  else if (value_size == sizeof(short))
		    *(short *) value_ptr = (short) value;
		  else if (value_size == sizeof(char))
		    *(char *) value_ptr = (char) value;
		  else
		    *(XtArgVal*) value_ptr= value;
		}
	      else
		{
		  args[i].value = value;
		}

	      break;
	    }
	}
    }
}

/**********************************************************************
 *
 *  ImportConstraintArgs
 *	When a widget is a child of a constraint manager, this function
 *	processes the synthetic arg list with for any constraint based
 *	resource processing that needs to be done.
 *
 **********************************************************************/
static void
ImportConstraintArgs(Widget w,
		     Widget alt_w,
		     Cardinal alt_mask,
		     ArgList args,
		     Cardinal *num_args )
{
  XmManagerWidgetClass parent_wc =
    (XmManagerWidgetClass) w->core.parent->core.widget_class;

  if (XmIsManager(w->core.parent) &&
      parent_wc->manager_class.num_syn_constraint_resources)
    ImportArgs(w, w->core.constraints,
	       alt_w, alt_w->core.constraints, alt_mask,
	       parent_wc->manager_class.syn_constraint_resources,
	       parent_wc->manager_class.num_syn_constraint_resources,
	       args, *num_args);
}

/**********************************************************************
 *
 * _XmExtImportArgs
 * Does arg importing for sub-classes of VendorExt.
 *
 **********************************************************************/

void
_XmExtImportArgs(Widget w,
		 ArgList args,
		 Cardinal *num_args )
{
  XmExtObjectClass wc;
  Widget parent;

  _XmProcessLock();
  wc = (XmExtObjectClass)XtClass(w);
  parent = ((XmExtObject)w)->ext.logicalParent;
  if (wc->ext_class.num_syn_resources != 0)
    ImportArgs(w, (XtPointer) w,
	       parent, (XtPointer)parent, XmLOGICAL_PARENT_RESOURCE,
	       wc->ext_class.syn_resources,
	       wc->ext_class.num_syn_resources,
	       args, *num_args);
  _XmProcessUnlock();
}

/**********************************************************************
 *
 * _XmPrimitiveImportArgs
 * Does arg importing for sub-classes of XmPrimitive.
 *
 **********************************************************************/

void
_XmPrimitiveImportArgs(Widget w,
		       ArgList args,
		       Cardinal *num_args )
{
  XmPrimitiveWidgetClass wc;

  _XmProcessLock();
  wc = (XmPrimitiveWidgetClass) w->core.widget_class;
  if (wc->primitive_class.num_syn_resources != 0)
    ImportArgs (w, (XtPointer) w, w, (XtPointer) w, None,
		wc->primitive_class.syn_resources,
		wc->primitive_class.num_syn_resources,
		args, *num_args);

  if (w->core.constraints != NULL)
    ImportConstraintArgs (w, w, None, args, num_args);
  _XmProcessUnlock();
}

/**********************************************************************
 *
 *  _XmGadgetImportArgs
 * Does arg importing for sub-classes of XmGadget.
 *
 **********************************************************************/

void
_XmGadgetImportArgs(Widget w,
		    ArgList args,
		    Cardinal *num_args )
{
  XmGadgetClass wc;

  /* Main object args */
  _XmProcessLock();
  wc = (XmGadgetClass) w->core.widget_class;
  if (wc->gadget_class.num_syn_resources != 0)
    ImportArgs (w, (XtPointer) w, w, (XtPointer) w, None,
		wc->gadget_class.syn_resources,
		wc->gadget_class.num_syn_resources,
		args, *num_args);

  if (w->core.constraints != NULL)
    ImportConstraintArgs (w, w, None, args, num_args);
  _XmProcessUnlock();
}

/**********************************************************************
 *
 *  _XmGadgetImportSecondaryArgs
 * Does arg importing for sub-classes of XmGadget which have secondary
 * objects.
 *
 **********************************************************************/
void
_XmGadgetImportSecondaryArgs(Widget w,
			     ArgList args,
			     Cardinal *num_args)
{
  XmGadgetClass wc;
  XmBaseClassExt *classExtPtr;
  XmExtClassRec *secondaryObjClass;

  _XmProcessLock();
  wc = (XmGadgetClass) w->core.widget_class;
  classExtPtr = _XmGetBaseClassExtPtr(wc, XmQmotif);
  secondaryObjClass = (XmExtClassRec *)
			((*classExtPtr)->secondaryObjectClass);
  /* Secondary object args */
  if ((secondaryObjClass != NULL) &&
      (secondaryObjClass->ext_class.num_syn_resources != 0))
    ImportArgs (w, NULL, w, NULL, None,
		secondaryObjClass->ext_class.syn_resources,
		secondaryObjClass->ext_class.num_syn_resources,
		args, *num_args);
  _XmProcessUnlock();
}

/**********************************************************************
 *
 *  _XmManagerImportArgs
 * Does arg importing for sub-classes of XmManager.
 *
 **********************************************************************/

void
_XmManagerImportArgs(Widget w,
		     ArgList args,
		     Cardinal *num_args )
{
  XmManagerWidgetClass wc = (XmManagerWidgetClass) w->core.widget_class;

  _XmProcessLock();
  wc = (XmManagerWidgetClass) w->core.widget_class;
  if (wc->manager_class.num_syn_resources != 0)
    ImportArgs (w, (XtPointer) w, w, (XtPointer) w, None,
		wc->manager_class.syn_resources,
		wc->manager_class.num_syn_resources,
		args, *num_args);

  if (w->core.constraints != NULL)
    ImportConstraintArgs (w, w, None, args, num_args);
  _XmProcessUnlock();
}
