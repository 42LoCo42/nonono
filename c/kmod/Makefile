obj-m += hello.o

PWD := $(CURDIR)

all:
	make -C $(KERNEL)/build M=$(PWD) modules

clean:
	make -C $(KERNEL)/build M=$(PWD) clean
