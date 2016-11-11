#include "BMP180.h"

#ifndef FAKE_WP
#include <wiringPi.h>
#include <wiringPiI2C.h>
#else
#include "fakeWiringPi.hpp"
#endif // FAKE_WP

#include <cmath>

constexpr auto OSS = BMP180_STANDARD;

BMP180::BMP180()
{
    fd = wiringPiI2CSetup(BMP180_Address);
}

BMP180::~BMP180()
{

}

void BMP180::Calibrate()
{
    AC1 = ReadI2CS16(BMP180_CAL_AC1);
	AC2 = ReadI2CS16(BMP180_CAL_AC2);
	AC3 = ReadI2CS16(BMP180_CAL_AC3);
	AC4 = ReadI2CS16(BMP180_CAL_AC4);
	AC5 = ReadI2CS16(BMP180_CAL_AC5);
	AC6 = ReadI2CS16(BMP180_CAL_AC6);
	B1  = ReadI2CS16(BMP180_CAL_B1);
	B2  = ReadI2CS16(BMP180_CAL_B2);
	MB  = ReadI2CS16(BMP180_CAL_MB);
	MC  = ReadI2CS16(BMP180_CAL_MC);
    MD  = ReadI2CS16(BMP180_CAL_MD);
}

int8_t BMP180::ReadI2CByte(int regAdr)
{
    return static_cast<int8_t>(wiringPiI2CReadReg8(fd, regAdr));
}

uint16_t BMP180::ReadI2CU16(int regAdr)
{
    int MSB, LSB;
	MSB = ReadI2CByte(regAdr);
	LSB = ReadI2CByte(regAdr + 1);
	int value = (MSB << 8) + LSB;
	return static_cast<uint16_t>(value);
}

int16_t BMP180::ReadI2CS16(int regAdr)
{
	int result;
	result = ReadI2CU16(regAdr);
	if (result > 32767)result -= 65536;
	return (int16_t)result;
}

void BMP180::WriteI2CByte(int reg, int val)
{
	wiringPiI2CWriteReg8(fd,reg,val);
}

int BMP180::ReadRawTemp()
{
	int raw;
	WriteI2CByte(BMP180_CONTROL, BMP180_READTEMPCMD);
	delay(5);  //5ms;
	raw = ReadI2CByte(BMP180_TEMPDATA) << 8;
 	raw += ReadI2CByte(BMP180_TEMPDATA + 1);
	return raw;
}

int BMP180::ReadRawPressure()
{
	int MSB, LSB, XLSB, raw;
	WriteI2CByte(BMP180_CONTROL, BMP180_READPRESSURECMD +(OSS << 6));
	switch(OSS)
	{
		case BMP180_ULTRALOWPOWER:
			delay(5);break;
		case BMP180_HIGHRES:
			delay(14);break;
		case BMP180_ULTRAHIGHRES:
			delay(26);break;
		default:
			delay(8);
	}
	MSB  = ReadI2CByte(BMP180_PRESSUREDATA);
	LSB  = ReadI2CByte(BMP180_PRESSUREDATA + 1);
	XLSB = ReadI2CByte(BMP180_PRESSUREDATA + 2);
	raw = ((MSB << 16) + (LSB << 8) + XLSB) >> (8 - OSS);
	return raw;
}

float BMP180::ReadTemperature()
{
	float T;
	int UT, X1, X2, B5;
	UT = ReadRawTemp();
	X1 = ((UT - AC6)*AC5) >> 15;
	X2 = (MC << 11) / (X1 + MD);
  	B5 = X1 + X2;
  	T = ((B5 + 8) >> 4) /10.0;
	return T;
}

int BMP180::ReadPressure()
{
	int P;
	int UT, UP, X1, X2, X3, B3, B5, B6;
	unsigned int B4;
	int B7;
	UT = ReadRawTemp();
	UP = ReadRawPressure();

	X1 = ((UT - AC6)*AC5) >> 15;
 	X2 = (MC << 11) / (X1 + MD);
	B5 = X1 + X2;

	//Pressure Calculations
	B6 = B5 - 4000;
	X1 = (B2 * (B6 * B6) >> 12) >> 11;
	X2 = (AC2 * B6) >> 11;
	X3 = X1 + X2;
	B3 = (((AC1 * 4 + X3) << OSS) + 2) / 4;
	X1 = (AC3 * B6) >> 13;
	X2 = (B1 * ((B6 * B6) >> 12)) >> 16;
	X3 = ((X1 + X2) + 2) >> 2;
	B4 = (AC4 * (X3 + 32768)) >> 15;
	B7 = (UP - B3) * (50000 >> OSS);
	if (B7 < 0x80000000){P = (B7 * 2) / B4;}
	else {P = (B7 / B4) * 2;}
    X1 = (P >> 8) * (P >> 8);
    X1 = (X1 * 3038) >> 16;
    X2 = (-7357 * P) >> 16;
    P = P + ((X1 + X2 + 3791) >> 4);
    return P;

}
float BMP180::ReadAltitude()
{
	float pressure, altitude;
	float sealevel_pa = 101325.0;
	pressure = (float)ReadPressure();
	altitude = 44330.0 * (1.0 - pow(pressure / sealevel_pa,(1.0/5.255)));
	return altitude;
}

float BMP180::ReadSealevelPressure()
{
	float altitude_m = 0.0;
	float pressure, p0;
	pressure =(float)ReadPressure();
	p0 = pressure / pow(1.0 - altitude_m/44330.0,5.255);
	return p0;
}
