using System;

namespace GraphicsWindow
{
	/// <summary>
	/// Summary description for ColorRGB.
	/// </summary>
	public class ColorRGB
	{
		public byte r,g,b;

		public ColorRGB( byte red,     // default color is black
			byte green , 
			byte  blue )
		{ 
			r = red; 
			g = green; 
			b = blue; 
		}

		public ColorRGB():this(0,0,0)
		{
			
		}


	}
}
