#ifndef _LIDAR_LASER_H_
#define _LIDAR_LASER_H_

#include "definitions.h"

//Has calibration data for a laser
//TODO: Make an abstract superclass with lidarLaser's data being private to protect them
//from the class itself
//TODO: If getter functions are too slow in dividing, then cache values
//TODO: Make a constructor to encapsulate members for safety
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
	//Returns a vector of size 3 floats (12 B total)
	//Formulas obtained from paper by Naveed Muhammad and Simon Lacroix
	//WARNING: Do not call this function until all of the calibration parameters have
	//been initialized

	//If advanced calibration is true, use the algorithm from ROS-drivers, at
	// https://github.com/ros-drivers/velodyne/blob/master/velodyne_pointcloud/src/lib/rawdata.cc, which incorporates
	//distance_correction_x, distance_correction_y, focal distance, and focal slope.
	vector<float> computeXYZ(const laser_point& point, double lidarAngle,bool advancedCalibration) const{
		double x, y, z;

		double beta = D2R(lidarAngle - this->computeRotCorrection());
		double theta = D2R(this->computeVertCorrection());

		double dist = point.computeDist() + this->computeDistCorr();

		double dist_xy = dist * cos(theta) - this->computeVertOffCorr() * sin(theta);

		x = dist_xy * sin(beta) - this->computeHorizOffCorr() * cos(beta);
		y = dist_xy * cos(beta) + this->computeHorizOffCorr() * sin(beta);

		double x_distance = dist, y_distance = dist;
		if (advancedCalibration){
			x = abs(x);
			y = abs(y);

			double dist_corr_x = 0, dist_corr_y = 0;
			dist_corr_x = (this->computeDistCorr() - this->computeDistCorrX()) * (x - 2.4) / (25.04 - 2.4) + this->computeDistCorrX();
			dist_corr_x -= this->computeDistCorr();

			dist_corr_y = (this->computeDistCorr() - this->computeDistCorrY()) * (y - 1.93) / (25.04 - 1.93) + this->computeDistCorrY();
			dist_corr_y -= this->computeDistCorr();

			//Recompute distxy, x,y with these two calibration offsets
			x_distance = point.computeDist() + dist_corr_x;
			dist_xy = x_distance * cos(theta) - this->computeVertOffCorr() * sin(theta);
			x = dist_xy * sin(beta) - this->computeHorizOffCorr() * cos(beta);

			y_distance = point.computeDist() + dist_corr_y;
			dist_xy = y_distance * cos(theta) - this->computeVertOffCorr() * sin(theta);
			y = dist_xy * cos(beta) + this->computeHorizOffCorr() * sin(beta);
		}

		z = y_distance * sin(theta) + this->computeVertOffCorr() * cos(theta);

		vector<float> xyz;
		xyz.push_back((float)x);
		xyz.push_back((float)y);
		xyz.push_back((float)z);
		return xyz;
	}

	//If advancedCalibration is true, uses the algorithm in the Velodyne manual to adjust intensity
	//otherwise, just returns point.intensity
	u_char computeIntensity(const laser_point& point, bool advancedCalibration) const{
		u_char intensity = point.computeIntensity();
		if (advancedCalibration){
			double focal_offset = 256 * pow(1 - this->computeFocDist()/13100.0,2);
			//TODO: Check what happens when you add an int to a u_char
			intensity += static_cast<int>(this->computeFocSlope() * abs(focal_offset - 256 * pow(1 - point.computeDist()/65535.0,2)));

			auto min = this->computeMinIntensity();
			auto max = this->computeMaxIntensity();

			intensity = (intensity < min) ? min : intensity;
			intensity = (intensity > max) ? max : intensity;
		}

		return intensity;
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