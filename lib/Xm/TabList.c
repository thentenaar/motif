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
 *
 */

#include <Xm/Xm.h>
#include <Xm/Ext.h>
#include <Xm/TabList.h>

typedef struct _XmTabbedStackListRec {
    int    allocated;
    int    used;
    XmTabAttributes tabs;
} XmTabbedStackListRec;

#define XmLIST_GROWTH_FACTOR 10
#define XiXmStringCopy(s) (((s) == (XmString)NULL) ? NULL : XmStringCopy(s))
#define XiXmStringCompare(s1,s2) (((s1) == (s2)) ? True \
				  : ((s1) != NULL && (s2) != NULL) \
				  ? XmStringCompare(s1,s2) : False)
#define XiXmStringFree(s) if((s)!=(XmString)NULL)XmStringFree(s);else(s)=NULL

/*
 * Function:
 *      TabListCreate(void)
 * Description:
 *      Allocates and initializes an empty XmTabbedStackList structure and returns
 *      a pointer to the structure.
 * Input:
 *      None.
 * Output:
 *      XmTabbedStackList - an empty allocated XiTablist
 */
XmTabbedStackList XmTabbedStackListCreate(void)
{
    XmTabbedStackList tabList;

    /*
     * Allocate the structure and zero it.  Then give it back to the
     * caller.
     */
    tabList = (XmTabbedStackList) XtMalloc(sizeof(XmTabbedStackListRec));
    tabList->allocated = tabList->used = 0;
    tabList->tabs = (XmTabAttributes) NULL;

    return( tabList );
}

/*
 * Function:
 *      XmTabbedStackListCopy(tab_list)
 * Description:
 *      This makes a copy of the given XmTabbedStackList and returns the
 *      copy.
 * Input:
 *      tab_list : XmTabbedStackList - the XmTabbedStackList to copy
 * Output:
 *      XmTabbedStackList - the copy of the given list
 */
XmTabbedStackList XmTabbedStackListCopy(XmTabbedStackList tab_list)
{
    XmTabbedStackList newList;
    int       i;

    /*
     * First if they gave us nothing, then lets return nothing to
     * them.
     */
    if( tab_list == NULL ) return( NULL );

    /*
     * Now allocate the new structure and set its allocated and used
     * to the number of items we need.
     */
    newList = (XmTabbedStackList) XtMalloc(sizeof(XmTabbedStackListRec));
    newList->allocated = newList->used = tab_list->used;

    /*
     * Now check if we have any items and if not just zero the
     * data, else we will have to copy item by item.
     */
    if( newList->used == 0 )
    {
        newList->tabs = (XmTabAttributes) NULL;
    }
    else
    {
        /*
         * First allocate the memory for all the entries. and then copy
         * item by item the list.
         */
        newList->tabs = (XmTabAttributes)
            XtMalloc(sizeof(XmTabAttributeRec) * newList->used);

        for( i = 0; i < newList->used; ++i )
        {
            newList->tabs[i].label_string =
                XiXmStringCopy(tab_list->tabs[i].label_string);
            newList->tabs[i].label_pixmap = tab_list->tabs[i].label_pixmap;
	    newList->tabs[i].string_direction =
		tab_list->tabs[i].string_direction;
	    newList->tabs[i].pixmap_placement =
		tab_list->tabs[i].pixmap_placement;
	    newList->tabs[i].label_alignment =
		tab_list->tabs[i].label_alignment;
            newList->tabs[i].foreground = tab_list->tabs[i].foreground;
            newList->tabs[i].background = tab_list->tabs[i].background;
	    newList->tabs[i].background_pixmap =
		tab_list->tabs[i].background_pixmap;
	    newList->tabs[i].sensitive = tab_list->tabs[i].sensitive;
            newList->tabs[i].value_mode = XmTAB_VALUE_COPY;
        }
    }

    return( newList );
}

/*
 * Function:
 *      XmTabbedStackListFree(tab_list)
 * Description:
 *      Deallocates the memory associated with the given tab list.
 * Input:
 *      tab_list : XiTablist - the XmTabbedStackList to deallocate
 * Output:
 *      None.
 */
void XmTabbedStackListFree(XmTabbedStackList tab_list)
{
    int i;

    /*
     * If we do not have anything to deallocate the lets just
     * get out of here.
     */
    if( tab_list == NULL ) return;

    /*
     * If we have used items then lets walk through the list and
     * deallocate anything we have to.
     */
    if( tab_list->used != 0 )
    {
        for( i = 0; i < tab_list->used; ++i )
        {
            if( tab_list->tabs[i].value_mode == XmTAB_VALUE_SHARE ) continue;

            XiXmStringFree(tab_list->tabs[i].label_string);
        }
    }

    /*
     * If we have any allocated slots lets deallocate them and finally
     * deallocate the parent structure.
     */
    if( tab_list->allocated != 0 )
    {
        XtFree((XtPointer) tab_list->tabs);
    }
    XtFree((XtPointer) tab_list);
}

/*
 * Function:
 *      XmTabbedStackListRemove(tab_list, position)
 * Description:
 *      Remove and deallocate the tab at the specified position in
 *      the list.
 * Input:
 *      tab_list : XmTabbedStackList - the XmTabbedStackList to work with
 *      position : int       - the position of the tab to remove
 * Output:
 *      None.
 */
void XmTabbedStackListRemove(XmTabbedStackList tab_list, int position)
{
    XmTabAttributes to, from;
    int             count;

    if( tab_list == NULL || position < 0 ||
        (position > 0 && position >= tab_list->used) )
    {
        return;
    }
    to = &(tab_list->tabs[position]);

    /*
     * Now deallocate the data associated with this tab, if needed.
     */
    if( to->value_mode == XmTAB_VALUE_COPY )
    {
        XmStringFree(to->label_string);
    }

    /*
     * Now decrement the number of used slots and shift down any slots
     * above the one that is being deleted.
     */
    tab_list->used--;
    if( position != tab_list->used )
    {
        from = &(tab_list->tabs[position+1]);
        count = tab_list->used - position;
        memmove((XtPointer) to, (XtPointer) from,
                sizeof(XmTabAttributeRec) * count);
    }
}

/*
 * Function:
 *      XmTabbedStackListInsert(tab_list, position, mask, attributes)
 * Description:
 *      Adds a tab with the specified attributes to the given
 *      XmTabbedStackList.
 * Input:
 *      tab_list   : XmTabbedStackList       - the XmTabbedStackList to add the new tab to
 *      position   : int             - the position to place the new tab at
 *      mask       : XtValueMask     - the mask of attributes to set on the
 *                                     new tab
 *      attributes : XmTabAttributes - the list of attributes to set on the
 *				       new tab
 * Output:
 *      int - the position of the new tab in the list
 */
int
XmTabbedStackListInsert(XmTabbedStackList tab_list, int position, XtValueMask mask,
                XmTabAttributes attributes)
{
    XmTabAttributes newTab;

    /*
     * First let's see if they gave us a valid XmTabbedStackList and position
     * and if not let's just leave.
     */
    if( tab_list == NULL || position < XmTAB_LAST_POSITION ||
        (position > 0 && position >= tab_list->used) ) return XmTAB_NOT_FOUND;

    /*
     * Now lets add our new tab to the list in the correct position.
     * If the list is not large enough to hold our new tab we will
     * grow the list.
     */
    if( tab_list->allocated == tab_list->used )
    {
	/*
	 * Our list is not large enough to take another tab, so lets
	 * grow the list by XmLIST_GROWTH_FACTOR.
	 */
	tab_list->allocated += XmLIST_GROWTH_FACTOR;
	tab_list->tabs = (XmTabAttributes)
	    XtRealloc((XtPointer) tab_list->tabs,
		      sizeof(XmTabAttributeRec) * tab_list->allocated);
    }

    /*
     * Once here we know that the list is large enough to hold our new
     * item so now lets move any existing items up to make room for our
     * new kiddie.
     */
    if( position == XmTAB_LAST_POSITION )
    {
	newTab = &(tab_list->tabs[tab_list->used]);
	position = tab_list->used;
    }
    else
    {
	XmTabAttributes to, from;
	int             count;

	from = &(tab_list->tabs[position]);
	to = &(tab_list->tabs[position+1]);
	count = tab_list->used - position;
	memmove(to, from, sizeof(XmTabAttributeRec) * count);
	newTab = &(tab_list->tabs[position]);
    }
    tab_list->used++;

    /*
     * Now lets initialize the new tab.  After that we will take a look
     * at the mask and see what attributes were customized.
     */
    newTab->label_string = (XmString) NULL;
    newTab->string_direction = XmSTRING_DIRECTION_L_TO_R;
    newTab->label_pixmap = XmUNSPECIFIED_PIXMAP;
    newTab->label_alignment = XmALIGNMENT_CENTER;
    newTab->pixmap_placement = XmPIXMAP_RIGHT;
    newTab->foreground = XmCOLOR_DYNAMIC;
    newTab->background = XmCOLOR_DYNAMIC;
    newTab->background_pixmap = XmPIXMAP_DYNAMIC;
    newTab->sensitive = True;
    newTab->value_mode = XmTAB_VALUE_COPY;

    if( mask & XmTAB_VALUE_MODE )
    {
	newTab->value_mode = attributes->value_mode;
    }

    if( mask & XmTAB_LABEL_STRING )
    {
	if( newTab->value_mode == XmTAB_VALUE_COPY )
	{
	    newTab->label_string = XiXmStringCopy(attributes->label_string);
	}
	else
	{
	    newTab->label_string = attributes->label_string;
	}
    }

    if( mask & XmTAB_STRING_DIRECTION )
    {
	newTab->string_direction = attributes->string_direction;
    }

    if( mask & XmTAB_LABEL_PIXMAP )
    {
	newTab->label_pixmap = attributes->label_pixmap;
    }

    if( mask & XmTAB_PIXMAP_PLACEMENT )
    {
	newTab->pixmap_placement = attributes->pixmap_placement;
    }

    if( mask & XmTAB_BACKGROUND )
    {
	newTab->background = attributes->background;
    }

    if( mask & XmTAB_BACKGROUND_PIXMAP )
    {
	newTab->background_pixmap = attributes->background_pixmap;
    }

    if( mask * XmTAB_SENSITIVE )
    {
	newTab->sensitive = attributes->sensitive;
    }

    if( mask & XmTAB_FOREGROUND )
    {
	newTab->foreground = attributes->foreground;
    }

    if( mask & XmTAB_LABEL_ALIGNMENT )
    {
	newTab->label_alignment = attributes->label_alignment;
    }

    return( position );
}

/*
 * Function:
 *	XmTabbedStackListAppend(tab_list, mask, attributes)
 * Description:
 *	Appends a new tab with the given attributes.
 * Input:
 *	tab_list   : XmTabbedStackList       - the XmTabbedStackList to append a tab to
 *	mask       : XtValueMask     - the mask of attributes to set on the
 *				       new tab
 *	attributes : XmTabAttributes - the list of attributes to set on the
 *				       new tab
 * Output:
 *	int - the position of the new tab
 */
int
XmTabbedStackListAppend(XmTabbedStackList tab_list, XtValueMask mask,
                XmTabAttributes attributes)
{
    /*
     * So we cheat a little here.  All we need to do is call insert and
     * lets that function know that we want it at the end of the list.
     */
    return( XmTabbedStackListInsert(tab_list, XmTAB_LAST_POSITION, mask, attributes) );
}

/*
 * Function:
 *	XmTabbedStackListModify(tab_list, position, mask, attributes)
 * Description:
 *	Change the specified attributes for the tab at the given position
 *	in the given XmTabbedStackList.
 * Input:
 *	tab_list   : XmTabbedStackList       - the XmTabbedStackList that contains the
 *				       tab to modify.
 *	position   : int             - the index of the tab to modify
 *	mask       : XtValueMask     - the mask of attributes to modify
 *	attributes : XmTabAttributes - the list of attributes to modify
 * Output:
 *	None.
 */
void
XmTabbedStackListModify(XmTabbedStackList tab_list, int position, XtValueMask mask,
                XmTabAttributes attributes)
{
    XmTabAttributes tab;

    /*
     * First lets see if they gave us anything valid to work with and
     * if they did not lets vacate the premises.
     */
    if( tab_list == NULL || position < 0 || position >= tab_list->used )
    {
	return;
    }

    tab = &(tab_list->tabs[position]);

    /*
     * Now lets check the various flags and see what we need to
     * change.
     */
    if( mask & XmTAB_VALUE_MODE && tab->value_mode != attributes->value_mode &&
        attributes->value_mode == XmTAB_VALUE_COPY )
    {
	/*
	 * Can only change this mode from share to copy, NOT from copy
	 * to share.
	 */
	tab->value_mode = attributes->value_mode;
	tab->label_string = XiXmStringCopy(attributes->label_string);
    }

    if( mask & XmTAB_LABEL_STRING )
    {
	if( tab->value_mode == XmTAB_VALUE_COPY )
	{
	    XiXmStringFree(tab->label_string);
	    tab->label_string = XiXmStringCopy(attributes->label_string);
	}
	else
	{
	    tab->label_string = attributes->label_string;
	}
    }

    if( mask & XmTAB_STRING_DIRECTION )
    {
	tab->string_direction = attributes->string_direction;
    }

    if( mask & XmTAB_LABEL_PIXMAP )
    {
	tab->label_pixmap = attributes->label_pixmap;
    }

    if( mask & XmTAB_PIXMAP_PLACEMENT )
    {
	tab->pixmap_placement = attributes->pixmap_placement;
    }

    if( mask & XmTAB_BACKGROUND )
    {
	tab->background = attributes->background;
    }

    if( mask & XmTAB_BACKGROUND_PIXMAP )
    {
	tab->background_pixmap = attributes->background_pixmap;
    }

    if( mask & XmTAB_SENSITIVE )
    {
	tab->sensitive = attributes->sensitive;
    }

    if( mask & XmTAB_FOREGROUND )
    {
	tab->foreground = attributes->foreground;
    }

    if( mask & XmTAB_LABEL_ALIGNMENT )
    {
	tab->label_alignment = attributes->label_alignment;
    }
}

/*
 * Function:
 *	XmTabbedStackListQuery(tab_list, position, mask, attributes)
 * Description:
 *	Retrieve attributes from the specified tab. (NOTE: This may be
 *	a copy out, depends on value_mode, so be sure to free
 *	this when done.)
 * Input:
 *	tab_list   : XmTabbedStackList       - the tab list that contains the tab
 *	position   : int             - the tab index to query
 *	attributes : XmTabAttributes - the attributes returned.
 * Output:
 *	None.
 */
void
XmTabbedStackListQuery(XmTabbedStackList tab_list, int position, XmTabAttributes attributes)
{
    XmTabAttributes tab;

    /*
     * Make sure that we have valid data and if not "asta la vesta, baby"
     * I have NO idea how to spell that.
     */
    if( tab_list == NULL || position < 0 || position >= tab_list->used )
    {
	return;
    }

    tab = &(tab_list->tabs[position]);

    if( tab->value_mode == XmTAB_VALUE_COPY )
    {
	attributes->label_string = XiXmStringCopy(tab->label_string);
    }
    else
    {
	attributes->label_string = tab->label_string;
    }

    attributes->label_pixmap = tab->label_pixmap;
    attributes->string_direction = tab->string_direction;
    attributes->label_alignment = tab->label_alignment;
    attributes->pixmap_placement = tab->pixmap_placement;
    attributes->background = tab->background;
    attributes->background_pixmap = tab->background_pixmap;
    attributes->sensitive = tab->sensitive;
    attributes->foreground = tab->foreground;
    attributes->value_mode = tab->value_mode;
}

/*
 * Function:
 *	XmTabbedStackListFind(tab_list, label_string)
 * Description:
 *	Finds the first tab whose label_string matches the given
 *	and returns its index.
 * Input:
 *	tab_list     : XmTabbedStackList - the XmTabbedStackList to search
 *	label_string : XmString  - the label_string to search for
 * Output:
 *	int - the index of the tab found, or XmTAB_NOT_FOUND if
 *	      a match is not found.
 */
int
XmTabbedStackListFind(XmTabbedStackList tab_list, XmString label_string)
{
    int i;

    if( tab_list == NULL ) return( XmTAB_NOT_FOUND );

    for( i = 0; i < tab_list->used; ++i )
    {
	if( XiXmStringCompare(tab_list->tabs[i].label_string, label_string) )
	{
	    return( i );
	}
    }

    return( XmTAB_NOT_FOUND );
}

/*
 * Function:
 *	XmTabbedStackListSimpleRemove(tab_list, label_string)
 * Description:
 *	Removes the first tab from the given XmTabbedStackList that matches the
 *	given label string.
 * Input:
 *	tab_list     : XmTabbedStackList - tab list to remove the item from
 *	label_string : XmString  - the label string to match
 * Output:
 *	None.
 */
void
XmTabbedStackListSimpleRemove(XmTabbedStackList tab_list, XmString label_string)
{
    int position = XmTabbedStackListFind(tab_list, label_string);

    if( position != XmTAB_NOT_FOUND ) XmTabbedStackListRemove(tab_list, position);
}

/*
 * Function:
 *	XmTabbedStackListSimpleInsert(tab_list, position, label_string)
 * Description:
 *	Inserts a new tab with the specified label string at the
 *	specified position.
 * Input:
 *	tab_list     : XmTabbedStackList - the XmTabbedStackList to add a tab to
 *	position     : int       - the position to put the tab at
 *	label_string : XmString  - the label for the new tab
 * Output:
 *	int - the position of the new tab
 */
int
XmTabbedStackListSimpleInsert(XmTabbedStackList tab_list, int position, XmString label_string)
{
    XmTabAttributeRec attributes;

    attributes.label_string = label_string;

    return( XmTabbedStackListInsert(tab_list, position, XmTAB_LABEL_STRING,
			    &attributes) );
}

/*
 * Function:
 *	XmTabbedStackListSimpleAppend(tab_list, label_string)
 * Description:
 *	Append a new tab to the given tab list with the specified label
 *	string.
 * Input:
 *	tab_list     : XmTabbedStackList - the XmTabbedStackList to append a tab to
 *	label_string : XmString  - the label for the new tab
 * Output:
 *	int - the position of the new tab
 */
int
XmTabbedStackListSimpleAppend(XmTabbedStackList tab_list, XmString label_string)
{
    XmTabAttributeRec attributes;

    attributes.label_string = label_string;

    return( XmTabbedStackListAppend(tab_list, XmTAB_LABEL_STRING,
			    &attributes) );
}

/*
 * Function:
 *	XmTabbedStackListsimpleModify(tab_list, position, label_string)
 * Description:
 *	Changes the label string for the specified tab.
 * Input:
 *	tab_list     : XmTabbedStackList - the XmTabbedStackList that contains the tab to
 *				   change
 *	position     : int       - the tab to change
 * 	label_string : XmString  - the new value of the label string
 * Output:
 *	None.
 */
void
XmTabbedStackListSimpleModify(XmTabbedStackList tab_list, int position, XmString label_string)
{
    XmTabAttributeRec attributes;

    XmTabbedStackListModify(tab_list, position, XmTAB_LABEL_STRING, &attributes);
}

/*
 * Function:
 *	XmTabbedStackListSimpleQuery(tab_list, position)
 * Description:
 *	Query the label string from the specified tab
 * Input:
 *	tab_list : XmTabbedStackList - the XmTabbedStackList to query
 *	position : int       - the tab to query
 * Output:
 *	XmString - the label for the given tab (NOTE: this is a copy
 *	           so please free when you are done)
 */
XmString
XmTabbedStackListSimpleQuery(XmTabbedStackList tab_list, int position)
{
    XmTabAttributeRec attributes;

    XmTabbedStackListQuery(tab_list, position, &attributes);

    if( attributes.value_mode != XmTAB_VALUE_COPY )
    {
	attributes.label_string = XiXmStringCopy(attributes.label_string);
    }

    return( attributes.label_string );
}

/*
 * Function:
 *	XmTabAttributesFree(attributes)
 * Description:
 *	This deallocates any memory that the attribute structure is
 *	using.  This does not free the attribute structure.
 * Input:
 *	attributes : XiAttributes - the structure whose contants should be
 *		                    deallocated.
 * Output:
 *	None.
 */
void
XmTabAttributesFree(XmTabAttributes attributes)
{
    if( attributes->value_mode == XmTAB_VALUE_COPY )
    {
	XiXmStringFree(attributes->label_string);
    }
}

/*
 * Function:
 *	_XmTabbedStackListCount(tab_list)
 * Description:
 *	Returns the number of entries in the given XmTabbedStackList.
 * Input:
 *	tab_list : XmTabbedStackList - the tab list whose entries to count
 * Output:
 *	int - the number of items in the given XmTabbedStackList
 */
int
_XmTabbedStackListCount(XmTabbedStackList tab_list)
{
    if( tab_list == NULL ) return( 0 );

    return( tab_list->used );
}

/*
 * Function:
 *	_XmTabbedStackListGet(tab_list, position)
 * Description:
 *	Returns a pointer to the given element in the XmTabbedStackList.  Use
 *	with caution as it returns a pointer to live memory.
 * Input:
 *	tab_list : XmTabbedStackList - the tab list to index into
 *	int      : Position  - the index of the tab you want
 * Output:
 *	XmTabAttributes - a pointer to the specified tab
 */
XmTabAttributes
_XmTabbedStackListGet(XmTabbedStackList tab_list, int position)
{
    XmTabAttributes tab;

    /*
     * Make sure that we have valid data and if not "asta la vesta, baby"
     * I have NO idea how to spell that.
     */
    if( tab_list == NULL || position < 0 || position >= tab_list->used )
    {
	return( NULL );
    }

    tab = &(tab_list->tabs[position]);

    return( tab );
}

XmTabAttributes _XmTabbedStackListArray(XmTabbedStackList tab_list)
{
    /*
     * Make sure that we have valid data and if not "asta la vesta, baby"
     * I have NO idea how to spell that.
     */
    if( tab_list == NULL )
    {
	return( NULL );
    }

    return( tab_list->tabs );
}

/*
 * Function:
 *	XmTabbedStackListCompare(XmTabbedStackList list1, XmTabbedStackList list2)
 * Description:
 *	Compares to tab lists to see if they are logically
 *	equivalent.
 * Input:
 *	list1 : XmTabbedStackList - tab list to compare
 *	list2 : XmTabbedStackList - tab list to compare
 * Output:
 *	XmTabResult - indicates the differences it any
 */
XmTabResult XmTabbedStackListCompare(XmTabbedStackList list1, XmTabbedStackList list2)
{
    XmTabResult result = XmTAB_CMP_EQUAL;
    int         i;

    if( (list1 == NULL && list2 != NULL) ||
        (list1 != NULL && list2 == NULL) ||
        list1->used != list2->used ) return( XmTAB_CMP_SIZE );

    for( i = 0; i < list1->used; ++i )
    {
	if( list1->tabs[i].label_pixmap != list2->tabs[i].label_pixmap ||
	    !XiXmStringCompare(list1->tabs[i].label_string,
			       list2->tabs[i].label_string) ||
 	    list1->tabs[i].pixmap_placement !=
	    list2->tabs[i].pixmap_placement )
	{
	    return( XmTAB_CMP_SIZE );
	}
	if( list1->tabs[i].string_direction !=
	    list2->tabs[i].string_direction ||
	    list1->tabs[i].label_alignment != list2->tabs[i].label_alignment ||
	    list1->tabs[i].foreground != list2->tabs[i].foreground ||
	    list1->tabs[i].background != list2->tabs[i].background ||
	    list1->tabs[i].sensitive != list2->tabs[i].sensitive ||
	    list1->tabs[i].background_pixmap !=
	    list2->tabs[i].background_pixmap )
	{
	    result = XmTAB_CMP_VISUAL;
	}
    }
    return( result );
}
