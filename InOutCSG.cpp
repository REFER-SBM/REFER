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

#include "CueMainAxes.h"

#include "InOutCSG.h"

InOutCSG::InOutCSG()
{
};

InOutCSG::~InOutCSG()
{
};


void InOutCSG::writeDatumOrigins(std::ofstream &out, CDB_CSG *pDB_CSG_Model,
								 CDB_Model *DB_DatumGraph)
// Writes the data of each origin vertex for each subgraph
{
	char Aux0[20], Aux1[20], Aux2[20];

	//Loop over the origins of subgraphs
	long numOrigins= (long)pDB_CSG_Model->DatumOriginsSize();
	if(!ReferApp.CSG.CSG_UseMultipleOrigins){
		numOrigins= 1;
	}
	out << "      Datum Origins ="<< numOrigins << std::endl;

	std::vector<long> Origins= pDB_CSG_Model->GetDatumOrigins();
	std::vector<COrthoDistance> orthoDistances= pDB_CSG_Model->GetOrthoDistancesOrigins();

	for(long i= 0; i<numOrigins; i++){
		out << "         Datum Origin Idd       = "<< i << std::endl;
		out << "            Reference vertex    = " << Origins[i] <<std::endl;

		// Writes the coordinates
		CVertex ivertex= DB_DatumGraph->GetVertex(Origins[i]);
		sprintf_s(Aux0, sizeof Aux0/sizeof *Aux0, "%10f", ivertex.x);
		sprintf_s(Aux1, sizeof Aux1/sizeof *Aux1, "%10f", ivertex.y);
		//sprintf_s(Aux2, sizeof Aux2/sizeof *Aux2, "%10f", ivertex.z);
		out <<"                  O"<<i<<" ["<<Aux0<<","<<Aux1<<"]"<< std::endl;

		// Writes the orthogonal distance to main origin
		sprintf_s(Aux0, sizeof Aux0/sizeof *Aux0, "%10f", orthoDistances[i].x);
		sprintf_s(Aux1, sizeof Aux1/sizeof *Aux1, "%10f", orthoDistances[i].y);
		sprintf_s(Aux2, sizeof Aux2/sizeof *Aux2, "%10f", orthoDistances[i].z);
		out <<"                  Ortho-distances to O0 ["<<Aux0<<","<<Aux1<<","<<Aux2<<"]"<< std::endl;
	}
	out << "      End Datum Origins"<< std::endl;
}


void InOutCSG::writeDatumPlanes(std::ofstream &out, CDB_CSG *pDB_CSG_Model)
// Writes the data of each plane of the Datum planes list
{
	char Aux[20];

	//Loop over the datum planes
	long numPlanes= (long)pDB_CSG_Model->DatumPlanesSize();
	out << "      Datum Planes ="<< numPlanes << std::endl;
	for(long i= 0; i<numPlanes; i++){
		CDatumPlane Plane=pDB_CSG_Model->GetDatumPlane(i);
		out << "         Datum Plane Idd        = "<< i << std::endl;
		out << "            Reference subgraph  = " << Plane.GetSubGraph() <<std::endl;
		out << "            Reference origin    = " << Plane.GetOrigin() <<std::endl;

		double idist= Plane.GetNormalDistance();
		switch (Plane.GetOrientation()){
			case XY:{sprintf_s(Aux, sizeof Aux/sizeof *Aux, "%10f", idist);
					 out << "            Plane orientation   = XY" << std::endl;
					 out << "            Distance in Z       = " << Aux <<std::endl;} break;
			case XZ:{sprintf_s(Aux, sizeof Aux/sizeof *Aux, "%10f", idist);
					 out << "            Plane orientation   = XZ" << std::endl;
					 out << "            Distance in Y       = " << Aux <<std::endl;} break;
			case YZ:{sprintf_s(Aux, sizeof Aux/sizeof *Aux, "%10f", idist);
					 out << "            Plane orientation   = YZ" << std::endl;
					 out << "            Distance in X       = " << Aux <<std::endl;} break;
		}
	}
	out << "      End Datum Planes"<< std::endl;
}


void InOutCSG::writeCrossSection(std::ofstream &out, 
								CFeature Feature,
								CDB_CSG *pDB_CSG_Model,
								CDB_Model *DB_DatumGraph)
// Writes cross section of "Feature" in the coordinate system of the drawing
	//This is the projected cross section,
	//useful for visualization, but irrelevant for the output file!!
{
	//char Aux0[20], Aux1[20], Aux2[20];
	char Aux0[20], Aux1[20];

	long numVertices= (long)Feature.SizeCrossSection();
	out << "               Profile cross section (drawing coordinates)= " << numVertices << std::endl;
	out << "               {" << std::endl;
	// Loop over the strokes
	for (long i= 0; i<numVertices; i++){
		CVertex ivertex= DB_DatumGraph->GetVertex(Feature.GetCrossSectionVertex(i));
		// Writes the sequence of coordinates
		sprintf_s(Aux0, sizeof Aux0/sizeof *Aux0, "%10f", ivertex.x);
		sprintf_s(Aux1, sizeof Aux1/sizeof *Aux1, "%10f", ivertex.y);
		//sprintf_s(Aux2, sizeof Aux2/sizeof *Aux2, "%10f", ivertex.z);
		out <<"                  V"<<i<<" ["<<Aux0<<","<<Aux1<<"]"<< std::endl ;
	}
	out <<"               }" << std::endl;
};


void InOutCSG::writeFeatureCSG(std::ofstream &out,
							   long Branch,
							   CFeature Feature, 
							   CDB_CSG *pDB_CSG_Model,
							   CDB_Model *DB_DatumGraph)
// Writes Feature information into the ofstream out
{
	switch (Feature.GetType()){
		case RIB:    {out <<"               RIB: " << std::endl;
				      out <<"               Extrusion both sides       = YES" << std::endl;} break;
		case DIVIDER:{out <<"               DIVIDER: " << std::endl;
				      out <<"               Extrusion both sides       = YES" << std::endl;} break;
		case SLOT:   {out <<"               SLOT: " << std::endl;
				      out <<"               Extrusion both sides       = NO"  << std::endl;} break;
		case RAIL:   {out <<"               RAIL: " << std::endl;
				      out <<"               Extrusion both sides       = NO"  << std::endl;} break;
		case POCKET: {out <<"               POCKET: " << std::endl;
				      out <<"               Extrusion both sides       = NO"  << std::endl;} break;
		case STEP:   {out <<"               STEP: " << std::endl;
				      out <<"               Extrusion both sides       = NO"  << std::endl;} break;
		//case NODE:   {out <<"               NODE: " << std::endl;
		//		      out <<"               Extrusion both sides       = YES" << std::endl;} break;
		case CORE:   {out <<"               CORE: " << std::endl;
				      out <<"               Extrusion both sides       = YES" << std::endl;} break;
	}

	out << "               Subgraph                   = " << Feature.GetSubGraph() <<std::endl;
	out << "               Branch                     = " << Branch <<std::endl;
	out << "               Merit                      = "<< Feature.GetMerit() << std::endl;

	switch (Feature.GetProfileOrientation()){
		case XY:     {out<< "               Profile orientation        = XY" << std::endl;} break;
		case XZ:     {out<< "               Profile orientation        = XZ" << std::endl;} break;
		case YZ:     {out<< "               Profile orientation        = YZ" << std::endl;} break;
		case SLANTED:{out<< "               Profile orientation        = SLANTED" << std::endl;} break;
		case UNKNOWN:{out<< "               Profile orientation        = UNKNOWN" << std::endl;} break; 
	}

	//out << "               Rotation_angle             = " << Feature.GetRotationAngle() <<std::endl;

	long numVertices= (long)Feature.SizeParametricProfile();
	out << "               Profile vertices           = " << numVertices << std::endl;
	out << "               {" << std::endl;

	char Aux0[20], Aux1[20];

	// Loop over the strokes
	for (long i= 0; i<numVertices; i++){
		CVertex ivertex= Feature.GetParametricProfileVertex(i);
		// Writes the sequence of XY coordinates of the parametic profile
		sprintf_s(Aux0, sizeof Aux0/sizeof *Aux0, "%10f", ivertex.x);
		sprintf_s(Aux1, sizeof Aux1/sizeof *Aux1, "%10f", ivertex.y);
		out <<"                  V"<<i<<" ["<<Aux0<<","<<Aux1<<"]"<< std::endl ;
	}
	out <<"               }" << std::endl;

	//writeCrossSection(out, Feature, pDB_CSG_Model, DB_DatumGraph);

	switch (Feature.GetExtrusionDirection()){
		case 0:{out<< "               Extrusion direction        = X" << std::endl;} break; 
		case 1:{out<< "               Extrusion direction        = Y" << std::endl;} break;
		case 2:{out<< "               Extrusion direction        = Z" << std::endl;} break;
	}

	sprintf_s(Aux1, sizeof Aux1/sizeof *Aux1, "%10f", Feature.GetLengthExtrusion());
	out << "               Linear_extrusion length    =" << Aux1 <<std::endl;

	//Write datum planes that define the location of the feature
	long numDatums= (long)Feature.DatumIdSize();
	out << "               Datums referencing feature =" << numDatums <<std::endl;

	for (long i= 0; i<numDatums; i++){
		out << "                  Datum "<<i<<"= " << Feature.GetDatumId(i) <<std::endl;
	}
};


bool InOutCSG::writeCSG( const char *file, CDB_CSG *pDB_CSG_Model, CDB_Model *DB_DatumGraph)
//Writes the csg model into a CSG file

//Returns TRUE if the process is sucessful
{
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

	//Write the header of a CSG file
	//------------------------------
	out << "CSG model by REFER " << std::endl;

	//Write Datums
	//------------
	out << "   DATUMS "<< std::endl;
	writeDatumOrigins(out, pDB_CSG_Model, DB_DatumGraph);
	writeDatumPlanes(out, pDB_CSG_Model);
	out << "   END DATUMS "<< std::endl;

	//Write Tree
	//----------
	out << "   TREE "<< std::endl;

	//Loop over all the branches
	long numBranches= pDB_CSG_Model->TreeBranchesSize();
	out << "      Branches ="<< numBranches << std::endl;
	long iFeat= 0;
	for(long i= numBranches-1; i>=0; i--){
		//out << "         Branch "<<(numBranches-1-i)<<std::endl;
		out << "         Branch "<<(i)<<std::endl;

		//Loop over all the features in the current branch
		//(from contained to containers)
		long numFeatures= (long)pDB_CSG_Model->TreeBranchSize(i);
		for (long j= numFeatures-1; j>=0; j--){
			CFeature ijFeature= pDB_CSG_Model->GetFeature(i, j);
			//out << "            Feature "<<(numFeatInBranch-j-1)<<std::endl;
			out << "            Feature "<<(iFeat)<<std::endl;

			writeFeatureCSG(out, i, ijFeature, pDB_CSG_Model, DB_DatumGraph);

			iFeat++;
		}
		out << "         End Branch "<<(i)<<std::endl;
	}
	out << "      End Branches" << std::endl;
	out << "   END TREE "<< std::endl;

	//Write the closing header of a CSG file
	//--------------------------------------
	out << "End CSG model by REFER"<< std::endl;

	//Close the file
	out.close ();

	return true;
};
