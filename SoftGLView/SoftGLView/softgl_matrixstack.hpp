//============================================================================
// softgl_matrixstack.hpp
//============================================================================

#ifndef _SOFTGL_MATRIXSTACK_
#define _SOFTGL_MATRIXSTACK_

#include "mat16fv.hpp"

class MatrixStack
{
  private:

    Matrix16fv *Matrices;
    Matrix16fv *invMatrices;
    int NumMatrices, NumMatricesAlloced;
  
  public:

    MatrixStack(int MaxStackDepth);
    ~MatrixStack();    
    void Push();
    void Pop();
    float* Top();
    float* InverseTop();
    void PrintTop();
    void PrintInverseTop();

    void LoadIdentity();
    void Rotate(float DegAng, float AxisX, float AxisY, float AxisZ);
    void Translate(float TransX, float TransY, float TransZ);
    void Scale(float ScaleX, float ScaleY, float ScaleZ);
    void LookAt(float eyex, float eyey, float eyez,
                float lookatptx, float lookatpty, float lookatptz,
                float upx, float upy, float upz);
    void Frustum(float l, float r, float b, float t, float n, float f);
    void Perspective(float Yfov, float Aspect, float Ndist, float Fdist);
    void Ortho(float l, float r, float b, float t, float n, float f);
};

#endif
