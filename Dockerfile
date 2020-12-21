FROM ubuntu:18.04
RUN apt-get update && apt-get install -y gdb build-essential software-properties-common
RUN add-apt-repository ppa:ubuntu-toolchain-r/test && apt update
RUN apt install -y gcc-9
RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-9 60
