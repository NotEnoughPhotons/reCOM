#include "zrender.h"

#include "gamez/zMath/zmath_vu.h"

CMatrix* CStack::m_pointer = NULL;
CMatrix* CStack::m_top = NULL;

CMatrix* CStack::Multiply(CMatrix* matrix, bool other)
{
	CMatrix m = *m_top;
	if (other)
	{
		m_top++;
		m_pointer++;
		zMathMulMatrix(m_top, &m, matrix);
	}
	else
		zMathMulMatrix(m_top, m_top, matrix);

	return m_top;
}