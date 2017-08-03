#!/bin/bash

cd common;make clean;make;
cd ../core/;make clean;make;
cd ../test/server/;make clean;make;
cd ../client/;make clean;make;
