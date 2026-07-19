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
#ifndef XM_CHARI_H
#define XM_CHARI_H

#include <limits.h>
#include "XmChar.h"

/**
 * Props: 0000 0000 0000 0000 0000 0000 0000 0000
 *        ^^^^   ^       ^      ^     ^  ^   ^---- Decimal Value (4 bits)
 *        ||||   |       |      |     |  |
 *        ||||   |       |      |     |  --------- Alphanumeric Type (3 bits)
 *        ||||   |       |      |     ------------ Casing (2 bits)
 *        ||||   |       |      ------------------ Word Break (5 bits)
 *        ||||   |       ------------------------- Line Break (6 bits)
 *        ||||   --------------------------------- Props (6 bits)
 *        |||------------------------------------- Quick Check (3 bits)
 *        ||-------------------------------------- Combine Forward (1 bit)
 *        |--------------------------------------- Combine Backward (1 bit)
 *        ---------------------------------------- Reserved (1 bit)
 */
struct codepoint_info {
#if UINT_MAX >= (1 << 32) - 1
	unsigned int props;     /**< General properties                 */
#else
	unsigned long props;
#endif
	unsigned char ccc;      /**< Combining class (for ordering)     */
	unsigned char n_comp;   /**< Number of composition entries      */
	unsigned char n_decomp; /**< Number of characters in sequence   */
	unsigned short comp;    /**< Index into the composition table   */
	unsigned short decomp;  /**< Index into the decomposition table */
};

/* Decimal value */
#define DECIMAL_MASK 0xf

/**
 * Unicode Character Properties
 *
 * https://www.unicode.org/Public/UCD/latest/ucd/DerivedCoreProperties.txt
 * https://www.unicode.org/Public/UCD/latest/ucd/extracted/DerivedNumericType.txt
 * https://www.unicode.org/Public/UCD/latest/ucd/emoji/emoji-data.txt
 */
enum codepoint_alnum {
	AN_None = 0,
	AN_Alphabetic,
	AN_Numeric,
	AN_Digit,
	AN_Decimal
};

#define ALNUM_COUNT 4
#define ALNUM_SHIFT 4
#define ALNUM_MASK  (0x7 << ALNUM_SHIFT)

enum codepoint_case {
	CC_None = 0,
	CC_Lowercase,
	CC_Uppercase,
	CC_Titlecase
};

#define CASE_COUNT 3
#define CASE_SHIFT 7
#define CASE_MASK  (0x3 << CASE_SHIFT)

enum codepoint_props {
	CP_None                 = 0, /* default */
	CP_ExtendedPictographic = (1 << 1),
	CP_Emoji                = (1 << 2),
	CP_EmojiPresentation    = (1 << 3),
	CP_EmojiModifier        = (1 << 4),
	CP_EmojiModifierBase    = (1 << 5),
	CP_EmojiComponent       = (1 << 6)
};

#define PROP_COUNT 6
#define PROP_SHIFT 20
#define PROP_MASK  (0x3f << PROP_SHIFT)

/**
 * Unicode WordBreak Properties
 * https://www.unicode.org/Public/UCD/latest/ucd/auxiliary/WordBreakProperty.txt
 */
enum word_break_props {
    WB_Other = 0, /* Default */
    WB_CR,
    WB_LF,
    WB_Newline,
    WB_Extend,
    WB_ZWJ,
    WB_Regional_Indicator,
    WB_Format,
    WB_Katakana,
    WB_Hebrew_Letter,
    WB_ALetter,
    WB_SingleQuote,
    WB_DoubleQuote,
    WB_MidNumLet,
    WB_MidLetter,
    WB_MidNum,
    WB_Numeric,
    WB_ExtendNumLet,
    WB_WSegSpace
};

#define WB_COUNT 19
#define WB_SHIFT 9
#define WB_MASK  (0x1f << WB_SHIFT)

/**
 * Unicode LineBreak Properties
 *
 * https://www.unicode.org/Public/UCD/latest/ucd/extracted/DerivedLineBreak.txt
 */
enum line_break_props {
	LB_XX = 0, /* default                              */
	LB_BK,     /* Mandatory Break                      */
	LB_CM,     /* Combining Mark                       */
	LB_CR,     /* Carriage Return                      */
	LB_GL,     /* Non-breaking Glue                    */
	LB_LF,     /* Line Feed                            */
	LB_NL,     /* Next Line                            */
	LB_SP,     /* Space                                */
	LB_WJ,     /* Word Joiner                          */
	LB_ZW,     /* Zero Width Space                     */
	LB_ZWJ,    /* Zero Width Joiner                    */
	LB_AI,     /* Ambiguous Ideographic                */
	LB_AK,     /* Aksara                               */
	LB_AL,     /* Alphabetic                           */
	LB_AP,     /* Aksara Pre-base                      */
	LB_AS,     /* Aksara Start                         */
	LB_B2,     /* Break opportunity before + after     */
	LB_BA,     /* Break after spaces                   */
	LB_BB,     /* Break before                         */
	LB_CB,     /* Contingent break opportunity         */
	LB_CJ,     /* Conditional Japanese starter         */
	LB_CL,     /* Close punctuation                    */
	LB_CP,     /* Close parenthesis                    */
	LB_EB,     /* Emoji base                           */
	LB_EM,     /* Emoji modifier                       */
	LB_EX,     /* Exclamation / Interrogation          */
	LB_H2,     /* Hangul LV syllable                   */
	LB_H3,     /* Hangul LVT syllable                  */
	LB_HH,     /* Unambiguous Hyphen                   */
	LB_HL,     /* Hebrew Letter                        */
	LB_HY,     /* Hyphen                               */
	LB_ID,     /* Ideographic                          */
	LB_IN,     /* Inseparable Leader                   */
	LB_IS,     /* Infix Numeric Separator              */
	LB_JL,     /* Hangul L Jamo                        */
	LB_JT,     /* Hangul T Jamo                        */
	LB_JV,     /* Hangul V Jamo                        */
	LB_NS,     /* Nonstarter                           */
	LB_NU,     /* Numeric Digits                       */
	LB_OP,     /* Open Punctuation                     */
	LB_PO,     /* Postfix Numeric                      */
	LB_PR,     /* Prefix Numeric                       */
	LB_QU,     /* Quotation                            */
	LB_RI,     /* Regional Indicator                   */
	LB_SA,     /* Complex Context Dependent (SE Asian) */
	LB_SY,     /* Symbols Allowing Break After         */
	LB_VF,     /* Virama Final                         */
	LB_VI      /* Virama                               */
};

#define LB_COUNT 0x30
#define LB_SHIFT 14
#define LB_MASK  (0x3f << LB_SHIFT)

/**
 * QuickCheck states for normalization
 *
 * https://www.unicode.org/Public/UCD/latest/ucd/DerivedNormalizationProps.txt
 */
enum quick_check_props {
	QC_Standard = 0,    /* Standard characters (NFC_QC=Y, NFD_QC=Y) */
	QC_BackwardCombine, /* Backwards combining marks (NFC_QC=Maybe, NFD_QC=Y) */
	QC_Precomposed,     /* Precomposed characters (NFC_QC=Y, NFD_QC=N) */
	QC_Exclusion,       /* Singletons / Exclusions (NFC_QC=N, NFD_QC=N) */
	QC_BackwardDecomp   /* Backwards combining decomposables (NFC_QC=Maybe, NFD_QC=N) */
};

#define QC_LEN   5
#define QC_SHIFT 26
#define QC_MASK  (0x7 << QC_SHIFT)

/**
 * Bit which indicates whether or not this codepoint combines forward.
 */
#define CF_SHIFT 29
#define CF_MASK (1 << CF_SHIFT)

/**
 * Bit indicating if this codepoint can combine backward.
 */
#define CB_SHIFT 30
#define CB_MASK (1 << CB_SHIFT)

/**
 * Composition table entry
 */
struct comp_entry {
	XmCodepoint comb;   /**< Combining character */
	XmCodepoint result; /**< Resulting character */
};

#endif /* XM_CHARI_H */
