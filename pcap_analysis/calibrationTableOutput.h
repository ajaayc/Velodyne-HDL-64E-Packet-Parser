#ifndef _TABLE_OUTPUT_H_
#define _TABLE_OUTPUT_H_

#include <pcap.h>
#include <string>
#include "definitions.h"
#include "packetOutput.h"
#include "definitions.h"

struct l_status{
	u_char type;
	u_char value;
};

//Class that stores calibration data by reading packets one after another.
class calibrationTableOutput : public packetOutput {
private:
	//2 Byte threshold
	u_short threshold;
	// NUM_LASERS should be 64.
	laser_params params[NUM_LASERS];

	//Current index of laser_params array
	int laserIndex;

	//Current laser we've got data for
	l_status currLaser[PACKETS_PER_LASER];

	//TODO: resolve compiler issues for no in-class initializer
	static const u_char STATUS_TYPES[];

	//Current index of currLaser array
	int currIndex;

	//First cycle contains threshold parameters
	bool isFirstCycle;

	bool detectedUNIT;

	bool finishedCalibration;

private:
	//Converts 2 consecutive bytes in currLaser array to a u_short.
	//Takes in index of currLaser[] where bytes begin
	u_short bytes_to_short(int index){
		u_char bytes[sizeof(short)];
		u_short s;

		bytes[0] = 	currLaser[index].value;
		bytes[1] = currLaser[index + 1].value;

		memcpy(&s, bytes, sizeof(short));
		return s;
	}
public:
	calibrationTableOutput(string outFile) : packetOutput(outFile){
		detectedUNIT = false;
		finishedCalibration = false;
		isFirstCycle = false;
		currIndex = 0;
		laserIndex = 0;
		memset((void*)currLaser, 0, sizeof(l_status)* CYCLE_LENGTH);

		memset((void*)params, 0, sizeof(laser_params)* NUM_LASERS);
	}
private:
	inline int getLaserIndex(int cycle_num, int index){
		return cycle_num * CYCLE_LENGTH + MISC_DATA_LENGTH + index;
	}

	//Puts values into laser_params[laserIndex] from currLaser
	void parseLaserData(){
		laser_params& currParam = params[laserIndex];

		currParam.laser_num = currLaser[getLaserIndex(0,0)].value;
		currParam.vert_correction = bytes_to_short(getLaserIndex(0,1));
		currParam.rot_correction = bytes_to_short(getLaserIndex(0,3));
		currParam.far_dist_correction = bytes_to_short(getLaserIndex(0,5));
		currParam.dist_correction_x = bytes_to_short(getLaserIndex(0,0));
		currParam.dist_correction_v = bytes_to_short(getLaserIndex(2,0));
		currParam.vert_offset_correction = bytes_to_short(getLaserIndex(4,0));
		//currParam.horiz_offset_correction = 0;
		currParam.focal_dist = bytes_to_short(getLaserIndex(2, 1));
		currParam.focal_slope = bytes_to_short(getLaserIndex(2, 3));
	}

public:
	//Requires that this function is called on consecutive
	//packets. (The caller must give every packet that it receives
	//to this function without skipping)
	//Packptr won't be recorded if we already have calibration data
	void recordNewPacket(const lidarPacket& packptr){
		if (finishedCalibration){
			return;
		}

		if (!detectedUNIT) {
			if (packptr.statusType == '1' && packptr.statusVal == 'U'){
				detectedUNIT = true;
				//Starts at different index after all of the H,M,S,D,M,Y,GPS,T,V data
				currIndex = 8;

				isFirstCycle = true;
			}
		}

		//If currently recording packets since we've read unit:
		if (detectedUNIT) {
			l_status currUnit;
			currUnit.type = packptr.statusType;
			currUnit.value = packptr.statusVal;

			currLaser[currIndex] = currUnit;

			++currIndex;
			if (isFirstCycle && currIndex >= CYCLE_LENGTH){
				//Extract threshold bytes
				memcpy((void*)&this->threshold, (void*) (currLaser + 14), sizeof(this->threshold));

				isFirstCycle = false;
				currIndex = 0;
			}

		}

		//Finished calibration data for this laser
		if (currIndex >= PACKETS_PER_LASER) {
			//Puts values into laser_params[laserIndex] from currLaser
			parseLaserData();
			++laserIndex;
			currIndex = 0;

			if (laserIndex >= NUM_LASERS) {
				finishedCalibration = true;
				/*
				detectedUNIT = false;
				laserIndex = 0;
				*/
			}
		}
	}


	~calibrationTableOutput(){}
};

const u_char calibrationTableOutput::STATUS_TYPES[] = { 'H', 'M', 'S', 'D', 'N', 'Y', 'G', 'T', 'V', '1', '2', '3', '4', '5', '6', '7' };

#endif