//============================================================================
// vec3fv.cpp
//============================================================================
#include "stdafx.h"

#include <stdio.h>
#include <math.h>

void Set3fv(float v[3], float x, float y, float z)
{
  v[0]=x;
  v[1]=y;
  v[2]=z;
}

void Copy3fv(float A[3], float B[3]) // A=B
{
  A[0]=B[0];
  A[1]=B[1];
  A[2]=B[2];
}

void ScalarMult3fv(float c[3], float a[3], float s) // c=a*s
{
  c[0] = a[0] * s;
  c[1] = a[1] * s;
  c[2] = a[2] * s;
}

void ScalarDiv3fv(float v[3], float s)
{
  v[0] /= s;
  v[1] /= s;
  v[2] /= s;
}

void Add3fv(float c[3], float a[3], float b[3]) // c = a + b
{
  c[0] = a[0] + b[0];
  c[1] = a[1] + b[1];
  c[2] = a[2] + b[2];
}

void Subtract3fv(float c[3], float a[3], float b[3]) // c = a - b
{
  c[0] = a[0] - b[0];
  c[1] = a[1] - b[1];
  c[2] = a[2] - b[2];
}

void Negate3fv(float a[3], float b[3])  // a = -b
{
  a[0] = -b[0];
  a[1] = -b[1];
  a[2] = -b[2];
}

float Length3fv(float v[3])
{
  return( (float)sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]) );
}

void Normalize3fv(float v[3])
{
  float l = Length3fv(v);
  v[0] /= l;
  v[1] /= l;
  v[2] /= l;
}

float DotProd3fv(float a[3], float b[3])
{
  return( a[0]*b[0] + a[1]*b[1] + a[2]*b[2] );
}

void CrossProd3fv(float* C, float* A, float* B) // C = A X B
{ 
  Set3fv(C, A[1]*B[2]-A[2]*B[1], A[2]*B[0]-A[0]*B[2], A[0]*B[1]-A[1]*B[0]);
}

void UpdateMinMax3fv(float Min[3], float Max[3], float NewVec[3])
{
  if (NewVec[0]<Min[0]) Min[0]=NewVec[0]; else if (NewVec[0]>Max[0]) Max[0]=NewVec[0];
  if (NewVec[1]<Min[1]) Min[1]=NewVec[1]; else if (NewVec[1]>Max[1]) Max[1]=NewVec[1];
  if (NewVec[2]<Min[2]) Min[2]=NewVec[2]; else if (NewVec[2]>Max[2]) Max[2]=NewVec[2];
}

int Equal3fv(float A[3], float B[3])
{
  return( A[0]==B[0] && A[1]==B[1] && A[2]==B[2] );
}

void Print3fv(float v[3])
{
  printf("%g %g %g\n",v[0],v[1],v[2]);
}
