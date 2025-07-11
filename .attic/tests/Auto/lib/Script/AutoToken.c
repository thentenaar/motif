/* $XConsortium: AutoToken.c /main/5 1995/07/15 20:59:27 drk $ */
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

#include <Xm/Xm.h>		/* Xm.h is only used to set _NO_PROTO */
#include "AutoToken.h"

int	AutoFindToken(char* token);

static struct {
    char *token;
    int value;
} token_list[] = {
{"Continue",		CONTINUE },
{"Exit",			END },
{"Manual",		MANUAL },

{"PressMB",		PRESSMB },
{"ReleaseMB",		RELEASEMB },
{"ClickMB",		CLICKMB },

{"<Shift>",		SHIFTMASK },
{"<Lock>",		LOCKMASK },
{"<Ctrl>",		CTRLMASK },
{"<Meta>",		METAMASK },
{"<Alt>",			ALTMASK },
{"<Mod1>",		MOD1MASK },
{"<Mod2>",		MOD2MASK },
{"<Mod3>",		MOD3MASK },
{"<Mod4>",		MOD4MASK },
{"<Mod5>",		MOD5MASK },

{"Btn1",			BTN1 },
{"Btn2",			BTN2 },
{"Btn3",			BTN3 },
{"Btn4",			BTN4 },
{"Btn5",			BTN5 },

{"ClickKey",		CLICKKEY },
{"PressKey",		PRESSKEY },
{"ReleaseKey",		RELEASEKEY },

{"KBackSpace",		BACKSPACE },
{"KInsert",		INSERT },
{"KDelete",		DELETE },
{"KCopy",			COPY },
{"KCut",			CUT },
{"KPaste",		PASTE },
{"KAddMode",		ADDMODE },
{"KPrimaryPaste",		PRIMARYPASTE },
{"KQuickPaste",		QUICKPASTE },
{"KPageUp",		PAGEUP },
{"KPageDown",		PAGEDOWN } ,
{"KEndLine",		ENDLINE },
{"KBeginLine",		BEGINLINE },
{"KActivate",		ACTIVATE },
{"KMenuBar",		MENUBAR },
{"KClear",		CLEAR },
{"KCancel",		CANCEL },
{"KHelp",			HELP },
{"KMenu",			MENU },
{"KSelect",		SELECT },
{"KUndo",		 	UNDO },
{"KUp",			UPK },
{"KRight",		RIGHTK },
{"KDown",		 	DOWNK },
{"KLeft",		 	LEFTK },

{"KBackTab",		BACKTAB },
{"KBeginData",		BEGINDATA },
{"KDeselectAll",		DESELECTALL },
{"KEndData",		ENDDATA },
{"KEnter",		ENTER },
{"KEscape",		ESCAPE },
{"KExtend",		EXTEND },
{"KNextField",		NEXTFIELD },
{"KNextMenu",		NEXTMENU },
{"KPageRight",		PAGERIGHT },
{"KPageLeft",		PAGELEFT },
{"KPrevField",		PREVFIELD },
{"KPrevMenu",		PREVMENU },
{"KPrimaryCopy",		PRIMARYCOPY },
{"KPrimaryCut",		PRIMARYCUT },
{"KQuickCopy",		QUICKCOPY },
{"KQuickCut",		QUICKCUT },
{"KQuickExtend",		QUICKEXTEND },
{"KReselect",		RESELECT },
{"KRestore",		RESTORE },
{"KSelectAll",		SELECTALL },
{"KSpace",		SPACE },
{"KTab",			TAB },

{"Ka",			KEYA },
{"Kb",			KEYB },
{"Kc",			KEYC },
{"Kd",			KEYD },
{"Ke",			KEYE },
{"Kf",			KEYF },
{"Kg",			KEYG },
{"Kh",			KEYH },
{"Ki",			KEYI },
{"Kj",			KEYJ },
{"Kk",			KEYK },
{"Kl",			KEYL },
{"Km",			KEYM },
{"Kn",			KEYN },
{"Ko",			KEYO },
{"Kp",			KEYP },
{"Kq",			KEYQ },
{"Kr",			KEYR },
{"Ks",			KEYS },
{"Kt",			KEYT },
{"Ku",			KEYU },
{"Kv",			KEYV },
{"Kw",			KEYW },
{"Kx",			KEYX },
{"Ky",			KEYY },
{"Kz",			KEYZ },

{"KSlash",		KEYSLASH },
{"KBackSlash",		KEYBACKSLASH },
{"KBraceRight",		KEYBRACERIGHT },
{"KBraceLeft",		KEYBRACELEFT },
{"KBracketRight",		KEYBRACKETRIGHT },
{"KBracketLeft",		KEYBRACKETLEFT },
{"KReturn",		KEYRETURN },

{"K1",			KEY1 },
{"K2",			KEY2 },
{"K3",			KEY3 },
{"K4",			KEY4 },
{"K5",			KEY5 },
{"K6",			KEY6 },
{"K7",			KEY7 },
{"K8",			KEY8 },
{"K9",			KEY9 },
{"K0",			KEY0 },

{"KExcalmation",	KEYEXCLAM },
{"KAt",			KEYAT },
{"KNumberSign",		KEYNUMBERSIGN },
{"KDollar",		KEYDOLLAR },
{"KPercent",		KEYPERCENT },
{"KCaret",		KEYCARET },
{"KAmpersand",		KEYAMPERSAND },
{"KAsterisk",		KEYASTERISK },
{"KParenLeft",		KEYPARENLEFT },
{"KParenRight",		KEYPARENRIGHT },

{"KNone",		KEYNONE },

{"LocatePointer",	LOCATEPOINTER },
{"LocatePointerXYAbs",	LOCATEPOINTERABS },
{"LocatePointerXYRel",	LOCATEPOINTERREL },
{"System",		SYSTEM },
{"Wait",		WAIT },
{"CompareVisual",	COMPAREVISUAL },
{"StoreVisual",	        STOREVISUAL },
{"CompareStoredVisual",	COMPARESTOREDVISUAL },
{"SetFocus",		SETFOCUS },
{"InputString",		INPUTSTRING },

{"DragComponent",	DRAG },
{"DragXYRelative",	DRAGRELATIVE },
{"DragSliderValue",	DRAGSLIDERVAL },
{"DragXYAbs",		DRAGABS },

{"RSeparator",		SEPARATOR },
{"RLabel",		LABEL },
{"RListItem",		LISTITEM },
{"RFileListItem",	FILELISTITEM },
{"RDirListItem",	DIRLISTITEM },
{"RDirFilterLabel",	DIRFILTERLABEL },
{"RDirFilterText",	DIRFILTERTEXT },
{"RDirFilterTextChar",	DIRFILTERTEXTCHAR },
{"RSelectionListItem",	SELECTIONLISTITEM },
{"RTextChar",		TEXTCHAR },
{"RSlider",		SLIDER },
{"RComboBoxDropDownButton",	COMBOBOXDROPDOWNBUTTON },
{"ROutlineBtn",		OUTLINEBTN },
{"RIncrementBtn",	INCREMENTBTN },
{"RDecrementBtn",	DECREMENTBTN },
{"RPageScroller",	PAGESCROLLER },
{"RMajorTabScrollerNext",	MAJORTABSCROLLERNEXT },
{"RMajorTabScrollerPrev",	MAJORTABSCROLLERPREV },
{"RMinorTabScrollerNext",	MINORTABSCROLLERNEXT },
{"RMinorTabScrollerPrev",	MINORTABSCROLLERPREV },
{"RUpOrLeftArrow",	UPORLEFTARROW },
{"RDownOrRightArrow",	DOWNORRIGHTARROW },
{"RUpOrLeftSliderArea",	UPORLEFTSLIDERAREA },
{"RDownOrRightSliderArea",	DOWNORRIGHTSLIDERAREA },
{"RScrollBar",		SCROLLBAR },
{"RHorizScrollBar",	HORIZSCROLLBAR },
{"RHorizScrollBarLeftArrow",	HORIZSBLEFTARROW },
{"RHorizScrollBarRightArrow",	HORIZSBRIGHTARROW },
{"RHorizScrollBarLeftSliderArea",	HORIZSBLEFTSLIDERAREA },
{"RHorizScrollBarRightSliderArea",	HORIZSBRIGHTSLIDERAREA },
{"RHorizScrollBarSlider",	HORIZSBSLIDER },
{"RVertScrollBarUpArrow",	VERTSBUPARROW },
{"RVertScrollBarDownArrow",	VERTSBDOWNARROW },
{"RVertScrollBarUpSliderArea",	VERTSBUPSLIDERAREA },
{"RVertScrollBarDownSliderArea",	VERTSBDOWNSLIDERAREA },
{"RVertScrollBarSlider",	VERTSBSLIDER },
{"RVertScrollBar",	VERTSCROLLBAR },
{"RMenuBar",		MENUBAR },
{"RSash",		SASH },
{"RCloseButton",	CLOSEBUTTON },
{"RHelpButton",		HELPBUTTON },
{"ROkButton",		OKBUTTON },
{"RFilterButton",	FILTERBUTTON },
{"RCancelButton",	CANCELBUTTON },
{"RApplyButton",	APPLYBUTTON },
{"RMessageLabel",	MESSAGELABEL },
{"RSymbolLabel",	SYMBOLLABEL },
{"RSelectionText",	SELECTIONTEXT },
{"RSelectionTextChar",	SELECTIONTEXTCHAR },
{"RSelectionLabel",	SELECTIONLABEL },
{"RSelectionList",	SELECTIONLIST },
{"RSelectionListLabel",	SELECTIONLISTLABEL },
{"RFileList",		FILELIST },
{"RFileListLabel",	FILELISTLABEL },
{"RDirList",		DIRLIST },
{"RDirListLabel",	DIRLISTLABEL },
{"RFileSelectionText",	FILESELECTIONTEXT },
{"RFileSelectionTextChar",	FILESELECTIONTEXTCHAR },
{"RFileSelectionLabel",	FILESELECTIONLABEL },
{"RFileFilterText",	FILEFILTERTEXT },
{"RFileFilterTextChar",	FILEFILTERTEXTCHAR },
{"RFileFilterLabel",	FILEFILTERLABEL },
{"RWorkArea",		WORKAREA },
{"RCommandWindow",	COMMANDWINDOW },
{"RMessageWindow",	MESSAGEWINDOW },
{"RWorkWindow",		WORKWINDOW },
{"RScale",		SCALE },
{"RScaleLabel",		SCALELABEL },
{"ROptionButton",	OPTIONBUTTON },
{"RScrolledWindow",	SCROLLEDWINDOW },
{"RScrollBarSlider",	SCROLLBARSLIDER },
{"RScaleSlider",		SCROLLBARSLIDER },
{"RScrollBarUpOrLeftArrow",	SCROLLBARUPORLEFTARROW },
{"RScrollBarDownOrRightArrow",	SCROLLBARDOWNORRIGHTARROW },
{"RScrollBarUpOrLeftSliderArea",SCROLLBARUPORLEFTSLIDERAREA },
{"RScaleUpOrLeftSliderArea",	SCROLLBARUPORLEFTSLIDERAREA },
{"RScrollBarDownOrRightSliderArea",	SCROLLBARDOWNORRIGHTSLIDERAREA },
{"RScaleDownOrRightSliderArea",	SCROLLBARDOWNORRIGHTSLIDERAREA },

{"RFileListHorizScrollBarLeftArrow", FLHSBUPORLEFTARROW },
{"RFileListHorizScrollBarRightArrow", FLHSBDOWNORRIGHTARROW },
{"RFileListHorizScrollBarLeftSliderArea", FLHSBUPORLEFTSLIDERAREA },
{"RFileListHorizScrollBarRightSliderArea", FLHSBDOWNORRIGHTSLIDERAREA },
{"RFileListHorizScrollBarSlider", FLHSBSLIDER },
{"RFileListVertScrollBarUpArrow", FLVSBUPORLEFTARROW },
{"RFileListVertScrollBarDownArrow", FLVSBDOWNORRIGHTARROW },
{"RFileListVertScrollBarUpSliderArea", FLVSBUPORLEFTSLIDERAREA },
{"RFileListVertScrollBarDownSliderArea", FLVSBDOWNORRIGHTSLIDERAREA },
{"RFileListVertScrollBarSlider",	FLVSBSLIDER },


{"RDirListHorizScrollBarLeftArrow"	,DLHSBUPORLEFTARROW },
{"RDirListHorizScrollBarRightArrow"	,DLHSBDOWNORRIGHTARROW },
{"RDirListHorizScrollBarLeftSliderArea" ,DLHSBUPORLEFTSLIDERAREA },
{"RDirListHorizScrollBarRightSliderArea", DLHSBDOWNORRIGHTSLIDERAREA },
{"RDirListHorizScrollBarSlider" ,	DLHSBSLIDER },
{"RDirListVertScrollBarUpArrow" ,	DLVSBUPORLEFTARROW },
{"RDirListVertScrollBarDownArrow", DLVSBDOWNORRIGHTARROW },
{"RDirListVertScrollBarUpSliderArea", DLVSBUPORLEFTSLIDERAREA },
{"RDirListVertScrollBarDownSliderArea", DLVSBDOWNORRIGHTSLIDERAREA },
{"RDirListVertScrollBarSlider",	DLVSBSLIDER },


{"RSelectListHorizScrollBarLeftArrow", SLHSBUPORLEFTARROW },
{"RSelectListHorizScrollBarRightArrow", SLHSBDOWNORRIGHTARROW },
{"RSelectListHorizScrollBarLeftSliderArea", SLHSBUPORLEFTSLIDERAREA },
{"RSelectListHorizScrollBarRightSliderArea", SLHSBDOWNORRIGHTSLIDERAREA },
{"RSelectListHorizScrollBarSlider", SLHSBSLIDER },
{"RSelectListVertScrollBarUpArrow", SLVSBUPORLEFTARROW },
{"RSelectListVertScrollBarDownArrow", SLVSBDOWNORRIGHTARROW },
{"RSelectListVertScrollBarUpSliderArea", SLVSBUPORLEFTSLIDERAREA },
{"RSelectListVertScrollBarDownSliderArea", SLVSBDOWNORRIGHTSLIDERAREA },
{"RSelectListVertScrollBarSlider",SLVSBSLIDER },

{"RTearOffButton",		TEAROFFBUTTON },

{"WindowIconify",		WINDOWICONIFY },
{"WindowDeiconify",		WINDOWDEICONIFY },
{"WindowMenuPost",		WINDOWMENUPOST },
{"WindowMenuUnpost",		WINDOWMENUUNPOST },
{"WindowMenuSelect",		WINDOWMENUSELECT },
{"WindowMaximize",		WINDOWMAXIMIZE },
{"WindowNormalize",		WINDOWNORMALIZE },
{"WindowResize",		WINDOWRESIZE },
{"WindowMove",			WINDOWMOVE },
{"WindowRaise",			WINDOWRAISE },
{"WindowFocus",			WINDOWFOCUS },
{"WindowClick",			WINDOWCLICK },
{"WindowItemCheck",		WINDOWICHECK },

{"IconMove",			ICONMOVE },
{"IconMenuPost",		ICONMENUPOST },
{"IconMenuUnpost",		ICONMENUUNPOST },
{"IconMenuSelect",		ICONMENUSELECT },

{"North",			NORTH },
{"South",			SOUTH },
{"West",			WEST },
{"East",			EAST },
{"NorthEast",			NORTHEAST },
{"NorthWest",			NORTHWEST },
{"SouthEast",			SOUTHEAST },
{"SouthWest",			SOUTHWEST },

{"RWindowMenu",			SYSTEMMENU },
{"RIconMenu",			ICONMENU },
{"RWindowDecoration",		WINDOWDECORATION },

{"Keyboard",			KEYBOARD },

{"Off",				OFF },
{"Left",			LEFT },
{"Right",			RIGHT },
{"Up",				UP },
{"Down",			DOWN },
{"Any",				ANY },

{"AutoPerformMonitor",          AUTOPERFORMMONITOR },
{"On",                          ON },


{"Max",				AUTOMAX },
{"Min",				AUTOMIN },
{"",				-1},
};

int	
AutoFindToken(char* token)
{
Cardinal	n;

	n = 0;
	while ( strcmp(token_list[n].token, token) != 0 )	{
		if ( strcmp(token_list[n].token, "" ) == 0 ) return(-1);
		n++;
	} 
	return(token_list[n].value);
}

