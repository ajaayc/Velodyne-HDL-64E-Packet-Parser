#ifndef _LIDAR_LASER_H_
#define _LIDAR_LASER_H_

#include "definitions.h"

//Has calibration data for a laser
class lidarLaser {
public:
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
	//Maximum intensity distance
	short focal_dist;
	//Control intensity amount
	short focal_slope;
	u_char min_intensity;
	u_char max_intensity;

	//point represents a distance and intensity reading made by this laser. lidarAngle
	//is the angle of the lidar at the time when this laser was emitted
	//Returns a vector of size 3
	//Formulas obtained from paper by Naveed Muhammad and Simon Lacroix
	//WARNING: Do not call this function until all of the calibration parameters have
	//been initialized

	//If advanced calibration is true, use the algorithm from ROS-drivers, at
	// https://github.com/ros-drivers/velodyne/blob/master/velodyne_pointcloud/src/lib/rawdata.cc, which incorporates
	//distance_correction_x, distance_correction_y, focal distance, and focal slope
	vector<double> computeXYZ(const laser_point& point, double lidarAngle,bool advancedCalibration) const{
		double x, y, z;

		double beta = D2R(lidarAngle - this->computeRotCorrection());
		double theta = D2R(this->computeVertCorrection());

		double dist = point.computeDist() + this->computeDistCorr();
		double dist_xy = dist * cos(theta) - this->computeVertOffCorr() * sin(theta);

		x = dist_xy * sin(beta) - this->computeHorizOffCorr() * cos(beta);
		y = dist_xy * cos(beta) + this->computeHorizOffCorr() * sin(beta);
		z = dist * sin(theta) + this->computeVertOffCorr() * cos(theta);

		vector<double> xyz;
		xyz.push_back(x);
		xyz.push_back(y);
		xyz.push_back(z);
		return xyz;
	}

	//If advancedCalibration is true, uses the algorithm in the Velodyne manual to adjust intensity
	//otherwise, just returns point.intensity
	u_char computeIntensity(const laser_point& point, bool advancedCalibration) const{
		return point.intensity;
	}

	//Returns in degrees
	double computeVertCorrection() const{
		return vert_correction / 100.0;
	}

	//Returns in degrees
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


#endif