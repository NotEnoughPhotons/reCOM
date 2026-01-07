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

static void zMathCopyVector(PNT3D& out, PNT3D& in);
static void zMathAddVector(PNT3D& sum, PNT3D& addend, PNT3D& summand);
static void zMathSubVector(PNT3D& diff, PNT3D& subend, PNT3D& diffand);
static void zMathScaleVector();
static void zMathFTOI0Vector();
static void zMathFTOI4Vector();
static void zMathInnerVector();

static void zMathInnerProduct();
static void zMathOuterProduct();
static void zMathNormalize();

static void zMathRotTransPers();

static void zMathClipBBox();

static void zMathCopyMatrix();
static void zMathApplyMatrix();
static void zMathUnitMatrix(CMatrix& mat);
static void zMathMulMatrix();
static void zMathCameraMatrix();
static void zMathViewScreenMatrix();
static void zMathTransMatrix();
static void zMathTransposeMatrix();
static void zMathInverseMatrix();
static void zMathRotMatrixX();
static void zMathRotMatrixY();
static void zMathRotMatrixZ();
static void zMathLightColorMatrix();
static void zMathNormalLightMatrix();
#endif