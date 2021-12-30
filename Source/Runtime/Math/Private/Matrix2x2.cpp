
#include "Precompiled.h"
using namespace CK;

const Matrix2x2 Matrix2x2::Identity(Vector2(1.f, 0.f), Vector2(0.f, 1.f));

std::vector<std::string> Matrix2x2::ToStrings() const
{
	std::vector<std::string> result;

	Matrix2x2 trMatrix = this->Transpose();
	for (BYTE i = 0; i < Rank; ++i)
	{
		char row[64];
		std::snprintf(row, sizeof(row), "| %.3f , %.3f |", trMatrix.Cols[i].X, trMatrix.Cols[i].Y);
		result.emplace_back(row);
	}
	return result;
}