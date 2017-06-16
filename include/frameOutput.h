#ifndef _FRAME_OUTPUT_H_
#define _FRAME_OUTPUT_H_

#include "lidarFrame.h"

class frameOutput : public packetOutput{
private:
	const lidarFrame* frame;
public:
	frameOutput(string filename,const lidarFrame* frame) :packetOutput(filename), frame(frame){
		fprintf(pktFile, "X,Y,Z,Laser Intensity\n");
	}

	void outputData(){
		const vector<lidarPoint>* fVec = frame->getPoints();
		for (int i = 0; i < fVec->size(); ++i){
			const float* xyz = fVec->at(i).xyz;
			u_char intensity = fVec->at(i).intensity;

			fprintf(pktFile, "%f,%f,%f,", xyz[0] / 100.0, xyz[1] / 100.0, xyz[2] / 100.0);
			fprintf(pktFile, "%u,", intensity);
			fprintf(pktFile, "\n");
		}

	}
};

#endif