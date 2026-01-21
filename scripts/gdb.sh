#!/bin/sh

pushd build/game >/dev/null
gdb ./Debug/vox_game -q
popd >/dev/null
