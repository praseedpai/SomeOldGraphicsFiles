using System;
using System.Drawing; 
using System.Drawing.Imaging;  

namespace GraphicsWindow
{
	/// <summary>
	/// Summary description for FrameBuff.
	/// </summary>
	public class FrameBuff
	{
		public ColorRGB[]  arrayPtr;
		public int numRows;
		public int numCols;
		
		public int size;


		public FrameBuff( int  r, int  c )
		{
			numRows = r;
			numCols = c;
			size = r*c;
		
			arrayPtr = new ColorRGB [size];  
			for( int  index = 0; index < size; index++ )
				arrayPtr[index] = new ColorRGB(255,0,0);
				
			  
           

		}
		
		public void clear()
		{
			ColorRGB color = new ColorRGB(255,0,0);  
			for( int  index = 0; index < size; index++ )
			{
				arrayPtr[index].r = color.r;
				arrayPtr[index].g = color.g;
				arrayPtr[index].b = color.b;
			}
		

		}

		public int Draw( System.Drawing.Graphics g )
		{
			int r = numRows;
			int c = numCols;

			Bitmap bmp = new Bitmap(  r, c,System.Drawing.Imaging.PixelFormat.Format24bppRgb);

			Rectangle s = new Rectangle(0,0,r,c);
			BitmapData bd;
			bd = bmp.LockBits(s,System.Drawing.Imaging.ImageLockMode.ReadWrite,System.Drawing.Imaging.PixelFormat.Format24bppRgb); 

			ColorRGB color = new ColorRGB();  
			System.IntPtr  tarr=bd.Scan0; 

			unsafe 
			{

				
				byte *marrayPtr = (byte *)tarr.ToPointer(); 
				
			
				for( int  index = 0; index < size; index++ )
				{
					*marrayPtr++ = arrayPtr[index].b;
					*marrayPtr++ = arrayPtr[index].g;
					*marrayPtr++ = arrayPtr[index].r;
				}

			}

			bmp.UnlockBits( bd);

			g.DrawImage(bmp,0,0,r, c); 


			return 1;


		}

		public int Fill(System.Drawing.Color col )
		{

		
			
			for( int  index = 0; index < size; index++ )
			{
				arrayPtr[index].b  = col.B;
				arrayPtr[index].g = col.G;
				arrayPtr[index].r = col.R;
			}

			
			
		
			return 1;  

		}


		

		public void FillTriangleFlat()
		{






		}


		public ColorRGB this[int r,int c ]
		{
			get 
			{
				int rindex = r*numCols + c;

				if ( rindex >=0 && rindex < size )
					return arrayPtr[rindex];
				else 
					return new ColorRGB(); 

			}

			set 
			{
				int rindex = r*numCols + c;

				if ( rindex >=0 && rindex < size )
					arrayPtr[rindex] = value;
				 


			}

		}



	}
}