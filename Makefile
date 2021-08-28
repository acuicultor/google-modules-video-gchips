# SPDX-License-Identifier: GPL-2.0
#
# Makefile for bigocean
#

obj-$(CONFIG_BIGOCEAN) += bigocean.o
bigocean-$(CONFIG_BIGOCEAN) += bigo.o bigo_pm.o bigo_io.o bigo_of.o bigo_iommu.o
bigocean-$(CONFIG_SLC_PARTITION_MANAGER) += bigo_slc.o
bigocean-$(CONFIG_DEBUG_FS) += bigo_debug.o

KERNEL_SRC ?= /lib/modules/$(shell uname -r)/build
M ?= $(shell pwd)

KBUILD_OPTIONS += CONFIG_BIGOCEAN=m CONFIG_SLC_PARTITION_MANAGER=m \
		  CONFIG_DEBUG_FS=m

ccflags-y := -I$(KERNEL_SRC)/../google-modules/video/gchips

modules modules_install clean:
	$(MAKE) -C $(KERNEL_SRC) M=$(M) $(KBUILD_OPTIONS) W=1 $(@)