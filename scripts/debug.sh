#!/bin/sh

set -e 
set -o pipefail

cmake --build build --config Debug

if [ $? -ne 0 ]; then
exit
fi

pushd build/client >/dev/null
./Debug/vox
popd >/dev/null
