# ARM Confidential Compute Architecture using ARM fastmodels


**Dependencies:**  
**System package (ubuntu)**  
```
# binutils python3-pip device-tree-compiler xterm fakeroot mtools fdisk cpio dosfstools e2fsprogs libxml-libxml-perl jq lcov graphviz inkscape flex bison
# sudo apt install -y -qq --no-install-recommends <DEPS>
```


**Build:**  
```
# unzip the assets
tar -zxvf assets.tar.gz

# prepare source code
make init

# compile and setup everything
make 
```

**Run:**  
```
./run_fvp.sh

# wait for the host system to load, login with 'root'
```

**Launch realm VM**
```
cd /shared/guest
./launch_realm_on_fvp.sh
# this will take very long ... just wait
```

**In the realm VM**:  
run the attestation demo:  
```
# simply get and parse the (realm) attestation token 
./call
```

also this can be run in a server-client style:
```
./server &

echo <CHALLENGE> | ./client -c
# or use a default challenge
./client
```

currently the server and client are both run inside of the realm. (We haven't
set up network bridging and port fortwarding between worlds, once that's done,
it's trivial to run the client anywhere)

please do this before running server/client demo (sorry somrthing was missing in
the rootfs)
```
touch /dev/null
ifocnfig lo 127.0.0.1 netmask 255.0.0.0 up
``` 



# "Borrowed" from islet (samsung)

- their fork of optee-build
- deconfig for the host and guest kernel

The (busybox) rootfs: these are placed under assets directory in the islet repo.
We don't know where the credit goes to.

