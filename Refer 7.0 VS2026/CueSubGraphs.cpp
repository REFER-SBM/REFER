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
#include <queue>

#include <vector>

#include "Tools_Vector.h"
#include "Tools_Geometry.h"

#include "DB_Model.h"
#include "Refer.h"

#include "CueSubGraphs.h"


//WARNING: an embeeded version of this code
//is used to calculate symmetry planes in "CueSymmetry"




void CCueSubGraphs::InputData(CDB_Model *DDBB)
// Copies information from external DataBase
// into the local database

// The intput required by the algorithms is:
//		VertexCount= number of vertex in the wireframe
//		VertexX[i]= X coordinate of the i-th vertex
//		VertexY[i]= Y coordinate of the i-th vertex
//		VertexZ[i]= Z coordinate of the i-th vertex (VertexZ not required)

//		EdgeCount= number of edges in the wireframe
//		EdgeU[i]= Head vertex defining the i-th edge
//		EdgeV[i]= Tail vertex defining the i-th edge
{

	//Copy vertices from global to local database
	VertexX.clear();
	VertexY.clear();
	VertexCount= DDBB->GetSizeVertices();
	for (long i=0; i<VertexCount; i++){
		CVertex V= DDBB->GetVertex(i);
		VertexX.push_back(V.x);
		VertexY.push_back(V.y);
	}

	//Copy edges from global to local database
	EdgeHead.clear();
	EdgeTail.clear();
	EdgeCount= DDBB->GetSizeEdges();
	for (long i=0; i<EdgeCount; i++){
		CEdge E= DDBB->GetEdge(i);

		EdgeHead.push_back(E.Head);
		EdgeTail.push_back(E.Tail);
	}
}


void CCueSubGraphs::DetermineSubgraphs()
//Break down the graph of the line-drawing into subgraphs as follows:
//	- Selects a not yet visited vertex and assigns it in the current sub-graph
//	- Visits all edges not yet assigned in any subraph
//	  to add to the current subgraph those connected to any vertex assigned to the current subgraph
//	- Begin a new subgraph and repeat previous stages

//Returns labels that identify the subgraph where each vertex and edge belong
//as much as the number of subgraphs:
//    SubgraphCount is the number of subgraphs contained in the model
//    SubgraphForVertex[i] contains the number of the subgraphs where vertex i belongs
//    SubgraphForEdge[i] contains the number of the subgraphs where edge i belongs
{
	//Identify subgraphs, until no more vertices remain unassigned
	for (;;)
	{
		// identify an unassigned vertex
		int V = -1;
		for (int i=0; i<VertexCount; ++i)  if (SubgraphForVertex[i]==-1)
		{
			V = i;
			break;
		}

		if (V < 0)  break;  // no more unassigned vertices

		//Start new subgraph
		SubgraphForVertex[V] = SubgraphCount;
		for (;;)
		{
			bool Progress = false;

			//Search for edges not yet visited, and connected to the current subgraph
			for (int i=0; i<EdgeCount; ++i)  if (SubgraphForEdge[i] == -1)
			{
				if (SubgraphForVertex[EdgeHead[i]] == SubgraphCount)
				{
					SubgraphForEdge[i] = SubgraphCount;
					SubgraphForVertex[EdgeTail[i]] = SubgraphCount;
					Progress = true;
				}
				else
				if (SubgraphForVertex[EdgeTail[i]] == SubgraphCount)
				{
					SubgraphForEdge[i] = SubgraphCount;
					SubgraphForVertex[EdgeHead[i]] = SubgraphCount;
					Progress = true;
				}
			}
			if (!Progress)  break;  //Current graph already visited
		}
		++SubgraphCount;
	}
}


long CCueSubGraphs::majority4 (long a, long b, long c, long d)
{
	if ((a == b) || (a == c) || (a == d))  return a;
	if ((b == c) || (b == d))  return b;
	if (c == d)  return c;
	return 0;
}


long CCueSubGraphs::GetOuterSubgraph()
//Identifies the outer subgraph: the one that encloses all the other
{
	//Determine extrema
	long LeftVertex= 0, RightVertex= 0, TopVertex= 0, BottomVertex= 0;
	for (int j = 1;  j < VertexCount;  ++j){
		if (VertexX[j] < VertexX[LeftVertex])  LeftVertex = j;
		if (VertexX[j] > VertexX[RightVertex])  RightVertex = j;
		if (VertexY[j] < VertexY[TopVertex])  TopVertex = j;
		if (VertexY[j] > VertexY[BottomVertex])  BottomVertex = j;
	}

	return majority4(SubgraphForVertex[BottomVertex],
					 SubgraphForVertex[LeftVertex],
					 SubgraphForVertex[RightVertex],
					 SubgraphForVertex[TopVertex]);
}


std::vector<long> CCueSubGraphs::GetAllEdgesInVertex(long iVertex)
// Returns the edges connected to the vertex "iVertex"
{
	std::vector<long> Destination;
	for(long i= 0; i<EdgeCount; i++){
		if(EdgeHead[i]==iVertex || EdgeTail[i]==iVertex)
			Destination.push_back(i);
	}
	return Destination;
}


void CCueSubGraphs::GetVisitedVerticesAndEdges(long first, std::vector<bool> &visited)
//Labels the vertices and edges that belong to the same subgraph
//than vertex "first"

//Breadth First Search is applied to visit all those vertices
{
	std::queue<long> Q;

	//Insert the first vertex in the queue
	Q.push(first);
	visited[first] = true;
	SubgraphForVertex[first]= SubgraphCount;

	//Loop to visit all the vertices
	//that belong to the same graph than vertex "first"
	while(!Q.empty()){
		//Extract the front vertex from the queue
		long v = Q.front();
		Q.pop();

		//Search all direct "sons" (vertices connected to vertex "v")
		std::vector<long> Sons;
		std::vector<long> EdgesInF= GetAllEdgesInVertex(v);
		for (long i= 0; i<(long)EdgesInF.size(); i++){
			SubgraphForEdge[EdgesInF[i]]= SubgraphCount;
			if(EdgeHead[EdgesInF[i]] == v)
				Sons.push_back(EdgeTail[EdgesInF[i]]);
			else
				Sons.push_back(EdgeHead[EdgesInF[i]]);
		}

		//Insert son vertices in the queue
		for(long u= 0; u<(long)Sons.size(); u++){
			if(SubgraphForVertex[Sons[u]] != SubgraphCount){
				Q.push(Sons[u]);
				visited[Sons[u]] = true;
				SubgraphForVertex[Sons[u]]= SubgraphCount;
			}
		}
	}
}


void CCueSubGraphs::GetNextGraph(long first, std::vector<bool> &visited)
//Breadth First Search is recursively applied to visit all vertices
//connected to first

//Returns labels that identify the subgraph where each vertex and edge belong
//as much as the number of subgraphs:
//    SubgraphCount is the number of subgraphs contained in the model
//    SubgraphForVertex[i] contains the number of the subgraphs where vertex i belongs
//    SubgraphForEdge[i] contains the number of the subgraphs where edge i belongs

{
	//BFS to get the vertices and edges connected to vertex "first"
	GetVisitedVerticesAndEdges(first, visited);
	SubgraphCount++;

	//Search for the next sub-graph
	for (long i= 0; i <(long)visited.size(); i++){
		if(!visited[i]){
			GetNextGraph(i, visited);
			break;
		};
	}

	return;
}


void CCueSubGraphs::SaveCurrentSubgraph(long numgraph, std::vector<CDB_Model> &Graphs)
//Returns a graph containing all the vertices and edges belonging to the subgraph "numgraph"
{
	CDB_Model DDBBAux;

	//Add vertices
	for (long i= 0; i <VertexCount; i++){
		if(SubgraphForVertex[i] == numgraph){
			CVertex Vertex (VertexX[i], VertexY[i], 0);
			DDBBAux.AddVertex(Vertex);
		};
	}

	//Add edges
	for (long i= 0; i <EdgeCount; i++){
		if(SubgraphForEdge[i] == numgraph){
			CEdge Edge;

			//Renumber vertices
			long head= -1;
			for(long j= EdgeHead[i]; j>=0; j--)
				if(SubgraphForVertex[j] == numgraph) head++;
			Edge.Head= head;
			long tail= -1;
			for(long j= EdgeTail[i]; j>=0; j--)
				if(SubgraphForVertex[j] == numgraph) tail++;
			Edge.Tail= tail;

			//Save edge
			DDBBAux.AddEdge(Edge);
		};
	}

	Graphs.push_back(DDBBAux);
}


void CCueSubGraphs::GetSubGraphs()
//Determines the subgraphs existing in the graph stored in the database

//Two similar approaches are available:
//	-Varley approach (compact):
//		Uses iterative search to break down the wireframe into subgraphs
//		Returns labels that identify the subgraph where each vertex and edge belong
//		as much as the number of subgraphs

//	-Company approach (explanative):
//		Uses Breadth First Search to isolate the graph containing vertex "0"
//		The process is recursively repeated until all the vertices have been
//		visited by one of the sub-graphs

//Returns labels that identify the subgraph where each vertex and edge belong
//as much as the number of subgraphs:
//    SubgraphCount is the number of subgraphs contained in the model
//    SubgraphForVertex[i] contains the number of the subgraphs where vertex i belongs
//    SubgraphForEdge[i] contains the number of the subgraphs where edge i belongs
{
	//Reset subgraph labels
	SubgraphCount= 0;
	SubgraphForVertex.assign(VertexCount, -1);
	SubgraphForEdge.assign(EdgeCount, -1);

	//Select the approach
	if (ReferApp.Cu.SG.SubGraphsApproach == 0){
		//Varley
		DetermineSubgraphs();
	}

	else{
		//Company
		std::vector<bool> visited(VertexCount, false);
		GetNextGraph(0, visited);
	}
}


void CCueSubGraphs::SplitEdge(CDB_Model *DDBB, long Position)
//Edge "Position" is splitted into two sub-edges
{
	//Get mid-point
	CEdge Edge= DDBB->GetEdge(Position);
	CVertex VertexFirst= DDBB->GetVertex(Edge.Tail);
	CVertex VertexLast= DDBB->GetVertex(Edge.Head);
	CVertex VertexMid ((VertexFirst.x + VertexLast.x)/2, 
					   (VertexFirst.y + VertexLast.y)/2,
					   0);

	//Add semi-edge
	DDBB->AddVertex(VertexMid);
	CEdge EdgeMid;
	EdgeMid.Tail= Edge.Tail;
	EdgeMid.Head= DDBB->GetSizeVertices()-1;
	DDBB->AddEdge(EdgeMid);

	DDBB->AddVertex(VertexMid);
	EdgeMid.Tail= Edge.Head;
	EdgeMid.Head= DDBB->GetSizeVertices()-1;
	DDBB->AddEdge(EdgeMid);

	//Remove the original edge
	DDBB->RemoveEdge(Position, false);
}


void CCueSubGraphs::GetSubGraphs(CDB_Model *DDBB)
//Determines the subgraphs existing in the graph stored in the database

//Returns the information about subgraphs in the database ReferApp.Cu.SG
//Also identifies the outer subgraph (the one that encloses all the other)
{
	//Input data
	InputData(DDBB);

	//Get subgraphs
	GetSubGraphs();

	//Output subgraphs from local to global database
	ReferApp.Cu.SG.NumSubgraphs= SubgraphCount;
	ReferApp.Cu.SG.SubgraphForEdge= SubgraphForEdge;
	ReferApp.Cu.SG.SubgraphForVertex= SubgraphForVertex;

	//Get the outer subgraph
	ReferApp.Cu.SG.Outer= GetOuterSubgraph();
}


void CCueSubGraphs::GetSubGraphs(CDB_Model *DDBB, std::vector<CDB_Model> &Graphs)
//Determines the subgraphs existing in the graph stored in the database

//Stores separately each subgraph (as a new database)
{
	//Input data
	InputData(DDBB);

	//Get subgraphs
	GetSubGraphs();

	//Save subgraphs in separate databases
	Graphs.clear();
	for (long i= 0; i <SubgraphCount; i++){
		SaveCurrentSubgraph(i, Graphs);
	}
}


void CCueSubGraphs::CalculateSubgraphs(CDB_Model *DDBB,
									   long &OutSubgraphCount,  //Number of subgraphs
									   std::vector <long> &OutSubgraphForVertex, //Number of the subgraph where each vertex belongs
									   std::vector <long> &OutSubgraphForEdge) //Number of the subgraph where each edge belongs
//Uses Varley's approach to determine subgraphs
//Then, outputs the information about subgraphs
{
	//Create local database
	if(DDBB->GetSizeVertices() == 0) return;
	InputData(DDBB);

	SubgraphCount= 0;
	SubgraphForVertex.assign(VertexCount, -1);
	SubgraphForEdge.assign(EdgeCount, -1);

	//Uses Varley's approach to determine subgraphs
	DetermineSubgraphs();

	//Outputs the information about subgraphs
	OutSubgraphCount= SubgraphCount;
	OutSubgraphForVertex= SubgraphForVertex;
	OutSubgraphForEdge= SubgraphForEdge;
}


bool CCueSubGraphs::IsMultiGraph(CDB_Model *DDBB)
//Returns true if the database contains a multigraph model,
//and false if the model determines a single graph

//It is useful to analyze derived models
//(like models obtained after removing some edges or vertices,
//using, for instance, SplitByCircuit or SplitByVertex)

//Breadth First Search is applied to visit vertices linked to vertex 0,
//and determine whether the model is a single or multiple graph
{
	if(DDBB->GetSizeVertices() == 0) return false;

	InputData(DDBB);

	//GetSubGraphs();

	//if(SubgraphCount > 0)
	//	return true;
	//else
	//	return false;

	//Get only the vertices visited from vertex first
	long first= 0;
	SubgraphCount= 0;
	std::vector<bool> visited(VertexCount, false);
	SubgraphForVertex.assign(VertexCount, -1);
	SubgraphForEdge.assign(EdgeCount, -1);

	GetVisitedVerticesAndEdges(first, visited);

	//The model is multigraph if some vertex has not been visited
	for (long i= 0; i <(long)SubgraphForVertex.size(); i++){
		if(SubgraphForVertex[i] == -1) return true;
	}
	return false;
}


void CCueSubGraphs::SplitVertex(CDB_Model *DDBB, long Position,
								std::vector <long> &NewVertices)
//Modifies the graph that defines the model by splitting one of its vertices

//Vertex "Position" is splitted into a set of overlapped vertices
//Edges connected to vertex "position" are re-connected to those new vertices

//Returns the list of numbers of the new verties
{
	//Recover the information of the original vertex, and remove it
	CVertex Vertex= DDBB->GetVertex(Position);
	std::vector<long> EdgesInVertex= DDBB->GetAllEdgesInVertex(Position); //Save the list, as the endpoints of the edges are changed
	DDBB->RemoveVertex(Position, true);

	//Split the vertex and reconnect concurrent edges
	for (long k=0; k<(long)EdgesInVertex.size();k++){
		CEdge Arista= DDBB->GetEdge(EdgesInVertex[k]);
		if (Arista.Tail == Position){
			DDBB->AddVertex(Vertex);
			long numV= DDBB->GetSizeVertices()-1;
			Arista.Tail= numV;
			DDBB->SetEdge(EdgesInVertex[k], Arista);
			NewVertices.push_back(numV);
		}
		else if (Arista.Head == Position){
			DDBB->AddVertex(Vertex);
			long numV= DDBB->GetSizeVertices()-1;
			Arista.Head= numV;
			DDBB->SetEdge(EdgesInVertex[k], Arista);
			NewVertices.push_back(numV);
		}
	}
};


void CCueSubGraphs::SplitVertex(CDB_Model *DDBB, long Position)
//Modifies the graph that defines the model by splitting one of its vertices

//Vertex "Position" is splitted into a set of overlapped vertices
//Edges connected to vertex "position" are re-connected to those new vertices
{
	std::vector <long> NewVertices;
	SplitVertex(DDBB, Position, NewVertices);
}


void CCueSubGraphs::SplitByCircuit(CDB_Model *DDBB, std::vector<long> Circuit)
//Modifies the graph that defines the model 
//by splitting those vertices and edges that belong to the circuit

//Information is stored in Circuit as follows:
//		- vertices are stored by their numbers
//		- midpoints are stored by the number of the edge they belong to
//		  switched to negative and decreased by one unit
//		  (i.e. midpoint of edge 3 is stored as -4)

//This function is used for display purposes
{

	//Remove edges contained in the circuit
	for(long i= 0; i<(long)Circuit.size()-1; i++){
		if(Circuit[i]>=0 && Circuit[i+1] >= 0){
			long iEdge= DDBB->GetEdgeByVerticesNums(Circuit[i], Circuit[i+1]);
			if(iEdge >= 0){
				DDBB->RemoveEdge(iEdge, false);

				//Renumber circuit
				for(long j= 0; j<(long)Circuit.size(); j++){
					if(Circuit[j]<-iEdge-1)
						Circuit[j]++;
				}
			}
		}
	}

	//Sort the circuit of axes and vertices to remove from last to first
	std::vector<long> SortedCircuit= Circuit;
	SortedCircuit.erase(SortedCircuit.begin()+SortedCircuit.size()-1); //Remove repeated item
	SortedCircuit= SortMinorFirst(SortedCircuit);

	//Split edges
	for(long i= 0; i<(long)SortedCircuit.size(); i++){
		if(SortedCircuit[i]<0)
			SplitEdge(DDBB, -SortedCircuit[i]-1);
		else
			break;
	}

	//Split vertices and update edges
	for(long i= (long)SortedCircuit.size()-1; i>=0; i--){
		if(SortedCircuit[i]>=0)
			SplitVertex(DDBB, SortedCircuit[i]);
		else
			break;
	}
}
