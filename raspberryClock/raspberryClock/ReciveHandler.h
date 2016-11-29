#ifndef RECIVE_HANDLER_H

#include "Session.h"
#include "Result.h"
#include "AlarmManager.h"
#include "Request.h"
#include "Response.h"

#include <deque>
#include <string>
#include <memory>

class ReciveHandler
{
	using resultColletionT = const std::deque<Result>&;

public:
	static void FrameHandler(
		const std::string& recivedMsg,
		std::shared_ptr<Session> session,
		resultColletionT resultsColletion,
		AlarmManager& alarmMan);

private:
	static std::string GetLastResult(resultColletionT resultsColletion);
	static std::string GetResultsHistory(resultColletionT resultsColletion);
	static std::string GetAlarmTime(const AlarmManager& alarmMan);
	static void SetClockAlarm(int hour, int minute, AlarmManager& alarmMan);
	static void DisableClockAlarm(AlarmManager& alarmMan);
};

#endif // !RECIVE_HANDLER_H
