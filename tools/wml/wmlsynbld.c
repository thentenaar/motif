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
static char rcsid[] = "$XConsortium: wmlsynbld.c /main/9 1995/08/29 11:11:12 drk $"
#endif
#endif

/*
 * This module contains the programs which construct the syntactic
 * representation of the WML input. All the routines are called as
 * actions of the grammar productions.
 *
 * Since WML is so simple, no stack frame technology is used. Instead,
 * context is maintained by global pointers and vectors which contain
 * the intermediate results of parsing a statement. At most, these
 * contain an object being constructed (for instance a class descriptor)
 * and a subobject (for instance a resource reference in a class).
 *
 * Results are communicated back using the global error count
 * wml_err_count, and the ordered handle list wml_synobj_ptr.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "wml.h"
#include "wmlparse.h"
#include "wmlnoreturn.h"

/**
 * Globals used during WML parsing.
 *
 * Character arrays and other variables to hold lexemes
 * are defined in wmllex.l
 */

/* Current principal object and subobject being constructed */
ObjectPtr wml_cur_obj;
ObjectPtr wml_cur_subobj;

/**
 * Give context around errors generated here
 */
static void err(const char *fmt, ...)
{
	va_list ap;

	fputc('\n', stderr);
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);

	fprintf(stderr, "\n\tnear input line %d", wml_line_count);
	wml_err_count++;
}

/**
 * Exit on fatal errors
 */
noreturn static void fatal(const char *fmt, ...)
{
	va_list ap;

	fputc('\n', stderr);
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);

	fprintf(stderr, "\n\tnear input line %d", wml_line_count);
	exit(EXIT_FAILURE);
}

/**
 * The error reporting routine.
 *
 * For now, issue a very simple error message
 */
void LexIssueError(int token)
{
	switch (token) {
	case SEMICOLON:
		err("Syntax error: expected a semicolon");
		break;
	case RBRACE:
		err("Syntax error: expected a right brace");
		break;
	case 0:
		err("Syntax error: couldn't recognize a section name, probably fatal");
		break;
	default:
		err("Unexpected Syntax error");
	}

	fprintf(stderr, "\n\tnear name='%s', value='%s'", yynameval, yystringval);
}

/**
 * Routine to create a class descriptor. The result is placed in both
 * wml_cur_obj and wml_synobj.
 *
 *	name		the class name
 *	ctype		class type, one of METACLASS | WIDGET | GADGET
 */
void wmlCreateClass(const char *name, int ctype)
{
	WmlSynClassDefPtr cdesc;

	/**
	 * Initialize the new class descriptor. Enter it in the object list.
	 * Set the current object global to the descriptor.
	 */
	if (!(cdesc = calloc(1, sizeof *cdesc)))
		fatal("wmlCreateClass: Out of memory");

	cdesc->rslvdef    = NULL;
	cdesc->validation = WmlClassDefValid;
	if (!(cdesc->name = wmlAllocateString(name)))
		fatal("wmlCreateClass: Out of memory allocating name");

	switch (ctype) {
	case METACLASS: cdesc->type = WmlClassTypeMetaclass; break;
	case WIDGET:    cdesc->type = WmlClassTypeWidget;    break;
	case GADGET:    cdesc->type = WmlClassTypeGadget;    break;
	default:
		err("wmlCreateClass: unknown class type %d", ctype);
		free(cdesc->name);
		free(cdesc);
		return;
	}

	if (!wmlInsertInHList(wml_synobj_ptr, name, cdesc)) {
		err("wmlCreateClass: failed to insert %s", name);
		free(cdesc->name);
		free(cdesc);
		return;
	}

	wml_cur_obj    = cdesc;
	wml_cur_subobj = NULL;
}

/**
 * Routine to create a resource descriptor. The result is placed in both
 * wml_cur_obj and wml_synobj.
 *
 *	name		the resource name
 *	rtype		resource type, one of
 *			ARGUMENT | REASON | CONSTRAINT | SUBRESOURCE
 */
void wmlCreateResource(const char *name, int rtype)
{
	WmlSynResourceDefPtr rdesc;

	/**
	 * Initialize the new resource descriptor. Enter it in the object list.
	 * Set the current object global to the descriptor.
	 */
	if (!(rdesc = calloc(1, sizeof *rdesc)))
		fatal("wmlCreateResource: Out of memory");

	if (!(rdesc->name = wmlAllocateString(name)))
		fatal("wmlCreateResource: Out of memory allocating name");

	if (!(rdesc->resliteral = wmlAllocateString(name)))
		fatal("wmlCreateResource: Out of memory allocating resliteral");

	rdesc->validation = WmlResourceDefValid;
	switch (rtype) {
	case ARGUMENT:
		rdesc->type = WmlResourceTypeArgument;
		rdesc->xrm_support = WmlAttributeTrue;
		break;
	case REASON:
		rdesc->type = WmlResourceTypeReason;
		break;
	case CONSTRAINT:
		rdesc->type = WmlResourceTypeConstraint;
		rdesc->xrm_support = WmlAttributeTrue;
		break;
	case SUBRESOURCE:
		rdesc->type = WmlResourceTypeSubResource;
		rdesc->xrm_support = WmlAttributeTrue;
		break;
	default:
		err("wmlCreateResource: unknown resource type %d", rtype);
		free(rdesc->resliteral);
		free(rdesc->name);
		free(rdesc);
		return;

	}

	if (!wmlInsertInHList(wml_synobj_ptr, name, rdesc)) {
		err("wmlCreateResource: failed to insert %s", name);
		free(rdesc->resliteral);
		free(rdesc->name);
		free(rdesc);
		return;
	}

	wml_cur_obj    = rdesc;
	wml_cur_subobj = NULL;
}

/**
 * Routine to create a datatype descriptor. The result is placed in both
 * wml_cur_obj and wml_synobj.
 *
 *	name		the datatype name
 */
void wmlCreateDatatype(const char *name)
{
	WmlSynDataTypeDefPtr ddesc;

	/**
	 * Initialize the new datatype descriptor. Enter it in the object list.
	 * Set the current object global to the descriptor.
	 */
	if (!(ddesc = calloc(1, sizeof *ddesc)))
		fatal("wmlCreateDatatype: Out of memory");

	if (!(ddesc->name = wmlAllocateString(name)))
		fatal("wmlCreateDatatype: Out of memory allocating name");

	ddesc->validation  = WmlDataTypeDefValid;
	ddesc->xrm_support = WmlAttributeTrue;
	if (!wmlInsertInHList(wml_synobj_ptr, name, ddesc)) {
		err("wmlCreateDatatype: failed to insert %s", name);
		free(ddesc->name);
		free(ddesc);
		return;
	}

	wml_cur_obj    = ddesc;
	wml_cur_subobj = NULL;
}

/**
 * Routine to create a child descriptor. The result is placed in both
 * wml_cur_obj and wml_synobj.
 *
 *	name		the child name
 *	class		the class name
 */
void wmlCreateChild(const char *name, const char *class)
{
	WmlSynChildDefPtr chdesc;

	/*
	 * Initialize the new child descriptor. Enter it in the object list.
	 * Set the current object global to the descriptor.
	 */
	if (!(chdesc = calloc(1, sizeof *chdesc)))
		fatal("wmlCreateChild: Out of memory");

	if (!(chdesc->name = wmlAllocateString(name)))
		fatal("wmlCreateChild: Out of memory allocating name");

	if (!(chdesc->class = wmlAllocateString(class)))
		fatal("wmlCreateChild: Out of memory allocating class");

	chdesc->validation = WmlChildDefValid;
	if (!wmlInsertInHList(wml_synobj_ptr, name, chdesc)) {
		err("wmlCreateChild: failed to insert %s", name);
		free(chdesc->class);
		free(chdesc->name);
		free(chdesc);
		return;
	}

	wml_cur_obj    = chdesc;
	wml_cur_subobj = NULL;
}

/**
 * Routine to create a controls list descriptor. The result is placed in both
 * wml_cur_obj and wml_synobj.
 *
 *	name		the controls list name
 */
void wmlCreateOrAppendCtrlList(const char *name)
{
	int i;
	WmlSynCtrlListDefPtr cdesc;

	if ((i = wmlFindInHList(wml_synobj_ptr, name)) != -1) {
		printf("\nAppending to list name %s", name);
		wml_cur_obj    = wml_synobj_ptr->hvec[i].objptr;
		wml_cur_subobj = NULL;
		return;
	}

	/**
	 * Initialize the new CtrlList descriptor. Enter it in the object list.
	 * Set the current object global to the descriptor.
	 */
	if (!(cdesc = calloc(1, sizeof *cdesc)))
		fatal("wmlCreateOrAppendCtrlList: Out of memory");

	if (!(cdesc->name = wmlAllocateString(name)))
		fatal("wmlCreateOrAppendCtrlList: Out of memory allocating name");

	cdesc->validation = WmlCtrlListDefValid;
	if (!wmlInsertInHList(wml_synobj_ptr, name, cdesc)) {
		err("wmlCreateOrAppendCtrlList: failed to insert %s", name);
		free(cdesc->name);
		free(cdesc);
		return;
	}

	wml_cur_obj    = cdesc;
	wml_cur_subobj = NULL;
}

/**
 * Routine to create an enumeration set descriptor. The result is placed in both
 * wml_cur_obj and wml_synobj.
 *
 *	name		the enumeration set name
 *	type		data type, must match a data type name
 */
void wmlCreateEnumSet(const char *name, const char *dtype)
{
	WmlSynEnumSetDefPtr esdesc;

	if (wmlFindInHList(wml_synobj_ptr, name) != -1) {
		err("Duplicate name %s found", name);
		return;
	}

	/**
	 * Initialize the new enum set descriptor. Enter it in the object list.
	 * Set the current object global to the descriptor.
	 */
	if (!(esdesc = calloc(1, sizeof *esdesc)))
		fatal("wmlCreateEnumSet: Out of memory");

	if (!(esdesc->name = wmlAllocateString(name)))
		fatal("wmlCreateEnumSet: Out of memory allocating name");

	if (!(esdesc->datatype = wmlAllocateString(dtype)))
		fatal("wmlCreateEnumSet: Out of memory allocating datatype");

	esdesc->validation = WmlEnumSetDefValid;
	if (!wmlInsertInHList(wml_synobj_ptr, name, esdesc)) {
		err("wmlCreateEnumSet: failed to insert %s", name);
		free(esdesc->datatype);
		free(esdesc->name);
		free(esdesc);
		return;
	}

	wml_cur_obj    = esdesc;
	wml_cur_subobj = NULL;
}

/**
 * Routine to create an enumeration value descriptor. The result is placed in both
 * wml_cur_obj and wml_synobj.
 *
 *	name		the enumeration value name
 */
void wmlCreateEnumValue(const char *name)
{
	WmlSynEnumValueDefPtr evdesc;

	if (wmlFindInHList(wml_synobj_ptr, name) != -1) {
		err("Duplicate name %s found", name);
		return;
	}

	/**
	 * Initialize the new enum value descriptor. Enter it in the object list.
	 * Set the current object global to the descriptor.
	 */
	if (!(evdesc = calloc(1, sizeof *evdesc)))
		fatal("wmlCreateEnumValue: Out of memory");

	if (!(evdesc->name = wmlAllocateString(name)))
		fatal("wmlCreateEnumValue: Out of memory allocating name");

	if (!(evdesc->enumlit = wmlAllocateString(name)))
		fatal("wmlCreateEnumValue: Out of memory allocating enumlit");

	evdesc->validation = WmlEnumValueDefValid;
	if (!wmlInsertInHList(wml_synobj_ptr, name, evdesc)) {
		err("wmlCreateEnumValue: failed to insert %s", name);
		free(evdesc->enumlit);
		free(evdesc->name);
		free(evdesc);
		return;
	}

	wml_cur_obj    = evdesc;
	wml_cur_subobj = NULL;
}

/**
 * Routine to create a charset descriptor. The result is placed in both
 * wml_cur_obj and wml_synobj.
 *
 *	name		the charset name
 */
void wmlCreateCharset(const char *name)
{
	WmlSynCharSetDefPtr csdesc;

	if (wmlFindInHList(wml_synobj_ptr, name) != -1) {
		err("Duplicate name %s found", name);
		return;
	}

	/**
	 * Initialize the new charset descriptor. Enter it in the object list.
	 * Set the current object global to the descriptor.
	 */
	if (!(csdesc = calloc(1, sizeof *csdesc)))
		fatal("wmlCreateCharset: Out of memory");

	if (!(csdesc->name = wmlAllocateString(name)))
		fatal("wmlCreateCharset: Out of memory allocating name");

	csdesc->direction      = WmlCharSetDirectionLtoR;
	csdesc->parsedirection = WmlAttributeUnspecified;
	csdesc->charsize       = WmlCharSizeOneByte;
	csdesc->validation     = WmlCharSetDefValid;
	if (!wmlInsertInHList(wml_synobj_ptr, name, csdesc)) {
		err("wmlCreateCharset: failed to insert %s", name);
		free(csdesc->name);
		free(csdesc);
		return;
	}

	wml_cur_obj    = csdesc;
	wml_cur_subobj = NULL;
}

/**
 * Routine to set an attribute in a class descriptor (int).
 *
 * This routine sets the given attribute in the current object, which
 * must be a class descriptor. The current object and subobject do not
 * change.
 *
 *	attrid		DIALOGCLASS
 *	val		value of the attribute
 */
void wmlAddClassAttributeInt(int attrid, int val)
{
	WmlSynClassDefPtr cdesc;

	/* Acquire the current class descriptor */
	if (!(cdesc = wml_cur_obj)) {
		err("wmlAddClassAttributeInt: NULL current object");
		return;
	}

	if (cdesc->validation != WmlClassDefValid) {
		err("wmlAddClassAttributeInt: %d not a class descriptor", cdesc->validation);
		return;
	}

	/* Set the appropriate resource */
	if (attrid == DIALOGCLASS && val == ATTRTRUE)
		cdesc->dialog = TRUE;
}

/**
 * Routine to set an attribute in a class descriptor (string).
 *
 * This routine sets the given attribute in the current object, which
 * must be a class descriptor. The current object and subobject do not
 * change.
 *
 *	attrid		oneof SUPERCLASS | INTERNALLITERAL | DOCNAME |
 *			CONVFUNC | WIDGETCLASS | CTRLMAPSRESOURCE
 *	val		value of the attribute
 */
void wmlAddClassAttributeStr(int attrid, const char *val)
{
	char *s;
	char **synlist; /* alias pointer list */
	WmlSynClassDefPtr cdesc;

	/* Acquire the current class descriptor */
	if (!(cdesc = wml_cur_obj)) {
		err("wmlAddClassAttributeStr: NULL current object");
		return;
	}

	if (cdesc->validation != WmlClassDefValid) {
		err("wmlAddClassAttributeStr: %d not a class descriptor", cdesc->validation);
		return;
	}

	if (!(s = wmlAllocateString(val)))
		fatal("wmlAddClassAttributeStr: Out of memory");

	/* Set the appropriate resource */
	switch (attrid) {
	case SUPERCLASS:
		free(cdesc->superclass);
		cdesc->superclass = s;
		break;
	case PARENTCLASS:
		free(cdesc->parentclass);
		cdesc->parentclass = s;
		break;
	case INTERNALLITERAL:
		free(cdesc->int_lit);
		cdesc->int_lit = s;
		break;
	case CONVFUNC:
		free(cdesc->convfunc);
		cdesc->convfunc = s;
		break;
	case DOCNAME:
		free(cdesc->docname);
		cdesc->docname = s;
		break;
	case WIDGETCLASS:
		free(cdesc->widgetclass);
		cdesc->widgetclass = s;
		break;
	case CTRLMAPSRESOURCE:
		free(cdesc->ctrlmapto);
		cdesc->ctrlmapto = s;
		break;
	case ALIAS:
		if (!(synlist = realloc(cdesc->alias_list, (1 + cdesc->alias_cnt) * sizeof(char *))))
			fatal("wmlAddClassAttributeStr: ALIAS: Out of memory");

		synlist[cdesc->alias_cnt++] = s;
		cdesc->alias_list = synlist;
		break;
	default:
		free(s);
		err("wmlAddClassAttributeStr: Unknown attribute id %d", attrid);
	}
}

/**
 * Routine to add a control specification to the current class.
 * The current object must be a class descriptor. The entry name
 * is added to the controls list. The control specification becomes the
 * current subobject.
 *
 *	name		the name of the controlled class
 */
void wmlAddClassControl(const char *name)
{
	WmlSynClassDefPtr cdesc;
	WmlSynClassCtrlDefPtr ctrlelem; /* controls element */

	/* Acquire the current class descriptor */
	if (!(cdesc = wml_cur_obj)) {
		err("wmlAddClassControl: NULL current object");
		return;
	}

	if (cdesc->validation != WmlClassDefValid) {
		err("wmlAddClassControl: %d not a class descriptor", cdesc->validation);
		return;
	}

	if (!(ctrlelem = calloc(1, sizeof *ctrlelem)))
		fatal("wmlAddClassControl: Out of memory");

	if (!(ctrlelem->name = wmlAllocateString(name)))
		fatal("wmlAddClassControl: Out of memory allocating name");

	/* Add the control to the control list */
	ctrlelem->validation = WmlClassCtrlDefValid;
	ctrlelem->next       = cdesc->controls;
	cdesc->controls      = ctrlelem;

	/* This becomes the current subobject */
	wml_cur_subobj = ctrlelem;
}

/**
 * Add a resource descriptor to a class.
 * The current object must be a class descriptor. Create and add a
 * resource descriptor, which becomes the current subobject. It is not
 * entered in the named object list.
 *
 *	name		the resource name
 */
void wmlAddClassResource(const char *name)
{
	WmlSynClassDefPtr cdesc;
	WmlSynClassResDefPtr rdesc; /* resource reference descriptor */

	/* Acquire the current class descriptor */
	if (!(cdesc = wml_cur_obj)) {
		err("wmlAddClassResource: NULL current object");
		return;
	}

	if (cdesc->validation != WmlClassDefValid) {
		err("wmlAddClassResource: %d not a class descriptor", cdesc->validation);
		return;
	}

	if (!(rdesc = calloc(1, sizeof *rdesc)))
		fatal("wmlAddClassControl: Out of memory");

	if (!(rdesc->name = wmlAllocateString(name)))
		fatal("wmlAddClassResource: Out of memory allocating name");

	/* Add the resource to the resource list */
	rdesc->validation = WmlClassResDefValid;
	rdesc->exclude    = WmlAttributeUnspecified;
	rdesc->next       = cdesc->resources;
	cdesc->resources  = rdesc;

	/* This becomes the current subobject */
	wml_cur_subobj = rdesc;
}

/**
 * Add a child descriptor to a class.
 * The current object must be a class descriptor. Create and add a
 * child descriptor, which becomes the current subobject. It is not
 * entered in the named object list.
 *
 *	name		the resource name
 */
void wmlAddClassChild(const char *name)
{
	WmlSynClassDefPtr cdesc;
	WmlSynClassChildDefPtr chdesc;

	/* Acquire the current class descriptor */
	if (!(cdesc = wml_cur_obj)) {
		err("wmlAddClassChild: NULL current object");
		return;
	}

	if (cdesc->validation != WmlClassDefValid) {
		err("wmlAddClassChild: %d not a class descriptor", cdesc->validation);
		return;
	}

	if (!(chdesc = calloc(1, sizeof *chdesc)))
		fatal("wmlAddClassChild: Out of memory");

	if (!(chdesc->name = wmlAllocateString(name)))
		fatal("wmlAddClassChild: Out of memory allocating name");

	/* Add the child to the child list */
	chdesc->validation = WmlClassChildDefValid;
	chdesc->next       = cdesc->children;
	cdesc->children    = chdesc;

	/* This becomes the current subobject */
	wml_cur_subobj = chdesc;
}

/**
 * This routine sets an attribute in the current class resource descriptor.
 * The current subobject must be a class resource descriptor. The
 * named attribute is set.
 *
 *	attrid		EXCLUDE
 *	val		attribute value (ATTRTRUE or ATTRFALSE for EXCLUDE)
 */
void wmlAddClassResourceAttributeInt(int attrid, int val)
{
	WmlSynClassResDefPtr rdesc;

	/* Acquire the descriptor from the current subobject */
	if (!(rdesc = wml_cur_subobj)) {
		err("wmlAddClassResourceAttributeInt: NULL current subobject");
		return;
	}

	if (rdesc->validation != WmlClassResDefValid) {
		err("wmlAddClassResourceAttributeInt: %d not a class resource descriptor", rdesc->validation);
		return;
	}

	switch (attrid) {
	case EXCLUDE:
		switch (val) {
		case ATTRTRUE:  rdesc->exclude = WmlAttributeTrue;  break;
		case ATTRFALSE: rdesc->exclude = WmlAttributeFalse; break;
		default:
			err("wmlAddClassResourceAttributeInt: Unknown value for EXCLUDE %d", val);
		}
		break;
	default:
		err("wmlAddClassResourceAttributeInt: unknown attribute id %d", attrid);
	}

}

/**
 * This routine sets an attribute in the current class resource descriptor.
 * The current subobject must be a class resource descriptor. The
 * named attribute is set.
 *
 *	attrid		one of TYPE | DEFAULT
 *	val		attribute value.
 */
void wmlAddClassResourceAttributeStr(int attrid, const char *val)
{
	char *s;
	WmlSynClassResDefPtr rdesc;

	/* Acquire the descriptor from the current subobject */
	if (!(rdesc = wml_cur_subobj)) {
		err("wmlAddClassResourceAttributeStr: NULL current subobject");
		return;
	}

	if (rdesc->validation != WmlClassResDefValid) {
		err("wmlAddClassResourceAttributeStr: %d not a class resource descriptor", rdesc->validation);
		return;
	}

	if (!(s = wmlAllocateString(val)))
		fatal("wmlAddClassResourceAttributeStr: Out of memory");

	switch (attrid) {
	case TYPE:
		free(rdesc->type);
		rdesc->type = s;
		break;
	case DEFAULT:
		free(rdesc->dflt);
		rdesc->dflt = s;
		break;
	default:
		free(s);
		err("wmlAddClassResourceAttribute: unknown attribute id %d", attrid);
	}
}

/**
 * Routine to set an attribute in a resource descriptor.
 *
 * This routine sets the given attribute in the current object, which
 * must be a resource descriptor. The current object and subobject do not
 * change.
 *
 *	attrid		XRMRESOURCE
 *	val		value of the attribute (int)
 */
void wmlAddResourceAttributeInt(int attrid, int val)
{
	WmlSynResourceDefPtr rdesc;

	/* Acquire the current resource descriptor */
	if (!(rdesc = wml_cur_obj)) {
		err("wmlAddResourceAttributeInt: NULL current object");
		return;
	}

	if (rdesc->validation != WmlResourceDefValid) {
		err("wmlAddResourceAttributeInt: %d not a resource descriptor", rdesc->validation);
		return;
	}

	/* Set the appropriate resource */
	switch (attrid) {
	case XRMRESOURCE:
		switch (val) {
		case ATTRTRUE:  rdesc->xrm_support = WmlAttributeTrue;
		case ATTRFALSE: rdesc->xrm_support = WmlAttributeFalse;
		default:
			err("wmlAddResourceAttributeInt: Bad XRMRESOURCE value %d", val);
		}
		break;
	default:
		err("wmlAddResourceAttributeInt: Unknown attribute id %d", attrid);
	}
}

/**
 * Routine to set an attribute in a resource descriptor.
 *
 * This routine sets the given attribute in the current object, which
 * must be a resource descriptor. The current object and subobject do not
 * change.
 *
 *	attrid		oneof TYPE | RESOURCELITERAL | INTERNALLITERAL |
 *			RELATED | DOCNAME | DEFAULT | ALIAS | ENUMERATIONSET
 *	val		value of the attribute (str)
 */
void wmlAddResourceAttributeStr(int attrid, const char *val)
{
	char *s;
	char **synlist; /* alias pointer list */
	WmlSynResourceDefPtr rdesc;

	/* Acquire the current resource descriptor */
	if (!(rdesc = wml_cur_obj)) {
		err("wmlAddResourceAttributeStr: NULL current object");
		return;
	}

	if (rdesc->validation != WmlResourceDefValid) {
		err("wmlAddResourceAttributeStr: %d not a resource descriptor", rdesc->validation);
		return;
	}

	if (!(s = wmlAllocateString(val)))
		fatal("wmlAddResourceAttributeStr: Out of memory");

	/* Set the appropriate resource */
	switch (attrid) {
	case TYPE:
		free(rdesc->datatype);
		rdesc->datatype = s;
		break;
	case INTERNALLITERAL:
		free(rdesc->int_lit);
		rdesc->int_lit = s;
		break;
	case RESOURCELITERAL:
		free(rdesc->resliteral);
		rdesc->resliteral = s;
		break;
	case ENUMERATIONSET:
		free(rdesc->enumset);
		rdesc->enumset = s;
		break;
	case DOCNAME:
		free(rdesc->docname);
		rdesc->docname = s;
		break;
	case RELATED:
		free(rdesc->related);
		rdesc->related = s;
		break;
	case DEFAULT:
		free(rdesc->dflt);
		rdesc->dflt = s;
		break;
	case ALIAS:
		if (!(synlist = realloc(rdesc->alias_list, (1 + rdesc->alias_cnt) * sizeof(char *))))
			fatal("wmlAddResourceAttributeStr: ALIAS: Out of memory");

		synlist[rdesc->alias_cnt++] = s;
		rdesc->alias_list = synlist;
		break;
	default:
		free(s);
		err("wmlAddResourceAttributeStr: Unknown attribute id %d", attrid);
	}
}

/**
 * Routine to set an attribute in a datatype descriptor.
 *
 * This routine sets the given attribute in the current object, which
 * must be a datatype descriptor. The current object and subobject do not
 * change.
 *
 *	attrid		oneof INTERNALLITERAL | DOCNAME
 *	val		value of the attribute (str)
 */
void wmlAddDatatypeAttribute(int attrid, const char *val)
{
	char *s;
	WmlSynDataTypeDefPtr ddesc;

	/* Acquire the current datatype descriptor */
	if (!(ddesc = wml_cur_obj)) {
		err("wmlAddDatatypeAttribute: NULL current object");
		return;
	}

	if (ddesc->validation != WmlDataTypeDefValid) {
		err("wmlAddDatatypeAttribute: %d not a datatype descriptor", ddesc->validation);
		return;
	}

	if (!(s = wmlAllocateString(val)))
		fatal("wmlAddDatatypeAttribute: Out of memory");

	/* Set the appropriate slot */
	switch (attrid) {
	case INTERNALLITERAL:
		free(ddesc->int_lit);
		ddesc->int_lit = s;
		break;
	case DOCNAME:
		free(ddesc->docname);
		ddesc->docname = s;
		break;
	default:
		free(s);
		err("wmlAddDatatypeAttribute: Unknown attribute id %d", attrid);
	}
}

/**
 * Routine to add a control specification to the current controls list.
 * The current object must be a controls list descriptor. The entry name
 * is added to the controls list. The new element becomes the current
 * subobject.
 *
 *	name		the name of the controlled class
 */
void wmlAddCtrlListControl(const char *name)
{
	WmlSynCtrlListDefPtr cdesc;
	WmlSynClassCtrlDefPtr ctrlelem;

	/* Acquire the current control list descriptor */
	if (!(cdesc = wml_cur_obj)) {
		err("wmlAddCtrlListControl: NULL current object");
		return;
	}

	if (cdesc->validation != WmlCtrlListDefValid) {
		err("wmlAddCtrlListControl: %d not a control list descriptor", cdesc->validation);
		return;
	}

	if (!(ctrlelem = calloc(1, sizeof *ctrlelem)))
		fatal("wmlAddCtrlListControl: Out of memory");

	if (!(ctrlelem->name = wmlAllocateString(name)))
		fatal("wmlAddCtrlListControl: Out of memory allocating name");

	/* Add the control to the control list */
	ctrlelem->validation = WmlClassCtrlDefValid;
	ctrlelem->next       = cdesc->controls;
	cdesc->controls      = ctrlelem;

	/* This becomes the current subobject */
	wml_cur_subobj = ctrlelem;
}

/**
 * Routine to add an enumeration value to the current enumeration set
 * The current object must be an enumeration set descriptor. The entry name
 * is added to the the enumeration value list.
 *
 *	name		the name of the enumeration value
 */
void wmlAddEnumSetValue(const char *name)
{
	WmlSynEnumSetDefPtr    esdesc; /* the enumeration set descriptor */
	WmlSynEnumSetValDefPtr evelm;  /* EnumSet EnumValue element */

	/* Acquire the current enumeration set descriptor */
	if (!(esdesc = wml_cur_obj)) {
		err("wmlAddEnumSetValue: NULL current object");
		return;
	}

	if (esdesc->validation != WmlEnumSetDefValid) {
		err("wmlAddEnumSetValue: %d not an enumeration set descriptor", esdesc->validation);
		return;
	}

	if (!(evelm = calloc(1, sizeof *evelm)))
		fatal("wmlAddEnumSetValue: Out of memory");

	if (!(evelm->name = wmlAllocateString(name)))
		fatal("wmlAddEnumSetValue: Out of memory allocating name");

	/* Add the value to the set */
	evelm->validation = WmlEnumValueDefValid;
	evelm->next       = esdesc->values;
	esdesc->values    = evelm;

	/* This becomes the current subobject */
	wml_cur_subobj = evelm;
}

/**
 * Routine to set an attribute in an enumeration value
 *
 * This routine sets the given attribute in the current object, which must
 * be an enumeration value descriptor. The current object does not change.
 *
 *	attrid		ENUMLITERAL
 *	val		value of the attribute (str)
 */
void wmlAddEnumValueAttribute(int attrid, const char *val)
{
	char *s;
	WmlSynEnumValueDefPtr evdesc; /* the enumeration value descriptor */

	/* Acquire the current enumeration value descriptor */
	if (!(evdesc = wml_cur_obj)) {
		err("wmlAddEnumValueAttribute: NULL current object");
		return;
	}

	if (evdesc->validation != WmlEnumValueDefValid) {
		err("wmlAddEnumValueAttribute: %d not an enumeration value descriptor", evdesc->validation);
		return;
	}

	if (!(s = wmlAllocateString(val)))
		fatal("wmlAddEnumValueAttribute: Out of memory");

	/* Set the appropriate slot */
	switch (attrid) {
	case ENUMLITERAL:
		free(evdesc->enumlit);
		evdesc->enumlit = s;
		break;
	default:
		free(s);
		err("wmlAddEnumValueAttribute: Unknown attribute id %d", attrid);
	}
}

/**
 * Routine to set an attribute in a charset descriptor.
 *
 * This routine sets the given attribute in the current object, which
 * must be a charset descriptor. The current object and subobject do not
 * change.
 *
 *	attrid		oneof DIRECTION | PARSEDIRECTION | CHARACTERSIZE
 *	val		value of the attribute (int)
 */
void wmlAddCharsetAttributeInt(int attrid, int val)
{
	WmlSynCharSetDefPtr csdesc;

	/* Acquire the current charset descriptor */
	if (!(csdesc = wml_cur_obj)) {
		err("wmlAddCharsetAttributeInt: NULL current object");
		return;
	}

	if (csdesc->validation != WmlCharSetDefValid) {
		err("wmlAddCharsetAttributeInt: %d not a charset descriptor", csdesc->validation);
		return;
	}

	/* Set the appropriate resource */
	switch (attrid) {
	case DIRECTION:
		switch (val) {
		case LEFTTORIGHT: csdesc->direction = WmlCharSetDirectionLtoR; break;
		case RIGHTTOLEFT: csdesc->direction = WmlCharSetDirectionRtoL; break;
		default:
			err("wmlAddCharsetAttributeInt: bad DIRECTION value %d", val);
		}
		break;
	case PARSEDIRECTION:
		switch (val) {
		case LEFTTORIGHT: csdesc->parsedirection = WmlCharSetDirectionLtoR; break;
		case RIGHTTOLEFT: csdesc->parsedirection = WmlCharSetDirectionRtoL; break;
		default:
			err("wmlAddCharsetAttributeInt: bad PARSEDIRECTION value %d", val);
		}
		break;
	case CHARACTERSIZE:
		switch (val) {
		case ONEBYTE:      csdesc->charsize = WmlCharSizeOneByte;      break;
		case TWOBYTE:      csdesc->charsize = WmlCharSizeTwoByte;      break;
		case MIXED1_2BYTE: csdesc->charsize = WmlCharSizeMixed1_2Byte; break;
		default:
			err("wmlAddCharsetAttributeInt: bad CHARACTERSIZE value %d", val);
		}
		break;
	default:
		err("wmlAddCharsetAttributeInt: Unknown attribute id %d", attrid);
	}
}

/**
 * Routine to set an attribute in a charset descriptor.
 *
 * This routine sets the given attribute in the current object, which
 * must be a charset descriptor. The current object and subobject do not
 * change.
 *
 *	attrid		oneof INTERNALLITERAL | ALIAS | XMSTRINGCHARSETNAME
 *	val		value of the attribute (str)
 */
void wmlAddCharsetAttributeStr(int attrid, const char *val)
{
	char *s;
	char **synlist; /* alias pointer list */
	WmlSynCharSetDefPtr csdesc;

	/* Acquire the current charset descriptor */
	if (!(csdesc = wml_cur_obj)) {
		err("wmlAddCharsetAttributeStr: NULL current object");
		return;
	}

	if (csdesc->validation != WmlCharSetDefValid) {
		err("wmlAddCharsetAttributeStr: %d not a charset descriptor", csdesc->validation);
		return;
	}

	if (!(s = wmlAllocateString(val)))
		fatal("wmlAddCharseteAttributeStr: Out of memory");

	/* Set the appropriate resource */
	switch (attrid) {
	case INTERNALLITERAL:
		free(csdesc->int_lit);
		csdesc->int_lit = s;
		break;
	case XMSTRINGCHARSETNAME:
		free(csdesc->xms_name);
		csdesc->xms_name = s;
		break;
	case ALIAS:
		if (!(synlist = realloc(csdesc->alias_list, (1 + csdesc->alias_cnt) * sizeof(char *))))
			fatal("wmlAddCharsetAttributeStr: ALIAS: Out of memory");

		synlist[csdesc->alias_cnt++] = s;
		csdesc->alias_list = synlist;
		break;
	default:
		free(s);
		err("wmlAddCharsetAttributeStr: Unknown attribute id %d", attrid);
	}
}

