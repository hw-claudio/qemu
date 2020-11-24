/*
 * CPUS module (softmmu/cpus.c) Accelerator Ops
 *
 * Copyright 2020 SUSE LLC
 *
 * This work is licensed under the terms of the GNU GPL, version 2 or later.
 * See the COPYING file in the top-level directory.
 *
 */

#ifndef QEMU_CPUS_H
#define QEMU_CPUS_H

#include "qemu/timer.h"
#include "qom/object.h"

/* accel/dummy-cpus.c */

/* Create a dummy vcpu for CpusAccelOps->create_vcpu_thread */
void dummy_start_vcpu_thread(CPUState *);

/* cpus.c */

#define TYPE_CPUS_ACCEL_OPS "accel-ops"
#define CPUS_ACCEL_TYPE_NAME(name) (name "-" TYPE_CPUS_ACCEL_OPS)

typedef struct CpusAccelOps CpusAccelOps;
DECLARE_CLASS_CHECKERS(CpusAccelOps, CPUS_ACCEL_OPS, TYPE_CPUS_ACCEL_OPS)

struct CpusAccelOps {
    ObjectClass parent_class;

    /* initialization function called when accel is chosen */
    void (*accel_chosen_init)(CpusAccelOps *ops);

    void (*create_vcpu_thread)(CPUState *cpu); /* MANDATORY NON-NULL */
    void (*kick_vcpu_thread)(CPUState *cpu);

    void (*synchronize_post_reset)(CPUState *cpu);
    void (*synchronize_post_init)(CPUState *cpu);
    void (*synchronize_state)(CPUState *cpu);
    void (*synchronize_pre_loadvm)(CPUState *cpu);

    void (*handle_interrupt)(CPUState *cpu, int mask);

    int64_t (*get_virtual_clock)(void);
    int64_t (*get_elapsed_ticks)(void);
};

/* interface available for cpus accelerator threads */

/* For temporary buffers for forming a name */
#define VCPU_THREAD_NAME_SIZE 16

void cpus_kick_thread(CPUState *cpu);
bool cpu_work_list_empty(CPUState *cpu);
bool cpu_thread_is_idle(CPUState *cpu);
bool all_cpu_threads_idle(void);
bool cpu_can_run(CPUState *cpu);
void qemu_wait_io_event_common(CPUState *cpu);
void qemu_wait_io_event(CPUState *cpu);
void cpu_thread_signal_created(CPUState *cpu);
void cpu_thread_signal_destroyed(CPUState *cpu);
void cpu_handle_guest_debug(CPUState *cpu);

/* end interface for cpus accelerator threads */

bool qemu_in_vcpu_thread(void);
void qemu_init_cpu_loop(void);
void resume_all_vcpus(void);
void pause_all_vcpus(void);
void cpu_stop_current(void);

extern int icount_align_option;

/* Unblock cpu */
void qemu_cpu_kick_self(void);

void cpu_synchronize_all_states(void);
void cpu_synchronize_all_post_reset(void);
void cpu_synchronize_all_post_init(void);
void cpu_synchronize_all_pre_loadvm(void);

#ifndef CONFIG_USER_ONLY
/* vl.c */
/* *-user doesn't have configurable SMP topology */
extern int smp_cores;
extern int smp_threads;
#endif

void list_cpus(const char *optarg);

#endif
