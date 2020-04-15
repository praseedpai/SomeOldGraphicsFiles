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
	public class CLineChartGDI
	{
		int _width;
		int _height; 
        float xscale;
 		float yscale;

		
		double _xmax_value; 
		double[] _xaxis;
		double[] _yaxis;
		int _number_of_data;

		PointF[] df;

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


		public void SetData( PointF[] f )
		{
			int length = f.Length;
			_xmax_value = (float) -99999999;
			df = new PointF[length];
   
			for( int i=0;i<length;++i )
			{
				df[i] = new PointF(f[i].X,f[i].Y);  
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


		public CLineChartGDI()
		{
			

		}

		public void DrawAxis(Graphics graph)
		{
			Pen pen = new Pen(Color.Red);
            
			//
			// Vertical Axis

			graph.DrawLine(pen,(float)0,(float)0,(float)0,(float)75.0*yscale); 

			//
			//
			//
			double  mxx = ComputeMaxValue();
			graph.DrawLine(pen,(float)0,(float) 0,(float)mxx*xscale,(float)0);

		}


		public void DrawLegend(Graphics g)
		{
			Pen pen = new Pen(Color.Green);
			Font fl = new Font("Verdana",9,System.Drawing.FontStyle.Regular);
			SolidBrush brs = new SolidBrush(Color.Black);
			Matrix ms = new Matrix();
			
			String str = "OVERS BOWLED";
			Matrix m = g.Transform;
			PointF[] fr = new PointF[1]; 
			fr[0] = new PointF(-65*xscale,25*yscale);
			g.TransformPoints(CoordinateSpace.Device ,CoordinateSpace.World , fr );
			g.Transform = ms;
			StringFormat fmt = new StringFormat();
			fmt.FormatFlags = StringFormatFlags.DirectionVertical;
			g.DrawString(str,fl,brs,fr[0].X ,fr[0].Y,fmt); 
			g.Transform = m; 

		}


		public void DrawAxisText( Graphics g )
		{
			Pen pen = new Pen(Color.Green);
			Font fl = new Font("Verdana",9,System.Drawing.FontStyle.Regular);
			SolidBrush brs = new SolidBrush(Color.Black);
				
			Matrix mt = new Matrix();  
			            
			for(int i=5; i<=75; i+=5 )
			{
				String str = Convert.ToString(i);
				Matrix m = g.Transform;
				PointF[] fr = new PointF[1]; 
				fr[0] = new PointF(-35*xscale,i*yscale);
				g.TransformPoints(CoordinateSpace.Device ,CoordinateSpace.World , fr );
				g.Transform = mt;
				g.DrawString(str,fl,brs,fr[0].X ,fr[0].Y); 
				g.Transform = m; 
				g.DrawLine(pen,-5*xscale,i*yscale,5*xscale,i*yscale);   
				
			}


			int mx = (int) ComputeMaxValue();
	
			int addendum = mx/10;
              
			for(int i=0; i<mx; i+=addendum )
			{
				String str = Convert.ToString(i);
				Matrix m = g.Transform;
				PointF[] fr = new PointF[1]; 
				fr[0] = new PointF(i*xscale,-2*yscale);
				g.TransformPoints(CoordinateSpace.Device ,CoordinateSpace.World , fr );
				g.Transform = mt;
				g.DrawString(str,fl,brs,fr[0].X ,fr[0].Y );
 
			

				g.Transform = m; 
				if ( i != 0 )
					g.DrawLine(pen,i*xscale,5,i*xscale,0); 
			}
     

		}

		public void DrawCircles(Graphics g)
		{
			int len = df.Length;
 
			Pen pens  = new Pen(Color.Navy);
            
			SolidBrush  ns = new SolidBrush(Color.Chocolate);  				
			for( int i=0;i<len;++i )
			{
			   	
				if((df[i].X != 0) && (df[i].Y != 0))
				     g.DrawEllipse( pens,df[i].X*xscale ,df[i].Y*yscale,5,5);
				
					
			}

            


		}

		public void PlotData( Graphics g )
		{
			Pen pen = new Pen(Color.Brown);


			g.DrawLine(pen,(float)_xaxis[0]*xscale,(float)_yaxis[0]*yscale,(float)_xaxis[0]*xscale,(float)_yaxis[0]*yscale);

			float oldx  = (float) _xaxis[0]*xscale;
			float oldy  = (float) _yaxis[0]*yscale;

			for( int i=1;i<180;++i)
			{

				g.DrawLine(pen,oldx,oldy,(float)_xaxis[i]*xscale ,(float)_yaxis[i]*yscale);
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

			
			graph.TranslateTransform(60,(_height-40));
            graph.ScaleTransform(1,-1);   
			    
				
            
                

				

				
			DrawAxis(graph);
			DrawAxisText(graph);
			DrawLegend(graph);
			DrawCircles(graph);
			PlotData(graph);


			return bmp;
 
		}

	}
	
	

}
