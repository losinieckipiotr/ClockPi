#include "Buzzer.h"

using namespace std;

constexpr auto BEEP = 64 + 7;

Buzzer::Buzzer() : workerFlag(false), period_(DEFAULT_PERIOD)
{

}

Buzzer::~Buzzer()
{
	Off();
}

void Buzzer::Setup()
{
	wiringPi_.PCF8574Setup();
}

void Buzzer::On()
{
	if (!workerFlag)
	{
		Off();
		workerFlag = true;
		worker_ = thread([this]() { workerFunc(); });
	}
}

void Buzzer::Off()
{
	workerFlag = false;
	if (worker_.joinable())
		worker_.join();
}

void Buzzer::workerFunc() const
{
	while (workerFlag)
	{
		wiringPi_.DigitalWrite(BEEP, 0);
		wiringPi_.Delay(period_ / 2);
		wiringPi_.DigitalWrite(BEEP, 1);
		wiringPi_.Delay(period_ / 2);
	}
	wiringPi_.DigitalWrite(BEEP, 1);
}
