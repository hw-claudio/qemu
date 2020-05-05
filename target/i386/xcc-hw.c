/*
 * HW accelerator methods for X86CPUClass
 *
 *  Copyright (c) 2020 SUSE LINUX LLC
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
#include "xcc-hw.h"

uint32_t xcc_hw_compute_eflags(CPUX86State *env)
{
    return env->eflags;
}
void xcc_hw_set_mxcsr(CPUX86State *env, uint32_t mxcsr)
{
    env->mxcsr = mxcsr;
}
void xcc_hw_set_fpuc(CPUX86State *env, uint16_t fpuc)
{
     env->fpuc = fpuc;
}
void xcc_hw_report_tpr_access(CPUX86State *env, TPRAccess access)
{
    CPUState *cs = env_cpu(env);
    env->tpr_access_type = access;
    cpu_interrupt(cs, CPU_INTERRUPT_TPR);
}
