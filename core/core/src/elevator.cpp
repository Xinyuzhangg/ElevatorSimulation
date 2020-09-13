#include "elevator.h"

bool operator == (const BookedFloor& b1, const BookedFloor& b2) {
	return (b2.bookedFloor == b1.bookedFloor) && (b2.buttonUp == b1.buttonUp);
}

bool operator<(const Floor& f1, const Floor& f2) {
	if (f1.targetDirectionUp && f2.targetDirectionUp) {
		return f1.targetFloor > f2.targetFloor;
	}
	else if (!f1.targetDirectionUp && !f2.targetDirectionUp) {
		return f1.targetFloor < f2.targetFloor;
	}
	else {
		return false;
	}
}



Elevator::Elevator(int startFloor)
{
	curStatus = ElevatorStatus::Closed;
	curMove = ElevatorMove::Idle;
	curFloor = startFloor;
	internalTimer = 0;
	curLoad = 0;

}

/**
 * Main function to process the elevator, checking waiting passengers and 
 * book floor so that the other elevators don't come to pick the same 
 * passengers
 */
void Elevator::MoveElevator(int curTime, 
	std::map<int, std::deque<Passenger>> &waitingPassengers,
	std::deque<BookedFloor> &bookedFloors)
{
	BookFloor(waitingPassengers, bookedFloors);
	if (IsReady()) {
		
		switch (curMove)
		{
		case ElevatorMove::Idle:
			TakeTime(ElevatorMove::Idle, curTime, waitingPassengers, bookedFloors);
			break;

		case ElevatorMove::Up:
			TakeTime(ElevatorMove::Up, curTime, waitingPassengers, bookedFloors);
			break;

		case ElevatorMove::Down:
			TakeTime(ElevatorMove::Down, curTime, waitingPassengers, bookedFloors);
			break;
		}
	}
	TickTimer();

}

/**
 * return the current floor of the elevator
 */
int Elevator::CurFloor()
{
	return curFloor;
}

/**
 * return the current moving condition of the elevator
 */
ElevatorMove Elevator::CurMove()
{
	return curMove;
}

/**
 * return the current the current status of the elevator
 */
ElevatorStatus Elevator::CurStatus()
{
	return curStatus;
}

/**
 * return the counting of the internal timer of the elevator
 */
int Elevator::InternalTimer()
{
	return internalTimer;
}

/**
 * return the number of passengers left in the elevator
 */
int Elevator::PassengerInElevator()
{
	return passengerInElevator.size();
}


/**
 * when the timer is ready take the corresponding action according to the 
 * current status and moving condition
 */
void Elevator::TakeTime(ElevatorMove move, int curTime,
	std::map<int, std::deque<Passenger>>& waitingPassengers, std::deque<BookedFloor>& bookedFloors)
{
	switch (curStatus)
	{
	case ElevatorStatus::Opened:
		if (IfPassengerOut()) {
			TakeOutPassenger();
			SetTimer(PassengerInOut);
		}
		else {
			if (IfPassengerIn(waitingPassengers) && !Overloaded()) {
				TakeInPassenger(waitingPassengers);
				SetTimer(PassengerInOut);
			} else
			{
				curStatus = ElevatorStatus::Closing;
				SetTimer(ElevatorOpenClose);
			}
		}

		break;
	case ElevatorStatus::Opening:
		curStatus = ElevatorStatus::Opened;
		break;
	case ElevatorStatus::Closed:
		if (!targetFloor.empty()) {
			if (IsTarget()) {
				popTarget();
				curStatus = ElevatorStatus::Opening;
				SetTimer(ElevatorOpenClose);
			}
			else {
				SetTimer(ElevatorUpDown);
				curStatus = ElevatorStatus::Moving;
				if (GoingUp())
				{
					curMove = ElevatorMove::Up;
				}
				else {
					curMove = ElevatorMove::Down;
				}
			}
		}
		else {
			curMove = ElevatorMove::Idle;
		}
		break;

	case ElevatorStatus::Closing:
		curStatus = ElevatorStatus::Closed;
		break;

	case ElevatorStatus::Moving:
		if (IsTarget()) {
			popTarget();
			RefreshBookedFloor(bookedFloors);
			curStatus = ElevatorStatus::Opening;
			SetTimer(ElevatorOpenClose);
		}
		else {
			if (move == ElevatorMove::Up) {
				SetTimer(ElevatorUpDown);
				curFloor++;
			} else if (move == ElevatorMove::Down)
			{
				SetTimer(ElevatorUpDown);
				curFloor--;
			}
		}
		break;
	}

}

/**
 * check if the elevator is overloaded
 */
bool Elevator::Overloaded()
{
	return curLoad == ElevatorLoad;
}

/**
 * if the timer is counting, tick the timer by one
 */
void Elevator::TickTimer()
{
	if (internalTimer > 0)
	internalTimer--;
}

/**
 * set how much time each action takes to the timer
 */
void Elevator::SetTimer(Action action)
{
	switch (action)
	{
	case PassengerInOut:
		internalTimer = TransitionTime;
		break;
	case ElevatorUpDown:
		internalTimer = ElevatorSpeed;
		break;
	case ElevatorOpenClose:
		internalTimer = DoorTime;
		break;
	}
}

/**
 * check if the current action is ready 
 */
bool Elevator::IsReady()
{
	return internalTimer == 0;
}

/**
 * check if there is passenger in the elevator reaching the target floor 
 */
bool Elevator::IfPassengerOut()
{
	if (passengerInElevator.find(curFloor) != passengerInElevator.end())
	{
		return true;
	}
	else {
		return false;
	}

}

/**
 * check if there is passenger going into the elevator in the current floor
 */
bool Elevator::IfPassengerIn(std::map<int, std::deque<Passenger>>& waitingPassengers)
{
	return waitingPassengers.find(curFloor) != waitingPassengers.end();
}

/**
 * as there are passengers in the elevator, take on of them out of the 
 * elevator
 */
void Elevator::TakeOutPassenger()
{
	std::map<int, std::stack<Passenger>>::iterator itr;
	itr = passengerInElevator.find(curFloor);
	itr->second.pop();
	if (itr->second.empty())
	{
		passengerInElevator.erase(itr);
	}
	
}

/**
 * as there are passengers waiting outside, take one of them into the 
 * elevator
 */
void Elevator::TakeInPassenger(std::map<int, std::deque<Passenger>>& waitingPassengers)
{
	std::map<int, std::deque<Passenger>>::iterator itr;
	itr = waitingPassengers.find(curFloor);
	Passenger temp_passenger = itr->second.front();
	int endFloor = temp_passenger.GetEndFloor();
	Floor floor(endFloor, (endFloor - curFloor) > 0);
	targetFloor.push(floor);
	if (passengerInElevator.find(endFloor) != passengerInElevator.end())
	{
		passengerInElevator[endFloor].push(itr->second.front());
	}
	else {
		std::stack<Passenger> temp_stack;
		temp_stack.push(itr->second.front());
		passengerInElevator[endFloor] = temp_stack;
	}
	//TODO
	itr->second.pop_front();
	if (itr->second.empty())
	{
		waitingPassengers.erase(itr);
	}
}


/**
 * check is the current floor is target floor 
 */
bool Elevator::IsTarget()
{
	return curFloor == targetFloor.top().targetFloor;
}

/**
 * check if the elevator is going up
 */
bool Elevator::GoingUp()
{
	return targetFloor.top().targetDirectionUp;
}

/**
 * take the target floor into the booked floor deque
 */
void Elevator::BookFloor(std::map<int, std::deque<Passenger>>& waitingPassengers, std::deque<BookedFloor>& bookedFloors)
{
	std::map<int, std::deque<Passenger>>::iterator itr;
	bool buttonUp = false;
	bool buttonDown = false;
	for (itr = waitingPassengers.begin(); itr != waitingPassengers.end(); itr++) 
	{
		std::deque<Passenger>::iterator queItr;
		bool directionUp = ((itr->first - curFloor) > 0);
		Floor target(itr->first, directionUp);
		for (queItr = itr->second.begin(); queItr != itr->second.end(); queItr++) 
		{
			if (queItr->Goup())
				buttonUp = true;
			else buttonDown = true;
		}
		if (buttonUp) {
			BookedFloor booked(itr->first, true);
			if (!IsFloorBooked(booked, bookedFloors))
			{
				if (!targetFloor.empty())
				{
					if (directionUp && targetFloor.top().targetDirectionUp && buttonUp)
					{
						bookedFloors.push_back(booked);
						targetFloor.push(target);
					}
				}
				else {
					bookedFloors.push_back(booked);
					targetFloor.push(target);
				}
			}
		}
		else if (buttonDown) {
			BookedFloor booked(itr->first, false);
			if (!IsFloorBooked(booked, bookedFloors))
			{
				if (!targetFloor.empty())
				{
					if (!directionUp && !targetFloor.top().targetDirectionUp)
					{
						bookedFloors.push_back(booked);
						targetFloor.push(target);
					}
				}
				else {
					bookedFloors.push_back(booked);
					targetFloor.push(target);
				}
			}
		}
	}
	
}

/**
 * check if floor is booked
 */
bool Elevator::IsFloorBooked(BookedFloor& floor, std::deque<BookedFloor>& bookedFloors)
{

	return (std::find(bookedFloors.begin(), bookedFloors.end(), floor) != bookedFloors.end());
}

/**
 * when the elevator reaches the target floor, take the target floor out
 * of the booked floor deque
 */
void Elevator::RefreshBookedFloor(std::deque<BookedFloor>& bookedFloors)
{
	std::deque<BookedFloor>::iterator itr;
	if (curMove == ElevatorMove::Down) {
		BookedFloor floor(curFloor, false);
		itr = std::find(bookedFloors.begin(), bookedFloors.end(), floor);
		
	}
	if (curMove == ElevatorMove::Up) {
		BookedFloor floor(curFloor, true);
		itr = std::find(bookedFloors.begin(), bookedFloors.end(), floor);
	}
	if (itr != bookedFloors.end())
	{
		bookedFloors.erase(itr);
	}
	
}

/**
 * when the elevator has reached the target floor, take the current
 * floor out the target floor queue
 */
void Elevator::popTarget()
{
	bool popping = true;
	while (popping)
	{
		if (targetFloor.empty())
		{
			popping = false;
		}
		else {
			if (targetFloor.top().targetFloor == curFloor)
			{
				targetFloor.pop();
			}
			else {
				popping = false;
			}
		}
	}
}

