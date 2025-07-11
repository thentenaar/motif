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


%{
#include "stdio.h"
#include <Xm/Xm.h>
#include "toolchest.h"
%}

%union
{
    char *string;
    struct menuEntry *menuEntry;
}

%token <string>  STRING
%token <string>  MENU
%token <string>  INCLUDE
%token <string>  SINCLUDE
%token <string>  REMOVE
%token <string>  FROM
%token <string>  F_EXEC
%token <string>  F_CHECKEXEC
%token <string>  F_CHECKEXPR
%token <string>  F_SEPARATOR
%token <string>  F_LABEL
%token <string>  F_TITLE
%token <string>  F_MENU

%type <string> string _string
%type <menuEntry> menuitem menuspec

%%
specs	: /* empty */
	| specs spec newline
	| specs newline
	;

spec	: INCLUDE string
	    {
		IncludeFile($2, TRUE);
	    }
	| SINCLUDE string
	    {
		IncludeFile($2, FALSE);
	    }
	| MENU string newline '{' newline menuspec '}'
	    {
		if ($6 != NULL);
		{
		    register struct menu *menu;
		    
		    menu = FindMenu($2, TRUE);
		    /* if the menu already existed, add new items to end */
		    if (menu->firstMenuEntry)
		    {
			struct menuEntry *tlast;
			
			menu->lastMenuEntry->next = $6;
			$6->prev->next = NULL;
			tlast = menu->lastMenuEntry;
			menu->lastMenuEntry = $6->prev;
			$6->prev = tlast;
		    }
		    /* otherwise just put these on the new menu */
		    else
		    {
			menu->firstMenuEntry = $6;
			menu->lastMenuEntry = $6->prev;
			$6->prev->next = NULL;
			$6->prev = NULL;
		    }
#ifdef TCMENU
		    {
			register struct menuEntry *entry;

			for (entry = menu->firstMenuEntry; entry;
			     entry = entry->next)
			    entry->containingMenu = menu;
		    }
#endif /* TCMENU */
		}
	    }
		
	| REMOVE string
	    {
		RemoveMenuEntry($2, NULL);
	    }
	| REMOVE string FROM string
	    {
		RemoveMenuEntry($2, $4);
	    }
	;

menuspec: /* empty */
	    {
		$$ = NULL;
	    }
	| menuspec newline
	| menuspec menuitem newline
	    {
		/* so that we can keep track of both the first and
		 * last menu entry, keep the menu entries as a circular
		 * list.  Always return a pointer to the first item
		 */
		if ($1 == NULL)
		{
		    $2->next = $2;
		    $2->prev = $2;
		    $$=$2;
		}
		else
		{
		    $2->prev = $1->prev;
		    $2->next = $1;
		    $1->prev->next = $2;
		    $1->prev = $2;
		    $$ = $1;
		}
	    }
	;

menuitem: string F_MENU string
	    {
		$$ = NewMenuEntry($1, ME_MENU);
		$$->menuName = $3;
	    }
	| string F_LABEL
	    {
		$$ = NewMenuEntry($1, ME_LABEL);
	    }
	| string F_TITLE
	    {
		$$ = NewMenuEntry($1, ME_TITLE);
	    }
	| string F_SEPARATOR
	    {
		$$ = NewMenuEntry($1, ME_SEPARATOR);
	    }
	| string F_EXEC string
	    {
		$$ = NewMenuEntry($1, ME_EXEC);
		$$->execString = $3;
	    }
	| string F_CHECKEXEC string
	    {
		$$ = NewMenuEntry($1, ME_CHECKEXEC);
		$$->execString = $3;
	    }
	| string F_CHECKEXPR string string
	    {
		$$ = NewMenuEntry($1, ME_CHECKEXPR);
		$$->exprString = $3;
		$$->execString = $4;
	    }
	;

/* all tokens are also valid strings, so accept them.  Include an intermediate
 * node so that all strings can be stripped of quotes
 */
string	: _string
	    {
		$$ = UnquoteString($1);
	    }
	;

_string	: STRING
	| MENU
	| INCLUDE
	| REMOVE
	| FROM
	| F_EXEC
	| F_CHECKEXEC
	| F_CHECKEXPR
	| F_SEPARATOR
	| F_LABEL
	| F_TITLE
	| F_MENU
	| '{'
	    {
		$$ = "{";
	    }
	| '}'
	    {
		$$ = "{";
	    }
	;

newline	: '\n'
	    {
		lineno++;
	    }
%%
yyerror(s)
char *s;
{
    FileError("%s\n",s);
}

    
