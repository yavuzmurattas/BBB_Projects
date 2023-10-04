obj-m := sysfs.o
ARCH = arm
CROSS_COMPILE = arm-linux-gnueabihf-
BBB_KERN_DIR = ~/Desktop/linux-bb-5.18-rc4/
HOST_KERN_DIR = /lib/modules/$(shell uname -r)/build/

all:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(BBB_KERN_DIR) M=$(PWD) modules

clean:
	make -C $(HOST_KERN_DIR) M=$(PWD) clean

help:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(BBB_KERN_DIR) M=$(PWD) help

host:
	make -C $(HOST_KERN_DIR) M=$(PWD) modules

dt: am335x-boneblack-sysfs.dts
	dtc -@ -I dts -O dtb -o am335x-boneblack-sysfs.dtbo am335x-boneblack-sysfs.dts
	cp am335x-boneblack-sysfs.dtbo /lib/firmware
dtb:
	cp am335x-boneblack-sysfs.dtsi $(BBB_KERN_DIR)arch/arm/boot/dts/
	cd $(BBB_KERN_DIR)
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) am335x-boneblack.dtb
	cd -

copy-dtb:
	scp $(BBB_KERN_DIR)arch/arm/boot/dts/am335x-boneblack.dtb debian@192.168.7.2:/home/debian/Desktop/sysfs 

copy-drv:
	scp *.ko  debian@192.168.7.2:/home/debian/Desktop/sysfs 

copy_sources:
	scp sysfs.c sysfs.h Makefile am335x-boneblack-sysfs.dts am335x-boneblack.dtb debian@192.168.7.2:/home/debian/Desktop/sysfs
