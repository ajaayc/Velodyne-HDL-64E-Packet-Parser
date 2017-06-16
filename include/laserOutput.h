#ifndef _LASER_OUTPUT_H_
#define _LASER_OUTPUT_H_

#include <pcap.h>
#include "packetOutput.h"
#include "lidarLaser.h"
#include "lidarPoint.h"
#include "lidarFrame.h"
#include "frameOutput.h"
#include <string>

class laserOutput : public packetOutput{
private:
	//True if user wants to also store frame objects from the packet output. Storing
	//frame objects is expensive and this might not be necessary for all laser outputs
	bool constructFrames;

	//Size of this array must be NUM_LASERS
	const lidarLaser* params;

	//lidarFrame data for rendering purposes
	vector<lidarFrame> frames;

	//index of current frame in frames. Trying to avoid doing vector push_back to save time
	int currFrame;

	//Used to detect a new frame and increment currFrame
	double prevAngle;

	//If true, prints out packet numbers and blocks to an output file
	bool printPackets;
public:
	laserOutput(string outFile, lidarLaser* params, bool printPackets, bool constructFrames) : constructFrames(constructFrames), currFrame(-1), prevAngle(-361), packetOutput(outFile), params(params), printPackets(printPackets) {
		if (constructFrames){
			//Reserve half a minutes worth of data to reduce recopying
			frames.resize(300);
		}
	}
	
	//Returns frame vector and its size. If currFrame is -1, then it's empty
	std::pair<const vector<lidarFrame>*, int> getFrames(){
		return std::make_pair(&frames,currFrame+1);
	}

	//Takes in a pointer to beginning of the packet. Requires
	//that packptr points to memory allocated at 1248 bytes
	void printLaserData(const lidarPacket& packptr, u_int id){
		if (printPackets){
			fprintf(pktFile, "PacketID,%d\n", id);
			fprintf(pktFile, "-------------\n");
		}

		for (int i = 0; i < BLOCKS_PER_PACKET; ++i){
			const laser_block& curr_block = packptr.blocks[i];
			if (printPackets){
				fprintf(pktFile, "Laser Block ID,%.4X\n", curr_block.laser_block_id);
				fprintf(pktFile, "Rotational Position,%f\n", curr_block.computeRotation());
				fprintf(pktFile, "Laser Index,cm Distance,X,Y,Z,Laser Intensity\n");
			}

			//Print all laser data for this block
			for(int j = 0; j < LASERS_PER_BLOCK; ++j){
				const laser_point& curr_reading = curr_block.laserData[j];
				//TODO: Check that laser id is computed correctly
				u_char laser_id = (curr_block.laser_block_id == 0xDDFF ? j : j + LASERS_PER_BLOCK);

				//Also includes laser index and cm Distance
				if (printPackets){
					fprintf(pktFile, "%u,", laser_id);
					fprintf(pktFile, "%f,", curr_reading.computeDist());
				}

				//Applying correction factors to laser. It looks like the advanced calibration algorithm for xyz sucks. Would
				//not recommend (unless my code has a bug)
				const lidarLaser& curr_laser= params[laser_id];
				vector<float> xyz = curr_laser.computeXYZ(curr_reading,curr_block.computeRotation(),false);
				u_char intensity = curr_laser.computeIntensity(curr_reading, true);

				if (printPackets){
					fprintf(pktFile, "%f,%f,%f,", xyz[0] / 100.0, xyz[1] / 100.0, xyz[2] / 100.0);
					fprintf(pktFile, "%u,", intensity);
					fprintf(pktFile, "\n");
				}

				if (constructFrames){
					//Storing data for frames
					lidarPoint currPoint(xyz, intensity);
					//Move to a new frame if completed a full rotation. This if statement also executes on the
					//first iteration since prevAngle was set to -361
					if (abs(curr_block.computeRotation() - prevAngle) >= 359){
						++currFrame;
						//Resize the vector if out of bounds
						if (currFrame >= frames.size()){
							frames.resize(frames.size() * 2);
						}
					}

					//TODO: Could possibly optimize. prevAngle doesn't change for current block
					prevAngle = curr_block.computeRotation();
					//Append to currFrame
					frames[currFrame].addPoint(currPoint);
				}

			}
			if (printPackets){
				fprintf(pktFile, "--------------------------\n");
			}
		}
		void;
	}

	~laserOutput(){}
};
#endif /*_LASER_OUTPUT_H_*/