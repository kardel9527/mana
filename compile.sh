#!/bin/bash

cd common;make clean;make;
cd ../network/;make clean;make;
cd ../test/server/;make clean;make;
cd ../client/;make clean;make;
