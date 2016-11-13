#ifndef BUTTON_H
#define BUTTON_H

#include "WiringPi.h"

#include <functional>
#include <thread>

class Button
{
public:
	Button();
	~Button();

	void Setup(std::function<void(void)> handler);

private:
	void StartWorker();
	void ShutdownWorker();

	bool work_;
	bool buttonPressed_;
	std::function<void(void)> handler_;

	WiringPi wiringPi_;

	std::thread worker_;
};

#endif // !BUTTON_H

