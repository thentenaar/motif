/**
 * Motif
 *
 * Copyright (c) 2026 Tim Hentenaar
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
#ifndef MWM_NORETURN_H
#define MWM_NORETURN_H

/* The many facets of noreturn... */
#if __has_attribute(noreturn) \
    || (defined(__GNUC__) && ((__GNUC__ * 100 + __GNUC_MINOR__) >= 205)) \
    || (defined(__SUNPRO_C) && (__SUNPRO_C >= 0x590))
#define noreturn __attribute((noreturn))
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 202311L
#define noreturn [[noreturn]]
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
#include <stdnoreturn.h>
#else
#define noreturn
#endif

#endif /* MWM_NORETURN_H */
