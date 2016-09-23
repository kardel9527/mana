#!/bin/bash

cd common;make clean;make;
cd ../reactor/;make clean;make;
cd ../test/client/;make clean;make;
cd ../server/;make clean;make
