#obj-m = main.o pipe.o
#K_DIR = $(shell uname -r)
#PWD = $(shell pwd)
#KBUILD_EXTRA_SYMBOLS=~/Driver/hello/Module.symvers

	
#all:
#	make -C /lib/modules/$(K_DIR)/build M=$(PWD) modules 
	
	
#clean:
#	make -C /lib/modules/$(K_DIR)/build M=$(PWD) clean


ifeq ($(KERNELRELEASE),)

K_DIR = $(shell uname -r)
PWD := $(shell pwd)
modules:
	$(MAKE) -C /lib/modules/$(K_DIR)/build M=$(PWD) modules  
modules_install:  
	$(MAKE) -C /lib/modules/$(K_DIR)/build M=$(PWD) modules_install
	
clean:  
	make -C /lib/modules/$(K_DIR)/build M=$(PWD) clean
  
.PHONY: modules modules_install clean  
  
else
MODULE_NAME := scull
RESMAIN_CORE_OBJS := main.o
RESMAIN_GLUE_OBJS := pipe.o
$(MODULE_NAME)-objs := $(RESMAIN_GLUE_OBJS) $(RESMAIN_CORE_OBJS)   
obj-m := scull.o


endif  
