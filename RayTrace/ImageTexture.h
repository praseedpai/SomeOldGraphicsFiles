#ifndef _IMAGE_TEXTURE_DOT_H
#define _IMAGE_TEXTURE_DOT_H

#include "Texture.h"

class ImageTexture: public Texture 
{
  int width;
  int height;
  unsigned long *pixel;


private:
 int LoadTexture( char *FileName )
 {		

         HBITMAP hImage = (HBITMAP) LoadImage(NULL,FileName,IMAGE_BITMAP,0,0,
                 LR_LOADFROMFILE | LR_CREATEDIBSECTION);

               if ( !hImage )
                     return -5;

               DIBSECTION dibSection;
               GetObject(hImage,sizeof(DIBSECTION),&dibSection);
               int iWidth = dibSection.dsBm.bmWidth;
               int iHeight = dibSection.dsBm.bmHeight;
               int iQuantity = dibSection.dsBm.bmWidth*dibSection.dsBm.bmHeight;

			   width = iWidth;
			   height = iHeight;

               if ( dibSection.dsBm.bmBitsPixel != 24 )
                             return -6;

               // Windows BMP stores BGR, need to invert to RGB.
               unsigned char* acSrc = (unsigned char*) dibSection.dsBm.bmBits;
               
			   

			   pixel = new unsigned long [iQuantity];
               width = iWidth;
			   height = iHeight;

               for (int i = 0, i0 = 0, i1 = 1, i2 = 2; i < iQuantity; i++)
			   {
                    pixel[i] = RGB(acSrc[i0],acSrc[i1],acSrc[i2]);
                    i0 += 3;
                    i1 += 3;
                    i2 += 3;
			   }

               DeleteObject(hImage);
               return 1;
		}

public:

 ImageTexture (char *FileName) 
 {
     LoadTexture(FileName);
 }


 pRGB *getTexel (double i, double j) 
 {
    i -= floor (i);
    j -= floor (j);
    return new pRGB (pixel[(int) (i * width) + width* (int) (j * height)]);
 }





};










#endif
