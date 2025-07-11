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
static char rcsid[] = "$XConsortium: editor.c /main/8 1995/07/14 12:04:07 drk $"
#endif
#endif
/**---------------------------------------------------------------------
***	
***	file:		editor.c
***	description:	This program demonstrates the Motif text, 
*** main_parent window, and dialog widgets, as well as the cut and paste
***	functions.
***	
***	defaults:	editor.c depends on these defaults:
***
#
*allowShellResize:		true
*borderWidth:			0
*highlightThickness:		2
*traversalOn:			true
***-------------------------------------------------------------------*/


/*-------------------------------------------------------------
**	Include Files
*/

#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <X11/Xatom.h>

#include <testlib.h>



/*-------------------------------------------------------------
**	Global Variables
*/

#define MENU_HELP		200
#define MENU_EXIT		201
#define MENU_OPEN		202
#define MENU_NEW		203
#define MENU_CLOSE		204
#define MENU_SAVE		205
#define MENU_SAVE_AS		206
#define MENU_PRINT		207
#define MENU_CUT		208
#define MENU_COPY		209
#define MENU_PASTE		210
#define MENU_CLEAR		211
#define MENU_FILE		212
#define MENU_EDIT		213

#define DIALOG_FSELECT		300
#define DIALOG_CWARNING		301
#define DIALOG_XWARNING		302
#define DIALOG_NEW		303
#define DIALOG_SAVE		304
#define DIALOG_HELP		305
#define DIALOG_PRINT		306

#define	DEFAULT_WIDTH	500
#define	DEFAULT_HEIGHT	400

#define	MAX_NAME_LEN	50

static void CreateText ();

/* defines a temporary file for file transfers */

Widget text;			/* multi-line text widget		    */
Widget cut_button;		/* clipboard cut button 		    */
Widget copy_button;		/* clipboard copy button 		    */
Widget paste_button;		/* clipboard paste button 		    */
Widget clear_button;		/* clipboard clear button 		    */
Widget open_dialog;		/* file selection dialog 		    */
Widget new_dialog;		/* file name prompt dialog 		    */
Widget close_warning;		/* special internal selection dialog	    */
Widget exit_warning;		/* special internal selection dialog	    */
Widget general_warning;		/* warning dialog	 		    */
Widget save_dialog;		/* save as prompt dialog	 	    */
Widget print_warning;		/* warning dialog		 	    */
Boolean file_saved = True;	/* indicates that the present file is saved */
char *filename = NULL;		/* string containing file name 		    */
int start_pos, end_pos;		/* start and end position of last action    */

XmStringCharSet charset = (XmStringCharSet) XmSTRING_DEFAULT_CHARSET;
				/* used to set up XmStrings */

char Error[128];

Widget		main_parent;/*  MainWindow	 	*/
Widget		menu_bar;	/*  RowColumn	 		*/
Bool		text_first; /*  "Open"/"New" selected for the first time */
Bool		text_startup;
Bool		edit_startup;
Bool		file_startup;

/* bits for exclamation point in dialog */
char warningBits[] = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x07, 0x00, 0x00, 0xe0, 0x07, 0x00,
   0x00, 0xe0, 0x07, 0x00, 0x00, 0xe0, 0x07, 0x00, 0x00, 0xe0, 0x07, 0x00,
   0x00, 0xe0, 0x07, 0x00, 0x00, 0xe0, 0x07, 0x00, 0x00, 0xe0, 0x07, 0x00,
   0x00, 0xe0, 0x07, 0x00, 0x00, 0xe0, 0x07, 0x00, 0x00, 0xe0, 0x07, 0x00,
   0x00, 0xe0, 0x07, 0x00, 0x00, 0xe0, 0x07, 0x00, 0x00, 0xe0, 0x07, 0x00,
   0x00, 0xe0, 0x07, 0x00, 0x00, 0xe0, 0x07, 0x00, 0x00, 0xe0, 0x07, 0x00,
   0x00, 0xe0, 0x07, 0x00, 0x00, 0xe0, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x03, 0x00,
   0x00, 0xe0, 0x07, 0x00, 0x00, 0xf0, 0x0f, 0x00, 0x00, 0xf0, 0x0f, 0x00,
   0x00, 0xf0, 0x0f, 0x00, 0x00, 0xf0, 0x0f, 0x00, 0x00, 0xe0, 0x07, 0x00,
   0x00, 0xc0, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00};


/************************************************************************
 *
 *  CreateDefaultImage - create a default images for warning symbol.
 *
 **********************************<->***********************************/
static XImage * CreateDefaultImage (bits, width, height)
char *bits;
int   width, height;
{
    XImage *image;

    image = (XImage *) XtMalloc (sizeof (XImage));
    image->width = width;
    image->height = height;
    image->data = bits;
    image->depth = 1;
    image->xoffset = 0;
    image->format = XYBitmap;
    image->byte_order = LSBFirst;
    image->bitmap_unit = 8;
    image->bitmap_bit_order = LSBFirst;
    image->bitmap_pad = 8;
    image->bytes_per_line = (width+7)/8;
    return (image);
}


/*-------------------------------------------------------------
**	OpenFile
**		Open the present file.  Returns true if file 
**  exists and open is sucessful.
*/
Boolean OpenFile()
{
   struct stat statbuf;		/* Information on a file. */
   int file_length;		/* Length of file. 	  */
   char * file_string;		/* Contents of file. 	  */
   FILE *fp = NULL;		/* Pointer to open file   */
   
   if ((fp = fopen(filename, "r+")) == NULL)
	if ((fp = fopen(filename, "r")) != NULL) {
	    fprintf(stderr, "Warning: file opened read only.\n");
	} else {
	    return(False);
	}

   if (stat(filename, &statbuf) == 0)
	 file_length = statbuf.st_size;
   else
	 file_length = 1000000; /* arbitrary file length */

   /* read the file string */
   file_string =  XtMalloc((unsigned)file_length);
   fread(file_string, sizeof(char), file_length, fp);

   /* close up the file */
   if (fclose(fp) != NULL) fprintf(stderr, "Warning: unable to close file.\n");

   /* added the file string to the text widget */
   XmTextSetString(text, file_string);

   file_saved = True; /* intialize to True */
	 
   /* make appropriate item sensitive */
   XtSetSensitive(text, True);
   XtSetSensitive(cut_button, True);
   XtSetSensitive(copy_button, True);
   XtSetSensitive(clear_button, True);

   return(True);
}


/*-------------------------------------------------------------
**	SaveFile
**		Save the present file.
*/
Boolean SaveFile()
{
    char   * file_string = NULL;    /* Contents of file.		   */
    FILE   *tfp;	            /* Pointer to open temporary file.     */
    char   namebuf[BUFSIZ];         /* for "system" call below             */
    int	   status;
    char   *tempname = (char *)XtMalloc(25); /* Temporary file name. 	   */

    strcpy(tempname, "/tmp/xmeditXXXXXX");
    
    if ((tfp = fopen(mktemp(tempname), "w")) == NULL) {
       fprintf(stderr, "Warning: unable to open temp file, text not saved.\n");
       return(False);;
    }

    /* get the text string */
    file_string = XmTextGetString(text);

    /* write to a temp file */
    fwrite(file_string, sizeof(char), strlen(file_string) + 1, tfp);

    /* flush and close the temp file */
    if (fflush(tfp) != NULL) fprintf(stderr,"Warning: unable to flush file.\n");
    if (fclose(tfp) != NULL) fprintf(stderr,"Warning: unable to close file.\n");

    if (file_string != NULL) {
        XtFree(file_string); /* free the text string */
    }

    /* move the tempname to the saved file, but do it independent
	   of filesystem boundaries */
	sprintf (namebuf, "cp %s %s\0", tempname, filename);
	status = system(namebuf);
	unlink (tempname);
	if (status == 0) {
        file_saved = True;
    } else {
        fprintf(stderr, "Warning: unable to save file.\n");
        XtFree(tempname);
        return(False);
    }
           
    XtFree(tempname);
    return(True);
}


/*-------------------------------------------------------------
**      CloseFile
**              Close the present file.
*/
void CloseFile()
{
    /* zero out the text string in the text widget.
       caution: is causes a value changed callack. */
	/* Do this only if text widget is created. */
	if (text_first != True)
    	XmTextSetString(text, "");

    file_saved = True; /* reinitialize file_saved flag */

    /* free the file name */
    if (filename != NULL) {
	XtFree(filename);
        filename = NULL;
    }

    /* set text to insensitive */
	/* Do this only if text widget is created. */
	if (text_first != True)
    	XtSetSensitive(text, False);
}


/*-------------------------------------------------------------
**	CopyFileToClipboard
**		Copy the present file to the clipboard.
*/
void CopyFileToClipboard(time)
Time time;
{
   char *selected_string = XmTextGetSelection (text); /* text selection    */
   long item_id = 0;			      /* clipboard item id */
   long data_id = 0;			      /* clipboard data id */
   long status = 0;			      /* clipboard status  */
   XmString clip_label;

   /* using the clipboard facilities, copy the selected text to the clipboard */
   if (selected_string != NULL) {
	clip_label = XmStringCreateLtoR ("NEW_EDITOR", charset);
	/* start copy to clipboard, and continue till
	   a sucessful start copy is made */
	status = 0;
	while (status != ClipboardSuccess)
	    status = XmClipboardStartCopy (XtDisplay(text), XtWindow(text),
				       	clip_label, time,
				       	text, NULL, &item_id);

   	/* move the data to the clipboard, and
	   continue till a sucessful copy is made */
	status = 0;
	while (status != ClipboardSuccess)
	    status = XmClipboardCopy (XtDisplay(text), XtWindow(text),
				      item_id, "STRING", selected_string,
                       	     	      (long)strlen(selected_string)+1, 0,
				      &data_id);

	/* end the copy to the clipboard and continue till
	   a sucessful end copy is made */
	status = 0;
	while (status != ClipboardSuccess)
	    status = XmClipboardEndCopy (XtDisplay(text), XtWindow(text),
					 item_id);

	/* allow pasting when an item is sucessfully copied to the clipboard */
	XtSetSensitive(paste_button, True);

   }

}


/*-------------------------------------------------------------
**	DeletePrimarySelection
**		Deletes the primary selection.
*/
void DeletePrimarySelection()
{
   XClientMessageEvent cm;
   XEvent *event;

   /* send a client message to the text widget
      to delete the current selection */
   cm.type = ClientMessage;
   cm.display = XtDisplay(text);
   cm.message_type = XmInternAtom(XtDisplay(text), "KILL_SELECTION", FALSE);
   cm.window = XtWindow(text);
   cm.format = 32;
   cm.data.l[0] = XA_PRIMARY;

   event = (XEvent *) &cm;
   XSendEvent(XtDisplay(text), cm.window, TRUE, NoEventMask, event);
}

/*-------------------------------------------------------------
**	PasteItemFromClipboard
**		paste item from the clipboard to the current cursor location
*/
void PasteItemFromClipboard()
{
   /* retrieve the current data from the clipboard
      and paste it at the current cursor position */
   char * selected_string = XmTextGetSelection (text);
					      /* containts of selection  */
   int status = 0;			      /* clipboard status	 */
   char *buffer;			      /* temporary text buffer 	 */
   unsigned long length;		      /* length of buffer     	 */
   unsigned long outlength = 0;		      /* length of bytes copied	 */
   long private_id = 0;	      		      /* id of item on clipboard */
   XmTextPosition cursorPos;		      /* text cursor position 	 */
   register int n;			      /* arg count  	      	 */
   Arg args[10];			      /* arg list	      	 */

   /* find the length of the paste item, continue till the length is found */
   while (status != ClipboardSuccess) {
     status = XmClipboardInquireLength(XtDisplay(text), XtWindow(text),
				      "STRING", &length);
     if (status == ClipboardNoData) {
	length = 0;
	break;
     }
   }
			
   if (length == 0) {
	fprintf(stderr, "Warning: paste failed, no items to paste.\n");	
	return;
   }

   /* malloc to necessary space */
   buffer = XtMalloc(length);

   status = XmClipboardRetrieve (XtDisplay(text), XtWindow(text), "STRING",
				 buffer, length, &outlength, &private_id);
			
   /* Dialogs need to be added to indicate errors in pasting */
   if (status != ClipboardSuccess) {
	fprintf(stderr, "Warning: paste failed, status = %d\n", status);	
	return;
   }

   /* get cursor position for pasting */
   n = 0;
   XtSetArg(args[n], XmNcursorPosition, &cursorPos); n++;
   XtGetValues(text, args, n);
			
   /* add new text */
   XmTextReplace(text, cursorPos, cursorPos, buffer);
}

/*-------------------------------------------------------------
**	FileChangedCB
**		Process callback from Text.
*/
void FileChangedCB (w, client_data, call_data) 
Widget		w;		/*  widget id		*/
XtPointer		client_data;	/*  data from application   */
XtPointer		call_data;	/*  data from widget class  */
{
    /* set the file_saved flag to indicate that the
       file has been modified and the user should be
       notified before exiting. */

    file_saved = False;
}

/*-------------------------------------------------------------
**	MenuCB
**		Process callback from PushButtons in PulldownMenus.
*/
void MenuCB (w, client_data, call_data) 
Widget		w;		/*  widget id		*/
XtPointer		client_data;	/*  data from application   */
XtPointer		call_data;	/*  data from widget class  */
{
	register int n;		/* arg count		    */
	Arg args[10];			/* arg list		    */
	char *command;			/* command used in printing */

	switch ((int)client_data)
	{
		case MENU_OPEN:
		case MENU_NEW:

			/*	
			 * Create Text if not already created.
			 */
			if (text_first == True) {
				CreateText (main_parent);
				XtManageChild (text);

				XmAddTabGroup(text);

				XtSetSensitive(text, False);
				text_first = False;
			}
			/* if "Open", display the file selection dialog */
			if ((int)client_data == MENU_OPEN)
				XtManageChild (open_dialog);
			/* else, display the prompt dialog */
			else
				XtManageChild (new_dialog);
			break;

		case MENU_CLOSE:
	 		/* the present file has not been saved since
			   the last modification */
			if (!file_saved) /* display the 'save' message dialog */
			   XtManageChild (close_warning);
			else
			   CloseFile();
			break;

		case MENU_SAVE:
			/* open a temp file for writing */
			SaveFile();
			break;

		case MENU_SAVE_AS:
			/* Display the 'save as' dialog with the
			   present filename displayed in it. */
			n = 0;
			XtSetArg(args[n], XmNtextString,
			     XmStringCreateLtoR (filename, charset));  n++;
			XtSetValues(save_dialog, args, n);
			XtManageChild (save_dialog);
			break;

		case MENU_PRINT:
			if (!file_saved)
			   XtManageChild(print_warning);
			else if (filename != NULL) {
				/* malloc space for the command name. 
			   	   Note: command = size of the filename +
			   	   "lp " + null terminator */
				command = XtMalloc(strlen(filename) + 4);
				sprintf(command, "lp %s\0", filename);
				if (system(command) != NULL)
					fprintf(stderr, "print failed");
				XtFree(command);
			}
			break;

		case MENU_EXIT:
			/* exit if there is no files open */
			if (!file_saved) /* display the 'save' message dialog */
			   XtManageChild (exit_warning);
			else {
			   /* close up file pointers and descriptors */
			   CloseFile();

			   /* exit this program */
			   exit (0);
			}
			break;

		case MENU_CUT:
			{
			   /* needed to get the event time */
                           XmAnyCallbackStruct * cb =
					    (XmAnyCallbackStruct *) call_data;

			   /* call routine to copy selection to clipboard */
			   CopyFileToClipboard(cb->event->xbutton.time);

			   /* call routine to delete primary selection */
			   DeletePrimarySelection();
    			}
			break;

		case MENU_COPY:
    			{
			   /* needed to get the event time */
                           XmAnyCallbackStruct * cb =
					    (XmAnyCallbackStruct *) call_data;

			   /* call routine to copy selection to clipboard */
			   CopyFileToClipboard(cb->event->xbutton.time);

    			}
			break;

		case MENU_PASTE:
			/* call the routine that pastes the
			   text at the cursor position */
			PasteItemFromClipboard();
			break;

		case MENU_CLEAR:
			/* call routine to delete primary selection */
			DeletePrimarySelection();
			break;

		case MENU_HELP:
			/* no help at this time */
			break;

		default:
			/* unknown client_data was recieved and
			   there is no setup to handle this */
			fprintf(stderr, "Warning: in menu callback\n");
			break;
	}

}


/*-------------------------------------------------------------
**	DialogApplyCB
**		Process callback from Dialog apply actions.
*/
static void DialogApplyCB (w, client_data, call_data) 
Widget		w;		/*  widget id		*/
XtPointer		client_data;	/*  data from application   */
XtPointer		call_data;	/*  data from widget class  */
{
	char *command;			/* command used in printing */

	switch ((int)client_data)
	{
		case DIALOG_PRINT:
			if (filename != NULL) {
			       /* malloc space for the command name. 
			          Note: command = size of the filename +
			          "lp " + null terminator */
				command = XtMalloc(strlen(filename) + 4);
				sprintf(command, "lp %s\0", filename);
				if (system(command) != NULL)
					fprintf(stderr, "print failed");
				XtFree(command);
			}
		case DIALOG_CWARNING:
			CloseFile();
			file_saved = True; /* reset the default */
			break;

		case DIALOG_XWARNING:
			CloseFile();
			exit(0);
			break;		

		default:
			/* unknown client_data was recieved and
			   there is no setup to handle this */
			fprintf (stderr, "Warning: in apply callback\n");
			break;

	}
}

/*-------------------------------------------------------------
**	DialogCancelCB
**		Process callback from Dialog cancel actions.
*/
static void DialogCancelCB (w, client_data, call_data) 
Widget		w;		/*  widget id		*/
XtPointer		client_data;	/*  data from application   */
XtPointer		call_data;	/*  data from widget class  */
{
	switch ((int)client_data)
	{
		case DIALOG_FSELECT:
			/* popdown the file selection box */
		  	XtUnmanageChild (open_dialog);
			break;

		case DIALOG_CWARNING:
		case DIALOG_XWARNING:
		case DIALOG_NEW:
		case DIALOG_PRINT:
		case DIALOG_SAVE:
		case DIALOG_HELP:
			/* no action is necessary at this time */
			break;

		default:
			/* a unknown client_data was recieved and
			   there is no setup to handle this */
			fprintf (stderr, "Warning: in cancel callback\n");
			break;
	}
}


/*-------------------------------------------------------------
**	DialogAcceptCB
**		Process callback from Dialog actions.
*/
static void DialogAcceptCB (w, client_data, call_data) 
Widget		w;		/*  widget id		*/
XtPointer		client_data;	/*  data from application   */
XtPointer		call_data;	/*  data from widget class  */
{
	char *command;		/* command used in printing */

	switch ((int)client_data)
	{
		case DIALOG_FSELECT:
		        /* open the file and read it into the text widget */
			if (filename != NULL) {
			   XtFree(filename);
			   filename = NULL;
                        }
			{
			   XmFileSelectionBoxCallbackStruct *fcb =
				 (XmFileSelectionBoxCallbackStruct *) call_data;

			   /* get the filename from the file selection box */
			   XmStringGetLtoR(fcb->value, charset, &filename);

			   /* Open file, print error if it does not exist. */
			   if (!OpenFile())
	   		      fprintf(stderr, "Warning: unable to open file\n");

			   /* popdown the file selection box */
		  	   XtUnmanageChild (open_dialog);
			}
			break;

		case DIALOG_NEW:
		        /* open the file and read it into the text widget */
			if (filename != NULL) {
			   XtFree(filename);
			   filename = NULL;
                        }
			{
			   XmSelectionBoxCallbackStruct *scb =
				 (XmSelectionBoxCallbackStruct *) call_data;

			   /* get the filename string from the file
			      name prompt box */
			   XmStringGetLtoR(scb->value, charset, &filename);

			   /* open file if it exists,
			      if not set items sensitive */
			   if (!OpenFile(True)) {
   			   	/* make appropriate item sensitive */
   			   	XtSetSensitive(text, True);
   			   	XtSetSensitive(cut_button, True);
   			   	XtSetSensitive(copy_button, True);
   			   	XtSetSensitive(clear_button, True);
			   }

			   /* popdown the file selection box */
		  	   XtUnmanageChild (new_dialog);
			}
			break;

		case DIALOG_CWARNING:
			/* save the file */
			if (SaveFile()) {
			   CloseFile(); /* close the file */
			} else
			   fprintf(stderr, 
			       "Warning: unable to save file, file not closed");
			break;

		case DIALOG_XWARNING:
			/* save the file */
			if (SaveFile()) {
			   CloseFile(); /* close the file */
			   exit(0);
                        } else
			   fprintf(stderr,
				 "Warning: unable to save file, exit aborted");
			break;

		case DIALOG_SAVE:
			{
			   XmSelectionBoxCallbackStruct *scb =
				 (XmSelectionBoxCallbackStruct *) call_data;

			   /* get the filename string from the file
			      selection box */
			   XmStringGetLtoR(scb->value, charset, &filename);
			
			   SaveFile();

			   XtUnmanageChild (save_dialog);
			}
			break;

		case DIALOG_PRINT:
			/* save the file */
			if (SaveFile()) {
			   if (filename != NULL) {
			       /* malloc space for the command name. 
			          Note: command = size of the filename +
			          "lp " + null terminator */
				command = XtMalloc(strlen(filename) + 4);
				sprintf(command, "lp %s\0", filename);
				if (system(command) != NULL)
					fprintf(stderr, "print failed");
				XtFree(command);
				}
			} else
			   fprintf(stderr, 
			      "Warning: unable to save file, file not printed");
			break;

		case DIALOG_HELP:
			/* no help at this time */
			break;

		default:
			/* unknown callback type */
			fprintf (stderr, "Warning: in accept callback\n");
			break;
	}
}


/*-------------------------------------------------------------
**	CreateSpecialWarningDialog
**		Create special 4 button message box out of a
**	Selection box.
*/
static Widget CreateSpecialWarningDialog (parent, name, image_string, message,
					  arglist, argcount)
Widget		parent;
String		name;
String		image_string;
String		message;
ArgList		arglist;
int		argcount;
{
	Widget 		warning_dialog;	/*  special warning selection box */
	Widget 		work_area;	/*  rowcolumn for pixmap and text */
	Widget 		pixmap_label;	/*  pixmap label 		  */
	Widget 		text_label;	/*  text label 			  */
	Widget 		apply_button;	/*  apply button		  */
	Widget 		ok_button;	/*  ok button			  */
        Widget          kid[5];         /*  buttons		          */
	Pixel		foreground;	/*  dialog foreground		  */
	Pixel		background;	/*  dialog background		  */
	Pixmap		pixmap;		/*  dialog pixmap		  */
        register int    i;              /*  kid index			  */
        Arg             args[10];         /*  arg list		          */
        register int    n;             /*  arg count		          */


	warning_dialog = XmCreatePromptDialog(parent, name, arglist, argcount);

	n = 0;
	XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
	work_area = XmCreateRowColumn(warning_dialog, "workarea", args, n);
	XtManageChild(work_area);

	n = 0;
	XtSetArg(args[n], XmNforeground, &foreground); n++;
	XtSetArg(args[n], XmNbackground, &background); n++;
	XtGetValues(warning_dialog, args, n);

	n = 0;
	XtSetArg(args[n], XmNlabelType, XmPIXMAP); n++;
	pixmap = XmGetPixmap(XtScreen(warning_dialog), image_string,
			     foreground, background);
	XtSetArg(args[n], XmNlabelPixmap, pixmap); n++;
	pixmap_label = XmCreateLabel(work_area, "pixmap_label", args, n);
	XtManageChild(pixmap_label);

	n = 0;
	XtSetArg(args[n], XmNlabelString,
		 XmStringCreateLtoR(message, charset)); n++;
	text_label = XmCreateLabel(work_area, "text_label", args, n);
	XtManageChild(text_label);

        apply_button = XmSelectionBoxGetChild (warning_dialog,
							 XmDIALOG_APPLY_BUTTON);
	
	n = 0;
	XtSetArg(args[n], XmNlabelString,
		 XmStringCreateLtoR("Discard", charset)); n++;
	XtSetValues(apply_button, args, n);
	XtManageChild(apply_button);

        ok_button = XmSelectionBoxGetChild (warning_dialog,
							 XmDIALOG_OK_BUTTON);
	n = 0;
	XtSetArg(args[n], XmNlabelString,
		 XmStringCreateLtoR("Save", charset)); n++;
	XtSetValues(ok_button, args, n);

	
        /*      Unmanage unneeded children.
        */
        i = 0;
        kid[i++] = XmSelectionBoxGetChild (warning_dialog, XmDIALOG_TEXT);
        kid[i++] = XmSelectionBoxGetChild (warning_dialog,
						     XmDIALOG_SELECTION_LABEL);
        XtUnmanageChildren (kid, i);

	return(warning_dialog);
}


static void CreateFileMenuCB (w, client_data, call_data) 
Widget		w;		/*  widget id		*/
XtPointer		client_data;	/*  data from application   */
XtPointer		call_data;	/*  data from widget class  */

{

	CreateFileMenu((Widget)client_data);
	XtDestroyWidget(w);
	return;

}

CreateFileMenu(file_button)
Widget	file_button;

{

	Widget			menu_pane;
	XImage			*image;		/*  image for warning pixmap	*/
	Widget			button;		/*  PushButton			*/
	register int 	n;			/* arg count 	      	 */
	Arg 			args[10];		/* arg list		   	 */

	/*	Create "File" PulldownMenu.
	*/
	n = 0;
	menu_pane = XmCreatePulldownMenu (menu_bar, "menu_pane", args, n);

    image = CreateDefaultImage (warningBits, 32, 32);
    XmInstallImage (image, "warning_image");
					
	n = 0;
	XtSetArg(args[n], XmNlabelString,
		 XmStringCreateLtoR("Open", charset)); n++;
	XtSetArg(args[n], XmNmnemonic, 'O'); n++;
	button = XmCreatePushButton (menu_pane, "Open", args, n);
	XtAddCallback (button, XmNactivateCallback, MenuCB, (XtPointer) MENU_OPEN);
	XtManageChild (button);

	open_dialog = XmCreateFileSelectionDialog(menu_pane,
			   "file selection dialog", NULL, 0);

	XtAddCallback (open_dialog, XmNokCallback,
			DialogAcceptCB, (XtPointer) DIALOG_FSELECT);
	XtAddCallback (open_dialog, XmNcancelCallback,
			DialogCancelCB, (XtPointer) DIALOG_FSELECT);

	n = 0;
	XtSetArg(args[n], XmNlabelString,
		 XmStringCreateLtoR("New", charset)); n++;
	XtSetArg(args[n], XmNmnemonic, 'N'); n++;
	button = XmCreatePushButton (menu_pane, "New", args, n);
	XtAddCallback (button, XmNactivateCallback, MenuCB, (XtPointer) MENU_NEW);
	XtManageChild (button);

	n = 0;
	XtSetArg(args[n], XmNselectionLabelString, XmStringCreateLtoR
	   ("Enter name of new file.", charset));  n++;
	new_dialog = XmCreatePromptDialog(menu_pane,
			   "new file dialog", args, n);
	XtAddCallback (new_dialog, XmNokCallback,
			DialogAcceptCB, (XtPointer) DIALOG_NEW);
	XtAddCallback (new_dialog, XmNcancelCallback,
			DialogCancelCB, (XtPointer) DIALOG_NEW);

	n = 0;
	XtSetArg(args[n], XmNlabelString,
		 XmStringCreateLtoR("Close", charset)); n++;
	XtSetArg(args[n], XmNmnemonic, 'C'); n++;
	button = XmCreatePushButton (menu_pane, "Close", args, n);
	XtAddCallback (button, XmNactivateCallback, MenuCB, (XtPointer) MENU_CLOSE);
	XtManageChild (button);

	n = 0;
	close_warning = CreateSpecialWarningDialog(menu_pane, "save_warning",
				      "warning_image", "Save Changes?", args, n);

	XtAddCallback (close_warning, XmNapplyCallback,
		       DialogApplyCB, (XtPointer) DIALOG_CWARNING);
	XtAddCallback (close_warning, XmNokCallback,
		       DialogAcceptCB, (XtPointer) DIALOG_CWARNING);


	n = 0;
	XtSetArg(args[n], XmNlabelString,
		 XmStringCreateLtoR("Save", charset)); n++;
	XtSetArg(args[n], XmNmnemonic, 'S'); n++;
	button = XmCreatePushButton (menu_pane, "Save", args, n);
	XtAddCallback (button, XmNactivateCallback, MenuCB, (XtPointer) MENU_SAVE);
	XtManageChild (button);


	n = 0;
	XtSetArg(args[n], XmNlabelString,
		 XmStringCreateLtoR("Save As...", charset)); n++;
	XtSetArg(args[n], XmNmnemonic, 'A'); n++;
	button = XmCreatePushButton (menu_pane, "Save As...", args, n);
	XtAddCallback (button, XmNactivateCallback, MenuCB, (XtPointer) MENU_SAVE_AS);
	XtManageChild (button);

	n = 0;
	XtSetArg(args[n], XmNselectionLabelString, XmStringCreateLtoR
	   ("Save As...", charset));  n++;
	save_dialog = XmCreatePromptDialog(menu_pane, "save dialog", args, n);
	XtAddCallback (save_dialog, XmNokCallback,
		       DialogAcceptCB, (XtPointer) DIALOG_SAVE);


	n = 0;
	XtSetArg(args[n], XmNlabelString,
		 XmStringCreateLtoR("Print", charset)); n++;
	XtSetArg(args[n], XmNmnemonic, 'P'); n++;
	button = XmCreatePushButton (menu_pane, "Print", args, n);
	XtAddCallback (button, XmNactivateCallback, MenuCB, (XtPointer) MENU_PRINT);
	XtManageChild (button);

	n = 0;
	XtSetArg(args[n], XmNselectionLabelString, XmStringCreateLtoR
	   ("Save file before printing?", charset));  n++;
	print_warning = CreateSpecialWarningDialog(menu_pane, "print_warning",
			 "warning_image", "Save file before printing?", args, n);
	XtAddCallback (print_warning, XmNokCallback,
		       DialogAcceptCB, (XtPointer) DIALOG_PRINT);

	n = 0;
	XtSetArg(args[n], XmNlabelString,
		 XmStringCreateLtoR("Exit", charset)); n++;
	XtSetArg(args[n], XmNmnemonic, 'E'); n++;
	XtSetArg(args[n], XmNacceleratorText,
		 XmStringCreateLtoR("F3", charset)); n++;
	XtSetArg(args[n], XmNaccelerator, "<Key>F3:"); n++;
	button = XmCreatePushButton (menu_pane, "Exit", args, n);
	XtAddCallback (button, XmNactivateCallback, MenuCB, (XtPointer) MENU_EXIT);
	XtManageChild (button);

	n = 0;
	exit_warning = CreateSpecialWarningDialog(menu_pane, "exit warning",
				      "warning_image", "Save Changes?", args, n);
	XtAddCallback (exit_warning, XmNapplyCallback,
		       DialogApplyCB, (XtPointer) DIALOG_XWARNING);
	XtAddCallback (exit_warning, XmNokCallback,
			DialogAcceptCB, (XtPointer) DIALOG_XWARNING);

	n = 0;
	XtSetArg (args[n], XmNsubMenuId, menu_pane);  n++;
	XtSetValues(file_button, args, n);

}


static void CreateEditMenuCB (w, client_data, call_data) 
Widget		w;		/*  widget id		*/
XtPointer		client_data;	/*  data from application   */
XtPointer		call_data;	/*  data from widget class  */

{

	CreateEditMenu((Widget)client_data);
	XtDestroyWidget(w);
	return;

}


CreateEditMenu (edit_button) 
Widget		edit_button;

{

	Widget			menu_pane;
	XImage			*image;		/*  image for warning pixmap	*/
	Widget			button;		/*  PushButton			*/
	register int 	n;			/* arg count 	      	 */
	Arg 			args[10];		/* arg list		   	 */

	/*	Create "Edit" PulldownMenu.
	*/
	n = 0;
	menu_pane = XmCreatePulldownMenu (menu_bar, "menu_pane", args, n);

    image = CreateDefaultImage (warningBits, 32, 32);
    XmInstallImage (image, "warning_image");

	n = 0;
	XtSetArg(args[n], XmNlabelString,
		 XmStringCreateLtoR("Cut", charset)); n++;
	XtSetArg(args[n], XmNmnemonic, 't'); n++;
	XtSetArg(args[n], XmNacceleratorText,
		 XmStringCreateLtoR("Shift+Del", charset)); n++;
#ifdef hpux
	XtSetArg(args[n], XmNaccelerator, "Shift<Key>DeleteChar:"); n++;
#else /* hpux */
	XtSetArg(args[n], XmNaccelerator, "Shift<Key>Delete:"); n++;
#endif /* hpux */
	cut_button = XmCreatePushButton (menu_pane, "Cut", args, n);
	XtAddCallback (cut_button, XmNactivateCallback, MenuCB, (XtPointer) MENU_CUT);
	XtManageChild (cut_button);
	XtSetSensitive(cut_button, False);

	n = 0;
	XtSetArg(args[n], XmNlabelString,
		 XmStringCreateLtoR("Copy", charset)); n++;
	XtSetArg(args[n], XmNmnemonic, 'C'); n++;
	XtSetArg(args[n], XmNacceleratorText,
		 XmStringCreateLtoR("Ctrl+Ins", charset)); n++;
#ifdef hpux
	XtSetArg(args[n], XmNaccelerator, "Ctrl<Key>InsertChar:"); n++;
#else /* hpux */
	XtSetArg(args[n], XmNaccelerator, "Ctrl<Key>Insert:"); n++;
#endif /* hpux */
	copy_button = XmCreatePushButton (menu_pane, "Copy", args, n);
	XtAddCallback (copy_button, XmNactivateCallback, MenuCB, (XtPointer) MENU_COPY);
	XtManageChild (copy_button);
	XtSetSensitive(copy_button, False);

	n = 0;
	XtSetArg(args[n], XmNlabelString,
		 XmStringCreateLtoR("Paste", charset)); n++;
	XtSetArg(args[n], XmNmnemonic, 'P'); n++;
	XtSetArg(args[n], XmNacceleratorText,
		 XmStringCreateLtoR("Shift+Ins", charset)); n++;
#ifdef hpux
	XtSetArg(args[n], XmNaccelerator, "Shift<Key>InsertChar:"); n++;
#else /* hpux */
	XtSetArg(args[n], XmNaccelerator, "Shift<Key>Insert:"); n++;
#endif /* hpux */
	paste_button = XmCreatePushButton (menu_pane, "Paste", args, n);
	XtAddCallback (paste_button, XmNactivateCallback, MenuCB, (XtPointer) MENU_PASTE);
	XtManageChild (paste_button);
	XtSetSensitive(paste_button, False);

	n = 0;
	XtSetArg(args[n], XmNlabelString,
		 XmStringCreateLtoR("Clear", charset)); n++;
	XtSetArg(args[n], XmNmnemonic, 'e'); n++;
	/* pseudo accelerator - Text already handles
	   this action appropriately */
	XtSetArg(args[n], XmNacceleratorText,
		 XmStringCreateLtoR("Del", charset)); n++;
	clear_button = XmCreatePushButton (menu_pane, "Clear", args, n);
	XtAddCallback (clear_button, XmNactivateCallback, MenuCB, (XtPointer) MENU_CLEAR);
	XtManageChild (clear_button);
	XtSetSensitive(clear_button, False);

	n = 0;
	XtSetArg (args[n], XmNsubMenuId, menu_pane);  n++;
	XtSetValues(edit_button, args, n);

}


/*-------------------------------------------------------------
**	CreateMenuBar
**		Create MenuBar in MainWindow
*/
static Widget CreateMenuBar (parent)
Widget		parent;
{
	Widget		cascade;	/*  CascadeButton		*/
	Widget		file_cascade;	/*  CascadeButton		*/
	Widget		edit_cascade;	/*  CascadeButton		*/
	Widget		menu_pane;	/*  RowColumn	 		*/
	Widget		button;		/*  PushButton			*/

	XImage		*image;		/*  image for warning pixmap	*/

	Arg		args[10];		/*  arg list			*/
	register int	n;		/*  arg count			*/

	/*	Create MenuArea.
	*/
	n = 0;
	menu_bar = XmCreateMenuBar (parent, "menu_bar", args, n);


	XtSetArg(args[n], XmNlabelString,
		 XmStringCreateLtoR("File", charset)); n++;
	XtSetArg(args[n], XmNmnemonic, 'F'); n++;
	file_cascade = XmCreateCascadeButton (menu_bar, "File", args, n);
	XtManageChild (file_cascade);

	XtSetArg(args[n], XmNlabelString,
		 XmStringCreateLtoR("Edit", charset)); n++;
	XtSetArg(args[n], XmNmnemonic, 'E'); n++;
	edit_cascade= XmCreateCascadeButton (menu_bar, "Edit", args, n);
	XtManageChild (edit_cascade);

	/*	Create "Help" button.
	*/
	n = 0;
	cascade = XmCreateCascadeButton (menu_bar, "Help", args, n);
	XtAddCallback (cascade, XmNactivateCallback, MenuCB, (XtPointer) MENU_HELP);
	XtManageChild (cascade);
	n = 0;
	XtSetArg (args[n], XmNmenuHelpWidget, cascade);  n++;
	XtSetValues (menu_bar, args, n);

	/*	
	 * Create "Create File Menu" button if File menu not created at
	 * startup time.
	 */
	if (file_startup == True)
		CreateFileMenu(file_cascade);
	else {

		n = 0;
		cascade = XmCreateCascadeButton (menu_bar, "Create File Menu", args, n);
		XtAddCallback (cascade, XmNactivateCallback, CreateFileMenuCB, 
				   	    (XtPointer) file_cascade);
		XtManageChild (cascade);

	}

	/*	
	 * Create "Create Edit Menu" button if Edit menu not created at
	 * startup time.
	 */

	if (edit_startup == True)
		CreateEditMenu(edit_cascade);
	else {

		n = 0;
		cascade = XmCreateCascadeButton (menu_bar, "Create Edit Menu", args, n);
		XtAddCallback (cascade, XmNactivateCallback, CreateEditMenuCB, 
				   	    (XtPointer) edit_cascade);
		XtManageChild (cascade);

	}

	return (menu_bar);
}


/*-------------------------------------------------------------
**	CreateText
**		Create Text.
*/
static void CreateText (parent)
Widget		parent;
{
	Arg		args[10];		/*  arg list		*/
	register int	n;		/*  arg count		*/
	register int	i;		/*  counter		*/
   	XFontStruct * newfont;

	/* create text widget */
	n = 0;
	XtSetArg (args[n], XmNrows, 24);  n++;
	XtSetArg (args[n], XmNcolumns, 80);  n++;
	XtSetArg (args[n], XmNresizeWidth, False);  n++;
	XtSetArg (args[n], XmNresizeHeight, False);  n++;
	XtSetArg (args[n], XmNscrollVertical, True);  n++;
	XtSetArg (args[n], XmNscrollHorizontal, True);  n++;
	XtSetArg (args[n], XmNeditMode, XmMULTI_LINE_EDIT);  n++;

	text = XmCreateScrolledText (parent, "text", args, n);

	/* add value changed callback */
	XtAddCallback (text, XmNmodifyVerifyCallback, FileChangedCB, NULL);

	return;
}

/*-------------------------------------------------------------
**	main
**		Initialize toolkit.
**		Create MainWindow and subwidgets.
**		Realize Shell1 widgets.
**		Process events.
*/
void main (argc,argv)
unsigned int	argc;
char 		**argv;
{
	Widget		form;		/*  Form		*/

	Arg		args[10];		/*  arg list		*/
	register int	n;		/*  arg count		*/
	register int	i;		/*  counter		*/
	char	c;
	char	progname[MAX_NAME_LEN + 1];


	text_first = True;
	file_startup = False;
	edit_startup = False;
	text_startup = False;

	strncpy(progname, argv[0], MAX_NAME_LEN);
	if (argc > 1) {

		while (--argc > 0) {

			argv++;
			c = **argv;
			if (c != '-')
				continue;
			c = *(*argv + 1);
			switch(c) {

				case 'a':
					file_startup = True;
					edit_startup = True;
					text_startup = True;
					break;

				case 'e':
					edit_startup = True;
					break;

				case 'f':
					file_startup = True;
					break;

				case 'T':
					text_startup = True;
					break;

				case 'h':
					print_usage();
					exit(0);
					break;

			}
		}

	}

	/*	Initialize toolkit and open display.
	*/
    CommonTestInit(argc, argv);
    


	/*	Create MainWindow.
	*/
	n = 0;
	XtSetArg (args[n], XmNshadowThickness, 0);  n++;
	XtSetArg (args[n], XmNwidth, DEFAULT_WIDTH);  n++;
	XtSetArg (args[n], XmNheight, DEFAULT_HEIGHT);  n++;
	main_parent = XmCreateMainWindow (Shell1, "main_parent", args, n);
	XtManageChild (main_parent);


	/*	Create MenuBar in MainWindow.
	*/
	CreateMenuBar (main_parent);
	XtManageChild (menu_bar);

	if (text_startup == True) {

		CreateText (main_parent);
		XtManageChild (text);
		XmAddTabGroup(text);
		XtSetSensitive(text, False);
		text_first = False;

	}
	/*	Realize Shell1 widgets.
	*/
	XtRealizeWidget (Shell1);

	/*	Process events.
	*/
	XtAppMainLoop (app_context);
}


print_usage()
{

	printf("Usage: editor [-e -f -T -a -h]\n\tWhere :\n");
	printf("\t\ta - Create all the required widgets at startup time\n");
	printf("\t\te - Create Edit Menu pane widget at startup time\n");
	printf("\t\tf - Create File Menu pane widget at startup time\n");
	printf("\t\tT - Create Text widget at startup time\n");
	printf("\t\th - print this message\n");

}
