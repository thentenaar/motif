/* $XConsortium: animate.c /main/5 1995/07/15 20:44:58 drk $ */
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
/***************************************************************************
* Motifanim.
* ---------
* This program displays a animation made of a succession of pixmaps drawn
*    side by side in a scrolled drawingarea; there are also control 
*    buttons for managing the animation (start, speed, stop, step ...)
* This program reads two uid files: 'motifanim.uid', describing the general
*    interface of the buttons, label etc, and a <NAME>.uid file, describing
*    the data for the animation itself (essentially a set of icons to
*    be read using MrmFetchIconLiteral. <NAME> can be set at run time
*    in the command line and is supposed to define the following value:
*         anim_width     : width of the drawing area receiving animation
*         anim_height    : height of the drawing area receiving animation
*         anim_number    : number of icons in the animation (N)
*         <NAME>icon<N>  : name of the icon pixmap, for instance 'dogicon1'
*         anim_backcolor : color of the drawing area receiving animation
*         anim_step      : horizontal offset added before each animation step
* See dog.uil, the default <NAME>, for an example of such a file.
* -----------------------------------
* Authors: Daniel Dardailler, 90 (this version - last updated 94)
*          Vincent Bouthors,  89 (Egerie version)
*          Daniel Dardailler, 88 (xdog version)
****************************************************************************/

#include <stdio.h>
#include <Xm/Xm.h>          /* Motif Toolkit */
#include <Mrm/MrmPublic.h>   /* Mrm */
#include <Xmd/Help.h>   

static MrmHierarchy	s_MrmHierarchy;	   /* MRM database hierarch id */

static char *vec[2]={"animate.uid"};     /* MRM database file list   */
static int vecnum = sizeof(vec) / sizeof(char*);
       /* the second uid file name (vec[1]), containing animation
	  data, is taken at run time in command line option -anim */

static MrmCode		class, return_type ;

       /* forward declaration of interface procedures */
static void p_motifanim_start();
static void p_motifanim_stop();
static void p_motifanim_step();
static void p_motifanim_speed();
static void p_motifanim_draw();
static void p_motifanim_exit();
static void p_motifanim_help();
static void InitAnim();

       /* binding of uil procedure names with C functions */
static MRMRegisterArg	regvec[] = {
	{"p_motifanim_start",(XtPointer)p_motifanim_start},
	{"p_motifanim_stop",(XtPointer)p_motifanim_stop},
	{"p_motifanim_step",(XtPointer)p_motifanim_step},
	{"p_motifanim_speed",(XtPointer)p_motifanim_speed},
	{"p_motifanim_exit",(XtPointer)p_motifanim_exit},
	{"p_motifanim_draw",(XtPointer)p_motifanim_draw},
	{"p_motifanim_help",(XtPointer)p_motifanim_help}
	};
static MrmCount regnum = XtNumber(regvec);


static Display 	*display;
static XtAppContext    app_context;

/** 
--  Animation variables 
**/
static int scalespeed, max_scale ; /* init with widget values */
static short width ;

static Boolean stop = True ;

static int xanim = 50,
           yanim = 25,
           ianim = 0 ;

static int speed_factor = 20 ;                /* reinitialized with argv */
static char * anim_name = "dog";              /* reinitialized with argv */

#define MAXIMAGES 10
static unsigned int wanim[MAXIMAGES], hanim[MAXIMAGES] ;

static Pixmap panim[MAXIMAGES] ;   /* initalized with uil value by fetching */
static int nimage, step_size  ;    /* initalized with uil value by fetching */

static GC gc ;
static Widget drawingArea;
static int speedcount ;

#define APP_CLASS "XmdAnimate"

static String fallbacks[] = {
"*help_manager.helpFile: animate",
NULL
};

/******************************************************************
 *  Main program: motifanim [-anim anim_name] [-speed speed_factor]
 */
int main(argc, argv)
     int    argc;
     String argv[];
{
     /*
     *  Declare the variables to contain the two widget ids
     */
    Widget toplevel, motifanimmain = NULL ;
    Arg arglist[1] ;
    char uidanimfile[100] ;
    int n;


    MrmInitialize ();
    XtSetLanguageProc(NULL, NULL, NULL);
    toplevel = XtVaOpenApplication(&app_context, APP_CLASS,
				 NULL , 0, &argc, argv, fallbacks, 
                                 sessionShellWidgetClass, 
                                 NULL);

    /*
     *  Parsing of the remaining animation options
     */

    while (*++argv) {
	if (!strcmp(*argv,"-anim")) {
	    if (*++argv)  anim_name = *argv ; 
	} else
	if (!strcmp(*argv,"-speed")) {
	    if (*++argv)  speed_factor = atoi(*argv) ; 
	}
    }
		

    /*
     *  Build the secong uid file and open the Mrm.hierarchy (2 files)
     */ 
    strcpy(uidanimfile,anim_name);
    strcat(uidanimfile,".uid");
    vec[1] = uidanimfile ;
    if (MrmOpenHierarchy (vecnum,	 /* number of files	    */
			vec, 		 /* files     	    */
			NULL,		 /* os_ext_list (null)   */
			&s_MrmHierarchy) /* ptr to returned id   */
			!= MrmSUCCESS) {
	printf ("can't open hierarchy defined by %s and %s\n",vec[0],vec[1]);
	exit(0);
     }

    if (MrmRegisterNames (regvec, regnum)
			!= MrmSUCCESS) {
	printf("can't register names\n");
	exit(0) ;
    }

    if (MrmFetchWidget (s_MrmHierarchy,
			"motifanim_main",
			toplevel,
			&motifanimmain,
			&class)
			!= MrmSUCCESS) {
	printf("can't fetch interface\n");
	exit(0);
    }

    XtManageChild(motifanimmain);
    
    XtRealizeWidget(toplevel);

    /*
     *  Call the routine that will fetch the animation variables.
     */
    
    InitAnim() ;


    XtAppMainLoop(app_context);

    /* UNREACHABLE */
    return (0);
}


static void InitAnim()
/********/
{

    XGCValues gcv;
    int i,dum ;
    char uiliconname[100];
    int * pint ;
    Window wdum ;

    /* fetch the number of animation icons from Mrm hierarchy */
    if (MrmFetchLiteral(s_MrmHierarchy,
			"anim_number",
			XtDisplay(drawingArea),
			(XtPointer *)&pint,
			&return_type) != MrmSUCCESS) {
	printf("Can't fetch literal anim_number in %s.uid\n",anim_name);
	exit(0) ;
    }

    nimage = (*pint > MAXIMAGES)?(MAXIMAGES-1):(*pint-1);

    /* fetch the step value from Mrm hierarchy */
    if (MrmFetchLiteral(s_MrmHierarchy,
			"anim_step",
			XtDisplay(drawingArea),
			(XtPointer *)&pint,
			&return_type) != MrmSUCCESS) {
	printf("Can't fetch literal anim_step in %s.uid\n",anim_name);
	exit(0) ;
    }

    step_size = *pint ;

    /* fetch the icon pixmaps from Mrm hierarchy */
    for (i = 0 ; i <= nimage ; i++) {
	sprintf(uiliconname, "%sicon%d",anim_name, i+1);
	if (MrmFetchIconLiteral(s_MrmHierarchy,
			uiliconname,
			XtScreen(drawingArea),
			XtDisplay(drawingArea),
			BlackPixel(XtDisplay(drawingArea),
				   DefaultScreen(XtDisplay(drawingArea))),
			WhitePixel(XtDisplay(drawingArea),
				   DefaultScreen(XtDisplay(drawingArea))),
			&panim[i]) != MrmSUCCESS) {
	    fprintf(stderr, "Can't fetch uiliconname %s\n", uiliconname);
	    exit (-1);
	}
	/* get the icons geometry with X standard requests */
	XGetGeometry(XtDisplay(drawingArea),panim[i],&wdum,
		     &dum,&dum,&wanim[i],&hanim[i],
		     (unsigned int*)&dum,(unsigned int*)&dum);
    }

    /* to avoid event accumulation during animation */
    gcv.graphics_exposures = False ; 
    gc = XCreateGC(XtDisplay(drawingArea), XtWindow(drawingArea), 
		   GCGraphicsExposures, &gcv);
    speedcount = 0 ;
}



/**
  Background Work Procedure: it return the current value of stop
  and then is automatically removed when stop = true.
**/
static Boolean fstep(client_data)
/************************/
     XtPointer client_data ;       /* scalespeed */
{
    speedcount += (int)*(int*)client_data ;
    if (speedcount >= (max_scale*speed_factor)) {
	speedcount = 0 ;
	XClearArea(XtDisplay(drawingArea), XtWindow(drawingArea),
		   xanim, yanim, wanim[ianim], hanim[ianim] , False);
	xanim = (xanim > width)?(-wanim[ianim]):(xanim+step_size) ;
	ianim = (ianim == nimage)?0:(ianim+1);
	XCopyArea(XtDisplay(drawingArea), panim[ianim], XtWindow(drawingArea),
		  gc, 0, 0, wanim[ianim], hanim[ianim], 
		  xanim, yanim);
	XSync(XtDisplay(drawingArea),False);
    }
    return stop ;
}

/** The callbacks **/
/*******************/

static void 
p_motifanim_help (Widget w, XtPointer client_data, XtPointer call_data) 
{
    static Widget help_widget = NULL ;

    if (!help_widget)
	help_widget = XmdCreateHelpDialog(w, "help_manager", NULL, 0);

    XtManageChild(help_widget);   
}


static void p_motifanim_start( widget, tag, callback_data )
	Widget	widget;
	char    *tag;
	XmAnyCallbackStruct *callback_data;
{
    XtAppAddWorkProc(app_context,fstep, (XtPointer)&scalespeed);
    stop = False ;
}

static void p_motifanim_stop( widget, tag, callback_data )
	Widget	widget;
	char    *tag;
	XmAnyCallbackStruct *callback_data;
{
    stop = True ;
}

static void p_motifanim_step( widget, tag, callback_data )
	Widget	widget;
	char    *tag;
	XmAnyCallbackStruct *callback_data;
{
    int max = (max_scale*speed_factor) ;

    fstep(&max) ;
}

static void p_motifanim_speed( widget, tag, callback_data )
	Widget	widget;
	int    *tag;
	XmScaleCallbackStruct *callback_data;
{
    Arg arg ;

    if (*tag == 0) {
	XmScaleGetValue(widget,&scalespeed);
	XtSetArg(arg,XmNmaximum,&max_scale);
	XtGetValues(widget,&arg,1);
    }
    else scalespeed = callback_data->value ;
}

static void p_motifanim_draw( widget, tag, callback_data )
	Widget	widget;
	int    *tag;
	XmAnyCallbackStruct *callback_data;
{
    Arg arg ;

    if (*tag == 0) {
	XtSetArg(arg,XmNwidth,&width);
	XtGetValues(widget,&arg,1);
	drawingArea = widget ; 
    } else {
	XCopyArea(XtDisplay(drawingArea), panim[ianim], 
		  XtWindow(drawingArea),
		  gc, 0, 0, wanim[ianim], hanim[ianim], 
		  xanim, yanim);
    }

}

static void p_motifanim_exit( widget, tag, callback_data )
	Widget	widget;
	char    *tag;
	XmAnyCallbackStruct *callback_data;
{
    exit(0);
}
