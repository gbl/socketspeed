Two programs to measure (TCPv4) network speed between two nodes.
================================================================

I needed those for a while to test raw TCPv4 network speed between two nodes.

I'm involved in "why is my network so slow" questions quite a lot, and customers
typically measure anything but the network, when it comes to fast (10 Gbps...)
networks. Most of these are bad ideas.

Copying files tends to measure disk speed, not network speed.

Copying files via SMB (Windows shares ..) measures your network latency a lot
more than your connection speed, even if disk isn't the bottleneck. Especially
if you use SMBv1 with a 64K per read/write limit.

Downloading files by http measures your disk writes, and the disk reads of the
server (at best), or the speed your apache processes php (worse) or even the
speed your tomcat executes java servlets (much worse).

Downloading files via https measures AES encryption/decryption speed. Hint 1:
an Intel CPU isn't able to saturate a 10 Gbps link BY FAR, even with hardware
AES. Hint 2: don't even think of anything but AES, or AES in software.

Just like with disk access, lots of small-buffer reads/writes can slow down your
network connection considerably. So you really need to be able to control your
buffer size. My server could be replaced by netcat reading from /dev/zero, and
the client could be replaced by netcat writing to /dev/null or nul, but I found
a pair of netcats to be a lot slower than my programs with a buffer size of 1MB.

Anyway, here's how to use them:

On the server: socketsend  0.0.0.0  12321 1M 1024

means bind to IP address 0.0.0.0 port 12321, transfer 1MB at a time, and try to
do 1024 of those transfers. You can use 127.0.0.1 or one of your real ips instead.

On the client: socketslurp 1.2.3.4 12321 1M 1024

means connect to 1.2.3.4 and try to read up to 1024 1M buffers.

The side of the connection that has the smaller amount of data determines how 
much will be transferred. After the transfer is done, the server will write
some statistics to stdout and continue listening for requests:

sent 134217728 bytes in 0.031454 seconds to 192.168.178.161, 4267 MByte/s

and the client will write out its own statistics and exit.

