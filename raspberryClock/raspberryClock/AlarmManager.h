#ifndef CLOCK_MANAGER_H
#define CLOCK_MANAGER_H

#include <chrono>
#include <functional>
#include <thread>

using timeP = std::chrono::system_clock::time_point;
using handlerT = std::function <void(void)>;

class AlarmManager
{
public:
	AlarmManager();
	~AlarmManager();

	bool IsAlarmSet() { return isSet_; }
	timeP GetAlarmTime();
	void SetAlarm(
		int hours, int minutes,//h:00-23 m:00-59
		handlerT alarmHandler, handlerT disableHandler);
	void DisableAlarm();

	void ClearAlarm();

private:
	void StartWorker();
	void StopWorker();
	void WorkerFunc();

	bool isSet_;
	bool workerFlag_;

	timeP alarmTime_;
	handlerT alarmHandler_;
	handlerT disableHandler_;

	std::thread worker_;
};

#endif // !CLOCK_MANAGER_H

