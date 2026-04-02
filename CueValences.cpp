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
#include <cassert>
#include <vector>

#include "Tools_Geometry.h"

#include "DB_Model.h"
#include "DB_Model_Maths.h"
#include "DB_CSG.h"
#include "resource.h"
#include "Refer.h"

#include "ReferDoc.h"

#include "CueValences.h"


int CCueValences::GetSingleValence(CDB_Model *DDBB, long v)
//Calculates and returns the "valence" of vertex v:
//		- The valence is the number of edges connected to one vertex.
//		- Collinear edges are counted as single edges if flag
//		  CollinearAsSingle is active
{
	long valence= DDBB->GetSizeEdgesInVertex(v);

	//Detection of collinear edges
	//(as they are connected to a common vertex, 
	//being nearly parallel means being nearly collinear)
	if (ReferApp.Cu.Va.CollinearAsSingle){
		for (long j=0; j<DDBB->GetSizeEdgesInVertex(v); j++){
			for (long k=j+1; k<DDBB->GetSizeEdgesInVertex(v); k++){
				double meritParallel= NearlyParallel(DDBB,
													 DDBB->GetEdgeInVertex(v, j),
													 DDBB->GetEdgeInVertex(v, k),
													 DEGREES_TO_RADIANS(5));   //Parallelism threshold
				if (meritParallel > 0.){
					valence--;
				}
			}
		}
	}

	return valence;
}


void CCueValences::GetValences(CDB_Model *DDBB)
//Calculates and returns the "valences" of the vertices
//(The valence is the number of edges connected to one vertex)

//The value is calculated and stored only for display purposes
//(see "display valences of vertices")
//For other (multiple) purposes, the valences are recalculated whenever needed
//trough a call to the "GetSizeEdgesInVertex" operation of the DataBase.

//Recalculating is preferred since DataBase changes quite frequently.

//A dialog is launched to ask the user whether the 
//collinear edges must be counted as single edges

{
	for (long i=0; i<DDBB->GetSizeVertices(); i++)
		ReferApp.Cu.Va.Valences.push_back (GetSingleValence(DDBB, i));
	return;
}