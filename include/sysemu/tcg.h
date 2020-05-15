/*
 * QEMU TCG support
 *
 * This work is licensed under the terms of the GNU GPL, version 2 or later.
 * See the COPYING file in the top-level directory.
 */

#ifndef SYSEMU_TCG_H
#define SYSEMU_TCG_H

#include "sysemu/accel.h"

void tcg_exec_init(unsigned long tb_size);
#define tcg_enabled() (accel_allowed(ACCEL_TYPE_TCG))

#endif /* SYSEMU_TCG_H */
