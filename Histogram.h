#ifndef _HISTOGRAM_H_
#define _HISTOGRAM_H_

#include <Math.h>

#define HIST_SIZE 6

class Histogram
{
private:
	unsigned char *src;

	int CalcAngleRange12(double dx, double dy)
	{
		double inner1,inner2;
		if(dx>0)
		{
			inner1 = dx*0.7071 + dy*0.7071;
			if(dx>dy)
			{
				if(dx>inner1)
				{
					inner2 = dx*0.9659 + dy*0.2588;
					if(dx>inner2)
						return 0;
					else
						return 1;
				}
				else
				{
					inner2 = dx*0.866 + dy*0.5;
					if(inner2>inner1)
						return 2;
					else
						return 3;
				}
			}
			else
			{
				if(inner1>dy)
				{
					inner2 = dx*0.5 + dy*0.866;
					if(inner1>inner2)
						return 3;
					else
						return 4;
				}
				else
				{
					inner2 = dx*0.2588 + dy*0.9659;
					if(inner2>dy)
						return 5;
					else
						return 6;
				}
			}
		}
		else
		{
			inner1 = -dx*0.7071 + dy*0.7071;
			if(dy>-dx)
			{
				if(dy>inner1)
				{
					inner2 = -dx*0.2588 + dy*0.9659;
					if(dy>inner2)
						return 6;
					else
						return 7;
				}
				else
				{
					inner2 = -dx*0.5 + dy*0.866;
					if(inner2>inner1)
						return 8;
					else
						return 9;
				}
			}
			else
			{
				if(inner1>-dx)
				{
					inner2 = -dx*0.866 + dy*0.5;
					if(inner1>inner2)
						return 9;
					else
						return 10;
				}
				else
				{
					inner2 = -dx*0.9659 + dy*0.2588;
					if(inner2>-dx)
						return 11;
					else
						return 0;
				}
			}
		}
	}

	int CalcAngleRange6(double dx, double dy)
	{
		double inner1;
		if(dx>0)
		{
			if(dx>dy)
			{
				inner1 = dx*0.866 + dy*0.5;
				if(dx>inner1)
					return 0;
				else
					return 1;
			}
			else
			{
				inner1 = dx*0.5 + dy*0.866;
				if(inner1>dy)
					return 2;
				else
					return 3;
			}
		}
		else
		{
			if(dy>-dx)
			{
				inner1 = -dx*0.5 + dy*0.866;
				if(dy>inner1)
					return 3;
				else
					return 4;
			}
			else
			{
				inner1 = -dx*0.866 + dy*0.5;
				if(inner1>-dx)
					return 5;
				else
					return 0;
			}
		}
	}

	int CalcAngleRange4(double dx, double dy)
	{
		double inner1;
		if(dx>0)
		{
			inner1 = dx*0.7071 + dy*0.7071;
			if(dx>dy)
			{
				if(dx>inner1)
					return 0;
				else
					return 1;
			}
			else
			{
				if(inner1>dy)
					return 1;
				else
					return 2;
			}
		}
		else
		{
			inner1 = -dx*0.7071 + dy*0.7071;
			if(dy>-dx)
			{
				if(dy>inner1)
					return 2;
				else
					return 3;
			}
			else
			{
				if(inner1>-dx)
					return 3;
				else
					return 0;
			}
		}
	}

public:
	double result[4][4][HIST_SIZE];

	Histogram(unsigned char* src):src(src){}

	void Make()
	{
		double hist[11][11][HIST_SIZE];

		int width = 28;
		int height = 28;

		double *mag = new double[width*height];
		int *angle = new int[width*height];

		double dx=0.0,dy=0.0;
		for(int y=0; y<height; ++y)
			for(int x=0; x<width; ++x)
			{
				if(x==0 || x==width || y==0 || y==height)
				{
					mag[x+y*width] = 0.0;
					angle[x+y*width] = 0;
				}
				else
				{
					dx = (double)(src[x+1+y*width] - src[x-1+y*width]);
					dy = (double)(src[x+(y-1)*width] - src[x+(y+1)*width]);
					mag[x+y*width] = sqrt(dx*dx+dy*dy);

					if(dy<0.0)
					{
						dx = -dx;
						dy = -dy;
					}

					if( abs(dx)<1.0 && abs(dy)<1.0 )
						angle[x+y*width] = -1;
					else
						angle[x+y*width] = CalcAngleRange6(dx,dy);
				}
			}
		// Initialize
		for(int j=0; j<11; ++j)
			for(int i=0; i<11; ++i)
				for(int k=0; k<HIST_SIZE; ++k)
					hist[i][j][k] = 0.0;

		// Make Histogram
		for(int y=2; y<9; ++y)
			for(int x=2; x<9; ++x)
				for(int j=(y-2)*4; j<(y-1)*4; ++j)
					for(int i=(x-2)*4; i<(x-1)*4; ++i)
						if( angle[i+j*width]!=-1 )
							hist[x][y][angle[i+j*width]] += mag[i+j*width];

		double total = 0.0;
		for(int y=2; y<9; ++y)
			for(int x=2; x<9; ++x)
				for(int k=0; k<HIST_SIZE; ++k)
					total += hist[x][y][k];
		total /= 100;
		for(int y=2; y<9; ++y)
			for(int x=2; x<9; ++x)
				for(int k=0; k<HIST_SIZE; ++k)
					hist[x][y][k] /= total;

		// normalize
		for(int j=0; j<4; ++j)
			for(int i=0; i<4; ++i)
			{
				int x = (i+1)*2;
				int y = (j+1)*2;
				for(int k=0; k<HIST_SIZE; ++k)
				{
					double val = 0.0;
					val += hist[x][y][k] * 0.2;
					val += (hist[x-1][y][k]+hist[x+1][y][k]+hist[x][y-1][k]+hist[x][y+1][k]) * 0.1;
					val += (hist[x-1][y-1][k]+hist[x-1][y+1][k]+hist[x+1][y-1][k]+hist[x+1][y+1][k]) * 0.06;
					val += (hist[x-2][y][k]+hist[x+2][y][k]+hist[x][y-2][k]+hist[x][y+2][k]) * 0.02;
					val += (hist[x-1][y-2][k]+hist[x+1][y-2][k]+hist[x-2][y-1][k]+hist[x+2][y-1][k]) * 0.01;
					val += (hist[x-1][y+2][k]+hist[x+1][y+2][k]+hist[x-2][y+1][k]+hist[x+2][y+1][k]) * 0.01;
					result[i][j][k] = val;
				}
			}

		delete[] mag;
		delete[] angle;
	}
};

#endif