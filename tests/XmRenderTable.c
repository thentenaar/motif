/**
 * Motif
 *
 * Copyright (c) 2026 Tim Hentenaar
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#include "SharedPtrI.h"
#include "XmStringI.h"
#include "XmRenderTI.h"
#include "XmTabListI.h"
#include <check.h>

#include "suites.h"

static Display *display;

static void _init_xt(void)
{
	setenv("LANG", "C", 1);
	display = XtDisplay(init_xt("check_XmRenderTable"));
}

/**
 * Create a table with one rendition (with an optional tag)
 */
static XmRenderTable setup_table(XmStringTag tag)
{
	Arg arg[3];
	XmRendition rend;
	XmRenderTable rt;
	struct __XmRenditionRec *r;
	struct __XmRenderTableRec *t;

	XtSetArg(arg[0], XmNloadModel, XmUNSPECIFIED_LOAD_MODEL);
	XtSetArg(arg[1], XmNfontType, XmFONT_IS_FONT);
	XtSetArg(arg[2], XmNfont, (void *)0x1234);
	rend = XmRenditionCreate(NULL, tag ? tag : XmFONTLIST_DEFAULT_TAG, arg, 3);
	ck_assert_msg((r = XmSharedPtrGet(rend)), "Expected to create a XmRendition");

	rt = XmRenderTableCreate(NULL);
	ck_assert_msg((t = XmSharedPtrGet(rt)), "Expected to create a XmRenderTable");
	t->count = 1;
	t->renditions = (XmRendition *)XtMalloc(sizeof *t->renditions);
	t->renditions[0] = rend;
	_XmAddHashEntry(t->ht, r->tag, rend);
	return rt;
}

/**
 * Setup a table for testing XmRenderTableResolve's resolution order
 */
static XmRenderTable setup_resolve_table(XmStringTag tag,
                                         XmStringTag fallback,
                                         XmStringTag cs,
                                         Boolean default_cs,
                                         Boolean default_locale)
{
	XmRenderTable rt;
	struct __XmRenderTableRec *t;

	rt = setup_table(tag);
	t  = XmSharedPtrGet(rt);
	t->count += 4;
	t->renditions = (XmRendition *)XtRealloc(
		(XtPointer)t->renditions,
		t->count * sizeof *t->renditions
	);

	t->renditions[1] = XmSharedPtrCopy(t->renditions[0], True);
	(*t->renditions[1])->tag = _XmStringCacheTag(
		default_cs ? XmFONTLIST_DEFAULT_TAG : "ccc",
		XmSTRING_TAG_STRLEN
	);
	_XmAddHashEntry(t->ht, (*t->renditions[1])->tag, t->renditions[1]);

	t->renditions[2] = XmSharedPtrCopy(t->renditions[0], True);
	(*t->renditions[2])->tag = _XmStringCacheTag(
		default_locale ? _MOTIF_DEFAULT_LOCALE : "lll",
		XmSTRING_TAG_STRLEN
	);
	_XmAddHashEntry(t->ht, (*t->renditions[2])->tag, t->renditions[2]);

	t->renditions[3] = XmSharedPtrCopy(t->renditions[0], True);
	(*t->renditions[3])->tag = _XmStringCacheTag(fallback, XmSTRING_TAG_STRLEN);
	_XmAddHashEntry(t->ht, (*t->renditions[3])->tag, t->renditions[3]);

	t->renditions[4] = XmSharedPtrCopy(t->renditions[0], True);
	(*t->renditions[4])->tag = _XmStringCacheTag(cs, XmSTRING_TAG_STRLEN);
	_XmAddHashEntry(t->ht, (*t->renditions[4])->tag, t->renditions[4]);
	return rt;
}

/**
 * Client-side function for the NO_RENDITION callback called during
 * XmRenderTableResolve when a tag is requested but doesn't get a hit
 * when probing the render table.
 *
 * If the client data is non-null, supply a fresh render table containing
 * a rendition for the missing tag.
 */
static volatile Boolean no_rendition_called;
static void no_rendition_callback(Widget w, XtPointer client, XtPointer call)
{
	XmDisplayCallbackStruct *cb;

	(void)w;
	cb = (XmDisplayCallbackStruct *)call;

	/* Free the given table, as expected */
	XmRenderTableFree(cb->render_table);
	cb->render_table = client ? setup_table(cb->tag) : NULL;
	no_rendition_called = True;
}

/* Generally, this always succeeds unless we're out of memory */
START_TEST(create)
{
	XmRenderTable t;

	t = XmRenderTableCreate(NULL);
	ck_assert_msg(t, "Expected non-NULL result");
	XmRenderTableFree(t);
}
END_TEST


START_TEST(copy_null_table)
{
	ck_assert_msg(!XmRenderTableCopy(NULL, NULL, 0),
	              "Expected NULL result for NULL rendertable");
}
END_TEST

/* Dup the whole table */
START_TEST(copy_no_tags)
{
	XmRenderTable t, c;

	t = XmRenderTableCreate(NULL);
	c = XmRenderTableCopy(t, NULL, 0);
	ck_assert_msg(c, "Expected a non-NULL result");
	ck_assert_msg(c != t, "Expected tables to not be equal");
	XmRenderTableFree(c);
	XmRenderTableFree(t);
}
END_TEST

START_TEST(copy_no_match)
{
	XmRenderTable rt, rc;
	XmStringTag tag = (XmStringTag)"tag";
	struct __XmRenderTableRec *c;

	rt = setup_table(NULL);
	rc = XmRenderTableCopy(rt, &tag, 1);
	ck_assert_msg(rc != rt, "Tables should not be equal");
	ck_assert_msg((c = XmSharedPtrGet(rc)), "Expected a non-NULL result");
	ck_assert_msg(!c->count, "Expected a count of zero (got %u)", c->count);
	ck_assert_msg(!c->renditions, "Expected NULL renditions");
	XmRenderTableFree(rc);
	XmRenderTableFree(rt);
}
END_TEST

START_TEST(copy_matches)
{
	XmRenderTable rt, rc;
	XmStringTag tag = (XmStringTag)"tag";
	struct __XmRenderTableRec *c;

	rt = setup_table(tag);
	rc = XmRenderTableCopy(rt, &tag, 1);
	ck_assert_msg(rc != rt, "Tables should not be equal");
	ck_assert_msg((c = XmSharedPtrGet(rc)), "Expected a non-NULL result");
	ck_assert_msg(c->count == 1, "Expected a count of 1 (got %u)", c->count);
	ck_assert_msg(c->renditions, "Expected non-NULL renditions");
	XmRenderTableFree(rc);
	XmRenderTableFree(rt);
}
END_TEST

START_TEST(get_tags_null_table)
{
	XmStringTag *tags;

	ck_assert_msg(!XmRenderTableGetTags(NULL, &tags), "Expected 0 tags");
	ck_assert_msg(!tags, "Expected tags to be set to NULL");
}
END_TEST

START_TEST(get_tags_null_tag_list)
{
	XmRenderTable rt;
	XmStringTag tag = (XmStringTag)"tag";

	rt = setup_table(tag);
	ck_assert_msg(!XmRenderTableGetTags(rt, NULL), "Expected 0 tags");
	XmRenderTableFree(rt);
}
END_TEST

START_TEST(get_tags_empty_table)
{
	XmRenderTable rt;
	XmStringTag tag = (XmStringTag)"tag", *tags;
	struct __XmRenderTableRec *t;

	rt = XmRenderTableCreate(NULL);
	ck_assert_msg((t = XmSharedPtrGet(rt)), "Expected to create a XmRenderTable");
	ck_assert_msg(!t->count, "Expected an empty rendertable");
	ck_assert_msg(!XmRenderTableGetTags(rt, &tags), "Expected 0 tags");
	ck_assert_msg(!tags, "Expected tags to be NULL");
	XmRenderTableFree(rt);
}
END_TEST

START_TEST(get_tags)
{
	XmRenderTable rt;
	XmStringTag tag = (XmStringTag)"tag", *tags;

	rt = setup_table(tag);
	ck_assert_msg(XmRenderTableGetTags(rt, &tags) == 1, "Expected 1 tag");
	ck_assert_msg(tags && tags[0], "Expected to get a tag back");
	ck_assert_msg(!strcmp(tags[0], tag),
	              "Expected tag (%s) to be %s", tags[0], tag);
	XtFree(tags[0]);
	XtFree((XtPointer)tags);
	XmRenderTableFree(rt);
}
END_TEST

START_TEST(get_renditions_null_table)
{
	XmStringTag *tags;

	tags = (XmStringTag *)XtCalloc(1, sizeof *tags);
	ck_assert_msg(!XmRenderTableGetRenditions(NULL, tags, 1),
	              "Expected NULL result for NULL rendertable");
	XtFree((XtPointer)tags);
}
END_TEST

START_TEST(get_renditions_null_tags)
{
	XmRenderTable rt;

	rt = setup_table((XmStringTag)"tag");
	ck_assert_msg(!XmRenderTableGetRenditions(rt, NULL, 1),
	              "Expected NULL result for NULL tags");
	XmRenderTableFree(rt);
}
END_TEST

START_TEST(get_renditions_empty_table)
{
	XmRenderTable rt;
	XmRendition *rends;
	XmStringTag tag = (XmStringTag)"tag";

	rt = XmRenderTableCreate(NULL);
	rends = XmRenderTableGetRenditions(rt, &tag, 1);
	ck_assert_msg(rends, "Expected a non-NULL result");
	ck_assert_msg(!rends[0], "Expected the returned rendition to be NULL");
	XmRenderTableFree(rt);
	XmFreeRenditionArray(rends, 1);
}
END_TEST

START_TEST(get_renditions_no_count)
{
	XmRenderTable rt;
	XmStringTag tag = (XmStringTag)"tag";

	rt = setup_table(tag);
	ck_assert_msg(!XmRenderTableGetRenditions(rt, &tag, 0),
	              "Expected NULL result for 0 count");
	XmRenderTableFree(rt);
}
END_TEST

START_TEST(get_renditions_no_match)
{
	XmRenderTable rt;
	XmRendition *rends;
	XmStringTag tag2 = (XmStringTag)"tag2";

	rt = setup_table((XmStringTag)"tag");
	rends = XmRenderTableGetRenditions(rt, &tag2, 1);
	ck_assert_msg(rends, "Expected a non-NULL result");
	ck_assert_msg(!rends[0], "Expected the returned rendition to be NULL");
	XmRenderTableFree(rt);
	XmFreeRenditionArray(rends, 1);
}
END_TEST

START_TEST(get_renditions_null_tag)
{
	XmRenderTable rt;
	XmRendition *rends;
	XmStringTag tag2 = NULL;

	rt = setup_table((XmStringTag)"tag");
	rends = XmRenderTableGetRenditions(rt, &tag2, 1);
	ck_assert_msg(rends, "Expected a non-NULL result");
	ck_assert_msg(!rends[0], "Expected the returned rendition to be NULL");
	XmRenderTableFree(rt);
	XmFreeRenditionArray(rends, 1);
}
END_TEST

START_TEST(get_renditions_match)
{
	XmRenderTable rt;
	XmRendition *rends;
	XmStringTag tag = (XmStringTag)"tag", *tags;

	tags = (XmStringTag *)XtCalloc(3, sizeof *tags);
	tags[0] = NULL;
	tags[1] = (XmStringTag)"tag2";
	tags[2] = tag;

	rt = setup_table(tag);
	rends = XmRenderTableGetRenditions(rt, tags, 3);
	ck_assert_msg(rends, "Expected a non-NULL result");
	ck_assert_msg(!rends[0], "Expected the first rendition to be NULL");
	ck_assert_msg(!rends[1], "Expected the second rendition to be NULL");
	ck_assert_msg(rends[2], "Expected the third rendition to be non-NULL");
	XmRenderTableFree(rt);
	XmFreeRenditionArray(rends, 3);
	XtFree((XtPointer)tags);
}
END_TEST

START_TEST(get_rendition_null_table)
{
	ck_assert_msg(!XmRenderTableGetRendition(NULL, (XmStringTag)"tag"),
	              "Expected NULL result for NULL tag");
}
END_TEST

START_TEST(get_rendition_null_tag)
{
	XmRenderTable rt;

	rt = setup_table((XmStringTag)"tag");
	ck_assert_msg(!XmRenderTableGetRendition(rt, NULL),
	              "Expected NULL result for NULL tag");
	XmRenderTableFree(rt);
}
END_TEST

START_TEST(get_rendition_empty_table)
{
	XmRenderTable rt;

	rt = XmRenderTableCreate(NULL);
	ck_assert_msg(!XmRenderTableGetRendition(rt, (XmStringTag)"tag"),
	              "Expected a NULL result");
	XmRenderTableFree(rt);
}
END_TEST

START_TEST(get_rendition_no_match)
{
	XmRenderTable rt;

	rt = setup_table((XmStringTag)"tag");
	ck_assert_msg(!XmRenderTableGetRendition(rt, (XmStringTag)"tag2"),
	              "Expected NULL result for non-matching tag");
	XmRenderTableFree(rt);
}
END_TEST

START_TEST(get_rendition_match)
{
	XmRenderTable rt;
	XmRendition r;
	XmStringTag tag = (XmStringTag)"tag";

	rt = setup_table(tag);
	ck_assert_msg((r = XmRenderTableGetRendition(rt, tag)),
	              "Expected non-NULL result for matching tag");
	XmRenditionFree(r);
	XmRenderTableFree(rt);
}
END_TEST

START_TEST(add_null_renditions)
{
	XmRenderTable rt, xt;
	XmRendition r;

	rt = setup_table((XmStringTag)"tag");
	xt = XmRenderTableAddRenditions(rt, NULL, 1, XmSKIP);
	ck_assert_msg(xt == rt, "Expected to get the original table");
	XmRenderTableFree(rt);
}
END_TEST

START_TEST(add_no_count)
{
	XmRenderTable rt, xt;
	XmRendition r;
	struct __XmRenderTableRec *t;

	rt = setup_table((XmStringTag)"tag");
	t  = XmSharedPtrGet(rt);
	xt = XmRenderTableAddRenditions(rt, t->renditions, 0, XmSKIP);
	ck_assert_msg(xt == rt, "Expected to get the original table");
	XmRenderTableFree(rt);
}
END_TEST

/* Creates a new table with given renditions */
START_TEST(add_null_table)
{
	XmRenderTable rt, xt;
	XmRendition r;
	struct __XmRenderTableRec *t;

	rt = setup_table((XmStringTag)"tag");
	t  = XmSharedPtrGet(rt);
	xt = XmRenderTableAddRenditions(NULL, t->renditions, 1, XmSKIP);
	t  = XmSharedPtrGet(xt);
	ck_assert_msg(xt != rt, "Expected to get a new table");
	ck_assert_msg(t->count == 1, "Expected a count of 1 (got %u)", t->count);
	ck_assert_msg(t->renditions[0], "Expected the table to have 1 rendition");
	XmRenderTableFree(xt);
	XmRenderTableFree(rt);
}
END_TEST

START_TEST(add_merge_old)
{
	XmRenderTable rt, xt, yt;
	XmRendition r;
	struct __XmRenderTableRec *t;

	/* rt <- xt */
	rt = setup_table((XmStringTag)"tag");
	xt = XmRenderTableCopy(rt, NULL, 0);
	t  = XmSharedPtrGet(xt);
	(**t->renditions)->style.underline = 0x5a;

	t  = XmSharedPtrGet(xt);
	yt = XmRenderTableAddRenditions(rt, t->renditions, 1, XmMERGE_OLD);
	t  = XmSharedPtrGet(yt);
	ck_assert_msg(yt != xt, "Expected to get a new table");
	ck_assert_msg(t->count == 1, "Expected a count of 1 (got %u)", t->count);
	ck_assert_msg(t->renditions[0], "Expected the table to have 1 rendition");
	ck_assert_msg((**t->renditions)->style.underline == 0x5a,
	              "Expected 0x5a (got %x)",
	              (**t->renditions)->style.underline);
	XmRenderTableFree(yt);
	XmRenderTableFree(xt);
}
END_TEST

START_TEST(add_merge_new)
{
	XmRenderTable rt, xt, yt;
	XmRendition r;
	struct __XmRenderTableRec *t;

	/* xt <- rt */
	rt = setup_table((XmStringTag)"tag");
	xt = XmRenderTableCopy(rt, NULL, 0);
	t  = XmSharedPtrGet(xt);
	(**t->renditions)->style.underline = 0x5a;

	t  = XmSharedPtrGet(rt);
	yt = XmRenderTableAddRenditions(xt, t->renditions, 1, XmMERGE_NEW);
	t  = XmSharedPtrGet(yt);
	ck_assert_msg(yt != xt, "Expected to get a new table");
	ck_assert_msg(t->count == 1, "Expected a count of 1 (got %u)", t->count);
	ck_assert_msg(t->renditions[0], "Expected the table to have 1 rendition");
	ck_assert_msg((**t->renditions)->style.underline == 0x5a, "Unexpected value");
	XmRenderTableFree(yt);
	XmRenderTableFree(rt);
}
END_TEST

START_TEST(add_merge_replace)
{
	XmRenderTable rt, xt, yt;
	XmRendition r;
	struct __XmRenderTableRec *t;

	/* [rt] */
	rt = setup_table((XmStringTag)"tag");
	xt = XmRenderTableCopy(rt, NULL, 0);
	t  = XmSharedPtrGet(xt);
	(**t->renditions)->style.underline = 0x5a;

	t  = XmSharedPtrGet(rt);
	(**t->renditions)->style.underline = 0xaa;
	yt = XmRenderTableAddRenditions(xt, t->renditions, 1, XmMERGE_NEW);
	t  = XmSharedPtrGet(yt);
	ck_assert_msg(yt != xt, "Expected to get a new table");
	ck_assert_msg(t->count == 1, "Expected a count of 1 (got %u)", t->count);
	ck_assert_msg(t->renditions[0], "Expected the table to have 1 rendition");
	ck_assert_msg((**t->renditions)->style.underline == 0xaa, "Unexpected value");
	XmRenderTableFree(yt);
	XmRenderTableFree(rt);
}
END_TEST

/* XmDUPLICATE */
START_TEST(add_merge_duplicate)
{
	XmRenderTable rt, xt, yt;
	XmRendition r;
	struct __XmRenderTableRec *t;

	/* [rt] + [xt] */
	rt = setup_table((XmStringTag)"tag");
	xt = XmRenderTableCopy(rt, NULL, 0);
	t  = XmSharedPtrGet(rt);
	yt = XmRenderTableAddRenditions(xt, t->renditions, 1, XmDUPLICATE);
	t  = XmSharedPtrGet(yt);
	ck_assert_msg(yt != xt, "Expected to get a new table");
	ck_assert_msg(t->count == 2, "Expected a count of 2 (got %u)", t->count);
	ck_assert_msg(t->renditions[0] && t->renditions[1],
	              "Expected the table to have 2 renditions");
	XmRenderTableFree(yt);
	XmRenderTableFree(rt);
}
END_TEST

START_TEST(add_merge_skip)
{
	XmRenderTable rt, xt, yt;
	XmRendition r;
	struct __XmRenderTableRec *t;

	/* [rt] */
	rt = setup_table((XmStringTag)"tag");
	xt = XmRenderTableCopy(rt, NULL, 0);
	t  = XmSharedPtrGet(rt);
	yt = XmRenderTableAddRenditions(xt, t->renditions, 1, XmSKIP);
	t  = XmSharedPtrGet(yt);
	ck_assert_msg(yt != xt, "Expected to get a new table");
	ck_assert_msg(!t->count, "Expected a count of 0 (got %u)", t->count);
	ck_assert_msg(!t->renditions, "Expected to have no renditions");
	XmRenderTableFree(yt);
	XmRenderTableFree(rt);
}
END_TEST

START_TEST(add_copies_empty_tag)
{
	XmRenderTable rt, xt, yt;
	XmRendition r;
	struct __XmRenderTableRec *t;

	/* XmS-tagged entries are copied thru */
	rt = setup_table(NULL);
	t  = XmSharedPtrGet(rt);
	(**t->renditions)->tag = XmS;
	xt = XmRenderTableCopy(rt, NULL, 0);
	t  = XmSharedPtrGet(rt);
	(**t->renditions)->tag = XmS;
	yt = XmRenderTableAddRenditions(xt, t->renditions, 1, XmMERGE_REPLACE);
	t  = XmSharedPtrGet(yt);
	ck_assert_msg(yt != xt, "Expected to get a new table");
	ck_assert_msg(t->count == 2, "Expected a count of 2 (got %u)", t->count);
	ck_assert_msg(t->renditions[0] && t->renditions[1],
	              "Expected the table to have 2 rendition");
	XmRenderTableFree(yt);
	XmRenderTableFree(rt);
}
END_TEST

START_TEST(remove_null_table)
{
	XmStringTag *tags;

	tags = (XmStringTag *)XtCalloc(1, sizeof *tags);
	ck_assert_msg(!XmRenderTableRemoveRenditions(NULL, tags, 1),
	              "Expected NULL result for NULL rendertable");
	XtFree((XtPointer)tags);
}
END_TEST

START_TEST(remove_null_tags)
{
	XmRenderTable rt;

	rt = setup_table((XmStringTag)"tag");
	ck_assert_msg(rt == XmRenderTableRemoveRenditions(rt, NULL, 1),
	              "Expected the original table NULL tags");
	XmRenderTableFree(rt);
}
END_TEST

START_TEST(remove_no_count)
{
	XmRenderTable rt;
	XmStringTag tag = (XmStringTag)"tag";

	rt = setup_table(tag);
	ck_assert_msg(rt == XmRenderTableRemoveRenditions(rt, &tag, 0),
	              "Expected the original table for 0 count");
	XmRenderTableFree(rt);
}
END_TEST

START_TEST(remove_empty_table)
{
	XmRenderTable rt, rx;
	XmStringTag tag = (XmStringTag)"tag";

	rt = XmRenderTableCreate(NULL);
	rx = XmRenderTableRemoveRenditions(rt, &tag, 1);
	ck_assert_msg(!rx, "Expected a NULL result");
}

START_TEST(remove_sole_entry)
{
	XmRenderTable rt, rx;
	XmStringTag tag = (XmStringTag)"tag";

	rt = setup_table(tag);
	rx = XmRenderTableRemoveRenditions(rt, &tag, 1);
	ck_assert_msg(!rx, "Expected a NULL result");
}
END_TEST

START_TEST(remove_one)
{
	XmRenderTable rt, rx;
	XmStringTag tag = (XmStringTag)"tag";
	struct __XmRenderTableRec *t;

	rt = setup_table(tag);
	t  = XmSharedPtrGet(rt);
	t->count++;
	t->renditions = (XmRendition *)XtRealloc(
		(XtPointer)t->renditions,
		t->count * sizeof *t->renditions
	);
	t->renditions[1]       = XmSharedPtrCopy(t->renditions[0], True);
	_XmRemoveHashEntry(t->ht, (**t->renditions)->tag);
	_XmAddHashEntry(t->ht, (**t->renditions)->tag, t->renditions[1]);
	(**t->renditions)->tag = _XmStringCacheTag("tag2", XmSTRING_TAG_STRLEN);
	_XmAddHashEntry(t->ht, (**t->renditions)->tag, *t->renditions);

	rx = XmRenderTableRemoveRenditions(rt, &tag, 1);
	ck_assert_msg((t = XmSharedPtrGet(rx)), "Expected a non-NULL result");
	ck_assert_msg(rx != rt, "Expected rx to not equal rt");
	ck_assert_msg(t->count == 1, "Expected a count of 1 (got %u)", t->count);
	ck_assert_msg(!strcmp((**t->renditions)->tag, "tag2"),
	              "Expected tag (%s) to be tag2",
	              (**t->renditions)->tag);
	XmRenderTableFree(rx);
}
END_TEST

START_TEST(remove_many)
{
	XmRenderTable rt, rx;
	XmStringTag tag = (XmStringTag)"tag", tags[2];
	struct __XmRenderTableRec *t;

	tags[0] = (XmStringTag)"tag2";
	tags[1] = (XmStringTag)"tag3";
	rt = setup_table(tag);
	t  = XmSharedPtrGet(rt);
	t->count += 2;
	t->renditions = (XmRendition *)XtRealloc(
		(XtPointer)t->renditions,
		t->count * sizeof *t->renditions
	);
	t->renditions[1] = XmSharedPtrCopy(t->renditions[0], True);
	(*t->renditions[1])->tag = _XmStringCacheTag(tags[0], XmSTRING_TAG_STRLEN);
	_XmAddHashEntry(t->ht, (*t->renditions[1])->tag, t->renditions[1]);

	t->renditions[2] = XmSharedPtrCopy(t->renditions[0], True);
	(*t->renditions[2])->tag = _XmStringCacheTag(tags[1], XmSTRING_TAG_STRLEN);
	_XmAddHashEntry(t->ht, (*t->renditions[2])->tag, t->renditions[2]);

	rx = XmRenderTableRemoveRenditions(rt, tags, 2);
	ck_assert_msg((t = XmSharedPtrGet(rx)), "Expected a non-NULL result");
	ck_assert_msg(rx != rt, "Expected rx to not equal rt");
	ck_assert_msg(t->count == 1, "Expected a count of 1 (got %u)", t->count);
	ck_assert_msg(!strcmp((**t->renditions)->tag, tag),
	              "Expected tag (%s) to be %s",
	              (**t->renditions)->tag, tag);
	XmRenderTableFree(rx);
}
END_TEST

START_TEST(resolve_null_table)
{
	ck_assert_msg(!XmRenderTableResolve(NULL, NULL, 0, NULL, NULL),
	              "Expected NULL on null rendertable");
}
END_TEST

/* Should take the fallback tag */
START_TEST(resolve_no_tags)
{
	XmRendition rend;
	XmRenderTable rt;
	XmStringTag cs, tag, fallback;
	struct __XmRenditionRec *r;

	tag      = (XmStringTag)"tag";
	fallback = (XmStringTag)"fallback";
	cs       = XmStringGetCharset();

	rt = setup_resolve_table(tag, fallback, cs, True, True);
	rend = XmRenderTableResolve(rt, NULL, 1, fallback, NULL);
	ck_assert_msg((r = XmSharedPtrGet(rend)), "Expected to get a rendition");
	ck_assert_msg(!strcmp(r->tag, fallback),
	              "Expected tag (%s) to be %s", r->tag, fallback);
	XmRenderTableFree(rt);
	XmRenditionFree(rend);
	XtFree(cs);
}
END_TEST

/* This should try the current charset */
START_TEST(resolve_no_fallback)
{
	XmRendition rend;
	XmRenderTable rt;
	XmStringTag cs, tag, fallback;
	struct __XmRenditionRec *r;

	tag      = (XmStringTag)"tag";
	fallback = (XmStringTag)"fallback";
	cs       = XmStringGetCharset();

	rt = setup_resolve_table(tag, fallback, cs, True, True);
	rend = XmRenderTableResolve(rt, NULL, 1, (XmStringTag)"not-fallback", NULL);
	ck_assert_msg((r = XmSharedPtrGet(rend)), "Expected to get a rendition");
	ck_assert_msg(!strcmp(r->tag, cs),
	              "Expected tag (%s) to be %s", r->tag, cs);
	XmRenderTableFree(rt);
	XmRenditionFree(rend);
	XtFree(cs);
}
END_TEST

/* Should take XmFONTLIST_DEFAULT_TAG */
START_TEST(resolve_no_charset)
{
	XmRendition rend;
	XmRenderTable rt;
	XmStringTag cs, tag, fallback;
	struct __XmRenditionRec *r;

	tag      = (XmStringTag)"tag";
	fallback = (XmStringTag)"fallback";
	cs       = (XmStringTag)"not-charset";

	rt = setup_resolve_table(tag, fallback, cs, True, True);
	rend = XmRenderTableResolve(rt, NULL, 1, (XmStringTag)"not-fallback", NULL);
	ck_assert_msg((r = XmSharedPtrGet(rend)), "Expected to get a rendition");
	ck_assert_msg(!strcmp(r->tag, XmFONTLIST_DEFAULT_TAG),
	              "Expected tag (%s) to be %s", r->tag, XmFONTLIST_DEFAULT_TAG);
	XmRenderTableFree(rt);
	XmRenditionFree(rend);
}
END_TEST

/* Should take _MOTIF_DEFAULT_LOCALE */
START_TEST(resolve_no_default_charset)
{
	XmRendition rend;
	XmRenderTable rt;
	XmStringTag cs, tag, fallback;
	struct __XmRenditionRec *r;

	tag      = (XmStringTag)"tag";
	fallback = (XmStringTag)"fallback";
	cs       = (XmStringTag)"not-charset";

	rt = setup_resolve_table(tag, fallback, cs, False, True);
	rend = XmRenderTableResolve(rt, NULL, 1, (XmStringTag)"not-fallback", NULL);
	ck_assert_msg((r = XmSharedPtrGet(rend)), "Expected to get a rendition");
	ck_assert_msg(!strcmp(r->tag, _MOTIF_DEFAULT_LOCALE),
	              "Expected tag (%s) to be %s", r->tag, _MOTIF_DEFAULT_LOCALE);
	XmRenderTableFree(rt);
	XmRenditionFree(rend);
}
END_TEST

/* In absence of the above, it should take the first font it can find */
START_TEST(resolve_first_font)
{
	XmRendition rend;
	XmRenderTable rt;
	XmStringTag cs, tag, fallback;
	struct __XmRenditionRec *r;

	tag      = (XmStringTag)"tag";
	fallback = (XmStringTag)"fallback";
	cs       = (XmStringTag)"not-charset";

	rt = setup_resolve_table(tag, fallback, cs, False, False);
	rend = XmRenderTableResolve(rt, NULL, 1, (XmStringTag)"not-fallback", NULL);
	ck_assert_msg((r = XmSharedPtrGet(rend)), "Expected to get a rendition");
	ck_assert_msg(!strcmp(r->tag, tag),
	              "Expected tag (%s) to be %s", r->tag, tag);
	XmRenderTableFree(rt);
	XmRenditionFree(rend);
}
END_TEST

START_TEST(resolve_calls_no_rendition_callback)
{
	Widget xd;
	XmRenderTable rt;
	XmRendition rend;
	XmStringTag tag = (XmStringTag)"tag";
	struct __XmRenderTableRec *t;

	rt = XmRenderTableCreate(NULL);
	t  = XmSharedPtrGet(rt);
	xd = XmGetXmDisplay(t->display);

	no_rendition_called = False;
	XtAddCallback(xd, XmNnoRenditionCallback, no_rendition_callback, NULL);
	rend = XmRenderTableResolve(rt, &tag, 1, NULL, NULL);
	XtRemoveCallback(xd, XmNnoRenditionCallback, no_rendition_callback, NULL);
	XmRenderTableFree(rt);
	ck_assert_msg(rend && strcmp((*rend)->tag, tag),
	              "Expected to not get a rendition from the callback");
	ck_assert_msg(no_rendition_called, "Expected NO_RENDITION callback to be called");
}
END_TEST

START_TEST(resolve_callback_supplies_rendition)
{
	Widget xd;
	XmRenderTable rt;
	XmRendition rend;
	XmStringTag tag = (XmStringTag)"tag";
	struct __XmRenderTableRec *t;

	rt = XmRenderTableCreate(NULL);
	t  = XmSharedPtrGet(rt);
	xd = XmGetXmDisplay(t->display);

	no_rendition_called = False;
	XtAddCallback(xd, XmNnoRenditionCallback, no_rendition_callback, (void *)True);
	rend = XmRenderTableResolve(rt, &tag, 1, NULL, NULL);
	XtRemoveCallback(xd, XmNnoRenditionCallback, no_rendition_callback, (void *)True);
	XmRenderTableFree(rt);
	ck_assert_msg(rend, "Expected to get a rendition");
	ck_assert_msg(no_rendition_called, "Expected NO_RENDITION callback to be called");
	ck_assert_msg(!strcmp((*rend)->tag, tag),
	              "Expected tag (%s) to be (%s)", (*rend)->tag, tag);
	XmRenditionFree(rend);
}
END_TEST

START_TEST(resolve_cascades_styles)
{
	XmRendition rend;
	XmRenderTable rt;
	Widget xd;
	XmRenditionStyle style;
	XmStringTag cs, tag, fallback, tags[5];
	struct __XmRenditionRec *r;
	struct __XmRenderTableRec *t;

	tag      = (XmStringTag)"tag";
	fallback = (XmStringTag)"fallback";
	cs       = XmStringGetCharset();
	style    = XmRenditionStyleCreate();

	rt = setup_resolve_table(tag, fallback, cs, True, True);
	t  = XmSharedPtrGet(rt);
	xd = XmGetXmDisplay(t->display);

	/* Ensure this doesn't get cascaded */
	style->bg_state = XmFORCE_COLOR;
	style->bg.pixel = 0x42424242;

	/* It should take the first font it finds */
	(*t->renditions[2])->font = (void *)0xdeadbeef;
	(*t->renditions[3])->font = NULL;
	(*t->renditions[4])->font = NULL;

	/* and the first style prop that isn't XmAS_IS / XmUNSPECIFIED_PIXEL */
	(*t->renditions[0])->style.fg.pixel = 0x77ee77ee;
	(*t->renditions[1])->style.fg.pixel = XmUNSPECIFIED_PIXEL;
	(*t->renditions[2])->style.fg.pixel = 0x55aa55aa;
	(*t->renditions[3])->style.fg.pixel = 0x33cc33cc;
	(*t->renditions[4])->style.fg.pixel = XmUNSPECIFIED_PIXEL;

	(*t->renditions[0])->style.underline = XmAS_IS;
	(*t->renditions[1])->style.underline = 0xdd;
	(*t->renditions[2])->style.underline = XmAS_IS;
	(*t->renditions[3])->style.underline = XmAS_IS;
	(*t->renditions[4])->style.underline = XmAS_IS;

	(*t->renditions[0])->style.strikethru = XmAS_IS;
	(*t->renditions[1])->style.strikethru = XmAS_IS;
	(*t->renditions[2])->style.strikethru = XmAS_IS;
	(*t->renditions[3])->style.strikethru = XmAS_IS;
	(*t->renditions[4])->style.strikethru = XmAS_IS;

	/* This will determine the order of the style cascade */
	tags[0] = (*t->renditions[0])->tag;
	tags[1] = (*t->renditions[1])->tag;
	tags[2] = (*t->renditions[2])->tag;
	tags[3] = (*t->renditions[3])->tag;
	tags[4] = (*t->renditions[4])->tag;

	no_rendition_called = False;
	XtAddCallback(xd, XmNnoRenditionCallback, no_rendition_callback, NULL);
	rend = XmRenderTableResolve(rt, tags, 5, (XmStringTag)"not-fallback", style);
	XtRemoveCallback(xd, XmNnoRenditionCallback, no_rendition_callback, (void *)True);

	ck_assert_msg(!no_rendition_called, "No rendition callback shouldn't have been called");
	ck_assert_msg((r = XmSharedPtrGet(rend)), "Expected to get a rendition");
	ck_assert_msg(!strcmp(r->tag, _MOTIF_DEFAULT_LOCALE),
	              "Expected tag (%s) to be %s", r->tag, _MOTIF_DEFAULT_LOCALE);
	ck_assert_msg(style->fg.pixel == 0x33cc33cc,
	              "Expected fg.pixel (0x%08lx) to be 0x33cc33cc", style->fg.pixel);
	ck_assert_msg(style->bg.pixel == 0x42424242,
	              "Expected bg.pixel (0x%08lx) to be 0x42424242", style->bg.pixel);
	ck_assert_msg(style->underline == 0xdd,
	              "Expected underline (%x) to be 0xdd", style->underline);
	ck_assert_msg(style->strikethru == XmAS_IS,
	              "Expected strikethru (%x) to be XmAS_IS (%x)",
	              style->underline, XmAS_IS);

	ck_assert_msg(r->style.fg.pixel == (*t->renditions[2])->style.fg.pixel,
	              "Expected rend->style.fg.pixel (0x%08lx) to be 0x%08lx",
	              r->style.fg.pixel, (*t->renditions[2])->style.fg.pixel);
	ck_assert_msg(r->style.underline == XmAS_IS,
	              "Expected rend->style.underline (0x%x) to be XmAS_IS",
	              r->style.underline);
	ck_assert_msg(r->style.strikethru == XmAS_IS,
	              "Expected rend->style.strikethru (0x%x) to be XmAS_IS (%x)",
	              r->style.underline, XmAS_IS);
	ck_assert_msg(r->font == (void *)0xdeadbeef,
	              "Expected rend->font (%p) to be 0xdeadbeef", r->font);
	XmRenderTableFree(rt);
	XmRenditionFree(rend);
	XmRenditionStyleFree(style);
	XtFree(cs);
}
END_TEST

START_TEST(resolve_tags_without_fonts)
{
	XmRendition rend;
	XmRenderTable rt;
	Widget xd;
	XmStringTag cs, tag, fallback, tags[5];
	struct __XmRenditionRec *r;
	struct __XmRenderTableRec *t;

	tag      = (XmStringTag)"tag";
	fallback = (XmStringTag)"fallback";
	cs       = XmStringGetCharset();

	rt = setup_resolve_table(tag, fallback, cs, True, True);
	t  = XmSharedPtrGet(rt);
	xd = XmGetXmDisplay(t->display);

	tags[0] = (*t->renditions[0])->tag;
	tags[1] = (*t->renditions[1])->tag;
	tags[2] = (*t->renditions[2])->tag;
	tags[3] = (*t->renditions[3])->tag;
	tags[4] = (*t->renditions[4])->tag;

	/* If none of the renditions have a font, we should get the default */
	(*t->renditions[0])->font = NULL;
	(*t->renditions[1])->font = NULL;
	(*t->renditions[2])->font = NULL;
	(*t->renditions[3])->font = NULL;
	(*t->renditions[4])->font = NULL;

	rend = XmRenderTableResolve(rt, tags, 5, (XmStringTag)"not-fallback", NULL);
	ck_assert_msg((r = XmSharedPtrGet(rend)), "Expected to get a rendition");
	ck_assert_msg(!strcmp(r->tag, XmFONTLIST_DEFAULT_TAG),
	              "Expected tag (%s) to be %s", r->tag, XmFONTLIST_DEFAULT_TAG);
	ck_assert_msg(r->font || r->xftFont, "Expected to have a font");
	XmRenderTableFree(rt);
	XmRenditionFree(rend);
	XtFree(cs);
}
END_TEST

/* Ensure we get something */
START_TEST(default_extents_empty_table)
{
	XmRenderTable rt;
	int w = 0, x = 0, h = 0, a = 0, d = 0;

	rt = XmRenderTableCreate(NULL);
	XmRenderTableGetDefaultExtents(rt, &w, &x, &h, &a, &d);
	ck_assert_msg(w, "Expected a width");
	ck_assert_msg(x, "Expected an ink width");
	ck_assert_msg(h, "Expected a height");
	ck_assert_msg(a, "Expected an ascent");
	ck_assert_msg(d, "Expected a descent");
	XmRenderTableFree(rt);
}
END_TEST

/* tag, font type, load model */
static const char basic_props[42] = {
	0x01, 0x00, 0x27,
	0x03, 0x1c, 0x46, 0x4f, 0x4e, 0x54, 0x4c, 0x49, 0x53, 0x54, 0x5f,
	0x44, 0x45, 0x46, 0x41, 0x55, 0x4c, 0x54, 0x5f, 0x54, 0x41, 0x47,
	0x5f, 0x53, 0x54, 0x52, 0x49, 0x4e, 0x47, 0x00,
	0x05, 0x01, 0x00,
	0x04, 0x01, 0x00,
	0x01, 0x00, 0x00
};

#define AP_TAG_OFFSET      5
#define AP_PATTERN_OFFSET 17
#define AP_STYLE_OFFSET   30
#define AP_FG_OFFSET      52
#define AP_BG_OFFSET      62

static const char all_props[115] = {
	0x01, 0x00, 0x70,
	0x03, 0x04, 0x74, 0x61, 0x67, 0x00,
	0x05, 0x01, 0x02,
	0x04, 0x01, 0x03,
	0x06, 0x0b, 0x73, 0x61, 0x6e, 0x73, 0x2d, 0x73, 0x65, 0x72, 0x69, 0x66, 0x00,
	0x07, 0x06, 0x53, 0x74, 0x79, 0x6c, 0x65, 0x00,
	0x08, 0x01, 0x6e,
	0x09, 0x01, 0xc8,
	0x0c, 0x01, 0x03,
	0x0d, 0x01, 0x01,
	0x0b, 0x08, 0xff, 0xff, 0xcc, 0xcc, 0x33, 0x33, 0x11, 0x11,
	0x0a, 0x08, 0xff, 0xff, 0x22, 0x22, 0xee, 0xee, 0x11, 0x11,
	0x0e, 0x09, '1', '.', '0', '0', '0', '0', '0', '0', 0x00,
	0x0f, 0x01, 0x05,
	0x10, 0x01, 0x02,
	0x11, 0x01, 0x01,
	0x02, 0x00,
	0x0e, 0x09, '2', '.', '0', '0', '0', '0', '0', '0', 0x00,
	0x0f, 0x01, 0x07,
	0x10, 0x01, 0x01,
	0x11, 0x01, 0x00,
	0x02, 0x00,
	0x01, 0x00, 0x00
};

START_TEST(toprop_null_table)
{
	char *ret = NULL;

	ck_assert_msg(!XmRenderTableCvtToProp(NULL, NULL, &ret),
	              "Expected a return of zero for NULL table");
}
END_TEST

START_TEST(toprop_empty_table)
{
	XmRenderTable rt;
	char *props = NULL;

	rt = XmRenderTableCreate(NULL);
	ck_assert_msg(!XmRenderTableCvtToProp(NULL, rt, &props),
	              "Expected a return of zero for empty table");
	ck_assert_msg(!props, "Expected NULL props for empty table");
	XmRenderTableFree(rt);
}
END_TEST

START_TEST(toprop_basic_props)
{
	XmRenderTable rt;
	char *props = NULL;
	unsigned int len = 0;

	rt  = setup_table(NULL);
	len = XmRenderTableCvtToProp(NULL, rt, &props);

	ck_assert_msg(len == sizeof basic_props,
	              "Expected a length of %lu bytes (got %u)",
	              sizeof basic_props, len);
	ck_assert_msg(props, "Expected props to be non-NULL");
	ck_assert_msg(!memcmp(props, basic_props, len),
	              "Expected bytes to match");

	XtFree(props);
	XmRenderTableFree(rt);
}
END_TEST

START_TEST(toprop_all_props)
{
	Arg arg[11];
	Colormap cmap;
	XColor bg, fg;
	XmTab tab;
	XmTabList tabs;
	XmRendition rend;
	XmRenderTable rt;
	unsigned int len = 0;
	char *props = NULL, *expected;

	fg.red   = 0xcccc;
	fg.green = 0x3333;
	fg.blue  = 0x1111;
	cmap = DefaultColormap(display, DefaultScreen(display));
	if (!XAllocColor(display, cmap, &fg))
		fg.pixel = XmUNSPECIFIED_PIXEL;

	bg.red   = 0x2222;
	bg.green = 0xeeee;
	bg.blue  = 0x1111;
	if (!XAllocColor(display, cmap, &bg))
		bg.pixel = XmUNSPECIFIED_PIXEL;

	tab  = XmTabCreate(1.0f, XmINCHES, XmABSOLUTE, XmALIGNMENT_END, NULL);
	tabs = XmTabListInsertTabs(NULL, &tab, 1, INT_MAX);
	tab  = XmTabCreate(2.0f, XmMILLIMETERS, XmRELATIVE, XmALIGNMENT_CENTER, NULL);
	tabs = XmTabListInsertTabs(tabs, &tab, 1, INT_MAX);
	ck_assert_msg(tabs, "Expected to create a tablist");

	XtSetArg(arg[0],  XmNloadModel,           XmLOAD_LAZY);
	XtSetArg(arg[1],  XmNfontType,            XmFONT_IS_XFT);
	XtSetArg(arg[2],  XmNfontName,            "sans-serif");
	XtSetArg(arg[3],  XmNfontStyle,           "Style");
	XtSetArg(arg[4],  XmNfontWeight,          XmWEIGHT_BOLD);
	XtSetArg(arg[5],  XmNfontSlant,           XmSLANT_OBLIQUE);
	XtSetArg(arg[6],  XmNunderlineType,       XmSINGLE_DASHED_LINE);
	XtSetArg(arg[7],  XmNstrikethruType,      XmSINGLE_LINE);
	XtSetArg(arg[8],  XmNrenditionForeground, fg.pixel);
	XtSetArg(arg[9],  XmNrenditionBackground, bg.pixel);
	XtSetArg(arg[10], XmNtabList,             tabs);
	rend = XmRenditionCreate(NULL, "tag", arg, 11);
	ck_assert_msg(rend, "Expected to create a rendition");
	rt = XmRenderTableAddRenditions(NULL, &rend, 1, XmMERGE_NEW);
	ck_assert_msg(rt, "Expected a render table");
	XmTabListFree(tabs);
	XmRenditionFree(rend);

	/* Render table -> TLV */
	len = XmRenderTableCvtToProp(NULL, rt, &props);

	/* Make sure we have the correct color values */
	XQueryColor(display, cmap, &fg);
	XQueryColor(display, cmap, &bg);
	XFreeColors(display, cmap, &fg.pixel, 1, 0);
	XFreeColors(display, cmap, &bg.pixel, 1, 0);

	expected = XtMalloc(sizeof all_props);
	memcpy(expected, all_props, sizeof all_props);
	expected[AP_FG_OFFSET]     = (fg.red >> 8) & 0xff;
	expected[AP_FG_OFFSET + 1] = fg.red & 0xff;
	expected[AP_FG_OFFSET + 2] = (fg.green >> 8) & 0xff;
	expected[AP_FG_OFFSET + 3] = fg.green & 0xff;
	expected[AP_FG_OFFSET + 4] = (fg.blue >> 8) & 0xff;
	expected[AP_FG_OFFSET + 5] = fg.blue & 0xff;
	expected[AP_BG_OFFSET]     = (bg.red >> 8) & 0xff;
	expected[AP_BG_OFFSET + 1] = bg.red & 0xff;
	expected[AP_BG_OFFSET + 2] = (bg.green >> 8) & 0xff;
	expected[AP_BG_OFFSET + 3] = bg.green & 0xff;
	expected[AP_BG_OFFSET + 4] = (bg.blue >> 8) & 0xff;
	expected[AP_BG_OFFSET + 5] = bg.blue & 0xff;

	ck_assert_msg(len == sizeof all_props, "Expected %lu bytes (got %u)",
	              sizeof all_props, len);
	ck_assert_msg(!memcmp(expected, all_props, sizeof all_props),
	              "Expected bytes to compare equal");
	XtFree(expected);
	XtFree(props);
	XmRenderTableFree(rt);
}
END_TEST

START_TEST(fromprop_null_prop)
{
	ck_assert_msg(!XmRenderTableCvtFromProp(NULL, NULL, 100),
	              "Expected NULL result for NULL prop");
}
END_TEST

START_TEST(fromprop_zero_length)
{
	ck_assert_msg(!XmRenderTableCvtFromProp(NULL, basic_props, 0),
	              "Expected NULL result for zero length");
}
END_TEST

START_TEST(fromprop_no_rendition_tag)
{
	char *data;

	data = XtMalloc(sizeof basic_props);
	memcpy(data, basic_props, sizeof basic_props);

	/* Nuke the RT_RENDITION tag */
	*data = 0x55;

	ck_assert_msg(!XmRenderTableCvtFromProp(NULL, data, sizeof basic_props),
	              "Expected NULL result for no rendition tag");
	XtFree(data);
}
END_TEST

START_TEST(fromprop_basic_props)
{
	char *data;
	XmRenderTable rt;
	XmRendition rend;
	struct __XmRenditionRec *r;
	struct __XmRenderTableRec *t;

	data = XtMalloc(sizeof basic_props);
	memcpy(data, basic_props, sizeof basic_props);
	rt = XmRenderTableCvtFromProp(NULL, data, sizeof basic_props);

	ck_assert_msg((t = XmSharedPtrGet(rt)), "Expected to get a render table");
	ck_assert_msg(t->count == 1, "Expected one rendition (got %u)", t->count);
	ck_assert_msg((r = XmSharedPtrGet(*t->renditions)), "Expected a rendition");
	ck_assert_msg(r->tag, "Expected rendition to have a tag");
	ck_assert_msg(!strcmp(r->tag, XmFONTLIST_DEFAULT_TAG),
	              "Expected XmFONTLIST_DEFAULT_TAG, got %s", r->tag);
	ck_assert_msg(r->loadModel == XmUNSPECIFIED_LOAD_MODEL,
	              "Expected XmUNSPECIFIED_LOAD_MODEL, got %d",
	              r->loadModel);
	ck_assert_msg(r->fontType == XmFONT_IS_FONT,
	              "Expected XmFONT_IS_FONT, got %d", r->fontType);
	XtFree(data);
	XmRenderTableFree(rt);
}
END_TEST

START_TEST(fromprop_all_props)
{
	char *data;
	Colormap cmap;
	XColor bg, fg;
	_XmTab tab;
	_XmTabList tabs;
	XmRenderTable rt;
	XmRendition rend;
	unsigned int i;
	struct __XmRenditionRec *r;
	struct __XmRenderTableRec *t;

	fg.red   = 0xcccc;
	fg.green = 0x3333;
	fg.blue  = 0x1111;
	cmap = DefaultColormap(display, DefaultScreen(display));
	if (!XAllocColor(display, cmap, &fg))
		fg.pixel = XmUNSPECIFIED_PIXEL;

	bg.red   = 0x2222;
	bg.green = 0xeeee;
	bg.blue  = 0x1111;
	if (!XAllocColor(display, cmap, &bg))
		bg.pixel = XmUNSPECIFIED_PIXEL;

	data = XtMalloc(sizeof all_props);
	memcpy(data, all_props, sizeof all_props);
	data[AP_FG_OFFSET]     = (fg.red >> 8) & 0xff;
	data[AP_FG_OFFSET + 1] = fg.red & 0xff;
	data[AP_FG_OFFSET + 2] = (fg.green >> 8) & 0xff;
	data[AP_FG_OFFSET + 3] = fg.green & 0xff;
	data[AP_FG_OFFSET + 4] = (fg.blue >> 8) & 0xff;
	data[AP_FG_OFFSET + 5] = fg.blue & 0xff;
	data[AP_BG_OFFSET]     = (bg.red >> 8) & 0xff;
	data[AP_BG_OFFSET + 1] = bg.red & 0xff;
	data[AP_BG_OFFSET + 2] = (bg.green >> 8) & 0xff;
	data[AP_BG_OFFSET + 3] = bg.green & 0xff;
	data[AP_BG_OFFSET + 4] = (bg.blue >> 8) & 0xff;
	data[AP_BG_OFFSET + 5] = bg.blue & 0xff;

	rt = XmRenderTableCvtFromProp(NULL, data, sizeof all_props);
	XtFree(data);
	ck_assert_msg((t = XmSharedPtrGet(rt)), "Expected to get a render table");
	ck_assert_msg(t->count == 1, "Expected one rendition (got %u)", t->count);
	ck_assert_msg((r = XmSharedPtrGet(*t->renditions)), "Expected a rendition");
	ck_assert_msg(r->tag, "Expected rendition to have a tag");
	ck_assert_msg(!strcmp(r->tag, all_props + AP_TAG_OFFSET),
	              "Expected tag %s, got %s", all_props + AP_TAG_OFFSET,
	              r->tag);
	ck_assert_msg(r->loadModel == XmLOAD_LAZY,
	              "Expected XmLOAD_LAZY, got %d", r->loadModel);
	ck_assert_msg(r->fontType == XmFONT_IS_XFT,
	              "Expected XmFONT_IS_XFT, got %d", r->fontType);
	ck_assert_msg(!strcmp(r->pattern, all_props + AP_PATTERN_OFFSET),
	              "Expected pattern %s, got %s",
	              all_props + AP_PATTERN_OFFSET, r->pattern);
	ck_assert_msg(!strcmp(r->fontStyle, all_props + AP_STYLE_OFFSET),
	              "Expected style %s, got %s",
	              all_props + AP_STYLE_OFFSET, r->fontStyle);
	ck_assert_msg(r->fontWeight == XmWEIGHT_BOLD,
	              "Expected XmWEIGHT_BOLD, got %d", r->fontWeight);
	ck_assert_msg(r->fontSlant == XmSLANT_OBLIQUE,
	              "Expected XmSLANT_OBLIQUE, got %d", r->fontSlant);
	ck_assert_msg(r->style.underline == XmSINGLE_DASHED_LINE,
	              "Expected underline XmSINGLE_DASHED_LINE, got %d", r->style.underline);
	ck_assert_msg(r->style.strikethru == XmSINGLE_LINE,
	              "Expected stikethru XmSINGLE_LINE, got %d", r->style.strikethru);
	ck_assert_msg(r->style.fg.pixel == fg.pixel,
	              "Expected fg.pixel 0x%08lx, got 0x%08lx", fg.pixel, r->style.fg.pixel);
	ck_assert_msg(r->style.bg.pixel == bg.pixel,
	              "Expected bg.pixel 0x%08lx, got 0x%08lx", bg.pixel, r->style.bg.pixel);
	ck_assert_msg(r->free_fg, "Expected r->free_fg to be True");
	ck_assert_msg(r->free_bg, "Expected r->free_bg to be True");

	ck_assert_msg((tabs = (_XmTabList)r->tabs), "Expected a tablist");
	ck_assert_msg(tabs->count == 2, "Expected 2 tabs, got %u", tabs->count);
	ck_assert_msg((tab = tabs->start), "Expected non-NULL starting tab");
	ck_assert_msg(tab->value == 1.0f,
	              "Expected tab 1's value to be 1.0f (got %f)",
	              tab->value);
	ck_assert_msg(tab->units == XmINCHES,
	              "Expected tab 1's units to be XmINCHES (got %d)",
	              tab->units);
	ck_assert_msg(tab->offsetModel == XmABSOLUTE,
	              "Expected tab 1's offsetModel to be XmABSOLUTE (got %d)",
	              tab->offsetModel);
	ck_assert_msg(tab->alignment == XmALIGNMENT_END,
	              "Expected tab 1's alignment to be XmALIGNMENT_END (got %d)",
	              tab->alignment);
	ck_assert_msg((tab = tab->next), "Expected non-NULL second tab");
	ck_assert_msg(tab->value == 2.0f,
	              "Expected tab 2's value to be 2.0f (got %f)",
	              tab->value);
	ck_assert_msg(tab->units == XmMILLIMETERS,
	              "Expected tab 2's units to be XmMILLIMETERS (got %d)",
	              tab->units);
	ck_assert_msg(tab->offsetModel == XmRELATIVE,
	              "Expected tab 1's offsetModel to be XmRELATIVE (got %d)",
	              tab->offsetModel);
	ck_assert_msg(tab->alignment == XmALIGNMENT_CENTER,
	              "Expected tab 1's alignment to be XmALIGNMENT_CENTER (got %d)",
	              tab->alignment);

	XFreeColors(display, cmap, &fg.pixel, 1, 0);
	XFreeColors(display, cmap, &bg.pixel, 1, 0);
	XmRenderTableFree(rt);
}
END_TEST

void xmrendertable_suite(SRunner *runner)
{
	TCase *t;
	Suite *s = suite_create("XmRenderTable");

	t = tcase_create("Create");
	tcase_add_test(t, create);
	tcase_add_checked_fixture(t, _init_xt, uninit_xt);
	tcase_set_timeout(t, 1);
	suite_add_tcase(s, t);

	t = tcase_create("Copy");
	tcase_add_test(t, copy_null_table);
	tcase_add_test(t, copy_no_tags);
	tcase_add_test(t, copy_no_match);
	tcase_add_test(t, copy_matches);
	tcase_add_checked_fixture(t, _init_xt, uninit_xt);
	tcase_set_timeout(t, 1);
	suite_add_tcase(s, t);

	t = tcase_create("GetTags");
	tcase_add_test(t, get_tags_null_table);
	tcase_add_test(t, get_tags_null_tag_list);
	tcase_add_test(t, get_tags_empty_table);
	tcase_add_test(t, get_tags);
	tcase_add_checked_fixture(t, _init_xt, uninit_xt);
	tcase_set_timeout(t, 1);
	suite_add_tcase(s, t);

	t = tcase_create("GetRenditions");
	tcase_add_test(t, get_renditions_null_table);
	tcase_add_test(t, get_renditions_null_tags);
	tcase_add_test(t, get_renditions_empty_table);
	tcase_add_test(t, get_renditions_no_count);
	tcase_add_test(t, get_renditions_no_match);
	tcase_add_test(t, get_renditions_null_tag);
	tcase_add_test(t, get_renditions_match);
	tcase_add_checked_fixture(t, _init_xt, uninit_xt);
	tcase_set_timeout(t, 1);
	suite_add_tcase(s, t);

	t = tcase_create("GetRendition");
	tcase_add_test(t, get_rendition_null_table);
	tcase_add_test(t, get_rendition_null_tag);
	tcase_add_test(t, get_rendition_empty_table);
	tcase_add_test(t, get_rendition_no_match);
	tcase_add_test(t, get_rendition_match);
	tcase_add_checked_fixture(t, _init_xt, uninit_xt);
	tcase_set_timeout(t, 1);
	suite_add_tcase(s, t);

	t = tcase_create("AddRenditions");
	tcase_add_test(t, add_null_renditions);
	tcase_add_test(t, add_no_count);
	tcase_add_test(t, add_null_table);
	tcase_add_test(t, add_merge_old);
	tcase_add_test(t, add_merge_new);
	tcase_add_test(t, add_merge_replace);
	tcase_add_test(t, add_merge_duplicate);
	tcase_add_test(t, add_merge_skip);
	tcase_add_test(t, add_copies_empty_tag);
	tcase_add_checked_fixture(t, _init_xt, uninit_xt);
	tcase_set_timeout(t, 1);
	suite_add_tcase(s, t);

	t = tcase_create("RemoveRenditions");
	tcase_add_test(t, remove_null_table);
	tcase_add_test(t, remove_null_tags);
	tcase_add_test(t, remove_no_count);
	tcase_add_test(t, remove_empty_table);
	tcase_add_test(t, remove_sole_entry);
	tcase_add_test(t, remove_one);
	tcase_add_test(t, remove_many);
	tcase_add_checked_fixture(t, _init_xt, uninit_xt);
	tcase_set_timeout(t, 1);
	suite_add_tcase(s, t);

	t = tcase_create("Resolve");
	tcase_add_test(t, resolve_null_table);
	tcase_add_test(t, resolve_no_tags);
	tcase_add_test(t, resolve_no_fallback);
	tcase_add_test(t, resolve_no_charset);
	tcase_add_test(t, resolve_no_default_charset);
	tcase_add_test(t, resolve_first_font);
	tcase_add_test(t, resolve_calls_no_rendition_callback);
	tcase_add_test(t, resolve_callback_supplies_rendition);
	tcase_add_test(t, resolve_cascades_styles);
	tcase_add_test(t, resolve_tags_without_fonts);
	tcase_add_checked_fixture(t, _init_xt, uninit_xt);
	tcase_set_timeout(t, 1);
	suite_add_tcase(s, t);

	t = tcase_create("GetDefaultExtents");
	tcase_add_test(t, default_extents_empty_table);
	tcase_add_checked_fixture(t, _init_xt, uninit_xt);
	tcase_set_timeout(t, 1);
	suite_add_tcase(s, t);

	t = tcase_create("CvtToProp");
	tcase_add_test(t, toprop_null_table);
	tcase_add_test(t, toprop_empty_table);
	tcase_add_test(t, toprop_basic_props);
	tcase_add_test(t, toprop_all_props);
	tcase_add_checked_fixture(t, _init_xt, uninit_xt);
	tcase_set_timeout(t, 1);
	suite_add_tcase(s, t);

	t = tcase_create("CvtFromProp");
	tcase_add_test(t, fromprop_null_prop);
	tcase_add_test(t, fromprop_zero_length);
	tcase_add_test(t, fromprop_no_rendition_tag);
	tcase_add_test(t, fromprop_basic_props);
	tcase_add_test(t, fromprop_all_props);
	tcase_add_checked_fixture(t, _init_xt, uninit_xt);
	tcase_set_timeout(t, 1);
	suite_add_tcase(s, t);
	srunner_add_suite(runner, s);
}

