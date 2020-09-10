#pragma once
#include <stack>
#include <queue>
#include <vector>
#include <map>
#include "elevator.h"

extern int PassengerNum;
extern int FloorNum;
extern int ElevatorNum;
extern int GlobalTimeCount;


class Building {
private:
	int totalPassengers;
	int leftPassengers;
	std::deque<BookedFloor> bookedFloors;
	std::map<int ,std::deque<Passenger>> passengersInBuilding;
	std::vector<Elevator> elevatorsInBuilding;
	



private:
	void LoadPassenagers();
	void ClearBuilding();
	bool IsElevatorEmpty();


public:
	
	Building();
public:
	bool Init();
	bool Execute();
	void Render();
	void Close();


};