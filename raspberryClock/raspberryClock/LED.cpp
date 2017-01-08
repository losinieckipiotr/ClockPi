#include "LED.h"

#include <iostream>

constexpr auto LED_PIN = 25;
constexpr auto OUTPUT = 1;

LED::LED()
{

}

LED::~LED()
{
	Off();
}

void LED::Setup()
{
    wiringPi_.SetPinMode(LED_PIN, OUTPUT);
	wiringPi_.DigitalWrite(LED_PIN, 0);
	isOn_ = false;
}

void LED::On()
{
	if (!isOn_)
		wiringPi_.DigitalWrite(LED_PIN, 1);
	isOn_ = true;

	using namespace std;
	cout << "LED ON" << endl;
}

void LED::Off()
{
	if (isOn_)
		wiringPi_.DigitalWrite(LED_PIN, 0);
	isOn_ = false;

	using namespace std;
	cout << "LED OFF" << endl;
}
