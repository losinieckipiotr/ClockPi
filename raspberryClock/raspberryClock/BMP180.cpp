#include "BMP180.h"

#include <cmath>

constexpr auto OSS = BMP180_STANDARD;

BMP180::BMP180() : wiringPi_(WiringPi::GetInstance())
{
	
}

BMP180::~BMP180()
{

}

void BMP180::SetupI2C()
{
	fd = wiringPi_.I2CSetupAdr(BMP180_Address);
}

void BMP180::Calibrate()
{
	AC1 = I2C_readS16(BMP180_CAL_AC1);
	AC2 = I2C_readS16(BMP180_CAL_AC2);
	AC3 = I2C_readS16(BMP180_CAL_AC3);
	AC4 = I2C_readU16(BMP180_CAL_AC4);
	AC5 = I2C_readU16(BMP180_CAL_AC5);
	AC6 = I2C_readU16(BMP180_CAL_AC6);
	B1 = I2C_readS16(BMP180_CAL_B1);
	B2 = I2C_readS16(BMP180_CAL_B2);
	MB = I2C_readS16(BMP180_CAL_MB);
	MC = I2C_readS16(BMP180_CAL_MC);
	MD = I2C_readS16(BMP180_CAL_MD);
}

float BMP180::ReadTemperature()
{
	float T;
	int UT, X1, X2, B5;
	UT = read_raw_temp();
	X1 = ((UT - AC6)*AC5) >> 15;
	X2 = (MC << 11) / (X1 + MD);
	B5 = X1 + X2;
	T = ((B5 + 8) >> 4) / 10.0f;
	return T;
}

int BMP180::ReadPressure()
{
	int P;
	int UT, UP, X1, X2, X3, B3, B5, B6;
	unsigned int B4;
	int B7;
	UT = read_raw_temp();
	UP = read_raw_pressure();

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
	if (B7 < 0x80000000LL) { P = (B7 * 2) / B4; }
	else { P = (B7 / B4) * 2; }
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
	altitude = 44330.0f * (1.0f - pow(pressure / sealevel_pa, (1.0f / 5.255f)));
	return altitude;
}

float BMP180::ReadSealevelPressure()
{
	float altitude_m = 0.0;
	float pressure, p0;
	pressure = (float)ReadPressure();
	p0 = pressure / pow(1.0f - altitude_m / 44330.0f, 5.255f);
	return p0;
}


char BMP180::I2C_readByte(int reg)
{
	return (char)wiringPi_.I2CReadReg8(fd, reg);
}

unsigned short BMP180::I2C_readU16(int reg)
{
	int MSB, LSB;
	MSB = I2C_readByte(reg);
	LSB = I2C_readByte(reg + 1);
	int value = (MSB << 8) + LSB;
	return (unsigned short)value;
}

short BMP180::I2C_readS16(int reg)
{
	int result;
	result = I2C_readU16(reg);
	if (result > 32767)result -= 65536;
	return (short)result;
}

void BMP180::I2C_writeByte(int reg, int val)
{
	wiringPi_.I2CWriteReg8(fd, reg, val);
}

int BMP180::read_raw_temp()
{
	int raw;
	I2C_writeByte(BMP180_CONTROL, BMP180_READTEMPCMD);
	wiringPi_.Delay(5);  //5ms;
	raw = I2C_readByte(BMP180_TEMPDATA) << 8;
	raw += I2C_readByte(BMP180_TEMPDATA + 1);
	return raw;
}

int BMP180::read_raw_pressure()
{
	int MSB, LSB, XLSB, raw;
	I2C_writeByte(BMP180_CONTROL, BMP180_READPRESSURECMD + (OSS << 6));
	switch (OSS)
	{
	case BMP180_ULTRALOWPOWER:
		wiringPi_.Delay(5); break;
	case BMP180_HIGHRES:
		wiringPi_.Delay(14); break;
	case BMP180_ULTRAHIGHRES:
		wiringPi_.Delay(26); break;
	default:
		wiringPi_.Delay(8);
	}
	MSB = I2C_readByte(BMP180_PRESSUREDATA);
	LSB = I2C_readByte(BMP180_PRESSUREDATA + 1);
	XLSB = I2C_readByte(BMP180_PRESSUREDATA + 2);
	raw = ((MSB << 16) + (LSB << 8) + XLSB) >> (8 - OSS);
	return raw;
}





