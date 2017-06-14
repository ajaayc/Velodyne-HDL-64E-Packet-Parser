#ifndef _DEFINITIONS_H_
#define _DEFINITIONS_H_

#include <vector>
#include "pcap.h"

#define NUM_LASERS 64
#define PACKETS_PER_LASER (CYCLE_LENGTH * CYCLES_PER_LASER)
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
#define POINTS_PER_REV 133333

#define PI 3.1415926535

#define D2R(x) ((x) * PI / 180.0)
#define R2D(x) ((x) * 180.0 / PI)

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

struct l_status{
	u_char type;
	u_char value;
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
