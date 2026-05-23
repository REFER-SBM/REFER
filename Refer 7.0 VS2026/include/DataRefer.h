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

#include "DataVectorize.h"
#include "DataCues.h"
#include "DataFeatures.h"
#include "DataDirectInflation.h"
#include "DataInflation.h"
#include "DataRefine2D.h"
#include "DataRefine3D.h"
#include "DataTessellate3D.h"
#include "DataCSG.h"


#ifndef DATAREFER_VISITED
	#define DATAREFER_VISITED


	//GENERAL PARAMETERS
	//------------------
	class CDataRefer{
		public:
			//FLAGS TO CONTROL THE MAIN FLOW
			bool	bVectorize_Int;
			bool	bVectorize_Batch;
			bool	bRefine2D_OnLine;
			bool	bRefine2D_OffLine;

			bool	bDetectCuesAndFeatures;

			//FLAGS TO CONTROL INFLATION
			bool	bInflateBRep;
			bool	DirectInflation;
			bool	Optimize;
			bool	bRefine3D;
			bool	bTessellate3D;

			bool	bBuildModelCSG;

			int		MRU_Files;
			bool	bIGS_Type_143;
			bool	bIGS_NgonFaces;

			//FLAGS TO CONTROL CUES DETECTION
			bool	DetectCues;

			bool	DetectValences;
			bool	DetectParallelEdges;
			bool	DetectVanishingPoints;

			bool	DetectGraphs;
			bool	DetectPerimeter;
			bool	DetectLabels;
			bool	DetectMainAxes;
			bool	DetectFaces;
			bool	DetectTypology;
			bool	DetectHiddenEdges;
			bool	DetectSymmetry;

			bool	DetectLevels;

			//FLAGS TO CONTROL FEATURES DETECTION
			bool	DetectFeatures;

			bool	DetectRibsAndDividers;
			bool	DetectRailsAndSlots;
			bool	DetectStepsAndPockets;
			bool	DetectChamfersAndRounds;    //TBD ChamfersAndRounds

			//PARAMETER TO CONTROL TOOLBAR BUTTONS SIZE
			int		toolbarButtonsSize;

			//PARAMETERS TO CONTROL PATHS FOR REFER FILES
			CString	PathINI;
			CString	PathExamples;

			CDataRefer();
			~CDataRefer();
	};
#endif  //DATAREFER_VISITED
