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

#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$TOG: UilSemCSet.c /main/10 1997/03/12 15:21:53 dbl $"
#endif
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

/*
**++
**  FACILITY:
**
**      User Interface Language Compiler (UIL)
**
**  ABSTRACT:
**
**      This file contains and routines related to the semantics of
**	character sets.
**	semantic validation.
**
**--
**/

/*
**
**  INCLUDE FILES
**
**/
#include <Xm/Xm.h>
#include "UilDefI.h"

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function returns a pointer to a character set name given
**	its sym_k_..._charset code and possibly a userdefined charset
**	value entry.
**
**  FORMAL PARAMETERS:
**
**	l_charset	charset of the string (token value)
**	az_charset_entry   charset of the string (symbol table value entry)
**
**  IMPLICIT INPUTS:
**
**      charset data tables
**
**  IMPLICIT OUTPUTS:
**
**      none
**
**  FUNCTION VALUE:
**
**      pointer to name (must NOT be freed)
**
**  SIDE EFFECTS:
**
**      error messages may be issued for objects that are still undefined
**	or of the wrong type
**
**--
**/
const char *sem_charset_name(int l_charset, sym_value_entry_type *az_charset_entry)
{

int		charset;		/* mapped character set */

charset = sem_map_subclass_to_charset (l_charset);
switch ( charset )
    {
    case sym_k_fontlist_default_tag:
      return XmFONTLIST_DEFAULT_TAG;
    case sym_k_userdefined_charset:
        /*
	 ** If the charset is user-defined, then fetch info from the symbol
	 ** table entry for it.
	 */
	_assert (az_charset_entry!=NULL, "null userdefined charset entry");
	return az_charset_entry->value.c_value;
    default:
	return charset_xmstring_names_table[charset];
    }
}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function returns information about the character set specified
**	(writing direction and sixteen_bit properties).
**
**  FORMAL PARAMETERS:
**
**	l_charset	charset of the string (token value)
**	az_charset_entry   charset of the string (symbol table value entry)
**	direction	string writing direction
**	sixteen_bit	Boolean return value
**
**  IMPLICIT INPUTS:
**
**      none
**
**  IMPLICIT OUTPUTS:
**
**      none
**
**  FUNCTION VALUE:
**
**      none
**
**  SIDE EFFECTS:
**
**      none
**
**--
**/
void sem_charset_info(int l_charset, sym_value_entry_type *az_charset_entry,
                      int *write_direction, int *parse_direction,
                      int *sixteen_bit)
{

int		charset;	/* mapped character set */

charset = sem_map_subclass_to_charset (l_charset);
switch (charset)
    {
    /*
     ** If the charset is user-defined, then fetch info from the symbol
     ** table entry for it.
     */
    case sym_k_userdefined_charset:
        {
	*write_direction = az_charset_entry->b_direction;
	*parse_direction = az_charset_entry->b_direction;
	*sixteen_bit = (az_charset_entry->b_aux_flags &
			sym_m_sixteen_bit) != 0;
	break;
	}
    default:
	{
	*write_direction = charset_writing_direction_table[charset];
	*parse_direction = charset_parsing_direction_table[charset];
	if ( charset_character_size_table[charset] != sym_k_onebyte_charsize )
	    *sixteen_bit = TRUE;
	else
	    *sixteen_bit = FALSE;
	break;
	}
	}
}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This function returns the charset id corresponding to the
**	charset specified in a $LANG variable. User-defined character
**	sets are not recognized - it must be one available in the
**	the compiler tables.
**
**	The name match is case-insensitive.
**
**  FORMAL PARAMETERS:
**
**	lang_charset	string naming a character set
**
**  IMPLICIT INPUTS:
**
**      none
**
**  IMPLICIT OUTPUTS:
**
**      none
**
**  FUNCTION VALUE:
**
**      0	no match - character set not found
**	>0	character set code from sym_k_..._charset.
**		sym_k_userdefined_charset is never returned.
**
**
**  SIDE EFFECTS:
**
**      none
**
**--
**/
int sem_charset_lang_name(const char *lang_charset)
{
	size_t i;
	char uname[256];

	/*
	 * Convert name to upper case, then search table (which is already in
	 * upper case).
	 */
	if (!lang_charset || !*lang_charset)
		return 0;

	for (i = 0; i < strlen(lang_charset) && i < sizeof uname - 1; i++)
		uname[i] = _upper(lang_charset[i]);
	uname[i] = '\0';

	for (i = 0; i < charset_lang_table_max; i++)
		if (!strcmp(uname, charset_lang_names_table[i]))
			return charset_lang_codes_table[i];
	return 0;
}

