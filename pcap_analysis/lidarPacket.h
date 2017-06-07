#ifndef _LIDAR_PACKET_H_
#define _LIDAR_PACKET_H_

#include "pcap.h"
#include "definitions.h"

class lidarPacket{
public:
	//array of 12 laser blocks which are extracted from the packet
	laser_block blocks[BLOCKS_PER_PACKET];

	u_char gpsT[4];
	u_char statusType;
	u_char statusVal;

	//Requires that data points to the beginning of a 1248 byte valid array
	lidarPacket(const u_char *data) {
		extractLaserReadings(data);
		extractCalibrationData(data);
	}

	void extractLaserReadings(const u_char *data){
		//Loop through 12 blocks of 100 byte data
		for (int block_no = 0; block_no < BLOCKS_PER_PACKET; ++block_no){
			int block_start = PACKET_HEADER_LENGTH + LASER_BLOCK_SIZE * block_no;
			
			laser_block& curr_block = blocks[block_no];
			memcpy(&curr_block.laser_block_id, data + block_start, sizeof(curr_block.laser_block_id));
			memcpy(&curr_block.rotational_pos,data + block_start + sizeof(short),sizeof(curr_block.rotational_pos));
			
			int laser_start = block_start + 2 * sizeof(short);

			//Now loop through all laser readings in this block
			for (int laser_index = 0; laser_index < LASERS_PER_BLOCK; ++laser_index){
				//offset in data array of current laser we're looking at
				int curr_laser_offset = laser_start + laser_index * LASER_POINT_SIZE;

				//Now insert data into curr_block's laserData array
				laser_point& curr_point = curr_block.laserData[laser_index];

				memcpy(&curr_point.distance, data+curr_laser_offset, sizeof(curr_point.distance));
				curr_point.intensity = data[curr_laser_offset + 2];
			}


		}

	}

	void extractCalibrationData(const u_char *data){
		gpsT[0] = data[1242];
		gpsT[1] = data[1243];
		gpsT[2] = data[1244];
		gpsT[3] = data[1245];

		statusType = data[1246];
		statusVal = data[1247];
	}

};
#endif