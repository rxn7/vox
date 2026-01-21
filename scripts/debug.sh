#!/bin/sh

set -e 
set -o pipefail

cmake --build build --config Debug

if [ $? -ne 0 ]; then
exit
fi

pushd build/game >/dev/null
./Debug/vox_game
popd >/dev/null
