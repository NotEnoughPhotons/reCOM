#include "zmath.h"
#include "glm/ext/matrix_transform.hpp"

CMatrix::CMatrix()
{
	*this = CMatrix::identity;
}

CMatrix::CMatrix(f32 m00, f32 m01, f32 m02, f32 m03, f32 m10, f32 m11, f32 m12, f32 m13, f32 m20, f32 m21, f32 m22, f32 m23, f32 m30, f32 m31, f32 m32, f32 m33)
{
	m_matrix[0][0] = m00;
	m_matrix[0][1] = m01;
	m_matrix[0][2] = m02;
	m_matrix[0][3] = m03;

	m_matrix[1][0] = m10;
	m_matrix[1][1] = m11;
	m_matrix[1][2] = m12;
	m_matrix[1][3] = m13;

	m_matrix[2][0] = m20;
	m_matrix[2][1] = m21;
	m_matrix[2][2] = m22;
	m_matrix[2][3] = m23;

	m_matrix[3][0] = m30;
	m_matrix[3][1] = m31;
	m_matrix[3][2] = m32;
	m_matrix[3][3] = m33;
}

const float* CMatrix::operator[](const int row)
{
	return m_matrix[row];
}

const float* CMatrix::GetTranslate() const
{
	return m_matrix[3];
}

/// Transform a matrix by a point.
void CMatrix::Transform(CPnt3D* point, s32 count)
{
	m_matrix[3][0] = point->x;
	m_matrix[3][1] = point->y;
	m_matrix[3][2] = point->z;
	m_matrix[3][3] = 1.0f;
}

void CMatrix::Multiply(const CMatrix* first, const CMatrix* second)
{
	// First row
	m_matrix[0][0] = 
		(first->m_matrix[0][0] * second->m_matrix[0][0]) + 
		(first->m_matrix[0][1] * second->m_matrix[1][0]) + 
		(first->m_matrix[0][2] * second->m_matrix[2][0]) + 
		(first->m_matrix[0][3] * second->m_matrix[3][0]);

	m_matrix[0][1] =
		(first->m_matrix[0][0] * second->m_matrix[0][1]) +
		(first->m_matrix[0][1] * second->m_matrix[1][1]) +
		(first->m_matrix[0][2] * second->m_matrix[2][1]) +
		(first->m_matrix[0][3] * second->m_matrix[3][1]);

	m_matrix[0][2] =
		(first->m_matrix[0][0] * second->m_matrix[0][2]) +
		(first->m_matrix[0][1] * second->m_matrix[1][2]) +
		(first->m_matrix[0][2] * second->m_matrix[2][2]) +
		(first->m_matrix[0][3] * second->m_matrix[3][2]);

	m_matrix[0][3] =
		(first->m_matrix[0][0] * second->m_matrix[0][3]) +
		(first->m_matrix[0][1] * second->m_matrix[1][3]) +
		(first->m_matrix[0][2] * second->m_matrix[2][3]) +
		(first->m_matrix[0][3] * second->m_matrix[3][3]);

	// Second row
	m_matrix[1][0] =
		(first->m_matrix[1][0] * second->m_matrix[0][0]) +
		(first->m_matrix[1][1] * second->m_matrix[1][0]) +
		(first->m_matrix[1][2] * second->m_matrix[2][0]) +
		(first->m_matrix[1][3] * second->m_matrix[3][0]);

	m_matrix[1][1] =
		(first->m_matrix[1][0] * second->m_matrix[0][1]) +
		(first->m_matrix[1][1] * second->m_matrix[1][1]) +
		(first->m_matrix[1][2] * second->m_matrix[2][1]) +
		(first->m_matrix[1][3] * second->m_matrix[3][1]);

	m_matrix[1][2] =
		(first->m_matrix[1][0] * second->m_matrix[0][2]) +
		(first->m_matrix[1][1] * second->m_matrix[1][2]) +
		(first->m_matrix[1][2] * second->m_matrix[2][2]) +
		(first->m_matrix[1][3] * second->m_matrix[3][2]);

	m_matrix[1][3] =
		(first->m_matrix[1][0] * second->m_matrix[0][3]) +
		(first->m_matrix[1][1] * second->m_matrix[1][3]) +
		(first->m_matrix[1][2] * second->m_matrix[2][3]) +
		(first->m_matrix[1][3] * second->m_matrix[3][3]);

	// Third row
	m_matrix[2][0] =
		(first->m_matrix[2][0] * second->m_matrix[0][0]) +
		(first->m_matrix[2][1] * second->m_matrix[1][0]) +
		(first->m_matrix[2][2] * second->m_matrix[2][0]) +
		(first->m_matrix[2][3] * second->m_matrix[3][0]);

	m_matrix[2][1] =
		(first->m_matrix[2][0] * second->m_matrix[0][1]) +
		(first->m_matrix[2][1] * second->m_matrix[1][1]) +
		(first->m_matrix[2][2] * second->m_matrix[2][1]) +
		(first->m_matrix[2][3] * second->m_matrix[3][1]);

	m_matrix[2][2] =
		(first->m_matrix[2][0] * second->m_matrix[0][2]) +
		(first->m_matrix[2][1] * second->m_matrix[1][2]) +
		(first->m_matrix[2][2] * second->m_matrix[2][2]) +
		(first->m_matrix[2][3] * second->m_matrix[3][2]);

	m_matrix[2][3] =
		(first->m_matrix[2][0] * second->m_matrix[0][3]) +
		(first->m_matrix[2][1] * second->m_matrix[1][3]) +
		(first->m_matrix[2][2] * second->m_matrix[2][3]) +
		(first->m_matrix[2][3] * second->m_matrix[3][3]);

	// Fourth row
	m_matrix[3][0] =
		(first->m_matrix[3][0] * second->m_matrix[0][0]) +
		(first->m_matrix[3][1] * second->m_matrix[1][0]) +
		(first->m_matrix[3][2] * second->m_matrix[2][0]) +
		(first->m_matrix[3][3] * second->m_matrix[3][0]);

	m_matrix[3][1] =
		(first->m_matrix[3][0] * second->m_matrix[0][1]) +
		(first->m_matrix[3][1] * second->m_matrix[1][1]) +
		(first->m_matrix[3][2] * second->m_matrix[2][1]) +
		(first->m_matrix[3][3] * second->m_matrix[3][1]);

	m_matrix[3][2] =
		(first->m_matrix[3][0] * second->m_matrix[0][2]) +
		(first->m_matrix[3][1] * second->m_matrix[1][2]) +
		(first->m_matrix[3][2] * second->m_matrix[2][2]) +
		(first->m_matrix[3][3] * second->m_matrix[3][2]);

	m_matrix[3][3] =
		(first->m_matrix[3][0] * second->m_matrix[0][3]) +
		(first->m_matrix[3][1] * second->m_matrix[1][3]) +
		(first->m_matrix[3][2] * second->m_matrix[2][3]) +
		(first->m_matrix[3][3] * second->m_matrix[3][3]);
}

void CMatrix::ToEuler(CPnt3D* p)
{
	p->x = acosf(-m_matrix[1][2]);
	p->y = atan2f(-m_matrix[2][2], m_matrix[0][2]);
	p->z = atan2f(-m_matrix[1][0], m_matrix[1][1]);
}

// TODO: Implement matrix conversion to quaternion
void CMatrix::ToQuat(CQuat* q)
{
	*q = CQuat::identity;
}

void CMatrix::SetRows(const CPnt3D* m0, const CPnt3D* m1, const CPnt3D* m2, const CPnt3D* m3)
{
	if (m0) memcpy(this->m_matrix[0], m0, sizeof(CPnt3D));
	if (m1) memcpy(this->m_matrix[1], m1, sizeof(CPnt3D));
	if (m2) memcpy(this->m_matrix[2], m2, sizeof(CPnt3D));
	if (m3) memcpy(this->m_matrix[3], m3, sizeof(CPnt3D));
}

void CMatrix::SetZero()
{
	memset(this, 0, sizeof(CMatrix));
}