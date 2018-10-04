# Linux kernel develop

This topic is about how to develop the Linux Kernel in MacOS.

* Create docker image for building and debug
* Build kernel in Docker
* Debug kernel with Qemu and gdb

**Note: if you are linux user, you don't need docker**

First of all, you need to clone this project and export shell environment:

~~~sh
$ git clone https://github.com/joans321/kernel-dev
$ cd kernel-dev
$ export KDEV_TOP=`pwd`
~~~



# About Souce Code

There are two ways to store the source code of kernel and busybox.

* One way is hold codes into docker image, this way is preferred. But every time you start the docker container, you must build source code from scratch.  And you can only use vim to browse the source code in docker container.

* The other way is keep the codes in MacOS and then  use docker volume to share codes. There may be some errors when building kernel, because MacOs filename is not case insensitive. You need to have the skills to fix the error.

  After the source is built for the first time, the temporary objects are cached in the docker volume. The next time build is faster then a fresh build.  You will get more benefits when you build the kernel again and again. 

~~~sh
$ cd $KDEV_TOP/shares

### Busybox git repo
$ git clone https://git.busybox.net/busybox

### Kernel git repo
$ git clone https://git.kernel.org/pub/scm/linux/kernel/git/gregkh/staging.git kernel
~~~

When starting docker image, we will mount `shares` directory to docker container with the -v parameter.



# Installation

* [Docker Install for Mac](https://docs.docker.com/docker-for-mac/install/)
* Qemu Install by [source](https://www.qemu.org/download/#source) :

~~~sh
$ git clone git://git.qemu.org/qemu.git
$ cd qemu
$ git submodule init
$ git submodule update --recursive
$ ./configure
$ make
$ make install
~~~

* Build docker image use these [Dockerfile](ubuntu-docker/Dockerfile )

~~~Sh
$ cd $KDEV_TOP/ubuntu-docker

# use ubuntu 16.04 to build kernel and busybox
$ docker build . -t ubuntu-dev
~~~



# Building

* Run docker image with

~~~sh
### Start the docker images as a daemon
$ cd $KDEV_TOP
$ ./scripts/start-ubuntu.sh
### Or do command : docker run -it -d --name ubuntu-dev -v $PWD/shares:/root/shares ubuntu-dev /bin/bash

### Enter the container
$ docker exec -it ubuntu-dev /bin/bash
~~~

* Build kernel in docker

~~~sh
$ cd /root/kernel

### Modify Makefile :
###   KBUILD_CFLAGS += -O2 --> KBUILD_CFLAGS += -O1
$ sed -i 's/-O2/-O1/g' Makefile

$ make x86_64_defconfig

$ make menuconfig 
###   Processor type and features --->
###     Build a relocatable kernel [Disable]
###   Kernel hacking --->
###     Kernel debugging [Enable]
###     Compile-time checks and copiler options --->
###       Compile the kernel with debug info [Enable]
###       Provide GDB scripts for kernel debuggin [Enable]
###     
### or edit .config file
###   CONFIG_DEBUG_INFO=y
###   CONFIG_GDB_SCRIPTS=y
###   CONFIG_DEBUG_KERNEL=y
###   # CONFIG_RELOCATABLE is not set
###   # CONFIG_DYNAMIC_MEMORY_LAYOUT is not set
###   # CONFIG_RANDOMIZE_MEMORY is not set
###   # CONFIG_RANDOMIZE_MEMORY_PHYSICAL_PADDING is not set

$ make -j8

### Copy kernel image to share volume
$ mkdir -p /root/shares/output/
$ cp arch/x86_64/boot/bzImage /root/shares/output/bzImage
~~~

* Build busybox in docker

~~~sh
$ cd /root/busybox
$ make defconfig
$ make menuconfig
### enable Settings ---> Build static binary
### or LDFLAGS="--static" make -j4
$ make -j4

### install busybox to special directory
$ mkdir -p /root/shares/output/rootfs
$ make CONFIG_PREFIX=/root/shares/output/rootfs install

### create rootfs
$ /root/shares/mkramfs.sh /root/shares/output/rootfs
~~~



# Initramfs

Qemu supports initramfs, initrd ramdisk and hda root image. The only problem is that MacOS does not support mounting an ext4 filesystem, nor docker support it. Therefor, we choose initramfs as rootfs.

Make rootfs command with newc format :

~~~sh
$ cd <rootfs directory>
$ find . | cpio -o -H newc | gzip > ../rootfs.cpio.gz

# Check rootfs.cpio.gz file
# make a temp directory and copy rootfs.cpio.gz to the directory
$ zcat rootfs.cpio.gz | cpio -i -d -H newc --no-absolute-filename
~~~

Here we run the [mkramfs.sh](shares/mkramfs.sh) script in docker container to create rootfs.cpio.gz after busybox is built.

~~~sh
$ /root/shares/mkramfs.sh /root/shares/output/rootfs
~~~



# Debug

* start qemu in macos

~~~sh
$ cd scripts
### start qemu with gdb debug port 4433
$ ./start-qemu.sh -g
~~~

* Run gdb in docker container

~~~sh
$ cd /root/kernel
### My Host Machine IP is 192.168.0.103
$ gdb vmlinux
(gdb) target remote 192.168.0.103:4433
Remote debugging using 192.168.0.103:4433
0x000000000000fff0 in cpu_hw_events ()
(gdb) break start_kernel 
Breakpoint 1 at 0xffffffff82604b82: file init/main.c, line 532.
(gdb) c
Continuing.

Thread 1 hit Breakpoint 1, start_kernel () at init/main.c:532
532	{
(gdb) 
~~~



# License

[MIT](LICENSE.md )