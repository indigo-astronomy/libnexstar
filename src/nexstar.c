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

#include <time.h>

/*****************************************************
 Telescope communication
 *****************************************************/
int open_telescope(char *dev_file) {
	int dev_fd;
	struct termios options;

	if ((dev_fd = open(dev_file, O_RDWR | O_NOCTTY | O_SYNC))==-1) {
		return RC_FAILED;
	}

	memset(&options, 0, sizeof options);
	if (tcgetattr(dev_fd, &options) != 0) {
		close(dev_fd);
		return RC_FAILED;
	}

	cfsetispeed(&options,B9600);
	cfsetospeed(&options,B9600);
	/* Finaly!!!!  Works on Linux & Solaris  */
	options.c_lflag &= ~(ICANON|ECHO|ECHOE|ISIG|IEXTEN);
	options.c_oflag &= ~(OPOST);
	options.c_iflag &= ~(INLCR|ICRNL|IXON|IXOFF|IXANY|IMAXBEL);
	options.c_cflag &= ~PARENB;
	options.c_cflag &= ~CSTOPB;
	options.c_cflag &= ~CSIZE;
	options.c_cflag |= CS8;
	options.c_cc[VMIN]  = 0;	// read doesn't block
	options.c_cc[VTIME] = 50;	// 5 seconds read timeout

	if (tcsetattr(dev_fd,TCSANOW, &options) != 0) {
		close(dev_fd);
		return RC_FAILED;
	}

	return dev_fd;
}

int read_telescope(int devfd, char *reply, int len) {
	char c;
	int res;
	int count=0;

	while ((count < len) && ((res=read(devfd,&c,1)) != -1 )) {
		if (res == 1) {
			reply[count] = c;
			count++;
			//printf("R: %d, C:%d\n", (unsigned char)reply[count-1], count);
		} else {
			return RC_FAILED;
		}
	}
	if (c == '#') return count;
	return RC_FAILED;
}

/*****************************************************
 Telescope commands
 *****************************************************/
int tc_pass_through_cmd(int dev, char msg_len, char dest_id, char cmd_id,
                        char data1, char data2, char data3, char res_len, char *response) {
	char cmd[8];

	cmd[0] = 'P';
	cmd[1] = msg_len;
	cmd[2] = dest_id;
	cmd[3] = cmd_id;
	cmd[4] = data1;
	cmd[5] = data2;
	cmd[6] = data3;
	cmd[7] = res_len;

	if (write_telescope(dev, cmd, sizeof cmd) < 1) return RC_FAILED;

	/* must read res_len + 1 bytes to accomodate "#" at the end */
	if (read_telescope(dev, response, res_len + 1) < 0) return RC_FAILED;

	return RC_OK;
}

int _tc_get_rade(int dev, double *ra, double *de, char precise) {
	char reply[18];

	if (precise) {
		if (write_telescope(dev, "e", 1) < 1) return RC_FAILED;
	} else {
		if (write_telescope(dev, "E", 1) < 1) return RC_FAILED;
	}

	if (read_telescope(dev, reply, sizeof reply) < 0) return RC_FAILED;

	if (precise) pnex2dd(reply, ra, de);
	else nex2dd(reply, ra, de);

	return RC_OK;
}

int _tc_get_azalt(int dev, double *az, double *alt, char precise) {
	char reply[18];

	if (precise) {
		if (write_telescope(dev, "z", 1) < 1) return RC_FAILED;
	} else {
		if (write_telescope(dev, "Z", 1) < 1) return RC_FAILED;
	}

	if (read_telescope(dev, reply, sizeof reply) < 0) return RC_FAILED;

	if (precise) pnex2dd(reply, az, alt);
	else nex2dd(reply, az, alt);

	return RC_OK;
}

int _tc_goto_rade(int dev, double ra, double de, char precise) {
	char nex[18];
	char reply;

	if ((ra < 0) || (ra > 360)) return RC_PARAMS;
	if ((de < -90) || (de > 90)) return RC_PARAMS;

	if (precise) {
		nex[0]='r';
		dd2pnex(ra, de, nex+1);
		if (write_telescope(dev, nex, 18) < 1) return RC_FAILED;
	} else {
		nex[0]='R';
		dd2nex(ra, de, nex+1);
		if (write_telescope(dev, nex, 10) < 1) return RC_FAILED;
	}

	if (read_telescope(dev, &reply, sizeof reply) < 0) return RC_FAILED;

	return RC_OK;
}

int _tc_goto_azalt(int dev, double az, double alt, char precise) {
	char nex[18];
	char reply;

	if ((az < 0) || (az > 360)) return RC_PARAMS;
	if ((alt < -90) || (alt > 90)) return RC_PARAMS;

	if (precise) {
		nex[0]='b';
		dd2pnex(az, alt, nex+1);
		if (write_telescope(dev, nex, 18) < 1) return RC_FAILED;
	} else {
		nex[0]='B';
		dd2nex(az, alt, nex+1);
		if (write_telescope(dev, nex, 10) < 1) return RC_FAILED;
	}

	if (read_telescope(dev, &reply, sizeof reply) < 0) return RC_FAILED;

	return RC_OK;
}

int _tc_sync_rade(int dev, double ra, double de, char precise) {
	char nex[18];
	char reply;

	if ((ra < 0) || (ra > 360)) return RC_PARAMS;
	if ((de < -90) || (de > 90)) return RC_PARAMS;

	if (precise) {
		nex[0]='s';
		dd2pnex(ra, de, nex+1);
		if (write_telescope(dev, nex, 18) < 1) return RC_FAILED;
	} else {
		nex[0]='S';
		dd2nex(ra, de, nex+1);
		if (write_telescope(dev, nex, 10) < 1) return RC_FAILED;
	}

	if (read_telescope(dev, &reply, sizeof reply) < 0) return RC_FAILED;

	return RC_OK;
}

int tc_check_align(int dev) {
	char reply[2];

	if (write_telescope(dev, "J", 1) < 1) return RC_FAILED;

	if (read_telescope(dev, reply, sizeof reply) < 0) return RC_FAILED;

	return reply[0];
}

int tc_goto_in_progress(int dev) {
	char reply[2];

	if (write_telescope(dev, "L", 1) < 1) return RC_FAILED;

	if (read_telescope(dev, reply, sizeof reply) < 0) return RC_FAILED;

	if (reply[0]=='1') return 1;
	if (reply[0]=='0') return 0;

	return RC_FAILED;
} 

int tc_goto_cancel(int dev) {
	char reply;
	if (write_telescope(dev, "M", 1) < 1) return RC_FAILED;

	if (read_telescope(dev, &reply, sizeof reply) < 0) return RC_FAILED;

	if (reply=='#') return RC_OK;

	return RC_FAILED;
} 

int tc_echo(int dev, char ch) {
	char buf[2];

	buf[0] = 'K';
	buf[1] = ch;
	if (write_telescope(dev, buf, sizeof buf) < 1) return RC_FAILED;

	if (read_telescope(dev, buf, sizeof buf) < 0) return RC_FAILED;

	return buf[0];
}

int tc_get_model(int dev) {
	char reply[2];

	if (write_telescope(dev, "m", 1) < 1) return RC_FAILED;

	if (read_telescope(dev, reply, sizeof reply) < 0) return RC_FAILED;

	return reply[0];
}

int tc_get_version(int dev, char *major, char *minor) {
	char reply[3];

	if (write_telescope(dev, "V", 1) < 1) return RC_FAILED;

	if (read_telescope(dev, reply, sizeof reply) < 0) return RC_FAILED;

	*major = reply[0];
	*minor = reply[1];
	return (reply[0] << 8) + reply[1];
}

int tc_get_tracking_mode(int dev) {
	char reply[2];

	if (write_telescope(dev, "t", 1) < 1) return RC_FAILED;

	if (read_telescope(dev, reply, sizeof reply) < 0) return RC_FAILED;

	return reply[0];
}

int tc_set_tracking_mode(int dev, char mode) {
	char cmd[2];
	char res;
	if ((mode < 0) || (mode > 3)) return RC_FAILED;

	cmd[0] = 'T';
	cmd[1] = mode;

	if (write_telescope(dev, cmd, sizeof cmd) < 1) return RC_FAILED;

	if (read_telescope(dev, &res, sizeof res) < 0) return RC_FAILED;

	return RC_OK;
}

int tc_slew_fixed(int dev, char axis, char direction, char rate) {
	char axis_id, cmd_id, res;

	if (axis > 0) axis_id = _TC_AXIS_RA_AZM;
	else axis_id = _TC_AXIS_DE_ALT;

	if (direction > 0) cmd_id = _TC_DIR_POSITIVE + 30;
	else cmd_id = _TC_DIR_NEGATIVE + 30;

	if ((rate < 0) || (rate > 9)) return RC_PARAMS;

	return tc_pass_through_cmd(dev, 2, axis_id, cmd_id, rate, 0, 0, 0, &res);
}

int tc_slew_variable(int dev, char axis, char direction, float rate) {
	char axis_id, cmd_id, res;

	if (axis > 0) axis_id = _TC_AXIS_RA_AZM;
	else axis_id = _TC_AXIS_DE_ALT;

	if (direction > 0) cmd_id = _TC_DIR_POSITIVE;
	else cmd_id = _TC_DIR_NEGATIVE;

	int16_t irate = (int)(4*rate);
	unsigned char rateH = (char)(irate / 256);
	unsigned char rateL = (char)(irate % 256);
	//printf("rateH = %d, rateL = %d , irate = %d\n", rateH, rateL, irate);

	return tc_pass_through_cmd(dev, 3, axis_id, cmd_id, rateH, rateL, 0, 0, &res);
}

int tc_get_location(int dev, double *lon, double *lat) {
	unsigned char reply[9];

	if (write_telescope(dev, "w", 1) < 1) return RC_FAILED;

	if (read_telescope(dev, (char *)reply, sizeof reply) < 0) return RC_FAILED;

	*lat = (double)reply[0] + reply[1]/60.0 + reply[2]/3600.0;
	*lon = (double)reply[4] + reply[5]/60.0 + reply[6]/3600.0;
	if (reply[3]) {
		*lat *= -1;
	}
	if (reply[7]) {
		*lon *= -1;
	}
	return RC_OK;
}

int tc_set_location(int dev, double lon, double lat) {
	unsigned char cmd[9];
	char res;
	unsigned char deg, min, sec, sign;

	cmd[0] = 'W';
	dd2dms(lat, &deg, &min, &sec, (char *)&sign);
	if (deg > 90) {
		return RC_PARAMS;
	}
	cmd[1] = deg;
	cmd[2] = min;
	cmd[3] = sec;
	cmd[4] = sign;

	dd2dms(lon, &deg, &min, &sec, (char *)&sign);
	if (deg > 180) {
		return RC_PARAMS;
	}
	cmd[5] = deg;
	cmd[6] = min;
	cmd[7] = sec;
	cmd[8] = sign;

	if (write_telescope(dev, cmd, sizeof cmd) < 1) return RC_FAILED;

	if (read_telescope(dev, &res, sizeof res) < 0) return RC_FAILED;

	return RC_OK;
}

time_t tc_get_time(int dev, time_t *ttime, int *tz, int *dst) {
	char reply[9];
	struct tm tms;

	if (write_telescope(dev, "h", 1) < 1) return RC_FAILED;

	if (read_telescope(dev, reply, sizeof reply) < 0) return RC_FAILED;

	tms.tm_hour = reply[0];
	tms.tm_min = reply[1];
	tms.tm_sec = reply[2];
	tms.tm_mon = reply[3] - 1;
	tms.tm_mday = reply[4];
	tms.tm_year = 100 + reply[5];
	/* tms.tm_isdst = reply[7]; */
	/* set tm_isdst to -1 and leave mktime to set it,
	   but we use the one returned by the telescope
	   this way we avoid 1 hour error if dst is
	   not set correctly on the telescope.
	*/
	tms.tm_isdst = -1;
	*tz = (int)reply[6];
	if (*tz > 12) *tz -= 256;
	*dst = reply[7];

	*ttime = mktime(&tms);
	return *ttime;
}

int tc_set_time(char dev, time_t ttime, int tz, int dst) {
	unsigned char cmd[9];
	struct tm tms;
	char res;
	int model;
	int timezone;
	time_t utime;

	timezone = tz;
	if (tz < 0) tz += 256;

	if (dst) dst = 1;
	else dst=0;

	localtime_r(&ttime, &tms);

	cmd[0] = 'H';
	cmd[1] = (unsigned char)tms.tm_hour;
	cmd[2] = (unsigned char)tms.tm_min;
	cmd[3] = (unsigned char)tms.tm_sec;
	cmd[4] = (unsigned char)(tms.tm_mon + 1);
	cmd[5] = (unsigned char)tms.tm_mday;
	// year is actual_year-1900
	// here is required actual_year-2000
	// so we need year-100
	cmd[6] = (unsigned char)(tms.tm_year - 100);
	cmd[7] = (unsigned char)tz;
	cmd[8] = (unsigned char)dst;

	if (write_telescope(dev, cmd, sizeof cmd) < 1) return RC_FAILED;

	if (read_telescope(dev, &res, sizeof res) < 0) return RC_FAILED;

	model = tc_get_model(dev);
	if (model <= 0) return model;

	/* If the mount has RTC set date/time to RTC too */
	/* I only know CGE(5) and AdvancedVX(20) to have RTC */
	if ((model = 5) || (model = 20)) {
		/* calculate UT from localtime */
		utime = ttime - ((timezone + dst) * 3600);
		localtime_r(&utime, &tms);

		/* set year */
		if (tc_pass_through_cmd(dev, 3, 178, 132,
		                       (unsigned char)((tms.tm_year + 1900) / 256),
		                       (unsigned char)((tms.tm_year + 1900) % 256),
		                       0, 0, &res)) {
			return RC_FAILED;
		}
		/* set month and day */
		if (tc_pass_through_cmd(dev, 3, 178, 131,
		                       (unsigned char)(tms.tm_mon + 1),
		                       (unsigned char)tms.tm_mday,
		                       0, 0, &res)) {
			return RC_FAILED;
		}
		/* set time */
		if (tc_pass_through_cmd(dev, 4, 178, 179,
		                       (unsigned char)tms.tm_hour,
		                       (unsigned char)tms.tm_min,
		                       (unsigned char)tms.tm_sec,
		                       0, &res)) {
			return RC_FAILED;
		}
	}
	return RC_OK;
}

char *get_model_name(int id, char *name, int len) {
	switch(id) {
	case 1:
		strncpy(name,"NexStar GPS Series",len);
		return name;
	case 3:
		strncpy(name,"NexStar i-Series",len);
		return name;
	case 4:
		strncpy(name,"NexStar i-Series SE",len);
		return name;
	case 5:
		strncpy(name,"CGE",len);
		return name;
	case 6:
		strncpy(name,"Advanced GT",len);
		return name;
	case 7:
		strncpy(name,"SLT",len);
		return name;
	case 9:
		strncpy(name,"CPC",len);
		return name;
	case 10:
		strncpy(name,"GT",len);
		return name;
	case 11:
		strncpy(name,"NexStar 4/5 SE",len);
		return name;
	case 12:
		strncpy(name,"NexStar 6/8 SE",len);
		return name;
	case 14:
		strncpy(name,"CGEM",len);
		return name;
	case 20:
		strncpy(name,"Advanced VX",len);
		return name;
	default:
		name[0]='\0';
		return NULL;
	}
	return NULL;
}

/******************************************************************
 The following commands are not officially documented by Celestron.
 They are reverse engineered, more information can be found here:
 http://www.paquettefamily.ca/nexstar/NexStar_AUX_Commands_10.pdf
 ******************************************************************/
/*
int tc_get_guide_rate() {
	return RC_PARAMS;
}
int tc_set_guide_rate_fixed() {
	return RC_PARAMS;
}
int tc_set_guide_rate() {
	return RC_PARAMS;
}
*/
int tc_get_autoguide_rate(int dev, char axis) {
	char cmd[8];
	char res[2];

	cmd[0] = 'P';
	cmd[1] = 2;

	if (axis > 0) cmd[2] = _TC_AXIS_RA_AZM;
	else cmd[2] = _TC_AXIS_DE_ALT;

	cmd[3] = 0x47; /* Get autoguide rate */
	cmd[4] = 0;
	cmd[5] = 0;
	cmd[6] = 0;
	cmd[7] = 1;

	if (write_telescope(dev, cmd, sizeof cmd) < 1) return RC_FAILED;

	if (read_telescope(dev, res, sizeof res) < 0) return RC_FAILED;

	unsigned char rate = (unsigned char)(100 * (unsigned char)res[0] / 256);
	//printf("res[0] = %d, rate = %d\n", (unsigned char) res[0], rate);

	return rate;
}

int tc_set_autoguide_rate(int dev, char axis, char rate) {
	char cmd[8];
	char res;
	unsigned char rrate;

	cmd[0] = 'P';
	cmd[1] = 2;

	if (axis > 0) cmd[2] = _TC_AXIS_RA_AZM;
	else cmd[2] = _TC_AXIS_DE_ALT;

	cmd[3] = 0x46;  /* Set autoguide rate */

	/* rate should be [0%-99%] */
	if ((rate < 0) || (rate > 99)) return RC_PARAMS;

	/* This is wired, but is done to match as good as
	   possible the values given by the HC */
	if (rate == 0) rrate = 0;
	else if (rate == 99) rrate = 255;
	else rrate = (unsigned char)(256 * rate / 100) + 1;
	//printf("rate = %d, rrate = %d\n", rate, rrate);

	cmd[4] = rrate;
	cmd[5] = 0;
	cmd[6] = 0;
	cmd[7] = 0;

	if (write_telescope(dev, cmd, sizeof cmd) < 1) return RC_FAILED;

	if (read_telescope(dev, &res, sizeof res) < 0) return RC_FAILED;

	return RC_OK;
}

int tc_get_backlash(int dev, char axis, char direction) {
	char cmd[8];
	char res[2];

	cmd[0] = 'P';
	cmd[1] = 2;

	if (axis > 0) cmd[2] = _TC_AXIS_RA_AZM;
	else cmd[2] = _TC_AXIS_DE_ALT;

	if (direction > 0) cmd[3] = 0x40;  /* Get positive backlash */
	else cmd[3] = 0x41; /* Get negative backlash */

	cmd[4] = 0;
	cmd[5] = 0;
	cmd[6] = 0;
	cmd[7] = 1;

	if (write_telescope(dev, cmd, sizeof cmd) < 1) return RC_FAILED;

	if (read_telescope(dev, res, sizeof res) < 0) return RC_FAILED;

	return (unsigned char) res[0];
}

int tc_set_backlash(int dev, char axis, char direction, char backlash) {
	char cmd[8];
	char res;

	cmd[0] = 'P';
	cmd[1] = 2;

	if (axis > 0) cmd[2] = _TC_AXIS_RA_AZM;
	else cmd[2] = _TC_AXIS_DE_ALT;

	if (direction > 0) cmd[3] = 0x10;  /* Set positive backlash */
	else cmd[3] = 0x11; /* Set negative backlash */

	/* backlash should be [0-99] */
	if ((backlash < 0) || (backlash > 99)) return RC_PARAMS;
	//printf("backlash = %d\n", backlash);

	cmd[4] = backlash;
	cmd[5] = 0;
	cmd[6] = 0;
	cmd[7] = 0;

	if (write_telescope(dev, cmd, sizeof cmd) < 1) return RC_FAILED;

	if (read_telescope(dev, &res, sizeof res) < 0) return RC_FAILED;

	return RC_OK;
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
	if (*d2 < -90.0001) *d2 += 360;
	if (*d2 > 90.0001) *d2 -= 360;

	return RC_OK;
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
	if (*d2 < -90.0001) *d2 += 360;
	if (*d2 > 90.0001) *d2 -= 360;

	return RC_OK;
}

int dd2nex(double d1, double d2, char *nex) {
	d1 = d1 - 360 * floor(d1/360);
	d2 = d2 - 360 * floor(d2/360);

	if (d2 < 0) {
		d2 = d2 + 360;
	}
	//printf("%f *** %f\n" ,d1,d2);
	
	double d2_factor = d2 / 360.0;
	double d1_factor = d1 / 360.0;
	unsigned short int nex1 = (unsigned short int)(d1_factor*65536);
	unsigned short int nex2 = (unsigned short int)(d2_factor*65536);

	sprintf(nex, "%04X,%04X", nex1, nex2);
	return RC_OK;
}

int dd2pnex(double d1, double d2, char *nex) {
	d1 = d1 - 360 * floor(d1/360);
	d2 = d2 - 360 * floor(d2/360);

	if (d2 < 0) {
		d2 = d2 + 360;
	}
	//printf("%f *** %f\n" ,d1,d2);
	
	double d2_factor = d2 / 360.0;
	double d1_factor = d1 / 360.0;
	unsigned int nex1 = (unsigned)(d1_factor*(0xffffffff));
	unsigned int nex2 = (unsigned)(d2_factor*(0xffffffff));

	sprintf(nex, "%08X,%08X", nex1, nex2);
	return RC_OK;
}
