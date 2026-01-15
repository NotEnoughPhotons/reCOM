#include <math.h>

#include "zmath.h"

void CPnt3D::Normalize()
{
	f32 sqrMagnitude = sqrtf(z * z + x * x + y * y);

	if (sqrMagnitude != 0.0f)
	{
		sqrMagnitude = 1.0f / sqrMagnitude;
		x *= sqrMagnitude;
		y *= sqrMagnitude;
		z *= sqrMagnitude;
	}
}

void CPnt3D::Normalize(CPnt3D* other)
{
	f32 sqrMagnitude = sqrtf(z * z + x * x + y * y);

	if (sqrMagnitude != 0.0f)
	{
		sqrMagnitude = 1.0f / sqrMagnitude;
		other->x = x * sqrMagnitude;
		other->y = y * sqrMagnitude;
		other->z = z * sqrMagnitude;
	}
}

void CPnt3D::Add(CPnt3D* other, CPnt3D* out)
{
	out->x = x + other->x;
	out->y = y + other->y;
	out->z = z + other->z;
}

void CPnt3D::Sub(CPnt3D* other, CPnt3D* out)
{
	out->x = x - other->x;
	out->y = y - other->y;
	out->z = z - other->z;
}

void CPnt3D::Scale(f32 scaleFactor, CPnt3D* out)
{
	out->x = x * scaleFactor;
	out->y = y * scaleFactor;
	out->z = z * scaleFactor;
}

void CPnt3D::Cross(const CPnt3D* a, CPnt3D* b, bool normalize)
{

}

void CPnt3D::Interp(const CPnt3D* from, f32 step, CPnt3D* to)
{
	if (step == 0.0f)
	{
		to = this;
	}
	else
	{
		f32 value = 1.0f - step;

		if (step == 1.0f)
		{
			to->x = from->x;
			to->y = from->y;
			to->z = from->z;
		}
		else
		{
			to->x = x * value;
			to->y = y * value;
			to->z = z * value;

			to->x += from->x * step;
			to->y += from->y * step;
			to->z += from->z * step;
		}
	}
}