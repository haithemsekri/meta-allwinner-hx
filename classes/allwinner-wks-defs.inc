# Definitions for creating wks images for the allwinner
#
# Author: Dimitris Tassopoulos <dimtass@gmail.com>

# sun8i builds a composite image for SPL and u-boot
SPL_NAME_sun8i ?= "u-boot-sunxi-with-spl.bin"

# sun50i builds an SPL and a u-boot image
SPL_NAME_sun50iw2 ?= "sunxi-spl.bin"
UBOOT_IMAGE_sun50iw2 ?= "u-boot.itb"
SPL_NAME_sun50iw6 ?= "sunxi-spl.bin"
UBOOT_IMAGE_sun50iw6 ?= "u-boot.itb"

# name of boot partition image, empty value combines the boot and rootfs partitions
SUNXI_BOOT_IMAGE ?= "boot.img"

# device will be used in u-boot and /etc/fstab
SUNXI_STORAGE_DEVICE ?= "mmcblk0"

# This is the extra space for the rootfs
ROOT_EXTRA_SPACE ?= "4096"

# if you need swap file then add here the size of it
SUNXI_SWAP_SIZE ?= "512"
