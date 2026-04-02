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
#include "Refer.h"
#include "resource.h"
#include "DB_Model_Maths.h"

#include "CueFacesV_Geometry.h"
#include "CueFacesVarley.h"

#include <stack>
#include "CueFacesCB.h"
#include "DialogProgress.h"
#include "CueFacesSL.h"

#include "CueFaces.h"


bool GetFaces(CDB_Model *DDBB)
// Manager for choosing the approach for finding faces in wireframes

// The input is a list of vertices and edges contained in DDBB
// The oputput is a list of faces, also contained in DDBB
// The approach is chosed depending on the flag "DetectFacesApproach"
{
	bool State= true;

	//If database is not empty, faces have already been calculated
	//if (DDBB->GetSizeFaces()!=0) return true;
	
	// Detect faces following the approach selected in "DetectFacesApproach"
	if (ReferApp.Cu.Fa.DetectFacesApproach==0){
		// Varley's approach
		CFacesVarley FVarley(DDBB);
		State= FVarley.Get_Faces_Varley();
		FVarley.Output_Faces_Varley(DDBB);
	}
	else if (ReferApp.Cu.Fa.DetectFacesApproach==1){
		// Shpitalni and Lipson approach (modified)
		CFacesSL FacesSL;
		State= FacesSL.Get_Faces_SL(DDBB, 1, 1, 0);
	}
	else{
		// Courter and Brewer approach
		CFacesCB FacesCB;
		State= FacesCB.Get_Faces_CB(DDBB);
	}

	return State;
}


void GetImplodedFaces(CDB_Model *DDBB, CDB_Model *DB_Implosion) 
// Calculates polygons inscribed in the faces
// They are used to visualize the faces unconnected to each other
// (i.e. when they are displayed, they appear like "imploded" faces)

//Faces must have been calculated before!
{
	//Distance between the original and the displaced sides of the polygonal faces (in %)
	double ImplosionFactor= 0.85;
	
	//Loop over the set of true faces to calculate their imploded contours
	for(long i=0; i<DDBB->GetSizeFaces(); i++){
		CVertex Centroid= GetFaceCentroid(DDBB, i);

		//Size of DBImplosion is different from model database
		//as vertices and edges are splited for each face
		long L= DB_Implosion->GetSizeVertices();
		std::vector<long> vertices= DDBB->GetAllVerticesInFace(DDBB->GetFace(i));
		long N= vertices.size();

		//Modify vertices and add them to the imploded database
		for(long j=0; j<N; j++){
			//Modify vertex coordinates to produce the "implosion"
			CVertex Vertice= DDBB->GetVertex(vertices[j]);
			double Vertice_x= Centroid.x + (Vertice.x-Centroid.x)* ImplosionFactor;
			double Vertice_y= Centroid.y + (Vertice.y-Centroid.y)* ImplosionFactor;;

			//Save the new vertex
			CVertex VerticeJ;
			VerticeJ.x= Vertice_x;
			VerticeJ.y= Vertice_y;
			DB_Implosion->AddVertex(VerticeJ);
		}

		//Add edges to the imploded database
		for(long j=0; j<N; j++){
			long k=(j+1)%N;
			CEdge Edge;
			Edge.Head= L+j;
			Edge.Tail= L+k;

			DB_Implosion->AddEdge(Edge);
		}
	}
}
