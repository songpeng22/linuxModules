# author
https://blog.csdn.net/kuishao1314aa/article/details/80505657

# test method 1
# create device inode
mknod /dev/globalmem c 230 0   //230 0 为你创建设备的主设备与次设备号
# root
su - root
# write string
echo "hello world!">/dev/globalmem
# show what is written
cat /dev/globalmem
# show log
dmesg

# test method 2
make globalmem_test and run
