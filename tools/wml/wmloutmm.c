/*
 * Motif
 *
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
static char rcsid[] = "$XConsortium: wmloutmm.c /main/9 1995/08/29 11:10:59 drk $"
#endif
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

/*
 * This module contains routines responsible for writing the .mm files
 * produced by WML. All files are written into the current directory.
 *
 * Input:
 *	The resolved objects
 *
 * Output:
 *	wml-uil.mm
 *
 */
#include <stdio.h>
#include <string.h>
#include "wml.h"

/*
 * Routines used only in this module
 */
static void wmlOutputWmlUilMm(void);
static void wmlOutputWmlUilMmClass(FILE *outfil, WmlClassDefPtr clsobj, const char *name);

/**
 * lists re-used repeatedly to order lists for printing
 */
DynamicHandleListDef    mm_arg;
DynamicHandleListDefPtr mm_arg_ptr = &mm_arg;
DynamicHandleListDef    mm_rsn;
DynamicHandleListDefPtr mm_rsn_ptr = &mm_rsn;
DynamicHandleListDef    mm_ctl;
DynamicHandleListDefPtr mm_ctl_ptr = &mm_ctl;

static const char canned1[] =
".bp\n\
.ps 12\n";

static const char canned2[] =
".ps 10\n\
.vs 12\n\
.LP\n\
.TS H\n\
tab(@);\n\
lB lB\n\
l l.\n\
_\n\
.sp 6p\n\
Controls@Reasons\n\
.sp 6p\n\
_\n\
.sp 6p\n\
.TH\n";

static const char canned3[] =
".TE\n\
.TS H\n\
tab(@);\n\
lB lB lB\n\
l l l.\n\
_\n\
.sp 6p\n\
UIL Argument Name@Argument Type@Default Value\n\
.sp 6p\n\
_\n\
.sp 6p\n\
.TH\n";

static const char canned4[] =
".TE\n";

/**
 * Output control routine, which simply outputs each .mm file in turn.
 */
void wmlOutputMmFiles(void)
{
	wmlOutputWmlUilMm();
}

/**
 * Routine to write out wml-uil.mm
 *
 * This .mm file contains the tables which are to be included as an
 * appendix to the Uil manual. The tables give the arguments with their
 * default values, reasons, constraints, and controls for each class
 * in the class vectors.
 */
static void wmlOutputWmlUilMm(void)
{
	FILE *outfil;          /* output file */
	int ndx;               /* loop index */
	WmlClassDefPtr clsobj; /* class object */

	/**
	 * Open the output file. Write the canned header stuff
	 */
	if (!(outfil = fopen("wml-uil.mm", "w+"))) {
		fputs("Couldn't open wml-uil.mm", stderr);
		return;
	}

	/**
	 * Initialize order lists for the tables.
	 */
	wmlInitHList(mm_arg_ptr, 200, TRUE);
	wmlInitHList(mm_rsn_ptr, 200, TRUE);
	wmlInitHList(mm_ctl_ptr, 200, TRUE);

	/**
	 * Write out a table for each class, for both widget and gadget variants
	 */
	for (ndx=0 ; ndx<wml_obj_class_ptr->cnt; ndx++) {
		clsobj = (WmlClassDefPtr) wml_obj_class_ptr->hvec[ndx].objptr;
		wmlOutputWmlUilMmClass(outfil, clsobj, clsobj->syndef->name);
	}

	/* close the output file */
	puts("Created wml-uil.mm");
	fclose(outfil);
}

/**
 * Routine to write a table for a class entry
 */
static void wmlOutputWmlUilMmClass(FILE *outfil, WmlClassDefPtr clsobj,
                                   const char *name)
{
	WmlClassResDefPtr argref;  /* current argument reference */
	WmlClassResDefPtr rsnref;  /* current reason reference */
	WmlClassCtrlDefPtr ctlref; /* current controls reference */
	int argndx = 0;            /* to access ordered vector */
	int rsnndx = 0;            /* to access ordered vector */
	int ctlndx = 0;            /* to access ordered vector */

	/*
	 * Write out header information
	 */
	fputs(canned1, outfil);
	fputs(name, outfil);
	fputc('\n', outfil);
	fputs(canned2, outfil);

	/**
	 * Alphabetize the controls, reason, and argument lists
	 */
	wmlClearHList(mm_arg_ptr);
	wmlClearHList(mm_rsn_ptr);
	wmlClearHList(mm_ctl_ptr);

	argref = clsobj->arguments;
	while (argref) {
		while (argref && argref->exclude == WmlAttributeTrue)
			argref = argref->next;

		if (!argref)
			break;

		wmlInsertInHList(mm_arg_ptr, argref->act_resource->syndef->name,
		                 (ObjectPtr)argref);
		argref = argref->next;
	}

	rsnref = clsobj->reasons;
	while (rsnref) {
		while (rsnref && rsnref->exclude == WmlAttributeTrue)
			rsnref = rsnref->next;

		if (!rsnref)
			break;

		wmlInsertInHList(mm_rsn_ptr, rsnref->act_resource->syndef->name,
		                 (ObjectPtr)rsnref);
		rsnref = rsnref->next;
	}

	ctlref = clsobj->controls;
	while (ctlref) {
		wmlInsertInHList(mm_ctl_ptr, ctlref->ctrl->syndef->name, (ObjectPtr)ctlref);
		ctlref = ctlref->next;
	}

	/**
	 * Write out the controls and reason table.
	 */
	if (!mm_ctl_ptr->cnt)
		fputs("No children are supported", outfil);

	while (rsnndx < mm_rsn_ptr->cnt || ctlndx < mm_ctl_ptr->cnt) {
		if (ctlndx < mm_ctl_ptr->cnt) {
			ctlref = (WmlClassCtrlDefPtr)mm_ctl_ptr->hvec[ctlndx++].objptr;
			fputs(ctlref->ctrl->syndef->name, outfil);
		}

		fputc('@', outfil);
		if (rsnndx < mm_rsn_ptr->cnt) {
			rsnref = (WmlClassResDefPtr)mm_rsn_ptr->hvec[rsnndx++].objptr;
			fputs(rsnref->act_resource->syndef->name, outfil);
		}

		fputc('\n', outfil);
	}
	fputs(canned3, outfil);

	/**
	 * Write out the argument table
	 */
	argndx = 0;
	while (argndx < mm_arg_ptr->cnt) {
		argref = (WmlClassResDefPtr)mm_arg_ptr->hvec[argndx++].objptr;
		fputs(argref->act_resource->syndef->name, outfil);
		fputc('@', outfil);
		fputs(argref->act_resource->dtype_def->syndef->name, outfil);
		fputc('@', outfil);

		if (argref->dflt) {
			fprintf(outfil,
			       strchr(argref->dflt, ' ')
			       ? "T{\n%s\nT}\n" : "%s\n",
			       argref->dflt);
		} else if (argref->act_resource->syndef->dflt) {
			fprintf(outfil,
			       strchr(argref->act_resource->syndef->dflt, ' ')
			       ? "T{\n%s\nT}\n" : "%s\n",
			       argref->act_resource->syndef->dflt);
		} else fputs("  \n", outfil);
	}

	fputs(canned4, outfil);
}

