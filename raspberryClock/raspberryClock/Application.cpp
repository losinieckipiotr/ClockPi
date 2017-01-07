#include "Application.h"

#include "Session.h"

#include <iostream>
#include <iomanip>
#include <exception>
#include <memory>
#include <ctime>
#include <fstream>
#include <utility>
#include <sstream>
#include <streambuf>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using namespace std;
using namespace chrono;
namespace pt = boost::property_tree;

constexpr auto FILE_NAME_JSON = "measurement.json";
constexpr auto FILE_NAME_CSV = "measurement.csv";

Application::Application() : alarmRinging_(false), mode_(DisplayMode::COLCK)
{

}

Application::~Application()
{

}

void Application::Start()
{
	LoadResults();

	wiringPi_.Setup();

	screen_.Begin();

	sensor_.SetupI2C();
	sensor_.Calibrate();

	buzzer_.Setup();

	auto alarmHandler = [this]()
	{
        lock_guard<mutex> lck(appMutex_);
        alarmRinging_ = true;
        mode_ = DisplayMode::COLCK;
        DisplayClock(system_clock::now());
        buzzer_.On();
    };
	auto disableAlarmHandler = [this]()
	{
		buzzer_.Off();
		alarmRinging_ = false;
	};
	alarmMan_.SetAlarmHandler(alarmHandler);
	alarmMan_.SetDisableHandler(disableAlarmHandler);

	auto clickHandler = [this]()
	{
		if(!alarmRinging_)
			NextDisplayMode();
	};
	auto holdHandler = [this]() {
		alarmMan_.DisableAlarm();
	};
	button_.Setup(clickHandler, holdHandler);

	auto reciveFrameHandler =
		[this](string recivedMsg, shared_ptr<Session> session)->void
	{
		try
		{
			lock_guard<mutex> lck(appMutex_);

			reciveHandler_.FrameHandler(
				recivedMsg,
				session,
				resultsCollection_,
				alarmMan_);
		}
		catch (const std::exception& ex)
		{
			cout << ex.what() << endl;
		}
	};
	server_.Start(reciveFrameHandler);

	appFlag_ = true;//application exit flag
	measureTh_ = thread([this]() { DisplayLoop(); });//main loop

	cout << "Raspberry Clock" << endl;
	cin.get();//locks main thread

	server_.Stop();
	appFlag_ = false;
	measureTh_.join();

	SaveResults();
}

void Application::NextDisplayMode()
{
    lock_guard<mutex> lck(appMutex_);

	switch (mode_)
	{
	case DisplayMode::MEASURE:
		mode_ = DisplayMode::COLCK;
		DisplayClock(system_clock::now());
		break;
	case DisplayMode::COLCK:
		mode_ = DisplayMode::MEASURE;
		DisplayMeasure(resultsCollection_.back());
		break;
	default:
		break;
	}
}

void Application::SwitchBuzzer()
{
	if (buzzer_.IsOn())
		buzzer_.Off();
	else
		buzzer_.On();
}

void Application::DisplayLoop()
{
	while (appFlag_)
	{
		const auto now = system_clock::now();
		MeasureAndDisplay(now);
		LoopDelay(now);
	}
}

void Application::MeasureAndDisplay(const timeP& now)
{
	lock_guard<mutex> lck(appMutex_);
	Result res = Measure(now);

	switch (mode_)
	{
	case DisplayMode::MEASURE:
		DisplayMeasure(res);
		break;
	case DisplayMode::COLCK:
		DisplayClock(now);
		break;
	default:
		break;
	}
}

Result Application::Measure(const timeP& now)
{
	Result res;
	res.temperature = sensor_.ReadTemperature();
	res.pressure = sensor_.ReadPressure();
	res.timeStamp = now;
	resultsCollection_.push_back(res);
	return res;
}

void Application::DisplayMeasure(const Result& res)
{
	const OLED::Font16x8 font;
	const unsigned int h1 = 0;
	const unsigned int h2 = 32;

	auto strTemp = to_string(res.temperature);
	auto strPre = to_string(res.pressure);

	auto pos = strTemp.find('.');
	auto size = strTemp.size();
	if ((pos + 2) < size)
		strTemp.erase(pos + 2);
	strTemp = "T: " + strTemp + " C";

	pos = strPre.find('.');
	strPre.erase(pos);
	strPre = "P: " + strPre + " Pa";

	unsigned int x1 = (OLED::SCREEN_WIDTH - strTemp.length()*font.charWidth) / 2;
	unsigned int x2 = (OLED::SCREEN_WIDTH - strPre.length()*font.charWidth) / 2;

	screen_.Write(x1, h1, strTemp, font);
	screen_.Write(x2, h2, strPre, font);
	screen_.Display();

	//cout << strTemp << endl;
	//cout << strPre << endl;
	//cout << endl;
}

void Application::DisplayClock(const timeP& now)
{
	const OLED::Font16x8 font1;
	const OLED::Font32x16 font2;

	const auto timeT = system_clock::to_time_t(now);
	const auto timeNow = localtime(&timeT);

	const auto dateStr = to_string(timeNow->tm_mday) +
		'.' +
		to_string(timeNow->tm_mon + 1) +
		'.' +
		to_string(timeNow->tm_year + 2000 - 100);

	const auto timeStr = to_string(timeNow->tm_hour / 10) +
		to_string(timeNow->tm_hour % 10) +
		':' +
		to_string(timeNow->tm_min / 10) +
		to_string(timeNow->tm_min % 10) +
		':' +
		to_string(timeNow->tm_sec / 10) +
		to_string(timeNow->tm_sec % 10);

	unsigned int centerPos = (OLED::SCREEN_WIDTH - dateStr.length()*font1.charWidth) / 2;
	screen_.Write(centerPos, 0, dateStr, font1);
	screen_.Write(0, 16, timeStr, font2);

	//cout << dateStr << endl;
	//cout << timeStr << endl;
	//cout << endl;

	screen_.Display();
}

void Application::LoopDelay(const timeP& now)
{
	const timeP tp;//epoch
	const auto sec = duration_cast<seconds>(now - tp);//seconds from epoch
	const auto nexTime = tp + sec + seconds(1);//now plus 1 second
	this_thread::sleep_until(nexTime);
}

void Application::LoadResults()
{
	try
	{
		ifstream file(FILE_NAME_CSV, ios::in | ios::binary);
		string buf;
		file.seekg(0, ios::end);
		buf.resize(static_cast<size_t>(file.tellg()));
		file.seekg(0, ios::beg);
		file.read(&buf[0], buf.size());
		file.close();

		stringstream ss(move(buf));
		buf.clear();
		while (ss)
		{
			Result res;
			getline(ss, buf, ',');
			res.temperature = stof(buf);
			buf.clear();
			getline(ss, buf, ',');
			res.pressure = stof(buf);
			buf.clear();
			getline(ss, buf, '\n');
			system_clock::time_point t;//epoch
			system_clock::duration sysClockTicks(stoll(buf));
			res.timeStamp = t + sysClockTicks;
			resultsCollection_.push_back(move(res));
		}

		/*pt::ptree tree;
		pt::read_json(FILE_NAME_JSON, tree);
		auto& resultsNode = tree.get_child("results");
		for (pt::ptree::value_type& val : resultsNode)
		{
			Result res;
			res.temperature = val.second.get<float>("temperature");
			res.pressure = val.second.get<float>("pressure");
			system_clock::time_point t;//epoch
			system_clock::duration sysClockTicks(val.second.get<long long>("timeStamp"));
			res.timeStamp = t + sysClockTicks;
			resultsCollection_.push_back(move(res));
		}*/
	}
	catch (const std::exception&) { } //ignore
}

void Application::SaveResults()
{
	/*pt::ptree tree;
	pt::ptree results;
	for (const auto& res : resultsCollection_)
	{
		pt::ptree node;
		node.put("temperature", res.temperature);
		node.put("pressure", res.pressure);
		node.put("timeStamp", res.timeStamp.time_since_epoch().count());
		results.push_back(make_pair("", node));
	}
	tree.add_child("results", results);
	pt::write_json(FILE_NAME_JSON, tree, locale(), false);*/

	string buf;
	for (const auto& res : resultsCollection_)
	{
		buf += to_string(res.temperature) + ',';
		buf += to_string(res.pressure) + ',';
		buf += to_string(res.timeStamp.time_since_epoch().count()) + '\n';
	}
	ofstream file(FILE_NAME_CSV, ios::trunc | ios::binary);
	file.write(buf.data(), buf.size());
	file.close();
}
