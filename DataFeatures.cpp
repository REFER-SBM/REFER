
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

#include "Refer.h"

#include "DataFeatures.h"

CDataFeatures::CDataFeatures()
//Parameters for Features
{
	//There is only one general parameters for features
		ReferApp.Fe.featuresSequence.clear();
		ReferApp.Fe.featuresSequence.push_back(RIB);
		ReferApp.Fe.featuresSequence.push_back(RAIL);
		ReferApp.Fe.featuresSequence.push_back(STEP);

	//Parameters for each type of feature are managed through their own data managers:
	//	DataRaislAndSlots
	//	DataRibsAndDividers
	//	DataStepsAndPockets
	//	DataChamfersAndRounds
};


CDataFeatures::~CDataFeatures(){
};
