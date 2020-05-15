/*
 * TCG accelerator methods for X86CPUClass
 *
 *  Copyright (c) 2003 Fabrice Bellard
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>.
 */


#include "qemu/osdep.h"
#include "cpu.h"
#include "xcc-tcg.h"
#include "exec/exec-all.h"

uint32_t xcc_tcg_compute_eflags(CPUX86State *env)
{
    uint32_t eflags = env->eflags;
    eflags |= cpu_cc_compute_all(env, CC_OP) | (env->df & DF_MASK);
    return eflags;
}

void xcc_tcg_set_mxcsr(CPUX86State *env, uint32_t mxcsr)
{
    env->mxcsr = mxcsr;
    update_mxcsr_status(env);
}

void xcc_tcg_set_fpuc(CPUX86State *env, uint16_t fpuc)
{
    env->fpuc = fpuc;
    update_fp_status(env);
}

void xcc_tcg_post_load(CPUX86State *env)
{
    CPUState *cs = env_cpu(env);
    target_ulong dr7;
    update_fp_status(env);
    update_mxcsr_status(env);

    cpu_breakpoint_remove_all(cs, BP_CPU);
    cpu_watchpoint_remove_all(cs, BP_CPU);

    /* Indicate all breakpoints disabled, as they are, then
       let the helper re-enable them.  */
    dr7 = env->dr[7];
    env->dr[7] = dr7 & ~(DR7_GLOBAL_BP_MASK | DR7_LOCAL_BP_MASK);
    cpu_x86_update_dr7(env, dr7);
    tlb_flush(cs);
}

#ifndef CONFIG_USER_ONLY
void xcc_tcg_report_tpr_access(CPUX86State *env, TPRAccess access)
{
    X86CPU *cpu = env_archcpu(env);
    CPUState *cs = env_cpu(env);

    cpu_restore_state(cs, cs->mem_io_pc, false);
    apic_handle_tpr_access_report(cpu->apic_state, env->eip, access);
}
#endif
