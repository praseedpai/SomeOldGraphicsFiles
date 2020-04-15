//============================================================================
// softgl_constants.hpp
//============================================================================

#ifndef _SOFTGL_CONSTANTS_
#define _SOFTGL_CONSTANTS_

#define TRUE 1
#define FALSE 0

#define UNSIGNED_BYTE 0           // PIXEL/TEXEL TYPES

#define _RGB 3                    // PIXEL/TEXEL FORMATS

#define MODELVIEW_MATRIXSTACK_DEPTH 32
#define PROJECTION_MATRIXSTACK_DEPTH 3
#define TEXTURE_MATRIXSTACK_DEPTH 8

#define MODELVIEW 1               // Matrixmode
#define PROJECTION 2
#define TEXTURE 3

#define TRIANGLES 1               // Begin
#define POLYGON 2
#define TRIANGLE_FAN 3

#define COLOR_BUFFER_BIT 1        // Clear: MUST BE POWERS OF 2
#define DEPTH_BUFFER_BIT 2

#define SMOOTH 1                  // ShadeModel
#define FLAT 2

#define LIGHT0 0                  // Enable/Disable STATE
#define LIGHT1 1
#define LIGHT2 2
#define LIGHT3 3
#define LIGHT4 4
#define LIGHT5 5
#define LIGHT6 6
#define LIGHT7 7
#define LIGHTING 10
#define CULL_FACE 11
#define COLOR_MATERIAL 12
#define NORMALIZE 13
#define TEXTURE_2D 14

#define AMBIENT 0                  // Lightf, Lightfv
#define DIFFUSE 1
#define SPECULAR 2
#define POSITION 3
#define SPOT_DIRECTION 4
#define SPOT_EXPONENT 5
#define SPOT_CUTOFF 6
#define CONSTANT_ATTENUATION 7
#define LINEAR_ATTENUATION 8
#define QUADRATIC_ATTENUATION 9

#define LIGHT_MODEL_AMBIENT 0      // LightModelfv, LightModeli
#define LIGHT_MODEL_LOCAL_VIEWER 1
#define LIGHT_MODEL_TWO_SIDE 2

#define AMBIENT_AND_DIFFUSE 10     // Material
#define SHININESS 11
#define EMISSION 12

#define FRONT 0                    // ColorMaterial
#define BACK 1
#define FRONT_AND_BACK 3

#define TEXTURE_WRAP_S 0           // TexParameter
#define TEXTURE_WRAP_T 1
#define TEXTURE_MIN_FILTER 2
#define TEXTURE_MAG_FILTER 3
#define CLAMP 0
#define REPEAT 1
#define NEAREST 0
#define LINEAR 1

#define TEXTURE_ENV 0              // TexEnv
#define TEXTURE_ENV_MODE 0
#define REPLACE 0
#define MODULATE 1
#define DECAL 2

#define PERSPECTIVE_CORRECTION_HINT 0  // Hint
#define FASTEST 0
#define NICEST 1

#endif
