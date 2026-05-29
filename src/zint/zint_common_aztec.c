/* zint_common_aztec.c - helpers cherry-picked from upstream zint common.c */
#include "common.h"
#include <stdarg.h>
#include <assert.h>

INTERNAL int z_to_int(const unsigned char source[], const int length) {
    int val = 0;
    int non_digit = 0;
    int i;

    for (i = 0; i < length; i++) {
        val *= 10;
        val += source[i] - '0';
        non_digit |= !z_isdigit(source[i]);
    }

    return non_digit ? -1 : val;
}

INTERNAL int z_chr_cnt(const unsigned char source[], const int length, const unsigned char ch) {
    int count = 0;
    int i;

    for (i = 0; i < length; i++) {
        count += source[i] == ch;
    }
    return count;
}

INTERNAL int z_extra_escape_position_fnc1(const unsigned char source[], const int length) {
    if (length >= 3) {
        if (source[0] == '\\' && source[1] == '^' && source[2] == '1') {
            return 3;
        }
        if (length >= 4) {
            if (z_isalpha(source[0]) && source[1] == '\\' && source[2] == '^' && source[3] == '1') {
                return 4;
            }
            if (length >= 5) {
                if (z_isdigit(source[0]) && z_isdigit(source[1]) && source[2] == '\\' && source[3] == '^'
                        && source[4] == '1') {
                    return 5;
                }
            }
        }
    }
    return 0;
}

/* Zero-fill `dest` buffer, appending `source'. Returns no. of zeroes added */
INTERNAL int z_zero_fill(const unsigned char source[], const int length, unsigned char *dest, const int dest_length) {
    const int zeroes = dest_length - length;
    if (zeroes >= 0) {
        if (zeroes > 0) {
            memset(dest, '0', zeroes);
        }
        memcpy(dest + zeroes, source, length);
    } else {
        memcpy(dest, source, length);
    }
    return zeroes;
}

/* Flag table for `z_is_chr()` and `z_not_sane()` */
static const unsigned short flags[256] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*00-1F*/
               IS_SPC_F,            IS_C82_F,            IS_C82_F,            IS_HSH_F, /*20-23*/ /*  !"# */
               IS_CLS_F, IS_SIL_F | IS_C82_F,            IS_C82_F,            IS_C82_F, /*24-27*/ /* $%&' */
               IS_C82_F,            IS_C82_F,            IS_AST_F,            IS_PLS_F, /*28-2B*/ /* ()*+ */
               IS_C82_F,            IS_MNS_F, IS_CLS_F | IS_C82_F, IS_CLS_F | IS_C82_F, /*2B-2F*/ /* ,-./ */
               IS_NUM_F,            IS_NUM_F,            IS_NUM_F,            IS_NUM_F, /*30-33*/ /* 0123 */
               IS_NUM_F,            IS_NUM_F,            IS_NUM_F,            IS_NUM_F, /*34-37*/ /* 4567 */
               IS_NUM_F,            IS_NUM_F, IS_CLI_F | IS_C82_F,            IS_C82_F, /*38-3B*/ /* 89:; */
               IS_C82_F,            IS_C82_F,            IS_C82_F,            IS_C82_F, /*3B-3F*/ /* <=>? */
                      0, IS_UHX_F | IS_ARS_F, IS_UHX_F | IS_ARS_F, IS_UHX_F | IS_ARS_F, /*40-43*/ /* @ABC */
    IS_UHX_F | IS_ARS_F, IS_UHX_F | IS_ARS_F, IS_UHX_F | IS_ARS_F, IS_UPO_F | IS_ARS_F, /*44-47*/ /* DEFG */
    IS_UPO_F | IS_ARS_F,            IS_UPO_F, IS_UPO_F | IS_ARS_F, IS_UPO_F | IS_ARS_F, /*48-4B*/ /* HIJK */
    IS_UPO_F | IS_ARS_F, IS_UPO_F | IS_ARS_F, IS_UPO_F | IS_ARS_F,            IS_UPO_F, /*4B-4F*/ /* LMNO */
    IS_UPO_F | IS_ARS_F,            IS_UPO_F, IS_UPO_F | IS_ARS_F, IS_UPO_F | IS_ARS_F, /*50-53*/ /* PQRS */
    IS_UPO_F | IS_ARS_F, IS_UPO_F | IS_ARS_F, IS_UPO_F | IS_ARS_F, IS_UPO_F | IS_ARS_F, /*53-57*/ /* TUVW */
    IS_UX__F | IS_ARS_F, IS_UPO_F | IS_ARS_F, IS_UPO_F | IS_ARS_F,                   0, /*58-5B*/ /* XYZ[ */
                      0,                   0,                   0,            IS_C82_F, /*5B-5F*/ /* \]^_ */
                      0,            IS_LHX_F,            IS_LHX_F,            IS_LHX_F, /*60-63*/ /* `abc */
               IS_LHX_F,            IS_LHX_F,            IS_LHX_F,            IS_LWO_F, /*64-67*/ /* defg */
               IS_LWO_F,            IS_LWO_F,            IS_LWO_F,            IS_LWO_F, /*68-6B*/ /* hijk */
               IS_LWO_F,            IS_LWO_F,            IS_LWO_F,            IS_LWO_F, /*6B-6F*/ /* lmno */
               IS_LWO_F,            IS_LWO_F,            IS_LWO_F,            IS_LWO_F, /*70-73*/ /* pqrs */
               IS_LWO_F,            IS_LWO_F,            IS_LWO_F,            IS_LWO_F, /*74-77*/ /* tuvw */
               IS_LX__F,            IS_LWO_F,            IS_LWO_F,                   0, /*78-7B*/ /* xyz{ */
                      0,                   0,                   0,                   0, /*7B-7F*/ /* |}~D */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*80-9F*/
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*A0-BF*/
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*C0-DF*/
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*E0-FF*/
};

/* Whether a character `ch` matches `flag` */
INTERNAL int z_is_chr(const unsigned int flag, const unsigned int ch) {
    return z_isascii(ch) && (flags[ch] & flag); /* As `ch` passed as an int need to check it's ASCII */
}

/* Verifies if a string only uses valid characters, returning 1-based position in `source` if not, 0 for success */
INTERNAL int z_not_sane(const unsigned int flag, const unsigned char source[], const int length) {
    int i;

    for (i = 0; i < length; i++) {
        if (!(flags[source[i]] & flag)) {
            return i + 1;
        }
    }
    return 0;
}

/* Replaces huge switch statements for looking up in tables */
/* Verifies if a string only uses valid characters as above, but also returns `test_string` position of each in
   `posns` array */
INTERNAL int z_not_sane_lookup(const char test_string[], const int test_length, const unsigned char source[],
                const int length, int *posns) {
    int i, j;

    for (i = 0; i < length; i++) {
        posns[i] = -1;
        for (j = 0; j < test_length; j++) {
            if (source[i] == test_string[j]) {
                posns[i] = j;
                break;
            }
        }
        if (posns[i] == -1) {
            return i + 1;
        }
    }

    return 0;
}

/* Returns the position of `data` in `set_string`, or -1 if not found */
INTERNAL int z_posn(const char set_string[], const char data) {
    const char *s;

    for (s = set_string; *s; s++) {
        if (data == *s) {
            return (int) (s - set_string);
        }
    }
    return -1;
}

/* Converts `arg` to a string representing its binary equivalent of length `length` and places in `binary` at
  `bin_posn`. Returns `bin_posn` + `length` */
INTERNAL int z_bin_append_posn(const int arg, const int length, char *binary, const int bin_posn) {
    int i;
    const int end = length - 1;

    for (i = 0; i < length; i++) {
        binary[bin_posn + i] = '0' + ((arg >> (end - i)) & 1);
    }
    return bin_posn + length;
}
/* Helper for `z_errtxt()` & `z_errtxtf()` to set "err_id: " part of error message, returning length */
static int errtxt_id_str(char *errtxt, int num) {
    int len = 0;

    if (num == -1) {
        errtxt[0] = '\0';
        return 0;
    }
    if (num < 0 || num > 9999) { /* Restrict to 4 digits */
        num = 9999;
    }
    if (num >= 1000) {
        errtxt[len++] = '0' + (num / 1000);
        num %= 1000;
    }
    errtxt[len++] = '0' + (num / 100);
    num %= 100;
    errtxt[len++] = '0' + (num / 10);
    num %= 10;
    errtxt[len++] = '0' + num;
    errtxt[len++] = ':';
    errtxt[len++] = ' ';

    return len;
}

/* Set `symbol->errtxt` to "err_id: msg", returning `error_number`. If `err_id` is -1, the "err_id: " prefix is
   omitted */
INTERNAL int z_errtxt(const int error_number, struct zint_symbol *symbol, const int err_id, const char *msg) {
    const int max_len = ARRAY_SIZE(symbol->errtxt) - 1;
    const int id_len = errtxt_id_str(symbol->errtxt, err_id);
    int msg_len = (int) strlen(msg);

    if (id_len + msg_len > max_len) {
        if (!(symbol->debug & ZINT_DEBUG_TEST)) assert(0); /* Catch truncations */
        msg_len = max_len - id_len;
    }
    memcpy(symbol->errtxt + id_len, msg, msg_len);

    symbol->errtxt[id_len + msg_len] = '\0';

    return error_number;
}

static int errtxtf_dpad(const char *fmt); /* Forward reference */

/* Helper for `z_errtxtf()` to parse numbered specifier "n$" (where "n" 1-9), returning `fmt` advance increment */
static int errtxtf_num_arg(const char *fmt, int *p_arg) {
    int ret = 0;
    int arg = -2;

    if (!errtxtf_dpad(fmt) && z_isdigit(fmt[0])) {
        arg = fmt[1] == '$' ? fmt[0] - '0' - 1 : -1;
        ret = 2;
    }
    if (p_arg) {
        *p_arg = arg;
    }
    return ret;
}

/* Helper for `z_errtxtf()` to parse length precision for "%s", returning `fmt` advance increment */
static int errtxtf_slen(const char *fmt, const int arg, int *p_arg_cnt, int *p_len) {
    int ret = 0;
    int len = -1;

    if (fmt[0] == '.') {
        if (z_isdigit(fmt[1]) && fmt[1] != '0') {
            len = fmt[1] - '0';
            for (ret = 2; z_isdigit(fmt[ret]); ret++) {
                len = len * 10 + fmt[ret] - '0';
            }
            if (fmt[ret] != 's') {
                len = -1;
            }
        } else if (fmt[1] == '*' && fmt[2] == 's' && arg < 0) {
            len = 0;
            ret = 2;
        } else if (fmt[1] == '*' && z_isdigit(fmt[2]) && fmt[3] == '$' && fmt[4] == 's') {
            if (arg == -1 || arg == fmt[2] - '0') {
                len = 0;
                if (p_arg_cnt) {
                    (*p_arg_cnt)++;
                }
            }
            ret = 4;
        } else {
            ret = 1;
        }
    }
    if (p_len) {
        *p_len = len;
    }

    return ret;
}

/* Helper for `z_errtxtf()` to parse zero-padded minimum field length for "%d", returning `fmt` advance increment */
static int errtxtf_dpad(const char *fmt) {
    /* Allow one leading zero plus one or two digits only */
    if (fmt[0] == '0' && z_isdigit(fmt[1])) {
        if (fmt[1] != '0' && fmt[2] == 'd') {
            return 2;
        }
        if (z_isdigit(fmt[1]) && fmt[1] != '0' && z_isdigit(fmt[2]) && fmt[3] == 'd') {
            return 3;
        }
    }
    return 0;
}

/* Helper for `z_errtxtf()` to parse conversion precision for "%f"/"%g", returning `fmt` advance increment */
static int errtxtf_fprec(const char *fmt) {
    /* Allow one digit only */
    if (fmt[0] == '.' && z_isdigit(fmt[1]) && (fmt[2] == 'f' || fmt[2] == 'g')) {
        return 2;
    }
    return 0;
}

/* Set `symbol->errtxt` to "err_id: msg" with restricted subset of `printf()` formatting, returning `error_number`.
   If `err_id` is -1, the "err_id: " prefix is omitted. Only the following specifiers are supported: "c", "d", "f",
   "g" and "s", with no modifiers apart from "<n>$" numbering for l10n ("<n>" 1-9), in which case all specifiers must
   be numbered, "%s" with length precisions: "%.*s", "%<n+1>$.*<n>$s", "%.<p>s" and "%<n>$.<p>s", "%d" with
   zero-padded minimum field lengths: "%0<m>d" or %<n>$0<m>d" ("<m>" 1-99), and "%f"/"%g" with single-digit precision:
   "%.<m>f" or "%<n>$.<m>f" */
INTERNAL int z_errtxtf(const int error_number, struct zint_symbol *symbol, const int err_id, const char *fmt, ...) {
    const int max_len = ARRAY_SIZE(symbol->errtxt) - 1;
    int p = errtxt_id_str(symbol->errtxt, err_id);
    const char *f;
    int i;
    int arg_cnt = 0;
    int have_num_arg = 0, have_unnum_arg = 0;
    va_list ap;
    int idxs[9] = {0}; /* Argument order */
    char specs[9] = {0}; /* Format specifiers */
    const char *ss[9] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL }; /* "%s" */
    int slens[9] = {0}; /* "%s" length precisions */
    int have_slens[9] = {0}; /* Bools for if "%s" has length precision */
    char dpads[9][3] = {{0}}; /* 2-digit minimum field length */
    char fprecs[9] = {0}; /* 1-digit conversion precision */
    char dfgs[9][100] = {{0}}; /* "%d", "%f" and "%g", allowing for padding up to 99 */
    int cs[9] = {0}; /* "%c" */

    /* Get argument order and specifiers */
    for (f = fmt, i = 0; *f; f++) {
        if (*f == '%') {
            int inc, arg, len;
            if (*++f == '%') {
                continue;
            }
            if ((inc = errtxtf_num_arg(f, &arg))) {
                if (arg == -1) {
                    if (!(symbol->debug & ZINT_DEBUG_TEST)) assert(0);
                    return z_errtxt(ZINT_ERROR_ENCODING_PROBLEM, symbol, 0,
                                    "Internal error: invalid numbered format specifier");
                }
                if (i >= 9) {
                    if (!(symbol->debug & ZINT_DEBUG_TEST)) assert(0);
                    return z_errtxt(ZINT_ERROR_ENCODING_PROBLEM, symbol, 0,
                                    "Internal error: too many format specifiers (9 maximum)");
                }
                f += inc;
                have_num_arg = 1;
                idxs[i] = arg;
            } else {
                if (i >= 9) {
                    if (!(symbol->debug & ZINT_DEBUG_TEST)) assert(0);
                    return z_errtxt(ZINT_ERROR_ENCODING_PROBLEM, symbol, 0,
                                    "Internal error: too many format specifiers (9 maximum)");
                }
                have_unnum_arg = 1;
                idxs[i] = i;
            }
            if ((inc = errtxtf_fprec(f))) {
                assert(inc == 2);
                fprecs[idxs[i]] = f[1]; /* TODO: keep `fprecs` separate else last mentioned trumps */
                f += inc;
            }
            if ((inc = errtxtf_slen(f, arg, &arg_cnt, &len))) {
                if (len == -1) {
                    if (!(symbol->debug & ZINT_DEBUG_TEST)) assert(0);
                    return z_errtxt(ZINT_ERROR_ENCODING_PROBLEM, symbol, 0,
                                    "Internal error: invalid length precision");
                }
                slens[idxs[i]] = len == 0 ? -1 : len; /* TODO: keep `slens` separate else last mentioned trumps */
                have_slens[idxs[i]] = 1;
                f += inc;
            }
            if ((inc = errtxtf_dpad(f))) {
                memcpy(dpads[idxs[i]], f + 1, inc - 1); /* TODO: keep `dpads` separate else last mentioned trumps */
                dpads[idxs[i]][inc - 1] = '\0';
                f += inc;
            }
            if (*f != 'c' && *f != 'd' && *f != 'f' && *f != 'g' && *f != 's') {
                if (!(symbol->debug & ZINT_DEBUG_TEST)) assert(0);
                return z_errtxt(ZINT_ERROR_ENCODING_PROBLEM, symbol, 0,
                                "Internal error: unknown format specifier ('%c','%d','%f','%g','%s' only)");
            }
            specs[idxs[i++]] = *f;
            arg_cnt++;
        }
    }
    if (have_num_arg && have_unnum_arg) {
        if (!(symbol->debug & ZINT_DEBUG_TEST)) assert(0);
        return z_errtxt(ZINT_ERROR_ENCODING_PROBLEM, symbol, 0,
                        "Internal error: mixed numbered and unnumbered format specifiers");
    }

    /* Get arguments */
    va_start(ap, fmt);
    for (i = 0; i < arg_cnt; i++) {
        if (specs[i] == 'c') {
            cs[i] = va_arg(ap, int);
        } else if (specs[i] == 'd') {
            if (dpads[i][0]) {
                char dpad_fmt[30]; /* Make 30 to suppress gcc 14 "-Wformat-overflow=" false positive */
                sprintf(dpad_fmt, "%%0%sd", dpads[i]);
                sprintf(dfgs[i], dpad_fmt, va_arg(ap, int));
            } else {
                sprintf(dfgs[i], "%d", va_arg(ap, int));
            }
        } else if (specs[i] == 'f' || specs[i] == 'g') {
            if (fprecs[i]) {
                char fprec_fmt[5];
                sprintf(fprec_fmt, "%%.%c%c", fprecs[i], specs[i]);
                sprintf(dfgs[i], fprec_fmt, va_arg(ap, double));
            } else {
                sprintf(dfgs[i], specs[i] == 'f' ? "%f" : "%g", va_arg(ap, double));
            }
        } else if (specs[i] == 's') {
            if (have_slens[i] && slens[i] == -1) {
                slens[i] = va_arg(ap, int);
            }
            ss[i] = va_arg(ap, char *);
        }
    }
    va_end(ap);

    /* Populate `errtxt` */
    for (f = fmt, i = 0; *f && p < max_len; f++) {
        if (*f == '%') {
            int idx;
            if (*++f == '%') {
                symbol->errtxt[p++] = '%';
                continue;
            }
            f += errtxtf_num_arg(f, NULL /*p_arg*/);
            f += errtxtf_slen(f, -1 /*arg*/, NULL /*arg_cnt*/, NULL /*p_len*/);
            f += errtxtf_dpad(f);
            idx = idxs[i];
            if (specs[idx] == 'c') {
                symbol->errtxt[p++] = cs[idx];
            } else {
                int len;
                if (specs[idx] == 's') {
                    if (have_slens[idx]) {
                        const char *si = ss[idx];
                        for (len = 0; len < slens[idx] && si[len]; len++);
                    } else {
                        len = (int) strlen(ss[idx]);
                    }
                } else {
                    len = (int) strlen(dfgs[idx]);
                }
                if (len) {
                    if (p + len > max_len) {
                        if (!(symbol->debug & ZINT_DEBUG_TEST)) assert(0); /* Catch truncations */
                        len = max_len - p;
                    }
                    memcpy(symbol->errtxt + p, specs[idx] == 's' ? ss[idx] : dfgs[idx], len);
                    p += len;
                }
            }
            i++;
        } else {
            symbol->errtxt[p++] = *f;
        }
    }
    if (*f) {
        if (!(symbol->debug & ZINT_DEBUG_TEST)) assert(0); /* Catch truncations */
    }

    symbol->errtxt[p] = '\0';

    return error_number;
}

/* Helper to prepend/append to existing `symbol->errtxt` by calling `z_errtxtf(fmt)` with 2 arguments (copy of
   `errtxt` & `msg`) if `msg` not NULL, or 1 argument (just copy of `errtxt`) if `msg` NULL, returning `error_number`
*/
INTERNAL int z_errtxt_adj(const int error_number, struct zint_symbol *symbol, const char *fmt, const char *msg) {
    char err_buf[ARRAY_SIZE(symbol->errtxt)];

    memcpy(err_buf, symbol->errtxt, strlen(symbol->errtxt) + 1); /* Include terminating NUL */

    if (msg) {
        z_errtxtf(0, symbol, -1, fmt, err_buf, msg);
    } else {
        z_errtxtf(0, symbol, -1, fmt, err_buf);
    }

    return error_number;
}
INTERNAL int z_extra_escapes(struct zint_symbol *symbol, const unsigned char source[], int *p_length, const int eci,
                unsigned char *dest, char *fncs, int *p_have_extra_escapes) {
    const int length = *p_length;
    int i, j = 0;

    if (eci == 20 || eci == 25 || eci >= 28) {
        return z_errtxt(ZINT_ERROR_INVALID_OPTION, symbol, 716, "Extra Escape mode requires ASCII-compatible ECI");
    }
    for (i = 0; i < length; i++) {
        if (source[i] == '\\' && i + 2 < length && source[i + 1] == '^') {
            const unsigned char ch = source[i + 2];
            if (ch == '1' || ch == '^') {
                if (ch == '^') { /* Escape sequence '\^^' */
                    dest[j++] = source[i++];
                    dest[j++] = source[i++];
                    /* Drop second '^' */
                } else { /* ch == '1' FNC1 */
                    i += 2;
                    fncs[j] = 1;
                    dest[j++] = '\x1D'; /* Manual FNC1 dummy */
                }
            } else {
                return z_errtxtf(ZINT_ERROR_INVALID_DATA, symbol, 717, "Unrecognized extra escape \"\\^%c\"",
                                !z_isascii(ch) || z_iscntrl(ch) ? '?' : ch);
            }
        } else {
            dest[j++] = source[i];
        }
    }
    if (j != length) {
        assert(j > 0 && j < length);
        dest[j] = '\0';
        *p_length = j;
        *p_have_extra_escapes = 1;
    }

    return 0;
}
/* Prevent inlining of `z_stripf()` which can optimize away its effect */
#if defined(__GNUC__) && (__GNUC__ >= 4 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 1))
#define ZINT_NOINLINE __attribute__((__noinline__))
#elif defined(_MSC_VER) && _MSC_VER >= 1310 /* MSVC 2003 (VC++ 7.1) */
#define ZINT_NOINLINE __declspec(noinline)
#else
#define ZINT_NOINLINE
#endif

/* Removes excess precision from floats - see https://stackoverflow.com/q/503436 */
INTERNAL ZINT_NOINLINE float z_stripf(const float arg) {
    return *((volatile const float *) &arg);
}

/* Returns total length of segments */
INTERNAL int z_segs_length(const struct zint_seg segs[], const int seg_count) {
    int total_len = 0;
    int i;

    for (i = 0; i < seg_count; i++) {
        total_len += segs[i].length == -1 ? (int) z_ustrlen(segs[i].source) : segs[i].length;
    }

    return total_len;
}

INTERNAL void z_ct_set_seg_eci(struct zint_symbol *symbol, const int seg_idx, const int eci) {
    (void)symbol; (void)seg_idx; (void)eci;
}
INTERNAL void z_ct_set_seg_extra_escapes_eci(struct zint_symbol *symbol, const int seg_idx, const int eci) {
    (void)symbol; (void)seg_idx; (void)eci;
}
INTERNAL void z_debug_test_codeword_dump(struct zint_symbol *symbol, const unsigned char *codewords, const int length) {
    (void)symbol; (void)codewords; (void)length;
}
INTERNAL int z_ct_printf_256(struct zint_symbol *symbol, const char *fmt, int value) {
    (void)symbol; (void)fmt; (void)value;
    return 0;
}
