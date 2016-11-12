#include "Application.h"

#include <iostream>
#include <iomanip>
#include <string>
#include <thread>
#include <exception>
#include <memory>
#include <ctime>
#include <fstream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using namespace std;
namespace pt = boost::property_tree;

constexpr auto FILE_NAME_JSON = "measurement.json";
//constexpr auto FILE_NAME_CSV = "measurement.csv";

Application::Application()
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

	bool flag = true;
	thread th([this, &flag]()
	{
		const OLED::Font16x8 font;
		const unsigned int h1 = 0;
		const unsigned int h2 = 32;
		
		while (flag)
		{
			Result res = Measure();

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

			cout << strTemp << endl;
			cout << strPre << endl;
			cout << endl;

			wiringPi_.Delay(1000);
		}
	});
	cin.get();
	flag = false;
	th.join();

	SaveResults();
}

Result Application::Measure()
{
	Result res;
	res.temperature = sensor_.ReadTemperature();
	res.pressure = sensor_.ReadPressure();
	res.timeStamp = chrono::system_clock::now();
	resultsCollection_.push_back(res);
	return res;
}

void Application::LoadResults()
{
	try
	{
		pt::ptree tree;
		pt::read_json(FILE_NAME_JSON, tree);
		auto& resultsNode = tree.get_child("measurement");
		for (pt::ptree::value_type& val : resultsNode)
		{
			if (val.first == "result")//node name
			{
				Result res;
				res.temperature = val.second.get<float>("temperature");
				res.pressure = val.second.get<float>("pressure");
				chrono::system_clock::time_point t;//epoch
				chrono::system_clock::duration sysClockTicks(val.second.get<long long>("timeStamp"));
				res.timeStamp = t + sysClockTicks;
				resultsCollection_.push_back(move(res));
			}
		}
	}
	catch (const std::exception&) { } //ignore
}

void Application::SaveResults()
{
	pt::ptree tree;
	tree.put("measurement", "");

	for (const auto& res : resultsCollection_)
	{
		auto& node = tree.add("measurement.result", "");
		node.put("temperature", res.temperature);
		node.put("pressure", res.pressure);
		node.put("timeStamp", res.timeStamp.time_since_epoch().count());
	}
	pt::write_json(FILE_NAME_JSON, tree);

	/*ofstream file(FILE_NAME_CSV, ios::trunc);
	for (const auto& res : resultsCollection_)
	{
		file << res.temperature << ',';
		file << res.pressure << ',';
		file << res.timeStamp.time_since_epoch().count() << endl;
	}*/
}
