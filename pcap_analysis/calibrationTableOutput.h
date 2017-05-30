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

	//This function ignores packets until it sees the 'U','N','I','T'
	//as status values 
	u_char currCycle[CYCLE_LENGTH];

	bool detectedUNIT;
public:
	calibrationTableOutput(string outFile) : packetOutput(outFile){
		detectedUNIT = false;
		memset((void*)params,0,sizeof(laser_params) * NUM_LASERS);
	}

	//Takes in a pointer to beginning of the packet. Requires
	//that packptr points to memory allocated at 1248 bytes.
	//Also requires that this function is called on consecutive
	//packets. (The caller must give every packet that it receives
	//to this function without skipping)
	void recordNewPacket(const lidarPacket* packptr){

	}


	~calibrationTableOutput(){}
};
#endif