//+-----------------------------------------------------------------------------+
//| Copyleft (C) 2025 REGEO (http://www.regeo.uji.es/)
//|
//| This file is free software
//| You can redistribute it and/or modify it under the terms of the GNU General Public License,
//| as published by the Free Software Foundation.
//| 
//| This program is distributed in the hope that it will be useful,
//| but WITHOUT ANY WARRANTY; without even the implied warranty of
//| MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//+----------------------------------------------------------------------------*/

// The first approach is a C++ version of the Corner Finder by Xiong and LaViola
//		Xiong Y. and LaViola J.J.
//		A ShortStraw-based algoritm for corner finding in sketcth-based interfaces
//		Computers&Graphics 34 (2010) 513-527
//      http://www.eecs.ucf.edu/isuelab/downloads.php

// *
// * Copyright 2010, Yiyan Xiong and University of Central Florida, Orlando, FL.
// * 
// *                           All Rights Reserved
// * 
// * Permission to use, copy, modify, and distribute this software and its
// * documentation for any purpose other than its incorporation into a commercial
// * product is hereby granted without fee, provided that the above copyright
// * notice appear in all copies and that both that copyright notice and this 
// * permission notice appear in supporting documentation, and that the name 
// * of Brown University not be used in advertising or publicity pertaining to 
// * distribution of the software without specific, written prior permission.
// * 
// * UNIVERSITY OF CENTRAL FLORIDA DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS 
// * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
// * FOR ANY PARTICULAR PURPOSE.  IN NO EVENT SHALL BROWN UNIVERSITY BE LIABLE 
// * FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
// * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
// * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
// * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
// * 

//The second approach is based on:
//		Asif Masood, M. Sarfraz
//		Corner detection by sliding rectangles along planar curves
//		Computers & Graphics 31 (2007) 440–448

//		Sarfraz M., Swati Z.N.K., Zaman S.
//		A novel corner detector approach using sliding ellipses
//		2009 Sixth International Conference on Computer Graphics, Imaging and Visualization

//		Zar Nawab Khan Swati, Safdar Zaman, Muhammad Sarfraz
//		A novel corner detector approach using sliding
//		two ellipses and one rectangle
//		7th International Conference on Frontiers of Information Technology, 2009
//		DOI: 10.1145/1838002.1838086


//The mapping strategy to map the corners from the resampled nodes space
//into the original stroke space is an original contribution of P.Company, 2013.


#ifndef STROKECORNERFINDER_VISITED
	#define STROKECORNERFINDER_VISITED


	class CResampleStroke
	//Resample strokes into point sets with constant inter-distance
	{
		private:
			long DIAGONAL_INTERVAL;

		public:
			CResampleStroke();
			~CResampleStroke();

			std::vector <POINT2D> GetResampledStroke(std::vector <POINT2D> Nodes,
													 std::vector <long> &MappedNodes);  //pcompany

			std::vector <POINT2D> GetResampledStroke(int pass,
													 std::vector <POINT2D> Nodes,
													 std::vector <long> &MappedNodes,  //pcompany
													 bool UseTime,
													 std::vector <double> Times,
													 std::vector <double> &ResampledTimes,
													 double &meanTime);
	};


//////////////////////////////////////////////////////////////////////////////////
	//iStraw approach

	class CIStraw
	{
		private:
			long STRAW_WINDOW;
			double MEDIAN_THRESHOLD;
			double LINE_THRESHOLD;

			bool SameDirection(POINT2D O, POINT2D A, POINT2D B, POINT2D C, POINT2D D);
			double GetAngle(POINT2D center, POINT2D p1, POINT2D p2);
			int HalfwayCorner(std::vector <double> straws, int c1, int c2);
			bool IsLine(int x, int y, double threshold,
						std::vector <POINT2D> ResampledNodes);

			// polyline post process the inital corner set
			void AddCorners_Collinear(std::vector <double> straws,
									  std::vector <POINT2D> ResampledNodes,
									  std::vector <long> &cornerIndex);
			void AddCorners_Time(std::vector <double> ResampledTimes, double meanTime,
								 std::vector <long> &cornerIndex);
			void AdjustCorners(std::vector <POINT2D> ResampledNodes,
							   std::vector <long> &cornerIndex);
			void TriCollinearPass0(std::vector <POINT2D> ResampledNodes,
								   std::vector <long> &cornerIndex);
			void TriCollinearPass1(std::vector <POINT2D> ResampledNodes,
								   bool UseTime,
								   std::vector <double> ResampledTimes,
								   double meanTime,
								   std::vector <long> &cornerIndex);
			void SharpNoiseAvoid(std::vector <double> straws,
								 std::vector <long> &cornerIndex);
			void PostProcess(std::vector <double> straws,
							 std::vector <POINT2D> ResampledNodes,
							 bool UseTime,
							 std::vector <double> ResampledTimes,
							 double meanTime,
							 std::vector <long> &cornerIndex);

			// stroke curve part process
			void CurvProcessPass0(std::vector <POINT2D> ResampledNodes,
								  std::vector <long> &cornerIndex);
			void CurvProcessPass1(std::vector <POINT2D> ResampledNodes,
								  std::vector <long> &cornerIndex);

			// get all the corners of a stroke
			void GetCorners(std::vector <POINT2D> ResampledNodes,
							bool UseTime,
							std::vector <double> ResampledTimes, double meanTime,
							std::vector <long> &cornerIndex);

		public:
			CIStraw();
			~CIStraw();

			//Find all the corners of a stroke
			void FindIStrawCorners(std::vector <POINT2D> Nodes, // input the sequence of points of the stroke
								   std::vector <double> Times,       // input the sequence of times between consecutive points
								   //std::vector <POINT2D> &Corners,  // output the positions of the corners in the sequence
								   std::vector <long> &positions);  // output the nodes where the corners happen   //pcompany
	};


//////////////////////////////////////////////////////////////////////////////////
	//Sliding Strips approach

	class Strips
	{
		private:
			void GetParams(double CellSize, double &A, double &B, double &nu, long &k);
			double GetSlope(std::vector <POINT2D> Nodes, long ipoint, long k);
			bool IsPointInRectangle(POINT2D Point,
									POINT2D Centroid, double Length, double Height,
									double rotation);
			bool IsPointInEllipse(POINT2D Point,
								  POINT2D Centroid, double A, double B, double angle);

		public:
			Strips();
			~Strips();

			void GetStrip(std::vector <POINT2D> Nodes,  // input the sequence of nodes of the stroke
						  long StripType,  // input the strip type (S1, S2 or S3)
						  long ipoint,
						  double &A, double &B, double &Angle);  // output the parameters of the strip

			//Find all the corners of a stroke
			void FindSSCorners(std::vector <POINT2D> Nodes,  // input the sequence of nodes of the stroke
							   //std::vector <POINT2D> &Corners,    // output the positions of the corners in the sequence
							   std::vector <long> &positions);  // output the nodes where the corners happen   //pcompany
	};


//////////////////////////////////////////////////////////////////////////////////


	class CStrokeCornerFinder
	{
		private:
			//Find the nodes of the original stroke that best fit
			//the corners obtained from resampled nodes
			void CStrokeCornerFinder::RefineCorners(std::vector <POINT2D> Nodes,
													std::vector <long> &positions);
			//void FilterCollinearCorners(std::vector <POINT2D> Nodes,
			//							std::vector <long> &positions);

		public:
			CStrokeCornerFinder();
			~CStrokeCornerFinder();

			//Find all the corners of a stroke
			void FindStrokeCorners(CStroke InputStroke,    // input the sequence of points of the stroke
								   std::vector <long> &positions,  // output the positions of the corners in the sequence
								   long StrokeCornersApproach);
	};
#endif