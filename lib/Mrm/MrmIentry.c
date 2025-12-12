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
static char rcsid[] = "$XConsortium: MrmIentry.c /main/13 1996/11/13 13:56:11 drk $"
#endif
#endif

/*
 *++
 *  FACILITY:
 *
 *      UIL Resource Manager (URM): IDB Facility
 *	Data entry management routines
 *
 *  ABSTRACT:
 *
 *	These routines get and put data entries from a record into a buffer.
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
#include <Mrm/IDB.h>
#include "MrmMsgI.h"


/*
 *
 *  TABLE OF CONTENTS
 *
 *	Idb__DB_GetDataEntry		- Get data entry into buffer
 *
 *	Idb__DB_PutDataEntry		- Put data entry into record
 *
 */



/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	Idb__DB_GetDataEntry retrieves the requested data entry into a
 *	resource context, returning the number of bytes, resource group, and
 *	resource type. The resource context is resized as required to hold
 *	the data block.
 *
 *  FORMAL PARAMETERS:
 *
 *	file_id		Open ID file
 *	data_entry	Data entry to be fetched
 *	context_id	To receive data block for data entry
 *
 *  IMPLICIT INPUTS:
 *
 *  IMPLICIT OUTPUTS:
 *
 *  FUNCTION VALUE:
 *
 *	MrmSUCCESS	operation succeeded
 *	MrmFAILURE	some other failure
 *
 *  SIDE EFFECTS:
 *
 *--
 */

Cardinal 
Idb__DB_GetDataEntry (IDBFile			file_id,
		      IDBDataHandle		data_entry,
		      URMResourceContextPtr	context_id)
{

  /*
   *  Local variables
   */
  Cardinal		status;		/* return status */
  IDBRecordNumber	record_number ;	/* Record to be read in */
  IDBDataEntryHdrPtr	datahdr ;	/* Header part of entry */
  IDBSimpleDataPtr	sim_data ;	/* Simple data entry */
  IDBOverflowDataPtr	ofl_data ;	/* Overflow data entry */
  IDBDataRecordPtr	data_rec;	/* pointer data record */
  IDBRecordBufferPtr	curbuf ;	/* temp buffer for record */
  Cardinal		num_recs;	/* # records to save overflow */
  Cardinal		cur_rec;	/* the current record */
  char			*buff_ptr;	/* ptr into context buffer */

  /*
   * Check and see if the context is valid
   */
  if (! UrmRCValid (context_id))
    return Urm__UT_Error ("Idb__DB_GetDataEntry", _MrmMMsg_0006,
			  NULL, NULL, MrmBAD_CONTEXT) ;

  /*
   * Make the data entry accessible as a data pointer. Let the header
   * handle this request if the entry is there.
   */
  record_number = data_entry.rec_no;
  if ( record_number == IDBHeaderRecordNumber ) 
    return Idb__HDR_GetDataEntry (file_id, data_entry, context_id);

  /*
   * Get the record that contains this data, get to the correct offset in
   * that record.
   */
  status = Idb__BM_GetRecord (file_id, record_number, &curbuf) ;
  if ( status != MrmSUCCESS ) return status ;

  /*
   * Point to the header in the data entry, set the context data. The context
   * is resized if necessary. Note that all context info except the
   * actual data can be set now regardless of the entry type.
   */
  data_rec = (IDBDataRecord *) curbuf->IDB_record;
  datahdr = (IDBDataEntryHdrPtr)
    &data_rec->data[data_entry.item_offs] ;

  if ((datahdr->validation != IDBDataEntryValid) && ( file_id->byte_swapped ))
    SwapIDBDataEntryHdr(datahdr) ;
  if (datahdr->validation != IDBDataEntryValid)
    return Urm__UT_Error ("Idb__DB_GetDataEntry", _MrmMMsg_0007,
			  NULL, context_id, MrmNOT_VALID) ;

  if ( datahdr->entry_size > UrmRCSize(context_id) )
    {
      status = UrmResizeResourceContext (context_id, datahdr->entry_size) ;
      if ( status != MrmSUCCESS ) return status ;
    }

  UrmRCSetSize (context_id, datahdr->entry_size) ;
  UrmRCSetGroup (context_id, datahdr->resource_group) ;
  UrmRCSetType (context_id, datahdr->resource_type) ;
  UrmRCSetAccess (context_id, datahdr->access) ;
  UrmRCSetLock (context_id, datahdr->lock) ;
  UrmRCSetByteSwap (context_id, file_id->byte_swapped) ;

  /*
   * Read the data into the context. Technique depends on entry type.
   */
  buff_ptr = (char *) UrmRCBuffer(context_id) ;
  switch ( datahdr->entry_type )
    {
    case IDBdrSimple:
      sim_data = (IDBSimpleDataPtr) datahdr ;
      UrmBCopy (sim_data->data, buff_ptr, datahdr->entry_size) ;
      return MrmSUCCESS ;

    case IDBdrOverflow:
      ofl_data = (IDBOverflowDataPtr) datahdr ;
      if ( file_id->byte_swapped ) SwapIDBOverflowData(ofl_data);
      num_recs = ofl_data->segment_count ;
      for ( cur_rec=1 ; cur_rec<=num_recs ; cur_rec++ )
	{
	  UrmBCopy (ofl_data->data, buff_ptr, ofl_data->segment_size) ;
	  buff_ptr += ofl_data->segment_size ;

	  /*
	   * Read the next record in the chain if this is not the last
	   */
	  if ( cur_rec < num_recs )
	    {
	      record_number = ofl_data->next_segment.internal_id.rec_no;
	      status =
		Idb__BM_GetRecord (file_id, record_number, &curbuf) ;
	      if ( status != MrmSUCCESS ) return status ;

	      data_rec = (IDBDataRecord *) curbuf->IDB_record;
	      datahdr = (IDBDataEntryHdrPtr)
		&data_rec->data[data_entry.item_offs] ;
	      if ( file_id->byte_swapped ) SwapIDBDataEntryHdr(datahdr) ;
	      if (datahdr->validation != IDBDataEntryValid)
		return Urm__UT_Error ("Idb__DB_GetDataEntry", _MrmMMsg_0008,
				      NULL, context_id, MrmNOT_VALID) ;
	      ofl_data = (IDBOverflowDataPtr) datahdr ;
	      if ( file_id->byte_swapped ) SwapIDBOverflowData(ofl_data);
	    }
	}
      return MrmSUCCESS ;

    default:
      return Urm__UT_Error ("Idb__DB_GetDataEntry", _MrmMMsg_0009,
			    NULL, context_id, MrmFAILURE) ;
    }

}



/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	Idb_DB_PutDataEntry stores the resource described in the resource
 *	context into the database, returning the resulting data entry pointer.
 *
 *  FORMAL PARAMETERS:
 *
 *	file_id		Open IDB file
 *	context_id	contains data block to be stored
 *	one_entry	To return data entry for newly stored entry
 *
 *  IMPLICIT INPUTS:
 *
 *  IMPLICIT OUTPUTS:
 *
 *  FUNCTION VALUE:
 *
 *	MrmSUCCESS	operation succeeded
 *	MrmFAILURE	some other failure
 *
 *  SIDE EFFECTS:
 *
 *--
 */

Cardinal 
Idb__DB_PutDataEntry (IDBFile			file_id,
		      URMResourceContextPtr	context_id,
		      IDBDataHandle		*data_entry)
{

  /*
   *  Local variables
   */
  Cardinal		result;		/* returned status */
  MrmType		ent_typ ;	/* entry type */
  IDBOverflowDataPtr	overflowdata;	/* complex data entry ptr */
  IDBSimpleDataPtr	simpledata;	/* simple data entry ptr */
  IDBDataRecordPtr	data_rec;	/* pointer data record */
  IDBRecordBufferPtr	curbuf;		/* current record buffer pointer */
  IDBRecordBufferPtr	nxtbuf;		/* next record buffer pointer */
  IDBDataHdrPtr		dataheader;	/* data record header */
  MrmCount		entsiz ;	/* Number of bytes for new entry */
  MrmOffset		entoffs ;	/* Entry offset in buffer */
  Cardinal		num_recs;	/* # records to save overflow */
  Cardinal		cur_rec;	/* the current record */
  char			*dataptr ;	/* pointer to data in context */
  MrmCount		datarem ;	/* # bytes left to copy in data */
  MrmCount		cursiz ;	/* # bytse of data in cur. segment */

  /*
   * Consistency check
   */
  if (! UrmRCValid (context_id))
    return Urm__UT_Error ("Idb__DB_PutDataEntry", _MrmMMsg_0006,
			  NULL, NULL, MrmBAD_CONTEXT) ;

  /*
   * Try to put this entry in the header record.
   */
  result = Idb__HDR_PutDataEntry (file_id, context_id, data_entry );
  if ( result == MrmSUCCESS )
    return result;

  /*
   * Initialize the first data record if required, or use the last one.
   */
  if ( !file_id->last_data_record )
    {
      result = Idb__BM_InitDataRecord (file_id, &curbuf) ;
      if ( result != MrmSUCCESS ) return result ;
      file_id->last_data_record = _IdbBufferRecordNumber (curbuf) ;
    }
  else
    {
      result =
	Idb__BM_GetRecord (file_id, file_id->last_data_record, &curbuf) ;
      if ( result != MrmSUCCESS ) return result ;
    }

  /*
   * This is a simple data entry if it will fit, else it is an overflow entry.
   * The size computes is of a simple entry, which will be used if this turns
   * out to be correct.
   */
  entsiz = IDBSimpleDataHdrSize + UrmRCSize(context_id) ;
  entsiz = _FULLWORD (entsiz) ;
  if ( entsiz <= IDBDataFreeMax )
    ent_typ = IDBdrSimple ;
  else ent_typ = IDBdrOverflow ;

  /*
   * Create the data entry depending on entry type.
   */
  switch ( ent_typ )
    {
    case IDBdrSimple:

      /*
       * Bind pointers into this record, and see if this entry will fit. If
       * not, get a new record and rebind pointers.
       */
      data_rec = (IDBDataRecord *) curbuf->IDB_record;
      dataheader = (IDBDataHdr *) &data_rec->data_header;
      if ( entsiz > dataheader->free_count )
	{
	  result = Idb__BM_InitDataRecord (file_id, &curbuf) ;
	  if ( result != MrmSUCCESS ) return result ;
	  data_rec = (IDBDataRecord *) curbuf->IDB_record;
	  dataheader = (IDBDataHdr *) &data_rec->data_header;
	}

      /*
       * Get the offset for the entry, and create the entry at that offset.
       * Copy in the data contents.
       */
      entoffs = dataheader->free_ptr ;
      simpledata = (IDBSimpleData *) &data_rec->data[entoffs];
      simpledata->header.validation	= IDBDataEntryValid;
      simpledata->header.entry_type	= IDBdrSimple;
      simpledata->header.resource_group	= UrmRCGroup(context_id);
      simpledata->header.resource_type	= UrmRCType(context_id);
      simpledata->header.access		= UrmRCAccess(context_id);
      simpledata->header.entry_size	= UrmRCSize(context_id);
      simpledata->header.lock		= UrmRCLock(context_id);

      dataptr = (char *) UrmRCBuffer(context_id) ;

      UrmBCopy (dataptr, simpledata->data, UrmRCSize(context_id));

      /*
       * Set the return value to the data pointer for this entry
       */	
      data_entry->rec_no = _IdbBufferRecordNumber (curbuf) ;
      data_entry->item_offs = dataheader->free_ptr;

      /*
       * Update the entry chain, mark the buffer, and return.
       */
      simpledata->header.prev_entry = dataheader->last_entry ;
      dataheader->num_entry++ ;
      dataheader->last_entry = entoffs ;
      dataheader->free_ptr += entsiz ;
      dataheader->free_count -= entsiz ;

      Idb__BM_MarkModified (curbuf) ;
      return MrmSUCCESS ;

    case IDBdrOverflow:

      /*
       * Compute the number of records required to hold this entry. Each segment
       * of the entry will begin in a new record, and occupy all of it, except,
       * usually, the last.
       */
      num_recs = (UrmRCSize(context_id)+IDBDataOverflowMax-1) /
	IDBDataOverflowMax ;

      /*
       * Enter a loop to create and fill all the records needed. Initialize the
       * record and pointers outside the loop, as a convenience for chaining
       * the segments. Also set the the result data pointer to point to this
       * record. Note that the offset of all overflow segments is 0.
       */
      result = Idb__BM_InitDataRecord (file_id, &curbuf) ;
      if ( result != MrmSUCCESS ) return result ;
      data_rec = (IDBDataRecordPtr) curbuf->IDB_record ;
      dataheader = (IDBDataHdrPtr) &data_rec->data_header ;
      overflowdata = (IDBOverflowDataPtr) data_rec->data ;

      data_entry->rec_no = _IdbBufferRecordNumber (curbuf) ;
      data_entry->item_offs = 0 ;

      /*
       * Set up pointers to copy the data from the context.
       */
      dataptr = (char *) UrmRCBuffer (context_id) ;
      datarem = UrmRCSize (context_id) ;

      for ( cur_rec=1 ; cur_rec<=num_recs ; cur_rec++ )
	{

	  /*
	   * Set up the header of this segment, and copy in the appropriate part
	   * of the data buffer in the context
	   */
	  cursiz = MIN(datarem, IDBDataOverflowMax) ;
	  entsiz = cursiz + IDBOverflowDataHdrSize ;
	  entsiz = _FULLWORD (entsiz) ;
	  overflowdata->header.validation	= IDBDataEntryValid;
	  overflowdata->header.entry_type	= IDBdrOverflow;
	  overflowdata->header.resource_group	= UrmRCGroup(context_id);
	  overflowdata->header.resource_type	= UrmRCType(context_id);
	  overflowdata->header.access		= UrmRCAccess(context_id);
	  overflowdata->header.lock		= UrmRCLock(context_id);
	  overflowdata->header.entry_size	= UrmRCSize(context_id);

	  UrmBCopy (dataptr, overflowdata->data, cursiz) ;
	  dataptr += cursiz ;
	  datarem -= cursiz ;

	  /*
	   * Set up the segment info, including chaining.  Chaining is
	   * done after the next buffer is acquired.  If this is not the
	   * last record, then the chain is updated.  If it is the last
	   * record, then the free pointer is set.  This code assumes
	   * that acquiring a new data record does not free/reuse the
	   * current buffer. 
	   */
	  overflowdata->segment_size = cursiz ;
	  overflowdata->segment_count = num_recs ;
	  overflowdata->segment_num = cur_rec ;

	  overflowdata->header.prev_entry = 0 ;
	  dataheader->num_entry++ ;
	  dataheader->last_entry = 0 ;
	  dataheader->free_ptr += entsiz ;
	  dataheader->free_count -= entsiz ;

	  Idb__BM_MarkModified (curbuf) ;

	  if ( cur_rec == num_recs ) 
	    {
	      overflowdata->next_segment.internal_id.rec_no = 0 ;
	      overflowdata->next_segment.internal_id.item_offs = 0 ;
	    }
	  else
	    {
	      result = Idb__BM_InitDataRecord (file_id, &nxtbuf) ;
	      if ( result != MrmSUCCESS ) return result ;

	      overflowdata->next_segment.internal_id.rec_no =
		_IdbBufferRecordNumber (nxtbuf) ;
	      overflowdata->next_segment.internal_id.item_offs = 0 ;

	      curbuf = nxtbuf ;
	      data_rec = (IDBDataRecordPtr) curbuf->IDB_record ;
	      dataheader = (IDBDataHdrPtr) &data_rec->data_header ;
	      overflowdata = (IDBOverflowDataPtr) data_rec->data ;
	    }
        }

      return MrmSUCCESS ;
    }

  return MrmFAILURE;
}



/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	This routine checks if a data entry matchs a set of filters.
 *	It reads the record containing the header for the data entry,
 *	then does the filter match. If both filters are NUL, then
 *	the test becomes merely one of confirming the data entry
 *	can be read.
 *
 *  FORMAL PARAMETERS:
 *
 *	file_id		Open ID file
 *	data_entry	Data entry to be matched
 *	group_filter	if not null, entry found must match this group
 *	type_filter	if not null, entry found must match this type
 *
 *  IMPLICIT INPUTS:
 *
 *  IMPLICIT OUTPUTS:
 *
 *  FUNCTION VALUE:
 *
 *	TRUE		Match is good
 *	FALSE		match not good.
 *
 *  SIDE EFFECTS:
 *
 *--
 */

Boolean 
Idb__DB_MatchFilter (IDBFile 		file_id,
		     IDBDataHandle	data_entry,
		     MrmCode 		group_filter,
		     MrmCode 		type_filter)
{

  /*
   *  Local variables
   */
  Cardinal		result ;	/* return status */
  IDBRecordNumber	record_number ;	/* Record to be read in */
  IDBRecordBufferPtr	bufptr ;	/* buffer for data record */
  IDBDataRecordPtr	data_rec;	/* pointer data record */
  IDBDataEntryHdrPtr	datahdr ;	/* Header part of entry */

  /*
   * Get the record that contains this data, get to the correct offset in
   * that record. Immediately decommit the buffer, since it won't be
   * compromised by this read. As usual, go to the header record if required.
   */
  record_number = data_entry.rec_no ;
  if ( record_number == IDBHeaderRecordNumber )
    return Idb__HDR_MatchFilter
      (file_id, data_entry, group_filter, type_filter);

  result = Idb__BM_GetRecord (file_id, record_number, &bufptr) ;
  if ( result != MrmSUCCESS ) return FALSE ;
  Idb__BM_Decommit (bufptr) ;

  /*
   * Point to the header in the entry, and check the filters.
   */
  data_rec = (IDBDataRecord *) bufptr->IDB_record;
  datahdr = (IDBDataEntryHdrPtr) &data_rec->data[data_entry.item_offs] ;
  if (datahdr->validation != IDBDataEntryValid)
    {
      Urm__UT_Error ("Idb__DB_GetDataEntry", _MrmMMsg_0007,
		     NULL, NULL, MrmNOT_VALID) ;
      return FALSE ;
    }

  if ( group_filter!=URMgNul && group_filter!=datahdr->resource_group )
    return FALSE ;
  if ( type_filter!=URMtNul && type_filter!=datahdr->resource_type )
    return FALSE ;

  return TRUE ;

}

