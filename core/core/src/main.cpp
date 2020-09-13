#include "building.h"


int PassengerNum = 45;
int FloorNum = 10;
int ElevatorNum = 6;
int ElevatorLoad = 15;
int ElevatorSpeed = 3;
int TransitionTime = 5;
int DoorTime = 3;
int GlobalTimeCount = 1500;

int main(int argc, char* argv[]) {

	Building building;
	if (!building.Init()) {
		printf("Fail to initialize building!\n");
	}
	building.Execute();
	building.Close();


	return 0;
}