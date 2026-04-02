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
#include <stack>
#include <vector>

#include "Tools_Vector.h"
#include "Tools_Geometry.h"

#include "DB_Model.h"
#include "DB_Model_Maths.h"
#include "Refer.h"
#include "resource.h"

#include "CueFaces.h"  // IsInternalToFace

#include "DialogProgress.h"
#include "CueFacesSL.h"


bool CFacesSL::IsInternalToFace(std::vector<long> vertices,
								double PointX, double PointY, CDB_Model *DDBB)
//Returns true if point (PointX, PointY) is internal to the face with "vertices"
//and false otherwise

//Based on:
//    Copyright © 1994-2006, W Randolph Franklin (WRF)
//    You may use my material for non-profit research and education,
//    provided that you credit me, and link back to my home page.
//    https://www.ecse.rpi.edu/Homepages/wrf/Research/Short_Notes/pnpoly.html
{
	bool inside = false;

	long n= vertices.size();
	long i, j;
	for (i = 0, j = n-1; i < n; j = i++) {
		CVertex vi= DDBB->GetVertex(vertices[i]);
		CVertex vj= DDBB->GetVertex(vertices[j]);

		if ( ((vi.y>PointY) != (vj.y>PointY)) &&
		     (PointX < (vj.x-vi.x) * (PointY-vi.y) / (vj.y-vi.y) + vi.x) )
			inside = !inside;
	}

	return inside;
}


void CFacesSL::GetTriangularFaces(CFacesSet &CarasTriangulares, CDB_Model *DDBB)
//Searches for triangular loops

//Secuentially visits all the vertices of the graph
//Then visits the other tip (vertex 2) of all the edges connected to the current vertex (vertex 1)
//Then searches for edges:
//	(1) conected to vertex 2 (i.e. second vertex of the first edge), and
//	(2) connected as well to the current vertex (vertex 3== vertex 1)

//In a second stage, triangular loops that are presumed to be false faces are removed
{
	for (long iVertice1=0; iVertice1<DDBB->GetSizeVertices(); iVertice1++){
		//Loop over the edges connected to vertex Vertice1
		for (long i=0; i<DDBB->GetSizeEdgesInVertex(iVertice1); i++){
			long iArista1= DDBB->GetEdgeInVertex(iVertice1, i);
			CEdge AristaI= DDBB->GetEdge(iArista1);

			//Vertice2 is the other tip of the edge
			long iVertice2;
			if (AristaI.Head==iVertice1) iVertice2=AristaI.Tail;
			else iVertice2=AristaI.Head;

			//Loop over the edges connected to vertex Vertice2
			for (long j=0; j<DDBB->GetSizeEdgesInVertex(iVertice2); j++){
				long iArista2= DDBB->GetEdgeInVertex(iVertice2, j);
				CEdge AristaJ= DDBB->GetEdge(iArista2);

				//Search for a third edge that closes the triangle
				if ((AristaJ.Head==iVertice1 && AristaJ.Tail==iVertice2) ||
					(AristaJ.Tail==iVertice1 && AristaJ.Head==iVertice2));
					//Skip the edge that connects Vertice1 to Vertice2
				else{
					long iVertice3;
					if (AristaJ.Head==iVertice2)
						iVertice3=AristaJ.Tail;
					else 
						iVertice3=AristaJ.Head;

					for (long k=0; k<DDBB->GetSizeEdgesInVertex(iVertice3); k++){
						long iArista3= DDBB->GetEdgeInVertex(iVertice3, k);
						CEdge AristaK= DDBB->GetEdge(iArista3);
						if (AristaK.Head==iVertice1 || AristaK.Tail==iVertice1){
							//The edge closes the triangular circuit
							long iArista1Aux=iArista1;
							long iArista2Aux=iArista2;
							long iArista3Aux=iArista3;

							//Reorder the triangular face (to simplify the searches)
							if (iArista2Aux < iArista1Aux){
								std::swap(iArista1Aux, iArista2Aux);
							}
							if (iArista3Aux < iArista1Aux){
								std::swap(iArista1Aux, iArista3Aux);
								std::swap(iArista2Aux, iArista3Aux);
							}
							else{
								if (iArista3Aux < iArista2Aux){
									std::swap(iArista2Aux, iArista3Aux);
								}
							}

							//Check if the triangular face is already in the list
							bool exist=false;
							for (long ic= 0; ic<CarasTriangulares.GetSize(); ic++){
								CFace Cara= CarasTriangulares.GetFace(ic);
								if (Cara.GetEdgeInFace(0)==iArista1Aux &&
									Cara.GetEdgeInFace(1)==iArista2Aux &&
									Cara.GetEdgeInFace(2)==iArista3Aux){
									exist=true;
									break;
								}
							}
							//Add the new triangular face
							if (!exist){
								std::vector<long> AristasCara;
								AristasCara.push_back(iArista1Aux);
								AristasCara.push_back(iArista2Aux);
								AristasCara.push_back(iArista3Aux);

								CFace Cara;
								Cara.SetAllEdgesInFace(AristasCara);
								CarasTriangulares.AddFace(Cara);
							}
						}
					}
				}
			}
		}
	}

	//Si el grafo tiene más aristas que las pertenecientes a las caras triangulares
	//tengo que eliminar aquellas caras triangulares que encierren a las demás
	if (CarasTriangulares.GetSize()!=0){
		std::vector<long> VAristasCarasTriangulares;
		CFace Cara;
		long IndiceArista;

		for (long i=0; i<CarasTriangulares.GetSize(); i++){
			Cara= CarasTriangulares.GetFace(i);
			for (long j=0; j<Cara.GetSizeEdgesInFace(); j++){
				IndiceArista= Cara.GetEdgeInFace(j);
				if (!IsValueInVector(IndiceArista, VAristasCarasTriangulares))
				VAristasCarasTriangulares.push_back(IndiceArista);
			}
		}

		if ((long)VAristasCarasTriangulares.size() != DDBB->GetSizeEdges()){
			//Busco Caras triangulares que encierren a las otras caras
			for (long i=0; i<CarasTriangulares.GetSize(); i++){
				CFace CaraContenedora= CarasTriangulares.GetFace(i);
				std::vector<long> AristasCaraContenedora= CaraContenedora.GetAllEdgesInFace();

				long j=0;
				bool Contiene=false;
				while (j<CarasTriangulares.GetSize() && !Contiene){
					if (j!=i){
						//Para cada cara triangular distinta de la contenedora saco los vertices
						//que no pertenecen a la cara contenedora
						CFace CaraContenida= CarasTriangulares.GetFace(j);
						std::vector<long> VerticesCaraContenida;
						for (long k=0; k<CaraContenida.GetSizeEdgesInFace(); k++){
							IndiceArista= CaraContenedora.GetEdgeInFace(k);
							CEdge AristaK= DDBB->GetEdge(IndiceArista);
							if (!DDBB->IsVertexInFace(CaraContenedora, AristaK.Head) &
								!IsValueInVector(AristaK.Head, VerticesCaraContenida))
								VerticesCaraContenida.push_back(AristaK.Head);
							if (!DDBB->IsVertexInFace(CaraContenedora, AristaK.Tail) &
								!IsValueInVector(AristaK.Tail, VerticesCaraContenida))
								VerticesCaraContenida.push_back(AristaK.Tail);
						}
						//Compruebo si los vértices son interiores a la cara contenedora
						long k=0;
						long VerticesContenidos=0;
						while (k<(long)VerticesCaraContenida.size()){
							CVertex Vertice= DDBB->GetVertex(VerticesCaraContenida[k]);
							if (IsInternalToFace(AristasCaraContenedora, Vertice.x, Vertice.y, DDBB))
								VerticesContenidos++;
							k++;
						}
						if (VerticesContenidos == (long)VerticesCaraContenida.size())
							Contiene=true;
					}
					j++;
				}

				if (Contiene){
					CarasTriangulares.RemoveFace(i);
					i--;
				}
			}
		}
	}
}


void CFacesSL::GenFace(CDB_Model* pDDBB,
					   std::stack<long> pila_vertice,
					   std::stack<long> pila_arista,
					   std::stack<int>  pila_marca)
// Añade el circuito guardado en la pila como una cara de la base de datos
{
	//Crea e inicializa la lista de vertices pasados
	std::vector<int> varios(pDDBB->GetSizeVertices(), 0);

	//se cuentan los vertices repetidos por si hay bucles
	long centinel= pila_vertice.top();  // Para detectar cuando se completa el bucle

	std::stack<long> pila_verticeQ= pila_vertice;
	std::stack<long> pila_aristaQ= pila_arista;
	std::stack<int>  pila_marcaQ= pila_marca;

	pila_verticeQ.pop(); pila_aristaQ.pop(); pila_marcaQ.pop(); //desapilar
	while ((centinel!=pila_verticeQ.top()) || (pila_marcaQ.top()==0)) {
	// si es un nodo explorado pertenece al circuito
		if (pila_marcaQ.top() == 1)
			varios[pila_verticeQ.top()]++;
		pila_verticeQ.pop(); pila_aristaQ.pop(); pila_marcaQ.pop();   // ERROR: Se acaba la lista antes de acabar el bucle while
	}

	pila_verticeQ= pila_vertice;
	pila_aristaQ= pila_arista;
	pila_marcaQ= pila_marca;

	// Add the edge to a new face
	CFace Cara;
	Cara.AddEdgeInFace(pila_aristaQ.top());

	pila_verticeQ.pop(); pila_aristaQ.pop(); pila_marcaQ.pop(); //desapilar

	// Continue while the centinel is not reached, 
	// nor is reached an already visited vertex
	bool abierto=true;
	long antes=-1;
	while ((centinel!=pila_verticeQ.top()) || (pila_marcaQ.top()==0)) {
		// si es un nodo explorado pertenece al circuito
		if (pila_marcaQ.top() == 1) {
			if ((varios[pila_verticeQ.top()]>1) && abierto){
				antes= pila_verticeQ.top();
				abierto= false;
			}
			else if ((varios[pila_verticeQ.top()]>1) && (!abierto) && (antes==pila_verticeQ.top()))
				abierto= true;
			else if ((varios[pila_verticeQ.top()]>1) && !abierto)
				varios[pila_verticeQ.top()]--;

			if (abierto)
				Cara.AddEdgeInFace(pila_aristaQ.top());
		}
		pila_verticeQ.pop(); pila_aristaQ.pop(); pila_marcaQ.pop(); //desapilar
	}

	pDDBB->AddFace(Cara);

//ANA++ Aqui voy a comprobar que solo las aristas que se incluyen en la cara 
//son las marcadas como bPresente=1. Igual que los vertices

//	for(i=0; i<TamanyoListaAristas(Aristas); i++){
//		si =0;
//		ObtenListaAristas(Aristas, i, &Arista);
//		if(Arista.bPresente==1){
//			for (j=0; j<GetSize(Caras); j++){
//				CFace Cara1= Caras.GetFace(j);
//				if (Cara1.SearchEdgeInFace(i, Posicion)){
//					(si=1);
//					DestruyeCara(&Cara1);
//					break;
//				}
//			}
//			if (si==0){
//				Arista.bPresente=0;
//				SustituyeListaAristas(Aristas, i, &Arista);
//				NuevoVertice(&Vertice1);
//				NuevoVertice(&Vertice2);
//				ObtenListaVertices(Vertices, Arista.Head, &Vertice1);
//				ObtenListaVertices(Vertices, Arista.Tail, &Vertice2);
//				if (Vertice1.bPresente ==1 && Vertice2.bPresente==1){
//					for(j=0; j<GetSize(Caras); j++){
//						CFace Cara1= GetFace.Caras(j);
//					
//						for (k=0; k<Cara1.GetSizeEdgesInFace(); k++){
//							Cara1.GetEdgeInFace(k, IndiceArista);
//							if (((Aristas->aLista[IndiceArista].Head)== (Arista.Head))||
//								((Aristas->aLista[IndiceArista].Tail)== (Arista.Head))){
//								Vertice2.bPresente=0;
//								SustituyeListaVertices(Vertices, Arista.Tail, &Vertice2);
								//Arista.lcola= pila_arista.top();
								//i= pila_vertice.top();
								//pila_vertice.pop(); pila_arista.pop(); pila_marca.pop();
//								break;
//							}
//							if (((Aristas->aLista[IndiceArista].Head)== (Arista.Tail))||
//								((Aristas->aLista[IndiceArista].Tail)== (Arista.Tail))){
//								Vertice1.bPresente=0;
//								SustituyeListaVertices(Vertices, Arista.Head, &Vertice1);
								//Arista.lcola= pila_arista.top();
								//i= pila_vertice.top();
								//pila_vertice.pop(); pila_arista.pop(); pila_marca.pop();
//								break;
//							}
//						}
//					}
//				}
//				DestruyeVertice(&Vertice1);
//				DestruyeVertice(&Vertice2);
//			}
//		}
//	}
}


void CFacesSL::GenCircuits(long current, CDB_Model* pDDBB, 
						   std::vector<bool> &VisitedVertices,
						   std::vector<bool> &VisitedEdges)
// Searches for closed paths of connected edges 
// reachable from vertex current

// Edges are visited just one time

// Closed paths (i.e. circuits) are considered candidate faces
{
	// Information of the visited vertices is stored in three stacks
	std::stack<long> pila_vertice;
	std::stack<long> pila_arista;
	std::stack<int>  pila_marca; 

	//Vertex current is added to begin the stacks
	//The number of the edge is assigned as 0 as it is useless
	//because it is the first vertex and no one edge has been transversed
	//The mark -1 identifies it as the beginning
	pila_vertice.push(current); pila_arista.push(0);pila_marca.push(-1); //add

	//Mark the vertex as visited
	VisitedVertices[current]= true;

	//Loop over all the non transversed edges (to find "next" vertices)
	long next;
	while (true) {
		//Add vertices of non-visited adjacent edges
		std::vector<long> EdgesInVert= pDDBB->GetAllEdgesInVertex(current);
		for(long i=0; i<(long)EdgesInVert.size(); i++){
			if (!VisitedEdges[EdgesInVert[i]]) {
				// The other vertex of the edge is added to the stack,
				// linked to the current edge and marked as unexplored (marca=0)
				CEdge EdgeI= pDDBB->GetEdge(EdgesInVert[i]);
				if (EdgeI.Head != current) next= EdgeI.Head;
				else next= EdgeI.Tail;

				pila_vertice.push(next); pila_arista.push(EdgesInVert[i]);pila_marca.push(0); //add
			}
		}

		// The stack is examined backwards until finding
		// the first non-explored vertex with a non transversed edge,
		// or the end of the stacks
		while (VisitedEdges[pila_arista.top()]){
			pila_vertice.pop(); pila_arista.pop(); pila_marca.pop(); //remove
			if (pila_marca.top()==-1){
				goto fin;  //End of the stack
			}
		}

		//Update the current vertex
		current= pila_vertice.top();
		pila_marca.top()= 1;
		VisitedEdges[pila_arista.top()]= true;

		//If the other vertex was already included in the path, then the circuit is closed
		if (VisitedVertices[next])
			GenFace(pDDBB, pila_vertice, pila_arista, pila_marca); 

		// Otherwise, it is marked as visited
		else{
			VisitedVertices[next]= true;
		}
	}
	fin:;  //Stack is empty
}


void CFacesSL::FundamentalCircuits(CDB_Model* pDDBB, 
								   std::vector<long> PreviouslyVisitedEdges)
//Provides a list of fundamental circuits of the graph representation 
//of the line drawing stored in pDDBB

//The number of fundamental circuits is Edges-Vertices+1

//Excludes the PreviouslyVisitedEdges,
//as much as the vertices exclusively linked to them
{
	//Mark already visited vertices
	std::vector<bool> VisitedVertices;
	VisitedVertices.assign(pDDBB->GetSizeVertices(), false);
	for (long i=0; i<pDDBB->GetSizeVertices(); i++){
		bool found= false;
		for(long j=0; j<pDDBB->GetSizeEdgesInVertex(i); j++){
			if (!IsValueInVector(pDDBB->GetEdgeInVertex(i, j), PreviouslyVisitedEdges)){
				found= true;
				break;
			}
			j++;
		}
		if (!found)
			VisitedVertices[i]= true;
	}

	//Reformat flags of previously visited edges
	std::vector<bool> VisitedEdges;
	VisitedEdges.assign(pDDBB->GetSizeEdges(), false);
	for (long i=0; i<(long)PreviouslyVisitedEdges.size(); i++){
		VisitedEdges[PreviouslyVisitedEdges[i]]= true;
	}

	//Consecutively generates the fundamental circuits 
	//reachable from each not yet visited vertices
	for (long i=0; i<pDDBB->GetSizeVertices(); i++){
		if (!VisitedVertices[i])
			//Generates all circuits reachable from vertex i
			GenCircuits(i, pDDBB, VisitedVertices, VisitedEdges);
	}
}


void CFacesSL::AddCircuit(CFace *TentativeFace, CFace *Circuit)
//Adds fundamental "Circuit" to the face "TentativeFace"
{
	// Check all the edges of the fundamental circuit
	for (long i=0; i<Circuit->GetSizeEdgesInFace(); i++){
		long IndexEdge= Circuit->GetEdgeInFace(i);

		long size= TentativeFace->GetSizeEdgesInFace();
		if (size == 0){
			//Add directly, as it is the first
			TentativeFace->AddEdgeInFace(IndexEdge);
		}
		else{
			if (IndexEdge > TentativeFace->GetEdgeInFace(size-1)){
				//Add at the end, as it is greater than existing edges
				TentativeFace->AddEdgeInFace(IndexEdge);
			}
			else{
				//Check if the edge is already in the face
				long pos;
				if (!TentativeFace->SearchEdgeInFace(IndexEdge, pos)){
					//Insert it, if it was not inthe face
					for (long j=0; j<size; j++){
						// la insercion en orden es mejorable
						if (TentativeFace->GetEdgeInFace(j) > IndexEdge){
							TentativeFace->SetEdgeInFace(j, IndexEdge);
							break;
						}
					}
				}
				else{
					// Remove it, as it was in the face 
					// and the combination is xor
					TentativeFace->RemoveEdgeInFace(pos);
				}
			}
		}
	}
}


bool CFacesSL::SingleCircuit(CFace circuit, CDB_Model *DDBB)
//Check that the circuit is actually a true circuit,
//with just one closed path

//Note that edges in the circuit are NOT sequentially ordered!!
//Thus, the approach selects a seed vertex, 
//and then searches for a continuation edge
{
	//Arbirtrarily select a first vertex
	long edge= circuit.GetEdgeInFace(0);
	CEdge edge0= DDBB->GetEdge(edge);
	long vertex= edge0.Head;
	std::vector<bool> visited (DDBB->GetSizeVertices(), 0);
	visited[vertex]= true;

	long numVisited= 0;

	// Loop to travel the circuit
	//bool fin= false;
	//while (!fin){
	while (true){
		//Select next edge
		std::vector<long> edgesInV= DDBB->GetAllEdgesInVertex(vertex);
		for (long i=0; i<(long)edgesInV.size(); i++){ 
			if (edgesInV[i] != edge && circuit.SearchEdgeInFace(edgesInV[i])){
				edge= edgesInV[i];
				numVisited++;

				// Proceed to the other vertex of the edge
				CEdge edgeJ= DDBB->GetEdge(edge);
				if (vertex == edgeJ.Head)
					vertex= edgeJ.Tail;
				else
					vertex= edgeJ.Head;

				if (visited[vertex]){
					//fin= true;  // The path is now closed
					//break;
					goto fin;
				}

				visited[vertex]=true;
				break;
			}
		}
	}
	fin:;

	// Circuit is simple and closed if all the edges have been transversed
	return (numVisited == circuit.GetSizeEdgesInFace());
}


bool CFacesSL::Intercept(long iii, CFace *pCara, CDB_Model *DDBB)
// Returns true if the candidate circuit stored in pCara 
// intersects any edge
{
	long IndiceAristaEvaluada= pCara->GetEdgeInFace(iii);

	CEdge Arista= DDBB->GetEdge(IndiceAristaEvaluada);

	CVertex HeadVertex1= DDBB->GetVertex(Arista.Head);
	CVertex TailVertex1= DDBB->GetVertex(Arista.Tail);

	long lHeadVertex=Arista.Head;
	long lTailVertex=Arista.Tail;
	if(HeadVertex1.y>TailVertex1.y){
		lHeadVertex=Arista.Tail;
		lTailVertex=Arista.Head;
		TailVertex1= DDBB->GetVertex(Arista.Head);
		HeadVertex1= DDBB->GetVertex(Arista.Tail);
	}

	double Modulo=sqrt((HeadVertex1.x-TailVertex1.x)*(HeadVertex1.x-TailVertex1.x)+
					   (HeadVertex1.y-TailVertex1.y)*(HeadVertex1.y-TailVertex1.y));

	CVertex VectorU1, VectorU2;
	VectorU1.x=(TailVertex1.x-HeadVertex1.x)/Modulo;
	VectorU1.y=(TailVertex1.y-HeadVertex1.y)/Modulo;
	double Angulo1=acos((TailVertex1.x-HeadVertex1.x)/Modulo);

	long Indice=iii+1;
	bool Interceptada=false;
	while(Indice<pCara->GetSizeEdgesInFace() && !Interceptada){
		long IndiceArista= pCara->GetEdgeInFace(Indice);
		Arista= DDBB->GetEdge(IndiceArista);

		CVertex HeadVertex2= DDBB->GetVertex(Arista.Head);
		CVertex TailVertex2= DDBB->GetVertex(Arista.Tail);
		if(HeadVertex2.y>TailVertex2.y){
			TailVertex2= DDBB->GetVertex(Arista.Head);
			HeadVertex2= DDBB->GetVertex(Arista.Tail);
		}

		Modulo=sqrt((HeadVertex2.x-TailVertex2.x)*(HeadVertex2.x-TailVertex2.x)+
					(HeadVertex2.y-TailVertex2.y)*(HeadVertex2.y-TailVertex2.y));
		double Angulo2=acos((TailVertex2.x-HeadVertex2.x)/Modulo);

		//Si las aristas no tiene vertice en comun
		if(lHeadVertex!=Arista.Head && lHeadVertex!=Arista.Tail &&
			lTailVertex!=Arista.Head && lTailVertex!=Arista.Tail){
			//Si las aristas no son paralelas calculo donde se cortan
			if (std::fabs(Angulo1-Angulo2)>0.01){
				VectorU2.x=(TailVertex2.x-HeadVertex2.x)/Modulo;
				VectorU2.y=(TailVertex2.y-HeadVertex2.y)/Modulo;
				double Landa=((HeadVertex1.y-HeadVertex2.y)*VectorU1.x+
							  (HeadVertex2.x-HeadVertex1.x)*VectorU1.y)/
							  (VectorU2.y*VectorU1.x-VectorU2.x*VectorU1.y);

				CVertex VerticeInterseccion;
				VerticeInterseccion.x=HeadVertex2.x+Landa*VectorU2.x;
				VerticeInterseccion.y=HeadVertex2.y+Landa*VectorU2.y;
				//Compruebo si el punto de corte pertenece a las aristas:
				//Si la arista de referencia es horizontal
				if(std::fabs(Angulo1)<0.01 || std::fabs(Angulo1-3.1416)<0.01){
					if(((HeadVertex1.x<VerticeInterseccion.x && VerticeInterseccion.x<TailVertex1.x)||
						(TailVertex1.x<VerticeInterseccion.x && VerticeInterseccion.x<HeadVertex1.x)) &&
						((HeadVertex2.y<VerticeInterseccion.y && VerticeInterseccion.y<TailVertex2.y)||
						(TailVertex2.y<VerticeInterseccion.y && VerticeInterseccion.y<HeadVertex2.y)))
						Interceptada=true;
				}
				else{
					//Si la arista de referencia es vertical
					if(std::fabs(Angulo1-3.1416/2)<0.01 || std::fabs(Angulo1-3*3.1416/2)<0.01){
						if (((HeadVertex1.y<VerticeInterseccion.y && VerticeInterseccion.y<TailVertex1.y)||
							(TailVertex1.y<VerticeInterseccion.y && VerticeInterseccion.y<HeadVertex1.y)) &&
							((HeadVertex2.x<VerticeInterseccion.x && VerticeInterseccion.x<TailVertex2.x)||
							(TailVertex2.x<VerticeInterseccion.x && VerticeInterseccion.x<HeadVertex2.x)))
							Interceptada=true;
					}
					else{
						//Si la arista con la que se compara es horizontal
						if(std::fabs(Angulo2)<0.01 || std::fabs(Angulo2-3.1416)<0.01){
							if(((HeadVertex2.x<VerticeInterseccion.x && VerticeInterseccion.x<TailVertex2.x)||
								(TailVertex2.x<VerticeInterseccion.x && VerticeInterseccion.x<HeadVertex2.x)) &&
								((HeadVertex1.y<VerticeInterseccion.y && VerticeInterseccion.y<TailVertex1.y)||
								(TailVertex1.y<VerticeInterseccion.y && VerticeInterseccion.y<HeadVertex1.y)))
								Interceptada=true;
						}
						else{
							//Si la arista con la que se compara es vertical
							if(std::fabs(Angulo2-3.1416/2)<0.01 || std::fabs(Angulo1-3*3.1416/2)<0.01){
								if (((HeadVertex2.y<VerticeInterseccion.y && VerticeInterseccion.y<TailVertex2.y)||
									(TailVertex2.y<VerticeInterseccion.y && VerticeInterseccion.y<HeadVertex2.y)) &&
									((HeadVertex1.x<VerticeInterseccion.x && VerticeInterseccion.x<TailVertex1.x)||
									(TailVertex1.x<VerticeInterseccion.x && VerticeInterseccion.x<HeadVertex1.x)))
									Interceptada=true;
							}
							//Niguna de las aristas comparadas son ni horizontales ni verticales
							else{
								if((((HeadVertex1.x<VerticeInterseccion.x && VerticeInterseccion.x<TailVertex1.x)||
									(TailVertex1.x<VerticeInterseccion.x && VerticeInterseccion.x<HeadVertex1.x))&&
									((HeadVertex1.y<VerticeInterseccion.y && VerticeInterseccion.y<TailVertex1.y)||
									(TailVertex1.y<VerticeInterseccion.y && VerticeInterseccion.y<HeadVertex1.y)))
									&(((HeadVertex2.x<VerticeInterseccion.x && VerticeInterseccion.x<TailVertex2.x)||
									(TailVertex2.x<VerticeInterseccion.x && VerticeInterseccion.x<HeadVertex2.x))&&
									((HeadVertex2.y<VerticeInterseccion.y && VerticeInterseccion.y<TailVertex2.y)||
									(TailVertex2.y<VerticeInterseccion.y && VerticeInterseccion.y<HeadVertex2.y))))
									Interceptada=true;
							}
						}
					}
				}
			}
		}
		Indice++;
	}

	return Interceptada;
}


bool CFacesSL::SelfIntersect(CFace *pCara, CDB_Model *DDBB)
// Returns true if the circuit stored in pCara self-intersects
{
	for (long i=0; i<pCara->GetSizeEdgesInFace(); i++){
		if (Intercept(i, pCara, DDBB))
			return true;
	}
	return false;
}


bool CFacesSL::InternalEdge(CFace *pCara, CDB_Model *DDBB)
//Retun true if any internal edge exists
//(Its midpoint is internal to the polygonal face)

//(Connects two non-consecutive vertices)???
{
	std::vector<long> VerticesInFace= DDBB->GetAllVerticesInFace(*pCara);

	for (long IndiceArista=0; IndiceArista<DDBB->GetSizeEdges(); IndiceArista++){
		if (!pCara->SearchEdgeInFace(IndiceArista)){
			CEdge Arista= DDBB->GetEdge(IndiceArista);
			if (DDBB->IsVertexInFace(*pCara, Arista.Head) &&
				DDBB->IsVertexInFace(*pCara, Arista.Tail)){
				//Si existe una arista que une dos vertices de la cara,
				//compruebo si la arista es interior a la cara
				//Para ello cojo el punto medio de la arista y compruebo si es interior a la cara
				CVertex HeadVertex= DDBB->GetVertex(Arista.Head);
				CVertex TailVertex= DDBB->GetVertex(Arista.Tail);
				double XPuntoMedio=TailVertex.x+(HeadVertex.x-TailVertex.x)/2;
				double YPuntoMedio=TailVertex.y+(HeadVertex.y-TailVertex.y)/2;

				if (IsInternalToFace(VerticesInFace, XPuntoMedio,YPuntoMedio, DDBB)){
					return true;
				}
			}
		}
	}

	return false;
}


bool CFacesSL::CombinedCircuits(CDB_Model *DDBB, CDialogProgress *pProgress)
//Calcula los circuitos (caminos cíclicos) posibles en el grafo
//que constituirán todas sus caras potenciales.

//Las caras potenciales se obtienen por combinación de circuitos fundamentales.
//Por lo que se denomina espacio de circuitos al conjunto de caras potenciales
//y base del espacio al conjunto de circuitos fundamentales que los genera.

//No todas las combinaciones de circuitos fundamentales dan posibles caras, 
//Se usan los siguientes criterios:
//·- Si el circuito combinado de dos o más circuitos fundamentales 
//   tiene aristas que se interceptan entre sí, se elimina del conjunto de caras posibles.

//·- Si la combinación de dos o más circuitos fundamentales produce más de un circuito,
//   el circuito resultante es eliminado del conjunto de caras posibles.


//La combinación de circuitos fundamentales se realiza como sigue:
//·- Dados dos circuitos fundamentales obtendremos el circuito resultante tomando las
//   aristas del primer circuito y añadiendo las del segundo del tal modo que si alguna
//   arista se repite en los dos se anula y no formaría parte del circuito resultante.

//·- En el caso de que se realice una combinación partiendo de más de dos circuitos, si
//   una arista coincide “n” veces entre todos los circuitos, se distinguen dos casos: si
//   “n” es un número par en el circuito resultante se anulará la arista, si es impar la
//   arista formará parte del circuito resultante.
{
	// On input, database contains the fundamental circuits
	long numFcircuits= DDBB->GetSizeFaces();

	// Auxiliary database to temporarily store 
	// the faces resulting from combining fundamental circuits
	CDB_Model DBAux;

	// Reset the counters to manage arrangements with repetition
	// (numFcircuits-tuples of a 2-set (0, 1))
	std::vector<long> combinedCircuits(numFcircuits, 0);
	long numTuples= (long)pow(2.0,numFcircuits);

	//Check all possible permutations of circuits
	bool State= true;
	for (long i = 1; i <numTuples; i++){   // Exclude the null tuple (0,0,...,0)
		State= pProgress->UpdateProgress((long)(98*i/numTuples));
		if(!State) break;

		long number= i;
		for (long j= numFcircuits - 1; j >= 0; j--){
			combinedCircuits[j]= number % 2;
			number= number / 2;
		}

		// New face to store the new combination 
		CFace newFace;

		// Add selected fundamental circuits
		for (long j=0; j<numFcircuits; j++){
			if (combinedCircuits[j] == 1){
				// Add fundamental circuit "j"
				CFace CirtuitJ= DDBB->GetFace(j);
				AddCircuit(&newFace, &CirtuitJ);
			}
		}

		// Check if the circuit is a candidate face, and add it to the database
		if (SingleCircuit(newFace, DDBB)) {
			if (!SelfIntersect(&newFace, DDBB)){
				if (!InternalEdge(&newFace, DDBB)){
					DBAux.AddFace(newFace);
				}
			}
		}
	}

	// Copy new faces into the model database
	if (State) DBAux.CopyFacesTo(DDBB);

	return State;
}


void CFacesSL::SortFaces(CDB_Model* pDDBB)
//Reorders candidate faces, placing first those with less sides

//QuickSortLon(pListaCaras->aLista, 0, DDBB->GetSizeFaces()-1);
//void CFacesSL::QuickSortLon(CFace *vector,long izq,long der) {
{
	long numFaces= pDDBB->GetSizeFaces();
	std::vector <long> sizeFaces;
	std::vector <long> numF;
	for (long i = 0; i <numFaces; i++){
		CFace faceI= pDDBB->GetFace(i);
		sizeFaces.push_back(faceI.GetSizeEdgesInFace());
		numF.push_back(i);
	}

	SortMinorFirst(sizeFaces, numF);

	CDB_Model DBAux;
	for (long i = 0; i <numFaces; i++){
		CFace faceI= pDDBB->GetFace(numF[i]);
		DBAux.AddFace(faceI);
	}

	DBAux.CopyFacesTo(pDDBB);
}


long CFacesSL::sharedEdges(CFace *circ1, CFace *circ2, CDB_Model *DDBB)
//Returns the number of edges shared by candidate faces circ1 and circ2

//Collinear edges are returned as single shared edges
{
	std::stack<long> LL; //Stack tostore edges common to both circuits
	long cont=0;

	//All shared edges are searched
	for (long i=0; i<circ1->GetSizeEdgesInFace(); i++){
		if (circ2->SearchEdgeInFace(circ1->GetEdgeInFace(i))){
			LL.push(i);
			cont++;
		}
	}

	//Count sets of collinear edges only once
	if (cont > 1){
		long II= circ1->GetEdgeInFace(LL.top());
		LL.pop();
		cont=1;
		while (!LL.empty()) {
			long JJ= circ1->GetEdgeInFace(LL.top());
			LL.pop();

			if (!NearlyCollinear(DDBB, II, JJ, ReferApp.Cu.Fa.EdgeCollinearityAngleThreshold, 0.05))
				cont++;
			II= JJ;
		}
	}

	return cont;
}


void CFacesSL::RemoveOverlappingCircuits(CDB_Model *DDBB)
//Search for circuits that share one edge (or more, if they are collinear)
//and both share two or more non-collinear edges with a third circuit

//The approach searches the list of potential faces (ordered from smaller to greater)
//to find groups in which a face completely contains other adjacent faces 
//and shares at least one edge with each of the faces it contains. 
//completely contains two other adjacent faces sharing edges with each of them.
{
	// Reorder polygonal circuits according to their number of edges
	SortFaces(DDBB);

	long numFaces= DDBB->GetSizeFaces();

	// Flags to control removed edges
	std::vector<bool> exist (numFaces, true);

	// Three nested loops to detect nested faces
	for (long i=0; i<numFaces; i++) {
		CFace CaraI= DDBB->GetFace(i);
		if (exist[i]) {
			for (long j= 0; j<numFaces; j++){
				// se comprueba que no sean el mismo circuito y que no este eliminado j
				if ((j!=i) && exist[j]){
					CFace CaraJ= DDBB->GetFace(j);

					// si mantienen una arista o varias colineales se sigue
					if (sharedEdges(&CaraI, &CaraJ, DDBB) == 1) {
						for (long k= 0; k<numFaces; k++){
							if ((j!=k) && (i!=k) && exist[k]){
								// si el circuito k es mayor que los anteriores se sigue
								CFace CaraK= DDBB->GetFace(k);

								if((CaraK.GetSizeEdgesInFace()>CaraI.GetSizeEdgesInFace()) &&
								   (CaraK.GetSizeEdgesInFace()>CaraJ.GetSizeEdgesInFace())){
									// si el circuito i y j tienen mas de una arista en comun
									// no colineales con el circuito k, entonces se elimina k
									if (sharedEdges(&CaraI, &CaraK, DDBB) > 1){
										if (sharedEdges(&CaraJ, &CaraK, DDBB) > 1)
											exist[k]= false;
									}
								}
							}
						}
					}
				}
			}
		}
	}

	for (long i= DDBB->GetSizeFaces()-1; i>=0; i--){
		if (!exist[i]) 
			DDBB->RemoveFace(i);
	}
}


void CFacesSL::GetIncompatibilities(CDB_Model *DDBB,
									std::vector<bool> incompatibilities)
//Checks the adjacency theorem:
//	“If two adjacent faces have more than one edges in common, 
//	 both edges must be collinear”

//They are marked for elimination all those candidate faces that do not verify the theorem.

//Given two circuits, it returns 0 if they do not have common edges,
//returns 1 if they have one or more common edges
//(but if there are several, must be collinear).
//And in case there is more than one edge, that is
//two or more non-collinear, then return another value
{
	long numFaces= DDBB->GetSizeFaces();

	for (long i=0; i<numFaces; i++){
		CFace CaraI= DDBB->GetFace(i);
		for (long j=0; j<numFaces; j++){
			if (i==j)
				incompatibilities[i*numFaces+j]= false;
			else{
				CFace CaraJ= DDBB->GetFace(j);

				if ((sharedEdges(&CaraI, &CaraJ, DDBB)) > 1){
					incompatibilities[i*numFaces+j]= true;
				}
				else
					incompatibilities[i*numFaces+j]= false;
			}
		}
	}
}


void CFacesSL::Rangos_Maximos(std::vector<long> RAsup,
							  std::vector<long> RVsup,
							  CDB_Model *DDBB)
//Three parameters are used to estimate the maximum number
//of faces of a polyhedral body represented by a graph:
//	edge range “a”, indicates the number of faces of which the edge is a part
//	vertex range “v”, indicates the number of faces of which the vertex is a part.
//	vertex degree “v”, indicates the number of edges of which the vertex is a part.

//From certain conditions that relate //the three parameters described above, 
//we can limit its maximum values. Consequently, we can establish a 
//system of equations of maximum range that allows them to be determined

//The procedure to limit the maximum values is:
//1.	The degree of each vertex is detected
//2.	A preliminary estimate of the ranges of vertices and edges is computed
//3.	Iterate until equilibrium is reached.
//		(until the ranks of the edges and vertices verify the maximum rank equations)


//Based on:
	//Julián Conesa Pastor.
	//RECONSTRUCCIÓN GEOMÉTRICA DE SÓLIDOS UTILIZANDO TÉCNICAS DE OPTIMIZACIÓN.
	//PhD Thesis, 2001
	//Departamento de Estructuras y Construcción.
	//Universidad Politécnica de Cartagena
{
	// Calculamos los grados de todos los vertices
	std::vector<long> DV(DDBB->GetSizeVertices());
	for (long i=0; i<DDBB->GetSizeVertices(); i++){
		DV[i]= DDBB->GetSizeEdgesInVertex(i);
	}

	// Estimacion preliminar de los rangos
	for (long i=0; i<DDBB->GetSizeVertices(); i++)
		RVsup[i]=(int)0.5*(DV[i]*(DV[i]-1));
	for (long i=0; i<DDBB->GetSizeEdges(); i++){
		CEdge Arista= DDBB->GetEdge(i);
		RAsup[i]= min(DV[Arista.Head],DV[Arista.Tail])-1;
	}

	// Calculo iterativo para lograr valores estables de los rangos
	char estable=0;
	while (estable==0) {
		estable=1;
		// calculo RVsup de todos los vertices
		for (long i=0;i<DDBB->GetSizeVertices();i++) {
			long suma=0;
			long IndiceArista;
			for (long j=0; j<DDBB->GetSizeEdgesInVertex(i); j++){
				IndiceArista= DDBB->GetEdgeInVertex(i, j);
				suma+= RAsup[IndiceArista];
			}
			suma=(long)floor(0.5*suma);
			if (suma!=RVsup[i]){
				RVsup[i]=suma;
				estable=0;
			}
		}
		// calculo RAsup de todas las aristas
		for (long i=0; i<DDBB->GetSizeEdges(); i++) {
			CEdge Arista= DDBB->GetEdge(i);
			long suma= min(RVsup[Arista.Head],RVsup[Arista.Tail]);
			if (RAsup[i]>suma){
				RAsup[i]=suma;
				estable=0;
			}
		}
	}
}


//----------CueFacesSL_Optimization---------------
void CFacesSL::pushPere(Rb_node k,Rb_node *cabeza) {
// como es una pila simplemente se inserta en la cabeza
	Rb_node p;
	if ((p= new rb_node)==NULL) abort();   // delete is missing
	*p=*k;
	p->c.list.flink=*cabeza;
	*cabeza=p;
}


ffaces CFacesSL::calcular_f(std::vector<presen> &presencia,
							long iCara,
							std::vector<long> RAsup,
							std::vector<long> RVsup,
							CDB_Model *DDBB)
{
	ffaces devolver;
	int sum=0,resta,acumulado=0;
	long na=0;

	//se van ha almacenar los rangos de la aristas y los vertices para
	//una combinacion de caras determinada
	std::vector<long> RA(DDBB->GetSizeEdges(), 0);
	std::vector<long> RV(DDBB->GetSizeVertices(), 0);
	std::vector<long> NA(DDBB->GetSizeEdges(), 0);

	// este array sirve para determinar cuando se repite un vertice
	// en el recorrido de aristas de una cara. recordar que cada vertice
	// del circuito pertenecera a dos aristas y seria incluido dos veces
	std::vector<long> repetido(DDBB->GetSizeVertices());

	// se recorren todas las posibles caras
	for (long j=0; j<DDBB->GetSizeFaces(); j++){
		///si dicha cara esta en la combinacion analizada se sigue calculan los
		//rangos en los que influye
		if (((presencia[j].elemento)&&(presencia[j].activo))||(iCara==j)) {
			// inicializo a cero "repetido"
			repetido.clear();
			repetido.assign(DDBB->GetSizeVertices(), 0);
			for (long k=0; k<DDBB->GetSizeVertices(); k++)
				repetido[k]=0;

			CFace CaraJ= DDBB->GetFace(j);

			// recorro todas las aristas de la posible cara
			for (long k=0; k<CaraJ.GetSizeEdgesInFace(); k++) {
				long Ari= CaraJ.GetEdgeInFace(k);
				// sumo en uno el rango de la arista k-esima de la posible cara
				NA[Ari]=1;
				RA[Ari]++;
				// igualmente hago con los vertices si no esta ya incluido
				CEdge AristaK= DDBB->GetEdge(Ari);

				if (repetido[AristaK.Head]==0){
					RV[AristaK.Head]++;
					repetido[AristaK.Head]=1;
				}
				if (repetido[AristaK.Tail]==0){
					RV[AristaK.Tail]++;
					repetido[AristaK.Tail]=1;
				}
			}
		}
	}
//           /* aqui voy calculando h(x), este else se ejecuta cuando se llega
//              a una cara que no esta en el camino ni es la actual ni es
//              conflictiva */
///*           else { TamAri=lista->aLista[j].ListaAristas.lTamanyo;
//                 /* se suma el numero de aristas( que coincide
//                    con el numero de vertices de una cara) */
///*                  nv=nv+TamAri;
///*                  }

	// calculo la funcion g(x)
	for (long i=0; i<DDBB->GetSizeEdges(); i++) {
		resta=RAsup[i]-RA[i];
		if (resta<0) {
			devolver.bondad=MAXINT;
			devolver.recorrido=MAXINT;
			devolver.total=MAXINT;
			return devolver; //si es negativo implica que se sobrepasan los limites
		}
		sum=sum+resta;
	}
	acumulado=sum;

	sum=0;

	for (long i=0; i<DDBB->GetSizeVertices(); i++) {
		resta=RVsup[i]-RV[i];
		if (resta<0) {
			devolver.bondad=MAXINT;
			devolver.recorrido=MAXINT;
			devolver.total=MAXINT;
			return devolver;
		}
		sum=sum+resta;
	}

	acumulado=acumulado+sum;
	devolver.recorrido=sum;
	// le añado h(x)
	long TamAri= DDBB->GetSizeEdges();
	for (long k=0;k<TamAri;k++)
		na=na+NA[k];
	devolver.bondad= TamAri-na;
	acumulado=acumulado+devolver.bondad;

	// se devuelve la funcion f(x)
	devolver.total=acumulado;
	return devolver;
}


unsigned int CFacesSL::insertar_hijos(Rb_node padre,Rb_node Abiertos,long TamCar,
									  std::vector<long> RAsup,
									  std::vector<long> RVsup,
									  std::vector<bool> matrizPere,
									  int inicio,
									  std::vector<bool> seg,
									  CDB_Model *DDBB)
{
	char presente;
	Rb_node sig;
	int num;
	ffaces f;
	f.total=MAXINT; f.recorrido=MAXINT; f.bondad=MAXINT;

	// este array determina las caras ya añadidas a la combinacion
	std::vector<presen> presencia(TamCar);

	// se inicializa a cero
	for (long i=0;i<TamCar;i++)
		{presencia[i].elemento=0; presencia[i].activo=0;}

	// este array determina las caras imcompatibles
	// con las del recorrido actual
	std::vector<bool> incompat(TamCar, false);

	// se van marcando las caras recorridas hasta que se llega al nodo primero
	sig=padre;
	while (sig!=NULL) {
		num=sig->anexo.cara;
		presente=sig->s.presente;
		if (num!=-1){
			presencia[num].elemento=1;
			presencia[num].activo=presente;
			if (presente)
				for (long m=0;m<TamCar;m++)
					if (matrizPere[num*TamCar+m])
						incompat[m]= true;
		}
		sig=sig->anexo.anterior;
	}

	for (long i=0;i<TamCar;i++)
		//si la cara no está en el recorrido se sigue
		if (presencia[i].elemento!=1) {
			//y además no es el inicio, o si lo es debe ser una de las caras seguras
			if ((inicio==0)||((inicio==1)&&(seg[i]))) {
				// si es una cara incompatible solo se pondra la opcion no
				if (!incompat[i]) {
					// se calcula f del nuevo nodo (es decir de la combinacion propuesta
					f= calcular_f(presencia, i, RAsup, RVsup, DDBB);
					// si es negativo implica que se ha superado alguna vez los supremos
					//y por tanto se pone valor maximo para que nunca se estime (poda)

					// se inserta en los nodo abiertos
					f.bondad=MAXINT;
					f.recorrido=MAXINT;
					f.total=MAXINT;
					// Call to FacesOptimization_rb
					rb_inserti(Abiertos,f.total,NULL,padre,i,1);

					/* se añade la combinacion en la que no se añade la cara */
				}

				if (inicio==0)
					// Call to FacesOptimization_rb
					rb_inserti(Abiertos,padre->k.ikey,NULL,padre,i,0);
				if (f.total<(unsigned int)padre->k.ikey) return f.total;
				else return (unsigned int)padre->k.ikey;
			}
		}

	if (f.total<(unsigned int)padre->k.ikey) return f.total;
	else return (unsigned int)padre->k.ikey;
}


inline coord CFacesSL::prod_vec(coord &l1,coord &l2)
// Determines the vectorial product of vectors l1 and l2
{
	coord p;
	p.x= l1.y*l2.z-l1.z*l2.y;
	p.y= l1.z*l2.x-l1.x*l2.z;
	p.z= l1.x*l2.y-l1.y*l2.x;
	return p;
}


inline double CFacesSL::prod_esc(coord &l1,coord &l2)
// Determines the scalar product of vectors l1 and l2
{
	return (l1.x*l2.x+l1.y*l2.y+l1.z*l2.z);
}


double CFacesSL::calcular_w(long cara, CDB_Model *DDBB)
//Determines w coefficient of face "cara"
{
	coord l1, l2, m;

	CFace Cara= DDBB->GetFace(cara);

	double beta_i=0,suma=0,alfa;
	long p3,p4;

	long IndiceArista= Cara.GetEdgeInFace(0);

	CEdge Arista= DDBB->GetEdge(IndiceArista);
	long p2= Arista.Head;
	long p1= Arista.Tail;

	long base=p1;
	coord k;
	k.x=0; k.y=0; k.z=1;

	std::stack<double> pila_w;

	bool fin= false;
	long i, j=0;
	long contador=0;

	while (!fin) {
		for (i=0; i<Cara.GetSizeEdgesInFace(); i++){
			if (i!=j){
				IndiceArista= Cara.GetEdgeInFace(i);
				Arista= DDBB->GetEdge(IndiceArista);
				if (Arista.Tail == p2) {
					p3= Arista.Tail;
					p4= Arista.Head;
					break;
				}
				else if (Arista.Head == p2) {
					p3= Arista.Head;
					p4= Arista.Tail;
					break;
				}
			}
		}

		j=i;

		CVertex Vertice1= DDBB->GetVertex(p1);
		CVertex Vertice2= DDBB->GetVertex(p2);
		CVertex Vertice3= DDBB->GetVertex(p3);
		CVertex Vertice4= DDBB->GetVertex(p4);

		l1.x= Vertice2.x - Vertice1.x;
		l1.y= Vertice2.y - Vertice1.y;
		l1.z= 0;
		l2.x= Vertice4.x - Vertice3.x;
		l2.y= Vertice4.y - Vertice3.y;
		l2.z= 0;

		if (p3==base) fin=true;
		p2=p4; p1=p3;

		m= prod_vec(l1,l2);
		alfa= prod_esc(l1,l2);
		m.x*= alfa; m.y*= alfa; m.z*= alfa;
		beta_i= pow(double(-1),contador)*prod_esc(m, k);

		suma+=beta_i;
		contador++;
		pila_w.push(beta_i);
	}

	// Returns the variance-from-a-sample of "pila_w"
	// where the number of elements is "contador"
	// and their sum is "suma"
	double a=0, media=suma/contador;

	while (!pila_w.empty()) {
		a+=pow((pila_w.top()-media),2);
		pila_w.pop();
	}

	return sqrt(a/(contador-1));
}


Rb_node CFacesSL::seleccion(Rb_node Cerrados, unsigned long solucion, CDB_Model *DDBB)
// elige la solucion correcta en base a la regularidad(es)
{
	Rb_node sig=Cerrados,minimo=NULL,next;
	double suma,termino,almacen= (double)0xffffffffffffffff;

	while (sig!=NULL) {
		if (((unsigned long)sig->k.ikey == solucion) && ((unsigned long)sig->k.ikey != MAXINT ) ) {
			next=sig; suma=0;
			while (next->anexo.cara!=-1) {
				if (next->s.presente) {
					termino=calcular_w(next->anexo.cara, DDBB);
					suma+=termino;
				}
				next=next->anexo.anterior;
			}

			if (suma<almacen) { almacen=suma; minimo=sig; }

			//siguiente=sig;
			//while (siguiente!=NULL) {
			//      if ((siguiente->anexo.cara!=-1)&&(siguiente->s.presente))
			//         { 
			//           }
			//      siguiente=siguiente->anexo.anterior;
			//      }
		}
		sig=sig->c.list.flink;
	}
	return minimo;
}


void CFacesSL::eliminacion(Rb_node elegido,long TamCar, CDB_Model *DDBB)
// se eliminan todas las caras excepto las de la solucion
{
	Rb_node sig; 

	std::vector<bool> borrar(TamCar, true);

	sig=elegido;
	while (sig!=NULL) {
		if ((sig->anexo.cara!=-1)&&(sig->s.presente))
			borrar[sig->anexo.cara]= false;
		sig=sig->anexo.anterior;
	}

	for (long i=TamCar-1;i>=0;i--)
		if (borrar[i])
			DDBB->RemoveFace(i);
}
//-------------------------------------------


void CFacesSL::OptimizeFaces(CDB_Model *DDBB, 
							 bool tipo_objeto, bool metodo, bool valor_f)
//Se aborda el problema de identificación de caras de un objeto general
//como un problema de selección

//De forma genérica se puede establecer que el número “A” de aristas
//y el número “C” de caras se podrán expresar 
//en función del número “V” de vértices.
//Y su proyección tendrá “m” circuitos no interceptados entre sí, 
//siendo “m” una función exponencial de V. 
//Del conjunto “m” de caras potenciales, únicamente un subconjunto “k” 
//corresponderá con caras reales, siendo 0 <= k <= m

//El método consiste básicamente en 
//recorrer un árbol de búsqueda (espacio del problema) 
//mientras se evalúan condiciones de compatibilidad entre las caras. 
//Para reducir el tiempo de computo se definen:
//	- condiciones de compatibilidad 
//	- dominio de la búsqueda


//Según nuestras observaciones, 
//un sujeto humano típicamente selecciona 
//una configuración que verifique:
//	“Tantos vértices como sean posibles 
//	participarán en tantas caras como sean posibles”

//La función objetivo, entonces, es 
//la que concuerda más estrechamente 
//con los límites superiores derivados de las ecuaciones de rango máximo. 
//Dado un subconjunto “x” de caras potenciales, 
//y calculados los rangos máximos de vértices y aristas, 
//se puede calcular una función de conformidad g(x) 
//minimizando la suma de las desviaciones 
//de los rangos actuales a partir de los límites superiores de los rangos.

//La suma se realiza sobre todos los nodos y enlaces del grafo. 
//La función de conformidad es baja para un buen ajuste y alta para un mal ajuste


//Three flags control the optimization flow:
//	-tipo_objeto
//	-metodo
//	-valor_f

//Based on:
	//Julián Conesa Pastor.
	//RECONSTRUCCIÓN GEOMÉTRICA DE SÓLIDOS UTILIZANDO TÉCNICAS DE OPTIMIZACIÓN.
	//PhD Thesis, 2001
	//Departamento de Estructuras y Construcción.
	//Universidad Politécnica de Cartagena
{

	// Tamaños de las listas de vertices,aristas y caras
	long TamAri= DDBB->GetSizeEdges();
	long TamVer= DDBB->GetSizeVertices();
	long TamCar= DDBB->GetSizeFaces();

	// Incompatibilidades
	/////////////////////////////////////////////////////////
	//¿Es un cálculo redundante 
	//con la detección previa de "RemoveOverlappingCircuits"?
	/////////////////////////////////////////////////////////
	std::vector<bool> incompatibilities(TamCar*TamCar);
	GetIncompatibilities(DDBB, incompatibilities);

	// Rangos superiores de aristas y vertices
	std::vector<long> RAsup(TamAri);
	std::vector<long> RVsup(TamVer);

	Rangos_Maximos(RAsup, RVsup, DDBB);

	// el nodo inicial no contiene ninguna cara y por tanto tiene el valor maximo
	int suma=0;
	for (long i=0;i<TamAri;i++) suma+=RAsup[i];
	for (long i=0;i<TamVer;i++) suma+=RVsup[i];
	for (long i=0;i<TamCar;i++){
		//suma+=pGrafo->CFaces.aLista[i].ListaAristas.lTamanyo;
		CFace Cara= DDBB->GetFace(i);
		suma+=Cara.GetSizeEdgesInFace();
	}

	//////////// Call to FacesOptimization_rb///////////////////////////////
			/* Revision 1.2.  Jim Plank */
			/* Original code by Jim Plank (plank@cs.utk.edu) */
			/* modified for THINK C 6.0 for Macintosh by Chris Bartley */

	unsigned int almacen=MAXINT,solucion;
	Rb_node Abiertos=make_rb(),inter,buscado;
	Rb_node Cerrados=NULL;

	// se inserta dicho nodo con anterior NULL y cara -1
	rb_inserti(Abiertos, suma, NULL, NULL, -1, 1);

	// inserta los circuitos que se sabe con seguridad que son cara
	std::vector<bool> seg(TamCar, true);

	for (long i=0; i<TamCar; i++) {
		for (long j=0;j<TamCar;j++){
			if (incompatibilities[i*TamCar+j]) {
				seg[i]= false;
				break;
			}
		}
	}

	// insercion de las caras seguras
	long contar=0;
	if (tipo_objeto) {
		for (long s=0;s<TamCar;s++)
			if (seg[s]) contar++;
		while (contar) {
			inter=make_rb();
			buscado=rb_find_min(Abiertos);
			*inter=*buscado;
			rb_delete_node(buscado);
			pushPere(inter,&Cerrados);
			insertar_hijos(Cerrados, Abiertos, TamCar,
						   RAsup, RVsup, incompatibilities, 1, seg, DDBB);
			contar--;
		}
	}
	int mejor=MAXINT;
	while (true) {
		inter=make_rb();
		// se busca el nodo con menor valor de f
		if ((buscado=rb_find_min(Abiertos))==NULL)
			goto fin;
		// se copia el nodo en inter
		*inter=*buscado;
		// se elimina el nodo de la lista de abiertos
		if (rb_delete_node(buscado)==-1) goto fin;

		// añade el nodo en la lista de cerrados
		pushPere(inter,&Cerrados);
		// insertan los hijos devolviendose en caso de llegar al
		// final el nodo solucion y salida=1
		if (!(metodo))
			if ((inter->k.ikey>mejor)&&((!valor_f)||
				((valor_f)&&(mejor==0))))
				goto fin;
			else
				mejor=inter->k.ikey;

		solucion=insertar_hijos(Cerrados, Abiertos, TamCar,
								RAsup, RVsup, incompatibilities, 0, seg, DDBB);
		if (almacen>solucion)
			almacen=solucion;
	}
	fin:

	eliminacion(seleccion(Cerrados, almacen, DDBB), TamCar, DDBB);
	rb_free_tree(Abiertos);
	//////////// Call to FacesOptimization_rb///////////////////////////////
}


bool CFacesSL::Get_Faces_SL(CDB_Model *DDBB, bool tipo_objeto, bool metodo,
							bool valor_f)
// Detect faces in wireframe 2D line-drawings of polyhedral shapes
// This is a modified version of the approach proposed by Shpitalni and Lipson

//Based on:
//    Shpitalni M. and Lipson H.
//    Identification of faces in a 2D line drawing projection of a wireframe object
//    IEEE Transactions on Pattern Analysis and Machine Intelligence, 18(10), 1000 - 1012, 1996
{
	// Opens the Dialog that shows the progress, and allows cancellation
	CDialogProgress Progress;
	Progress.Create( IDD_DIALOG_Progress );
	Progress.Message("Finding Faces...");
	Progress.RangeProgress(0, 100);
	Progress.UpdateProgress(0);

	//Search for triangular faces
	CFacesSet TriangularFaces;
	GetTriangularFaces(TriangularFaces, DDBB);

	////////////////////////////////////////////////////////////////////////
	//Label visited those edges that belong to 
	//at least two different triangular faces
	std::vector<long> VisitedEdges;
	if (TriangularFaces.GetSize() != 0){
		CFace faceT= TriangularFaces.GetFace(0);

		std::vector<long>edgesTriangulaFaces= faceT.GetAllEdgesInFace();

		for (long c=1; c<TriangularFaces.GetSize(); c++){
			faceT= TriangularFaces.GetFace(c);
			for (long i=0; i<faceT.GetSizeEdgesInFace(); i++){
				long edgeI= faceT.GetEdgeInFace(i);
				if (IsValueInVector(edgeI, edgesTriangulaFaces)){
					if (!IsValueInVector(edgeI, VisitedEdges))
						VisitedEdges.push_back(edgeI);
				}
				else
					edgesTriangulaFaces.push_back(edgeI);
			}
		}
	}
	////////////////////////////////////////////////////////////////////////

	//Get the fundamental circuits (excluding visited edges)
	FundamentalCircuits(DDBB, VisitedEdges);
	Progress.UpdateProgress(1);

	//Search for combined circuits
	bool State= true;
	if (DDBB->GetSizeFaces() != 0)
		State= CombinedCircuits(DDBB, &Progress);

	//Add the triangular faces found first
	//(Just in case any of them was not detected while combining circuits)
	for (long i=0; i<TriangularFaces.GetSize(); i++){
		CFace CaraT= TriangularFaces.GetFace(i);
		//Check if already exists before adding it
		long c=0;
		bool Exists=false;
		while (c<DDBB->GetSizeFaces() && !Exists){
			CFace CaraC= DDBB->GetFace(c);
			if (CaraC.GetSizeEdgesInFace() == 3 &&
				CaraC.GetEdgeInFace(0)==CaraT.GetEdgeInFace(0) &&
				CaraC.GetEdgeInFace(1)==CaraT.GetEdgeInFace(1) &&
				CaraC.GetEdgeInFace(2)==CaraT.GetEdgeInFace(2)){
				Exists=true;
			}
			c++;
		}
		if (!Exists)
			DDBB->AddFace(CaraT);
	}

	//REFINEMENT OF FACES
	if (State){
		if (DDBB->GetSizeFaces() != 0)
			RemoveOverlappingCircuits(DDBB);

		///////////////////////////////////////////////////////
		//WARNING: shortcut because some bugs remain after updating database management!!!
		//if (DDBB->GetSizeFaces() != 0)
		//	OptimizeFaces(DDBB, tipo_objeto, metodo, valor_f);
		///////////////////////////////////////////////////////

		//Remove tentative faces whith edges shared by
		//at least three previously accepted faces
		//--------------------------------------------
		//Get the list of faces to remove (in EliminarCaras)
		std::vector<long> VEliminarCaras;
		for (long IndiceCara1=0; IndiceCara1<DDBB->GetSizeFaces(); IndiceCara1++){
			CFace Cara1= DDBB->GetFace(IndiceCara1);
			if (Cara1.GetSizeEdgesInFace()<3)
				//Delete the face if the Edge Size on the face is less than 3
				VEliminarCaras.push_back(IndiceCara1);
			else{
				//Obtains the edges of the evaluated face or the evaluated edge one by one
				long IndiceAristaCara1;
				for (IndiceAristaCara1=0; IndiceAristaCara1<Cara1.GetSizeEdgesInFace(); IndiceAristaCara1++){
					long NumeroDeCaras=0;
					long IndiceArista1= Cara1.GetEdgeInFace(IndiceAristaCara1);
					//Obtains the remaining faces
					for (long ICara2=0; ICara2<DDBB->GetSizeFaces(); ICara2++){
						if (DDBB->IsEdgeInFace(ICara2, IndiceArista1))
							NumeroDeCaras++;
					}
					if (NumeroDeCaras<3) break;
				}
				if (IndiceAristaCara1 == Cara1.GetSizeEdgesInFace())
					VEliminarCaras.push_back(IndiceCara1);
			}
		}
	
		//Remove the faces marked to delete
		for (long IndiceCara1=0; IndiceCara1<DDBB->GetSizeFaces(); IndiceCara1++){
			long Position;
			if (IsValueInVector(IndiceCara1, VEliminarCaras, Position)){
				VEliminarCaras.erase(VEliminarCaras.begin()+Position);
				DDBB->RemoveFace(IndiceCara1);
				IndiceCara1--;
			}
		}
		Progress.UpdateProgress(100);
	}

	return State;
}