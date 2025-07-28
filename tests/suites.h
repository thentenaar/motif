/**
 * Motif
 *
 * Copyright (c) 2025 Tim Hentenaar.
 * Copyright (c) 1987 - 2012 The Open Group.
 * Licensed under the LGPL 2.1 license.
 */
#ifndef SUITES_H
#define SUITES_H

#include <X11/Intrinsic.h>
#include <check.h>

extern XtAppContext app;

/* Xt fixture */
Widget init_xt(const char *klass);
void uninit_xt(void);

/* Suites */
void xmfontlistentry_suite(SRunner *runner);
void xmfontlist_suite(SRunner *runner);
void png_suite(SRunner *runner);

#endif /* SUITES_H */
