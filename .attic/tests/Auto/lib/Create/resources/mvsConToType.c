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
static char rcsid[] = "$XConsortium: mvsConToType.c /main/8 1995/07/14 11:19:33 drk $"
#endif
#endif
/***********************************************************************
  	@(#)mvsConToType.c	1.11.1.1	Date:1/22/91
  	Author: TAT
	History:
	   03/30/90 SJS Add to sccs tree
	   04/20/90 SJS Add XmNavigationType
	   05/29/90 SJS Add more types
	   07/23/90 SJS Eliminate the silliness of OXmNavigationType
	Calls:

	Summary:
		This function converts the value contained in an
	MvsArgVal type variable into the requested type.
	An MvsArgVal is the sizeof the largest data type in a widget
	resource set, so it is necessary to type cast in order to
	extract values. The type casting is done so as to extract the
	appropriate value.
************************************************************************/
#include <AutoMessages.h>
#include "mvslib.h"

MvsArgVal mvsConvertToType(value, type)
MvsArgVal value;
int type;
{
    MvsArgVal return_value;
    char msg_string[125];

    switch(type) {
      case T_XtTranslations:
        return_value = (MvsArgVal)(*((XtTranslations *)&value));
        break;
      case T_Boolean:       
        return_value = (MvsArgVal)(*((Boolean *)&value));
        break;
      case T_Pixel:         
        return_value = (MvsArgVal)(*((Pixel *)&value));
        break;
      case T_XtProc:        /* This type needs fixing!!! AES */
        return_value = (MvsArgVal)(*((caddr_t *)&value));
        break;
      case T_Pixmap:        
        return_value = (MvsArgVal)(*((Pixmap *)&value));
        break;
      case T_Dimension:     
        return_value = (MvsArgVal)(*((Dimension *)&value));
        break;
      case T_Colormap:      
        return_value = (MvsArgVal)(*((Colormap *)&value));
        break;
      case T_XtCallbackList:
        return_value = (MvsArgVal)(*((XtCallbackList *)&value));
        break;
      case T_XtEnum:
        return_value = (MvsArgVal)(*((XtEnum *)&value));
        break;
      case T_Pointer:        
        return_value = (MvsArgVal)(*((Pointer *)&value));
        break;
      case T_Position:      
        return_value = (MvsArgVal)(*((Position *)&value));
        break;
      case T_Integer:     
        return_value = (MvsArgVal)(*((Integer *)&value));
        break;
      case T_caddr_t:       
        return_value = (MvsArgVal)(*((caddr_t *)&value));
        break;
      case T_unsigned_char:       
        return_value = (MvsArgVal)(*((unsigned char *)&value));
        break;
      case T_short:       
        return_value = (MvsArgVal)(*((short *)&value));
        break;
      case T_String:
        return_value = (MvsArgVal)(*((String *)&value));       
        break;
      case T_Widget_ref:       
        return_value = (MvsArgVal)(*((Widget *)&value));       
        break;
      case T_XmFontList:       
        return_value = (MvsArgVal)(*((XmFontList *)&value));       
        break;
      case T_XmString:       
        return_value = (MvsArgVal)(*((XmString *)&value));
        break;
      case T_XmStringTable:       
        return_value = (MvsArgVal)(*((XmStringTable *)&value));
        break;
      case T_char:       
        return_value = (MvsArgVal)(*((char *)&value));
        break;
      case T_XmNavigationType:       
        return_value = (MvsArgVal)(*((XmNavigationType *)&value));
        break;
      case T_XmStringDirection:       
        return_value = (MvsArgVal)(*((XmStringDirection *)&value));
        break;
      case T_KeySym:       
        return_value = (MvsArgVal)(*((KeySym *)&value));
        break;
      case T_WidgetClass_ref:
        return_value = (MvsArgVal)(*((WidgetClass *)&value));
        break;
      case T_Cardinal:
        return_value = (MvsArgVal)(*((Cardinal *)&value));
        break;
      case T_WidgetList:      
        return_value = (MvsArgVal)(*((WidgetList *)&value));
        break;
      case T_XmFunction:        /* This type needs fixing!!! AES */
        return_value = (MvsArgVal)(*((caddr_t *)&value));
        break;
      case T_XmTextSource:        /* This type needs fixing!!! AES */
        return_value = (MvsArgVal)(*((WidgetClass *)&value));
        break;
      case T_XmTextPosition:        
        return_value = (MvsArgVal)(*((XmTextPosition *)&value));
        break;
      case T_XtAccelerators:
        return_value = (MvsArgVal)(*((XtAccelerators *)&value));
        break;
      case T_XmDirection:
	return_value = (MvsArgVal)(*((XmDirection *)&value));
	break;
      case T_XmRenderTable:
	return_value = (MvsArgVal)(*((XmRenderTable *)&value));
	break;
/*  Removed from Xm source
 *    case T_XmBitArray:
 *	return_value = (MvsArgVal)(*((XmBitArray *)&value));
 *	break;
 */
      case T_XmTabList:
	return_value = (MvsArgVal)(*((XmTabList *)&value));
	break;
      case T_XmViewType:
	return_value = (MvsArgVal)(*((unsigned char *)&value));
	break;
      case T_XmVisualEmphasis:
	return_value = (MvsArgVal)(*((unsigned char *)&value));
	break;
      case T_unsigned_int:
	return_value = (MvsArgVal)(*((unsigned int *)&value));
	break;
      default:
	sprintf (msg_string, _AutoMessages[WARNMSG65], type);
        AutoMessage(msg_string);
        return_value = value;
        break;
    } /* End switch() */

    return(return_value);

} /* End mvsConvertToType() */
