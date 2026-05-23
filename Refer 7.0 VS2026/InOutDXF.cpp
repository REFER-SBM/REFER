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
#include "Refer.h"
#include "resource.h"

#include "InOutDXF.h"

// Recognizable DXF codes 
#define COD_SECTION     0
#define COD_STYLE       6
#define COD_X1         10
#define COD_Y1         20
#define COD_Z1         30
#define COD_X2         11
#define COD_Y2         21
#define COD_Z2         31
#define COD_COLOR      62


void InOutDXF::TransformaAcadRGB( int iCode, long &R, long &G, long &B)
// Translates the color code from AutoCAD to RGB
{
	switch (iCode)
	{
		case 0 : R = G = B = 0;       /* black */
		         break;
		case 1 : R = 255;               /* red */
		         G = B = 0;
		         break;
		case 2 : R = G = 255;          /* yellow */
		         B = 0;
		         break;
		case 3 : G = 255;               /* green */
		         R = B = 0;
		         break;
		case 4 : B = G = 255;          /* cian */
		         R = 0;
		         break;
		case 5 : B = 255;               /* blue */
		         R = G = 0;
		         break;
		case 6 : B = R = 255;          /* magenta */
		         G = 0;
		         break;
		case 8 : R = G = B = 128;     /* gray */
		         break;
		case 9 : R = G = B = 192;     /* silver gray */
		         break;
		//case 7 : *R = *G = *B = 255;     /*  white */
		case 7 : R = G = B = 0;     // White switched to black
		         break;
		default : R = G = B = 0;    // Default is black
		          break;
	};
};

int InOutDXF::TransformaRGBAcad( int R, int G, int B)
// Translates the color code from RGB to AutoCAD
{
	if ((R == 0)   && (G == 0)   && (B == 0))    return 0;  /* negro */
	if ((R == 255) && (G == 0)   && (B == 0))    return 1;  /* rojo */
	if ((R == 255) && (G == 255) && (B == 0))    return 2;  /* amarillo */
	if ((R == 0)   && (G == 255) && (B == 0))    return 3;  /* verde */
	if ((R == 0)   && (G == 255) && (B == 255))  return 4;  /* cian */
	if ((R == 0)   && (G == 0)   && (B == 255))  return 5;  /* azul */
	if ((R == 255) && (G == 0)   && (B == 255))  return 6;  /* magenta */
	if ((R == 255) && (G == 255) && (B == 255))  return 7;  /* blanco */
	if ((R == 128) && (G == 128) && (B == 128))  return 8;  /* gris */
	if ((R == 192) && (G == 192) && (B == 192))  return 9;  /* gris plata */
	return 7;  // If the color is unknown, returns white
};

// public:

InOutDXF::InOutDXF()
{
};

InOutDXF::~InOutDXF()
{
};


bool InOutDXF::readDXF( const char *file, CDB_Model *pDDBB)
//  Reads DXF ASCII 'file'
//  Stores entities "LINE" in 'pDDBB'
//  Returns TRUE if ends normally (even if nothing known is found)
{

	//Opens the file
	std::ifstream in (file);

	if (!in.is_open ()) {
		AfxMessageBox("Cannot open the file to read");
		return false;
	};

	std::string linea, codigo, valor;

	//Skips contents previous to the tag "ENTITIES"
	while (getline(in, linea)){
		if (linea == "ENTITIES") break; //== if (linea.compare(1,8,"ENTITIES") == 0) break;
	}

	int iCode;
	//int iValor;
	bool newLine= false;
	double X1, Y1, Z1, X2, Y2, Z2;
	//long ColorR= 0;
	//long ColorG= 0;
	//long ColorB= 0;

	//Looks for LINEs inside section ENTITIES
	while (getline(in, codigo) && getline(in, valor)){
		sscanf_s(codigo.c_str(), "%3d", &iCode);

		switch(iCode) {
			case COD_SECTION:
				if (newLine) {
					// Copies the current entity into the database
					pDDBB->AddEdgeByVertices(X1, Y1, Z1, X2, Y2, Z2);
					////Change color
					//CEdge E= pDDBB->GetEdge(pDDBB->GetSizeEdges()-1);
					//E.ColorR= ColorR;
					//E.ColorG= ColorG;
					//E.ColorB= ColorB;
					//pDDBB->SetEdge(pDDBB->GetSizeEdges()-1, E);

					newLine= false;
				}

				if (valor == "ENDSEC" || valor == "EOF") break; // End of section or end of file
				if (valor == "LINE") newLine= true;
				break;

			//case COD_COLOR:
			//	sscanf_s(valor.c_str(), "%3d", &iValor);
			//	TransformaAcadRGB( iValor, ColorR, ColorG, ColorB);
			//	break;

			case COD_STYLE:
				if (valor == "SOLID")
					ReferApp.Cu.Hi.TypesOfEdges.push_back(VISIBLE);
				else if (valor == "DASHED")
					ReferApp.Cu.Hi.TypesOfEdges.push_back(HIDDEN);
				else 
					ReferApp.Cu.Hi.TypesOfEdges.push_back(UNDEFINED);  // Unknown styles are asigned UNDEFINED

				break;

			case COD_X1		:	X1 = atof(valor.c_str());		break;
			case COD_Y1		:	Y1 = atof(valor.c_str());		break;
			case COD_Z1		:	Z1 = atof(valor.c_str());		break;
			case COD_X2		:	X2 = atof(valor.c_str());		break;
			case COD_Y2		:	Y2 = atof(valor.c_str());		break;
			case COD_Z2		:	Z2 = atof(valor.c_str());		break;
			default			:	break;
		}; //switch
	}; //while

	//Closes the file
	in.close();

	return true;
};

bool InOutDXF::writeDXF( const char *file, CDB_Model *pDDBB)
// Writes the entities of the database 'pDDBB' into a DXF file
// Returns TRUE if the process is sucessful

// This version creates only the section ENTITIES
// which containes the entities LINE
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
	
	// Writes the header of a DXF file
	out << "999" << std::endl;
	out << "DXF created from <REFER>" << std::endl;

	out << "  0" << std::endl;
	out << "SECTION" << std::endl;

	out << "  2" << std::endl;
	out << "ENTITIES" << std::endl;

	//Writes all the edges contained in database pDDBB
	for(long i=0; i < pDDBB->GetSizeEdges(); i++) {
		// Obtains the edge "i" from database pDDBB
		CEdge Arista= pDDBB->GetEdge(i);

		// Writes the attributes of current edge
		out << "  0" << std::endl;  // COD_SECTION
		out << "LINE" << std::endl;

		out << " 62" << std::endl;  // COD_COLOR
		int iColor = TransformaRGBAcad( Arista.ColorR, Arista.ColorG, Arista.ColorB);
		sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%3d",iColor );
		out << CadenaAux << std::endl;

		out << "  6" << std::endl;  // COD_STYLE
		if(ReferApp.Cu.Hi.TypesOfEdges.size() == pDDBB->GetSizeEdges()){
			if(ReferApp.Cu.Hi.TypesOfEdges[i] == HIDDEN)
				out << "DASHED" << std::endl;
			else
				out << "SOLID" << std::endl;
		}
		else
			out << "SOLID" << std::endl;

		// Obtains vertices for edge "i"
		CVertex Vertice1= pDDBB->GetVertex(Arista.Head);
		CVertex Vertice2= pDDBB->GetVertex(Arista.Tail);

		// Writes the coordinates of its vertices
		out << " 10" << std::endl;  // COD_X1
		sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%f",Vertice1.x );
		out << CadenaAux << std::endl;

		out << " 20" << std::endl;  // COD_Y1
		sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%f",Vertice1.y );
		out << CadenaAux << std::endl;

		out << " 30" << std::endl;  // COD_Z1
		sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%f",Vertice1.z );
		out << CadenaAux << std::endl;

		out << " 11" << std::endl;  // COD_X2
		sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%f",Vertice2.x );
		out << CadenaAux << std::endl;

		out << " 21" << std::endl;  // COD_Y2
		sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%f",Vertice2.y );
		out << CadenaAux << std::endl;

		out << " 31" << std::endl;  // COD_Z2
		sprintf_s( CadenaAux, sizeof CadenaAux/sizeof *CadenaAux, "%f",Vertice2.z );
		out << CadenaAux << std::endl;
	};

	// Writes the closing header of section "ENTITIES"
	out << "  0"<< std::endl;
	out << "ENDSEC"<< std::endl;

	// Writes the closing header of a DXF file
	out << "  0"<< std::endl;
	out << "EOF"<< std::endl;

	// Closes the file
	out.close ();

	return true;
};