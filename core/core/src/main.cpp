#include "building.h"


int PassengerNum = 45;
int FloorNum = 15;
int ElevatorNum = 3;
int ElevatorLoad = 15;
int ElevatorSpeed = 3;
int TransitionTime = 5;
int DoorTime = 3;
int GlobalTimeCount = 5000;

int main(int argc, char* argv[]) {
	Building building;
	if (!building.Init()) {
		printf("Fail to initialize building!\n");
	}
	if (!building.Execute()) {
		printf("Fail to execute building!\n");
	}
	building.Close();


	return 0;
}