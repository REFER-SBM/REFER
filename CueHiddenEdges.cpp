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
#include <bitset>

#include "Tools_Vector.h"
#include "Tools_Geometry.h"

#include "DB_Model.h"
#include "Refer.h"
#include "resource.h"

#include "DialogHiddenEdges.h"

#include "CueHiddenEdges.h"


bool CCueHiddenEdges::InputModel(CDB_Model *DDBB)
//The input required by the algorithms is:
//		VertexCount= number of vertex in the line drawing
//		Vertex[i].x= X coordinate of the i-th vertex
//		Vertex[i].y= Y coordinate of the i-th vertex

//		EdgeCount= number of edges in the line drawing
//		EdgeHead[i]= Head vertex defining the i-th edge
//		EdgeTail[i]= Tail vertex defining the i-th edge

//		FaceCount= number of faces found
//		Face[i].C= number of edges in face i
//		Face[i].E[j]= j-th edge in face i
//		Face[i].V[j]= j-th vertex in face i

//		Contour[i]= i-th edge of the contour
{
	//Copy external vertices into local database
	Vertex.clear();
	VertexCount= DDBB->GetSizeVertices();
	for (long i=0; i<VertexCount; i++){
		CVertex v= DDBB->GetVertex(i);
		POINT2D p(v.x, v.y);
		Vertex.push_back(p);
	}

	//Copy external edges into local database
	EdgeHead.clear();
	EdgeTail.clear();
	EdgeCount= DDBB->GetSizeEdges();
	for (long i=0; i<EdgeCount; i++){
		CEdge e= DDBB->GetEdge(i);
		EdgeHead.push_back(e.Head);
		EdgeTail.push_back(e.Tail);
	}

	//Check if faces have been calculated
	if (DDBB->GetSizeFaces() == 0){
		AfxMessageBox("No faces found, hence all the edges have been labelled as visible");
		return false;
	}

	//Copy external faces into local database
	FaceCount= DDBB->GetSizeFaces();
	for (long i=0; i<FaceCount; i++){
		CFace faceI= DDBB->GetFace(i);

		FACE f;
		f.C= faceI.GetSizeEdgesInFace();
		Face.push_back(f);

		std::vector<long> vertices= DDBB->GetAllVerticesInFace(faceI);
		for (long j=0; j<Face[i].C; j++){
			Face[i].V.push_back(vertices[j]);
		}

		for (long j=0; j<Face[i].C; j++){
			Face[i].E.push_back(faceI.GetEdgeInFace(j));
		}
	}

	//Check if the apparent contour has been calculated
	if(ReferApp.Cu.Pe.PerimeterEdges.size() == 0){
		AfxMessageBox("No perimeter,\r\n hence all the edges have been labelled as visible");
		return false;
	}

	//Warning: Refer calculates contour of multigraph models
	//But hidden edges are only calculated for single graph models
	PerimeterEdges= ReferApp.Cu.Pe.PerimeterEdges[0];
	PerimeterVertices= ReferApp.Cu.Pe.PerimeterVertices[0];

	return true;
}


std::vector<long> CCueHiddenEdges::GetAllEdgesInVertex(long iVertex)
// Returns the edges connected to the vertex "iVertex"
{
	std::vector<long> Destination;
	for(long i= 0; i<EdgeCount; i++){
		if(EdgeHead[i]==iVertex || EdgeTail[i]==iVertex)
			Destination.push_back(i);
	}
	return Destination;
}


bool CCueHiddenEdges::IsInternalToFace(long numFace, double PointX, double PointY)
//Returns true if point (PointX, PointY) is internal to face numFace,
//and false otherwise

//Based on:
//    Copyright © 1994-2006, W Randolph Franklin (WRF)
//    You may use my material for non-profit research and education,
//    provided that you credit me, and link back to my home page.
//    https://www.ecse.rpi.edu/Homepages/wrf/Research/Short_Notes/pnpoly.html
{
	bool inside = false;

	long n= Face[numFace].C;
	long i, j;
	for (i = 0, j = n-1; i < n; j = i++) {
		long ii= Face[numFace].V[i];
		long jj= Face[numFace].V[j];
		if ( ((Vertex[ii].y>PointY) != (Vertex[jj].y>PointY)) &&
		     (PointX < (Vertex[jj].x-Vertex[ii].x) *
			           (PointY-Vertex[ii].y) /
			           (Vertex[jj].y-Vertex[ii].y) + Vertex[ii].x) )
			inside = !inside;
	}

	return inside;
}


void CCueHiddenEdges::GetInterceptions(long CurrentEdge,
									   std::vector<long> &InterceptedEdges)
// Finds the edges that intersect with the edge CurrentEdge

// Returns the list of edges intersected in InterceptedEdges
// Also returns the (x,y) coordinates of the intersection points
{
	for (long i=0; i<EdgeCount; i++) if(i != CurrentEdge){
		//Exclude edges that share vertices
		if(EdgeHead[i]==EdgeHead[CurrentEdge] || EdgeHead[i]==EdgeTail[CurrentEdge] ||
		   EdgeTail[i]==EdgeHead[CurrentEdge] || EdgeTail[i]==EdgeTail[CurrentEdge])
			continue;

		//Find intersection with i-th edge
		double x, y;
		if(SegmentsIntersect(Vertex[EdgeHead[CurrentEdge]], Vertex[EdgeTail[CurrentEdge]],
							 Vertex[EdgeHead[i]], Vertex[EdgeTail[i]],x, y)){
			InterceptedEdges.push_back(i);
		}
	}
}


void CCueHiddenEdges::GetInterceptions(long CurrentEdge,
									   std::vector<long> &InterceptedEdges,
									   std::vector<double> &XIntersections,
									   std::vector<double> &YIntersections)
// Finds the edges that intersect with the edge CurrentEdge

// Returns the list of edges intersected in InterceptedEdges
// Also returns the (x,y) coordinates of the intersection points
{
	for (long i=0; i<EdgeCount; i++) if(i != CurrentEdge){
		//Exclude edges that share vertices
		if(EdgeHead[i]==EdgeHead[CurrentEdge] || EdgeHead[i]==EdgeTail[CurrentEdge] ||
		   EdgeTail[i]==EdgeHead[CurrentEdge] || EdgeTail[i]==EdgeTail[CurrentEdge])
			continue;

		//Find intersection with i-th edge
		double x, y;
		if(SegmentsIntersect(Vertex[EdgeHead[CurrentEdge]], Vertex[EdgeTail[CurrentEdge]],
							 Vertex[EdgeHead[i]], Vertex[EdgeTail[i]], x, y)){
			InterceptedEdges.push_back(i);
			XIntersections.push_back(x);
			YIntersections.push_back(y);
		}
	}
}


void CCueHiddenEdges::ChunkPartiallyHiddenEdges(CDB_Model* DDBB)
//Edges currently labelled as hidden are revisited to determine if some chunks are visible

//Find partially hidden edges
{
	int VertexCountAux= VertexCount;
	std::vector <POINT2D> VertexAux= Vertex;

	int EdgeCountAux= EdgeCount;
	std::vector <long> EdgeHeadAux= EdgeHead;
	std::vector <long> EdgeTailAux= EdgeTail;

	CVerticesSet ListaVerticesAux= DDBB->GetSetOfVertices();
	CEdgesSet ListaAristasAux= DDBB->GetSetOfEdges();

	//Loop over all hidden edges to find partially hidden edges
	for (long i=0; i<EdgeCount; i++) if (Visible[i] == HIDDEN){
		//Get intersecting edges and their intersection points
		std::vector<long> InterceptedEdges;
		std::vector<double> XInt;
		std::vector<double> YInt;
		GetInterceptions(i, InterceptedEdges, XInt, YInt);

		//Remove the intersections produced by intersecting edges that are hidden
		for(long ii=0; ii<(long)InterceptedEdges.size(); ii++){
			if (Visible[InterceptedEdges[ii]] == HIDDEN){
				InterceptedEdges.erase(InterceptedEdges.begin()+ ii);
				XInt.erase(XInt.begin()+ ii);
				YInt.erase(YInt.begin()+ ii);
				ii--;
			}
		}

		//Skip if none intersection remains
		if (InterceptedEdges.size() == 0)
			continue;

		//Order intersections from minor to major x coordinate
		//(orders Y coordinates first, to get ordered those
		//intersection point where the X coordinate is the same)
		SortMinorFirst(YInt, XInt, InterceptedEdges);
		SortMinorFirst(XInt, YInt, InterceptedEdges);

		//Añado ordenadamente el vertice cabeza o cola de la arista evaluada.
		//Solo aquel en el que todas las aristas que llegan son visibles
		bool headIsVisible= false;
		long index= 0;
		for(long ii=0; ii<EdgeCount; ii++){
			if (Visible[ii] == VISIBLE &&
				(EdgeHead[ii]==EdgeHead[i] || EdgeTail[ii]==EdgeHead[i])){
				headIsVisible= true;
				index= ii;
				break;
			}
		}

		//Si todas las aristas que llegan al vertice cabeza son discontinuas
		//añado el vertice cola, si no añado el vertice cabeza
		long IndiceVerticeAnyadido, IndiceVerticeRestante;

		POINT2D V1;
		if (headIsVisible){
			IndiceVerticeAnyadido= EdgeHead[i];
			IndiceVerticeRestante= EdgeTail[i];
			V1= Vertex[EdgeHead[i]];
		}
		else{
			IndiceVerticeAnyadido= EdgeTail[i];
			IndiceVerticeRestante= EdgeHead[i];
			V1= Vertex[EdgeTail[i]];
		}

		POINT2D V2 (XInt[0], YInt[0]);

		bool InsertFirst;

		if (std::fabs(V2.x-V1.x) < 0.001){
			if (V1.y < V2.y){
				InsertFirst= true;
				InterceptedEdges.insert(InterceptedEdges.begin(), index);
				XInt.insert(XInt.begin(), V1.x);
				YInt.insert(YInt.begin(), V1.y);

			}
			else{
				InsertFirst= false;
				InterceptedEdges.push_back(index);
				XInt.push_back(V1.x);
				YInt.push_back(V1.y);
			}
		}
		else{
			if (V1.x < V2.x){
				InsertFirst= true;
				InterceptedEdges.insert(InterceptedEdges.begin(), index);
				XInt.insert(XInt.begin(), V1.x);
				YInt.insert(YInt.begin(), V1.y);
			}
			else{
				InsertFirst= false;
				InterceptedEdges.push_back(index);
				XInt.push_back(V1.x);
				YInt.push_back(V1.y);
			}
		}

		//Entre cada dos puntos interceptados miro si la arista sería continua
		//o discontinua
		for (long j=0; j<(long)InterceptedEdges.size()-1; j++){
			POINT2D middle;
			middle.x= XInt[j]+(XInt[j+1]-XInt[j])/2;
			middle.y= YInt[j]+(YInt[j+1]-YInt[j])/2;

			//Check if middle point is inside any visible face
			bool isInside=false;
			for(long k=0; k<FaceCount; k++){
				bool faceVisible=true;
				for(long kk=0; kk<(long)Face[k].E.size(); kk++){
					if (Visible[Face[k].E[kk]] == HIDDEN){
						faceVisible=false;
						break;
					}
				}

				if (faceVisible){
					isInside= IsInternalToFace(k, middle.x, middle.y);
				}

				if(isInside)
					break;
			}

			//if (j == 0){
				//if (InsertFirst){
				//	CVertex Vertice22(XInt[j+1], YInt[j+1], 0);
				//	ListaVerticesAux.AddVertex(Vertice22);
				//	CEdge edge;
				//	if (IndiceVerticeAnyadido==edge.Head)
				//		edge.Tail= ListaVerticesAux.GetSize()-1;
				//	else
				//		edge.Head= ListaVerticesAux.GetSize()-1;
				//	if (!isInside)
				//		edge.Style= VISIBLE;
				//	ListaAristasAux.SetEdge(i, edge);
				//}
				//else{
				//	CVertex Vertice11(XInt[j], YInt[j], 0);
				//	ListaVerticesAux.AddVertex(Vertice11);
				//	CEdge edge2;
				//	edge2.Style=HIDDEN;
				//	edge2.Head=IndiceVerticeRestante;
				//	edge2.Tail= ListaVerticesAux.GetSize()-1;
				//	ListaAristasAux.SetEdge(i, edge2);

				//	edge2.Head= ListaVerticesAux.GetSize()-1;
				//	if (j != XInt.size()-2){
				//		CVertex Vertice22(XInt[j+1], YInt[j+1], 0);
				//		ListaVerticesAux.AddVertex(Vertice22);
				//		edge2.Tail= ListaVerticesAux.GetSize()-1;
				//	}
				//	else
				//		edge2.Tail=IndiceVerticeAnyadido;
				//	if (!isInside)
				//		edge2.Style=VISIBLE;
				//	else
				//		edge2.Style=HIDDEN;
				//	ListaAristasAux.AddEdge(edge2);
				//}
			//}

			//else{
////				if (j == InterceptedEdges.size()-1){
//				if (j == InterceptedEdges.size()-2){
//					if (!InsertFirst){
//						CEdge edgei= DDBB->GetEdge(i);
//						if (IndiceVerticeAnyadido==edgei.Head)
//							edgei.Tail= ListaVerticesAux.GetSize()-1;
//						else
//							edgei.Head= ListaVerticesAux.GetSize()-1;
//						if (!isInside)
//							edgei.Style=VISIBLE;
////						ListaAristasAuxSetEdge(IndiceArista, &Arista);
//						ListaAristasAux.AddEdge(edgei);
//					}
//					else{
//						CEdge edge2;
//						edge2.Head= ListaVerticesAux.GetSize()-1;
//						edge2.Tail=IndiceVerticeRestante;
//						edge2.Style=HIDDEN;
//						ListaAristasAux.AddEdge(edge2);
//					}
//				}
//				else{
//					CVertex V22(XInt[j+1], YInt[j+1], 0);
//					ListaVerticesAux.AddVertex(V22);
//					CEdge edge2;
//					edge2.Head= ListaVerticesAux.GetSize()-1;
//					edge2.Tail= ListaVerticesAux.GetSize()-2;
//					if (!isInside)
//						edge2.Style=VISIBLE;
//					else
//						edge2.Style=HIDDEN;
//					ListaAristasAux.AddEdge(edge2);
//				}
			//}
		}

		//if (InsertFirst){
		//	CEdge edge2;
		//	edge2.Head= ListaVerticesAux.GetSize()-1;
		//	edge2.Tail=IndiceVerticeRestante;
		//	edge2.Style=HIDDEN;
		//	ListaAristasAux.AddEdge(edge2);
		//}
	}

	DDBB->UpdateSetOfVertices(ListaVerticesAux);
	DDBB->UpdateSetOfEdges(ListaAristasAux);
}


////////////////////////////////////////////////////////////////////////////////


bool CCueHiddenEdges::HiddenEdgeInPerimeter()
//Checks if the current Variation of hidden edges fits the following criterion:
//		Variation is invalid if at least one hidden edge belongs to the apparent contour
//		unless it also intersects another edge of the apparent contour

//True is returned if the Variation is found invalid
{
	//Search hidden edges in the contour
	for(long i=0; i<(long)PerimeterEdges.size(); i++){
		if (Visible[PerimeterEdges[i]] == HIDDEN){
			bool invalid= true;

			//Check if intersects another edge of the contour
			std::vector<long> InterceptedEdges;
			GetInterceptions(PerimeterEdges[i], InterceptedEdges);

			for(long j=0; j<(long)InterceptedEdges.size(); j++){
				if (IsValueInVector(InterceptedEdges[j], PerimeterEdges)){
					invalid= false;
					break;
				}
			}

			if (invalid)
				//Current contour edge is hidden
				//and does intersect another edge of the contour
				return true;
		}
	}

	//None incompatibility was found
	return false;
}


bool CCueHiddenEdges::VisibleEdgesIntersect()
//Checks if the current Variation of hidden edges fits the following criterion:
//		Variation is invalid if at least two visible edges intersect to each other

//True is returned if the Variation is found invalid
{
	//Loop over all the (visible) edges
	for(long i=0; i<EdgeCount; i++){
		if (Visible[i] == VISIBLE){
			std::vector<long> InterceptedEdges;
			GetInterceptions(i, InterceptedEdges);

			for (long j=0; j<(long)InterceptedEdges.size(); j++){
				if (Visible[InterceptedEdges[j]] == VISIBLE){
					//At least two visible edges intersect to each other
					return true;
				}
			}
		}
	}

	//No visible edges intersect to each other
	return false;
}


bool CCueHiddenEdges::FaceIsVisible(long numFace)
//Return true if the current face is visible
//(All their edges must be visible)
{
	for(long i= 0; i<(long)Face[numFace].E.size(); i++){
		if (Visible[Face[numFace].E[i]] == HIDDEN){
			return false;
		}
	}
	return true;
}


bool CCueHiddenEdges::VisibleVertexInsideVisibleFace()
//Checks if the current Variation of hidden edges fits the following criterion:
//		Vertices located inside a visible faces must be hidden
//		(cannot be connected to visible edges)

//If, at least, one edge connected to an internal vertex is visible,
//the variation is declared invalid (flag true is returned)
{
	//Loop over all the (visible) faces
	for (long i=0; i<FaceCount; i++){
		if (FaceIsVisible(i)){
			//Check vertices not belonging to the face
			//but located inside it
			for(long j=0; j<VertexCount; j++){
				if (!IsValueInVector(j, Face[i].V)){
					if(IsInternalToFace(i, Vertex[j].x, Vertex[j].y)){
						std::vector<long> EdgesInJ= GetAllEdgesInVertex(j);
						for(long k=0; k<(long)EdgesInJ.size(); k++){
							if (Visible[EdgesInJ[k]] == VISIBLE){
								//One invalid edge was found
								return true;
							}
						}
					}
				}
			}
		}
	}

	//None incompatibility was found
	return false;
}


bool CCueHiddenEdges::HiddenEdgeConnectedToVisibleEdges()
//Checks if the current variation of hidden edges fits the following criterion:
//		If hidden edges do not intersect any visible edge
//		at least one of their vertices must be hidden
//		(all the edges connected to, at least, one of their vertices must be hidden)

//True is returned if the Variation is found invalid
{
	//Loop over all the (hidden) edges
	for (long i=0; i<EdgeCount; i++){
		if (Visible[i] == HIDDEN){
			//Get interceptions
			std::vector<long> InterceptedEdges;
			GetInterceptions(i, InterceptedEdges);

			//Remove the intersected edges that are hidden
			for (long j=(long)InterceptedEdges.size()-1; j>=0; j--){
				if (Visible[j] == HIDDEN){
					InterceptedEdges.erase(InterceptedEdges.begin()+ j);
					j--;
				}
			}

			//Skip if hidden edge does not intersect any visible edge
			if(InterceptedEdges.size()==0)
				continue;

			//Return the Variation as invalid if both vertices
			//of the current hidden edge are visible (i.e. connected to some visible edge)
			bool ConnectedToVisible= false;
			std::vector<long> EdgesInHeadI= GetAllEdgesInVertex(EdgeHead[i]);
			for(long k= 0; k<(long)EdgesInHeadI.size(); k++){
				if (Visible[EdgesInHeadI[k]] == VISIBLE){
					//The first vertex is connected to one visible edge
					ConnectedToVisible= true;
					break;
				}
			}

			if (ConnectedToVisible){
				std::vector<long> EdgesInTailI= GetAllEdgesInVertex(EdgeTail[i]);
				for(long k= 0; k<(long)EdgesInTailI.size(); k++){
					if (Visible[EdgesInTailI[k]] == VISIBLE){
						//Both vertices are connected to visible edges
						return true;
					}
				}
			}
		}
	}
	//None incompatibility was found
	return false;
}


long CCueHiddenEdges::GetNumHiddenEdges()
//Get number of hidden edges
{
	long numHidden= 0;
	for(long i=0; i<EdgeCount; i++){
		if(Visible[i] == HIDDEN)
			numHidden++;
	}

	return numHidden;
}


bool CCueHiddenEdges::GetHiddenByFaceVariations(CDB_Model *DDBB)
//Exhaustive search of all posible variations with repetition
//of visible/hidden faces
//Every candidate solution is named as a "Variation"
//and is accepted as valid if, and only if, accomplishes some heuristic rules

//For each Variation of faces, edges occluded by visible faces are declared hidden
//This is done as follows:

//    Varying one face as visible means that all its edges are labeled as visible
//    Otherwise, they are labeled as hidden

//    Variations where one face is declared hidden
//    while all the surrounding faces are declared visible are repetitive
//    (as their edges are finally declared visible)

//    Variation is invalid if does not fit, at least, one of the following rules:
//    - Edges of the perimeter cannot be hidden
//      (unless they also intersect another edge of the perimeter)
//    - Visible edges cannot intersect another visible edge
//    - Vertices located inside a visible faces must be hidden
//      (cannot be connected to visible edges)
//    - If hidden edges do not intersect any visible edge
//      at least one of their vertices must be hidden
//      (all the edges connected to, at least, one of their vertices must be hidden)

//The approach does not generally find one single valid solution
//Thus, one of the valid solutions is selected automatically or interactively
//(depending on the flag HiddenEdgesAutomatic)

//This approach requires perimeter to be previously determined
{
	//Select one Necker reversal by selecting front faces as visible
	std::vector<long> numFrontFaces= GetFrontFaces();

	std::vector<LINE_VISIBILITY> VisibleFrontFace(EdgeCount, HIDDEN);
	for (long i=0; i<(long)numFrontFaces.size(); i++){
		for(long j=0; j<Face[numFrontFaces[i]].C; j++){
			VisibleFrontFace[Face[numFrontFaces[i]].E[j]]= VISIBLE;
		}
	}

	//Get the size of the set of variations
	//variations of two alternatives (visible-hidden) taken in groups of number of faces
	long NumVariations= (long)pow(2.0, FaceCount);

	const int bitfacesize= 1024;   //Increase the size of facesize if required
	if (FaceCount > bitfacesize){
		AfxMessageBox("Not enough memory allocated to calculate hidden faces");
		Visible.clear();
		return false;
	}

	const int bitedgesize= 1024;   //Increase the size of edgesize if required
	if (EdgeCount > bitedgesize){
		AfxMessageBox("Not enough memory allocated to calculate hidden edges");
		Visible.clear();
		return false;
	}

	//Check the variations for compatibility
	//and save the valid Variations
	std::vector<long> ValidFaceVariations;
	std::vector<long> NumHiddenLines;  //Used to sort valid solutions

	std::vector<long> VisitedEdgeVariations;

	for (long v=0; v<NumVariations; v++){
		Visible= VisibleFrontFace; //Set visible all the edges of the front faces

		//Update visibility of lines in the database, following the current variation of faces
		std::bitset<bitfacesize> bitv= v;  //Decimal variation is converted to bitset to get visible/hidden flags
		//for (long i=0; i<(long)numFrontFaces.size(); i++){
		//	bitv[numFrontFaces[i]]= 1;
		//}
		for(long i=0; i<FaceCount; i++){
			if(bitv[i] == 1){  //Face i is declared as visible in the current variation
				//Set visible all its edges
				for(long j=0; j<Face[i].C; j++){
					Visible[Face[i].E[j]]= VISIBLE;
				}
			}
		}

		//Check if the current variation of faces
		//produces a variation of edges already analyzed
		std::bitset<bitedgesize> BitPatternEdges= 0;
		for (long i=0; i<EdgeCount; i++){
			if (Visible[i] == VISIBLE)
				BitPatternEdges[i]= 1;
		}
		long patternEdges= BitPatternEdges.to_ulong();

		if (IsValueInVector(patternEdges, VisitedEdgeVariations))
			continue;  //Skip to the next variation
		else
			//Add the current Variation to the list of analyzed Variations
			VisitedEdgeVariations.push_back(patternEdges);

		//Variation is invalid if fails to fit any of the following rules:
		//----------------------------------------------------------------
		//Edges of the perimeter cannot be hidden
		//(unless they also intersect another edge of the perimeter)
		if(HiddenEdgeInPerimeter())
			continue;

		//Visible edges cannot intersect another visible edge
		if (VisibleEdgesIntersect())
			continue;

		//Vertices located inside a visible faces must be hidden
		if(VisibleVertexInsideVisibleFace())
			continue;

		//If hidden edges do not intersect any visible edge
		//at least one of their vertices must be hidden
		//(all the edges connected to, at least, one of their vertices must be hidden)
		if(HiddenEdgeConnectedToVisibleEdges())
			continue;

		//If valid and not previously included,
		//add the current variation to the list of valid Variations
		ValidFaceVariations.push_back(v);
		NumHiddenLines.push_back(GetNumHiddenEdges());
	}


	if ((long)ValidFaceVariations.size() == 0){
		AfxMessageBox("No valid hidden edges configuration was found");
		Visible.clear();
		return false;
	}


	//Reorder valid Variations from less to more hidden edges
	SortMinorFirst(NumHiddenLines, ValidFaceVariations);

	//Save edges' visibility of valid variations of visible faces
	std::vector<std::vector<bool>> visible;

	for(long v= 0; v<(long)ValidFaceVariations.size(); v++){
		visible.push_back(std::vector<bool>());
		visible[v].assign(EdgeCount, false);

		for(long i= 0; i<(long)numFrontFaces.size(); i++){
			for(long j=0; j<Face[numFrontFaces[i]].C; j++){
				visible[v][Face[numFrontFaces[i]].E[j]]= true;
			}
		}

		std::bitset<bitfacesize> BitV= ValidFaceVariations[v];
		for(long i= 0; i<FaceCount; i++){
			if(BitV[i] == 1){
				//Set as visible all the edges belonging to visible faces
				for(long j=0; j<Face[i].C; j++)
					visible[v][Face[i].E[j]]= true;
			}
		}
	}

	//Automatic or interactive selection of the best solution
	long selected= 0;
	if (ReferApp.Cu.Hi.HiddenEdgesAutomatic){
		//If automatic selection, choose the first valid Variation 
		//(which is the one with less hidden edges)
		selected= 0;
	}
	else{
		//If interactive selection, ask the user
		std::vector<long> Void;
		CDialogHiddenEdges Dlg;
		Dlg.SetParams(visible, selected);
		if(Dlg.DoModal()==IDOK){
			Dlg.GetParams(selected);
		}
	}

	//Update visibility of lines in the database, following the selected variation of faces
	Visible= VisibleFrontFace;
	std::bitset<bitfacesize> BitVV= ValidFaceVariations[selected];
	for(long i=0; i<FaceCount; i++){
		//Search faces declared as visible in the current Variation
		if(BitVV[i] == 1){
			//Loop over all the edges of the visible face
			for(long j=0; j<Face[i].C; j++)
				Visible[Face[i].E[j]]= VISIBLE;
		}
	}

	//Calculate partially hidden edges
	ChunkPartiallyHiddenEdges(DDBB);

	return true;
}


////////////////////////////////////////////////////////////////////////////////


void CCueHiddenEdges::FixPerimeterVisibility()
//Fix visibility of the edges of the perimeter

//WARNING, partially visible perimeter edges are not yet considered!!!
{
	for (long i=0; i<(long)PerimeterEdges.size(); i++){
		Visible[PerimeterEdges[i]]= VISIBLE;
	}
}


std::vector<long> CCueHiddenEdges::GetFrontFaces()
//Get the indices of the "front" faces,
//which are those faces that may indistinctly act
//as front or side faces in an axonometric view

//The approach assumes that the view is NOT a worm's-eye perspective
//(thus discarding one of the two Necker reversals)

//The process works as follows:
//   -Finds the vertex with the lower y coordinate
//   -Finds the perimeter edges that share the lower vertex
//   -Finds the faces that share those perimeter edges
//   -The face that shares more of those perimeter edges
//    is assumed to be the base
//   -The other faces that share those perimeter edges
//    are assumed to be the front faces

//If various vertices share the same minimum Y coordinate,
//we arbitrarily use the first one we get
{
	std::vector<long> numFrontFaces;

	//Get the lower vertex (the one with the lower Y coordinate)
	long LowerVertex= 0;
	double MinY= Vertex[0].y;
	for (long i=1; i<VertexCount; i++){
		if (Vertex[i].y < MinY){
			LowerVertex= i;
			MinY= Vertex[i].y;
		}
	}

	//Finds the perimeter edges that share the lower vertex
	std::vector<long> LowerEdges;
	for (long i=0; i<(long)PerimeterEdges.size(); i++){
		if(EdgeHead[PerimeterEdges[i]]==LowerVertex ||
		   EdgeTail[PerimeterEdges[i]]==LowerVertex)
			LowerEdges.push_back(PerimeterEdges[i]);
	}

	//Finds the faces that share those lower perimeter edges
	std::vector<long> FacesWithEdgesInBase(FaceCount, 0);
	for (long i=0; i<FaceCount; i++){
		for (long j=0; j<Face[i].C; j++){
			if(IsValueInVector(Face[i].E[j], LowerEdges))
				FacesWithEdgesInBase[i]++;
		}
	}

	//Finds the base face
	long BaseFace= 0;
	long NumPerimeterEdgesInBase= FacesWithEdgesInBase[0];
	for (long i=1; i<FaceCount; i++){
		if(FacesWithEdgesInBase[i] > NumPerimeterEdgesInBase){
			BaseFace= i;
			NumPerimeterEdgesInBase= FacesWithEdgesInBase[i];
		}
	}

	if (NumPerimeterEdgesInBase == 0){
		//AfxMessageBox("No valid base was found");
		numFrontFaces.clear();
	}
	else{
		//Remove the base from the list of front faces
		FacesWithEdgesInBase[BaseFace]= 0;
		//Save the front faces
		for (long i=0; i<FaceCount; i++){
			if(FacesWithEdgesInBase[i] != 0)
				numFrontFaces.push_back(i);
		}
	}

	return numFrontFaces;
}


void CCueHiddenEdges::FixFrontFaceVisibility(std::vector<long> numFrontFaces)
//Fix visibility of the edges of the front faces
{
	for (long i=0; i<(long)numFrontFaces.size(); i++){
		for (long j=0; j<Face[numFrontFaces[i]].C; j++){
			Visible[Face[numFrontFaces[i]].E[j]]= VISIBLE;
		}
	}
}


void CCueHiddenEdges::FixInternalHiddenVertices(std::vector<long> numVisibleFaces)
//Vertices located inside visible faces must be hidden
//(cannot be connected to visible edges)
{

	//Loop over all the (visible) faces
	for (long i=0; i<(long)numVisibleFaces.size(); i++){
		std::vector<bool> vertexInVisibleFaceI(VertexCount, false);
		for(long j=0; j<(long)Face[numVisibleFaces[i]].V.size(); j++){
			vertexInVisibleFaceI[Face[numVisibleFaces[i]].V[j]]= true;
		}
		//Check vertices not belonging to the face
		//but located inside it
		for(long j=0; j<VertexCount; j++){
			//if (!IsValueInVector(j, Face[numVisibleFaces[i]].V)){
			if (!vertexInVisibleFaceI[j]){
				if(IsInternalToFace(numVisibleFaces[i], Vertex[j].x, Vertex[j].y)){
					std::vector<long> EdgesInJ= GetAllEdgesInVertex(j);
					for(long k=0; k<(long)EdgesInJ.size(); k++){
						Visible[EdgesInJ[k]]= HIDDEN;
					}
				}
			}
		}
	}
}


void CCueHiddenEdges::FixEdgesBetweenVisibleFaces()
//Fix visibility of edges connecting visible faces

//Searches all unlabelled edges and mark them as visible if:
//    share both vertices with visible edges
//    do not intersect any visible edge

//Besides, edges intersected by this new edge are labelled as hidden
{
	for(long i=0; i<EdgeCount; i++){
		if (Visible[i] == UNDEFINED){
			//Search visible edges connected to the current (undefined) edge
			bool VisibleHead= false;
			bool VisibleTail= false;
			for (long ii=0; ii<(long)EdgeCount; ii++){
				if (Visible[ii] == VISIBLE){
					if (EdgeHead[i]==EdgeHead[ii] || EdgeHead[i]==EdgeTail[ii])
						VisibleHead= true;
					if (EdgeTail[i]==EdgeHead[ii] || EdgeTail[i]==EdgeTail[ii])
						VisibleTail=true;
				}
			}
			if (VisibleHead && VisibleTail){
				//If the undefined edge with visible endpoints 
				//does not intersect other edges
				//must be visibble

				std::vector<long> InterceptedEdges;
				GetInterceptions(i, InterceptedEdges);

				bool AddVisibility= true;
				for (long ii=0; ii<(long)InterceptedEdges.size(); ii++){
					if (Visible[InterceptedEdges[ii]] == VISIBLE){
						AddVisibility= false;
						break;
					}
				}

				if (AddVisibility){
					//Current edge is visible and intercepted edges are hidden
					Visible[i]= VISIBLE;
					for (long ii=0; ii<(long)InterceptedEdges.size(); ii++)
						Visible[InterceptedEdges[ii]]= HIDDEN;
				}
			}
		}
	}
}


bool CCueHiddenEdges::HiddenVertexOutsideVisibleFaces()
//Checks if the current variation of hidden edges fits the following criterion:
//		Hidden vertices (connected to only hidden edges)
//		cannot be placed outside all visible faces

//If, at least, one hidded edge falls outside of all visible faces
//the Variation is declared invalid (flag true is returned)
{
	//Loop over all (hidden) vertices
	for (long i=0; i<VertexCount; i++){
		//Check if the current vertex is hidden
		//(All the edges connected to it must be hidden)
		std::vector<long> EdgesInI= GetAllEdgesInVertex(i);
		bool VertexIsHidden= true;
		for(long ii= 0; ii<(long)EdgesInI.size(); ii++){
			if (Visible[EdgesInI[ii]] == HIDDEN){
				VertexIsHidden= false;
				break;
			}
		}

		//Check if hidden vertex is inside any visible face
		if (VertexIsHidden){
			//Loop over the (visible) faces
			bool Internal= false;
			for(long j=0; j<FaceCount; j++){
				//Check if the current face is visible
				//(All their edges must be visible)
				bool FaceIsVisible= true;
				for(long ii= 0; ii<(long)Face[j].E.size(); ii++){
					if (Visible[Face[j].E[ii]] == HIDDEN){
						FaceIsVisible= false;
						break;
					}
				}

				if(FaceIsVisible){
					//Check if vertex is internal to face
					if(IsInternalToFace(j, Vertex[i].x, Vertex[i].y)){
						Internal= true;
						break;
					}
				}
			}

			if(!Internal)
				//The current hidden vertex is not internal to any visible face
				return false;
		}
	}

	//None incompatibility was found
	return false;
}


bool CCueHiddenEdges::HiddenEdgeConnectedToHiddenEdges()
//Checks if the current Variation of hidden edges fits the following criterion:
//		If a hidden edge does not intersect any visible edge,
//		all the edges connected to one of their vertices must be hidden

//True is returned if the Variation is found invalid
{
	for (long i=0; i<EdgeCount; i++){
		if (Visible[i] == HIDDEN){
			std::vector<long> InterceptedEdges;
			GetInterceptions(i, InterceptedEdges);

			bool NoVisible=true;
			for(long ii= 0; ii<(long)InterceptedEdges.size(); ii++){
				if (Visible[InterceptedEdges[ii]] == VISIBLE){
					NoVisible=false;
					break;
				}
			}

			if(NoVisible){
				//Check if all shared edges are visible
				bool VisibleHead= true;
				bool VisibleTail= true;
				long ii=0;
				while (ii<EdgeCount && (VisibleHead||VisibleTail)){
					if (i != ii){
						if(Visible[ii] == HIDDEN){
							if (EdgeHead[ii]==EdgeHead[i] || EdgeTail[ii]==EdgeHead[i])
								VisibleHead= false;
							if (EdgeHead[ii]==EdgeTail[i] || EdgeTail[ii]==EdgeTail[i])
								VisibleTail= false;
							}
					}
					ii++;
				}

				if (VisibleHead && VisibleTail)
					return true;
			}
		}
	}

	//None incompatibility was found
	return false;
}


bool CCueHiddenEdges::GetHiddenByEdgeVariations(CDB_Model *DDBB)
//Exhaustive search of all possible Variations with repetition
//of visible/hidden edges
//Every candidate solution is named as a "Variation" 
//and is accepted as valid if, and only if, accomplishes some heuristic rules

//Firstly, the search space is reduced by applying a sequence of those
//well known descriptive geometry rules:
//    - Edges belonging to the perimeter are marked as visible
//    - Edges belonging to the front faces are labeled as visible
//      (thus selecting one of the two Necker reversal solutions)
//    - All the edges meeting at an internal vertex
//      must have the same visibility
//      Thus, internal vertices with one or more edges marked as visible
//      are revisited to mark as visible all of their edges

//Secondly, variations are marked as invalid if they do not fit,
//at least, one of the following rules:
//    - Visible edges cannot intersect another visible edge
//    - If hidden edges do not intersect any visible edge
//      at least one of their vertices must be hidden
//      (all the edges connected to, at least, one of their vertices must be hidden)
//    - Vertices located inside a visible face must be hidden
//      (cannot be connected to visible edges)

//Finally, one of the valid solutions is selected automatically or interactively
//(depends on the flag HiddenEdgesAutomatic)
//    Valid solutions are ordered from less to more hidden edges.
//    Thus, in case of automatic selection, the solution with less hiddend edges is selected

//Based on:
	//Julián Conesa Pastor.
	//RECONSTRUCCIÓN GEOMÉTRICA DE SÓLIDOS UTILIZANDO TÉCNICAS DE OPTIMIZACIÓN.
	//PhD Thesis, 2001
	//Departamento de Estructuras y Construcción.
	//Universidad Politécnica de Cartagena
{
	//Fix some edges that are known to be visible
	//-------------------------------------------
	//Fix visibility of the edges of the perimeter
	FixPerimeterVisibility();

	//Fix visibility of the front faces
	std::vector<long> numFrontFaces= GetFrontFaces();
	FixFrontFaceVisibility(numFrontFaces);

	//Fix visibility of edges meeting at an internal vertex
	FixInternalHiddenVertices(numFrontFaces);

	FixEdgesBetweenVisibleFaces();

	//Evaluate the variations to be considered
	//----------------------------------------
	//Get the list of edges still undefined (NotDefinedEdges)
	std::vector<long> NotDefinedEdges;
	for (long i=0; i<EdgeCount; i++){
		if (Visible[i] == UNDEFINED)
			NotDefinedEdges.push_back(i);
	}

	if ((long)NotDefinedEdges.size()==0){
		//AfxMessageBox("All the edges are labelled");

		//Calculate partially hidden edges
		//ChunkPartiallyHiddenEdges(DDBB);

		return true;
	}

	//Evaluate the size of the undefined set of edges
	//(Variations with repetition of 2 visibility conditions over "NotDefinedEdges" edges)
	long NumVariations= (long)pow(2.0, (long)NotDefinedEdges.size());

	const int bitsize= 1024;
	if (NotDefinedEdges.size() > bitsize){
		AfxMessageBox("Not enough memory allocated to calculate hidden edges");
		Visible.assign(EdgeCount, UNDEFINED);
		return false;
	}

	//Check the Variations for compatibility
	//and save the valid Variations
	std::vector<long> ValidVariations;
	std::vector<long> NumVisibleLines;
	std::vector<std::vector<bool>> visible;

	for (long v=0; v<NumVariations; v++){
		//Load the current Variation in the database
		//SetEdgesVariation(NotDefinedEdges, v);

		std::bitset<bitsize> vv= v;
		//long vvv= vv.to_ulong();
		Visible.assign(EdgeCount, VISIBLE);
		for(long i=0; i<(long)NotDefinedEdges.size(); i++){
			if(vv[i] == 0)
				Visible[NotDefinedEdges[i]]= HIDDEN;
		}

		//Variation is invalid if does not fit the following rules:
		//---------------------------------------------------------
		//Visible edges cannot intersect another visible edge
		if(VisibleEdgesIntersect())  // #1 in Conesa
			continue;

		//Hidden vertices (connected to hidden edges)
		//cannot be located outside all visible faces
		if(HiddenVertexOutsideVisibleFaces())  // #2 in Conesa
			continue;

		//If hidden edges do not intersect any visible edge
		//at least one of their vertices must be hidden
		//(all the edges connected to, at least, one of their vertices must be hidden)
		if(HiddenEdgeConnectedToVisibleEdges())  // #3.1 in Conesa
			continue;

/////////////////////////////////////////////////////////////////////
		//If a hidden edge does not intersect two visible edges of the same face,
		//then, at least one of the edges connected to its two vertices must be hidden
		if(HiddenEdgeConnectedToHiddenEdges())   // #3.2 in Conesa
			continue;
/////////////////////////////////////////////////////////////////////

		//Vertices located inside a visible faces must be hidden
		//(cannot be connected to visible edges)
		if(VisibleVertexInsideVisibleFace())  // #4 in Conesa
			continue;

		//If not rejected and not previously included,
		//add variation to the list of valid variations
		if (!IsValueInVector(v, ValidVariations)){
			ValidVariations.push_back(v);

			visible.push_back(std::vector<bool>());
			long last= visible.size()-1;
			visible[last].assign(EdgeCount, false);

			long numVisible= 0;
			for(long i=0; i<EdgeCount; i++){
				if(Visible[i] == VISIBLE){
					visible[last][i]= true;
					numVisible++;
				}
			}

			NumVisibleLines.push_back(numVisible);
		}
	}

	if ((long)ValidVariations.size()==0){
		AfxMessageBox("No valid hidden edges configuration was found");
		Visible.assign(EdgeCount, UNDEFINED);
		return false;
	}

	//Reorder valid Variations from less to more hidden edges
	SortMinorFirst(NumVisibleLines, visible);

	//Automatic or interactive selection of the best solution
	long selected= 0;
	if (ReferApp.Cu.Hi.HiddenEdgesAutomatic){
		//If automatic selection, choose the first valid Variation
		//(which is the one with less hidden edges)
		selected= 0;
	}
	else{
		//If interactive selection, ask the user
		std::vector<long> Void;
		CDialogHiddenEdges Dlg;
		Dlg.SetParams(visible, selected);
		if(Dlg.DoModal()==IDOK){
			Dlg.GetParams(selected);
		}
	}

	//Update visibility of lines in the database, following the selected variation of edges
	//SetEdgesVariation(NotDefinedEdges, ValidVariations[selected]);

	std::bitset<bitsize> VV= ValidVariations[selected];
	//long vvv= vv.to_ulong();
	Visible.assign(EdgeCount, VISIBLE);
	for(long i=0; i<(long)NotDefinedEdges.size(); i++){
		if(VV[i] == 0)
			Visible[NotDefinedEdges[i]]= HIDDEN;
	}

	//Calculate partially hidden edges
	ChunkPartiallyHiddenEdges(DDBB);

	return true;
}


////////////////////////////////////////////////////////////////////////////////


bool CCueHiddenEdges::GetHiddenEdges(CDB_Model *DDBB)
//Manager for choosing the approach for finding hidden edges in wireframes
//	The input must contain vertices, edges and faces (in DDBB)
//	The oputput contains flags for visible/hidden edges (in DDBB)
//	The approach is chosen depending on the flag "HiddenEdgesApproach"

//Returns true if hidden edges are succesfully calculated, false otherwise

//Faces, as much as perimeter, are required to detect hidden edges!
{
	if(!InputModel(DDBB))
		return false;

	bool State= false;

	//Reset all the edges as not-visited ("UNDEFINED")
	Visible.assign(EdgeCount, UNDEFINED);

	//Label the dashed lines of the sketch as hidden edges
	if (ReferApp.Cu.Hi.HiddenEdgesByDashedLines){
		//Add dashed lines as hidden edges
		bool hidden= false;
		for (long i=0; i<EdgeCount; i++){
			CEdge edgei= DDBB->GetEdge(i);
			if (edgei.Dashed){
				Visible[i]= HIDDEN;
				hidden= true;
			}
		}

		if (hidden && !ReferApp.Cu.Hi.HiddenEdgesRefineDashedLines){
			//Do not further refine the list of hidden edges
			for (long i=0; i<EdgeCount; i++){
				if (Visible[i] == UNDEFINED)
					Visible[i]= VISIBLE;
			}
			ReferApp.Cu.Hi.TypesOfEdges= Visible;
			return true;
		}
	}

	//Select the approach
	if (ReferApp.Cu.Hi.HiddenEdgesApproach == 0){
		//Exhaustive search of all posible variations of visible/hidden faces
		State=GetHiddenByFaceVariations(DDBB);
	}

	else{
		//Exhaustive search of all posible variations of visible/hidden edges
		State=GetHiddenByEdgeVariations(DDBB);
	}

	//Output visibility
	ReferApp.Cu.Hi.TypesOfEdges= Visible;

	return State;
}
