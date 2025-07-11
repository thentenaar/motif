%{
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

#ifndef X_NOT_STDC_ENV
#include <stdlib.h>			/* For malloc prototype. */
#endif
#include <string.h>			/* For prototypes. */

#include "AutoToken.h"

#define		MAX_KEY_SIZE 512
int  pos, i, count;
char *yyscaleval;
extern char     *yystringval;           /* contains all string data */
extern char     *yyinputstring;         /* contains input string data */
extern int      yydigitval;             /* count info for Key and Button */
int	find_token;	/* Return value from AutoFindToken */

%}

DigitOrLetterOrUnder		[\-\~\/A-Za-z][\-\~\/a-zA-Z0-9$_]*
KeyWord				[A-Z][a-zA-Z0-9$]+
StringData			[ \-\~\/\\,\'\.\?\<\>\*\&\!\@\#\$\%\^\(\)A-Za-z0-9$_][ \-\~\/\\,\'\.\?\<\>\*\&\!\@\#\$\%\^\(\)a-zA-Z0-9$_]*
Digit				[0-9]


%%

[ \t\f\n]			{}
^#.*$				{}

\-?\.?{Digit}*			{ 
				/* Remove decimal point from value. Reasoning
				   is that Scale does not use floating point
				   values, rather the integer representation
				   of the float. */
				yyscaleval = (char *) malloc(sizeof(char) *
						            (yyleng + 1));
				if ((pos = strcspn((char *)yytext, ".")) != yyleng) {
				   count = 0;
				   for (i = 0 ; i < yyleng; i++)
					if (i != pos)
						yyscaleval[count++] = yytext[i];
				}
				else
					strcpy(yyscaleval, (char *)yytext);
				yydigitval = (int) atoi(yyscaleval);
				if (yyscaleval)
					free(yyscaleval);
				return(DIGIT); 
				}

\"{StringData}\"		{
				yyinputstring = (char *) malloc(sizeof(char) *
						               (yyleng + 1));
				strcpy(yyinputstring, (char *) yytext);
				return(ISTRING);
				}

\<{KeyWord}\>			|
{KeyWord}			{
				find_token = AutoFindToken((char*) yytext);
				if ( find_token == -1 ) {
					yystringval = (char *) malloc(
					sizeof(char) * (yyleng + 1));
					strcpy(yystringval, (char *) yytext);
					return(STRING);
				} else {
					return(find_token);
				}
				}

{DigitOrLetterOrUnder}		{
				yystringval = (char *) malloc(sizeof(char) * 
						             (yyleng + 1));
				strcpy(yystringval, (char *) yytext);
				return(STRING);
				}
%%
