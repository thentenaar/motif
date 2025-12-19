/**
 * Motif
 *
 * Copyright (c) 2025 Tim Hentenaar
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

#ifndef SUITES_H
#define SUITES_H

#include <X11/Intrinsic.h>
#include <check.h>

extern XtAppContext app;

/* Xt fixture */
Widget init_xt(const char *klass);
void uninit_xt(void);

/* Suites */
void jpeg_suite(SRunner *runner);
void png_suite(SRunner *runner);
void svg_suite(SRunner *runner);
void xmdesktopobject_suite(SRunner *runner);
void xmfontlistentry_suite(SRunner *runner);
void xmfontlist_suite(SRunner *runner);
void xmscreen_suite(SRunner *runner);

#endif /* SUITES_H */
