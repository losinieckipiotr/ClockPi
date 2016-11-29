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
	void SetAlarmHandler(handlerT alarmHandler) { alarmHandler_ = alarmHandler; }
	void SetDisableHandler(handlerT disableHandler) { disableHandler_ = disableHandler; }

	void SetAlarm(int hours, int minutes);

	void DisableAlarm();
	void ClearAlarm();

	timeP GetAlarmTime() const;

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

