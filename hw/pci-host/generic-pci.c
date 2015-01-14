/*
 * Generic PCI host controller
 *
 * Copyright (c) 2014 Linaro, Ltd.
 * Author: Rob Herring <rob.herring@linaro.org>
 *
 * Based on ARM Versatile PCI controller (hw/pci-host/versatile.c):
 * Copyright (c) 2006-2009 CodeSourcery.
 * Written by Paul Brook
 *
 * This code is licensed under the LGPL.
 */

#include "hw/sysbus.h"
#include "hw/pci-host/generic-pci.h"
#include "exec/address-spaces.h"
#include "sysemu/device_tree.h"

static const VMStateDescription pci_generic_host_vmstate = {
    .name = "generic-host-pci",
    .version_id = 1,
    .minimum_version_id = 1,
};

static void pci_cam_config_write(void *opaque, hwaddr addr,
                                 uint64_t val, unsigned size)
{
    PCIGenState *s = opaque;
    pci_data_write(&s->pci_bus, addr, val, size);
}

static uint64_t pci_cam_config_read(void *opaque, hwaddr addr, unsigned size)
{
    PCIGenState *s = opaque;
    uint32_t val;
    val = pci_data_read(&s->pci_bus, addr, size);
    return val;
}

static const MemoryRegionOps pci_generic_config_ops = {
    .read = pci_cam_config_read,
    .write = pci_cam_config_write,
    .endianness = DEVICE_LITTLE_ENDIAN,
};

static void pci_generic_set_irq(void *opaque, int irq_num, int level)
{
    qemu_irq *pic = opaque;
    qemu_set_irq(pic[irq_num], level);
}

static void pci_generic_host_realize(DeviceState *dev, Error **errp)
{
    PCIHostState *h = PCI_HOST_BRIDGE(dev);
    PCIGenState *s = PCI_GEN(dev);
    GenericPCIHostState *gps = &s->pci_gen;
    SysBusDevice *sbd = SYS_BUS_DEVICE(dev);
    int i;

    memory_region_init(&s->pci_io_window, OBJECT(s), "pci_io", s->pio_win_size);
    memory_region_init(&s->pci_mem_space, OBJECT(s), "pci_mem", 1ULL << 32);

    pci_bus_new_inplace(&s->pci_bus, sizeof(s->pci_bus), dev, "pci",
                        &s->pci_mem_space, &s->pci_io_window,
                        PCI_DEVFN(0, 0), TYPE_PCI_BUS);
    h->bus = &s->pci_bus;

    object_initialize(gps, sizeof(*gps), TYPE_GENERIC_PCI_HOST);
    qdev_set_parent_bus(DEVICE(gps), BUS(&s->pci_bus));

    for (i = 0; i < s->irqs; i++) {
        sysbus_init_irq(sbd, &s->irq[i]);
    }

    pci_bus_irqs(&s->pci_bus, pci_generic_set_irq, pci_swizzle_map_irq_fn,
                                                         s->irq, s->irqs);
    memory_region_init_io(&s->mem_config, OBJECT(s), &pci_generic_config_ops, s,
                          "pci-config", s->cfg_win_size);
    memory_region_init_alias(&s->pci_mem_window, OBJECT(s), "pci-mem-win",
                    &s->pci_mem_space, s->mmio_win_addr, s->mmio_win_size);

    sysbus_init_mmio(sbd, &s->mem_config);
    sysbus_init_mmio(sbd, &s->pci_io_window);
    sysbus_init_mmio(sbd, &s->pci_mem_window);
}

static void pci_generic_host_class_init(ObjectClass *klass, void *data)
{
    PCIDeviceClass *k = PCI_DEVICE_CLASS(klass);
    DeviceClass *dc = DEVICE_CLASS(klass);

    k->vendor_id = PCI_VENDOR_ID_REDHAT;
    k->device_id = PCI_DEVICE_ID_REDHAT_BRIDGE;
    k->class_id = PCI_CLASS_PROCESSOR_CO;
    /*
     * PCI-facing part of the host bridge, not usable without the
     * host-facing part, which can't be device_add'ed, yet.
     */
    dc->cannot_instantiate_with_device_add_yet = true;
}

static const TypeInfo pci_generic_host_info = {
    .name          = TYPE_GENERIC_PCI_HOST,
    .parent        = TYPE_PCI_DEVICE,
    .instance_size = sizeof(GenericPCIHostState),
    .class_init    = pci_generic_host_class_init,
};

static Property pci_generic_props[] = {
    DEFINE_PROP_UINT32("cfg_win_size", PCIGenState, cfg_win_size, 1ULL << 20),
    DEFINE_PROP_UINT32("pio_win_size", PCIGenState, pio_win_size, 64 * 1024),
    DEFINE_PROP_UINT64("mmio_win_size", PCIGenState, mmio_win_size, 1ULL << 32),
    DEFINE_PROP_UINT64("mmio_win_addr", PCIGenState, mmio_win_addr, 0),
    DEFINE_PROP_UINT32("irqs", PCIGenState, irqs, 4),
    DEFINE_PROP_END_OF_LIST(),
};

static void pci_generic_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    dc->realize = pci_generic_host_realize;
    dc->vmsd = &pci_generic_host_vmstate;
    dc->props = pci_generic_props;
}

static const TypeInfo pci_generic_info = {
    .name          = TYPE_GENERIC_PCI,
    .parent        = TYPE_PCI_HOST_BRIDGE,
    .instance_size = sizeof(PCIGenState),
    .class_init    = pci_generic_class_init,
};

static void generic_pci_host_register_types(void)
{
    type_register_static(&pci_generic_info);
    type_register_static(&pci_generic_host_info);
}

type_init(generic_pci_host_register_types)