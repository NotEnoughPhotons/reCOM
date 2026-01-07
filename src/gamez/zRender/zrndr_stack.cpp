#include "zrender.h"

CMatrix* CStack::m_pointer = NULL;
CMatrix* CStack::m_top = NULL;

CMatrix* CStack::Multiply(const CMatrix* matrix, bool other)
{
	CMatrix m = *m_top;
	if (other)
	{
		m_top++;
		m_pointer++;
		// vu0MulMatrix(m_top->m_matrix, m->m_matrix, matrix->m_matrix);
	}
	else
		// vu0MulMatrix(m_top->m_matrix, m_top->m_matrix, matrix->m_matrix);

	return m_top;
}