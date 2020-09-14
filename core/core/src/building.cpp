#include "building.h"
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#include <SDL.h>
#include <gl3w.h>  




void Building::Execute()
{
	Mix_PlayMusic(gMusic, -1);
	const char* glsl_version = "#version 130";
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
	SDL_Window* window = SDL_CreateWindow("Dear ImGui SDL2+OpenGL3 example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);
	SDL_GLContext gl_context = SDL_GL_CreateContext(window);
	SDL_GL_MakeCurrent(window, gl_context);
	SDL_GL_SetSwapInterval(1); // Enable vsync
	gl3wInit();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
	ImGui_ImplOpenGL3_Init(glsl_version);
	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	bool done = false;
	while (!done)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			ImGui_ImplSDL2_ProcessEvent(&event);
			if (event.type == SDL_QUIT)
				done = true;
			if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
				done = true;
		}

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame(window);
		ImGui::NewFrame();

		
		{

			ImGui::Begin("Elevator Simulation System");                          

			ImGui::Text("Elevator Controller");               // Display some 

			ImGui::SliderInt("Elevator Number", &ElevatorNum, 3, 6);       
			ImGui::SliderInt("Simulation Time", &GlobalTimeCount, 500, 2000);
			ImGui::SliderInt("Floor Number", &FloorNum, 5, 17);

			if (ImGui::Button("Complete"))     
				done = true;
			
			ImGui::End();
		}

		// Rendering
		ImGui::Render();
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		SDL_GL_SwapWindow(window);
	}

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	SDL_GL_DeleteContext(gl_context);
	SDL_DestroyWindow(window);


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
			}

			// present the renderer to the window
			SDL_RenderPresent(gRenderer);
			SDL_Delay(100);
			
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
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
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

	gMusic = NULL;
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
		if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
		{
			printf("could initialize music: SDL_Mixer Error:%s\n", Mix_GetError());
		}
		else {
			gMusic = Mix_LoadMUS("bg.mp3");
			if (gMusic == NULL)
			{
				printf("fail to load music\n");
			}
		}
	}
	return success;
}



void Building::Close()
{
	gTextTexture.free();
	gElevatorTexture.free();
	
	Mix_FreeMusic(gMusic);
	gMusic = NULL;
	TTF_CloseFont(gFont);
	gFont = NULL;
	SDL_DestroyWindow(gWindow);
	SDL_DestroyRenderer(gRenderer);
	gWindow = NULL;
	gRenderer = NULL;
	SDL_Quit();
	IMG_Quit();
	TTF_Quit();
	Mix_Quit();

}





