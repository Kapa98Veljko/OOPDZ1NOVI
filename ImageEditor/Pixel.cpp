#include "Pixel.h"

Pixel::Pixel()
{
	this->r = 0;
	this->b = 0;
	this->g = 0;

}

Pixel::Pixel(int r, int g, int b)
{
	this->r = r;
	this->g = g;
	this->b = b;
}



void Pixel::setRed(unsigned int red)
{
	this->r = red;
}

void Pixel::setBlue(unsigned int blue)
{
	this->b = blue;
}

void Pixel::setGreen(unsigned int green)
{
	this->g = green;
}

unsigned int Pixel::getRed()
{
	return this->r;
}

unsigned int Pixel::getGreen()
{
	return this->g;
}

unsigned int Pixel::getBlue()
{
	return this->b;
}