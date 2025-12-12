/*
 * Motif
 *
 * Copyright (c) 1987-2012, The Open Group. All rights reserved.
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
/*
 * HISTORY
*/
#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$XConsortium: CntrbmI.h /main/5 1995/07/14 10:15:34 drk $"
#endif
#endif

#ifndef	_XmCntrbmP_h
#define	_XmCntrbmP_h

#ifdef __cplusplus
extern "C" {
#endif

/***************************/
/* Default collapsedPixmap */
/***************************/

#define xm_collapsed16_width 16
#define xm_collapsed16_height 16
static char xm_collapsed16_bits[] = {
   0xff, 0xff, 0xff, 0xff, 0xfe, 0x7f, 0xfe, 0x7f, 0xfc, 0x3f, 0xfc, 0x3f,
   0xf8, 0x1f, 0xf8, 0x1f, 0xf0, 0x0f, 0xf0, 0x0f, 0xe0, 0x07, 0xe0, 0x07,
   0xc0, 0x03, 0xc0, 0x03, 0x80, 0x01, 0x80, 0x01};

/**************************/
/* Default expandedPixmap */
/**************************/

#define xm_expanded16_width 16
#define xm_expanded16_height 16
static char xm_expanded16_bits[] = {
   0x80, 0x01, 0x80, 0x01, 0xc0, 0x03, 0xc0, 0x03, 0xe0, 0x07, 0xe0, 0x07,
   0xf0, 0x0f, 0xf0, 0x0f, 0xf8, 0x1f, 0xf8, 0x1f, 0xfc, 0x3f, 0xfc, 0x3f,
   0xfe, 0x7f, 0xfe, 0x7f, 0xff, 0xff, 0xff, 0xff};

#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmCntrbmP_h */
