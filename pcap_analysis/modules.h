#ifndef _MODULES_H_
#define _MODULES_H_

#include "definitions.h"
#include "calibrationTableOutput.h"
#include "calibrationRowOutput.h"

// Builds a calibration table from the packets of a pcap file
// The pointer returned by this function must be freed by the caller w/
// delete []
const laser_params* calibrateLidar(const string& calibrationFile){
	char errbuff[PCAP_ERRBUF_SIZE];

	// Use pcap_open_offline
	// http://www.winpcap.org/docs/docs_41b5/html/group__wpcapfunc.html#g91078168a13de8848df2b7b83d1f5b69
	pcap_t * pcap = pcap_open_offline(calibrationFile.c_str(), errbuff);

	if (pcap == nullptr || pcap == 0) {
		printf("Couldn't open pcap file.\n");
		printf("%.*s", PCAP_ERRBUF_SIZE, pcap);
		exit(1);
	}

	struct pcap_pkthdr *header;

	const u_char *data;

	u_int packetCount = 0;

	int returnValue;

	//Create calibration output file. This is for all data
	calibrationRowOutput cal_all("calibration_all.csv", false);

	//Create calibration output file. This is for only the 7 bytes
	calibrationRowOutput cal_calib("calibration_only.csv", true);

	calibrationTableOutput table_calib("calibration_table.csv");

	int count = 1;
	while (returnValue = pcap_next_ex(pcap, &header, &data) >= 0)
	{
		lidarPacket pack(data);

		bool finishedCalibration = false;

		//TODO: Use polymorphism if this gets crazy
		if (header->caplen == PACKET_SIZE) {
			cal_all.printCalibrationData(pack);
			cal_calib.printCalibrationData(pack);

			int rval;
			//Stop reading packets if calibration failed or
			//calibration finished
			rval = table_calib.recordNewPacket(pack);
			if (rval <= 0){
				break;
			}
		}
		++count;

	}

	return table_calib.getCalibrationData();
}

#endif