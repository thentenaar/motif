/* 
 * Motif
 *
 * Copyright (c) 1987-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
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
 * HISTORY
*/ 
#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$XConsortium: font_compare.c /main/4 1995/07/13 20:35:53 drk $"
#endif
#endif

#include <stdio.h>

#include <X11/Intrinsic.h>
#include <X11/Xlib.h>

#define MIN(a, b)   ((a) < (b) ? (a) : (b))

void	add_err_msg();

void compare_fonts(font1, font2, errors, error_message)
     XFontStruct	*font1,
     			*font2;
     int		*errors;
     String		error_message;
{
    int		per_char_errors = 0,
    		num1, num2,
    		i;

    char	err_msg[80];
    
    XCharStruct	*ptr1,
    		*ptr2;

    *errors = 0;

    if((font1 == NULL) || (font2 == NULL))
    {
	sprintf(err_msg,"NULL font(s)       \t");
	add_err_msg(error_message, err_msg);
	
	if(font1 == NULL)
	{
	    (*errors)++;

	    sprintf(err_msg,"  NULL  \t");
	}
	else
	{
	    sprintf(err_msg,"non-NULL\t");
	}
	add_err_msg(error_message, err_msg);

	if(font2 == NULL)
	{
	    (*errors)++;

	    sprintf(err_msg,"  NULL  \n");
	}
	else
	{
	    sprintf(err_msg,"non-NULL\n");
	}
	add_err_msg(error_message, err_msg);

	return;
    }

    if(font1->direction != font2->direction)
    {
	(*errors)++;

	sprintf(err_msg,"direction          \t%8u\t%8u\n",
		font1->direction, font2->direction);
	add_err_msg(error_message, err_msg);
    }
	    
    if(font1->min_char_or_byte2 != font2->min_char_or_byte2)
    {
	(*errors)++;

	sprintf(err_msg,"min_char_or_byte2  \t%8u\t%8u\n",
		font1->min_char_or_byte2, font2->min_char_or_byte2);
	add_err_msg(error_message, err_msg);
    }

    if(font1->max_char_or_byte2 != font2->max_char_or_byte2)
    {
	(*errors)++;

	sprintf(err_msg,"max_char_or_byte2  \t%8u\t%8u\n",
		font1->max_char_or_byte2, font2->max_char_or_byte2);
	add_err_msg(error_message, err_msg);
    }

    if(font1->min_byte1 != font2->min_byte1)
    {
	(*errors)++;

	sprintf(err_msg,"min_byte1          \t%8u\t%8u\n",
		font1->min_byte1, font2->min_byte1);
	add_err_msg(error_message, err_msg);
    }

    if(font1->max_byte1 != font2->max_byte1)
    {
	(*errors)++;

	sprintf(err_msg,"max_byte1          \t%8u\t%8u\n",
		font1->max_byte1, font2->max_byte1);
	add_err_msg(error_message, err_msg);
    }

    if(font1->all_chars_exist != font2->all_chars_exist)
    {
	(*errors)++;

	sprintf(err_msg,"all_chars_exist    \t%8d\t%8d\n",
		font1->all_chars_exist, font2->all_chars_exist);
	add_err_msg(error_message, err_msg);
    }

    if(font1->default_char != font2->default_char)
    {
	(*errors)++;

	sprintf(err_msg,"default_char       \t%8u\t%8u\n",
		font1->default_char, font2->default_char);
	add_err_msg(error_message, err_msg);
    }

    if(font1->n_properties != font2->n_properties)
    {
	(*errors)++;

	sprintf(err_msg,"n_properties       \t%8d\t%8d\n",
		font1->n_properties, font2->n_properties);
	add_err_msg(error_message, err_msg);
    }

    if(font1->min_bounds.lbearing != font2->min_bounds.lbearing)
    {
	(*errors)++;

	sprintf(err_msg,"min_bounds.lbearing\t%8d\t%8d\n",
		font1->min_bounds.lbearing, font2->min_bounds.lbearing);
	add_err_msg(error_message, err_msg);
    }

    if(font1->min_bounds.rbearing != font2->min_bounds.rbearing)
    {
	(*errors)++;

	sprintf(err_msg,"min_bounds.rbearing\t%8d\t%8d\n",
		font1->min_bounds.rbearing, font2->min_bounds.rbearing);
	add_err_msg(error_message, err_msg);
    }

    if(font1->min_bounds.width != font2->min_bounds.width)
    {
	(*errors)++;

	sprintf(err_msg,"min_bounds.width   \t%8d\t%8d\n",
		font1->min_bounds.width, font2->min_bounds.width);
	add_err_msg(error_message, err_msg);
    }

    if(font1->min_bounds.ascent != font2->min_bounds.ascent)
    {
	(*errors)++;

	sprintf(err_msg,"min_bounds.ascent  \t%8d\t%8d\n",
		font1->min_bounds.ascent, font2->min_bounds.ascent);
	add_err_msg(error_message, err_msg);
    }

    if(font1->min_bounds.descent != font2->min_bounds.descent)
    {
	(*errors)++;

	sprintf(err_msg,"min_bounds.descent \t%8d\t%8d\n",
		font1->min_bounds.descent, font2->min_bounds.descent);
	add_err_msg(error_message, err_msg);
    }

    if(font1->max_bounds.lbearing != font2->max_bounds.lbearing)
    {
	(*errors)++;

	sprintf(err_msg,"max_bounds.lbearing\t%8d\t%8d\n",
		font1->max_bounds.lbearing, font2->max_bounds.lbearing);
	add_err_msg(error_message, err_msg);
    }

    if(font1->max_bounds.rbearing != font2->max_bounds.rbearing)
    {
	(*errors)++;

	sprintf(err_msg,"max_bounds.rbearing\t%8d\t%8d\n",
		font1->max_bounds.rbearing, font2->max_bounds.rbearing);
	add_err_msg(error_message, err_msg);
    }

    if(font1->max_bounds.width != font2->max_bounds.width)
    {
	(*errors)++;

	sprintf(err_msg,"max_bounds.width   \t%8d\t%8d\n",
		font1->max_bounds.width, font2->max_bounds.width);
	add_err_msg(error_message, err_msg);
    }

    if(font1->max_bounds.ascent != font2->max_bounds.ascent)
    {
	(*errors)++;

	sprintf(err_msg,"max_bounds.ascent  \t%8d\t%8d\n",
		font1->max_bounds.ascent, font2->max_bounds.ascent);
	add_err_msg(error_message, err_msg);
    }

    if(font1->max_bounds.descent != font2->max_bounds.descent)
    {
	(*errors)++;

	sprintf(err_msg,"max_bounds.descent \t%8d\t%8d\n",
		font1->max_bounds.descent, font2->max_bounds.descent);
	add_err_msg(error_message, err_msg);
    }

    if(font1->ascent != font2->ascent)
    {
	(*errors)++;

	sprintf(err_msg,"ascent           \t%8d\t%8d\n",
		font1->ascent, font2->ascent);
	add_err_msg(error_message, err_msg);
    }

    if(font1->descent != font2->descent)
    {
	(*errors)++;

	sprintf(err_msg,"descent          \t%8d\t%8d\n",
		font1->descent, font2->descent);
	add_err_msg(error_message, err_msg);
    }

    ptr1 = font1->per_char;
    ptr2 = font2->per_char;

    /*
     * Don't do any more testing if the per_char is null on both. 
     */

    if((ptr1 == NULL) && (ptr2 == NULL))
    {
   	return;
    }

    if((ptr1 == NULL) || (ptr2 == NULL))
    {
	per_char_errors++;
    }
    else
    {
	num1 = font1->max_char_or_byte2 - font1->min_char_or_byte2;
	num2 = font2->max_char_or_byte2 - font2->min_char_or_byte2;

	for(i=0 ; i<MIN(num1, num2) ; ptr1++, ptr2++, i++)
	{
	    if((ptr2->lbearing != ptr1->lbearing) ||
	       (ptr2->rbearing != ptr1->rbearing) ||
	       (ptr2->width != ptr1->width) ||
	       (ptr2->ascent != ptr1->ascent) ||
	       (ptr2->descent != ptr1->descent))
	    {
		per_char_errors++;
	    }
	}
    }
    
    if(num1 != num2)
    {
	per_char_errors++;
    }
    
    if(per_char_errors)
    {
	*errors += per_char_errors;

	sprintf(err_msg, "per_char        \t different per_char data\n");
	add_err_msg(error_message, err_msg);
    }
}

void add_err_msg(error_message, err_msg)
     String	error_message,
     		err_msg;
{

    if(error_message != NULL)
    {
	strcat(error_message, err_msg);
    }
}
