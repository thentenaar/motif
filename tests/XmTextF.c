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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#include <Xm/TextF.h>
#include <Xm/AccTextT.h>
#include <Xm/TraitP.h>
#include <check.h>

#include "suites.h"

static Boolean called_vc;
static Widget shell, text;
static Display *display;

static void _init_xt(void)
{
	setenv("LANG", "C", 1);
	shell   = init_xt("check_XmTextF");
	display = XtDisplay(shell);
	text    = XmCreateTextField(shell, "text", NULL, 0);
	XtManageChild(text);
}

START_TEST(initial_state)
{
	XmString s = XmTextFieldGetXmString(text);
	ck_assert_msg(XmStringEmpty(s), "Initial value should be empty");
	XmStringFree(s);
	ck_assert_msg(!XmTextFieldGetLastPosition(text), "Last position should be 0 by default");
	ck_assert_msg(XmTextFieldGetEditable(text), "Should be editable by default");
	ck_assert_msg(XmTextFieldGetMaxLength(text) == INT_MAX, "Max length should be INT_MAX by default");
}
END_TEST

START_TEST(set_value)
{
	XmString t, s = XmStringCreate("test", XmFONTLIST_DEFAULT_TAG);
	XmTextFieldSetXmString(text, s);
	t = XmTextFieldGetXmString(text);
	ck_assert_msg(XmStringCompare(s, t), "Expected obtained value to equal set value");
	XmStringFree(s);
	XmStringFree(t);
	ck_assert_msg(XmTextFieldGetLastPosition(text) == 4, "Last position should equal text length");
}
END_TEST

static void set_mv_cb(Widget w, XtPointer client, XtPointer call)
{
	XmString s = XmStringCreate("test", XmFONTLIST_DEFAULT_TAG);
	XmTextVerifyCallbackStructStr *cb = (XmTextVerifyCallbackStructStr *)call;

	(void)w;
	(void)client;
	ck_assert_msg(cb->reason == XmCR_MODIFYING_TEXT_VALUE, "Unexpected reason value");
	ck_assert_msg(!cb->currInsert, "Expected 0 for insert position");
	ck_assert_msg(!cb->newInsert,  "Expected 0 for new insert position");
	ck_assert_msg(!cb->startPos,   "Expected 0 for start position");
	ck_assert_msg(!cb->endPos,     "Expected 0 for end position");
	ck_assert_msg(XmStringCompare(cb->str, s), "Expected string to be 'test'");
	XmStringFree(s);
	cb->doit = False; /* Prevent modification from proceeding */
}

START_TEST(set_calls_modify_verify_cb)
{
	XmString s = XmStringCreate("test", XmFONTLIST_DEFAULT_TAG);
	XtAddCallback(text, XmNmodifyVerifyCallbackStr, set_mv_cb, NULL);
	XmTextFieldSetXmString(text, s);
	XmStringFree(s);
	s = XmTextFieldGetXmString(text);
	ck_assert_msg(XmStringEmpty(s), "Expected obtained value to be empty");
	XmStringFree(s);
	called_vc = True;
}
END_TEST

static void set_vc_cb(Widget w, XtPointer client, XtPointer call)
{
	XmString s, t = XmStringCreate("test", XmFONTLIST_DEFAULT_TAG);
	XmAnyCallbackStruct *cb = (XmAnyCallbackStruct *)call;

	(void)w;
	(void)client;
	ck_assert_msg(cb->reason == XmCR_VALUE_CHANGED, "Unexpected reason value");
	s = XmTextFieldGetXmString(w);
	ck_assert_msg(XmStringCompare(s, t), "Expected obtained value to equal 'test'");
	XmStringFree(s);
	XmStringFree(t);
	called_vc = True;
}

START_TEST(set_calls_value_changed_cb)
{
	XmString s = XmStringCreate("test", XmFONTLIST_DEFAULT_TAG);
	XtAddCallback(text, XmNvalueChangedCallback, set_vc_cb, NULL);
	called_vc = False;
	XmTextFieldSetXmString(text, s);
	ck_assert_msg(called_vc, "value changed callback was not called");
	XmStringFree(s);
}
END_TEST

START_TEST(get_substring)
{
	XmString t, s = XmStringCreate("test", XmFONTLIST_DEFAULT_TAG);
	XmTextFieldSetXmString(text, s);
	XmStringFree(s);

	t = XmTextFieldSubstring(text, 0, 4);
	ck_assert_msg(XmStringCompare(t, s), "Substring mismatch");
	XmStringFree(t);
}
END_TEST

START_TEST(replace_string)
{
	XmString t, s = XmStringCreate("test", XmFONTLIST_DEFAULT_TAG);
	XmString replacement = XmStringCreate("b", XmFONTLIST_DEFAULT_TAG);
	XmTextFieldSetXmString(text, s);
	XmStringFree(s);

	s = XmStringCreate("best", XmFONTLIST_DEFAULT_TAG);
	XmTextFieldReplaceString(text, 0, 1, replacement);
	XmStringFree(replacement);

	t = XmTextFieldGetXmString(text);
	ck_assert_msg(XmStringCompare(t, s), "Unexpected value post-replacement");
	XmStringFree(t);
	XmStringFree(s);
	ck_assert_msg(XmTextFieldGetLastPosition(text) == 4, "Last position should equal text length");
}
END_TEST

START_TEST(insert_string)
{
	XmString t, s = XmStringCreate("test", XmFONTLIST_DEFAULT_TAG);
	XmString insertion = XmStringCreate("re", XmFONTLIST_DEFAULT_TAG);
	XmTextFieldSetXmString(text, s);
	XmStringFree(s);

	s = XmStringCreate("retest", XmFONTLIST_DEFAULT_TAG);
	XmTextFieldInsertString(text, 0, insertion);
	XmStringFree(insertion);

	t = XmTextFieldGetXmString(text);
	ck_assert_msg(XmStringCompare(t, s), "Unexpected value post-insertion");
	XmStringFree(t);
	XmStringFree(s);
	ck_assert_msg(XmTextFieldGetLastPosition(text) == 6, "Last position should equal text length");
}
END_TEST


/* void XmTextFieldSetMaxLength(Widget w, int max_length) */
/* int XmTextFieldGetMaxLength(Widget w) */
/* void XmTextFieldSetEditable(Widget w, Boolean editable) */
/* Boolean XmTextFieldGetEditable(Widget w) */
/* XmTextPosition XmTextFieldGetInsertionPosition(Widget w) */
/* void XmTextFieldSetInsertionPosition(Widget w, XmTextPosition position) */

/** Actions **/
START_TEST(backward_char)
{
	XmTextPosition pos = 4;
	XmString s = XmStringCreate("test", XmFONTLIST_DEFAULT_TAG);
	XmTextFieldSetXmString(text, s);
	XmStringFree(s);

	XtVaSetValues(text, XmNcursorPosition, pos, NULL);
	XtCallActionProc(text, "backward-character", NULL, NULL, 0);
	XtVaGetValues(text, XmNcursorPosition, &pos, NULL);
	ck_assert_msg(pos == 3, "Expected cursor to be one before end of string");

	pos = 0;
	XtVaSetValues(text, XmNcursorPosition, pos, NULL);
	XtCallActionProc(text, "backward-character", NULL, NULL, 0);
	XtVaGetValues(text, XmNcursorPosition, &pos, NULL);
	ck_assert_msg(pos == 0, "Expected cursor to be at the beginning of the string");
}
END_TEST

START_TEST(forward_char)
{
	XmTextPosition pos = 4;
	XmString s = XmStringCreate("test", XmFONTLIST_DEFAULT_TAG);
	XmTextFieldSetXmString(text, s);
	XmStringFree(s);

	XtVaSetValues(text, XmNcursorPosition, pos, NULL);
	XtCallActionProc(text, "forward-character", NULL, NULL, 0);
	XtVaGetValues(text, XmNcursorPosition, &pos, NULL);
	ck_assert_msg(pos == 4, "Expected cursor to be at the end of the string");

	pos = 0;
	XtVaSetValues(text, XmNcursorPosition, pos, NULL);
	XtCallActionProc(text, "forward-character", NULL, NULL, 0);
	XtVaGetValues(text, XmNcursorPosition, &pos, NULL);
	ck_assert_msg(pos == 1, "Expected cursor to be one beyond the start of the string");
}
END_TEST

START_TEST(backward_word)
{
	XmTextPosition pos = 9;
	XmString s = XmStringCreate("test tset", XmFONTLIST_DEFAULT_TAG);
	XmTextFieldSetXmString(text, s);
	XmStringFree(s);

	XtVaSetValues(text, XmNcursorPosition, pos, NULL);
	XtCallActionProc(text, "backward-word", NULL, NULL, 0);
	XtVaGetValues(text, XmNcursorPosition, &pos, NULL);
	ck_assert_msg(pos == 5, "Expected cursor to be at the previous word");

	pos = 0;
	XtVaSetValues(text, XmNcursorPosition, pos, NULL);
	XtCallActionProc(text, "backward-word", NULL, NULL, 0);
	XtVaGetValues(text, XmNcursorPosition, &pos, NULL);
	ck_assert_msg(pos == 0, "Expected cursor to be at the beginning of the string");
}
END_TEST

START_TEST(forward_word)
{
	XmTextPosition pos = 9;
	XmString s = XmStringCreate("test tset", XmFONTLIST_DEFAULT_TAG);
	XmTextFieldSetXmString(text, s);
	XmStringFree(s);

	XtVaSetValues(text, XmNcursorPosition, pos, NULL);
	XtCallActionProc(text, "forward-word", NULL, NULL, 0);
	XtVaGetValues(text, XmNcursorPosition, &pos, NULL);
	ck_assert_msg(pos == 9, "Expected cursor to be at the end of the string");

	pos = 0;
	XtVaSetValues(text, XmNcursorPosition, pos, NULL);
	XtCallActionProc(text, "forward-word", NULL, NULL, 0);
	XtVaGetValues(text, XmNcursorPosition, &pos, NULL);
	ck_assert_msg(pos == 5, "Expected cursor to be at the next word");
	XtCallActionProc(text, "forward-word", NULL, NULL, 0);
	XtVaGetValues(text, XmNcursorPosition, &pos, NULL);
	ck_assert_msg(pos == 9, "Expected cursor to be at the end of the string");
}
END_TEST

START_TEST(end_of_line)
{
	XmTextPosition pos = 9;
	XmString s = XmStringCreate("test tset", XmFONTLIST_DEFAULT_TAG);
	XmTextFieldSetXmString(text, s);
	XmStringFree(s);

	XtVaSetValues(text, XmNcursorPosition, pos, NULL);
	XtCallActionProc(text, "end-of-line", NULL, NULL, 0);
	XtVaGetValues(text, XmNcursorPosition, &pos, NULL);
	ck_assert_msg(pos == 9, "Expected cursor to be at the end of the string");

	pos = 0;
	XtVaSetValues(text, XmNcursorPosition, pos, NULL);
	XtCallActionProc(text, "end-of-line", NULL, NULL, 0);
	XtVaGetValues(text, XmNcursorPosition, &pos, NULL);
	ck_assert_msg(pos == 9, "Expected cursor to be at the end of the string");
}
END_TEST

START_TEST(beginning_of_line)
{
	XmTextPosition pos = 9;
	XmString s = XmStringCreate("test tset", XmFONTLIST_DEFAULT_TAG);
	XmTextFieldSetXmString(text, s);
	XmStringFree(s);

	XtVaSetValues(text, XmNcursorPosition, pos, NULL);
	XtCallActionProc(text, "beginning-of-line", NULL, NULL, 0);
	XtVaGetValues(text, XmNcursorPosition, &pos, NULL);
	ck_assert_msg(pos == 0, "Expected cursor to be at the beginning of the string");
	XtCallActionProc(text, "beginning-of-line", NULL, NULL, 0);
	XtVaGetValues(text, XmNcursorPosition, &pos, NULL);
	ck_assert_msg(pos == 0, "Expected cursor to be at the beginning of the string");
}
END_TEST

/* AccessTextual trait */
START_TEST(has_access_textual)
{
	XmAccessTextualTrait t = XmeTraitGet(XtClass(text), XmQTaccessTextual);
	ck_assert_msg(t, "Expected to find the access textual trait");
	ck_assert_msg(t->preferredFormat(text) == XmFORMAT_XmSTRING,
	              "Expected the preferred format to be XmFORMAT_XMSTRING");
}
END_TEST

START_TEST(trait_access)
{
	XmString q, s = XmStringCreate("access textual", XmFONTLIST_DEFAULT_TAG);
	XmAccessTextualTrait t = XmeTraitGet(XtClass(text), XmQTaccessTextual);
	t->setValue(text, s, XmFORMAT_XmSTRING);
	q = t->getValue(text, XmFORMAT_XmSTRING);
	ck_assert_msg(XmStringCompare(q, s),
	              "Expected the resulting string to equal the supplied string");
	XmStringFree(q);
	XmStringFree(s);
}
END_TEST

START_TEST(get_null_if_bad_format)
{
	XmAccessTextualTrait t = XmeTraitGet(XtClass(text), XmQTaccessTextual);
	ck_assert_msg(!t->getValue(text, -1), "A bad format value should have a NULL result");
}
END_TEST

/**
 * ************* Deprecated Routines ******************
 *
 * These need to be tested to ensure that the callbacks are called
 * correctly to avoid breaking exising usages of these calls.
 */
#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#elif defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif

START_TEST(set_value_string)
{
	static const char *s = "test", *t = NULL;
	XmTextFieldSetString(text, s);
	t = XmTextFieldGetString(text);
	ck_assert_msg(t && !strcmp(t, s), "Expected obtained value to equal set value");
	XtFree(t);
}
END_TEST

START_TEST(set_value_wcs)
{
	static const wchar_t *s = L"test", *t = NULL;
	XmTextFieldSetStringWcs(text, s);
	t = XmTextFieldGetStringWcs(text);
	ck_assert_msg(t && !wcscmp(t, s), "Expected obtained value to equal set value");
	XtFree((XtPointer)t);
}
END_TEST

static void set_mv_cb_string(Widget w, XtPointer client, XtPointer call)
{
	static const char *s = "test";
	XmTextVerifyCallbackStruct *cb = (XmTextVerifyCallbackStruct *)call;

	(void)w;
	(void)client;
	ck_assert_msg(cb->reason == XmCR_MODIFYING_TEXT_VALUE, "Unexpected reason value");
	ck_assert_msg(!cb->currInsert, "Expected 0 for insert position");
	ck_assert_msg(!cb->newInsert,  "Expected 0 for new insert position");
	ck_assert_msg(!cb->startPos,   "Expected 0 for start position");
	ck_assert_msg(!cb->endPos,     "Expected 0 for end position");
	ck_assert_msg(cb->text->length == 4, "Expected text length to be 4");
	ck_assert_msg(!strcmp(cb->text->ptr, s), "Expected string to be 'test'");
	cb->doit = False; /* Prevent modification from proceeding */
}

static void set_mv_cb_wcs(Widget w, XtPointer client, XtPointer call)
{
	static const wchar_t *s = L"test";
	XmTextVerifyCallbackStructWcs *cb = (XmTextVerifyCallbackStructWcs *)call;

	(void)w;
	(void)client;
	ck_assert_msg(cb->reason == XmCR_MODIFYING_TEXT_VALUE, "Unexpected reason value");
	ck_assert_msg(!cb->currInsert, "Expected 0 for insert position");
	ck_assert_msg(!cb->newInsert,  "Expected 0 for new insert position");
	ck_assert_msg(!cb->startPos,   "Expected 0 for start position");
	ck_assert_msg(!cb->endPos,     "Expected 0 for end position");
	ck_assert_msg(cb->text->length == 4, "Expected text length to be 4");
	ck_assert_msg(!wcscmp(cb->text->wcsptr, s), "Expected string to be 'test'");
	cb->doit = False; /* Prevent modification from proceeding */
}

START_TEST(set_calls_modify_verify_cb_string)
{
	static const char *s = "test", *t = NULL;
	XtAddCallback(text, XmNmodifyVerifyCallback, set_mv_cb_string, NULL);
	XmTextFieldSetString(text, s);
	t = XmTextFieldGetString(text);
	ck_assert_msg(t && !*t, "Expected obtained value to be empty");
	XtFree((XtPointer)t);
	called_vc = True;
}
END_TEST

START_TEST(set_calls_modify_verify_cb_wcs)
{
	static const wchar_t *s = L"test", *t = NULL;
	XtAddCallback(text, XmNmodifyVerifyCallbackWcs, set_mv_cb_wcs, NULL);
	XmTextFieldSetStringWcs(text, s);
	t = XmTextFieldGetStringWcs(text);
	ck_assert_msg(t && !*t, "Expected obtained value to be empty");
	XtFree((XtPointer)t);
	called_vc = True;
}
END_TEST

#if defined(__GNUC__)
#pragma GCC diagnostic pop
#elif defined(__clang__)
#pragma clang diagnostic pop
#endif

void xmtextf_suite(SRunner *runner)
{
	TCase *t;
	Suite *s = suite_create("XmTextF");

	t = tcase_create("Init");
	tcase_add_test(t, initial_state);
	tcase_add_checked_fixture(t, _init_xt, uninit_xt);
	tcase_set_timeout(t, 1);
	suite_add_tcase(s, t);

	t = tcase_create("Set Value");
	tcase_add_test(t, set_value);
	tcase_add_test(t, set_calls_modify_verify_cb);
	tcase_add_test(t, set_calls_value_changed_cb);
	tcase_add_checked_fixture(t, _init_xt, uninit_xt);
	tcase_set_timeout(t, 1);
	suite_add_tcase(s, t);

	t = tcase_create("String Manipulation");
	tcase_add_test(t, get_substring);
	tcase_add_test(t, replace_string);
	tcase_add_test(t, insert_string);
	tcase_add_checked_fixture(t, _init_xt, uninit_xt);
	tcase_set_timeout(t, 1);
	suite_add_tcase(s, t);

	t = tcase_create("Actions");
	tcase_add_test(t, backward_char);
	tcase_add_test(t, forward_char);
	tcase_add_test(t, backward_word);
	tcase_add_test(t, forward_word);
	tcase_add_test(t, end_of_line);
	tcase_add_test(t, beginning_of_line);
	tcase_add_checked_fixture(t, _init_xt, uninit_xt);
	tcase_set_timeout(t, 1);
	suite_add_tcase(s, t);

	t = tcase_create("Trait");
	tcase_add_test(t, has_access_textual);
	tcase_add_test(t, trait_access);
	tcase_add_test(t, get_null_if_bad_format);
	tcase_add_checked_fixture(t, _init_xt, uninit_xt);
	tcase_set_timeout(t, 1);
	suite_add_tcase(s, t);

	t = tcase_create("Deprecated Routines");
	tcase_add_test(t, set_value_string);
	tcase_add_test(t, set_value_wcs);
	tcase_add_test(t, set_calls_modify_verify_cb_string);
	tcase_add_test(t, set_calls_modify_verify_cb_wcs);
	tcase_add_checked_fixture(t, _init_xt, uninit_xt);
	tcase_set_timeout(t, 1);
	suite_add_tcase(s, t);
	srunner_add_suite(runner, s);
}

