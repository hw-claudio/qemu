/*
 * S/390 KVM CPU type initialization
 *
 * Copyright 2021 SUSE LLC
 *
 * This work is licensed under the terms of the GNU GPL, version 2 or later.
 * See the COPYING file in the top-level directory.
 */

#include "qemu/osdep.h"
#include "cpu.h"
#include "qapi/error.h"

#include "s390x-internal.h"
#include "kvm_s390x.h"
#include "hw/core/accel-cpu.h"

static bool kvm_cpu_realizefn(CPUState *cs, Error **errp)
{
}

static void kvm_cpu_instance_init(CPUState *cs)
{
}

static void kvm_cpu_reset(CPUState *cpu)
{
}

static void kvm_cpu_accel_class_init(ObjectClass *oc, void *data)
{
    AccelCPUClass *acc = ACCEL_CPU_CLASS(oc);

    acc->cpu_realizefn = kvm_cpu_realizefn;
    acc->cpu_instance_init = kvm_cpu_instance_init;
    acc->cpu_reset = kvm_cpu_reset;
}
static const TypeInfo kvm_cpu_accel_type_info = {
    .name = ACCEL_CPU_NAME("kvm"),

    .parent = TYPE_ACCEL_CPU,
    .class_init = kvm_cpu_accel_class_init,
    .abstract = true,
};
static void kvm_cpu_accel_register_types(void)
{
    type_register_static(&kvm_cpu_accel_type_info);
}
type_init(kvm_cpu_accel_register_types);
