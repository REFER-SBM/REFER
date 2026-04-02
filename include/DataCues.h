
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

#include <vector>
#include "Tools_Geometry.h"

#include "DataValences.h"
#include "DataParallelEdges.h"
#include "DataVanishingPoints.h"

#include "DataSubGraphs.h"
#include "DataPerimeter.h"
#include "DataHiddenEdges.h"
#include "DataLabels.h"
#include "DataMainAxes.h"
#include "DataFaces.h"
#include "DataTypology.h"
#include "DataSymmetry.h"
#include "DataLevels.h"


#if !defined DATACUES_DEFINED
	#define DATACUES_DEFINED

	class CDataCues{

		public:
			//Parameters of valences
			CDataValences Va;

			//Parameters of parallel edges
			CDataParallelEdges PE;

			//Parameters of vanishing points
			CDataVanishingPoints VP;

			//Parameters of sub-graphs
			CDataSubGraphs SG;

			//Parameters of perimeters
			CDataPerimeter Pe;

			//Parameters of hidden edges
			CDataHiddenEdges Hi;

			//Parameters of labelling
			CDataLabels La;

			//Parameters of main axes
			CDataMainAxes MA;

			//Parameters to detect faces
			CDataFaces Fa;

			//Parameters to detect typology
			CDataTypology Ty;

			//Parameters to detect symmetry
			CDataSymmetry Sy;

			CDataCues();
			~CDataCues();
	};

#endif // DATACUES_DEFINED