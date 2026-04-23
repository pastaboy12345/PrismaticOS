#!/bin/bash

# Create a simple disk image with GRUB legacy
dd if=/dev/zero of=os.img bs=1M count=16

# Create filesystem on the entire disk
mkfs.ext2 -F os.img

# Mount the image
mkdir -p mnt
sudo mount -o loop os.img mnt

# Create boot directory structure
sudo mkdir -p mnt/boot/grub

# Copy kernel and grub config
sudo cp kernel.bin mnt/boot/
sudo cp grub.cfg mnt/boot/grub/

# Install GRUB legacy
sudo grub-install --target=i386-pc --boot-directory=mnt/boot --force os.img

# Cleanup
sudo umount mnt
rmdir mnt

echo "Disk image os.img created with GRUB!"
