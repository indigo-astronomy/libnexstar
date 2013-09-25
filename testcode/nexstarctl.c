

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
	
	close_telescope(dev);
}
