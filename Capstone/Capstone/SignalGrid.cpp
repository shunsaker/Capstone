#include "stdafx.h"
#include "SignalGrid.h"


SignalBank& SignalGrid::operator[](int frame) {
	return *(grid[frame]);
}

SignalGrid::SignalGrid(SignalBank& signalBank, int frameSize, int frameOverlap)
	: FRAME_OFFSET(frameSize - frameOverlap),
	FRAME_SIZE(frameSize), FRAME_OVERLAP(frameOverlap),	SAMPLES(signalBank.SAMPLES), 
	SAMPLE_RATE(signalBank.SAMPLE_RATE), CHANNELS(signalBank.CHANNELS)
{
	//FRAMES = SAMPLES / FRAME_SIZE + SAMPLES / FRAME_SIZE / 2 + (SAMPLES % FRAME_SIZE >= FRAME_OVERLAP ? 1 : 0);

	FRAMES = FRAME_OFFSET == 0 ? SAMPLES / FRAME_SIZE : (SAMPLES - FRAME_SIZE) / FRAME_OFFSET + 1;
	//FRAMES -= 5;
	grid = new SignalBank*[FRAMES];
	HanningWindow window(frameSize);

	for (int frame = 0; frame < FRAMES; frame++) {
		SignalBank* bank = new SignalBank(CHANNELS, SAMPLE_RATE, SAMPLES);
		for (int channel = 0; channel < CHANNELS; channel++) {
			Signal* sig = new Signal(FRAME_SIZE, SAMPLE_RATE);
			for (int sample = 0; sample < FRAME_SIZE; sample++) {
				(*sig)[sample] = signalBank[channel][sample + FRAME_OFFSET * frame] * (*window.window)[sample];
			}
			//window.filter(*sig);
			bank->add(sig, channel);
		}
		grid[frame] = bank;
	}
	//print FRAMES end;
}

doubleGrid* SignalGrid::toSmrPower() {
	double** powerGrid = new double*[FRAMES];
	for (int frame = 0; frame < FRAMES; frame++) {
		double* powerColumn = new double[CHANNELS];
		powerGrid[frame] = powerColumn;
		for (int channel = 0; channel < CHANNELS; channel++){
			double power = 0;
			for (int i = 0; i < FRAME_SIZE; i++) {
				double sample = (*(grid[frame]))[channel][i];
				power += sample * sample;
			}
			powerColumn[channel] = sqrt(power);
		}
	}
	/*for (int i = 0; i < FRAMES; i++) {
		print minInArray(powerGrid[i], CHANNELS) << '\t' << maxInArray(powerGrid[i], CHANNELS) << '\t' << avgInArray(powerGrid[i], CHANNELS) end;
	}*/
	return new doubleGrid(powerGrid, FRAMES, CHANNELS);
}

void SignalGrid::deleteCell(int frame, int channel) {
	(*(grid[frame]))[channel].zeroOut();
}



SignalGrid::~SignalGrid()
{
	for (int i = 0; i < FRAMES; i++) {
		delete grid[i];
	}
}