/* zint_common_aztec.h - macros and declarations for upstream Aztec encoder */
#ifndef ZINT_COMMON_AZTEC_H
#define ZINT_COMMON_AZTEC_H

#include "zint.h"
#include "zintconfig.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#define ARRAY_SIZE(x) ((int) (sizeof(x) / sizeof((x)[0])))

#ifdef _MSC_VER
#  include <malloc.h>
#  define z_alloca(nmemb) _alloca(nmemb)
#else
#  if (defined(__GNUC__) && !defined(alloca) && !defined(__NetBSD__)) || defined(__NuttX__) || defined(_AIX) \
        || (defined(__sun) && defined(__SVR4))
#    include <alloca.h>
#  endif
#  define z_alloca(nmemb) alloca(nmemb)
#endif

#define INTERNAL

#define z_isdigit(ch) ((ch) <= '9' && (ch) >= '0')
#define z_isupper(ch) ((ch) >= 'A' && (ch) <= 'Z')
#define z_islower(ch) ((ch) >= 'a' && (ch) <= 'z')
#define z_isalpha(ch) (z_isupper(ch) || z_islower(ch))
#define z_isascii(ch) (!((ch) & ~0x7F))
#define z_iscntrl(ch) (!((ch) & ~0x1F) || (ch) == 127)

#define ZCUCP(p)    ((const unsigned char *) (p))
#define z_ustrlen(p) strlen((const char *) (p))

/* `z_is_chr()` & `z_not_sane()` flags (from upstream common.h) */
#define IS_SPC_F    0x0001
#define IS_HSH_F    0x0002
#define IS_AST_F    0x0004
#define IS_PLS_F    0x0008
#define IS_MNS_F    0x0010
#define IS_NUM_F    0x0020
#define IS_UPO_F    0x0040
#define IS_UHX_F    0x0080
#define IS_UX__F    0x0100
#define IS_LWO_F    0x0200
#define IS_LHX_F    0x0400
#define IS_LX__F    0x0800
#define IS_C82_F    0x1000
#define IS_SIL_F    0x2000
#define IS_CLI_F    0x4000
#define IS_ARS_F    0x8000
#define IS_UPR_F    (IS_UPO_F | IS_UHX_F | IS_UX__F)
#define IS_LWR_F    (IS_LWO_F | IS_LHX_F | IS_LX__F)
#define IS_CLS_F    (IS_CLI_F | IS_SIL_F)
#define NEON_F      IS_NUM_F

#if defined(__GNUC__) && !defined(__clang__)
#define ZEXT __extension__
#else
#define ZEXT
#endif

#define Z_COMMON_INLINE 1

#ifdef Z_COMMON_INLINE
#  define z_module_is_set(s, y, x) (((s)->encoded_data[y][(x) >> 3] >> ((x) & 0x07)) & 1)
#  define z_set_module(s, y, x) do { (s)->encoded_data[y][(x) >> 3] |= 1 << ((x) & 0x07); } while (0)
#  define z_unset_module(s, y, x) do { (s)->encoded_data[y][(x) >> 3] &= ~(1 << ((x) & 0x07)); } while (0)
#endif

#if defined(__GNUC__) && (__GNUC__ >= 4 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 1))
#  define ZINT_NOINLINE __attribute__((__noinline__))
#elif defined(_MSC_VER) && _MSC_VER >= 1310
#  define ZINT_NOINLINE __declspec(noinline)
#else
#  define ZINT_NOINLINE
#endif

INTERNAL int z_to_int(const unsigned char source[], const int length);
INTERNAL int z_chr_cnt(const unsigned char source[], const int length, const unsigned char ch);
INTERNAL int z_extra_escape_position_fnc1(const unsigned char source[], const int length);
INTERNAL int z_is_chr(const unsigned int flag, const unsigned int ch);
INTERNAL int z_not_sane(const unsigned int flag, const unsigned char source[], const int length);
INTERNAL int z_bin_append_posn(const int arg, const int length, char *binary, const int bin_posn);
INTERNAL int z_errtxt(const int error_number, struct zint_symbol *symbol, const int err_id, const char *msg);
INTERNAL int z_errtxtf(const int error_number, struct zint_symbol *symbol, const int err_id, const char *fmt, ...);
INTERNAL int z_errtxt_adj(const int error_number, struct zint_symbol *symbol, const char *fmt, const char *msg);
INTERNAL int z_extra_escapes(struct zint_symbol *symbol, const unsigned char source[], int *p_length, const int eci,
                unsigned char *dest, char *fncs, int *p_have_extra_escapes);
INTERNAL float z_stripf(const float arg);
INTERNAL int z_segs_length(const struct zint_seg segs[], const int seg_count);
INTERNAL void z_ct_set_seg_eci(struct zint_symbol *symbol, const int seg_idx, const int eci);
INTERNAL void z_ct_set_seg_extra_escapes_eci(struct zint_symbol *symbol, const int seg_idx, const int eci);
INTERNAL void z_debug_test_codeword_dump(struct zint_symbol *symbol, const unsigned char *codewords, const int length);
INTERNAL int z_ct_printf_256(struct zint_symbol *symbol, const char *fmt, int value);

#endif /* ZINT_COMMON_AZTEC_H */
