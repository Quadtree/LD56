#!/bin/bash

set -xe

source ~/emsdk/emsdk_env.sh

BUILD_DIR="${BUILD_DIR:-/tmp/build}"
OUTPUT_DIR="${OUTPUT_DIR:-/tmp/output}"
SOURCE_DIR="${SOURCE_DIR:-/src}"

if [ ! -d "$BUILD_DIR" ]; then
    mkdir "$BUILD_DIR"
fi

if [ ! -d "$OUTPUT_DIR" ]; then
    mkdir "$OUTPUT_DIR"
fi

cd "$BUILD_DIR"
embuilder build sdl2 sdl2_ttf sdl2_image
emcmake cmake "-DDEBUG_MODE=${DEBUG_MODE}" "-DOUTPUT_DIR=${OUTPUT_DIR}" "-DLIBS_DIR=${LIBS_DIR}" "$SOURCE_DIR"
cmake --build . -j

rsync -a "$SOURCE_DIR/" "/output/data/src/"
