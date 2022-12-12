#!/usr/bin/env bash

# This script is intended to be run from an Ubuntu Docker image (or other Debian-derived distros).
# It uses `apt-get` to install dependencies required by the examples,

# build with:
cmake -S . -B "./build"
cmake --build "./build"