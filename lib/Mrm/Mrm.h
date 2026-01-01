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
#ifndef Mrm_H
#define Mrm_H

#define	urm_h	1
#include <Mrm/MrmPublic.h>
#include <Xm/Xm.h>			/* XmString definition */
#include <Xm/XmosP.h>

/*
 * This is the internal file used throughout URM for literal and
 * struct definitions. It defines everything not found in public
 * files.
 */

#ifndef MIN
#define MIN(x,y)	((x) < (y) ? (x) : (y))
#endif
#ifndef MAX
#define MAX(x,y)	((x) > (y) ? (x) : (y))
#endif

/* Backward-compat */
#define XmConst const

/*
 * The following macro is used to round the size of memory allocation requests
 * up to eight bytes. This is done to help satisfy alignment
 * request for MIPS, CRAY, Alpah, HP9000 (for doubles) and similar machines.
 */
#define _sl sizeof(long)
#define _slm (sizeof(long) - 1)
#define _FULLWORD(exp) (_sl * (((exp) + _slm) / _sl))	/* (4*(((exp)+3)/4)) */

/*
 *  Swap the byte order of 4- and 2- byte quantities.
 *  "tp +=" lines are needed on Cray (CARD32 is actually 64 bits).
 */

#define swap4bytes(l) {\
        CARD32 t = (CARD32) l;\
        char n, *tp = (char *) &t;\
	tp += sizeof(CARD32) - 4;\
	n = tp[0]; tp[0] = tp[3]; tp[3] = n;\
	n = tp[1]; tp[1] = tp[2]; tp[2] = n;\
        l = t;\
}

#define swap2bytes(s) {\
        CARD16 t = (CARD16) s;\
        char n, *tp = (char *) &t;\
	tp += sizeof(CARD16) - 2;\
	n = tp[0]; tp[0] = tp[1]; tp[1] = n;\
        s = t;\
}

#define swapbytes(v) {\
        if (sizeof(v) == 2) \
	  swap2bytes(v) \
	else if (sizeof(v) == 4) \
	  swap4bytes(v); \
}

#define swapdouble(d) {\
        double dt = (double) d;\
        unsigned m, *dp = (unsigned *) &dt;\
	swap4bytes( dp[0] );\
	swap4bytes( dp[1] );\
        m = dp[0]; dp[0] = dp[1]; dp[1] = m;\
	d = dt;\
}


/*
 * When a resource record is referenced but not yet fetched, logical or
 * the convert type with this value as a flag.
 */
#define MrmResourceUnswapped (unsigned char)128


/*
 * URM-wide constants
 */

#define	URMversion		"URM 1.2"
#define URM1_1version		"URM 1.1"

/*
 * The indexes of the self-encoding compression tables in a UID file. They
 * intentionally include characters which are illegal in UIL identifiers.
 */
#define	UilMrmClassTableIndex		">ClassTable"
#define UilMrmResourceTableIndex	">ResourceTable"


/*
 * Resource classes and types
 */
#define	URMgNul		0
#define	URMgWidget	1
#define	URMgLiteral	2		/* types in MrmRtype... */
#define	URMgResourceSet	3		/* type in URMrs... */
/*			4		unused */
#define	URMgMessage	5

#define URMgMin		1		/* lowest legal group value */
#define	URMgMax		5		/* highest legal group value */
#define	URMgVecSize	URMgMax+1	/* To size any vectors */

/*
 * The null resource type
 */
#define	URMtNul		0


/*
 * Access types
 */
#define	URMaPublic	1
#define	URMaPrivate	2

/*
 * Index types. Immediate is included for the convenience of the UIL compiler
 */
#define	URMrIndex	1
#define	URMrRID		2
#define	URMrImmediate	3

/*
 * Maximum number of characters in an index
 */
#define	URMMaxIndexLen	31
#define	URMMaxIndexLen1	(URMMaxIndexLen + 1)	/* to size vectors */


/*
 * File open access types
 */
#define	URMReadAccess	1
#define	URMWriteAccess	2

/*
 * Standard date string length
 */
#define	URMhsDate		29		/* any date string */


/*
 * Types of resource sets.
 */
#define	URMrsInterfaceModule	1	/* Interface module */


/*
 * Values for the manage flag in memory only -- the manage field in RGMChildDesc
 * is just a Boolean.
 */
typedef enum {
MrmManageUnmanage,
MrmManageManage,
MrmManageDefault
} MrmManageFlag;


/*
 * Urm typedefs
 */

/*
 * The maximum number of arguments allowed in an arglist or children in
 * a child list. A number intended to catch gross errors.
 */
#define	RGMListSizeMax		32767


/*
 * URM pointer list
 *
 * The following structure is inspired by resource contexts, but is used
 * to store lists of pointers. Its supporting routines automatically manage
 * space in the same the context routines do. Since this is used only
 * internally, no validation is provided.
 */

typedef struct {
	int		num_slots ;	/* number of available pointer slots */
	int		num_ptrs ;	/* number of slots actually used */
	XtPointer	*ptr_vec ;	/* pointer slot vector */
} URMPointerList, *URMPointerListPtr ;


/*
 * Accessors for the list or a pointer in the list
 */
#define	UrmPlistPtrList(list) ((list)->ptr_vec)
#define	UrmPlistNum(list) ((list)->num_ptrs)
#define	UrmPlistPtrN(list,ndx) ((list)->ptr_vec[ndx])



/*
 * URM Resource context
 *
 * The following context structure is provided by URM to cover all memory
 * management of in-memory resources. This mechanism provides:
 *	o improved memory management efficiency through buffer re-use
 *	o collection of resource attributes in one place
 *	o support for non-standard allocation routines
 */
#define	URMResourceContextValid	232570204
typedef struct {
	unsigned	validation ;	/* validation code =
					   URMResourceContextValid */
	char		*data_buffer ;	/* pointer to data buffer in memory */
	MrmSize		buffer_size ;	/* number of bytes in buffer */
	MrmSize		resource_size ;	/* number of bytes used by resource */
	MrmGroup	group ;		/* resource group */
	MrmType		type ;		/* resource type */
	MrmCode		access ;	/* URMaPrivate or URMaPublic */
	MrmCode		lock ;		/* resource locking code */
	char		*(*alloc_func)(Cardinal);	/* memory allocation func */
	void		(*free_func)(void *);	/* deallocation func */
	MrmFlag		byte_swapped ;	/* resource is byte_swapped */
} URMResourceContext, *URMResourceContextPtr ;


/*
 * Validation macro, returns TRUE/FALSE
 */
#define	UrmRCValid(context) ((context)->validation==URMResourceContextValid)


/*
 * The following are accessor macros for a resource context
 */
#define	UrmRCBuffer(context)	((context)->data_buffer)
#define UrmRCBufSize(context)	((context)->buffer_size)
#define UrmRCSize(context)	((context)->resource_size)
#define UrmRCGroup(context)	((context)->group)
#define UrmRCType(context)	((context)->type)
#define UrmRCAccess(context)	((context)->access)
#define UrmRCLock(context)	((context)->lock)
#define UrmRCByteSwap(context)	((context)->byte_swapped)


/*
 * The following are modifiers for a resource context
 */
#define	UrmRCSetBuffer(context,bufadr)    {(context)->data_buffer=(bufadr);}
#define	UrmRCSetSize(context,sizval)      {(context)->resource_size=(sizval);}
#define UrmRCSetGroup(context,grpval)     {(context)->group=(grpval);}
#define UrmRCSetType(context,typval)      {(context)->type=(typval);}
#define UrmRCSetAccess(context,accval) 	  {(context)->access=(accval);}
#define UrmRCSetLock(context,lckval)	  {(context)->lock=(lckval);}
#define UrmRCSetByteSwap(context,swapval) {(context)->byte_swapped=(swapval);}


/*
 * Error reporting states
 */
#define	URMRCErrorDefault	1	/* default - report on line */
#define	URMRCErrorSave		2	/* save error, don't report */

/*
 * Error reporting destinations
 */
#define	URMErrOutMemory	1	/* Save error messages in memory, no print */
#define	URMErrOutStdout	2	/* print error messages to stdout */



/*
 * IDB definitions in common use. The principal one is IDBFile; others
 * are included to make that definition valid.
 */

/*
 * Record type definitions - IDBrt...
 */
#define	IDBrtMin		1
#define	IDBrtHeader		1
#define	IDBrtIndexLeaf		2
#define	IDBrtIndexNode		3
#define	IDBrtRIDMap		4
#define	IDBrtData		5
#define	IDBrtMax		5
#define	IDBrtVecSize		6


/*
 * Sizes of fixed-length strings in the file header. Definitions are
 * provided for both the number of characters in the string and the
 * constant which gives the string length including the terminating nul.
 * IDBhs...
 */
#define	IDBhsVersion		9		/* Any version string */
#define	IDBhsVersion1		(IDBhsVersion + 1)
#define	IDBhsCreator		29		/* Creator id */
#define	IDBhsCreator1		(IDBhsCreator + 1)
#define	IDBhsDate		URMhsDate	/* Any date string */
#define	IDBhsDate1		(IDBhsDate + 1)
#define	IDBhsModule		29		/* Module id */
#define	IDBhsModule1		(IDBhsModule + 1)


/*
 * Record number. 65k should be enough...
 */
typedef short int       IDBRecordNumber ;


/*
 * A resource ID consists of the resource map index and the ID index
 * in the given resource map.
 */
typedef short int       IDBResourceIndex ;
typedef union {
	unsigned		external_id ;	/* Looks like a fullword */
        struct {
	    IDBRecordNumber	map_rec B32 ;    /* resource map record */
	    IDBResourceIndex	res_index B32 ;  /* resource index into map */
        }       internal_id ;     		/* 2 fields internally */
} IDBridDesc ;

/*
 * A resource ID passed by value in a fullword.
 */
typedef unsigned        IDBResource ;		/* a declaration for RIDs
						   passed by value */




/*
 * Class descriptor.
 *
 * This descriptor contains the information URM requires about a toolkit
 * class.
 */
#define	URMWCIClassDescValid	39963371
#define	UrmWCIClassDescValid(classdesc)	\
    ((classdesc)->validation==URMWCIClassDescValid)

typedef struct _WCIClassDesc {
	unsigned	validation ;	/* URMWCIClassDescValid */
	struct _WCIClassDesc *next_desc ;
					/* next descriptor in chain */
	String		creator_name ;	/* create routine name. This is also
					   the accessor key for non-toolkit
					   widget classes. */
	MrmWidgetCreateProc		creator;	/* low-level create routine. This is
					   also the class identifier (name)
					   used to match user classes. */
	WidgetClass	class_record ;	/* Pointer to toolkit class record */
	MrmWidgetCleanupProc		cleanup;
					/* low-level destructor routine.
					   Used to clean up after creation
					   routines that leave dangling
					   memory (i.e. XmRendition,
					   XmRenderTable) */
} WCIClassDesc, *WCIClassDescPtr ;


/*
 * Table used to uncompress compression codes in a self-encoding UID file.
 * Table entries can be overlaid with function pointers for tables which
 * encode convenience functions.
 *
 * The table is accessed by the compression code. Code 0 is unused by
 * convention, and code 1 is unused because it is the marker for an
 * unknown (user defined) entry, which is specified by an explicit string.
 * Thus the valid entries begin at 2 (UilMrmMinValidCode). Any code
 * < UilMrmMinValidCode or > num_entries is invalid.
 *
 * When stored in a UID file, a table is a block of memory with the
 * following properties:
 *	o The table is allocated with enough space for all the decompression
 *	  pointers required.
 *	o The table is followed by the strings in the table. They are allocated
 *	  in a heap built from contiguous memory following table itself.
 */
#define UidCompressionTableValid	317938336
#define	UilMrmUnknownCode		1
#define UilMrmMinValidCode		2
#define	UilMrmReservedCodeCount		2

typedef union
    {
    MrmOffset		stoffset;		/* offset of string in
						   heap (in file) */
    char		*cstring;		/* compressed string as
						 a memory pointer */
    WCIClassDescPtr	cldesc;			/* class descriptor for
						   class table */
    } UidCTableEntry;

typedef struct
    {
    unsigned		validation;		/* validation code =
						   UIDCompressionTableValid */
    MrmCount		num_entries;		/* number of table entries */
    UidCTableEntry	entry[1];		/* first entry */
    } UidCompressionTable, *UidCompressionTablePtr;



/*
 * Hash table entry
 */
#define k_hash_table_size	(127)

typedef struct _URMHashTableEntry
{
    char		*az_value;	/* value associated with this name */
    struct _URMHashTableEntry
			*az_next_entry;	/* next entry on hash chain */
    char		c_text[1];	/* text of the name */
} URMHashTableEntry, *URMHashTableEntryPtr;



/*
 * Low level file information.
 *
 * This structure is pointed to in the IDBOpenFile structure and blindly
 * passed to the low level routines.
 */
typedef struct {
	unsigned	rab;		/* Record Access Block	*/
	int		file_desc;	/* File Descriptor */
	char		*name;		/* The file name		*/
} IDBLowLevelFile, *IDBLowLevelFilePtr;

/*
 * IDB open file information
 *
 * The following is the information retained by IDB for an open file.
 * Much of this information matches that found in the file header,
 * and is the source of for that info when the header is updated.
 */

#define	IDBOpenFileValid	421642674
typedef struct {
	unsigned	validation ;		/* validation code =
						   IDBOpenFileValid */
	int		get_count ;		/* # get operations on file */
	int		put_count ;		/* # put operations on file */
	MrmCode		access ;		/* URMReadAccess or
						   URMWriteAccess */
	IDBLowLevelFilePtr lowlevel_id ;	/* open file id returned
						   by low-level routines */
	IDBRecordNumber	index_root ;		/* index root pointer */
	MrmCount	num_indexed ;		/* # entries in index */
	MrmCount	num_RID ;		/* # RID entries in file */
	IDBridDesc	next_RID ;		/* next available RID */
	IDBRecordNumber	last_record ;		/* last record used in file */
	IDBRecordNumber	last_data_record ;	/* last data record in file.
						   NULL if no records yet */
	MrmCount	group_counts[URMgVecSize] ;
						/* vector of record counts
						   by resource group */
	MrmCount	rt_counts[IDBrtVecSize] ;
						/* vector of record counts by
						   record type (statistics) */
	unsigned long	timer ;			/* for statistics */
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
	UidCompressionTablePtr
			class_ctable;		/* compression table for
						   class info */
	UidCompressionTablePtr
			resource_ctable;	/* compression table for
						   resource info */
	int		user1 ;			/* for caller use */
	int		user2 ;			/* for caller use */
	MrmFlag         byte_swapped ;          /* indicates uid file created on
	  					   opposite-endian machine */
	MrmFlag		in_memory ;		/* for memory mapped files */
	unsigned char 	*uid_buffer ;		/* pointer to memory buffer */
} IDBOpenFile, *IDBFile ;



/*
 * Open hierarchy struct
 *
 * The following structure represents an open hierarchy. A hierarchy
 * is principally a list of open IDB files in search order. As an aid to
 * efficient searching, a list of files by resource group is also kept.
 * Each entry in the list is the subset of the files in the hierarchy
 * which contains resources in the corresponding group.
 */

#define	MrmHIERARCHY_VALID	102214835
typedef struct MrmHierarchyDescStruct {
	unsigned	validation ;		/* MrmHIERARCHY_VALID */
	MrmCount	num_file ;		/* number of files open */
	IDBFile		*file_list ;		/* list of open files */
	MrmCount	grp_num[URMgVecSize] ;	/* vector of file counts by
						   resource group */
	IDBFile		*grp_ids[URMgVecSize] ;	/* vectors of file ids by
						   resource group */
	URMHashTableEntryPtr
			*name_registry;		/* to register names */
} MrmHierarchyDesc;


/*
 * Validation macro
 */
#define	MrmHierarchyValid(hierarchy)		\
    ((hierarchy)->validation==MrmHIERARCHY_VALID)



/*
 * RGM widget record typedefs
 */

/*
 * RGM Widget record. All offset are relative to the beginning of the record
 */
#define	URMWidgetRecordValid	373946338

/* Variety codes. */
#define UilMrmWidgetVariety	0
#define UilMrmAutoChildVariety	1

typedef struct {
	unsigned		validation;	/* validation code =
					   URMWidgetRecordValid */
	MrmSize		size;		/* Total number of bytes for record */
	MrmCode		access;		/* URMaPublic or URMaPrivate */
	MrmCode		lock;		/* locking code */
	MrmType		type;		/* widget type, equates to class
					   for toolkit widgets.  (Unknown or
					   a uid-based compression code). */
	MrmOffset	name_offs;	/* Offset of widget name. Locates a
					   String. Empty string is illegal */
	MrmOffset	class_offs;	/* Offset of class name. The class name
					   is required for application-defined
					   widgets. */
	MrmOffset	arglist_offs; 	/* Offset of arglist descriptor.
					   Locates a RGMArgListDesc struct */
	MrmOffset	children_offs;	/* Offset of children descriptor.
					   Locates a RGMChildrenDesc struct */
	MrmOffset	comment_offs; 	/* Offset of comment string for
					   this widget. (URM extension) */
	MrmOffset	creation_offs;	/* Offset of RGMCallnackDesc struct
					   specifying URM creation callback
					   for this widget */
	unsigned long		variety;/* Regular widget or auto child. */
	unsigned long		annex; 	/* annex allows compatible growth */
} RGMWidgetRecord, *RGMWidgetRecordPtr ;

/*
 * Size of the header part of the record
 */
#define	RGMWidgetRecordHdrSize	sizeof(RGMWidgetRecord)

/*
 * Macro to access the widget record in a context
 */
#define	UrmWRWidgetRec(context)	\
	((RGMWidgetRecordPtr) UrmRCBuffer(context))


/*
 * All validation, accessor, and setting macros access a widget record in
 * a context. They all take a pointer to the widget record, which is
 * the context buffer CAST to RGMWidgetRecordptr.
 */
#define	UrmWRValid(widgetrec)	\
    ((widgetrec)->validation==URMWidgetRecordValid)

/*
 * Accessor macros for the widget record header
 */
#define UrmWRPSize(context)		(UrmWRWidgetRec(context)->size)
#define	UrmWRPAccess(context)		(UrmWRWidgetRec(context)->access)
#define	UrmWRPLock(context)		(UrmWRWidgetRec(context)->lock)
#define	UrmWRPType(context)		(UrmWRWidgetRec(context)->type)



/*
 * value descriptor. This consists of a representation type, followed
 * by a union of either an immediate value or an offset, depending
 * on the representation type.
 */
typedef struct {
	MrmType		rep_type;	/* representation type from
					   RGMrType... */
	union {
	  int		ival;		/* integer, boolean, compressed */
	  MrmOffset	offset ;	/* offset for all others */
	} datum ;			/* immediate value or offset */
} RGMArgValue, *RGMArgValuePtr ;


/*
 * RGM widget record argument descriptor
 */
typedef struct {
	MrmCode		tag_code;	/* Either the compressed code for
					   the argument tag from URMc...,
					   or UilMrmUnknownCode to indicate no
					   known string. If UilMrmUnknownCode, then
					   the offset is in tag_off */
	union
	    {
	    MrmOffset	tag_offs;	/* offset to tag string if needed */
	    MrmCode	related_code;	/* related argument code for case
					   where tag_code is valid and a
					   related argument is to be set */
	    } stg_or_relcode;
	RGMArgValue	arg_val ;	/* argument value */
} RGMArgument, *RGMArgumentPtr ;


/*
 * RGM widget record arglist descriptor
 */
typedef struct {
	MrmCount	count;		/* number of arguments in list */
	MrmCount	extra;		/* # extra runtime args needed */
	unsigned long	annex1 ;	/* annex allows compatible growth */
	RGMArgument	args[1] ;	/* first of the argument descriptors */
} RGMArgListDesc, *RGMArgListDescPtr ;



/*
 * RGM widget record text vector item (text vector literal)
 *
 * The following represents vectors of either ASCIZ or compound strings.
 * each member of the vector is type-tagged so that mixed vectors are possible.
 * This descriptor is a union so that a vector of these items can be over-
 * written to become an in-memory list of pointers. This avoids some
 * allocation problems at widget instantiation. Char8 items are String,
 * CString items are XmString.
 *
 * Offsets are interpreted as follows:
 *	text vector as immediate in widget record - offset from beginning
 *		of record buffer.
 *	text vector as UID literal - offset from beginning of resource buffer
 */
typedef union {
	  XtPointer	pointer ;	/* for use as memory pointer */
	  struct {
	      MrmType	rep_type B32;	/* MrmRtypeChar8 or MrmRtypeCString */
	      MrmOffset	offset B32;	/* offset of item in record */
	  } text_item ;			/* for use as item type & offset */
} RGMTextEntry, *RGMTextEntryPtr ;

/*
 * The text Vector. This is designed to allow the vector of text entries
 * to be over-written into an in-memory vector of pointers. Thus this list
 * has one extra entry to provide a terminating null.
 */
#define	URMTextVectorValid	34966592
typedef struct {
	unsigned	validation;	/* URMTextVectorValid */
	MrmCount	count;		/* Number of text items in vector */
	RGMTextEntry	item[1] ;	/* first text item in a vector of
					   n = .count+1 items. item[.count]
					   always = NULL */
} RGMTextVector, *RGMTextVectorPtr ;


/*
 * The integer Vector. This is designed to allow the vector of integers
 * to be over-written into an in-memory vector of pointers.
 */
#define	URMIntegerVectorValid	34966593
typedef struct {
	unsigned	validation;	/* URMIntegerVectorValid */
	MrmCount	count;		/* Number of integer items in vector */
	int		item[1] ;	/* first integer item in a vector   */
					/* of n = .count+1 items.	    */
} RGMIntegerVector, *RGMIntegerVectorPtr ;


/*
 * RGM widget record callback item
 *
 * A callback item is the offset of a routine name (which must be a
 * non-imported string), and the routine tag value, which is encoded
 * as a value. The tag value can evaluate to almost anything, including
 * an imported value.
 *
 * This structure is intended to be over-written with memory pointers
 * and thus function as an arglist value.
 */
typedef union
{
  int		ival;		/* integer, boolean, compressed */
  MrmOffset	offset;		/* offset for all others */
} RGMdatum;			/* immediate value, offset, widget */


typedef union {
  struct {
    XtCallbackRec	callback ;	/* item as Xt callback */
    MrmFlag		resolved; 	/* Item resolved to XtCallbackRec */
    String		wname;		/* Name of widget reference */
  } 	runtime;

  struct {
    MrmOffset		routine B32;	/* offset to routine name string */
					/* duplicate RGMArgValue, avoiding
					   union alignment hassles */
    MrmType		rep_type B32;	/* from RGMrType... */
    RGMdatum		datum ;		/* immediate value, offset, widget */
  } 	cb_item ;			/* callback item in record */
} RGMCallbackItem, *RGMCallbackItemPtr ;

/*
 * RGM widget record callback argument descriptor. This list has
 * count + 1 entries so that this structure may be overwritten
 */
#define	URMCallbackDescriptorValid	107397480
typedef struct {
        unsigned	validation;		/* URMCallbackDescriptorValid */
	MrmCount	count;			/* Number of callback items */
	MrmCount	annex;		  	/* annex allows growth */
	MrmCount	unres_ref_count;	/* # of unresolved widget refs
	                                           in the RGMCallbackItems */
	RGMCallbackItem item[1] ;		/* First callback item in a list
	                                           of n = .count+1 items */
} RGMCallbackDesc, *RGMCallbackDescPtr ;

/*
 * We need to have the 1.1 versions of RGMCallbackItem and RGMCallbackDesc
 * so that backwards compatibility can be maintained between 1.1 and 1.2.
 */
typedef union {
	XtCallbackRec	runtime ;	/* item as Xt callback */
	struct {
	  MrmOffset	routine ;	/* offset to routine name string */
					/* duplicate RGMArgValue, avoiding
					   union alignment hassles */
	  MrmType	rep_type ;	/* from RGMrType... */
	  RGMdatum	 datum ;	/* immediate value or offset */
	} cb_item ;			/* callback item in record */
} OldRGMCallbackItem, *OldRGMCallbackItemPtr ;

typedef struct {
	unsigned	validation ;	/* URMCallbackDescriptorValid */
	MrmCount	count ;		/* Number of callback items */
	unsigned long	annex1 ;	/* annex allows compatible growth */
	OldRGMCallbackItem  item[1] ;	/* First callback item in a list
					   of n = .count+1 items */
} OldRGMCallbackDesc, *OldRGMCallbackDescPtr ;

/* BEGIN OSF Fix CR 4859 */

/*
 * Wide character uid form
 *
 * The following represents a multi-byte string which needs to be converted
 * to a wide-character string when it is retrieved.
 * The structure contains a byte count followed by the bytes.
 */
typedef union {
    XtPointer	pointer;			  /* for use as memory pointer */
    struct {
      MrmCount	count;				  /* # of bytes */
      char bytes[1];				  /* vector of bytes */
    } wchar_item;
} RGMWCharEntry, *RGMWCharEntryPtr;

/* END OSF Fix CR 4859 */

/*
 * Font literal
 *
 * The following represents a MrmRtypeFont literal. Since it contains two
 * strings, the same offset mechanism is used to store both, and this
 * is a fixed-length data structure.
 * data structure.
 */
typedef struct {
    union
	{
	/*XmStringCharset*/char* 	charset; /* character set (resolved) */
	MrmOffset	cs_offs;	/* character set in buffer */

	} cset;
    union
	{
	String		font;		/* string naming font */
	MrmOffset	font_offs;	/* font name in buffer */
	} font;
    MrmCode		type;
} RGMFontItem, *RGMFontItemPtr ;


/*
 * A font list, which is a vector of font items. The font items are
 * treated as a heap store following the fixed length list (similar
 * to text vectors). Font lists in widget records have offsets from
 * the beginning of the record; font list literals in a UID file have
 * offsets from the beginning of the list itself (just like text
 * vectors). Font items in a font list are a union of a pointer and
 * an offset, so that (as in text vectors) they can be converted in
 * place to an in-memory pointer list.
 */
#define URMFontListValid	74149235
typedef struct {
	unsigned		validation;	/* URMFontListValid */
	MrmCount		count;		/* # of font items in list */
	RGMFontItem		item[1] ;	/* vector of RGMFontItems */
} RGMFontList, *RGMFontListPtr ;

/*
 * We need to have the 1.1 versions of RGMFontItem and RGMFontList
 * so that backwards compatibility can be maintained between 1.1 and 1.2.
 */
typedef struct {
    union
	{
	/*XmStringCharset*/char*	charset;	/* character set (resolved) */
	MrmOffset	cs_offs;	/* character set in buffer */
	} cset;
    union
	{
	String		font;		/* string naming font */
	MrmOffset	font_offs;	/* font name in buffer */
	} font;
} OldRGMFontItem, *OldRGMFontItemPtr ;

typedef struct {
	unsigned		validation ;	/* URMFontListValid */
	MrmCount		count ;		/* # of font items in list */
	OldRGMFontItem		item[1] ;	/* vector of RGMFontItems */
} OldRGMFontList, *OldRGMFontListPtr ;


/*
 * RGM widget record child descriptor
 */
typedef struct {
	MrmFlag		manage ;	/* true if child is managed by parent
					   at initialization */
	MrmSCode	access ;	/* URMaPublic or URMaPrivate */
	MrmSCode	unused1 ;	/* formerly class, unused anywhere */
	MrmSCode	type ;		/* specifier type - URMrIndex or
					   URMrRID */
	unsigned long	annex1;		/* annex allows compatible growth */
	union {
	  int		index_offs;	/* index string offset (URMrIndex) */
	  MrmResource_id	id;	/* IDB resource id (URMrRID) */
	} key ;				/* key to widget by index or id */
} RGMChildDesc, *RGMChildDescPtr ;


/*
 * RGM widget record children list descriptor
 */
typedef struct {
	MrmCount	count;		/* number of children */
	MrmCode		unused1;	/* formerly access, unused anywhere */
	unsigned long	annex1;		/* annex allows compatible growth */
	RGMChildDesc	child[1] ;	/* First child descriptor in a list of
					   n = .count descriptors. */
} RGMChildrenDesc, *RGMChildrenDescPtr ;


/*
 * RGM widget record URM resource descriptor. This is commonly used as the
 * value of an argument which is a discrete resource. In this case, the
 * resource specifies the data type required for the argument (from
 * RGMrType...) to allow conversion if the literal is a different type
 * when read.
 */
typedef struct {
	MrmSize		size;		/* number of bytes in structure */
	MrmFlag		access ;	/* Public/private - URMaPublic or
					   URMaPrivate for this resource */
	MrmSCode	type ;		/* URMrIndex or URMrRID */
	MrmSCode	res_group ;	/* resource group */
	MrmSCode	cvt_type ;	/* RGMrType... to which the resource
					   needs to be converted when used
					   as an argument value. RGMwrType
					   for widget references. */
	unsigned long	annex1 ;	/* annex allows compatible growth */

	union {
	  char		index[1] ;	/* index for URMrIndex resource
					   (nul teminated) */
	  MrmResource_id	id ;	/* resource id */
	} key ;				/* key to resource by index or id */
} RGMResourceDesc, *RGMResourceDescPtr ;

/*
 * When a resource descriptor names a widget, the cvt_type is used to
 * indicate the type of reference, which indicates the processing required.
 */
#define	RGMwrTypeReference	233	/* Reference a widget defined by
					   a CreateInstance elsewhere */
#define	RGMwrTypeSubTree	234	/* Subtree reference. The subtree
					   must be instantiated as part of
					   the current operation */



/*
 * Interface module descriptor
 *
 * An interface module is currently the only type of resource set defined.
 * It defines the topmost widgets for some application (there is no
 * requirement that there be only one interface module for an application).
 */

/*
 * A topmost widget descriptor. These are currently required to be
 * accessed by index only, and the index is currently the only information
 * saved. The index is stored as a fixed-length entry to simplify the
 * layout of an interface module.
 *
 * The widgets referenced in an interface module are those who have
 * no parent in the interface definition. Commonly these will have
 * the top-level widget as their parent at runtime, but this is not
 * required.
 */
typedef struct {
	char		index[URMMaxIndexLen1] ;
					/* Index (name) of widget */
} RGMTopmostDesc, *RGMTopmostDescPtr ;


#define URMInterfaceModuleValid	408427447
typedef struct {
	unsigned	validation;	/* URMInterfaceModuleValid */
	MrmCount	count;		/* number of topmost widgets */
	unsigned long	annex1;		/* annex allows compatible growth */
	unsigned long	annex2;		/* annex allows compatible growth */
	RGMTopmostDesc	topmost[1] ;	/* First topmost descriptor in a
					   list of n= .count descriptors */
} RGMModuleDesc, *RGMModuleDescPtr ;


/*
 * Validation macro
 */
#define	UrmInterfaceModuleValid(module)		\
    ((module)->validation==URMInterfaceModuleValid)



/*
 * Representations for URM color descriptor (MrmRtypeColor) and
 * color table (MrmRtypeColorTable).
 */

/*
 * RGB color data structure
 */

typedef struct {
	unsigned short	red;   		/* RGB colors */
	unsigned short	green;   	/* RGB colors */
	unsigned short	blue;   	/* RGB colors */
}RGBColor, *RGBColorPtr;

/*
 * URM color descriptor. This is a color name plus a fallback
 * designator specifying the color as foreground or background.
 */
typedef struct {
	MrmSCode	mono_state ;	/* monochrome treatment, from
					   URMColorMonochrome... */
	MrmSCode	desc_type;	/* color desc_type from
					   URMColorDescType... */
        union {
	    char	name[1] ;	/* color name */
	    RGBColor	rgb ;		/* RGB Color */
	    } desc;
} RGMColorDesc, *RGMColorDescPtr ;

/*
 * Monochrome treatments
 */
#define	URMColorMonochromeUnspecified	1
#define	URMColorMonochromeForeground	2
#define	URMColorMonochromeBackground	3


/*
 * Color descriptor types
 */
#define URMColorDescTypeName	1
#define URMColorDescTypeRGB	2

/*
 * Use this Min size plus the size of the color name string to determine the
 * exact size that the RGMColorDesc will need to allocate.
 */
#define RGMColorDescNameMinSize (sizeof (RGMColorDesc)\
				 - sizeof (RGBColor)\
				 + sizeof (char))

/*
 * URM color table item. This is a pointer to a color descriptor, plus
 * a Pixel for the runtime conversion of the color.
 */
typedef struct {
	Pixel			color_pixel ;	/* runtime translation */
	MrmType			type;	/* descriptor reference type,
					   either MrmRtypeColor, or
					   or MrmRtypeResource if
					   coffs locates a resource */
	unsigned long		annex1;	/* annex allows growth */

	union {
	  RGMColorDescPtr	cptr ;	/* in-memory color descriptor */
	  MrmOffset		coffs; 	/* descriptor/resource in record */
	} color_item ;			/* pointer or offset */
} RGMColorTableEntry, *RGMColorTableEntryPtr ;


/*
 * A color table. This is a vector of color items. Items 0 and 1 are
 * reserved for "background"(0) and "foreground"(1) in all color tables;
 * user-specified colors begin at entry 2. The color count always
 * includes these predefined colors, so the minimum count is 2. No
 * color descriptor is required or used for the predefined entries;
 * only the color_pixel field in the table item is ever used.
 *
 * Color descriptor offsets are valid only in a widget record. All
 * other instances of a color table, including those which are
 * are parameters to the widget record creation routines, must have
 * valid memory pointers.
 */
#define	URMColorTableValid	9424574
typedef struct {
	unsigned		validation ;	/* URMColorTableValid */
	MrmCount		count ;		/* # entries in table */
	RGMColorTableEntry	item[1] ;	/* 1st entry in table */
} RGMColorTable, *RGMColorTablePtr ;

/*
 * Predefined entries, first accessible user-defined entry
 */
#define	URMColorTableBG		0
#define	URMColorTableFG		1
#define	URMColorTableUserMin	2



/*
 * Definitions to a support a URM icon; the representation for
 * MrmRtypeIconImage objects.
 *
 * A URM icon supports a pixmap of arbitrary size and with, and with support
 * for up to 256 colors including foreground and background. The
 * icon pixels are stored using either 1, 4, or 8 bits, depending on
 * how much information is required to index the icon's color map.
 *
 * The pixel array is stored in row-major order. Each row begins on a byte
 * boundary no matter what the pixel size is. URM uses the ZPixmap
 * representation for the image data at runtime, and attempts to avoid
 * allocating memory for the pixel array by overwriting the pixel data
 * array with Pixel values if the number of bits per pixel is sufficient.
 *
 * The color table design is intended to support eventual creation of
 * color tables as discrete resources, possibly accessible as resource
 * references. Currently, the color table must be allocated and managed
 * as part of the icon image.
 *
 * Icon image offsets are valid only in a widget record. All
 * other instances of an icon image, including those which are
 * are parameters to the widget record creation routines, must have
 * valid memory pointers.
 */

#define	URMIconImageValid	160003640
typedef struct {
	unsigned		validation;	/* URMIconImageValid */
	MrmSCode		pixel_size ;	/* from URMIconPixelSize... */
	MrmCount		width;		/* # pixels wide */
	MrmCount		height;		/* # pixels high */
	MrmCount		hot_x, hot_y ;	/* hot spot location */
	MrmType			ct_type;	/* MrmRtypeColorTable for
						   table in record.
						   MrmRtypeResource for
						   separate resource ref. */
	unsigned long		annex1;	 	/* annex allows growth */
	union {
	  RGMColorTablePtr	ctptr ;		/* in-memory color table */
	  MrmOffset		ctoff;		/* color table or resource
						   desc offset in record */
	} color_table ;				/* memory/record color table */
	union {
	  char			*pdptr ;	/* pixel data in memory */
	  MrmOffset		pdoff;		/* pixel data record offset */
	} pixel_data ;
} RGMIconImage, *RGMIconImagePtr ;


/*
 * Pixel sizes. These are limited and specified with literals.
 */
#define	URMPixelSize1Bit	1	/* 1 bit (bitmap) */
#define	URMPixelSize2Bit	2	/* 2 bits */
#define	URMPixelSize4Bit	3	/* 4 bits */
#define	URMPixelSize8Bit	4	/* 8 bits */



/*
 * Definitions of the data structures used to store units information for
 * dimensional values.
 */
typedef struct {
    unsigned value; /* the actual integer value */
    unsigned units; /* the units designation of the value. Matches
		     the Xm units values in Xm.h */
} RGMUnitsInteger, *RGMUnitsIntegerPtr ;

typedef struct {
    unsigned long value[2]; /* the actual floating point value */
    unsigned units;  /* the units designation of the value. Matches
		     the Xm units values in Xm.h */
} RGMUnitsFloat, *RGMUnitsFloatPtr ;


/*
 * Definition of the data structure used during widget creation to retain
 * and provide the information required to resolve widget arguments which
 * must be SetValue rather than create. The layout and use of
 * the descriptor depends on the type argument:
 *		URMsvWidgetRef - widget resource forward reference. Resolved
 *			when the widget is defined, applied when all the
 *			widgets in a tree are instantiated.
 *		URMsvIconImage - Icon image which is SetValued immediately
 *			after the widget create (when foreground/backgound
 *			info is available).
 */
typedef struct {
	Widget		setw ;		/* widget to be modified */
	MrmCode		type ;		/* from URMsv... */
	MrmCode		tagcode ;	/* code of argument to be set */
	String		tagname ;	/* string name of argument. If
					   the tag code is UilMrmUnknownCode,
					   this must be freed. */
	MrmFlag		done ;		/* set TRUE when SetValues done */
	union {
	    String		wname ;		/* arg widget index */
	    RGMIconImagePtr	icon ;		/* icon pointer only */
	    RGMCallbackDescPtr	callbacks;	/* callback list */
	} sv ;				/* some SetValues descriptor */
} URMSetValuesDesc, *URMSetValuesDescPtr ;

/*
 * SetValues descriptor types
 */
#define	URMsvWidgetRef		1	/* widget reference */
#define	URMsvIconImage		2	/* icon image */
#define	URMsvCallBackList	3	/* list of callbacks */


/*
 * The following data structures support resolving references to widgets
 * found during creation of a widget tree. The data structure retains
 * an ordered list of the widget name/id pairs instantiated during
 * tree creation. The data structure is maintained in a resource context
 * which allows for expansion. The top part of the data structure retains
 * fixed-length information, while the bottom part of the buffer is
 * used as a string heap.
 */

/*
 * Single element in the fixed-length vector
 */
typedef struct {
    Widget		w_id;			/* widget id */
    MrmOffset		w_name_offs;		/* name offset in heap */
} URMWRef, URMWRefPtr;

/*
 * Header for the structure
 */
typedef struct {
    MrmCount		num_refs;		/* # entries defined */
    MrmCount		heap_size;		/* # bytes used in heap */
    URMWRef		refs[1];		/* beginning of vector */
} URMWRefStruct, *URMWRefStructPtr;


/*
 * size and access macros
 */
#define UrmWRefHeaderBytes(ref_id) \
    (sizeof(URMWRefStruct)+((ref_id)->num_refs-1)*sizeof(URMWRef))
#define UrmWRefBytesUsed(ref_id) \
    (UrmWRefHeaderBytes((ref_id))+(ref_id)->heap_size)
#define UrmWRefIdN(ref_id,ndx) \
    ((ref_id)->refs[(ndx)].w_id)
#define UrmWRefNameN(ref_id,ndx) \
    ((String)((String)(ref_id)+(ref_id)->refs[(ndx)].w_name_offs))

/*
 * Function prototypes for URM routines
 */
#ifndef _ARGUMENTS
#define _ARGUMENTS(arglist) arglist
#endif

#include "IDB.h"			/* needed for prototype definitions */

/* mrmcontext.c */
extern Cardinal UrmGetResourceContext(char *(*alloc_func)(Cardinal),
                                      void (*free_func)(void *),
                                      MrmSize size,
                                      URMResourceContextPtr *context_id_return);
extern Cardinal UrmResizeResourceContext  _ARGUMENTS(( URMResourceContextPtr context_id ,
						    int size ));
extern Cardinal UrmFreeResourceContext  _ARGUMENTS(( URMResourceContextPtr context_id ));

/* mrmerror.c */
extern Cardinal Urm__UT_Error(const char *module,
                              const char *error,
                              IDBFile file_id,
                              URMResourceContextPtr context_id,
                              Cardinal status);
extern Cardinal Urm__UT_SetErrorReport  _ARGUMENTS(( MrmCode report_type ));
extern MrmCode Urm__UT_LatestErrorCode  _ARGUMENTS(( void ));
extern String Urm__UT_LatestErrorMessage  _ARGUMENTS(( void ));
extern String Urm__UT_UrmCodeString  _ARGUMENTS(( MrmCode cod ));

/* mrmhier.c */
extern Cardinal Urm__OpenHierarchy  _ARGUMENTS(( MrmCount num_files ,
						 String *name_list ,
						 MrmOsOpenParamPtr *os_ext_list ,
						 MrmHierarchy *hierarchy_id_return,
						 MrmFlag in_memory,
						 unsigned char *uid_buffer));
extern Cardinal Urm__CloseHierarchy  _ARGUMENTS(( MrmHierarchy hierarchy_id ));
extern Cardinal UrmHGetIndexedResource(MrmHierarchy hierarchy_id,
                                       const char *index,
                                       MrmGroup group_filter,
                                       MrmType type_filter,
                                       URMResourceContextPtr context_id,
                                       IDBFile *file_id_return);
extern Cardinal Urm__RegisterNamesInHierarchy  _ARGUMENTS(( MrmHierarchy hierarchy_id ,
							    String *names ,
							    XtPointer *values ,
							    MrmCount num_cb ));
extern Cardinal Urm__LookupNameInHierarchy  _ARGUMENTS(( MrmHierarchy hierarchy_id ,
							String name ,
							XtPointer *value_return ));

/* mrmibuffer.c */
extern Cardinal Idb__BM_InitBufferVector _ARGUMENTS(( void ));
extern Cardinal Idb__BM_GetBuffer  _ARGUMENTS(( IDBFile file_id ,
						IDBRecordBufferPtr *buffer_return ));
extern Cardinal Idb__BM_MarkActivity  _ARGUMENTS(( IDBRecordBufferPtr buffer ));
extern Cardinal Idb__BM_MarkModified  _ARGUMENTS(( IDBRecordBufferPtr buffer ));
extern Cardinal Idb__BM_GetRecord  _ARGUMENTS(( IDBFile file_id ,
						IDBRecordNumber record ,
						IDBRecordBufferPtr *buffer_return ));
extern Cardinal Idb__BM_InitRecord  _ARGUMENTS(( IDBFile file_id ,
						IDBRecordNumber record ,
						MrmType type ,
						IDBRecordBufferPtr *buffer_return ));
extern Cardinal Idb__BM_InitDataRecord  _ARGUMENTS(( IDBFile file_id ,
						IDBRecordBufferPtr *buffer_return ));
extern Cardinal Idb__BM_Decommit  _ARGUMENTS(( IDBRecordBufferPtr buffer ));
extern Cardinal Idb__BM_DecommitAll  _ARGUMENTS(( IDBFile file_id ));

/* mrmicon.c */
extern Cardinal UrmCreatePixmap  _ARGUMENTS(( RGMIconImagePtr icon ,
					    Screen *screen ,
					    Display *display ,
					    Pixel fgpix ,
					    Pixel bgpix ,
					    Pixel *pixmap ,
					    Widget parent ));
extern Cardinal UrmCreateBitmap	_ARGUMENTS(( RGMIconImagePtr icon ,
					    Screen *screen ,
					    Display *display ,
					    Pixel *pixmap ));
extern Cardinal Urm__CW_ReadBitmapFile  _ARGUMENTS(( String filename ,
						    Screen *screen ,
						    Pixel fgint ,
						    Pixel bgint ,
						    Pixmap *pixmap ,
						    Widget parent ));
extern Cardinal Urm__MapIconBitmap  _ARGUMENTS(( RGMIconImagePtr icon ,
						int srcpix ,
						RGMColorTablePtr ctable ,
						Screen *screen ,
						Display *display ,
						Pixmap *pixmap ));
extern Cardinal Urm__MapIconBitmapDepth1  _ARGUMENTS(( RGMIconImagePtr icon ,
 						int srcpix ,
 						Screen *screen ,
 						Display *display ,
 						Pixmap *pixmap ));
extern Cardinal Urm__MapIconReplace  _ARGUMENTS(( RGMIconImagePtr icon ,
						int srcpix ,
						RGMColorTablePtr ctable ,
						Screen *screen ,
						Display *display ,
						Pixmap *pixmap ,
						Widget parent ));
extern Cardinal Urm__MapIconAllocate  _ARGUMENTS(( RGMIconImagePtr icon ,
						int srcpix ,
						int dstpix ,
						RGMColorTablePtr ctable ,
						Screen *screen ,
						Display *display ,
						Pixmap *pixmap ,
						Widget parent ));
extern Cardinal Urm__RealizeColorTable  _ARGUMENTS(( Screen *screen ,
						Display *display ,
						Pixel fgpix ,
						Pixel bgpix ,
						RGMColorTablePtr ctable ,
						Widget parent ));
extern RGMIconImagePtr UrmCopyAllocatedIconImage  _ARGUMENTS(( RGMIconImagePtr dst_icon ,
							    RGMIconImagePtr src_icon ));
extern Cardinal Urm__UT_GetNamedColorPixel  _ARGUMENTS(( Display *display ,
							Colormap cmap ,
							RGMColorDescPtr colorptr ,
							Pixel *pixel_return ,
							Pixel fallback));
extern Cardinal Urm__UT_GetColorPixel  _ARGUMENTS(( Display *display ,
						    Colormap cmap ,
						    RGMColorDescPtr colorptr ,
						    Pixel *pixel_return,
						    Pixel fallback));
extern Cardinal UrmIconImageSize  _ARGUMENTS(( RGMIconImagePtr icon ));
extern Cardinal UrmColorTableSize  _ARGUMENTS(( RGMColorTablePtr ctable ));

/* mrmientry.c */
extern Cardinal Idb__DB_GetDataEntry  _ARGUMENTS(( IDBFile file_id ,
						IDBDataHandle data_entry ,
						URMResourceContextPtr context_id ));
extern Cardinal Idb__DB_PutDataEntry  _ARGUMENTS(( IDBFile file_id ,
						URMResourceContextPtr context_id ,
						IDBDataHandle *data_entry ));
extern Boolean Idb__DB_MatchFilter  _ARGUMENTS(( IDBFile file_id ,
						IDBDataHandle data_entry ,
						MrmCode group_filter ,
						MrmCode type_filter ));

/* mrmifile.c */
extern Cardinal Idb__FU_OpenFile  _ARGUMENTS(( char *name ,
						MrmCode access ,
						MrmOsOpenParamPtr os_ext ,
						IDBLowLevelFilePtr *file_id ,
						char *returned_fname ));
extern Cardinal Idb__FU_PutBlock _ARGUMENTS((IDBLowLevelFile	*file_id,
					     IDBRecordNumber	block_num,
					     char		*buffer)) ;
extern Cardinal Idb__FU_CloseFile _ARGUMENTS((IDBLowLevelFile	*file_id,
				   int			del)) ;
extern Cardinal Idb__FU_GetBlock  _ARGUMENTS(( IDBLowLevelFile *file_id ,
						IDBRecordNumber block_num ,
						char *buffer ));

/* mrmiheader.c */
extern Cardinal Idb__HDR_GetHeader  _ARGUMENTS(( IDBFile file_id ));
extern Cardinal Idb__HDR_InitHeader  _ARGUMENTS(( IDBFile file_id ,
						String creator ,
						String creator_version ,
						String module ,
						String module_version ));
extern Cardinal Idb__HDR_PutHeader  _ARGUMENTS(( IDBFile file_id ));
extern Cardinal Idb__HDR_UpdateHeader  _ARGUMENTS(( IDBFile file_id ,
						    IDBRecordBufferPtr *buffer ));
extern Cardinal Idb__HDR_EnterItem  _ARGUMENTS(( IDBFile file_id ,
						IDBResource resource_id ,
						IDBDataHandle data_entry ));
extern Cardinal Idb__HDR_ReturnItem  _ARGUMENTS(( IDBFile file_id ,
						IDBResource resource_id ,
						Boolean signal_null ,
						IDBDataHandle *entry_return ));
extern Cardinal Idb__HDR_NextRID  _ARGUMENTS(( IDBFile file_id ,
						IDBResource *res_id_return ));
extern Cardinal Idb__HDR_GetDataEntry  _ARGUMENTS(( IDBFile file_id ,
						    IDBDataHandle data_entry ,
						    URMResourceContextPtr context_id ));
extern Cardinal Idb__HDR_PutDataEntry  _ARGUMENTS(( IDBFile file_id ,
						    URMResourceContextPtr context_id ,
						    IDBDataHandle *data_entry ));
extern Boolean Idb__HDR_MatchFilter  _ARGUMENTS(( IDBFile file_id ,
						    IDBDataHandle data_entry ,
						    MrmCode group_filter ,
						    MrmCode type_filter ));

/* mrmiindex.c */
extern Cardinal Idb__INX_ReturnItem(IDBFile file_id,
                                    const char *index,
                                    IDBDataHandle *data_entry);
extern Cardinal Idb__INX_FindIndex(IDBFile file_id,
                                   const char *index,
                                   IDBRecordBufferPtr *buffer_return,
                                   MrmCount *index_return);
extern Cardinal Idb__INX_SearchIndex(IDBFile file_id,
                                     const char *index,
                                     IDBRecordBufferPtr buffer,
                                     MrmCount *index_return);
extern Cardinal Idb__INX_GetBtreeRecord  _ARGUMENTS(( IDBFile file_id ,
							IDBRecordBufferPtr *buffer_return ,
							MrmCount entry_index ,
							Cardinal order ));
extern Cardinal Idb__INX_FindResources  _ARGUMENTS(( IDBFile file_id ,
							IDBRecordNumber recno ,
							MrmGroup group_filter ,
							MrmType type_filter ,
							URMPointerListPtr index_list ));

/* mrmiindexw.c */
extern Cardinal Idb__INX_EnterItem(IDBFile file_id,
                                   const char *index,
                                   IDBDataHandle data_entry);
extern Cardinal Idb__INX_EnterLeafIndex(IDBFile file_id,
                                        IDBRecordBufferPtr buffer,
                                        const char *index,
                                        IDBDataHandle data_entry,
                                        MrmCount entry_index,
                                        Cardinal order);
extern Cardinal Idb__INX_EnterNodeIndex(IDBFile file_id,
                                        IDBRecordBufferPtr buffer,
                                        const char *index,
                                        IDBDataHandle data_entry,
                                        IDBRecordNumber lt_record,
                                        IDBRecordNumber gt_record);
extern Cardinal Idb__INX_SplitLeafRecord  _ARGUMENTS(( IDBFile file_id ,
							IDBRecordBufferPtr gt_buffer ));
extern Cardinal Idb__INX_SplitNodeRecord  _ARGUMENTS(( IDBFile file_id ,
							IDBRecordBufferPtr gt_buffer ));
extern Cardinal Idb__INX_InitRootLeafRecord  _ARGUMENTS(( IDBFile file_id ,
							    IDBRecordBufferPtr *buffer_return ));
extern Cardinal Idb__INX_InitRootNodeRecord(IDBFile file_id,
                                            IDBRecordBufferPtr *buffer_return,
                                            const char *index,
                                            IDBDataHandle data_entry,
                                            IDBRecordNumber lt_record,
                                            IDBRecordNumber gt_record);
extern void Idb__INX_CopyLeafRecord  _ARGUMENTS(( IDBIndexLeafRecordPtr dst_recptr ,
						    IDBIndexLeafRecordPtr src_recptr ));
extern void Idb__INX_CopyNodeRecord  _ARGUMENTS(( IDBIndexNodeRecordPtr dst_recptr ,
						    IDBIndexNodeRecordPtr src_recptr ));
extern void Idb__INX_CollapseLeafRecord  _ARGUMENTS(( IDBIndexLeafRecordPtr recptr ,
							MrmCount start ,
							MrmCount end ));
extern void Idb__INX_CollapseNodeRecord  _ARGUMENTS(( IDBIndexNodeRecordPtr recptr ,
							MrmCount start ,
							MrmCount end ));
extern Cardinal Idb__INX_ConfirmNodeSpace  _ARGUMENTS(( IDBFile file_id ,
							IDBRecordBufferPtr buffer ));
extern Cardinal Idb__INX_SetParent  _ARGUMENTS(( IDBFile file_id ,
						    IDBRecordNumber parent_record ,
						    IDBRecordNumber child_record ));
extern Cardinal Idb__INX_FixNodeChildren  _ARGUMENTS(( IDBFile file_id ,
							IDBRecordNumber p_record ));

/* mrmirid.c */
extern Cardinal Idb__RID_EnterItem  _ARGUMENTS(( IDBFile file_id ,
						    IDBResource resource_id ,
						    IDBDataHandle data_entry ));
extern Cardinal Idb__RID_ReturnItem  _ARGUMENTS(( IDBFile file_id ,
						    IDBResource resource_id ,
						    Boolean signal_null ,
						    IDBDataHandle *entry_return ));
extern Cardinal Idb__RID_NextRID  _ARGUMENTS(( IDBFile file_id ,
						IDBResource *res_id_return ));
extern Cardinal Idb__RID_AddRecord  _ARGUMENTS(( IDBFile file_id ));

/* mrmitop.c */
extern Cardinal UrmIdbOpenFileRead  _ARGUMENTS(( String name ,
						    MrmOsOpenParamPtr os_ext ,
						    IDBFile *file_id_return ,
						    char *fname_return ));
extern Cardinal UrmIdbOpenBuffer  _ARGUMENTS(( unsigned char *uid_buffer ,
						    IDBFile *file_id_return ));
extern Cardinal UrmIdbCloseFile  _ARGUMENTS(( IDBFile file_id ,
						Boolean keep_new_file ));
extern Cardinal UrmIdbGetIndexedResource(IDBFile file_id,
                                         const char *index,
                                         MrmGroup group_filter,
                                         MrmType type_filter,
                                         URMResourceContextPtr context_id );
extern Cardinal UrmIdbFindIndexedResource  _ARGUMENTS(( IDBFile file_id ,
							MrmGroup group_filter ,
							MrmType type_filter ,
							URMPointerListPtr index_list ));
extern Cardinal UrmIdbGetRIDResource  _ARGUMENTS(( IDBFile file_id ,
							MrmResource_id resource_id ,
							MrmGroup group_filter ,
							MrmType type_filter ,
							URMResourceContextPtr context_id ));
extern Cardinal UrmIdbGetResourceId  _ARGUMENTS(( IDBFile file_id ,
							MrmResource_id *resource_id_return ));
extern Cardinal Idb__FIL_Valid  _ARGUMENTS(( IDBFile file_id ));

/* mrmitopw.c */
extern Cardinal UrmIdbOpenFileWrite  _ARGUMENTS(( String name ,
						    MrmOsOpenParamPtr os_ext ,
						    String creator ,
						    String creator_version ,
						    String module ,
						    String module_version ,
						    IDBFile *file_id_return ,
						    char *fname_return ));
extern Cardinal UrmIdbPutIndexedResource(IDBFile file_id,
                                         const char *index,
                                         URMResourceContextPtr context_id );
extern Cardinal UrmIdbPutRIDResource  _ARGUMENTS(( IDBFile file_id ,
						    MrmResource_id resource_id ,
						    URMResourceContextPtr context_id ));

/* mrmlread.c */
extern Cardinal Urm__FetchLiteral(MrmHierarchy hierarchy_id,
                                  const char *index,
                                  URMResourceContextPtr context_id,
                                  URMPointerListPtr *ctxlist);
extern Cardinal UrmGetIndexedLiteral(IDBFile file_id,
                                     const char *index,
                                     URMResourceContextPtr context_id);
extern Cardinal UrmGetRIDLiteral  _ARGUMENTS(( IDBFile file_id ,
						MrmResource_id resource_id ,
						URMResourceContextPtr context_id ));
extern Cardinal Urm__HGetIndexedLiteral(MrmHierarchy hierarchy_id,
                                        const char *index,
                                        URMResourceContextPtr context_id,
                                        IDBFile *file_id_return);
extern Cardinal UrmHGetIndexedLiteral(MrmHierarchy hierarchy_id,
                                      const char *index,
                                      URMResourceContextPtr context_id);

/* mrmlwrite.c */
extern Cardinal UrmPutIndexedLiteral  _ARGUMENTS(( IDBFile file_id ,
						    String index ,
						    URMResourceContextPtr context_id ));
extern Cardinal UrmPutRIDLiteral  _ARGUMENTS(( IDBFile file_id ,
						    MrmResource_id resource_id ,
						    URMResourceContextPtr context_id ));

/* mrmmodule.c */
extern Cardinal UrmIFMInitModule  _ARGUMENTS(( URMResourceContextPtr context_id ,
						MrmCount num_widget ,
						MrmCode access ,
						MrmCode lock ));
extern Cardinal UrmIFMSetTopmost  _ARGUMENTS(( URMResourceContextPtr context_id ,
						Cardinal topmost_ndx ,
						String index ));
extern Cardinal UrmIFMPutModule(IDBFile file_id, const char *index,
                                URMResourceContextPtr context_id);
extern Cardinal UrmIFMHGetModule(MrmHierarchy hierarchy_id,
                                 const char *index,
                                 URMResourceContextPtr context_id,
                                 IDBFile *file_id_return);
extern Cardinal UrmIFMGetModule(IDBFile file_id, const char *index,
                                URMResourceContextPtr context_id);

/* mrmptrlist.c */
extern Cardinal UrmPlistInit  _ARGUMENTS(( int size ,
					    URMPointerListPtr *list_id_return ));
extern Cardinal UrmPlistResize  _ARGUMENTS(( URMPointerListPtr list_id ,
						int size ));
extern Cardinal UrmPlistFree  _ARGUMENTS(( URMPointerListPtr list_id ));
extern Cardinal UrmPlistFreeContents  _ARGUMENTS(( URMPointerListPtr list_id ));
extern Cardinal UrmPlistAppendPointer  _ARGUMENTS(( URMPointerListPtr list_id ,
						    XtPointer ptr ));
extern Cardinal UrmPlistAppendString  _ARGUMENTS(( URMPointerListPtr list_id ,
						    String stg ));
extern MrmCount UrmPlistFindString  _ARGUMENTS(( URMPointerListPtr list_id ,
						    String stg ));

/* mrmtable.c */
extern Cardinal Urm__FixupCompressionTable  _ARGUMENTS(( UidCompressionTablePtr ctable ,
							    Boolean qfindcldesc,							    Boolean byte_swapped ));
extern Cardinal Urm__FindClassDescriptor  _ARGUMENTS(( IDBFile cfile ,
							MrmCode code ,
							char *name ,
							WCIClassDescPtr *class_return ));
extern Cardinal Urm__UncompressCode  _ARGUMENTS(( IDBFile cfile ,
						    MrmCode code ,
						    String *stg_return ));
extern Boolean Urm__IsSubtreeResource  _ARGUMENTS(( IDBFile cfile ,
						    MrmCode code ));

/* mrmtime.c */
extern void Urm__UT_Time  _ARGUMENTS(( char *time_stg ));

/* mrmvm.c */
extern String Urm__UT_AllocString  _ARGUMENTS(( String stg ));

/* mrmwci.c */
extern Cardinal Urm__WCI_RegisterNames  _ARGUMENTS(( String *names ,
							XtPointer *values ,
							MrmCount num_cb ));
extern Cardinal Urm__WCI_UnregisterName  _ARGUMENTS(( String name ));
extern Cardinal Urm__WCI_LookupClassDescriptor  _ARGUMENTS(( String class_name ,
								WCIClassDescPtr *class_return ));
extern WidgetClass Urm__WCI_GetClRecOfName  _ARGUMENTS(( String clname ));
extern Cardinal Urm__WCI_LookupRegisteredName  _ARGUMENTS(( String name ,
							    XtPointer *value_return ));
extern void hash_initialize  _ARGUMENTS(( URMHashTableEntryPtr *htable ,
					    Boolean *initflag ));
extern URMHashTableEntryPtr hash_find_name  _ARGUMENTS(( URMHashTableEntryPtr *htable ,
							    char *c_text ));
extern URMHashTableEntryPtr hash_insert_name  _ARGUMENTS(( URMHashTableEntryPtr *htable ,
							    char *c_text ));
extern URMHashTableEntryPtr hash_delete_name  _ARGUMENTS(( URMHashTableEntryPtr *htable ,
							    char *c_text ));

/* mrmwcrw.c */
extern Cardinal UrmCreateWidgetTree(URMResourceContextPtr context_id,
                                    Widget parent,
                                    MrmHierarchy hierarchy_id,
                                    IDBFile file_id,
                                    const char *ov_name,
                                    ArgList ov_args,
                                    Cardinal ov_num_args,
                                    MrmCode keytype,
                                    const char *kindex,
                                    MrmResource_id krid,
                                    MrmManageFlag manage,
                                    URMPointerListPtr *svlist,
                                    URMResourceContextPtr wref_id,
                                    Widget *w_return);
extern Cardinal UrmCreateWidgetInstance
                               _ARGUMENTS((URMResourceContextPtr context_id ,
					   Widget parent ,
					   MrmHierarchy hierarchy_id ,
					   IDBFile file_id ,
					   String ov_name ,
					   ArgList ov_args ,
					   Cardinal ov_num_args ,
					   MrmCode keytype ,
					   String kindex ,
					   MrmResource_id krid ,
					   MrmManageFlag manage,
					   URMPointerListPtr *svlist ,
					   URMResourceContextPtr wref_id ,
					   Widget *w_return,
					   char **w_name));
extern Cardinal UrmCreateOrSetWidgetInstance
                               _ARGUMENTS((URMResourceContextPtr context_id ,
					   Widget parent ,
					   MrmHierarchy hierarchy_id ,
					   IDBFile file_id ,
					   String ov_name ,
					   ArgList ov_args ,
					   Cardinal ov_num_args ,
					   MrmCode keytype ,
					   String kindex ,
					   MrmResource_id krid ,
					   MrmManageFlag manage ,
					   URMPointerListPtr *svlist ,
					   URMResourceContextPtr wref_id ,
					   Widget *w_return,
					   char **w_name));
extern Cardinal UrmSetWidgetInstance(URMResourceContextPtr context_id,
                                     Widget parent,
                                     MrmHierarchy hierarchy_id,
                                     IDBFile file_id,
                                     ArgList ov_args,
                                     Cardinal ov_num_args,
                                     MrmCode keytype,     /* unused */
                                     const char *kindex,  /* unused */
                                     MrmResource_id krid, /* unused */
                                     MrmManageFlag manage,
                                     URMPointerListPtr *svlist,
                                     URMResourceContextPtr wref_id,
                                     Widget *w_return);
extern void Urm__CW_CreateArglist  _ARGUMENTS(( Widget parent ,
						RGMWidgetRecordPtr widgetrec ,
						RGMArgListDescPtr argdesc ,
						URMPointerListPtr ctxlist ,
						URMPointerListPtr cblist,
					        URMPointerListPtr ftllist ,
						MrmHierarchy hierarchy_id ,
						IDBFile file_id ,
						ArgList args ,
						URMPointerListPtr *svlist ,
						URMResourceContextPtr wref_id ,
						Cardinal *num_used ));
extern long Urm__CW_EvaluateValOrOffset  _ARGUMENTS(( MrmType reptype ,
						XtPointer bufptr ,
						long ival ,
						MrmOffset offset ));
extern Cardinal Urm__CW_FixupValue  _ARGUMENTS(( long val ,
						 MrmType reptype ,
						 XtPointer bufptr ,
						 IDBFile file_id ,
						 Boolean *swap_needed));
extern String Urm__CW_DisplayToString _ARGUMENTS(( char *val ,
						    String add_string ,
						    int add_string_size ));
extern Cardinal Urm__CW_ConvertValue  _ARGUMENTS((  Widget parent,
						    long *val ,
						    MrmType reptype ,
						    MrmType cvttype ,
						    Display *display ,
						    MrmHierarchy hierarchy_id ,
						    URMPointerListPtr ftllist ));
extern void Urm__CW_SafeCopyValue  _ARGUMENTS(( long *val ,
						MrmType reptype ,
					        URMPointerListPtr cblist,
						int vec_count ,
					        int vec_size ));
extern void UrmDestroyCallback _ARGUMENTS((Widget w ,
					    URMPointerListPtr list_id ,
					    XmAnyCallbackStruct *reason));
extern Cardinal Urm__CW_ReadLiteral  _ARGUMENTS(( RGMResourceDescPtr resptr ,
						  MrmHierarchy hierarchy_id ,
						  IDBFile file_id ,
						  URMPointerListPtr ctxlist ,
						  MrmType *type ,
						  long *val ,
						  int *vec_count ,
						  IDBFile *act_file_id ,
						  int *vec_size ));
extern Cardinal Urm__CW_LoadIconImage  _ARGUMENTS(( RGMIconImagePtr iconptr ,
						    XtPointer bufptr ,
						    MrmHierarchy hierarchy_id ,
						    IDBFile file_id ,
						    URMPointerListPtr ctxlist ));
extern Cardinal Urm__CW_FixupCallback  _ARGUMENTS(( Widget parent ,
						    XtPointer bufptr ,
						    RGMCallbackDescPtr cbdesc ,
						    URMPointerListPtr ctxlist ,
						    URMPointerListPtr cblist ,
						    MrmHierarchy hierarchy_id ,
						    IDBFile file_id ,
						    URMResourceContextPtr   wref_id));
extern Cardinal Urm__CW_LoadWidgetResource  _ARGUMENTS(( Widget parent ,
						    RGMWidgetRecordPtr widgetrec ,
						    RGMResourceDescPtr resptr ,
						    URMPointerListPtr ctxlist ,
						    MrmHierarchy hierarchy_id ,
						    IDBFile file_id ,
						    URMPointerListPtr *svlist ,
						    URMResourceContextPtr wref_id ,
						    long *val ));
extern void Urm__CW_GetPixmapParms  _ARGUMENTS(( Widget w ,
						    Screen **screen ,
						    Display **display ,
						    Pixel *fgint ,
						    Pixel *bgint ));
extern RGMCallbackDescPtr
  Urm__CW_TranslateOldCallback _ARGUMENTS(( OldRGMCallbackDescPtr oldptr));

/* mrmwcrwr.c */
extern Cardinal UrmCWRInit  _ARGUMENTS(( URMResourceContextPtr context_id ,
					    String name ,
					    MrmCode access ,
					    MrmCode lock ));
extern Cardinal UrmCWRSetClass  _ARGUMENTS(( URMResourceContextPtr context_id,
					     MrmCode type,
					     String urm_class,
					     unsigned long variety ));
extern Cardinal UrmCWRInitArglist  _ARGUMENTS(( URMResourceContextPtr context_id ,
						Cardinal nargs ));
extern Cardinal UrmCWRSetCompressedArgTag  _ARGUMENTS(( URMResourceContextPtr context_id ,
							Cardinal arg_ndx ,
							MrmCode tag ,
							MrmCode related_tag ));
extern Cardinal UrmCWRSetUncompressedArgTag  _ARGUMENTS(( URMResourceContextPtr context_id ,
							    Cardinal arg_ndx ,
							    String tag ));
extern Cardinal UrmCWRSetArgValue  _ARGUMENTS(( URMResourceContextPtr context_id ,
						Cardinal arg_ndx ,
						MrmCode type ,
						unsigned long arg_val ));
extern Cardinal UrmCWRSetArgResourceRef  _ARGUMENTS(( URMResourceContextPtr context_id ,
							Cardinal arg_ndx ,
							MrmCode access ,
							MrmGroup group ,
							MrmCode type ,
							MrmCode key_type ,
							String index ,
							MrmResource_id resource_id ));
extern Cardinal UrmCWRSetArgChar8Vec  _ARGUMENTS(( URMResourceContextPtr context_id ,
						    Cardinal arg_ndx ,
						    String *stg_vec ,
						    MrmCount num_stg ));
extern Cardinal UrmCWRSetArgCStringVec  _ARGUMENTS(( URMResourceContextPtr context_id ,
							Cardinal arg_ndx ,
							XmString *cstg_vec ,
							MrmCount num_cstg ));
extern Cardinal UrmCWRSetArgCallback  _ARGUMENTS(( URMResourceContextPtr context_id ,
						    Cardinal arg_ndx ,
						    Cardinal nitems ,
						    MrmOffset *cb_offs_return ));
extern Cardinal UrmCWRSetCallbackItem  _ARGUMENTS(( URMResourceContextPtr context_id ,
						    MrmOffset cb_offs ,
						    Cardinal item_ndx ,
						    String routine ,
						    MrmCode type ,
						    unsigned long itm_val ));
extern Cardinal UrmCWRSetCallbackItemRes  _ARGUMENTS(( URMResourceContextPtr context_id ,
							MrmOffset cb_offs ,
							Cardinal item_ndx ,
							String routine ,
							MrmGroup group ,
							MrmCode access ,
							MrmCode type ,
							MrmCode key_type ,
							String index ,
							MrmResource_id resource_id ));
extern Cardinal UrmCWRSetExtraArgs  _ARGUMENTS(( URMResourceContextPtr context_id ,
						    Cardinal nextra ));
extern Cardinal UrmCWRInitChildren  _ARGUMENTS(( URMResourceContextPtr context_id ,
						    Cardinal nchildren ));
extern Cardinal UrmCWRSetChild  _ARGUMENTS(( URMResourceContextPtr context_id ,
						Cardinal child_ndx ,
						Boolean manage ,
						MrmCode access ,
						MrmCode key_type ,
						String index ,
						MrmResource_id resource_id ));
extern Cardinal UrmCWRSetComment  _ARGUMENTS(( URMResourceContextPtr context_id ,
						String comment ));
extern Cardinal UrmCWRSetCreationCallback  _ARGUMENTS(( URMResourceContextPtr context_id ,
							Cardinal nitems ,
							MrmOffset *cb_offs_return ));
extern Cardinal UrmCWR__AppendString  _ARGUMENTS(( URMResourceContextPtr context_id ,
						    String stg ,
						    MrmOffset *offset ));
extern Cardinal UrmCWR__AppendCString  _ARGUMENTS(( URMResourceContextPtr context_id ,
						    XmString cstg ,
						    MrmOffset *offset ));
extern Cardinal UrmCWR__AppendWcharString
                               _ARGUMENTS(( URMResourceContextPtr context_id ,
					    wchar_t *wcs ,
					    MrmOffset *offset ));
extern Cardinal UrmCWR__GuaranteeSpace  _ARGUMENTS(( URMResourceContextPtr context_id ,
							MrmSize delta ,
							MrmOffset *offset ,
							char **addr ));
extern Cardinal UrmCWR__AppendResource  _ARGUMENTS(( URMResourceContextPtr context_id ,
							MrmCode access ,
							MrmCode group ,
							MrmCode type ,
							MrmCode key_type ,
							String index ,
							MrmResource_id resource_id ,
							MrmOffset *offset ));
extern Cardinal UrmCWR__ValidateContext  _ARGUMENTS(( URMResourceContextPtr context_id ,
							String routine));
extern Cardinal UrmCWR__BindArgPtrs  _ARGUMENTS(( URMResourceContextPtr context_id ,
						    String routine ,
						    Cardinal argndx ,
						    RGMArgListDescPtr *descptr ,
						    RGMArgumentPtr *argptr ));
extern Cardinal UrmCWR__BindCallbackPtrs  _ARGUMENTS(( URMResourceContextPtr context_id ,
							String routine ,
							MrmOffset descoffs ,
							Cardinal itemndx ,
							RGMCallbackDescPtr *descptr ,
							RGMCallbackItemPtr *itmptr ));

/* mrmwread.c */
extern Cardinal UrmHGetWidget(MrmHierarchy hierarchy_id,
                              const char *index,
                              URMResourceContextPtr context_id,
                              IDBFile *file_id_return);
extern Cardinal UrmGetIndexedWidget(IDBFile file_id, const char *index,
                                    URMResourceContextPtr context_id);
extern Cardinal UrmGetRIDWidget  _ARGUMENTS(( IDBFile file_id ,
						MrmResource_id resource_id ,
						URMResourceContextPtr context_id ));

/* mrmwrefs.c */
extern Cardinal Urm__CW_InitWRef  _ARGUMENTS(( URMResourceContextPtr *wref_id ));
extern Cardinal Urm__CW_AddWRef  _ARGUMENTS(( URMResourceContextPtr wref_id ,
						String w_name ,
						Widget w_id ));
extern Cardinal Urm__CW_FindWRef  _ARGUMENTS(( URMResourceContextPtr wref_id ,
						String w_name ,
						Widget *w_id_return ));
extern void Urm__CW_AppendSVWidgetRef  _ARGUMENTS(( IDBFile file_id ,
						    URMPointerListPtr *svlist ,
						    String w_name ,
						    MrmCode argtag ,
						    String argname ));
extern void Urm__CW_AppendCBSVWidgetRef _ARGUMENTS((IDBFile file_id,
						    URMPointerListPtr *svlist,
						    RGMCallbackDescPtr cbptr,
						    MrmCode argtag,
						    String argname));
extern void Urm__CW_UpdateSVWidgetRef  _ARGUMENTS(( URMPointerListPtr *svlist ,
						    Widget cur_id ));
extern void Urm__CW_ResolveSVWidgetRef  _ARGUMENTS(( URMPointerListPtr *svlist ,
						    String cur_name ,
						    Widget cur_id ));

extern void Urm__CW_FreeSetValuesDesc _ARGUMENTS((URMSetValuesDescPtr svdesc));


/* mrmwvalues.c */
extern Cardinal UrmFetchSetValues  _ARGUMENTS(( MrmHierarchy hierarchy_id ,
						Widget w ,
						ArgList args ,
						Cardinal num_args ));

/* mrmwwrite.c */
extern Cardinal UrmPutIndexedWidget  _ARGUMENTS(( IDBFile file_id ,
						    String index ,
						    URMResourceContextPtr context_id ));
extern Cardinal UrmPutRIDWidget  _ARGUMENTS(( IDBFile file_id ,
						MrmResource_id resource_id ,
						URMResourceContextPtr context_id ));

/* mrmiswap.c */
extern Cardinal Idb__BM_SwapRecordBytes _ARGUMENTS(( IDBRecordBufferPtr	buffer ));
extern unsigned Urm__SwapValidation _ARGUMENTS(( unsigned validation ));
extern Cardinal Urm__SwapRGMResourceDesc _ARGUMENTS(( RGMResourceDescPtr res_desc ));
extern Cardinal Urm__SwapRGMCallbackDesc _ARGUMENTS(( RGMCallbackDescPtr callb_desc,
					      RGMWidgetRecordPtr widget_rec ));
extern Cardinal Urm__SwapRGMWidgetRecord _ARGUMENTS(( RGMWidgetRecordPtr widget_rec ));



/*
 * string copy macro
 */

/* If the alignment characteristics of your machine are right, these may be
   faster */

#ifdef UNALIGNED

#define UrmBCopy(src, dst, size)                            \
    if (size == sizeof(long))                               \
        *((long *) (dst)) = *((long *) (src));              \
    else if (size == sizeof(int))                           \
        *((int *) (dst)) = *((int *) (src));                \
    else if (size == sizeof(short))                         \
        *((short *) (dst)) = *((short *) (src));            \
    else if (size == sizeof(char))                          \
        *((char *) (dst)) = *((char *) (src));              \
    else                                                    \
        memmove((char *) (dst), (char *) (src), (int) (size));

#else

#define UrmBCopy(src, dst, size)                \
        memmove((char *) (dst), (char *) (src), (int) (size));

#endif /* UNALIGNED */

/********    Conditionally defined macros for thread_safe DtTerm ******/
#ifdef XTHREADS
#define _MrmWidgetToAppContext(w) \
        XtAppContext app = XtWidgetToApplicationContext(w)
#define _MrmDisplayToAppContext(d) \
        XtAppContext app = XtDisplayToApplicationContext(d)
#define _MrmAppLock(app) XtAppLock(app)
#define _MrmAppUnlock(app) XtAppUnlock(app)
#define _MrmProcessLock() XtProcessLock()
#define _MrmProcessUnlock() XtProcessUnlock()
#else /* XTHREADS */
#define _MrmWidgetToAppContext(w)
#define _MrmDisplayToAppContext(d)
#define _MrmAppLock(app)
#define _MrmAppUnlock(app)
#define _MrmProcessLock()
#define _MrmProcessUnlock()
#endif /* XTHREADS */

#endif /* Mrm_H */
/* DON'T ADD STUFF AFTER THIS #endif */
