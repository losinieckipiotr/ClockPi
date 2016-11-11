#ifndef APPLICATION_H
#define APPLICATION_H

#include "BMP180.h"

class Application
{
public:
	Application();
	~Application();

	void Start();

private:
	BMP180 sensor_;
};

#endif // !APPLICATION_H

