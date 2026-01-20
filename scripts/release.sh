#!/bin/sh

set -e 
set -o pipefail

cmake --build build --config Release

if [ $? -ne 0 ]; then
exit
fi

pushd build >/dev/null
./Release/vox
popd >/dev/null
