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

/**
 * Macros for marking functions as deprecated, and suggesting
 * alternatives
 */
#ifndef XM_DEPRECATED_H
#define XM_DEPRECATED_H

/**
 * Try for C23's [[deprecated]], falling back to good ol' __attribute__,
 * assuming we're building with GCC 3.x or newer.
 */
#if (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 202311L) || (defined(__cplusplus) && __cplusplus >= 201402L)
#define XM_DEPRECATED       [[deprecated]]
#define XM_ALTERNATIVE(...) [[deprecated(#__VA_ARGS__)]]
#elif ((defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 5))) || defined(__clang__)) && defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#define XM_DEPRECATED       __attribute__((deprecated))
#define XM_ALTERNATIVE(...) __attribute__((deprecated(#__VA_ARGS__)))
#elif defined(__GNUC__) || defined(__clang__)
#define XM_DEPRECATED       __attribute__((deprecated))
#define XM_ALTERNATIVE(x)   __attribute__((deprecated))
#else
#define XM_DEPRECATED
#define XM_ALTERNATIVE(x)
#endif

#endif /* XM_DEPRECATED_H */
