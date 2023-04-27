
----- SETUP: ---------------

1. apply the changes to the realm guest linux kernel, on top of linux-arm/linux-cca: cca-full/rfc-v1
2. compile the userspace program
    $ make call client server
    # call is a standalone program that does a syscall and parse the output
    # server will listen (port 1234) to challenge and return the attest token
    # client sends the challenge to server, and get the attest token
3. bundle the program into a guest rootfs
    # for example:
    $ cp call PATH/TO/ROOTFS/call
    $ cp server PATH/TO/ROOTFS/server
    $ cd /PATH/TO/ROOTFS
    $ find . | cpio --create --format='newc' > ../rootfs-realm.cpio
    $ cd ..
    $ gzip -f rootfs-realm.cpio
4. run fastmodel, then launch realm inside of it





----- USAGE: ---------------

# in the realm
./call

# or run the server
./server

# and use client somewhere (if you make port forwarding work)
./client

# or pass a custom challenge via stdio
cat challenge.txt | ./client -c


