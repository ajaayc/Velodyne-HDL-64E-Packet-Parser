#ifndef _ROW_OUTPUT_H_
#define _ROW_OUTPUT_H_

#include <pcap.h>
#include <string>
#include "packetOutput.h"
#include "lidarPacket.h"

class calibrationRowOutput : public packetOutput {
private:
	bool calibration_only;

public:
	calibrationRowOutput(string outFile, bool calibration_only_ = false) : packetOutput(outFile){
		calibration_only = calibration_only_;

		fprintf(pktFile, "Calibration Packets\n");

		fprintf(pktFile,
			"%s%s%s%s%s%s%s%s",
			"GPS Timestamp,",
			"Timestamp in us,",
			"Status Type,",
			"Status Type Decimal,",
			"Status Type ASCII,",
			"Status Value,",
			"Status Value Decimal,",
			"Status Value ASCII\n");
	}

	//Takes in a pointer to beginning of the packet. Requires
	//that packptr points to memory allocated at 1248 bytes
	void printCalibrationData(const lidarPacket& pack){

		int i = 0;

		if (!calibration_only || (calibration_only && '1' <= pack.statusType && pack.statusType <= '7')){

			//1. 4 Bytes for GPS Timestamp. Have to follow manual and reverse packets
			fprintf(pktFile, "%.2x%.2x%.2x%.2x,", pack.gpsT[3], pack.gpsT[2], pack.gpsT[1], pack.gpsT[0]);

			//2. Convert Timestamp to us and print it. This works without reversing the bytes because
			//computer is little endian
			u_int* gpsT_decimal = (u_int*)(pack.gpsT);

			fprintf(pktFile, "%u,", *gpsT_decimal);

			//3. Status Type in Hex
			fprintf(pktFile, "%.2x,", pack.statusType);

			//4. Status Type in Decimal
			fprintf(pktFile, "%d,", pack.statusType);

			//5. Status Type in ASCII
			fprintf(pktFile, "%c,", pack.statusType);

			//6. Status Value in Hex
			fprintf(pktFile, "%.2x,", pack.statusVal);

			//7. Status Value in Decimal
			fprintf(pktFile, "%d,", pack.statusVal);

			//8. Status Value in ASCII
			fprintf(pktFile, "%c,", pack.statusVal);

			fprintf(pktFile, "%s", "\n");
		}
	}

	~calibrationRowOutput(){
	}
};

#endif