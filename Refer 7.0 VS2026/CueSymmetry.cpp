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
#include <cfloat>   // Used by isnan
#include <cmath>
#include <vector>

#include "Tools_Vector.h"
#include "Tools_Geometry.h"

//-------used to import/export data from/to the external database-----------
#include "DB_Model.h"
#include "Refer.h"   //Used for ReferApp
//--------------------------------------------------------------------------

#include "CueSymmetry.h"


CModelSymmetry::CModelSymmetry()
//Model for Symmetry detection
{
	VertexCount= 0;
	VertexX.clear();
	VertexY.clear();

	EdgeCount= 0;
	EdgeU.clear();
	EdgeV.clear();

	FaceCount= 0;
	Face.clear();
}


CModelSymmetry::CModelSymmetry(const CModelSymmetry& MS)
:VertexCount (MS.VertexCount),
 VertexX (MS.VertexX),
 VertexY (MS.VertexY),
 EdgeCount (MS.EdgeCount),
 EdgeU (MS.EdgeU),
 EdgeV (MS.EdgeV),
 FaceCount (MS.FaceCount),
 Face (MS.Face)
{
}


CModelSymmetry::~CModelSymmetry()
{
}


CModelSymmetry::CFACE::CFACE()
//Model for Symmetry detection
{
	C= 0;
	E.clear();
	V.clear();
}


CModelSymmetry::CFACE::~CFACE(){
}


void CModelSymmetry::DeleteVertex(long NumVertex)
//Deletes vertex NumVertex
//and renumbers subsequent vertices in the lists of edges and faces

//Warning: This function is currently used only after faces have been removed
{
	//Delete vertex
	VertexX.erase(VertexX.begin()+NumVertex);
	VertexY.erase(VertexY.begin()+NumVertex);
	VertexCount--;

	//Renumber subsequent vertices in the edge list
	for (long i=0; i<EdgeCount;i++){
		assert (EdgeU[i]!=NumVertex && EdgeV[i]!=NumVertex);

		if(EdgeU[i]>NumVertex)
			EdgeU[i]--;
		if(EdgeV[i]>NumVertex)
			EdgeV[i]--;
	}

	/////////////////////////////////
	//Renumber subsequent vertices in the face list
	//for (long i=0; i<FaceCount;i++){
	//	for (long j=0; j<(long)Face[i].V.size();i++){
	//		assert (Face[i].V[j]!=NumVertex && Face[i].V[j]!=NumVertex);

	//		if(Face[i].V[j]>NumVertex)
	//			Face[i].V[j]--;
	//	}
	//}
	/////////////////////////////////
}


void CModelSymmetry::DeleteEdge(long NumEdge)
//Deletes edge NumEdge
//and renumbers subsequent edges in the list of faces

//Warning: This function is currently used only after faces have been removed
{
	//Delete edge
	EdgeU.erase(EdgeU.begin()+NumEdge);
	EdgeV.erase(EdgeV.begin()+NumEdge);
	EdgeCount--;

	/////////////////////////////////
	//Renumber subsequent edges in the face list
	//for (long i=0; i<FaceCount;i++){
	//	for (long j=0; j<(long)Face[i].V.size();i++){
	//		assert (Face[i].E[j]!=NumEdge && Face[i].E[j]!=NumEdge);

	//		if(Face[i].E[j]>NumEdge)
	//			Face[i].E[j]--;
	//	}
	//}
	/////////////////////////////////
}


void CModelSymmetry::ClearAllFaces()
//Clears all the faces
{
	FaceCount= 0;
	Face.clear();
}


double CModelSymmetry::GetFaceSize(long NumFace)
//Returns the size of the face Numface,
//defined as the maximum size of the minimal box that encloses the face
{
	double Xmin, Ymin, Xmax, Ymax;
	Xmin = Xmax = VertexX[Face[NumFace].V[0]];
	Ymin = Ymax = VertexY[Face[NumFace].V[0]];
	for (long i= 1; i<(long)Face[NumFace].V.size() ; i++){
		Xmin = min(Xmin, VertexX[Face[NumFace].V[i]]);
		Xmax = max(Xmax, VertexX[Face[NumFace].V[i]]);
		Ymin = min(Ymin, VertexY[Face[NumFace].V[i]]);
		Ymax = max(Ymax, VertexY[Face[NumFace].V[i]]);
	}
	return max(std::fabs(Xmax-Xmin), std::fabs(Ymax-Ymin));
}


long CModelSymmetry::GetEdge(long V1, long V2)
//Returns the number of the edge that connects vertices V1 and V2
//Returns -1 if none edge connects vertices V1 and V2
{
	for(long i=0; i<EdgeCount; i++){
		if (((EdgeU[i] == V1)||(EdgeU[i] == V2))&&
			((EdgeV[i] == V1)||(EdgeV[i] == V2))){
			return i;
		}
	}
	return -1;
}


void CGraphs::SplitEdge(CModelSymmetry &MS, long Position)
//Edge "Position" is splitted into two sub-edges
//Its mid point is added two times as two overlapped vertices
{
	////Get mid-point
	POINT2D VertexMid ((MS.VertexX[MS.EdgeU[Position]] + MS.VertexX[MS.EdgeV[Position]])/2, 
					   (MS.VertexY[MS.EdgeU[Position]] + MS.VertexY[MS.EdgeV[Position]])/2);

	PairedMidPoints.push_back(std::vector <long> ());

	//Add the mid-point two times
	MS.VertexX.push_back(VertexMid.x);
	MS.VertexY.push_back(VertexMid.y);
	PairedMidPoints[PairedMidPoints.size()-1].push_back(MS.VertexCount);
	MS.VertexCount++;

	MS.VertexX.push_back(VertexMid.x);
	MS.VertexY.push_back(VertexMid.y);
	PairedMidPoints[PairedMidPoints.size()-1].push_back(MS.VertexCount);
	MS.VertexCount++;

	//Add two sub-edges
	MS.EdgeU.push_back(MS.EdgeU[Position]);
	MS.EdgeV.push_back(MS.VertexCount-2);
	MS.EdgeCount++;

	MS.EdgeU.push_back(MS.EdgeV[Position]);
	MS.EdgeV.push_back(MS.VertexCount-1);
	MS.EdgeCount++;

	////Remove the original edge
	MS.DeleteEdge(Position);
}


void CGraphs::SplitVertex(CModelSymmetry &MS, long Position)
//Vertex "Position" is splitted into a set of overlapped vertices
//Edges connected to vertex "position" are re-connected to those new vertices
{
	PairedVertices.push_back(std::vector <long> ());

	//Split the vertex and reconnect concurrent edges
	for (long i=0; i<MS.EdgeCount;i++){
		if(MS.EdgeU[i]==Position){
			MS.VertexX.push_back(MS.VertexX[Position]);
			MS.VertexY.push_back(MS.VertexY[Position]);
			PairedVertices[PairedVertices.size()-1].push_back(MS.VertexCount);
			MS.VertexCount++;

			MS.EdgeU[i]= MS.VertexCount-1;
		}

		else if(MS.EdgeV[i]==Position){
			MS.VertexX.push_back(MS.VertexX[Position]);
			MS.VertexY.push_back(MS.VertexY[Position]);
			PairedVertices[PairedVertices.size()-1].push_back(MS.VertexCount);
			MS.VertexCount++;

			MS.EdgeV[i]= MS.VertexCount-1;
		}
	}

	//Remove the original vertex
	MS.DeleteVertex(Position);

	//Renumber subsequent vertices in the paired midpoints list
	for (long i=0; i<(long)PairedMidPoints.size();i++){
		for (long j=0; j<(long)PairedMidPoints[i].size();j++){
			if(PairedMidPoints[i][j]>Position)
				PairedMidPoints[i][j]--;
		}
	}

	//Renumber subsequent vertices in the paired vertices list
	for (long i=0; i<(long)PairedVertices.size();i++){
		for (long j=0; j<(long)PairedVertices[i].size();j++){
			if(PairedVertices[i][j]>Position)
				PairedVertices[i][j]--;
		}
	}

}


void CGraphs::SplitByCircuit(CModelSymmetry &MS, std::vector<long> Circuit)
//Splits the model into two subgraphs
//through those vertices and edges that belong to the circuit

//Cirtuits are closed chains of vertices/midpoints

//Information is stored in Circuit as follows:
//		-vertices are stored by their numbers
//		-midpoints are stored by the number of the edge they belong to
//		 switched to negative and decreased by one unit
//		 (i.e. midpoint of edge 3 is stored as -4)
{
	//Remove edges of the model that are sides of the circuit
	for(long i= 0; i<(long)Circuit.size()-1; i++){
		if(Circuit[i]>=0 && Circuit[i+1] >= 0){
			long edge= MS.GetEdge(Circuit[i], Circuit[i+1]);
			if(edge>=0){
				MS.DeleteEdge(edge);

				//Renumber corners of the circuit
				//that are midpoints of subsequent edges
				for(long k= 0; k<(long)Circuit.size(); k++){
					if(Circuit[k]<-edge-1)
						Circuit[k]++;
				}
			}
		}
	}

	//Sort the circuit of sides to remove from last to first
	std::vector<long> SortedCircuit= Circuit;
	SortedCircuit.erase(SortedCircuit.begin()+SortedCircuit.size()-1); //Remove closing item
	SortedCircuit= SortMinorFirst(SortedCircuit);

	//Split edges intersected at their midpoints
	for(long i= 0; i<(long)SortedCircuit.size(); i++){
		if(SortedCircuit[i]<0){
			SplitEdge(MS, -SortedCircuit[i]-1);  //Also calculates PairedMidPoints
		}
		else
			break;
	}

	//Split vertices and update edges
	for(long i= (long)SortedCircuit.size()-1; i>=0; i--){
		if(SortedCircuit[i]>=0){
			SplitVertex(MS, SortedCircuit[i]);  //Also calculates PairedVertices
		}
		else
			break;
	}
}


void CGraphs::DetermineSubgraphs(CModelSymmetry MS)
//Breaks down the wireframe into subgraphs

	//WARNING: this is an embeeded version of the code
	//used to calculate subgraphs in "CueSubGraphs"

{
	SubgraphForVertex.assign(MS.VertexCount, -1);
	SubgraphForEdge.assign(MS.EdgeCount, -1);

	SubgraphCount = 0;

	//Identify subgraphs, until no more vertices remain unassigned
	for (;;){
		// identify an unassigned vertex
		int V = -1;
		for (int i=0; i<MS.VertexCount; ++i)  if (SubgraphForVertex[i]==-1){
			V = i;
			break;
		}

		if (V < 0)  break;  // no more unassigned vertices
		SubgraphForVertex[V] = SubgraphCount;

		for (;;){
			bool Progress = false;

			for (int i=0; i<MS.EdgeCount; ++i)  if (SubgraphForEdge[i] == -1){
				if (SubgraphForVertex[MS.EdgeU[i]] == SubgraphCount){
					SubgraphForEdge[i] = SubgraphCount;
					SubgraphForVertex[MS.EdgeV[i]] = SubgraphCount;
					Progress = true;
				}
				else
				if (SubgraphForVertex[MS.EdgeV[i]] == SubgraphCount){
					SubgraphForEdge[i] = SubgraphCount;
					SubgraphForVertex[MS.EdgeU[i]] = SubgraphCount;
					Progress = true;
				}
			}
			if (!Progress) break;
		}
		++SubgraphCount;
	}
}


void CGraphs::DetermineFaceSubgraphs(CModelSymmetry MS)
//Determines the subgraph for each face of the model MS

//The function assumes that DetermineSubgraphs was executed first
{
	SubgraphForFace.assign(MS.FaceCount, 0);

	if(SubgraphCount == 1) return;

	for (long NumFace=0; NumFace<MS.FaceCount; NumFace++){
		long NumGraph= SubgraphForEdge[MS.Face[NumFace].E[0]];

		//Alternatively, assume that all the edges and vertices
		//belong to the same subgraph than the first edge of the face
		//---------------------------------------------------------------------
		//Check that the rest of the edges belong to the same subgraph
		bool EdgesInSubGraph= true;
		for (long j=1; j<(long)MS.Face[NumFace].E.size(); j++){
			if(SubgraphForEdge[MS.Face[NumFace].E[j]] != NumGraph){
				EdgesInSubGraph= false;
				break;
			}
		}

		//Check that vertices belong to the same subgraph
		bool VerticesInSubGraph= true;
		for (long j=0; j<(long)MS.Face[NumFace].V.size(); j++){
			if(SubgraphForVertex[MS.Face[NumFace].V[j]] != NumGraph){
				VerticesInSubGraph= false;
				break;
			}
		}

		if(VerticesInSubGraph && EdgesInSubGraph)
		//---------------------------------------------------------------------
			SubgraphForFace[NumFace]= NumGraph;
	}
}


void CCueSymmetry::Input(CDB_Model *DDBB)
//Copies information from external DataBase
//into the local database

//The intput required by the algorithms is:
//		VertexCount= number of vertices in the wireframe
//		VertexX[i]= X coordinate of the i-th vertex
//		VertexY[i]= Y coordinate of the i-th vertex
//		EdgeCountForVertex[i]= number of edges connected to i-th vertex

//		EdgeCount= number of edges in the wireframe
//		EdgeU[i]= Head vertex defining the i-th edge
//		EdgeV[i]= Tail vertex defining the i-th edge

//		FaceCount= number of faces found
//		Face[i].C= number of edges in face i
//		Face[i].E[j]= j-th edge in face i
//		Face[i].V[j]= j-th vertex in face i

//This function should be adapted to different external databases
{
	//Copy vertices from global to local database
	MS_main.VertexX.clear();
	MS_main.VertexY.clear();
	MS_main.VertexCount= DDBB->GetSizeVertices();
	for (long i=0; i<MS_main.VertexCount; i++){
		CVertex v= DDBB->GetVertex(i);
		MS_main.VertexX.push_back(v.x);
		MS_main.VertexY.push_back(v.y);
	}

	//Copy edges from global to local database
	MS_main.EdgeU.clear();
	MS_main.EdgeV.clear();
	MS_main.EdgeCount= DDBB->GetSizeEdges();
	for (long i=0; i<MS_main.EdgeCount; i++){
		CEdge e= DDBB->GetEdge(i);
	
		MS_main.EdgeU.push_back(e.Head);
		MS_main.EdgeV.push_back(e.Tail);
	}

	//Copy faces from global to local database
	MS_main.FaceCount= DDBB->GetSizeFaces();
	for (long i=0; i<MS_main.FaceCount; i++){
		CFace f= DDBB->GetFace(i);

		CModelSymmetry::CFACE NewFace;
		NewFace.C= f.GetSizeEdgesInFace();
		for (long j=0; j<NewFace.C; j++){
			NewFace.E.push_back(f.GetEdgeInFace(j));
		}

		std::vector<long> vertices= DDBB->GetAllVerticesInFace(f);
		long NumVertices= (long)vertices.size();
		for (long j=0; j<NumVertices; j++){
			NewFace.V.push_back(vertices[j]);
		}

		MS_main.Face.push_back(NewFace);

		//////////////////////////////////////////////////////
		//Check that edges are consecutive
		long First= MS_main.EdgeU[f.GetEdgeInFace(0)];
		long Next= MS_main.EdgeV[f.GetEdgeInFace(0)];
		if(First==MS_main.EdgeU[f.GetEdgeInFace(1)] ||
			First==MS_main.EdgeV[f.GetEdgeInFace(1)]){
			First= MS_main.EdgeV[f.GetEdgeInFace(0)];
			Next= MS_main.EdgeU[f.GetEdgeInFace(0)];
		}
		for (long j=1; j<NewFace.C; j++){
			long edge= f.GetEdgeInFace(j);
			if(MS_main.EdgeU[edge]== Next)
				Next= MS_main.EdgeV[edge];
			else if(MS_main.EdgeV[edge]== Next)
				Next= MS_main.EdgeU[edge];
			else
				assert(false);
		}
		//////////////////////////////////////////////////////
	}

	//Parameters and flags to control symmetry finding process
	bDetectNormalCrossing= ReferApp.Cu.Sy.DetectNormalCrossing;
	MaxParallelCrossing= ReferApp.Cu.Sy.MaxParallelCrossing;
	MinParallelCrossing= ReferApp.Cu.Sy.MinParallelCrossing;
	MeritWeight_1= ReferApp.Cu.Sy.MeritWeight_1;

	bDetectDistorted= ReferApp.Cu.Sy.DetectDistorted;
	MinSymDistortion= ReferApp.Cu.Sy.MinSymDistortion;
	MaxSymDistortion= ReferApp.Cu.Sy.MaxSymDistortion;
	//MaxParallelFaceSymLines= ReferApp.Cu.Sy.MaxParallelFaceSymLines;
	//MinParallelFaceSymLines= ReferApp.Cu.Sy.MinParallelFaceSymLines;
	MeritWeight_2= ReferApp.Cu.Sy.MeritWeight_2;

	bSymmetricalEdges= ReferApp.Cu.Sy.SymmetricalEdges;
	MaxParallelEdgeSymLines= ReferApp.Cu.Sy.MaxParallelEdgeSymLines;
	MinParallelEdgeSymLines= ReferApp.Cu.Sy.MinParallelEdgeSymLines;
	MeritWeight_3= ReferApp.Cu.Sy.MeritWeight_3;

	bPrioritizeVertical= ReferApp.Cu.Sy.PrioritizeVertical;
	minVerticalThreshold= ReferApp.Cu.Sy.minVerticalThreshold;
	maxVerticalThreshold= ReferApp.Cu.Sy.maxVerticalThreshold;
	MeritWeight_4= ReferApp.Cu.Sy.MeritWeight_4;

	bRemoveBadMerit= ReferApp.Cu.Sy.RemoveBadMerit;
	MinMerit= ReferApp.Cu.Sy.MinMerit;


	//PARAMETERS NOT INCLUDED IN INTERACTIVE DIALOGS!!!!
	/////////////////////////////////////
	//Tolerances to determine if two segments "nearly" intersect to each other
	IntersectTolInside= 0.1;  // 10% of the total length of the smaller segment
	IntersectTolParallel= DEGREES_TO_RADIANS(5.0); //Threshold to consider two edges as "nearly" parallel

	/////////////////////////////////////
	//Thresholds to detect "nearly" collinear edges
	Collinear_angle= DEGREES_TO_RADIANS(10);
	Collinear_offset= 20;   //% of the length of the shortest line
	/////////////////////////////////////
}


void CCueSymmetry::GetImplicitData()
//Obtains data derived from the input data and required to detect symmetries
{
	//Update the number of edges connected to every vertex
	MS.EdgesInVertex.assign(MS.VertexCount, std::vector <long>());
	for (long i=0; i<MS.EdgeCount; ++i){
		MS.EdgesInVertex[MS.EdgeU[i]].push_back(i);
		MS.EdgesInVertex[MS.EdgeV[i]].push_back(i);
	}

	//Update the list of faces sharing every edge
	MS.FacesInEdge.assign(MS.EdgeCount, std::vector <long>());
	for (long i=0; i<MS.FaceCount; ++i){
		for (long j=0; j<(long)MS.Face[i].E.size(); ++j){
			MS.FacesInEdge[MS.Face[i].E[j]].push_back(i);
		}
	}

	//for (long i=0; i<MS.EdgeCount; ++i){
		//assert(MS.FacesInEdge[i].size() ==2); //Otherwise it is not a closed polyhedron
	//}
}


void CCueSymmetry::GetSubGraph(CGraphs CG, long NumGraph,
							   std::vector <long> &ReverseMapVertices,
							   std::vector <long> &ReverseMapEdges,
							   std::vector <long> &ReverseMapFaces)
//Extracts subgraph NumGraph and calculates its mappings

//Mappings are later required to link the symmetry planes to the whole model
{
	//Copy vertices from global to local database
	MS.VertexX.clear();
	MS.VertexY.clear();
	MS.VertexCount= 0;
	std::vector <long> MapVertices;
	for (long i=0; i<MS_main.VertexCount; i++){
		if(CG.SubgraphForVertex[i] == NumGraph){
			MS.VertexX.push_back(MS_main.VertexX[i]);
			MS.VertexY.push_back(MS_main.VertexY[i]);
			MapVertices.push_back(MS.VertexCount);
			ReverseMapVertices.push_back(i);
			MS.VertexCount++;
		}
		else
			MapVertices.push_back(-1);
	}

	//Copy edges from global to local database
	MS.EdgeU.clear();
	MS.EdgeV.clear();
	MS.EdgeCount= 0;
	std::vector <long> MapEdges;
	for (long i=0; i<MS_main.EdgeCount; i++){
		if(CG.SubgraphForEdge[i] == NumGraph){
			MS.EdgeU.push_back(MapVertices[MS_main.EdgeU[i]]);
			MS.EdgeV.push_back(MapVertices[MS_main.EdgeV[i]]);
			MapEdges.push_back(MS.EdgeCount);
			ReverseMapEdges.push_back(i);
			MS.EdgeCount++;
		}
		else
			MapEdges.push_back(-1);
	}

	//Copy faces from global to local database
	MS.Face.clear();
	MS.FaceCount= 0;
	for (long i=0; i<MS_main.FaceCount; i++){
		if(CG.SubgraphForFace[i] == NumGraph){
			//Copy face (after re-mapping edges and vertices)
			CModelSymmetry::CFACE face;
			face.C= MS_main.Face[i].C;
			for (long j=0; j<face.C; j++)
				face.V.push_back(MapVertices[MS_main.Face[i].V[j]]);
			for (long j=0; j<face.C; j++)
				face.E.push_back(MapEdges[MS_main.Face[i].E[j]]);
			MS.Face.push_back(face);
			ReverseMapFaces.push_back(i);
			MS.FaceCount++;
		}
	}

	GetImplicitData();
}


void CCueSymmetry::PrintInfo()
//Displays information about the performance of the algorithm
{
	CString sMensaje, sMensajeAux;
	sMensaje=      "AXES";
	sMensaje+="\r\n      Total =";
	sMensajeAux.Format("%i", info_TotalAxes);
	sMensaje+=sMensajeAux;
	sMensaje+="\r\n      Non-bilateral (Rule #1.1) =";
	sMensajeAux.Format("%i", info_NonBilateralAxes);
	sMensaje+=sMensajeAux;
	sMensaje+="\r\n      Discontinued  (Rule #1.2) =";
	sMensajeAux.Format("%i", info_DiscontinuedAxes);
	sMensaje+=sMensajeAux;
	sMensaje+="\r\n      Simplification =";
	sMensajeAux.Format("%.2f", 100.*(float)(info_NonBilateralAxes+info_DiscontinuedAxes)/(float)info_TotalAxes);
	sMensaje+=sMensajeAux;
	sMensaje+=" %";

	sMensaje+="\r\n";

	sMensaje+="\r\n EDGES";
	sMensaje+="\r\n      Total =";
	sMensajeAux.Format("%i", MS.EdgeCount);
	sMensaje+=sMensajeAux;
	sMensaje+="\r\n      Non symmetric edges (Rule #1.3) =";
	sMensajeAux.Format("%i", info_NonSymmetricEdges);
	sMensaje+=sMensajeAux;
	sMensaje+="\r\n      Simplification =";
	sMensajeAux.Format("%.2f", 100.*(float)info_NonSymmetricEdges/(float)MS.EdgeCount);
	sMensaje+=sMensajeAux;
	sMensaje+=" %";

	sMensaje+="\r\n";

	sMensaje+="\r\n SIDES";
	sMensaje+="\r\n      Total =";
	sMensajeAux.Format("%i", info_TotalAxes + MS.EdgeCount);
	sMensaje+=sMensajeAux;
	long numsides= info_TotalAxes-info_NonBilateralAxes-info_DiscontinuedAxes +
					MS.EdgeCount-info_NonSymmetricEdges;
	sMensaje+="\r\n      Valid =";
	sMensajeAux.Format("%i", numsides);
	sMensaje+=sMensajeAux;
	sMensaje+="\r\n      Simplification (Rule #1) =";
	sMensajeAux.Format("%.2f", 100.*(1.-(float)numsides/(float)(info_TotalAxes+MS.EdgeCount)));
	sMensaje+=sMensajeAux;
	sMensaje+=" %";

	sMensaje+="\r\n";

	sMensaje+="\r\n BRANCHES";
	sMensaje+="\r\n      Total tested branches =";
	sMensajeAux.Format("%i", info_Branches);
	sMensaje+=sMensajeAux;
	sMensaje+="\r\n      Branches at invalid CCR =";
	sMensajeAux.Format("%i", info_BranchesAtInvalidCCR);
	sMensaje+=sMensajeAux;
	sMensaje+="\r\n      Branches at visited faces =";
	sMensajeAux.Format("%i", info_BranchesAtVisitedFaces);
	sMensaje+=sMensajeAux;
	sMensaje+="\r\n      Branches at visited corners =";
	sMensajeAux.Format("%i", info_BranchesAtVisitedCorners);
	sMensaje+=sMensajeAux;
	sMensaje+="\r\n      Valid branches =";
	sMensajeAux.Format("%i", info_ValidBranches);
	sMensaje+=sMensajeAux;
	sMensaje+="\r\n      Simplification (Rules #2 #3) =";
	sMensajeAux.Format("%.2f", 100.*(1.-(float)info_ValidBranches/(float)info_Branches));
	sMensaje+=sMensajeAux;
	sMensaje+=" %";

	sMensaje+="\r\n";

	sMensaje+="\r\n CIRCUITS";
	sMensaje+="\r\n      Tested =";
	sMensajeAux.Format("%i", info_TestedCircuits);
	sMensaje+=sMensajeAux;
	sMensaje+="\r\n      Found =";
	sMensajeAux.Format("%i", info_Found);
	sMensaje+=sMensajeAux;
	sMensaje+="\r\n      Removed =";
	sMensajeAux.Format("%i", info_RemovedCircuits);
	sMensaje+=sMensajeAux;

	AfxMessageBox (sMensaje);
}


void CCueSymmetry::OutputReset(long FaceCount)
{
	//Clear global symmetry information for every face
	ReferApp.Cu.Sy.FaceAxesU.assign(FaceCount, std::vector<long>());
	ReferApp.Cu.Sy.FaceAxesV.assign(FaceCount, std::vector<long>());

	//Clear global symmetry circuits
	ReferApp.Cu.Sy.SymPlanes.clear();
	ReferApp.Cu.Sy.SymMerits.clear();
}


void CCueSymmetry::Output(long NumGraph,
						  std::vector <long> ReverseMapVertices,
						  std::vector <long> ReverseMapEdges,
						  std::vector <long> ReverseMapFaces)
//Copies information from the local DataBase
//into the external database

//The output provided by the algorithms is:
//    - For every face of the model:
//         * FaceAxes is the list of skewed axes of symmetry of the face
//              FaceAxesU contains the list of initial endpoints of the succesive local symmetry axis
//              FaceAxesV contains the list of final endpoints of the succesive local symmetry axis

//    - For the whole model:
//         * SymPlanes is the list of bilateral symmetry circuits of the model
//           Information is stored in circuits as follows:
//             -vertices are stored by their numbers
//             -midpoints are stored by the number of the edge they belong to
//              switched to negative and decreased by one unit
//             (i.e. midpoint of edge 3 is stored as -4)
//         * SymMerits is the list of merits of all the bilateral symmetry circuits of the model


//This function should be adapted to different external databases
{
	//Copy face symmetry axes to global database
	//(This information is useful for representation)
	for (long i=0; i<(long)FaceAxesU.size(); i++){
		for (long j=0; j<(long)FaceAxesU[i].size(); j++){
			if(FaceAxesU[i][j] >= 0)
				ReferApp.Cu.Sy.FaceAxesU[ReverseMapFaces[i]].push_back(ReverseMapVertices[FaceAxesU[i][j]]);
			else
				ReferApp.Cu.Sy.FaceAxesU[ReverseMapFaces[i]].push_back(-ReverseMapEdges[-FaceAxesU[i][j]-1]-1);
		}
	}

	for (long i=0; i<(long)FaceAxesV.size(); i++){
		for (long j=0; j<(long)FaceAxesV[i].size(); j++){
			if(FaceAxesV[i][j] >= 0)
				ReferApp.Cu.Sy.FaceAxesV[ReverseMapFaces[i]].push_back(ReverseMapVertices[FaceAxesV[i][j]]);
			else
				ReferApp.Cu.Sy.FaceAxesV[ReverseMapFaces[i]].push_back(-ReverseMapEdges[-FaceAxesV[i][j]-1]-1);
		}
	}

	//Copy bilateral symmetry circuits to global database
	ReferApp.Cu.Sy.SymPlanes.push_back(std::vector<std::vector<long>>());
	for (long i=0; i<(long)SymPlanes.size(); i++){
		ReferApp.Cu.Sy.SymPlanes[NumGraph].push_back(std::vector<long>());
		for (long j=0; j<(long)SymPlanes[i].size(); j++){
			if(SymPlanes[i][j] >= 0)
				ReferApp.Cu.Sy.SymPlanes[NumGraph][i].push_back(ReverseMapVertices[SymPlanes[i][j]]);
			else
				ReferApp.Cu.Sy.SymPlanes[NumGraph][i].push_back(-ReverseMapEdges[-SymPlanes[i][j]-1]-1);
		}
	}

	//Copy merits of bilateral symmetry cirtuits to global database
	ReferApp.Cu.Sy.SymMerits.push_back(SymMerits);
}


long CCueSymmetry:: findSide(POINT2D A1, POINT2D A2, POINT2D P)
//Calculates the side of point P relative to the axis A1-A2

//Returns:
//	 0 if point P is collinear with the axis defined by A1 and A2
//	 1 if point P is in the left side of A1-A2
//	-1 if point P is in the right side of A1-A2
{
	//Threshold to cope with the imperfections of sketches
	double accuracy= 0.0000001 * max(max(P.x, max(A2.x, A1.x)), max(P.y, max(A2.y, A1.y)));

	if(std::fabs(A2.x-A1.x) < accuracy){ // vertical line
		if(std::fabs(P.x-A2.x) < accuracy)
			return 0;
		if(P.x < A2.x)
			return A2.y > A1.y ? 1 : -1;
		if(P.x > A2.x)
			return A2.y > A1.y ? -1 : 1;
	}
	if(std::fabs(A2.y-A1.y) < accuracy){ // horizontal line
		if(std::fabs(P.y-A2.y) < accuracy)
			return 0;
		if(P.y < A2.y)
			return A2.x > A1.x ? -1 : 1;
		if(P.y > A2.y)
			return A2.x > A1.x ? 1 : -1;
	}

	double slope= (A2.y-A1.y) / (A2.x-A1.x);
	double yIntercept= A1.y-A1.x * slope;
	double cSolution= (slope*P.x) + yIntercept;
	if(slope != 0){
		if(std::fabs(P.y-cSolution) < accuracy)
			return 0;
		if(P.y > cSolution)
			return A2.x > A1.x ? 1 : -1;
		if(P.y < cSolution)
			return A2.x > A1.x ? -1 : 1;
	}
	return 0;
}


bool CCueSymmetry::SearchInList(std::vector <long> List,
								long value, long next,
								long &position, long &delta)
//Returns the position of value in the List
//Returns delta=+1 if next is placed after value, and -1 if it is place before
{
	long n= (long)List.size();

	if(!IsValueInVector(value, List, position))
		return false;

	long ii= position+1;
	while(ii >= n) ii= ii-n;
	if(List[ii] == next){
		delta=+1;
		return true;
	}

	ii= position-1;
	while(ii < 0) ii= ii+n;
	if(List[ii] == next){
		delta=-1;
		return true;
	}

	return false;
}


bool CCueSymmetry::IntersectInside(POINT2D a1, POINT2D b1,
								   POINT2D a2, POINT2D b2,
								   double TolInside,
								   double TolParallel)
//Returns true if the intersection of two segments
//is internal to both (whithin a tolerance)
//and they are not parallel (within a tolerance)
//False otherwise

//Input: segment 1 endpoints (a1.x,a1.y) (b1.x,b1.y)
//       segment 2 endpoints (a2.x,a2.y) (b2.x,b2.y)

//The intersection is considered internal if
//the intersection point belongs to both REDUCED/EXTENDED segments

//Segments are reduced (the neighborhood of their endpoints is excluded) if Threshold is positive
//Segments are extended (the neighborhood of their endpoints is added) if Threshold is negative
{
	double s1x = b1.x - a1.x;
	double s1y = b1.y - a1.y;
	double s2x = b2.x - a2.x;
	double s2y = b2.y - a2.y;
	double denom= (-s2x * s1y + s1x * s2y);

	if(denom == 0)
		//Edges are parallel
		return false;

	double s = (-s1y * (a1.x - a2.x) + s1x * (a1.y - a2.y)) / denom;
	double t = ( s2x * (a1.y - a2.y) - s2y * (a1.x - a2.x)) / denom;

	//Check if intersection occurs inside the reduced/extended segments
	if ((s < 0+TolInside || s > 1-TolInside) || 
		(t < 0+TolInside || t > 1-TolInside)){
		//Intersection point is external to, at least, one of the segments
		return false;
	}

	//IntX = a1.x + (t * s1x);
	//IntY = a1.y + (t * s1y);

	return (NearlyParallel(a1, b1, a2, b2, TolParallel) == 0);
}


POINT2D CCueSymmetry::GetCorner(long ii)
//Returns the coordinates of a corner

//The corner may be a vertex (positive value of ii)
//or a midpoint of an edge (negative value of ii)
{
	POINT2D point(0, 0);

	if(ii>=0){
		point.x= MS.VertexX[ii];
		point.y= MS.VertexY[ii];
	}
	else{
		ii= -ii-1;
		point.x= (MS.VertexX[MS.EdgeU[ii]] + MS.VertexX[MS.EdgeV[ii]])/2;
		point.y= (MS.VertexY[MS.EdgeU[ii]] + MS.VertexY[MS.EdgeV[ii]])/2;
	}
	return point;
}


void CCueSymmetry::GetPolygonAxes(long NumFace)
//Gets axes of skewed symmetry for NumFace

//Every axis must pass through oposite vertices or mid-points
//of the polygon that delimits the face

//Thus, every vertex and every mid-point of the polygon
//is a candidate head/tail for one axis

//FaceAxes is the list of skewed axes of symmetry of the face
//      FaceAxesU contains the list of initial endpoints of the succesive local symmetry axis
//      FaceAxesV contains the list of final endpoints of the succesive local symmetry axis
//The values are as follows:
//     If vertices, their indices are stored
//     (as positive numbers)
//     If midpoints, the indices of the edges they belong to are stored
//     (switched to negative numbers and displaced by one unit to prevent zero index)

//Note that vertices and midpoints are interspersed, to ease the finding of their opposites
{
	//Add vertices
	std::vector<std::vector<long>> NumAxis;
	NumAxis.push_back(std::vector<long>());
	NumAxis[0]= MS.Face[NumFace].V;
	NumAxis.push_back(std::vector<long>());

	long V= (long)MS.Face[NumFace].V.size();
	long E= (long)MS.Face[NumFace].E.size();

	//Add midpoints
	for (long i=0; i<V; i++){
		long ii= (i+1)%V;

		//Search for the edge that contains the midpoint
		for (long j=0; j<E; j++){
			long jj= MS.Face[NumFace].E[j];
			if((MS.EdgeU[jj] == NumAxis[0][i] && MS.EdgeV[jj] == NumAxis[0][ii]) ||
			   (MS.EdgeV[jj] == NumAxis[0][i] && MS.EdgeU[jj] == NumAxis[0][ii])){
				NumAxis[1].push_back(-jj-1);
				break;
			}
		}
	}

	//Reorder axes to intersperse midpoints and vertices
	long a=0, b=0;
	for (long j=0; j<V; j++){
		for (long i=0; i<2; i++){
			if(a==0)
				FaceAxesU[NumFace].push_back(NumAxis[i][j]);
			else
				FaceAxesV[NumFace].push_back(NumAxis[i][j]);
			b++;
			if(b>=V){
				b=0;
				a++;
			}
		}
	}
}


void CCueSymmetry::RemoveAxis(long face, long axis)
{
	FaceAxesU[face].erase(FaceAxesU[face].begin()+axis);
	FaceAxesV[face].erase(FaceAxesV[face].begin()+axis);
}


void CCueSymmetry::RemoveNonBilateralAxes(long NumFace)
//Removes axes that do not divide the polygon in two sides
//with exactly the same number of vertices

//The function also checks that all the vertices of each half
//are in the same side
{
	//Axes in triangular faces are certainly bilateral!!
	if(MS.Face[NumFace].E.size() < 4)
		return;

	long N = (long)FaceAxesU[NumFace].size();
	if(N==0) return;

	for (long i=N-1; i>=0; i--){
		//Current axis
		long endU= FaceAxesU[NumFace][i];
		long endV= FaceAxesV[NumFace][i];
		POINT2D A1= GetCorner(endU);
		POINT2D A2= GetCorner(endV);

		//Discard if it does not divide the vertices in two sets of the same size
		long deviation= 0;
		for (long j=0; j<(long)MS.Face[NumFace].V.size(); j++){
			long jj= MS.Face[NumFace].V[j];
			if(jj==endU || jj==endV)
				continue;
			POINT2D point (MS.VertexX[jj], MS.VertexY[jj]);
			deviation+= findSide(A1, A2, point);
		}

		if(deviation != 0){ //Invalid axis, as it is non-bilateral
			RemoveAxis(NumFace, i);
			info_NonBilateralAxes++;
		}

		//Discard if any edge of the face intersects the axis
		else{
			for (long j=0; j<(long)MS.Face[NumFace].E.size(); j++){

				//Exclude edges that share endpoints or midpoint with the axis
				long EndU= MS.EdgeU[MS.Face[NumFace].E[j]];
				long EndV= MS.EdgeV[MS.Face[NumFace].E[j]];
				long Mid= -MS.Face[NumFace].E[j]-1;
				if(FaceAxesU[NumFace][i] != EndU && FaceAxesU[NumFace][i] != EndV && FaceAxesU[NumFace][i] != Mid &&
				   FaceAxesV[NumFace][i] != EndU && FaceAxesV[NumFace][i] != EndV && FaceAxesV[NumFace][i] != Mid){

					POINT2D U= GetCorner(EndU);
					POINT2D V= GetCorner(EndV);
					//if(IntersectInside(A1, A2, U, V, IntersectTolInside, IntersectTolParallel)){
					if(IntersectInside(A1, A2, U, V, 0., IntersectTolParallel)){
						//Invalid axis, as it is non-bilateral
						RemoveAxis(NumFace, i);
						info_NonBilateralAxes++;
						break;
					}
				}
			}
		}
	}
}


void CCueSymmetry::RemoveDiscontinuedMidPointAxes()
//Removes symmetry axes ending in, at least, one mid-point of an edge,
//when the face sharing the same edge does not have an axis
//connected to the same mid-point

//The rule is:
//   When a symmetry axis across a face ends in the mid-point of an edge,
//   the other face sharing this edge must also have an axis
//   ending in the mid-point of this edge
{
	//Repeat until no more axes are removed
	//(Is recursive, as deleting one discontinued axis
	// may result in another axis becoming discontinued)
	bool removed= true;
	while (removed){
		removed= false;

		//Loop over all the faces
		for (long Face1=0; Face1<MS.FaceCount; Face1++){
			//Get axes of the face ending at midpoints
			for (long j=0; j<(long)FaceAxesU[Face1].size(); j++){
				long AxisEnd= FaceAxesU[Face1][j];
				if (AxisEnd < 0){  //MidPoint axis
					//Select the other face sharing the edge
					long Face2= MS.FacesInEdge[-AxisEnd-1][0];
					if(Face2==Face1)
						Face2= MS.FacesInEdge[-AxisEnd-1][1];

					//Check if no other symmetry axis ends at the same midpoint
					if(!IsValueInVector(AxisEnd, FaceAxesU[Face2]) &&
					   !IsValueInVector(AxisEnd, FaceAxesV[Face2])){
						//Remove the isolated axis
						RemoveAxis(Face1, j);
						info_DiscontinuedAxes++;
						j--;
						removed= true;
					}
				}
			}

			for (long j=0; j<(long)FaceAxesV[Face1].size(); j++){
				long AxisEnd= FaceAxesV[Face1][j];
				if (AxisEnd < 0){  //MidPoint axis
					//Select the other face sharing the edge
					long Face2= MS.FacesInEdge[-AxisEnd-1][0];
					if(Face2==Face1)
						Face2= MS.FacesInEdge[-AxisEnd-1][1];

					//Check if no other symmetry axis ends at the same midpoint
					if(!IsValueInVector(AxisEnd, FaceAxesU[Face2]) &&
					   !IsValueInVector(AxisEnd, FaceAxesV[Face2])){
						//Remove the isolated axis
						RemoveAxis(Face1, j);
						info_DiscontinuedAxes++;
						j--;
						removed= true;
					}
				}
			}
		}
	}//end while
}


void CCueSymmetry::GetSkewAxes()
//Detects candidate axes of bilateral symmetry for all the faces
//    Lines connecting the centroid with every vertex of the face,
//    and every midpoint of the edges of the face are candidate symmetry axes

//Candidate axes that do not divide the polygon in two halves are removed

//Discontinued axes are removed
{
	info_TotalAxes= 0;
	info_NonBilateralAxes= 0;
	info_DiscontinuedAxes= 0;

	//Search candidate symmetry axes for every face
	for(long NumFace=0; NumFace<MS.FaceCount; NumFace++){
		//Get candidate axes of the face
		GetPolygonAxes(NumFace);

		info_TotalAxes+= (long)FaceAxesU[NumFace].size();

		//Remove topologically non-valid axes
		RemoveNonBilateralAxes(NumFace);
	}

	//Remove local symmetry axes that cannot belong to global symmetry circuits
	RemoveDiscontinuedMidPointAxes();
}


bool CCueSymmetry::AddAxesToSidesList()
//Adds new sides to the list of candidate symmetry sides
//including all local symmetry axes

//Returns true if new sides are added to the list
{
	bool added= false;

	for (long i=0; i<MS.FaceCount; i++){
		for (long j=0; j<(long)FaceAxesU[i].size(); j++){
			SymSidesU.push_back(FaceAxesU[i][j]);          //Initial endpoint of a candidate local symmetry axis
			SymSidesV.push_back(FaceAxesV[i][j]);          //Final endpoint of a candidate local symmetry axis
			SymSidesFace0.push_back(i);                    //Face at with the axis belongs
			added= true;
		}
	}

	SymSidesFace1= SymSidesFace0;
	SymSidesCollinear.assign(SymSidesU.size(), false);

	return added;
}


bool CCueSymmetry::AddEdgesToSidesList()
//Adds new sides to the list of candidate symmetry sides
//including all valid edges

//Edges are valid if they share two faces,
//and those faces have the same number of sides/vertices

//A second list of faces is also added,
//since edges belong simultaneously to two faces
//(this is so because the algorithm is only valid for closed polyhedral shapes)

//Edges that (1) share common faces, (2) are nearly collinear and (3) are non consecutive,
//are labelled to allow the circuit to go through them

//Returns true if new sides are added to the list
{
	info_NonSymmetricEdges= 0;

	bool added= false;

	for (long i=0; i<MS.EdgeCount; i++){
		//Only faces with the same number of edges may be symmetric
		if(MS.FacesInEdge[i].size()>1 &&
		   (MS.Face[MS.FacesInEdge[i][0]].E.size() ==
			MS.Face[MS.FacesInEdge[i][1]].E.size())){

			SymSidesU.push_back(MS.EdgeU[i]);
			SymSidesV.push_back(MS.EdgeV[i]);

			SymSidesFace0.push_back(MS.FacesInEdge[i][0]);
			if(MS.FacesInEdge[i].size()>1)
				SymSidesFace1.push_back(MS.FacesInEdge[i][1]);
			else
				SymSidesFace1.push_back(MS.FacesInEdge[i][0]);

			added= true;

			//Detect collinear edges
			bool collinear= false;
			for (long j=0; j<i; j++){
				if((MS.FacesInEdge[j][0]==MS.FacesInEdge[i][0] &&
					MS.FacesInEdge[i][1]==MS.FacesInEdge[i][1]) ||
				   (MS.FacesInEdge[j][0]==MS.FacesInEdge[i][1] &&
					MS.FacesInEdge[i][1]==MS.FacesInEdge[i][0])){
					//Share two faces

					if(MS.EdgeU[i]!=MS.EdgeU[j] && MS.EdgeU[i]!=MS.EdgeV[j] &&
					   MS.EdgeV[i]!=MS.EdgeU[j] && MS.EdgeV[i]!=MS.EdgeV[j]){  //Do not share vertices
						//Are not consecutive

						if (NearlyCollinear(MS.VertexX[MS.EdgeU[i]], MS.VertexY[MS.EdgeU[i]],
											MS.VertexX[MS.EdgeV[i]], MS.VertexY[MS.EdgeV[i]],
											MS.VertexX[MS.EdgeU[j]], MS.VertexY[MS.EdgeU[j]],
											MS.VertexX[MS.EdgeV[j]], MS.VertexY[MS.EdgeV[j]],
											Collinear_angle, Collinear_offset) > 0){
							collinear= true;
							break;
						}
					}
				}
			}
			SymSidesCollinear.push_back(collinear);
		}
		else{
			info_NonSymmetricEdges++;
		}
	}

	return added;
}


bool CCueSymmetry::EvenSubgraphsAtCorners(CGraphs CG)
//Check that the number of subgraphs sharing each common corner is even

//This is the CCR applied to subgraphs:
//	Each valid circuit must produce an even number of subgraphs at each visited corner

//The function checks only vertices, as corners that are midpoints
//are always connected to two subgraphs
{
	//Check that the number of subgraphs sharing each common vertex is even
	//---------------------------------------------------------------------
	for (long i=0; i<(long)CG.PairedVertices.size(); i++){
		std::vector <bool> Graphs(CG.SubgraphCount, false);
		for (long j=0; j<(long)CG.PairedVertices[i].size(); j++){
			Graphs[CG.SubgraphForVertex[CG.PairedVertices[i][j]]]= true;
		}
		long numgraphs= 0;
		for (long i=0; i<(long)Graphs.size(); i++){
			if(Graphs[i])
				numgraphs++;
		}

		if(numgraphs%2 != 0)
			//Circuit cannot be a symmetry plane
			//as and odd number of subgraphs share the current vertex
			return false;
	}

	return true;
}


bool CCueSymmetry::NewPair(std::vector<std::vector<long>> Pairs, long Subgraph0, long Subgraph1)
//Returns true if the pair (Subgraph0, Subgraph1) is not yet included in Pairs
//Returns false otherwise
{
	//Check if they appear as a pair
	for (long j=0; j<(long)Pairs.size(); j++){
		if((Subgraph0==Pairs[j][0] && Subgraph1==Pairs[j][1]) ||
			(Subgraph0==Pairs[j][1] && Subgraph1==Pairs[j][0]))
			return false;
	}

	return true;
}


bool CCueSymmetry::GetPairs(CGraphs CG, std::vector <std::vector <long>> &Pairs)
//Get pairs of subgraphs

//The function pairs those subgraphs that share common corners

//The function returns false if corners are not well paired
{
	//Pair subgraphs that share paired midpoints
	for (long i=0; i<(long)CG.PairedMidPoints.size(); i++){
		if(CG.PairedMidPoints[i].size() != 2)
			return false;

		long Subgraph0= CG.SubgraphForVertex[CG.PairedMidPoints[i][0]];
		long Subgraph1= CG.SubgraphForVertex[CG.PairedMidPoints[i][1]];
		if(NewPair(Pairs, Subgraph0, Subgraph1)){
			Pairs.push_back(std::vector <long> ());
			Pairs[Pairs.size()-1].push_back(Subgraph0);
			Pairs[Pairs.size()-1].push_back(Subgraph1);
		}
	}

	//Pair subgraphs that share paired vertices
	for (long i=0; i<(long)CG.PairedVertices.size(); i++){
		if(CG.PairedVertices[i].size() != 2)
			return false;

		long Subgraph0= CG.SubgraphForVertex[CG.PairedVertices[i][0]];
		long Subgraph1= CG.SubgraphForVertex[CG.PairedVertices[i][1]];
		if(NewPair(Pairs, Subgraph0, Subgraph1)){
			Pairs.push_back(std::vector <long> ());
			Pairs[Pairs.size()-1].push_back(Subgraph0);
			Pairs[Pairs.size()-1].push_back(Subgraph1);
		}
	}

	return true;
}


bool CCueSymmetry::EqualSubgraphs(std::vector<long> Circuit)
//Checks if circuit breaks down the model into an even number of sub-models
//with exactly the same number of vertices and edges in each paired sub-model

//The function copes with a particular case:
//		Splitting two-connected models through their symmetry planes
//		may produce more than two sub-graphs (this is typical for two-connedged graphs)

//		To determine if the resulting subgraphs are topologically symmetrical,
//		they must be properly paired, before comparing their number of vertices and edges
{
	//Split the graph through circuit
	//-------------------------------
	CModelSymmetry MSaux= MS;
	MSaux.ClearAllFaces();
	CGraphs CG;
	CG.SplitByCircuit(MSaux, Circuit); //Also determines PairedVertices and PairedMidPoints
	CG.DetermineSubgraphs(MSaux);

	//Check that the number of subgraphs is even
	if(CG.SubgraphCount%2 != 0)
		//Circuit cannot be a symmetry plane
		//as it splits the model into an odd number of sub-models
		return false;

	//Check that the number of subgraphs sharing each corner is even
	if(!EvenSubgraphsAtCorners(CG))
		return false;

	//Determine the number of vertices and edges for each subgraph
	//------------------------------------------------------------
	std::vector <long> NumVertices (CG.SubgraphCount, 0);
	for (long i=0; i< MSaux.VertexCount; i++){
		NumVertices[CG.SubgraphForVertex[i]]++;
	}
	std::vector <long> NumEdges (CG.SubgraphCount, 0);
	for (long i=0; i< MSaux.EdgeCount; i++){
		NumEdges[CG.SubgraphForEdge[i]]++;
	}

	//Compare paired subgraphs
	//------------------------
	if(CG.SubgraphCount == 2){
		return (NumVertices[0]==NumVertices[1] && NumEdges[0]==NumEdges[1]);
	}

	else{
		//Check that paired subgraphs may exist
		std::vector <long> SortV= SortMinorFirst(NumVertices);
		std::vector <long> SortE= SortMinorFirst(NumEdges);
		for (long i=0; i<(long)SortV.size(); i=i+2){
			if(SortV[i]!=SortV[i+1] || SortE[i]!=SortE[i+1])
				return false;
		}

		//Pair subgraphs
		std::vector <std::vector <long>> Pairs;
		if(!GetPairs(CG, Pairs))
			return false;

		//Check that all subgraphs have been paired
		if(Pairs.size()!=CG.SubgraphCount/2)
			return false;

		//Check that paired subgraphs have the same size
		for (long i=0; i<(long)Pairs.size(); i++){
			if(NumVertices[Pairs[i][0]]!=NumVertices[Pairs[i][1]] ||
			   NumEdges[Pairs[i][0]]!=NumEdges[Pairs[i][1]])
				return false;
		}

		return true;
	}
}


bool CCueSymmetry::CircuitIsSymmetryPlane(std::vector<long> Circuit)
//Returns true if the circuit is a valid symmetry plane

//Valid symmetry planes must fit the following rule:
//		-Break down the model into an even number of sub-models
//		 which define two groups with exactly the same number of vertices and edges in each group
{
	if(!EqualSubgraphs(Circuit))
		return false;

	return true;
}


bool CCueSymmetry::IntersectPreviousSides(long U, long V, std::vector<long> Circuit)
//Returns true if the current branch intersects any previous side
{
	POINT2D Pu=GetCorner(U);
	POINT2D Pv=GetCorner(V);

	//Test all previous sides
	for(long i=0; i<(long)Circuit.size()-1; i++){
		POINT2D Ci=GetCorner(Circuit[i]);
		POINT2D Cii=GetCorner(Circuit[i+1]);
		if(IntersectInside(Pu, Pv, Ci, Cii, IntersectTolInside, IntersectTolParallel)){
			return true;
		}
	}

	return false;
}


void CCueSymmetry::GetNexts(std::vector<long> &Next,
							long father, bool FatherIsAxis,
							std::vector <bool> FacesVisited,
							std::vector <bool> SymSidesUsed,
							std::vector<long> Circuit)
//Returns the list of "sons" (Sides conected to vertex "father")

//Next sides may not intersect any previous side

//Next sides starting at a vertex must follow the CCR (corner continuation rule):
//   -After visiting one vertex, the number of non-visited edges in this vertex must be even
//    This is because odd edges cannot be divided in two equal groups of edges
//    belonging to oposite halves

//   This rule implies that:
//   -If the number of edges sharing the current vertex is odd,
//    the current end point is a vertex, and the current end side is an axis
//    then the next side must be of the opposite type than the previous side
//   -Otherwise, the next side must be of the same type than the previous side
{
	//Search for continuation sides
	for(long i=0; i<(long)SymSidesU.size(); i++){
		if (!SymSidesUsed[i]){ //Side not yet visited
			//New side must contain "father"
			if (SymSidesU[i]==father || SymSidesV[i]==father){
				info_Branches++;

				//New side must belong to a non-visited face
				if (SymSidesCollinear[i] ||
					(!FacesVisited[SymSidesFace0[i]] && !FacesVisited[SymSidesFace1[i]])){

					//New side cannot end at an already visited corner
					//(Unless when they are closing te circuit)
					if(!IsValueInVector(SymSidesU[i], Circuit) ||
					   !IsValueInVector(SymSidesV[i], Circuit) ||
						SymSidesU[i]==Circuit[0] ||
						SymSidesV[i]==Circuit[0]){

						//New side may not intersect previous sides
						if (!IntersectPreviousSides(SymSidesU[i], SymSidesV[i], Circuit)){

							if (father<0){
								//Mode 1: new side starts at a mid-point
								Next.push_back(i);
								info_ValidBranches++;
							}

							else{
								//Mode 2: new side starts at a vertex
								//(New side must follow the CCR (cornerx continuation rule))
								bool nextIsAxis= (SymSidesFace0[i]==SymSidesFace1[i]);
								bool oddEdges= (MS.EdgesInVertex[father].size()%2 != 0);
								if ( oddEdges && ((FatherIsAxis && !nextIsAxis) ||
												  (!FatherIsAxis && nextIsAxis)) ||
									!oddEdges && ((FatherIsAxis && nextIsAxis) ||
												  (!FatherIsAxis && !nextIsAxis))){
									Next.push_back(i);
									info_ValidBranches++;
								}
								else{
									info_BranchesAtInvalidCCR++;
								}
							}
						}
					}
					else{
						info_BranchesAtVisitedCorners++;
					}
				}
				else{
					info_BranchesAtVisitedFaces++;
				}
			}
		}
	}
}


bool CCueSymmetry::VisitNext(std::vector<long> &Circuit,
							 std::vector<long> &CircuitFaces,
							 std::vector <bool> &FacesVisited,
							 std::vector <bool> &SymSidesUsed)
//Recursively follows the branches that continue the current Circuit 
//(list of consecutive sides defining the contour of a bilateral symmetry plane)

//Returns true if the circuit is closed and defines a valid symmetry plane
{
	//Get candidate branches
	std::vector<long> Next;
	long Father= Circuit[Circuit.size()-1];
	bool FatherIsAxis= (CircuitFaces[CircuitFaces.size()-1] >= 0);
	GetNexts(Next, Father, FatherIsAxis, FacesVisited, SymSidesUsed, Circuit);

	//Explore all candidate branches
	for(long i=0; i<(long)Next.size(); i++){
		if (!SymSidesUsed[Next[i]]){
			//Check the current branch
			if (SymSidesU[Next[i]] == Father)
				Father= SymSidesV[Next[i]];
			else if (SymSidesV[Next[i]] == Father)
				Father= SymSidesU[Next[i]];
			//else
			//	assert(false);
			Circuit.push_back(Father);
			if(SymSidesFace0[Next[i]] == SymSidesFace1[Next[i]])
				CircuitFaces.push_back(SymSidesFace0[Next[i]]);
			else
				CircuitFaces.push_back(-1);
			SymSidesUsed[Next[i]]= true;
			if(!SymSidesCollinear[Next[i]]){
				FacesVisited[SymSidesFace0[Next[i]]]= true;
				FacesVisited[SymSidesFace1[Next[i]]]= true;
			}

			if (Father != Circuit[0]){  //Circuit not yet closed
				if(VisitNext(Circuit, CircuitFaces,
							 FacesVisited, SymSidesUsed))
					return true;
			}
			else{
				info_TestedCircuits++;
				if(CircuitIsSymmetryPlane(Circuit)){
					return true;
				}
			}

			//Erase the already explored branch
			Circuit.erase(Circuit.begin()+Circuit.size()-1);
			CircuitFaces.erase(CircuitFaces.begin()+CircuitFaces.size()-1);
			Father= Circuit[Circuit.size()-1];
			SymSidesUsed[Next[i]]= false;
			if(!SymSidesCollinear[Next[i]]){
				FacesVisited[SymSidesFace0[Next[i]]]= false;
				FacesVisited[SymSidesFace1[Next[i]]]= false;
			}
		}
	}
	return false;
}


void CCueSymmetry::GetSymmetryCircuits()
//Gets all the symmetry circuits of the subgraph NumGraph

//Symmetry planes are circuits of "sides" (axes or edges)
//and "corners" (i.e. midpoints or vertices):
//		-vertices are stored by their numbers
//		-midpoints are stored by the number of the edge they belong to
//		 switched to negative and decreased by one unit
//		 (i.e. midpoint of edge 3 is stored as -4)

//The algorithm assumes that every side may belong to no more than one symmetry plane
//Thus, sides already found to belong to a symmetry plane are removed from the list of candidate sides
{
	info_Branches= 0;
	info_BranchesAtInvalidCCR= 0;
	info_BranchesAtVisitedFaces= 0;
	info_BranchesAtVisitedCorners= 0;
	info_ValidBranches= 0;
	info_TestedCircuits= 0;

	if (SymSidesU.size() <= 0){
		AfxMessageBox("No symmetry sides found");
		return;
	}

	std::vector <bool> FacesVisited;
	std::vector <bool> SymSidesUsed;

	long NumSymSides= (long)SymSidesU.size();

	//Search for circuits
	long NumSide= 0;
	while(NumSide < NumSymSides){
		//Add the endpoints of the first side
		std::vector<long> Circuit;
		Circuit.push_back(SymSidesU[NumSide]);
		Circuit.push_back(SymSidesV[NumSide]);

		//Create a list of flags to distinguish sides that are axes
		std::vector<long> CircuitFaces;
		if(SymSidesFace0[NumSide] == SymSidesFace1[NumSide])
			CircuitFaces.push_back(SymSidesFace0[NumSide]);  //Side is an axis
		else
			CircuitFaces.push_back(-1);  //Side is an edge

		//Define flags of visited sides
		FacesVisited.assign(MS.FaceCount, false);
		SymSidesUsed.assign(NumSymSides, false);
		SymSidesUsed[NumSide]= true;

		if(!SymSidesCollinear[NumSide]){
			FacesVisited[SymSidesFace0[NumSide]]= true;
			FacesVisited[SymSidesFace1[NumSide]]= true;
		}

		//Recursively search for next sides until done (or no-continuation allowed)
		//(If done, saves the valid circuit as a candidate symmetry plane)
		if(VisitNext(Circuit, CircuitFaces, FacesVisited, SymSidesUsed)){

			//Save current circuit as a new symmetry plane
			SymPlanes.push_back(Circuit);
			SymPlanesFaces.push_back(CircuitFaces);

			//Remove sides already used in the current symmetry plane
			for(long i=(long)SymSidesU.size()-1; i>=0; i--){
				if(SymSidesUsed[i]){
					SymSidesCollinear.erase(SymSidesCollinear.begin()+i);
					SymSidesFace1.erase(SymSidesFace1.begin()+i);
					SymSidesFace0.erase(SymSidesFace0.begin()+i);
					SymSidesV.erase(SymSidesV.begin()+i);
					SymSidesU.erase(SymSidesU.begin()+i);
				}
			}

			NumSymSides= (long)SymSidesU.size();
		}
		else
			NumSide++;
	}

	info_Found= (long)SymPlanes.size();
}


double CCueSymmetry::MeritCrossingEdges(std::vector<long> Circuit)
//Evaluates the edges crossing the symetry plane
//that are parallel to each other (i.e. perpendicular to the symmetry plane)

//Merit is the product of the ratio crossing lines/corners
//and a ramp function that depends on the parallelism between the crossed lines

//Difference between the maximum and the minimum angle (dispersion)
//is used to measure parallelism

//Ramp function equals 1 for dispersion smaller or equal than a minimum threshold,
//equals -1 for dispersion greater that the maximum threshold,
//and decreases linearly in between

//Alternatively, checks that all intersected edges
//belong to the same group of parallel lines
//as calculated by CueVanishingPoints
{
	//if(ReferApp.GeneralParams.DetectVanishingPoints){
	//	//Count the edges that belong to the same group of parallel edges
	//	return 0.;
	//}

	//else{
		//Check that angles between all the edges are minor than a threshold
		double minAngle= 0, maxAngle= 0;
		bool first= true;
		long crossing= 0;
		long n= (long)Circuit.size()-1;
		for(long i= 0; i<n; i++){
			if(Circuit[i]<0){
				crossing++;

				long NumEdge= -Circuit[i]-1;
				POINT2D U(MS.VertexX[MS.EdgeU[NumEdge]], MS.VertexY[MS.EdgeU[NumEdge]]);
				POINT2D V(MS.VertexX[MS.EdgeV[NumEdge]], MS.VertexY[MS.EdgeV[NumEdge]]);
				double angle= GetAngle2D_PI(U, V);

				if(first){
					first= false;
					minAngle= angle;
					maxAngle= angle;
				}
				else{
					if(angle<minAngle) minAngle= angle;
					if(angle>maxAngle) maxAngle= angle;
				}
			}
		}

		if(crossing < 2){
			return 0;
		}
		else{
			double dispersion= std::fabs(maxAngle-minAngle);
			if(dispersion < MinParallelCrossing)
				return ((double)crossing/(double)(n));
			else if(dispersion < MaxParallelCrossing)
				return ((double)crossing/(double)(n)) *
						(1.-2*(dispersion-MinParallelCrossing)/
						      (MaxParallelCrossing-MinParallelCrossing));
			else
				return -1.;
		}
	//}
}


void CCueSymmetry::GetPair(long NumFace, long Current,
						   long &next, long &prev, long &size)
//Pairs the previous vertex with the next vertex
//of current vertex of face NumFace

//The function assumes that the list of vertices MS.Face[NumFace].V
//is ordered sequentially
{
	size= (long)MS.Face[NumFace].V.size();

	if(Current >= 0){
		long position;
		IsValueInVector(Current, MS.Face[NumFace].V, position);
		next= position+1;
		prev= position-1;
		size= (size-1)/2;
	}
	else{
		long positionU, positionV;
		IsValueInVector(MS.EdgeU[-Current-1], MS.Face[NumFace].V, positionU);
		IsValueInVector(MS.EdgeV[-Current-1], MS.Face[NumFace].V, positionV);
		if((positionU == 0  && positionV == size-1) ||
		   (positionV == 0  && positionU == size-1) ){
			next= 0;
			prev= size-1;
		}
		else{
			next= max(positionU, positionV);
			prev= min(positionU, positionV);
		}
		size= size/2;
	}
}


double CCueSymmetry::Ramp11Merit(double value, double minmerit, double maxmerit)
//Returns the merit of value calculated through a ramp function:
//		Merit is 1 if value is minor than minmerit
//		Merit is -1 if value is major than maxmerit
//		Otherwise, Merit decreases linearly from 1 up to -1
{
	if(value < minmerit)
		return 1.;
	else if(value > maxmerit)
		return -1.;
	else
		return (1.-2*(value-minmerit)/(maxmerit-minmerit));
}


double CCueSymmetry::MeritAxes(std::vector<long> Circuit, std::vector<long> CircuitFaces)
//Evaluates the degree of symmetry of the axes that belong to the symmetry circuit

//Merit is calculated as the product of the ratio axes/sides
//and the average merit of the axes of the circuit

//The average merit of the cirtuit is:
//		the average value of bad merits if there are the same or more bad than good axes
//		the average value of all merits otherwise

//		0 if none of the sides is an axis
//		-1 if at least two sides are bad quality axis
{
	long n= (long)Circuit.size()-1;

	double BadDistance= 0, BadAngle= 0;
	long numBadDistance= 0, numBadAngle= 0;
	double MeritDistance= 0., MeritAngle= 0.;
	long axes= 0, badaxes= 0;

	//Loop over all the sides of the circuit
	for(long i= 0; i<n; i++){
		if(CircuitFaces[i] >= 0){
			//The side is an axis
			long n= (long)MS.Face[CircuitFaces[i]].V.size();
			if(n <= 3 ||                                    //The face is triangular
			   (n==4 && Circuit[i]>=0 && Circuit[i+1]>=0)){  //The axis is a diagonal of a quadrilateron
				//None distortion can be calculated
				continue;
			}

			double distance= 0, minangle= 0, maxangle= 0;
			bool firstangle= true;

			POINT2D Tail= GetCorner(Circuit[i]);
			POINT2D Head= GetCorner(Circuit[i+1]);

			//Pair the vertices of the face
			long first, last, nn;
			GetPair (CircuitFaces[i], Circuit[i+1], first, last, nn);

			//Calculate distortions of each pair
			for(long j= 0; j<nn; j++){
				long jj= first+j;
				while(jj >= n) jj= jj-n;

				long mj= last-j;
				while(mj < 0) mj= mj+n;

				POINT2D P0= GetCorner(MS.Face[CircuitFaces[i]].V[jj]);
				POINT2D P1= GetCorner(MS.Face[CircuitFaces[i]].V[mj]);

				//Calculate distance distortion
				double d0= GetDistancePointToLine(P0, Tail, Head);
				double d1= GetDistancePointToLine(P1, Tail, Head);
				distance+= std::fabs(d0-d1);

				//Calculate angle distortion
				double angle= GetAngle2D_PI(P0, P1);

				if(firstangle){
					firstangle= false;
					minangle= maxangle= angle;
				}
				else{
					if(angle<minangle) minangle= angle;
					if(angle>maxangle) maxangle= angle;
				}
			}

			//Calculate merits of the side
			double Size= MS.GetFaceSize(CircuitFaces[i]);
			double meritDistanceI= Ramp11Merit(distance/Size, MinSymDistortion, MaxSymDistortion);
			if(meritDistanceI < 0){
				BadDistance+= meritDistanceI;
				numBadDistance++;
			}

			double meritAngleI= Ramp11Merit(std::fabs(maxangle-minangle), 
											MinParallelEdgeSymLines, MaxParallelEdgeSymLines);
			if(meritAngleI < 0){
				BadAngle+= meritAngleI;
				numBadAngle++;
			}

			if(meritDistanceI == -1 || meritAngleI == -1)
				badaxes++;
			axes++;
			MeritDistance+= meritDistanceI;
			MeritAngle+= meritAngleI;
		}
	}

	//Return merit of the circuit
	if(axes <= 0)
		return 0;

	else if(badaxes >= 1)
		return -1.;

	else{
		if(2*numBadDistance >= axes)
			MeritDistance= BadDistance/numBadDistance;
		else
			MeritDistance/= axes;

		if(2*numBadAngle >= axes)
			MeritAngle= BadAngle/numBadAngle;
		else
			MeritAngle/= axes;

		return (double(axes)/(double)n * min(MeritDistance, MeritAngle));
	}
}


double CCueSymmetry::Ramp01Merit(double value, double minmerit, double maxmerit)
//Returns the merit of value calculated through a ramp function:
//		Merit is 0 if value is minor than minmerit
//		Merit is -1 if value is major than maxmerit
//		Otherwise, Merit decreases linearly from 0 up to -1
{
	if(value < minmerit)
		return 0.;
	else if(value > maxmerit)
		return -1.;
	else
		return (-(value-minmerit)/(maxmerit-minmerit));
}


double CCueSymmetry::MeritSymEdges(std::vector<long> Circuit)
//Evaluates the degree of symmetry of the edges that belong to the symmetry circuit

//Determines the parallelism between face symmetry lines
//(Face symmetry lines connect paired vertices of tentative symmetric faces)

//Dispersion is calculated for each edge
//as the angle between the two face symmetry lines
//that diverge more from each other

//Merit for each edge is calculated as:
//		0  if its dispersion is within a minimum threshold
//		-1 if its dispersion is greater than a maximum threshold
//		Decreases linearly from 0 to -1 in between minimum and maximum threshold

//Merit of the circuit is calculated as the average merit of all the edges
//weighted by the ratio of sides of the circuit that are edges
{
	long symmetricSides= 0;
	long n= (long)Circuit.size()-1;
	long badedges= 0;

	//Loop over all the edges of the circuit
	double MaxAngle= 0.;
	double MeritAngle= 0., edges= 0.;

	for(long i= 0; i<n; i++){
		double maxdif= 0.;

		if(Circuit[i]>=0 && Circuit[i+1] >= 0){
			long edgeI= MS.GetEdge(Circuit[i], Circuit[i+1]);
			if(edgeI >= 0){
				long face0= MS.FacesInEdge[edgeI][0];
				long face1= MS.FacesInEdge[edgeI][1];

				if(MS.Face[face0].V.size() != MS.Face[face1].V.size())
					return -1.;  //Topologically non-symmetric faces for one edge

				//At least two face-symmetry lines required
				long nn= (long)MS.Face[face0].V.size();
				if(nn <= 3)
					continue;

				//Pair the edge shared by the two faces
				long position0, delta0;
				SearchInList(MS.Face[face0].V, Circuit[i], Circuit[i+1], position0, delta0);
				long position1, delta1;
				SearchInList(MS.Face[face1].V, Circuit[i], Circuit[i+1], position1, delta1);

				//Compare the orientations of the face-symmetry lines
				double minangle= 0, maxangle= 0;
				bool firstangle= true;

				//Calculate dispersion
				for(long j= 2; j<nn; j++){
					long jj0= position0+j*delta0;
					while(jj0 < 0) jj0= jj0+nn;
					while(jj0 >= nn) jj0= jj0-nn;

					long jj1= position1+j*delta1;
					while(jj1 < 0) jj1= jj1+nn;
					while(jj1 >= nn) jj1= jj1-nn;

					if(MS.VertexX[MS.Face[face0].V[jj0]] == MS.VertexX[MS.Face[face1].V[jj1]])
						//Vertex of a colinear edge
						continue;

					//Calculate angle distortion
					POINT2D P0 (MS.VertexX[MS.Face[face0].V[jj0]], MS.VertexY[MS.Face[face0].V[jj0]]);
					POINT2D P1 (MS.VertexX[MS.Face[face1].V[jj1]], MS.VertexY[MS.Face[face1].V[jj1]]);
					double angle= GetAngle2D_PI(P0, P1);

					if(firstangle){
						firstangle= false;
						minangle= angle;
						maxangle= angle;
					}
					else{
						if(angle<minangle) minangle= angle;
						if(angle>maxangle) maxangle= angle;
					}
				}

				//Calculate merit of the i-th side
				double meritI= Ramp01Merit(std::fabs(maxangle-minangle), 
										   MinParallelEdgeSymLines,
										   MaxParallelEdgeSymLines);
				if(meritI == -1)
					badedges++;
				MeritAngle+= meritI;
				edges++;
			}
		}
	}

	//Return the merit of the circuit
	if(edges <= 0)
		return 0;

	else if(badedges >= 1)
		return -1.;

	else{
		MeritAngle/= edges;
		return (double(edges)/(double)n * MeritAngle);
	}
}


double CCueSymmetry::MeritVertical(std::vector<long> Circuit)
//Evaluates the verticality of the symetry plane

//Merit is:
//		1 if half the sides are vertical (they are within a minimum threshold)
//		0 if none of the sides is vertical (they are outside a maximum threshold)
//		Decreases linearly from 1 to 0 in between minimum and maximum threshold
{
	double AverageVertical= 0.;

	long n= (long)Circuit.size()-1;

	for (long i=0; i<n; i++){
		POINT2D U;
		long NodeI= Circuit[i];
		if(NodeI >= 0){
			U.x= MS.VertexX[NodeI];
			U.y= MS.VertexY[NodeI];
		}
		else{
			NodeI= -NodeI-1;
			U.x= (MS.VertexX[MS.EdgeU[NodeI]] + MS.VertexX[MS.EdgeV[NodeI]])/2;
			U.y= (MS.VertexY[MS.EdgeU[NodeI]] + MS.VertexY[MS.EdgeV[NodeI]])/2;
		}

		POINT2D V;
		long NodeII= Circuit[i+1];
		if(NodeII >= 0){
			V.x= MS.VertexX[NodeII];
			V.y= MS.VertexY[NodeII];
		}
		else{
			NodeII= -NodeII-1;
			V.x= (MS.VertexX[MS.EdgeU[NodeII]] + MS.VertexX[MS.EdgeV[NodeII]])/2;
			V.y= (MS.VertexY[MS.EdgeU[NodeII]] + MS.VertexY[MS.EdgeV[NodeII]])/2;
		}

		double deviation= std::fabs(NUMBER_PI/2. - GetAngle2D_PI(U, V));
		if(deviation < minVerticalThreshold)
			AverageVertical+= 1.;
		else if(deviation < maxVerticalThreshold)
			AverageVertical+= (1-(deviation-minVerticalThreshold)/
								 (maxVerticalThreshold-minVerticalThreshold));
	}

	return (min(1., AverageVertical/(double)(n/2.)));  //min us used just in case more than half of the sides are vertical
}


double CCueSymmetry::MeritHorizontal(std::vector<long> Circuit)
//Evaluates the horizontality of the symetry plane

//Merit is:
//		1 if all the crossing edges are vertical (they are within a minimum threshold)
//		0 if no crossing edges exist, or they are not vertical (they are outside a maximum threshold)
//		Decreases linearly from 1 to 0 in between minimum and maximum threshold
{
	double AverageHorizontal= 0;

	long n= (long)Circuit.size()-1;

	for (long i=0; i<n; i++){
		if(Circuit[i] < 0){
			long CrossingI= -Circuit[i]-1;
			POINT2D U (MS.VertexX[MS.EdgeU[CrossingI]], MS.VertexY[MS.EdgeU[CrossingI]]);
			POINT2D V (MS.VertexX[MS.EdgeV[CrossingI]], MS.VertexY[MS.EdgeV[CrossingI]]);

			double deviation= std::fabs(NUMBER_PI/2. - GetAngle2D_PI(U, V));
			if(deviation < minVerticalThreshold)
				AverageHorizontal+= 1.;
			else if(deviation < maxVerticalThreshold)
				AverageHorizontal+= (1-(deviation-minVerticalThreshold)/(maxVerticalThreshold-minVerticalThreshold));
		}
	}

	return (AverageHorizontal/(double)(n));
}


void CCueSymmetry::reorder()
//Reorders vectors SymMerits, SymPlanes and SymPlanesFaces
//according to the order in SymMerits
{
	//bubble sort!
	long n= (long)SymMerits.size();
	for (long i= 0; i<n-1; i++){
		for (long j= 0; j<n-1-i; j++){
			if (SymMerits[j] < SymMerits[j+1]){
				double swap = SymMerits[j];
				SymMerits[j] = SymMerits[j+1];
				SymMerits[j+1] = swap;

				std::vector<long> copy1 = SymPlanes[j];
				SymPlanes[j] = SymPlanes[j+1];
				SymPlanes[j+1] = copy1;

				//No need to reorder as this information is not further used
				//std::vector<long> copy2 = SymPlanesFaces[j];
				//SymPlanesFaces[j] = SymPlanesFaces[j+1];
				//SymPlanesFaces[j+1] = copy2;
			}
		}
	}
}


void CCueSymmetry::GetMerits()
//Calculates the merits of the symmetry planes of the current SubGraph,
//removes those symmetry planes with merits below a minimum threshold
//and reorders the valid symmetry planes from best to worst
{
	info_RemovedCircuits= 0;

	SymMerits.assign((long)SymPlanes.size(), 0.);

	//Calculate merits
	//----------------
	for (long i=0; i<(long)SymPlanes.size(); i++){
		double Merit1= 0., Merit2= 0., Merit3= 0., Merit4= 0., Merit5= 0.;

		//Parallelism between crossing edges
		if(bDetectNormalCrossing){
			Merit1= MeritCrossingEdges(SymPlanes[i]);
		}
		if(Merit1 == -1){
			SymMerits[i]= -1.;
			continue;
		}

		//Skewed symmetry of axes
		if(bDetectDistorted){
			Merit2= MeritAxes(SymPlanes[i], SymPlanesFaces[i]);
		}
		if(Merit2 == -1){
			SymMerits[i]= -1.;
			continue;
		}

		//Edges sharing symmetrical faces
		if(bSymmetricalEdges){
			Merit3= MeritSymEdges(SymPlanes[i]);
		}
		if(Merit3 == -1){
			SymMerits[i]= -1.;
			continue;
		}

		//Vertical and horizontal planes
		if(bPrioritizeVertical){
			Merit4= MeritVertical(SymPlanes[i]);
			Merit4= max (Merit4, MeritHorizontal(SymPlanes[i]));
		}

		if(Merit1>=MinMerit &&
		   Merit2>=MinMerit &&
		   Merit3>=MinMerit &&
		   Merit4>=MinMerit){
			SymMerits[i]= Merit1 * MeritWeight_1 +
						  Merit2 * MeritWeight_2 +
						  Merit3 * MeritWeight_3 +
						  Merit4 * MeritWeight_4;
		}
		else{
			if(Merit1 < 0)
				SymMerits[i]+= Merit1 * MeritWeight_1;
			if(Merit2 < 0)
				SymMerits[i]+= Merit2 * MeritWeight_2;
			if(Merit3 < 0)
				SymMerits[i]+= Merit3 * MeritWeight_3;
			if(Merit4 < 0)
				SymMerits[i]+= Merit4 * MeritWeight_4;
		}
	}

	//Remove if quality is unnaceptable
	//---------------------------------
	if(bRemoveBadMerit){
		for (long i=0; i<(long)SymPlanes.size(); i++){
			if(SymMerits[i] < MinMerit){
				SymPlanes.erase(SymPlanes.begin()+i);
				//SymPlanesFaces.erase(SymPlanesFaces.begin()+i); //No longer used
				SymMerits.erase(SymMerits.begin()+i);
				i--;
				info_RemovedCircuits++;
			}
		}
	}

	//Reorder symmetry planes
	//-----------------------
	reorder();
}


void CCueSymmetry::GetBilateralSymmetryPlanes(CDB_Model *DDBB)
//Returns the symmetry planes found in the model

//The approach detects the skewed axes of symmetry for each polygonal face
//Then searches for closed sequences of "sides"
//(polygon edges plus symmetry axes)
//delimiting single three-dimensional planes of bilateral symmetry

//The list of symmetry planes, together with their merits is returned

//Based on:
//    Plumed R., Company P. and Varley P.A.C.
//    "Detecting mirror symmetry in single-view wireframe sketches of polyhedral shapes"
//    Computers & Graphics, Vol. 59. pp. 1-12, 2016
{
	if (DDBB->GetSizeFaces() == 0){
		AfxMessageBox("Faces required to detect symmetry");
		return;
	}

	Input(DDBB);

	OutputReset(MS_main.FaceCount);

	CGraphs CG;
	CG.DetermineSubgraphs(MS_main);
	CG.DetermineFaceSubgraphs(MS_main);

	//Loop over all subgraphs to detect their symmetry planes
	for(long NumGraph= 0; NumGraph<CG.SubgraphCount; NumGraph++){
		//Local memory for mappings required to link the symmetry planes to the whole model
		std::vector <long> ReverseMapVertices;
		std::vector <long> ReverseMapEdges;
		std::vector <long> ReverseMapFaces;

		//Load current subgraph
		GetSubGraph(CG, NumGraph, ReverseMapVertices, ReverseMapEdges, ReverseMapFaces);

		//Detect candidate symmetry axes for all the faces of the current subgraph
		FaceAxesU.assign(MS.FaceCount, std::vector<long>());
		FaceAxesV.assign(MS.FaceCount, std::vector<long>());
		GetSkewAxes();

		//Add sides to the side list
		SymSidesU.clear();
		SymSidesV.clear();
		SymSidesFace0.clear();
		AddAxesToSidesList();
		AddEdgesToSidesList();

		//Get global symmetry circuits of the current subgraph
		SymPlanes.clear();
		SymPlanesFaces.clear();
		GetSymmetryCircuits();

		//Calculate merits of the symmetry planes
		SymMerits.clear();
		GetMerits();

		//Save symmetry planes of the current subgraph
		Output(NumGraph, ReverseMapVertices, ReverseMapEdges, ReverseMapFaces);

		//Output performance data
		//PrintInfo();
	}
}
