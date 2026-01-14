#include "zmath_vu.h"

// https://stackoverflow.com/questions/2624422/efficient-4x4-matrix-inverse-affine-transform
void Vu0MatrixInverseWithUniformScale(CMatrix* dst, CMatrix* src)
{
	f32 s0 = src->m_matrix[0][0] * src->m_matrix[1][1] - src->m_matrix[1][0] * src->m_matrix[0][1];
	f32 s1 = src->m_matrix[0][0] * src->m_matrix[1][2] - src->m_matrix[1][0] * src->m_matrix[0][2];
	f32 s2 = src->m_matrix[0][0] * src->m_matrix[1][3] - src->m_matrix[1][0] * src->m_matrix[0][3];
	f32 s3 = src->m_matrix[0][1] * src->m_matrix[1][2] - src->m_matrix[1][1] * src->m_matrix[0][2];
	f32 s4 = src->m_matrix[0][1] * src->m_matrix[1][3] - src->m_matrix[1][1] * src->m_matrix[0][3];
	f32 s5 = src->m_matrix[0][2] * src->m_matrix[1][3] - src->m_matrix[1][2] * src->m_matrix[0][3];

	f32 c5 = src->m_matrix[2][2] * src->m_matrix[3][3] - src->m_matrix[3][2] * src->m_matrix[2][3];
	f32 c4 = src->m_matrix[2][1] * src->m_matrix[3][3] - src->m_matrix[3][1] * src->m_matrix[2][3];
	f32 c3 = src->m_matrix[2][1] * src->m_matrix[3][2] - src->m_matrix[3][1] * src->m_matrix[2][2];
	f32 c2 = src->m_matrix[2][0] * src->m_matrix[3][3] - src->m_matrix[3][0] * src->m_matrix[2][3];
	f32 c1 = src->m_matrix[2][0] * src->m_matrix[3][2] - src->m_matrix[3][0] * src->m_matrix[2][2];
	f32 c0 = src->m_matrix[2][0] * src->m_matrix[3][1] - src->m_matrix[3][0] * src->m_matrix[2][1];

	f32 div = (s0 * c5 - s1 * c4 + s2 * c3 + s3 * c2 - s4 * c1 + s5 * c0);

	// Check against divide by zero
	if (div <= 0.0000000125f)
		return;

	f32 invdet = 1.0f / div;

	dst->m_matrix[0][0] = (src->m_matrix[1][1] * c5 - src->m_matrix[1][2] * c4 + src->m_matrix[1][3] * c3) * invdet;
	dst->m_matrix[0][1] = (-src->m_matrix[0][1] * c5 + src->m_matrix[0][2] * c4 - src->m_matrix[0][3] * c3) * invdet;
	dst->m_matrix[0][2] = (src->m_matrix[3][1] * s5 - src->m_matrix[3][2] * s4 + src->m_matrix[3][3] * s3) * invdet;
	dst->m_matrix[0][3] = (src->m_matrix[2][1] * s5 - src->m_matrix[2][2] * s4 + src->m_matrix[2][3] * s3) * invdet;

	dst->m_matrix[1][0] = (-src->m_matrix[1][0] * c5 + src->m_matrix[1][2] * c2 - src->m_matrix[1][3] * c1) * invdet;
	dst->m_matrix[1][1] = (src->m_matrix[0][0] * c5 - src->m_matrix[0][2] * c2 + src->m_matrix[0][3] * c1) * invdet;
	dst->m_matrix[1][2] = (-src->m_matrix[3][0] * s5 + src->m_matrix[3][2] * s2 - src->m_matrix[3][3] * s1) * invdet;
	dst->m_matrix[1][3] = (src->m_matrix[2][0] * s5 - src->m_matrix[2][2] * s2 + src->m_matrix[2][3] * s1) * invdet;

	dst->m_matrix[2][0] = (src->m_matrix[1][0] * c4 - src->m_matrix[1][1] * c2 + src->m_matrix[1][3] * c0) * invdet;
	dst->m_matrix[2][1] = (-src->m_matrix[0][0] * c4 + src->m_matrix[0][1] * c2 - src->m_matrix[0][3] * c0) * invdet;
	dst->m_matrix[2][2] = (src->m_matrix[3][0] * s4 - src->m_matrix[3][1] * s2 + src->m_matrix[3][3] * s0) * invdet;
	dst->m_matrix[2][3] = (-src->m_matrix[2][0] * s4 + src->m_matrix[2][1] * s2 - src->m_matrix[2][3] * s0) * invdet;

	dst->m_matrix[3][0] = (-src->m_matrix[1][0] * c3 + src->m_matrix[1][1] * c1 - src->m_matrix[1][2] * c0) * invdet;
	dst->m_matrix[3][1] = (src->m_matrix[0][0] * c3 - src->m_matrix[0][1] * c1 + src->m_matrix[0][2] * c0) * invdet;
	dst->m_matrix[3][2] = (-src->m_matrix[3][0] * s3 + src->m_matrix[3][1] * s1 - src->m_matrix[3][2] * s0) * invdet;
	dst->m_matrix[3][3] = (src->m_matrix[2][0] * s3 - src->m_matrix[2][1] * s1 + src->m_matrix[2][2] * s0) * invdet;
}

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

void zMathCopyMatrix(CMatrix* dest, const CMatrix* src)
{
	s32 i, j, k;
	f32* pA = (f32*)dest;
	f32* pB = (f32*)src;

	for (i = 0; i < 16; i++)
		pA[i] = pB[i];
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