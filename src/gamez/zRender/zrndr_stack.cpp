#include "zrender.h"

#include "gamez/zMath/zmath_vu.h"

CMatrix CStack::m_stack[64];
CMatrix* CStack::m_pointer = CStack::m_stack;
CMatrix* CStack::m_top = CStack::m_stack;

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