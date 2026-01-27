#!/bin/sh

find ./client ./common ./server -name "*.cpp" -o -name "*.hpp" | xargs wc -l | sort -n
