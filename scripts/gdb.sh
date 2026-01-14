#!/bin/sh

pushd build/debug >/dev/null
gdb ./vox -q
popd >/dev/null
