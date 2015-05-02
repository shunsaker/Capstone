#include "stdafx.h"
#include "crossCorrelationSegmentation.h"


crossCorrelationSegmentation::crossCorrelationSegmentation(SignalGrid& correlogram)
	: correlogram(correlogram)
{
	print "cross correlation" end;
	segmentGrid = new intGrid(correlogram.CHANNELS, correlogram.FRAMES, UNASSIGNED);
	for (int row = 0; row < correlogram.CHANNELS; row++) {
		for (int col = 0; col < correlogram.FRAMES; col++) {
			traverse(row, col);
		}
		print row end;
	}
}

void crossCorrelationSegmentation::traverse(int channel, int frame) {
	Signal& thisCorrelation = correlogram[frame][channel];
	if (thisCorrelation[0] == 0){
		segmentGrid->set(channel, frame, 0);
		return;
	}
	int group = (*segmentGrid)(channel, frame);
	bool matchFound = false;
	int nextFrame = frame - 1;
	int nextChannel = channel + 0;
	double threshold = .90;

	if (nextFrame >= 0) {
		if ((*segmentGrid)(nextChannel, nextFrame) == UNASSIGNED) {
			Signal& nextCorrelation = correlogram[nextFrame][nextChannel];
			double relation = crossCorrelation(thisCorrelation, nextCorrelation);
			if (relation > threshold) {
				if (!matchFound) group = group > UNASSIGNED ? group : ++groups;
				(*segmentGrid).set(channel, frame, group);
				(*segmentGrid).set(nextChannel, nextFrame, group);
				traverse(nextChannel, nextFrame);
				matchFound = true;
			}
		}
	}

	nextFrame = frame + 1;
	if (nextFrame < correlogram.FRAMES) {
		if ((*segmentGrid)(nextChannel, nextFrame) == UNASSIGNED) {
			Signal& nextCorrelation = correlogram[nextFrame][nextChannel];
			double relation = crossCorrelation(thisCorrelation, nextCorrelation);
			if (relation > threshold) {
				if (!matchFound) group = group > UNASSIGNED ? group : ++groups;
				(*segmentGrid).set(channel, frame, group);
				(*segmentGrid).set(nextChannel, nextFrame, group);
				traverse(nextChannel, nextFrame);
				matchFound = true;
			}
		}
	}

	nextFrame = frame;
	nextChannel = channel + 1;
	if (nextChannel < correlogram.CHANNELS) {
		if ((*segmentGrid)(nextChannel, nextFrame) == UNASSIGNED) {
			Signal& nextCorrelation = correlogram[nextFrame][nextChannel];
			double relation = crossCorrelation(thisCorrelation, nextCorrelation);
			if (relation > threshold) {
				if (!matchFound) group = group > UNASSIGNED ? group : ++groups;
				(*segmentGrid).set(channel, frame, group);
				(*segmentGrid).set(nextChannel, nextFrame, group);
				traverse(nextChannel, nextFrame);
				matchFound = true;
			}
		}
	}

	nextChannel = channel - 1;
	if (nextChannel > 0) {
		if ((*segmentGrid)(nextChannel, nextFrame) == UNASSIGNED) {
			Signal& nextCorrelation = correlogram[nextFrame][nextChannel];
			double relation = crossCorrelation(thisCorrelation, nextCorrelation);
			if (relation > threshold) {
				if (!matchFound) group = group > UNASSIGNED ? group : ++groups;
				(*segmentGrid).set(channel, frame, group);
				(*segmentGrid).set(nextChannel, nextFrame, group);
				traverse(nextChannel, nextFrame);
				matchFound = true;
			}
		}
	}

	// check up
	// check down
	// check left
	// check right
}

double crossCorrelationSegmentation::crossCorrelation(Signal& one, Signal& two) {
	if (one[0] == 0 || two[0] == 0)
		return 0;

	double sum = 0;
	for (int i = 0; i < one.SAMPLES - 1; i++) {
		sum += one[i] * two[i + 1];
	}
	return sum / one.SAMPLES;
}


boolGrid* crossCorrelationSegmentation::getBinaryMask(int group) {
	bool** binaryMask = new bool*[segmentGrid->ROWS];
	for (int row = 0; row < segmentGrid->ROWS; row++) {
		binaryMask[row] = new bool[segmentGrid->COLUMNS];
		for (int col = 0; col < segmentGrid->COLUMNS; col++) {
			binaryMask[row][col] = (*segmentGrid)(row, col) == group;
			//if(binaryMask[row][col]) print group end;
		}
	}
	boolGrid* idealBinaryMask1 = new boolGrid(binaryMask, segmentGrid->ROWS, segmentGrid->COLUMNS);
	return idealBinaryMask1;
}

void crossCorrelationSegmentation::writeSegmentText(std::ostream& os)
{
	for (int i = segmentGrid->ROWS - 1; i >= 0 ; --i)
	{
		for (int j = 0; j < segmentGrid->COLUMNS; ++j)
		{
			int length = std::to_string((*segmentGrid)(i, j)).length();
			os << (*segmentGrid)(i, j);
			for (int x = 0; x < 5 - length; x++) {
				os << " ";
			}
		}
		os << "\n";
	}
}

crossCorrelationSegmentation::~crossCorrelationSegmentation()
{
}
