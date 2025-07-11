/* $XConsortium: xmAuto.h /main/4 1995/07/15 21:14:40 drk $ */
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

#define MANUAL		0
#define RECORD  	1
#define COMPARE 	2
#define IGNORE  	3
#define FULL		4
#define DUMP		5
#define TRACE		6
#define DELAY		7
#define SCRIPT  	8
#define VISUAL		9
#define INSTRUCT 	10
#define OUTPUT		11
#define COLOR		12
#define EXTRA		13
#define WRITE		14
#define READ 		15

#define MAXLINE		1024

static void CreateLayout();
static void QuitCB();
static void CreateDialogs();
static void ResetFSB();
static void AddTestCB();
static void GetTestNameCB();
static void BuildTestString();
static void SimpleCB();
static void ClearCB();
static void ClearArgs();
static void PromptCB();
static void ReadInCB();
static void WriteOutCB();
static void BrowseCB();
static void SaveToListCB();
static void ChangeFileCB();
static void ChangeFileCB();
static void FileSelectCB();

Widget Label1, List1, ScaleDelay;
Widget ToggleManual, ToggleIgnore;
Widget ToggleRecord, ToggleCompare;
Widget ToggleFull, ToggleTrace, ToggleWinDump;
Widget PromptDialog, FileSelectBox;

static char CurrentTestString[256];
static char LabelString[1024];
static char arguments[256];

static int list_pos = 0;

static Boolean manual = False;
static Boolean record = False;
static Boolean compare = True;
static Boolean ignore = False;

static Boolean full = False;
static Boolean dumps = False;
static Boolean trace = False;

static int delay = 0;
