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
#include <algorithm>    // std::reverse

#include "Tools_Vector.h"
#include "Tools_Geometry.h"

#include "DB_Model.h"
#include "DB_Model_Maths.h"
#include "DB_CSG.h"
#include "Refer.h"
#include "CSG_Maths.h"


bool IsVertexInVector(CVertex vertex, std::vector<CVertex> vertices)
//Returns true if a vertex with similar 2D coordinates to "vertex"
//already exists in the vector "vertices"

//Vertices are considered "similar" if they are within a threshold distance
{
	for(long i=0; i<(long)vertices.size(); i++){
		if (MODULE_2D(vertex, vertices[i]) < ReferApp.CSG.CSG_ThresholdLocation)
			return true;
	}

	return false;

	//Alternative code to detect the location of the closest vertex
	//bool isVertex= false;
	//double distance= 0;

	//for(long i=0; i<(long)vertices.size(); i++){
	//	double dist= MODULE_2D(vertex, vertices[i]);
	//	if (dist < ReferApp.CSG.ThresholdLocation){
	//		if (!isVertex || dist < distance){
	//			distance= dist;
	//			location= i;
	//			isVertex= true;
	//		}
	//	}
	//}

	//return isVertex;
}


long GetVertexNumInVector(CVertex Vertex, std::vector<CVertex> Vertices)
//Returns the pointer to the location of vertex "Vertex" in the list of "Vertices"

//The vertex must fit with exactly the same coordinates

//Returns -1 if not found
{
	for(long i= 0; i< (long) Vertices.size(); i++){
		if( (Vertices[i].x == Vertex.x) && (Vertices[i].y == Vertex.y))
			return i;
	}
	return -1;
}


void RemoveVertexFromVector(CVertex Vertex, std::vector<CVertex> &Vertices)
{
	long position= GetVertexNumInVector(Vertex, Vertices);
	if (position != -1)
		Vertices.erase(Vertices.begin()+position);
}


double GetBoundingBoxArea(CDB_Model *DDBB, 
						  std::vector<long> FeatureFaces)
//Returns the area of the bounding box of the faces of feature
{
	double Area= -1;
	CVerticesSet SetOfVertices;
	std::vector<long> ListNumVertices;
	ListNumVertices= DDBB->GetAllVerticesInFace(DDBB->GetFace(FeatureFaces[0]));

	for(long f=1; f<(long)FeatureFaces.size(); f++){
		std::vector<long> ListNumLateralVertices;
		ListNumLateralVertices= DDBB->GetAllVerticesInFace(DDBB->GetFace(FeatureFaces[f]));

		for(long v=0; v<(long)ListNumLateralVertices.size(); v++){
			if(!IsValueInVector(ListNumLateralVertices[v], ListNumVertices))
				ListNumVertices.push_back(ListNumLateralVertices[v]);
		}
	}

	for(long r=0; r<(long) ListNumVertices.size(); r++){
		CVertex Vertex= DDBB->GetVertex(ListNumVertices[r]);
		SetOfVertices.AddVertex(Vertex);
	}

	double Xmin, Ymin, Zmin, Xmax, Ymax, Zmax;
	SetOfVertices.GetBoundingBox(Xmin, Ymin, Zmin, Xmax, Ymax, Zmax);

	Area= std::fabs(Xmax-Xmin)*std::fabs(Ymax-Ymin);

	return Area;
}


long IsMainDirection(double angle, std::vector<double> MainAngles)
//Determines if the angle (or its conjugate) is similar 
//to the direction of any of the three axis of the main coordinate system

//Input angle is in the range [0, PI)

//Returns one of the following flags:
//	-1 if Angle is not similar to any main direction
//	0 if similar to X axis
//	1 if similar to Y axis
//	2 if similar to Z axis
{
	long maindirection= -1;

	for(long i= 0; i< 3; i++){
		double DeltaAngle= min((std::fabs(angle - MainAngles[i])), 
								std::fabs((angle+NUMBER_PI) - MainAngles[i]));

		//If the minimum difference between both angles is smaller than ThresholdAngle
		//then we consider the lines as parallel and we stop looking for another parallel edge
		if (DeltaAngle < ReferApp.CSG.CSG_ThresholdAngle){
			maindirection= i;
			break;
		}
	}
	return maindirection;
}


long IsMainDirection(CDB_Model *DDBB, 
					 CVertex Vertex1, CVertex Vertex2,
					 std::vector<double> MainAngles)
//Determines if the segment defined by Vertex1->Vertex2 (from DDBB)
//is parallel (whithing an angle threshold) 
//to any of the three axis of the main coordinate system

//Returns one of the following flags:
//	-1 if segment (Vertex1,Vertex2) is not collinear to any main direction
//	0 if collinear to X axis
//	1 if collinear to Y axis
//	2 if collinear to Z axis
{
	//Calculate the angle of the edge in the range [0, PI)
	POINT2D p1(Vertex1.x, Vertex1.y);
	POINT2D p2(Vertex2.x, Vertex2.y);
	double angleEdge= GetAngle2D_PI(p1, p2);

	return IsMainDirection(angleEdge, MainAngles);
}


bool IsBrokenChain(CDB_Model *DDBB_Dec_Current,
					   long V1, long V2,
					   double threshold,
					   long &Ed1, long &Ed2)
//Returns edge edg1 connected to vertex Vert1 
//and collinear to edge edg2 connected to vertex Vert2

//Both edges are NOT coonected to each other

//Returns false if such pair of edges does not exist, 
//or vertices are already connected
{
	//std::vector<long> chain;
	//if (GetChainedEdges(DDBB_Dec_Current, V1, V2, threshold, chain)){
	//	//AfxMessageBox("Unexpected linked vertices found while proccessing slots/rails");
	//	return false;
	//}

	bool isV1= false, isV2= false;

	//Calculate orientation
	CVertex Vcc1= DDBB_Dec_Current->GetVertex(V1);
	CVertex Vcc2= DDBB_Dec_Current->GetVertex(V2);
	POINT2D P1(Vcc1.x, Vcc1.y);
	POINT2D P2(Vcc2.x, Vcc2.y);
	double angleV1V2= GetAngle2D_PI(P1, P2);

	//Select external link to V1
	for(long i= 0; i<DDBB_Dec_Current->GetSizeEdgesInVertex(V1); i++){
		long EdgeI= DDBB_Dec_Current->GetEdgeInVertex(V1, i);
		//if(IsValueInVector(EdgeI, chain))
		//	continue;  //already visited

		double angleI= DDBB_Dec_Current->GetAngle2DX_PI(EdgeI, Accuracy);
		double delta= abs(angleV1V2 - angleI);

		//Check alignment
		if(delta < threshold || abs(NUMBER_PI-delta) < threshold){
			//EdgeI is aligned to V1V2
			Ed1= EdgeI;
			isV1= true;
			break;
		}
	}

	//Select external link to V2
	for(long i= 0; i<DDBB_Dec_Current->GetSizeEdgesInVertex(V2); i++){
		long EdgeI= DDBB_Dec_Current->GetEdgeInVertex(V2, i);
		//if(IsValueInVector(EdgeI, chain))
		//	continue;  //already visited

		double angleI= DDBB_Dec_Current->GetAngle2DX_PI(EdgeI, Accuracy);
		double delta= abs(angleV1V2 - angleI);

		//Check alignment
		if(delta < threshold || abs(NUMBER_PI-delta) < threshold){
			//EdgeI is aligned to V1V2
			Ed2= EdgeI;
			isV2= true;
			break;
		}
	}

	return (isV1 && isV2);
}


bool GetChainedEdges(CDB_Model *DB_DatumGraph, 
					 long V1, long V2, 
					 double threshold,
					 std::vector<long> &chain)
//Returns the chain of collinear edges that connects vertices Vert1 and Vert2

//Collinearity is checked whith a threshold

//Return false if such a connexion does not exist
{
	//Calculate orientation
	CVertex Vcc1= DB_DatumGraph->GetVertex(V1);
	CVertex Vcc2= DB_DatumGraph->GetVertex(V2);
	POINT2D P1(Vcc1.x, Vcc1.y);
	POINT2D P2(Vcc2.x, Vcc2.y);
	double angleV1V2= GetAngle2D_PI(P1, P2);

	long currentV= V1;
	long endV= V2;
	bool found= false;

	//Search for edges that are both consecutive and aligned
	long numEdgesToExplore= DB_DatumGraph->GetSizeEdgesInVertex(currentV);

	for(long i= 0; i<numEdgesToExplore; i++){
		//Select consecutive edges
		long EdgeI= DB_DatumGraph->GetEdgeInVertex(currentV, i);
		if(IsValueInVector(EdgeI, chain))
			continue;  //already visited

		double angleI= DB_DatumGraph->GetAngle2DX_PI(EdgeI, Accuracy);
		double delta= abs(angleV1V2 - angleI);

		//Check alignment
		if(delta < threshold || abs(NUMBER_PI-delta) < threshold){
			//EdgeI is next in chain
			chain.push_back(EdgeI);
			CEdge edgeC= DB_DatumGraph->GetEdge(EdgeI);
			if(currentV == edgeC.Head)
				currentV= edgeC.Tail;
			else
				currentV= edgeC.Head;


			if(currentV == endV){
				found= true;
				break;
			}
			else{
				numEdgesToExplore= DB_DatumGraph->GetSizeEdgesInVertex(currentV);
				i= -1;
			}
		}
	}

	if(!found)
		chain.clear();

	return (chain.size() != 0);
}


long getVertexInChain(CDB_Model *DB_DatumGraph, 
					  long V1, long V2, 
					  CVertex insertVert,
					  std::vector<long> chain)
//Returns the number of the vertex located at cordinates "insertVert"

//The vertex must belong to "chain" that runs from VC1 to VC2.

//If the vertex does not exists, updates DatumGraph,
//by inserting "midVert" in "chain" 
//and spliting the corresponding link of the chain
{
	long inV;

	CVertex Vcc1= DB_DatumGraph->GetVertex(V1);
	CVertex Vcc2= DB_DatumGraph->GetVertex(V2);
	double deltaX= Vcc2.x-Vcc1.x;
	double deltaY= Vcc2.y-Vcc1.y;
	double length= MODULE_2D(Vcc1, Vcc2);
	double threshold= 0.10 * length;

	bool smallStep= (abs(deltaX) > abs(deltaY));

	double tInsert;
	if(smallStep)	tInsert= (insertVert.x-Vcc1.x) / deltaX;
	else			tInsert= (insertVert.y-Vcc1.y) / deltaY;


	long numLinks= (long)chain.size();
	long prev= V1;
	long next;

	CEdge midEdge;

	for (long i= 0; i<numLinks; i++){
		CEdge edgeI= DB_DatumGraph->GetEdge(chain[i]);
		if(prev == edgeI.Head)
			next= edgeI.Tail;
		else
			next= edgeI.Head;
		CVertex vertexN= DB_DatumGraph->GetVertex(next);

		double t;
		if(smallStep)	t= (vertexN.x-Vcc1.x) / deltaX;
		else			t= (vertexN.y-Vcc1.y) / deltaY;

		if(t>(tInsert-Accuracy) && t<(tInsert+Accuracy)){
			//Return the location of the vertex
			inV= next;
			break;
		}
		else if(t > tInsert){
			//Insert the vertex in the current link
			DB_DatumGraph->AddVertex(insertVert);
			inV= DB_DatumGraph->GetSizeVertices()-1;

			//Split the edge
			midEdge= edgeI;
			if(midEdge.Head == prev)
				midEdge.Tail= inV;
			else
				midEdge.Head= inV;
			DB_DatumGraph->SetEdge(chain[i], midEdge);

			CEdge NewEdge(next, inV);
			DB_DatumGraph->AddEdge(NewEdge);

			break;
		}

		prev= next;
	}

	return inV;
}


CPlaneOrientation PlaneOrientation(CDB_Model *DB_DatumGraph, 
								   CDB_CSG *DB_CSG_Model,
								   std::vector<long> Vertices,
								   std::vector<double> MainAngles)
//Returns the orientation of the plane 
//that contains the face defined by "Vertices" that belong to DB_DatumGraph

//Determines if there is at least one edge parallel to any of the main directions
//Then determines if the face is parallel to any of the main reference planes
//(All of its edges are parallel to one of exactly two different directions).

//Consequently, orientations are enumerated as:
//		XY,
//		XZ,
//		YZ,
//		SLANTED,
//		UNKNOWN

//If NO match is found, the candidate plane is compared to current datum planes,
//to determine whether the candidate matches any of them
{
	CPlaneOrientation Plane= UNKNOWN;

	//Check if edges are parallel to main directions
	long numVert= Vertices.size();
	if (numVert == 0)
		return Plane;

	bool x= false, y= false, z= false;
	CVertex VertexI= DB_DatumGraph->GetVertex(Vertices[0]);
	for(long i= 0; i<numVert; i++){
		CVertex VertexII= DB_DatumGraph->GetVertex(Vertices[(i+1) % numVert]);
		long direction= IsMainDirection(DB_DatumGraph, VertexI, VertexII, MainAngles);

		if ((direction == 0)&&(!x)) x= true;
		if ((direction == 1)&&(!y)) y= true;
		if ((direction == 2)&&(!z)) z= true;

		VertexI= VertexII;
	}

	//Check if plane is parallel to any of the main reference planes
	if (x && y && !z){
		return XY;
	}
	if (x && !y && z){
		return XZ;
	}
	if (!x && y && z){
		return YZ;
	}

	//Alternatively, try to match the plane to any datum plane
	//--------------------------------------------------------
	for(long i= 0; i<(long)DB_CSG_Model->DatumPlanesSize(); i++){
		//Count shared vertices
		long num= 0;
		for(long j= 0; j<(long)Vertices.size(); j++){
			if(IsValueInVector(Vertices[j], DB_CSG_Model->GetDatumPlaneVertices(i)))
				num++;
		}

		//Check matching
		if(num == (long)Vertices.size()){
			//Current datum plane matches the candidate face
			Plane= DB_CSG_Model->GetDatumPlaneOrientation(i);
			break;
		}
	}

	return Plane;
}


bool UpdateDistance(CDB_Model *DB_DatumGraph,
					std::vector<double> MainAngles,
					std::vector<double> ReductionCoefficients,
					CVertex Vertex1, CVertex Vertex2,
					COrthoDistance &Distance)
//Increases (or decreases) the input orthogonal Distance 
//by adding the distance between Vertex1 and Vertex2,
//iff they define a segment parallel to one of the three main directions

//Otherwhise Distance is returned unchanged

//The distance is scaled by the corresponding Reduction Coefficient
{
	long direction= IsMainDirection(DB_DatumGraph, Vertex1, Vertex2, MainAngles);

	if(direction < 0 || direction > 2){
		//AfxMessageBox("CSG module unable to measure some orthogonal distances");
		return false;
	}

	double angle= DB_DatumGraph->GetAngle2DX_2PI(Vertex1, Vertex2);
	double dist= MODULE_2D(Vertex1, Vertex2) / ReductionCoefficients[direction];

	switch(direction){
		case 0:{
			if(std::fabs(angle-MainAngles[0])< ReferApp.CSG.CSG_ThresholdAngle)
				Distance.x += dist;
			if((std::fabs(angle-MainAngles[0])> NUMBER_PI-ReferApp.CSG.CSG_ThresholdAngle) && 
			   (std::fabs(angle-MainAngles[0])< NUMBER_PI+ReferApp.CSG.CSG_ThresholdAngle))
				Distance.x -= dist;
		};
		break;

		case 1:{
			if(std::fabs(angle-MainAngles[1])< ReferApp.CSG.CSG_ThresholdAngle)
				Distance.y += dist;
			if((std::fabs(angle-MainAngles[1])> NUMBER_PI-ReferApp.CSG.CSG_ThresholdAngle) &&
			   (std::fabs(angle-MainAngles[1])< NUMBER_PI+ReferApp.CSG.CSG_ThresholdAngle))
				Distance.y -= dist;
		}
		break;

		case 2:{
			if(std::fabs(angle-MainAngles[2])< ReferApp.CSG.CSG_ThresholdAngle)
				Distance.z += dist;
			if((std::fabs(angle-MainAngles[2])> NUMBER_PI-ReferApp.CSG.CSG_ThresholdAngle) && 
			   (std::fabs(angle-MainAngles[2])< NUMBER_PI+ReferApp.CSG.CSG_ThresholdAngle))
				Distance.z -= dist;
		};
		break;
	}

	return true;
}


std::vector<std::vector<std::pair<long, long>>> construct_Dijkstra_Adj(CDB_Model *DB_DatumGraph)
//Store the datum graph as adjacency list: 
//	-for each vertex v, adj[v] contains the list of edges going from this vertex
//	-Edges are stored in pairs, where the second value in the pair is the "cost" or "weight" of transversing this edge:
//		Assigning weight 1 to all the edges will minimize the edges transversed to connect two vertices
//		Assigning lengths as weitgh will prioritize paths through longer edges 
//		(which are less prone to imperfections in sketched drawings)
{
	std::vector<std::vector<std::pair<long, long>>> adj;

	long numVertices= DB_DatumGraph->GetSizeVertices();

	for (long i= 0; i<numVertices; i++){
		adj.push_back(std::vector<std::pair<long, long>>());

		std::vector<long> edgesI= DB_DatumGraph->GetAllEdgesInVertex(i);
		long numEdges= edgesI.size();
		for (long j= 0; j<numEdges; j++){
			CEdge edge= DB_DatumGraph->GetEdge(edgesI[j]);

			std::pair<long, long> adjEdge;
			if(edge.Head == i)
				adjEdge.first = edge.Tail;
			else
				adjEdge.first = edge.Head;

			if(ReferApp.DIA.NormalonEdges[edgesI[j]] >= 0)
				adjEdge.second = 1;
			else
				adjEdge.second = 10;   //Very big weight for non-orthogonal edges

			adj[i].push_back(adjEdge);
		}
	}

	return adj;
}


void get_Dijkstra_predecessors(CDB_Model *DB_DatumGraph, long s,
							   std::vector<long> &predecessors)
//The function takes the starting vertex s 

//After performing all the iterations array distances[] stores 
//the lengths of the shortest paths to all vertices, 
//and array predecessors[] stores the predecessors of all vertices
//(except starting vertex s).


//If relaxation along the edge is possible 
//(i.e. distance d[to] can be improved), 
//the distance d[to] and predecessor p[to] are updated.

//MOdified from:
//https://cp-algorithms.com/graph/dijkstra.html
{
	//The graph DB_DatumGraph is stored as adjacency list
	std::vector<std::vector<std::pair<long, long>>> adj= construct_Dijkstra_Adj(DB_DatumGraph);

	//Initialize arrays
	long n = (long)adj.size();
	std::vector<long> distances;
	distances.assign(n, INT_MAX);

	std::vector<bool> u(n, false);  //labels

	predecessors.assign(n, -1);

	//Perform n iterations. 
	//At each iteration the vertex v is selected 
	//which has the smallest distance d[v] among all the unmarked vertices
	distances[s] = 0;
	for (long i = 0; i < n; i++) {
		int v = -1;
		for (long j = 0; j < n; j++) {
			if (!u[j] && (v == -1 || distances[j] < distances[v]))
				v = j;
		}

		//If the distance to selected vertex v is equal to infinity, 
		//the algorithm stops
		if (distances[v] >= INT_MAX)
			break;

		//Otherwise the vertex is marked, 
		//and all the edges going out from this vertex are checked
		u[v] = true;

		//for (auto edge : adj[v]) {   //Range based for (since C++11)
		long sizeV= adj[v].size();
		for (long j= 0; j<sizeV; j++){
			std::pair<long, long> edge= adj[v][j];

			long to = edge.first;
			long len = edge.second;

			if (distances[v] + len < distances[to]) {
				distances[to] = distances[v] + len;
				predecessors[to] = v;
			}
		}
	}
}



void get_Dijkstra_path(std::vector<long> predecessors,
					   long s, long t, std::vector<long> &path)
//Uses the list of predecessors calcultated by getDijkstra_predecessors
//to return the path that connects s to t
{
static bool warning= false;
	path.clear();

	//Follow the pach backwards, from t
	for (long v = t; v != s; v = predecessors[v]){
		if (IsValueInVector(v, path)){
			if(!warning){
				AfxMessageBox("Failure while calculating orthogonal distances: Unexpected loop in Dijkstra path! Possible failures in *.SSG file ortho-distances");
				warning= true;
			}
			return;
		}

		if(v == -1){
			if(!warning){
				AfxMessageBox("Failure while calculating orthogonal distances: Predecessor not found in Dijkstra path! Possible failures in *.SSG file ortho-distances");
				warning= true;
			}
			return;
		}

		path.push_back(v);
	}
	//Add the source
	path.push_back(s);

	//Reverse the path
	reverse(path.begin(), path.end());
}


COrthoDistance CalculeOrthoDistance(CDB_Model *DB_DatumGraph,
									long Origin, 
									std::vector<long> predecessors,
									long Vertex,
									std::vector<double> MainAngles,
									std::vector<double> ReductionCoefficients)
//Calculates the "orthogonal" distance.from "Origin" to "Vertex" 
//following a polyline path whose segments are parallel to "MainAngles"
{
	//Calculate orthogonal path
	std::vector<long> path;
	get_Dijkstra_path(predecessors, Origin, Vertex, path);

	//Calculate orthogonal distance, by accumulating lengths of succesive stretchs
	COrthoDistance Distance(0, 0, 0);
	long numPath= path.size();
	for(long j=0; j<numPath-1; j++){
		UpdateDistance(DB_DatumGraph, MainAngles, ReductionCoefficients,
					   DB_DatumGraph->GetVertex(path[j]), 
					   DB_DatumGraph->GetVertex(path[j+1]),
					   Distance);
	}

	return Distance;
}


double CalculeNormalDistance(CDB_Model *DB_DatumGraph,
							 CPlaneOrientation Orientation,
							 long Origin, 
							 std::vector<long> predecessors,
							 std::vector<long> Vertices,
							 std::vector<double> MainAngles,
							 std::vector<double> ReductionCoefficients)
//Returns average distance perpendicular to the vertices of the polygonal face

//Calculates the "orthogonal" distance.from the "Origin" vertex 
//to the plane defined by the polygonal face "Vertices"
//following a polyline path whose segments are parallel to "MainAngles"

//Then returns only the component normal to the plane
{
	double normalDistance= 0;
	long numVertices= (long)Vertices.size();

	for(long i=0; i<numVertices; i++){
		//Calculate orthogonal distance
		COrthoDistance Distance= CalculeOrthoDistance(DB_DatumGraph,
													  Origin, 
													  predecessors,
													  Vertices[i],
													  MainAngles,
													  ReductionCoefficients);

		//Calculate normal distance according to orientation
		switch (Orientation){
			case XY:{normalDistance+= Distance.z;} break;
			case XZ:{normalDistance+= Distance.y;} break;
			case YZ:{normalDistance+= Distance.x;} break;
		}
	}

	//Return average normal distance
	return normalDistance/numVertices;
}
