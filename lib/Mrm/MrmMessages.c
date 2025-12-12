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
 *++
 *  FACILITY:
 *
 *      UIL Resource Manager (URM):
 *
 *  ABSTRACT:
 *
 *	All error declarations are found here.
 *
 *--
 */


/*
 *
 *  INCLUDE FILES
 *
 */

/* Define _MrmConst before including MrmDecl.h, so that the
 * declarations will be in agreement with the definitions.
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#ifndef _MrmConst
#define _MrmConst XmConst
#endif

#include <Mrm/MrmAppl.h>
#include "MrmMsgI.h"



#if 0				/* Comments below contain nested comments. */
/*===========================================================================*/
/* IMPORTANT NOTICE!! Please READ before editing this file!		     */
/*									     */
/* This file is also used to generate the XPG4 type message catalog.	     */
/* NO MESSAGES CAN BE REMOVED! If a message is no longer used, it can	     */
/* be removed from the object module by adding an #if 0 or /* above the      */
/* line(s) containing the message, and the corresponding #endif or 	     */
/* end-of-comment after the line(s).					     */
/*									     */
/* When ADDING messages to existing "sets" - a set is found inside /**** *****/
/* always add the new "externaldef(messages)" definition after everything    */
/* else in the set. Example:                                                 */
/*									     */
/* /**************** First.c ****************/
/*									     */
/* externaldef(mrmmsg) _MrmConst char *_MrmMsgFirst_0000 =		     */
/*    "Memory error";							     */
/*									     */
/* /* Needed for message catalog BC. Do not remove */
/* /***+MSG_First_1000 "Fake message."*/
/* /***+$ MSG_First_1000 message is obsolete - DO NOT localize this message.*/
/*									     */
/*									     */
/* /**************** Second.c ****************/
/*									     */
/* Messages added to the set "First" should be added after the line          */
/* /****+MSG_First_1000 "Fake message."*/
/* not after "Memory error".                                                 */
/*									     */
/* If you need to add another message "set", do so right before the          */
/* externaldef(mrmmsg) nl_catd Mrm_catd = NULL; definition.		     */
/*									     */
/*===========================================================================*/
#endif


/*
 * Text for message catalog header
 */
/***+$ This message catalog contains messages issued by Motif Mrm library.*/
/***+$ Consult X-Window systems, OSF/MOTIF Programmer's Guide and OSF/MOTIF*/
/***+$ Programmer's Reference Manual for technical terms if you have any*/
/***+$ doubts about their meanings.*/
/***+$ Do not translate variables such as %s, %d %s ... etc.*/
/***+$ Do not translate Motif resource name, such as XmNlabelType.*/
/***+$ Special terms with capital letter(s) should not be translated.*/
/***+$ "False" and "True" are keywords. Do not translate.*/
/***+ */
/***+ */


/***************** Mrm *********************/

/* Messages in the code */
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0000 =
	 "Vector allocation failed";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0001 =
	 "Buffer allocation failed";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0002 =
	 "Invalid buffer";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0003 =
	"Get block failed";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0004 =
	 "Put block failed";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0005 =
	 "Invalid record header";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0006 =
	 "Invalid context";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0007 =
	 "Invalid data entry";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0008 =
	 "Invalid segment entry";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0009 =
	 "Unknown data entry type";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0010 =
	 "Unexpected record type";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0011 =
	 "File version %s is greater than Mrm version %s.";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0012 =
	 "This is not the header record.";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0013 =
	 "Resource index out of range";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0014 =
	 "NULL data pointer";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0015 =
	 "Illegal overflow record";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0016 =
	 "GT record mismatch";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0017 =
	 "LT record mismatch";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0018 =
	 "Unexpected parent record type";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0019 =
	 "Record beyond EOF";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0020 =
	 "Cannot swap record #%d type %d";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0021 =
	 "Encountered unimplemented type %d, in Callback item %d";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0022 =
	 "Encountered unimplemented argument type %d";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0023 =
	 "NULL hierarchy id";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0024 =
	 "Invalid hierarchy";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0025 =
	 "Invalid interface module";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0026 =
	 "Invalid widget record";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0027 =
	 "Context allocation failed";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0028 =
	 "Validation failed";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0029 =
	 "Resource size too large";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0030 =
	 "Display not yet opened";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0031 =
	 "Could not open file %s";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0032 =
	 "Could not open file %s - UID file version mismatch";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0033 =
	 "XmGetPixmap failed with XmUNSPECIFIED_PIXMAP for %s";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0034 =
	 "XCreateImage failed during IconImage conversion";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0035 =
	 "XCreatePixmap failed during IconImage conversion";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0036 =
	 "XCreateGC failed during IconImage conversion";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0037 =
	 "XtMalloc failed during IconImage conversion";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0038 =
	 "Could not load color '%s'";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0039 =
	 "Could not convert RGB color/pixel '%d,%d,%d'";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0040 =
	 "Invalid color descriptor type";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0041 =
	 "Illegal Color item monochrome state %d";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0042 =
	 "Cannot find literal '%s'";
/***+$ literal in this context means a named value. */
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0043 =
	 "Invalid resource context";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0044 =
	 "Too many widgets";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0045 =
	 "Topmost index out of bounds";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0046 =
	 "List allocation failed";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0047 =
	 "List vector allocation failed";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0048 =
	 "Vector re-allocation failed";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0049 =
	 "String allocation failed";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0050 =
	 "UID file is obsolete - has no compression table";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0051 =
	 "Could not find class descriptor for class %s";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0052 =
	 "Cannot find indexed widget '%s'";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0053 =
	 "Cannot find RID widget '%x'";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0054 =
	 "?? UNKNOWN key type %d";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0055 =
	 "Unknown widget variety";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0056 =
	 "Unresolved Widget reference in creation callback";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0057 =
	 "Could not Fixup creation callbacks";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0058 =
	 "Unknown child type";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0059 =
	 "Child of parent not found";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0060 =
	 "Widget reference not Indexed";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0061 =
	 "Could not convert color/pixel '%s'";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0062 =
	 "Could not uncompress string code %d";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0063 =
	 "Unhandled resource group";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0064 =
	 "Could not convert ASCIZ '%s' to compound string";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0065 =
	 "Could not parse translation table '%s'";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0066 =
	 "Could not convert identifier '%s'";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0067 =
	 "Internal error: case MrmRtypeIconImage found";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0068 =
	 "Internal error: case MrmRtypeXBitmapFile found";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0069 =
	 "Internal error: Could not convert Display to String";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0070 =
	 "Could not convert font '%s'";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0071 =
	 "Could not convert fontset '%s'";
/***+$ please do not translate fontset.*/
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0072 =
	 "Could not open one or more fonts for fontset '%s'";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0073 =
	 "Could not add fontlist font '%s' to list";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0074 =
	 "Could not add fontlist fontset '%s' to list";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0075 =
	 "Could not convert class record name '%s'";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0076 =
	 "Could not convert keysym string '%s' to KeySym";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0077 =
	 "Cannot find indexed literal '%s'";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0078 =
	 "Cannot find RID literal '%x'";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0079 =
	 "Unknown literal key type %d";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0080 =
	 "Invalid ColorTable literal type %d";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0081 =
	 "Invalid ColorTable type code %d";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0082 =
	 "Invalid Color literal type %d";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0083 =
	 "Invalid ColorItem type code %d";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0084 =
	 "Callback routine '%s' not registered";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0085 =
	 "Widget reference not Indexed";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0086 =
	 "Cannot find indexed widget resource '%s'";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0087 =
	 "Cannot find RID widget resource '%x'";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0088 =
	 "Unknown resource key type %d";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0089 =
	 "Could not instantitate widget tree";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0090 =
	 "Null user-defined class name";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0091 =
	 "Invalid class code";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0092 =
	 "Too many arguments";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0093 =
	 "Invalid compression code";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0094 =
	 "Invalid or unhandled type";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0095 =
	 "Vector too big";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0096 =
	 "Empty routine name";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0097 =
	 "Char8Vector not yet implemented";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0098 =
	 "CStringVector not yet implemented";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0099 =
	 "Too many children";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0100 =
	 "Null children list descriptor";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0101 =
	 "Child index out of bounds";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0102 =
	 "Null index";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0103 =
	 "Invalid key type";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0104 =
	 "Arg index out of bounds";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0105 =
	 "Invalid widget/gadget record";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0106 =
	 "Invalid callback descriptor";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0107 =
	 "Callback item index out of bounds";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0108 =
	 "Could not uncompress string code %d";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0109 =
         "Too many items";
/* BEGIN OSF Fix CR 4859 */
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0110 =
       "Could not convert multibyte string to widecharacter string";
/* END OSF Fix CR 4859 */
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0111 =
       "Could not convert integer '%d' to icon image";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0112 =
       "Could not convert to icon image";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0113 =
       "Could not open buffer - UID version mismatch";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0114 =
       "Could not open buffer";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0115 =
       "Unable to convert units to XmPIXELS: bad units format or type";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0116 =
       "Unable to convert units to XmPIXELS: floating point overflow";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0117 =
       "Invalid version string %s in UID file.  Unable to read file.";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0118 =
	 "Mrm version %s is greater than file version %s;\nplease recompile UIL file.";
externaldef(mrmmsg) _MrmConst char *_MrmMsg_0119 =
       "Object '%s' declared PRIVATE.";

#if XM_MSGCAT
externaldef(mrmmsg) nl_catd Mrm_catd = NULL;
#endif
