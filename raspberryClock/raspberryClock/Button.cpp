#include "Button.h"

constexpr auto INT_EDGE_RISING = 2;
constexpr auto PIN = 28;

using namespace std;

Button::Button() : work_(false), buttonPressed_(false)
{

}

Button::~Button()
{
	ShutdownWorker();
}

void Button::Setup(handlerType clickHandler, handlerType holdHandler)
{
	wiringPi_.SetPinMode(PIN, 0);//input mode
	wiringPi_.pullUpDn(PIN, 2);//pull up
	clickHandler_ = clickHandler;
	holdHandler_ = holdHandler;
	StartWorker();
}

void Button::StartWorker()
{
	ShutdownWorker();
	work_ = true;
	worker_ = thread([this]()
	{
		while (work_)
		{
			holdCtr_ = 0;
			if (wiringPi_.DigitalRead(PIN) == 0)
			{
				while (wiringPi_.DigitalRead(PIN) == 0 && work_)
				{
					if (holdCtr_ == HOLD_TICKS)
						holdHandler_();

					++holdCtr_;
					wiringPi_.Delay(INTERVAL);
				}
				if(holdCtr_ < HOLD_TICKS)
					clickHandler_();
					
			}
			wiringPi_.Delay(INTERVAL);
		}
	});
}

void Button::ShutdownWorker()
{
	work_ = false;
	if (worker_.joinable())
		worker_.join();
}
