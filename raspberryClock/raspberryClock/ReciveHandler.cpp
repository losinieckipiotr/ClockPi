#include "ReciveHandler.h"

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
	//note: if error session should be destoyed

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
		throw runtime_error("unknow request");
	}

	Response resp(req.id, 1, respMsg.length());
	string respJSON = resp.ToJSON();

	session->Send(move(respJSON), move(respMsg));
	session->Recive();
}

string ReciveHandler::GetLastResult(resultColletionT resultsColletion)
{
	auto& res = resultsColletion.back();

	ptree tree;
	ptree result;

	result.put("temperature", res.temperature);
	result.put("pressure", res.pressure);
	result.put("timeStamp", res.timeStamp.time_since_epoch().count());

	tree.put_child("result", result);
	stringstream ss;
	write_json(ss, tree, false);

	return ss.str();
}

string ReciveHandler::GetResultsHistory(resultColletionT resultsColletion)
{
	ptree tree;
	ptree results;

	for (const auto& res : resultsColletion)
	{
		ptree node;
		node.put("temperature", res.temperature);
		node.put("pressure", res.pressure);
		node.put("timeStamp", res.timeStamp.time_since_epoch().count());
		results.push_back(make_pair("", node));
	}
	tree.add_child("results", results);

	stringstream ss;
	write_json(ss, tree, false);

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
	if (alarmMan.IsAlarmSet()) alarmMan.DisableAlarm();
}