#pragma once
#ifndef IMAGE_H
#define IMAGE_H


#include "CImg.h"
#include "Common.h"

class Image {
private:
	int m_width;
	int m_height;
	d_vect2_t m_intensity;//double greyscale pixel intensity, of dimensions [m_width][m_height]
public:
	Image();
	Image(int width, int height);
	void setPixel(int xCoord, int yCoord, double intensity);
	Image resize(int newWidth, int newHeight);
	Image scaleRatio(double ratio);
	Image subtract(Image anotherImage);
	Image blur(double scaleSigma);
	double getPixel(int xCoord, int yCoord);
	double getPixel(double xCoord, double yCoord);
	//although pixel values are not defined for non-integer coordinates, we can use bilinear interpolation
	//to get pixel values within the borders of the image
	int getWidth();
	int getHeight();
	bool xInRange(int xCoord);
	bool xInRange(double xCoord);
	bool yInRange(int yCoord);
	bool yInRange(double yCoord);
	void displayImage();
	std::vector<std::vector<int>> Image::findInitFlatExtrema();
};
#endif