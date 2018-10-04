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


cp $SCRIPT_DIR/init $rootfs_dir/init

cd $rootfs_dir

find . | cpio -o -H newc | gzip > ../rootfs.cpio.gz




