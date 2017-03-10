#!/bin/bash

docker run -t -i -v "$PWD":/sensys-bld erichcm/sensys-1.0 sh -c "cd /sensys-bld/ && autoreconf --install --symlink && ./configure --prefix=/opt/sensys && make && LD_LIBRARY_PATH=/opt/sensys/lib make check TESTS= && ./ipmi_ts_tests"


