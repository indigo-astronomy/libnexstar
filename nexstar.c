#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <termios.h>
#include <fcntl.h>
#include <string.h>

#include "deg2str.h"
#include "nexstar.h"

int open_telescope(char *dev_file) {
	int dev_fd;
	struct termios options;
	
	if ((dev_fd = open(dev_file, O_RDWR | O_NOCTTY | O_SYNC))==-1) {
		return -1;
	}
	
	memset(&options, 0, sizeof options);
	if (tcgetattr(dev_fd, &options) != 0) {
		close(dev_fd);
		return -1;
	}
	cfsetispeed(&options,B9600);
	cfsetospeed(&options,B9600);
	options.c_lflag = 0 ;
	options.c_oflag = 0;
	options.c_iflag = ~IGNBRK;
	options.c_oflag = 0;                // no remapping, no delays
	options.c_cc[VMIN]  = 0;            // read doesn't block
    options.c_cc[VTIME] = 50;            // 5 seconds read timeout
    options.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl
	options.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
                                        // enable reading
    options.c_cflag &= ~(PARENB | PARODD);      // shut off parity
    options.c_cflag |= 0;
	options.c_cflag &= ~CSTOPB;
	options.c_cflag &= ~CRTSCTS;
	
	if (tcsetattr(dev_fd,TCSANOW, &options) != 0) {
		close(dev_fd);
		return -1;
	}

	return dev_fd;
}

int read_telescope(int devfd, char *reply, int len) {
	char c;
	int res;
	int count=0;
	
	while ((res=read(devfd,&c,1)) != -1 ) {
		if ((res == 1) && (count < len)) {
			reply[count] = c;
			count++;
			if (c == '#') return count;
		} else {
			return -1;
		}
	}
	return -1;
}

int tc_check_align(int dev) {
	char reply[2];

	if (write_telescope(dev, "J", 1) < 1) return -1;
	
	if (read_telescope(dev, reply, sizeof reply) < 0) return -1;
	else return reply[0];
}

int tc_goto_in_progress(int dev) {
	char reply[2];
	if (write_telescope(dev, "L", 1) < 1) return -1;
	
	if (read_telescope(dev, reply, sizeof reply) < 0) return -1;
	
	if (reply[0]=='1') return 1;
	if (reply[0]=='0') return 0;
	
	return -1;
} 

int tc_goto_cancel(int dev) {
	char reply;
	if (write_telescope(dev, "M", 1) < 1) return -1;
	
	if (read_telescope(dev, &reply, sizeof reply) < 0) return -1;
	
	if (reply=='#') return 1;
	return -1;
} 

int tc_echo(int dev, char ch) {
	char buf[2];

	buf[0] = 'K';
	buf[1] = ch;
	if (write_telescope(dev, buf, sizeof buf) < 1) return -1;
	
	if (read_telescope(dev, buf, sizeof buf) < 0) return -1;
	else return buf[0];
}

/******************************************
 conversion:	nexstar <-> decimal degrees
 ******************************************/
int pnex2dd(char *nex, double *d1, double *d2){
	unsigned int d1_x;
	unsigned int d2_x;
	double d1_factor;
	double d2_factor;
	
	sscanf(nex, "%x,%x", &d1_x, &d2_x);
	d1_factor = d1_x / (double)0xffffffff;
	d2_factor = d2_x / (double)0xffffffff;
	*d1 = 360 * d1_factor; 
	*d2 = 360 * d2_factor;
	if (*d2 < -90) *d2 += 360;
	if (*d2 > 90) *d2 -= 360;

	return 0;
}

int nex2dd(char *nex, double *d1, double *d2){
	unsigned int d1_x;
	unsigned int d2_x;
	double d1_factor;
	double d2_factor;
	
	sscanf(nex, "%x,%x", &d1_x, &d2_x);
	d1_factor = d1_x / 65536.0;
	d2_factor = d2_x / 65536.0;
	*d1 = 360 * d1_factor; 
	*d2 = 360 * d2_factor;
	if (*d2 < -90) *d2 += 360;
	if (*d2 > 90) *d2 -= 360;

	return 0;
}

int dd2nex(double d1, double d2, char *nex) {
	d1 = d1 - 360 * floor(d1/360);
	d2 = d2 - 360 * floor(d2/360);

	if (d2 < 0) {
		d2 = d2 + 360;
	}
	
	printf("%f *** %f\n" ,d1,d2);
	
	double d2_factor = d2 / 360.0;
	double d1_factor = d1 / 360.0;
	unsigned short int nex1 = (unsigned short int)(d1_factor*65536);
	unsigned short int nex2 = (unsigned short int)(d2_factor*65536);

	sprintf(nex, "%04X,%04X", nex1, nex2);
	return 0;
}

int dd2pnex(double d1, double d2, char *nex) {
	d1 = d1 - 360 * floor(d1/360);
	d2 = d2 - 360 * floor(d2/360);

	if (d2 < 0) {
		d2 = d2 + 360;
	}

	printf("%f *** %f\n" ,d1,d2);
	
	double d2_factor = d2 / 360.0;
	double d1_factor = d1 / 360.0;
	unsigned int nex1 = (unsigned)(d1_factor*(0xffffffff));
	unsigned int nex2 = (unsigned)(d2_factor*(0xffffffff));

	sprintf(nex, "%08X,%08X", nex1, nex2);
	return 0;
}

int main(int argc, char *argv[]) {
	char nex[100];
	double d1, d2;
	double ra;
	double de;

/*	
	printf("%s %s\n",argv[1], argv[2]);
	
	a2dd(&ra, argv[1]);
	a2dd(&de, argv[2]);
	
	printf("RA= %f, DE= %f\n", ra, de);
		
	dd2nex(ra,de, nex);
	printf("%s\n",nex);
	nex2dd(nex, &d1, &d2);
	printf("%f, %f\n",d1,d2);
	
	printf("\n");
	
	dd2pnex(ra,de, nex);
	printf("%s\n",nex);
	pnex2dd(nex, &d1, &d2);
	printf("%f, %f\n",d1,d2);
	
	printf("%s ****",dd2a(d1,0));
	printf(" %s \n",dd2a(d2,0));
*/
	
	int dev = open_telescope("/dev/cu.usbserial");
	printf("res = %d\n", dev);
	int align = tc_check_align(dev);
	printf("align = %d\n", align);
	
	int gotop = tc_goto_in_progress(dev);
	printf("gotop = %d\n", gotop);
	
	int echo = tc_echo(dev, 'Z');
	printf("echo = %c\n", echo);
	
	close_telescope(dev);
}
