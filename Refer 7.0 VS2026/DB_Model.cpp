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
#include <cmath>
#include <cassert>
#include <vector>
#include <algorithm>  //min, max

#include "Tools_Vector.h"
#include "Tools_Geometry.h"

#include "DB_Model.h"

//----------------------------------------------------------------------
// VERTICES
CVertex::CVertex()
{
	x= y= z= 0;
	merit= 0;
}


CVertex::CVertex(double xx, double yy, double zz)
:
x (xx),
y (yy),
z (zz)
{
	merit= 0;
}


CVertex::CVertex(double xx, double yy, double zz, double merit)
:
x (xx),
y (yy),
z (zz),
merit (merit)
{
}


CVertex::CVertex(CVertex const &V)
:
x (V.x),
y (V.y),
z (V.z),
merit (V.merit)
{
}


CVertex::~CVertex()
{
}


void CVertex::Clear()
{
	x= y= z= 0;
	merit= 0;
}

//----------------------------------------------------------------------
// VECTORS
CVector::CVector()
{
	//Components
	x= y= z= 0;

	merit= 0;

	//Tips
	Head.x= Head.y= Head.z= 0;
	Tail.x= Tail.y= Tail.z= 0;
}


CVector::CVector(double xx, double yy)
:
x (xx),
y (yy)
{
	z= 0;
	merit= 0;

	Head.x= x;
	Head.y= y;
	Head.z= z;
}


CVector::CVector(double xx, double yy, double zz)
:
x (xx),
y (yy),
z (zz)
{
	merit= 0;

	Head.x= x;
	Head.y= y;
	Head.z= z;
}


CVector::CVector(double xx, double yy, double zz, double merit)
:
x (xx),
y (yy),
z (zz),
merit (merit)
{
	Head.x= x;
	Head.y= y;
	Head.z= z;

	Tail.x= 0;
	Tail.y= 0;
	Tail.z= 0;
}



CVector::CVector(CVertex const &newHead, CVertex const &newTail)
:
Head (newHead),
Tail (newTail)
{
	x= Head.x - Tail.x;
	y= Head.y - Tail.y;
	z= Head.z - Tail.z;

	merit= 0;
}


CVector::CVector(CVector const &V)
:
x (V.x),
y (V.y),
z (V.z),
merit (V.merit),

Head (V.Head),
Tail (V.Tail)
{
}


CVector::~CVector()
{
}


void CVector::Clear()
{
	x= y= z= 0;

	merit= 0;

	Head.x= Head.y= Head.z= 0;
	Tail.x= Tail.y= Tail.z= 0;
}


//----------------------------------------------------------------------
// SETS OF VERTICES
CVerticesSet::CVerticesSet()
{
}


CVerticesSet::CVerticesSet(CVerticesSet const &VS)
:
SetOfVertices (VS.SetOfVertices)
{
}


CVerticesSet::~CVerticesSet()
{
}


void CVerticesSet::Clear()
{
	SetOfVertices.clear();
}


long CVerticesSet::GetSize()
{
	return (long)SetOfVertices.size();
}


void CVerticesSet::AddVertex(CVertex const &Vertex)
{
	SetOfVertices.push_back(Vertex);
}


CVertex CVerticesSet::GetVertex(long Position)
{
	assert(Position>=0 && Position<(long)SetOfVertices.size());
	return SetOfVertices[Position];
}

long CVerticesSet::GetVertexNum(CVertex const &Vertex)
{
	long position= -1;
	for(long i= 0; i< (long)SetOfVertices.size(); i++){
		if((SetOfVertices[i].x== Vertex.x)&&(SetOfVertices[i].y== Vertex.y)){
			position= i;
			return position;
			break;
		}
	}
	return position;
}

void CVerticesSet::SetVertex(long Position, CVertex const &Vertex)
{
	assert(Position>=0 && Position<(long)SetOfVertices.size());
	SetOfVertices[Position]= Vertex;
}


void CVerticesSet::RemoveVertex(long Position)
{
	assert(Position>=0 && Position<(long)SetOfVertices.size());
	SetOfVertices.erase(SetOfVertices.begin()+Position);
}


void CVerticesSet::GetBoundingBox(double &Xmin, double &Ymin, double &Zmin,
									double &Xmax, double &Ymax, double &Zmax)
// Returns the maximum and minimum coordinates of the List of Vertices
{
	if (SetOfVertices.size() == 0) return;

	Xmin = Xmax = SetOfVertices[0].x;
	Ymin = Ymax = SetOfVertices[0].y;
	Zmin = Zmax = SetOfVertices[0].z;
	for (long i=1; i<(long)SetOfVertices.size() ; i++){
		Xmin = std::min(Xmin, SetOfVertices[i].x);
		Xmax = std::max(Xmax, SetOfVertices[i].x);
		Ymin = std::min(Ymin, SetOfVertices[i].y);
		Ymax = std::max(Ymax, SetOfVertices[i].y);
		Zmin = std::min(Zmin, SetOfVertices[i].z);
		Zmax = std::max(Zmax, SetOfVertices[i].z);
	}
}


//----------------------------------------------------------------------
// EDGES
CEdge::CEdge()
{
	Head= 0;
	Tail= 0;
	ColorR= 0;
	ColorG= 0;
	ColorB= 0;
	Dashed= false;
}


CEdge::CEdge(long const &newHead, long const &newTail)
:
Head (newHead),
Tail (newTail)
{
	ColorR= 0;
	ColorG= 0;
	ColorB= 0;
	Dashed= false;
}


CEdge::CEdge(CEdge const &E)
:
Head (E.Head),
Tail (E.Tail),
ColorR (E.ColorR),
ColorG (E.ColorG),
ColorB (E.ColorB),
Dashed (E.Dashed)
{
}


CEdge::~CEdge()
{
}


//----------------------------------------------------------------------
// LISTS OF EDGES
CEdgesSet::CEdgesSet()
{
}


CEdgesSet::CEdgesSet(CEdgesSet const &ES)
:
SetOfEdges (ES.SetOfEdges)
{
}


CEdgesSet::~CEdgesSet()
{
}


void CEdgesSet::Clear()
{
	SetOfEdges.clear();
}


long CEdgesSet::GetSize()
{
	return (long)SetOfEdges.size();
}


void CEdgesSet::AddEdge(CEdge const &Edge)
//WARNING!  intended as a private function

//If this function is called from outside CDB_Model::AddEdge
//then the list of edges connected to the vertex is not updated!
{
	SetOfEdges.push_back(Edge);
}


CEdge CEdgesSet::GetEdge(long Position)
{
	assert(Position>=0 && Position<(long)SetOfEdges.size());

	return SetOfEdges[Position];
}


long CEdgesSet::GetEdgeNum(CEdge const &Edge)
{
	long position= -1;
	for(long i= 0; i<(long)SetOfEdges.size(); i++){
		if((Edge.Tail== SetOfEdges[i].Tail)&&(Edge.Head== SetOfEdges[i].Head) ||
			(Edge.Tail== SetOfEdges[i].Head)&&(Edge.Head== SetOfEdges[i].Tail))
			position= i;
	}
	return position;
}


void CEdgesSet::SetEdge(long Position, CEdge const &Edge)
{
	assert(Position>=0 && Position<(long)SetOfEdges.size());
	SetOfEdges[Position]= Edge;
}


void CEdgesSet::RemoveEdge(long Position)
//WARNING! intended as a private function

//If this function is called from outside CDB_Model::RemoveEdge
//then the list of edges connected to the vertex is not updated!
{
	assert(Position>=0 && Position<(long)SetOfEdges.size());
	SetOfEdges.erase(SetOfEdges.begin()+Position);
}


//----------------------------------------------------------------------
// ARCS
CArc::CArc()
{
	Centre_x= 0.;
	Centre_y= 0.;
	Radius_a= 0.;
	Radius_b= 0.;
	Rotation= 0.;
	Focus1_x= 0.;
	Focus1_y= 0.;
	Focus2_x= 0.;
	Focus2_y= 0.;

	StartAngle= 0.;
	EndAngle= 0.;
	GapEnds= 0.;

	ColorR= 0;
	ColorG= 0;
	ColorB= 0;
}


CArc::CArc(CArc const &A)
:
Centre_x (A.Centre_x),
Centre_y (A.Centre_y),
Radius_a (A.Radius_a),
Radius_b (A.Radius_b),
Rotation (A.Rotation),
Focus1_x (A.Focus1_x),
Focus1_y (A.Focus1_y),
Focus2_x (A.Focus2_x),
Focus2_y (A.Focus2_y),

StartAngle (A.StartAngle),
EndAngle (A.EndAngle),
GapEnds (A.GapEnds),

//T0_x (A.T0_x),
//T0_y (A.T0_y),
//TM_x (A.TM_x),
//TM_y (A.TM_y),
//T1_x (A.T1_x),
//T1_y (A.T1_y),

ColorR (A.ColorR),
ColorG (A.ColorG),
ColorB (A.ColorB)
{
}


CArc::~CArc()
{
}


void CArc::clear()
{
	Centre_x= 0.;
	Centre_y= 0.;
	Radius_a= 0.;
	Radius_b= 0.;
	Rotation= 0.;
	Focus1_x= 0.;
	Focus1_y= 0.;
	Focus2_x= 0.;
	Focus2_y= 0.;

	StartAngle= 0.;
	EndAngle= 0.;
	GapEnds= 0.;

	ColorR= 0;
	ColorG= 0;
	ColorB= 0;
}


bool CArc::operator==(const CArc& right) const
//Overload of == operator for CArcs
{
	return (this->Centre_x==right.Centre_x && this->Centre_y==right.Centre_y &&
			this->Radius_a==right.Radius_a && this->Radius_b==right.Radius_b &&
			this->Rotation==right.Rotation &&
			this->Focus1_x==right.Focus1_x && this->Focus1_y==right.Focus1_y &&
			this->Focus2_x==right.Focus2_x && this->Focus2_y==right.Focus2_y &&
			this->StartAngle==right.StartAngle &&
			this->EndAngle==right.EndAngle &&
			this->GapEnds==right.GapEnds &&
			this->ColorR==right.ColorR && this->ColorG==right.ColorG && this->ColorB==right.ColorB);
}


//----------------------------------------------------------------------
// LISTS OF ARCS
CArcsSet::CArcsSet()
{
}


CArcsSet::CArcsSet(CArcsSet const &AL)
:
SetOfArcs (AL.SetOfArcs)
{
}


CArcsSet::~CArcsSet()
{
}


void CArcsSet::Clear()
{
	SetOfArcs.clear();
}


long CArcsSet::GetSize()
{
	return (long)SetOfArcs.size();
}


void CArcsSet::AddArc(CArc const &Arc)
//WARNING!
//If this function is called from outside CDB_Model::AddArc
//then the list of edges connected to the vertex is not updated!
{
	SetOfArcs.push_back(Arc);
}


CArc CArcsSet::GetArc(long Position)
{
	assert(Position>=0 && Position<(long)SetOfArcs.size());
	return SetOfArcs[Position];
}


void CArcsSet::SetArc(long Position, CArc const &Arc)
{
	assert(Position>=0 && Position<(long)SetOfArcs.size());
	SetOfArcs[Position]= Arc;
}


void CArcsSet::RemoveArc(long Position)
//WARNING!
//If this function is called from outside CDB_Model::RemoveArc
//then the list of arcs connected to the vertices is not updated!
{
	assert(Position>=0 && Position<(long)SetOfArcs.size());
	SetOfArcs.erase(SetOfArcs.begin()+Position);
}


//-------------------------------------------------
// FACES
CFace::CFace()
{
}


CFace::CFace(CFace const &C)
:
EdgesInFace (C.EdgesInFace),
Axes (C.Axes),
Normal (C.Normal),
Triangulation (C.Triangulation),
TriangulationNormals (C.TriangulationNormals)
{
}


CFace::~CFace()
{
}


void CFace::Clear()
{
	EdgesInFace.clear();
	Axes.clear();
	Normal.Clear();
	Triangulation.clear();
	TriangulationNormals.clear();
}


void CFace::AddEdgeInFace(long IndexEdge)
{
	EdgesInFace.push_back(IndexEdge);
}


void CFace::SetEdgeInFace(long Position, long IndexEdge)
// Adds "IndexEdge" to the list of edges connected to the face
// The new element is inserted in position "Position"
{
	assert(Position>=0 && Position<=(long)EdgesInFace.size());
	EdgesInFace.insert(EdgesInFace.begin()+Position, IndexEdge);
}


void CFace::RemoveEdgeInFace(long Position)
// Removes the "Position"th edge connected to the face
{
	assert(Position>=0 && Position<(long)EdgesInFace.size());
	EdgesInFace.erase(EdgesInFace.begin()+Position);
}


bool CFace::SearchEdgeInFace(long IndexEdge, long &Position)
// Returns true if the face contains the edge "IndexEdge"
// Also returns the position of this edge in the list of edges
{
	return IsValueInVector(IndexEdge, EdgesInFace, Position);
}


bool CFace::SearchEdgeInFace(long IndexEdge)
// Returns true if the face contains the edge "IndexEdge"
{
	long Position;
	return IsValueInVector(IndexEdge, EdgesInFace, Position);
}


long CFace::GetSizeEdgesInFace()
{
	return (long)EdgesInFace.size();
}


long CFace::GetEdgeInFace(long Position)
// Returns the index of the ith edge connected to the face
{
	assert(Position>=0 && Position<(long)EdgesInFace.size());
	return EdgesInFace[Position];
}


std::vector<long> CFace::GetAllEdgesInFace()
{
	return EdgesInFace;
}


void CFace::SetAllEdgesInFace(std::vector<long> &Origin)
{
	EdgesInFace= Origin;
}


void CFace::RemoveAllEdgesInFace()
{
	EdgesInFace.clear();
}


void CFace::CopyEdgesInFaceTo(CFace &Destination)
{
	Destination.EdgesInFace= EdgesInFace;
}


void CFace::AddSymAxisInFace(long IndexAxis)
// Adds "IndexAxis" to the list of axes connected to the face
{
	Axes.push_back(IndexAxis);
}


//void CFace::SetSymAxisInFace(long Position, long IndexAxis)
//{
//	Axes[Position]= IndexAxis;
//}


void CFace::RemoveSymAxisInFace(long Position)
// Removes the axis "Position" connected to the face
{
	Axes.erase(Axes.begin()+Position);
}


long CFace::GetSizeSymAxesInFace()
// Returns the number of axes connected to the face
{
	return (long)Axes.size();
}


long CFace::GetSymAxisInFace(long IndexAxis)
// Returns the index "IndexAxis" of the ith axis of the face
{
	return Axes[IndexAxis];
}


bool CFace::SearchSymAxisInFace(long IndexAxis, long &Position)
// Returns true if the face "Cara" contains the axis "IndexAxis"
// also return the position of this axis in the list of axes
{
	return IsValueInVector(IndexAxis, Axes, Position);
}


void CFace::SetNormal(CVector &newNormal)
//Save a Normal vector calculated outside
{
	Normal= newNormal;
}


CVector CFace::GetNormal(bool unit)
{
	if(!unit)
		return Normal;
	else{
		double module= MODULE1_3D(Normal);
		CVector N1(Normal.x/module, Normal.y/module, Normal.z/module);
		return N1;
	}
}


void CFace::ReverseFace()
// Reverses the order of the edges of the polygon that defines the face NumFace

//This is typically done after determining the normal that points outward
//the BRep model where the face belongs
//The purpose is to get a counter clockwise sequence of edges
//thus producing a normal vector pointing outwards
{
	long nEdges= this->GetSizeEdgesInFace();
	std::vector<long> reversedEdges(nEdges);

	for(long i= 0; i<nEdges; i++){
		reversedEdges[i]= this->GetEdgeInFace(nEdges-1-i);
		this->RemoveEdgeInFace(nEdges-1-i);
	}

	for(long i= 0; i<nEdges; i++){
		this->AddEdgeInFace(reversedEdges[i]);
	}
}


void CFace::SetFaceTriangulation(std::vector<long> &Triangles)
{
	Triangulation= Triangles;
}


std::vector<long> CFace::GetFaceTriangulation()
{
	return Triangulation;
}


void CFace::SetFaceTrianglesNormals(std::vector<CVector> &TrianglesNormals)
{
	TriangulationNormals= TrianglesNormals;
}


std::vector<CVector> CFace::GetFaceTrianglesNormals()
{
	return TriangulationNormals;
}


//--------------------------------------------------------------------
// LISTS OF FACES
CFacesSet::CFacesSet()
{
}


CFacesSet::CFacesSet(CFacesSet const &LC)
:
SetOfFaces (LC.SetOfFaces)
{
}


CFacesSet::~CFacesSet()
{
}


void CFacesSet::Clear()
{
	SetOfFaces.clear();
}


long CFacesSet::GetSize()
{
	return (long)SetOfFaces.size();
}


void CFacesSet::AddFace(CFace const &Face)
{
	SetOfFaces.push_back(Face);
}


long CFacesSet::IsFace(CFace Face)
//Returns the position of the face in the collection of faces
//whose contour edges match those of "Face"

//Returns -1 if such a face is not included in the collection of faces
{
	long numEdgesFace= Face.GetSizeEdgesInFace();
	std::vector<long> EdgesInFace= Face.GetAllEdgesInFace();

	for(long i=0; i<(long)SetOfFaces.size(); i++){
		long numEdges= SetOfFaces[i].GetSizeEdgesInFace();
		if(numEdges == numEdgesFace){
			bool equal= true;
			for(long j=0; j<numEdges; j++){
				if(SetOfFaces[i].GetEdgeInFace(j) != EdgesInFace[j]){
					equal= false;
					break;
				}
			}
			if(equal)
				return i;
		}
	}

	return -1;
}


CFace CFacesSet::GetFace(long Position)
{
	assert(Position>=0 && Position<(long)SetOfFaces.size());
	return SetOfFaces[Position];
}


void CFacesSet::SetFace(long Position, CFace const &Face)
{
	assert(Position>=0 && Position<(long)SetOfFaces.size());
	SetOfFaces[Position]= Face;
}


void CFacesSet::RemoveFace(long Position)
{
	assert(Position>=0 && Position<(long)SetOfFaces.size());
	SetOfFaces.erase(SetOfFaces.begin()+Position);
}



//-------------------------------------------------------------------------------
// DATABASES
CDB_Model::CDB_Model()
{
}


CDB_Model::CDB_Model(CDB_Model const &DB)
:
CVertices (DB.CVertices),
CEdges (DB.CEdges),
CArcs (DB.CArcs),
CFaces (DB.CFaces)
{
}


CDB_Model::~CDB_Model()
{
}


void CDB_Model::Clear()
{
	CVertices.Clear();
	CEdges.Clear();
	CArcs.Clear();
	CFaces.Clear();
}


void CDB_Model::ClearFaces()
{
	CFaces.Clear();
}

void CDB_Model::ClearVertices()
{
	CVertices.Clear();
}

void CDB_Model::CopyVerticesTo(CDB_Model *Destination)
{
	Destination->CVertices= CVertices;
}


void CDB_Model::CopyVerticesToVector(std::vector<CVertex> &Vertices)
//Outputs the vertices of the model in the vector "Vertices"
{
	for(long i=0; i<(long)GetSizeVertices(); i++){
		Vertices.push_back(GetVertex(i));
	}
}


void CDB_Model::CopyEdgesTo(CDB_Model *Destination)
{
	Destination->CEdges= CEdges;
}


void CDB_Model::CopyEdgesToVector(std::vector<CEdge> &EdgesVector)
{
	for(long i= 0; i<(long)GetSizeEdges(); i++){
		EdgesVector.push_back(GetEdge(i));
	}
}


void CDB_Model::CopyFacesTo(CDB_Model *Destination)
{
	Destination->CFaces= CFaces;
}


long CDB_Model::GetSizeVertices()
{
	return CVertices.GetSize();
}

long CDB_Model::GetSizeEdges()
{
	return CEdges.GetSize();
}


long CDB_Model::GetSizeArcs()
{
	return CArcs.GetSize();
}


long CDB_Model::GetSizeFaces()
{
	return CFaces.GetSize();
}


void CDB_Model::AddVertex(CVertex const &Vertex)
{
	CVertices.AddVertex(Vertex);
}


long CDB_Model::AddEdge(CEdge const &Edge)
//Returns the number of the edge
{
	//Add the edge to the list of edges
	CEdges.AddEdge(Edge);
	return CEdges.GetSize()-1;

	////Modify the vertices to add the new edge connected to them
	//long lPositionEdge= GetSizeEdges() - 1;

	//CVertex VertexH= GetVertex(Edge.Head);
	//VertexH.AddEdgeInVertex(lPositionEdge);
	//SetVertex(Edge.Head, VertexH);

	//CVertex VertexT= GetVertex(Edge.Tail);
	//VertexT.AddEdgeInVertex(lPositionEdge);
	//SetVertex(Edge.Tail, VertexT);
}


long CDB_Model::AddEdgeByVertices(double X1, double Y1, double Z1,
								  double X2, double Y2, double Z2)
// Inserts one edge in the DataBase
// The edge is defined by the coordinates of its two vertices:
//   (X1, Y1, Z1) is the first vertex 
//   (X2, Y2, Z2) is the second vertex
// If the vertices are not yet included in the database, the funtion does include them
// Returns the number of the new edge

// WARNING! The function does not check if the edge already exists
//(i.e. if one edge connecting exactly the two vertices is already included in the database)
{
	CEdge NewEdge;
	long locationVertex;

	//Search for vertices in the database, to add them if not previously included
	CVertex Vertice1;
	Vertice1.x = X1;
	Vertice1.y = Y1;
	Vertice1.z = Z1;
	if (IsVertex(Vertice1, locationVertex))
		//Vertex 1 already exists
		NewEdge.Head= locationVertex;
	else{
		//Vertex 1 is added to the database
		NewEdge.Head= GetSizeVertices();
		AddVertex(Vertice1);
	}

	CVertex Vertice2;
	Vertice2.x = X2;
	Vertice2.y = Y2;
	Vertice2.z = Z2;
	if (IsVertex(Vertice2, locationVertex))
		//Vertex 2 already exists
		NewEdge.Tail= locationVertex;
	else{
		//Vertex 2 is added to the database
		NewEdge.Tail= GetSizeVertices();
		AddVertex(Vertice2);
	}

	//Add the edge to the database
	return AddEdge(NewEdge);
}


long CDB_Model::AddArc(CArc const &Arc)
{
	CArcs.AddArc(Arc);
	return CArcs.GetSize()-1;
}


void CDB_Model::AddFace(CFace const &Face)
{
	CFaces.AddFace(Face);
}


long CDB_Model::IsFace(CFace Face)
{
	return CFaces.IsFace(Face);
}


CVertex CDB_Model::GetVertex(long Position)
{
	return CVertices.GetVertex(Position);
}


CEdge CDB_Model::GetEdge(long Position)
{
	return CEdges.GetEdge(Position);
}


//Twin vertices and edges//////////////////////////////////
//Useful functions to find twin vertices and edges 
//between deconstruction and datum graph


//double CDB_Model::MinVertexDist()
////Returns the euclidean distance 
////between the closest vertices in the list of vertices
//{
//	CVertex Vertex0= CVertices.GetVertex(0);
//	CVertex Vertex1= CVertices.GetVertex(1);
//	double minDist= MODULE_2D(Vertex0, Vertex1);
//
//	//long ii= 0, jj= 1;
//
//	long numVertices= CVertices.GetSize();
//	for (long i= 0; i<numVertices-1; i++){
//		CVertex VertexI= CVertices.GetVertex(i);
//		for (long j= i+1; j<numVertices; j++){
//			CVertex VertexJ= CVertices.GetVertex(j);
//
//			double Dist= MODULE_2D(VertexI, VertexJ);
//			if (Dist < minDist){
//				minDist= Dist;
//				//ii= i;
//				//jj= j;
//			}
//		}
//	}
//	return minDist;
//}


//long CDB_Model::IsNearVertex(CVertex Vertex, double threshold)
////Returns the number of the vertex in the list of vertices
////whose coordinates are close enought to those of "Vertex"
//
////Returns -1 if none matching vertex is found in the list
//
////A threshold is used to check the match of each coordinate
//{
//	for (long i= 0; i<CVertices.GetSize(); i++){
//		CVertex VerticeI= CVertices.GetVertex(i);
//		if ((std::fabs(VerticeI.x-Vertex.x) < threshold) && 
//			(std::fabs(VerticeI.y-Vertex.y) < threshold) &&
//			(std::fabs(VerticeI.z-Vertex.z) < threshold)){
//				return i;
//		}
//	}
//	return -1;
//}


long CDB_Model::GetVertexNum(CVertex Vertex)
//Returns the number of the vertex in the list of vertices
//whose coordinates match those of "Vertex"

//Returns -1 if none matching vertex is found in the list

//An "Accuracy" threshold is used to check the match of each coordinate
{
	for (long i= 0; i<CVertices.GetSize(); i++){
		CVertex VerticeI= CVertices.GetVertex(i);
		if ((std::fabs(VerticeI.x-Vertex.x) < Accuracy) && 
			(std::fabs(VerticeI.y-Vertex.y) < Accuracy) &&
			(std::fabs(VerticeI.z-Vertex.z) < Accuracy)){
				return i;
		}
	}
	return -1;
}


long CDB_Model::GetEdgeNum(CEdge const &Edge)
//Return the position of the edge in the edge list

//Return -1 if the edge is not included in the list

//Both semi-edges (Tail-Head, Head-Tail) are considered the same
{
	return CEdges.GetEdgeNum(Edge);
}


long CDB_Model::GetEdgeByVerticesNums(long Vertex1, long Vertex2)
//Gets the number of the edge that connects Vertex1 to Vertex2
//Returns -1 if vertices are not connected

//Returns the first edge that matches the query
//(It is assumed than no more than one edge connects the same two vertices)
{
	for (long i=0 ; i<GetSizeEdges(); i++){
		CEdge Edge= GetEdge(i);
		if((Edge.Head == Vertex1 && Edge.Tail == Vertex2) ||
		   (Edge.Head == Vertex2 && Edge.Tail == Vertex1)){
			return i;
		}
	}
	return -1;
}


//End Twin vertices and edges//////////////////////////////


CArc CDB_Model::GetArc(long Position)
{
	return CArcs.GetArc(Position);
}


CFace CDB_Model::GetFace(long Position)
{
	return CFaces.GetFace(Position);
}


long CDB_Model::GetFaceByVertices(std::vector<long> numVertices)
//The algorithm finds the face to which all the vertices of a list belong

//The input list includes the numbers of the vertices
{
	if(numVertices.size()>0){
		for(long i=0; i<GetSizeFaces(); i++){
			long QtyVer= 0;

			for(long j=0; j<(long)numVertices.size(); j++){
				if( IsVertexInFace(i, numVertices[j]))
					QtyVer++;
			}
			if(QtyVer == (long) numVertices.size())
				return i;
		}
		return -1;
	}
	else return -1;
}


long CDB_Model::GetFaceByVertices(std::vector<CVertex> Vertices)
//The algorithm finds the face to which all the vertices of a list belong

//The input list includes the vertex information of the vertices
{
	//Determine the numbers of the vertices
	std::vector<long> numVertices;
	for(long j=0; j<(long)Vertices.size(); j++){
		numVertices.push_back(GetVertexNum(Vertices[j]));
	}

	return GetFaceByVertices(numVertices);
}


std::vector<long> CDB_Model::GetFacesSharingVertex(long vertex)
//Returns the list of all faces that contain the "vertex"
{
	std::vector<long> FacesContainingVertex;

	for(long i=0; i<GetSizeFaces(); i++){
		std::vector<long> vertices= GetAllVerticesInFace(GetFace(i));
		if(IsValueInVector(vertex, vertices))
			FacesContainingVertex.push_back(i);
	}
	return FacesContainingVertex;
}


std::vector<long> CDB_Model::GetFacesSharingEdge(long edge)
//Returns the list of all faces that contain the "edge"
{
	std::vector<long> FacesContainingEdge;

	for(long i=0; i<GetSizeFaces(); i++){
		CFace iFace= GetFace(i);
		std::vector<long> EdgesInFace= iFace.GetAllEdgesInFace();

		if(IsValueInVector(edge, EdgesInFace))
			FacesContainingEdge.push_back(i);
	}
	return FacesContainingEdge;
}


std::vector<std::vector<long>> CDB_Model::GetAllFacesSharingEdges()
//Returns the list of all edges, 
//containing the lists of faces that are connected to each edge
{
	std::vector<std::vector<long>> FacesSharedByEdges;
	for(long i=0; i<GetSizeEdges(); i++){
		FacesSharedByEdges.push_back(std::vector<long>());
		FacesSharedByEdges[i]= GetFacesSharingEdge(i);
	}
	return FacesSharedByEdges;
}


long CDB_Model::GetSizeEdgesInVertex(long iVertex)
// Returns the total number of edges connected to the vertex "iVertex"
{
	long NumEdges= 0;
	for(long i= 0; i<GetSizeEdges(); i++){
		CEdge EdgeI= GetEdge(i);
		if(EdgeI.Head==iVertex || EdgeI.Tail==iVertex)
			NumEdges++;
	}
	return NumEdges;
}


long CDB_Model::GetEdgeInVertex(long iVertex, long ith)
// Returns the index of the ith edge connected to the vertex "iVertex"
{
	long NumEdgesInVertex= -1;
	for(long i= 0; i<GetSizeEdges(); i++){
		CEdge EdgeI= GetEdge(i);
		if(EdgeI.Head==iVertex || EdgeI.Tail==iVertex)
			NumEdgesInVertex++;

		if(NumEdgesInVertex == ith)
			return i;
	}
	return -1;
}


std::vector<long> CDB_Model::GetAllEdgesInVertex(long iVertex)
// Returns the edges connected to the vertex "iVertex"
{
	std::vector<long> Destination;
	for(long i= 0; i<GetSizeEdges(); i++){
		CEdge EdgeI= GetEdge(i);
		if(EdgeI.Head==iVertex || EdgeI.Tail==iVertex)
			Destination.push_back(i);
	}
	return Destination;
}


std::vector<long> CDB_Model::GetAllEdgesNotInVertex(long iVertex)
// Returns the edges not connected to the vertex "iVertex"
{
	std::vector<long> Destination;
	for(long i= 0; i<GetSizeEdges(); i++){
		CEdge EdgeI= GetEdge(i);
		if(EdgeI.Head!=iVertex && EdgeI.Tail!=iVertex)
			Destination.push_back(i);
	}
	return Destination;
}


bool CDB_Model::GetVerticesConnectedToVertex(CVertex Vertex, 
											 std::vector<CVertex> &ConnectedVertices)
//Updates the current list of ConnectedVertices 
//by adding all the edges connected to "Vertex"
{
	long vert= GetVertexNum(Vertex);
	if(vert == -1)
		return false;

	std::vector<long> EdgesConnectedToVertex= GetAllEdgesInVertex(vert);

	for (long i= 0; i< (long) EdgesConnectedToVertex.size(); i++){
		CEdge Edge= GetEdge(EdgesConnectedToVertex[i]);
		if (Edge.Head == vert)
			ConnectedVertices.push_back(GetVertex(Edge.Tail));
		else
			ConnectedVertices.push_back(GetVertex(Edge.Head));
	}

	return true;
}


CVertex CDB_Model::GetMidPointInEdge(long edge)
//Returns the coordinates of a point
//located in the middle of segment "edge"
{
	CEdge Edge= GetEdge(edge);
	CVertex Head= GetVertex(Edge.Head);
	CVertex Tail= GetVertex(Edge.Tail);

	CVertex MidPoint;
	MidPoint.x= (Head.x + Tail.x)/2;
	MidPoint.y= (Head.y + Tail.y)/2;
	MidPoint.z= (Head.z + Tail.z)/2;

	return MidPoint;
}


long CDB_Model::GetSizeVerticesInFace(long iFace)
// Returns the total number of vertices belonging to the face "iFace"
{
	CFace Face= GetFace(iFace);
	return Face.GetSizeEdgesInFace();  //In a polygon, Num vertices= Num edges
}


long CDB_Model::GetVertexInFace(long iFace, long iVertex)
//Returns the index of the ith vertex (in the range 0 to numvertices-1))
//belonging to the face "iFace"

//Vertices are taversed in the same order as edges

//WARNING: repeatedly calling this function to get all the vertices of a face
//         is much less efficient than calling "GetAllVerticesInFace"
{
	CFace face= GetFace(iFace);
	std::vector<long> edges= face.GetAllEdgesInFace();

	for(long i= 0; i<(long)edges.size(); i++){
		CEdge Edgei= GetEdge(edges[i]);
		CEdge Edgei1= GetEdge(edges[i+1]);
		if(Edgei.Head == Edgei1.Tail || Edgei.Head == Edgei1.Head){
			if(iVertex==0) return Edgei.Tail;
			if(iVertex==i+1) return Edgei.Head;
		}
		else{
			if(iVertex==0) return Edgei.Head;
			if(iVertex==i+1) return Edgei.Tail;
		}
	}

	return -1;
}


std::vector<long> CDB_Model::GetAllVerticesInFace(CFace &face)
// Calculates the list of vertices of the face "face"
// The ordered list of numbers of vertices is returned

//Vertices are traversed in the same order as edges
{
	std::vector<long> edges= face.GetAllEdgesInFace();

	std::vector<long> Polygon;
	for(long i= 0; i<(long)edges.size()-1; i++){
		CEdge Edgei= GetEdge(edges[i]);
		CEdge Edgei1= GetEdge(edges[i+1]);
		if(Edgei.Head == Edgei1.Tail || Edgei.Head == Edgei1.Head){
			if(i==0) Polygon.push_back(Edgei.Tail);  //Add both vertices only for the first edge
			Polygon.push_back(Edgei.Head);
		}
		else{
			if(i==0) Polygon.push_back(Edgei.Head);  //Add both vertices only for the first edge
			Polygon.push_back(Edgei.Tail);
		}
	}

	return Polygon;
}


std::vector<long> CDB_Model::GetAllVerticesInFace(long iface)
// Calculates the list of vertices of the face "iface"
// The ordered list of numbers of vertices is returned

//Vertices are traversed in the same order as edges
{
	return GetAllVerticesInFace(GetFace(iface));
}


std::vector<POINT2D> CDB_Model::GetFaceAsPolygon(CFace &face)
// Calculates the list of vertices of the face "face"
// The ordered list of points 2D corresponding to the vertices is returned

//Vertices are taversed in the same order as edges
{
	std::vector<long> edges= face.GetAllEdgesInFace();

	CVertex v;
	std::vector<POINT2D> polygonPoints;

	for(long i= 0; i<(long)edges.size()-1; i++){
		CEdge Edgei= GetEdge(edges[i]);
		CEdge Edgei1= GetEdge(edges[i+1]);
		if(Edgei.Head == Edgei1.Tail || Edgei.Head == Edgei1.Head){
			if(i==0){
				//Add both vertices only for the first edge
				v= GetVertex(Edgei.Tail);
				POINT2D p(v.x, v.y);
				polygonPoints.push_back(p);
			}
			v= GetVertex(Edgei.Head);
			POINT2D p(v.x, v.y);
			polygonPoints.push_back(p);
		}
		else{
			if(i==0){
				//Add both vertices only for the first edge
				v= GetVertex(Edgei.Head);
				POINT2D p(v.x, v.y);
				polygonPoints.push_back(p);
			}
			v= GetVertex(Edgei.Tail);
			POINT2D p(v.x, v.y);
			polygonPoints.push_back(p);
		}
	}

	return polygonPoints;
}


std::vector<CVertex> CDB_Model::GetAllCoordinateVerticesInFace(long iface)
//Returns a vector with the coordinates of all the vertices of face "Cara"
{
	std::vector<long> Vertices= GetAllVerticesInFace(GetFace(iface));
	std::vector<CVertex> Coordinates;

	for(long i= 0; i<(long) Vertices.size(); i++){
		CVertex Vertex= GetVertex(Vertices[i]);
		Coordinates.push_back(Vertex);
	}

	return Coordinates;
}


std::vector<long> CDB_Model::GetAllEdgesInFace(long iface)
//Returns a vector with all the edges of face "iface"
{
	CFace face= GetFace(iface);
	return face.GetAllEdgesInFace();
}


bool CDB_Model::IsVertexInFace(CFace &Face, long iVertex)
// Returns true if vertex iVertex belongs to the face "Face"
{
	std::vector<long> Edges= Face.GetAllEdgesInFace();

	for(long i= 0; i<(long)Edges.size(); i++){
		CEdge EdgeI= GetEdge(Edges[i]);
		if(EdgeI.Tail == iVertex){
			return true;
		}
		else if(EdgeI.Head == iVertex){
			return true;
		}
	}
	return false;
}


bool CDB_Model::IsVertexInFace(long iFace, long iVertex)
// Returns true if vertex iVertex belongs to the face "iFace"
{
	return IsVertexInFace(GetFace(iFace), iVertex);
}


bool CDB_Model::IsVertex(CVertex Vertex, long &Num)
// Searches a vertex whose coordinates match those of "Vertex"
// Returns "true" and its position "Num" if finds it
// Returns "false" otherwise.
{
	Num= -1;

	for (long i= 0; i<CVertices.GetSize(); i++){
		CVertex VerticeI= CVertices.GetVertex(i);
		if ((std::fabs(VerticeI.x-Vertex.x) < Accuracy) && 
			(std::fabs(VerticeI.y-Vertex.y) < Accuracy) &&
			(std::fabs(VerticeI.z-Vertex.z) < Accuracy)){
				Num= i;
				return true;
		}
	}
	return false;
}


bool CDB_Model::IsEdgeInFace(long iFace, long iEdge)
// Returns true if edge iEdge belongs to the face "iFace"
{
	CFace Face= GetFace(iFace);
	return Face.SearchEdgeInFace(iEdge);
}


void CDB_Model::GetBoundingBox(double &Xmin, double &Ymin, double &Zmin,
							   double &Xmax, double &Ymax, double &Zmax)
{
	CVertices.GetBoundingBox(Xmin, Ymin, Zmin, Xmax, Ymax, Zmax);
}


double CDB_Model::GetLength2D(long IndexEdge)
//Gets the length of the edge "IndexEdge"
//Considers only X and Y coordinates
{
	CEdge Edge= GetEdge(IndexEdge);
	return MODULE_2D(GetVertex(Edge.Head), GetVertex(Edge.Tail));
}


double CDB_Model::GetLength3D(long IndexEdge)
//Gets the length of the edge "IndexEdge"
//Considers all three coordinates (X, Y, Z)
{
	CEdge Edge= GetEdge(IndexEdge);
	return MODULE_3D(GetVertex(Edge.Head), GetVertex(Edge.Tail));
}


double CDB_Model::GetDistance2D(long NumVertex1, long NumVertex2)
//Gets the length of the segment that connects Vertex1 to Vertex2
//Considers only two coordinates (X, Y)
{
	return MODULE_2D(GetVertex(NumVertex1), GetVertex(NumVertex2));
}


double CDB_Model::GetDistance2D(CVertex Vertex1, CVertex Vertex2)
//Gets the length of the segment that connects Vertex1 to Vertex2
//Considers only two coordinates (X, Y)
{
	return MODULE_2D(Vertex1, Vertex2);
}


//double CDB_Model::GetDistance3D(long NumVertex1, long NumVertex2)
////Gets the length of the segment that connects Vertex1 to Vertex2
////Considers three coordinates (X, Y, Z)
//{
//	return MODULE_3D(GetVertex(NumVertex1), GetVertex(NumVertex2));
//}


//double CDB_Model::GetDistanceVertexEdge(long Vertex, long Edge)
////Gets the distance between the Vertex and the Edge
//{
//	CVertex A= GetVertex(Vertex);
//	CEdge Segment= GetEdge(Edge);
//	CVertex T= GetVertex(Segment.Tail);
//	CVertex H= GetVertex(Segment.Head);
//
//	return ((H.x-T.x)*(A.y-T.y) - (H.y-T.y)*(A.x-T.x)) / GetLength2D(Edge);
//}


double CDB_Model::GetAngle2D_PI_half(CVector V1, CVector V2)
//Gets the angle (in radians) between the vectors "V1" and "V2"

//returns a value in the range [0, PI/2]

//Valid in 2D (ignores z coordinate)
{
	double angle= std::fabs(atan2(V1.x*V2.y - V1.y*V2.x, V1.x*V2.x - V1.y*V2.y)); // angle in range [0, PI]

	if (angle > (NUMBER_PI/2)) angle = NUMBER_PI - angle;

	return (angle);// angle in range [0, PI/2]
}


double CDB_Model::GetAngle2D_PI_half(long numEdge1, long numEdge2)
//Gets the angle (in radians) between the edges "NumEdge1" and "NumEdge2"

//Values are obtained from DDBB database
//Z coordinates are ignored

//Since the edges are not oriented,
//returns a value in the range [0, PI/2]

//Valid in 2D
{
	CEdge E1= GetEdge(numEdge1);
	CEdge E2= GetEdge(numEdge2);

	CVector VI (GetVertex(E1.Head).x - GetVertex(E1.Tail).x,
				GetVertex(E1.Head).y - GetVertex(E1.Tail).y,
				0.);
	CVector VJ (GetVertex(E2.Head).x - GetVertex(E2.Tail).x,
				GetVertex(E2.Head).y - GetVertex(E2.Tail).y,
				0.);

	return GetAngle2D_PI_half(VI, VJ);
}


double CDB_Model::GetAngle2DX_PI(long IndexEdge, double Threshold)
//Gets the angle (in radians) between the edge "IndexEdge" and the X axis

//Values are obtained from DDBB database
//Z coordinates are ignored

//Since the edge is not oriented,
//returns a value in the range [0, PI)

//Valid in 2D
{
	CEdge Edge= GetEdge(IndexEdge);
	CVertex Vertex1= GetVertex(Edge.Tail);
	CVertex Vertex2= GetVertex(Edge.Head);

	//return GetAngle2DX_PI(Vertex1, Vertex2, Threshold);

	//assert(MODULE_2D(Vertex1, Vertex2) > 0);
	double angle= atan2(Vertex2.y-Vertex1.y, Vertex2.x-Vertex1.x);
	if(angle<0) angle+= NUMBER_PI;

	if (std::fabs(angle-NUMBER_PI) < Threshold) angle= 0.0;

	return angle;
}


double CDB_Model::GetAngle2D_PI(CVertex v1, CVertex vP, CVertex v2)

//Gets the angle (in radians) between the two segments
//defined by three consecutive 2D vertices

//vP is Pivot (common vertex) of the angle
//This means that the function returns the minimum angle between two consecutive segments

//Returns a value in the range [0, PI]

//Valid in 2D
{
	v1.x-= vP.x; v1.y-= vP.y;
	v2.x-= vP.x; v2.y-= vP.y;
	//assert(MODULE_2D(v1, v2) > 0);
	double norm_cross= v1.x*v2.y-v1.y*v2.x;
	double dot= v1.x*v2.x+v1.y*v2.y;
	return std::fabs(atan2(norm_cross, dot));
}


double CDB_Model::GetAngle2D_PI(long numVertex1, long numVertexP, long numVertex2)

//Gets the angle (in radians) between the two segments
//defined by three consecutive 2D vertices

//numVertexp is Pivot (common vertex) of the angle
//This means that the function returns the minimum angle between two consecutive segments

//Returns a value in the range [0, PI]

//Valid in 2D
{
	return (GetAngle2D_PI(GetVertex(numVertex1), GetVertex(numVertexP), GetVertex(numVertex2)));

	//CVertex v1= GetVertex(numVertex1);
	//CVertex vP= GetVertex(numVertexP);   //Pivot
	//CVertex v3= GetVertex(numVertex3);

	//v1.x-= vP.x; v1.y-= vP.y;
	//v2.x-= vP.x; v2.y-= vP.y;
	////assert(MODULE_2D(v1, v2) > 0);
	//double norm_cross= v1.x*v2.y-v1.y*v2.x;
	//double dot= v1.x*v2.x+v1.y*v2.y;
	//return std::fabs(atan2(norm_cross, dot));
}


double CDB_Model::GetAngle2DX_2PI(long numEdge, long numVertexTail, double Threshold)
//Gets the angle (in radians) between the edge "numEdge" and the X axis

//Values are obtained from DDBB database
//Z coordinates are ignored

//The edge is ORIENTED, and vertex "numVertexTail" is the tail
//Returns a value in the range [0, 2*PI-Threshold)

//Valid in 2D
{
	CEdge Edge= GetEdge(numEdge);

	CVertex H= GetVertex(Edge.Head);
	CVertex T= GetVertex(Edge.Tail);
	if(Edge.Tail != numVertexTail)
		std::swap(H, T);

	//assert(MODULE_2D(H, T) > 0);
	double angle= atan2(H.y-T.y, H.x-T.x);
	if(angle<0)
		angle+= 2*NUMBER_PI;

	if (std::fabs(angle-2*NUMBER_PI) < Threshold)
		angle= 0.0;

	return angle;
}


double CDB_Model::GetAngle2DX_2PI(CVertex Vertexorigin, CVertex Vertexend)
//Gets the angle (in radians) between the line defined by vertices
//Vertexorigin and Vertexend and the X axis

//Values are obtained from DDBB database
//Z coordinates are ignored

//The edge is ORIENTED, and vertex "Vertexorigin" is the tail

//Returns a value in the range [0, 2*PI]




//WARNING, PROBABLY THE ANGLE SHOUD BE IN THE RANGE [0, 2*PI-Threshold)



//Valid in 2D
{
	//assert(MODULE_2D(Vertexorigin, Vertexend) > 0);
	double angle= atan2(Vertexend.y-Vertexorigin.y, Vertexend.x-Vertexorigin.x); // angle in range [-PI, PI]
	if(angle<0) angle+= 2*NUMBER_PI;   //changes the range up to [0, 2*PI]
	return angle;
}


double CDB_Model::GetAngle2DX_PIPI(CVertex Vertexorigin, CVertex Vertexend)
//Gets the angle (in radians) between the line defined by vertices
//Vertexorigin and Vertexend and the X axis

//Values are obtained from DDBB database
//Z coordinates are ignored

//The edge is ORIENTED, and vertex "Vertexorigin" is the tail

//Returns a value in the range [-PI, PI]

//Valid in 2D
{
	//assert(MODULE_2D(Vertexorigin, Vertexend) > 0);
	double angle= atan2(Vertexend.y-Vertexorigin.y, Vertexend.x-Vertexorigin.x); // angle in range [-PI, PI]
	return angle;
}


double CDB_Model::GetAngle3D_PI(CVertex v1, CVertex vP, CVertex v2)
//Gets the angle (in radians) between the two segments
//defined by three consecutive 3D vertices

//NumVertexP is Pivot (common vertex) of the angle
//This means that the function returns the minimum angle between two consecutive segments

//Returns a value in the range [0, PI]

//Valid in 3D
{
	//Translate v1 and v2 (new origin is vP)
	v1.x-= vP.x; v1.y-= vP.y; v1.z-= vP.z;
	v2.x-= vP.x; v2.y-= vP.y; v2.z-= vP.z;

	double xx= (v1.y*v2.z - v1.z*v2.y);
	double yy= (v1.z*v2.x - v1.x*v2.z);
	double zz= (v1.x*v2.y - v1.y*v2.x);
	double norm_cross= sqrt(xx*xx + yy*yy + zz*zz);

	double dot= v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;

	double angle= std::fabs(atan2(norm_cross, dot)); // angle in range [0, PI]

	return (angle);// angle in range [0, PI]
}


double CDB_Model::GetAngle3D_PI_half(CVector v1, CVector v2)
//Gets the angle (in radians) between the two 3D vectors V1 and V2

//Returns a value in the range [0, PI/2]

//Valid in 3D

//Based on:
//	Press W.H., Teukolsky S.A., Vetterling W.T., Flannery B.P., 
//	Numerical Recipes: The Art of Scientific Computing 3rd Edition, 
//	Cambridge University Press, 2007. ISBN-13: 978-0521880688.
{
	double xx= (v1.y*v2.z - v1.z*v2.y);
	double yy= (v1.z*v2.x - v1.x*v2.z);
	double zz= (v1.x*v2.y - v1.y*v2.x);
	double norm_cross= sqrt(xx*xx + yy*yy + zz*zz);

	double dot= v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;

	double angle= std::fabs(atan2(norm_cross, dot)); // angle in range [0, PI]

	if (angle>(NUMBER_PI/2)) angle = NUMBER_PI - angle;

	return (angle);// angle in range [0, PI/2]
}


double CDB_Model::GetMinAngleInVertex(long iVertex)
//Returns the minimum angle between edges connected to the vertex iVertex
{
	long NumEdges= GetSizeEdgesInVertex(iVertex);

	if(NumEdges<2) return 0.;  //Unconnected vertices are returned as "zero degrees"

	//Get angles from origin
	std::vector<double> Angles;
	for (long i=0; i<NumEdges; i++){
		Angles.push_back(GetAngle2DX_2PI(GetEdgeInVertex(iVertex, i), iVertex, Accuracy));
	}
	Angles= SortMinorFirst(Angles);

	//Initialize the minimum angle between edges
	double MinAngle= std::fabs(Angles[1]-Angles[0]);
	MinAngle= std::min(MinAngle, 2*NUMBER_PI-MinAngle);

	//Loop to find the minimum angle between edges
	for (long i=1; i<NumEdges; i++){
		double MinI= std::fabs(Angles[(i+1) % NumEdges]-Angles[i]);
		MinI= std::min(MinI, 2*NUMBER_PI-MinI);
		MinAngle= std::min(MinAngle, MinI);
	}

	return MinAngle;
}


void CDB_Model::GetAngularDistribution(std::vector<double> &Angles,
									   std::vector<double> &Frequencies,
									   bool normalizeFrequencies)
//Calculates the angular distribution of the model
//	Main angles are stored in the vector "Angles"
//	Their frequencies are stored in the vector "Frequencies"

//Calculates the histogram by calculating angles and their frequencies
//Angles are sampled at intervals "delta"

//Then, main angles are obtained
//	- When an angle is added to the graph, a Gaussian distribution curve
//	  with a mean value equal to the ange and
//	  with standard deviation equal to 7 degrees (or 30% of the minimum angle)
//	  is superpositioned onto the graph
//	  to account for the inaccuracy of the sketch.
//	- After all the curves have been superpositioned and combined
//	  with their different weights, the graph is normalized with its maximum at 1.0.
//	..(If flag normalizeFrequencies is false, the frecuencies remain absolute, non-normalized)
//	- The resulting ADG qualitatively shows the prevailing angles as humps

//	- Crests of the humps must have a frequency greater than a thereshold (MinCrest)

//Based on:
//	H Lipson, M Shpitalni
//	Optimization-based reconstruction of a 3D object
//	from a single freehand line drawing
//	Computer-Aided Design, 28(8) 651-663, 1996
{
	//Adjustable parameters
	//---------------------
	//Interval of the histogram
	double delta= 1.0; //One interval per degree
	long Intervals= (long) (180/delta);

	//Minimum crest height considered
	double MinCrest= 0.10; //Crests smaller than 10% of maxADG are ignored

	//Standard deviation (used to group nearly parallel edges)
	double AcuteAngle= GetMinAngleInVertex(0);
	for (long i=1; i<GetSizeVertices(); i++){
		AcuteAngle= std::min(AcuteAngle, GetMinAngleInVertex(i));
	}
	double sigma= std::max(RADIANS_TO_DEGREES(0.30*AcuteAngle), 7.0);  //Alternative fix threshold is set to 7.o degrees

	//Histogram
	//---------
	std::vector<long> Histogram(Intervals, 0);

	//Loop over all the edges
	for (long i=0; i<GetSizeEdges(); i++){
		double Angle= RADIANS_TO_DEGREES(GetAngle2DX_PI(i, Accuracy));
		long lAngle= (long)(Angle/delta);
		Histogram[lAngle] += 1;
	}

	//Angular Distribution Graph
	//--------------------------
	std::vector<double> ADG(Intervals, 0);
	double a= 1/(sigma*sqrt(2*NUMBER_PI));
	double sigma2= 2*sigma*sigma;

	for(long i= 0; i<Intervals; i++){
		if(Histogram[i]!=0){
			long jmin= (long)(i-3*sigma);
			long jmax= (long)(i+3*sigma);
			long N= (long)ADG.size();
			for(long j= jmin; j<jmax; j++){
				double b= (j-i)*(j-i)/sigma2;
				ADG[(N+j)%N] += (a * exp(-b)) * Histogram[i];
			}
		}
	}

	double maxADG= ADG[0];
	for(long i= 1; i<Intervals; i++){
		maxADG= std::max(maxADG, ADG[i]);
	}

	if(normalizeFrequencies){
		//Normalize ADG (to get a maxADG of 1)
		for(long i= 0; i<Intervals; i++){
			ADG[i]= ADG[i]/maxADG;
		}
	}
	else{
		MinCrest*= maxADG;
	}

	//Save the humps as main angles
	//-----------------------------
	Angles.clear();
	Frequencies.clear();

	//Save as zero the possible hump that is either close to zero or close to PI
	if(ADG[0]>MinCrest && (ADG[Intervals-1]<=ADG[0] && ADG[0]>ADG[1]) ||
		ADG[Intervals-1]>MinCrest && (ADG[Intervals-2]<=ADG[Intervals-1] && ADG[Intervals-1]>ADG[0])){
			Angles.push_back(0);
			Frequencies.push_back(std::max(ADG[0], ADG[Intervals-1]));
	}

	//Loop over the remainder humps
	for(long i= 1; i<Intervals-1; i++){
		if(ADG[i]>MinCrest && (ADG[i-1]<=ADG[i] && ADG[i]>ADG[i+1])){ //If changes from increasing or stable to decreasing
			Angles.push_back(DEGREES_TO_RADIANS(i*delta));
			Frequencies.push_back(ADG[i]);
			i+= 1;  //To prevent consecutive false humps due to roundoffs
		}
	}
}


void CDB_Model::SetVertex(long Position, CVertex const &Vertex)
{
	CVertices.SetVertex(Position, Vertex);
}


void CDB_Model::SetEdge(long Position, CEdge const &Edge)
{
	CEdges.SetEdge(Position, Edge);
}


void CDB_Model::SetFace(long Position, CFace const &Face)
{
	CFaces.SetFace(Position, Face);
}


void CDB_Model::SetFaceAsPolygon(long numFace, std::vector<POINT2D> &FacePolygon)
//Outputs as a vector of POINT2D the vertices of face numFace

//Using POINT2D only makes sense for "flat" faces, previous to any inflation,
//as z coordinates are ignored
{
	std::vector<long> vertices= GetAllVerticesInFace(GetFace(numFace));
	for(long i=0; i<(long)vertices.size(); i++){
		CVertex v= GetVertex(vertices[i]);
		POINT2D p(v.x, v.y);
		FacePolygon.push_back(p);
	}
}


void CDB_Model::SetAllVertex_XY(std::vector<double> XY)
{
	long j=0;
	for( long i=0; i<GetSizeVertices(); i++ ) {
		CVertex Vertice= GetVertex(i);
		Vertice.x = XY[j];
		//assert( !_isnan(Vertice.x));
		j++;
		Vertice.y = XY[j];
		//assert( !_isnan(Vertice.y));
		j++;
		SetVertex(i, Vertice);
	}
}


void CDB_Model::SetAllVertex_Z(std::vector<double> Z)
{
	assert(Z.size()==GetSizeVertices());

	for(long i=0; i<GetSizeVertices(); i++ ) {
		CVertex Vertice= GetVertex(i);
		Vertice.z = Z[i];
		//assert(!_isnan(Vertice.z));
		SetVertex(i, Vertice);
	}
}


void CDB_Model::SetAllVertex_Z_ToZero()
{
	for(long i=0; i<GetSizeVertices(); i++ ) {
		CVertex Vertice= GetVertex(i);
		Vertice.z = 0.0;
		SetVertex(i, Vertice);
	}
}


void CDB_Model::RemoveVertex(long Position, bool updateEdges)
//Removes the vertex "Position"

//If the flag "update" is true, edges connected to
//vertices subsequent to Position are renumbered
//(This is the default, to prevent inconsistencies in the database)
{
	CVertices.RemoveVertex(Position);

	if (updateEdges){
		//Vertices of edges connected to vertices subsequent to Position are renumbered
		for (long k=0; k<GetSizeEdges(); k++){
			CEdge edge= GetEdge(k);
			if (edge.Tail > Position){
				edge.Tail--;
				SetEdge(k, edge);
			}
			if (edge.Head > Position){
				edge.Head--;
				SetEdge(k, edge);
			}
		}
	}
}


void CDB_Model::RemoveEdge(long Position, bool clearVertices)
//Removes the edge "Position"

//If the flag "clearVertices" is true, vertices no longer connected to any other edges are removed
{
	if (clearVertices){
		//Vertices no longer connected to any other edges are removed
		CEdge Arista= GetEdge(Position);
		long last= std::min(Arista.Tail, Arista.Head);
		long first= std::max(Arista.Tail, Arista.Head);
			if(GetSizeEdgesInVertex(first) <= 1)
				RemoveVertex(first, true);
			if(GetSizeEdgesInVertex(last) <= 1)
				RemoveVertex(last, true);
	}

	//Remove the edge from the list of edges
	CEdges.RemoveEdge(Position);
}



void CDB_Model::RemoveFace(long Position)
//Removes the face "Position" from the collection of faces
{
	CFaces.RemoveFace(Position);
}


void CDB_Model::MergeVertices (long MainVertex, long VertexToMerge)
//Reconects to vertex MainVertex
//all the edges connected to vertex VertexToMerge,
//then removes vertex VertexToMerge

//The merit of the new vertex
//is the minimum of both merged vertices
{
	//Update merit ofv vertex MainVertex
	CVertex vertex1= GetVertex(MainVertex);
	CVertex vertex2= GetVertex(VertexToMerge);
	vertex1.merit= std::min(vertex1.merit, vertex2.merit);
	SetVertex(MainVertex, vertex1);

	//Re-connect to MainVertex all thev edges connected to VertexToMerge
	std::vector<long> edgesInVertex= GetAllEdgesInVertex(VertexToMerge);
	for (long i=0; i<(long)edgesInVertex.size(); i++){
		//Recalculate the vertex to merge
		CEdge edge= GetEdge(edgesInVertex[i]);

		if (edge.Head == VertexToMerge){
			edge.Head = MainVertex;
		}
		else if (edge.Tail == VertexToMerge){
			edge.Tail = MainVertex;
		}

		//Update the merged vertex
		SetEdge(edgesInVertex[i], edge);
	}

	//Delete VertexToMerge (and update the edges)
	RemoveVertex(VertexToMerge, true);
}


//Used by HiddenEdges-------------

CVerticesSet CDB_Model::GetSetOfVertices()
//Returns an external copy of the current collection of vertices
{
	return CVertices;
}


CEdgesSet CDB_Model::GetSetOfEdges()
//Returns an external copy of the current collection of edges
{
	return CEdges;
}


void CDB_Model::UpdateSetOfVertices(CVerticesSet inVertices)
//Updates the collection of vertices of the database 
//with and external collection "inVertices"

//WARNING: Integrity of the vertices is not checked!
{
	CVertices= inVertices;
}


void CDB_Model::UpdateSetOfEdges(CEdgesSet inEdges)
//Updates the collection of edges of the database 
//with and external collection "inEdges"

//WARNING: Integrity of the edges is not checked!
{
	CEdges= inEdges;
}
//End used by HiddenEdges---------
