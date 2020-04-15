using System;
using System.Collections;
using System.Drawing;  
using System.Drawing.Drawing2D; 
using System.Drawing.Imaging ;


namespace GraphicsWindow
{
	
	

	

	
	/// <summary>
	/// Summary description for CLineChart.
	/// </summary>
	public class CLineChart
	{
		int _width;
		int _height; 
		float xscale;
		float yscale;

		double _xmax_value; 
		double[] _xaxis;
		double[] _yaxis;
		int _number_of_data;

		Point[] df;

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


		public void SetData( Point[] f )
		{
			int length = f.Length;
			_xmax_value = (float) -99999999;
			df = new Point[length];
   
			for( int i=0;i<length;++i )
			{
				df[i] = new Point(f[i].X,f[i].Y);  
				if ( df[i].X >  _xmax_value )
					_xmax_value = df[i].X;

			
			}

           

		}

		double ComputeMaxValue()
		{
			if ( _xmax_value <= 1000 )
				return (double)1000;


			int x =  ((int)( _xmax_value + 100 ))/100;

			return (double)(x*100);

		}


		private void computevalues()
		{
			//_xaxis = new double[180];
			//_yaxis = new double[180];
  
			int nr = 0;
			int nst =(int) ComputeMaxValue();

			_number_of_data = 180;
			_xaxis = new double[180];
			_yaxis = new double[180];

			
			nr = 100;
			for(int i=0;i<_number_of_data;i++ )
			{
				_xaxis[i]= nr;
				_yaxis[i]= 18459.0 * System.Math.Pow((double)nr,-1.2326);
				nr+=(int)(nst/180); 
                           
			}

		}


		public CLineChart()
		{
			

		}

		public void DrawAxis(CJumboGraphics graph)
		{
			Pen pen = new Pen(Color.Red);
            
			//
			// Vertical Axis

			graph.DrawLine( pen,0,0,0,(int)(75.0*yscale)); 

			//
			//
			//
			double  mxx = ComputeMaxValue();
			graph.DrawLine(pen,0, 0,(int)(xscale*mxx),0);

		}

		public void DrawAxisText( CJumboGraphics g )
		{
			Pen pen = new Pen(Color.Green);
			Font fl = new Font("Verdana",9,System.Drawing.FontStyle.Regular);
			SolidBrush brs = new SolidBrush(Color.Black);
				
			//Matrix mt = new Matrix();  
			            
			for(int i=5; i<=75; i+=5 )
			{
				String str = Convert.ToString(i);
				//	Matrix m = g.Transform;
				//	Point[] fr = new Point[1]; 
				//	fr[0] = new Point(-15,(int)(i*yscale));
				//	g.TransformPoints(CoordinateSpace.Device ,CoordinateSpace.World , fr );
				//	g.Transform = mt;
				//	g.DrawString(str,fl,brs,fr[0].X ,fr[0].Y ); 
				g.DrawString(str,fl,brs,-15,(int)(i*yscale)); 

				//	g.Transform = m; 
				g.DrawLine(pen,-5,(int)(i*yscale),5,(int)(i*yscale));   
			}

			int mx = (int) ComputeMaxValue();
	
			int addendum = mx/10;
              
			for(int i=0; i<mx; i+=addendum )
			{
				String str = Convert.ToString(i);
				//	Matrix m = g.Transform;
				//	Point[] fr = new Point[1]; 
				//	fr[0] = new Point((int)(i*xscale),-2);
				//	g.TransformPoints(CoordinateSpace.Device ,CoordinateSpace.World , fr );
				//	g.Transform = mt;
				//g.DrawString(str,fl,brs,fr[0].X ,fr[0].Y );
 
				g.DrawString(str,fl,brs,(int)(i*xscale),-2); 

				//g.Transform = m; 
				if ( i != 0 )
					g.DrawLine(pen,(int)(i*xscale),10,(int)(i*xscale),0); 
			}
             

		}

		public void DrawCircles(CJumboGraphics g)
		{
			int len = df.Length;
 
			Pen pens  = new Pen(Color.Navy);
            
			SolidBrush  ns = new SolidBrush(Color.Chocolate);  				
			for( int i=0;i<len;++i )
			{
			   	 
				//g.DrawEllipse( pens,df[i].X *xscale,df[i].Y*yscale,5,5);
				if((df[i].X != 0) && (df[i].Y != 0))
					g.FillEllipse(ns , (int)(df[i].X *xscale),(int)(df[i].Y*yscale),5,5);
			}

            


		}

		public void PlotData( CJumboGraphics g )
		{
			Pen pen = new Pen(Color.Brown);


			//	g.DrawLine(pen,(float)_xaxis[0]*xscale,(float)_yaxis[0]*yscale,(float)_xaxis[0]*xscale ,(float) _yaxis[0]*yscale);

			float oldx  = (float) _xaxis[0]*xscale;
			float oldy  = (float) _yaxis[0]*yscale;

			for( int i=1;i<180;++i)
			{

				g.DrawLine(pen,(int)oldx,(int)oldy,(int)(_xaxis[i]*xscale) ,(int)(_yaxis[i]*yscale));
				oldx  = (float) _xaxis[i]*xscale;
				oldy  = (float) _yaxis[i]*yscale;
               

			}
  
             

		}

		public System.Drawing.Bitmap Render()
		{
			Bitmap bmp = new Bitmap(_width, _height);
			Graphics graph = Graphics.FromImage(bmp);
				
			Color x = Color.FromArgb(237, 243, 251);
 
			graph.FillRectangle(new SolidBrush(x), 
				-1, -1, bmp.Width + 1, bmp.Height + 1);

			float maxvalue =(float) ComputeMaxValue();  

			computevalues();
             
			yscale = (float) (400.0/75.0);
			xscale =  (float)(500.0/maxvalue);


			//graph.ScaleTransform(1,-1);  
			//graph.TranslateTransform(40,-(_height-20));

			CJumboGraphics cs = new CJumboGraphics(graph);
			cs.SetViewPortOrig(40,_height-40); 
                
				
            
                

				

				
			DrawAxis(cs);
			DrawAxisText(cs);
			DrawCircles(cs);
			PlotData(cs);


			return bmp;
 
		}

	}
	
	

}
