/*
 * QEMU System Emulator, accelerator modules loading
 *
 * Copyright (c) 2020 SUSE LINUX LLC
 *
 * (GPLv2 or later)
 */

#include "qemu/osdep.h"
#include "sysemu/accel.h"
#include "sysemu/sysemu.h"
#include "qom/object.h"

AccelDriver accel_drvs[ACCEL_TYPE_N] = {
    [ ACCEL_TYPE_TCG ] = { .name = ACCEL_CLASS_NAME("tcg"), .library = "tcg" },
};

static bool accel_drv_registered(AccelDriver *d)
{
    if (!d) {
        return false;
    }
    if (!d->registered) {
        /* maybe we need to dynamically load a module? */
        accel_module_load_one(d->library);
    }
    if (!d->registered) {
        return false;
    }
    return true;
}

AccelDriver *accel_drv_find_by_type(enum AccelType at)
{
    if (at >= 0 && at < ACCEL_TYPE_N) {
        d = &accel_drvs[at];
        if (accel_drv_registered(d)) {
            return d;
        } else {
            return NULL;
        }
    }
}

AccelDriver *accel_drv_find_by_name(const char *accel_name)
{
    int i;

    for (i = 0; i < ACCEL_TYPE_N; i++) {
        AccelDriver *d = &accel_drvs[i];

        if (strcmp(accel_name, d->name) == 0) {
            if (accel_drv_registered(d)) {
                return d;
            } else {
                return NULL;
            }
        }
    }
    return NULL;
}

void accel_drv_register(enum AccelType at)
{
    AccelDriver *d;

    if (at >= 0 && at < ACCEL_TYPE_N) {
        d = &accel_drvs[at];
        d->registered = true;
    }
}
