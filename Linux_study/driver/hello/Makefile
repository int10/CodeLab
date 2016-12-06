obj-m = tst.o

K_DIR = $(shell uname -r)
PWD = $(shell pwd)

all:
	make -C /lib/modules/$(K_DIR)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(K_DIR)/build M=$(PWD) clean