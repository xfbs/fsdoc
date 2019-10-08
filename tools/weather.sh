#!/bin/bash

# open tcp connection at fd 3
exec 3<>/dev/tcp/wttr.in/80
# send http request, masquerading as curl
printf "GET / HTTP/1.0\r\n" >&3
printf "User-Agent: curl/7.58.0\r\n" >&3
printf "Host: wttr.in\r\n\r\n" >&3
# print response
cat <&3
