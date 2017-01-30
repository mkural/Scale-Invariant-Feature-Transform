#pragma once

//depends on initial image, number octaves, number of guys per octave, and k (where hopefully k^5 = an octave or something...)

#ifndef IMAGEPYRAMID_H
#define IMAGEPYRAMID_H

#include "Common.h"
#include "Image.h"
class ImagePyramid
{
private:
	int m_numberOfOctaves;
	int m_levelsPerOctave;
	std::vector<std::vector<std::pair<Image, double>>> m_data;
public:
	ImagePyramid();
	ImagePyramid(int numberOfOctaves, int levelsPerOctave);
	int getOctaves();
	int getLevels();
	std::vector<std::vector<std::pair<Image, double>>> getData();
	void setImage(int octave, int level, Image newImage, double scale);
	void displayImagePyramid();
	std::vector<std::vector<int>> findInitScaleSpaceExtrema();//data format: octave, level, x, y
};
#endif