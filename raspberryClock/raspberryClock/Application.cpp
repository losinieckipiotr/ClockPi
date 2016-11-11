#include "Application.h"

#ifndef FAKE_WP
#include <wiringPi.h>
#include <wiringPiI2C.h>
#else
#include "fakeWiringPi.hpp"
#endif // FAKE_WP

#include <iostream>
#include <iomanip>
#include <string>

using namespace std;

Application::Application() : wiringPi_(WiringPi::GetInstance())
{

}

Application::~Application()
{

}

void Application::Start()
{
	wiringPi_.Setup();

	screen_.Begin();

	OLED::Font16x8 font;
	const auto h1 = 16;
	const auto h2 = 48;

	sensor_.SetupI2C();
	sensor_.Calibrate();

	float temp;
	float pressure;

    while(1)
    {
		temp = sensor_.ReadTemperature();
		pressure = sensor_.ReadPressure() / 100.0f;

		screen_.Write(0, h1, "T: " + to_string(temp) + " C", font);
		screen_.Write(0, h2, "P: " + to_string(temp) + " Pa", font);
		screen_.Display();

        cout << "Temperature: " << setprecision(5) << temp << " C" << endl;
        cout << "Pressure: " << setprecision(5) << pressure << " Pa" << endl;
		cout << endl;

        delay(1000);
    }
}
