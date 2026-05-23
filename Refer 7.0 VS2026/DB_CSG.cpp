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
//#include <cmath>
#include <cassert>
#include <vector>

#include "Tools_Geometry.h"

#include "DB_Model.h"
#include "DB_CSG.h"


//----------------------------------------------------------------------
// DISTANCE
COrthoDistance::COrthoDistance()
{
	x= y= z= 0;
}


COrthoDistance::COrthoDistance(double xx, double yy, double zz)
:
x (xx),
y (yy),
z (zz)
{
}


COrthoDistance::COrthoDistance(COrthoDistance const &OD)
:
x (OD.x),
y (OD.y),
z (OD.z)
{
}


COrthoDistance::~COrthoDistance()
{
}


void COrthoDistance::Clear()
{
	x= y= z= 0;
}


//----------------------------------------------------------
//DATUM PLANE
CDatumPlane::CDatumPlane()
{
	Orientation= UNKNOWN;
	SubGraph= 0;
	Origin= 0;
	IsSymmetry= 0;
	normalDistance= 0;
	PlaneVertices.clear();
	IDDPlane= 0;
}


CDatumPlane::CDatumPlane(const CDatumPlane &DP)
:
Orientation (DP.Orientation),
SubGraph (DP.SubGraph),
Origin (DP.Origin),
IsSymmetry (DP.IsSymmetry),
normalDistance (DP.normalDistance),
PlaneVertices (DP.PlaneVertices),
IDDPlane (DP.IDDPlane)
{
}


CDatumPlane::~CDatumPlane()
{
}


void CDatumPlane::Clear()
{
	Orientation= UNKNOWN;
	SubGraph= 0;
	Origin= 0;
	IsSymmetry= 0;
	normalDistance= 0;
	PlaneVertices.clear();
	IDDPlane= 0;
}


void CDatumPlane::SetOrientation(CPlaneOrientation InOrientation)
{
	Orientation= InOrientation;
}


CPlaneOrientation CDatumPlane::GetOrientation()
{
	return Orientation;
}


void CDatumPlane::SetSubGraph(long InSubGraph)
{
	SubGraph= InSubGraph;
}


long CDatumPlane::GetSubGraph()
{
	return SubGraph;
}


void CDatumPlane::SetOrigin(long InOrigin)
{
	Origin= InOrigin;
}


long CDatumPlane::GetOrigin()
{
	return Origin;
}


void CDatumPlane::SetIsSymmetry(long InIsSymmetry)
{
	IsSymmetry= InIsSymmetry;
}


long CDatumPlane::GetIsSymmetry()
{
	return IsSymmetry;
}


void CDatumPlane::SetNormalDistance(double InNormalDistance)
{
	normalDistance= InNormalDistance;
}


double CDatumPlane::GetNormalDistance()
{
	return normalDistance;
}


void CDatumPlane::SetPlaneVertices(std::vector<long> InPlaneVertices)
{
	PlaneVertices= InPlaneVertices;
}


std::vector<long> CDatumPlane::GetPlaneVertices()
{
	return PlaneVertices;
}


long CDatumPlane::PlaneVerticesSize()
{
	return (long)PlaneVertices.size();
}


void CDatumPlane::AddPlaneVertex(long InPlaneVertex)
{
	PlaneVertices.push_back(InPlaneVertex);
}


long CDatumPlane::GetPlaneVertex(long Position)
{
	assert(Position >= 0 && Position < (long)PlaneVertices.size());
	return PlaneVertices[Position];
}


void CDatumPlane::SetIDDPlane(long InIDDPlane)
{
	IDDPlane= InIDDPlane;
}


long CDatumPlane::GetIDDPlane()
{
	return IDDPlane;
}


// DATUM AXIS
//CDatumAxis::CDatumAxis()
//{
//}
//
//
//CDatumAxis::CDatumAxis(const CDatumAxis &DA)
//:
//IDDAxis (DA.IDDAxis)
//{
//}
//
//CDatumAxis::~CDatumAxis()
//{
//}


// DATUMS
CDatums::CDatums()
{
	DatumPlanes.clear();
	//DatumAxes.clear();
}


CDatums::CDatums(const CDatums &DD)
:
DatumPlanes (DD.DatumPlanes)
//DatumAxes (DD.DatumAxes)
{
}

void CDatums::Clear()
{
	DatumPlanes.clear();
	//DatumAxes.clear();
}


CDatums::~CDatums()
{
}


long CDatums::DatumOriginsSize()
{
	return (long)DatumOrigins.size();
}


void CDatums::SetDatumOrigins(std::vector<long>InOrigins)
{
	DatumOrigins= InOrigins;
}


std::vector<long> CDatums::GetDatumOrigins()
{
	return DatumOrigins;
}


long CDatums::GetDatumOrigin(long numOrigin)
{
	return DatumOrigins[numOrigin];
}


void CDatums::SetOrthoDistancesOrigins(std::vector<COrthoDistance>InOrthoDistancesOrigins)
{
	OrthoDistancesOrigins= InOrthoDistancesOrigins;
}


std::vector<COrthoDistance> CDatums::GetOrthoDistancesOrigins()
{
	return OrthoDistancesOrigins;
}


long CDatums::DatumPlanesSize()
{
	return (long)DatumPlanes.size();
}


void CDatums::AddDatumPlane(CDatumPlane InDatumPlane)
{
	InDatumPlane.SetIDDPlane((long)DatumPlanes.size());
	DatumPlanes.push_back(InDatumPlane);
}


void CDatums::RemoveDatumPlane(long Position)
{
	assert(Position>=0 && Position<(long)DatumPlanes.size());
	DatumPlanes.erase(DatumPlanes.begin() + Position);
};


void CDatums::SetDatumPlane(CDatumPlane InDatumPlane, long Position)
{
	assert(Position >= 0 && Position < (long)DatumPlanes.size());
	DatumPlanes[Position]= InDatumPlane;
}


CDatumPlane CDatums::GetDatumPlane(long Position)
{
	assert(Position >= 0 && Position < (long)DatumPlanes.size());
	return DatumPlanes[Position];
}


long CDatums::GetDatumPlaneSubGraph(long numPlane)
{
	assert(numPlane >= 0 && numPlane < (long)DatumPlanes.size());
	return DatumPlanes[numPlane].GetSubGraph();
}


CPlaneOrientation CDatums::GetDatumPlaneOrientation(long numPlane)
{
	assert(numPlane >= 0 && numPlane < (long)DatumPlanes.size());
	return DatumPlanes[numPlane].GetOrientation();
}


void CDatums::SetDatumPlaneNormalDistance(long numPlane, double inNormalDistance)
{
	assert(numPlane >= 0 && numPlane < (long)DatumPlanes.size());
	DatumPlanes[numPlane].SetNormalDistance(inNormalDistance);
}


double CDatums::GetDatumPlaneNormalDistance(long numPlane)
{
	assert(numPlane >= 0 && numPlane < (long)DatumPlanes.size());
	return DatumPlanes[numPlane].GetNormalDistance();
}


void CDatums::SetDatumPlaneIsSymmetry(long numPlane, long InIsSymmetry)
{
	assert(numPlane >= 0 && numPlane < (long)DatumPlanes.size());
	assert(InIsSymmetry == 0 || InIsSymmetry == 1);
	DatumPlanes[numPlane].SetIsSymmetry(InIsSymmetry);
}


long CDatums::GetDatumPlaneIsSymmetry(long numPlane)
{
	assert(numPlane >= 0 && numPlane < (long)DatumPlanes.size());
	return DatumPlanes[numPlane].GetIsSymmetry();
}


void CDatums::SetDatumPlaneIDDPlane(long numPlane, long inIDDPlane)
{
	assert(numPlane >= 0 && numPlane < (long)DatumPlanes.size());
	DatumPlanes[numPlane].SetIDDPlane(inIDDPlane);
}


long CDatums::GetDatumPlaneIDDPlane(long numPlane)
{
	assert(numPlane >= 0 && numPlane < (long)DatumPlanes.size());
	return DatumPlanes[numPlane].GetIDDPlane();
}


long CDatums::DatumPlaneVerticesSize(long numPlane)
{
	assert(numPlane >= 0 && numPlane < (long)DatumPlanesSize());
	return DatumPlanes[numPlane].PlaneVerticesSize();
}


std::vector<long> CDatums::GetDatumPlaneVertices(long numPlane)
{
	assert(numPlane >= 0 && numPlane < (long)DatumPlanes.size());
	return DatumPlanes[numPlane].GetPlaneVertices();
}


long CDatums::GetDatumPlaneVertex(long numPlane, long numVertex)
{
	assert(numPlane >= 0 && numPlane < (long)DatumPlanes.size());
	assert(numVertex >= 0 && numVertex < (long)DatumPlanes[numPlane].PlaneVerticesSize());
	return DatumPlanes[numPlane].GetPlaneVertex(numVertex);
}


//-----------------------------------------------------------
// PROFILE CONSTRAINTS
//CProfileConstraint::CProfileConstraint()
//{
//}
//
//
//CProfileConstraint::CProfileConstraint(const CProfileConstraint &PC)
//:
//GeomConstraint (PC.GeomConstraint),
////ElementsType (PC.ElementsType),
//Elements (PC.Elements),
//ParallelGroups (PC.ParallelGroups)
//{
//}
//
//CProfileConstraint::~CProfileConstraint()
//{
//}


//-----------------------------------------------------------


// CSG FEATURES
CFeature::CFeature()
{
	FeatureType= NONE;
	Merit= 0.;
	SubGraph= -1;
	ContainerFaces.clear();
	DatumsId.clear();

	DB_Feature2D.Clear();

	CrossSectionVertices.clear();

	ParametricProfileCoorVertices.clear();

	ProfileOrientation= UNKNOWN;
	ExtrusionDirection= -1;
	LengthExtrusion= 0;
	RotationAngle= 0;
};


CFeature::CFeature(const CFeature &FT)
:
FeatureType (FT.FeatureType),
Merit (FT.Merit),
SubGraph (FT.SubGraph),
ContainerFaces (FT.ContainerFaces),
DatumsId (FT.DatumsId),

DB_Feature2D (FT.DB_Feature2D),

CrossSectionVertices (FT.CrossSectionVertices),

ParametricProfileCoorVertices (FT.ParametricProfileCoorVertices),

ProfileOrientation (FT.ProfileOrientation),
ExtrusionDirection (FT.ExtrusionDirection),
LengthExtrusion (FT.LengthExtrusion),
RotationAngle (FT.RotationAngle)
{
};


CFeature::~CFeature()
{
};


void CFeature::Clear()
{
	FeatureType= NONE;
	Merit= 0.;
	SubGraph= -1;
	ContainerFaces.clear();
	DatumsId.clear();

	DB_Feature2D.Clear();

	CrossSectionVertices.clear();

	ParametricProfileCoorVertices.clear();

	ProfileOrientation= UNKNOWN;
	ExtrusionDirection= -1;
	LengthExtrusion= 0;
	RotationAngle= 0;
};


void CFeature::SetType(FEATURE_TYPE InFeatureType)
{
	FeatureType= InFeatureType;
};


FEATURE_TYPE CFeature::GetType()
{
	return FeatureType;
};


void CFeature::SetMerit(double InMerit)
{
	Merit= InMerit;
};


double CFeature::GetMerit()
{
	return Merit;
};


void CFeature::SetSubGraph(long InSubGraph)
{
	SubGraph= InSubGraph;
};


long CFeature::GetSubGraph()
{
	return SubGraph;
};


void CFeature::AddContainerFace(std::vector<long> InContainerFace)
{
	ContainerFaces.push_back(InContainerFace);
};


std::vector<long> CFeature::GetContainerFace(long numFace)
{
	assert(numFace >= 0 && numFace < (long)ContainerFaces.size());
	return ContainerFaces[numFace];
};


long CFeature::ContainerFacesSize()
{
	return (long)ContainerFaces.size();
};


long CFeature::ContainerFaceSize(long numFace)
{
	assert(numFace >= 0 && numFace < (long)ContainerFaces.size());
	return (long)ContainerFaces[numFace].size();
};


long CFeature::ContainerFaceGetVertex(long numFace, long numVertex)
{
	assert(numFace >= 0 && numFace < (long)ContainerFaces.size());
	assert(numVertex >= 0 && numVertex < (long)ContainerFaces[numFace].size());
	return ContainerFaces[numFace][numVertex];
};


long CFeature::DatumIdSize()
{
	return (long)DatumsId.size();
};


void CFeature::AddDatumId(long numDatum)
{
	//assert(numDatum >= 0 && numDatum < (long)Datums.size());
	DatumsId.push_back(numDatum);
};


void CFeature::RemoveDatumId(long Position)
{
	assert(Position >= 0 && Position < (long)DatumsId.size());
	DatumsId.erase(DatumsId.begin() + Position);
};


void CFeature::SetDatumId(long numDatum, long Position)
{
	//assert(numDatum >= 0 && numDatum < (long)Datums.size());
	assert(Position >= 0 && Position < (long)DatumsId.size());
	DatumsId[Position]= numDatum;
};


long CFeature::GetDatumId(long Position)
{
	assert(Position >= 0 && Position < (long)DatumsId.size());
	return DatumsId[Position];
};


void CFeature::SetFeature2D(CDB_Model InDB_F)
//Updates the Feature2D database
{
	DB_Feature2D= InDB_F;
}


CDB_Model CFeature::GetFeature2D()
//Returns the Feature2D database
{
	return DB_Feature2D;
}


//void CFeature::SetCrossSection(std::vector<CVertex> In_CS)
////Updates the CrossSection vertices
//{
//	CrossSectionCoorVertices= In_CS;
//}


std::vector<long> CFeature::GetCrossSection()
//Returns the CrossSection vertices
{
	return CrossSectionVertices;
}


//void CFeature::ClearCrossSection()
////Clear the CrossSection vertices
//{
//	CrossSectionCoorVertices.clear();
//}


long CFeature::SizeCrossSection()
//Return the number of CrossSection vertices
{
	return (long)CrossSectionVertices.size();
}


void CFeature::AddCrossSectionVertex(long In_vertex)
//Add an input vertex to the CrossSection vertices
{
	CrossSectionVertices.push_back(In_vertex);
}


//void CFeature::SetCrossSectionVertex(long In_vertex, long Position)
////Updates the CrossSection vertex located at Position
//{
//	assert(Position >= 0 && Position < (long)CrossSectionVertices.size());
//	CrossSectionVertices[Position]= In_vertex;
//}


long CFeature::GetCrossSectionVertex(long Position)
//Returns the CrossSection vertex located at Position
{
	assert(Position >= 0 && Position < (long)CrossSectionVertices.size());
	return CrossSectionVertices[Position];
}


//void CFeature::SetParametricProfile(std::vector<long> InProfile)
////Updates the ParametricProfile vertices with the input profile
//{
//	ParametricProfileVertices= InProfile;
//}


//std::vector<long> CFeature::GetParametricProfile()
////Returns the ParametricProfile vertices
//{
//	return ParametricProfileVertices;
//}


//void CFeature::ClearParametricProfile()
////Clear the ParametricProfile vertices
//{
//	ParametricProfileVertices.clear();
//}


long CFeature::SizeParametricProfile()
//Return the number of ParametricProfile vertices
{
	return (long)ParametricProfileCoorVertices.size();
}


void CFeature::AddParametricProfileVertex(CVertex In_vertex)
//Add an input vertex to the ParametricProfile vertices
{
	ParametricProfileCoorVertices.push_back(In_vertex);
}


void CFeature::SetParametricProfileVertex(CVertex In_vertex, long Position)
//Updates the ParametricProfile vertex located at Position
{
	assert(Position >= 0 && Position < (long)ParametricProfileCoorVertices.size());
	ParametricProfileCoorVertices[Position]= In_vertex;
}


CVertex CFeature::GetParametricProfileVertex(long Position)
//Returns the ParametricProfile vertex located at Position
{
	assert(Position >= 0 && Position < (long)ParametricProfileCoorVertices.size());
	return ParametricProfileCoorVertices[Position];
}


void CFeature::SetProfileOrientation(CPlaneOrientation InProfileOrientation)
{
	ProfileOrientation= InProfileOrientation;
}


CPlaneOrientation CFeature::GetProfileOrientation()
{
	return ProfileOrientation;
}


void CFeature::SetExtrusionDirection(long InExtrusionDirection)
{
	ExtrusionDirection= InExtrusionDirection;
}


long CFeature::GetExtrusionDirection()
{
	return ExtrusionDirection;
}


void CFeature::SetLengthExtrusion(double InLengthExtrusion)
{
	LengthExtrusion= InLengthExtrusion;
}


double CFeature::GetLengthExtrusion()
{
	return LengthExtrusion;
}


//void CFeature::SetRotationAngle(double InRotationAngle)
//{
//	RotationAngle= InRotationAngle;
//}
//
//
//double CFeature::GetRotationAngle()
//{
//	return RotationAngle;
//}



	//-------------------------------------------------------------------------------
// DATABASE CSG
CDB_CSG::CDB_CSG()
{
}


CDB_CSG::CDB_CSG(CDB_CSG const &DB)
:
Datums (DB.Datums),
Tree (DB.Tree)
{
}


CDB_CSG::~CDB_CSG()
{
}


void CDB_CSG::Clear()
{
	ClearDatums();
	ClearTree();
}


//---------------------- Datums-----------------------

void CDB_CSG::ClearDatums()
{
	Datums.Clear();
}


long CDB_CSG::DatumOriginsSize()
{
	return Datums.DatumOriginsSize();
};


void CDB_CSG::SetDatumOrigins(std::vector<long>InDatumOrigins)
{
	Datums.SetDatumOrigins(InDatumOrigins);
}


std::vector<long> CDB_CSG::GetDatumOrigins()
{
	return Datums.GetDatumOrigins();
}


long CDB_CSG::GetDatumOrigin(long numOrigin)
{
	return Datums.GetDatumOrigin(numOrigin);
}


void CDB_CSG::SetOrthoDistancesOrigins(std::vector<COrthoDistance>InOrthoDistancesOrigins)
{
	Datums.SetOrthoDistancesOrigins(InOrthoDistancesOrigins);
}


std::vector<COrthoDistance> CDB_CSG::GetOrthoDistancesOrigins()
{
	return Datums.GetOrthoDistancesOrigins();
}


long CDB_CSG::DatumPlanesSize()
{
	return Datums.DatumPlanesSize();
};


void CDB_CSG::AddDatumPlane(CDatumPlane InDatumPlane)
{
	Datums.AddDatumPlane(InDatumPlane);
}


void CDB_CSG::SetDatumPlane(CDatumPlane InDatumPlane, long Position)
{
	Datums.SetDatumPlane(InDatumPlane, Position);
}


CDatumPlane CDB_CSG::GetDatumPlane(long numDatumPlane)
{
	return Datums.GetDatumPlane(numDatumPlane);
}


long CDB_CSG::GetDatumPlaneSubGraph(long numPlane)
{
	return Datums.GetDatumPlaneSubGraph(numPlane);
}


void CDB_CSG::SetDatumPlaneIDDPlane(long numPlane, long inIDDPlane)
{
	Datums.SetDatumPlaneIDDPlane(numPlane, inIDDPlane);
}


long CDB_CSG::GetDatumPlaneIDDPlane(long numPlane)
{
	return Datums.GetDatumPlaneIDDPlane(numPlane);
}


CPlaneOrientation CDB_CSG::GetDatumPlaneOrientation(long numPlane)
{
	return Datums.GetDatumPlaneOrientation(numPlane);
}


void CDB_CSG::SetDatumPlaneNormalDistance(long numPlane, double inNormalDistance)
{
	Datums.SetDatumPlaneNormalDistance(numPlane, inNormalDistance);
}


double CDB_CSG::GetDatumPlaneNormalDistance(long numPlane)
{
	return Datums.GetDatumPlaneNormalDistance(numPlane);
}


void CDB_CSG::SetDatumPlaneIsSymmetry(long numPlane, long InIsSymmetry)
{
	Datums.SetDatumPlaneIsSymmetry(numPlane, InIsSymmetry);
}


long CDB_CSG::GetDatumPlaneIsSymmetry(long numPlane)
{
	return Datums.GetDatumPlaneIsSymmetry(numPlane);
}


long CDB_CSG::DatumPlaneVerticesSize(long numPlane)
{
	return Datums.DatumPlaneVerticesSize(numPlane);
}


std::vector<long> CDB_CSG::GetDatumPlaneVertices(long numPlane)
{
	return Datums.GetDatumPlaneVertices(numPlane);
}


long CDB_CSG::GetDatumPlaneVertex(long numPlane, long numVertex)
{
	return Datums.GetDatumPlaneVertex(numPlane, numVertex);
}
//---------------------- End Datums-------------------




//---------------------- Branch of Features-----------

void CDB_CSG::ClearBranch()
//Clear the internal branch
{
	Branch.clear();
}


long CDB_CSG::BranchSize()
//Return the number of features of the internal branch
{
	return (long)Branch.size();
};


void CDB_CSG::AddFeatureToBranch(CFeature &Feature)
//Add an input feature at the end of the internal branch
{
	Branch.push_back(Feature);
};


CFeature CDB_CSG::GetFeatureFromBranch(long Position)
//Return the feature placed at "Position" of the internal branch
{
	assert(Position>=0 && Position<(long)Branch.size());
	return Branch[Position];
};
//---------------------- End Branch of Features---------


//---------------------- Tree of Branches---------------

void CDB_CSG::ClearTree()
//Clear the whole tree
{
	Tree.clear();
}


void CDB_CSG::AddTreeBranch()
//Add the internal branch to the Tree
{
	Tree.push_back(Branch);
}


long CDB_CSG::TreeBranchesSize()
//Return the number of branches in the Tree
{
	return (long)Tree.size();
}


long CDB_CSG::TreeFeaturesSize()
//Return the total number of features 
//in all the branches of the Tree
{
	long numFeatures= 0;
	long numBranches= (long)Tree.size();
	//Loop over all the branches
	for(long i= 0; i< numBranches; i++){
		numFeatures+= (long)Tree[i].size();
	}
	return numFeatures;
}


long CDB_CSG::TreeBranchSize(long numBranch)
//Return the total number of features 
//in the branch numBrach
{
	assert(numBranch>=0 && numBranch<(long)Tree.size());
	return (long)Tree[numBranch].size();
}


void CDB_CSG::SetFeature(long numBranch, long Position, CFeature InFeature)
//Update the feature located in the place "Position" of the branch "numBranch"
//with the contents of the external feature "InFeature"
{
	assert(numBranch>=0 && numBranch<(long)Tree.size());
	assert(Position>=0 && Position<(long)Tree[numBranch].size());
	Tree[numBranch][Position]= InFeature;
};


void CDB_CSG::SetFeature(long ith, CFeature InFeature)
//Update the i-th feature irrespective of its branch
{
	assert(ith>=0 && ith<TreeFeaturesSize());

	long prevFeatures= 0;
	long numBranches= (long)Tree.size();

	//Loop to get the i-th feature
	for(long i= 0; i< numBranches; i++){
		long numFeatures= (long)Tree[i].size();
		if(prevFeatures+numFeatures > ith){
			Tree[i][ith-prevFeatures]= InFeature;
			break;
		}
		prevFeatures+= numFeatures;
	}
};


CFeature CDB_CSG::GetFeature(long numBranch, long Position)
//Returns the feature located in the place "Position" of the branch "numBranch"
{
	assert(numBranch>=0 && numBranch<(long)Tree.size());
	assert(Position>=0 && Position<(long)Tree[numBranch].size());
	return Tree[numBranch][Position];
};


CFeature CDB_CSG::GetFeature(long ith)
//Returns the i-th feature irrespective of its branch
{
	assert(ith>=0 && ith<TreeFeaturesSize());

	long prevFeatures= 0;
	long numBranches= (long)Tree.size();

	//Loop to get the i-th feature
	for(long i= 0; i< numBranches; i++){
		long numFeatures= (long)Tree[i].size();
		if(prevFeatures+numFeatures > ith){
			return Tree[i][ith-prevFeatures];
		}

		prevFeatures+= numFeatures;
	}
	return Tree[0][0];
};
//---------------------- End Tree of branches---------
