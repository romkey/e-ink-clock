#!/bin/bash

hash=$(git rev-parse --short HEAD)
date=$(date +"%y/%m/%d %H:%M:%S")
version="$hash $date"

#echo "-DMQTT_MAX_PACKET_SIZE=1024 -DMQTT_KEEPALIVE=10 -DNEW_PMS_LIBRARY=1 -DBUILD_INFO=\"\\\"$version\\\"\""
echo $version
