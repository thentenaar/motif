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

#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#include "SharedPtrI.h"
#include "XmStringI.h"
#include "XmRenderTI.h"
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

	ck_assert_msg((*rend)->style.fg.pixel == (*t->renditions[2])->style.fg.pixel,
	              "Expected rend->style.fg.pixel (0x%08lx) to be 0x%08lx",
	              (*rend)->style.fg.pixel, (*t->renditions[2])->style.fg.pixel);
	ck_assert_msg((*rend)->style.underline == XmAS_IS,
	              "Expected rend->style.underline (0x%x) to be XmAS_IS",
	              (*rend)->style.underline);
	ck_assert_msg((*rend)->style.strikethru == XmAS_IS,
	              "Expected rend->style.strikethru (0x%x) to be XmAS_IS (%x)",
	              (*rend)->style.underline, XmAS_IS);
	ck_assert_msg((*rend)->font == (void *)0xdeadbeef,
	              "Expected rend->font (%p) to be 0xdeadbeef",
	              (*rend)->font);
	XmRenderTableFree(rt);
	XmRenditionFree(rend);
	XmRenditionStyleFree(style);
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
	tcase_add_checked_fixture(t, _init_xt, uninit_xt);
	tcase_set_timeout(t, 1);
	suite_add_tcase(s, t);

	t = tcase_create("GetDefaultExtents");
	tcase_add_test(t, default_extents_empty_table);
	tcase_add_checked_fixture(t, _init_xt, uninit_xt);
	tcase_set_timeout(t, 1);
	suite_add_tcase(s, t);
	srunner_add_suite(runner, s);
}

