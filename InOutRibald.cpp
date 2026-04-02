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

#include "Tools_Geometry.h"

#include "DB_Model.h"
#include "resource.h"

#include "InOutRibald.h"

InOutRibald::InOutRibald()
{
};

InOutRibald::~InOutRibald()
{
};


bool InOutRibald::readRibald( const char *file, CDB_Model *pDDBB)
//  Reads Ribald files and stores the natural drawing in the database

//  Returns TRUE if ends normally (even if nothing known is found)
{

	//Opens the file
	std::ifstream in (file);

	if (!in.is_open ()) {
		AfxMessageBox("Cannot open the file to read");
		return false;
	};

	std::string linea;

	//Reads Vertices and Edges
	while (getline(in, linea)){
		if(linea.substr(0, 6)== "Vertex"){
			CVertex Vertice;
			long BeginX= linea.find(':')+1;
			long EndX= linea.find(',', BeginX);
			Vertice.x= atof(linea.substr(BeginX, EndX-BeginX).c_str());
			//sscanf(linea.substr(BeginX, EndX-BeginX).c_str(), "%f", &Vertice.x);

			long BeginY= EndX+1;
			while(linea.substr(BeginY, 1) == " ")
				BeginY++;
			long EndY= linea.find(' ', BeginY);
			if(EndY<0)
				EndY= linea.size()-1;
			Vertice.y= -atof(linea.substr(BeginY, EndY-BeginY).c_str());
			//sscanf(linea.substr(BeginY, EndY-BeginY).c_str(), "%f", &Vertice.y);
			pDDBB->AddVertex(Vertice);
		}
		else if(linea.substr(0, 4)== "Line" || linea.substr(0, 4)== "Edge"){
			CEdge E;

			long BeginV1 = linea.find('V')+1;
			long EndV1 = linea.find(' ', BeginV1);
			sscanf_s(linea.substr(BeginV1, EndV1-BeginV1).c_str(), "%d", &E.Tail);

			long BeginV2 = linea.rfind('V')+1;
			long EndV2 = linea.size();
			sscanf_s(linea.substr(BeginV2, EndV2-BeginV2).c_str(), "%d", &E.Head);

			pDDBB->AddEdge(E);
		}
	} //while

	// Closes the file
	in.close();

	return true;
};


int InOutRibald::digits(long number)
//returns the number of digits of number
{
	int digits= 0;
	do {number/=10; digits++;} while(number!=0);
	return digits;
}


bool InOutRibald::writeRibald( const char *file, CDB_Model *pDDBB)
// Writes the entities of the database 'pDDBB' into a Ribald file
// Returns TRUE if the process is sucessful
// This version creates only the section ENTITIES and the entities LINE
// Atributes of lines are ignored

{
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

	//Writes all the vertices contained in database pDDBB
	for(long i=0; i < pDDBB->GetSizeVertices(); i++) {
		CVertex Vertice= pDDBB->GetVertex(i);
		out << "Vertex ";
		sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", digits(i), i);

		out << CadenaAux << ": ";
		sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%15f",Vertice.x);
		out << CadenaAux << ",";
		sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%15f",-Vertice.y);

		//Writes the "degree" or "valence" of the vertex (number of edges connected to it)
		out << CadenaAux << " D: ";
		sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%3d",pDDBB->GetEdgeInVertex(i, 0));
		out << CadenaAux << std::endl;
	};

	//Writes all the edges contained in database pDDBB
	for(long i=0; i < pDDBB->GetSizeEdges(); i++) {
		CEdge Arista= pDDBB->GetEdge(i);
		out << "Line ";
		sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", digits(i), i);

		out << CadenaAux << ": V";
		sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", digits(Arista.Head), Arista.Head);
		out << CadenaAux << " to V";

		sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d",digits(Arista.Tail), Arista.Tail);
		out << CadenaAux << std::endl;
	};

	//Writes all the faces contained in database pDDBB
	for(long i=0; i < pDDBB->GetSizeFaces(); i++) {
		CFace Cara= pDDBB->GetFace(i);
		out << "Face ";
		sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d",digits(i), i);

		out << CadenaAux << ": ";
		long Size= Cara.GetSizeEdgesInFace();
		sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", digits(Size), Size);

		for (long j=0; j<Size; j++){
			out << CadenaAux << " L";
			long edge= Cara.GetEdgeInFace(j);
			sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%*d", digits(edge), edge);
		}

		out << CadenaAux << std::endl;
	};

	// Closes the file
	out.close ();

	return true;
};