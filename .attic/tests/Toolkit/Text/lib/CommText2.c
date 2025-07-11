/* $XConsortium: CommText2.c /main/5 1995/07/15 21:11:03 drk $ */
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

Arg args[MAX_ARGS];
int n;

static char starting_string[] = "Text provides a single and multiline text editor for customizing both user and programmatic interfaces.\n\nText provides separate callback lists to verify the movement of the insert cursor, modification of the text, and changes in input focus.";


int main_text2 (argc, argv)
     int argc;
     char **argv;
{
   Widget         Text1;
   XmString       tcs;
   XtTranslations new_table;
   static char new_translations[] =
	"Ctrl<Key>g:               beep() \n\
	 Ctrl<Key>i:               insert-string(newstring) \n\
	 Ctrl<Key>bracketright:    forward-paragraph() \n\
	 Ctrl<Key>bracketleft:     backward-paragraph() \n\
	 Meta<Key>d:               scroll-one-line-down() \n\
	 Meta<Key>u:               scroll-one-line-up() \n\
	 Ctrl<Key>f:               next-page() \n\
	 Ctrl<Key>b:               previous-page() \n\
	 Ctrl<Key>u:         	   unkill() \n\
	 <Key>BackSpace:	   kill-previous-character() \n\
	 Ctrl<Key>s:		   kill-selection() \n\
	 Ctrl<Key>k:	   	   kill-next-word() \n\
	 Meta<Key>k:	   	   kill-previous-word() \n\
	 Ctrl<Key>r:   	   	   delete-previous-word() \n\
	 Meta<Key>r:	           delete-next-word() \n\
	 Ctrl<Key>c:		   cut-primary()";

/* cut-primary() added for Pir 4244 */

   /*  initialize toolkit  */
   CommonTestInit(argc, argv);
    
   /* Begin Test for Pir2718 */

   n = 0;
   XtSetArg(args[n], XmNdefaultFontList, CommonGetFontList("variable"));  n++;

   /* End Test for Pir2718 */

   XtSetArg(args[n], XmNwidth, 390); n++;
   XtSetArg(args[n], XmNheight, 115); n++;
   XtSetValues(Shell1, args, n);
   
   new_table = XtParseTranslationTable (new_translations);

   n = 0;
   XtSetArg(args[n], XmNeditMode, XmMULTI_LINE_EDIT);  n++;
   XtSetArg(args[n], XmNwordWrap, True); n++;
   XtSetArg(args[n], XmNresizeWidth, False);  n++;
   XtSetArg(args[n], XmNresizeHeight, False);  n++;
   XtSetArg(args[n], XmNcolumns, 40);  n++;
   XtSetArg(args[n], XmNcursorPosition, 35);  n++;
   XtSetArg(args[n], XmNrows, 3);  n++;
   XtSetArg(args[n], XmNvalue, starting_string);  n++;
   Text1 = XmCreateText(Shell1, "Text1", args, n);
   XtManageChild(Text1);

   XtOverrideTranslations(Text1, new_table);
   XtRealizeWidget(Shell1);

   CommonPause ();              /* try translations */

#ifdef	PIR_CLOSED
   /* test case for PIR 3520 */
   CommonPause ();
   /* end of test case for PIR 3520 */
#endif	/* PIR_CLOSED */

   CommonPause ();              /* test for 8029 */

   CommonPause ();              /* exit */

   XtAppMainLoop(app_context);
}
