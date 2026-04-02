
#ifndef _ORTHOLIN__H_
	#define _ORTHOLIN__H_

	//private:
	static double RowProduct (double Ra[], double Rb[], long RowSize);
	static double ColumnProduct (double Ma[], double Rb[], long ColumnSize, long RowSize);

	//public:
	extern bool Ortholin2 (double A[], long Nrows, long Ncols, double B[], double EPS, double X[], long MaxIterations);
#endif
