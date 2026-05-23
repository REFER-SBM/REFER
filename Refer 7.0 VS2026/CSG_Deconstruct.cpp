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
#include "DB_CSG.h"
#include "Refer.h"
#include "resource.h"
#include "ReferDoc.h"

#include "CueManager.h"
#include "DetectionManager.h"

#include "DataCSG.h"

#include "CueTypology.h"

#include "CSG_Maths.h"
#include "CSG_Deconstruct.h"


CCSG_Deconstruct::CCSG_Deconstruct()
{
}


CCSG_Deconstruct::~CCSG_Deconstruct()
{
}


void CCSG_Deconstruct::SetUp()
//Updates local copies of external information required to deconstruct
{
	MainAngles= ReferApp.Cu.MA.MainAngles;

	ReductionCoefficients.clear();
	ReductionCoefficients.push_back(1/ReferApp.Cu.MA.CoefE[0]);
	ReductionCoefficients.push_back(1/ReferApp.Cu.MA.CoefE[1]);
	ReductionCoefficients.push_back(1/ReferApp.Cu.MA.CoefE[2]);

	OuterSubGraph= ReferApp.Cu.SG.Outer; //Outer subgraph, as calculated before deconstructing the model
}


long CCSG_Deconstruct::LowMeritFeatures(std::vector<double> Merits)
//Returns the number of features of the list "Merits" with low merit
{
	long PoorFeatures= 0;
	for (long i= 0; i< (long)Merits.size(); i++){
		if(Merits[i]< ReferApp.CSG.CSG_ThresholdFMerit)
			PoorFeatures++;
	}
	return PoorFeatures;
}


long CCSG_Deconstruct::GetNumValidFeatures()
//Counts all valid features

//Features below a minimum threshold merit are discarded
{
	return (ReferApp.Fe.RD.NumRibs- LowMeritFeatures(ReferApp.Fe.RD.MeritRibs)+
			ReferApp.Fe.RD.NumDividers- LowMeritFeatures(ReferApp.Fe.RD.MeritDividers)+
			ReferApp.Fe.SP.NumSteps- LowMeritFeatures(ReferApp.Fe.SP.MeritSteps)+
			ReferApp.Fe.SP.NumPockets- LowMeritFeatures(ReferApp.Fe.SP.MeritPockets)+
			ReferApp.Fe.RS.NumSlots- LowMeritFeatures(ReferApp.Fe.RS.MeritSlots)+
			ReferApp.Fe.RS.NumRails- LowMeritFeatures(ReferApp.Fe.RS.MeritRails));
}




//------Begin SelectFeature---------------------------------


void CCSG_Deconstruct::GetSmallestFeature(CDB_Model *DDBB_Dec_Current,
										  CFeature &NewFeat,
										  long &FeatureId)
//Returns the identifier FeatureId of the smallest feature

//We look for small areas (as we measure 2D images) 
//and select the smallest of them to get removed

//Alternatively, features may be selected following a hierarchy:
//  - First we will remove ribs
//    assuming that they usually are
//    included as final reinforcements
{
	double AreaFeature= 0;

	//Select smallest rib
	if(ReferApp.Fe.RD.NumRibs > 0){
		double AreaRib= 0;
		long numfeat= 0;
		for(long j=0; j<ReferApp.Fe.RD.NumRibs; j++){
			if(ReferApp.Fe.RD.MeritRibs[j] > ReferApp.CSG.CSG_ThresholdFMerit){      //Minimum merit to parse the feature
				std::vector<long> FeatureFaces;
				for(long b=0; b<(long)ReferApp.Fe.RD.Ribs.size(); b++){
					if(ReferApp.Fe.RD.Ribs[b]== -(j+1))
						FeatureFaces.push_back(b); 
				}
				double Areacomp= GetBoundingBoxArea(DDBB_Dec_Current, FeatureFaces);

				if((Areacomp<AreaRib) || (AreaRib==0)){
					AreaRib= Areacomp;
					numfeat=j+1;
				}
			}
		}

		if ((AreaRib!=0) && (AreaFeature==0)){
			//Select this rib
			NewFeat.SetType(RIB);
			NewFeat.SetMerit(ReferApp.Fe.RD.MeritRibs[numfeat-1]);
			FeatureId= numfeat;
			AreaFeature= AreaRib;
		}
	}

	//Select smallest divider
	if(ReferApp.Fe.RD.NumDividers > 0){
		double AreaDivider= 0;
		long numfeat= 0;
		for(long j=0; j<ReferApp.Fe.RD.NumDividers; j++){
			if(ReferApp.Fe.RD.MeritDividers[j] > ReferApp.CSG.CSG_ThresholdFMerit){
				std::vector<long> FeatureFaces;
				for(long b=0; b<(long)ReferApp.Fe.RD.Dividers.size(); b++){
					if(ReferApp.Fe.RD.Dividers[b]== -(j+1)) FeatureFaces.push_back(b);
				}
				double Areacomp= GetBoundingBoxArea(DDBB_Dec_Current, FeatureFaces);

				if((Areacomp<AreaDivider) || (AreaDivider==0)){
					AreaDivider= Areacomp;
					numfeat=j+1;
				}
			}
		}

		if (AreaDivider>0.01 && ((AreaDivider<AreaFeature) || (AreaFeature==0))){
			//Seletc this divider
			NewFeat.SetType(DIVIDER);
			NewFeat.SetMerit(ReferApp.Fe.RD.MeritDividers[numfeat-1]);
			FeatureId= numfeat;
			AreaFeature= AreaDivider;
		}
		else if((AreaDivider!=0) && (AreaDivider==AreaFeature) && (ReferApp.Fe.RD.MeritDividers[numfeat-1]>NewFeat.GetMerit())){
			//Si es el mismo feature pero con otra denominación con mayor mérito
			NewFeat.SetType(DIVIDER);
			NewFeat.SetMerit(ReferApp.Fe.RD.MeritDividers[numfeat-1]);
			FeatureId= numfeat;
		}
	}

	//Select smallest slot
	if(ReferApp.Fe.RS.NumSlots > 0){
		double AreaSlot= 0;
		long numfeat= 0;
		for(long j=0; j<ReferApp.Fe.RS.NumSlots; j++){
			if(ReferApp.Fe.RS.MeritSlots[j] > ReferApp.CSG.CSG_ThresholdFMerit){
				std::vector<long> FeatureFaces;
				for(long b=0; b<(long)ReferApp.Fe.RS.Slots.size(); b++){
					if(ReferApp.Fe.RS.Slots[b]== -(j+1)) FeatureFaces.push_back(b);
				}
				double Areacomp= GetBoundingBoxArea(DDBB_Dec_Current, FeatureFaces);

				if(((AreaSlot> Areacomp)||(AreaSlot==0))&&(FeatureFaces.size()>1)){
					AreaSlot= Areacomp;
					numfeat=j+1;
				}
			}
		}

		if ((AreaSlot>0.01) && ((AreaSlot<AreaFeature) || (AreaFeature==0))){
			//Select this slot
			NewFeat.SetType(SLOT);
			NewFeat.SetMerit(ReferApp.Fe.RS.MeritSlots[numfeat-1]);
			FeatureId= numfeat;
			AreaFeature= AreaSlot;
		}
		else if((AreaSlot!=0) && (AreaSlot==AreaFeature) && (ReferApp.Fe.RS.MeritSlots[numfeat-1]>NewFeat.GetMerit())){
			//Si es el mismo feature pero con otra denominación con mayor mérito
			NewFeat.SetType(SLOT);
			NewFeat.SetMerit(ReferApp.Fe.RS.MeritSlots[numfeat-1]);
			FeatureId= numfeat;
		}
	}

	//Select smallest rail
	if(ReferApp.Fe.RS.NumRails > 0){
		double AreaRail= 0;
		long numfeat= 0;
		for(long j=0; j<ReferApp.Fe.RS.NumRails; j++){
			if(ReferApp.Fe.RS.MeritRails[j] > ReferApp.CSG.CSG_ThresholdFMerit){
				std::vector<long> FeatureFaces;
				for(long b=0; b<(long)ReferApp.Fe.RS.Rails.size(); b++){
					if(ReferApp.Fe.RS.Rails[b]== -(j+1)) FeatureFaces.push_back(b);
				}
				double Areacomp= GetBoundingBoxArea(DDBB_Dec_Current, FeatureFaces);

				if((AreaRail> Areacomp)||(AreaRail==0)){
					AreaRail= Areacomp;
					numfeat=j+1;
				}
			}
		}

		if ((AreaRail>0.01) && ((AreaRail<AreaFeature) || (AreaFeature==0))){
			NewFeat.SetType(RAIL);
			NewFeat.SetMerit(ReferApp.Fe.RS.MeritRails[numfeat-1]);
			FeatureId= numfeat;
			AreaFeature= AreaRail;
		}
		else if((AreaRail!=0) && (AreaRail==AreaFeature) && (ReferApp.Fe.RS.MeritRails[numfeat-1]>NewFeat.GetMerit())){
			//Si es el mismo feature pero con otra denominación con mayor mérito
			NewFeat.SetType(RAIL);
			NewFeat.SetMerit(ReferApp.Fe.RS.MeritRails[numfeat-1]);
			FeatureId= numfeat;
		}
	}

	//Select smallest pocket
	if(ReferApp.Fe.SP.NumPockets > 0){
		double AreaPocket= 0;
		long numfeat= 0;
		for(long j=0; j<ReferApp.Fe.SP.NumPockets; j++){
			if(ReferApp.Fe.SP.MeritPockets[j] > ReferApp.CSG.CSG_ThresholdFMerit){
				std::vector<long> FeatureFaces;
				for(long b=0; b<(long)ReferApp.Fe.SP.Pockets.size(); b++){
					if(ReferApp.Fe.SP.Pockets[b]== -(j+1))
						FeatureFaces.push_back(b);
				}
				double Areacomp= GetBoundingBoxArea(DDBB_Dec_Current, FeatureFaces);

				if((AreaPocket> Areacomp)||(AreaPocket==0)){
					AreaPocket= Areacomp;
					numfeat=j+1;
				}
			}
		}

		if ((AreaPocket>0.01) && ((AreaPocket<AreaFeature) || (AreaFeature==0))){
			NewFeat.SetType(POCKET);
			NewFeat.SetMerit(ReferApp.Fe.SP.MeritPockets[numfeat-1]);
			FeatureId= numfeat;
			AreaFeature= AreaPocket;
		}
		else if((AreaPocket!=0) && (AreaPocket==AreaFeature) && (ReferApp.Fe.SP.MeritPockets[numfeat-1]>NewFeat.GetMerit())){
			//Si es el mismo feature pero con otra denominación con mayor mérito
			NewFeat.SetType(POCKET);
			NewFeat.SetMerit(ReferApp.Fe.SP.MeritPockets[numfeat-1]);
			FeatureId= numfeat;
		}
	}

	//Select smallest step
	if(ReferApp.Fe.SP.NumSteps >0){
		double AreaStep= 0;
		long numfeat= 0;
		for(long j=0; j<ReferApp.Fe.SP.NumSteps; j++){
			if(ReferApp.Fe.SP.MeritSteps[j] > ReferApp.CSG.CSG_ThresholdFMerit){
				std::vector<long> FeatureFaces;
				for(long b=0; b<(long)ReferApp.Fe.SP.Steps.size(); b++){
					if(ReferApp.Fe.SP.Steps[b]== -(j+1))
						FeatureFaces.push_back(b);
				}
				double Areacomp= GetBoundingBoxArea(DDBB_Dec_Current, FeatureFaces);

				if((AreaStep> Areacomp)||(AreaStep==0)){
					AreaStep= Areacomp;
					numfeat=j+1;
				}
			}
		}

		if ((AreaStep>0.01) && ((AreaStep<AreaFeature) || (AreaFeature==0))){
			NewFeat.SetType(STEP);
			NewFeat.SetMerit(ReferApp.Fe.SP.MeritSteps[numfeat-1]);
			FeatureId= numfeat;
			AreaFeature= AreaStep;
		}
		else if((AreaStep!=0) && (AreaStep==AreaFeature) && (ReferApp.Fe.SP.MeritSteps[numfeat-1]>NewFeat.GetMerit())){
			//Si es el mismo feature pero con otra denominación con mayor mérito
			NewFeat.SetMerit(ReferApp.Fe.SP.MeritSteps[numfeat-1]);
			FeatureId= numfeat;
		}
	}
}


long CCSG_Deconstruct::GetFatherFace(CDB_Model *DDBB_Dec_Current,
									 CFeature PrevFeature)
//Revisits the last feature added to the current branch
//to return the number of its father face
{
	//Recover the vertices of the first container face of the selected feature
	//(Vertices of container face are referenced to DDBB_Dec_Current!!)
	std::vector<long> ContainerVertices= PrevFeature.GetContainerFace(0);

	long DDBBfaceposition= -1;

	long numFaces= (long)DDBB_Dec_Current->GetSizeFaces();
	long numCVertices= (long)ContainerVertices.size();

	long f= 0;
	bool continuar= (numCVertices>0);

	//Loop over all the faces of the current deconstruction
	//to match the container face
	while(continuar && (f < numFaces)){
		CFace DBFace= DDBB_Dec_Current->GetFace(f);
		long j= 0;
		long k= 0;
		bool follow= true;
		//Loop over all the vertices of the container faces
		while(follow && j<numCVertices){
			if (ContainerVertices[j] == -1)
				k++;
			else if(!DDBB_Dec_Current->IsVertexInFace(DBFace, ContainerVertices[j]))
				follow= false;

			j++;
		}

		if ((j-k) == DDBB_Dec_Current->GetSizeVerticesInFace(f)){
			DDBBfaceposition= f;
			continuar= false;
		}
		f++;
	}

	//Return container (father) face
	return DDBBfaceposition;
}


void CCSG_Deconstruct::GetFatherFeature(CDB_Model *DDBB_Dec_Current,
										CFeature &NewFeat,
										long &FeatureId,
										CFeature PrevFeature)
//Returns the feature selecte as
//the best candidate father feature of current feature.

//Features are father of current features if they share
//the face that is classified as FatherFace

//Also returns the class of this father feature

//If more than one feature shares the FatherFace, 
//	- The one with the highest merit is selected
//	- For equal merits, the priority (higher to lower) is: rib, slot, pocket, step
{
	//Default
	double merit= 0;
	NewFeat.SetType(NODE);
	FeatureId= 0;

	//Get father face
	long FatherFace= GetFatherFace(DDBB_Dec_Current, PrevFeature);

	//Search for best Father Feature
	if (FatherFace != -1){

		merit= ReferApp.CSG.CSG_ThresholdFMerit;   //Assign minimum merit

		//Search for best Father Rib
		if(ReferApp.Fe.RD.NumRibs > 0 &&
		   ReferApp.Fe.RD.Ribs[FatherFace] != 0){
			long Id= std::abs(ReferApp.Fe.RD.Ribs[FatherFace]);
			if(ReferApp.Fe.RD.MeritRibs[Id-1] >= merit){
				NewFeat.SetType(RIB);
				FeatureId= Id;
				merit= ReferApp.Fe.RD.MeritRibs[FeatureId-1];
			}
		}

		//Search for best Father Divider
		if(ReferApp.Fe.RD.NumDividers > 0 &&
		   ReferApp.Fe.RD.Dividers[FatherFace] != 0){
			long Id= std::abs(ReferApp.Fe.RD.Dividers[FatherFace]);
			if(ReferApp.Fe.RD.MeritDividers[Id-1] > merit){
				NewFeat.SetType(DIVIDER);
				FeatureId= Id;
				merit= ReferApp.Fe.RD.MeritDividers[FeatureId-1];
			}
		}

		//Search for best Father Slot
		if(ReferApp.Fe.RS.NumSlots > 0 &&
		   ReferApp.Fe.RS.Slots[FatherFace] != 0){
			long Id= std::abs(ReferApp.Fe.RS.Slots[FatherFace]);
			if (ReferApp.Fe.RS.MeritSlots[Id-1] > merit){
				NewFeat.SetType(SLOT);
				FeatureId= Id;
				merit= ReferApp.Fe.RS.MeritSlots[FeatureId-1];
			}
		}

		//Search for best Father Rail
		if(ReferApp.Fe.RS.NumRails > 0 &&
		   ReferApp.Fe.RS.Rails[FatherFace] != 0){
			long Id= std::abs(ReferApp.Fe.RS.Rails[FatherFace]);
			if (ReferApp.Fe.RS.MeritRails[Id-1] > merit){
				NewFeat.SetType(RAIL);
				FeatureId= Id;
				merit= ReferApp.Fe.RS.MeritRails[FeatureId-1];
			}
		}

		//Search for best Father Pocket
		if(ReferApp.Fe.SP.NumPockets > 0 &&
		   ReferApp.Fe.SP.Pockets[FatherFace] != 0){
			long Id= std::abs(ReferApp.Fe.SP.Pockets[FatherFace]);
			if (ReferApp.Fe.SP.MeritPockets[Id-1] > merit){
				NewFeat.SetType(POCKET);
				FeatureId= Id;
				merit= ReferApp.Fe.SP.MeritPockets[FeatureId-1];
			}
		}

		//Search for best Father Step
		if(ReferApp.Fe.SP.NumSteps > 0 &&
		   ReferApp.Fe.SP.Steps[FatherFace] != 0){
			long Id= std::abs(ReferApp.Fe.SP.Steps[FatherFace]);
			if (ReferApp.Fe.SP.MeritSteps[Id-1] > merit){
				NewFeat.SetType(STEP);
				FeatureId= Id;
				merit= ReferApp.Fe.SP.MeritSteps[FeatureId-1];
			}
		}
	}

	NewFeat.SetMerit(merit);
}


bool CCSG_Deconstruct::GetFatherFaces(CDB_Model *DDBB_Dec_Current, CFeature &Feat, long FeatureId)
//Returns true if father faces exist for the feature Feat

//The vertices of the father faces are updated as well in Feat

//(Coordinates are stored instead of numbering tags of vertices;
//this is done so because the numbering of the faces varies in each iteration,
//therefore it is useless storing the number of the "parent" face vertices)
{
	//Populate the list of father faces
	std::vector<long> FatherFaces;

	switch (Feat.GetType()){
		case RIB:{
			for(long i= 0; i< (long)ReferApp.Fe.RD.ContainerRibFaces[FeatureId-1].size(); i++){
				FatherFaces.push_back(ReferApp.Fe.RD.ContainerRibFaces[FeatureId-1][i]);
			}
		}
		break;

		case DIVIDER:{
			for(long i= 0; i< (long)ReferApp.Fe.RD.ContainerDividerFaces[FeatureId-1].size(); i++){
				FatherFaces.push_back(ReferApp.Fe.RD.ContainerDividerFaces[FeatureId-1][i]);
			}
		}
		break;

		case RAIL:{
			FatherFaces.push_back(ReferApp.Fe.RS.ContainerRailFaces[FeatureId-1]);
		}
		break;

		case SLOT:{
			FatherFaces.push_back(ReferApp.Fe.RS.ContainerSlotFaces[FeatureId-1]);
		}
		break;

		case STEP:{
			FatherFaces.push_back(ReferApp.Fe.SP.ContainerStepFaces[FeatureId-1]);
		}
		break;

		case POCKET:{
			FatherFaces.push_back(ReferApp.Fe.SP.ContainerPocketFaces[FeatureId-1]);
		}
		break;
	}//end switch

	//Save father faces, if exist
	//(Save the coordinates of the vertices, not the face number)
	if(FatherFaces.size() > 0){
		for(long j= 0; j< (long)FatherFaces.size(); j++){
			//Store container face
			std::vector<long> VertFatherFace= DDBB_Dec_Current->GetAllVerticesInFace(DDBB_Dec_Current->GetFace(FatherFaces[j]));
			Feat.AddContainerFace(VertFatherFace);    //Vertices of container face are referenced to DDBB_Dec_Current!!
		}
		return true;
	}

	return false;
}


long CCSG_Deconstruct::SelectFeature(CDB_Model *DDBB_Dec_Current, 
									 CDB_CSG *DB_CSG_Model,
									 CFeature &NextFeat, 
									 bool &FeatureFather)
//Selects the most plausible son (i.e. contained) feature

//If father faces are not identified (flag FeatureFather is false),
//the smallest feature is returned
//Otherwise, the feature connected to the father faces is returned

//If more than one feature is connected to the father faces,
//GetFatherFeature selects the best candidate

//Returns the son feature identifier

//Updates feature parameters in NewFeat
//Updates the flag FeatureFather


//The proposed algorithm is governed by the following 
//hierarchy of selection criteria for lower-level child characteristic elements:

//	1. Ribs are usually added in the later stages of design, 
//	   as they are typically used to reinforce an existing design. 
//	   Therefore they will be the first to be removed from the drawing.

//	2. Designers will most likely start drawing the largest 2D feature elements 
//	   (as they can act as container 2D feature elements) 
//	   to finish with the smallest ones (usually contained within the former). 
//	   Therefore, we consider 2D feature elements with smaller bounding rectangles 
//	   as lower level elements and therefore they will be selected next.

//	3. Designers typically begin by drawing additive 2D feature elements 
//	   (guides, steps, and bumps), and then proceed to subtractive 2D feature elements
//	   (slots, shells, and holes). 
//	   Therefore, in the case of feature elements 
//	   with similar delimiting rectangles, 
//	   the one that represents a subtractive feature 
//	   will be selected as the lowest level element.

//	4. Designers tend to first draw 2D feature elements 
//	   that are located on faces parallel to the coordinate planes, 
//	   before those located on inclined faces. 
//	   Therefore, in case of ambiguity, the one linked with inclined faces 
//	   will be selected as the lowest level element.

//	5. In the rest of the cases, the most probable feature will be selected 
//	   (the one with a higher figure of merit).

//To search for links that allow finding the parent features, 
//the algorithm also tries to replicate the behavior observed in designers 
//when they produce CSG models through 2D CAD applications:

//	6. The larger 2D feature elements will most likely act as containers, 
//	   with the smaller ones being the contained elements. 
//	   Therefore, we consider the content-container link to be related 
//	   to both the size and the relative position between two elements. 
//	   Consequently, we understand that a feature 
//	   is a candidate to be the parent of another if it is larger and contains it.

//	7. Contact between faces is also an important indication of parent-child link, 
//	   therefore we try to detect pairs of coplanar faces belonging respectively 
//	   to the child feature and to the candidate feature to be the parent.
{
	//Default
	long FeatureId= 0;

	//Select best next feature to deconstruct
	if(!FeatureFather){
		GetSmallestFeature(DDBB_Dec_Current, NextFeat, FeatureId);
	}
	else{
		//Assume as previous the last feature added to the current branch
		CFeature PrevFeature= DB_CSG_Model->GetFeatureFromBranch(DB_CSG_Model->BranchSize()-1);

		GetFatherFeature(DDBB_Dec_Current, NextFeat, FeatureId, PrevFeature);
	}

	//Check core and update father faces
	if (NextFeat.GetType() == NODE || NextFeat.GetType() == CORE){
		FeatureFather= false;

		if((NextFeat.GetType() == NODE) && (GetNumValidFeatures() == 0)){
			NextFeat.SetType(CORE);
		}
	}

	else{
		FeatureFather= GetFatherFaces(DDBB_Dec_Current, NextFeat, FeatureId);
	}

	return FeatureId;
}

//--End SelectFeature-----------------------------------------




//--Begin CompleteFeatureData----------------------------------------


			//--Begin FacesFeature----------------------------------------------
void CCSG_Deconstruct::GetCoreBases(CDB_Model *DDBB_Dec_Current, 
									CDB_CSG *DB_CSG_Model,
									long &face, long &base1, long &base2)
//Identifies the faces of the subgraph that define the core feature

//The applied criterion is that the face with the most vertices 
//should be the base of the extrusion.

//The rationale is that defining the most complex face as a profile 
//usually minimizes the number of extrusions required to obtain the solid.

//Obviously, two opposite bases are required (base1 and base2)

//If more than two faces have the same maximum number of vertices,
//the pair of larger and opposite ones is selected,
//except for wedges with two opossite triangular faces.

//A third "face" that connects both bases is returned as well
{
	//Search for the face (or faces) with the most vertices
	std::vector<long> MostVerticesFaces;
	long numvert= 0;
	for(long i= 0; i<(long)DDBB_Dec_Current->GetSizeFaces(); i++){
		long numVertI= DDBB_Dec_Current->GetSizeVerticesInFace(i);
		if(numVertI > numvert){
			MostVerticesFaces.clear();
			numvert= numVertI;
			MostVerticesFaces.push_back(i);
		}
		else if(numVertI == numvert)
			MostVerticesFaces.push_back(i);
	}

	//If all faces have the same maximum number of vertices 
	//(they should be six quadrilaterals), 
	//we take as bases those that are larger (and opposite)
	if(MostVerticesFaces.size() == DDBB_Dec_Current->GetSizeFaces()){
		MostVerticesFaces.clear();

		std::vector<double> areas;
		std::vector<long> bigFaces;
		long numFaces= (long)DDBB_Dec_Current->GetSizeFaces();
		for(long i= 0; i<numFaces; i++){
			areas.push_back(GetFaceArea(DDBB_Dec_Current, i));
			bigFaces.push_back(i);
		}
		SortMajorFirst(areas, bigFaces);

		//The first base face is the largest one
		MostVerticesFaces.push_back(bigFaces[0]);
		std::vector<long> vertices1= DDBB_Dec_Current->GetAllVerticesInFace(bigFaces[0]);

		//The second base face is the largest one that does not share any vertex with the first (is opposite)
		for(long i= 1; i<numFaces; i++){
			std::vector<long> vertices2= DDBB_Dec_Current->GetAllVerticesInFace(bigFaces[i]);
			if(!CommonVerticesInVectors(vertices1, vertices2)){
				MostVerticesFaces.push_back(bigFaces[i]);
				break;
			}
		}
	}

	//If exactly two candidate bases have been found, save them as bases
	if(MostVerticesFaces.size() == 2){
		base1= MostVerticesFaces[0];
		base2= MostVerticesFaces[1];

		//Select the first extruded face
		for(long j= 0; j<(long)DDBB_Dec_Current->GetSizeFaces(); j++){
			if((j!= base1) && (j!= base2)){
				face= j;
				break;
			}
		}
	}

	else{
		//Detect wedges, as they have two triangular bases 
		//and three lateral quadrilateral faces
		long numFaces= (long)DDBB_Dec_Current->GetSizeFaces();
		if (numFaces == 5){
			std::vector<long> TriangularFaces;
			std::vector<long> QuadrilateralFaces;
			for(long i= 0; i<numFaces; i++){
				if(DDBB_Dec_Current->GetSizeVerticesInFace(i) == 3){
					TriangularFaces.push_back(i);
				}
				else if(DDBB_Dec_Current->GetSizeVerticesInFace(i) == 4){
					QuadrilateralFaces.push_back(i);
				}
			}

			if (TriangularFaces.size() == 2 && QuadrilateralFaces.size() == 3){
				base1= TriangularFaces[0];
				base2= TriangularFaces[1];
				face= QuadrilateralFaces[0];  //Arbitrary selection of one of the lateral faces
			}
		}
	}
}


void CCSG_Deconstruct::FacesFeature(CDB_Model *DDBB_Dec_Current,
									CDB_CSG *DB_CSG_Model,
									CFeature &NewFeat, 
									FEATURE_TYPE type, 
									long FeatureId,
									long &base1, long &base2, long &face,
									std::vector<long> &faces)
//Select faces of the current feature "FeatureId"
{
	switch (type){
		case RIB:{
			//Border face (+)
			for(long i=0; i<(long)ReferApp.Fe.RD.Ribs.size(); i++){
				if(ReferApp.Fe.RD.Ribs[i] == FeatureId) faces.push_back(i);
			}
			//Lateral faces (-)
			for(long i=0; i<(long)ReferApp.Fe.RD.Ribs.size(); i++){
				if(ReferApp.Fe.RD.Ribs[i] == -FeatureId) faces.push_back(i);
			}
			base1= faces[1];
			base2= faces[2];
			face= faces[0];
		}
		break;

		case DIVIDER:{
			//Border faces (+)
			for(long i=0; i<(long)ReferApp.Fe.RD.Dividers.size(); i++){
				if(ReferApp.Fe.RD.Dividers[i] == FeatureId) faces.push_back(i);
			}
			//Lateral faces (-)
			for(long i=0; i<(long)ReferApp.Fe.RD.Dividers.size(); i++){
				if(ReferApp.Fe.RD.Dividers[i] == -FeatureId) faces.push_back(i);
			}
			base1= faces[faces.size()-2];
			base2= faces[faces.size()-1];
			face= faces[0];
		}
		break;

		case RAIL:{
			//Bottom face (+)
			for(long i=0; i<(long)ReferApp.Fe.RS.Rails.size(); i++){
				if(ReferApp.Fe.RS.Rails[i] == FeatureId) faces.push_back(i);
			}
			//Lateral faces (-)
			for(long i=0; i<(long)ReferApp.Fe.RS.Rails.size(); i++){
				if(ReferApp.Fe.RS.Rails[i] == -FeatureId) faces.push_back(i);
			}
		}
		break;

		case SLOT:{
			//Bottom face (+)
			for(long i=0; i<(long)ReferApp.Fe.RS.Slots.size(); i++){
				if(ReferApp.Fe.RS.Slots[i] == FeatureId) faces.push_back(i);
			}
			//Lateral faces (-)
			for(long i=0; i<(long)ReferApp.Fe.RS.Slots.size(); i++){
				if(ReferApp.Fe.RS.Slots[i] == -FeatureId) faces.push_back(i);
			}
		}
		break;

		case STEP:{
			//Lateral faces (+)
			for(long i=0; i<(long)ReferApp.Fe.SP.Steps.size(); i++){
				if(ReferApp.Fe.SP.Steps[i] == FeatureId) faces.push_back(i);
			}
			//Base faces (-)
			for(long i=0; i<(long)ReferApp.Fe.SP.Steps.size(); i++){
				if(ReferApp.Fe.SP.Steps[i] == -FeatureId) faces.push_back(i);
			}
			base1= faces[faces.size()-1];
			base2= faces[faces.size()-2];
			face= faces[0];
		}
		break;

		case POCKET:{
			//Base faces (-)
			for(long i=0; i<(long)ReferApp.Fe.SP.Pockets.size(); i++){
				if(ReferApp.Fe.SP.Pockets[i] == FeatureId) faces.push_back(i);
			}
			//Lateral faces (+)
			for(long i=0; i<(long)ReferApp.Fe.SP.Pockets.size(); i++){
				if(ReferApp.Fe.SP.Pockets[i] == -FeatureId) faces.push_back(i);
			}
			base1= faces[faces.size()-1];
			base2= faces[faces.size()-2];
			face= faces[0];
		}
		break;

		case NODE:{
			//Save faces of the node
			for(long i=0; i<(long)DDBB_Dec_Current->GetSizeFaces(); i++){
				faces.push_back(i);
			}
		}
		break;

		case CORE:{
			//Check if the core is a prism
			CCueTypology TY;
			if (TY.IsPrism(DDBB_Dec_Current, base1, base2) > 0.5){   //At least 50% probability of being prismatic
				//Recalculate base faces (with more sophisticated checks)
				GetCoreBases(DDBB_Dec_Current, DB_CSG_Model, face, base1, base2);
				double meritCore= TY.IsPrism(DDBB_Dec_Current, base1, base2);
				NewFeat.SetMerit(meritCore);
			}

			//Save faces of the core
			for(long i=0; i<(long)DDBB_Dec_Current->GetSizeFaces(); i++){
				faces.push_back(i);
			}
		}
		break;
	}
}
			//--End FacesFeature----------------------------------------------



void CCSG_Deconstruct::RemoveElementFromVector(long Element, std::vector<long> &Vector)
//Searches the element "Element" in vector "Vector", and removes it
{
	for(long i=0; i<(long)Vector.size(); i++){
		if (Element == Vector[i]){
			Vector.erase(Vector.begin()+i, Vector.begin()+i+1);
			break;
		}
	}
}


std::vector<long> CCSG_Deconstruct::GetEdgesIntersectedByFaces(CDB_Model *DDBB_Dec_Current, long face1, long face2)
//Returns the edge obtained by the intersection between face 1 and face2
{
	std::vector<long> edges;

	CFace Face1= DDBB_Dec_Current->GetFace(face1);
	std::vector<long> EdgesInFace1= Face1.GetAllEdgesInFace();

	CFace Face2= DDBB_Dec_Current->GetFace(face2);
	std::vector<long> EdgesInFace2= Face2.GetAllEdgesInFace();

	for(long i=0; i<(long)EdgesInFace2.size(); i++){
		for(long j=0; j<(long)EdgesInFace1.size();j++){
			if (EdgesInFace2[i]== EdgesInFace1[j])
				edges.push_back(EdgesInFace1[j]);
		}
	}
	return edges;
}


void CCSG_Deconstruct::DetectOppositeFaces(CDB_Model *DDBB_Dec_Current, long bottom, 
										   std::vector<long> &laterals)
{
	if (laterals.size() > 2){
		std::vector<long> RemovingFaces;

		CFace FaceBot= DDBB_Dec_Current->GetFace(bottom);
		std::vector<long>BottomEdges= FaceBot.GetAllEdgesInFace();
		long pon= (long) BottomEdges.size()/2;

		for(long i=0; i<(long)laterals.size(); i++){
			std::vector<long> EdgesInCommon= GetEdgesIntersectedByFaces(DDBB_Dec_Current, bottom, laterals[i]);
			long Position;
			IsValueInVector(EdgesInCommon[0], BottomEdges, Position);
			Position+= pon;
			std::vector<long> FacesToCompare= DDBB_Dec_Current->GetFacesSharingEdge(BottomEdges[Position%BottomEdges.size()]);

			if(FacesToCompare[1] == bottom){
				if(!IsValueInVector(FacesToCompare[0], laterals))
					RemovingFaces.push_back(laterals[i]);
			}
			else if(!IsValueInVector(FacesToCompare[1], laterals))
					RemovingFaces.push_back(laterals[i]);
		}

		RemovingFaces= SortMajorFirst(RemovingFaces);
		laterals= SortMajorFirst(laterals);

		for(long k=0; k<(long)RemovingFaces.size(); k++){ 
			RemoveElementFromVector(RemovingFaces[k], laterals);
		}
		laterals.insert(laterals.begin(),bottom);
	}
}


void CCSG_Deconstruct::RailSlotExtrusion(CDB_Model *DDBB_Dec_Current, 
										 std::vector<long> &faces,
										 std::vector<long> &PairedVertL1,
										 std::vector<long> &PairedVertL2,
										 long &direction, double &ExtrudedLength)
//Return the direction and module of the extrusion
//required to build an extruded rail or slot features 
//from face PairedVertL1 to face PairedVertL2

//Note that the vertices of the faces are paired

//Faces of the feature are updated as well
{
	ExtrudedLength= 0;
	std::vector<long> ParallelEdges;
	long bottom= faces[0];

	if(faces.size() > 3){  //if NOT a rectangular rail/slot
		//Update opposite faces
		faces.erase(faces.begin(), faces.begin()+1);
		DetectOppositeFaces(DDBB_Dec_Current, bottom, faces);
	}

	PairVertices(DDBB_Dec_Current, bottom, faces[1], faces[2],
				 PairedVertL1, PairedVertL2);  //Updates PairedVertL1 and PairedVertL2

	//Calculate average length and direction
	for(long r= 1; r<(long)faces.size(); r++){
		std::vector<long> edge= GetEdgesIntersectedByFaces(DDBB_Dec_Current, bottom, faces[r]);
		ParallelEdges.push_back(edge[0]);
		edge.clear();
	}

	for(long i= 0; i<(long)ParallelEdges.size(); i++){
		ExtrudedLength += DDBB_Dec_Current->GetLength2D(ParallelEdges[i]);
	}
	CEdge EdgeDirection= DDBB_Dec_Current->GetEdge(ParallelEdges[0]);

	//Save length and direction 
	direction= IsMainDirection(DDBB_Dec_Current,
							   DDBB_Dec_Current->GetVertex(EdgeDirection.Head),
							   DDBB_Dec_Current->GetVertex(EdgeDirection.Tail),
						   	   MainAngles);
	double reduction = 1;
	if (direction != -1)
		reduction= ReductionCoefficients[direction];

	ExtrudedLength= ExtrudedLength/ParallelEdges.size();
	ExtrudedLength= ExtrudedLength/reduction;  //Non-reduced length
}


void CCSG_Deconstruct::ExtrusionDirectionAndLength(CDB_Model *DDBB_Dec_Current, 
												   std::vector<long> PairedVertL1,
												   std::vector<long> PairedVertL2,
												   long &direction, double &ExtrudeLength)
//Returns the direction and module of the extrusion
//required to build an extruded feature 
//from face PairedVertL1 to face PairedVertL2

//From the base faces, the extrusion length is calculated 
//as the average distance between paired nodes of the base faces
//multiplied by the scale corresponding to the extrusion direction.

//Note that the vertices of the faces must have been paired in advance

//WARNING: only main directions are detected and returned!!
{
	//Calculate average length and direction
	double ExtrudeAngle= 0;
	ExtrudeLength= 0;

	long numVertices= (long)PairedVertL1.size();

	for(long i= 0; i<numVertices; i++){
		//Angle [0, 180) of the non-oriented segment
		CVertex Vertex1= DDBB_Dec_Current->GetVertex(PairedVertL1[i]);
		CVertex Vertex2= DDBB_Dec_Current->GetVertex(PairedVertL2[i]);
		POINT2D p1(Vertex1.x, Vertex1.y);
		POINT2D p2(Vertex2.x, Vertex2.y);
		ExtrudeAngle+= GetAngle2D_PI(p1, p2);

		ExtrudeLength += DDBB_Dec_Current->GetDistance2D(PairedVertL1[i], PairedVertL2[i]);
	}

	ExtrudeAngle= ExtrudeAngle/numVertices;      //Average angle in range [0, 180)
	direction= IsMainDirection(ExtrudeAngle, MainAngles);

	ExtrudeLength= ExtrudeLength/numVertices;  //Average length
	double reduction = 1;
	if (direction != -1)
		reduction= ReductionCoefficients[direction];
	ExtrudeLength= ExtrudeLength/reduction;  //Non-reduced length
}


void CCSG_Deconstruct::AddVerticesOfFaces(CDB_Model *DDBB_Dec_Current, 
										  CDB_Model *DDBB_F2D, 
										  std::vector<long> faces)
//Adds vertices of the feature 
//from the dababase DDBB_Dec_Current (current deconstruction) 
//to the database DDBB_F2D (Feature.Geometry)

//Adds all the vertices of the faces from the list "faces"
{
	std::vector<long> vertices;

	for(long j=0; j<(long)faces.size(); j++){
		CFace face= DDBB_Dec_Current->GetFace(faces[j]);
		if (j==0)
			vertices= DDBB_Dec_Current->GetAllVerticesInFace(face);
		else{
			std::vector<long> temporal= DDBB_Dec_Current->GetAllVerticesInFace(face);
			for(long f=0; f<(long) temporal.size(); f++){
				long vert= temporal[f];
				if(!IsValueInVector(vert, vertices)) 
					vertices.push_back(vert);
			}
		}
	}

	//Add vertices backwards to minimize database updates
	vertices= SortMajorFirst(vertices);

	long total= (long)vertices.size();
	for(long h=0; h<total; h++){
		CVertex vert= DDBB_Dec_Current->GetVertex(vertices[total-1-h]);
		DDBB_F2D->AddVertex(vert);
	}
}


void CCSG_Deconstruct::AddEdgesOfFaces(CDB_Model *DDBB_Dec_Current, 
									   CDB_Model *DDBB_F2D, 
									   std::vector<long> faces)
//Adds edges of the feature 
//from the dababase DDBB_Dec_Current (current deconstruction) 
//to the database DDBB_F2D (Feature.Geometry)

//Adds all the edges of the faces from the list "faces"
{
	std::vector<long> edges;
	for(long j=0; j<(long)faces.size(); j++){
		CFace face= DDBB_Dec_Current->GetFace(faces[j]);
		if (j==0)
			edges= face.GetAllEdgesInFace();
		else{
			std::vector<long> temporal= face.GetAllEdgesInFace();
			for(long f=0; f<(long) temporal.size(); f++){
				long edg= temporal[f];
				if(!IsValueInVector(edg, edges)) 
					edges.push_back(edg);
			}
		}
	}

	//Add edges backwards to minimize database updates
	long total= (long)edges.size();
	for(long h=0; h<total; h++){
		//Recover vertices from current deconstruction
		CEdge edg= DDBB_Dec_Current->GetEdge(edges[total-1-h]);
		CVertex VHead= DDBB_Dec_Current->GetVertex(edg.Head);
		CVertex VTail= DDBB_Dec_Current->GetVertex(edg.Tail);

		//Search for twin vertices as renumbered in current feature
		edg.Head= DDBB_F2D->GetVertexNum(VHead);
		edg.Tail= DDBB_F2D->GetVertexNum(VTail);
		DDBB_F2D->AddEdge(edg);
	}
}


void CCSG_Deconstruct::CompleteEdges(CDB_Model *DDBB_Dec_Current, CDB_Model *DDBB_F2D,
									 std::vector<long> PairedVertL1,
									 std::vector<long> PairedVertL2)
//There are missing edges in rails/slots or ribs/dividers,
//as they always interrupt at least one edge 
//of the containing faces of the parent element
{
	//Loop to search and add missing edges between paired vertices
	for(long i= 0; i<(long)PairedVertL1.size(); i++){
		long EdgePos= DDBB_Dec_Current->GetEdgeByVerticesNums(PairedVertL1[i], PairedVertL2[i]);
		if(EdgePos == -1){
			//Add edge to the feature
			CVertex Vertex1= DDBB_Dec_Current->GetVertex(PairedVertL1[i]);
			CVertex Vertex2= DDBB_Dec_Current->GetVertex(PairedVertL2[i]);
			DDBB_F2D->AddEdgeByVertices(Vertex1.x, Vertex1.y, Vertex1.z, 
										Vertex2.x, Vertex2.y, Vertex2.z);
		}
	}
}


void CCSG_Deconstruct::AddDrawingFaces(CDB_Model *DDBB_Dec_Current, 
									   CDB_Model *DDBB_F2D, 
									   std::vector<long> faces)
//Adds faces of the feature 
//from the dababase DDBB (current deconstruction) 
//to the database NFeature (Feature.Geometry)

//Adds all the faces from the list "faces"

{
	for(long i=0; i<(long)faces.size(); i++){
		CFace NewFeatureFace;
		CFace DDBBface= DDBB_Dec_Current->GetFace(faces[i]);
		std::vector<long> EdgesInDDBBface= DDBBface.GetAllEdgesInFace();
		for(long j= 0; j<(long) EdgesInDDBBface.size(); j++){
			CEdge DDBBedge= DDBB_Dec_Current->GetEdge(EdgesInDDBBface[j]);

			//Search for vertices as renumbered in current feature
			long NFVertH= DDBB_F2D->GetVertexNum(DDBB_Dec_Current->GetVertex(DDBBedge.Head));
			long NFVertT= DDBB_F2D->GetVertexNum(DDBB_Dec_Current->GetVertex(DDBBedge.Tail));

			long NewEdge= DDBB_F2D->GetEdgeByVerticesNums(NFVertH, NFVertT);
			NewFeatureFace.AddEdgeInFace(NewEdge);
		}

		DDBB_F2D->AddFace(NewFeatureFace);
	}
}


void CCSG_Deconstruct::AddDrawingToFeature(CDB_Model *DDBB_Dec_Current, 
										   CFeature &NewFeat, 
										   std::vector<long> faces,
										   std::vector<long> PairedVertL1,
										   std::vector<long> PairedVertL2)
//Updates NewFeat by adding the geometry of the feature

//Extracts information from the drawing of the database DDBB_Dec_Current (current deconstruction), 
//to update the Geometrty of the feature NewFeat

//Add all the vertices, edges and faces linked to the faces from the list "faces"
{
	CDB_Model DDBB_F2D= NewFeat.GetFeature2D();

		AddVerticesOfFaces(DDBB_Dec_Current, &DDBB_F2D, faces);

		AddEdgesOfFaces(DDBB_Dec_Current, &DDBB_F2D, faces);

		//Complete broken lines
		FEATURE_TYPE type= NewFeat.GetType();
		if(type == RIB || type == DIVIDER || type == RAIL || type == SLOT)
			CompleteEdges(DDBB_Dec_Current, &DDBB_F2D, PairedVertL1, PairedVertL2);

		AddDrawingFaces(DDBB_Dec_Current, &DDBB_F2D, faces);

	NewFeat.SetFeature2D(DDBB_F2D);
}


void CCSG_Deconstruct::PutSubgraphData(CDB_Model *DDBB_Dec_Current, CFeature &NewFeat)
//Assign to the feature the subgraph where its first vertex belongs
{
	//Subgraph of the first vertex
	CVertex vertex0= NewFeat.GetFeature2D().GetVertex(0);  //F2D Read only
	long vert0= DDBB_Dec_Current->GetVertexNum(vertex0);  //Vertex renumbering in current deconstruction
	long subGraph= ReferApp.Cu.SG.SubgraphForVertex[vert0];

	NewFeat.SetSubGraph(subGraph);
}


void CCSG_Deconstruct::SetCrossSectionRS(CDB_Model *DDBB_Dec_Current,
										 CDB_Model *DB_DatumGraph,
										 long FeatureId, 
										 CFeature &NewFeat)
//Calculate cross section of features type RAIL or SLOT

//The vertices shared by the feature and the container face are selected
{
	long ContainerFace;
	if(NewFeat.GetType() == SLOT)
		ContainerFace= ReferApp.Fe.RS.ContainerSlotFaces[FeatureId-1];
	else 
		ContainerFace= ReferApp.Fe.RS.ContainerRailFaces[FeatureId-1];

	std::vector<long>VerticesContainerFace= DDBB_Dec_Current->GetAllVerticesInFace(DDBB_Dec_Current->GetFace(ContainerFace));

	std::vector<CVertex>VerticesFeature;
	NewFeat.GetFeature2D().CopyVerticesToVector(VerticesFeature);  //F2D Read only

	for(long i= 0; i<(long)VerticesContainerFace.size(); i++){
		CVertex vertex= DDBB_Dec_Current->GetVertex(VerticesContainerFace[i]);
		if(IsVertexInVector(vertex, VerticesFeature))
			//Select in DatumGraph the twin of DDBB_Dec_Current vertex
			NewFeat.AddCrossSectionVertex(DB_DatumGraph->GetVertexNum(vertex));
	}
}


void CCSG_Deconstruct::SetCrossSectionSP(CDB_Model *DDBB_Dec_Current,
										 CDB_Model *DB_DatumGraph,
										 std::vector<long> PairedVertL1,
										 std::vector<long> PairedVertL2,
										 CFeature &NewFeat)
//Calculate cross section of features type STEPS or POCKETS

//Select the base face contained in the container face
//If both bases are included, select the one with the most centered centroid
{
	if(NewFeat.ContainerFacesSize() <= 0){
		return;
	}

	//Calculate if any of the two bases (PairedVertL1, PairedVertL2)
	//are fully contained inside the current container face
	std::vector<POINT2D> ContainerFacePolygon;
	for (long i=0; i<NewFeat.ContainerFaceSize(0); i++){
		CVertex vI= DDBB_Dec_Current->GetVertex(NewFeat.ContainerFaceGetVertex(0, i));
		POINT2D pI(vI.x, vI.y);
		ContainerFacePolygon.push_back(pI);
	}

	std::vector<long> InnerBases;

	bool inside1= true;
	for (long i=0; i<(long)PairedVertL1.size(); i++){
		CVertex vI= DDBB_Dec_Current->GetVertex(PairedVertL1[i]);
		POINT2D pI(vI.x, vI.y);
		if(!isPointInPolygon(ContainerFacePolygon, pI)){
			inside1= false;
			break;
		}
	}
	if(inside1)
		InnerBases.push_back(0);

	bool inside2= true;
	for (long i=0; i<(long)PairedVertL2.size(); i++){
		CVertex vI= DDBB_Dec_Current->GetVertex(PairedVertL2[i]);
		POINT2D pI(vI.x, vI.y);
		if(!isPointInPolygon(ContainerFacePolygon, pI)){
			inside2= false;
			break;
		}
	}
	if(inside2)
		InnerBases.push_back(1);

	//Select the best candidate base face
	long numBase= -1;
	if(InnerBases.size()>1){
		//If both bases are in the ContainerFace, 
		//the one whose centroid is next to the centroid of the container face is selected
		CVertex ContCentroid= GetFaceCentroid(DDBB_Dec_Current, DDBB_Dec_Current->GetFaceByVertices(NewFeat.GetContainerFace(0))); 
		CVertex B1Centroid= GetFaceCentroid(DDBB_Dec_Current, DDBB_Dec_Current->GetFaceByVertices(PairedVertL1));
		CVertex B2Centroid= GetFaceCentroid(DDBB_Dec_Current, DDBB_Dec_Current->GetFaceByVertices(PairedVertL2));
		double dist1= MODULE_2D(ContCentroid, B1Centroid);
		double dist2= MODULE_2D(ContCentroid, B2Centroid);

		if ((dist1<dist2) || (dist1==dist2))
			numBase= 0;
		else
			numBase= 1;
	}
	else{
		//Otherwise, the only base fully contained in the ContainerFace is selected
		if(InnerBases.size()>0)
			numBase= InnerBases[0];
	}

	//Save cross section vertices
	if(numBase == 0){
		for(long i=0; i<(long)PairedVertL1.size(); i++){
			//Select in DatumGraph the twin of DDBB_Dec_Current paired vertex
			NewFeat.AddCrossSectionVertex(DB_DatumGraph->GetVertexNum(DDBB_Dec_Current->GetVertex(PairedVertL1[i])));
		}
	}
	else if(numBase == 1){
		for(long i=0; i<(long)PairedVertL2.size(); i++){
			//Select in DatumGraph the twin of DDBB_Dec_Current paired vertex
			NewFeat.AddCrossSectionVertex(DB_DatumGraph->GetVertexNum(DDBB_Dec_Current->GetVertex(PairedVertL2[i])));
		}
	}
}


void CCSG_Deconstruct::SetCrossSectionMid(CDB_Model *DDBB_Dec_Current, 
										  CDB_Model *DB_DatumGraph,
										  std::vector<long> PairedVertL1, 
										  std::vector<long> PairedVertL2, 
										  CFeature &Feature)
//Cross section is parallel to the bases, and contained in the symmetry plane

//The mid points of the segments connecting their paired vertices
//are select as cross section vertices

//The datum graph is updated accordingly
{
	//Loop over all the paired vertices to select their midpoints as cross section vertices
	for(long i= 0; i<(long) PairedVertL1.size(); i++){
		//Select in DatumGraph the twins of DDBB_Dec_Current paired vertices
		long V1= DB_DatumGraph->GetVertexNum(DDBB_Dec_Current->GetVertex(PairedVertL1[i]));
		long V2= DB_DatumGraph->GetVertexNum(DDBB_Dec_Current->GetVertex(PairedVertL2[i]));

		CVertex Vcc1= DB_DatumGraph->GetVertex(V1);
		CVertex Vcc2= DB_DatumGraph->GetVertex(V2);

		//Check if vertices are already conected by one single edge or a set of sub-edges
		long midV;

		//Search for a chain of collinear (and consecutive) edges
		std::vector<long> chain;
		if (GetChainedEdges(DB_DatumGraph, 
							V1, V2, 
							ReferApp.CSG.CSG_ThresholdAngle, 
							chain)){

			//Search for the mid point of the chain
			//(adds the corresponding vertex, if required)
			CVertex Vcc1= DB_DatumGraph->GetVertex(V1);
			CVertex Vcc2= DB_DatumGraph->GetVertex(V2);
			CVertex midVert(((Vcc1.x + Vcc2.x)/2),
							((Vcc1.y + Vcc2.y)/2),
							((Vcc1.z + Vcc2.z)/2));

			midV= getVertexInChain(DB_DatumGraph, 
								   V1, V2, 
								   midVert,
								   chain);
		}

		else{
			//Add the midpoint to the datum graph, if required
			CVertex midVert;
			midVert.x= (Vcc1.x + Vcc2.x)/2;
			midVert.y= (Vcc1.y + Vcc2.y)/2;
			midVert.z= (Vcc1.z + Vcc2.z)/2;
			midV= DB_DatumGraph->GetVertexNum(midVert); // check if vertex already exists

			if(midV == -1){
				DB_DatumGraph->AddVertex(midVert);
				midV= DB_DatumGraph->GetSizeVertices()-1;
			}

			//Add semi-edges, as none edge existed
			CEdge Edge1(midV, V1);
			DB_DatumGraph->AddEdge(Edge1);

			CEdge Edge2(V2, midV);
			DB_DatumGraph->AddEdge(Edge2);
		}

		//Add mid vertex to cross section
		Feature.AddCrossSectionVertex(midV);
	}
}


void CCSG_Deconstruct::SetCrossSection(CDB_Model *DDBB_Dec_Current, 
									   CDB_Model *DB_DatumGraph,
									   std::vector<long> PairedVertL1, 
									   std::vector<long> PairedVertL2,
									   CFeature &NewFeat, long FeatureId)
//Determines the cross-sections used in sweep operations to construct the features,
//and defines their related reference system.

//In an extrusion, the base faces of the feature
//remain constant throughout the extrusion,
//are parallel to the cross-section
//and perpendicular to the extrusion direction.

//Besides, the cross-section must be contained 
//in the symmetry plane, or the container face.
{
	//Get cross section for each particular type of feature
	switch (NewFeat.GetType()){
		case RAIL:{
			SetCrossSectionRS(DDBB_Dec_Current, 
							 DB_DatumGraph, 
							 FeatureId, 
							 NewFeat);
		}
		break;

		case SLOT:{
			SetCrossSectionRS(DDBB_Dec_Current, 
							 DB_DatumGraph, 
							 FeatureId, 
							 NewFeat);
		}
		break;

		case RIB:{
			SetCrossSectionMid(DDBB_Dec_Current, 
							   DB_DatumGraph, 
							   PairedVertL1, 
							   PairedVertL2, 
							   NewFeat);
		}
		break;

		case DIVIDER:{
			SetCrossSectionMid(DDBB_Dec_Current, 
							   DB_DatumGraph, 
							   PairedVertL1, 
							   PairedVertL2, 
							   NewFeat);
		}
		break;

		case STEP:{
			SetCrossSectionSP(DDBB_Dec_Current, 
							  DB_DatumGraph, 
							  PairedVertL1, 
							  PairedVertL2, 
							  NewFeat);
		}
		break;

		case POCKET:{
			SetCrossSectionSP(DDBB_Dec_Current, 
							  DB_DatumGraph, 
							  PairedVertL1, 
							  PairedVertL2, 
							  NewFeat);
		};
		break;

		case NODE:{
			bool doNothing= true;
		}
		break;

		case CORE:{
			SetCrossSectionMid(DDBB_Dec_Current,
							   DB_DatumGraph, 
							   PairedVertL1, 
							   PairedVertL2, 
							   NewFeat);
		}
		break;
	}

	//Add cross section edges to datumgraph
	std::vector<long> vertices= NewFeat.GetCrossSection();
	long numVertices= (long)vertices.size();
	for(long i= 0; i<numVertices; i++){
		std::vector<long> chain;
		if (!GetChainedEdges(DB_DatumGraph, 
							 vertices[i], vertices[(i+1)%numVertices], 
							 ReferApp.CSG.CSG_ThresholdAngle, 
							 chain)){
			CEdge edge(vertices[i], vertices[(i+1)%numVertices]);
			DB_DatumGraph->AddEdge(edge);
		}
	}
}


void CCSG_Deconstruct::ProfilePlaneOrientation(CDB_Model *DB_DatumGraph,
											   CDB_CSG *DB_CSG_Model, 
											   CFeature &Feature)
//Return the orientation of the reference plane 
//that contains the profile used to extrude the feature

//The reference plane is perpendicular to the extrusion direction

//Alternatively, it must be coplanar with:
//	- The plane of the cross section, or
//	- The container face (for steps and pockets)
{
	CPlaneOrientation Plane= UNKNOWN;

	long dir= Feature.GetExtrusionDirection();

	if (dir == 2){
		Plane= XY;
	}
	else if (dir == 1){
		Plane= XZ;
	}
	else if (dir == 0){
		Plane= YZ;
	}
	else{
		//Try to check if parallel to the cross section plane
		std::vector<long> CrossSectionVertices= Feature.GetCrossSection();
		Plane= PlaneOrientation(DB_DatumGraph,
								DB_CSG_Model,
								CrossSectionVertices, 
								MainAngles);

		//If feature extrusion plane is not yet defined, try the container face.
		//WARNING:  Repasar... creo que sólo funcionará con steps y pockets!!!!!!!!!
		if ((Plane== UNKNOWN) && (Feature.ContainerFacesSize() != 0)) {
			//CVertex Vertex1= DDBB_Dec_Current->GetVertex(Feature.ContainerFaceGetVertex(0, 0));
			CVertex Vertex1= DB_DatumGraph->GetVertex(Feature.ContainerFaceGetVertex(0, 0));
			bool x= false, y= false, z= false;
			for(long j= 0; j<Feature.ContainerFaceSize(0); j++){
				//CVertex Vertex2= DDBB_Dec_Current->GetVertex(Feature.ContainerFaceGetVertex(0, (j+1) % Feature.ContainerFaceSize(0)));
				CVertex Vertex2= DB_DatumGraph->GetVertex(Feature.ContainerFaceGetVertex(0, (j+1) % Feature.ContainerFaceSize(0)));

				//long direction= IsMainDirection(DDBB_Dec_Current, Vertex1, Vertex2, MainAngles);
				long direction= IsMainDirection(DB_DatumGraph, Vertex1, Vertex2, MainAngles);

				if ((direction== 0)&&(!x)) x= true;
				if ((direction== 1)&&(!y)) y= true;
				if ((direction== 2)&&(!z)) z= true;

				Vertex1= Vertex2;
			}
			if (x && y && !z)  Plane= XY;
			if (x && !y && z)  Plane= XZ;
			if (!x && y && z)  Plane= YZ;
		}

		//If still not parallel to main axes, then the plane is slanted
		if(Plane== UNKNOWN)
			Plane= SLANTED;
	}

	Feature.SetProfileOrientation(Plane);
}


//--Begin DefineDatumPlanes----------------------------------------------


void CCSG_Deconstruct::SplitEdge(CDB_Model *DB_DatumGraph,
								 CVertex V1, CVertex V2, long midVertex)
//Update datum graph edge V1-V2 by adding an intermediate vertex
//and splitting the original edge

//Note that coordinates of V1 and V2 come from the current deconstruction
{
	//Search in the datum graph for twin vertices of the current deconstruction
	long vv1= DB_DatumGraph->GetVertexNum(V1);
	long vv2= DB_DatumGraph->GetVertexNum(V2);

	long DB_Edge= DB_DatumGraph->GetEdgeByVerticesNums(vv1, vv2);

	if(DB_Edge != -1){
		//Split the current edge
		CEdge DB_ModifEdge= DB_DatumGraph->GetEdge(DB_Edge);
		DB_ModifEdge.Head= vv1;
		DB_ModifEdge.Tail= midVertex;
		DB_DatumGraph->SetEdge(DB_Edge, DB_ModifEdge);

		CEdge DB_NewEdge;
		DB_NewEdge.Head= midVertex;
		DB_NewEdge.Tail= vv2;
		DB_DatumGraph->AddEdge(DB_NewEdge);
	}
	else{
		//Create two new sub-edges
		CEdge NewEdge;
		NewEdge.Tail= vv1;
		NewEdge.Head= midVertex;
		DB_DatumGraph->AddEdge(NewEdge);

		NewEdge.Tail= midVertex;
		NewEdge.Head= vv2;
		DB_DatumGraph->AddEdge(NewEdge);
	}
}


void CCSG_Deconstruct::GetSymmetryPlaneForRS(CDB_Model *DDBB_Dec_Current, 
											 CDB_Model *DB_DatumGraph,
											 CFeature Feature, 
											 std::vector<long> PairedVertL1, 
											 std::vector<long> PairedVertL2, 
											 std::vector<long> &Vertices)
//Search the midpoints of the paired vertices
//and add them to the DB_DatumGraph 
{
	for (long j= 0; j< (long) PairedVertL1.size(); j++){
		CVertex V1= DDBB_Dec_Current->GetVertex(PairedVertL1[j]); 
		CVertex V2= DDBB_Dec_Current->GetVertex(PairedVertL2[j]);

		CVertex midVert;
		midVert.x= (V1.x + V2.x)/2;
		midVert.y= (V1.y + V2.y)/2;

		//Update the DB_DatumGraph if new vertex does not yet exist
		long midVertex= DB_DatumGraph->GetVertexNum(midVert);
		if(midVertex == -1){
			//Add the missing mid vertex
			DB_DatumGraph->AddVertex(midVert);
			midVertex= DB_DatumGraph->GetSizeVertices()-1;

			//Split the edge of datum graph that connects vertices V1-V2
			SplitEdge(DB_DatumGraph, V1, V2, midVertex);
		}

		Vertices.push_back(midVertex);
	}
}


void CCSG_Deconstruct::GetSymmetryPlanesForSP(CDB_Model *DDBB_Dec_Current, 
											  CDB_Model *DB_DatumGraph,
											  CDB_CSG *DB_CSG_Model, 
											  CFeature Feature, 
											  std::vector<long> PairedVertL1, 
											  std::vector<long> PairedVertL2, 
											  std::vector<long> &Vertices, 
											  std::vector<CEdge> &Edges, long jj)
//Lo ideal sería aplicar el algoritmo de simetrías al feature y después 
//seleccionar aquellos planos que sean perpendiculares al plano del cross-section. 
//Por ahora me limito a "construir" los planos de simetría de un step/pocket prismático
{
	CPlaneOrientation Orientation= DB_CSG_Model->GetDatumPlane(Feature.GetDatumId(0)).GetOrientation();

	long direction;
	switch (Orientation){
		case XY:{if (jj==0) direction= 0;
				 else       direction= 1;} break;
		case XZ:{if (jj==0) direction= 0;
				 else       direction= 2;} break;
		case YZ:{if (jj==0) direction= 1;
				 else       direction= 2;} break;
	}

	//Search the midpoints of the paired vertices
	//and add them to the DB_DatumGraph 
	for (long j= 0; j< (long) PairedVertL1.size(); j++){
		CVertex V11= DDBB_Dec_Current->GetVertex(PairedVertL1[j]); 
		CVertex V12= DDBB_Dec_Current->GetVertex(PairedVertL1[(j+1)%PairedVertL1.size()]);

		if(direction  == IsMainDirection(DDBB_Dec_Current, V11, V12, MainAngles)){
			CVertex midVert;
			midVert.x= (V11.x + V12.x)/2;
			midVert.y= (V11.y + V12.y)/2;

			//Update the DB_DatumGraph if new vertex does not yet exist
			long midVertex= DB_DatumGraph->GetVertexNum(midVert);
			if(midVertex == -1){
				//Add the missing mid vertex
				DB_DatumGraph->AddVertex(midVert);
				midVertex= DB_DatumGraph->GetSizeVertices()-1;

				//Split the edge of datum graph that connects vertices V1-V2
				SplitEdge(DB_DatumGraph, V11, V12, midVertex);
			}

			Vertices.push_back(midVertex);
		}
	}

	for (long j= 0; j< (long) PairedVertL2.size(); j++){
		CVertex V21= DDBB_Dec_Current->GetVertex(PairedVertL2[j]); 
		CVertex V22= DDBB_Dec_Current->GetVertex(PairedVertL2[(j+1)%PairedVertL2.size()]);

		if(direction == IsMainDirection(DDBB_Dec_Current, V21, V22, MainAngles)){
			CVertex midVert;
			midVert.x= (V21.x + V22.x)/2;
			midVert.y= (V21.y + V22.y)/2;

			//Update the DB_DatumGraph if new vertex does not yet exist
			long midVertex= DB_DatumGraph->GetVertexNum(midVert);
			if(midVertex == -1){
			//Add the missing mid vertex
				DB_DatumGraph->AddVertex(midVert);
				midVertex= DB_DatumGraph->GetSizeVertices()-1;

				//Split the edge of datum graph that connects vertices V1-V2
				SplitEdge(DB_DatumGraph, V21, V22, midVertex);
			}

			Vertices.push_back(midVertex);
		}
	}

	//Movemos el 2do vértice a la última posición para que quede organizado
	Vertices.push_back(Vertices[1]);
	Vertices.erase(Vertices.begin()+1, Vertices.begin()+2);
	//Ahora incluimos la información de las aristas
	for(long j= 0; j<(long)Vertices.size(); j++){
		CEdge NewEdge;
		long V1= Vertices[j];
		long V2= Vertices[(j+1)%Vertices.size()];
		long Edg1= DB_DatumGraph->GetEdgeByVerticesNums(V1, V2);
		if(Edg1==-1){
			NewEdge.Head= V1;
			NewEdge.Tail= V2;
			DB_DatumGraph->AddEdge(NewEdge);
		}
		else{
			NewEdge= DB_DatumGraph->GetEdge(Edg1);
		}
		Edges.push_back(NewEdge);
	}
}


bool CCSG_Deconstruct::IsSameDatumPlane(CDB_Model *DB_DatumGraph,
										CDB_CSG *DB_CSG_Model,
										long numPlane, 
										long SubGraphVertices, 
										std::vector<long> Vertices)
//Return true if the datum plane defined by "Vertices" matches "numPlane" datum
//false otherwise

//Updates ...?
{
	//Discard datums planes that belong to a different subgraph
	long SubGraphDatumPlane= DB_CSG_Model->GetDatumPlaneSubGraph(numPlane);

	if((SubGraphDatumPlane!= SubGraphVertices))
		return false;

	//Discard datums planes with a different orientation
	CPlaneOrientation OrientDatum= DB_CSG_Model->GetDatumPlaneOrientation(numPlane);

	CPlaneOrientation OrientVertices= PlaneOrientation(DB_DatumGraph,
													   DB_CSG_Model,
													   Vertices, 
													   MainAngles);
	if((OrientVertices!= OrientDatum))
		return false;

	//Count the points shared by the datum plane
	//(We only compare planes if datum plane intersects the feature)

	//Coordinates of the vertices of the datum plane "numPlane"
	std::vector<CVertex> CoordsDatumPlane;
	for(long i= 0; i<(long)DB_CSG_Model->DatumPlaneVerticesSize(numPlane); i++){
		long position= DB_CSG_Model->GetDatumPlaneVertex(numPlane, i);
		CVertex vertex= DB_DatumGraph->GetVertex(position);

		if(!IsVertexInVector(vertex, CoordsDatumPlane))
			CoordsDatumPlane.push_back(vertex);
	}

	long numMatch= 0;
	std::vector<CVertex> CoincidentVertices;
	for (long k= 0; k<(long)Vertices.size(); k++){
		CVertex vertex= DB_DatumGraph->GetVertex(Vertices[k]);
		if ((IsVertexInVector(vertex, CoordsDatumPlane)) &&
		    (!IsVertexInVector(vertex, CoincidentVertices))){
			CoincidentVertices.push_back(vertex);
			numMatch++;
		}
	}

	//Check that matching vertices are not alligned
	long x=0, y=0, z=0, none=0;
	for (long n= 0; n< (long)CoincidentVertices.size(); n++){
		long direction= IsMainDirection(DB_DatumGraph, 
										CoincidentVertices[n], 
										CoincidentVertices[(n+1)%CoincidentVertices.size()],
										MainAngles);
		if ((direction==0)&&(x==0)) x=1;
		if ((direction==1)&&(y==0)) y=1;
		if ((direction==2)&&(z==0)) z=1;
		if ((direction==-1)&&(none==0)) none=1;
	}

	//Return true if the datum plane shares at least 3 points (to define a plane)
	//and these points are not alligned
	if ((numMatch>2) && (x+y+z+none > 1)) 
		return true;
	else 
		return false;
}


bool CCSG_Deconstruct::IsDatumPlane(CDB_Model *DB_DatumGraph,
									CDB_CSG *DB_CSG_Model,
									CFeature &Feature, 
									std::vector<long> Vertices)
//Returns true if datum plane defined by "Vertices" already exists,
//Updates the feature as well

//Returns false otherwise, and update noting

{
	bool PlaneExists= false;

	long numDatumPlanes= DB_CSG_Model->DatumPlanesSize();

	for(long i= 0; i<numDatumPlanes; i++){
		if(IsSameDatumPlane(DB_DatumGraph, DB_CSG_Model,
							i, Feature.GetSubGraph(), Vertices)){

			CDatumPlane DatPlane= DB_CSG_Model->GetDatumPlane(i); 
			DatPlane.SetIDDPlane(i);

			Feature.AddDatumId(DatPlane.GetIDDPlane());

			PlaneExists= true;
			break;
		}
	}

	return PlaneExists;
}


void CCSG_Deconstruct::AddNewDatumPlane(CDB_Model *DB_DatumGraph,
										CDB_CSG *DB_CSG_Model, 
										CFeature &Feature, 
										std::vector<long> Vertices)
//Calculates the parameters of a datum plane defined by the polygon "Vertices", 
//and adds it to the Datums database

//The datum is also updated with vertices and edges of "Feature"
{

	long numVertices= (long)Vertices.size();
	if(numVertices < 2)
		return; //Datum planes must be defined, at least, by three vertices

	CDatumPlane DatPlane;

	//Include vertices in the new Datum Plane
	DatPlane.SetPlaneVertices(Vertices);

	//Include in the datum graph edges of the polyline defined by vertices, 
	//if they do not yet exist
	for(long i= 0; i<numVertices; i++){
		CEdge edge(Vertices[i], Vertices[(i+1)%numVertices]);

		long EdgePosition= DB_DatumGraph->GetEdgeNum(edge);
		if (EdgePosition == -1){
			//Check if a chain of consecutive (and collinear) edges 
			//bridges head and tail
			std::vector<long> chain;
			if (!GetChainedEdges(DB_DatumGraph, edge.Head, edge.Tail, ReferApp.CSG.CSG_ThresholdAngle, chain)){
				//No chain: add the edge
				DB_DatumGraph->AddEdge(edge);
			}
		}
	}

	//Calcule parameters of the datum plane
	DatPlane.SetIDDPlane(DB_CSG_Model->DatumPlanesSize()); //Label the datum plane with the corresponding consecutive natural number

	DatPlane.SetOrientation(PlaneOrientation(DB_DatumGraph, 
											 DB_CSG_Model, 
											 Vertices, 
											 MainAngles));
	DatPlane.SetSubGraph(Feature.GetSubGraph());

	if(ReferApp.CSG.CSG_UseMultipleOrigins){
		DatPlane.SetOrigin(Feature.GetSubGraph());   //Use this assigement to calculate distances to local origins
	}
	else{
		DatPlane.SetOrigin(0);                       //Use this assigement to calculate distances to global origin
	}
		DatPlane.SetIsSymmetry(0);

	//Add Datum plane to the database
	//-------------------------------
	DB_CSG_Model->AddDatumPlane(DatPlane);

	//Update in the feature the link to the datum plane
	//-------------------------------------------------
	Feature.AddDatumId(DatPlane.GetIDDPlane());
}


void CCSG_Deconstruct::DatumPlaneForFeature(CDB_Model *DB_DatumGraph,
											CDB_CSG *DB_CSG_Model, 
											CFeature &Feature, 
											std::vector<long> Vertices)
//Adds links to the datum planes related to the feature "Feature"

//Iff the required datum plane does not yet exists, adds it to the Datums database
{
	//Check if the datum plane already exists
	if(!IsDatumPlane(DB_DatumGraph, DB_CSG_Model, Feature, Vertices)){
		//Add the datum, if new
		AddNewDatumPlane(DB_DatumGraph, DB_CSG_Model, Feature, Vertices);
	}
}


//CVertex CCSG_Deconstruct::internalInsertionVertex(CDB_Model *DB_DatumGraph, 
//												  CDB_CSG *DB_CSG_Model, 
//												  std::vector<long> CrossSectionVertices)
////Returns the vertex of the cross section closer to the main origin
//
////Closeness is measured as Euclidean distance, measured in 2D
//{
//	CVertex NearestVertex;
//
//	long numVertices= (long)CrossSectionVertices.size();
//	CVertex V0= DB_DatumGraph->GetVertex(DB_CSG_Model->GetDatumOrigin(OuterSubGraph));
//	double distance= 0;
//
//	for(long i= 0; i<numVertices; i++){
//		CVertex Vi= DB_DatumGraph->GetVertex(CrossSectionVertices[i]);
//		double dist= MODULE_2D(V0, Vi);
//
//		if((distance==0) || (dist<distance)){
//			NearestVertex= Vi;
//			distance= dist;
//		}
//	}
//	return NearestVertex;
//}


long CCSG_Deconstruct::internalInsertionVertex(CDB_Model *DB_DatumGraph, 
											   CDB_CSG *DB_CSG_Model, 
											   std::vector<long> CrossSectionVertices)
//Returns the number of the vertex of the cross section closer to the main origin

//Closeness is measured as Euclidean distance, measured in 2D
{
	long numVertices= (long)CrossSectionVertices.size();
	CVertex V0= DB_DatumGraph->GetVertex(DB_CSG_Model->GetDatumOrigin(OuterSubGraph));
	double distance= 0;
	long numVertex= CrossSectionVertices[0];

	for(long i= 0; i<numVertices; i++){
		CVertex Vi= DB_DatumGraph->GetVertex(CrossSectionVertices[i]);
		double dist= MODULE_2D(V0, Vi);

		if((distance==0) || (dist<distance)){
			numVertex= CrossSectionVertices[i];
			distance= dist;
		}
	}
	return numVertex;
}


CVertex CCSG_Deconstruct::GetIntersectionPoint(CVertex V1, CVertex V2,
											   long dir2, CVertex V3)
//Returns the coordinates of a vertex (V4)
//calculated as the intersection of a line passing through V3 with orientation dir2
//to a segment (V1-V2)
{
	CVertex V4;

	double angle= MainAngles[dir2];  //Orientation of the second line

	if(std::fabs(V1.x-V2.x)> Accuracy){
		double m1= (V2.y-V1.y)/(V2.x-V1.x);
		double c1= V1.y- m1*V1.x;

		if((std::fabs(std::fabs(angle)-(NUMBER_PI/2))>Accuracy) && (std::fabs(std::fabs(angle)-(3*NUMBER_PI/2))>Accuracy)){
			double m2= tan(angle);
			double c2= V3.y- m2*V3.x;

			V4.x= (c2-c1)/(m1-m2);
			V4.y= (m1*c2-m2*c1)/(m1-m2);
		}
		else{
			V4.x= V3.x;
			V4.y= m1*V4.x+c1;
		}
	}
	else if((std::fabs(std::fabs(angle)-(NUMBER_PI/2))>Accuracy) && (std::fabs(std::fabs(angle)-(3*NUMBER_PI/2))>Accuracy)){
		V4.x= (V1.x+V2.x)/2;

		double m2= tan(angle);
		double c2= V3.y- m2*V3.x;

		V4.y= m2*V4.x+c2;
	}

	return V4;
}


CVertex CCSG_Deconstruct::externalInsertionVertex(CDB_Model *DB_DatumGraph, 
												  CDB_CSG *DB_CSG_Model, 
												  CFeature Feature, 
												  CVertex NearVert,
												  std::vector<long> vertices,
												  long &edgPos)
//Returns a vertex:
//	- in the perimeter of the container face 
//	- closer to the insertion point NearVert
//	- defining a line vith NearVert aligned to a main direction
{
	CVertex InsertionVertex;

	//Orientation of the container face
	CPlaneOrientation Orient= PlaneOrientation(DB_DatumGraph,
											   DB_CSG_Model,
											   vertices, 
											   MainAngles);

	double Distance= 0;
	long numVertices= (long)Feature.GetContainerFace(0).size();

	for(long i= 0; i<numVertices; i++){
		long dir1= IsMainDirection(DB_DatumGraph, 
								   DB_DatumGraph->GetVertex(vertices[i]), 
								   DB_DatumGraph->GetVertex(vertices[(i+1)%numVertices]),
								   MainAngles);
		if(dir1 == -1)
			continue;

		long dir2= -1;
		switch(Orient){
			case XY:{ if(dir1 == 0)      dir2= 1;
					  else if(dir1 == 1) dir2= 0;}
			break;

			case XZ:{ if(dir1 == 0)      dir2= 2;
					  else if(dir1 == 2) dir2= 0;}
			break;

			case YZ:{ if(dir1 == 1)      dir2= 2;
					  else if(dir1 == 2) dir2= 1;}
			break;
		}

		if(dir2 != -1){
			CVertex Point= GetIntersectionPoint(DB_DatumGraph->GetVertex(vertices[i]),
												DB_DatumGraph->GetVertex(vertices[(i+1)%numVertices]),
												dir2, NearVert);
			double dist= MODULE_2D(NearVert, Point);

			if((Distance==0) || (dist<Distance)){
				InsertionVertex= Point;
				Distance= dist;
				edgPos= i;
			}
		}
	}

	return InsertionVertex;
}


void CCSG_Deconstruct::BridgeToContainer(CDB_Model *DDBB_Dec_Current,
										 CDB_Model *DB_DatumGraph,
										 CDB_CSG *DB_CSG_Model, 
										 CFeature Feature)
//Create and add in DB_DatumGraph an auxiliary edge
//to connect the cross section of subgraph "Feature" to its container face

//Connect the vertex of the cross section of Feature that is closer to the origin

//Applies only for STEPS and POCKETS (which are depicted as isolated subgraphs)
{
	//Select internal insertion vertex
	//--------------------------------
	std::vector<long> CrossSectionVertices= Feature.GetCrossSection();
	//CVertex NearVert= internalInsertionVertex(DB_DatumGraph, DB_CSG_Model, CrossSectionVertices);
	long numNearVert= internalInsertionVertex(DB_DatumGraph, DB_CSG_Model, CrossSectionVertices);
	CVertex NearVert= DB_DatumGraph->GetVertex(numNearVert);

	//Save internal insertion vertex as local origin for the subgraph
	std::vector<long> Origins= DB_CSG_Model->GetDatumOrigins();
	Origins[Feature.GetSubGraph()]= numNearVert;
	DB_CSG_Model->SetDatumOrigins(Origins);

	//Select external insertion vertex
	//(the vertex of the container face 
	//defining the shortest orthogonal bridge to the internal insertion vertex)
	//-------------------------------------------------------------------------
	std::vector<long> vertices;
	long numVertices= (long)Feature.GetContainerFace(0).size();
	for(long i= 0; i<numVertices; i++){
		//Search in the datum graph for twin vertices from Dec_Current
		vertices.push_back(DB_DatumGraph->GetVertexNum(DDBB_Dec_Current->GetVertex(Feature.GetContainerFace(0)[i])));
	}
	long edgPos= -1;

	CVertex InsertionVertex= externalInsertionVertex(DB_DatumGraph, DB_CSG_Model,
													 Feature, NearVert, vertices, edgPos);

	//Add insertion point on the container face
	//splitting the corresponding edge
	//-----------------------------------------
	std::vector<long> chain;
	if (!GetChainedEdges(DB_DatumGraph, 
						 vertices[edgPos], vertices[(edgPos+1)%numVertices], 
						 ReferApp.CSG.CSG_ThresholdAngle, 
						 chain)){
		AfxMessageBox("Missing chain of edges in perimeter of container face");
		return;
	}
	long numInsertionVertex= getVertexInChain(DB_DatumGraph, 
											  vertices[edgPos], vertices[(edgPos+1)%numVertices], 
											  InsertionVertex,
											  chain);

	//Add the bridge edge that connects external and internal insertion points
	//------------------------------------------------------------------------
	CEdge edgeBridge;
	edgeBridge.Tail= numInsertionVertex;
	edgeBridge.Head= DB_DatumGraph->GetVertexNum(NearVert);
	DB_DatumGraph->AddEdge(edgeBridge);
}


void CCSG_Deconstruct::DefineDatumPlanesRD(CDB_Model *DDBB_Dec_Current,
										   CDB_Model *DB_DatumGraph,
										   CDB_CSG *DB_CSG_Model,
										   CFeature &Feature, 
										   std::vector<long> faces, 
										   long FeatureId)
//Add the container faces of the RIB or DIVIDER as datum planes
{
	//The other two datum planes match with the contact faces of the rib or divider,
	//(the "ContainerFaces")
	for(long i= 0; i<Feature.ContainerFacesSize(); i++){
		std::vector<long> vertices;
		long numVertices= (long)Feature.GetContainerFace(i).size();
		for(long j= 0; j<numVertices; j++){
			//Search in the datum graph for twin vertices of Feature container face from Dec_Current
			vertices.push_back(DB_DatumGraph->GetVertexNum(DDBB_Dec_Current->GetVertex(Feature.GetContainerFace(i)[j])));
		}

		DatumPlaneForFeature(DB_DatumGraph, DB_CSG_Model, Feature, vertices);

		long pos= Feature.DatumIdSize()-1;
		DB_CSG_Model->SetDatumPlaneIsSymmetry(Feature.GetDatumId(pos), 0);
	}
}


void CCSG_Deconstruct::DefineDatumPlanesRS(CDB_Model *DDBB_Dec_Current,
										   CDB_Model *DB_DatumGraph,
										   CDB_CSG *DB_CSG_Model,
										   std::vector<long> PairedVertL1, 
										   std::vector<long> PairedVertL2, 
										   CFeature &Feature, 
										   std::vector<long> faces, 
										   bool isBlind)
//Add contact faces of Rails and Slots as non-symmetrical datum planes

//Add the symmetry datum plane of the Rail/Slot,
//perpendicular to the CrossSection plane
{
	CDB_Model DDBB_F2D= Feature.GetFeature2D();  //F2D Read only

	//We first look for "contact" faces of the current deconstruction
	//that are neighbor to the feature
	//---------------------------------------------------------------
	std::vector<long> ContactFaces;

	for(long i= 0; i<(long)DDBB_F2D.GetSizeEdges(); i++){
		CEdge iEdge= DDBB_F2D.GetEdge(i);

		CVertex iVertexHead= DDBB_F2D.GetVertex(iEdge.Head);
		long iVertH= DDBB_Dec_Current->GetVertexNum(iVertexHead);
		CVertex iVertexTail= DDBB_F2D.GetVertex(iEdge.Tail);
		long iVertT= DDBB_Dec_Current->GetVertexNum(iVertexTail);

		long iedge= DDBB_Dec_Current->GetEdgeByVerticesNums(iVertH, iVertT);

		std::vector<long>SharedFaces= DDBB_Dec_Current->GetFacesSharingEdge(iedge);
		for(long j= 0; j<(long) SharedFaces.size(); j++){
			if((!IsValueInVector(SharedFaces[j], faces)) &&
			   (!IsValueInVector(SharedFaces[j], ContactFaces))){
				ContactFaces.push_back(SharedFaces[j]);
			}
		}
	}

	//Alternative search of "contact" faces for blind slots
	//if(isBlind){
	//	for(long i= 0; i<(long)DDBB_F2D.GetSizeEdges(); i++){
	//		//Recuperamos la arista de la BBDD a partir de la información almacenada
	//		//de manera local en el Feature
	//		CEdge iEdge= DDBB_F2D.GetEdge(i);
	//		CVertex iVertexHead= DDBB_F2D.GetVertex(iEdge.Head);
	//		long iVertH= DDBB_Dec_Current->GetVertexNum(iVertexHead);
	//		CVertex iVertexTail= DDBB_F2D.GetVertex(iEdge.Tail);
	//		long iVertT= DDBB_Dec_Current->GetVertexNum(iVertexTail);
	//		long iedge= DDBB_Dec_Current->GetEdgeByVerticesNums(iVertH, iVertT);

	//		std::vector<long>FSharingEdge= DDBB->GetFacesSharingEdge(iedge);
	//		for(long j= 0; j<(long) FSharingEdge.size(); j++){
	//			long jface= FSharingEdge[j];
	//			if((!IsValueInVector(jface, faces))&&(!IsValueInVector(jface, ContactFaces))){
	//				ContactFaces.push_back(jface);
	//			}
	//		}
	//	}
	//}

	//Save these contact faces as non-symmetrical datum planes
	//--------------------------------------------------------
	for(long k= 0; k<(long)ContactFaces.size(); k++){
		//First we get the coordinates of vertices of current contact face
		//retrieved from the DDBB_Dec_Current graph
		long kface= ContactFaces[k];
		std::vector<long> vertices= DDBB_Dec_Current->GetAllVerticesInFace(DDBB_Dec_Current->GetFace(kface));

		//Obtain the edges of the face according to the information from DB_DatumGraph, 
		//if there is any datum vertex that was not included, 
		//collinear edges are recovered and the datum vertex is added to the datum plane data
		long numVertices= (long)vertices.size();
		for(long j= 0; j<numVertices; j++){
			//Select in DatumGraph the twin of DDBB_Dec_Current face vertices
			long v1= DB_DatumGraph->GetVertexNum(DDBB_Dec_Current->GetVertex(vertices[j]));
			long v2= DB_DatumGraph->GetVertexNum(DDBB_Dec_Current->GetVertex(vertices[(j+1)%numVertices]));

			//Check if an edge (or a chain) connects both vertices, add it if not
			std::vector<long> chain;
			if (!GetChainedEdges(DB_DatumGraph, 
								 v1, v2, 
								 ReferApp.CSG.CSG_ThresholdAngle, 
								 chain)){
				CEdge Edge(v1, v2);
				DB_DatumGraph->AddEdge(Edge);
			}
		}

		//Add the contact plane as non-symmetry datum plane
		DatumPlaneForFeature(DB_DatumGraph, 
							 DB_CSG_Model, 
							 Feature, 
							 vertices);

		long pos= Feature.DatumIdSize()-1;
		DB_CSG_Model->SetDatumPlaneIsSymmetry(Feature.GetDatumId(pos), 0);
	}

	//If the rail/slot is through, there will be repeated datum planes, 
	//we will choose the ones closest to the origin
	//-----------------------------------------------------------------
	if(!isBlind){
		bool seguir= true;
		while(seguir){
			bool repeat= false;
			for (long i=0; i<Feature.DatumIdSize(); i++){
				CPlaneOrientation Orient_first= DB_CSG_Model->GetDatumPlane(Feature.GetDatumId(i)).GetOrientation();

				for(long j= (i+1); j<Feature.DatumIdSize(); j++){
					CPlaneOrientation Orient_second= DB_CSG_Model->GetDatumPlane(Feature.GetDatumId(j)).GetOrientation();

					if(Orient_first == Orient_second){
						repeat= true;

						if(DB_CSG_Model->GetDatumPlane(Feature.GetDatumId(i)).GetNormalDistance() < 
						   DB_CSG_Model->GetDatumPlane(Feature.GetDatumId(j)).GetNormalDistance()){
							Feature.RemoveDatumId(j);
						}
						else{
							Feature.RemoveDatumId(i);
						}
					}
				}
				if(repeat) break;
			}
			if(!repeat) seguir= false;
		}
	}

	//Add the symmetry datum plane of the feature,
	//perpendicular to the CrossSection plane
	//--------------------------------------------
	std::vector<long> vertices;
	GetSymmetryPlaneForRS(DDBB_Dec_Current, DB_DatumGraph, Feature, PairedVertL1, PairedVertL2, vertices);
	DatumPlaneForFeature(DB_DatumGraph, DB_CSG_Model, Feature, vertices);

	long pos= Feature.DatumIdSize()-1;
	DB_CSG_Model->SetDatumPlaneIsSymmetry(Feature.GetDatumId(pos), 1);
}


void CCSG_Deconstruct::DefineDatumPlanesSP(CDB_Model *DDBB_Dec_Current,
										   CDB_Model *DB_DatumGraph,
										   CDB_CSG *DB_CSG_Model,
										   std::vector<long> PairedVertL1, 
										   std::vector<long> PairedVertL2, 
										   CFeature &Feature, 
										   std::vector<long> faces, 
										   long FeatureId)
//We choose the other datum planes so that they define the origin of the SG

//The heuristic is that the other two datum planes 
//are perpendicular and are associated to the origin point
{
	long Origin= DB_CSG_Model->GetDatumOrigin(Feature.GetSubGraph());
	std::vector<long>PotentialDatumPlanes= DDBB_Dec_Current->GetFacesSharingVertex(Origin);
	CPlaneOrientation Orient= DB_CSG_Model->GetDatumPlane(Feature.GetDatumId(0)).GetOrientation();

	for(long i= 0; i<(long)PotentialDatumPlanes.size(); i++){
		std::vector<long> Vertices= DDBB_Dec_Current->GetAllVerticesInFace(PotentialDatumPlanes[i]);
		//Search in the datum graph for twin vertices of the current deconstruction
		for(long j= 0; j<(long)Vertices.size(); j++){
			Vertices[j]= DB_DatumGraph->GetVertexNum(DDBB_Dec_Current->GetVertex(Vertices[j]));
		}

		CPlaneOrientation Orientation= PlaneOrientation(DB_DatumGraph, 
														DB_CSG_Model, 
														Vertices, 
														MainAngles);
		if(Orient != Orientation){
			DatumPlaneForFeature(DB_DatumGraph, 
								 DB_CSG_Model, 
								 Feature, 
								 Vertices);
		}
	}

	//Add the symmetry datum plane of the feature, 
	//perpendicular to the CrossSection plane
	//std::vector<long> Vertices;
	//GetSymmetryPlaneForSP(DDBB_Dec_Current, 
	//					  Feature, 
	//					  PairedVertL1,  PairedVertL2, 
	//					  Vertices, jj);
	//DatumPlaneForFeature(DDBB_Dec_Current, DB_DatumGraph, DB_CSG_Model, Feature, Vertices);

	//long pos= Feature.DatumIdSize()-1;
	//DB_CSG_Model->SetDatumPlaneIsSymmetry(Feature.GetDatumId(pos), 1);
}


void CCSG_Deconstruct::DefineDatumPlanesCore(CDB_Model *DDBB_Dec_Current,
											 CDB_Model *DB_DatumGraph,
											 CDB_CSG *DB_CSG_Model,
											 std::vector<long> PairedVertL1, 
											 std::vector<long> PairedVertL2, 
											 CFeature &Feature, 
											 std::vector<long> faces, 
											 long FeatureId)
{
	//The other two datum planes are perpendicular and are associated to the origin point
	long Origin= DB_CSG_Model->GetDatumOrigin(Feature.GetSubGraph());
	std::vector<long>PotentialDatumPlanes= DDBB_Dec_Current->GetFacesSharingVertex(Origin);
	CPlaneOrientation Orient= DB_CSG_Model->GetDatumPlane(Feature.GetDatumId(0)).GetOrientation();

	for(long i= 0;  i<(long)PotentialDatumPlanes.size(); i++){
		std::vector<long> vertices= DDBB_Dec_Current->GetAllVerticesInFace(PotentialDatumPlanes[i]);

		std::vector<CVertex> coorVertices;
		for(long j= 0; j<(long)vertices.size();j++){
			coorVertices.push_back(DDBB_Dec_Current->GetVertex(vertices[j]));
		}

		CPlaneOrientation Orientation= PlaneOrientation(DB_DatumGraph, 
														DB_CSG_Model, 
														//coorVertices, 
														vertices, 
														MainAngles);
		if(Orient != Orientation){
			//Core has NO container faces !!!
		}
	}
}


void CCSG_Deconstruct::DefineDatumPlanes(CDB_Model *DDBB_Dec_Current,
										 CDB_Model *DB_DatumGraph,
										 CDB_CSG *DB_CSG_Model,
										 std::vector<long> PairedVertL1, 
										 std::vector<long> PairedVertL2, 
										 CFeature &Feature, 
										 std::vector<long> faces, 
										 long FeatureId)
//Adds links to the datum planes related to the feature "Feature"

//Iff the required datum plane does not yet exists, adds it to the Datums database
{
	FEATURE_TYPE Feat= Feature.GetType();

	//switch (Feature.FeatureClass){
	switch (Feat){
		//Ribs have 3 datum planes: the feature's symmetry plane and two container faces
		case RIB:{
			//First datum plane is the symmetry plane, whitch matches the profile plane
			std::vector<long> Vertices= Feature.GetCrossSection();
			DatumPlaneForFeature(DB_DatumGraph, DB_CSG_Model, Feature, Vertices);

			//Container faces are datum planes as well
			DefineDatumPlanesRD(DDBB_Dec_Current,
								DB_DatumGraph,
								DB_CSG_Model,
								Feature, 
								faces, 
								FeatureId);
		}
		break;

		case DIVIDER:{
			//First datum plane is the symmetry plane, whitch matches the profile plane
			std::vector<long> Vertices= Feature.GetCrossSection();
			DatumPlaneForFeature(DB_DatumGraph, DB_CSG_Model, Feature, Vertices);

			//Container faces are datum planes as well
			DefineDatumPlanesRD(DDBB_Dec_Current,
								DB_DatumGraph,
								DB_CSG_Model,
								Feature, 
								faces, 
								FeatureId);
		}
		break;

		case (RAIL):{
			bool isBlind= (ReferApp.Fe.RS.BlindRails[FeatureId-1] == 1);
			DefineDatumPlanesRS(DDBB_Dec_Current,
								DB_DatumGraph,
								DB_CSG_Model,
								PairedVertL1, 
								PairedVertL2, 
								Feature, 
								faces, 
								isBlind);
		}
		break;

		case SLOT:{
			bool isBlind= (ReferApp.Fe.RS.BlindSlots[FeatureId-1] == 1);
			DefineDatumPlanesRS(DDBB_Dec_Current,
								DB_DatumGraph,
								DB_CSG_Model,
								PairedVertL1, 
								PairedVertL2, 
								Feature, 
								faces, 
								isBlind);
		}
		break;

		case (STEP):{
			//Update datum graph by connecting the STEP subgraph to the subgraph of its container face
			//(Also recalcule origin of the subgraph to align it to the container face)
			BridgeToContainer(DDBB_Dec_Current,
							  DB_DatumGraph,
							  DB_CSG_Model, 
							  Feature);

			//Define the Cross section as a datum plane
			//(Because the Cross section and the Container face are coplanar)
			//(Should we include both cross section plus container face??)
			std::vector<long> Vertices= Feature.GetCrossSection();
			DatumPlaneForFeature(DB_DatumGraph, DB_CSG_Model, Feature, Vertices);

			//Find the remainder datum planes
			DefineDatumPlanesSP(DDBB_Dec_Current,
								DB_DatumGraph,
								DB_CSG_Model,
								PairedVertL1, 
								PairedVertL2, 
								Feature, 
								faces, 
								FeatureId);
		}
		break;

		case (POCKET):{
			//Update datum graph by connecting the POCKET subgraph to the subgraph of its container face
			//(Also recalcule origin of the subgraph to align it to the container face)
			BridgeToContainer(DDBB_Dec_Current,
							  DB_DatumGraph,
							  DB_CSG_Model, 
							  Feature);

			//Define the Cross section as a datum plane
			//(Because the Cross section and the Container face are coplanar)
			//(Should we include both cross section plus container face??)
			std::vector<long> Vertices= Feature.GetCrossSection();
			DatumPlaneForFeature(DB_DatumGraph, DB_CSG_Model, Feature, Vertices);

			//Find the remainder datum planes
			DefineDatumPlanesSP(DDBB_Dec_Current,  //Same function than step
								DB_DatumGraph,
								DB_CSG_Model,
								PairedVertL1, 
								PairedVertL2, 
								Feature, 
								faces, 
								FeatureId);
		}
		break;

		case NODE:{
			bool doNothing= true;
		}
		break;

		case CORE:{
			//First: The Cross section is located in a symmetry plane
			std::vector<long> Vertices= Feature.GetCrossSection();
			DatumPlaneForFeature(DB_DatumGraph, 
								 DB_CSG_Model, 
								 Feature, 
								 Vertices);

			DB_CSG_Model->SetDatumPlaneIsSymmetry(Feature.GetDatumId(0), 1);

			//Find the remainder datum planes
			DefineDatumPlanesCore(DDBB_Dec_Current,
								  DB_DatumGraph,
								  DB_CSG_Model,
								  PairedVertL1, 
								  PairedVertL2, 
								  Feature, 
								  faces, 
								  FeatureId);
		}
	}//end switch
}
//--End DefineDatumPlanes----------------------------------------------


void CCSG_Deconstruct::CompleteFeatureData(CDB_Model *DDBB_Dec_Current,
										   CDB_Model *DB_DatumGraph,
										   CDB_CSG *DB_CSG_Model,
										   CFeature &NewFeat, 
										   long FeatureId,
										   std::vector<long> &PairedVertL1,
										   std::vector<long> &PairedVertL2)
//Calculate building information for the current feature
	//The feature information (location and profile) is saved
	//for subsequent construction

//Store feature information:
	//Once a 2D feature element and its parent have been selected, 
	//and before proceeding to the next sub-stage of drawing simplification, 
	//the geometric information of the feature element 
	//is stored in the 2D feature.

	//At the same time, the Boolean operation 
	//that relates the pair of parent-child features is stored.
	//Until now and to avoid any loss of information, 
	//all the geometric information regarding the feature in question 
	//(vertices and edges) 
	//is stored as well in the 2D features tree.
{
	FEATURE_TYPE type= NewFeat.GetType();

	//Select faces of the current feature
	//-----------------------------------
	std::vector<long> faces;
	long base1, base2, face;
	FacesFeature(DDBB_Dec_Current, DB_CSG_Model, NewFeat, type, FeatureId, 
				 base1, base2, face, faces);
	if (faces.size() <= 0){
		return;
	}

	//Calculate direction and module of the extrusion
	//(Vertices are paired as well)
	//-----------------------------------------------
	long direction;
	double ExtrudedLength;
	if(type == NODE){
		bool doNothing= true;
	}

	else if((type == RAIL) || (type == SLOT)){
		//Specific calculation for rails and slots
		//(Requires adjustement before pairing vertices)
		RailSlotExtrusion(DDBB_Dec_Current, 
						  faces,                        //Updates faces
						  PairedVertL1, PairedVertL2,   //Updates PairedVertL1 and PairedVertL2
						  direction, ExtrudedLength);
	}

	else{
		//Generic calculation
		PairVertices(DDBB_Dec_Current, face, base1, base2,
					 PairedVertL1, PairedVertL2);
		ExtrusionDirectionAndLength(DDBB_Dec_Current, PairedVertL1, PairedVertL2, 
									direction, ExtrudedLength);
	}

	//Update NewFeat: Save average direction and length
	NewFeat.SetExtrusionDirection(direction);
	NewFeat.SetLengthExtrusion(ExtrudedLength);

	//Save 2D drawing of the feature as feature data
	//(after completing the drawing, if required)
	//----------------------------------------------
	AddDrawingToFeature(DDBB_Dec_Current, NewFeat,
						faces, PairedVertL1, PairedVertL2);

	//Add subgraph flag
	//-----------------
	PutSubgraphData(DDBB_Dec_Current, NewFeat);

	//Add cross-section
	//-----------------
	SetCrossSection(DDBB_Dec_Current,
					DB_DatumGraph, 
					PairedVertL1, PairedVertL2,
					NewFeat, FeatureId);

	//Calcule the profile plane for the feature
	//-----------------------------------------
	ProfilePlaneOrientation(DB_DatumGraph, DB_CSG_Model, NewFeat);

	//Define the datum planes of the feature
	//--------------------------------------
	DefineDatumPlanes(DDBB_Dec_Current,
					  DB_DatumGraph, 
					  DB_CSG_Model,
					  PairedVertL1, PairedVertL2, 
					  NewFeat, faces, FeatureId);
}

//--End CompleteFeatureData-------------------------------------------




//--Begin RemoveFeature------------------------------------

void CCSG_Deconstruct::RepairInterleavedEdgesRSRD(CDB_Model *DDBB_Dec_Current, 
												  std::vector<long> PairedVertL1, 
												  std::vector<long> PairedVertL2, 
												  CFeature &Feature)
//After removing rails/slots or ribs/dividers,
//which always interrupt at least one edge 
//of the containing face of the parent element,
//the adjacent edges of the containing face 
//that are recognized as collinear 
//must be merged as single edges
{
	//Search for missing edges between paired vertices of the feature
	for(long i= 0; i<(long)PairedVertL1.size(); i++){

		std::vector<long> chain;
		if (!GetChainedEdges(DDBB_Dec_Current, 
							 PairedVertL1[i], PairedVertL2[i], 
							 ReferApp.CSG.CSG_ThresholdAngle, 
							 chain)){

		//long EdgePos= DDBB_Dec_Current->GetEdgeByVerticesNums(PairedVertL1[i], PairedVertL2[i]);
		//if(EdgePos == -1){

			//Check whether the edge is the missing link of a broken external chain
			long Ed1, Ed2;
			if(IsBrokenChain(DDBB_Dec_Current, 
							 PairedVertL1[i], PairedVertL2[i], 
							 ReferApp.CSG.CSG_ThresholdAngle, 
							 Ed1, Ed2)){
				//Simplify the chain as a single edge
				//connecting its most extreme and opposite vertices
				CEdge edge1= DDBB_Dec_Current->GetEdge(Ed1);
				CEdge edge2= DDBB_Dec_Current->GetEdge(Ed2);

				long V1, V2;
				if(edge1.Head!= PairedVertL1[i]) V1= edge1.Head;
				if(edge1.Tail!= PairedVertL1[i]) V1= edge1.Tail;
				if(edge2.Head!= PairedVertL2[i]) V2= edge2.Head;
				if(edge2.Tail!= PairedVertL2[i]) V2= edge2.Tail;

				//Remove major first, to avoid renumbering issues
				if(Ed1 > Ed2){
					DDBB_Dec_Current->RemoveEdge(Ed1, false);
					DDBB_Dec_Current->RemoveEdge(Ed2, false);
				}
				else{
					DDBB_Dec_Current->RemoveEdge(Ed2, false);
					DDBB_Dec_Current->RemoveEdge(Ed1, false);
				}

				CEdge edge12(V1, V2);
				DDBB_Dec_Current->AddEdge(edge12);
			}
			else{
				//If no broken chain exist, then the paired vertices 
				//are the end points of the new edge
				CEdge edge12(PairedVertL1[i], PairedVertL2[i]);
				DDBB_Dec_Current->AddEdge(edge12);
			}
		}
	}
}


void CCSG_Deconstruct::GetVerticesPositions(CDB_Model *DDBB_Dec_Current, 
											CFeature Feature, 
											std::vector<long> &FeatureVertices)
//Since the list of vertices is updated after removing features previously detected,
//the number (position of the vertices in the list of vertices)
//of the remaining vertices must be recalculated.
{
	std::vector<CVertex> VerticesFeature;
	Feature.GetFeature2D().CopyVerticesToVector(VerticesFeature);  //F2D Read only

	for(long i= 0; i<(long)VerticesFeature.size(); i++){
		long pos= DDBB_Dec_Current->GetVertexNum(VerticesFeature[i]);
		if(!IsValueInVector(pos, FeatureVertices)){
			FeatureVertices.push_back(pos);
		}
	}
}


void CCSG_Deconstruct::GettingFacesOfFeature(FEATURE_TYPE FeatureClass, 
											 long FeatureId, 
											 std::vector<long> &faces)
{
	switch (FeatureClass){
		case SLOT:{
			for(long b=0; b<(long)ReferApp.Fe.RS.Slots.size(); b++){
					if(ReferApp.Fe.RS.Slots[b]== FeatureId) faces.push_back(b);
			}
		}//case Slot
		break;
		case RAIL:{
			for(long b=0; b<(long)ReferApp.Fe.RS.Rails.size(); b++){
				if(ReferApp.Fe.RS.Rails[b]== FeatureId) faces.push_back(b);
			}
		}//case Rail
		break;
		case POCKET:{
			for(long b=0; b<(long)ReferApp.Fe.SP.Pockets.size(); b++){
				if(ReferApp.Fe.SP.Pockets[b]== FeatureId) faces.push_back(b);
			}
		}//case Pocket
		break;
		case STEP:{
			for(long b=0; b<(long)ReferApp.Fe.SP.Steps.size(); b++){
				if(ReferApp.Fe.SP.Steps[b]== FeatureId) faces.push_back(b);
			}
		}//case Step
		break;
		case RIB:{
			for(long b=0; b<(long)ReferApp.Fe.RD.Ribs.size(); b++){
				if(ReferApp.Fe.RD.Ribs[b]== FeatureId) faces.push_back(b);
			}
		}//case Rib
		break;
		case DIVIDER:{
			for(long b=0; b<(long)ReferApp.Fe.RD.Dividers.size(); b++){
				if(ReferApp.Fe.RD.Dividers[b]== FeatureId) faces.push_back(b);
			}
		}//case Divider
		break;
	}//end switch
}


void CCSG_Deconstruct::RemoveEdgesFromDec(CDB_Model *DDBB_Dec_Current, 
										  std::vector<long> Vertices)
//Remove all edges connected to the list of Vertices
{
	for(long i= 0; i<(long)Vertices.size(); i++){
		std::vector<long> EdgesInVert= DDBB_Dec_Current->GetAllEdgesInVertex(Vertices[i]);
		//EdgesInVert= SortMajorFirst(EdgesInVert);

		//Remove edges backwards (from major to minor, to avoid renumbering issues)
		long num= (long)EdgesInVert.size()-1;
		for(long j= 0; j<(long)EdgesInVert.size(); j++){
			DDBB_Dec_Current->RemoveEdge(EdgesInVert[num-j], false);  //Do not remove vertices now, to avoid orphan edges
		}
	}
}


void CCSG_Deconstruct::RemoveVerticesFromDec(CDB_Model *DDBB_Dec_Current, 
											 std::vector<long> Vertices)
//Remove vertices in the list
{
	//Vertices are removed backwards 
	//to keep the location of the non-removed vertices unaltered
	Vertices= SortMajorFirst(Vertices);

	for(long k= 0; k<(long)Vertices.size(); k++){
		DDBB_Dec_Current->RemoveVertex(Vertices[k], true);  //Flag true forces recalculation of edges
	}
}


void CCSG_Deconstruct::RemoveFeature(CDB_Model *DDBB_Dec_Current, 
									 std::vector<long> PairedVertL1, 
									 std::vector<long> PairedVertL2,
									 long FeatureId, 
									 CFeature &Feature,
									 bool FeatureFatherP)
//Remove the selected 2D feature element from the drawing:
//    Simplifying the drawing consists of 
//    removing the edges and vertices 
//    that belong to the selected 2D feature.

//Simplification is easy if the 2D feature is a subgraph
//disconnected from the rest of the drawing, 
//since then it is enough to remove all its edges and vertices.

//Otherwise, simplification of the outlined drawing 
//may require "repair", by modifying neighbor edges or faces.
{
	//Select vertices and edges to remove
	//And repair edges linked to the feature to remove
	//------------------------------------------------
	std::vector<long> FeatureVertices;
	switch (Feature.GetType()){
			case RIB:{
				RepairInterleavedEdgesRSRD(DDBB_Dec_Current, 
										   PairedVertL1, PairedVertL2, 
										   Feature);

				GetVerticesPositions(DDBB_Dec_Current, Feature, FeatureVertices);
			}
			break;

			case DIVIDER:{
				RepairInterleavedEdgesRSRD(DDBB_Dec_Current, 
										   PairedVertL1, PairedVertL2,
										   Feature);

				GetVerticesPositions(DDBB_Dec_Current, Feature, FeatureVertices);
			}
			break;

			case RAIL:{
				RepairInterleavedEdgesRSRD(DDBB_Dec_Current, 
										   PairedVertL1, PairedVertL2,
										   Feature);

				GetVerticesPositions(DDBB_Dec_Current, Feature, FeatureVertices);
			}
			break;

			case SLOT:{
				RepairInterleavedEdgesRSRD(DDBB_Dec_Current, 
										   PairedVertL1, PairedVertL2,
										   Feature);

				GetVerticesPositions(DDBB_Dec_Current, Feature, FeatureVertices);
			}
			break;

			case STEP:{
				GetVerticesPositions(DDBB_Dec_Current, Feature, FeatureVertices);
			}
			break;

			case POCKET:{
				GetVerticesPositions(DDBB_Dec_Current, Feature, FeatureVertices);
			}
			break;

			case NODE:{
				bool doNothing= true;
			}
			break;

			case CORE:{
				//for(long b=0; b<(long)DDBB_Dec_Current->GetSizeVertices(); b++){
				//	FeatureVertices.push_back(b);
				//}
				bool doNothing= true;
			}
			break;
		}

	//Remove the edges and vertices of the current feature
	//----------------------------------------------------
	if(FeatureVertices.size() > 0){
		//Remove edges of the feature from DDBB deconstruction
		RemoveEdgesFromDec(DDBB_Dec_Current, FeatureVertices);

		//Remove vertices of the feature from DDBB deconstruction
		RemoveVerticesFromDec(DDBB_Dec_Current, FeatureVertices);

		//Removing faces from DDBB deconstruction
		DDBB_Dec_Current->ClearFaces();
	}
}
//--End RemoveFeature------------------------------------


//--Begin ParametricProfiles----------------------------------------------


void CCSG_Deconstruct::GetOrthoDistancesToOrigins(CDB_Model *DB_DatumGraph,
												  CDB_CSG *DB_CSG_Model,
												  std::vector<std::vector<long>> predecessors)
//Completes the calculations of distances from main origin to all local origins
{
	//Loop over the local origins
	std::vector<long> Origins= DB_CSG_Model->GetDatumOrigins();
	std::vector<COrthoDistance> OrthoDistanceOrigins;

	for(long i= 0; i<(long)Origins.size(); i++){
		COrthoDistance D= CalculeOrthoDistance(DB_DatumGraph,
											   Origins[0],
											   predecessors[0],
											   Origins[i],
											   MainAngles,
											   ReductionCoefficients);
		OrthoDistanceOrigins.push_back(D);
	}

	DB_CSG_Model->SetOrthoDistancesOrigins(OrthoDistanceOrigins);
}


void CCSG_Deconstruct::GetOrthoDistancesToDatumPlanes(CDB_Model *DB_DatumGraph,
													  CDB_CSG *DB_CSG_Model,
													  std::vector<std::vector<long>> predecessors)
//Completes the calculations of distances from origins to all the datum planes
{
	//Loop over the datum planes
	long numPlanes= (long)DB_CSG_Model->DatumPlanesSize();
	std::vector<long> Origins= DB_CSG_Model->GetDatumOrigins();

	for(long i= 0; i<numPlanes; i++){
		CDatumPlane DatPlane= DB_CSG_Model->GetDatumPlane(i);
			////long Origin= DB_CSG_Model->GetDatumOrigin(DatPlane.GetOrigin());
			DatPlane.SetNormalDistance(CalculeNormalDistance(DB_DatumGraph,
															 DatPlane.GetOrientation(),
															 Origins[DatPlane.GetOrigin()],        //Distance to local origin
															 predecessors[DatPlane.GetOrigin()],
															 DatPlane.GetPlaneVertices(),
															 MainAngles,
															 ReductionCoefficients));
		DB_CSG_Model->SetDatumPlane(DatPlane, i);
	}
}


void CCSG_Deconstruct::GetOrthoDistanceToFeature(CDB_Model *DB_DatumGraph,
												 CDB_CSG *DB_CSG_Model, 
												 std::vector<std::vector<long>> predecessors,
												 CFeature Feature,
												 std::vector<COrthoDistance> &Distances)
//Returns the orthogonal distances from origin 
//to each vertex of the cross section of the "Feature"

//Distances are "orthogonal" as they are measured adding the length
//of succesive path segments aligned to main axes,
//and belonging to the DB_DatumGraph

//DatumGraph was used in advance to find the "predecessors" 
//that connect each vertex to the origin, and ease finding the paths
{
	std::vector<long> VerticesCS= Feature.GetCrossSection();

	long Origin= DB_CSG_Model->GetDatumOrigin(OuterSubGraph);

	//Calculate orthogonal distances for all the vertices of the cross-section
	for(long i=0; i<(long)VerticesCS.size(); i++){
		std::vector<long> path;
		get_Dijkstra_path(predecessors[0], Origin, VerticesCS[i], path);

		COrthoDistance Distance;
		long numPath= (long)path.size();
		for(long j=0; j<numPath-1; j++){
			UpdateDistance(DB_DatumGraph, MainAngles, ReductionCoefficients,
						   DB_DatumGraph->GetVertex(path[j]), 
						   DB_DatumGraph->GetVertex(path[j+1]),
						   Distance);
		}

		//Save distance of the ith vertex
		Distances.push_back(Distance);
	}
}


void CCSG_Deconstruct::TranslateToOrigin(CDB_CSG *DB_CSG_Model,
										 CFeature &Feature,
										 std::vector<COrthoDistance> &Distances)
//Calculates the insertion point of the profile,
//as the point where the three datum planes of the feature intersect

//Then recalculates distances to make the insertion point be the origin
{
	//The insertion point of the profile is determined, 
	//by the vertex intersection of the three datum planes of the feature
	CVertex Insertion;

	for(long i= 0; i< (long) Feature.DatumIdSize(); i++){
		CDatumPlane datPlaneI= DB_CSG_Model->GetDatumPlane(Feature.GetDatumId(i));
		switch (datPlaneI.GetOrientation()){
			case XY:{Insertion.z = datPlaneI.GetNormalDistance();} break;
			case XZ:{Insertion.y = datPlaneI.GetNormalDistance();} break;
			case YZ:{Insertion.x = datPlaneI.GetNormalDistance();} break;
		}
	}

	//Subtract distances from each vertex of the Profile
	//with respect to the insertion point
	long numVertices= (long)Distances.size();

	for(long i= 0; i<numVertices; i++){
		Distances[i].x -= Insertion.x;
		if (std::fabs(Distances[i].x)< Accuracy)
			Distances[i].x= 0;

		Distances[i].y -= Insertion.y;
		if (std::fabs(Distances[i].y)< Accuracy)
			Distances[i].y= 0;

		Distances[i].z -= Insertion.z;
		if (std::fabs(Distances[i].z)< Accuracy)
			Distances[i].z= 0;
	}
}


void CCSG_Deconstruct::TransformToSketchPlane(CDB_CSG *DB_CSG_Model,
											  CFeature &Feature,
											  std::vector<COrthoDistance> Distances)
//Transforms 3D orthodistances into distances in the 2D sketch plane
{
	long numVertices= (long)Distances.size();

	//Transform the 3D distances into 2D vertices of the profile plane, 
	//(assuming the known orientation of the profile plane)
	switch (Feature.GetProfileOrientation()){
		case XY:{
			for(long i= 0; i<numVertices; i++){
				CVertex ProfileVertex(Distances[i].x, Distances[i].y, 0);
				Feature.AddParametricProfileVertex(ProfileVertex);
			}
		}
		break;

		case XZ:{
			for(long i= 0; i<numVertices; i++){
				CVertex ProfileVertex(Distances[i].x, Distances[i].z, 0);
				Feature.AddParametricProfileVertex(ProfileVertex);
			}
		}
		break;

		case YZ:{
			for(long i= 0; i<numVertices; i++){
				CVertex ProfileVertex(Distances[i].y, Distances[i].z, 0);
				Feature.AddParametricProfileVertex(ProfileVertex);
			}
		}
		break;
	}
}


void CCSG_Deconstruct::GetParametricProfiles(CDB_Model *DB_DatumGraph,
											 CDB_CSG *DB_CSG_Model)
//Calculates the parametric profiles of all the features in the Tree of DB_CSG_Model
{
	//Update cues required to calculate orthogonal distances
	CCueManager C(false, //Set_DetectValences
				  false, //Set_DetectParallelEdges
				  false, //Set_DetectVanishingPoints
				  true, //Set_DetectGraphs
				  true, //Set_DetectPerimeter
				  false, //Set_DetectLabels
				  true, //Set_DetectMainAxes
				  false, //Set_DetectFaces
				  true, //Set_DetectTypology
				  false, //Set_DetectHiddenEdges
				  false, //Set_DetectSymmetry
				  false);//Set_DetectLevels
	C.FindCues(DB_DatumGraph);

	//Calculate predecessors from Origin
	//(Required to calculate orthodistances)
	std::vector<std::vector<long>> predecessors;
	std::vector<long> Origins= DB_CSG_Model->GetDatumOrigins();
	for(long i=0; i<(long)Origins.size(); i++){
		std::vector<long> pred;
		get_Dijkstra_predecessors(DB_DatumGraph, Origins[i], pred);
		predecessors.push_back(pred);
	}

	//Calculate orthodistances to local origins
	GetOrthoDistancesToOrigins(DB_DatumGraph, DB_CSG_Model, predecessors);

	//Calculate orthodistances to datum planes
	GetOrthoDistancesToDatumPlanes(DB_DatumGraph, DB_CSG_Model, predecessors);

	//Loop over all the features
	long numFeatures= DB_CSG_Model->TreeFeaturesSize();
	for(long i= 0; i<numFeatures; i++){
		CFeature FeatureI= DB_CSG_Model->GetFeature(i);
			//Calculates orthogonal distances from origin 
			//to vertices of the Feature cross section
			std::vector<COrthoDistance> Distances;
			GetOrthoDistanceToFeature(DB_DatumGraph, DB_CSG_Model, predecessors, FeatureI, Distances);

			//Translate parametric profile
			TranslateToOrigin(DB_CSG_Model, FeatureI, Distances);

			//Unskew and save parametric profile
			TransformToSketchPlane(DB_CSG_Model, FeatureI, Distances);
		DB_CSG_Model->SetFeature(i, FeatureI);
	}
}
//--End ParametricProfiles----------------------------------------------




void CCSG_Deconstruct::DeconstructCSG_2D(CReferDoc* pDoc, bool &FeatureFather)
//Deconstructs the 3D model implicit in the 2D drawing
//by RECURSIVELY removing its features until only the CORE feature remains

//The process begins by selecting the lowest level 2D feature of the drawing.
//From there, the algorithm "descends" through the selected branch, 
//giving priority to the parent-child relationships 
//that occur between the detected 2D features, 
//and their container-contained related features.

//Thus the branches of the tree are recursively determined 
//until the core is reached

//Each iteration requires re-executing the recognition of cues and features,
//since the simplification of the line drawing 
//typically arises hiden cues of features and/or removes some of them.

//At the moment, the algorithm can only process 2D features
//that interact with each other with a simple child-parent relationship, 
//but does not consider other types of more complex interactions. 
//For example, it does not recognize multiple-child to parent, 
//child to multiple-parent relationships, 
//or other types of interactions between features such as intersection. 
//Besides, the algorithm can currently work with independent branches, 
//without interference due to intersections.
{
	//Current deconstruction is initialized the same as the previous one
	//------------------------------------------------------------------
	CDB_Model DDBB_Dec_Current= pDoc->DB_Deconstruct[pDoc->DB_Deconstruct.size()-1];

	//Select the next feature to remove
	//(also updates father feature)
	//---------------------------------
	CFeature NextFeat;
	long FeatureId= SelectFeature(&DDBB_Dec_Current,
								  pDoc->DB_CSG_Model,
								  NextFeat, FeatureFather);

	//Store the feature to remove
	//---------------------------

	//Calculate all its information data
	std::vector<long> PairedVertL1, PairedVertL2;
	CompleteFeatureData(&DDBB_Dec_Current,
						pDoc->DB_DatumGraph,
						pDoc->DB_CSG_Model,
						NextFeat,                     //Populates feature data before storing it
						FeatureId, 
						PairedVertL1, PairedVertL2);  //Returns vertices paired

	//Add the feature to the current branch of the model tree
	pDoc->DB_CSG_Model->AddFeatureToBranch(NextFeat);

	//Simplify the model by removing the feature from the drawing
	//-----------------------------------------------------------
	RemoveFeature(&DDBB_Dec_Current, //Updates the current drawing
				  PairedVertL1, PairedVertL2,  //Uses paired vertices
				  FeatureId, 
				  NextFeat, 
				  FeatureFather);

	//Update deconstruction information for the next recursive iteration
	//------------------------------------------------------------------
	bool continueRecursion;

	if(NextFeat.GetType() == NODE || NextFeat.GetType() == CORE){
		continueRecursion= (GetNumValidFeatures() != 0);

		//Upload current internal branch to the tree
		pDoc->DB_CSG_Model->AddTreeBranch();

		//Begin a new branch
		pDoc->DB_CSG_Model->ClearBranch();
		FeatureFather= false;
	}

	else{
		continueRecursion= true;

		//Update cues and features
		//(This overwrites the only database where cues and features are stored)
		CDetectionManager DM;  //Uses interactively activated detection flags
		DM.DetectCuesAndFeatures(&DDBB_Dec_Current);

		//Check that critical cues have been updated
		if(ReferApp.Cu.MA.MainAngles.size() != 3){
			AfxMessageBox ("Detection of main axes failed while deconstructing CSG model");
			CReferDoc::GetDoc()->RefreshWindows();
			return;
		}
	}

	//Manage recursion
	//----------------
	if(continueRecursion){
		//Save current deconstruction
		pDoc->DB_Deconstruct.push_back(DDBB_Dec_Current);

		//Proceed to the next deconstruction nested level
		//-----------------------------------------------
		DeconstructCSG_2D(pDoc, FeatureFather);  //RECURSION
	}

	else{
		//Deconstruction is done
		//Calculate parametric profiles
		//(including their orthodistances)
		//--------------------------------
		GetParametricProfiles(pDoc->DB_DatumGraph, pDoc->DB_CSG_Model);
	}
}
