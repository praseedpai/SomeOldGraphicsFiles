//============================================================================
// softgl.hpp
//============================================================================

#ifndef _SOFTGL_
#define _SOFTGL_
#include "stdafx.h"

#include "softgl_constants.hpp"
#include "softgl_matrixstack.hpp"

#define MAX_NUM_LIGHTS 8  // IF YOU CHANGE THIS, YOU MUST UPDATE softgl_constants.hpp

struct VertexParams
{
  float EyeCoords[4], Color[4], Normal[3];  // ALPHA IS IGNORED IN COLOR
  float TexCoords[4];
};

struct LightParams
{
  float Ambient[4], Diffuse[4], Specular[4], Position[4], 
        SpotDirection[3], SpotExponent, SpotCutoff,
        ConstantAttenuation, LinearAttenuation, QuadraticAttenuation;
  int IsEnabled;
};

struct MaterialParams
{
  float Ambient[4], Diffuse[4], Specular[4], Shininess, Emission[4];
};

class Texture
{
 public:

  int WrapS, WrapT, MinFilter, MagFilter;
  float *Texels;  // FLOATING-POINT RGB (in [0,1]) PER TEXEL
  int W, H;       // TEXTURE DIMENSIONS
  Texture();
  ~Texture();
  void StoreRGB(unsigned char *Color, int Width, int Height);
};

class SoftGL
{
  protected:

    unsigned char *ColorBuffer;        // 24-BIT COLOR
    float *DepthBuffer;                // 32-BIT FLOATING-POINT DEPTH
    int PixelWidth, PixelHeight;

    MatrixStack *Modelview;
    MatrixStack *Projection;
    MatrixStack *TextureMat;

    // CURRENT STATE
    MatrixStack *CurrentMatrixStack;   // MatrixMode
    float CurrentColor[3];             // Color
    float CurrentNormal[3];            // Normal
    float CurrentTexCoords[4];         // Tex-coords
    
    int BackfaceCullingEnabled;        // Enable(CULL_FACE)
    int ColorInterpolation;            // ShadeModel: "FLAT" OR "SMOOTH"
    int AutoNormalizationEnabled;      // Enable(NORMALIZE): AUTO NORMALIZATION OF VERTEX NORMALS

    LightParams Lights[MAX_NUM_LIGHTS];
    int LightingEnabled;               // Enable(LIGHTING)
    float LightModelAmbient[4];        // LightModelfv
    int LightModelLocalViewer;         // LightModelf
    int LightModelTwoSide;             // LightModelf

    MaterialParams ColorMaterials[2];  // Materialfv: MATERIAL PROPERTIES FOR FRONT (0) AND BACK (1)
    int ColorMaterialEnabled;          // Enable(COLOR_MATERIAL)
    int ColorMaterialFace;             // ColorMaterial: FRONT, BACK, FRONT_AND_BACK
    int ColorMaterialMode;             // ColorMaterial: AMBIENT, DIFFUSE, AMBIENT_AND_DIFFUSE, SPECULAR, EMISSION
    
    int Texture2dEnabled;              // Enable(TEXTURE_2D);
    int TexEnvMode;                    // TexEnvf(TEXTURE_ENV,*,*);
    Texture* *Textures;                // ARRAY OF TEXTURES ([0] IS RESERVED AS DEFAULT TEXTURE)
    unsigned int NumTextures;          // # OF TEXTURES ALLOCATED IN Textures (ONLY GROWS)
    unsigned int iCurrentBoundTexture; // INDEX INTO Textures INDICATING CURRENT TEXTURE BEING USED
    
    int UsePerspCorrectInterpolation;  // Hint(PERSPECTIVE_CORRECTION_HINT,*);

    // TRIANGLE BUILDING STATE (STORAGE FOR 3 VERTICES AND THEIR ATTRIBUTES)
    int CurrentVert;                   // CURRENT VERTEX BEING ADDED TO BUILD A TRIANGLE
    VertexParams Verts[3];             // HAVE TO STORE ALL INFO NEEDED FOR LIGHTING
    MaterialParams Materials[3][2];    // FOR EACH VERTEX (INCLUDING FRONT AND BACK MATERIALS)

    int PrimitiveBeingBuilt;           // TRIANGLES, POLYGON, TRIANGLE_FAN
    void VertexHandler_TRIANGLES_POLYGON_TRIANGLE_FAN(float V[4]);

    void TransformNormalIntoEyeCoords(float *NewN, float *N);
    int CullBackfacingEyeSpaceTri(float *A, float *B, float *C);
    int HomogeneousViewFrustumCull(float *HomoTriVerts);

    void PerspectiveDivide_ViewportXform(float *ClippedPolyPts, int NumClippedPolyPts);
    void PerspectiveDivide_PerspCorrect_Color(float *V, float *C, int N);
    void PerspectiveDivide_PerspCorrect_Color_Tex(float *V, float *C, float *T, int N);

    void ClipAndDrawCurrentTri_FLAT();
    void ClipAndDrawCurrentTri_SMOOTH();
    void ClipAndDrawCurrentTri_SMOOTH_TEX();

    void SetEnableState(int i, int value);
    unsigned int GetNextTexID();
    
    void CalcLighting(float FinalVertexColor[4], const float p[4], const float n[3], 
                      MaterialParams ColorMaterials[2], const int IsBackfacing);

    void SetDefaultLightingState();
    void SetDefaultMaterialState();
    void SetDefaultTextureState();

  public:

    SoftGL(int pixelwidth, int pixelheight);
    ~SoftGL();

    void Clear(unsigned int WhichBuffers);
    void Begin(int WhichPrimitive);
    void End();

    void ShadeModel(int shade_model) { ColorInterpolation=shade_model; }
    void Enable(int i) { SetEnableState(i,1); }
    void Disable(int i) { SetEnableState(i,0); };

    void Vertex3f(float x, float y, float z);
    void Vertex3fv(float V[3]);
    void Color3f(float r, float g, float b);
    void Color3ub(unsigned char r, unsigned char g, unsigned char b);
    void Color3ubv(unsigned char C[3]);
    void Normal3f(float x, float y, float z);
    void Normal3fv(float N[3]);
    void TexCoord2f(float s, float t);
    void TexCoord2fv(float T[2]);
    void TexCoord3f(float s, float t, float r);
    void TexCoord3fv(float T[3]);

    void Lightf(int LightID, int ParamName, float Value);
    void Lightfv(int LightID, int ParamName, float *Value);
    void LightModelfv(int ParamName, float *Value);
    void LightModeli(int ParamName, int Value);

    void Materialf(int Face, int ParamName, float Value);
    void Materialfv(int Face, int ParamName, float *Value);
    void ColorMaterial(int Face, int Mode);

    void GenTextures(int NumTexIDs, unsigned int *TexIDs);
    void DeleteTextures(int NumTexIDs, unsigned int *TexIDs);
    int IsTexture(unsigned int TexID);
    void BindTexture(int Target, unsigned int TexID);
    void TexParameterf(int Target, int ParamName, int Param);
    void TexImage2D(int Target, int Level, int InternalFormat, int W, int H, 
                    int Border, int Format, int Type, const void *Texels);
    void TexEnvf(int Target, int ParamName, int Param);

    void Hint(int Target, int hint);

    void MatrixMode(int WhichMatrixMode);
    void PushMatrix() { CurrentMatrixStack->Push(); }
    void PopMatrix() { CurrentMatrixStack->Pop(); }

    void LoadIdentity()
      { CurrentMatrixStack->LoadIdentity(); }

    void Rotate(float DegAng, float AxisX, float AxisY, float AxisZ)
      { CurrentMatrixStack->Rotate(DegAng,AxisX,AxisY,AxisZ); }

    void Translate(float TransX, float TransY, float TransZ)
      { CurrentMatrixStack->Translate(TransX,TransY,TransZ); }

    void Scale(float ScaleX, float ScaleY, float ScaleZ)
      { CurrentMatrixStack->Scale(ScaleX,ScaleY,ScaleZ); }

    void LookAt(float eyex, float eyey, float eyez,
                float lookatptx, float lookatpty, float lookatptz,
                float upx, float upy, float upz)
      { CurrentMatrixStack->LookAt(eyex,eyey,eyez,lookatptx,lookatpty,lookatptz,upx,upy,upz); }

    void Frustum(float l, float r, float b, float t, float n, float f)
      { CurrentMatrixStack->Frustum(l,r,b,t,n,f); }

    void Perspective(float Yfov, float Aspect, float Ndist, float Fdist)
      { CurrentMatrixStack->Perspective(Yfov,Aspect,Ndist,Fdist); }

    void Ortho(float l, float r, float b, float t, float n, float f)
      { CurrentMatrixStack->Ortho(l,r,b,t,n,f); }

    unsigned char* GetColorBuffer() { return(ColorBuffer); }
    float* GetDepthBuffer() { return(DepthBuffer); }
    int GetPixelWidth() { return(PixelWidth); }
    int GetPixelHeight() { return(PixelHeight); }

    void DrawToPPM(char *FileName);

	void RenderDevice(const HDC& dc );
};

#endif
