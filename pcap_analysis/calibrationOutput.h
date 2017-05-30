#include <pcap.h>
#include <string>
#include "packetOutput.h"

class calibrationOutput : public packetOutput {
private:
	u_char gpsT[4];
	u_char statusType;
	u_char statusVal;
	bool calibration_only;

public:
	calibrationOutput(string outFile, bool calibration_only_ = false) : packetOutput(outFile){
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
	void printCalibrationData(const u_char* packptr){
		//Prints last 6 Bytes to csv

		gpsT[0] = packptr[1242];
		gpsT[1] = packptr[1243];
		gpsT[2] = packptr[1244];
		gpsT[3] = packptr[1245];

		statusType = packptr[1246];
		statusVal = packptr[1247];

		int i = 0;

		if (!calibration_only || (calibration_only && '1' <= statusType && statusType <= '7')){

			//1. 4 Bytes for GPS Timestamp. Have to follow manual and reverse packets
			fprintf(pktFile, "%.2x%.2x%.2x%.2x,", gpsT[3], gpsT[2], gpsT[1], gpsT[0]);

			//2. Convert Timestamp to us and print it. This works without reversing the bytes because
			//computer is little endian
			u_int* gpsT_decimal = (u_int*)(gpsT);

			fprintf(pktFile, "%u,", *gpsT_decimal);

			//3. Status Type in Hex
			fprintf(pktFile, "%.2x,", statusType);

			//4. Status Type in Decimal
			fprintf(pktFile, "%d,", statusType);

			//5. Status Type in ASCII
			fprintf(pktFile, "%c,", statusType);

			//6. Status Value in Hex
			fprintf(pktFile, "%.2x,", statusVal);

			//7. Status Value in Decimal
			fprintf(pktFile, "%d,", statusVal);

			//8. Status Value in ASCII
			fprintf(pktFile, "%c,", statusVal);

			fprintf(pktFile, "%s", "\n");
		}
	}

	~calibrationOutput(){
	}
};