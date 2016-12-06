
CC= arm-linux-gcc
K_DIR = $(shell uname -r)
PWD = $(shell pwd)

ifeq ($(KERNELRELEASE),)
all:
	make -C ~/w55fa95bsp/linux-2.6.17.14 M=$(PWD) modules
	#make -C /lib/modules/$(K_DIR)/build M=$(PWD) modules
clean:
	make -C ~/w55fa95bsp/linux-2.6.17.14 M=$(PWD) clean
	#make -C /lib/modules/$(K_DIR)/build M=$(PWD) clean
	
.PHONY: modules modules_install clean

else	
MODULE_NAME := skobj
obj-m := $(MODULE_NAME).o
endif  
	




	
