/**
 * Motif
 *
 * Copyright (c) 2025 Tim Hentenaar.
 * Copyright (c) 1987 - 2012 The Open Group.
 * Licensed under the LGPL 2.1 license.
 */

#ifndef _XmPngI_h
#define _XmPngI_h

#include <stdio.h>
#include <X11/Xlib.h>

int _XmPngGetImage(FILE *infile, XColor *bg, XImage **ximage);

#endif
