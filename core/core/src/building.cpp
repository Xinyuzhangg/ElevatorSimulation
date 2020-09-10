#include "building.h"




bool Building::Execute()
{
	// Set global timer
	int timer = 0;
	bool success = true;
	while (timer < GlobalTimeCount) {
		LoadPassenagers();
		for (auto elevator = elevatorsInBuilding.begin(); elevator != elevatorsInBuilding.end(); ++elevator) {
			elevator->MoveElevator(timer, passengersInBuilding, bookedFloors);
		}
		timer++;
		Render();
		ClearBuilding();
	}
	return success;

}



void Building::LoadPassenagers()
{
	// Get passengers from passengers left
	int loadedPassnegers = GenerateRandom(2, 0);


	// initialize all passengers
	for (int i = 0; i < loadedPassnegers; i++) {
		Passenger temp_passenger;
		if (passengersInBuilding.find(temp_passenger.GetStartFloor()) != passengersInBuilding.end()) {
			passengersInBuilding[i].push_back(temp_passenger);
		}
		else {
			std::deque<Passenger> temp_stack;
			temp_stack.push_back(temp_passenger);
			passengersInBuilding[i] = temp_stack;
		}
	}

}

void Building::ClearBuilding()
{
	std::map<int, std::deque<Passenger>>::iterator itr;
	std::stack<std::map<int, std::deque<Passenger>>::iterator> t;

	for (itr = passengersInBuilding.begin(); itr != passengersInBuilding.end(); itr++) {
		if (itr->second.empty()) {
			t.push(itr);
		}
	}
	while (!t.empty())
	{
		passengersInBuilding.erase(t.top());
		t.pop();
	}
}

bool Building::IsElevatorEmpty()
{
	// TODO: check is all elevators are empty
	return false;
}

Building::Building() {
	totalPassengers = PassengerNum;
	leftPassengers = PassengerNum;
}

bool Building::Init()
{
	bool success = true;
	
	// Initialize all elevators 
	for (int i = 0; i < ElevatorNum; i++) {
		if (i < ElevatorNum / 3) {
			Elevator temp_elevator(1);
			elevatorsInBuilding.push_back(temp_elevator);
		}
		else if (i < (ElevatorNum * 2 / 3)) {
			Elevator temp_elevator(ElevatorNum / 2);
			elevatorsInBuilding.push_back(temp_elevator);
		}
		else {
			Elevator temp_elevator(FloorNum);
			elevatorsInBuilding.push_back(temp_elevator);
		}

	}
	return success;
}

void Building::Render()
{
	


}

void Building::Close()
{

}

