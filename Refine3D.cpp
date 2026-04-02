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

#include "Tools_Vector.h"
#include "Tools_Geometry.h"

#include "DB_Model.h"
#include "DB_CSG.h"
#include "Refer.h"
#include "DB_Model_Maths.h"
#include "resource.h"
#include "ReferDoc.h"

#include "CueMainAxes.h"

#include "Refine3D.h"

//private:
//void RefineFacePlanarity_Ana(CDB_Model *DDBB)
//// Refines planarity of faces, without
//
//// Analize candidate 3D face to enforce its panarity as follows:
////	- Look for the face with the greatest number
////	  of edges (not yet evaluated)
//
////	- Parse all possible planes defined on the face 
////	  by sequentially using every triplet of its vertices
//
////	- Get the best plane on the list 
////	  (the one that minimizes the distances of the remaining vertices)
//
////	- Repeat until all faces have been visited
//{
//	long IndiceVertice, IndiceArista, IndiceCara;
//	long Aux;
//	double A, B, C, D, Denominador, Distancia, MejorDistancia;
//
//	//MejorDistancia= 100000.;
//
//	CFace Cara;
//	std::vector<long> VVerticesEvaluados;
//	std::vector<long> VCarasEvaluadas;
//	bool TodasCarasEvaluadas=false;
//	long CarasMasTresVertices=0;
//
//	for (IndiceCara=0; IndiceCara<DDBB->GetSizeFaces(); IndiceCara++){
//		Cara= DDBB->GetFace(IndiceCara);
//		if (Cara.GetSizeEdgesInFace()>3) 
//			CarasMasTresVertices++;
//	}
//
//	do{
//		//Look for the face that is composed of the greatest number of edges and that is not already evaluated
//		//Its index is saved in IndiceMayorCara and its number of edges in AristasMayorCara
//		long IndiceMayorCara=0;
//		while (IndiceMayorCara<DDBB->GetSizeFaces() &&
//			   IsValueInVector(IndiceMayorCara, VCarasEvaluadas, Aux)){
//			IndiceMayorCara++;
//		}
//
//		Cara= DDBB->GetFace(IndiceMayorCara);
//		long AristasMayorCara = Cara.GetSizeEdgesInFace();
//
//		if (IndiceMayorCara<DDBB->GetSizeFaces()-2){
//			for (IndiceCara=IndiceMayorCara+1; IndiceCara<DDBB->GetSizeFaces(); IndiceCara++){
//				if (!IsValueInVector(IndiceCara, VCarasEvaluadas, Aux)){
//					Cara= DDBB->GetFace(IndiceCara);
//					if (Cara.GetSizeEdgesInFace() > AristasMayorCara){
//						AristasMayorCara=Cara.GetSizeEdgesInFace();
//						IndiceMayorCara=IndiceCara;
//					}
//				}
//			}
//		}
//
//		//Faces with 3 vertices are trivially flat
//		//Hence, only faces with more thant 3 vertices are analyzed
//		CFace CaraI= DDBB->GetFace(IndiceMayorCara);
//		if (CaraI.GetSizeEdgesInFace() > 3){
//
//			//Get all the vertices that belong to the face
//			std::vector<long> VVerticesCara;
//			Cara= DDBB->GetFace(IndiceMayorCara);
//			for (long IndiceIndiceArista=0; IndiceIndiceArista<Cara.GetSizeEdgesInFace(); IndiceIndiceArista++){
//				IndiceArista= Cara.GetEdgeInFace(IndiceIndiceArista);
//				CEdge Arista= DDBB->GetEdge(IndiceArista);
//				if (!IsValueInVector(Arista.Head, VVerticesCara, Aux))
//					VVerticesCara.push_back(Arista.Head);
//				if (!IsValueInVector(Arista.Tail, VVerticesCara, Aux))
//					VVerticesCara.push_back(Arista.Tail);
//			}
//
//			//Construct all possible planes defined on the face for every triplet of vertices
//			//Taking care not to modify those that have already been evaluated previously
//			std::vector<long> EvaluatedVerticesInFace;
//			for(long IndiceIndiceVertice=0; IndiceIndiceVertice<(long)VVerticesCara.size(); IndiceIndiceVertice++){
//				IndiceVertice= VVerticesCara[IndiceIndiceVertice];
//				if (IsValueInVector(IndiceVertice, VVerticesEvaluados, Aux)){
//					EvaluatedVerticesInFace.push_back(IndiceVertice);
//					VVerticesCara.erase(VVerticesCara.begin()+IndiceIndiceVertice);
//					IndiceIndiceVertice--;
//				}
//			}
//			
//			std::vector<long> VerticesBestFace;
//			std::vector<double> CoefBestFace;
//
//			long i=0;
//			long i1, j1, k1;
//			do{
//				if ((long)EvaluatedVerticesInFace.size()!=0){
//					//Take the first of the already evaluated vertices
//					IndiceVertice= EvaluatedVerticesInFace[0];
//					//This prevents from loop repetition
//					i= (long)VVerticesCara.size()-2;
//					//This indicates that the we use an evaluated vertex
//					i1=-1;
//				}
//				else{
//					//Take the first vertex of the face, is none has been evaluated yet
//					IndiceVertice= VVerticesCara[i];
//					//Reset the loop counter
//					i1=i;
//				}
//
//				CVertex Vertice1= DDBB->GetVertex(IndiceVertice);
//
//				//Set j to "0" or "1" depending on wheter the previous vertex is already evaluated or not
//				long j=i1+1;
//				do{
//					if ((long)EvaluatedVerticesInFace.size()>1){
//						//If more evaluated vertices exist, take the next of them
//						IndiceVertice= EvaluatedVerticesInFace[1];
//						//This prevents from loop repetition
//						j= (long)VVerticesCara.size()-1;
//						//This indicates that the we use an evaluated vertex
//						j1=-1;
//					}
//					else{
//						//If no more evaluated vertices exist, take the next vertex of the face
//						IndiceVertice= VVerticesCara[j];
//						//Update the loop counter
//						j1=j;
//					}
//					CVertex Vertice2= DDBB->GetVertex(IndiceVertice);
//					
//					CVertex V1;
//					V1.x=Vertice2.x-Vertice1.x;
//					V1.y=Vertice2.y-Vertice1.y;
//					V1.z=Vertice2.z-Vertice1.z;
//
//					long k=j1+1;
//					do{
//						if ((long)EvaluatedVerticesInFace.size()>2){
//							IndiceVertice= EvaluatedVerticesInFace[2];
//							k= (long)VVerticesCara.size();
//							k1=-1;
//						}
//						else{
//							IndiceVertice= VVerticesCara[k];
//							k1=k;
//						}
//
//						CVertex Vertice3= DDBB->GetVertex(IndiceVertice);
//						CVertex V2;
//						V2.x=Vertice3.x-Vertice1.x;
//						V2.y=Vertice3.y-Vertice1.y;
//						V2.z=Vertice3.z-Vertice1.z;
//
//						CVertex VNormal;
//						VNormal.x=V1.y*V2.z-V1.z*V2.y;
//						VNormal.y=V1.z*V2.x-V1.x*V2.z;
//						VNormal.z=V1.x*V2.y-V1.y*V2.x;
//						D=-VNormal.x*Vertice1.x-VNormal.y*Vertice1.y-VNormal.z*Vertice1.z;
//				
//						//Check the distance of the rest of the vertices to the plane 
//						//and keep the combination of vertices that makes these distances smaller,
//						//and the coefficients of the plane
//						Denominador=sqrt(pow(VNormal.x,2)+pow(VNormal.y,2)+pow(VNormal.z,2));
//						Distancia=0;
//						for (long IndiceIndiceVertice=0; IndiceIndiceVertice<(long)VVerticesCara.size(); IndiceIndiceVertice++){
//							if (IndiceIndiceVertice!=i1 && IndiceIndiceVertice!=j1 && IndiceIndiceVertice!=k1){
//								IndiceVertice= VVerticesCara[IndiceIndiceVertice];
//								CVertex Vertice= DDBB->GetVertex(IndiceVertice);
//								Distancia+=std::fabs(VNormal.x*Vertice.x+VNormal.y*Vertice.y+VNormal.z*Vertice.z+D)/Denominador;
//							}
//						}
//
//						if (i1==0 && j1==1 && k1==2 ||
//							i1==-1 && j1==0 && k1==1 ||
//							i1==-1 && j1==-1 && k1==0 ||
//							i1==-1 && j1==-1 && k1==-1 ||
//							Distancia<MejorDistancia){
//							MejorDistancia=Distancia;
//							VerticesBestFace.push_back(i1);
//							VerticesBestFace.push_back(j1);
//							VerticesBestFace.push_back(k1);
//							CoefBestFace.push_back(VNormal.x);
//							CoefBestFace.push_back(VNormal.y);
//							CoefBestFace.push_back(VNormal.z);
//							CoefBestFace.push_back(D);
//						}
//						k++;
//					}while (k<(long)VVerticesCara.size());
//					j++;
//				}while (j<(long)VVerticesCara.size()-1);
//				i++;
//			}while (i<(long)VVerticesCara.size()-2);
//		
//			//Get the best plane on the list
//			//and force all the vertices of the face to be placed on it
//			long kk= VerticesBestFace[(long)VerticesBestFace.size()-1];
//			if (kk==-1) kk= EvaluatedVerticesInFace[2];
//			else kk= VVerticesCara[kk];
//			long jj= VerticesBestFace[(long)VerticesBestFace.size()-2];
//			if (jj==-1) jj= EvaluatedVerticesInFace[1];
//			else jj= VVerticesCara[jj];
//			long ii= VerticesBestFace[(long)VerticesBestFace.size()-3];
//			if (ii==-1) ii= EvaluatedVerticesInFace[0];
//			//else i= VVerticesCara[ii];
//			else ii= VVerticesCara[ii];
//
//			D= CoefBestFace[CoefBestFace.size()-1];
//			C= CoefBestFace[CoefBestFace.size()-2];
//			assert( !_isnan(C) );
//			B= CoefBestFace[CoefBestFace.size()-3];
//			A= CoefBestFace[CoefBestFace.size()-4];
//
//			for (long IndiceIndiceVertice=0; IndiceIndiceVertice<(long)VVerticesCara.size()	; IndiceIndiceVertice++){
//				IndiceVertice= VVerticesCara[IndiceIndiceVertice];
//				if (IndiceVertice!=ii && IndiceVertice!=jj && IndiceVertice!=kk){
//					CVertex Vertice= DDBB->GetVertex(IndiceVertice);
//					Vertice.z=(-D-A*Vertice.x-B*Vertice.y)/C;
//					assert(std::fabs(Vertice.z)<10000);
//					DDBB->SetVertex(IndiceVertice, Vertice);
//				}
//			}
//			
//			//Add to the list of evaluated vertices those that I finish evaluating
//			for (long IndiceIndiceVertice=0; IndiceIndiceVertice<(long)VVerticesCara.size(); IndiceIndiceVertice++){
//				IndiceVertice= VVerticesCara[IndiceIndiceVertice];
//				if (!IsValueInVector(IndiceVertice, VVerticesEvaluados, Aux))
//					VVerticesEvaluados.push_back(IndiceVertice);
//			}
//			//Add to the list of evaluated faces the one I finish evaluating
//			VCarasEvaluadas.push_back(IndiceMayorCara);
//		}
//
//		if ((long)VCarasEvaluadas.size()==CarasMasTresVertices ||
//			(long)VVerticesEvaluados.size() == DDBB->GetSizeVertices())
//			TodasCarasEvaluadas=true;
//
//	}while (!TodasCarasEvaluadas);
//}


void CheckTriplets(CDB_Model *DDBB,
				   std::vector<long> VerticesInFace,
				   std::vector<long> EvaluatedVerticesInFace,
				   std::vector<long>EvaluatedVertices,
				   std::vector<long> &VerticesBestFace,
				   std::vector<double> &CoefBestFace)
//Consecutively select triplets of vertices of the current face
//to detect the one that minimizes the distances
//from the remaining vertices to the plane defined by the triplet


//The approach prioritizes triplets of vertices 
//previously relocated while visiting prior faces

//If previously visited vertices existe, they are assumed as best choices,
//and no alternative triplets are searched
{
	bool FirstTriplet= true;
	double BestDistance;

	//Loop to select the first vertex of the triplet
	//----------------------------------------------
	//for (long i= 0; i<(long)VerticesInFace.size()-2; i++){
	long i=0;
	do{
		long IndexVertex1;
		long i1;
		if (EvaluatedVerticesInFace.size() > 0){
			//Take the first of the already evaluated vertices
			IndexVertex1= EvaluatedVerticesInFace[0];
			i= VerticesInFace.size()-2;  //No more first vertices will be tested
			i1=-1;   //Flag to indicates the use of an evaluated vertex
		}
		else{
			//Take the next non-evaluated vertex of the face
			IndexVertex1= VerticesInFace[i];
			i1=i;
		}

		CVertex Vertice1= DDBB->GetVertex(IndexVertex1);

		//Loop to select the second vertex of the triplet
		//-----------------------------------------------
		//for (long j= i1+1; j<(long)VerticesInFace.size()-1; j++){
		long j=i1+1;
		do{
			long IndexVertex2;
			long j1;
			if (EvaluatedVerticesInFace.size() > 1){
				//If more evaluated vertices exist, take the next of them
				IndexVertex2= EvaluatedVerticesInFace[1];
				j= VerticesInFace.size()-1;   //No more second vertices will be tested
				j1=-1;   //Flag to indicates the use an evaluated vertex
			}
			else{
				//Take the next non-evaluated vertex of the face
				IndexVertex2= VerticesInFace[j];
				j1=j;
			}

			CVertex Vertice2= DDBB->GetVertex(IndexVertex2);

			CVertex V1; //First vector to calculate the normal
			V1.x=Vertice2.x-Vertice1.x;
			V1.y=Vertice2.y-Vertice1.y;
			V1.z=Vertice2.z-Vertice1.z;

			//Loop to select the third vertex of the triplet
			//----------------------------------------------
			//for (long k= j1+1; k<(long)VerticesInFace.size(); k++){
			long k=j1+1;
			do{
				long IndiceVertice3;
				long k1;
				if (EvaluatedVerticesInFace.size() > 2){
					IndiceVertice3= EvaluatedVerticesInFace[2];
					k= VerticesInFace.size(); //No more third vertices will be tested
					k1=-1;
				}
				else{
					IndiceVertice3= VerticesInFace[k];
					k1=k;
				}

				CVertex Vertice3= DDBB->GetVertex(IndiceVertice3);

				CVertex V2; //Second vector to calculate the normal
				V2.x=Vertice3.x-Vertice1.x;
				V2.y=Vertice3.y-Vertice1.y;
				V2.z=Vertice3.z-Vertice1.z;

				//Check the triplet
				//-----------------
				//Normal to the plane defined by the current triplet
				CVertex VNormal;
				VNormal.x=V1.y*V2.z-V1.z*V2.y;
				VNormal.y=V1.z*V2.x-V1.x*V2.z;
				VNormal.z=V1.x*V2.y-V1.y*V2.x;
				double DD=-VNormal.x*Vertice1.x-VNormal.y*Vertice1.y-VNormal.z*Vertice1.z;

				//Calculate the sum of distances of the rest of the vertices to the plane
				//defined by the current triplet
				double Distance= 0;
				double Denominator= sqrt(pow(VNormal.x,2)+pow(VNormal.y,2)+pow(VNormal.z,2));
				for (long iiv= 0; iiv<(long)VerticesInFace.size(); iiv++){
					if (iiv!=i1 && iiv!=j1 && iiv!=k1){  //if the vertex does not belong to the current triplet
						CVertex Vertex= DDBB->GetVertex(VerticesInFace[iiv]);
						Distance+= std::fabs(VNormal.x*Vertex.x + VNormal.y*Vertex.y + VNormal.z*Vertex.z + DD)/Denominator;
					}
				}

				//Update current triplet if better than previous
				//----------------------------------------------
				if (FirstTriplet || Distance < BestDistance){
					//Keep the combination of vertices that makes the distances smaller
					FirstTriplet= false;
					BestDistance= Distance;

					if (i1==-1)
						VerticesBestFace[0]= EvaluatedVerticesInFace[0];
					else
						VerticesBestFace[0]= VerticesInFace[i1];

					if (j1==-1)
						VerticesBestFace[1]= EvaluatedVerticesInFace[1];
					else
						VerticesBestFace[1]= VerticesInFace[j1];

					if (k1==-1)
						VerticesBestFace[2]= EvaluatedVerticesInFace[2];
					else
						VerticesBestFace[2]= VerticesInFace[k1];

					CoefBestFace[0]= VNormal.x;
					CoefBestFace[1]= VNormal.y;
					CoefBestFace[2]= VNormal.z;
					CoefBestFace[3]= DD;
				}
	//		}  //loop k
	//	}  //loop j
	//}  //loop i
				k++;
			}while (k<(long)VerticesInFace.size());
			j++;
		}while (j<(long)VerticesInFace.size()-1);
		i++;
	}while (i<(long)VerticesInFace.size()-2);
}


void RefineFaces(CDB_Model *DDBB, std::vector<long> FacesToParse)
//Consecutively parse the faces
//by relocating all their not yet relocated vertices

//The approach avoids to repeatedly relocating the same vertices,
//by visiting each vertex no more than one time
{
	std::vector<long> EvaluatedVertices;  //List of vertices of the B-Re model already evaluated (in prior faces)

	//Loop over all the faces to parse
	for (long iface=0; iface<(long)FacesToParse.size(); iface++){
		//Get all the vertices that belong to the face
		std::vector<long> VerticesInFace= DDBB->GetAllVerticesInFace(DDBB->GetFace(FacesToParse[iface]));

		//Remove vertices that have already been evaluated previously
		//(To prevent further modifying them)
		std::vector<long> EvaluatedVerticesInFace;
		for(long iv=0; iv<(long)VerticesInFace.size(); iv++){
			if (IsValueInVector(VerticesInFace[iv], EvaluatedVertices)){
				EvaluatedVerticesInFace.push_back(VerticesInFace[iv]);
				VerticesInFace.erase(VerticesInFace.begin()+iv);
				iv--;
			}
		}

		//Check triplets of vertices of the face
		std::vector<long> VerticesBestFace (3, 0);
		std::vector<double> CoefBestFace (4, 0.);
		CheckTriplets(DDBB, VerticesInFace, EvaluatedVerticesInFace, EvaluatedVertices,
					  VerticesBestFace, CoefBestFace);

		//Use the plane defined by the best triplet
		//to relocate the remaining vertices of the face to be placed on it
		double D= CoefBestFace[3];
		double C= CoefBestFace[2];
		assert( !_isnan(C) );
		double B= CoefBestFace[1];
		double A= CoefBestFace[0];

		//Update coordinates of vertices
		for (long i=0; i<(long)VerticesInFace.size(); i++){
			if (VerticesInFace[i]!=VerticesBestFace[0] && 
				VerticesInFace[i]!=VerticesBestFace[1] && 
				VerticesInFace[i]!=VerticesBestFace[2]){

				CVertex Vertice= DDBB->GetVertex(VerticesInFace[i]);
				Vertice.z=(-D-A*Vertice.x-B*Vertice.y)/C;
				//assert(std::fabs(Vertice.z)<10000);
				DDBB->SetVertex(VerticesInFace[i], Vertice);
			}
		}

		//Update the list of evaluated vertices
		for (long i=0; i<(long)VerticesInFace.size(); i++){
			if (!IsValueInVector(VerticesInFace[i], EvaluatedVertices))
				EvaluatedVertices.push_back(VerticesInFace[i]);
		}
	}
}


void RefineFacePlanarity(CDB_Model *DDBB)
// Analize each 3D face of the B-Rep model to enforce its planarity as follows:
//	- Look for the face with the greatest number
//	  of edges (not yet evaluated)

//	- Parse all possible planes defined on the face 
//	  by sequentially using every triplet of its vertices

//	- Get the best plane on the list 
//	  (the one that minimizes the distances of the remaining vertices)

//	- Relocate the remaining vertices to fit them in the plane

//	- Repeat until all non-trivial faces have been visited
//	  (triangular faces are trivially planar)
{

	//Select faces to parse
	//---------------------
	std::vector<long> FacesToParse;
	std::vector<long> SizesFacesToParse;
	for (long i=0; i<DDBB->GetSizeFaces(); i++){
		CFace FaceI= DDBB->GetFace(i);
		long sizeI= FaceI.GetSizeEdgesInFace();
		if (sizeI > 3){
			//Faces with 3 vertices are trivially flat
			//Hence, only faces with more than 3 vertices are analyzed...
			//if they are not flat enough
			if(!IsFacePlanar(DDBB, i, 0.01)){           //Threshold to be better adjusted!!
				FacesToParse.push_back(i);
				SizesFacesToParse.push_back(sizeI);
			}
		}
	}

	//Reorder to priorityze faces with more vertices
	SortMajorFirst(SizesFacesToParse, FacesToParse);

	//Consecutively parse the faces
	//by relocating all their not yet relocated vertices
	//--------------------------------------------------
	RefineFaces (DDBB, FacesToParse);
}


void RefineFaceAlignment(CDB_Model *DDBB)
//The approach detects faces that are "nearly" parallel to
//reference planes of the main reference system

//This is done by detecting normals "nearly" parallel to
//one out of the three main axes

//Parallelism is reinforced by forcing all the vertices to
//be at the same distance from that reference plane

//This is done by calculating the average value of the correspongin coordinate (x, y or z)
//and then applying this average value to 
//all the corresponding coordinates of all the vertices of the face

//WARNING: This calculation only makes sense for normalons
//represented in its main reference system (not in the inflation system)
{
	double Gap= 0.8;   //Configure here the maximum variation to be interpreted as involuntary deviation

	for(long i=0; i < DDBB->GetSizeFaces(); i++) {
		CFace currentFace= DDBB->GetFace(i);
		CVector normal_i= GetFaceNormal(DDBB, i, false);

		if(std::fabs(normal_i.x) > Gap){
			//Face is made parallel to the YZ plane
			std::vector<long> vertices= DDBB->GetAllVerticesInFace(DDBB->GetFace(i));
			long numV= vertices.size();
			double Average= 0.;
			for(long j=0; j<numV; j++) {
				CVertex Vertice= DDBB->GetVertex(vertices[j]);
				Average+= Vertice.x;
			}
			Average/= numV;

			for(long j=0; j<numV; j++) {
				CVertex Vertice= DDBB->GetVertex(vertices[j]);
				Vertice.x= Average;
				DDBB->SetVertex(vertices[j], Vertice);
			}
		}

		else if(std::fabs(normal_i.y) > Gap){
			//Face is made parallel to the XZ plane
			std::vector<long> vertices= DDBB->GetAllVerticesInFace(DDBB->GetFace(i));
			long numV= vertices.size();
			double Average= 0.;
			for(long j=0; j<numV; j++) {
				CVertex Vertice= DDBB->GetVertex(vertices[j]);
				Average+= Vertice.y;
			}
			Average/= numV;

			for(long j=0; j<numV; j++) {
				CVertex Vertice= DDBB->GetVertex(vertices[j]);
				Vertice.y= Average;
				DDBB->SetVertex(vertices[j], Vertice);
			}
		}

		else if(std::fabs(normal_i.z) > Gap){
			//Face is made parallel to the XY plane
			std::vector<long> vertices= DDBB->GetAllVerticesInFace(DDBB->GetFace(i));
			long numV= vertices.size();
			double Average= 0.;
			for(long j=0; j<numV; j++) {
				CVertex Vertice= DDBB->GetVertex(vertices[j]);
				Average+= Vertice.z;
			}
			Average/= numV;

			for(long j=0; j<numV; j++) {
				CVertex Vertice= DDBB->GetVertex(vertices[j]);
				Vertice.z= Average;
				DDBB->SetVertex(vertices[j], Vertice);
			}
		}
	}
}


//public:
void RefineModelBrep(CReferDoc* pDoc)
//Refines the 3D model Brep
//Includes two types of refinement:
//	- Enforcing planarity of faces
//	- Enforcing alignment of faces parallel to main reference planes
{
	//Refines the 3D model by reinforcing planarity of faces
	//(This is only feasible is faces have been calculated)
	//-------------------------------------------------------
	if(ReferApp.R3D.RefineFaces){
		if (pDoc->DB_Brep_Model->GetSizeFaces()==0)
			AfxMessageBox("Enforcing faces planarity requires faces calculated in advance");
		else{
			RefineFacePlanarity(pDoc->DB_Brep_Model);
		}
	}

	//Refines the 3D model by better aligning its faces
	//with the main reference planes
	//-------------------------------------------------
	if(ReferApp.R3D.AlignFaces){
		if(!ReferApp.Cu.MA.MainAngles.empty()){
			CCueMainAxes MainAxes;
			MainAxes.TransformToMainAxes(pDoc->DB_Brep_Model, false);

			RefineFaceAlignment(pDoc->DB_Brep_Model);

			MainAxes.TransformFromMainAxes(pDoc->DB_Brep_Model, false);
		}
		else{
			AfxMessageBox("Alignment of faces requires Main Axes calculated in advance");
		}
	}


	//Updates the window of the 3D model
	CReferDoc::GetDoc()->RefreshWindowModelBrep();
}
