#!/bin/sh

dropbear_dir=/etc/dropbear
dropbearkey=dropbearkey
dropbear=dropbear
port=22

if [ ! -e $dropbear_dir/.ssh/authorized_keys ]; then
  mkdir -p $dropbear_dir
  mkdir -p $dropbear_dir/.ssh/
  touch $dropbear_dir/.ssh/authorized_keys
fi

if [ ! -f $dropbear_dir/dropbear_rsa_host_key ]; then
  $dropbearkey -t rsa -f $dropbear_dir/dropbear_rsa_host_key
  $dropbearkey -t dss -f $dropbear_dir/dropbear_dss_host_key
fi

key_args="-r $dropbear_dir/dropbear_dss_host_key -r $dropbear_dir/dropbear_rsa_host_key"

${dropbear} $key_args -p $port $*


