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
#include <vector>

#include "Tools_Geometry.h"  //POINT2D
#include "DataRefer.h"

CDataRefer::CDataRefer()
//Parameters to control the 3D reconstruction process
{
	//FLAGS TO CONTROL THE MAIN STAGES IN 3D RECONSTRUCTION
	bVectorize_Int= false;
	bVectorize_Batch= true;
	bRefine2D_OnLine= false;
	bRefine2D_OffLine= true;

	bDetectCuesAndFeatures= true;

	bInflateBRep= true;
	DirectInflation= true;
	Optimize= false;
	bBuildModelCSG= true;
	bRefine3D= false;
	bTessellate3D= false;

	bBuildModelCSG= false;

	MRU_Files= 4;
	bIGS_Type_143= true;  //false for type 144
	bIGS_NgonFaces= true;  //false for triangular meshes

	//FLAGS TO CONTROL DETECTION OF CUES
	DetectCues = true;

	DetectValences= false;
	DetectParallelEdges= true;
	DetectVanishingPoints= false;

	DetectGraphs = true;
	DetectPerimeter = true;
	DetectLabels = false;
	DetectMainAxes=true;
	DetectFaces = false;
	DetectTypology = true;
	DetectSymmetry = false;

	DetectLevels = false;

	//FLAGS TO CONTROL DETECTION OF FEATURES
	DetectFeatures = true;

	DetectRibsAndDividers= false;
	DetectRailsAndSlots= false;
	DetectStepsAndPockets= false;
	DetectChamfersAndRounds= false;    //TBD ChamfersAndRounds

	//FLAGS TO CONTROL INFLATION
	DirectInflation= true;
	Optimize= true;

	//PARAMETER TO CONTROL TOOLBAR BUTTONS SIZE
	toolbarButtonsSize= 1;  //(0 default 24x22, >0 scale x1, x2, ...)

	//PARAMETERS TO CONTROL PATHS FOR REFER FILES
	PathINI= "C:\\REFER Data";
	PathExamples = "C:\\REFER Data\\Examples";
};


CDataRefer::~CDataRefer()
{
};
