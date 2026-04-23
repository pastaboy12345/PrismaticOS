#!/bin/bash

# Create a 32MB disk image
dd if=/dev/zero of=os.img bs=1M count=32

# Create FAT32 filesystem
mkfs.vfat -F32 os.img

# Create mount point
mkdir -p mnt

# Mount the image
sudo mount -o loop os.img mnt

# Create directory structure
sudo mkdir -p mnt/boot/grub

# Copy kernel and GRUB config
sudo cp kernel.bin mnt/boot/
sudo cp grub.cfg mnt/boot/grub/

# Install GRUB for FAT
sudo grub-install --target=i386-pc --boot-directory=mnt/boot --removable --force os.img

# Unmount and cleanup
sudo umount mnt
rmdir mnt

echo "GRUB FAT disk image os.img created successfully!"
