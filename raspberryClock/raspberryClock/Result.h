#ifndef RESULT_H
#define RESULT_H

#include <chrono>
#include <string>

struct Result
{
	float temperature;
	float pressure;
	std::chrono::system_clock::time_point timeStamp;
};

#endif // !RESULT_H
