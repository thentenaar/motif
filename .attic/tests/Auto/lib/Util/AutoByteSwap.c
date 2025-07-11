/* $XConsortium: AutoByteSwap.c /main/5 1995/07/15 20:59:44 drk $ */
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
/*
 *	file: AutoByteOrder.c
 *
 *	Contains routines to initialize and swap byteOrder if needed
 *
 */
#include <testlib.h>
#include <AutoMwm.h>


/*
 *	Swap the byte order 
 */

#define	swap4bytes(l) {\
		unsigned int t = (unsigned int) l;\
		char n;\
		char *tp = (char *) &t;\
		tp += sizeof(unsigned int) - 4;\
		n = tp[0]; tp[0] = tp[3]; tp[3] = n;\
		n = tp[1]; tp[1] = tp[2]; tp[2] = n;\
		l = t;\
}

#define	swap2bytes(s) {\
		unsigned short t = (unsigned short) s;\
		char n;\
		char *tp = (char *) &t;\
		tp += sizeof(unsigned short) - 2;\
		n = tp[0]; tp[0] = tp[1]; tp[1] = n;\
		s = t;\
}

unsigned char	AutoByteOrderChar = (char) 0;


void
AutoSwapBytes(PropMwmFrameIconInfo *window_prop)
{

	int	i;
		

	if (window_prop->byte_order == AutoByteOrderChar)
		return;

	swap4bytes(window_prop->windowX);
	swap4bytes(window_prop->windowY);
	swap4bytes(window_prop->upperBorderWidth);
	swap4bytes(window_prop->lowerBorderWidth);
	swap4bytes(window_prop->frameWin);
	swap4bytes(window_prop->titleWin);
	swap4bytes(window_prop->menuWin);
	swap2bytes(window_prop->titleRect.x);
	swap2bytes(window_prop->titleRect.y);
	swap2bytes(window_prop->titleRect.width);
	swap2bytes(window_prop->titleRect.height);

	for (i = 0; i < MAX_TITLE_REGIONS; i++) {

		swap4bytes(window_prop->titleGadgets[i].id);
		swap2bytes(window_prop->titleGadgets[i].rect.x);
		swap2bytes(window_prop->titleGadgets[i].rect.y);
		swap2bytes(window_prop->titleGadgets[i].rect.width);
		swap2bytes(window_prop->titleGadgets[i].rect.height);

	}
	swap4bytes(window_prop->titleGadgetCount);

	for (i = 0; i < MAX_RESIZE_HANDLES; i++) {

		swap4bytes(window_prop->resizeGadgets[i].id);
		swap2bytes(window_prop->resizeGadgets[i].rect.x);
		swap2bytes(window_prop->resizeGadgets[i].rect.y);
		swap2bytes(window_prop->resizeGadgets[i].rect.width);
		swap2bytes(window_prop->resizeGadgets[i].rect.height);

	}

	for (i = 0; i < MAX_MENU_ITEMS; i++)
		swap4bytes(window_prop->windowMenu[i].sensitive);
	swap4bytes(window_prop->menuItemCount);
	swap4bytes(window_prop->sensitiveItemCount);

	swap4bytes(window_prop->iconInfo.clientState);
	swap4bytes(window_prop->iconInfo.useIconBox);
	swap4bytes(window_prop->iconInfo.X);
	swap4bytes(window_prop->iconInfo.Y);
	swap4bytes(window_prop->iconInfo.Width);
	swap4bytes(window_prop->iconInfo.Height);
	swap4bytes(window_prop->iconInfo.iconFrameWin);

	swap4bytes(window_prop->iconBoxInfo.iconboxX);
	swap4bytes(window_prop->iconBoxInfo.iconboxY);
	swap4bytes(window_prop->iconBoxInfo.iconboxWidth);
	swap4bytes(window_prop->iconBoxInfo.iconboxHeight);
	swap4bytes(window_prop->iconBoxInfo.pointerX);
	swap4bytes(window_prop->iconBoxInfo.pointerY);
	swap4bytes(window_prop->iconBoxInfo.top_arrowX);
	swap4bytes(window_prop->iconBoxInfo.top_arrowY);
	swap4bytes(window_prop->iconBoxInfo.bottom_arrowX);
	swap4bytes(window_prop->iconBoxInfo.bottom_arrowY);
	swap4bytes(window_prop->iconBoxInfo.left_arrowX);
	swap4bytes(window_prop->iconBoxInfo.left_arrowY);
	swap4bytes(window_prop->iconBoxInfo.right_arrowX);
	swap4bytes(window_prop->iconBoxInfo.right_arrowY);
	swap4bytes(window_prop->iconBoxInfo.vert_inc_needed);
	swap4bytes(window_prop->iconBoxInfo.horiz_inc_needed);
	swap4bytes(window_prop->iconBoxInfo.iconFrameWin);
	swap4bytes(window_prop->iconBoxInfo.iconShellWin);
	swap4bytes(window_prop->iconBoxInfo.iconScrollWin);
	swap4bytes(window_prop->iconBoxInfo.hScrollWin);
	swap4bytes(window_prop->iconBoxInfo.vScrollWin);

}


void
AutoInitByteOrderChar(void )
{

	unsigned int	endian;


	/* bootstrap the byteorder if needed */

	if (AutoByteOrderChar == 0) {

		endian = 1;

		if (*((char *)&endian))
			AutoByteOrderChar = 'l';
		else
			AutoByteOrderChar = 'B';

	}

}
