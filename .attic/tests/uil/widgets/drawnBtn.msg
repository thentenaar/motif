! 
! Motif
!
! Copyright (c) 1987-2012, The Open Group. All rights reserved.
!
! These libraries and programs are free software; you can
! redistribute them and/or modify them under the terms of the GNU
! Lesser General Public License as published by the Free Software
! Foundation; either version 2 of the License, or (at your option)
! any later version.
!
! These libraries and programs are distributed in the hope that
! they will be useful, but WITHOUT ANY WARRANTY; without even the
! implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
! PURPOSE. See the GNU Lesser General Public License for more
! details.
!
! You should have received a copy of the GNU Lesser General Public
! License along with these librararies and programs; if not, write
! to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
! Floor, Boston, MA 02110-1301 USA
! 
!
 value
        message : "This is a Drawn Button Widget with the\nfollowing characteristics:\n\n   - X and Y " &
	"values are 0\n   - width is 118\n   - height is 118\n   - borderWidth is 4\n" &
	"   - backgroundPixmap is the\n     motif icon\n" &
	"   - highlightThickness is 1\n   - marginWidth and marginHeight are 20\n" &
	"   - pushButtonEnabled is true\n\n\nAll other resources are set by default.\n";

value

	black:	color('black');
	white:	color('white');
	
motif_colors : color_table(black = 'x',white=' ');
motif_icon : icon(color_table = motif_colors,
'                                                        ',
'                                                        ',
'                                                        ',
'                                                        ',
'                                                        ',
'                                                        ',
'       x x x x x x x x x x x     x x x x x x x x x x x  ',
'        x x x x x x x x x x       x x x x x x x x x x   ',
'      x                      x  x                      x',
'       x xxxxxxxxxxxxxxxxxx xx   x x x x x x x x x x  xx',
'      x  xxxxxxxxxxxxxxxxxx xx  x   x x x x x x x x x xx',
'       x xxxxxxxxxxxxxxxxxx xx   x x x x x x x x x x  xx',
'      x  xxxxxxxxxxxxxxxxxx xx  x   x x x x x x x x x xx',
'       x xxxxxxxxxxxxxxxxxx xx   x x x x x x x x x x  xx',
'      x  xxxxxxxxxxxxxxxxxx xx  x   x x x x x x x x x xx',
'       x xxxxxxxxxxxxxxxxxx xx   x x x x x x x x x x  xx',
'      x  xxxxxxxxxxxxxxxxxx xx  x   x x x x x x x x x xx',
'       x xxxxxxxxxxxxxxxxxx xx   x x x x x x x x x x  xx',
'      x  xxxxxxxxxxxxxxxxxx xx  x   x x x x x x x x x xx',
'       x xxxxxxxxxxxxxxxxxx xx   x x x x x x x x x x  xx',
'      x  xxxxxxxxxxxxxxxxxx xx  x   x x x x x x x x x xx',
'       x xxxxxxxxxxxxxxxxxx xx   x x x x x x x x x x  xx',
'      x  xxxxxxxxxxxxxxxxxx xx  x   x x x x x x x x x xx',
'       x xxxxxxxxxxxxxxxxxx xx   x x x x x x x x x x  xx',
'      x  xxxxxxxxxxxxxxxxxx xx  x   x x x x x x x x x xx',
'       x xxxxxxxxxxxxxxxxxx xx   x x x x x x x x x x  xx',
'      x  xxxxxxxxxxxxxxxxxx xx  x   x x x x x x x x x xx',
'       x                     x   x                     x',
'      x  xxxxxxxxxxxxxxxxxx     x  xxxxxxxxxxxxxxxxxx   ',
'        xxxxxxxxxxxxxxxxxxxx      xxxxxxxxxxxxxxxxxxxx  ',
'                                                        ',
'                                                        ',
'       x x x x x x x x x x x     x x x x x x x x x x x  ',
'        x x x x x x x x x x       x x x x x x x x x x   ',
'      x                      x  x                      x',
'       x x x x x x x x x x  xx   x xxxxxxxxxxxxxxxxxx xx',
'      x   x x x x x x x x x xx  x  xxxxxxxxxxxxxxxxxx xx',
'       x x x x x x x x x x  xx   x xxxxxxxxxxxxxxxxxx xx',
'      x   x x x x x x x x x xx  x  xxxxxxxxxxxxxxxxxx xx',
'       x x x x x x x x x x  xx   x xxxxxxxxxxxxxxxxxx xx',
'      x   x x x x x x x x x xx  x  xxxxxxxxxxxxxxxxxx xx',
'       x x x x x x x x x x  xx   x xxxxxxxxxxxxxxxxxx xx',
'      x   x x x x x x x x x xx  x  xxxxxxxxxxxxxxxxxx xx',
'       x x x x x x x x x x  xx   x xxxxxxxxxxxxxxxxxx xx',
'      x   x x x x x x x x x xx  x  xxxxxxxxxxxxxxxxxx xx',
'       x x x x x x x x x x  xx   x xxxxxxxxxxxxxxxxxx xx',
'      x   x x x x x x x x x xx  x  xxxxxxxxxxxxxxxxxx xx',
'       x x x x x x x x x x  xx   x xxxxxxxxxxxxxxxxxx xx',
'      x   x x x x x x x x x xx  x  xxxxxxxxxxxxxxxxxx xx',
'       x x x x x x x x x x  xx   x xxxxxxxxxxxxxxxxxx xx',
'      x   x x x x x x x x x xx  x  xxxxxxxxxxxxxxxxxx xx',
'       x x x x x x x x x x  xx   x xxxxxxxxxxxxxxxxxx xx',
'      x   x x x x x x x x x xx  x  xxxxxxxxxxxxxxxxxx xx',
'       x                     x   x                     x',
'      x  xxxxxxxxxxxxxxxxxx     x  xxxxxxxxxxxxxxxxxx   ',
'        xxxxxxxxxxxxxxxxxxxx      xxxxxxxxxxxxxxxxxxxx  ',
'                                                        ');
