#ifndef _LASER_OUTPUT_H_
#define _LASER_OUTPUT_H_

#include <pcap.h>
#include "packetOutput.h"
#include <string>

class laserOutput : public packetOutput{
private:
	u_int packetID;
public:
	laserOutput(string outFile) : packetOutput(outFile),packetID(0) {
	}

	//Takes in a pointer to beginning of the packet. Requires
	//that packptr points to memory allocated at 1248 bytes
	void printLaserData(const lidarPacket& packptr){
		fprintf(pktFile, "--------------------------------\n");
		fprintf(pktFile, "PacketID,%d\n",packetID++);
		for (int i = 0; i < BLOCKS_PER_PACKET; ++i){
			const laser_block& curr_block = packptr.blocks[i];

			fprintf(pktFile, "Laser Block ID,%.4X\n", curr_block.laser_block_id);
			fprintf(pktFile, "Rotational Position,%f\n", curr_block.rotational_pos / 100.0);

			//Print all laser data for this block
			for (int j = 0; j < LASERS_PER_BLOCK; ++j){
				const laser_point& curr_laser = curr_block.laserData[j];
				fprintf(pktFile, "Laser Index,%u\n", (curr_block.laser_block_id == 0xDDFF ? j : j + LASERS_PER_BLOCK));
				fprintf(pktFile, "cm Distance,%f\n", curr_laser.distance * 2 / 10.0);
				fprintf(pktFile, "Laser Intensity,%u\n", curr_laser.intensity);
			}
			fprintf(pktFile, "%s", "\n");
		}
	}

	~laserOutput(){}
};
#endif /*_LASER_OUTPUT_H_*/