FROM ubuntu:16.04

RUN apt-get update

RUN apt-get install -y gcc make
RUN apt-get install -y build-essential
RUN apt-get install -y ccache
RUN apt-get install -y libssl-dev
RUN apt-get install -y wget curl git
RUN apt-get install -y gawk sed bison flex
RUN apt-get install -y bc libelf-dev
RUN apt-get install -y vim
RUN apt-get install -y net-tools gdb
RUN apt-get install -y iputils-ping
RUN apt-get install -y libncurses-dev
RUN apt-get install -y cpio
RUN apt-get install -y codespell

ENV KERNEL_GIT=https://git.kernel.org/pub/scm/linux/kernel/git/gregkh/staging.git
ENV BUSYBOX_GIT=https://git.busybox.net/busybox

RUN cd /root && \
	git clone ${KERNEL_GIT} kernel && \
	git clone ${BUSYBOX_GIT} busybox


WORKDIR /root

