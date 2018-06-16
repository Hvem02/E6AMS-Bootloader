#!/bin/bash
work_dir=`pwd`
avrdude -C avrdude.conf -P/dev/ttyACM0 -cwiring -patmega2560 -b115200 -v -D -Uflash:w:../cmake-build-release/E6AMS