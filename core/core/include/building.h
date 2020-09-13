#pragma once
#include <stack>
#include <queue>
#include <vector>
#include <map>
#include "elevator.h"
#include "texture.h"

extern int PassengerNum;
extern int FloorNum;
extern int ElevatorNum;
extern int GlobalTimeCount;

const int WINDOW_WIDTH = 1080;
const int WINDOW_HEIGHT = 720;


class Building {
private:
	int totalPassengers;
	int leftPassengers;
	std::deque<BookedFloor> bookedFloors;
	std::map<int ,std::deque<Passenger>> passengersInBuilding;
	std::vector<Elevator> elevatorsInBuilding;

private:
	SDL_Renderer* gRenderer;
	SDL_Window* gWindow;
	TTF_Font* gFont;
	LTexture gTextTexture;
	LTexture gElevatorTexture;
	SDL_Rect  gClips[4];




private:
	void LoadPassenagers();
	void ClearBuilding();
	bool RenderInit();
	bool LoadMedia();


public:
	Building();

public:
	bool Init();
	void Execute();
	void Close();


};