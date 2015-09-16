#ifndef BRIDGETRAINER
#define BRIDGETRAINER

#include <Windows.h>

#import "msado15.dll" \
	no_namespace \
	rename("EOF","adoEOF")

#include <string>
#include "BridgeModel.h"

using namespace std;


class BridgeNode {

public:
	time_t timestamp;
	double util;

	BridgeNode(const char infoSet[INFOSETLEN], bool isBanker, int actionsNum);
	void getStrategy(double strategy[13]);
	void accRegret(double regret[13]);

private:
	bool fromDB, isBanker;
	char infoSet[INFOSETLEN];
	int actionsNum;
	double regretSum[13];
};

class BridgeTrainer {

public:
	BridgeTrainer();
	~BridgeTrainer();

	void storeGameInfoIntoDB();
	void restoreGameInfoFromDB();

	double cfr();
	void train(int iterations);

private:
	BridgeModel bridgeModel;
	time_t timestamp;
};

#endif