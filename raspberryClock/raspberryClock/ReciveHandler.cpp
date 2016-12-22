#include "ReciveHandler.h"

#include <iostream>
#include <sstream>
#include <chrono>
#include <stdexcept>
#include <ctime>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using namespace std;
using namespace boost::property_tree;

void ReciveHandler::FrameHandler(
	const string& recivedMsg,
	shared_ptr<Session> session,
	resultColletionT resultsColletion,
	AlarmManager& alarmMan)
{
    cout.write(recivedMsg.data(), recivedMsg.length());
    cout.flush();

	try {
		Request req(recivedMsg);
		string respMsg("");

		if (req.name == "getLastResult")
		{
			respMsg = GetLastResult(resultsColletion);
		}
		else if (req.name == "getResultsHistory")
		{
			respMsg = GetResultsHistory(resultsColletion);
		}
		else if (req.name == "getAlarmTime")
		{
			respMsg = GetAlarmTime(alarmMan);
		}
		else if (req.name == "setClockAlarm")
		{
			int hour = stoi(req.params["hour"]);
			int minute = stoi(req.params["minute"]);
			SetClockAlarm(hour, minute, alarmMan);
		}
		else if (req.name == "disableClockAlarm")
		{
			DisableClockAlarm(alarmMan);
		}
		else
		{
			throw runtime_error("unknown request");
		}

		Response resp(req.id, 1, respMsg.length());
		string respJSON = resp.ToJSON();

		cout.write(respMsg.data(), respMsg.length());
        cout.flush();

		session->Send(move(respJSON), move(respMsg));
	}
	catch (exception& ex)
	{
		cout << ex.what() << endl;
	}
	session->Recive();
}

string ReciveHandler::GetLastResult(resultColletionT resultsColletion)
{
	using namespace chrono;
	auto& res = resultsColletion.back();

	ptree tree;
	ptree result;
	stringstream ss;

	milliseconds ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch());

	result.put("temperature", res.temperature);
	result.put("pressure", res.pressure);
	result.put("timeStamp", duration_cast<milliseconds>(res.timeStamp.time_since_epoch()).count());
	tree.put_child("result", result);
	write_json(ss, tree, false);

	return ss.str();
}

string ReciveHandler::GetResultsHistory(resultColletionT resultsColletion)
{
	stringstream ss;

	for (const auto& res : resultsColletion)
	{
		ptree tree;
		ptree node;
		node.put("temperature", res.temperature);
		node.put("pressure", res.pressure);
		node.put("timeStamp", res.timeStamp.time_since_epoch().count());
		tree.put_child("result", node);
		write_json(ss, tree, false);
	}

	return ss.str();
}

string ReciveHandler::GetAlarmTime(const AlarmManager& alarmMan)
{
	ptree tree;
	auto alarmPoint = alarmMan.GetAlarmTime();
	if (alarmPoint == chrono::system_clock::time_point())
	{
		tree.put("hour", -1);
		tree.put("minute", -1);
	}
	else
	{
		auto alarmTime = chrono::system_clock::to_time_t(alarmPoint);
		const auto timeStruct = localtime(&alarmTime);
		tree.put("hour", timeStruct->tm_hour);
		tree.put("minute", timeStruct->tm_min);
	}

	stringstream ss;
	write_json(ss, tree, false);

	return ss.str();
}

void ReciveHandler::SetClockAlarm(int hour, int minute, AlarmManager& alarmMan)
{
	alarmMan.SetAlarm(hour, minute);
}

void ReciveHandler::DisableClockAlarm(AlarmManager& alarmMan)
{
	alarmMan.DisableAlarm();
}
