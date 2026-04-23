#!/bin/bash

# Create a 32MB disk image
dd if=/dev/zero of=os.img bs=1M count=32

# Create partition table with one partition
echo -e "o\nn\np\n1\n\n\na\n1\nw\n" | fdisk os.img

# Setup loop device for partition
LOOP=$(sudo losetup -f --show os.img)
PARTITION="${LOOP}p1"

# Format the partition as FAT32
sudo mkfs.vfat -F32 $PARTITION

# Mount the partition
mkdir -p mnt
sudo mount $PARTITION mnt

# Create directory structure
sudo mkdir -p mnt/boot/grub

# Copy kernel and GRUB config
sudo cp kernel.bin mnt/boot/
sudo cp grub.cfg mnt/boot/grub/

# Install GRUB to the disk (not partition)
sudo grub-install --target=i386-pc --boot-directory=mnt/boot --recheck $LOOP

# Unmount and cleanup
sudo umount mnt
sudo losetup -d $LOOP
rmdir mnt

echo "Proper GRUB disk image os.img created successfully!"
