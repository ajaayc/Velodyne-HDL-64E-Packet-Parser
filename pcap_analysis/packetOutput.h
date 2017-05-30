#include <pcap.h>
#include <string>

using namespace std;

class packetOutput{
protected:
	FILE* pktFile;

public:
	packetOutput(string outFile){
		pktFile = fopen(outFile.c_str(), "w");
		if (!pktFile) {
			printf("Failed to open %s.\n", outFile.c_str());
			system("pause");
			exit(1);
		}
	}

	//Takes in a pointer to beginning of the packet. Requires
	//that packptr points to memory allocated at 1248 bytes
	virtual void printCalibrationData(const u_char* packptr) = 0;

	~packetOutput(){
		fclose(pktFile);
	}

};