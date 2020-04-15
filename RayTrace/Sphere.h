#ifndef _SPHERE_DOT_H
#define _SPHERE_DOT_H

#include "Light.h"
#include "Texture.h"
#include  "obj.h"
class Sphere :public Obj 
{
   
private:

  Texture *texture;
  Light *light;
  double sphereZ, sphereR;
  double c;
  
public:
  //////////////////////////////////////////
  //
  //   Ctor
  //
  //
  Sphere (double sphereZ, double sphereR, Texture *texture, Light *light) {
    this->sphereZ = sphereZ;
    this->sphereR = sphereR;
    this->texture = texture;
    this->light = light;
    c = sphereZ * sphereZ - sphereR * sphereR;
  }

  //////////////////////////////////////////////////////
  //
  //
  //
  //
  //
  //

  pRGB *getIntersection (const Vec& ray) 
  {
    double a = ray.x * ray.x + ray.y * ray.y + ray.z * ray.z;

    double b = -2.0 * sphereZ * ray.z;

    double tmp = b * b - 4.0 * a * c;

    if (tmp < 0) 
	{
      return new pRGB (1.0, 1.0, 1.0);
    } 
	else 
	{
      double t = (- b - sqrt (tmp)) / 2.0 / a;
      Loc *intersect = new Loc (ray.x * t, ray.y * t, ray.z * t);
      double j = asin (intersect->y / sphereR);
      double i = acos (intersect->x / sphereR / cos (j));
      pRGB *rgb = texture->getTexel (i / 3.14159, j / 3.14159);

      Vec *norm = new Vec (intersect->x, intersect->y, intersect->z - sphereZ);
      double length = norm->getLength ();
      norm->scale (1.0 / length);
      Vec *dir = light->getDirection (*intersect);
      double coincidence = - norm->dot (*dir);
      double lighting = 0.25 + 0.75 * ((coincidence >= 0) ? coincidence : 0.0);
      rgb->scale (lighting);
	  
	  /////////////////////////////////////
	  //
	  //  Free resources
	  //
	  //
	 
	  delete norm;
      delete intersect;
	  
      return rgb;
    }
  }
};





#endif
