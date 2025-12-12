/* $XConsortium: exm_in_uil.c /main/5 1995/07/15 20:44:21 drk $ */
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
 * 
 */
/*
 * HISTORY
 */

/*******************************************************************************
 *
 * exm_in_uil.c - main routine and C callbacks for exm_in_uil program.
 *
 ******************************************************************************/

#include <stdio.h>
#include <Xm/Xm.h>
#include <Xm/MessageB.h>
#include <Mrm/MrmPublic.h>
#include <Exm/ExmMrm.h>

static void ManageCb(Widget, String, XtPointer);
static void ExitCb(Widget, XtPointer, XtPointer);

static MrmHierarchy mrmId;
static MrmCode mrmClass;
static char *mrmFiles[]={"exm_in_uil.uid"};
static MRMRegisterArg mrmNames[] = {
        {"ManageCb", (caddr_t)ManageCb },
        {"ExitCb", (caddr_t)ExitCb }
};

static Display *display;
static Widget shell;

int 
main(int argc,
     char **argv)
{
Widget appMain;
XtAppContext appContext;

    MrmInitialize();
    ExmMrmInitialize();	/* register Exm widgets with Mrm */
    XtToolkitInitialize();
    appContext = XtCreateApplicationContext();
    display = XtOpenDisplay(appContext, NULL, "exmdemo", "ExmDemo", NULL, 0,
                            &argc, argv);
    if (display == NULL) {
	fprintf(stderr, "Can't open display\n");
	exit(1);
    }
    shell = XtAppCreateShell("demo", NULL, applicationShellWidgetClass,
						display, NULL, 0);
    if (MrmOpenHierarchy (1, mrmFiles, NULL, &mrmId) != MrmSUCCESS) 
      exit(1);
    MrmRegisterNames(mrmNames, XtNumber(mrmNames));
    MrmFetchWidget (mrmId, "appMain", shell, &appMain, &mrmClass);
    XtManageChild(appMain);
    XtRealizeWidget(shell);
    XtAppMainLoop(appContext);
    exit(0);
}



/*
 * ManageCb - Manage widget passed as client_data.
 */
static void 
ManageCb(Widget w,
	 String id,
	 XtPointer cb)
{
    XtManageChild (XtNameToWidget(shell, id));
}


/*
 * ExitCb - Exit application.
 */
static void 
ExitCb(Widget w,
       XtPointer cd,
       XtPointer cb)
{
    exit(0);
}
