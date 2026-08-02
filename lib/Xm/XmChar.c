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

#include <errno.h>

#include "XmChar.h"
#include "XmCharI.h"
#include "unicode.h"

/**
 * Determine the length of a UTF-8 character based on it's initial
 * four bits (0 for contiuation bytes).
 */
static const unsigned int utf8_len[16] = {
	1, 1, 1, 1, 1, 1, 1, 1,
	0, 0, 0, 0, 2, 2, 3, 4
};

/**
 * Get the length (in bytes) of the given XmChar
 */
unsigned int XmCharLen(const XmChar c)
{
	return utf8_len[(*c & 0xf0) >> 4];
}

/**
 * Decode a XmChar to a Unicode codepoint
 */
XmCodepoint XmCharToCodepoint(const XmChar c)
{
	unsigned int i, len;
	XmCodepoint cp = 0;

	if (!c || !*c)
		return 0;

	if ((len = XmCharLen(c)) == 1)
		return c[0];

	switch (len) {
	case 2: return ((c[0] & 0x1f) << 6) | (c[1] & 0x3f);
	case 3:
		cp = ((c[0] & 0x0f) << 12) | ((c[1] & 0x3f) << 6) | (c[2] & 0x3f);
		if (cp >= 0xd800 && cp <= 0xdfff)
			return XM_INVALID_CODEPOINT;
		return cp;
	default:
		/* Ensure we have the mandatory zero bit */
		if ((c[0] & 0xf8) != 0xf0)
			return XM_INVALID_CODEPOINT;
		return ((c[0] & 0x07) << 18) | ((c[1] & 0x1f) << 12) |
		        ((c[2] & 0x3f) << 6) | (c[3] & 0x3f);
	}
}

/**
 * Encode a Unicode codepoint to XmChar.
 *
 * Note: The resulting XmChar must be freed via XtFree().
 */
XmChar XmCodepointToChar(XmCodepoint cp)
{
	XmChar c;
	unsigned int len = 4;

	/* Use a non-character for out-of-range values / UTF-16 surrogates */
	if (cp > XM_CODEPOINT_MAX || (cp >= 0xd800 && cp <= 0xdfff))
		cp = XM_INVALID_CODEPOINT;

	if (cp < 0x80)         len = 1;
	else if (cp < 0x800)   len = 2;
	else if (cp < 0x10000) len = 3;
	c = (XmChar)XtMalloc(len);

	switch (len) {
	case 1:
		c[0] = cp & 0xff;
		break;
	case 2:
		c[0] = 0xc0 | (cp >> 6);
		c[1] = 0x80 | (cp & 0x3f);
		break;
	case 3:
		c[0] = 0xe0 | (cp  >> 12);
		c[1] = 0x80 | ((cp >> 6) & 0x3f);
		c[2] = 0x80 | (cp & 0x3f);
		break;
	default:
		c[0] = 0xf0 | (cp  >> 18);
		c[1] = 0x80 | ((cp >> 12) & 0x3f);
		c[2] = 0x80 | ((cp >> 6)  & 0x3f);
		c[3] = 0x80 | (cp & 0x3f);
	}

	return c;
}

/**
 * Determine whether a word boundary exists between the two
 * given codepoints based on the tr29 default rules.
 */
Boolean XmCodepointIsWordBoundary(XmCodepoint a, XmCodepoint b)
{
	enum codepoint_props cp;
	enum word_break_props wba, wbb;

	if (a > XM_CODEPOINT_MAX || b > XM_CODEPOINT_MAX)
		return True;

	wba = (UCD_PROPS(a).props & WB_MASK)   >> WB_SHIFT;
	wbb = (UCD_PROPS(b).props & WB_MASK)   >> WB_SHIFT;
	cp  = (UCD_PROPS(b).props & PROP_MASK) >> PROP_SHIFT;

	/* WB3c: Do not break within emoji zwj sequences */
	if (wba == WB_ZWJ && cp == CP_ExtendedPictographic)
		return False;

	return wb_table[wba][wbb];
}

/**
 * Returns True if cp is a Hangul Jamo, syllable, or syllabic block
 */
Boolean XmCodepointIsHangul(XmCodepoint cp)
{
	enum line_break_props lb;

	lb = (UCD_PROPS(cp).props & LB_MASK) >> LB_SHIFT;
	return lb == LB_JL || lb == LB_JV || lb == LB_JT ||
	       lb == LB_H2 || lb == LB_H3;
}

/**
 * Compose two codepoints, if able.
 *
 * Returns the composed codepoint, or XM_INVALID_CODEPOINT on error with
 * errno set to indicate the reason (EINVAL: invalid codepoint passed;
 * ENOTSUP: The two codepoints can't combine).
 */
XmCodepoint XmCodepointCompose(XmCodepoint a, XmCodepoint b)
{
	unsigned short i;
	enum line_break_props lba, lbb;

	if (a > XM_CODEPOINT_MAX || b > XM_CODEPOINT_MAX) {
		errno = EINVAL;
		return XM_INVALID_CODEPOINT;
	}

	if (XmCodepointIsHangul(a) && XmCodepointIsHangul(b)) {
		lba = (UCD_PROPS(a).props & LB_MASK) >> LB_SHIFT;
		lbb = (UCD_PROPS(b).props & LB_MASK) >> LB_SHIFT;

		if (lba == LB_JL && lbb == LB_JV) /* L + V */
			return SBASE + (a - LBASE) * NCOUNT + (b - VBASE) * TCOUNT;

		if (lba == LB_H2 && lbb == LB_JT) /* LV + T */
			return a + (b - TBASE);

		errno = ENOTSUP;
		return XM_INVALID_CODEPOINT;
	}

	/* These lists are small, so a linear probe is good enough */
	for (i = 0; i < UCD_PROPS(a).n_comp; i++) {
		if (comp_table[UCD_PROPS(a).comp + i].comb == b)
			return comp_table[UCD_PROPS(a).comp + i].result;
	}

	errno = ENOTSUP;
	return XM_INVALID_CODEPOINT;
}

/**
 * Decompose a codepoint into the given buffer.
 *
 * \param cp Codepoint to decompose
 * \param buf Buffer to write decomposed codepoints to
 * \param len Length of \a buf (in codepoints)
 *
 * Returns the number of codepoints written to the buffer, or 0 if the
 * codepoint could not be decomposed or if an error occured. errno will
 * be set to EINVAL if the parameters given are invalid; ENOSPC if the
 * buffer has insufficient space.
 *
 * If the codepoint doesn't decompose, it gets copied into the buffer,
 * and 1 is returned.
 */
size_t XmCodepointDecompose(XmCodepoint cp, XmCodepoint *buf, size_t len)
{
	size_t out;
	enum line_break_props lb;

	if (!buf || !len || cp > XM_CODEPOINT_MAX) {
		errno = EINVAL;
		return 0;
	}

	if (XmCodepointIsHangul(cp)) {
		lb = (UCD_PROPS(cp).props & LB_MASK) >> LB_SHIFT;
		if (lb != LB_H2 && lb != LB_H3)
			goto nodecomp;

		if (len < 3)
			goto nospc;

		cp    -= SBASE;
		buf[0] = LBASE + cp / NCOUNT;
		buf[1] = VBASE + (cp % NCOUNT) / TCOUNT;
		buf[2] = TBASE + cp % TCOUNT;
		return 2 + (buf[2] != TBASE);
	}

	/* Decompose according to our table */
	if (!(out = UCD_PROPS(cp).n_decomp))
		goto nodecomp;

	if (out <= len) {
		memcpy(buf, decomp_table + UCD_PROPS(cp).decomp, out * sizeof *buf);
		return out;
	}

nospc:
	errno = ENOSPC;
	return 0;

nodecomp:
	*buf = cp;
	return 1;
}

/**
 * Sort combining marks in-place according to the Canonical Ordering
 * Algorithm. Larger values for ccc go further down the sequence, while
 * batches of similar marks should not be reordered.
 */
static void sort_marks(XmCodepoint *buf, size_t len)
{
	size_t i, j;
	XmCodepoint c;
	unsigned int ccc, ccx = 1;

	for (i = 0; i < len; i++) {
		if (!UCD_PROPS(buf[i]).ccc)
			continue;

		for (j = i; j < len - 1; j++) {
			ccc = UCD_PROPS(buf[j]).ccc;
			ccx = UCD_PROPS(buf[j + 1]).ccc;
			if (!ccc || !ccx)
				break;

			if (ccc > ccx) {
				c = buf[j];
				buf[j] = buf[j + 1];
				buf[j + 1] = c;
			}
		}
	}
}

/**
 * Perform a NFC normalization pass over the given set of codepoints,
 * assuming they're NFD-normal and combining marks have been properly
 * reordered.
 */
static XmCodepoint *normalize_nfc(const XmCodepoint *buf, size_t len,
                                  size_t *len_out)
{
	unsigned int qca, qcb;
	size_t i, out_len = 0;
	XmCodepoint *out = NULL, c, d;

	for (i = 0; i < len; i++) {
		/**
		 * Excluded codepoints (NFC_QC=N).
		 */
		qca = (UCD_PROPS(buf[i]).props & QC_MASK) >> QC_SHIFT;
		if (qca == QC_Exclusion)
			continue;

		/**
		 * NFC_QC=M indicates that this is an isolated combining
		 * mark. Copy it out and continue.
		 */
		if (qca == QC_BackwardDecomp || qca == QC_BackwardCombine) {
			out = (XmCodepoint *)XtRealloc((XtPointer)out, (out_len + 1) * sizeof *out);
			out[out_len++] = buf[i];
			continue;
		}

		/**
		 * Here, we have NFC_QC=Y / NFC_QC=M, and can possibly compose.
		 * If the composition fails, emit the current codepoint, and
		 * continue on.
		 */
		c = buf[i];
		while (i < len) {
			d = XmCodepointCompose(c, buf[++i]);
			if (d == XM_INVALID_CODEPOINT) {
				out = (XmCodepoint *)XtRealloc((XtPointer)out, (out_len + 1) * sizeof *out);
				out[out_len++] = c;
				--i;
				break;
			}

			c = d;
		}
	}

	if (len_out) *len_out = out_len;
	return out;
}

/**
 * Perform a NFD normalization pass over the given set of codepoints.
 */
static XmCodepoint *normalize_nfd(const XmCodepoint *buf, size_t len,
                                  size_t *len_out)
{
	unsigned int qc;
	size_t i, x, decomp_len = 3, out_len = 0;
	XmCodepoint *out = NULL, *decomp;

	decomp = (XmCodepoint *)XtMalloc(3 * sizeof *decomp);
	for (i = 0; i < len; i++) {
		qc = (UCD_PROPS(buf[i]).props & QC_MASK) >> QC_SHIFT;

		/**
		 * NFD_QC=Y indicates this codepoint is already D-normal.
		 */
		if (qc == QC_Standard || qc == QC_BackwardCombine) {
			out = (XmCodepoint *)XtRealloc((XtPointer)out, (out_len + 1) * sizeof *out);
			out[out_len++] = buf[i];
			continue;
		}

again:
		/**
		 * NFD_QC=N here implies this codepoint should decompose.
		 */
		x = XmCodepointDecompose(buf[i], decomp, decomp_len);
		if (!x && errno == ENOSPC) {
			if (decomp_len > 8) {
				/**
				 * This shouldn't happen, but this guards against
				 * runaway if we somehow have corrupted data in our
				 * decomposition table, at the expense of the current
				 * codepoint.
				 */
				continue;
			}

			decomp_len += 2;
			decomp = (XmCodepoint *)XtRealloc((XtPointer)decomp, decomp_len * sizeof *decomp);
			goto again;
		}

		/* Add our decomposition to the out buffer */
		out = (XmCodepoint *)XtRealloc((XtPointer)out, (out_len + x) * sizeof *out);
		memcpy(out + out_len, decomp, x * sizeof *decomp);
		out_len += x;
	}

	sort_marks(out, out_len);
	XtFree((XtPointer)decomp);
	if (len_out) *len_out = out_len;
	return out;
}

/**
 * Canonically normalize a series of codepoints.
 *
 * Returns a newly-allocated buffer containing the normalized form of
 * the codepoints from \a buf, with \a len_out being set to the number of
 * codepoints written to the resultant buffer. NULL will be returned if
 * the input parameters are invalid.
 *
 * For NFC, this function will first apply NFD before performing NFC
 * normalization.
 */
XmCodepoint *XmCodepointNormalize(enum XmCodepointNormalForm form,
                                  const XmCodepoint *buf, size_t len,
                                  size_t *len_out)
{
	size_t norm_len;
	XmCodepoint *norm, *out;

	if (len_out) *len_out   = 0;

	if (!buf || !len)
		return NULL;

	norm = normalize_nfd(buf, len, &norm_len);
	if (form == XM_CODEPOINT_NORM_NFD) {
		if (len_out) *len_out = norm_len;
		return norm;
	}

	out = normalize_nfc(norm, norm_len, len_out);
	XtFree((XtPointer)norm);
	return out;
}

