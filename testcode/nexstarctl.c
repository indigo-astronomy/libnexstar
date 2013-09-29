

#include <nexstar.h>
#include <stdio.h>
#include <deg2str.h>

int main(int argc, char *argv[]) {
	char nex[100];
	double d1, d2;
	double ra;
	double de;
	double lon,lat;
	
	unsigned char deg,min,sec, sign;
	dd2dms(-22.9998, &deg, &min, &sec, &sign);
	printf("%d:%d:%d, %d, %s\n",deg,min,sec,sign,dd2a(-22.9998,0));
	
	int dev = open_telescope("/dev/cu.usbserial");
	printf("res = %d\n", dev);
	int align = tc_check_align(dev);
	printf("align = %d\n", align);
	
	/*
	unsigned char i, echo;
	for (i=0; i<255; i++) {
		echo = (unsigned char) tc_echo(dev, i);
		if (echo != i) {
			printf("ERROR: Sent %d read %d\n", i, echo);
		} else {
			printf("OK: Sent %d read %d\n", i, echo);
		}
	}
	*/
	
	//printf("XXX %d\n",tc_set_location(dev, +123.999, 79.9999));
	
	int r= tc_get_location(dev, &lon, &lat);
	
	printf("lon = %f, lat = %f, res = %d\n", lon, lat, r);


	//time_t tm;
	//time(&tm);
	//printf("TTT %d\n",tc_set_time(dev, tm, +2, 1));

/*	
	printf("%s %s\n",argv[1], argv[2]);
	
	a2dd(&ra, argv[1]);
	a2dd(&de, argv[2]);
*/

	int res = tc_get_rade_p(dev, &ra, &de);
	printf("RA= %f, DE= %f, res = %d\n", ra, de, res);
		
/*	idd2nex(ra,de, nex);
	printf("%s\n",nex);
	nex2dd(nex, &d1, &d2);
	printf("%f, %f\n",d1,d2);
	
	printf("\n");
	
	dd2pnex(ra,de, nex);
	printf("%s\n",nex);
	pnex2dd(nex, &d1, &d2);
	printf("%f, %f\n",d1,d2);
*/	
	printf("%s ****",dd2a(lon,0));
	printf(" %s \n",dd2a(lat,0));

	time_t ttime;
	int tz;
	int dst;
	tc_get_time(dev, &ttime, &tz, &dst);
	
	printf("time = %s, tz = %d, dst = %d\n", ctime(&ttime), tz, dst);   
	
	/*
	res = tc_goto_rade(dev, ra-0.5, de-.5);
	
	while (tc_goto_in_progress(dev)>0) {
		sleep(1);
	}
	res = tc_get_rade_p(dev, &ra, &de);
	printf("RA= %f, DE= %f, res = %d\n", ra, de, res);
	
	res = tc_goto_rade(dev, ra, de);
	printf("RA= %f, DE= %f, res = %d\n", ra, de, res);
	*/
	
//	printf("%s ****",dh2a(ra/15));
//	printf(" %s \n",dd2a(de,0));

/*
		
	int gotop = tc_goto_in_progress(dev);
	printf("gotop = %d\n", gotop);
	
	int echo = tc_echo(dev, 'Z');
	printf("echo = %c\n", echo);
	
	int tmode=tc_get_tracking_mode(dev);
	printf("Tacking = %d\n", tmode);
	
	int model = tc_get_model(dev);
	printf("Model = %d\n", model);
	
	int tmode2 = tc_set_tracking_mode(dev, TC_TRACK_OFF);
	printf("set sacking off = %d\n", tmode2);
	
	int move = tc_slew_variable(dev, TC_AXIS_RA, TC_DIR_POSITIVE, 15*60.0 );
	printf("Moving RA+ 7 = %d\n", move);
	
	unsigned char major;
	unsigned char minor;
	tc_get_version(dev, &major, &minor);
	printf("Version %d.%d\n", major,minor);
	
	sleep(5);
//	move = tc_slew_fixed(dev, TC_AXIS_DE, TC_DIR_POSITIVE, 7);
//	printf("Moving DE- 6 = %d\n", move);
	
	sleep(5);
	move = tc_slew_fixed(dev, TC_AXIS_DE, TC_DIR_NEGATIVE, 0);
	printf("Moving RA- 0 = %d\n", move);

//	sleep(5);
//	move = tc_slew_fixed(dev, TC_AXIS_DE, TC_DIR_NEGATIVE, 0);
//	printf("Moving DE- 0 = %d\n", move);
	
	int tmode3 = tc_set_tracking_mode(dev, TC_TRACK_EQ_NORTH);
	printf("set sacking EQ = %d\n", tmode3);
*/	
	close_telescope(dev);
}
