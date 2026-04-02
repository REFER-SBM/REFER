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
#include "DB_Model_Maths.h"
#include "Refer.h"

#include "FeatureRailsAndSlots.h"


void CFeatureRailsAndSlots::GetFaceCues(CDB_Model *DDBB)
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
}


void CFeatureRailsAndSlots::RemoveVertexFromPolygon(long vert, std::vector<long> &containerface1)
//Removes vertex "vert" from the list of vertices of the polygon "container face"
{
	for(long i= 0; i<(long)containerface1.size(); i++){
		if(vert == containerface1[i]){
			containerface1.erase(containerface1.begin()+i, containerface1.begin()+i+1);
			break;
		}
	}
}


bool CFeatureRailsAndSlots::ContainerAreaMinorThanFeature(CDB_Model *DDBB, long containerface, long bottom, 
														  long &vert1, std::vector<long> &containerpolygon)
{
	CFace bottom1= DDBB->GetFace(bottom);
	std::vector<long> verticesbottom= DDBB->GetAllVerticesInFace(bottom1);

	CFace containerface1= DDBB->GetFace(containerface);
	containerpolygon= DDBB->GetAllVerticesInFace(containerface1);

	std::vector<long> verticesfeature;

	//Eliminamos del contenedor los vertices del bottom
	for(long i= 0; i< (long)verticesbottom.size(); i++){
		vert1= verticesbottom[i];
		long vert2= verticesbottom[(i+1)%verticesbottom.size()];

		if((DDBB->IsVertexInFace(containerface1, vert1))&&
			(DDBB->IsVertexInFace(containerface1, vert2))){

			long N= containerpolygon.size();
			for(long i= 0; i< (long)DDBB->GetSizeVerticesInFace(containerface); i++){
				if(((vert1== containerpolygon[i])&&(vert2== containerpolygon[(i+1)% N]))
					||((vert2== containerpolygon[i])&&(vert1== containerpolygon[(i+1)% N]))){

						long ii= (i-1)% N; if(ii<0) ii+= N;
						verticesfeature.push_back(containerpolygon[ii]);
						verticesfeature.push_back(containerpolygon[i]);
						verticesfeature.push_back(containerpolygon[(i+1)% N]);
						verticesfeature.push_back(containerpolygon[(i+2)% N]);

						RemoveVertexFromPolygon(vert1, containerpolygon);
						RemoveVertexFromPolygon(vert2, containerpolygon);
						break;
				}
			}
			break;
		}
	}
	double Area_container= GetFaceArea(DDBB, containerpolygon);
	double Area_feature= GetFaceArea(DDBB, verticesfeature);

	if(Area_container < Area_feature) return true;

	return false;
}


bool CFeatureRailsAndSlots::IsVertexInteriorToPolygon(CDB_Model *DDBB, long vert, std::vector<long> polygon)
//Returns true if vertex "vert" is interior to the "polygon" of vertices
{
	//Convert vertex into a point
	CVertex v= DDBB->GetVertex(vert);
	POINT2D pV(v.x, v.y);

	//Converto polygon of vertices into a polygon of points
	std::vector<POINT2D> polygonVertices;
	for (long i=0; i<(long)polygon.size(); i++){
		CVertex vI= DDBB->GetVertex(polygon[i]);
		POINT2D pI(vI.x, vI.y);
		polygonVertices.push_back(pI);
	}

	return isPointInPolygon(polygonVertices, pV);
}


void CFeatureRailsAndSlots::ConvertToRailData(CDB_Model *DDBB)
//Flip from slot to rail in the database
{
	ReferApp.Fe.RS.NumRails+=1;
	ReferApp.Fe.RS.MeritRails.push_back(ReferApp.Fe.RS.MeritSlots[ReferApp.Fe.RS.NumSlots-1]);
	for(long i= 0; i< (long)DDBB->GetSizeFaces(); i++){
		if(ReferApp.Fe.RS.Slots[i] == ReferApp.Fe.RS.NumSlots){
			ReferApp.Fe.RS.Rails[i]= ReferApp.Fe.RS.NumRails;
			ReferApp.Fe.RS.Slots[i]= BackupSlots[i];
		}
		if(ReferApp.Fe.RS.Slots[i] == -ReferApp.Fe.RS.NumSlots){
			ReferApp.Fe.RS.Rails[i]= -ReferApp.Fe.RS.NumRails;
			ReferApp.Fe.RS.Slots[i]= BackupSlots[i];
		}
	}
	ReferApp.Fe.RS.MeritSlots.erase(ReferApp.Fe.RS.MeritSlots.begin()+(ReferApp.Fe.RS.NumSlots-1), 
		ReferApp.Fe.RS.MeritSlots.begin()+(ReferApp.Fe.RS.NumSlots));

	ReferApp.Fe.RS.BlindSlots.erase(ReferApp.Fe.RS.BlindSlots.begin()+(ReferApp.Fe.RS.NumSlots-1), 
		ReferApp.Fe.RS.BlindSlots.begin()+(ReferApp.Fe.RS.NumSlots));

	ReferApp.Fe.RS.ContainerSlotFaces.erase(ReferApp.Fe.RS.ContainerSlotFaces.begin()+(ReferApp.Fe.RS.NumSlots-1), 
		ReferApp.Fe.RS.ContainerSlotFaces.begin()+(ReferApp.Fe.RS.NumSlots));

	ReferApp.Fe.RS.NumSlots-= 1;
}


bool CFeatureRailsAndSlots::NewFaceIsLateralFace(CDB_Model *DDBB, long bottom2, long lateral1, bool rail)
{
	long bottom1;
	if (rail){
		for(long i= 0; i< (long)ReferApp.Fe.RS.Rails.size(); i++){
			if(ReferApp.Fe.RS.Rails[i] == -ReferApp.Fe.RS.Rails[lateral1]){
				bottom1= i;
				break;
			}
		}
	}
	else{
		for(long i= 0; i< (long)ReferApp.Fe.RS.Slots.size(); i++){
			if(ReferApp.Fe.RS.Slots[i] == -ReferApp.Fe.RS.Slots[lateral1]){
				bottom1= i;
				break;
			}
		}
	}

	//If bottom1 and bottom2 share any edge, then bottom2 is a lateral face,
	//otherwise it is considered another bottom face
	std::vector<long> edgesbottom2= DDBB->GetFace(bottom2).GetAllEdgesInFace();
	for(long i= 0; i< (long)edgesbottom2.size(); i++){
		std::vector<long> faces= DDBB->GetFacesSharingEdge(edgesbottom2[i]);
		if(((faces[0]== bottom1) && (faces[1]== bottom2)) ||
			((faces[0]== bottom2) && (faces[1]== bottom1)))
			return true;
	}
	return false;
}


bool CFeatureRailsAndSlots::GetCandidateRailSlot(CDB_Model *DDBB, long &centralFace, 
												 long &connect1, long &connect2,
												 long &lateral1, long &lateral2)
//Searches for a candidate Rail/Slot:
//	-Searches first for a cuadrilateral and parallelogram face
//	 (Candidate Top/Bottom face of a rail/slot)
//	-Searches then for two lateral faces similar to each other

//Returns true if a candidate rail/slot is found, false otherwise
//Returns the edges of the centralFace that connect the lateral faces
{
	//Loop to check quadrilateral faces
	//The last (bigger) face is excluded, as it must belong to the core of the shape
	//(not to any "outer" rail)
	for (long ii=0; ii<DDBB->GetSizeFaces()-1; ii++){
		//Test smaller faces first
		long i= SmallToBigFaces[ii];

		//Check if candidate Top/Bottom face does not belong to any previous rail/slot
		if (ReferApp.Fe.RS.Slots[i]==0 && ReferApp.Fe.RS.Rails[i]==0){
			//Check if face is a quadrilateral
			if(FaceQuadrilateral[i]){
				//Check if face is a parallelogram
				if(Parallelogram[i]){
					//Face is a candidate bottom face of a rail/slot
					centralFace= i;

					long long1, long2, short1, short2;
					PairEdgesOfParallelogramFace(DDBB->GetFace(centralFace),
												 edgeLength,
												 long1, long2, short1, short2);

					//Search for its long lateral faces
					if (GetLateralFaces(DDBB, centralFace, long1, long2, lateral1, lateral2)){
						//Check if both long lateral faces are parallelograms
						if(Parallelogram[lateral1] && Parallelogram[lateral2]){
							//Check if both long lateral faces are similar
							if (LateralsAreSimilar(DDBB,
												   ThresholdSimilarDistance,
												   ThresholdSimilarAngle,
												   short1, short2, lateral1, lateral2)){
								connect1= short1;
								connect2= short2;
								return true;
							}
						}
					}

					//Give a chance to short lateral faces
					if (GetLateralFaces(DDBB, centralFace, short1, short2, lateral1, lateral2)){
						//Check if both short lateral faces are parallelograms
						if(Parallelogram[lateral1] && Parallelogram[lateral2]){
							//Check if both short lateral faces are similar
							if (LateralsAreSimilar(DDBB,
												   ThresholdSimilarDistance,
												   ThresholdSimilarAngle,
												   long1, long2, lateral1, lateral2)){
								connect1= long1;
								connect2= long2;
								return true;
							}
						}
					}
				}
			}
		}
	}

	connect1= connect2= lateral1= lateral2= -1;
	return false;
}


bool CFeatureRailsAndSlots::RailFitsSymmetry(CDB_Model *DDBB)
//Returns true if somme symmetry happens to rails/slots

//	-Increase the figure of merit if
//	 candidate rail/slot is symmetrically located
//	-Increase the figure of merit if
//	 sets of candidate rails/slots are symmetrically located
{
	//TBD

	bool flag_symmetry= false;

	//Increase the figure of merit if
	//candidate rails/slots is symmetrically located
	////for (each rails/slots r){
	////	for (each bilateral symmetry plane s){
	////		if (s transverses the short edges in the top/bottom face of r){
	////			increase the figure of merit of rib r
	////		}
	////	}
	////}

	//Increase the figure of merit if
	//sets of candidate rails/slots are symmetrically located

	////for each rails/slots r1
	////	for each bilateral symmetry plane s
	////		for each rails/slots r2
	////			if r1 and r2 are symmetric relative to s
	////				increase the figures of merit of both rails/slots (r1 and r2)
	////			end if
	////	end for
	////end for

	return flag_symmetry;
}


long CFeatureRailsAndSlots::GetMinorArea(CDB_Model *DDBB, 
										long bottom2, 
										std::vector<long> FeatureFaces)
{
	long bottom1= -1;
	for(long i=0; i<(long) FeatureFaces.size(); i++){
		if ((i != bottom2) && (FeatureFaces[i] == FeatureFaces[bottom2]))
			bottom1= i;
	}

	if (FaceArea[bottom1] < FaceArea[bottom2])
		return bottom1;
	return bottom2;
}


bool CFeatureRailsAndSlots::EdgesIntersection(CDB_Model *DDBB, long bottom_edge, long lateral1, long lateral2)
//Returns true if exists any intersection in the 2D line-drawing
//between the edges of the bottom
//and any edge from the lateral faces
{
	std::vector<long> lateraledges = DDBB->GetFace(lateral1).GetAllEdgesInFace();
	std::vector<long>lat2= DDBB->GetFace(lateral2).GetAllEdgesInFace();
	for(long i= 0; i<(long) lat2.size(); i++){
		lateraledges.push_back(lat2[i]);
	}

	CEdge bot_edge= DDBB->GetEdge(bottom_edge);
	CVertex P1= DDBB->GetVertex(bot_edge.Tail);
	CVertex P2= DDBB->GetVertex(bot_edge.Head);

	for(long i= 0; i<(long) lateraledges.size(); i++){
		CEdge edge= DDBB->GetEdge(lateraledges[i]);
		CVertex P3= DDBB->GetVertex(edge.Tail);
		CVertex P4= DDBB->GetVertex(edge.Head);

		//if (P1-P3)x(P4-P3) have different sign from (P2-P3)x(P4-P3) and
		// (P3-P1)x(P2-P1) have different sign from (P4-P1)x(P2-P1)
		// then intersection exists
		double test1= (P1.x-P3.x)*(P4.y-P3.y)-(P4.x-P3.x)*(P1.y-P3.y);
		double test2= (P2.x-P3.x)*(P4.y-P3.y)-(P4.x-P3.x)*(P2.y-P3.y);
		double test3= (P3.x-P1.x)*(P2.y-P1.y)-(P2.x-P1.x)*(P3.y-P1.y);
		double test4= (P4.x-P1.x)*(P2.y-P1.y)-(P2.x-P1.x)*(P4.y-P1.y);

		if(((test1*test2)<0) && ((test3*test4)<0)) return true;
	}

	return false;
}


void CFeatureRailsAndSlots::GetContainerFace(CDB_Model *DDBB, long centralFace, 
											 long lateral1, long lateral2, 
											 long &containerface, long &numBlind)
//Returns the number of the face that contains the rail/slot
//defined by (centralFace, lateral1, lateral2)

//A container face must be:
//	- Adjacent to the bottom face
//	- Different from lateral faces

//If more than one container face exists, 
//the one that does not overlap with lateral faces is selected

//Updates as well the blid flag
{
	containerface= -1;
	numBlind= -1;

	std::vector<long> CentralEdges= DDBB->GetFace(centralFace).GetAllEdgesInFace();

	//Check all edges of the bottom face
	for (long i= 0; i<(long)CentralEdges.size(); i++){
		std::vector<long> FacesSharingEdgeI= DDBB->GetFacesSharingEdge(CentralEdges[i]);

		//Check all neighbour faces, other than those of the rail/slot
		for(long j= 0; j<(long) FacesSharingEdgeI.size(); j++){
			if ((FacesSharingEdgeI[j] != centralFace) &&
				(FacesSharingEdgeI[j] != lateral1)&&
				(FacesSharingEdgeI[j] != lateral2)){

				//Check number of edges of the candidate container
				long numEdgesContainerCandidate= DDBB->GetFace(FacesSharingEdgeI[j]).GetSizeEdgesInFace();

				if(numEdgesContainerCandidate > 4){
					if(containerface == -1){
						containerface= FacesSharingEdgeI[j];
					}
					else if(!EdgesIntersection(DDBB, CentralEdges[i], lateral1, lateral2)){
						//Select the container face whose 2D drawing does not overlap with lateral faces
						containerface= FacesSharingEdgeI[j];
					}
				}

				else if(numEdgesContainerCandidate == 4){
					numBlind= 1;  //WARNING: replicates a bool, but should contain the number of the blind face
				}
			}
		}
	}
}


void CFeatureRailsAndSlots::EraseRailsAndSlots()
//Erase rails and slots
{
	ReferApp.Fe.RS.NumRails= 0;
	ReferApp.Fe.RS.Rails.clear();
	ReferApp.Fe.RS.MeritRails.clear();
	ReferApp.Fe.RS.ContainerRailFaces.clear();
	ReferApp.Fe.RS.BlindRails.clear();

	ReferApp.Fe.RS.NumSlots= 0;
	ReferApp.Fe.RS.Slots.clear();
	ReferApp.Fe.RS.MeritSlots.clear();
	ReferApp.Fe.RS.ContainerSlotFaces.clear();
	ReferApp.Fe.RS.BlindSlots.clear();
}


void CFeatureRailsAndSlots::GetRailsAndSlots(CDB_Model *DDBB)
//Procedure to find rectangular rails and slots in 2D B-Reps

//Returns two lists of faces labeled with the number of the rail/slot to which they belong
	//Faces are labeled as 0 when they do not belong to any rail/slot
	//Faces labeled with a positive integer value "i" are side faces of the ith rail/slot
	//Faces labeled with a negative integer value "i" are bottom faces of the ith rail/slot

//Candidate "weak" slots are detected first,
//Their "strongness" is increased if they fit some other criteria
//Strongness is quantified by a figure of merit

//Faces are required overall

//Based on:
//	Company P., Varley P.A.C., Plumed R., and Martin R. (2012) 
//	Perceiving Ribs in Single-View Wireframe Sketches of Polyhedral Shapes. 
//	Lecture Notes in Computer Science. ISVC 2012, Part II, 
//	Advances in Visual Computing, Volume 7432, pp. 557-567.
{
	if(DDBB->GetSizeFaces() == 0){
		//AfxMessageBox("Faces must be detected prior detecting Rails and Slots");
		return;
	}

	//Reset Rails and Slots
	//---------------------
	ReferApp.Fe.RS.Rails.assign(DDBB->GetSizeFaces(), 0);
	ReferApp.Fe.RS.Slots.assign(DDBB->GetSizeFaces(), 0);

	//Assign thresholds to cope with sketching imperfections
	//------------------------------------------------------
	//Assign the threshold for "short" and "long" edges
	//to detect slender faces
	double Xmin, Ymin, Zmin, Xmax, Ymax, Zmax;
	DDBB->GetBoundingBox(Xmin, Ymin, Zmin, Xmax, Ymax, Zmax);
	ThresholdShort= ReferApp.Fe.RS.ShortEdges * min(std::fabs(Xmax-Xmin), std::fabs(Ymax-Ymin));

	ThresholdLong= ReferApp.Fe.RS.LongEdges;

	//Assign the threshold for parallelogram
	ThresholdLength= ThresholdShort; //WARNING: ThresholdShort is used as ThresholdLength!!
	ThresholdAngle= ReferApp.Fe.RD.CollinearEdges;

	//Assign the thresholds for similar faces
	ThresholdSimilarDistance= ReferApp.Fe.RS.SimilarFacesDistances;
	ThresholdSimilarAngle= ReferApp.Fe.RS.SimilarFacesAngles;

	//Calculate face cues
	//-------------------
	GetFaceCues(DDBB);

	//Search for rails and slots
	//--------------------------
	long centralFace, connect1, connect2, lateral1, lateral2;
	BackupSlots.assign(DDBB->GetSizeFaces(), 0);
	bool rail= false;  //True for rail, false for slot
	long containerface= -1;
	long numBlind= -1;

	while(GetCandidateRailSlot(DDBB, centralFace, connect1, connect2, lateral1, lateral2)){

		if((ReferApp.Fe.RS.Slots[lateral1] == 0) && (ReferApp.Fe.RS.Rails[lateral1] == 0) &&
			(ReferApp.Fe.RS.Slots[lateral2] == 0) && (ReferApp.Fe.RS.Rails[lateral2] == 0)){
			//New slot
			//--------
			rail= false;   //Initialy, the candidate rail/slot is assumed to be a slot (by default)
			ReferApp.Fe.RS.NumSlots += 1;
			//Save previous slots information
			BackupSlots[centralFace]= ReferApp.Fe.RS.Slots[centralFace];
			BackupSlots[lateral1]= ReferApp.Fe.RS.Slots[lateral1];
			BackupSlots[lateral2]= ReferApp.Fe.RS.Slots[lateral2];

			//Update slots information
			ReferApp.Fe.RS.Slots[centralFace]= ReferApp.Fe.RS.NumSlots;
			ReferApp.Fe.RS.Slots[lateral1]= -ReferApp.Fe.RS.NumSlots;
			ReferApp.Fe.RS.Slots[lateral2]= -ReferApp.Fe.RS.NumSlots;

			//Assign an average figure of merit to the slot
			ReferApp.Fe.RS.MeritSlots.push_back(0.51);

			if(ExternalAreOpposite(DDBB, centralFace, ThresholdAngle,connect1, connect2, lateral1, lateral2)){
				//Increase the figure of merit if
				//external semiedges of one lateral face are collinear
				//and one of them runs in opposite direction
				//to the external semiedges of the paired vertex of the other lateral face
				ReferApp.Fe.RS.MeritSlots[ReferApp.Fe.RS.NumSlots-1] += 0.10;
			}

			if(FaceSlender[lateral1] || FaceSlender[lateral2]){
				//Increase the figure of merit if lateral faces are slender
				ReferApp.Fe.RS.MeritSlots[ReferApp.Fe.RS.NumSlots-1] += 0.25;
			}

			if(RailFitsSymmetry(DDBB)){
				//Increase the figure of merit if bylateral symmetry happens to the rail
				ReferApp.Fe.RS.MeritSlots[ReferApp.Fe.RS.NumSlots-1] += 0.05;
			}

			//Get the container face, and recalculate blindness
			numBlind= -1;
			GetContainerFace(DDBB, centralFace, lateral1, lateral2, 
							 containerface, numBlind);
			ReferApp.Fe.RS.ContainerSlotFaces.push_back(containerface);
			ReferApp.Fe.RS.BlindSlots.push_back(numBlind);

			//The merit is reduced if the area of the container face
			//is smaller than the area of the slot/rail
			long vertbottom;
			std::vector<long> containerpolygon;
			if (containerface == -1){ //No container face was found
				ReferApp.Fe.RS.MeritSlots[ReferApp.Fe.RS.NumSlots-1] -= 0.4;
			}
			else{
				if(ContainerAreaMinorThanFeature(DDBB, containerface, centralFace, vertbottom, containerpolygon)){
					ReferApp.Fe.RS.MeritSlots[ReferApp.Fe.RS.NumSlots-1] -= 0.4;
				}
				//if the vertices of the containing face, 
				//which in turn belong to the bottom face,
				//they are outside the polygon formed by 
				//the vertices of the containing face minus these vertices,
				//then the feature is a rail, otherwise it is a slot
				if(!IsVertexInteriorToPolygon(DDBB, vertbottom, containerpolygon)){
					rail= true;
					ConvertToRailData(DDBB);
					ReferApp.Fe.RS.ContainerRailFaces.push_back(containerface);
					ReferApp.Fe.RS.BlindRails.push_back(numBlind);
				}
			}
		}

		else{
			//Add the new face to the slot/rail, since the slot/rail already exists
			//---------------------------------------------------------------------
			//Is the new face a lateral or a bottom face?
			if(NewFaceIsLateralFace(DDBB, centralFace, lateral1, rail))
				//Raquel: si el nuevo bottom comparte con el anterior al menos una arista, 
				//esta cara será una cara lateral y por tanto, el slot/rail no es pasante
				if(rail){
					ReferApp.Fe.RS.Rails[centralFace]= -ReferApp.Fe.RS.Rails[lateral1];
					//The area of the two possible bases is compared
					//and the one that is largest is taken as the base, 
					//the other face being a lateral face
					long minFace= GetMinorArea(DDBB, centralFace, ReferApp.Fe.RS.Rails);
					ReferApp.Fe.RS.Rails[minFace]= ReferApp.Fe.RS.Rails[lateral1];
					if (minFace!= centralFace){
						//si cambia la cara base, entonces también cambia la cara
						//contenedora del feature
						GetContainerFace(DDBB, centralFace, lateral1, lateral2, containerface, numBlind);
						ReferApp.Fe.RS.ContainerRailFaces[ReferApp.Fe.RS.ContainerRailFaces.size()-1]= containerface;
					}
				}
				else{
					BackupSlots[centralFace]= ReferApp.Fe.RS.Slots[centralFace];
					ReferApp.Fe.RS.Slots[centralFace]= -ReferApp.Fe.RS.Slots[lateral1];

					long minFace= GetMinorArea(DDBB, centralFace, ReferApp.Fe.RS.Slots);
					ReferApp.Fe.RS.Slots[minFace]= ReferApp.Fe.RS.Slots[lateral1];

					if (minFace!= centralFace){
						GetContainerFace(DDBB, centralFace, lateral1, lateral2, containerface, numBlind);

						if(containerface == -1) //No container face was found
							ReferApp.Fe.RS.MeritSlots[ReferApp.Fe.RS.NumSlots-1] -= 0.10;
						else{
							long vertbottom;
							std::vector<long> containerpolygon;
							ContainerAreaMinorThanFeature(DDBB, containerface, centralFace, vertbottom, containerpolygon);
							if(!IsVertexInteriorToPolygon(DDBB, vertbottom, containerpolygon)){
								rail= true;
								ConvertToRailData(DDBB);
								ReferApp.Fe.RS.ContainerRailFaces.push_back(containerface);
								ReferApp.Fe.RS.BlindRails.push_back(numBlind);
							}
							else {
								ReferApp.Fe.RS.ContainerSlotFaces[ReferApp.Fe.RS.ContainerSlotFaces.size()-1]= containerface;
								ReferApp.Fe.RS.BlindSlots[ReferApp.Fe.RS.ContainerSlotFaces.size()-1]= numBlind;
							}
						}
					}
				}

			else{
				//se define la cara añadida como cara base... es un escalón
				if(rail) 
					ReferApp.Fe.RS.Rails[centralFace]= -ReferApp.Fe.RS.Rails[lateral1];
				else //Slot
					ReferApp.Fe.RS.Slots[centralFace]= -ReferApp.Fe.RS.Slots[lateral1];

				//Reduce its figure of merit
				if(rail){
					if(ReferApp.Fe.RS.Rails[centralFace] > 0) 
						ReferApp.Fe.RS.MeritRails[ReferApp.Fe.RS.Rails[centralFace]-1] -=0.2;

					if(ReferApp.Fe.RS.Rails[centralFace]== 0)
						//Mark as a void rail
						ReferApp.Fe.RS.Rails[centralFace]= 999;
				}
				else{ //Slot
					if(ReferApp.Fe.RS.Slots[centralFace] > 0)
						ReferApp.Fe.RS.MeritSlots[ReferApp.Fe.RS.Slots[centralFace]-1] -= 0.2;

					if(ReferApp.Fe.RS.Slots[centralFace]== 0)
						//Mark as a void slot
						ReferApp.Fe.RS.Slots[centralFace]= 999;
				}
			}
		}
	}
}