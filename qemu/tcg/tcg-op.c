/*
 * Tiny Code Generator for QEMU
 *
 * Copyright (c) 2008 Fabrice Bellard
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "qemu/osdep.h"
#include "qemu-common.h"
#include "cpu.h"
#include "exec/exec-all.h"
#include "tcg.h"
#include "tcg-op.h"

/* Reduce the number of ifdefs below.  This assumes that all uses of
   TCGV_HIGH and TCGV_LOW are properly protected by a conditional that
   the compiler can eliminate.  */
#if TCG_TARGET_REG_BITS == 64
extern TCGv_i32 TCGV_LOW_link_error(TCGv_i64);
extern TCGv_i32 TCGV_HIGH_link_error(TCGv_i64);
#define TCGV_LOW  TCGV_LOW_link_error
#define TCGV_HIGH TCGV_HIGH_link_error
#endif

/* Note that this is optimized for sequential allocation during translate.
   Up to and including filling in the forward link immediately.  We'll do
   proper termination of the end of the list after we finish translation.  */

static void tcg_emit_op(TCGContext *ctx, TCGOpcode opc, int args)
{
    int oi = ctx->gen_next_op_idx;
    int ni = oi + 1;
    int pi = oi - 1;
    TCGOp op = {0};

    tcg_debug_assert(oi < OPC_BUF_SIZE);
    ctx->gen_op_buf[0].prev = oi;
    ctx->gen_next_op_idx = ni;

    op.opc = opc;
    op.args = args;
    op.prev = pi;
    op.next = ni;

    ctx->gen_op_buf[oi] = op;
}

void tcg_gen_op1(TCGContext *ctx, TCGOpcode opc, TCGArg a1)
{
    int pi = ctx->gen_next_parm_idx;

    tcg_debug_assert(pi + 1 <= OPPARAM_BUF_SIZE);
    ctx->gen_next_parm_idx = pi + 1;
    ctx->gen_opparam_buf[pi] = a1;

    tcg_emit_op(ctx, opc, pi);
}

void tcg_gen_op2(TCGContext *ctx, TCGOpcode opc, TCGArg a1, TCGArg a2)
{
    int pi = ctx->gen_next_parm_idx;

    tcg_debug_assert(pi + 2 <= OPPARAM_BUF_SIZE);
    ctx->gen_next_parm_idx = pi + 2;
    ctx->gen_opparam_buf[pi + 0] = a1;
    ctx->gen_opparam_buf[pi + 1] = a2;

    tcg_emit_op(ctx, opc, pi);
}

void tcg_gen_op3(TCGContext *ctx, TCGOpcode opc, TCGArg a1,
                 TCGArg a2, TCGArg a3)
{
    int pi = ctx->gen_next_parm_idx;

    tcg_debug_assert(pi + 3 <= OPPARAM_BUF_SIZE);
    ctx->gen_next_parm_idx = pi + 3;
    ctx->gen_opparam_buf[pi + 0] = a1;
    ctx->gen_opparam_buf[pi + 1] = a2;
    ctx->gen_opparam_buf[pi + 2] = a3;

    tcg_emit_op(ctx, opc, pi);
}

void tcg_gen_op4(TCGContext *ctx, TCGOpcode opc, TCGArg a1,
                 TCGArg a2, TCGArg a3, TCGArg a4)
{
    int pi = ctx->gen_next_parm_idx;

    tcg_debug_assert(pi + 4 <= OPPARAM_BUF_SIZE);
    ctx->gen_next_parm_idx = pi + 4;
    ctx->gen_opparam_buf[pi + 0] = a1;
    ctx->gen_opparam_buf[pi + 1] = a2;
    ctx->gen_opparam_buf[pi + 2] = a3;
    ctx->gen_opparam_buf[pi + 3] = a4;

    tcg_emit_op(ctx, opc, pi);
}

void tcg_gen_op5(TCGContext *ctx, TCGOpcode opc, TCGArg a1,
                 TCGArg a2, TCGArg a3, TCGArg a4, TCGArg a5)
{
    int pi = ctx->gen_next_parm_idx;

    tcg_debug_assert(pi + 5 <= OPPARAM_BUF_SIZE);
    ctx->gen_next_parm_idx = pi + 5;
    ctx->gen_opparam_buf[pi + 0] = a1;
    ctx->gen_opparam_buf[pi + 1] = a2;
    ctx->gen_opparam_buf[pi + 2] = a3;
    ctx->gen_opparam_buf[pi + 3] = a4;
    ctx->gen_opparam_buf[pi + 4] = a5;

    tcg_emit_op(ctx, opc, pi);
}

void tcg_gen_op6(TCGContext *ctx, TCGOpcode opc, TCGArg a1, TCGArg a2,
                 TCGArg a3, TCGArg a4, TCGArg a5, TCGArg a6)
{
    int pi = ctx->gen_next_parm_idx;

    tcg_debug_assert(pi + 6 <= OPPARAM_BUF_SIZE);
    ctx->gen_next_parm_idx = pi + 6;
    ctx->gen_opparam_buf[pi + 0] = a1;
    ctx->gen_opparam_buf[pi + 1] = a2;
    ctx->gen_opparam_buf[pi + 2] = a3;
    ctx->gen_opparam_buf[pi + 3] = a4;
    ctx->gen_opparam_buf[pi + 4] = a5;
    ctx->gen_opparam_buf[pi + 5] = a6;

    tcg_emit_op(ctx, opc, pi);
}

void tcg_gen_mb(TCGContext *ctx, TCGBar mb_type)
{
    if (ctx->uc->parallel_cpus) {
        tcg_gen_op1(ctx, INDEX_op_mb, mb_type);
    }
}

/* 32 bit ops */

void tcg_gen_addi_i32(TCGContext *s, TCGv_i32 ret, TCGv_i32 arg1, int32_t arg2)
{
    /* some cases can be optimized here */
    if (arg2 == 0) {
        tcg_gen_mov_i32(s, ret, arg1);
    } else {
        TCGv_i32 t0 = tcg_const_i32(s, arg2);
        tcg_gen_add_i32(s, ret, arg1, t0);
        tcg_temp_free_i32(s, t0);
    }
}

void tcg_gen_subfi_i32(TCGContext *s, TCGv_i32 ret, int32_t arg1, TCGv_i32 arg2)
{
    if (arg1 == 0 && TCG_TARGET_HAS_neg_i32) {
        /* Don't recurse with tcg_gen_neg_i32.  */
        tcg_gen_op2_i32(s, INDEX_op_neg_i32, ret, arg2);
    } else {
        TCGv_i32 t0 = tcg_const_i32(s, arg1);
        tcg_gen_sub_i32(s, ret, t0, arg2);
        tcg_temp_free_i32(s, t0);
    }
}

void tcg_gen_subi_i32(TCGContext *s, TCGv_i32 ret, TCGv_i32 arg1, int32_t arg2)
{
    /* some cases can be optimized here */
    if (arg2 == 0) {
        tcg_gen_mov_i32(s, ret, arg1);
    } else {
        TCGv_i32 t0 = tcg_const_i32(s, arg2);
        tcg_gen_sub_i32(s, ret, arg1, t0);
        tcg_temp_free_i32(s, t0);
    }
}

void tcg_gen_andi_i32(TCGContext *s, TCGv_i32 ret, TCGv_i32 arg1, uint32_t arg2)
{
    TCGv_i32 t0;
    /* Some cases can be optimized here.  */
    switch (arg2) {
    case 0:
        tcg_gen_movi_i32(s, ret, 0);
        return;
    case 0xffffffffu:
        tcg_gen_mov_i32(s, ret, arg1);
        return;
    case 0xffu:
        /* Don't recurse with tcg_gen_ext8u_i32.  */
        if (TCG_TARGET_HAS_ext8u_i32) {
            tcg_gen_op2_i32(s, INDEX_op_ext8u_i32, ret, arg1);
            return;
        }
        break;
    case 0xffffu:
        if (TCG_TARGET_HAS_ext16u_i32) {
            tcg_gen_op2_i32(s, INDEX_op_ext16u_i32, ret, arg1);
            return;
        }
        break;
    }
    t0 = tcg_const_i32(s, arg2);
    tcg_gen_and_i32(s, ret, arg1, t0);
    tcg_temp_free_i32(s, t0);
}

void tcg_gen_ori_i32(TCGContext *s, TCGv_i32 ret, TCGv_i32 arg1, int32_t arg2)
{
    /* Some cases can be optimized here.  */
    if (arg2 == -1) {
        tcg_gen_movi_i32(s, ret, -1);
    } else if (arg2 == 0) {
        tcg_gen_mov_i32(s, ret, arg1);
    } else {
        TCGv_i32 t0 = tcg_const_i32(s, arg2);
        tcg_gen_or_i32(s, ret, arg1, t0);
        tcg_temp_free_i32(s, t0);
    }
}

void tcg_gen_xori_i32(TCGContext *s, TCGv_i32 ret, TCGv_i32 arg1, int32_t arg2)
{
    /* Some cases can be optimized here.  */
    if (arg2 == 0) {
        tcg_gen_mov_i32(s, ret, arg1);
    } else if (arg2 == -1 && TCG_TARGET_HAS_not_i32) {
        /* Don't recurse with tcg_gen_not_i32.  */
        tcg_gen_op2_i32(s, INDEX_op_not_i32, ret, arg1);
    } else {
        TCGv_i32 t0 = tcg_const_i32(s, arg2);
        tcg_gen_xor_i32(s, ret, arg1, t0);
        tcg_temp_free_i32(s, t0);
    }
}

void tcg_gen_shli_i32(TCGContext *s, TCGv_i32 ret, TCGv_i32 arg1, unsigned arg2)
{
    tcg_debug_assert(arg2 < 32);
    if (arg2 == 0) {
        tcg_gen_mov_i32(s, ret, arg1);
    } else {
        TCGv_i32 t0 = tcg_const_i32(s, arg2);
        tcg_gen_shl_i32(s, ret, arg1, t0);
        tcg_temp_free_i32(s, t0);
    }
}

void tcg_gen_shri_i32(TCGContext *s, TCGv_i32 ret, TCGv_i32 arg1, unsigned arg2)
{
    tcg_debug_assert(arg2 < 32);
    if (arg2 == 0) {
        tcg_gen_mov_i32(s, ret, arg1);
    } else {
        TCGv_i32 t0 = tcg_const_i32(s, arg2);
        tcg_gen_shr_i32(s, ret, arg1, t0);
        tcg_temp_free_i32(s, t0);
    }
}

void tcg_gen_sari_i32(TCGContext *s, TCGv_i32 ret, TCGv_i32 arg1, unsigned arg2)
{
    tcg_debug_assert(arg2 < 32);
    if (arg2 == 0) {
        tcg_gen_mov_i32(s, ret, arg1);
    } else {
        TCGv_i32 t0 = tcg_const_i32(s, arg2);
        tcg_gen_sar_i32(s, ret, arg1, t0);
        tcg_temp_free_i32(s, t0);
    }
}

void tcg_gen_brcond_i32(TCGContext *s, TCGCond cond, TCGv_i32 arg1, TCGv_i32 arg2, TCGLabel *l)
{
    if (cond == TCG_COND_ALWAYS) {
        tcg_gen_br(s, l);
    } else if (cond != TCG_COND_NEVER) {
        tcg_gen_op4ii_i32(s, INDEX_op_brcond_i32, arg1, arg2, cond, label_arg(s, l));
    }
}

void tcg_gen_brcondi_i32(TCGContext *s, TCGCond cond, TCGv_i32 arg1, int32_t arg2, TCGLabel *l)
{
    if (cond == TCG_COND_ALWAYS) {
        tcg_gen_br(s, l);
    } else if (cond != TCG_COND_NEVER) {
        TCGv_i32 t0 = tcg_const_i32(s, arg2);
        tcg_gen_brcond_i32(s, cond, arg1, t0, l);
        tcg_temp_free_i32(s, t0);
    }
}

void tcg_gen_setcond_i32(TCGContext *s, TCGCond cond, TCGv_i32 ret,
                         TCGv_i32 arg1, TCGv_i32 arg2)
{
    if (cond == TCG_COND_ALWAYS) {
        tcg_gen_movi_i32(s, ret, 1);
    } else if (cond == TCG_COND_NEVER) {
        tcg_gen_movi_i32(s, ret, 0);
    } else {
        tcg_gen_op4i_i32(s, INDEX_op_setcond_i32, ret, arg1, arg2, cond);
    }
}

void tcg_gen_setcondi_i32(TCGContext *s, TCGCond cond, TCGv_i32 ret,
                          TCGv_i32 arg1, int32_t arg2)
{
    TCGv_i32 t0 = tcg_const_i32(s, arg2);
    tcg_gen_setcond_i32(s, cond, ret, arg1, t0);
    tcg_temp_free_i32(s, t0);
}

void tcg_gen_muli_i32(TCGContext *s, TCGv_i32 ret, TCGv_i32 arg1, int32_t arg2)
{
    TCGv_i32 t0 = tcg_const_i32(s, arg2);
    tcg_gen_mul_i32(s, ret, arg1, t0);
    tcg_temp_free_i32(s, t0);
}

void tcg_gen_div_i32(TCGContext *s, TCGv_i32 ret, TCGv_i32 arg1, TCGv_i32 arg2)
{
    if (TCG_TARGET_HAS_div_i32) {
        tcg_gen_op3_i32(s, INDEX_op_div_i32, ret, arg1, arg2);
    } else if (TCG_TARGET_HAS_div2_i32) {
        TCGv_i32 t0 = tcg_temp_new_i32(s);
        tcg_gen_sari_i32(s, t0, arg1, 31);
        tcg_gen_op5_i32(s, INDEX_op_div2_i32, ret, t0, arg1, t0, arg2);
        tcg_temp_free_i32(s, t0);
    } else {
        gen_helper_div_i32(s, ret, arg1, arg2);
    }
}

void tcg_gen_rem_i32(TCGContext *s, TCGv_i32 ret, TCGv_i32 arg1, TCGv_i32 arg2)
{
    if (TCG_TARGET_HAS_rem_i32) {
        tcg_gen_op3_i32(s, INDEX_op_rem_i32, ret, arg1, arg2);
    } else if (TCG_TARGET_HAS_div_i32) {
        TCGv_i32 t0 = tcg_temp_new_i32(s);
        tcg_gen_op3_i32(s, INDEX_op_div_i32, t0, arg1, arg2);
        tcg_gen_mul_i32(s, t0, t0, arg2);
        tcg_gen_sub_i32(s, ret, arg1, t0);
        tcg_temp_free_i32(s, t0);
    } else if (TCG_TARGET_HAS_div2_i32) {
        TCGv_i32 t0 = tcg_temp_new_i32(s);
        tcg_gen_sari_i32(s, t0, arg1, 31);
        tcg_gen_op5_i32(s, INDEX_op_div2_i32, t0, ret, arg1, t0, arg2);
        tcg_temp_free_i32(s, t0);
    } else {
        gen_helper_rem_i32(s, ret, arg1, arg2);
    }
}

void tcg_gen_divu_i32(TCGContext *s, TCGv_i32 ret, TCGv_i32 arg1, TCGv_i32 arg2)
{
    if (TCG_TARGET_HAS_div_i32) {
        tcg_gen_op3_i32(s, INDEX_op_divu_i32, ret, arg1, arg2);
    } else if (TCG_TARGET_HAS_div2_i32) {
        TCGv_i32 t0 = tcg_temp_new_i32(s);
        tcg_gen_movi_i32(s, t0, 0);
        tcg_gen_op5_i32(s, INDEX_op_divu2_i32, ret, t0, arg1, t0, arg2);
        tcg_temp_free_i32(s, t0);
    } else {
        gen_helper_divu_i32(s, ret, arg1, arg2);
    }
}

void tcg_gen_remu_i32(TCGContext *s, TCGv_i32 ret, TCGv_i32 arg1, TCGv_i32 arg2)
{
    if (TCG_TARGET_HAS_rem_i32) {
        tcg_gen_op3_i32(s, INDEX_op_remu_i32, ret, arg1, arg2);
    } else if (TCG_TARGET_HAS_div_i32) {
        TCGv_i32 t0 = tcg_temp_new_i32(s);
        tcg_gen_op3_i32(s, INDEX_op_divu_i32, t0, arg1, arg2);
        tcg_gen_mul_i32(s, t0, t0, arg2);
        tcg_gen_sub_i32(s, ret, arg1, t0);
        tcg_temp_free_i32(s, t0);
    } else if (TCG_TARGET_HAS_div2_i32) {
        TCGv_i32 t0 = tcg_temp_new_i32(s);
        tcg_gen_movi_i32(s, t0, 0);
        tcg_gen_op5_i32(s, INDEX_op_divu2_i32, t0, ret, arg1, t0, arg2);
        tcg_temp_free_i32(s, t0);
    } else {
        gen_helper_remu_i32(s, ret, arg1, arg2);
    }
}

void tcg_gen_andc_i32(TCGContext *s, TCGv_i32 ret, TCGv_i32 arg1, TCGv_i32 arg2)
{
    if (TCG_TARGET_HAS_andc_i32) {
        tcg_gen_op3_i32(s, INDEX_op_andc_i32, ret, arg1, arg2);
    } else {
        TCGv_i32 t0 = tcg_temp_new_i32(s);
        tcg_gen_not_i32(s, t0, arg2);
        tcg_gen_and_i32(s, ret, arg1, t0);
        tcg_temp_free_i32(s, t0);
    }
}

void tcg_gen_eqv_i32(TCGContext *s, TCGv_i32 ret, TCGv_i32 arg1, TCGv_i32 arg2)
{
    if (TCG_TARGET_HAS_eqv_i32) {
        tcg_gen_op3_i32(s, INDEX_op_eqv_i32, ret, arg1, arg2);
    } else {
        tcg_gen_xor_i32(s, ret, arg1, arg2);
        tcg_gen_not_i32(s, ret, ret);
    }
}

void tcg_gen_nand_i32(TCGContext *s, TCGv_i32 ret, TCGv_i32 arg1, TCGv_i32 arg2)
{
    if (TCG_TARGET_HAS_nand_i32) {
        tcg_gen_op3_i32(s, INDEX_op_nand_i32, ret, arg1, arg2);
    } else {
        tcg_gen_and_i32(s, ret, arg1, arg2);
        tcg_gen_not_i32(s, ret, ret);
    }
}

void tcg_gen_nor_i32(TCGContext *s, TCGv_i32 ret, TCGv_i32 arg1, TCGv_i32 arg2)
{
    if (TCG_TARGET_HAS_nor_i32) {
        tcg_gen_op3_i32(s, INDEX_op_nor_i32, ret, arg1, arg2);
    } else {
        tcg_gen_or_i32(s, ret, arg1, arg2);
        tcg_gen_not_i32(s, ret, ret);
    }
}

void tcg_gen_orc_i32(TCGContext *s, TCGv_i32 ret, TCGv_i32 arg1, TCGv_i32 arg2)
{
    if (TCG_TARGET_HAS_orc_i32) {
        tcg_gen_op3_i32(s, INDEX_op_orc_i32, ret, arg1, arg2);
    } else {
        TCGv_i32 t0 = tcg_temp_new_i32(s);
        tcg_gen_not_i32(s, t0, arg2);
        tcg_gen_or_i32(s, ret, arg1, t0);
        tcg_temp_free_i32(s, t0);
    }
}

void tcg_gen_rotl_i32(TCGContext *s, TCGv_i32 ret, TCGv_i32 arg1, TCGv_i32 arg2)
{
    if (TCG_TARGET_HAS_rot_i32) {
        tcg_gen_op3_i32(s, INDEX_op_rotl_i32, ret, arg1, arg2);
    } else {
        TCGv_i32 t0, t1;

        t0 = tcg_temp_new_i32(s);
        t1 = tcg_temp_new_i32(s);
        tcg_gen_shl_i32(s, t0, arg1, arg2);
        tcg_gen_subfi_i32(s, t1, 32, arg2);
        tcg_gen_shr_i32(s, t1, arg1, t1);
        tcg_gen_or_i32(s, ret, t0, t1);
        tcg_temp_free_i32(s, t0);
        tcg_temp_free_i32(s, t1);
    }
}

void tcg_gen_rotli_i32(TCGContext *s, TCGv_i32 ret, TCGv_i32 arg1, unsigned arg2)
{
    tcg_debug_assert(arg2 < 32);
    /* some cases can be optimized here */
    if (arg2 == 0) {
        tcg_gen_mov_i32(s, ret, arg1);
    } else if (TCG_TARGET_HAS_rot_i32) {
        TCGv_i32 t0 = tcg_const_i32(s, arg2);
        tcg_gen_rotl_i32(s, ret, arg1, t0);
        tcg_temp_free_i32(s, t0);
    } else {
        TCGv_i32 t0, t1;
        t0 = tcg_temp_new_i32(s);
        t1 = tcg_temp_new_i32(s);
        tcg_gen_shli_i32(s, t0, arg1, arg2);
        tcg_gen_shri_i32(s, t1, arg1, 32 - arg2);
        tcg_gen_or_i32(s, ret, t0, t1);
        tcg_temp_free_i32(s, t0);
        tcg_temp_free_i32(s, t1);
    }
}

void tcg_gen_rotr_i32(TCGContext *s, TCGv_i32 ret, TCGv_i32 arg1, TCGv_i32 arg2)
{
    if (TCG_TARGET_HAS_rot_i32) {
        tcg_gen_op3_i32(s, INDEX_op_rotr_i32, ret, arg1, arg2);
    } else {
        TCGv_i32 t0, t1;

        t0 = tcg_temp_new_i32(s);
        t1 = tcg_temp_new_i32(s);
        tcg_gen_shr_i32(s, t0, arg1, arg2);
        tcg_gen_subfi_i32(s, t1, 32, arg2);
        tcg_gen_shl_i32(s, t1, arg1, t1);
        tcg_gen_or_i32(s, ret, t0, t1);
        tcg_temp_free_i32(s, t0);
        tcg_temp_free_i32(s, t1);
    }
}

void tcg_gen_rotri_i32(TCGContext *s, TCGv_i32 ret, TCGv_i32 arg1, unsigned arg2)
{
    tcg_debug_assert(arg2 < 32);
    /* some cases can be optimized here */
    if (arg2 == 0) {
        tcg_gen_mov_i32(s, ret, arg1);
    } else {
        tcg_gen_rotli_i32(s, ret, arg1, 32 - arg2);
    }
}

void tcg_gen_deposit_i32(TCGContext *s, TCGv_i32 ret, TCGv_i32 arg1, TCGv_i32 arg2,
                         unsigned int ofs, unsigned int len)
{
    uint32_t mask;
    TCGv_i32 t1;

    tcg_debug_assert(ofs < 32);
    tcg_debug_assert(len <= 32);
    tcg_debug_assert(ofs + len <= 32);

    if (ofs == 0 && len == 32) {
        tcg_gen_mov_i32(s, ret, arg2);
        return;
    }
    if (TCG_TARGET_HAS_deposit_i32 && TCG_TARGET_deposit_i32_valid(ofs, len)) {
        tcg_gen_op5ii_i32(s, INDEX_op_deposit_i32, ret, arg1, arg2, ofs, len);
        return;
    }

    mask = (1u << len) - 1;
    t1 = tcg_temp_new_i32(s);

    if (ofs + len < 32) {
        tcg_gen_andi_i32(s, t1, arg2, mask);
        tcg_gen_shli_i32(s, t1, t1, ofs);
    } else {
        tcg_gen_shli_i32(s, t1, arg2, ofs);
    }
    tcg_gen_andi_i32(s, ret, arg1, ~(mask << ofs));
    tcg_gen_or_i32(s, ret, ret, t1);

    tcg_temp_free_i32(s, t1);
}

void tcg_gen_extract_i32(TCGContext *s, TCGv_i32 ret, TCGv_i32 arg,
                         unsigned int ofs, unsigned int len)
{
    tcg_debug_assert(ofs < 32);
    tcg_debug_assert(len > 0);
    tcg_debug_assert(len <= 32);
    tcg_debug_assert(ofs + len <= 32);

    /* Canonicalize certain special cases, even if extract is supported.  */
    if (ofs + len == 32) {
        tcg_gen_shri_i32(s, ret, arg, 32 - len);
        return;
    }
    if (ofs == 0) {
        tcg_gen_andi_i32(s, ret, arg, (1u << len) - 1);
        return;
    }

    if (TCG_TARGET_HAS_extract_i32
        && TCG_TARGET_extract_i32_valid(ofs, len)) {
        tcg_gen_op4ii_i32(s, INDEX_op_extract_i32, ret, arg, ofs, len);
        return;
    }

    /* Assume that zero-extension, if available, is cheaper than a shift.  */
    switch (ofs + len) {
    case 16:
        if (TCG_TARGET_HAS_ext16u_i32) {
            tcg_gen_ext16u_i32(s, ret, arg);
            tcg_gen_shri_i32(s, ret, ret, ofs);
            return;
        }
        break;
    case 8:
        if (TCG_TARGET_HAS_ext8u_i32) {
            tcg_gen_ext8u_i32(s, ret, arg);
            tcg_gen_shri_i32(s, ret, ret, ofs);
            return;
        }
        break;
    }

    /* ??? Ideally we'd know what values are available for immediate AND.
       Assume that 8 bits are available, plus the special case of 16,
       so that we get ext8u, ext16u.  */
    switch (len) {
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8: case 16:
        tcg_gen_shri_i32(s, ret, arg, ofs);
        tcg_gen_andi_i32(s, ret, ret, (1u << len) - 1);
        break;
    default:
        tcg_gen_shli_i32(s, ret, arg, 32 - len - ofs);
        tcg_gen_shri_i32(s, ret, ret, 32 - len);
        break;
    }
}

void tcg_gen_sextract_i32(TCGContext *s, TCGv_i32 ret, TCGv_i32 arg,
                          unsigned int ofs, unsigned int len)
{
    tcg_debug_assert(ofs < 32);
    tcg_debug_assert(len > 0);
    tcg_debug_assert(len <= 32);
    tcg_debug_assert(ofs + len <= 32);

    /* Canonicalize certain special cases, even if extract is supported.  */
    if (ofs + len == 32) {
        tcg_gen_sari_i32(s, ret, arg, 32 - len);
        return;
    }
    if (ofs == 0) {
        switch (len) {
        case 16:
            tcg_gen_ext16s_i32(s, ret, arg);
            return;
        case 8:
            tcg_gen_ext8s_i32(s, ret, arg);
            return;
        }
    }

    if (TCG_TARGET_HAS_sextract_i32
        && TCG_TARGET_extract_i32_valid(ofs, len)) {
        tcg_gen_op4ii_i32(s, INDEX_op_sextract_i32, ret, arg, ofs, len);
        return;
    }

    /* Assume that sign-extension, if available, is cheaper than a shift.  */
    switch (ofs + len) {
    case 16:
        if (TCG_TARGET_HAS_ext16s_i32) {
            tcg_gen_ext16s_i32(s, ret, arg);
            tcg_gen_sari_i32(s, ret, ret, ofs);
            return;
        }
        break;
    case 8:
        if (TCG_TARGET_HAS_ext8s_i32) {
            tcg_gen_ext8s_i32(s, ret, arg);
            tcg_gen_sari_i32(s, ret, ret, ofs);
            return;
        }
        break;
    }
    switch (len) {
    case 16:
        if (TCG_TARGET_HAS_ext16s_i32) {
            tcg_gen_shri_i32(s, ret, arg, ofs);
            tcg_gen_ext16s_i32(s, ret, ret);
            return;
        }
        break;
    case 8:
        if (TCG_TARGET_HAS_ext8s_i32) {
            tcg_gen_shri_i32(s, ret, arg, ofs);
            tcg_gen_ext8s_i32(s, ret, ret);
            return;
        }
        break;
    }

    tcg_gen_shli_i32(s, ret, arg, 32 - len - ofs);
    tcg_gen_sari_i32(s, ret, ret, 32 - len);
}

void tcg_gen_movcond_i32(TCGContext *s, TCGCond cond, TCGv_i32 ret, TCGv_i32 c1,
                         TCGv_i32 c2, TCGv_i32 v1, TCGv_i32 v2)
{
    if (cond == TCG_COND_ALWAYS) {
        tcg_gen_mov_i32(s, ret, v1);
    } else if (cond == TCG_COND_NEVER) {
        tcg_gen_mov_i32(s, ret, v2);
    } else if (TCG_TARGET_HAS_movcond_i32) {
        tcg_gen_op6i_i32(s, INDEX_op_movcond_i32, ret, c1, c2, v1, v2, cond);
    } else {
        TCGv_i32 t0 = tcg_temp_new_i32(s);
        TCGv_i32 t1 = tcg_temp_new_i32(s);
        tcg_gen_setcond_i32(s, cond, t0, c1, c2);
        tcg_gen_neg_i32(s, t0, t0);
        tcg_gen_and_i32(s, t1, v1, t0);
        tcg_gen_andc_i32(s, ret, v2, t0);
        tcg_gen_or_i32(s, ret, ret, t1);
        tcg_temp_free_i32(s, t0);
        tcg_temp_free_i32(s, t1);
    }
}

void tcg_gen_add2_i32(TCGContext *s, TCGv_i32 rl, TCGv_i32 rh, TCGv_i32 al,
                      TCGv_i32 ah, TCGv_i32 bl, TCGv_i32 bh)
{
    if (TCG_TARGET_HAS_add2_i32) {
        tcg_gen_op6_i32(s, INDEX_op_add2_i32, rl, rh, al, ah, bl, bh);
    } else {
        TCGv_i64 t0 = tcg_temp_new_i64(s);
        TCGv_i64 t1 = tcg_temp_new_i64(s);
        tcg_gen_concat_i32_i64(s, t0, al, ah);
        tcg_gen_concat_i32_i64(s, t1, bl, bh);
        tcg_gen_add_i64(s, t0, t0, t1);
        tcg_gen_extr_i64_i32(s, rl, rh, t0);
        tcg_temp_free_i64(s, t0);
        tcg_temp_free_i64(s, t1);
    }
}

void tcg_gen_sub2_i32(TCGContext *s, TCGv_i32 rl, TCGv_i32 rh, TCGv_i32 al,
                      TCGv_i32 ah, TCGv_i32 bl, TCGv_i32 bh)
{
    if (TCG_TARGET_HAS_sub2_i32) {
        tcg_gen_op6_i32(s, INDEX_op_sub2_i32, rl, rh, al, ah, bl, bh);
    } else {
        TCGv_i64 t0 = tcg_temp_new_i64(s);
        TCGv_i64 t1 = tcg_temp_new_i64(s);
        tcg_gen_concat_i32_i64(s, t0, al, ah);
        tcg_gen_concat_i32_i64(s, t1, bl, bh);
        tcg_gen_sub_i64(s, t0, t0, t1);
        tcg_gen_extr_i64_i32(s, rl, rh, t0);
        tcg_temp_free_i64(s, t0);
        tcg_temp_free_i64(s, t1);
    }
}

void tcg_gen_mulu2_i32(TCGContext *s, TCGv_i32 rl, TCGv_i32 rh, TCGv_i32 arg1, TCGv_i32 arg2)
{
    if (TCG_TARGET_HAS_mulu2_i32) {
        tcg_gen_op4_i32(s, INDEX_op_mulu2_i32, rl, rh, arg1, arg2);
    } else if (TCG_TARGET_HAS_muluh_i32) {
        TCGv_i32 t = tcg_temp_new_i32(s);
        tcg_gen_op3_i32(s, INDEX_op_mul_i32, t, arg1, arg2);
        tcg_gen_op3_i32(s, INDEX_op_muluh_i32, rh, arg1, arg2);
        tcg_gen_mov_i32(s, rl, t);
        tcg_temp_free_i32(s, t);
    } else {
        TCGv_i64 t0 = tcg_temp_new_i64(s);
        TCGv_i64 t1 = tcg_temp_new_i64(s);
        tcg_gen_extu_i32_i64(s, t0, arg1);
        tcg_gen_extu_i32_i64(s, t1, arg2);
        tcg_gen_mul_i64(s, t0, t0, t1);
        tcg_gen_extr_i64_i32(s, rl, rh, t0);
        tcg_temp_free_i64(s, t0);
        tcg_temp_free_i64(s, t1);
    }
}

void tcg_gen_muls2_i32(TCGContext *s, TCGv_i32 rl, TCGv_i32 rh, TCGv_i32 arg1, TCGv_i32 arg2)
{
    if (TCG_TARGET_HAS_muls2_i32) {
        tcg_gen_op4_i32(s, INDEX_op_muls2_i32, rl, rh, arg1, arg2);
    } else if (TCG_TARGET_HAS_mulsh_i32) {
        TCGv_i32 t = tcg_temp_new_i32(s);
        tcg_gen_op3_i32(s, INDEX_op_mul_i32, t, arg1, arg2);
        tcg_gen_op3_i32(s, INDEX_op_mulsh_i32, rh, arg1, arg2);
        tcg_gen_mov_i32(s, rl, t);
        tcg_temp_free_i32(s, t);
    } else if (TCG_TARGET_REG_BITS == 32) {
        TCGv_i32 t0 = tcg_temp_new_i32(s);
        TCGv_i32 t1 = tcg_temp_new_i32(s);
        TCGv_i32 t2 = tcg_temp_new_i32(s);
        TCGv_i32 t3 = tcg_temp_new_i32(s);
        tcg_gen_mulu2_i32(s, t0, t1, arg1, arg2);
        /* Adjust for negative inputs.  */
        tcg_gen_sari_i32(s, t2, arg1, 31);
        tcg_gen_sari_i32(s, t3, arg2, 31);
        tcg_gen_and_i32(s, t2, t2, arg2);
        tcg_gen_and_i32(s, t3, t3, arg1);
        tcg_gen_sub_i32(s, rh, t1, t2);
        tcg_gen_sub_i32(s, rh, rh, t3);
        tcg_gen_mov_i32(s, rl, t0);
        tcg_temp_free_i32(s, t0);
        tcg_temp_free_i32(s, t1);
        tcg_temp_free_i32(s, t2);
        tcg_temp_free_i32(s, t3);
    } else {
        TCGv_i64 t0 = tcg_temp_new_i64(s);
        TCGv_i64 t1 = tcg_temp_new_i64(s);
        tcg_gen_ext_i32_i64(s, t0, arg1);
        tcg_gen_ext_i32_i64(s, t1, arg2);
        tcg_gen_mul_i64(s, t0, t0, t1);
        tcg_gen_extr_i64_i32(s, rl, rh, t0);
        tcg_temp_free_i64(s, t0);
        tcg_temp_free_i64(s, t1);
    }
}

void tcg_gen_mulsu2_i32(TCGContext *s, TCGv_i32 rl, TCGv_i32 rh, TCGv_i32 arg1, TCGv_i32 arg2)
{
    if (TCG_TARGET_REG_BITS == 32) {
        TCGv_i32 t0 = tcg_temp_new_i32(s);
        TCGv_i32 t1 = tcg_temp_new_i32(s);
        TCGv_i32 t2 = tcg_temp_new_i32(s);
        tcg_gen_mulu2_i32(s, t0, t1, arg1, arg2);
        /* Adjust for negative input for the signed arg1.  */
        tcg_gen_sari_i32(s, t2, arg1, 31);
        tcg_gen_and_i32(s, t2, t2, arg2);
        tcg_gen_sub_i32(s, rh, t1, t2);
        tcg_gen_mov_i32(s, rl, t0);
        tcg_temp_free_i32(s, t0);
        tcg_temp_free_i32(s, t1);
        tcg_temp_free_i32(s, t2);
    } else {
        TCGv_i64 t0 = tcg_temp_new_i64(s);
        TCGv_i64 t1 = tcg_temp_new_i64(s);
        tcg_gen_ext_i32_i64(s, t0, arg1);
        tcg_gen_extu_i32_i64(s, t1, arg2);
        tcg_gen_mul_i64(s, t0, t0, t1);
        tcg_gen_extr_i64_i32(s, rl, rh, t0);
        tcg_temp_free_i64(s, t0);
        tcg_temp_free_i64(s, t1);
    }
}

void tcg_gen_ext8s_i32(TCGContext *s, TCGv_i32 ret, TCGv_i32 arg)
{
    if (TCG_TARGET_HAS_ext8s_i32) {
        tcg_gen_op2_i32(s, INDEX_op_ext8s_i32, ret, arg);
    } else {
        tcg_gen_shli_i32(s, ret, arg, 24);
        tcg_gen_sari_i32(s, ret, ret, 24);
    }
}

void tcg_gen_ext16s_i32(TCGContext *s, TCGv_i32 ret, TCGv_i32 arg)
{
    if (TCG_TARGET_HAS_ext16s_i32) {
        tcg_gen_op2_i32(s, INDEX_op_ext16s_i32, ret, arg);
    } else {
        tcg_gen_shli_i32(s, ret, arg, 16);
        tcg_gen_sari_i32(s, ret, ret, 16);
    }
}

void tcg_gen_ext8u_i32(TCGContext *s, TCGv_i32 ret, TCGv_i32 arg)
{
    if (TCG_TARGET_HAS_ext8u_i32) {
        tcg_gen_op2_i32(s, INDEX_op_ext8u_i32, ret, arg);
    } else {
        tcg_gen_andi_i32(s, ret, arg, 0xffu);
    }
}

void tcg_gen_ext16u_i32(TCGContext *s, TCGv_i32 ret, TCGv_i32 arg)
{
    if (TCG_TARGET_HAS_ext16u_i32) {
        tcg_gen_op2_i32(s, INDEX_op_ext16u_i32, ret, arg);
    } else {
        tcg_gen_andi_i32(s, ret, arg, 0xffffu);
    }
}

/* Note: we assume the two high bytes are set to zero */
void tcg_gen_bswap16_i32(TCGContext *s, TCGv_i32 ret, TCGv_i32 arg)
{
    if (TCG_TARGET_HAS_bswap16_i32) {
        tcg_gen_op2_i32(s, INDEX_op_bswap16_i32, ret, arg);
    } else {
        TCGv_i32 t0 = tcg_temp_new_i32(s);

        tcg_gen_ext8u_i32(s, t0, arg);
        tcg_gen_shli_i32(s, t0, t0, 8);
        tcg_gen_shri_i32(s, ret, arg, 8);
        tcg_gen_or_i32(s, ret, ret, t0);
        tcg_temp_free_i32(s, t0);
    }
}

void tcg_gen_bswap32_i32(TCGContext *s, TCGv_i32 ret, TCGv_i32 arg)
{
    if (TCG_TARGET_HAS_bswap32_i32) {
        tcg_gen_op2_i32(s, INDEX_op_bswap32_i32, ret, arg);
    } else {
        TCGv_i32 t0, t1;
        t0 = tcg_temp_new_i32(s);
        t1 = tcg_temp_new_i32(s);

        tcg_gen_shli_i32(s, t0, arg, 24);

        tcg_gen_andi_i32(s, t1, arg, 0x0000ff00);
        tcg_gen_shli_i32(s, t1, t1, 8);
        tcg_gen_or_i32(s, t0, t0, t1);

        tcg_gen_shri_i32(s, t1, arg, 8);
        tcg_gen_andi_i32(s, t1, t1, 0x0000ff00);
        tcg_gen_or_i32(s, t0, t0, t1);

        tcg_gen_shri_i32(s, t1, arg, 24);
        tcg_gen_or_i32(s, ret, t0, t1);
        tcg_temp_free_i32(s, t0);
        tcg_temp_free_i32(s, t1);
    }
}

/* 64-bit ops */

#if TCG_TARGET_REG_BITS == 32
/* These are all inline for TCG_TARGET_REG_BITS == 64.  */

void tcg_gen_discard_i64(TCGContext *s, TCGv_i64 arg)
{
    tcg_gen_discard_i32(s, TCGV_LOW(arg));
    tcg_gen_discard_i32(s, TCGV_HIGH(arg));
}

void tcg_gen_mov_i64(TCGContext *s, TCGv_i64 ret, TCGv_i64 arg)
{
    tcg_gen_mov_i32(s, TCGV_LOW(ret), TCGV_LOW(arg));
    tcg_gen_mov_i32(s, TCGV_HIGH(ret), TCGV_HIGH(arg));
}

void tcg_gen_movi_i64(TCGContext *s, TCGv_i64 ret, int64_t arg)
{
    tcg_gen_movi_i32(s, TCGV_LOW(ret), arg);
    tcg_gen_movi_i32(s, TCGV_HIGH(ret), arg >> 32);
}

void tcg_gen_ld8u_i64(TCGContext *s, TCGv_i64 ret, TCGv_ptr arg2, tcg_target_long offset)
{
    tcg_gen_ld8u_i32(s, TCGV_LOW(ret), arg2, offset);
    tcg_gen_movi_i32(s, TCGV_HIGH(ret), 0);
}

void tcg_gen_ld8s_i64(TCGContext *s, TCGv_i64 ret, TCGv_ptr arg2, tcg_target_long offset)
{
    tcg_gen_ld8s_i32(s, TCGV_LOW(ret), arg2, offset);
    tcg_gen_sari_i32(s, TCGV_HIGH(ret), TCGV_LOW(ret), 31);
}

void tcg_gen_ld16u_i64(TCGContext *s, TCGv_i64 ret, TCGv_ptr arg2, tcg_target_long offset)
{
    tcg_gen_ld16u_i32(s, TCGV_LOW(ret), arg2, offset);
    tcg_gen_movi_i32(s, TCGV_HIGH(ret), 0);
}

void tcg_gen_ld16s_i64(TCGContext *s, TCGv_i64 ret, TCGv_ptr arg2, tcg_target_long offset)
{
    tcg_gen_ld16s_i32(s, TCGV_LOW(ret), arg2, offset);
    tcg_gen_sari_i32(s, TCGV_HIGH(ret), TCGV_LOW(ret), 31);
}

void tcg_gen_ld32u_i64(TCGContext *s, TCGv_i64 ret, TCGv_ptr arg2, tcg_target_long offset)
{
    tcg_gen_ld_i32(s, TCGV_LOW(ret), arg2, offset);
    tcg_gen_movi_i32(s, TCGV_HIGH(ret), 0);
}

void tcg_gen_ld32s_i64(TCGContext *s, TCGv_i64 ret, TCGv_ptr arg2, tcg_target_long offset)
{
    tcg_gen_ld_i32(s, TCGV_LOW(ret), arg2, offset);
    tcg_gen_sari_i32(s, TCGV_HIGH(ret), TCGV_LOW(ret), 31);
}

void tcg_gen_ld_i64(TCGContext *s, TCGv_i64 ret, TCGv_ptr arg2, tcg_target_long offset)
{
    /* Since arg2 and ret have different types,
       they cannot be the same temporary */
#ifdef TCG_TARGET_WORDS_BIGENDIAN
    tcg_gen_ld_i32(s, TCGV_HIGH(ret), arg2, offset);
    tcg_gen_ld_i32(s, TCGV_LOW(ret), arg2, offset + 4);
#else
    tcg_gen_ld_i32(s, TCGV_LOW(ret), arg2, offset);
    tcg_gen_ld_i32(s, TCGV_HIGH(ret), arg2, offset + 4);
#endif
}

void tcg_gen_st_i64(TCGContext *s, TCGv_i64 arg1, TCGv_ptr arg2, tcg_target_long offset)
{
#ifdef TCG_TARGET_WORDS_BIGENDIAN
    tcg_gen_st_i32(s, TCGV_HIGH(arg1), arg2, offset);
    tcg_gen_st_i32(s, TCGV_LOW(arg1), arg2, offset + 4);
#else
    tcg_gen_st_i32(s, TCGV_LOW(arg1), arg2, offset);
    tcg_gen_st_i32(s, TCGV_HIGH(arg1), arg2, offset + 4);
#endif
}

void tcg_gen_and_i64(TCGContext *s, TCGv_i64 ret, TCGv_i64 arg1, TCGv_i64 arg2)
{
    tcg_gen_and_i32(s, TCGV_LOW(ret), TCGV_LOW(arg1), TCGV_LOW(arg2));
    tcg_gen_and_i32(s, TCGV_HIGH(ret), TCGV_HIGH(arg1), TCGV_HIGH(arg2));
}

void tcg_gen_or_i64(TCGContext *s, TCGv_i64 ret, TCGv_i64 arg1, TCGv_i64 arg2)
{
    tcg_gen_or_i32(s, TCGV_LOW(ret), TCGV_LOW(arg1), TCGV_LOW(arg2));
    tcg_gen_or_i32(s, TCGV_HIGH(ret), TCGV_HIGH(arg1), TCGV_HIGH(arg2));
}

void tcg_gen_xor_i64(TCGContext *s, TCGv_i64 ret, TCGv_i64 arg1, TCGv_i64 arg2)
{
    tcg_gen_xor_i32(s, TCGV_LOW(ret), TCGV_LOW(arg1), TCGV_LOW(arg2));
    tcg_gen_xor_i32(s, TCGV_HIGH(ret), TCGV_HIGH(arg1), TCGV_HIGH(arg2));
}

void tcg_gen_shl_i64(TCGContext *s, TCGv_i64 ret, TCGv_i64 arg1, TCGv_i64 arg2)
{
    gen_helper_shl_i64(s, ret, arg1, arg2);
}

void tcg_gen_shr_i64(TCGContext *s, TCGv_i64 ret, TCGv_i64 arg1, TCGv_i64 arg2)
{
    gen_helper_shr_i64(s, ret, arg1, arg2);
}

void tcg_gen_sar_i64(TCGContext *s, TCGv_i64 ret, TCGv_i64 arg1, TCGv_i64 arg2)
{
    gen_helper_sar_i64(s, ret, arg1, arg2);
}

void tcg_gen_mul_i64(TCGContext *s, TCGv_i64 ret, TCGv_i64 arg1, TCGv_i64 arg2)
{
    TCGv_i64 t0;
    TCGv_i32 t1;

    t0 = tcg_temp_new_i64(s);
    t1 = tcg_temp_new_i32(s);

    tcg_gen_mulu2_i32(s, TCGV_LOW(t0), TCGV_HIGH(t0),
                      TCGV_LOW(arg1), TCGV_LOW(arg2));

    tcg_gen_mul_i32(s, t1, TCGV_LOW(arg1), TCGV_HIGH(arg2));
    tcg_gen_add_i32(s, TCGV_HIGH(t0), TCGV_HIGH(t0), t1);
    tcg_gen_mul_i32(s, t1, TCGV_HIGH(arg1), TCGV_LOW(arg2));
    tcg_gen_add_i32(s, TCGV_HIGH(t0), TCGV_HIGH(t0), t1);

    tcg_gen_mov_i64(s, ret, t0);
    tcg_temp_free_i64(s, t0);
    tcg_temp_free_i32(s, t1);
}
#endif /* TCG_TARGET_REG_SIZE == 32 */

void tcg_gen_addi_i64(TCGContext *s, TCGv_i64 ret, TCGv_i64 arg1, int64_t arg2)
{
    /* some cases can be optimized here */
    if (arg2 == 0) {
        tcg_gen_mov_i64(s, ret, arg1);
    } else {
        TCGv_i64 t0 = tcg_const_i64(s, arg2);
        tcg_gen_add_i64(s, ret, arg1, t0);
        tcg_temp_free_i64(s, t0);
    }
}

void tcg_gen_subfi_i64(TCGContext *s, TCGv_i64 ret, int64_t arg1, TCGv_i64 arg2)
{
    if (arg1 == 0 && TCG_TARGET_HAS_neg_i64) {
        /* Don't recurse with tcg_gen_neg_i64.  */
        tcg_gen_op2_i64(s, INDEX_op_neg_i64, ret, arg2);
    } else {
        TCGv_i64 t0 = tcg_const_i64(s, arg1);
        tcg_gen_sub_i64(s, ret, t0, arg2);
        tcg_temp_free_i64(s, t0);
    }
}

void tcg_gen_subi_i64(TCGContext *s, TCGv_i64 ret, TCGv_i64 arg1, int64_t arg2)
{
    /* some cases can be optimized here */
    if (arg2 == 0) {
        tcg_gen_mov_i64(s, ret, arg1);
    } else {
        TCGv_i64 t0 = tcg_const_i64(s, arg2);
        tcg_gen_sub_i64(s, ret, arg1, t0);
        tcg_temp_free_i64(s, t0);
    }
}

void tcg_gen_andi_i64(TCGContext *s, TCGv_i64 ret, TCGv_i64 arg1, uint64_t arg2)
{
    TCGv_i64 t0;

    if (TCG_TARGET_REG_BITS == 32) {
        tcg_gen_andi_i32(s, TCGV_LOW(ret), TCGV_LOW(arg1), arg2);
        tcg_gen_andi_i32(s, TCGV_HIGH(ret), TCGV_HIGH(arg1), arg2 >> 32);
        return;
    }

    /* Some cases can be optimized here.  */
    switch (arg2) {
    case 0:
        tcg_gen_movi_i64(s, ret, 0);
        return;
    case 0xffffffffffffffffull:
        tcg_gen_mov_i64(s, ret, arg1);
        return;
    case 0xffull:
        /* Don't recurse with tcg_gen_ext8u_i64.  */
        if (TCG_TARGET_HAS_ext8u_i64) {
            tcg_gen_op2_i64(s, INDEX_op_ext8u_i64, ret, arg1);
            return;
        }
        break;
    case 0xffffu:
        if (TCG_TARGET_HAS_ext16u_i64) {
            tcg_gen_op2_i64(s, INDEX_op_ext16u_i64, ret, arg1);
            return;
        }
        break;
    case 0xffffffffull:
        if (TCG_TARGET_HAS_ext32u_i64) {
            tcg_gen_op2_i64(s, INDEX_op_ext32u_i64, ret, arg1);
            return;
        }
        break;
    }
    t0 = tcg_const_i64(s, arg2);
    tcg_gen_and_i64(s, ret, arg1, t0);
    tcg_temp_free_i64(s, t0);
}

void tcg_gen_ori_i64(TCGContext *s, TCGv_i64 ret, TCGv_i64 arg1, int64_t arg2)
{
    if (TCG_TARGET_REG_BITS == 32) {
        tcg_gen_ori_i32(s, TCGV_LOW(ret), TCGV_LOW(arg1), arg2);
        tcg_gen_ori_i32(s, TCGV_HIGH(ret), TCGV_HIGH(arg1), arg2 >> 32);
        return;
    }
    /* Some cases can be optimized here.  */
    if (arg2 == -1) {
        tcg_gen_movi_i64(s, ret, -1);
    } else if (arg2 == 0) {
        tcg_gen_mov_i64(s, ret, arg1);
    } else {
        TCGv_i64 t0 = tcg_const_i64(s, arg2);
        tcg_gen_or_i64(s, ret, arg1, t0);
        tcg_temp_free_i64(s, t0);
    }
}

void tcg_gen_xori_i64(TCGContext *s, TCGv_i64 ret, TCGv_i64 arg1, int64_t arg2)
{
    if (TCG_TARGET_REG_BITS == 32) {
        tcg_gen_xori_i32(s, TCGV_LOW(ret), TCGV_LOW(arg1), arg2);
        tcg_gen_xori_i32(s, TCGV_HIGH(ret), TCGV_HIGH(arg1), arg2 >> 32);
        return;
    }
    /* Some cases can be optimized here.  */
    if (arg2 == 0) {
        tcg_gen_mov_i64(s, ret, arg1);
    } else if (arg2 == -1 && TCG_TARGET_HAS_not_i64) {
        /* Don't recurse with tcg_gen_not_i64.  */
        tcg_gen_op2_i64(s, INDEX_op_not_i64, ret, arg1);
    } else {
        TCGv_i64 t0 = tcg_const_i64(s, arg2);
        tcg_gen_xor_i64(s, ret, arg1, t0);
        tcg_temp_free_i64(s, t0);
    }
}

static inline void tcg_gen_shifti_i64(TCGContext *s, TCGv_i64 ret, TCGv_i64 arg1,
                                      unsigned c, bool right, bool arith)
{
    tcg_debug_assert(c < 64);
    if (c == 0) {
        tcg_gen_mov_i32(s, TCGV_LOW(ret), TCGV_LOW(arg1));
        tcg_gen_mov_i32(s, TCGV_HIGH(ret), TCGV_HIGH(arg1));
    } else if (c >= 32) {
        c -= 32;
        if (right) {
            if (arith) {
                tcg_gen_sari_i32(s, TCGV_LOW(ret), TCGV_HIGH(arg1), c);
                tcg_gen_sari_i32(s, TCGV_HIGH(ret), TCGV_HIGH(arg1), 31);
            } else {
                tcg_gen_shri_i32(s, TCGV_LOW(ret), TCGV_HIGH(arg1), c);
                tcg_gen_movi_i32(s, TCGV_HIGH(ret), 0);
            }
        } else {
            tcg_gen_shli_i32(s, TCGV_HIGH(ret), TCGV_LOW(arg1), c);
            tcg_gen_movi_i32(s, TCGV_LOW(ret), 0);
        }
    } else {
        TCGv_i32 t0, t1;

        t0 = tcg_temp_new_i32(s);
        t1 = tcg_temp_new_i32(s);
        if (right) {
            tcg_gen_shli_i32(s, t0, TCGV_HIGH(arg1), 32 - c);
            if (arith) {
                tcg_gen_sari_i32(s, t1, TCGV_HIGH(arg1), c);
            } else {
                tcg_gen_shri_i32(s, t1, TCGV_HIGH(arg1), c);
            }
            tcg_gen_shri_i32(s, TCGV_LOW(ret), TCGV_LOW(arg1), c);
            tcg_gen_or_i32(s, TCGV_LOW(ret), TCGV_LOW(ret), t0);
            tcg_gen_mov_i32(s, TCGV_HIGH(ret), t1);
        } else {
            tcg_gen_shri_i32(s, t0, TCGV_LOW(arg1), 32 - c);
            /* Note: ret can be the same as arg1, so we use t1 */
            tcg_gen_shli_i32(s, t1, TCGV_LOW(arg1), c);
            tcg_gen_shli_i32(s, TCGV_HIGH(ret), TCGV_HIGH(arg1), c);
            tcg_gen_or_i32(s, TCGV_HIGH(ret), TCGV_HIGH(ret), t0);
            tcg_gen_mov_i32(s, TCGV_LOW(ret), t1);
        }
        tcg_temp_free_i32(s, t0);
        tcg_temp_free_i32(s, t1);
    }
}

void tcg_gen_shli_i64(TCGContext *s, TCGv_i64 ret, TCGv_i64 arg1, unsigned arg2)
{
    tcg_debug_assert(arg2 < 64);
    if (TCG_TARGET_REG_BITS == 32) {
        tcg_gen_shifti_i64(s, ret, arg1, arg2, 0, 0);
    } else if (arg2 == 0) {
        tcg_gen_mov_i64(s, ret, arg1);
    } else {
        TCGv_i64 t0 = tcg_const_i64(s, arg2);
        tcg_gen_shl_i64(s, ret, arg1, t0);
        tcg_temp_free_i64(s, t0);
    }
}

void tcg_gen_shri_i64(TCGContext *s, TCGv_i64 ret, TCGv_i64 arg1, unsigned arg2)
{
    tcg_debug_assert(arg2 < 64);
    if (TCG_TARGET_REG_BITS == 32) {
        tcg_gen_shifti_i64(s, ret, arg1, arg2, 1, 0);
    } else if (arg2 == 0) {
        tcg_gen_mov_i64(s, ret, arg1);
    } else {
        TCGv_i64 t0 = tcg_const_i64(s, arg2);
        tcg_gen_shr_i64(s, ret, arg1, t0);
        tcg_temp_free_i64(s, t0);
    }
}

void tcg_gen_sari_i64(TCGContext *s, TCGv_i64 ret, TCGv_i64 arg1, unsigned arg2)
{
    tcg_debug_assert(arg2 < 64);
    if (TCG_TARGET_REG_BITS == 32) {
        tcg_gen_shifti_i64(s, ret, arg1, arg2, 1, 1);
    } else if (arg2 == 0) {
        tcg_gen_mov_i64(s, ret, arg1);
    } else {
        TCGv_i64 t0 = tcg_const_i64(s, arg2);
        tcg_gen_sar_i64(s, ret, arg1, t0);
        tcg_temp_free_i64(s, t0);
    }
}

void tcg_gen_brcond_i64(TCGContext *s, TCGCond cond, TCGv_i64 arg1, TCGv_i64 arg2, TCGLabel *l)
{
    if (cond == TCG_COND_ALWAYS) {
        tcg_gen_br(s, l);
    } else if (cond != TCG_COND_NEVER) {
        if (TCG_TARGET_REG_BITS == 32) {
            tcg_gen_op6ii_i32(s, INDEX_op_brcond2_i32, TCGV_LOW(arg1),
                              TCGV_HIGH(arg1), TCGV_LOW(arg2),
                              TCGV_HIGH(arg2), cond, label_arg(s, l));
        } else {
            tcg_gen_op4ii_i64(s, INDEX_op_brcond_i64, arg1, arg2, cond,
                              label_arg(s, l));
        }
    }
}

void tcg_gen_brcondi_i64(TCGContext *s, TCGCond cond, TCGv_i64 arg1, int64_t arg2, TCGLabel *l)
{
    if (cond == TCG_COND_ALWAYS) {
        tcg_gen_br(s, l);
    } else if (cond != TCG_COND_NEVER) {
        TCGv_i64 t0 = tcg_const_i64(s, arg2);
        tcg_gen_brcond_i64(s, cond, arg1, t0, l);
        tcg_temp_free_i64(s, t0);
    }
}

void tcg_gen_setcond_i64(TCGContext *s, TCGCond cond, TCGv_i64 ret,
                         TCGv_i64 arg1, TCGv_i64 arg2)
{
    if (cond == TCG_COND_ALWAYS) {
        tcg_gen_movi_i64(s, ret, 1);
    } else if (cond == TCG_COND_NEVER) {
        tcg_gen_movi_i64(s, ret, 0);
    } else {
        if (TCG_TARGET_REG_BITS == 32) {
            tcg_gen_op6i_i32(s, INDEX_op_setcond2_i32, TCGV_LOW(ret),
                             TCGV_LOW(arg1), TCGV_HIGH(arg1),
                             TCGV_LOW(arg2), TCGV_HIGH(arg2), cond);
            tcg_gen_movi_i32(s, TCGV_HIGH(ret), 0);
        } else {
            tcg_gen_op4i_i64(s, INDEX_op_setcond_i64, ret, arg1, arg2, cond);
        }
    }
}

void tcg_gen_setcondi_i64(TCGContext *s, TCGCond cond, TCGv_i64 ret,
                          TCGv_i64 arg1, int64_t arg2)
{
    TCGv_i64 t0 = tcg_const_i64(s, arg2);
    tcg_gen_setcond_i64(s, cond, ret, arg1, t0);
    tcg_temp_free_i64(s, t0);
}

void tcg_gen_muli_i64(TCGContext *s, TCGv_i64 ret, TCGv_i64 arg1, int64_t arg2)
{
    TCGv_i64 t0 = tcg_const_i64(s, arg2);
    tcg_gen_mul_i64(s, ret, arg1, t0);
    tcg_temp_free_i64(s, t0);
}

void tcg_gen_div_i64(TCGContext *s, TCGv_i64 ret, TCGv_i64 arg1, TCGv_i64 arg2)
{
    if (TCG_TARGET_HAS_div_i64) {
        tcg_gen_op3_i64(s, INDEX_op_div_i64, ret, arg1, arg2);
    } else if (TCG_TARGET_HAS_div2_i64) {
        TCGv_i64 t0 = tcg_temp_new_i64(s);
        tcg_gen_sari_i64(s, t0, arg1, 63);
        tcg_gen_op5_i64(s, INDEX_op_div2_i64, ret, t0, arg1, t0, arg2);
        tcg_temp_free_i64(s, t0);
    } else {
        gen_helper_div_i64(s, ret, arg1, arg2);
    }
}

void tcg_gen_rem_i64(TCGContext *s, TCGv_i64 ret, TCGv_i64 arg1, TCGv_i64 arg2)
{
    if (TCG_TARGET_HAS_rem_i64) {
        tcg_gen_op3_i64(s, INDEX_op_rem_i64, ret, arg1, arg2);
    } else if (TCG_TARGET_HAS_div_i64) {
        TCGv_i64 t0 = tcg_temp_new_i64(s);
        tcg_gen_op3_i64(s, INDEX_op_div_i64, t0, arg1, arg2);
        tcg_gen_mul_i64(s, t0, t0, arg2);
        tcg_gen_sub_i64(s, ret, arg1, t0);
        tcg_temp_free_i64(s, t0);
    } else if (TCG_TARGET_HAS_div2_i64) {
        TCGv_i64 t0 = tcg_temp_new_i64(s);
        tcg_gen_sari_i64(s, t0, arg1, 63);
        tcg_gen_op5_i64(s, INDEX_op_div2_i64, t0, ret, arg1, t0, arg2);
        tcg_temp_free_i64(s, t0);
    } else {
        gen_helper_rem_i64(s, ret, arg1, arg2);
    }
}

void tcg_gen_divu_i64(TCGContext *s, TCGv_i64 ret, TCGv_i64 arg1, TCGv_i64 arg2)
{
    if (TCG_TARGET_HAS_div_i64) {
        tcg_gen_op3_i64(s, INDEX_op_divu_i64, ret, arg1, arg2);
    } else if (TCG_TARGET_HAS_div2_i64) {
        TCGv_i64 t0 = tcg_temp_new_i64(s);
        tcg_gen_movi_i64(s, t0, 0);
        tcg_gen_op5_i64(s, INDEX_op_divu2_i64, ret, t0, arg1, t0, arg2);
        tcg_temp_free_i64(s, t0);
    } else {
        gen_helper_divu_i64(s, ret, arg1, arg2);
    }
}

void tcg_gen_remu_i64(TCGContext *s, TCGv_i64 ret, TCGv_i64 arg1, TCGv_i64 arg2)
{
    if (TCG_TARGET_HAS_rem_i64) {
        tcg_gen_op3_i64(s, INDEX_op_remu_i64, ret, arg1, arg2);
    } else if (TCG_TARGET_HAS_div_i64) {
        TCGv_i64 t0 = tcg_temp_new_i64(s);
        tcg_gen_op3_i64(s, INDEX_op_divu_i64, t0, arg1, arg2);
        tcg_gen_mul_i64(s, t0, t0, arg2);
        tcg_gen_sub_i64(s, ret, arg1, t0);
        tcg_temp_free_i64(s, t0);
    } else if (TCG_TARGET_HAS_div2_i64) {
        TCGv_i64 t0 = tcg_temp_new_i64(s);
        tcg_gen_movi_i64(s, t0, 0);
        tcg_gen_op5_i64(s, INDEX_op_divu2_i64, t0, ret, arg1, t0, arg2);
        tcg_temp_free_i64(s, t0);
    } else {
        gen_helper_remu_i64(s, ret, arg1, arg2);
    }
}

void tcg_gen_ext8s_i64(TCGContext *s, TCGv_i64 ret, TCGv_i64 arg)
{
    if (TCG_TARGET_REG_BITS == 32) {
        tcg_gen_ext8s_i32(s, TCGV_LOW(ret), TCGV_LOW(arg));
        tcg_gen_sari_i32(s, TCGV_HIGH(ret), TCGV_LOW(ret), 31);
    } else if (TCG_TARGET_HAS_ext8s_i64) {
        tcg_gen_op2_i64(s, INDEX_op_ext8s_i64, ret, arg);
    } else {
        tcg_gen_shli_i64(s, ret, arg, 56);
        tcg_gen_sari_i64(s, ret, ret, 56);
    }
}

void tcg_gen_ext16s_i64(TCGContext *s, TCGv_i64 ret, TCGv_i64 arg)
{
    if (TCG_TARGET_REG_BITS == 32) {
        tcg_gen_ext16s_i32(s, TCGV_LOW(ret), TCGV_LOW(arg));
        tcg_gen_sari_i32(s, TCGV_HIGH(ret), TCGV_LOW(ret), 31);
    } else if (TCG_TARGET_HAS_ext16s_i64) {
        tcg_gen_op2_i64(s, INDEX_op_ext16s_i64, ret, arg);
    } else {
        tcg_gen_shli_i64(s, ret, arg, 48);
        tcg_gen_sari_i64(s, ret, ret, 48);
    }
}

void tcg_gen_ext32s_i64(TCGContext *s, TCGv_i64 ret, TCGv_i64 arg)
{
    if (TCG_TARGET_REG_BITS == 32) {
        tcg_gen_mov_i32(s, TCGV_LOW(ret), TCGV_LOW(arg));
        tcg_gen_sari_i32(s, TCGV_HIGH(ret), TCGV_LOW(ret), 31);
    } else if (TCG_TARGET_HAS_ext32s_i64) {
        tcg_gen_op2_i64(s, INDEX_op_ext32s_i64, ret, arg);
    } else {
        tcg_gen_shli_i64(s, ret, arg, 32);
        tcg_gen_sari_i64(s, ret, ret, 32);
    }
}

void tcg_gen_ext8u_i64(TCGContext *s, TCGv_i64 ret, TCGv_i64 arg)
{
    if (TCG_TARGET_REG_BITS == 32) {
        tcg_gen_ext8u_i32(s, TCGV_LOW(ret), TCGV_LOW(arg));
        tcg_gen_movi_i32(s, TCGV_HIGH(ret), 0);
    } else if (TCG_TARGET_HAS_ext8u_i64) {
        tcg_gen_op2_i64(s, INDEX_op_ext8u_i64, ret, arg);
    } else {
        tcg_gen_andi_i64(s, ret, arg, 0xffu);
    }
}

void tcg_gen_ext16u_i64(TCGContext *s, TCGv_i64 ret, TCGv_i64 arg)
{
    if (TCG_TARGET_REG_BITS == 32) {
        tcg_gen_ext16u_i32(s, TCGV_LOW(ret), TCGV_LOW(arg));
        tcg_gen_movi_i32(s, TCGV_HIGH(ret), 0);
    } else if (TCG_TARGET_HAS_ext16u_i64) {
        tcg_gen_op2_i64(s, INDEX_op_ext16u_i64, ret, arg);
    } else {
        tcg_gen_andi_i64(s, ret, arg, 0xffffu);
    }
}

void tcg_gen_ext32u_i64(TCGContext *s, TCGv_i64 ret, TCGv_i64 arg)
{
    if (TCG_TARGET_REG_BITS == 32) {
        tcg_gen_mov_i32(s, TCGV_LOW(ret), TCGV_LOW(arg));
        tcg_gen_movi_i32(s, TCGV_HIGH(ret), 0);
    } else if (TCG_TARGET_HAS_ext32u_i64) {
        tcg_gen_op2_i64(s, INDEX_op_ext32u_i64, ret, arg);
    } else {
        tcg_gen_andi_i64(s, ret, arg, 0xffffffffu);
    }
}

/* Note: we assume the six high bytes are set to zero */
void tcg_gen_bswap16_i64(TCGContext *s, TCGv_i64 ret, TCGv_i64 arg)
{
    if (TCG_TARGET_REG_BITS == 32) {
        tcg_gen_bswap16_i32(s, TCGV_LOW(ret), TCGV_LOW(arg));
        tcg_gen_movi_i32(s, TCGV_HIGH(ret), 0);
    } else if (TCG_TARGET_HAS_bswap16_i64) {
        tcg_gen_op2_i64(s, INDEX_op_bswap16_i64, ret, arg);
    } else {
        TCGv_i64 t0 = tcg_temp_new_i64(s);

        tcg_gen_ext8u_i64(s, t0, arg);
        tcg_gen_shli_i64(s, t0, t0, 8);
        tcg_gen_shri_i64(s, ret, arg, 8);
        tcg_gen_or_i64(s, ret, ret, t0);
        tcg_temp_free_i64(s, t0);
    }
}

/* Note: we assume the four high bytes are set to zero */
void tcg_gen_bswap32_i64(TCGContext *s, TCGv_i64 ret, TCGv_i64 arg)
{
    if (TCG_TARGET_REG_BITS == 32) {
        tcg_gen_bswap32_i32(s, TCGV_LOW(ret), TCGV_LOW(arg));
        tcg_gen_movi_i32(s, TCGV_HIGH(ret), 0);
    } else if (TCG_TARGET_HAS_bswap32_i64) {
        tcg_gen_op2_i64(s, INDEX_op_bswap32_i64, ret, arg);
    } else {
        TCGv_i64 t0, t1;
        t0 = tcg_temp_new_i64(s);
        t1 = tcg_temp_new_i64(s);

        tcg_gen_shli_i64(s, t0, arg, 24);
        tcg_gen_ext32u_i64(s, t0, t0);

        tcg_gen_andi_i64(s, t1, arg, 0x0000ff00);
        tcg_gen_shli_i64(s, t1, t1, 8);
        tcg_gen_or_i64(s, t0, t0, t1);

        tcg_gen_shri_i64(s, t1, arg, 8);
        tcg_gen_andi_i64(s, t1, t1, 0x0000ff00);
        tcg_gen_or_i64(s, t0, t0, t1);

        tcg_gen_shri_i64(s, t1, arg, 24);
        tcg_gen_or_i64(s, ret, t0, t1);
        tcg_temp_free_i64(s, t0);
        tcg_temp_free_i64(s, t1);
    }
}

void tcg_gen_bswap64_i64(TCGContext *s, TCGv_i64 ret, TCGv_i64 arg)
{
    if (TCG_TARGET_REG_BITS == 32) {
        TCGv_i32 t0, t1;
        t0 = tcg_temp_new_i32(s);
        t1 = tcg_temp_new_i32(s);

        tcg_gen_bswap32_i32(s, t0, TCGV_LOW(arg));
        tcg_gen_bswap32_i32(s, t1, TCGV_HIGH(arg));
        tcg_gen_mov_i32(s, TCGV_LOW(ret), t1);
        tcg_gen_mov_i32(s, TCGV_HIGH(ret), t0);
        tcg_temp_free_i32(s, t0);
        tcg_temp_free_i32(s, t1);
    } else if (TCG_TARGET_HAS_bswap64_i64) {
        tcg_gen_op2_i64(s, INDEX_op_bswap64_i64, ret, arg);
    } else {
        TCGv_i64 t0 = tcg_temp_new_i64(s);
        TCGv_i64 t1 = tcg_temp_new_i64(s);

        tcg_gen_shli_i64(s, t0, arg, 56);

        tcg_gen_andi_i64(s, t1, arg, 0x0000ff00);
        tcg_gen_shli_i64(s, t1, t1, 40);
        tcg_gen_or_i64(s, t0, t0, t1);

        tcg_gen_andi_i64(s, t1, arg, 0x00ff0000);
        tcg_gen_shli_i64(s, t1, t1, 24);
        tcg_gen_or_i64(s, t0, t0, t1);

        tcg_gen_andi_i64(s, t1, arg, 0xff000000);
        tcg_gen_shli_i64(s, t1, t1, 8);
        tcg_gen_or_i64(s, t0, t0, t1);

        tcg_gen_shri_i64(s, t1, arg, 8);
        tcg_gen_andi_i64(s, t1, t1, 0xff000000);
        tcg_gen_or_i64(s, t0, t0, t1);

        tcg_gen_shri_i64(s, t1, arg, 24);
        tcg_gen_andi_i64(s, t1, t1, 0x00ff0000);
        tcg_gen_or_i64(s, t0, t0, t1);

        tcg_gen_shri_i64(s, t1, arg, 40);
        tcg_gen_andi_i64(s, t1, t1, 0x0000ff00);
        tcg_gen_or_i64(s, t0, t0, t1);

        tcg_gen_shri_i64(s, t1, arg, 56);
        tcg_gen_or_i64(s, ret, t0, t1);
        tcg_temp_free_i64(s, t0);
        tcg_temp_free_i64(s, t1);
    }
}

void tcg_gen_not_i64(TCGContext *s, TCGv_i64 ret, TCGv_i64 arg)
{
    if (TCG_TARGET_REG_BITS == 32) {
        tcg_gen_not_i32(s, TCGV_LOW(ret), TCGV_LOW(arg));
        tcg_gen_not_i32(s, TCGV_HIGH(ret), TCGV_HIGH(arg));
    } else if (TCG_TARGET_HAS_not_i64) {
        tcg_gen_op2_i64(s, INDEX_op_not_i64, ret, arg);
    } else {
        tcg_gen_xori_i64(s, ret, arg, -1);
    }
}

void tcg_gen_andc_i64(TCGContext *s, TCGv_i64 ret, TCGv_i64 arg1, TCGv_i64 arg2)
{
    if (TCG_TARGET_REG_BITS == 32) {
        tcg_gen_andc_i32(s, TCGV_LOW(ret), TCGV_LOW(arg1), TCGV_LOW(arg2));
        tcg_gen_andc_i32(s, TCGV_HIGH(ret), TCGV_HIGH(arg1), TCGV_HIGH(arg2));
    } else if (TCG_TARGET_HAS_andc_i64) {
        tcg_gen_op3_i64(s, INDEX_op_andc_i64, ret, arg1, arg2);
    } else {
        TCGv_i64 t0 = tcg_temp_new_i64(s);
        tcg_gen_not_i64(s, t0, arg2);
        tcg_gen_and_i64(s, ret, arg1, t0);
        tcg_temp_free_i64(s, t0);
    }
}

void tcg_gen_eqv_i64(TCGContext *s, TCGv_i64 ret, TCGv_i64 arg1, TCGv_i64 arg2)
{
    if (TCG_TARGET_REG_BITS == 32) {
        tcg_gen_eqv_i32(s, TCGV_LOW(ret), TCGV_LOW(arg1), TCGV_LOW(arg2));
        tcg_gen_eqv_i32(s, TCGV_HIGH(ret), TCGV_HIGH(arg1), TCGV_HIGH(arg2));
    } else if (TCG_TARGET_HAS_eqv_i64) {
        tcg_gen_op3_i64(s, INDEX_op_eqv_i64, ret, arg1, arg2);
    } else {
        tcg_gen_xor_i64(s, ret, arg1, arg2);
        tcg_gen_not_i64(s, ret, ret);
    }
}

void tcg_gen_nand_i64(TCGContext *s, TCGv_i64 ret, TCGv_i64 arg1, TCGv_i64 arg2)
{
    if (TCG_TARGET_REG_BITS == 32) {
        tcg_gen_nand_i32(s, TCGV_LOW(ret), TCGV_LOW(arg1), TCGV_LOW(arg2));
        tcg_gen_nand_i32(s, TCGV_HIGH(ret), TCGV_HIGH(arg1), TCGV_HIGH(arg2));
    } else if (TCG_TARGET_HAS_nand_i64) {
        tcg_gen_op3_i64(s, INDEX_op_nand_i64, ret, arg1, arg2);
    } else {
        tcg_gen_and_i64(s, ret, arg1, arg2);
        tcg_gen_not_i64(s, ret, ret);
    }
}

void tcg_gen_nor_i64(TCGContext *s, TCGv_i64 ret, TCGv_i64 arg1, TCGv_i64 arg2)
{
    if (TCG_TARGET_REG_BITS == 32) {
        tcg_gen_nor_i32(s, TCGV_LOW(ret), TCGV_LOW(arg1), TCGV_LOW(arg2));
        tcg_gen_nor_i32(s, TCGV_HIGH(ret), TCGV_HIGH(arg1), TCGV_HIGH(arg2));
    } else if (TCG_TARGET_HAS_nor_i64) {
        tcg_gen_op3_i64(s, INDEX_op_nor_i64, ret, arg1, arg2);
    } else {
        tcg_gen_or_i64(s, ret, arg1, arg2);
        tcg_gen_not_i64(s, ret, ret);
    }
}

void tcg_gen_orc_i64(TCGContext *s, TCGv_i64 ret, TCGv_i64 arg1, TCGv_i64 arg2)
{
    if (TCG_TARGET_REG_BITS == 32) {
        tcg_gen_orc_i32(s, TCGV_LOW(ret), TCGV_LOW(arg1), TCGV_LOW(arg2));
        tcg_gen_orc_i32(s, TCGV_HIGH(ret), TCGV_HIGH(arg1), TCGV_HIGH(arg2));
    } else if (TCG_TARGET_HAS_orc_i64) {
        tcg_gen_op3_i64(s, INDEX_op_orc_i64, ret, arg1, arg2);
    } else {
        TCGv_i64 t0 = tcg_temp_new_i64(s);
        tcg_gen_not_i64(s, t0, arg2);
        tcg_gen_or_i64(s, ret, arg1, t0);
        tcg_temp_free_i64(s, t0);
    }
}

void tcg_gen_rotl_i64(TCGContext *s, TCGv_i64 ret, TCGv_i64 arg1, TCGv_i64 arg2)
{
    if (TCG_TARGET_HAS_rot_i64) {
        tcg_gen_op3_i64(s, INDEX_op_rotl_i64, ret, arg1, arg2);
    } else {
        TCGv_i64 t0, t1;
        t0 = tcg_temp_new_i64(s);
        t1 = tcg_temp_new_i64(s);
        tcg_gen_shl_i64(s, t0, arg1, arg2);
        tcg_gen_subfi_i64(s, t1, 64, arg2);
        tcg_gen_shr_i64(s, t1, arg1, t1);
        tcg_gen_or_i64(s, ret, t0, t1);
        tcg_temp_free_i64(s, t0);
        tcg_temp_free_i64(s, t1);
    }
}

void tcg_gen_rotli_i64(TCGContext *s, TCGv_i64 ret, TCGv_i64 arg1, unsigned arg2)
{
    tcg_debug_assert(arg2 < 64);
    /* some cases can be optimized here */
    if (arg2 == 0) {
        tcg_gen_mov_i64(s, ret, arg1);
    } else if (TCG_TARGET_HAS_rot_i64) {
        TCGv_i64 t0 = tcg_const_i64(s, arg2);
        tcg_gen_rotl_i64(s, ret, arg1, t0);
        tcg_temp_free_i64(s, t0);
    } else {
        TCGv_i64 t0, t1;
        t0 = tcg_temp_new_i64(s);
        t1 = tcg_temp_new_i64(s);
        tcg_gen_shli_i64(s, t0, arg1, arg2);
        tcg_gen_shri_i64(s, t1, arg1, 64 - arg2);
        tcg_gen_or_i64(s, ret, t0, t1);
        tcg_temp_free_i64(s, t0);
        tcg_temp_free_i64(s, t1);
    }
}

void tcg_gen_rotr_i64(TCGContext *s, TCGv_i64 ret, TCGv_i64 arg1, TCGv_i64 arg2)
{
    if (TCG_TARGET_HAS_rot_i64) {
        tcg_gen_op3_i64(s, INDEX_op_rotr_i64, ret, arg1, arg2);
    } else {
        TCGv_i64 t0, t1;
        t0 = tcg_temp_new_i64(s);
        t1 = tcg_temp_new_i64(s);
        tcg_gen_shr_i64(s, t0, arg1, arg2);
        tcg_gen_subfi_i64(s, t1, 64, arg2);
        tcg_gen_shl_i64(s, t1, arg1, t1);
        tcg_gen_or_i64(s, ret, t0, t1);
        tcg_temp_free_i64(s, t0);
        tcg_temp_free_i64(s, t1);
    }
}

void tcg_gen_rotri_i64(TCGContext *s, TCGv_i64 ret, TCGv_i64 arg1, unsigned arg2)
{
    tcg_debug_assert(arg2 < 64);
    /* some cases can be optimized here */
    if (arg2 == 0) {
        tcg_gen_mov_i64(s, ret, arg1);
    } else {
        tcg_gen_rotli_i64(s, ret, arg1, 64 - arg2);
    }
}

void tcg_gen_deposit_i64(TCGContext *s, TCGv_i64 ret, TCGv_i64 arg1, TCGv_i64 arg2,
                         unsigned int ofs, unsigned int len)
{
    uint64_t mask;
    TCGv_i64 t1;

    tcg_debug_assert(ofs < 64);
    tcg_debug_assert(len <= 64);
    tcg_debug_assert(ofs + len <= 64);

    if (ofs == 0 && len == 64) {
        tcg_gen_mov_i64(s, ret, arg2);
        return;
    }
    if (TCG_TARGET_HAS_deposit_i64 && TCG_TARGET_deposit_i64_valid(ofs, len)) {
        tcg_gen_op5ii_i64(s, INDEX_op_deposit_i64, ret, arg1, arg2, ofs, len);
        return;
    }

    if (TCG_TARGET_REG_BITS == 32) {
        if (ofs >= 32) {
            tcg_gen_deposit_i32(s, TCGV_HIGH(ret), TCGV_HIGH(arg1),
                                TCGV_LOW(arg2), ofs - 32, len);
            tcg_gen_mov_i32(s, TCGV_LOW(ret), TCGV_LOW(arg1));
            return;
        }
        if (ofs + len <= 32) {
            tcg_gen_deposit_i32(s, TCGV_LOW(ret), TCGV_LOW(arg1),
                                TCGV_LOW(arg2), ofs, len);
            tcg_gen_mov_i32(s, TCGV_HIGH(ret), TCGV_HIGH(arg1));
            return;
        }
    }

    mask = (1ull << len) - 1;
    t1 = tcg_temp_new_i64(s);

    if (ofs + len < 64) {
        tcg_gen_andi_i64(s, t1, arg2, mask);
        tcg_gen_shli_i64(s, t1, t1, ofs);
    } else {
        tcg_gen_shli_i64(s, t1, arg2, ofs);
    }
    tcg_gen_andi_i64(s, ret, arg1, ~(mask << ofs));
    tcg_gen_or_i64(s, ret, ret, t1);

    tcg_temp_free_i64(s, t1);
}

void tcg_gen_extract_i64(TCGContext *s, TCGv_i64 ret, TCGv_i64 arg,
                         unsigned int ofs, unsigned int len)
{
    tcg_debug_assert(ofs < 64);
    tcg_debug_assert(len > 0);
    tcg_debug_assert(len <= 64);
    tcg_debug_assert(ofs + len <= 64);

    /* Canonicalize certain special cases, even if extract is supported.  */
    if (ofs + len == 64) {
        tcg_gen_shri_i64(s, ret, arg, 64 - len);
        return;
    }
    if (ofs == 0) {
        tcg_gen_andi_i64(s, ret, arg, (1ull << len) - 1);
        return;
    }

    if (TCG_TARGET_REG_BITS == 32) {
        /* Look for a 32-bit extract within one of the two words.  */
        if (ofs >= 32) {
            tcg_gen_extract_i32(s, TCGV_LOW(ret), TCGV_HIGH(arg), ofs - 32, len);
            tcg_gen_movi_i32(s, TCGV_HIGH(ret), 0);
            return;
        }
        if (ofs + len <= 32) {
            tcg_gen_extract_i32(s, TCGV_LOW(ret), TCGV_LOW(arg), ofs, len);
            tcg_gen_movi_i32(s, TCGV_HIGH(ret), 0);
            return;
        }
        /* The field is split across two words.  One double-word
           shift is better than two double-word shifts.  */
        goto do_shift_and;
    }

    if (TCG_TARGET_HAS_extract_i64
        && TCG_TARGET_extract_i64_valid(ofs, len)) {
        tcg_gen_op4ii_i64(s, INDEX_op_extract_i64, ret, arg, ofs, len);
        return;
    }

    /* Assume that zero-extension, if available, is cheaper than a shift.  */
    switch (ofs + len) {
    case 32:
        if (TCG_TARGET_HAS_ext32u_i64) {
            tcg_gen_ext32u_i64(s, ret, arg);
            tcg_gen_shri_i64(s, ret, ret, ofs);
            return;
        }
        break;
    case 16:
        if (TCG_TARGET_HAS_ext16u_i64) {
            tcg_gen_ext16u_i64(s, ret, arg);
            tcg_gen_shri_i64(s, ret, ret, ofs);
            return;
        }
        break;
    case 8:
        if (TCG_TARGET_HAS_ext8u_i64) {
            tcg_gen_ext8u_i64(s, ret, arg);
            tcg_gen_shri_i64(s, ret, ret, ofs);
            return;
        }
        break;
    }

    /* ??? Ideally we'd know what values are available for immediate AND.
       Assume that 8 bits are available, plus the special cases of 16 and 32,
       so that we get ext8u, ext16u, and ext32u.  */
    switch (len) {
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8: case 16: case 32:
    do_shift_and:
        tcg_gen_shri_i64(s, ret, arg, ofs);
        tcg_gen_andi_i64(s, ret, ret, (1ull << len) - 1);
        break;
    default:
        tcg_gen_shli_i64(s, ret, arg, 64 - len - ofs);
        tcg_gen_shri_i64(s, ret, ret, 64 - len);
        break;
    }
}

void tcg_gen_sextract_i64(TCGContext *s, TCGv_i64 ret, TCGv_i64 arg,
                          unsigned int ofs, unsigned int len)
{
    tcg_debug_assert(ofs < 64);
    tcg_debug_assert(len > 0);
    tcg_debug_assert(len <= 64);
    tcg_debug_assert(ofs + len <= 64);

    /* Canonicalize certain special cases, even if sextract is supported.  */
    if (ofs + len == 64) {
        tcg_gen_sari_i64(s, ret, arg, 64 - len);
        return;
    }
    if (ofs == 0) {
        switch (len) {
        case 32:
            tcg_gen_ext32s_i64(s, ret, arg);
            return;
        case 16:
            tcg_gen_ext16s_i64(s, ret, arg);
            return;
        case 8:
            tcg_gen_ext8s_i64(s, ret, arg);
            return;
        }
    }

    if (TCG_TARGET_REG_BITS == 32) {
        /* Look for a 32-bit extract within one of the two words.  */
        if (ofs >= 32) {
            tcg_gen_sextract_i32(s, TCGV_LOW(ret), TCGV_HIGH(arg), ofs - 32, len);
        } else if (ofs + len <= 32) {
            tcg_gen_sextract_i32(s, TCGV_LOW(ret), TCGV_LOW(arg), ofs, len);
        } else if (ofs == 0) {
            tcg_gen_mov_i32(s, TCGV_LOW(ret), TCGV_LOW(arg));
            tcg_gen_sextract_i32(s, TCGV_HIGH(ret), TCGV_HIGH(arg), 0, len - 32);
            return;
        } else if (len > 32) {
            TCGv_i32 t = tcg_temp_new_i32(s);
            /* Extract the bits for the high word normally.  */
            tcg_gen_sextract_i32(s, t, TCGV_HIGH(arg), ofs + 32, len - 32);
            /* Shift the field down for the low part.  */
            tcg_gen_shri_i64(s, ret, arg, ofs);
            /* Overwrite the shift into the high part.  */
            tcg_gen_mov_i32(s, TCGV_HIGH(ret), t);
            tcg_temp_free_i32(s, t);
            return;
        } else {
            /* Shift the field down for the low part, such that the
               field sits at the MSB.  */
            tcg_gen_shri_i64(s, ret, arg, ofs + len - 32);
            /* Shift the field down from the MSB, sign extending.  */
            tcg_gen_sari_i32(s, TCGV_LOW(ret), TCGV_LOW(ret), 32 - len);
        }
        /* Sign-extend the field from 32 bits.  */
        tcg_gen_sari_i32(s, TCGV_HIGH(ret), TCGV_LOW(ret), 31);
        return;
    }

    if (TCG_TARGET_HAS_sextract_i64
        && TCG_TARGET_extract_i64_valid(ofs, len)) {
        tcg_gen_op4ii_i64(s, INDEX_op_sextract_i64, ret, arg, ofs, len);
        return;
    }

    /* Assume that sign-extension, if available, is cheaper than a shift.  */
    switch (ofs + len) {
    case 32:
        if (TCG_TARGET_HAS_ext32s_i64) {
            tcg_gen_ext32s_i64(s, ret, arg);
            tcg_gen_sari_i64(s, ret, ret, ofs);
            return;
        }
        break;
    case 16:
        if (TCG_TARGET_HAS_ext16s_i64) {
            tcg_gen_ext16s_i64(s, ret, arg);
            tcg_gen_sari_i64(s, ret, ret, ofs);
            return;
        }
        break;
    case 8:
        if (TCG_TARGET_HAS_ext8s_i64) {
            tcg_gen_ext8s_i64(s, ret, arg);
            tcg_gen_sari_i64(s, ret, ret, ofs);
            return;
        }
        break;
    }
    switch (len) {
    case 32:
        if (TCG_TARGET_HAS_ext32s_i64) {
            tcg_gen_shri_i64(s, ret, arg, ofs);
            tcg_gen_ext32s_i64(s, ret, ret);
            return;
        }
        break;
    case 16:
        if (TCG_TARGET_HAS_ext16s_i64) {
            tcg_gen_shri_i64(s, ret, arg, ofs);
            tcg_gen_ext16s_i64(s, ret, ret);
            return;
        }
        break;
    case 8:
        if (TCG_TARGET_HAS_ext8s_i64) {
            tcg_gen_shri_i64(s, ret, arg, ofs);
            tcg_gen_ext8s_i64(s, ret, ret);
            return;
        }
        break;
    }
    tcg_gen_shli_i64(s, ret, arg, 64 - len - ofs);
    tcg_gen_sari_i64(s, ret, ret, 64 - len);
}

void tcg_gen_movcond_i64(TCGContext *s, TCGCond cond, TCGv_i64 ret, TCGv_i64 c1,
                         TCGv_i64 c2, TCGv_i64 v1, TCGv_i64 v2)
{
    if (cond == TCG_COND_ALWAYS) {
        tcg_gen_mov_i64(s, ret, v1);
    } else if (cond == TCG_COND_NEVER) {
        tcg_gen_mov_i64(s, ret, v2);
    } else if (TCG_TARGET_REG_BITS == 32) {
        TCGv_i32 t0 = tcg_temp_new_i32(s);
        TCGv_i32 t1 = tcg_temp_new_i32(s);
        tcg_gen_op6i_i32(s, INDEX_op_setcond2_i32, t0,
                         TCGV_LOW(c1), TCGV_HIGH(c1),
                         TCGV_LOW(c2), TCGV_HIGH(c2), cond);

        if (TCG_TARGET_HAS_movcond_i32) {
            tcg_gen_movi_i32(s, t1, 0);
            tcg_gen_movcond_i32(s, TCG_COND_NE, TCGV_LOW(ret), t0, t1,
                                TCGV_LOW(v1), TCGV_LOW(v2));
            tcg_gen_movcond_i32(s, TCG_COND_NE, TCGV_HIGH(ret), t0, t1,
                                TCGV_HIGH(v1), TCGV_HIGH(v2));
        } else {
            tcg_gen_neg_i32(s, t0, t0);

            tcg_gen_and_i32(s, t1, TCGV_LOW(v1), t0);
            tcg_gen_andc_i32(s, TCGV_LOW(ret), TCGV_LOW(v2), t0);
            tcg_gen_or_i32(s, TCGV_LOW(ret), TCGV_LOW(ret), t1);

            tcg_gen_and_i32(s, t1, TCGV_HIGH(v1), t0);
            tcg_gen_andc_i32(s, TCGV_HIGH(ret), TCGV_HIGH(v2), t0);
            tcg_gen_or_i32(s, TCGV_HIGH(ret), TCGV_HIGH(ret), t1);
        }
        tcg_temp_free_i32(s, t0);
        tcg_temp_free_i32(s, t1);
    } else if (TCG_TARGET_HAS_movcond_i64) {
        tcg_gen_op6i_i64(s, INDEX_op_movcond_i64, ret, c1, c2, v1, v2, cond);
    } else {
        TCGv_i64 t0 = tcg_temp_new_i64(s);
        TCGv_i64 t1 = tcg_temp_new_i64(s);
        tcg_gen_setcond_i64(s, cond, t0, c1, c2);
        tcg_gen_neg_i64(s, t0, t0);
        tcg_gen_and_i64(s, t1, v1, t0);
        tcg_gen_andc_i64(s, ret, v2, t0);
        tcg_gen_or_i64(s, ret, ret, t1);
        tcg_temp_free_i64(s, t0);
        tcg_temp_free_i64(s, t1);
    }
}

void tcg_gen_add2_i64(TCGContext *s, TCGv_i64 rl, TCGv_i64 rh, TCGv_i64 al,
                      TCGv_i64 ah, TCGv_i64 bl, TCGv_i64 bh)
{
    if (TCG_TARGET_HAS_add2_i64) {
        tcg_gen_op6_i64(s, INDEX_op_add2_i64, rl, rh, al, ah, bl, bh);
    } else {
        TCGv_i64 t0 = tcg_temp_new_i64(s);
        TCGv_i64 t1 = tcg_temp_new_i64(s);
        tcg_gen_add_i64(s, t0, al, bl);
        tcg_gen_setcond_i64(s, TCG_COND_LTU, t1, t0, al);
        tcg_gen_add_i64(s, rh, ah, bh);
        tcg_gen_add_i64(s, rh, rh, t1);
        tcg_gen_mov_i64(s, rl, t0);
        tcg_temp_free_i64(s, t0);
        tcg_temp_free_i64(s, t1);
    }
}

void tcg_gen_sub2_i64(TCGContext *s, TCGv_i64 rl, TCGv_i64 rh, TCGv_i64 al,
                      TCGv_i64 ah, TCGv_i64 bl, TCGv_i64 bh)
{
    if (TCG_TARGET_HAS_sub2_i64) {
        tcg_gen_op6_i64(s, INDEX_op_sub2_i64, rl, rh, al, ah, bl, bh);
    } else {
        TCGv_i64 t0 = tcg_temp_new_i64(s);
        TCGv_i64 t1 = tcg_temp_new_i64(s);
        tcg_gen_sub_i64(s, t0, al, bl);
        tcg_gen_setcond_i64(s, TCG_COND_LTU, t1, al, bl);
        tcg_gen_sub_i64(s, rh, ah, bh);
        tcg_gen_sub_i64(s, rh, rh, t1);
        tcg_gen_mov_i64(s, rl, t0);
        tcg_temp_free_i64(s, t0);
        tcg_temp_free_i64(s, t1);
    }
}

void tcg_gen_mulu2_i64(TCGContext *s, TCGv_i64 rl, TCGv_i64 rh, TCGv_i64 arg1, TCGv_i64 arg2)
{
    if (TCG_TARGET_HAS_mulu2_i64) {
        tcg_gen_op4_i64(s, INDEX_op_mulu2_i64, rl, rh, arg1, arg2);
    } else if (TCG_TARGET_HAS_muluh_i64) {
        TCGv_i64 t = tcg_temp_new_i64(s);
        tcg_gen_op3_i64(s, INDEX_op_mul_i64, t, arg1, arg2);
        tcg_gen_op3_i64(s, INDEX_op_muluh_i64, rh, arg1, arg2);
        tcg_gen_mov_i64(s, rl, t);
        tcg_temp_free_i64(s, t);
    } else {
        TCGv_i64 t0 = tcg_temp_new_i64(s);
        tcg_gen_mul_i64(s, t0, arg1, arg2);
        gen_helper_muluh_i64(s, rh, arg1, arg2);
        tcg_gen_mov_i64(s, rl, t0);
        tcg_temp_free_i64(s, t0);
    }
}

void tcg_gen_muls2_i64(TCGContext *s, TCGv_i64 rl, TCGv_i64 rh, TCGv_i64 arg1, TCGv_i64 arg2)
{
    if (TCG_TARGET_HAS_muls2_i64) {
        tcg_gen_op4_i64(s, INDEX_op_muls2_i64, rl, rh, arg1, arg2);
    } else if (TCG_TARGET_HAS_mulsh_i64) {
        TCGv_i64 t = tcg_temp_new_i64(s);
        tcg_gen_op3_i64(s, INDEX_op_mul_i64, t, arg1, arg2);
        tcg_gen_op3_i64(s, INDEX_op_mulsh_i64, rh, arg1, arg2);
        tcg_gen_mov_i64(s, rl, t);
        tcg_temp_free_i64(s, t);
    } else if (TCG_TARGET_HAS_mulu2_i64 || TCG_TARGET_HAS_muluh_i64) {
        TCGv_i64 t0 = tcg_temp_new_i64(s);
        TCGv_i64 t1 = tcg_temp_new_i64(s);
        TCGv_i64 t2 = tcg_temp_new_i64(s);
        TCGv_i64 t3 = tcg_temp_new_i64(s);
        tcg_gen_mulu2_i64(s, t0, t1, arg1, arg2);
        /* Adjust for negative inputs.  */
        tcg_gen_sari_i64(s, t2, arg1, 63);
        tcg_gen_sari_i64(s, t3, arg2, 63);
        tcg_gen_and_i64(s, t2, t2, arg2);
        tcg_gen_and_i64(s, t3, t3, arg1);
        tcg_gen_sub_i64(s, rh, t1, t2);
        tcg_gen_sub_i64(s, rh, rh, t3);
        tcg_gen_mov_i64(s, rl, t0);
        tcg_temp_free_i64(s, t0);
        tcg_temp_free_i64(s, t1);
        tcg_temp_free_i64(s, t2);
        tcg_temp_free_i64(s, t3);
    } else {
        TCGv_i64 t0 = tcg_temp_new_i64(s);
        tcg_gen_mul_i64(s, t0, arg1, arg2);
        gen_helper_mulsh_i64(s, rh, arg1, arg2);
        tcg_gen_mov_i64(s, rl, t0);
        tcg_temp_free_i64(s, t0);
    }
}

void tcg_gen_mulsu2_i64(TCGContext *s, TCGv_i64 rl, TCGv_i64 rh, TCGv_i64 arg1, TCGv_i64 arg2)
{
    TCGv_i64 t0 = tcg_temp_new_i64(s);
    TCGv_i64 t1 = tcg_temp_new_i64(s);
    TCGv_i64 t2 = tcg_temp_new_i64(s);
    tcg_gen_mulu2_i64(s, t0, t1, arg1, arg2);
    /* Adjust for negative input for the signed arg1.  */
    tcg_gen_sari_i64(s, t2, arg1, 63);
    tcg_gen_and_i64(s, t2, t2, arg2);
    tcg_gen_sub_i64(s, rh, t1, t2);
    tcg_gen_mov_i64(s, rl, t0);
    tcg_temp_free_i64(s, t0);
    tcg_temp_free_i64(s, t1);
    tcg_temp_free_i64(s, t2);
}

/* Size changing operations.  */

void tcg_gen_extrl_i64_i32(TCGContext *s, TCGv_i32 ret, TCGv_i64 arg)
{
    if (TCG_TARGET_REG_BITS == 32) {
        tcg_gen_mov_i32(s, ret, TCGV_LOW(arg));
    } else if (TCG_TARGET_HAS_extrl_i64_i32) {
        tcg_gen_op2(s, INDEX_op_extrl_i64_i32,
                    GET_TCGV_I32(ret), GET_TCGV_I64(arg));
    } else {
        tcg_gen_mov_i32(s, ret, MAKE_TCGV_I32(GET_TCGV_I64(arg)));
    }
}

void tcg_gen_extrh_i64_i32(TCGContext *s, TCGv_i32 ret, TCGv_i64 arg)
{
    if (TCG_TARGET_REG_BITS == 32) {
        tcg_gen_mov_i32(s, ret, TCGV_HIGH(arg));
    } else if (TCG_TARGET_HAS_extrh_i64_i32) {
        tcg_gen_op2(s, INDEX_op_extrh_i64_i32,
                    GET_TCGV_I32(ret), GET_TCGV_I64(arg));
    } else {
         TCGv_i64 t = tcg_temp_new_i64(s);
         tcg_gen_shri_i64(s, t, arg, 32);
         tcg_gen_mov_i32(s, ret, MAKE_TCGV_I32(GET_TCGV_I64(t)));
         tcg_temp_free_i64(s, t);

    }
}

void tcg_gen_extu_i32_i64(TCGContext *s, TCGv_i64 ret, TCGv_i32 arg)
{
    if (TCG_TARGET_REG_BITS == 32) {
        tcg_gen_mov_i32(s, TCGV_LOW(ret), arg);
        tcg_gen_movi_i32(s, TCGV_HIGH(ret), 0);
    } else {
        tcg_gen_op2(s, INDEX_op_extu_i32_i64,
                    GET_TCGV_I64(ret), GET_TCGV_I32(arg));
    }
}

void tcg_gen_ext_i32_i64(TCGContext *s, TCGv_i64 ret, TCGv_i32 arg)
{
    if (TCG_TARGET_REG_BITS == 32) {
        tcg_gen_mov_i32(s, TCGV_LOW(ret), arg);
        tcg_gen_sari_i32(s, TCGV_HIGH(ret), TCGV_LOW(ret), 31);
    } else {
        tcg_gen_op2(s, INDEX_op_ext_i32_i64,
                    GET_TCGV_I64(ret), GET_TCGV_I32(arg));
    }
}

void tcg_gen_concat_i32_i64(TCGContext *s, TCGv_i64 dest, TCGv_i32 low, TCGv_i32 high)
{
    TCGv_i64 tmp;

    if (TCG_TARGET_REG_BITS == 32) {
        tcg_gen_mov_i32(s, TCGV_LOW(dest), low);
        tcg_gen_mov_i32(s, TCGV_HIGH(dest), high);
        return;
    }

    tmp = tcg_temp_new_i64(s);
    /* These extensions are only needed for type correctness.
       We may be able to do better given target specific information.  */
    tcg_gen_extu_i32_i64(s, tmp, high);
    tcg_gen_extu_i32_i64(s, dest, low);
    /* If deposit is available, use it.  Otherwise use the extra
       knowledge that we have of the zero-extensions above.  */
    if (TCG_TARGET_HAS_deposit_i64 && TCG_TARGET_deposit_i64_valid(32, 32)) {
        tcg_gen_deposit_i64(s, dest, dest, tmp, 32, 32);
    } else {
        tcg_gen_shli_i64(s, tmp, tmp, 32);
        tcg_gen_or_i64(s, dest, dest, tmp);
    }
    tcg_temp_free_i64(s, tmp);
}

void tcg_gen_extr_i64_i32(TCGContext *s, TCGv_i32 lo, TCGv_i32 hi, TCGv_i64 arg)
{
    if (TCG_TARGET_REG_BITS == 32) {
        tcg_gen_mov_i32(s, lo, TCGV_LOW(arg));
        tcg_gen_mov_i32(s, hi, TCGV_HIGH(arg));
    } else {
        tcg_gen_extrl_i64_i32(s, lo, arg);
        tcg_gen_extrh_i64_i32(s, hi, arg);
    }
}

void tcg_gen_extr32_i64(TCGContext *s, TCGv_i64 lo, TCGv_i64 hi, TCGv_i64 arg)
{
    tcg_gen_ext32u_i64(s, lo, arg);
    tcg_gen_shri_i64(s, hi, arg, 32);
}

/* QEMU specific operations.  */

void tcg_gen_goto_tb(TCGContext *s, unsigned idx)
{
    /* We only support two chained exits.  */
    tcg_debug_assert(idx <= 1);
#ifdef CONFIG_DEBUG_TCG
    /* Verify that we havn't seen this numbered exit before.  */
    tcg_debug_assert((s->goto_tb_issue_mask & (1 << idx)) == 0);
    s->goto_tb_issue_mask |= 1 << idx;
#endif
    tcg_gen_op1i(s, INDEX_op_goto_tb, idx);
}

static inline TCGMemOp tcg_canonicalize_memop(TCGMemOp op, bool is64, bool st)
{
    /* Trigger the asserts within as early as possible.  */
    (void)get_alignment_bits(op);

    switch (op & MO_SIZE) {
    case MO_8:
        op &= ~MO_BSWAP;
        break;
    case MO_16:
        break;
    case MO_32:
        if (!is64) {
            op &= ~MO_SIGN;
        }
        break;
    case MO_64:
        if (!is64) {
            tcg_abort();
        }
        break;
    }
    if (st) {
        op &= ~MO_SIGN;
    }
    return op;
}

static void gen_ldst_i32(TCGContext *s, TCGOpcode opc, TCGv_i32 val, TCGv addr,
                         TCGMemOp memop, TCGArg idx)
{
    TCGMemOpIdx oi = make_memop_idx(memop, idx);
#if TARGET_LONG_BITS == 32
    tcg_gen_op3i_i32(s, opc, val, addr, oi);
#else
    if (TCG_TARGET_REG_BITS == 32) {
        tcg_gen_op4i_i32(s, opc, val, TCGV_LOW(addr), TCGV_HIGH(addr), oi);
    } else {
        tcg_gen_op3(s, opc, GET_TCGV_I32(val), GET_TCGV_I64(addr), oi);
    }
#endif
}

static void gen_ldst_i64(TCGContext *s, TCGOpcode opc, TCGv_i64 val, TCGv addr,
                         TCGMemOp memop, TCGArg idx)
{
    TCGMemOpIdx oi = make_memop_idx(memop, idx);
#if TARGET_LONG_BITS == 32
    if (TCG_TARGET_REG_BITS == 32) {
        tcg_gen_op4i_i32(s, opc, TCGV_LOW(val), TCGV_HIGH(val), addr, oi);
    } else {
        tcg_gen_op3(s, opc, GET_TCGV_I64(val), GET_TCGV_I32(addr), oi);
    }
#else
    if (TCG_TARGET_REG_BITS == 32) {
        tcg_gen_op5i_i32(s, opc, TCGV_LOW(val), TCGV_HIGH(val),
                         TCGV_LOW(addr), TCGV_HIGH(addr), oi);
    } else {
        tcg_gen_op3i_i64(s, opc, val, addr, oi);
    }
#endif
}

// Unicorn engine
// check if the last memory access was invalid
// if so, we jump to the block epilogue to quit immediately.
void check_exit_request(TCGContext *tcg_ctx)
{
    TCGv_i32 flag;

    flag = tcg_temp_new_i32(tcg_ctx);
    tcg_gen_ld_i32(tcg_ctx, flag, tcg_ctx->cpu_env,
            offsetof(CPUState, tcg_exit_req) - ENV_OFFSET);
    tcg_gen_brcondi_i32(tcg_ctx, TCG_COND_NE, flag, 0, tcg_ctx->exitreq_label);
    tcg_temp_free_i32(tcg_ctx, flag);
}

void tcg_gen_qemu_ld_i32(struct uc_struct *uc, TCGv_i32 val, TCGv addr, TCGArg idx, TCGMemOp memop)
{
    TCGContext *tcg_ctx = uc->tcg_ctx;

    memop = tcg_canonicalize_memop(memop, 0, 0);
    gen_ldst_i32(tcg_ctx, INDEX_op_qemu_ld_i32, val, addr, memop, idx);
    check_exit_request(tcg_ctx);
}

void tcg_gen_qemu_st_i32(struct uc_struct *uc, TCGv_i32 val, TCGv addr, TCGArg idx, TCGMemOp memop)
{
    TCGContext *tcg_ctx = uc->tcg_ctx;

    memop = tcg_canonicalize_memop(memop, 0, 1);
    gen_ldst_i32(tcg_ctx, INDEX_op_qemu_st_i32, val, addr, memop, idx);
    check_exit_request(tcg_ctx);
}

void tcg_gen_qemu_ld_i64(struct uc_struct *uc, TCGv_i64 val, TCGv addr, TCGArg idx, TCGMemOp memop)
{
    TCGContext *tcg_ctx = uc->tcg_ctx;

    if (TCG_TARGET_REG_BITS == 32 && (memop & MO_SIZE) < MO_64) {
        tcg_gen_qemu_ld_i32(uc, TCGV_LOW(val), addr, idx, memop);
        if (memop & MO_SIGN) {
            tcg_gen_sari_i32(tcg_ctx, TCGV_HIGH(val), TCGV_LOW(val), 31);
        } else {
            tcg_gen_movi_i32(tcg_ctx, TCGV_HIGH(val), 0);
        }

        check_exit_request(tcg_ctx);
        return;
    }

    memop = tcg_canonicalize_memop(memop, 1, 0);
    gen_ldst_i64(tcg_ctx, INDEX_op_qemu_ld_i64, val, addr, memop, idx);
    check_exit_request(tcg_ctx);
}

void tcg_gen_qemu_st_i64(struct uc_struct *uc, TCGv_i64 val, TCGv addr, TCGArg idx, TCGMemOp memop)
{
    TCGContext *tcg_ctx = uc->tcg_ctx;

    if (TCG_TARGET_REG_BITS == 32 && (memop & MO_SIZE) < MO_64) {
        tcg_gen_qemu_st_i32(uc, TCGV_LOW(val), addr, idx, memop);
        check_exit_request(tcg_ctx);
        return;
    }

    memop = tcg_canonicalize_memop(memop, 1, 1);
    gen_ldst_i64(tcg_ctx, INDEX_op_qemu_st_i64, val, addr, memop, idx);
    check_exit_request(tcg_ctx);
}

static void tcg_gen_ext_i32(TCGContext *s, TCGv_i32 ret, TCGv_i32 val, TCGMemOp opc)
{
    switch (opc & MO_SSIZE) {
    case MO_SB:
        tcg_gen_ext8s_i32(s, ret, val);
        break;
    case MO_UB:
        tcg_gen_ext8u_i32(s, ret, val);
        break;
    case MO_SW:
        tcg_gen_ext16s_i32(s, ret, val);
        break;
    case MO_UW:
        tcg_gen_ext16u_i32(s, ret, val);
        break;
    default:
        tcg_gen_mov_i32(s, ret, val);
        break;
    }
}

static void tcg_gen_ext_i64(TCGContext *s, TCGv_i64 ret, TCGv_i64 val, TCGMemOp opc)
{
    switch (opc & MO_SSIZE) {
    case MO_SB:
        tcg_gen_ext8s_i64(s, ret, val);
        break;
    case MO_UB:
        tcg_gen_ext8u_i64(s, ret, val);
        break;
    case MO_SW:
        tcg_gen_ext16s_i64(s, ret, val);
        break;
    case MO_UW:
        tcg_gen_ext16u_i64(s, ret, val);
        break;
    case MO_SL:
        tcg_gen_ext32s_i64(s, ret, val);
        break;
    case MO_UL:
        tcg_gen_ext32u_i64(s, ret, val);
        break;
    default:
        tcg_gen_mov_i64(s, ret, val);
        break;
    }
}

#ifdef CONFIG_SOFTMMU
typedef void (*gen_atomic_cx_i32)(TCGContext *,
                                  TCGv_i32, TCGv_env, TCGv,
                                  TCGv_i32, TCGv_i32, TCGv_i32);
typedef void (*gen_atomic_cx_i64)(TCGContext *,
                                  TCGv_i64, TCGv_env, TCGv,
                                  TCGv_i64, TCGv_i64, TCGv_i32);
typedef void (*gen_atomic_op_i32)(TCGContext *,
                                  TCGv_i32, TCGv_env, TCGv,
                                  TCGv_i32, TCGv_i32);
typedef void (*gen_atomic_op_i64)(TCGContext *,
                                  TCGv_i64, TCGv_env, TCGv,
                                  TCGv_i64, TCGv_i32);
#else
typedef void (*gen_atomic_cx_i32)(TCGContext *, TCGv_i32, TCGv_env, TCGv, TCGv_i32, TCGv_i32);
typedef void (*gen_atomic_cx_i64)(TCGContext *, TCGv_i64, TCGv_env, TCGv, TCGv_i64, TCGv_i64);
typedef void (*gen_atomic_op_i32)(TCGContext *, TCGv_i32, TCGv_env, TCGv, TCGv_i32);
typedef void (*gen_atomic_op_i64)(TCGContext *, TCGv_i64, TCGv_env, TCGv, TCGv_i64);
#endif

#ifdef CONFIG_ATOMIC64
# define WITH_ATOMIC64(X) X,
#else
# define WITH_ATOMIC64(X) NULL,
#endif

#ifdef HOST_WORDS_BIGENDIAN
static void * const table_cmpxchg[16] = {
    gen_helper_atomic_cmpxchgb,
    gen_helper_atomic_cmpxchgw_be,
    gen_helper_atomic_cmpxchgl_be,
    WITH_ATOMIC64(gen_helper_atomic_cmpxchgq_be)
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    gen_helper_atomic_cmpxchgw_le,
    gen_helper_atomic_cmpxchgl_le,
    WITH_ATOMIC64(gen_helper_atomic_cmpxchgq_le)
}
#else
static void * const table_cmpxchg[16] = {
    gen_helper_atomic_cmpxchgb,
    gen_helper_atomic_cmpxchgw_le,
    gen_helper_atomic_cmpxchgl_le,
    WITH_ATOMIC64(gen_helper_atomic_cmpxchgq_le)
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    gen_helper_atomic_cmpxchgw_be,
    gen_helper_atomic_cmpxchgl_be,
    WITH_ATOMIC64(gen_helper_atomic_cmpxchgq_be)
};
#endif

void tcg_gen_atomic_cmpxchg_i32(TCGContext *s,
                                TCGv_i32 retv, TCGv addr, TCGv_i32 cmpv,
                                TCGv_i32 newv, TCGArg idx, TCGMemOp memop)
{
    memop = tcg_canonicalize_memop(memop, 0, 0);

    if (!s->uc->parallel_cpus) {
        TCGv_i32 t1 = tcg_temp_new_i32(s);
        TCGv_i32 t2 = tcg_temp_new_i32(s);

        tcg_gen_ext_i32(s, t2, cmpv, memop & MO_SIZE);

        tcg_gen_qemu_ld_i32(s->uc, t1, addr, idx, memop & ~MO_SIGN);
        tcg_gen_movcond_i32(s, TCG_COND_EQ, t2, t1, t2, newv, t1);
        tcg_gen_qemu_st_i32(s->uc, t2, addr, idx, memop);
        tcg_temp_free_i32(s, t2);

        if (memop & MO_SIGN) {
            tcg_gen_ext_i32(s, retv, t1, memop);
        } else {
            tcg_gen_mov_i32(s, retv, t1);
        }
        tcg_temp_free_i32(s, t1);
    } else {
        gen_atomic_cx_i32 gen;

        gen = table_cmpxchg[memop & (MO_SIZE | MO_BSWAP)];
        tcg_debug_assert(gen != NULL);

#ifdef CONFIG_SOFTMMU
        {
            TCGv_i32 oi = tcg_const_i32(s, make_memop_idx(memop & ~MO_SIGN, idx));
            gen(s, retv, s->tcg_env, addr, cmpv, newv, oi);
            tcg_temp_free_i32(s, oi);
        }
#else
        gen(s, retv, s->tcg_env, addr, cmpv, newv);
#endif

        if (memop & MO_SIGN) {
            tcg_gen_ext_i32(s, retv, retv, memop);
        }
    }
}

void tcg_gen_atomic_cmpxchg_i64(TCGContext *s,
                                TCGv_i64 retv, TCGv addr, TCGv_i64 cmpv,
                                TCGv_i64 newv, TCGArg idx, TCGMemOp memop)
{
    memop = tcg_canonicalize_memop(memop, 1, 0);

    if (!s->uc->parallel_cpus) {
        TCGv_i64 t1 = tcg_temp_new_i64(s);
        TCGv_i64 t2 = tcg_temp_new_i64(s);

        tcg_gen_ext_i64(s, t2, cmpv, memop & MO_SIZE);

        tcg_gen_qemu_ld_i64(s->uc, t1, addr, idx, memop & ~MO_SIGN);
        tcg_gen_movcond_i64(s, TCG_COND_EQ, t2, t1, t2, newv, t1);
        tcg_gen_qemu_st_i64(s->uc, t2, addr, idx, memop);
        tcg_temp_free_i64(s, t2);

        if (memop & MO_SIGN) {
            tcg_gen_ext_i64(s, retv, t1, memop);
        } else {
            tcg_gen_mov_i64(s, retv, t1);
        }
        tcg_temp_free_i64(s, t1);
    } else if ((memop & MO_SIZE) == MO_64) {
#ifdef CONFIG_ATOMIC64
        gen_atomic_cx_i64 gen;

        gen = table_cmpxchg[memop & (MO_SIZE | MO_BSWAP)];
        tcg_debug_assert(gen != NULL);

#ifdef CONFIG_SOFTMMU
        {
            TCGv_i32 oi = tcg_const_i32(s, make_memop_idx(memop, idx));
            gen(s, retv, s->tcg_env, addr, cmpv, newv, oi);
            tcg_temp_free_i32(s, oi);
        }
#else
        gen(s, retv, s->tcg_env, addr, cmpv, newv);
#endif
#else
        gen_helper_exit_atomic(s, s->tcg_env);
#endif /* CONFIG_ATOMIC64 */
    } else {
        TCGv_i32 c32 = tcg_temp_new_i32(s);
        TCGv_i32 n32 = tcg_temp_new_i32(s);
        TCGv_i32 r32 = tcg_temp_new_i32(s);

        tcg_gen_extrl_i64_i32(s, c32, cmpv);
        tcg_gen_extrl_i64_i32(s, n32, newv);
        tcg_gen_atomic_cmpxchg_i32(s, r32, addr, c32, n32, idx, memop & ~MO_SIGN);
        tcg_temp_free_i32(s, c32);
        tcg_temp_free_i32(s, n32);

        tcg_gen_extu_i32_i64(s, retv, r32);
        tcg_temp_free_i32(s, r32);

        if (memop & MO_SIGN) {
            tcg_gen_ext_i64(s, retv, retv, memop);
        }
    }
}

static void do_nonatomic_op_i32(TCGContext *s,
                                TCGv_i32 ret, TCGv addr, TCGv_i32 val,
                                TCGArg idx, TCGMemOp memop, bool new_val,
                                void (*gen)(TCGContext *, TCGv_i32, TCGv_i32, TCGv_i32))
{
    TCGv_i32 t1 = tcg_temp_new_i32(s);
    TCGv_i32 t2 = tcg_temp_new_i32(s);

    memop = tcg_canonicalize_memop(memop, 0, 0);

    tcg_gen_qemu_ld_i32(s->uc, t1, addr, idx, memop & ~MO_SIGN);
    gen(s, t2, t1, val);
    tcg_gen_qemu_st_i32(s->uc, t2, addr, idx, memop);

    tcg_gen_ext_i32(s, ret, (new_val ? t2 : t1), memop);
    tcg_temp_free_i32(s, t1);
    tcg_temp_free_i32(s, t2);
}

static void do_atomic_op_i32(TCGContext *s,
                             TCGv_i32 ret, TCGv addr, TCGv_i32 val,
                             TCGArg idx, TCGMemOp memop, void * const table[])
{
    gen_atomic_op_i32 gen;

    memop = tcg_canonicalize_memop(memop, 0, 0);

    gen = table[memop & (MO_SIZE | MO_BSWAP)];
    tcg_debug_assert(gen != NULL);

#ifdef CONFIG_SOFTMMU
    {
        TCGv_i32 oi = tcg_const_i32(s, make_memop_idx(memop & ~MO_SIGN, idx));
        gen(s, ret, s->tcg_env, addr, val, oi);
        tcg_temp_free_i32(s, oi);
    }
#else
    gen(s, ret, s->tcg_env, addr, val);
#endif

    if (memop & MO_SIGN) {
        tcg_gen_ext_i32(s, ret, ret, memop);
    }
}

static void do_nonatomic_op_i64(TCGContext *s,
                                TCGv_i64 ret, TCGv addr, TCGv_i64 val,
                                TCGArg idx, TCGMemOp memop, bool new_val,
                                void (*gen)(TCGContext *, TCGv_i64, TCGv_i64, TCGv_i64))
{
    TCGv_i64 t1 = tcg_temp_new_i64(s);
    TCGv_i64 t2 = tcg_temp_new_i64(s);

    memop = tcg_canonicalize_memop(memop, 1, 0);

    tcg_gen_qemu_ld_i64(s->uc, t1, addr, idx, memop & ~MO_SIGN);
    gen(s, t2, t1, val);
    tcg_gen_qemu_st_i64(s->uc, t2, addr, idx, memop);

    tcg_gen_ext_i64(s, ret, (new_val ? t2 : t1), memop);
    tcg_temp_free_i64(s, t1);
    tcg_temp_free_i64(s, t2);
}

static void do_atomic_op_i64(TCGContext *s,
                             TCGv_i64 ret, TCGv addr, TCGv_i64 val,
                             TCGArg idx, TCGMemOp memop, void * const table[])
{
    memop = tcg_canonicalize_memop(memop, 1, 0);

    if ((memop & MO_SIZE) == MO_64) {
#ifdef CONFIG_ATOMIC64
        gen_atomic_op_i64 gen;

        gen = table[memop & (MO_SIZE | MO_BSWAP)];
        tcg_debug_assert(gen != NULL);

#ifdef CONFIG_SOFTMMU
        {
            TCGv_i32 oi = tcg_const_i32(s, make_memop_idx(memop & ~MO_SIGN, idx));
            gen(s, ret, s->tcg_env, addr, val, oi);
            tcg_temp_free_i32(s, oi);
        }
#else
        gen(s, ret, s->tcg_env, addr, val);
#endif
#else
        gen_helper_exit_atomic(s, s->tcg_env);
#endif /* CONFIG_ATOMIC64 */
    } else {
        TCGv_i32 v32 = tcg_temp_new_i32(s);
        TCGv_i32 r32 = tcg_temp_new_i32(s);

        tcg_gen_extrl_i64_i32(s, v32, val);
        do_atomic_op_i32(s, r32, addr, v32, idx, memop & ~MO_SIGN, table);
        tcg_temp_free_i32(s, v32);

        tcg_gen_extu_i32_i64(s, ret, r32);
        tcg_temp_free_i32(s, r32);

        if (memop & MO_SIGN) {
            tcg_gen_ext_i64(s, ret, ret, memop);
        }
    }
}

#ifdef HOST_WORDS_BIGENDIAN
#define GEN_ATOMIC_TABLE(NAME) \
static void * const table_##NAME[16] = {    \
    gen_helper_atomic_##NAME##b,            \
    gen_helper_atomic_##NAME##w_be,         \
    gen_helper_atomic_##NAME##l_be,         \
    gen_helper_atomic_##NAME##q_be,         \
    NULL,                                   \
    NULL,                                   \
    NULL,                                   \
    NULL,                                   \
    NULL,                                   \
    gen_helper_atomic_##NAME##w_le,         \
    gen_helper_atomic_##NAME##l_le,         \
    gen_helper_atomic_##NAME##q_le,         \
};
#else
#define GEN_ATOMIC_TABLE(NAME) \
static void * const table_##NAME[16] = {    \
    gen_helper_atomic_##NAME##b,            \
    gen_helper_atomic_##NAME##w_le,         \
    gen_helper_atomic_##NAME##w_be,         \
    gen_helper_atomic_##NAME##l_le,         \
    NULL,                                   \
    NULL,                                   \
    NULL,                                   \
    NULL,                                   \
    NULL,                                   \
    gen_helper_atomic_##NAME##l_be,         \
    gen_helper_atomic_##NAME##q_le,         \
    gen_helper_atomic_##NAME##q_be,         \
};
#endif

#define GEN_ATOMIC_HELPER(NAME, OP, NEW)                                \
GEN_ATOMIC_TABLE(NAME)                                                  \
void tcg_gen_atomic_##NAME##_i32                                        \
    (TCGContext *s, TCGv_i32 ret, TCGv addr, TCGv_i32 val, TCGArg idx, TCGMemOp memop) \
{                                                                       \
    if (s->uc->parallel_cpus) {                                         \
        do_atomic_op_i32(s, ret, addr, val, idx, memop, table_##NAME);  \
    } else {                                                            \
        do_nonatomic_op_i32(s, ret, addr, val, idx, memop, NEW,         \
                            tcg_gen_##OP##_i32);                        \
    }                                                                   \
}                                                                       \
void tcg_gen_atomic_##NAME##_i64                                        \
    (TCGContext *s, TCGv_i64 ret, TCGv addr, TCGv_i64 val, TCGArg idx, TCGMemOp memop) \
{                                                                       \
    if (s->uc->parallel_cpus) {                                         \
        do_atomic_op_i64(s, ret, addr, val, idx, memop, table_##NAME);  \
    } else {                                                            \
        do_nonatomic_op_i64(s, ret, addr, val, idx, memop, NEW,         \
                            tcg_gen_##OP##_i64);                        \
    }                                                                   \
}

GEN_ATOMIC_HELPER(fetch_add, add, 0)
GEN_ATOMIC_HELPER(fetch_and, and, 0)
GEN_ATOMIC_HELPER(fetch_or, or, 0)
GEN_ATOMIC_HELPER(fetch_xor, xor, 0)

GEN_ATOMIC_HELPER(add_fetch, add, 1)
GEN_ATOMIC_HELPER(and_fetch, and, 1)
GEN_ATOMIC_HELPER(or_fetch, or, 1)
GEN_ATOMIC_HELPER(xor_fetch, xor, 1)

static void tcg_gen_mov2_i32(TCGContext *s, TCGv_i32 r, TCGv_i32 a, TCGv_i32 b)
{
    tcg_gen_mov_i32(s, r, b);
}

static void tcg_gen_mov2_i64(TCGContext *s, TCGv_i64 r, TCGv_i64 a, TCGv_i64 b)
{
    tcg_gen_mov_i64(s, r, b);
}

GEN_ATOMIC_HELPER(xchg, mov2, 0)

#undef GEN_ATOMIC_TABLE

#undef GEN_ATOMIC_HELPER
