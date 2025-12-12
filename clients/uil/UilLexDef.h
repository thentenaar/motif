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
**++
**  FACILITY:
**
**      User Interface Language Compiler (UIL)
**
**  ABSTRACT:
**
**      This include file defines the interface to the UIL lexical
**	analyzer.
**
**--
**/

#ifndef UilLexDef_h
#define UilLexDef_h


/*
**  Define flags to indicate whether certain characters are to be
**  filtered in text output.
*/

#define		lex_m_filter_tab	(1 << 0)

/*
**  Define the default character set.  In Motif, the default character set is
**  not isolatin1, but simply the null string, thus we must be able to
**  distinguish the two.
*/


#define lex_k_default_charset	    -1
#define lex_k_userdefined_charset   -2
#define lex_k_fontlist_default_tag  -3

/*
**  Since key_k_keyword_max_length assumes the length of the longest
**  WML generated keyword, we need a new constant to define the
**  longest allowable identifier.  This length should not exceed
**  URMMaxIndexLen.  (CR 5566)
*/

#define lex_k_identifier_max_length	31

#endif /* UilLexDef_h */
/* DON'T ADD STUFF AFTER THIS #endif */
