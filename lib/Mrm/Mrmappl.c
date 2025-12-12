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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$XConsortium: Mrmappl.c /main/17 1996/11/13 13:59:58 drk $"
#endif
#endif

/*
 *++
 *  FACILITY:
 *
 *      UIL Resource Manager (URM):
 *
 *  ABSTRACT:
 *
 *	These are the top-level routines in URM normally accessible to
 *	and used by an application at runtime to access URM facilities.
 *
 *--
 */


/*
 *
 *  INCLUDE FILES
 *
 */

#include <Mrm/MrmAppl.h>
#include <Mrm/Mrm.h>
#include "MrmMsgI.h"


/*
 *
 *  TABLE OF CONTENTS
 *
 *	MrmOpenHierarchy		Open a hierarchy
 *
 *	MrmOpenHierarchyPerDisplay     	Open a hierarchy taking display arg
 *
 *	MrmOpenHierarchyFromBuffer	Open a hierarchy from a memory buffer
 *
 *	MrmCloseHierarchy		Close an open hierarchy
 *
 *	MrmRegisterClass		Register a widget class
 *
 *	MrmFetchInterfaceModule		Fetch widgets in an interface module
 *
 *	MrmFetchWidget			Fetch a widget
 *
 *	MrmFetchWidgetOverride		Fetch a widget, overriding name, args
 *
 *	MrmFetchSetValues		Do SetValues from UID literals
 *
 */



/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	This routine allocates a hierarchy descriptor, and opens
 *	all the IDB files in the hierarchy. It initializes the
 *	optimized search lists in the hierarchy from the open files.
 *	All files are closed if there are any errors.
 *
 *  FORMAL PARAMETERS:
 *
 *	num_files		The number of files in the name list
 *	name_list		A list of the file names
 *	os_ext_list		A list of system-dependent ancillary
 *				structures corresponding to the files.
 *				This parameter may be NULL.
 *	hierarchy_id_return	To return the hierarchy id
 *
 *  IMPLICIT INPUTS:
 *
 *  IMPLICIT OUTPUTS:
 *
 *  FUNCTION VALUE:
 *
 *	MrmSUCCESS	operation succeeded
 *	MrmFAILURE	operation failed, no further reason
 *	Others		see UrmIdbOpenFileRead
 *
 *  SIDE EFFECTS:
 *
 *--
 */

Cardinal
MrmOpenHierarchy (
		  MrmCount		num_files,
		  String		*name_list,
		  MrmOsOpenParamPtr	*os_ext_list,
		  MrmHierarchy		*hierarchy_id_return)
{

  /*
   *  Local variables
   */
  Cardinal result;

  _MrmProcessLock();
  result = Urm__OpenHierarchy
    (num_files, name_list, os_ext_list, hierarchy_id_return, FALSE, NULL);
  _MrmProcessUnlock();

  return result;
}


/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	This routine allocates a hierarchy descriptor, and opens
 *	all the IDB files in the hierarchy. It initializes the
 *	optimized search lists in the hierarchy from the open files.
 *	All files are closed if there are any errors.
 *
 *  FORMAL PARAMETERS:
 *
 *	display			The Display to be passed to XtResolvePathname
 *	num_files		The number of files in the name list
 *	name_list		A list of the file names
 *	os_ext_list		A list of system-dependent ancillary
 *				structures corresponding to the files.
 *				This parameter may be NULL.
 *	hierarchy_id_return	To return the hierarchy id
 *
 *  IMPLICIT INPUTS:
 *
 *  IMPLICIT OUTPUTS:
 *
 *  FUNCTION VALUE:
 *
 *	MrmSUCCESS	operation succeeded
 *	MrmFAILURE	operation failed, no further reason
 *	Others		see UrmIdbOpenFileRead
 *
 *  SIDE EFFECTS:
 *
 *--
 */

Cardinal
MrmOpenHierarchyPerDisplay (Display		*display,
			    MrmCount		num_files,
			    String		*name_list,
			    MrmOsOpenParamPtr	*os_ext_list,
			    MrmHierarchy	*hierarchy_id_return)
{

  /*
   *  Local variables
   */
  MrmOsOpenParam	os_data;
  MrmOsOpenParamPtr	new_os_ext_list = &os_data;
  Cardinal		result;

  _MrmProcessLock();

  if (os_ext_list == NULL)
    os_ext_list = (MrmOsOpenParamPtr *)&new_os_ext_list;

  (*os_ext_list)->display = display;

  result = Urm__OpenHierarchy(num_files, name_list, os_ext_list,
			      hierarchy_id_return, FALSE, NULL);
  _MrmProcessUnlock();
  return result;
}



/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	MrmOpenHierarcyFromBuffer opens a buffer containing the
 *	memory image of a UID file.
 *
 *  FORMAL PARAMETERS:
 *
 *	hierarchy_id	ID of an open URM database hierarchy
 *
 *  IMPLICIT INPUTS:
 *
 *  IMPLICIT OUTPUTS:
 *
 *  FUNCTION VALUE:
 *

 *	MrmSUCCESS	operation succeeded
 *	MrmFAILURE	operation failed, no further reason
 *
 *  SIDE EFFECTS:
 *
 *--
 */

Cardinal
MrmOpenHierarchyFromBuffer (unsigned char	*uid_buffer,
			    MrmHierarchy	*hierarchy_id_return)
{

  /*
   *  Local variables
   */
  Cardinal		result;

  _MrmProcessLock();
  result = Urm__OpenHierarchy((MrmCount) 1, NULL, NULL,
			      hierarchy_id_return, TRUE, uid_buffer);
  _MrmProcessUnlock();
  return result;
}

/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	MrmCloseHierarchy closes a URM search hierarchy.
 *
 *  FORMAL PARAMETERS:
 *
 *	hierarchy_id	ID of an open URM database hierarchy
 *
 *  IMPLICIT INPUTS:
 *
 *  IMPLICIT OUTPUTS:
 *
 *  FUNCTION VALUE:
 *

 *	MrmSUCCESS	operation succeeded
 *	MrmFAILURE	operation failed, no further reason
 *
 *  SIDE EFFECTS:
 *
 *--
 */

Cardinal
MrmCloseHierarchy (MrmHierarchy                hierarchy_id)
{

  /*
   *  Local variables
   */
  Cardinal		result;

  _MrmProcessLock();
  result = Urm__CloseHierarchy (hierarchy_id);
  _MrmProcessUnlock();
  return result;
}



/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	This routine registers a vector of names and associated values
 *	for access in URM. The values may be callback routines, pointers
 *	to user-defined data, or any other values. The information provided
 *	is used exactly as registered callback information is used.
 *
 *	The names in the list are case-sensitive, as usual. The list may
 *	either ordered or unordered; this routine will detect lexicographic
 *	ordering if it exists, and exploit it.
 *
 *	For details on callbacks in URM, consult XmRegisterMRMCallbacks.
 *
 *  FORMAL PARAMETERS:
 *
 *	reglist		A list of name/value pairs for the names to
 *			be registered. Each name is a case-sensitive
 *			nul-terminated ASCII string. Each value is
 *			a 32-bit quantity, interpreted as a procedure
 *			address if the name is a callback routine, and
 *			uninterpreted otherwise.
 *	num_reg		The number of entries in reglist.
 *
 *  IMPLICIT INPUTS:
 *
 *  IMPLICIT OUTPUTS:
 *
 *  FUNCTION VALUE:
 *
 *  SIDE EFFECTS:
 *
 *--
 */

Cardinal
MrmRegisterNames (MrmRegisterArglist		reglist,
		  MrmCount			num_reg
		  )
{

  /*
   *  Local variables
   */
  Cardinal		result;		/* function result */
  String		*names;		/* vector of names */
  XtPointer		*values;	/* vector of values */
  int			ndx;		/* loop index */


  _MrmProcessLock();
  /*
   * Construct RegisterNames vectors, and call the WCI routine
   */
  names = (String *) XtMalloc (num_reg*sizeof(String));
  values = (XtPointer *) XtMalloc (num_reg*sizeof(XtPointer));
  for ( ndx=0 ; ndx<num_reg ; ndx++ )
    {
      names[ndx] = reglist[ndx].name;
      values[ndx] = reglist[ndx].value;
    }

  result = Urm__WCI_RegisterNames (names, values, num_reg);
  XtFree ((char*)names);
  XtFree ((char*)values);
  _MrmProcessUnlock();
  return result;

}



/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	This routine registers a vector of names and associated values for
 *	access in URM within a specific hierarchy. It is similar to
 *	MrmRegisterNames, except that the names have scope only over the
 *	hierarchy rather than global scope. For information on the names
 *	and values, see MrmRegister Names.
 *
 *  FORMAL PARAMETERS:
 *
 *	hierarchy_id	An open hierarchy descriptor.
 *	reglist		A list of name/value pairs for the names to
 *			be registered. Each name is a case-sensitive
 *			nul-terminated ASCII string. Each value is
 *			a 32-bit quantity, interpreted as a procedure
 *			address if the name is a callback routine, and
 *			uninterpreted otherwise.
 *	num_reg		The number of entries in reglist.
 *
 *  IMPLICIT INPUTS:
 *
 *  IMPLICIT OUTPUTS:
 *
 *  FUNCTION VALUE:
 *
 *  SIDE EFFECTS:
 *
 *--
 */

Cardinal
MrmRegisterNamesInHierarchy (MrmHierarchy		hierarchy_id,
			     MrmRegisterArglist		reglist,
			     MrmCount			num_reg
			     )
{

  /*
   *  Local variables
   */
  Cardinal		result;		/* function result */
  String		*names;		/* vector of names */
  XtPointer		*values;	/* vector of values */
  int			ndx;		/* loop index */

  _MrmProcessLock();
  /*
   * Construct RegisterNames vectors, and call the hierarchy routine
   */
  names = (String *) XtMalloc (num_reg*sizeof(String));
  values = (XtPointer *) XtMalloc (num_reg*sizeof(XtPointer));
  for ( ndx=0 ; ndx<num_reg ; ndx++ )
    {
      names[ndx] = reglist[ndx].name;
      values[ndx] = reglist[ndx].value;
    }

  result = Urm__RegisterNamesInHierarchy
    (hierarchy_id, names, values, num_reg);
  XtFree ((char*)names);
  XtFree ((char*)values);
  _MrmProcessUnlock();
  return result;

}



/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	MrmFetchInterfaceModule fetches all the widgets defined in some
 *	interface module in the URM database hierarchy. Typically, each
 *	application has one or more modules which define its interface;
 *	each must be fetched in order to initialize all the widgets the
 *	application requires. Applications are not constrained to have all
 *	their widgets defined in a single module.
 *
 *	If the module defines a main window widget, MrmFetchInterfaceModule
 *	returns its id. If no main window widget is contained in the module,
 *	NULL is returned. No widgets are realized. The ids of widgets other
 *	than the main window may be obtained using creation callbacks.
 *
 *  FORMAL PARAMETERS:
 *
 *	hierarchy_id	Hierarchy containing interface definition
 *	module_name	Name of interface module defining top level of
 *			interface; by convention, this is usually the generic
 *			name of the application
 *	parent		The parent widget for the topmost widgets being
 *			fetched from the module. Usually the top-level
 *			widget.
 *	w_return	To return the widget id of the main window widget
 *			for the application
 *
 *  IMPLICIT INPUTS:
 *
 *  IMPLICIT OUTPUTS:
 *
 *  FUNCTION VALUE:
 *
 *	MrmSUCCESS	operation succeeded
 *	MrmNOT_FOUND	interface module or topmost widget not found
 *	MrmFAILURE	Couldn't complete initialization
 *
 *  SIDE EFFECTS:
 *
 *--
 */
Cardinal
MrmFetchInterfaceModule (MrmHierarchy		hierarchy_id,
			 char			*module_name,
			 Widget			parent,
			 Widget			*w_return) /* unused */

{

  /*
   *  Local variables
   */
  Cardinal		result;		/* function results */
  URMResourceContextPtr	mod_context;	/* context containing module */
  RGMModuleDescPtr	modptr;		/* Interface module in context */
  int			ndx;		/* loop index */
  Widget		cur_w;		/* current widget id */
  MrmType		class;		/* current widget class */
  IDBFile		hfile_id;	/* file where module was found */
  _MrmWidgetToAppContext(parent);

  _MrmAppLock(app);
  _MrmProcessLock();
  /*
   * Validate the hierachy, then attempt to fetch the module.
   */
  if ( hierarchy_id == NULL )
    {
      result = Urm__UT_Error ("MrmFetchInterfaceModule", _MrmMMsg_0023,
			      NULL, NULL, MrmBAD_HIERARCHY);
      _MrmProcessUnlock();
      _MrmAppUnlock(app);
      return result;
    }

  if ( ! MrmHierarchyValid(hierarchy_id) )
    {
      result = Urm__UT_Error ("MrmFetchInterfaceModule", _MrmMMsg_0024,
			      NULL, NULL, MrmBAD_HIERARCHY);
      _MrmProcessUnlock();
      _MrmAppUnlock(app);
      return result;
    }

  result = UrmGetResourceContext (NULL, NULL, 0, &mod_context);
  if ( result != MrmSUCCESS )
    {
      _MrmProcessUnlock();
      _MrmAppUnlock(app);
      return result;
    }

  result = UrmIFMHGetModule (hierarchy_id, module_name, mod_context, &hfile_id);
  if ( result != MrmSUCCESS )
    {
      UrmFreeResourceContext (mod_context);
      _MrmProcessUnlock();
      _MrmAppUnlock(app);
      return result;
    }

  /*
   * We have the module. Loop through all the widgets it defines, and fetch
   * each one.
   */
  modptr = (RGMModuleDescPtr) UrmRCBuffer (mod_context);
  if ( ! UrmInterfaceModuleValid(modptr) )
    {
      UrmFreeResourceContext (mod_context);
      result = Urm__UT_Error ("MrmFetchInterfaceModule", _MrmMMsg_0025,
			      NULL, mod_context, MrmBAD_IF_MODULE);
      _MrmProcessUnlock();
      _MrmAppUnlock(app);
      return result;
    }

  for ( ndx=0 ; ndx<modptr->count ; ndx++ )
    {
      result = MrmFetchWidget (hierarchy_id, modptr->topmost[ndx].index,
			       parent, &cur_w, &class);
      if ( result != MrmSUCCESS )
        {
	  UrmFreeResourceContext (mod_context);
	  _MrmProcessUnlock();
	  _MrmAppUnlock(app);
	  return result;
        }
    }

  /*
   * successfully fetched all widgets
   */
  UrmFreeResourceContext (mod_context);
  _MrmProcessUnlock();
  _MrmAppUnlock(app);
  return MrmSUCCESS;

}



/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	MrmFetchWidget fetchs any indexed application widget. As usual in fetch
 *	operations, the fetched widget's subtree is also fetched. There are
 *	no constraints on this widget except that it must not also appear
 *	as the child of some widget within its own subtree, i.e. there must
 *	be no cycles in the subtree graph! MrmFetchWidget does not do a
 *	XtManageChild for the newly created widget.
 *
 *	The semantics of the URM database require that any widget which is to
 *	be fetched with MrmFetchWidget meet the following requirements:
 *
 *		o Not be referenced as the child of any widget in the database
 *
 *		o Be indexed
 *
 *	MrmFetchWidget replaces XmFetchTopmost, and is used to fetch
 *	topmost widgets where MrmFetchInterfaceModule is not used. A topmost
 *	widget is either the main window or any indexed widget whose parent is
 *	the top-level widget. MrmFetchWidget may be called at any time to fetch
 *	a widget which was not fetched at application startup. MrmFetchWidget
 *	determines if a widget has already been fetched by checking *w_return
 *	for a NULL value. Non-NULL values signify that the widget already
 *	has been fetched, and MrmFetchWidget no-ops. (If the toolkit ever
 *	supplies a validation routine for widgets, this will be used in
 *	place of a non-NULL check). Thus MrmFetchWidget may be used to
 *	defer fetching popup widgets until they are first referenced
 *	(presumably in a callback), and then fetching them once.
 *
 *	MrmFetchWidget may also be used to make multiple instances of a
 *	widget (and its subtree). In this case, the UID definition functions
 *	as a skeleton; there are no constraints on how many times a widget
 *	definition may be fetched. The only requirement is the *w_return be
 *	NULL on each call. This may be used to make multiple copies of
 *	a widget in e.g. a dialog box or menu (to construct a uniform form).
 *
 *	The index which identifies the widget must be known to the application
 *	via previous agreement.
 *	MrmFetchWidget will successfully fetch topmost widgets as long as the
 *	parent parameter is correct (the top-level widget), and this
 *	replaces XmFetchTopmost (which vanishes).
 *
 *  FORMAL PARAMETERS:
 *
 *	hierarchy_id	Hierarchy containing interface definition
 *	index		The index of the widget to fetch.
 *	parent		ID of the parent widget
 *	w_return	To return the widget id of the created widget.
 *			*w_return must be NULL or MrmFetchWidget no-ops.
 *	class_return	To return the code identifying the widget class.
 *			This is principally used to distinguish main window
 *			and other toolkit widgets. It will be one of the
 *			URMwc... codes defined in MRM.h. The code
 *			for a main window is URMwcMainWindow.
 *
 *  IMPLICIT INPUTS:
 *
 *  IMPLICIT OUTPUTS:
 *
 *  FUNCTION VALUE:
 *
 *	MrmSUCCESS	operation succeeded
 *	MrmNOT_FOUND	widget not found in database
 *	MrmFAILURE	operation failed, no further reason
 *
 *  SIDE EFFECTS:
 *
 *--
 */

Cardinal
MrmFetchWidget (MrmHierarchy                hierarchy_id,
		String                      index,
		Widget                      parent,
		Widget                      *w_return,
		MrmType                     *class_return)
{

  return MrmFetchWidgetOverride (hierarchy_id, index, parent,
				 NULL, NULL, 0, w_return, class_return);

}



/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	This procedure is the extended version of MrmFetchWidget. It is
 *	identical to MrmFetchWidget in all respsects, except that it allows
 *	the caller to override the widget's name and any number of the
 *	arguments which would otherwise receive from the UID database or
 *	one of the defaulting mechanisms (i.e. the override is not limited
 *	to those arguments in the UID file).
 *
 *	The override parameters apply only to the widget fetched and returned
 *	by this procedure; its children (subtree) do not receive any override
 *	parameters.
 *
 *  FORMAL PARAMETERS:
 *
 *	hierarchy_id	Hierarchy containing interface definition
 *	index		The index of the widget to fetch.
 *	parent		ID of the parent widget
 *	ov_name		Name to override widget name (NULL for no override)
 *	ov_args		Override arglist, exactly as would be given to
 *			XtCreateWidget (conversion complete, etc). NULL
 *			for no override.
 *	ov_num_args	# args in ov_args; 0 for no override
 *	w_return	To return the widget id of the created widget.
 *			*w_return must be NULL or MrmFetchWidget no-ops.
 *	class_return	To return the code identifying the widget class.
 *			This is principally used to distinguish main window
 *			and other toolkit widgets. It will be one of the
 *			URMwc... codes defined in MRM.h. The code
 *			for a main window is URMwcMainWindow.
 *
 *  IMPLICIT INPUTS:
 *
 *  IMPLICIT OUTPUTS:
 *
 *  FUNCTION VALUE:
 *
 *	MrmSUCCESS	operation succeeded
 *	MrmNOT_FOUND	widget not found in database
 *	MrmFAILURE	operation failed, no further reason
 *
 *  SIDE EFFECTS:
 *
 *--
 */

Cardinal
MrmFetchWidgetOverride (MrmHierarchy		hierarchy_id,
			String			index,
			Widget			parent,
			String			ov_name,
			ArgList			ov_args,
			Cardinal		ov_num_args,
			Widget			*w_return,
			MrmType			*class_return)
{

  /*
   *  Local variables
   */
  Cardinal		result;		/* function results */
  URMResourceContextPtr	w_context;	/* context containing widget */
  RGMWidgetRecordPtr	widgetrec;	/* widget record in context */
  IDBFile		hfile_id;	/* file in widget was found */
  URMResourceContextPtr	wref_ctx;	/* for widget references */
  URMSetValuesDescPtr	svlist = NULL;	/* list of SetValues descriptors */
  int			ndx ;		/* loop index */
  char			errmsg[300];
  _MrmWidgetToAppContext(parent);

  _MrmAppLock(app);
  _MrmProcessLock();

  /*
   * Validate the hierachy, then attempt to fetch the widget
   */
  if ( hierarchy_id == NULL )
    {
      result = Urm__UT_Error ("MrmFetchWidgetOverride", _MrmMMsg_0023,
			      NULL, NULL, MrmBAD_HIERARCHY);
      _MrmProcessUnlock();
      _MrmAppUnlock(app);
      return result;
    }

  if ( ! MrmHierarchyValid(hierarchy_id) )
    {
      result = Urm__UT_Error ("MrmFetchWidgetOverride", _MrmMMsg_0024,
			      NULL, NULL, MrmBAD_HIERARCHY);
      _MrmProcessUnlock();
      _MrmAppUnlock(app);
      return result;
    }

  result = UrmGetResourceContext (NULL, NULL, 300, &w_context);
  if ( result != MrmSUCCESS )
    {
      _MrmProcessUnlock();
      _MrmAppUnlock(app);
      return result;
    }

  result = UrmHGetWidget (hierarchy_id, index, w_context, &hfile_id);
  if ( result != MrmSUCCESS )
    {
      UrmFreeResourceContext (w_context);
      _MrmProcessUnlock();
      _MrmAppUnlock(app);
      return result;
    }

  /*
   * Validate the widget record, then set the class return. Then instantiate the
   * widget and its subtree.
   */
  widgetrec = (RGMWidgetRecordPtr) UrmRCBuffer (w_context);
  if ( ! UrmWRValid(widgetrec) )
    {
      UrmFreeResourceContext (w_context);
      result = Urm__UT_Error ("MrmFetchWidgetOverride", _MrmMMsg_0026,
			      NULL, w_context, MrmFAILURE);
      _MrmProcessUnlock();
      _MrmAppUnlock(app);
      return result;
    }
  /*
   * The following test breaks compatibility with the CDE1.0
   * version of Mrm and cannot be done for CDEnext.
   *
   *   if (widgetrec->access == URMaPrivate)
   *     {
   *       UrmFreeResourceContext (w_context);
   *       sprintf(errmsg, _MrmMMsg_0119, index);
   *       result = Urm__UT_Error("MrmFetchWidgetOverride", errmsg,
   * 			     NULL, w_context, MrmFAILURE);
   *       _MrmProcessUnlock();
   *       _MrmAppUnlock(app);
   *       return result;
   *     }
   */

  *class_return = widgetrec->type;

  Urm__CW_InitWRef (&wref_ctx);
  result = UrmCreateWidgetTree (w_context, parent, hierarchy_id, hfile_id,
				ov_name, ov_args, ov_num_args,
				URMrIndex, index, 0L, MrmManageDefault,
				(URMPointerListPtr *)&svlist, wref_ctx,
				w_return);
  UrmFreeResourceContext (w_context);
  if ( result != MrmSUCCESS )
    {
      _MrmProcessUnlock();
      _MrmAppUnlock(app);
      return result;
    }

  /*
   * Free up resources
   */
  if ( svlist != NULL )
    {
      for ( ndx=0 ; ndx<UrmPlistNum((URMPointerListPtr)svlist) ; ndx++ )
        Urm__CW_FreeSetValuesDesc
	  ((URMSetValuesDescPtr)UrmPlistPtrN((URMPointerListPtr)svlist,ndx));
      UrmPlistFree ((URMPointerListPtr)svlist);
    }
  UrmFreeResourceContext (wref_ctx);

  /*
   * successfully initialized
   */
  _MrmProcessUnlock();
  _MrmAppUnlock(app);
  return MrmSUCCESS;

}

/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	This routine does an XtSetValues on a widget, evaluating the values
 *	as public literal resource references resolvable from a URM
 *	hierarchy. Each literal is fetched from the hierarchy, and
 *	its value is fixed up and converted as required. This value is
 *	then placed in the arglist, and used as the actual value for
 *	an XtSetValues call. This routine allows a widget to be modified
 *	after creation using UID file values exactly as is done for creation
 *	values in MrmFetchWidget.
 *
 *	As in FetchWidget, each argument whose value can be evaluated from
 *	the UID hierarchy is set in the widget. Values which are not found
 *	or for which conversion errors occur are not modified.
 *
 *	Each entry in the arglist identifies an argument to be modified in
 *	the widget. The .name part identifies the tag, as usual (XmN...).
 *	the .value part must be a String whose values is the index of the
 *	literal. Thus
 *		args[n].name = "label"		(XmNlabel)
 *		args[n].value = "OK_button_label"
 *	would modify the label resource of the widget to have the value
 *	of the literal accessed by index 'OK_button_label' in the
 *	hierarchy.
 *
 *  FORMAL PARAMETERS:
 *
 *	hierarchy_id	URM hierarchy to be searched for literal definitions
 *	w		the widget to be modified
 *	args		An arglist specifying the widget arguments to be
 *			modified. The .name part of each argument must be
 *			the usual XmN... string identifying the argument
 *			(argument tag). The .value part must be a String
 *			giveing the index of the literal. All literals must
 *			be public literals accessed by index.
 *	num_args	the number of entries in args.
 *
 *  IMPLICIT INPUTS:
 *
 *  IMPLICIT OUTPUTS:
 *
 *  FUNCTION VALUE:
 *
 *  SIDE EFFECTS:
 *
 *--
 */

Cardinal
MrmFetchSetValues (MrmHierarchy                hierarchy_id,
		   Widget                      w,
		   ArgList                     args,
		   Cardinal                    num_args)
{

  /*
   *  Local variables
   */
  Cardinal		result;
  _MrmWidgetToAppContext(w);

  _MrmAppLock(app);
  _MrmProcessLock();

  /*
   * Validate the hierachy, then attempt to modify the widget
   */
  if ( hierarchy_id == NULL )
    {
      result = Urm__UT_Error ("MrmFetchSetValues", _MrmMMsg_0023,
			      NULL, NULL, MrmBAD_HIERARCHY);
      _MrmProcessUnlock();
      _MrmAppUnlock(app);
      return result;
    }

  if ( ! MrmHierarchyValid(hierarchy_id) )
    {
      result = Urm__UT_Error ("MrmFetchSetValues", _MrmMMsg_0024,
			      NULL, NULL, MrmBAD_HIERARCHY);
      _MrmProcessUnlock();
      _MrmAppUnlock(app);
      return result;
    }

  result = UrmFetchSetValues (hierarchy_id, w, args, num_args);
  _MrmProcessUnlock();
  _MrmAppUnlock(app);
  return result;
}
