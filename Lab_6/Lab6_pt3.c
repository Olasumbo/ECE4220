/* ece4220lab6_isr.c
 * ECE4220/7220
 * Author: Luis Alberto Rivera
 
 Basic steps needed to configure GPIO interrupt and attach a handler.
 Check chapter 6 of the BCM2835 ARM Peripherals manual for details about
 the GPIO registers, how to configure, set, clear, etc.
 
 Note: this code is not functional. It shows some of the actual code that you need,
 but some parts are only descriptions, hints or recommendations to help you
 implement your module.
 
 You can compile your module using the same Makefile provided. Just add
 obj-m += YourModuleName.o
 */

#ifndef MODULE
#define MODULE
#endif
#ifndef __KERNEL__
#define __KERNEL__
#endif

#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/io.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/interrupt.h>
#include <linux/ktime.h>
#include <linux/delay.h>
#include <linux/hrtimer.h>
#include <linux/gpio.h>
#include <linux/fs.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/string.h>

MODULE_LICENSE("GPL");


#define MSG_SIZE 50
#define CDEV_NAME "Lab6"

/* Declare your pointers for mapping the necessary GPIO registers.
   You need to map:
   
   - Pin Event detect status register(s)
   - Rising Edge detect register(s) (either synchronous or asynchronous should work)
   - Function selection register(s)
   - Pin Pull-up/pull-down configuration registers
   
   Important: remember that the GPIO base register address is 0x3F200000, not the
   one shown in the BCM2835 ARM Peripherals manual.
*/

int mydev_id;	// variable needed to identify the handler
unsigned long * Select_pin;
unsigned long * GP_SET;
unsigned long * GP_CLEAR;
unsigned long * GP_AREN; //GPIO Pin Async. Rising Edge Detect 1
unsigned long * GP_PUD; // GPIO Pin Pull-up/down Enable
unsigned long * GP_EVENT;
unsigned long * GP_SEL0;
unsigned long * GP_SEL1;
unsigned long * GP_SEL2;

unsigned long timer_interval_ns = 1e6;	// timer interval length (nano sec part)
static struct hrtimer hr_timer; // timer structure
static int count = 0, dummy = 0;

static int devs; // same as major
static char msg[MSG_SIZE];
static char send[MSG_SIZE];


static ssize_t device_read( struct file *filp, char __user * buffer, size_t length, loff_t *offset )
{
	ssize_t dummy = copy_to_user( buffer, send, length );

//	printk( "\nSent message to user: %s", buffer );

	send[0] = '\0';

	return length;

}

static ssize_t device_write( struct file * filp, const char __user *buff, size_t len, loff_t *off )
{

	ssize_t dummy;
        char lastchar;

	if( len > MSG_SIZE )
		return -EINVAL;

	dummy = copy_from_user( msg, buff, len );

	if( len == MSG_SIZE )
	{
		msg[len-1] = '\0';
	} else {
		msg[len] = '\0';
	}

	printk( "\nMessage from user space: %s", msg );

	if( msg[0] == '@' )
	{
		//changeSound( msg[1] ); 
            switch (msg[1])
            {
              
                case 'A':
                    printk("Button 1");
                    timer_interval_ns = 4545454;
                    break;
                case 'B':
                    printk("Button 2");
                    timer_interval_ns = 4048582;
                    break;
                case 'C':
                    printk("Button 3");
                    timer_interval_ns = 3816794;
                    break;
                case 'D':
                    printk("Button 4");
                    timer_interval_ns = 300000;
                    break;
                case 'E':
                    printk("Button 5");
                    timer_interval_ns = 700000;
                    break;
	   }

	return len;

}

static struct file_operations fops =
{
        .read = device_read,
        .write = device_write,
};


// Interrupt handler function. Tha name "button_isr" can be different.
// You may use printk statements for debugging purposes. You may want to remove
// them in your final code.
static irqreturn_t button_isr(int irq, void *dev_id)
{
        unsigned long btn = *GP_EVENT & 0x1F0000 ;
	// In general, you want to disable the interrupt while handling it.
	disable_irq_nosync(79);
        

	// This same handler will be called regardless of what button was pushed,
	// assuming that they were properly configured.
	// How can you determine which button was the one actually pushed?
		
	// DO STUFF (whatever you need to do, based on the button that was pushed)

	// IMPORTANT: Clear the Event Detect status register before leaving.	
	

        switch(btn)
        {
            //The smaller the number the higher the sound
                case 65536: //(2^16)
                timer_interval_ns = 500000;
                break;
                
                case 131072: //(2^17)
                timer_interval_ns = 750000;
                break;
                
                case 262144: //(2^18)
                timer_interval_ns = 1000000;
                break;
                
                case 524288: //(2^19)
                timer_interval_ns = 1250000;
                break;
                
                case 1048576: //(2^20)
                timer_interval_ns = 1500000;
                break;
        }
        
        iowrite32( ( *GP_EVENT | 0x1F0000 ), GP_EVENT ); // This clears the event status register
        
	printk("Interrupt handled\n");	
	enable_irq(79);		// re-enable interrupt
	
    return IRQ_HANDLED;
}


//Timer callback function: this executes when the timer expires
enum hrtimer_restart timer_callback(struct hrtimer *timer_for_restart)
{
        ktime_t currtime, interval;	// time type, in nanoseconds
	unsigned long overruns = 0;

        // Re-configure the timer parameters (if needed/desired)
        currtime  = ktime_get();
        interval = ktime_set(0, timer_interval_ns); // (long sec, long nano_sec)

        // Advance the expiration time to the next interval. This returns how many
        // intervals have passed. More than 1 may happen if the system load is too high.
        overruns = hrtimer_forward(timer_for_restart, currtime, interval);

        // The following printk only executes once every 1000 cycles.
        if(dummy == 0)
        {
            //printk("Count: %d, overruns: %ld\n", ++count, overruns);
        }
        dummy = (dummy + 1)%1000;
        
        //Turn sound off/on depending an odd/even
        if((count % 2) == 1)
        {
            iowrite32( ( *GP_SET | 0x00000044 ), GP_SET );
        }
        else
	{
            iowrite32( ( *GP_CLEAR | 0x00000044 ), GP_CLEAR );
	}
	
	count++;

        return HRTIMER_RESTART;	// Return this value to restart the timer.
        // If you don't want/need a recurring timer, return
        // HRTIMER_NORESTART (and don't forward the timer).
}

int timer_init(void)
{
       // Configure and initialize timer
	ktime_t ktime = ktime_set(0, timer_interval_ns); // (long sec, long nano_sec)
	
	// CLOCK_MONOTONIC: always move forward in time, even if system time changes
	// HRTIMER_MODE_REL: time relative to current time.
	hrtimer_init(&hr_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	
	// Attach callback function to the timer
	hr_timer.function = &timer_callback;
	
        //Setup GPIO Stuff
	Select_pin = (unsigned long * ) ioremap( ( 0x3f200000 ), 4096 );
	
        //Set GP_SET to the GP_SET1 Register
        GP_SET = Select_pin + 7;
        
        //SET GP_CLEAR to the GP_CLR Register
	GP_CLEAR = Select_pin + 10;
        
         //SET GP_AREN to the Async Register
        GP_AREN = Select_pin + 31;
        
	 //SET GP_PUD to the Pull_up_down Register
        GP_PUD = Select_pin + 37;
	 
        //SET GP_EVENT to the Event Register
        GP_EVENT = Select_pin + 16;
        
        GP_SEL0 = Select_pin;
        GP_SEL1 = Select_pin + 1;
        GP_SEL2 = Select_pin + 2;

        //SET SPEAKER OUTPUT MODE to OUTPUT (001)
	iowrite32( ( *GP_SEL0 | 0x00040000 ), GP_SEL0 );                     //Pins 0-9
        
        //Set 5 Btns output modes to input (000)
        iowrite32( ( *( GP_SEL1 ) | 0x00000000 ), ( GP_SEL1 ) );     //Pins 10-18
	iowrite32( ( *( GP_SEL2 ) | 0x00000000 ), ( GP_SEL2 ) );     //Pins 19-27
        
        iowrite32( ( *GP_PUD | 0x155 ), GP_PUD );
        iowrite32( ( *( GP_PUD + 1 ) | 0x001F0000 ), ( GP_PUD + 1 ) );
        iowrite32( ( *GP_PUD & ~(0x155) ), GP_PUD );
	iowrite32( ( *( GP_PUD + 1 ) & ~(0x1F0000) ), ( GP_PUD + 1 ) );
        iowrite32( ( *GP_AREN | 0x1F0000 ), GP_AREN );
        
	//SETUP the major Characted Device
	devs = register_chrdev( 0, CDEV_NAME, &fops );
	if( devs < 0 ) 
	{
		printk( "Dead character device :( -> %d\n", devs );
		return devs;
	}
	printk( "Create char device (node) with: sudo mknod /dev/%s c %d 0\n", CDEV_NAME, devs );
        

	dummy = request_irq(79, button_isr, IRQF_SHARED, "Button_handler", &mydev_id);
        if( dummy < 0 )
	{
		printk( "Interrupt failed to register" );
	}
	//SETUP the Characted Device
	
 	
        interval = ktime_set(0, timer_interval_ns); 
        
        hrtimer_init(&hr_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
        
        hr_timer.function = &timer_callback;
	
	// Start the timer
 	hrtimer_start(&hr_timer, ktime, HRTIMER_MODE_REL);
        
	
	return 0;
}

void timer_exit(void)
{
	int ret;
  	ret = hrtimer_cancel(&hr_timer);	// cancels the timer.
        free_irq( 79, &mydev_id );
        
	unregister_chrdev( devs, CDEV_NAME );
        
  	if(ret)
            printk("The timer was still in use...\n");
	else
            printk("The timer was already canceled...\n");	// if not restarted or // canceled before
	
  	printk("HR Timer module uninstalling\n");
	
}
// Notice this alternative way to define your init_module()
// and cleanup_module(). "timer_init" will execute when you install your
// module. "timer_exit" will execute when you remove your module.
// You can give different names to those functions.
module_init(timer_init);
module_exit(timer_exit);

