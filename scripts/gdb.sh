#!/bin/sh

pushd build/client >/dev/null
gdb ./Debug/vox -q
popd >/dev/null
