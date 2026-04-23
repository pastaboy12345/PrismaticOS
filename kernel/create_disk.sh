#!/bin/bash

# Create a disk image
dd if=/dev/zero of=os.img bs=1M count=16

# Create partition table
sgdisk -n 1:2048:32767 os.img

# Setup loop device
LOOP=$(losetup -f --show -P os.img)

# Format partition
mkfs.ext2 ${LOOP}p1

# Mount partition
mkdir -p mnt
mount ${LOOP}p1 mnt

# Create boot directory structure
mkdir -p mnt/boot/grub

# Copy kernel and grub config
cp kernel.bin mnt/boot/
cp grub.cfg mnt/boot/grub/

# Install GRUB
grub-install --target=i386-pc --boot-directory=mnt/boot ${LOOP}

# Cleanup
umount mnt
losetup -d ${LOOP}
rmdir mnt

echo "Disk image os.img created successfully!"
