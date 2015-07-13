obj-m := nugpgpu.o

# C Source files
# Add C files here <space> separated
nugpgpu-objs := nugpgpu_dbg.o nugpgpu_gtt.o nugpgpu_regrw.o nugpgpu_init.o nugpgpu_ioctl.o nugpgpu_execbuffer.o nugpgpu_ringbuffer.o nugpgpu_memmgr.o nugpgpu_tests.o nugpgpu_fileops.o

KERNEL = $(shell uname -r)

all:	udev
	make -C /lib/modules/$(KERNEL)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(KERNEL)/build M=$(PWD) clean

udev:
	sudo su -c 'echo "KERNEL==\"nugpgpu\", MODE=\"0666\"" > /etc/udev/rules.d/50-nugpgpu.rules'
