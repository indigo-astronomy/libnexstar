/**************************************************************
	Celestron NexStar compatible telescope control library
	
	(C)2013 by Rumen G.Bogdanovski
***************************************************************/
#if !defined(__NEXSTAR_H)
#define __NEXSTAR_H

#define TC_TRACK_OFF 0
#define TC_TRACK_ALT_AZ 1
#define TC_TRACK_EQ_NORTH 2
#define TC_REACK_EQ_SOUTH 3
	
#define TC_DIR_POSITIVE 1
#define TC_DIR_NEGATIVE 0
	
#define TC_AXIS_RA_AZM 1
#define TC_AXIS_DE_ALT 0

#define TC_AXIS_RA TC_AXIS_RA_AZM
#define TC_AXIS_DE TC_AXIS_DE_ALT

#define TC_AXIS_AZM TC_AXIS_RA_AZM
#define TC_AXIS_ALT TC_AXIS_DE_ALT

#define _TC_DIR_POSITIVE 6
#define _TC_DIR_NEGATIVE 7
	
#define _TC_AXIS_RA_AZM 16
#define _TC_AXIS_DE_ALT 17

#define DEG2RAD (3.1415926535897932384626433832795/180.0)
#define RAD2DEG (180.0/3.1415926535897932384626433832795)

/* Telescope communication */
int open_telescope(char *dev_file);
#define close_telescope(dev_fd) (close(dev_fd))
#define write_telescope(dev_fd, buf, size) (write(dev_fd, buf, size))
int read_telescope(int devfd, char *reply, int len);

/* Telescope control */
int tc_check_align(int dev);
int tc_goto_in_progress(int dev);
int tc_goto_cancel(int dev);
int tc_echo(int dev, char ch);

/* nextar turns <=> degrees conversion */
int pnex2dd(char *nex, double *d1, double *d2);
int nex2dd(char *nex, double *d1, double *d2);
int dd2nex(double d1, double d2, char *nex);
int dd2pnex(double d1, double d2, char *nex);

#endif /* __NEXSTAR_H */