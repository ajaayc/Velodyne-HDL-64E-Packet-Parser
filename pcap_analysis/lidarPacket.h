#ifndef _LIDAR_PACKET_H_
#define _LIDAR_PACKET_H_

#include "pcap.h"

class lidarPacket{
public:
	u_char gpsT[4];
	u_char statusType;
	u_char statusVal;

	const u_char* data;

	//Requires that data points to the beginning of a 1248 byte valid array
	lidarPacket(const u_char *data) {
		this->data = data;

		gpsT[0] = data[1242];
		gpsT[1] = data[1243];
		gpsT[2] = data[1244];
		gpsT[3] = data[1245];

		statusType = data[1246];
		statusVal = data[1247];
	}

	//TODO: Extract laser readings, too, if necessary
	
};
#endif