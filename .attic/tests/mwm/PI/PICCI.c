/* $XConsortium: PICCI.c /main/5 1995/07/15 21:13:51 drk $ */
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





#include <testlib.h>

Atom target;
int size_of_param;

/* THIS IS JUST TEMPORARY */
#include <Wsm/wsm_proto.h>




typedef enum {ROOT, ALL, ICON, WIN, DEFINE, INCLUDE, REMOVE,
	      ENABLE, DISABLE, INLINE, MAXTOGGLES } RType;

enum {WIN_ID, PARENT, ID, NAME, DEFAULT, MAXFIELDS};

Atom WM_S0,
     MY_PRETTY_ATOM,
     _MOTIF_WM_DEFINE_COMMAND,
     _MOTIF_WM_INCLUDE_COMMAND,
     _MOTIF_WM_DISABLE_COMMAND,
     _MOTIF_WM_ENABLE_COMMAND,
     _MOTIF_WM_INVOKE_COMMAND,
     _MOTIF_WM_REMOVE_COMMAND;

Widget shell_ui, text, toggle[MAXTOGGLES], field[MAXFIELDS];
Widget DrawingArea;

static XtPointer PackCARD32(XtPointer,CARD32);
static XtPointer PackCARD16(XtPointer,CARD16);
static XtPointer PackCARD8(XtPointer,CARD8);
static XtPointer PackString(XtPointer,String);


/*----------------------------------------------------------------------*
 |                              SelectionProc                           |
 *----------------------------------------------------------------------*/
void
SelectionProc (Widget w, XtPointer clientData,
		    XtPointer callData)
{
  printf("Convert request returned.\n");
}

static void
DestinationCB (
     Widget w,
     XtPointer clientData,
     XtPointer callData)
{
  XmDestinationCallbackStruct *dcs = (XmDestinationCallbackStruct *)callData;




  XmTransferSetParameters(dcs->transfer_id,
                          dcs->location_data,   /* pointer to param data. */
                          8,
                          size_of_param,
                          dcs->selection);      /* type - don't care. */

  XmTransferValue(dcs->transfer_id,
                  target,
                  SelectionProc,        /* CB proc invoked when done. */
		  dcs->location_data,   /* clientData - to be freed. */
                  dcs->time);
}


/*----------------------------------------------------------------------*
 |                             SendMessage                              |
 *----------------------------------------------------------------------*/
void
SendMessage (RType rtype, XtPointer msg, int len, Time time)
{

  size_of_param = len;

  switch (rtype)
    {
    case DEFINE:
      {
	target = _MOTIF_WM_DEFINE_COMMAND;
	XmeNamedSink (DrawingArea, WM_S0, 
				   XmCOPY,
				   (XtPointer)msg, 
				   time);
	break;
      }
    case INCLUDE:
      {
	target = _MOTIF_WM_INCLUDE_COMMAND;
	XmeNamedSink (DrawingArea, WM_S0, 
				   XmCOPY,
				   (XtPointer)msg, 
				   time);
	break;
      }
    case REMOVE:
      {
	target = _MOTIF_WM_REMOVE_COMMAND;
        XmeNamedSink (DrawingArea, WM_S0, 
				   XmCOPY,
				   (XtPointer)msg, 
				   time);
       break;
      }
    case ENABLE:
      {
      target = _MOTIF_WM_ENABLE_COMMAND;
      XmeNamedSink (DrawingArea, WM_S0, 
				   XmCOPY,
				   (XtPointer)msg, 
				   time);
       break;
      }
    case DISABLE:
      {
        target = _MOTIF_WM_DISABLE_COMMAND;
	XmeNamedSink (DrawingArea, WM_S0, 
				   
		                   XmCOPY,
				   (XtPointer)msg, 
				   time);
       break;

      }
    }
}




/*----------------------------------------------------------------------*
 |                                 SendCB                               |
 *----------------------------------------------------------------------*/
void
SendCB (Widget w, XtPointer clientData, XtPointer callData)
{
  Time    time;
  CARD32  parentID=0L, commandID=0L;
  char   *cmdName, *cmdDefault, *buf;
  MessageData msg, fulldata;
  int     size=0, i;
  

  time = ((XButtonEvent *) ((XmPushButtonCallbackStruct *)callData)->event)->time;


  buf = XmTextFieldGetString(field[PARENT]);
  sscanf(buf, "%d", &parentID);
  XtFree(buf);

  buf = XmTextFieldGetString(field[ID]);
  sscanf(buf, "%d", &commandID);
  XtFree(buf);

  cmdName    = XmTextFieldGetString(field[NAME]);
  cmdDefault = XmTextFieldGetString(field[DEFAULT]);


  if (XmToggleButtonGetState( toggle[DEFINE]) )
    {
      size += sizeof(CARD32);  /* command id */
      size += sizeof(CARD32);  /* notify window id */
      size += sizeof(CARD32);  /* command_set id */
      size += sizeof(CARD16) + strlen(cmdName) + 1;
      size += sizeof(CARD16) + strlen(cmdDefault) + 1;
	
      msg = fulldata = (MessageData) XtMalloc(sizeof(CARD8) * size);
	
      msg = PackCARD32(msg, commandID);
      msg = PackCARD32(msg, MY_PRETTY_ATOM);
      msg = PackCARD32(msg, parentID);
      msg = PackString(msg, cmdName);
      msg = PackString(msg, cmdDefault);

      SendMessage(DEFINE, fulldata, size, time);
    }
  else if (XmToggleButtonGetState( toggle[INCLUDE]) )
    {
      CARD32  in_line = 0L;
      Atom    selection = MY_PRETTY_ATOM;
      CARD32  count = 1L;
      CARD32  window;
      CARD32  iconBit = 1<<31;
      CARD32  windowBit = 1<<30;
      char   *buf, str[128];


      /*
       *  Fixup the window id receiving the client command.
       *  Top 2 bits:
       *  	10 - Icon only
       *	01 - Window only
       *	11 - Both Icon and Window
       *	00 - Reserved (Don't Use!)
       *
       *  Bottom 30 bits:
       *	0x3fffffff - All windows and/or icons
       *	0x00000000 - Root window menu
       */

      if (XmToggleButtonGetState(toggle[ROOT]))
	window = 0L;
      else if (XmToggleButtonGetState(toggle[ALL]))
	window = 0x3fffffff;
      else
	{
	  buf = XmTextFieldGetString(field[WIN_ID]);
	  if (buf[1] == 'x')
	    sscanf(buf, "%x", &window);
	  else
	    sscanf(buf, "%d", &window);
	  XtFree(buf);
	}

      if (XmToggleButtonGetState(toggle[ICON]))
	window |= iconBit;
      if (XmToggleButtonGetState(toggle[WIN]))
	window |= windowBit;
      if (XmToggleButtonGetState(toggle[INLINE]))
      {
	  in_line = 1L;
      }

      size += 5 * sizeof(CARD32); 
      msg = fulldata = (MessageData) XtMalloc(sizeof(CARD8) * size);

      msg = PackCARD32(msg, in_line);
      msg = PackCARD32(msg, commandID);
      msg = PackCARD32(msg, selection);
      msg = PackCARD32(msg, count);
      msg = PackCARD32(msg, window);

      sprintf(str, "IncludeCommand #%d on window %x (%u).\n", commandID, window, window);
      XmTextInsert(text, 0, str);

      SendMessage(INCLUDE, fulldata, size, time);
    }
  else if (XmToggleButtonGetState( toggle[REMOVE]) )
    {
      CARD32  count = 1L;
      CARD32  window;
      CARD32  iconBit = 1<<31;
      CARD32  windowBit = 1<<30;
      char   *buf, str[128];

      if (XmToggleButtonGetState(toggle[ROOT]))
	window = 0L;
      else if (XmToggleButtonGetState(toggle[ALL]))
	window = 0x3fffffff;
      else
	{
	  buf = XmTextFieldGetString(field[WIN_ID]);
	  if (buf[1] == 'x')
	    sscanf(buf, "%x", &window);
	  else
	    sscanf(buf, "%d", &window);
	  XtFree(buf);
	}

      if (XmToggleButtonGetState(toggle[ICON]))
	window |= iconBit;
      if (XmToggleButtonGetState(toggle[WIN]))
	window |= windowBit;

      size += 5 * sizeof(CARD32); 
      msg = fulldata = (MessageData) XtMalloc(sizeof(CARD8) * size);

      msg = PackCARD32(msg, commandID);
      msg = PackCARD32(msg, count);
      msg = PackCARD32(msg, window);

      sprintf(str, "RemoveCommand #%d on window %x (%u).\n",
	      commandID, window, window);
      XmTextInsert(text, 0, str);

      SendMessage(REMOVE, fulldata, size, time);
    }
  else if (XmToggleButtonGetState( toggle[ENABLE]) )
    {
      CARD32  count = 1L;
      CARD32  window;
      CARD32  iconBit = 1<<31;
      CARD32  windowBit = 1<<30;
      char   *buf, str[128];

      if (XmToggleButtonGetState(toggle[ROOT]))
	window = 0L;
      else if (XmToggleButtonGetState(toggle[ALL]))
	window = 0x3fffffff;
      else
	{
	  buf = XmTextFieldGetString(field[WIN_ID]);
	  if (buf[1] == 'x')
	    sscanf(buf, "%x", &window);
	  else
	    sscanf(buf, "%d", &window);
	  XtFree(buf);
	}

      if (XmToggleButtonGetState(toggle[ICON]))
	window |= iconBit;
      if (XmToggleButtonGetState(toggle[WIN]))
	window |= windowBit;

      size += 5 * sizeof(CARD32); 
      msg = fulldata = (MessageData) XtMalloc(sizeof(CARD8) * size);

      msg = PackCARD32(msg, commandID);
      msg = PackCARD32(msg, count);
      msg = PackCARD32(msg, window);

      sprintf(str, "EnableCommand #%d on window %x (%u).\n",
	      commandID, window, window);
      XmTextInsert(text, 0, str);

      SendMessage(ENABLE, fulldata, size, time);
    }
  else if (XmToggleButtonGetState( toggle[DISABLE]) )
    {
      CARD32  count = 1L;
      CARD32  window;
      CARD32  iconBit = 1<<31;
      CARD32  windowBit = 1<<30;
      char   *buf, str[128];

      if (XmToggleButtonGetState(toggle[ROOT]))
	window = 0L;
      else if (XmToggleButtonGetState(toggle[ALL]))
	window = 0x3fffffff;
      else
	{
	  buf = XmTextFieldGetString(field[WIN_ID]);
	  if (buf[1] == 'x')
	    sscanf(buf, "%x", &window);
	  else
	    sscanf(buf, "%d", &window);
	  XtFree(buf);
	}

      if (XmToggleButtonGetState(toggle[ICON]))
	window |= iconBit;
      if (XmToggleButtonGetState(toggle[WIN]))
	window |= windowBit;

      size += 5 * sizeof(CARD32); 
      msg = fulldata = (MessageData) XtMalloc(sizeof(CARD8) * size);

      msg = PackCARD32(msg, commandID);
      msg = PackCARD32(msg, count);
      msg = PackCARD32(msg, window);

      sprintf(str, "DisableCommand #%d on window %x (%u).\n",
	      commandID, window, window);
      XmTextInsert(text, 0, str);

      SendMessage(DISABLE, fulldata, size, time);
    }
}



/*----------------------------------------------------------------------*
 |                              PredefinedCB                            |
 *----------------------------------------------------------------------*/
void
predefined_command (Widget w, XtPointer which_predefine, XtPointer callData)
{
  printf("Predefined command called with %d\n", (int)which_predefine);

  switch ((int)which_predefine)
  {
    case 1:
      XmToggleButtonSetState(toggle[DEFINE], True, True);
      XmTextFieldSetString(field[PARENT], "0");
      XmTextFieldSetString(field[ID], "1");
      XmTextFieldSetString(field[NAME], "OCCUPY");
      XmTextFieldSetString(field[DEFAULT], "Occupy");
      SendCB(w, NULL, callData);
      
      XmTextFieldSetString(field[PARENT], "1");
      XmTextFieldSetString(field[ID], "2");
      XmTextFieldSetString(field[NAME], "ONE");
      XmTextFieldSetString(field[DEFAULT], "One");
      SendCB(w, NULL, callData);
      
      XmTextFieldSetString(field[PARENT], "1");
      XmTextFieldSetString(field[ID], "3");
      XmTextFieldSetString(field[NAME], "TWO");
      XmTextFieldSetString(field[DEFAULT], "Two");
      SendCB(w, NULL, callData);
      
      XmTextFieldSetString(field[PARENT], "1");
      XmTextFieldSetString(field[ID], "4");
      XmTextFieldSetString(field[NAME], "THREE");
      XmTextFieldSetString(field[DEFAULT], "Three");
      SendCB(w, NULL, callData);
      
      XmToggleButtonSetState(toggle[INCLUDE], True, True);
      XmTextFieldSetString(field[PARENT], "0");
      XmTextFieldSetString(field[ID], "1");
      XmTextFieldSetString(field[NAME], "OCCUPY");
      XmTextFieldSetString(field[DEFAULT], "Occupy");
      XmToggleButtonSetState(toggle[INLINE], False, True);
      SendCB(w, NULL, callData);
      break;

    case 2:
      XmToggleButtonSetState(toggle[DEFINE], True, True);
      XmTextFieldSetString(field[PARENT], "0");
      XmTextFieldSetString(field[ID], "1");
      XmTextFieldSetString(field[NAME], "OCCUPY");
      XmTextFieldSetString(field[DEFAULT], "Occupy");
      SendCB(w, NULL, callData);
      
      XmTextFieldSetString(field[PARENT], "1");
      XmTextFieldSetString(field[ID], "2");
      XmTextFieldSetString(field[NAME], "ONE");
      XmTextFieldSetString(field[DEFAULT], "One");
      SendCB(w, NULL, callData);
      
      XmTextFieldSetString(field[PARENT], "1");
      XmTextFieldSetString(field[ID], "3");
      XmTextFieldSetString(field[NAME], "TWO");
      XmTextFieldSetString(field[DEFAULT], "Two");
      SendCB(w, NULL, callData);
      
      XmTextFieldSetString(field[PARENT], "1");
      XmTextFieldSetString(field[ID], "4");
      XmTextFieldSetString(field[NAME], "THREE");
      XmTextFieldSetString(field[DEFAULT], "Three");
      SendCB(w, NULL, callData);
      
      XmToggleButtonSetState(toggle[INCLUDE], True, True);
      XmTextFieldSetString(field[PARENT], "0");
      XmTextFieldSetString(field[ID], "1");
      XmTextFieldSetString(field[NAME], "OCCUPY");
      XmTextFieldSetString(field[DEFAULT], "Occupy");
      XmToggleButtonSetState(toggle[INLINE], True, True);
      SendCB(w, NULL, callData);
      break;

    case 3:
      break;

    case 4:
      break;
  }
}



/*----------------------------------------------------------------------*
 |                          CreateBulletinBoard                         |
 *----------------------------------------------------------------------*/
void
CreateBulletinBoard (Widget parent)
{
  Arg    args[32];
  int    n;
  Widget bulletinBoard;
  Widget window_frame, arg_frame, cmd_frame, predef_frame;
  Widget windowRC, argWin, msgRC, predefRC;
  Widget label, label1, label2, label3;
  Widget scrolledWindow, send, predef;


  XmRegisterConverters();
  
  n = 0;
  bulletinBoard = XtCreateManagedWidget("form",
				 xmFormWidgetClass,
				 parent,
				 args,
				 n);
  
  n = 0;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); ++n;
  XtSetArg(args[n], XmNtopOffset, 10); ++n;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); ++n;
  XtSetArg(args[n], XmNleftOffset, 10); ++n;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); ++n;
  XtSetArg(args[n], XmNrightPosition, 50); ++n;
  window_frame = XtCreateManagedWidget("window_frame",
			  xmFrameWidgetClass,
			  bulletinBoard,
			  args,
			  n);
  
  n = 0;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); ++n;
  XtSetArg(args[n], XmNtopOffset, 10); ++n;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); ++n;
  XtSetArg(args[n], XmNleftPosition, 50); ++n;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); ++n;
  XtSetArg(args[n], XmNrightOffset, 10); ++n;
  XtSetArg(args[n], XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET); ++n;
  XtSetArg(args[n], XmNbottomWidget, window_frame); ++n;
  arg_frame = XtCreateManagedWidget("arg_frame",
			  xmFrameWidgetClass,
			  bulletinBoard,
			  args,
			  n);

  n = 0;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); ++n;
  XtSetArg(args[n], XmNtopWidget, window_frame); ++n;
  XtSetArg(args[n], XmNtopOffset, 10); ++n;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); ++n;
  XtSetArg(args[n], XmNleftOffset, 10); ++n;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); ++n;
  XtSetArg(args[n], XmNrightOffset, 10); ++n;
  cmd_frame = XtCreateManagedWidget("cmd_frame",
			 xmFrameWidgetClass,
			 bulletinBoard,
			 args,
			 n);
  
  n = 0;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); ++n;
  XtSetArg(args[n], XmNtopWidget, cmd_frame); ++n;
  XtSetArg(args[n], XmNtopOffset, 10); ++n;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); ++n;
  XtSetArg(args[n], XmNleftOffset, 10); ++n;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); ++n;
  XtSetArg(args[n], XmNrightOffset, 10); ++n;
  send = XtCreateManagedWidget("send",
			xmPushButtonWidgetClass,
			bulletinBoard,
			args,
			n);
  XtAddCallback(send, XmNactivateCallback, SendCB, NULL);
    
  n = 0;
  XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); ++n;
  XtSetArg(args[n], XmNbottomOffset, 10); ++n;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); ++n;
  XtSetArg(args[n], XmNleftOffset, 10); ++n;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); ++n;
  XtSetArg(args[n], XmNrightOffset, 10); ++n;
  predef_frame = XtCreateManagedWidget("predef_frame",
			 xmFrameWidgetClass,
			 bulletinBoard,
			 args,
			 n);
  n = 0;
  XtSetArg(args[n], XmNbottomAttachment, XmATTACH_WIDGET); ++n;
  XtSetArg(args[n], XmNbottomWidget, predef_frame); ++n;
  XtSetArg(args[n], XmNbottomOffset, 10); ++n;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); ++n;
  XtSetArg(args[n], XmNtopWidget, send); ++n;
  XtSetArg(args[n], XmNtopOffset, 10); ++n;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); ++n;
  XtSetArg(args[n], XmNleftOffset, 10); ++n;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); ++n;
  XtSetArg(args[n], XmNrightOffset, 10); ++n;
  scrolledWindow = XtCreateManagedWidget("scrolledWindow",
				  xmScrolledWindowWidgetClass,
				  bulletinBoard,
				  args,
				  n);

  n = 0;
  XtSetArg(args[n], XmNeditMode, XmMULTI_LINE_EDIT); n++;
  text = XtCreateManagedWidget("text",
			       xmTextWidgetClass,
			       scrolledWindow,
			       args,
			       n);

  /**********************The Window Box **********************************/


  n=0;
  XtSetArg (args[n], XmNadjustLast, True); n++;
  XtSetArg (args[n], XmNpacking, XmPACK_TIGHT); n++;
  windowRC     = XtCreateManagedWidget("windowRC",
					 xmRowColumnWidgetClass,
					 window_frame,
					 args, n);
  
  

  
  
  toggle[ROOT] = XtCreateManagedWidget("Root (Ignores Other Toggles)",
					 xmToggleButtonWidgetClass,
					 windowRC,
					 NULL,0);
  

  toggle[ALL]  = XtCreateManagedWidget("All Windows &/^ Icons",
					 xmToggleButtonWidgetClass,
					 windowRC,
					 NULL,0);





  
  XtVaCreateManagedWidget("separator", xmSeparatorWidgetClass, windowRC,
			  XmNorientation, XmHORIZONTAL, NULL);


  toggle[ICON] = XtCreateManagedWidget("Icon",
					 xmToggleButtonWidgetClass,
					 windowRC,
					 NULL,0); 


  toggle[WIN]  = XtCreateManagedWidget("Window",
					 xmToggleButtonWidgetClass,
					 windowRC,
					 NULL,0);

  field[WIN_ID]= XtVaCreateManagedWidget("textField",
					 xmTextFieldWidgetClass,
					 windowRC,
					 NULL);

  /*************************************************************************/

  n = 0;
  argWin = XtCreateManagedWidget("argWinRC",
			     xmFormWidgetClass,
			     arg_frame,
			     args,
			     n);

  n = 0;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNtopOffset, 2); n++;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNleftPosition, 50); n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNrightOffset, 2); n++;
  XtSetArg(args[n], XmNshadowThickness, 1); n++;
  field[PARENT] = XtCreateManagedWidget("textField1",
			      xmTextFieldWidgetClass,
			      argWin,
			      args,
			      n);
  

  n = 0;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_OPPOSITE_WIDGET); n++;
  XtSetArg(args[n], XmNtopWidget, field[PARENT]); n++;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNrightPosition, 50); n++;
  XtSetArg(args[n], XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET); n++;
  XtSetArg(args[n], XmNbottomWidget, field[PARENT]); n++;
  label = XtCreateManagedWidget("Parent ID",
				xmLabelWidgetClass,
				argWin,
				args,
				n);

  
  n = 0;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
  XtSetArg(args[n], XmNtopWidget, field[PARENT]); n++;
  XtSetArg(args[n], XmNtopOffset, 2); n++;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNleftPosition, 50); n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNrightOffset, 2); n++;
  XtSetArg(args[n], XmNshadowThickness, 1); n++;
  field[ID] = XtCreateManagedWidget("textField2",
				     xmTextFieldWidgetClass,
				     argWin,
				     args,
				     n);

  n = 0;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_OPPOSITE_WIDGET); n++;
  XtSetArg(args[n], XmNtopWidget, field[ID]); n++;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNrightPosition, 50); n++;
  XtSetArg(args[n], XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET); n++;
  XtSetArg(args[n], XmNbottomWidget, field[ID]); n++;
  label1 = XtCreateManagedWidget("Command ID",
				 xmLabelWidgetClass,
				 argWin,
				 args,
				 n);

  

  n = 0;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
  XtSetArg(args[n], XmNtopWidget, field[ID]); n++;
  XtSetArg(args[n], XmNtopOffset, 2); n++;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNleftPosition, 50); n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNrightOffset, 2); n++;
  XtSetArg(args[n], XmNshadowThickness, 1); n++;
  field[NAME] = XtCreateManagedWidget("textField3",
				     xmTextFieldWidgetClass,
				     argWin,
				     args,
				     n);
  


  n = 0;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_OPPOSITE_WIDGET); n++;
  XtSetArg(args[n], XmNtopWidget, field[NAME]); n++;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNrightPosition, 50); n++;
  XtSetArg(args[n], XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET); n++;
  XtSetArg(args[n], XmNbottomWidget, field[NAME]); n++;
  label2 = XtCreateManagedWidget("Command Name",
			  xmLabelWidgetClass,
			  argWin,
			  args,
			  n);

  

  n = 0;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
  XtSetArg(args[n], XmNtopWidget, field[NAME]); n++;
  XtSetArg(args[n], XmNtopOffset, 2); n++;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNleftPosition, 50); n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNrightOffset, 2); n++;
  XtSetArg(args[n], XmNshadowThickness, 1); n++;
  field[DEFAULT] = XtCreateManagedWidget("Default Name",
				     xmTextFieldWidgetClass,
				     argWin,
				     args,n);




  n = 0;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_OPPOSITE_WIDGET); n++;
  XtSetArg(args[n], XmNtopWidget, field[DEFAULT]); n++;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNrightPosition, 50); n++;
  XtSetArg(args[n], XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET); n++;
  XtSetArg(args[n], XmNbottomWidget, field[DEFAULT]); n++;
  label3 = XtCreateManagedWidget("Command Name",
				 xmLabelWidgetClass,
				 argWin,
				 args,
				 n);

  
  n = 0;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
  XtSetArg(args[n], XmNtopWidget, field[DEFAULT]); n++;
  XtSetArg(args[n], XmNtopOffset, 2); n++;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNleftOffset, 2); n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNrightOffset, 2); n++;
  XtSetArg(args[n], XmNshadowThickness, 0); n++;
  toggle[INLINE] = XtCreateManagedWidget("Inline Command Set",
					 xmToggleButtonWidgetClass,
					 argWin,
					 args,
					 n);

  
  /************************************************************************/

  n = 0;
  XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
  XtSetArg(args[n], XmNentryAlignment, XmALIGNMENT_CENTER); n++;
  XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
  XtSetArg(args[n], XmNradioBehavior, True); n++;
  XtSetArg(args[n], XmNradioAlwaysOne, True); n++;
  XtSetArg(args[n], XmNisHomogeneous, False); n++;
  XtSetArg(args[n], XmNrowColumnType, XmWORK_AREA); n++;
  msgRC = XtCreateManagedWidget("rowColumn2",
				     xmRowColumnWidgetClass,
				     cmd_frame,
				     args,
				     n);
  
  
  n = 0;
  XtSetArg(args[n], XmNshadowThickness, 2); n++;
  XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
  XtSetArg(args[n], XmNmarginLeft, 2); n++;
  XtSetArg(args[n], XmNmarginRight, 2); n++;
  XtSetArg(args[n], XmNmarginTop, 2); n++;
  XtSetArg(args[n], XmNmarginBottom, 2); n++;
  XtSetArg(args[n], XmNrecomputeSize, False); n++;
  XtSetArg(args[n], XmNindicatorSize, 0); n++;
  XtSetArg(args[n], XmNset, False); n++;
  XtSetArg(args[n], XmNindicatorOn, False); n++;
  XtSetArg(args[n], XmNfillOnSelect, True); n++;
  XtSetArg(args[n], XmNset, True); n++;
  toggle[DEFINE] = XtCreateManagedWidget("defineCommand",
					 xmToggleButtonWidgetClass,
					 msgRC,
					 args,
					 n);
  
  
  n = 0;
  XtSetArg(args[n], XmNshadowThickness, 2); n++;
  XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
  XtSetArg(args[n], XmNmarginLeft, 2); n++;
  XtSetArg(args[n], XmNmarginRight, 2); n++;
  XtSetArg(args[n], XmNindicatorSize, 0); n++;
  XtSetArg(args[n], XmNindicatorOn, False); n++;
  XtSetArg(args[n], XmNfillOnSelect, True); n++;
  toggle[INCLUDE] = XtCreateManagedWidget("includeCommand",
					  xmToggleButtonWidgetClass,
					  msgRC,
					  args,
					  n);
  
  n = 0;
  XtSetArg(args[n], XmNshadowThickness, 2); n++;
  XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
  XtSetArg(args[n], XmNmarginLeft, 0); n++;
  XtSetArg(args[n], XmNindicatorSize, 0); n++;
  XtSetArg(args[n], XmNindicatorOn, False); n++;
  XtSetArg(args[n], XmNfillOnSelect, True); n++;
  toggle[REMOVE] = XtCreateManagedWidget("removeCommand",
					 xmToggleButtonWidgetClass,
					 msgRC,
					 args,
					 n);
  
  n = 0;
  XtSetArg(args[n], XmNshadowThickness, 2); n++;
  XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
  XtSetArg(args[n], XmNmarginLeft, 0); n++;
  XtSetArg(args[n], XmNindicatorSize, 0); n++;
  XtSetArg(args[n], XmNindicatorOn, False); n++;
  XtSetArg(args[n], XmNfillOnSelect, True); n++;
  toggle[ENABLE] = XtCreateManagedWidget("enableCommand",
					  xmToggleButtonWidgetClass,
					  msgRC,
					  args,
					  n);

  n = 0;
  XtSetArg(args[n], XmNshadowThickness, 2); n++;
  XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
  XtSetArg(args[n], XmNmarginLeft, 0); n++;
  XtSetArg(args[n], XmNindicatorSize, 0); n++;
  XtSetArg(args[n], XmNindicatorOn, False); n++;
  XtSetArg(args[n], XmNfillOnSelect, True); n++;
  toggle[DISABLE] = XtCreateManagedWidget("disableCommand",
					  xmToggleButtonWidgetClass,
					  msgRC,
					  args,
					  n);

  /************************************************************************/

  n = 0;
  XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
  XtSetArg(args[n], XmNentryAlignment, XmALIGNMENT_CENTER); n++;
  XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
  XtSetArg(args[n], XmNradioBehavior, True); n++;
  XtSetArg(args[n], XmNradioAlwaysOne, True); n++;
  XtSetArg(args[n], XmNisHomogeneous, False); n++;
  XtSetArg(args[n], XmNrowColumnType, XmWORK_AREA); n++;
  predefRC = XtCreateManagedWidget("rowColumn2",
				   xmRowColumnWidgetClass,
				   predef_frame,
				   args,
				   n);
  
  
  n = 0;
  XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
  XtSetArg(args[n], XmNrecomputeSize, False); n++;
  predef = XtCreateManagedWidget("Predefine 1",
				 xmPushButtonWidgetClass,
				 predefRC,
				 args,
				 n);
  XtAddCallback(predef, XmNactivateCallback, predefined_command,
		(XtPointer) 1);
  
  
  n = 0;
  XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
  XtSetArg(args[n], XmNrecomputeSize, False); n++;
  predef = XtCreateManagedWidget("Predefine 2",
				 xmPushButtonWidgetClass,
				 predefRC,
				 args,
				 n);
  XtAddCallback(predef, XmNactivateCallback, predefined_command,
		(XtPointer) 2);
  
  
  n = 0;
  XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
  XtSetArg(args[n], XmNrecomputeSize, False); n++;
  predef = XtCreateManagedWidget("Predefine 3",
				 xmPushButtonWidgetClass,
				 predefRC,
				 args,
				 n);
  XtAddCallback(predef, XmNactivateCallback, predefined_command,
		(XtPointer) 3);
  
  
  n = 0;
  XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
  XtSetArg(args[n], XmNrecomputeSize, False); n++;
  predef = XtCreateManagedWidget("Predefine 4",
				 xmPushButtonWidgetClass,
				 predefRC,
				 args,
				 n);
  XtAddCallback(predef, XmNactivateCallback, predefined_command,
		(XtPointer) 4);
}



/*----------------------------------------------------------------------*
 |                              InternStuff                             |
 *----------------------------------------------------------------------*/
void
InternStuff (Display *dsp)
{
  WM_S0                      = XInternAtom(dsp, "WM_S0", False);
  MY_PRETTY_ATOM	    = XInternAtom(dsp, "MY_PRETTY_ATOM", False);
  _MOTIF_WM_DEFINE_COMMAND  = XInternAtom(dsp, "_MOTIF_WM_DEFINE_COMMAND", False);
  _MOTIF_WM_INCLUDE_COMMAND = XInternAtom(dsp, "_MOTIF_WM_INCLUDE_COMMAND", False);
  _MOTIF_WM_DISABLE_COMMAND = XInternAtom(dsp, "_MOTIF_WM_DISABLE_COMMAND", False);
  _MOTIF_WM_ENABLE_COMMAND  = XInternAtom(dsp, "_MOTIF_WM_ENABLE_COMMAND", False);
  _MOTIF_WM_INVOKE_COMMAND  = XInternAtom(dsp, "_MOTIF_WM_INVOKE_COMMAND", False);
  _MOTIF_WM_REMOVE_COMMAND  = XInternAtom(dsp, "_MOTIF_WM_REMOVE_COMMAND", False);
}


/*----------------------------------------------------------------*
 |                      PACKING FUNCTIONS                         |
 *----------------------------------------------------------------*/


static XtPointer
PackString(XtPointer data, String str)
{
    register int i, len = strlen(str);

    data = PackCARD16(data, len);
    for (i = 0; i < len; i++, str++) 
	data = PackCARD8(data, *str);

    return(data);
}


static XtPointer
PackCARD32 (XtPointer data, CARD32 val)
{
  CARD16 bottom = val & (0xFFFF);
  CARD16 top = val >> 16;

  data = PackCARD16(data, top);
  data = PackCARD16(data, bottom);
  return(data);
}


static XtPointer
PackCARD16 (XtPointer data, CARD16 val)
{
  CARD8 bottom = val & (0xFF);
  CARD8 top = val >> 8;

  data = PackCARD8(data, top);
  data = PackCARD8(data, bottom);
  return(data);
}



static XtPointer
PackCARD8 (XtPointer data, CARD8 val)
{
  CARD8 *ptr = (CARD8 *) data;

  *ptr = (CARD8) val;
  data = ((char*)data) + 1;
  return(data);
}




/*----------------------------------------------------------------------*
 |                               m a i n                                |
 *----------------------------------------------------------------------*/
void
main (int argc, char **argv)
{
  
  CommonTestInit (argc,argv);
  InternStuff(display);
  
  shell_ui = XtVaAppCreateShell("cmd", "Cmd", applicationShellWidgetClass, 
				display, NULL);
  CreateBulletinBoard(shell_ui);

  XtRealizeWidget(shell_ui);

  DrawingArea = XmCreateDrawingArea (Shell1, "DrawingArea", NULL,0);
  XtManageChild (DrawingArea);
  XtAddCallback (DrawingArea, XmNdestinationCallback, DestinationCB, (XtPointer)NULL);
  XtRealizeWidget(Shell1);

  CommonPause();
  CommonPause();
  CommonPause();

  XtAppMainLoop(app_context);
}


