# Default configuration for aarch64-softmmu

include pci.mak
include usb.mak

# We support all the 32 bit boards so need all their config
include arm-softmmu.mak

# Currently no 64-bit specific config requirements
