obj-m += hid-aero-wireless.o
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	zstd -f hid-aero-wireless.ko
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
install:
	cp $(shell pwd)/hid-aero-wireless.ko.zst /lib/modules/$(shell uname -r)/kernel/drivers/hid/
	depmod -a
uninstall:
	rm $(shell pwd)/hid-aero-wireless.zst /lib/modules/$(shell uname -r)/kernel/drivers/hid/hid-aero-wireless.ko.zst
	depmod -a
dkms:
	make -C $(INCLUDEDIR) M=$(PWD) modules