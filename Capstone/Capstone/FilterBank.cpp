#include "stdafx.h"
#include "FilterBank.h"


FilterBank::FilterBank(int channels, int lowFreq, int highFreq, int sampleRate) : 
	CHANNELS(channels), LOW(lowFreq), HIGH(highFreq), SAMPLE_RATE(sampleRate) {
	bank = new GammatoneFilter*[CHANNELS];

	double lowErb = freqToErbScale(LOW);
	double highErb = freqToErbScale(HIGH);
	//print lowErb << " to " << highErb endl;

	double* erbScale = logSpacedErbScale(lowErb, highErb);
	for (int i = 0; i < CHANNELS; i++) {
		int frequency = erbScaleToFreq(erbScale[i]);
		double bandwith = erb(frequency) * bandwidthCorrection;
		//print i << "\t" << frequency << "\t" << erbScale[i] << "\t" << bandwith endl;
		bank[i] = new GammatoneFilter(frequency, bandwith, sampleRate);
	}

	delete[] erbScale;
}

SignalBank* FilterBank::filter(Signal& signal){
	print "filterbank" endl;
	channels = new SignalBank(CHANNELS, SAMPLE_RATE, signal.SAMPLES);
	for (int i = 0; i < CHANNELS; i++) {
		Signal* filtered = bank[i]->filter(signal); //(*bank)->filter(signal);
		channels->add(filtered, i);
	}
	return channels;
}

void FilterBank::filter(SignalBank& signal) {
	for (int i = 0; i < CHANNELS; i++) {
		Signal* filtered = (*bank)->filter(signal[i]);
		signal.add(filtered, i);
	}
}


Signal* FilterBank::reverse(SignalBank& inputBank) {
	Signal* signal = new Signal(inputBank.SAMPLES, inputBank.SAMPLE_RATE);
	for (int i = 0; i < CHANNELS; i++) {
		Signal channel = inputBank[i];
		inputBank[i].reverse();
		inputBank.add((*bank)->filter(inputBank[i]), i);
		inputBank[i].reverse();
	}
	return signal;

}

double FilterBank::erb(int frequency){
	return 24.7 * ( 4.37e-3 * frequency + 1);
}

double FilterBank::freqToErbScale(int frequency){
	double f = 4.37e-3 * frequency + 1;
	return log10(f) * 21.4;
}

int FilterBank::erbScaleToFreq(double erb){
	double e = erb / 21.4;
	return round((pow(10, e) - 1) / 4.37e-3);
}

double* FilterBank::logSpacedErbScale(double minErb, double maxErb){
	double erbRange = maxErb - minErb;
	double erbStep = erbRange / (CHANNELS - 1);
	double* spacedErb = new double[CHANNELS];

	spacedErb[0] = minErb;
	for (int i = 1; i < CHANNELS; i++) {
		spacedErb[i] = spacedErb[i - 1] + erbStep;
	}
	return spacedErb;
}

FilterBank::~FilterBank()
{
	for (int i = 0; i < CHANNELS; i++) {
		delete bank[i];
	}
}
