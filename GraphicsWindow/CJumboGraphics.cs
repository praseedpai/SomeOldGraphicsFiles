using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Drawing.Imaging;

namespace GraphicsWindow
{
	
     
	
	/// <summary>
	/// Summary description for CJumboGraphics.
	/// </summary>
	public class CJumboGraphics
	{

		System.Drawing.Graphics  gr;
		float _org_x;
		float _org_y;
		float _x_scale;
		float _y_scale;

		public void SetViewPortOrig( float x , float y )
		{
			_org_x = x;
			_org_y = y;
		}


		public CJumboGraphics(Graphics lgr)
		{
			gr = lgr;	
			_x_scale = 1;
			_y_scale = -1;

		

		}

		private Point LPtoDP( Point r )
		{

			Point s= new Point(0,0) ;
			s.X =  (int) (_org_x + r.X*_x_scale); 
			s.Y =   (int)(_org_y + r.Y*_y_scale); 
			return s;  

		}


		////////////////////////////////////////////////
		///
		//
		//
		//
		public void DrawLine( Pen pen , int x , int y , int x1, int y1 )
		{
			Point first  = this.LPtoDP(new Point(x,y));
			Point second = this.LPtoDP(new Point(x1,y1)); 
			gr.DrawLine(pen,first.X ,first.Y ,second.X ,second.Y); 
		}

		////////////////////////////////////////////////////////
		//
		//
		//

		public void FillEllipse(Brush br , int x , int y , int cx , int cy)
		{
			Point first  = this.LPtoDP(new Point(x,y));
			Point second = this.LPtoDP(new Point(x+cx,y+cy)); 
			gr.FillEllipse(br,first.X,first.Y,second.X-first.X,second.Y - first.Y); 

		}

		/////////////////////////////////////////////////////
		///
		//
		//
		//
		//
		public void DrawString( String str,Font fl , Brush br,int x1, int y1)
		{
			Point first  = this.LPtoDP(new Point(x1,y1));
			gr.DrawString(str,fl,br,first.X,first.Y);   
		}

		
	     	
	}
	

}
