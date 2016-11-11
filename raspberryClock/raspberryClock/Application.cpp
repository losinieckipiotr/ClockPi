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

Application::Application()
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

		auto strTemp = to_string(temp);
		auto strPre = to_string(pressure);

		auto pos = strTemp.find('.');
        auto size = strTemp.size();
        if ((pos + 3) < size)
            strTemp.erase(pos + 3);

        pos = strPre.find('.');
        size = strPre.size();
        if ((pos + 3) < size)
            strPre.erase(pos + 3);

		screen_.Write(0, h1, "T: " + strTemp + " C", font);
		screen_.Write(0, h2, "P: " + strPre + " Pa", font);
		screen_.Display();

        cout << "Temperature: " << strTemp << " C" << endl;
        cout << "Pressure: " << strPre << " Pa" << endl;
		cout << endl;

        delay(1000);
    }
}
