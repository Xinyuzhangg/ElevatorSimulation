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

void Elevator::MoveElevator(int curTime, 
	std::map<int, std::deque<Passenger>> &waitingPassengers,
	std::deque<BookedFloor> &bookedFloors)
{
	TickTimer();
	if (IsReady()) {

		BookFloor(waitingPassengers, bookedFloors);
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
}



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
				curStatus = ElevatorStatus::Opening;
				SetTimer(ElevatorOpenClose);
			}
			else {
				SetTimer(ElevatorUpDown);
				curStatus = ElevatorStatus::Moving;
				if (GoingUp())
				{
					move = ElevatorMove::Up;
				}
				else {
					move = ElevatorMove::Down;
				}
			}
		}
		else {
			move = ElevatorMove::Idle;
		}
		break;

	case ElevatorStatus::Closing:
		curStatus = ElevatorStatus::Closed;
		break;

	case ElevatorStatus::Moving:
		if (IsTarget()) {
			targetFloor.pop();
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

bool Elevator::Overloaded()
{
	return curLoad == ElevatorLoad;
}

void Elevator::TickTimer()
{
	internalTimer--;
}

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

bool Elevator::IsReady()
{
	return internalTimer == 0;
}

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

bool Elevator::IfPassengerIn(std::map<int, std::deque<Passenger>>& waitingPassengers)
{
	return waitingPassengers.find(curFloor) != waitingPassengers.end();
}

void Elevator::TakeOutPassenger()
{
	std::map<int, std::stack<Passenger>>::iterator itr;
	itr = passengerInElevator.find(curFloor);
	passengerInElevator.erase(itr);
}

void Elevator::TakeInPassenger(std::map<int, std::deque<Passenger>>& waitingPassengers)
{
	std::map<int, std::deque<Passenger>>::iterator itr;
	itr = waitingPassengers.find(curFloor);
	Passenger temp_passenger = itr->second.front();
	
	Floor floor(temp_passenger.GetEndFloor(), (temp_passenger.GetEndFloor() - curFloor) > 0);
	targetFloor.push(floor);
	if (IfPassengerIn(waitingPassengers))
	{
		passengerInElevator[curFloor].push(itr->second.front());
	}
	else {
		std::stack<Passenger> temp_stack;
		temp_stack.push(itr->second.front());
		passengerInElevator[curFloor] = temp_stack;
	}
}



bool Elevator::IsTarget()
{
	return curFloor == targetFloor.top().targetFloor;
}

bool Elevator::GoingUp()
{
	return targetFloor.top().targetDirectionUp;
}

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
					if (directionUp && targetFloor.top().targetDirectionUp)
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

bool Elevator::IsFloorBooked(BookedFloor& floor, std::deque<BookedFloor>& bookedFloors)
{

	return (std::find(bookedFloors.begin(), bookedFloors.end(), floor) != bookedFloors.end());
}

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
	bookedFloors.erase(itr);
}

