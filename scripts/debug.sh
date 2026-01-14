#!/bin/sh

cmake --build build/debug

if [ $? -ne 0 ]; then
exit
fi

cp build/debug/compile_commands.json .

pushd build/debug >/dev/null
./vox
popd >/dev/null
