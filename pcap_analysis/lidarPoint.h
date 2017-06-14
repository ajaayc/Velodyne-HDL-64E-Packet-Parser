#ifndef _LIDAR_POINT_H_
#define _LIDAR_POINT_H_

#include "pcap.h"
#include <vector>

//12 byte point within in a lidar revolution
class lidarPoint{
public:
	float xyz[3];
	u_char intensity;

public:
	lidarPoint(float x, float y, float z, u_char intensity) : intensity(intensity){
		xyz[0] = x;
		xyz[1] = y;
		xyz[2] = z;
	}
	
	lidarPoint(const vector<float>& xyz_, u_char intensity) : intensity(intensity){
		xyz[0] = xyz_[0];
		xyz[1] = xyz_[1];
		xyz[2] = xyz_[2];
	}

};

#endif