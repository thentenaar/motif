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
static char rcsid[] = "$XConsortium: xmruler.c /main/6 1995/07/13 20:57:53 drk $"
#endif
#endif
/*
 *    file:    xmruler.c
 */

#include <testlib.h>
#include <X11/cursorfont.h>

#define    XK_MISCELLANY
#include <X11/keysymdef.h>

#define    MM_PER_INCH    25.4

/* callback procedures */
static void  quitCB();
static void  measureFunc();
static Boolean    do_measure();
static void    draw_box();


Widget        upper_left_Xlabel, upper_left_Xvalue;
Widget        upper_left_Ylabel, upper_left_Yvalue;
Widget        lower_right_Xlabel, lower_right_Xvalue;
Widget        lower_right_Ylabel, lower_right_Yvalue;

Widget        pixel_width_label, width_in_pixels;
Widget        pixel_height_label, height_in_pixels;
Widget        mm_width_label, width_in_mms;
Widget        mm_height_label, height_in_mms;
Widget        inch_width_label, width_in_inches;
Widget        inch_height_label, height_in_inches;


int    user_button, press_button;
int    box_x, box_y;
unsigned int    box_width, box_height;

int    upper_leftX, upper_leftY;
int    lower_rightX, lower_rightY;
int    pixel_width, pixel_height;
double mm_width, mm_height;
double inch_width, inch_height;

int    screenWidth, screenHeight;
double screenWidthMM, screenHeightMM;
double screenWidthInches, screenHeightInches;

int    prev_ulx = 0;
int    prev_uly = 0;
int    prev_lrx = 0;
int    prev_lry = 0;


static void  measureFunc(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{

    Arg    args[1];
    int    n;
    char    label[100];
    int    value;

    upper_leftX = prev_ulx;
    upper_leftY = prev_uly;
    lower_rightX = prev_lrx;
    lower_rightY = prev_lry;

    XtUnmapWidget(Shell1);

    if (do_measure() == True) {

        prev_ulx = upper_leftX;
         prev_uly = upper_leftY;
         prev_lrx = lower_rightX;
         prev_lry = lower_rightY;

    }

    n = 0;
    sprintf(label, "%d", upper_leftX);
    XtSetArg(args[n], XmNlabelString, XmStringCreateSimple (label)); n++;
    XtSetValues(upper_left_Xvalue, args, n);

    n = 0;
    sprintf(label, "%d", upper_leftY);
    XtSetArg(args[n], XmNlabelString, XmStringCreateSimple (label)); n++;
    XtSetValues(upper_left_Yvalue, args, n);

    n = 0;
    sprintf(label, "%d", lower_rightX);
    XtSetArg(args[n], XmNlabelString, XmStringCreateSimple (label)); n++;
    XtSetValues(lower_right_Xvalue, args, n);

    n = 0;
    sprintf(label, "%d", lower_rightY);
    XtSetArg(args[n], XmNlabelString, XmStringCreateSimple (label)); n++;
    XtSetValues(lower_right_Yvalue, args, n);

    pixel_width = lower_rightX - upper_leftX;
    pixel_height = lower_rightY - upper_leftY;
    mm_width = pixel_width * (screenWidthMM / screenWidth);
    mm_height = pixel_height * (screenHeightMM / screenHeight);
    inch_width = pixel_width * (screenWidthInches / screenWidth);
    inch_height = pixel_height * (screenHeightInches / screenHeight);


    n = 0;
    sprintf(label, "%d", pixel_width);
    XtSetArg(args[n], XmNlabelString, XmStringCreateSimple (label)); n++;
    XtSetValues(width_in_pixels, args, n);

    n = 0;
    sprintf(label, "%d", pixel_height);
    XtSetArg(args[n], XmNlabelString, XmStringCreateSimple (label)); n++;
    XtSetValues(height_in_pixels, args, n);

    n = 0;
    sprintf(label, "%.2f", mm_width);
    XtSetArg(args[n], XmNlabelString, XmStringCreateSimple (label)); n++;
    XtSetValues(width_in_mms, args, n);

    n = 0;
    sprintf(label, "%.2f", mm_height);
    XtSetArg(args[n], XmNlabelString, XmStringCreateSimple (label)); n++;
    XtSetValues(height_in_mms, args, n);

    n = 0;
    sprintf(label, "%.2f", inch_width);
    XtSetArg(args[n], XmNlabelString, XmStringCreateSimple (label)); n++;
    XtSetValues(width_in_inches, args, n);

    n = 0;
    sprintf(label, "%.2f", inch_height);
    XtSetArg(args[n], XmNlabelString, XmStringCreateSimple (label)); n++;
    XtSetValues(height_in_inches, args, n);

    XtMapWidget(Shell1);

}


static void  quitCB(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
     printf("Begin exiting ... please standby ... \n");
     fflush(stdout);
     exit(0);
}


void  main(argc, argv)
    int     argc;
    char  **argv;
{
    register int  n;
    Arg           args[MAX_ARGS];
    Widget        Form1;
    Widget          RowCol1, RowCol2;
    Widget        quit;
    Widget        measure;


    CommonTestInit(argc, argv);
    

    screenWidth = WidthOfScreen(screen);
    screenHeight = HeightOfScreen(screen);
    screenWidthMM = WidthMMOfScreen(screen);
    screenHeightMM = HeightMMOfScreen(screen);
    screenWidthInches = screenWidthMM / MM_PER_INCH;
    screenHeightInches = screenHeightMM / MM_PER_INCH;

    pixel_width = pixel_height = 
    mm_width = mm_height = 
    inch_width = inch_height = 0;

    /* Shell 1 children */

    n = 0;
    Form1 = XmCreateForm (Shell1, "Form1", args, n);
    XtManageChild (Form1);
    
    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNtopOffset, 5); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftOffset, 5); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, 50); n++;
    XtSetArg(args[n], XmNentryAlignment, XmALIGNMENT_CENTER); n++;
    XtSetArg(args[n], XmNspacing, 5); n++;
    XtSetArg(args[n], XmNorientation, XmVERTICAL); n++;
    XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
    XtSetArg(args[n], XmNbackground, CommonGetColor("darkgreen")); n++;

    RowCol1 = XmCreateRowColumn(Form1, "RowCol1", args, n);
    XtManageChild(RowCol1);
    
    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNtopOffset, 5); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, 50); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightOffset, 5); n++;
    XtSetArg(args[n], XmNentryAlignment, XmALIGNMENT_CENTER); n++;
    XtSetArg(args[n], XmNspacing, 5); n++;
    XtSetArg(args[n], XmNorientation, XmVERTICAL); n++;
    XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
    XtSetArg(args[n], XmNbackground, CommonGetColor("darkgreen")); n++;

    RowCol2 = XmCreateRowColumn(Form1, "RowCol2", args, n);
    XtManageChild(RowCol2);

    n = 0;
    upper_left_Xlabel = XmCreateLabel (RowCol1, "UpperLeft_X: ", args, n);      
    XtManageChild (upper_left_Xlabel);
    
    n = 0;
    upper_left_Ylabel = XmCreateLabel (RowCol1, "UpperLeft_Y: ", args, n);      
    XtManageChild (upper_left_Ylabel);
    
    n = 0;
    lower_right_Xlabel = XmCreateLabel (RowCol1, "LowerRight_X: ", args, n);
    XtManageChild (lower_right_Xlabel);
    
    n = 0;
    lower_right_Ylabel = XmCreateLabel (RowCol1, "LowerRight_Y: ", args, n);
    XtManageChild (lower_right_Ylabel);
    
    n = 0;
    pixel_width_label = XmCreateLabel (RowCol1, "Width_In_Pixels: ", args, n);
    XtManageChild (pixel_width_label);
    
    n = 0;
    pixel_height_label = XmCreateLabel (RowCol1, "Height_In_Pixels: ", args, n);
    XtManageChild (pixel_height_label);
    
    n = 0;
    mm_width_label = XmCreateLabel (RowCol1, "Width_In_MMs: ", args, n);      
    XtManageChild (mm_width_label);
    
    n = 0;
    mm_height_label = XmCreateLabel (RowCol1, "Height_In_MMs: ", args, n);
    XtManageChild (mm_height_label);
    
    n = 0;
    inch_width_label = XmCreateLabel (RowCol1, "Width_In_Inches: ", args, n);
    XtManageChild (inch_width_label);
    
    n = 0;
    inch_height_label = XmCreateLabel (RowCol1, "Height_In_Inches: ", args, n);
    XtManageChild (inch_height_label);

    n = 0;
    upper_left_Xvalue = XmCreateLabel (RowCol2, "0", args, n);      
    XtManageChild (upper_left_Xvalue);
    
    n = 0;
    upper_left_Yvalue = XmCreateLabel (RowCol2, "0", args, n);      
    XtManageChild (upper_left_Yvalue);
    
    n = 0;
    lower_right_Xvalue = XmCreateLabel (RowCol2, "0", args, n);      
    XtManageChild (lower_right_Xvalue);
    
    n = 0;
    lower_right_Yvalue = XmCreateLabel (RowCol2, "0", args, n);      
    XtManageChild (lower_right_Yvalue);
    
    n = 0;
    width_in_pixels = XmCreateLabel (RowCol2, "0", args, n);      
    XtManageChild (width_in_pixels);
    
    n = 0;
    height_in_pixels = XmCreateLabel (RowCol2, "0", args, n);      
    XtManageChild (height_in_pixels);
    
    n = 0;
    width_in_mms = XmCreateLabel (RowCol2, "0.0", args, n);      
    XtManageChild (width_in_mms);
    
    n = 0;
    height_in_mms = XmCreateLabel (RowCol2, "0.0", args, n);      
    XtManageChild (height_in_mms);
    
    n = 0;
    width_in_inches = XmCreateLabel (RowCol2, "0.0", args, n);      
    XtManageChild (width_in_inches);
    
    n = 0;
    height_in_inches = XmCreateLabel (RowCol2, "0.0", args, n);      
    XtManageChild (height_in_inches);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopOffset, 5); n++;
    XtSetArg(args[n], XmNtopWidget, RowCol1); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftOffset, 5); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, 50); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomOffset, 5); n++;
    XtSetArg(args[n], XmNlabelString, XmStringCreateSimple ("Measure")); n++;
    measure = XmCreatePushButton (Form1, "Measure", args, n);      
    XtManageChild (measure);
    XtAddCallback (measure,  XmNactivateCallback, measureFunc, NULL);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopOffset, 5); n++;
    XtSetArg(args[n], XmNtopWidget, RowCol2); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, 50); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightOffset, 5); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomOffset, 5); n++;
    XtSetArg (args[n], XmNlabelString, XmStringCreateSimple ("Quit")); n++;
    quit = XmCreatePushButton (Form1, "Quit", args, n);      
    XtManageChild (quit);
    XtAddCallback (quit,  XmNactivateCallback, quitCB, NULL);
    
    XtRealizeWidget(Shell1);

    XtAppMainLoop(app_context);

}


static Boolean do_measure ()
{

    register int x, y;
    Window root_window, child_window;
    int root_x, root_y, win_x, win_y;
    int first_x, first_y;
    int final_x, final_y;
    unsigned int mask;
    Bool done, cancel;
    static GC crossGC = (GC) NULL;
    static Cursor crossCursor;
    static Cursor reversecrossCursor;
    XEvent event;
    static GC    gc;
    static int    first_time = True;
    static int    box_drawn = False;


    crossCursor = XCreateFontCursor (display, XC_cross);
    reversecrossCursor = XCreateFontCursor (display, XC_cross_reverse);

    if (first_time == True) {

#ifdef DEBUG
        gc = XCreateGC(display, rootWindow, CommonGetColor("black"), 0, NULL);
        XSetSubwindowMode(display, gc, IncludeInferiors);
        XSetForeground(display, gc, CommonGetColor("black"));
        XSetFunction(display, gc, GXxor);
#endif

        if (UserData != NULL) {
            user_button = atoi(UserData);
            switch (user_button) {

                case 1:
                    press_button = Button1;
                    break;

                case 2:
                    press_button = Button2;
                    break;

                case 3:
                    press_button = Button3;
                    break;

                default:
                    press_button = Button1;
                    break;

            }
        }
        else
            press_button = Button1;

        first_time = False;

    }

    if (XGrabPointer (display, rootWindow, False, ButtonReleaseMask,
              GrabModeAsync, GrabModeAsync, rootWindow,
              crossCursor, CurrentTime) != GrabSuccess) {
        fprintf (stderr, "Unable to grab pointer\n");
        exit (1);
    }

    /* Wait for ButtonPress with press_button event giving the first point */
    done = False;
    while (!done) {

        XNextEvent(display, &event);
        switch (event.type) {

            case ButtonRelease:

                if (event.xbutton.button == press_button) {

                    first_x = event.xbutton.x_root;
                    first_y = event.xbutton.y_root;
                    XUngrabPointer (display, CurrentTime);
                    XFlush (display);
                    if (XGrabPointer(display, rootWindow, False, 
                                     ButtonReleaseMask, GrabModeAsync, 
                                     GrabModeAsync, rootWindow, 
                                     reversecrossCursor, CurrentTime) 
                                     != GrabSuccess) {
                        fprintf (stderr, "Unable to grab pointer :2\n");
                        exit (1);
                    }
                    done = True;
        
                }

                break;

            default:

                break;

        }

    }

#ifdef DEBUG
    /* We need pointer motion events also. */
    XChangeActivePointerGrab(display, PointerMotionHintMask |
                             ButtonMotionMask | ButtonPressMask |
                             ButtonReleaseMask | OwnerGrabButtonMask,
                             crossCursor, CurrentTime);

    XGrabServer(display);
#endif

    done = False;
    cancel = True;
    while (!done) {

        XNextEvent (display, &event);

        switch (event.type) {

            case ButtonRelease:

                if (event.xbutton.button != press_button)
                    continue;

#ifdef DEBUG
                if (box_drawn == True)
                    /* Undraw box */
                    draw_box(gc, box_x, box_y, box_width, box_height);

                XUngrabServer(display);
#endif
                XUngrabPointer (display, CurrentTime);
                XFlush (display);

                /* Get final pointer position */
                XQueryPointer(display, rootWindow, &root_window, 
                              &child_window, &final_x, &final_y, &win_x,
                              &win_y, &mask);
                XFlush(display);
#ifdef DEBUG
                box_drawn = False;

                while(XCheckMaskEvent(display, ButtonReleaseMask |
                                      ButtonPressMask, &event))
                    ;
#endif
                done = True;
                break;

#ifdef DEBUG
            case MotionNotify:

                if (box_drawn == True)
                    /* Undraw box */
                    draw_box(gc, box_x, box_y, box_width, box_height);
                /*
                 * Get rid of all MotionNotify events in queue, since
                 * otherwise the round-trip delays caused by
                 * XQueryPointer may cause a backlog of MotionNotify
                 * events, thus causing additional wasted XQueryPointer
                 * calls.
                 */
                while (XCheckTypedEvent(display, MotionNotify, &event))
                    ;

                /* Get current pointer position */
                XQueryPointer(display, rootWindow, &root_window,
                              &child_window, &root_x, &root_y,
                              &win_x, &win_y, &mask);
                box_width = abs(root_x - first_x);
                box_height = abs(root_y - first_y);
                box_x = (root_x < first_x) ? root_x : first_x;
                box_y = (root_y < first_y) ? root_y : first_y;
                draw_box(gc, box_x, box_y, box_width, box_height);
                box_drawn = True;
                break;
#endif

/**Debug**
            case KeyRelease:

                if (XKeycodeToKeysym(display, event.xkey.keycode, 0) ==
                    XK_Escape) {

                    done = True;
                    break;

                }

**/
            default:

                break;

        }

    }

/**Debug**
    if (cancel == True)
        return(False);
**/
    upper_leftX = (first_x < final_x) ? first_x : final_x;
    upper_leftY = (first_y < final_y) ? first_y : final_y;
    lower_rightX = (first_x < final_x) ? final_x : first_x;
    lower_rightY = (first_y < final_y) ? final_y : first_y;

    return(True);

}


#ifdef DEBUG
static void    draw_box(gc, x, y, width, height)
GC    gc;
int    x, y;
unsigned int    width, height;
{

    /* Set foreground pixel value */
    XSetForeground(display, gc, CommonGetColor("black"));

    /* Drawing on root window, through all windows */
    XSetSubwindowMode(display, gc, IncludeInferiors);

    /*
     * Logical function is XOR, so that double drawing erases box
     * on both color and monochrome screens.
     */
     XSetFunction(display, gc, GXxor);

     XDrawRectangle(display, rootWindow, gc, x, y, width, height);

}
#endif
