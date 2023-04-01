#!/bin/zsh

mkdir ./build
pushd ./build
export GRPC_LIB=../grpc_lib
cmake -DCMAKE_PREFIX_PATH=$GRPC_LIB ..
make
popd