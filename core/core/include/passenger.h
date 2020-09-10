#pragma once

extern int FloorNum;

#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <stack>
#include <queue>
#include <vector>
#include <map>

int GenerateRandom(int range, int init);

class Passenger
{
public:
	Passenger();

	int GetStartFloor();
	int GetEndFloor();
	bool Goup();

private:
	int endFloor;
	int startFloor;
};



inline int Passenger::GetStartFloor()
{
	return startFloor;
}

inline int Passenger::GetEndFloor()
{
	return endFloor;
}

inline bool Passenger::Goup()
{
	return ((endFloor - startFloor) > 0);
}
