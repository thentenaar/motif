/**
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
static char rcsid[] = "$TOG: Mrmwci.c /main/16 1999/05/19 15:25:58 mgreess $"
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
 *	This module holds all routines, structures, and structure definitions
 *	required for the FT1 widget class information package (WCI). This
 *	package provides routines which register information about widget
 *	classes, and which retrieve information about widget classes
 *	as required by URM.
 *
 *--
 */

/*
 *
 *  INCLUDE FILES
 *
 */
#include <stdio.h>
#include <string.h>
#include <X11/IntrinsicP.h>
#include <X11/CoreP.h>

#include <Mrm/MrmAppl.h>
#include <Mrm/Mrm.h>

#include "MrmMsgI.h"

/*
 *
 *  TABLE OF CONTENTS
 *
 */
static int hash_function(int l_length , char *c_value);

/*
 *
 *  GLOBAL VARIABLE DECLARATIONS
 *
 */

/*
 * The list of registered classes, so they can be searched.
 */
externaldef(urm__wci) WCIClassDescPtr	wci_cldesc_list = NULL;

/*
 *
 *  OWN VARIABLE DECLARATIONS
 *
 */

/*  The hash tables are initialized to NULL by default. */
static Boolean			hash_hash_inited = FALSE;
static URMHashTableEntryPtr	hash_az_hash_table [k_hash_table_size];
static Boolean			cldesc_hash_inited = FALSE;
static URMHashTableEntryPtr	cldesc_hash_table [k_hash_table_size];

/*
 *++
 *
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	MrmRegisterClass saves the information needed to access the widget
 *	creation routine using the information in URM databases.
 *	type conversion of arglists.
 *
 *  FORMAL PARAMETERS:
 *
 *	class_code		NOW UNUSED
 *	class_name		NOW UNUSED
 *	create_name		The case-sensitive name of the low-level
 *				create routine for the class; an example
 *				from the Motif toolkit is XmLabelCreate.
 *				Arguments are (parent, name, arglist, argcount)
 *				The string which names the class in UIL.
 *	creator			The low-level routine named create_name
 *	class_record		The toolkit class record for the class
 *
 *  IMPLICIT INPUTS:
 *
 *  IMPLICIT OUTPUTS:
 *
 *  FUNCTION VALUE:
 *
 *	MrmSUCCESS	operation succeeded
 *	MrmFAILURE	failure allocating class descriptor
 *
 *  SIDE EFFECTS:
 *
 *--
 */
Cardinal MrmRegisterClass(MrmType    class_code, /* unused */
                          const char *class_name, /* unused */
                          const char *create_name,
                          MrmWidgetCreateProc creator,
                          WidgetClass class_record)
{
	Cardinal status;

	(void)class_code;
	(void)class_name;

	return MrmRegisterClassWithCleanup(class_code, class_name, create_name,
	                                   creator, class_record, NULL);
}

Cardinal MrmRegisterClassWithCleanup(MrmType class_code,
                                     const char *class_name,
                                     const char *create_name,
                                     MrmWidgetCreateProc creator,
                                     WidgetClass class_record,
                                     MrmWidgetCleanupProc cleanup)
{
	WCIClassDescPtr cldesc; /* creator descriptor being written */
	URMHashTableEntryPtr hash_entry;

	(void)class_code;
	(void)class_name;
	_MrmProcessLock();

	/* Allocate and fill in a new descriptor */
	if (!create_name) {
		_MrmProcessUnlock();
		return MrmFAILURE;
	}

	cldesc = (WCIClassDescPtr)XtMalloc(sizeof(WCIClassDesc) +
	                                   strlen(create_name) + 1);
	if (!cldesc) {
		_MrmProcessUnlock();
		return MrmFAILURE;
	}

	cldesc->creator_name = (String)cldesc + sizeof(WCIClassDesc);
	strcpy(cldesc->creator_name, create_name);
	cldesc->validation   = URMWCIClassDescValid;
	cldesc->next_desc    = wci_cldesc_list;
	wci_cldesc_list      = cldesc;
	cldesc->creator      = creator;
	cldesc->class_record = class_record;
	cldesc->cleanup      = cleanup;

	/* Enter the descriptor in the descriptor hash table */
	hash_initialize(cldesc_hash_table, &cldesc_hash_inited);
	hash_entry = (URMHashTableEntryPtr)hash_insert_name(cldesc_hash_table,
	                                                    cldesc->creator_name);

	if (hash_entry->az_value)
		XtFree((XtPointer)hash_entry->az_value);
	hash_entry->az_value = (XtPointer)cldesc;

	_MrmProcessUnlock();
	return MrmSUCCESS;
}


Cardinal
Urm__WCI_UnregisterName (String			name)
{
  /*
   * Make sure the hash table is initialized
   */
  hash_initialize (hash_az_hash_table, &hash_hash_inited);
  hash_delete_name (hash_az_hash_table, name);

  return MrmSUCCESS;	/* in any case */
}


/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	This routine inserts the names and their associated values into
 *	the hash table.  No check for duplicate names is made - new
 *	values for duplicate names will override the previous value.
 *
 *  FORMAL PARAMETERS:
 *
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
Urm__WCI_RegisterNames (String			*names,
			XtPointer		*values,
			MrmCount		num_cb)
{

  int			ndx;
  URMHashTableEntryPtr	hash_entry;
  char			*current_name;
  char			*current_value;


  /*
   * Make sure the hash table is initialized
   */
  hash_initialize (hash_az_hash_table, &hash_hash_inited);

  /*
   * Store each name-value pair in the hash table.
   */
  for (ndx = 0 ; ndx < num_cb ; ndx++)
    {
      current_name = names [ndx];
      current_value = values [ndx];
      hash_entry = (URMHashTableEntryPtr)
	hash_insert_name (hash_az_hash_table, current_name);
      hash_entry -> az_value = current_value;
    }

  return MrmSUCCESS;

}



/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	This routin returns a pointer to the class descriptor for a class.
 *	The descriptor hash table is used to look up descriptors.
 *
 *  FORMAL PARAMETERS:
 *
 *	class_name	Class name if class_code is URMwcUnknown
 *	class_return	To return pointer to class descriptor
 *
 *  IMPLICIT INPUTS:
 *
 *  IMPLICIT OUTPUTS:
 *
 *  FUNCTION VALUE:
 *
 *	MrmSUCCESS	operation succeeded
 *	MrmNOT_FOUND	descriptor not found
 *
 *  SIDE EFFECTS:
 *
 *--
 */

Cardinal
Urm__WCI_LookupClassDescriptor (String		class_name,
				WCIClassDescPtr	*class_return)
{
  /*
   *  Local variables
   */
  URMHashTableEntryPtr	hash_entry;	/* hash entry found */
  char			err_msg[300];


  /*
   * Find a hash entry, and return the value.
   */
  hash_entry = (URMHashTableEntryPtr)
    hash_find_name (cldesc_hash_table, class_name);
  if ( hash_entry == NULL )
    {
      *class_return = NULL;
      sprintf (err_msg, _MrmMMsg_0051, class_name);
      return Urm__UT_Error ("Urm__WCI_LookupClassDescriptor",
			    err_msg, NULL, NULL, MrmNOT_FOUND);
    }
  *class_return = (WCIClassDescPtr) hash_entry->az_value;
  return MrmSUCCESS;

}



/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	This routine locates a class record for a given class name,
 *	and returns the widget class record associated with it.
 *	It searches for a match on either the name in the class record
 *	or the creation routine name.
 *
 *  FORMAL PARAMETERS:
 *
 *	clname		the display name of the class
 *
 *  IMPLICIT INPUTS:
 *
 *  IMPLICIT OUTPUTS:
 *
 *  FUNCTION VALUE:
 *
 *	Class record pointer, or NULL if not found
 *
 *  SIDE EFFECTS:
 *
 *--
 */

WidgetClass
Urm__WCI_GetClRecOfName (String		clname)
{
  /*
   *  Local variables
   */
  WCIClassDescPtr		cldesc;		/* current class */

  /*
   * Search the class list
   */
  for ( cldesc=wci_cldesc_list ; cldesc!=NULL ; cldesc=cldesc->next_desc)
    {
      if ( strcmp(clname,cldesc->creator_name) == 0 )
        return cldesc->class_record;
      if ( cldesc->class_record == NULL ) continue;
      if ( strcmp(clname,cldesc->class_record->core_class.class_name) == 0 )
	return cldesc->class_record;
    }

  return NULL;

}



/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	This routine gets, from the hash table, the value that corresponds
 *	to the given name.  If no match is found, NULL is returned.
 *
 *  FORMAL PARAMETERS:
 *
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
Urm__WCI_LookupRegisteredName (String		name,
			       XtPointer	*value_return)
{

  URMHashTableEntryPtr	hash_entry;


  hash_entry = (URMHashTableEntryPtr)
    hash_find_name (hash_az_hash_table, name);

  if (hash_entry == NULL)
    {
      *value_return = NULL;
      return MrmNOT_FOUND;
    }
  else
    {
      *value_return = hash_entry->az_value;
      return MrmSUCCESS;
    }

}



/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**  This routine initializes a hash table.
**
**  FORMAL PARAMETERS:
**
**	htable		the hash table to initialize
**	initflag	if TRUE, the table is already initialized
**
**  IMPLICIT INPUTS:
**
**  IMPLICIT OUTPUTS:
**
**      none
**
**  FUNCTION VALUE:
**
**  SIDE EFFECTS:
**
**      none
**
**--
**/

void
hash_initialize (URMHashTableEntryPtr	*htable,
		 Boolean		*initflag)
{
  int			ndx;	/* loop index */


  /*
   * Initialize only once
   */
  if ( *initflag )
    return;
  for ( ndx=0 ; ndx<k_hash_table_size ; ndx++ )
    htable[ndx] = NULL;
  *initflag = TRUE;

}



/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**  This routine searches for a hash table entry of the same name
**  as its parameters.  If the entry is found, a pointer to the
**  found hash table entry is returned as the value of the function.
**  If no entry is found, a NULL pointer is returned.
**
**  FORMAL PARAMETERS:
**
**	htable		the hash table to search
**	c_text		pointer to a null terminated string for name
**
**  IMPLICIT INPUTS:
**
**  IMPLICIT OUTPUTS:
**
**      none
**
**  FUNCTION VALUE:
**
**      a pointer to a hash table entry, or NULL
**
**  SIDE EFFECTS:
**
**      none
**
**--
**/

URMHashTableEntryPtr
hash_find_name (URMHashTableEntryPtr	*htable,
		char			*c_text)
{
  int				l_length;
  URMHashTableEntryPtr	az_current_name;
  int				l_hash_code;
  int				l_compare_result;

  if (c_text == NULL) return(NULL);

  /* obtain the hash code for the name */

  l_length = strlen (c_text);
  l_hash_code = hash_function( l_length, c_text );

  /*
  **  chain along hash chain looking for symbol - exit loop under 3 condition
  **        1) come to the end of the chain: name not found
  **        2) find symbol: return this symbol
  **        3) find node > symbol: name not found
  */

  for (az_current_name = htable[ l_hash_code ];
       az_current_name != NULL;
       az_current_name = az_current_name->az_next_entry)
    {
      l_compare_result = strcmp (c_text, az_current_name->c_text);

      if (l_compare_result == 0)	/* c_text = current name */
	{
	  /* found the entry we are looking for */

	  return az_current_name;
	}

      if (l_compare_result > 0)	/* c_text > current name */
	{
	  /* return NULL - name should be before this spot in list */

	  return (URMHashTableEntryPtr) NULL;
	}

    }

  /* came to end of the list without finding the name */

  return (URMHashTableEntryPtr) NULL;
}



/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**  This routine searches for a name entry of the same name as its parameters.
**  If the entry is found, a pointer to that entry node is
**  returned as the value of the function.  If no entry is found, one is
**  allocated and inserted.  In this case the value of the function is
**  a pointer to the name entry created.
**
**  Name entries are linked off of a hash table.  Those
**  entries that have the same hash code, are sorted according to the
**  collating sequence.  Thus the algorithm involves hashing the symbol and
**  then following the chain for that hash code until one of the following
**  conditions is met.  1) the identifier is found, then return a pointer
**  to that entry.  2) come to the end of the chain or a hash table
**  entry that comes later in the collating sequence than the symbol being
**  searched for.  In this case the name is inserted just prior to this
**  point in the chain.
**
**  FORMAL PARAMETERS:
**
**	htable		the hash table to update
**	c_text		pointer to a null terminated string for name
**
**  IMPLICIT INPUTS:
**
**  IMPLICIT OUTPUTS:
**
**      htable   may be updated with an additional name
**
**  FUNCTION VALUE:
**
**      a pointer to a hash table entry
**
**  SIDE EFFECTS:
**
**      may create a new entry and update the hash table
**
**--
**/

URMHashTableEntryPtr
hash_insert_name (URMHashTableEntryPtr	*htable,
		  char			*c_text)
{
  int				l_length;
  URMHashTableEntryPtr	az_previous_name;
  URMHashTableEntryPtr	az_current_name;
  URMHashTableEntryPtr	az_new_name;
  int				l_hash_code;
  int				l_compare_result;

  /*
  **  algorithm keeps 2 pointers, one for the previous name and one
  **  for the current name.  This permits easy insertion of a new name
  */


  /* obtain the hash code of for the name */

  l_length = strlen (c_text);
  l_hash_code = hash_function( l_length, c_text );

  /*
  **  chain along hash chain looking for symbol - exit loop under 3 condition
  **        1) come to the end of the chain: insert new node on end
  **        2) find symbol: return this symbol
  **        3) find node > symbol: insert new node prior to current node
  */

  for (az_current_name = htable[ l_hash_code ],
	 az_previous_name = (URMHashTableEntryPtr) NULL;

       az_current_name != (URMHashTableEntryPtr) NULL;

       az_previous_name = az_current_name,
	 az_current_name = az_current_name->az_next_entry)
    {
      l_compare_result = strcmp (c_text, az_current_name->c_text);

      if (l_compare_result == 0)	/* c_text = current name */
	{
	  /* found the name we are looking for */

	  return az_current_name;
	}

      if (l_compare_result > 0)	/* c_text > current name */
	{
	  /* exit the loop to insert just prior to current name */

	  goto insert_name;
	}

    }

insert_name:

  /*
  **	name is not in the table so it must be inserted between the
  **  az_previous_name and az_current_name entries.
  */

  /* allocate and initialize the name entry, including the null */
  az_new_name = (URMHashTableEntryPtr)
    XtMalloc (sizeof(URMHashTableEntry)+l_length+1);

  az_new_name -> az_value = (char *) NULL;
  UrmBCopy (c_text, az_new_name -> c_text, l_length + 1);

  /*
  **  link the name entry into the hash table
  */

  az_new_name->az_next_entry = az_current_name;

  if (az_previous_name == (URMHashTableEntryPtr) NULL)
    htable[ l_hash_code ] = az_new_name;
  else
    az_previous_name->az_next_entry = az_new_name;

  return az_new_name;

}




/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**  just like hash_insert_name but removes the item instead
**--
*/

URMHashTableEntryPtr
hash_delete_name (URMHashTableEntryPtr	*htable,
		  char			*c_text)
{
  int				l_length;
  URMHashTableEntryPtr	az_previous_name;
  URMHashTableEntryPtr	az_current_name;
  int				l_hash_code;
  int				l_compare_result;

  /*
  **  algorithm keeps 2 pointers, one for the previous name and one
  **  for the current name.  This permits easy deletion of a name
  */


  /* obtain the hash code of for the name */

  l_length = strlen (c_text);
  l_hash_code = hash_function( l_length, c_text );

  /*
  **  chain along hash chain looking for symbol - exit loop under 3 condition
  **        1) come to the end of the chain: too far; return
  **        2) find symbol: delete
  **        3) find node > symbol: too far; return
  */

  for (az_current_name = htable[ l_hash_code ],
	 az_previous_name = (URMHashTableEntryPtr) NULL;

       az_current_name != (URMHashTableEntryPtr) NULL;

       az_previous_name = az_current_name,
	 az_current_name = az_current_name->az_next_entry)
    {
      l_compare_result = strcmp (c_text, az_current_name->c_text);

      if (l_compare_result == 0)	/* c_text = current name */
	{
	  /* found the name we are looking for */
	  goto delete_name;
	}

      if (l_compare_result > 0)	/* c_text > current name */
	{
	  /* return NULL - name should be before this spot in list */
	  return (URMHashTableEntryPtr) NULL;
	}

    }
  /* came to end of the list without finding the name */
  return (URMHashTableEntryPtr) NULL;

delete_name:

  /*
  **  remove the name entry from the hash table
  */

  if (az_previous_name == (URMHashTableEntryPtr) NULL)
    htable[ l_hash_code ] = az_current_name->az_next_entry;
  else
    az_previous_name->az_next_entry = az_current_name->az_next_entry;

  XtFree((char*)az_current_name);

  return (URMHashTableEntryPtr) NULL;
}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This procedure is a hashing function.  It takes a length and a
**	pointer to a value.  Using this value as a string, the function
**	returns an integer in the range of 0 to sym_k_hash_table_limit-1.
**
**  FORMAL PARAMETERS:
**
**      l_length	    length of the value in bytes not including null
**	c_value		    a null terminated string
**
**  IMPLICIT INPUTS:
**
**      sym_k_hash_table_limit
**
**  IMPLICIT OUTPUTS:
**
**      none
**
**  FUNCTION VALUE:
**
**      integer (the hash code) in range 0 to sym_k_hash_table_limit-1
**
**  SIDE EFFECTS:
**
**      none
**
**--
**/

static int
hash_function(int	l_length,
	      char	*c_value)
{
#ifdef WORD64
#define _shift 3
  static unsigned int XmConst	mask[ 8 ] =
  { 0x00000000000000FF, 0x000000000000FFFF,
    0x0000000000FFFFFF, 0x00000000FFFFFFFF,
    0x00000000FFFFFFFF, 0x0000FFFFFFFFFFFF,
    0x00FFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, };
#else
#define _shift 2
  static unsigned int XmConst	mask[ 4 ] =
  { 0x000000FF, 0x0000FFFF, 0x00FFFFFF, 0xFFFFFFFF };
#endif
  unsigned int    	l_hash_code;
  unsigned int 	al_value[20];
  int	 	   	l_limit;
  int	  	  	l_extra;
  int	   	 	i;

  /* Don't go past array bounds */
  if (l_length > (sizeof al_value)) l_length = sizeof(al_value) - 1;
  l_limit = (l_length-1) >> _shift;	/* divide by wordsize */
  l_extra = (l_length-1) & _shift;	/* remainder from divide by wordsize */

  memset(al_value, 0, sizeof al_value);
  strncpy((char *)al_value, c_value, l_length);
  l_hash_code = 0;

  for (i = 0 ; i < l_limit ; i++)
    l_hash_code = l_hash_code ^ al_value[ i ];

  l_hash_code = l_hash_code ^ (al_value[ i ] & mask[ l_extra ]);

  /* Make sure result isn't negative */
  return abs((int)(l_hash_code % k_hash_table_size));
}

