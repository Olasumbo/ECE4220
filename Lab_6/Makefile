#obj-m += lab6_hrtimer_test.o
#obj-m += lab6_kthread_test.o
#obj-m += Lab6_Interrupt.o
obj-m += Lab6_pt3.o


all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean