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

void Button::Setup(std::function<void(void)> handler)
{
	wiringPi_.SetPinMode(PIN, 0);//input mode
	wiringPi_.pullUpDn(PIN, 2);//pull up
	handler_ = handler;
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
			if (wiringPi_.DigitalRead(PIN) == 0)
			{
				handler_();//call handler

				//end if button released
				while (wiringPi_.DigitalRead(PIN) == 0 && work_)
					wiringPi_.Delay(100);
			}

			wiringPi_.Delay(100);
		}
	});
}

void Button::ShutdownWorker()
{
	work_ = false;
	if (worker_.joinable())
		worker_.join();
}
