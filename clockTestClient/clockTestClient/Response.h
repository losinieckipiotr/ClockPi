#ifndef RESPONSE_H
#define RESPONSE_H
#include <string>

class Response
{
public:
	Response(const std::string& json);
	Response(int id, int state, int length)
		: id(id), state(state), length(length) { }

	std::string ToJSON();

	int id;
	int state;
	int length;
};

#endif // !RESPONSE_H
