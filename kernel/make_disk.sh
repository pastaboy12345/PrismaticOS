#!/bin/bash

# Create a 32MB disk image
dd if=/dev/zero of=os.img bs=1M count=32

# Create ext2 filesystem on the entire image
mkfs.ext2 -F os.img

# Create mount point
mkdir -p mnt

# Mount the image (requires sudo)
sudo mount -o loop os.img mnt

# Create directory structure
sudo mkdir -p mnt/boot/grub

# Copy kernel and GRUB config
sudo cp kernel.bin mnt/boot/
sudo cp grub.cfg mnt/boot/grub/

# Install GRUB
sudo grub-install --target=i386-pc --boot-directory=mnt/boot --recheck --force os.img

# Unmount and cleanup
sudo umount mnt
rmdir mnt

echo "GRUB disk image os.img created successfully!"
