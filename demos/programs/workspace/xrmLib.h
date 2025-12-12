/* $XConsortium: xrmLib.h /main/5 1995/07/15 20:46:59 drk $ */
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

#ifndef xrmLib_h
#define xrmLib_h

/*	Function Name: InitializeXrm
 *
 *      Description: Open the Xrm database 
 *   
 *      Arguments: file_name - name of xrm database file
 * 
 *
 */

extern Boolean InitializeXrm(
char *
);

/*	Function Name: GetWorkspaceResources
 *
 *	Description: Gets the workspace resources for a window
 *     
 *      Arguments: widget -    any widget
 *                 specifier - A list of quarks that specifies the
 *                             window (SM_CLIENT_ID, WM_ROLE).  
 *                 room_list - returns a quark list of room names that
 *                             the window is in.
 *                 all_workspaces - returns allWorkspaces resource
 *                 linked     - returns linked resource
 *                            
 */
 

extern Boolean GetWorkspaceResources(
				     Widget w,
				     XrmQuarkList specifier,
				     XrmQuarkList *room_qlist,
				     Boolean *all_workspaces,
				     Boolean *linked
);





extern Boolean GetSpaceListResources(
Widget w,
char ***,
char ***,
char ***,
char**,
Boolean*
);


/*	Function Name: GetWindowConfigurationEntry 
 *
 *	Description: Gets the configuration information for a specified
 *                   window.
 *
 *	Arguments: specifier -       A list of quarks that specifies the
 *                                   window (SM_CLIENT_ID, WM_ROLE).
 *                 attribute_list -  A quark list of attribute names
 *                                   to search for in database.
 *                 room_name -       Room name that the window is in.
 *                                   if it is linked then the linkedRoom
 *                                   quark should be passed.
 *                 attributes_values - Returns array of values
 *                                   found in the database.  The array
 *                                   length is equal to attribute_list length.
 *                                   (If no value is found for an attribute then
 *                                   a value with 0 length and NULL addr is passed)
 *                                  
 *	Returns:   True if any resources found, else False.
 *
 * 
 */



extern Boolean GetWindowConfigurationEntry(
XrmQuarkList specifier,
XrmQuarkList attribute_list,
XrmQuark room_name,
XrmValue **attribute_values
);


/*	Function Name: GetWindowStackedEntry 
 *
 *	Description: Gets the stacked resource for a specified
 *                   window.
 *
 *	Arguments: specifier -       A list of quarks that specifies the
 *                                   window (SM_CLIENT_ID, WM_ROLE).
 *                 room_name -       Room name that the window is in.
 *                                   if it is linked then the linkedRoom
 *                                   quark should be passed.
 *                 attributes_value - Returns a stacked value
 *                                  
 *	Returns:   True if resources found, else False.
 *
 * 
 */

extern Boolean GetWindowStackedEntry(
XrmQuarkList specifier,
XrmQuark room_name,
XrmValue *attribute_value
);

/*
 *	Function Name: GetAllWindowConfigurationEntry 
 *
 *	Description: Gets the configuration information for a specified
 *                   window.
 *
 *	Arguments: specifier -       A list of quarks that specifies the
 *                                   window (SM_CLIENT_ID, WM_ROLE).
 *                 attribute_list -  A quark list of attribute names
 *                                   to search for in database.
 *                 room_list -       list of room names that the window is in.
 *                                   if it is linked then the linkedRoom
 *                                   quark should be passed.
 *                 attributes_values - Returns double array of values
 *                                   found in the database.  The array
 *                                   width is equal to the number of rooms. The array
 *                                   length is equal to attribute_list length.
 *                                   (If no value is found for an attribute then
 *                                   a value with 0 length and NULL addr is passed)
 *                                  
 *	Returns:   True if any resources found, else False.
 *
 *
 *
 */

extern Boolean GetAllWindowConfigurationEntry(
XrmQuarkList specifier,
XrmQuarkList attribute_list,
XrmQuarkList room_list,
XrmValue ***attribute_values
);


/* 
 *
 *	Function Name: SaveWorkspaceResources
 *
 *	Description: Save the workspaces resources in the xrm database
 *                   window.
 *
 *	Arguments: widget - any widget
 *                 specifier - A list of quarks that specifies the
 *                             window (SM_CLIENT_ID, WM_ROLE).
 *                 room_nameq - Room name quark to be added to database.
 *                 all_workspaces - allWorkspaces resource
 *                 linked     - linked resource
 *
 */

extern Boolean SaveWorkspaceResources(
				      Widget,
				      XrmQuarkList,
				      XrmQuark,
				      XrmQuark,
				      Boolean,
				      Boolean
);



extern Boolean SaveSpaceListResources(
char *,
char *,
char *,
char *
);


/*
 *
 *	Function Name: SaveWindowConfiguration
 *
 *	Description: Save the window configuration in the xrm database
 *
 *	Arguments: specifier - A list of quarks that specifies the
 *                             window (SM_CLIENT_ID, WM_ROLE).
 *                 attribute_qlist - A quark list of attribute names to
 *                              be saved in database.
 *                 room_nameq - Room name quark that the configuration is in.
 *                              if it is linked then the linkedRoom
 *                              quark should be passed.
 *                 attr_values - list of attribute values (matches attrib_qlist)
 *
 */

extern Boolean SaveWindowConfiguration(
				       XrmQuarkList,
				       XrmQuarkList,
				       XrmQuark,
				       XrmValue *
);

/*
 *
 *	Function Name: PurgeWindowConfiguration
 *
 *	Description: Remove the window configuration from the xrm database
 *
 *	Arguments: specifier - A list of quarks that specifies the
 *                             window (SM_CLIENT_ID, WM_ROLE).
 *                 attribute_qlist - A quark list of attribute names to
 *                              be removed in database.
 *                 room_nameq - the room that the configuration
 *
 */

extern Boolean
PurgeWindowConfiguration(
Widget w,
XrmQuarkList specifier_list,
XrmQuarkList attribute_qlist,
XrmQuark room_nameq
);




/*
 *
 *	Function Name: PurgeAllWindowConfiguration
 *
 *	Description: Remove the window configuration from the xrm database
 *
 *	Arguments: specifier - A list of quarks that specifies the
 *                             window (SM_CLIENT_ID, WM_ROLE).
 *                 attribute_qlist - A quark list of attribute names to
 *                              be removed in database.
 *                 room_nameq - the room that the configuration
 *
 */
extern Boolean
PurgeAllWindowConfiguration(
Widget w,
XrmQuarkList specifier_list,
XrmQuarkList attribute_qlist
);




/*
 *
 *	Function Name: SaveWsmToFile
 *
 *	Description: Saves the Xrm database to a specified file
 *
 *	Arguments: file_name - file name
 *
 */
extern void
SaveWsmToFile(
char *file_name
);

#endif
