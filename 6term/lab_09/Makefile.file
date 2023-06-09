ifneq ($(KERNELRELEASE),)
	obj-m := my_workqueue.o
else
	CURRENT = $(shell uname -r)
	KDIR = /lib/modules/$(CURRENT)/build
	PWD = $(shell pwd)

default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
	make clean

clean:
	@rm -f *.o .*.cmd .*.flags *.mod.c *.order 
	@rm -f .*.*.cmd *~ *.*~ TODO.*
	@rm -fR .tmp*
	@rm -rf .tmp_versions

disclean: clean
	@rm *.ko *.symvers *.mod
endif 

# sudo insmod my_tasklet.ko
# lsmod | grep my_tasklet
# sudo dmesg
# sudo rmmod my_tasklet.ko
