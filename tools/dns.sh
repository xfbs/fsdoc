#!/bin/bash

exec 3<>/dev/udp/9.9.9.9/53
packet="\x00\x00\x01\x00\x00\x01\x00\x00\x00\x00\x00\x00"
packet="$packet\x06github\x03com\x00\x00\x01\x00\x01"
printf $packet >&3
head -c 44 <&3 | tail -c 4 | hexdump -n 4 -e '/1 "%u" "."'
