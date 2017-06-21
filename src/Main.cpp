/*
Code adapted from:
www.rhyous.com/2011/11/13/how-to-read-a-pcap-file-from-wireshark-with-c

Visual Studio Setup Video for pcap.h:
www.youtube.com/watch?v=YpnrR7D_lRI
*/

/* Demonstration program of reading packet trace files recorded by pcap
* (used by tshark and tcpdump) for the Velodyne HDL-64E S2 Lidar and dumping
* out some corresponding information in a human-readable form.
*

/*
* Step 1 - Add includes
*/
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <pcap.h>
#include "calibrationRowOutput.h"
#include "laserOutput.h"
#include "calibrationTableOutput.h"
#include "lidarPacket.h"
#include "modules.h"
#include "frameGUI.h"

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
	string calibrationFile = "C:\\Users\\Ajaay\\Documents\\UMTRI\\veloview\\veloview_parkinglot_recordings_5.23.17\\parking_lot_2.pcap";

	//Calibrates the lidar by reading a pcap file
	//Size of this array is NUM_LASERS
	const lidarLaser* calTable = calibrateLidar(calibrationFile);
	if (calTable == nullptr){
		printf("Error in calibration. See csv table output for more info.");
		system("pause");
		exit(1);
	}

	//Copy from heap to stack since heap isn't necessary anymore
	lidarLaser params[NUM_LASERS];
	memcpy(params, calTable, sizeof(lidarLaser)* NUM_LASERS);
	delete[] calTable;
	calTable = 0;

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

	string analysisFile = "C:\\Users\\Ajaay\\Documents\\UMTRI\\veloview\\calibration_5.24.17\\one_meter_enclosure_6.12.17.pcap";
	//string analysisFile = "C:\\Users\Ajaay\\Documents\\UMTRI\\veloview\\veloview_parkinglot_recordings_5.23.17\\parking_lot_5.23.17.pcap";

	// Use pcap_open_offline
	// http://www.winpcap.org/docs/docs_41b5/html/group__wpcapfunc.html#g91078168a13de8848df2b7b83d1f5b69
	pcap_t * pcap = pcap_open_offline(analysisFile.c_str(), errbuff);

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

	//Same contents as previous variables. Just made them to make a copy of the files in the visual studio directory
	laserOutput laser_out2("laser_packets.csv", params, false, true);

	int count = 1;
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

		//TODO: Use polymorphism if this gets crazy
		if (header->caplen == PACKET_SIZE) {
			if (true || 800 <= count && count <= 2800){
				//laser_out.printLaserData(pack,count);

				laser_out2.printLaserData(pack, count);
			}
		}
		++count;

	}

	auto framePair = laser_out2.getFrames();
	const vector<lidarFrame>* frames = framePair.first;
	int size = framePair.second;

	printf("Total number of frames: %d\n", size);
	printf("The number of packets read in is: %d\n", count);
	//system("pause");


	/*
	//Make some output files of points
	for (int i = 0; i < min(size,50); ++i){
	stringstream f;
	f << "C:\\Users\\Ajaay\\Documents\\UMTRI\\veloview\\python_point_visualizer\\";
	f << "frame_" << i << ".csv";
	frameOutput out(f.str(), &(frames->at(i)));
	out.outputData();
	}
	*/

	for (int i = 0; i < min(size, 50); ++i){
		frameGUI temp;
		printf("Frame size: %d\n", frames->at(i).getPoints()->size());
		temp.renderFrame(frames->at(i));
		printf("Displayed frame. Press enter\n");

		printf("Continuing...\n");

	}

	system("pause");

}
