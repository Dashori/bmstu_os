CURRENT = $(shell uname -r)
KDIR = /lib/modules/$(CURRENT)/build
PWD = $(shell pwd)
TARGET = seqfile
#TARGET = fortune
obj-m := $(TARGET).o

default :
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean :
	@rm -f *.o .*.cmd .*.flags *.mod.c *.order Module.*
	@rm -f .*.*.cmd *~ *.*~ TODO.*
	@rm -fR .tmp*
	@rm -rf .tmp_versions

disclean : clean
	@rm *.ko *.symvers *.mod
