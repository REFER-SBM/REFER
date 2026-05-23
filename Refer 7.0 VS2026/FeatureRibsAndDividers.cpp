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

#include "FeatureRibsAndDividers.h"


void CFeatureRibsAndDividers::GetFaceCues(CDB_Model *DDBB)
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


bool CFeatureRibsAndDividers::GetThinWall(CDB_Model *DDBB, long &border, 
										  long &connect1, long &connect2,
										  long &lateral1, long &lateral2)
//Searches for a thin wall:
//	-Searches first for cuadrilateral and narrow face
//	 (Candidate border faces of an open thin wall)
//	-Searches then for two lateral faces similar to each other

//Returns true if a thin wall is found, false otherwise
//Returns the edges of the centralFace that connect the lateral faces
{
	//Loop to search a thin wall
	//The last (bigger) face is excluded, as it must belong to the core of the shape
	//(not to any "outer" rib)
	for (long ii=0; ii<DDBB->GetSizeFaces()-1; ii++){
		//Test smaller faces first
		long i= SmallToBigFaces[ii];

		//Check if candidate border face does not belong to any previous rib/divider
		if (ReferApp.Fe.RD.Ribs[i]==0 && ReferApp.Fe.RD.Dividers[i]==0){
			//Check if face is a quadrilateral
			if(FaceQuadrilateral[i]){
				//Check if face is a parallelogram
				if(Parallelogram[i]){
					//Face is a candidate border face of a rib/divider
					if(FaceSlender[i]){
						border= i;

						long long1, long2, short1, short2;
						PairEdgesOfParallelogramFace(DDBB->GetFace(border), edgeLength,
													 long1, long2, short1, short2);

						//Search for its long lateral faces
						if (GetLateralFaces(DDBB, border, long1, long2, lateral1, lateral2)){
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
				}
			}
		}
	}

	connect1= connect2= lateral1= lateral2= -1;
	return false;
}


bool CFeatureRibsAndDividers::RibFitsSymmetry(CDB_Model *DDBB)
//Returns true if somme symmetry happens to ribs

//	-Increase the figure of merit if
//	 candidate rib is symmetrically located
//	-Increase the figure of merit if
//	 sets of candidate ribs are symmetrically located
{
	//TBD

	bool flag_symmetry= false;

	//Increase the figure of merit if
	//candidate rib is symmetrically located
	////for (each rib r){
	////	for (each bilateral symmetry plane s){
	////		if (s transverses the short edges in the border face of r){
	////			increase the figure of merit of rib r
	////		}
	////	}
	////}

	//Increase the figure of merit if
	//sets of candidate ribs are symmetrically located

	////for each rib r1
	////	for each bilateral symmetry plane s
	////		for each rib r2
	////			if r1 and r2 are symmetric relative to s
	////				increase the figures of merit of both ribs (r1 and r2)
	////			end if
	////	end for
	////end for

	return flag_symmetry;
}


//bool CFeatureRibsAndDividers::DividerIsNotASubgraph(CDB_Model *DDBB, long NumDiv)
//{
//	std::vector<long> EdgesOfDivider;
//
//	for(long i= 0; i< (long)ReferApp.Fe.RD.Dividers.size(); i++){
//		if((ReferApp.Fe.RD.Dividers[i]== NumDiv)||(ReferApp.Fe.RD.Dividers[i]== -NumDiv)){
//			CFace Face= DDBB->GetFace(i);
//			if (EdgesOfDivider.size()== 0)
//				EdgesOfDivider= Face.GetAllEdgesInFace();
//			else{
//				std::vector<long> NewEdges= Face.GetAllEdgesInFace();
//				for(long j= 0; j<(long) NewEdges.size(); j++){
//					if(!IsValueInVector(NewEdges[j], EdgesOfDivider))
//						EdgesOfDivider.push_back(NewEdges[j]);
//				}
//			}
//		}
//	}
//
//	long NumSG= ReferApp.Cu.SG.SubgraphForEdge[EdgesOfDivider[0]];
//
//	for(long k=0; k<DDBB->GetSizeEdges(); k++){
//		if(ReferApp.Cu.SG.SubgraphForEdge[k]== NumSG){
//			if(!IsValueInVector(k, EdgesOfDivider))
//				return false;
//		}
//	}
//
//	long para= 0;
//	return true;
//}


std::vector<long> CFeatureRibsAndDividers::GetContainerFaces(CDB_Model *DDBB, 
															 long border,
															 long lateral1, long lateral2)
//Returns a list of faces that "contain" the rib/divider defined by "border",
//"lateral1"  and "lateral2"

//Faces are containers if they are contiguous to the lateral face,
////and they have more than four sides

//Container faces of ith rib/divider are returned
//as the ith sub-vector of ContainerFaces
//Typically each rib/divider must contain two faces
{
	//Add edges of lateral face 1
	std::vector<long> LateralFaceEdges = DDBB->GetFace(lateral1).GetAllEdgesInFace();

	//Add edges of lateral face 2
	std::vector<long> Lateral2 = DDBB->GetFace(lateral2).GetAllEdgesInFace();
	for(long i= 0; i<(long)Lateral2.size(); i++){
		LateralFaceEdges.push_back(Lateral2[i]);
	}

	//Remove edges of border face
	std::vector<long> Border = DDBB->GetFace(border).GetAllEdgesInFace();
	long position;
	for(long i= 0; i<(long)Border.size(); i++){
		if(IsValueInVector(Border[i], LateralFaceEdges, position))
			LateralFaceEdges.erase(LateralFaceEdges.begin()+position);
	}

	std::vector<long> ContainerFaces;

	//Check all faces sharing lateral edges
	for(long i= 0; i<(long)LateralFaceEdges.size(); i++){
		std::vector<long> FacesSharingI= DDBB->GetFacesSharingEdge(LateralFaceEdges[i]);

		//We assume that sketch depicts a two-manifold model, 
		//where exactly two faces share each edge,
		//and we search for faces contiguous to the lateral, and other than the border
		if(FacesSharingI.size() == 2){
			long ContactFace;
			bool contiguous= false;

			if((FacesSharingI[0] == lateral1 ||
				FacesSharingI[0] == lateral2 )){
				ContactFace= FacesSharingI[1];
				contiguous = true;
			}
			else if((FacesSharingI[1] == lateral1 ||
				FacesSharingI[1] == lateral2 )){
				ContactFace= FacesSharingI[0];
				contiguous = true;
			}

			//Contiguous face is considered a container iff:
			//- it has more than four edges
			//- it was not included before
			if (contiguous){
				//CFace ContFace= DDBB->GetFace(ContactFace);
				//if(ContFace.GetSizeEdgesInFace() > 4){
					if(!IsValueInVector(ContactFace, ContainerFaces)){
						ContainerFaces.push_back(ContactFace);
					}
				//}
			}
		}
	}

	return ContainerFaces;
}


void CFeatureRibsAndDividers::ConvertRibInDivider(CDB_Model *DDBB, long border, long lateral1, long lateral2)
//Convert the current candidate rib into a candidate divider
{
	ReferApp.Fe.RD.NumDividers += 1;
	ReferApp.Fe.RD.Dividers[border]= ReferApp.Fe.RD.NumDividers;
	ReferApp.Fe.RD.Dividers[lateral1]= -ReferApp.Fe.RD.NumDividers;
	ReferApp.Fe.RD.Dividers[lateral2]= -ReferApp.Fe.RD.NumDividers;

	for(long i= 0; i<(long)ReferApp.Fe.RD.Ribs.size(); i++){
		if(ReferApp.Fe.RD.Ribs[i]==-ReferApp.Fe.RD.Ribs[lateral1])
			ReferApp.Fe.RD.Dividers[i]= ReferApp.Fe.RD.NumDividers;
	}

	//Remove it from the ribs list
	long NumRib= -ReferApp.Fe.RD.Ribs[lateral1];

	for(long i= 0; i<(long)ReferApp.Fe.RD.Ribs.size(); i++){
		if(std::abs(ReferApp.Fe.RD.Ribs[i])==NumRib)
			ReferApp.Fe.RD.Ribs[i]= 0;
	}
	ReferApp.Fe.RD.MeritRibs.erase(ReferApp.Fe.RD.MeritRibs.begin()+NumRib-1);
	ReferApp.Fe.RD.NumRibs -= 1;

	ReferApp.Fe.RD.ContainerRibFaces.erase(ReferApp.Fe.RD.ContainerRibFaces.begin()+NumRib-1);

	//Assign an average figure of merit to the divider
	ReferApp.Fe.RD.MeritDividers.push_back(0.51);

//+++++++++++++++++++++++++++++++++++++++++++++++
	long long1, long2, short1, short2;
	PairEdgesOfParallelogramFace(DDBB->GetFace(border), edgeLength, long1, long2, short1, short2);
//+++++++++++++++++++++++++++++++++++++++++++++++

	if(ExternalAreOpposite(DDBB, border, ThresholdAngle, short1, short2, lateral1, lateral2)){
		//Increase the figure of merit if
		//external semiedges of one lateral face are collinear and runs in opposite direction
		//to the external semiedges of the paired vertex of the other lateral face
		ReferApp.Fe.RD.MeritDividers[ReferApp.Fe.RD.NumDividers-1] += 0.10;
	}

	if(Parallelogram[lateral1] || Parallelogram[lateral2]){
		//Increase the figure of merit if lateral faces are quadrilateral parallelograms
		ReferApp.Fe.RD.MeritDividers[ReferApp.Fe.RD.NumDividers-1] += 0.25;
	}

	if(RibFitsSymmetry(DDBB)){
		//Increase the figure of merit if bylateral symmetry happens to the divider
		ReferApp.Fe.RD.MeritDividers[ReferApp.Fe.RD.NumDividers-1] += 0.05;
	}

	//Raquel
	//Decrease the figure of merit if the edge which would close the feature is not a short edge
	if(!ClosingEdgeIsShort(DDBB, lateral1, lateral2)){
		ReferApp.Fe.RD.MeritDividers[ReferApp.Fe.RD.NumDividers-1] -=0.2;
	}

	//Raquel
	std::vector<long> containers= GetContainerFaces(DDBB, border, lateral1, lateral2);
	ReferApp.Fe.RD.ContainerDividerFaces.push_back(containers);
	if(containers.size() == 0)
		ReferApp.Fe.RD.MeritDividers[ReferApp.Fe.RD.NumDividers-1] -=0.30;

}


bool CFeatureRibsAndDividers::ClosingEdgeIsShort(CDB_Model *DDBB, long lateral1, long lateral2)
{
	std::vector<long> VerticesL1= DDBB->GetAllVerticesInFace(DDBB->GetFace(lateral1));
	std::vector<long> VerticesL2= DDBB->GetAllVerticesInFace(DDBB->GetFace(lateral2));
	//se calcula la media de los segmentos cortos y se compara con la distancia entre los vértices de las caras laterales
	//que no están conectados
	double distance= 0;
	long num= 0;
	long V1= -1;
	long V2= -1;

	for(long i= 0; i<(long)VerticesL1.size(); i++){
		std::vector<long> ConnectedEdges= DDBB->GetAllEdgesInVertex(VerticesL1[i]);

		for(long j= 0; j<(long)ConnectedEdges.size(); j++)	{
			CEdge edge= DDBB->GetEdge(ConnectedEdges[j]);

			if((IsValueInVector(edge.Head, VerticesL1))&&(IsValueInVector(edge.Tail, VerticesL1))){
				if((distance== 0)||(((distance/num)-ThresholdSimilarDistance)> DDBB->GetDistance2D(edge.Head, edge.Tail))){
					distance= DDBB->GetDistance2D(edge.Head, edge.Tail);
					num= 1;
				}
				else if(std::fabs((distance/num)-DDBB->GetDistance2D(edge.Head, edge.Tail))< ThresholdSimilarDistance){
					distance+= DDBB->GetDistance2D(edge.Head, edge.Tail);
					num++;
				}
			}
			
			else if((!IsValueInVector(edge.Head, VerticesL2))&&(!IsValueInVector(edge.Tail, VerticesL2))){
				if((IsValueInVector(edge.Head, VerticesL1))&&(!IsValueInVector(edge.Tail, VerticesL1)))
					V1= edge.Head;
				else if((!IsValueInVector(edge.Head, VerticesL1))&&(IsValueInVector(edge.Tail, VerticesL1)))
					V1= edge.Tail;
			}
		}
	}

	for(long i= 0; i<(long)VerticesL2.size(); i++){
		std::vector<long> ConnectedEdges= DDBB->GetAllEdgesInVertex(VerticesL2[i]);

		for(long j= 0; j<(long)ConnectedEdges.size(); j++){
			CEdge edge= DDBB->GetEdge(ConnectedEdges[j]);

			if((!IsValueInVector(edge.Head, VerticesL1))&&(!IsValueInVector(edge.Tail, VerticesL1))){
				if((IsValueInVector(edge.Head, VerticesL2))&&(!IsValueInVector(edge.Tail, VerticesL2)))
					V2= edge.Head;
				else if((!IsValueInVector(edge.Head, VerticesL2))&&(IsValueInVector(edge.Tail, VerticesL2)))
					V2= edge.Tail;
			}
		}
	}

	if((V1!=-1)&&(V2!=-1)&&((distance/num)> DDBB->GetDistance2D(V1, V2)))
		return true;
	else
		return false;
}


void CFeatureRibsAndDividers::EraseRibsAndDividers()
//Erase ribs and dividers
{
	ReferApp.Fe.RD.Ribs.clear();
	ReferApp.Fe.RD.MeritRibs.clear();
	ReferApp.Fe.RD.NumRibs= 0;
	ReferApp.Fe.RD.ContainerRibFaces.clear();

	ReferApp.Fe.RD.Dividers.clear();
	ReferApp.Fe.RD.MeritDividers.clear();
	ReferApp.Fe.RD.NumDividers= 0;
	ReferApp.Fe.RD.ContainerDividerFaces.clear();
}


void CFeatureRibsAndDividers::GetRibsAndDividers(CDB_Model *DDBB)
//Procedure to find angular ribs and dividers in 2D B-Reps

//Returns two lists of faces labeled with the number of the rib/divider to which they belong
	//Faces are labeled as 0 when they do not belong to any rib/divider
	//Faces labeled with a positive integer value "i" are side faces of the ith rib/divider
	//Faces labeled with a negative integer value "i" are bottom faces of the ith rib/divider

//Returns as well a list of container faces:
	//Container faces of ith rib/divider are returned
	//as the ith sub-vector of ContainerRibFaces/ContainerDividerFaces
//Typically each rib/divider must contain two faces

//Candidate "weak" ribs are detected first,
//Their "strongness" is increased if they fit some other criteria
//Strongness is quantified by a figure of merit

//Ribs are reasigned as dividers if they possess more than one border face
//(If lateral faces are not triangular)

//Faces are required overall

//Based on:
//	Company P., Varley P.A.C., Plumed R., and Martin R. (2012) 
//	Perceiving Ribs in Single-View Wireframe Sketches of Polyhedral Shapes. 
//	Lecture Notes in Computer Science. ISVC 2012, Part II, 
//	Advances in Visual Computing, Volume 7432, pp. 557-567.
{

	if(DDBB->GetSizeFaces() == 0){
		//AfxMessageBox("Faces must be detected prior detecting Ribs and Dividers");
		return;
	}

	//Reset ribs and dividers
	//-----------------------
	ReferApp.Fe.RD.Ribs.assign(DDBB->GetSizeFaces(), 0);   //Lateral faces are marked as possitive, while border faces are marked as negative
	ReferApp.Fe.RD.Dividers.assign(DDBB->GetSizeFaces(), 0);

	//Assign thresholds to cope with sketching imperfections
	//------------------------------------------------------
	//Assign the threshold for "short" and "long"edges
	//to detect slender faces
	double Xmin, Ymin, Zmin, Xmax, Ymax, Zmax;
	DDBB->GetBoundingBox(Xmin, Ymin, Zmin, Xmax, Ymax, Zmax);
	ThresholdShort= ReferApp.Fe.RD.ShortEdges * min(std::fabs(Xmax-Xmin), std::fabs(Ymax-Ymin));

	ThresholdLong= ReferApp.Fe.RD.LongEdges;

	//Assign the threshold for parallelogram
	ThresholdLength= ThresholdShort; //WARNING: ThresholdShort is used as ThresholdLength!!
	ThresholdAngle= ReferApp.Fe.RD.CollinearEdges;

	//Assign the thresholds for similar faces
	ThresholdSimilarDistance= ReferApp.Fe.RD.SimilarFacesDistances;
	ThresholdSimilarAngle= ReferApp.Fe.RD.SimilarFacesAngles;

	//Calculate face cues
	//-------------------
	GetFaceCues(DDBB);

	//Search for ribs and dividers
	//----------------------------
	long border, connect1, connect2, lateral1, lateral2;

	while(GetThinWall(DDBB, border, connect1, connect2, lateral1, lateral2)){

		if((ReferApp.Fe.RD.Ribs[lateral1] == 0 && ReferApp.Fe.RD.Ribs[lateral2] == 0) &&
		   (ReferApp.Fe.RD.Dividers[lateral1] == 0 && ReferApp.Fe.RD.Dividers[lateral2] == 0)){
			//New rib (or divider)
			//--------------------
			ReferApp.Fe.RD.NumRibs += 1;

			ReferApp.Fe.RD.Ribs[border]= ReferApp.Fe.RD.NumRibs;
			ReferApp.Fe.RD.Ribs[lateral1]= -ReferApp.Fe.RD.NumRibs;
			ReferApp.Fe.RD.Ribs[lateral2]= -ReferApp.Fe.RD.NumRibs;

			//Assign an average figure of merit to the rib
			ReferApp.Fe.RD.MeritRibs.push_back(0.51);

			//Recalculate merit
			if(ExternalAreOpposite(DDBB, border, ThresholdAngle, connect1, connect2, lateral1, lateral2)){
				//Increase the figure of merit if
				//external semiedges of one lateral face are collinear and runs in opposite direction
				//to the external semiedges of the paired vertex of the other lateral face
				ReferApp.Fe.RD.MeritRibs[ReferApp.Fe.RD.NumRibs-1] += 0.15;
			}

			if(Parallelogram[lateral1] || Parallelogram[lateral2]){
				//Decrease the figure of merit if lateral faces are quadrilateral parallelograms
				ReferApp.Fe.RD.MeritRibs[ReferApp.Fe.RD.NumRibs-1] -= 0.40;
			}

			if(RibFitsSymmetry(DDBB)){
				//Increase the figure of merit if bylateral symmetry occurs to the rib
				ReferApp.Fe.RD.MeritRibs[ReferApp.Fe.RD.NumRibs-1] += 0.05;
			}

			if(FaceSlender[border]){
				//Increase merit if border is a slender face
				ReferApp.Fe.RD.MeritRibs[ReferApp.Fe.RD.NumRibs-1] +=0.15;
			}

			if(!ClosingEdgeIsShort(DDBB, lateral1, lateral2)){
				//Decrease the figure of merit if the edge which would close the feature is not one of the shortest ones.
				ReferApp.Fe.RD.MeritRibs[ReferApp.Fe.RD.NumRibs-1] -=0.10;
			}

			//if(RibInArray(pDoc)){
			//	//Increase the figure of merit if the rib belongs to an array of similar ribs
			//	ReferApp.Fe.RD.MeritRibs[ReferApp.Fe.RD.NumRibs-1] += 0.05;
			//}

			//Detect container faces
			std::vector<long> containers= GetContainerFaces(DDBB, border, lateral1, lateral2);
			ReferApp.Fe.RD.ContainerRibFaces.push_back(containers);

			if(containers.size() > 1){
				ReferApp.Fe.RD.MeritRibs[ReferApp.Fe.RD.NumRibs-1] += 0.05;
			}
			else if(containers.size() == 0){
				ReferApp.Fe.RD.MeritRibs[ReferApp.Fe.RD.NumRibs-1] -= 0.25;
			}

		//	//Convert the candidate rib into a divider if it is not a triangular thin wedge
		//	CFace face= DDBB->GetFace(lateral1);
		//	if(face.GetSizeEdgesInFace() > 3)
		//		ConvertRibInDivider(DDBB, border, lateral1, lateral2);
		//		//ConvertRibInDivider(DDBB, border, side1, side2, lateral1, lateral2);
		}

		else{
			if((ReferApp.Fe.RD.Dividers[lateral1] == 0 && ReferApp.Fe.RD.Dividers[lateral2] == 0) &&
			   (ReferApp.Fe.RD.Ribs[lateral1] == ReferApp.Fe.RD.Ribs[lateral2])){

				CFace face= DDBB->GetFace(lateral1);
				if(face.GetSizeEdgesInFace()>3)
					//Convert the rib into a divider
					//------------------------------
					ConvertRibInDivider(DDBB, border, lateral1, lateral2);
					//ConvertRibInDivider(DDBB, border, side1, side2, lateral1, lateral2);
			}

			else{
				if((ReferApp.Fe.RD.Dividers[lateral1] != 0 && ReferApp.Fe.RD.Dividers[lateral2] != 0) &&
					ReferApp.Fe.RD.Dividers[lateral1] == ReferApp.Fe.RD.Dividers[lateral2]){
					//Add the new border to the divider where it belongs
					//--------------------------------------------------
					ReferApp.Fe.RD.Dividers[border]= -ReferApp.Fe.RD.Dividers[lateral1];
				}
				else{
					//Mark as a void thin wall (very unlike rib or divider)
					ReferApp.Fe.RD.Ribs[border]= 999;
					ReferApp.Fe.RD.Dividers[border]= 999;
				}
			}
		}
	}
}