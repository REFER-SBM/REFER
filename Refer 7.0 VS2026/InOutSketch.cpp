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
#include "DB_CSG.h"
#include "ReferDoc.h"
#include "resource.h"
#include "Refer.h"

#include "DB_Sketch.h"

#include "InOutSketch.h"


CInOutSketch::CInOutSketch()
{
};


CInOutSketch::~CInOutSketch()
{
};


bool CInOutSketch::readSketch(const char *file, CDB_Sketch *DB_Sketch)
// Reads "stk" type 'file's
//		Stores strokes in 'DB_Sketch'
//		Returns TRUE if ends normally (even if nothing known is found)
{
	// Opens the file
	std::ifstream in (file);

	if (!in.is_open ()) {
		AfxMessageBox("Failure opening file in InOutSketch::readSketch");
		return false;
	};

	if (!in.is_open ()) {
		AfxMessageBox("File cannot be opened to read");
		return false;
	};

	std::string linea;
	char aux[30];
	long numS, numP;

	// Checks heading and gets the number of strokes
	getline(in, linea);
	if (linea!= "REFER SKETCH") return false;
	getline(in, linea);
	//if (linea.compare(1,17,"number of strokes") == 0) return false;
	sscanf_s(linea.c_str(), "%s %d", &aux, (unsigned int)sizeof(aux), &numS);

	// Loop over the strokes stored in the file
	// ----------------------------------------
	float point_x, point_y, point_time;
	//long iistroke;
	for (long istroke= 0; istroke < numS; istroke++){
		getline(in, linea);
		//if (linea.compare(1,6,"STROKE") == 0) return false;
		//sscanf_s(linea.c_str(), "%s %d", aux, sizeof(aux), &iistroke);
		//if (iistroke != istroke) return false;
		getline(in, linea);
		//if (linea.compare(1,16,"number of points") == 0) return false;
		sscanf_s(linea.c_str(), "%s %d", aux, (unsigned int)sizeof(aux), &numP);

		//Loads the points of the current stroke
		CStroke Stroke;
		for (long ipoint= 0; ipoint < numP; ipoint++){
			getline(in, linea);
			sscanf_s(linea.c_str(), "%f %f %f", &point_x, &point_y, &point_time);

			CStrokePoint p(point_x, point_y, point_time);
			Stroke.AddPoint(p);
		}

		DB_Sketch->AddStroke(Stroke);
	}

	// Closes the file
	in.close();

	return true;
};

bool CInOutSketch::writeSketch(const char *file, CDB_Sketch *DB_Sketch)
// Writes the entities of the database 'DB_Sketch' into a "stk" file
// Returns TRUE if the process is sucessful

{
	char Aux1[20], Aux2[20], Aux3[20];

	// Opens the file "file"
	std::ofstream out (file);

	if(!out.is_open ()){
		CString sMensaje;
		sMensaje+="File ";
		sMensaje+=file;
		sMensaje+=" cannot be opened to write";
		AfxMessageBox(sMensaje);
		return false;
	};

	// Writes the header
	out << "REFER SKETCH" << std::endl;
	sprintf_s(Aux1, sizeof Aux1/sizeof *Aux1, "%5d",DB_Sketch->GetNumStrokes());
	out << "number_of_strokes " << Aux1 <<std::endl;

	// Loop over the strokes
	for (long istroke= 0; istroke<(long)DB_Sketch->GetNumStrokes(); istroke++){
		CStroke StrokeI= DB_Sketch->GetStroke(istroke);

		// Writes the header
		sprintf_s(Aux1, sizeof Aux1/sizeof *Aux1, "%5d",istroke);
		out << "STROKE " << Aux1 << std::endl;
		sprintf_s(Aux1, sizeof Aux1/sizeof *Aux1, "%5d",StrokeI.GetNumPoints());
		out << "number_of_points " << Aux1 << std::endl;

		// Writes the sequence of coordinates and times
		for (long ipoint=0; ipoint<StrokeI.GetNumPoints(); ipoint++){
			CStrokePoint PointI= StrokeI.GetPoint(ipoint);
			POINT2D c= PointI.GetPosition();
			double ctime= PointI.GetTime();
			sprintf_s(Aux1, sizeof Aux1/sizeof *Aux1, "%10f", c.x);
			sprintf_s(Aux2, sizeof Aux1/sizeof *Aux1, "%10f", c.y);
			sprintf_s(Aux3, sizeof Aux1/sizeof *Aux1, "%10f", ctime);
			out << Aux1 <<"  " << Aux2 <<"  " << Aux3 << std::endl;
		}
	}

	// Closes the file
	out.close ();

	return true;
};