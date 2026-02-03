#!/bin/sh

find ./client ./common ./dedicated_server -name "*.cpp" -o -name "*.hpp" -o -name "*.glsl" | xargs wc -l | sort -n
