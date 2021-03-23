/*
 * QEMU ARM CPU
 *
 * Copyright (c) 2012 SUSE LINUX Products GmbH
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see
 * <http://www.gnu.org/licenses/gpl-2.0.html>
 */
#ifndef ARM_TCG_CPU_H
#define ARM_TCG_CPU_H

#include "cpu.h"
#include "hw/core/tcg-cpu-ops.h"
#include "hw/core/accel-cpu.h"

void arm_cpu_synchronize_from_tb(CPUState *cs,
                                 const TranslationBlock *tb);

void tcg_arm_init_accel_cpu(AccelCPUClass *accel_cpu, CPUClass *cc);

#ifndef CONFIG_USER_ONLY
/* Do semihosting call and set the appropriate return value. */
void tcg_handle_semihosting(CPUState *cs);
bool tcg_cpu_realizefn(CPUState *cs, Error **errp);
bool tcg_cpu_realize_gt_timers(CPUState *cs, Error **errp);

#endif /* !CONFIG_USER_ONLY */

#endif /* ARM_TCG_CPU_H */