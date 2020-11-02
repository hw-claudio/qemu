/*
 * x86 HVF CPU type initialization
 *
 * Copyright 2020 SUSE LLC
 *
 * This work is licensed under the terms of the GNU GPL, version 2 or later.
 * See the COPYING file in the top-level directory.
 */

/* this information overloads the TYPE_X86_CPU type in cpu.c */

static void hvf_cpu_common_class_init(ObjectClass *oc, void *data)
{
    X86CPUClass *xcc = X86_CPU_CLASS(oc);
    DeviceClass *dc = DEVICE_CLASS(oc);

    x86_cpu_common_class_init(oc, data);

    device_class_set_parent_realize(dc, host_cpu_realizefn,
                                    &xcc->parent_realize);
}

static void hvf_cpu_initfn(Object *obj)
{
    X86CPU *cpu = X86_CPU(obj);

    host_cpu_initfn(obj);

    /* Special cases not set in the X86CPUDefinition structs: */
    /* TODO: in-kernel irqchip for hvf */
}

static const TypeInfo hvf_cpu_type_info = {
    .name = TYPE_X86_CPU,
    .class_init = hvf_cpu_common_class_init,
    .instance_init = hvf_cpu_initfn,
};

static void hvf_cpu_max_initfn(Object *obj)
{
    X86CPU *cpu = X86_CPU(obj);
    CPUX86State *env = &cpu->env;

    host_cpu_max_initfn(cpu);

    env->cpuid_min_level =
        hvf_get_supported_cpuid(0x0, 0, R_EAX);
    env->cpuid_min_xlevel =
        hvf_get_supported_cpuid(0x80000000, 0, R_EAX);
    env->cpuid_min_xlevel2 =
        hvf_get_supported_cpuid(0xC0000000, 0, R_EAX);
}

static const TypeInfo kvm_cpu_max_type_info = {
    .name = X86_CPU_TYPE_NAME("max"),
    .instance_init = kvm_cpu_max_initfn,
};

void hvf_cpu_type_init()
{
    type_overload_init(&hvf_cpu_type_info);
    type_overload_init(&hvf_cpu_max_type_info);
} 
 
