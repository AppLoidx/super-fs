#!/bin/bash
#
# Create loop device file in current folder (with mounting on /dev/loop* via losetup)
# 
# P.S. Feel free to use, because it's not a part of labwork
#
# Author : Arthur Kuprianov, 2021
#

# File name (can be anything u want)
FS_FILE="xfs.fs"
# File system type. xfs, ext4 and etc. This variable will be used in mkfs command like: mkfs -t xfs
FS_TYPE="xfs"

touch $FS_FILE
dd if=/dev/zero of=$FS_FILE bs=1M count=64

mkfs -t "$FS_TYPE" "$FS_FILE"

ld=$(sudo losetup --show -fP $FS_FILE)

if [ $? -eq 0 ];
then
  echo "Successfully mounted ${FS_TYPE} loop device on ${ld}"
fi
