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

#ifndef idb_h
#define	idb_h	1

/*
 * This file defines the constants and structs required by the URM
 * Indexed Data Base facility (IDB). The order of definition is:
 *	o literals
 *	o primitive typedefs
 *	o typedefs for file records
 *	o typedefs for in-memory structures
 */


/*
 * IDB literals
 */


/*
 * Primitive typedefs
 */


/*
 * A data pointer. Consists of a record number (of a data record) and an
 * offset in the record to the data entry.
 */
typedef union {
	unsigned		pointer ;	/* the pointer as a value */
	struct {
	  IDBRecordNumber	rec_no B32 ;	/* record number */
	  MrmOffset		item_offs B32 ;	/* offset of data entry in
						   record. Must be a
						   IDB*DataEntry struct */
	}			internal_id ;	/* 2 fields internally */
} IDBDataPointer ;

/*
 * A data pointer as a passed-by-value reference
 */
#if 0
typedef unsigned		IDBDataHandle ;
#endif

typedef struct {
  IDBRecordNumber	rec_no ;		  /* record number */
  MrmOffset		item_offs ;		  /* offset of data entry in
						     record. Must be a
						     IDB*DataEntry struct */
} IDBDataHandle ;

/*
 * A data item in a data record.
 */

/*
 * entry types distinguish simple and overflow records
 */
#define IDBdrSimple	1
#define	IDBdrOverflow	2


/*
 * Common header for both simple and overflow data entries
 */
#define	IDBDataEntryValid	222857390
typedef struct {
	unsigned	validation;		/* validation code =
						   IDBDataEntryValid */
	MrmCode		entry_type;		/* IDBdrSimple, IDBdrOverflow */
	MrmCode		resource_group;		/* resource group */
	MrmCode		resource_type;		/* resource type */
	MrmCode		access;			/* URMaPrivate or URMaPublic */
	MrmCode		lock;			/* locking code */
	MrmSize		entry_size;		/* number of data bytes */
	MrmOffset	prev_entry;		/* Offset of previous entry in
						   this data record.  */
} IDBDataEntryHdr, *IDBDataEntryHdrPtr ;

#define SwapIDBDataEntryHdr(deh) { \
        swap4bytes(deh->validation); \
        swap2bytes(deh->entry_type) ; \
        swap2bytes(deh->resource_group) ; \
        swap2bytes(deh->resource_type) ; \
        swap2bytes(deh->access) ; \
        swap2bytes(deh->lock) ; \
        swap2bytes(deh->entry_size) ; \
        swap2bytes(deh->prev_entry) ; \
}

/*
 * A simple data entry
 */
typedef struct {
	IDBDataEntryHdr	header ;		/* common header, entry_type =
						   IDBdrSimple */
	char		data[1] ;		/* First byte in data */
} IDBSimpleData, *IDBSimpleDataPtr ;


/*
 * An overflow data entry. These are used for all data entries which are
 * too big to fit in a single record. These are broken up into 2 or more
 * overflow entries, whose contents are concatenated to produce the
 * actual data entry.
 */
typedef struct {
	IDBDataEntryHdr	header ;		/* common header, entry_type =
						   IDBdrOverflow */
	MrmSize		segment_size;		/* number of data bytes in
						   this segment */
	MrmCount	segment_count;		/* number of records needed
						   to store entry */
	MrmCount	segment_num;		/* this segment's number */
	IDBDataPointer	next_segment ;		/* next segment */
	char		data[1] ;		/* first data byte */
} IDBOverflowData, *IDBOverflowDataPtr ;

#define SwapIDBOverflowData(doh) { \
        swap2bytes(doh->segment_size) ; \
        swap2bytes(doh->segment_count) ; \
        swap2bytes(doh->segment_num) ; \
        swap2bytes(doh->next_segment.internal_id.rec_no) ; \
        swap2bytes(doh->next_segment.internal_id.item_offs) ; \
}

/*
 * Header sizes for data entries
 */
#define	IDBSimpleDataHdrSize	(sizeof(IDBSimpleData) - sizeof(char))
#define	IDBOverflowDataHdrSize	(sizeof(IDBOverflowData) - sizeof(char))



/*
 * File record definitions
 *
 * IDB uses one size of fixed-length record for all its file records.
 * The size of this record is determined as a reasonable common size
 * which gives efficient disk access on all the file systems on which
 * IDB runs.
 *
 * All records are accessed by record number. These increase monotonically
 * from the lowest valid record number, which is maintained in the file
 * header.
 *
 * IDB defines the following types of records:
 *	Header		- one per file
 *	Index leaf	- contains leaf nodes of the B-tree index
 *	Index node	- contains non-leaf nodes of the B-tree index
 *	RID map		- contains pointers which map IDB resourc IDs to
 *			  data block pointers
 *	Data		- Contains data blocks
 *
 * All records have a common record header which gives the record number
 * and record type. This header and the information for each record type
 * are defined below.
 */

/*
 * Number of bytes in a low-level file block = # bytes in an IDB file record
 */
#define	IDBRecordSize	4096

/*
 * IDB record header
 */
#define	IDBRecordHeaderValid	310144882
typedef struct {
	unsigned	validation;	/* validation code =
					   IDBRecordHeaderValid */
	MrmType		record_type;  	/* record type */
	IDBRecordNumber	record_num;	/* IDB record number */
} IDBRecordHeader, *IDBRecordHeaderPtr ;


/*
 * Dummy IDB record.
 */
typedef struct {
	IDBRecordHeader	header ;
	char		dummy[IDBRecordSize-sizeof(IDBRecordHeader)] ;
} IDBDummyRecord, *IDBDummyRecordPtr ;



/*
 * IDB header record definition
 *
 * There is one header record per IDB file. It is the only record in the
 * which must be at a fixed location; its record number is
 * IDBHeaderRecordNumber.
 *
 * The header contains the following:
 *	o Information about how the file was created
 *	o A pointer to the root node record for the B-tree index
 *	o The next available resourc id (RID)
 *	o An end-of-file pointer (record number of last record in file)
 *	o Counters of the number of index and RID entries in the file
 *	o Counter of the different types of records in the file
 *	o The remainder of the record is used as the initial space in
 *	  which to allocate a small RID map and data entries. This
 *	  enables relatively small files to be created when a small
 *	  amount of information will be stored, as the minimal file
 *	  need contain only a header and (probably) and index leaf record
 *	  rather than also requiring an RID map record and a data record.
 */

/*
 * IDB file header record header
 */
typedef struct {
	IDBRecordHeader	header ;		/* record_type =
						   IDBrtHeader,
						   record_num =
						   IDBrtHeader, */
	char		db_version[IDBhsVersion1] ;
						/* database version */
	char		creator[IDBhsCreator1] ; /* creator id */
	char		creator_version[IDBhsVersion1] ;
						/* creator version */
	char		creation_date[IDBhsDate1] ;
						/* creation date */
	char		module[IDBhsModule1] ;	/* module id */
	char		module_version[IDBhsVersion1] ;
						/* module version */
	IDBRecordNumber	index_root ;		/* index root pointer */
	MrmCount	num_indexed ;		/* # entries in index */
	MrmCount	num_RID ;		/* # RID entries in file */
	IDBridDesc	next_RID ;		/* next available RID. */
	IDBRecordNumber	last_record ;		/* last record used in file */
	IDBRecordNumber	last_data_record ;	/* last data record in file */
	MrmCount	group_counts[URMgVecSize] ;
						/* vector of record counts
						   by resource group */
	MrmCount	rt_counts[IDBrtVecSize] ;
						/* vector of record counts by
						   record type (statistics) */
} IDBHeaderHdr, *IDBHeaderHdrPtr ;


/*
 * IDB file header record
 *
 * Max number of entries in RID pointer vector
 */
#define	IDBHeaderRIDMax		20
typedef struct {
	IDBHeaderHdr	header_hdr ;	/* header part */
	IDBDataPointer	RID_pointers[IDBHeaderRIDMax];
					/* RID pointer vector */
	MrmCount	num_entry;	/* number of entries in record */
	MrmOffset	last_entry;	/* last entry in page */
	MrmOffset	free_ptr;	/* offset of first free byte.
					   initial value = 0 */
	MrmCount	free_count;	/* number of free bytes. Initial
					   value = IDBHeaderFreeMax */
	char		data[1];	/* First available byte for data */
} IDBHeaderRecord, *IDBHeaderRecordPtr ;


/*
 * Free space in header record, max number of bytes available for data.
 */
#define	IDBHeaderFreeMax (IDBRecordSize-sizeof(IDBHeaderRecord)+sizeof(char))


/*
 * Record number of the header record
 */
#define	IDBHeaderRecordNumber	1



/*
 * Index leaf record definition
 *
 * IDB provides two mutually exclusive mechanisms for accessing data.
 * The first is an index system based on ASCII keys (indexes). This
 * uses a B-tree index as its lookup mechanism. The second mechanism
 * is a resource id mechanism offering potentially faster access, as
 * described below.
 *
 * As usual in B-tree systems, the B-tree index has two kinds of nodes:
 * leaf nodes, which contain only data pointers, and B-tree nodes,
 * which contain pointers to other B-tree records. The following describes
 * leaf records in the B-tree index.
 *
 * The root node of the index is a leaf record if there is exactly 1
 * record in the index, and is a tree node otherwise.
 *
 * A leaf record consists of a sorted vector of index entries, ordered
 * in increasing alphabetical order by the index value. The vector
 * contains fixed-length entries, with the key strings being allocated
 * at the bottom of the record. The vector grows down from the top and
 * the string heap up from the bottom as entries are added, until the
 * record fills. 
 */

/*
 * An entry in the index vector
 */
typedef struct {
	MrmOffset	index_stg;	/* offset of the index string in
					   the record. Nul-terminated. */
	IDBDataPointer	data ;		/* pointer to the data entry */
} IDBIndexLeafEntry, *IDBIndexLeafEntryPtr ;

/*
 * Size of an index vector entry
 */
#define	IDBIndexLeafEntrySize	sizeof(IDBIndexLeafEntry)

/*
 * Max length of an index string (terminating nul not included)
 */
#define	IDBMaxIndexLength	URMMaxIndexLen
#define	IDBMaxIndexLength1	(IDBMaxIndexLength + 1)


/*
 * Leaf record header
 */
typedef struct {
	IDBRecordHeader	header ;	/* record_type = IDBrtIndexLeaf */
	IDBRecordNumber	parent;		/* B-tree parent record */
	MrmCount	index_count;	/* Number of index entries in record */
	MrmOffset	heap_start;	/* Offset of first byte in use for
					   index string heap storage. Offset
					   is from .index[0] */
	MrmCount	free_bytes;	/* Number of free bytes in record */
} IDBIndexLeafHdr, *IDBIndexLeafHdrPtr ;


/*
 * The leaf record
 */
typedef struct {
	IDBIndexLeafHdr	leaf_header ;	/* header part of record. Initial
					   values: heap_start =
					   free_bytes = IDBIndexLeafFreeMax */
	IDBIndexLeafEntry index[1] ;	/* First entry in index vector. There
					   are index_count entries. */
} IDBIndexLeafRecord, *IDBIndexLeafRecordPtr ;


/*
 * Max number of free bytes in leaf index record (0 entries)
 */
#define	IDBIndexLeafFreeMax	(IDBRecordSize - sizeof(IDBIndexLeafHdr))



/*
 * Index B-tree node record definition
 *
 * A B-tree node record is similar to a leaf entry, except that each node
 * contains a pair of pointers to other nodes in the tree; one to a LT
 * node, all of whose entries order < the entry's index; one to a GT node,
 * all of whose entries order > the entry's index. The GT pointer of node
 * m = the LT pointer of node m+1. Thus any node record containing n nodes
 * points to n+1 tree nodes. Allocation of heap for index strings is as in
 * a leaf record. All entries also locate a data entry.
 */

/*
 * An entry in the index vector
 */
typedef struct {
	MrmOffset	index_stg;	/* offset of the index string in
					   the record. Nul-terminated. */
	IDBDataPointer	data ;		/* pointer to the data entry */
	IDBRecordNumber	LT_record;	/* pointer to LT record */
	IDBRecordNumber	GT_record;	/* pointer to GT record */
} IDBIndexNodeEntry, *IDBIndexNodeEntryPtr ;

/*
 * Size of an index vector entry
 */
#define	IDBIndexNodeEntrySize	sizeof(IDBIndexNodeEntry)

/*
 * Node record header
 */
typedef struct {
	IDBRecordHeader	header ;	/* record_type = IDBrtIndexNode */
	IDBRecordNumber	parent;		/* B-tree parent record */
	MrmCount	index_count;	/* Number of index entries in record */
	MrmOffset	heap_start;	/* Offset of first byte in use for
					   index string heap storage. Offset
					   is from .index[0] */
	MrmCount	free_bytes;	/* Number of free bytes in record */
} IDBIndexNodeHdr, *IDBIndexNodeHdrPtr ;


/*
 * The B-tree node entry
 */
typedef struct {
	IDBIndexNodeHdr	node_header ;	/* header part of record. Initial
					   values: heap_start =
					   free_bytes = IDBIndexNodeFreeMax */
	IDBIndexNodeEntry index[1] ;	/* First entry in index vector. There
					   are index_count entries. */
} IDBIndexNodeRecord, *IDBIndexNodeRecordPtr ;


/*
 * Max number of free bytes in node index record (0 entries)
 */
#define	IDBIndexNodeFreeMax	(IDBRecordSize - sizeof(IDBIndexNodeHdr))

/*
 * Max number of bytes consumed by a new entry
 */
#define	IDBIndexNodeEntryMax	(IDBMaxIndexLength1 + IDBIndexNodeEntrySize \
				+ sizeof(IDBRecordNumber))



/*
 * Resource ID record
 *
 * RID records locate the data block associated with a given resource ID.
 * Looking up a data block accessed by RID is a two-step process:
 *	1. The index map in the file header (maintained in-memory for open
 *	   files) is used to locate the correct resource ID record. The
 *	   map_index field in a RID descriptor accesses this record via
 *	   the RID_maps vector in the header.
 *	2. The Resource ID record supplies the data block pointer. The
 *	   data block pointer is found in the resource_ptr vector using the
 *	   res_index field of the RID descriptor.
 *
 * Aside from the standard record header, an RID record contains only
 * a vector of data block pointer.
 */

/*
 * The RID map record header
 */
typedef struct {
	IDBRecordHeader	header ;	/* record type = IDBrdRIDMap */
} IDBridMapHdr, *IDBridMapHdrPtr ;

/*
 * The RID map record is a vector of data pointers
 */
typedef struct {
	IDBridMapHdr	map_header ;	/* Header part of record */
	IDBDataPointer	pointers[1] ;	/* First pointer in vector. */
} IDBridMapRecord, *IDBridMapRecordPtr ;

/*
 * Max number of free bytes, number of vector entries in RID record
 */
#define	IDBridFreeMax		(IDBRecordSize - sizeof(IDBridMapHdr))
#define	IDBridPtrVecMax		(IDBridFreeMax / sizeof(IDBDataPointer))



/*
 * Data record
 *
 * Data records hold data entries. The record maintains a free space
 * pointer, but otherwise contains little information. IDB attempts
 * to avoid splitting data entries into segments, and is thus willing
 * to waste some space in a data record if a data item will fit in
 * a new record.
 *
 * All the data entries in a record are chained via the prev_entry offset
 * field. This is for the convenience of the dump routines. The first
 * entry is always at offset 0 (= initial value of free ptr).
 */

/*
 * The data record header
 */
typedef struct {
	IDBRecordHeader	header ;	/* record = IDBrtData */
	MrmCount	num_entry;	/* number of entries in record */
	MrmOffset	last_entry;	/* last entry in page */
	MrmOffset	free_ptr;	/* offset of first free byte.
					   initial value = 0 */
	MrmCount	free_count;	/* number of free bytes. Initial
					   value = IDBDataFreeMax */
} IDBDataHdr, *IDBDataHdrPtr ;


/*
 * The data record
 */
typedef struct {
	IDBDataHdr	data_header ;	/* Header part of record. Initial
					   values: free_ptr = &.data[0],
					   free_count = IDBDataFreeMax */
	char		data[1] ;	/* First available byte for data */
} IDBDataRecord, *IDBDataRecordPtr ;


/*
 * Max number of free bytes in data record
 */
#define	IDBDataFreeMax		(IDBRecordSize - sizeof(IDBDataHdr))


/*
 * Max number of bytes of data which can be stored as simple or overflow
 * entries in a data record.
 */
#define	IDBDataSimpleMax	(IDBDataFreeMax - IDBSimpleDataHdrSize)
#define	IDBDataOverflowMax	(IDBDataFreeMax - IDBOverflowDataHdrSize)



/*
 * In-memory definitions
 *
 * The following structs are used to save runtime informtion used
 * by IDB. These definitions cover file management and buffer management.
 */

/*
 * The IDBOpenFile/IDBFile struct is defined in Mrm.h
 */

/*
 * Buffer management
 *
 * IDB manages a pool of record buffers which is shared among all open
 * IDB files. All access to these buffers is via the IDB buffer manager,
 * which must be used in all requests to access these buffers for
 * either reading or writing.
 */

/*
 * IDB buffer header
 */
#define	IDBRecordBufferValid	327711354
typedef struct {
	unsigned	validation ;	/* validation code =
					   IDBRecordBufferValid */
	long int	activity ;	/* activity count to determine
					   moldiest buffer */
	IDBFile		cur_file ;	/* file which currently owns buffer */
	MrmCode		access ;	/* URMReadAccess or URMWriteAccess */
	MrmCode		modified ;	/* t if buffer has been modified */
	IDBDummyRecord	*IDB_record ;	/* record in buffer */
} IDBRecordBuffer, *IDBRecordBufferPtr ;


/*
 * Macro which validates a buffer
 */
#define	Idb__BM_Valid(bufptr) ((bufptr)->validation==IDBRecordBufferValid)

/*
 * Macros which return the record type, number of the record in a buffer
 */
#define _IdbBufferRecordType(bufptr) ((bufptr)->IDB_record->header.record_type)
#define _IdbBufferRecordNumber(bufptr) ((bufptr)->IDB_record->header.record_num)

#endif /* idb_h */
/* DON'T ADD STUFF AFTER THIS #endif */
