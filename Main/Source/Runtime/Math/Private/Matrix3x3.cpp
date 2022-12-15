
#include "Precompiled.h"
using namespace CK;

const Matrix3x3 Matrix3x3::Identity(Vector3(1.f, 0.f, 0.f), Vector3(0.f, 1.f, 0.f), Vector3(0.f, 0.f, 1.f));

std::vector<std::string> Matrix3x3::ToStrings() const
{
	std::vector<std::string> result;

	Matrix3x3 trMatrix = this->Transpose();
	for (BYTE i = 0; i < Rank; ++i)
	{
		char row[64];
		std::snprintf(row, sizeof(row), "| %.3f , %.3f , %.3f |", trMatrix.Cols[i].X, trMatrix.Cols[i].Y, trMatrix.Cols[i].Z);
		result.emplace_back(row);
	}
	return result;
}