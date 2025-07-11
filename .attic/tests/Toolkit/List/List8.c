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
static char rcsid[] = "$XConsortium: List8.c /main/7 1995/07/13 18:21:19 drk $"
#endif
#endif

/* Test for Pir 2511 */

#include <testlib.h>

static void listCallback(list, p1, p2) 
Widget list;
XtPointer p1;
XtPointer p2; 
{
    XmString	strs[10];
    Cardinal	i, n;
    Arg		args[5];
    int		itemCnt;

    XtSetArg(args[0], XmNitemCount, &itemCnt);
    XtGetValues(list, args, 1);

    if (itemCnt >= 10) 
	itemCnt = 1;
    else 
	itemCnt = 10;

    printf("Switching to %d items\n", itemCnt);

    for (i = 0; i < itemCnt; ++i) 
	strs[i] = XmStringCreateSimple("foobar is as foobar does");

    n = 0;
    XtSetArg(args[n], XmNitems, strs); 			n++;
    XtSetArg(args[n], XmNitemCount, itemCnt); 		n++;
    XtSetArg(args[n], XmNselectedItemCount, 0); 	n++;
    XtSetValues(list, args, n);

    for (i = 0; i < itemCnt; ++i) 
	XmStringFree(strs[i]);

}


main(argc, argv)
int 	argc;
char	*argv[];
{
    Cardinal		n, i;
    Arg			args[5];
    Widget		list;
    XmString		strs[10];
    Widget		parent;
    int			count;
    Dimension		h;
    Boolean		manage_sooner = False; 

    CommonTestInit(argc, argv);

    if (UserData && (strcmp (UserData, "sooner") == 0)) 
	manage_sooner = True;

    for (i = 0; i < 10; ++i) 
	strs[i] = XmStringCreateSimple("foobar is as foobard does");

    n = 0;
    XtSetArg(args[n], XmNitems, strs); 					n++;
    XtSetArg(args[n], XmNitemCount, 10); 				n++;
    XtSetArg(args[n], XmNfontList, CommonGetFontList("8x13bold"));	n++;
    list = XmCreateList(Shell1, "List1", args, n);
    XtManageChild(list);

    n = 0;
    XtSetArg(args[n], XmNvisibleItemCount, 10);				n++;
    XtSetValues(list, args, n);

    XtAddCallback(list, XmNbrowseSelectionCallback, listCallback, NULL);
    XtAddCallback(list, XmNdefaultActionCallback, listCallback, NULL);
    XtAddCallback(list, XmNextendedSelectionCallback, listCallback, NULL);
    XtAddCallback(list, XmNmultipleSelectionCallback, listCallback, NULL);
    XtAddCallback(list, XmNsingleSelectionCallback, listCallback, NULL);


    XtRealizeWidget(Shell1);

    CommonPause();

    /* begin Test for  PIR 4014 */

    XtDestroyWidget (list);
    
    n = 0;
    XtSetArg (args[n], XmNheight, 75); n++;
/*    parent = XmCreateForm (Shell1, "parent", args, n); */
    parent = XmCreateBulletinBoard (Shell1, "parent", args, n);
    if (manage_sooner) 
       XtManageChild (parent);
    
    n = 0;
    XtSetArg(args[n], XmNitems, strs); 					n++;
    XtSetArg(args[n], XmNitemCount, 10); 				n++;
    XtSetArg(args[n], XmNfontList, CommonGetFontList("8x13bold"));	n++;
/*    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++; */
    list = XmCreateList(parent, "List1", args, n);
    XtManageChild(list);
    if (!manage_sooner)
	XtManageChild (parent);

    n = 0;
    XtSetArg (args[n], XmNvisibleItemCount, &count); n++;
    XtGetValues (list, args, n);
    printf ("Visible item count is %d\n", count); 
    n = 0;
    XtSetArg (args[n], XmNvisibleItemCount, count); n++;
    XtSetValues (list, args, n);

    n = 0;
    XtSetArg (args[n], XmNheight, &h); n++;
    XtGetValues (parent, args, n);
    printf ("Height of form is %d\n", (int) h);


    CommonPause();
  
    /* end test for PIR 4014 */
 
    for (i = 0; i < 10; ++i) 
	XmStringFree(strs[i]);
    XtAppMainLoop(app_context);
}
