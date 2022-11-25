#!/bin/bash

set -e

usage() { echo "Usage: $0 [-v 'ubuntu18.04'|'ubuntu20.04' ]" 1>&2; exit 1; }

LinuxVersion="None"

while getopts ":v:" options; do
    case "${options}" in
        v)
            LinuxVersion=${OPTARG}
            if ! [[ $LinuxVersion == "ubuntu18.04" || $LinuxVersion == "ubuntu20.04" ]]; then
                usage
            fi
            ;;
        *)
            usage
            ;;
    esac
done
          
if [ $LinuxVersion == "ubuntu18.04" ]; then
    echo "Building Docker image with Ubuntu 18.04 as Linux version"
    docker build --file ./deploy/docker/Dockerfile-build-linux --build-arg LINUX_VERSION=ubuntu:18.04 -t qgc-linux-docker-ubuntu18.04 .
    rm -r build
    mkdir build
    docker run --rm -v ${PWD}:/project/source -v ${PWD}/build:/project/build qgc-linux-docker-ubuntu18.04
    sh ./deploy/create_linux_appimage.sh . build/staging/
elif [ $LinuxVersion == "ubuntu20.04" ]; then
    echo "Building Docker image with Ubuntu 20.04 as Linux version"
    docker build --file ./deploy/docker/Dockerfile-build-linux --build-arg LINUX_VERSION=ubuntu:20.04 -t qgc-linux-docker-ubuntu20.04 .
    rm -r build
    mkdir build
    docker run --rm -v ${PWD}:/project/source -v ${PWD}/build:/project/build qgc-linux-docker-ubuntu20.04
    sh ./deploy/create_linux_appimage.sh . build/staging/
else
    usage
    exit 1
fi
