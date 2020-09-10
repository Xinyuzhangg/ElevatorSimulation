#include "passenger.h"

int GenerateRandom(int range, int init) {
	srand(time(NULL));
	return (rand() % range + init);
}

Passenger::Passenger()
{
	// Initialize start and end floor 

	startFloor = GenerateRandom(FloorNum, 1);
	endFloor = GenerateRandom(FloorNum, 1);
	while (startFloor == endFloor) {
		endFloor = GenerateRandom(FloorNum, 1);
	}
}
