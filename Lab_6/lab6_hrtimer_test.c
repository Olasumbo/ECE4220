#ifndef MODULE
#define MODULE
#endif
#ifndef __KERNEL__
#define __KERNEL__
#endif

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>
//#include <asm/io.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/gpio.h>


#define Speaker     22      
#define Button1     27   
#define Button2     0
#define Button3     1
#define Button4     24
#define Button5     28

MODULE_LICENSE("GPL");

unsigned long timer_interval_ns = 1e6;	// timer interval length (nano sec part)
static struct hrtimer hr_timer;			// timer structure
static int count = 0, dummy = 0;

//According to Joel this makes it easier to know 
//what register you are messing with
unsigned long * Select_pin; 
unsigned long * GP_SET;
unsigned long * GP_CLEAR;

// Timer callback function: this executes when the timer expires
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
        printk("Count: %d, overruns: %ld\n", ++count, overruns);
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
	
	// Start the timer
 	hrtimer_start(&hr_timer, ktime, HRTIMER_MODE_REL);
        //Setup GPIO Stuff
	Select_pin = (unsigned long * ) ioremap( ( 0x3f200000 ), 4096 );
	
        //Set GP_SET to the GP_SET1 Register
        GP_SET = Select_pin + 7;
        
        //SET GP_CLEAR to the GP_CLR Register
	GP_CLEAR = Select_pin + 10;

	iowrite32( ( *Select_pin | 0x00040040 ), Select_pin );
	iowrite32( ( *Select_pin | 0x00000044 ), Select_pin );
	
	return 0;
}

void timer_exit(void)
{
	int ret;
  	ret = hrtimer_cancel(&hr_timer);	// cancels the timer.
  	if(ret)
		printk("The timer was still in use...\n");
	else
		printk("The timer was already canceled...\n");	// if not restarted or
														// canceled before
	
  	printk("HR Timer module uninstalling\n");
	
}
// Notice this alternative way to define your init_module()
// and cleanup_module(). "timer_init" will execute when you install your
// module. "timer_exit" will execute when you remove your module.
// You can give different names to those functions.
module_init(timer_init);
module_exit(timer_exit);

