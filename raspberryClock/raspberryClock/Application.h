#ifndef APPLICATION_H
#define APPLICATION_H

#include "WiringPi.h"

#include "Result.hpp"

#include "SSD1306.h"
#include "BMP180.h"
#include "Buzzer.h"

#include <deque>
#include <thread>

class Application
{
public:
	Application();
	~Application();

	void Start();

private:

	enum class  DisplayMode
	{
		MEASURE,
		COLCK
	};

	void DisplayMeasure(const Result& res);
	void DisplayClock();

	Result Measure();
	void LoadResults();
	void SaveResults();

	WiringPi wiringPi_;

	OLED::SSD1306 screen_;
	BMP180 sensor_;
	Buzzer buzzer_;

	DisplayMode mode_;

	std::deque<Result> resultsCollection_;
	std::thread measureTh_;
};

#endif // !APPLICATION_H

