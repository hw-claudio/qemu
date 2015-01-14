#ifndef QEMU_GENERIC_PCI_H
#define QEMU_GENERIC_PCI_H

#include "hw/pci/pci.h"
#include "hw/pci/pci_bus.h"
#include "hw/pci/pci_host.h"

#define MAX_PCI_DEVICES (PCI_SLOT_MAX * PCI_FUNC_MAX)

typedef struct {
    /*< private >*/
    PCIDevice parent_obj;
} GenericPCIHostState;

typedef struct PCIGenState {
    /*< private >*/
    PCIHostState parent_obj;

    qemu_irq irq[MAX_PCI_DEVICES];
    MemoryRegion mem_config;
    /* Container representing the PCI address MMIO space */
    MemoryRegion pci_mem_space;
    /* Alias region into PCI address spaces which we expose as sysbus region */
    MemoryRegion pci_mem_window;
    /* PCI I/O region */
    MemoryRegion pci_io_window;
    PCIBus pci_bus;
    GenericPCIHostState pci_gen;

    uint32_t cfg_win_size;
    uint32_t pio_win_size;
    uint64_t mmio_win_addr; // offset of pci_mem_window inside pci_mem_space
    uint64_t mmio_win_size;
    uint32_t irqs;
} PCIGenState;

#define TYPE_GENERIC_PCI "generic_pci"
#define PCI_GEN(obj) \
    OBJECT_CHECK(PCIGenState, (obj), TYPE_GENERIC_PCI)

#define TYPE_GENERIC_PCI_HOST "generic_pci_host"
#define PCI_GEN_HOST(obj) \
    OBJECT_CHECK(GenericPCIHostState, (obj), TYPE_GENERIC_PCI_HOST)

#endif