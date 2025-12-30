/* $XConsortium: TextFind.c /main/6 1995/09/19 23:17:17 cde-sun $ */
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
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <ctype.h>
#include <X11/Xmd.h>
#include <Xm/XmosP.h>		/* for <stdlib.h>, and wcstombs() */
#include "XmI.h"
#include "TextI.h"
#include "TextStrSoI.h"

Boolean
_XmTextFindStringBackwards(Widget w,
			   XmTextPosition start,
			   char* search_string,
			   XmTextPosition *position)
{
  int i;
  XmTextWidget tw = (XmTextWidget) w;
  XmSourceData data = ((XmTextWidget)w)->text.source->data;
  Boolean return_val = False, match = False;
  int search_length = 0;
  char *ptr, *end_ptr, *tmp_ptr, *end_of_data;

  search_length = _XmTextCountCharacters(search_string,strlen(search_string));

  if (!search_length || !data->length || search_length > data->length)
    return FALSE;

  /* Search can be broken into three phases for fastest search:
   *
   *    - search from data_end - strlen(search_string) back until
   *      base search is at gap_end.  This is a fast simple compare
   *      that doesn't worry about incursions into the gap.
   *
   *    - search from gap_start-strlen(search_string) until the base
   *      of the search is moved across the gap.
   *
   *    - search from start up to gap_start-strlen(search_string).
   *      This is a fast simple compare that doesn't worry about
   *      incursions into the gap.
   */

  switch ((int)tw->text.char_size) {
  case 1: {

    /* Make sure you don't search past end of data. End of data is...  */
    end_of_data = data->ptr + data->length +
      (data->gap_end - data->gap_start);

    /* actually, no need to search beyond end - search_length position */
    if (end_of_data - search_length >= data->gap_end)
      end_ptr = end_of_data - search_length;
    else
      end_ptr = data->gap_start -
	(search_length - (end_of_data - data->gap_end));

    /* Phase one: search from start back to gap_end */
    /* Set the base for the search */
    if (data->ptr + start > data->gap_start) /* backside of gap */
      ptr = data->ptr + (data->gap_end - data->gap_start) + start;
    else /* we're starting before the gap */
      ptr = data->ptr + start;

    if (ptr > end_ptr)
      ptr = end_ptr; /* no need search where a match can't be found */

    while (!return_val && ptr >= data->gap_end) {
      if (*ptr == *search_string) { /* potential winner! */
	for (match = True, i = 1; match && (i < search_length); i++) {
	  if (ptr[i] != search_string[i]) {
	    match = False;
	    i--;
	  }
	}
	if (i == search_length) { /* we have a winner! */
	  *position = ptr - data->ptr -
	    (data->gap_end - data->gap_start);
	  return_val = True;
	}
      }
      ptr--; /* decrement base of search */
      match = True;
    }

    /* Phase two: these searches span the gap and are SLOW!
     * Two possibilities: either I've just backed the base back to gap
     * end (and must do searches that span the gap) or start puts
     * the base prior to the gap end.  Also, possibility that there
     * isn't enough room after the gap for a complete match
     * (so no need to search it).  This phase must be performed as
     * long as data->ptr + start places the base to the right of
     * gap_start - search_length.
     */

    /* Do this as nested for loops; the outer loop decrements the base for
     * the search, the inner loop compares character elements from 0 to
     * length(search_string).
     */

    /* If no match yet and if need to search prior to gap_start... */
    /* Set the base for the search. */

    if (!return_val &&
	(data->ptr + start) > (data->gap_start - search_length)) {
      if (data->ptr < data->gap_start)
	ptr = data->gap_start - 1;
      /* else, we're done... gap_start is at data->ptr and still no match*/

      for (match = True;
	   ptr >= data->ptr && (data->gap_start - ptr) +
	   (end_of_data - data->gap_end) >= search_length;
	   ptr--, match = True) {

	if (*ptr == *search_string) { /* we have a potential winner */
	  for (i = 1; i < search_length && match == True; i ++) {
	    if (ptr + i >= data->gap_start) {
	      tmp_ptr = ptr +(data->gap_end - data->gap_start) + i;
	      if (*tmp_ptr != search_string[i]) {
		match = False;
		i--;
	      }
	    } else {
	      if (ptr[i] != search_string[i]) {
		match = False;
		i--;
	      }
	    }
	  } /* end inner for loop - searching from current base */
	  if (match && (i == search_length)) {/* a winner! */
	    *position = ptr - data->ptr;
	    return_val = True;
	  }
	}
	if (return_val) break;
      } /* end outer for - restart search from a new base */
    }

    /* phase three: search backwards from base == gap_start - search_length
     * through and including base == data->ptr.
     */

    if (!return_val) {
      if (data->ptr + start > data->gap_start - search_length)
	ptr = data->gap_start - search_length;
      else
	ptr = data->ptr + start;

      while (!return_val && ptr >= data->ptr) {
	if (*ptr == *search_string) { /* potential winner! */
	  for (match = True, i = 1; match && (i < search_length); i++) {
	    if (ptr[i] != search_string[i]) {
	      match = False;
	      i--;
	    }
	  }
	  if (i == search_length) { /* we have a winner! */
	    *position = ptr - data->ptr;
	    return_val = True;
	  }
	}
	ptr--; /* decrement base of search */
	match = True;
      }
    }
    break;
  } /* end case 1 */
  case 2: {
    BITS16 *bits16_ptr, *bits16_search_string, *bits16_gap_start;
    BITS16 *bits16_gap_end, *bits16_end_ptr, *bits16_tmp_ptr;
    BITS16 *bits16_end_of_data;
    bits16_ptr = bits16_search_string = NULL;
    bits16_gap_start = bits16_gap_end = NULL;

    /* search_length is number of characters (!bytes) in search_string */
    bits16_search_string =
      (BITS16 *) XtMalloc((unsigned)
			  (search_length + 1) * (int)tw->text.char_size);
    (void) _XmTextBytesToCharacters((char *) bits16_search_string,
				    search_string, search_length, True,
				    (int)tw->text.char_size);

    /* setup the variables for the search */
    bits16_gap_start = (BITS16 *) data->gap_start;
    bits16_gap_end = (BITS16 *) data->gap_end;

    /* Make sure you don't search past end of data. End of data is...  */
    bits16_ptr = (BITS16 *)data->ptr;
    bits16_end_of_data = bits16_ptr + data->length +
      (bits16_gap_end - bits16_gap_start);

    /* only need to search up to end - search_length position */
    if (bits16_end_of_data - search_length >= bits16_gap_end)
      bits16_end_ptr = bits16_end_of_data - search_length;
    else
      bits16_end_ptr = bits16_gap_start -
	(search_length - (bits16_end_of_data - bits16_gap_end));

    /* Phase one: search from start back to gap_end */

    bits16_ptr = (BITS16 *)data->ptr;
    if (bits16_ptr + start > bits16_gap_start) /* backside of gap */
      bits16_ptr = (BITS16 *)data->ptr +
	(bits16_gap_end - bits16_gap_start) + start;
    else /* we're starting before the gap */
      bits16_ptr = (BITS16 *)data->ptr + start;

    /* no need search where a match can't be found */
    if (bits16_ptr > bits16_end_ptr)
      bits16_ptr = bits16_end_ptr;

    while (!return_val && bits16_ptr >= bits16_gap_end) {
      if (*bits16_ptr == *bits16_search_string) { /* potential winner! */
	for (match = True, i = 1; match && (i < search_length); i++) {
	  if (bits16_ptr[i] != bits16_search_string[i]) {
	    match = False;
	    i--;
	  }
	}
	if (i == search_length) { /* we have a winner! */
	  *position = bits16_ptr - (BITS16 *)data->ptr -
	    (bits16_gap_end - bits16_gap_start);
	  return_val = True;
	}
      }
      bits16_ptr--; /* decrement base of search */
      match = True;
    }

    /* Phase two: these searches span the gap and are SLOW!
     * Two possibilities: either I've just backed the base back to gap
     * end (and must do searches that span the gap) or start puts
     * the base prior to the gap end.  Also, possibility that there
     * isn't enough room after the gap for a complete match
     * (so no need to search it).  This phase must be performed as
     * long as data->ptr + start places the base to the right of
     * gap_start - search_length.
     */

    /* Do this as nested for loops; the outer loop decrements the base for
     * the search, the inner loop compares character elements from 0 to
     * length(search_string).
     */

    /* If no match yet and if need to search prior to gap_start... */
    /* Set the base for the search. */

    bits16_ptr = (BITS16 *) data->ptr;
    if (!return_val &&
	(bits16_ptr + start) > (bits16_gap_start - search_length)) {
      if (bits16_ptr < bits16_gap_start)
	bits16_ptr = bits16_gap_start - 1;
      /* else, we're done... gap_start is at data->ptr and still no match*/

      for (match = True;
	   bits16_ptr >= (BITS16*)data->ptr &&
	   (bits16_gap_start - bits16_ptr) +
	   (bits16_end_of_data - bits16_gap_end) >= search_length;
	   bits16_ptr--, match = True) {

	if (*bits16_ptr == *bits16_search_string) { /* potential winner */
	  for (i = 1; i < search_length && match == True; i ++) {
	    if (bits16_ptr + i >= bits16_gap_start) {
	      bits16_tmp_ptr = bits16_ptr +
		(bits16_gap_end - bits16_gap_start) + i;
	      if (*bits16_tmp_ptr != bits16_search_string[i]) {
		match = False;
		i--;
	      }
	    } else {
	      if (bits16_ptr[i] != bits16_search_string[i]) {
		match = False;
		i--;
	      }
	    }
	  } /* end inner for loop - searching from current base */
	  if (match && (i == search_length)) {/* a winner! */
	    *position = bits16_ptr - (BITS16*)data->ptr;
	    return_val = True;
	  }
	}
	if (return_val) break;
      } /* end outer for - restart search from a new base */
    }

    /* phase three: search backwards from base == gap_start - search_length
     * through and including base == data->ptr.
     */

    if (!return_val) {
      bits16_ptr = (BITS16 *) data->ptr;
      if (bits16_ptr + start > bits16_gap_start - search_length)
	bits16_ptr = bits16_gap_start - search_length;
      else
	bits16_ptr = bits16_ptr + start;

      while (!return_val && bits16_ptr >= (BITS16 *)data->ptr) {
	if (*bits16_ptr == *bits16_search_string) {/* potential winner!*/
	  for (match = True, i = 1; match && (i < search_length); i++) {
	    if (bits16_ptr[i] != bits16_search_string[i]) {
	      match = False;
	      i--;
	    }
	  }
	  if (i == search_length) { /* we have a winner! */
	    *position = bits16_ptr - (BITS16 *)data->ptr;
	    return_val = True;
	  }
	}
	bits16_ptr--; /* decrement base of search */
	match = True;
      }
    }
    /* clean up before you go */
    if (bits16_search_string != NULL)
      XtFree((char*)bits16_search_string);
    break;
  } /* end case 2 */
  default: {
    wchar_t *wchar_t_ptr, *wchar_t_search_string, *wchar_t_gap_start;
    wchar_t *wchar_t_gap_end, *wchar_t_end_ptr, *wchar_t_tmp_ptr;
    wchar_t *wchar_t_end_of_data;
    wchar_t_ptr = wchar_t_search_string = NULL;
    wchar_t_gap_start = wchar_t_gap_end = NULL;

    wchar_t_search_string =
      (wchar_t *) XtMalloc((unsigned)
			   (search_length + 1) * sizeof(wchar_t));
    (void)_XmTextBytesToCharacters((char *) wchar_t_search_string,
				   search_string, search_length, True,
				   (int)tw->text.char_size);

    /* setup the variables for the search of new lines before the gap */
    wchar_t_gap_start = (wchar_t *) data->gap_start;
    wchar_t_gap_end = (wchar_t *) data->gap_end;

    /* Make sure you don't search past end of data. End of data is...  */
    wchar_t_ptr = (wchar_t *)data->ptr;
    wchar_t_end_of_data = wchar_t_ptr + data->length +
      (wchar_t_gap_end - wchar_t_gap_start);

    /* only need to search up to end - search_length position */
    if (wchar_t_end_of_data - search_length >= wchar_t_gap_end)
      wchar_t_end_ptr = wchar_t_end_of_data - search_length;
    else
      wchar_t_end_ptr = wchar_t_gap_start -
	(search_length - (wchar_t_end_of_data - wchar_t_gap_end));

    /* Phase one: search from start back to gap_end */

    wchar_t_ptr = (wchar_t *)data->ptr;
    if (wchar_t_ptr + start > wchar_t_gap_start) /* backside of gap */
      wchar_t_ptr = (wchar_t *)data->ptr +
	(wchar_t_gap_end - wchar_t_gap_start) + start;
    else /* we're starting before the gap */
      wchar_t_ptr = (wchar_t *)data->ptr + start;

    /* no need search where a match can't be found */
    if (wchar_t_ptr > wchar_t_end_ptr)
      wchar_t_ptr = wchar_t_end_ptr;

    while (!return_val && wchar_t_ptr >= wchar_t_gap_end) {
      if (*wchar_t_ptr == *wchar_t_search_string) { /* potential winner! */
	for (match = True, i = 1; match && (i < search_length); i++) {
	  if (wchar_t_ptr[i] != wchar_t_search_string[i]) {
	    match = False;
	    i--;
	  }
	}
	if (i == search_length) { /* we have a winner! */
	  *position = wchar_t_ptr - (wchar_t *)data->ptr -
	    (wchar_t_gap_end - wchar_t_gap_start);
	  return_val = True;
	}
      }
      wchar_t_ptr--; /* decrement base of search */
      match = True;
    }

    /* Phase two: these searches span the gap and are SLOW!
     * Two possibilities: either I've just backed the base back to gap
     * end (and must do searches that span the gap) or start puts
     * the base prior to the gap end.  Also, possibility that there
     * isn't enough room after the gap for a complete match
     * (so no need to search it).  This phase must be performed as
     * long as data->ptr + start places the base to the right of
     * gap_start - search_length.
     */

    /* Do this as nested for loops; the outer loop decrements the base for
     * the search, the inner loop compares character elements from 0 to
     * length(search_string).
     */

    /* If no match yet and if need to search prior to gap_start... */
    /* Set the base for the search. */

    wchar_t_ptr = (wchar_t *) data->ptr;
    if (!return_val &&
	(wchar_t_ptr + start) > (wchar_t_gap_start - search_length)) {
      if (wchar_t_ptr < wchar_t_gap_start)
	wchar_t_ptr = wchar_t_gap_start - 1;
      /*else, we're done... gap_start is at data->ptr and still no match*/

      for (match = True;
	   wchar_t_ptr >= (wchar_t*)data->ptr &&
	   (wchar_t_gap_start - wchar_t_ptr) +
	   (wchar_t_end_of_data - wchar_t_gap_end) >= search_length;
	   wchar_t_ptr--, match = True) {

	if (*wchar_t_ptr == *wchar_t_search_string) {/* potential winner */
	  for (i = 1; i < search_length && match == True; i ++) {
	    if (wchar_t_ptr + i >= wchar_t_gap_start) {
	      wchar_t_tmp_ptr = wchar_t_ptr +
		(wchar_t_gap_end - wchar_t_gap_start) + i;
	      if (*wchar_t_tmp_ptr != wchar_t_search_string[i]) {
		match = False;
		i--;
	      }
	    } else {
	      if (wchar_t_ptr[i] != wchar_t_search_string[i]) {
		match = False;
		i--;
	      }
	    }
	  } /* end inner for loop - searching from current base */
	  if (match && (i == search_length)) {/* a winner! */
	    *position = wchar_t_ptr - (wchar_t*)data->ptr;
	    return_val = True;
	  }
	}
	if (return_val) break;
      } /* end outer for - restart search from a new base */
    }

    /* phase three: search backwards from base == gap_start - search_length
     * through and including base == data->ptr.
     */

    if (!return_val) {
      wchar_t_ptr = (wchar_t *) data->ptr;
      if (wchar_t_ptr + start > wchar_t_gap_start - search_length)
	wchar_t_ptr = wchar_t_gap_start - search_length;
      else
	wchar_t_ptr = wchar_t_ptr + start;

      while (!return_val && wchar_t_ptr >= (wchar_t *)data->ptr) {
	if (*wchar_t_ptr == *wchar_t_search_string) {/* potential winner!*/
	  for (match = True, i = 1; match && (i < search_length); i++) {
	    if (wchar_t_ptr[i] != wchar_t_search_string[i]) {
	      match = False;
	      i--;
	    }
	  }
	  if (i == search_length) { /* we have a winner! */
	    *position = wchar_t_ptr - (wchar_t *)data->ptr;
	    return_val = True;
	  }
	}
	wchar_t_ptr--; /* decrement base of search */
	match = True;
      }
    }
    /* clean up before you go */
    if (wchar_t_search_string != NULL)
      XtFree((char*)wchar_t_search_string);
    break;
  } /* end default */
  } /* end switch */

  return return_val;
}


Boolean
_XmTextFindStringForwards(Widget w,
			  XmTextPosition start,
			  char* search_string,
			  XmTextPosition *position)
{
  int i;
  XmTextWidget tw = (XmTextWidget) w;
  XmSourceData data = tw->text.source->data;
  Boolean return_val = False, match = False;
  int search_length = 0;
  char *ptr, *end_ptr, *tmp_ptr, *end_of_data;

  search_length = _XmTextCountCharacters(search_string,strlen(search_string));

  if (!search_length || !data->length || search_length > data->length)
    return FALSE;

  /* Search can be broken into three phases for fastest search:
   *
   *    - search from start up to gap_start-strlen(search_string).
   *      This is a fast simple compare that doesn't worry about
   *      incursions into the gap.
   *
   *    - search from gap_start-strlen(search_string) until the base
   *      of the search is moved across the gap.
   *
   *    - search from gap_end to data_end - strlen(search_string).
   *      This is a fast, simple compare that doesn't worry about
   *      incursions into the gap or overrunning end of data.
   */

  switch ((int)tw->text.char_size) {
  case 1: {

    /* Make sure you don't search past end of data. End of data is...  */
    end_of_data = data->ptr + data->length +
      (data->gap_end - data->gap_start);

    /* actually, only need to search up to end - search_length position */
    if (end_of_data - search_length >= data->gap_end)
      end_ptr = end_of_data - search_length;
    else
      end_ptr = data->gap_start -
	(search_length - (end_of_data - data->gap_end));

    /* Phase one: search from start to gap_start-strlen(search_string) */

    ptr = data->ptr + start;
    while (!return_val && ptr + search_length <= data->gap_start) {
      if (*ptr == *search_string) { /* potential winner! */
	for (match = True, i = 1; match && (i < search_length); i++) {
	  if (ptr[i] != search_string[i]) {
	    match = False;
	    i--;
	  }
	}
	if (i == search_length) { /* we have a winner! */
	  *position = ptr - data->ptr;
	  return_val = True;
	}
      }
      ptr++; /* advance base of search */
      match = True;
    }

    /* Phase two: these searches span the gap and are SLOW!
     * Two possibilities: either I'm just short of the gap
     * (and must do searches that span the gap) or start puts
     * the base after gap end.  Also, possibility that there
     * isn't enough room after the gap for a complete match
     * (so no need to search it).
     */

    /* Do this as nested for loops; the outer loop advances the base for
     * the search, the inner loop compares character elements from 0 to
     * length(search_string).
     */

    /* if no match yet and if need to search prior to gap_start... */

    if (!return_val && (data->ptr + start) < data->gap_start) {
      if (data->ptr + start < data->gap_start - search_length)
	ptr = data->gap_start - search_length;
      else
	ptr = data->ptr + start;

      for (match = True;
	   ptr < data->gap_start && (data->gap_start - ptr) +
	   (end_of_data - data->gap_end) >= search_length;
	   ptr++, match = True) {

	if (*ptr == *search_string) { /* we have a potential winner */
	  for (i = 1; i < search_length && match == True; i ++) {
	    if (ptr + i >= data->gap_start) {
	      tmp_ptr = ptr +(data->gap_end - data->gap_start) + i;
	      if (*tmp_ptr != search_string[i]) {
		match = False;
		i--;
	      }
	    } else {
	      if (ptr[i] != search_string[i]) {
		match = False;
		i--;
	      }
	    }
	  } /* end inner for loop - searching from current base */
	  if (match && (i == search_length)) {/* a winner! */
	    *position = ptr - data->ptr;
	    return_val = True;
	  }
	}
	if (return_val) break;
      } /* end outer for - restart search from a new base */
    }
    /* phase three: search after gap end upto end of data - search_length */

    if (!return_val) {
      if (data->ptr + start < data->gap_start)
	ptr = data->gap_end; /* we've already started - continue at
			      * gap end. */
      else
	ptr = data->ptr + (data->gap_end - data->gap_start) + start;

      while (!return_val && ptr <= end_ptr) {
	if (*ptr == *search_string) { /* potential winner! */
	  for (match = True, i = 1; match && (i < search_length); i++) {
	    if (ptr[i] != search_string[i]) {
	      match = False;
	      i--;
	    }
	  }
	  if (i == search_length) { /* we have a winner! */
	    *position = ptr - data->ptr -
	      (data->gap_end - data->gap_start);
	    return_val = True;
	  }
	}
	ptr++; /* advance base of search */
	match = True;
      }
    }
  } /* end case 1 */
    break;
  case 2: {
    BITS16 *bits16_ptr, *bits16_search_string, *bits16_gap_start;
    BITS16 *bits16_gap_end, *bits16_end_ptr, *bits16_tmp_ptr;
    BITS16 *bits16_end_of_data;
    bits16_ptr = bits16_search_string = NULL;
    bits16_gap_start = bits16_gap_end = NULL;

    /* search_length is number of characters (!bytes) in search_string */
    bits16_search_string =
      (BITS16 *) XtMalloc((unsigned)
			  (search_length + 1) * (int)tw->text.char_size);
    (void) _XmTextBytesToCharacters((char *) bits16_search_string,
				    search_string, search_length, True,
				    (int)tw->text.char_size);

    /* setup the variables for the search */
    bits16_gap_start = (BITS16 *) data->gap_start;
    bits16_gap_end = (BITS16 *) data->gap_end;

    /* Make sure you don't search past end of data. End of data is...  */
    bits16_ptr = (BITS16 *)data->ptr;
    bits16_end_of_data = bits16_ptr + data->length +
      (bits16_gap_end - bits16_gap_start);

    /* only need to search up to end - search_length position */
    if (bits16_end_of_data - search_length >= bits16_gap_end)
      bits16_end_ptr = bits16_end_of_data - search_length;
    else
      bits16_end_ptr = bits16_gap_start -
	(search_length - (bits16_end_of_data - bits16_gap_end));

    /* Phase one: search from start to gap start - search_length */

    bits16_ptr = (BITS16 *)data->ptr + start;
    while (!return_val && bits16_ptr + search_length <= bits16_gap_start) {
      if (*bits16_ptr == *bits16_search_string) { /* potential winner! */
	for (match = True, i = 1; match && (i < search_length); i++) {
	  if (bits16_ptr[i] != bits16_search_string[i]) {
	    match = False;
	    i--;
	  }
	}
	if (i == search_length) { /* we have a winner! */
	  *position = bits16_ptr - (BITS16 *)data->ptr;
	  return_val = True;
	}
      }
      bits16_ptr++; /* advance base of search */
      match = True;
    }

    /* Phase two: these searches span the gap and are SLOW!
     * Two possibilities: either I'm just short of the gap
     * (and must do searches that span the gap) or start puts
     * the base after gap end.  Also, possibility that there
     * isn't enough room after the gap for a complete match
     * (so no need to search it).
     */

    /* Do this as nested for loops; the outer loop advances the base for
     * the search, the inner loop compares character elements from 0 to
     * length(search_string).
     */

    /* if no match yet and if need to search prior to gap_start... */

    bits16_ptr = (BITS16 *) data->ptr;
    if (!return_val && (bits16_ptr + start) < bits16_gap_start) {
      if (bits16_ptr + start < bits16_gap_start - search_length)
	bits16_ptr = bits16_gap_start - search_length;
      else
	bits16_ptr = (BITS16*)data->ptr + start;

      for (match = True;
	   bits16_ptr < bits16_gap_start &&
	   (bits16_gap_start - bits16_ptr) +
	   (bits16_end_of_data - bits16_gap_end) >= search_length;
	   bits16_ptr++, match = True) {

	if (*bits16_ptr == *bits16_search_string)
	  { /* have a potential winner*/
	    for (i = 1; i < search_length && match == True; i ++) {
	      if (bits16_ptr + i >= bits16_gap_start) {
		bits16_tmp_ptr = bits16_ptr +
		  (bits16_gap_end - bits16_gap_start) + i;
		if (*bits16_tmp_ptr != bits16_search_string[i]) {
		  match = False;
		  i--;
		}
	      } else {
		if (bits16_ptr[i] != bits16_search_string[i]) {
		  match = False;
		  i--;
		}
	      }
	    } /* end inner for loop - searching from current base */
	    if (match && (i == search_length)) { /* a winner! */
	      *position = bits16_ptr - (BITS16*)data->ptr;
	      return_val = True;
	    }
	  }
	if (return_val) break;
      } /* end outer for - restart search from a new base */
    }

    /* phase three: search after gap end upto end of data - search_length */

    if (!return_val) {
      bits16_ptr = (BITS16 *) data->ptr;
      if (bits16_ptr + start < bits16_gap_start)
	bits16_ptr = bits16_gap_end; /* we've already started...
				      * continue at gap end. */
      else
	bits16_ptr = (BITS16*)data->ptr +
	  (bits16_gap_end - bits16_gap_start) + start;

      while (!return_val && bits16_ptr <= bits16_end_ptr) {
	if (*bits16_ptr == *bits16_search_string)
	  { /* potential winner! */
	    for (match = True, i=1; match && (i < search_length); i++) {
	      if (bits16_ptr[i] != bits16_search_string[i]) {
		match = False;
		i--;
	      }
	    }
	    if (i == search_length) { /* we have a winner! */
	      *position = bits16_ptr - (BITS16 *)data->ptr -
		(bits16_gap_end - bits16_gap_start);
	      return_val = True;
	    }
	  }
	bits16_ptr++; /* advance base of search */
	match = True;
      }
    }
    /* clean up before you go */
    if (bits16_search_string != NULL)
      XtFree((char*)bits16_search_string);
    break;
  } /* end case 2 */
  default: {
    wchar_t *wchar_t_ptr, *wchar_t_search_string, *wchar_t_gap_start;
    wchar_t *wchar_t_gap_end, *wchar_t_end_ptr, *wchar_t_tmp_ptr;
    wchar_t *wchar_t_end_of_data;
    wchar_t_ptr = wchar_t_search_string = NULL;
    wchar_t_gap_start = wchar_t_gap_end = NULL;

    wchar_t_search_string =
      (wchar_t *) XtMalloc((unsigned)
			   (search_length + 1) * sizeof(wchar_t));
    (void)_XmTextBytesToCharacters((char *) wchar_t_search_string,
				   search_string, search_length, True,
				   (int)tw->text.char_size);

    /* setup the variables for the search of new lines before the gap */
    wchar_t_gap_start = (wchar_t *) data->gap_start;
    wchar_t_gap_end = (wchar_t *) data->gap_end;

    wchar_t_ptr = (wchar_t *)data->ptr;
    /* Make sure you don't search past end of data. End of data is...  */
    wchar_t_end_of_data = wchar_t_ptr + data->length +
      (wchar_t_gap_end - wchar_t_gap_start);

    /* only need to search up to end - search_length position */
    if (wchar_t_end_of_data - search_length >= wchar_t_gap_end)
      wchar_t_end_ptr = wchar_t_end_of_data - search_length;
    else
      wchar_t_end_ptr = wchar_t_gap_start -
	(search_length - (wchar_t_end_of_data - wchar_t_gap_end));

    /* Phase one: search from start to gap start - search_length */

    wchar_t_ptr = (wchar_t *)data->ptr + start;
    while (!return_val && wchar_t_ptr + search_length <= wchar_t_gap_start) {
      if (*wchar_t_ptr == *wchar_t_search_string) { /* potential winner! */
	for (match = True, i = 1; match && (i < search_length); i++) {
	  if (wchar_t_ptr[i] != wchar_t_search_string[i]) {
	    match = False;
	    i--;
	  }
	}
	if (i == search_length) { /* we have a winner! */
	  *position = wchar_t_ptr - (wchar_t *)data->ptr;
	  return_val = True;
	}
      }
      wchar_t_ptr++; /* advance base of search */
      match = True;
    }

    /* Phase two: these searches span the gap and are SLOW!
     * Two possibilities: either I'm just short of the gap
     * (and must do searches that span the gap) or start puts
     * the base after gap end.  Also, possibility that there
     * isn't enough room after the gap for a complete match
     * (so no need to search it).
     */

    /* Do this as nested for loops; the outer loop advances the base for
     * the search, the inner loop compares character elements from 0 to
     * length(search_string).
     */

    /* if no match yet and if need to search prior to gap_start... */

    wchar_t_ptr = (wchar_t *) data->ptr;
    if (!return_val && (wchar_t_ptr + start) < wchar_t_gap_start) {
      if (wchar_t_ptr + start < wchar_t_gap_start - search_length)
	wchar_t_ptr = wchar_t_gap_start - search_length;
      else
	wchar_t_ptr = (wchar_t*)data->ptr + start;

      for (match = True;
	   wchar_t_ptr < wchar_t_gap_start &&
	   (wchar_t_gap_start - wchar_t_ptr) +
	   (wchar_t_end_of_data - wchar_t_gap_end) >= search_length;
	   wchar_t_ptr++, match = True) {

	if (*wchar_t_ptr == *wchar_t_search_string) {
	  /* have a potential winner */
	  for (i = 1; i < search_length && match == True; i ++) {
	    if (wchar_t_ptr + i >= wchar_t_gap_start) {
	      wchar_t_tmp_ptr = wchar_t_ptr +
		(wchar_t_gap_end - wchar_t_gap_start) + i;
	      if (*wchar_t_tmp_ptr != wchar_t_search_string[i]) {
		match = False;
		i--;
	      }
	    } else {
	      if (wchar_t_ptr[i] != wchar_t_search_string[i]) {
		match = False;
		i--;
	      }
	    }
	  } /* end inner for loop - searching from current base */
	  if (match && (i == search_length)) { /* a winner! */
	    *position = wchar_t_ptr - (wchar_t*)data->ptr;
	    return_val = True;
	  }
	}
	if (return_val) break;
      } /* end outer for - restart search from a new base */
    }

    /* phase three: search after gap end upto end of data - search_length */

    if (!return_val) {
      wchar_t_ptr = (wchar_t *) data->ptr;
      if (wchar_t_ptr + start < wchar_t_gap_start)
	wchar_t_ptr = wchar_t_gap_end; /* we've already started...
					* continue at gap end. */
      else
	wchar_t_ptr = (wchar_t*)data->ptr +
	  (wchar_t_gap_end - wchar_t_gap_start) + start;

      while (!return_val && wchar_t_ptr <= wchar_t_end_ptr) {
	if (*wchar_t_ptr == *wchar_t_search_string) {
	  /* potential winner!*/
	  for (match = True, i = 1; match && (i < search_length); i++) {
	    if (wchar_t_ptr[i] != wchar_t_search_string[i]) {
	      match = False;
	      i--;
	    }
	  }
	  if (i == search_length) { /* we have a winner! */
	    *position = wchar_t_ptr - (wchar_t *)data->ptr -
	      (wchar_t_gap_end - wchar_t_gap_start);
	    return_val = True;
	  }
	}
	wchar_t_ptr++; /* advance base of search */
	match = True;
      }
    }
    /* clean up before you go */
    if (wchar_t_search_string != NULL)
      XtFree((char*)wchar_t_search_string);
    break;
  } /* end default */
  } /* end switch */
  return return_val;
}

Boolean
XmTextFindString(Widget w,
		 XmTextPosition start,
		 char* search_string,
		 XmTextDirection direction,
		 XmTextPosition *position)
{
  XmSourceData data;
  Boolean ret_val;
  _XmWidgetToAppContext(w);

  if (XmIsTextField(w)) return False;

  _XmAppLock(app);
  data = ((XmTextWidget)w)->text.source->data;
  if (start > data->length)
    start = data->length;
  else if (start < 0)
    start = 0;

  if (direction == XmTEXT_BACKWARD)
    ret_val =  _XmTextFindStringBackwards(w, start, search_string, position);
  else
    ret_val = _XmTextFindStringForwards(w, start, search_string, position);

  _XmAppUnlock(app);
  return (ret_val);

}

Boolean
XmTextFindStringWcs(Widget w,
		    XmTextPosition start,
		    wchar_t* wc_string,
		    XmTextDirection direction,
		    XmTextPosition *position)
{
  wchar_t *tmp_wc;
  char *string;
  int num_chars = 0;
  Boolean return_val = False;
  XmTextWidget tw = (XmTextWidget) w;
  int wcs_ret_val = 0;
  _XmWidgetToAppContext(w);

  _XmAppLock(app);
  if (!XmIsTextField(w)) {
    for (num_chars = 0, tmp_wc = wc_string; *tmp_wc != (wchar_t)0L;
	 num_chars++) tmp_wc++;
    string = XtMalloc ((unsigned) (num_chars + 1) * (int)tw->text.char_size);
    wcs_ret_val = wcstombs(string, wc_string,
			   (num_chars + 1) * (int)tw->text.char_size);
    if (wcs_ret_val >= 0)
      return_val = XmTextFindString(w, start, string, direction, position);
    XtFree(string);
    _XmAppUnlock(app);
    return(return_val);
  } else {
    _XmAppUnlock(app);
    return False;
  }
}

