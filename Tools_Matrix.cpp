//+-----------------------------------------------------------------------------+
//| Copyleft (C) 2025 REGEO (http://www.regeo.uji.es/)
//|
//| This program is free software
//| You can redistribute it and/or modify it under the terms of the GNU General Public License,
//| as published by the Free Software Foundation.
//| 
//| This program is distributed in the hope that it will be useful,
//| but WITHOUT ANY WARRANTY; without even the implied warranty of
//| MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//+----------------------------------------------------------------------------*/

#include <stdafx.h>
#include <cmath>
#include <cassert>
#include <limits>

#include <vector>
#include <complex>
#include "Tools_Matrix.h"

#define SIGN(a,b) ((b) >= 0.0 ? fabs(a) : -fabs(a))

static double sqrarg;
#define SQR(a) (sqrarg=(a),sqrarg*sqrarg)

//----------------------------------------------------------------------
// MATRIX MANAGEMENT


std::vector<std::vector<double>> MTranspose(std::vector<std::vector<double>> M)
//Returns the transpose of matrix M
{
	//Check size
	long Mrows= M.size();
	long Mcolumns= M[0].size();
	for(long i=1; i<Mrows; i++){
		assert(Mcolumns == M[i].size());
	}

	std::vector<std::vector<double>> MT;

	for(long i=0; i<Mcolumns; i++){
		MT.push_back(std::vector<double>());
		for(long j=0; j<Mrows; j++){
			MT[i].push_back(M[j][i]);
		}
	}

	return MT;
};
//
//
//std::vector<std::vector<double>> MInvert3x3(std::vector<std::vector<double>> M)
////Returns the inverse of matrix M 3x3
////Only 3x3 square matrices may be inverted by this function
//{
//	long Mrows= M.size();
//	long Mcolumns= M[0].size();
//	for(long i=1; i<Mrows; i++){
//		assert(Mcolumns == M[i].size());
//	}
//	assert(Mrows == 3 && Mcolumns == 3);
//
//	double det= +M[0][0]*(M[1][1]*M[2][2]-M[2][1]*M[1][2])
//				-M[0][1]*(M[1][0]*M[2][2]-M[1][2]*M[2][0])
//				+M[0][2]*(M[1][0]*M[2][1]-M[1][1]*M[2][0]);
//
//	std::vector<std::vector<double>> MI(3, std::vector<double>(3, 0));
//
//	MI[0][0] =  (M[1][1]*M[2][2]-M[2][1]*M[1][2])/det;
//	MI[1][0] = -(M[0][1]*M[2][2]-M[0][2]*M[2][1])/det;
//	MI[2][0] =  (M[0][1]*M[1][2]-M[0][2]*M[1][1])/det;
//	MI[0][1] = -(M[1][0]*M[2][2]-M[1][2]*M[2][0])/det;
//	MI[1][1] =  (M[0][0]*M[2][2]-M[0][2]*M[2][0])/det;
//	MI[2][1] = -(M[0][0]*M[1][2]-M[1][0]*M[0][2])/det;
//	MI[0][2] =  (M[1][0]*M[2][1]-M[2][0]*M[1][1])/det;
//	MI[1][2] = -(M[0][0]*M[2][1]-M[2][0]*M[0][1])/det;
//	MI[2][2] =  (M[0][0]*M[1][1]-M[1][0]*M[0][1])/det;
//
//	return MI;
//};


std::vector<std::vector<double>> MScale(std::vector<std::vector<double>> M, double Scale)
//Returns the scaled matrix M

// All its coefficients are multiplied by the scale factor
{
	std::vector<std::vector<double>> MM;

	for(long i=0; i<(long)M.size(); i++){
		MM.push_back(std::vector<double>());
		for(long j=0; j<(long)M[i].size(); j++){
			MM[i].push_back(M[i][j]*Scale);
		}
	}

	return MM;
};


std::vector<std::vector<double>> MAdd(std::vector<std::vector<double>> M1,
									  std::vector<std::vector<double>> M2)
//Returns the sum of matrices M1 + M2
{
	//Check sizes
	long Mrows1= M1.size();
	long Mcolumns1= M1[0].size();
	for(long i=1; i<Mrows1; i++){
		assert(Mcolumns1 == M1[i].size());
	}

	long Mrows2= M2.size();
	long Mcolumns2= M2[0].size();
	for(long i=1; i<Mrows2; i++){
		assert(Mcolumns2 == M2[i].size());
	}

	assert(Mcolumns1 == Mcolumns2 && Mrows1 == Mrows2);

	std::vector<std::vector<double>> MA;

	for(long i=0; i<Mrows1; i++){
		MA.push_back(std::vector<double>());
		for(long j=0; j<Mcolumns1; j++){
			MA[i].push_back(M1[i][j]+M2[i][j]);
		}
	}

	return MA;
};


std::vector<std::vector<double>> MSubtract(std::vector<std::vector<double>> M1,
										   std::vector<std::vector<double>> M2)
//Returns the subtraction of matrices M1 - M2
{
	//Check sizes
	long Mrows1= M1.size();
	long Mcolumns1= M1[0].size();
	for(long i=1; i<Mrows1; i++){
		assert(Mcolumns1 == M1[i].size());
	}

	long Mrows2= M2.size();
	long Mcolumns2= M2[0].size();
	for(long i=1; i<Mrows2; i++){
		assert(Mcolumns2 == M2[i].size());
	}

	assert(Mcolumns1 == Mcolumns2 && Mrows1 == Mrows2);

	std::vector<std::vector<double>> MS;

	for(long i=0; i<Mrows1; i++){
		MS.push_back(std::vector<double>());
		for(long j=0; j<Mcolumns1; j++){
			MS[i].push_back(M1[i][j]-M2[i][j]);
		}
	}

	return MS;
};


std::vector<std::vector<double>> MProduct(std::vector<std::vector<double>> M1,
										  std::vector<std::vector<double>> M2)
//Returns the product of matrices M1 x M2
{
	//Check sizes
	long Mrows1= M1.size();
	long Mcolumns1= M1[0].size();
	for(long i=1; i<Mrows1; i++){
		assert(Mcolumns1 == M1[i].size());
	}

	long Mrows2= M2.size();
	long Mcolumns2= M2[0].size();
	for(long i=1; i<Mrows2; i++){
		assert(Mcolumns2 == M2[i].size());
	}

	assert(Mcolumns1 == Mrows2);

	std::vector<std::vector<double>> MP;

	for(long i=0; i<Mrows1; i++){
		MP.push_back(std::vector<double>());
		for(long j=0; j<Mcolumns2; j++){
			double MPij= 0;
			for(long k=0; k<Mcolumns1; k++){
				MPij += M1[i][k]*M2[k][j];
			}
			MP[i].push_back(MPij);
		}
	}

	return MP;
};


std::vector<std::vector<double>> MConcatenateRows(std::vector<std::vector<double>> M1,
												  std::vector<std::vector<double>> M2)
//Returns one single matrix resulting from concatenating the rows of M1 and M2
//Rows of M2 are added next to rows of M1

//Example: m1(3,3), m2(2,3), output(5,3)
//    m1  m1  m1
//    m1  m1  m1
//    m1  m1  m1

//    m2  m2  m2
//    m2  m2  m2
{
	//Check sizes
	long Mrows1= M1.size();
	long Mcolumns1= M1[0].size();
	long Mrows2= M2.size();
	long Mcolumns2= M2[0].size();
	assert(Mcolumns1 == Mcolumns2);

	std::vector<std::vector<double>> MC= M1;

	for(long i=0; i<Mrows2; i++){
		MC.push_back(M2[i]);
	}

	return MC;
};


std::vector<std::vector<double>> MConcatenateColumns(std::vector<std::vector<double>> M1,
													 std::vector<std::vector<double>> M2)
//Returns one single matrix resulting from concatenating the columns of M1 and M2
//Columns of M2 are added next to columns of M1

//Example
//    m1  m1  m1     m2  m2
//    m1  m1  m1     m2  m2
{
	//Check sizes
	long Mrows1= M1.size();
	long Mcolumns1= M1[0].size();
	long Mrows2= M2.size();
	long Mcolumns2= M2[0].size();
	assert(Mrows1 == Mrows2);

	std::vector<std::vector<double>> MC= M1;

	for(long i=0; i<Mrows1; i++){
		for(long j=0; j<Mcolumns2; j++){
			MC[i].push_back(M2[i][j]);
		}
	}

	return MC;
};


void MClear(std::vector<std::vector<double>> &M)
//Clears matrix M
{
	for(long i=0; i<(long)M.size(); i++){
		for(long j=0; j<(long)M[i].size(); j++){
			M[i][j]= 0.;
		}
	}
};


double norm_2(std::vector<double> M)
//returns the 2-norm of input vector M
//If M is a vector, this is equal to the Euclidean length of the vector
{
	//2-norm for vectors
	double norm= 0.;
	long n= M.size();
	for(long i= 0; i<n; i++){
		norm += M[i]*M[i];
	}
	return sqrt(norm);
}


double norm_2(std::vector<std::vector<double>> M)
//returns the 2-norm of input vector M
//If M is a vector, this is equal to the Euclidean length of the vector
{
	//check if the matrix is a vector (second dimension equals 1)
	assert(M[0].size() == 1);

	//2-norm for vectors
	double norm= 0.;
	long n= M.size();
	for(long i= 0; i<n; i++){
		norm += M[i][0]*M[i][0];
	}
	return sqrt(norm);
}


double MDeterminant(std::vector<std::vector<double>> M)
//Outputs determinant of square matrix M

//Recursively defines determinant using expansion by minors
{
	double det = 0;
	long n= M.size();

	if(n==1){
		det = M[0][0];
	}

	else if(n==2){
		det = M[0][0] * M[1][1] - M[0][1] * M[1][0];
	}

	else if(n==3){
		//M[0][0] M[0][1] M[0][2] M[0][0] M[0][1] M[0][2]
		//M[1][0] M[1][1] M[1][2] M[1][0] M[1][1] M[1][2]
		//M[2][0] M[2][1] M[2][2] M[2][0] M[2][1] M[2][2]

		det= M[0][0]*M[1][1]*M[2][2] +
			 M[0][1]*M[1][2]*M[2][0] +
			 M[0][2]*M[1][0]*M[2][1] -
			 M[0][0]*M[1][2]*M[2][1] -
			 M[0][1]*M[1][0]*M[2][2] -
			 M[0][2]*M[1][1]*M[2][0];
	}

	else{
		for (long k=0; k<n; k++){
			std::vector<std::vector<double>> m(n-1, std::vector<double>(n-1, 0));

			for (long i=1; i<n; i++){
				long j2 = 0;
				for (long j=0; j<n; j++){
					if (j == k)
						continue;
					m[i-1][j2] = M[i][j];
					j2++;
				}
			}
			det += pow(-1.0,k+2.0) * M[0][k] * MDeterminant(m);
		}
	}

	return det;
}


std::vector<std::vector<double>> MCoFactor(std::vector<std::vector<double>> M)
//Outputs cofactor matrix of square matrix M
{
	long m_rows= M.size();
	long m_cols= M[0].size();

	std::vector<std::vector<double>> cofactor(m_rows, std::vector<double>(m_cols, 0));

	if(m_rows != m_cols)
		return cofactor;

	if(m_rows < 2)
		return cofactor;

	else if(m_rows == 2){
		cofactor[0][0] =  M[1][1];
		cofactor[0][1] = -M[1][0];
		cofactor[1][0] = -M[0][1];
		cofactor[1][1] =  M[0][0];
		return cofactor;
	}

	else if(m_rows >= 3){
		double det;
		long n= M.size();
		std::vector<std::vector<double>> c(n-1, std::vector<double>(n-1, 0));

		for (long j=0; j<n; j++) {
			for (long i=0; i<n; i++) {
				long i1 = 0;
				for (long ii=0; ii<n; ii++) {
					if (ii == i)
						continue;
					long j1 = 0;
					for (long jj=0; jj<n; jj++) {
						if (jj == j)
							continue;
						c[i1][j1] = M[ii][jj];
						j1++;
					}
					i1++;
				}

				det = MDeterminant(c);

				cofactor[i][j] = pow(-1.0,i+j+2.0) * det;
			}
		}
	}

	return cofactor;
}


std::vector<std::vector<double>> MInverse(std::vector<std::vector<double>> M)
//Outputs the inverse of square matrix M

//The inverse of a square matrix with a non zero determinant
//is the adjoint matrix divided by the determinant

//The adjoint matrix is the transpose of the cofactor matrix

// See https://www.cs.rochester.edu/~brown/Crypto/assts/projects/adj.html
//See also http://www.softwareandfinance.com/CPP/Matrix_Inverse.html
{
	long m_rows= M.size();
	long m_cols= M[0].size();

	std::vector<std::vector<double>> inv(m_rows, std::vector<double>(m_cols, 0));

	if(m_rows != m_cols)
		return inv;

	double det = MDeterminant(M);

	if(det == 0.)
		return inv;

	std::vector<std::vector<double>> cofactor= MCoFactor(M);

	// Inverse = Adjoint / Determinant  (Adjoint= transpose of cofactor)
	inv= MScale(MTranspose(cofactor), 1./det);
	return inv;
}


std::vector<std::vector<double>> MPseudoInverse(std::vector<std::vector<double>> M,
												double cutoff)
//Outputs the Moore-Penrose inverse or pseudoinverse of square matrix M

//If M is square and not singular, then pseudoinverse is an expensive way
//to compute inverse
//If M is not square, or is square and singular, then inverse does not exist.
//In these cases, pseudoinverse has some of, but not all, the properties of inverse

//Approach:
//- Find the SVD of the of matrix you wish to find the pseudo-inverse for:
//     A=UEV'
//- the Pseudo-inverse is VFU' where F is the pseudo-inverse of E.
//- E is a diagonal matrix so finding its pseudo-inverse is the same as
//  forming the diagonal matrix with reciprocal elements of E on the diagonal.
{
	long m= M.size();

	//Check that input is a square matrix
	assert(m == M[0].size());

	//Find the SVD of the of the matrix
	SVD svd(M);

	//Find the pseudoinverse of W
	std::vector<std::vector<double>> ww(m,std::vector<double>(m, 0));
	for(long i= 0; i< m; i++)
		if(svd.w[i] > cutoff)
			ww[i][i] = 1./svd.w[i];
		//else
		//	ww[i][i] = 0.;

	//Return the pseudoinverse of M
	return MProduct(svd.v, MProduct(ww, MTranspose(svd.u)));
}


std::vector<std::vector<double>> MLeftDivide(std::vector<std::vector<double>> A,
											 std::vector<std::vector<double>> B)
//Backslash (\) or matrix left division.

//If A is a square matrix, A\B is roughly the same as inv(A)*B,
//except it is computed in a different way.

//If A is an n-by-n matrix and B is a column vector with n components,
//or a matrix with several such columns,
//then X = A\B is the solution to the equation AX = B
//computed by Gaussian elimination.

//A warning message prints if A is badly scaled or nearly singular.
{
	//Find the LU of the of the matrix A
	LU lu(A);

	//Solve A/B
	return lu.solve(B);
}


////////////////////////////////////////////////////////////////////////////////////


LU::LU(std::vector<std::vector<double>> &a)
: n(a.size()),
lu(a),
//aref(a),
indx(n)
{
	const double TINY=1.0e-40;
	long imax;
	double big,temp;
	std::vector<double> vv(n);
	d=1.0;
	for (long i=0;i<n;i++){
		big=0.0;
		for (long j=0;j<n;j++)
			if ((temp=std::fabs(lu[i][j])) > big) big=temp;
		if (big == 0.0){
			AfxMessageBox ("Singular matrix in LUdcmp");
			throw("Singular matrix in LUdcmp");
		}
		vv[i]=1.0/big;
	}
	for (long k=0;k<n;k++){
		big=0.0;
		for (long i=k;i<n;i++) {
			temp=vv[i]*std::fabs(lu[i][k]);
			if (temp > big) {
				big=temp;
				imax=i;
			}
		}
		if (k != imax){
			for (long j=0;j<n;j++) {
				temp=lu[imax][j];
				lu[imax][j]=lu[k][j];
				lu[k][j]=temp;
			}
			d = -d;
			vv[imax]=vv[k];
		}
		indx[k]=imax;
		if (lu[k][k] == 0.0) lu[k][k]=TINY;
		for (long i=k+1;i<n;i++){
			temp=lu[i][k] /= lu[k][k];
			for (long j=k+1;j<n;j++)
				lu[i][j] -= temp*lu[k][j];
		}
	}
}


std::vector<std::vector<double>> LU::solve(std::vector<std::vector<double>> B)
{
	assert(B.size() == n);
	std::vector<std::vector<double>> X(n, std::vector<double>(1, 0));

	long ii=0,ip;
	double sum;
	X= B;

	for (long i=0;i<n;i++) {
		ip=indx[i];
		sum=X[ip][0];
		X[ip][0]=X[i][0];
		if (ii != 0)
			for (long j=ii-1;j<i;j++) sum -= lu[i][j]*X[j][0];
		else if (sum != 0.0)
			ii=i+1;
		X[i][0]=sum;
	}
	for (long i=n-1;i>=0;i--) {
		sum=X[i][0];
		for (long j=i+1;j<n;j++) sum -= lu[i][j]*X[j][0];
		X[i][0]=sum/lu[i][i];
	}

	return X;
}


//void LU::solve(std::vector<std::vector<double>> &b,
//				   std::vector<std::vector<double>> &x)
//{
//	int m= b[0].size();
//	if (b.size() != n || x.size() != n || b[0].size() != x[0].size())
//		throw("LUdcmp::solve bad sizes");
//	std::vector<double> xx(n);
//	for (long j=0;j<m;j++) {
//		for (long i=0;i<n;i++) xx[i] = b[i][j];
//		solve(xx,xx);
//		for (long i=0;i<n;i++) x[i][j] = xx[i];
//	}
//}
//
//
//void LU::inverse(std::vector<std::vector<double>> &ainv)
//{
//	ainv.resize(n);
//	for(long i= 0; i< n; i++){
//		ainv[i].resize(n);
//	}
//
//	for (long i=0;i<n;i++) {
//		for (long j=0;j<n;j++) ainv[i][j] = 0.;
//		ainv[i][i] = 1.;
//	}
//	solve(ainv, ainv);
//}
//
//
//double LU::det()
//{
//	double dd = d;
//	for (long i=0;i<n;i++) dd *= lu[i][i];
//	return dd;
//}


//void LU::mprove(std::vector<double> &b, std::vector<double> &x)
//{
//	std::vector<double> r(n);
//	for (long i=0;i<n;i++) {
//		long double sdp = -b[i];
//		for (long j=0;j<n;j++)
//			sdp += (long double)aref[i][j] * (long double)x[j];
//		r[i]=sdp;
//	}
//	solve(r,r);
//	for (long i=0;i<n;i++) x[i] -= r[i];
//}


////////////////////////////////////////////////////////////////////////////////////


//long SVD::rank(double thresh = -1.)
//{
//	long nr=0;
//	tsh = (thresh >= 0. ? thresh : 0.5*sqrt(m+n+1.)*w[0]*eps);
//	for (long j=0;j<n;j++) if (w[j] > tsh) nr++;
//	return nr;
//}


//long SVD::nullity(double thresh = -1.)
//{
//	long nn=0;
//	tsh = (thresh >= 0. ? thresh : 0.5*sqrt(m+n+1.)*w[0]*eps);
//	for (long j=0;j<n;j++) if (w[j] <= tsh) nn++;
//	return nn;
//}


//std::vector<std::vector<double>> SVD::range(double thresh = -1.)
//{
//	long nr=0;
//	std::vector<std::vector<double>> rnge(m,rank(thresh));
//	for (long j=0;j<n;j++) {
//		if (w[j] > tsh) {
//			for (long i=0;i<m;i++) rnge[i][nr] = u[i][j];
//			nr++;
//		}
//	}
//	return rnge;
//}


//std::vector<std::vector<double>> SVD::nullspace(double thresh = -1.)
//{
//	long nn=0;
//	std::vector<std::vector<double>> nullsp(n,nullity(thresh));
//	for (long j=0;j<n;j++) {
//		if (w[j] <= tsh) {
//			for (long jj=0;jj<n;jj++) nullsp[jj][nn] = v[jj][j];
//			nn++;
//		}
//	}
//	return nullsp;
//}


double SVD::pythag(const double a, const double b)
{
	double absa=std::fabs(a), absb=std::fabs(b);
	return (absa > absb ? absa*sqrt(1.0+SQR(absb/absa)) :
		(absb == 0.0 ? 0.0 : absb*sqrt(1.0+SQR(absa/absb))));
}


void SVD::decompose()
{
	bool flag;
	long i,its,j,jj,k,l,nm;
	double anorm,c,f,g,h,s,scale,x,y,z;
	std::vector<double> rv1(n);
	g = scale = anorm = 0.0;
	for (i=0;i<n;i++) {
		l=i+2;
		rv1[i]=scale*g;
		g=s=scale=0.0;
		if (i < m) {
			for (k=i;k<m;k++) scale += std::fabs(u[k][i]);
			if (scale != 0.0) {
				for (k=i;k<m;k++) {
					u[k][i] /= scale;
					s += u[k][i]*u[k][i];
				}
				f=u[i][i];
				g = -SIGN(sqrt(s),f);
				h=f*g-s;
				u[i][i]=f-g;
				for (j=l-1;j<n;j++) {
					for (s=0.0,k=i;k<m;k++) s += u[k][i]*u[k][j];
					f=s/h;
					for (k=i;k<m;k++) u[k][j] += f*u[k][i];
				}
				for (k=i;k<m;k++) u[k][i] *= scale;
			}
		}
		w[i]=scale *g;
		g=s=scale=0.0;
		if (i+1 <= m && i+1 != n) {
			for (k=l-1;k<n;k++) scale += std::fabs(u[i][k]);
			if (scale != 0.0) {
				for (k=l-1;k<n;k++) {
					u[i][k] /= scale;
					s += u[i][k]*u[i][k];
				}
				f=u[i][l-1];
				g = -SIGN(sqrt(s),f);
				h=f*g-s;
				u[i][l-1]=f-g;
				for (k=l-1;k<n;k++) rv1[k]=u[i][k]/h;
				for (j=l-1;j<m;j++) {
					for (s=0.0,k=l-1;k<n;k++) s += u[j][k]*u[i][k];
					for (k=l-1;k<n;k++) u[j][k] += s*rv1[k];
				}
				for (k=l-1;k<n;k++) u[i][k] *= scale;
			}
		}
		anorm=max(anorm,(std::fabs(w[i])+std::fabs(rv1[i])));
	}
	for (i=n-1;i>=0;i--) {
		if (i < n-1) {
			if (g != 0.0) {
				for (j=l;j<n;j++)
					v[j][i]=(u[i][j]/u[i][l])/g;
				for (j=l;j<n;j++) {
					for (s=0.0,k=l;k<n;k++) s += u[i][k]*v[k][j];
					for (k=l;k<n;k++) v[k][j] += s*v[k][i];
				}
			}
			for (j=l;j<n;j++) v[i][j]=v[j][i]=0.0;
		}
		v[i][i]=1.0;
		g=rv1[i];
		l=i;
	}
	for (i=min(m,n)-1;i>=0;i--) {
		l=i+1;
		g=w[i];
		for (j=l;j<n;j++) u[i][j]=0.0;
		if (g != 0.0) {
			g=1.0/g;
			for (j=l;j<n;j++) {
				for (s=0.0,k=l;k<m;k++) s += u[k][i]*u[k][j];
				f=(s/u[i][i])*g;
				for (k=i;k<m;k++) u[k][j] += f*u[k][i];
			}
			for (j=i;j<m;j++) u[j][i] *= g;
		} else for (j=i;j<m;j++) u[j][i]=0.0;
		++u[i][i];
	}
	for (k=n-1;k>=0;k--) {
		for (its=0;its<30;its++) {
			flag=true;
			for (l=k;l>=0;l--) {
				nm=l-1;
				if (l == 0 || std::fabs(rv1[l]) <= eps*anorm) {
					flag=false;
					break;
				}
				if (std::fabs(w[nm]) <= eps*anorm) break;
			}
			if (flag) {
				c=0.0;
				s=1.0;
				for (i=l;i<k+1;i++) {
					f=s*rv1[i];
					rv1[i]=c*rv1[i];
					if (std::fabs(f) <= eps*anorm) break;
					g=w[i];
					h=pythag(f,g);
					w[i]=h;
					h=1.0/h;
					c=g*h;
					s = -f*h;
					for (j=0;j<m;j++) {
						y=u[j][nm];
						z=u[j][i];
						u[j][nm]=y*c+z*s;
						u[j][i]=z*c-y*s;
					}
				}
			}
			z=w[k];
			if (l == k) {
				if (z < 0.0) {
					w[k] = -z;
					for (j=0;j<n;j++) v[j][k] = -v[j][k];
				}
				break;
			}
			if (its == 29)
				throw("no convergence in 30 svdcmp iterations");
			x=w[l];
			nm=k-1;
			y=w[nm];
			g=rv1[nm];
			h=rv1[k];
			f=((y-z)*(y+z)+(g-h)*(g+h))/(2.0*h*y);
			g=pythag(f,1.0);
			f=((x-z)*(x+z)+h*((y/(f+SIGN(g,f)))-h))/x;
			c=s=1.0;
			for (j=l;j<=nm;j++) {
				i=j+1;
				g=rv1[i];
				y=w[i];
				h=s*g;
				g=c*g;
				z=pythag(f,h);
				rv1[j]=z;
				c=f/z;
				s=h/z;
				f=x*c+g*s;
				g=g*c-x*s;
				h=y*s;
				y *= c;
				for (jj=0;jj<n;jj++) {
					x=v[jj][j];
					z=v[jj][i];
					v[jj][j]=x*c+z*s;
					v[jj][i]=z*c-x*s;
				}
				z=pythag(f,h);
				w[j]=z;
				if (z) {
					z=1.0/z;
					c=f*z;
					s=h*z;
				}
				f=c*g+s*y;
				x=c*y-s*g;
				for (jj=0;jj<m;jj++) {
					y=u[jj][j];
					z=u[jj][i];
					u[jj][j]=y*c+z*s;
					u[jj][i]=z*c-y*s;
				}
			}
			rv1[l]=0.0;
			rv1[k]=f;
			w[k]=x;
		}
	}
}


void SVD::reorder()
{
	long inc=1;
	double sw;
	std::vector<double> su(m), sv(n);
	do { inc *= 3; inc++; } while (inc <= n);
	do {
		inc /= 3;
		for (long i=inc;i<n;i++) {
			sw = w[i];
			for (long k=0;k<m;k++) su[k] = u[k][i];
			for (long k=0;k<n;k++) sv[k] = v[k][i];
			long j = i;
			while (w[j-inc] < sw) {
				w[j] = w[j-inc];
				for (long k=0;k<m;k++) u[k][j] = u[k][j-inc];
				for (long k=0;k<n;k++) v[k][j] = v[k][j-inc];
				j -= inc;
				if (j < inc) break;
			}
			w[j] = sw;
			for (long k=0;k<m;k++) u[k][j] = su[k];
			for (long k=0;k<n;k++) v[k][j] = sv[k];
		}
	} while (inc > 1);

	for (long k=0;k<n;k++) {
		long s=0;
		for (long i=0;i<m;i++) if (u[i][k] < 0.) s++;
		for (long j=0;j<n;j++) if (v[j][k] < 0.) s++;
		if (s > (m+n)/2) {
			for (long i=0;i<m;i++) u[i][k] = -u[i][k];
			for (long j=0;j<n;j++) v[j][k] = -v[j][k];
		}
	}
}


SVD::SVD(std::vector<std::vector<double>> &a)
//: m(a.size()),
//  n(a[0].size()),
//  u(a),
//  v(n,n),
//  w(n)
{
	m= a.size();
	n= a[0].size();
	u= a;
	v.resize(n);
	for (long i=0; i<n; i++)
		v[i].resize(n);
	w.resize(n);

	eps = std::numeric_limits<double>::epsilon();
	decompose();
	reorder();
	tsh = 0.5*sqrt(m+n+1.)*w[0]*eps;
}


//void SVD::solve(std::vector<double> b,
//				std::vector<double> &x,
//				double thresh = -1.)
//{
//	double s;
//	if (b.size() != m || x.size() != n) throw("SVD::solve bad sizes");
//	std::vector<double> tmp(n);
//	tsh = (thresh >= 0. ? thresh : 0.5*sqrt(m+n+1.)*w[0]*eps);
//	for (long j=0;j<n;j++) {
//		s=0.0;
//		if (w[j] > tsh) {
//			for (long i=0;i<m;i++) s += u[i][j]*b[i];
//			s /= w[j];
//		}
//		tmp[j]=s;
//	}
//	for (long j=0;j<n;j++) {
//		s=0.0;
//		for (long jj=0;jj<n;jj++) s += v[j][jj]*tmp[jj];
//		x[j]=s;
//	}
//}


//void SVD::solve(std::vector<std::vector<double>> b,
//				std::vector<std::vector<double>> &x,
//				double thresh = -1.)
//{
//	long m=b[0].size();
//	if (b.size() != n || x.size() != n || b[0].size() != x[0].size())
//		throw("SVD::solve bad sizes");
//	std::vector<double> xx(n);
//	for (long j=0;j<m;j++) {
//		for (long i=0;i<n;i++) xx[i] = b[i][j];
//		solve(xx,xx,thresh);
//		for (long i=0;i<n;i++) x[i][j] = xx[i];
//	}
//}


////////////////////////////////////////////////////////////////////////////////////


void Eigen::balance(std::vector<std::vector<double>> &M,
					std::vector<double> &scale)
//Given a matriz M, replaces it by a balanced matrix with identical eigenvalues

//Symmetric matrices are unnafected by this process, as they are already balanced
{
	long n= M.size();

	//Radix base employed for floating-point arithmetic
	//i.e. 2 for all modern machines, but 16 for some historical mainframe architectures)
	const double RADIX= std::numeric_limits<double>::radix;
	bool done= false;
	double sqrdx= RADIX*RADIX;

	while (!done){
		done= true;
		//Calculate row and column norms
		for (long i= 0; i<n; i++){
			double r= 0.0, c= 0.0;
			for (long j= 0; j<n; j++){
				c += std::fabs(M[j][i]);
				r += std::fabs(M[i][j]);
			}

			//If both are non-zero
			if(c!=0.0 && r!=0.0){
				//Find the integer power of the machine raxi
				//that comes closest to balancing the matrix
				double g= r/RADIX;
				double f= 1.0;
				double s= c+r;
				while (c<g){
					f *= RADIX;
					c *= sqrdx;
				}

				g= r*RADIX;
				while(c>g){
					f/= RADIX;
					c/= sqrdx;
				}

				if ((c+r)/f < 0.95*s){
					done= false;
					g= 1.0/f;
					scale[i] *= f;
					//Apply similarity transformation
					for(long j= 0; j<n; j++) M[i][j] *= g;
					for(long j= 0; j<n; j++) M[j][i] *= f;
				}
			}
		}
	}
}


void Eigen::elmhes(std::vector<std::vector<double>> &M,
				   std::vector<long> &perm)
//Reduction to Hessenberg form by elimination method

//Replaces the real nonsymmetric matrix M[0...n-1][0...n-1]
//by an upper Hessenberg matrix with identical eigenvalues

//Recommended, but not required, is that this routine be preceded by balance

//On output, the Hessenberg matrix is in elements M[i[[j] with i<=j
//Elements with i<=j should be zero, but are returned with random values
{
	long n= M.size();

	for (long m= 1; m<n-1; m++){
		double x= 0.0;
		long i= m;
		for (long j= m; j<n; j++){
			if(std::fabs(M[j][m-1]) > std::fabs(x)){
				x= M[j][m-1];
				i= j;
			}
		}
		//Store permutation
		perm[m]= i;
		//Interchange rows and columns
		if(i!=m){
			for(long j= m-1; j<n; j++) std::swap(M[i][j], M[m][j]);
			for(long j= 0; j<n; j++) std::swap(M[j][i], M[j][m]);
		}
		if(x!=0.0){
			//Carry out the elimination
			for(i= m+1; i<n; i++){
				double y= M[i][m-1];
				if(y!=0.0){
					y /= x;
					M[i][m-1]= y;
					for(long j= m; j<n; j++) M[i][j] -= y*M[m][j];
					for(long j= 0; j<n; j++) M[j][m] += y*M[j][i];
				}
			}
		}
	}
}


void Eigen::eltran(std::vector<std::vector<double>> M,
				   std::vector<long> perm,
				   std::vector<std::vector<double>> &zz)
//This routine accumulates the stabilized elementary similarity transformations used in the reduction
//to upper Hessenberg form by elmhes

//The multipliers that were used in the reduction are obtained from the lower triangle
//(below the subdiagonal) of M
//The transformations are permuted according to the permutations stored in perm by elmhes
{
	long n= M.size();

	for (long mp= n-2; mp>0; mp--){
		for (long k= mp+1; k<n; k++){
			zz[k][mp]= M[k][mp-1];
		}
		long i= perm[mp];
		if(i!=mp){
			for (long j= mp; j<n; j++){
				zz[mp][j]= zz[i][j];
				zz[i][j]= 0.0;
			}
			zz[i][mp]= 1.0;
		}
	}
}


bool Eigen::hqr2(std::vector<std::vector<double>> &M,
				 std::vector<std::vector<double>> &zz,
				 std::vector<std::complex<double>> &wri)
//Finds all eigenvalues of an upper Hessenberg matrix M[0..n-1][0..n-1]

//On input M can be exactly as output from elmhes and eltran
//On output, wri[0..n-1] contains the eigenvalues of M
//while zz[0..n-1][0..n-1] is a matrix whose columns contain the
//corresponding eigenvectors.

//The eigenvalues are not sorted,
//except that complex conjugate pairs appear consecutively
//with the eigenvalue having the positive imaginary part first

//(A real matrix can have complex-conjugate pairs of eigenvalues)

//For a complex eigenvalue, only the eigenvector corresponding
//to the eigenvalue with positive imaginary part is stored,
//with real part in zz[0..n-1][i] and imaginary part in zz[0..n-1][i+1]

//The eigenvectors are not normalized.
{
	long n= M.size();

	long m,l,k,j,i,mmin,na;
	double z,y,x,w,v,u,s,r,q,p,anorm=0.0,ra,sa,vr,vi;

	//Compute matrix norm for possible use in locating single small subdiagonal elements
	const double EPS= std::numeric_limits<double>::epsilon();
	for (i=0;i<n;i++)
		for (j=max(i-1,0);j<n;j++)
			anorm += std::fabs(M[i][j]);

	long nn=n-1;
	double t=0.0;
	//Begin search for mext eigenvalue
	while (nn >= 0) {
		long its=0;
		do {
			for (l=nn;l>0;l--) {
				s=std::fabs(M[l-1][l-1])+std::fabs(M[l][l]);
				if (s == 0.0) s=anorm;
				if (std::fabs(M[l][l-1]) <= EPS*s) {
					M[l][l-1] = 0.0;
					break;
				}
			}
			x=M[nn][nn];
			if (l == nn) {                           //One root found
				wri[nn]=M[nn][nn]=x+t;
				nn--;
			} else {
				y=M[nn-1][nn-1];
				w=M[nn][nn-1]*M[nn-1][nn];
				if (l == nn-1) {                    //Two roots found
					p=0.5*(y-x);
					q=p*p+w;
					z=sqrt(std::fabs(q));
					x += t;
					M[nn][nn]=x;
					M[nn-1][nn-1]=y+t;
					if (q >= 0.0) {                        //...a real pair
						z=p+SIGN(z,p);
						wri[nn-1]=wri[nn]=x+z;
						if (z != 0.0) wri[nn]=x-w/z;
						x=M[nn][nn-1];
						s=std::fabs(x)+std::fabs(z);
						p=x/s;
						q=z/s;
						r=sqrt(p*p+q*q);
						p /= r;
						q /= r;
						for (j=nn-1;j<n;j++) {                             //Row modification
							z=M[nn-1][j];
							M[nn-1][j]=q*z+p*M[nn][j];
							M[nn][j]=q*M[nn][j]-p*z;
						}
						for (i=0;i<=nn;i++) {                             //Column modification
							z=M[i][nn-1];
							M[i][nn-1]=q*z+p*M[i][nn];
							M[i][nn]=q*M[i][nn]-p*z;
						}
						for (i=0;i<n;i++) {                               //Accumulate transformations
							z=zz[i][nn-1];
							zz[i][nn-1]=q*z+p*zz[i][nn];
							zz[i][nn]=q*zz[i][nn]-p*z;
						}
					} else {                             //...a complex pair
						wri[nn]=std::complex<double>(x+p,-z);
						wri[nn-1]=conj(wri[nn]);
					}
					nn -= 2;
				} else {                                 //No roots found. Continue iteration
					if (its == 30)
						//throw("Too many iterations in hqr");
						return false;
					if (its == 10 || its == 20) {
						t += x;
						for (i=0;i<nn+1;i++) M[i][i] -= x;
						s=std::fabs(M[nn][nn-1])+std::fabs(M[nn-1][nn-2]);
						y=x=0.75*s;
						w = -0.4375*s*s;
					}
					++its;
					for (m=nn-2;m>=l;m--) {
						z=M[m][m];
						r=x-z;
						s=y-z;
						p=(r*s-w)/M[m+1][m]+M[m][m+1];
						q=M[m+1][m+1]-z-r-s;
						r=M[m+2][m+1];
						s=std::fabs(p)+std::fabs(q)+std::fabs(r);
						p /= s;
						q /= s;
						r /= s;
						if (m == l) break;
						u=std::fabs(M[m][m-1])*(std::fabs(q)+std::fabs(r));
						v=std::fabs(p)*(std::fabs(M[m-1][m-1])+std::fabs(z)+std::fabs(M[m+1][m+1]));
						if (u <= EPS*v) break;
					}
					for (i=m;i<nn-1;i++) {
						M[i+2][i]=0.0;
						if (i != m) M[i+2][i-1]=0.0;
					}
					for (k=m;k<nn;k++) {
						//Double QR step on rows 1 to nn and columns n to nn
						if (k != m) {
							p=M[k][k-1];
							q=M[k+1][k-1];
							r=0.0;
							if (k+1 != nn) r=M[k+2][k-1];
							if ((x=std::fabs(p)+std::fabs(q)+std::fabs(r)) != 0.0) {
								p /= x;
								q /= x;
								r /= x;
							}
						}
						if ((s=SIGN(sqrt(p*p+q*q+r*r),p)) != 0.0) {
							if (k == m) {
								if (l != m)
								M[k][k-1] = -M[k][k-1];
							} else
								M[k][k-1] = -s*x;
							p += s;
							x=p/s;
							y=q/s;
							z=r/s;
							q /= p;
							r /= p;
							for (j=k;j<n;j++) {
								p=M[k][j]+q*M[k+1][j];
								if (k+1 != nn) {
									p += r*M[k+2][j];
									M[k+2][j] -= p*z;
								}
								M[k+1][j] -= p*y;
								M[k][j] -= p*x;
							}
							mmin = nn < k+3 ? nn : k+3;
							for (i=0;i<mmin+1;i++) {
								p=x*M[i][k]+y*M[i][k+1];
								if (k+1 != nn) {
									p += z*M[i][k+2];
									M[i][k+2] -= p*r;
								}
								M[i][k+1] -= p*q;
								M[i][k] -= p;
							}
							for (i=0; i<n; i++) {
								p=x*zz[i][k]+y*zz[i][k+1];
								if (k+1 != nn) {
									p += z*zz[i][k+2];
									zz[i][k+2] -= p*r;
								}
								zz[i][k+1] -= p*q;
								zz[i][k] -= p;
							}
						}
					}
				}
			}
		} while (l+1 < nn);
	}

	//All roots found
	//Backsubstitute to find vectors of upper triangular form
	if (anorm != 0.0) {
		for (nn=n-1;nn>=0;nn--) {
			p=real(wri[nn]);
			q=imag(wri[nn]);
			na=nn-1;
			if (q == 0.0) {    //Real vector
				m=nn;
				M[nn][nn]=1.0;
				for (i=nn-1;i>=0;i--) {
					w=M[i][i]-p;
					r=0.0;
					for (j=m;j<=nn;j++)
						r += M[i][j]*M[j][nn];
					if (imag(wri[i]) < 0.0) {
						z=w;
						s=r;
					} else {
						m=i;
						
						if (imag(wri[i]) == 0.0) {
							t=w;
							if (t == 0.0)
								t=EPS*anorm;
							M[i][nn]=-r/t;
						} else {                //Solve real equation
							x=M[i][i+1];
							y=M[i+1][i];
							q=SQR(real(wri[i])-p)+SQR(imag(wri[i]));
							t=(x*s-z*r)/q;
							M[i][nn]=t;
							if (std::fabs(x) > std::fabs(z))
								M[i+1][nn]=(-r-w*t)/x;
							else
								M[i+1][nn]=(-s-y*t)/z;
						}
						t=std::fabs(M[i][nn]);         //Overflow control
						if (EPS*t*t > 1)
							for (j=i;j<=nn;j++)
								M[j][nn] /= t;
					}
				}
			} else if (q < 0.0) {              //Complex vector, only do one case
				m=na;                          //Last vector component chosen imaginary, so that eigenvector matrix is triangular
				if (std::fabs(M[nn][na]) > std::fabs(M[na][nn])) {
					M[na][na]=q/M[nn][na];
					M[na][nn]=-(M[nn][nn]-p)/M[nn][na];
				} else {
					std::complex<double> temp= std::complex<double>(0.0,-M[na][nn])/
											   std::complex<double>(M[na][na]-p,q);
					M[na][na]=real(temp);
					M[na][nn]=imag(temp);
				}
				M[nn][na]=0.0;
				M[nn][nn]=1.0;
				for (i=nn-2;i>=0;i--) {
					w=M[i][i]-p;
					ra=sa=0.0;
					for (j=m;j<=nn;j++) {
						ra += M[i][j]*M[j][na];
						sa += M[i][j]*M[j][nn];
					}
					if (imag(wri[i]) < 0.0) {
						z=w;
						r=ra;
						s=sa;
					} else {
						m=i;
						if (imag(wri[i]) == 0.0) {
							std::complex<double> temp = std::complex<double>(-ra,-sa)/std::complex<double>(w,q);
							M[i][na]=real(temp);
							M[i][nn]=imag(temp);
						} else {                           //Solve complex equations
							x=M[i][i+1];
							y=M[i+1][i];
							vr=SQR(real(wri[i])-p)+SQR(imag(wri[i]))-q*q;
							vi=2.0*q*(real(wri[i])-p);
							if (vr == 0.0 && vi == 0.0)
								vr=EPS*anorm*(std::fabs(w)+std::fabs(q)+std::fabs(x)+std::fabs(y)+std::fabs(z));
							std::complex<double> temp= std::complex<double>(x*r-z*ra+q*sa,x*s-z*sa-q*ra)/
													   std::complex<double>(vr,vi);
							M[i][na]=real(temp);
							M[i][nn]=imag(temp);
							if (std::fabs(x) > std::fabs(z)+std::fabs(q)) {
								M[i+1][na]=(-ra-w*M[i][na]+q*M[i][nn])/x;
								M[i+1][nn]=(-sa-w*M[i][nn]-q*M[i][na])/x;
							} else {
								std::complex<double> temp= std::complex<double>(-r-y*M[i][na],-s-y*M[i][nn])/
														   std::complex<double>(z,q);
								M[i+1][na]=real(temp);
								M[i+1][nn]=imag(temp);
							}
						}
					}
					t=max(std::fabs(M[i][na]),std::fabs(M[i][nn]));          //Overflow control
					if (EPS*t*t > 1)
						for (j=i;j<=nn;j++) {
							M[j][na] /= t;
							M[j][nn] /= t;
						}
				}
			}
		}

		//Multiply by transforming matrix to give vectors of original full matrix
		for (j=n-1;j>=0;j--)
			for (i=0;i<n;i++) {
				z=0.0;
				for (k=0;k<=j;k++)
					z += zz[i][k]*M[k][j];
				zz[i][j]=z;
			}
	}

	return true;
}


void Eigen::balbak(std::vector<std::vector<double>> &zz,
				   std::vector<double> scale)
//Forms the eigenvectors of a real nonsymmetric matrix by back transforming
//those of the corresponding balanced matrix determined by balance
{
	long n= zz.size();

	for (long i= 0; i<n; i++){
		for (long j= 0; j<n; j++){
			zz[i][j]*= scale[i];
		}
	}
}


void Eigen::sortvecs(std::vector<std::vector<double>> &zz,
					 std::vector<std::complex<double>> &wri)
{
	long n= zz.size();

	long i;
	std::vector<double> temp(n, 0);
	for (long j=1;j<n;j++) {
		std::complex<double> x=wri[j];
		for (long k=0;k<n;k++)
			temp[k]=zz[k][j];
		for (i=j-1;i>=0;i--) {
			if (real(wri[i]) >= real(x)) break;
			wri[i+1]=wri[i];
			for (long k=0;k<n;k++)
				zz[k][i+1]=zz[k][i];
		}
		wri[i+1]=x;
		for (long k=0;k<n;k++)
			zz[k][i+1]=temp[k];
	}
}


bool Eigen::UnsymEigen(std::vector<std::vector<double>> M,
					   std::vector<std::complex<double>> &eigenvalues,
					   std::vector<std::vector<double>> &EigenVectors)
//Computes all eigenvalues and eigenvectors of a real nonsymmetric matrix M [0...n-1][0...n-1]
//The approach is a Hessenberg reduction followed by QR iteration

//On output, wri [0...n-1] containes eigenvalues sorted in descending order
//zz[0...n-1][0...n-1] contain the corresponging eigenvectors stored in columns

//For a complex eigenvalue, only eigenvector corresponding to the eigenvalue
//with a positive imaginary part is stored,
//with the real part in zz[0...n-1][i] and the imaginary part in h.zz[0...n-1][i+1]
//The eigenvectors are not normalized

//This function was adapted from Numerical Recipes 3rd edition
	//Numerical Recipes: The Art of Scientific Computing
	//Third Edition in C++ (2007)
	//www.nr.com
{
	long n= M.size();

	std::vector<double> scale(n,1);    //Stores scaling from balance
	balance(M, scale);

	std::vector<long> perm(n,0);  //Stores permutation from elmhes
	elmhes(M, perm);

	//initialize eigenvectors matrix to unit matrix
	for (long i= 0; i<n; i++){
		EigenVectors[i][i]= 1.0;
	}
	eltran(M, perm, EigenVectors);

	if(!hqr2(M, EigenVectors, eigenvalues))
		return false;

	balbak(EigenVectors, scale);

	sortvecs(EigenVectors, eigenvalues);

	return true;
}


//bool Eigen::Jacobi(std::vector<std::vector<double>> Matrix,
//				   std::vector<std::vector<double>> &eigenvalues,
//				   std::vector<std::vector<double>> &EigenVectors)
////The Jacobi eigenvalue method repeatedly performs rotations
////until the matrix becomes almost diagonal
//
////After every rotation, the elements in the diagonal are approximations
////of the (real) eigenvalues of Matrix
//
////The function always returns estimate eigenvalues and EigenVectors
////But function is only true when true eigenvalues and eigenvectors are returned
//
////Adapted from:
//		//José Guerrero
//		//Autovalores y autovectores: Rotación de Jacobi
//		//Publicado en 3 junio, 2010
//		//http://joseguerreroa.wordpress.com/2010/06/03/autovalores-y-autovectores-rotacion-de-jacobi/
//{
//
//	//Only square matrices are valid
//	long n= Matrix.size();
//	for(long i=0; i<n; i++){
//		assert(n == Matrix[i].size());
//	}
//
//	//Only non-null matrices are valid
//	for(long i=0; i<n; i++)
//		for(long j=0; j<n; j++)
//			if(Matrix[i][j] != 0) goto seguir1;
//	return false;
//	seguir1:
//
//	//Only symmetric matrices are valid
//	for (long ip = 0; ip<n-1; ip++) {
//		for (long iq = ip+1; iq<n; iq++) {
//			assert(Matrix[ip][iq] == Matrix[iq][ip]);
//		}
//	}
//
//	//Assign local memory
//	std::vector<double> b(n,0);
//	std::vector<double> z(n,0);
//
//	double c, g, h, s, t, tau, theta;
//
//	//Set initial EigenVectors
//	for (long ip= 0; ip<n; ip++) {
//		for (long iq = 0; iq < n; iq++) {
//			EigenVectors[ip][iq] = 0;
//		}
//		EigenVectors[ip][ip] = 1;
//	}
//
//	//Set initial eigenvalues to the diagonal of Matrix
//	for (long ip= 0; ip<n; ip++){
//		b[ip] = Matrix[ip][ip];
//		eigenvalues[ip][0] = b[ip];
//	}
//
//	long nrot = 0;
//
//	long MaxIterations= 5000;
//
//	for (long i = 0; i < MaxIterations; i++) {
//		double sm = 0;
//		for (long ip = 0; ip<n-1; ip++) {
//			for (long iq = ip+1; iq<n; iq++) {
//				sm +=std::fabs(Matrix[ip][iq]);
//			}
//		}
//		if (sm == 0)
//			return true;
//
//		double tresh= 0.0;
//		if (i < 4)
//			tresh = 0.2*sm/(n*n);
//
//		for (long ip= 0; ip<n-1; ip++) {
//			for (long iq = ip+1; iq<n; iq++) {
//				g = 100.0*std::fabs(Matrix[ip][iq]);
//
//				if(i>4 && (std::fabs(eigenvalues[ip][0])+g) == std::fabs(eigenvalues[ip][0])
//					   && (std::fabs(eigenvalues[iq][0])+g) == fabs(eigenvalues[iq][0]))
//					Matrix[ip][iq] = 0.0;
//
//				else if (std::fabs(Matrix[ip][iq]) > tresh) {
//					h = eigenvalues[iq][0] - eigenvalues[ip][0];
//					if ((std::fabs(h)+g) == std::fabs(h))
//						t = (Matrix[ip][iq])/h;   // t = 1/(2theta)
//					else {
//						theta = 0.5*h/(Matrix[ip][iq]);
//						t = 1.0/(std::fabs(theta)+sqrt(1.0+theta*theta));
//						if(theta < 0.0) t = -t;
//					}
//
//					c = 1.0/sqrt(1+t*t);
//					s = t*c;
//					tau = s/(1.0+c);
//					h = t*Matrix[ip][iq];
//					z[ip] -=h;
//					z[iq] +=h;
//					eigenvalues[ip][0] -=h;
//					eigenvalues[iq][0] +=h;
//					Matrix[ip][iq] = 0.0;
//
//					//Rotate Matrix
//					for (long j= 0; j<ip; j++) {
//						g = Matrix[j][ip];
//						h = Matrix[j][iq];
//						Matrix[j][ip] = g - s*(h+g*tau);
//						Matrix[j][iq] = h + s*(g-h*tau);
//					}
//					for (long j= ip+1; j < iq; j++) {
//						g = Matrix[ip][j];
//						h = Matrix[j][iq];
//						Matrix[ip][j] = g - s*(h+g*tau);
//						Matrix[j][iq] = h + s*(g-h*tau);
//					}
//					for (long j= iq+1; j<n; j++) {
//						g = Matrix[ip][j];
//						h = Matrix[iq][j];
//						Matrix[ip][j] = g - s*(h+g*tau);
//						Matrix[iq][j] = h + s*(g-h*tau);
//					}
//
//					//Save current estimate EigenVectors
//					for (long j= 0; j<n; j++) {
//						g = EigenVectors[j][ip];
//						h = EigenVectors[j][iq];
//						EigenVectors[j][ip] = g - s*(h+g*tau);
//						EigenVectors[j][iq] = h + s*(g-h*tau);
//					}
//					++(nrot);
//				}
//			}
//		}
//
//		//Save current estimate EigenValues
//		for (long ip= 0; ip<n; ip++) {
//			b[ip] = b[ip]+z[ip];
//			eigenvalues[ip][0] = b[ip];
//			z[ip] = 0.0;
//		}
//	}
//
//	return false;
//}
//
//
//bool Eigen::Eigen3x3(std::vector<std::vector<double>> Matrix,
//					 std::vector<std::vector<double>> &eigenvalues,
//					 std::vector<std::vector<double>> &EigenVectors)
////Returns the eigenvalues and eigenvectors of a 3x3 matrix
//
////values are calculated analitically, after the formulas obtained from:
//// http://www.wolframalpha.com/input/?i=eigenvalues+of+{{a%2Cb%2Cc}%2C{d%2Ce%2Cf}%2C{g%2Ch%2Ci}}
//{
//	//Only 3x3 square matrices are valid
//	assert(Matrix.size() == 3);
//	for(long i=0; i<3; i++){
//		assert(Matrix[i].size() == 3);
//	}
//
//	//Only non-null matrices are valid
//	for(long i=0; i<3; i++)
//		for(long j=0; j<3; j++)
//			if(Matrix[i][j] != 0) goto seguir1;
//	return false;
//	seguir1:
//
//	//Local variables
//	double a= Matrix[0][0];
//	double b= Matrix[0][1];
//	double c= Matrix[0][2];
//	double d= Matrix[1][0];
//	double e= Matrix[1][1];
//	double f= Matrix[1][2];
//	double g= Matrix[2][0];
//	double h= Matrix[2][1];
//	double i= Matrix[2][2];
//
//	double a2= a*a;
//	double a3= a2*a;
//	double e2= e*e;
//	double e3= e2*e;
//	double i2= i*i;
//	double i3= i2*i;
//
//	//eigenvalues[0] = (2 a^3-3 a^2 e-3 a^2 i+sqrt(4 (-a^2+a e+a i-3 b d-3 c g-e^2+e i-3 f h-i^2)^3+(2 a^3-3 a^2 e-3 a^2 i+9 a b d+9 a c g-3 a e^2+12 a e i-18 a f h-3 a i^2+9 b d e-18 b d i+27 b f g+27 c d h-18 c e g+9 c g i+2 e^3-3 e^2 i+9 e f h-3 e i^2+9 f h i+2 i^3)^2)+9 a b d+9 a c g-3 a e^2+12 a e i-18 a f h-3 a i^2+9 b d e-18 b d i+27 b f g+27 c d h-18 c e g+9 c g i+2 e^3-3 e^2 i+9 e f h-3 e i^2+9 f h i+2 i^3)^(1/3)/(3 2^(1/3))-(2^(1/3) (-a^2+a e+a i-3 b d-3 c g-e^2+e i-3 f h-i^2))/(3 (2 a^3-3 a^2 e-3 a^2 i+sqrt(4 (-a^2+a e+a i-3 b d-3 c g-e^2+e i-3 f h-i^2)^3+(2 a^3-3 a^2 e-3 a^2 i+9 a b d+9 a c g-3 a e^2+12 a e i-18 a f h-3 a i^2+9 b d e-18 b d i+27 b f g+27 c d h-18 c e g+9 c g i+2 e^3-3 e^2 i+9 e f h-3 e i^2+9 f h i+2 i^3)^2)+9 a b d+9 a c g-3 a e^2+12 a e i-18 a f h-3 a i^2+9 b d e-18 b d i+27 b f g+27 c d h-18 c e g+9 c g i+2 e^3-3 e^2 i+9 e f h-3 e i^2+9 f h i+2 i^3)^(1/3))+1/3 (a+e+i);
//	//eigenvalues[1] = -((1-i sqrt(3)) (2 a^3-3 a^2 e-3 a^2 i+sqrt(4 (-a^2+a e+a i-3 b d-3 c g-e^2+e i-3 f h-i^2)^3+(2 a^3-3 a^2 e-3 a^2 i+9 a b d+9 a c g-3 a e^2+12 a e i-18 a f h-3 a i^2+9 b d e-18 b d i+27 b f g+27 c d h-18 c e g+9 c g i+2 e^3-3 e^2 i+9 e f h-3 e i^2+9 f h i+2 i^3)^2)+9 a b d+9 a c g-3 a e^2+12 a e i-18 a f h-3 a i^2+9 b d e-18 b d i+27 b f g+27 c d h-18 c e g+9 c g i+2 e^3-3 e^2 i+9 e f h-3 e i^2+9 f h i+2 i^3)^(1/3))/(6 2^(1/3))+((1+i sqrt(3)) (-a^2+a e+a i-3 b d-3 c g-e^2+e i-3 f h-i^2))/(3 2^(2/3) (2 a^3-3 a^2 e-3 a^2 i+sqrt(4 (-a^2+a e+a i-3 b d-3 c g-e^2+e i-3 f h-i^2)^3+(2 a^3-3 a^2 e-3 a^2 i+9 a b d+9 a c g-3 a e^2+12 a e i-18 a f h-3 a i^2+9 b d e-18 b d i+27 b f g+27 c d h-18 c e g+9 c g i+2 e^3-3 e^2 i+9 e f h-3 e i^2+9 f h i+2 i^3)^2)+9 a b d+9 a c g-3 a e^2+12 a e i-18 a f h-3 a i^2+9 b d e-18 b d i+27 b f g+27 c d h-18 c e g+9 c g i+2 e^3-3 e^2 i+9 e f h-3 e i^2+9 f h i+2 i^3)^(1/3))+1/3 (a+e+i);
//	//eigenvalues[2] = -((1+i sqrt(3)) (2 a^3-3 a^2 e-3 a^2 i+sqrt(4 (-a^2+a e+a i-3 b d-3 c g-e^2+e i-3 f h-i^2)^3+(2 a^3-3 a^2 e-3 a^2 i+9 a b d+9 a c g-3 a e^2+12 a e i-18 a f h-3 a i^2+9 b d e-18 b d i+27 b f g+27 c d h-18 c e g+9 c g i+2 e^3-3 e^2 i+9 e f h-3 e i^2+9 f h i+2 i^3)^2)+9 a b d+9 a c g-3 a e^2+12 a e i-18 a f h-3 a i^2+9 b d e-18 b d i+27 b f g+27 c d h-18 c e g+9 c g i+2 e^3-3 e^2 i+9 e f h-3 e i^2+9 f h i+2 i^3)^(1/3))/(6 2^(1/3))+((1-i sqrt(3)) (-a^2+a e+a i-3 b d-3 c g-e^2+e i-3 f h-i^2))/(3 2^(2/3) (2 a^3-3 a^2 e-3 a^2 i+sqrt(4 (-a^2+a e+a i-3 b d-3 c g-e^2+e i-3 f h-i^2)^3+(2 a^3-3 a^2 e-3 a^2 i+9 a b d+9 a c g-3 a e^2+12 a e i-18 a f h-3 a i^2+9 b d e-18 b d i+27 b f g+27 c d h-18 c e g+9 c g i+2 e^3-3 e^2 i+9 e f h-3 e i^2+9 f h i+2 i^3)^2)+9 a b d+9 a c g-3 a e^2+12 a e i-18 a f h-3 a i^2+9 b d e-18 b d i+27 b f g+27 c d h-18 c e g+9 c g i+2 e^3-3 e^2 i+9 e f h-3 e i^2+9 f h i+2 i^3)^(1/3))+1/3 (a+e+i);
//
//	double divider= (3*(2*a3-3*a2*e-3*a2*i+
//						sqrt(4*pow(-a2+a*e+a*i-3*b*d-3*c*g-e2+e*i-3*f*h-i2, 3)+
//							 pow(2*a3-3*a2*e-3*a2*i+9*a*b*d+9*a*c*g-3*a*e2+12*a*e*i-18*a*f*h-
//							 3*a*i2+9*b*d*e-18*b*d*i+27*b*f*g+27*c*d*h-18*c*e*g+
//							 9*c*g*i+2*e3-3*e2*i+9*e*f*h-3*e*i2+9*f*h*i+2*i3, 2))+
//						9*a*b*d+9*a*c*g-3*a*e2+12*a*e*i-18*a*f*h-3*a*i2+9*b*d*e-
//						18*b*d*i+27*b*f*g+27*c*d*h-18*c*e*g+9*c*g*i+2*e3-
//						3*e2*i+9*e*f*h-3*e*i2+9*f*h*i+2*i3, (1/3)));
//	if(divider != 0){
//		eigenvalues[0][0] = pow(2*a3-3*a2*e-3*a2*i+
//								sqrt(4*pow(-a2+a*e+a*i-3*b*d-3*c*g-e2+e*i-3*f*h-i2, 3)+
//									 pow(2*a3-3*a2*e-3*a2*i+9*a*b*d+9*a*c*g-3*a*e2+12*a*e*i-18*a*f*h-
//										 3*a*i2+9*b*d*e-18*b*d*i+27*b*f*g+27*c*d*h-18*c*e*g+
//										 9*c*g*i+2*e3-3*e2*i+9*e*f*h-3*e*i2+9*f*h*i+2*i3, 2))+
//								9*a*b*d+9*a*c*g-3*a*e2+12*a*e*i-18*a*f*h-3*a*i2+9*b*d*e-
//								18*b*d*i+27*b*f*g+27*c*d*h-18*c*e*g+9*c*g*i+
//								2*e3-3*e2*i+9*e*f*h-3*e*i2+9*f*h*i+2*i3, (1/3))/
//							(3*pow(2., 1/3))-
//
//							(pow(2., 1/3)*(-a2+a*e+a*i-3*b*d-3*c*g-e2+e*i-3*f*h-i2))/divider+
//
//							1/3*(a+e+i);
//	}
//
//	//EigenVectors[0][0] = (-(1/3 (-a-e-i)+i+(2^(1/3) (-a^2-3 b d+a e-e^2-3 c g-3 f h+a i+e i-i^2))/(3 (2 a^3+9 a b d-3 a^2 e+9 b d e-3 a e^2+2 e^3+9 a c g-18 c e g+27 b f g+27 c d h-18 a f h+9 e f h-3 a^2 i-18 b d i+12 a e i-3 e^2 i+9 c g i+9 f h i-3 a i^2-3 e i^2+2 i^3+sqrt(4 (-a^2-3 b d+a e-e^2-3 c g-3 f h+a i+e i-i^2)^3+(2 a^3+9 a b d-3 a^2 e+9 b d e-3 a e^2+2 e^3+9 a c g-18 c e g+27 b f g+27 c d h-18 a f h+9 e f h-3 a^2 i-18 b d i+12 a e i-3 e^2 i+9 c g i+9 f h i-3 a i^2-3 e i^2+2 i^3)^2))^(1/3))-(2 a^3+9 a b d-3 a^2 e+9 b d e-3 a e^2+2 e^3+9 a c g-18 c e g+27 b f g+27 c d h-18 a f h+9 e f h-3 a^2 i-18 b d i+12 a e i-3 e^2 i+9 c g i+9 f h i-3 a i^2-3 e i^2+2 i^3+sqrt(4 (-a^2-3 b d+a e-e^2-3 c g-3 f h+a i+e i-i^2)^3+(2 a^3+9 a b d-3 a^2 e+9 b d e-3 a e^2+2 e^3+9 a c g-18 c e g+27 b f g+27 c d h-18 a f h+9 e f h-3 a^2 i-18 b d i+12 a e i-3 e^2 i+9 c g i+9 f h i-3 a i^2-3 e i^2+2 i^3)^2))^(1/3)/(3 2^(1/3)))/g+(h (-f g+d i-d (1/3 (a+e+i)-(2^(1/3) (-a^2-3 b d+a e-e^2-3 c g-3 f h+a i+e i-i^2))/(3 (2 a^3+9 a b d-3 a^2 e+9 b d e-3 a e^2+2 e^3+9 a c g-18 c e g+27 b f g+27 c d h-18 a f h+9 e f h-3 a^2 i-18 b d i+12 a e i-3 e^2 i+9 c g i+9 f h i-3 a i^2-3 e i^2+2 i^3+sqrt(4 (-a^2-3 b d+a e-e^2-3 c g-3 f h+a i+e i-i^2)^3+(2 a^3+9 a b d-3 a^2 e+9 b d e-3 a e^2+2 e^3+9 a c g-18 c e g+27 b f g+27 c d h-18 a f h+9 e f h-3 a^2 i-18 b d i+12 a e i-3 e^2 i+9 c g i+9 f h i-3 a i^2-3 e i^2+2 i^3)^2))^(1/3))+(2 a^3+9 a b d-3 a^2 e+9 b d e-3 a e^2+2 e^3+9 a c g-18 c e g+27 b f g+27 c d h-18 a f h+9 e f h-3 a^2 i-18 b d i+12 a e i-3 e^2 i+9 c g i+9 f h i-3 a i^2-3 e i^2+2 i^3+sqrt(4 (-a^2-3 b d+a e-e^2-3 c g-3 f h+a i+e i-i^2)^3+(2 a^3+9 a b d-3 a^2 e+9 b d e-3 a e^2+2 e^3+9 a c g-18 c e g+27 b f g+27 c d h-18 a f h+9 e f h-3 a^2 i-18 b d i+12 a e i-3 e^2 i+9 c g i+9 f h i-3 a i^2-3 e i^2+2 i^3)^2))^(1/3)/(3 2^(1/3)))))/(g (-e g+d h+g (1/3 (a+e+i)-(2^(1/3) (-a^2-3 b d+a e-e^2-3 c g-3 f h+a i+e i-i^2))/(3 (2 a^3+9 a b d-3 a^2 e+9 b d e-3 a e^2+2 e^3+9 a c g-18 c e g+27 b f g+27 c d h-18 a f h+9 e f h-3 a^2 i-18 b d i+12 a e i-3 e^2 i+9 c g i+9 f h i-3 a i^2-3 e i^2+2 i^3+sqrt(4 (-a^2-3 b d+a e-e^2-3 c g-3 f h+a i+e i-i^2)^3+(2 a^3+9 a b d-3 a^2 e+9 b d e-3 a e^2+2 e^3+9 a c g-18 c e g+27 b f g+27 c d h-18 a f h+9 e f h-3 a^2 i-18 b d i+12 a e i-3 e^2 i+9 c g i+9 f h i-3 a i^2-3 e i^2+2 i^3)^2))^(1/3))+(2 a^3+9 a b d-3 a^2 e+9 b d e-3 a e^2+2 e^3+9 a c g-18 c e g+27 b f g+27 c d h-18 a f h+9 e f h-3 a^2 i-18 b d i+12 a e i-3 e^2 i+9 c g i+9 f h i-3 a i^2-3 e i^2+2 i^3+sqrt(4 (-a^2-3 b d+a e-e^2-3 c g-3 f h+a i+e i-i^2)^3+(2 a^3+9 a b d-3 a^2 e+9 b d e-3 a e^2+2 e^3+9 a c g-18 c e g+27 b f g+27 c d h-18 a f h+9 e f h-3 a^2 i-18 b d i+12 a e i-3 e^2 i+9 c g i+9 f h i-3 a i^2-3 e i^2+2 i^3)^2))^(1/3)/(3 2^(1/3)))));
//	//EigenVectors[0][1] = -(-f g+d i-d (1/3 (a+e+i)-(2^(1/3) (-a^2-3 b d+a e-e^2-3 c g-3 f h+a i+e i-i^2))/(3 (2 a^3+9 a b d-3 a^2 e+9 b d e-3 a e^2+2 e^3+9 a c g-18 c e g+27 b f g+27 c d h-18 a f h+9 e f h-3 a^2 i-18 b d i+12 a e i-3 e^2 i+9 c g i+9 f h i-3 a i^2-3 e i^2+2 i^3+sqrt(4 (-a^2-3 b d+a e-e^2-3 c g-3 f h+a i+e i-i^2)^3+(2 a^3+9 a b d-3 a^2 e+9 b d e-3 a e^2+2 e^3+9 a c g-18 c e g+27 b f g+27 c d h-18 a f h+9 e f h-3 a^2 i-18 b d i+12 a e i-3 e^2 i+9 c g i+9 f h i-3 a i^2-3 e i^2+2 i^3)^2))^(1/3))+(2 a^3+9 a b d-3 a^2 e+9 b d e-3 a e^2+2 e^3+9 a c g-18 c e g+27 b f g+27 c d h-18 a f h+9 e f h-3 a^2 i-18 b d i+12 a e i-3 e^2 i+9 c g i+9 f h i-3 a i^2-3 e i^2+2 i^3+sqrt(4 (-a^2-3 b d+a e-e^2-3 c g-3 f h+a i+e i-i^2)^3+(2 a^3+9 a b d-3 a^2 e+9 b d e-3 a e^2+2 e^3+9 a c g-18 c e g+27 b f g+27 c d h-18 a f h+9 e f h-3 a^2 i-18 b d i+12 a e i-3 e^2 i+9 c g i+9 f h i-3 a i^2-3 e i^2+2 i^3)^2))^(1/3)/(3 2^(1/3))))/(-e g+d h+g (1/3 (a+e+i)-(2^(1/3) (-a^2-3 b d+a e-e^2-3 c g-3 f h+a i+e i-i^2))/(3 (2 a^3+9 a b d-3 a^2 e+9 b d e-3 a e^2+2 e^3+9 a c g-18 c e g+27 b f g+27 c d h-18 a f h+9 e f h-3 a^2 i-18 b d i+12 a e i-3 e^2 i+9 c g i+9 f h i-3 a i^2-3 e i^2+2 i^3+sqrt(4 (-a^2-3 b d+a e-e^2-3 c g-3 f h+a i+e i-i^2)^3+(2 a^3+9 a b d-3 a^2 e+9 b d e-3 a e^2+2 e^3+9 a c g-18 c e g+27 b f g+27 c d h-18 a f h+9 e f h-3 a^2 i-18 b d i+12 a e i-3 e^2 i+9 c g i+9 f h i-3 a i^2-3 e i^2+2 i^3)^2))^(1/3))+(2 a^3+9 a b d-3 a^2 e+9 b d e-3 a e^2+2 e^3+9 a c g-18 c e g+27 b f g+27 c d h-18 a f h+9 e f h-3 a^2 i-18 b d i+12 a e i-3 e^2 i+9 c g i+9 f h i-3 a i^2-3 e i^2+2 i^3+sqrt(4 (-a^2-3 b d+a e-e^2-3 c g-3 f h+a i+e i-i^2)^3+(2 a^3+9 a b d-3 a^2 e+9 b d e-3 a e^2+2 e^3+9 a c g-18 c e g+27 b f g+27 c d h-18 a f h+9 e f h-3 a^2 i-18 b d i+12 a e i-3 e^2 i+9 c g i+9 f h i-3 a i^2-3 e i^2+2 i^3)^2))^(1/3)/(3 2^(1/3))));
//	//EigenVectors[0][2] = 1;
//	//EigenVectors[1][0] = (-(1/3 (-a-e-i)+i-((1+i sqrt(3)) (-a^2-3 b d+a e-e^2-3 c g-3 f h+a i+e i-i^2))/(3 2^(2/3) (2 a^3+9 a b d-3 a^2 e+9 b d e-3 a e^2+2 e^3+9 a c g-18 c e g+27 b f g+27 c d h-18 a f h+9 e f h-3 a^2 i-18 b d i+12 a e i-3 e^2 i+9 c g i+9 f h i-3 a i^2-3 e i^2+2 i^3+sqrt(4 (-a^2-3 b d+a e-e^2-3 c g-3 f h+a i+e i-i^2)^3+(2 a^3+9 a b d-3 a^2 e+9 b d e-3 a e^2+2 e^3+9 a c g-18 c e g+27 b f g+27 c d h-18 a f h+9 e f h-3 a^2 i-18 b d i+12 a e i-3 e^2 i+9 c g i+9 f h i-3 a i^2-3 e i^2+2 i^3)^2))^(1/3))+((1-i sqrt(3)) (2 a^3+9 a b d-3 a^2 e+9 b d e-3 a e^2+2 e^3+9 a c g-18 c e g+27 b f g+27 c d h-18 a f h+9 e f h-3 a^2 i-18 b d i+12 a e i-3 e^2 i+9 c g i+9 f h i-3 a i^2-3 e i^2+2 i^3+sqrt(4 (-a^2-3 b d+a e-e^2-3 c g-3 f h+a i+e i-i^2)^3+(2 a^3+9 a b d-3 a^2 e+9 b d e-3 a e^2+2 e^3+9 a c g-18 c e g+27 b f g+27 c d h-18 a f h+9 e f h-3 a^2 i-18 b d i+12 a e i-3 e^2 i+9 c g i+9 f h i-3 a i^2-3 e i^2+2 i^3)^2))^(1/3))/(6 2^(1/3)))/g+(h (-f g+d i-d (1/3 (a+e+i)+((1+i sqrt(3)) (-a^2-3 b d+a e-e^2-3 c g-3 f h+a i+e i-i^2))/(3 2^(2/3) (2 a^3+9 a b d-3 a^2 e+9 b d e-3 a e^2+2 e^3+9 a c g-18 c e g+27 b f g+27 c d h-18 a f h+9 e f h-3 a^2 i-18 b d i+12 a e i-3 e^2 i+9 c g i+9 f h i-3 a i^2-3 e i^2+2 i^3+sqrt(4 (-a^2-3 b d+a e-e^2-3 c g-3 f h+a i+e i-i^2)^3+(2 a^3+9 a b d-3 a^2 e+9 b d e-3 a e^2+2 e^3+9 a c g-18 c e g+27 b f g+27 c d h-18 a f h+9 e f h-3 a^2 i-18 b d i+12 a e i-3 e^2 i+9 c g i+9 f h i-3 a i^2-3 e i^2+2 i^3)^2))^(1/3))-((1-i sqrt(3)) (2 a^3+9 a b d-3 a^2 e+9 b d e-3 a e^2+2 e^3+9 a c g-18 c e g+27 b f g+27 c d h-18 a f h+9 e f h-3 a^2 i-18 b d i+12 a e i-3 e^2 i+9 c g i+9 f h i-3 a i^2-3 e i^2+2 i^3+sqrt(4 (-a^2-3 b d+a e-e^2-3 c g-3 f h+a i+e i-i^2)^3+(2 a^3+9 a b d-3 a^2 e+9 b d e-3 a e^2+2 e^3+9 a c g-18 c e g+27 b f g+27 c d h-18 a f h+9 e f h-3 a^2 i-18 b d i+12 a e i-3 e^2 i+9 c g i+9 f h i-3 a i^2-3 e i^2+2 i^3)^2))^(1/3))/(6 2^(1/3)))))/(g (-e g+d h+g (1/3 (a+e+i)+((1+i sqrt(3)) (-a^2-3 b d+a e-e^2-3 c g-3 f h+a i+e i-i^2))/(3 2^(2/3) (2 a^3+9 a b d-3 a^2 e+9 b d e-3 a e^2+2 e^3+9 a c g-18 c e g+27 b f g+27 c d h-18 a f h+9 e f h-3 a^2 i-18 b d i+12 a e i-3 e^2 i+9 c g i+9 f h i-3 a i^2-3 e i^2+2 i^3+sqrt(4 (-a^2-3 b d+a e-e^2-3 c g-3 f h+a i+e i-i^2)^3+(2 a^3+9 a b d-3 a^2 e+9 b d e-3 a e^2+2 e^3+9 a c g-18 c e g+27 b f g+27 c d h-18 a f h+9 e f h-3 a^2 i-18 b d i+12 a e i-3 e^2 i+9 c g i+9 f h i-3 a i^2-3 e i^2+2 i^3)^2))^(1/3))-((1-i sqrt(3)) (2 a^3+9 a b d-3 a^2 e+9 b d e-3 a e^2+2 e^3+9 a c g-18 c e g+27 b f g+27 c d h-18 a f h+9 e f h-3 a^2 i-18 b d i+12 a e i-3 e^2 i+9 c g i+9 f h i-3 a i^2-3 e i^2+2 i^3+sqrt(4 (-a^2-3 b d+a e-e^2-3 c g-3 f h+a i+e i-i^2)^3+(2 a^3+9 a b d-3 a^2 e+9 b d e-3 a e^2+2 e^3+9 a c g-18 c e g+27 b f g+27 c d h-18 a f h+9 e f h-3 a^2 i-18 b d i+12 a e i-3 e^2 i+9 c g i+9 f h i-3 a i^2-3 e i^2+2 i^3)^2))^(1/3))/(6 2^(1/3)))));
//	//EigenVectors[1][1] = -(-f g+d i-d (1/3 (a+e+i)+((1+i sqrt(3)) (-a^2-3 b d+a e-e^2-3 c g-3 f h+a i+e i-i^2))/(3 2^(2/3) (2 a^3+9 a b d-3 a^2 e+9 b d e-3 a e^2+2 e^3+9 a c g-18 c e g+27 b f g+27 c d h-18 a f h+9 e f h-3 a^2 i-18 b d i+12 a e i-3 e^2 i+9 c g i+9 f h i-3 a i^2-3 e i^2+2 i^3+sqrt(4 (-a^2-3 b d+a e-e^2-3 c g-3 f h+a i+e i-i^2)^3+(2 a^3+9 a b d-3 a^2 e+9 b d e-3 a e^2+2 e^3+9 a c g-18 c e g+27 b f g+27 c d h-18 a f h+9 e f h-3 a^2 i-18 b d i+12 a e i-3 e^2 i+9 c g i+9 f h i-3 a i^2-3 e i^2+2 i^3)^2))^(1/3))-((1-i sqrt(3)) (2 a^3+9 a b d-3 a^2 e+9 b d e-3 a e^2+2 e^3+9 a c g-18 c e g+27 b f g+27 c d h-18 a f h+9 e f h-3 a^2 i-18 b d i+12 a e i-3 e^2 i+9 c g i+9 f h i-3 a i^2-3 e i^2+2 i^3+sqrt(4 (-a^2-3 b d+a e-e^2-3 c g-3 f h+a i+e i-i^2)^3+(2 a^3+9 a b d-3 a^2 e+9 b d e-3 a e^2+2 e^3+9 a c g-18 c e g+27 b f g+27 c d h-18 a f h+9 e f h-3 a^2 i-18 b d i+12 a e i-3 e^2 i+9 c g i+9 f h i-3 a i^2-3 e i^2+2 i^3)^2))^(1/3))/(6 2^(1/3))))/(-e g+d h+g (1/3 (a+e+i)+((1+i sqrt(3)) (-a^2-3 b d+a e-e^2-3 c g-3 f h+a i+e i-i^2))/(3 2^(2/3) (2 a^3+9 a b d-3 a^2 e+9 b d e-3 a e^2+2 e^3+9 a c g-18 c e g+27 b f g+27 c d h-18 a f h+9 e f h-3 a^2 i-18 b d i+12 a e i-3 e^2 i+9 c g i+9 f h i-3 a i^2-3 e i^2+2 i^3+sqrt(4 (-a^2-3 b d+a e-e^2-3 c g-3 f h+a i+e i-i^2)^3+(2 a^3+9 a b d-3 a^2 e+9 b d e-3 a e^2+2 e^3+9 a c g-18 c e g+27 b f g+27 c d h-18 a f h+9 e f h-3 a^2 i-18 b d i+12 a e i-3 e^2 i+9 c g i+9 f h i-3 a i^2-3 e i^2+2 i^3)^2))^(1/3))-((1-i sqrt(3)) (2 a^3+9 a b d-3 a^2 e+9 b d e-3 a e^2+2 e^3+9 a c g-18 c e g+27 b f g+27 c d h-18 a f h+9 e f h-3 a^2 i-18 b d i+12 a e i-3 e^2 i+9 c g i+9 f h i-3 a i^2-3 e i^2+2 i^3+sqrt(4 (-a^2-3 b d+a e-e^2-3 c g-3 f h+a i+e i-i^2)^3+(2 a^3+9 a b d-3 a^2 e+9 b d e-3 a e^2+2 e^3+9 a c g-18 c e g+27 b f g+27 c d h-18 a f h+9 e f h-3 a^2 i-18 b d i+12 a e i-3 e^2 i+9 c g i+9 f h i-3 a i^2-3 e i^2+2 i^3)^2))^(1/3))/(6 2^(1/3))));
//	//EigenVectors[1][2] = 1;
//	//EigenVectors[2][0] = (-(1/3 (-a-e-i)+i-((1-i sqrt(3)) (-a^2-3 b d+a e-e^2-3 c g-3 f h+a i+e i-i^2))/(3 2^(2/3) (2 a^3+9 a b d-3 a^2 e+9 b d e-3 a e^2+2 e^3+9 a c g-18 c e g+27 b f g+27 c d h-18 a f h+9 e f h-3 a^2 i-18 b d i+12 a e i-3 e^2 i+9 c g i+9 f h i-3 a i^2-3 e i^2+2 i^3+sqrt(4 (-a^2-3 b d+a e-e^2-3 c g-3 f h+a i+e i-i^2)^3+(2 a^3+9 a b d-3 a^2 e+9 b d e-3 a e^2+2 e^3+9 a c g-18 c e g+27 b f g+27 c d h-18 a f h+9 e f h-3 a^2 i-18 b d i+12 a e i-3 e^2 i+9 c g i+9 f h i-3 a i^2-3 e i^2+2 i^3)^2))^(1/3))+((1+i sqrt(3)) (2 a^3+9 a b d-3 a^2 e+9 b d e-3 a e^2+2 e^3+9 a c g-18 c e g+27 b f g+27 c d h-18 a f h+9 e f h-3 a^2 i-18 b d i+12 a e i-3 e^2 i+9 c g i+9 f h i-3 a i^2-3 e i^2+2 i^3+sqrt(4 (-a^2-3 b d+a e-e^2-3 c g-3 f h+a i+e i-i^2)^3+(2 a^3+9 a b d-3 a^2 e+9 b d e-3 a e^2+2 e^3+9 a c g-18 c e g+27 b f g+27 c d h-18 a f h+9 e f h-3 a^2 i-18 b d i+12 a e i-3 e^2 i+9 c g i+9 f h i-3 a i^2-3 e i^2+2 i^3)^2))^(1/3))/(6 2^(1/3)))/g+(h (-f g+d i-d (1/3 (a+e+i)+((1-i sqrt(3)) (-a^2-3 b d+a e-e^2-3 c g-3 f h+a i+e i-i^2))/(3 2^(2/3) (2 a^3+9 a b d-3 a^2 e+9 b d e-3 a e^2+2 e^3+9 a c g-18 c e g+27 b f g+27 c d h-18 a f h+9 e f h-3 a^2 i-18 b d i+12 a e i-3 e^2 i+9 c g i+9 f h i-3 a i^2-3 e i^2+2 i^3+sqrt(4 (-a^2-3 b d+a e-e^2-3 c g-3 f h+a i+e i-i^2)^3+(2 a^3+9 a b d-3 a^2 e+9 b d e-3 a e^2+2 e^3+9 a c g-18 c e g+27 b f g+27 c d h-18 a f h+9 e f h-3 a^2 i-18 b d i+12 a e i-3 e^2 i+9 c g i+9 f h i-3 a i^2-3 e i^2+2 i^3)^2))^(1/3))-((1+i sqrt(3)) (2 a^3+9 a b d-3 a^2 e+9 b d e-3 a e^2+2 e^3+9 a c g-18 c e g+27 b f g+27 c d h-18 a f h+9 e f h-3 a^2 i-18 b d i+12 a e i-3 e^2 i+9 c g i+9 f h i-3 a i^2-3 e i^2+2 i^3+sqrt(4 (-a^2-3 b d+a e-e^2-3 c g-3 f h+a i+e i-i^2)^3+(2 a^3+9 a b d-3 a^2 e+9 b d e-3 a e^2+2 e^3+9 a c g-18 c e g+27 b f g+27 c d h-18 a f h+9 e f h-3 a^2 i-18 b d i+12 a e i-3 e^2 i+9 c g i+9 f h i-3 a i^2-3 e i^2+2 i^3)^2))^(1/3))/(6 2^(1/3)))))/(g (-e g+d h+g (1/3 (a+e+i)+((1-i sqrt(3)) (-a^2-3 b d+a e-e^2-3 c g-3 f h+a i+e i-i^2))/(3 2^(2/3) (2 a^3+9 a b d-3 a^2 e+9 b d e-3 a e^2+2 e^3+9 a c g-18 c e g+27 b f g+27 c d h-18 a f h+9 e f h-3 a^2 i-18 b d i+12 a e i-3 e^2 i+9 c g i+9 f h i-3 a i^2-3 e i^2+2 i^3+sqrt(4 (-a^2-3 b d+a e-e^2-3 c g-3 f h+a i+e i-i^2)^3+(2 a^3+9 a b d-3 a^2 e+9 b d e-3 a e^2+2 e^3+9 a c g-18 c e g+27 b f g+27 c d h-18 a f h+9 e f h-3 a^2 i-18 b d i+12 a e i-3 e^2 i+9 c g i+9 f h i-3 a i^2-3 e i^2+2 i^3)^2))^(1/3))-((1+i sqrt(3)) (2 a^3+9 a b d-3 a^2 e+9 b d e-3 a e^2+2 e^3+9 a c g-18 c e g+27 b f g+27 c d h-18 a f h+9 e f h-3 a^2 i-18 b d i+12 a e i-3 e^2 i+9 c g i+9 f h i-3 a i^2-3 e i^2+2 i^3+sqrt(4 (-a^2-3 b d+a e-e^2-3 c g-3 f h+a i+e i-i^2)^3+(2 a^3+9 a b d-3 a^2 e+9 b d e-3 a e^2+2 e^3+9 a c g-18 c e g+27 b f g+27 c d h-18 a f h+9 e f h-3 a^2 i-18 b d i+12 a e i-3 e^2 i+9 c g i+9 f h i-3 a i^2-3 e i^2+2 i^3)^2))^(1/3))/(6 2^(1/3)))));
//	//EigenVectors[2][1] = -(-f g+d i-d (1/3 (a+e+i)+((1-i sqrt(3)) (-a^2-3 b d+a e-e^2-3 c g-3 f h+a i+e i-i^2))/(3 2^(2/3) (2 a^3+9 a b d-3 a^2 e+9 b d e-3 a e^2+2 e^3+9 a c g-18 c e g+27 b f g+27 c d h-18 a f h+9 e f h-3 a^2 i-18 b d i+12 a e i-3 e^2 i+9 c g i+9 f h i-3 a i^2-3 e i^2+2 i^3+sqrt(4 (-a^2-3 b d+a e-e^2-3 c g-3 f h+a i+e i-i^2)^3+(2 a^3+9 a b d-3 a^2 e+9 b d e-3 a e^2+2 e^3+9 a c g-18 c e g+27 b f g+27 c d h-18 a f h+9 e f h-3 a^2 i-18 b d i+12 a e i-3 e^2 i+9 c g i+9 f h i-3 a i^2-3 e i^2+2 i^3)^2))^(1/3))-((1+i sqrt(3)) (2 a^3+9 a b d-3 a^2 e+9 b d e-3 a e^2+2 e^3+9 a c g-18 c e g+27 b f g+27 c d h-18 a f h+9 e f h-3 a^2 i-18 b d i+12 a e i-3 e^2 i+9 c g i+9 f h i-3 a i^2-3 e i^2+2 i^3+sqrt(4 (-a^2-3 b d+a e-e^2-3 c g-3 f h+a i+e i-i^2)^3+(2 a^3+9 a b d-3 a^2 e+9 b d e-3 a e^2+2 e^3+9 a c g-18 c e g+27 b f g+27 c d h-18 a f h+9 e f h-3 a^2 i-18 b d i+12 a e i-3 e^2 i+9 c g i+9 f h i-3 a i^2-3 e i^2+2 i^3)^2))^(1/3))/(6 2^(1/3))))/(-e g+d h+g (1/3 (a+e+i)+((1-i sqrt(3)) (-a^2-3 b d+a e-e^2-3 c g-3 f h+a i+e i-i^2))/(3 2^(2/3) (2 a^3+9 a b d-3 a^2 e+9 b d e-3 a e^2+2 e^3+9 a c g-18 c e g+27 b f g+27 c d h-18 a f h+9 e f h-3 a^2 i-18 b d i+12 a e i-3 e^2 i+9 c g i+9 f h i-3 a i^2-3 e i^2+2 i^3+sqrt(4 (-a^2-3 b d+a e-e^2-3 c g-3 f h+a i+e i-i^2)^3+(2 a^3+9 a b d-3 a^2 e+9 b d e-3 a e^2+2 e^3+9 a c g-18 c e g+27 b f g+27 c d h-18 a f h+9 e f h-3 a^2 i-18 b d i+12 a e i-3 e^2 i+9 c g i+9 f h i-3 a i^2-3 e i^2+2 i^3)^2))^(1/3))-((1+i sqrt(3)) (2 a^3+9 a b d-3 a^2 e+9 b d e-3 a e^2+2 e^3+9 a c g-18 c e g+27 b f g+27 c d h-18 a f h+9 e f h-3 a^2 i-18 b d i+12 a e i-3 e^2 i+9 c g i+9 f h i-3 a i^2-3 e i^2+2 i^3+sqrt(4 (-a^2-3 b d+a e-e^2-3 c g-3 f h+a i+e i-i^2)^3+(2 a^3+9 a b d-3 a^2 e+9 b d e-3 a e^2+2 e^3+9 a c g-18 c e g+27 b f g+27 c d h-18 a f h+9 e f h-3 a^2 i-18 b d i+12 a e i-3 e^2 i+9 c g i+9 f h i-3 a i^2-3 e i^2+2 i^3)^2))^(1/3))/(6 2^(1/3))));
//	//EigenVectors[2][2] = 1;
//
//	return true;
//}
