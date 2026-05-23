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

#include "Tools_Geometry.h"

#include "DB_Model.h"
#include "Refer.h"

#include "DataLabels.h"
#include "CueLabelsVarley_Types.h"
#include "CueLabelsVarley_Constants.h"
#include "CueLabelsVarley_Object.h"
#include "CueLabelsVarley_LabellingConstants.h"
#include "CueLabelsVarley_LabellingParameters.h"
#include "CueLabelsVarley.h"

#include "CueLabels.h"


void CCueLabels::GetLabels(CDB_Model *DDBB)
//Get labels from a graph-like representation of a polyhedrical shape

//The implemented approach is based on:
	//Varley P.A.C.
	//Automatic Creation of Boundary-Representation Models 
	//from Single Line Drawings, PhD Thesis, University of Wales. 2003
{
	//Check if the apparent contour has been calculated
	if(ReferApp.Cu.Pe.PerimeterVertices.size() == 0){
		AfxMessageBox("No apparent contour,\r\n hence labelling edges cannot proceed");
		return;
	}

	//Initialization of vectors that contain the labels
	ReferApp.Cu.La.LabelsInEdges.assign(DDBB->GetSizeEdges(), BLANK);
	ReferApp.Cu.La.LabelsInVertices.assign(DDBB->GetSizeVertices(), JunctionIsUndefined);

	//Each subgraph is analyzed and labelled
	for(long NumGraph= 0; NumGraph<ReferApp.Cu.SG.NumSubgraphs; NumGraph++){
		CCueLabelsVarley LBV;
		LBV.Get_Labels_Varley(DDBB);
	}
}
