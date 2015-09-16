#include "BridgeTrainer.h"


void end() {
	CoUninitialize();
}

int main() {
	atexit(end);
	srand(time(NULL));

	CoInitialize(NULL);
	
	BridgeTrainer bridgeTrainer;
	bridgeTrainer.train(1000);

	return 0;
}