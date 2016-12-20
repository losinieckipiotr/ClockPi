#include "AlarmManager.h"

#include "Buzzer.h"

#include <ctime>
#include <iostream>

using namespace std;
using namespace chrono;

AlarmManager::AlarmManager() : isSet_(false)
{

}

AlarmManager::~AlarmManager()
{
	StopWorker();
}

timeP AlarmManager::GetAlarmTime() const
{
	if (isSet_)
		return alarmTime_;
	else
		return system_clock::time_point();//epoch
}

void AlarmManager::SetAlarm(int hours, int minutes)
{
	DisableAlarm();

	const auto now = system_clock::now();
	const auto timeT = system_clock::to_time_t(now);
	auto localTimeStruct = localtime(&timeT);
	localTimeStruct->tm_hour = hours;
	localTimeStruct->tm_min = minutes;
	localTimeStruct->tm_sec = 0;
	auto alarmTime = mktime(localTimeStruct);
	auto alarmTimePoint = system_clock::from_time_t(alarmTime);
	if (now > alarmTimePoint)
		alarmTimePoint += chrono::hours(24);//add 1 day
	alarmTime_ = alarmTimePoint;
	isSet_ = true;

	StartWorker();

	auto t = system_clock::to_time_t(alarmTime_);
	cout << "Alarm was set to: " << ctime(&t);
}

void AlarmManager::DisableAlarm()
{
	if(isSet_)
    {
        ClearAlarm();
		if (disableHandler_) disableHandler_();
    }
    else
        ClearAlarm();

	cout << "Alarm disabled" << endl;
}

void AlarmManager::ClearAlarm()
{
	isSet_ = false;
	StopWorker();
}

void AlarmManager::StartWorker()
{
	StopWorker();
	workerFlag_ = true;
	worker_ = thread([this]() { WorkerFunc(); });
}

void AlarmManager::StopWorker()
{
	workerFlag_ = false;
	if (worker_.joinable())
		worker_.detach();
}

void AlarmManager::WorkerFunc()
{
	const timeP tp;//epoch
	while (workerFlag_)
	{
		const auto now = system_clock::now();
		if (alarmTime_ <= now)
		{
			if (alarmHandler_) alarmHandler_();
			workerFlag_ = false;

			cout << "Alarm !!!" << endl;
		}
		const auto sec = duration_cast<seconds>(now - tp);//seconds from epoch
		const auto nexTime = tp + sec + seconds(1);//now plus 1 second
		this_thread::sleep_until(nexTime);
	}
}
