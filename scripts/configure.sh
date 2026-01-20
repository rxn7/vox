#!/bin/sh

cmake -Bbuild -G "Ninja Multi-Config" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
