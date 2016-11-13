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

static int digitalRead(int pin)
{
	return 1;//TO DO: random
}

static int wiringPiSPISetup(int channel, int speed)
{
	return 1;
}

static int wiringPiSPIDataRW(int channel, unsigned char *data, int len)
{
	return 1;
}


static int wiringPiI2CRead(int fd)
{
	return 1;
}
static int wiringPiI2CReadReg8(int fd, int reg)
{
	return 1;
}
static int wiringPiI2CReadReg16(int fd, int reg)
{
	return 1;
}

static int wiringPiI2CWrite(int fd, int data)
{
	return 1;
}
static int wiringPiI2CWriteReg8(int fd, int reg, int data)
{
	return 1;
}
static int wiringPiI2CWriteReg16(int fd, int reg, int data)
{
	return 1;
}

static int wiringPiI2CSetup(const int devId)
{
	return 1;
}

static void delay(unsigned int del)
{
	std::this_thread::sleep_for(
		std::chrono::milliseconds(del));
}

static void delayMicroseconds(unsigned int howLong)
{
	std::this_thread::sleep_for(
		std::chrono::microseconds(howLong));
}

static int pcf8574Setup(const int pinBase, const int i2cAddress)
{
	return 1;
}

static void pullUpDnControl(int pin, int pud)
{

}

#endif // !FAKE_WIRING_PI_H

