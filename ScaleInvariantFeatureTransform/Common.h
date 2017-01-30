#pragma once
#ifndef COMMON_H
#define COMMON_H

#include <cstdio>
#include <cstdlib>
#include <vector>
#include <string>
#include <cassert>
#include <iostream>

using d_vect_t = std::vector<double>;
using d_vect2_t = std::vector<std::vector<double>>;

namespace ConstantValues
{
	const int defaultHeight = 500;
	const int defaultWidth = 500;
	const double eps = 1e-5;
	const double defaultSigma = sqrt(2);
	const double windowToSigmaRatio = 3.0;
	const int defaultNumberOfOctaves = 4;
	const int defaultLevelsPerOctave = 5;
	const double PI = 3.1415926535897932384626;
	const double scaleMultiplier = sqrt(2);
	const double defaultScaleSigma = 1.6;
}

#endif