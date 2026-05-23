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
#include <cfloat>
#include <cmath>
#include <cassert>
#include <vector>

#include "Tools_Geometry.h"
#include "Tools_Vector.h"

#include "DB_Model.h"
#include "DB_CSG.h"
#include "ReferDoc.h"
#include "Refer.h"
#include "resource.h"

#include "WinRf_MainFrame.h"
#include "WinRf_GDI_Transform.h"

#include "DB_Model_Maths.h"
#include "DialogProgress.h"
#include "CueSubGraphs.h"

#include "CueMainAxes.h"

#include "Refine2D.h"


///////////////////////////////////////////////////////////////
//Delaunay triangulation adapted from:
//
// Gilles Dumoulin's C++ implementation of Paul Bourke's Delaunay triangulation algorithm.
// Source: http://paulbourke.net/papers/triangulate/


bool Junctions2D::Delaunay::ReorderPoints(std::vector<POINT2D> &p,
										  std::vector<long> &NumPoint)
//Bubble sort points from smaller to larger x coordinate

//The list of renumbered points is also returned
{
	long nv= (long)p.size();

	for (long i=0; i<nv; i++){
		NumPoint.push_back(i);
	}

	bool swapped;
	do{
		swapped= false;
		for (long i= 0; i<nv-1; i++){
			if (p[i].x > p[i+1].x){
				std::swap(NumPoint[i], NumPoint[i+1]);
				std::swap(p[i], p[i+1]);
				swapped= true;
			}
		}
	}while(swapped);

	//Check for repeated points
	//(Since triangulation fails for clouds with repeated points!!)
	for (long i=0; i<nv-1; i++){
		if (p[i].x == p[i+1].x && p[i].y == p[i+1].y){
			return false;
		}
	}
	return true;

}


bool Junctions2D::Delaunay::CircumCircle(double xp, double yp, double x1, double y1,
										 double x2, double y2, double x3, double y3,
										 double &xc, double &yc, double &r)
// Return true if a point (xp,yp) is inside the circumcircle made up
// of the points (x1,y1), (x2,y2), (x3,y3)
// Note : A point on the edge is inside the circumcircle

// Also returns the circumcircle centre in (xc,yc), and the radius r
{
	double EPSILON = 0.000001;
	double m1, m2, mx1, mx2, my1, my2;

	// Check for coincident points
	if(std::fabs(y1 - y2) < EPSILON && std::fabs(y2 - y3) < EPSILON)
		return(false);

	if(std::fabs(y2-y1) < EPSILON){ 
		m2 = - (x3 - x2) / (y3 - y2);
		mx2 = (x2 + x3) / 2.0;
		my2 = (y2 + y3) / 2.0;

		xc = (x2 + x1) / 2.0;
		yc = m2 * (xc - mx2) + my2;
	}
	else if(std::fabs(y3 - y2) < EPSILON){ 
		m1 = - (x2 - x1) / (y2 - y1);
		mx1 = (x1 + x2) / 2.0;
		my1 = (y1 + y2) / 2.0;

		xc = (x3 + x2) / 2.0;
		yc = m1 * (xc - mx1) + my1;
	}
	else{
		m1 = - (x2 - x1) / (y2 - y1); 
		m2 = - (x3 - x2) / (y3 - y2); 
		mx1 = (x1 + x2) / 2.0; 
		mx2 = (x2 + x3) / 2.0;
		my1 = (y1 + y2) / 2.0;
		my2 = (y2 + y3) / 2.0;

		xc = (m1 * mx1 - m2 * mx2 + my2 - my1) / (m1 - m2); 
		yc = m1 * (xc - mx1) + my1; 
	}

	double dx = x2 - xc;
	double dy = y2 - yc;
	double rsqr = dx * dx + dy * dy;
	r = sqrt(rsqr); 
	dx = xp - xc;
	dy = yp - yc;
	double drsqr = dx * dx + dy * dy;

	return((drsqr <= rsqr) ? true : false);
}


void Junctions2D::Delaunay::Triangulate(std::vector<POINT2D> point,
										std::vector<TRIANGLE> &v,
										int &ntri)
//Triangulation subroutine
//Takes as input a set of vertices in array "point"
//Returned is a list of ntri triangular faces in the array of triangles "v"
//(These triangles are arranged in a consistent clockwise order)

//The vertex array must be sorted in increasing x values
{
	int nv= (long)point.size();

	//The vector point must be big enough to hold 3 more points
	//(used to save the vertices of the supertriangle)
	POINT2D p(0, 0);
	point.push_back(p);
	point.push_back(p);
	point.push_back(p);

	//Allocate memory for the completeness list, flag for each triangle
	std::vector<bool> complete;

	//Find the maximum and minimum vertex bounds.
	//This is to allow calculation of the bounding triangle
	double xmin = point[0].x;
	double ymin = point[0].y;
	double xmax = xmin;
	double ymax = ymin;
	for(long i = 1; i < nv; i++){
		if (point[i].x < xmin) xmin = point[i].x;
		if (point[i].x > xmax) xmax = point[i].x;
		if (point[i].y < ymin) ymin = point[i].y;
		if (point[i].y > ymax) ymax = point[i].y;
	}
	double dx = xmax - xmin;
	double dy = ymax - ymin;
	double dmax = (dx > dy) ? dx : dy;
	double xmid = (xmax + xmin) / 2.0;
	double ymid = (ymax + ymin) / 2.0;

	//Set up the supertriangle that encompasses all the sample points
	//(The supertriangle coordinates are added to the end of the vertex list)
	//(The supertriangle is the first triangle in the triangle list)
	point[nv+0].x = xmid - 20 * dmax;
	point[nv+0].y = ymid - dmax;
	point[nv+1].x = xmid;
	point[nv+1].y = ymid + 20 * dmax;
	point[nv+2].x = xmid + 20 * dmax;
	point[nv+2].y = ymid - dmax;

	TRIANGLE t;
	t.p1= nv;
	t.p2= nv+1;
	t.p3= nv+2;
	v.push_back(t);

	complete.push_back(false);
	ntri = 1;

	//Include each point, one at a time, into the existing mesh
	for(long i=0; i<nv; i++){
		double xp = point[i].x;
		double yp = point[i].y;

		//Allocate memory for the edge list
		int nedge = 0;
		std::vector<EDGE> edges;

		//Set up the edge buffer
		//(If the point (xp,yp) lies inside the circumcircle then the
		//three edges of that triangle are added to the edge buffer
		//and that triangle is removed)
		for(long j=0; j<ntri; j++){
			if(complete[j])
				continue;

			double xc, yc, r;
			bool inside = CircumCircle(xp, yp,
									   point[v[j].p1].x, point[v[j].p1].y,
									   point[v[j].p2].x, point[v[j].p2].y,
									   point[v[j].p3].x, point[v[j].p3].y,
									   xc, yc, r);

			//if (xc + r < xp) // Suggested
			//if (xc + r + EPSILON < xp)
			if (xc < xp && ((xp-xc)*(xp-xc)) > r)
				complete[j] = true;

			if(inside){
				EDGE e;
				e.p1= v[j].p1; e.p2= v[j].p2;
				edges.push_back(e);
				e.p1= v[j].p2; e.p2= v[j].p3;
				edges.push_back(e);
				e.p1= v[j].p3; e.p2= v[j].p1;
				edges.push_back(e);
				nedge += 3;

				v[j] = v[ntri-1];
				v.erase(v.begin()+v.size()-1);
				complete[j] = complete[ntri-1];
				complete.erase(complete.begin()+complete.size()-1);
				ntri--;

				j--;
			}
		}

		//Tag multiple edges
		//(Note: if all triangles are specified anticlockwise
		//then all interior edges are opposite pointing in direction)
		for(long j=0; j<nedge-1; j++){
			for(long k=j+1; k<nedge; k++){
				if((edges[j].p1 == edges[k].p2) && (edges[j].p2 == edges[k].p1)){
					edges[j].p1 = -1;
					edges[j].p2 = -1;
					edges[k].p1 = -1;
					edges[k].p2 = -1;
				}
				//Shouldn't need the following, see note above
				if((edges[j].p1 == edges[k].p1) && (edges[j].p2 == edges[k].p2)){
					edges[j].p1 = -1;
					edges[j].p2 = -1;
					edges[k].p1 = -1;
					edges[k].p2 = -1;
				}
			}
		}

		//Form new triangles for the current point
		//Skipping over any tagged edges
		//All edges are arranged in clockwise order
		for(long j=0; j<nedge; j++) {
			if(edges[j].p1 < 0 || edges[j].p2 < 0)
				continue;

			t.p1= edges[j].p1;
			t.p2= edges[j].p2;
			t.p3= i;
			v.push_back(t);

			complete.push_back(false);
			ntri++;
		}
	}

	//Remove triangles with supertriangle vertices
	//These are triangles which have a vertex number greater than nv
	for(long i=0; i<ntri; i++) {
		if(v[i].p1 >= nv || v[i].p2 >= nv || v[i].p3 >= nv){
			v[i] = v[ntri-1];
			v.erase(v.begin()+v.size()-1);
			ntri--;
			i--;
		}
	}

	return;
}


bool Junctions2D::Delaunay::NeighborTips(CDB_Model *DDBB,
										 std::vector<std::vector<long>> &nt)
//Given a set of tips of lines, its Delaunay triangulation is calculated

//Then, all the tips sharing one triangle with a given tip
//are saved as its Neighbors
//But tips that are connected to each other by a line are not eligible as neighbors
{
	//Get the cloud of tips to triangulate
	//------------------------------------
	std::vector<POINT2D> tips;
	POINT2D point;

	for (long i=0; i<DDBB->GetSizeVertices(); i++){
		CVertex vi= DDBB->GetVertex(i);
		point.assign(vi.x, vi.y);
		tips.push_back(point);
	}

	//Sort the cloud of tips in increasing x values
	//(and save the original numbers of the reordered tips)
	//-----------------------------------------------------
	std::vector<long> NumTip;
	if(!ReorderPoints(tips, NumTip))
		return false;

	//Get the Delaunay triangulation of the tips
	//------------------------------------------
	std::vector<TRIANGLE> v;
	int ntri = 0;
	Triangulate(tips, v, ntri);

	//Get the Neighbors of the tips
	//(while keeping their original numbers)
	//--------------------------------------
	std::vector<std::vector<long>> noNeighbor;
	for (long i=0; i<DDBB->GetSizeVertices(); i++){
		nt.push_back(std::vector<long>());
		noNeighbor.push_back(std::vector<long>());
	}

	//(Tips already connected to each other are not eligible as neighbors)
	for (long i=0; i<DDBB->GetSizeEdges(); i++){
		CEdge ei= DDBB->GetEdge(i);
		noNeighbor[ei.Head].push_back(ei.Tail);
		noNeighbor[ei.Tail].push_back(ei.Head);
	}

	//Add neighbors for each vertex
	for (long i=0; i<DDBB->GetSizeVertices(); i++){
		for (long j=0; j<ntri; j++){
			//Add the other two corners of those triangles that contain the current vertex
			if(NumTip[v[j].p1] == NumTip[i]){
				if(!IsValueInVector(NumTip[v[j].p2], noNeighbor[NumTip[i]]))
					addNewNumber(NumTip[v[j].p2], nt[NumTip[i]]);
				if(!IsValueInVector(NumTip[v[j].p3], noNeighbor[NumTip[i]]))
					addNewNumber(NumTip[v[j].p3], nt[NumTip[i]]);
			}
			else if(NumTip[v[j].p2] == NumTip[i]){
				if(!IsValueInVector(NumTip[v[j].p1], noNeighbor[NumTip[i]]))
					addNewNumber(NumTip[v[j].p1], nt[NumTip[i]]);
				if(!IsValueInVector(NumTip[v[j].p3], noNeighbor[NumTip[i]]))
					addNewNumber(NumTip[v[j].p3], nt[NumTip[i]]);
			}
			else if(NumTip[v[j].p3] == NumTip[i]){
				if(!IsValueInVector(NumTip[v[j].p1], noNeighbor[NumTip[i]]))
					addNewNumber(NumTip[v[j].p1], nt[NumTip[i]]);
				if(!IsValueInVector(NumTip[v[j].p2], noNeighbor[NumTip[i]]))
					addNewNumber(NumTip[v[j].p2], nt[NumTip[i]]);
			}
		}
	}

	return true;
}
///////////////////////////////////////////////////////////////


Junctions2D::Junctions2D(bool i_MergeCubicCorners,
						 double i_DistanceMergeCubicCorners,
						 bool i_MergeAlignedCubicCorners,
						 double i_MergeRotationCubicCorners,
						 bool i_MergeCarefulLineTips,
						 double i_maxDistance,
						 double i_maxRotation,
						 double i_ReificationMerit,
						 bool i_MergeCasualLineTips,
						 double i_CasualInTol,
						 double i_CasualOutTol,
						 bool i_ReMergeTriplets,
						 double i_maxDistanceTriplets,
						 double i_EmergentTripletsMerit,
						 bool i_ReMergeDangling,
						 double i_maxDistanceDangling,
						 double i_EmergenceDanglingDeMerit)
//Initialize with input parameters
: MergeCubicCorners(i_MergeCubicCorners),
  DistanceMergeCubicCorners(i_DistanceMergeCubicCorners),
  MergeAlignedCubicCorners(i_MergeAlignedCubicCorners),
  MergeRotationCubicCorners(i_MergeRotationCubicCorners),
  MergeCarefulLineTips(i_MergeCarefulLineTips),
  maxDistance(i_maxDistance),
  maxRotation (i_maxRotation),
  ReificationMerit (i_ReificationMerit),
  MergeCasualLineTips(i_MergeCasualLineTips),
  CasualInTol(i_CasualInTol),
  CasualOutTol(i_CasualOutTol),
  ReMergeTriplets (i_ReMergeTriplets),
  maxDistanceTriplets (i_maxDistanceTriplets),
  EmergentTripletsMerit(i_EmergentTripletsMerit),
  ReMergeDangling (i_ReMergeDangling),
  maxDistanceDangling (i_maxDistanceDangling),
  EmergenceDanglingDeMerit (i_EmergenceDanglingDeMerit)
{
}


Junctions2D::~Junctions2D()
{
}


void Junctions2D::ResetMerits()
//Resets merits of vertices in the global database
{
	CVertex vi;
	for (long i=0; i<DDBB->GetSizeVertices(); i++){
		vi= DDBB->GetVertex(i);
		vi.merit= 0;
		DDBB->SetVertex(i, vi);
	}
}


long Junctions2D::GetCubicCornerJunctions(CDB_Model *DDBB)
//Detects and merges candidate cubic corners (or thrihedral corners)

//Merges triplets of vertices, iff they are:
//	-Not previously connected to any other edge
//	-Very close to each other (their mutual "influence" circles intersect each other)
//	-Optionally, they must belong to edges respectively aligned with the main coordinate system
//	 (This requires a pre-calculation of Main Axes)

//This junction merging approach is intended 
//as a pre-stage of the main merging junctions stage

//It is based in assuming the prevalence of well drawn thriedral, and possibly cubic, corners
//while sketching polyhedral shapes

//Implemented by Pedro Company, 2024
{
	//Determine the threshold to merge neighbor vertices
	//--------------------------------------------------
	//Query to receive the transformed size of a pixel from viewport to windows in the 2D Sketch window
	//(Note that the returned value depends on the current zoom)
	double PixelSizeX, PixelSizeY;
	CReferDoc::GetDoc()->m_pFrameSketch2D->GetActiveView()->SendMessage(WM_GET_PIXEL_SIZE, (WPARAM) &PixelSizeX, (LPARAM) &PixelSizeY);
	//This is intented to be a wysiwyg measure in pixel coordinates!!!!
	double W_distanceMergingCubicCorners= DistanceMergeCubicCorners * PixelSizeX;  //We are assuming PixelSizeX == PixelSizeY

	//Detect triplets of very close tips
	//----------------------------------
	long numVertices= DDBB->GetSizeVertices();

	std::vector<std::vector<long>> triplets;
	std::vector<std::vector<long>> edges;
	std::vector<double> merits;
	CIRCLE2D CC;
	CVertex vi;
	CIRCLE2D ci;
	for (long i=0; i<numVertices-2; i++){
		if(DDBB->GetSizeEdgesInVertex(i) == 1){  //Only isolated tips are parsed
			std::vector<long> triplet;
			std::vector<long> edge;
			double merit= 1.0;    //Min merit
			//double merit= 0.0;  //Average merit
			triplet.push_back(i);
			edge.push_back(DDBB->GetEdgeInVertex(i, 0));

			vi= DDBB->GetVertex(i);
			ci.centerX= vi.x;
			ci.centerY= vi.y;
			ci.radius= W_distanceMergingCubicCorners;

			CVertex vj;
			CIRCLE2D cj;

			for (long j=i+1; j<numVertices; j++){
				if(DDBB->GetSizeEdgesInVertex(j) == 1){  //Only isolated tips are parsed
					vj= DDBB->GetVertex(j);
					cj.centerX= vj.x;
					cj.centerY= vj.y;
					cj.radius= W_distanceMergingCubicCorners;

					double meritIJ= CC.MeritCirclesIntersect(ci, cj);
					if(meritIJ > 0){
						//Select vertices that are close to ith vertex
						triplet.push_back(j);
						edge.push_back(DDBB->GetEdgeInVertex(j, 0));
						merit= min(merit, meritIJ);  //Min merit
						//merit+= meritIJ;           //Average merit
					};
				}
			}

			//Check whether exactly three vertices are close to each other
			if(triplet.size() == 3){
				//Check if second and third vertices are close to each other as well
				CVertex v1, v2;
				CIRCLE2D c1, c2;
				v1= DDBB->GetVertex(triplet[1]);
				c1.centerX= v1.x;
				c1.centerY= v1.y;
				c1.radius= W_distanceMergingCubicCorners;
				v2= DDBB->GetVertex(triplet[2]);
				c2.centerX= v2.x;
				c2.centerY= v2.y;
				c2.radius= W_distanceMergingCubicCorners;

				double meritJK= CC.MeritCirclesIntersect(c1, c2);
				if(meritJK > 0){
					//Add candidate triplet
					triplets.push_back(triplet);
					edges.push_back(edge);
					merit= min(merit, meritJK);    //Min merit
					//merit+= meritJK;             //Average merit
					merits.push_back(merit);       //Min merit
					//merits.push_back(merit/3.);  //Average merit
				}
			}
		}
	}

	//Check whether close tips belong to edges aligned with main axes
	//---------------------------------------------------------------
	if(MergeAlignedCubicCorners){
		CCueMainAxes MA;
		MA.GetMainAxes(DDBB);    //Pre-calculate main axes (as 2D refine is done before detecting cues)
		if(ReferApp.Cu.MA.MainAngles.size() != 3){
			return 0;
		}

		std::vector<POINT2D> pointAxis;
		for (long i= 0; i<3; i++){
			POINT2D pI (cos(ReferApp.Cu.MA.MainAngles[i]), sin(ReferApp.Cu.MA.MainAngles[i]));
			pointAxis.push_back(pI);
		}
		POINT2D origin(0, 0);

		for (long i=0; i<(long)triplets.size(); i++){
			std::vector<long> tripletAligned(3, false);

			for (long k=0; k<3; k++){
				CEdge edgeIK= DDBB->GetEdge(edges[i][k]);
				CVertex hIK= DDBB->GetVertex(edgeIK.Head);
				CVertex tIK= DDBB->GetVertex(edgeIK.Tail);
				POINT2D pedgeIK(hIK.x-tIK.x, hIK.y-tIK.y);

				double meritParallel= NearlyParallel(origin, pedgeIK, origin, pointAxis[0], MergeRotationCubicCorners);
				if (meritParallel > 0){
					tripletAligned[0]= true;
				}
				else{
					meritParallel= NearlyParallel(origin, pedgeIK, origin, pointAxis[1], MergeRotationCubicCorners);
					if(meritParallel > 0){
						tripletAligned[1]= true;
					}
					else{
						meritParallel= NearlyParallel(origin, pedgeIK, origin, pointAxis[2], MergeRotationCubicCorners);
						if(meritParallel > 0){
							tripletAligned[2]= true;
						}
					}
				}
			}
			if(!tripletAligned[0] || !tripletAligned[1] || !tripletAligned[2]){
				//Remove triplet
				triplets.erase(triplets.begin()+i);
				edges.erase(edges.begin()+i);
				merits.erase(merits.begin()+i);
				i--;
			}
		}
	}

	//Merge triplets of vertices into single junctions
	//------------------------------------------------
	for (long i=0; i<(long)triplets.size(); i++){
		for (long k=1; k<3; k++){
			CEdge edgeK= DDBB->GetEdge(edges[i][k]);
			if(triplets[i][k]==edgeK.Head)
				edgeK.Head= triplets[i][0];
			else
				edgeK.Tail= triplets[i][0];
			DDBB->SetEdge(edges[i][k], edgeK);
		}

		//Move triplet to the average coordinates
		CVertex v0= DDBB->GetVertex(triplets[i][0]);
		CVertex v1= DDBB->GetVertex(triplets[i][1]);
		CVertex v2= DDBB->GetVertex(triplets[i][2]);
		v0.x= (v0.x+v1.x+v2.x)/3.;
		v0.y= (v0.y+v1.y+v2.y)/3.;
		DDBB->SetVertex(triplets[i][0], v0);

		//Remove old vertices
		long vM= 1, vm= 2;
		if(triplets[i][2] > triplets[i][1]){
			vM= 2;
			vm= 1;
		}
		DDBB->RemoveVertex(triplets[i][vM], true);
		DDBB->RemoveVertex(triplets[i][vm], true);

		//Update triplets
		for (long j=i+1; j<(long)triplets.size(); j++){
			for (long k=0; k<3; k++){
				if(triplets[j][k] == triplets[i][0] ||
				   triplets[j][k] == triplets[i][1] ||
				   triplets[j][k] == triplets[i][2]){
					//Remove triplet as some of its vertices has already been merged
					triplets.erase(triplets.begin()+j);
					edges.erase(edges.begin()+j);
					merits.erase(merits.begin()+j);
					j--;
					break;
				}

				//Renumber subsequent triplets, if necessary
				if(triplets[j][k] > triplets[i][vM]) triplets[j][k]--;
				if(triplets[j][k] > triplets[i][vm]) triplets[j][k]--;
			}
		}

		//Update merit
		CVertex v= DDBB->GetVertex(triplets[i][0]);
		v.merit= merits[i];
		DDBB->SetVertex(triplets[i][0], v);

		//Clean merged triplets
		//triplets[i][vM]= triplets[i][0];
		//triplets[i][vm]= triplets[i][0];
	}

	return (long)triplets.size();
}


void Junctions2D::ReorderLines()
//Reorders lines from longer to shorter
//and lists them in "PriorLine"

//Also saves their lengths in "LengthEdges"
{
	//Calculate lengths of lines
	for (long i=0; i<DDBB->GetSizeEdges(); i++){
		LengthEdge.push_back(DDBB->GetLength2D(i));
		PriorLine.push_back(i);
	}

	//Bubble sort lines
	std::vector<double> buble= LengthEdge;
	bool swapped;
	do{
		swapped= false;
		for (long i= 0; i<(long)DDBB->GetSizeEdges()-1; i++){
			if (buble[i] < buble[i+1]){  //from longer to shorter
				std::swap(buble[i], buble[i+1]);
				std::swap(PriorLine[i], PriorLine[i+1]);
				swapped= true;
			}
		}
	}while(swapped);
}


void Junctions2D::GetAllSemiLines()
//List all the semi-lines of the line-drawing

//Adds two tripets for each line:
//	Number of the line
//	First tip
//	Second tip
//	Number of the line
//	Second tip
//	First tip
{
	long numL= 0;

	for (long i=0; i<DDBB->GetSizeEdges(); i++){
		CEdge edgeI= DDBB->GetEdge(PriorLine[i]);  //Longer lines are parsed first
		semiLines.push_back(std::vector<long>());
		semiLines[numL].push_back(PriorLine[i]);   //Number of the line
		semiLines[numL].push_back(edgeI.Head);     //First tip
		semiLines[numL].push_back(edgeI.Tail);     //Second tip
		numL++;

		semiLines.push_back(std::vector<long>());
		semiLines[numL].push_back(PriorLine[i]);
		semiLines[numL].push_back(edgeI.Tail);
		semiLines[numL].push_back(edgeI.Head);
		numL++;
	}
}


double Junctions2D::AvgLength(long numTip1, long numTip2)
//Returns the average length of the lines that contain tips numTip1 or numTip2
{
	double L= 0;
	long n= 0;
	for (long i=0; i<DDBB->GetSizeEdges(); i++){
		CEdge e= DDBB->GetEdge(i);
		if(e.Head == numTip1 || e.Tail == numTip1 ||
			e.Head == numTip2 || e.Tail == numTip2){
			L+= LengthEdge[i];
			n++;
		}
	}
	if(n==0)
		return 0;
	else
		return L/n;
}


long Junctions2D::GetFarTip(long CloseTip, long &line)
//Returns the tip connected to CloseTip

//Also returns the line that connects both tips

//WARNING: it is only valid for tips of valence 1!!
{
	for (long i=0; i<(long)semiLines.size(); i++){
		if(semiLines[i][1] == CloseTip){
			line= semiLines[i][0];
			return semiLines[i][2];
		}
	}
	return -1;
}


double Junctions2D::GetIntersection(double x1, double y1, double x11, double y11,
									double x2, double y2, double x22, double y22,
									double maxDist, double &x, double &y)
//Returns a merit value which is 1 if both tips are coincidental, otherwise
//it is in the range [0, 1] depending on the distance from the intersection point to the tips

//Lines may indisctintly intersect inside their segments,
//or outside the segments and by the side of their first tips
//In both cases the intersection must be closer than maxDist to the tips
//Returns 0 otherwise

//First line goes through points (x1,y1) and (x11,y11)
//Second line goes through points (x2,y2) and (x22,y22)

//Also returns the intersection point (x, y)

//Based on: Paul Bourke, April 1989 (http://paulbourke.net/geometry/lineline2d/)
{
	double denom  = (y22-y2) * (x11-x1) - (x22-x2) * (y11-y1);
	double numera = (x22-x2) * (y1-y2) - (y22-y2) * (x1-x2);
	double numerb = (x11-x1) * (y1-y2) - (y11-y1) * (x1-x2);

	if (std::fabs(numera) < Accuracy && std::fabs(numerb) < Accuracy && std::fabs(denom) < Accuracy)
		return -1;  //Lines are coincident

	if (std::fabs(denom) < Accuracy)
		return -1;  //lines are parallel

	//Find the intersection
	double mua = numera / denom;
	double mub = numerb / denom;

	//if (std::fabs(mua) > 0.5 || std::fabs(mub) > 0.5)
	//	return -1; //The intersection is far from the first tips

	x = x1 + mua * (x11 - x1);
	y = y1 + mua * (y11 - y1);

	//return (1. - (max(std::fabs(mua), std::fabs(mub)))/maxDist);  //The merit depends on the distance to the farthest tip
	//return (1. - (min(std::fabs(mua), std::fabs(mub)))/maxDist);  //The merit depends on the distance to the closest tip
	return (1. - ((std::fabs(mua)+std::fabs(mub))/2.)/maxDist);   //The merit depends on the average distance to both tips
}


double Junctions2D::MeritRotation(long NumTip, POINT2D newPoint)
//Returns the merit of tip NumTip
//which decreases when the lines connected to it
//must be rotated to redirect them to newPoint

//Merit is 1 if lines do not rotate,
//and is negative if at least one of them rotates more than maxRotation
{
	double meritR= 1.;

	if(maxRotation >= NUMBER_PI/2)
		return meritR;

	CVertex v= DDBB->GetVertex(NumTip);
	POINT2D t(v.x, v.y);

	std::vector<long> edgesInVertex= DDBB->GetAllEdgesInVertex(NumTip);
	for (long i=0; i<(long)edgesInVertex.size(); i++){
		CEdge edge= DDBB->GetEdge(edgesInVertex[i]);
		CVertex p;
		if (edge.Head == NumTip)
			p= DDBB->GetVertex(edge.Tail);
		else if (edge.Tail == NumTip)
			p= DDBB->GetVertex(edge.Head);
		POINT2D pivot(p.x, p.y);

		meritR= min(meritR, (1. - (GetAngle2D_PI(t, pivot, newPoint) / maxRotation)));
	}

	return meritR;
}


double Junctions2D::MeritTips(long NumTip1, long NumTip2,
							  POINT2D &junction, bool &swapTips)
//Calculate merit of merging tips NumTip1 and NumTip2

//The junction is also calculated, as follows:
//   - If both tips belong to edges not connected (their valences are 1),
//     then the merged tip is placed in their intersection
//     (unless they are nearly collinear, in which case,
//     as the intersection is unrelaiable, because of being extremely sensitive
//     to small failures in the alignment,
//     the merged tip is placed in the midpoint between the two close tips)

//   - If both tips have the same valence (and this valence is different from 1),
//     the midpoint of the segment connecting their closer tips 
//     is used as junction

//   - If, at least, one tip is shared by more than one edge,
//     then the vertex with the highest valence (the one where more edges end)
//     becomes the junction


//The approach uses an allowance (based on a cosine function) to prioritize perpendicular lines:
//   - Collinear lines are given a 100% allowance
//     while perpendicular lines are given a 200% allowance
//   - Transition is trigonometric, instead of linear, to give a great allowance for
//     lines in the range of "axonometric angles" (between 60 and 90 degrees)
{
	//Get coordinates and valences of the tips
	CVertex v1= DDBB->GetVertex(NumTip1);
	POINT2D t1(v1.x, v1.y);

	CVertex v2= DDBB->GetVertex(NumTip2);
	POINT2D t2(v2.x, v2.y);

	long Valence1= DDBB->GetSizeEdgesInVertex(NumTip1);
	long Valence2= DDBB->GetSizeEdgesInVertex(NumTip2);

	swapTips= false;

	double merit;

	//Merit of isolated lines
	//-----------------------
	if (Valence1==1 && Valence2==1){
		long line1, line2;
		CVertex t11= DDBB->GetVertex(GetFarTip(NumTip1, line1));  //far tip of the first semi-line
		CVertex t22= DDBB->GetVertex(GetFarTip(NumTip2, line2));  //far tip of the second semi-line

		//Calculate allowance, to prioritize perpendicular lines
		POINT2D segment1 (t11.x-t1.x, t11.y-t1.y);
		POINT2D segment2 (t22.x-t2.x, t22.y-t2.y);

		double dotProduct= std::fabs(DOT_PRODUCT_2D(segment1, segment2))/LengthEdge[line1]/LengthEdge[line2];
		double distance= sqrt(pow(t2.x - t1.x, 2) + pow(t2.y - t1.y, 2));
		double length1= sqrt(pow(t11.x - t1.x, 2) + pow(t11.y - t1.y, 2));
		double length2= sqrt(pow(t22.x - t2.x, 2) + pow(t22.y - t2.y, 2));
		double length= max(length1, length2);

		//if(dotProduct > 0.966){    //Angle below 15 degrees
		if(dotProduct > maxRotation){    //Angle minor than maxRotation
			if(distance < length*maxDistance){  //Distance minor than allowance
				//Lines are nearly parallel, 
				//and both tips are close enough from each other
				//hence, common tip is placed in the midpoint of both original tips
				junction.x= (t1.x + t2.x)/2.;
				junction.y= (t1.y + t2.y)/2.;
				merit= min(MeritRotation(NumTip1, junction), MeritRotation(NumTip2, junction));
			}
			else{
				merit= -1;
			}
		}

		else{
			double allowance= (2. - dotProduct);
			double maxDistAlw= maxDistance * allowance;

			//Get intersection point and its merit
			merit= GetIntersection(t1.x, t1.y, t11.x, t11.y,
								   t2.x, t2.y, t22.x, t22.y,
								   maxDistAlw,
								   junction.x, junction.y);
		}
		//if(merit < 0){
		//	return -1.;  //Do not merge if tips are too distant from the intersection point

	}

	//Merit of already connected lines
	//--------------------------------
	else{
		double meritD, meritR;
		meritD= (1. - (MODULE_2D(t1, t2) / (AvgLength(NumTip1, NumTip2) * maxDistance)));
		//if (meritD < 0)  //Do not merge if tips are too distant from each other
		//	return -1.;

		if(Valence2 == Valence1){
			//Place junction at the midpoint
			junction.x= (t1.x+t2.x)/2;
			junction.y= (t1.y+t2.y)/2;
			meritR= min(MeritRotation(NumTip1, junction), MeritRotation(NumTip2, junction));
		}

		else if(Valence2 > Valence1){
			//Swap tips to place the junction at the tip with the highest valence
			swapTips= true;
			junction= t2;
			meritR= MeritRotation(NumTip2, t1);
		}

		else{
			//Place the junction at the tip with the highest valence
			junction= t1;
			meritR= MeritRotation(NumTip2, t1);
		}

		merit= (meritD +    //Extra merit if tips are closer to each other than maxDistance
			    meritR)/2;  //Extra merit if the edges connected to the vertex rotate less than maxRotation
	}

	//Return merit
	//------------
	if(merit >= 0)
		return (1-ReificationMerit) * merit + ReificationMerit;  //Add reward for careful vertices
	else
		return -1.;   //Do not merge if tips are too distant, and/or lines rotate too much
}


void Junctions2D::DeleteNeighbor(long numNeighbor)
//Deletes the neighbor tip numNeighbor,
//and renumbers all subsequent neighbors
{
	Neighbor.erase(Neighbor.begin()+numNeighbor);

	for (long i= 0; i<(long)Neighbor.size(); i++){
		for (long j= 0; j<(long)Neighbor[i].size(); j++){
			if(Neighbor[i][j] == numNeighbor){
				Neighbor[i].erase(Neighbor[i].begin()+j);
				j--;
			}
			else if(Neighbor[i][j] > numNeighbor){
				Neighbor[i][j]--;
			}
		}
	}
}


void Junctions2D::UpdateNeighbors(long NumTip1, long NumTip2,
								  long NumTip22)
//When vertex NumTip1 is connected to vertex NumTip2,
//and vertex NumTip2 is removed,
//the lists of neighbors must be updated

//The line to be merged is defined by NumTip2 and NumTip22 (the far tip of the line)

//(Alternatively, the whole triangulation should be recalculated)

//The following changes apply:
//	- The far vertices of the merged lines
//	  are removed from the neighbor list
//	  as they can no longer be neighbor of the merged vertex

//	- The neighbors of the vertex NumTip2 are added to be 
//	  neighbors of the vertex NumTip1

//´- The neighbors of the vertex NumTip2 are removed

//	- Vertices are renumbered
{

	//Remove the far vertices from the neighbor list
	for (long j= 0; j<(long)Neighbor[NumTip1].size(); j++){
		if(Neighbor[NumTip1][j] == NumTip22){
			Neighbor[NumTip1].erase(Neighbor[NumTip1].begin()+j);
			break;
		}
	}
	for (long j= 0; j<(long)Neighbor[NumTip22].size(); j++){
		if(Neighbor[NumTip22][j] == NumTip1){
			Neighbor[NumTip22].erase(Neighbor[NumTip22].begin()+j);
			break;
		}
	}

	//Remove merged vertex NumTip1 from the neighbor list of NumTip2
	for (long j= 0; j<(long)Neighbor[NumTip2].size(); j++){
		if(Neighbor[NumTip2][j] == NumTip1){
			Neighbor[NumTip2].erase(Neighbor[NumTip2].begin()+j);
			break;
		}
	}

	std::vector<long> edgesInVertex1= DDBB->GetAllEdgesInVertex(NumTip1);
	for (long k1=0; k1<(long)edgesInVertex1.size(); k1++){
		CEdge edge1= DDBB->GetEdge(edgesInVertex1[k1]);
		long NumTip11;
		if (edge1.Head == NumTip1){
			NumTip11= edge1.Tail;
		}
		else if (edge1.Tail == NumTip1){
			NumTip11= edge1.Head;
		}

		for (long j= 0; j<(long)Neighbor[NumTip2].size(); j++){
			if(Neighbor[NumTip2][j] == NumTip11){
				Neighbor[NumTip2].erase(Neighbor[NumTip2].begin()+j);
				break;
			}
		}
	}

	//Add the list of neighbors to NumTip2 to the list of neighbors to NumTip1
	for (long j= 0; j<(long)Neighbor[NumTip2].size(); j++){
		if(Neighbor[NumTip2][j] != NumTip1){
			addNewNumber (Neighbor[NumTip2][j], Neighbor[NumTip1]);
		}
	}
}


void Junctions2D::MergeTips(long NumTip1, long NumTip2,
							POINT2D junction, 
							double &merit1, double merit2,
							bool updateNeighbors,
							bool updateSemilines)
//Updates the global database by reconecting to vertex NumTip1
//all the edges connected to vertex NumTip2,
//then removes vertex NumTip2

//vertex NumTip1 is moved to junction

//Depending on the flags, it may also update the Neighbor list and the semi-lines list
{
	//Update vertex 1
	CVertex vertex1= DDBB->GetVertex(NumTip1);
	vertex1.x= junction.x;
	vertex1.y= junction.y;
	if(merit1 >= 0){   //The vertex already merges two or more edges
		vertex1.merit= min(merit1, merit2);
	}
	else{             //The vertex is still isolated
		vertex1.merit= merit1= merit2;
	}
	DDBB->SetVertex(NumTip1, vertex1);

	//Re-connect to vertex 1 all the edges connected to vertex 2
	//(while updating the neighbors)
	//----------------------------------------------------------
	std::vector<long> edgesInVertex2= DDBB->GetAllEdgesInVertex(NumTip2);
	for (long k2=0; k2<(long)edgesInVertex2.size(); k2++){
		//Recalculate the vertex to merge
		CEdge edge2= DDBB->GetEdge(edgesInVertex2[k2]);

		long NumTip22;
		if (edge2.Head == NumTip2){
			edge2.Head = NumTip1;
			NumTip22= edge2.Tail;
		}
		else if (edge2.Tail == NumTip2){
			edge2.Tail = NumTip1;
			NumTip22= edge2.Head;
		}

		//Update the lists of neighbors
		if(updateNeighbors)
			UpdateNeighbors(NumTip1, NumTip2, NumTip22);

		//Update the merged vertex
		DDBB->SetEdge(edgesInVertex2[k2], edge2);
	}

	//Delete the removed vertex from the neighbor list
	if(updateNeighbors)
		DeleteNeighbor(NumTip2);

	//Delete vertex 2 (and update the edges)
	DDBB->RemoveVertex(NumTip2, true);

	//Update the list of semi-lines
	if(updateSemilines){
		for (long i=0; i<(long)semiLines.size(); i++){
			if(semiLines[i][1] == NumTip2) semiLines[i][1]= NumTip1;
			if(semiLines[i][1] > NumTip2) semiLines[i][1]--;
			if(semiLines[i][2] == NumTip2) semiLines[i][2]= NumTip1;
			if(semiLines[i][2] > NumTip2) semiLines[i][2]--;
		}
	}
}


long Junctions2D::MergeCarefulTips()
//Merge neighbor tips of lines into common junctions that represent vertices

//This is a Nearest-Neighbor approach, as it is based on the assumption that
//tips of carefully drawn lines that share junctions must be neighbor
//from each other in a Delaunay triangulation of the cloud of tips

//The function merges sequentially the pairs of tips that best fit
//the following merging conditions:
//		- They are neighbor and belong to different lines.
//		  This is determined by the Delaunay triangulation, as all the vertices sharing
//		  a triangle with the current vertex are considered its neighbors.
//		  The list of neighbors is updated by converting into neighbors of the current vertex
//		  to those vertices that were neighbors of the vertices merged to it.

//		- Are closer than a threshold distance "maxDistance",
//		  which varies depending on the angle of their lines(*)
//		(*) maxDistance is weighted to prioritize perpendicular lines, as 
//		    an allowance is used to prioritize lines that meet at a square angle

//		- Lines to connect rotate less than a threshold rotation "maxRotation"

//Lines are parsed from longer to shorter,
//as it is asumed that short lines define auxiliary elements

//The Delaunay triangulation was adapted from:
//		Gilles Dumoulin's C++ implementation of Paul Bourke's Delaunay triangulation algorithm.
//		Source: http://paulbourke.net/papers/triangulate/


//Implemented by Pedro Company, 2017
{
	GetAllSemiLines();   //They are saved in the vector semiLines

	long DeletedVertices= 0;
	std::vector<bool> Merged(DDBB->GetSizeVertices(),false);

	//Merge only not yet merged vertices
	//CCueValences V;
	//V.GetValences(DDBB);
	for(long i=0; i<(long)DDBB->GetSizeVertices(); i++){
		//if(ReferApp.Cu.Va.Valences[i] > 1)   //Use this alternative to count collinear edges as a single edge
		if(DDBB->GetSizeEdgesInVertex(i) > 1)
			Merged[i]= true;
	}

	//Loop to merge isolated vertices
	for(long i=0; i<(long)semiLines.size(); i++){
		long tipI= semiLines[i][1];

		if(Merged[tipI])
			continue;

		double bestMeritI= -1.;

		//Loop to merge further isolated vertices to the current vertex
		bool merging;
		do{
			merging= false;

			long bestJ;
			double bestMeritJ= -1.;
			bool bestSwap= false;
			POINT2D bestJunction;

			for(long j= 0; j<(long)Neighbor[tipI].size(); j++){
				long tipJ= Neighbor[tipI][j];  //J-th neighbor tip of tipI

				if(Merged[tipJ])
					continue;  //The candidate closer tip is already merged
				if(tipJ == tipI)
					continue;  //Both tips were previously merged

				//Calculate the merit of the candidate junction
				POINT2D junction;
				bool swapTips;
				double meritJ= MeritTips(tipI, tipJ, junction, swapTips);

				//Save current best junction
				if(meritJ > bestMeritJ){
					bestJ= tipJ;
					bestMeritJ= meritJ;
					bestSwap= swapTips;
					bestJunction= junction;
				}
			}

			//Merge the best valid junction
			if(bestMeritJ >= 0){
				if(bestSwap){
					MergeTips(bestJ, tipI, bestJunction, bestMeritI, bestMeritJ,
							  true,  //Updates neighbors as well
							  true);  //Updates semi-lines as well
					Merged[bestJ]= true;
					Merged.erase(Merged.begin()+tipI);
					tipI= bestJ;  //tipI has been renumbered
				}
				else{
					MergeTips(tipI, bestJ, bestJunction, bestMeritI, bestMeritJ,
							  true,  //Updates neighbors as well
							  true);  //Updates semi-lines as well
					Merged[tipI]= true;
					Merged.erase(Merged.begin()+bestJ);
					if(bestJ < tipI)
						tipI--; //tipI has been renumbered
				}
				DeletedVertices++;
				merging= true;
			}
		}while(merging);
	}

	semiLines.clear();

	return DeletedVertices;
}


void Junctions2D::GetTripletSemiLines()
//List all the dangling semi-lines
//(These are lines whose tip is not connected to any other line)

//They are listed to search for triplets of lines that intersect each other
{
	//Calculate valences of vertices, to detect dangling semi-lines
	std::vector<long> val;
	for (long i=0; i<DDBB->GetSizeVertices(); i++){
		val.push_back(DDBB->GetSizeEdgesInVertex(i));
	}

	//List all the dangling semi-lines
	for (long i=0; i<DDBB->GetSizeEdges(); i++){
		CEdge edgeI= DDBB->GetEdge(PriorLine[i]);  //Longer lines are parsed first
		if(val[edgeI.Head]==1){
			SLX slx;
			slx.sl.push_back(PriorLine[i]);
			slx.sl.push_back(edgeI.Head);
			slx.sl.push_back(edgeI.Tail);
			sLines.push_back(slx);
		}
		if(val[edgeI.Tail]==1){
			SLX slx;
			slx.sl.push_back(PriorLine[i]);
			slx.sl.push_back(edgeI.Tail);
			slx.sl.push_back(edgeI.Head);
			sLines.push_back(slx);
		}
	}
}


void Junctions2D::RemoveDanglingSemiLine(long num)
//Removes one dangling semi-line
//and updates the numbers of the remaining semi-lines
{
	sLines.erase(sLines.begin()+num);

	//Update numbers of semi-lines
	//and remove references to the dangling line removed
	for (long j=0; j<(long)sLines.size(); j++){
		for (long k=0; k<(long)sLines[j].lineX.size(); k++){
			if(sLines[j].lineX[k] > num)
				//Update number of a subsequent semi-line
				sLines[j].lineX[k]--;
			else if(sLines[j].lineX[k] == num){
				//Remove reference to the dangling line removed
				sLines[j].lineX.erase(sLines[j].lineX.begin()+k);
				sLines[j].pointX.erase(sLines[j].pointX.begin()+k);
				k--;
			}
		}
	}
}


bool Junctions2D::SemiLinesIntersect(double x1, double y1, double x11, double y11,
									 double x2, double y2, double x22, double y22,
									 double &ta, double &tb, POINT2D &px)
//Returns true if semi-lines intersect in the valid range,
//which covers an asimetric region close to the first tip (both inside and outside the segments)
//Returns false otherwise

//First line contains the segment defined by tips (x1,y1) and (x11,y11)
//Second line contains the segment defined by tips (x2,y2) and (x22,y22)

//Also returns:
//    the parametric distance from the tips to the intersection point (ta, tb)
//    the intersection point (x, y)

//The parametric distances "t" from tips to the intersection are:
//   0 if intersection is in the tip,
//   1 if intersection is in the other tip of the same line, and
//  -1 if intersection is outside the segment and at a distance equal to the length of the tip
//The valid range for distances "t" is [CasualInTol, CasualOutTol]

//Based on: Paul Bourke, April 1989 (http://paulbourke.net/geometry/lineline2d/)
{
	double denom  = (y22-y2) * (x11-x1) - (x22-x2) * (y11-y1);
	double numera = (x22-x2) * (y1-y2) - (y22-y2) * (x1-x2);
	double numerb = (x11-x1) * (y1-y2) - (y11-y1) * (x1-x2);

	if (std::fabs(numera) < Accuracy && std::fabs(numerb) < Accuracy && std::fabs(denom) < Accuracy)
		return false;  //Lines are coincident

	if (std::fabs(denom) < Accuracy)
		return false;  //lines are parallel

	//Find the intersection
	ta = numera / denom;
	tb = numerb / denom;

	//Check if intersection is in the valid ranges
	if (ta > CasualInTol || ta < -CasualOutTol ||   //Intersection is in the valid range of the first line
		tb > CasualInTol || tb < -CasualOutTol)     //Intersection is in the valid range of the second line
		return false; //The intersection is far from, at least, one tip

	px.x = x1 + ta * (x11 - x1);
	px.y = y1 + ta * (y11 - y1);

	//Return the parametrized distances to the tips
	//if(ta < 0)
	//	ta*= -CasualInTol/CasualOutTol;
	//if(tb < 0)
	//	tb*= -CasualInTol/CasualOutTol;

	return true;
}


void Junctions2D::GetTripletsIntersections()
//The dangling semi-lines that intersect each semi-line I (if any) are tabulated in lineX
//Semi-lines in lineX are ordered from closer to farther to the first tip of line I

//Intersection points between line I and lines X are also saved (pointX)
{
	//Reset parametric distances used to order intersections
	std::vector<std::vector<double>> distTo;
	for (long i=0; i<(long)sLines.size(); i++){
		distTo.push_back(std::vector<double>());
	}

	long numP= 0;

	for (long i=0; i<(long)sLines.size(); i++){
		CVertex vi, vii;
		vi= DDBB->GetVertex(sLines[i].sl[1]);
		vii= DDBB->GetVertex(sLines[i].sl[2]);

		//Loop for the intersecting dangling lines
		for (long j=i+1; j<(long)sLines.size(); j++){
			CVertex vj, vjj;
			vj= DDBB->GetVertex(sLines[j].sl[1]);
			vjj= DDBB->GetVertex(sLines[j].sl[2]);

			//Save dangling semi-lines that intersect semi-line I inside the valid range
			double tti, ttj;
			POINT2D px;
			if(SemiLinesIntersect(vi.x, vi.y, vii.x, vii.y,
								  vj.x, vj.y, vjj.x, vjj.y,
								  tti, ttj, px)){

				if(tti < 0)
					distTo[i].push_back(-CasualInTol/CasualOutTol*tti);  //Increase distance for lines that intersect outside
				else
					distTo[i].push_back(tti);

				sLines[i].lineX.push_back(j);
				sLines[i].pointX.push_back(px);

				if(ttj < 0)
					distTo[j].push_back(-CasualInTol/CasualOutTol*ttj);  //Increase distance for lines that intersect outside
				else
					distTo[j].push_back(ttj);

				sLines[j].lineX.push_back(i);
				sLines[j].pointX.push_back(px);
			}
		}
	}

	//Bubble sort intersecting lines from closest to farther from the tip of each semi-line
	for (long i=0; i<(long)sLines.size(); i++){
		bool swapped;
		do{
			swapped= false;
			for (long k= 0; k<(long)distTo[i].size()-1; k++){
				if (distTo[i][k] > distTo[i][k+1]){
					std::swap(distTo[i][k], distTo[i][k+1]);
					std::swap(sLines[i].lineX[k], sLines[i].lineX[k+1]);
					std::swap(sLines[i].pointX[k], sLines[i].pointX[k+1]);
					swapped= true;
				}
			}
		}while(swapped);
	}

	//Remove dangling lines not in range
	for (long i=0; i<(long)sLines.size(); i++){
		//if(sLines[i].lineX.size() == 0){    //of any other intersecting line
		if(sLines[i].lineX.size() < 2){       //of at least other two intersecting lines
			RemoveDanglingSemiLine(i);
			i--;
		}
	}
}


void Junctions2D::RenumberTripletTips(long v1, long v2, long v3)
//Updates the local database by renumbering the merged tips

//Renumbers tips v2 and v3 as v1
//and reduces the numbers subsequent to v2 and v3
{
	if(v2 > v3)
		std::swap(v2, v3);

	for (long i=0; i<(long)sLines.size(); i++){
		if(sLines[i].sl[1] == v2 || sLines[i].sl[1] == v3)
			sLines[i].sl[1]= v1;
		else{
			if(sLines[i].sl[1] > v2 && sLines[i].sl[1] <= v3)
				sLines[i].sl[1]--;
			else if(sLines[i].sl[1] > v3)
				sLines[i].sl[1]-=2;
		}

		if(sLines[i].sl[2] == v2 || sLines[i].sl[2] == v3)
			sLines[i].sl[2]= v1;
		else{
			if(sLines[i].sl[2] > v2 && sLines[i].sl[2] <= v3)
				sLines[i].sl[2]--;
			else if(sLines[i].sl[2] > v3)
				sLines[i].sl[2]-=2;
		}
	}
}


void Junctions2D::MergeTriplet(long numDangling, long numNeighbor, CVertex center, double merit,
							   long line2, long line3)
//Defines a common junction in the global database,
//shared by lines numDangling, line2 and line3
{
	//Define vertices to merge
	long v1= sLines[numDangling].sl[1];
	long v2= sLines[sLines[numDangling].lineX[numNeighbor]].sl[1];
	long v3= sLines[sLines[numDangling].lineX[numNeighbor+1]].sl[1];

	//Update vertex v1 as common vertex
	center.merit= EmergentTripletsMerit * merit;
	DDBB->SetVertex(v1, center);

	//Connect vertices 2 and 3 to vertex1
	CEdge edgeJ= DDBB->GetEdge(line2);
	if(v2==edgeJ.Head)
		edgeJ.Head= v1;
	else
		edgeJ.Tail= v1;
	DDBB->SetEdge(line2, edgeJ);

	CEdge edgeK= DDBB->GetEdge(line3);
	if(v3==edgeK.Head)
		edgeK.Head= v1;
	else
		edgeK.Tail= v1;
	DDBB->SetEdge(line3, edgeK);

	//Remove old vertices
	if(v3 > v2){
		DDBB->RemoveVertex(v3, true);
		DDBB->RemoveVertex(v2, true);
	}
	else{
		DDBB->RemoveVertex(v2, true);
		DDBB->RemoveVertex(v3, true);
	}

	//Renumber tips in the local database of dangling semi-lines
	RenumberTripletTips(v1, v2, v3);

	//Remove the triplet from the lists of intersections of dangling semi-lines
	long dangling1= numDangling;
	long dangling2= sLines[numDangling].lineX[numNeighbor];
	long dangling3= sLines[numDangling].lineX[numNeighbor+1];

	RemoveDanglingSemiLine(dangling1);
	if(dangling2 > dangling1) dangling2--;
	if(dangling3 > dangling1) dangling3--;

	RemoveDanglingSemiLine(dangling2);
	if(dangling3 > dangling2) dangling3--;

	RemoveDanglingSemiLine(dangling3);
}


long Junctions2D::GetTriplets()
//Triplets of dangling semi-lines are connected to define a commmon junction
//if their three intersection points are close to each other, and close to their tips

//Only parses the two first semi-lines that intersect each semi-line
//(those that are closer to the tips)
{
	long NumRemovedTips= 0;

	//Loop to search for triplets of semi-lines that intersect close to each other
	bool triplets;
	do{
		triplets= false;

		//Check the longest unvisited dangling semi-line
		for (long i=0; i<(long)sLines.size(); i++){
			//Check if the two first lines that intersect the current semi-line
			//are the first to intersect to each other
			if((sLines[i].lineX.size() > 1 &&
				sLines[sLines[i].lineX[0]].lineX.size() > 1 &&
				sLines[sLines[i].lineX[1]].lineX.size() > 1)
				    &&
				    ((sLines[sLines[i].lineX[0]].lineX[0] == i                  && sLines[sLines[i].lineX[0]].lineX[1] == sLines[i].lineX[1]) ||
				     (sLines[sLines[i].lineX[0]].lineX[0] == sLines[i].lineX[1] && sLines[sLines[i].lineX[0]].lineX[1] == i))
				    &&
				    ((sLines[sLines[i].lineX[1]].lineX[0] == i                  && sLines[sLines[i].lineX[1]].lineX[1] == sLines[i].lineX[0]) ||
				     (sLines[sLines[i].lineX[1]].lineX[0] == sLines[i].lineX[0] && sLines[sLines[i].lineX[1]].lineX[1] == i))
					){

				//Calculate tolerance for the triplet
				long line2= sLines[sLines[i].lineX[0]].sl[0];
				long line3= sLines[sLines[i].lineX[1]].sl[0];

				double maxDistTripletI= (LengthEdge[sLines[i].sl[0]] +
										 LengthEdge[line2] +
										 LengthEdge[line3]) /3 *
										 maxDistanceTriplets;

				//Get intersections
				POINT2D X1= sLines[i].pointX[0];
				POINT2D X2= sLines[i].pointX[1];
				POINT2D X3= sLines[sLines[i].lineX[0]].pointX[0];
				if(X3==X1 || X3==X2)
					X3= sLines[sLines[i].lineX[0]].pointX[1];

				//Check if intersections are in range
				if(MODULE2D(X1.x, X1.y, X2.x, X2.y) < maxDistTripletI &&
				   MODULE2D(X2.x, X2.y, X3.x, X3.y) < maxDistTripletI &&
				   MODULE2D(X3.x, X3.y, X1.x, X1.y) < maxDistTripletI){

					//Calculate merit
					CVertex center ((X1.x + X2.x + X3.x)/3,
									(X1.y + X2.y + X3.y)/3, 0);

					double distCentroid= max(MODULE2D(center.x, center.y, X1.x, X1.y),
										 max(MODULE2D(center.x, center.y, X2.x, X2.y),
										 MODULE2D(center.x, center.y, X3.x, X3.y)));

					double merit= 1. - distCentroid/maxDistTripletI;

					//Merge the tips if the merit is non-negative
					if(merit >= 0){
						//Define a common junction in the database
						MergeTriplet(i, 0, center, merit, line2, line3);

						NumRemovedTips+= 2;
						triplets= true;
					}
					break;
				}
			}
		}
	}while(triplets);

	return NumRemovedTips;
}


long Junctions2D::GetPoorTriplets()
//Triplets of dangling lines are connected to define a commmon junction
//if their three intersection points are close to each other
{
	long NumRemovedTips= 0;

	//Check the longest unvisited dangling semi-line
	for (long i=0; i<(long)sLines.size(); i++){

		//Search for close intersecting lines
		long bestj;
		double bestMerit= -1.;
		CVertex bestCenter;
		long bestLine2, bestLine3;

		long jsize= (long)sLines[i].lineX.size();
		if(jsize > 1) for (long j=0; j<jsize-1; j++){

			long ksize= (long)sLines[sLines[i].lineX[j]].lineX.size();
			if(ksize > 1) for (long k=0; k<ksize-1; k++){

				long msize= (long)sLines[sLines[i].lineX[j+1]].lineX.size();
				if(msize > 1) for (long m=0; m<msize-1; m++){

					if(((sLines[sLines[i].lineX[j]].lineX[k] == i                     && sLines[sLines[i].lineX[j]].lineX[k+1] == sLines[i].lineX[j+1]) ||
						(sLines[sLines[i].lineX[j]].lineX[k] == sLines[i].lineX[j+1] && sLines[sLines[i].lineX[j]].lineX[k+1] == i))
					   &&
					   ((sLines[sLines[i].lineX[j+1]].lineX[m] == i                  && sLines[sLines[i].lineX[j+1]].lineX[m+1] == sLines[i].lineX[j]) ||
						(sLines[sLines[i].lineX[j+1]].lineX[m] == sLines[i].lineX[j] && sLines[sLines[i].lineX[j+1]].lineX[m+1] == i))
					  ){

						//Calculate tolerance for the triplet
						long line1= sLines[i].sl[0];
						long line2= sLines[sLines[i].lineX[j]].sl[0];
						long line3= sLines[sLines[i].lineX[j+1]].sl[0];

						double maxDistTripletI= (LengthEdge[line1] +
												 LengthEdge[line2] +
												 LengthEdge[line3]) /3 *
												 maxDistanceTriplets;

						//Get intersections
						POINT2D X1= sLines[i].pointX[j];
						POINT2D X2= sLines[i].pointX[j+1];
						POINT2D X3= sLines[sLines[i].lineX[j]].pointX[0];
						if(X3==X1 || X3==X2)
							X3= sLines[sLines[i].lineX[j]].pointX[1];

						//Check if intersections are in range
						if(MODULE2D(X1.x, X1.y, X2.x, X2.y) < maxDistTripletI &&
						   MODULE2D(X2.x, X2.y, X3.x, X3.y) < maxDistTripletI &&
						   MODULE2D(X3.x, X3.y, X1.x, X1.y) < maxDistTripletI){

							//Calculate merit
							CVertex center ((X1.x + X2.x + X3.x)/3,
											(X1.y + X2.y + X3.y)/3, 0);

							double distCentroid= max(MODULE2D(center.x, center.y, X1.x, X1.y),
												 max(MODULE2D(center.x, center.y, X2.x, X2.y),
												 MODULE2D(center.x, center.y, X3.x, X3.y)));

							double merit= 1. - distCentroid/maxDistTripletI;

							//Save the triplet with the best merit
							if(merit > bestMerit){
								bestj= j;
								bestMerit= merit;
								bestCenter= center;
								bestLine2= line2;
								bestLine3= line3;
							}
						}
					}
				}
			}
		}

		//Merge the best valid triplet
		if(bestMerit >= 0){
			long j= sLines[i].lineX[bestj];
			long j1= sLines[i].lineX[bestj+1];

			MergeTriplet(i, bestj, bestCenter, bestMerit, bestLine2, bestLine3);

			i--;
			if(i > j) i--;
			if(i > j1) i--;

			NumRemovedTips+= 2;
		}
	}

	return NumRemovedTips;
}


long Junctions2D::mergeCasualTriplets()
//Triplets of dangling lines are connected to define a commmon junction
//if their three intersection points are close to each other

//Lines are parsed as two "semi-lines":
//first and second tips are switched to get each semi-line
//A semi-line is dangling if its first tip is not shared
//by any other line (it is said that the "valence" of this tip is 1)

//The procedure is as follows:
//   1. All the dangling semi-lines are listed
//   2. The dangling semi-lines that intersect each semi-line I in the valid range (if any) are listed (lineX)
//...3. Semi-lines X are ordered from closest to farthest to the tip of semi-line I
//   4. If the two semilines that are close to the tip of a given semiline
//      are also close to their own tips, the triplet is selected
//   5  Then it is checked whether its mutual intersections are also close to each other
//...5. The three semi-lines that intersect close to each other are connected to a common junction
//      located in the centroid of the three intersection points
//   6. The merit of the common junction is assigned as
//      EmergentTripletsMerit*(1-distCentroid/maxDistTriplets),
//      where distCentroid is the distance between the centroid and the most distant intersection point
//   7. The procedure is repeated while valid triplets are found
//Longer semi-lines are parsed first
{
	long NumRemovedTips= 0;

	//List all the dangling semi-lines
	GetTripletSemiLines();

	//List all the valid intersections between dangling semi-lines
	//(and removes isolated dangling lines)
	GetTripletsIntersections();

	//Loop to search for triplets of lines that intersect close to each other
	//and close to the tips
	NumRemovedTips+= GetTriplets();

	//Loop to search for other (poor quality) triplets
	NumRemovedTips+= GetPoorTriplets();

	//Clear list of dangling semilines
	sLines.clear();

	//Clear list of lengths
	LengthEdge.clear();

	return NumRemovedTips;
}

void Junctions2D::GetDanglingSemiLines()
{
	//Calculate valences, to detect dangling lines
	for (long i=0; i<DDBB->GetSizeVertices(); i++){
		valence.push_back(DDBB->GetSizeEdgesInVertex(i));
	}

	//Get dangling semilines
	long numL= 0;

	for (long i=0; i<DDBB->GetSizeEdges(); i++){
		CEdge edgeI= DDBB->GetEdge(PriorLine[i]);  //Longer lines are parsed first

		if(valence[edgeI.Head]==1){
			dsLines.push_back(std::vector<long>());
			dsLines[numL].push_back(PriorLine[i]);
			dsLines[numL].push_back(edgeI.Head);
			dsLines[numL].push_back(edgeI.Tail);
			numL++;
		}

		if(valence[edgeI.Tail]==1){
			dsLines.push_back(std::vector<long>());
			dsLines[numL].push_back(PriorLine[i]);
			dsLines[numL].push_back(edgeI.Tail);
			dsLines[numL].push_back(edgeI.Head);
			numL++;
		}
	}
}


double Junctions2D::alwDangling(POINT2D p1, POINT2D p11, long junction)
//Calculate allowance, to prioritize perpendicular lines

//The allowance is calculated between the dangling line (p1-p11)
//and its most collinear line in the junction
{
	double allowance= 2.;   //max allowance, for normal lines

	POINT2D segment1 (p11.x-p1.x, p11.y-p1.y);
	double length1= MODULE_2D(p1, p11);

	//Loop to calculate the smallest allowance
	std::vector<long> edgesInVertex= DDBB->GetAllEdgesInVertex(junction);
	for (long j=0; j<(long)edgesInVertex.size(); j++){
		CEdge ej= DDBB->GetEdge(edgesInVertex[j]);
		CVertex pj, pjj;
		if (ej.Head == junction){
			pj= DDBB->GetVertex(ej.Head);
			pjj= DDBB->GetVertex(ej.Tail);
		}
		else{
			pj= DDBB->GetVertex(ej.Tail);
			pjj= DDBB->GetVertex(ej.Head);
		}

		POINT2D segment2 (pjj.x-pj.x, pjj.y-pj.y);
		double length2= MODULE_2D(pj, pjj);

		double allowancej= (2. - std::fabs(DOT_PRODUCT_2D(segment1, segment2))/length1/length2);

		if(allowancej < allowance)
			allowance= allowancej;
	}

	return allowance;
}


double Junctions2D::meritCasualDangling(POINT2D p1, POINT2D p11, double length,
										long junction)
//Returns the merit of merging the dangling segment p1-p11
//and the junction, which must be smaller than maxDistanceDangling
//while its closest point must be in range

//The merit of the junction is linearly reduced

//One merit equals 1 if the dangling line passes through the junction,
//or 0 if the dangling line passes at a distance of maxDistanceDangling or greater

//Another merit is calculated depending on the distance between the tip of the semi-line
//and the point of perpendicular projection of the junction on the line

//The minimum of both merits is returned as merit of the close junction

//The calculation of the minimum distance is based on:
	//Given the point C (Cx,Cy) and the segment AB (Ax,Ay) to (Bx,By).
	//Let P be the point of perpendicular projection of C on AB
	//The parameter t, which indicates P's position along AB,
	//is computed by the dot product of AC and AB
	//divided by the square of the length of AB
	//Philip Nicoletti (2002) CodeGuru
{
	CVertex junc= DDBB->GetVertex(junction);

	double t = ((junc.x - p1.x) * (p11.x - p1.x) + (junc.y - p1.y) * (p11.y - p1.y)) / (length*length);

	//Check if the distance normal intersects in the valid range
	if (t > CasualInTol ||  //The intersection is valid if it occurs in the half line close to the first tip
		t < -CasualOutTol)  //The intersection is also valid if it occurs in the near extension of the segment
		return -1;

	//Distance
	double dX = junc.x - (p1.x + t * (p11.x - p1.x));
	double dY = junc.y - (p1.y + t * (p11.y - p1.y));
	double dist= sqrt(dX*dX + dY*dY);

	//Check if the distance is in the valid range
	if (dist/length > CasualInTol)
		return -1;

	//Merits
	double merit1= (1 - (dist/(length*maxDistanceDangling*alwDangling(p1, p11, junction))));

	double merit2;
	if(t >= 0)
		merit2= (1 - t/CasualInTol);
	else
		merit2= (1 - t/(-CasualOutTol));

	return min(merit1, merit2);
}


long Junctions2D::mergeCasualDanglingLines()
//Tips of dangling semi-lines are connected to the closest junction
//if they are closer than a threshold distance

//This is based on the assumption that the line-drawing depicts a polyhedral shape,
//thus dangling lines are considered uncommon, and are merged if they pass near enough to a junction

//The procedure is as follows:
//    1.One dangling semi-line is selected (I)
//    2.For every junction J (tip of valence higher than 1)
//      closer than a threshold (maxDistDangling)
//      and within the valid range of semi-line I,
//      the merit of merging I to J is calculated
//    3.The junction JJ with the higher positive merit is selected
//    4.The tip of the dangling line I is added to the junction JJ,
//      only if the reduced merit of JJ is non-negative
//    5.The procedure is repeated for all dangling semi-lines
{
	long NumRemovedTips= 0;

	//Get dangling semilines
	GetDanglingSemiLines();

	//Loop to merge dangling semi-lines
	for(long i=0; i<(long)dsLines.size(); i++){
		CVertex vi= DDBB->GetVertex(dsLines[i][1]);
		POINT2D di(vi.x, vi.y);
		CVertex vii= DDBB->GetVertex(dsLines[i][2]);
		POINT2D dii(vii.x, vii.y);

		//double lengthi= LengthEdge[dsLines[i][0]];
		double lengthi= MODULE_2D(di, dii);
		if (lengthi == 0)
			continue;

		//Loop to search for the junction most meritorious to merge
		double maxmerit= -1;
		long junCloser;

		for (long j=0; j<DDBB->GetSizeVertices(); j++){
			if(valence[j] < 2)
				continue;  //Do not merge danglindg lines to another dangling line
			if(j==dsLines[i][1] || j==dsLines[i][2])
				continue;  //Do not merge danglindg lines to themselves

			double merit= meritCasualDangling(di, dii, lengthi, j);

			if(merit > maxmerit){
				maxmerit= merit;
				junCloser= j;
			}
		}

		//Connect dangling semi-line to its closest junction
		if(maxmerit >= 0){
			//Update merit of the junction
			CVertex Junction= DDBB->GetVertex(junCloser);
			//double newMerit= Junction.merit- EmergenceDanglingDeMerit * (1-maxmerit);  //Merit of the junction is reduced, if a poor dangling line is added to it
			double newMerit= min(Junction.merit, maxmerit);

			if(newMerit >= 0){
				Junction.merit= newMerit;
				DDBB->SetVertex(junCloser, Junction);

				//Remove dangling tip
				CEdge edgeI= DDBB->GetEdge(dsLines[i][0]);
				if(edgeI.Head == dsLines[i][1])
					edgeI.Head= junCloser;
				else
					edgeI.Tail= junCloser;
				DDBB->SetEdge(dsLines[i][0], edgeI);
				DDBB->RemoveVertex(dsLines[i][1], true);

				//Update pointers and flags
				valence[junCloser]++;
				valence.erase(valence.begin() + dsLines[i][1]);
				NumRemovedTips++;

				//Update the list of semi-lines
				long tipD= dsLines[i][1];
				for (long i=0; i<(long)dsLines.size(); i++){
					if(dsLines[i][1] == tipD) dsLines[i][1]= junCloser;
					if(dsLines[i][1] > tipD) dsLines[i][1]--;
					if(dsLines[i][2] == tipD) dsLines[i][2]= junCloser;
					if(dsLines[i][2] > tipD) dsLines[i][2]--;
				}
			}
		}
	}

	return NumRemovedTips;
}


long Junctions2D::GetJunctions(CDB_Model *i_DDBB)
//Merge tips of lines into common 2D junctions
//that are assumed to represent 3D vertices

//The goal is merging first those junctions most clearly perceived,
//while giving a second chance to poorly drawn junctions

//The approach follows three stages:
//		- Firstly, Neighbor tips are merged, iff they fit a threshold distance and an orientation limit
//		- Secondly, triplets of dangling lines are merged (if they intersect close to each other)
//		- Finally, isolated dangling lines are merged (if they pass close to a junction)

//The first stage is intended to reificate careful vertices,
//while the second and the third stages are aimed at emerging casual vertices

//Implemented by Pedro Company, 2017

//Published:
//	Company P., Plumed R., Varley P.A.C., Camba J.D. (2019) 
//	Algorithmic Perception of Vertices in Sketched Drawings of Polyhedral Shapes. 
//	ACM Transactions on Applied Perception, Vol. 16, Issue 3. Article No. 18. pp. 1-19. 


//A pre-stage was introduced in 2024 by Pedro Company
//It is a replica of the reification stage, 
//but measuring the separations between junctions to merge in screen coordinates, 
//and giving priority to edges parallel to the coordinate axes:

//		Measuring in screen coordinates activates the mouse wheel 
//		as an indirect adjustment of the precision that is considered acceptable.
//		It's the philosophy that "you get what you see"

//		Giving priority to principal directions prioritizes cubic corners, 
//		although merging vertices has traditionally been considered 
//		a step prior to calculating principal directions, 
//		so working with "provisional" directions becomes necessary.
{
	//Point the local database to the global database
	DDBB= i_DDBB;

	long NumRemovedTips= 0;
	ResetMerits();

	//Merges cubic corners
	//--------------------
	if(MergeCubicCorners){
		//Merge triplets of isolated vertices
		NumRemovedTips+= GetCubicCornerJunctions(DDBB);
	}

	//Reorder to place longer lines first
	ReorderLines();

	//Reify careful tips
	//------------------
	if(MergeCarefulLineTips && maxDistance !=0){
		//Find neighbor tips by way of Delaunay triangulation
		Delaunay D;
		if(D.NeighborTips(DDBB, Neighbor)){
			//Merge close tips
			NumRemovedTips+= MergeCarefulTips();
		}
		else{
			AfxMessageBox("Tips of careful lines not merged, as no valid triangulation was found");
		}
		Neighbor.clear();
	}

	//Emerge casual tips
	//------------------
	if(MergeCasualLineTips){
		//Merge dangling triplets of tips
		if(ReMergeTriplets && maxDistanceTriplets !=0){
			NumRemovedTips+= mergeCasualTriplets();
		}

		//Merge dangling isolated tips
		if(ReMergeDangling && maxDistanceDangling !=0){
			NumRemovedTips+= mergeCasualDanglingLines();
		}
	}

	return NumRemovedTips;
}


void CRefine2D::RemoveShortEdges(CDB_Model *DDBB)
//Lines of the line drawing shorter than a tolerance
//are removed in the assumption that they do not represent actual edges
{
	//Calculate the threshold to consider edges no valid
	double aXmax, aYmax, aZmax, aXmin, aYmin, aZmin;
	DDBB->GetBoundingBox(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
	double Range= min(std::fabs(aXmax-aXmin), std::fabs(aYmax-aYmin));
	double shortEdge= Range * (ReferApp.R2D.ToleranceIgnoreShortEdges);

	//Loop to remove short edges
	for (long i= DDBB->GetSizeEdges()-1; i>=0; i--){
		if(DDBB->GetLength2D(i) < shortEdge){
			DDBB->RemoveEdge(i, true);
		}
	}
}


void CRefine2D::Split_T_Vertices(CDB_Model *DDBB)
//This functions tidies-up T vertices
//by fixing its junction and splitting its crossing edge

//Edges with one tip of another edge closer than a tolerance
//are asumed to define a T vertex,
//iff they are not connected to any other edge

//Changes apply to both sides of the T:
//    1 The tip of the ending edge (the lower side of the T)
//      is moved to be placed exactly at the intersection between both lines

//    2 The crossing edge (the upper side of the T)
//      is splitted into two new edges
//      sharing the close vertex of the ending edge
//      (This change only applies if Split flag is active)
{
	//Calculate the threshold to consider tip close enough
	double aXmax, aYmax, aZmax, aXmin, aYmin, aZmin;
	DDBB->GetBoundingBox(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
	double aRango= min(std::fabs(aXmax-aXmin), std::fabs(aYmax-aYmin));
	double close= aRango * (ReferApp.R2D.ToleranceSplitTVertices);

	//Loop over all the edges of the model
	for (long i= 0; i<DDBB->GetSizeEdges(); i++){
		CEdge EdgeI= DDBB->GetEdge(i);
		CVertex line_start= DDBB->GetVertex(EdgeI.Tail);
		CVertex line_end= DDBB->GetVertex(EdgeI.Head);

		//Loop to search vertices of dangling edges close to the current edge
		for (long j= 0; j<DDBB->GetSizeVertices(); j++){
			if(j==EdgeI.Tail || j==EdgeI.Head)
				continue;  //The vertex is an tip of the edge
			if(DDBB->GetSizeEdgesInVertex(j) > 1)
				continue;  //The vertex is shared by more than one edge

			CVertex VertexJ= DDBB->GetVertex(j);
			double NewVertexX, NewVertexY;

			if(GetDistancePointIntoSegment(VertexJ.x, VertexJ.y,
											line_start.x, line_start.y,
											line_end.x, line_end.y,
											NewVertexX, NewVertexY) < close){

				//Move vertex j to the intersection point
				VertexJ.x= NewVertexX;
				VertexJ.y= NewVertexY;
				DDBB->SetVertex(j, VertexJ);

				//Split edge i
				if(ReferApp.R2D.Split_T_Vertices){
					DDBB->RemoveEdge(i, false);

					CEdge Edge1= EdgeI;
					Edge1.Head= j;
					DDBB->AddEdge(Edge1);

					CEdge Edge2= EdgeI;
					Edge2.Tail= j;
					DDBB->AddEdge(Edge2);

					i--;
					break;
				}
			}
		}
	}
}


void CRefine2D::MergeCollinearEdges(CDB_Model *DDBB)
//Two edges sharing one tip and nearly parallel to each other
//are merged into one single edge, and the common vertex is removed

//(Only if the two candidate edges are the only ones connected to the common vertex)

//To be considered nearly parallel, the angle they determine 
//must be below a threshold (ToleranceMergeCollinearEdges)
{
	//Loop over the current drawing
	//(The loop is reversed to reduce database workload while removing edges and vertices)
	for (long i= DDBB->GetSizeVertices()-1; i>=0; i--){
		//Search for vertices of valence 2
		if(DDBB->GetSizeEdgesInVertex(i) == 2){
			long numEdge1= DDBB->GetEdgeInVertex(i, 0);
			long numEdge2= DDBB->GetEdgeInVertex(i, 1);

			//Search for consecutive edges "nearly" parallel to each other
			double angle= DDBB->GetAngle2D_PI_half(numEdge1, numEdge2);

			if(min(angle, std::fabs(angle-NUMBER_PI)) < ReferApp.R2D.ToleranceMergeCollinearEdges){
				//Find merged edge
				CEdge Arista1= DDBB->GetEdge(numEdge1);
				CEdge Edge= Arista1;
				Edge.Head= Arista1.Head;
				if(Arista1.Head == i) Edge.Head= Arista1.Tail;

				CEdge Arista2= DDBB->GetEdge(numEdge2);
				Edge.Tail= Arista2.Head;
				if(Arista2.Head == i) Edge.Tail= Arista2.Tail;

				//Update first edge
				DDBB->SetEdge(numEdge1, Edge);

				//Remove second edge
				DDBB->RemoveEdge(numEdge2, false);

				//Remove common vertex
				DDBB->RemoveVertex(i, true);
			}
		}
	}
}


void CRefine2D::SplitNonTwoManifoldVertices(CDB_Model *DDBB)
//Splits non-2-manifold vertices to produce a set of manifold shapes

//One vertex is considered non-two-manifold if:
//	1. Its valence is 6 or greater
//	2. Removing it from the drawing increases the number of subgraphs

//The tip of every edges converging at the non-two-manifold vertex
//is converted into a new vertex (with the same location)

//Then, the non-two-manifold vertex is remerged by grouping
//all the tips of edges that converged initially to it and
//belong to the same subgraph raised after removing it
{
	//Create a local copy of the database
	CDB_Model DBAux= *DDBB;
	bool reconnected= false;

	//Loop over the current vertices
	//(The loop is reversed to reduce database workload while removing vertices)
	for (long i= DBAux.GetSizeVertices()-1; i>=0; i--){
		if(DBAux.GetSizeEdgesInVertex(i) >= 6){

			//Split candidate non-two-manifold vertex
			CCueSubGraphs SG;
			std::vector <long> NewVertices;
			SG.SplitVertex(&DBAux, i, NewVertices);

			long SubgraphCount;                   //Number of subgraphs
			std::vector <long> SubgraphForVertex; //Number of the subgraph where each vertex belongs
			std::vector <long> SubgraphForEdge;   //Number of the subgraph where each edge belongs

			SG.CalculateSubgraphs(&DBAux,
								  SubgraphCount,
								  SubgraphForVertex,
								  SubgraphForEdge);

			if(SubgraphCount > 1){
				//Reconnect all the splitted vertices belonging to the same subgraph
				for (long j= 0; j<SubgraphCount; j++){
					bool first= true;
					long VertexFirst;

					for (long k= 0; k<(long)NewVertices.size(); k++){
						if(SubgraphForVertex[NewVertices[k]] == j){
							if(first){
								//Save the first vertex to merge
								first= false;
								VertexFirst= NewVertices[k];
							}
							else{
								//Merge the next vertex to merge
								DBAux.MergeVertices(VertexFirst, NewVertices[k]);
								reconnected= true;

								//Update the list of subgraphs
								SubgraphForVertex.erase(SubgraphForVertex.begin()+NewVertices[k]);

								//Update the list of new vertices
								for (long kk= k+1; kk<(long)NewVertices.size(); kk++){
									NewVertices[kk]= NewVertices[kk]-1;
								}
								NewVertices.erase(NewVertices.begin()+k);

								k--;
							}
						}
					}
				}
			}
		}
	}

	//Update the external database
	if(reconnected) *DDBB= DBAux;
}


void CRefine2D::SplitNonTwoManifoldEdges(CDB_Model *DDBB)
//Splits non-2-manifold edges to produce a set of manifold shapes

//One edge is considered non-two-manifold if:
//	1. The valences of its two vertices are 5 or greater
//	2. Removing it from the drawing 
//	   (and disconnecting from each other the other edges connected to it)
//	   increases the number of subgraphs

//The procedure is as follows:

//	- Both vertices of the candidate non-2-manifold edge are splitted. 
//	  This implies that the tip of every edge converging at one of the vertices 
//	  of the non-2-manifold edge is converted into a new vertex (with the same location),
//	  until none other edge is connected to any of the endpoints of the the non-2-manifold edge.<br>

//	- Subgraphs of the resulting graph are calculated, 
//	  and the subgraph defined by the non-2-manifold edge is removed.<br>

//	- Iff the graph still contains two subgraphs (thus proving that the edge is non-2-manifold), 
//	  all the splitted vertices belonging to the same subgraph are remerged.
//	  This implies that the vertices are remerged by grouping
//	  all the tips of edges that converged initially to each of the endpoints of
//	  the non-2-manifold edge and belong to the same subgraph raised after removing it.<br>

//	- One copy of the non-two-manifold edge is connected to the remerged vertices of each subraph,
//	  while the original (now uncoupled) non-2-manifold edge is removed.
{
	//Create a local copy of the database
	CDB_Model DBAux= *DDBB;
	bool reconnected= false;

	//Loop over the current edges
	//(The loop is reversed to reduce database workload while removing edges)
	for (long i= DBAux.GetSizeEdges()-1; i>=0; i--){
		CEdge EdgeI= DBAux.GetEdge(i);
		//std::vector <long> VertexFirst;
		//VertexFirst.push_back(EdgeI.Head);
		//VertexFirst.push_back(EdgeI.Tail);

		if(DBAux.GetSizeEdgesInVertex(EdgeI.Tail) >= 5 &&
		   DBAux.GetSizeEdgesInVertex(EdgeI.Head) >= 5){

			//Split vertices of candidate non-two-manifold edge
			CCueSubGraphs SG;
			std::vector <std::vector <long>> NewVertices;
			NewVertices.push_back(std::vector <long>());
			SG.SplitVertex(&DBAux, EdgeI.Tail, NewVertices[0]);


			NewVertices.push_back(std::vector <long>());
			SG.SplitVertex(&DBAux, EdgeI.Head, NewVertices[1]);

			for (long kk= 0; kk<(long)NewVertices[0].size(); kk++){  //Renumber the list of fist new vertices, after removing the second endpoint
				NewVertices[0][kk]= NewVertices[0][kk]-1;
			}

			long SubgraphCount;                   //Number of subgraphs
			std::vector <long> SubgraphForVertex; //Number of the subgraph where each vertex belongs
			std::vector <long> SubgraphForEdge;   //Number of the subgraph where each edge belongs

			SG.CalculateSubgraphs(&DBAux,
								  SubgraphCount,
								  SubgraphForVertex,
								  SubgraphForEdge);

			//Remove the subgraph of the non 2-manifold edge
			std::vector <long> counter(SubgraphCount, 0);
			for (long ii= 0; ii<(long)SubgraphForVertex.size(); ii++){
				counter[SubgraphForVertex[ii]]++;
			}
			for (long jj= 0; jj<SubgraphCount; jj++){
				if(counter[jj] == 2){
					SubgraphCount--;
					for (long ii= 0; ii<(long)SubgraphForVertex.size(); ii++){
						if(SubgraphForVertex[ii] == jj){
							SubgraphForVertex[ii] = -1;
						}
					}
					break;
				}
			}

			if(SubgraphCount > 1){
				//Reconnect all the splitted vertices belonging to the same subgraph
				for (long j= 0; j<SubgraphCount; j++){
					std::vector <long> VertexFirst;
					VertexFirst.push_back(-1);
					VertexFirst.push_back(-1);

					//Loop over the two vertices of the edge
					//(In reverse order, to avoid renumbering New vertices of the first endpoint)
					for (long vv= 0; vv<2; vv++){
						bool first= true;
						//long VertexFirst;

						for (long k= 0; k<(long)NewVertices[vv].size(); k++){
							if(SubgraphForVertex[NewVertices[vv][k]] == j){
								if(first){
									//Save the first vertex to merge
									first= false;
									//VertexFirst= NewVertices[vv][k];
									VertexFirst[vv]= NewVertices[vv][k];
								}
								else{
									//Merge the next vertex to merge
									DBAux.MergeVertices(VertexFirst[vv], NewVertices[vv][k]);
									reconnected= true;

									//Update the list of subgraphs
									SubgraphForVertex.erase(SubgraphForVertex.begin()+NewVertices[vv][k]);

									//Update the list of new vertices
									for (long kk= k+1; kk<(long)NewVertices[vv].size(); kk++){
										NewVertices[vv][kk]= NewVertices[vv][kk]-1;
									}
									NewVertices[vv].erase(NewVertices[vv].begin()+k);
									if(vv==0){
										for (long kk= 0; kk<(long)NewVertices[1].size(); kk++){
											NewVertices[1][kk]= NewVertices[1][kk]-1;
										}
									}

									k--;
								}
							}
						}
					}
					//Connect one copy of the non 2-manifold edge to the merged vertices
					CEdge EdgeIvv= EdgeI;
					EdgeIvv.Head= VertexFirst[0];
					EdgeIvv.Tail= VertexFirst[1];
					DBAux.AddEdge(EdgeIvv);
				}
			}
			//Remove the non two-manifold edge
			DBAux.RemoveEdge(i, true);
			i--;
		}
	}

	//Update the external database
	if(reconnected) *DDBB= DBAux;
}


double CRefine2D::CalculeMUab(double a, double b, double x)
// Calcule continuos compliance factor
//It is a "dumped" factor that equals 1 for null divergence (x=a)
//and descends to zero like a normal distribution curve for x different to a (with sigma=b)

//In other words, x is the value to be checked,
//a is the nominal value, and b is a reasonable max deviation

//Based on:
//	H Lipson, M Shpitalni
//	Optimization-based reconstruction of a 3D object
//	from a single freehand line drawing
//	Computer-Aided Design, 28(8) 651-663, 1996
{
	double dValor= 1.1 * exp( - pow((x-a)/b, 2) ) - 0.1;
	assert(!_isnan(dValor));

	return max(0, dValor);
};


void CRefine2D::WeightsParallelism(CDB_Model *DDBB,
								   std::vector<std::vector<double>> &MatrixParallelism,
								   double ErrorParallelism2D)
// A matrix is calculated to determine how much edge I is parallel to edge J in the 2D drawing
// The matrix is symmetric
// Main dialgonal is null, as it makes no sense measuring the self-parallelism of edges
{
	for(long i=0; i<DDBB->GetSizeEdges(); i++ ){
		MatrixParallelism[i][i]= 0;

		for(long j=i+1; j<DDBB->GetSizeEdges(); j++) {
			//Calculate weight as a damped function
			//which equals 1 when they are parallel (angle 0)
			//reduce a little until reaching the threshold
			//and reduces to zero for values slightly larger than the threshold
			MatrixParallelism[i][j]=
			MatrixParallelism[j][i]= CalculeMUab( 0, ErrorParallelism2D, DDBB->GetAngle2D_PI_half(i, j) );
		};
	}
}


void CRefine2D::WeightsCollinearity(CDB_Model *DDBB,
									std::vector<std::vector<double>> &MatrixCollinearity,
									double ErrorCollinearity2D)
//Explore all possible pairings between drawing edges
//and calculate a collinearity weight for each pairing.

//Current implementation assumes that collinearity in the strict sense 
//can only be applied to consecutive lines (that share a vertex).

//Those pairs with non-zero weight will be used to increase the cost of collinearity
//in the event that they are not collinear in the tentative drawing.

//Previous implementations compared all possible pairs of axes as potentially collinear
{
	//Loop over the current drawing
	for (long i= 0; i<DDBB->GetSizeVertices(); i++){
		//Search for consecutive edges
		if(DDBB->GetSizeEdgesInVertex(i) == 2){   // Check only vertices of valence 2
			long numEdge1= DDBB->GetEdgeInVertex(i, 0);
			long numEdge2= DDBB->GetEdgeInVertex(i, 1);

			////Check "nearly" parallelism of consecutive edges
			//double angle= DDBB->GetAngle2D_PI_half(numEdge1, numEdge2);
			//double weight = CalculeMUab( 0, ErrorCollinearity2D, angle);  //Wrong calculation: lines may overlap!!

			CEdge E1= DDBB->GetEdge(numEdge1);
			CEdge E2= DDBB->GetEdge(numEdge2);

			//Encompassed angle around the pivot (vertex "i")
			double angle= 0.;
			if(E1.Head == E2.Head){
				angle= DDBB->GetAngle2D_PI(E1.Tail, E1.Head, E2.Tail);
			}
			else if(E1.Head == E2.Tail){
				angle= DDBB->GetAngle2D_PI(E1.Tail, E1.Head, E2.Head);
			}
			else if(E1.Tail == E2.Head){
				angle= DDBB->GetAngle2D_PI(E1.Head, E1.Tail, E2.Tail);
			}
			else if(E1.Tail == E2.Tail){
				angle= DDBB->GetAngle2D_PI(E1.Head, E1.Tail, E2.Head);
			}
			else //Edges do not share a common vertex!
				continue;

			//Collinear lines sharing a common vertex must encompass an angle of PI
			double weight = CalculeMUab( 0, ErrorCollinearity2D, std::fabs(NUMBER_PI - angle));

			if(weight > Accuracy){
				MatrixCollinearity[numEdge1][numEdge2]= weight;
				MatrixCollinearity[numEdge2][numEdge1]= weight;
			}
			//else{
			//	MatrixCollinearity[numEdge1][numEdge2]= 0;
			//	MatrixCollinearity[numEdge2][numEdge1]= 0;
			//}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////

//What follows is the deprecated Ana Piquer's formulation, 
//which differed from the original (deprecated as well) from Lipson and Shpitalni


//// ANA++ La colinealidad de lineas ahora está formulada según las distancias de dos puntos a la primera arista
////BAjo esta puenteada la misma función pero con mi formulacion: a base de angulos.
///*void PesosColinealidadLineas2D(CReferDoc* pDoc, double **MatrizColinealidad, double ErrorColinealidad) {
//	
//	long i,j, tam=pDB->ListaAristas.lTamanyo;
//	CEdge Arista;
//	TListaLong Aristas;
//	double Distancia, Distancia1, Distancia2;
//	bool CompartenCara=false, bien=false;
//    double modulo, w;
//	CVertex v1,v2,v3,v4;
//	CVertex PuntoCorte1, PuntoCorte2;
//	NuevaListaLong(&Aristas);
//
//	for (i=0;i<tam;i++){
//
//		MatrizColinealidad [i][i] =0;
//		ObtenListaAristas(&pDB->ListaAristas, i, &Arista);
//		NuevoVertice(&v1);
//		NuevoVertice(&v2);
//		ObtenListaVertices(&pDB->ListaVertices, Arista.Head, &v1);
//		ObtenListaVertices(&pDB->ListaVertices, Arista.Tail, &v2);
//
//		for (j=i+1;j<tam;j++) {
//		//	assert( !_isnan(MatrizColinealidad[i][j]) );
//
////			if(MatrizColinealidad[i][j]> Accuracy){
//
//			ObtenListaAristas(&pDB->ListaAristas, j, &Arista);
//			NuevoVertice(&v3);
//			NuevoVertice(&v4);
//			ObtenListaVertices(&pDB->ListaVertices, Arista.Head, &v3);
//			ObtenListaVertices(&pDB->ListaVertices, Arista.Tail, &v4);
//				
//			NuevoVertice(&PuntoCorte1);
//			NuevoVertice(&PuntoCorte2);
//
//			DistanciaPuntoRecta(&v1, &v2, &v3, &Distancia1, &PuntoCorte1);
//			DistanciaPuntoRecta(&v1, &v2, &v4, &Distancia2, &PuntoCorte2);
//			Distancia = std::fabs (Distancia1) + std::fabs(Distancia2);
//				//La distancia la voy a normalizar en función de la longitud de la primera linea
//				//comparada, porque creo que si la normalizo según el rango del dibujo 
//			//la comparación va a ser excesiva 
//			modulo= modulo2d(v1, v2);
//			Distancia /= modulo;
//
//			DestruyeVertice(&PuntoCorte1);
//			DestruyeVertice(&PuntoCorte2);
//			w = CalculeUab( 0, ErrorColinealidad, Distancia);
//				
//			if(w >Accuracy){
//			/*		CompartenCara=false;
//					for(k=0; k<GetSize(&pDB->CFaces); k++){
//						CFace Cara= pDB->GetFace(k);
//						if((Cara.SearchEdgeInFace(i, posicion1)) 
//							&& (Cara.SearchEdgeInFace(j, posicion2)))
//						{
//							CompartenCara=true;
//							break;
//						}
//					}
//					if (CompartenCara==true){
//						MatrizColinealidad[i][j]= w; 
//						MatrizColinealidad[j][i]= w;}
//					else
//						MatrizColinealidad[j][i]= MatrizColinealidad[i][j]= 0.0;
//			*/
//				
///*				MatrizColinealidad[i][j]= w; 
//				MatrizColinealidad[j][i]= w;
//			}
//			else 
//				MatrizColinealidad[j][i]= MatrizColinealidad[i][j]= 0.0;
//
//			DestruyeVertice(&v3);
//			DestruyeVertice(&v4);
//		}
//		DestruyeVertice(&v1);
//		DestruyeVertice(&v2);
//	}
//}


//bool CRefine2D::linesOverlap(CVertex v1, CVertex v2, CVertex v3, CVertex v4)
////Check that lines (v1,v2) and (v3,v4) do not overlap
//
////Return false if lines do not overlap, true otherwise
//{
//	if (v1.x > v2.x) std::swap(v1, v2);  //line 1 left to right
//	if (v3.x > v4.x) std::swap(v3, v4);  //line 2 left to right
//
//	if (v1.x < v3.x){
//		//if (v2.x <= v4.x)  //line 2 after line 1
//		if (v2.x <= v3.x)  //line 2 after line 1
//			return false;
//	}
//	else if (v3.x < v1.x){
//		if (v4.x <= v1.x)  //line 2 before line 1
//			return false;
//	}
//
//	else{ //if (v1.x == v3.x)
//		//lines are vertical
//		if (v1.y > v2.y) std::swap(v1, v2);  //line 1 up
//		if (v3.y > v4.y) std::swap(v3, v4);  //line 2 up
//
//		if (v2.y <= v3.y)  //line 2 after line 1
//			return false;
//
//		if (v4.y <= v1.y)  //line 2 before line 1
//			return false;
//
//
//		if (v1.y<v2.y){  //line 1 up
//			if (v3.y<v4.y){  //line 2 up
//				if (v2.y<=v3.y)
//					return false;
//			}
//			else{  //line 2 down
//				if (v2.y<=v4.y)
//					return false;
//			}
//		}
//		else{  //line 1 down
//			if (v3.y<v4.y){  //line 2 up
//				if (v1.y<=v3.y)
//					return false;
//			}
//			else{  //line 2 down
//				if (v1.y<=v4.y)
//					return false;
//			}
//		}
//	}
//	return true;
//}


//void CRefine2D::WeightsCollinearity(CDB_Model *DDBB,
//									std::vector<std::vector<double>> &MatrixCollinearity,
//									double ErrorCollinearity2D)
//{
//	for (long i=0; i<DDBB->GetSizeEdges(); i++){
//		MatrixCollinearity [i][i] =0;
//		CEdge AristaI= DDBB->GetEdge(i);
//		CVertex v1= DDBB->GetVertex(AristaI.Head);
//		CVertex v2= DDBB->GetVertex(AristaI.Tail);
//
//		for (long j=i+1; j<DDBB->GetSizeEdges(); j++) {
//			CEdge AristaJ= DDBB->GetEdge(j);
//			CVertex v3= DDBB->GetVertex(AristaJ.Head);
//			CVertex v4= DDBB->GetVertex(AristaJ.Tail);
//
//			//if (!linesOverlap(v1, v2, v3, v4)){
//			CVector VectorUnitario2D1= GetUnitVector2D(v1, v2);
//			CVector VectorUnitario2D2= GetUnitVector2D(v3, v4);
//			double dProductoEscalar = DOT_PRODUCT_2D( VectorUnitario2D1, VectorUnitario2D2 );
//			//if (dProductoEscalar < 1+Accuracy && dProductoEscalar > 1) dProductoEscalar=1;
//			//if (dProductoEscalar < -1 && dProductoEscalar > -1-Accuracy) dProductoEscalar=-1;
//			if(std::abs(dProductoEscalar > 1.0))
//				dProductoEscalar = NORMALIZE(dProductoEscalar);
//			double angle=acos(dProductoEscalar);
//			if (std::fabs(angle)>(NUMBER_PI/2)) angle = std::fabs(angle-NUMBER_PI);
//
//			double weight = CalculeMUab( 0, ErrorCollinearity2D, angle);
//
//			if(weight < Accuracy){
//				MatrixCollinearity[i][j]= 0.;
//				MatrixCollinearity[j][i]= 0.;
//			}
//
//			else{
//				double angulo3;
//				if ((std::fabs(v1.x-v4.x)<Accuracy) && (std::fabs(v1.y-v4.y)<Accuracy) &&
//					(std::fabs(v1.z-v4.z)<Accuracy)){
//					angulo3= 0;
//				}
//				else{
//					CVector VectorUnitario2D3= GetUnitVector2D(v1, v4);
//					dProductoEscalar = DOT_PRODUCT_2D( VectorUnitario2D1, VectorUnitario2D3 );
//					//if (dProductoEscalar < 1+Accuracy && dProductoEscalar > 1) dProductoEscalar=1;
//					//if (dProductoEscalar < -1 && dProductoEscalar > -1-Accuracy) dProductoEscalar=-1;
//					if(std::abs(dProductoEscalar > 1.0))
//						dProductoEscalar = NORMALIZE(dProductoEscalar);
//					angulo3=acos(dProductoEscalar);
//					if (std::fabs(angulo3)>(NUMBER_PI/2)) angulo3 = std::fabs(angulo3-NUMBER_PI);
//				}
//
//				double angulo4;
//				if ((std::fabs(v1.x-v3.x)<Accuracy) && (std::fabs(v1.y-v3.y)<Accuracy) &&
//					(std::fabs(v1.z-v3.z)<Accuracy)){
//					angulo4= 0;
//				}
//				else{
//					CVector VectorUnitario2D4= GetUnitVector2D(v1, v3);
//					dProductoEscalar = DOT_PRODUCT_2D( VectorUnitario2D1, VectorUnitario2D4 );
//					//if (dProductoEscalar < 1+Accuracy && dProductoEscalar > 1) dProductoEscalar=1;
//					//if (dProductoEscalar < -1 && dProductoEscalar > -1-Accuracy) dProductoEscalar=-1;
//					if(std::abs(dProductoEscalar > 1.0))
//						dProductoEscalar = NORMALIZE(dProductoEscalar);
//					angulo4=acos(dProductoEscalar);
//					if (std::fabs(angulo4)>(NUMBER_PI/2)) angulo4 = std::fabs(angulo4-NUMBER_PI);
//				}
//
//				double angulo5;
//				if ((std::fabs(v2.x-v4.x)<Accuracy) && (std::fabs(v2.y-v4.y)<Accuracy) &&
//					(std::fabs(v2.z-v4.z)<Accuracy)){
//					angulo5= 0;
//				}
//				else{
//					CVector VectorUnitario2D5= GetUnitVector2D(v2, v4);
//					dProductoEscalar = DOT_PRODUCT_2D( VectorUnitario2D1, VectorUnitario2D5 );
//					//if (dProductoEscalar < 1+Accuracy && dProductoEscalar > 1) dProductoEscalar=1;
//					//if (dProductoEscalar < -1 && dProductoEscalar > -1-Accuracy) dProductoEscalar=-1;
//					if(std::abs(dProductoEscalar > 1.0))
//						dProductoEscalar = NORMALIZE(dProductoEscalar);
//					angulo5=acos(dProductoEscalar);
//					if (std::fabs(angulo5)>(NUMBER_PI/2)) angulo5 = std::fabs(angulo5-NUMBER_PI);
//				}
//
//				double angulo6;
//				if ((std::fabs(v2.x-v3.x)<Accuracy) && (std::fabs(v2.y-v3.y)<Accuracy) &&
//					(std::fabs(v2.z-v3.z)<Accuracy)){
//					angulo6= 0;
//				}
//				else{
//					CVector VectorUnitario2D6= GetUnitVector2D(v2, v3);
//					dProductoEscalar = DOT_PRODUCT_2D( VectorUnitario2D1, VectorUnitario2D6 );
//					//if (dProductoEscalar < 1+Accuracy && dProductoEscalar > 1) dProductoEscalar=1;
//					//if (dProductoEscalar < -1 && dProductoEscalar > -1-Accuracy) dProductoEscalar=-1;
//					if(std::abs(dProductoEscalar > 1.0))
//						dProductoEscalar = NORMALIZE(dProductoEscalar);
//					angulo6=acos(dProductoEscalar);
//					if (std::fabs(angulo6)>(NUMBER_PI/2)) angulo6 = std::fabs(angulo6-NUMBER_PI);
//				}
//
//				double angleAux= max(angulo3, angulo4);
//				angleAux= max(angleAux, angulo5);
//				angleAux= max(angleAux, angulo6);
//
//				double weightAux= CalculeMUab( 0, 2. * ErrorCollinearity2D, angleAux);
//
//				weight= min(weight, weightAux);
//				if(weight < Accuracy){
//					MatrixCollinearity[i][j]= 0.;
//					MatrixCollinearity[j][i]= 0.;
//				}
//				else{
//					MatrixCollinearity[i][j]= weight;
//					MatrixCollinearity[j][i]= weight;
//				}
//			}
//
////			if(w >Accuracy){
//////quito la condicion de que pertenezcan a una misma cara
//////				bool CompartenCara=false;
//////				for(k=0; k<GetSize(&pDB->CFaces); k++){
//////					CFace Cara= pDB->GetFace(k);
//////					if((Cara.SearchEdgeInFace(i, posicion1)) 
//////						&& (Cara.SearchEdgeInFace(j, posicion2)))
//////					{
//////						CompartenCara=true;
//////						break;
//////					}
//////				}
//////				if (CompartenCara==true){
////					MatrixCollinearity[i][j]= w; 
////					MatrixCollinearity[j][i]= w;}
//////				else
//////					MatrixCollinearity[j][i]= MatrixCollinearity[i][j]= 0.0;
//////			}
////			else 
////				MatrixCollinearity[j][i]= MatrixCollinearity[i][j]= 0.0;
//
//
//		}
//	}
//}

/////////////////////////////////////////////////////////////////////////////


void CRefine2D::WeightsVerticality(CDB_Model *DDBB,
									std::vector<double> &LineVerticality,
									double ErrorVerticality2D)
//Vertical lines must be parallel to the Y axis

//The compliance function to measure parallelism between
//each edge and the vertical axis is calculated
{
	for (long i=0;i<DDBB->GetSizeEdges();i++) {
		CEdge Edge= DDBB->GetEdge(i);

		////////// Verticality by angle ///////////////////////
		//CVector UnitVector= GetUnitVector2D(DDBB->GetVertex(Edge.Head),
		//									DDBB->GetVertex(Edge.Tail));

		//double Cos2D= std::fabs(UnitVector.y);
		//if (Cos2D < 1+Accuracy && Cos2D > 1) Cos2D= 1;
		//if (Cos2D < -1 && Cos2D > -1-Accuracy) Cos2D= -1;
		//double AngleVertical2D = acos(Cos2D);

		//LineVerticality[i]= CalculeMUab(0, ErrorVerticality2D, AngleVertical2D);

		CVector V (DDBB->GetVertex(Edge.Head), DDBB->GetVertex(Edge.Tail));
		CVector VV= V;
		VV.x= 0.;

		LineVerticality[i]= CalculeMUab(0, ErrorVerticality2D, DDBB->GetAngle2D_PI_half(V, VV));
		////////////////////////////////////////////////////////

		////////// Verticality by x coordinates ///////////////
		//double deltaX= std::fabs(DDBB->GetVertex(Edge.Head).x - DDBB->GetVertex(Edge.Tail).x);
		//LineVerticality[i]= CalculeMUab(0, ErrorVerticality2D, deltaX);
		////////////////////////////////////////////////////////
	}
}


double CRefine2D::GetCostParallelism(CDB_Model *DDBB, std::vector<double> &Solucion2D,
									 std::vector<std::vector<double>> MatrixParallelism)
//Calculates a cost that penalizes those lines 
//that have been classified as almost parallel (in MatrixParallelism), 
//when they are not so in the current drawing
{
	double costParallelism = 0;

	// Check the parallelism of each edge with all the others
	for(long i=0; i<DDBB->GetSizeEdges(); i++ ){ 
		CEdge Arista1= DDBB->GetEdge(i);

		////////// Cost by distances ///////////////////
		//CVertex HeadVertex1;
		//HeadVertex1.x= Solucion2D[2*(Arista1.Head)];
		//HeadVertex1.y= Solucion2D[2*(Arista1.Head)+1];
		//CVertex TailVertex1;
		//TailVertex1.x= Solucion2D[2*(Arista1.Tail)];
		//TailVertex1.y= Solucion2D[2*(Arista1.Tail)+1];
		//////////////////////////////////////////////

		////////// Cost by angles ///////////////////
		CVector V1 (Solucion2D[2*(Arista1.Head)] - Solucion2D[2*(Arista1.Tail)],
					Solucion2D[2*(Arista1.Head)+1] - Solucion2D[2*(Arista1.Tail)+1],
					0.);
		//////////////////////////////////////////////

		//Given the edge 'Arista1', check its parallelism with all the others
		for(long j=i+1; j<DDBB->GetSizeEdges(); j++ ) {
			//Check if they were classified as "nearly" parallel in the 2D drawing
			//assert( !_isnan(MatrixParallelism[i][j]) );
			if (MatrixParallelism[i][j] >Accuracy){
				CEdge Arista2= DDBB->GetEdge(j);

				////////// Cost by distances ///////////////////
				//CVertex HeadVertex2;
				//HeadVertex2.x= Solucion2D[2*(Arista2.Head)];
				//HeadVertex2.y= Solucion2D[2*(Arista2.Head)+1];
				//CVertex TailVertex2;
				//TailVertex2.x= Solucion2D[2*(Arista2.Tail)];
				//TailVertex2.y= Solucion2D[2*(Arista2.Tail)+1];

				//double D1= GetDistance2DVertexToLine(HeadVertex2, HeadVertex1, TailVertex1);
				//double D2= GetDistance2DVertexToLine(TailVertex2, HeadVertex1, TailVertex1);
				//double Distance = std::fabs (D1 - D2);
				//Distance /= DDBB->GetLength2D(j); //Normalize the distance based on the length of the lines

				//costParallelism += std::fabs( MatrixParallelism[i][j] * Distance);
				//////////////////////////////////////////////

				////////// Cost by angles ///////////////////
				CVector V2 (Solucion2D[2*(Arista2.Head)] - Solucion2D[2*(Arista2.Tail)],
							Solucion2D[2*(Arista2.Head)+1] - Solucion2D[2*(Arista2.Tail)+1],
							0.);
				double angle = DDBB->GetAngle2D_PI_half(V1, V2);

				costParallelism += std::fabs( MatrixParallelism[i][j] * angle);
				//////////////////////////////////////////////
			}
		}
	}

	//assert( !_isnan(costParallelism) );
	return costParallelism;
}


double CRefine2D::GetCostCollinearity(CDB_Model *DDBB, std::vector<double> &Solucion2D,
									  std::vector<std::vector<double>> MatrixCollinearity)
//Calculates a cost that penalizes those lines 
//that have been classified as almost collinear, when they are not so in the current drawing
{
	double costCollinearity= 0;

	for (long i=0; i<DDBB->GetSizeEdges(); i++){
		CEdge Arista1= DDBB->GetEdge(i);

		CVertex v1= DDBB->GetVertex(Arista1.Head);
		v1.x= Solucion2D[2*(Arista1.Head)];
		v1.y= Solucion2D[2*(Arista1.Head)+1];

		CVertex v2= DDBB->GetVertex(Arista1.Tail);
		v2.x= Solucion2D[2*(Arista1.Tail)];
		v2.y= Solucion2D[2*(Arista1.Tail)+1];

		for (long j=i+1; j<DDBB->GetSizeEdges(); j++) {
			//assert( !_isnan(MatrixCollinearity[i][j]) );
			if(MatrixCollinearity[i][j] > Accuracy){   //Matrix collinear contains the weigths of the pairs of edges
				CEdge Arista2= DDBB->GetEdge(j);

				CVertex v3= DDBB->GetVertex(Arista2.Head);
				v3.x= Solucion2D[2*(Arista2.Head)];
				v3.y= Solucion2D[2*(Arista2.Head)+1];
				CVertex v4= DDBB->GetVertex(Arista2.Tail);
				v4.x= Solucion2D[2*(Arista2.Tail)];
				v4.y= Solucion2D[2*(Arista2.Tail)+1];

				////////// Cost by distances /////////////////
				//double Distance = std::fabs(GetDistance2DVertexToLine(v3, v1, v2)) + 
				//				  std::fabs(GetDistance2DVertexToLine(v4, v1, v2));
				////Normalize the distance based on the length of the lines
				//Distance /= MODULE_2D(v1, v2);

				//costCollinearity += std::fabs( MatrixCollinearity[i][j] * Distance);
				//////////////////////////////////////////////

				////////// Cost by angles ////////////////////
				//CVector ii(v1, v2);
				//CVector jj(v3, v4);
				//double angle= DDBB->GetAngle2D_PI_half(ii, jj);
				//costCollinearity += std::fabs( MatrixCollinearity[i][j] * (angle));  //Does not prevent overlapping!!

				//Candidate collinear lines must share a common vertex or "pivot"
				double angle= 0.;
				if(Arista1.Head == Arista2.Head){
					angle= DDBB->GetAngle2D_PI(v2, v1, v4);
				}
				else if(Arista1.Head == Arista2.Tail){
					angle= DDBB->GetAngle2D_PI(v2, v1, v3);
				}
				else if(Arista1.Tail == Arista2.Head){
					angle= DDBB->GetAngle2D_PI(v1, v2, v4);
				}
				else if(Arista1.Tail == Arista2.Tail){
					angle= DDBB->GetAngle2D_PI(v1, v2, v3);
				}
				else //Edges do not share a common vertex!
					continue;

				//Collinear lines sharing a common vertex must encompass an angle of PI
				costCollinearity += std::fabs( MatrixCollinearity[i][j] * std::fabs(NUMBER_PI - angle));
				//////////////////////////////////////////////
			}
		}
	}

	//assert( !_isnan(costCollinearity) );
	return costCollinearity;
}


//What follows is the deprecated implementation by Ana Piquer.

//double CRefine2D::GetCostCollinearity(CDB_Model *DDBB, TListaDouble *pSolucion2D,double **MatrizColinealidad) {
//	
//	int es;
//	long i,j, tam=pDB->ListaAristas.lTamanyo;
//	CEdge Arista1, Arista2;
//	double dDatoX, dDatoY;
//	bool CompartenCara=false, bien=false;
//    double angulo1, angulo2, angulo3, angulo4, angulo5, angulo6, dProductoEscalar,
//		dSumatorioColinealidad;
//	CVertex v1,v2,v3,v4, vaux;
//	CVertex VectorUnitario2D1, VectorUnitario2D2,VectorUnitario2D3,VectorUnitario2D4,VectorUnitario2D5,
//		VectorUnitario2D6;
//	dSumatorioColinealidad=0;
//
//	for (i=0;i<tam;i++){
//		ObtenListaAristas( &pDB->ListaAristas, i, &Arista1 );
//		NuevoVertice( &v1);
//		NuevoVertice( &v2 );
//		NuevoVertice(&vaux);
//		ObtenListaVertices(&pDB->ListaVertices, Arista1.Head, &v1);
//		ObtenListaVertices(&pDB->ListaVertices, Arista1.Tail, &v2);
//		ObtenListaDouble( pSolucion2D, 2*(Arista1.Head), &dDatoX );
//		ObtenListaDouble( pSolucion2D, 2*(Arista1.Head)+1, &dDatoY );
//		v1.x= dDatoX;
//		v1.y= dDatoY;
//		ObtenListaDouble( pSolucion2D, 2*(Arista1.Tail), &dDatoX );
//		ObtenListaDouble( pSolucion2D, 2*(Arista1.Tail)+1, &dDatoY );
//		v2.x= dDatoX;
//		v2.y= dDatoY;
//
//		if (v1.x > v2.x){
//			v1.VCopyTo(&vaux);
//			v2.VCopyTo(&v1);
//			vaux.VCopyTo(&v2);
//		}
//	//	DestruyeVertice(&vaux);
//		for (j=i+1;j<tam;j++) {
//			assert( !_isnan(MatrizColinealidad[i][j]) );
//
//			if(MatrizColinealidad[i][j]> Accuracy){
//
//				ObtenListaAristas( &pDB->ListaAristas, j, &Arista2 );
//				NuevoVertice( &v3);
//				NuevoVertice( &v4 );
//				ObtenListaVertices(&pDB->ListaVertices, Arista2.Head, &v3);
//				ObtenListaVertices(&pDB->ListaVertices, Arista2.Tail, &v4);
//				ObtenListaDouble( pSolucion2D, 2*(Arista2.Head), &dDatoX );
//				ObtenListaDouble( pSolucion2D, 2*(Arista2.Head)+1, &dDatoY );
//				v3.x= dDatoX;
//				v3.y= dDatoY;
//				ObtenListaDouble( pSolucion2D, 2*(Arista2.Tail), &dDatoX );
//				ObtenListaDouble( pSolucion2D, 2*(Arista2.Tail)+1, &dDatoY );
//				v4.x= dDatoX;
//				v4.y= dDatoY;
//				
//				if (v3.x > v4.x){
//					NuevoVertice(&vaux);
//					v3.VCopyTo(&vaux);
//					v4.VCopyTo(&v3);
//					vaux.VCopyTo(&v4);
//				}
//				if (v1.x < v3.x) es=1;
//				if (v1.x >v3.x) es=2;
//				if (v1.x == v3.x) es=0;
//				bien=false;
//				switch (es){
//				case 1:
//					if (v2.x <=v4.x)(bien=true);
//					break;
//				case 2:	
//					if (v4.x<=v1.x) bien=true;
//					break;
//				case 0:
//					//este caso es si las lineas son verticales
//					if (v1.y<v2.y){
//						if (v3.y<v4.y){
//							if (v2.y<=v3.y)
//								bien=true;
//						}
//						else{
//							if (v2.y<=v4.y)
//								bien=true;
//						}
//					}
//					else{
//						if (v3.y<v4.y){
//							if (v1.y<=v3.y)
//								bien=true;
//						}
//						else{
//							if (v1.y<=v4.y)
//								bien=true;
//						}
//					}
//				}
//				if (bien){	
//					NuevoVertice (&VectorUnitario2D1);
//					NuevoVertice (&VectorUnitario2D2);
//					VectorUnitario2D1 = GetUnitVector2D( &v1, &v2 );
//					VectorUnitario2D2 = GetUnitVector2D( &v3, &v4 );
//			
//					dProductoEscalar = DOT_PRODUCT_2D( VectorUnitario2D1, VectorUnitario2D2 );
//					assert( !_isnan(dProductoEscalar) );
//
//					if (dProductoEscalar < 1+Accuracy && dProductoEscalar > 1) dProductoEscalar=1;
//					if (dProductoEscalar < -1 && dProductoEscalar > -1-Accuracy) dProductoEscalar=-1;
//					assert( !_isnan(dProductoEscalar) );
//
//					angulo1=acos(dProductoEscalar);
//					assert( !_isnan(angulo1) );	
//					if (std::fabs(angulo1)>(NUMBER_PI/2)) angulo1 = std::fabs(angulo1-NUMBER_PI);
//
//					if ((std::fabs(v1.x-v4.x)<Accuracy) && (std::fabs(v1.y-v4.y)<Accuracy) &&
//						(std::fabs(v1.z-v4.z)<Accuracy))	(angulo3=0);
//					else{
//						NuevoVertice (&VectorUnitario2D3);
//						VectorUnitario2D3 = GetUnitVector2D( &v1, &v4 );
//						dProductoEscalar = DOT_PRODUCT_2D( VectorUnitario2D1, VectorUnitario2D3 );
//						DestruyeVertice (&VectorUnitario2D3);
//						if (dProductoEscalar < 1+Accuracy && dProductoEscalar > 1) dProductoEscalar=1;
//						if (dProductoEscalar < -1 && dProductoEscalar > -1-Accuracy) dProductoEscalar=-1;
//						assert( !_isnan(dProductoEscalar) );
//	
//						angulo3=acos(dProductoEscalar);
//						assert( !_isnan(angulo3) );	
//						if (std::fabs(angulo3)>(NUMBER_PI/2)) angulo3 = std::fabs(angulo3-NUMBER_PI);
//					}
//					if ((std::fabs(v1.x-v3.x)<Accuracy) && (std::fabs(v1.y-v3.y)<Accuracy) &&
//						(std::fabs(v1.z-v3.z)<Accuracy))	(angulo4=0);
//					else{
//						NuevoVertice (&VectorUnitario2D4);
//						VectorUnitario2D4 = GetUnitVector2D( &v1, &v3 );
//						dProductoEscalar = DOT_PRODUCT_2D( VectorUnitario2D1, VectorUnitario2D4 );
//						DestruyeVertice (&VectorUnitario2D4);
//
//						if (dProductoEscalar < 1+Accuracy && dProductoEscalar > 1) dProductoEscalar=1;
//						if (dProductoEscalar < -1 && dProductoEscalar > -1-Accuracy) dProductoEscalar=-1;
//						assert( !_isnan(dProductoEscalar) );
//
//						angulo4=acos(dProductoEscalar);
//						assert( !_isnan(angulo4) );	
//						if (std::fabs(angulo4)>(NUMBER_PI/2)) angulo4 = std::fabs(angulo4-NUMBER_PI);
//					}
//					if ((std::fabs(v2.x-v4.x)<Accuracy) && (std::fabs(v2.y-v4.y)<Accuracy) &&
//						(std::fabs(v2.z-v4.z)<Accuracy))	(angulo5=0);
//					else{
//						NuevoVertice (&VectorUnitario2D5);
//						VectorUnitario2D5 = GetUnitVector2D( &v2, &v4 );
//						dProductoEscalar = DOT_PRODUCT_2D( VectorUnitario2D1, VectorUnitario2D5 );
//						DestruyeVertice (&VectorUnitario2D5);
//
//						if (dProductoEscalar < 1+Accuracy && dProductoEscalar > 1) dProductoEscalar=1;
//						if (dProductoEscalar < -1 && dProductoEscalar > -1-Accuracy) dProductoEscalar=-1;
//						assert( !_isnan(dProductoEscalar) );
//
//						angulo5=acos(dProductoEscalar);
//						assert( !_isnan(angulo5) );	
//						if (std::fabs(angulo5)>(NUMBER_PI/2)) angulo5 = std::fabs(angulo5-NUMBER_PI);
//					}
//					if ((std::fabs(v2.x-v3.x)<Accuracy) && (std::fabs(v2.y-v3.y)<Accuracy) &&
//						(std::fabs(v2.z-v3.z)<Accuracy))	(angulo6=0);
//					else{
//						NuevoVertice (&VectorUnitario2D6);
//						VectorUnitario2D6 = GetUnitVector2D( &v2, &v3 );
//						dProductoEscalar = DOT_PRODUCT_2D( VectorUnitario2D1, VectorUnitario2D6 );
//						DestruyeVertice (&VectorUnitario2D6);
//
//						if (dProductoEscalar < 1+Accuracy && dProductoEscalar > 1) dProductoEscalar=1;
//						if (dProductoEscalar < -1 && dProductoEscalar > -1-Accuracy) dProductoEscalar=-1;
//						assert( !_isnan(dProductoEscalar) );
//
//						angulo6=acos(dProductoEscalar);
//						assert( !_isnan(angulo6) );	
//						if (std::fabs(angulo6)>(NUMBER_PI/2)) angulo6 = std::fabs(angulo6-NUMBER_PI);
//					}
//					DestruyeVertice (&VectorUnitario2D1);
//					DestruyeVertice (&VectorUnitario2D2);
//					angulo2= max(angulo3, angulo4);
//					angulo2= max(angulo2, angulo5);
//					angulo2= max(angulo2, angulo6);
//
//					assert( !_isnan(angulo2) );
//				}
//				dSumatorioColinealidad += std::fabs(( MatrizColinealidad[i][j] * (angulo1+angulo2)*(angulo1+angulo2)));
//				assert( !_isnan(dSumatorioColinealidad) );
//
//				DestruyeVertice(&v3);
//				DestruyeVertice(&v4);
//			}
//		}
//		DestruyeVertice(&v1);
//		DestruyeVertice(&v2);
//		DestruyeVertice(&vaux);
//	}
//	return dSumatorioColinealidad;
//}


double CRefine2D::GetCostVerticality(CDB_Model *DDBB,
									  std::vector<double> &Solucion2D,
									  std::vector<double> LineVerticality)
//Calculates a cost that penalizes those lines 
//that have been classified as almost vertical, when they are not so in the current drawing

//WARNING: most probably, only x coordinates whould be varying while optimizing Verticality!!!
{
	double costVertical=0;

	for (long i=0; i<DDBB->GetSizeEdges(); i++){
		if (LineVerticality[i] >Accuracy){
			CEdge Arista= DDBB->GetEdge(i);

			////////// Verticality by angle ///////////////////////
			//CVertex HeadVertex;
			//HeadVertex.x= Solucion2D[2*(Arista.Head)];
			//HeadVertex.y= Solucion2D[2*(Arista.Head)+1];
			//CVertex TailVertex;
			//TailVertex.x= Solucion2D[2*(Arista.Tail)];
			//TailVertex.y= Solucion2D[2*(Arista.Tail)+1];

			//CVector UnitVector= GetUnitVector2D(HeadVertex, TailVertex);
			//double Cos2D= std::fabs(UnitVector.y);
			//if (Cos2D < 1+Accuracy && Cos2D > 1) Cos2D= 1;
			//if (Cos2D < -1 && Cos2D > -1-Accuracy) Cos2D= -1;
			//double AngleVertical2D = acos(Cos2D);

			//costVertical+= LineVerticality[i] * AngleVertical2D*AngleVertical2D;

			CVector V (Solucion2D[2*(Arista.Head)] - Solucion2D[2*(Arista.Tail)],
					   Solucion2D[2*(Arista.Head)+1] - Solucion2D[2*(Arista.Tail)+1],
					   0.);
			CVector VV= V;
			VV.x= 0.;

			costVertical+= LineVerticality[i] * DDBB->GetAngle2D_PI_half(V, VV);
			////////////////////////////////////////////////////////

			////////// Verticality by x coordinates ///////////////
			//costVertical+= std::fabs(Solucion2D[2*(Arista.Head)] - Solucion2D[2*(Arista.Tail)]);
			////////////////////////////////////////////////////////
		}
	}
	return costVertical;
}


void CRefine2D::GetIncrements(CDB_Model *DDBB,
							  std::vector<double> &Increments,
							  double InitialStep,
							  double FinalStep,
							  double StepReduction)
// Calculates an estimation of the best sequence of increment (or decrement)
// of variables for the Hill-Climbing algorithm
{
	double dXmin, dYmin, dZmin, dXmax, dYmax, dZmax;
	DDBB->GetBoundingBox(dXmin, dYmin, dZmin, dXmax, dYmax, dZmax);
	double dRange = min(std::fabs(dXmax-dXmin), std::fabs(dYmax-dYmin));

	double CurrentIncrement = dRange * InitialStep;
	double MinimIncrement = dRange * FinalStep;

	Increments.clear();
	while (CurrentIncrement > MinimIncrement){
		Increments.push_back(CurrentIncrement);
		InitialStep *= StepReduction;
		CurrentIncrement = dRange * InitialStep;
	};
}


double CRefine2D::GetCost(CDB_Model *DDBB,
						  std::vector<double> &Solution,
						  std::vector<double> &EvolutionCost,
						  std::vector<double> &EvolutionXY,
						  bool evolution,
						  std::vector<std::vector<double>> MatrixParallelism,
						  std::vector<double> LineVerticality,
						  std::vector<std::vector<double>> MatrixCollinearity)
// Esta es la "Funcion de Coste" del Algoritmo de Optimizacion.
// Devuelve el coste de la 'Solucion' teniendo en cuenta la Base de Datos 'DDBB',
// las regularidades activadas y sus coeficientes
{
	double aCostesRegularidades[REGULARITY_LAST+1];

	if (ReferApp.R2D.Regularities2D.ListOfRegularities[REGULARITY_LINE_PARALLELISM].bActive)
		aCostesRegularidades[REGULARITY_LINE_PARALLELISM]=
				GetCostParallelism(DDBB, Solution, MatrixParallelism);
	else
		aCostesRegularidades[REGULARITY_LINE_PARALLELISM]=0;

	if (ReferApp.R2D.Regularities2D.ListOfRegularities[REGULARITY_LINE_COLLINEARITY].bActive)
		aCostesRegularidades[REGULARITY_LINE_COLLINEARITY]=
				GetCostCollinearity(DDBB, Solution, MatrixCollinearity);
	else
		aCostesRegularidades[REGULARITY_LINE_COLLINEARITY]=0;

	if (ReferApp.R2D.Regularities2D.ListOfRegularities[REGULARITY_LINE_VERTICALITY].bActive)
		aCostesRegularidades[REGULARITY_LINE_VERTICALITY]=
				GetCostVerticality(DDBB, Solution, LineVerticality);
	else
		aCostesRegularidades[REGULARITY_LINE_VERTICALITY]=0;

	// EL COSTE TOTAL ES LA SUMA DE LOS COSTES DE TODAS LAS REGULARIDADES/
	double dCosteTotal = 0;
	dCosteTotal += aCostesRegularidades[REGULARITY_LINE_PARALLELISM];
	dCosteTotal += aCostesRegularidades[REGULARITY_LINE_VERTICALITY];
	dCosteTotal += aCostesRegularidades[REGULARITY_LINE_COLLINEARITY];
	assert( !_isnan(dCosteTotal) );   // Asegurate que es un numero finito representable

	if(evolution){
		// GUARDAR LA EVOLUCION DEL COSTE, Y DE LAS VARIABLES
		EvolutionCost.push_back(dCosteTotal);

		// Tambien hay que guardar cada coste de cada regularidad, en caso que este seleccionada
		if (ReferApp.In.bViewPartialCostsEvolution){
			for(long i=0; i < REGULARITY_LAST+1; i++ ){
				//TRegularity Regularity = ReferApp.R2D.Regularities2D.ListOfRegularities[ i ];
				//if ((i==REGULARITY_LINE_PARALLELISM ||
				//	 i==REGULARITY_LINE_COLLINEARITY) ||
				//	 i==REGULARITY_LINE_VERTICALITY &&
				//	Regularity.bActive)
				//	EvolutionCost.push_back(aCostesRegularidades[ i ]);

				if ( ReferApp.R2D.Regularities2D.ListOfRegularities[ i ].bActive)
					EvolutionCost.push_back(aCostesRegularidades[i]);




				//else
				//	EvolutionCost.push_back(0);
			}
		}

		// SAVE ALL THE VARIABLES (the X's and Y's)
		for(long i=0; i<ReferApp.In.NumVariables; i++) {
			EvolutionXY.push_back(Solution[i]);
		};
	}

	return dCosteTotal;
};


void CRefine2D::GetNewSolutionHillClimbing(std::vector<double> &Solution,
										   std::vector<double> &NewSolution,
										   CDataOptHC* pParams)
// Selects which variable to modify, and how long modify it.
// Stores the result in NewSolution

//The strategy to select a new candidate solution is as follows:
//	- Each variable is visited two times
//	- Even visits will result in positive increments, and 
//	- odd visits will result in negative increments

//This function is the same as GetNewSolutionHC, 
//they have been duplicated to better encapsulate 2D refinement and 3D optimization
{
	// Copy previous solution in current solution
	NewSolution= Solution;

	// Select WHICH variable is going to be modified
	div_t ResultadoDivision = div(pParams->iSolutionCurrent, 2);
	int iVariableModified = ResultadoDivision.quot;
	int signModification= ResultadoDivision.rem;
//////Alternative random selection of the variable to modify//////
	//int iVariableModified = rand() % ReferApp.In.NumVariables;
/////////////////////////////////////////////////////////////////

	// Select the length of the step
	double dIncrement= pParams->IncrementsOfVariables[pParams->iIncrementVariableCurrent];

	// Select the orientation of the step
	// (Odd solutions are added, while even solutions are substacted)
	double step= NewSolution[iVariableModified];
	if ( signModification == 0 )
	if ( rand() % 2 == 0 )    //random number may be 0 or 1
		step += dIncrement;
	else
		step -= dIncrement;

////////Alternative random selection of the sign//////////////////
	//if ( rand() % 2 == 0 )    //random number may be 0 or 1
	//	step += dIncrement;
	//else
	//	step -= dIncrement;
/////////////////////////////////////////////////////////////////

	//assert(step < 1000000);

	NewSolution[iVariableModified]= step;
};


void CRefine2D::OptimizeDrawing2D(CDB_Model *DDBB)
// Optimization-based offline refinement of the Drawing 2D
// This approach modifies X and Y coordinates of vertices
// to improve one or more of the following regularities:
	//	edge parallelism
	//	edge Verticality
	//	edge collinearity

// The result is a "tidied-up" 2D drawing

// Optimization is done by "Hill-Climbing" algorithm.
//The code is duplicated from OptimizationHC,
//to better encapsulate 2D refinement and 3D optimization
{
	//Select regularities
	for (long i=0; i<REGULARITY_LAST +1; i++){
		ReferApp.In.Regularities.ListOfRegularities[i].bActive =
			ReferApp.R2D.Regularities2D.ListOfRegularities[i].bActive;
	}

	if (ReferApp.R2D.Regularities2D.NumActiveRegularities == 0)
		return;

	ReferApp.In.Regularities.NumActiveRegularities =
				ReferApp.R2D.Regularities2D.NumActiveRegularities;

	//Copy input database to restore it if refinement fails
	CDB_Model DB_Aux= *DDBB;

	//Get the weights of the regularities 2D
	std::vector<std::vector<double>> MatrixParallelism(DDBB->GetSizeEdges(),
													   std::vector<double>(DDBB->GetSizeEdges()));
	std::vector<double> LineVerticality(DDBB->GetSizeEdges(), 0);
	std::vector<std::vector<double>> MatrixCollinearity(DDBB->GetSizeEdges(),
														std::vector<double>(DDBB->GetSizeEdges()));

	WeightsParallelism(DDBB, MatrixParallelism, ReferApp.R2D.ErrorParallelism2D);
	WeightsVerticality(DDBB, LineVerticality, ReferApp.R2D.ErrorVerticality2D);
	WeightsCollinearity(DDBB, MatrixCollinearity, ReferApp.R2D.ErrorCollinearity2D);

	//Adjust the step sizes for the Hill-Climbing algorithm
	GetIncrements(DDBB,
				  ReferApp.R2D.DataOptHC_2D.IncrementsOfVariables,
				  ReferApp.R2D.DataOptHC_2D.dInitialStep,
				  ReferApp.R2D.DataOptHC_2D.dFinalStep,
				  ReferApp.R2D.DataOptHC_2D.dStepReduction);

	//Opens the Dialog that shows the progress, and allows cancellation
	CDialogProgress Progress;
	Progress.Create( IDD_DIALOG_Progress );
	CTime HoraInicial = CTime::GetCurrentTime();
	Progress.Message("Applying 2D refine Offline \"Hill-Climbing\"...");

	//Update progress window
	long lRangeProgress = ReferApp.R2D.DataOptHC_2D.iMaxIterations *
						  (long)ReferApp.R2D.DataOptHC_2D.IncrementsOfVariables.size();

	Progress.RangeProgress(0, lRangeProgress);
	Progress.UpdateProgress(0);

	//INICIALIZATION OF OPTIMIZATION VARIABLES
	long numVertices= DDBB->GetSizeVertices();
	std::vector<double> VariablesXY;
	for (long i=0; i<numVertices; i++){
		CVertex Vertice= DDBB->GetVertex(i);
		VariablesXY.push_back(Vertice.x);
		VariablesXY.push_back(Vertice.y);
	}
	ReferApp.In.NumVariables= 2*numVertices;

	long iMaxIterations = ReferApp.R2D.DataOptHC_2D.iMaxIterations;
	double dThresholdRejectBestCost = 1 - ReferApp.R2D.DataOptHC_2D.dThresholdRejectBestCost;
	double dThresholdRejectBestCostInLevel = 1 - ReferApp.R2D.DataOptHC_2D.dThresholdRejectBestCostInLevel;

	//Reset solution
	std::vector<double> NewSolution;
	std::vector<double> BestSolution= VariablesXY;

	//Reset counters for every step
	long iContadorIncrementosVariables = 0;
	long lContadorIteraciones_ParaCoeficientesRegularidades = 0;

	//Define local memory to temporarily save the evolution of the optimization
	std::vector<double> EvolutionCost;
	std::vector<double> EvolutionXY;

//////Alternative random selection of the variable to modify//////
	//srand( (unsigned)time( NULL ) );  // This reset should be done before iterations begin
/////////////////////////////////////////////////////////////////

	//Optimization loop
	//-----------------
	bool OptimizationDone= true;
	do {
		ReferApp.R2D.DataOptHC_2D.iIncrementVariableCurrent = iContadorIncrementosVariables;

		//REPEAT UNTIL NO IMPROVEMENT IS ACHIEVED, OR MAX ITERATIONS HAPPENS
		long iteration= 0;
		bool BetterSolution;
		do {
			//Save current step
			EvolutionCost.push_back(ReferApp.R2D.DataOptHC_2D.IncrementsOfVariables[ReferApp.R2D.DataOptHC_2D.iIncrementVariableCurrent]);

			//Look for a solution that improves the current one
			double BestCost = GetCost(DDBB, BestSolution, 
									  EvolutionCost,
									  EvolutionXY,
									  true,
									  MatrixParallelism,
									  LineVerticality,
									  MatrixCollinearity);
			long iBestSolution = -1;

			//REPEAT two times FOR EACH VARIABLE
			//(Thus, increments will be positive in even iterations and negative in odd ones)
			for (long i = 0; i < 2 * ReferApp.In.NumVariables; i++ ) {
				ReferApp.R2D.DataOptHC_2D.iSolutionCurrent = i;

				//Given i-th solution, i+1 solution is calculated
				GetNewSolutionHillClimbing( BestSolution, NewSolution, &ReferApp.R2D.DataOptHC_2D);
				double CurrentCost = GetCost(DDBB, NewSolution, 
											 EvolutionCost,
											 EvolutionXY,
											 false,
											 MatrixParallelism,
											 LineVerticality,
											 MatrixCollinearity);

				//Checks if this is the last iteration to apply the appropriate acceptance coefficient
				if (iContadorIncrementosVariables == (long)ReferApp.R2D.DataOptHC_2D.IncrementsOfVariables.size()-1){
					//Check if it is the best solution so far 
					//with the particular acceptance coefficient for the last increment of variables
					if ( CurrentCost < BestCost * dThresholdRejectBestCost ) {
						BestCost = CurrentCost;
						iBestSolution = i;
					}
				}
				else{
					//Check if it is the best solution so far 
					//with the common acceptance coefficient for all variable increments
					if ( CurrentCost < BestCost * dThresholdRejectBestCostInLevel ) {
						BestCost = CurrentCost;
						iBestSolution = i;
					}
				}
			}

			//Save new solution, if it is better than previous ones
			if (iBestSolution != -1) {
				ReferApp.R2D.DataOptHC_2D.iSolutionCurrent = iBestSolution;
				GetNewSolutionHillClimbing( BestSolution, NewSolution, &ReferApp.R2D.DataOptHC_2D);
				BestSolution= NewSolution;
				BetterSolution = true;
			}
			else
				BetterSolution = false;
		
			OptimizationDone= Progress.UpdateProgress( lContadorIteraciones_ParaCoeficientesRegularidades );

			//Save the drawing of the object according to
			//the lowest cost solution achieved up to this moment
			DDBB->SetAllVertex_XY(BestSolution);

			CReferDoc::GetDoc()->RefreshWindowSketch2D();

			lContadorIteraciones_ParaCoeficientesRegularidades++;
			iteration++;

		} while (iteration < iMaxIterations &&
		         BetterSolution && OptimizationDone);

		iContadorIncrementosVariables++;
	} while ((iContadorIncrementosVariables < (long)ReferApp.R2D.DataOptHC_2D.IncrementsOfVariables.size()) &&
	         OptimizationDone);

	//Save best solution if optimization was succesful
	if (OptimizationDone){
		VariablesXY= BestSolution;
	};

	CTime HoraFinal = CTime::GetCurrentTime();
	CTimeSpan Tiempo = HoraFinal - HoraInicial;
	ReferApp.m_sTimeCalculating.Format("%i' %i\"", Tiempo.GetMinutes(), Tiempo.GetSeconds() );

	if (OptimizationDone){
		//Save the evolution of the cost
		ReferApp.In.EvolutionCosts= EvolutionCost;
		ReferApp.In.EvolutionZ= EvolutionXY;

		//Update DataBase with the X,Y coordinates obtained from optimization algorithm
		DDBB->SetAllVertex_XY(VariablesXY);
	}
	else{
		//Restore previous DataBase, because optimization has failed
		*DDBB= DB_Aux;
	}

	Progress.DestroyWindow();
};


void CRefine2D::GetRefineDrawing(CDB_Model *DDBB, bool off_line)
//Manager to call the procedures to refine, or "tidy up", the 2D wireframe drawing

//The procedure is called for both online and offline refinements,
//but different refinements apply in each case

//The following refinements may apply:
//		1. Remove very short edges
//		2. Merge junctions by clustering close tips
//		3. Split T edges
//		4. Merge collinear edges
//		5. Split non-2-manifold vertices
//		6. Optimized displacement of X and Y coordinates of vertices
//		   to improve up to three regulatiries
//		   that the drawing is presumed to adhere to:
//				- parallelism
//				- verticality
//				- collinearity

//Refinement #6 only applies for offline refinement

//Based on:
	//Piquer A.
	//Percepción artificial de dibujo lineales.
	//PhD Thesis, 2003
	//Departamento de Tecnología.
	//Universitat Jaume I de Castellon

	//Company P., Contero M., Conesa J. and Piquer A.
	//An optimisation-based reconstruction engine for 3D modelling by sketching.
	//Computers & Graphics (ISSN 0097-8493). Vol. 28, No 6. December 2004. pp. 955-979

	//Company P., Plumed R., Varley P.A.C., Camba J.D. (2019) 
	//Algorithmic Perception of Vertices in Sketched Drawings of Polyhedral Shapes. 
	//ACM Transactions on Applied Perception, Vol. 16, Issue 3. Article No. 18. pp. 1-19. 
{
	if (DDBB->GetSizeEdges()+DDBB->GetSizeArcs() == 0){
		AfxMessageBox("No vectorized 2D model to refine");
		return;
	}

	//COMMON (ONLINE AND OFFLINE) REFINEMENT

	//Remove very short edges
	//-----------------------
	if (ReferApp.R2D.IgnoreShortEdges){
		RemoveShortEdges(DDBB);
	}

	//Merge close tips to get junctions
	//---------------------------------
	if (ReferApp.R2D.bMergeVertices){
		Junctions2D J2D(ReferApp.R2D.MergeCubicCorners,
						ReferApp.R2D.DistanceMergeCubicCorners,
						ReferApp.R2D.MergeAlignedCubicCorners,
						ReferApp.R2D.MergeRotationCubicCorners,
						ReferApp.R2D.MergeCarefulLineTips,
						ReferApp.R2D.MergeMaxDistVertices,
						ReferApp.R2D.MergeMaxVerticesRotation,
						ReferApp.R2D.ReificationMerit,
						ReferApp.R2D.MergeCasualLineTips,
						ReferApp.R2D.ReMergeCasualInTol,
						ReferApp.R2D.ReMergeCasualOutTol,
						ReferApp.R2D.ReMergeVerticesTriplets,
						ReferApp.R2D.ReMergeMaxDistTriplets,
						ReferApp.R2D.EmergentTripletsMerit,
						ReferApp.R2D.ReMergeVerticesDangling,
						ReferApp.R2D.ReMergeMaxDistDangling,
						ReferApp.R2D.EmergenceDanglingDeMerit);

		long NumRemovedTips= J2D.GetJunctions(DDBB);

		//Display message
		//if(NumRemovedTips > 0){
		//	CString sMensaje, sMensajeAux;
		//	sMensaje= "There have been merged ";
		//	sMensajeAux.Format("%i", NumRemovedTips);
		//	sMensaje+=sMensajeAux;
		//	sMensaje+=" tips";
		//	AfxMessageBox (sMensaje);
		//	CReferDoc::GetDoc()->RefreshWindows();
		//}
	}

	//Merge and/or split T vertices
	//-----------------------------
	if (ReferApp.R2D.Merge_T_Vertices || ReferApp.R2D.Split_T_Vertices){
		Split_T_Vertices(DDBB);
	}

	//Merge collinear edges
	//---------------------
	if (ReferApp.R2D.MergeCollinearEdges){
		MergeCollinearEdges(DDBB);
	}

	//Split non-2-manifold vertices
	//-----------------------------
	if (ReferApp.R2D.SplitNon2ManifoldVertices){
		SplitNonTwoManifoldVertices(DDBB);
	}

	//Split non-2-manifold edges
	//--------------------------
	if (ReferApp.R2D.SplitNon2ManifoldEdges){
		SplitNonTwoManifoldEdges(DDBB);
	}

	//OFFLINE REFINEMENT

	//Optimization-based tidy up of the 2D drawing
	//--------------------------------------------
	if (off_line && ReferApp.R2D.bOptimize2D){
		OptimizeDrawing2D(DDBB);
	}

	//Force screen update
	CReferDoc::GetDoc()->RefreshWindows();
}
