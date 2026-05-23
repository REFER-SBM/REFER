
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

//#include <vector>

#include "DB_Model.h"
#include "DB_CSG.h"

#include "DataRibsAndDividers.h"
#include "DataRailsAndSlots.h"
#include "DataStepsAndPockets.h"
#include "DataChamfersAndRounds.h"    //TBD ChamfersAndRounds


#if !defined DATAFEATURES_DEFINED
	#define DATAFEATURES_DEFINED

	class CDataFeatures{
		public:
			//Vector to store de sequence to follow while detecting features
			std::vector <FEATURE_TYPE> featuresSequence;

			CDataRibsAndDividers RD;
			CDataRailsAndSlots RS;
			CDataStepsAndPockets SP;
			CDataChamfersAndRounds ChR;    //TBD ChamfersAndRounds

			CDataFeatures();
			~CDataFeatures();
	};

#endif // DATAFEATURES_DEFINED