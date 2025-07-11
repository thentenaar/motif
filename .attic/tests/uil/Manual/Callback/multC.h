/* 
 * Motif
 *
 * Copyright (c) 1987-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
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
/*   $XConsortium: multC.h /main/4 1995/07/13 20:31:01 drk $ */

#include <stdio.h>

#include <Mrm/MrmAppl.h>

#define CALLBACK1_1	1
#define CALLBACK2_1	1<<1
#define CALLBACK2_2	1<<2
#define CALLBACK2_3	1<<3
#define CALLBACK3_1	1<<4
#define CALLBACK3_2	1<<5
#define CALLBACK4_1	1<<6
#define CALLBACK4_2	1<<7

#define CALLBACK5_1	1
#define CALLBACK5_2	1<<1
#define CALLBACK5_3	1<<2
#define CALLBACK6_1	1<<3
#define CALLBACK6_2	1<<4
#define CALLBACK7_1	1<<5

#define CALLBACK8_1	1
#define CALLBACK8_2	1<<1
#define CALLBACK8_3	1<<2
#define CALLBACK8_4	1<<3
#define CALLBACK8_5	1<<4
#define CALLBACK9_1	1<<5
#define CALLBACK9_2	1<<6
#define CALLBACK9_3	1<<7
#define CALLBACK9_4	1<<8
#define CALLBACK10_1	1<<9
#define CALLBACK10_2	1<<10
#define CALLBACK10_3	1<<11

#define CALLBACK11_1_1	1
#define CALLBACK11_1_2	1<<1
#define CALLBACK11_2	1<<2
#define CALLBACK12_1_1	1<<3
#define CALLBACK12_1_2	1<<4
#define CALLBACK12_2	1<<5

/*
 * Name of uid file
 */

String		filename_vec[] =
		{
		    "multC.uid"
		};

MrmCount	filename_num =
		(sizeof(filename_vec) / sizeof(filename_vec[0]));

/*
 * Convenience routine
 */

void		error_proc();

/*
 * Callback routines
 */

void		callback1_1();
void		callback2_1();
void		callback2_2();
void		callback2_3();
void		callback3_1();
void		callback3_2();
void		callback4_1();
void		callback4_2();
void		callback5_1();
void		callback5_2();
void		callback5_3();
void		callback6_1();
void		callback6_2();
void		callback7_1();
void		callback8_1();
void		callback8_2();
void		callback8_3();
void		callback8_4();
void		callback8_5();
void		callback9_1();
void		callback9_2();
void		callback9_3();
void		callback9_4();
void		callback10_1();
void		callback10_2();
void		callback10_3();
void		callback11_1();
void		callback11_2();
void		callback12_1();
void		callback12_2();

/*
 * Names to register with MrmRegisterNames
 */

static MrmRegisterArg	reglist[] = 
			{
			    "callback1_1",	(caddr_t) callback1_1,
			    "callback2_1",	(caddr_t) callback2_1,
			    "callback2_2",	(caddr_t) callback2_2,
			    "callback2_3",	(caddr_t) callback2_3,
			    "callback3_1",	(caddr_t) callback3_1,
			    "callback3_2",	(caddr_t) callback3_2,
			    "callback4_1",	(caddr_t) callback4_1,
			    "callback4_2",	(caddr_t) callback4_2,
			    "callback5_1",	(caddr_t) callback5_1,
			    "callback5_2",	(caddr_t) callback5_2,
			    "callback5_3",	(caddr_t) callback5_3,
			    "callback6_1",	(caddr_t) callback6_1,
			    "callback6_2",	(caddr_t) callback6_2,
			    "callback7_1",	(caddr_t) callback7_1,
			    "callback8_1",	(caddr_t) callback8_1,
			    "callback8_2",	(caddr_t) callback8_2,
			    "callback8_3",	(caddr_t) callback8_3,
			    "callback8_4",	(caddr_t) callback8_4,
			    "callback8_5",	(caddr_t) callback8_5,
			    "callback9_1",	(caddr_t) callback9_1,
			    "callback9_2",	(caddr_t) callback9_2,
			    "callback9_3",	(caddr_t) callback9_3,
			    "callback9_4",	(caddr_t) callback9_4,
			    "callback10_1",	(caddr_t) callback10_1,
			    "callback10_2",	(caddr_t) callback10_2,
			    "callback10_3",	(caddr_t) callback10_3,
			    "callback11_1",	(caddr_t) callback11_1,
			    "callback11_2",	(caddr_t) callback11_2,
			    "callback12_1",	(caddr_t) callback12_1,
			    "callback12_2",	(caddr_t) callback12_2,
			};

static int		reglist_num = (sizeof(reglist) / sizeof(reglist[0]));

/*
 * Data for summary
 */

#define NUM_TESTS_PART1		5
#define PART1_TEST_NAME_LENGTH	58

String	part1_test_names[NUM_TESTS_PART1] = 
	{
	    "One inline callback procedure assigned to one reason      ",
	    "Multiple inline callback procedures assigned to one reason",
	    "Predefined procedures list assigned to one reason         ",
	    "Predefined nested procedures lists assigned to one reason ",
            "Inline nested procedures lists assigned to one reason     ",
	};

#define NUM_TESTS_PART2	3	
#define PART2_TEST_NAME_LENGTH	33

String	part2_test_names[NUM_TESTS_PART2] = 
	{
	    "Multiple inline callbacks        ",
	    "Predefined callbacks list        ",
	    "Predefined nested callbacks lists",
	};

#define NUM_TESTS_PART3		3
#define PART3_TEST_NAME_LENGTH	63

String	part3_test_names[NUM_TESTS_PART3] = 
	{
	    "Multiple inline callbacks with multiply defined reasons        ",
	    "Predefined callbacks list with multiply defined reasons        ",
	    "Predefined nested callbacks lists with multiply defined reasons",
	};

#define NUM_TESTS_PART4		2
#define PART4_TEST_NAME_LENGTH	63

String	part4_test_names[NUM_TESTS_PART2] = 
	{
	    "Single callback procedure assigned to multiple reasons         ",
	    "Single callback procedure assigned multiple times to one reason",
	};

/*
 * Local declarations
 */

MrmHierarchy	s_MrmHierarchy;
MrmType		class;

Display		*dsp;
Colormap	cmap;

Widget		toplevel,
		part1_widget[NUM_TESTS_PART1],
		part2_widget[NUM_TESTS_PART2],
		part3_widget[NUM_TESTS_PART3],
		part4_widget[NUM_TESTS_PART4];

String		part1_widget_names[NUM_TESTS_PART1] =
		{
		    "widget1",
		    "widget2",
		    "widget3",
		    "widget4",
		    "widget4a",
		};

String		part2_widget_names[NUM_TESTS_PART2] =
		{
		    "widget5",
		    "widget6",
		    "widget7",
		};

String		part3_widget_names[NUM_TESTS_PART3] =
		{
		    "widget8",
		    "widget9",
		    "widget10",
		};

String		part4_widget_names[NUM_TESTS_PART4] =
		{
		    "widget11",
		    "widget12",
		};

int		part1_callbacks_called[NUM_TESTS_PART1] =
		{
		    0, 0, 0, 0, 0,
		};

int		part2_callbacks_called[NUM_TESTS_PART2] =
		{
		    0, 0, 0,
		};

int		part3_callbacks_called[NUM_TESTS_PART3] =
		{
		    0, 0, 0,
		};

int		part4_callbacks_called[NUM_TESTS_PART4] =
		{
		    0, 0,
		};

int		part1_callbacks_expected[NUM_TESTS_PART1] =
		{
		    CALLBACK1_1,
		    CALLBACK2_1 | CALLBACK2_2 | CALLBACK2_3,
		    CALLBACK3_1 | CALLBACK3_2,
		    CALLBACK3_1 | CALLBACK3_2 | CALLBACK4_1 | CALLBACK4_2,
		    CALLBACK4_1 | CALLBACK3_1 | CALLBACK3_2 | CALLBACK4_2,
		};

int		part2_callbacks_expected[NUM_TESTS_PART2] =
		{
		    CALLBACK5_1 | CALLBACK5_2 | CALLBACK5_3,
		    CALLBACK6_1 | CALLBACK6_2,
		    CALLBACK6_1 | CALLBACK6_2 | CALLBACK7_1,
		};

int		part3_callbacks_expected[NUM_TESTS_PART3] =
		{
		    CALLBACK8_5 | CALLBACK8_4 | CALLBACK8_3,
		    CALLBACK9_3 | CALLBACK9_2 | CALLBACK9_4,
		    CALLBACK10_1 | CALLBACK9_2 | CALLBACK10_3,
		};

int		part4_callbacks_expected[NUM_TESTS_PART4] =
		{
		    CALLBACK11_1_1 | CALLBACK11_1_2 | CALLBACK11_2,
		    CALLBACK12_1_1 | CALLBACK12_2 | CALLBACK12_1_2,
		};

int		part1_order_actual[NUM_TESTS_PART1] =
		{
		    0, 0, 0, 0, 0,
		};
    
int		part1_order_expected[NUM_TESTS_PART1] =
		{
		    1, 3, 2, 4, 1,
		};
    
int		callbacks_called = 0,
		order_preserved = 0;

char		result_buffer[480],
		mrm_err[80];
