using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;

namespace GraphicsWindow
{
	/// <summary>
	/// Summary description for Form1.
	/// </summary>
	public class Form1 : System.Windows.Forms.Form
	{
		private System.Windows.Forms.MainMenu mainMenu1;
		private System.Windows.Forms.MenuItem menuItem1;
		private System.Windows.Forms.MenuItem menuItem2;
		private System.Windows.Forms.MenuItem menuItem3;
		private System.Windows.Forms.MenuItem menuItem4;
		private System.Windows.Forms.MenuItem menuItem5;
		private System.Windows.Forms.PictureBox PctGraph;
		private System.Windows.Forms.MenuItem menuItem6;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public Form1()
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

			//
			// TODO: Add any constructor code after InitializeComponent call
			//
		}

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if (components != null) 
				{
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		}

		#region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.PctGraph = new System.Windows.Forms.PictureBox();
			this.mainMenu1 = new System.Windows.Forms.MainMenu();
			this.menuItem1 = new System.Windows.Forms.MenuItem();
			this.menuItem2 = new System.Windows.Forms.MenuItem();
			this.menuItem3 = new System.Windows.Forms.MenuItem();
			this.menuItem4 = new System.Windows.Forms.MenuItem();
			this.menuItem5 = new System.Windows.Forms.MenuItem();
			this.menuItem6 = new System.Windows.Forms.MenuItem();
			this.SuspendLayout();
			// 
			// PctGraph
			// 
			this.PctGraph.Name = "PctGraph";
			this.PctGraph.Size = new System.Drawing.Size(512, 512);
			this.PctGraph.TabIndex = 0;
			this.PctGraph.TabStop = false;
			this.PctGraph.Click += new System.EventHandler(this.PctGraph_Click);
			// 
			// mainMenu1
			// 
			this.mainMenu1.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
																					  this.menuItem1});
			// 
			// menuItem1
			// 
			this.menuItem1.Index = 0;
			this.menuItem1.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
																					  this.menuItem2,
																					  this.menuItem3,
																					  this.menuItem4,
																					  this.menuItem5,
																					  this.menuItem6});
			this.menuItem1.Text = "Demos";
			// 
			// menuItem2
			// 
			this.menuItem2.Index = 0;
			this.menuItem2.Text = "Clear Canvas";
			this.menuItem2.Click += new System.EventHandler(this.menuItem2_Click);
			// 
			// menuItem3
			// 
			this.menuItem3.Index = 1;
			this.menuItem3.Text = "Line Chart (Generic)";
			this.menuItem3.Click += new System.EventHandler(this.menuItem3_Click);
			// 
			// menuItem4
			// 
			this.menuItem4.Index = 2;
			this.menuItem4.Text = "Line Chart (GDI+)";
			this.menuItem4.Click += new System.EventHandler(this.menuItem4_Click);
			// 
			// menuItem5
			// 
			this.menuItem5.Index = 3;
			this.menuItem5.Text = "Analog Clock";
			this.menuItem5.Click += new System.EventHandler(this.menuItem5_Click);
			// 
			// menuItem6
			// 
			this.menuItem6.Index = 4;
			this.menuItem6.Text = "Color To GreyScale";
			this.menuItem6.Click += new System.EventHandler(this.menuItem6_Click);
			// 
			// Form1
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(504, 457);
			this.Controls.AddRange(new System.Windows.Forms.Control[] {
																		  this.PctGraph});
			this.Menu = this.mainMenu1;
			this.Name = "Form1";
			this.Text = "Form1";
			this.ResumeLayout(false);

		}
		#endregion

		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		[STAThread]
		static void Main() 
		{
			Application.Run(new Form1());
		}

		private void menuItem2_Click(object sender, System.EventArgs e)
		{
			FrameBuff s = new FrameBuff(PctGraph.Width,PctGraph.Height);
			s.Fill( Color.Red ); 
			s.Draw(PctGraph.CreateGraphics());  

		}

		private void menuItem3_Click(object sender, System.EventArgs e)
		{
			CLineChart chrt = new CLineChart();
			chrt.Width = PctGraph.Width;
			chrt.Height = PctGraph.Height;
 
			Point[] x = new Point[4];
			x[0] = new Point((int)8.8,(int)781.132);

			//x[0] = new PointF(0,0);
			x[1] = new Point((int)100.00,(int)34.0);
			x[2] = new Point((int)110.12,(int)45.36);
			x[3] = new Point((int)220,(int)43.33333);
			chrt.SetData(x);
 
			PctGraph.Image = chrt.Render();

		}

		private void menuItem4_Click(object sender, System.EventArgs e)
		{

			CLineChartGDI chrt = new CLineChartGDI();
			chrt.Width = PctGraph.Width;
			chrt.Height = PctGraph.Height;
 
			PointF[] x = new PointF[4];
			x[0] = new PointF((float)8.8,(float)781.132);

			
			x[1] = new PointF((float)100.00,(float)34.0);
			x[2] = new PointF((float)110.12,(float)45.36);
			x[3] = new PointF((float)220,(float)43.33333);
			chrt.SetData(x);
 
			PctGraph.Image = chrt.Render();

		
		}

		private void menuItem5_Click(object sender, System.EventArgs e)
		{
			CAnalogClock clk = new CAnalogClock();
			clk.Width = PctGraph.Width;
			clk.Height = PctGraph.Height;
			PctGraph.Image = clk.Render();

		}

		private void PctGraph_Click(object sender, System.EventArgs e)
		{

			

		
		}

		private void menuItem6_Click(object sender, System.EventArgs e)
		{
			CGreyScaleConverter conv = new CGreyScaleConverter();
            PctGraph.Image = conv.Render("juliasmall.jpg");

		
		}
	}
}
