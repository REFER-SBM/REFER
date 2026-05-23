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

#ifndef TOOLS_MATRIX_VISITED
	#define TOOLS_MATRIX_VISITED
	// MATRIX MANAGEMENT
	std::vector<std::vector<double>> MTranspose(std::vector<std::vector<double>> M);
	//std::vector<std::vector<double>> MInvert3x3(std::vector<std::vector<double>> M);
	std::vector<std::vector<double>> MScale(std::vector<std::vector<double>> M, double Scale);
	std::vector<std::vector<double>> MAdd(std::vector<std::vector<double>> M1,
										  std::vector<std::vector<double>> M2);
	std::vector<std::vector<double>> MSubtract(std::vector<std::vector<double>> M1,
											   std::vector<std::vector<double>> M2);
	std::vector<std::vector<double>> MProduct(std::vector<std::vector<double>> M1,
											  std::vector<std::vector<double>> M2);
	std::vector<std::vector<double>> MConcatenateRows(std::vector<std::vector<double>> M1,
													 std::vector<std::vector<double>> M2);
	std::vector<std::vector<double>> MConcatenateColumns(std::vector<std::vector<double>> M1,
														 std::vector<std::vector<double>> M2);
	void MClear(std::vector<std::vector<double>> &M);

	double norm_2(std::vector<double> x);
	double norm_2(std::vector<std::vector<double>>);
	double MDeterminant(std::vector<std::vector<double>> M);
	std::vector<std::vector<double>> MCoFactor(std::vector<std::vector<double>> M);
	std::vector<std::vector<double>> MInverse(std::vector<std::vector<double>> M);

	std::vector<std::vector<double>> MPseudoInverse(std::vector<std::vector<double>> M,
													double cutoff);
	std::vector<std::vector<double>> MLeftDivide(std::vector<std::vector<double>> A,
												 std::vector<std::vector<double>> B);

	//----------------------------------------------------------------------
	// LU decomposition

	class LU{
		public:
			long n;
			std::vector<std::vector<double>> lu;
		private:
			std::vector<int> indx;
			double d;
			//std::vector<std::vector<double>> &aref;

		public:
			LU(std::vector<std::vector<double>> &a);

			std::vector<std::vector<double>> solve(std::vector<std::vector<double>> B);
			//void solve(std::vector<std::vector<double>> &b,
			//		   std::vector<std::vector<double>> &x);
			//void inverse(std::vector<std::vector<double>> &ainv);
			//double det();
			//void mprove(std::vector<double> &b, std::vector<double> &x);
	};

	//----------------------------------------------------------------------
	// singular-value decomposition (SVD)

	class SVD{
		public:
			std::vector<std::vector<double>> u,v;
			std::vector<double> w;
		private:
			long m,n;
			double eps, tsh;

			//long rank(double thresh);
			//long nullity(double thresh);
			//std::vector<std::vector<double>> range(double thresh);
			//std::vector<std::vector<double>> nullspace(double thresh);

			//double inv_condition() {
			//	return (w[0] <= 0. || w[n-1] <= 0.) ? 0. : w[n-1]/w[0];
			//}

			double pythag(const double a, const double b);
			void decompose();
			void reorder();

		public:
			SVD(std::vector<std::vector<double>> &a);

			//void solve(std::vector<double> b,
			//		   std::vector<double> &x,
			//		   double thresh);
			//void solve(std::vector<std::vector<double>> b,
			//		   std::vector<std::vector<double>> &x,
			//		   double thresh);

	};

	//----------------------------------------------------------------------
	// EIGENVALUES AND EIGENVECTORS
	class Eigen{
		private:
			void balance(std::vector<std::vector<double>> &M, std::vector<double> &scale);
			void elmhes(std::vector<std::vector<double>> &M, std::vector<long> &perm);
			void eltran(std::vector<std::vector<double>> M, std::vector<long> perm,
						std::vector<std::vector<double>> &zz);
			bool hqr2(std::vector<std::vector<double>> &M,
					  std::vector<std::vector<double>> &zz,
					  std::vector<std::complex<double>> &wri);
			void balbak(std::vector<std::vector<double>> &zz, std::vector<double> scale);
			void sortvecs(std::vector<std::vector<double>> &zz, std::vector<std::complex<double>> &wri);

		public:
			bool UnsymEigen(std::vector<std::vector<double>> M,
							std::vector<std::complex<double>> &eigenvalues,
							std::vector<std::vector<double>> &EigenVectors);
			//bool Jacobi(std::vector<std::vector<double>> Matrix,
			//			std::vector<std::vector<double>> &eigenvalues,
			//			std::vector<std::vector<double>> &EigenVectors);
			//bool Eigen3x3(std::vector<std::vector<double>> Matrix,
			//			  std::vector<std::vector<double>> &eigenvalues,
			//			  std::vector<std::vector<double>> &EigenVectors);
	};

	//----------------------------------------------------------------------
#endif  //TOOLS_MATRIX_VISITED
