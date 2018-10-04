#!/bin/bash

SCRIPT_DIR=$(dirname "$0")
SCRIPT_DIR=$(cd $SCRIPT_DIR && pwd -P)

echo $SCRIPT_DIR

docker run -it -d --name ubuntu-dev -v $SCRIPT_DIR/../shares:/root/shares ubuntu-dev /bin/bash

