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
*/ 
#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$XConsortium: hellomotif.c /main/6 1995/07/14 09:42:12 drk $"
#endif
#endif
#include <stdlib.h>
#include <Xm/XmAll.h>       /* Motif Toolkit */
#include <Mrm/MrmPublic.h>  /* Mrm Toolkit */


/* Although it's really bad to hardcode Positions, keep this for
   now for compatibility */
#define NEW_BUTTON_X 11
/* not mentioning hardcoding labels... look at hellomotif_i18n for a 
   better example  */
#define NEW_BUTTON_LABEL  "Goodbye\nWorld!"


static void Activate(
        Widget	widget,
	XtPointer tag,
	XtPointer data);



static String uid_vec[]={"hellomotif.uid"}; /* MRM database file list   */
static MrmRegisterArg	reg_vec[] = {       /* Callback bindings */
    {"helloworld_button_activate", (XtPointer)Activate}
};


/*
 *  Main program: fetch the hellomotif.uil description.
 */
int main(int argc, char *argv[])
{
    XtAppContext        app_context;
    Widget              top_level, helloworld_main;
    MrmCode		mrm_class ;
    MrmHierarchy	mrm_hierarchy;		

    /*
     *  Initialize the MRM
     */

    MrmInitialize ();

    /* 
     * Initialize Xt and create a resizable shell 
     */

    top_level = XtVaAppInitialize(&app_context, "XmdHelloMotif", 
				  NULL, 0, &argc, argv, NULL, 
				  XmNallowShellResize, True, NULL); 

    /*
     *  Create the Mrm hierarchy
     */

    if (MrmOpenHierarchy (XtNumber(uid_vec),	    /* number of files	    */
			  uid_vec, 		    /* files     	    */
			  NULL,			    /* os_ext_list (null)   */
			  &mrm_hierarchy)	    /* ptr to returned id   */
	!= MrmSUCCESS) {
	XtError ("can't open hierarchy\n");
    }

    /*
     * 	Register our callback routines so that the resource manager can 
     * 	resolve them at widget-creation time.
     */

    if (MrmRegisterNames (reg_vec, XtNumber(reg_vec)) != MrmSUCCESS)
	XtError("can't register names\n");

    /*
     *  Call MRM to fetch and create the widgets 
     */

    if (MrmFetchWidget (mrm_hierarchy,
			"helloworld_main",
			top_level,
			&helloworld_main,
			&mrm_class)
	!= MrmSUCCESS)
	XtError ("can't fetch interface\n");

    /*
     *  Make the toplevel widget "manage" the main window (or whatever the
     *  the uil defines as the topmost widget).  This will
     *  cause it to be "realized" when the toplevel widget is "realized"
     */

    XtManageChild(helloworld_main);
    
    /*
     *  Realize the toplevel widget.  This will cause the entire "managed"
     *  widget hierarchy to be displayed
     */

    XtRealizeWidget(top_level);

    /*
     *  Loop and process events
     */

    XtAppMainLoop(app_context);

    /* UNREACHABLE */
    return (0);
}


static void 
Activate(
        Widget	widget,
	XtPointer tag,
	XtPointer data)

{
    static Boolean first_time = True ;

    /* First click: change the pushbutton label and recenter */
    if (first_time) {
	XmString new_button_label = XmStringGenerate(NEW_BUTTON_LABEL,
						     NULL, 
						     XmCHARSET_TEXT, NULL);
	XtVaSetValues(widget, 
		      XmNx, NEW_BUTTON_X,
		      XmNlabelString, new_button_label,
		      NULL);

	XmStringFree(new_button_label);
	first_time = False ;
    } else {
	exit(0);
    }
}

 
