using System;
using System.Collections;
using System.Drawing;  
using System.Drawing.Drawing2D; 
using System.Drawing.Imaging ;

namespace GraphicsWindow
{
	/// <summary>
	/// Summary description for CGreyScaleConverter.
	/// </summary>
	public class CGreyScaleConverter
	{
		public CGreyScaleConverter()
		{
			
		}

		public System.Drawing.Bitmap Render(String Filename)
		{
			Bitmap b = new Bitmap(Filename);
			BitmapData bd = b.LockBits(new Rectangle(0, 0, b.Width, b.Height), 
				ImageLockMode.ReadWrite, PixelFormat.Format24bppRgb); 

			int size = b.Width*b.Height;
 		

			System.IntPtr  tarr=bd.Scan0; 

			unsafe 
			{

				
				byte *marrayPtr = (byte *)tarr.ToPointer(); 
				
			
				for( int  index = 0; index < size; index++ )
				{
					byte *OldPtr = marrayPtr;
					int rc = *marrayPtr++;
					int gc = *marrayPtr++;
					int bc = *marrayPtr++;
					int grey = (int)(rc*0.33 + gc*0.59 + bc*0.11);
					grey=grey%255;
					*OldPtr++ =(byte)grey;
					*OldPtr++ =(byte)grey;
					*OldPtr++=(byte)grey;
				}

			}
			b.UnlockBits( bd);
			return b; 



		}

	}
}
