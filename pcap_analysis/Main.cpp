/*
Code adapted from:
www.rhyous.com/2011/11/13/how-to-read-a-pcap-file-from-wireshark-with-c

Visual Studio Setup Video for pcap.h:
www.youtube.com/watch?v=YpnrR7D_lRI
*/

/* Demonstration program of reading packet trace files recorded by pcap
* (used by tshark and tcpdump) and dumping out some corresponding information
* in a human-readable form.
*

/*
* Step 1 - Add includes
*/
#include <string>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <pcap.h>
#include "calibrationRowOutput.h"
#include "laserOutput.h"
#include "calibrationTableOutput.h"
#include "lidarPacket.h"

using namespace std;

int main(int argc, char *argv[])
{
	/*
	* Step 2 - Get a file name
	*/

	//This file has calibration data until Laser 37
	//string file = "C:\\Users\\Ajaay\\Documents\\UMTRI\\veloview\\calibration_5.24.17\\calibration_packets_shorter.pcap";

	//This file has calibration data until Laser 58. Missing Lasers 59 through 64
	//string file = "C:\\Users\\Ajaay\\Documents\\UMTRI\\veloview\\calibration_5.24.17\\calibration_packets.pcap";

	//This file has calibration data for all 64 lasers
	string file = "C:\\Users\\Ajaay\\Documents\\UMTRI\\veloview\\veloview_parkinglot_recordings_5.23.17\\parking_lot_2.pcap";

	/*
	* Step 3 - Create an char array to hold the error.
	*/

	// Note: errbuf in pcap_open functions is assumed to be able to hold at least PCAP_ERRBUF_SIZE chars
	//       PCAP_ERRBUF_SIZE is defined as 256.
	// http://www.winpcap.org/docs/docs_40_2/html/group__wpcap__def.html
	char errbuff[PCAP_ERRBUF_SIZE];

	/*
	* Step 4 - Open the file and store result in pointer to pcap_t
	*/

	// Use pcap_open_offline
	// http://www.winpcap.org/docs/docs_41b5/html/group__wpcapfunc.html#g91078168a13de8848df2b7b83d1f5b69
	pcap_t * pcap = pcap_open_offline(file.c_str(), errbuff);
	
	if (pcap == nullptr || pcap == 0) {
		printf("Couldn't open pcap file.\n");
		printf("%.*s", PCAP_ERRBUF_SIZE, pcap);
		exit(1);
	}

	/*
	* Step 5 - Create a header and a data object
	*/

	// Create a header object:
	// http://www.winpcap.org/docs/docs_40_2/html/structpcap__pkthdr.html
	struct pcap_pkthdr *header;

	// Create a character array using a u_char
	// u_char is defined here:
	// C:\Program Files (x86)\Microsoft SDKs\Windows\v7.0A\Include\WinSock2.h
	// typedef unsigned char   u_char;
	const u_char *data;

	/*
	* Step 6 - Loop through packets and print them to screen
	*/
	u_int packetCount = 0;

	//pcap_next_ex changes the header and data pointer to point to a different location
	//The pointer is mutable but the pointed-to data is immutable

	int returnValue;

	//Create calibration output file. This is for all data
	calibrationRowOutput cal_all("calibration_all.csv",false);

	//Create calibration output file. This is for only the 7 bytes
	calibrationRowOutput cal_calib("calibration_only.csv", true);

	calibrationTableOutput table_calib("table.csv");

	//Create output file to show a packet

	while (returnValue = pcap_next_ex(pcap, &header, &data) >= 0)
	{
		// Print using printf. See printf reference:
		// http://www.cplusplus.com/reference/clibrary/cstdio/printf/

		/*
		// Show the packet number
		printf("Packet # %i\n", ++packetCount);

		// Show the size in bytes of the packet
		printf("Packet size: %d bytes\n", header->len);

		// Show a warning if the length captured is different
		if (header->len != header->caplen){
			printf("Warning! Capture size different than packet size: %ld bytes\n", header->len);
		}
		// Show Epoch Time
		printf("Epoch Time: %d:%d seconds\n", header->ts.tv_sec, header->ts.tv_usec);

		// loop through the packet and print it as hexidecimal representations of octets
		// We also have a function that does this similarly below: PrintData()
		for (u_int i = 0; (i < header->caplen); i++)
		{
			// Start printing on the next after every 16 octets
			if ((i % 16) == 0) printf("\n");

			// Print each octet as hex (x), make sure there is always two characters (.2).
			printf("%.2x ", data[i]);
		}			

		// Add two lines between packets
		printf("\n\n");
		*/

		lidarPacket pack(data);

		if (header->caplen == PACKET_SIZE) {
			cal_all.printCalibrationData(pack);
			cal_calib.printCalibrationData(pack);
			table_calib.recordNewPacket(pack);
		}

	}

}
