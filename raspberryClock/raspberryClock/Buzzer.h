#ifndef BUZZER_H
#define BUZZER_H

#include "WiringPi.h"
#include <thread>

class Buzzer
{
public:
	Buzzer();
	~Buzzer();

	void Setup();
	void On();
	void Off();

	void SetPeriod(int period) { period_ = period; }
	int GetPeriod() { return period_; }

private:
	void workerFunc() const;

	bool workerFlag;
	int period_;

	WiringPi wiringPi_;
	std::thread worker_;
public:
	enum 
	{
		DEFAULT_PERIOD = 500
	};
};

#endif // !BUZZER_H
