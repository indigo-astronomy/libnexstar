/**************************************************************
	Celestron NexStar compatible telescope control library

	(C)2013 by Rumen G.Bogdanovski
***************************************************************/
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <termios.h>
#include <fcntl.h>
#include <string.h>

#include "deg2str.h"
#include "nexstar.h"

/*****************************************************
 Telescope communication
 *****************************************************/
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
	options.c_oflag = 0;		// no remapping, no delays
	options.c_cc[VMIN]  = 0;	// read doesn't block
	options.c_cc[VTIME] = 50;	// 5 seconds read timeout
	options.c_iflag &= ~(IXON | IXOFF | IXANY);	// shut off xon/xoff ctrl
	options.c_cflag |= (CLOCAL | CREAD);	// ignore modem controls,enable reading
	options.c_cflag &= ~(PARENB | PARODD);	// shut off parity
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

/*****************************************************
 Telescope commands
 *****************************************************/

int _tc_get_rade(int dev, double *ra, double *de, char precise) {
	char reply[18];

	if (precise) {
		if (write_telescope(dev, "e", 1) < 1) return -1;
	} else {
		if (write_telescope(dev, "E", 1) < 1) return -1;
	}

	if (read_telescope(dev, reply, sizeof reply) < 0) return -1;

	if (precise) pnex2dd(reply, ra, de);
	else nex2dd(reply, ra, de);

	return 0;
}

int _tc_get_azalt(int dev, double *az, double *alt, char precise) {
	char reply[18];

	if (precise) {
		if (write_telescope(dev, "z", 1) < 1) return -1;
	} else {
		if (write_telescope(dev, "Z", 1) < 1) return -1;
	}

	if (read_telescope(dev, reply, sizeof reply) < 0) return -1;

	if (precise) pnex2dd(reply, az, alt);
	else nex2dd(reply, az, alt);

	return 0;
}

int _tc_goto_rade(int dev, double ra, double de, char precise) {
	char nex[18];
	char reply;

	if ((ra < 0) || (ra > 360)) return -2;
	if ((de < -90) || (de > 90)) return -2;

	if (precise) {
		nex[0]='r';
		dd2pnex(ra, de, nex+1);
		if (write_telescope(dev, nex, 18) < 1) return -1;
	} else {
		nex[0]='R';
		dd2nex(ra, de, nex+1);
		if (write_telescope(dev, nex, 10) < 1) return -1;
	}

	if (read_telescope(dev, &reply, sizeof reply) < 0) return -1;

	return 0;
}

int _tc_goto_azalt(int dev, double az, double alt, char precise) {
	char nex[18];
	char reply;

	if ((az < 0) || (az > 360)) return -2;
	if ((alt < -90) || (alt > 90)) return -2;

	if (precise) {
		nex[0]='b';
		dd2pnex(az, alt, nex+1);
		if (write_telescope(dev, nex, 18) < 1) return -1;
	} else {
		nex[0]='B';
		dd2nex(az, alt, nex+1);
		if (write_telescope(dev, nex, 10) < 1) return -1;
	}

	if (read_telescope(dev, &reply, sizeof reply) < 0) return -1;

	return 0;
}

int _tc_sync_rade(int dev, double ra, double de, char precise) {
	char nex[18];
	char reply;

	if ((ra < 0) || (ra > 360)) return -2;
	if ((de < -90) || (de > 90)) return -2;

	if (precise) {
		nex[0]='s';
		dd2pnex(ra, de, nex+1);
		if (write_telescope(dev, nex, 18) < 1) return -1;
	} else {
		nex[0]='S';
		dd2nex(ra, de, nex+1);
		if (write_telescope(dev, nex, 10) < 1) return -1;
	}

	if (read_telescope(dev, &reply, sizeof reply) < 0) return -1;

	return 0;
}

int tc_check_align(int dev) {
	char reply[2];

	if (write_telescope(dev, "J", 1) < 1) return -1;

	if (read_telescope(dev, reply, sizeof reply) < 0) return -1;

	return reply[0];
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

	if (reply=='#') return 0;

	return -1;
} 

int tc_echo(int dev, char ch) {
	char buf[2];

	buf[0] = 'K';
	buf[1] = ch;
	if (write_telescope(dev, buf, sizeof buf) < 1) return -1;

	if (read_telescope(dev, buf, sizeof buf) < 0) return -1;

	return buf[0];
}

int tc_get_model(int dev) {
	char reply[2];

	if (write_telescope(dev, "m", 1) < 1) return -1;

	if (read_telescope(dev, reply, sizeof reply) < 0) return -1;

	return reply[0];
}

int tc_get_version(int dev, char *major, char *minor) {
	char reply[3];

	if (write_telescope(dev, "V", 1) < 1) return -1;

	if (read_telescope(dev, reply, sizeof reply) < 0) return -1;

	*major = reply[0];
	*minor = reply[1];
	return reply[0] << 8 + reply[1];
}

int tc_get_tracking_mode(int dev) {
	char reply[2];

	if (write_telescope(dev, "t", 1) < 1) return -1;

	if (read_telescope(dev, reply, sizeof reply) < 0) return -1;

	return reply[0];
}

int tc_set_tracking_mode(int dev, char mode) {
	char cmd[2];
	char res;
	if ((mode < 0) || (mode > 3)) return -1;

	cmd[0] = 'T';
	cmd[1] = mode;

	if (write_telescope(dev, cmd, sizeof cmd) < 1) return -1;

	if (read_telescope(dev, &res, sizeof res) < 0) return -1;

	return 0;
}

int tc_slew_fixed(int dev, char axis, char direction, char rate) {
	char cmd[8];
	char res;

	cmd[0] = 'P';
	cmd[1] = 2;

	if (axis > 0) cmd[2] = _TC_AXIS_RA_AZM;
	else cmd[2] = _TC_AXIS_DE_ALT;

	if (direction > 0) cmd[3] = _TC_DIR_POSITIVE + 30;
	else cmd[3] = _TC_DIR_NEGATIVE + 30;

	if ((rate < 0) || (rate > 9)) return -1;

	cmd[4] = rate;
	cmd[5] = 0;
	cmd[6] = 0;
	cmd[7] = 0;

	if (write_telescope(dev, cmd, sizeof cmd) < 1) return -1;

	if (read_telescope(dev, &res, sizeof res) < 0) return -1;

	return 0;
}

int tc_slew_variable(int dev, char axis, char direction, float rate) {
	char cmd[8];
	char res;

	cmd[0] = 'P';
	cmd[1] = 3;

	if (axis > 0) cmd[2] = _TC_AXIS_RA_AZM;
	else cmd[2] = _TC_AXIS_DE_ALT;

	if (direction > 0) cmd[3] = _TC_DIR_POSITIVE;
	else cmd[3] = _TC_DIR_NEGATIVE;

	int16_t irate = (int)(4*rate);
	unsigned char rateH = (char)(irate / 256);
	unsigned char rateL = (char)(irate % 256);
	//printf("rateH = %d, rateL = %d , irate = %d\n", rateH, rateL, irate);

	cmd[4] = rateH;
	cmd[5] = rateL;
	cmd[6] = 0;
	cmd[7] = 0;

	if (write_telescope(dev, cmd, sizeof cmd) < 1) return -1;

	if (read_telescope(dev, &res, sizeof res) < 0) return -1;

	return 0;
}

int tc_get_location(int dev, double *lon, double *lat) {
	unsigned char reply[9];

	if (write_telescope(dev, "w", 1) < 1) return -1;

	if (read_telescope(dev, reply, sizeof reply) < 0) return -1;

	*lat = (double)reply[0] + reply[1]/60.0 + reply[2]/3600.0;
	*lon = (double)reply[4] + reply[5]/60.0 + reply[6]/3600.0;
	if (reply[3]) {
		*lat *= -1;
	}
	if (reply[7]) {
		*lon *= -1;
	}
	return 0;
}

int tc_set_location(int dev, double lon, double lat) {
	unsigned char cmd[9];
	char res;
	unsigned char deg, min, sec, sign;

	cmd[0] = 'W';
	dd2dms(lat, &deg, &min, &sec, &sign);
	if (deg > 90) {
		return -2;
	}
	cmd[1] = deg;
	cmd[2] = min;
	cmd[3] = sec;
	cmd[4] = sign;

	dd2dms(lon, &deg, &min, &sec, &sign);
	if (deg > 180) {
		return -2;
	}
	cmd[5] = deg;
	cmd[6] = min;
	cmd[7] = sec;
	cmd[8] = sign;

	if (write_telescope(dev, cmd, sizeof cmd) < 1) return -1;

	if (read_telescope(dev, &res, sizeof res) < 0) return -1;

	return 0;
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
