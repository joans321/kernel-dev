#!/bin/bash

SCRIPT_DIR=$(dirname "$0")
SCRIPT_DIR=$(cd $SCRIPT_DIR && pwd -P)

TOP_DIR=$SCRIPT_DIR/..
kernel_img=$TOP_DIR/shares/output/bzImage
rootfs_img="$TOP_DIR/shares/output/rootfs.cpio.gz"

qemu_cmd=qemu-system-x86_64
nodisplay=false
enable_gdb=false
gdb_port=4433


if [ "$1" = "-g" ]; then
  enable_gdb=true
fi


qemu_args="-ctrl-grab -m 1024 -smp 4 -append 'console=ttyS0'"
qemu_args="$qemu_args -serial mon:stdio"
qemu_args="$qemu_args -kernel $kernel_img -initrd $rootfs_img"

if [ "$enable_kvm" = "true" ];then
  qemu_args="$qemu_args --enable-kvm"
fi

if [ "$nodisplay" = "true" ]; then
  qemu_args="$qemu_args -nographic"
fi

if [ "$enable_gdb" = "true" ]; then
  qemu_args="$qemu_args -S -gdb tcp::${gdb_port}"
fi


echo "$qemu_cmd $qemu_args"
$qemu_cmd $qemu_args 

