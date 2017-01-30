#include "stdafx.h"
#include "CImg.h"
#include "Common.h"
#include "Image.h"


Image::Image(int width, int height) : m_height(height), m_width(width) {
	m_intensity.resize(m_width);
	for (int i = 0; i < m_width; i++) {
		m_intensity[i].resize(m_height);
	}
}
Image::Image() : Image::Image(ConstantValues::defaultWidth, ConstantValues::defaultHeight) {

}
void Image::setPixel(int xCoord, int yCoord, double intensity) {
	assert(xInRange(xCoord));
	assert(yInRange(yCoord));
	m_intensity[xCoord][yCoord] = intensity;
}

Image Image::resize(int newWidth, int newHeight)//scales an image to a new level, finding new pixel values by bilinear interpolation
{
	Image newImage{ newWidth,newHeight };
	for (int i = 0; i < newWidth; i++) {
		for (int j = 0; j < newHeight; j++) {
			newImage.setPixel(i,j,getPixel(((static_cast<double>(i) / newWidth)*getWidth()), ((static_cast<double>(j) / newWidth)*getWidth())));
		}
	}
	return newImage;
}

Image Image::scaleRatio(double ratio) {
	return resize((int)(ratio*getWidth()), (int)(ratio*getHeight()));
}

Image Image::subtract(Image anotherImage) {//subtract an image from another,  pixelwise
	Image newImage{ m_width,m_height };
	for (int i = 0; i < m_width; i++) {
		for (int j = 0; j < m_height; j++) {
			newImage.setPixel(i, j, this->getPixel(i, j) - anotherImage.getPixel(i, j));
		}
	}
	return newImage;
}

Image Image::blur(double scaleSigma) {//we convolve a gaussian kernel of standard deviation scaleSigma with the image.
	//this is sped up by decomposing the filter into two parts, one for the x and one for the y dimension
	int window = (int)ceil(scaleSigma*ConstantValues::windowToSigmaRatio);//need the window to be several times larger than scaleSigma
	std::vector<double> gaussian{};
	gaussian.resize(2 * window + 1);
	double sumOfWeights = 0.0;
	for (int a = -window; a <= window; a++) {
		gaussian[a + window] = exp((-a*a) / (2.0*scaleSigma*scaleSigma)) / (sqrt(2 * ConstantValues::PI)*scaleSigma);
		sumOfWeights += gaussian[a + window];
	}
	for (int a = -window; a <= window; a++) {
		gaussian[a + window] /= sumOfWeights;//normalize to 1
	}
	Image firstNewImage{ getWidth(),getHeight() };//convolve with the x-gaussian filter 
	for (int x = 0; x < getWidth(); x++) {
		for (int y = 0; y < getHeight(); y++) {
			double newPixelValue = 0.0;
			for (int a = -window; a <= window; a++) {
				newPixelValue += gaussian[a+window] * getPixel(x - a, y);
			}
			firstNewImage.setPixel(x, y, newPixelValue);
		}
	}
	Image secondNewImage{ getWidth(),getHeight() };//convolve with the y-gaussian filter
	for (int x = 0; x < getWidth(); x++) {
		for (int y = 0; y < getHeight(); y++) {
			double newPixelValue = 0.0;
			for (int a = -window; a <= window; a++) {
				newPixelValue += gaussian[a+window] * firstNewImage.getPixel(x, y-a);
			}
			secondNewImage.setPixel(x, y, newPixelValue);
		}
	}

	return secondNewImage;
}
double Image::getPixel(int xCoord, int yCoord) {
	if (!((xInRange(xCoord)) && (yInRange(yCoord)))) return 0.0;
	return m_intensity[xCoord][yCoord];
}
double Image::getPixel(double xCoord, double yCoord)//bilinear interpolation to get pixel values at indices which are not integers
{
	if (!((xInRange(xCoord)) && (yInRange(yCoord)))) return 0.0;
	int x = (int)xCoord;
	int y = (int)yCoord;
	double r = xCoord - x;
	double s = yCoord - y;
	return r*s*getPixel(x, y) + (1 - r)*s*getPixel(x + 1, y) + r*(1 - s)*getPixel(x, y + 1) + (1 - r)*(1 - s)*getPixel(x + 1, y + 1);
}
int Image::getHeight() {
	return m_height;
}
int Image::getWidth() {
	return m_width;
}
//the following four functions detect whether an index is in the valid range to be part of the image.
bool Image::xInRange(int xCoord) {
	return 0 <= xCoord&&xCoord <= this->getWidth()-1;
}
bool Image::xInRange(double xCoord) {
	return ConstantValues::eps <= xCoord&&xCoord <= static_cast<double>(this->getWidth()-1) - ConstantValues::eps;
}
bool Image::yInRange(int yCoord) {
	return 0 <= yCoord&&yCoord <= this->getHeight()-1;
}
bool Image::yInRange(double yCoord) {
	return ConstantValues::eps <= yCoord&&yCoord <= static_cast<double>(this->getHeight()-1) - ConstantValues::eps;
}

void Image::displayImage()//display using CImg, useful for testing
{
	const unsigned int width = getWidth();
	const unsigned int height = getHeight();
	cimg_library::CImg<unsigned char> displayCImage{ width, height,1,1 };
	for (int i = 0; i < getWidth(); i++) {
		for (int j = 0; j < getHeight(); j++) {
			const unsigned int xIndex = i;
			const unsigned int yIndex = j;
			displayCImage(xIndex,yIndex) = static_cast<unsigned char>(std::max(std::min(255.0*getPixel(i, j), 255.0), 0.0));
		}
	}
	cimg_library::CImgDisplay main_disp(displayCImage);
	while (!main_disp.is_closed()) {
		main_disp.wait();
	}
}

std::vector<std::vector<int>> Image::findInitFlatExtrema() {//this function finds all local extrema on a 2D image. we will use this to find 3D extrema later.
	//returns x coordinate, y coordinate, and an integer which is 1 if it's a maximum, 0 if it's a minimum
	std::vector < std::vector<int>> answer;
	for (int i = 1; i < -1+getWidth(); i++) {
		for (int j = 1; j < -1+getHeight(); j++) {
			bool extreme = true;
			double val = getPixel(i, j);
			if (val > getPixel(i - 1, j - 1)) {
				for (int k = -1; k <= 1; k++) {
					for (int l = -1; l <= 1; l++) {
						if (val < getPixel(i + k, j + l)) {
							extreme = false;
							goto extremeFail;
						}
					}
				}
			}
			else {
				for (int k = -1; k <= 1; k++) {
					for (int l = -1; l <= 1; l++) {
						if (val > getPixel(i + k, j + l)) {
							extreme = false;
							goto extremeFail;
						}
					}
				}
			}
		extremeFail:
			if (extreme) {
				int testIfMaximum(0);//this variable is 0 if it's a min, 1 if it's a max
				if (val > getPixel(i - 1, j - 1)) testIfMaximum = 1;
				answer.push_back(std::vector<int>{i, j,testIfMaximum});
			}
		}
	}
	return answer;
}