#ifndef _DEFINITIONS_H_
#define _DEFINITIONS_H_

#include "pcap.h"

#define NUM_LASERS 64
#define PACKETS_PER_LASER CYCLE_LENGTH * CYCLES_PER_LASER
#define CYCLE_LENGTH (MISC_DATA_LENGTH + CAL_DATA_LENGTH)
#define MISC_DATA_LENGTH 9
#define CAL_DATA_LENGTH 7
#define CYCLES_PER_LASER 4

//Calibration data for a laser
struct laser_params {
	u_char laser_num;
	u_short vert_correction;
	u_short rot_correction;
	u_short far_dist_correction;
	u_short dist_correction_x;
	u_short dist_correction_v;
	u_short vert_offset_correction;
	u_short horiz_offset_correction;
	u_short focal_dist;
	u_short focal_slope;
	u_char min_intensity;
	u_char max_intensity;
};

//Last 6 bytes of a packet
struct last_6_bytes{
	u_char gpsT[4];
	u_char statusType;
	u_char statusVal;
};

#endif