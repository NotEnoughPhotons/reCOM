#ifndef ZMATH_VU_H
#define ZMATH_VU_H
#include "zmath.h"
#include "gamez/zSystem/zsys.h"

/// -----------------------------
/// PURPOSE:
/// This file provides functions for wrapping PS2 vector unit calls so that they're "platform agnostic".
/// For example, if we were compiling this project for the PS2, then PS2-related operations will be selected.
/// Likewise, if compiling for an x86 target, vector code compatible with the PC will be selected.
/// 
/// NOTICE:
/// This file does not contain anything from Sony's PS2 development kit.
/// Code is sourced from the unofficial PS2SDK repository:
/// https://github.com/ps2dev/ps2sdk
/// -----------------------------

extern void Vu0MatrixInverseWithUniformScale(CMatrix* m0, CMatrix* m1);

extern void zMathCopyVector(PNT3D& out, PNT3D& in);
extern void zMathAddVector(PNT3D& sum, PNT3D& addend, PNT3D& summand);
extern void zMathSubVector(PNT3D& diff, PNT3D& subend, PNT3D& diffand);
extern void zMathScaleVector();
extern void zMathFTOI0Vector();
extern void zMathFTOI4Vector();
extern void zMathInnerVector();

extern void zMathInnerProduct();
extern void zMathOuterProduct();
extern void zMathNormalize();

extern void zMathRotTransPers();

extern void zMathClipBBox();

extern void zMathCopyMatrix(CMatrix* dest, const CMatrix* src);
extern void zMathApplyMatrix();
extern void zMathUnitMatrix(CMatrix& mat);
extern void zMathMulMatrix(CMatrix* m0, CMatrix* m1, CMatrix* out);
extern void zMathCameraMatrix();
extern void zMathViewScreenMatrix();
extern void zMathTransMatrix();
extern void zMathTransposeMatrix();
extern void zMathInverseMatrix();
extern void zMathRotMatrixX();
extern void zMathRotMatrixY();
extern void zMathRotMatrixZ();
extern void zMathLightColorMatrix();
extern void zMathNormalLightMatrix();
#endif