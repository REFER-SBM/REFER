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
#include <cassert>
#include <stack>

#include <vector>

#include "Tools_Vector.h"
#include "Tools_Geometry.h"
#include "DB_Model.h"

#include "CueFacesCB.h"


void CFacesCB::GetCycleMatrix(CDB_Model* DDBB,
							  std::vector<std::vector<long>> &CycleMatrix)
//Calculates the cycle-edge incidence matrix (or cycle matrix)

// CMij=1 if edge j is in cycle i, otherwise is 0
{
	assert(CycleMatrix.size() == DDBB->GetSizeFaces());
	if(DDBB->GetSizeFaces()>0) assert(CycleMatrix[0].size() == DDBB->GetSizeEdges());

	// Loop over the faces
	for (long i=0; i<(long)CycleMatrix.size(); i++){
		// Loop over the edges
		for (long j=0; j<(long)CycleMatrix[0].size(); j++){
			if (DDBB->IsEdgeInFace(i, j))
				CycleMatrix[i][j]=1;  //Cycle i contains edge j
			else
				CycleMatrix[i][j]=0;
		}
	}
}


void CFacesCB::GetInteractionMatrix(std::vector<std::vector<long>> &CycleMatrix,
									std::vector<std::vector<long>> &InteractionMatrix)
//Calculate the interaction matrix: Zij= number of edges shared between faces i and j
//                                  Zii= number of edges in face i

//The method to obtain the element Zij is multiplying rows i and j of the cycle matrix
{
	for(long i=0; i<(long)InteractionMatrix.size(); i++){
		for(long j=0; j<(long)InteractionMatrix[i].size(); j++){
			InteractionMatrix[i][j] =0;
			for(long k=0; k<(long)CycleMatrix[i].size(); k++){
				InteractionMatrix[i][j] += CycleMatrix[i][k] * CycleMatrix[j][k];
			}
		}
	}
}


bool CFacesCB::OrderCircuit(CDB_Model* pDDBB, std::vector<long> &Circuit)
//Reordering guarantees that the circuit keeps the direction of the resulting face
{
	std::vector<long> OrderedCircuit;

	long currentEdge= Circuit[0];
	OrderedCircuit.push_back(currentEdge);
	CEdge edge0= pDDBB->GetEdge(currentEdge);
	//long lastVertex= edge0.Tail;
	long currentVertex= edge0.Head;

	std::vector<bool> VisitedEdge(pDDBB->GetSizeEdges(),false);
	VisitedEdge[currentEdge]= true;
	long numVisitedEdges= 1;

	bool progress= true;
	while(progress){
		progress= false;
		for (long i=0; i<(long)Circuit.size(); i++){
			if(!VisitedEdge[Circuit[i]]){
				CEdge edgei= pDDBB->GetEdge(Circuit[i]);
				if(edgei.Head==currentVertex){
					currentEdge= Circuit[i];
					VisitedEdge[currentEdge]= true;
					numVisitedEdges++;
					OrderedCircuit.push_back(currentEdge);
					currentVertex= edgei.Tail;
					progress= true;
					break;
				}
				else if(edgei.Tail==currentVertex){
					currentEdge= Circuit[i];
					VisitedEdge[currentEdge]= true;
					numVisitedEdges++;
					OrderedCircuit.push_back(currentEdge);
					currentVertex= edgei.Head;
					progress= true;
					break;
				}
			}
		}
	}

	if(numVisitedEdges == (long)Circuit.size()){
		Circuit= OrderedCircuit;
		return true;
	}
	else{
		//Failure when traveling the circuit
		return false;
	}
}


void CFacesCB::GetSymmetricDifference(std::vector<long> &Circuit1,
									  std::vector<long> &Circuit2,
									  std::vector<long> &SymmetricDifference)
//Get the symmetric difference, which is the candidate face resulting from
//the union of the fundamental circuits Circuit1 and Circuit2 minus their intersection

//WARNING: Edges are NOT saved in the candidate face SymmetricDifference in ordered sequence!
{
	//Find the union and the intersection
	std::vector<long> Union;
	std::vector<long> Interseccion;
	Union= Circuit2;

	for(long i=0; i<(long)Circuit1.size(); i++){
	//for (long i=(long)Circuit1.size()-1; i>=0; i--){
		long position;
		if (!IsValueInVector(Circuit1[i], Union, position))
			Union.push_back(Circuit1[i]);
		
		if (IsValueInVector(Circuit1[i], Circuit2, position))
			Interseccion.push_back(Circuit1[i]);
	}

	//Substract the intersection from the union
	long FinalSize= (long)(Union.size()-Interseccion.size());
	for(long i=0; i<(long)Interseccion.size(); i++){
		long position;
		if(IsValueInVector(Interseccion[i], Union, position))
			Union.erase(Union.begin()+position);
	}

	//Return
	if (Union.size() == FinalSize)
		SymmetricDifference= Union;
	else
		abort();
}


void CFacesCB::VisitedCircuits(CDB_Model* DDBB,
							   std::vector<bool> &VisitedVertices,
							   std::vector<bool> &VisitedEdges)
//Searches in the faces of DDBB
//to provide a list of previously VisitedEdges,
//as much as the vertices only linked to them (VisitedVertices)

//It is assumed that each visited edge must be shared by exactly (or, at least) two faces
{
	//Calculate the Cycle Matrix
	std::vector<std::vector<long>> CycleMatrix (DDBB->GetSizeFaces(),std::vector<long>(DDBB->GetSizeEdges()));
	GetCycleMatrix(DDBB, CycleMatrix);

	//List of edges already explored
	VisitedEdges.assign(DDBB->GetSizeEdges(), false);

	for (long i=0; i<DDBB->GetSizeEdges(); i++){
			long Sum=0;
			for (long j=0; j<DDBB->GetSizeFaces(); j++){
				Sum += CycleMatrix[j][i];
			}
			if (Sum > 1)  //At least more than one edge
				VisitedEdges[i]= true;
	}

	//List of vertices already explored
	VisitedVertices.assign(DDBB->GetSizeVertices(), false);

	for (long i=0; i<DDBB->GetSizeVertices(); i++){
		bool found= false;
		for(long j=0; j<DDBB->GetSizeEdgesInVertex(i); j++){
			if (!VisitedEdges[DDBB->GetEdgeInVertex(i, j)]){
				found= true;
				break;
			}
			j++;
		}
		if (!found)
			VisitedVertices[i]= true;
	}
}


void CFacesCB::GetFalseFaces(CDB_Model* DDBB,
							 std::vector<long> &ListBadFaces,
							 bool GreaterFirst)
// Tentative faces are considered false if they share at least one edge
// with more than one other face
{
	long num_faces= DDBB->GetSizeFaces();
	long num_edges= DDBB->GetSizeEdges();

	std::vector<std::vector<long>> CycleMatrix(num_faces, std::vector<long>(num_edges));
	GetCycleMatrix(DDBB, CycleMatrix);

	//Detect faces with edges connected to more than two faces
	std::vector<bool> BadFaces(num_faces, false);
	for (long j=0; j<num_edges; j++){
		long Sum=0;
		for (long i=0; i<num_faces; i++){
			Sum+= CycleMatrix[i][j];
		}
		if (Sum > 2){
			for (long i=0; i<num_faces; i++){
				if(CycleMatrix[i][j] > 0)
					BadFaces[i]= true;
			}
		}
	}

	//Bad faces are ordered in ascending order
	std::vector<long> ListSize;
	for (long i=0; i<num_faces; i++){
		if(BadFaces[i]){
			ListBadFaces.push_back(i);

			if(GreaterFirst){
				CFace Cara= DDBB->GetFace(i);
				ListSize.push_back(Cara.GetSizeEdgesInFace());
			}
		}
	}

	//Bad faces are reordered from greater to smaller
	if(GreaterFirst){
		SortMajorFirst(ListSize, ListBadFaces);   //#include "Tools_Vector.h"
	}
}


void CFacesCB::GetFundamentalCycles(CDB_Model *DDBB,
									std::vector<bool> VisitedVertices,
									std::vector<bool> VisitedEdges)
// Finds a Fundamental Set of Cycles
// of the graph representation of the line drawing stored in DDBB
// and saves them as candidate faces in the database

// Fundamental circuits are obtained from the spanning tree
// created by way of a Breath-First Search

// This is our implementation of the Paton approach
//    Paton K.
//    An Algorithm for Finding a Fundamental Set of Cycles of a Graph.
//    Communications of the ACM, 12(9), 514-518, 1969.

// Edges in visitedEdges are excluded from the search,
// as much as vertices in VisitedVertices,
// thus allowing for incremental searches of fundamental circuits
//(This implies searching in unconnected graphs!)

//As implemented in:
//	Ana Piquer Vicent.
//	PERCEPCIÓN ARTIFICIAL DE DIBUJOS LINEALES.
//	PhD Thesis, 2003.
//	Departamento de Tecnología.
//	Universitat Jaume I.
//	http://www.tdx.cat/handle/10803/10554;jsessionid=A882045F99097C47E8CBE8B5D67E6FC3.tdx2

//Fundamental circuits are a set of linearly independent circuits 
//that are obtained from the spanning tree

//The spanning tree is a subgraph in which 
//all the vertices of the original graph are maintained, 
//but only includes those edges that hold all the vertices of the graph together, 
//without forming any closed circuit

//The edges of the graph that do not belong to the spanning tree are called chords

//Each time a chord is added to the spanning tree, an independent circuit is formed

//In the incidence matrix, each row and each column represents a vertex 
//and the content of each of its elements xij 
//will be the unit when a link that connect vertices i and j exists in the graph
{
	long num_edges= DDBB->GetSizeEdges();
	long num_vertices= DDBB->GetSizeVertices();

	//Mark all the edges as not yet included in the graph
	std::vector<bool> NotYetIncludedEdges(num_edges, true);

	//Exclude previously visited edges
	for (long i=0; i<(long)VisitedEdges.size(); i++){
		if(VisitedEdges[i])
			NotYetIncludedEdges[i]= false;
	}

	//Mark all the vertices as not yet examined
	std::vector<bool> NoTreeVertices(num_vertices, true);

	//Exclude previously visited vertices
	for (long i=0; i<(long)VisitedVertices.size(); i++){
		if(VisitedVertices[i])
			NoTreeVertices[i]= false;
	}

	//Mark all vertices as not belonging to the tree
	std::vector<std::vector<long>> TreeVertices (2,std::vector<long>(num_vertices));
	for (long j=0; j<num_vertices; j++){
		TreeVertices[0][j]=0;
		TreeVertices[1][j]=-1; //Assign them a null father (-1)
	}

	//Loop to visit all the trees of the graph
	//WARNING: this is required to prevent some edges from remaining unvisited in disconnected graphs!!
	bool newTree= true;
	while(newTree){
		newTree= false;

		//Take the first non-visited vertex as the root of the new tree
		//(This is an arbitrary choice, and can be changed without loss of generality,
		//but repetibility of the algorithm may be affected)
		bool done=true;
		for(long j=0; j<(long)NoTreeVertices.size(); j++){
			if(NoTreeVertices[j]){
				TreeVertices[0][j]=1;
				TreeVertices[1][j]=-1; //Assign it a null father (-1)
				newTree= true;
				done= false;
				break;
			}
		}

		//Loop to examine the set of vertices and add them to the graph
		while (!done){
			//Find the next vertex to examine(z)
			long z= -1;
			for(long i=0; i<num_vertices; i++){    // Produces faces with less vertices (prone to be true faces)
			//for(long i=num_vertices-1; i>-1; i--){  // Produces faces with more vertices (prone to be false faces)
				if (TreeVertices[0][i]==1 && NoTreeVertices[i]){
					z= i;
					break;
				}
			}
			//Finish if all the edges have already been examined
			if (z == -1){
				done=true;
				break;
			}

			//Add to the graph all the edges connected to the vertex z
			//and not yet included in the tree
			//(Until a fundamental cycle is found)
			for(long j=0; j<DDBB->GetSizeEdgesInVertex(z); j++){
				long lEdgej= DDBB->GetEdgeInVertex(z, j);
				if(NotYetIncludedEdges[lEdgej]){
					NotYetIncludedEdges[lEdgej]= false;

					//Visit a new edge linked to the current vertex Z
					long w;
					CEdge Edgej= DDBB->GetEdge(lEdgej);
					if (Edgej.Head==z)
						w= Edgej.Tail;
					else
						w= Edgej.Head;

					//Add the edge z-w to the graph
					//or close a fundamental cycle
					if (TreeVertices[0][w]==0){
						//Add the new vertex w to the tree
						TreeVertices[0][w]= 1;
						TreeVertices[1][w]= z;
					}
					else{
						//This edge yields a fundamental cycle!

						//Find the two chains of the cycle
						//going backwards to the root from the two end vertices
						std::vector<long> Chain1;
						long zz= z;
						while (zz!=-1){
							Chain1.push_back (zz);
							zz= TreeVertices[1][zz];
						}
						std::vector<long> Chain2;
						while (w!=-1){
							Chain2.push_back (w);
							w= TreeVertices[1][w];
						}
						//Find the overlapped part of both chains
						long position1= -1;
						long position2= -1;
						for (long ll=0; ll<(long)Chain1.size(); ll++){
							if(IsValueInVector(Chain1[ll], Chain2, position2)){
								position1=ll;
								break;
							}
						}
						assert(position2!=-1); //Both chains should share at least the root vertex

						//Mix the two chains in one single path
						std::vector<long> Chain;
						for(long l=0; l<position1; l++){
							Chain.push_back (Chain1[l]);
						}
						for(long l=position2; l>-1; l--){
							Chain.push_back (Chain2[l]);
						}

						//Save the tentative face stored in the path "Chain"
						CFace newFace;
						for (long ll=0; ll<(long)Chain.size(); ll++){
							long Index1= Chain[ll];
							long Index2= Chain[(ll+1)%(Chain.size())];

							for(long m=0; m<DDBB->GetSizeEdgesInVertex(Index1); m++){
								long lEdgem= DDBB->GetEdgeInVertex(Index1, m);
								CEdge Edgem= DDBB->GetEdge(lEdgem);
								if((Edgem.Head==Index2)||(Edgem.Tail==Index2)){
									newFace.AddEdgeInFace(lEdgem);
									break;
								}
							}
						}
						DDBB->AddFace(newFace);
					}
				}
			}
			//Mark the current vertex as already examined
			NoTreeVertices[z]= false;
		}
	}
}


void CFacesCB::ReduceCyclesToFaces(CDB_Model* pDDBB)
//Changing cycles into faces: the reduction process

//This is accomplished by making use of 
//a set operation called the symmetric difference

//Pairs of fundamental circuits are combined
//and its Symmetric Difference is obtained

//The Symmetric Difference is defined as 
//“the result of the union of the sets of edges of two given circuits minus their intersection”
//In other words, the symmetric difference is the circuit 
//formed by the union of the two given graphs, minus their intersection

//The circuit with more adges is combined 
//with the circuit that shares more edges with it

//The proccess is repeated until no more circuits may be reduced
{
	long num_faces= pDDBB->GetSizeFaces();  //The number of faces is constant while reducing cycles to faces (faces are neither added nor deleted)
	std::vector<std::vector<long>> CycleMatrix (num_faces, std::vector<long>(pDDBB->GetSizeEdges()));
	std::vector<std::vector<long>> InteractionMatrix (num_faces, std::vector<long>(num_faces));

	//This matrix is used to check if a pair of faces has already been evaluated (=1) or not (=0)
	std::vector<std::vector<bool>> CheckMatrix(num_faces,std::vector<bool>(num_faces, false));

	std::vector<long> FinalList;

	bool RemainingCycles= true;
	while (RemainingCycles==true){
		GetCycleMatrix(pDDBB, CycleMatrix);

		GetInteractionMatrix(CycleMatrix, InteractionMatrix);

		//Check if reductions are still feasible
		for(long k= 0; k<(long)InteractionMatrix.size(); k++){
			for (long m= k+1; m<(long)InteractionMatrix[k].size(); m++){
				if (InteractionMatrix[k][m]>1){
					goto follow;
				}
			}
		}
		RemainingCycles= false;
		break;
		follow:

		if((long)FinalList.size() >= num_faces){
			RemainingCycles= false;
			break;
		}

		//Get the greatest cycle (the one with more edges)
		long icycle=0;
		long major1=0;
		for(long k=0; k<(long)InteractionMatrix.size(); k++){
			if ((InteractionMatrix[k][k] > major1) &&
				(!IsValueInVector(k, FinalList))){
				icycle= k;
				major1= InteractionMatrix[k][k];
			}
		}

		CFace CaraI= pDDBB->GetFace(icycle);
		std::vector<long> ICircuit= CaraI.GetAllEdgesInFace();

		std::vector<long> ColumnsVisited;
		bool reduce=false;
		while(!reduce){
			//Find the cycle that shares more edges with the greatest cycle
			long jcycle=-1;
			long major2= 0;
			for (long k=0; k<num_faces; k++){
				if((InteractionMatrix[icycle][k]> major2)&&
					(icycle!=k)&&
					!IsValueInVector(k, ColumnsVisited)&&
					!CheckMatrix[icycle][k]){
					jcycle=k;
					major2=InteractionMatrix[icycle][k];
				}
			}

			//Finish if no cycles share edges
			if (jcycle == -1){
				FinalList.push_back(icycle);
				reduce=true;
				break;
			}

			CFace CaraJ= pDDBB->GetFace(jcycle);
			std::vector<long> JCircuit= CaraJ.GetAllEdgesInFace();

			//Get symmetric difference between greatest (icycle) and current (jcycle) cycles
			std::vector<long> SymmetricDifference;
			GetSymmetricDifference(ICircuit, JCircuit, SymmetricDifference);

			CheckMatrix[icycle][jcycle]= true;
			long test= InteractionMatrix[jcycle][jcycle]-(2*InteractionMatrix[icycle][jcycle]);
			if (test < 0){
				//Reducing is advantageous
				OrderCircuit(pDDBB, SymmetricDifference); //Warning: Edges must be saved in ordered sequence!!
				//CFace CaraI= pDDBB->GetFace(icycle);
				CaraI.SetAllEdgesInFace(SymmetricDifference);
				pDDBB->SetFace(icycle, CaraI);
				reduce=true;
			}
			else if (test > 0){
				//Current face is better
				ColumnsVisited.push_back(jcycle);
				reduce=false;
			}
			else{
				//Compare the current interaction matrix
				//with the one resulting from accepting the reduction

				//Size of the current
				long Sum1= 0;
				for (long k=0; k<num_faces; k++){
					Sum1+=InteractionMatrix[icycle][k];
				}

				//Size of the reduced
				OrderCircuit(pDDBB, SymmetricDifference); //Warning: Edges must be saved in ordered sequence!!
				CDB_Model DDBBAux= *pDDBB;
				CFace CaraII= DDBBAux.GetFace(icycle);
				CaraII.SetAllEdgesInFace(SymmetricDifference);
				DDBBAux.SetFace(icycle, CaraII);
				GetCycleMatrix(&DDBBAux, CycleMatrix);
				std::vector<std::vector<long>> InteractionMatrix2 (num_faces,std::vector<long>(num_faces));
				GetInteractionMatrix(CycleMatrix, InteractionMatrix2);

				long Sum2= 0;
				for (long k=0; k<num_faces; k++){
					Sum2+=InteractionMatrix2[icycle][k];
				}

				//Comparison
				if (Sum2 > Sum1){
					ColumnsVisited.push_back(jcycle); 
					reduce=false;
				}
				else{
					CFace CaraIII= pDDBB->GetFace(icycle);
					CaraIII.SetAllEdgesInFace(SymmetricDifference);
					pDDBB->SetFace(icycle, CaraIII);
					reduce=true;
				}
			}

			if((long)ColumnsVisited.size() >= num_faces){
				RemainingCycles= true;
				FinalList.push_back(icycle);
				break;
			}
		}
	}
}


void CFacesCB::PostProcessFaces(CDB_Model* DDBB)
// The post-processing of anomalies consists of 
// detecting and deleting unlikely faces

// Finding unlikely faces is based on the following heuristic:
//   Tentative faces are considered false if they share at least one edge
//   with more than another face

// After removing false faces, the face detection process is repeated
// on the subgraph resulting from considering 
// those edges of the original graph 
// that still belong to less than two faces

// Occasionally this produces new false faces!!

// ALTERNATIVE: Only the larger false face is removed,
// and the remaining false faces are recalculated
{
	//Search for bad faces requiring postprocessing
	std::vector<long> ListBadFaces;
	//GetFalseFaces(DDBB, ListBadFaces, false);  //False faces are listed in ascending order
	GetFalseFaces(DDBB, ListBadFaces, true);  //ALTERNATIVE: Larger false face is listed first

	if (ListBadFaces.size() == 0){
		return;
	}

	//Postprocess bad faces
	//////----------------------------------
	//Erase all bad faces (in descending order to prevent renumbering failures)
	//for(long i=(long)ListBadFaces.size()-1; i>=0 ; i--){
	//	DDBB->RemoveFace(ListBadFaces[i]);
	//}

	//----------------------------------
	//ALTERNATIVE: Recalculate remaining bad faces after removing only the larger one
	while (ListBadFaces.size() > 0){
		// Larger false face is removed first
		DDBB->RemoveFace(ListBadFaces[0]);

		ListBadFaces.clear();
		GetFalseFaces(DDBB, ListBadFaces, true);
	}
	//----------------------------------

	//Search for missing faces in an auxiliary database 
	//which contains the not fully visited edges and vertices
	std::vector<bool> VisitedEdges;
	std::vector<bool> VisitedVertices;
	VisitedCircuits(DDBB, VisitedVertices, VisitedEdges);  //Visited faces are excluded in the new search

	CDB_Model DDBBAux= *DDBB;
	DDBBAux.ClearFaces();

	GetFundamentalCycles(&DDBBAux, VisitedVertices, VisitedEdges);
	ReduceCyclesToFaces(&DDBBAux);

	//Add the missing faces to the main database
	for(long i=0; i<DDBBAux.GetSizeFaces(); i++){
		if(DDBB->IsFace(DDBBAux.GetFace(i)) == -1){
			DDBB->AddFace(DDBBAux.GetFace(i));
		}
		else{
			//Face already included
		}
	}

	//Delete empty faces
	for(long i=(DDBB->GetSizeFaces()-1); i>=0 ; i--){
		CFace Cara= DDBB->GetFace(i);
		if (Cara.GetSizeEdgesInFace() < 3)
			DDBB->RemoveFace(i);
	}
}


void CFacesCB::AddLastFace(CDB_Model* DDBB)
//Search for the last face
//(Only if model is not already closed)

//WARNING: The procedure may result in an unclosed circuit
//         for graphs of non two-manifold polihedra!!
{
	std::vector<std::vector<long>> CycleMatrix (DDBB->GetSizeFaces(),std::vector<long>(DDBB->GetSizeEdges()));
	GetCycleMatrix(DDBB, CycleMatrix);

	std::vector<long> circuitFace;
	bool addLastFace= false;
	for (long j=0; j<DDBB->GetSizeEdges(); j++){
		long Sum=0;
		for (long i=0; i<DDBB->GetSizeFaces(); i++){
			Sum += CycleMatrix[i][j];
		}

		if (Sum == 1){
			circuitFace.push_back(j);
			addLastFace=true;
		}
	}

	if(addLastFace && circuitFace.size()>2){
		if(OrderCircuit(DDBB, circuitFace)){ //Warning: Edges must be saved in ordered sequence!!
			CFace LastFace;
			LastFace.SetAllEdgesInFace(circuitFace);
			DDBB->AddFace(LastFace);
		}
	}
}


bool CFacesCB::Get_Faces_CB(CDB_Model *DDBB)
// Detect faces in wireframe 2D line-drawings of polyhedral shapes
// This is our implementation of the Courter and Brewer approach

//Based on:
//    Courter S.M., Brewer J.A.
//    Automated conversion of curvilinear wire-frame models
//    to surface boundary models; a topological approach.
//    Int. Conf. on Computer Graphics and Interactive Techniques. pp.171-178. 1986.

//As implemented in:
//	Ana Piquer Vicent.
//	PERCEPCIÓN ARTIFICIAL DE DIBUJOS LINEALES.
//	PhD Thesis, 2003.
//	Departamento de Tecnología.
//	Universitat Jaume I.
//	http://www.tdx.cat/handle/10803/10554;jsessionid=A882045F99097C47E8CBE8B5D67E6FC3.tdx2

//The approach is an exclusively topological method.
//It is based on the determination of fundamental circuits and their combination
//following certain criteria to select those potential faces obtained by
//combination of fundamental circuits that are most likely to be faces of the object

//The spanning tree is a subgraph in which 
//all the vertices of the original graph are maintained, 
//but only includes those edges that hold all the vertices of the graph together, 
//without forming any closed circuit

//The edges of the graph that do not belong to the spanning tree are called chords
//Each time a chord is added to the spanning tree, an independent circuit is formed

//Once the fundamental circuits have been obtained, 
//a method is proposed to convert these circuits in real faces.

//To decide which pairs of cycles should be combined, 
//two basic criteria are used taken from Graph Theory:
//	No edge can belong to more than two fundamental circuits
//	The sum of the number of all edges in all circuits must be minimum

//Courter and Brewer's algorithm includes post-processing of certain anomalies.
//The postprocess consists on detecting and deleting invalid faces, 
//and then repeating the face detection process 
//on the subgraph resulting from considering those edges
//of the original graph that still belong to less than two faces.
{
	//Find a Fundamental Set of Cycles of the Graph
	std::vector<bool> VisitedVertices;  //Void, to search in the whole graph
	std::vector<bool> VisitedEdges;     //Void, to search in the whole graph
	GetFundamentalCycles(DDBB, VisitedVertices, VisitedEdges);

	//Change cycles into faces: the reduction process
	ReduceCyclesToFaces(DDBB);

	//Remove unlikely faces and search for missing faces
	PostProcessFaces(DDBB);

	//Add last face (if missing)
	AddLastFace(DDBB);

	return true;
}
