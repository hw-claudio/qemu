/*
 * S/390 TCG accel cpu class initialization
 *
 * Copyright 2021 SUSE LLC
 *
 * This work is licensed under the terms of the GNU GPL, version 2 or later.
 * See the COPYING file in the top-level directory.
 */

#include "qemu/osdep.h"
#include "cpu.h"
#include "s390x-internal.h"
#include "qemu/accel.h"
#include "hw/core/accel-cpu.h"
#include "tcg-cpu.h"
#include "hw/core/tcg-cpu-ops.h"

static struct TCGCPUOps s390_tcg_ops = {
    .initialize = s390x_translate_init,
    .tlb_fill = s390_cpu_tlb_fill,

#if !defined(CONFIG_USER_ONLY)
    .cpu_exec_interrupt = s390_cpu_exec_interrupt,
    .do_interrupt = s390_cpu_do_interrupt,
    .debug_excp_handler = s390x_cpu_debug_excp_handler,
    .do_unaligned_access = s390x_cpu_do_unaligned_access,
#endif /* !CONFIG_USER_ONLY */
};

static void tcg_cpu_init_ops(AccelCPUClass *accel_cpu, CPUClass *cc)
{
    /* for S/390, all cpus use the same set of operations */
    cc->tcg_ops = &s390_tcg_ops;
}

static void tcg_cpu_class_init(CPUClass *cc)
{
    cc->init_accel_cpu = tcg_cpu_init_ops;
}

static void tcg_cpu_instance_init(CPUState *cs)
{
}

static bool tcg_cpu_realizefn(CPUState *cs, Error **errp)
{
    return true;
}

static void tcg_cpu_accel_class_init(ObjectClass *oc, void *data)
{
    AccelCPUClass *acc = ACCEL_CPU_CLASS(oc);

    acc->cpu_realizefn = tcg_cpu_realizefn;
    acc->cpu_class_init = tcg_cpu_class_init;
    acc->cpu_instance_init = tcg_cpu_instance_init;
}
static const TypeInfo tcg_cpu_accel_type_info = {
    .name = ACCEL_CPU_NAME("tcg"),

    .parent = TYPE_ACCEL_CPU,
    .class_init = tcg_cpu_accel_class_init,
    .abstract = true,
};
static void tcg_cpu_accel_register_types(void)
{
    type_register_static(&tcg_cpu_accel_type_info);
}
type_init(tcg_cpu_accel_register_types);
