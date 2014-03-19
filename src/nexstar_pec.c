/**************************************************************
	Celestron NexStar compatible telescope control library

	(C)2013-2014 by Rumen G.Bogdanovski
***************************************************************/
#include <nexstar.h>
#include <nexstar_pec.h>

int pec_index_found(int dev) {
	return 0;
}

int pec_seek_index(int dev) {
	return 0;
}

int pec_record(int dev, char action) {
	return 0;
}

int pec_record_complete(int dev) {
	return 0;
}

int pec_playback(int dev, char action) {
	return 0;
}

int pec_get_playback_index(int dev) {
	return 0;
}

int pec_get_data_len(int dev) {
	return 0;
}

int _pec_set_data(int dev, unsigned char index, char data) {
	return 0;
}

int _pec_get_data(int dev, unsigned char index) {
	return 0;
}

int pec_set_data(int dev, float *data, int len) {
	return 0;
}

int pec_get_data(int dev, float *data, const int max_len) {
	return 0;
}
