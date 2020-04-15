using System;
using System.Collections;
using System.Drawing;  
using System.Drawing.Drawing2D; 
using System.Drawing.Imaging ;

namespace GraphicsWindow
{
	/// <summary>
	/// Summary description for CAnalogClock.
	/// </summary>
	public class CAnalogClock
	{
       
		int _width;
		int _height; 

		public int Height 
		{
			get { return _height; }   	
			set { _height = (int) value; }
		}

		public int  Width
		{
			get { return _width; }   	
			set { _width = (int)value; }

		}


		public CAnalogClock()
		{
			
		}
        /// <summary>
        /// 
        /// </summary>
        /// <returns></returns>
 		public System.Drawing.Bitmap Render()
		{
			Bitmap bmp = new Bitmap(_width, _height);
			Graphics graph = Graphics.FromImage(bmp);
				
			Color x = Color.FromArgb(237, 243, 251);
 
			graph.FillRectangle(new SolidBrush(x), 
				-1, -1, bmp.Width + 1, bmp.Height + 1);

			
			graph.TranslateTransform(_width/2,_height/2);
			graph.ScaleTransform(1,-1); 
			 
			
			
			graph.SmoothingMode = System.Drawing.Drawing2D.SmoothingMode.AntiAlias;

			Font f = new Font("Arial",12, FontStyle.Bold);   // set antialiasing

			graph.FillEllipse(Brushes.MidnightBlue,                // clock
				-200 ,-200, 400, 400); 

			double i_h   = System.DateTime.Now.Hour;
			double i_min = DateTime.Now.Minute;
			double i_sec = DateTime.Now.Second;

			//Adjust the hour clock

			i_h = i_h + (i_min/60);


			float Second_X = (float)(200*Math.Cos(2*Math.PI*( (double)i_sec/60.0)));
			float Second_Y = (float)(200*Math.Sin(2*Math.PI*( (double)i_sec/60.0)));
			graph.DrawLine(new Pen(Color.Blue,2), 0,0, Second_Y, Second_X);
             
			float Min_X = (float)(0.8* 200 * Math.Cos(2 * Math.PI * (double)i_min/60.0));
			float Min_Y = (float)(0.8* 200 * Math.Sin(2 * Math.PI * (double)i_min/60.0));
			float Hr_X  = (float)(0.5* 200 * Math.Cos(2 * Math.PI * (double)i_h/12.0));
			float Hr_Y  = (float)(0.5* 200 * Math.Sin(2 * Math.PI * (double)i_h/12.0));
			graph.DrawLine(new Pen(Color.Green,3), 0,0, Min_Y, Min_X);
			graph.DrawLine(new Pen(Color.Red,4), 0,0, Hr_Y, Hr_X);
           
			int[] str = new int[12];
            str[0]=3;
            str[1]=2;
			str[2]=1;
			str[3]=12;
            str[4]=11;
			str[5]=10;
			str[6]=9;
			str[7]=8;
			str[8]=7;
            str[9]=6;
            str[10]=5;
			str[11]=4;
			

			Matrix m = graph.Transform; 
			Matrix mt = new Matrix();
			mt.Translate(_width/2,_height/2);
			graph.Transform = mt;
			for(int j=0; j < 12; j++)   // draw numerals
			{
				
				graph.DrawString(""+str[j], 
					                f, 
					                Brushes.Red,
					 (float)(200*Math.Cos(j*Math.PI/6))	,
					 -(float)(200*Math.Sin(j * Math.PI/6)));
				
 
				
				    
				
			}

	graph.Transform = m;



			return bmp;
 
		}

	}
}
