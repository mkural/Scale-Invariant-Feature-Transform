#include "stdafx.h"
#include "Common.h"
#include "ImagePyramid.h"

ImagePyramid::ImagePyramid() : ImagePyramid::ImagePyramid(ConstantValues::defaultNumberOfOctaves, ConstantValues::defaultLevelsPerOctave) {

}
ImagePyramid::ImagePyramid(int numberOfOctaves, int levelsPerOctave) : m_numberOfOctaves(numberOfOctaves), m_levelsPerOctave(levelsPerOctave)
{
	m_data.resize(numberOfOctaves);
	for (int i = 0; i < numberOfOctaves; i++) {
		m_data[i].resize(levelsPerOctave);
	}
}
int ImagePyramid::getOctaves()
{
	return m_numberOfOctaves;
}
int ImagePyramid::getLevels()
{
	return m_levelsPerOctave;
}
std::vector<std::vector<std::pair<Image, double>>> ImagePyramid::getData()//format of data is: [octave][level][{Image, scaleSigma}]
{
	return m_data;
}
void ImagePyramid::setImage(int octave, int level, Image newImage, double scale)
{
	m_data[octave][level] = std::make_pair(newImage,scale);
}
void ImagePyramid::displayImagePyramid()
{

	for (int i = 0; i < getOctaves(); i++) {
		for (int j = 0; j < getLevels(); j++) {
			std::cout << i << " " << j << std::endl;
			getData()[i][j].first.displayImage();
		}
	}
}
std::vector<std::vector<int>> ImagePyramid::findInitScaleSpaceExtrema() {
	//finds 3D extrema by first finding all local extrema in each image
	//and testing these candidates by comparing to adjacent levels in the octave
	//note that we exclude the edges of the images and the "edges" of each octave from candidacy
	std::vector<std::vector<int>> initExtrema {};
	int maxOctave = getOctaves();
	int maxLevels = getLevels();
	for (int octave = 0; octave < maxOctave; octave++) {
		int xMax = getData()[octave][0].first.getWidth();
		int yMax = getData()[octave][0].first.getHeight();
		for (int level = 1; level < maxLevels - 1; level++) {
			int count = 0;
			//we store the images at levels level-1, level, level+1 for comparison
			Image currentImage[3]{ getData()[octave][level - 1].first,getData()[octave][level].first, getData()[octave][level + 1].first };
			std::vector<std::vector<int>> imageExtrema = currentImage[1].findInitFlatExtrema();
			for (std::vector<int> coordinates: imageExtrema) {
				count++;
				//if (rand() % 100 == 0) std::cout << initExtrema.size() << " " <<octave << " " << level <<" "<< coordinates[0] << " " << coordinates[1] << " " <<count << std::endl;
				int xCoord = coordinates[0];
				int yCoord = coordinates[1];
				int max = coordinates[2];//tells us if it's a max or a min-0 is min, 1 is max
				bool actualExtreme = true;
				double val = currentImage[1].getPixel(xCoord, yCoord);
				
				for (int compareLevel = 0; compareLevel <= 2; compareLevel += 2) {
					for (int compareXCoord = xCoord - 1; compareXCoord <= xCoord + 1; compareXCoord++) {
						for (int compareYCoord = yCoord - 1; compareYCoord <= yCoord + 1; compareYCoord++) {
							if ((val>currentImage[compareLevel].getPixel(compareXCoord, compareYCoord))!=(max == 1)) {
								actualExtreme = false;
								goto notExtreme;
							}

						}
					}
				}
			notExtreme:
				if (!actualExtreme) continue;
				initExtrema.push_back(std::vector<int>{octave, level, coordinates[0], coordinates[1]});
			}
		}
	}
	return initExtrema;
}