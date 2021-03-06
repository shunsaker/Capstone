#include "stdafx.h"
#include "LEGION.h"


LEGION::LEGION(Correlogram& correlogram, std::string name) : FRAMES(correlogram.T_FGrid->FRAMES), CHANNELS(correlogram.T_FGrid->CHANNELS), correlogram(correlogram)
{
	segments = new std::vector<Segment*>();
	segmentGrid = new intGrid(CHANNELS, FRAMES, -1);
	initializeGrid();
	createConnections(name);
	findLeaders();
}

void LEGION::initializeGrid() {
	print "initializing" endl;

	neuralGrid = new Oscillator**[CHANNELS];
	for (int channel = 0; channel < CHANNELS; channel++) {
		neuralGrid[channel] = new Oscillator*[FRAMES];
		for (int frame = 0; frame < FRAMES; frame++) {
			neuralGrid[channel][frame] = new Oscillator(MINIMUM_ACTIVITY,
				(*correlogram.T_FGrid)[frame][channel][0] > correlogram.activityThreshold ? .2 : -5);
		}
	}
}


void LEGION::createConnections(std::string name) {
	print "linking" endl;
	//timeConnections = new Connection**[correlogram.CHANNELS];
	//freqConnections = new Connection**[correlogram.CHANNELS - 1];


	//int count = 0;
	//double total = 0;
	//double totalChanoff = 0;
	//for (int channel = 0; channel < CHANNELS; channel++) {
	//	//timeConnections[channel] = new Connection*[correlogram.FRAMES - 1];
	//	//if (channel < CHANNELS - 1)
	//		//freqConnections[channel] = new Connection*[correlogram.FRAMES];
	//	//double correctValue = 1 - channel * .002;
	//	for (int frame = 0; frame < FRAMES; frame++) {
	//		if (frame < FRAMES - 1) {
	//			double crossCorrelation = (*correlogram.T_FGrid)[frame][channel].crossCorrelate((*correlogram.T_FGrid)[frame + 1][channel]);
	//			if (crossCorrelation > .1) {
	//				print crossCorrelation <<'\t' << channel endl;
	//				count++;
	//				total += crossCorrelation;
	//				totalChanoff += channel;// (1 - crossCorrelation);
	//			}
	//			//double weight = crossCorrelation
	//			//	//> (channel >= highFreqThreshold ? crossCorrelationThresholdHigh : crossCorrelationThreshold) ? 1 : 0;
	//			//	> crossCorrelationThresholdHigh ? 1 : 0;
	//			////Connection* timeConnection = new Connection(weight, neuralGrid[channel][frame], neuralGrid[channel][frame + 1]);
	//			////timeConnections[channel][frame] = timeConnection;
	//			//if (weight > 0) {
	//			//	neuralGrid[channel][frame]->right = neuralGrid[channel][frame + 1];
	//			//	neuralGrid[channel][frame]->wRight = weight;
	//			//	neuralGrid[channel][frame + 1]->left = neuralGrid[channel][frame];
	//			//	neuralGrid[channel][frame + 1]->wLeft = weight;
	//			//}
	//		}
	//		if (channel < CHANNELS - 1) {
	//			double crossCorrelation = (*correlogram.T_FGrid)[frame][channel].crossCorrelate((*correlogram.T_FGrid)[frame][channel + 1]);
	//			if (crossCorrelation > .1) {
	//				print crossCorrelation << '\t' << channel endl;
	//				count++;
	//				total += crossCorrelation;
	//				totalChanoff += channel;//(1 - crossCorrelation);
	//			}
	//			//double weight = crossCorrelation
	//			//	//> (channel >= highFreqThreshold ? crossCorrelationThresholdHigh : crossCorrelationThreshold) ? 1 : 0;
	//			//	> crossCorrelationThresholdHigh ? 1 : 0;

	//			//Connection* freqConnection = new Connection(weight, neuralGrid[channel][frame], neuralGrid[channel + 1][frame]);
	//			//freqConnections[channel][frame] = freqConnection;
	//			/*if (weight > 0) {
	//				neuralGrid[channel][frame]->down = neuralGrid[channel + 1][frame];
	//				neuralGrid[channel][frame]->wDown = weight;
	//				neuralGrid[channel + 1][frame]->up = neuralGrid[channel][frame];
	//				neuralGrid[channel + 1][frame]->wUp = weight;
	//			}*/
	//		}
	//	}
	//}
	//double average = total / count;
	//double avgChan = totalChanoff / count;
	//print "total: " << average endl;

	std::fstream vertCons(name + " vertcons.txt", std::ios::out);
	std::fstream horCons(name + " horcons.txt", std::ios::out);

	int dead = 0;
	for (int channel = 0; channel < CHANNELS; channel++) {
		double accuracyReq = accuracyRequirement(channel);
		for (int frame = 0; frame < FRAMES; frame++) {
			if (frame < FRAMES - 1) {
				double crossCorrelation = (*correlogram.T_FGrid)[frame][channel].crossCorrelate((*correlogram.T_FGrid)[frame + 1][channel]);
				vertCons << crossCorrelation << " ";
				//if (crossCorrelation > 0) print crossCorrelation endl;
				if (crossCorrelation < .001) dead++;
				double weight = crossCorrelation
					//> (channel >= highFreqThreshold ? crossCorrelationThresholdHigh : crossCorrelationThreshold) ? 1 : 0;
				> accuracyReq/*(channel > highFreqThreshold ? crossCorrelationThresholdHigh : crossCorrelationThreshold)*/ ? 1 : 0;
				//Connection* timeConnection = new Connection(weight, neuralGrid[channel][frame], neuralGrid[channel][frame + 1]);
				//timeConnections[channel][frame] = timeConnection;
				if (weight > 0) {
					neuralGrid[channel][frame]->right = neuralGrid[channel][frame + 1];
					neuralGrid[channel][frame]->wRight = weight;
					neuralGrid[channel][frame + 1]->left = neuralGrid[channel][frame];
					neuralGrid[channel][frame + 1]->wLeft = weight;
				}
			}
			if (channel < CHANNELS - 1) {
				double crossCorrelation = (*correlogram.T_FGrid)[frame][channel].crossCorrelate((*correlogram.T_FGrid)[frame][channel + 1]);
				horCons << crossCorrelation << " ";
				if (crossCorrelation < .001) dead++;
				double weight = crossCorrelation
					//> (channel >= highFreqThreshold ? crossCorrelationThresholdHigh : crossCorrelationThreshold) ? 1 : 0;
				>  accuracyReq/*(channel > highFreqThreshold ? crossCorrelationThresholdHigh : crossCorrelationThreshold)*/ ? 1 : 0;
				//Connection* freqConnection = new Connection(weight, neuralGrid[channel][frame], neuralGrid[channel + 1][frame]);
				//freqConnections[channel][frame] = freqConnection;
				if (weight > 0) {
					neuralGrid[channel][frame]->down = neuralGrid[channel + 1][frame];
					neuralGrid[channel][frame]->wDown = weight;
					neuralGrid[channel + 1][frame]->up = neuralGrid[channel][frame];
					neuralGrid[channel + 1][frame]->wUp = weight;
				}
			}
		}
		vertCons endl;
		horCons endl;
	}
	print "DEAD: " << dead endl;
}

double LEGION::accuracyRequirement(int channel) {
	return .96 - pow(channel, 2) / pow(350, 2) - channel / 4267;
}

void LEGION::findLeaders() {
	int leaders = 0;
	for (int channel = 0; channel < CHANNELS; channel++) {
		for (int frame = 0; frame < FRAMES; frame++) {
			if (neuralGrid[channel][frame]->totalWeight() >= leaderNeighbors) {
				leaders++;
				neuralGrid[channel][frame]->makeLeader();
			}
		}
	}
	//print "leaders: " << leaders endl;
}

Oscillator* LEGION::maxExcitement() {
	double max = -5;
	Oscillator* next = nullptr;
	for (int channel = 0; channel < CHANNELS; channel++) {
		for (int frame = 0; frame < FRAMES; frame++) {
			if (neuralGrid[channel][frame]->isLeader() && neuralGrid[channel][frame]->excitement > max) {
				next = neuralGrid[channel][frame];
				max = neuralGrid[channel][frame]->excitement;
			}
		}
	}
	return next;
}

void LEGION::run() {
	bool spiked = true;
	do {
		Oscillator* next = maxExcitement();
		if (next == nullptr) {
			spiked = true;
			print "ended by null" endl;
		}
		else {
			spiked = next->hasSpiked;
			if (!spiked) {
				int spikes = next->spike(numSegments);
				if (spikes < minGroup) {
					next->kill();
				}
				else {
					if (spikes > numInLargestSegment) {
						numInLargestSegment = spikes;
						largestSegment = numSegments;
					}
					allStep(spikes);
					numSegments++;
				}
			}
			//else {
			//	print next->excitement endl;
			//	print next->segment endl;
			//	print next->totalWeight() endl;
			//	print "has spiked" endl;
			//}
		}
	} while (!spiked);
	print "segments: " << numSegments endl;
	//saveSegmentGrid("LEGION.txt");
}

void LEGION::allStep(int spiked) {
	Segment* segment = new Segment(spiked);
	bool stepped = false;
	for (int channel = 0; channel < CHANNELS; channel++) {
		for (int frame = 0; frame < FRAMES; frame++) {
			if (neuralGrid[channel][frame]->isSpiking()) {
				// store channel and frame in segment
				segment->add(new RowColumn(channel, frame));
				segmentGrid->set(channel, frame, neuralGrid[channel][frame]->segment);
				neuralGrid[channel][frame]->reset();
			}
			else {
				if (!stepped) {
					neuralGrid[channel][frame]->step();
					stepped = neuralGrid[channel][frame]->excitement >= SPIKE_THRESHOLD;
				}
			}
		}
	}
	if (segment->inserted < spiked) {
		print "NOT ENOUGH IN SEG\t" << spiked - segment->inserted endl;
	}
	(*segments).push_back(segment);
}

void LEGION::markLargestSegment() {
	//for (int channel = 0; channel < CHANNELS; channel++) {
	//	for (int frame = 0; frame < FRAMES; frame++) {
	//		if (neuralGrid[channel][frame]->segment == largestSegment) {
	//			neuralGrid[channel][frame]->largestSegment = true;
	//		}
	//	}
	//}
		//Segment seg = *(*segments)[largestSegment];
		//seg.largest = true;

}

boolGrid* LEGION::segmentsAsMask(int segment) {
	bool** grid = new bool*[CHANNELS];
	for (int channel = 0; channel < CHANNELS; channel++) {
		grid[channel] = new bool[FRAMES];
		for (int frame = 0; frame < FRAMES; frame++) {
			grid[channel][frame] = (*segmentGrid)(channel, frame) == segment;
		}
	}
	return new boolGrid(grid, CHANNELS, FRAMES);
}


void LEGION::saveSegmentGrid(std::string location) {
	segmentGrid->toFile(location);
}

LEGION::~LEGION()
{
	for (int i = 0; i < numSegments; i++) {
		(*segments)[i]->deleteSeg();
		delete ((*segments)[i]);
	}
	segments->clear();
	delete segments;

	for (int channel = 0; channel < CHANNELS; channel++) {
		for (int frame = 0; frame < FRAMES; frame++) {
			delete neuralGrid[channel][frame];
		}
		delete[] neuralGrid[channel];
	}
	delete[] neuralGrid;

	delete segmentGrid;
}
