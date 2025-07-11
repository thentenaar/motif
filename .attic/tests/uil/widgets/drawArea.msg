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
        message : "This is a Drawing Area widget with the\nfollowing characteristics:\n\n   - width is" &
	" 95\n   - height is 95\n   - backgroundPixmap is of a\n     painter's palette\n" &
	"   - exposeCallback is ON\n   - inputCallback is ON\n" &
	"   - marginWidth is 20\n   - mar" &
	"ginHeight is 20\n\nAll other resources are set be default.\n";

value black  : color('black',foreground);
value white  : color('white',background);
value green  : color('green',background);
value blue   : color('blue',background);
value yellow : color('yellow',background);
value orange : color('orange',background);
value violet : color('violet',background);
value red    : color('red',background);

value mre_colors : color_table(black = ' ',
	white='x',
	red='r',
	green='g',
	blue='b',
	yellow='y',
	orange='o',
	violet='v');

value

        palette: icon(color_table=mre_colors,
'                                ',
'                                ',
'           xxxxxxxxxx           ',
'         xx          xx         ',
'       xx              x        ',
'      x     rrrr        x       ',
'    xx      rrr        x        ',
'   x   b    r       xxx         ',
'   x  bbb         xx            ',
'  x   bbb        xx             ',
' x     b         x              ',
' x           xxx  x     xxxxxx  ',
'x  ggg      x   x  xxxxx      x ',
'x  gggg    x    x              x',
'x   g      x    x              x',
'x          xx  x       oooo    x',
'x           xxx        ooo     x',
'x  yyyy                 o      x',
'x  yyy                         x',
'x   y                          x',
' x                 xxxx       x ',
' x     ooo          xx        x ',
'  x    ooo    v      xx      x  ',
'   x         vvv            x   ',
'    x        vvv           x    ',
'     x       v            x     ',
'      x                  x      ',
'       xx              xx       ',
'         xx          xx         ',
'           xxxxxxxxxx           ',
'                                ',
'                                ');
