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

#include "Tools_Geometry.h"

#include "DB_Model.h"
#include "DB_CSG.h"
#include "Refer.h"

#include "ReferDoc.h"

#include "CueManager.h"
#include "FeatureManager.h"

#include "DetectionManager.h"


CDetectionManager::CDetectionManager()
{
	//Set flags as configured though the interactive menu
	DetectCues = ReferApp.GeneralParams.DetectCues;
	DetectValences = ReferApp.GeneralParams.DetectValences;
	DetectParallelEdges = ReferApp.GeneralParams.DetectParallelEdges;
	DetectVanishingPoints = ReferApp.GeneralParams.DetectVanishingPoints;
	DetectGraphs = ReferApp.GeneralParams.DetectGraphs;
	DetectPerimeter = ReferApp.GeneralParams.DetectPerimeter;
	DetectLabels = ReferApp.GeneralParams.DetectLabels;
	DetectMainAxes = ReferApp.GeneralParams.DetectMainAxes;
	DetectFaces = ReferApp.GeneralParams.DetectFaces;
	DetectTypology = ReferApp.GeneralParams.DetectTypology;
	DetectHiddenEdges = ReferApp.GeneralParams.DetectHiddenEdges;
	DetectSymmetry = ReferApp.GeneralParams.DetectSymmetry;
	DetectLevels = ReferApp.GeneralParams.DetectLevels;

	DetectFeatures= ReferApp.GeneralParams.DetectFeatures;
	DetectRibsAndDividers = ReferApp.GeneralParams.DetectRibsAndDividers;
	DetectRailsAndSlots = ReferApp.GeneralParams.DetectRailsAndSlots;
	DetectStepsAndPockets = ReferApp.GeneralParams.DetectStepsAndPockets;
	DetectChamfersAndRounds = ReferApp.GeneralParams.DetectChamfersAndRounds;    //TBD ChamfersAndRounds
}


CDetectionManager::CDetectionManager(bool Set_DetectCues,
									bool Set_DetectValences,
									bool Set_DetectParallelEdges,
									bool Set_DetectVanishingPoints,
									bool Set_DetectGraphs,
									bool Set_DetectPerimeter,
									bool Set_DetectLabels,
									bool Set_DetectMainAxes,
									bool Set_DetectFaces,
									bool Set_DetectTypology,
									bool Set_DetectHiddenEdges,
									bool Set_DetectSymmetry,
									bool Set_DetectLevels,

									bool Set_DetectFeatures,
									bool Set_DetectRibsAndDividers,
									bool Set_DetectRailsAndSlots,
									bool Set_DetectStepsAndPockets,
									bool Set_DetectChamfersAndRounds)    //TBD ChamfersAndRounds
:
	DetectCues (Set_DetectCues),
	DetectValences (Set_DetectValences),
	DetectParallelEdges (Set_DetectParallelEdges),
	DetectVanishingPoints (Set_DetectVanishingPoints),
	DetectGraphs (Set_DetectGraphs),
	DetectPerimeter (Set_DetectPerimeter),
	DetectLabels (Set_DetectLabels),
	DetectMainAxes (Set_DetectMainAxes),
	DetectFaces (Set_DetectFaces),
	DetectTypology (Set_DetectTypology),
	DetectHiddenEdges (Set_DetectHiddenEdges),
	DetectSymmetry (Set_DetectSymmetry),
	DetectLevels (Set_DetectLevels),

	DetectFeatures (Set_DetectFeatures),
	DetectRibsAndDividers (Set_DetectRibsAndDividers),
	DetectRailsAndSlots (Set_DetectRailsAndSlots),
	DetectStepsAndPockets (Set_DetectStepsAndPockets),
	DetectChamfersAndRounds (Set_DetectChamfersAndRounds)    //TBD ChamfersAndRounds
{
}


CDetectionManager::~CDetectionManager()
{
}


void CDetectionManager::EraseCuesAndFeatures()
// Removes prevously calculated detections:
//		Cues
//		Features
{
	//DELETION OF 3D CUES
	//-------------------
	CCueManager C;
	C.EraseCues();

	//DELETION OF 3D FEATURES
	//-----------------------
	CFeatureManager F;
	F.EraseFeatures();
}


void CDetectionManager::DetectCuesAndFeatures(CDB_Model *DDBB)
// Manages the detection flow:
//		Cues
//		Features

//The cues and features that must be detected are governed by the set of flags
//managed by "CDataRefer" through "ReferApp.GeneralParams"
{
	//Reset previous cues and features
	//--------------------------------
	EraseCuesAndFeatures();

	//Detect 3D cues
	if(DetectCues){
		CCueManager C;

		C.FindCues(DDBB);
	}

	//Detect 3D features
	//------------------
	if(DetectFeatures){
		CFeatureManager F;

		F.FindFeatures(DDBB);
	}
}
