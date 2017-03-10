#!/bin/bash

while [ "1"="1" ]
do
	./client &
	sleep 6
	killall client
done
