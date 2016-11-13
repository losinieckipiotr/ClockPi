#ifndef APPLICATION_H
#define APPLICATION_H

#include "WiringPi.h"

#include "Result.hpp"

#include "SSD1306.h"
#include "BMP180.h"
#include "Buzzer.h"
#include "Button.h"

#include <deque>
#include <thread>
#include <chrono>

using timeP = std::chrono::system_clock::time_point;

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
	void DisplayClock(const timeP& now);

	Result Measure(const timeP& now);
	void LoadResults();
	void SaveResults();

	WiringPi wiringPi_;

	OLED::SSD1306 screen_;
	BMP180 sensor_;
	Buzzer buzzer_;
	Button button_;

	DisplayMode mode_;

	std::deque<Result> resultsCollection_;
	std::thread measureTh_;
};

#endif // !APPLICATION_H

