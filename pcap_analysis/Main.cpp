/*Code obtained from:
www.rhyous.com/2011/11/13/how-to-read-a-pcap-file-from-wireshark-with-c
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
#include <pcap.h>

using namespace std;

int main(int argc, char *argv[])
{
	/*
	* Step 2 - Get a file name
	*/

	string file = "C:\\users\\jared\\testfiles\\smallcapture.pcap";

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
	while (int returnValue = pcap_next_ex(pcap, &header, &data) >= 0)
	{
		// Print using printf. See printf reference:
		// http://www.cplusplus.com/reference/clibrary/cstdio/printf/

		// Show the packet number
		printf("Packet # %i\n", ++packetCount);

		// Show the size in bytes of the packet
		printf("Packet size: %d bytes\n", header->len);

		// Show a warning if the length captured is different
		if (header->len != header->caplen)
			printf("Warning! Capture size different than packet size: %ld bytes\n", header->len);

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
	}
}