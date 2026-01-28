#!/bin/sh

find ./client ./common ./dedicated_server -name "*.cpp" -o -name "*.hpp" | xargs wc -l | sort -n
