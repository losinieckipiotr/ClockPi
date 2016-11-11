#ifndef APPLICATION_H
#define APPLICATION_H

#include "WiringPi.h"

#include "SSD1306.h"
#include "BMP180.h"

class Application
{
public:
	Application();
	~Application();

	void Start();

private:
	WiringPi& wiringPi_;

	OLED::SSD1306 screen_;
	BMP180 sensor_;
};

#endif // !APPLICATION_H

