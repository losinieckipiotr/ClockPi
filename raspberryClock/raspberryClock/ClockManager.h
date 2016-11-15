#ifndef CLOCK_MANAGER_H
#define CLOCK_MANAGER_H

#include <chrono>

class Buzzer;

using timeP = std::chrono::system_clock::time_point;

class ClockManager
{
public:
	ClockManager(Buzzer& buzzer);
	~ClockManager();

	timeP GetAlarmTime();
	void SetAlarm(int hours, int minutes);//h:00-23 m:00-59
	void DisableAlarm();

	bool UpdateTime(const timeP& now);

private:
	bool isSet_;
	Buzzer& buzzer_;

	timeP alarmTime_;
};

#endif // !CLOCK_MANAGER_H

