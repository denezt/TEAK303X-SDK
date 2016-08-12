/* This is a gpio testing code. */
/* 1. Create a gpio device in /dev directory with major 95 */
/*      #mknod /dev/led c 95 0 
	2. Operation
		# w -- Write GPIO function
		# t -- Toggle LED auto running
		# ? -- help
		# q -- Quit program
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>				
#include <sys/types.h>			
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>	
#include <sys/ioctl.h>
#include <termios.h>
#include <linux/types.h>
//#include <linux/watchdog.h>
typedef	unsigned int 		u32;
typedef	unsigned short		u16;


extern int ledtimer;

/******************************************************************
* Prototype
******************************************************************/

/* The following three statements are defined in key.c */
extern int stdin_init(int);
extern int stdin_read(char *c, int fd);
void set_timer_vector(void);

char Version[]= "V1.00";

/* Start the interrupt-driven serial I/O */
main(int argc, char *argv[]){
	unsigned char c, b;
	int  datafound;
	int ret, i;
	int quit;
	unsigned char val;
	unsigned int set;
	static int status= 0;
	unsigned char buf[80];
	unsigned char *p=buf;
	int fd;
	int autoled= 0;
	
    printf("ledIO: Start LED IO Application \n");
 
	fd= open("/dev/ledio", O_RDWR);
	// Start the Net
	if(fd < 0) {
		printf("ledIO:  Can not open device driver\n");
		exit(0);	
	}
	printf("ledIO: open success fd=%X\n", fd);
	set_timer_vector();
	if(stdin_init(fileno(stdin)) < 0)
		printf("ledIO: stdin set non-blocking err\n");	
		
	printf("ledIO: device open done\n");

	quit= 0;
	datafound= 0;
	status= 0x01;
	c= b= 0;
	val= 0;
	autoled= 0;

	while(!quit) {
        c= 0;

		ret=stdin_read((char *)&c, fileno(stdin));
		//printf("ret=%d C=%X\n", ret, c);
		if( c == 0x0A) 
		    continue;
		
		if(ret > 0) {
			switch(c) {
				case '?':
					printf("LED IO operation command\n");
					printf(" w -- write LED IO function\n");
					printf(" t -- toggle for auto running\n");
					printf(" ? -- help\n");
					printf(" q -- Quit program\n");
					break;
				case 'q':
					quit= 1;
					if(status & 0x01) {
						close(fd);
						status= 0x02;
					}
					break;
      			case 0x0A:      // return code
					break;
				case 'w':   // Get gpio input value
					b ^= 0x03;
					ret=write(fd, (char *)&b, 1);
					if(ret > 0)
						printf("hex val= 0x%02X\n", b);
					else 
						printf("read err %d\n", ret);
					break;
				case 't':
					autoled ^= 1;
					if(autoled)
						printf("ledIO-ap: led ON\n");
					else
						printf("ledIO-ap: led OFF\n");
					break;
				default:
					printf("ledIO-ap: command err !\n");
				    break;

			}
		}
		if(autoled && ledtimer) {
			ledtimer= 0;
			b ^= 0x03;
			ret=write(fd, (char *)&b, 1);
		}
	}	// while loop
	return 0;
}
