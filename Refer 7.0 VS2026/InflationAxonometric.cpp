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
#include <cassert>
#include <vector>

#include "Tools_Vector.h"
#include "Tools_Geometry.h"

#include "DB_Model.h"
#include "DB_CSG.h"
#include "ReferDoc.h"
#include "Refer.h"
#include "resource.h"

#include "CueMainAxes.h"

#include "InflationAxonometric.h"


long CInflationAxonometric::GetCentralNormalonVertex(CDB_Model *DDBB)
//Returns the central vertex, according to the following conditions:
//		* Must connect at least 3 different normalon edges
//		* Must contain the longest normalon edge (among those that accomplish the previous criterion)
//		* Must be the closest to the centre of mass (among those that accomplish the previous criteria)

//Default is vertex 0
{
	//Calculates lengths of edges and orders them by decreasing length
	std::vector<long> LongestEdges;
	std::vector<double> Lengths;
	double Longitud0= DDBB->GetLength2D(0);
	LongestEdges.push_back(0);
	Lengths.push_back(Longitud0);

	for (long i=1; i<DDBB->GetSizeEdges(); i++){
		if(ReferApp.DIA.NormalonEdges[i] >= 0){
			double Longitud= DDBB->GetLength2D(i);
			bool IsSmaller= true;
			for (long j=0; j<(long)LongestEdges.size(); j++){
				if (Lengths[j]<Longitud){
					LongestEdges.insert(LongestEdges.begin()+j, i);
					Lengths.insert(Lengths.begin()+j, Longitud);
					IsSmaller= false;
					break;
				}
			}
			if (IsSmaller){
				LongestEdges.push_back(i);
				Lengths.push_back(Longitud);
			}
		}
		else{
			//Assigns length 0 to non-normalon edges
			LongestEdges.push_back(i);
			Lengths.push_back(0);
		}
	}

	//Get the centre of mass of the 2D model
	CVertex CentreOfMass;
	double dXmin, dYmin, dZmin, dXmax, dYmax, dZmax;
	DDBB->GetBoundingBox(dXmin, dYmin, dZmin, dXmax, dYmax, dZmax);
	CentreOfMass.x= dXmin+(dXmax-dXmin)/2;
	CentreOfMass.y= dYmin+(dYmax-dYmin)/2;

	//Loop over decreasing length groups of edges
	//until a valid central vertex is found
	long CentralVertex= 0;

	bool ValidVertex= false;
	long FirstEdgeInTheGroup= 0;
	double MinDistToCentre= -1;

	////////////////////////////////////////////////
	double thresholdLength= 0.1 * Lengths[0]; //10% longest edge
	////////////////////////////////////////////////

	do{
		//Number of remaining edges with the same length
		long NumEdgesEqual= 1;
		for(long i= FirstEdgeInTheGroup+1; i<(long)Lengths.size(); i++){
			if(std::fabs(Lengths[i]-Lengths[FirstEdgeInTheGroup]) < thresholdLength){
				NumEdgesEqual++;
			}
		}

		//Select the vertex connected to two or more normalon edges and closest to the centre of mass
		for (long i=FirstEdgeInTheGroup; i<FirstEdgeInTheGroup+NumEdgesEqual; i++){
			CEdge Arista= DDBB->GetEdge(LongestEdges[i]);

			//Get the number of different normalon edges connected to the head vertex
			long NumEdges= 0;
			bool NotX, NotY, NotZ;
			NotX= NotY= NotZ= true;
			for(long j= 0; j<DDBB->GetSizeEdgesInVertex(Arista.Head); j++){
				if(NotX && ReferApp.DIA.NormalonEdges[DDBB->GetEdgeInVertex(Arista.Head, j)] == 0){
					NumEdges++;
					NotX= false;
				}
				if(NotY && ReferApp.DIA.NormalonEdges[DDBB->GetEdgeInVertex(Arista.Head, j)] == 1){
					NumEdges++;
					NotY= false;
				}
				if(NotZ && ReferApp.DIA.NormalonEdges[DDBB->GetEdgeInVertex(Arista.Head, j)] == 2){
					NumEdges++;
					NotZ= false;
				}
			}

			if (NumEdges >= 2){
				CVertex HeadVertex= DDBB->GetVertex(Arista.Head);
				double Distance= MODULE_2D(HeadVertex, CentreOfMass);
				if (MinDistToCentre < 0){
					MinDistToCentre= Distance;
					CentralVertex= Arista.Head;
					ValidVertex = true;
				}
				else{
					if (Distance < MinDistToCentre){
						MinDistToCentre= Distance;
						CentralVertex= Arista.Head;
						ValidVertex = true;
					}
				}
			}

			//Get the number of normalon edges connected to the tail vertex
			NumEdges= 0;
			for(long j= 0; j<DDBB->GetSizeEdgesInVertex(Arista.Tail); j++){
				if(ReferApp.DIA.NormalonEdges[DDBB->GetEdgeInVertex(Arista.Tail, j)] >= 0) NumEdges++;
			}

			if (NumEdges >= 2){
				CVertex TailVertex= DDBB->GetVertex(Arista.Tail);
				double Distance= MODULE_2D(TailVertex, CentreOfMass);
				if (MinDistToCentre < 0){
					MinDistToCentre= Distance;
					CentralVertex= Arista.Tail;
					ValidVertex= true;
				}
				else{
					if (Distance < MinDistToCentre){
						MinDistToCentre= Distance;
						CentralVertex= Arista.Tail;
						ValidVertex= true;
					}
				}
			}
		}

		FirstEdgeInTheGroup+= NumEdgesEqual;

	}while(!ValidVertex && FirstEdgeInTheGroup<(long)Lengths.size());

	return CentralVertex;
}


void CInflationAxonometric::GetSortedEdges(CDB_Model *DDBB,
										   long CurrentVertex,
										   std::vector<long> &EdgesInVertex)
//Returns the list of normalon edges connected to the "CurrentVertex",
//sorted by decreasing length
{
	//Get all normalon edges connected to the current vertex
	for (long i=0; i<DDBB->GetSizeEdges(); i++){
		if (ReferApp.DIA.NormalonEdges[i] >= 0){
			CEdge Arista= DDBB->GetEdge(i);
			if (Arista.Head==CurrentVertex || Arista.Tail==CurrentVertex)
				EdgesInVertex.push_back(i);
		}
	}
	if (EdgesInVertex.size()<=2) return;

	//Sort them by decreasing length
	std::vector<long> Sorted;
	while (EdgesInVertex.size()>0){
		if (EdgesInVertex.size() == 1){
			Sorted.push_back(EdgesInVertex[0]);
			EdgesInVertex.erase(EdgesInVertex.begin()+0);
		}
		else{
			long PosicionMayorArista=0;
			CEdge Arista= DDBB->GetEdge(EdgesInVertex[0]);
			CVertex HeadVertex= DDBB->GetVertex(Arista.Head);
			CVertex TailVertex= DDBB->GetVertex(Arista.Tail);
			double Modulo1 = MODULE_2D(HeadVertex, TailVertex);
			for (long j=1; j<(long)EdgesInVertex.size(); j++){
				Arista= DDBB->GetEdge(EdgesInVertex[j]);
				HeadVertex= DDBB->GetVertex(Arista.Head);
				TailVertex= DDBB->GetVertex(Arista.Tail);
				double Modulo2 = MODULE_2D(HeadVertex, TailVertex);
				if (Modulo2 > Modulo1){
					Modulo1= Modulo2;
					PosicionMayorArista= j;
				}
			}
			Arista= DDBB->GetEdge(EdgesInVertex[PosicionMayorArista]);
			Sorted.push_back(EdgesInVertex[PosicionMayorArista]);
			EdgesInVertex.erase(EdgesInVertex.begin()+PosicionMayorArista);
		}
	}
	EdgesInVertex= Sorted;
}


long CInflationAxonometric::GetReferenceEdge(CDB_Model *DDBB,
											 std::vector<long> EdgesInVertex,
											 std::vector<long> SpanningTree)
//Returns the Reference Edge of one vertex,
//selecting it from the list of all the edges connected to the vertex

//The algorithm works in two stages:
//First,	all edges whose two vertices have already been visited 
//			(i.e. are in the SpanningTree),are marked as valid Reference Edges
//Second,	one of those valid Reference Edges is returned as the best choice
//			(The current version simply returns the first Reference Edge found)

//Value -1 is returned if no reference edge is found
{
	long ReferenceEdge= -1;

	//Find all valid Reference Edges
	std::vector<long> ReferenceEdges;

	for (long j=0; j<(long)EdgesInVertex.size(); j++){
		CEdge AristaJ= DDBB->GetEdge(EdgesInVertex[j]);
		if (IsValueInVector(AristaJ.Head, SpanningTree) &&
			IsValueInVector(AristaJ.Tail, SpanningTree)){
			ReferenceEdges.push_back(EdgesInVertex[j]);
		}
	}

	//Select the best Reference Edge
	if (ReferenceEdges.size()!=0){
		ReferenceEdge= ReferenceEdges[0];
	}

	return ReferenceEdge;
}


double CInflationAxonometric::GetLateralZ(CDB_Model *DDBB,
										  CVertex VectorA, double ModuleA, double Fi,
										  CVertex HeadVertexC, CVertex TailVertexC,
										  long CurrentVertex,
										  long ReferenceEdge,
										  std::vector<double> VariablesZ)
//Returns the value of the Z coordinate of one lateral vertex,

//Two different cases are considered:
//	* With reference edge
//	* Without reference edge

//Central vertex is a vertex whose Z coordinate has been calculated.
//Lateral vertex is a vertex connected to the central vertex through a normalon edge.
//Reference edge is an edge connected to the central vertex
//and connected to another vertex already calculated
{
	double CentralZ= VariablesZ[CurrentVertex];
	double LateralZ;

	//Without reference edge
	//----------------------
	if (ReferenceEdge < 0){
		LateralZ= CentralZ + ModuleA*tan(Fi);

		////If edge is horizontal
		//if ((VectorA.y-Accuracy) < 0){
		//	//Si se extiende hacia valores positivos del eje X
		//	if (VectorA.x>0)
		//		LateralZ= CentralZ - ModuloA*tan(Fi);
		//	else
		//		//Si se extiende hacia valores negativos del eje X
		//		LateralZ= CentralZ + ModuloA*tan(Fi);
		//}
		//else{
		//	//Si la arista tiende hacia valores de y negativos
		//	if (VectorA.y<0)
		//		LateralZ= CentralZ + ModuloA*tan(Fi);
		//	// Si la arista tiende hacia valores de y positivos
		//	else
		//		LateralZ= CentralZ - ModuloA*tan(Fi);
		//}
	}

	//With reference edge
	//-------------------
	else{
		CEdge EdgeR= DDBB->GetEdge(ReferenceEdge);
		CVertex HeadVertexR= DDBB->GetVertex(EdgeR.Head);
		CVertex TailVertexR= DDBB->GetVertex(EdgeR.Tail);
		HeadVertexR.z= VariablesZ[EdgeR.Head];
		TailVertexR.z= VariablesZ[EdgeR.Tail];

		if (NearlyCollinear(HeadVertexC.x, HeadVertexC.y,
							TailVertexC.x, TailVertexC.y,
							HeadVertexR.x, HeadVertexR.y,
							TailVertexR.x, TailVertexR.y,
							DEGREES_TO_RADIANS(5), 0.10) <= 0){

			//Evaluate the two Necker reversal solutions
			double LateralZ1= CentralZ + ModuleA*tan(Fi);
			double LateralZ2= CentralZ - ModuleA*tan(Fi);

			//Evaluate the reference edge
			double ModuleReference= MODULE_3D(HeadVertexR, TailVertexR);

			CVertex VectorReference;
			double zRef;
			if (EdgeR.Head == CurrentVertex){
				VectorReference.x= TailVertexR.x-HeadVertexR.x;
				VectorReference.y= TailVertexR.y-HeadVertexR.y;
				VectorReference.z= TailVertexR.z-HeadVertexR.z;
				zRef= HeadVertexR.z;
			}
			else{
				VectorReference.x= HeadVertexR.x-TailVertexR.x;
				VectorReference.y= HeadVertexR.y-TailVertexR.y;
				VectorReference.z= HeadVertexR.z-TailVertexR.z;
				zRef= TailVertexR.z;
			}

			//Select the Necker reversal that produces an angle closer to 90
			//between the new edge and the reference edge
			CVertex VectorA1= VectorA;
			CVertex VectorA2= VectorA;
			VectorA1.z= LateralZ1 - zRef;
			VectorA2.z= LateralZ2 - zRef;
			CVertex ZERO;
			ZERO.x=0; ZERO.y=0; ZERO.z=0;
			double ModuloA1= MODULE_3D(VectorA1, ZERO);
			double ModuloA2= MODULE_3D(VectorA2, ZERO);

			double cosAngle1= DOT_PRODUCT_3D(VectorA1,VectorReference)/(ModuloA1*ModuleReference);
			cosAngle1= max(cosAngle1, -1);
			cosAngle1= min(cosAngle1, +1);
			double cosAngle2= DOT_PRODUCT_3D(VectorA2,VectorReference)/(ModuloA2*ModuleReference);
			cosAngle2= max(cosAngle2, -1);
			cosAngle2= min(cosAngle2, +1);
			double Angle1= acos(cosAngle1);
			double Angle2= acos(cosAngle2);

			if (std::fabs(Angle1-NUMBER_PI/2) < std::fabs(Angle2-NUMBER_PI/2))
				LateralZ= LateralZ1;
			else
				LateralZ= LateralZ2;
		}

		else{
			//Evaluate the new vertex applying proportionallity
			//between the new edge and its collinear reference edge
			double ModuleR= DDBB->GetLength2D(ReferenceEdge);

			double LateralZ1;
			if (EdgeR.Head == CurrentVertex)
				LateralZ1= VariablesZ[EdgeR.Tail];
			else
				LateralZ1= VariablesZ[EdgeR.Head];

			LateralZ=((CentralZ-LateralZ1)*(ModuleA+ModuleR)/ModuleR)+LateralZ1;
		}
	}

	return LateralZ;
}


bool CInflationAxonometric::GetTriplet(CDB_Model *DDBB, long CurrentEdge,
										std::vector<long> EdgesInVertex,
										long CentralVertex,
										//double ModuleA, CVertex VectorA,
										double &ModuleB, CVertex &VectorB,
										double &ModuleC, CVertex &VectorC)
//Get the triplet formed by the current edge (already visited)
//and two other normalon edges connected to the CentralVertex

//If only two normalon edges are connected to the Central vertex,
//then the third edge of the triplet is replaced by a fictitious normalon edge

//If only one normalon edge is connected to the central vertex 
//(i.e. the current edge)
//then the spanning tree cannot propagate through it
{
	if(EdgesInVertex.size() > 2){
		//Use two actual lateral vertices
		long j=(CurrentEdge+1) % EdgesInVertex.size();
		long k=(CurrentEdge+2) % EdgesInVertex.size();

		CEdge Aristaj= DDBB->GetEdge(EdgesInVertex[j]);
		CVertex HeadVertexJ= DDBB->GetVertex(Aristaj.Head);
		CVertex TailVertexJ= DDBB->GetVertex(Aristaj.Tail);
		ModuleB= MODULE_2D(HeadVertexJ, TailVertexJ);

		if (Aristaj.Tail == CentralVertex){
			VectorB.x= HeadVertexJ.x-TailVertexJ.x;
			VectorB.y= HeadVertexJ.y-TailVertexJ.y;
		}
		else{
			VectorB.x= TailVertexJ.x-HeadVertexJ.x;
			VectorB.y= TailVertexJ.y-HeadVertexJ.y;
		}

		//Get the vector defined by the next edge
		CEdge Aristak= DDBB->GetEdge(EdgesInVertex[k]);
		CVertex HeadVertexK= DDBB->GetVertex(Aristak.Head);
		CVertex TailVertexK= DDBB->GetVertex(Aristak.Tail);
		ModuleC= MODULE_2D(HeadVertexK, TailVertexK);

		if (Aristak.Tail == CentralVertex){
			VectorC.x= HeadVertexK.x-TailVertexK.x;
			VectorC.y= HeadVertexK.y-TailVertexK.y;
		}
		else{
			VectorC.x= TailVertexK.x-HeadVertexK.x;
			VectorC.y= TailVertexK.y-HeadVertexK.y;
		}
	}
	else if(EdgesInVertex.size() == 2){
		//Use one actual lateral edge, plus one fictitious edge
		long j=(CurrentEdge+1) % EdgesInVertex.size();

		//Get the vector defined by the actual edge
		CEdge Aristaj= DDBB->GetEdge(EdgesInVertex[j]);
		CVertex HeadVertexJ= DDBB->GetVertex(Aristaj.Head);
		CVertex TailVertexJ= DDBB->GetVertex(Aristaj.Tail);
		ModuleB= MODULE_2D(HeadVertexJ, TailVertexJ);

		if (Aristaj.Tail == CentralVertex){
			VectorB.x= HeadVertexJ.x-TailVertexJ.x;
			VectorB.y= HeadVertexJ.y-TailVertexJ.y;
		}
		else{
			VectorB.x= TailVertexJ.x-HeadVertexJ.x;
			VectorB.y= TailVertexJ.y-HeadVertexJ.y;
		}

		//Get the vector defined by the fictitious edge
		long KIsParallelTo= 0;
		while(KIsParallelTo == ReferApp.DIA.NormalonEdges[EdgesInVertex[j]] ||
			KIsParallelTo == ReferApp.DIA.NormalonEdges[EdgesInVertex[CurrentEdge]])
			KIsParallelTo++;

		ModuleC= 1;
		VectorC.x= cos(ReferApp.Cu.MA.MainAngles[KIsParallelTo]);
		VectorC.y= sin(ReferApp.Cu.MA.MainAngles[KIsParallelTo]);
	}
	else{
		//The current edge is the only normalon edge connected to the vertex
		//hence, no propagation tree can follow this branch
		return false;
	}

	return true;
}


bool CInflationAxonometric::SolveLateralVertex(CDB_Model *DDBB,
											   long CentralVertex,
											   std::vector<long> EdgesInVertex,
											   long CurrentEdge,
											   std::vector<long> &SpanningTree,
											   std::vector<double> &VariablesZ,
											   double &NewZ)
//If lateral vertex was not yet evaluated, evaluates it and returns its Z coordinate
//Returns true if the lateral vertex is evaluated, false otherwise
{
	//Get the current edge
	CEdge AristaC= DDBB->GetEdge(EdgesInVertex[CurrentEdge]);

	//Get the vector defined by the current edge
	CVertex HeadVertexC= DDBB->GetVertex(AristaC.Head);
	CVertex TailVertexC= DDBB->GetVertex(AristaC.Tail);
	double ModuleA= MODULE_2D(HeadVertexC, TailVertexC);

	//Get oriented edge
	CVertex VectorA;
	if (AristaC.Tail == CentralVertex){
		VectorA.x= HeadVertexC.x-TailVertexC.x;
		VectorA.y= HeadVertexC.y-TailVertexC.y;
	}
	else{
		VectorA.x= TailVertexC.x-HeadVertexC.x;
		VectorA.y= TailVertexC.y-HeadVertexC.y;
	}

	//Get the triplet formed by the current edge
	//plus the vectors defined by the previous and next edges
	double ModuleB, ModuleC;
	CVertex VectorB, VectorC;

	if(!GetTriplet(DDBB, CurrentEdge, EdgesInVertex, CentralVertex,
				   //ModuleA, VectorA,
				   ModuleB, VectorB,
				   ModuleC, VectorC))
		return false;

	//Get the angles defined by the triplet in the plane
	double cosAlfa= DOT_PRODUCT_2D(VectorA,VectorB)/(ModuleA*ModuleB);
	cosAlfa= min(max(cosAlfa, -1), +1);
	double Alfa= acos(cosAlfa);
	double cosBeta= DOT_PRODUCT_2D(VectorA,VectorC)/(ModuleA*ModuleC);
	cosBeta= min(max(cosBeta, -1), +1);
	double Beta= acos(cosBeta);

	//----------------------------------------------------------------------
	//The particular cases when only one of the angles is 90 degrees
	//while the other two are 135 degrees or similar
	//(like cabinet, cavalier or militar axonometry),
	//are solved using an artifice to obtain a tentative model.
	//To avoid numerical run-outs, 
	//we simply modify conflictive angles adding a threshold
	//if (ReferApp.DIA.AxonometricApproach==0 && ReferApp.DIA.bAmendCabinet){
	if (ReferApp.DIA.bAmendCabinet){
		if(std::fabs(Alfa-NUMBER_PI/2)<Accuracy) 
			Alfa= ReferApp.DIA.d90degrees;
		else if(std::fabs(Alfa-DEGREES_TO_RADIANS(135))<Accuracy) 
			Alfa= ReferApp.DIA.d135degrees;

		if(std::fabs(Beta-NUMBER_PI/2)<Accuracy) 
			Beta= ReferApp.DIA.d90degrees;
		else if(std::fabs(Beta-DEGREES_TO_RADIANS(135))<Accuracy) 
			Beta= ReferApp.DIA.d135degrees;
	}
	//----------------------------------------------------------------------

	//Get the Z coordinate of the lateral vertex
	bool Evaluado= false;
	if (std::fabs(Alfa+Beta-NUMBER_PI)>Accuracy && 
		std::fabs(Alfa-NUMBER_PI)>Accuracy && 
		std::fabs(Beta-NUMBER_PI)>Accuracy){
		//Evaluate the vertex by the general approach:
		//Get the angle Phi between the Central edge and its projection
		double CotangentA=1/tan(Alfa);
		double CotangentB=1/tan(Beta);
		double root= sqrt(std::fabs(CotangentA*CotangentB));

		if (root <= 1){
			double Fi= asin(root);

			long ReferenceEdge= GetReferenceEdge(DDBB, EdgesInVertex, SpanningTree);

			NewZ= GetLateralZ(DDBB, VectorA, ModuleA, Fi, HeadVertexC, TailVertexC,
							  CentralVertex, ReferenceEdge, VariablesZ);

			Evaluado= true;
		}
		else{
			//WARNING! If angles Alfa and Beta are equal,
			//or at least one is equal to 0 or 180 degrees,
			//then some collinearities prevent the calculations to be done
			Evaluado= false;
		}
	}
	else{
		//Evaluate the vertex using the particular approach for collinear edges
		long j=(CurrentEdge+1) % EdgesInVertex.size();
		CEdge Aristaj= DDBB->GetEdge(EdgesInVertex[j]);

		long k=(CurrentEdge+2) % EdgesInVertex.size();
		CEdge Aristak= DDBB->GetEdge(EdgesInVertex[k]);

		if (std::fabs(Alfa-NUMBER_PI)<Accuracy && 
			IsValueInVector(Aristaj.Head, SpanningTree) &&
			IsValueInVector(Aristaj.Tail, SpanningTree)){

			double NewZ1;
			if (Aristaj.Head == CentralVertex)
				NewZ1= VariablesZ[Aristaj.Tail];
			else
				NewZ1= VariablesZ[Aristaj.Head];

			NewZ=((VariablesZ[CentralVertex]-NewZ1)*(ModuleA+ModuleB)/ModuleB)+NewZ1;
			Evaluado= true;
		}
		else{
			if (std::fabs(Beta-NUMBER_PI)<Accuracy &&
				IsValueInVector(Aristak.Head, SpanningTree) &&
				IsValueInVector(Aristak.Tail, SpanningTree)){

				double NewZ1;
				if (Aristak.Head == CentralVertex)
					NewZ1= VariablesZ[Aristak.Tail];
				else
					NewZ1= VariablesZ[Aristak.Head];

				NewZ=((VariablesZ[CentralVertex]-NewZ1)*(ModuleA+ModuleC)/ModuleC)+NewZ1;
				Evaluado= true;
			}
			else
				Evaluado= false;
		}
	}

	return Evaluado;
}


void CInflationAxonometric::AddLateralToTree(CDB_Model *DDBB,
											 std::vector<long> &SpanningTree,
											 std::vector<double> &Lengths,
											 bool &First,
											 long CurrentNode,
											 long LateralVertex,
											 double Length)
//Add lateral vertex to the spanning tree

//The position depends on whether it is
//the first lateral edge of the current central vertex:
//	* If it is the first lateral vertex of the current central vertex,
//	  then it is placed right after the current vertex
//	* Otherwise, it is sorted by decreasing length
//	  (starting after the current vertex)

//The length of the edge that connects the lateral vertex
//to the central vertex is saved too
{
	if(First){
		if ((long)SpanningTree.size() == CurrentNode+1){
			SpanningTree.push_back(LateralVertex);
			Lengths.push_back(Length);
		}
		else{
			SpanningTree.insert(SpanningTree.begin()+CurrentNode+1, LateralVertex);
			Lengths.insert(Lengths.begin()+CurrentNode+1, Length);
		}
		First= false;
	}
	else{
		//Otherwise, it is sorted by decreasing length (starting after the current vertex)
		long j= CurrentNode+1;
		while (j<(long)Lengths.size()-1 && Lengths[j]>Length){
			j++;
		}
		if (j == Lengths.size()){
			SpanningTree.push_back(LateralVertex);
			Lengths.push_back(Length);
		}
		else{
			SpanningTree.insert(SpanningTree.begin()+j, LateralVertex);
			Lengths.insert(Lengths.begin()+j, Length);
		}
	}
}


void CInflationAxonometric::SolveVertex(CDB_Model *DDBB,
										std::vector<double> &VariablesZ,
										std::vector<long> &SpanningTree,
										long &CurrentNode,
										std::vector<double> &Lengths)
//Calculates the z coordinates of all the lateral vertices
//connected to the current node (CentralVertex)

//Adds the z coordinates to the VariablesZ vector,
//and adds the lateral vertices to the "SpanningTree"

//Updates CurrentNode and Lengths
{
	long CentralVertex= SpanningTree[CurrentNode];

	//Get all the edges connected to the current central vertex
	//sorted by decreasing length
	std::vector<long> EdgesInVertex;
	GetSortedEdges(DDBB, CentralVertex, EdgesInVertex);

	//Loop over all the lateral vertices connected to the central vertex
	//through normalon edges
	bool First= true;
	for (long CurrentEdge= 0; CurrentEdge<(long)EdgesInVertex.size(); CurrentEdge++){
		CEdge AristaC= DDBB->GetEdge(EdgesInVertex[CurrentEdge]);

		long LateralVertex= AristaC.Head;
		if (AristaC.Head == CentralVertex) LateralVertex= AristaC.Tail;

		if(!IsValueInVector(LateralVertex, SpanningTree)){
			//Calculate the Z coordinate of the lateral vertex
			double NewZ;
			if(SolveLateralVertex(DDBB,
								  CentralVertex,
								  EdgesInVertex,
								  CurrentEdge,
								  SpanningTree,
								  VariablesZ,
								  NewZ)){
				//Save Z coordinate of the lateral vertex
				VariablesZ[LateralVertex]= NewZ;

				//Update information to display the tree
				ReferApp.DIA.AxonometricTreeVertices.push_back(CentralVertex);
				ReferApp.DIA.AxonometricTreeVertices.push_back(LateralVertex);

				//Add lateral vertex to the spanning tree
				//(its distance is added too)
				double Length= DDBB->GetLength2D(EdgesInVertex[CurrentEdge]);

				AddLateralToTree(DDBB, SpanningTree, Lengths,
								 First, CurrentNode, LateralVertex, Length);
			}
		}
	}
}


void CInflationAxonometric::GetAxonometricInflation_2001(CDB_Model *DDBB,
														 long VertexCloserToCentre)
//Calculates Z coordinates for vertices of normalon objects
//following a Kruskal-type spanning tree

//The calculated Z coordinates are stored in the DDBB model database

//Based on:
	//Company P., Contero M., Conesa J. and Piquer A.
	//An optimisation-based reconstruction engine for 3D modelling by sketching.
	//Computers & Graphics (ISSN 0097-8493). Vol. 28, No 6. December 2004. pp. 955-979

	//Conesa-Pastor J.
	//RECONSTRUCCIÓN GEOMÉTRICA DE SÓLIDOS UTILIZANDO TÉCNICAS DE OPTIMIZACIÓN.
	//PhD Thesis, 2001
	//Departamento de Estructuras y Construcción.
	//Universidad Politécnica de Cartagena
{
	//Initialize local copy of Z's
	std::vector<double> VariablesZ(DDBB->GetSizeVertices());
	VariablesZ.assign(DDBB->GetSizeVertices(), 0);

	//Start the spanning tree with the vertex closer to the centre
	std::vector<long>SpanningTree;
	SpanningTree.push_back(VertexCloserToCentre);

	std::vector<double> Lengths;  //Distances to father
	Lengths.push_back(0);  //Distance root to father

	//Loop to follow the spanning tree until all vertices are evaluated
	//(or no more edges can be evaluated)
		//WARNING! The spanning tree is incrementally calculated!
		//Direct sons are added for every vertex visited
	//-----------------------------------------------------------------
	for (long CurrentNode= 0; CurrentNode<(long)SpanningTree.size(); CurrentNode++){
		SolveVertex(DDBB, VariablesZ, SpanningTree, CurrentNode, Lengths);
	}

	//Check if all the vertices have been inflated
	//--------------------------------------------
	if(DDBB->GetSizeVertices() != SpanningTree.size()){
		AfxMessageBox("Axonometric inflation is unable to solve all the vertices");
	}

	//Translate the 3D model
	//(to place the most receding vertex 
	//at the same depth that the origin of coordinates)
	//-------------------------------------------------
	double MinimaZ= VariablesZ[0];
	for (long i=1; i<(long)VariablesZ.size(); i++){
		MinimaZ= min(MinimaZ, VariablesZ[i]);
	}
	for (long i=0; i<(long)VariablesZ.size(); i++){
		VariablesZ[i]-=MinimaZ;
	}

	//Update the coordinates of the vertices in the model database
	//------------------------------------------------------------
	DDBB->SetAllVertex_Z(VariablesZ);

	return;
}


//////////////////////////////////////////////////////////////////////


void CInflationAxonometric::GetSons(CDB_Model *DDBB, long father, std::vector<long> &Sons)
//Returns the list of normalon "sons"
//(vertices connected to vertex "father" through normalon edges)
{
	for (long v= 0; v <(long)DDBB->GetSizeEdgesInVertex(father); v++){
		long iEdge= DDBB->GetEdgeInVertex(father, v);
		if (ReferApp.DIA.NormalonEdges[iEdge] >= 0){
			CEdge Arista= DDBB->GetEdge(iEdge);
			if(Arista.Head == father)
				Sons.push_back(Arista.Tail);
			else
				Sons.push_back(Arista.Head);
		}
	}
}


void CInflationAxonometric::SortSons(CDB_Model *DDBB,
									 long father,
									 std::vector<long> &Sons,
									 std::vector<double> &DistanceToFather)
//Sorts the "Sons" by decreasing distance
//(Sons are vertices connected to vertex "father" through normalon edges)

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


void CInflationAxonometric::GetCoordinates(long father, long son, double DistanceToFather,
										   CDB_Model *DDBB,
										   std::vector<std::vector<double>> &Coords)
//Calculate coordinates of a son vertex
//incrementally from the coordinates of its father
{
	long NumEdge= DDBB->GetEdgeByVerticesNums(father, son);
	long NumAxis= ReferApp.DIA.NormalonEdges[NumEdge];

	//Same coordinates for axis non-parallel to the edge
	long Other1= (NumAxis+1) % 3;
	long Other2= (NumAxis+2) % 3;
	Coords[Other1][son]= Coords[Other1][father];
	Coords[Other2][son]= Coords[Other2][father];

	//Increment the coordinate of the axis parallel to the edge
	//if segment has the same sense than coordinate axis
	//(decrement otherwise)
	double Direct= DDBB->GetAngle2DX_2PI(NumEdge, father, Accuracy);
	double Reverse= DDBB->GetAngle2DX_2PI(NumEdge, son, Accuracy);

	if (std::fabs(ReferApp.Cu.MA.MainAngles[NumAxis]-Direct) < 
		std::fabs(ReferApp.Cu.MA.MainAngles[NumAxis]-Reverse)){
		Coords[NumAxis][son]= Coords[NumAxis][father] + DistanceToFather*ReferApp.Cu.MA.CoefE[NumAxis];
	}
	else{
		Coords[NumAxis][son]= Coords[NumAxis][father] - DistanceToFather*ReferApp.Cu.MA.CoefE[NumAxis];
	}
}


void CInflationAxonometric::DFS_VisitSons(long father,
										  std::vector<bool> &visited,
										  CDB_Model *DDBB,
										  std::vector<std::vector<double>> &Coords)
//Deep First Search for recursively assigning incremental coordinates to "son" vertices:
//		- connected to vertex "father"
//		- not yet explored

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
			//Get coordinates of current son
			GetCoordinates(father, Sons[v], DistanceToFather[v], DDBB, Coords);

			//Save information to display the tree
			ReferApp.DIA.AxonometricTreeVertices.push_back(father);
			ReferApp.DIA.AxonometricTreeVertices.push_back(Sons[v]);

			//RECURSIVELY explore its own sons
			DFS_VisitSons(Sons[v], visited, DDBB, Coords);
		}
	}
}


void CInflationAxonometric::BFS_VisitVertices(long first,
											  CDB_Model *DDBB,
											  std::vector<std::vector<double>> &Coords)
//Incrementally assign coordinates

//Breadth First Search is applied to assign coordinates to all son vertices
//by incrementing or decrementing the coordinate of its father

//Son vertices are ordered to be visited by decreasing distance to their father
{
	//Initialize local data
	std::queue<long> Q;

	std::vector<bool> visited(DDBB->GetSizeVertices(), false);
	std::vector<long> father(DDBB->GetSizeVertices(), -1);
	std::vector<double> DistanceToFather(DDBB->GetSizeVertices(), 0);

	//Insert the first vertex in the queue
	Q.push(first);
	visited[first] = true;
	father[first]= first;
	DistanceToFather[first]= 0;

	bool First= true;

	//Loop to calculate the incremental Z coordinate of the current vertex
	//and insert their son vertices in the queue
	while(!Q.empty()){
		//Extract the front vertex from the queue
		long v = Q.front();
		Q.pop();

		//Get coordinates of current son
		if(First)
			First= false;  //First vertex was fixed in advance
		else
			GetCoordinates(father[v], v, DistanceToFather[v], DDBB, Coords);

		//Save information to display the tree
		ReferApp.DIA.AxonometricTreeVertices.push_back(father[v]);
		ReferApp.DIA.AxonometricTreeVertices.push_back(v);

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


void CInflationAxonometric::GetAxonometricInflation_2011(CDB_Model *DDBB, long CentralVertex)
//Calculates coordinates by axonometric inflation

//Vertices are visited following a spanning tree
//The sequence may be "deep first" or "breath first"
//The coordinates of each vertex are incremental 
//(are obtained increasing or decreasing) from parent coordinates

//Uses recursivity to get a compact implementation

//The algorithm only uses topological information of the model stored in DDBB
//(which uses the global reference system)
//Geometrical information about the Z coordinates of the vertices is calculated
//in the main reference system (which must have been calculated in advance)

//The inflated model is stored in the DDBB database,
//after transforming the inflated coordinates
//from the main reference system to the global reference system
{
	//Use a temporary local database to store the inflated coordinates
	std::vector<std::vector<double>> Coords; 
	Coords.resize(3);
	for (long i=0; i<3; ++i){
		Coords[i].resize((long)DDBB->GetSizeVertices());
	}

	//Assign arbitrary coordinates to the central vertex
	CVertex cVertex= DDBB->GetVertex(CentralVertex);

	//Coords[0][CentralVertex]= cVertex.x;
	//Coords[1][CentralVertex]= cVertex.y;
	Coords[0][CentralVertex]= 0;
	Coords[1][CentralVertex]= 0;
	Coords[2][CentralVertex]= 0;

	//Assign coordinates, following the spanning tree
	if (ReferApp.DIA.SpanningTreeAxonometric==0){
		//Deep First Search
		//-----------------

		//Information to display the tree
		ReferApp.DIA.AxonometricTreeVertices.push_back(CentralVertex);
		ReferApp.DIA.AxonometricTreeVertices.push_back(CentralVertex);

		//Reset search flags
		std::vector<bool> visited(DDBB->GetSizeVertices(), false);

		//RECURSIVELY assign coordinates to the son vertices
		DFS_VisitSons(CentralVertex, visited, DDBB, Coords);

	}
	else{
		//Breath first search
		//-------------------
		BFS_VisitVertices(CentralVertex, DDBB, Coords);
	}

	//Save the inflated coordinates in a Main Axes database
	for(long i=0; i<DDBB->GetSizeVertices(); i++ ) {
		CVertex V= DDBB->GetVertex(i);
		V.x = Coords[0][i];
		V.y = Coords[1][i];
		V.z = Coords[2][i];
		DDBB->SetVertex(i, V);
	};

	//Update the model database
	//by transforming the coordinates from the main reference system to the inflation system
	CCueMainAxes MA;
	MA.TransformFromMainAxes(DDBB, true);
}


//////////////////////////////////////////////////////////////////////


void CInflationAxonometric::GetAxonometricInflation(CDB_Model *DDBB)
//Get Z values as calculated by axonometric inflation

//Z values can be calculated following two different strategies
//		* Approach 2001, as published in Technical Report REGEO 01
//				Company, P., Conesa, J and Aleixos, N.
//				Axonometric Inflation in Line Drawings Reconstruction.
//				Regeo Technical Report 01/2001.
//				URL: http://www.regeo.uji.es/technicalreports.htm

//		* Approach 2011, new contribution by Pedro Company
//				-The implementation is recursive
//				-Some improvements to deal with Cabinet projections were added

{
	//Initialization
	//--------------
	//Check that main axes have been calculated
	if(ReferApp.Cu.MA.MainAngles.size() != 3){
		AfxMessageBox ("Axonometric inflation requires main axes");
		CReferDoc::GetDoc()->RefreshWindows();
		return;
	}

	//Clear information to display the tree
	ReferApp.DIA.AxonometricTreeVertices.clear();

	//Get the vertex of the input drawing closer to the centre
	long CentralVertex= GetCentralNormalonVertex(DDBB);

	//Select approach
	//---------------
	if (ReferApp.DIA.AxonometricApproach==0){
		GetAxonometricInflation_2001(DDBB, CentralVertex);
	}
	else{
		GetAxonometricInflation_2011(DDBB, CentralVertex);
	}
}