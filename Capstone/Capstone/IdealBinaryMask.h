#pragma once
#include "Definitions.h"
#include "Cochleagram.h"
#include "SignalGrid.h"
#include "SignalBank.h"

class IdealBinaryMask
{
private:
	doubleGrid* diffGrid(doubleGrid& powerGrid1, doubleGrid& powerGrid2);
	doubleGrid* simpleRatio(doubleGrid& powerGrid1, doubleGrid& powerGrid2);
	doubleGrid* gridOfRelativeDecibels(doubleGrid& powerGrid1, doubleGrid& powerGrid2);
	doubleGrid* gridOfRelativeDecibelRatios(doubleGrid& powerGrid1, doubleGrid& powerGrid2);
	boolGrid* createIdealBinaryMask(doubleGrid& decibelGrid, double decibelReq);
	Signal& signal1;
	Signal& signal2;

public:
	boolGrid* idealBinaryMask1;
	boolGrid* idealBinaryMask2;

	IdealBinaryMask(Signal& sig1, Signal& sig2);
	Signal* applyIdealBinaryMask(boolGrid* mask, bool normalize = true);
	void saveIdealBinaryMask(std::string name, boolGrid* mask, double normal = 1);
	static Signal* applyIdealBinaryMask(boolGrid* mask, Signal* signal, bool normalize = true);
	static void saveIdealBinaryMask(std::string name, boolGrid* mask, Signal* signal);
	static void writeBinaryMask(std::ostream& os, boolGrid* mask);
	~IdealBinaryMask();
};

