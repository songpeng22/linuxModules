# make
make

# clean
make clena

# insert module
insmod hello.so
sudo journalctl -b

# test
lsmod | grep hello
# or
sudo journalctl -b

# remove module
sudo rmmod hello
sudo journalctl -b

