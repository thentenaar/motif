/* $XConsortium: ScrollFrameT.c /main/5 1995/07/15 20:55:20 drk $ */
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
 * HISTORY
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include "XmI.h"
#include <Xm/TraitP.h>
#include <Xm/ScrollFrameT.h>
#include <Xm/NavigatorT.h>
#include "MessagesI.h"
#include "ScrollFramTI.h"

#define SWMessage3      _XmMMsgScrollFrameT_0000
#define SWMessage4      _XmMMsgScrollFrameT_0001



/************************************************************************
 *
 *  _XmSFAddNavigator convenience function
 *    Entering here, sf is an initialized scrollframe, and 
 *    scroll_frame_data is always valid (but the move_cb field can be 
 *    NULL, for pure APP_DEFINED case support).
 *
 *************************************<->***********************************/
void 
_XmSFAddNavigator(
    Widget sf,
    Widget nav,
    Mask dimMask,
    XmScrollFrameData scroll_frame_data)
{
    XmNavigatorTrait nav_trait ;
    XmNavigatorDataRec nav_data ;

    if ((nav_trait = (XmNavigatorTrait) 
	 XmeTraitGet((XtPointer) XtClass(nav), XmQTnavigator)) != NULL) {
	
	/* check for NULL move_cb */
	if (scroll_frame_data->move_cb)
	    nav_trait -> changeMoveCB(nav, scroll_frame_data->move_cb,
				      (XtPointer) scroll_frame_data->scrollable,
				      True) ;

	nav_data.valueMask = NavDimMask ;
	nav_data.dimMask = dimMask ;
	nav_trait -> setValue(nav, &nav_data, False);

	if (scroll_frame_data->num_nav_list == 
	    scroll_frame_data->num_nav_slots) {
	    /* Allocate more space */
	    scroll_frame_data->num_nav_slots +=  2;
	    scroll_frame_data->nav_list = 
		(WidgetList) XtRealloc((char*)scroll_frame_data->nav_list,
			scroll_frame_data->num_nav_slots * sizeof(Widget));
	}

	scroll_frame_data->nav_list[scroll_frame_data->num_nav_list] = nav;
	scroll_frame_data->num_nav_list++;

    } else {
	XmeWarning(sf, SWMessage3);
    }

}






/************************************************************************
 *
 *  _XmSFRemoveNavigator convenience function
 *
 *************************************<->***********************************/
void 
_XmSFRemoveNavigator(
    Widget sf,
    Widget nav,
    XmScrollFrameData scroll_frame_data)
{
    Cardinal	     position, i;
    XmNavigatorTrait nav_trait ;

    if ((nav_trait = (XmNavigatorTrait) 
	 XmeTraitGet((XtPointer) XtClass(nav), XmQTnavigator)) != NULL) {

	/* remove the move callback */
	if (scroll_frame_data->move_cb) 
	    nav_trait -> changeMoveCB(nav, scroll_frame_data->move_cb,
				(XtPointer) scroll_frame_data->scrollable,
				False) ;
    }  else {
	XmeWarning(sf, SWMessage4);
	return ;
    }

    for (position = 0; position<scroll_frame_data->num_nav_list; position++) {
        if (scroll_frame_data->nav_list[position] == nav) {
	    break;
	}
    }
    if (position == scroll_frame_data->num_nav_list) return;

    
    scroll_frame_data->num_nav_list--;
    for (i = position; i < scroll_frame_data->num_nav_list; i++) {
        scroll_frame_data->nav_list[i] = scroll_frame_data->nav_list[i+1];
    }

}


/************************************************************************
 *
 *  _XmSFUpdateNavigatorsValues convenience function
 *
 *************************************<->***********************************/
void
_XmSFUpdateNavigatorsValue(
        Widget sf,
        XmNavigatorData nav_data,
        Boolean notify)
{
    Cardinal i, num_nav_list ;
    Widget * nav_list ;
    Boolean inited ;

    /* there is a possibility that the SW was not inited for
       navigation business: APP_DEFINED where no scrollbar have
       been added yet */
    inited =  ((XmScrollFrameTrait) 
	       XmeTraitGet((XtPointer) XtClass(sf), XmQTscrollFrame))
	->getInfo(sf, NULL, &nav_list, &num_nav_list);

    if (!inited) return ;

    /* loop over the associated navigator list and call the change value
       method for each navigator */
    /* Updating the first navigator only if notify is True is not
       enough, since the dimension is pertinent */

    for (i=0; i < num_nav_list; i++) {
	Widget nav = nav_list[i] ;
	XmNavigatorSetValueProc	nav_setValue =
	    ((XmNavigatorTrait) 
	     XmeTraitGet((XtPointer) XtClass(nav), XmQTnavigator))->setValue;

	nav_setValue(nav, nav_data, notify);
    }
}

