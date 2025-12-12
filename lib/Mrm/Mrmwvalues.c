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
static char rcsid[] = "$TOG: Mrmwvalues.c /main/12 1999/04/16 09:42:44 mgreess $"
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
 *	This module contains the routines which implement modifying
 *	a widget (XtSetValues) from UID literals.
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


/*
 *
 *  TABLE OF CONTENTS
 *
 *	UrmFetchSetValues	Do XtSetValues from UID literals.
 *
 */



/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	UrmFetchSetValues is the internal routine which will
 *	modify a widget via XtSetValues on an argument list whose values
 *	are evaluated literals from a URM hierarchy. 
 *
 *	The args list has ordinary Strings as each tag name. Each value
 *	is interpreted as a String whose value is the index of a literal
 *	to be fetched and evaluated as the actual value. This routine
 *	constructs its own arglist, copying each tag, and setting the
 *	value to the fetched, fixed up, and converted literal value. The
 *	argument is not set in this list (is dropped) if there are any
 *	errors.
 *
 *	This code must also handle pixmaps in the same way that URM does;
 *	it defers converting icons and inserting them in the list
 *	until all the rest of the arglist is done; this allows FG/BG
 *	values to be uncovered in the list for use in the pixmap.
 *
 *  FORMAL PARAMETERS:
 *
 *	hierarchy_id	URM hierarchy from which to read public resources
 *	w		widget to be modified
 *	args		arglist, with literal indexes as values
 *	num_args	# args in args
 *
 *  IMPLICIT INPUTS:
 *
 *  IMPLICIT OUTPUTS:
 *
 *  FUNCTION VALUE:
 *
 *	MrmSUCCESS		operation succeeded
 *	MrmPARTIAL_SUCCESS	at least one fetch suboperation succeeded
 *	other			any error fatal to the operation
 *
 *  SIDE EFFECTS:
 *
 *--
 */

Cardinal 
UrmFetchSetValues (MrmHierarchy		hierarchy_id ,
		   Widget		w ,
		   ArgList		args ,
		   Cardinal		num_args )
{
  /*
   * Local structures
   */
  typedef struct {
    RGMIconImagePtr	icon ;		/* icon to be converted */
    int		pixndx ;	/* argument in arglist (args) */
  } _SavePixmapArg, *_SavePixmapArgPtr ;

  /*
   *  Local variables
   */
  Cardinal		result ;	/* function results */
  int			num_succ = 0;	/* # of successful fetches */
  int			fet_fail = 0;	/* # of failed fetches */
  int			cvt_fail = 0;	/* # failed conversions */
  Cardinal		badfet_res = MrmNOT_FOUND; /* to save NOT_FOUND */
  ArgList		locargs ;	/* local arglist */
  Cardinal		num_used = 0 ;	/* # args used in arglist */
  RGMResourceDescPtr	resptr ;	/* descriptor for literals */
  int			indexlen = 0 ;	/* longest literal length */
  URMPointerListPtr	ptrlist ;	/* to hold scratch contexts */
  int			ndx ;		/* loop index */
  IDBFile		file_id ;	/* file from which literal read */
  MrmType		reptype ;	/* arg value representation type */
  long			val ;		/* value as immediate or pointer */
  int			vec_count ;	/* number of items in val if vector */
  int			vec_size ;
  _SavePixmapArg	pixargs[10] ;	/* to save pixmap args */
  Cardinal		pixargs_cnt = 0 ;  /* # pixargs saved */
  _SavePixmapArgPtr	savepix ;	/* current saved pixmap entry */
  Screen		*screen ;	/* screen for pixmaps */
  Display		*display ;	/* display for pixmaps */
  Pixel			fgint = (Pixel) -1 ; /* fg for pixmaps. */
  Pixel			bgint = (Pixel) -1 ; /* background for pixmaps */
  Pixmap		pixmap ;	/* pixmap created from icon */
  Boolean		swap_needed = FALSE; 

  /*
   * Create local arglist and pointer list for contexts.
   */
  locargs = (ArgList) XtMalloc (num_args*sizeof(Arg)) ;
  UrmPlistInit (num_args, &ptrlist) ;

  /*
   * Find the longest literal index, and allocate a resource descriptor
   * which can hold it. Set the fixed fields of the descriptor.
   */
  for ( ndx=0 ; ndx<num_args ; ndx++ )
    indexlen = MAX (indexlen, strlen((char*)args[ndx].value)) ;

  /*
   * Create a resource descriptor which can be reused to fetch each literal
   */
  resptr = (RGMResourceDescPtr) XtMalloc (sizeof(RGMResourceDesc)+indexlen) ;
  resptr->access = URMaPublic ;
  resptr->type = URMrIndex ;
  resptr->res_group = URMgLiteral ;
  resptr->cvt_type = URMtNul ;

  /*
   * Loop through all the entries in the given arglist. Copy each tag into
   * the local arglist. Create a new context to hold the literal, then
   * try to read it from the hierarchy.
   */
  for ( ndx=0 ; ndx<num_args ; ndx++ )
    {
      locargs[num_used].name = args[ndx].name ;
      strcpy (resptr->key.index, (char*)args[ndx].value) ;
      result = Urm__CW_ReadLiteral (resptr, hierarchy_id, NULL,
				    ptrlist, &reptype, &val, &vec_count, 
				    &file_id, &vec_size) ;
      if ( result == MrmSUCCESS )
	num_succ += 1;
      else
	{
	  fet_fail += 1;
	  if ( result != MrmNOT_FOUND )
	    badfet_res = result;
	  continue;
	}

      /*
       * Fix up and perform conversion on the value. If this succeeds, put it
       * in the local arglist. Special casing is required for some types,
       * particularly icons.
       *
       * Type conversion for some ASCIZ strings is also provided as a V1
       * compatibility feature.
       */
      switch ( reptype )
        {
	case MrmRtypeIconImage:
	  savepix = &pixargs[pixargs_cnt] ;
	  savepix->icon = (RGMIconImagePtr) val ;
	  savepix->pixndx = ndx ;
	  pixargs_cnt += 1 ;
	  continue ;
	case MrmRtypeFontList:
	  /* Check for 1.1 version and malloc new fontlist if necessary. */
	  if (strcmp(file_id->db_version, URM1_1version) <= 0)
	    {
	      int count = ((OldRGMFontListPtr)val)->count;
	      RGMFontListPtr fontlist = (RGMFontListPtr)
		XtMalloc(sizeof(RGMFontList) +
			 (sizeof(RGMFontItem) * (count - 1)));
	      result = Urm__CW_FixupValue((long)fontlist, reptype, 
					  (XtPointer)val, file_id,
					  &swap_needed);
	      XtFree((char *)val);
	      val = (long)fontlist;
	    }
	  else
	    result = Urm__CW_FixupValue (val, reptype, (XtPointer)val, 
					 file_id, &swap_needed) ;

	default:
	  result = 
	    Urm__CW_FixupValue (val, reptype, (XtPointer)val, file_id,
				&swap_needed) ;
	  if ( result != MrmSUCCESS )
	    {
	      num_succ -= 1;
	      cvt_fail += 1;
	      continue;
	    }
	  if (XtIsWidget(w))
	    display = XtDisplay(w);
	  else
	    display = XtDisplay(XtParent(w));
	    
	  result = Urm__CW_ConvertValue (XtParent(w), &val, reptype, (MrmType)0,
					 display, hierarchy_id,  NULL) ;
	  if ( result != MrmSUCCESS )
	    {
	      num_succ -= 1;
	      cvt_fail += 1;
	      continue;
	    }
	  locargs[num_used].value = (XtArgVal)val ;
	  num_used += 1 ;
	  break ;
        }

      /*
       * Do any processing based on specific arguments (after conversion):
       *	- retain FG/BG info
       */
      switch ( reptype )
	{
	case MrmRtypeColor:
	  if ( strcmp(args[ndx].name,XmNforeground) == 0 )
	    fgint = val;

	  else if ( strcmp (args[ndx].name,XmNbackground) == 0 )
	    bgint = val;
	  break ;
        }

    }	/* end loop */

  /*
   * Now set any pixmap arguments. This requires finding the display, screen,
   * foreground, and background values for this widget. These values are
   * available from the parent widget and the arglist.
   */
  if ( pixargs_cnt > 0 )
    {
      Urm__CW_GetPixmapParms (w, &screen, &display, &fgint, &bgint) ;
      for ( ndx=0,savepix=pixargs ; ndx<pixargs_cnt ; ndx++,savepix++ )
        {
	  result = UrmCreatePixmap (savepix->icon, screen, display,
				    (Pixel)fgint, (Pixel)bgint, &pixmap, w) ;
	  if ( result != MrmSUCCESS )
	    {
	      num_succ -= 1;
	      cvt_fail += 1;
	      continue;
	    }
	  locargs[num_used].name = args[savepix->pixndx].name ;
	  locargs[num_used].value = (XtArgVal) pixmap ;
	  num_used += 1 ;
        }
    }

  /*
   * arglist creation complete. Modify the widget if we have succeeded in
   * fetching any values. Deallocate resources.
   */
  if ( num_used > 0 )
    XtSetValues (w, locargs, num_used) ;

  XtFree ((char*)locargs) ;
  XtFree ((char*)resptr) ;
  for ( ndx=0 ; ndx<UrmPlistNum(ptrlist) ; ndx++ )
    UrmFreeResourceContext ((URMResourceContextPtr)UrmPlistPtrN(ptrlist,ndx)) ;
  UrmPlistFree (ptrlist) ;

  /*
   * Result depends on number of successes vs. number of failurs
   */
  if ( num_succ > 0 )
    {
      if ( fet_fail==0 && cvt_fail == 0 )
	return MrmSUCCESS;
      else
	return MrmPARTIAL_SUCCESS;
    }
  if ( fet_fail > 0 )
    return badfet_res;
  else
    return MrmFAILURE;

}

