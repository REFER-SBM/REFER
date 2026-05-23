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
#include <vector>

#include "Tools_Vector.h"
#include "Tools_Geometry.h"

#include "DB_Model.h"
#include "Refer.h"

#include "CuePerimeter.h"


bool CCuePerimeter::InputSubGraph(CDB_Model *DDBB, long NumGraph,
								std::vector <long> &ReverseMapVertices,
								std::vector <long> &ReverseMapEdges)
//The input required by the algorithms is:
//		VertexCount= number of vertex in the line drawing
//		Vertex[i].x= X coordinate of the i-th vertex
//		Vertex[i].y= Y coordinate of the i-th vertex

//		EdgeCount= number of edges in the line drawing
//		EdgeHead[i]= Head vertex defining the i-th edge
//		EdgeTail[i]= Tail vertex defining the i-th edge
{
	//Copy external vertices into local database
	Vertex.clear();
	VertexCount= 0;
	std::vector <long> MapVertices;
	for (long i=0; i<(long)DDBB->GetSizeVertices(); i++){
		if(NumGraph==0 || ReferApp.Cu.SG.SubgraphForVertex[i] == NumGraph-1){
			CVertex v= DDBB->GetVertex(i);
			POINT2D p(v.x, v.y);
			Vertex.push_back(p);
			MapVertices.push_back(VertexCount);
			ReverseMapVertices.push_back(i);
			VertexCount++;
		}
		else
			MapVertices.push_back(-1);
	}

	//Copy external edges into local database
	EdgeHead.clear();
	EdgeTail.clear();
	EdgeCount= 0;
	for (long i=0; i<(long)DDBB->GetSizeEdges(); i++)
	if(NumGraph==0 || ReferApp.Cu.SG.SubgraphForEdge[i] == NumGraph-1){
		CEdge e= DDBB->GetEdge(i);
		EdgeHead.push_back(MapVertices[e.Head]);
		EdgeTail.push_back(MapVertices[e.Tail]);
		ReverseMapEdges.push_back(i);
		EdgeCount++;
	}

	return true;
}


void CCuePerimeter::GetImplicitData()
//Obtains data derived from the input data and required to detect perimeters
{
	//Update the number of edges connected to every vertex
	EdgesInVertex.assign(VertexCount, std::vector <long>());
	for (long i=0; i<EdgeCount; ++i){
		EdgesInVertex[EdgeHead[i]].push_back(i);
		EdgesInVertex[EdgeTail[i]].push_back(i);
	}
}


long CCuePerimeter::GetFirstVertex()
//Returns the first vertex
//The upper vertex is selected as first vertex
{
	double CentroidY= 0;
	for(long i=0; i<VertexCount; i++){
		CentroidY += Vertex[i].y;
	}
	CentroidY/= VertexCount;

	double ymax= CentroidY;
	long NumVertex= 0;
	for(long i=0; i<VertexCount; i++){
		if(Vertex[i].y > ymax){
			ymax= Vertex[i].y;
			NumVertex= i;};
	}

	if(ymax > CentroidY)
		return NumVertex;
	else
		return -1;  //The drawing depicts a vertically "flat" graph
}


bool CCuePerimeter::isDangling(long numEdge, long closeEnd, long &numVisited)
//Returns true if 
//    -the edge numEdge is dangling, or
//    -the vertex farEnd of the edge numEdge is connected to a dangling edge
{
	long farEnd;
	if(EdgeHead[numEdge] != closeEnd)
		farEnd= EdgeHead[numEdge];
	else
		farEnd= EdgeTail[numEdge];

	if(EdgesInVertex[farEnd].size() == 1)
		return true;

	else if(EdgesInVertex[farEnd].size() == 2){
		numVisited++;
		if(numVisited > EdgeCount)
			return false;

		long nextEdge;
		if(EdgesInVertex[farEnd][0] != numEdge)
			nextEdge= EdgesInVertex[farEnd][0];
		else
			nextEdge= EdgesInVertex[farEnd][1];

		return isDangling(nextEdge, farEnd, numVisited);
	}

	else
		return false;
}


long CCuePerimeter::GetDanglingEdges(long numVertex)
//Returns the number of chains of dangling edges connected to NumVertex
{
	long numDangling= 0;
	long numVisited= 0;

	for(long i=0; i<(long)EdgesInVertex[numVertex].size(); i++){
		if(isDangling(EdgesInVertex[numVertex][i], numVertex, numVisited))
			numDangling++;
	}

	return numDangling;
}


bool CCuePerimeter::isTjunction(long numVertex, long numEdge, long otherEdge)
//Returns true if numVertex is a T-junction
//If so, the other edge of the T-junction is also retured as otherEdge

//numVertex is a T-junction if it is contained (or near enough)
//to any other edge of the graph different from numEdge
{
	return false;
}


double CCuePerimeter::GetInnerAngle(POINT2D P1, POINT2D P2, POINT2D P3)
//Gets the angle (in radians) between the two segments
//defined by P1-P2 and P2-P3

//Returns a value in the range [0, 2*PI]

//The function DOES NOT determine the usual minimum angle between two segments
//Instead, it determines the INTERNAL angle
//Where INTERNAL is the angle required to rotate counterclockwise
//from segment P1-P2 up to P2-P3, while using P2 as pivot point

//The origin is firstly translated up to point P2, then
//the angle is measured from "OUTSIDE", as the difference between two angles:
//    -The angle required to rotate x axis counterclockwise up to P1 orientation
//    -The angle required to rotate x axis counterclockwise up to P3 orientation

//For those triplets where x axis is inside, -x is used
//It never minds if -x is also inside, since in doing the transformation
//angle1 is measured in the range (-PI, PI)
//while angle3 is measured in the range (0, 3PI)
{
	P1-= P2;
	P3-= P2;
	double angle1= atan2(P1.y, P1.x);
	if(angle1<0) angle1+= 2*NUMBER_PI;
	double angle3= atan2(P3.y, P3.x);
	if(angle3<=0) angle3+= 2*NUMBER_PI;

	if(angle1 > angle3){
		angle1-= NUMBER_PI;
		angle3+= NUMBER_PI;
	}

	return angle3 - angle1;
}


long CCuePerimeter::GetNextEdge(POINT2D PreviousVertex, long CurrentVertex, long CurrentEdge, bool &dangling)
//Returns the number of the edge that follows the current edge in the envelope

//Current edge is defined by Previous and Current vertices

//All the edges connected to Current vertex are compared
//The one making the maximum INTERNAL angle with current edge is selected
//(By using maximum INTERNAL angle, the perimeter is followed clockwise)
{
	long NextEdge= EdgeCount+1;

	//Check for dangling edges
	if(EdgesInVertex[CurrentVertex].size() == 1){
		if(isTjunction(CurrentVertex, CurrentEdge, NextEdge)){
			return -NextEdge;
		}
		else{
			//Dangling edge
			if(!IncludeDanglingEdges)
				dangling= true;
			return EdgesInVertex[CurrentVertex][0];
		}
	}

	//Loop over all the edges connected to Current vertex
	double BestAngle;

	for (long j=0; j<(long)EdgesInVertex[CurrentVertex].size(); j++){
		long NumEdgeJ= EdgesInVertex[CurrentVertex][j];
		if(NumEdgeJ != CurrentEdge){
			long CandidateVertex= EdgeHead[NumEdgeJ];
			if (CandidateVertex == CurrentVertex) CandidateVertex= EdgeTail[NumEdgeJ];

			double angle= GetInnerAngle(PreviousVertex, Vertex[CurrentVertex], Vertex[CandidateVertex]);

			//Save if maximum
			//WARNING: switch from maximum to minimum to get the perimeter counterclockwise
			//if(j==0 || angle < BestAngle){
			if(j==0 || angle > BestAngle){
				BestAngle= angle;
				NextEdge= NumEdgeJ;
			}
		}
	}

	return NextEdge;
}


std::vector <bool> CCuePerimeter::GetEdgesToCheck(long CurrentEdge)
//Defines the list of edges to check, by excluding the current edge
//and edges connected to the current edge
{
	std::vector <bool> CheckEdge(EdgeCount, true);
	for(long i= 0; i<(long)EdgesInVertex[EdgeHead[CurrentEdge]].size(); i++){
		CheckEdge[EdgesInVertex[EdgeHead[CurrentEdge]][i]]= false;
	}
	for(long i= 0; i<(long)EdgesInVertex[EdgeTail[CurrentEdge]].size(); i++){
		CheckEdge[EdgesInVertex[EdgeTail[CurrentEdge]][i]]= false;
	}
	CheckEdge[CurrentEdge]= false;

	return CheckEdge;
}


bool CCuePerimeter::GetCloserIntersection(POINT2D a1, POINT2D b1, std::vector <bool> CheckEdge,
										long &InterceptingEdge, POINT2D &Intercept, long &T_vertex)
//Finds the edge whose intersection with the segment a1-b1
//is closer to the endpoint a1

//Only checks edges marked in CheckEdge

//Also returns the (x,y) coordinates of the intersection point

//Finally, if the intersection is close to one vertex of the outgoing edge,
//this vertex is returned as T-vertex
{
	bool intersection= false;
	double tmin= 1;
	for(long i= 0; i<EdgeCount; i++) if(CheckEdge[i]){
		POINT2D a2= Vertex[EdgeHead[i]];
		POINT2D b2= Vertex[EdgeTail[i]];

		//Get intersection
		double s1_x = b1.x - a1.x;
		double s1_y = b1.y - a1.y;
		double s2_x = b2.x - a2.x;
		double s2_y = b2.y - a2.y;
		double denom= (-s2_x * s1_y + s1_x * s2_y);

		if(denom != 0){
			double s = (-s1_y * (a1.x - a2.x) + s1_x * (a1.y - a2.y)) / denom;
			double t = ( s2_x * (a1.y - a2.y) - s2_y * (a1.x - a2.x)) / denom;

			if ((s >= 0-ThresholdIntersection && s <= 1+ThresholdIntersection) &&
				(t >= 0-ThresholdIntersection && t <= 1+ThresholdIntersection)){
				//Intersection point is internal to both edges
				if(!intersection || t<tmin){
					intersection= true;
					tmin= t;
					InterceptingEdge= i;
					Intercept.x= a1.x + (t * s1_x);
					Intercept.y= a1.y + (t * s1_y);
				}

				if(std::fabs(s)<ThresholdIntersection)
					T_vertex= EdgeHead[i];
				else if (std::fabs(s-1)<ThresholdIntersection)
					T_vertex= EdgeTail[i];
				else
					T_vertex= -1;
			}
			//else{
			//	//External intersection
			//	intersection= false;
			//}
		}
		//else{
			//Edges are parallel
			//intersection= false;
		//}
	}

	return intersection;
}	


void CCuePerimeter::PerimeterByEnvelope(std::vector <long> &ReverseMapVertices,
										std::vector <long> &ReverseMapEdges,
										std::vector <long> &PerimeterVertices,
										std::vector <long> &PerimeterEdges,
										std::vector <POINT2D> &Tjunction,
										std::vector <std::vector <long>> &Tedges)
// Determine the set of vertices and edges that belong to the perimeter
// of a subgraph made out of vertices and edges

// This algorithm is based on an idea first published by Roberts in 1963:
		// Roberts L.G. Machine Perception of Three-Dimensional Solids.
		// PhD Thesis. MIT, Certified by Peter Elias (Thesis Supervisor). 1963.

//The algorithm has been extended to cope with wireframes and their intersected edges
//As a result, it also copes with line-drawings with multiple sub-graphs

//Vertices and edges in Boundary Loop are oriented clockwise
//To get them oriented counterclockwise "GetNextEdge" function must be modified
{
	//Get first vertex
	long FirstVertex= GetFirstVertex();
	if(FirstVertex == -1){ /*AfxMessageBox("Unable to find initial vertex for perimeter"); */ return; }
	PerimeterVertices.push_back(ReverseMapVertices[FirstVertex]);

	//Calculate the number of dangling edges connected to the first vertex
	long danglindInFirstVertex= GetDanglingEdges(FirstVertex);

	//Define a ficticious "previous" corner
	//located farther from the centroid than the first vertex
	POINT2D PreviousCorner= Vertex[FirstVertex];
	PreviousCorner.y += 1;

	//Loop to detect the envelope of the 2D line-drawing
	//--------------------------------------------------
	long maxT= EdgeCount*(EdgeCount-1)/2;  //Upper limit for intersection points
	long maxEdgesInLoops= 2*EdgeCount + 2*maxT;    //Every edge may be visited up to two times
												   //if it is a dangling edge
												   //Every T vertex splits two edges in two halves

	long numLassos= 0;  //Circuit may be multi-lasso, due to dangling edges

	long counterSides= 0;
	long CurrentVertex= FirstVertex;
	long CurrentEdge= EdgeCount+1;
	long NumTjunction= 0;
	long InterceptingEdge= -1;
	POINT2D Intercept;
	long NextCorner= FirstVertex;
	bool removeDangling= false;
	long T_vertex= -1;

	do{
		if(NextCorner >= 0){
			//Next edge
			CurrentEdge= GetNextEdge(PreviousCorner, CurrentVertex, CurrentEdge, removeDangling);
			if(CurrentEdge > EdgeCount){ /*AfxMessageBox("Perimeter cannot close");*/ break; }

			if(removeDangling){
				long last= (long)PerimeterEdges.size()-1;
				if(last>=0 && PerimeterEdges[last] == ReverseMapEdges[CurrentEdge]){
					PerimeterEdges.erase(PerimeterEdges.begin()+last);
					PerimeterVertices.erase(PerimeterVertices.begin()+PerimeterVertices.size()-1);
					counterSides --;
				}
				else
					removeDangling= false;
			}

			if(!removeDangling){
				PerimeterEdges.push_back(ReverseMapEdges[CurrentEdge]);
				counterSides ++;
			}

			PreviousCorner= Vertex[CurrentVertex];

			//Next corner
			long farVertex= EdgeTail[CurrentEdge];
			POINT2D closeEnd= Vertex[EdgeHead[CurrentEdge]];
			POINT2D farEnd= Vertex[EdgeTail[CurrentEdge]];
			if(CurrentVertex != EdgeHead[CurrentEdge]){
				std::swap(closeEnd, farEnd);
				farVertex = EdgeHead[CurrentEdge];
			}

			std::vector <bool> CheckEdge= GetEdgesToCheck(CurrentEdge);

			if(GetCloserIntersection(closeEnd, farEnd, CheckEdge,
									 InterceptingEdge, Intercept, T_vertex))
				NextCorner= -1;  //The next corner is a T vertex
			else
				NextCorner= farVertex;  //The perimeter continues up to the far end of the edge
		}

		else{
			//Next edge
			long PreviousEdge= CurrentEdge;
			CurrentEdge= InterceptingEdge;
			if(CurrentEdge == -1){ /*AfxMessageBox("Perimeter cannot close");*/ break; }

			PerimeterEdges.push_back(ReverseMapEdges[CurrentEdge]);
			counterSides ++;

			//Next corner
			long farVertex;
			if(T_vertex >= 0){
				//Outgoing line is the ending line of a T-junction
				farVertex= EdgeTail[CurrentEdge];
				if(farVertex == T_vertex)
					farVertex= EdgeHead[CurrentEdge];
			}
			else{
				//Outgoing line is a crossing line of an intersection or a T-junction
				double angleHead= GetInnerAngle(PreviousCorner, Intercept, Vertex[EdgeHead[CurrentEdge]]);
				double angleTail= GetInnerAngle(PreviousCorner, Intercept, Vertex[EdgeTail[CurrentEdge]]);
				farVertex= EdgeTail[CurrentEdge];
				if(angleHead > angleTail)
					farVertex= EdgeHead[CurrentEdge];
			}

			PreviousCorner= Intercept;

			std::vector <bool> CheckEdge= GetEdgesToCheck(CurrentEdge);
			CheckEdge[PreviousEdge]= false;  //Do not check previously intersected edge

			if(GetCloserIntersection(Intercept, Vertex[farVertex], CheckEdge,
									 InterceptingEdge, Intercept, T_vertex))
				NextCorner= -1;  //The next corner is a T vertex
			else
				NextCorner= farVertex;  //The perimeter continues up to the far end of the edge
		}

		//Check whether the next perimeter corner is a vertex or an intersection point (T vertex)
		if(NextCorner >= 0){
			//The next corner is a vertex
			if(!removeDangling)
				PerimeterVertices.push_back(ReverseMapVertices[NextCorner]);
			CurrentVertex= NextCorner;
			if(CurrentVertex == FirstVertex)
				numLassos++;
		}

		else{
			//The next corner is an intersection point
			//(Save intersection point as a T vertex)
			Tjunction.push_back(Intercept);
			NumTjunction--;
			long i= ReverseMapEdges[CurrentEdge];
			long j= ReverseMapEdges[InterceptingEdge];
			if(i > j)
				std::swap(i, j);
			Tedges.push_back(std::vector <long> ());
			Tedges[-NumTjunction-1].push_back(i);
			Tedges[-NumTjunction-1].push_back(j);
			PerimeterVertices.push_back(NumTjunction);
		}

	}
	while (counterSides <maxEdgesInLoops &&     //This is to prevent an endless loop 
	       numLassos <= danglindInFirstVertex); //This is normal circuit termination
}


//public:


void CCuePerimeter::GetPerimeter(CDB_Model *DDBB)
//Detects apparent contours:
//	Determines the set of vertices and edges that belong to the perimeter
//	of a graph made out of vertices and edges

//The input is the set of vertices and edges in the DataBase "DDBB"

//The graph may consist of a set of subgraphs, which must have been previously calculated

//The output is the set of vectors containing the list of vertices and edges
//of the perimeter of every subgraph, plus the perimeter of the full line-drawing:
//		PerimeterVertices
//		PerimeterEdges

//Based on:
//    Company P., Varley P.A.C. and Plumed R.
//    "Perimeter detection in sketched drawings of polyhedral shapes"
//    STAG: Smart Tools and Applications in Graphics (2017)
//    A. Giachetti, P. Pingi and F. Stanco (Editors)

{
	long maxGraph= ReferApp.Cu.SG.NumSubgraphs;
	if(maxGraph == 1) maxGraph= 0;

	IncludeDanglingEdges= ReferApp.Cu.Pe.IncludeDanglingEdges;
	ThresholdIntersection= ReferApp.Cu.Pe.ThresholdIntersection;

	//Loop over all the subgraphs
	//(Note that subgraph 0 is the whole graph!!)
	for(long NumGraph= 0; NumGraph<=maxGraph; NumGraph++){
		//Local memory for mapping backwards the vertices and edges of the perimeter of each subgraph
		std::vector <long> ReverseMapVertices;
		std::vector <long> ReverseMapEdges;

		if(!InputSubGraph(DDBB, NumGraph, ReverseMapVertices, ReverseMapEdges)){
			//AfxMessageBox("Unable to input data to get perimeter");
			return;
		}
		GetImplicitData();

		//Reset database
		ReferApp.Cu.Pe.PerimeterVertices.push_back(std::vector<long>());
		ReferApp.Cu.Pe.PerimeterEdges.push_back(std::vector<long>());
		ReferApp.Cu.Pe.Tjunction.push_back(std::vector<POINT2D>());
		ReferApp.Cu.Pe.Tedges.push_back(std::vector<std::vector<long>>());

		//Calculate perimeter
		PerimeterByEnvelope(ReverseMapVertices,
							ReverseMapEdges,
							ReferApp.Cu.Pe.PerimeterVertices[NumGraph],
							ReferApp.Cu.Pe.PerimeterEdges[NumGraph],
							ReferApp.Cu.Pe.Tjunction[NumGraph],
							ReferApp.Cu.Pe.Tedges[NumGraph]);
	}
}