
CC= arm-linux-gcc
K_DIR = $(shell uname -r)
PWD = $(shell pwd)

ifeq ($(KERNELRELEASE),)
all:
	make -C ~/w55fa95bsp/linux-2.6.17.14 M=$(PWD) modules
clean:
	make -C ~/w55fa95bsp/linux-2.6.17.14 M=$(PWD) clean
	
.PHONY: modules modules_install clean

else	
MODULE_NAME := hello
RESMAIN_CORE_OBJS := tst.o
RESMAIN_GLUE_OBJS := 
$(MODULE_NAME)-objs := $(RESMAIN_GLUE_OBJS) $(RESMAIN_CORE_OBJS)   
obj-m := hello.o
endif  
	




	
