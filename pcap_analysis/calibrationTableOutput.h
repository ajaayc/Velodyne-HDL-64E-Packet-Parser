#ifndef _TABLE_OUTPUT_H_
#define _TABLE_OUTPUT_H_

#include <pcap.h>
#include <string>
#include "definitions.h"
#include "packetOutput.h"
#include "definitions.h"

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

	bool calibrationFail;

private:
	//Converts 2 consecutive bytes in currLaser array to a signed short.
	//Takes in index of currLaser[] where bytes begin
	short bytes_to_short(int index){
		return bytes_to_short(index,index+1);
	}

	//Overloaded. Can use bytes at different indexes
	short bytes_to_short(int index1, int index2){
		u_char bytes[sizeof(short)];
		short s;
		
		bytes[0] = currLaser[index1].value;
		bytes[1] = currLaser[index2].value;

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
		calibrationFail = false;

		memset((void*)currLaser, 0, sizeof(l_status)* PACKETS_PER_LASER);

		memset((void*)params, 0, sizeof(laser_params)* NUM_LASERS);
	}

	//Returns a pointer to the calibration data. Returns nullptr if calibration failed
	//or calibration isn't done yet
	//Caller is responsible for freeing memory.
	const laser_params* getCalibrationData(){
		if (!finishedCalibration || calibrationFail){
			return nullptr;
		}

		//Returning a copy of the member variable to avoid scope issues
		laser_params* copy = new laser_params[NUM_LASERS];

		memcpy(copy, params, sizeof(laser_params) * NUM_LASERS);

		return copy;
	}

private:
	inline int getLaserIndex(int cycle_num, int index){
		return cycle_num * CYCLE_LENGTH + MISC_DATA_LENGTH + index;
	}

	/*
	// Divides laser_params accordingly based on the manual
	void scaleLaserData(laser_params& params){
		//Scale
		params.vert_correction /= 100;
		params.rot_correction /= 100;

		// For cm
		params.far_dist_correction /= 10;
		params.dist_correction_x /= 10;
		params.dist_correction_v /= 10;
		params.vert_offset_correction /= 10;
		params.horiz_offset_correction /= 10;
		params.focal_dist /= 10;

		//Scale
		params.focal_slope /= 10;
	}
	*/

	void printLaserData(const laser_params& param){
	  fprintf(pktFile,"laser_num: %d\n",param.laser_num);
	  fprintf(pktFile,"vert_correction: %f\n",param.computeVertCorrection());
	  fprintf(pktFile,"rot_correction: %f\n", param.computeRotCorrection());
	  fprintf(pktFile,"far_dist_correction: %f\n",param.computeDistCorr());
	  fprintf(pktFile, "dist_correction_x: %f\n", param.computeDistCorrX());
	  fprintf(pktFile, "dist_correction_v: %f\n", param.computeDistCorrY());
	  fprintf(pktFile, "vert_offset_correction: %f\n", param.computeVertOffCorr());
	  fprintf(pktFile, "horiz_offset_correction: %f\n", param.computeHorizOffCorr());
	  fprintf(pktFile, "focal_dist: %f\n", param.computeFocDist());
	  fprintf(pktFile, "focal_slope: %f\n", param.computeFocSlope());
	  fprintf(pktFile,"min_intensity: %u\n",param.computeMinIntensity());
	  fprintf(pktFile,"max_intensity: %u\n",param.computeMaxIntensity());
	  fprintf(pktFile,"\n");
	}

	//Puts values into laser_params[laserIndex] from currLaser
	bool parseLaserData(){
		laser_params& currParam = params[laserIndex];

		currParam.laser_num = currLaser[getLaserIndex(0,0)].value;
		currParam.vert_correction = bytes_to_short(getLaserIndex(0,1));
		currParam.rot_correction = bytes_to_short(getLaserIndex(0,3));
		currParam.far_dist_correction = bytes_to_short(getLaserIndex(0,5));
		currParam.dist_correction_x = bytes_to_short(getLaserIndex(1,0));
		currParam.dist_correction_v = bytes_to_short(getLaserIndex(1,2));
		currParam.vert_offset_correction = bytes_to_short(getLaserIndex(1,4));
		
		//Requires overloaded version
		currParam.horiz_offset_correction = bytes_to_short(getLaserIndex(1,6), getLaserIndex(2,0));
		
		currParam.focal_dist = bytes_to_short(getLaserIndex(2, 1));
		currParam.focal_slope = bytes_to_short(getLaserIndex(2, 3));
		currParam.min_intensity = currLaser[getLaserIndex(2,5)].value;
		currParam.max_intensity = currLaser[getLaserIndex(2,6)].value;

		//scaleLaserData(currParam);
		printLaserData(currParam);

		if (currParam.laser_num != (u_int)laserIndex){
			calibrationFail = true;
			fprintf(pktFile, "ERROR: Calibration data no longer aligned at laser index %d.\n", laserIndex);
			fprintf(pktFile, "It's suggested that you try to calibrate with a different pcap file.\n");
			return false;
		}
		return true;
	}

public:
	//Requires that this function is called on consecutive
	//packets. (The caller must give every packet that it receives
	//to this function without skipping)
	//Packptr won't be recorded if we already have calibration data
	//Returns 0 if calibration complete, -1 if calibration failed, or + value
	//if still need more data to calibrate
	int recordNewPacket(const lidarPacket& packptr){
		if (finishedCalibration){
			return 0;
		}

		if (calibrationFail){
			return -1;
		}

		if (!detectedUNIT) {
			if (packptr.statusType == '1' && packptr.statusVal == 'U'){
				detectedUNIT = true;
				//Starts at different index after all of the H,M,S,D,M,Y,GPS,T,V data
				currIndex = MISC_DATA_LENGTH;

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
			bool success = parseLaserData();
			if (!success){
				return -1;
			}
			
			++laserIndex;
			currIndex = 0;
			

			if (laserIndex >= NUM_LASERS) {
				finishedCalibration = true;
				return 0;
			}
		}
		return 1;
	}


	~calibrationTableOutput(){}
};

const u_char calibrationTableOutput::STATUS_TYPES[] = { 'H', 'M', 'S', 'D', 'N', 'Y', 'G', 'T', 'V', '1', '2', '3', '4', '5', '6', '7' };

#endif
