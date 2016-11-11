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
	const auto h1 = 0;
	const auto h2 = 32;

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
        if ((pos + 2) < size)
            strTemp.erase(pos + 2);
        strTemp = "T: " + strTemp + " C";

        pos = strPre.find('.');
        strPre.erase(pos);
        strPre = "P: " + strPre + " Pa";

        auto x1 = (OLED::SCREEN_WIDTH - strTemp.length()*font.charWidth) / 2;
        auto x2 = (OLED::SCREEN_WIDTH - strPre.length()*font.charWidth) / 2;

		screen_.Write(x1, h1, strTemp, font);
		screen_.Write(x2, h2, strPre, font);
		screen_.Display();

        cout << strTemp << endl;
        cout << strPre << endl;
		cout << endl;

        delay(1000);
    }
}

void Application::Measure()
{

}
