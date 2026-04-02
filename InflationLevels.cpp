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
#include "DB_CSG.h"
#include "ReferDoc.h"
#include "Refer.h"
#include "resource.h"

#include "CueLevels.h"

#include "InflationLevels.h"


long CInflationLevels::ConnectGraphs(CDB_Model *DDBB)
//Multigraphs are linked together, adding artificial edges, to get a single graph

//The number of edges added is returned
//Added edges are inserted at the end of the list
{
	long AddedEdges=0;
	long Dato;

	//Loop until all vertices belong to the same graph
	for(;;){
		//Add the root vertex
		std::vector<long> SpanningTree;
		SpanningTree.push_back(0);

		//Get the spannig tree of vertices connected to the root vertex
		for(long i=0; i<(long)SpanningTree.size(); i++){
			for (long j=0; j<DDBB->GetSizeEdges(); j++){
				CEdge Arista= DDBB->GetEdge(j);
				if (Arista.Head==SpanningTree[i] && 
					!IsValueInVector(Arista.Tail, SpanningTree, Dato))
					SpanningTree.push_back(Arista.Tail);
				else{
					if (Arista.Tail==SpanningTree[i] && 
						!IsValueInVector(Arista.Head, SpanningTree, Dato))
						SpanningTree.push_back(Arista.Head);
				}
			}
		}

		//If there are less vertices in the spannig tree than in the model,
		//then the model is multigraph
		if ((long)SpanningTree.size() < DDBB->GetSizeVertices()){
			//Search for the first vertex not yet contained in the spanning tree
			long NotInSpanningTree=0;
			do{
				NotInSpanningTree++;
			}while(IsValueInVector(NotInSpanningTree, SpanningTree, Dato));

			CEdge Arista;
			Arista.Head=0;
			Arista.Tail=NotInSpanningTree;
			DDBB->AddEdge(Arista);
			AddedEdges++;
		}
		else
			break;
	}
	return AddedEdges;
}


long CInflationLevels::GetCentralVertex(CDB_Model *DDBB)
//Returns the vertex of the input drawing closer to the centre
//(Vertices with -1 level are not considered)

//There was a MISTAKE in previous versions:
//Only vertices of level 0 were considered!
{
	double dXmin, dYmin, dZmin, dXmax, dYmax, dZmax;
	DDBB->GetBoundingBox(dXmin, dYmin, dZmin, dXmax, dYmax, dZmax);
	double Xcentre=(dXmin+dXmax)/2;
	double Ycentre=(dYmin+dYmax)/2;

	long i=0;
	while (ReferApp.DIL.LevelOfVertex[i]==-1 && i<(long)ReferApp.DIL.LevelOfVertex.size()) i++;
	CVertex Vertice= DDBB->GetVertex(i);
	double MinDist=sqrt((Vertice.x-Xcentre)*(Vertice.x-Xcentre)+(Vertice.y-Ycentre)*(Vertice.y-Ycentre));
	long CentralVertex= i;

	for (long j=i+1; j<DDBB->GetSizeVertices(); j++){
		if (ReferApp.DIL.LevelOfVertex[j] != -1){
			CVertex VerticeJ= DDBB->GetVertex(j);
			double Dist=sqrt((VerticeJ.x-Xcentre)*(VerticeJ.x-Xcentre)+(VerticeJ.y-Ycentre)*(VerticeJ.y-Ycentre));
			if (MinDist>Dist){
				MinDist= Dist;
				CentralVertex=j;
			}
		}
	}
	return CentralVertex;
}


void CInflationLevels::GetSons(CDB_Model *DDBB, long father, std::vector<long> &Sons)
//Search all "sons" (vertices connected to vertex "father")
{
	for (long v= 0; v <(long)DDBB->GetSizeEdgesInVertex(father); v++){
		CEdge Arista= DDBB->GetEdge(DDBB->GetEdgeInVertex(father, v));
		if(Arista.Head == father)
			Sons.push_back(Arista.Tail);
		else
			Sons.push_back(Arista.Head);
	}
}


void CInflationLevels::SortSons(CDB_Model *DDBB, long father,
								std::vector<long> &Sons,
								std::vector<double> &DistanceToFather)
//Sorts the "Sons" (vertices connected to the father vertex), by decreasing distance

//Also returns the distances to the father
{
	//Calculate distances
	for (long i= 0; i<(long)Sons.size(); i++){
		DistanceToFather.push_back(DDBB->GetDistance2D(father, Sons[i]));
	}

	//Insertion sort algorithm to order from longest to shortest
	for (long i= 1; i<(long)Sons.size(); i++){
		double dist = DistanceToFather[i];
		long index = Sons[i];
		long a;
		for (a= i-1; a >= 0 && DistanceToFather[a] < dist; a--){ // Long distances first
		//for (a= i-1; a >= 0 && DistanceToFather[a] > dist; a--){ // Short distances first
			DistanceToFather[a+1]= DistanceToFather[a];
			Sons[a+1]= Sons[a];
		}
		DistanceToFather[a+1]= dist;
		Sons[a+1]= index;
	}
}


void CInflationLevels::DFS_VisitSons(long father, std::vector<bool> &visited,
									 CDB_Model *DDBB, double Delta,
									 std::vector<double> &VariablesZ)
//Deep First Search for recursively assigning incremental z coordinates to "son" vertices:
//		connected to vertex "father"
//		not yet explored

//Son vertices are ordered to be visited by decreasing distance to their father
{
	visited[father]= true;

	//Search direct "sons" (vertices connected to vertex "father")
	std::vector<long> Sons;
	GetSons(DDBB, father, Sons);

	//Sort son vertices by decreasing distance to their father
	std::vector<double> DistanceToFather;
	SortSons(DDBB, father, Sons, DistanceToFather);

	//Explore all son vertices not yet explored
	for (long v= 0; v<(long)Sons.size(); v++){
		if (!visited[Sons[v]]){
			//Get z coordinate of current son
			double Step= (ReferApp.DIL.LevelOfVertex[Sons[v]] - ReferApp.DIL.LevelOfVertex[father]) * Delta;
			Step= Step * DistanceToFather[v];
			if (Step < 0)
				VariablesZ[Sons[v]]= VariablesZ[father] + Step*ReferApp.DIL.Coef_TreeDown;
			else
				VariablesZ[Sons[v]]= VariablesZ[father] + Step*ReferApp.DIL.Coef_TreeUp;

			//Save information to display the tree
			ReferApp.DIL.LevelsTreeVertices.push_back(father);
			ReferApp.DIL.LevelsTreeVertices.push_back(Sons[v]);

			//RECURSIVELY explore its own sons
			DFS_VisitSons(Sons[v], visited, DDBB, Delta, VariablesZ);
		}
	}
}


void CInflationLevels::BFS_VisitVertices(long first,
										 CDB_Model *DDBB,
										 double Delta,
										 std::vector<double> &VariablesZ)
//Incrementally assign Z coordinates

//Breadth First Search is applied to assign z coordinates to all son vertices
//by incrementing or decrementing the Z coordinate of its father

//Son vertices are ordered to be visited by decreasing distance to their father
{
	//Clear information to display the tree
	ReferApp.DIL.LevelsTreeVertices.clear();

	//Initialize local data
	std::queue<long> Q;

	std::vector<bool> visited(DDBB->GetSizeVertices(), false);
	std::vector<long> father(DDBB->GetSizeVertices(), -1);
	std::vector<double> DistanceToFather(DDBB->GetSizeVertices(), 0);

	//Insert the first vertex in the queu
	Q.push(first);
	visited[first] = true;
	father[first]= first;
	DistanceToFather[first]= 0;

	//Arbitrarily put the first vertex in the origin
	VariablesZ[first]= 0.;

	//Loop to calculate the incremental Z coordinate of the current vertex
	//and insert their son vertices in the queue
	while(!Q.empty()){
		//Extract the front vertex from the queue
		long v = Q.front();
		Q.pop();

		//Calculate its incremental Z coordinate
		double Step= (ReferApp.DIL.LevelOfVertex[v] - ReferApp.DIL.LevelOfVertex[father[v]]) * Delta;
		Step= Step * DistanceToFather[v];
		if (Step < 0)
			VariablesZ[v]= VariablesZ[father[v]] + Step*ReferApp.DIL.Coef_TreeDown;
		else
			VariablesZ[v]= VariablesZ[father[v]] + Step*ReferApp.DIL.Coef_TreeUp;

		//Save information to display the tree
		ReferApp.DIL.LevelsTreeVertices.push_back(father[v]);
		ReferApp.DIL.LevelsTreeVertices.push_back(v);

		//Search all direct "sons" (vertices connected to vertex "v")
		std::vector<long> Sons;
		GetSons(DDBB, v, Sons);

		//Sort son vertices by decreasing distance to their father
		std::vector<double> DistanceToV;
		SortSons(DDBB, v, Sons, DistanceToV);

		//Insert son vertices in the queue
		for(long u= 0; u<(long)Sons.size(); u++){
			if(!visited[Sons[u]]){
				Q.push(Sons[u]);
				visited[Sons[u]] = true;
				father[Sons[u]] = v;
				DistanceToFather[Sons[u]]= DistanceToV[u];
			}
		}
	}
}


void CInflationLevels::GetIncrementalLevels(CDB_Model *DDBB,
											std::vector<double> &VariablesZ)
//Get Z values if incremental coordinates have been requested

//Vertices are visites following a spanning tree
//The sequence may be "deep first" or "breath first"
//Each Z value is obtained increasing or decreasing from parent Z value

//Multigraphs are converted into single graphs, by adding fictitious edges,
//which are removed befor returning
{
	////////////////  MISTAKE! in both the Julian's thesis and the 2004 paper /////////////
	//double Delta= Range/5.;
	//if(ReferApp.GeneralParams.DetectHiddenEdges) Delta= Range/17.;
	///////////////////////////////////////////////////////////////////////////////////////
	double Delta= 1/5.;
	if(ReferApp.GeneralParams.DetectHiddenEdges) Delta= 1/17.;


	//Clear information to display the tree
	ReferApp.DIL.LevelsTreeVertices.clear();

	//Connect multigraphs to propagate the spanning tree
	long AddedEdges= ConnectGraphs(DDBB);

	//Get the vertex of the input drawing closer to the centre
	//(Vertices without level are not considered)
	long CentralVertex= GetCentralVertex(DDBB);

	//Assign coordinates Z, following the spanning tree
	if (ReferApp.DIL.SpanningTreeLevels==0){
		//Deep First Search
		//Assign coordinate Z=0 to the central vertex
		VariablesZ[CentralVertex]= 0;

		//Information to display the tree
		ReferApp.DIL.LevelsTreeVertices.push_back(CentralVertex);
		ReferApp.DIL.LevelsTreeVertices.push_back(CentralVertex);

		//Reset search flags
		std::vector<bool> visited(DDBB->GetSizeVertices(), false);

		//RECURSIVELY assign coordinate Z to the son vertices
		DFS_VisitSons(CentralVertex, visited, DDBB, Delta, VariablesZ);
	}
	else{
		//Breath first search
		BFS_VisitVertices(CentralVertex, DDBB, Delta, VariablesZ);
	}

	//Remove fictitious edges added by ConnectGraphs
	//(to restore the original multigraphs)
	while(AddedEdges != 0){
		DDBB->RemoveEdge(DDBB->GetSizeEdges()-1, false);
		AddedEdges--;
	};
}


void CInflationLevels::GetFixedLevels(CDB_Model *DDBB,
									  std::vector<double> &VariablesZ)
//Get Z values if fixed coordinates have been requested
{
	//Get range of Z coordinates
	double dXmin, dYmin, dZmin, dXmax, dYmax, dZmax;
	DDBB->GetBoundingBox(dXmin, dYmin, dZmin, dXmax, dYmax, dZmax);
	double Range= max(std::fabs(dXmax-dXmin), std::fabs(dYmax-dYmin));

	//Assign Z coordinates
	if(ReferApp.GeneralParams.DetectHiddenEdges){
		for (long i=0; i<DDBB->GetSizeVertices(); i++){
			VariablesZ[i]= Range * ReferApp.DIL.LevelsH[ReferApp.DIL.LevelOfVertex[i]];
		}
	}
	else{
		for (long i=0; i<DDBB->GetSizeVertices(); i++){
			VariablesZ[i]= Range * ReferApp.DIL.LevelsV[ReferApp.DIL.LevelOfVertex[i]];
		}
	}
}


void CInflationLevels::InflateByLevels(CDB_Model *DDBB)
// Assigns Z coordinates according to levels stored in "LevelOfVertex"

// Two approaches are implemented:
	// Pre-fixed coordinates

	// Incremental coordinates
		//We use a particular version of Kruskal algorithm,
		//departing from the most central level 0 vertex
		//(the one closer to the figure barycentre)
		//and following to the vertex connected with the actual
		//through the longest edge not yet visited
{
	std::vector<double> VariablesZ(DDBB->GetSizeVertices(), 0);

	//Get Z values
	//------------
	if (ReferApp.DIL.IncrementalLevels){
		//Get Z values if incremental coordinates have been requested
		GetIncrementalLevels(DDBB, VariablesZ);
	}
	else{
		//Get Z coordinates applying pre-fixed increments
		GetFixedLevels(DDBB, VariablesZ);
	}

	//Vertices connected to one single edge,
	//are given the same Z than the vertex to which they are connected
	//----------------------------------------------------------------
	for (long i=0; i<DDBB->GetSizeVertices(); i++){
		if (DDBB->GetSizeEdgesInVertex(i) == 1){
			CEdge Arista= DDBB->GetEdge(DDBB->GetEdgeInVertex(i, 0));
			if (Arista.Head==i){
				VariablesZ[Arista.Head]= VariablesZ[Arista.Tail];
			}
			else{
				VariablesZ[Arista.Tail]= VariablesZ[Arista.Head];
			}
		}
	}

	//Translate the resulting model
	//(Substracting the minimum Z to all the vertices)
	//------------------------------------------------
	double Zmin= VariablesZ[0];
	for (long i=1; i<(long)VariablesZ.size(); i++){
		Zmin= min(Zmin, VariablesZ[i]);
	}
	for (long i=0; i<(long)VariablesZ.size(); i++){
		VariablesZ[i] -= Zmin;
	}

	//Save Z coordinates into global database
	//---------------------------------------
	DDBB->SetAllVertex_Z(VariablesZ);
}
