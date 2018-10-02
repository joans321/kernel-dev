#!/bin/bash

PWD=`pwd`

docker run -it --rm --name ubuntu-dev -v $PWD/../ext-sources:/root/sources ubuntu-dev /bin/bash

