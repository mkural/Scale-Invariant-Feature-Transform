// ScaleInvariantFeatureTransform.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Common.h"
#include "Image.h"
#include "CImg.h"
#include "ImagePyramid.h"
#include "Matrix.h"
#include <fstream>

using namespace cimg_library;


ImagePyramid GaussianPyramid(Image inputImage) {
	//we will build a pyramid as described by lowe
	//at each level, the scale space factor sigma is multiplied by k, which satisfies k^(levelsPerOctave-3)=2.
	const double k = ConstantValues::scaleMultiplier;
	//sigmaZero is the initial value of sigma, so that the scale varies from sigmaZero/k, sigmaZero, sigmaZero*k,...sigmaZero*2,sigmaZero*2*k in the first octave
	//we need this range of scales so that our difference of gaussians pyramid can range from sigmaZero/k to sigmaZero*2,
	//which would give us an entire octave sigmaZero, sigmaZero*k,...sigmaZero*2/k in which local extrema can be found
	const double sigmaZero = ConstantValues::defaultScaleSigma*k;
	ImagePyramid scaleSpacePyramid{};
	//set up first octave
	scaleSpacePyramid.setImage(0,0,inputImage.blur(sigmaZero / k),sigmaZero/k);
	for (int j = 1; j < scaleSpacePyramid.getLevels(); j++) {
		double lastScale = scaleSpacePyramid.getData()[0][j - 1].second;
		//we need to blur successively along the octave, with marginal blur sigma = sqrt(k^(2j)*sigmaZero^2-k^(2j-2)*sigmaZero^2) at each step
		//noting that convolving two gaussians with sigma1, sigma2 yields a new sigma=sqrt(sigma1^2+sigma2^2)
		scaleSpacePyramid.setImage(0, j, scaleSpacePyramid.getData()[0][j - 1].first.blur(lastScale*sqrt(k*k-1)), lastScale*k);
		
	}
	for (int i = 1; i < scaleSpacePyramid.getOctaves(); i++) {
		//we start each successive octave by resizing by a factor of 0.5 at the appropriate scale, which is levelsPerOctave-3 = log_k(2) 
		//levels into the previous octave
		scaleSpacePyramid.setImage(i, 0, scaleSpacePyramid.getData()[i - 1][scaleSpacePyramid.getLevels() - 3].first.scaleRatio(0.5), scaleSpacePyramid.getData()[i - 1][scaleSpacePyramid.getLevels() - 3].second);
		//like the first octave, we can show that at each step if the blur multiplies by k, we must blur marginally by lastScale*sqrt(k^2-1)
		for (int j = 1; j < scaleSpacePyramid.getLevels(); j++) {
			double lastScale = scaleSpacePyramid.getData()[i][j - 1].second;
			scaleSpacePyramid.setImage(i, j, scaleSpacePyramid.getData()[i][j - 1].first.blur(lastScale*sqrt(k*k-1)), lastScale*k);
		}
	}
	return scaleSpacePyramid;
}

ImagePyramid FiniteDifference(ImagePyramid initialPyramid) {//given an initial pyramid, we can find the finite differences at each level, storing differences between subsequent images
	//number of levels is reduced by 1
	ImagePyramid newPyramid{initialPyramid.getOctaves(),initialPyramid.getLevels()-1};
	for (int i = 0; i < initialPyramid.getOctaves(); i++) {
		for (int j = 0; j < initialPyramid.getLevels() - 1;j++) {
			newPyramid.setImage(i,j,initialPyramid.getData()[i][j].first.subtract(initialPyramid.getData()[i][j + 1].first), initialPyramid.getData()[i][j].second);
		}
	}
	return newPyramid;
}

ImagePyramid DifferenceOfGaussian(Image inputImage) {
	return FiniteDifference(GaussianPyramid(inputImage));
}

void getFeatures(Image inputImage) {

	ImagePyramid DoG = DifferenceOfGaussian(inputImage.blur(0.5).scaleRatio(2.0));
	std::vector<std::vector<int>> initExtrema = DoG.findInitScaleSpaceExtrema();
	std::ofstream fout("ListOfFeatures.out");
	if (!fout) {
		std::cerr << "Failed to open ListOfFeatures.out" << std::endl;
		exit(1);
	}
	fout << "Number of features is: " << initExtrema.size() << "\n";
	for (std::vector<int> feature: initExtrema) {
		fout << "Feature at octave: " << feature[0] << ", level: " << feature[1] << ", x coordinate: " <<feature[2] << ", y coordinate: " << feature[3] << "\n";
	}
	fout << std::endl;
}

int main()
{
	CImg<unsigned char> inputCImage("lena.bmp");//we can input other filenames
	Image originalImage{ inputCImage.width(),inputCImage.height() };
	for (int i = 0; i < inputCImage.width(); i++) {
		for (int j = 0; j < inputCImage.height(); j++) {
			originalImage.setPixel(i, j, static_cast<double>(inputCImage(i, j))/255.0);
		}
	}
	getFeatures(originalImage);
	return 0;
}
