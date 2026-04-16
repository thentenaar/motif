/**
 * Motif
 *
 * Copyright (c) 2025-2026 Tim Hentenaar
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
#ifndef _XMOS_R_H
#define _XMOS_R_H

/**
 * This header has been shipped with X since X11R6.3, so it should
 * be there.
 */
#include <X11/Xos_r.h>
#if defined(__GNUC__) || defined(__clang__) || (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 202311L)
#warning "Xmos_r.h is deprecated, include <X11/Xos_r.h> instead"
#endif
#endif /* _XMOS_R_H */

