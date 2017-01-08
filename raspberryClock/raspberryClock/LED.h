#ifndef LED_H
#define LED_H

#include "WiringPi.h"

class LED
{
public:
	LED();
	~LED();

	void Setup();
	bool IsOn() const  { return isOn_; }
	void On();
	void Off();

private:
	bool isOn_;
	WiringPi wiringPi_;
};

#endif // !LED_H

