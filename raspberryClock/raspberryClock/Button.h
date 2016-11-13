#ifndef BUTTON_H
#define BUTTON_H

#include "WiringPi.h"

#include <functional>
#include <thread>

using handlerType = std::function<void(void)>;

class Button
{
public:
	Button();
	~Button();

	void Setup(handlerType clickHandler, handlerType holdHandler);

private:
	void StartWorker();
	void ShutdownWorker();

	bool work_;
	bool buttonPressed_;
	int holdCtr_;

	handlerType clickHandler_;
	handlerType holdHandler_;

	WiringPi wiringPi_;

	std::thread worker_;
public:
	enum
	{
		INTERVAL = 100,
		HOLD_TIME = 1000,
		HOLD_TICKS = HOLD_TIME / INTERVAL
	};
};

#endif // !BUTTON_H

