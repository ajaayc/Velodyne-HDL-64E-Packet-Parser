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

using namespace std;

class calibrationOutput{
private:
  FILE* calFile;
  u_char gpsT[4];
  u_char statusType;
  u_char statusVal;
  bool calibration_only;

public:
  calibrationOutput(string outFile,bool calibration_only_ = false){
    calFile = fopen(outFile.c_str(), "w");
	if (!calFile) {
		printf("Failed to open %s.\n",outFile.c_str());
		system("pause");
		exit(1);
	}
	calibration_only = calibration_only_;

    fprintf(calFile,"Calibration Packets\n");

    fprintf(calFile,
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
		  fprintf(calFile, "%.2x%.2x%.2x%.2x,", gpsT[3], gpsT[2], gpsT[1], gpsT[0]);

		  //2. Convert Timestamp to us and print it. This works without reversing the bytes because
		  //computer is little endian
		  u_int* gpsT_decimal = (u_int*)(gpsT);

		  fprintf(calFile, "%u,", *gpsT_decimal);

		  //3. Status Type in Hex
		  fprintf(calFile, "%.2x,", statusType);

		  //4. Status Type in Decimal
		  fprintf(calFile, "%d,", statusType);

		  //5. Status Type in ASCII
		  fprintf(calFile, "%c,", statusType);

		  //6. Status Value in Hex
		  fprintf(calFile, "%.2x,", statusVal);

		  //7. Status Value in Decimal
		  fprintf(calFile, "%d,", statusVal);

		  //8. Status Value in ASCII
		  fprintf(calFile, "%c,", statusVal);

		  fprintf(calFile, "%s", "\n");
	  }
  }

  ~calibrationOutput(){
    fclose(calFile);
  }
};


int main(int argc, char *argv[])
{
	/*
	* Step 2 - Get a file name
	*/

	string file = "C:\\Users\\Ajaay\\Documents\\UMTRI\\veloview\\calibration_5.24.17\\calibration_packets_shorter.pcap";
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
	calibrationOutput cal_all("calibration_all.csv",false);

	//Create calibration output file. This is for only the 7 bytes
	calibrationOutput cal_calib("calibration_only.csv", true);

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

		if (header->caplen == 1248) {
			cal_all.printCalibrationData(data);
			cal_calib.printCalibrationData(data);
		}

	}

}
