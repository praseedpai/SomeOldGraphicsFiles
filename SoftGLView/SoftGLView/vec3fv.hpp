//============================================================================
// vec3fv.hpp
//============================================================================

void Set3fv(float v[3], float x, float y, float z);
void Copy3fv(float A[3], float B[3]); // A=B

void ScalarMult3fv(float c[3], float a[3], float s);
void ScalarDiv3fv(float v[3], float s);
void Add3fv(float c[3], float a[3], float b[3]); // c = a + b
void Subtract3fv(float c[3], float a[3], float b[3]); // c = a - b
void Negate3fv(float a[3], float b[3]);  // a = -b

float Length3fv(float v[3]);
void Normalize3fv(float v[3]);
float DotProd3fv(float a[3], float b[3]);
void CrossProd3fv(float* C, float* A, float* B); // C = A X B

void UpdateMinMax3fv(float Min[3], float Max[3], float NewVec[3]);
int Equal3fv(float A[3], float B[3]);

void Print3fv(float v[3]);
