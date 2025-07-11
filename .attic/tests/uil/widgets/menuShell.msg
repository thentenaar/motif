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
        message : "This is a RowColumn inside a MenuShell with\nthe following characteristics:\n\n" &
		  "XmMenuShell resources:\n\n" &
	          "   - XmNwidth is 5\n   - XmNheight is 5\n" &
		  "   - XmNallowShellResize is true\n" &
		  "   - XmNoverrideRedirect is true\n\n" &
		  "Other Widget resources:\n\n" &
	 	  "   - RowColumn consists of four pushbuttons.\n" &
		  "   - menuAccelerator is <Ctrl>P\n" &
		  "   - fontList is variable\n" &
		  "   - activateCallback is associated with \n     the pushbuttons\n\n" &
		  "The activation of a pushbutton will change\n" &
		  "the color of the next pushButton to yellow\n" &
		  "This is done by passing the widget ID in\n" &
		  "the callback. Activating the last pushButton\n" &
		  "will change the color of the first pushButton\n\n" &
		  "You should be able to traverse the popup\nmenu via the keyboard.\n\n" &
		  "All other resources are set by default\n";

