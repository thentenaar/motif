/**
 * Motif
 *
 * Copyright (c) 2025 Tim Hentenaar
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
#ifndef _XpmP_h
#define _XpmP_h

#include <X11/xpm.h>

#if defined(__GNUC__) || defined(__clang__) || (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 202311L)
#warning "XpmP.h is deprecated, include <X11/xpm.h> instead"
#endif

/* Xpm external symbols were previously prefixed with Xme */

#define XmeXpmCreatePixmapFromData XpmCreatePixmapFromData
#define XmeXpmCreateDataFromPixmap XpmCreateDataFromPixmap
#define XmeXpmReadFileToPixmap XpmReadFileToPixmap
#define XmeXpmWriteFileFromPixmap XpmWriteFileFromPixmap
#define XmeXpmCreateImageFromData XpmCreateImageFromData
#define XmeXpmCreateDataFromImage XpmCreateDataFromImage
#define XmeXpmReadFileToImage XpmReadFileToImage
#define XmeXpmWriteFileFromImage XpmWriteFileFromImage
#define XmeXpmCreateImageFromBuffer XpmCreateImageFromBuffer
#define XmeXpmCreatePixmapFromBuffer XpmCreatePixmapFromBuffer
#define XmeXpmCreateBufferFromImage XpmCreateBufferFromImage
#define XmeXpmCreateBufferFromPixmap XpmCreateBufferFromPixmap
#define XmeXpmReadFileToBuffer XpmReadFileToBuffer
#define XmeXpmWriteFileFromBuffer XpmWriteFileFromBuffer
#define XmeXpmReadFileToData XpmReadFileToData
#define XmeXpmWriteFileFromData XpmWriteFileFromData
#define XmeXpmAttributesSize XpmAttributesSize
#define XmeXpmFreeAttributes XpmFreeAttributes
#define XmeXpmFreeExtensions XpmFreeExtensions
#define XmeXpmFreeXpmImage XpmFreeXpmImage
#define XmeXpmFreeXpmInfo XpmFreeXpmInfo
#define XmeXpmGetErrorString XpmGetErrorString
#define XmeXpmLibraryVersion XpmLibraryVersion
#define XmeXpmReadFileToXpmImage XpmReadFileToXpmImage
#define XmeXpmWriteFileFromXpmImage XpmWriteFileFromXpmImage
#define XmeXpmCreatePixmapFromXpmImage XpmCreatePixmapFromXpmImage
#define XmeXpmCreateImageFromXpmImage XpmCreateImageFromXpmImage
#define XmeXpmCreateXpmImageFromImage XpmCreateXpmImageFromImage
#define XmeXpmCreateXpmImageFromPixmap XpmCreateXpmImageFromPixmap
#define XmeXpmCreateDataFromXpmImage XpmCreateDataFromXpmImage
#define XmeXpmCreateXpmImageFromData XpmCreateXpmImageFromData
#define XmeXpmCreateXpmImageFromBuffer XpmCreateXpmImageFromBuffer
#define XmeXpmCreateBufferFromXpmImage XpmCreateBufferFromXpmImage
#define XmeXpmFree XpmFree
#endif /* _XpmP_h */
