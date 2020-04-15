//============================================================================
// mat16fv.hpp : opengl-style float[16] matrix routines.
//============================================================================

typedef float Matrix16fv[16];

float* Copy16fv(float* A, float* B); // A=B
float* Mult16fv(float* C, float* A, float* B); // C=A*B

float* Mult16fv3fv(float *NewV, float* M, float *V); // NewV = M * [Vx,Vy,Vz,0]
float* Mult16fv3fvPerspDiv(float *NewV, float* M, float *V); // NewV = M * [Vx,Vy,Vz,1]
float* Mult16fv4fv(float *NewV, float* M, float *V); // NewV = M * [Vx,Vy,Vz,Vw]

float* Identity16fv(float* M);
float* Transpose16fv(float* M);

float* Rotate16fv(float *M, float DegAng, float Axis[3]);
float* invRotate16fv(float *M, float DegAng, float Axis[3]);

float* Scale16fv(float* M, float sx, float sy, float sz);
float* invScale16fv(float* M, float sx, float sy, float sz);

float* Translate16fv(float* M, float tx, float ty, float tz);
float* invTranslate16fv(float* M, float tx, float ty, float tz);

float* LookAt16fv(float* M, float Eye[3], float LookAtPt[3], float ViewUp[3]);
float* invLookAt16fv(float* M, float Eye[3], float LookAtPt[3], float ViewUp[3]);

float* Frustum16fv(float* M, float l, float r, float b, float t, float n, float f);
float* invFrustum16fv(float* M, float l, float r, float b, float t, float n, float f);

float* Perspective16fv(float* M, float Yfov, float Aspect, float Ndist, float Fdist);
float* invPerspective16fv(float* M, float Yfov, float Aspect, float Ndist, float Fdist);

float *Ortho16fv(float *M, float l, float r, float b, float t, float n, float f);
float *invOrtho16fv(float *M, float l, float r, float b, float t, float n, float f);

float* Viewing16fv(float* M, float X[3], float Y[3], float Z[3], float O[3]);
float* invViewing16fv(float* M, float X[3], float Y[3], float Z[3], float O[3]);

float* Viewport16fv(float* M, int WW, int WH);
float* invViewport16fv(float* M, int WW, int WH);

float* PlanarReflection16fv(float M[16], float P[4]);

float XformCoordFrame16fv(float *M, float X[3], float Y[3], float Z[3], float O[3]);
float* Obj2WorldXform16fv(float *M, float X[3], float Y[3], float Z[3], float O[3], float Scale);
float* World2ObjXform16fv(float *M, float X[3], float Y[3], float Z[3], float O[3], float Scale);

float* Screen2WorldXform16fv(float* M, 
                             float X[3], float Y[3], float Z[3], float O[3],
                             float l, float r, float b, float t, float n, float f,
                             int WW, int WH);
float* World2ScreenXform16fv(float* M,
                             float X[3], float Y[3], float Z[3], float O[3],
                             float l, float r, float b, float t, float n, float f,
                             int WW, int WH);

void Print16fv(float* M);
