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
static char rcsid[] = "$XConsortium: AutoBuildCmd.c /main/10 1995/07/14 11:27:03 drk $"
#endif
#endif
#include "mvslib.h"
#include <X11/Intrinsic.h>
#include "Automation.h"
#include "AutoToken.h"

int      yytknval1;              /* holder for token information */
int      yytknval2;              /* holder for token information */
int      yytknval3;              /* holder for token information */
int      yytknval4;              /* holder for token information */

char     *yystringval;           /* contains all string data */
char     *yystringval2;          /* temporary string holder */
char     *yystringval3;          /* temporary string holder */
char	 *yyinputstring;	 /* contains input string data */
int      yydigitval;             /* count info for Key and Button */
int      yydigitval2;            /* temporary count holder */
int      yydigitval3;            /* temporary count holder */

int      yymodmaskval[5];        /* array of Key or Button Masks */
int      yymaskcnt;              /* number of Key or Button Masks */

char	 *yysysarglist[5];	 /* list of system paramters */
int 	 yysysargs;		 /* number of system arguments */


void
BuildFlowCommand(
        int command )
{

    AutoCommandStructPtr        FlowCommand;
    AutoCommandStructPtr        InsertPointer;

    InsertPointer = AutoCommandRoot;
    while (InsertPointer->next != NULL)
        InsertPointer = InsertPointer->next;

    FlowCommand = (AutoCommandStructPtr) malloc(sizeof(AutoCommandStruct));
    FlowCommand->CommandName = command;

    FlowCommand->Mask[0] = 0;
    FlowCommand->NumMask = 0;
    FlowCommand->ButtonNumber = 0;
    FlowCommand->Key = 0;
    FlowCommand->KeyOrButtonCount = 0;
    FlowCommand->WidgetName = NULL;
    FlowCommand->WidgetComponent = -1;
    FlowCommand->SystemCommand = NULL;
    FlowCommand->XCoord = 0;
    FlowCommand->YCoord = 0;
    FlowCommand->Location = -1;
    FlowCommand->Gravity = 0;
    FlowCommand->OnOffSwitch = 0;
    FlowCommand->Identifier = NULL;

    FlowCommand->next = NULL;

    InsertPointer->next = FlowCommand;

}


void
BuildMouseCommand(
        int command,
        int mask[5],
        int num_mask,
        int button,
        int count )
{

    AutoCommandStructPtr	MouseCommand;
    AutoCommandStructPtr	InsertPointer;
    int				i;

    InsertPointer = AutoCommandRoot;
    while (InsertPointer->next != NULL)
	InsertPointer = InsertPointer->next;

    MouseCommand = (AutoCommandStructPtr) malloc(sizeof(AutoCommandStruct));
    MouseCommand->CommandName = command;

    for (i = 0 ; i < num_mask; i++)
	MouseCommand->Mask[i] = mask[i];

    MouseCommand->NumMask = num_mask;
    MouseCommand->ButtonNumber = button;
    MouseCommand->KeyOrButtonCount = count;

    /* Null out fields not used for a Mouse Command */

    MouseCommand->Key = 0;
    MouseCommand->WidgetName = NULL;
    MouseCommand->WidgetComponent = -1;
    MouseCommand->SystemCommand = NULL;
    MouseCommand->SystemArgs[0] = NULL;
    MouseCommand->SystemNumArgs = 0;
    MouseCommand->XCoord = 0;
    MouseCommand->YCoord = 0;
    MouseCommand->Location = -1;
    MouseCommand->Gravity = 0;
    MouseCommand->OnOffSwitch = 0;
    MouseCommand->Identifier = NULL;

    MouseCommand->next = NULL;

    InsertPointer->next = MouseCommand;
    
}


void
BuildKeyCommand(
        int command,
        int mask[5],
        int num_mask,
        int key )
{

    AutoCommandStructPtr	KeyCommand;
    AutoCommandStructPtr	InsertPointer;
    int				i;

    InsertPointer = AutoCommandRoot;
    while (InsertPointer->next != NULL)
	InsertPointer = InsertPointer->next;

    KeyCommand = (AutoCommandStructPtr) malloc(sizeof(AutoCommandStruct));
    KeyCommand->CommandName = command;

    for (i = 0 ; i < num_mask; i++)
	KeyCommand->Mask[i] = mask[i];

    KeyCommand->NumMask = num_mask;
    KeyCommand->Key = key;

    /* Null out fields not used for a Key Command */

    KeyCommand->KeyOrButtonCount = 0;
    KeyCommand->ButtonNumber = 0;
    KeyCommand->WidgetName = NULL;
    KeyCommand->WidgetComponent = -1;
    KeyCommand->SystemCommand = NULL;
    KeyCommand->SystemArgs[0] = NULL;
    KeyCommand->SystemNumArgs = 0;
    KeyCommand->XCoord = 0;
    KeyCommand->YCoord = 0;
    KeyCommand->Location = -1;
    KeyCommand->Gravity = 0;
    KeyCommand->OnOffSwitch = 0;
    KeyCommand->Identifier = NULL;

    KeyCommand->next = NULL;

    InsertPointer->next = KeyCommand;

}

void
BuildPerformCommand(
		    int command,
		    int on_off_switch )
{
  AutoCommandStructPtr     PerformanceCommand;
  AutoCommandStructPtr     InsertPointer;

  InsertPointer = AutoCommandRoot;
  while (InsertPointer->next != NULL)
    InsertPointer = InsertPointer->next;

  PerformanceCommand = (AutoCommandStructPtr) malloc(sizeof(AutoCommandStruct));
  PerformanceCommand->CommandName = command;
  PerformanceCommand->OnOffSwitch = on_off_switch;

  /* Null out fields not used for a Perfomance Command */

  PerformanceCommand->KeyOrButtonCount = 0;
  PerformanceCommand->ButtonNumber = 0;
  PerformanceCommand->WidgetName = NULL;
  PerformanceCommand->WidgetComponent = -1;
  PerformanceCommand->SystemCommand = NULL;
  PerformanceCommand->SystemArgs[0] = NULL;
  PerformanceCommand->SystemNumArgs = 0;
  PerformanceCommand->XCoord = 0;
  PerformanceCommand->YCoord = 0;
  PerformanceCommand->Location = -1;
  PerformanceCommand->Gravity = 0;
  PerformanceCommand->Identifier = NULL;
  
  PerformanceCommand->next = NULL;
  
  InsertPointer->next = PerformanceCommand;
}



void
BuildPointerCommand(
        int command,
        char *widget,
        int component,
        int location )
{

    AutoCommandStructPtr	PointerCommand;
    AutoCommandStructPtr	InsertPointer;

    InsertPointer = AutoCommandRoot;
    while (InsertPointer->next != NULL)
	InsertPointer = InsertPointer->next;

    PointerCommand = (AutoCommandStructPtr) malloc(sizeof(AutoCommandStruct));
    PointerCommand->CommandName = command;
    PointerCommand->WidgetName = widget;
    PointerCommand->WidgetComponent = component;
    PointerCommand->Location = location;

    /* Null out fields not used for a Pointer Command */

    PointerCommand->Mask[0] = 0;
    PointerCommand->NumMask = 0;
    PointerCommand->ButtonNumber = 0;
    PointerCommand->Key = 0;
    PointerCommand->KeyOrButtonCount = 0;
    PointerCommand->SystemCommand = NULL;
    PointerCommand->SystemArgs[0] = NULL;
    PointerCommand->SystemNumArgs = 0;
    PointerCommand->XCoord = 0;
    PointerCommand->YCoord = 0;
    PointerCommand->Gravity = 0;
    PointerCommand->OnOffSwitch = 0;
    PointerCommand->Identifier = NULL;

    PointerCommand->next = NULL;

    InsertPointer->next = PointerCommand;

}


void
BuildRelCommand(
        int command,
        char* widget,
        int component,
        int XCoord,
        int YCoord )
{

    AutoCommandStructPtr	RelCommand;
    AutoCommandStructPtr	InsertPointer;

    InsertPointer = AutoCommandRoot;
    while (InsertPointer->next != NULL)
	InsertPointer = InsertPointer->next;

    RelCommand = (AutoCommandStructPtr) malloc(sizeof(AutoCommandStruct));
    RelCommand->CommandName = command;
    RelCommand->WidgetName = widget;
    RelCommand->WidgetComponent = component;
    RelCommand->XCoord = XCoord;
    RelCommand->YCoord = YCoord;

    /* Null out fields not used for a Relative Pointer Command */

    RelCommand->Location = 0;
    RelCommand->Mask[0] = 0;
    RelCommand->NumMask = 0;
    RelCommand->ButtonNumber = 0;
    RelCommand->Key = 0;
    RelCommand->KeyOrButtonCount = 0;
    RelCommand->SystemCommand = NULL;
    RelCommand->SystemArgs[0] = NULL;
    RelCommand->SystemNumArgs = 0;
    RelCommand->Gravity = 0;
    RelCommand->OnOffSwitch = 0;
    RelCommand->Identifier = NULL;

    RelCommand->next = NULL;

    InsertPointer->next = RelCommand;

}

void
BuildAbsCommand(
        int command,
        int XCoord,
        int YCoord,
        int mask[5],
        int num_mask,
        int button )
{

    AutoCommandStructPtr	AbsCommand;
    AutoCommandStructPtr	InsertPointer;
    int				i;

    InsertPointer = AutoCommandRoot;
    while (InsertPointer->next != NULL)
	InsertPointer = InsertPointer->next;

    AbsCommand = (AutoCommandStructPtr) malloc(sizeof(AutoCommandStruct));
    AbsCommand->CommandName = command;
    AbsCommand->XCoord = XCoord;
    AbsCommand->YCoord = YCoord;
    AbsCommand->NumMask = num_mask;
    AbsCommand->ButtonNumber = button;

    for (i = 0 ; i < num_mask; i++)
	    AbsCommand->Mask[i] = mask[i];

    /* Null out fields not used for a Pointer Command */

    AbsCommand->WidgetName = NULL;
    AbsCommand->WidgetComponent = 0;
    AbsCommand->Location = 0;
    AbsCommand->Key = 0;
    AbsCommand->KeyOrButtonCount = 0;
    AbsCommand->SystemCommand = NULL;
    AbsCommand->SystemArgs[0] = NULL;
    AbsCommand->SystemNumArgs = 0;
    AbsCommand->Gravity = 0;
    AbsCommand->OnOffSwitch = 0;
    AbsCommand->Identifier = NULL;

    AbsCommand->next = NULL;

    InsertPointer->next = AbsCommand;

}


void
BuildSystemCommand(
        int command,
        char *sys_command,
        char *sys_args[5],
        int number_args )
{

    AutoCommandStructPtr	SystemCommand;
    AutoCommandStructPtr	InsertPointer;
    int				i;

    InsertPointer = AutoCommandRoot;
    while (InsertPointer->next != NULL)
	InsertPointer = InsertPointer->next;

    SystemCommand = (AutoCommandStructPtr) malloc(sizeof(AutoCommandStruct));
    SystemCommand->CommandName = command;
    SystemCommand->SystemCommand = sys_command;

    for (i = 0 ; i < number_args; i++)
    	SystemCommand->SystemArgs[i] = sys_args[i];

    SystemCommand->SystemNumArgs = number_args;

    /* Null out fields not used for a System Command */

    SystemCommand->Mask[0] = 0;
    SystemCommand->NumMask = 0;
    SystemCommand->ButtonNumber = 0;
    SystemCommand->Key = 0;
    SystemCommand->KeyOrButtonCount = 0;
    SystemCommand->WidgetName = NULL;
    SystemCommand->WidgetComponent = -1;
    SystemCommand->XCoord = 0;
    SystemCommand->YCoord = 0;
    SystemCommand->Location = -1;
    SystemCommand->Gravity = 0;
    SystemCommand->OnOffSwitch = 0;
    SystemCommand->Identifier = NULL;

    SystemCommand->next = NULL;

    InsertPointer->next = SystemCommand;

}


void
BuildCompareCommand(
        int command,
        char *widget,
        int component )
{
    AutoCommandStructPtr	CompareCommand;
    AutoCommandStructPtr	InsertPointer;

    InsertPointer = AutoCommandRoot;
    while (InsertPointer->next != NULL)
	InsertPointer = InsertPointer->next;

    CompareCommand = (AutoCommandStructPtr) malloc(sizeof(AutoCommandStruct));
    CompareCommand->CommandName = command;
    CompareCommand->WidgetName = widget;
    CompareCommand->WidgetComponent = component;

    /* Null out fields not used for a Compare Command */

    CompareCommand->Mask[0] = 0;
    CompareCommand->NumMask = 0;
    CompareCommand->ButtonNumber = 0;
    CompareCommand->Key = 0;
    CompareCommand->KeyOrButtonCount = 0;
    CompareCommand->SystemCommand = NULL;
    CompareCommand->SystemArgs[0] = NULL;
    CompareCommand->SystemNumArgs = 0;
    CompareCommand->XCoord = 0;
    CompareCommand->YCoord = 0;
    CompareCommand->Location = -1;
    CompareCommand->Gravity = 0;
    CompareCommand->OnOffSwitch = 0;
    CompareCommand->Identifier = NULL;

    CompareCommand->next = NULL;

    InsertPointer->next = CompareCommand;

}


void
BuildStoreCommand(
        int command,
        char *widget,
        int component,
	char *identifier )
{
    AutoCommandStructPtr	StoreCommand;
    AutoCommandStructPtr	InsertPointer;

    InsertPointer = AutoCommandRoot;
    while (InsertPointer->next != NULL)
	InsertPointer = InsertPointer->next;

    StoreCommand = (AutoCommandStructPtr) malloc(sizeof(AutoCommandStruct));
    StoreCommand->CommandName = command;
    StoreCommand->WidgetName = widget;
    StoreCommand->Identifier = identifier;
    StoreCommand->WidgetComponent = component;

    /* Null out fields not used for a Store Command */

    StoreCommand->Mask[0] = 0;
    StoreCommand->NumMask = 0;
    StoreCommand->ButtonNumber = 0;
    StoreCommand->Key = 0;
    StoreCommand->KeyOrButtonCount = 0;
    StoreCommand->SystemCommand = NULL;
    StoreCommand->SystemArgs[0] = NULL;
    StoreCommand->SystemNumArgs = 0;
    StoreCommand->XCoord = 0;
    StoreCommand->YCoord = 0;
    StoreCommand->Location = -1;
    StoreCommand->Gravity = 0;
    StoreCommand->OnOffSwitch = 0;

    StoreCommand->next = NULL;

    InsertPointer->next = StoreCommand;

}


void
BuildFocusCommand(
        int command,
        char *widget,
        int component )
{

    AutoCommandStructPtr	FocusCommand;
    AutoCommandStructPtr	InsertPointer;

    InsertPointer = AutoCommandRoot;
    while (InsertPointer->next != NULL)
	InsertPointer = InsertPointer->next;

    FocusCommand = (AutoCommandStructPtr) malloc(sizeof(AutoCommandStruct));
    FocusCommand->CommandName = command;
    FocusCommand->WidgetName = widget;
    FocusCommand->WidgetComponent = component;

    /* Null out fields not used for a Focus Command */

    FocusCommand->Mask[0] = 0;
    FocusCommand->NumMask = 0;
    FocusCommand->ButtonNumber = 0;
    FocusCommand->Key = 0;
    FocusCommand->KeyOrButtonCount = 0;
    FocusCommand->SystemCommand = NULL;
    FocusCommand->SystemArgs[0] = NULL;
    FocusCommand->SystemNumArgs = 0;
    FocusCommand->XCoord = 0;
    FocusCommand->YCoord = 0;
    FocusCommand->Location = -1;
    FocusCommand->Gravity = 0;
    FocusCommand->OnOffSwitch = 0;
    FocusCommand->Identifier = NULL;

    FocusCommand->next = NULL;

    InsertPointer->next = FocusCommand;

}


void
BuildDragCommand(
        int command,
        int Xcoord,
        int Ycoord,
        char *widget,
        int component,
        int location,
        int button,
        int mask[5],
	int num_mask )
{

    AutoCommandStructPtr	DragCommand;
    AutoCommandStructPtr	InsertPointer;
    int				i;

    InsertPointer = AutoCommandRoot;
    while (InsertPointer->next != NULL)
	InsertPointer = InsertPointer->next;

    DragCommand = (AutoCommandStructPtr) malloc(sizeof(AutoCommandStruct));
    DragCommand->CommandName = command;
    DragCommand->XCoord = Xcoord;
    DragCommand->YCoord = Ycoord;
    DragCommand->WidgetName = widget;
    DragCommand->WidgetComponent = component;
    DragCommand->Location = location;
    DragCommand->ButtonNumber = button;

    for (i = 0 ; i < num_mask; i++)
   	DragCommand->Mask[i] = mask[i];

    DragCommand->NumMask = num_mask;

    /* Null out fields not used for a Focus Command */

    DragCommand->Key = 0;
    DragCommand->KeyOrButtonCount = 0;
    DragCommand->SystemCommand = NULL;
    DragCommand->SystemArgs[0] = NULL;
    DragCommand->SystemNumArgs = 0;
    DragCommand->Gravity = 0;
    DragCommand->OnOffSwitch = 0;
    DragCommand->Identifier = NULL;

    DragCommand->next = NULL;

    InsertPointer->next = DragCommand;

}


void
BuildInputCommand(
        int command,
        char *istringdata )
{

    AutoCommandStructPtr        InputCommand;
    AutoCommandStructPtr        InsertPointer;

    InsertPointer = AutoCommandRoot;
    while (InsertPointer->next != NULL)
        InsertPointer = InsertPointer->next;

    InputCommand = (AutoCommandStructPtr) malloc(sizeof(AutoCommandStruct));
    InputCommand->CommandName = command;
    InputCommand->WidgetName = istringdata;

    InputCommand->Mask[0] = 0;
    InputCommand->NumMask = 0;
    InputCommand->ButtonNumber = 0;
    InputCommand->Key = 0;
    InputCommand->KeyOrButtonCount = 0;
    InputCommand->WidgetComponent = -1;
    InputCommand->SystemCommand = NULL;
    InputCommand->XCoord = 0;
    InputCommand->YCoord = 0;
    InputCommand->Location = -1;
    InputCommand->Gravity = 0;
    InputCommand->OnOffSwitch = 0;
    InputCommand->Identifier = NULL;

    InputCommand->next = NULL;

    InsertPointer->next = InputCommand;

}


void
BuildWindowCommand(
        int command,
        char *widget )
{

    AutoCommandStructPtr	WindowCommand;
    AutoCommandStructPtr	InsertPointer;

    InsertPointer = AutoCommandRoot;
    while (InsertPointer->next != NULL)
	InsertPointer = InsertPointer->next;

    WindowCommand = (AutoCommandStructPtr) malloc(sizeof(AutoCommandStruct));
    WindowCommand->CommandName = command;
    WindowCommand->WidgetName = widget;

    /* Null out fields not used for a Window Command */

    WindowCommand->KeyOrButtonCount = 0;
    WindowCommand->Location = -1;
    WindowCommand->ButtonNumber = 0;
    WindowCommand->WidgetComponent = -1;
    WindowCommand->XCoord = 0;
    WindowCommand->YCoord = 0;
    WindowCommand->Mask[0] = 0;
    WindowCommand->NumMask = 0;
    WindowCommand->Key = 0;
    WindowCommand->SystemCommand = NULL;
    WindowCommand->SystemArgs[0] = NULL;
    WindowCommand->SystemNumArgs = 0;
    WindowCommand->Gravity = 0;
    WindowCommand->OnOffSwitch = 0;
    WindowCommand->Identifier = NULL;

    WindowCommand->next = NULL;

    InsertPointer->next = WindowCommand;
}


void
BuildPostCommand(
        int command,
        char *widget,
	int keyboard )
{

    AutoCommandStructPtr	WindowPostCommand;
    AutoCommandStructPtr	InsertPointer;

    InsertPointer = AutoCommandRoot;
    while (InsertPointer->next != NULL)
	InsertPointer = InsertPointer->next;

    WindowPostCommand = (AutoCommandStructPtr)malloc(sizeof(AutoCommandStruct));
    WindowPostCommand->CommandName = command;
    WindowPostCommand->WidgetName = widget;
    WindowPostCommand->Key = keyboard;

    /* Null out fields not used for a Window Post Command */

    WindowPostCommand->KeyOrButtonCount = 0;
    WindowPostCommand->Location = -1;
    WindowPostCommand->ButtonNumber = 0;
    WindowPostCommand->WidgetComponent = -1;
    WindowPostCommand->XCoord = 0;
    WindowPostCommand->YCoord = 0;
    WindowPostCommand->Mask[0] = 0;
    WindowPostCommand->NumMask = 0;
    WindowPostCommand->SystemCommand = NULL;
    WindowPostCommand->SystemArgs[0] = NULL;
    WindowPostCommand->SystemNumArgs = 0;
    WindowPostCommand->Gravity = 0;
    WindowPostCommand->OnOffSwitch = 0;
    WindowPostCommand->Identifier = NULL;

    WindowPostCommand->next = NULL;

    InsertPointer->next = WindowPostCommand;
}


void
BuildWindowCheckCommand(
        int command,
        char *widget,
        int group,
        char *iname )
{

    AutoCommandStructPtr	WindowCheckCommand;
    AutoCommandStructPtr	InsertPointer;

    InsertPointer = AutoCommandRoot;
    while (InsertPointer->next != NULL)
	InsertPointer = InsertPointer->next;

    WindowCheckCommand = 
		(AutoCommandStructPtr) malloc(sizeof(AutoCommandStruct));

    WindowCheckCommand->CommandName = command;
    WindowCheckCommand->WidgetName = widget;
    WindowCheckCommand->Gravity = group;
    WindowCheckCommand->SystemCommand = iname;

    /* Null out fields not used for a Window Check Command */

    WindowCheckCommand->KeyOrButtonCount = 0;
    WindowCheckCommand->Location = -1;
    WindowCheckCommand->ButtonNumber = 0;
    WindowCheckCommand->WidgetComponent = -1;
    WindowCheckCommand->XCoord = 0;
    WindowCheckCommand->YCoord = 0;
    WindowCheckCommand->Mask[0] = 0;
    WindowCheckCommand->NumMask = 0;
    WindowCheckCommand->Key = 0;
    WindowCheckCommand->SystemArgs[0] = NULL;
    WindowCheckCommand->SystemNumArgs = 0;
    WindowCheckCommand->OnOffSwitch = 0;
    WindowCheckCommand->Identifier = NULL;

    WindowCheckCommand->next = NULL;

    InsertPointer->next = WindowCheckCommand;
}


void
BuildWindowSelectCommand(
        int command,
        char *widget,
        char *name,
        int count,
	int keyboard)
{

    AutoCommandStructPtr	WindowSelectCommand;
    AutoCommandStructPtr	InsertPointer;

    InsertPointer = AutoCommandRoot;
    while (InsertPointer->next != NULL)
	InsertPointer = InsertPointer->next;

    WindowSelectCommand = (AutoCommandStructPtr) 
				malloc(sizeof(AutoCommandStruct));

    WindowSelectCommand->CommandName = command;
    WindowSelectCommand->WidgetName = widget;
    WindowSelectCommand->SystemCommand = name;
    WindowSelectCommand->KeyOrButtonCount = count;
    WindowSelectCommand->Key = keyboard;

    /* Null out fields not used for a Window Command */

    WindowSelectCommand->Location = -1;
    WindowSelectCommand->ButtonNumber = 0;
    WindowSelectCommand->WidgetComponent = -1;
    WindowSelectCommand->XCoord = 0;
    WindowSelectCommand->YCoord = 0;
    WindowSelectCommand->Mask[0] = 0;
    WindowSelectCommand->NumMask = 0;
    WindowSelectCommand->SystemArgs[0] = NULL;
    WindowSelectCommand->SystemNumArgs = 0;
    WindowSelectCommand->Gravity = 0;
    WindowSelectCommand->OnOffSwitch = 0;
    WindowSelectCommand->Identifier = NULL;

    WindowSelectCommand->next = NULL;

    InsertPointer->next = WindowSelectCommand;
}


void
BuildResizeCommand(
        int command,
        char *widget,
        int gravity,
        int width,
        int height )
{

    AutoCommandStructPtr	ResizeCommand;
    AutoCommandStructPtr	InsertPointer;

    InsertPointer = AutoCommandRoot;
    while (InsertPointer->next != NULL)
	InsertPointer = InsertPointer->next;

    ResizeCommand = (AutoCommandStructPtr) malloc(sizeof(AutoCommandStruct));
    ResizeCommand->CommandName = command;
    ResizeCommand->WidgetName = widget;
    ResizeCommand->XCoord = width;
    ResizeCommand->YCoord = height;
    ResizeCommand->Gravity = gravity;

    /* Null out fields not used for a Window Command */

    ResizeCommand->Location = -1;
    ResizeCommand->ButtonNumber = 0;
    ResizeCommand->WidgetComponent = -1;
    ResizeCommand->Mask[0] = 0;
    ResizeCommand->NumMask = 0;
    ResizeCommand->Key = 0;
    ResizeCommand->KeyOrButtonCount = 0;
    ResizeCommand->SystemCommand = NULL;
    ResizeCommand->SystemArgs[0] = NULL;
    ResizeCommand->SystemNumArgs = 0;
    ResizeCommand->OnOffSwitch = 0;
    ResizeCommand->Identifier = NULL;

    ResizeCommand->next = NULL;

    InsertPointer->next = ResizeCommand;
}


void
BuildMoveCommand(
        int command,
        char *widget,
        int xcoord,
        int ycoord )
{

    AutoCommandStructPtr	MoveCommand;
    AutoCommandStructPtr	InsertPointer;

    InsertPointer = AutoCommandRoot;
    while (InsertPointer->next != NULL)
	InsertPointer = InsertPointer->next;

    MoveCommand = (AutoCommandStructPtr) malloc(sizeof(AutoCommandStruct));
    MoveCommand->CommandName = command;
    MoveCommand->WidgetName = widget;
    MoveCommand->XCoord = xcoord;
    MoveCommand->YCoord = ycoord;

    /* Null out fields not used for a Window Command */

    MoveCommand->Gravity = -1;
    MoveCommand->Location = -1;
    MoveCommand->ButtonNumber = 0;
    MoveCommand->WidgetComponent = -1;
    MoveCommand->Mask[0] = 0;
    MoveCommand->NumMask = 0;
    MoveCommand->Key = 0;
    MoveCommand->KeyOrButtonCount = 0;
    MoveCommand->SystemCommand = NULL;
    MoveCommand->SystemArgs[0] = NULL;
    MoveCommand->SystemNumArgs = 0;
    MoveCommand->OnOffSwitch = 0;
    MoveCommand->Identifier = NULL;

    MoveCommand->next = NULL;

    InsertPointer->next = MoveCommand;
}


void
BuildWaitCommand(
        int command,
        int count )
{

    AutoCommandStructPtr	WaitCommand;
    AutoCommandStructPtr	InsertPointer;

    InsertPointer = AutoCommandRoot;
    while (InsertPointer->next != NULL)
	InsertPointer = InsertPointer->next;

    WaitCommand = (AutoCommandStructPtr) malloc(sizeof(AutoCommandStruct));
    WaitCommand->CommandName = command;
    WaitCommand->ButtonNumber = count;

    /* Null out fields not used for a Window Command */

    WaitCommand->WidgetName = NULL;
    WaitCommand->XCoord = 0;
    WaitCommand->YCoord = 0;
    WaitCommand->Gravity = -1;
    WaitCommand->Location = -1;
    WaitCommand->WidgetComponent = -1;
    WaitCommand->Mask[0] = 0;
    WaitCommand->NumMask = 0;
    WaitCommand->Key = 0;
    WaitCommand->KeyOrButtonCount = 0;
    WaitCommand->SystemCommand = NULL;
    WaitCommand->SystemArgs[0] = NULL;
    WaitCommand->SystemNumArgs = 0;
    WaitCommand->OnOffSwitch = 0;
    WaitCommand->Identifier = NULL;

    WaitCommand->next = NULL;

    InsertPointer->next = WaitCommand;
}
