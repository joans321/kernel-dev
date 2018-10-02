# Linux kernel develop

This topic is about how to develop the Linux Kernel in MacOS.

* Build kernel in Docker
* Debug kernel with Qemu and gdb



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
$ cd ubuntu-docker

# use ubuntu 16.04 to build kernel and busybox
$ docker build . -t ubuntu-dev
~~~



# Build

* Clone source into ext-sources directory

~~~sh
$ cd ext-sources
$ git clone https://git.busybox.net/busybox
$ git clone https://git.kernel.org/pub/scm/linux/kernel/git/gregkh/staging.git kernel
$ cd ..
~~~

* Run docker image with 

~~~sh
$ docker run -it --rm --name ubuntu-dev -v $PWD/ext-sources:/root/sources ubuntu-dev /bin/bash

### For convernience, you can use script:
### $ cd dbg-kernel; ./start-ubuntu.sh
~~~

* Build kernel in docker

~~~sh
$ cd /root/sources/kernel
$ make x86_64_defconfig
$ make menuconfig 
### or edit .config file
###   CONFIG_DEBUG_INFO=y
###   CONFIG_GDB_SCRIPTS=y
###   CONFIG_DEBUG_KERNEL=y
~~~

* Build busybox in docker

~~~sh
$ cd /root/sources/busybox
$ make defconfig
$ make menuconfig
### enable Settings ---> Build static binary
### or LDFLAGS="--static" make -j4
$ make -j4
$ mkdir -p ../rootfs

### install busybox to directory ../rootfs
$ make CONFIG_PREFIX=../rootfs install
~~~



# Debug

* Create initramfs as rootfs in docker container

~~~sh
$ cd /root/sources/rootfs
$ cp ../init .
$ find . | cpio -o -H newc | gzip > ../rootfs.cpio.gz
~~~

* check ramfs for test, you can skip this command

~~~sh
# make a temp directory and copy rootfs.cpio.gz to the directory
$ zcat rootfs.cpio.gz | cpio -i -d -H newc --no-absolute-filename
~~~

* start qemu in macos

~~~sh
$ cd scripts
$ ./start-qemu.sh
~~~



# License

[MIT](LICENSE.md )