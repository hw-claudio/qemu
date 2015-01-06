/*
 * QEMU Generic PCI Express Bridge Emulation
 *
 * Copyright (C) 2015 Alexander Graf <agraf@suse.de>
 *
 * Code loosely based on q35.c.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include "hw/hw.h"
#include "hw/pci-host/gpex.h"

/****************************************************************************
 * GPEX host
 */

static void gpex_set_irq(void *opaque, int irq_num, int level)
{
    GPEXHost *s = opaque;

    qemu_set_irq(s->irq, level);
}

static int gpex_map_irq(PCIDevice *pci_dev, int irq_num)
{
    /* We only support one IRQ line so far */
    return 0;
}

static void gpex_host_realize(DeviceState *dev, Error **errp)
{
    PCIHostState *pci = PCI_HOST_BRIDGE(dev);
    GPEXHost *s = GPEX_HOST(dev);
    SysBusDevice *sbd = SYS_BUS_DEVICE(dev);
    PCIExpressHost *pex = PCIE_HOST_BRIDGE(dev);

    pcie_host_mmcfg_init(pex, PCIE_MMCFG_SIZE_MIN);
    memory_region_init(&s->io_mmio, OBJECT(s), "gpex_mmio", s->mmio_window_size);
    memory_region_init(&s->io_ioport, OBJECT(s), "gpex_ioport", 64 * 1024);

    sysbus_init_mmio(sbd, &pex->mmio);
    sysbus_init_mmio(sbd, &s->io_mmio);
    sysbus_init_mmio(sbd, &s->io_ioport);
    sysbus_init_irq(sbd, &s->irq);

    pci->bus = pci_register_bus(dev, "pcie.0", gpex_set_irq, gpex_map_irq, s,
                              &s->io_mmio, &s->io_ioport, 0, 1, TYPE_PCIE_BUS);

    qdev_set_parent_bus(DEVICE(&s->gpex_root), BUS(pci->bus));
    qdev_init_nofail(DEVICE(&s->gpex_root));
}

static const char *gpex_host_root_bus_path(PCIHostState *host_bridge,
                                          PCIBus *rootbus)
{
    return "0000:00";
}

static Property gpex_root_props[] = {
    DEFINE_PROP_UINT64("mmio_window_size", GPEXHost, mmio_window_size, 1ULL << 32),
    DEFINE_PROP_END_OF_LIST(),
};

static void gpex_host_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);
    PCIHostBridgeClass *hc = PCI_HOST_BRIDGE_CLASS(klass);

    hc->root_bus_path = gpex_host_root_bus_path;
    dc->realize = gpex_host_realize;
    dc->props = gpex_root_props;
    set_bit(DEVICE_CATEGORY_BRIDGE, dc->categories);
    dc->fw_name = "pci";
}

static void gpex_host_initfn(Object *obj)
{
    GPEXHost *s = GPEX_HOST(obj);

    object_initialize(&s->gpex_root, sizeof(s->gpex_root), TYPE_GPEX_ROOT_DEVICE);
    object_property_add_child(OBJECT(s), "gpex_root", OBJECT(&s->gpex_root), NULL);
    qdev_prop_set_uint32(DEVICE(&s->gpex_root), "addr", PCI_DEVFN(0, 0));
    qdev_prop_set_bit(DEVICE(&s->gpex_root), "multifunction", false);
}

static const TypeInfo gpex_host_info = {
    .name       = TYPE_GPEX_HOST,
    .parent     = TYPE_PCIE_HOST_BRIDGE,
    .instance_size = sizeof(GPEXHost),
    .instance_init = gpex_host_initfn,
    .class_init = gpex_host_class_init,
};

/****************************************************************************
 * GPEX Root D0:F0
 */

static const VMStateDescription vmstate_gpex_root = {
    .name = "gpex_root",
    .version_id = 1,
    .minimum_version_id = 1,
    .fields = (VMStateField[]) {
        VMSTATE_PCI_DEVICE(parent_obj, GPEXRootState),
        VMSTATE_END_OF_LIST()
    }
};

static void gpex_root_class_init(ObjectClass *klass, void *data)
{
    PCIDeviceClass *k = PCI_DEVICE_CLASS(klass);
    DeviceClass *dc = DEVICE_CLASS(klass);

    set_bit(DEVICE_CATEGORY_BRIDGE, dc->categories);
    dc->desc = "Host bridge";
    dc->vmsd = &vmstate_gpex_root;
    k->vendor_id = PCI_VENDOR_ID_REDHAT;
    k->device_id = PCI_DEVICE_ID_REDHAT_BRIDGE;
    k->revision = 0;
    k->class_id = PCI_CLASS_BRIDGE_HOST;
    /*
     * PCI-facing part of the host bridge, not usable without the
     * host-facing part, which can't be device_add'ed, yet.
     */
    dc->cannot_instantiate_with_device_add_yet = true;
}

static const TypeInfo gpex_root_info = {
    .name = TYPE_GPEX_ROOT_DEVICE,
    .parent = TYPE_PCI_DEVICE,
    .instance_size = sizeof(GPEXRootState),
    .class_init = gpex_root_class_init,
};

static void gpex_register(void)
{
    type_register_static(&gpex_root_info);
    type_register_static(&gpex_host_info);
}

type_init(gpex_register);
