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
#ifndef MrmPublic
#define MrmPublic

#include <X11/Intrinsic.h>
#include <X11/Xmd.h>

/*
 * Success or other non-error return codes
 */
#define	MrmSUCCESS	1
#define MrmCREATE_NEW	3
#define	MrmINDEX_RETRY	5	/* Retry on entering index required */
#define	MrmINDEX_GT	7	/* Index orders greater-than entry */
#define	MrmINDEX_LT	9	/* Index orders less-than entry */
#define MrmPARTIAL_SUCCESS	11	/* operation partly succeeded */

/*
 * Failure return codes
 */
#define	MrmFAILURE	0
#define	MrmNOT_FOUND	2
#define	MrmEXISTS	4
#define	MrmNUL_GROUP	6
#define	MrmNUL_TYPE	8
#define	MrmWRONG_GROUP	10
#define	MrmWRONG_TYPE	12
#define	MrmOUT_OF_RANGE	14	/* Record number too big */
#define	MrmBAD_RECORD	16	/* Record number wrong type */
#define	MrmNULL_DATA	18	/* No data for entry */
#define	MrmBAD_DATA_INDEX	20	/* Data index in RID out of range */
#define	MrmBAD_ORDER	22	/* Bad ordering specifier */
#define	MrmBAD_CONTEXT	24	/* Invalid Mrm context */
#define	MrmNOT_VALID	26	/* Validation failure */
#define	MrmBAD_BTREE	28	/* GT/LT pointer error in BTree */
#define	MrmBAD_WIDGET_REC	30	/* Validation failure on widget record */
#define	MrmBAD_CLASS_TYPE	32	/* Class type not a valid Mrmwc... value */
#define	MrmNO_CLASS_NAME	34	/* User class name is null */
#define	MrmTOO_MANY	36	/* Too many entries requested in some list */
#define	MrmBAD_IF_MODULE	38	/* invalid interface module */
#define	MrmNULL_DESC	40	/* Arglist or children descriptor null */
#define	MrmOUT_OF_BOUNDS	42	/* Argument index out of arglist bounds */
#define	MrmBAD_COMPRESS	44	/* Invalid compression code */
#define	MrmBAD_ARG_TYPE	46	/* Invalid type, not in RGMrType... */
#define	MrmNOT_IMP	48	/* Not yet implemented */
#define	MrmNULL_INDEX	50	/* empty index string */
#define	MrmBAD_KEY_TYPE	52	/* key must be MrmrIndex or MrmrRID */
#define	MrmBAD_CALLBACK	54	/* Invalid callback descriptor */
#define	MrmNULL_ROUTINE	56	/* Empty callback routine name string */
#define	MrmVEC_TOO_BIG	58	/* too many elements in vector */
#define	MrmBAD_HIERARCHY	60	/* invalid Mrm file hierarchy */
#define	MrmBAD_CLASS_CODE	62	/* Class code not found in Mrmwc... */
#define MrmDISPLAY_NOT_OPENED	63	/* Display not yet created */
#define MrmEOF			64      /* End of file */
#define MrmUNRESOLVED_REFS	65	/* Unresolved widget refs in callback*/

/*
 * Definitions associated with widget representations
 */

/*
 * Define the creation callback
 */
#define MrmNcreateCallback	"createCallback"
/* BEGIN OSF Fix pir 2813 */
#define MrmCR_CREATE		XmCR_CREATE
/* END OSF Fix pir 2813 */

/*
 * Code for unknown (user-defined) classes.
 */
#define	MrmwcUnknown		1


/*
 * The data types of values stored in uid files.
 */
#define	MrmRtypeMin		1
#define	MrmRtypeInteger		1	/* int */
#define	MrmRtypeBoolean		2
#define	MrmRtypeChar8		3	/* a nul-terminated string */
#define	MrmRtypeChar8Vector	4	/* a vector of char_8 strings */
#define	MrmRtypeCString		5	/* a compound string (DDIS) */
#define	MrmRtypeCStringVector	6	/* a vector of compound strings */
#define	MrmRtypeFloat		7
					/* 8 = TypeCompressed now unused */
#define	MrmRtypeCallback	9	/* code for a callback descriptor */
#define	MrmRtypePixmapImage	10	/* Pixmap in image form */
#define	MrmRtypePixmapDDIF	11	/* Pixmap in DDIF form */
#define	MrmRtypeResource	12	/* Mrm resource descriptor */
#define	MrmRtypeNull		13	/* no value given */
#define	MrmRtypeAddrName	14	/* nul-terminated string to be
					   interpreted as runtime address */
#define	MrmRtypeIconImage	15	/* icon image */
#define	MrmRtypeFont		16	/* Mrm font structure */
#define	MrmRtypeFontList	17	/* Mrm font list */
#define	MrmRtypeColor		18	/* Mrm color descriptor */
#define	MrmRtypeColorTable	19	/* Mrm color table */
#define	MrmRtypeAny		20	/* Any is allowed in UID file */
#define MrmRtypeTransTable	21	/* Translation table (ASCIZ string) */
#define MrmRtypeClassRecName	22	/* class record name (ASCIZ string) */
#define MrmRtypeIntegerVector	23	/* a vector of integers */
#define MrmRtypeXBitmapFile	24	/* X bitmap file to make pixmap with */
#define MrmRtypeCountedVector	25	/* vector with associated count */
#define MrmRtypeKeysym		26	/* X keysym data type */
#define MrmRtypeSingleFloat	27	/* single float data type */
#define MrmRtypeWideCharacter	28	/* wide_character string type */
#define MrmRtypeFontSet		29
#define MrmRtypeHorizontalInteger 30
#define MrmRtypeVerticalInteger   31
#define MrmRtypeHorizontalFloat   32
#define MrmRtypeVerticalFloat     33
#define	MrmRtypeMax		33


/*
 * MRM typedefs
 */

/*
 * MRM primitive types
 */
typedef short int		MrmCode;  /* Used for codes, e.g. Mrmcr... */
typedef	unsigned char		MrmSCode; /* Short code for small ranges */
typedef unsigned short int	MrmOffset;/* Used for offsets in records */
typedef short int		MrmType;  /* Used for types, e.g. MrmrType... */
typedef unsigned short int	MrmSize;  /* For size fields */
typedef short int		MrmCount; /* For counter fields */
typedef unsigned char		MrmFlag;  /* flag fields */
typedef long int		MrmResource_id; /* Resource id in portable IDB files */
typedef short int		MrmGroup; /* For Mrm resource groups */

#define	MrmMaxResourceSize	65535	/* (2)16 - 1 */

/*
 * Operating System specific parameter struct.  Passed to low level
 * file OPEN.
 */
#define MrmOsOpenParamVersion 1
typedef struct	{
/*
 * The version of this structure. Set to MrmOsOpenParamVersion
 */
	Cardinal		version;
/*
 * The default name, for systems which can use it
 */
	char			*default_fname;
/*
 * The related name, for systems which can use it
 * Flag for file clobber (1 means OK to clobber)
 */
	union {
	    unsigned long	related_nam;
	    Boolean		clobber_flg;
	} nam_flg;

/*
 * The display, for which the user wants us to open
 * this hierarchy on. Used for the LANG variable.
 */
	Display			*display;

} MrmOsOpenParam, *MrmOsOpenParamPtr ;

/*
 * The opaque result of opening a Mrm hierarchy.
 */
typedef struct MrmHierarchyDescStruct *MrmHierarchy;

/*
 * Structure used to pass name/value pairs to MrmRegisterNames
 */
typedef struct {
	const char *name; /* case-sensitive name */
	XPointer value;  /* value/address associated with name */
} MRMRegisterArg, MrmRegisterArg, *MrmRegisterArglist ;

/*
 * Code for unknown (user-defined) classes.
 */
#define	URMwcUnknown	1


/* Widget creation / cleanup callbacks */
typedef Widget (*MrmWidgetCreateProc)(Widget parent, char *name, ArgList args, Cardinal num);
typedef void (*MrmWidgetCleanupProc)(void *);

/*
 * Declarations of Mrm public entry points
 */
#include <Mrm/MrmDecls.h>

#endif /* MrmPublic */
/* DON'T ADD STUFF AFTER THIS #endif */
