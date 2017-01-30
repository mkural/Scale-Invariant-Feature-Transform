#pragma once

#ifndef MATRIX_X
#define MATRIX_H

#include "stdafx.h"
#include "Common.h"

class Matrix
{
private:
	int m_numRows;
	int m_numColumns;
	d_vect2_t m_data;
public:
	Matrix(int numRows, int numColumns);
	Matrix(int numRows, int numColumns, d_vect2_t data);
	d_vect2_t getData();
	int getNumRows();
	int getNumColumns();
	double det();
	Matrix cofactor(int x, int y);
	Matrix adjoint();
	Matrix multiply(Matrix otherMatrix);
	double get(int x, int y);
	void set(int x, int y, double newValue);
	Matrix scale(double ratio);
	Matrix inv();
	void print();
	double trace();
};
#endif