#!/bin/sh

SCRIPT_DIR=$(dirname "$0")
SCRIPT_DIR=$(cd $SCRIPT_DIR && pwd -P)

rootfs_dir=$1

if [ -z "$rootfs_dir" ]; then
	echo "Usage: $0 <rootfs directory>"
	exit 1
fi

if [ ! -d $rootfs_dir ]; then
	echo "$rootfs_dir is not a valid directory"
	exit 1
fi


mkdir -p $rootfs_dir/bin/
mkdir -p $rootfs_dir/etc/
mkdir -p $rootfs_dir/etc/dropbear

cp $SCRIPT_DIR/files/init $rootfs_dir/
cp $SCRIPT_DIR/files/start-dropbear.sh $rootfs_dir/bin
cp $SCRIPT_DIR/files/passwd $rootfs_dir/etc/
cp $SCRIPT_DIR/files/profile $rootfs_dir/etc/
cp $SCRIPT_DIR/files/dropbear_rsa_host_key $rootfs_dir/etc/dropbear/
cp $SCRIPT_DIR/files/dropbear_dss_host_key $rootfs_dir/etc/dropbear/

cd $rootfs_dir

find . | cpio -o -H newc | gzip > ../rootfs.cpio.gz




