#!/bin/sh

pushd build/client/game >/dev/null
gdb ./Debug/vox_game -q
popd >/dev/null
