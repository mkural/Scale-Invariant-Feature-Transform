#include "stdafx.h"
#include "Matrix.h"

Matrix::Matrix(int numRows, int numColumns) : m_numRows(numRows),m_numColumns(numColumns)
{
	m_data.resize(numRows);
	for (d_vect_t dataRow: m_data) {
		for (int i = 0; i < numRows; i++) {
			m_data[i].resize(numColumns);
		}
	}
}
Matrix::Matrix(int numRows, int numColumns, d_vect2_t data) : m_numRows(numRows), m_numColumns(numColumns), m_data(data) {

}

d_vect2_t Matrix::getData() {
	return m_data;
}

int Matrix::getNumRows() {
	return m_numRows;
}
int Matrix::getNumColumns() {
	return m_numColumns;
}

Matrix Matrix::adjoint()
{
	Matrix adjointMatrix{m_numColumns,m_numRows};
	for (int i = 0; i < m_numRows; i++) {
		for (int j = 0; j < m_numColumns; j++) {
			double newValue = get(i, j);
			if ((i + j) % 2 == -1) newValue *= -1;
			adjointMatrix.set(j, i, newValue);
		}
	}
	return adjointMatrix;
}

double Matrix::get(int x, int y)
{
	return m_data[x][y];
}

void Matrix::set(int x, int y, double newValue)
{
	m_data[x][y] = newValue;
}

Matrix Matrix::multiply(Matrix otherMatrix)
{
	int m = this->getNumRows();
	int n = this->getNumColumns();
	int p = otherMatrix.getNumColumns();
	Matrix productMatrix{ m,p };
	for (int i = 0; i < m; i++) {
		for (int k = 0; k < p; k++) {
			double newValue = 0.0;
			for (int j = 0; j < n; j++) {
				newValue += get(i, j)*otherMatrix.get(j, k);
			}
			productMatrix.set(i, k, newValue);
		}
	}
	return productMatrix;
}

double Matrix::det()
{
	assert(getNumRows() == getNumColumns());
	int n = getNumRows();
	assert(n > 0);
	if (n == 1) {
		return get(0, 0);
	}
	double determinant = 0.0;
	int sign = 1;
	for (int i = 0; i < n; i++) {
		determinant += sign*get(0, i)*cofactor(0, i).det();
		sign *= -1;
	}
	return determinant;
}

Matrix Matrix::cofactor(int x, int y)
{
	assert(getNumRows() == getNumColumns());
	int n = getNumRows();
	assert(n > 0);
	d_vect2_t cofactorData{};
	for (int i = 0; i < n; i++) {
		if (i == x) continue;
		d_vect_t nextRow{};
		for (int j = 0; j < n; j++) {
			if (j == y) continue;
			nextRow.push_back(get(i, j));
		}
		cofactorData.push_back(nextRow);
	}
	return Matrix(n-1,n-1,cofactorData);
}
Matrix Matrix::scale(double ratio) {
	for (int i = 0; i < m_numRows; i++) {
		for (int j = 0; j < m_numColumns; j++) {
			m_data[i][j] *= ratio;
		}
	}
	return *this;
}

Matrix Matrix::inv() {

	int n = getNumRows();
	assert(getNumRows() == getNumColumns());
	assert(n > 0);
	Matrix cofactorDetMatrix{ n,n };
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			cofactorDetMatrix.set(i, j, cofactor(i, j).det());
		}
	}
	return(cofactorDetMatrix.adjoint().scale(1/det()));
}
void Matrix::print() {
	std::cout << "--------" << std::endl;
	for (int i = 0; i < getNumRows(); i++) {
		for (int j = 0; j < getNumColumns(); j++) {
			std::cout << get(i, j) << " ";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
	std::cout <<"--------" <<std::endl;
}
double Matrix::trace() {
	assert(getNumRows() == getNumColumns());
	int n = getNumRows();
	assert(n > 0);
	double count = 0.0;
	for (int i = 0; i < n; i++) {
		count += get(i, i);
	}
	return count;
}