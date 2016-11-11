#include "Application.h"

#ifndef FAKE_WP
#include <wiringPi.h>
#include <wiringPiI2C.h>
#else
#include "fakeWiringPi.hpp"
#endif // FAKE_WP

#include <iostream>
#include <exception>
#include <stdexcept>

using namespace std;

Application::Application()
{

}

Application::~Application()
{

}

void Application::Start()
{
    if(wiringPiSetup() < 0)
        throw runtime_error("wiringPiSetup error");

	sensor_.Calibrate();

    while(1)
    {
        cout << "Temperature: " << sensor_.ReadTemperature() << " C" << endl;
        cout << "Pressure: " << sensor_.ReadPressure() / 100.0f << " Pa" << endl;
        cout << "Altitude: " << sensor_.ReadAltitude() << " m" << endl;

        delay(1000);
    }
}
