#include "SSD1306.h"

#include <regex>
#include <cmath>

constexpr auto RST_PIN = 24;
constexpr auto DC_PIN = 27;

constexpr auto SPI_CHANNEL = 0;
constexpr auto SPI_SPEED = 2000000;//2M

constexpr auto	LOW = 0;
constexpr auto	HIGH = 1;
constexpr auto	INPUT = 0;
constexpr auto	OUTPUT = 1;

using namespace OLED;

using cmd = SSD1306_CMDS;

Bitmap::Bitmap(const unsigned int width,
	           const unsigned int height,
	           const unsigned char* data,
	           bool flip)
	:	width_(width),
        height_(height),
		rows_(height),
		columns_((width % 8 == 0) ? (width / 8) : (width / 8 + 1)),
		bitmapSize_(rows_ * columns_),
		dataBitmap_(height),
		data_(data, data + bitmapSize_)
{
	auto ptr = data;
	for (auto& row : dataBitmap_)
	{
		row.reserve(columns_);
		for (unsigned j = 0; j < columns_; ++j)
		{
			row.push_back(*ptr);
			++ptr;
		}
	}

	if (flip)
	{
		Flip();
	}
}

Bitmap& Bitmap::Flip()
{
	for (auto& row : dataBitmap_)
	{
		for (auto& val : row)
		{
			val = ~val;
		}
	}
	PrepareData_();
	return *this;
}

void Bitmap::PrepareData_()
{
	auto it = data_.begin();
	for (const auto& row : dataBitmap_)
	{
		for (const auto& val : row)
		{
			*it = val;
			++it;
		}
	}
}

SSD1306::SSD1306() : wiringPi_(WiringPi::GetInstance())
{
	ClearBuffer();
}

SSD1306::~SSD1306()
{
	ClearBuffer();
	Display();
}

void SSD1306::Begin()
{
	wiringPi_.SetPinMode(RST_PIN, OUTPUT);
	wiringPi_.SetPinMode(DC_PIN, OUTPUT);
	wiringPi_.SPISetup(SPI_CHANNEL, SPI_SPEED);

	wiringPi_.DigitalWrite(RST_PIN, HIGH);
	wiringPi_.Delay(10);
	wiringPi_.DigitalWrite(RST_PIN, LOW);
	wiringPi_.Delay(10);
	wiringPi_.DigitalWrite(RST_PIN, HIGH);

	SendCmd(cmd::SSD1306_DISPLAYOFF);
	SendCmd(cmd::SSD1306_SETDISPLAYCLOCKDIV);
	SendCmd(0x80);//the suggested ratio 0x80

	SendCmd(cmd::SSD1306_SETMULTIPLEX);
	SendCmd(0x3F);
	SendCmd(cmd::SSD1306_SETDISPLAYOFFSET);
	SendCmd(0x0);// no offset
	SendCmd(uchar_cast(cmd::SSD1306_SETSTARTLINE) | 0x0);// line #0
	SendCmd(cmd::SSD1306_CHARGEPUMP);
	SendCmd((VCC_STATE == cmd::SSD1306_EXTERNALVCC) ? 0x10 : 0x14);

	SendCmd(cmd::SSD1306_MEMORYMODE);
	SendCmd(0x00);// 0x0 act like ks0108

	SendCmd(uchar_cast(cmd::SSD1306_SEGREMAP) | 0x1);
	SendCmd(cmd::SSD1306_COMSCANDEC);
	SendCmd(cmd::SSD1306_SETCOMPINS);
	SendCmd(0x12);// TODO - calculate based on _rawHieght ?
	SendCmd(cmd::SSD1306_SETCONTRAST);
	SendCmd((VCC_STATE == cmd::SSD1306_EXTERNALVCC) ? 0x9F : 0xCF);
	SendCmd(cmd::SSD1306_SETPRECHARGE);
	SendCmd((VCC_STATE == cmd::SSD1306_EXTERNALVCC) ? 0x22 : 0xF1);
	SendCmd(cmd::SSD1306_SETVCOMDETECT);
	SendCmd(0x40);
	SendCmd(cmd::SSD1306_DISPLAYALLON_RESUME);
	SendCmd(cmd::SSD1306_NORMALDISPLAY);
	SendCmd(cmd::SSD1306_DISPLAYON);
}

void SSD1306::Display()
{
	SendCmd(cmd::SSD1306_COLUMNADDR);
	SendCmd(0);//cloumn start address
	SendCmd(SCREEN_WIDTH - 1); //cloumn end address
	SendCmd(cmd::SSD1306_PAGEADDR);
	SendCmd(0);//page atart address
	SendCmd(PAGES - 1);//page end address

	wiringPi_.DigitalWrite(DC_PIN, HIGH);
	wiringPi_.SPIDataRW(SPI_CHANNEL, buffer_.data(), buffer_.size());
}

void SSD1306::ClearBuffer()
{
	buffer_.fill(0);
}

void SSD1306::SetPixel(uint8_t x, uint8_t y, bool color)
{
	if (x > SCREEN_WIDTH || y > SCREEN_HEIGHT)
		return;
	if (color)
		buffer_[x + (y / 8)*SCREEN_WIDTH] |= 1 << (y % 8);
	else
		buffer_[x + (y / 8)*SCREEN_WIDTH] &= ~(1 << (y % 8));
}

void SSD1306::SetBitmap(uint8_t x, uint8_t y, Bitmap bitmap)
{
	uint8_t byteWidth = (bitmap.Width() + 7) / 8;
	auto w = bitmap.Width();
	auto h = bitmap.Height();
	auto pBmp = bitmap.GetData();

	for (unsigned int j = 0; j < h; ++j)
	{
		for (unsigned int i = 0; i < w; ++i)
		{
			if (*(pBmp + j*byteWidth + i / 8) & (128 >> (i & 7)))
			{
				SetPixel(x + i, y + j, 1);
			}
		}
	}
}

void SSD1306::Write(uint8_t x, uint8_t y, const std::string& str, const IFont& font)
{
	font.WriteOnScreen(x, y, str, *this);
}

void SSD1306::SendCmd(uint8_t cmd)
{
	wiringPi_.DigitalWrite(DC_PIN, LOW);
	wiringPi_.SPIDataRW(SPI_CHANNEL, &cmd, 1);
}

void SSD1306::SendCmd(SSD1306_CMDS cmd)
{
	SendCmd(uchar_cast(cmd));
}

//////////////////////////////////FONTS CLASSES///////////////////////////////////////////////////////////////////////

void Font12x6::WriteOnScreen(uint8_t x, uint8_t y, const std::string& str, SSD1306& screen) const
{
	auto filtered = FilterString(str);
	fonts::SSD1306_string(x, y, filtered.c_str(), charHeight, !mode_, screen);
}

std::string Font12x6::FilterString(const std::string& str)
{
	std::string copy(str);
	fonts::filterSmallFonts(copy);
	return copy;
}

void Font16x8::WriteOnScreen(uint8_t x, uint8_t y, const std::string& str, SSD1306& screen) const
{
	auto filtered = FilterString(str);
	fonts::SSD1306_string(x, y, filtered.c_str(), charHeight, !mode_, screen);
}

std::string Font16x8::FilterString(const std::string& str)
{
	std::string copy(str);
	fonts::filterSmallFonts(copy);
	return copy;
}

void Font16x16::WriteOnScreen(uint8_t x, uint8_t y, const std::string& str, SSD1306& screen) const
{
	auto filtered = FilterString(str);
	int length = (filtered.length()*charWidth <= SCREEN_WIDTH) ? filtered.length() : SCREEN_WIDTH / charWidth;
	for (auto i = 0; i < length; ++i)
	{
		fonts::SSD1306_char1616(x + (i*charWidth), y, filtered[i], screen);
	}
}

std::string Font16x16::FilterString(const std::string& str)
{
	std::string copy(str);
	fonts::filterBigFonts(copy);
	return copy;
}

void Font32x16::WriteOnScreen(uint8_t x, uint8_t y, const std::string& str, SSD1306& screen) const 
{
	auto filtered = FilterString(str);
	int length = (filtered.length()*charWidth <= SCREEN_WIDTH) ? filtered.length() : SCREEN_WIDTH / charWidth;
	for (auto i = 0; i < length; ++i)
	{
		fonts::SSD1306_char1616(x + (i*charWidth), y, filtered[i], screen);
	}
}

std::string OLED::Font32x16::FilterString(const std::string& str)
{
	std::string copy(str);
	fonts::filterBigFonts(copy);
	return copy;
}

void OLED::fonts::filterSmallFonts(std::string& str)
{
	for (auto& c : str)
	{
		if (c < ' ' || c > '~')
			c = ' ';
	}
}

void OLED::fonts::filterBigFonts(std::string& str)
{
	auto it = str.begin();
	while (it < str.end())
	{
		if ((*it < '0' || *it > '9') && *it != ':')
			str.erase(it);
		else
			++it;
	}
}

void fonts::SSD1306_char(unsigned char x, unsigned char y, char acsii, char size, char mode, SSD1306& screen)
{
	unsigned char i, j, y0 = y;
	char temp;
	unsigned char ch = acsii - ' ';
	for (i = 0; i<size; i++)
	{
		if (size == 12)
		{
			if (mode)temp = fonts::Font1206[ch][i];
			else temp = ~fonts::Font1206[ch][i];
		}
		else
		{
			if (mode)temp = fonts::Font1608[ch][i];
			else temp = ~fonts::Font1608[ch][i];
		}
		for (j = 0; j<8; j++)
		{
			if (temp & 0x80) screen.SetPixel(x, y, true);
			else screen.SetPixel(x, y, false);
			temp <<= 1;
			y++;
			if ((y - y0) == size)
			{
				y = y0;
				x++;
				break;
			}
		}
	}
}

void fonts::SSD1306_string(unsigned char x, unsigned  char y, const char *pString,
	unsigned char size, unsigned char mode, SSD1306& screen)
{
	while (*pString != '\0') {
		if (x > (SCREEN_WIDTH - size / 2)) {
			x = 0;
			y += size;
			if (y > (SCREEN_HEIGHT - size)) {
				y = x = 0;
			}
		}

		SSD1306_char(x, y, *pString, size, mode, screen);
		x += size / 2;
		pString++;
	}
}


template<class fontTabType, uint8_t xSize>
void SSD1306_char16(uint8_t x, uint8_t y, unsigned char chChar, fontTabType font, SSD1306& screen)
{
	uint8_t i, j;
	unsigned char chTemp = 0, y0 = y;
	bool chMode = false;

	for (i = 0; i < xSize * 2; ++i)
	{
		chTemp = font[chChar - 0x30][i];
		for (j = 0; j < 8; j++)
		{
			chMode = chTemp & 0x80 ? true : false;
			screen.SetPixel(x, y, chMode);
			chTemp <<= 1;
			++y;
			if ((y - y0) == xSize)
			{
				y = y0;
				++x;
				break;
			}
		}
	}
}

void fonts::SSD1306_char1616(uint8_t x, uint8_t y, unsigned char chChar, SSD1306& screen)
{
	SSD1306_char16<decltype(fonts::Font1616), 16>(x, y, chChar, fonts::Font1616, screen);
}

void fonts::SSD1306_char3216(uint8_t x, uint8_t y, unsigned char chChar, SSD1306& screen)
{
	SSD1306_char16<decltype(fonts::Font3216), 32>(x, y, chChar, fonts::Font3216, screen);
}
