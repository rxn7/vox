#!/bin/sh

pushd build >/dev/null
gdb ./Debug/vox -q
popd >/dev/null
