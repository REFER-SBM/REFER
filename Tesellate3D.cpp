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
#include <cfloat>
#include <cmath>
#include <cassert>
#include <vector>

#include "Tools_Vector.h"
#include "Tools_Geometry.h"

#include "DB_Model.h"
#include "DB_CSG.h"
#include "Refer.h"
#include "DB_Model_Maths.h"   //ReorientFaces, GetFaceNormals
#include "resource.h"
#include "ReferDoc.h"

#include "Tessellate3D.h"

//private:


void TriangulateFaces(CDB_Model *DDBB)
//Triangulate polygonal faces
//to get them converted into sets of triangular facets

//WARNINGS:
//	(1)	It is assumed that the face is flat, although it may be somewhat slanted

//	(2)	The triangulation is done in the XY projection of the face,
//		as it is simpler, and the result is invariant to projections
//		except in cases of degenerate projections
//		(which should not appear, since the faces were originarily detected
//		on a flat image depicted in the XY plane)

//Returns triplets of numbers of vertices of the face,
//one triplet for each trianglular facet
{
	//Return if no faces have been searched
	if (DDBB->GetSizeFaces()==0){
		AfxMessageBox("Triangulating faces requires faces calculated in advance");
		return;
	}

	//Loop over all the faces
	for (long iFace=0; iFace<DDBB->GetSizeFaces(); iFace++){
		long numVer= DDBB->GetSizeVerticesInFace(iFace);

		CFace currentFace= DDBB->GetFace(iFace);
		std::vector<long> vertices= DDBB->GetAllVerticesInFace(currentFace);

		std::vector<long> faceTriangles;

		if (numVer > 3){ 
			//Represent the face as a non-trivial 2D polygon
			CVertex V= DDBB->GetVertex(vertices[0]);
			double minX= V.x;   double maxX= V.x;
			double minY= V.y;   double maxY= V.y;
			double minZ= V.z;   double maxZ= V.z;
			for(long iVertex=1; iVertex<numVer; iVertex++){
				CVertex V= DDBB->GetVertex(vertices[iVertex]);
				minX= min(minX , V.x);   maxX= max(maxX , V.x);
				minY= min(minY , V.y);   maxY= max(maxY , V.y);
				minZ= min(minZ , V.z);   maxZ= max(maxZ , V.z);
			}
			double rangeX= maxX-minX;
			double rangeY= maxY-minY;
			double rangeZ= maxZ-minZ;

			long numPlane= 2;   //Default is projecting the face on the XY plane (ignoring Z coordinates)
								//But faces orthogonal to the XY plane are projected over it as lines (degenerate polygons)
			//if(rangeZ < rangeX && rangeZ < rangeY)
			//	numPlane= 2;
			//else if(rangeX < rangeY && rangeX < rangeZ)
			if(rangeX < rangeY && rangeX < rangeZ)
				numPlane= 0;  //Ignore X coordinates
			else if(rangeY < rangeX && rangeY < rangeZ)
				numPlane= 1;  //Ignore Y coordinates

			POLYGON2D polygonFace;
			for(long iVertex=0; iVertex<numVer; iVertex++){
				CVertex V= DDBB->GetVertex(vertices[iVertex]);
				NumPOINT2D P;
				P.Number= vertices[iVertex];
				if(numPlane == 2){
					P.Point.x= V.x;
					P.Point.y= V.y;
				}
				else if(numPlane == 0){
					P.Point.x= V.y;
					P.Point.y= V.z;
				}
				else if(numPlane == 1){
					P.Point.x= V.x;
					P.Point.y= V.z;
				}
				polygonFace.push_back(P);
			}

			//Check if the polygon is simple, thus tesellable
			long itype= polygonFace.polygonType();
			if(itype == 0){
				//Complex polygon
				CString sMensaje, sMensajeAux;
				sMensaje+="ERROR: unable to triangulate complex face ";
				sMensajeAux.Format("%i", iFace);
				sMensaje+=sMensajeAux;
				AfxMessageBox(sMensaje);
				continue;
			}
			if(itype < 0){
				//Clockwise polygon (CW)
				//Must be reversed before triangulation
				//(Because triangulation algorithm only works for CCW polygons)
				polygonFace.ReversePolygon();
			}

			//Triangulate polygon
			std::vector<TRIANGLE2D> triangles;
			if(!polygonFace.triangulate(triangles)){
				CString sMensaje, sMensajeAux;
				sMensaje+="ERROR: unable to triangulate face ";
				sMensajeAux.Format("%i", iFace);
				sMensaje+=sMensajeAux;
				AfxMessageBox(sMensaje);
			}

			//Save current face triangulation
			for (long iTriangle=0; iTriangle<(long)triangles.size(); iTriangle++){
				//Un-reverse triangles if face has been previously reversed for triangulation
				if(itype < 0){
					faceTriangles.push_back(triangles[iTriangle].v3.Number);
					faceTriangles.push_back(triangles[iTriangle].v2.Number);
					faceTriangles.push_back(triangles[iTriangle].v1.Number);
				}
				else{
					faceTriangles.push_back(triangles[iTriangle].v1.Number);
					faceTriangles.push_back(triangles[iTriangle].v2.Number);
					faceTriangles.push_back(triangles[iTriangle].v3.Number);
				}
			}
		}
		else{
			//Save the one single triangular facet that equals all the face
			faceTriangles.push_back(vertices[0]);
			faceTriangles.push_back(vertices[1]);
			faceTriangles.push_back(vertices[2]);
		}

		//Save face triangulation in the database
		currentFace.SetFaceTriangulation(faceTriangles);
		DDBB->SetFace(iFace, currentFace);
	}
}


//public:

void TessellateModelBrep(CDB_Model *DDBB)
//Tessellates the 3D model Brep
{
	//Return if no faces have been searched
	if (DDBB->GetSizeFaces()==0){
		AfxMessageBox("Tessellating faces requires faces calculated in advance");
		return;
	}

	//Reorient faces outward
	//(so that counter clockwise order of vertices matches the normal pointing outwards)
	if(ReferApp.T3D.FacesOutward)
		ReorientFaces(DDBB);

	//Triangulate the faces of the 3D model
	if(ReferApp.T3D.TriangulateFaces)
		TriangulateFaces(DDBB);  //Warning! To get triangles properly oriented,
								 //faces must be reoriented prior to triangulate them!

	//Calculate the normals to the faces of the 3D model
	if(ReferApp.T3D.NormalsToFaces)
		GetFaceNormals(DDBB);

	//Updates the window of the 3D model
	CReferDoc::GetDoc()->RefreshWindowModelBrep();
}
