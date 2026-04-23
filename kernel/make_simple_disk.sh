#!/bin/bash

# Create a 32MB disk image
dd if=/dev/zero of=os.img bs=1M count=32

# Use parted to create partition table and FAT32 partition
parted os.img mklabel msdos
parted os.img mkpart primary fat32 1MiB 31MiB
parted os.img set 1 boot on

# Setup loop device
LOOP=$(sudo losetup -f --show os.img)
sudo partprobe $LOOP

# Format the partition
sudo mkfs.vfat -F32 ${LOOP}p1

# Mount the partition
mkdir -p mnt
sudo mount ${LOOP}p1 mnt

# Create directory structure
sudo mkdir -p mnt/boot/grub

# Copy kernel and GRUB config
sudo cp kernel.bin mnt/boot/
sudo cp grub.cfg mnt/boot/grub/

# Install GRUB
sudo grub-install --target=i386-pc --boot-directory=mnt/boot --recheck $LOOP

# Unmount and cleanup
sudo umount mnt
sudo losetup -d $LOOP
rmdir mnt

echo "GRUB disk image os.img created successfully!"
