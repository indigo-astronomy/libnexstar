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

/* return codes */
#define RC_OK 0			/* success */
#define RC_FAILED (-1)	/* general error */
#define RC_PARAMS (-2)	/* invalid parameters */

#define DEG2RAD (3.1415926535897932384626433832795/180.0)
#define RAD2DEG (180.0/3.1415926535897932384626433832795)

#include <time.h>
#include <unistd.h>

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

/* Telescope communication */
int open_telescope(char *dev_file);
#define close_telescope(dev_fd) (close(dev_fd))
#define write_telescope(dev_fd, buf, size) (write(dev_fd, buf, size))
int read_telescope(int devfd, char *reply, int len);

/* Telescope commands */
int tc_pass_through_cmd(int dev, char msg_len, char dest_id, char cmd_id,
                        char data1, char data2, char data3, char res_len, char *response);
int tc_check_align(int dev);
int tc_goto_in_progress(int dev);
int tc_goto_cancel(int dev);
int tc_echo(int dev, char ch);
int tc_get_model(int dev);
int tc_get_version(int dev, char *major, char *minor);
int tc_get_tracking_mode(int dev);
int tc_set_tracking_mode(int dev, char mode);

int _tc_get_rade(int dev, double *ra, double *de, char precise);
#define tc_get_rade(dev, ra, de) (_tc_get_rade(dev, ra, de, 0))
#define tc_get_rade_p(dev, ra, de) (_tc_get_rade(dev, ra, de, 1))

int _tc_get_azalt(int dev, double *az, double *alt, char precise);
#define tc_get_azalt(dev, az, alt) (_tc_get_azalt(dev, az, alt, 0))
#define tc_get_azalt_p(dev, az, alt) (_tc_get_azalt(dev, az, alt, 1))

int _tc_goto_rade(int dev, double ra, double de, char precise);
#define tc_goto_rade(dev, ra, de) (_tc_goto_rade(dev, ra, de, 0))
#define tc_goto_rade_p(dev, ra, de) (_tc_goto_rade(dev, ra, de, 1))

int _tc_goto_azalt(int dev, double az, double alt, char precise);
#define tc_goto_azalt(dev, az, alt) (_tc_goto_azalt(dev, az, alt, 0))
#define tc_goto_azalt_p(dev, az, alt) (_tc_goto_azalt(dev, az, alt, 1))

int _tc_sync_rade(int dev, double ra, double de, char precise);
#define tc_sync_rade(dev, ra, de) (_tc_sync_rade(dev, ra, de, 0))
#define tc_sync_rade_p(dev, ra, de) (_tc_sync_rade(dev, ra, de, 1))

int tc_get_location(int dev, double *lon, double *lat);
int tc_set_location(int dev, double lon, double lat);

time_t tc_get_time(int dev, time_t *ttime, int *tz, int *dst);
int tc_set_time(char dev, time_t ttime, int tz, int dst);

int tc_slew_fixed(int dev, char axis, char direction, char rate);
int tc_slew_variable(int dev, char axis, char direction, float rate);

char *get_model_name(int id, char *name, int len);

/* Reverse engineered commands */
/*
int tc_get_guide_rate();
int tc_set_guide_rate_fixed();
int tc_set_guide_rate();
*/
int tc_get_autoguide_rate(int dev, char axis);
int tc_set_autoguide_rate(int dev, char axis, char rate);
int tc_get_backlash(int dev, char axis, char direction);
int tc_set_backlash(int dev, char axis, char direction, char backlash);
/* End of reverse engineered commands */

/* nextar turns <=> degrees conversion */
int pnex2dd(char *nex, double *d1, double *d2);
int nex2dd(char *nex, double *d1, double *d2);
int dd2nex(double d1, double d2, char *nex);
int dd2pnex(double d1, double d2, char *nex);

#ifdef __cplusplus /* If this is a C++ compiler, end C linkage */
}
#endif

#endif /* __NEXSTAR_H */
