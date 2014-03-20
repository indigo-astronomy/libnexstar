/**************************************************************
	Celestron NexStar compatible telescope control library

	(C)2013-2014 by Rumen G.Bogdanovski
***************************************************************/
#include <nexstar.h>
#include <nexstar_pec.h>


int pec_index_found(int dev) {
	char res[2];

	if (tc_pass_through_cmd(dev, 1, _TC_AXIS_RA_AZM, 0x18, 0, 0, 0, 1, res) < 0) {
		return RC_FAILED;
	}

	if (res[0] == 0) return 0;
	else return 1;
}


int pec_seek_index(int dev) {
	char res;

	if (tc_pass_through_cmd(dev, 1, _TC_AXIS_RA_AZM, 0x19, 0, 0, 0, 0, &res) < 0) {
		return RC_FAILED;
	}

	return RC_OK;
}


int pec_record(int dev, char action) {
	char res, cmd_id;

	if (action == PEC_START) {
		cmd_id = 0x0c;  /* Start PEC record */
	} else if (action == PEC_STOP) {
		cmd_id = 0x16;  /* Stop PEC record */
	} else {
		return RC_PARAMS;
	}

	if (tc_pass_through_cmd(dev, 1, _TC_AXIS_RA_AZM, cmd_id, 0, 0, 0, 0, &res) < 0) {
		return RC_FAILED;
	}

	return RC_OK;
}


int pec_record_complete(int dev) {
	char res[2];

	if (tc_pass_through_cmd(dev, 1, _TC_AXIS_RA_AZM, 0x15, 0, 0, 0, 1, res) < 0) {
		return RC_FAILED;
	}

	if (res[0] == 0) return 0;
	else return 1;
}


int pec_playback(int dev, char action) {
	char res;

	if ((action != PEC_START) && (action != PEC_STOP)) {
		return RC_PARAMS;
	}

	if (tc_pass_through_cmd(dev, 2, _TC_AXIS_RA_AZM, 0x0d, action, 0, 0, 0, &res) < 0) {
		return RC_FAILED;
	}

	return RC_OK;
}


int pec_get_playback_index(int dev) {
	char res[2];

	if (tc_pass_through_cmd(dev, 1, _TC_AXIS_RA_AZM, 0x0e, 0, 0, 0, 1, res) < 0) {
		return RC_FAILED;
	}

	return (unsigned char) res[0];
}


int pec_get_data_len(int dev) {
	char res[2];

	if (tc_pass_through_cmd(dev, 2, _TC_AXIS_RA_AZM, 0x30, 0x3f, 0, 0, 1, res) < 0) {
		return RC_FAILED;
	}

	return (unsigned char) res[0];
}


int _pec_set_data(int dev, unsigned char index, char data) {
	char res;

	if (tc_pass_through_cmd(dev, 4, _TC_AXIS_RA_AZM, 0x31, 0x40+index, data, 0, 0, &res) < 0) {
		return RC_FAILED;
	}

	return RC_OK;
}


/* on success the returned value should be threated as a signed char */
int _pec_get_data(int dev, unsigned char index) {
	char res[2];

	if (tc_pass_through_cmd(dev, 2, _TC_AXIS_RA_AZM, 0x30, 0x40+index, 0, 0, 1, res) < 0) {
		return RC_FAILED;
	}

	return (unsigned char) res[0];
}


int pec_set_data(int dev, float *data, int len) {
	return 0;
}


int pec_get_data(int dev, float *data, const int max_len) {
	return 0;
}
