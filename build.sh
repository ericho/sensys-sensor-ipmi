#!/bin/bash

docker run -t -i -v "$PWD":/sensys-bld erichcm/sensys-1.0 sh -c "cd /sensys-bld/ && autoreconf --install --symlink && ./configure && make"


