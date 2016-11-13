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
#include <mutex>
#include <atomic>
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

	void NextDisplayMode();
	void SwitchBuzzer();

	void DisplayLoop();
	void MeasureAndDisplay(const timeP& now);
	Result Measure(const timeP& now);
	void DisplayMeasure(const Result& res);
	void DisplayClock(const timeP& now);
	void LoopDelay(const timeP& now);

	void LoadResults();
	void SaveResults();

	WiringPi wiringPi_;

	OLED::SSD1306 screen_;
	BMP180 sensor_;
	Buzzer buzzer_;
	Button button_;

	DisplayMode mode_;

	std::deque<Result> resultsCollection_;

	std::atomic<bool> appFlag_;
	std::thread measureTh_;
	std::mutex appMutex_;
};

#endif // !APPLICATION_H
