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

#include "InOutOBJ.h"

// public:


InOutOBJ::InOutOBJ()
{
};


InOutOBJ::~InOutOBJ()
{
};


int InOutOBJ::digits(long number)
//returns the number of digits of number
{
	int digits= 0;
	do {number/=10; digits++;} while(number!=0);
	return digits;
}


bool InOutOBJ::writeOBJ( const char *file, CDB_Model *pDDBB)
//Writes the faces of the database 'pDDBB' into a OBJ file
//Returns TRUE if the process is sucessful

//OBJ does accept polygonal faces,
//Hence, Refer BRep faces are saved as such.

//Normal vectors of faces must also be calculated in advance,
//during the "tessellation" stage.
//Normals must be unit vectors pointing outwards from the solid object.
{
	//If main axes are calculated, the model is aligned to them, before outputing its data
	//(otherwise the database aligned with the inflation reference system is selected)
	CDB_Model DDBB_OBJ= *pDDBB;
	CCueMainAxes MainAxes;
	MainAxes.TransformToMainAxes(&DDBB_OBJ, true);

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
	
	// Writes the header of a OBJ file
	out << "#REFER 7.0 BRep solid (obj format)" << std::endl;

	//Writes all the vertices contained in database pDDBB
	out << "#vertices " << std::endl;
	for(long i=0; i < DDBB_OBJ.GetSizeVertices(); i++) {
		CVertex Vertice= DDBB_OBJ.GetVertex(i);
		out << "v ";
		sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%15f",Vertice.x);
		out << CadenaAux << "    ";
		sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%15f",Vertice.y);
		out << CadenaAux << "    ";
		sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%15f",Vertice.z);
		out << CadenaAux << std::endl;
	};

	//Writes all the normals to faces contained in database pDDBB
	out << "#normals (one per face)" << std::endl;
	for(long i=0; i < DDBB_OBJ.GetSizeFaces(); i++) {
		CFace currentFace= DDBB_OBJ.GetFace(i);
		CVector Normal= currentFace.GetNormal(false);
		out << "vn ";
		sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%15f",Normal.x);
		out << CadenaAux << "    ";
		sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%15f",Normal.y);
		out << CadenaAux << "    ";
		sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%15f",Normal.z);
		out << CadenaAux << std::endl;
	};

	//Writes all the faces contained in database pDDBB
	//Faces are represented as polygonal contours, defined by their consecutive vertices
	out << "#faces " << std::endl;
	for(long i=0; i < DDBB_OBJ.GetSizeFaces(); i++) {
		std::vector<long> vertices= DDBB_OBJ.GetAllVerticesInFace(DDBB_OBJ.GetFace(i));
		//Write vertices of the current face
		out << "f  ";
		for(long j=0; j<(long)vertices.size(); j++) {
			sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", 4, vertices[j]+1); //In the obj file first vertex is 1
			out << CadenaAux << "//";
			sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", digits(i+1), i+1); //Normal associated to the vertex: In the obj file first normal is 1
			out << CadenaAux << "  ";
		}
		out << std::endl;
	}

	out << "#endsolid"<< std::endl;

	// Closes the file
	out.close ();

	return true;
};