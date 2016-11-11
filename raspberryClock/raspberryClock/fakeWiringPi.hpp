#ifndef FAKE_WIRING_PI_H
#define FAKE_WIRING_PI_H

#include <thread>
#include <chrono>

#define	LOW			0
#define	HIGH		1

#define	INPUT		0
#define	OUTPUT		1

static int wiringPiSetup()
{
	return 1;
}

static void pinMode(int pin, int mode)
{

}

static void digitalWrite(int pin, int value)
{

}

static int wiringPiSPISetup(int channel, int speed)
{
	return 1;
}

static int wiringPiSPIDataRW(int channel, unsigned char *data, int len)
{
	return 1;
}

static void delay(unsigned int del)
{
	std::this_thread::sleep_for(
		std::chrono::milliseconds(del));
}
#endif // !FAKE_WIRING_PI_H

