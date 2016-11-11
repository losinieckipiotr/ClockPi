#ifndef BMP180_H
#define BMP180_H

class BMP180
{
public:
	BMP180();
	~BMP180();

	void Calibrate();

	float ReadTemperature();
	int ReadPressure();
	float ReadAltitude();

private:

};

#endif // !BMP180_H

