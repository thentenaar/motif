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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$XConsortium: Mrmhier.c /main/17 1996/11/13 14:01:19 drk $"
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
 *--
 */

/*
 * This file contains routines which assist in managing URM hierarchies
 */


/*
 *
 *  INCLUDE FILES
 *
 */

#include <stdio.h>
#include <Mrm/MrmAppl.h>
#include <Mrm/Mrm.h>

#include <Xm/XmosI.h>		/* for _XmOSInitPath() */
#include <Xm/XmI.h>		/* for _XmGetDefaultDisplay() */

#include "MrmMsgI.h"

#if XM_MSGCAT
#if !defined(NL_CAT_LOCALE)
#define NL_CAT_LOCALE	0
#endif
#endif


/*
 *
 *  TABLE OF CONTENTS
 *
 *	Urm__OpenHierarchy		Open hierarchy (internal version)
 *
 *	Urm__CloseHierarchy		Close hierarchy (internal version)
 *
 *	UrmHGetIndexedResource		Get resource from hierarchy
 *
 */


/*
 *
 *  DEFINE and MACRO DEFINITIONS
 *
 */

static Cardinal I18NOpenFile (Display		 *display,
			      String		 name ,
			      MrmOsOpenParamPtr	 os_ext ,
			      IDBFile		 *file_id_return );


/*
 *
 *  OWN VARIABLE DECLARATIONS
 *
 */
static char		*uidPath;
static SubstitutionRec	uidSubs[1];



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
 *	MrmSUCCESS		operation succeeded
 *	MrmFAILURE		allocation or other failure
 *	MrmDISPLAY_NOT_OPENED	Display hasn't been opened yet
 *	Others			see UrmIdbOpenFileRead
 *
 *  SIDE EFFECTS:
 *
 *--
 */

Cardinal
Urm__OpenHierarchy (MrmCount			num_files,
		    String			*name_list,
		    MrmOsOpenParamPtr		*os_ext_list,
		    MrmHierarchy		*hierarchy_id_return,
		    MrmFlag			in_memory,
		    unsigned char		*uid_buffer)
{

  /*
   *  Local variables
   */
  Cardinal		result ;	/* function result */
  MrmHierarchy		hiptr ;		/* hierarchy descriptor */
  MrmCount		list_size ;	/* # bytes for file lists */
  int			ndx ;		/* loop index */
  IDBFile		*idvec ;	/* current file id vector */
  int			file_ndx ;	/* file loop index */
  IDBFile		cur_file ;	/* current IDB file */
  URMResourceContextPtr	class_ctx;	/* for class compression table */
  URMResourceContextPtr	resource_ctx;	/* for resource compression table */
  Display		*display;	/* display for XtResolvePathNames */
  char                 	err_stg[300];

#if XM_MSGCAT
  static Boolean first = True;

  /*
   * We open the message catalog from MrmOpenHierarchy... routines.
   * We cannot open it from MrmInitialize, since this call can be done
   * before Xt has been initialized, so at that time the locale could
   * be wrong.
   */

  /* We only want to open the message catalog once. */

  if (first)
    {
      Mrm_catd = catopen("Mrm", NL_CAT_LOCALE);
      first = False;
    }
#endif

  if ( os_ext_list == NULL )
    {
      display = NULL;
    }
  else
    {
      MrmOsOpenParamPtr os_data;
      os_data = *os_ext_list;
      if (os_data->display == NULL)
	{
	  display = NULL;
	}
      else
	{
	  display = os_data->display;
	}
    }

  if (display == NULL)
    {
      display = _XmGetDefaultDisplay();
    }

  if (display == NULL)
    {
      return Urm__UT_Error ("Urm__OpenHierarchy", _MrmMMsg_0030,
			    NULL, NULL, MrmDISPLAY_NOT_OPENED);
    };

  /*
   * If the uidPath was previously set, XtFree it so we can try any
   * new paths that may have been setup.
   */
  if (uidPath != 0)
    {
      XtFree (uidPath);
      uidPath = 0;
    }

  /*
   * Allocate a hierarchy, and allocate all file lists.
   */
  hiptr = (MrmHierarchy) XtMalloc (sizeof(MrmHierarchyDesc)) ;
  if ( hiptr == NULL ) return MrmFAILURE ;

  hiptr->validation = MrmHIERARCHY_VALID;
  hiptr->num_file = 0 ;

  list_size = num_files * sizeof(IDBFile) ;
  hiptr->file_list = (IDBFile *) XtMalloc (list_size) ;
  if ( hiptr->file_list == NULL ) return MrmFAILURE ;

  for ( ndx=URMgMin ; ndx<=URMgMax ; ndx++ )
    {
      hiptr->grp_num[ndx] = 0 ;
      idvec = (IDBFile *) XtMalloc (list_size) ;
      if ( idvec == NULL ) return MrmFAILURE ;
      hiptr->grp_ids[ndx] = idvec ;
    }

  hiptr->name_registry = NULL;

  /*
   * Now open each file. Any failure causes an error return, with any
   * open files closed and the descriptor and lists freed.
   */
  for ( file_ndx=0 ; file_ndx<num_files ; file_ndx++ )
    {
      if ( in_memory == TRUE )
	{
	  result = UrmIdbOpenBuffer(uid_buffer, &cur_file) ;
	  switch ( result )
	    {
	    case MrmSUCCESS:
	      break;
	    case MrmNOT_VALID:
	      strcpy(err_stg, _MrmMMsg_0113);
	      break;
	    default:
	      strcpy(err_stg, _MrmMMsg_0114);
	      break;
	    }
	}
      else if ( os_ext_list == NULL )
	result = I18NOpenFile (display, name_list[file_ndx], NULL, &cur_file) ;
      else
        result = I18NOpenFile (display, name_list[file_ndx],
			       os_ext_list[file_ndx], &cur_file) ;
      if ( result != MrmSUCCESS )
        {
	  XtFree (uidPath);
	  uidPath = 0;
	  Urm__CloseHierarchy (hiptr) ;
	  return result;
        }

      hiptr->file_list[hiptr->num_file] = cur_file ;
      hiptr->num_file++ ;
      for ( ndx=URMgMin ; ndx<=URMgMax ; ndx++ )
        if ( cur_file->group_counts[ndx] > 0 )
	  {
            idvec = hiptr->grp_ids[ndx] ;
            idvec[hiptr->grp_num[ndx]] = cur_file ;
            hiptr->grp_num[ndx]++ ;
	  }

      /*
       * Attempt to read in compression tables for this UID file.
       * Retain and fixup the tables if they are found.
       */
      cur_file->class_ctable = NULL;
      cur_file->resource_ctable = NULL;
      result = UrmGetResourceContext(NULL, NULL, 0, &class_ctx);
      if ( result != MrmSUCCESS ) return result;
      result = UrmGetResourceContext(NULL, NULL, 0, &resource_ctx);
      if ( result != MrmSUCCESS ) return result;
      result = UrmGetIndexedLiteral (cur_file, UilMrmClassTableIndex,
				     class_ctx);
      if ( result != MrmSUCCESS ) continue;
      result = UrmGetIndexedLiteral (cur_file, UilMrmResourceTableIndex,
				     resource_ctx);
      if ( result != MrmSUCCESS ) continue;

      /*
       * Retain the buffers from the contexts, but free the contexts
       * themselves. Fixup the tables.
       */
      cur_file->class_ctable =  (UidCompressionTablePtr)
	UrmRCBuffer (class_ctx);
      UrmRCSetBuffer (class_ctx, NULL);
      UrmFreeResourceContext (class_ctx);
      Urm__FixupCompressionTable (cur_file->class_ctable, TRUE,
				  cur_file->byte_swapped);
      cur_file->resource_ctable = (UidCompressionTablePtr)
	UrmRCBuffer (resource_ctx);
      UrmRCSetBuffer (resource_ctx, NULL);
      UrmFreeResourceContext (resource_ctx);
      Urm__FixupCompressionTable (cur_file->resource_ctable, FALSE,
				  cur_file->byte_swapped);
    }

  /*
   * successfully opened. Free the uidPath at this point
   */
  XtFree (uidPath);
  uidPath = 0;
  *hierarchy_id_return = hiptr ;
  return MrmSUCCESS ;

}



/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	This is the internal routine which closes a URM search hierarchy
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
 *	MrmBAD_HIERARCHY	invalid URM hierarchy
 *	MrmFAILURE	operation failed, no further reason
 *
 *  SIDE EFFECTS:
 *
 *--
 */

Cardinal
Urm__CloseHierarchy (MrmHierarchy	hierarchy_id)
{

  /*
   *  Local variables
   */
  int			ndx ;		/* loop index */
  URMHashTableEntryPtr	cp, np;         /* for fixing 7303 */

  /*
   * validity check the hierarchy, then close all files, deallocate all
   * lists, and deallocate the descriptor
   */
  if ( hierarchy_id == NULL )
    return Urm__UT_Error ("Urm__CloseHierarchy", _MrmMMsg_0023,
			  NULL, NULL, MrmBAD_HIERARCHY) ;
  if ( ! MrmHierarchyValid(hierarchy_id) )
    return Urm__UT_Error ("Urm__CloseHierarchy", _MrmMMsg_0024,
			  NULL, NULL, MrmBAD_HIERARCHY) ;

  for ( ndx=0 ; ndx<hierarchy_id->num_file ; ndx++ )
    if (hierarchy_id->file_list[ndx]->in_memory == FALSE )
      UrmIdbCloseFile (hierarchy_id->file_list[ndx], FALSE) ;

  /* Begin fixing DTS 7303 */
  if(hierarchy_id->name_registry){
    for ( ndx=0 ; ndx<k_hash_table_size; ndx++ )
      if((cp = hierarchy_id->name_registry[ndx]) != NULL)
	while(cp){
	  np = cp->az_next_entry;
	  XtFree((char*)cp);
	  cp = np;
	};
    XtFree ((char*)hierarchy_id->name_registry);
  }
  /* End fixing DTS 7303 */

  XtFree ((char*)hierarchy_id->file_list);

  for ( ndx=URMgMin ; ndx<=URMgMax ; ndx++ )
    XtFree ((char*)hierarchy_id->grp_ids[ndx]) ;

  hierarchy_id->validation = 0;
  XtFree ((char*)hierarchy_id) ;
  return MrmSUCCESS ;

}



/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	UrmHGetIndexedResource attempts to retrieve a resource
 *	from an open URM hierarchy. It functions exactly like
 *	UrmIdbGetIndexedResource except that it queries each file
 *	in the hierarchy in turn. It uses the optimized search lists
 *	where possible.
 *
 *  FORMAL PARAMETERS:
 *
 *	hierarchy_id		open URM hierarchy to search
 *	index			case-sensitive index for the entry to match
 *	group_filter		if not null, entry found must match this group
 *	type_filter		if not null, entry found must match this type
 *	context_id		URM resource context to receieve data block
 *	file_id_return		to return file in which resource was found
 *
 *  IMPLICIT INPUTS:
 *
 *  IMPLICIT OUTPUTS:
 *
 *  FUNCTION VALUE:
 *
 *	MrmSUCCESS	operation succeeded
 *	MrmBAD_HIERARCHY	invalid URM hierarchy
 *	MrmNOT_FOUND	entry not found
 *	MrmWRONG_GROUP	entry didn't match group filter
 *	MrmWRONG_TYPE	entry didn't match type filter
 *	MrmFAILURE	operation failed, no further reason
 *
 *  SIDE EFFECTS:
 *
 *--
 */

Cardinal
UrmHGetIndexedResource (MrmHierarchy		hierarchy_id,
			String			index ,
			MrmGroup			group_filter,
			MrmType			type_filter,
			URMResourceContextPtr	context_id,
			IDBFile			*file_id_return)
{

  /*
   *  Local variables
   */
  Cardinal		result ;	/* function results */
  IDBFile		*file_ids ;	/* list of files to search */
  int			num_ids ;	/* number of entries in file_ids */
  int			ndx ;		/* loop index */


  /*
   * Validate hierarchy. Then loop through the files which define the resource,
   * searching for the desired resource.
   */
  if ( hierarchy_id == NULL )
    return Urm__UT_Error ("UrmHGetIndexedResource", _MrmMMsg_0023,
			  NULL, NULL, MrmBAD_HIERARCHY) ;
  if ( ! MrmHierarchyValid(hierarchy_id) )
    return Urm__UT_Error ("UrmHGetIndexedResource", _MrmMMsg_0024,
			  NULL, context_id, MrmBAD_HIERARCHY) ;

  if ( group_filter>=URMgMin && group_filter<=URMgMax)
    {
      file_ids = hierarchy_id->grp_ids[group_filter] ;
      num_ids = hierarchy_id->grp_num[group_filter] ;
    }
  else
    {
      file_ids = hierarchy_id->file_list ;
      num_ids = hierarchy_id->num_file ;
    }
  for ( ndx=0 ; ndx<num_ids ; ndx++ )
    {
      *file_id_return = file_ids[ndx] ;
      result = UrmIdbGetIndexedResource
        (*file_id_return, index, group_filter, type_filter, context_id) ;
      if ( result == MrmSUCCESS ) return result ;
    }

  /*
   * Not found
   */
  return MrmNOT_FOUND ;

}



/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	This routine inserts the names and their associated values into
 *	the hierarchy's name registration hash table (which is constructed
 *	if needed). In all respects it functions like Urm__WCI_RegisterNames.
 *
 *  FORMAL PARAMETERS:
 *
 *	hierarchy_id	open URM hierarchy in which to register names.
 *	names		A vector of case-sensitive callback routine names.
 *	values		A vector of the corresponding routine addresses
 *	num_cb		The number of entries in names and values.
 *
 *  IMPLICIT INPUTS:
 *
 *  IMPLICIT OUTPUTS:
 *
 *  FUNCTION VALUE:
 *
 *	MrmSUCCESS	operation succeeded
 *
 *  SIDE EFFECTS:
 *
 *--
 */

Cardinal
Urm__RegisterNamesInHierarchy (MrmHierarchy		hierarchy_id,
			       String			*names,
			       XtPointer			*values,
			       MrmCount			num_cb)
{

  URMHashTableEntryPtr	*name_table;
  Boolean		inited = FALSE;
  int			ndx;
  URMHashTableEntryPtr	hash_entry;
  char			*current_name;
  char			*current_value;


  /*
   * Make sure the hash table is initialized
   */
  name_table = hierarchy_id->name_registry;
  if ( name_table == NULL )
    {
      name_table = (URMHashTableEntryPtr *)
	XtMalloc(sizeof(URMHashTableEntryPtr)*k_hash_table_size);
      /* Begin fixing DTS 7303 */
      for ( ndx=0 ; ndx<k_hash_table_size; ndx++ )
	name_table[ndx] = NULL;
      /* End fixing DTS 7303 */
      hierarchy_id->name_registry = name_table;
      hash_initialize (name_table, &inited);
    }

  /*
   * Store each name-value pair in the hash table.
   */
  for (ndx = 0 ; ndx < num_cb ; ndx++)
    {
      current_name = names [ndx];
      current_value = values [ndx];
      hash_entry = (URMHashTableEntryPtr)
	hash_insert_name (name_table, current_name);
      hash_entry->az_value = current_value;
    }

  return MrmSUCCESS;

}



/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	This routine returns the value registered for a name. It first
 *	attempts to look up the name in the hierarchy's name registry.
 *	If that fails, or their is no registry, then a global lookup is
 *	attempted.
 *
 *  FORMAL PARAMETERS:
 *
 *	hierarchy_id	open URM hierarchy to search.
 *	name		case-sensitive name to be matched
 *	value_return	to return value.
 *
 *  IMPLICIT INPUTS:
 *
 *  IMPLICIT OUTPUTS:
 *
 *  FUNCTION VALUE:
 *
 *	MrmSUCCESS	operation succeeded
 *	MrmNOT_FOUND	no match found
 *
 *  SIDE EFFECTS:
 *
 *--
 */

Cardinal
Urm__LookupNameInHierarchy (MrmHierarchy	hierarchy_id,
			    String		name,
			    XtPointer		*value_return)

{

  URMHashTableEntryPtr	*name_table;
  URMHashTableEntryPtr	hash_entry;


  /*
   * Look up in hierarchy first (if there is a registry)
   */
  name_table = hierarchy_id->name_registry;
  if ( name_table != NULL )
    {
      hash_entry = (URMHashTableEntryPtr) hash_find_name (name_table, name);
      if (hash_entry != NULL)
	{
	  *value_return = hash_entry->az_value;
	  return MrmSUCCESS;
	}
    }

  /*
   * Fall back on global table
   */
  return Urm__WCI_LookupRegisteredName (name, value_return);

}



/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	This routine opens a single UID file in a platform-dependent way,
 *	performing i18n language switching in order to do so.
 *
 *	Per the latest agreement on semantics, this routine does:
 *		- first, try to open in the local directory (that is, with
 *		  no switching).
 *		- second, try language switching and open
 *
 *  FORMAL PARAMETERS:
 *
 *	name		A system-dependent string specifying the IDB file
 *			to be opened.
 *	os_ext		An operating-system specific structure which
 *			supports using specific file system features
 *	file_id_return	returns the IDB file id used in all other IDB routines
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

static Cardinal
I18NOpenFile (Display			*display,
	      String			name,
	      MrmOsOpenParamPtr		os_ext,
	      IDBFile			*file_id_return)
{

  /*
   *  Local variables
   */
  Cardinal		result;		/* function results */
  char			dummy[300];	/* file name (unused) */
  char			err_stg[300];

  /*
   * Use XtResolvePathName.  If the last 4 characters of the file name
   * are not .uid then pass in the suffix of .uid.  If a file is not
   * found with the suffix passed in then try without the suffix.
   */
  char			*resolvedname; /* current resolved name */
  Boolean		user_path ;

  uidSubs[0].substitution = name;

  if (uidPath == 0)
    {
      uidPath = _XmOSInitPath(name, "UIDPATH", &user_path);
      if (user_path) uidSubs[0].match = 'U';
      else           uidSubs[0].match = MATCH_CHAR ;
    }

  resolvedname = 0;

  /*
   * Check and see if the .uid suffix is already on the file. If not then try to
   * resolve the pathname with .uid suffix first. If that fails or the suffix is
   * already on the file then just try to resolve the pathname.
   */
  if ( strcmp (&name[strlen(name)-4],".uid") != 0 )
    resolvedname = XtResolvePathname (display,
				      "uid",
				      NULL,
				      ".uid",
				      uidPath,
				      uidSubs,
				      XtNumber(uidSubs),
				      (XtFilePredicate)NULL);

  /*
   * No .uid suffix or a failure to resolve the pathname with the .uid suffix
   */
  if (resolvedname == 0)
    resolvedname = XtResolvePathname (display,
				      "uid",
				      NULL,
				      NULL,
				      uidPath,
				      uidSubs,
				      XtNumber(uidSubs),
				      (XtFilePredicate)NULL);

  if (resolvedname == 0)
    {
      sprintf (err_stg, _MrmMMsg_0031, name) ;
      return Urm__UT_Error ("I18NOpenFile", err_stg, NULL, NULL, MrmNOT_FOUND);
    }

  result = UrmIdbOpenFileRead (resolvedname, os_ext, file_id_return, dummy) ;
  switch ( result )
    {
    case MrmSUCCESS:
      break;
    case MrmNOT_VALID:
      sprintf (err_stg, _MrmMMsg_0032, resolvedname) ;
      break;
    case MrmNOT_FOUND:
    default:
      sprintf (err_stg, _MrmMMsg_0031, resolvedname) ;
      break;
    }

  XtFree(resolvedname); /* allocated in XtResolvePathName() */

  if (result == MrmSUCCESS)
    return result;
  else
    return Urm__UT_Error ("I18NOpenFile", err_stg, NULL, NULL, result);
}

