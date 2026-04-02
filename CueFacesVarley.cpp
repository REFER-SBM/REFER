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

//Get faces from a graph-like representation of a polyhedrical shape

//Based on:
	//Varley P.A.C. and Company P.
	//A new algorithm for finding faces in wireframes.
	//Computer-Aided Design 42(4), 279-309

#include <stdafx.h>
#include <algorithm>  //sort
#include <cassert>
#include <cmath>

//-------used to import/export data from/to the external database-----------
#include <vector>

#include "Tools_Geometry.h"
#include "DB_Model.h"
#include "DB_CSG.h"
#include "Refer.h"
#include "resource.h"
#include "ReferDoc.h"
//--------------------------------------------------------------------------

#include "CueFacesV_Geometry.h"
#include "CueFacesV_Nuts.h"

#include "CueFacesVarley.h"

// Output data (Defined in CFacesVarley, but used too in STRINGS)
int FaceCount;
std::vector <FACE> Face;


/* -------------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------------- */
STRING::STRING()
{
	Sta= Fin= Pri= Mic= 0;
};


STRING::STRING(const STRING &S)
:
Sta (S.Sta),
Fin (S.Fin),
Pri (S.Pri),
Mic (S.Mic),
Mid (S.Mid)
{
};


STRING::~STRING()
{
};


int STRING::VertexInString(int Index)
{
	if (!Index)  return Sta;
	if (Index > Mic)  return Fin;
	return Mid[Index-1];
}


int STRING::TripleExistsInString(int U, int V, int W)
{
	for (int i=0; i<Mic; ++i)
	{
		if ((VertexInString(i) == U) && (VertexInString(i+1) == V) && (VertexInString(i+2) == W))
		{
			return 1;
		}
	}

	return 0;
}


/* -------------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------------- */


THROUGH::THROUGH()
{
	Sta= Mid= Fin= Use= 0;
};


THROUGH::THROUGH(const THROUGH &T)
:
Sta (T.Sta),
Mid (T.Mid),
Fin (T.Fin),
Use (T.Use)
{
};


THROUGH::~THROUGH()
{
};


/* -------------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------------- */


STRINGS::STRINGS()
{
	Count= 0;
};


STRINGS::STRINGS(const STRINGS &S)
:
Count (S.Count),
Strings (S.Strings),
ThroughCount (S.ThroughCount)
{
	for(long i= 0; i< (long)Throughs.size(); i++){
		for(long j= 0; j< (long)Throughs[0].size(); j++){
			Throughs[i][j]= S.Throughs[i][j];
		}
	}
};


STRINGS::~STRINGS()
{
};


bool CompareStrings(STRING A, STRING B)
// AUXILIAR for SortStrings
//Returns true if string A has got higest priority than string B
{
	return (A.Pri > B.Pri);
}


void STRINGS::SortStrings()
{
	sort(Strings.begin(), Strings.end(), CompareStrings);
}


void STRINGS::DeleteThrough (int V, int T)
{
	--(ThroughCount[V]);
	Throughs[V][T] = Throughs[V][ThroughCount[V]];
}


void STRINGS::MergeThroughs(int V, int U, int W)
{
	for (int i=0;  i<ThroughCount[V];  ++i)
	{
		if (permute2(Throughs[V][i].Sta,Throughs[V][i].Fin,U,W))
		{
			Throughs[V][i].Use -= 2;
		}
		else
		{
			int C = common2of4(Throughs[V][i].Sta,Throughs[V][i].Fin,U,W,-1);
			if (C >= 0)
			{
				Throughs[V][i].Use -= 1;
			}
		}
	}

	for (int i=ThroughCount[V]-1;  i>=0;  --i)
	{
		if (!Throughs[V][i].Use)  DeleteThrough(V,i);
	}
}


int STRINGS::ThroughsMustBeMerged(int V, int U, int W)
{
	for (int i=0;  i<ThroughCount[V];  ++i)
	{
		if (permute2(Throughs[V][i].Sta,Throughs[V][i].Fin,U,W))
		{
			return (Throughs[V][i].Use == 2);
		}
	}

	return 0;
}


void STRINGS::DeleteString(int T, bool Sortnow)
{
	--(Count);
	Strings[T] = Strings[Count];
	Strings.pop_back();
	if (Sortnow) SortStrings();
}


bool STRINGS::MergeStrings(int S, int T, bool Sortnow, double Devalue)
{
	if (Strings[S].Fin != Strings[T].Sta)
	{
		AfxMessageBox("Error merging strings");
		return false;
	}

	if (!StringsCanBeMerged(S,T))
	{
		AfxMessageBox("Error merging strings: string cannot merge");
		return false;
	}

	MergeThroughs(Strings[S].Fin,
				  Strings[S].VertexInString(Strings[S].Mic),
				  Strings[T].VertexInString(1));

	Strings[S].Fin = Strings[T].Fin;
	Strings[S].Pri += Strings[T].Pri;

	Strings[S].Mid.push_back(Strings[T].Sta);

	for (int i=0;  i<Strings[T].Mic;  ++i)
	{
		Strings[S].Mid.push_back(Strings[T].Mid[i]);
	}

	Strings[S].Mic += 1 + Strings[T].Mic;

	Strings[S].Pri *= (int)Devalue;  // give someone else a turn

	DeleteString(T, Sortnow);

	return true;
}


int STRINGS::FindStringByVertices(int E, int F)
{
	for (int i=0;  i<Count;  ++i)
	{
		if (Strings[i].Sta == E)
		{
			if (Strings[i].Mic)
			{
				if (Strings[i].Mid[0] == F)  return i;
			}
			else
			{
				if (Strings[i].Fin == F)  return i;
			}
		}
	}
	return -1;
}


int STRINGS::ThroughsCanBeMerged (int V, int U, int W)
{
	for (int i=0;  i<ThroughCount[V];  ++i)
	{
		if (permute2(Throughs[V][i].Sta,Throughs[V][i].Fin,U,W))
		{
			return (Throughs[V][i].Use >= 2);
		}
	}

	return 1;
}


bool STRINGS::TripleExistsInAnyString (int U, int V, int W)
{
	for (int S=0; S<Count; ++S)
	{
		if (Strings[S].Mic)
		{
			if (Strings[S].TripleExistsInString(U,V,W))  return true;
		}
	}

	return false;
}


bool STRINGS::TripleExistsInAnyFace (int U, int V, int W)
{
	for (int F=0; F<FaceCount; ++F)
	{
		int N = Face[F].C;
		for (int i=0; i<N; ++i){
			if ((Face[F].V[i] == U) && (Face[F].V[(i+1)%N] == V) && (Face[F].V[(i+2)%N] == W))  return true;
		}
	}
	return false;
}


bool STRINGS::StringsCanBeMerged(int T, int S)
//Two strings may be merged to form a larger string if the
//last vertex in one is the first vertex in another, provided that:
//		(a) no other vertex appears in both strings, and
//		(b) the new triple of three consecutive vertices formed by
//			the concatenation does not already appear in reverse order 
//			in an existing face or already-concatenated string
{
	int U,V,W;

	if ((!(Strings[S].Mic)) && (!(Strings[T].Mic)))
	{
		if (Strings[S].Fin == Strings[T].Sta) return false;
		U = Strings[T].Sta;
		V = Strings[T].Fin;
		W = Strings[S].Fin;
	}

	if (Strings[S].Mic)
	{
		for (int i=0;  i<Strings[S].Mic;  ++i)
		{
			if (Strings[S].Mid[i] == Strings[T].Sta)  return false;
			if (Strings[S].Mid[i] == Strings[T].Fin)  return false;
		}
		U = Strings[T].Sta;
		V = Strings[T].Fin;
		W = Strings[S].Mid[0];
	}

	if (Strings[T].Mic)
	{
		for (int i=0;  i<Strings[T].Mic;  ++i)
		{
			if (Strings[T].Mid[i] == Strings[S].Sta)  return false;
			if (Strings[T].Mid[i] == Strings[S].Fin)  return false;
		}
		U = Strings[T].Mid[Strings[T].Mic-1];
		V = Strings[T].Fin;
		W = Strings[S].Fin;
	}

	if (Strings[S].Mic && Strings[T].Mic)
	{
		for (int i=0;  i<Strings[S].Mic;  ++i)
		for (int j=0;  j<Strings[T].Mic;  ++j)
		{
			if (Strings[S].Mid[i] == Strings[T].Mid[j])  return false;
		}
		U = Strings[T].Mid[Strings[T].Mic-1];
		V = Strings[T].Fin;
		W = Strings[S].Mid[0];
	}

	if (!ThroughsCanBeMerged(V,U,W))  return false;

	// At this point, we check whether
	// the vertex triple produced by the merger
	// appears in reverse order in an existing face
	// or existing hypothesised or accepted string
	if (TripleExistsInAnyFace(U,V,W))  return false;
	if (TripleExistsInAnyString(W,V,U))  return false;

	return true;
}


/* -------------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------------- */


void CFacesVarley::DetermineExtrema()
{
	LeftVertex = RightVertex = TopVertex = BottomVertex = 0;

	for (int j = 1;  j < VertexCount;  ++j)
	{
		if (VertexX[j] < VertexX[LeftVertex])  LeftVertex = j;
		if (VertexX[j] > VertexX[RightVertex])  RightVertex = j;
		if (VertexY[j] < VertexY[TopVertex])  TopVertex = j;
		if (VertexY[j] > VertexY[BottomVertex])  BottomVertex = j;
	}
}


void CFacesVarley::DetermineSubgraphs()
//Break down the wireframe into subgraphs
{
	SubgraphForVertex.assign(VertexCount, 0);
	SubgraphForEdge.assign(EdgeCount, 0);

	int SubgraphCount = 0;
	int OuterSubgraph = 0;

	//Identify subgraphs, until no more vertices remain unassigned
	for (;;)
	{
		// identify an unassigned vertex
		int V = -1;
		for (int i=0; i<VertexCount; ++i)  if (!SubgraphForVertex[i])
		{
			V = i;
			break;
		}

		if (V < 0)  break;  // no more unassigned vertices
		++SubgraphCount;
		SubgraphForVertex[V] = SubgraphCount;

		for (;;)
		{
			bool Progress = false;

			for (int i=0; i<EdgeCount; ++i)  if (SubgraphForEdge[i] == 0)
			{
				if (SubgraphForVertex[EdgeU[i]] == SubgraphCount)
				{
					SubgraphForEdge[i] = SubgraphCount;
					SubgraphForVertex[EdgeV[i]] = SubgraphCount;
					Progress = true;
				}
				else
				if (SubgraphForVertex[EdgeV[i]] == SubgraphCount)
				{
					SubgraphForEdge[i] = SubgraphCount;
					SubgraphForVertex[EdgeU[i]] = SubgraphCount;
					Progress = true;
				}
			}
			if (!Progress)  break;
		}
	}

	//Get the outer subgraph
	DetermineExtrema();
	OuterSubgraph = majority4(SubgraphForVertex[BottomVertex],
							  SubgraphForVertex[LeftVertex],
							  SubgraphForVertex[RightVertex],
							  SubgraphForVertex[TopVertex]);
}


void CFacesVarley::GetImplicitData()
//Obtains data, implicit in the input data, required to detect faces:
//		- subgraphs
//		- valences of vertices (number of edges connected to every vertex)
{
	DetermineSubgraphs();

	//Update the number of edges connected to every vertex
	EdgeCountForVertex.assign(VertexCount, 0);
	for (long i=0; i<EdgeCount; ++i){
		++EdgeCountForVertex[EdgeU[i]];
		++EdgeCountForVertex[EdgeV[i]];
	}
}


DEGREES CFacesVarley::AngleFromVertexToVertex (int U, int V)
{
	DEGREES Angle = (180.0/M_PI) * atan2(VertexX[V]-VertexX[U],VertexY[U]-VertexY[V]);

	// Make the angle always positive
	while (Angle < 0)  Angle += 360.0;

	return Angle;
}


int CFacesVarley::EdgeJoining (int U, int V)
//Returns the number of the edge that connects vertices U and V.

//Returns -1 if they are not connected
{
	if (U == V)  return -1;
	if (SubgraphForVertex[U] != SubgraphForVertex[V])  return -1;

	for (int i=0;  i<EdgeCount;  ++i)
	{
		if ((EdgeU[i] == U) && (EdgeV[i] == V))  return i;
		if ((EdgeU[i] == V) && (EdgeV[i] == U))  return i;
	}
	return -1;
}


int CFacesVarley::UsedInAQuadrilateralLoop (int E)
//Returns an increment if edge E belongs to, at least, one quadrilateral loop

//The increment is:
//	1*InitialPriorityQuad if edge belongs to the quadrilateral loop
//						  but is not parallel to its opposite edge
//	2*InitialPriorityQuad if edge belongs to the quadrilateral loop
//						  and is parallel to its opposite edge
{
	int increment= 0;
	for (int i=0;  i<QuadrilateralLoopCount;  ++i)
	{
		if (QuadrilateralLoop[i].E == E) increment= max(2,increment);
		if (QuadrilateralLoop[i].F == E) increment= max(2,increment);
		if (QuadrilateralLoop[i].G == E) increment= max(1,increment);
		if (QuadrilateralLoop[i].H == E) increment= max(1,increment);
	}
	return increment;
}


int CFacesVarley::EdgeOtherEnd (int E, int V)
//Returns the other vertex (different from V), in edge E

//Returns -1 if vertex V is not in edge E
{
	if (EdgeU[E] == V)  return EdgeV[E];
	if (EdgeV[E] == V)  return EdgeU[E];
	return -1;
}


int CFacesVarley::CommonVertex (int E, int F)
//Returns the number of the vertex common to edges E and F

//Returns -1 if there is no common vertex
{
	if (EdgeU[E] == EdgeU[F])  return EdgeU[E];
	if (EdgeU[E] == EdgeV[F])  return EdgeU[E];
	if (EdgeV[E] == EdgeU[F])  return EdgeV[E];
	if (EdgeV[E] == EdgeV[F])  return EdgeV[E];
	return -1;
}


bool CFacesVarley::UsedInATriangularLoop (int E)
//Returns true if edge E is used in any triangular loop
//Returns false otherwise
{
	for (int i=0;  i<TriangularLoopCount;  ++i)
	{
		if (TriangularLoop[i].E == E)  return true;
		if (TriangularLoop[i].F == E)  return true;
		if (TriangularLoop[i].G == E)  return true;
	}
	return false;
}
//
//
//bool CFacesVarley::AllOtherEdgesAreTheSameSide (int V, int E, int F)
//// TDB
//{
//	return false;  // work on this later
//}


int CFacesVarley::TotalVertexPriority (int V)
//Returns the current priority of vertex V
{
	int Total = 0;

	for (int i=0;  i<Master.Count;  ++i)
	{
		if ((Master.Strings[i].Sta == V) || (Master.Strings[i].Fin == V))
			Total += Master.Strings[i].Pri;
	}

	return Total;
}


int CFacesVarley::FindAStringStarting (int V)
//Find a string starting at vertex V
{
	for (int i=0;  i<Master.Count;  ++i)
	{
		if (Master.Strings[i].Sta == V)  return i;
	}

	return -1;
}


int CFacesVarley::FindAStringFinishing (int V, int W)
//Find a string finishing at vertex V, and NOT starting at W
{
	for (int i=0;  i<Master.Count;  ++i)
	{
		if ((Master.Strings[i].Fin == V) && (Master.Strings[i].Sta != W))  return i;
	}

	return -1;
}


void CFacesVarley::MakeAFaceFromAnyTriangle ()
{
	for (int i=0;  i<Master.Count;  ++i)
	{
		int A = Master.Strings[i].Sta;
		int B = Master.Strings[i].Fin;

		for (int j=i+1;  j<Master.Count;  ++j)  if (Master.Strings[j].Sta == B)
		{
			int C = Master.Strings[j].Fin;

			for (int k=i+1;  k<Master.Count;  ++k)
			{
				if ((C == Master.Strings[k].Sta) && (Master.Strings[k].Fin == A))
				{
					// found one
					FACE NewFace;
					NewFace.C= 3;
					NewFace.V.push_back(A);
					NewFace.V.push_back(B);
					NewFace.V.push_back(C);
					NewFace.E.push_back(EdgeJoining(A,B));
					NewFace.E.push_back(EdgeJoining(B,C));
					NewFace.E.push_back(EdgeJoining(C,A));
					Face.push_back(NewFace);
					FaceCount++;

					Master.DeleteString(i, false);
					Master.DeleteString(j, false);
					Master.DeleteString(k, true);
					return;
				}
			}
		}
	}
}


void CFacesVarley::CreateAFaceFromStrings(STRINGS *Strings, int S, int T)
//The vertices of F are (in sequence):
//	the start vertex of T; 
//	the intermediate vertices (if any) of T;
//	the start vertex of S, and
//	the intermediate vertices (if any) of S

//The face F is created and the two strings S and T are deleted
{
	STRING *sS = &Strings->Strings[S];
	STRING *sT = &Strings->Strings[T];

	Strings->MergeThroughs(sS->Fin,sS->VertexInString(sS->Mic),sT->VertexInString(1));
	Strings->MergeThroughs(sT->Fin,sT->VertexInString(sT->Mic),sS->VertexInString(1));

	FACE NewFace;
	NewFace.C=  sT->Mic + sS->Mic+2;
	NewFace.V.push_back(sT->Sta);
	for (int i=0;  i<sT->Mic;  ++i)
		NewFace.V.push_back(sT->Mid[i]);
	NewFace.V.push_back(sS->Sta);
	for (int i=0;  i<sS->Mic;  ++i)
		NewFace.V.push_back(sS->Mid[i]);

	//The face F is created and the two strings S and T are deleted
	Face.push_back(NewFace);
	int Nface = FaceCount++;

	for (int i=0;  i<Face[Nface].C;  ++i)
	{
		int V = Face[Nface].V[i];
		int W = Face[Nface].V[(i+1)%Face[Nface].C];
		Face[Nface].E.push_back(EdgeJoining(V,W));

		int St = Master.FindStringByVertices(V,W);
		if (St >= 0)  Master.DeleteString(St, false);
	}

	Master.SortStrings();
}


VECTOR3D* CFacesVarley::StringEdgeVector3D (VECTOR3D *VS, STRING *S, int n)
{
	int U = n ? S->Mid[n-1]:S->Sta;
	int V = (n < S->Mic) ? S->Mid[n] : S->Fin;
	VS->x = VertexX[V]-VertexX[U];
	VS->y = VertexY[V]-VertexY[U];
	//VS->z = VertexZ[V]-VertexZ[U];
	VS->z = 0;
	Normalise(VS);
	return VS;
}


DEGREES CFacesVarley::StringEdgeAngle2D (STRING *S, int n)
//Returns the angle between the n-th edge in string S and the horizontal

// Oddly enough, it is faster to calculate angles as required than to calculate them
// initially and copy the lot every time we take a copy of the master list
{
	int U = n ? S->Mid[n-1]:S->Sta;
	int V = (n < S->Mic) ? S->Mid[n] : S->Fin;
	return AngleFromVertexToVertex(U,V);
}


double CFacesVarley::FoMAnglesParallel2D (DEGREES A, DEGREES B)
// Function to determine how close two lines are to being parallel
// in 2D
{
	return pow(std::fabs(cos(dangleclock(A,B)*M_PI/180.0)),tcMp);
}


double CFacesVarley::FoMParallel3D (VECTOR3D *V1, VECTOR3D *V2)
// Function to determine how close two lines are to being parallel
// in 3D (when z coordinates are availeble)
{
	return pow(std::fabs(DotProduct(V1,V2)),tcMp);
}


double CFacesVarley::EdgeMatingValue (STRING *S, int i, STRING *T, int j)
//Returns a "mating value" measuring how close two edges are to being parallel
//(i-th edge in string S and j-th edge in string T)
{
	if (Inflated)
	{
		VECTOR3D VS,VT;
		return FoMParallel3D(StringEdgeVector3D(&VS,S,i),StringEdgeVector3D(&VT,T,j));
	}
	else
	{
		return FoMAnglesParallel2D(StringEdgeAngle2D(S,i),StringEdgeAngle2D(T,j));
	}
}


bool CompareDouble(double A, double B)
//AUXILIAR FOR StringMatingValue
//Returns true if double A goes before double B
{
	return (A > B);
}


double CFacesVarley::StringMatingValue(STRING *S, STRING *T)
//Returns a Mating Value, which reflects how close strings S and T are to being coplanar

//We use a value in the range 0 to 1, where
//0 is "bad" and 1 is "perfect"
{
	if (S->Mic || T->Mic)
	{
		//Calculate edge merits
		//(distinguishing between 2D and 3D)
		std::vector<double> EdgeMerits;
		int MeritCount = 0;

		for (int i=0;  i<=S->Mic;  ++i)
		for (int j=0;  j<=T->Mic;  ++j)
		{
			EdgeMerits.push_back(EdgeMatingValue(S,i,T,j));
			MeritCount++;
		}

		//We discard the lowest half of the edge merit results,
		//calculate the mean of what remains,
		//and use this as the mating value
		sort(EdgeMerits.begin(), EdgeMerits.end(), CompareDouble);

		MeritCount = (MeritCount+1)/2;

		for (int i=1;  i<MeritCount;  ++i)  EdgeMerits[0] += EdgeMerits[i];
		return EdgeMerits[0] / ((double)MeritCount);
	}
	else
	//Planarity is guaranteed, since we just have one edge in each string
	{
		return 1.0;
	}
}


void CFacesVarley::ListExternalTriangularLoops()
//We find all triangular loops of edges,
//by considering each edge UV
//and each vertex W which is neither U nor V.
//If edges UW and VW also exist,
//the three edges touch a triangular loop

//All of them must belong to the same subgraph.
{
	TriangularLoopCount = 0;
	TriangularLoop.clear();

	for (int i=0;  i<EdgeCount;  ++i)
	{
		int Subgraph = SubgraphForEdge[i];

		for (int j=0;  j<VertexCount;  ++j)  if (SubgraphForVertex[j] == Subgraph)
		{
			int f = EdgeJoining(EdgeU[i],j);
			int g = EdgeJoining(EdgeV[i],j);
			if ((f > i) && (g > i))
			{
				int U3 = (EdgeCountForVertex[EdgeU[i]] == 3);
				int V3 = (EdgeCountForVertex[EdgeV[i]] == 3);

				if (U3 || V3 || (EdgeCountForVertex[j] == 3))
				{
					TRIANGULAR_LOOP TL;
					TL.E= i;
					TL.F= f;
					TL.G= g;
					TL.Vtotal= EdgeU[i] + EdgeV[i] + j;
					TriangularLoop.push_back(TL);

					++TriangularLoopCount;
				}
			}
		}
	}
}


void CFacesVarley::ListParallelQuadrilateralLoops()
//We find all quadrilateral loops of edges

//First pair of edges(E and F) are parallel to each other
//Second pair (G and H) is not guaranteed to be parallel to each other

//Hence, quadrilateral faces with two pairs of opposed parallel edges are listed twice
{
	//Tuneable constant, anywhere between 0.6 and 0.995 seems OK
	double PARALLEL_THRESHOLD= 0.9;

	QuadrilateralLoopCount = 0;
	QuadrilateralLoop.clear();

	for (int i=0;  i<EdgeCount;  ++i)
	{
		for (int j=i+1;  j<EdgeCount;  ++j)  if (SubgraphForEdge[j] == SubgraphForEdge[i])
		{
			if (AllDifferent(EdgeU[i],EdgeV[i],EdgeU[j],EdgeV[j]))
			{
				DEGREES Di = AngleFromVertexToVertex(EdgeU[i],EdgeV[i]);
				DEGREES Dj = AngleFromVertexToVertex(EdgeU[j],EdgeV[j]);
				double Mll = FoMAnglesParallel2D(Di,Dj);
				if (Mll > PARALLEL_THRESHOLD)
				{
					int f = EdgeJoining(EdgeU[i],EdgeU[j]);
					int g = EdgeJoining(EdgeV[i],EdgeV[j]);
					if ((f >= 0) && (g >= 0))
					{
						QUADRILATERAL_LOOP QL;
						QL.E= i;
						QL.F= j;
						QL.G= f;
						QL.H= g;
						QuadrilateralLoop.push_back(QL);
						++QuadrilateralLoopCount;
					}
					else
					{
						f = EdgeJoining(EdgeU[i],EdgeV[j]);
						g = EdgeJoining(EdgeV[i],EdgeU[j]);
						if ((f >= 0) && (g >= 0))
						{
							QUADRILATERAL_LOOP QL;
							QL.E= i;
							QL.F= j;
							QL.G= f;
							QL.H= g;
							QuadrilateralLoop.push_back(QL);
							++QuadrilateralLoopCount;
						}
					}
				}
			}
		}
	}
}


void CFacesVarley::IdentifyThroughVertices()
//Identifies tetrahedral K-vertex or pentahedral extended-K vertex
//They are vertices where four of five edges met and two of them are coolinear

//Angle between collinear edges must be less than MIN_ANGLE

//Higher-order extended-K vertices (hexahedral and above) are not through vertices,
//unless all of the other edges at the vertex are the same side of the through edge
{
	double MIN_ANGLE= 1.0;  // one degree, tuneable constant

	//Resize local vectors used to store Throughs
	Master.ThroughCount.assign(VertexCount, 0);

	THROUGH Tnull;
	Master.Throughs.resize(VertexCount);
	for(long i= 0; i< VertexCount; i++){
		Master.Throughs[i].assign(4, Tnull);
	}

	//Add flags to all vertices where collinear edges met
	for (int i=0;  i<EdgeCount;  ++i)  for (int j=i+1;  j<EdgeCount;  ++j)
	{
		int V = CommonVertex(i,j);
		if (V >= 0)
		{
			if ((EdgeCountForVertex[V] == 4) || (EdgeCountForVertex[V] == 5))
			//if ((EdgeCountForVertex[V] == 4) || (EdgeCountForVertex[V] == 5)  ||
			//	((EdgeCountForVertex[V] >= 6) && AllOtherEdgesAreTheSameSide(V,i,j)))
			{
				DEGREES Di = AngleFromVertexToVertex(EdgeOtherEnd(i,V),V);
				DEGREES Dj = AngleFromVertexToVertex(V,EdgeOtherEnd(j,V));
				if (dangleclock(Di,Dj) < MIN_ANGLE)
				{
					int n = Master.ThroughCount[V]++;
					Master.Throughs[V][n].Sta = EdgeOtherEnd(i,V);
					Master.Throughs[V][n].Fin = EdgeOtherEnd(j,V);
					Master.Throughs[V][n].Mid = V;
					Master.Throughs[V][n].Use = 3;
				}
			}
		}
	}
}


void CFacesVarley::CreateInitialStrings()
//Create initial Master list
//Every half edge definer one initial string

//We allocate higher initial priorities to edges which touch
//triangular or quadrilateral loops
{
	int INITIAL_PRIORITY= 100;

	//Resize Strings
	STRING Snull;
	Master.Strings.assign(EdgeCount*2, Snull);

	//Loop over all edges
	for (int i=0;  i<EdgeCount;  ++i)
	{
		//Calculate edge priority
		int Prior = INITIAL_PRIORITY;
		if (UsedInATriangularLoop(i))  Prior += InitialPriorityTri;
		else Prior += InitialPriorityQuad * UsedInAQuadrilateralLoop(i);

		//Add current edge as a string to the master list
		Master.Strings[i*2].Sta = Master.Strings[i*2+1].Fin = EdgeU[i];
		Master.Strings[i*2].Fin = Master.Strings[i*2+1].Sta = EdgeV[i];
		Master.Strings[i*2].Pri = Master.Strings[i*2+1].Pri = Prior;
		Master.Strings[i*2].Mic = Master.Strings[i*2+1].Mic = 0;
	}
	Master.Count = EdgeCount*2;

	Master.SortStrings();
}


int CFacesVarley::ChooseSeedVertex()
//Returns the trihedral vertex with the highest priority

// The seed vertex must always be trihedral
{
	int SeedVertex = -1;
	int TotalSeedPriority = 0;

	for (int i=0;  i<VertexCount;  ++i)  if (EdgeCountForVertex[i] == 3)
	{
		int TotalPriority = TotalVertexPriority(i);
		if (TotalPriority > TotalSeedPriority)
		{
			SeedVertex = i;
			TotalSeedPriority = TotalPriority;
		}
	}
	return SeedVertex;
}


void CFacesVarley::MakeFirstMove(int SeedVertex)
//We start by trying to concatenate one string terminating at seed junction with another
//starting at seed junction.

//Note that it is only at trihedral junctions
//that we can be sure that the concatenated string
//is part of a true face rather than an internal face

//If no trhiedral juntion was found, we try to find a triangular face
{
	if (SeedVertex >= 0)
	{
		int SeedS = FindAStringStarting(SeedVertex);
		int SeedT = FindAStringFinishing(SeedVertex,Master.Strings[SeedS].Fin);
		Master.MergeStrings(SeedT,SeedS, true, 1.0);
	}
	else  // no trihedral vertices, pick a triangle and hope for the best
	{
		MakeAFaceFromAnyTriangle();
	}
}


int CFacesVarley::ForcedMove(STRINGS *Strings)
//Consider a string T terminating at vertex V.
//Enumerate the set (S) of strings starting at V which can legitimately be
//concatenated/merged with T.
//If there is only one string in this set,
//the concatenation/merger must be performed.
//(If there are no strings in this set, this is an error condition)

//TDB: this must be adapted to ensure that one pair of half-edges in a through-vertex is merged
{
	for (int T=0;  T<Strings->Count; ++T)
	{
		STRING *sT = &Strings->Strings[T];

		int Count = 0;
		int UniqueS = -1;

		for (int S=0;  S<Strings->Count;  ++S)  if (S != T)
		{
			if (sT->Fin == Strings->Strings[S].Sta)
			{
				if (Strings->ThroughsMustBeMerged(sT->Fin,sT->VertexInString(sT->Mic),
												  Strings->Strings[S].VertexInString(1)))
				{
					Count = 1;
					UniqueS = S;
					break;
				}

				if (Strings->StringsCanBeMerged(T,S))
				{
					++Count;
					UniqueS = S;
				}
			}
		}

		if (Count == 1)
		{
			if (Strings->Strings[UniqueS].Fin == sT->Sta)
			{
				CreateAFaceFromStrings(Strings,T,UniqueS);
				return 2;
			}

			if (!Strings->MergeStrings(T, UniqueS, true, 1.0))  return -1;
			return 1;
		}
	}

	return 0;
}


bool CFacesVarley::MergeArbitraryStrings()
//Returns true if a face is found, false otherwise

//Repeats the followign process until a face is found
//or an error happens:
//		Find the string that best matches with the top priority string
//		If both string cannot merge, return false
//		Do all forced moves
//		Search for faces resulting from the forced moves
{
	STRINGS Working;
	Working= Master;

	for (;;)
	{
		// find the best match for the top-priority string
		int BestMatch = -1;
		double BestValue = 0.0;

		for (int i=1;  i<Working.Count;  ++i)
		{
			if (((Working.Strings[i].Sta == Working.Strings[0].Fin) && Working.StringsCanBeMerged(0,i)) ||
				((Working.Strings[0].Sta == Working.Strings[i].Fin) && Working.StringsCanBeMerged(i,0)))
			{
				double Value = StringMatingValue(&Working.Strings[0],&Working.Strings[i]);
				if (Value >= BestValue)
				{
					BestMatch = i;
					BestValue = Value;
				}
			}
		}
		//assert(BestMatch>0 && BestMatch<Working.Count);
		if(BestMatch<=0 || BestMatch>=Working.Count) return false;

		if (Working.Strings[BestMatch].Sta == Working.Strings[0].Fin)
		{
			if (!Working.MergeStrings(0, BestMatch, false, PassATurn*BestValue))  return false;
		}
		else
		{
			if (!Working.MergeStrings(BestMatch, 0, false, PassATurn*BestValue))  return false;
		}

		while (true)
		{
			int F = ForcedMove(&Working);

			if (F < 0)  return false;

			if (F == 2)  return true;  // we have found a face

			if (F == 0)
			{
				if (CanCompleteAFace(&Working))  return true;  // we have found a face
				break;  // we haven't made progress
			}
		}

		Working.SortStrings();
	}
	return false;
}


bool CFacesVarley::CanCompleteAFace(STRINGS *Strings)
//Creates a new face if possible, and returns true.

//Two strings may be merged to form a face if the last vertex in
//each is the first vertex in the other, provided that no other vertex
//appears in both strings.
{
	for (int T=0;  T<Strings->Count; ++T)
	{
		for (int S=T+1;  S<Strings->Count;  ++S)
		{
			if ((Strings->Strings[T].Fin == Strings->Strings[S].Sta) &&
				(Strings->Strings[S].Fin == Strings->Strings[T].Sta) &&
				Strings->StringsCanBeMerged(T,S))
			{
				CreateAFaceFromStrings(Strings, T, S);
				return true;
			}
		}
	}

	return false;
}


void CFacesVarley::Input_Varley(CDB_Model *DDBB)
// Copies information from external DataBase
// into the local database of this FindingFaces algorithm

// The intput required by the algorithms is:
//		VertexCount= number of vertex in the wireframe
//		VertexX[i]= X coordinate of the i-th vertex
//		VertexY[i]= Y coordinate of the i-th vertex
//		VertexZ[i]= Z coordinate of the i-th vertex
//		(VertexZ onlyr required for the Inflated approach)
//		EdgeCount= number of edges in the wireframe
//		EdgeU[i]= Head vertex defining the i-th edge
//		EdgeV[i]= Tail vertex defining the i-th edge

// This function should be adapted to different external databases
// This function should be called before calculating faces by Get_Faces_Varley
{
	//Copy vertices from global to local database
	VertexX.clear();
	VertexY.clear();
	VertexCount= DDBB->GetSizeVertices();
	for (long i=0; i<VertexCount; i++){
		CVertex Vertice= DDBB->GetVertex(i);
		VertexX.push_back(Vertice.x);
		VertexY.push_back(Vertice.y);
	}

	//Copy edges from global to local database
	EdgeU.clear();
	EdgeV.clear();
	EdgeCount= DDBB->GetSizeEdges();
	for (long i=0; i<EdgeCount; i++){
		CEdge Arista= DDBB->GetEdge(i);
	
		EdgeU.push_back(Arista.Head);
		EdgeV.push_back(Arista.Tail);
	}
}


bool CFacesVarley::Get_Faces_Varley()
//Main flow to get faces from a graph-like representation of a polyhedrical shape

//Based on:
	//Varley P.A.C. and Company P.
	//A new algorithm for finding faces in wireframes.
	//Computer-Aided Design (ISSN 0010-4485). Volume 42, Issue 4, April 2010, Pages 279-309

//See also:
	//Varley P.A.C.
	//Implementing the new algorithm for finding faces in wireframes
	//Regeo Technical Report 05/2009
	//(http://www.regeo.uji.es/publicaciones/regeo05.pdf)
{
	FaceCount = 0;
	Face.clear();

	ListExternalTriangularLoops();
	ListParallelQuadrilateralLoops();
	IdentifyThroughVertices();
	CreateInitialStrings();

	int SeedVertex= ChooseSeedVertex();
	MakeFirstMove(SeedVertex);

	while(Master.Count){
		int F= ForcedMove(&Master);

		if (F < 0){
			AfxMessageBox("Unable to find faces");
			return false;
		}
		else if (F==0){
			if (!CanCompleteAFace(&Master)){
				if (!MergeArbitraryStrings()){
					AfxMessageBox("Unable to find faces");
					return false;
				}
			}
		}
	}
	return true;
}


CFacesVarley::CFacesVarley(CDB_Model *DDBB)
{
	//Input data
	Input_Varley(DDBB);

	//Add implicit data to the local database
	//(Valences and sub-graphs)
	GetImplicitData();

	//Reset counters
	//--------------
	TriangularLoopCount = 0;
	QuadrilateralLoopCount = 0;

	//Subgraphs data
	SubgraphCount = 0;
	OuterSubgraph = 0;

	//Tuneable parameters for string priority
	//---------------------------------------
	InitialPriorityTri  = 50;
	InitialPriorityQuad = 10;
	PassATurn = 0.4;

	//Tuneable paremeter for parallelism checking
	//-------------------------------------------
	//For historical reasons, we use 10 in most of our investigations.
	//Any value from 8 to 36 gives the same results
	tcMp = 10.0;

	//Switch between 2D and 3D
	//------------------------
	Inflated= 0; // O if 3D vertices are not available, 1 otherwise
};


CFacesVarley::~CFacesVarley()
{
};


bool CFacesVarley::EdgeInContour(long NumGraph, long Edge)
// Return true if current edge does belong to the contour
{
	for (long i=0; i<(long)ReferApp.Cu.Pe.PerimeterEdges[NumGraph].size(); i++){
		if(ReferApp.Cu.Pe.PerimeterEdges[NumGraph][i] == Edge)
			return true;
	}

	return false;
}


bool CFacesVarley::FaceIsContour(FACE FaceI)
// Return true if current face does match contour
{
	if(ReferApp.Cu.Pe.PerimeterEdges.size() == 0)
		return false; // Contour not calculated

	for(long NumGraph= 0; NumGraph<ReferApp.Cu.SG.NumSubgraphs; NumGraph++){

		if(FaceI.C != (long)ReferApp.Cu.Pe.PerimeterEdges[NumGraph].size())
			return false;

		for (long j=0; j<FaceI.C; j++){
			if(!EdgeInContour(NumGraph, FaceI.E[j]))
				return false;
		}
	}

	return true;
}


void CFacesVarley::Output_Faces_Varley(CDB_Model *DDBB)
// Copies information from local DataBase of the FindingFaces algorithm
// into the external database (should be adapted to different external databases)

// Copy "true" faces from local to global database
// (Remove contour "face")

//In natural (or frontal geometry) line drawings,
//the algorithm detects the contour as if it was a face
//Such false faces are removed here before copying faces into the global database

// The output of the algorithms is:
//		FaceCount= number of faces found
//		Face[i].C= number of edges in face i
//		Face[i].E[j]= j-th edge in face i
//		Face[i].V[j]= j-th vertex in face i

// This function should be adapted to different external databases
// This function should be called after calculating faces by Get_Faces_Varley
{
	// Clear previos faces
	DDBB->ClearFaces();

	for (long i=0; i<FaceCount; i++){
		if(!FaceIsContour(Face[i])){ // (Remove contour "face")
			CFace Cara;
			for (long j=0; j<Face[i].C; j++){
				long edge= Face[i].E[j];
				Cara.AddEdgeInFace(Face[i].E[j]);
			}
			DDBB->AddFace(Cara);
		}
	}
}
