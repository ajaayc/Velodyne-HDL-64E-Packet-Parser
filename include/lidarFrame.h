#ifndef _LIDAR_FRAME_H_
#define _LIDAR_FRAME_H_

#include "lidarPoint.h"
#include "definitions.h"

//Each frame should be approximately 2.035MB.
//A minute of data is about 600 frame = 1220.70 MB
class lidarFrame{
	//utime?
	vector<lidarPoint> points;
public:
	lidarFrame() {
		//To reduce time spent on memory reallocation
		points.reserve(POINTS_PER_REV);
	}

	void addPoint(lidarPoint& p) {
		points.push_back(p);
	}

	const vector<lidarPoint>* getPoints() const{
		return &points;
	}

};

#endif