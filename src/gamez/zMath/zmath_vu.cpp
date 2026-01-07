#include "zmath_vu.h"

static void zMathCopyVector(PNT3D& out, PNT3D& in)
{
#ifdef PS2 && PS2DEV
#else
	out.x = in.x;
	out.y = in.y;
	out.z = in.z;
#endif
}

static void zMathAddVector(PNT3D& sum, PNT3D& addend, PNT3D& summand)
{
	PNT3D work;
	work.x = addend.x + summand.x;
	work.y = addend.y + summand.y;
	work.z = addend.z + summand.z;
	zMathCopyVector(sum, work);
}

static void zMathSubVector(PNT3D& diff, PNT3D& subend, PNT3D& diffand)
{
	PNT3D work;
	work.x = subend.x - diffand.x;
	work.y = subend.y - diffand.y;
	work.z = subend.z - diffand.z;
	zMathCopyVector(diff, work);
}

static void zMathUnitMatrix(CMatrix& mat)
{
	memset(&mat, 0, sizeof(CMatrix));
	mat.m_matrix[0][0] = 1.0f;
	mat.m_matrix[1][1] = 1.0f;
	mat.m_matrix[2][2] = 1.0f;
	mat.m_matrix[3][3] = 1.0f;
}