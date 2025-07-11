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
  message :
	"Enumerated Values Test 6:\n\n" &
	"The following is a Bulletin Board set up to test correct\n" &
	"usage of enumeration values. (Note: this explanation text\n" &
	"itself tests the following:\n" &
	"  - XmNeditMode = XmMULTI_LINE_EDIT\n" &
	"Bulletin Board characteristics:\n" &
	"  - XmNborderWidth = 1\n" &
	"  - XmNbackground =  sky_blue\n" &
	"  - XmNmarginWidth =  20\n" &
	"  - XmNmarginHeight =  20\n" &
	"  - XmNdefaultPosition =  false\n" &
	"  - XmNnoResize =  true\n" &
	"  - XmNResizePolicy = XmRESIZE_GROW\n" &
	"  - XmNshadowType = XmSHADOW_ETCHED_OUT\n" &
	"  - XmNshadowThickness = 10\n" &
	"Form charactereristics:\n" &
	"  - XmNx = 30\n" &
	"  - XmNy = 30\n" &
	"  - XmNwidth = 300\n" &
	"  - XmNheight = 150\n" &
	"  - XmNborderWidth = 2\n" &
	"  - XmNresizePolicy = XmRESIZE_GROW\n" &
	"  Form children:\n" &
	"    Toggle 1:\n" &
	"      - XmNleftAttachment = XmATTACH_FORM\n" &
	"      - XmNleftOffset = 10\n" &
	"      - XmNtopAttachment = XmATTACH_POSITION\n" &
	"      - XmNtopPosition = 10\n" &
	"      - XmNlabelString = Toggle 1 (with a longer name)\n" &
	"      - XmNshadowThickness = 2\n" &
	"      - XmNborderWidth = 1\n" &
	"    Toggle 2:\n" &
	"      - XmNbottomAttachment = XmATTACH_OPPOSITE_FORM\n" &
	"      - XmNbottomOffset = -90\n" &
	"      - XmNrightAttachment = XmATTACH_OPPOSITE_WIDGET\n" &
	"      - XmNrightWidget = XmToggleButton ToggleButtonF1\n" &
	"      - XmNindicatorType = XmN_OF_MANY\n" &
	"      - XmNlabelString = Toggle 2 (shorter name)\n" &
	"      - XmNshadowThickness = 2\n" &
	"      - XmNborderWidth = 1\n" &
	"    Toggle 3:\n" &
	"      - XmNtopAttachment = XmATTACH_WIDGET\n" &
	"      - XmNtopWidget = XmToggleButton ToggleButtonF2\n" &
	"      - XmNtopOffset = 10\n" &
	"      - XmNleftAttachment = XmATTACH_WIDGET\n" &
	"      - XmNleftWidget = XmToggleButton ToggleButtonF2\n" &
	"      - XmNbottomAttachment = XmATTACH_FORM\n" &
	"      - XmNbottomOffset = 10\n" &
	"      - XmNindicatorType = XmONE_OF_MANY\n" &
	"      - XmNlabelString = Toggle 3\n" &
	"      - XmNshadowThickness = 2\n" &
	"      - XmNborderWidth = 1";
