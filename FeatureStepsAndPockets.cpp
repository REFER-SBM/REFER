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
#include <cmath>
#include <vector>

#include "Tools_Vector.h"
#include "Tools_Geometry.h"

#include "DB_Model.h"
#include "DB_Model_Maths.h"
#include "CueSubGraphs.h"
#include "Refer.h"

#include "FeatureStepsAndPockets.h"


void CFeatureStepsAndPockets::DetermineFaceSubgraphs(CDB_Model *DDBB)
//Determines the subgraph for each face of the model DDBB

//The function assumes that Subgraphs have been found first
{
	SubgraphForFace.assign(DDBB->GetSizeFaces(), 0);

	if(ReferApp.Cu.SG.NumSubgraphs == 1) return;

	for (long NumFace=0; NumFace<DDBB->GetSizeFaces(); NumFace++){
		CFace Face= DDBB->GetFace(NumFace);
		std::vector<long> VectorEdges= Face.GetAllEdgesInFace();
		long NumGraph= ReferApp.Cu.SG.SubgraphForEdge[VectorEdges[0]];

		//Alternatively, assume that all the edges and vertices
		//belong to the same subgraph than the first edge of the face
		//---------------------------------------------------------------------
		//Check that rest of the edges belong to the same subgraph
		bool EdgesInSubGraph= true;
		for (long j=1; j<(long)VectorEdges.size(); j++){
			if(ReferApp.Cu.SG.SubgraphForEdge[VectorEdges[j]] != NumGraph){
				EdgesInSubGraph= false;
				break;
			}
		}

		//Check that vertices belong to the same subgraph
		std::vector<long> VectorVertices= DDBB->GetAllVerticesInFace(Face);
		bool VerticesInSubGraph= true;
		for (long j=0; j<(long)VectorVertices.size(); j++){
			if(ReferApp.Cu.SG.SubgraphForVertex[VectorVertices[j]] != NumGraph){
				VerticesInSubGraph= false;
				break;
			}
		}

		//Save face subgraph
		if(VerticesInSubGraph && EdgesInSubGraph)
		//---------------------------------------------------------------------
			SubgraphForFace[NumFace]= NumGraph;
	}
}


void CFeatureStepsAndPockets::GetFaceCues(CDB_Model *DDBB)
//Calculate face cues requiered to detect features
{
	//Loop to calculate lengths and angles of edges
	edgeLength.clear();
	edgeAngle.clear();
	for (long i=0; i<DDBB->GetSizeEdges(); i++){
		edgeLength.push_back(DDBB->GetLength2D(i));
		edgeAngle.push_back(DDBB->GetAngle2DX_PI(i, Accuracy));
	}

	//Loop to calculate face areas
	FaceArea.clear();
	SmallToBigFaces.clear();
	for (long i=0; i<DDBB->GetSizeFaces(); i++){
		SmallToBigFaces.push_back(i);
		FaceArea.push_back(GetFaceArea(DDBB, i));
	}
	std::vector<double> FaceAreaToSort= FaceArea;
	SortMinorFirst(FaceAreaToSort, SmallToBigFaces);

	//Loop to search for quadrilateral, slender and parallelogram faces
	long numFaces= DDBB->GetSizeFaces();

	FaceQuadrilateral.assign(numFaces, false);
	FaceSlender.assign(numFaces, false);
	Parallelogram.assign(numFaces, false);
	for (long i=0; i<numFaces; i++){
		CFace Face= DDBB->GetFace(i);
		if(Face.GetSizeEdgesInFace() == 4){
			FaceQuadrilateral[i]= true;

			if(IsASlenderFace(Face,
							  ThresholdShort, ThresholdLong,
							  edgeLength)){
				FaceSlender[i]= true;
			}

			if(IsParallelogram(Face,
							   ThresholdLength,
							   ThresholdAngle,
							   !FaceSlender[i],   //Avoid checking parallelism of short sides
							   edgeLength,
							   edgeAngle)){
				Parallelogram[i]= true;
			}
		}
	}

	//Calculate face subgraphs
	DetermineFaceSubgraphs(DDBB);
}


void CFeatureStepsAndPockets::GetEdgesOfLateralFace(CDB_Model *DDBB, long lateral, long &long1, long &long2, long &short1, long &short2)
//Given a border face of a rib (border), returns:
//		the numbers of the two long edges (long1, long2)
//		the numbers of the two short edges (short1, short2)
{
	//Calculate lengths
	CFace Cara= DDBB->GetFace(lateral);

	if (DDBB->GetLength2D(Cara.GetEdgeInFace(0))+
		DDBB->GetLength2D(Cara.GetEdgeInFace(2)) <
		DDBB->GetLength2D(Cara.GetEdgeInFace(1))+
		DDBB->GetLength2D(Cara.GetEdgeInFace(3))){
		long1= Cara.GetEdgeInFace(1);
		long2= Cara.GetEdgeInFace(3);

		short1= Cara.GetEdgeInFace(0);
		short2= Cara.GetEdgeInFace(2);

	}
	else{
		long1= Cara.GetEdgeInFace(0);
		long2= Cara.GetEdgeInFace(2);

		short1= Cara.GetEdgeInFace(1);
		short2= Cara.GetEdgeInFace(3);
	}
}


bool CFeatureStepsAndPockets::GetBaseFaces(CDB_Model *DDBB, long Numgraph,
										   long lateral, long &base1, long &base2)
//Searches the base faces of the "lateral" face
//Returns false if two base faces are not found

//Returns the numbers of the base faces (base1, base2)
{
	bool bas1= false;
	bool bas2= false;

	base1= base2= -1;

	long long1, long2, short1, short2;
	GetEdgesOfLateralFace(DDBB, lateral, long1, long2, short1, short2);

	for (long j=0; j<(long)DDBB->GetSizeFaces(); j++){
		if(j != lateral){
			if (SubgraphForFace[j] == Numgraph){
				CFace CaraJ= DDBB->GetFace(j);
				std::vector<long> Edges= CaraJ.GetAllEdgesInFace();
				long Position;
				if(!bas1 && IsValueInVector(long1, Edges, Position)){
					base1= j;
					bas1= true;
					if (bas2) break;
				};
				if(!bas2 && IsValueInVector(long2, Edges, Position)){
					base2= j;
					bas2= true;
					if (bas1) break;
				};
			}
		}
	}
	return (bas1 && bas2);
}


bool CFeatureStepsAndPockets::BasesAreSimilar(CDB_Model *DDBB, long lateral, long base1, long base2)
//Returns true if base faces base1 and base2 are quite similar (i.e. homothetic)

//Faces are similar if:
//		They have the same number of vertices
//		Vertices may be paired in pairs whose distance is always nearly the same
//		Vertices may be paired in pairs whose orientation is always nearly the same
{
	//Get base faces
	CFace FaceB1= DDBB->GetFace(base1);
	CFace FaceB2= DDBB->GetFace(base2);

	//Check if they have the same number of edges
	if(FaceB1.GetSizeEdgesInFace() != FaceB2.GetSizeEdgesInFace()) return false;

	//Get all vertices in each face and pair them
	std::vector<long> VertInFace1;
	std::vector<long> VertInFace2;

	long long1, long2, short1, short2;
	GetEdgesOfLateralFace(DDBB, lateral, long1, long2, short1, short2);
	PairVertices(DDBB, short1, short2, base1, base2, VertInFace1, VertInFace2);

	//Check if paired vertices of lateral faces
	//are at nearly the same distance from each other
	if(!SimilarDistances(DDBB, ThresholdSimilarDistance, VertInFace1, VertInFace2))
		return false;

	//Check if paired edges (defined by consecutive vertices) of lateral faces
	//are at nearly the same orientation from each other
	if(!SimilarAngles(DDBB, ThresholdSimilarAngle, VertInFace1, VertInFace2))
		return false;

	return true;
}


//double CFeatureStepsAndPockets::AreaTriangle(CVertex v1, CVertex v2, CVertex v3)
////Returns the area of the triangle defined by v1, v2 and v3
//{
//	return ((v1.x*v2.y)+(v2.x*v3.y)+(v3.x*v1.y)-(v1.x*v3.y)-(v2.x*v1.y)-(v3.x*v2.y))/2;
//}


bool CFeatureStepsAndPockets::BasesHaveMoreArea(CDB_Model *DDBB, long lateral, long base1, long base2)
//Returns true if the areas of faces base1 and base2 are bigger than the area of lateral face
//False otherwise
{
	return (min(GetFaceArea(DDBB, base1), GetFaceArea(DDBB, base2)) > 
			ThresholdAreaBases*GetFaceArea(DDBB, lateral));
}


bool CFeatureStepsAndPockets::IsAClosedVolume(CDB_Model *DDBB, long lateral, long base1, long base2)
//Returns true if all the vertices of base1 are conected to their paired vertices of base2
//False otherwise

//Vertices of both bases are paired following connections of lateral face
{
	std::vector<long> Vertices1, Vertices2;
	long long1, long2, short1, short2;
	GetEdgesOfLateralFace(DDBB, lateral, long1, long2, short1, short2);
	PairVertices(DDBB, short1, short2, base1, base2, Vertices1, Vertices2);

	for(long i= 0; i<(long)Vertices1.size(); i++){
		if(DDBB->GetEdgeByVerticesNums(Vertices1[i], Vertices2[i])== -1) return false;
	}
	return true;
}


bool CFeatureStepsAndPockets::GetLateralFace(CDB_Model *DDBB, long Numgraph, 
											 std::vector<long> faces, 
											 long &lateral, long &base1, long &base2)
//Searches for lateral Faces:
//	-Searches first for cuadrilateral and narrow face
//	 (Candidate lateral faces of a step or pocket)
//	-Searches then for two base faces similar to each other
//  -Areas of base faces are bigger than lateral face

//Returns true if a lateral face is found, false otherwise
{
	//Loop to search for a thin wall
	for (long i=0; i<DDBB->GetSizeFaces(); i++){
		if (SubgraphForFace[i] == Numgraph){
			if (faces[i]== 0){
				if(FaceSlender[i]){
					lateral= i;
					if (GetBaseFaces(DDBB, Numgraph, lateral, base1, base2)){

						if (BasesAreSimilar(DDBB, lateral, base1, base2)){
							if(BasesHaveMoreArea(DDBB, lateral, base1, base2)){
								if (IsAClosedVolume(DDBB, lateral, base1, base2)){
									return true;
								}
							}
						}

					}
				}
			}
		}
	}

	return false;
}


bool CFeatureStepsAndPockets::GetOuterFaceContainingBaseFace(CDB_Model *DDBB, long numgraph, long numFace,
															 std::vector<long> &outerfacescandidates)
//Returns true if face "Face" is fully contained
//in one or more of the faces of another subgraph (different from numgraph)

//The list of candidate containers ("outers") is returned
{
	bool continuar= false;

	//Obtaining the vertices of the base face
	CFace testFace= DDBB->GetFace(numFace);
	std::vector<long> VerticesBase= DDBB->GetAllVerticesInFace(DDBB->GetFace(numFace));

	//Visit all faces to find candidate containers
	for (long j=0; j< (long) DDBB->GetSizeFaces(); j++){
		if (SubgraphForFace[j] != numgraph){  //Exclude faces from subgraph numgraph

			//Calculate if current face is a valid candidate outer face
			CFace outerFace= DDBB->GetFace(j);

			std::vector<POINT2D> polygonOuterFace;
			DDBB->SetFaceAsPolygon(j, polygonOuterFace);

			bool inside= true;

			for (long v=0; v<(long)VerticesBase.size(); v++){
				CVertex VertexBase= DDBB->GetVertex(VerticesBase[v]);

				POINT2D pbase(VertexBase.x, VertexBase.y);

				if(!isPointInPolygon(polygonOuterFace, pbase)){
					inside= false;
					break;
				}
			}

			//Further checking for candidate container faces
			if (inside) {
				continuar= true;
				//Searching if the face is already in the list
				bool alreadyIncluded= false;
				for(long i= 0; i< (long) outerfacescandidates.size(); i++){ 
					if (j== outerfacescandidates[i])
						alreadyIncluded= true;
				}

				if (!alreadyIncluded){
					//checking that no intersections exist between inner and outer face edges
					std::vector<long> vectorI= testFace.GetAllEdgesInFace();
					std::vector<long> vectorO= outerFace.GetAllEdgesInFace();
					std::vector<long> intersectededges;
					IntersectionBetweenVectorsOfEdges(DDBB, vectorI, vectorO, intersectededges);

					if (intersectededges.size()==0)
						outerfacescandidates.push_back(j);
				}
			}
		}//end if
	}//end for

	if((outerfacescandidates.size()> 0) && (continuar))
		return true;

	return false;
}


bool CFeatureStepsAndPockets::Intersection(CDB_Model *DDBB, CVertex P1, CVertex P2, long out)
{
	CEdge edge= DDBB->GetEdge(out);
	CVertex P3= DDBB->GetVertex(edge.Tail);
	CVertex P4= DDBB->GetVertex(edge.Head);

	//if (P1-P3)x(P4-P3) have different sign from (P2-P3)x(P4-P3) 
	// (P3-P1)x(P2-P1) have different sign from (P4-P1)x(P2-P1)
	// then intersection exists
	double prueba1, prueba2, prueba3, prueba4;

	prueba1= (P1.x-P3.x)*(P4.y-P3.y)-(P4.x-P3.x)*(P1.y-P3.y);
	prueba2= (P2.x-P3.x)*(P4.y-P3.y)-(P4.x-P3.x)*(P2.y-P3.y);
	prueba3= (P3.x-P1.x)*(P2.y-P1.y)-(P2.x-P1.x)*(P3.y-P1.y);
	prueba4= (P4.x-P1.x)*(P2.y-P1.y)-(P2.x-P1.x)*(P4.y-P1.y);

	if(((prueba1*prueba2)<0)&&((prueba3*prueba4)<0)) return true;

	return false;
}


void CFeatureStepsAndPockets::FindShortesEdgesInGraph(CDB_Model *DDBB, long Numgraph, std::vector<long> &shortedges, std::vector<long> faces)
{
	std::vector<long> lateralfaces;
	//Second all short edges of the inner subgraph will be stored in the vector "shortedges"
	//first lateral faces are looked for
	for (long j=0; j< (long)faces.size(); j++){
		if((SubgraphForFace[j] == Numgraph) && (faces[j]== 1))
			lateralfaces.push_back(j);
	}
	//Next short edges are looked for in these lateral faces
	for (long k=0; k<(long) lateralfaces.size(); k++){
		long void1, void2, short1, short2;

		GetEdgesOfLateralFace(DDBB, lateralfaces[k], void1, void2, short1, short2);
		if(!IsValueInVector(short1, shortedges))
			shortedges.push_back(short1);
		if(!IsValueInVector(short2, shortedges))
			shortedges.push_back(short2);
	}
}


void CFeatureStepsAndPockets::IntersectionBetweenVectorsOfEdges(CDB_Model *DDBB,  
																std::vector<long> vectorI, 
																std::vector<long> vectorO,
																std::vector<long> &edgesintersection)
{
	for(long i=0; i<(long) vectorI.size(); i++){
		long ei= vectorI[i];
		CEdge edge1= DDBB->GetEdge(ei);

		CVertex P1= DDBB->GetVertex(edge1.Tail);
		CVertex P2= DDBB->GetVertex(edge1.Head);

		for(long j=0; j<(long) vectorO.size(); j++){
			if (Intersection(DDBB, P1, P2, vectorO[j]) &&
				!IsValueInVector(vectorO[j], edgesintersection)){
				edgesintersection.push_back(vectorO[j]);
			}
		}
	}
}


void CFeatureStepsAndPockets::GetOuterEdgesIntersecting(CDB_Model *DDBB, long Numgraph, long outer,
														long containerface, std::vector<long> faces, 
														std::vector<long> &edgesintersection)
{
	CFace Outerface= DDBB->GetFace(containerface);
	//First all edges of outer graph will be stored in the vector "outeredges"
	std::vector<long> outeredges= Outerface.GetAllEdgesInFace();


	std::vector<long> shortedges;

	FindShortesEdgesInGraph(DDBB, Numgraph, shortedges, faces);

	IntersectionBetweenVectorsOfEdges(DDBB, shortedges, outeredges, edgesintersection);

	//Now we check if any edge of the outer graph intersect the short edges of lateral faces
	for(long corto=0; corto<(long) shortedges.size(); corto++){
		long short1= shortedges[corto];
		CEdge edge1= DDBB->GetEdge(short1);

		CVertex P1= DDBB->GetVertex(edge1.Tail);
		CVertex P2= DDBB->GetVertex(edge1.Head);

		for(long outedge=0; outedge<(long) outeredges.size(); outedge++){
			if (Intersection(DDBB, P1, P2, outeredges[outedge]) &&
				!IsValueInVector(outeredges[outedge], edgesintersection)){
				edgesintersection.push_back(outeredges[outedge]);
			}
		}
	}
}


long CFeatureStepsAndPockets::LookingForParallelFaces(CDB_Model *DDBB, long Numgraph,
													  std::vector<long> bases, std::vector<long> outercandidates, 
													  std::vector<long> &containercandidates)
{
	long face= -1;

	//The first condition consists on checking if the edges of the bases are parallel to some outer face
	for (long bas= 0; bas< (long) bases.size(); bas++){

		CFace base= DDBB->GetFace(bases[bas]);

		std::vector<long> PositionEdgesInBase= base.GetAllEdgesInFace();

		//First we check whether all edges of the bases are parallel to the edges of one of the outer face. 
		//This case serves only for polyhedrical shapes
		for (long candidate= 0; candidate< (long) outercandidates.size(); candidate++){
			long posit= outercandidates[candidate];
			CFace facecandidate= DDBB->GetFace(posit);

			std::vector<long> PositionEdgesInOuterface= facecandidate.GetAllEdgesInFace();
			//First we check if it doesn't exist any intersection between edges
			//in inner face and edges in outer face
			std::vector<long> intersectededges;
			IntersectionBetweenVectorsOfEdges(DDBB, PositionEdgesInBase, PositionEdgesInOuterface, intersectededges);
			if (intersectededges.size()!=0) continue;

			long parallel= 0;
			for (long j= 0; j< (long) PositionEdgesInBase.size(); j++){
				long basej= PositionEdgesInBase[j];
				for (long k= 0; k< (long) PositionEdgesInOuterface.size(); k++){
					long outerk= PositionEdgesInOuterface[k];

					double anglej= DDBB->GetAngle2DX_PI(basej, Accuracy);
					double anglek= DDBB->GetAngle2DX_PI(outerk, Accuracy);
					double angle= min((std::fabs(anglej-anglek)), std::fabs((std::fabs(NUMBER_PI-anglej)+anglek)));
					//if the minimum difference between both angles is smaller than ThresholdAngle
					//then we consider the lines as parallel and we stop looking for another parallel line
					if (angle < ThresholdAngle) {
						parallel++;
						break;
					}
				}//end for of outer edges
			}// end for of base edges

			if (parallel== (long) PositionEdgesInBase.size()){
				if(!IsValueInVector(posit, containercandidates))
					containercandidates.push_back(posit);
			}
		}//end for

	}//end for
	if (containercandidates.size()==1)	face= containercandidates[0];
	//if several candidate outerfaces have their edges parallel to the base faces, then 
	//later we will choose the one which its centroid is closer to a base face
	if (containercandidates.size()>1) face= -2;

	return face;
}


long CFeatureStepsAndPockets::LookingForPerpendicularityEdges(CDB_Model *DDBB, long Numgraph,
								   std::vector<long> bases, std::vector<long> outercandidates, 
								   std::vector<long> faces, std::vector<long> &containercandidates)
{
	//Next we check wheter the shortest edges are parallel to some edge of the outer face. In this case
	//the face doesn't contain the feature
	//This case serves only for polyhedrical shapes
	long perpend= 0;
	long face=-1;

	//Now we get the shortedges of the inner graph;
	std::vector<long> shortedges;
	FindShortesEdgesInGraph(DDBB, Numgraph, shortedges, faces);

	for (long j= 0; j< (long) outercandidates.size(); j++){
		long posit= outercandidates[j];
		CFace facecandidate= DDBB->GetFace(posit);

		std::vector<long> PositionEdgesInOuterface= facecandidate.GetAllEdgesInFace();

		//Next we compare the angles of the shortest edges of the inner graph with the outer edges
		for( long s= 0; s< (long) shortedges.size(); s++){
			long shortedg= shortedges[s];

			for (long k= 0; k< (long) PositionEdgesInOuterface.size(); k++){
				long outerk= PositionEdgesInOuterface[k];

				double anglesh= DDBB->GetAngle2DX_PI(shortedg, Accuracy);
				double anglek= DDBB->GetAngle2DX_PI(outerk, Accuracy);
				double angle= min((std::fabs(anglesh-anglek)), std::fabs((std::fabs(NUMBER_PI-anglesh)+anglek)));
				//if the minimum difference between both angles is smaller than ThresholdAngle
				//then we consider the lines as parallel and we stop looking for another parallel line
				if ((angle >((NUMBER_PI/2)-ThresholdAngle))&&(angle<((NUMBER_PI/2)+ThresholdAngle))) {
					perpend++;
					break;
				}
			}//end for of outer edges
		}//end for of shortedges

		if (perpend== (long)shortedges.size()){
			containercandidates.push_back(posit);
		}
		perpend= 0;
	}//end for of candidates 
	if (containercandidates.size()==1)	face= containercandidates[0];
	if (containercandidates.size()>1)	face= -2;
		
	return face;
}

double CFeatureStepsAndPockets::GetBoundingBoxArea(CDB_Model *DDBB, std::vector<long> Vertices)
{
	CVerticesSet SetOfVertices;

	for(long r=0; r<(long) Vertices.size(); r++){
		//Raquel: solución provisional ¿por qué se modifican los vértices
		//de contorno?
		if(Vertices[r]>-1){
			CVertex Vertex= DDBB->GetVertex(Vertices[r]);
			SetOfVertices.AddVertex(Vertex);
		}
	}

	double Xmin, Ymin, Zmin, Xmax, Ymax, Zmax;
	SetOfVertices.GetBoundingBox(Xmin, Ymin, Zmin, Xmax, Ymax, Zmax);

	double Area= std::fabs(Xmax-Xmin)*std::fabs(Ymax-Ymin);

	return Area;
}

void CFeatureStepsAndPockets::DiscardingExternalSubgraphs(CDB_Model *DDBB, std::vector<long> &outerfacescandidates)
{
	//We discard the outercandidates which belong to the outermost subgraphs. It is more likely that the 
	//feature is contained by the smallest subgraph
	bool continuar= true;

	while (continuar){
		long PositionFace= -1;
		double AreaA= 0;
		double AreaB= 0;

		for (long i=0; i<(long)outerfacescandidates.size(); i++){
			long ContainerSubgraph= SubgraphForFace[outerfacescandidates[i]];

			if(SubgraphAreas[ContainerSubgraph]== 0){
				AreaB= GetBoundingBoxArea(DDBB, ReferApp.Cu.Pe.PerimeterVertices[ContainerSubgraph]);
				SubgraphAreas[ContainerSubgraph]= AreaB;
			}
			else
				AreaB= SubgraphAreas[ContainerSubgraph];

			if(AreaA==0){
				AreaA= AreaB;
				PositionFace= i;
			}
			else{
				if(AreaB> AreaA){
					AreaA= AreaB;
					PositionFace= i;
					continuar= false;
				}
				if(AreaB< AreaA){
					continuar= false;
					break;
				}
			}
		}

		if(!continuar){
			outerfacescandidates.erase(outerfacescandidates.begin()+PositionFace, outerfacescandidates.begin()+PositionFace+1); 
			continuar= true;
		}
		else
			continuar= false;

	}
}

void CFeatureStepsAndPockets::ObtainingMainBaseAndOuterFace(CDB_Model *DDBB, long &baseface, long &containerface,
															std::vector<long> bases, std::vector<long> containercandidates)
{
	//if containerface==-2, then several outercandidates fulfill some previous criterion
	//then we will choose the one whose centroid is closer to a base face
	double distmin= 0;

	for(long j=0; j<(long)containercandidates.size(); j++)
	{
		long out= containercandidates[j];
		long bas= -1;
		if (bases.size()== 1)
			bas= bases[0];
		if (bases.size()>1)
			bas= MainBase(DDBB, out, bases);

		CVertex outcentroid= GetFaceCentroid(DDBB, out);
		CVertex bascentroid= GetFaceCentroid(DDBB, bas);
		double dist= MODULE_2D(outcentroid, bascentroid);
		if (j==0){
			containerface= out;
			baseface= bas;
			distmin= dist;
		}
		else{
			if (dist<distmin){
				containerface= out;
				baseface= bas;
				distmin= dist;
			}
		}
	}// end for
}


long CFeatureStepsAndPockets::LookingForParallelismShortestEdges(CDB_Model *DDBB, long Numgraph,
																 std::vector<long> bases, std::vector<long> outercandidates, 
																 std::vector<long> faces, std::vector<long> &containercandidates)
{
	//Next we check wheter the shortest edges are parallel to some edge of the outer face. In this case
	//the face doesn't contain the feature
	//This case serves only for polyhedrical shapes
	long parallel= 0;
	long face=-1;
	//Now we get the shortedges of the inner graph;
	std::vector<long> shortedges;
	
	FindShortesEdgesInGraph(DDBB, Numgraph, shortedges, faces);
	std::vector<long> falsecandidates;

	for (long candidate= 0; candidate< (long) outercandidates.size(); candidate++){
		long posit= outercandidates[candidate];
		CFace facecandidate= DDBB->GetFace(posit);

		std::vector<long> PositionEdgesInOuterface= facecandidate.GetAllEdgesInFace();

		//Next we compare the angles of the outer edges with the shortest edges of the inner graph
		for( long s= 0; s< (long) shortedges.size(); s++){
			long shortedg= shortedges[s];

			for (long k= 0; k< (long) PositionEdgesInOuterface.size(); k++){
				long outerk= PositionEdgesInOuterface[k];

				double anglesh= DDBB->GetAngle2DX_PI(shortedg, Accuracy);
				double anglek= DDBB->GetAngle2DX_PI(outerk, Accuracy);
				double angle= min((std::fabs(anglesh-anglek)), std::fabs((std::fabs(NUMBER_PI-anglesh)+anglek)));
				//if the minimum difference between both angles is smaller than ThresholdAngle
				//then we consider the lines as parallel and we stop looking for another parallel line
				if (angle < ThresholdAngle) {
					parallel++;
					break;
				}
			}//end for of outer edges
		}//end for of shortedges
			
		if (parallel== (long)shortedges.size()){
			falsecandidates.push_back(posit);
		}
	}//end for of candidates 

	for(long i=0; i<(long)outercandidates.size(); i++){
		long candid= outercandidates[i];
		if (!IsValueInVector(candid, falsecandidates))
			containercandidates.push_back(candid);
	}
		
	if (containercandidates.size()== 1)	face= outercandidates[0];
	if (containercandidates.size()>1) face= -2;
	return face;
}


long CFeatureStepsAndPockets::MainBase(CDB_Model *DDBB, long containerface, std::vector<long> bases)
{
	//if at this moment we still have more than 1 face as outer candidate
	//we will apply the last condition based on the calculus of the minimum distance between the barycenters
	CFace faceouter= DDBB->GetFace(containerface);
	CVertex faceouterBarycenter= GetFaceCentroid(DDBB, containerface);

	double dist2;
	long pos;

	//checking if it doesn't exist any intersection between edges
	//in inner face and edges in outer face
	for(long b=0; b<(long)bases.size(); b++){
		CFace Base= DDBB->GetFace(bases[b]);
		CFace OuterFace= DDBB->GetFace(containerface);
		std::vector<long> vectorI= Base.GetAllEdgesInFace();
		std::vector<long> vectorO= OuterFace.GetAllEdgesInFace();
		std::vector<long> intersectededges;
		IntersectionBetweenVectorsOfEdges(DDBB, vectorI, vectorO, intersectededges);
		if (intersectededges.size()!=0) bases.erase(bases.begin()+b, bases.begin()+b+1);
	}

	if(bases.size()==1) pos=bases[0];
	//The second conditiion consists on taking as main base the closer one to the barycenter of the 
	//outerface
	else{
		for (long fb= 0; fb< (long)bases.size(); fb++){
			CFace faceb= DDBB->GetFace(bases[fb]);

			CVertex Baryc= GetFaceCentroid(DDBB, bases[fb]);

			double dist1= MODULE_2D(GetFaceCentroid(DDBB, bases[fb]),
									faceouterBarycenter);

			if (fb== 0){
				pos= bases[fb];
				dist2= dist1;
			}
			else if(dist1< dist2){
				pos= bases[fb];
				dist2= dist1;
			}
		}//end for
	}//end else
	return pos;
}


//long CFeatureStepsAndPockets::LookForFaceInDDBB(CDB_Model *DDBB, CDB_Model Graph, CFace iface)
////Now we look for the position of the face "iface" in the general data base
//{
//	std::vector<long> ivertices;
//	Graph.GetAllVerticesInFace(iface, ivertices);
//
//	long f= 0;
//	bool continuar= true;
//	long DDBBfaceposition= -1;
//
//	while (f< (long)DDBB->GetSizeFaces() && continuar){
//		CFace DBFace= DDBB->GetFace(f);
//
//		if (DDBB->GetSizeVerticesInFace(f) == ivertices.size()){
//			long j= 0;
//			bool follow= true;
//			while (j< (long)ivertices.size() && follow){
//				CVertex ivertex= Graph.GetVertex(ivertices[j]);
//				long numvert;
//				if(DDBB->IsVertex(ivertex, numvert)){
//					if(!DDBB->IsVertexInFace(DBFace, numvert))
//						follow= false;
//				}
//				j++;
//			}
//
//			if (j== DDBB->GetSizeVerticesInFace(f)){
//				continuar= false;
//				DDBBfaceposition= f;
//			}
//
//		}//end if
//		f++;
//
//	}//end while
//	return DDBBfaceposition;
//}


void CFeatureStepsAndPockets::LoadBoundaryEdges()
//Relabels all perimeter edges as "Boundary"

//This is only done with tags that have the default value of "blank"
{
	for(long i=1; i<ReferApp.Cu.SG.NumSubgraphs+1; i++){
		//Raquel: hay que comprobar que el primer perímetro es del dibujo general, no de los subgrafos, por eso i=1
		for(long j=0; j<(long)ReferApp.Cu.Pe.PerimeterEdges[i].size(); j++){
			if(ReferApp.Cu.La.LabelsInEdges[ReferApp.Cu.Pe.PerimeterEdges[i][j]]== BLANK)
				ReferApp.Cu.La.LabelsInEdges[ReferApp.Cu.Pe.PerimeterEdges[i][j]]= BOUNDARY;
		}
	}
}


void CFeatureStepsAndPockets::BarycenterAdjoiningFaces(CDB_Model *DDBB, long containerface, CVertex &v1)
//Calculates v1 as the barycenter of the outer face (containerface) and its adjoining faces
{
	CFace Face0= DDBB->GetFace(containerface);

	std::vector<long> EdgesFace0= Face0.GetAllEdgesInFace();

	CVertex baryFace0= GetFaceCentroid(DDBB, containerface);
	double bary_x= baryFace0.x;
	double bary_y= baryFace0.y;

	double qty= 1;
	std::vector<long> faceslist;

	for (long i=0; i<(long)EdgesFace0.size(); i++){
		long edgei= EdgesFace0[i];
		EDGE_LABEL EdgeLabel= ReferApp.Cu.La.LabelsInEdges[edgei];

		std::vector<long> faces= DDBB->GetFacesSharingEdge(edgei);

		long adjface;
		if (faces[0] == containerface)
			adjface= faces[1];
		else
			adjface= faces[0];

		CVertex baryFacei= GetFaceCentroid(DDBB, adjface);
		//If the adjoining face shares a CONVEX or BOUNDARY edge, then first we check the 
		//adjoining face has not been used before for calculating the barycenter
		//We avoid to take into account the same face several times
		if ((EdgeLabel!= CONCAVE) && (EdgeLabel!= BLANK)){
			if (!IsValueInVector(adjface, faceslist)){
				bary_x += baryFacei.x;
				bary_y += baryFacei.y;
				faceslist.push_back(adjface);
				qty++;
			}
		}
		//If the adjoining face shares a CONCAVE edge, first we check the adjoining face
		//has not been used before, in that case, we delete the barycenter took into account previously
		if (EdgeLabel == CONCAVE){
			if (IsValueInVector(adjface, faceslist)){
				bary_x -= baryFacei.x;
				bary_y -= baryFacei.y;
				qty--;
			}
			else
				faceslist.push_back(adjface);
		}
		faces.clear();
	}

	v1.x= (bary_x/qty);
	v1.y= (bary_y/qty);
}


void CFeatureStepsAndPockets::Output_Step_Pocket(CDB_Model *DDBB)
//Returns a set of steps and pockets.

//The subgraph that contains most of the boundary edges is labeled as external subgraph.
//The algorithm takes as candidates those subgraphs 
//detected and located within the external subgraph (internal subgraphs).
{
	//Get outer subgraph
	long Outer= ReferApp.Cu.SG.Outer;

	SubgraphAreas.assign(ReferApp.Cu.SG.NumSubgraphs, 0);
	//Raquel: Mientras no tenemos información de las etiquetas de las aristas
	//cargamos la información de los contornos para conocer las aristas BOUNDARY
	LoadBoundaryEdges();

	for (long NumGraph= 0; NumGraph< ReferApp.Cu.SG.NumSubgraphs; NumGraph++){
		if(NumGraph == Outer)
			continue;  //We only look for Steps/Pockets in INNER subgraphs

		//Search for steps and pockets
		//----------------------------
		long lateral, base1, base2;
		std::vector<long> faces;
		faces.assign(DDBB->GetSizeFaces(), 0);

		double merits= 0;
		bool pocket = false;
		bool step= false;
		lateral= base1= base2= -1;

		while(GetLateralFace(DDBB, NumGraph, faces, lateral, base1, base2)){
			if(faces[base1] == 0 && faces[base2] == 0){
				//New Step/Pocket
				//---------------
				faces[lateral]= 1;
				faces[base1]= -1;
				faces[base2]= -1;

				//Assign an average figure of merit to the step
				merits= 0.51;
			}
			else{
				if(faces[base1] != 0 && faces[base2] != 0){
				//Add the new lateral to the step where it belongs
				//------------------------------------------------
					faces[lateral]= 1;
				}
			}
		}

		//Count faces
		long qtyfacesLateral= 0;
		long qtyfacesSG= 0;
		for (long i=0; i<(DDBB->GetSizeFaces()); i++){
			if(SubgraphForFace[i] == NumGraph){
				qtyfacesSG++;
				if (faces[i] > 0)
					qtyfacesLateral++;
			}
		}

		if (qtyfacesLateral == qtyfacesSG-2)
			//Increase the merit for features where 
			//all faces in the subgraph but two are lateral faces
			merits += 0.10;

		if (qtyfacesLateral == 0)
			//Cancel if no lateral faces have been found
			continue;

//--------------------------------------------------------------------------------------------
		//If at least one base face is contained entirely in some face of the outer subgraph
		//then probably the feature is a step or a pocket
		std::vector<long> bases;
		std::vector<long> outerfacescandidates;

		if (GetOuterFaceContainingBaseFace(DDBB, NumGraph, base1, outerfacescandidates)) 
			bases.push_back(base1);

		if (GetOuterFaceContainingBaseFace(DDBB, NumGraph, base2, outerfacescandidates))
			bases.push_back(base2);

		//If no bases are fully contained in an outer face,
		//then probably we have a boss/hole
		//then some lateral face must be fully contained  in an outer face 
		bool continuar= true;
		if (bases.size()== 0){
			faces.clear();
			continuar= false;
		}

//---------------------------------------------------------------------------------------

		long containerface=-1;
		long baseface= -1;
		std::vector<long> containercandidates;

		if (outerfacescandidates.size()==1){
			containercandidates.push_back(outerfacescandidates[0]);
			containerface= -2;
		}

		if (continuar){
			//If there is more than one outer face as candidate, it is necessary to choose the
			//real face which contain the feature. 
			//If the edges of the basis are parallel to the edges of an outerfacecandidate, 
			//then we suppose the bases are contained in that outerfacecandidate
			//Else, if the shortest edges are parallel to any outerfacecandidate, 
			//then it will not be the container face.
			if(containerface==-1)
				DiscardingExternalSubgraphs(DDBB, outerfacescandidates);

			if(containerface==-1)
				containerface= LookingForParallelFaces(DDBB, NumGraph, bases, 
											outerfacescandidates, containercandidates);

			if(containerface==-1)
				containerface= LookingForParallelismShortestEdges(DDBB, NumGraph, bases, 
											outerfacescandidates, faces, containercandidates);

			if(containerface==-1)
				containerface= LookingForPerpendicularityEdges(DDBB, NumGraph, bases, 
											outerfacescandidates,faces, containercandidates);
		
			if(containerface!= -1)
				ObtainingMainBaseAndOuterFace(DDBB, baseface, containerface, bases, containercandidates);

			if(containerface== -1)
				break;
		//}// primer end de continuar
		//-------------------------------------------------------------------------------
		//Now, it is supposed we have only 1 outercandidate and 1 base face
		//Next algorithm looks for whether edges of outer face which contains basis faces
		//intersect short edges of the inner graph
		//If these outer edges are "boundary" or "concave" edges then the feature is a step
		//If these outer edges are all "convex" edges then the feature is a pocket
		//this part only should work if labelling has been made
		std::vector<long> edgesintersection;
		long ContainerGraph= SubgraphForFace[containerface];

		GetOuterEdgesIntersecting(DDBB, NumGraph, ContainerGraph, containerface, 
								faces, edgesintersection);

		long ii=0;
		while ((continuar)&&(ii<(long)edgesintersection.size())){
			long edgeintersecting= edgesintersection[ii];
			if((ReferApp.Fe.RS.Slots.size()>0)&&(ReferApp.Fe.RS.Slots[containerface]!= 0))
				ReferApp.Cu.La.LabelsInEdges[edgeintersecting]= CONCAVE;

			if (ReferApp.Fe.SP.Pockets[containerface]!= 0)
				ReferApp.Cu.La.LabelsInEdges[edgeintersecting]= CONCAVE;
			else
				ReferApp.Cu.La.LabelsInEdges[edgeintersecting]= CONVEX;

			EDGE_LABEL label;
			label= ReferApp.Cu.La.LabelsInEdges[edgeintersecting];

			//We check if the intersecting edge is a boundary edge of any subgraph
			long SubGraphOfIntersectingEdge= ReferApp.Cu.SG.SubgraphForEdge[edgeintersecting];
			if(IsValueInVector(edgeintersecting, ReferApp.Cu.Pe.PerimeterEdges[SubGraphOfIntersectingEdge+1]))
				label= BOUNDARY;

			if ((label==BOUNDARY)||(label==CONCAVE)){
				continuar= false;
				step= true;
			}
			if (label== CONVEX){
				continuar= false;
				pocket= true;
			}
			ii++;
		}
		if (edgesintersection.size()>0){
			merits += 0.35;
			continuar= false;
		}
		}

//----------------------------------------------------------------------------------------------
		if (continuar && (baseface!= -1)){
			//This is the case when we only have 1 base and 1 outer face candidate
			//Next we classify the feature by means of the direction between three main points
			CVertex v1, v2, v3;

			//v1 is the point defining the barycenter of the outer face (container face)
			//and its adjoining faces
			BarycenterAdjoiningFaces(DDBB, containerface, v1);

			//v2 is the point defining the barycenter of the main base face
			//v3 is the point defining the barycenter of the second base face
			CFace basef;
			for (long f= 0; f<(long) faces.size(); f++){
				if (faces[f]==-1){
					basef= DDBB->GetFace(f);
					if (f== baseface)
						v2= GetFaceCentroid(DDBB, f);
					else
						v3= GetFaceCentroid(DDBB, f);
				}//end if
			}//end for

			double angle1= DDBB->GetAngle2DX_PIPI(v2, v1);

			double x= v2.x+(v3.x-v2.x)*cos(-angle1)-(v3.y-v2.y)*sin(-angle1);
			CVertex VertexRotated2;
			VertexRotated2.x= x;

			if (VertexRotated2.x>v2.x){
				pocket= true;
				merits += 0.35;
			}
			else{
				step= true;
				merits += 0.35;
			}
		}

//-----------------------------------------------------------------------------------------
		//Add the feature to the general BBDD
		if (step){
			ReferApp.Fe.SP.NumSteps ++;
			for (long i= 0; i< (long)faces.size(); i++){
				if(std::abs(faces[i])== 1)
					ReferApp.Fe.SP.Steps[i]= (faces[i]*ReferApp.Fe.SP.NumSteps);
			}
			ReferApp.Fe.SP.MeritSteps.push_back(merits);
			ReferApp.Fe.SP.ContainerStepFaces.push_back(containerface);
			ReferApp.Fe.SP.StepsBaseFace.push_back(baseface);
		}

		if (pocket){
			ReferApp.Fe.SP.NumPockets ++;
			for (long i= 0; i< (long)faces.size(); i++){
				if(std::abs(faces[i])== 1)
					ReferApp.Fe.SP.Pockets[i]= (faces[i]*ReferApp.Fe.SP.NumPockets);
			}
			ReferApp.Fe.SP.MeritPockets.push_back(merits);
			ReferApp.Fe.SP.ContainerPocketFaces.push_back(containerface);
			ReferApp.Fe.SP.PocketsBaseFace.push_back(baseface);
		}
	}//end for every graph
}


void CFeatureStepsAndPockets::EraseStepsAndPockets()
//Reset previos steps and pockets
{
	ReferApp.Fe.SP.Steps.clear();
	ReferApp.Fe.SP.MeritSteps.clear();
	ReferApp.Fe.SP.NumSteps= 0;
	ReferApp.Fe.SP.ContainerStepFaces.clear();
	ReferApp.Fe.SP.StepsBaseFace.clear();

	ReferApp.Fe.SP.Pockets.clear();
	ReferApp.Fe.SP.MeritPockets.clear();
	ReferApp.Fe.SP.NumPockets= 0;
	ReferApp.Fe.SP.ContainerPocketFaces.clear();
	ReferApp.Fe.SP.PocketsBaseFace.clear();
}


void CFeatureStepsAndPockets::GetStepsAndPockets(CDB_Model *DDBB)
//Procedure to find steps and pockets in 2D B-Reps

//Candidate steps/pockets are first assumed to have a "weak" merit
//Their "strongness" is increased if they fit some other criteria
//Strongness is quantified by a figure of merit

//Faces are required overall
//Disconnected subgraphs are required
//Labels are required
{
	if(DDBB->GetSizeFaces() == 0){
		//AfxMessageBox("Faces must be detected prior detecting Steps and Pockets");
		return;
	}

	//Reset steps and pockets
	//-----------------------
	ReferApp.Fe.SP.Steps.assign(DDBB->GetSizeFaces(), 0);
	ReferApp.Fe.SP.Pockets.assign(DDBB->GetSizeFaces(), 0);

	//Check if subgraphs have been calculated
	if(ReferApp.Cu.SG.NumSubgraphs == 0){
		AfxMessageBox("No subgraphs, hence steps and pockets cannot be searched");
		return;
	}

	//Check if labels have been reseted (and optionally calculated)
	ReferApp.Cu.La.LabelsInEdges.assign(DDBB->GetSizeEdges(), BLANK); //Remove this initialization when labelling edges gets fixed!
	if(ReferApp.Cu.La.LabelsInEdges.size() == 0){
		AfxMessageBox("No labels, hence steps and pockets cannot be searched");
		return;
	}

	//Assign thresholds to cope with sketching imperfections
	//------------------------------------------------------
	//Assign the threshold for "short" and "long"edges
	//to detect slender faces
	double Xmin, Ymin, Zmin, Xmax, Ymax, Zmax;
	DDBB->GetBoundingBox(Xmin, Ymin, Zmin, Xmax, Ymax, Zmax);
	ThresholdShort= ReferApp.Fe.SP.ShortEdges * min(std::fabs(Xmax-Xmin), std::fabs(Ymax-Ymin));

	ThresholdLong= ReferApp.Fe.SP.LongEdges;

	//Assign the threshold for parallelogram
	ThresholdLength= ThresholdShort; //WARNING: ThresholdShort is used as ThresholdLength!!
	ThresholdAngle= ReferApp.Fe.SP.CollinearEdges;

	//Assign the thresholds for similar faces
	ThresholdSimilarDistance= ReferApp.Fe.SP.SimilarFacesDistances;
	ThresholdSimilarAngle= ReferApp.Fe.SP.SimilarFacesAngles;

	//Assign the thresholds for comparison of areas between lateral faces and base faces
	ThresholdAreaBases= ReferApp.Fe.SP.ThresholdAreaBases;

	//Calculate face cues
	//-------------------
	GetFaceCues(DDBB);

	//Recognizing Steps/Pockets
	//-------------------------
	if (ReferApp.Cu.SG.NumSubgraphs > 1)
		Output_Step_Pocket(DDBB);
}
