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

#include "InOutSTEP.h"

CInOutSTEP::CInOutSTEP()
{
};

CInOutSTEP::~CInOutSTEP()
{
};


bool CInOutSTEP::WriteHeader(const char *file)
// Writes the entities of the database 'DB_CSG' into a "STEP" file
// Returns TRUE if the process is sucessful
{

	// Opens the file "file"
	std::ofstream out(file);

	if(!out.is_open ()){
		CString sMensaje;
		sMensaje+="File ";
		sMensaje+=file;
		sMensaje+=" cannot be opened to write";
		AfxMessageBox(sMensaje);
		return false;
	};

	out << "\n" << "ISO-10303-21;"<< std::endl;
	out << "HEADER;" << "\n\n";
	out << "FILE_DESCRIPTION(('STEP file'), 2;1);" << "\n\n";
	out << "FILE_NAME(";
	out << "'output_step_file', ";//file name
	out << "(add time), "; //data and time
	out << "'P. Company', "; //authors
	out << "'Regeo Group', "; //organization
	out << "(''), "; //preprocessor_version
	out << "'Example STEP file', "; //originating_system
	out << "''); " << "\n\n";
	out << "FILE_SCHEMA (('REGEO_DESIGN')); " << std::endl;
	out << "ENDSEC;" << "\n\n";
	out << "DATA; " << std::endl;	

	out.close();
		
	return true;
};


bool CInOutSTEP::WriteSTEPFile(CDB_CSG *pDB_CSG_Model)
// Writes the CSG model tree into a "STEP" file
// Returns TRUE if the process is sucessful
{

	// Opens the file "file"
	std::ofstream out("A_STEP_file.stp", std::ios::app);

	if(!out.is_open ()){
		CString sMensaje;
		sMensaje+="File ";
		//sMensaje+=file;
		sMensaje+="A_STEP_file.stp";
		sMensaje+=" cannot be opened to write";
		AfxMessageBox(sMensaje);
		return false;
	};

	//First, the file header is added to the STEP file
	std::string file= "A_STEP_file.stp";   //Converts the Name of the file into a string
	WriteHeader(file.c_str());

	out << "prueba; " << std::endl;

	out << "\n\n" << "ENDSEC;" << std::endl;
	out << "END-ISO-10303-21;" << std::endl;

	out.close ();

	return true;
};
