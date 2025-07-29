/**
 * Motif
 *
 * Copyright (c) 2025 Tim Hentenaar.
 * Copyright (c) 1987 - 2012 The Open Group.
 * Licensed under the LGPL 2.1 license.
 */

#ifndef _XmSVGI_H
#define _XmSVGI_H

#include <stdio.h>
#include <X11/Xlib.h>

#define XImageIsSVG(x) ((x)->obdata && (x)->f.sub_image && !(x)->f.create_image)

extern int _XmSvgGetImage(FILE *fp, XImage **out);

#endif /* _XmSVGI_H */
