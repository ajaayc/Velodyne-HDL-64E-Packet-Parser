#ifndef _LASER_OUTPUT_H_
#define _LASER_OUTPUT_H_

#include <pcap.h>
#include "packetOutput.h"
#include <string>

class laserOutput : public packetOutput{
private:

public:
	laserOutput(string outFile) : packetOutput(outFile) {
	}

	//Takes in a pointer to beginning of the packet. Requires
	//that packptr points to memory allocated at 1248 bytes
	void printCalibrationData(const u_char* packptr){

	}

	~laserOutput(){}
};
#endif /*_LASER_OUTPUT_H_*/