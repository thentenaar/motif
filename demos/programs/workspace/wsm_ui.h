/* $XConsortium: wsm_ui.h /main/4 1995/07/15 20:46:54 drk $ */
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

#ifndef wsmUI_h
#define wsmUI_h

#include "wsm.h"

typedef struct _WSM_UI{
Widget wsm_row_column;
Widget *space_button;
int num_space_buttons;
Widget configure_form;
Widget from_list;
Widget to_list;
Widget copy_toggle;
Widget link_toggle;
Widget window_toggle;
Widget client_toggle;
Widget from_option_menu;
Widget *from_option_button;
Widget to_option_menu;
Widget *to_option_button;
Widget delete_button;
Widget move_button;
Space *from_space;
Space *to_space;
WorkWindow *w_window;
Widget delete_form;
Widget delete_list;
Widget ok_button;
Widget name_form;
Widget name_list;
Widget name_text;
Widget pixmap_text;
Widget background_form;
Widget background_list;
Widget background_text;
Widget save_as_form;
Widget save_as_text;
Widget occupy_form;
Widget occupy_list;
Widget copy_occupy_toggle;
Widget link_occupy_toggle;
WorkWindow* occupy_window;
} WSM_UI;

#endif
