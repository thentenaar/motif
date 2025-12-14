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
static char rcsid[] = "$XConsortium: wmlresolve.c /main/9 1995/08/29 11:11:05 drk $"
#endif
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

/**
 * This module contains all routines which perform semantic analysis of
 * the parsed WML specification. It is responsible for building all
 * ordered structures which can be directly translated into literal
 * code values for the various .h files. It is responsible for performing
 * inheritance of resources for all classes.
 *
 * Input:
 *	the ordered list of syntactic objects in wml_synobj_ptr
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "wml.h"
#include "wmlnoreturn.h"

/* Local routines */
static void wmlResolveGenerateSymK(void);
static void wmlResolveSymKDataType(void);
static void wmlResolveSymKChild(void);
static void wmlResolveSymKEnumVal(void);
static void wmlResolveSymKEnumSet(void);
static void wmlResolveSymKReason(void);
static void wmlResolveSymKArgument(void);
static void wmlResolveSymKRelated(void);
static void wmlResolveSymKClass(void);
static void wmlResolveSymKCtrlList(void);
static void wmlResolveSymKCharSet(void);

static void wmlResolveValidateClass(void);
static void wmlResolvePrintReport(void);
static void wmlResolveClassInherit(WmlClassDefPtr clsobj);
static void wmlResolveClearRefPointers(void);
static void wmlResolveInitRefObj(WmlClassResDefPtr dst, WmlClassResDefPtr src);
static void wmlResolveInitChildRefObj(WmlClassChildDefPtr dst,
                                      WmlClassChildDefPtr src);
static void wmlResolvePrintClass(FILE *fp, WmlClassDefPtr clsobj);
static void wmlResolvePrintClassArgs(FILE *fp, WmlClassDefPtr clsobj);
static void wmlResolvePrintClassReasons(FILE *fp, WmlClassDefPtr clsobj);
static ObjectPtr wmlResolveFindObject(const char *name, int type, const char *requester);

/**
 * Report an error
 */
static void err(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	fputc('\n', stderr);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	wml_err_count++;
}

/**
 * Report a fatal error and exit
 */
noreturn static void fatal(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	fputc('\n', stderr);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	exit(EXIT_FAILURE);
}

/**
 * Report an object reference error
 *
 *	srcname		the object making the reference
 *	badname		the missing object
 */
static void wmlIssueReferenceError(const char *src, const char *bad)
{
	err("Object %s references undefined object %s", src, bad);
}

/**
 * Report an attempt to make a reference which is not supported.
 */
static void wmlIssueIllegalReferenceError(const char *src, const char *bad)
{
	err("Object %s cannot reference a %s object", src, bad);
}

/**
 * The control routine for semantic analysis. It calls the various phases.
 */
void wmlResolveDescriptors(void)
{
	/*
	 * Perform the code assignment pass. This results in assignment of sym_k_...
	 * codes to all entities. Also, all objects and cross-linking are validated.
	 */
	wmlResolveGenerateSymK();
	puts("Initial validation and reference resolution complete");

	/*
	 * Perform class inheritance and validation
	 */
	wmlResolveValidateClass();
	puts("Class validation and inheritance complete");

	/*
	 * Print a report
	 */
	if (!wml_err_count)
		wmlResolvePrintReport();
}

/**
 * Routine to mark reference pointers for a class
 *
 * This routine clears all reference pointers, then marks the class and
 * resource objects to flag those contained in the current class. This
 * allows processing of the widget and resource vectors in order to produce
 * bit masks or reports.
 */
void wmlMarkReferencePointers(WmlClassDefPtr clsobj)
{
	WmlClassResDefPtr resref;
	WmlClassCtrlDefPtr ctrlref;

	/**
	 * Clear the reference pointers. Then go through the arguments, reasons,
	 * and controls lists, and mark the referenced classes.
	 */
	wmlResolveClearRefPointers();

	for (resref=clsobj->arguments; resref; resref=resref->next)
		resref->act_resource->ref_ptr = resref;

	for (resref=clsobj->reasons; resref; resref=resref->next)
		resref->act_resource->ref_ptr = resref;

	for (ctrlref=clsobj->controls; ctrlref; ctrlref=ctrlref->next)
		ctrlref->ctrl->ref_ptr = ctrlref;
}

/**
 * Routine to linearize and assign sym_k... literals for objects. Simply
 * a dispatching routine.
 */
static void wmlResolveGenerateSymK(void)
{
	/* Process the datatype objects */
	wmlResolveSymKDataType();

	/* Process the enumeration value and enumeration sets */
	wmlResolveSymKEnumVal();
	wmlResolveSymKEnumSet();

	/* Process the resources, producing argument and reason vectors */
	wmlResolveSymKReason();
	wmlResolveSymKArgument();

	/* Bind related arguments */
	wmlResolveSymKRelated();

	/* Process the class definitions */
	wmlResolveSymKClass();

	/* Process the controls list definitions */
	wmlResolveSymKCtrlList();

	/* Process the charset objects */
	wmlResolveSymKCharSet();

	/* Process the child definitions */
	wmlResolveSymKChild();
}

/**
 * Routine to linearize data types
 *
 * - Generate the wml_obj_datatype... vector of resolved data type objects,
 *   ordered lexicographically.
 *   Do name processing, and acquire links to any other objects named in
 *   the syntactic descriptor.
 */
static void wmlResolveSymKDataType(void)
{
	int i;
	WmlSynDataTypeDefPtr cursyn;
	WmlDataTypeDefPtr newobj;

	/**
	 * Initialize the object vector. Then process the syntactic vector,
	 * processing each datatype object encountered (the vector is ordered).
	 * create and append a resolved object for each one encountered. This
	 * will be ordered as well.
	 */
	if (!wmlInitHList(wml_obj_datatype_ptr, 50, TRUE, FALSE))
		fatal("wmlResolveSymKDataType: Failed to initialize list");

	for (i = 0; i < wml_synobj_ptr->cnt; i++) {
		cursyn = wml_synobj_ptr->hvec[i].objptr;
		if (cursyn->validation != WmlDataTypeDefValid)
			continue;

		/* Create and initialize a new object and apppend to resolved objects */
		if (!(newobj = malloc(sizeof *newobj)))
			fatal("wmlResolveSymKDataType: Out of memory");

		newobj->syndef  = cursyn;
		newobj->tkname  = cursyn->int_lit ? cursyn->int_lit : cursyn->name;
		cursyn->rslvdef = newobj;
		if (!wmlInsertInHList(wml_obj_datatype_ptr, newobj->tkname, newobj))
			fatal("wmlResolveSymKDataType: List insert failed");
	}
}

/**
 * Routine to linearize children
 *
 * - Generate the wml_obj_child... vector of resolved child objects,
 *   ordered lexicographically.  Assign sym_k_... values while doing so.
 *   Link child to its class.
 */
static void wmlResolveSymKChild(void)
{
	int i, code = 0;
	WmlSynChildDefPtr cursyn;
	WmlChildDefPtr newobj;

	/**
	 * Initialize the object vector. Then process the syntactic vector,
	 * processing each child object encountered (the vector is ordered).
	 * create and append a resolved object for each one encountered. This
	 * will be ordered as well.
	 */
	if (!wmlInitHList(wml_obj_child_ptr, 50, TRUE, FALSE))
		fatal("wmlResolveSymKChild: Failed to initialize list");

	for (i = 0; i < wml_synobj_ptr->cnt; i++) {
		cursyn = wml_synobj_ptr->hvec[i].objptr;
		if (cursyn->validation != WmlChildDefValid)
			continue;

		/* Create and initialize a new object and apppend to resolved objects */
		if (!(newobj = calloc(1, sizeof *newobj)))
			fatal("wmlResolveSymKChild: Out of memory");

		newobj->syndef  = cursyn;
		newobj->tkname  = cursyn->name;
		cursyn->rslvdef = newobj;
		if (!wmlInsertInHList(wml_obj_child_ptr, newobj->tkname, newobj))
			fatal("wmlResolveSymKChild: List insert failed");

		/* Link class to the resolved object */
		if (cursyn->class) {
			newobj->class = wmlResolveFindObject(cursyn->class,
			                                     WmlClassDefValid, cursyn->name);
		}
	}

	/**
	 * All objects are in the vector. The order is the code order, so
	 * process it again and assign codes to each object
	 */
	for (i = 0; i < wml_obj_child_ptr->cnt; i++)
		((WmlChildDefPtr)wml_obj_child_ptr->hvec[i].objptr)->sym_code = ++code;
}

/**
 * Routine to linearize and assign sym_k values to enumeration values
 *
 * - Generate the wml_obj_datatype... vector of resolved data type objects,
 *   ordered lexicographically.
 */
static void wmlResolveSymKEnumVal(void)
{
	int i, code = 0;
	WmlSynEnumSetDefPtr cures;
	WmlSynEnumSetValDefPtr curesv;
	WmlSynEnumValueDefPtr cursyn;
	WmlEnumValueDefPtr newobj;

	/**
	 * Perform defaulting. Process all the enumeration sets, and define a
	 * syntactic object for every enumeration value named in an enumeration set
	 * which has no syntactic entry. If there is an error in a name, then
	 * this error won't be detected until we attempt to compile the output .h files.
	 */
	for (i = 0; i < wml_synobj_ptr->cnt; i++) {
		cures = wml_synobj_ptr->hvec[i].objptr;
		if (cures->validation != WmlEnumSetDefValid)
			continue;

		/* Create missing enum values */
		for (curesv = cures->values; curesv; curesv = curesv->next) {
			if (wmlFindInHList(wml_synobj_ptr, curesv->name) == -1)
				wmlCreateEnumValue(curesv->name);
		}
	}

	/**
	 * Initialize the object vector. Then process the syntactic vector,
	 * processing each enumeration value object encountered (the vector is ordered).
	 * create and append a resolved object for each one encountered. This
	 * will be ordered as well.
	 */
	if (!wmlInitHList(wml_obj_enumval_ptr, 50, TRUE, FALSE))
		fatal("wmlResolveSymKEnumValue: Failed to initialize list");

	for (i = 0; i < wml_synobj_ptr->cnt; i++) {
		cursyn = wml_synobj_ptr->hvec[i].objptr;
		if (!cursyn || cursyn->validation != WmlEnumValueDefValid)
			continue;

		/* Create and initialize new object. Append to resolved object vector. */
		if (!(newobj = calloc(1, sizeof *newobj)))
			fatal("wmlResolveSymKEnumVal: Out of memory");

		newobj->syndef  = cursyn;
		cursyn->rslvdef = newobj;
		if (!wmlInsertInHList(wml_obj_enumval_ptr, cursyn->name, newobj))
			fatal("wmlResolveSymKEnumVal: List insert failed");
	}

	/**
	 * All objects are in the vector. The order is the code order, so
	 * process it again and assign codes to each object
	 */
	for (i = 0; i < wml_obj_enumval_ptr->cnt; i++)
		((WmlEnumValueDefPtr)wml_obj_enumval_ptr->hvec[i].objptr)->sym_code = ++code;
}

/**
 * Routine to linearize and assign sym_k values to enumeration sets
 *
 * - Generate the wml_obj_datatype... vector of resolved data type objects,
 *   ordered lexicographically.
 */
static void wmlResolveSymKEnumSet(void)
{
	int i, code = 0;
	WmlSynEnumSetDefPtr cursyn;
	WmlEnumSetDefPtr newobj;
	WmlEnumValueDefPtr evobj;
	WmlEnumSetValDefPtr esvobj;
	WmlSynEnumSetValDefPtr esvelm;

	/**
	 * Initialize the object vector. Then process the syntactic vector,
	 * processing each enumeration set object encountered (the vector is ordered).
	 * create and append a resolved object for each one encountered. This
	 * will be ordered as well.
	 */
	if (!wmlInitHList(wml_obj_enumset_ptr, 20, TRUE, FALSE))
		fatal("wmlResolveSymKEnumSet: Failed to initialize list");

	for (i = 0; i < wml_synobj_ptr->cnt; i++) {
		cursyn = wml_synobj_ptr->hvec[i].objptr;
		if (cursyn->validation != WmlEnumSetDefValid)
			continue;

		/* Create and initialize new object. Append to resolved object vector. */
		if (!(newobj = calloc(1, sizeof *newobj)))
			fatal("wmlResolveSymKEnumSet: Out of memory");

		cursyn->rslvdef   = newobj;
		newobj->syndef    = cursyn;
		newobj->tkname    = cursyn->name;
		newobj->dtype_def = wmlResolveFindObject(cursyn->datatype,
		                                         WmlDataTypeDefValid, cursyn->name);
		if (!wmlInsertInHList(wml_obj_enumset_ptr, cursyn->name, newobj))
			fatal("wmlResolveSymKEnumSet: List insert failed");
	}

	/**
	 * All objects are in the vector. That order is the code order, so
	 * process it again and assign codes to each object. Simultaneously
	 * construct a vector of resolved enumeration values, and count them.
	 */
	for (i = 0; i < wml_obj_enumset_ptr->cnt; i++) {
		newobj = wml_obj_enumset_ptr->hvec[i].objptr;
		newobj->sym_code = ++code;

		/**
		 * Validate and construct a resolved enumeration value list
		 */
		cursyn = newobj->syndef;
		for (esvelm = cursyn->values; esvelm; esvelm = esvelm->next) {
			evobj = wmlResolveFindObject(esvelm->name,
			                             WmlEnumValueDefValid, cursyn->name);
			if (!evobj) continue;

			if (!(esvobj = malloc(sizeof *esvobj)))
				fatal("wmlResolveSymKEnumSet: Out of memory");

			esvobj->value  = evobj;
			esvobj->next   = newobj->values;
			newobj->values = esvobj;
			newobj->values_cnt++;
		}
    }
}

/**
 * Routine to linearize and assign sym_k values to reasons.
 *
 * - Generate the wml_obj_reason... vector of resolved reason objects,
 *   ordered lexicographically. Assign a sym_k_... value as this is done.
 *   Do name processing, and acquire links to any other objects named in
 *   the syntactic descriptor.
 */
static void wmlResolveSymKReason(void)
{
	int i, code = 0;
	WmlSynResourceDefPtr cursyn;
	WmlResourceDefPtr newobj;

	/**
	 * Initialize the object vector. Then process the syntactic vector,
	 * processing each reason resource object encountered (the vector is
	 * ordered). Create and append a resolved object for each one
	 * encountered. This will be ordered as well.
	 */
	if (!wmlInitHList(wml_obj_reason_ptr, 100, TRUE, FALSE))
		fatal("wmlResolveSymKReason: Failed to initialize list");

	for (i = 0; i < wml_synobj_ptr->cnt; i++) {
		cursyn = wml_synobj_ptr->hvec[i].objptr;
		if (cursyn->validation != WmlResourceDefValid ||
		    cursyn->type       != WmlResourceTypeReason)
			continue;

		/* Create and initialize new object. Append to resolved object vector. */
		if (!(newobj = calloc(1, sizeof *newobj)))
			fatal("wmlResolveSymKReason: Out of memory");

		newobj->syndef  = cursyn;
		newobj->tkname  = cursyn->int_lit ? cursyn->int_lit : cursyn->name;
		cursyn->rslvdef = newobj;
		if (!wmlInsertInHList(wml_obj_reason_ptr, newobj->tkname, newobj))
			fatal("wmlResolveSymKReason: List insert failed");

		/**
		 * Validate any object references in the syntactic object
		 * Reason can't bind to some objects.
		 */
		if (cursyn->datatype)
			wmlIssueIllegalReferenceError(cursyn->name, "DataType");
	}

	/**
	 * All objects are in the vector. The order is the code order, so
	 * process it again and assign codes to each object
	 */
	for (i = 0; i < wml_obj_reason_ptr->cnt; i++)
		((WmlResourceDefPtr)wml_obj_reason_ptr->hvec[i].objptr)->sym_code = ++code;
}

/**
 * Routine to linearize and assign sym_k values to arguments.
 *
 * - Generate the wml_obj_arg... vector of resovled reason objects,
 *   ordered lexicographically. Assign a sym_k_... values while doing so.
 *   validate the data type for each argument, and link it to its data type
 *   object.
 *   Do name processing, and acquire links to any other objects named in
 *   the syntactic descriptor.
 */
static void wmlResolveSymKArgument(void)
{
	int i, code = 0;
	WmlSynResourceDefPtr cursyn;
	WmlResourceDefPtr newobj;

	/**
	 * Initialize the object vector. Then process the syntactic vector,
	 * processing each reason resource object encountered (the vector is
	 * ordered). Create and append a resolved object for each one
	 * encountered. This will be ordered as well.
	 */
	if (!wmlInitHList(wml_obj_arg_ptr, 500, TRUE, FALSE))
		fatal("wmlResolveSymKArgument: Failed to initialize list");

	for (i = 0; i < wml_synobj_ptr->cnt; i++) {
		cursyn = wml_synobj_ptr->hvec[i].objptr;
		if (cursyn->validation != WmlResourceDefValid ||
		    cursyn->type       == WmlResourceTypeReason)
			continue;

		/* Create and initialize new object. Append to resolved object vector. */
		if (!(newobj = calloc(1, sizeof *newobj)))
			fatal("wmlResolveSymKArgument: Out of memory");

		newobj->syndef  = cursyn;
		newobj->tkname  = cursyn->int_lit ? cursyn->int_lit : cursyn->name;
		cursyn->rslvdef = newobj;
		if (!wmlInsertInHList(wml_obj_arg_ptr, newobj->tkname, newobj))
			fatal("wmlResolveSymKArgument: List insert failed");

		/* Validate any object references in the syntactic object */
		newobj->dtype_def = wmlResolveFindObject(cursyn->datatype,
		                                         WmlDataTypeDefValid,
		                                         cursyn->name);
		if (cursyn->enumset) {
			newobj->enumset_def = wmlResolveFindObject(cursyn->enumset,
			                                           WmlEnumSetDefValid,
			                                           cursyn->name);
		}
	}

	/**
	 * All objects are in the vector. The order is the code order, so
	 * process it again and assign codes to each object
	 */
	for (i = 0; i < wml_obj_arg_ptr->cnt; i++)
		((WmlResourceDefPtr)wml_obj_arg_ptr->hvec[i].objptr)->sym_code = ++code;
}

/**
 * Routine to resolve related argument references.
 *
 * Search the argument vector for any argument with its related
 * argument set. Find the related argument, and bind the relation.
 * The binding only goes one way.
 */
static void wmlResolveSymKRelated(void)
{
	int i;
	WmlResourceDefPtr srcobj, dstobj;

	/* Scan all arguments for related argument bindings */
	for (i = 0; i < wml_obj_arg_ptr->cnt; i++) {
		srcobj = wml_obj_arg_ptr->hvec[i].objptr;
		if (!srcobj->syndef->related)
			continue;

		dstobj = wmlResolveFindObject(srcobj->syndef->related,
		                              WmlResourceDefValid, srcobj->syndef->name);
		if (dstobj) srcobj->related_code = dstobj->related_code;
	}
}

/**
 * Routine to linearize and assign sym_k values to classes
 *
 * There are two linearizations of classes:
 *	- all classes in wml_obj_allclass...
 *	- all widgets and gadgets in wml_obj_class...
 * Create and linearize all class objects into these vectors. Assign sym_k
 * codes. Link all subclasses to their superclasses. Perform name processing
 * and link to any other named object.
 *
 * Resources are not inherited and linked at this time.
 */
static void wmlResolveSymKClass(void)
{
	int i, code = 0;
	WmlSynClassDefPtr cursyn;
	WmlClassDefPtr newobj;

	/**
	 * Initialize the object vectors. Then process the syntactic vector,
	 * processing each class object encountered (the vector is ordered).
	 * create and append a resolved object for each one encountered. This
	 * will be ordered as well.
	 */
	if (!wmlInitHList(wml_obj_allclass_ptr, 200, TRUE, FALSE) ||
	    !wmlInitHList(wml_obj_class_ptr,    200, TRUE, FALSE))
		fatal("wmlResolveSymKClass: Failed to initialize list");

	for (i = 0; i < wml_synobj_ptr->cnt; i++) {
		cursyn = wml_synobj_ptr->hvec[i].objptr;
		if (cursyn->validation != WmlClassDefValid)
			continue;

		/* Create and initialize new object. Append to resolved object vector. */
		if (!(newobj = calloc(1, sizeof *newobj)))
			fatal("wmlResolveSymKClass: Out of memory");

		newobj->syndef  = cursyn;
		newobj->tkname  = cursyn->int_lit ? cursyn->int_lit : cursyn->name;
		cursyn->rslvdef = newobj;

		switch (cursyn->type) {
		case WmlClassTypeGadget:
		case WmlClassTypeWidget:
			if (!wmlInsertInHList(wml_obj_class_ptr, newobj->tkname, newobj))
				fatal("wmlResolveSymKClass: List insert (class) failed");

			if (!cursyn->convfunc)
				err("Class %s lacks a convenience function", cursyn->name);
		case WmlClassTypeMetaclass:
			if (!wmlInsertInHList(wml_obj_allclass_ptr, newobj->tkname, newobj))
				fatal("wmlResolveSymKClass: List insert (allclass) failed");
			break;
		}

		if (cursyn->ctrlmapto) {
			newobj->ctrlmapto = wmlResolveFindObject(cursyn->ctrlmapto,
			                                         WmlResourceDefValid,
			                                         cursyn->name);
		}
	}

	/**
	 * All objects are in the vector. The order is the code order, so
	 * process it again and assign codes to each object
	 */
	for (i = 0; i < wml_obj_class_ptr->cnt; i++)
		((WmlClassDefPtr)wml_obj_class_ptr->hvec[i].objptr)->sym_code = ++code;
}

/**
 * Routine to validate controls lists
 *
 * Construct and linearize resolved controls lists. The linearized list
 * is used to resolve references.
 */
static void wmlResolveSymKCtrlList(void)
{
	int i;
	WmlSynCtrlListDefPtr cursyn;
	WmlSynClassCtrlDefPtr refptr;
	WmlCtrlListDefPtr newobj;
	WmlClassDefPtr classobj;
	WmlClassCtrlDefPtr ctrlobj;

	/* Process each control list. Construct a resolved control list for each */
	if (!wmlInitHList(wml_obj_ctrlist_ptr, 20, TRUE, FALSE))
			fatal("wmlResolveSymKCtrlList: Failed to initialize list");

	for (i = 0; i < wml_synobj_ptr->cnt; i++) {
		cursyn = wml_synobj_ptr->hvec[i].objptr;
		if (cursyn->validation != WmlCtrlListDefValid)
			continue;

		/* Create and initialize new object. Append to resolved object vector. */
		if (!(newobj = calloc(1, sizeof *newobj)))
			fatal("wmlResolveSymKCtrlList: Out of memory");

		newobj->syndef  = cursyn;
		cursyn->rslvdef = newobj;
		if (!wmlInsertInHList(wml_obj_ctrlist_ptr, cursyn->name, newobj))
			fatal("wmlResolveSymKCtrlList: List insert failed");

		/* Validate and construct a resolved controls reference list. */
		for (refptr = cursyn->controls; refptr; refptr = refptr->next) {
			classobj = wmlResolveFindObject(refptr->name, WmlClassDefValid,
			                                cursyn->name);
			if (!classobj) continue;

			if (!(ctrlobj = calloc(1, sizeof *ctrlobj)))
				fatal("wmlResolveSymKCtrlList: Out of memory");

			ctrlobj->next    = newobj->controls;
			ctrlobj->ctrl    = classobj;
			newobj->controls = ctrlobj;
		}
	}
}

/**
 * Routine to linearize and assign sym_k values to character sets
 *
 * - Generate the wml_obj_charset... vector of resolved data type objects,
 *   ordered lexicographically. Assign a sym_k... value as this is done.
 *   Do name processing, and acquire links to any other objects named in
 *   the syntactic descriptor.
 */
static void wmlResolveSymKCharSet(void)
{
	int i, code = 1;
	WmlSynCharSetDefPtr cursyn;
	WmlCharSetDefPtr newobj;

	/**
	 * Initialize the object vector. Then process the syntactic vector,
	 * processing each charset object encountered (the vector is ordered).
	 * create and append a resolved object for each one encountered. This
	 * will be ordered as well.
	 */
	if (!wmlInitHList(wml_obj_charset_ptr, 50, TRUE, FALSE))
			fatal("wmlResolveSymKCharSet: Failed to initialize list");

	for (i = 0; i < wml_synobj_ptr->cnt; i++) {
		cursyn = wml_synobj_ptr->hvec[i].objptr;
		if (cursyn->validation != WmlCharSetDefValid)
			continue;

		/* Create and initialize new object. Append to resolved object vector. */
		if (!(newobj = calloc(1, sizeof *newobj)))
			fatal("wmlResolveSymKCharSet: Out of memory");

		newobj->syndef  = cursyn;
		newobj->tkname  = cursyn->int_lit ? cursyn->int_lit : cursyn->name;
		cursyn->rslvdef = newobj;
		if (!wmlInsertInHList(wml_obj_charset_ptr, newobj->tkname, newobj))
			fatal("wmlResolveSymKCharSet: List insert failed");

		/* Parsing direction defaults to writing direction if unspecified */
		if (cursyn->parsedirection == WmlAttributeUnspecified)
			cursyn->parsedirection = cursyn->direction;

		/* Require StandardsName attribute for character set */
		if (!cursyn->xms_name)
			err("CharacterSet %s does not have a StandardsName", cursyn->name);
	}

	/**
	 * All objects are in the vector. The order is the code order, so
	 * process it again and assign codes to each object
	 */
	for (i = 0; i < wml_obj_charset_ptr->cnt; i++)
		((WmlCharSetDefPtr)(wml_obj_charset_ptr->hvec[i].objptr))->sym_code = ++code;
}

/**
 * Routine to perform class inheritance and validation.
 *
 * This routine has two major phases:
 *	- Complete resolution of class references, and expand out
 *	  controls list.
 *	- Perform inheritance of resources, partitioning them into
 *	  into arguments and reasons. When complete, the class has
 *	  a list of all its resources, including copies from a
 *	  superclass and possibly a parentclass.
 *	  Excluded resources remain in the list, and are simply marked.
 */
static void wmlResolveValidateClass(void)
{
	int i, idx;
	WmlClassDefPtr clsobj, superobj, parentobj, widgetobj, refcls;
	WmlSynClassCtrlDefPtr refptr;
	WmlClassCtrlDefPtr ctrlobj, el;
	WmlCtrlListDefPtr reflist;

	/* Acquire parent/super pointers for widget and gadget classes */
	for (i = 0; i < wml_obj_allclass_ptr->cnt; i++) {
		clsobj = wml_obj_allclass_ptr->hvec[i].objptr;

		/* Acquire the superclass pointer */
		if (clsobj->syndef->superclass) {
			superobj = wmlResolveFindObject(clsobj->syndef->superclass,
			                                WmlClassDefValid,
			                                clsobj->syndef->name);
			if (superobj) clsobj->superclass = superobj;
		}

		/* Acquire the parentclass pointer */
		if (clsobj->syndef->parentclass) {
			parentobj = wmlResolveFindObject(clsobj->syndef->parentclass,
			                                 WmlClassDefValid,
			                                 clsobj->syndef->name);
			if (parentobj) clsobj->parentclass = parentobj;
		}
	}

	/**
	 * Link each gadget class with its widget class (both ways).
	 * Link any class with a non-dialog version to the non-dialog class.
	 */
	for (i = 0; i < wml_obj_class_ptr->cnt; i++) {
		clsobj = wml_obj_class_ptr->hvec[i].objptr;
		if (clsobj->syndef->type == WmlClassTypeGadget) {
			if (!clsobj->syndef->widgetclass) {
				err("Gadget class %s has no widgetclass reference",
				    clsobj->syndef->name);
			} else {
				widgetobj = wmlResolveFindObject(clsobj->syndef->widgetclass,
				                                 WmlClassDefValid,
				                                 clsobj->syndef->name);

				if (widgetobj) {
					clsobj->variant    = widgetobj;
					widgetobj->variant = clsobj;
				}
			}
		}

		if (clsobj->syndef->dialog) {
			clsobj->nondialog = clsobj->superclass;
			while (clsobj->nondialog->syndef->dialog)
				clsobj->nondialog = clsobj->nondialog->superclass;
		} else if (clsobj->superclass) {
			clsobj->syndef->dialog = clsobj->superclass->syndef->dialog;
			clsobj->nondialog      = clsobj->superclass->nondialog;
		}
	}

	/**
	 * Construct the list of resolved controls. Control lists are expanded
	 * in place.
	 */
	for (i = 0; i < wml_obj_class_ptr->cnt; i++) {
		clsobj = wml_obj_class_ptr->hvec[i].objptr;

		for (refptr = clsobj->syndef->controls; refptr; refptr = refptr->next) {
			if ((idx = wmlFindInHList(wml_obj_class_ptr, refptr->name)) >= 0) {
				refcls = wml_obj_class_ptr->hvec[idx].objptr;
				if (!(ctrlobj = malloc(sizeof *ctrlobj)))
					fatal("wmlResolveValidateClass: Out of memory");
				ctrlobj->ctrl    = refcls;
				ctrlobj->next    = clsobj->controls;
				clsobj->controls = ctrlobj;
				continue;
			}

			if ((idx = wmlFindInHList(wml_obj_ctrlist_ptr, refptr->name)) >= 0) {
				reflist = wml_obj_ctrlist_ptr->hvec[idx].objptr;
				for (el = reflist->controls; el; el = el->next) {
					if (!(ctrlobj = malloc(sizeof *ctrlobj)))
						fatal("wmlResolveValidateClass: Out of memory");
					ctrlobj->ctrl    = el->ctrl;
					ctrlobj->next    = clsobj->controls;
					clsobj->controls = ctrlobj;
				}
			} else wmlIssueReferenceError(clsobj->syndef->name, refptr->name);
		}
	}

	/**
	 * Perform resource inheritance for each class. This constructs the
	 * arguments and reasons reference vectors.
	 */
	for (idx = 0; idx < wml_obj_allclass_ptr->cnt; idx++)
		wmlResolveClassInherit(wml_obj_allclass_ptr->hvec[idx].objptr);
}

/**
 * Routine to perform resource inheritance for a class.
 *
 * This routine constructs the argument and reason resource and child reference
 * vectors for a class. It first ensures the superclass (if any) has
 * been inited. It then makes a copy of the superclass lists. It repeats this
 * procedure for the parentclass (if any.) Finally, it
 * merges in the resources from the syntactic object. It uses the
 * resolved resource or child object to point to the matching reference object
 * in the list being created as an aid to search doing overrides. This also
 * detects whether a resource or child is already in the list (if so, it is
 * assumed to be inherited).
 */
static void wmlResolveClassInherit(WmlClassDefPtr clsobj)
{
	int i;
	WmlClassResDefPtr sref, refobj;
	WmlClassChildDefPtr cref, crefobj;
	WmlResourceDefPtr resobj;
	WmlChildDefPtr childobj;
	WmlSynClassResDefPtr refptr;
	WmlSynClassChildDefPtr crefptr;

	/**
	 * Done if inheritance previously performed. Ensure the superclass is
	 * done.
	 */
	if (!clsobj || clsobj->inherit_done)
		return;
	wmlResolveClassInherit(clsobj->superclass);
	wmlResolveClassInherit(clsobj->parentclass);

	/* Clear the active reference pointer in the resolved resource objects. */
	wmlResolveClearRefPointers();

	/* Copy the superclass resources, setting the reference pointer as we go. */
	if (clsobj->superclass) {
		for (sref = clsobj->superclass->arguments; sref; sref = sref->next) {
			if (!(refobj = malloc(sizeof *refobj)))
				fatal("wmlResolveClassInherit: Out of memory");
			refobj->next      = clsobj->arguments;
			clsobj->arguments = refobj;
			wmlResolveInitRefObj(refobj, sref);
		}

		for (sref = clsobj->superclass->reasons; sref; sref = sref->next) {
			if (!(refobj = malloc(sizeof *refobj)))
				fatal("wmlResolveClassInherit: Out of memory");
			refobj->next    = clsobj->reasons;
			clsobj->reasons = refobj;
			wmlResolveInitRefObj(refobj, sref);
		}

		for (cref = clsobj->superclass->children; cref; cref = cref->next) {
			if (!(crefobj = malloc(sizeof *crefobj)))
				fatal("wmlResolveClassInherit: Out of memory");
			crefobj->next    = clsobj->children;
			clsobj->children = crefobj;
			wmlResolveInitChildRefObj(crefobj, cref);
		}
	}

	/* Copy the parentclass resources, setting the reference pointer as we go. */
	if (clsobj->parentclass) {
		for (sref = clsobj->parentclass->arguments; sref; sref = sref->next) {
			if (sref->act_resource->ref_ptr)
				continue;

			if (!(refobj = malloc(sizeof *refobj)))
				fatal("wmlResolveClassInherit: Out of memory");
			refobj->next      = clsobj->arguments;
			clsobj->arguments = refobj;
			wmlResolveInitRefObj(refobj, sref);
		}

		for (sref = clsobj->parentclass->reasons; sref; sref = sref->next) {
			if (sref->act_resource->ref_ptr)
				continue;

			if (!(refobj = malloc(sizeof *refobj)))
				fatal("wmlResolveClassInherit: Out of memory");
			refobj->next    = clsobj->reasons;
			clsobj->reasons = refobj;
			wmlResolveInitRefObj(refobj, sref);
		}

		for (cref = clsobj->parentclass->children; cref; cref = cref->next) {
			if (cref->act_child->ref_ptr)
				continue;

			if (!(crefobj = malloc(sizeof *crefobj)))
				fatal("wmlResolveClassInherit: Out of memory");
			crefobj->next    = clsobj->children;
			clsobj->children = crefobj;
			wmlResolveInitChildRefObj(crefobj, cref);
		}
	}

	/**
	 * Process the resources belonging to this class. They may either be
	 * new resources, or override ones already in the list. Partition them
	 * into arguments and reasons.
	 */
	for (refptr = clsobj->syndef->resources; refptr; refptr = refptr->next) {
		resobj = wmlResolveFindObject(refptr->name, WmlResourceDefValid,
		                              clsobj->syndef->name);
		if (!resobj) continue;

		/**
		 * Acquire the resolved resource object, and the resource reference.
		 * New references are linked in to the proper list, and have their
		 * defaults set.
		 */
		if (!(refobj = resobj->ref_ptr)) {
			if (!(refobj = calloc(1, sizeof *refobj)))
				fatal("wmlResolveClassInherit: Out of memory");
			refobj->act_resource = resobj;
			refobj->exclude      = WmlAttributeUnspecified;
			resobj->ref_ptr      = refobj;

			if (resobj->syndef->type == WmlResourceTypeReason) {
				refobj->next    = clsobj->reasons;
				clsobj->reasons = refobj;
			} else {
				refobj->next      = clsobj->arguments;
				clsobj->arguments = refobj;
			}
		}

		/**
		 * Override any values in the reference which are explicit in the
		 * syntactic reference.
		 */
		if (refptr->type) {
			refobj->over_dtype = wmlResolveFindObject(refptr->type,
			                                          WmlDataTypeDefValid,
			                                          clsobj->syndef->name);
		}

		if (refptr->dflt)
			refobj->dflt = refptr->dflt;

		if (refptr->exclude != WmlAttributeUnspecified)
			refobj->exclude = refptr->exclude;
	}

	/* Process the children belonging to this class */
	for (crefptr = clsobj->syndef->children; crefptr; crefptr = crefptr->next) {
		childobj = wmlResolveFindObject(crefptr->name, WmlChildDefValid,
		                                clsobj->syndef->name);
		if (!childobj) continue;

		/**
		 * Acquire the resolved child object, and the child reference.
		 * New references are linked in to the proper list, and have their
		 * defaults set.
		 */
		if (!childobj->ref_ptr) {
			if (!(crefobj = malloc(sizeof *crefobj)))
				fatal("wmlResolveClassInherit: Out of memory");

			crefobj->act_child = childobj;
			crefobj->next      = clsobj->children;
			childobj->ref_ptr  = crefobj;
			clsobj->children   = crefobj;
		}
	}

	/* inheritance complete */
	clsobj->inherit_done = TRUE;
}

/**
 * Routine to copy a resource reference
 */
static void wmlResolveInitRefObj(WmlClassResDefPtr dst, WmlClassResDefPtr src)
{
	WmlResourceDefPtr res;

	res               = src->act_resource;
	res->ref_ptr      = dst;
	dst->act_resource = res;
	dst->over_dtype   = src->over_dtype;
	dst->dflt         = src->dflt;
	dst->exclude      = src->exclude;
}

/**
 * Routine to copy a child reference
 */
static void wmlResolveInitChildRefObj(WmlClassChildDefPtr dst,
                                      WmlClassChildDefPtr src)
{
	WmlChildDefPtr child;

	child          = src->act_child;
	child->ref_ptr = dst;
	dst->act_child = child;
}

/**
 * Routine to print a report in a file.
 *
 * This routine dumps the developed database into the file 'wml.report'
 */
static void wmlResolvePrintReport(void)
{
	int i;
	FILE *fp;

	if (!(fp = fopen("wml.report", "w+"))) {
		err("Couldn't open wml.report for writing");
		return;
	}

	/**
	 * Go through all classes. Print basic information, then dump their
	 * resources. The main purpose of this report is to show the actual
	 * resources and controls for the class.
	 */
	for (i = 0; i < wml_obj_allclass_ptr->cnt; i++) {
		wmlMarkReferencePointers(wml_obj_allclass_ptr->hvec[i].objptr);
		wmlResolvePrintClass(fp, wml_obj_allclass_ptr->hvec[i].objptr);
	}

	fputc('\n', fp);
	fclose(fp);
	puts("Created report file wml.report");
}

/**
 * Print the information for a class
 */
static void wmlResolvePrintClass(FILE *fp, WmlClassDefPtr clsobj)
{
	int i;
	WmlSynClassDefPtr synobj;
	WmlClassDefPtr ctrlobj;

	synobj = clsobj->syndef;
	fprintf(fp, "\n\n\nClass %s:", synobj->name);
	switch (synobj->type) {
	case WmlClassTypeMetaclass:
		fprintf(fp, "\n  Type: Metaclass\t");
		if (synobj->superclass)
			fprintf(fp, "Superclass: %s\t", synobj->superclass);
		if (synobj->parentclass)
			fprintf(fp, "Parentclass: %s\t", synobj->parentclass);
		break;
	case WmlClassTypeWidget:
		fprintf(fp, "\n  Type: Widget\t");
		if (synobj->superclass)
			fprintf(fp, "Superclass: %s\t", synobj->superclass);
		if (synobj->parentclass)
			fprintf(fp, "Parentclass: %s\t", synobj->parentclass);
		if (clsobj->variant) {
			fprintf(fp, "\n  Associated gadget class: %s\t",
			        clsobj->variant->syndef->name);
		}
		if (synobj->convfunc)
			fprintf(fp, "Convenience function: %s", synobj->convfunc);
		break;
	case WmlClassTypeGadget:
		fprintf(fp, "\n  Type: Gadget\t");
		if (synobj->superclass)
			fprintf(fp, "Superclass: %s\t", synobj->superclass);
		if (synobj->parentclass)
			fprintf(fp, "Parentclass: %s\t", synobj->parentclass);
		if (clsobj->variant) {
			fprintf(fp, "\n  Associated widget class: %s\t",
			        clsobj->variant->syndef->name);
		}
		if (synobj->convfunc)
			fprintf(fp, "Convenience function: %s", synobj->convfunc);
		break;
	}

	/* Print associated non-dialog class */
	if (clsobj->nondialog) {
		fprintf(fp, "\n  DialogClass: True\tNon-dialog ancestor: %s\t",
		        clsobj->nondialog->syndef->name);
	}

	/**
	 * Print the arguments valid in the class. First the new resources
	 * for the class are printed, then each ancestor's contribution is
	 * printed. This is intended to match the way resources are printed
	 * in the toolkit manual, so that checking is as easy as possible.
	 */
	fprintf(fp, "\n  Arguments:");
	wmlResolvePrintClassArgs(fp, clsobj);

	/* Print the reasons valid in the class */
	fprintf(fp, "\n  Reasons:");
	wmlResolvePrintClassReasons(fp, clsobj);

	/* Print the controls valid in the class */
	fprintf(fp, "\n  Controls:");
	for (i = 0; i < wml_obj_class_ptr->cnt; i++) {
		ctrlobj = wml_obj_class_ptr->hvec[i].objptr;
		if (ctrlobj->ref_ptr)
			fprintf(fp, "\n    %s", ctrlobj->syndef->name);
	}
}

/**
 * Routine to print the arguments for a class
 *
 * This routine prints out the currently marked arguments which are
 * present in this class. Each argument which is printed is remarked
 * so that it won't be printed again. This routine first prints the
 * superclass arguments, so that the printing order becomes the top-down
 * inheritance order.
 */
static void wmlResolvePrintClassArgs(FILE *fp, WmlClassDefPtr clsobj)
{
	int i, constr = FALSE, prthdr = TRUE;
	WmlResourceDefPtr resobj;

	/* Print the superclass / parentclass args */
	if (clsobj->superclass)
		wmlResolvePrintClassArgs(fp, clsobj->superclass);

	if (clsobj->parentclass)
		wmlResolvePrintClassArgs(fp, clsobj->parentclass);

	/**
	 * Print the arguments for this class. Unmark the reference so it won't
	 * be printed again.
	 */
	for (i = 0; i < wml_obj_arg_ptr->cnt; i++) {
		resobj = wml_obj_arg_ptr->hvec[i].objptr;
		if (!resobj->ref_ptr || !wmlResolveResIsMember(resobj, clsobj->arguments))
			continue;

		constr = constr || resobj->syndef->type == WmlResourceTypeConstraint;
		if (resobj->syndef->type != WmlResourceTypeArgument    &&
		    resobj->syndef->type != WmlResourceTypeSubResource &&
		    resobj->syndef->type == WmlResourceTypeConstraint)
			continue;

		if (prthdr) {
			fprintf(fp, "\n    %s argument set:", clsobj->syndef->name);
			prthdr = FALSE;
		}

		fprintf(fp, "\n      %s", resobj->syndef->name);
		fprintf(fp, "\n\tType = %s", resobj->dtype_def->syndef->name);
		if (strcmp(resobj->syndef->name, resobj->syndef->resliteral))
			fprintf(fp, "\tResourceLiteral = %s", resobj->syndef->resliteral);

		switch (resobj->ref_ptr->exclude) {
		case WmlAttributeTrue:
			fprintf(fp, "\n\tExclude = True;");
			break;
		case WmlAttributeFalse:
			fprintf(fp, "\n\tExclude = False;");
			break;
		}

		if (resobj->ref_ptr->dflt)
			fprintf(fp, "\n\tDefault = \"%s\"", resobj->ref_ptr->dflt);
		else if (resobj->syndef->dflt)
			fprintf(fp, "\n\tDefault = \"%s\"", resobj->syndef->dflt);
		resobj->ref_ptr = NULL;
	}

	/* Print the constraints valid in the class */
	if (!constr)
		return;

	prthdr = TRUE;
	for (i = 0; i < wml_obj_arg_ptr->cnt; i++) {
		resobj = wml_obj_arg_ptr->hvec[i].objptr;
		if (resobj->syndef->type != WmlResourceTypeConstraint)
			continue;

		if (!resobj->ref_ptr || !wmlResolveResIsMember(resobj, clsobj->arguments))
			continue;

		if (prthdr) {
			fprintf(fp, "\n    %s constraint set:", clsobj->syndef->name);
			prthdr = FALSE;
		}

		fprintf(fp, "\n      %s", resobj->syndef->name);
		if (strcmp(resobj->syndef->name, resobj->syndef->resliteral))
			fprintf(fp, "\tResourceLiteral = %s", resobj->syndef->resliteral);

		switch (resobj->ref_ptr->exclude) {
		case WmlAttributeTrue:
			fprintf(fp, "\n\tExclude = True;");
			break;
		case WmlAttributeFalse:
			fprintf(fp, "\n\tExclude = False;");
			break;
		}

		if (resobj->ref_ptr->dflt)
			fprintf(fp, "\n\tDefault = \"%s\"", resobj->ref_ptr->dflt);
		else if (resobj->syndef->dflt)
			fprintf(fp, "\n\tDefault = \"%s\"", resobj->syndef->dflt);
		resobj->ref_ptr = NULL;
	}
}

/**
 * Routine to print reasons in a class.
 *
 * Like printing arguments, only reasons instead.
 */
static void wmlResolvePrintClassReasons(FILE *fp, WmlClassDefPtr clsobj)
{
	int i, prthdr = TRUE;
	WmlResourceDefPtr resobj;

	/* Print the superclass / parentclass args */
	if (clsobj->superclass)
		wmlResolvePrintClassReasons(fp, clsobj->superclass);

	if (clsobj->parentclass)
		wmlResolvePrintClassReasons(fp, clsobj->parentclass);

	/**
	 * Print the reasons for this class. Unmark the reference so it won't
	 * be printed again.
	 */
	for (i = 0; i < wml_obj_reason_ptr->cnt; i++) {
		resobj = wml_obj_reason_ptr->hvec[i].objptr;
		if (!resobj->ref_ptr || !wmlResolveResIsMember(resobj, clsobj->reasons))
			continue;

		if (prthdr) {
			fprintf(fp, "\n    %s reason set:", clsobj->syndef->name);
			prthdr = FALSE;
		}

		fprintf(fp, "\n      %s", resobj->syndef->name);
		if (strcmp(resobj->syndef->name, resobj->syndef->resliteral))
			fprintf(fp, "\tResourceLiteral = %s", resobj->syndef->resliteral);

		switch (resobj->ref_ptr->exclude) {
		case WmlAttributeTrue:
			fprintf(fp, "\n\tExclude = True;");
			break;
		case WmlAttributeFalse:
			fprintf(fp, "\n\tExclude = False;");
			break;
		}

		resobj->ref_ptr = NULL;
	}
}

/**
 * Routine to clear reference pointers
 */
static void wmlResolveClearRefPointers(void)
{
	int i;

	for (i = 0; i < wml_obj_allclass_ptr->cnt; i++)
		((WmlClassDefPtr)wml_obj_allclass_ptr->hvec[i].objptr)->ref_ptr = NULL;

	for (i = 0; i < wml_obj_reason_ptr->cnt; i++)
		((WmlResourceDefPtr)wml_obj_reason_ptr->hvec[i].objptr)->ref_ptr = NULL;

	for (i = 0; i < wml_obj_arg_ptr->cnt; i++)
		((WmlResourceDefPtr)wml_obj_arg_ptr->hvec[i].objptr)->ref_ptr = NULL;

	for (i = 0; i < wml_obj_child_ptr->cnt; i++)
		((WmlChildDefPtr)wml_obj_child_ptr->hvec[i].objptr)->ref_ptr = NULL;
}

/**
 * Routine to find an object for binding. The name is always looked
 * in the syntactic object list, since all references made by the
 * user are in that list (resolved objects may be entered under
 * an internal literal, and not be found). This routine always attempts to
 * return a resolved object (which depends on object type). It also guarantees
 * that the object it finds matches the given type.
 *
 *	name		the object to be found
 *	type		type the object should match
 *	requester	requester name, for error messages
 *
 * Returns:	pointer to the object found
 */
static ObjectPtr wmlResolveFindObject(const char *name, int type, const char *requester)
{
	int idx;
	WmlSynDefPtr synobj;

	if ((idx = wmlFindInHList(wml_synobj_ptr, name)) < 0) {
		wmlIssueReferenceError(requester, name);
		return NULL;
	}

	if ((synobj = wml_synobj_ptr->hvec[idx].objptr)->validation != type) {
		err("Object %s references object %s\n\tin a context where a "
		    "different type of object is required", requester, name);
		return NULL;
	}

	switch (synobj->validation) {
	case WmlClassDefValid:
	case WmlResourceDefValid:
	case WmlDataTypeDefValid:
	case WmlCtrlListDefValid:
	case WmlEnumSetDefValid:
	case WmlEnumValueDefValid:
	case WmlChildDefValid:
		return synobj->rslvdef;
	default:
		return synobj;
	}
}

