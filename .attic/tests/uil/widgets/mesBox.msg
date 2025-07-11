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
! HISTORY
! 
!   $XConsortium: mesBox.msg /main/7 1995/07/13 20:48:53 drk $
!OSF/Motif: @(#)mesBox.msg	3.3  92/01/31
!******************************************************************************
 value
        message : "This is a Message Box with the \nfollowing characteristics:\n\n   - borderWidth is " &
	"4\n   - background color is forest green\n   - foreground color is white\n   - shadowThickness is 5\n" &
	"   - width and height are 400\n" &
	"   - messageAlignment is XmALIGNMENT_END\n   - okCallback is ON" &
	"\n   - helpCallback is ON\n   - cancelCallback is ON\n   - noResize is " &
	"true\n   - autoUnmanage is false\n" &
	"   - messageString is The Message Box\n\n" &
	"Automatically Created Widget Resources:\n\n" &
	"   - the OK button is unmanaged\n" &
	"   - the message label is overidden with\n" &
	"     Overrride(The MessageBox)\n" &
	"   - the symbol label's labelPixmap\n" &
	"     is the pallete icon, also has\n" &
	"     margin width and height of 75\n\n" &
	"All other resources are set by default.\n";


  value
	  wheat	 : color('wheat');
	  navy	 : color('navy');
	  black  : color('black',foreground);
	  white  : color('white',background);
	  green  : color('green',background);
	  blue   : color('blue',background);
	  yellow : color('yellow',background);
	  orange : color('orange',background);
	  violet : color('violet',background);
	  red    : color('red',background);
	
  value 
	colors_8 : color_table(black = ' ',
       			       white='x',
        		       red='r',
        		       green='g',
        		       blue='b',
        		       yellow='y',
       			       orange='o',
       			       violet='v');

   value

        palette: icon(color_table=colors_8,
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
