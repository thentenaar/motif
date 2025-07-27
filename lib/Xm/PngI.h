#ifndef _XmPngI_h
#define _XmPngI_h

#include <stdio.h>
#include <X11/Xlib.h>

int _XmPngGetImage(Screen * screen, FILE * infile, XColor *xc, XImage **ximage);

#endif
