
ifeq ($(KERNELRELEASE),)

K_DIR = $(shell uname -r)
PWD := $(shell pwd)
modules:
	$(MAKE) -C /lib/modules/$(K_DIR)/build M=$(PWD) modules  
	
clean:  
	make -C /lib/modules/$(K_DIR)/build M=$(PWD) clean
  
.PHONY: modules clean  
  
else
MODULE_NAME := int

obj-m := $(MODULE_NAME).o


endif  
