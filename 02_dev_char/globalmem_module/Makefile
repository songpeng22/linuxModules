KVERS = $(shell uname -r)
# Kernel modules
obj-m += globalmem.o

# Specify flags for the module compilation.
#EXTRA_CFLAGS=-g -O0
build: kernel_modules

kernel_modules:
	make -C /lib/modules/$(KVERS)/build M=$(CURDIR) modules
arm_kernel_modules:
	make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- -C /home/tim/firefly-rk3399-jd4/kernel M=$(CURDIR) modules
clean:
	make -C /lib/modules/$(KVERS)/build M=$(CURDIR) clean
