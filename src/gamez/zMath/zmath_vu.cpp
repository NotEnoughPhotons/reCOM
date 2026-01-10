#include "zmath_vu.h"

void zMathCopyVector(PNT3D& out, PNT3D& in)
{
#ifdef PS2 && PS2DEV
#else
	out.x = in.x;
	out.y = in.y;
	out.z = in.z;
#endif
}

void zMathAddVector(PNT3D& sum, PNT3D& addend, PNT3D& summand)
{
	PNT3D work;
	work.x = addend.x + summand.x;
	work.y = addend.y + summand.y;
	work.z = addend.z + summand.z;
	zMathCopyVector(sum, work);
}

void zMathSubVector(PNT3D& diff, PNT3D& subend, PNT3D& diffand)
{
	PNT3D work;
	work.x = subend.x - diffand.x;
	work.y = subend.y - diffand.y;
	work.z = subend.z - diffand.z;
	zMathCopyVector(diff, work);
}

void zMathUnitMatrix(CMatrix& mat)
{
	memset(&mat, 0, sizeof(CMatrix));
	mat.m_matrix[0][0] = 1.0f;
	mat.m_matrix[1][1] = 1.0f;
	mat.m_matrix[2][2] = 1.0f;
	mat.m_matrix[3][3] = 1.0f;
}

void zMathMulMatrix(CMatrix* m0, CMatrix* m1, CMatrix* out)
{
	s32 i, j, k;
	f32* pA = (f32*)m1;
	f32* pB = (f32*)m0;
	f32* pQ = (f32*)out;
	f32 pM[16];

	for (s32 i = 0; i < 16; i++)
		pM[i] = 0;

	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
			for (k = 0; k < 4; k++)
				pM[4 * i + j] += pA[4 * k + j] * pB[4 * i + k];

	for (i = 0; i < 16; i++)
		pQ[i] = pM[i];
;}