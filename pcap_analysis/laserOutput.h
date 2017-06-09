#ifndef _LASER_OUTPUT_H_
#define _LASER_OUTPUT_H_

#include <pcap.h>
#include "packetOutput.h"
#include <string>

class laserOutput : public packetOutput{
private:
	//Size of this array must be NUM_LASERS
	const laser_params* params;
	u_int packetID;
public:
	laserOutput(string outFile,laser_params* params) : packetOutput(outFile),packetID(0),params(params) {
	}

	//Takes in a pointer to beginning of the packet. Requires
	//that packptr points to memory allocated at 1248 bytes
	void printLaserData(const lidarPacket& packptr){
		fprintf(pktFile, "PacketID,%d\n", packetID++);
		fprintf(pktFile, "-------------\n");
		for (int i = 0; i < BLOCKS_PER_PACKET; ++i){
			const laser_block& curr_block = packptr.blocks[i];

			fprintf(pktFile, "Laser Block ID,%.4X\n", curr_block.laser_block_id);
			fprintf(pktFile, "Rotational Position,%f\n", curr_block.rotational_pos / 100.0);
			fprintf(pktFile, "Laser Index,cm Distance,Laser Intensity\n");
			//Print all laser data for this block
			for(int j = 0; j < LASERS_PER_BLOCK; ++j){
				const laser_point& curr_laser = curr_block.laserData[j];
				fprintf(pktFile, "%u,", (curr_block.laser_block_id == 0xDDFF ? j : j + LASERS_PER_BLOCK));
				fprintf(pktFile, "%f,", curr_laser.distance * 2 / 10.0);
				fprintf(pktFile, "%u\n", curr_laser.intensity);
			}
			fprintf(pktFile, "--------------------------\n");
		}
	}

	~laserOutput(){}
};
#endif /*_LASER_OUTPUT_H_*/