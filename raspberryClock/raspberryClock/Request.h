#ifndef REQUEST_H
#define REQUEST_H
#include <string>
#include <map>
#include <utility>

class Request
{
public:
	Request() = default;
	Request(const std::string& json);

	std::string ToJSON();

	int id;
	std::string name;
	std::map<std::string, std::string> params;
};

#endif // !REQUEST_H