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
/*   $XConsortium: text.h /main/6 1995/07/14 10:04:01 drk $ */

#include <stdio.h>
#include <stdlib.h>

#include <Xm/Xm.h>
#include <Mrm/MrmPublic.h>
#include <Xm/MainW.h>
#include <Xm/Label.h>
#include <Xm/RowColumn.h>
#include <Xm/PushB.h>
#include <Xm/ToggleB.h>
#include <Xm/Separator.h>
#include <Xm/Form.h>
#include <Xm/Frame.h>
#include <Xm/CascadeB.h>
#include <Xm/MessageB.h>
#include <Xm/PanedW.h>
#include <Xm/SelectioB.h>
#include <Xm/FileSB.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>

#define text_h
#include "fileview.h"
#include "textE.h"
#include "mainE.h"
#include "fileE.h"

/*
 * Localized strings, loaded at run time from a localized resource file
 */

static XmString no_search_msg = NULL;
static XmString not_found_msg = NULL;
static XmString no_pattern_msg = NULL;
static XmString search_msg = NULL;

/*
 * UIL literal names for localized strings.
 */

static String no_search = "no_search" ;
static String not_found = "not_found" ;
static String no_pattern = "no_pattern" ;
static String search_prompt = "searchprompt";

static void CancelSearch(Widget button, ViewPtr this,
			    XmPushButtonCallbackStruct *call_data);

static void SearchSubstring(Widget button, ViewPtr this,
			    XmPushButtonCallbackStruct *call_data);

static void NoInsert(Widget text, ViewPtr this, XmTextVerifyPtr verify);

static void ChangeCurrentPane(Widget text, ViewPtr this, 
			      XmAnyCallbackStruct verify);


#undef text_h
