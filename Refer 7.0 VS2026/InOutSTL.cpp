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

#include <fstream>
#include <string>
#include <vector>
#include <cmath>  // sin, cos, sqrt, atan2

#include "Tools_Geometry.h"

#include "DB_Model.h"
#include "DB_Model_Maths.h"
#include "Refer.h"
#include "resource.h"

#include "CueMainAxes.h"

#include "InOutSTL.h"

// public:


InOutSTL::InOutSTL()
{
};


InOutSTL::~InOutSTL()
{
};


bool InOutSTL::writeSTL( const char *file, CDB_Model *pDDBB)
//Writes the faces of the database 'pDDBB' into a STL file
//Returns TRUE if the process is sucessful

//STL does not accept polygonal faces,
//Hence, Refer BRep faces must be tessellated as sets of triangles.
//Each n-vertex face is tessellated as n-2 triangles
//This must have been in advance, 
//during the "tessellation" stage of the reconstruction procedure.

//Normal vectors of faces must also be calculated in advance,
//during the "tessellation" stage.
//Normals must be unit vectors pointing outwards from the solid object.

//However, many destination software will ignore normals provided in the file.
//Instead, they will calculate normals
//based on the order of the triangle vertices using the "right-hand rule"
//Hence, proper calculation of normals is recomended, 
//but ordering the vertices of the triangles counter clocwise (CCW)
//(as seen from outside the 3D model) is mandatory

{
	//If main axes are calculated, the model is aligned to them, before outputing its data
	//(otherwise the database aligned with the inflation reference system is selected)
	CDB_Model DDBB_STL= *pDDBB;
	CCueMainAxes MainAxes;
	MainAxes.TransformToMainAxes(&DDBB_STL, true);

	char CadenaAux[20];

	// Opens the file "file"
	std::ofstream out (file);

	if (!out.is_open ()) {
		CString sMensaje;
		sMensaje+="File ";
		sMensaje+=file;
		sMensaje+=" cannot be opened to write";
		AfxMessageBox(sMensaje);
		return false;
	};
	
	// Writes the header of a STL file
	out << "solid " << std::endl;

	//Writes all the faces contained in database pDDBB
	//Faces are represented as polyface meshes, which are variants of DXF polylines
	//They must have been tessellated, thus the resulting triangles are written here
	for(long i=0; i < DDBB_STL.GetSizeFaces(); i++) {
		CFace currentFace= DDBB_STL.GetFace(i);

		long NumVertices= DDBB_STL.GetSizeVerticesInFace(i);
		if (NumVertices < 3) continue;   //Improbable faces with less than three vertices are skipped

		//Recover triangulation
		std::vector<long> Triangles= currentFace.GetFaceTriangulation();
		if ((long)Triangles.size() == 0) continue;   //Faces not triangulated are skipped

		//Recover normal vectors
		std::vector<CVector> Normals= currentFace.GetFaceTrianglesNormals();

		//Write triangles of the current face
		for(long t=0; t<(long)Triangles.size(); t+=3 ) {
			//Writes facet normal
			out << "   facet normal  ";

			sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%3f", Normals[t/3].x);
			out << CadenaAux << "  ";
			sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%3f", Normals[t/3].y);
			out << CadenaAux << "  ";
			sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%3f", Normals[t/3].z);
			out << CadenaAux << std::endl;

			out << "      outer loop"<< std::endl;

			//Writes the coordinates of the vertices of the current triangle
			CVertex V1= DDBB_STL.GetVertex(Triangles[t]);

			out << "         vertex  ";
			sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%f",V1.x );
			out << CadenaAux << "  ";
			sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%f",V1.y );
			out << CadenaAux << "  ";
			sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%f",V1.z );
			out << CadenaAux << std::endl;

			CVertex V2= DDBB_STL.GetVertex(Triangles[t+1]);

			out << "         vertex  ";
			sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%f",V2.x );
			out << CadenaAux << "  ";
			sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%f",V2.y );
			out << CadenaAux << "  ";
			sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%f",V2.z );
			out << CadenaAux << std::endl;

			CVertex V3= DDBB_STL.GetVertex(Triangles[t+2]);

			out << "         vertex  ";
			sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%f",V3.x );
			out << CadenaAux << "  ";
			sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%f",V3.y );
			out << CadenaAux << "  ";
			sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%f",V3.z );
			out << CadenaAux << std::endl;

			out << "      endloop"<< std::endl;

			//End of the current triangle
			out << "   endfacet"<< std::endl;
		}
	};

	//Writes the closing header of a STL file
	out << "endsolid"<< std::endl;

	//Closes the file
	out.close ();

	return true;
};
