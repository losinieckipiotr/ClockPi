#include "Application.h"

#ifndef FAKE_WP
#include <wiringPi.h>
#include <wiringPiI2C.h>
#else
#include "fakeWiringPi.hpp"
#endif // FAKE_WP

#include <iostream>
#include <iomanip>
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
        cout << "Temperature: " << setprecision(5) << sensor_.ReadTemperature() << " C" << endl;
        cout << "Pressure: " << setprecision(5) << sensor_.ReadPressure() / 100.0f << " Pa" << endl;
		cout << "Sealevel pressure: " << setprecision(5)  << sensor_.ReadSealevelPressure() / 100.0f << " Pa" << endl;
		cout << endl;

        delay(1000);
    }
}
