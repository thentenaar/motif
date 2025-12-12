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
static char rcsid[] = "$XConsortium: MrmIindexw.c /main/12 1996/11/13 13:57:54 drk $"
#endif
#endif


/*
 *++
 *  FACILITY:
 *
 *      UIL Resource Manager (URM): IDB Facility
 *	Index management routines
 *
 *  ABSTRACT:
 *
 *	These routines manage the index of an IDB file, including 
 *	retrieving data entries accessed by index, and maintaing the
 *	index structure, particularly index splitting
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
 *	Idb__INX_EnterItem		- Enter a data entry under an index
 *
 *	Idb__INX_EnterLeafIndex		- Add an entry to a leaf record
 *
 *	Idb__INX_EnterNodeIndex		- Add an entry to a node record
 *
 *	Idb__INX_SplitLeafRecord	- Split a leaf index record
 *
 *	Idb__INX_SplitNodeRecord	- Split a node index record
 *
 *	Idb__INX_InitRootLeafRecord	- Init a (root) leaf index record
 *
 *	Idb__INX_InitRootNodeRecord	- Init a (root) node index record
 *
 *	Idb__INX_CopyLeafRecord		- Copy a leaf record
 *
 *	Idb__INX_CopyNodeRecord		- Copy a node record
 *
 *	Idb__INX_CollapseLeafRecord	- Collapse a leaf record (truncate)
 *
 *	Idb__INX_CollapseNodeRecord	- Collapse a node record (truncate)
 *
 *	Idb__INX_ConfirmNodeSpace	- Confirm enough space in node
 *					  record for new entry
 *
 *	Idb__INX_SetParent		- Set parent pointer in record
 *
 *	Idb__INX_FixNodeChildren	- Reset parent pointers for all
 *					  the children of some node
 *
 */


/*
 *
 *  DEFINE and MACRO DEFINITIONS
 *
 */

/*
 * Macros which validate index records in buffers
 */
#define	Idb__INX_ValidLeaf(buffer) \
     (_IdbBufferRecordType(buffer)==IDBrtIndexLeaf)
#define	Idb__INX_ValidNode(buffer) \
     (_IdbBufferRecordType(buffer)==IDBrtIndexNode)
#define	Idb__INX_ValidRecord(buffer) \
     (_IdbBufferRecordType(buffer)==IDBrtIndexLeaf ||  \
      _IdbBufferRecordType(buffer)==IDBrtIndexNode)



/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	Idb__INX_EnterItem makes an entry in the file's index for a data
 *	entry which has been previously entered in the file.
 *
 *  FORMAL PARAMETERS:
 *
 *	file_id		Open IDB file in which to write entry
 *	index		The entry's case-sensitive index
 *	data_entry	Data entry pointer for data
 *
 *  IMPLICIT INPUTS:
 *
 *  IMPLICIT OUTPUTS:
 *
 *  FUNCTION VALUE:
 *
 *	MrmSUCCESS	operation succeeded
 *	MrmEXISTS	index already exists in file
 *	MrmFAILURE	some other failure
 *
 *  SIDE EFFECTS:
 *
 *--
 */

Cardinal 
Idb__INX_EnterItem (IDBFile		file_id ,
		    char		*index ,
		    IDBDataHandle	data_entry )
{

  /*
   *  Local variables
   */
  Cardinal		result ;	/* function results */
  IDBRecordBufferPtr	bufptr ;	/* buffer into which to stuff entry */
  MrmCount		entndx ;	/* locates pivotal entry in buffer */


  /*
   * Initialize the index with this entry if this is the initial one.
   */
  if ( !file_id->index_root )
    {
      result = Idb__INX_InitRootLeafRecord (file_id, &bufptr) ;
      if (result != MrmSUCCESS ) return result ;
      result = Idb__INX_EnterLeafIndex
        (file_id, bufptr, index, data_entry, 0, MrmINDEX_LT) ;
      return result ;
    }

  /*
   * Find the (leaf) record in which to place this entry, and the
   * position in the record. Place it in the record (which must be
   * a leaf record). This process loops as long as record splitting
   * forces retries.
   */
  do  {
    result = Idb__INX_FindIndex (file_id, index, &bufptr, &entndx) ;
    switch ( result )
      {
      case MrmINDEX_GT:
      case MrmINDEX_LT:
	break ;
      case MrmSUCCESS:
	return MrmEXISTS ;
      default:
	return result ;
      }
    result = Idb__INX_EnterLeafIndex
      (file_id, bufptr, index, data_entry, entndx, result) ;
  } while ( result == MrmINDEX_RETRY ) ;

  /*
   * Return results of final attempt to stuff in a leaf record
   */
  return result ;

}



/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	Idb__INX_EnterLeafIndex creates a new entry for a data entry in a
 *	leaf index record. If there isn't enough room in the record for
 *	the new entry, the record is split and the enter operation must
 *	be retried.
 *
 *  FORMAL PARAMETERS:
 *
 *	file_id		Open IDB file
 *	buffer		Buffer containing leaf index record
 *	index		The entry's case-sensitive index
 *	data_entry	Data entry pointer for data
 *	entry_index	Entry in record at which to force new entry
 *	order		Specifies how new entry orders WRT entry at
 *			entry_index; MrmINDEX_GT or MrmINDEX_LT.
 *
 *  IMPLICIT INPUTS:
 *
 *  IMPLICIT OUTPUTS:
 *
 *  FUNCTION VALUE:
 *
 *	MrmSUCCESS	operation succeeded
 *	MrmINDEX_RETRY	operation must be tried again.
 *	MrmFAILURE	some other failure
 *
 *  SIDE EFFECTS:
 *
 *--
 */

Cardinal 
Idb__INX_EnterLeafIndex (IDBFile		file_id,
			 IDBRecordBufferPtr	buffer,
			 char			*index,
			 IDBDataHandle		data_entry,
			 MrmCount		entry_index,
			 Cardinal		order)
{

  /*
   *  Local variables
   */
  Cardinal		result ;	/* function results */
  IDBIndexLeafRecordPtr	recptr ;	/* leaf record in buffer */
  IDBIndexLeafHdrPtr	hdrptr ;	/* record header */
  MrmCount		entndx ;	/* index for new entry */
  Cardinal		entsiz ;	/* # bytes needed for new entry */
  MrmCount		ndxsiz ;	/* # bytes needed for new string */
  char			*ndxstg ;	/* location for new string */
  int			ndx ;		/* loop index */
  char			*stgheap ;	/* string heap beginning */
  MrmCount		nfree ;		/* # free bytes */
  IDBIndexLeafEntryPtr	itemvec ;	/* The vector of index entries */
  MrmCount		itemcnt ;	/* # entries in vector */


  /*
   * Initialize pointers into the record
   */
  recptr = (IDBIndexLeafRecordPtr) buffer->IDB_record ;
  hdrptr = (IDBIndexLeafHdrPtr) &recptr->leaf_header ;

  /*
   * Compute sizes for new entry. Split record and retry if required
   * to get enough space.
   */
  ndxsiz = MIN(strlen(index),IDBMaxIndexLength) + 1 ;
  ndxsiz = _FULLWORD(ndxsiz);
  entsiz = IDBIndexLeafEntrySize + ndxsiz ;
  nfree = hdrptr->free_bytes ;
  if ( entsiz > nfree )
    {
      result = Idb__INX_SplitLeafRecord (file_id, buffer) ;
      if ( result != MrmSUCCESS ) return result ;
      return MrmINDEX_RETRY ;
    }

  /*
   * Pick up values and pointers into the record.
   * Adjust entry index based on ordering, then make room for the
   * new entry.
   */
  stgheap = (char *) &recptr->index[0] + hdrptr->heap_start ;
  itemvec = recptr->index ;
  itemcnt = hdrptr->index_count ;

  entndx = (order==MrmINDEX_GT) ? entry_index+1 : entry_index ;
  ndxstg = (char *) stgheap - ndxsiz ;

  for ( ndx=itemcnt ; ndx>entndx ; ndx--)
    {
      itemvec[ndx].index_stg = itemvec[ndx-1].index_stg ;
      itemvec[ndx].data = itemvec[ndx-1].data ;
    }

  /*
   * Move the string and set the values in the vector entry
   */
  strcpy (ndxstg, "") ;
  strncat (ndxstg, index, IDBMaxIndexLength) ;
  itemvec[entndx].index_stg = (MrmOffset) (ndxstg-(char *)itemvec) ;
  itemvec[entndx].data.internal_id.rec_no = data_entry.rec_no ;
  itemvec[entndx].data.internal_id.item_offs = data_entry.item_offs ;

  /*
   * update the header
   */
  hdrptr->index_count++ ;
  hdrptr->heap_start -= ndxsiz ;
  hdrptr->free_bytes -= entsiz ;

  /*
   * entry successfully added
   */
  Idb__BM_MarkModified (buffer) ;
  return MrmSUCCESS ;

}



/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	Idb__INX_EnterNodeIndex creates a new entry for a data entry in a
 *	node index record. It differs from entering an item in a leaf record
 *	in that the position for the new entry is not known.
 *	If there isn't room for the new entry, the record is split, and
 *	the operation must be tried again.
 *
 *  FORMAL PARAMETERS:
 *
 *	file_id		Open IDB file
 *	buffer		Buffer containing node index record
 *	index		The entry's case-sensitive index
 *	data_entry	Data entry pointer for data
 *	lt_record	Record number of the less-than record associated with
 *			this entry
 *	gt_record	Record number of the greater-than record associated with
 *			this entry
 *
 *  IMPLICIT INPUTS:
 *
 *  IMPLICIT OUTPUTS:
 *
 *  FUNCTION VALUE:
 *
 *	MrmSUCCESS	operation succeeded
 *	MrmINDEX_RETRY	operation must be tried again.
 *	MrmFAILURE	some other failure
 *
 *  SIDE EFFECTS:
 *
 *--
 */

Cardinal 
Idb__INX_EnterNodeIndex (IDBFile		file_id,
			 IDBRecordBufferPtr	buffer,
			 char			*index,
			 IDBDataHandle		data_entry,
			 IDBRecordNumber	lt_record,
			 IDBRecordNumber	gt_record)
{

  /*
   *  Local variables
   */
  Cardinal		result ;	/* function results */
  IDBIndexNodeRecordPtr	recptr ;	/* node record in buffer */
  IDBIndexNodeHdrPtr	hdrptr ;	/* record header */
  MrmCount		entry_index ;	/* searched location for new entry */
  Cardinal		order ;		/* order of index WRT location */
  MrmCount		entndx ;	/* index for new entry */
  Cardinal		entsiz ;	/* # bytes needed for new entry */
  MrmCount		ndxsiz ;	/* # bytes needed for new string */
  char			*ndxstg ;	/* location for new string */
  int			ndx ;		/* loop index */
  char			*stgheap ;	/* string heap beginning */
  MrmCount		nfree ;		/* # free bytes */
  IDBIndexNodeEntryPtr	itemvec ;	/* The vector of index entries */
  MrmCount		itemcnt ;	/* # entries in vector */
  MrmCount		prvndx ;	/* preceding entry index */
  MrmCount		nxtndx ;	/* succeeding entry index */
  IDBRecordNumber	p_recno ;	/* this node record number */


  /*
   * Initialize pointers into the record
   */
  recptr = (IDBIndexNodeRecordPtr) buffer->IDB_record ;
  hdrptr = (IDBIndexNodeHdrPtr) &recptr->node_header ;

  /*
   * Compute sizes for new entry. Split record and retry if required
   * to get enough space.
   */
  ndxsiz = MIN(strlen(index),IDBMaxIndexLength) + 1 ;
  ndxsiz = _FULLWORD(ndxsiz);
  entsiz = IDBIndexNodeEntrySize + ndxsiz ;
  nfree = hdrptr->free_bytes ;
  if ( entsiz > nfree )
    {
      result = Idb__INX_SplitNodeRecord (file_id, buffer) ;
      if ( result != MrmSUCCESS ) return result ;
      return MrmINDEX_RETRY ;
    }

  /*
   * Pick up value and pointers into the record. Figure out the
   * location at which to insert the record (0 for a new entry),
   * and make room for the new entry.
   */
  stgheap = (char *) &recptr->index[0] + hdrptr->heap_start ;
  itemvec = recptr->index ;
  itemcnt = hdrptr->index_count ;
  if ( itemcnt == 0 )
    entndx = 0 ;
  else
    {
      order = Idb__INX_SearchIndex (file_id, index, buffer, &entry_index) ;
      entndx = (order==MrmINDEX_GT) ? entry_index+1 : entry_index ;
      for ( ndx=itemcnt ; ndx>entndx ; ndx--)
        {
	  itemvec[ndx].index_stg = itemvec[ndx-1].index_stg ;
	  itemvec[ndx].data = itemvec[ndx-1].data ;
	  itemvec[ndx].LT_record = itemvec[ndx-1].LT_record ;
	  itemvec[ndx].GT_record = itemvec[ndx-1].GT_record ;
        }
    }

  /*
   * Move the string and set the values in the vector entry and record vector
   */
  ndxstg = (char *) stgheap - ndxsiz ;
  strcpy (ndxstg, "") ;
  strncat (ndxstg, index, IDBMaxIndexLength) ;
  itemvec[entndx].index_stg = (MrmOffset) (ndxstg-(char *)itemvec) ;
  itemvec[entndx].data.internal_id.rec_no = data_entry.rec_no ;
  itemvec[entndx].data.internal_id.item_offs = data_entry.item_offs ;
  itemvec[entndx].LT_record = lt_record ;
  itemvec[entndx].GT_record = gt_record ;

  /*
   * update the header
   */
  hdrptr->index_count = ++itemcnt ;
  hdrptr->heap_start -= ndxsiz ;
  hdrptr->free_bytes -= entsiz ;

  /*
   * Now the entries to either side of the new index must have their LT
   * and LT pointers verified and changed. By practice, the GT record of the
   * new entry is a previous record which should occur as the GT record of
   * the preceding entry and the LT record of the succeeding entry.
   * These entries must be modified to have LT and GT pointers matching the
   * records given here as arguments.
   */
  if ( entndx > 0 )
    {
      prvndx = entndx - 1 ;
      if ( itemvec[prvndx].GT_record != gt_record )
        return Urm__UT_Error ("Idb__INX_EnterNodeIndex", _MrmMMsg_0016,
			      file_id, NULL, MrmBAD_BTREE) ;
      itemvec[prvndx].GT_record = lt_record ;
    }
  if ( entndx < (itemcnt-1) )
    {
      nxtndx = entndx + 1 ;
      if ( itemvec[nxtndx].LT_record != gt_record )
        return Urm__UT_Error ("Idb__INX_EnterNodeIndex", _MrmMMsg_0017,
			      file_id, NULL, MrmBAD_BTREE) ;
      itemvec[nxtndx].LT_record = gt_record ;
    }

  /*
   * entry successfully added
   */
  Idb__BM_MarkModified (buffer) ;

  /*
   * Set the parent pointer in the LT  and GT records
   */
  p_recno = _IdbBufferRecordNumber (buffer) ;
  result = Idb__INX_SetParent (file_id, p_recno, lt_record) ;
  result = Idb__INX_SetParent (file_id, p_recno, gt_record) ;
  if ( result != MrmSUCCESS ) return result ;

  return MrmSUCCESS ;

}



/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	Idb__INX_SplitRecord splits an index record in order to make room
 *	for new entries. This is the crucial routine in causing the B-tree
 *	to grow. It splits leaf records only.
 *
 *	The split process takes place as follows:
 *		- extract the middle entry from the record and save it.
 *		- create a new leaf record and move all the less-than
 *		  ordered entries into it.
 *		- reorganize the current record to contain only the greater-than
 *		  ordered entries (garbage collecting the string heap).
 *		- Enter the extracted entry in the parent (creating a new
 *		  parent if required). This entry takes the less-than
 *		  record with it to the parent, thus entering the new leaf
 *		  record in the B-tree. The old record is retained as a
 *		  greater-than record for the extracted index.
 *
 *	The trickiest aspect of splitting nodes is maintaining parent
 *	pointers when splitting may cause parents to change. IDB deals
 *	this by splitting from the top of the tree down, so that a node's
 *	parent pointer is guaranteed correct when the node is split.
 *	This is done by:
 *		- Before splitting, check that parent has enough room
 *		  for a new entry. If not, the parent will split and
 *		  inform the caller to retry.
 *		- Splitting the root node is always safe
 *
 *  FORMAL PARAMETERS:
 *
 *	file_id		Open IDB file
 *	gt_buffer	Buffer containing leaf index record to be split
 *			This will become the new GT buffer
 *
 *  IMPLICIT INPUTS:
 *
 *  IMPLICIT OUTPUTS:
 *
 *  FUNCTION VALUE:
 *
 *	MrmSUCCESS	operation succeeded
 *	MrmBAD_RECORD	not a leaf index record
 *	MrmFAILURE	some other failure
 *
 *  SIDE EFFECTS:
 *
 *--
 */

Cardinal 
Idb__INX_SplitLeafRecord (IDBFile		file_id,
			  IDBRecordBufferPtr	gt_buffer)
{

  /*
   *  Local variables
   */
  Cardinal		result ;	/* function results */
  IDBRecordNumber	p_recno ;	/* parent record number */
  IDBRecordBufferPtr	p_buffer ;	/* parent buffer */
  IDBRecordBufferPtr	lt_buffer ;	/* buffer for LT leaf record */
  IDBIndexLeafRecordPtr	lt_recptr ;	/* LT leaf record in buffer */
  IDBIndexLeafRecordPtr	gt_recptr ;	/* GT leaf record in buffer */
  IDBIndexLeafHdrPtr	gt_hdrptr ;	/* GT record header */
  MrmCount		p_index ;	/* index of hoisted entry */
  char			p_index_stg[IDBMaxIndexLength1] ; /* save hoisted idx */
  char			*p_index_stgadr ;  /* Address of hoisted index string */
  IDBDataHandle		p_data ;	/* saves hoisted entry data */
  MrmCount		lt_cnt ;	/* number of LT items */
  MrmCount		gt_cnt ;	/* number of GT items */
  MrmCount		old_cnt ;	/* original number of items */
  IDBIndexLeafEntryPtr	old_itmvec ;	/* Original vector */


  /*
   * Initialize pointers into the record and sanity check. This record
   * will become the GT leaf record
   */
  if ( ! Idb__INX_ValidLeaf(gt_buffer) )
    return Urm__UT_Error ("Idb__INX_SplitLeafRecord", _MrmMMsg_0010,
			  file_id, NULL, MrmBAD_RECORD) ;

  gt_recptr = (IDBIndexLeafRecordPtr) gt_buffer->IDB_record ;
  gt_hdrptr = (IDBIndexLeafHdrPtr) &gt_recptr->leaf_header ;

  /*
   * If this node has a parent, make sure it can hold a new entry.
   * If not, it will split, and we must retry. Note a parent must be
   * a node record.
   */
  p_recno = gt_hdrptr->parent ;
  if ( gt_hdrptr->parent )
    {
      result = Idb__BM_GetRecord (file_id, gt_hdrptr->parent, &p_buffer) ;
      if ( result != MrmSUCCESS ) return result ;
      if ( ! Idb__INX_ValidNode(p_buffer) )
        return Urm__UT_Error ("Idb__INX_SplitLeafRecord", _MrmMMsg_0018,
			      file_id, NULL, MrmBAD_RECORD) ;
      result = Idb__INX_ConfirmNodeSpace (file_id, p_buffer) ;
      if ( result != MrmSUCCESS ) return result ;
    }

  /*
   * Pick up current parameters
   */
  old_cnt = gt_hdrptr->index_count ;
  old_itmvec = gt_recptr->index ;

  /*
   * Compute the indexes and counts for the split, and save the hoisted entry.
   */
  lt_cnt = old_cnt / 2 ;
  p_index = lt_cnt ;
  gt_cnt = old_cnt - lt_cnt - 1;
  p_index_stgadr = (char *) old_itmvec+old_itmvec[p_index].index_stg ;
  strcpy (p_index_stg, p_index_stgadr) ;
  p_data.rec_no = old_itmvec[p_index].data.internal_id.rec_no ;
  p_data.item_offs = old_itmvec[p_index].data.internal_id.item_offs ;

  /*
   * Acquire a new record to become the LT part. Copy the entire current
   * record into it, then collapse both records into their final form.
   */
  result = Idb__BM_InitRecord (file_id, 0, IDBrtIndexLeaf, &lt_buffer) ;
  lt_recptr = (IDBIndexLeafRecordPtr) lt_buffer->IDB_record ;
  Idb__INX_CopyLeafRecord (lt_recptr, gt_recptr) ;
  Idb__INX_CollapseLeafRecord (lt_recptr, 0, lt_cnt-1) ;
  Idb__INX_CollapseLeafRecord (gt_recptr, p_index+1, p_index+gt_cnt) ;

  /*
   * Both records now have their parent set correctly via the copy operation,
   * since our check on space in the parent guarantees that the parent
   * pointer present in the original buffer will remain the parent after
   * we host the pivot index (unless we create a new root node, which
   * is guaranteed safe anyway. So we we can mark the buffers and
   * hoist the pivot index.
   */
  Idb__BM_MarkModified (lt_buffer) ;
  Idb__BM_MarkModified (gt_buffer) ;

  /*
   * Either enter the hoisted entry into the parent, or create
   * a parent (which will be the root record).
   */
  if ( !p_recno )
    {
      result = Idb__INX_InitRootNodeRecord
        (file_id, &p_buffer, p_index_stg, p_data,
         _IdbBufferRecordNumber(lt_buffer), _IdbBufferRecordNumber(gt_buffer)) ;
      return result ;
    }

  /*
   * Hoist the entry into the parent (we know there should be room).
   * The parent is already loaded in its buffer as part of the space check.
   */
  result = Idb__INX_EnterNodeIndex
    (file_id, p_buffer, p_index_stg, p_data,
     _IdbBufferRecordNumber(lt_buffer), _IdbBufferRecordNumber(gt_buffer)) ;
  if ( result != MrmSUCCESS ) return result ;

  /*
   * Successfully added.
   */
  return MrmSUCCESS ;

}



/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	Idb__INX_SplitRecord splits an index record in order to make room
 *	for new entries. This is the crucial routine in causing the B-tree
 *	to grow. It splits node records only.
 *
 *	The split process takes place as follows:
 *		- extract the middle entry from the record and save it.
 *		- create a new node record and move all the less-than
 *		  ordered entries into it.
 *		- reorganize the current record to contain only the greater-than
 *		  ordered entries (garbage collecting the string heap).
 *		- Enter the extracted entry in the parent (creating a new
 *		  parent if required). This entry takes the less-than
 *		  record with it to the parent, thus entering the new node
 *		  record in the B-tree. The old record is retained as a
 *		  greater-than record for the extracted index.
 *
 *	For node records, the record vectors are handled entirely by
 *	the collapse routines. No record number is saved for the hoisted
 *	index, since the associated record number is either new LT record
 *	or both records if a root node is created.
 *
 *  FORMAL PARAMETERS:
 *
 *	file_id		Open IDB file
 *	gt_buffer	Buffer containing node index record to be split. This
 *			will become the new GT buffer.
 *
 *  IMPLICIT INPUTS:
 *
 *  IMPLICIT OUTPUTS:
 *
 *  FUNCTION VALUE:
 *
 *	MrmSUCCESS	operation succeeded
 *	MrmBAD_RECORD	not a node index record
 *	MrmFAILURE	some other failure
 *
 *  SIDE EFFECTS:
 *
 *--
 */

Cardinal
Idb__INX_SplitNodeRecord (IDBFile		file_id,
			  IDBRecordBufferPtr	gt_buffer)
{

  /*
   *  Local variables
   */
  Cardinal		result ;	/* function results */
  IDBRecordNumber	p_recno ;	/* parent record number */
  IDBRecordBufferPtr	p_buffer ;	/* parent buffer */
  IDBRecordBufferPtr	lt_buffer ;	/* buffer for LT node record */
  IDBIndexNodeRecordPtr	lt_recptr ;	/* LT node record in buffer */
  IDBIndexNodeRecordPtr	gt_recptr ;	/* GT node record in buffer */
  IDBIndexNodeHdrPtr	gt_hdrptr ;	/* GT record header */
  IDBRecordNumber	lt_recno ;	/* LT node record number */
  IDBRecordNumber	gt_recno ;	/* GT node record number */
  MrmCount		p_index ;	/* index of hoisted entry */
  char			p_index_stg[IDBMaxIndexLength1]; /* save hoisted indx */
  char			*p_index_stgadr ; /* Address of hoisted index string */
  IDBDataHandle		p_data ;	/* saves hoisted entry data */
  MrmCount		lt_cnt ;	/* number of LT items */
  MrmCount		gt_cnt ;	/* number of GT items */
  MrmCount		old_cnt ;	/* original number of items */
  IDBIndexNodeEntryPtr	old_itmvec ;	/* Original vector */


  /*
   * Initialize pointers into the record and sanity check. This record
   * will become the GT node record
   */
  if ( ! Idb__INX_ValidNode(gt_buffer) )
    return Urm__UT_Error ("Idb__INX_SplitNodeRecord", _MrmMMsg_0010,
			  file_id, NULL, MrmBAD_RECORD) ;

  gt_recptr = (IDBIndexNodeRecordPtr) gt_buffer->IDB_record ;
  gt_hdrptr = (IDBIndexNodeHdrPtr) &gt_recptr->node_header ;

  /*
   * If this node has a parent, make sure it can hold a new entry.
   * If not, it will split, and we must retry. Note a parent must be
   * a node record.
   */
  p_recno = gt_hdrptr->parent ;
  if ( p_recno )
    {
      result = Idb__BM_GetRecord (file_id, p_recno, &p_buffer) ;
      if ( result != MrmSUCCESS ) return result ;
      if ( ! Idb__INX_ValidNode(p_buffer) )
        return Urm__UT_Error ("Idb__INX_SplitNodeRecord", _MrmMMsg_0018,
			      file_id, NULL, MrmBAD_RECORD) ;
      result = Idb__INX_ConfirmNodeSpace (file_id, p_buffer) ;
      if ( result != MrmSUCCESS ) return result ;
    }

  /*
   * Pick up current parameters
   */
  old_cnt = gt_hdrptr->index_count ;
  old_itmvec = gt_recptr->index ;

  /*
   * Compute the indexes and counts for the split, and save the hoisted entry.
   */
  lt_cnt = old_cnt / 2 ;
  p_index = lt_cnt ;
  gt_cnt = old_cnt - lt_cnt - 1;
  p_index_stgadr = (char *) old_itmvec+old_itmvec[p_index].index_stg ;
  strcpy (p_index_stg, p_index_stgadr) ;
  p_data.rec_no = old_itmvec[p_index].data.internal_id.rec_no ;
  p_data.item_offs = old_itmvec[p_index].data.internal_id.item_offs ;

  /*
   * Acquire a new record to become the LT part. Copy the entire current
   * record into it, then collapse both records into their final form.
   */
  result = Idb__BM_InitRecord (file_id, 0, IDBrtIndexNode, &lt_buffer) ;
  lt_recptr = (IDBIndexNodeRecordPtr) lt_buffer->IDB_record ;
  Idb__INX_CopyNodeRecord (lt_recptr, gt_recptr) ;
  Idb__INX_CollapseNodeRecord (lt_recptr, 0, lt_cnt-1) ;
  Idb__INX_CollapseNodeRecord (gt_recptr, p_index+1, p_index+gt_cnt) ;

  /*
   * Both records now have their parent set correctly via the copy operation,
   * since our check on space in the parent guarantees that the parent
   * pointer present in the original buffer will remain the parent after
   * we host the pivot index (unless we create a new root node, which
   * is guaranteed safe anyway. Thus we are done with all changes to these
   * buffers, and can mark them. Then save their record numbers and child
   * list for future operations.
   */
  Idb__BM_MarkModified (lt_buffer) ;
  Idb__BM_MarkModified (gt_buffer) ;

  lt_recno = _IdbBufferRecordNumber (lt_buffer) ;
  gt_recno = _IdbBufferRecordNumber (gt_buffer) ;

  /*
   * Either enter the hoisted entry into the parent, or create
   * a parent (which will be the root record).
   *
   * Otherwise, hoist the entry into the parent (we know there should be room).
   * The parent should be already loaded in its buffer as part of the space
   * check, but a reload is done to make sure buffer turning hasn't interfered.
   */
  if ( !p_recno )
    {
      result = Idb__INX_InitRootNodeRecord
        (file_id, &p_buffer, p_index_stg, p_data, lt_recno, gt_recno) ;
      if ( result != MrmSUCCESS ) return result ;
    }
  else
    {
      result = Idb__BM_GetRecord (file_id, p_recno, &p_buffer) ;
      if ( result != MrmSUCCESS ) return result ;
      result = Idb__INX_EnterNodeIndex
        (file_id, p_buffer, p_index_stg, p_data, lt_recno, gt_recno) ;
      if ( result != MrmSUCCESS ) return result ;
    }

  /*
   * Now all child nodes of the split record must have their parent
   * pointers updated. The gt_buffer children should still have the same
   * parent, but the update will be done to that buffer as well for
   * completeness.
   */
  result = Idb__INX_FixNodeChildren (file_id, lt_recno) ;
  if ( result != MrmSUCCESS ) return result ;
  result = Idb__INX_FixNodeChildren (file_id, gt_recno) ;
  if ( result != MrmSUCCESS ) return result ;

  /*
   * Successfully added.
   */
  return MrmSUCCESS ;

}



/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	Idb__INX_InitLeafRecord initializes a new leaf index record,
 *	resulting in an empty record with the maximum free space available.
 *	It may be immediately used to enter an index item. This routine
 *	is used just once, to create the initial root record. Thereafter,
 *	all leaf records are created by splitting and collapsing existing
 *	records.
 *
 *  FORMAL PARAMETERS:
 *
 *	file_id		Open IDB file
 *	buffer_return	To return pointer to buffer containing new record
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
Idb__INX_InitRootLeafRecord (IDBFile		file_id,
			     IDBRecordBufferPtr	*buffer_return)
{

  /*
   *  Local variables
   */
  Cardinal		result ;	/* function results */
  IDBRecordBufferPtr	bufptr ;	/* leaf record buffer */
  IDBIndexLeafRecordPtr	recptr ;	/* leaf record in buffer */
  IDBIndexLeafHdrPtr	hdrptr ;	/* record header */


  /*
   * Acquire a record
   */
  result = Idb__BM_InitRecord (file_id, 0, IDBrtIndexLeaf, &bufptr) ;
  if ( result != MrmSUCCESS ) return result ;
  recptr = (IDBIndexLeafRecordPtr) bufptr->IDB_record ;
  hdrptr = (IDBIndexLeafHdrPtr) &recptr->leaf_header ;

  /*
   * Initialize the record header
   */
  hdrptr->parent = 0 ;
  hdrptr->index_count = 0 ;
  hdrptr->heap_start = IDBIndexLeafFreeMax ;
  hdrptr->free_bytes = IDBIndexLeafFreeMax ;

  /*
   * Successfully initialized
   */
  Idb__BM_MarkModified (bufptr) ;
  *buffer_return = bufptr ;

  file_id->index_root = hdrptr->header.record_num ;

  return MrmSUCCESS ;

}

/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	Idb__INX_InitNodeRecord initializes a new node index record. It
 *	creates the initial entry in the record, with its index, data pointer,
 *	and pointers to two children records in the B-tree. This entry always
 *	becomes the root of the B-tree, since the only occasion on which
 *	a node record is created in this way is when a new root is needed.
 *
 *  FORMAL PARAMETERS:
 *
 *	file_id		Open IDB file
 *	buffer_return	To return pointer to buffer containing new record
 *	index		Index for single entry in record
 *	data_entry	Data entry pointer for data
 *	lt_record	Record number of B-tree record ordering < the index
 *	gt_record	Record number of B-tree record ordering > the index
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
Idb__INX_InitRootNodeRecord (IDBFile			file_id,
			     IDBRecordBufferPtr		*buffer_return,
			     char			*index,
			     IDBDataHandle		data_entry,
			     IDBRecordNumber		lt_record,
			     IDBRecordNumber		gt_record)
{

  /*
   *  Local variables
   */
  Cardinal		result ;	/* function results */
  IDBRecordBufferPtr	bufptr ;	/* node record buffer */
  IDBIndexNodeRecordPtr	recptr ;	/* node record in buffer */
  IDBIndexNodeHdrPtr	hdrptr ;	/* record header */
  IDBRecordNumber	recno ;		/* this buffer's record number */


  /*
   * Acquire a record
   */
  result = Idb__BM_InitRecord (file_id, 0, IDBrtIndexNode, &bufptr) ;
  if ( result != MrmSUCCESS ) return result ;
  recptr = (IDBIndexNodeRecordPtr) bufptr->IDB_record ;
  hdrptr = (IDBIndexNodeHdrPtr) &recptr->node_header ;

  /*
   * Initialize the record header
   */
  hdrptr->parent = 0 ;
  hdrptr->index_count = 0 ;
  hdrptr->heap_start = IDBIndexNodeFreeMax ;
  hdrptr->free_bytes = IDBIndexNodeFreeMax ;

  /*
   * Enter the initial entry
   */
  result = Idb__INX_EnterNodeIndex
    (file_id, bufptr, index, data_entry, lt_record, gt_record) ;
  if ( result != MrmSUCCESS ) return result ;

  /*
   * Successfully initialized
   */
  Idb__BM_MarkModified (bufptr) ;
  *buffer_return = bufptr ;

  /*
   * Set the parent pointers in the two child entries.
   */
  recno = _IdbBufferRecordNumber (bufptr) ;
  result = Idb__INX_SetParent (file_id, recno, lt_record) ;
  if ( result != MrmSUCCESS ) return result ;
  result = Idb__INX_SetParent (file_id, recno, gt_record) ;
  if ( result != MrmSUCCESS ) return result ;

  /*
   * Root node successfully created. Update file header.
   */
  file_id->index_root = hdrptr->header.record_num ;
  return MrmSUCCESS ;

}



/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	This routines copies one leaf record into another.
 *
 *  FORMAL PARAMETERS:
 *
 *	dst_recptr	pointer to record into which to copy
 *	src_recptr	source record pointer
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

void 
Idb__INX_CopyLeafRecord (IDBIndexLeafRecordPtr	dst_recptr,
			 IDBIndexLeafRecordPtr	src_recptr)
{

  /*
   *  Local variables
   */
  IDBIndexLeafHdrPtr	dst_hdrptr ;	/* destination record header */
  IDBIndexLeafHdrPtr	src_hdrptr ;	/* source record header */
  char			*dst_data ;	/* data part of dest record */
  char			*src_data ;	/* data part of source record */

  /*
   * copy the header, field by field
   */
  dst_hdrptr = (IDBIndexLeafHdrPtr) &dst_recptr->leaf_header ;
  src_hdrptr = (IDBIndexLeafHdrPtr) &src_recptr->leaf_header ;
  dst_hdrptr->parent = src_hdrptr->parent ;
  dst_hdrptr->index_count = src_hdrptr->index_count ;
  dst_hdrptr->heap_start = src_hdrptr->heap_start ;
  dst_hdrptr->free_bytes = src_hdrptr->free_bytes ;

  /*
   * copy the data area in the record
   */
  dst_data = (char *) dst_recptr->index ;
  src_data = (char *) src_recptr->index ;

  UrmBCopy (src_data, dst_data, IDBIndexLeafFreeMax) ;

}

/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	This routines copies one node record into another.
 *
 *  FORMAL PARAMETERS:
 *
 *	dst_recptr	pointer to record into which to copy
 *	src_recptr	source record pointer
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

void 
Idb__INX_CopyNodeRecord (IDBIndexNodeRecordPtr	dst_recptr,
			 IDBIndexNodeRecordPtr	src_recptr)
{

  /*
   *  Local variables
   */
  IDBIndexNodeHdrPtr	dst_hdrptr ;	/* destination record header */
  IDBIndexNodeHdrPtr	src_hdrptr ;	/* source record header */
  char			*dst_data ;	/* data part of dest record */
  char			*src_data ;	/* data part of source record */

  /*
   * copy the header, field by field
   */
  dst_hdrptr = (IDBIndexNodeHdrPtr) &dst_recptr->node_header ;
  src_hdrptr = (IDBIndexNodeHdrPtr) &src_recptr->node_header ;
  dst_hdrptr->parent = src_hdrptr->parent ;
  dst_hdrptr->index_count = src_hdrptr->index_count ;
  dst_hdrptr->heap_start = src_hdrptr->heap_start ;
  dst_hdrptr->free_bytes = src_hdrptr->free_bytes ;

  /*
   * copy the data area in the record
   */
  dst_data = (char *) dst_recptr->index ;
  src_data = (char *) src_recptr->index ;
  UrmBCopy (src_data, dst_data, IDBIndexNodeFreeMax) ;

}

/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	This routine collapses a leaf index record as part of splitting
 *	a record. Collapsing the record truncates the record contents
 *	by removing part of the index vector, re-organizing the string
 *	heap to consume minimal space, resetting the heap parameters,
 *	and resetting the index count.
 *
 *  FORMAL PARAMETERS:
 *
 *	recptr		The leaf index record to collapse
 *	start		First entry in the index vector to be preserved
 *	end		Last entry in the index vector to be preserved
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

void 
Idb__INX_CollapseLeafRecord (IDBIndexLeafRecordPtr	recptr,
			     MrmCount			start,
			     MrmCount			end)
{

  /*
   *  Local variables
   */
  IDBIndexLeafHdrPtr	hdrptr ;	/* record header */
  int			ndx ;		/* loop index */
  char			*temp_heap ;	/* temporary heap */
  char			*cur_heap ;	/* current heap pointer */
  MrmCount		heap_size ;	/* # bytes used in temp heap */
  IDBIndexLeafEntryPtr	srcvec ;	/* source index vector */
  IDBIndexLeafEntryPtr	dstvec ;	/* destination index vector */
  char			*stgbase ;	/* base address for string offsets */
  char			*ndxstg ;	/* current index string */
  MrmCount		stgsiz ;	/* # bytes for current string */
  MrmCount		ndxcnt ;	/* # entries in collapsed record */
  MrmCount		nfree ;		/* # free bytes in collapsed record */
  MrmOffset		heap_start ;	/* new heap start offset */


  /*
   * Allocate a temporary heap (big enough to hold data area). Copy each
   * string which must be preserved into the temporary heap. The heap
   * is allocated top-to-bottom, and the temporary offsets will be
   * made permanent when the temporary heap is copied into the record.
   *
   * Copy the surviving part of the index vector while saving the strings.
   * The new offset is set as part of this operation.
   */
  temp_heap = (char *) XtMalloc (IDBIndexLeafFreeMax) ;
  cur_heap = temp_heap ;
  heap_size = 0 ;

  hdrptr = (IDBIndexLeafHdrPtr) &recptr->leaf_header ;
  srcvec = &recptr->index[start] ;
  dstvec = &recptr->index[0] ;
  stgbase = (char *) recptr->index ;
  ndxcnt = end - start + 1 ;

  for ( ndx=0 ; ndx<ndxcnt ; ndx++ )
    {
      dstvec[ndx].data = srcvec[ndx].data ;
      ndxstg = (char *) stgbase + srcvec[ndx].index_stg ;
      strcpy (cur_heap, ndxstg) ;
      dstvec[ndx].index_stg = (MrmOffset) (cur_heap - temp_heap) ;
      stgsiz = strlen(cur_heap) + 1 ;
      stgsiz = _FULLWORD(stgsiz);
      cur_heap += stgsiz ;
      heap_size += stgsiz ;
    }

  /*
   * Compute offsets and sizes, and copy heap into record. Then adjust the
   * offset to allow for the free space.
   */
  hdrptr->index_count = ndxcnt ;
  heap_start = IDBIndexLeafFreeMax - heap_size ;
  hdrptr->heap_start = heap_start ;
  nfree = IDBIndexLeafFreeMax - heap_size - ndxcnt*IDBIndexLeafEntrySize ;
  hdrptr->free_bytes = nfree ;

  UrmBCopy (temp_heap, &stgbase[heap_start], heap_size) ;
  for ( ndx=0 ; ndx<ndxcnt ; ndx++ )
    dstvec[ndx].index_stg += heap_start ;

  XtFree (temp_heap) ;

}


/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	This routine collapses a node index record as part of splitting
 *	a record. Collapsing the record truncates the record contents
 *	by removing part of the index vector, re-organizing the string
 *	heap to consume minimal space, resetting the heap parameters,
 *	and resetting the index count.
 *
 *	The record vector is preserved by moving entires start to end+1.
 *	For both records being collapsed, these entries are the correct
 *	ones to associate with the collapsed record.
 *
 *  FORMAL PARAMETERS:
 *
 *	recptr		The node index record to collapse
 *	start		First entry in the index vector to be preserved
 *	end		Last entry in the index vector to be preserved
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

void 
Idb__INX_CollapseNodeRecord (IDBIndexNodeRecordPtr	recptr,
			     MrmCount			start,
			     MrmCount			end)
{

  /*
   *  Local variables
   */
  IDBIndexNodeHdrPtr	hdrptr ;	/* record header */
  int			ndx ;		/* loop index */
  char			*temp_heap ;	/* temporary heap */
  char			*cur_heap ;	/* current heap pointer */
  MrmCount		heap_size ;	/* # bytes used in temp heap */
  IDBIndexNodeEntryPtr	srcvec ;	/* source index vector */
  IDBIndexNodeEntryPtr	dstvec ;	/* destination index vector */
  char			*stgbase ;	/* base address for string offsets */
  char			*ndxstg ;	/* current index string */
  MrmCount		stgsiz ;	/* # bytes for current string */
  MrmCount		ndxcnt ;	/* # entries in collapsed record */
  MrmCount		nfree ;		/* # free bytes in collapsed record */
  MrmOffset		heap_start ;	/* new heap start offset */


  /*
   * Allocate a temporary heap (big enough to hold data area). Copy each
   * string which must be preserved into the temporary heap. The heap
   * is allocated top-to-bottom, and the temporary offsets will be
   * made permanent when the temporary heap is copied into the record.
   *
   * Copy the surviving part of the index vector while saving the strings.
   * The new offset is set as part of this operation.
   */
  temp_heap = (char *) XtMalloc (IDBIndexNodeFreeMax) ;
  cur_heap = temp_heap ;
  heap_size = 0 ;

  hdrptr = (IDBIndexNodeHdrPtr) &recptr->node_header ;
  srcvec = &recptr->index[start] ;
  dstvec = &recptr->index[0] ;
  stgbase = (char *) recptr->index ;
  ndxcnt = end - start + 1 ;

  for ( ndx=0 ; ndx<ndxcnt ; ndx++ )
    {
      dstvec[ndx].data = srcvec[ndx].data ;
      dstvec[ndx].LT_record = srcvec[ndx].LT_record ;
      dstvec[ndx].GT_record = srcvec[ndx].GT_record ;
      ndxstg = (char *) stgbase + srcvec[ndx].index_stg ;
      strcpy (cur_heap, ndxstg) ;
      dstvec[ndx].index_stg = (MrmOffset) (cur_heap - temp_heap) ;
      stgsiz = strlen(cur_heap) + 1 ;
      stgsiz = _FULLWORD(stgsiz);
      cur_heap += stgsiz ;
      heap_size += stgsiz ;
    }

  /*
   * Compute offsets and sizes, and copy heap into record. Then adjust the
   * offset to allow for the free space.
   */
  hdrptr->index_count = ndxcnt ;
  heap_start = IDBIndexNodeFreeMax - heap_size ;
  hdrptr->heap_start = heap_start ;
  nfree = IDBIndexNodeFreeMax - heap_size - ndxcnt*IDBIndexNodeEntrySize ;
  hdrptr->free_bytes = nfree ;

  UrmBCopy (temp_heap, &stgbase[heap_start], heap_size) ;
  for ( ndx=0 ; ndx<ndxcnt ; ndx++ )
    dstvec[ndx].index_stg += heap_start ;

  XtFree (temp_heap) ;

}


/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	This routine confirms that there is enough space in an index
 *	node for a new entry. If there is not, it splits the entry
 *	and tells the caller to retry.
 *
 *  FORMAL PARAMETERS:
 *
 *	file_id		Open IDB file
 *	buffer		Buffer containing the node record to be checked.
 *
 *  IMPLICIT INPUTS:
 *
 *  IMPLICIT OUTPUTS:
 *
 *  FUNCTION VALUE:
 *
 *	MrmSUCCESS	There is enough space
 *	MrmINDEX_RETRY	Node was split, caller must retry
 *	MrmFAILURE	Some other failure
 *
 *  SIDE EFFECTS:
 *
 *--
 */

Cardinal 
Idb__INX_ConfirmNodeSpace (IDBFile		file_id,
			   IDBRecordBufferPtr	buffer)
{

  /*
   *  Local variables
   */
  Cardinal		result ;	/* function results */
  IDBIndexNodeRecordPtr	recptr ;	/* node record in buffer */
  IDBIndexNodeHdrPtr	hdrptr ;	/* record header */


  /*
   * Initialize pointers into the record
   */
  recptr = (IDBIndexNodeRecordPtr) buffer->IDB_record ;
  hdrptr = (IDBIndexNodeHdrPtr) &recptr->node_header ;

  /*
   * Check the size. If there is enough, OK. Else split this record and
   * return a retry.
   */
  if ( hdrptr->free_bytes >= IDBIndexNodeEntryMax ) return MrmSUCCESS ;

  result = Idb__INX_SplitNodeRecord (file_id, buffer) ;
  if ( result == MrmSUCCESS ) result = MrmINDEX_RETRY ;

  return result ;

}


/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	This routine sets the parent pointer in a child record
 *
 *  FORMAL PARAMETERS:
 *
 *	file_id		Open IDB file
 *	parent_record	Parent record number
 *	child_record	Child record number
 *
 *  IMPLICIT INPUTS:
 *
 *  IMPLICIT OUTPUTS:
 *
 *  FUNCTION VALUE:
 *
 *	MrmSUCCESS	operation succeeded
 *	MrmFAILURE	some failure occurred
 *
 *  SIDE EFFECTS:
 *
 *--
 */

Cardinal 
Idb__INX_SetParent (IDBFile		file_id,
		    IDBRecordNumber	parent_record,
		    IDBRecordNumber	child_record)
{

  /*
   *  Local variables
   */
  Cardinal		result ;	/* function results */
  IDBRecordBufferPtr	buffer ;	/* buffer for child record */
  IDBIndexLeafRecordPtr	leafrec ;	/* index leaf record */
  IDBIndexLeafHdrPtr	leafhdr ;	/* index leaf header */
  IDBIndexNodeRecordPtr	noderec ;	/* index node record */
  IDBIndexNodeHdrPtr	nodehdr ;	/* index node header */


  /*
   * Get the child record
   */
  result = Idb__BM_GetRecord (file_id, child_record, &buffer) ;
  if ( result != MrmSUCCESS ) return result ;
  if ( ! Idb__INX_ValidRecord(buffer) )
    return Urm__UT_Error ("Idb__INX_SetParent", _MrmMMsg_0010,
			  file_id, NULL, MrmBAD_RECORD) ;

  /*
   * Set up pointers and set parent based on record type
   */
  switch ( _IdbBufferRecordType (buffer) )
    {
    case IDBrtIndexLeaf:
      leafrec = (IDBIndexLeafRecordPtr) buffer->IDB_record ;
      leafhdr = (IDBIndexLeafHdrPtr) &leafrec->leaf_header ;
      if ( leafhdr->parent != parent_record )
	{
	  leafhdr->parent = parent_record ;
	  Idb__BM_MarkModified (buffer) ;
	}
      return MrmSUCCESS ;
    case IDBrtIndexNode:
      noderec = (IDBIndexNodeRecordPtr) buffer->IDB_record ;
      nodehdr = (IDBIndexNodeHdrPtr) &noderec->node_header ;
      if ( nodehdr->parent != parent_record )
	{
	  nodehdr->parent = parent_record ;
	  Idb__BM_MarkModified (buffer) ;
	}
      return MrmSUCCESS ;
    default:
      return MrmBAD_RECORD ;
    }

}



/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	This routine guarantees that the children of a node record have
 *	a correct parent pointer.
 *
 *  FORMAL PARAMETERS:
 *
 *	file_id		Open IDB file
 *	p_record	Record number of parent record
 *
 *  IMPLICIT INPUTS:
 *
 *  IMPLICIT OUTPUTS:
 *
 *  FUNCTION VALUE:
 *
 *  SIDE EFFECTS:
 *
 *	This routine uses MarkActivity to guarantee that the parent
 *	record remains in memory.
 *
 *--
 */

Cardinal 
Idb__INX_FixNodeChildren (IDBFile		file_id,
			  IDBRecordNumber	p_record)
{

  /*
   *  Local variables
   */
  Cardinal		result ;	/* function results */
  IDBRecordBufferPtr	buffer ;	/* parent node buffer */
  IDBIndexNodeRecordPtr	recptr ;	/* parent node record in buffer */
  IDBIndexNodeHdrPtr	hdrptr ;	/* parent record header */
  int			ndx ;		/* loop index */


  /*
   * Get the parent record
   */
  result = Idb__BM_GetRecord (file_id, p_record, &buffer) ;
  if ( result != MrmSUCCESS ) return result ;
  if ( ! Idb__INX_ValidNode(buffer) )
    return Urm__UT_Error ("Idb__INX_FixNodeChildren", _MrmMMsg_0010,
			  file_id, NULL, MrmBAD_RECORD) ;

  /*
   * Bind pointers into the record, and set each child's parent pointer
   */
  recptr = (IDBIndexNodeRecordPtr) buffer->IDB_record ;
  hdrptr = (IDBIndexNodeHdrPtr) &recptr->node_header ;

  for ( ndx=0 ; ndx<hdrptr->index_count ; ndx++ )
    {
      result = Idb__INX_SetParent (file_id, p_record,
				   recptr->index[ndx].LT_record) ;
      if ( result != MrmSUCCESS ) return result ;
      result = Idb__INX_SetParent (file_id, p_record,
				   recptr->index[ndx].GT_record) ;
      if ( result != MrmSUCCESS ) return result ;
      Idb__BM_MarkActivity (buffer) ;
    }

  /*
   * Successfully modified
   */
  return MrmSUCCESS ;

}


