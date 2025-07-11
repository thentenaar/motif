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
static char rcsid[] = "$XConsortium: WorkingDia1.c /main/9 1995/07/13 19:03:51 drk $"
#endif
#endif

/*  include files  */
#include <testlib.h>
#include <Xm/XmP.h>

static Pixmap CreatePixmap ();
static Pixmap hourPix = (Pixmap)0;
static char hourBits[] = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xff, 0xff, 0x07,
   0x40, 0x00, 0x00, 0x02, 0x80, 0x00, 0x00, 0x01, 0x00, 0x03, 0xc0, 0x00,
   0x00, 0x0e, 0x70, 0x00, 0x00, 0xfc, 0x3f, 0x00, 0x00, 0xf8, 0x1f, 0x00,
   0x00, 0xf0, 0x0f, 0x00, 0x00, 0xe0, 0x0f, 0x00, 0x00, 0xe0, 0x0f, 0x00,
   0x00, 0xe0, 0x0f, 0x00, 0x00, 0xe0, 0x0f, 0x00, 0x00, 0xe0, 0x0f, 0x00,
   0x00, 0xf0, 0x0f, 0x00, 0x00, 0xe8, 0x1f, 0x00, 0x00, 0xe4, 0x2f, 0x00,
   0x00, 0xf2, 0x5f, 0x00, 0x00, 0xf9, 0x9f, 0x00, 0x80, 0xfd, 0x3f, 0x01,
   0xc0, 0xff, 0xff, 0x03, 0xe0, 0xff, 0xff, 0x07, 0xe0, 0xff, 0xff, 0x07,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

Widget mbox;                        /*  MessageBox widget   */

static int destroy = False;
static  XmString str1 = NULL;
static  XmString str2 = NULL;
static  XmString str3 = NULL;
static  XmString str4 = NULL;
static  XmString str5 = NULL;
static  XmString str6 = NULL;


/* callbacks */
static void OkCB();
static XtCallbackRec ok_cb[] = {
    { OkCB, NULL},
    { NULL, NULL}
};
static void CancelCB();
static XtCallbackRec cancel_cb[] = {
    { CancelCB, NULL},
    { NULL, NULL}
};
static void HelpCB();
static XtCallbackRec help_cb[] = {
    { HelpCB, NULL},
    { NULL, NULL}
};


/*#### OkCB ####*/
static void OkCB (w, client_data, call_data) 
Widget      w;                           /*  widget id           */
XtPointer     client_data;                 /*  data from application   */
XtPointer     call_data;                   /*  data from widget class  */
{
    Arg          args[10];
    register int n;
    Widget   child=NULL;

    child = XmMessageBoxGetChild (w, XmDIALOG_HELP_BUTTON);
    if (child) XtDestroyWidget (child);

    n=0;
    XtSetArg (args[n], XmNhelpLabelString, str1); n++;
    XtSetValues (w, args, n);
}


/*#### CancelCB ####*/
static void CancelCB (w, client_data, call_data) 
Widget      w;                           /*  widget id           */
XtPointer     client_data;                 /*  data from application   */
XtPointer     call_data;                   /*  data from widget class  */
{
    Arg          args[10];
    register int n;
    Widget   child=NULL;

    child = XmMessageBoxGetChild (w, XmDIALOG_SYMBOL_LABEL);
    if (child) XtDestroyWidget (child);

    n=0;
    XtSetArg (args[n], XmNsymbolPixmap, hourPix); n++;
    XtSetValues (w, args, n);
}


/*#### HelpCB ####*/
static void HelpCB (w, client_data, call_data) 
Widget      w;                           /*  widget id           */
XtPointer     client_data;                 /*  data from application   */
XtPointer     call_data;                   /*  data from widget class  */
{
    Arg          args[10];
    register int n;
    Widget   child=NULL;

    child = XmMessageBoxGetChild (w, XmDIALOG_MESSAGE_LABEL);
    if (child) XtDestroyWidget (child);

    n=0;
    XtSetArg (args[n], XmNmessageString, str1); n++;
    XtSetValues (w, args, n);
}


/*#### main ####*/
void main (argc,argv)
unsigned int argc;
char **argv;
{
    Arg             args[MAX_ARGS];
    register int    n;            
    Widget	    Frame;


    /*  initialize toolkit  */
    CommonTestInit(argc, argv);
    
    n = 0;
    XtSetArg (args[n], XmNwidth, 10);                     n++;
    XtSetArg (args[n], XmNheight, 10);                    n++;
    XtSetValues(Shell1, args, n);

    n = 0;
    Frame = XmCreateFrame(Shell1, "Frame1", args, n);
    XtManageChild(Frame);
    

    /* create a pixmap */
    hourPix = CreatePixmap (XtScreen(Shell1), hourBits, 32, 32,
                            BlackPixelOfScreen(XtScreen(Shell1)),
                            WhitePixelOfScreen(XtScreen(Shell1)));

    /* create compound string */
    str1 = XmStringCreate ("HelpBtn", XmSTRING_DEFAULT_CHARSET);
    str2 = XmStringCreate ("Symbol", XmSTRING_DEFAULT_CHARSET);
    str3 = XmStringCreate ("Message", XmSTRING_DEFAULT_CHARSET);
    str4 = XmStringCreate ("destroy & set resource", XmSTRING_DEFAULT_CHARSET);

    n = 0;
    XtSetArg (args[n], XmNokCallback, ok_cb);                         n++;
    XtSetArg (args[n], XmNcancelCallback, cancel_cb);                 n++;
    XtSetArg (args[n], XmNhelpCallback, help_cb);                     n++;
    XtSetArg (args[n], XmNmessageString, str4);                       n++;
    XtSetArg (args[n], XmNokLabelString, str1);                       n++;
    XtSetArg (args[n], XmNcancelLabelString, str2);                   n++;
    XtSetArg (args[n], XmNhelpLabelString, str3);                     n++;
    XtSetArg (args[n], XmNautoUnmanage, False);                       n++;

    mbox = XmCreateWorkingDialog(Frame, "mbox", args, n);

    XtRealizeWidget (Shell1);
    XtManageChild (mbox);

    CommonPause();

    /*  process events  */
    XtAppMainLoop(app_context);
}


/***********************************************************************
 *
 *  CreatePixmap - This routine will return a pixmap from the output
 *         of the bitmap editor.  The output pixmap will be 
 *         the depth of the screen.  This is so that label can
 *         write it using its current foreground and background.
 *
 ***********************************************************************/

static Pixmap CreatePixmap (screen, bitmap_bits, bitmap_height, bitmap_width,
                foreground_pixel, background_pixel)
Screen *screen;
unsigned short *bitmap_bits;
int     bitmap_height, bitmap_width;
Pixel   foreground_pixel, background_pixel;
{
    register Display *display = DisplayOfScreen (screen);
    XImage image;
    Pixmap pixmap;
    GC gc;
    XGCValues gcValues;

    image.height       = bitmap_height;     /* dimensions */
    image.width        = bitmap_width;

    image.format       = XYBitmap;       /* assumed input is bit-map */
    image.depth        = 1;          /* hence, depth of 1    */

    image.data         = (char*) bitmap_bits;   /* where the bits are */

    image.xoffset      = 0;         /* leading pixels to skip ?  */
    image.byte_order       = LSBFirst;      /* magic numbers, describing */
    image.bitmap_pad       = 8;         /* data format of bit map    */
    image.bitmap_bit_order = LSBFirst;
    image.bitmap_unit      = 8;
    image.bytes_per_line   = (bitmap_width + 7) / 8;

    image.obdata       = NULL;

    /* get a chunk of off-screen, display on root screen */
    /* 1 bit deep, this is still just bit data, not color pixel data */
    pixmap = XCreatePixmap (display, RootWindowOfScreen (screen),
                image.width, image.height, 
                (unsigned) DefaultDepthOfScreen(screen));
    if (!pixmap)
    return (pixmap);

    /* When putting a XYPixmap type image, the foreground and */
    /* and background of the gc should not be used, but I'll  */
    /* leave this in here anyway.                 */

    gcValues.foreground = foreground_pixel;
    gcValues.background = background_pixel;

    /* build a gc to use when drawing into the pixmap */
    gc = XCreateGC (display, pixmap, GCForeground | GCBackground, &gcValues);

    if (! gc)
    return ((Pixmap) NULL);

    /* put bits into the pixmap */
    XPutImage (display, pixmap, gc, &image, 0, 0, 
           0, 0, image.width, image.height);

    XFreeGC (display, gc);

    return (pixmap);
}
