#ifndef BMP180_H
#define BMP180_H

#include "WiringPi.h"

#include <cstdint>

//i2c address
constexpr auto BMP180_Address =         0x77;

//Operating Modes
constexpr auto BMP180_ULTRALOWPOWER =   0;
constexpr auto BMP180_STANDARD =        1;
constexpr auto BMP180_HIGHRES =         2;
constexpr auto BMP180_ULTRAHIGHRES =    3;

//BMP185 Registers
constexpr auto BMP180_CAL_AC1 =         0xAA;  //Calibration data (16 bits)
constexpr auto BMP180_CAL_AC2 =         0xAC;  //Calibration data (16 bits)
constexpr auto BMP180_CAL_AC3 =         0xAE;  //Calibration data (16 bits)
constexpr auto BMP180_CAL_AC4 =         0xB0;  //Calibration data (16 bits)
constexpr auto BMP180_CAL_AC5 =         0xB2;  //Calibration data (16 bits)
constexpr auto BMP180_CAL_AC6 =         0xB4;  //Calibration data (16 bits)
constexpr auto BMP180_CAL_B1 =          0xB6;  //Calibration data (16 bits)
constexpr auto BMP180_CAL_B2 =          0xB8;  //Calibration data (16 bits)
constexpr auto BMP180_CAL_MB =          0xBA;  //Calibration data (16 bits)
constexpr auto BMP180_CAL_MC =          0xBC;  //Calibration data (16 bits)
constexpr auto BMP180_CAL_MD =          0xBE;  //Calibration data (16 bits)
constexpr auto BMP180_CONTROL =         0xF4;
constexpr auto BMP180_TEMPDATA =        0xF6;
constexpr auto BMP180_PRESSUREDATA =    0xF6;

//Commands
constexpr auto BMP180_READTEMPCMD =     0x2E;
constexpr auto BMP180_READPRESSURECMD = 0x34;

class BMP180
{
public:
	BMP180();
	~BMP180();

	void SetupI2C();
	void Calibrate();

	float ReadTemperature();
	float ReadPressure();
	float ReadAltitude();
	float ReadSealevelPressure();

private:
	char I2C_readByte(int reg);
	unsigned short I2C_readU16(int reg);
	short I2C_readS16(int reg);
	void I2C_writeByte(int reg, int val);
	int read_raw_temp();
	int read_raw_pressure();

	short AC1, AC2, AC3, B1, B2, MB, MC, MD;
	unsigned short AC4, AC5, AC6;
    int fd;

	WiringPi wiringPi_;
};

#endif // !BMP180_H

