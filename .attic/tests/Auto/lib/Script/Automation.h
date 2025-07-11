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
/*   $XConsortium: Automation.h /main/10 1996/10/30 11:34:39 drk $ */

#define 	CommandSize		20
#define		MaskSize		10
#define 	KeySize			20
#define		ButtonSize		5
#define		SystemSize		100

extern int	yytknval1;		/* holder for token information */
extern int	yytknval2;		/* holder for token information */
extern int	yytknval3;		/* holder for token information */
extern int	yytknval4;		/* holder for token information */

extern char	*yystringval;		/* contains all string data */
extern char	*yystringval2;		/* temporary string holder */
extern char	*yyinputstring;		/* contains input string data */
extern int	yydigitval;		/* count info for Key and Button */
extern int	yydigitval2;		/* temporary count holder */
extern int	yydigitval3;		/* temporary count holder */

extern int	yymodmaskval[5];	/* array of Key or Button Masks */
extern int	yymaskcnt;		/* number of Key or Button Masks */

extern char	*yysysarglist[5];	/* arguments to system call */
extern int 	yysysargs;		/* number of arguments to system call */

extern Boolean  AutoManualMode;
extern Boolean  MonitorOn;              /* performance monitor switch */
extern Boolean  Command_In_Progress;    /* performance monitor status switch */

extern struct   timeval  Start_Time, End_Time; 

/* extern enum     Which_Time { START_TIME, END_TIME }; */

/* 
   Structure containing one command. All fields in this structure
   may not be valid for every command. When a field is not valid 
   for a particular command, that field is NULLed.
*/


typedef struct AutoCommandDefStruct
	{
	int			 CommandName;
	int			 Mask[5];
	int			 NumMask;
	int 			 ButtonNumber;
	int			 Key;
	int 			 KeyOrButtonCount;
	char			 *WidgetName;
	int			 WidgetComponent;
	char			 *SystemCommand;
	char			 *SystemArgs[5];
	int			 SystemNumArgs;
	int 			 XCoord;
	int			 YCoord;
	int			 Location;
	int			 Gravity;
	int                      OnOffSwitch;
	char			 *Identifier;
	struct AutoCommandDefStruct *next;

	} AutoCommandStruct, *AutoCommandStructPtr;


/* Global Pointer to beggining of Linked List of Commands */

extern AutoCommandStructPtr	AutoCommandRoot;

/* Private Functions */


extern void AutoParseAndGenerateCommands(void);
extern void InitTestAutomation(
                        int argc,
                        char **argv);
extern void AutoMoveMaxOrMinText( 
                        Widget widget,
                        int location);
extern int AutoGetMaxListPosition( 
                        Widget widget);
extern void AutoSystem( 
                        char *command_name,
                        char **command_args);
extern void AutoExit( void );
extern void AutoProcessAllEvents( void );
extern void AutoProcessKey( 
                        int key,
                        int mask,
                        int func_type);
extern void AutoInputString( 
                        char *istring);
extern void AutoContinueOrEnd( 
                        Widget widget,
                        int Component);
extern void AutoDrag( 
                        Widget widget,
                        int Component,
                        int Location,
                        int mask,
                        int XButton);
extern void AutoDragAbs( 
                        int XCoord,
                        int YCoord,
                        int mask,
                        int XButton);
extern void AutoDragRelative( 
                        int location,
                        int XCoord,
                        int YCoord,
                        int mask,
                        int XButton);
extern void AutoDragSliderValue( 
                        int location,
                        int XCoord,
                        int YCoord,
                        int mask,
                        int XButton);
extern void AutoPressMB( 
                        int mask,
                        int XButton);
extern void AutoReleaseMB( 
                        int mask,
                        int XButton);
extern void AutoClickMB( 
                        int mask,
                        int XButton,
                        int multi_click);
extern void AutoSetFocus( 
                        Widget widget,
                        int Component);
extern void AutoCompareVisual( 
                        MvsWidgetInfoRecord *w_info);
extern void AutoLocatePointerAbs( 
                        int XCoord,
                        int YCoord);
extern void AutoLocatePointerRel( 
                        Widget widget,
                        int Component,
                        int Location,
                        int XCoord,
                        int YCoord);
extern void AutoLocatePointer( 
                        Widget widget,
                        int Component,
                        int Location,
                        int direction);
extern void AutoDelayCycle(
			int delay_num);
extern void AutoGoManual( void );
extern void AutoWait(int count);

extern void BuildFlowCommand( 
			int flowCommand);
extern void BuildMouseCommand( 
                        int command,
                        int mask[5], 
			int num_mask,
                        int button,
                        int count);
extern void BuildKeyCommand( 
                        int command,
                        int mask[5],
			int num_mask,
                        int key);
extern void BuildPerformCommand(
				int command,
				int on_off_switch);
extern void BuildPointerCommand( 
                        int command,
                        char *widget,
                        int component,
                        int location);
extern void BuildAbsCommand( 
                        int command,
                        int XCoord,
                        int YCoord,
                        int mask[5],
                        int num_mask,
                        int button);
extern void BuildRelCommand( 
                        int command,
                        char *widget,
                        int component,
                        int XCoord,
                        int YCoord);
extern void BuildSystemCommand( 
                        int command,
                        char *sys_command,
                        char *sys_args[5],
                        int number_args);
extern void BuildCompareCommand( 
                        int command,
                        char *widget,
                        int component);
extern void BuildStoreCommand(
			int command,
			char *widget,
			int component,
			char *identifier);
extern void BuildFocusCommand( 
                        int command,
                        char *widget,
                        int component);
extern void BuildDragCommand( 
                        int command,
                        int Xcoord,
                        int Ycoord,
                        char *widget,
                        int component,
                        int location,
                        int button,
                        int mask[5],
			int num_mask);
extern void BuildInputCommand( 
                        int command,
                        char *istringdata);
extern void BuildWindowCommand( 
                        int command,
                        char *widget);
extern void BuildWindowCheckCommand( 
                        int command,
                        char *widget,
                        int group,
                        char *iname);
extern void BuildWindowSelectCommand( 
                        int command,
                        char *widget,
                        char *name,
                        int count,
			int keyboard);
extern void BuildPostCommand( 
                        int command,
                        char *widget,
			int keyboard);
extern void BuildResizeCommand( 
                        int command,
                        char *widget,
                        int gravity,
                        int width,
                        int height);
extern void BuildMoveCommand( 
                        int command,
                        char *widget,
                        int xcoord,
                        int ycoord);
extern void BuildWaitCommand( 
                        int command,
			int count);

extern void AutoError(char *error_string);
extern void AutoWarning(char *warning_string);
extern void AutoTraceMsg(char *trace_string);

extern void AutoTraceButtonAction( 
                        int button,
                        int mask[5],
			int num_mask,
                        int multi_click,
                        int func_type,
                        int line);
extern void AutoTraceKeyAction( 
                        int key,
                        int mask[5],
			int num_mask,
                        int func_type,
                        int line);
extern void AutoTraceLocateAction( 
                        char *widget,
                        int component,
                        int instance,
                        int x,
                        int y,
                        int func_type,
                        int line);
extern void AutoTraceCompareAction( 
                        char *widget,
                        int component,
                        int line);
extern void AutoTraceSystemAction( 
                        char *command,
                        char *args[CommandSize],
                        int num_args,
                        int line);
extern void AutoTraceInputStringAction( 
                        char *string,
                        int line);
extern void AutoTraceWindowAction( 
                        char *widget,
                        int func_type,
                        int line);
extern void AutoTracePostAction( 
                        char *widget,
			int keyboard,
                        int func_type,
                        int line);
extern void AutoTraceControlAction( 
                        int func_type,
                        int line);
extern void AutoTraceDragAction( 
                        int mask[5],
                        int num_mask,
                        char *widget,
                        int component,
                        int dest,
                        int button,
                        int x,
                        int y,
                        int value,
                        int func_type,
                        int line);
extern void AutoTraceMoveAction( 
                        char *widget,
                        int x,
                        int y,
                        int gravity,
                        int func_type,
                        int line);
extern void AutoTraceSelectAction( 
                        char *widget,
                        int item_num,
                        char *item_name,
			int keyboard,
                        int func_type,
                        int line);
extern void AutoTraceWaitAction(
			int count,
			int line);


