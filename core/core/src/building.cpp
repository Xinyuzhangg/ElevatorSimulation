#include "building.h"




void Building::Execute()
{
	// Set global timer
	int timer = 0;
	bool running = true;
	SDL_Event event;
	
	while (running) {
		if (timer > GlobalTimeCount)
		{
			running = false;
		}
		else {
			while (SDL_PollEvent(&event) != 0)
			{
				if (event.type == SDL_QUIT)
				{
					running = false;
				}
			}
			// draw the white background
			SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
			SDL_RenderClear(gRenderer);
			// draw the floors 
			SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0xFF);
			for (int i = 0; i < FloorNum; i++)
			{
				int height = i * WINDOW_HEIGHT / FloorNum + gElevatorTexture.getHeight() / 8;
				SDL_RenderDrawLine(gRenderer, 0, height, WINDOW_WIDTH, height);
			}
			// set the title color and draw title 
			SDL_Color textColor = { 0, 0, 0 };
			gTextTexture.loadText(gFont, gRenderer, "fancy elevator", textColor);
			gTextTexture.render(gRenderer, (WINDOW_WIDTH - gTextTexture.getWidth()) / 2, 0);
			// load passengers into the elevator
			LoadPassenagers();
			// get the width and height of elevators
			int w = gElevatorTexture.getWidth();
			int h = gElevatorTexture.getHeight();
			int temp;
			int index = 0;
			//individually operate on and render each elevator
			textColor = { 0xFF, 0, 0 };
			for (auto elevator = elevatorsInBuilding.begin();
				// operate on each elevator
				elevator != elevatorsInBuilding.end(); ++elevator) {
				elevator->MoveElevator(timer,
					passengersInBuilding, bookedFloors);
				// calculate the specific x, y position of the elevator
				int x = index * WINDOW_WIDTH / ElevatorNum + gElevatorTexture.getWidth() / 2;
				int y = (FloorNum - elevator->CurFloor()) * WINDOW_HEIGHT / FloorNum - gElevatorTexture.getHeight() / 8;

				// get the elevator information and output to the screen
				std::string str = "Passenger Number:  " + std::to_string(elevator->PassengerInElevator());
				gTextTexture.loadText(gFont, gRenderer, str, textColor);
				gTextTexture.render(gRenderer, x, 50);

				// render elevator based on its status
				switch (elevator->CurStatus())
				{
				default:
					gElevatorTexture.render(gRenderer, x, y, &gClips[0]);
					break;
				case ElevatorStatus::Closing:
					temp = elevator->InternalTimer();
					gElevatorTexture.render(gRenderer, x, y, &gClips[temp]);
					break;
				case ElevatorStatus::Opened:
					gElevatorTexture.render(gRenderer, x, y, &gClips[3]);
					break;
				case ElevatorStatus::Opening:
					temp = 3 - elevator->InternalTimer();
					gElevatorTexture.render(gRenderer, x, y, &gClips[temp]);
					break;
				}
				// switch to the next elevator
				index++;
				SDL_Delay(20);
			}

			// present the renderer to the window
			SDL_RenderPresent(gRenderer);
			
			timer++;			
		}
	}
	//ClearBuilding();
	

}



void Building::LoadPassenagers()
{
	// Get passengers from passengers left
	int loadedPassnegers = GenerateRandom(2, 0);


	// initialize all passengers
	for (int i = 0; i < loadedPassnegers; i++) {
		Passenger temp_passenger;
		if (passengersInBuilding.find(temp_passenger.GetStartFloor()) != passengersInBuilding.end()) {
			passengersInBuilding[temp_passenger.GetStartFloor()].push_back(temp_passenger);
		}
		else {
			std::deque<Passenger> temp_stack;
			temp_stack.push_back(temp_passenger);
			passengersInBuilding[temp_passenger.GetStartFloor()] = temp_stack;
		}
	}

}

void Building::ClearBuilding()
{
// 	std::map<int, std::deque<Passenger>>::iterator itr;
// 	std::stack<std::map<int, std::deque<Passenger>>::iterator> t;
// 
// 	for (itr = passengersInBuilding.begin(); itr != passengersInBuilding.end(); itr++) {
// 		if (itr->second.empty()) {
// 			t.push(itr);
// 		}
// 	}
// 	while (!t.empty())
// 	{
// 		passengersInBuilding.erase(t.top());
// 		t.pop();
// 	}
}


bool Building::RenderInit()
{
	bool success = true;
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("couldn't initialize! SDL error:%s\n", SDL_GetError());
		success = false;
	}
	else {
		SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
		gWindow = SDL_CreateWindow("window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE);
		if (gWindow == NULL) {
			printf("couldn't create window! SDL error:%s\n", SDL_GetError());
			success = false;
		}
		else {
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
			if (gRenderer == NULL)
			{
				printf("couldn't create renderer!:SDL error:%s\n", SDL_GetError());
				success = false;
			}
			else {
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
				int imageFlag = IMG_INIT_PNG;
				if (!(IMG_Init(imageFlag) & imageFlag)) {
					printf("couldn't initialize image! SDL error:%s\n", SDL_GetError());
					success = false;
				}
				if (TTF_Init() == -1)
				{
					printf("couldn't initialize TTF! SDL error:%s\n", SDL_GetError());
					success = false;
				}
			}
		}

	}
	return success;
}

bool Building::LoadMedia()
{
	bool success = true;
	gFont = TTF_OpenFont("cool.ttf", 15);
	if (gFont == NULL || !gElevatorTexture.loadFile(gRenderer, "ele.png")) {
		printf("couldn't load font and file!%s%s\n", SDL_GetError(), TTF_GetError());
		success = false;
	}
	else
	{
		//Set top left sprite
		SDL_Color textColor = { 0xF0, 0x00, 0x00 };

		gClips[0].x = 0;
		gClips[0].y = 0;
		gClips[0].w = gElevatorTexture.getWidth();
		gClips[0].h = gElevatorTexture.getHeight() / 4;

		//Set top right sprite
		gClips[1].x = 0;
		gClips[1].y = gElevatorTexture.getHeight() / 4;
		gClips[1].w = gElevatorTexture.getWidth();
		gClips[1].h = gElevatorTexture.getHeight() / 4;

		//Set bottom left sprite
		gClips[2].x = 0;
		gClips[2].y = gElevatorTexture.getHeight() * 2 / 4;
		gClips[2].w = gElevatorTexture.getWidth();
		gClips[2].h = gElevatorTexture.getHeight() / 4;

		//Set bottom right sprite
		gClips[3].x = 0;
		gClips[3].y = gElevatorTexture.getHeight() * 3 / 4;
		gClips[3].w = gElevatorTexture.getWidth();
		gClips[3].h = gElevatorTexture.getHeight() / 4;
		
	}

	return success;
}

Building::Building() {
	totalPassengers = PassengerNum;
	leftPassengers = PassengerNum;
	gFont = NULL;
	gRenderer = NULL;
	gWindow = NULL;
	gTextTexture = LTexture();
	gElevatorTexture = LTexture();
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
			Elevator temp_elevator(FloorNum / 2);
			elevatorsInBuilding.push_back(temp_elevator);
		}
		else {
			Elevator temp_elevator(FloorNum);
			elevatorsInBuilding.push_back(temp_elevator);
		}
	}
	if (!RenderInit())
	{
		printf("couldn't initialize SDL\n");
		success = false;
	}
	else {
		if (!LoadMedia()) {
			printf("couldn't load media\n");
			success = false;
		}
	}
	return success;
}



void Building::Close()
{
	gTextTexture.free();
	gElevatorTexture.free();
	TTF_CloseFont(gFont);
	gFont = NULL;
	SDL_DestroyWindow(gWindow);
	SDL_DestroyRenderer(gRenderer);
	gWindow = NULL;
	gRenderer = NULL;
	SDL_Quit();
	IMG_Quit();
	TTF_Quit();

}




