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

	//This function ignores packets until it sees the 'U','N','I','T'
	//as status values
	l_status currCycle[CYCLE_LENGTH];

	//TODO: resolve compiler issues. Make static
	u_char STATUS_TYPES[CYCLE_LENGTH];

	//Current index of status_types array. Helps check for packet continuity in
	//case packets were missed
	int currIndex;

	bool detectedUNIT;
public:
	calibrationTableOutput(string outFile) : packetOutput(outFile){
		detectedUNIT = false;
		//Workaround initialization
		u_char temp[] = { 'H', 'M', 'S', 'D', 'N', 'Y', 'G', 'T', 'V', '1', '2', '3', '4', '5', '6', '7' };
		memcpy(temp, STATUS_TYPES, CYCLE_LENGTH);

		currIndex = 0;

		memset((void*)params,0,sizeof(laser_params) * NUM_LASERS);
		memset((void*)currCycle,0,sizeof(l_status) * CYCLE_LENGTH);
	}

	//Requires that this function is called on consecutive
	//packets. (The caller must give every packet that it receives
	//to this function without skipping)
	void recordNewPacket(const lidarPacket& packptr){
		if (!detectedUNIT) {
			if (packptr.statusType == '1' && packptr.statusVal == 'U'){
				detectedUNIT = true;
			}
		}
		else {

		}
	}


	~calibrationTableOutput(){}
};
#endif