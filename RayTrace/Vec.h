#ifndef _VEC_DOT_H
#define _VEC_DOT_H
#include <math.h>

class Vec
{

public:
	/////////////////////////////////////
	// 
	//
	//
	double x , y, z;



   ////////////////////////////////////////////
   //  Ctor
   //
   //
   Vec (double x=0.0, double y=0.0, double z=0.0) 
   {
    this->x = x;
    this->y = y;
    this->z = z;
   }
   ////////////////////////////////////////
   //
   //  Get the Length of the vector
   //
   //
   double getLength () 
   {
     return sqrt (x * x + y * y + z * z);
   }

   //////////////////////////////////////////
   //
   //  Scale the vector
   //
   //
   //
   void scale (double scale) 
   {
    x *= scale;
    y *= scale;
    z *= scale;
   }

   //////////////////////////////////////////
   //  Calculate the dot product
   //
   //
   //
   //
   double dot (const Vec& v) 
   {
      return x * v.x + y * v.y + z * v.z;
   
   }



};



#endif
