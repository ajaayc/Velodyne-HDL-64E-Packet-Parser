#ifndef _DEFINITIONS_H_
#define _DEFINITIONS_H_

#include "pcap.h"

#define NUM_LASERS 64
#define PACKETS_PER_LASER CYCLE_LENGTH * CYCLES_PER_LASER
#define CYCLE_LENGTH (MISC_DATA_LENGTH + CAL_DATA_LENGTH)
#define MISC_DATA_LENGTH 9
#define CAL_DATA_LENGTH 7
#define CYCLES_PER_LASER 4
#define PACKET_SIZE 1248
#define LASERS_PER_BLOCK 32
#define BLOCKS_PER_PACKET 12
#define PACKET_HEADER_LENGTH 42
// Define these due to the byte alignment causing sizeof
// to be larger than necessary
#define LASER_POINT_SIZE 3
#define LASER_BLOCK_SIZE 100 

//Calibration data for a laser
struct laser_params {
	//Made these public in order to allow memset
	//to work from outside class. Otherwise would need
	//to add a bunch of setter functions
	u_char laser_num;
	short vert_correction;
	short rot_correction;
	short far_dist_correction;
	short dist_correction_x;
	short dist_correction_v;
	short vert_offset_correction;
	short horiz_offset_correction;
	short focal_dist;
	short focal_slope;
	u_char min_intensity;
	u_char max_intensity;

	//Returns in mm
	double computeVertCorrection() const{
		return vert_correction / 100.0;
	}
	//Returns in mm? TODO: What are the units?
	double computeRotCorrection() const{
		return rot_correction / 100.0;
	}

	//Returns in cm
	double computeDistCorr() const{
		return far_dist_correction / 10.0;
	}

	//Returns in cm
	double computeDistCorrX() const{
		return dist_correction_x / 10.0;
	}

	//Returns in cm
	double computeDistCorrY() const{
		return dist_correction_v / 10.0;
	}

	//Returns in cm
	double computeVertOffCorr() const{
		return vert_offset_correction / 10.0;
	}

	//Returns in cm
	double computeHorizOffCorr() const{
		return horiz_offset_correction / 10.0;
	}

	//Returns in cm
	double computeFocDist() const{
		return focal_dist / 10.0;
	}

	//Scales accordingly
	double computeFocSlope() const{
		return focal_slope / 10.0;
	}

	u_char computeMinIntensity() const{
		return min_intensity;
	}

	u_char computeMaxIntensity() const{
		return max_intensity;
	}
};

struct l_status{
	u_char type;
	u_char value;
};

//Warning: Due to byte alignment, sizeof(laser_point) = 4, not 3
struct laser_point{
	u_short distance;
	u_char intensity;

	//Returns cm distance
	double computeDist() const{
		return distance * 2 / 10.0;
	}

	//Returns 1 byte unsigned integer
	u_char computeIntensity() const{
		return intensity;
	}

};

//Represents data extracted for a single laser block,
// which has 32 lasers
struct laser_block{
	//0xEEFF if upper block. Houses 32-63
	//0xDDFF if lower block. Houses 0-31
	u_short laser_block_id;
	u_short rotational_pos;

	laser_point laserData[LASERS_PER_BLOCK];

	//Returns angle in degrees
	double computeRotation() const{
		return rotational_pos / 100.0;
	}
};

#endif