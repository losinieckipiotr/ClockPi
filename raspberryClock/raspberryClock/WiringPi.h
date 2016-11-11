#ifndef WIRING_PI_WRAPPER_H
#define WIRING_PI_WRAPPER_H

#include <stdexcept>

class WiringPi
{
public:
	WiringPi() { }
	~WiringPi() { }

	static WiringPi GetInstance()
	{
		return WiringPi();
	}

	static bool WasSetup() { return wiringPiWasInit; }

	static int Setup();
	static void SetPinMode(int pin, int mode);

	static void DigitalWrite(int pin, int value);

	static int SPISetup(int channel, int speed);
	static int SPIDataRW(int channel, unsigned char* data, int len);

	static int I2CSetupAdr(const int devId);
	static int I2CRead(int fd);
	static int I2CReadReg8(int fd, int reg);
	static int I2CReadReg16(int fd, int reg);
	static int I2CWrite(int fd, int data);
	static int I2CWriteReg8(int fd, int reg, int data);
	static int I2CWriteReg16(int fd, int reg, int data);

	static void Delay(unsigned int del);

private:
	static bool wiringPiWasInit;
};

#endif // !WIRING_PI_H

