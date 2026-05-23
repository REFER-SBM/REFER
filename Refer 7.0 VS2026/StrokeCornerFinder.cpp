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
//back into the original stroke space is an original contribution of P.Company, 2013.

#include <stdafx.h>
#include <cmath>
#include <cassert>
#include <vector>

#include "Tools_Geometry.h"
#include "DB_Model.h"

#include "DB_Sketch.h"
#include "StrokeCornerFinder.h"


CResampleStroke::CResampleStroke()
{
	//Optimizing resampling parameters is essential since
	//corner finders depend heavily on the resampling

	DIAGONAL_INTERVAL= 40;
}


CResampleStroke::~CResampleStroke()
{
}


std::vector <POINT2D> CResampleStroke::GetResampledStroke(std::vector <POINT2D> Nodes,
														  std::vector <long> &MappedNodes)  //pcompany
//Returns the nodes of a stroke resampled with constant inter-distance

//As suggested by Kim and Kim (DOI 10.1016/j.cad.2005.10.006)
//this mapping allows for the curvature calculation 
//to be defined as the direction change at a given point

//Based on:
//Wolin A., Eoff B., Hammond T.
//Shortstraw: a simple and effective corner finder for polylines.
//SBIM 2008, pp. 33-40.
{
	//Reset the resampled points
	std::vector <POINT2D> ResampledNodes;

	//Determine resample spacing
	RECTANGLE2D bound = GetBounds(Nodes);
	double deltaX = bound.right - bound.left;
	double deltaY = bound.top - bound.bottom;
	double CellSize= sqrt(deltaX * deltaX + deltaY * deltaY)/ DIAGONAL_INTERVAL;

	//The first resampled point is the start point of the stroke
	ResampledNodes.push_back(Nodes[0]);
	MappedNodes.push_back(0);  //pcompany

	//Compute other resampled points
	double length = 0;
	int preIndex = 0;
	int numBetween = 0;
	for (long i = 1; i <(long)Nodes.size(); i++){
		numBetween++;
		// compute the distance between adjacent two raw points in the stroke
		double dist= MODULE_2D(Nodes[i-1], Nodes[i]);

		// check the distance between the last resampled point and the visiting raw points
		if (length + dist >= CellSize){
			// add a new point to the resampled points
			POINT2D newPoint;
			POINT2D preP= Nodes[i-1];
			POINT2D curP= Nodes[i];

			// compute the position of the new resampled point
			newPoint.x = preP.x + ((CellSize - length) / dist) * (curP.x - preP.x);
			newPoint.y = preP.y + ((CellSize - length) / dist) * (curP.y - preP.y);
			ResampledNodes.push_back(newPoint);
			MappedNodes.push_back(i);  //pcompany

			// compute the distance from the new resampled point to the raw point i
			length = MODULE_2D(newPoint, curP);

			// add extra resampled points between newPoint and raw point i
			preIndex = i;
			numBetween = 0;

			// make sure length is less than CellSize
			POINT2D newPointR= newPoint;
			while (length > CellSize){
				// find all possible resampled points before i
				newPointR.x += (CellSize / dist) * (curP.x - preP.x);
				newPointR.y += (CellSize / dist) * (curP.y - preP.y);
				ResampledNodes.push_back(newPointR);
				MappedNodes.push_back(i-1);  //pcompany
				length -= CellSize;
			}
		}
		else{
			// get the last resampled point
			length += dist;
			//if ((length > CellSize) && (i == Nodes.size()-1)){ //This was wrong in the original implementation
			if ((length > CellSize*0.4) && (i == Nodes.size()-1)){
				ResampledNodes.push_back(Nodes[i]);
				MappedNodes.push_back(i);  //pcompany
			}
		}
	}
	return ResampledNodes;
}


std::vector <POINT2D> CResampleStroke::GetResampledStroke(int pass,
														  std::vector <POINT2D> Nodes,
														  std::vector <long> &MappedNodes,  //pcompany
														  bool UseTime,
														  std::vector <double> Times,
														  std::vector <double> &ResampledTimes,
														  double &meanTime)

//Returns the nodes of a stroke resampled with constant inter-distance

//Computes as well the time for each resampled point
{
	std::vector <POINT2D> ResampledNodes;

	//Compute the distance between resampled poits
	RECTANGLE2D bound = GetBounds(Nodes);
	double deltaX = bound.right - bound.left;
	double deltaY = bound.top - bound.bottom;
	double CellSize= sqrt(deltaX * deltaX + deltaY * deltaY) / DIAGONAL_INTERVAL;

	// pass 0: the first resampled point is the start point of the stroke
	if (pass == 0){
		ResampledNodes.push_back(Nodes[0]);
		MappedNodes.push_back(0);  //pcompany
		if(UseTime){
			ResampledTimes.push_back(Times[0]);
		}
	}

	// compute other resampled points
	meanTime = 0;
	int numBetween = 0;
	double length = 0;
	int preIndex = 0;

	for (int i = 1; i <(long)Nodes.size(); i++){
		numBetween++;
		// compute the distance between adjacent two raw points in the stroke
		double dist= MODULE_2D(Nodes[i-1], Nodes[i]);

		// check the distance between the last resampled point and the visiting raw points
		if (length + dist >= CellSize ||
			(pass == 1 && ResampledNodes.size() == 0 && length + dist >= CellSize/2.0)){
			// add a new point to the resampled points
			POINT2D newPoint;
			POINT2D preP= Nodes[i-1];
			POINT2D curP= Nodes[i];

			// compute the position of the new resampled point
			if (pass == 1 && ResampledNodes.size() == 0){
				// first shifted resampled point
				newPoint.x = preP.x + ((CellSize/2.0 - length) / dist) * (curP.x - preP.x);
				newPoint.y = preP.y + ((CellSize/2.0 - length) / dist) * (curP.y - preP.y);
			}
			else{
				// other resampled points
				newPoint.x = preP.x + ((CellSize - length) / dist) * (curP.x - preP.x);
				newPoint.y = preP.y + ((CellSize - length) / dist) * (curP.y - preP.y);
			}
			ResampledNodes.push_back(newPoint);
			MappedNodes.push_back(i);  //pcompany
			// compute the distance from the new resampled point to the raw point i
			length = MODULE_2D(newPoint, curP);

			// compute time for each resampled point
			if(UseTime){
				long numS= (long)ResampledNodes.size()-1;
				ResampledTimes.push_back(Times[i] - Times[preIndex]);
				if (ResampledTimes[numS] < 0)
					ResampledTimes[numS] = 0;
				// compute the mean time of the resampled points
				meanTime += ResampledTimes[numS];
			}

			// add extra resampled points between newPoint and raw point i
			preIndex = i;
			numBetween = 0;

			// make sure length is less than CellSize
			POINT2D newPointR= newPoint;
			while (length > CellSize){
				// find all possible resampled points before i
				newPointR.x += (CellSize / dist) * (curP.x - preP.x);
				newPointR.y += (CellSize / dist) * (curP.y - preP.y);
				ResampledNodes.push_back(newPointR);
				MappedNodes.push_back(i);  //pcompany
				length -= CellSize;
				// compute time for each resampled point
				if(UseTime){
					ResampledTimes.push_back(Times[i] - Times[i-1]);
					long numS= (long)ResampledTimes.size()-1;
					if (ResampledTimes[numS] < 0)
						ResampledTimes[numS] = 0;
					// compute the mean time of the resampled points
					meanTime += ResampledTimes[numS];
				}
			}
		}
		else{
			// get the last resampled point
			length += dist;
			//if ((length > CellSize) && (i == Nodes.size()-1)){ //This was wrong in the original implementation
			if ((length > CellSize*0.4) && (i == Nodes.size()-1)){
				ResampledNodes.push_back(Nodes[i]);
				MappedNodes.push_back(i);  //pcompany
				if(UseTime){
					ResampledTimes.push_back(Times[i] - Times[preIndex]);
				}
			}
		}
	}

	// compute mean time of resampled point
	if(UseTime){
		meanTime /= ResampledNodes.size();
	}

	return ResampledNodes;
}



//////////////////////////////////////////////////////////////////////////////////


CIStraw::CIStraw()
{
	//As explained in DOI 10.1016/j.cag.2010.06.008,
	//the corner finder depends heavily on the parameters 
	//used in the algorithm.
	//Optimizing these parameters
	//is essential to the accuracy of the system

	STRAW_WINDOW= 3;
	MEDIAN_THRESHOLD= 0.95;
	LINE_THRESHOLD= 0.988;
}


CIStraw::~CIStraw()
{
}


bool CIStraw::SameDirection(POINT2D O, POINT2D A, POINT2D B, POINT2D C, POINT2D D)
// check whether the rotate direction is the same
{
	// get rotate direction
	double cross0 = (A.x - O.x)*(O.y - B.y) - (A.y - O.y)*(O.x - B.x);
	double cross1 = (C.x - O.x)*(O.y - D.y) - (C.y - O.y)*(O.x - D.x);

	// check whether the two direction are the same or not
	return (cross0 * cross1 > 0);
}


double CIStraw::GetAngle(POINT2D center, POINT2D p1, POINT2D p2)
//Returns the angle (in degrees) of p1 to center to p2
{
	p1-= center;
	p2-= center;
	double norm_cross= p1.x*p2.y-p1.y*p2.x;
	double dot= p1.x*p2.x+p1.y*p2.y;
	return RADIANS_TO_DEGREES(std::fabs(atan2(norm_cross, dot)));
}


int CIStraw::HalfwayCorner(std::vector <double> straws, int c1, int c2)
// function to find the point with smallest straw
{
	// compute the quarter value
	int quarter = (c2 - c1) / 4;
	if (quarter == 0){
		quarter = 1;
	}
	// get the least straw
	double minValue = 10000;
	int minIndex = c1 + 1;
	for (int i = c1 + quarter; i < c2 - quarter; i++){
		// compare with the least straw
		if (straws[i] < minValue){
			minValue = straws[i];
			minIndex = i;
		}
	}
	return minIndex;
}


bool CIStraw::IsLine(int c1, int c2, double threshold,
					 std::vector <POINT2D> ResampledNodes)
//Returns true if the segment of the stroke included between c1 and c2 is a straight line
{
	//compute the distance between two points
	double distance= MODULE_2D(ResampledNodes[c1], ResampledNodes[c2]);

	//compute the path between two points
	double pathDist = 0;
	for (int i = c1; i < c2; i++){
		pathDist += MODULE_2D(ResampledNodes[i], ResampledNodes[i+1]);
	}
	//compare with the threshold
	return (distance / pathDist > threshold);
}


void CIStraw::GetCorners(std::vector <POINT2D> ResampledNodes,
						 bool UseTime,
						 std::vector <double> ResampledTimes, double meanTime,
						 std::vector <long> &cornerIndex)
//Get all the corners of a stroke
{
	long numS= (long)ResampledNodes.size();

	int win = STRAW_WINDOW;

	double totalStraw = 0;						// the sum of all straws
	std::vector <double> straws(numS);			// array to store all straws

	//Add the first raw point index as the first corner index
	cornerIndex.clear();
	cornerIndex.push_back(0);

	//Compute the straw value for the first 3 and last 3 points
	straws[1] = MODULE_2D(ResampledNodes[0], ResampledNodes[1 + win]) * 2 * win / (win+1) ;
	straws[2] = MODULE_2D(ResampledNodes[0], ResampledNodes[2 + win]) * 2 * win / (win+2);
	straws[numS-2] = MODULE_2D(ResampledNodes[numS-1], ResampledNodes[numS-2-win]) * 2 * win / (win+1);
	straws[numS-3] = MODULE_2D(ResampledNodes[numS-1], ResampledNodes[numS-3-win]) * 2 * win / (win+2);

	//Compute the other straws
	for (int i = win; i < numS - win; i++) {
		// compute the distance of the window
		straws[i] = MODULE_2D(ResampledNodes[i-win], ResampledNodes[i+win]);
		totalStraw += straws[i];
	}

	//Compute the threshold for straws
	double threshold = totalStraw / (numS-2*win) * MEDIAN_THRESHOLD;

	//Compute the initial corner set
	for (int i = win; i < numS-win; i++){
		// compare each straw with the threshold
		if (straws[i] < threshold){
			// get the point with local minimum straw
			double localMin = 10000;     // POSITIVE_INFINITY
			int localMinIndex = i;
			while ((i < numS-win) && (straws[i] < threshold)){
				if (straws[i] < localMin){
				localMin = straws[i];
				localMinIndex = i;
			}
			i++;
		}
		// add the new corner index as the last corner index
		cornerIndex.push_back(localMinIndex);
		}
	}

	//Add the last raw point index as the last corner index
	cornerIndex.push_back(numS-1);

	//Stroke polyline part post process
	PostProcess(straws, ResampledNodes, UseTime, ResampledTimes, meanTime, cornerIndex);

	//Stroke curv part process
	CurvProcessPass0(ResampledNodes, cornerIndex);
	CurvProcessPass1(ResampledNodes, cornerIndex);
}


void CIStraw::AddCorners_Collinear(std::vector <double> in_straws,
								   std::vector <POINT2D> ResampledNodes,
								   std::vector <long> &cornerIndex)
// add missing corners by two adjacent collinear test
{
	// set a tag to check whether any two adjacent corners are in a line
	bool tag = false;
	while (!tag) {
		tag = true;
		for (int i = 1; i <(long)cornerIndex.size(); i++){
			// get the corner index
			int c1 = cornerIndex[i - 1];
			int c2 = cornerIndex[i];
			// add extra corner if the two adjacent corners are not on a line
			if (!IsLine(c1, c2, 0.975, ResampledNodes) && (c2 > c1+1)){
				// get the index of the new corner
				int newCorner = HalfwayCorner(in_straws, c1, c2);
				// insert the new corner index to the corner index list
				cornerIndex.insert (cornerIndex.begin()+i, newCorner);
				// change tag to false
				tag = false;
			}
		}
	}
}


void CIStraw::AddCorners_Time(std::vector <double> ResampledTimes, double meanTime,
							  std::vector <long> &cornerIndex)
// add missing corners by checking drawing time
{
	// check each pair of adjacent corners
	for (int i = 1; i <(long)cornerIndex.size(); i++){
		// get the corner index
		int c1 = cornerIndex[i-1];
		int c2 = cornerIndex[i];
		if (c2-c1 < 2*STRAW_WINDOW) continue;
		int localMaxIdx = c1 + STRAW_WINDOW;

		// find local maximum time
		double localMax = ResampledTimes[localMaxIdx];
		for (int j = c1 + STRAW_WINDOW; j <= c2-STRAW_WINDOW; j++){
			if (localMax < ResampledTimes[j]){
				localMax = ResampledTimes[j];
				localMaxIdx = j;
			}
		}

		// check whether the drawing speed is slow enough
		if (localMax > 2 * meanTime)
			cornerIndex.insert (cornerIndex.begin()+i, localMaxIdx);
	}
}


void CIStraw::AdjustCorners(std::vector <POINT2D> ResampledNodes,
							std::vector <long> &cornerIndex)
// adjust corners to precise resampled point
{
	// check each corner
	for (int i = 1; i <(long)cornerIndex.size() - 1; i++){
		int index = cornerIndex[i];
		if (index < 3 || index > (long)ResampledNodes.size() - 4)
			continue;

		// compute angles of adjacent resampled points
		double angle0 = GetAngle(ResampledNodes[index - 2], ResampledNodes[index - 3], ResampledNodes[index - 1]);
		double angle1 = GetAngle(ResampledNodes[index - 1], ResampledNodes[index - 2], ResampledNodes[index]);
		double angle2 = GetAngle(ResampledNodes[index], ResampledNodes[index - 1], ResampledNodes[index + 1]);
		double angle3 = GetAngle(ResampledNodes[index + 1], ResampledNodes[index], ResampledNodes[index + 2]);
		double angle4 = GetAngle(ResampledNodes[index + 2], ResampledNodes[index + 1], ResampledNodes[index + 3]);

		// adjust the corner point based upon the angles
		if (angle1 < angle3){
			if (angle0 < angle1 && angle0 < angle2)
				index -= 2;
			else if (angle1 < angle2)
				index--;
		}
		else{
			if (angle4 < angle3 && angle4 < angle2)
				index += 2;
			else if (angle3 < angle2)
				index++;
		}
		cornerIndex[i] = index;
	}
}


void CIStraw::TriCollinearPass0(std::vector <POINT2D> ResampledNodes,
								std::vector <long> &cornerIndex)
// triplet collinear test pass 0
{
	// check each corner
	for (int i = 1; i <(long)cornerIndex.size() - 1; i++){
		// get the corner index
		int c1 = cornerIndex[i - 1];
		int c2 = cornerIndex[i + 1];
		// check whether the three points is inline
		if (c1 == cornerIndex[i] || IsLine(c1, c2, LINE_THRESHOLD, ResampledNodes))
		//if (IsLine(c1, c2, 0.95))
		{
			// remove this corner index
			cornerIndex.erase(cornerIndex.begin()+i);
			i--;
		}
	}
}


void CIStraw::TriCollinearPass1(std::vector <POINT2D> ResampledNodes,
								bool UseTime,
								std::vector <double> ResampledTimes,
								double meanTime,
								std::vector <long> &cornerIndex)
// triplet collinear test pass 1
{
	// check each corner
	for (int i = 1; i <(long)cornerIndex.size() - 1; i++){
		// get the corner index
		int c1 = cornerIndex[i - 1];
		int c2 = cornerIndex[i + 1];
		int cIdx = cornerIndex[i];

		// set a dynamic threshold
		double threshold = 0.9747;
		if (c2 - c1 > 10)
			threshold += 0.0053;    // increase threshold with long the stroke segment
		if(UseTime){
			if (ResampledTimes[cIdx] > 2 * meanTime || ResampledTimes[cIdx - 1] > 2 * meanTime
				|| ResampledTimes[cIdx + 1] > 2 * meanTime)
				threshold += 0.0066;    // increase threshold if the adjacent points drawn slowly
		}

		// check whether the three points is inline
		if (IsLine(c1, c2, threshold, ResampledNodes)){
			// remove this corner index
			cornerIndex.erase(cornerIndex.begin()+i);
			i--;
		}
	}
}


void CIStraw::SharpNoiseAvoid(std::vector <double> in_straws,
							  std::vector <long> &cornerIndex)
// sharp noice avoidance
{
	// check each corner
	for (int i = 0; i <(long)cornerIndex.size() - 2; i++){
		// get the corner index
		int c1 = cornerIndex[i];
		int c2 = cornerIndex[i+1];
		// check whether this two points are adjacent
		if (c2 - c1 <= 1 || (c2 - c1 <= 2 && (i == 0 || i == cornerIndex.size() - 2))){
			// delete the point with larger straw value
			int beginIndex = i;
			if (in_straws[c1] < in_straws[c2]){
				beginIndex = i+1;
			}
			// delete the point
			cornerIndex.erase(cornerIndex.begin()+beginIndex);
			i--;
		}
	}
}


void CIStraw::PostProcess(std::vector <double> in_straws,
						  std::vector <POINT2D> ResampledNodes,
						  bool UseTime,
						  std::vector <double> ResampledTimes,
						  double m_meanTime,
						  std::vector <long> &cornerIndex)
//Polyline post process the inital corner set
{
	// add missing corners to the list
	AddCorners_Collinear(in_straws, ResampledNodes, cornerIndex);

	// add the point mean time is small as the possible corner.
	if(UseTime){
		AddCorners_Time(ResampledTimes, m_meanTime, cornerIndex);
	}

	// adjust the corner to the resampled point closer to the real corner
	AdjustCorners(ResampledNodes, cornerIndex);

	// triplet collinear test pass 0: with a high threshold
	TriCollinearPass0(ResampledNodes, cornerIndex);

	// triplet collinear test pass 1: with a lower threshold
	TriCollinearPass1(ResampledNodes, UseTime, ResampledTimes, m_meanTime, cornerIndex);

	// Sharp Noise Avoidance
	SharpNoiseAvoid(in_straws, cornerIndex);
}


void CIStraw::CurvProcessPass0(std::vector <POINT2D> ResampledNodes,
							   std::vector <long> &cornerIndex)
// stroke curve part process pass 0
{
	int preCorner = cornerIndex[0];   // set the first corner as the previous corner 

	// check each corner candidate starting from the second corner
	for (int i = 1; i <(long)cornerIndex.size() - 1; i++){
		// init variable for each iteration
		bool notCorner = false;
		int theCorner = cornerIndex[i];
		int nxtCorner = cornerIndex[i + 1];
		int preDiff = theCorner - preCorner;
		int nxtDiff = nxtCorner - theCorner;
		int startIndex = theCorner - 12;

		// set previous corner as the start if it is too close to current checking corner
		if (preDiff < 12) startIndex = preCorner;
		int endIndex = theCorner + 12;
		// set next corner as the end if it is too close to current checking corner
		if (nxtDiff < 12) endIndex = nxtCorner;
		// compute angle1 (alpha)
		double angle1= GetAngle(ResampledNodes[theCorner], ResampledNodes[startIndex], ResampledNodes[endIndex]);

		// compute angle2 (beta)
		startIndex = theCorner - (int)ceil((float)(theCorner - startIndex) / 3);
		endIndex = theCorner + (int)ceil((float)(endIndex - theCorner) / 3);
		double angle2= GetAngle(ResampledNodes[theCorner], ResampledNodes[startIndex], ResampledNodes[endIndex]);

		// compute angle3 
		if (preDiff < 6) startIndex = theCorner - 1;
		else startIndex = theCorner - 2;
		if (nxtDiff < 6) endIndex = theCorner + 1;
		else endIndex = theCorner + 2;
		double angle3= GetAngle(ResampledNodes[theCorner], ResampledNodes[startIndex], ResampledNodes[endIndex]);

		// check all the angles to decide whether the current corner is real
		if ((angle2 > 36 + 0.85 * angle1 && angle1 > 20 && angle3 > 80 + 0.55 * angle1)
			|| angle3 > 161
			|| ((preDiff < 3 || nxtDiff < 3) && angle2 > 130)){
			notCorner = true;
		}

		// update previous corner
		preCorner = cornerIndex[i];

		// delete the incorrect corner
		if (notCorner){
			cornerIndex.erase(cornerIndex.begin()+i);
			i--;
		}
	}
}


void CIStraw::CurvProcessPass1(std::vector <POINT2D> ResampledNodes,
							   std::vector <long> &cornerIndex)
// stroke curve part process pass 1
{
	// check each corner candidate
	for (int i = 1; i <(long)cornerIndex.size() - 2; i++){
		// init variable for each iteration
		bool notCorner = false;
		bool hasCross = false;
		int theCorner = cornerIndex[i];
		int start0 = theCorner - 12;
		int end0 = theCorner + 12;
		if (start0 < cornerIndex[i-1]) start0 = cornerIndex[i-1];
		if (end0 > cornerIndex[i+1]) end0 = cornerIndex[i+1];
		int start1 = theCorner - (int)ceil((float)(theCorner - start0) / 3);
		int end1 = theCorner + (int)ceil((float)(end0 - theCorner) / 3);

		// compute angle3
		double angle3= GetAngle(ResampledNodes[theCorner], ResampledNodes[theCorner - 1], ResampledNodes[theCorner + 1]);

		// check whether the stroke segment has self-intersection

		if (!SameDirection(ResampledNodes[theCorner], ResampledNodes[start0], ResampledNodes[end0],
			ResampledNodes[start1], ResampledNodes[end1])){
			start0 = theCorner - 4;
			end0 = theCorner + 4;
			if (start0 < cornerIndex[i-1]) start0 = cornerIndex[i-1];
			if (end0 > cornerIndex[i+1]) end0 = cornerIndex[i+1];
			start1 = theCorner - 1;
			end1 = theCorner + 1;
			// check whether the intersection is close to the corner candidate
			if (!SameDirection(ResampledNodes[theCorner], ResampledNodes[start0], ResampledNodes[end0],
				ResampledNodes[start1], ResampledNodes[end1]))
			continue;
			hasCross = true;
		}
		else if (!IsLine(cornerIndex[i-1], theCorner, 0.975, ResampledNodes) && !IsLine(theCorner, cornerIndex[i+1], 0.975, ResampledNodes)){
			// check whether the stroke segment has S shape
			if (!SameDirection(ResampledNodes[theCorner], ResampledNodes[start0], ResampledNodes[start1],
				ResampledNodes[end1], ResampledNodes[end0]) && angle3 > 135)
			notCorner = true;
		}

		// compute angle1 and angle2
		double angle1= GetAngle(ResampledNodes[theCorner], ResampledNodes[start0], ResampledNodes[end0]);
		double angle2= GetAngle(ResampledNodes[theCorner], ResampledNodes[start1], ResampledNodes[end1]);
		// add new threshold for angle3
		double threshold = 96.3;
		if (IsLine(cornerIndex[i-1], theCorner, 0.975, ResampledNodes) || IsLine(theCorner, cornerIndex[i+1], 0.975, ResampledNodes))
			threshold = 128;
		// add new threshold for angle3
		if ((angle2 > 26.1 + 0.93 * angle1 && ((angle3 > 31 + angle1 && angle3 > threshold) || angle3 > 161))
			|| (hasCross && angle2 - angle1 > 15 && angle3 > 20)){
			notCorner = true;
		}

		// delete the incorrect corner
		if (notCorner){
			cornerIndex.erase(cornerIndex.begin()+i);
			i--;
		}
	}
}


void CIStraw::FindIStrawCorners(std::vector <POINT2D> Nodes,
								std::vector <double> Times,
								//std::vector <POINT2D> &Corners,
								std::vector <long> &positions)   //pcompany
// Find all the corners of a stroke

// The approach resamples the input stroke twice:
//   - the first time setting the first raw point as the first resampled point,
//   - the second time shifting all the resampled points backward
//     half the interspacing distance.
// Next the approach finds all the corners in these two resampled strokes and merges the two corner sets
// The distance between a resampled point to a real corner is,at worst,
// one fourth the interspacing length.

// The input is a vector of nodes of the stroke
// plus a vector of times between consecutive nodes

// After parsing the stroke, m_numC contains the number of corners
// After parsing the stroke, vector m_corners contains the coordinates of the corners

// The output is a sequence of coordinates of the corners in the resampled space
{
	//Switch between ShortStraw and IStraw
	//------------------------------------
	bool UseTime= false;
	if(Times.size() > 0 && (Times[Times.size()-1]-Times[0])>0)
		UseTime= true;

	//pass 0: find all the corners of first resampled point set
	//---------------------------------------------------------
	std::vector <POINT2D> ResampledNodes0;
	std::vector <long> positions0;  //pcompany
	std::vector <double> ResampledTimes0;
	double meanTime= 0;

	CResampleStroke RS;
	ResampledNodes0= RS.GetResampledStroke(0, Nodes, positions0,   //pcompany
										   UseTime, Times, ResampledTimes0, meanTime);

	std::vector <long> cornerIndex0;
	GetCorners(ResampledNodes0, UseTime, ResampledTimes0, meanTime, cornerIndex0);

	//pass 1: find all the corners of the shifted point set
	//-----------------------------------------------------
	std::vector <POINT2D> ResampledNodes1;
	std::vector <long> positions1;  //pcompany
	std::vector <double> ResampledTimes1;
	ResampledNodes1= RS.GetResampledStroke(1, Nodes, positions1,   //pcompany
										   UseTime, Times, ResampledTimes1, meanTime);

	std::vector <long> cornerIndex1;
	GetCorners(ResampledNodes1, UseTime, ResampledTimes1, meanTime, cornerIndex1);

	//Combine the corners together
	//----------------------------
	std::vector <long> Resampled (cornerIndex0.size(), 0);
	std::vector <long> NumCorners= cornerIndex0;
	for (int i = 0; i <(long)cornerIndex1.size(); i++){
		NumCorners.push_back(cornerIndex1[i]);
		Resampled.push_back(1);
	}

	//Bubble sort both samples
	//SortMinorFirst(NumCorners, Resampled);   //#include "Tools_Vector.h"
	bool swapped;
	do{
		swapped= false;
		for (long i= 1; i<(long)NumCorners.size()-1; i++){
			if (NumCorners[i] > NumCorners[i+1]){
				std::swap(NumCorners[i], NumCorners[i+1]);
				std::swap(Resampled[i], Resampled[i+1]);
				swapped= true;
			}
		}
	}while(swapped);

	//This was the original merging algorithm!!
	//int cIdx = 1;
	//Corners.push_back(ResampledNodes[0]);
	//for (int i = 1; i <(long)cornerIndex1.size(); i++){
	//	int idx0 = cornerIndex0[cIdx];   // This index may go out of range for strokes with short end tails
	//	int idx1 = cornerIndex1[i];
	//	// make sure the two corners are not too close
	//	while (idx0 < idx1 - 1){
	//		Corners.push_back(ResampledNodes[idx0]);
	//		cIdx++;
	//		idx0 = cornerIndex0[cIdx];
	//	}
	//	if (idx0 <= idx1 + 1){
	//		cIdx++;
	//	}
	//	Corners.push_back(ResampledNodes[idx1]);
	//}

	//Save corners coordinates as calculated in the resampled space
	//(except for the first and last corners, 
	//which are respectively linked to the first and last points)
	//-------------------------------------------------------------
	//Corners.clear();
	//Corners.push_back(Nodes[0]); //Assign non-resampled coordinates to the first corner
	positions.clear();  //pcompany
	positions.push_back(0);  //pcompany

	for (int i = 1; i <(long)NumCorners.size()-1; i++){
		if(Resampled[i] == 0){
			//Corners.push_back(ResampledNodes0[NumCorners[i]]);
			positions.push_back(positions0[NumCorners[i]]);  //pcompany
		}
		else{
			//Corners.push_back(ResampledNodes1[NumCorners[i]]);
			positions.push_back(positions1[NumCorners[i]]);  //pcompany
		}
	}

	//Corners.push_back(Nodes[Nodes.size()-1]); //Assign non-resampled coordinates to the last corners
	positions.push_back((long)Nodes.size()-1);  //pcompany
}


//////////////////////////////////////////////////////////////////////////////////


Strips::Strips()
{
}


Strips::~Strips()
// Clear sample points
{
}


bool Strips::IsPointInRectangle(POINT2D Point,
								POINT2D Centroid, double Length, double Height,
								double rotation)
//Returns true if Point is inside or in the border of the rectangle defined by:
//		Centroid is its central point
//		Length is the measure of half long side
//		Height is the measure of half short side
//		Rotation is the orientation of its long side
{
	//Translate coordinates of Point
	Point.x -= Centroid.x;
	Point.y -= Centroid.y;

	//Coordinates of point relative to the rotated axes
	double dA= std::fabs( Point.x * cos(rotation) + Point.y * sin(rotation));
	double dB= std::fabs(-Point.x * sin(rotation) + Point.y * cos(rotation));

	//Check if point is inside (or in the border of) rectangle
	if (dA<=Length && dB<=Height)
		return true;
	else
		return false;
}


bool Strips::IsPointInEllipse(POINT2D Point,
							  POINT2D Centroid, double A, double B, double rotation)
//Returns true if Point is inside or in the border of the ellipse defined by:
//		Centroid is its central point
//		A is the measure of half long axis
//		B is the measure of half short axis
//		Angle is the orientation of its long axis
{
	//Distance from centroid to foci
	double F= sqrt(A*A-B*B);
	double Fx= F*cos(rotation);
	double Fy= F*sin(rotation);

	//Foci coordinates
	POINT2D F1, F2;
	F1.x= Centroid.x - Fx;
	F1.y= Centroid.y - Fy;

	F2.x= Centroid.x + Fx;
	F2.y= Centroid.y + Fy;

	//Distance from foci to Point
	double r1= MODULE2D(Point.x, Point.y, F1.x, F1.y);
	double r2= MODULE2D(Point.x, Point.y, F2.x, F2.y);

	//Check if point is inside (or in the border of) ellipse
	if ((r1+r2) <= 2*A)
		return true;
	else
		return false;
}


void Strips::GetParams(double CellSize, double &A, double &B, double &nu, long &k)
//Parameters of the sliding strips (ellipses and rectangles)
{
	A= 4*CellSize;  // 14*CellSize  //
	B= A/8;
	nu= 3*A/4;
	k= 5;
}


double Strips::GetSlope(std::vector <POINT2D> Nodes, long ipoint, long k)
//Slope at ipoint

//Calculates the slope of the segment that connects
//the mid point of the previous k points (P1)
//and the mid point of the next k points (P2)
{
	//Get P1
	POINT2D P1 (0, 0);
	long counter= 0;
	for (long j= ipoint; j>max(-1, ipoint-k); j--){
		P1.x+= Nodes[j].x;
		P1.y+= Nodes[j].y;
		counter ++;
	}
	P1.x= P1.x/counter;
	P1.y= P1.y/counter;

	//Get P2
	POINT2D P2 (0, 0);
	counter= 0;
	for (long j= ipoint; j<min(ipoint+k, (long) Nodes.size()); j++){
		P2.x+= Nodes[j].x;
		P2.y+= Nodes[j].y;
		counter ++;
	}
	P2.x= P2.x/counter;
	P2.y= P2.y/counter;

	double DistP1P2= MODULE2D(P1.x, P1.y, P2.x, P2.y);
	if(DistP1P2 == 0)  //Happens in cusps where the stroke overlaps itself!!
		if(k<(long)Nodes.size())
			return GetSlope(Nodes, ipoint, k+1);  //Increase the interval recursively
		else
			return 0;  //No valid angle found!!

	//Get Angle
	double angle;
	if(P2.y>=P1.y){ //[0, 180]
		angle= acos((P2.x-P1.x)/DistP1P2);
	}
	else{ // [180, 360]
		angle= acos((P1.x-P2.x)/DistP1P2)+ NUMBER_PI;
	}

	return angle;
}


void Strips::GetStrip(std::vector <POINT2D> Nodes,
				  long StripType,  // input the strip type (S1, S2 or S3)
				  long ipoint,
				  double &A, double &B, double &Angle)  // output the parameters of the strip
//Returns the parameter of one of the strips used to detect corners

//StripType is the input variale that selects the strip returned
// Values returned are:
//		Half length of the long side (A)
//		half length of the short side (B)
//		Angle of rotation of the long side (relative to the horizontal)
{
	if(Nodes.size() < 2) return;

	double CellSize= MODULE2D(Nodes[0].x, Nodes[0].y, Nodes[1].x, Nodes[1].y);
	for (long i= 1; i<(long)Nodes.size()-1; i++){
		CellSize= max(CellSize, MODULE2D(Nodes[i].x, Nodes[i].y, Nodes[i+1].x, Nodes[i+1].y));
	}

	//Parameters of the sliding strips (ellipses and rectangles)
	double nu;
	long k;
	GetParams(CellSize, A, B, nu, k);

	//Parameters of the sliding strips at point ipoint
	if(StripType == 1){  //S1
		A= A;
		B= B;
	}
	else if(StripType == 2){  //S2
		A= 3*A/4;
		B= B;
	}
	else if(StripType == 3){  //S3
		A= A/2;
		B= B/2;
	}

	Angle= GetSlope(Nodes, ipoint, k);
}


void Strips::FindSSCorners(std::vector <POINT2D> Nodes, 
						   //std::vector <POINT2D> &Corners,
						   std::vector <long> &positions)  //pcompany
// Find all the corners of a stroke whose sequence of nodes is given

// This Sliding Strips approach detects differences in the number of nodes of the stroke
// located inside concentric rectangular and/or elliptical
// strips around the candidate corners

//Based on:
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

//The algorithm requires equally spaced nodes in the stroke
//This resampled stroke is obtained based on:
//		Wolin A., Eoff B., Hammond T.
//		Shortstraw: a simple and effective corner finder for polylines.
//		SBIM 2008, pp. 33-40.

{
	//Resample to get equally spaced nodes in the stroke
	std::vector <long> MappedNodes;
	CResampleStroke RS;
	std::vector <POINT2D> ResampledNodes= RS.GetResampledStroke(Nodes, MappedNodes);
	long NumPoints= (long)ResampledNodes.size();

	//Counters of points inside the sliding strips (ellipses and rectangles)
	std::vector <long> NumS1(NumPoints, 0);
	std::vector <long> NumS2(NumPoints, 0);
	std::vector <long> NumS3(NumPoints, 0);

	//General parameters of the sliding strips (ellipses and rectangles)
	double CellSize= MODULE_2D(ResampledNodes[0], ResampledNodes[1]);

	double A, B, nu;
	long k;
	GetParams(CellSize, A, B, nu, k);

	//Loop over all the points of the stroke
	//to count the number of points in S1, S2 and S3
	for (long ipoint= 0; ipoint<NumPoints; ipoint++){
		//Slope at ipoint
		double rotation= GetSlope(ResampledNodes, ipoint, k);

		//Count the number of points inside strips S1, S2 and S3
		for (long j= (long)max(0, ipoint-A); j<(long)min(NumPoints, ipoint+A); j++){
			if(IsPointInEllipse(ResampledNodes[j], ResampledNodes[ipoint], A, B, rotation)){
				NumS1[ipoint]+=1;
				if(IsPointInEllipse(ResampledNodes[j], ResampledNodes[ipoint], A/2, B, rotation)){
					NumS2[ipoint]+=1;
					if(IsPointInRectangle(ResampledNodes[j], ResampledNodes[ipoint], A/2, B/2, rotation))
						NumS3[ipoint]+=1;
				}
			}
		}
	}

	//Make groups of connected points in G
	std::vector <long> Groups;
	for (long ipoint= 0; ipoint<NumPoints; ipoint++){
		if(NumS1[ipoint] == NumS2[ipoint])
		Groups.push_back(ipoint);
	}

	//Search corners in the groups
	//Corners.clear();
	//Corners.push_back(ResampledNodes[0]);
	positions.clear();  //pcompany
	positions.push_back(0);  //pcompany

	long SizeGroups= (long)Groups.size();
	long ig=0;
	while(ig<SizeGroups){
		long position= Groups[ig];
		while((ig+1)<SizeGroups && Groups[ig+1] == (Groups[ig]+1)){
			if(NumS3[Groups[ig+1]] < NumS3[position]){
				position= Groups[ig+1];
			}
			ig++;
		}

		if(NumS3[position] < nu)
			//Corners.push_back(ResampledNodes[position]);
			positions.push_back(MappedNodes[position]);  //pcompany

		ig++;
	}
	//Corners.push_back(ResampledNodes[NumPoints-1]);
	positions.push_back(MappedNodes[NumPoints-1]);  //pcompany
}


//////////////////////////////////////////////////////////////////////////////////


CStrokeCornerFinder::CStrokeCornerFinder()
{
}


CStrokeCornerFinder::~CStrokeCornerFinder()
{
}


void CStrokeCornerFinder::RefineCorners(std::vector <POINT2D> Nodes,
										std::vector <long> &positions)
//Corners are refined according to the following criteria:
//  -Corners located very close to the first point are removed
//  -Corners located very close to the last point are removed
//  -If two corners are too close to each other,
//   the best node in the interval is selected instead of both neighbor nodes
//   (The best in the interval is the one forming the smallest angle
//    with its preceding and its subsequent)
{
	if(positions.size() <= 2)
		return;

	//Remove repeated corners
	//(They happen when NumCorners>NumNodes)
	for (long i= (long)positions.size()-1; i>0; i--){
		if(positions[i] == positions[i-1])
			positions.erase(positions.begin()+i);
	}
	if(positions.size() <= 2)
		return;

	long NumNodes= (long)Nodes.size();

	//Refine the ends
	//(Remove corners close to the ends)
	long STRAW_WINDOW= 4; //STRAW_WINDOW is a parameter of the stroke corner finder
	for (long i= (long)positions.size()-2; i>0; i--){
		if(positions[i] > NumNodes-1-STRAW_WINDOW ||
		   positions[i] < STRAW_WINDOW)
			positions.erase(positions.begin()+i);
	}
	if(positions.size() <= 2)
		return;

	//Select the best node for intervals (pairs of corners)
	//produced by iStraw resampling
	//long NEIGHBOR= NumNodes/((long)positions.size()-1)/2;
	long NEIGHBOR= NumNodes/((long)positions.size()-1)/2;
	NEIGHBOR= min(NEIGHBOR, 2*STRAW_WINDOW);
	if(NEIGHBOR > 2){

		long numP= (long)positions.size();
		for (long i= numP-2; i>0; i--){
			if((positions[i]-positions[i-1]) < NEIGHBOR){
				double AngleMin= 2*NUMBER_PI;
				long ini= max(1, positions[i-1]);
				long fin= min(NumNodes-2, positions[i]);
				for(long k= ini; k<=fin; k++){
					double Angle= GetAngle2D_PI(Nodes[k-1], Nodes[k], Nodes[k+1]);

					if (Angle < AngleMin){
						AngleMin= Angle;
						positions[i-1]= k;
					}
				}
				positions.erase(positions.begin()+i);
			}
		}
	}
}


//void CStrokeCornerFinder::FilterCollinearCorners(std::vector <POINT2D> Nodes,
//												 std::vector <long> &positions)
////Remove corners whose connected sub-strokes are collinear
////and do not overlap
//
////Local oscilations can result in spurious detection of corners
////They are detected because the two consecutive strokes they connect are alligned
//
//// Added by P.Company in 2013
//{
//	int num_corners= positions.size();
//
//	if (num_corners <= 2)
//		return;
//
//	// Detect collinear substrokes (consecutive and nearly-parallel strokes)
//	for (long i= num_corners-2; i>0 ; i--){
//		if(NearlyParallel(Nodes[positions[i-1]], Nodes[positions[i]],
//						Nodes[positions[i]], Nodes[positions[i+1]],
//						DEGREES_TO_RADIANS(10))){
//			if(GetAngle3Points(Nodes[positions[i-1]],
//							   Nodes[positions[i]],
//							   Nodes[positions[i+1]]) > NUMBER_PI/2){
//				// Are collinear and do not overlap
//				positions.erase(positions.begin()+i);
//			}
//		}
//	}
//}


void CStrokeCornerFinder::FindStrokeCorners(CStroke InputStroke,
											std::vector <long> &positions,
											long StrokeCornersApproach)
//Find all the corners of a stroke

//Three different approaches may be used,
//depending on the value of StrokeCornersApproach:
//		0 Automatic
//		1 IStraw by Xiong and LaViola       (requires time between consecutive points)
//		                                    (switches to ShortStraw if time sequence is not available)
//		2 Sliding Strips by Sarfraz et al.  (does not require time sequence)

//The input is a vector of pointers to Points of the stroke
//Every Point contains (x, y, time), althoug time is optional

//Corners are obtained in a resampled space,
//as strokes are resampled with constant inter-distance
//		Based on:
//		Wolin A., Eoff B., Hammond T.
//		Shortstraw: a simple and effective corner finder for polylines.
//		SBIM 2008, pp. 33-40.

//Corners are mapped back into the stroke space:
//The output is a list of positions of the points of the stroke closer to every corner
//Positions are relative to the sequence of points of the stroke
{
	positions.clear();

	//Short strokes are trivially parsed
	if(InputStroke.GetNumPoints() < 7){   //Num of point > 2*STRAW_WINDOW
		positions.push_back(0);
		positions.push_back(InputStroke.GetNumPoints()-1);
		return;
	}

	//Copy nodes (and times) from input stroke up to a local database
	std::vector <POINT2D> Nodes;
	std::vector <double> Times;
	double TotalTime= 0;
	long nn= 0;

	for (long ipoint=0; ipoint<InputStroke.GetNumPoints(); ipoint++){
		CStrokePoint I= InputStroke.GetPoint(ipoint);
		POINT2D P= I.GetPosition();
		if(nn==0 || P != Nodes[nn-1]){  //Filter duplicated nodes (consecutive nodes located in the same position)
			Nodes.push_back(P);
			nn++;
			double time= I.GetTime();
			Times.push_back(time);
			TotalTime += time;
		}
	}

	//Find corners in the resampled stroke
	//std::vector <POINT2D> Corners;   //Will contain the corners' coordinates in the resampled stroke

	switch (StrokeCornersApproach){
		case 0:{  //Automatic
			if(TotalTime > 0){  //IStraw if time information is available
				CIStraw* CF = new CIStraw();
				//CF->FindIStrawCorners(Nodes, Times, Corners, positions);   //pcompany
				CF->FindIStrawCorners(Nodes, Times, positions);   //pcompany
				delete CF;
			}
			else{  //Silding Strips otherwise
				Strips* CF = new Strips();
				//CF->FindSSCorners(Nodes, Corners, positions);   //pcompany
				CF->FindSSCorners(Nodes, positions);   //pcompany
				delete CF;
			}
			break;
		}

		case 1:{  //IStraw (or ShortStraw if no time available)
			CIStraw* CF = new CIStraw();
			//CF->FindIStrawCorners(Nodes, Times, Corners, positions);   //pcompany
			CF->FindIStrawCorners(Nodes, Times, positions);   //pcompany
			delete CF;
			break;
		}

		case 2:{  //Silding Strips
			Strips* CF = new Strips();
			//CF->FindSSCorners(Nodes, Corners, positions);   //pcompany
			CF->FindSSCorners(Nodes, positions);   //pcompany
			delete CF;
			break;
		}
	}

	//Refine the list of corners
	RefineCorners(Nodes, positions);

	//Filter collinear sub-strokes
	//FilterCollinearCorners(Nodes, positions);
}
