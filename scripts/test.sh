#!/bin/sh

cmake --build build --target vox_all_tests --config Debug

pushd build
ctest --output-on-failure -C Debug
popd
