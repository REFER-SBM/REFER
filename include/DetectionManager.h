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

#if !defined DETECTIONMANAGER_DEFINED
#define DETECTIONMANAGER_DEFINED 

class CDetectionManager
{
	private:
		bool DetectCues,
			DetectValences,
			DetectParallelEdges,
			DetectVanishingPoints,
			DetectGraphs,
			DetectPerimeter,
			DetectLabels,
			DetectMainAxes,
			DetectFaces,
			DetectTypology,
			DetectHiddenEdges,
			DetectSymmetry,
			DetectLevels;

		bool DetectFeatures,
			DetectRibsAndDividers,
			DetectRailsAndSlots,
			DetectStepsAndPockets,
			DetectChamfersAndRounds;    //TBD ChamfersAndRounds

	public:
		CDetectionManager();
		CDetectionManager(bool DetectCues,
						bool DetectValences,
						bool DetectParallelEdges,
						bool DetectVanishingPoints,
						bool DetectGraphs,
						bool DetectPerimeter,
						bool DetectLabels,
						bool DetectMainAxes,
						bool DetectFaces,
						bool DetectTypology,
						bool DetectHiddenEdges,
						bool DetectSymmetry,
						bool DetectLevels,

						bool DetectFeatures,
						bool DetectRibsAndDividers,
						bool DetectRailsAndSlots,
						bool DetectStepsAndPockets,
						bool DetectChamfersAndRounds);    //TBD ChamfersAndRounds
		~CDetectionManager();

		void EraseCuesAndFeatures();
		void DetectCuesAndFeatures(CDB_Model *DDBB);
};

#endif // DETECTIONMANAGER_DEFINED
