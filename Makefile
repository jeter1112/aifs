obj-m += mymap.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm -rf user
insmod:
	sudo insmod mymap.ko
rmmod:
	sudo rmmod mymap
user: insmod
	gcc  user.c -o user
run: user
	sudo ./user
