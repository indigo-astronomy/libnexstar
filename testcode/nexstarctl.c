

#include <nexstar.h>
#include <stdio.h>

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
	
	close_telescope(dev);
}
