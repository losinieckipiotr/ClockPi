#include "ClockManager.h"

#include "Buzzer.h"

#include <ctime>

using namespace std;
using namespace chrono;

ClockManager::ClockManager(Buzzer& buzzer) : isSet_(false), buzzer_(buzzer)
{

}

ClockManager::~ClockManager()
{
	buzzer_.Off();
}

timeP ClockManager::GetAlarmTime()
{
	if (isSet_)
		return alarmTime_;
	else
		return system_clock::time_point();//epoch
}

void ClockManager::SetAlarm(int hours, int minutes)
{
	const auto now = system_clock::now();
	const auto timeT = system_clock::to_time_t(now);
	auto localTimeStruct = localtime(&timeT);
	localTimeStruct->tm_hour = hours;
	localTimeStruct->tm_min = minutes;
	localTimeStruct->tm_sec = 0;
	auto alarmTime = mktime(localTimeStruct);
	auto alarmTimePoint = system_clock::from_time_t(alarmTime);
	if (now < alarmTimePoint)
		alarmTimePoint += chrono::hours(24);//add 1 day
	alarmTime_ = alarmTimePoint;
}

void ClockManager::DisableAlarm()
{
	isSet_ = false;
	buzzer_.Off();
}

bool ClockManager::UpdateTime(timeP now)
{
	if (isSet_)
	{
		if (alarmTime_ <= now)
		{
			buzzer_.On();
			return true;
		}
	}
	return false;
}
