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
#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$TOG: wml.c /main/8 1999/04/16 09:41:47 mgreess $"
#endif
#endif

/*
 * This is the main program for WML. It declares all global data structures
 * used during a compilation, and during output.
 */

/*
 * WML is a semi-standard Unix application. It reads its input from
 * stdin, which is expected to be a stream containing a WML description
 * of a UIL language. If this stream is successfully parsed and semantically
 * validated, then WML writes a series of standard .h and .dat files into
 * the user directory. The .h files will be used directly to construct
 * the UIL compiler. The .dat files are used by other phases of UIL
 * table generation.
 *
 * The files created by WML are:
 *
 *	.h files:
 *		UilSymGen.h
 *		UilSymArTy.h
 *		UilSymRArg.h
 *		UilDrmClas.h
 *		UilConst.h
 *		UilSymReas.h
 *		UilSymArTa.h
 *		UilSymCtl.h
 *		UilSymNam.h
 *	.dat files
 *		argument.dat
 *		reason.dat
 *		grammar.dat
 *	.mm files
 *		wml-uil.mm
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "wml.h"

/**
 * Globals used during WML parsing.
 *
 * WML uses globals exclusively to communicate data during parsing. The
 * current object being constructed is held by these globals, and all
 * routines called from the parse assume correct setting of these globals.
 * This simplisitic approach is possible since the WML description language
 * has no recursive constructs requiring a frame stack.
 */
int	wml_err_count  = 0; /* total errors */
int	wml_line_count = 0; /* lines read from input */

/**
 * Dynamic ordered vector of all objects encountered during parse. This
 * is used to detect name collisions, and is the primary order vector
 * used for all other vectors constructed curing the semantic resolution
 * phase of processing.
 */
static DynamicHandleListDef wml_synobj;
DynamicHandleListDefPtr wml_synobj_ptr = &wml_synobj;

/**
 * Dynamic vectors of vectors partitioned and ordered
 * as required by the semantic processing and output routines. All
 * point to resolved objects rather than syntactic objects.
 */
static DynamicHandleListDef wml_obj_datatype; /* datatype objects */
static DynamicHandleListDef wml_obj_enumval;  /* enumeration value objects */
static DynamicHandleListDef wml_obj_enumset;  /* enumeration set objects */
static DynamicHandleListDef wml_obj_reason;	  /* reason resource objects */
static DynamicHandleListDef wml_obj_arg;      /* argument resource objects */
static DynamicHandleListDef wml_obj_child;    /* argument resource objects */
static DynamicHandleListDef wml_obj_allclass; /* metaclass, widget, gadget */
static DynamicHandleListDef wml_obj_class;    /* widget & gadget objects */
static DynamicHandleListDef wml_obj_ctrlist;  /* controls list objects */
static DynamicHandleListDef wml_obj_charset;  /* charset objects */
static DynamicHandleListDef wml_tok_sens;     /* case-sensitive tokens */
static DynamicHandleListDef wml_tok_insens;   /* case-insensitive tokens */

DynamicHandleListDefPtr wml_obj_datatype_ptr = &wml_obj_datatype;
DynamicHandleListDefPtr wml_obj_enumval_ptr  = &wml_obj_enumval;
DynamicHandleListDefPtr wml_obj_enumset_ptr  = &wml_obj_enumset;
DynamicHandleListDefPtr wml_obj_reason_ptr   = &wml_obj_reason;
DynamicHandleListDefPtr wml_obj_arg_ptr      = &wml_obj_arg;
DynamicHandleListDefPtr wml_obj_child_ptr    = &wml_obj_child;
DynamicHandleListDefPtr wml_obj_allclass_ptr = &wml_obj_allclass;
DynamicHandleListDefPtr wml_obj_class_ptr    = &wml_obj_class;
DynamicHandleListDefPtr wml_obj_ctrlist_ptr  = &wml_obj_ctrlist;
DynamicHandleListDefPtr wml_obj_charset_ptr  = &wml_obj_charset;
DynamicHandleListDefPtr wml_tok_sens_ptr     = &wml_tok_sens;
DynamicHandleListDefPtr wml_tok_insens_ptr   = &wml_tok_insens;

extern int yyleng;
extern FILE *yyin;
extern int yyparse(void);

int yywrap(void)
{
	return 1;
}

/**
 * The WML main routine:
 *
 *	1. Initialize global storage
 *	2. Open the input file if there is one
 *	3. Parse the WML description in stdin. Exit on errors
 *	4. Perform semantic validation and resolution. Exit on errors.
 *	5. Output files
 */
int main (int argc, char *argv[])
{
	/* Initialize the list of all syntactic objects */
	if (!wmlInitHList(wml_synobj_ptr, 1000, TRUE, FALSE)) {
		++wml_err_count;
		fputs("Failed to initialize synobj list", stderr);
		goto done;
	}

	/* Assume that argv[1] is our input file */
	yyleng = 0;
	if (argc > 1 && !(yyin = fopen(argv[1], "r"))) {
		++wml_err_count;
		fprintf(stderr, "\nCouldn't open file %s", argv[1]);
		goto done;
	}

	/* Parse the input stream */
	if ((wml_err_count = yyparse()))
		goto done;
	puts("Parsing of WML input complete");

	/* Perform semantic validation, and construct resolved data structures */
	wmlResolveDescriptors();
	if (wml_err_count)
		goto done;
	puts("Semantic valdiation and resolution complete");

	/* Output */
	wmlOutput();
	if (wml_err_count)
		goto done;
	puts("WML Uil*.h and wml-uil.mm file creation complete");

done:
	if (yyin) fclose(yyin);
	if (wml_err_count) {
		fprintf(stderr, "\nWML found %d errors, no or incomplete output produced\n", wml_err_count);
		exit(EXIT_FAILURE);
	}

	return 0;
}

