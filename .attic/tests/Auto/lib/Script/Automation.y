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
/* static char rcsid[] = "$TOG: Automation.y /main/11 1999/10/27 12:31:31 jff $" */
%{

#include <stdio.h>
#include "mvslib.h"
#include "Automation.h"

char *temp;
char temp1[100];

int line_cnt = 1;

%}

%token			STRING 		
%token			DIGIT 	
%token			ISTRING

%token			CONTINUE
%token			END
%token			MANUAL

%token			WAIT

%token			PRESSMB 	
%token			RELEASEMB 
%token			CLICKMB 

%token			SHIFTMASK 	
%token			LOCKMASK 
%token			CTRLMASK
%token			METAMASK
%token			ALTMASK 	
%token			MOD1MASK 
%token			MOD2MASK 	
%token			MOD3MASK 
%token			MOD4MASK 
%token			MOD5MASK 

%token			BTN1 		
%token			BTN2 		
%token			BTN3 		
%token			BTN4 		
%token			BTN5 		

%token			CLICKKEY 
%token			PRESSKEY 
%token			RELEASEKEY 	

%token			BACKSPACE 
%token			INSERT 	
%token			DELETE 
%token			COPY 
%token			CUT 
%token			PASTE 		
%token			ADDMODE 
%token			PRIMARYPASTE 	
%token			QUICKPASTE 	
%token			PAGEUP 		
%token			PAGEDOWN 	
%token			ENDLINE 	
%token			BEGINLINE 	
%token			ACTIVATE 	
%token			MENUBAR	 	
%token			CLEAR 		
%token			CANCEL 		
%token			HELP 		
%token			MENU 	
%token			SELECT 		
%token			UNDO 	
%token			UPK
%token			RIGHTK
%token			DOWNK 		
%token			LEFTK

%token			AUTOMAX
%token			AUTOMIN

%token			BACKTAB
%token			BEGINDATA
%token			DESELECTALL
%token			ENDDATA
%token			ENTER
%token			ESCAPE
%token			EXTEND
%token			NEXTFIELD
%token			NEXTMENU
%token			PAGERIGHT
%token			PAGELEFT
%token			PREVFIELD
%token			PREVMENU
%token			PRIMARYCOPY
%token			PRIMARYCUT
%token			QUICKCOPY
%token			QUICKCUT
%token			QUICKEXTEND
%token			RESELECT
%token			RESTORE
%token			SELECTALL
%token			SPACE
%token			TAB

%token			KEYA
%token			KEYB
%token			KEYC
%token			KEYD
%token			KEYE
%token			KEYF
%token			KEYG
%token			KEYH
%token			KEYI
%token			KEYJ
%token			KEYK
%token			KEYL
%token			KEYM
%token			KEYN
%token			KEYO
%token			KEYP
%token			KEYQ
%token			KEYR
%token			KEYS
%token			KEYT
%token			KEYU
%token			KEYV
%token			KEYW
%token			KEYX
%token			KEYY
%token			KEYZ

%token			KEYSLASH
%token			KEYBACKSLASH
%token			KEYBRACERIGHT
%token			KEYBRACELEFT
%token			KEYBRACKETRIGHT
%token			KEYBRACKETLEFT
%token			KEYRETURN

%token			KEY0
%token			KEY1
%token			KEY2
%token			KEY3
%token			KEY4
%token			KEY5
%token			KEY6
%token			KEY7
%token			KEY8
%token			KEY9

%token			KEYEXCLAM
%token 			KEYAT
%token			KEYNUMBERSIGN
%token			KEYDOLLAR
%token 			KEYPERCENT
%token			KEYCARET
%token			KEYAMPERSAND
%token			KEYASTERISK
%token 			KEYPARENLEFT
%token 			KEYPARENRIGHT
%token			KEYNONE

%token			SEPARATOR
%token			LABEL
%token			LISTITEM
%token			FILELISTITEM
%token			DIRLISTITEM
%token			DIRFILTERLABEL
%token			DIRFILTERTEXT
%token			DIRFILTERTEXTCHAR
%token			SELECTIONLISTITEM
%token			TEXTCHAR
%token			CSTEXTCHAR
%token			SLIDER
%token			COMBOBOXDROPDOWNBUTTON
%token			OUTLINEBTN
%token			INCREMENTBTN
%token			DECREMENTBTN
%token			PAGESCROLLER
%token			MAJORTABSCROLLERNEXT
%token			MAJORTABSCROLLERPREV
%token			MINORTABSCROLLERNEXT
%token			MINORTABSCROLLERPREV
%token			UPORLEFTARROW
%token			DOWNORRIGHTARROW
%token			UPORLEFTSLIDERAREA
%token			DOWNORRIGHTSLIDERAREA
%token			SCROLLBAR
%token			HORIZSCROLLBAR
%token			HORIZSBLEFTARROW
%token			HORIZSBRIGHTARROW
%token			HORIZSBLEFTSLIDERAREA
%token			HORIZSBRIGHTSLIDERAREA
%token			HORIZSBSLIDER
%token			VERTSCROLLBAR
%token			VERTSBUPARROW
%token			VERTSBDOWNARROW
%token			VERTSBUPSLIDERAREA
%token			VERTSBDOWNSLIDERAREA
%token			VERTSBSLIDER
%token			VERTSCROLLBAR
%token			MENUBAR
%token			SASH
%token			CLOSEBUTTON
%token			HELPBUTTON
%token			OKBUTTON
%token			FILTERBUTTON
%token			CANCELBUTTON
%token			APPLYBUTTON
%token			MESSAGELABEL
%token			SYMBOLLABEL
%token			SELECTIONTEXT
%token			SELECTIONTEXTCHAR
%token			SELECTIONLABEL
%token			SELECTIONLIST
%token			SELECTIONLISTLABEL
%token			FILELIST
%token			FILELISTLABEL
%token			DIRLIST
%token			DIRLISTLABEL
%token			FILESELECTIONTEXT
%token			FILESELECTIONTEXTCHAR
%token			FILESELECTIONLABEL
%token			FILEFILTERTEXT
%token			FILEFILTERTEXTCHAR
%token			FILEFILTERLABEL
%token			WORKAREA
%token			COMMANDWINDOW
%token			MESSAGEWINDOW
%token			WORKWINDOW
%token			SCALE
%token			SCALELABEL
%token			OPTIONBUTTON
%token			SCROLLEDWINDOW
%token 			SCROLLBARSLIDER
%token			SCROLLBARUPORLEFTARROW
%token			SCROLLBARDOWNORRIGHTARROW
%token			SCROLLBARUPORLEFTSLIDERAREA
%token			SCROLLBARDOWNORRIGHTSLIDERAREA

%token 			FLHSBUPORLEFTARROW
%token			FLHSBDOWNORRIGHTARROW
%token			FLHSBUPORLEFTSLIDERAREA
%token 			FLHSBDOWNORRIGHTSLIDERAREA
%token			FLHSBSLIDER
%token			FLVSBUPORLEFTARROW
%token			FLVSBDOWNORRIGHTARROW
%token			FLVSBUPORLEFTSLIDERAREA
%token			FLVSBDOWNORRIGHTSLIDERAREA
%token			FLVSBSLIDER

%token 			DLHSBUPORLEFTARROW
%token			DLHSBDOWNORRIGHTARROW
%token			DLHSBUPORLEFTSLIDERAREA
%token 			DLHSBDOWNORRIGHTSLIDERAREA
%token			DLHSBSLIDER
%token			DLVSBUPORLEFTARROW
%token			DLVSBDOWNORRIGHTARROW
%token			DLVSBUPORLEFTSLIDERAREA
%token			DLVSBDOWNORRIGHTSLIDERAREA
%token			DLVSBSLIDER

%token 			SLHSBUPORLEFTARROW
%token			SLHSBDOWNORRIGHTARROW
%token			SLHSBUPORLEFTSLIDERAREA
%token 			SLHSBDOWNORRIGHTSLIDERAREA
%token			SLHSBSLIDER
%token			SLVSBUPORLEFTARROW
%token			SLVSBDOWNORRIGHTARROW
%token			SLVSBUPORLEFTSLIDERAREA
%token			SLVSBDOWNORRIGHTSLIDERAREA
%token			SLVSBSLIDER

%token			TEAROFFBUTTON

%token			LOCATEPOINTER 	
%token			LOCATEPOINTERABS
%token			LOCATEPOINTERREL
%token			SYSTEM 	
%token			COMPAREVISUAL 	
%token			SETFOCUS 
%token			INPUTSTRING

%token			STOREVISUAL
%token			COMPARESTOREDVISUAL

%token			DRAG 		
%token			DRAGRELATIVE
%token 			DRAGSLIDERVAL
%token 			DRAGABS

%token 			WINDOWICONIFY
%token			WINDOWDEICONIFY
%token			WINDOWMENUPOST
%token			WINDOWMENUUNPOST
%token			WINDOWMENUSELECT
%token			WINDOWMAXIMIZE
%token			WINDOWNORMALIZE
%token			WINDOWRESIZE
%token			WINDOWMOVE
%token			WINDOWRAISE
%token			WINDOWFOCUS
%token 			WINDOWCLICK
%token			WINDOWICHECK

%token			SYSTEMMENU
%token			ICONMENU
%token			WINDOWDECORATION

%token			ICONMENUPOST
%token			ICONMENUUNPOST
%token			ICONMENUSELECT
%token 			ICONMOVE

%token			NORTHWEST
%token			NORTH
%token			NORTHEAST
%token			WEST
%token			EAST
%token			SOUTHWEST
%token			SOUTH
%token			SOUTHEAST

%token			KEYBOARD

%token			OFF
%token			RIGHT
%token			LEFT
%token			UP
%token			DOWN
%token			ANY

%token                  AUTOPERFORMMONITOR
%token                  ON

%%

CommandList		:	/* empty */
			|	CommandList Command	{ line_cnt++; }

Command			:	FlowCommands
			|	MouseCommands
			|	KeyCommands
			|	PointerCommands
			|	PointerRelCommands
			|	AbsCommands
			|	SystemCommands
			|	CompareCommands
                        |	StoreCommands
			|	InputCommands
			|	DragCommands 
			| 	WindowCommands
			|	WindowCheckCommands
			|	WindowSelectCommands
			|	WindowPostCommands
			|	ResizeCommands
			|	MoveCommands
			| 	WaitCommands
                        |       PerformanceCommands
			;

/* Begin FlowCommands */

FlowCommands		:	END		{ BuildFlowCommand(END); }
			|	CONTINUE	{ BuildFlowCommand(CONTINUE); }
			|	MANUAL		{ BuildFlowCommand(MANUAL); }
			;

/* Begin MouseCommands */

MouseCommands		:	MCommandName ModKeyList Button Count 
			{	
				BuildMouseCommand(yytknval1,
						  yymodmaskval,
						  yymaskcnt,
						  yytknval3,
						  yydigitval);
				yymaskcnt = 0;
				yydigitval = 0;
			};

MCommandName		:	PRESSMB		{ yytknval1 = PRESSMB; }
			|	RELEASEMB	{ yytknval1 = RELEASEMB; }
			|	CLICKMB		{ yytknval1 = CLICKMB; }
			;


ModKeyList		:	/* empty */	{ yymaskcnt = 0; }
			|	ModKeyList ModKey
			;

ModKey			:	SHIFTMASK	{ yymodmaskval[yymaskcnt++] 
								= SHIFTMASK; }
			|	LOCKMASK	{ yymodmaskval[yymaskcnt++]
								= LOCKMASK; }
			|	CTRLMASK	{ yymodmaskval[yymaskcnt++]
								= CTRLMASK; }
			|	METAMASK	{ yymodmaskval[yymaskcnt++]
								= METAMASK; }
			|	ALTMASK		{ yymodmaskval[yymaskcnt++]
								= ALTMASK; }
			|	MOD1MASK	{ yymodmaskval[yymaskcnt++]
								= MOD1MASK; }
			|	MOD2MASK	{ yymodmaskval[yymaskcnt++]
								= MOD2MASK; }
			|	MOD3MASK	{ yymodmaskval[yymaskcnt++]
								= MOD3MASK; }
			|	MOD4MASK	{ yymodmaskval[yymaskcnt++]
								= MOD4MASK; }
			|	MOD5MASK	{ yymodmaskval[yymaskcnt++]
								= MOD5MASK; }
			;

Button			:	BTN1		{ yytknval3 = BTN1; }
			|	BTN2		{ yytknval3 = BTN2; }
			|	BTN3		{ yytknval3 = BTN3; }
			|	BTN4		{ yytknval3 = BTN4; }
			|	BTN5		{ yytknval3 = BTN5; }
			;

Count			:	/* empty */	{ yydigitval = 0; }
			|	DIGIT 
			;


/* End MouseCommands */
	
/* Begin KeyCommands */

KeyCommands		:	KCommandName ModKeyList Key 

			{
				BuildKeyCommand(yytknval1,
						yymodmaskval,
						yymaskcnt,
						yytknval3);
				yymaskcnt = 0;
				yydigitval = 0;
			};

KCommandName		:	CLICKKEY	{ yytknval1 = CLICKKEY; }
			|	PRESSKEY	{ yytknval1 = PRESSKEY; }
			|	RELEASEKEY	{ yytknval1 = RELEASEKEY; }
			;

Key			:	BACKSPACE	{ yytknval3 = BACKSPACE; }
			| 	INSERT		{ yytknval3 = INSERT; }
			| 	DELETE		{ yytknval3 = DELETE; } 
			| 	COPY		{ yytknval3 = COPY; }
			| 	CUT		{ yytknval3 = CUT; }
			| 	PASTE		{ yytknval3 = PASTE; }
			| 	ADDMODE		{ yytknval3 = ADDMODE; }
			| 	PRIMARYPASTE	{ yytknval3 = PRIMARYPASTE; }
			| 	QUICKPASTE	{ yytknval3 = QUICKPASTE; }
			| 	PAGEUP		{ yytknval3 = PAGEUP; }
			| 	PAGEDOWN	{ yytknval3 = PAGEDOWN; }
			| 	ENDLINE		{ yytknval3 = ENDLINE; }
			| 	BEGINLINE	{ yytknval3 = BEGINLINE; }
			| 	ACTIVATE	{ yytknval3 = ACTIVATE; }
			| 	MENUBAR		{ yytknval3 = MENUBAR; }
			| 	CLEAR		{ yytknval3 = CLEAR; }
			| 	CANCEL		{ yytknval3 = CANCEL; }
			| 	HELP		{ yytknval3 = HELP; }
			| 	MENU		{ yytknval3 = MENU; }
			| 	SELECT		{ yytknval3 = SELECT; }
			| 	UNDO		{ yytknval3 = UNDO; }
			| 	UPK		{ yytknval3 = UPK; }
			| 	RIGHTK		{ yytknval3 = RIGHTK; }
			| 	DOWNK		{ yytknval3 = DOWNK; }
			|	LEFTK		{ yytknval3 = LEFTK; }
			|	BACKTAB		{ yytknval3 = BACKTAB; }
			|	BEGINDATA	{ yytknval3 = BEGINDATA; }
			|	DESELECTALL	{ yytknval3 = DESELECTALL; }
			|	ENDDATA		{ yytknval3 = ENDDATA; }
			|	ENTER		{ yytknval3 = ENTER; }
			|	ESCAPE		{ yytknval3 = ESCAPE; }
			|	EXTEND		{ yytknval3 = EXTEND; }
			|	NEXTFIELD	{ yytknval3 = NEXTFIELD; }
			|	NEXTMENU	{ yytknval3 = NEXTMENU; }
			|	PAGERIGHT	{ yytknval3 = PAGERIGHT; }
			|	PAGELEFT	{ yytknval3 = PAGELEFT; }
			|	PREVFIELD	{ yytknval3 = PREVFIELD; }
			|	PREVMENU	{ yytknval3 = PREVMENU; }
			|	PRIMARYCOPY	{ yytknval3 = PRIMARYCOPY; }
			|	PRIMARYCUT	{ yytknval3 = PRIMARYCUT; }
			|	QUICKCOPY	{ yytknval3 = QUICKCOPY; }
			|	QUICKCUT	{ yytknval3 = QUICKCUT; }
			|	QUICKEXTEND	{ yytknval3 = QUICKEXTEND; }
			|	RESELECT	{ yytknval3 = RESELECT; }
			|	RESTORE		{ yytknval3 = RESTORE; }
			|	SELECTALL	{ yytknval3 = SELECTALL; }
			|	SPACE		{ yytknval3 = SPACE; }
			|	TAB		{ yytknval3 = TAB; }
			|	KEYA		{ yytknval3 = KEYA; }
			|	KEYB		{ yytknval3 = KEYB; }
			|	KEYC		{ yytknval3 = KEYC; }
			|	KEYD		{ yytknval3 = KEYD; }
			|	KEYE		{ yytknval3 = KEYE; }
			|	KEYF		{ yytknval3 = KEYF; }
			|	KEYG		{ yytknval3 = KEYG; }
			|	KEYH		{ yytknval3 = KEYH; }
			|	KEYI		{ yytknval3 = KEYI; }
			|	KEYJ		{ yytknval3 = KEYJ; }
			|	KEYK		{ yytknval3 = KEYK; }
			|	KEYL		{ yytknval3 = KEYL; }
			|	KEYM		{ yytknval3 = KEYM; }
			|	KEYN		{ yytknval3 = KEYN; }
			|	KEYO		{ yytknval3 = KEYO; }
			|	KEYP		{ yytknval3 = KEYP; }
			|	KEYQ		{ yytknval3 = KEYQ; }
			|	KEYR		{ yytknval3 = KEYR; }
			|	KEYS		{ yytknval3 = KEYS; }
			|	KEYT		{ yytknval3 = KEYT; }
			|	KEYU		{ yytknval3 = KEYU; }
			|	KEYV		{ yytknval3 = KEYV; }
			|	KEYW		{ yytknval3 = KEYW; }
			|	KEYX		{ yytknval3 = KEYX; }
			|	KEYY		{ yytknval3 = KEYY; }
			|	KEYZ		{ yytknval3 = KEYZ; }
			|	KEYSLASH	{ yytknval3 = KEYSLASH; }
			| 	KEYBACKSLASH	{ yytknval3 = KEYBACKSLASH; }
			| 	KEYBRACERIGHT	{ yytknval3 = KEYBRACERIGHT; }
			| 	KEYBRACELEFT	{ yytknval3 = KEYBRACELEFT; }
			| 	KEYBRACKETRIGHT	{ yytknval3 = KEYBRACKETRIGHT; }
			| 	KEYBRACKETLEFT	{ yytknval3 = KEYBRACKETLEFT; }
			|	KEYRETURN	{ yytknval3 = KEYRETURN; }
			|	KEY1		{ yytknval3 = KEY1; }
			|	KEY2		{ yytknval3 = KEY2; }
			|	KEY3		{ yytknval3 = KEY3; }
			|	KEY4		{ yytknval3 = KEY4; }
			|	KEY5		{ yytknval3 = KEY5; }
			|	KEY6		{ yytknval3 = KEY6; }
			|	KEY7		{ yytknval3 = KEY7; }
			|	KEY8		{ yytknval3 = KEY8; }
			|	KEY9		{ yytknval3 = KEY9; }
			|	KEY0		{ yytknval3 = KEY0; }
			|	KEYEXCLAM	{ yytknval3 = KEYEXCLAM; }
			|	KEYAT		{ yytknval3 = KEYAT; }
			|	KEYNUMBERSIGN	{ yytknval3 = KEYNUMBERSIGN; }
			|	KEYDOLLAR	{ yytknval3 = KEYDOLLAR; }
			|	KEYPERCENT	{ yytknval3 = KEYPERCENT; }
			|	KEYCARET	{ yytknval3 = KEYCARET; }
			|	KEYAMPERSAND	{ yytknval3 = KEYAMPERSAND; }
			|	KEYASTERISK	{ yytknval3 = KEYASTERISK; }
			|	KEYPARENLEFT	{ yytknval3 = KEYPARENLEFT; }
			|	KEYPARENRIGHT	{ yytknval3 = KEYPARENRIGHT; }
			|	KEYNONE		{ yytknval3 = KEYNONE; }
			;

/* End KeyCommands */

/* Begin Performance Commands */

PerformanceCommands      :       PerfCommandName PerformSwitch

                        {
                                BuildPerformCommand(yytknval1,
						    yytknval2);
				yymaskcnt = 0;
				yydigitval = 0;
			};

PerfCommandName         :       AUTOPERFORMMONITOR  { yytknval1 = AUTOPERFORMMONITOR; }
                        ;

PerformSwitch           :       ON                  { yytknval2 = ON ; }
                        |       OFF                 { yytknval2 = OFF; }
                        ;

/* End PerformanceCommands */


/* Begin PointerAbsCommands */

AbsCommands		:	AbsCommandName Coordinates ModKeyList DragButton
			{
				BuildAbsCommand(yytknval1,
					        yydigitval2,
					        yydigitval,
						yymodmaskval,
					        yymaskcnt,
						yytknval3);
			};

AbsCommandName		:	LOCATEPOINTERABS 
					    { yytknval1 = LOCATEPOINTERABS; }
			|	DRAGABS	    { yytknval1 = DRAGABS; }
			;

Coordinates		:	XCoordinate YCoordinate 
			;

XCoordinate		:	DIGIT		{ yydigitval2 = yydigitval; }
			;

YCoordinate		:	DIGIT
			;
DragButton		:	/* empty */	{ yytknval3 = 0; }
			|	BTN1		{ yytknval3 = BTN1; }
			|	BTN2		{ yytknval3 = BTN2; }
			|	BTN3		{ yytknval3 = BTN3; }
			|	BTN4		{ yytknval3 = BTN4; }
			|	BTN5		{ yytknval3 = BTN5; }
			;


/* End PointerAbsCommands */

/* Begin PointerRelCommands */

PointerRelCommands	:	PRelCommandName WidgetName Component Coordinates 
			{
				BuildRelCommand(yytknval1,
						yystringval,
						yytknval4,
						yydigitval2,
					        yydigitval);
			};

PRelCommandName		:	LOCATEPOINTERREL
						{ yytknval1 = LOCATEPOINTERREL; }
			;

WidgetName		:	STRING		
			;

Component		:	/* empty */	{ yytknval4 = -1; }
			|	WidgetComponents
			;

WidgetComponents	: SEPARATOR		{ yytknval4 = SEPARATOR; }
			| LABEL			{ yytknval4 = LABEL; }
			| LISTITEM		{ yytknval4 = LISTITEM; }
			| FILELISTITEM		{ yytknval4 = FILELISTITEM; }
			| DIRLISTITEM		{ yytknval4 = DIRLISTITEM; }
			| DIRFILTERLABEL	{ yytknval4 = DIRFILTERLABEL; }
			| DIRFILTERTEXT		{ yytknval4 = DIRFILTERTEXT; }
			| DIRFILTERTEXTCHAR	{ yytknval4 = DIRFILTERTEXTCHAR; }
			| SELECTIONLISTITEM	{ yytknval4 = SELECTIONLISTITEM; }
			| TEXTCHAR		{ yytknval4 = TEXTCHAR; }
			| CSTEXTCHAR		{ yytknval4 = CSTEXTCHAR; }
			| SLIDER		{ yytknval4 = SLIDER; }
                        | COMBOBOXDROPDOWNBUTTON{ yytknval4 = COMBOBOXDROPDOWNBUTTON;}
                        | OUTLINEBTN		{ yytknval4 = OUTLINEBTN;}
                        | INCREMENTBTN		{ yytknval4 = INCREMENTBTN;}
                        | DECREMENTBTN		{ yytknval4 = DECREMENTBTN;}
			| UPORLEFTARROW		{ yytknval4 = UPORLEFTARROW; }
			| DOWNORRIGHTARROW	{ yytknval4 = DOWNORRIGHTARROW;}
			| UPORLEFTSLIDERAREA	{ yytknval4 = UPORLEFTSLIDERAREA;}
			| DOWNORRIGHTSLIDERAREA	{ yytknval4 = DOWNORRIGHTSLIDERAREA;}
			| SCROLLBAR		{ yytknval4 = SCROLLBAR; }
			| HORIZSCROLLBAR	{ yytknval4 = HORIZSCROLLBAR; }
			| HORIZSBLEFTARROW	{ yytknval4 = HORIZSBLEFTARROW; }
			| HORIZSBRIGHTARROW	{ yytknval4 = HORIZSBRIGHTARROW; }
			| HORIZSBLEFTSLIDERAREA { yytknval4 = HORIZSBLEFTSLIDERAREA; }
			| HORIZSBRIGHTSLIDERAREA { yytknval4 = HORIZSBRIGHTSLIDERAREA; }
			| HORIZSBSLIDER		{ yytknval4 = HORIZSBSLIDER; }
			| VERTSCROLLBAR		{ yytknval4 = VERTSCROLLBAR; }
			| VERTSBUPARROW		{ yytknval4 = VERTSBUPARROW; }
			| VERTSBDOWNARROW	{ yytknval4 = VERTSBDOWNARROW; }
			| VERTSBUPSLIDERAREA	{ yytknval4 = VERTSBUPSLIDERAREA; }
			| VERTSBDOWNSLIDERAREA 	{ yytknval4 = VERTSBDOWNSLIDERAREA; }
			| VERTSBSLIDER		{ yytknval4 = VERTSBSLIDER; }
			| MENUBAR		{ yytknval4 = MENUBAR; }
			| SASH			{ yytknval4 = SASH; }
			| CLOSEBUTTON		{ yytknval4 = CLOSEBUTTON; }
			| HELPBUTTON		{ yytknval4 = HELPBUTTON; }
			| OKBUTTON		{ yytknval4 = OKBUTTON; }
			| FILTERBUTTON		{ yytknval4 = FILTERBUTTON; }
			| CANCELBUTTON		{ yytknval4 = CANCELBUTTON; }
			| APPLYBUTTON		{ yytknval4 = APPLYBUTTON; }
			| MESSAGELABEL		{ yytknval4 = MESSAGELABEL; }
			| SYMBOLLABEL		{ yytknval4 = SYMBOLLABEL; }
			| SELECTIONTEXT		{ yytknval4 = SELECTIONTEXT; }
			| SELECTIONTEXTCHAR	{ yytknval4 = SELECTIONTEXTCHAR; }
			| SELECTIONLABEL	{ yytknval4 = SELECTIONLABEL; }
			| SELECTIONLISTLABEL	{ yytknval4 = SELECTIONLISTLABEL;}
			| SELECTIONLIST		{ yytknval4 = SELECTIONLIST; }
			| FILELIST		{ yytknval4 = FILELIST; }
			| FILELISTLABEL		{ yytknval4 = FILELISTLABEL; }
			| DIRLIST		{ yytknval4 = DIRLIST; }
			| DIRLISTLABEL		{ yytknval4 = DIRLISTLABEL; }
			| FILESELECTIONTEXT	{ yytknval4 = FILESELECTIONTEXT; }
			| FILESELECTIONTEXTCHAR	{ yytknval4 = FILESELECTIONTEXTCHAR; }
			| FILESELECTIONLABEL	{ yytknval4 = FILESELECTIONLABEL; }
			| FILEFILTERTEXT	{ yytknval4 = FILEFILTERTEXT; }
			| FILEFILTERTEXTCHAR	{ yytknval4 = FILEFILTERTEXTCHAR; }
			| FILEFILTERLABEL	{ yytknval4 = FILEFILTERLABEL; }
			| WORKAREA		{ yytknval4 = WORKAREA; }
			| COMMANDWINDOW		{ yytknval4 = COMMANDWINDOW; }
			| MESSAGEWINDOW		{ yytknval4 = MESSAGEWINDOW; }
			| WORKWINDOW		{ yytknval4 = WORKWINDOW; }
			| SCALE			{ yytknval4 = SCALE; }
			| SCALELABEL		{ yytknval4 = SCALELABEL; }
			| OPTIONBUTTON		{ yytknval4 = OPTIONBUTTON; }
			| SCROLLEDWINDOW	{ yytknval4 = SCROLLEDWINDOW; }
			| SCROLLBARSLIDER	{ yytknval4 = SCROLLBARSLIDER; }
			| SCROLLBARUPORLEFTARROW
				{ yytknval4 = SCROLLBARUPORLEFTARROW; }
			| SCROLLBARDOWNORRIGHTARROW
				{ yytknval4 = SCROLLBARDOWNORRIGHTARROW; }
			| SCROLLBARUPORLEFTSLIDERAREA
				{ yytknval4 = SCROLLBARUPORLEFTSLIDERAREA; }
			| SCROLLBARDOWNORRIGHTSLIDERAREA
				{ yytknval4 = SCROLLBARDOWNORRIGHTSLIDERAREA; }
			| FLHSBUPORLEFTARROW
				{ yytknval4 = FLHSBUPORLEFTARROW; }
			| FLHSBDOWNORRIGHTARROW
				{ yytknval4 = FLHSBDOWNORRIGHTARROW; }
			| FLHSBUPORLEFTSLIDERAREA
				{ yytknval4 = FLHSBUPORLEFTSLIDERAREA; }
			| FLHSBDOWNORRIGHTSLIDERAREA
				{ yytknval4 = FLHSBDOWNORRIGHTSLIDERAREA; }
			| FLHSBSLIDER
				{ yytknval4 = FLHSBSLIDER; }
			| FLVSBUPORLEFTARROW
				{ yytknval4 = FLVSBUPORLEFTARROW; }
			| FLVSBDOWNORRIGHTARROW
				{ yytknval4 = FLVSBDOWNORRIGHTARROW; }
			| FLVSBUPORLEFTSLIDERAREA
				{ yytknval4 = FLVSBUPORLEFTSLIDERAREA; }
			| FLVSBDOWNORRIGHTSLIDERAREA
				{ yytknval4 = FLVSBDOWNORRIGHTSLIDERAREA; }
			| FLVSBSLIDER
				{ yytknval4 = FLVSBSLIDER; }
			| DLHSBUPORLEFTARROW
				{ yytknval4 = DLHSBUPORLEFTARROW; }
			| DLHSBDOWNORRIGHTARROW
				{ yytknval4 = DLHSBDOWNORRIGHTARROW; }
			| DLHSBUPORLEFTSLIDERAREA
				{ yytknval4 = DLHSBUPORLEFTSLIDERAREA; }
			| DLHSBDOWNORRIGHTSLIDERAREA
				{ yytknval4 = DLHSBDOWNORRIGHTSLIDERAREA; }
			| DLHSBSLIDER
				{ yytknval4 = DLHSBSLIDER; }
			| DLVSBUPORLEFTARROW
				{ yytknval4 = DLVSBUPORLEFTARROW; }
			| DLVSBDOWNORRIGHTARROW
				{ yytknval4 = DLVSBDOWNORRIGHTARROW; }
			| DLVSBUPORLEFTSLIDERAREA
				{ yytknval4 = DLVSBUPORLEFTSLIDERAREA; }
			| DLVSBDOWNORRIGHTSLIDERAREA
				{ yytknval4 = DLVSBDOWNORRIGHTSLIDERAREA; }
			| DLVSBSLIDER
				{ yytknval4 = DLVSBSLIDER; }
			| SLHSBUPORLEFTARROW
				{ yytknval4 = SLHSBUPORLEFTARROW; }
			| SLHSBDOWNORRIGHTARROW
				{ yytknval4 = SLHSBDOWNORRIGHTARROW; }
			| SLHSBUPORLEFTSLIDERAREA
				{ yytknval4 = SLHSBUPORLEFTSLIDERAREA; }
			| SLHSBDOWNORRIGHTSLIDERAREA
				{ yytknval4 = SLHSBDOWNORRIGHTSLIDERAREA; }
			| SLHSBSLIDER
				{ yytknval4 = SLHSBSLIDER; }
			| SLVSBUPORLEFTARROW
				{ yytknval4 = SLVSBUPORLEFTARROW; }
			| SLVSBDOWNORRIGHTARROW
				{ yytknval4 = SLVSBDOWNORRIGHTARROW; }
			| SLVSBUPORLEFTSLIDERAREA
				{ yytknval4 = SLVSBUPORLEFTSLIDERAREA; }
			| SLVSBDOWNORRIGHTSLIDERAREA
				{ yytknval4 = SLVSBDOWNORRIGHTSLIDERAREA; }
			| SLVSBSLIDER
				{ yytknval4 = SLVSBSLIDER; }
			| TEAROFFBUTTON
				{ yytknval4 = TEAROFFBUTTON; }
			;

Coordinates		:	XCoordinate YCoordinate 
			;

XCoordinate		:	DIGIT		{ yydigitval2 = yydigitval; }
			;

YCoordinate		:	DIGIT
			;


/* End PointerRelCommands */

/* Begin PointerCommands */

PointerCommands		:	PCommandName WidgetName Component Location 
			{
				BuildPointerCommand(yytknval1,
						    yystringval,
						    yytknval4,
						    yydigitval3);
				yydigitval3 = 0;
			};

PCommandName		:	LOCATEPOINTER	{ yytknval1 = LOCATEPOINTER; }
			;

WidgetName		:	STRING		
			;

Component		:	/* empty */	{ yytknval4 = -1; }
			|	WidgetComponents
			;

WidgetComponents	: SEPARATOR		{ yytknval4 = SEPARATOR; }
			| LABEL			{ yytknval4 = LABEL; }
			| LISTITEM		{ yytknval4 = LISTITEM; }
			| FILELISTITEM		{ yytknval4 = FILELISTITEM; }
			| DIRLISTITEM		{ yytknval4 = DIRLISTITEM; }
			| DIRFILTERLABEL	{ yytknval4 = DIRFILTERLABEL; }
			| DIRFILTERTEXT		{ yytknval4 = DIRFILTERTEXT; }
			| DIRFILTERTEXTCHAR	{ yytknval4 = DIRFILTERTEXTCHAR; }
			| SELECTIONLISTITEM	{ yytknval4 = SELECTIONLISTITEM; }
			| TEXTCHAR		{ yytknval4 = TEXTCHAR; }
			| CSTEXTCHAR		{ yytknval4 = CSTEXTCHAR; }
			| SLIDER		{ yytknval4 = SLIDER; }
                        | COMBOBOXDROPDOWNBUTTON{ yytknval4 = COMBOBOXDROPDOWNBUTTON;}
                        | OUTLINEBTN		{ yytknval4 = OUTLINEBTN;}
                        | INCREMENTBTN		{ yytknval4 = INCREMENTBTN;}
                        | DECREMENTBTN		{ yytknval4 = DECREMENTBTN;}
			| UPORLEFTARROW		{ yytknval4 = UPORLEFTARROW; }
			| DOWNORRIGHTARROW	{ yytknval4 = DOWNORRIGHTARROW;}
			| UPORLEFTSLIDERAREA	{ yytknval4 = UPORLEFTSLIDERAREA;}
			| DOWNORRIGHTSLIDERAREA	{ yytknval4 = DOWNORRIGHTSLIDERAREA;}
			| SCROLLBAR		{ yytknval4 = SCROLLBAR; }
			| HORIZSCROLLBAR	{ yytknval4 = HORIZSCROLLBAR; }
			| HORIZSBLEFTARROW	{ yytknval4 = HORIZSBLEFTARROW; }
			| HORIZSBRIGHTARROW	{ yytknval4 = HORIZSBRIGHTARROW; }
			| HORIZSBLEFTSLIDERAREA { yytknval4 = HORIZSBLEFTSLIDERAREA; }
			| HORIZSBRIGHTSLIDERAREA { yytknval4 = HORIZSBRIGHTSLIDERAREA; }
			| HORIZSBSLIDER		{ yytknval4 = HORIZSBSLIDER; }
			| VERTSCROLLBAR		{ yytknval4 = VERTSCROLLBAR; }
			| VERTSBUPARROW		{ yytknval4 = VERTSBUPARROW; }
			| VERTSBDOWNARROW	{ yytknval4 = VERTSBDOWNARROW; }
			| VERTSBUPSLIDERAREA	{ yytknval4 = VERTSBUPSLIDERAREA; }
			| VERTSBDOWNSLIDERAREA 	{ yytknval4 = VERTSBDOWNSLIDERAREA; }
			| VERTSBSLIDER		{ yytknval4 = VERTSBSLIDER; }
			| MENUBAR		{ yytknval4 = MENUBAR; }
			| SASH			{ yytknval4 = SASH; }
			| CLOSEBUTTON		{ yytknval4 = CLOSEBUTTON; }
			| HELPBUTTON		{ yytknval4 = HELPBUTTON; }
			| OKBUTTON		{ yytknval4 = OKBUTTON; }
			| FILTERBUTTON		{ yytknval4 = FILTERBUTTON; }
			| CANCELBUTTON		{ yytknval4 = CANCELBUTTON; }
			| APPLYBUTTON		{ yytknval4 = APPLYBUTTON; }
			| MESSAGELABEL		{ yytknval4 = MESSAGELABEL; }
			| SYMBOLLABEL		{ yytknval4 = SYMBOLLABEL; }
			| SELECTIONTEXT		{ yytknval4 = SELECTIONTEXT; }
			| SELECTIONTEXTCHAR	{ yytknval4 = SELECTIONTEXTCHAR; }
			| SELECTIONLABEL	{ yytknval4 = SELECTIONLABEL; }
			| SELECTIONLISTLABEL	{ yytknval4 = SELECTIONLISTLABEL;}
			| SELECTIONLIST		{ yytknval4 = SELECTIONLIST; }
			| FILELIST		{ yytknval4 = FILELIST; }
			| FILELISTLABEL		{ yytknval4 = FILELISTLABEL; }
			| DIRLIST		{ yytknval4 = DIRLIST; }
			| DIRLISTLABEL		{ yytknval4 = DIRLISTLABEL; }
			| FILESELECTIONTEXT	{ yytknval4 = FILESELECTIONTEXT; }
			| FILESELECTIONTEXTCHAR	{ yytknval4 = FILESELECTIONTEXTCHAR; }
			| FILESELECTIONLABEL	{ yytknval4 = FILESELECTIONLABEL; }
			| FILEFILTERTEXT	{ yytknval4 = FILEFILTERTEXT; }
			| FILEFILTERTEXTCHAR	{ yytknval4 = FILEFILTERTEXTCHAR; }
			| FILEFILTERLABEL	{ yytknval4 = FILEFILTERLABEL; }
			| WORKAREA		{ yytknval4 = WORKAREA; }
			| COMMANDWINDOW		{ yytknval4 = COMMANDWINDOW; }
			| MESSAGEWINDOW		{ yytknval4 = MESSAGEWINDOW; }
			| WORKWINDOW		{ yytknval4 = WORKWINDOW; }
			| SCALE			{ yytknval4 = SCALE; }
			| SCALELABEL		{ yytknval4 = SCALELABEL; }
			| OPTIONBUTTON		{ yytknval4 = OPTIONBUTTON; }
			| SCROLLEDWINDOW	{ yytknval4 = SCROLLEDWINDOW; }
			| SCROLLBARSLIDER	{ yytknval4 = SCROLLBARSLIDER; }
			| SCROLLBARUPORLEFTARROW
				{ yytknval4 = SCROLLBARUPORLEFTARROW; }
			| SCROLLBARDOWNORRIGHTARROW
				{ yytknval4 = SCROLLBARDOWNORRIGHTARROW; }
			| SCROLLBARUPORLEFTSLIDERAREA
				{ yytknval4 = SCROLLBARUPORLEFTSLIDERAREA; }
			| SCROLLBARDOWNORRIGHTSLIDERAREA
				{ yytknval4 = SCROLLBARDOWNORRIGHTSLIDERAREA; }
			| FLHSBUPORLEFTARROW
				{ yytknval4 = FLHSBUPORLEFTARROW; }
			| FLHSBDOWNORRIGHTARROW
				{ yytknval4 = FLHSBDOWNORRIGHTARROW; }
			| FLHSBUPORLEFTSLIDERAREA
				{ yytknval4 = FLHSBUPORLEFTSLIDERAREA; }
			| FLHSBDOWNORRIGHTSLIDERAREA
				{ yytknval4 = FLHSBDOWNORRIGHTSLIDERAREA; }
			| FLHSBSLIDER
				{ yytknval4 = FLHSBSLIDER; }
			| FLVSBUPORLEFTARROW
				{ yytknval4 = FLVSBUPORLEFTARROW; }
			| FLVSBDOWNORRIGHTARROW
				{ yytknval4 = FLVSBDOWNORRIGHTARROW; }
			| FLVSBUPORLEFTSLIDERAREA
				{ yytknval4 = FLVSBUPORLEFTSLIDERAREA; }
			| FLVSBDOWNORRIGHTSLIDERAREA
				{ yytknval4 = FLVSBDOWNORRIGHTSLIDERAREA; }
			| FLVSBSLIDER
				{ yytknval4 = FLVSBSLIDER; }
			| DLHSBUPORLEFTARROW
				{ yytknval4 = DLHSBUPORLEFTARROW; }
			| DLHSBDOWNORRIGHTARROW
				{ yytknval4 = DLHSBDOWNORRIGHTARROW; }
			| DLHSBUPORLEFTSLIDERAREA
				{ yytknval4 = DLHSBUPORLEFTSLIDERAREA; }
			| DLHSBDOWNORRIGHTSLIDERAREA
				{ yytknval4 = DLHSBDOWNORRIGHTSLIDERAREA; }
			| DLHSBSLIDER
				{ yytknval4 = DLHSBSLIDER; }
			| DLVSBUPORLEFTARROW
				{ yytknval4 = DLVSBUPORLEFTARROW; }
			| DLVSBDOWNORRIGHTARROW
				{ yytknval4 = DLVSBDOWNORRIGHTARROW; }
			| DLVSBUPORLEFTSLIDERAREA
				{ yytknval4 = DLVSBUPORLEFTSLIDERAREA; }
			| DLVSBDOWNORRIGHTSLIDERAREA
				{ yytknval4 = DLVSBDOWNORRIGHTSLIDERAREA; }
			| DLVSBSLIDER
				{ yytknval4 = DLVSBSLIDER; }
			| SLHSBUPORLEFTARROW
				{ yytknval4 = SLHSBUPORLEFTARROW; }
			| SLHSBDOWNORRIGHTARROW
				{ yytknval4 = SLHSBDOWNORRIGHTARROW; }
			| SLHSBUPORLEFTSLIDERAREA
				{ yytknval4 = SLHSBUPORLEFTSLIDERAREA; }
			| SLHSBDOWNORRIGHTSLIDERAREA
				{ yytknval4 = SLHSBDOWNORRIGHTSLIDERAREA; }
			| SLHSBSLIDER
				{ yytknval4 = SLHSBSLIDER; }
			| SLVSBUPORLEFTARROW
				{ yytknval4 = SLVSBUPORLEFTARROW; }
			| SLVSBDOWNORRIGHTARROW
				{ yytknval4 = SLVSBDOWNORRIGHTARROW; }
			| SLVSBUPORLEFTSLIDERAREA
				{ yytknval4 = SLVSBUPORLEFTSLIDERAREA; }
			| SLVSBDOWNORRIGHTSLIDERAREA
				{ yytknval4 = SLVSBDOWNORRIGHTSLIDERAREA; }
			| SLVSBSLIDER
				{ yytknval4 = SLVSBSLIDER; }
			| TEAROFFBUTTON
				{ yytknval4 = TEAROFFBUTTON; }
			;

Location		: 	/* empty */	{ yydigitval3 = -1; }
			| 	DIGIT		{ yydigitval3 = yydigitval; }
			| 	OFF		{ yydigitval3 = OFF; }
			| 	RIGHT		{ yydigitval3 = RIGHT; }
			| 	LEFT		{ yydigitval3 = LEFT; }
			| 	UP		{ yydigitval3 = UP; }
			| 	DOWN		{ yydigitval3 = DOWN; }
			| 	ANY		{ yydigitval3 = ANY; }
			| 	AUTOMAX		{ yydigitval3 = AUTOMAX; }
			| 	AUTOMIN		{ yydigitval3 = AUTOMIN; }
			;


/* End PointerCommands */

/* Begin SystemCommands */

SystemCommands		:	SCommandName SCommand SCommandArgs

			{
				BuildSystemCommand(yytknval1,
						   yystringval2,
						   yysysarglist,
						   yysysargs);
				yysysargs = 0;
			};

SCommandName		:	SYSTEM		{ yytknval1 = SYSTEM; }
			;

SCommand		:	STRING 		{ yystringval2 = (char *)malloc(
						     sizeof(char) * 
						     (strlen(yystringval) + 1));
						  strcpy(yystringval2,
							 yystringval); }
			;

SCommandArgs		:	/* empty */
			|	SCommandArgs SCommandArg
			;

SCommandArg		:	STRING		{ yysysarglist[yysysargs++] =
						  	yystringval; }
			|	DIGIT		{ 
						  temp = (char *) malloc
							  (sizeof(char) * 10);
						  sprintf(temp, "%d", 
							       yydigitval);
						  yysysarglist[yysysargs++] =
							temp; }
			;


/* End SystemCommands */

/* Begin StoreCommands */

StoreCommands		:	StoreCommandName WidgetName1 Component Identifier

			{
				BuildStoreCommand(yytknval1,
						  yystringval2,
						  yytknval4,
						  yystringval);
			};

StoreCommandName	:	STOREVISUAL          
                                               { yytknval1 = STOREVISUAL; }
                        |       COMPARESTOREDVISUAL  
                                               { yytknval1 = COMPARESTOREDVISUAL; }
			;

WidgetName1		:	STRING	       { yystringval2 = (char *)malloc(
						    sizeof(char) * 
						    (strlen(yystringval) + 1));
						  strcpy(yystringval2,
						         yystringval); }

Identifier	        :	STRING
                        ;

/* End StoreCommands */

/* Begin CompareCommands */

CompareCommands		:	CCommandName WidgetName Component

			{
				BuildCompareCommand(yytknval1,
						    yystringval,
						    yytknval4);
			};

CCommandName		:	COMPAREVISUAL	{ yytknval1 = COMPAREVISUAL; }
			;


/* End CompareCommands */


/* Begin DragCommands */

DragCommands		:	DCommandName DragList 

			{	
				BuildDragCommand(yytknval1,
						 yydigitval2,
						 yydigitval,
						 yystringval,
						 yytknval4,
						 yydigitval3,
						 yytknval3,
						 yymodmaskval, 
						 yymaskcnt);
				yydigitval2 = yydigitval = yydigitval3 = 0;
				yymaskcnt = 0;
			};


DCommandName		:	DRAG		{ yytknval1 = DRAG; }
			|	DRAGRELATIVE	{ yytknval1 = DRAGRELATIVE; }
			| 	DRAGSLIDERVAL	{ yytknval1 = DRAGSLIDERVAL; }
			;

DragList		:	DragRelativeList
						{ yystringval = NULL;
					      	  yystringval2 = NULL; }
			|	DragComponentList
						{ yydigitval = 0;
						  yydigitval2 = 0; }
			;

DragRelativeList	:	Direction Xdirection Ydirection ModKeyList DragButton
			;

Direction		: 	RIGHT      	{ yydigitval3 = RIGHT; }
                        | 	LEFT       	{ yydigitval3 = LEFT; }
                        | 	UP              { yydigitval3 = UP; }
                        | 	DOWN            { yydigitval3 = DOWN; }
			|	ANY	      	{ yydigitval3 = ANY; }
                        | 	AUTOMAX         { yydigitval3 = AUTOMAX; }
                        | 	AUTOMIN         { yydigitval3 = AUTOMIN; }
                        ;

Xdirection              :      	DIGIT           { yydigitval2 = yydigitval; }
			|	AUTOMAX		{ yydigitval2 = AUTOMAX; }
			|	AUTOMIN		{ yydigitval2 = AUTOMIN; }
                        ;

Ydirection              :       /* empty */     { yydigitval = -1; }
                        |       DIGIT
                        ;

DragComponentList	:	WidgetName Component Location ModKeyList DragButton
			;

/* End Drag Commands */

/* Begin Input Commands */

InputCommands		:	InputCommandName InputStringData

			{	
				BuildInputCommand(yytknval1,
						  yyinputstring);
			};

InputCommandName	:	INPUTSTRING	{ yytknval1 = INPUTSTRING; }
			;

InputStringData		:	ISTRING
			;

/* End Input Commands */

/* Begin Window Manager Commands */

WindowCommands		:	WCommandName WidgetName 
			{
				BuildWindowCommand(yytknval1,
						   yystringval);
			};


WCommandName		:	WINDOWICONIFY	{ yytknval1 = WINDOWICONIFY; }
			|	WINDOWDEICONIFY	{ yytknval1 = WINDOWDEICONIFY; }
			|	WINDOWMENUUNPOST
					      { yytknval1 = WINDOWMENUUNPOST; }
			|	WINDOWMAXIMIZE	{ yytknval1 = WINDOWMAXIMIZE; }
			|	WINDOWNORMALIZE	{ yytknval1 = WINDOWNORMALIZE; }
			|	WINDOWRAISE	{ yytknval1 = WINDOWRAISE; }
			|	WINDOWFOCUS	{ yytknval1 = WINDOWFOCUS; }
			|	WINDOWCLICK	{ yytknval1 = WINDOWCLICK; }
			|	ICONMENUUNPOST	{ yytknval1 = ICONMENUUNPOST; }
			;

/* End Window Manager Commands */

/* Begin Window Check Commands */


WindowCheckCommands	:	WCheckCommandName Wname Group Iname
			{
				BuildWindowCheckCommand(yytknval1,
						   	yystringval2,
						   	yytknval2,
						   	yystringval);
			};


WCheckCommandName	:	WINDOWICHECK	{ yytknval1 = WINDOWICHECK; }
			;

Wname			:	STRING 		{ yystringval2 = (char *)malloc(
						     sizeof(char) * 
						     (strlen(yystringval) + 1));
						  strcpy(yystringval2,
							 yystringval); }
			;

Iname			:	STRING
			;


Group			:	SYSTEMMENU	{ yytknval2 = SYSTEMMENU; }
			|	ICONMENU	{ yytknval2 = ICONMENU; }
			|	WINDOWDECORATION 
					       { yytknval2 = WINDOWDECORATION; }
			;


/* End Window Check Commands */

/* Begin Window Select Commands */

WindowSelectCommands	:	WSelectCommandName WidgetName1 Name ItemNumber Keyboard
			{
				BuildWindowSelectCommand(yytknval1,
						   	 yystringval2,
						   	 yystringval,
						   	 yydigitval,
							 yytknval2);
			};

WSelectCommandName	:	WINDOWMENUSELECT
					       { yytknval1 = WINDOWMENUSELECT; }
			|	ICONMENUSELECT 	
					       { yytknval1 = ICONMENUSELECT; }
			;

WidgetName1		:	STRING		{ yystringval2 = (char *)malloc(
						     sizeof(char) * 
						     (strlen(yystringval) + 1));
						     strcpy(yystringval2, 
							 yystringval); }

Name			:	/* empty */	{ yystringval = NULL; }
			|	STRING		{ yydigitval = -1; }
			;

ItemNumber		:	/* empty */	{ yydigitval = -1; }
			|	DIGIT
			;

Keyboard		:	/* empty */	{ yytknval2 = -1; }
			|	KEYBOARD	{ yytknval2 = KEYBOARD; }

/* End Window Select Commands */

/* Begin Window Post Commands */

WindowPostCommands	:	PostCommandName WidgetName Keyboard
			{
				BuildPostCommand(yytknval1, 
						 yystringval,
						 yytknval2);
			};

PostCommandName		:	WINDOWMENUPOST 	{ yytknval1 = WINDOWMENUPOST; }
			| 	ICONMENUPOST	{ yytknval1 = ICONMENUPOST; }
			;

/* End Window Post Commands */

/* Begin Window Manager Resize Command */

ResizeCommands          :       RCommandName WidgetName Gravity Width Height
			{
				BuildResizeCommand(yytknval1, yystringval,
						   yytknval2, yydigitval2,
						   yydigitval);
			};

RCommandName		:	WINDOWRESIZE	{ yytknval1 = WINDOWRESIZE; }
			;

Gravity			:	/* empty */	{ yytknval2 = 0; }
			|	NORTH		{ yytknval2 = NORTH; }
			|	SOUTH		{ yytknval2 = SOUTH; }
			|	EAST		{ yytknval2 = EAST; }
			|	WEST		{ yytknval2 = WEST; }
			|	NORTHEAST	{ yytknval2 = NORTHEAST; }
			|	NORTHWEST	{ yytknval2 = NORTHWEST; }
			|	SOUTHEAST	{ yytknval2 = SOUTHEAST; }
			|	SOUTHWEST	{ yytknval2 = SOUTHWEST; }
			;

Width			:	DIGIT		{ yydigitval2 = yydigitval; }
			;

Height			:	/* empty */	{ yydigitval = -1; }
			|	DIGIT
			;

/* End Resize Command */

/* Begin Window Manager Move Command */

MoveCommands          	:       MoveCommandName WidgetName Coordinates
			{
				BuildMoveCommand(yytknval1, yystringval,
						 yydigitval2, yydigitval);
			};

MoveCommandName		: 	WINDOWMOVE	{ yytknval1 = WINDOWMOVE; }
			|	ICONMOVE	{ yytknval1 = ICONMOVE; }
			;

/* End Window Manager Move Commands */

/* Begin Wait Commands */

WaitCommands		:	WaitCommandName Count
			{
				BuildWaitCommand(yytknval1, yydigitval);
				yydigitval = 0;
			};

WaitCommandName		:	WAIT		{ yytknval1 = WAIT; }
			;

/* End Wait Commands */

%%

yyerror (s)
char *s;
{

	fprintf(stderr, "%s found parsing input - line %d - Exiting...\n", 
		s, line_cnt);
	exit(0);

}
