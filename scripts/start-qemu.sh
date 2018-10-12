#!/bin/bash

SCRIPT_DIR=$(dirname "$0")
SCRIPT_DIR=$(cd $SCRIPT_DIR && pwd -P)

TOP_DIR=$SCRIPT_DIR/..
kernel_img=$TOP_DIR/shares/output/bzImage
rootfs_img="$TOP_DIR/shares/output/rootfs.cpio.gz"

qemu_cmd=qemu-system-x86_64
nodisplay=false
enable_gdb=false
enable_kvm=false
gdb_port=4433
ostype=Mac

# qemu_args="-ctrl-grab"
qemu_args="$qemu_args -m 1024 -smp 4 -append 'console=ttyS0'"
qemu_args="$qemu_args -serial mon:stdio"
qemu_args="$qemu_args -kernel $kernel_img -initrd $rootfs_img"

# enable network interface and forward host port to guest ip:port
qemu_args="$qemu_args -nic user,hostfwd=tcp::2222-10.0.2.15:22"


function os_detect() {
  osname=`uname -s`
  case "${osname}" in
    Linux*)  ostype=Linux;;
    Darwin*) ostype=Mac;;
    CYGWIN*) ostype=Cywin;;
    MINGW*)  ostype=Mingw;;
    *)       ostype="Unknown-${osname}"
  esac
  echo "Your system is : ${ostype}"
}

function kvm_detect() {
  if [ ${ostype} = "Linux" ]; then
    has_kvm=`lsmod |grep kvm`
    if [ -n "${has_kvm}" ]; then
      enable_kvm=true
    fi
  fi
}

os_detect
kvm_detect

if [ "$1" = "-g" ]; then
  enable_gdb=true
fi

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

